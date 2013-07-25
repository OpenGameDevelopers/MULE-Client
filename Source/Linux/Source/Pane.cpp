#include <Pane.hpp>
#include <iostream>
#include <cstring>
#include <X11/Xatom.h>

Pane::Pane( )
{
	m_pDisplay = NULL;
	m_pVisualInfo = NULL;
	m_GLVersion[ 0 ] = 0;
	m_GLVersion[ 1 ] = 0;
	m_GLXContext = 0;
	m_FBConfig = 0;
	m_Width = 0;
	m_Height = 0;
}

Pane::~Pane( )
{
	this->Destroy( );
}

int Pane::Initialise( )
{
	m_pDisplay = XOpenDisplay( "" );

	if( !m_pDisplay )
	{
		std::cout << "Failed to open display" << std::endl;
		return 0;
	}

	int VisualAttributes[ ] =
	{
		GLX_X_RENDERABLE,	True,
		GLX_DRAWABLE_TYPE,	GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,	GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE,	GLX_TRUE_COLOR,
		GLX_RED_SIZE,		8,
		GLX_GREEN_SIZE,		8,
		GLX_BLUE_SIZE,		8,
		GLX_ALPHA_SIZE,		8,
		GLX_DEPTH_SIZE,		24,
		GLX_STENCIL_SIZE,	8,
		GLX_DOUBLEBUFFER,	True,
		None
	};

	int GLXMajor = 0, GLXMinor = 0;
	if( !glXQueryVersion( m_pDisplay, &GLXMajor, &GLXMinor ) )
	{
		std::cout << "Failed to query GLX version" << std::endl;
		return 0;
	}

	int FBCount = 0;
	GLXFBConfig *pFBC = glXChooseFBConfig( m_pDisplay,
		DefaultScreen( m_pDisplay ), VisualAttributes, &FBCount );
	GLXFBConfig FBConfig = pFBC[ 0 ];
	XFree( pFBC );
	pFBC = NULL;

	m_pVisualInfo = glXGetVisualFromFBConfig( m_pDisplay, FBConfig );

	XSetWindowAttributes WindowAttributes;

	WindowAttributes.colormap = XCreateColormap( m_pDisplay,
		RootWindow( m_pDisplay, m_pVisualInfo->screen ), m_pVisualInfo->visual,
		AllocNone );
	WindowAttributes.border_pixel = 0;
	WindowAttributes.event_mask = StructureNotifyMask | ExposureMask |
		KeyPressMask | KeyReleaseMask | PointerMotionMask | FocusChangeMask |
		EnterWindowMask | LeaveWindowMask| ButtonPressMask | ButtonReleaseMask;
	
	m_Window = XCreateWindow( m_pDisplay,
		RootWindow( m_pDisplay, m_pVisualInfo->screen ), 0, 0, 640, 480, 0,
		m_pVisualInfo->depth, InputOutput, m_pVisualInfo->visual,
		CWEventMask | CWColormap | CWBorderPixel,
		&WindowAttributes );
	
	XMapWindow( m_pDisplay, m_Window );
	XMapRaised( m_pDisplay, m_Window );
	XMoveWindow( m_pDisplay, m_Window, 0, 0 );
	XRaiseWindow( m_pDisplay, m_Window );

	m_GLXContext = glXCreateContext( m_pDisplay, m_pVisualInfo, 0, True );
	glXMakeCurrent( m_pDisplay, m_Window, m_GLXContext );

	char *pGLVersion = ( char * )glGetString( GL_VERSION );
	char *pTokenVersion = strtok( pGLVersion, ". " );

	for( int i = 0; i < 2; ++i )
	{
		m_GLVersion[ i ] = atoi( pTokenVersion );
		pTokenVersion = strtok( NULL, ". " );
	}

	std::cout << "OpenGL version: " << m_GLVersion[ 0 ] << "." <<
		m_GLVersion[ 1 ] << std::endl;

	if( ( m_GLVersion[ 0 ] < 1 ) ||
		( ( m_GLVersion[ 0 ] == 1 ) && ( m_GLVersion[ 1 ] < 1 ) ) )
	{
		std::cout << "Failed to create an OpenGL 1.1 context" << std::endl;
		return 0;
	}

	XTextProperty Text;
	char *List[ 1 ] = { "MULE - Client\0" };

	XmbTextListToTextProperty( m_pDisplay, List, 1, XStringStyle, &Text );

	XSetWMName( m_pDisplay, m_Window, &Text );

	XEvent Event;
	Atom State = XInternAtom( m_pDisplay, "_NET_WM_STATE", False );
	Atom MaxHorz = XInternAtom( m_pDisplay, "_NET_WM_STATE_MAXIMIZED_HORZ",
		False );
	Atom MaxVert = XInternAtom( m_pDisplay, "_NET_WM_STATE_MAXIMIZED_VERT",
		False );
	
	memset( &Event, 0, sizeof( Event ) );
	Event.type = ClientMessage;
	Event.xclient.window = m_Window;
	Event.xclient.message_type = State;
	Event.xclient.format = 32;
	Event.xclient.data.l[ 0 ] = 1; //_NET_WM_STATE_ADD;
	Event.xclient.data.l[ 1 ] = MaxHorz;
	Event.xclient.data.l[ 2 ] = MaxVert;

	XSendEvent( m_pDisplay, DefaultRootWindow( m_pDisplay ), False,
		SubstructureNotifyMask, &Event );

	glClearColor( 0.20f, 0.0f, 0.0f, 1.0f );

	return 1;
}

void Pane::Destroy( )
{
	if( m_pDisplay )
	{
		glXMakeCurrent( m_pDisplay, 0, 0 );

		if( m_GLXContext )
		{
			glXDestroyContext( m_pDisplay, m_GLXContext );
		}
	}

	if( m_pVisualInfo )
	{
		XFree( m_pVisualInfo );
	}

	if( m_Window )
	{
		XDestroyWindow( m_pDisplay, m_Window );
	}

	if( m_pDisplay )
	{
		XCloseDisplay( m_pDisplay );
	}
}

void Pane::Update( )
{
	static Atom DeleteWindow;
	DeleteWindow = XInternAtom( m_pDisplay, "WM_DELETE_WINDOW", False );
	XSetWMProtocols( m_pDisplay, m_Window, &DeleteWindow, 1 );
	glXSwapBuffers( m_pDisplay, m_Window );
	bool quit = false;

	XEvent Event;

	while( !quit )
	{
		int Pending = XPending( m_pDisplay );
		for( int i = 0; i < Pending; ++i )
		{
			XNextEvent( m_pDisplay, &Event );

			switch( Event.type )
			{
				case Expose:
				{
					if( Event.xexpose.count != 0 )
					{
						break;
					}

					this->Render( );

					break;
				}
				case ConfigureNotify:
				{
					glViewport( 0, 0, Event.xconfigure.width,
						Event.xconfigure.height );

					break;
				}
				case ClientMessage:
				{
					if( ( Atom )Event.xclient.data.l[ 0 ] == DeleteWindow )
					{
						quit = true;
						break;
					}
				}
				default:
				{
					break;
				}
			}
		}

		this->Render( );
	}
}

void Pane::Render( )
{
	glClear( GL_COLOR_BUFFER_BIT );

	// Render all attached views

	glXSwapBuffers( m_pDisplay, m_Window );
}

