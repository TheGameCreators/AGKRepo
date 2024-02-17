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
			uint32_t m_iPtr;
			uint32_t m_iPacketSize;

		public:

			AGKPacket();
			~AGKPacket(){};
			void Copy( const AGKPacket *fromPacket );

			void AddData( const char* s, uint32_t length );
			void AddString( const char *s );
			void AddByte( uint8_t b );
			void AddUInt( uint32_t u );
			void AddInt( int i );
			void AddFloat( float f );

			uint32_t GetPos() const;
			void SetPos( uint32_t pos );
			const char* GetBuffer() const { return m_Buffer; }
			char* GetRaw(){ return m_Buffer; }
			uint32_t GetSize() const { return m_iPacketSize; }
			void SetSize( uint32_t size ) { m_iPacketSize = size; }

			uint32_t GetData( char* data, uint32_t length );
			int GetString( uString &s );		
			uint8_t GetByte();						
			int GetInt();						
			uint32_t GetUInt();						
			float GetFloat();					
	};

	class cNetworkMessage : public AGKPacket
	{
		protected:
			friend class cNetwork;

			uint32_t m_iToClientID;
			uint32_t m_iFromClientID;
			cNetworkMessage *m_pNext;

		public:
			uString sFromIP;
			int iFromPort;
			
			cNetworkMessage() { m_iToClientID = 0; m_iFromClientID = 0; m_pNext = 0; iFromPort = 0; }
			~cNetworkMessage() { };

			void CopyMessage( cNetworkMessage *pOther );

			uint32_t GetSenderID() { return m_iFromClientID; }
	};
}

#endif
