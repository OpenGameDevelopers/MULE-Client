#ifndef __MULECLIENT_GLEXTENDER_HPP__
#define __MULECLIENT_GLEXTENDER_HPP__

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glext.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern PFNGLBINDTEXTUREEXTPROC			__glBindTexture;
extern PFNGLBINDFRAMEBUFFERPROC			__glBindFrameBuffer;
extern PFNGLGENFRAMEBUFFERSEXTPROC		__glGenFrameBuffers;
extern PFNGLGENRENDERBUFFERSEXTPROC		__glGenRenderBuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC		__glDeleteFrameBuffers;
extern PFNGLDELETERENDERBUFFERSPROC		__glDeleteRenderBuffers;

#define glBindTexture		__glBindTexture
#define glBindFrameBuffer	__glBindFrameBuffer

#define glGenFrameBuffers	__glGenFrameBuffers
#define glGenRenderBuffers	__glGenRenderBuffers

#define glDeleteFrameBuffers	__glDeleteFrameBuffers
#define glDeleteRenderBuffers	__glDeleteRenderBuffers

#ifdef __cplusplus
}
#endif

bool InitGLExtensions( );

#endif

