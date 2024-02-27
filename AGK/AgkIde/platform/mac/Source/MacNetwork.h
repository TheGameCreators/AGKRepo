#ifndef _H_NETWORK_MAC
#define _H_NETWORK_MAC

#include "Common.h"
#include "NetworkPacket.h"

namespace AGK
{
    class cHTTPConnection;
    class cFile;
}

@interface AGKHTTPListener : NSObject
{
@public
    cHTTPConnection *m_pHTTP;
    NSMutableData *m_response;
    int m_length;
    int m_received;
    bool m_bToFile;
    uString m_sFilename;
    AGK::cFile *m_file;
}
@end

namespace AGK
{
	class cLock 
	{
		protected:
		NSRecursiveLock *MacLock;
			
		public:
			cLock()
			{
				MacLock = [[NSRecursiveLock alloc] init];
			}

			~cLock()
			{
				[ MacLock release ];
			}

			bool Acquire()
			{
				[ MacLock lock ];
				return true;
			}

			void Release()
			{
				[ MacLock unlock ];
			}
	};

	class cSpinLock 
	{
		protected:
		NSLock *MacLock;
			
		public:
			cSpinLock()
			{
				MacLock = [[NSLock alloc] init];
			}

			~cSpinLock()
			{
				[ MacLock release ];
			}

			bool Acquire()
			{
				[ MacLock lock ];
				return true;
			}

			void Release()
			{
				[ MacLock unlock ];
			}
	};

	class cCondition 
	{
        protected:
            NSCondition *m_condition;
            bool m_bLocked;
            
        public:
            cCondition()
            {
                m_condition = [[NSCondition alloc] init];
                m_bLocked = false;
            }
            
            ~cCondition()
            {
                [m_condition release];
            }
            
            void Lock()
            {
                [m_condition lock];
                m_bLocked = true;
            }
            
            void Unlock()
            {
                m_bLocked = false;
                [m_condition unlock];
            }
            
            void Wait()
            {
                [m_condition wait];
            }
            
            void Signal()
            {
                [m_condition signal];
            }
            
            void Broadcast()
            {
                [m_condition broadcast];
            }
	};
	
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
		int m_client;
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
		AGKSocket( int s );
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
			int m_socket;
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

	// wil listen for new connections until stopped
	class cNetworkListener : public AGKThread
	{
		protected:
			int m_socket;
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
			int m_socket;
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
			AGKPacket m_packet;
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
            NSURLConnection *m_connection;
            AGKHTTPListener *m_listener;
            NSString *m_sHost;
            uString m_sLastURL;
            bool m_bFinished;
            uString m_sRndFilename;
			int m_iTimeout;
			int m_iVerifyMode;
            
            int m_iSecure;
            volatile bool m_bConnected;
            
            char* volatile m_szResponse;
            float volatile m_fProgress;
        public:
			int volatile m_iStatusCode;
        
        protected:
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
        
            const char* GetBodyFileName() { return m_sRndFilename.GetStr(); }
        
            char m_szContentType[150];

            void FinishedInternal( int value );
            void ProgressInternal( float progress ) { m_fProgress = progress; }
            
            void Stop();
            
            bool SetHost( const char *szHost, int iSecure, const char *szUser=0, const char *szPass=0 );
            void Close();
			void SetTimeout( int milliseconds );
			void SetVerifyCertificate( int mode );
			int GetVerifyMode() { return m_iVerifyMode; }

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
