/***********************************************************
Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <string.h>

#include <X11/X.h>
#include <X11/Xproto.h>
#include "misc.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "gcstruct.h"
#include "dixstruct.h"
#include "resource.h"
#include "opaque.h"

#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvproto.h>
#include "xvdix.h"
#ifdef MITSHM
#include <X11/extensions/shmproto.h>
#endif

#include "xvdisp.h"

#ifdef PANORAMIX
#include "panoramiX.h"
#include "panoramiXsrv.h"

unsigned long XvXRTPort;
#endif

static int
SWriteQueryExtensionReply(
   ClientPtr client,
   xvQueryExtensionReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);
  swaps(&rep->version, n);
  swaps(&rep->revision, n);
  
  (void)WriteToClient(client, sz_xvQueryExtensionReply, (char *)rep);

  return Success;
}

static int
SWriteQueryAdaptorsReply(
   ClientPtr client,
   xvQueryAdaptorsReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);
  swaps(&rep->num_adaptors, n);
  
  (void)WriteToClient(client, sz_xvQueryAdaptorsReply, (char *)rep);

  return Success;
}

static int
SWriteQueryEncodingsReply(
   ClientPtr client,
   xvQueryEncodingsReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);
  swaps(&rep->num_encodings, n);
  
  (void)WriteToClient(client, sz_xvQueryEncodingsReply, (char *)rep);

  return Success;
}

static int
SWriteAdaptorInfo(
   ClientPtr client,
   xvAdaptorInfo *pAdaptor
){
  char n;

  swapl(&pAdaptor->base_id, n);
  swaps(&pAdaptor->name_size, n);
  swaps(&pAdaptor->num_ports, n);
  swaps(&pAdaptor->num_formats, n);

  (void)WriteToClient(client, sz_xvAdaptorInfo, (char *)pAdaptor);

  return Success;
}

static int
SWriteEncodingInfo(
   ClientPtr client,
   xvEncodingInfo *pEncoding
){
  char n;
  
  swapl(&pEncoding->encoding, n);
  swaps(&pEncoding->name_size, n);
  swaps(&pEncoding->width, n);
  swaps(&pEncoding->height, n);
  swapl(&pEncoding->rate.numerator, n);
  swapl(&pEncoding->rate.denominator, n);
  (void)WriteToClient(client, sz_xvEncodingInfo, (char *)pEncoding);

  return Success;
}

static int
SWriteFormat(
   ClientPtr client,
   xvFormat *pFormat
){
  char n;

  swapl(&pFormat->visual, n);
  (void)WriteToClient(client, sz_xvFormat, (char *)pFormat);

  return Success;
}

static int
SWriteAttributeInfo(
   ClientPtr client,
   xvAttributeInfo *pAtt
){
  char n;

  swapl(&pAtt->flags, n);
  swapl(&pAtt->size, n);
  swapl(&pAtt->min, n);
  swapl(&pAtt->max, n);
  (void)WriteToClient(client, sz_xvAttributeInfo, (char *)pAtt);

  return Success;
}

static int
SWriteImageFormatInfo(
   ClientPtr client,
   xvImageFormatInfo *pImage
){
  char n;

  swapl(&pImage->id, n);
  swapl(&pImage->red_mask, n);
  swapl(&pImage->green_mask, n);
  swapl(&pImage->blue_mask, n);
  swapl(&pImage->y_sample_bits, n);
  swapl(&pImage->u_sample_bits, n);
  swapl(&pImage->v_sample_bits, n);
  swapl(&pImage->horz_y_period, n);
  swapl(&pImage->horz_u_period, n);
  swapl(&pImage->horz_v_period, n);
  swapl(&pImage->vert_y_period, n);
  swapl(&pImage->vert_u_period, n);
  swapl(&pImage->vert_v_period, n);

  (void)WriteToClient(client, sz_xvImageFormatInfo, (char *)pImage);

  return Success;
}

static int
SWriteGrabPortReply(
   ClientPtr client,
   xvGrabPortReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);

  (void)WriteToClient(client, sz_xvGrabPortReply, (char *)rep);

  return Success;
}

static int
SWriteGetPortAttributeReply(
   ClientPtr client,
   xvGetPortAttributeReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);
  swapl(&rep->value, n);

  (void)WriteToClient(client, sz_xvGetPortAttributeReply, (char *)rep);

  return Success;
}

static int
SWriteQueryBestSizeReply(
   ClientPtr client,
   xvQueryBestSizeReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);
  swaps(&rep->actual_width, n);
  swaps(&rep->actual_height, n);

  (void)WriteToClient(client, sz_xvQueryBestSizeReply, (char *)rep);

  return Success;
}

static int
SWriteQueryPortAttributesReply(
   ClientPtr client,
   xvQueryPortAttributesReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);
  swapl(&rep->num_attributes, n);
  swapl(&rep->text_size, n);

  (void)WriteToClient(client, sz_xvQueryPortAttributesReply, (char *)rep);

  return Success;
}

static int
SWriteQueryImageAttributesReply(
   ClientPtr client,
   xvQueryImageAttributesReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);
  swapl(&rep->num_planes, n);
  swapl(&rep->data_size, n);
  swaps(&rep->width, n);
  swaps(&rep->height, n);

  (void)WriteToClient(client, sz_xvQueryImageAttributesReply, (char *)rep);

  return Success;
}

static int
SWriteListImageFormatsReply(
   ClientPtr client,
   xvListImageFormatsReply *rep
){
  char n;

  swaps(&rep->sequenceNumber, n);
  swapl(&rep->length, n);
  swapl(&rep->num_formats, n);

  (void)WriteToClient(client, sz_xvListImageFormatsReply, (char *)rep);

  return Success;
}

#define _WriteQueryAdaptorsReply(_c,_d) \
  if ((_c)->swapped) SWriteQueryAdaptorsReply(_c, _d); \
  else WriteToClient(_c, sz_xvQueryAdaptorsReply, (char*)_d)

#define _WriteQueryExtensionReply(_c,_d) \
  if ((_c)->swapped) SWriteQueryExtensionReply(_c, _d); \
  else WriteToClient(_c, sz_xvQueryExtensionReply, (char*)_d)

#define _WriteQueryEncodingsReply(_c,_d) \
  if ((_c)->swapped) SWriteQueryEncodingsReply(_c, _d); \
  else WriteToClient(_c, sz_xvQueryEncodingsReply, (char*)_d)

#define _WriteAdaptorInfo(_c,_d) \
  if ((_c)->swapped) SWriteAdaptorInfo(_c, _d); \
  else WriteToClient(_c, sz_xvAdaptorInfo, (char*)_d)

#define _WriteAttributeInfo(_c,_d) \
  if ((_c)->swapped) SWriteAttributeInfo(_c, _d); \
  else WriteToClient(_c, sz_xvAttributeInfo, (char*)_d)

#define _WriteEncodingInfo(_c,_d) \
  if ((_c)->swapped) SWriteEncodingInfo(_c, _d); \
  else WriteToClient(_c, sz_xvEncodingInfo, (char*)_d)

#define _WriteFormat(_c,_d) \
  if ((_c)->swapped) SWriteFormat(_c, _d); \
  else WriteToClient(_c, sz_xvFormat, (char*)_d)

#define _WriteGrabPortReply(_c,_d) \
  if ((_c)->swapped) SWriteGrabPortReply(_c, _d); \
  else WriteToClient(_c, sz_xvGrabPortReply, (char*)_d)

#define _WriteGetPortAttributeReply(_c,_d) \
  if ((_c)->swapped) SWriteGetPortAttributeReply(_c, _d); \
  else WriteToClient(_c, sz_xvGetPortAttributeReply, (char*)_d)

#define _WriteQueryBestSizeReply(_c,_d) \
  if ((_c)->swapped) SWriteQueryBestSizeReply(_c, _d); \
  else WriteToClient(_c, sz_xvQueryBestSizeReply,(char*) _d)

#define _WriteQueryPortAttributesReply(_c,_d) \
  if ((_c)->swapped) SWriteQueryPortAttributesReply(_c, _d); \
  else WriteToClient(_c, sz_xvQueryPortAttributesReply,(char*) _d)

#define _WriteQueryImageAttributesReply(_c,_d) \
  if ((_c)->swapped) SWriteQueryImageAttributesReply(_c, _d); \
  else WriteToClient(_c, sz_xvQueryImageAttributesReply,(char*) _d)

#define _WriteListImageFormatsReply(_c,_d) \
  if ((_c)->swapped) SWriteListImageFormatsReply(_c, _d); \
  else WriteToClient(_c, sz_xvListImageFormatsReply,(char*) _d)

#define _WriteImageFormatInfo(_c,_d) \
  if ((_c)->swapped) SWriteImageFormatInfo(_c, _d); \
  else WriteToClient(_c, sz_xvImageFormatInfo, (char*)_d)

#define _AllocatePort(_i,_p) \
  ((_p)->id != _i) ? (* (_p)->pAdaptor->ddAllocatePort)(_i,_p,&_p) : Success

static int
ProcXvQueryExtension(ClientPtr client)
{
  xvQueryExtensionReply rep;
  /* REQUEST(xvQueryExtensionReq); */
  REQUEST_SIZE_MATCH(xvQueryExtensionReq);

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.length = 0;
  rep.version = XvVersion;
  rep.revision = XvRevision;

  _WriteQueryExtensionReply(client, &rep);

  return Success;
}

static int
ProcXvQueryAdaptors(ClientPtr client)
{
  xvFormat format;
  xvAdaptorInfo ainfo;
  xvQueryAdaptorsReply rep;
  int totalSize, na, nf, rc;
  int nameSize;
  XvAdaptorPtr pa;
  XvFormatPtr pf;
  WindowPtr pWin;
  ScreenPtr pScreen;
  XvScreenPtr pxvs;

  REQUEST(xvQueryAdaptorsReq);
  REQUEST_SIZE_MATCH(xvQueryAdaptorsReq);

  rc = dixLookupWindow(&pWin, stuff->window, client, DixGetAttrAccess);
  if (rc != Success)
      return rc;

  pScreen = pWin->drawable.pScreen;
  pxvs = (XvScreenPtr)dixLookupPrivate(&pScreen->devPrivates,
				       XvGetScreenKey());
  if (!pxvs)
    {
      rep.type = X_Reply;
      rep.sequenceNumber = client->sequence;
      rep.num_adaptors = 0;
      rep.length = 0;

      _WriteQueryAdaptorsReply(client, &rep);

      return Success;
    }

  (* pxvs->ddQueryAdaptors)(pScreen, &pxvs->pAdaptors, &pxvs->nAdaptors);

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.num_adaptors = pxvs->nAdaptors;

  /* CALCULATE THE TOTAL SIZE OF THE REPLY IN BYTES */

  totalSize = pxvs->nAdaptors * sz_xvAdaptorInfo;

  /* FOR EACH ADPATOR ADD UP THE BYTES FOR ENCODINGS AND FORMATS */

  na = pxvs->nAdaptors;
  pa = pxvs->pAdaptors;
  while (na--)
    {
      totalSize += pad_to_int32(strlen(pa->name));
      totalSize += pa->nFormats * sz_xvFormat;
      pa++;
    }

  rep.length = bytes_to_int32(totalSize);

  _WriteQueryAdaptorsReply(client, &rep);

  na = pxvs->nAdaptors;
  pa = pxvs->pAdaptors;
  while (na--)
    {

      ainfo.base_id = pa->base_id;
      ainfo.num_ports = pa->nPorts;
      ainfo.type = pa->type;
      ainfo.name_size = nameSize = strlen(pa->name);
      ainfo.num_formats = pa->nFormats;

      _WriteAdaptorInfo(client, &ainfo);

      WriteToClient(client, nameSize, pa->name);

      nf = pa->nFormats;
      pf = pa->pFormats;
      while (nf--)
	{
	  format.depth = pf->depth;
	  format.visual = pf->visual;
	  _WriteFormat(client, &format);
	  pf++;
	}

      pa++;

    }

  return Success;
}

static int
ProcXvQueryEncodings(ClientPtr client)
{
  xvEncodingInfo einfo;
  xvQueryEncodingsReply rep;
  int totalSize;
  int nameSize;
  XvPortPtr pPort;
  int ne;
  XvEncodingPtr pe;
  int status;

  REQUEST(xvQueryEncodingsReq);
  REQUEST_SIZE_MATCH(xvQueryEncodingsReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.num_encodings = pPort->pAdaptor->nEncodings;

  /* FOR EACH ENCODING ADD UP THE BYTES FOR ENCODING NAMES */

  ne = pPort->pAdaptor->nEncodings;
  pe = pPort->pAdaptor->pEncodings;
  totalSize = ne * sz_xvEncodingInfo;
  while (ne--)
    {
      totalSize += pad_to_int32(strlen(pe->name));
      pe++;
    }

  rep.length = bytes_to_int32(totalSize);

  _WriteQueryEncodingsReply(client, &rep);

  ne = pPort->pAdaptor->nEncodings;
  pe = pPort->pAdaptor->pEncodings;
  while (ne--) 
    {
      einfo.encoding = pe->id;
      einfo.name_size = nameSize = strlen(pe->name);
      einfo.width = pe->width;
      einfo.height = pe->height;
      einfo.rate.numerator = pe->rate.numerator;
      einfo.rate.denominator = pe->rate.denominator;
      _WriteEncodingInfo(client, &einfo);
      WriteToClient(client, nameSize, pe->name);
      pe++;
    }

  return Success;
}

static int
ProcXvPutVideo(ClientPtr client)
{
  DrawablePtr pDraw;
  XvPortPtr pPort;
  GCPtr pGC;
  int status;

  REQUEST(xvPutVideoReq);
  REQUEST_SIZE_MATCH(xvPutVideoReq);

  VALIDATE_DRAWABLE_AND_GC(stuff->drawable, pDraw, DixWriteAccess);
  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  if (!(pPort->pAdaptor->type & XvInputMask) ||
	!(pPort->pAdaptor->type & XvVideoMask))
    {
      client->errorValue = stuff->port;
      return BadMatch;
    }

  status = XvdiMatchPort(pPort, pDraw);
  if (status != Success)
    {
      return status;
    }

  return XvdiPutVideo(client, pDraw, pPort, pGC, stuff->vid_x, stuff->vid_y,
		      stuff->vid_w, stuff->vid_h, stuff->drw_x, stuff->drw_y,
		      stuff->drw_w, stuff->drw_h);
}

static int
ProcXvPutStill(ClientPtr client)
{
  DrawablePtr pDraw;
  XvPortPtr pPort;
  GCPtr pGC;
  int status;

  REQUEST(xvPutStillReq);
  REQUEST_SIZE_MATCH(xvPutStillReq);

  VALIDATE_DRAWABLE_AND_GC(stuff->drawable, pDraw, DixWriteAccess);
  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  if (!(pPort->pAdaptor->type & XvInputMask) ||
	!(pPort->pAdaptor->type & XvStillMask))
    {
      client->errorValue = stuff->port;
      return BadMatch;
    }

  status = XvdiMatchPort(pPort, pDraw);
  if (status != Success)
    {
      return status;
    }

  return XvdiPutStill(client, pDraw, pPort, pGC, stuff->vid_x, stuff->vid_y,
		      stuff->vid_w, stuff->vid_h, stuff->drw_x, stuff->drw_y,
		      stuff->drw_w, stuff->drw_h);
}

static int
ProcXvGetVideo(ClientPtr client)
{
  DrawablePtr pDraw;
  XvPortPtr pPort;
  GCPtr pGC;
  int status;

  REQUEST(xvGetVideoReq);
  REQUEST_SIZE_MATCH(xvGetVideoReq);

  VALIDATE_DRAWABLE_AND_GC(stuff->drawable, pDraw, DixReadAccess);
  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  if (!(pPort->pAdaptor->type & XvOutputMask) ||
	!(pPort->pAdaptor->type & XvVideoMask))
    {
      client->errorValue = stuff->port;
      return BadMatch;
    }

  status = XvdiMatchPort(pPort, pDraw);
  if (status != Success)
    {
      return status;
    }

  return XvdiGetVideo(client, pDraw, pPort, pGC, stuff->vid_x, stuff->vid_y,
		      stuff->vid_w, stuff->vid_h, stuff->drw_x, stuff->drw_y,
		      stuff->drw_w, stuff->drw_h);
}

static int
ProcXvGetStill(ClientPtr client)
{
  DrawablePtr pDraw;
  XvPortPtr pPort;
  GCPtr pGC;
  int status;

  REQUEST(xvGetStillReq);
  REQUEST_SIZE_MATCH(xvGetStillReq);

  VALIDATE_DRAWABLE_AND_GC(stuff->drawable, pDraw, DixReadAccess);
  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  if (!(pPort->pAdaptor->type & XvOutputMask) ||
	!(pPort->pAdaptor->type & XvStillMask))
    {
      client->errorValue = stuff->port;
      return BadMatch;
    }

  status = XvdiMatchPort(pPort, pDraw);
  if (status != Success)
    {
      return status;
    }

  return XvdiGetStill(client, pDraw, pPort, pGC, stuff->vid_x, stuff->vid_y,
		      stuff->vid_w, stuff->vid_h, stuff->drw_x, stuff->drw_y,
		      stuff->drw_w, stuff->drw_h);
}

static int
ProcXvSelectVideoNotify(ClientPtr client)
{
  DrawablePtr pDraw;
  int rc;
  REQUEST(xvSelectVideoNotifyReq);
  REQUEST_SIZE_MATCH(xvSelectVideoNotifyReq);

  rc = dixLookupDrawable(&pDraw, stuff->drawable, client, 0, DixReceiveAccess);
  if (rc != Success)
    return rc;

  return XvdiSelectVideoNotify(client, pDraw, stuff->onoff);
}

static int
ProcXvSelectPortNotify(ClientPtr client)
{
  int status;
  XvPortPtr pPort;
  REQUEST(xvSelectPortNotifyReq);
  REQUEST_SIZE_MATCH(xvSelectPortNotifyReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  return XvdiSelectPortNotify(client, pPort, stuff->onoff);
}

static int
ProcXvGrabPort(ClientPtr client)
{
  int result, status;
  XvPortPtr pPort;
  xvGrabPortReply rep;
  REQUEST(xvGrabPortReq);
  REQUEST_SIZE_MATCH(xvGrabPortReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  status = XvdiGrabPort(client, pPort, stuff->time, &result);

  if (status != Success)
    {
      return status;
    }

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.length = 0;
  rep.result = result;

  _WriteGrabPortReply(client, &rep);

  return Success;
}

static int
ProcXvUngrabPort(ClientPtr client)
{
  int status;
  XvPortPtr pPort;
  REQUEST(xvGrabPortReq);
  REQUEST_SIZE_MATCH(xvGrabPortReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  return XvdiUngrabPort(client, pPort, stuff->time);
}

static int
ProcXvStopVideo(ClientPtr client)
{
  int status, rc;
  DrawablePtr pDraw;
  XvPortPtr pPort;
  REQUEST(xvStopVideoReq);
  REQUEST_SIZE_MATCH(xvStopVideoReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  rc = dixLookupDrawable(&pDraw, stuff->drawable, client, 0, DixWriteAccess);
  if (rc != Success)
    return rc;

  return XvdiStopVideo(client, pPort, pDraw);
}

static int
ProcXvSetPortAttribute(ClientPtr client)
{
  int status;
  XvPortPtr pPort;
  REQUEST(xvSetPortAttributeReq);
  REQUEST_SIZE_MATCH(xvSetPortAttributeReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixSetAttrAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  if (!ValidAtom(stuff->attribute))
    {
      client->errorValue = stuff->attribute;
      return BadAtom;
    }

  status = XvdiSetPortAttribute(client, pPort, stuff->attribute, stuff->value);

  if (status == BadMatch) 
      client->errorValue = stuff->attribute;
  else
      client->errorValue = stuff->value;

  return status;
}

static int
ProcXvGetPortAttribute(ClientPtr client)
{
  INT32 value;
  int status;
  XvPortPtr pPort;
  xvGetPortAttributeReply rep;
  REQUEST(xvGetPortAttributeReq);
  REQUEST_SIZE_MATCH(xvGetPortAttributeReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixGetAttrAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  if (!ValidAtom(stuff->attribute))
    {
      client->errorValue = stuff->attribute;
      return BadAtom;
    }

  status = XvdiGetPortAttribute(client, pPort, stuff->attribute, &value);
  if (status != Success)
    {
      client->errorValue = stuff->attribute;
      return status;
    }

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.length = 0;
  rep.value = value;
 
  _WriteGetPortAttributeReply(client, &rep);

  return Success;
}

static int
ProcXvQueryBestSize(ClientPtr client)
{
  int status;
  unsigned int actual_width, actual_height;
  XvPortPtr pPort;
  xvQueryBestSizeReply rep;
  REQUEST(xvQueryBestSizeReq);
  REQUEST_SIZE_MATCH(xvQueryBestSizeReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.length = 0;

  (* pPort->pAdaptor->ddQueryBestSize)(client, pPort, stuff->motion,
				       stuff->vid_w, stuff->vid_h, 
				       stuff->drw_w, stuff->drw_h, 
				       &actual_width, &actual_height);

  rep.actual_width = actual_width;
  rep.actual_height = actual_height;
 
  _WriteQueryBestSizeReply(client, &rep);

  return Success;
}


static int
ProcXvQueryPortAttributes(ClientPtr client)
{
  int status, size, i;
  XvPortPtr pPort;
  XvAttributePtr pAtt;
  xvQueryPortAttributesReply rep;
  xvAttributeInfo Info;
  REQUEST(xvQueryPortAttributesReq);
  REQUEST_SIZE_MATCH(xvQueryPortAttributesReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixGetAttrAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.num_attributes = pPort->pAdaptor->nAttributes;
  rep.text_size = 0;

  for(i = 0, pAtt = pPort->pAdaptor->pAttributes; 
      i < pPort->pAdaptor->nAttributes; i++, pAtt++) 
  {    
      rep.text_size += pad_to_int32(strlen(pAtt->name) + 1);
  }

  rep.length = (pPort->pAdaptor->nAttributes * sz_xvAttributeInfo)
      + rep.text_size;
  rep.length >>= 2;

  _WriteQueryPortAttributesReply(client, &rep);

  for(i = 0, pAtt = pPort->pAdaptor->pAttributes; 
      i < pPort->pAdaptor->nAttributes; i++, pAtt++) 
  {
      size = strlen(pAtt->name) + 1;  /* pass the NULL */
      Info.flags = pAtt->flags;
      Info.min = pAtt->min_value;
      Info.max = pAtt->max_value;
      Info.size = pad_to_int32(size);

      _WriteAttributeInfo(client, &Info);

      WriteToClient(client, size, pAtt->name);
  }

  return Success;
}

static int 
ProcXvPutImage(ClientPtr client)
{
  DrawablePtr pDraw;
  XvPortPtr pPort;
  XvImagePtr pImage = NULL;
  GCPtr pGC;
  int status, i, size;
  CARD16 width, height;

  REQUEST(xvPutImageReq);
  REQUEST_AT_LEAST_SIZE(xvPutImageReq);

  VALIDATE_DRAWABLE_AND_GC(stuff->drawable, pDraw, DixWriteAccess);
  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  if (!(pPort->pAdaptor->type & XvImageMask) ||
	!(pPort->pAdaptor->type & XvInputMask))
    {
      client->errorValue = stuff->port;
      return BadMatch;
    }

  status = XvdiMatchPort(pPort, pDraw);
  if (status != Success)
    {
      return status;
    }

  for(i = 0; i < pPort->pAdaptor->nImages; i++) {
      if(pPort->pAdaptor->pImages[i].id == stuff->id) {
	  pImage = &(pPort->pAdaptor->pImages[i]);
	  break;
      }
  }

  if(!pImage)
     return BadMatch;

  width = stuff->width;
  height = stuff->height;
  size = (*pPort->pAdaptor->ddQueryImageAttributes)(client, 
			pPort, pImage, &width, &height, NULL, NULL);
  size += sizeof(xvPutImageReq);
  size = bytes_to_int32(size);
  
  if((width < stuff->width) || (height < stuff->height))
     return BadValue;

  if(client->req_len < size)
     return BadLength;

  return XvdiPutImage(client, pDraw, pPort, pGC, stuff->src_x, stuff->src_y,
		      stuff->src_w, stuff->src_h, stuff->drw_x, stuff->drw_y,
		      stuff->drw_w, stuff->drw_h, pImage,
		      (unsigned char*)(&stuff[1]), FALSE,
		      stuff->width, stuff->height);
}

#ifdef MITSHM
/* redefined here since it's not in any header file */
typedef struct _ShmDesc {
    struct _ShmDesc *next;
    int shmid;
    int refcnt;
    char *addr;
    Bool writable;
    unsigned long size;
} ShmDescRec, *ShmDescPtr;

extern RESTYPE ShmSegType;
extern int ShmCompletionCode;

static int 
ProcXvShmPutImage(ClientPtr client)
{
  ShmDescPtr shmdesc;
  DrawablePtr pDraw;
  XvPortPtr pPort;
  XvImagePtr pImage = NULL;
  GCPtr pGC;
  int status, size_needed, i;
  CARD16 width, height;

  REQUEST(xvShmPutImageReq);
  REQUEST_SIZE_MATCH(xvShmPutImageReq);

  VALIDATE_DRAWABLE_AND_GC(stuff->drawable, pDraw, DixWriteAccess);
  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  if ((status = _AllocatePort(stuff->port, pPort)) != Success)
    {
      client->errorValue = stuff->port;
      return status;
    }

  if (!(pPort->pAdaptor->type & XvImageMask) ||
	!(pPort->pAdaptor->type & XvInputMask))
    {
      client->errorValue = stuff->port;
      return BadMatch;
    }

  status = XvdiMatchPort(pPort, pDraw);
  if (status != Success)
    {
      return status;
    }

  for(i = 0; i < pPort->pAdaptor->nImages; i++) {
      if(pPort->pAdaptor->pImages[i].id == stuff->id) {
	  pImage = &(pPort->pAdaptor->pImages[i]);
	  break;
      }
  }

  if(!pImage)
     return BadMatch;

  status = dixLookupResourceByType((pointer *)&shmdesc, stuff->shmseg,
				   ShmSegType, serverClient, DixReadAccess);
  if (status != Success)
      return status;
 
  width = stuff->width;
  height = stuff->height;
  size_needed = (*pPort->pAdaptor->ddQueryImageAttributes)(client, 
			pPort, pImage, &width, &height, NULL, NULL);
  if((size_needed + stuff->offset) > shmdesc->size)
      return BadAccess;

  if((width < stuff->width) || (height < stuff->height))
     return BadValue;
     
  status = XvdiPutImage(client, pDraw, pPort, pGC, stuff->src_x, stuff->src_y,
			stuff->src_w, stuff->src_h, stuff->drw_x, stuff->drw_y,
			stuff->drw_w, stuff->drw_h, pImage,
			(unsigned char *)shmdesc->addr + stuff->offset, 
			stuff->send_event, stuff->width, stuff->height);

  if((status == Success) && stuff->send_event) {
        xShmCompletionEvent ev;

        ev.type = ShmCompletionCode;
        ev.drawable = stuff->drawable;
        ev.minorEvent = xv_ShmPutImage;
        ev.majorEvent = XvReqCode;
        ev.shmseg = stuff->shmseg;
        ev.offset = stuff->offset;
        WriteEventsToClient(client, 1, (xEvent *) &ev);
  }

  return status;
}
#else /* !MITSHM */
static int
ProcXvShmPutImage(ClientPtr client)
{
    SendErrorToClient(client, XvReqCode, xv_ShmPutImage, 0, BadImplementation);
    return BadImplementation;
}
#endif

#ifdef XvMCExtension
#include "xvmcext.h"
#endif

static int 
ProcXvQueryImageAttributes(ClientPtr client)
{
  xvQueryImageAttributesReply rep;
  int size, num_planes, i;
  CARD16 width, height;
  XvImagePtr pImage = NULL;
  XvPortPtr pPort;
  int *offsets;
  int *pitches;
  int planeLength;
  REQUEST(xvQueryImageAttributesReq);

  REQUEST_SIZE_MATCH(xvQueryImageAttributesReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);
  
  for(i = 0; i < pPort->pAdaptor->nImages; i++) {
      if(pPort->pAdaptor->pImages[i].id == stuff->id) {
	  pImage = &(pPort->pAdaptor->pImages[i]);
	  break;
      }
  }

#ifdef XvMCExtension
  if(!pImage)
     pImage = XvMCFindXvImage(pPort, stuff->id);
#endif

  if(!pImage)
     return BadMatch;

  num_planes = pImage->num_planes;

  if(!(offsets = malloc(num_planes << 3)))
	return BadAlloc;
  pitches = offsets + num_planes;

  width = stuff->width;
  height = stuff->height;

  size = (*pPort->pAdaptor->ddQueryImageAttributes)(client, pPort, pImage,
					&width, &height, offsets, pitches);

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.length = planeLength = num_planes << 1;
  rep.num_planes = num_planes;
  rep.width = width;
  rep.height = height;
  rep.data_size = size;
 
  _WriteQueryImageAttributesReply(client, &rep);
  if(client->swapped)
    SwapLongs((CARD32*)offsets, planeLength);
  WriteToClient(client, planeLength << 2, (char*)offsets);

  free(offsets);

  return Success;
}

static int 
ProcXvListImageFormats(ClientPtr client)
{
  XvPortPtr pPort;
  XvImagePtr pImage;
  int i;
  xvListImageFormatsReply rep;
  xvImageFormatInfo info;
  REQUEST(xvListImageFormatsReq);

  REQUEST_SIZE_MATCH(xvListImageFormatsReq);

  VALIDATE_XV_PORT(stuff->port, pPort, DixReadAccess);

  rep.type = X_Reply;
  rep.sequenceNumber = client->sequence;
  rep.num_formats = pPort->pAdaptor->nImages;
  rep.length = bytes_to_int32(pPort->pAdaptor->nImages * sz_xvImageFormatInfo);

  _WriteListImageFormatsReply(client, &rep);

  pImage = pPort->pAdaptor->pImages;
  
  for(i = 0; i < pPort->pAdaptor->nImages; i++, pImage++) {
     info.id = pImage->id; 	
     info.type = pImage->type; 	
     info.byte_order = pImage->byte_order; 
     memcpy(&info.guid, pImage->guid, 16);	
     info.bpp = pImage->bits_per_pixel; 	
     info.num_planes = pImage->num_planes; 	
     info.depth = pImage->depth; 	
     info.red_mask = pImage->red_mask; 	
     info.green_mask = pImage->green_mask; 	
     info.blue_mask = pImage->blue_mask; 	
     info.format = pImage->format; 	
     info.y_sample_bits = pImage->y_sample_bits; 	
     info.u_sample_bits = pImage->u_sample_bits; 	
     info.v_sample_bits = pImage->v_sample_bits; 	
     info.horz_y_period = pImage->horz_y_period; 	
     info.horz_u_period = pImage->horz_u_period; 	
     info.horz_v_period = pImage->horz_v_period; 	
     info.vert_y_period = pImage->vert_y_period; 	
     info.vert_u_period = pImage->vert_u_period; 	
     info.vert_v_period = pImage->vert_v_period; 	
     memcpy(&info.comp_order, pImage->component_order, 32);	
     info.scanline_order = pImage->scanline_order;
     _WriteImageFormatInfo(client, &info);
  }  

  return Success;
}

static int (*XvProcVector[xvNumRequests])(ClientPtr) = {
    ProcXvQueryExtension,
    ProcXvQueryAdaptors,
    ProcXvQueryEncodings,
    ProcXvGrabPort,
    ProcXvUngrabPort,
    ProcXvPutVideo,
    ProcXvPutStill,
    ProcXvGetVideo,
    ProcXvGetStill,
    ProcXvStopVideo,
    ProcXvSelectVideoNotify,
    ProcXvSelectPortNotify,
    ProcXvQueryBestSize,
    ProcXvSetPortAttribute,
    ProcXvGetPortAttribute,
    ProcXvQueryPortAttributes,
    ProcXvListImageFormats,
    ProcXvQueryImageAttributes,
    ProcXvPutImage,
    ProcXvShmPutImage,
};

int
ProcXvDispatch(ClientPtr client)
{
  REQUEST(xReq);

  UpdateCurrentTime();

  if (stuff->data >= xvNumRequests) {
    SendErrorToClient(client, XvReqCode, stuff->data, 0, BadRequest);
    return BadRequest;
  }

  return XvProcVector[stuff->data](client);
}

/* Swapped Procs */

static int
SProcXvQueryExtension(ClientPtr client)
{
  char n;
  REQUEST(xvQueryExtensionReq);
  REQUEST_SIZE_MATCH(xvQueryExtensionReq);
  swaps(&stuff->length, n);
  return XvProcVector[xv_QueryExtension](client);
}

static int
SProcXvQueryAdaptors(ClientPtr client)
{
  char n;
  REQUEST(xvQueryAdaptorsReq);
  REQUEST_SIZE_MATCH(xvQueryAdaptorsReq);
  swaps(&stuff->length, n);
  swapl(&stuff->window, n);
  return XvProcVector[xv_QueryAdaptors](client);
}

static int
SProcXvQueryEncodings(ClientPtr client)
{
  char n;
  REQUEST(xvQueryEncodingsReq);
  REQUEST_SIZE_MATCH(xvQueryEncodingsReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  return XvProcVector[xv_QueryEncodings](client);
}

static int
SProcXvGrabPort(ClientPtr client)
{
  char n;
  REQUEST(xvGrabPortReq);
  REQUEST_SIZE_MATCH(xvGrabPortReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->time, n);
  return XvProcVector[xv_GrabPort](client);
}

static int
SProcXvUngrabPort(ClientPtr client)
{
  char n;
  REQUEST(xvUngrabPortReq);
  REQUEST_SIZE_MATCH(xvUngrabPortReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->time, n);
  return XvProcVector[xv_UngrabPort](client);
}

static int
SProcXvPutVideo(ClientPtr client)
{
  char n;
  REQUEST(xvPutVideoReq);
  REQUEST_SIZE_MATCH(xvPutVideoReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->drawable, n);
  swapl(&stuff->gc, n);
  swaps(&stuff->vid_x, n);
  swaps(&stuff->vid_y, n);
  swaps(&stuff->vid_w, n);
  swaps(&stuff->vid_h, n);
  swaps(&stuff->drw_x, n);
  swaps(&stuff->drw_y, n);
  swaps(&stuff->drw_w, n);
  swaps(&stuff->drw_h, n);
  return XvProcVector[xv_PutVideo](client);
}

static int
SProcXvPutStill(ClientPtr client)
{
  char n;
  REQUEST(xvPutStillReq);
  REQUEST_SIZE_MATCH(xvPutStillReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->drawable, n);
  swapl(&stuff->gc, n);
  swaps(&stuff->vid_x, n);
  swaps(&stuff->vid_y, n);
  swaps(&stuff->vid_w, n);
  swaps(&stuff->vid_h, n);
  swaps(&stuff->drw_x, n);
  swaps(&stuff->drw_y, n);
  swaps(&stuff->drw_w, n);
  swaps(&stuff->drw_h, n);
  return XvProcVector[xv_PutStill](client);
}

static int
SProcXvGetVideo(ClientPtr client)
{
  char n;
  REQUEST(xvGetVideoReq);
  REQUEST_SIZE_MATCH(xvGetVideoReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->drawable, n);
  swapl(&stuff->gc, n);
  swaps(&stuff->vid_x, n);
  swaps(&stuff->vid_y, n);
  swaps(&stuff->vid_w, n);
  swaps(&stuff->vid_h, n);
  swaps(&stuff->drw_x, n);
  swaps(&stuff->drw_y, n);
  swaps(&stuff->drw_w, n);
  swaps(&stuff->drw_h, n);
  return XvProcVector[xv_GetVideo](client);
}

static int
SProcXvGetStill(ClientPtr client)
{
  char n;
  REQUEST(xvGetStillReq);
  REQUEST_SIZE_MATCH(xvGetStillReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->drawable, n);
  swapl(&stuff->gc, n);
  swaps(&stuff->vid_x, n);
  swaps(&stuff->vid_y, n);
  swaps(&stuff->vid_w, n);
  swaps(&stuff->vid_h, n);
  swaps(&stuff->drw_x, n);
  swaps(&stuff->drw_y, n);
  swaps(&stuff->drw_w, n);
  swaps(&stuff->drw_h, n);
  return XvProcVector[xv_GetStill](client);
}

static int
SProcXvPutImage(ClientPtr client)
{
  char n;
  REQUEST(xvPutImageReq);
  REQUEST_AT_LEAST_SIZE(xvPutImageReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->drawable, n);
  swapl(&stuff->gc, n);
  swapl(&stuff->id, n);
  swaps(&stuff->src_x, n);
  swaps(&stuff->src_y, n);
  swaps(&stuff->src_w, n);
  swaps(&stuff->src_h, n);
  swaps(&stuff->drw_x, n);
  swaps(&stuff->drw_y, n);
  swaps(&stuff->drw_w, n);
  swaps(&stuff->drw_h, n);
  swaps(&stuff->width, n);
  swaps(&stuff->height, n);
  return XvProcVector[xv_PutImage](client);
}

#ifdef MITSHM
static int
SProcXvShmPutImage(ClientPtr client)
{
  char n;
  REQUEST(xvShmPutImageReq);
  REQUEST_SIZE_MATCH(xvShmPutImageReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->drawable, n);
  swapl(&stuff->gc, n);
  swapl(&stuff->shmseg, n);
  swapl(&stuff->id, n);
  swapl(&stuff->offset, n);
  swaps(&stuff->src_x, n);
  swaps(&stuff->src_y, n);
  swaps(&stuff->src_w, n);
  swaps(&stuff->src_h, n);
  swaps(&stuff->drw_x, n);
  swaps(&stuff->drw_y, n);
  swaps(&stuff->drw_w, n);
  swaps(&stuff->drw_h, n);
  swaps(&stuff->width, n);
  swaps(&stuff->height, n);
  return XvProcVector[xv_ShmPutImage](client);
}
#else /* MITSHM */
#define SProcXvShmPutImage ProcXvShmPutImage
#endif

static int
SProcXvSelectVideoNotify(ClientPtr client)
{
  char n;
  REQUEST(xvSelectVideoNotifyReq);
  REQUEST_SIZE_MATCH(xvSelectVideoNotifyReq);
  swaps(&stuff->length, n);
  swapl(&stuff->drawable, n);
  return XvProcVector[xv_SelectVideoNotify](client);
}

static int
SProcXvSelectPortNotify(ClientPtr client)
{
  char n;
  REQUEST(xvSelectPortNotifyReq);
  REQUEST_SIZE_MATCH(xvSelectPortNotifyReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  return XvProcVector[xv_SelectPortNotify](client);
}

static int
SProcXvStopVideo(ClientPtr client)
{
  char n;
  REQUEST(xvStopVideoReq);
  REQUEST_SIZE_MATCH(xvStopVideoReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->drawable, n);
  return XvProcVector[xv_StopVideo](client);
}

static int
SProcXvSetPortAttribute(ClientPtr client)
{
  char n;
  REQUEST(xvSetPortAttributeReq);
  REQUEST_SIZE_MATCH(xvSetPortAttributeReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->attribute, n);
  swapl(&stuff->value, n);
  return XvProcVector[xv_SetPortAttribute](client);
}

static int
SProcXvGetPortAttribute(ClientPtr client)
{
  char n;
  REQUEST(xvGetPortAttributeReq);
  REQUEST_SIZE_MATCH(xvGetPortAttributeReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->attribute, n);
  return XvProcVector[xv_GetPortAttribute](client);
}

static int
SProcXvQueryBestSize(ClientPtr client)
{
  char n;
  REQUEST(xvQueryBestSizeReq);
  REQUEST_SIZE_MATCH(xvQueryBestSizeReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swaps(&stuff->vid_w, n);
  swaps(&stuff->vid_h, n);
  swaps(&stuff->drw_w, n);
  swaps(&stuff->drw_h, n);
  return XvProcVector[xv_QueryBestSize](client);
}

static int
SProcXvQueryPortAttributes(ClientPtr client)
{
  char n;
  REQUEST(xvQueryPortAttributesReq);
  REQUEST_SIZE_MATCH(xvQueryPortAttributesReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  return XvProcVector[xv_QueryPortAttributes](client);
}

static int
SProcXvQueryImageAttributes(ClientPtr client)
{
  char n;
  REQUEST(xvQueryImageAttributesReq);
  REQUEST_SIZE_MATCH(xvQueryImageAttributesReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  swapl(&stuff->id, n);
  swaps(&stuff->width, n);
  swaps(&stuff->height, n);
  return XvProcVector[xv_QueryImageAttributes](client);
}

static int
SProcXvListImageFormats(ClientPtr client)
{
  char n;
  REQUEST(xvListImageFormatsReq);
  REQUEST_SIZE_MATCH(xvListImageFormatsReq);
  swaps(&stuff->length, n);
  swapl(&stuff->port, n);
  return XvProcVector[xv_ListImageFormats](client);
}

static int (*SXvProcVector[xvNumRequests])(ClientPtr) = {
    SProcXvQueryExtension,
    SProcXvQueryAdaptors,
    SProcXvQueryEncodings,
    SProcXvGrabPort,
    SProcXvUngrabPort,
    SProcXvPutVideo,
    SProcXvPutStill,
    SProcXvGetVideo,
    SProcXvGetStill,
    SProcXvStopVideo,
    SProcXvSelectVideoNotify,
    SProcXvSelectPortNotify,
    SProcXvQueryBestSize,
    SProcXvSetPortAttribute,
    SProcXvGetPortAttribute,
    SProcXvQueryPortAttributes,
    SProcXvListImageFormats,
    SProcXvQueryImageAttributes,
    SProcXvPutImage,
    SProcXvShmPutImage,
};

int
SProcXvDispatch(ClientPtr client)
{
  REQUEST(xReq);

  UpdateCurrentTime();

  if (stuff->data >= xvNumRequests) {
    SendErrorToClient(client, XvReqCode, stuff->data, 0, BadRequest);
    return BadRequest;
  }

  return SXvProcVector[stuff->data](client);
}

#ifdef PANORAMIX
static int
XineramaXvStopVideo(ClientPtr client)
{
   int result, i;
   PanoramiXRes *draw, *port;
   REQUEST(xvStopVideoReq);
   REQUEST_SIZE_MATCH(xvStopVideoReq);

   result = dixLookupResourceByClass((pointer *)&draw, stuff->drawable,
				     XRC_DRAWABLE, client, DixWriteAccess);
   if (result != Success)
       return (result == BadValue) ? BadDrawable : result;

   result = dixLookupResourceByType((pointer *)&port, stuff->port,
				    XvXRTPort, client, DixReadAccess);
   if (result != Success)
       return result;

   FOR_NSCREENS_BACKWARD(i) {
	if(port->info[i].id) {
	   stuff->drawable = draw->info[i].id;
	   stuff->port = port->info[i].id;
	   result = ProcXvStopVideo(client);
     	}
   }

   return result;
}

static int
XineramaXvSetPortAttribute(ClientPtr client)
{
    REQUEST(xvSetPortAttributeReq);
    PanoramiXRes *port;
    int result, i;

    REQUEST_SIZE_MATCH(xvSetPortAttributeReq);

    result = dixLookupResourceByType((pointer *)&port, stuff->port,
				     XvXRTPort, client, DixReadAccess);
    if (result != Success)
	return result;

    FOR_NSCREENS_BACKWARD(i) {
	if(port->info[i].id) {
	   stuff->port = port->info[i].id;
	   result = ProcXvSetPortAttribute(client);
	}
    }
    return result;
}

#ifdef MITSHM
static int 
XineramaXvShmPutImage(ClientPtr client)
{
    REQUEST(xvShmPutImageReq);
    PanoramiXRes *draw, *gc, *port;
    Bool send_event;
    Bool isRoot;
    int result, i, x, y;

    REQUEST_SIZE_MATCH(xvShmPutImageReq);

    send_event = stuff->send_event;

    result = dixLookupResourceByClass((pointer *)&draw, stuff->drawable,
				      XRC_DRAWABLE, client, DixWriteAccess);
    if (result != Success)
	return (result == BadValue) ? BadDrawable : result;

    result = dixLookupResourceByType((pointer *)&gc, stuff->gc,
				     XRT_GC, client, DixReadAccess);
    if (result != Success)
        return result;

    result = dixLookupResourceByType((pointer *)&port, stuff->port,
				     XvXRTPort, client, DixReadAccess);
    if (result != Success)
	return result;
 
    isRoot = (draw->type == XRT_WINDOW) && draw->u.win.root;

    x = stuff->drw_x;
    y = stuff->drw_y;

    FOR_NSCREENS_BACKWARD(i) {
	if(port->info[i].id) {
	   stuff->drawable = draw->info[i].id;
	   stuff->port = port->info[i].id;
	   stuff->gc = gc->info[i].id;
	   stuff->drw_x = x;
	   stuff->drw_y = y;
	   if(isRoot) {
		stuff->drw_x -= screenInfo.screens[i]->x;
		stuff->drw_y -= screenInfo.screens[i]->y;
	   }
	   stuff->send_event = (send_event && !i) ? 1 : 0;

	   result = ProcXvShmPutImage(client);
	}
    }
    return result;
}
#else
#define XineramaXvShmPutImage ProcXvShmPutImage
#endif

static int 
XineramaXvPutImage(ClientPtr client)
{
    REQUEST(xvPutImageReq);
    PanoramiXRes *draw, *gc, *port;
    Bool isRoot;
    int result, i, x, y;

    REQUEST_AT_LEAST_SIZE(xvPutImageReq);

    result = dixLookupResourceByClass((pointer *)&draw, stuff->drawable,
				      XRC_DRAWABLE, client, DixWriteAccess);
    if (result != Success)
	return (result == BadValue) ? BadDrawable : result;

    result = dixLookupResourceByType((pointer *)&gc, stuff->gc,
				     XRT_GC, client, DixReadAccess);
    if (result != Success)
        return result;

    result = dixLookupResourceByType((pointer *)&port, stuff->port,
				     XvXRTPort, client, DixReadAccess);
    if (result != Success)
	return result;
 
    isRoot = (draw->type == XRT_WINDOW) && draw->u.win.root;

    x = stuff->drw_x;
    y = stuff->drw_y;

    FOR_NSCREENS_BACKWARD(i) {
	if(port->info[i].id) {
	   stuff->drawable = draw->info[i].id;
	   stuff->port = port->info[i].id;
	   stuff->gc = gc->info[i].id;
	   stuff->drw_x = x;
	   stuff->drw_y = y;
	   if(isRoot) {
		stuff->drw_x -= screenInfo.screens[i]->x;
		stuff->drw_y -= screenInfo.screens[i]->y;
	   }

	   result = ProcXvPutImage(client);
	}
    }
    return result;
}

static int
XineramaXvPutVideo(ClientPtr client)
{
    REQUEST(xvPutImageReq);
    PanoramiXRes *draw, *gc, *port;
    Bool isRoot;
    int result, i, x, y;

    REQUEST_AT_LEAST_SIZE(xvPutVideoReq);

    result = dixLookupResourceByClass((pointer *)&draw, stuff->drawable,
				      XRC_DRAWABLE, client, DixWriteAccess);
    if (result != Success)
	return (result == BadValue) ? BadDrawable : result;

    result = dixLookupResourceByType((pointer *)&gc, stuff->gc,
				     XRT_GC, client, DixReadAccess);
    if (result != Success)
        return result;

    result = dixLookupResourceByType((pointer *)&port, stuff->port,
				     XvXRTPort, client, DixReadAccess);
    if (result != Success)
	return result;

    isRoot = (draw->type == XRT_WINDOW) && draw->u.win.root;

    x = stuff->drw_x;
    y = stuff->drw_y;

    FOR_NSCREENS_BACKWARD(i) {
        if(port->info[i].id) {
           stuff->drawable = draw->info[i].id;
           stuff->port = port->info[i].id;
           stuff->gc = gc->info[i].id;
           stuff->drw_x = x;
           stuff->drw_y = y;
           if(isRoot) {
                stuff->drw_x -= screenInfo.screens[i]->x;
                stuff->drw_y -= screenInfo.screens[i]->y;
           }

           result = ProcXvPutVideo(client);
        }
    }
    return result;
}

static int
XineramaXvPutStill(ClientPtr client)
{
    REQUEST(xvPutImageReq);
    PanoramiXRes *draw, *gc, *port;
    Bool isRoot;
    int result, i, x, y;

    REQUEST_AT_LEAST_SIZE(xvPutImageReq);

    result = dixLookupResourceByClass((pointer *)&draw, stuff->drawable,
				      XRC_DRAWABLE, client, DixWriteAccess);
    if (result != Success)
	return (result == BadValue) ? BadDrawable : result;

    result = dixLookupResourceByType((pointer *)&gc, stuff->gc,
				     XRT_GC, client, DixReadAccess);
    if (result != Success)
        return result;

    result = dixLookupResourceByType((pointer *)&port, stuff->port,
				     XvXRTPort, client, DixReadAccess);
    if (result != Success)
	return result;

    isRoot = (draw->type == XRT_WINDOW) && draw->u.win.root;

    x = stuff->drw_x;
    y = stuff->drw_y;

    FOR_NSCREENS_BACKWARD(i) {
        if(port->info[i].id) {
           stuff->drawable = draw->info[i].id;
           stuff->port = port->info[i].id;
           stuff->gc = gc->info[i].id;
           stuff->drw_x = x;
           stuff->drw_y = y;
           if(isRoot) {
                stuff->drw_x -= screenInfo.screens[i]->x;
                stuff->drw_y -= screenInfo.screens[i]->y;
           }

           result = ProcXvPutStill(client);
        }
    }
    return result;
}

static Bool
isImageAdaptor(XvAdaptorPtr pAdapt)
{
    return (pAdapt->type & XvImageMask) && (pAdapt->nImages > 0);
}

static Bool
hasOverlay(XvAdaptorPtr pAdapt)
{
    int i;
    for(i = 0; i < pAdapt->nAttributes; i++)
	if(!strcmp(pAdapt->pAttributes[i].name, "XV_COLORKEY"))
	    return TRUE;
    return FALSE;
}

static XvAdaptorPtr
matchAdaptor(ScreenPtr pScreen, XvAdaptorPtr refAdapt, Bool isOverlay)
{
    int i;
    XvScreenPtr xvsp = dixLookupPrivate(&pScreen->devPrivates, XvGetScreenKey());
    /* Do not try to go on if xv is not supported on this screen */
    if(xvsp == NULL)
	return NULL;

    /* if the adaptor has the same name it's a perfect match */
    for(i = 0; i < xvsp->nAdaptors; i++) {
	XvAdaptorPtr pAdapt = xvsp->pAdaptors + i;
	if(!strcmp(refAdapt->name, pAdapt->name))
	    return pAdapt;
    }

    /* otherwise we only look for XvImage adaptors */
    if(!isImageAdaptor(refAdapt))
	return NULL;

    /* prefer overlay/overlay non-overlay/non-overlay pairing */
    for(i = 0; i < xvsp->nAdaptors; i++) {
	XvAdaptorPtr pAdapt = xvsp->pAdaptors + i;
	if(isImageAdaptor(pAdapt) && isOverlay == hasOverlay(pAdapt))
	    return pAdapt;
    }

    /* but we'll take any XvImage pairing if we can get it */
    for(i = 0; i < xvsp->nAdaptors; i++) {
	XvAdaptorPtr pAdapt = xvsp->pAdaptors + i;
	if(isImageAdaptor(pAdapt))
	    return pAdapt;
    }
    return NULL;
}

void XineramifyXv(void)
{
   XvScreenPtr xvsp0 = dixLookupPrivate(&screenInfo.screens[0]->devPrivates, XvGetScreenKey());
   XvAdaptorPtr MatchingAdaptors[MAXSCREENS];
   int i, j, k;

   XvXRTPort = CreateNewResourceType(XineramaDeleteResource, "XvXRTPort");

   if (!xvsp0 || !XvXRTPort) return;
   SetResourceTypeErrorValue(XvXRTPort, _XvBadPort);

   for(i = 0; i < xvsp0->nAdaptors; i++) {
      Bool isOverlay;
      XvAdaptorPtr refAdapt = xvsp0->pAdaptors + i;
      if(!(refAdapt->type & XvInputMask)) continue;

      MatchingAdaptors[0] = refAdapt;
      isOverlay = hasOverlay(refAdapt);
      for(j = 1; j < PanoramiXNumScreens; j++)
	 MatchingAdaptors[j] = matchAdaptor(screenInfo.screens[j], refAdapt, isOverlay);

      /* now create a resource for each port */
      for(j = 0; j < refAdapt->nPorts; j++) {
	 PanoramiXRes *port = malloc(sizeof(PanoramiXRes));
	 if(!port)
	    break;

	 for(k = 0; k < PanoramiXNumScreens; k++) {
	    if(MatchingAdaptors[k] && (MatchingAdaptors[k]->nPorts > j)) 
		port->info[k].id = MatchingAdaptors[k]->base_id + j;
	    else
		port->info[k].id = 0;
	 } 
	 AddResource(port->info[0].id, XvXRTPort, port);
      }
   }

   /* munge the dispatch vector */
   XvProcVector[xv_PutVideo]		= XineramaXvPutVideo;
   XvProcVector[xv_PutStill]		= XineramaXvPutStill;
   XvProcVector[xv_StopVideo]		= XineramaXvStopVideo;
   XvProcVector[xv_SetPortAttribute]	= XineramaXvSetPortAttribute;
   XvProcVector[xv_PutImage]		= XineramaXvPutImage;
   XvProcVector[xv_ShmPutImage]		= XineramaXvShmPutImage;
}
#endif /* PANORAMIX */

void
XvResetProcVector(void)
{
#ifdef PANORAMIX
   XvProcVector[xv_PutVideo]		= ProcXvPutVideo;
   XvProcVector[xv_PutStill]		= ProcXvPutStill;
   XvProcVector[xv_StopVideo]		= ProcXvStopVideo;
   XvProcVector[xv_SetPortAttribute]	= ProcXvSetPortAttribute;
   XvProcVector[xv_PutImage]		= ProcXvPutImage;
   XvProcVector[xv_ShmPutImage]		= ProcXvShmPutImage;
#endif
}
