#include <RemoteElement.hpp>

RemoteElement::RemoteElement( const int p_Width, const int p_Height )
{
	m_Dimension.Width = p_Width;
	m_Dimension.Height = p_Height;

	m_TextureID = 0;
}

RemoteElement::~RemoteElement( )
{
}

int RemoteElement::Initialise( )
{
	glGenTextures( 1, m_TextureID );

	return 1;
}

