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

extern PFNGLBINDBUFFERARBPROC			__glBindBuffer;
extern PFNGLBINDTEXTUREEXTPROC			__glBindTexture;
extern PFNGLBINDFRAMEBUFFERPROC			__glBindFrameBuffer;
extern PFNGLBUFFERDATAARBPROC			__glBufferData;
extern PFNGLBUFFERSUBDATAARBPROC		__glBufferSubData;
extern PFNGLGENBUFFERSARBPROC			__glGenBuffers;
extern PFNGLGENFRAMEBUFFERSEXTPROC		__glGenFrameBuffers;
extern PFNGLGENRENDERBUFFERSEXTPROC		__glGenRenderBuffers;
extern PFNGLDELETEBUFFERSARBPROC		__glDeleteBuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC		__glDeleteFrameBuffers;
extern PFNGLDELETERENDERBUFFERSPROC		__glDeleteRenderBuffers;
extern PFNGLMAPBUFFERARBPROC			__glMapBuffer;
extern PFNGLUNMAPBUFFERARBPROC			__glUnmapBuffer;

#define glBindBuffer		__glBindBuffer
#define glBindTexture		__glBindTexture
#define glBindFrameBuffer	__glBindFrameBuffer

#define glBufferData		__glBufferData
#define glBufferSubData		__glBufferSubData

#define glGenBuffers		__glGenBuffers
#define glGenFrameBuffers	__glGenFrameBuffers
#define glGenRenderBuffers	__glGenRenderBuffers

#define glDeleteBuffers			__glDeleteBuffers
#define glDeleteFrameBuffers	__glDeleteFrameBuffers
#define glDeleteRenderBuffers	__glDeleteRenderBuffers

#define glMapBuffer		__glMapBuffer
#define glUnmapBuffer	__glUnmapBuffer

#ifdef __cplusplus
}
#endif

bool InitGLExtensions( );

#endif

