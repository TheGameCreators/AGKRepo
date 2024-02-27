#ifndef _H_NETWORK_WIN
#define _H_NETWORK_WIN

#include "Common.h"
#include "Thread.h"
#include "NetworkPacket.h"

namespace AGK
{
	class cLock 
	{
		protected:
			CRITICAL_SECTION m_cs;

		public:
			cLock()	{ InitializeCriticalSectionAndSpinCount( &m_cs, 4000 );	}
			~cLock() { DeleteCriticalSection( &m_cs ); }

			bool Acquire()
			{
				EnterCriticalSection( &m_cs );
				return true;
			}

			void Release() { LeaveCriticalSection( &m_cs );	}
	};

	class cSpinLock 
	{
		protected:
			CRITICAL_SECTION m_cs;

		public:
			cSpinLock()	{ InitializeCriticalSectionAndSpinCount( &m_cs, 4000 );	}

			~cSpinLock() { DeleteCriticalSection( &m_cs ); }

			bool Acquire()
			{
				EnterCriticalSection( &m_cs );
				return true;
			}

			void Release() { LeaveCriticalSection( &m_cs );	}
	};

	class cCondition 
	{
		protected:
			HANDLE condition[2];
			HANDLE hold;
			HANDLE release;
			CRITICAL_SECTION m_cs;
			volatile int waiting;
			
		public:
			cCondition()
			{
				condition[0] = CreateEvent( NULL, FALSE, FALSE, NULL );
				condition[1] = CreateEvent( NULL, TRUE, FALSE, NULL );
				hold = CreateEvent( NULL, TRUE, TRUE, NULL );
				release = CreateEvent( NULL, TRUE, TRUE, NULL );
				InitializeCriticalSection( &m_cs );
				waiting = 0;
			}

			~cCondition()
			{
				DeleteCriticalSection( &m_cs );
				CloseHandle( release );
				CloseHandle( hold );
				CloseHandle( condition[1] );
				CloseHandle( condition[0] );
			}

			void Lock() { EnterCriticalSection( &m_cs ); }
			void Unlock() { LeaveCriticalSection( &m_cs ); }

			// this would be so much simpler with condition variables
			void Wait()
			{
				waiting++;
				LeaveCriticalSection( &m_cs );
				
				// wait for single or broadcast conditions
				WaitForMultipleObjects( 2, condition, FALSE, INFINITE );

				EnterCriticalSection( &m_cs );
				waiting--;
				if ( waiting == 0 ) SetEvent( release ); // notify the broadcast function that all threads are awake, doesn't matter if it is waiting or not
				LeaveCriticalSection( &m_cs );

				// if this thread was woken by a broadcast then they will all be held here until they all wake up.
				WaitForSingleObject( hold, INFINITE );

				EnterCriticalSection( &m_cs );
			}

			void Signal()
			{
				// this will release a single thread
				SetEvent( condition[0] );
			}

			void Broadcast()
			{
				if ( waiting == 0 ) return;

				// this will release all threads and wait for them all to be awake before returning
				ResetEvent( release );
				ResetEvent( hold );
				SetEvent( condition[1] ); // this will release all threads
				LeaveCriticalSection( &m_cs );

				WaitForSingleObject( release, INFINITE ); // wait for them all to wake up
				
				EnterCriticalSection( &m_cs );
				ResetEvent( condition[1] );
				SetEvent( hold ); // release the threads to continue
			}
	};

	/*
	// not supported in Windows XP
	class cCondition 
	{
		protected:
			CONDITION_VARIABLE condition;
			CRITICAL_SECTION m_cs;
			
		public:
			cCondition()
			{
				InitializeConditionVariable( &condition );
				InitializeCriticalSection( &m_cs );
			}

			~cCondition() {	DeleteCriticalSection( &m_cs ); }

			void Lock()	{ EnterCriticalSection( &m_cs ); }
			void Unlock() { LeaveCriticalSection( &m_cs ); }

			void Wait() { SleepConditionVariableCS( &condition, &m_cs, INFINITE ); }
			void Signal() { WakeConditionVariable( &condition ); }
			void Broadcast() { WakeAllConditionVariable( &condition ); }
	};
	*/

	class AGKSocket;

	class AGKSocketTimeout : public AGKThread
	{
		protected:
			AGKSocket *m_pSocket;
			UINT m_iTimeout;

			UINT Run();

		public:
			AGKSocketTimeout();
			~AGKSocketTimeout();

			void SetData( AGKSocket *pSocket, UINT iTimeout );
	};

	class AGKSocket : public AGKThread
	{
		private:
			friend class AGKSocketTimeout;

		public:
			AGKSocket *m_pNext;

		protected:
			SOCKET m_client;
			char m_szIP[ 65 ];
			UINT m_port;
			volatile bool m_bConnected;
			volatile bool m_bConnecting;
			volatile bool m_bDisconnected;
			UINT m_iTimeout;
			bool m_bASync;

			volatile float m_fProgress;
			volatile bool m_bResult;
			
			char m_sendBuffer[ 1400 ];
			UINT m_iSendWritePtr;

			AGKSocketTimeout m_cTimeout;
						
			UINT Run();
			void Reset();
			int IsIPV6() { return strchr(m_szIP,':') ? 1 : 0; }

		public:

			AGKSocket();
			AGKSocket( SOCKET s );
			~AGKSocket();

			const char *GetRemoteIP() { return m_szIP; }

			bool Flush();
			void Close( bool bGraceful=true );
			void ForceClose();
			bool GetDisconnected() { return m_bDisconnected; }

			bool Connect( const char* IP, UINT port, UINT timeout=3000 );
			bool ConnectASync( const char* IP, UINT port, UINT timeout=3000 );
			bool IsConnected() { return m_bConnected; }
			bool IsConnecting() { return m_bConnecting; }
			float GetProgress() { return m_fProgress; }

			void SetTimeout( int iLength );
			
			bool SendFile( const char* szFilename );
			bool SendData( const char* s, int length );
			bool SendString( const char *s );
			bool SendChar( char c );
			bool SendUInt( UINT u );
			bool SendInt( int i );
			bool SendFloat( float f );

			// if this is zero the recv commands will block
			int GetBytes();
			
			int		RecvData( char* data, int length ); // blocks
			int		RecvString( uString &s );			// blocks
			char	RecvChar();							// blocks
			int		RecvInt();							// blocks
			UINT	RecvUInt();							// blocks
			float	RecvFloat();						// blocks			
	};

	class UDPManager
	{
		protected:
			SOCKET m_socket;
			UINT m_port;
			int m_iIPv6;
			int m_iValid;

		public:
			UDPManager( const char* szIP, UINT listenPort );
			~UDPManager();
			int IsValid() { return m_iValid; }

			bool SendPacket( const char *IP, UINT port, const AGKPacket *packet ); // does not block
			bool RecvPacket( char *fromIP, int *fromPort, AGKPacket *packet );  // does not block
			bool PacketReady();  // does not block
	};

	// will listen for new connections until stopped
	class cNetworkListener : public AGKThread
	{
		protected:
			SOCKET m_socket;
			UINT m_port;
			cLock m_lock;

			AGKSocket* volatile m_pConnections;

			UINT Run();

		public:
			cNetworkListener();
			~cNetworkListener();

			AGKSocket* GetNewConnection();
			int AcceptConnections( const char *szIP, UINT port );
			int AcceptConnections( UINT port );
			void Stop();
	};

	// will wait forever for broadcast packets until Stop() is called
	class BroadcastListener
	{
		protected:
			SOCKET m_socket;
			sockaddr_storage addr;
			//char m_data[1500];
			//char m_from[45];
			//void (*callback)(int, const char*, const char*);

			//UINT Run();

		public:
			BroadcastListener();
			~BroadcastListener();

			void Close();

			bool SetListenPort( const char *szIP, UINT port );
			bool SetListenPort( UINT port );
			bool ReceivedBroadcast();
			bool GetPacket( AGKPacket &packet, UINT &fromPort, char *fromIP );
	};

	// will broadcast a packet a regular intervals until stopped.
	class Broadcaster : public AGKThread
	{
		protected:
			//char m_data[1400];
			AGKPacket m_packet;
			//UINT m_size;
			UINT m_interval;
			int m_max;
			UINT m_port;
			int m_ipv6; // IPv6 or IPv4

			UINT Run();

		public:

			Broadcaster();
			~Broadcaster() {};

			// sets the data that will be broadcast every interval milliseconds for a maximum of max broadcasts (0=forever)
			// start with Start() and stop with Stop()
			//void SetData( UINT port, const char* buf, UINT length, UINT interval, int max=0 );
			void SetData( int ipv6, UINT port, const AGKPacket *packet, UINT interval, int max=0 );
	};

	class cHTTPHeader
	{
		public:
			uString sName;
			uString sValue;

			cHTTPHeader() {}
			~cHTTPHeader() {}
	};

	class cHTTPConnection : public AGKThread
	{
		protected:
			static HINTERNET m_hInet;

			char *m_sURL;
			HINTERNET m_hInetConnect;
			int m_iSecure;
			volatile bool m_bConnected;
			int m_iTimeout;
			int m_iVerifyMode;
			
			char* volatile m_szResponse;
			float volatile m_fProgress;
			int volatile m_iStatusCode;
			char m_szContentType[150];

			bool m_bSaveToFile;
			uString m_szServerFile;
			uString m_szLocalFile;
			uString m_szUploadFile;
			uString m_szPostData;
			uString m_sUsername;
			uString m_sPassword;

			cHashedList<cHTTPHeader> m_cHeaders;

			char* SendRequestInternal();
			char* SendFileInternal();
			UINT Run();

		public:
			cHTTPConnection();
			~cHTTPConnection();

			void Stop();

			bool SetHost( const char *szHost, int iSecure, const char *szUser=0, const char *szPass=0 );
			void Close();
			void SetTimeout( int milliseconds );
			void SetVerifyCertificate( int mode );

			void AddHeader( const char* headerName, const char* headerValue );
			void RemoveHeader( const char* headerName );

			float GetProgress() { return m_fProgress; }
			int GetStatusCode() { return m_iStatusCode; }

			// caller must delete returned pointer
			char* SendRequest( const char *szServerFile, const char *szPostData=0 ); //blocks
			bool SendRequestASync( const char *szServerFile, const char *szPostData=0 ); //does not block
			bool SendFile( const char *szServerFile, const char *szPostData, const char *szLocalFile ); //does not block

			int GetResponseReady();
			const char* GetResponse(); // caller must NOT delete returned pointer
			const char* GetContentType();

			bool DownloadFile( const char *szServerFile, const char *szLocalFile, const char *szPostData=0 ); //does not block
			bool DownloadComplete();
	};
}

#endif