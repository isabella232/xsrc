/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
*/
/* $XFree86: xc/lib/GL/glx/glxclient.h,v 1.25 2004/12/10 16:06:56 alanh Exp $ */

/**
 * \file glxclient.h
 * Direct rendering support added by Precision Insight, Inc.
 *
 * \author Kevin E. Martin <kevin@precisioninsight.com>
 */

#ifndef _GLX_client_h_
#define _GLX_client_h_
#define NEED_REPLIES
#define NEED_EVENTS
#include <X11/Xproto.h>
#include <X11/Xlibint.h>
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glxext.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <stdint.h>
#endif
#include "GL/glxint.h"
#include "GL/glxproto.h"
#include "GL/internal/glcore.h"
#include "glapitable.h"
#ifdef NEED_GL_FUNCS_WRAPPED
#include "indirect.h"
#endif
#ifdef XTHREADS
#include "Xthreads.h"
#endif
#ifdef GLX_BUILT_IN_XMESA
#include "realglx.h"  /* just silences prototype warnings */
#endif

#define GLX_MAJOR_VERSION	1	/* current version numbers */
#define GLX_MINOR_VERSION	4

#define __GLX_MAX_TEXTURE_UNITS 32

typedef struct __GLXcontextRec __GLXcontext;
typedef struct __GLXdisplayPrivateRec __GLXdisplayPrivate;
typedef struct _glapi_table __GLapi;

/************************************************************************/

#ifdef GLX_DIRECT_RENDERING

#include <GL/internal/dri_interface.h>

typedef void *(*CreateScreenFunc)(Display *dpy, int scrn, __DRIscreen *psc,
                                  int numConfigs, __GLXvisualConfig *config);

typedef void *(*CreateNewScreenFunc)(Display *dpy, int scrn, __DRIscreen *psc,
    const __GLcontextModes * modes, const __DRIversion * ddx_version,
    const __DRIversion * dri_version, const __DRIversion * drm_version,
    const __DRIframebuffer * frame_buffer, void * pSAREA,
    int fd, int internal_api_version, __GLcontextModes ** driver_modes);


/**
 * Display dependent methods.  This structure is initialized during the
 * \c driCreateDisplay call.
 */
struct __DRIdisplayRec {
    /**
     * Method to destroy the private DRI display data.
     */
    void (*destroyDisplay)(Display *dpy, void *displayPrivate);

    /**
     * Methods to create the private DRI screen data and initialize the
     * screen dependent methods.
     * This is an array [indexed by screen number] of function pointers.
     * 
     * \deprecated  This array of function pointers has been replaced by
     *              \c __DRIdisplayRec::createNewScreen.
     * \sa __DRIdisplayRec::createNewScreen
     */
    CreateScreenFunc * createScreen;

    /**
     * Opaque pointer to private per display direct rendering data.
     * \c NULL if direct rendering is not supported on this display.
     */
    struct __DRIdisplayPrivateRec *private;

    /**
     * Array of pointers to methods to create and initialize the private DRI
     * screen data.
     *
     * \sa __DRIdisplayRec::createScreen
     */
    CreateNewScreenFunc * createNewScreen;
};


/*
** We keep a linked list of these structures, one per DRI device driver.
*/
struct __DRIdriverRec {
   const char *name;
   void *handle;
   CreateScreenFunc createScreenFunc;
   CreateNewScreenFunc createNewScreenFunc;
   struct __DRIdriverRec *next;
};

/*
** Function to create and DRI display data and initialize the display
** dependent methods.
*/
extern void *driCreateDisplay(Display *dpy, __DRIdisplay *pdisp);

extern  __DRIdriver *driGetDriver(Display *dpy, int scrNum);

extern void DRI_glXUseXFont( Font font, int first, int count, int listbase );

/*
** Functions to obtain driver configuration information from a direct
** rendering client application
*/
extern const char *glXGetScreenDriver (Display *dpy, int scrNum);

extern const char *glXGetDriverConfig (const char *driverName);

#endif

/************************************************************************/

#define __GL_CLIENT_ATTRIB_STACK_DEPTH 16

typedef struct __GLXpixelStoreModeRec {
    GLboolean swapEndian;
    GLboolean lsbFirst;
    GLuint rowLength;
    GLuint imageHeight;
    GLuint imageDepth;
    GLuint skipRows;
    GLuint skipPixels;
    GLuint skipImages;
    GLuint alignment;
} __GLXpixelStoreMode;

/* The next 3 structures are deprecated.  Client state is no longer tracked
 * using them.  They only remain to maintain the layout / structure offset of
 * __GLXcontextRec.  In XFree86 5.0 they will be removed altogether.
 */

typedef struct __GLXvertexArrayPointerStateRecDEPRECATED {
    GLboolean enable;
    void (*proc)(const void *);
    const GLubyte *ptr;
    GLsizei skip;
    GLint size;
    GLenum type;
    GLsizei stride;
} __GLXvertexArrayPointerStateDEPRECATED;

typedef struct __GLXvertArrayStateRecDEPRECATED {
    __GLXvertexArrayPointerStateDEPRECATED vertex;
    __GLXvertexArrayPointerStateDEPRECATED normal;
    __GLXvertexArrayPointerStateDEPRECATED color;
    __GLXvertexArrayPointerStateDEPRECATED index;
    __GLXvertexArrayPointerStateDEPRECATED texCoord[__GLX_MAX_TEXTURE_UNITS];
    __GLXvertexArrayPointerStateDEPRECATED edgeFlag;
    GLint maxElementsVertices;
    GLint maxElementsIndices;
    GLint activeTexture;
} __GLXvertArrayStateDEPRECATED;

typedef struct __GLXattributeRecDEPRECATED {
	GLuint mask;

	/*
	** Pixel storage state.  Most of the pixel store mode state is kept
	** here and used by the client code to manage the packing and
	** unpacking of data sent to/received from the server.
	*/
	__GLXpixelStoreMode storePack, storeUnpack;

	/*
	** Vertex Array storage state.  The vertex array component
	** state is stored here and is used to manage the packing of
	** DrawArrays data sent to the server.
	*/
	__GLXvertArrayStateDEPRECATED vertArray;
} __GLXattributeDEPRECATED;

typedef struct __GLXvertexArrayPointerStateRec {
    void (*proc)(const void *);
    void (*mtex_proc)(GLenum, const void *);
    const GLubyte *ptr;
    GLsizei skip;
    GLint size;
    GLenum type;
    GLsizei stride;
} __GLXvertexArrayPointerState;

/**
 * Define which entries of \c __GLXvertArrayState::arrays match which
 * vertex arrays in the client-state vector.  These are only the one-of
 * arrays.  See the \c __GLXvertArrayState::arrays documentation for more
 * details.
 * 
 * \sa __GLXvertArrayState
 */
enum {
    edgeFlag_ARRAY,       /**< \c GL_EDGE_FLAG_ARRAY */
    index_ARRAY,          /**< \c GL_INDEX_ARRAY */
    fogCoord_ARRAY,       /**< \c GL_FOG_COORD_ARRAY */
    secondaryColor_ARRAY, /**< \c GL_SECONDARY_COLOR_ARRAY */
    color_ARRAY,          /**< \c GL_COLOR_ARRAY */
    normal_ARRAY,         /**< \c GL_NORMAL_ARRAY */
       
    /**
     * \c GL_VERTEX_ARRAY \b must be last!  All of the code for emitting arrays
     * and array elements is written based on the assumption that the vertex
     * array is last.
     */
    vertex_ARRAY,

    __GLX_MAX_ARRAYS      /**< Place holder entry. */
};

#define ENABLE_ARRAY(state,a) \
    do { (state)->vertArray.enables |=  (1U << (a ## _ARRAY)); } while( 0 )
#define DISABLE_ARRAY(state,a) \
    do { (state)->vertArray.enables &= ~(1U << (a ## _ARRAY)); } while( 0 )
#define IS_ARRAY_ENABLED_BY_INDEX(state, i) \
    (((state)->vertArray.enables & (1U << (i))) != 0)
#define IS_ARRAY_ENABLED(state, a) \
    IS_ARRAY_ENABLED_BY_INDEX(state, a ## _ARRAY)

#define ENABLE_TEXARRAY(state,a) \
    do { (state)->vertArray.texture_enables |=  (1U << a); } while( 0 )
#define DISABLE_TEXARRAY(state,a) \
    do { (state)->vertArray.texture_enables &= ~(1U << a); } while( 0 )
#define IS_TEXARRAY_ENABLED(state, a) \
    (((state)->vertArray.texture_enables & (1U << a)) != 0)

/**
 * Client-side vertex array state.
 */
typedef struct __GLXvertArrayStateRec {
    /**
     * Which client-side arrays are enabled?  These are the flag bits for
     * all of the non-texture coordinate arrays.
     */
    GLuint enables;

    /**
     * Which of the texture coordinate arrays are enabled?
     */
    GLuint texture_enables;

    /**
     * State for "one-of" arrays.  These are the arrays, such as
     * GL_COLOR_ARRAY or GL_FOG_COORD_ARRAY for which there is only one
     * array.  There are also "many-of" arrays, such as
     * GL_TEXTURE_COORD_ARRAY.
     */
    __GLXvertexArrayPointerState arrays[__GLX_MAX_ARRAYS];

    __GLXvertexArrayPointerState texCoord[__GLX_MAX_TEXTURE_UNITS];

    GLint maxElementsVertices;
    GLint maxElementsIndices;
    GLint activeTexture;
} __GLXvertArrayState;

typedef struct __GLXattributeRec {
	GLuint mask;

	/*
	** Pixel storage state.  Most of the pixel store mode state is kept
	** here and used by the client code to manage the packing and
	** unpacking of data sent to/received from the server.
	*/
	__GLXpixelStoreMode storePack, storeUnpack;

	/*
	** Vertex Array storage state.  The vertex array component
	** state is stored here and is used to manage the packing of
	** DrawArrays data sent to the server.
	*/
	__GLXvertArrayState vertArray;

	/**
	 * Is EXT_vertex_array / GL 1.1 DrawArrays protocol specifically
	 * disabled?
	 */
	GLboolean NoDrawArraysProtocol;
} __GLXattribute;

typedef struct __GLXattributeMachineRec {
	__GLXattribute *stack[__GL_CLIENT_ATTRIB_STACK_DEPTH];
	__GLXattribute **stackPointer;
} __GLXattributeMachine;

/**
 * GLX state that needs to be kept on the client.  One of these records
 * exist for each context that has been made current by this client.
 */
struct __GLXcontextRec {
    /**
     * \name Drawing command buffer.
     *
     * Drawing commands are packed into this buffer before being sent as a
     * single GLX protocol request.  The buffer is sent when it overflows or
     * is flushed by \c __glXFlushRenderBuffer.  \c pc is the next location
     * in the buffer to be filled.  \c limit is described above in the buffer
     * slop discussion.
     *
     * Commands that require large amounts of data to be transfered will
     * also use this buffer to hold a header that describes the large
     * command.
     *
     * These must be the first 6 fields since they are static initialized
     * in the dummy context in glxext.c
     */
    /*@{*/
    GLubyte *buf;
    GLubyte *pc;
    GLubyte *limit;
    GLubyte *bufEnd;
    GLint bufSize;
    /*@}*/

    /**
     * The XID of this rendering context.  When the context is created a
     * new XID is allocated.  This is set to None when the context is
     * destroyed but is still current to some thread. In this case the
     * context will be freed on next MakeCurrent.
     */
    XID xid;

    /**
     * The XID of the \c shareList context.
     */
    XID share_xid;

    /**
     * Visual id.
     * 
     * \deprecated
     * This filed has been largely been replaced by the \c mode field, but
     * the work is not quite done.
     */
    VisualID vid;

    /**
     * Screen number.
     */
    GLint screen;

    /**
     * \c GL_TRUE if the context was created with ImportContext, which
     * means the server-side context was created by another X client.
     */
    GLboolean imported;

    /**
     * The context tag returned by MakeCurrent when this context is made
     * current. This tag is used to identify the context that a thread has
     * current so that proper server context management can be done.  It is
     * used for all context specific commands (i.e., \c Render, \c RenderLarge,
     * \c WaitX, \c WaitGL, \c UseXFont, and \c MakeCurrent (for the old
     * context)).
     */
    GLXContextTag currentContextTag;

    /**
     * \name Rendering mode
     *
     * The rendering mode is kept on the client as well as the server.
     * When \c glRenderMode is called, the buffer associated with the
     * previous rendering mode (feedback or select) is filled.
     */
    /*@{*/
    GLenum renderMode;
    GLfloat *feedbackBuf;
    GLuint *selectBuf;
    /*@}*/

    /**
     * This is \c GL_TRUE if the pixel unpack modes are such that an image
     * can be unpacked from the clients memory by just copying.  It may
     * still be true that the server will have to do some work.  This
     * just promises that a straight copy will fetch the correct bytes.
     */
    GLboolean fastImageUnpack;

    /**
     * Fill newImage with the unpacked form of \c oldImage getting it
     * ready for transport to the server.
     */
    void (*fillImage)(__GLXcontext*, GLint, GLint, GLint, GLint, GLenum,
		      GLenum, const GLvoid*, GLubyte*, GLubyte*);

    /**
     * \name Client side attribs.
     */
    /*@{*/
    __GLXattributeDEPRECATED stateDEPRECATED;
    __GLXattributeMachine attributes;
    /*@}*/

    /**
     * Client side error code.  This is set when client side gl API
     * routines need to set an error because of a bad enumerant or
     * running out of memory, etc.
     */
    GLenum error;

    /**
     * Whether this context does direct rendering.
     */
    Bool isDirect;

    /**
     * \c dpy of current display for this context.  Will be \c NULL if not
     * current to any display, or if this is the "dummy context".
     */
    Display *currentDpy;

    /**
     * The current drawable for this context.  Will be None if this
     * context is not current to any drawable.  currentReadable is below.
     */
    GLXDrawable currentDrawable;

    /**
     * \name GL Constant Strings
     *
     * Constant strings that describe the server implementation
     * These pertain to GL attributes, not to be confused with
     * GLX versioning attributes.
     */
    /*@{*/
    GLubyte *vendor;
    GLubyte *renderer;
    GLubyte *version;
    GLubyte *extensions;
    /*@}*/

    /**
     * Record the dpy this context was created on for later freeing
     */
    Display *createDpy;

    /**
     * Maximum small render command size.  This is the smaller of 64k and
     * the size of the above buffer.
     */
    GLint maxSmallRenderCommandSize;

    /**
     * Major opcode for the extension.  Copied here so a lookup isn't
     * needed.
     */
    GLint majorOpcode;

#ifdef GLX_DIRECT_RENDERING
    /**
     * Per context direct rendering interface functions and data.
     */
    __DRIcontext driContext;
#endif
    
    /**
     * \c GLXFBConfigID used to create this context.  May be \c None.  This
     * field has been replaced by the \c mode field.
     *
     * \since Internal API version 20030317.
     *
     * \deprecated
     * This filed has been largely been replaced by the \c mode field, but
     * the work is not quite done.
     */
    GLXFBConfigID  fbconfigID;

    /**
     * The current read-drawable for this context.  Will be None if this
     * context is not current to any drawable.
     *
     * \since Internal API version 20030606.
     */
    GLXDrawable currentReadable;

   /** 
    * Pointer to client-state data that is private to libGL.  This is only
    * used for indirect rendering contexts.
    *
    * No internal API version change was made for this change.  Client-side
    * drivers should NEVER use this data or even care that it exists.
    */
   void * client_state_private;
   
   /**
    * Stored value for \c glXQueryContext attribute \c GLX_RENDER_TYPE.
    */
   int renderType;
    
   /**
    * \name Raw server GL version
    *
    * True core GL version supported by the server.  This is the raw value
    * returned by the server, and it may not reflect what is actually
    * supported (or reported) by the client-side library.
    */
    /*@{*/
   int server_major;        /**< Major version number. */
   int server_minor;        /**< Minor version number. */
    /*@}*/
};

#define __glXSetError(gc,code) \
    if (!(gc)->error) {	       \
	(gc)->error = code;    \
    }

extern void __glFreeAttributeState(__GLXcontext *);

/************************************************************************/

/**
 * The size of the largest drawing command known to the implementation
 * that will use the GLXRender GLX command.  In this case it is
 * \c glPolygonStipple.
 */
#define __GLX_MAX_SMALL_RENDER_CMD_SIZE	156

/**
 * To keep the implementation fast, the code uses a "limit" pointer
 * to determine when the drawing command buffer is too full to hold
 * another fixed size command.  This constant defines the amount of
 * space that must always be available in the drawing command buffer
 * at all times for the implementation to work.  It is important that
 * the number be just large enough, but not so large as to reduce the
 * efficacy of the buffer.  The "+32" is just to keep the code working
 * in case somebody counts wrong.
 */
#define __GLX_BUFFER_LIMIT_SIZE	(__GLX_MAX_SMALL_RENDER_CMD_SIZE + 32)

/**
 * This implementation uses a smaller threshold for switching
 * to the RenderLarge protocol than the protcol requires so that
 * large copies don't occur.
 */
#define __GLX_RENDER_CMD_SIZE_LIMIT	4096

/**
 * One of these records exists per screen of the display.  It contains
 * a pointer to the config data for that screen (if the screen supports GL).
 */
typedef struct __GLXscreenConfigsRec {
    /**
     * GLX visuals formated as \c __GLXvisualConfig structures.
     */
    /*@{*/
    __GLXvisualConfig * old_configs;
    int numOldConfigs;
    /*@}*/

    /**
     * GLX extension string reported by the X-server.
     */
    const char *serverGLXexts;

    /**
     * GLX extension string to be reported to applications.  This is the
     * set of extensions that the application can actually use.
     */
    char *effectiveGLXexts;

#ifdef GLX_DIRECT_RENDERING
    /**
     * Per screen direct rendering interface functions and data.
     */
    __DRIscreen driScreen;
#endif

    /**
     * Linked list of configurations for this screen.  This is intended to
     * be a superset of \c old_configs.
     */
    __GLcontextModes *configs;
    /**
     * Per-screen dynamic GLX extension tracking.  The \c direct_support
     * field only contains enough bits for 64 extensions.  Should libGL
     * ever need to track more than 64 GLX extensions, we can safely grow
     * this field.  The \c __GLXscreenConfigs structure is not used outside
     * libGL.
     */
    /*@{*/
    unsigned char direct_support[8];
    GLboolean ext_list_first_time;
    /*@}*/

} __GLXscreenConfigs;

/**
 * Per display private data.  One of these records exists for each display
 * that is using the OpenGL (GLX) extension.
 */
struct __GLXdisplayPrivateRec {
    /**
     * Back pointer to the display
     */
    Display *dpy;

    /**
     * The \c majorOpcode is common to all connections to the same server.
     * It is also copied into the context structure.
     */
    int majorOpcode;

    /**
     * \name Server Version
     *
     * Major and minor version returned by the server during initialization.
     */
    /*@{*/
    int majorVersion, minorVersion;
    /*@}*/

    /**
     * \name Storage for the servers GLX vendor and versions strings.
     * 
     * These are the same for all screens on this display. These fields will
     * be filled in on demand.
     */
    /*@{*/
    const char *serverGLXvendor;
    const char *serverGLXversion;
    /*@}*/

    /**
     * Configurations of visuals for all screens on this display.
     * Also, per screen data which now includes the server \c GLX_EXTENSION
     * string.
     */
    __GLXscreenConfigs *screenConfigs;

#ifdef GLX_DIRECT_RENDERING
    /**
     * Per display direct rendering interface functions and data.
     */
    __DRIdisplay driDisplay;
#endif
};

void __glXFreeContext(__GLXcontext*);

extern GLubyte *__glXFlushRenderBuffer(__GLXcontext*, GLubyte*);

extern void __glXSendLargeChunk(__GLXcontext *gc, GLint requestNumber, 
				GLint totalRequests,
				const GLvoid * data, GLint dataLen);

extern void __glXSendLargeCommand(__GLXcontext *, const GLvoid *, GLint,
				  const GLvoid *, GLint);

/* Initialize the GLX extension for dpy */
extern __GLXdisplayPrivate *__glXInitialize(Display*);

/************************************************************************/

extern int __glXDebug;

/* This is per-thread storage in an MT environment */
#if defined(GLX_DIRECT_RENDERING) && defined(XTHREADS)
extern __GLXcontext *__glXGetCurrentContext(void);
extern void __glXSetCurrentContext(__GLXcontext *c);
#else
extern __GLXcontext *__glXcurrentContext;
#define __glXGetCurrentContext()	__glXcurrentContext
#define __glXSetCurrentContext(gc)	__glXcurrentContext = gc
#endif


/*
** Global lock for all threads in this address space using the GLX
** extension
*/
#if defined(GLX_DIRECT_RENDERING) && defined(XTHREADS)
extern xmutex_rec __glXmutex;
#define __glXLock()    xmutex_lock(&__glXmutex)
#define __glXUnlock()  xmutex_unlock(&__glXmutex)
#else
#define __glXLock()
#define __glXUnlock()
#endif

/*
** Setup for a command.  Initialize the extension for dpy if necessary.
*/
extern CARD8 __glXSetupForCommand(Display *dpy);

/************************************************************************/

/*
** Data conversion and packing support.
*/

/* Return the size, in bytes, of some pixel data */
extern GLint __glImageSize(GLint, GLint, GLint, GLenum, GLenum);

/* Return the number of elements per group of a specified format*/
extern GLint __glElementsPerGroup(GLenum format, GLenum type);

/* Return the number of bytes per element, based on the element type (other
** than GL_BITMAP).
*/
extern GLint __glBytesPerElement(GLenum type);

/* Return the k value for a given map target */
extern GLint __glEvalComputeK(GLenum);

/*
** Fill the transport buffer with the data from the users buffer,
** applying some of the pixel store modes (unpack modes) to the data
** first.  As a side effect of this call, the "modes" field is
** updated to contain the modes needed by the server to decode the
** sent data.
*/
extern void __glFillImage(__GLXcontext*, GLint, GLint, GLint, GLint, GLenum,
			  GLenum, const GLvoid*, GLubyte*, GLubyte*);

/* Copy map data with a stride into a packed buffer */
extern void __glFillMap1f(GLint, GLint, GLint, const GLfloat *, GLubyte *);
extern void __glFillMap1d(GLint, GLint, GLint, const GLdouble *, GLubyte *);
extern void __glFillMap2f(GLint, GLint, GLint, GLint, GLint,
			  const GLfloat *, GLfloat *);
extern void __glFillMap2d(GLint, GLint, GLint, GLint, GLint,
			  const GLdouble *, GLdouble *);

/*
** Empty an image out of the reply buffer into the clients memory applying
** the pack modes to pack back into the clients requested format.
*/
extern void __glEmptyImage(__GLXcontext*, GLint, GLint, GLint, GLint, GLenum,
		           GLenum, const GLubyte *, GLvoid *);


/*
** Allocate and Initialize Vertex Array client state 
*/
extern void __glXInitVertexArrayState(__GLXcontext*);

/*
** Inform the Server of the major and minor numbers and of the client
** libraries extension string.
*/
extern void __glXClientInfo (  Display *dpy, int opcode );

/************************************************************************/

/*
** Declarations that should be in Xlib
*/
#ifdef __GL_USE_OUR_PROTOTYPES
extern void _XFlush(Display*);
extern Status _XReply(Display*, xReply*, int, Bool);
extern void _XRead(Display*, void*, long);
extern void _XSend(Display*, const void*, long);
#endif


/*
** GLX_BUILT_IN_XMESA controls whether libGL has a built-in verions of
** Mesa that can render to non-GLX displays.
*/
#ifdef GLX_BUILT_IN_XMESA
#define GLX_PREFIX(function)  _real_##function
#else
#define GLX_PREFIX(function)  function
#endif


extern void __glXInitializeVisualConfigFromTags( __GLcontextModes *config,
    int count, const INT32 *bp, Bool tagged_only, Bool fbconfig_style_tags );

extern char * __glXGetStringFromServer( Display * dpy, int opcode,
    CARD32 glxCode, CARD32 for_whom, CARD32 name );

extern char *__glXstrdup(const char *str);


extern const char __glXGLClientVersion[];
extern const char __glXGLClientExtensions[];

/* Determine the internal API version */
extern int __glXGetInternalVersion(void);

/* Get the unadjusted system time */
extern int __glXGetUST( int64_t * ust );

#endif /* !__GLX_client_h__ */
