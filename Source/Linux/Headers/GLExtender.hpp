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
extern PFNGLBINDFRAMEBUFFERPROC			__glBindFramebuffer;
extern PFNGLBUFFERDATAARBPROC			__glBufferData;
extern PFNGLBUFFERSUBDATAARBPROC		__glBufferSubData;
extern PFNGLGENBUFFERSARBPROC			__glGenBuffers;
extern PFNGLGENFRAMEBUFFERSEXTPROC		__glGenFramebuffers;
extern PFNGLGENRENDERBUFFERSEXTPROC		__glGenRenderbuffers;
extern PFNGLDELETEBUFFERSARBPROC		__glDeleteBuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC		__glDeleteFramebuffers;
extern PFNGLDELETERENDERBUFFERSPROC		__glDeleteRenderbuffers;
extern PFNGLMAPBUFFERARBPROC			__glMapBuffer;
extern PFNGLUNMAPBUFFERARBPROC			__glUnmapBuffer;

#define glBindBuffer		__glBindBuffer
#define glBindTexture		__glBindTexture
#define glBindFramebuffer	__glBindFramebuffer

#define glBufferData		__glBufferData
#define glBufferSubData		__glBufferSubData

#define glGenBuffers		__glGenBuffers
#define glGenFramebuffers	__glGenFramebuffers
#define glGenRenderbuffers	__glGenRenderbuffers

#define glDeleteBuffers			__glDeleteBuffers
#define glDeleteFramebuffers	__glDeleteFramebuffers
#define glDeleteRenderbuffers	__glDeleteRenderbuffers

#define glMapBuffer		__glMapBuffer
#define glUnmapBuffer	__glUnmapBuffer

#ifdef __cplusplus
}
#endif

bool InitGLExtensions( const int p_Major, const int p_Minor );

#endif

