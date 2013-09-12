#include <RemoteDisplayElement.hpp>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

const int		BLOCK_SIZE			= 32;
const int		MAX_BUFFER_LENGTH	= 1024;
const int		STREAM_WIDTH		= 800;
const int		STREAM_HEIGHT		= 600;
const int		STREAM_COLOURCOUNT	= 3;
const GLenum	STREAM_FORMAT		= GL_BGR;
const int		STREAM_SIZE			= STREAM_WIDTH * STREAM_HEIGHT *
									STREAM_COLOURCOUNT;
const int BLOCK_COLUMNS	= STREAM_WIDTH/BLOCK_SIZE +
			( STREAM_WIDTH%BLOCK_SIZE ? 1 : 0 );
const int BLOCK_ROWS = STREAM_HEIGHT/BLOCK_SIZE +
			( STREAM_HEIGHT%BLOCK_SIZE ? 1 : 0 );
char g_BufferToSend[ STREAM_WIDTH*STREAM_HEIGHT*STREAM_COLOURCOUNT ];

typedef struct __tagImagePacket
{
//	int		BlockIndex;
	int		Offset;
	char	Data[ 1020 ];
}ImagePacket;

const int PACKET_HEADER = sizeof( int );// * 2;

RemoteDisplayElement::RemoteDisplayElement( )
{
	m_TextureID = 0;
	m_PBO = 0;
	memset( &m_Dimension, 0, sizeof( m_Dimension ) );

	memset( &m_Point, 0, sizeof( m_Point ) );
	m_pImageData = NULL;
}

RemoteDisplayElement::RemoteDisplayElement( const int p_Width, const int p_Height )
{
	m_Dimension.Width = p_Width;
	m_Dimension.Height = p_Height;

	m_pImageData = new GLubyte[ STREAM_SIZE ];
	memset( m_pImageData, 0, STREAM_SIZE );

	m_TextureID = 0;
	m_PBO = 0;
}

RemoteDisplayElement::~RemoteDisplayElement( )
{
	this->Destroy( );
}
void *GetINetAddr( struct sockaddr *p_Addr )
{
	if( p_Addr->sa_family == AF_INET )
	{
		return &( ( ( struct sockaddr_in * )p_Addr )->sin_addr );
	}

	return &( ( ( struct sockaddr_in6 * )p_Addr )->sin6_addr );
}
int RemoteDisplayElement::Initialise( )
{
	// Initialise GL
	glGenTextures( 1, &m_TextureID );

	glBindTexture( GL_TEXTURE_2D, m_TextureID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, STREAM_WIDTH, STREAM_HEIGHT, 0,
		STREAM_FORMAT, GL_UNSIGNED_BYTE, ( GLvoid * )m_pImageData );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glGenBuffers( 1, &m_PBO );

	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_PBO );
	glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, STREAM_SIZE, 0,
		GL_STREAM_DRAW_ARB );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

	// Initialise sockets
	struct addrinfo ClientHints;
	int Error;

	memset( &ClientHints, 0, sizeof( ClientHints ) );
	ClientHints.ai_family	= AF_UNSPEC;
	ClientHints.ai_socktype	= SOCK_DGRAM;
	ClientHints.ai_flags	= AI_PASSIVE;

	if( ( Error = getaddrinfo( NULL, "5092", &ClientHints, &pServerInfo ) ) !=
		0 )
	{
		printf( "Failed to get address information: %s\n",
			gai_strerror( Error ) );
		return 0;
	}

	for( pAddrItr = pServerInfo; pAddrItr != NULL;
		pAddrItr = pAddrItr->ai_next )
	{
		if( ( m_Socket = socket( pAddrItr->ai_family, pAddrItr->ai_socktype,
			pAddrItr->ai_protocol ) ) == -1 )
		{
			printf( "Failed to create socket\n" );
			continue;
		}

		break;
	}

	if( pAddrItr == NULL )
	{
		freeaddrinfo( pServerInfo );
		printf( "Failed to obtain a valid address\n" );
		return 0;
	}

	int NonBlock = 1;
	if( fcntl( m_Socket, F_SETFL, O_NONBLOCK, NonBlock ) == -1 )
	{
		printf( "Failed to set non-blocking socket\n" );
		return 0;
	}
		
	memset( g_BufferToSend, 0xFF,
		STREAM_WIDTH*STREAM_HEIGHT*STREAM_COLOURCOUNT );
	for( int r = 0; r < STREAM_HEIGHT; ++r )
	{
		for( int i = 0; i < STREAM_WIDTH*STREAM_COLOURCOUNT; ++i )
		{
			g_BufferToSend[ i+( r*STREAM_WIDTH*STREAM_COLOURCOUNT ) ] =
				( r%256 ) & 0xFF;
		}
	}
	return 1;
}

void RemoteDisplayElement::Destroy( )
{
	if( pServerInfo )
	{
		freeaddrinfo( pServerInfo );
	}
	if( m_pImageData )
	{
		delete [ ] m_pImageData;
		m_pImageData = NULL;
	}

	if( m_PBO )
	{
		glDeleteBuffers( 1, &m_PBO );
	}

	if( m_TextureID )
	{
		glDeleteTextures( 1, &m_TextureID );
	}

	if( m_Socket )
	{
		close( m_Socket );
	}
}

void RemoteDisplayElement::Render( )
{
	// Send the image size, compression, and ID
	IMAGE_DATA Packet;
	int Compression = htonl( 100 );
	int Width = htonl( STREAM_WIDTH );
	int Height = htonl( STREAM_HEIGHT );
	struct sockaddr_storage RemoteAddress;
	socklen_t AddressLength;
	Packet.ID = htonl( 1 );
	memcpy( Packet.Data, &Width, sizeof( int ) );
	memcpy( &( Packet.Data[ sizeof( int ) ] ), &Height, sizeof( int ) );
	memcpy( &( Packet.Data[ sizeof( int )*2 ] ), &Compression, sizeof( int ) );

	sendto( m_Socket, &Packet, sizeof( Packet ), 0, pAddrItr->ai_addr,
		pAddrItr->ai_addrlen );
	
	// The first packet from the server will describe how many packets are to
	// be expected
  recvfrom( m_Socket, &Packet, sizeof( Packet ), 0,
		( struct sockaddr * )&RemoteAddress, &AddressLength ); 
		unsigned long PacketCount = 0;
		for( size_t i = 0; i < sizeof( unsigned long ); ++i )
		{
			PacketCount |= Packet.Data[ i ] << ( i*8 );
		}
		printf( "Packet count: %lu\n",  PacketCount  );
	
	// Loop
	//   Wait for the server to send the metadata about the image (packet count)
	//   Confirm the packet has been received, ignoring re-sent packets
	// EndLoop
	// Decompress the JPG and set the texture with the decompressed data
	glPushMatrix( );

    glBindTexture( GL_TEXTURE_2D, m_TextureID );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
    glNormal3f(0, 0, 1);
    glTexCoord2f( 0.0f, 0.0f );   glVertex3f( -1.0f, -1.0f, 0.0f );
    glTexCoord2f( 1.0f, 0.0f );   glVertex3f(  1.0f, -1.0f, 0.0f );
    glTexCoord2f( 1.0f, 1.0f );   glVertex3f(  1.0f,  1.0f, 0.0f );
    glTexCoord2f( 0.0f, 1.0f );   glVertex3f( -1.0f,  1.0f, 0.0f );
    glEnd( );

    glBindTexture( GL_TEXTURE_2D, 0 );
}

