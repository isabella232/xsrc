/* $NetBSD: cg14_render.c,v 1.13 2019/07/24 16:07:59 macallan Exp $ */
/*
 * Copyright (c) 2013 Michael Lorenz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>

/* all driver need this */
#include "xf86.h"
#include "xf86_OSproc.h"
#include "compiler.h"

#include "cg14.h"

/*#define SX_SINGLE*/
/*#define SX_RENDER_DEBUG*/
/*#define SX_ADD_SOFTWARE*/

#ifdef SX_RENDER_DEBUG
#define ENTER xf86Msg(X_ERROR, "%s>\n", __func__);
#define DPRINTF xf86Msg
#else
#define ENTER
#define DPRINTF while (0) xf86Msg
#endif

char c[8] = " .,:+*oX";


void CG14Comp_Over32Solid(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height)
{
	uint32_t msk = src, mskx, dstx, m;
	int line, x, i;

	ENTER;

	for (line = 0; line < height; line++) {
		mskx = msk;
		dstx = dst;
#ifndef SX_SINGLE
		int rest;
		for (x = 0; x < width; x += 4) {
			rest = width - x;
			/* fetch 4 mask values */
			write_sx_io(p, mskx, SX_LDUQ0(12, 3, mskx & 7));
			/* fetch destination pixels */
			write_sx_io(p, dstx, SX_LDUQ0(60, 3, dstx & 7));
			/* duplicate them for all channels */
			write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(0, 12, 13, 2));
			write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(0, 16, 17, 2));
			write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(0, 20, 21, 2));
			write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(0, 24, 25, 2));
			/* generate inverted alpha */
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_XORS(12, 8, 28, 15));
			/* multiply source */
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(8, 12, 44, 3));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(8, 16, 48, 3));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(8, 20, 52, 3));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(8, 24, 56, 3));
			/* multiply dest */
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(28, 60, 76, 15));
			/* add up */
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(44, 76, 92, 15));
			/* write back */
			if (rest < 4) {
				write_sx_io(p, dstx, SX_STUQ0C(92, rest - 1, dstx & 7));
			} else {
				write_sx_io(p, dstx, SX_STUQ0C(92, 3, dstx & 7));
			}
			dstx += 16;
			mskx += 16;
		}
#else /* SX_SINGLE */
		for (x = 0; x < width; x++) {
			m = *(volatile uint32_t *)(p->fb + mskx);
			m = m >> 24;
			if (m == 0) {
				/* nothing to do - all transparent */
			} else if (m == 0xff) {
				/* all opaque */
				write_sx_io(p, dstx, SX_STUQ0(8, 0, dstx & 7));
			} else {
				/* fetch alpha value, stick it into scam */
				/* mask is in R[12:15] */
				/*write_sx_io(p, mskx,
				    SX_LDUQ0(12, 0, mskx & 7));*/
				write_sx_reg(p, SX_QUEUED(12), m);
				/* fetch dst pixel */
				write_sx_io(p, dstx, SX_LDUQ0(20, 0, dstx & 7));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ORV(12, 0, R_SCAM, 0));
				/*
				 * src * alpha + R0
				 * R[9:11] * SCAM + R0 -> R[17:19]
				 */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(9, 0, 17, 2));
			
				/* invert SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_XORV(12, 8, R_SCAM, 0));
#ifdef SX_DEBUG
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_XORV(12, 8, 13, 0));
#endif
				/* dst * (1 - alpha) + R[13:15] */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(21, 17, 25, 2));
				write_sx_io(p, dstx,
				    SX_STUQ0C(24, 0, dstx & 7));
			}
			dstx += 4;
			mskx += 4;
		}
#endif /* SX_SINGLE */
		dst += dstpitch;
		msk += srcpitch;
	}
}

void CG14Comp_Over8Solid(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height)
{
	uint32_t msk = src, mskx, dstx, m;
	int line, x, i;
#ifdef SX_DEBUG
	char buffer[256];
#endif
	ENTER;

	DPRINTF(X_ERROR, "src: %d %d %d, %08x\n", read_sx_reg(p, SX_QUEUED(9)),
	    read_sx_reg(p, SX_QUEUED(10)), read_sx_reg(p, SX_QUEUED(11)),
	    *(uint32_t *)(p->fb + p->srcoff));
	for (line = 0; line < height; line++) {
		mskx = msk;
		dstx = dst;
#ifndef SX_SINGLE
		int rest;
		for (x = 0; x < width; x += 4) {
			rest = width - x;			
			/* fetch 4 mask values */
			write_sx_io(p, mskx, SX_LDB(12, 3, mskx & 7));
			/* fetch destination pixels */
			write_sx_io(p, dstx, SX_LDUQ0(60, 3, dstx & 7));
			/* duplicate them for all channels */
			write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(0, 13, 16, 3));
			write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(0, 14, 20, 3));
			write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(0, 15, 24, 3));
			write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(0, 12, 13, 2));
			/* generate inverted alpha */
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_XORS(12, 8, 28, 15));
			/* multiply source */
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(8, 12, 44, 3));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(8, 16, 48, 3));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(8, 20, 52, 3));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(8, 24, 56, 3));
			/* multiply dest */
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_MUL16X16SR8(28, 60, 76, 15));
			/* add up */
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(44, 76, 92, 15));
			/* write back */
			if (rest < 4) {
				write_sx_io(p, dstx, SX_STUQ0C(92, rest - 1, dstx & 7));
			} else {
				write_sx_io(p, dstx, SX_STUQ0C(92, 3, dstx & 7));
			}
			dstx += 16;
			mskx += 4;
		}
#else /* SX_SINGLE */
		for (x = 0; x < width; x++) {
			m = *(volatile uint8_t *)(p->fb + mskx);
#ifdef SX_DEBUG
			buffer[x] = c[m >> 5];
#endif
			if (m == 0) {
				/* nothing to do - all transparent */
			} else if (m == 0xff) {
				/* all opaque */
				write_sx_io(p, dstx, SX_STUQ0(8, 0, dstx & 7));
			} else {
				/* fetch alpha value, stick it into scam */
				/* mask is in R[12:15] */
				/*write_sx_io(p, mskx & ~7, 
				    SX_LDB(12, 0, mskx & 7));*/
				write_sx_reg(p, SX_QUEUED(12), m);
				/* fetch dst pixel */
				write_sx_io(p, dstx, SX_LDUQ0(20, 0, dstx & 7));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ORV(12, 0, R_SCAM, 0));
				/*
				 * src * alpha + R0
				 * R[9:11] * SCAM + R0 -> R[17:19]
				 */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(9, 0, 17, 2));
			
				/* invert SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_XORV(12, 8, R_SCAM, 0));
#ifdef SX_DEBUG
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_XORV(12, 8, 13, 0));
#endif
				/* dst * (1 - alpha) + R[13:15] */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(21, 17, 25, 2));
				write_sx_io(p, dstx,
				    SX_STUQ0C(24, 0, dstx & 7));
			}
			dstx += 4;
			mskx += 1;
		}
#endif /* SX_SINGLE */
#ifdef SX_DEBUG
		buffer[x] = 0;
		xf86Msg(X_ERROR, "%s\n", buffer);
#endif
		dst += dstpitch;
		msk += srcpitch;
	}
}

void CG14Comp_Add32(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height)
{
	int line;
	uint32_t srcx, dstx;
	int full, part, x;

	ENTER;
	full = width >> 3;	/* chunks of 8 */
	part = width & 7;	/* leftovers */
	/* we do this up to 8 pixels at a time */
	for (line = 0; line < height; line++) {
		srcx = src;
		dstx = dst;
		for (x = 0; x < full; x++) {
			write_sx_io(p, srcx, SX_LDUQ0(8, 31, srcx & 7));
			write_sx_io(p, dstx, SX_LDUQ0(40, 31, dstx & 7));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(8, 40, 72, 15));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(24, 56, 88, 15));
			write_sx_io(p, dstx, SX_STUQ0(72, 31, dstx & 7));
			srcx += 128;
			dstx += 128;
		}

		/* do leftovers */
		write_sx_io(p, srcx, SX_LDUQ0(8, part - 1, srcx & 7));
		write_sx_io(p, dstx, SX_LDUQ0(40, part - 1, dstx & 7));
		if (part & 16) {
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(8, 40, 72, 15));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(24, 56, 88, part - 17));
		} else {
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(8, 40, 72, part - 1));
		}
		write_sx_io(p, dstx, SX_STUQ0(72, part - 1, dstx & 7));
		
		/* next line */
		src += srcpitch;
		dst += dstpitch;
	}
}

void CG14Comp_Add8(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height)
{
	int line;
	uint32_t srcx, dstx, srcoff, dstoff;
	int pre, full, part, x;
	uint8_t *d;
	char buffer[256];
	ENTER;

	srcoff = src & 7;
	src &= ~7;
	dstoff = dst & 7;
	dst &= ~7;
	full = width >> 5;	/* chunks of 32 */
	part = width & 31;	/* leftovers */

#ifdef SX_DEBUG
	xf86Msg(X_ERROR, "%d %d, %d x %d, %d %d\n", srcpitch, dstpitch,
	    width, height, full, part);
#endif
	/* we do this up to 32 pixels at a time */
	for (line = 0; line < height; line++) {
		srcx = src;
		dstx = dst;
#ifdef SX_ADD_SOFTWARE
		uint8_t *s = (uint8_t *)(p->fb + srcx + srcoff); 
		d = (uint8_t *)(p->fb + dstx + dstoff); 
		for (x = 0; x < width; x++) {
			d[x] = min(255, s[x] + d[x]);
		}
#else
		for (x = 0; x < full; x++) {
			write_sx_io(p, srcx, SX_LDB(8, 31, srcoff));
			write_sx_io(p, dstx, SX_LDB(40, 31, dstoff));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(8, 40, 72, 15));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(24, 56, 88, 15));
			write_sx_io(p, dstx, SX_STBC(72, 31, dstoff));
			srcx += 32;
			dstx += 32;
		}

		if (part > 0) {
			/* do leftovers */
			write_sx_io(p, srcx, SX_LDB(8, part - 1, srcoff));
			write_sx_io(p, dstx, SX_LDB(40, part - 1, dstoff));
			if (part > 16) {
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ADDV(8, 40, 72, 15));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ADDV(24, 56, 88, part - 17));
			} else {
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ADDV(8, 40, 72, part - 1));
			}
			write_sx_io(p, dstx, SX_STBC(72, part - 1, dstoff));
		}
#endif
#ifdef SX_DEBUG
		d = (uint8_t *)(p->fb + src + srcoff);
		for (x = 0; x < width; x++) {
			buffer[x] = c[d[x]>>5];
		}
		buffer[x] = 0;
		xf86Msg(X_ERROR, "%s\n", buffer);
#endif
		/* next line */
		src += srcpitch;
		dst += dstpitch;
	}
}

void CG14Comp_Add8_32(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height)
{
	int line;
	uint32_t srcx, dstx, srcoff, dstoff;
	int pre, full, part, x;
	uint8_t *d;
	char buffer[256];
	ENTER;

	srcoff = src & 7;
	src &= ~7;
	dstoff = dst & 7;
	dst &= ~7;
	full = width >> 5;	/* chunks of 32 */
	part = width & 31;	/* leftovers */

#ifdef SX_DEBUG
	xf86Msg(X_ERROR, "%d %d, %d x %d, %d %d\n", srcpitch, dstpitch,
	    width, height, full, part);
#endif
	/* we do this up to 32 pixels at a time */
	for (line = 0; line < height; line++) {
		srcx = src;
		dstx = dst;
		for (x = 0; x < full; x++) {
			/* load source bytes */
			write_sx_io(p, srcx, SX_LDB(8, 31, srcoff));
			/* load alpha from destination */
			write_sx_io(p, dstx, SX_LDUC0(40, 31, dstoff));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(8, 40, 72, 15));
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_ADDV(24, 56, 88, 15));
			/* write clamped values back into dest alpha */
			write_sx_io(p, dstx, SX_STUC0C(72, 31, dstoff));
			srcx += 32;
			dstx += 128;
		}

		if (part > 0) {
			/* do leftovers */
			write_sx_io(p, srcx, SX_LDB(8, part - 1, srcoff));
			write_sx_io(p, dstx, SX_LDUC0(40, part - 1, dstoff));
			if (part > 16) {
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ADDV(8, 40, 72, 15));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ADDV(24, 56, 88, part - 17));
			} else {
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ADDV(8, 40, 72, part - 1));
			}
			write_sx_io(p, dstx, SX_STUC0C(72, part - 1, dstoff));
		}
#ifdef SX_DEBUG
		d = (uint8_t *)(p->fb + src + srcoff);
		for (x = 0; x < width; x++) {
			buffer[x] = c[d[x]>>5];
		}
		buffer[x] = 0;
		xf86Msg(X_ERROR, "%s\n", buffer);
#endif
		/* next line */
		src += srcpitch;
		dst += dstpitch;
	}
}

void CG14Comp_Over32(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height, int flip)
{
	uint32_t srcx, dstx, mskx, m;
	int line, x, i, num;

	ENTER;

	write_sx_reg(p, SX_QUEUED(8), 0xff);
	for (line = 0; line < height; line++) {
		srcx = src;
		dstx = dst;

		for (x = 0; x < width; x += 4) {
			/* we do up to 4 pixels at a time */
			num = min(4, width - x);
			if (num <= 0) {
				xf86Msg(X_ERROR, "wtf?!\n");
				continue;
			}
			/* fetch source pixels */
			write_sx_io(p, srcx, SX_LDUQ0(12, num - 1, srcx & 7));
			if (flip) {
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_GATHER(13, 4, 40, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_GATHER(15, 4, 44, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SCATTER(40, 4, 15, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SCATTER(44, 4, 13, num - 1));
			}
			/* fetch dst pixels */
			write_sx_io(p, dstx, SX_LDUQ0(44, num - 1, dstx & 7));
			/* now process up to 4 pixels */
			for (i = 0; i < num; i++) {
				int ii = i << 2;
				/* write inverted alpha into SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_XORS(12 + ii, 8, R_SCAM, 0));
				/* dst * (1 - alpha) + src */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(44 + ii, 12 + ii, 76 + ii, 3));
			}
			write_sx_io(p, dstx,
			    SX_STUQ0C(76, num - 1, dstx & 7));
			srcx += 16;
			dstx += 16;
		}
		src += srcpitch;
		dst += dstpitch;
	}
}

void CG14Comp_Over32Mask(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t msk, uint32_t mskpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height, int flip)
{
	uint32_t srcx, dstx, mskx, m;
	int line, x, i, num;

	ENTER;

	write_sx_reg(p, SX_QUEUED(8), 0xff);
	for (line = 0; line < height; line++) {
		srcx = src;
		mskx = msk;
		dstx = dst;

		for (x = 0; x < width; x += 4) {
			/* we do up to 4 pixels at a time */
			num = min(4, width - x);
			if (num <= 0) {
				xf86Msg(X_ERROR, "wtf?!\n");
				continue;
			}
			/* fetch source pixels */
			write_sx_io(p, srcx, SX_LDUQ0(12, num - 1, srcx & 7));
			if (flip) {
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_GATHER(13, 4, 40, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_GATHER(15, 4, 44, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SCATTER(40, 4, 15, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SCATTER(44, 4, 13, num - 1));
			}
			/* fetch mask */
			write_sx_io(p, mskx, SX_LDB(28, num - 1, mskx & 7));
			/* fetch dst pixels */
			write_sx_io(p, dstx, SX_LDUQ0(44, num - 1, dstx & 7));
			/* now process up to 4 pixels */
			for (i = 0; i < num; i++) {
				int ii = i << 2;
				/* mask alpha to SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ORS(28 + i, 0, R_SCAM, 0));
				/* src * alpha */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(12 + ii, 0, 60 + ii, 3));
				/* write inverted alpha into SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_XORS(28 + i, 8, R_SCAM, 0));
				/* dst * (1 - alpha) + R[60:] */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(44 + ii, 60 + ii, 76 + ii, 3));
			}
			write_sx_io(p, dstx,
			    SX_STUQ0C(76, num - 1, dstx & 7));
			srcx += 16;
			mskx += 4;
			dstx += 16;
		}
		src += srcpitch;
		msk += mskpitch;
		dst += dstpitch;
	}
}

void CG14Comp_Over32Mask_noalpha(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t msk, uint32_t mskpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height, int flip)
{
	uint32_t srcx, dstx, mskx, m;
	int line, x, i, num;

	ENTER;

	write_sx_reg(p, SX_QUEUED(8), 0xff);
	write_sx_reg(p, SX_QUEUED(9), 0xff);
	write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(8, 0, 10, 1));
	for (line = 0; line < height; line++) {
		srcx = src;
		mskx = msk;
		dstx = dst;

		for (x = 0; x < width; x += 4) {
			/* we do up to 4 pixels at a time */
			num = min(4, width - x);
			if (num <= 0) {
				xf86Msg(X_ERROR, "wtf?!\n");
				continue;
			}
			/* fetch source pixels */
			write_sx_io(p, srcx, SX_LDUQ0(12, num - 1, srcx & 7));
			if (flip) {
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_GATHER(13, 4, 40, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_GATHER(15, 4, 44, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SCATTER(40, 4, 15, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SCATTER(44, 4, 13, num - 1));
			}
			/* fetch mask */
			write_sx_io(p, mskx, SX_LDB(28, num - 1, mskx & 7));
			/* fetch dst pixels */
			write_sx_io(p, dstx, SX_LDUQ0(44, num - 1, dstx & 7));
			/* set src alpha to 0xff */			
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_SCATTER(8, 4, 12, num - 1));
			/* now process up to 4 pixels */
			for (i = 0; i < num; i++) {
				int ii = i << 2;
				/* mask alpha to SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ORS(28 + i, 0, R_SCAM, 0));
				/* src * alpha */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(12 + ii, 0, 60 + ii, 3));
				/* write inverted alpha into SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_XORS(28 + i, 8, R_SCAM, 0));
				/* dst * (1 - alpha) + R[60:] */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(44 + ii, 60 + ii, 76 + ii, 3));
			}
			write_sx_io(p, dstx,
			    SX_STUQ0C(76, num - 1, dstx & 7));
			srcx += 16;
			mskx += 4;
			dstx += 16;
		}
		src += srcpitch;
		msk += mskpitch;
		dst += dstpitch;
	}
}

void CG14Comp_Over32Mask32_noalpha(Cg14Ptr p,
                   uint32_t src, uint32_t srcpitch,
                   uint32_t msk, uint32_t mskpitch,
                   uint32_t dst, uint32_t dstpitch,
                   int width, int height, int flip)
{
	uint32_t srcx, dstx, mskx, m;
	int line, x, i, num;

	ENTER;

	write_sx_reg(p, SX_QUEUED(8), 0xff);
	write_sx_reg(p, SX_QUEUED(9), 0xff);
	write_sx_reg(p, SX_INSTRUCTIONS, SX_ORS(8, 0, 10, 1));
	for (line = 0; line < height; line++) {
		srcx = src;
		mskx = msk;
		dstx = dst;

		for (x = 0; x < width; x += 4) {
			/* we do up to 4 pixels at a time */
			num = min(4, width - x);
			if (num <= 0) {
				xf86Msg(X_ERROR, "wtf?!\n");
				continue;
			}
			/* fetch source pixels */
			write_sx_io(p, srcx, SX_LDUQ0(12, num - 1, srcx & 7));
			if (flip) {
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_GATHER(13, 4, 40, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_GATHER(15, 4, 44, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SCATTER(40, 4, 15, num - 1));
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SCATTER(44, 4, 13, num - 1));
			}
			/* fetch mask */
			write_sx_io(p, mskx, SX_LDUQ0(28, num - 1, mskx & 7));
			/* fetch dst pixels */
			write_sx_io(p, dstx, SX_LDUQ0(44, num - 1, dstx & 7));
			/* set src alpha to 0xff */			
			write_sx_reg(p, SX_INSTRUCTIONS,
			    SX_SCATTER(8, 4, 12, num - 1));
			/* now process up to 4 pixels */
			for (i = 0; i < num; i++) {
				int ii = i << 2;
				/* mask alpha to SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_ORS(28 + ii, 0, R_SCAM, 0));
				/* src * alpha */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(12 + ii, 0, 60 + ii, 3));
				/* write inverted alpha into SCAM */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_XORS(28 + ii, 8, R_SCAM, 0));
				/* dst * (1 - alpha) + R[60:] */
				write_sx_reg(p, SX_INSTRUCTIONS,
				    SX_SAXP16X16SR8(44 + ii, 60 + ii, 76 + ii, 3));
			}
			write_sx_io(p, dstx,
			    SX_STUQ0C(76, num - 1, dstx & 7));
			srcx += 16;
			mskx += 16;
			dstx += 16;
		}
		src += srcpitch;
		msk += mskpitch;
		dst += dstpitch;
	}
}
