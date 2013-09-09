#include <GLExtender.hpp>

PFNGLBINDBUFFERARBPROC			__glBindBuffer			= NULL;
PFNGLBINDTEXTUREEXTPROC			__glBindTexture			= NULL;
PFNGLBINDFRAMEBUFFERPROC		__glBindFrameBuffer		= NULL;

PFNGLBUFFERDATAARBPROC			__glBufferData			= NULL;
PFNGLBUFFERSUBDATAARBPROC		__glBufferSubData		= NULL;

PFNGLGENBUFFERSARBPROC			__glGenBuffers			= NULL;
PFNGLGENFRAMEBUFFERSEXTPROC		__glGenFrameBuffers		= NULL;
PFNGLGENRENDERBUFFERSEXTPROC	__glGenRenderBuffers	= NULL;

PFNGLDELETEBUFFERSARBPROC		__glDeleteBuffers		= NULL;
PFNGLDELETEFRAMEBUFFERSPROC		__glDeleteFrameBuffers	= NULL;
PFNGLDELETERENDERBUFFERSPROC	__glDeleteRenderBuffers	= NULL;

PFNGLMAPBUFFERARBPROC			__glMapBuffer			= NULL;
PFNGLUNMAPBUFFERARBPROC			__glUnmapBuffer			= NULL;

bool InitGLExtensions( )
{
	bool Ret = false;

	Ret = ( ( __glBindBuffer =
		( PFNGLBINDBUFFERARBPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glBindBufferARB" ) ) == NULL ) || Ret;
	Ret = ( ( __glBindTexture =
		( PFNGLBINDTEXTUREEXTPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glBindTexture" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glBindFrameBuffer =
		( PFNGLBINDFRAMEBUFFEREXTPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glBindFrameBuffer" ) ) == NULL ) || Ret;

	Ret = ( ( __glBufferData =
		( PFNGLBUFFERDATAARBPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glBufferDataARB" ) ) == NULL ) || Ret;

	Ret = ( ( __glBufferSubData =
		( PFNGLBUFFERSUBDATAARBPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glBufferSubDataARB" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glGenBuffers =
		( PFNGLGENBUFFERSARBPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glGenBuffersARB" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glGenFrameBuffers =
		( PFNGLGENFRAMEBUFFERSEXTPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glGenFrameBuffers" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glDeleteBuffers =
		( PFNGLDELETEBUFFERSARBPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glDeleteBuffersARB" ) ) == NULL ) || Ret;

	Ret = ( ( __glDeleteFrameBuffers =
		( PFNGLDELETEFRAMEBUFFERSPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glDeleteFrameBuffers" ) ) == NULL ) || Ret;

	Ret = ( ( __glGenRenderBuffers =
		( PFNGLGENRENDERBUFFERSEXTPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glGenRenderBuffers" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glMapBuffer =
		( PFNGLMAPBUFFERARBPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glMapBufferARB" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glUnmapBuffer =
		( PFNGLUNMAPBUFFERARBPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glUnmapBufferARB" ) ) == NULL ) || Ret;

	return Ret;
}

