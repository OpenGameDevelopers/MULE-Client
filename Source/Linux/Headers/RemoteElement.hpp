#ifndef __MULECLIENT_REMOTEELEMENT_HPP__
#define __MULECLIENT_REMOTEELEMENT_HPP__

#include <UIElement.hpp>
#include <GLExtender.hpp>

class RemoteElement : public UIElement
{
public:
	explicit RemoteElement( const int p_Width, const int p_Height );
	virtual ~RemoteElement( );

	virtual int Initialise( );
	virtual void Destroy( );

private:
	GLuint	m_TextureID;
};

#endif

