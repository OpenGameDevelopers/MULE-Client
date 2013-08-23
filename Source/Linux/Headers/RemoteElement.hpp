#ifndef __MULECLIENT_REMOTEELEMENT_HPP__
#define __MULECLIENT_REMOTEELEMENT_HPP__

#include <UIElement.hpp>
#include <GLExtender.hpp>

class RemoteDisplayElement : public UIElement
{
public:
	explicit RemoteDisplayElement( const int p_Width, const int p_Height );
	virtual ~RemoteDisplayElement( );

	virtual int Initialise( );
	virtual void Destroy( );

	void Render( );

private:
	GLuint	m_TextureID;
	GLuint	m_FBO;
	GLuint	m_RBO;
};

#endif

