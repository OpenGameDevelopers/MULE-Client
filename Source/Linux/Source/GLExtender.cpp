#include <GLExtender.hpp>
#include <iostream>
#include <cstring>
#include <cstdlib>

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

template < typename t_GLFunc >
bool InitGLExtension( const char *p_pExtension, t_GLFunc p_Function )
{
	bool Failed = false;

	Failed = ( ( p_Function = ( t_GLFunc )glXGetProcAddressARB(
		( const GLubyte * ) p_pExtension ) ) == NULL );
	
	if( Failed )
	{
		std::cout << "Failed to bind " << p_pExtension << std::endl;
		char tmpbuf[ 1024 ];
		memset( tmpbuf, '\0', sizeof( tmpbuf ) );
		sprintf( tmpbuf, "0x%p", p_Function );
		std::cout << "Function pointer: " << tmpbuf << std::endl;
	}

	return Failed;
}

bool InitGLExtensions( )
{
	bool Ret = false;

	Ret |= InitGLExtension( "glBindBufferARB", __glBindBuffer );
	Ret |= InitGLExtension( "glBindTexture", __glBindTexture );
	Ret |= InitGLExtension( "glBindFrameBuffer", __glBindFrameBuffer );
	Ret |= InitGLExtension( "glBufferDataARB", __glBufferData );
	Ret |= InitGLExtension( "glBufferSubDataARB", __glBufferSubData );
	Ret |= InitGLExtension( "glGenBuffersARB", __glGenBuffers );
	Ret |= InitGLExtension( "glGenFrameBuffers", __glGenFrameBuffers );
	Ret |= InitGLExtension( "glGenRenderBuffers", __glGenRenderBuffers );
	Ret |= InitGLExtension( "glDeleteBuffersARB", __glDeleteBuffers );
	Ret |= InitGLExtension( "glDeleteFrameBuffers", __glDeleteFrameBuffers );
	Ret |= InitGLExtension( "glDeleteRenderBuffers", __glDeleteRenderBuffers );
	Ret |= InitGLExtension( "glMapBufferARB", __glMapBuffer );
	Ret |= InitGLExtension( "glUnmapBufferARB", __glUnmapBuffer );

	return Ret;
}

