/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3gtimg.c,v 3.9 1996/06/29 09:07:10 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  David Wexelblat makes 
 * no representations about the suitability of this software for any 
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO 
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
 * OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: s3gtimg.c /main/4 1995/11/12 19:06:41 kaleb $ */

#include "X.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "cfbmskbits.h"
#include "s3.h"

void
s3GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine)
     DrawablePtr pDrawable;
     int         sx, sy, w, h;
     unsigned int format;
     unsigned long planeMask;
     char *pdstLine;
{
   int width;

   if ((w == 0) || (h == 0))
      return;

   if (pDrawable->bitsPerPixel == 1)
   {
      mfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
      return;
   }

   if (!xf86VTSema || pDrawable->type != DRAWABLE_WINDOW)
   {
      switch (pDrawable->bitsPerPixel) {
      case 8:
         cfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	 break;
      case 16:
         cfb16GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	 break;
      case 24:
         cfb24GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	 break;
      case 32:
         cfb32GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	 break;
      }
      return;
   }

   width = PixmapBytePad(w, pDrawable->depth);
   if (format == ZPixmap)
   {
      (s3ImageReadFunc)(sx+pDrawable->x, sy+pDrawable->y, w, h, 
			pdstLine, width, 0, 0, planeMask);
   }
   else
   {
      /*
       * Worry about this later (much!).  Should be straighforward, though.
       * Read an image into a dummy pixmap, then use cfbCopyPlane8to1 to
       * copy each plane in planeMask into the destination.  At least
       * this is the theory.
       */
      miGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
   }
}

