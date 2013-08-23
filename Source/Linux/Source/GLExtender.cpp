#include <GLExtender.hpp>

PFNGLBINDTEXTUREEXTPROC			__glBindTexture			= NULL;
PFNGLBINDFRAMEBUFFERPROC		__glBindFrameBuffer		= NULL;
PFNGLGENFRAMEBUFFERSEXTPROC		__glGenFrameBuffers		= NULL;
PFNGLGENRENDERBUFFERSEXTPROC	__glGenRenderBuffers	= NULL;
PFNGLDELETEFRAMEBUFFERSPROC		__glDeleteFrameBuffers	= NULL;
PFNGLDELETERENDERBUFFERSPROC	__glDeleteRenderBuffers	= NULL;

bool InitGLExtensions( )
{
	bool Ret = false;

	Ret = ( ( __glBindTexture =
		( PFNGLBINDTEXTUREEXTPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glBindTexture" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glBindFrameBuffer =
		( PFNGLBINDFRAMEBUFFEREXTPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glBindFrameBuffer" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glGenFrameBuffers =
		( PFNGLGENFRAMEBUFFERSEXTPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glGenFrameBuffers" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glGenRenderBuffers =
		( PFNGLGENRENDERBUFFERSEXTPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glGenRenderBuffers" ) ) == NULL ) || Ret;
	
	Ret = ( ( __glDeleteFrameBuffers =
		( PFNGLDELETEFRAMEBUFFERSPROC )glXGetProcAddressARB(
			( const GLubyte * ) "glDeleteFrameBuffers" ) ) == NULL ) || Ret;

	return Ret;
}

