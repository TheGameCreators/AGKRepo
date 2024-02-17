#include "agk.h"
#include <net/if.h>

using namespace AGK;

//*********************
// UDPManager
//*********************

UDPManager::UDPManager( const char* szIP, UINT listenPort )
{
	m_iValid = 0;
	if ( listenPort == 0 ) listenPort = 65535;
	if ( listenPort > 65535 ) listenPort = 65535;
	m_port = listenPort;

	if ( !szIP || !*szIP ) szIP = "anyip4";

	m_iIPv6 = 0;
	if ( strcmp(szIP,"anyip6") == 0 || strchr(szIP,':') ) m_iIPv6 = 1;

	unsigned short family;
	int size = 0;
	sockaddr_storage addr;
	if ( !m_iIPv6 )
	{
		family = AF_INET;
		sockaddr_in *addrv4 = (sockaddr_in*) &addr;
		addrv4->sin_family = family;
		addrv4->sin_port = htons( m_port );
		if ( strcmp(szIP,"anyip4") == 0 ) addrv4->sin_addr.s_addr = INADDR_ANY;
		else addrv4->sin_addr.s_addr = inet_addr( szIP );
		size = sizeof(sockaddr_in);
	}
	else
	{
		family = AF_INET6;
		sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
		memset(addrv6, 0, sizeof(sockaddr_in6));
		addrv6->sin6_family = family;
		addrv6->sin6_port = htons( m_port );
		if ( strcmp( szIP, "anyip6" ) == 0 ) addrv6->sin6_addr = in6addr_any;
		else inet_pton( AF_INET6, szIP, &(addrv6->sin6_addr) );
		size = sizeof(sockaddr_in6);
	}

	m_socket = socket( family, SOCK_DGRAM, IPPROTO_UDP );
	if ( m_socket == INVALID_SOCKET ) 
	{
		agk::Warning( "Failed to create UDP socket" );
		return;
	}

	int value = 1;
    setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value) );
	
	if ( family == AF_INET6 ) 
	{
		int value = 1;
		setsockopt( m_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&value, sizeof(value) ); // don't listen on IPV4 addresses

		// if address is multicast then join the group
		if ( strncmp(szIP,"FF",2) == 0 || strncmp(szIP,"ff",2) == 0 ) 
		{
			ipv6_mreq mreq;
			mreq.ipv6mr_interface = 0;
			inet_pton( AF_INET6, szIP, &(mreq.ipv6mr_multiaddr) );
			setsockopt(m_socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)&mreq, sizeof(mreq)) ;
		}
	}

	if ( bind( m_socket, (sockaddr*) &addr, size ) == SOCKET_ERROR )
	{
		close( m_socket );
		m_socket = INVALID_SOCKET;
		agk::Warning( "Failed to bind UDP socket" );
		return;
	}

	m_iValid = 1;
}

UDPManager::~UDPManager()
{
	if ( m_socket != INVALID_SOCKET )
	{
		close( m_socket );
		m_socket = INVALID_SOCKET;
	}
}

bool UDPManager::SendPacket( const char *IP, UINT port, const AGKPacket *packet )
{
	if ( m_socket == INVALID_SOCKET )
	{
		agk::Error( "Tried to send UDP packet on an uninitialised socket" );
		return false;
	}

	if ( !IP ) return false;
	if ( port == 0 || port > 65535 ) return false;
	if ( !packet ) return false;

	int size;
	sockaddr_storage addr;
	if ( !m_iIPv6 )
	{
		sockaddr_in *addrv4 = (sockaddr_in*) &addr;
		addrv4->sin_family = AF_INET;
		addrv4->sin_port = htons( port );
		addrv4->sin_addr.s_addr = inet_addr( IP );
		size = sizeof(sockaddr_in);
	}
	else
	{
		sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
		memset(addrv6, 0, sizeof(sockaddr_in6));
		addrv6->sin6_family = AF_INET6;
		addrv6->sin6_port = htons( port );
		inet_pton( AF_INET6, IP, &(addrv6->sin6_addr) );
		size = sizeof(sockaddr_in6);
	}
	
	int result = sendto( m_socket, packet->GetBuffer(), packet->GetSize(), 0, (sockaddr*)&addr, size );
	if ( result == 0 || result == SOCKET_ERROR ) return false;
	return true;
}

bool UDPManager::RecvPacket( char *fromIP, int *fromPort, AGKPacket *packet )
{
	if ( m_socket == INVALID_SOCKET )
	{
		agk::Error( "Tried to receive UDP packet on an uninitialised socket" );
		return false;
	}

	if ( !packet ) return false;

	unsigned long waiting = 0;
	if ( ioctl( m_socket, FIONREAD, &waiting ) == SOCKET_ERROR )
	{
		agk::Warning( "Failed to get receivable bytes on socket" );
		return false;
	}

	if ( waiting == 0 ) return false;

	sockaddr_storage addr;
	socklen_t length = sizeof(addr);
	int result = recvfrom( m_socket, packet->GetRaw(), AGK_NET_PACKET_SIZE, 0, (sockaddr*)&addr, &length );
	if ( result == SOCKET_ERROR )
	{
		agk::Warning( "Failed to receive UDP packet" );
		return false;
	}

	packet->SetSize( result );

	if ( result > 0 ) 
	{
		packet->SetPos( 0 );
		if ( addr.ss_family == AF_INET6 )
		{
			sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
			if ( fromPort ) *fromPort = ntohs(addrv6->sin6_port);
			if ( fromIP ) inet_ntop( AF_INET6, &(addrv6->sin6_addr), fromIP, 100 ); // don't know size so guess 100
		}
		else
		{
			sockaddr_in *addrv4 = (sockaddr_in*) &addr;
			if ( fromPort ) *fromPort = ntohs(addrv4->sin_port);
			if ( fromIP ) strcpy( fromIP, inet_ntoa( addrv4->sin_addr ) );
		}
	}

	return true;
}

bool UDPManager::PacketReady()
{
	if ( m_socket == INVALID_SOCKET )
	{
		agk::Error( "Tried to check UDP packet on an uninitialised socket" );
		return false;
	}
	
	unsigned long waiting = 0;
	if ( ioctl( m_socket, FIONREAD, &waiting ) == SOCKET_ERROR )
	{
		agk::Warning( "Failed to get receivable bytes on socket" );
		return false;
	}

	if ( waiting == 0 ) return false;
	return true;
}

//*********************
// AGKSocketTimeout
//*********************

AGKSocketTimeout::AGKSocketTimeout()
{
	m_pSocket = 0;
	m_iTimeout = 0;
}

AGKSocketTimeout::~AGKSocketTimeout()
{
	Stop();
	Join();
}

void AGKSocketTimeout::SetData( AGKSocket *pSocket, UINT iTimeout )
{
	m_pSocket = pSocket;
	m_iTimeout = iTimeout;
}

UINT AGKSocketTimeout::Run()
{
	if ( !m_pSocket ) return 0;
	if ( !m_pSocket->m_bConnecting ) return 0;

	if ( m_iTimeout > 0 ) SleepSafe( m_iTimeout );
	if ( m_bTerminate ) return 0;
	agk::Warning("Socket Timeout Exceeded");
	if ( m_pSocket->m_bConnecting ) 
	{
		if ( m_pSocket->m_bASync ) m_pSocket->Stop();
		m_pSocket->Close(false);
	}

	return 0;
}

//*********************
// AGKSocket
//*********************

UINT AGKSocket::Run()
{
	int family = 0;
	int size = 0;
	sockaddr_storage addr;
	if ( !IsIPV6() )
	{
		family = AF_INET;
		sockaddr_in *addrv4 = (sockaddr_in*) &addr;
		addrv4->sin_family = family;
		addrv4->sin_port = htons( m_port );
		addrv4->sin_addr.s_addr = inet_addr( m_szIP );
		size = sizeof(sockaddr_in);
	}
	else
	{
		family = AF_INET6;
		sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
		memset(addrv6, 0, sizeof(sockaddr_in6));
		addrv6->sin6_family = family;
		addrv6->sin6_port = htons( m_port );
        if ( strncmp(m_szIP, "fe", 2) == 0 || strncmp(m_szIP, "FE", 2) == 0 ) addrv6->sin6_scope_id = agk::GetNetworkInterface();
		inet_pton( AF_INET6, m_szIP, &(addrv6->sin6_addr) );
		size = sizeof(sockaddr_in6);
	}

	float endTime = agk::Timer() + m_iTimeout/1000.0f - 0.1f;

	// blocks, sometimes
	int result = 0;
	do
	{
		m_client = socket( family, SOCK_STREAM, IPPROTO_TCP );
		result = connect( m_client, (sockaddr*) &addr, size );
		if ( result != 0 || !m_bConnecting ) 
		{
			close( m_client );
			m_client = INVALID_SOCKET;
			agk::Sleep( 100 );
		}
	} while( result != 0 && endTime > agk::Timer() && m_bConnecting );

	if ( m_cTimeout.IsRunning() ) 
	{
		m_cTimeout.Stop();
		m_cTimeout.Join();
	}

	if ( result != 0 )
	{
		uString err( "Failed to connect to ", 70 );
		err += m_szIP;
		agk::Warning( err );
		Close();
		return false;
	}

	int opt = 1;
	int optlen = sizeof(int);
	setsockopt( m_client, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, optlen );
	
	m_bConnected = true;
	m_bConnecting = false;
	m_bDisconnected = false;

	return 0;
}

void AGKSocket::Reset()
{
	m_client = INVALID_SOCKET;
	strcpy( m_szIP, "" );
	m_port = 0;
	m_bConnected = false;
	m_bConnecting = false;
	m_bDisconnected = false;
	m_pNext = 0;
	m_iTimeout = 3000;
	m_fProgress = 0;
	m_bASync = false;
	
	m_sendBuffer[ 0 ] = 0;
	m_iSendWritePtr = 0;
}

AGKSocket::AGKSocket()
{
	Reset();
}

AGKSocket::AGKSocket( int s )
{
	Reset();
	m_client = s;
	m_bConnected = true;
	
	sockaddr_storage addr;
	socklen_t length = sizeof(addr);
	getpeername( m_client, (sockaddr*)&addr, &length );
	if ( addr.ss_family == AF_INET6 )
	{
		sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
		m_port = ntohs(addrv6->sin6_port);
		inet_ntop( AF_INET6, &(addrv6->sin6_addr), m_szIP, sizeof(m_szIP) );
	}
	else
	{
		sockaddr_in *addrv4 = (sockaddr_in*) &addr;
		m_port = ntohs(addrv4->sin_port);
		strcpy( m_szIP, inet_ntoa( addrv4->sin_addr ) );
	}
}

AGKSocket::~AGKSocket()
{
	Stop();
	Join();
	Close(false);
}

void AGKSocket::Close( bool bGraceful )
{
	m_bConnecting = false;
	m_bConnected = false;
	m_bDisconnected = true;
	if ( m_client != INVALID_SOCKET ) 
	{
		// try a graceful disconnect
		shutdown( m_client, 2 );
		/*
		if ( bGraceful )
		{
			char temp[ 32 ];
			int count = 0;
			while ( recv( m_client, temp, 32, 0 ) > 0 && count < 20 ) 
			{
				agk::Sleep( 10 );
				count++;
			}
		}
		*/
		//disconnect
		close( m_client );
	}
	m_client = INVALID_SOCKET;
}	

void AGKSocket::ForceClose()
{
	m_bConnecting = false;
	m_bConnected = false;
	m_bDisconnected = true;
	if ( m_client != INVALID_SOCKET ) 
	{
		//disconnect
		close( m_client );
	}
	m_client = INVALID_SOCKET;
}	

bool AGKSocket::Connect( const char* IP, UINT port, UINT timeout )
{
	if ( m_bConnected || m_client != INVALID_SOCKET ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString err( "Failed to connect socket to ", 50 );
		err += IP;
		err += ", socket is already connected to ";
		err += m_szIP;
		err += ", you must close a socket before connecting it again.";
		agk::Error( err );
#endif
		return false;
	}

	if ( !IP || strlen( IP ) >= 65 )
	{
#ifdef _AGK_ERROR_CHECK
		uString err( "Invalid IP address used to connect socket.", 50 );
		agk::Error( err );
#endif
		return false;
	}

	m_bASync = false;
	m_iTimeout = timeout;
	m_bConnecting = true;
	strcpy( m_szIP, IP );
	m_port = port;

	int family = AF_INET;
	int size = 0;
	sockaddr_storage addr;
	if ( !IsIPV6() )
	{
		sockaddr_in *addrv4 = (sockaddr_in*) &addr;
		addrv4->sin_family = family;
		addrv4->sin_port = htons( m_port );
		addrv4->sin_addr.s_addr = inet_addr( m_szIP );
		size = sizeof(sockaddr_in);
	}
	else
	{
		family = AF_INET6;
		sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
		memset(addrv6, 0, sizeof(sockaddr_in6));
		addrv6->sin6_family = family;
		addrv6->sin6_port = htons( m_port );
        if ( strncmp(m_szIP, "fe", 2) == 0 || strncmp(m_szIP, "FE", 2) == 0 ) addrv6->sin6_scope_id = agk::GetNetworkInterface();
		inet_pton( AF_INET6, m_szIP, &(addrv6->sin6_addr) );
		size = sizeof(sockaddr_in6);
	}

	float endTime = agk::Timer() + timeout/1000.0f - 0.1f;

	if ( m_cTimeout.IsRunning() ) 
	{
		m_cTimeout.Stop();
		m_cTimeout.Join();
	}
	m_cTimeout.SetData( this, timeout );
	m_cTimeout.Start();

	// blocks, sometimes
	int result = 0;
	do
	{
		m_client = socket( family, SOCK_STREAM, IPPROTO_TCP );
		result = connect( m_client, (sockaddr*) &addr, size );
		if ( result != 0 || !m_bConnecting ) 
		{
			close( m_client );
			m_client = INVALID_SOCKET;
			if ( !m_bConnecting ) return false;
			agk::Sleep( 100 );
		}
	} while( result != 0 && endTime > agk::Timer() );

	if ( m_cTimeout.IsRunning() ) 
	{
		m_cTimeout.Stop();
		m_cTimeout.Join();
	}

	if ( result != 0 )
	{
		uString err( "Failed to connect to ", 70 );
		err += m_szIP;
		agk::Warning( err );
		Close();
		return false;
	}

	int opt = 1;
	int optlen = sizeof(int);
	setsockopt( m_client, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, optlen );
	
	m_bConnected = true;
	m_bConnecting = false;
	m_bDisconnected = false;

	return true;
}

bool AGKSocket::ConnectASync( const char* IP, UINT port, UINT timeout )
{
	if ( m_bConnected || m_client != INVALID_SOCKET ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString err( "Failed to connect socket to ", 50 );
		err += IP;
		err += ", socket is already connected to ";
		err += m_szIP;
		err += ", you must close a socket before connecting it again.";
		agk::Error( err );
#endif
		return false;
	}

	if ( !IP || strlen( IP ) >= 65 )
	{
#ifdef _AGK_ERROR_CHECK
		uString err( "Invalid IP address used to connect socket.", 50 );
		agk::Error( err );
#endif
		return false;
	}

	m_bASync = true;
	m_iTimeout = timeout;
	m_bConnecting = true;
	strcpy( m_szIP, IP );
	m_port = port;

	if ( m_cTimeout.IsRunning() ) 
	{
		m_cTimeout.Stop();
		m_cTimeout.Join();
	}
	m_cTimeout.SetData( this, timeout );
	m_cTimeout.Start();

	Start();

	return true;
}

bool AGKSocket::SendFile( const char* szFilename )
{
	if ( m_bDisconnected ) return false;
	if ( !szFilename ) return false;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to send file on an unconnected socket" );
		return false;
	}
	
	cFile pFile;	
	if ( !pFile.OpenToRead( szFilename ) )
	{
		agk::Warning( "Could not send network file" );
		return false;
	}
	
	UINT size = pFile.GetSize();
	
	SendUInt( size );
	Flush();
	
	if ( size == 0 ) return true;
	
	UINT iSent = 0;
	while ( !pFile.IsEOF() )
	{
		int written = pFile.ReadData( m_sendBuffer, AGK_NET_SEND_BUFFER_SIZE ); 
		if ( written == 0 ) break;

		int result = 0;
		int sent = 0;
		do
		{
			result = send( m_client, m_sendBuffer+sent, written-sent, 0 );
			sent += result;
		} while ( result > 0 && result != SOCKET_ERROR && sent < written );
		
		if ( result == 0 || result == SOCKET_ERROR ) 
		{
			pFile.Close();
			m_bDisconnected = true;
			return false;
		}
		else
		{
			iSent += written;
			m_fProgress = iSent*100.0f / size;
			if ( m_fProgress > 100 ) m_fProgress = 100; 
		}
	}
	
	pFile.Close();
	return true;
}

bool AGKSocket::SendData( const char* s, int length )
{
	if ( m_bDisconnected ) return false;
	if ( length <= 0 ) return false;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to send data on an unconnected socket" );
		return false;
	}
	
	if ( length+m_iSendWritePtr > AGK_NET_SEND_BUFFER_SIZE )
	{
		// fill rest of the send buffer with some data
		int space = AGK_NET_SEND_BUFFER_SIZE - m_iSendWritePtr;
		if ( space > 0 ) memcpy( m_sendBuffer+m_iSendWritePtr, s, space );
		m_iSendWritePtr = AGK_NET_SEND_BUFFER_SIZE;

		Flush();
		if ( m_bDisconnected ) return false;
		
		UINT iSent = space;
		while ( length-iSent >= AGK_NET_SEND_BUFFER_SIZE )
		{
			int result = 0;
			unsigned int sent = 0;
			do
			{
				result = send( m_client, s+iSent+sent, AGK_NET_SEND_BUFFER_SIZE-sent, 0 );
				sent += result;
			} while ( result > 0 && result != SOCKET_ERROR && sent < AGK_NET_SEND_BUFFER_SIZE );

			if ( result == SOCKET_ERROR || result == 0 ) 
			{
				m_bDisconnected = true;
				return false;
			}
			
			iSent += result;
			
			m_fProgress = iSent*100.0f / length;
			if ( m_fProgress > 100 ) m_fProgress = 100;
		}
		
		// add rest of data to new buffer
		memcpy( m_sendBuffer, s+iSent, length-iSent );
		m_iSendWritePtr = length-iSent;
	}
	else
	{
		// add data to buffer
		memcpy( m_sendBuffer+m_iSendWritePtr, s, length );
		m_iSendWritePtr += length;
	}
	
	return true;
}

bool AGKSocket::SendString( const char *s )
{
	if ( m_bDisconnected ) return false;
	if ( !s ) return false;
	
	if ( !m_bConnected )
	{
		agk::Error( "Tried to send string on an unconnected socket" );
		return false;
	}
	
	UINT length = strlen( s );
	if ( !SendUInt( length ) ) return false;
	if ( length > 0 ) return SendData( s, length );
	return true;
}

bool AGKSocket::SendChar( char c )
{
	if ( m_bDisconnected ) return false;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to send char on an unconnected socket" );
		return false;
	}
	
	if ( m_iSendWritePtr+1 > AGK_NET_SEND_BUFFER_SIZE )
	{
		Flush();
	}
	
	// add char to buffer
	m_sendBuffer[ m_iSendWritePtr ] = c;
	m_iSendWritePtr += 1;
	return true;
}

bool AGKSocket::SendUInt( UINT u )
{
	if ( m_bDisconnected ) return false;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to send uint on an unconnected socket" );
		return false;
	}
	
	if ( m_iSendWritePtr+4 > AGK_NET_SEND_BUFFER_SIZE )
	{
		Flush();
	}
	
	// add char to buffer
	memcpy( m_sendBuffer + m_iSendWritePtr, &u, 4 ); // ARM does not like unaligned 4 byte access, so copy per byte
	m_iSendWritePtr += 4;
	return true;
}

bool AGKSocket::SendInt( int i )
{
	if ( m_bDisconnected ) return false;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to send int on an unconnected socket" );
		return false;
	}
	
	if ( m_iSendWritePtr+4 > AGK_NET_SEND_BUFFER_SIZE )
	{
		Flush();
	}
	
	// add char to buffer
	memcpy( m_sendBuffer + m_iSendWritePtr, &i, 4 ); // ARM does not like unaligned 4 byte access, so copy per byte
	m_iSendWritePtr += 4;
	return true;
}

bool AGKSocket::SendFloat( float f )
{
	if ( m_bDisconnected ) return false;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to send float on an unconnected socket" );
		return false;
	}
	
	if ( m_iSendWritePtr+4 > AGK_NET_SEND_BUFFER_SIZE )
	{
		Flush();
	}
	
	// add char to buffer
	memcpy( m_sendBuffer + m_iSendWritePtr, &f, 4 ); // ARM does not like unaligned 4 byte access, so copy per byte
	m_iSendWritePtr += 4;
	return true;
}

bool AGKSocket::Flush()
{
	if ( m_bDisconnected ) return false;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to flush data on an unconnected socket" );
		return false;
	}

	if ( m_iSendWritePtr > 0 )
	{
		int result = 0;
		unsigned int sent = 0;
		do
		{
			result = send( m_client, m_sendBuffer+sent, m_iSendWritePtr-sent, 0 );
			sent += result;
		} while ( result > 0 && result != SOCKET_ERROR && sent < m_iSendWritePtr );

		if ( result == 0 || result == SOCKET_ERROR ) 
		{
			uString err;
			err.Format( "Failed to flush socket data: %d", errno );
			agk::Warning( err );
			m_bDisconnected = true;
			return false;
		}
		m_iSendWritePtr = 0;
	}

	return true;
}

// if this is zero the recv commands will block
int AGKSocket::GetBytes()
{
	if ( m_bDisconnected ) return 0;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to get available bytes on an unconnected socket" );
		return 0;
	}
	
	unsigned long waiting = 0;
	if ( ioctl( m_client, FIONREAD, &waiting ) == SOCKET_ERROR )
	{
		agk::Warning( "Failed to get receivable bytes on socket" );
		m_bDisconnected = true;
		return 0;
	}
	
	return waiting;
}

// blocks until data is available.
// returns the number of bytes read, if this is zero the connection closed or an error occured.
int AGKSocket::RecvData( char* data, int length )
{
	if ( m_bDisconnected ) return 0;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to receive data on an unconnected socket" );
		return 0;
	}
	
	if ( !data || length <= 0 )
	{
		agk::Error( "Invalid network buffer passed to RecvData()" );
		return 0;
	}
	
	int result = recv( m_client, data, length, 0 );
	if ( result == SOCKET_ERROR || result == 0 )
	{
		agk::Warning( "Failed to get socket data" );
		m_bDisconnected = true;
		return 0;
	}
	return result;
}

// blocks until data is available.
// returns the number of characters read, not including the null terminator.
int AGKSocket::RecvString( uString &s )
{
	s.SetStr( "" );
	if ( m_bDisconnected ) return 0;
	
	if ( !m_bConnected )
	{
		agk::Error( "Tried to receive string on an unconnected socket" );
		return -1;
	}
	
	// get string length
	UINT length = RecvUInt();
	if ( length == 0 ) return 0;
	
	// get string data
	char *buf = new char[ length ];
	int result = recv( m_client, buf, length, 0 );
	int sofar = result;
	while ( result != SOCKET_ERROR && result > 0 && sofar < (int)length )
	{
		s.AppendN( buf, result );
		result = recv( m_client, buf, length-sofar, 0 );
		if ( result > 0 ) sofar += result;
	}
	
	if ( result != SOCKET_ERROR && result > 0 ) 
	{
		s.AppendN( buf, result );
	}
	else m_bDisconnected = true;
	
	delete [] buf;
	return sofar;
}

// blocks until data is available
char AGKSocket::RecvChar()
{
	if ( m_bDisconnected ) return 0;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to receive char on an unconnected socket" );
		return 0;
	}
	
	char c = 0;
	int result = recv( m_client, &c, 1, 0 );
	if ( result == SOCKET_ERROR || result == 0 ) m_bDisconnected = true;
	return c;
}

// blocks until data is available
int AGKSocket::RecvInt()
{
	if ( m_bDisconnected ) return 0;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to receive int on an unconnected socket" );
		return 0;
	}
	
	int i = 0;
	int received = 0;
	while ( received < 4 )
	{
		int result = recv( m_client, ((char*)&i)+received, 4-received, 0 );
		if ( result == SOCKET_ERROR || result == 0 ) 
		{
			m_bDisconnected = true;
			return 0;
		}
		received += result;
	}

	return agk::PlatformLocalEndian( i );
}

// blocks until data is available
UINT AGKSocket::RecvUInt()
{
	if ( m_bDisconnected ) return 0;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to receive uint on an unconnected socket" );
		return 0;
	}

	UINT u = 0;
	int received = 0;
	while ( received < 4 )
	{
		int result = recv( m_client, ((char*)&u)+received, 4-received, 0 );
		if ( result == SOCKET_ERROR || result == 0 ) 
		{
			m_bDisconnected = true;
			return 0;
		}
		received += result;
	}

	return agk::PlatformLocalEndian( u );
}

// blocks until data is available
float AGKSocket::RecvFloat()
{
	if ( m_bDisconnected ) return 0;
	if ( !m_bConnected )
	{
		agk::Error( "Tried to receive float on an unconnected socket" );
		return 0;
	}

	float f = 0;
	int received = 0;
	while ( received < 4 )
	{
		int result = recv( m_client, ((char*)&f)+received, 4-received, 0 );
		if ( result == SOCKET_ERROR || result == 0 ) 
		{
			m_bDisconnected = true;
			return 0;
		}
		received += result;
	}

	return f;
}		

//**********************
// Network Listener
//**********************

UINT cNetworkListener::Run()
{
	while ( !m_bTerminate )
	{
		if ( m_socket == INVALID_SOCKET ) SleepSafe(1000);
		else
		{
			int client = accept( m_socket, NULL, NULL );
			if ( m_bTerminate ) return 0;
			if ( client == INVALID_SOCKET )
			{
				agk::Warning( "Failed to accept connection" );
				//return 0;
				continue;
			}
			
			// don't wait to send packets (turns off nagle algorithm)
			int opt = 1;
			int optlen = sizeof(int);
			setsockopt( client, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, optlen );
			
			// don't generate an exception when other end disconnects
			opt = 1;
			optlen = sizeof(int);
			setsockopt( client, SOL_SOCKET, SO_NOSIGPIPE, (char*)&opt, optlen );
			
			AGKSocket *pAGKSocket = new AGKSocket( client );
			
			{
				cAutoLock autoLock(&m_lock); // will unlock when destroyed
				pAGKSocket->m_pNext = m_pConnections;
				m_pConnections = pAGKSocket;
			}
		}
	}

	return 0;
}

cNetworkListener::cNetworkListener()
{
	m_socket = INVALID_SOCKET;
	m_pConnections = 0;
	m_port = 0;
}

cNetworkListener::~cNetworkListener()
{
	Stop();
	Join();

	// delete waiting connections
	while ( m_pConnections )
	{
		AGKSocket *pSocket = m_pConnections;
		m_pConnections = m_pConnections->m_pNext;
		delete pSocket;
	}
}

void cNetworkListener::Stop()
{
	AGKThread::Stop();
	if ( m_socket != INVALID_SOCKET ) 
	{
		shutdown( m_socket, 2 );
		close( m_socket );
	}
	m_socket = INVALID_SOCKET;
}

AGKSocket* cNetworkListener::GetNewConnection()
{
	if ( !m_pConnections ) return 0;

	AGKSocket *pAGKSocket = 0;
	{
		cAutoLock autoLock(&m_lock); // will unlock when destroyed

		if ( !m_pConnections ) return 0; // might have changed before we got the lock

		pAGKSocket = m_pConnections;
		m_pConnections = pAGKSocket->m_pNext;
	}

	pAGKSocket->m_pNext = 0;
	return pAGKSocket;
}

int cNetworkListener::AcceptConnections( UINT port )
{
	return AcceptConnections( "", port );
}

int cNetworkListener::AcceptConnections( const char *szIP, UINT port )
{
	if ( m_socket != INVALID_SOCKET ) 
	{
		uString err;
		err.Format( "Failed to listen on port %d, this socket is already listening on port %d", port, m_port );
		agk::Error( err );
		return 0;
	}

	if ( !szIP || !*szIP ) szIP = "anyip4";

	int family = 0;
	int size = 0;
	sockaddr_storage addr;
	if ( strcmp(szIP,"anyip6") == 0 || strchr(szIP,':') ) 
	{
		size = sizeof(sockaddr_in6);
		family = AF_INET6;
		sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
		memset(addrv6, 0, sizeof(sockaddr_in6));
		addrv6->sin6_family = AF_INET6;
		addrv6->sin6_port = htons( port );
		if ( strcmp(szIP,"anyip6") == 0 ) addrv6->sin6_addr = in6addr_any;
		else inet_pton( AF_INET6, szIP, &(addrv6->sin6_addr) );
	}
	else
	{
		size = sizeof(sockaddr_in);
		family = AF_INET;
		sockaddr_in *addrv4 = (sockaddr_in*) &addr;
		addrv4->sin_family = AF_INET;
		addrv4->sin_port = htons( port );
		if ( strcmp(szIP,"anyip4") == 0 ) addrv4->sin_addr.s_addr = INADDR_ANY;
		else addrv4->sin_addr.s_addr = inet_addr( szIP );
	}

	m_socket = socket( family, SOCK_STREAM, IPPROTO_TCP );
	if ( m_socket == INVALID_SOCKET ) 
	{
		agk::Warning( "Failed to create listening socket" );
		return 0;
	}

	int value = 1;
    setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value) );
	if ( family == AF_INET6 ) setsockopt( m_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&value, sizeof(value) ); // don't listen on IPV4 addresses

	if ( bind( m_socket, (sockaddr*) &addr, size ) == SOCKET_ERROR )
	{
		agk::Warning( "Failed to bind listening socket" );
		close( m_socket );
		m_socket = INVALID_SOCKET;
		return 0;
	}

	m_port = port;

	if ( listen( m_socket, 5 ) == SOCKET_ERROR )
	{
		agk::Warning( "Failed to start socket listening" );
		close( m_socket );
		m_socket = INVALID_SOCKET;
		return 0;
	}

	Start();

	return 1;
}



//************************
// Broadcast Listener
//************************

BroadcastListener::BroadcastListener()
{
	m_socket = INVALID_SOCKET;
}

BroadcastListener::~BroadcastListener()
{
	if ( m_socket != INVALID_SOCKET ) 
	{
		shutdown( m_socket, 2 );
		close( m_socket );
	}
}

void BroadcastListener::Close()
{
	if ( m_socket != INVALID_SOCKET ) 
	{
		shutdown( m_socket, 2 );
		close( m_socket );
	}
	m_socket = INVALID_SOCKET;
}

bool BroadcastListener::SetListenPort( UINT port )
{
	return SetListenPort( "", port );
}

bool BroadcastListener::SetListenPort( const char *szIP, UINT port )
{
	if ( m_socket != INVALID_SOCKET ) 
	{
		shutdown( m_socket, 2 );
		close( m_socket );
	}

	if ( !szIP || !*szIP ) szIP = "anyip4";
	if ( strcmp( szIP, "anyip6" ) == 0 ) szIP = "FF02::41:474B"; // if they have selected any then join link local AGK multicast address

	int family = 0;
	int size = 0;
	sockaddr_storage addr;
	if ( strcmp(szIP,"anyip6") == 0 || strchr(szIP,':') ) 
	{
		size = sizeof(sockaddr_in6);
		family = AF_INET6;
		sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
		memset(addrv6, 0, sizeof(sockaddr_in6));
		addrv6->sin6_family = AF_INET6;
		addrv6->sin6_port = htons( port );
		if ( strncmp(szIP,"FF",2) == 0 || strncmp(szIP,"ff",2) == 0 ) addrv6->sin6_addr = in6addr_any;
		else inet_pton( AF_INET6, szIP, &(addrv6->sin6_addr) );
	}
	else
	{
		size = sizeof(sockaddr_in);
		family = AF_INET;
		sockaddr_in *addrv4 = (sockaddr_in*) &addr;
		addrv4->sin_family = AF_INET;
		addrv4->sin_port = htons( port );
		if ( strcmp(szIP,"anyip4") == 0 ) addrv4->sin_addr.s_addr = INADDR_ANY;
		else addrv4->sin_addr.s_addr = inet_addr( szIP );
	}

	m_socket = socket( family, SOCK_DGRAM, IPPROTO_UDP );
	if ( m_socket == INVALID_SOCKET ) 
	{
		agk::Warning( "Failed to create listening broadcast socket" );
		return false;
	}
	
	int value = 1;
    setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value) );

	if ( family == AF_INET6 ) 
	{
		int value = 1;
		setsockopt( m_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&value, sizeof(value) ); // don't listen on IPV4 addresses

		// if address is multicast then join the group
		if ( strncmp(szIP,"FF",2) == 0 || strncmp(szIP,"ff",2) == 0 ) 
		{
			ipv6_mreq mreq;
			mreq.ipv6mr_interface = 0;
			inet_pton( AF_INET6, szIP, &(mreq.ipv6mr_multiaddr) );
			setsockopt(m_socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)&mreq, sizeof(mreq)) ;
		}
	}

	if ( bind( m_socket, (sockaddr*) &addr, size ) == SOCKET_ERROR )
	{
		agk::Warning( "Failed to bind listening socket" );
		return false;
	}
	
	return true;
}

bool BroadcastListener::ReceivedBroadcast()
{
	if ( m_socket == INVALID_SOCKET ) return false;
	
	unsigned long waiting = 0;
	if ( ioctl( m_socket, FIONREAD, &waiting ) == SOCKET_ERROR ) return false;
	return waiting > 0;
}

bool BroadcastListener::GetPacket( AGKPacket &packet, UINT &fromPort, char *fromIP )
{
	if ( m_socket == INVALID_SOCKET ) return false;
	
	socklen_t length = sizeof(addr);
	int result = recvfrom( m_socket, packet.GetRaw(), AGK_NET_PACKET_SIZE, 0, (sockaddr*)&addr, &length );
	if ( result == SOCKET_ERROR )
	{
		agk::Warning( "Failed to receive broadcast" );
		return false;
	}
	if ( result == 0 ) 
	{
		return false;
	}
	
	packet.SetPos( 0 );
	packet.SetSize( result );
	
	if ( result > 0 ) 
	{
		if ( addr.ss_family == AF_INET6 )
		{
			sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
			inet_ntop( AF_INET6, &(addrv6->sin6_addr), fromIP, 65 );
			fromPort = ntohs(addrv6->sin6_port);
		}
		else
		{
			sockaddr_in *addrv4 = (sockaddr_in*) &addr;
			strcpy( fromIP, inet_ntoa( addrv4->sin_addr ) );
			fromPort = ntohs(addrv4->sin_port);
		}
	}
	
	return true;
}

//*****************************
// Broadcaster
//*****************************

UINT Broadcaster::Run()
{
	int count = 0;
	int sock = 0;

	if ( m_ipv6 )
	{
		sock = socket( AF_INET6, SOCK_DGRAM, IPPROTO_UDP );
		if ( sock == INVALID_SOCKET ) 
		{
			agk::Warning( "Failed to create broadcast packet for IPv6" );
			return 0;
		}
	}
	else
	{
		sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		if ( sock == INVALID_SOCKET ) 
		{
			agk::Warning( "Failed to create broadcast packet for IPv4" );
			return 0;
		}
	}

	int opt = 1;
	int optlen = sizeof(int);
	if ( m_ipv6 ) 
	{
        int interface = agk::GetNetworkInterface();        
        setsockopt( sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, &interface, sizeof(interface) );
        
		sockaddr_in6 addr;
		memset(&addr, 0, sizeof(sockaddr_in6));
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons( m_port );
		inet_pton( AF_INET6, "FF02::41:474B", &(addr.sin6_addr) ); // link local AGK multicast address
		int result = connect( sock, (sockaddr*) &addr, sizeof(addr) );
		if ( result == SOCKET_ERROR )
		{
			agk::Warning( "Failed to set broadcast packet address for IPv6" );
			close( sock );
			sock = INVALID_SOCKET;
			return 0;
		}
	}
	else
	{
		setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, optlen );
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons( m_port );
		addr.sin_addr.s_addr = inet_addr( "255.255.255.255" );
		int result = connect( sock, (sockaddr*) &addr, sizeof(addr) );
		if ( result == SOCKET_ERROR )
		{
			agk::Warning( "Failed to set broadcast packet address for IPv4" );
			close( sock );
			sock = INVALID_SOCKET;
			return 0;
		}
	}

	do 
	{
		int result = 0;
		unsigned int sent = 0;
		do
		{
			result = send( sock, m_packet.GetBuffer()+sent, m_packet.GetSize()-sent, 0 );
			sent += result;
		} while ( result > 0 && result != SOCKET_ERROR && sent < m_packet.GetSize() );

		if ( result == SOCKET_ERROR )
		{
			agk::Warning( "Failed to send broadcast packet" );
			close( sock );
			sock = INVALID_SOCKET;
			return 0;
		}

		// sleep for interval milliseconds unless woken by the stop event
		SleepSafe( m_interval );
		if ( m_bTerminate ) 
		{
			break;
		}

		count++;
	} while ( m_max == 0 || count < m_max );

	shutdown( sock, 2 );
	close( sock );

	return 0;
}

Broadcaster::Broadcaster()
{
	m_interval = 1000;
	m_max = 0;
}

// sets the data that will be broadcast every interval milliseconds for a maximum of max broadcasts (0=forever)
// start with Start() and stop with Stop()
void Broadcaster::SetData( int ipv6, UINT port, const AGKPacket* packet, UINT interval, int max )
{
	if ( !packet ) return;
	if ( interval < 1000 ) interval = 1000; //minimum interval of 1 seond to stop flooding the network
	if ( packet->GetSize() > 512 )
	{
		agk::Error( "Attempted to broadcast more than 512 bytes" );
		return;
	}
	
	m_ipv6 = ipv6;
	m_port = port;
	m_interval = interval;
	m_packet.Copy( packet );
	m_max = max;
}

//************************
// HTTPConnection
//************************

@implementation AGKHTTPListener
- (id) init
{
    m_response = [[ NSMutableData alloc ] init ];
    m_length = 0;
    m_received = 0;
    
    m_pHTTP = 0;
    m_bToFile = false;
    m_sFilename.SetStr("");
    m_file = 0;
    
    self = [super init];
    
    return self;
}

- (void) reset
{
    [m_response setLength:0];
    m_length = 0;
    m_received = 0;
    
    m_pHTTP = 0;
    m_bToFile = false;
    m_sFilename.SetStr("");
    m_file = 0;
}

/*
 - (id) initWithRequest:(NSURLRequest*)request delegate:(id)delegate
 {
 m_response = [[ NSMutableData alloc ] init ];
 m_length = 0;
 m_received = 0;
 
 self = [super initWithRequest:request delegate:delegate];
 
 //NSLog( @"Connection started to: %s", [[[request URL] path] UTF8String] );
 
 return self;
 }
 */

- (void) dealloc
{
    if ( m_response ) [m_response release];
    [super dealloc];
}

- (NSInputStream *)connection:(NSURLConnection *)connection needNewBodyStream:(NSURLRequest *)request
{
    uString sFullPath( m_pHTTP->GetBodyFileName() );
    agk::PlatformGetFullPathWrite(sFullPath);
    NSString *pPath = [ NSString stringWithCString:sFullPath.GetStr() encoding:NSUTF8StringEncoding ];
    
    return [ NSInputStream inputStreamWithFileAtPath:pPath ];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    if ( m_bToFile )
    {
        if ( m_file )
        {
            m_file->Close();
            delete m_file;
            m_file = 0;
        }
    }
    m_pHTTP->FinishedInternal(1);
    
    //NSLog( @"Connection finished" );
    // don't release connection here, do it in FinishedInternal instead
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    if ( m_bToFile )
    {
        if ( m_file ) delete m_file;
        m_file = new AGK::cFile();
        m_file->OpenToWrite(m_sFilename.GetStr());
    }
    else
    {
        [ m_response setLength:0 ];
    }
    NSHTTPURLResponse *responseHTTP = (NSHTTPURLResponse*) response;
    
    m_length = (int)[responseHTTP expectedContentLength];
    m_received = 0;
    
    strcpy( m_pHTTP->m_szContentType, [[response MIMEType] cStringUsingEncoding:NSUTF8StringEncoding] );
    
    
    m_pHTTP->m_iStatusCode = (int)[responseHTTP statusCode];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    if ( m_bToFile )
    {
        if ( !m_file )
        {
            m_file = new AGK::cFile();
            m_file->OpenToWrite(m_sFilename.GetStr());
        }
        m_file->WriteData( (const char*)[data bytes], (int)[data length]);
    }
    else
    {
        [ m_response appendData:data ];
    }
    
    m_received += [ data length ];
    if ( m_length > 0 )
    {
        float progress = m_received*100.0f / m_length;
        if ( progress > 100 ) progress = 100;
        if ( progress < 0 ) progress = 0;
        m_pHTTP->ProgressInternal(progress);
    }
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    if ( m_bToFile )
    {
        if ( m_file )
        {
            m_file->Close();
            delete m_file;
            m_file = 0;
        }
    }
    m_pHTTP->FinishedInternal(0);
    NSLog( @"%@", [error localizedDescription] );
    
    // don't relase connection here, do it in FinishedInternal instead
}

- (void)connection:(NSURLConnection *)connection didSendBodyData:(NSInteger)bytesWritten totalBytesWritten:(NSInteger)totalBytesWritten totalBytesExpectedToWrite:(NSInteger)totalBytesExpectedToWrite
{
    if ( totalBytesExpectedToWrite > 0 )
    {
        float progress = totalBytesWritten*100.0f / totalBytesExpectedToWrite;
        if ( progress > 100 ) progress = 100;
        if ( progress < 0 ) progress = 0;
        m_pHTTP->ProgressInternal(progress);
    }
}

- (BOOL)connection:(NSURLConnection *)connection canAuthenticateAgainstProtectionSpace:(NSURLProtectionSpace *)protectionSpace
{
    if ( m_pHTTP->GetVerifyMode() == 0 )
    {
        return [protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust];
    }
    else
    {
        return NO;
    }
}

- (void)connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge
{
    if ( m_pHTTP->GetVerifyMode() == 0 )
    {
        [challenge.sender continueWithoutCredentialForAuthenticationChallenge:challenge];
    }
    else
    {
        [challenge.sender useCredential:[NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust] forAuthenticationChallenge:challenge];
    }
}
@end

cHTTPConnection::cHTTPConnection()
{
    m_connection = nil;
    m_sHost = 0;
    m_listener = [[AGKHTTPListener alloc] init];
    m_iSecure = 0;
    m_bConnected = false;
    m_bFinished = true;
    m_iTimeout = 6000;
    m_iVerifyMode = 1;
    
    m_szResponse = 0;
    m_fProgress = 0;
    m_iStatusCode = 0;
    
    m_bSaveToFile = false;
    
    m_szContentType[0] = '\0';
}

cHTTPConnection::~cHTTPConnection()
{
    //NSLog( @"Connection deleted" );
    if ( m_connection )
    {
        [m_connection cancel];
        [m_connection release];
        m_connection = nil;
    }
    if ( m_listener ) [ m_listener release ];
    if ( m_sHost ) [ m_sHost release ];
    if ( m_szResponse ) delete [] m_szResponse;
    
    m_listener = 0;
    
    cHTTPHeader *pHeader = m_cHeaders.GetFirst();
    while( pHeader )
    {
        delete pHeader;
        pHeader = m_cHeaders.GetNext();
    }
    m_cHeaders.ClearAll();
}

UINT cHTTPConnection::Run()
{
    return 0;
}

void cHTTPConnection::Stop()
{
    
}

void cHTTPConnection::SetTimeout( int milliseconds )
{
    if ( milliseconds < 0 ) milliseconds = 0;
    m_iTimeout = milliseconds;
}

void cHTTPConnection::SetVerifyCertificate( int mode )
{
    m_iVerifyMode = mode;
}

void cHTTPConnection::AddHeader( const char* headerName, const char* headerValue )
{
    if ( !m_bFinished )
    {
        agk::Warning( "Cannot change HTTP headers whilst an async request or download is still in progress, wait for GetHTTPResponseReady() or GetHTTPFileComplete() to return 1" );
        return;
    }
    
    cHTTPHeader *pHeader = m_cHeaders.GetItem( headerName );
    if ( !pHeader )
    {
        pHeader = new cHTTPHeader();
        pHeader->sName.SetStr( headerName );
        m_cHeaders.AddItem( pHeader, headerName );
    }
    
    pHeader->sValue.SetStr( headerValue );
}

void cHTTPConnection::RemoveHeader( const char* headerName )
{
    if ( !m_bFinished )
    {
        agk::Warning( "Cannot change HTTP headers whilst an async request or download is still in progress, wait for GetHTTPResponseReady() or GetHTTPFileComplete() to return 1" );
        return;
    }
    
    cHTTPHeader *pHeader = m_cHeaders.RemoveItem( headerName );
    if ( pHeader ) delete pHeader;
}

void cHTTPConnection::FinishedInternal( int value )
{
    if ( m_connection )
    {
        [m_connection release];
        m_connection = nil;
    }
    
    if ( value == 0 )
    {
        uString err;
        err.Format( "Connection to %s closed unexpectedly", m_sLastURL.GetStr() );
        agk::Warning( err );
    }
    
    if ( m_listener )
    {
        int reslength = (int) [m_listener->m_response length];
        
        if ( value > 0 )
        {
            m_szResponse = new char[ reslength + 1 ];
            if ( reslength > 0 ) memcpy( m_szResponse, [m_listener->m_response bytes], reslength );
            m_szResponse[ reslength ] = 0;
        }
    }
    
    if ( m_sRndFilename.GetLength() > 0 ) agk::DeleteFile( m_sRndFilename );
    m_sRndFilename.SetStr( "" );
    
    m_bFinished = true;
    m_fProgress = 100;
}

bool cHTTPConnection::SetHost( const char *szHost, int iSecure, const char *szUser, const char *szPass )
{
    uString sURL( szHost );
    
    m_sUsername.SetStr(szUser);
    m_sPassword.SetStr(szPass);
    
    if ( iSecure == 0 ) sURL.Prepend( "http://" );
    else sURL.Prepend( "https://" );
    
    m_iSecure = iSecure;
    
    m_sHost = [ [ NSString alloc ] initWithUTF8String:sURL.GetStr() ];
    
    return true;
}

void cHTTPConnection::Close()
{
    
}

char* cHTTPConnection::SendRequest( const char *szServerFile, const char *szPostData )
{
    if ( !m_bFinished )
    {
        agk::Warning( "Failed to send HTTP request, a request is already in progress for this ID" );
        return 0;
    }
    
    if ( m_szResponse ) delete [] m_szResponse;
    m_szResponse = 0;
    m_fProgress = 0;
    m_iStatusCode = 0;
    m_bFinished = true;
    
    NSString *sURL = [ m_sHost stringByAppendingString:@"/" ];
    sURL = [ sURL stringByAppendingString:[NSString stringWithCString:szServerFile encoding:NSUTF8StringEncoding] ];
    m_sLastURL.SetStr( [sURL cStringUsingEncoding:NSUTF8StringEncoding] );
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:sURL] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:m_iTimeout/1000.0 ];
    
    if ( szPostData && *szPostData )
    {
        [request addValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
        [request setHTTPMethod:@"POST"];
        [request setHTTPBody:[NSData dataWithBytes:szPostData length:strlen(szPostData)] ];
    }
    
    uString sHeader;
    if ( m_sUsername.GetLength() > 0 || m_sPassword.GetLength() > 0 )
    {
        sHeader.Format( "%s:%s", m_sUsername.GetStr(), m_sPassword.GetStr() );
        char* base64 = agk::StringToBase64( sHeader.GetStr() );
        sHeader.Format( "Basic %s", base64 );
        delete [] base64;
        [request addValue:[NSString stringWithUTF8String:sHeader.GetStr()] forHTTPHeaderField:@"Authorization"];
    }
    
    cHTTPHeader *pHeader = m_cHeaders.GetFirst();
    while( pHeader )
    {
        [request setValue:[NSString stringWithUTF8String:pHeader->sValue.GetStr()] forHTTPHeaderField:[NSString stringWithUTF8String:pHeader->sName.GetStr()]];
        pHeader = m_cHeaders.GetNext();
    }
    
    
    NSURLResponse *response = 0;
    NSData *responseData = [ NSURLConnection sendSynchronousRequest:request returningResponse:&response error:NULL ];
    if ( !responseData )
    {
        uString err;
        err.Format( "Failed to connect to %s", [sURL cStringUsingEncoding:NSUTF8StringEncoding] );
        agk::Warning( err );
        return 0;
    }
    
    NSHTTPURLResponse *responseHTTP = (NSHTTPURLResponse*) response;
    m_iStatusCode = (int)[responseHTTP statusCode];
    
    int resLength = (int) [responseData length];
    char * str = new char[ resLength + 1 ];
    memcpy( str, [ responseData bytes ], resLength );
    str[ resLength ] = 0;
    return str;
}

bool cHTTPConnection::SendRequestASync( const char *szServerFile, const char *szPostData )
{
    if ( !m_bFinished )
    {
        agk::Warning( "Failed to send HTTP request, a request is already in progress for this ID" );
        return false;
    }
    
    if ( m_szResponse ) delete [] m_szResponse;
    m_szResponse = 0;
    m_fProgress = 0;
    m_iStatusCode = 0;
    m_bFinished = false;
    m_sRndFilename.SetStr("");
    
    NSString *sURL = [ m_sHost stringByAppendingString:@"/" ];
    sURL = [ sURL stringByAppendingString:[NSString stringWithCString:szServerFile encoding:NSUTF8StringEncoding] ];
    m_sLastURL.SetStr( [sURL cStringUsingEncoding:NSUTF8StringEncoding] );
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:sURL] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:m_iTimeout/1000.0 ];
    
    if ( szPostData && *szPostData )
    {
        [request addValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
        [request setHTTPMethod:@"POST"];
        [request setHTTPBody:[NSData dataWithBytes:szPostData length:strlen(szPostData)] ];
    }
    
    uString sHeader;
    if ( m_sUsername.GetLength() > 0 || m_sPassword.GetLength() > 0 )
    {
        sHeader.Format( "%s:%s", m_sUsername.GetStr(), m_sPassword.GetStr() );
        char* base64 = agk::StringToBase64( sHeader.GetStr() );
        sHeader.Format( "Basic %s", base64 );
        delete [] base64;
        [request addValue:[NSString stringWithUTF8String:sHeader.GetStr()] forHTTPHeaderField:@"Authorization"];
    }
    
    cHTTPHeader *pHeader = m_cHeaders.GetFirst();
    while( pHeader )
    {
        [request setValue:[NSString stringWithUTF8String:pHeader->sValue.GetStr()] forHTTPHeaderField:[NSString stringWithUTF8String:pHeader->sName.GetStr()]];
        pHeader = m_cHeaders.GetNext();
    }
    
    // fix Content-Length not being visible to didReceiveData callback
    [request setValue:@"identity" forHTTPHeaderField:@"Accept-Encoding"];
    
    [ m_listener reset ];
    m_listener->m_pHTTP = this;
    m_listener->m_bToFile = false;
    m_listener->m_file = 0;
    m_connection = [ [NSURLConnection alloc] initWithRequest:request delegate:m_listener ];
    
    if ( !m_connection )
    {
        uString err;
        err.Format( "Failed to connect to %s", [sURL cStringUsingEncoding:NSUTF8StringEncoding] );
        agk::Warning( err );
        m_bFinished = true;
        return false;
    }
    
    return true;
}

bool cHTTPConnection::SendFile( const char *szServerFile, const char *szPostData, const char *szLocalFile )
{
    if ( !m_bFinished )
    {
        agk::Warning( "Failed to send HTTP file, a request is already in progress for this ID" );
        return false;
    }
    
    if ( !szServerFile || !*szServerFile )
    {
        agk::Warning( "Failed to send HTTP file, invalid server file path (empty string)" );
        return false;
    }
    
    if ( !szLocalFile || !cFile::Exists(szLocalFile) )
    {
        agk::Warning( "Failed to send HTTP file, could not find file to upload" );
        return false;
    }
    
    // can't load the entire file into memory to send in the HTTP body due to memory concerns
    // also can't extend NSInputStream to handle post data + file separately
    // solution: copy the entire HTTP body to a temp file and stream that file using NSInputStream
    
    if ( m_szResponse ) delete [] m_szResponse;
    m_szResponse = 0;
    m_fProgress = 0;
    m_iStatusCode = 0;
    m_bFinished = false;
    
    NSString *sURL = [ m_sHost stringByAppendingString:@"/" ];
    sURL = [ sURL stringByAppendingString:[NSString stringWithCString:szServerFile encoding:NSUTF8StringEncoding] ];
    m_sLastURL.SetStr( [sURL cStringUsingEncoding:NSUTF8StringEncoding] );
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:sURL] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:m_iTimeout/1000.0 ];
    
    // create temp file that will stream the http body to NSInputStream
    int random = agk::Random();
    int random2 = agk::Random();
    m_sRndFilename.Format("/uploadtemp%d-%d",random,random2);
    
    cFile tempFile;
    tempFile.OpenToWrite(m_sRndFilename);
    
    uString postData( szPostData );
    uString sFinalPostData;
    
    // parse the post variables into a file upload friendly format.
    char sName[ 256 ];
    char sValue[ 512 ];
    const char* remaining = postData.GetStr();
    int count = postData.Count( '&' ) + 1;
    for ( int i = 0; i < count; i++ )
    {
        size_t pos = strcspn( remaining, "=" );
        strncpy( sName, remaining, pos );
        sName[ pos ] = '\0';
        remaining += pos+1;
        pos = strcspn( remaining, "&" );
        strncpy( sValue, remaining, pos );
        sValue[ pos ] = '\0';
        remaining += pos+1;
        
        if ( strlen( sName ) == 0 || strlen( sValue ) == 0 ) continue;
        
        sFinalPostData.Append( "--------------------AaB03x\r\nContent-Disposition: form-data; name=\"" );
        sFinalPostData.Append( sName );
        sFinalPostData.Append( "\"\r\n\r\n" );
        sFinalPostData.Append( sValue );
        sFinalPostData.Append( "\r\n" );
    }
    
    // extract filename from the path
    uString uploadFile( szLocalFile );
    uploadFile.Replace( '\\', '/' );
    uString sFilename;
    int pos = uploadFile.RevFind( '/' );
    if ( pos >= 0 ) uploadFile.SubString( sFilename, pos+1 );
    else sFilename.SetStr( uploadFile );
    
    // add file variable to post data
    sFinalPostData.Append( "--------------------AaB03x\r\nContent-Disposition: form-data; name=\"myfile\"; filename=\"" );
    sFinalPostData.Append( sFilename );
    sFinalPostData.Append( "\"\r\nContent-Type: application/x-zip-compressed\r\n\r\n" );
    
    // write the initial data to the temp file
    tempFile.WriteData( sFinalPostData.GetStr(), sFinalPostData.GetLength() );
    
    // copy upload file to temp file
    cFile localFile;
    localFile.OpenToRead( szLocalFile );
    char buf[5000];
    int written = 0;
    do
    {
        written = localFile.ReadData(buf, 5000);
        if ( written > 0 ) tempFile.WriteData(buf, written);
    } while ( written > 0 && !localFile.IsEOF() );
    localFile.Close();
    
    // finialise the http body data
    uString sEndPostData( "\r\n--------------------AaB03x--\r\n" );
    tempFile.WriteData(sEndPostData, sEndPostData.GetLength());
    tempFile.Close();
    
    uString sFullPath( m_sRndFilename );
    agk::PlatformGetFullPathWrite(sFullPath);
    NSString *pPath = [ NSString stringWithCString:sFullPath.GetStr() encoding:NSUTF8StringEncoding ];
    
    NSInputStream *stream = [ NSInputStream inputStreamWithFileAtPath:pPath ];
    
    [request setHTTPMethod:@"POST"];
    [request setHTTPBodyStream:stream ];
    
    int length = cFile::GetFileSize(m_sRndFilename);
    NSString *sLength = [NSString stringWithFormat:@"%d",length ];
    [request addValue:sLength forHTTPHeaderField:@"Content-Length"];
    [request addValue:@"multipart/form-data; boundary=------------------AaB03x" forHTTPHeaderField:@"Content-Type"];
    
    uString sHeader;
    if ( m_sUsername.GetLength() > 0 || m_sPassword.GetLength() > 0 )
    {
        sHeader.Format( "%s:%s", m_sUsername.GetStr(), m_sPassword.GetStr() );
        char* base64 = agk::StringToBase64( sHeader.GetStr() );
        sHeader.Format( "Basic %s", base64 );
        delete [] base64;
        [request addValue:[NSString stringWithUTF8String:sHeader.GetStr()] forHTTPHeaderField:@"Authorization"];
    }
    
    cHTTPHeader *pHeader = m_cHeaders.GetFirst();
    while( pHeader )
    {
        [request setValue:[NSString stringWithUTF8String:pHeader->sValue.GetStr()] forHTTPHeaderField:[NSString stringWithUTF8String:pHeader->sName.GetStr()]];
        pHeader = m_cHeaders.GetNext();
    }
    
    // fix Content-Length not being visible to didReceiveData callback
    [request setValue:@"identity" forHTTPHeaderField:@"Accept-Encoding"];
    
    [ m_listener reset ];
    m_listener->m_pHTTP = this;
    m_listener->m_bToFile = false;
    m_listener->m_file = 0;
    m_connection = [ [NSURLConnection alloc] initWithRequest:request delegate:m_listener ];
    
    if ( !m_connection )
    {
        uString err;
        err.Format( "Failed to connect to %s", [sURL cStringUsingEncoding:NSUTF8StringEncoding] );
        agk::Warning( err );
        m_bFinished = true;
        return false;
    }
    
    return true;
}

int cHTTPConnection::GetResponseReady()
{
    if ( !m_bFinished ) return 0;
    else if ( m_szResponse ) return 1;
    else return -1;
}

const char* cHTTPConnection::GetResponse()
{
    return m_szResponse;
}

const char* cHTTPConnection::GetContentType()
{
    return m_szContentType;
}

bool cHTTPConnection::DownloadFile( const char *szServerFile, const char *szLocalFile, const char *szPostData )
{
    if ( !m_bFinished )
    {
        agk::Warning( "Failed to send HTTP request, a request is already in progress for this ID" );
        return false;
    }
    
    if ( m_szResponse ) delete [] m_szResponse;
    m_szResponse = 0;
    m_fProgress = 0;
    m_iStatusCode = 0;
    m_bFinished = false;
    m_sRndFilename.SetStr("");
    
    NSString *sURL = [ m_sHost stringByAppendingString:@"/" ];
    sURL = [ sURL stringByAppendingString:[NSString stringWithCString:szServerFile encoding:NSUTF8StringEncoding] ];
    m_sLastURL.SetStr( [sURL cStringUsingEncoding:NSUTF8StringEncoding] );
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:sURL] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:m_iTimeout/1000.0 ];
    
    if ( szPostData && *szPostData )
    {
        [request addValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
        [request setHTTPMethod:@"POST"];
        [request setHTTPBody:[NSData dataWithBytes:szPostData length:strlen(szPostData)] ];
    }
    
    uString sHeader;
    if ( m_sUsername.GetLength() > 0 || m_sPassword.GetLength() > 0 )
    {
        sHeader.Format( "%s:%s", m_sUsername.GetStr(), m_sPassword.GetStr() );
        char* base64 = agk::StringToBase64( sHeader.GetStr() );
        sHeader.Format( "Basic %s", base64 );
        delete [] base64;
        [request addValue:[NSString stringWithUTF8String:sHeader.GetStr()] forHTTPHeaderField:@"Authorization"];
    }
    
    cHTTPHeader *pHeader = m_cHeaders.GetFirst();
    while( pHeader )
    {
        [request setValue:[NSString stringWithUTF8String:pHeader->sValue.GetStr()] forHTTPHeaderField:[NSString stringWithUTF8String:pHeader->sName.GetStr()]];
        pHeader = m_cHeaders.GetNext();
    }
    
    // fix Content-Length not being visible to didReceiveData callback
    [request setValue:@"identity" forHTTPHeaderField:@"Accept-Encoding"];
    
    [ m_listener reset ];
    m_listener->m_pHTTP = this;
    m_listener->m_bToFile = true;
    m_listener->m_sFilename.SetStr(szLocalFile);
    m_listener->m_file = 0;
    m_connection = [ [NSURLConnection alloc] initWithRequest:request delegate:m_listener ];
    
    if ( !m_connection )
    {
        uString err;
        err.Format( "Failed to connect to %s", [sURL cStringUsingEncoding:NSUTF8StringEncoding] );
        agk::Warning( err );
        m_bFinished = true;
        return false;
    }
    
    return true;
}

bool cHTTPConnection::DownloadComplete()
{
    return m_bFinished ? 1 : 0;
}
