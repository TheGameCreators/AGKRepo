#include "agk.h"
#include "errno.h"
#include <net/if.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "emscripten.h"

using namespace AGK;

int agk::PlatformGetIP( uString &sIP )
{
	sIP.SetStr("192.168.0.111");
	return 1;
}

int agk::PlatformGetIPv6( uString &sIP, int *iInterface )
{
	sIP.SetStr("");
	return 0;
}

//*********************
// UDPManager
//*********************

UDPManager::UDPManager( const char* szIP, UINT listenPort )
{
	
}

UDPManager::~UDPManager()
{

}

bool UDPManager::SendPacket( const char *IP, UINT port, const AGKPacket *packet )
{
	return false;
}

bool UDPManager::RecvPacket( char *fromIP, int *fromPort, AGKPacket *packet )
{
	return false;
}

bool UDPManager::PacketReady()
{
	return false;
}

//*********************
// AGKSocket
//*********************

cHashedList<AGKSocket*> AGKSocket::g_cAllSockets(128);

UINT AGKSocket::Run()
{
	if ( !m_bConnecting ) return 0;
	
	SleepSafe( m_iTimeout );
	if ( m_bTerminate ) return 0;
	//if ( m_bConnecting ) Close(false);
	
	return 0;
}

void AGKSocket::Reset()
{
	static int first = 1;
	if ( first )
	{
		first = 0;
		emscripten_set_socket_open_callback( 0, AGKSocket::ConnectedCallback );
		//emscripten_set_socket_connection_callback( 0, AGKSocket::ConnectedCallback );
		emscripten_set_socket_error_callback( 0, AGKSocket::ErrorCallback );
	}
	
	m_client = INVALID_SOCKET;
	strcpy( m_szIP, "" );
	m_port = 0;
	m_bConnected = false;
	m_bConnecting = false;
	m_bDisconnected = false;
	m_pNext = 0;
	m_iTimeout = 3000;
	m_fProgress = 0;
	
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
	g_cAllSockets.AddItem( this, s );
	
	sockaddr_in addr;
	socklen_t length = sizeof(addr);
	getpeername( m_client, (sockaddr*)&addr, &length );
	m_port = addr.sin_port;
	strcpy( m_szIP, inet_ntoa( addr.sin_addr ) );
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
		g_cAllSockets.RemoveItem( m_client );
	
		// try a graceful disconnect
		//shutdown( m_client, 2 );
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
		g_cAllSockets.RemoveItem( m_client );
	
		//disconnect
		close( m_client );
	}
	m_client = INVALID_SOCKET;
}

bool AGKSocket::Connect( const char* IP, UINT port, UINT timeout )
{
	agk::Error("Synchronous ConnectSocket is not supported on HTML5");
	return false;
}

void AGKSocket::ConnectedCallback( int fd, void* userdata )
{
	agk::Warning("Sebsocket Connected");
	AGKSocket *pSocket = g_cAllSockets.GetItem(fd);
	if ( !pSocket ) return;
	
	pSocket->m_bConnecting = false;
	pSocket->m_bConnected = true;
}

void AGKSocket::ErrorCallback( int fd, int err, const char *msg, void* userdata )
{
	agk::Warning(msg);
	AGKSocket *pSocket = g_cAllSockets.GetItem(fd);
	if ( !pSocket ) return;
	
	pSocket->Close(false);
}

void AGKSocket::TimeoutCallback( void* userdata )
{
	AGKSocket *pSocket = (AGKSocket*)userdata;
	if ( !pSocket ) return;
	
	if ( pSocket->m_bConnecting ) 
	{
		agk::Warning("Timeout Closed Socket");
		pSocket->Close(false);
	}
}

bool AGKSocket::ConnectASync( const char* IP, UINT port, UINT timeout )
{
	if ( m_bConnected || m_client != INVALID_SOCKET ) 
	{
		uString err( "Failed to connect socket to ", 50 );
		err += IP;
		err += ", socket is already connected to ";
		err += m_szIP;
		err += ", you must close a socket before connecting it again.";
		agk::Error( err );
		return false;
	}
	
	if ( m_bConnecting ) 
	{
		agk::Error( "Socket is already connecting" );
		return false;
	}
	
	if ( !IP || strlen( IP ) > 40 )
	{
		uString err( "Invalid IP address used to connect socket.", 50 );
		agk::Error( err );
		return false;
	}
	
	m_bConnecting = true;
	strcpy( m_szIP, IP );
	m_port = port;
	
	// don't generate an exception when other end disconnects
	//opt = 1;
	//optlen = sizeof(int);
	//setsockopt( m_client, SOL_SOCKET, SO_NOSIGPIPE, (char*)&opt, optlen );
	
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( m_port );
	addr.sin_addr.s_addr = inet_addr( m_szIP );

	m_iTimeout = timeout;
		
	int result = 0;
	m_client = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	g_cAllSockets.AddItem( this, m_client );
	
	// HTML5 doesn't block, it will return EINPROGRESS
	result = connect( m_client, (sockaddr*) &addr, sizeof(addr) );
	if ( result != 0 )
	{
		if ( errno != EINPROGRESS )
		{		
			Close();
			uString err;
			err.Format( "Failed to connect to %s, error %d", m_szIP, errno );
			agk::Warning( err );
			return false;
		}
		
		// set a timeout callback to close the socket if it hasn't connected
		emscripten_async_call( AGKSocket::TimeoutCallback, this, m_iTimeout );
	}
	
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
			agk::Warning( "Failed to send socket file data" );
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
				agk::Warning( "Failed to send socket data" );
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

		if ( (result == 0 || result == SOCKET_ERROR) && errno != EAGAIN && errno != EWOULDBLOCK ) 
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
	s.ClearTemp();
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
				//m_socket = INVALID_SOCKET;
				//return 0;
				continue;
			}
			
			// don't wait to send packets (turns off nagle algorithm)
			int opt = 1;
			int optlen = sizeof(int);
			setsockopt( client, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, optlen );
			
			// don't generate an exception when other end disconnects
			//opt = 1;
			//optlen = sizeof(int);
			//setsockopt( client, SOL_SOCKET, SO_NOSIGPIPE, (char*)&opt, optlen );
			
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
		if ( !m_pConnections ) return 0;

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

	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	
	if ( m_socket == INVALID_SOCKET ) 
	{
		agk::Warning( "Failed to create listening socket" );
		return 0;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	if ( szIP && *szIP ) addr.sin_addr.s_addr = inet_addr( szIP );
	else addr.sin_addr.s_addr = INADDR_ANY;

	int value = 1;
	setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value) );

	if ( bind( m_socket, (sockaddr*) &addr, sizeof(addr) ) == SOCKET_ERROR )
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

/*
UINT BroadcastListener::Run()
{
	if ( !callback ) return 0;

	int result = 0;
	do
	{
		// blocks
		socklen_t length = sizeof(addr);
		result = recvfrom( m_socket, m_data, 1500, 0, (sockaddr*)&addr, &length );
		if ( result == SOCKET_ERROR )
		{
			agk::Error( "Failed to receive broadcast, listener stopped" );
			return 0;
		}
		if ( result > 0 ) 
		{
			strcpy( m_from, inet_ntoa( addr.sin_addr ) );
			(*callback)( result, m_data, m_from );
		}
	} while ( !m_bTerminate && result != 0 );

	return 0;
}
*/

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

// pass a port to listen for the broadcast and a callback to be used when a broadcast is received.
// the callback takes a length and data parameter, the data ptr is not guaranteed to exist after the 
// function finishes. It also sends the IP that sent the broadcast in the final char*.
/*
bool BroadcastListener::SetData( UINT port, void(*OnReceiveBroadcast)(int len, const char* data, const char* ip) )
{
	if ( !OnReceiveBroadcast ) 
	{
		agk::Error("Broadcast listener must supply a callback function");
		return false;
	}
	
	callback = OnReceiveBroadcast;
	m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	
	if ( m_socket == INVALID_SOCKET ) 
	{
		agk::Error( "Failed to create listening broadcast socket" );
		return false;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = INADDR_ANY;
	if ( bind( m_socket, (sockaddr*) &addr, sizeof(addr) ) == SOCKET_ERROR )
	{
		agk::Error( "Failed to bind listening socket" );
		return false;
	}

	return true;
}

void BroadcastListener::Stop()
{
	AGKThread::Stop();
	close( m_socket );
}
 */

bool BroadcastListener::SetListenPort( UINT port )
{
	SetListenPort( "", port );
	return true;
}

bool BroadcastListener::SetListenPort( const char *szIP, UINT port )
{
	if ( m_socket != INVALID_SOCKET ) 
	{
		shutdown( m_socket, 2 );
		close( m_socket );
	}

	m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( m_socket == INVALID_SOCKET ) 
	{
		agk::Warning( "Failed to create listening broadcast socket" );
		return false;
	}
	
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = INADDR_ANY;
	if ( bind( m_socket, (sockaddr*) &addr, sizeof(addr) ) == SOCKET_ERROR )
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
		strcpy( fromIP, inet_ntoa( addr.sin_addr ) );
		fromPort = addr.sin_port;
	}
	
	return true;
}

//*****************************
// Broadcaster
//*****************************

UINT Broadcaster::Run()
{
	int count = 0;

	int sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( sock == INVALID_SOCKET ) 
	{
		agk::Warning( "Failed to create broadcast packet" );
		return 0;
	}
		
	int opt = 1;
	int optlen = sizeof(int);
	setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, optlen );

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( m_port );
	addr.sin_addr.s_addr = inet_addr( "255.255.255.255" );
	int result = connect( sock, (sockaddr*) &addr, sizeof(addr) );
	if ( result == SOCKET_ERROR )
	{
		uString err;
		err.Format( "Failed to set broadcast packet address: %d", 0 );
		agk::Warning( err );
		return 0;
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
			agk::Warning( "Failed to set broadcast packet address" );
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

	if ( sock != INVALID_SOCKET )
	{
		shutdown( sock, 2 );
		close( sock );
	}

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

cHTTPConnection::cHTTPConnection()
{
	request = -1;
	m_iSecure = 0;
	m_iVerifyMode = 1;
	
	m_fProgress = 0;
	m_iStatusCode = 0;
	m_bFailed = false;
	
	m_szContentType[0] = '\0';
}

cHTTPConnection::~cHTTPConnection()
{
	Close();

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
	AGKThread::Stop();
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
	agk::Error( "The HTML5 version does not support custom headers" );
}

void cHTTPConnection::RemoveHeader( const char* headerName )
{
	agk::Error( "The HTML5 version does not support custom headers" );
}

bool cHTTPConnection::SetHost( const char *szHost, int iSecure, const char *szUser, const char *szPass )
{
	m_sHost.SetStr( szHost );
	if ( iSecure == 0 ) m_sHost.Prepend( "http://" );
	else m_sHost.Prepend( "https://" );
	
	m_iSecure = iSecure;

	if ( szUser ) m_sUsername.SetStr( szUser );
	else m_sUsername.SetStr( "" );

	if ( szPass ) m_sPassword.SetStr( szPass );
	else m_sPassword.SetStr( "" );
	
	return true;
}

void cHTTPConnection::Close()
{
	if ( request >= 0 ) emscripten_async_wget2_abort( request );
	request = -1;
	
	m_sHost.SetStr( "" );
}

void cHTTPConnection::http_onload( unsigned int, void* userdata, void* response, unsigned length )
{
	cHTTPConnection *pHTTP = (cHTTPConnection*) userdata;
	if ( !pHTTP ) return;
	
	pHTTP->m_sResponse.SetStr( (const char*)response );
	pHTTP->request = -1;
}

void cHTTPConnection::http_onerror( unsigned int, void* userdata, int errorcode, const char* errormsg )
{
	cHTTPConnection *pHTTP = (cHTTPConnection*) userdata;
	if ( !pHTTP ) return;
	
	uString err;
	err.Format( "HTTP request failed: %d - %s", errorcode, errormsg ? errormsg : "unknown error" );
	agk::Warning( err );
	pHTTP->m_bFailed = true;
	pHTTP->request = -1;
}

void cHTTPConnection::http_onprogress( unsigned int, void* userdata, int bytesloaded, int totalbytes )
{
	cHTTPConnection *pHTTP = (cHTTPConnection*) userdata;
	if ( !pHTTP ) return;
	if ( totalbytes == 0 ) return;
	
	pHTTP->m_fProgress = bytesloaded / (float)totalbytes;
}

void cHTTPConnection::http_file_onload( unsigned int, void* userdata, const char* filename )
{
	cHTTPConnection *pHTTP = (cHTTPConnection*) userdata;
	if ( !pHTTP ) return;
	
	pHTTP->request = -1;
}

void cHTTPConnection::http_file_onerror( unsigned int, void* userdata, int statuscode )
{
	cHTTPConnection *pHTTP = (cHTTPConnection*) userdata;
	if ( !pHTTP ) return;
	
	uString err;
	err.Format( "HTTP file download failed: %d", statuscode );
	agk::Warning( err );
	pHTTP->m_bFailed = true;
	pHTTP->request = -1;
}

void cHTTPConnection::http_file_onprogress( unsigned int, void* userdata, int percent )
{
	cHTTPConnection *pHTTP = (cHTTPConnection*) userdata;
	if ( !pHTTP ) return;
	
	pHTTP->m_fProgress = percent;
}

char* cHTTPConnection::SendRequest( const char *szServerFile, const char *szPostData )
{
	agk::Error( "HTML5 does not support SendHTTPRequest, use SendHTTPRequestASync instead" );
	return 0;
}

bool cHTTPConnection::SendRequestASync( const char *szServerFile, const char *szPostData )
{
	if ( request >= 0 ) 
	{
		agk::Error( "Cannot send HTTP whilst an async request or download is still in progress, wait for GetResponseReady or DownloadComplete to return 1" );
		return false;
	}
	
	m_bFailed = false;
	m_sResponse.SetStr("");
	m_fProgress = 0;
	m_iStatusCode = 0;
	
	uString sURL( m_sHost );
	sURL.Append( "/" );
	sURL.Append( szServerFile );
	
	request = emscripten_async_wget2_data( sURL.GetStr(), "POST", szPostData, this, 1, cHTTPConnection::http_onload, cHTTPConnection::http_onerror, cHTTPConnection::http_onprogress );
	
	return true;
}

bool cHTTPConnection::SendFile( const char *szServerFile, const char *szPostData, const char *szLocalFile )
{
	agk::Error( "HTML5 does not support SendHTTPFile" );
	return false;
}

int cHTTPConnection::GetResponseReady()
{
	if ( request >= 0 ) return 0;
	else if ( !m_bFailed ) return 1;
	else return -1;
}

const char* cHTTPConnection::GetResponse()
{
	return m_sResponse.GetStr();
}

const char* cHTTPConnection::GetContentType()
{
	return m_szContentType;
}

bool cHTTPConnection::DownloadFile( const char *szServerFile, const char *szLocalFile, const char *szPostData )
{
	if ( request >= 0 ) 
	{
		agk::Error( "Cannot send HTTP whilst an async request or download is still in progress, wait for GetResponseReady or DownloadComplete to return 1" );
		return false;
	}
	
	m_bFailed = false;
	m_sResponse.SetStr("");
	m_fProgress = 0;
	m_iStatusCode = 0;
	
	uString sURL( m_sHost );
	sURL.Append( "/" );
	sURL.Append( szServerFile );
	
	uString sLocalFile( szLocalFile );
	agk::PlatformGetFullPathWrite( sLocalFile );
	
	request = emscripten_async_wget2( sURL.GetStr(), sLocalFile.GetStr(), "POST", szPostData, this, cHTTPConnection::http_file_onload, cHTTPConnection::http_file_onerror, cHTTPConnection::http_file_onprogress );
	
	return true;
}

bool cHTTPConnection::DownloadComplete()
{
	return request >= 0 ? 0 : 1;
}
