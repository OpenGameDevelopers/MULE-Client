#include <EditorViewport.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QFont>
#include <QOpenGLShaderProgram>
#include <QtGui/QMatrix4x4>
#include <QOpenGLFunctions>
#include <Utility.h>

static const char *pVertexShaderSource =
{
	"attribute highp vec4 a_Position;\n"
	"attribute lowp vec4 a_Colour;\n"
	"varying lowp vec4 f_Colour;\n"
	"uniform highp mat4 u_Matrix;\n"
	"void main( )\n"
	"{\n"
	"	f_Colour = a_Colour;\n"
	"	gl_Position = u_Matrix * a_Position;\n"
	"}\n"
};

static const char *pFragmentShaderSource =
{
	"varying lowp vec4 f_Colour;\n"
	"void main( )\n"
	"{\n"
	"	gl_FragColor = f_Colour;\n"
	"}\n"
};

EditorViewport::EditorViewport( QWidget *p_pParent ) :
	QWidget( p_pParent ),
	m_pFramebuffer( nullptr ),
	m_Type( ViewportOrthographic ),
	m_Side( ViewportXY ),
	m_RedClear( 0.13f ),
	m_GreenClear( 0.0f ),
	m_BlueClear( 0.13f ),
	m_Zoom( -2.0f ),
	m_Panning( false ),
	m_PanX( 0.0f ),
	m_PanY( 0.0f ),
	m_StoredPanX( 0.0f ),
	m_StoredPanY( 0.0f ),
	m_Scale( 1.0f )
{
}

EditorViewport::~EditorViewport( )
{
}

int EditorViewport::Create( const ViewportType p_Type,
	QOpenGLFunctions * const &p_pGLFunctions )
{
	m_pGLFunctions = p_pGLFunctions;
	m_pFramebuffer = new QOpenGLFramebufferObject( size( ), GL_TEXTURE_2D );

	m_Type = p_Type;

	m_pProgram = new QOpenGLShaderProgram( this );
	m_pProgram->addShaderFromSourceCode( QOpenGLShader::Vertex,
		pVertexShaderSource );
	m_pProgram->addShaderFromSourceCode( QOpenGLShader::Fragment,
		pFragmentShaderSource );
	m_pProgram->link( );
	m_PositionAttribute = m_pProgram->attributeLocation( "a_Position" );
	m_ColourAttribute = m_pProgram->attributeLocation( "a_Colour" );
	m_MatrixUniform = m_pProgram->uniformLocation( "u_Matrix" );

	setMouseTracking( true );

	return 0;
}

int EditorViewport::Resize( const int p_Width, const int p_Height )
{
	if( ( p_Width <= 0 ) || ( p_Height <= 0 ) )
	{
		return 1;
	}

	if( ( p_Width == m_Size.width( ) ) && ( p_Height == m_Size.height( ) ) )
	{
		return 1;
	}

	m_pFramebuffer->release( );
	delete m_pFramebuffer;
	m_pFramebuffer = nullptr;

	m_pFramebuffer = new QOpenGLFramebufferObject( p_Width, p_Height,
		GL_TEXTURE_2D );

	return 0;
}

int EditorViewport::SetType( const ViewportType p_Type )
{
	m_Type = p_Type;

	return 0;
}

int EditorViewport::SetClearColour( const float p_Red, const float p_Green,
	const float p_Blue )
{
	m_RedClear = p_Red;
	m_GreenClear = p_Green;
	m_BlueClear = p_Blue;

	return 0;
}

int EditorViewport::GetImage( QImage *p_pImage )
{
	( *p_pImage ) = m_pFramebuffer->toImage( );

	return 0;
}

void EditorViewport::Activate( )
{
	m_pFramebuffer->bind( );
}

void EditorViewport::Deactivate( )
{
	m_pFramebuffer->release( );
}

void EditorViewport::Render( )
{
	glViewport( 0, 0, width( ), height( ) );
	// The following should be replaced with rendering the JPEG image received
	// from the server
	glClearColor( m_RedClear, m_GreenClear, m_BlueClear, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	m_pProgram->bind( );

	RecreateProjectionMatrix( );

	m_pProgram->setUniformValue( m_MatrixUniform, m_ProjectionMatrix );

	GLfloat Vertices[ ] =
	{
		-1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f
	};

	GLfloat Colours[ ] =
	{
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};

	m_pGLFunctions->glVertexAttribPointer( m_PositionAttribute, 2, GL_FLOAT,
		GL_FALSE, 0, Vertices );
	m_pGLFunctions->glVertexAttribPointer( m_ColourAttribute, 3, GL_FLOAT,
		GL_FALSE, 0, Colours );
	
	m_pGLFunctions->glEnableVertexAttribArray( 0 );
	m_pGLFunctions->glEnableVertexAttribArray( 1 );

	glDrawArrays( GL_LINES, 0, 4 );

	m_pGLFunctions->glDisableVertexAttribArray( 1 );
	m_pGLFunctions->glDisableVertexAttribArray( 0 );

	m_pProgram->release( );
}

void EditorViewport::RecreateProjectionMatrix( )
{
	m_ProjectionMatrix.setToIdentity( );
	if( m_Type == ViewportPerspective )
	{
		m_ProjectionMatrix.perspective( 60,
			static_cast< float >( size( ).width( ) ) /
				static_cast< float >( size( ).height( ) ), 0.1f, 100.0f );

		m_ProjectionMatrix.translate( m_PanX, m_PanY, m_Zoom );
	}
	else if( m_Type == ViewportOrthographic )
	{
		if( size( ).width( ) > size( ).height( ) )
		{
			float Factor = static_cast< float >( size( ).width( ) ) /
				static_cast< float >( size( ).height( ) );
			m_ProjectionMatrix.ortho( -Factor, Factor, -1.0f, 1.0f,
				0.01f, 10000.0f );
		}
		else if( size( ).width( ) == size( ).height( ) )
		{
			m_ProjectionMatrix.ortho( -1.0f, 1.0f, -1.0f, 1.0f,
				0.01f, 10000.0f );
		}
		else
		{
			float Factor = static_cast< float >( size( ).height( ) ) /
				static_cast< float >( size( ).width( ) );
			m_ProjectionMatrix.ortho( -1.0f, 1.0f, -Factor, Factor,
				0.001f, 10000.0f );
		}
		m_ProjectionMatrix.scale( m_Scale );
		m_ProjectionMatrix.translate( m_PanX, m_PanY, -1.0f );
	}
}

void EditorViewport::paintEvent( QPaintEvent *p_pPaintEvent )
{
	this->Activate( );
	this->SetClearColour( 0.12f, 0.12f, 0.2f );
	this->Render( );
	this->Deactivate( );

	QPainter Painter( this );
	QPoint Zero( 0, 0 );
	QRect Rectangle( Zero, size( ) );

	Painter.drawImage( Rectangle, m_pFramebuffer->toImage( ) );

	QFont Qfont = Painter.font( );
	QFontMetrics FontMetrics( Qfont );
	Qfont.setPointSize( 8 );
	Painter.setFont( Qfont );
	Painter.setPen( Qt::white );
	Painter.drawText( QPoint( 0, FontMetrics.lineSpacing( ) ),
		GetNameFromViewport( m_Type, m_Side ) );
}

void EditorViewport::resizeEvent( QResizeEvent *p_pResizeEvent )
{
	m_pFramebuffer->release( );
	SafeDelete( m_pFramebuffer );
	m_pFramebuffer = new QOpenGLFramebufferObject( size( ), GL_TEXTURE_2D );
}

void EditorViewport::wheelEvent( QWheelEvent *p_pWheelEvent )
{
	m_Zoom += static_cast< float >( p_pWheelEvent->angleDelta( ).y( ) ) *
		0.01f; 

	if( p_pWheelEvent->angleDelta( ).y( ) > 0 )
	{
		m_Scale += 0.01f;
	}
	else
	{
		m_Scale -= 0.01f;
	}

	if( m_Scale < 0.01f )
	{
		m_Scale = 0.01f;
	}

	update( );
}

void EditorViewport::mousePressEvent( QMouseEvent *p_pMouseEvent )
{
	if( m_Panning )
	{

	}
	else
	{
		m_MousePosition = p_pMouseEvent->pos( );
		m_Panning = true;
	}

	update( );
}

void EditorViewport::mouseReleaseEvent( QMouseEvent *p_pMouseEvent )
{
	m_Panning = false;
	m_StoredPanX = m_PanX;
	m_StoredPanY = m_PanY;
	update( );
}

void EditorViewport::mouseMoveEvent( QMouseEvent *p_pMouseEvent )
{
	if( m_Panning )
	{
		QPoint NewPosition = p_pMouseEvent->pos( );

		NewPosition -= m_MousePosition;

		float PanXDelta = static_cast< float >( NewPosition.x( ) ) * 0.01f;
		float PanYDelta = static_cast< float >( NewPosition.y( ) ) * -0.01f;

		m_PanX = PanXDelta + m_StoredPanX;
		m_PanY = PanYDelta + m_StoredPanY;
	}

	update( );
}

char *GetNameFromViewport( const ViewportType p_Type,
	const ViewportSide p_Side )
{
	char Message[ 1024 ];
	memset( Message, '\0', sizeof( Message ) );

	switch( p_Type )
	{
		case ViewportOrthographic:
		{
			strcat( Message, "Orthographic" );
			break;
		}
		case ViewportPerspective:
		{
			strcat( Message, "Perspective" );
			break;
		}
		default:
		{
			strcat( Message, "Unknown Projection" );
		}
	}

	switch( p_Side )
	{
		case ViewportXZ:
		{
			strcat( Message, " [Top]" );
			break;
		}
		case ViewportXY:
		{
			strcat( Message, " [Front]" );
			break;
		}
		case ViewportYZ:
		{
			strcat( Message, " [Side]" );
			break;
		}
		case ViewportFreeMoving:
		{
			strcat( Message, " [Free]" );
			break;
		}
		default:
		{
			strcat( Message, " [Unknown View]" );
		}
	}

	return Message;
}

