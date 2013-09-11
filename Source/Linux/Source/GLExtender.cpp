#include <GLExtender.hpp>
#include <list>
#include <string>
#include <cstring>
#include <iostream>

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

bool InitGLExtensions( const int p_Major, const int p_Minor )
{
	bool Ret = false;

	std::cout << "OpenGL extensions being bound" << std::endl;
	std::cout << "-----------------------------" << std::endl;

	const char *pGLExtensions = ( const char * )glGetString( GL_EXTENSIONS );

	std::list< std::string > Extensions;
	std::string Extension;

	size_t ExtLen = strlen( pGLExtensions );
	size_t LongestExtName = 0;

	for( size_t i = 0; i < ExtLen; ++i )
	{
		if( pGLExtensions[ i ] == ' ' )
		{
			Extension.append( "\0" );
			Extensions.push_front( Extension );
			if( Extension.size( ) > LongestExtName )
			{
				LongestExtName = Extension.size( );
			}
			Extension.clear( );
			continue;
		}
		Extension.append( 1, pGLExtensions[ i ] );
	}

	if( p_Major >= 1 )
	{
		if( p_Minor >= 1 )
		{
			std::string Padding;
			Padding.append( LongestExtName-4, ' ' );

			std::cout << "Binding OpenGL 1.1 core API functions" << Padding;

			Ret = ( ( __glBindTexture =
				( PFNGLBINDTEXTUREEXTPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glBindTexture" ) ) == NULL ) || Ret;

			if( ( !__glBindTexture ) )
			{
				std::cout << "[ERROR]" << std::endl;
				if( !glBindTexture )
				{
					std::cout << "Failed to bind glBindTexture" << std::endl;
				}
			}
			else
			{
				std::cout << "[OK]" << std::endl;
			}
		}
	}

	std::cout << std::endl;
	std::list< std::string >::iterator Itr = Extensions.begin( );
	std::cout << "Found " << Extensions.size( ) << " extensions" << std::endl;

	for( ; Itr != Extensions.end( ); ++Itr )
	{
		size_t Padding = LongestExtName - ( *Itr ).size( );
		std::string StrPadding;
		StrPadding.clear( );
		StrPadding.append( Padding, ' ' );
		std::cout << "Binding OpenGL functions for " << ( *Itr ) << "    " <<
			StrPadding;

		if( ( *Itr ).compare( "GL_ARB_framebuffer_object" ) == 0 )
		{
			Ret = ( ( __glBindFrameBuffer =
			( PFNGLBINDFRAMEBUFFEREXTPROC )glXGetProcAddressARB(
				( const GLubyte * ) "glBindFrameBuffer" ) ) == NULL ) || Ret;

			Ret = ( ( __glGenFrameBuffers =
				( PFNGLGENFRAMEBUFFERSEXTPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glGenFrameBuffers" ) ) == NULL ) ||
						Ret;
			Ret = ( ( __glGenRenderBuffers =
				( PFNGLGENRENDERBUFFERSEXTPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glGenRenderBuffers" ) ) == NULL ) ||
						Ret;

			Ret = ( ( __glDeleteFrameBuffers =
				( PFNGLDELETEFRAMEBUFFERSPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glDeleteFrameBuffers" ) ) == NULL ) ||
						Ret;
			Ret = ( ( __glDeleteRenderBuffers =
				( PFNGLDELETERENDERBUFFERSPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glDeleteRenderBuffers" ) )
						== NULL ) || Ret;

			if( ( !__glBindFrameBuffer ) || ( !__glGenFrameBuffers ) ||
				( !__glDeleteFrameBuffers ) || ( !__glDeleteRenderBuffers ) )
			{
				std::cout << "[ERROR]" << std::endl;
				if( !__glBindFrameBuffer )
				{
					std::cout << "\tFailed to bind glBindFrameBuffer" <<
						std::endl;
				}
				if( !__glGenFrameBuffers )
				{
					std::cout << "\tFailed to bind glGenFrameBuffers" <<
						std::endl;
				}
				if( !__glDeleteFrameBuffers )
				{
					std::cout << "\tFailed to bind glDeleteFrameBuffers" <<
						std::endl;
				}
				if( !__glDeleteRenderBuffers )
				{
					std::cout << "\tFailed to bind glDeleteRenderBuffers" <<
						std::endl;
				}
			}
			else
			{
				std::cout << "[OK]" << std::endl;
			}
		}
		else if( ( *Itr ).compare( "GL_ARB_vertex_buffer_object" ) == 0 )
		{
			Ret = ( ( __glBindBuffer =
				( PFNGLBINDBUFFERARBPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glBindBufferARB" ) ) == NULL ) || Ret;

			Ret = ( ( __glBufferData =
				( PFNGLBUFFERDATAARBPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glBufferDataARB" ) ) == NULL ) || Ret;

			Ret = ( ( __glBufferSubData =
				( PFNGLBUFFERSUBDATAARBPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glBufferSubDataARB" ) ) == NULL ) ||
						Ret;

			Ret = ( ( __glDeleteBuffers =
				( PFNGLDELETEBUFFERSARBPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glDeleteBuffersARB" ) ) == NULL ) ||
						Ret;

			Ret = ( ( __glGenBuffers =
				( PFNGLGENBUFFERSARBPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glGenBuffersARB" ) ) == NULL ) || Ret;

			Ret = ( ( __glMapBuffer =
				( PFNGLMAPBUFFERARBPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glMapBufferARB" ) ) == NULL ) || Ret;
			
			Ret = ( ( __glUnmapBuffer =
				( PFNGLUNMAPBUFFERARBPROC )glXGetProcAddressARB(
					( const GLubyte * ) "glUnmapBufferARB" ) ) == NULL ) ||
						Ret;

			if( ( !__glBindBuffer ) || ( !__glBufferData ) ||
				( !__glBufferSubData ) || ( !__glDeleteBuffers ) ||
				( !__glGenBuffers ) || ( !__glMapBuffer ) ||
				( !__glUnmapBuffer ) )
			{
				std::cout << "[ERROR]" << std::endl;

				if( !__glBindBuffer )
				{
					std::cout << "\tFailed to bind glBindBuffer" << std::endl;
				}
				if( !__glBufferData )
				{
					std::cout << "\tFailed to bind glBufferData" << std::endl;
				}
				if( !__glBufferSubData )
				{
					std::cout << "\tFailed to bind glBufferSubData" <<
						std::endl;
				}
				if( !__glDeleteBuffers )
				{
					std::cout << "\tFailed to bind glDeleteBuffers" <<
						std::endl;
				}
				if( !__glGenBuffers )
				{
					std::cout << "\tFailed to bind glGenBuffers" << std::endl;
				}
				if( !__glMapBuffer )
				{
					std::cout << "\tFailed to bind glMapBuffer" << std::endl;
				}
				if( !__glUnmapBuffer )
				{
					std::cout << "\tFailed to bind glUnmapBuffer" << std::endl;
				}
			}
			else
			{
				std::cout << "[OK]" << std::endl;
			}
		}
		else
		{
			std::cout << "[UNBOUND]" << std::endl;
		}
	}

	return Ret;
}

