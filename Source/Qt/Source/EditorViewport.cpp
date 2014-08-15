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
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>

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

void *GetINetAddr( struct sockaddr *p_Addr )
{
	if( p_Addr->sa_family == AF_INET )
	{
		return &( ( ( struct sockaddr_in * )p_Addr )->sin_addr );
	}

	return &( ( ( struct sockaddr_in6 * )p_Addr )->sin6_addr );
}

uint64_t htonll( uint64_t p_Host )
{
	return ( ( ( uint64_t ) htonl( p_Host ) << 32 ) + htonl( p_Host >> 32 ) );
}

uint64_t ntohll( uint64_t p_Network )
{
	return ( ( ( uint64_t ) ntohl( p_Network ) << 32 ) +
		ntohl( p_Network >> 32 ) );
}

EditorViewport::EditorViewport( QWidget *p_pParent ) :
	QWidget( p_pParent ),
	m_pFramebuffer( nullptr ),
	m_Type( ViewportOrthographic ),
	m_Side( ViewportXY ),
	m_RedClear( 0.13f ),
	m_GreenClear( 0.0f ),
	m_BlueClear( 0.13f ),
	m_ViewID( 0 ),
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

	if( m_Socket != -1 )
	{
		::close( m_Socket );
	}

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

	// Create the socket for this view (one socket for the entire program would
	// make more sense...)
	struct addrinfo ConnectHints, *pServerInfo, *pAddrItr;

	memset( &ConnectHints, 0, sizeof( ConnectHints ) );

	ConnectHints.ai_family		= AF_UNSPEC;
	ConnectHints.ai_protocol	= IPPROTO_TCP;
	ConnectHints.ai_socktype	= SOCK_STREAM;

	int Error;

	if( ( Error = getaddrinfo( "192.168.10.117", "5092", &ConnectHints,
		&pServerInfo ) ) != 0 ) 
	{
		printf( "Failed to get address info for the server\n" );

		return 1;
	}

	for( struct addrinfo *pAddrItr = pServerInfo; pAddrItr != NULL;
		pAddrItr = pAddrItr->ai_next )
	{
		if( ( m_Socket = socket( pAddrItr->ai_family,
			pAddrItr->ai_socktype, pAddrItr->ai_protocol ) ) == -1 )
		{
			printf( "Failed to get socket\n" );
			continue;
		}
		if( ::connect( m_Socket, pAddrItr->ai_addr, pAddrItr->ai_addrlen ) ==
			-1 )
		{
			::close( m_Socket );
			m_Socket = -1;
			printf( "Failed to connect to server\n" );

			continue;
		}

		break;
	}

	freeaddrinfo( pServerInfo );

	if( pAddrItr == NULL )
	{
		printf( "Failed to create socket and connect to server\n" );
	}

	// Request a view ID and tell the server about the screen size
	if( m_Socket != -1 )
	{
		DATA_PACKET IDRequest;
		IDRequest.ID = htonl( 2 );
		send( m_Socket, &IDRequest, sizeof( IDRequest ), 0 );

		DATA_PACKET ViewID;
		// Wait to hear back
		if( recv( m_Socket, &ViewID, sizeof( ViewID ), 0 ) == -1 )
		{
			return 1;
		}

		if( ntohl( ViewID.ID ) != 0 )
		{
			return 1;
		}

		int ID = ViewID.Data[ 0 ] | ViewID.Data[ 1 ] << 8 |
			ViewID.Data[ 2 ] << 16 | ViewID.Data[ 3 ] << 24;

		m_ViewID = ntohl( ID );

		DATA_PACKET ScreenSize;
		ScreenSize.ID = htonl( 1 );
		IMAGE_LAYOUT Layout;
		Layout.Width = htonl( width( ) );
		Layout.Height = htonl( height( ) );
		Layout.Compression = htonl( 40 );
		Layout.ViewID = htonl( m_ViewID );
		memcpy( ScreenSize.Data, &Layout, sizeof( Layout ) );
		send( m_Socket, &ScreenSize, sizeof( ScreenSize ), 0 );
	}

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

	// Get the JPEG data
	DATA_PACKET Data;
	Data.ID = htonl( 3 );
	if( send( m_Socket, &Data, sizeof( Data ), 0 ) == -1 )
	{
		return;
	}
	if( recv( m_Socket, &Data, sizeof( Data ), 0 ) == -1 )
	{
		return;
	}

	if( Data.ID != ntohl( 1 ) )
	{
		return;
	}

	unsigned long PacketCount;
	unsigned long BufferSize;

	memcpy( &PacketCount, Data.Data, sizeof( unsigned long ) );
	memcpy( &BufferSize, Data.Data + sizeof( unsigned long ),
		sizeof( unsigned long ) );

	PacketCount = ntohll( PacketCount );
	BufferSize = ntohll( BufferSize );
	printf( "Packet Count: %llu\n", PacketCount );
	printf( "Buffer Size: %llu\n", BufferSize );

	unsigned char *pCompressedImage = new unsigned char[ BufferSize ];

	// Pretend to read the incoming data
	for( unsigned long i = 0; i < PacketCount; ++i )
	{
		recv( m_Socket, &Data, sizeof( Data ), 0 );

		unsigned int ID = ntohl( Data.ID );

		IMAGE_DATA_STREAM ImageData;
		memcpy( &ImageData, Data.Data, sizeof( Data.Data ) );
		ImageData.Offset = ntohl( ImageData.Offset );

		if( ID == 3 )
		{
			memcpy( pCompressedImage + ImageData.Offset, ImageData.Data,
				sizeof( ImageData.Data ) );
		}
		else if( ID == 2 )
		{
			// The offset becomes the amount of bytes to copy
			memcpy( pCompressedImage, ImageData.Data, ImageData.Offset );
		}
		else
		{
			// error
			printf( "ID: %d\n", ID );
		}
	}

	FILE *pFile = fopen( "/tmp/compressed.jpg", "w" );

	fwrite( pCompressedImage, 1, BufferSize, pFile );

	fclose( pFile );


	struct jpeg_decompress_struct JpegDecompress;
	struct jpeg_error_mgr		JpegError;

	JpegDecompress.err = jpeg_std_error( &JpegError );
	jpeg_create_decompress( &JpegDecompress );
	jpeg_mem_src( &JpegDecompress, pCompressedImage, BufferSize );

	if( jpeg_read_header( &JpegDecompress, true ) != 1 )
	{
		printf( "Failed to read JPEG header\n" );
		return;
	}

	jpeg_start_decompress( &JpegDecompress );

	int JPEGWidth = JpegDecompress.output_width;
	int JPEGHeight = JpegDecompress.output_height;
	int JPEGPixelSize = JpegDecompress.output_components;
	int JPEGTotalSize = JPEGWidth * JPEGHeight * JPEGPixelSize;
	printf( "Total size: %d\n", JPEGTotalSize );
	unsigned char *pDecompressedImage = new unsigned char[ JPEGTotalSize ];

	while( JpegDecompress.output_scanline < JpegDecompress.output_height )
	{
		unsigned char *pBuffer[ 1 ];
		pBuffer[ 0 ] = pDecompressedImage +
			( JpegDecompress.output_scanline ) * JPEGWidth * JPEGPixelSize;
		jpeg_read_scanlines( &JpegDecompress, pBuffer, 1 );
	}

	jpeg_finish_decompress( &JpegDecompress );

	delete [ ] pCompressedImage;

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_TextureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width( ), height( ), GL_RGB,
		GL_UNSIGNED_BYTE, ( GLvoid * )pDecompressedImage );
	glBindTexture( GL_TEXTURE_2D, 0 );

	delete [ ] pDecompressedImage;


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

	glDeleteTextures( 1, &m_TextureID );

	glGenTextures( 1, &m_TextureID );

	glBindTexture( GL_TEXTURE_2D, m_TextureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width( ), height( ), 0,
		GL_RGB, GL_UNSIGNED_BYTE, ( GLvoid * )NULL );
	glBindTexture( GL_TEXTURE_2D, 0 );

	// Update the server of the new size
	if( m_Socket != -1 )
	{
		DATA_PACKET ScreenSize;
		ScreenSize.ID = htonl( 1 );
		IMAGE_LAYOUT Layout;
		Layout.Width = htonl( width( ) );
		Layout.Height = htonl( height( ) );
		Layout.Compression = htonl( 40 );
		Layout.ViewID = htonl( m_ViewID );
		memcpy( ScreenSize.Data, &Layout, sizeof( Layout ) );
		send( m_Socket, &ScreenSize, sizeof( ScreenSize ), 0 );
	}

	printf( "Resize\n" );
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

