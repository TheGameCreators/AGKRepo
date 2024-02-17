//#include "agk.h"
#include "Network.h"
#include "uString.h"
#include "cFile.h"
#include "Wrapper.h"
#include <Ws2tcpip.h>

using namespace AGK;

/* License for agk_inet_pton6() and agk_inet_ntop6() only
/* Copyright (c) 1996 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

static int agk_inet_pton6(const char *src, u_char *dst)
{
	static const char xdigits_l[] = "0123456789abcdef",
					xdigits_u[] = "0123456789ABCDEF";
	u_char tmp[16], *tp, *endp, *colonp;
	const char *xdigits, *curtok;
	int ch, saw_xdigit;
	u_int val;
	
	memset((tp = tmp), '\0', 16);
	endp = tp + 16;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
			if (*++src != ':')
					return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = *src++) != '\0') {
			const char *pch;
	
			if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
					pch = strchr((xdigits = xdigits_u), ch);
			if (pch != NULL) {
					val <<= 4;
					val |= (pch - xdigits);
					if (val > 0xffff)
							return (0);
					saw_xdigit = 1;
					continue;
			}
			if (ch == ':') {
					curtok = src;
					if (!saw_xdigit) {
							if (colonp)
									return (0);
							colonp = tp;
							continue;
					} else if (*src == '\0')
							return (0);
					if (tp + 2 > endp)
							return (0);
					*tp++ = (u_char) (val >> 8) & 0xff;
					*tp++ = (u_char) val & 0xff;
					saw_xdigit = 0;
					val = 0;
					continue;
			}
			/*
			if (ch == '.' && ((tp + 4) <= endp) && inet_pton4(curtok, tp, 1) > 0) {
					tp += 4;
					saw_xdigit = 0;
					break;  // '\0' was seen by inet_pton4().
			}
			*/
			return (0);
	}
	if (saw_xdigit) {
			if (tp + 2 > endp)
					return (0);
			*tp++ = (u_char) (val >> 8) & 0xff;
			*tp++ = (u_char) val & 0xff;
	}
	if (colonp != NULL) {
			/*
			* Since some memmove()'s erroneously fail to handle
			* overlapping regions, we'll do the shift by hand.
			*/
			const int n = (int)(tp - colonp);
			int i;
	
			if (tp == endp)
					return (0);
			for (i = 1; i <= n; i++) {
					endp[- i] = colonp[n - i];
					colonp[n - i] = 0;
			}
			tp = endp;
	}
	if (tp != endp)
			return (0);
	memcpy(dst, tmp, 16);
	return (1);
}

const char* agk_inet_ntop6(const u_char *src, char *dst, size_t size)
{
	/*
	* Note that int32_t and int16_t need only be "at least" large enough
	* to contain a value of the specified size.  On some systems, like
	* Crays, there is no such thing as an integer variable with 16 bits.
	* Keep this in mind if you think this function should have been coded
	* to use pointer overlays.  All the world's not a VAX.
	*/
	char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
	struct { int base, len; } best, cur;
	u_int words[16 / 2];
	int i;
	
	/*
	* Preprocess:
	*      Copy the input (bytewise) array into a wordwise array.
	*      Find the longest run of 0x00's in src[] for :: shorthanding.
	*/
	memset(words, '\0', sizeof words);
	for (i = 0; i < 16; i++)
			words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
	best.base = -1;
	cur.base = -1;
	for (i = 0; i < (16 / 2); i++) {
			if (words[i] == 0) {
					if (cur.base == -1)
							cur.base = i, cur.len = 1;
					else
							cur.len++;
			} else {
					if (cur.base != -1) {
							if (best.base == -1 || cur.len > best.len)
									best = cur;
							cur.base = -1;
					}
			}
	}
	if (cur.base != -1) {
			if (best.base == -1 || cur.len > best.len)
					best = cur;
	}
	if (best.base != -1 && best.len < 2)
			best.base = -1;
	
	/*
	* Format the result.
	*/
	tp = tmp;
	for (i = 0; i < (16 / 2); i++) {
			/* Are we inside the best run of 0x00's? */
			if (best.base != -1 && i >= best.base &&
				i < (best.base + best.len)) {
					if (i == best.base)
							*tp++ = ':';
					continue;
			}
			/* Are we following an initial run of 0x00s or any real hex? */
			if (i != 0)
					*tp++ = ':';
			/* Is this address an encapsulated IPv4? */
			if (i == 6 && best.base == 0 &&
				(best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
					//if (!inet_ntoa((src+12, tp, sizeof tmp - (tp - tmp)))
							return (NULL);
					tp += strlen(tp);
					break;
			}
			tp += sprintf(tp, "%x", words[i]);
	}
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) == (16 / 2))
			*tp++ = ':';
	*tp++ = '\0';
	
	/*
	* Check for overflow, copy, and we're done.
	*/
	if ((size_t)(tp - tmp) > size) {
			errno = WSAEAFNOSUPPORT;
			return (NULL);
	}
	strcpy(dst, tmp);
	return (dst);
}

int agk::PlatformGetIP( uString &sIP )
{
	char hostname[ 256 ];
	if ( gethostname( hostname, 256 ) ) return 0;

	addrinfo hints;
	ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	sockaddr_in *sockaddr_ipv4;
	addrinfo *pIPs;
	int result = getaddrinfo( hostname, NULL, &hints, &pIPs );
	if ( result ) return 0;

	if ( !pIPs ) return 0;

	while ( pIPs )
	{
		if ( pIPs->ai_family == AF_INET )
		{
			sockaddr_ipv4 = (sockaddr_in*) pIPs->ai_addr;
			if ( strncmp( inet_ntoa(sockaddr_ipv4->sin_addr), "127.0.0.1", strlen("127.0.0.1") ) != 0 )
			{
				sIP.SetStr( inet_ntoa(sockaddr_ipv4->sin_addr) );
				return 1;
			}
		}

		pIPs = pIPs->ai_next;
	}

	// always use something on Windows, even if it is the loopback address
	sIP.SetStr( "127.0.0.1" );
	return 1;
	//return 0;
}

int agk::PlatformGetIPv6( uString &sIP, int *iInterface )
{
	char hostname[ 256 ];
	if ( gethostname( hostname, 256 ) ) return 0;

	sIP.SetStr("");

	addrinfo hints;
	ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	sockaddr_in6 *sockaddr_ipv6;
	addrinfo *pIPs;
	int result = getaddrinfo( hostname, NULL, &hints, &pIPs );
	if ( result ) return 0;
	if ( !pIPs ) return 0;

	int currLevel = 0;

	while ( pIPs )
	{
		if ( pIPs->ai_family == AF_INET6 )
		{
			sockaddr_ipv6 = (sockaddr_in6*) pIPs->ai_addr;
			char szIP[ 65 ];
			agk_inet_ntop6( (const unsigned char*)&(sockaddr_ipv6->sin6_addr), szIP, 65 );

			int level = 0;
			if ( strncmp( szIP, "::1", 3 ) == 0 ) level = -1; // loopback
			else if ( strncmp( szIP, "ff", 2 ) == 0 ) level = -1; // multicast address
			else if ( strncmp( szIP, "fe", 2 ) == 0 ) level = 1; // link local address
			else if ( strncmp( szIP, "fd", 2 ) == 0 ) level = 2; // site local address
			else if ( strncmp( szIP, "fc", 2 ) == 0 ) level = 2; // site local address
			else level = 3;

			//uString info; info.Format( "IP: %s, Level: %d, Name: %s", szIP, level, pIPs->ai_canonname );
			//agk::Warning( info );

			if ( level > currLevel )
			{
				currLevel = level;
				sIP.SetStr( szIP );
			}
		}

		pIPs = pIPs->ai_next;
	}

	if ( sIP.GetLength() == 0 ) 
	{
		// always use something on Windows, even if it is the loopback address
		sIP.SetStr( "::1" );
		//return 0;
	}
	return 1;
}

//*********************
// UDPManager
//*********************

UDPManager::UDPManager( const char* szIP, uint32_t listenPort )
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
		else agk_inet_pton6( szIP, (u_char*)&(addrv6->sin6_addr) );
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
		closesocket( m_socket );
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
		closesocket( m_socket );
		m_socket = INVALID_SOCKET;
	}
}

bool UDPManager::SendPacket( const char *IP, uint32_t port, const AGKPacket *packet )
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
		agk_inet_pton6( IP, (u_char*)&(addrv6->sin6_addr) );
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
	if ( ioctlsocket( m_socket, FIONREAD, &waiting ) == SOCKET_ERROR )
	{
		agk::Warning( "Failed to get receivable bytes on socket" );
		return false;
	}

	if ( waiting == 0 ) return false;

	sockaddr_storage addr;
	int length = sizeof(addr);
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
			if ( fromIP ) agk_inet_ntop6( (const unsigned char*)&(addrv6->sin6_addr), fromIP, 100 ); // don't know size so guess 100
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
	if ( ioctlsocket( m_socket, FIONREAD, &waiting ) == SOCKET_ERROR )
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

void AGKSocketTimeout::SetData( AGKSocket *pSocket, uint32_t iTimeout )
{
	m_pSocket = pSocket;
	m_iTimeout = iTimeout;
}


uint32_t AGKSocketTimeout::Run()
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


uint32_t AGKSocket::Run()
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
		//inet_pton( AF_INET6, m_szIP, &(addrv6->sin6_addr) );
		agk_inet_pton6( m_szIP, (u_char*)&(addrv6->sin6_addr) );
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
			closesocket( m_client );
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

AGKSocket::AGKSocket( SOCKET s )
{
	Reset();
	m_client = s;
	m_bConnected = true;
	
	sockaddr_storage addr;
	int length = sizeof(addr);
	getpeername( m_client, (sockaddr*)&addr, &length );
	if ( addr.ss_family == AF_INET6 )
	{
		sockaddr_in6 *addrv6 = (sockaddr_in6*) &addr;
		m_port = ntohs(addrv6->sin6_port);
		//inet_ntop( AF_INET6, &(addrv6->sin6_addr), m_szIP, sizeof(m_szIP) );
		agk_inet_ntop6( (const unsigned char*)&(addrv6->sin6_addr), m_szIP, sizeof(m_szIP) );
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
		// never used, and may not work anyway
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
		closesocket( m_client );
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
		closesocket( m_client );
	}
	m_client = INVALID_SOCKET;
}	

bool AGKSocket::Connect( const char* IP, uint32_t port, uint32_t timeout )
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
		//inet_pton( AF_INET6, m_szIP, &(addrv6->sin6_addr) );
		agk_inet_pton6( m_szIP, (u_char*)&(addrv6->sin6_addr) );
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
			closesocket( m_client );
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

bool AGKSocket::ConnectASync( const char* IP, uint32_t port, uint32_t timeout )
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

void AGKSocket::SetTimeout( int iLength )
{
	// limit send commands to 5 seconds before time out
	// only use for high-speed activities such as transmitting data from the AGK Viewer
	//if ( m_client == INVALID_SOCKET ) return;
	//struct timeval tv; 
	//tv.tv_sec = iLength;
	//tv.tv_usec = 0;
	//setsockopt ( m_client, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,  sizeof tv );
}

bool AGKSocket::SendFile( const char* szFilename )
{
	if ( m_bDisconnected ) return false;
	if ( !szFilename ) return false;
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to send file on an unconnected socket" );
#endif
		return false;
	}

	cFile pFile;	
	if ( !pFile.OpenToRead( szFilename ) )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Could not send network file" );
#endif
		return false;
	}

	uint32_t size = pFile.GetSize();
	
	SendUInt( size );
	Flush();

	if ( size == 0 ) return true;

	uint32_t iSent = 0;
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
#ifdef _AGK_ERROR_CHECK
			uString sErr;
			sErr.Format( "Failed to send file data %s %d", szFilename, WSAGetLastError() );
			agk::Warning( sErr );
#endif
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to send data on an unconnected socket" );
#endif
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
		
		uint32_t iSent = space;
		while ( length-iSent >= AGK_NET_SEND_BUFFER_SIZE )
		{
			int result = 0;
			int sent = 0;
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to send string on an unconnected socket" );
#endif
		return false;
	}

	uint32_t length = (uint32_t) strlen( s );
	if ( !SendUInt( length ) ) return false;
	if ( length > 0 ) return SendData( s, length );
	return true;
}

bool AGKSocket::SendChar( char c )
{
	if ( m_bDisconnected ) return false;
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to send char on an unconnected socket" );
#endif
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

bool AGKSocket::SendUInt( uint32_t u )
{
	if ( m_bDisconnected ) return false;
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to send uint on an unconnected socket" );
#endif
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to send int on an unconnected socket" );
#endif
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to send float on an unconnected socket" );
#endif
		return false;
	}

	if ( m_iSendWritePtr+4 > AGK_NET_SEND_BUFFER_SIZE )
	{
		Flush();
	}

	// add float to buffer
	memcpy( m_sendBuffer + m_iSendWritePtr, &f, 4 ); // ARM does not like unaligned 4 byte access, so copy per byte
	m_iSendWritePtr += 4;
	return true;
}

bool AGKSocket::Flush()
{
	if ( m_bDisconnected ) return false;
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to flush data on an unconnected socket" );
#endif
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to get available bytes on an unconnected socket" );
#endif
		return 0;
	}

	unsigned long waiting = 0;
	if ( ioctlsocket( m_client, FIONREAD, &waiting ) == SOCKET_ERROR )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Failed to get receivable bytes on socket" );
#endif
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to receive data on an unconnected socket" );
#endif
		return 0;
	}

	if ( !data || length <= 0 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Invalid network buffer passed to RecvData()" );
#endif
		return 0;
	}

	int result = recv( m_client, data, length, 0 );
	if ( result == SOCKET_ERROR || result == 0 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Failed to get socket data" );
#endif
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to receive string on an unconnected socket" );
#endif
		return -1;
	}

	// get string length
	uint32_t length = RecvUInt();
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to receive char on an unconnected socket" );
#endif
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to receive int on an unconnected socket" );
#endif
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

uint32_t AGKSocket::RecvUInt()
{
	if ( m_bDisconnected ) return 0;
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to receive uint on an unconnected socket" );
#endif
		return 0;
	}

	uint32_t u = 0;
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
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Tried to receive float on an unconnected socket" );
#endif
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


uint32_t cNetworkListener::Run()
{
	while ( !m_bTerminate )
	{
		if ( m_socket == INVALID_SOCKET ) SleepSafe(1000);
		else
		{
			SOCKET client = accept( m_socket, NULL, NULL );
			if ( m_bTerminate ) return 0;
			if ( client == INVALID_SOCKET )
			{
#ifdef _AGK_ERROR_CHECK
				agk::Warning( "Failed to accept connection" );
#endif
				continue;
			}

			int opt = 1;
			int optlen = sizeof(int);
			setsockopt( client, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, optlen );
			
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
		closesocket( m_socket );
		m_socket = INVALID_SOCKET;
	}
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

int cNetworkListener::AcceptConnections( uint32_t port )
{
	return AcceptConnections( "", port );
}

int cNetworkListener::AcceptConnections( const char *szIP, uint32_t port )
{
	if ( m_socket != INVALID_SOCKET ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString err;
		err.Format( "Failed to listen on port %d, this socket is already listening on port %d", port, m_port );
		agk::Error( err );
#endif
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
		else 
		{
			//inet_pton( AF_INET6, szIP, &(addrv6->sin6_addr) );
			agk_inet_pton6( szIP, (u_char*)&(addrv6->sin6_addr) );
		}
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
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Failed to create listening socket" );
#endif
		return 0;
	}

	int value = 1;
    setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value) );
	if ( family == AF_INET6 ) setsockopt( m_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&value, sizeof(value) ); // don't listen on IPV4 addresses

	if ( bind( m_socket, (sockaddr*) &addr, size ) == SOCKET_ERROR )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Failed to bind listening socket" );
#endif
		closesocket( m_socket );
		m_socket = INVALID_SOCKET;
		return 0;
	}

	m_port = port;

	if ( listen( m_socket, 5 ) == SOCKET_ERROR )
	{
#ifdef _AGK_ERROR_CHECK
		int err = WSAGetLastError();
		uString errStr;
		errStr.Format( "Failed to start socket listening: %d", err );
		agk::Warning( errStr );
#endif
		closesocket( m_socket );
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
	//callback = 0;
	m_socket = INVALID_SOCKET;
}

BroadcastListener::~BroadcastListener()
{
	//Stop();
	//Join();
	if ( m_socket != INVALID_SOCKET ) 
	{
		shutdown( m_socket, 2 );
		closesocket( m_socket );
	}
}

void BroadcastListener::Close()
{
	if ( m_socket != INVALID_SOCKET ) 
	{
		shutdown( m_socket, 2 );
		closesocket( m_socket );
	}
	m_socket = INVALID_SOCKET;
}

bool BroadcastListener::SetListenPort( uint32_t port )
{
	return SetListenPort( "", port );
}

bool BroadcastListener::SetListenPort( const char *szIP, uint32_t port )
{
	if ( m_socket != INVALID_SOCKET ) 
	{
		shutdown( m_socket, 2 );
		closesocket( m_socket );
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
		else 
		{
			//inet_pton( AF_INET6, szIP, &(addrv6->sin6_addr) );
			agk_inet_pton6( szIP, (u_char*)&(addrv6->sin6_addr) );
		}
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
			//inet_pton( AF_INET6, szIP, &(mreq.ipv6mr_multiaddr) );
			agk_inet_pton6( szIP, (u_char*)&(mreq.ipv6mr_multiaddr) );
			setsockopt(m_socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)&mreq, sizeof(mreq)) ;
		}
	}

	if ( bind( m_socket, (sockaddr*) &addr, size ) == SOCKET_ERROR )
	{
		int err = WSAGetLastError();
		uString sErr; sErr.Format( "Failed to bind listening socket: %d", err );
		agk::Warning( sErr );
		return false;
	}

	return true;
}

bool BroadcastListener::ReceivedBroadcast()
{
	if ( m_socket == INVALID_SOCKET ) return false;

	unsigned long waiting = 0;
	if ( ioctlsocket( m_socket, FIONREAD, &waiting ) == SOCKET_ERROR ) return false;
	return waiting > 0;
}

bool BroadcastListener::GetPacket( AGKPacket &packet, uint32_t &fromPort, char *fromIP )
{
	if ( m_socket == INVALID_SOCKET ) return false;

	int length = sizeof(addr);
	int result = recvfrom( m_socket, packet.GetRaw(), AGK_NET_PACKET_SIZE, 0, (sockaddr*)&addr, &length );
	if ( result == SOCKET_ERROR )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Failed to receive broadcast" );
#endif
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
			//inet_ntop( AF_INET6, &(addrv6->sin6_addr), fromIP, 65 );
			agk_inet_ntop6( (const unsigned char*)&(addrv6->sin6_addr), fromIP, 65 );
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


uint32_t Broadcaster::Run()
{
	int count = 0;
	SOCKET sock = 0;

	if ( m_ipv6 )
	{
		sock = socket( AF_INET6, SOCK_DGRAM, IPPROTO_UDP );
		if ( sock == INVALID_SOCKET ) 
		{
			agk::Warning( "Failed to create IPv6 broadcast packet, make sure your device is connected to a Wi-Fi network" );
			return 0;
		}
	}
	else
	{	
		sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		if ( sock == INVALID_SOCKET ) 
		{
			agk::Warning( "Failed to create IPv4 broadcast packet, make sure your device is connected to a Wi-Fi network" );
			return 0;
		}
	}
		
	BOOL opt = TRUE;
	int optlen = sizeof(BOOL);
	
	if ( m_ipv6 ) 
	{
		//setsockopt( sockv6, SOL_SOCKET, SO_BROADCAST, (char*)&opt, optlen );
		sockaddr_in6 addr;
		memset(&addr, 0, sizeof(sockaddr_in6));
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons( m_port );
		//inet_pton( AF_INET6, "FF02::41:474B", &(addr.sin6_addr) ); // link local AGK multicast address
		agk_inet_pton6( "FF02::41:474B", (u_char*)&(addr.sin6_addr) );
		int result = connect( sock, (sockaddr*) &addr, sizeof(addr) );
		if ( result == SOCKET_ERROR )
		{
			agk::Warning( "Failed to set broadcast packet address for IPv6" );
			closesocket( sock );
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
			closesocket( sock );
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
			closesocket( sock );
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
	closesocket( sock );
	
	return 0;
}

Broadcaster::Broadcaster()
{
	m_interval = 1000;
	m_max = 0;
}

// sets the data that will be broadcast every interval milliseconds for a maximum of max broadcasts (0=forever)
// start with Start() and stop with Stop()
void Broadcaster::SetData( int ipv6, uint32_t port, const AGKPacket* packet, uint32_t interval, int max )
{
	if ( !packet ) return;
	if ( interval < 1000 ) interval = 1000; //minimum interval of 1 seond to stop flooding the network
	if ( packet->GetSize() > 512 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Attempted to broadcast more than 512 bytes" );
#endif
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

HINTERNET cHTTPConnection::m_hInet = 0;

cHTTPConnection::cHTTPConnection()
{
	m_sURL = 0;
	m_hInetConnect = NULL;
	m_iSecure = 0;
	m_bConnected = false;
	m_iTimeout = 6000;
	m_iVerifyMode = 1;

	m_szResponse = 0;
	m_fProgress = 0;
	m_iStatusCode = 0;
	m_szContentType[0] = '\0';

	m_bSaveToFile = false;
}

cHTTPConnection::~cHTTPConnection()
{
	Close();
	if ( m_szResponse ) delete [] m_szResponse;
	m_szResponse = 0;

	cHTTPHeader *pHeader = m_cHeaders.GetFirst();
	while( pHeader )
	{
		delete pHeader;
		pHeader = m_cHeaders.GetNext();
	}
	m_cHeaders.ClearAll();
}


uint32_t cHTTPConnection::Run()
{
	if ( m_szUploadFile.GetLength() > 0 ) m_szResponse = SendFileInternal();
	else m_szResponse = SendRequestInternal();
	return 0;
}

void cHTTPConnection::Stop()
{
	AGKThread::Stop();

	m_bConnected = false;
	if ( m_hInetConnect ) InternetCloseHandle( m_hInetConnect );
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
	if ( IsRunning() )
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
	if ( IsRunning() )
	{
		agk::Warning( "Cannot change HTTP headers whilst an async request or download is still in progress, wait for GetHTTPResponseReady() or GetHTTPFileComplete() to return 1" );
		return;
	}

	cHTTPHeader *pHeader = m_cHeaders.RemoveItem( headerName );
	if ( pHeader ) delete pHeader;
}

bool cHTTPConnection::SetHost( const char *szHost, int iSecure, const char *szUser, const char *szPass )
{
	if ( !szHost ) return false;
	if ( IsRunning() )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot make a new HTTP connection whilst an async request or download is still in progress, wait for GetHTTPResponseReady() or GetHTTPFileComplete() to return 1" );
#endif
		return NULL;
	}

	if ( !m_hInet )
	{
		m_hInet = InternetOpen(	"InternetConnection", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( !m_hInet )
		{
#ifdef _AGK_ERROR_CHECK
			agk::Warning( "Failed to initialise internet handler" );
#endif
			return false;
		}
	}

	m_iSecure = iSecure;
	m_sUsername.SetStr( szUser );
	m_sPassword.SetStr( szPass );

	if ( m_hInetConnect )
	{
		if ( m_iSecure == iSecure && strcmp( m_sURL, szHost ) == 0 ) return true;
		else Close();
	}

	WORD wHTTPType = iSecure ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;

	m_hInetConnect = InternetConnect( m_hInet, szHost, wHTTPType, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
	if ( !m_hInetConnect )
	{
#ifdef _AGK_ERROR_CHECK
		uString err( "Failed to connect to connect to " );
		err += szHost;
		agk::Warning( err );
#endif
		return false;
	}

	DWORD connections = 8;
	InternetSetOption( m_hInetConnect, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &connections, sizeof(connections) );
	InternetSetOption( m_hInetConnect, INTERNET_OPTION_CONNECT_TIMEOUT, &m_iTimeout, sizeof(m_iTimeout) );  
	BOOL decode = TRUE;
	InternetSetOption( m_hInetConnect, INTERNET_OPTION_HTTP_DECODING, &decode, sizeof(decode) );  

	m_sURL = new char[ strlen(szHost) + 1 ];
	strcpy( m_sURL, szHost );

	m_bConnected = true;
	return true;
}

void cHTTPConnection::Close()
{
	Stop();
	Join();
	
	m_hInetConnect = 0;
	
	if ( m_sURL ) delete [] m_sURL;
	m_sURL = 0;
}

char* cHTTPConnection::SendFileInternal()
{
	cFile oFile;
	m_fProgress = 0;
	m_iStatusCode = 0;
	if ( m_szServerFile.GetLength() == 0 ) return 0;
	
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP file as no host is set, or Close() has been called" );
#endif
		return NULL;
	}

	if ( m_bSaveToFile )
	{
		if ( !m_szLocalFile ) 
		{
#ifdef _AGK_ERROR_CHECK
			agk::Warning( "Cannot download file, no local filename set." );
#endif
			return NULL;
		}

		if ( !oFile.OpenToWrite( m_szLocalFile ) )
		{
#ifdef _AGK_ERROR_CHECK
			agk::Warning( "Cannot download file, failed to open local file for writing." );
#endif
			return NULL;
		}
		oFile.Close();
	}

	if ( m_szUploadFile.GetLength() == 0 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP file as no upload file name has been set" );
#endif
		return NULL;
	}

	cFile fFile;
	if ( !fFile.OpenToRead( m_szUploadFile ) )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP file, failed to open file for reading" );
#endif
		return NULL;
	}

	uString sFinalPostData;
	char sName[ 256 ];
	char sValue[ 512 ];
	const char* remaining = m_szPostData.GetStr();
	int count = m_szPostData.Count( '&' ) + 1;
	for ( int i = 0; i < count; i++ )
	{
		int pos = (int) strcspn( remaining, "=" );
		strncpy( sName, remaining, pos ); 
		sName[ pos ] = '\0';
		remaining += pos+1;
		pos = (int) strcspn( remaining, "&" );
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

	m_szUploadFile.Replace( '\\', '/' );
	uString sFilename;
	int pos = m_szUploadFile.RevFind( '/' );
	if ( pos >= 0 ) m_szUploadFile.SubString( sFilename, pos+1 );
	else sFilename.SetStr( m_szUploadFile );

	sFinalPostData.Append( "--------------------AaB03x\r\nContent-Disposition: form-data; name=\"myfile\"; filename=\"" );
	sFinalPostData.Append( sFilename );
	sFinalPostData.Append( "\"\r\nContent-Type: application/x-zip-compressed\r\n\r\n" );

	uString sEndPostData( "\r\n--------------------AaB03x--\r\n" );

	uint32_t iTotalLength = sFinalPostData.GetLength() + fFile.GetSize() + sEndPostData.GetLength();

	DWORD dwFlag = INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_NO_CACHE_WRITE;
	if ( m_iSecure == 1 ) dwFlag |= INTERNET_FLAG_SECURE;

	// prepare request
	HINTERNET hHttpRequest = HttpOpenRequest( m_hInetConnect, "POST", m_szServerFile, "HTTP/1.1", "The Agent", NULL, dwFlag, 0 );
	if ( !hHttpRequest )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Failed to open HTTP request" );
#endif
		return NULL;
	}

	if ( !m_bConnected ) return 0;

	// add Content-Length header
	uString sHeader; 
	sHeader.Format( "Content-Length: %d", iTotalLength );
	HttpAddRequestHeaders( hHttpRequest, sHeader.GetStr(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE );

	// add Authorization header
	if ( m_sUsername.GetLength() > 0 || m_sPassword.GetLength() > 0 )
	{
		sHeader.Format( "%s:%s", m_sUsername.GetStr(), m_sPassword.GetStr() );
		char* base64 = agk::StringToBase64( sHeader.GetStr() );
		sHeader.Format( "Authorization: Basic %s", base64 );
		delete [] base64;
		HttpAddRequestHeaders( hHttpRequest, sHeader.GetStr(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE );
	}

	cHTTPHeader *pHeader = m_cHeaders.GetFirst();
	while( pHeader )
	{
		sHeader.Format( "%s: %s", pHeader->sName.GetStr(), pHeader->sValue.GetStr() );
		HttpAddRequestHeaders( hHttpRequest, sHeader.GetStr(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE );
		pHeader = m_cHeaders.GetNext();
	}
	
	// add Content-Type header
	HttpAddRequestHeaders( hHttpRequest, "Content-Type: multipart/form-data; boundary=------------------AaB03x", -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE );

	// start request
	BOOL bSendResult = HttpSendRequestEx( hHttpRequest, NULL, NULL, NULL, NULL );
	if ( !bSendResult )
	{
#ifdef _AGK_ERROR_CHECK
		uString err( "Failed to send request with error " );
		uint32_t iError = ::GetLastError( );
		char *szError = 0;
		if ( iError > 12000 && iError < 12174 ) FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE, GetModuleHandle("wininet.dll"), iError, 0, (char*)&szError, 0, 0 );
		else FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, iError, 0, (char*)&szError, 0, 0 );
		err += iError;
		if ( szError )
		{
			err += ": ";
			err += szError;
			LocalFree( szError );
		}
		agk::Warning( err );
#endif
		return 0;
	}

	DWORD iWritten = 0;
	uint32_t iTotalWritten = 0;
	// send initial post data
	bSendResult = InternetWriteFile( hHttpRequest, (void*)(sFinalPostData.GetStr()), sFinalPostData.GetLength(), &iWritten );
	if ( !bSendResult )
	{
		uString error;
		error.Format( "Failed to send initial data: %d", ::GetLastError( ) );
		agk::Warning( error );
		return NULL;
	}
	iTotalWritten += iWritten;

	char bytes[ 20000 ];

	//send file data a chunk at a time, allows progress bar to update
	while ( !fFile.IsEOF() && !m_bTerminate )
	{
		int bytesread = fFile.ReadData( bytes, 20000 );
		if ( bytesread > 0 )
		{
			bSendResult = InternetWriteFile( hHttpRequest, (void*)bytes, bytesread, &iWritten );
			if ( !bSendResult )
			{
				uString error;
				error.Format( "Failed to send file data: %d", ::GetLastError( ) );
				agk::Warning( error );
				return NULL;
			}

			iTotalWritten += iWritten;
			m_fProgress = iTotalWritten*100.0f / iTotalLength;
		}
	}

	fFile.Close();

	if ( m_bTerminate ) 
	{
		uString error( "Could not upload file, upload aborted by user" );
		agk::Warning( error );
		return NULL;
	}

	// send ending
	bSendResult = InternetWriteFile( hHttpRequest, (void*)(sEndPostData.GetStr()), sEndPostData.GetLength(), &iWritten );
	if ( !bSendResult )
	{
		uString error;
		error.Format( "Failed to send final data: %d", ::GetLastError( ) );
		agk::Warning( error );
		return NULL;
	}

	//close request
	bSendResult = HttpEndRequest( hHttpRequest, NULL, NULL, NULL );
	if ( !bSendResult )
	{
		uString error;
		error.Format( "Failed to close request: %d", ::GetLastError( ) );
		agk::Warning( error );
		return NULL;
	}

	m_fProgress = 99.9f;

	// finished sending, start receiving

	if ( !m_bConnected ) return 0;

	DWORD dwBufferSize = sizeof(int);
	DWORD dwHeaderIndex = 0;
	BOOL bReturnHeader = HttpQueryInfo( hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, (void*)&m_iStatusCode, &dwBufferSize, &dwHeaderIndex );

	DWORD dwContentLength = 0;
	dwBufferSize = sizeof(DWORD);
	dwHeaderIndex = 0;
	bReturnHeader = HttpQueryInfo( hHttpRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (void*)&dwContentLength, &dwBufferSize, &dwHeaderIndex );

	dwHeaderIndex = 0;
	DWORD ContentTypeLength = 150;
	HttpQueryInfo( hHttpRequest, HTTP_QUERY_CONTENT_TYPE, (void*)m_szContentType, &ContentTypeLength, &dwHeaderIndex );

	if ( !m_bConnected ) return 0;

	char* pDataReturned = 0;
	DWORD dwArraySize = 0;
	DWORD dwDataLength = 0;

	// server is not guaranteed to send a content length
	if ( dwContentLength > 0 && !m_bSaveToFile )
	{
		pDataReturned = new char [ dwContentLength + 1 ];
		dwArraySize = dwContentLength + 1;
	}

	if ( m_bSaveToFile ) oFile.OpenToWrite( m_szLocalFile );

	// create a read buffer
	char pBuffer[ 20000 ];

	// data comes in chunks
	for(;;)
	{
		// read the data from the HINTERNET handle.
		DWORD written = 0;
		if( !InternetReadFile( hHttpRequest, (LPVOID) pBuffer, 20000, &written ) )
		{
#ifdef _AGK_ERROR_CHECK
			// failed to read the data
			agk::Warning( "Failed to received HTTP response." );
#endif
			if ( pDataReturned ) delete [] pDataReturned;
			if ( m_bSaveToFile ) oFile.Close();
			return 0;
		}

		// Check the size of the remaining data. If it is zero it reached the end.
		if ( written == 0 ) break;

		// Add part-data to overall data returned
		DWORD dwNewDataSize = dwDataLength + written;

		if ( m_bSaveToFile )
		{
			oFile.WriteData( pBuffer, written );
		}
		else
		{
			if ( dwArraySize <= dwNewDataSize )
			{
				// recreate a bigger array in 10KB chunks
				LPSTR pNewData = new char [ dwNewDataSize + 20000 ];
				
				if ( pDataReturned && dwDataLength > 0 ) 
				{
					memcpy( pNewData, pDataReturned, dwDataLength );
					delete [] pDataReturned;
				}
				pDataReturned = pNewData;

				dwArraySize = dwNewDataSize + 20000;
			}

			memcpy( pDataReturned + dwDataLength, pBuffer, written );
		}

		dwDataLength = dwNewDataSize;
	}

	if ( m_bSaveToFile ) oFile.Close();

	m_fProgress = 100;

	InternetCloseHandle( hHttpRequest );

	if ( !m_bConnected ) 
	{
		if ( pDataReturned ) delete [] pDataReturned;
		return 0;
	}

	if ( pDataReturned ) pDataReturned [ dwDataLength ] = '\0';
	else
	{
		pDataReturned = new char[1];
		*pDataReturned = 0;
	}
	return pDataReturned;
}

char* cHTTPConnection::SendRequestInternal()
{
	cFile oFile;
	m_fProgress = 0;
	m_iStatusCode = 0;
	if ( m_szServerFile.GetLength() == 0 ) return 0;
	
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP request as no host is set, or Close() has been called" );
#endif
		return NULL;
	}

	if ( m_bSaveToFile )
	{
		if ( !m_szLocalFile ) 
		{
#ifdef _AGK_ERROR_CHECK
			agk::Warning( "Cannot download file, no local filename set." );
#endif
			return NULL;
		}

		if ( !oFile.OpenToWrite( m_szLocalFile ) )
		{
#ifdef _AGK_ERROR_CHECK
			agk::Warning( "Cannot download file, failed to open local file for writing." );
#endif
			return NULL;
		}
	}

	DWORD dwFlag = INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_NO_CACHE_WRITE;
	if ( m_iSecure == 1 ) dwFlag |= INTERNET_FLAG_SECURE;

	char *szType = m_szPostData.GetLength() > 0 ? "POST" : "GET";

	// prepare request
	HINTERNET hHttpRequest = HttpOpenRequest( m_hInetConnect, szType, m_szServerFile, "HTTP/1.1", NULL, NULL, dwFlag, 0 );
	if ( !hHttpRequest )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Failed to open HTTP request" );
#endif
		return NULL;
	}

	if ( !m_bConnected ) return 0;

	// add Authorization header
	uString sHeader; 
	if ( m_sUsername.GetLength() > 0 || m_sPassword.GetLength() > 0 )
	{
		sHeader.Format( "%s:%s", m_sUsername.GetStr(), m_sPassword.GetStr() );
		char* base64 = agk::StringToBase64( sHeader.GetStr() );
		sHeader.Format( "Authorization: Basic %s", base64 );
		delete [] base64;
		HttpAddRequestHeaders( hHttpRequest, sHeader.GetStr(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE );
	}
	
	// add Content-Type header
	if ( m_szPostData.GetLength() > 0 ) 
	{
		HttpAddRequestHeaders( hHttpRequest, "Content-Type: application/x-www-form-urlencoded", -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE );
	}

	cHTTPHeader *pHeader = m_cHeaders.GetFirst();
	while( pHeader )
	{
		sHeader.Format( "%s: %s", pHeader->sName.GetStr(), pHeader->sValue.GetStr() );
		HttpAddRequestHeaders( hHttpRequest, sHeader.GetStr(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE );
		pHeader = m_cHeaders.GetNext();
	}

	// post data length
	uint32_t iPostLength = m_szPostData.GetLength();

	// send request with header
	BOOL bSendResult = HttpSendRequest( hHttpRequest, NULL, 0, iPostLength ? (void*)(m_szPostData.GetStr()) : 0, iPostLength );
	if ( !bSendResult )
	{
#ifdef _AGK_ERROR_CHECK
		uString err( "Failed to send request with error " );
		uint32_t iError = ::GetLastError( );
		char *szError = 0;
		if ( iError > 12000 && iError < 12174 ) FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE, GetModuleHandle("wininet.dll"), iError, 0, (char*)&szError, 0, 0 );
		else FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, iError, 0, (char*)&szError, 0, 0 );
		err += iError;
		if ( szError )
		{
			err += ": ";
			err += szError;
			LocalFree( szError );
		}
		agk::Warning( err );
#endif
		return 0;
	}

	if ( !m_bConnected ) return 0;

	DWORD dwBufferSize = sizeof(int);
	DWORD dwHeaderIndex = 0;
	BOOL bReturnHeader = HttpQueryInfo( hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, (void*)&m_iStatusCode, &dwBufferSize, &dwHeaderIndex );

	dwHeaderIndex = 0;
	DWORD dwContentLength = 0;
	bReturnHeader = HttpQueryInfo( hHttpRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (void*)&dwContentLength, &dwBufferSize, &dwHeaderIndex );

	dwHeaderIndex = 0;
	DWORD ContentTypeLength = 150;
	HttpQueryInfo( hHttpRequest, HTTP_QUERY_CONTENT_TYPE, (void*)m_szContentType, &ContentTypeLength, &dwHeaderIndex );

	if ( !m_bConnected ) return 0;

	char* pDataReturned = 0;
	DWORD dwArraySize = 0;
	DWORD dwDataLength = 0;

	// server is not guaranteed to send a content length
	if ( dwContentLength > 0 && !m_bSaveToFile )
	{
		pDataReturned = new char [ dwContentLength + 1 ];
		dwArraySize = dwContentLength + 1;
	}

	// create a read buffer
	char pBuffer[ 20000 ];

	// data comes in chunks
	for(;;)
	{
		// read the data from the HINTERNET handle.
		DWORD written = 0;
		if( !InternetReadFile( hHttpRequest, (LPVOID) pBuffer, 20000, &written ) )
		{
#ifdef _AGK_ERROR_CHECK
			// failed to read the data
			agk::Warning( "Failed to received HTTP response." );
#endif
			if ( pDataReturned ) delete [] pDataReturned;
			return 0;
		}

		// Check the size of the remaining data. If it is zero it reached the end.
		if ( written == 0 ) break;

		// Add part-data to overall data returned
		DWORD dwNewDataSize = dwDataLength + written;

		if ( m_bSaveToFile )
		{
			oFile.WriteData( pBuffer, written );
		}
		else
		{
			if ( dwArraySize <= dwNewDataSize )
			{
				// recreate a bigger array in 10KB chunks
				LPSTR pNewData = new char [ dwNewDataSize + 20000 ];
				
				if ( pDataReturned && dwDataLength > 0 ) 
				{
					memcpy( pNewData, pDataReturned, dwDataLength );
					delete [] pDataReturned;
				}
				pDataReturned = pNewData;

				dwArraySize = dwNewDataSize + 20000;
			}

			memcpy( pDataReturned + dwDataLength, pBuffer, written );
		}

		dwDataLength = dwNewDataSize;
		if ( dwContentLength > 0 )
		{
			m_fProgress = dwDataLength*100.0f / dwContentLength;
		}
	}

	m_fProgress = 100;

	if ( m_bSaveToFile ) oFile.Close();

	InternetCloseHandle( hHttpRequest );

	if ( !m_bConnected ) 
	{
		if ( pDataReturned ) delete [] pDataReturned;
		return 0;
	}

	if ( pDataReturned ) pDataReturned [ dwDataLength ] = '\0';
	else
	{
		pDataReturned = new char[1];
		*pDataReturned = 0;
	}
	return pDataReturned;
}

char* cHTTPConnection::SendRequest( const char *szServerFile, const char *szPostData )
{
	if ( !szServerFile ) return 0;
	if ( IsRunning() )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP whilst an async request or download is still in progress, wait for GetHTTPResponseReady() or GetHTTPFileComplete() to return 1" );
#endif
		return 0;
	}

	m_szServerFile.SetStr( szServerFile );
	m_szPostData.SetStr( szPostData );
	m_szUploadFile.SetStr( "" );
	m_bSaveToFile = false;

	return SendRequestInternal();
}

bool cHTTPConnection::SendRequestASync( const char *szServerFile, const char *szPostData )
{
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP request as SetHTTPHost has not been called or failed to complete successfully" );
#endif
		return false;
	}

	if ( IsRunning() )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP whilst an async request or download is still in progress, wait for GetHTTPResponseReady() or GetHTTPFileComplete() to return 1" );
#endif
		return false;
	}

	if ( m_szResponse ) delete [] m_szResponse;
	m_szResponse = 0;
	m_fProgress = 0;
	m_iStatusCode = 0;
	m_szServerFile.SetStr( szServerFile );
	m_szPostData.SetStr( szPostData );
	m_szUploadFile.SetStr( "" );
	m_bSaveToFile = false;

	Start();

	return true;
}

bool cHTTPConnection::SendFile( const char *szServerFile, const char *szPostData, const char *szLocalFile )
{
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot send HTTP file as Connect() has not been called or failed to complete successfully" );
#endif
		return false;
	}

	if ( IsRunning() )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP file whilst an async request or download is still in progress, wait for GetHTTPResponseReady() or GetHTTPFileComplete() to return 1" );
#endif
		return false;
	}

	if ( !szLocalFile || !*szLocalFile )
	{
		agk::Error( "Cannot send HTTP file, filename is empty" );
		return false;
	}

	if ( m_szResponse ) delete [] m_szResponse;
	m_szResponse = 0;
	m_fProgress = 0;
	m_iStatusCode = 0;
	m_szServerFile.SetStr( szServerFile );
	m_szLocalFile.SetStr( "" );
	m_szPostData.SetStr( szPostData );
	m_szUploadFile.SetStr( szLocalFile );
	m_bSaveToFile = false;

	Start();

	return true;
}

int cHTTPConnection::GetResponseReady()
{
	if ( IsRunning() ) return 0;
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
	if ( !m_bConnected )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP request as Connect() has not been called or failed to complete successfully" );
#endif
		return false;
	}

	if ( IsRunning() )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Warning( "Cannot send HTTP whilst an async request or download is still in progress, wait for GetHTTPResponseReady() or GetHTTPFileComplete() to return 1" );
#endif
		return false;
	}

	if ( m_szResponse ) delete [] m_szResponse;
	m_szResponse = 0;
	m_fProgress = 0;
	m_iStatusCode = 0;
	m_szServerFile.SetStr( szServerFile );
	m_szLocalFile.SetStr( szLocalFile );
	m_szPostData.SetStr( szPostData );
	m_szUploadFile.SetStr( "" );
	m_bSaveToFile = true;

	Start();

	return true;
}

bool cHTTPConnection::DownloadComplete()
{
	return !(IsRunning());
}
