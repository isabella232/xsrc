/*
 * $XFree86: xc/programs/Xserver/hw/tinyx/pcmcia/pcmciarotate.c,v 1.1 2004/06/02 22:43:02 dawes Exp $
 *
 * Copyright � 2001 Keith Packard, member of The XFree86 Project, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Copyright (c) 2004 by The XFree86 Project, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *   1.  Redistributions of source code must retain the above copyright
 *       notice, this list of conditions, and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution, and in the same place and form as other copyright,
 *       license and disclaimer information.
 *
 *   3.  The end-user documentation included with the redistribution,
 *       if any, must include the following acknowledgment: "This product
 *       includes software developed by The XFree86 Project, Inc
 *       (http://www.xfree86.org/) and its contributors", in the same
 *       place and form as other third-party acknowledgments.  Alternately,
 *       this acknowledgment may appear in the software itself, in the
 *       same form and location as other such third-party acknowledgments.
 *
 *   4.  Except as contained in this notice, the name of The XFree86
 *       Project, Inc shall not be used in advertising or otherwise to
 *       promote the sale, use or other dealings in this Software without
 *       prior written authorization from The XFree86 Project, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE XFREE86 PROJECT, INC OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include    "X.h"
#include    "scrnintstr.h"
#include    "windowstr.h"
#include    "font.h"
#include    "dixfontstr.h"
#include    "fontstruct.h"
#include    "mi.h"
#include    "regionstr.h"
#include    "globals.h"
#include    "gcstruct.h"
#include    "shadow.h"
#include    "fb.h"
#include    "pcmcia.h"

/*
 * These indicate which way the source (shadow) is scanned when
 * walking the screen in a particular direction
 */

#define LEFT_TO_RIGHT	1
#define RIGHT_TO_LEFT	-1
#define TOP_TO_BOTTOM	2
#define BOTTOM_TO_TOP	-2

typedef CARD16	PcBits;
typedef INT32	PcStride;

#define PC_SHIFT    4
#define PC_UNIT	    (1 << PC_SHIFT)
#define PC_HALFUNIT (1 << (PC_SHIFT-1))
#define PC_MASK	    (PC_UNIT - 1)
#define PC_ALLONES  ((PcBits) -1)

#define PcScrRight(x,b) FbScrRight(x,b)
#define PcScrLeft(x,b) FbScrLeft(x,b)
#define PcBitsMask(x,w)	(PcScrRight(PC_ALLONES,(x) & PC_MASK) & \
			 PcScrLeft(PC_ALLONES,(PC_UNIT - ((x) + (w))) & PC_MASK))

#define pcGetDrawable(pDrawable, pointer, stride, bpp, xoff, yoff) { \
    PixmapPtr   _pPix; \
    if ((pDrawable)->type != DRAWABLE_PIXMAP) \
	_pPix = fbGetWindowPixmap(pDrawable); \
    else \
	_pPix = (PixmapPtr) (pDrawable); \
    (pointer) = (PcBits *) _pPix->devPrivate.ptr; \
    (stride) = ((int) _pPix->devKind) / sizeof (PcBits); \
    (bpp) = _pPix->drawable.bitsPerPixel; \
    (xoff) = 0; \
    (yoff) = 0; \
}

void
pcmciaUpdateRotatePacked (ScreenPtr	pScreen,
			  shadowBufPtr	pBuf)
{
    RegionPtr	damage = &pBuf->damage;
    PixmapPtr	pShadow = pBuf->pPixmap;
    int		nbox = REGION_NUM_RECTS (damage);
    BoxPtr	pbox = REGION_RECTS (damage);
    PcBits	*shaBits;
    PcStride	shaStride;
    int		shaBpp;
    int		shaXoff, shaYoff;
    int		box_x1, box_x2, box_y1, box_y2;
    int		sha_x1 = 0, sha_y1 = 0;
    int		scr_x1 = 0, scr_x2 = 0, scr_y1 = 0, scr_y2 = 0, scr_w, scr_h;
    int		scr_x, scr_y;
    int		w;
    int		pixelsPerBits;
    int		pixelsMask;
    PcStride	shaStepOverY = 0, shaStepDownY = 0;
    PcStride	shaStepOverX = 0, shaStepDownX = 0;
    PcBits	*shaLine, *sha;
    int		shaHeight = pShadow->drawable.height;
    int		shaWidth = pShadow->drawable.width;
    PcBits	shaMask;
    int		shaFirstShift, shaShift;
    int		o_x_dir;
    int		o_y_dir;
    int		x_dir;
    int		y_dir;

    pcGetDrawable (&pShadow->drawable, shaBits, shaStride, shaBpp, shaXoff, shaYoff);
    pixelsPerBits = (sizeof (PcBits) * 8) / shaBpp;
    pixelsMask = ~(pixelsPerBits - 1);
    shaMask = PcBitsMask (PC_UNIT-shaBpp, shaBpp);
    /*
     * Compute rotation related constants to walk the shadow
     */
    o_x_dir = LEFT_TO_RIGHT;
    o_y_dir = TOP_TO_BOTTOM;
    if (pBuf->randr & SHADOW_REFLECT_X)
	o_x_dir = -o_x_dir;
    if (pBuf->randr & SHADOW_REFLECT_Y)
	o_y_dir = -o_y_dir;
    switch (pBuf->randr & (SHADOW_ROTATE_ALL)) {
    case SHADOW_ROTATE_0:	/* upper left shadow -> upper left screen */
    default:
	x_dir = o_x_dir;
	y_dir = o_y_dir;
	break;
    case SHADOW_ROTATE_90:    	/* upper right shadow -> upper left screen */
	x_dir = o_y_dir;
	y_dir = -o_x_dir;
	break;
    case SHADOW_ROTATE_180:	/* lower right shadow -> upper left screen */
	x_dir = -o_x_dir;
	y_dir = -o_y_dir;
	break;
    case SHADOW_ROTATE_270:	/* lower left shadow -> upper left screen */
	x_dir = -o_y_dir;
	y_dir = o_x_dir;
	break;
    }
    switch (x_dir) {
    case LEFT_TO_RIGHT:
	shaStepOverX = shaBpp;
	shaStepOverY = 0;
	break;
    case TOP_TO_BOTTOM:
	shaStepOverX = 0;
	shaStepOverY = shaStride;
	break;
    case RIGHT_TO_LEFT:
	shaStepOverX = -shaBpp;
	shaStepOverY = 0;
	break;
    case BOTTOM_TO_TOP:
	shaStepOverX = 0;
	shaStepOverY = -shaStride;
	break;
    }
    switch (y_dir) {
    case TOP_TO_BOTTOM:
	shaStepDownX = 0;
	shaStepDownY = shaStride;
	break;
    case RIGHT_TO_LEFT:
	shaStepDownX = -shaBpp;
	shaStepDownY = 0;
	break;
    case BOTTOM_TO_TOP:
	shaStepDownX = 0;
	shaStepDownY = -shaStride;
	break;
    case LEFT_TO_RIGHT:
	shaStepDownX = shaBpp;
	shaStepDownY = 0;
	break;
    }
    
    while (nbox--)
    {
        box_x1 = pbox->x1;
        box_y1 = pbox->y1;
        box_x2 = pbox->x2;
        box_y2 = pbox->y2;
        pbox++;

	/*
	 * Compute screen and shadow locations for this box
	 */
	switch (x_dir) {
	case LEFT_TO_RIGHT:
	    scr_x1 = box_x1 & pixelsMask;
	    scr_x2 = (box_x2 + pixelsPerBits - 1) & pixelsMask;
	    
	    sha_x1 = scr_x1;
	    break;
	case TOP_TO_BOTTOM:
	    scr_x1 = box_y1 & pixelsMask;
	    scr_x2 = (box_y2 + pixelsPerBits - 1) & pixelsMask;

	    sha_y1 = scr_x1;
	    break;
	case RIGHT_TO_LEFT:
	    scr_x1 = (shaWidth - box_x2) & pixelsMask;
	    scr_x2 = (shaWidth - box_x1 + pixelsPerBits - 1) & pixelsMask;

	    sha_x1 = (shaWidth - scr_x1 - 1);
	    break;
	case BOTTOM_TO_TOP:
	    scr_x1 = (shaHeight - box_y2) & pixelsMask;
	    scr_x2 = (shaHeight - box_y1 + pixelsPerBits - 1) & pixelsMask;
	    
	    sha_y1 = (shaHeight - scr_x1 - 1);
	    break;
	}
	switch (y_dir) {
	case TOP_TO_BOTTOM:
	    scr_y1 = box_y1;
	    scr_y2 = box_y2;

	    sha_y1 = scr_y1;
	    break;
	case RIGHT_TO_LEFT:
	    scr_y1 = (shaWidth - box_x2);
	    scr_y2 = (shaWidth - box_x1);

	    sha_x1 = box_x2 - 1;
	    break;
	case BOTTOM_TO_TOP:
	    scr_y1 = shaHeight - box_y2;
	    scr_y2 = shaHeight - box_y1;
	    
	    sha_y1 = box_y2 - 1;
	    break;
	case LEFT_TO_RIGHT:
	    scr_y1 = box_x1;
	    scr_y2 = box_x2;

	    sha_x1 = box_x1;
	    break;
	}
	scr_w = ((scr_x2 - scr_x1) * shaBpp) >> PC_SHIFT;
	scr_h = scr_y2 - scr_y1;
	scr_y = scr_y1;

	/* shift amount for first pixel on screen */ 
	shaFirstShift = PC_UNIT - ((sha_x1 * shaBpp) & PC_MASK) - shaBpp;
	
	/* pointer to shadow data first placed on screen */
	shaLine = (shaBits + 
		   sha_y1 * shaStride + 
		   ((sha_x1 * shaBpp) >> PC_SHIFT));

	/*
	 * Copy the bits, always write across the physical frame buffer
	 * to take advantage of write combining.
	 */
	while (scr_h--)
	{
	    int	    p;
	    PcBits  bits;
	    PcBits  *win;
	    int	    i;
	    CARD32  winSize;
	    
	    sha = shaLine;
	    shaShift = shaFirstShift;
	    w = scr_w;
	    scr_x = scr_x1 * shaBpp >> PC_SHIFT;

	    while (w)
	    {
		/*
		 * Map some of this line
		 */
		win = (PcBits *) (*pBuf->window) (pScreen,
						  scr_y,
						  scr_x << 1,
						  SHADOW_WINDOW_WRITE,
						  &winSize,
						  pBuf->closure);
		i = (winSize >> 1);
		if (i > w)
		    i = w;
		w -= i;
		scr_x += i;
		/*
		 * Copy the portion of the line mapped
		 */
		while (i--)
		{
		    bits = 0;
		    p = pixelsPerBits;
		    /*
		     * Build one word of output from multiple inputs
		     * 
		     * Note that for 90/270 rotations, this will walk
		     * down the shadow hitting each scanline once.
		     * This is probably not very efficient.
		     */
		    while (p--)
		    {
			bits = PcScrLeft(bits, shaBpp);
			bits |= PcScrRight (*sha, shaShift) & shaMask;

			shaShift -= shaStepOverX;
			if (shaShift >= PC_UNIT)
			{
			    shaShift -= PC_UNIT;
			    sha--;
			}
			else if (shaShift < 0)
			{
			    shaShift += PC_UNIT;
			    sha++;
			}
			sha += shaStepOverY;
		    }
		    *win++ = bits;
		}
	    }
	    scr_y++;
	    shaFirstShift -= shaStepDownX;
	    if (shaFirstShift >= PC_UNIT)
	    {
		shaFirstShift -= PC_UNIT;
		shaLine--;
	    }
	    else if (shaFirstShift < 0)
	    {
		shaFirstShift += PC_UNIT;
		shaLine++;
	    }
	    shaLine += shaStepDownY;
	}
    }
}
