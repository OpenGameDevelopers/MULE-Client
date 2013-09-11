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

const int		MAX_BUFFER_LENGTH	= 1024;
const int		STREAM_WIDTH		= 800;
const int		STREAM_HEIGHT		= 600;
const int		STREAM_COLOURCOUNT	= 4;
const GLenum	STREAM_FORMAT		= GL_BGRA;
const int		STREAM_SIZE			= STREAM_WIDTH * STREAM_HEIGHT *
									STREAM_COLOURCOUNT;

typedef struct __tagImagePacket
{
	int		Offset;
	char	Data[ 1020 ];
}ImagePacket;

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
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, STREAM_WIDTH, STREAM_HEIGHT, 0,
		STREAM_FORMAT, GL_UNSIGNED_BYTE, ( GLvoid * )m_pImageData );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glGenBuffers( 1, &m_PBO );

	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_PBO );
	glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, STREAM_SIZE, 0,
		GL_STREAM_DRAW_ARB );
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

	// Initialise sockets
/*	int Error;
	struct addrinfo ClientHints, *pServerInfo, *pAddrItr;

	memset( &ClientHints, 0, sizeof( ClientHints ) );
	ClientHints.ai_family	= AF_UNSPEC;
	ClientHints.ai_socktype	= SOCK_DGRAM;
	ClientHints.ai_flags	= AI_PASSIVE;

	if( ( Error = getaddrinfo( NULL, "5093", &ClientHints, &pServerInfo ) ) !=
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

		if( ( bind( m_Socket, pAddrItr->ai_addr, pAddrItr->ai_addrlen ) ) ==
			-1 )
		{
			close( m_Socket );
			printf( "Failed to bind socket\n" );
			return 0;
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
	}

	freeaddrinfo( pServerInfo );


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
		if( ( m_ServerSocket = socket( pAddrItr->ai_family,
			pAddrItr->ai_socktype, pAddrItr->ai_protocol ) ) == -1 )
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

	if( fcntl( m_ServerSocket, F_SETFL, O_NONBLOCK, NonBlock ) == -1 )
	{
		printf( "Failed to set non-blocking socket\n" );
	}

	freeaddrinfo( pServerInfo );

	m_SocketLength = sizeof( m_SocketAddress );
	m_ServerLength = sizeof( m_ServerAddress );*/
	struct addrinfo ClientHints;//, *pServerInfo, *pAddrItr;
	int Error;
	int NumBytes;
	char Buffer[ MAX_BUFFER_LENGTH ];
	struct sockaddr_storage RemoteAddress;
	socklen_t AddressLength;
	static bool datasent = false;

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

// The image here should be updated via an offset into the image's data, really
void UpdateImage( GLubyte *p_pPixels, int p_Size )
{
    static int Colour = 0;

    if( !p_pPixels )
	{
        return;
	}

    int *pDataPtr = ( int * )p_pPixels;

    for( int i = 0; i < STREAM_HEIGHT; ++i )
    {
        for( int j = 0; j < STREAM_WIDTH; ++j )
        {
            *pDataPtr = Colour;
            ++pDataPtr;
        }
        Colour += 196;
    }

    ++Colour;
}

void RemoteDisplayElement::Render( )
{/*
	// Get the backbuffer from the server (assuming just one for now)
	int NumBytes;
	char Buffer[ MAX_BUFFER_LENGTH ];
	static bool datasent = false;

	if( !datasent )
	{
		if( ( NumBytes = sendto( m_ServerSocket, "Hello", strlen( "Hello" ), 0,
			( struct sockaddr * )&m_SocketAddress, m_SocketLength ) )
				== -1 )
		{
			printf( "Failed to send data: %s\n", strerror( errno ) );
			datasent = true;
			return;
		}
	}

	static int FrameData = 0;
	printf( "Getting frame data...\n" );

	if( ( NumBytes = recvfrom( m_ServerSocket, Buffer, MAX_BUFFER_LENGTH, 0,
		( struct sockaddr * )&m_ServerAddress, &( m_ServerLength ) ) ) == -1 )
	{
		printf( "Error receiving from socket: %s\n", strerror( errno ) );
		return;
	}
	else
	{
		printf( "Packet [%d bytes]:\n", NumBytes );
		++FrameData;
		printf( "Frame count: %d\n", FrameData );

		glBindTexture( GL_TEXTURE_2D, m_TextureID );
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_PBO );
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, STREAM_WIDTH, STREAM_HEIGHT,
			STREAM_FORMAT, GL_UNSIGNED_BYTE, 0 );
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_PBO );
		glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, STREAM_SIZE, 0,
			GL_STREAM_DRAW_ARB );
		GLubyte *pImagePointer = ( GLubyte * )glMapBuffer(
			GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );

		if( pImagePointer )
		{
			// The packet received should contain the ID of the image stream
			// for the user as well as the offset to update (in pixels)
			// followed by the image stream data to use (the packet's size
			// minus the ID and position will be used to calculate the data
			// size)
			UpdateImage( pImagePointer, STREAM_SIZE );
			glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB );
		}

		glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	}

	glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, m_TextureID );
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-0.5f, -0.5f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 0.5f, -0.5f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f( 0.5f,  0.5f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-0.5f,  0.5f, 0.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
*/
	// Get the backbuffer from the server (assuming just one for now)
//	struct addrinfo ClientHints, *pServerInfo, *pAddrItr;
	int Error;
	int NumBytes;
	char Buffer[ MAX_BUFFER_LENGTH ];
	struct sockaddr_storage RemoteAddress;
	socklen_t AddressLength;
	static bool datasent = false;
/*
	memset( &ClientHints, 0, sizeof( ClientHints ) );
	ClientHints.ai_family	= AF_UNSPEC;
	ClientHints.ai_socktype	= SOCK_DGRAM;
	ClientHints.ai_flags	= AI_PASSIVE;

	if( ( Error = getaddrinfo( NULL, "5092", &ClientHints, &pServerInfo ) ) !=
		0 )
	{
		printf( "Failed to get address information: %s\n",
			gai_strerror( Error ) );
		return;
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
		return;
	}*/
	if( !datasent )
	{
		if( ( NumBytes = sendto( m_Socket, "Hello", strlen( "Hello" ), 0,
			pAddrItr->ai_addr, pAddrItr->ai_addrlen ) ) == -1 )
		{
			freeaddrinfo( pServerInfo );
			printf( "Failed to send data\n" );
			datasent = true;
			return;
		}
	}

//	printf( "Getting frame data...\n" );

	ImagePacket TmpPkt;
	if( ( NumBytes = recvfrom( m_Socket, &TmpPkt, MAX_BUFFER_LENGTH, 0,
		( struct sockaddr * )&RemoteAddress, &AddressLength ) ) == -1 )
	{
//		printf( "Error receiving from socket\n" );
		return;
	}
	else
	{
		//printf( "Packet [%d bytes]: %s\n", NumBytes, Buffer );
		//printf( "Pos: %d\n", ntohl( TmpPkt.Offset ) );
		printf( "Data:\n" );
		for( int i = 0; i < NumBytes-4; ++i )
		{
			printf( "%02X  ", TmpPkt.Data[ i ] );
		}
		printf( "\n" );

		static unsigned int Colour = 0xFF00000FF;
		glBindTexture( GL_TEXTURE_2D, m_TextureID );
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_PBO );
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, STREAM_WIDTH, STREAM_HEIGHT,
			STREAM_FORMAT, GL_UNSIGNED_BYTE, 0 );
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, m_PBO );
		glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, STREAM_SIZE, 0,
			GL_STREAM_DRAW_ARB );
		GLubyte *pImagePointer = ( GLubyte * )glMapBuffer(
			GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
		if( pImagePointer )
		{
			UpdateImage( pImagePointer, STREAM_SIZE );
		
/*		for( int i = 0; i < STREAM_WIDTH*STREAM_HEIGHT; ++i )
		{
			*pImagePointer = Colour;
			++pImagePointer;
		}*/
		glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB );
		}

		glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	}

	glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, m_TextureID );
    glColor4f(0, 1, 1, 1);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-0.5f, -0.5f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 0.5f, -0.5f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f( 0.5f,  0.5f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-0.5f,  0.5f, 0.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}

