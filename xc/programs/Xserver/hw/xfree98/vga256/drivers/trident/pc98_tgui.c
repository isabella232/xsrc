/* $XFree86: xc/programs/Xserver/hw/xfree98/vga256/drivers/trident/pc98_tgui.c,v 3.3 1996/09/29 13:47:45 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "screenint.h"
#include "dix.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "vga.h"
#include "t89_driver.h"
#include "vgaPCI.h"

#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "extnsionst.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#ifdef XF86VGA16
#define MONOVGA
#endif

#ifndef MONOVGA
#include "tgui_drv.h"
#include "vga256.h"
extern vgaHWCursorRec vgaHWCursor;
#endif

#include <unistd.h>

#include "pc98_tgui.h"

pointer mmioBase = NULL;
pointer pc98PvramBase = NULL;
static int hsync31;
PC98TGUiTable *pc98TGUi;

Bool BoardInit(void);
void crtswNECGen(short);
void crtswTGUiGen(short);
void crtswNEC9680(short);
void crtswNEC9320(short);
void crtswDRV9680(short);
Bool testTRUE();
Bool testDRV();

static unsigned char seqreg_data[ 0x05 ] = {
	0x03, 0x31, 0x0f, 0x00, 0x0e
};

static unsigned char grctrl_data[ 0x09 ] = {
	0x0f, 0x0f, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0f, 0xff
};

static unsigned char vgareg_data[ 0x20 ] = {
	0x68, 0x4f, 0x50, 0x12, 0x53, 0x17, 0x06, 0x2e, /* 00 - 07 */
	0x00, 0x0f, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, /* 08 - 0F */
	0xec, 0x23, 0xdf, 0x00, 0x40, 0xe9, 0xed, 0xc0, /* 10 - 17 */
	0xff, 0x4a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, /* 18 - 1F */
};

static PC98TGUiTable pc98TGUiTab[]={
  {PC98NEC9680, PC98PCIBus, 0x20000000, 0x20400000
     , 45, 0x00af, {108000, 58500, 0, 31500}
   , crtswNEC9680, testTRUE}
  ,{PC98NEC9320, PC98PCIBus, 0xffc00000, 0xffe00000
      , 45, 0x00af, {108000, 58500, 0, 25175}
    , crtswNEC9320, testTRUE}
  ,{PC98DRV9680, PC98CBus,   0x00f20000, 0x00f00000
      , 45, 0x00af, {108000, 58500, 0, 25175}
    , crtswDRV9680, testDRV}
  ,{PC98NoExist, PC98Unknown, 0, 0
      , 0, 0, {0, 0, 0, 0}
    , NULL, NULL}};

Bool ChipInit(void);
void VideoEnable(void);
void CRTCwrite(unsigned char,unsigned char);
unsigned char CRTCread(unsigned char);
void SYNCDACwrite(unsigned char,unsigned char);
unsigned char SYNCDACread(unsigned char);
void SEQwrite(unsigned char,unsigned char);
unsigned char SEQread(unsigned char);
void GCwrite(unsigned char,unsigned char);
unsigned char GCread(unsigned char);
void sw_new(void);
void sw_old(void);
void reg_lock(void);
void reg_unlock(void);
static void SetRegisters( unsigned char *,unsigned char *,unsigned char *);

Bool BoardInit(void)
{
  int i;
  /* Save current horizontal sync, 1: 31.5KHz */
  hsync31 = _inb(0x9a8) & 0x01;

  for(i=0;pc98TGUiTab[i].TGUiType != PC98NoExist;i++){
    switch(pc98TGUiTab[i].BusType){
    case PC98PCIBus: /* Serach mmioBase on PCI Bus */ 
      if(vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_TRIDENT
	 && vgaPCIInfo->MemBase == pc98TGUiTab[i].vramBase){
	mmioBase  = xf86MapVidMem(0, VGA_REGION,
				  (pointer)(pc98TGUiTab[i].mmioBase), 0x10000);
	if(!pc98TGUiTab[i].chiptest()){
	  xf86UnMapVidMem(0, VGA_REGION, mmioBase, 0x10000);
	  mmioBase = NULL;
	}
      }
      break;
    case PC98CBus:  /* Serach mmioBase on C Bus */ 
      mmioBase  = xf86MapVidMem(0, VGA_REGION,
				(pointer)(pc98TGUiTab[i].mmioBase), 0x10000);
      if(!pc98TGUiTab[i].chiptest()){
	xf86UnMapVidMem(0, VGA_REGION, mmioBase, 0x10000);
	mmioBase = NULL;
      }
      break;
    }
    if(mmioBase != NULL)break;
  }

  pc98TGUi = &pc98TGUiTab[i];

  if(pc98TGUi->TGUiType != PC98NoExist){
    switch(pc98TGUi->BusType){
    case PC98PCIBus:
      ErrorF("%s %s: PC98: Found Trident MMIO port on PCI Bus @ 0x%08X\n",
	     XCONFIG_PROBED, vga256InfoRec.name, pc98TGUi->mmioBase);
      break;
    case PC98CBus:
      pc98PvramBase = (pointer)(pc98TGUi->vramBase);
      ErrorF("%s %s: PC98: Found Trident MMIO port on C Bus @ 0x%08X\n",
	     XCONFIG_PROBED, vga256InfoRec.name, pc98TGUi->mmioBase);
      break;
    }
    ChipInit();
  } else {
    FatalError("PC98: Not found Trident MMIO port\n");
  }

  return TRUE;
}

Bool testTRUE()
{
  return TRUE;
}

Bool testDRV()
{
  _outw(0x52e8,0x00ff);
  _outw(0x56e8,0x6fa1);
  _outw(0x5ae8,0x0000);
  /* insert chip test code here */

  return TRUE;
}

void crtswNECGen(short crtmod)
{
  if(crtmod != 0){
    _outb(0x68, 0x0e);
    _outb(0x6a, 0x07);
    _outb(0x6a, 0x8f);
    _outb(0x6a, 0x06);
    if(hsync31 == 0) _outb(0x9a8, 0x01); /* 24.8KHz -> 31.5KHz */
  } else {
    if(hsync31 == 0) _outb(0x9a8, 0x00); /* 31.5KHz-> 24.8KHz */
    _outb(0x6a, 0x07);
    _outb(0x6a, 0x8e);
    _outb(0x6a, 0x06);
    _outb(0x68, 0x0f);
  }
  return;
}

void crtswTGUiGen(short crtmod)
{
  if(crtmod != 0){
    CRTCwrite(0x23,0xdf & CRTCread(0x23));
    CRTCwrite(0x29,0x04 | CRTCread(0x29));

    SYNCDACwrite(0x04, 0x06 | SYNCDACread(0x04));
    usleep(1000);
    SYNCDACwrite(0x04, 0x08 | SYNCDACread(0x04));
    GCwrite(0x23, ~0x03 & GCread(0x23));
    SYNCDACwrite(0x04, 0x01 | SYNCDACread(0x04));
    SEQwrite(0x01, ~0x10 & SEQread(0x01));
  } else {
    SEQwrite(0x01, 0x10 | SEQread(0x01));
    SYNCDACwrite(0x04, ~0x01 & SYNCDACread(0x04));
    GCwrite(0x23, 0x01 | (~0x03 & GCread(0x23)));
    SYNCDACwrite(0x04, ~0x02 & SYNCDACread(0x04));
    SYNCDACwrite(0x04, ~0x30 & SYNCDACread(0x04));
    SYNCDACwrite(0x04, ~0x08 & SYNCDACread(0x04));
    SYNCDACwrite(0x04, ~0x04 & SYNCDACread(0x04));

    CRTCwrite(0x29,~0x04 & CRTCread(0x29));
    CRTCwrite(0x23,~0xdf | CRTCread(0x23));
  }
  return;
}

void crtswNEC9680(short crtmod)
{
  if(crtmod != 0){
    crtswNECGen(crtmod);
    _outb(0xfac, 0x02);
    crtswTGUiGen(crtmod);
  } else {
    crtswTGUiGen(crtmod);
    _outb(0xfac, 0x00);
    crtswNECGen(crtmod);
  }
  return;
}

void crtswNEC9320(short crtmod)
{
  if( crtmod != 0 ){
    crtswNECGen(crtmod);
    _outb(0xfaa, 0x03);
    _outb(0xfab, 0xff);
    reg_unlock();
    GCwrite(0x30,0x02 | GCread(0x30));
    reg_lock();
    crtswTGUiGen(crtmod);
  } else {
    crtswTGUiGen(crtmod);
    reg_unlock();
    GCwrite(0x30,~0x02 & GCread(0x30));
    reg_lock();
    _outb(0xfaa, 0x03);
    _outb(0xfab, 0xfd);
    crtswNECGen(crtmod);
  }
  return;
}

void crtswDRV9680(short crtmod)
{
  if(crtmod != 0){
    crtswNECGen(crtmod);
    _outw(0x5ee8, 0xdb30);
    crtswTGUiGen(crtmod);
  } else {
    crtswTGUiGen(crtmod);
    _outw(0x5ee8, 0x5b30);
    crtswNECGen(crtmod);
  }
  return;
}

/*
 * Trident Chip Init
 */
Bool ChipInit(void)
{
  int tmp;
  
  VideoEnable();

  outb(0x3c2, 0xc3);

  inb(0x3da);
  outb(0x3c0,0x10);
  outb(0x3c0,0x41);
  
  reg_unlock();
  
  SYNCDACwrite(0x00,0x01);
  GCwrite(0x2f,0x80);

  SetRegisters(seqreg_data,grctrl_data,vgareg_data);
  
  CRTCwrite(0x2f,0x07);
  CRTCwrite(0x33,0x01);
  CRTCwrite(0x3b,0x21);
  CRTCwrite(0x3c,0x00);

  reg_lock();
  return TRUE;
}

/* Video Sub System Enable */
void VideoEnable(void)
{
  int tmp;
  sw_old();

  tmp=SEQread(0x0e);
  SEQwrite(0x0e,(tmp | 0x20)); /* select Configuration port 1 */ 

  if((SEQread(0x0c) & 0x10)==0x10){
    SEQwrite(0x0e,tmp);
    outb(0x94,0x00);
    outb(0x102,0x01);
    outb(0x94,0x20);
    tmp=inb(0x3c3);
    outb(0x3c3,tmp | 0x01);
  }else{
    SEQwrite(0x0e,tmp);
    outb(0x46e8,0x10);
    outb(0x102,0x01);
    outb(0x46e8,0x08);
  }
}

static void SetRegisters( unsigned char *seqreg,unsigned char *grctrl, unsigned char *vgareg)
{
  int i;

  /* Sequence Registers */
  for( i=0; i<=0x04; i++ ) SEQwrite(i,seqreg[i]);

  sw_old();
  SEQwrite(0x0d,0x20);
  sw_new();
  SEQwrite(0x0d,0x00);
  
  /* Set Graphic Control Data */
  for( i=0; i<=0x08; i++ ) GCwrite(i,grctrl[i]);
  
  /* Set VGA CRTC Registers */
  CRTCwrite(0x11,(CRTCread(0x11) & 0x7f));
  for( i=0; i<=0x18; i++ ) CRTCwrite(i,vgareg[i]);

  return;
}

/* IO Methods */
/* VGA CRTC Registers */
void CRTCwrite( unsigned char Index ,unsigned char Data )
{
  outb(0x3d4,Index);
  outb(0x3d5,Data);
  return;
}
unsigned char CRTCread( unsigned char Index )
{
  outb(0x3d4,Index);
  return inb(0x3d5);
}
/* SYNCDAC Registers */
void SYNCDACwrite( unsigned char Index ,unsigned char Data )
{
  outb(0x83c8,Index);
  outb(0x83c6,Data);
  return;
}
unsigned char SYNCDACread( unsigned char Index )
{
  outb(0x83c8,Index);
  return inb(0x83c6);
}
/* Sequencer Registers */
void SEQwrite( unsigned char Index ,unsigned char Data )
{
  outb(0x3c4,Index);
  outb(0x3c5,Data);
  return;
}
unsigned char SEQread( unsigned char Index )
{
  outb(0x3c4,Index);
  return inb(0x3c5);
}
/* Graphics Controller Registers */
void GCwrite( unsigned char Index ,unsigned char Data )
{
  outb(0x3ce,Index);
  outb(0x3cf,Data);
  return;
}
unsigned char GCread( unsigned char Index )
{
  outb(0x3ce,Index);
  return inb(0x3cf);
}
/* Register set switch */
void sw_new(void)
{
  SEQread(0x0b);
  return;
}
void sw_old(void)
{
  unsigned char tmp;
  tmp=SEQread(0x0b);
  SEQwrite(0x0b,tmp);
  return;
}
/* Register Lock/Unlock */
void reg_lock(void)
{
  sw_new();
  SEQwrite(0x0e,~0x80 & SEQread(0x0e));
  return;
}
void reg_unlock(void)
{
  sw_new();
  SEQwrite(0x0e,0x80 | SEQread(0x0e));
  return;
}
