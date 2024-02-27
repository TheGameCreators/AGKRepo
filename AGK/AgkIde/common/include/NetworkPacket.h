// Sprite Header
#ifndef _H_NETWORK_PACKET
#define _H_NETWORK_PACKET

#define AGK_NET_SEND_BUFFER_SIZE 1400
#define AGK_NET_PACKET_SIZE 1400

// Common includes
#include "Common.h"
#include "uString.h"

namespace AGK
{
	class AGKPacket
	{
		protected:
			char m_Buffer[ AGK_NET_PACKET_SIZE ];
			UINT m_iPtr;
			UINT m_iPacketSize;

		public:

			AGKPacket();
			~AGKPacket(){};
			void Copy( const AGKPacket *fromPacket );

			void AddData( const char* s, UINT length );
			void AddString( const char *s );
			void AddByte( unsigned char c );
			void AddUInt( UINT u );
			void AddInt( int i );
			void AddFloat( float f );

			UINT GetPos() const;
			void SetPos( UINT pos );
			const char* GetBuffer() const { return m_Buffer; }
			char* GetRaw(){ return m_Buffer; }
			UINT GetSize() const { return m_iPacketSize; }
			void SetSize( UINT size ) { m_iPacketSize = size; }

			UINT GetData( char* data, UINT length );
			int GetString( uString &s );		
			unsigned char GetByte();						
			int GetInt();						
			UINT GetUInt();						
			float GetFloat();					
	};

	class cNetworkMessage : public AGKPacket
	{
		protected:
			friend class cNetwork;

			UINT m_iToClientID;
			UINT m_iFromClientID;
			cNetworkMessage *m_pNext;

		public:
			uString sFromIP;
			int iFromPort;
			
			cNetworkMessage() { m_iToClientID = 0; m_iFromClientID = 0; m_pNext = 0; iFromPort = 0; }
			~cNetworkMessage() { };

			void CopyMessage( cNetworkMessage *pOther );

			UINT GetSenderID() { return m_iFromClientID; }
	};
}

#endif

