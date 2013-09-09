#ifndef __MULECLIENT_REMOTEELEMENT_HPP__
#define __MULECLIENT_REMOTEELEMENT_HPP__

#include <UIElement.hpp>
#include <GLExtender.hpp>

class RemoteDisplayElement : public UIElement
{
public:
	RemoteDisplayElement( );
	explicit RemoteDisplayElement( const int p_Width, const int p_Height );
	virtual ~RemoteDisplayElement( );

	virtual int Initialise( );
	virtual void Destroy( );

	inline virtual int Type( ) const { return 0x00000004; }

	void Render( );

	inline void Dimensions( const int p_Width, const int p_Height );

private:
	GLuint	m_TextureID;
	GLuint	m_PBO;
	GLubyte	*m_pImageData;

	int		m_Socket;
};

void RemoteDisplayElement::Dimensions( const int p_Width, const int p_Height )
{
	m_Dimension.Width = p_Width;
	m_Dimension.Height = p_Height;
}

#endif

