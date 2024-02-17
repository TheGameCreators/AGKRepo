// Sprite Header
#ifndef _H_NETWORK_
#define _H_NETWORK_


#define AGK_NET_SEND_BUFFER_SIZE 1400
#define AGK_NET_PACKET_SIZE 1400

// Common includes
#include "Common.h"
#include "NetworkPacket.h"
#include "cHashedList.h"
#include "uString.h"
#include "Thread.h"
#include "NetworkCommon.h"

// Namespace
namespace AGK
{
        /*
        class cLock;
        //class cNetCon;
	class AGKSocket;
	class cNetworkListener;
	class Broadcaster;
	class BroadcastListener;
	class cFile;
	*/

	class cAutoLock
	{
		protected:
			cLock *m_pLock;

		public:
			cAutoLock( cLock *lock )
			{
				if ( !lock ) return;
				m_pLock = lock;
				m_pLock->Acquire();
			}

			~cAutoLock()
			{
				m_pLock->Release();
			}
	};

	class cAutoSLock
	{
		protected:
			cSpinLock *m_pLock;

		public:
			cAutoSLock( cSpinLock *lock )
			{
				if ( !lock ) return;
				m_pLock = lock;
				m_pLock->Acquire();
			}

			~cAutoSLock()
			{
				m_pLock->Release();
			}
	};

	class cFileSender : public AGKThread
	{
		protected:
			AGKSocket *m_pConnection;
			char m_szIP[ 40 ];
			uint32_t m_iPort;
			uint32_t m_iTimeout;
			//bool m_bClose;
			cLock *m_kNameLock;
			bool m_bCloseAfter;

			uString* m_pFiles;
			uint32_t m_iSize;

			volatile uint32_t m_iCurrentFileSize;
			volatile uint32_t m_iTotalSending;
			volatile uint32_t m_iTotalSent;
			uint32_t m_iAdded;
			volatile uint32_t m_iCurrentFile;
			
			uint32_t Run();

		public:
			cFileSender *m_pNext;

			cFileSender( uint32_t numFiles );
			~cFileSender();

			bool ConnectTo( const char *szIP, uint32_t port, uint32_t timeout=3000 );
			void SetReceiver( AGKSocket *pReceiver, bool closeafter=true );
			bool AddFile( const char *szFilename );
			float GetProgress();
			int GetCurrentFile( uString &s );
			void Reset( uint32_t numFiles );
			bool IsFinished() { return !IsRunning(); }
			void Stop();
	};

	class cFileBroadcaster : public AGKThread
	{
		protected:
			uint32_t Run();

			uint32_t m_iBrodcastPort;
			uint32_t m_iReturnPort;
			Broadcaster m_cBroadcaster;
			Broadcaster m_cBroadcasterIPv6;
			cNetworkListener m_cListener;

			cFileSender *m_pSenders;
			cLock *m_kSenderLock;

			uString* m_pFiles;
			uint32_t m_iNumFiles;
			uint32_t m_iAdded;

			volatile uint32_t m_iCompleted;

		public:
			cFileBroadcaster( uint32_t numFiles, const char* szGameName, uint32_t returnPort=45633, uint32_t brodcastPort=45632 );
			~cFileBroadcaster();

			// fills arrays with the latest data, each array must be at least max in size.
			int GetProgress( uint32_t max, uString *pDevices, uString *pCurrentFile, float *pProgress );
			uint32_t GetNumClients();
			uint32_t GetNumCompleted() { return m_iCompleted; }

			bool AddFile( const char *szFilename );

			void Start();
			void Stop();
			void StopBroadcasting();
			void ResumeBroadcasting();
	};

	class cFileReceiver : public AGKThread
	{
		protected:
			AGKSocket* m_pConnection;
			char m_szIP[ 40 ];
			uint32_t m_iPort;
			uint32_t m_iTimeout;
			//bool m_bClose;
			volatile bool m_bSuccess;
			bool m_bCloseAfter;

			volatile uint32_t m_iTotalReceiving;
			volatile uint32_t m_iTotalReceived;

			char m_buffer[ 1500 ];

			uint32_t Run();

		public:
			cFileReceiver( );
			~cFileReceiver();

			bool ConnectTo( const char *szIP, uint32_t port, uint32_t timeout=3000 );
			void SetSender( AGKSocket *pSender, bool closeafter=true );
			void Reset( );
			bool IsFinished() { return !IsRunning(); }
			bool IsSuccess() { return m_bSuccess; }	
			float GetProgress();
			void Stop();
	};

	class cNetworkVariable
	{
		public:
			uString m_sName;
			bool m_bChanged;
			bool m_bNew;
			uint32_t m_iMode; // 0=normal, 1=reset, 2=unique
			uint32_t m_iType;
			bool m_bReset;
			union 
			{
				float f;
				int i;
			} m_value;

			cNetworkVariable() { m_bChanged = true; m_bNew = true; m_iMode = 0; m_value.i = 0; m_iType = 0; m_bReset = false; }
			~cNetworkVariable() { }
	};

	class cNetworkClient
	{
		protected:
			cHashedList<cNetworkVariable*> m_pHashedVars;
			cNetworkVariable **m_pVars;
			uint32_t m_iNumVars;
			uint32_t m_iArraySize;
			cLock *m_kVarLock;
			
		public:
			uString m_sName;
			uint32_t m_iID;
			volatile bool m_bKick;
			int m_iUserValue[ 5 ];
			float m_fPingStart;
			char m_iPingResponded;
			float m_fPing;
			bool m_bDisconnected;
			bool m_bDelete;

			cNetworkClient();
			~cNetworkClient();

			cLock* GetVarLock() { return m_kVarLock; }
			uint32_t GetNumVars() { return m_iNumVars; }
			cNetworkVariable** GetVars() { return m_pVars; }

			int GetVariableI( const char *name, bool bDontReset=false );
			float GetVariableF( const char *name, bool bDontReset=false );
			uint32_t GetVariableType( const char *name );
			uint32_t GetVariableExists( const char *name );

			//int GetVariableI( uint32_t index );
			//float GetVariableF( uint32_t index );
			uint32_t GetVariableType( uint32_t index );

			void SetVariableI( const char *name, int i, int mode=0 );
			void SetVariableF( const char *name, float f, int mode=0 );

			void SetVariableI( uint32_t index, int i, int mode=0 );
			void SetVariableF( uint32_t index, float f, int mode=0 );
	};

	class cNetwork : public AGKThread
	{
		protected:
			uString m_sNetworkName;
			uString m_sMyName;
			char m_szServerIP[ 40 ];
			uint32_t m_iServerPort;
			uint32_t m_iHostPort;
			uint32_t m_iHostPortv6;

			Broadcaster *m_pBroadcaster;				//server only
			Broadcaster *m_pBroadcasterv6;				//server only
			cNetworkListener *m_pListener;				//server only
			cNetworkListener *m_pListenerv6;			//server only
			AGKSocket ** volatile m_ppClientSock;		//server only

			BroadcastListener *m_pBroadcastListener;	//client only
			BroadcastListener *m_pBroadcastListenerv6;	//client only
			AGKSocket *m_pServerSock;					//client only
			char m_iKeepAliveResponded;					//client only
			
			cNetworkClient** volatile m_ppClients; 
			volatile uint32_t m_iNumClients;
			uint32_t m_iArraySize;
			bool m_bServer;
			cLock *m_kClientLock;
			uint32_t m_iNextClientID;
			volatile uint32_t m_iMyClientID;
			uint32_t m_iClientIter;
			cHashedList<uint32_t> m_cClientRef;
			uint32_t m_iLatency;
			float m_fLastKeepAlive;

			cLock *m_kCleanUpLock;

			cLock *m_kMessageLock;
			cNetworkMessage* volatile m_pSendMessages;
			cNetworkMessage* volatile m_pSendTail;
			cNetworkMessage* volatile m_pRecvMessages;
			cNetworkMessage* volatile m_pRecvTail;

			uint32_t Run();

			void JoinCommon();

			void NewClient( AGKSocket *pNew );
			void CheckMessagesServer();
			void SendChangesServer();
			void SendDisconnects();
			void UpdateServer();

			void ConnectClient();
			void CheckMessagesClient();
			void SendChangesClient();
			void UpdateClient();

		public:
			cNetwork();
			~cNetwork();
			void Reset();
			void Stop();

			void HostNetwork( const char *szNetworkName, const char *szMyName, int port, int portv6 );
			void NoMoreConnections();
			void AllowConnections();

			void JoinNetwork( const char *szNetworkName, const char *szMyName );
			void JoinNetwork( const char *szIP, uint32_t port, const char *szMyName );

			bool IsActive() { return IsRunning(); }

			void CloseNetwork();
			void SetLatency( uint32_t latency );

			uint32_t GetMyID() { return m_iMyClientID; }
			uint32_t GetNumClients();
			uint32_t GetNumActiveClients();
			uint32_t GetFirstClient();
			uint32_t GetNextClient();

			void KickClient( uint32_t clientID );

			uint32_t GetClientDisconnected( uint32_t client );
			void DeleteDisconnectedClient( uint32_t client );
			int GetClientName( uint32_t client, uString &sName );
			int GetClientIP( uint32_t client, uString &sIP );
			float GetClientPing( uint32_t client );
			uint32_t GetServerID( ) { return 1; } // for now server is always client ID 1
			int GetServerIP( uString &sIP );
			
			void SetLocalVariableI( const char *name, int i, int mode=0 );
			void SetLocalVariableF( const char *name, float f, int mode=0 );
			
			int GetClientVariableI( uint32_t client, const char *name );
			float GetClientVariableF( uint32_t client, const char *name );
			
			void SendMessage( uint32_t toClient, cNetworkMessage* pMsg );
			cNetworkMessage* GetMessage( );
			
			void SetClientUserData( uint32_t client, uint32_t index, int value );
			int GetClientUserData( uint32_t client, uint32_t index );
			
			void Update();
	};
}

#endif
