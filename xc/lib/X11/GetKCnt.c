/* $XConsortium: GetKCnt.c,v 11.15 94/04/17 20:19:36 rws Exp $ */
/*

Copyright (c) 1986  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

*/

#define NEED_REPLIES
#include "Xlibint.h"

struct kmap {char keys[32];};

XGetKeyboardControl (dpy, state)
    register Display *dpy;
    register XKeyboardState *state;
    {
    xGetKeyboardControlReply rep;
    register xReq *req;
    LockDisplay(dpy);
    GetEmptyReq (GetKeyboardControl, req);
    (void) _XReply (dpy, (xReply *) &rep, 
	(SIZEOF(xGetKeyboardControlReply) - SIZEOF(xReply)) >> 2, xTrue);

    state->key_click_percent = rep.keyClickPercent;
    state->bell_percent = rep.bellPercent;
    state->bell_pitch = rep.bellPitch;
    state->bell_duration = rep.bellDuration;
    state->led_mask = rep.ledMask;
    state->global_auto_repeat = rep.globalAutoRepeat;
    * (struct kmap *) state->auto_repeats = * (struct kmap *) rep.map;
    UnlockDisplay(dpy);
    SyncHandle();
    }

