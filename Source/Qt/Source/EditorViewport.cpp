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
#include <QOpenGLFunctions_3_0>
#include <Utility.h>
#include <jpeglib.h>

#define BUFFER_OFFSET( offset ) ( ( char * )NULL + ( offset ) )

static const char *pVertexShaderSource =
{
	"#version 130\n"
	"in vec2 a_Position;\n"
	"out vec2 f_ST;\n"
	"void main( )\n"
	"{\n"
	"	gl_Position = vec4( a_Position, 0.0, 1.0 );\n"

	"	f_ST = a_Position * 0.5 + 0.5;\n"
	"}\n"
};

static const char *pFragmentShaderSource =
{
	"#version 130\n"
	"in vec2 f_ST;\n"
	"out vec4 o_FragColour;\n"
	"uniform sampler2D u_Texture;\n"
	"void main( )\n"
	"{\n"
	"	o_FragColour = texture2D( u_Texture, f_ST );\n"
	"}\n"
};

unsigned char *g_pJPEGImage;
unsigned char *g_pJPEGImageDecompressed;
unsigned char *g_pRAWImage;
unsigned long g_JPEGBufferLength;
struct jpeg_decompress_struct g_JpegDecompressInfo;

const int CHANNEL_COUNT = 3;

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
	g_pRAWImage = nullptr;
	g_pJPEGImage = nullptr;
	g_pJPEGImageDecompressed = nullptr;
	g_JPEGBufferLength = 0UL;
}

EditorViewport::~EditorViewport( )
{
	if( g_pRAWImage )
	{
		delete [ ] g_pRAWImage;
		g_pRAWImage = nullptr;
	}

	jpeg_destroy_decompress( &g_JpegDecompressInfo );

	if( g_pJPEGImageDecompressed )
	{
		delete [ ] g_pJPEGImageDecompressed;
		g_pJPEGImageDecompressed = nullptr;
	}

	if( g_pJPEGImage )
	{
		delete [ ] g_pJPEGImage;
		g_pJPEGImage = nullptr;
	}
}

int EditorViewport::Create( const ViewportType p_Type,
	QOpenGLFunctions_3_0 * const &p_pGLFunctions )
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
	m_STAttribute = m_pProgram->attributeLocation( "a_ST" );
	m_TextureSamplerUniform = m_pProgram->uniformLocation( "u_Texture" );

	setMouseTracking( true );

	g_pRAWImage = new unsigned char[ width( ) * height( ) * CHANNEL_COUNT ];

	// Create the initial JPEG test image (a green image the same width and
	// height as the viewport, and store it in memory)
	for( int Row = 0; Row < height( ); ++Row )
	{
		for( int Col = 0; Col < width( ) * CHANNEL_COUNT; ++Col )
		{
			g_pRAWImage[ Col + ( Row * width( ) * CHANNEL_COUNT ) ] = 
				( Row % 256 ) & 0xFF;
		}
	}

	// Compress the image, then immediately decompress it
	struct jpeg_compress_struct JpegCompressInfo;
	struct jpeg_error_mgr		JpegError;
	JpegCompressInfo.err = jpeg_std_error( &JpegError );
	jpeg_create_compress( &JpegCompressInfo );
	jpeg_mem_dest( &JpegCompressInfo, &g_pJPEGImage, &g_JPEGBufferLength );
	JpegCompressInfo.image_width = width( );
	JpegCompressInfo.image_height = height( );
	JpegCompressInfo.input_components = CHANNEL_COUNT;
	JpegCompressInfo.in_color_space = JCS_RGB;

	jpeg_set_defaults( &JpegCompressInfo );
	// By compressing to 10, banding in the test image should be visible
	jpeg_set_quality( &JpegCompressInfo, 10, true );
	jpeg_start_compress( &JpegCompressInfo, true );
	JSAMPROW pRow;

	while( JpegCompressInfo.next_scanline < JpegCompressInfo.image_height )
	{
		pRow = ( JSAMPROW )&g_pRAWImage[ JpegCompressInfo.next_scanline *
			CHANNEL_COUNT * width( ) ];
		jpeg_write_scanlines( &JpegCompressInfo, &pRow, 1 );
	}

	jpeg_finish_compress( &JpegCompressInfo );
	jpeg_destroy_compress( &JpegCompressInfo );

	g_JpegDecompressInfo.err = jpeg_std_error( &JpegError );
	jpeg_create_decompress( &g_JpegDecompressInfo );
	jpeg_mem_src( &g_JpegDecompressInfo, g_pJPEGImage, g_JPEGBufferLength );
	if( jpeg_read_header( &g_JpegDecompressInfo, true ) != 1 )
	{
		return 1;
	}

	jpeg_start_decompress( &g_JpegDecompressInfo );

	int JPEGWidth = g_JpegDecompressInfo.output_width;
	int JPEGHeight = g_JpegDecompressInfo.output_height;
	int JPEGPixelSize = g_JpegDecompressInfo.output_components;
	int JPEGTotalSize = JPEGWidth * JPEGHeight * JPEGPixelSize;

	g_pJPEGImageDecompressed = new unsigned char[ JPEGTotalSize ];

	while( g_JpegDecompressInfo.output_scanline <
		g_JpegDecompressInfo.output_height )
	{
		unsigned char *pBuffer[ 1 ];
		pBuffer[ 0 ] = g_pJPEGImageDecompressed +
			( g_JpegDecompressInfo.output_scanline ) *
			JPEGWidth * JPEGPixelSize;
		jpeg_read_scanlines( &g_JpegDecompressInfo, pBuffer, 1  );
	}

	jpeg_finish_decompress( &g_JpegDecompressInfo );

	glGenTextures( 1, &m_TextureID );

	glBindTexture( GL_TEXTURE_2D, m_TextureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width( ), height( ), 0,
		GL_RGB, GL_UNSIGNED_BYTE, ( GLvoid * )g_pJPEGImageDecompressed );
	glBindTexture( GL_TEXTURE_2D, 0 );

	m_pGLFunctions->glGenVertexArrays( 1, &m_VertexArrayObject );
	m_pGLFunctions->glBindVertexArray( m_VertexArrayObject );

	GLfloat VertexBufferData [ ] =
	{
		-1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f,
	};

	m_pGLFunctions->glGenBuffers( 1, &m_VertexBuffer );
	m_pGLFunctions->glBindBuffer( GL_ARRAY_BUFFER, m_VertexBuffer );
	m_pGLFunctions->glBufferData( GL_ARRAY_BUFFER, sizeof( VertexBufferData ),
		VertexBufferData, GL_STATIC_DRAW );

	m_pGLFunctions->glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET( 0 ) );

	m_pGLFunctions->glEnableVertexAttribArray( 0 );

	m_pGLFunctions->glBindVertexArray( 0 );

	m_pGLFunctions->glBindBuffer( GL_ARRAY_BUFFER, 0 );

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

	glActiveTexture( GL_TEXTURE0 );

	glBindTexture( GL_TEXTURE_2D, m_TextureID );

	m_pProgram->setUniformValue( m_TextureSamplerUniform, 0 );

	m_pGLFunctions->glBindVertexArray( m_VertexArrayObject );

	m_pGLFunctions->glDrawArrays( GL_TRIANGLE_STRIP, 0, 8 );

	m_pGLFunctions->glBindVertexArray( 0 );

	m_pProgram->release( );

	glBindTexture( GL_TEXTURE_2D, 0 );

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

