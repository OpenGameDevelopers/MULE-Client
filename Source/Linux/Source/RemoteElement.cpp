#include <RemoteElement.hpp>

RemoteDisplayElement::RemoteDisplayElement( const int p_Width, const int p_Height )
{
	m_Dimension.Width = p_Width;
	m_Dimension.Height = p_Height;

	m_TextureID = 0;
}

RemoteDisplayElement::~RemoteDisplayElement( )
{
}

int RemoteDisplayElement::Initialise( )
{
	glGenTextures( 1, &m_TextureID );
	glGenFrameBuffers( 1, &m_FBO );
	glGenRenderBuffers( 1, &m_RBO );

	return 1;
}

void RemoteDisplayElement::Destroy( )
{
	glDeleteFrameBuffers( 1, &m_FBO );
	glDeleteRenderBuffers( 1, &m_RBO );
	glDeleteTextures( 1, &m_TextureID );
}

void Render( )
{
}

