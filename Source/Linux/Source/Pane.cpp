#include <Pane.hpp>
#include <iostream>

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
	WindowAttributes.override_redirect = True;
	WindowAttributes.background_pixmap = None;
	WindowAttributes.border_pixel = 0;
	WindowAttributes.event_mask = StructureNotifyMask | ExposureMask |
		KeyPressMask | KeyReleaseMask | PointerMotionMask | FocusChangeMask |
		ResizeRedirectMask | EnterWindowMask | LeaveWindowMask |
		ButtonPressMask | ButtonReleaseMask;
	
	m_Window = XCreateWindow( m_pDisplay,
		RootWindow( m_pDisplay, m_pVisualInfo->screen ), 0, 0, 640, 480, 0,
		m_pVisualInfo->depth, InputOutput, m_pVisualInfo->visual,
		CWEventMask | CWColormap | CWBorderPixel | CWOverrideRedirect,
		&WindowAttributes );
	
	XMapWindow( m_pDisplay, m_Window );
	XMapRaised( m_pDisplay, m_Window );
	XMoveWindow( m_pDisplay, m_Window, 0, 0 );
	XRaiseWindow( m_pDisplay, m_Window );

	m_GLXContext = glXCreateContext( m_pDisplay, m_pVisualInfo, 0, True );
	glXMakeCurrent( m_pDisplay, m_Window, m_GLXContext );

	glClearColor( 0.20f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glXSwapBuffers( m_pDisplay, m_Window );


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
}

void Pane::Render( )
{
}

