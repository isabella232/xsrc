/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3Cursor.h,v 3.0 1996/09/22 13:25:19 dawes Exp $ */





/* $XConsortium: s3Cursor.h /main/2 1995/11/12 19:05:50 kaleb $ */


extern Bool s3BlockCursor;
extern Bool s3ReloadCursor;

#define BLOCK_CURSOR	s3BlockCursor = TRUE;

/* BL 0815150096:
 * UNBLOCK_CURSOR is called at the end of every graphics function, mem_barrier()
 * guarantees update of regs and memory (on e.g. Alpha)
 */
#define UNBLOCK_CURSOR	{  mem_barrier(); \
			   if (s3ReloadCursor) \
			      s3RestoreCursor(s3savepScreen); \
			   s3BlockCursor = FALSE; \
			}
