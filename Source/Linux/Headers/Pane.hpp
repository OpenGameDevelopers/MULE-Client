#ifndef __MULECLIENT_PANE_HPP__
#define __MULECLIENT_PANE_HPP__

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glext.h>

class RemoteDisplayElement;

class Pane
{
public:
	Pane( );
	~Pane( );

	int Initialise( );
	void Destroy( );

	void Update( );
	void Render( );

private:
	Display	*m_pDisplay;
	Window	m_Window;

	GLXFBConfig	m_FBConfig;
	GLXContext	m_GLXContext;
	XVisualInfo	*m_pVisualInfo;
	GLint		m_GLVersion[ 2 ];

	// Temporarily use the raw RDE instead of determining them via their
	// UIElement
	RemoteDisplayElement	*m_pElements;
	int						m_ElementCount;

	int		m_Width;
	int		m_Height;
};

#endif

