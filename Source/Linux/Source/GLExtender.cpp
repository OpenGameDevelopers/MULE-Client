#include <GLExtender.hpp>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <list>

PFNGLBINDBUFFERARBPROC			__glBindBuffer			= NULL;
PFNGLBINDTEXTUREEXTPROC			__glBindTexture			= NULL;
PFNGLBINDFRAMEBUFFERPROC		__glBindFramebuffer		= NULL;

PFNGLBUFFERDATAARBPROC			__glBufferData			= NULL;
PFNGLBUFFERSUBDATAARBPROC		__glBufferSubData		= NULL;

PFNGLGENBUFFERSARBPROC			__glGenBuffers			= NULL;
PFNGLGENFRAMEBUFFERSEXTPROC		__glGenFramebuffers		= NULL;
PFNGLGENRENDERBUFFERSEXTPROC	__glGenRenderbuffers	= NULL;

PFNGLDELETEBUFFERSARBPROC		__glDeleteBuffers		= NULL;
PFNGLDELETEFRAMEBUFFERSPROC		__glDeleteFramebuffers	= NULL;
PFNGLDELETERENDERBUFFERSPROC	__glDeleteRenderbuffers	= NULL;

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

			Ret = InitGLExtension( "glBindTexture", __glBindTexture );

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
			Ret = InitGLExtension( "glBindFramebuffer", __glBindFramebuffer );
			Ret = InitGLExtension( "glGenFramebuffers", __glGenFramebuffers );
			Ret = InitGLExtension( "glGenRenderbuffers",
				__glGenRenderbuffers );
			Ret = InitGLExtension( "glDeleteFramebuffers",
				__glDeleteFramebuffers );
			Ret = InitGLExtension( "glDeleteRenderbuffers",
				__glDeleteRenderbuffers );

			if( ( !__glBindFramebuffer ) || ( !__glGenFramebuffers ) ||
				( !__glGenRenderbuffers ) ||
				( !__glDeleteFramebuffers ) || ( !__glDeleteRenderbuffers ) )
			{
				std::cout << "[ERROR]" << std::endl;
				if( !__glBindFramebuffer )
				{
					std::cout << "\tFailed to bind glBindFramebuffer" <<
						std::endl;
				}
				if( !__glGenFramebuffers )
				{
					std::cout << "\tFailed to bind glGenFramebuffers" <<
						std::endl;
				}
				if( !__glDeleteFramebuffers )
				{
					std::cout << "\tFailed to bind glDeleteFramebuffers" <<
						std::endl;
				}
				if( !__glDeleteRenderbuffers )
				{
					std::cout << "\tFailed to bind glDeleteRenderbuffers" <<
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
			Ret = InitGLExtension( "glBindBufferARB", __glBindBuffer );
			Ret = InitGLExtension( "glBufferDataARB", __glBufferData );
			Ret = InitGLExtension( "glBufferSubDataARB", __glBufferSubData );
			Ret = InitGLExtension( "glDeleteBuffersARB", __glDeleteBuffers );
			Ret = InitGLExtension( "glGenBuffersARB", __glGenBuffers );
			Ret = InitGLExtension( "glMapBufferARB", __glMapBuffer );
			Ret = InitGLExtension( "glUnmapBufferARB", __glUnmapBuffer );
		
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

