#ifndef _H_FILE_
#define _H_FILE_

// Common includes
#include "Common.h"
#include "uString.h"

#define MAX_FILES 0xffffffff
#define AGK_FILE_READ_BUFFER 256

// Namespace
namespace AGK
{
	class cFolder
	{
		protected:
			uint32_t m_iNumFolders;
			uint32_t m_iNumFiles;
			uString* m_pFolders;
			uString* m_pFiles;

		public:
			cFolder() { m_iNumFiles = 0; m_iNumFolders = 0; m_pFiles = 0; m_pFolders = 0; }
			~cFolder() { if( m_pFiles ) delete [] m_pFiles; if( m_pFolders ) delete [] m_pFolders; }

			int OpenFolder( const char* szPath );
			int GetNumFolders() { return m_iNumFolders; }
			int GetNumFiles() { return m_iNumFiles; }
			const char* GetFile( uint32_t index ) { if( index >= m_iNumFiles ) return 0; else return m_pFiles[index].GetStr(); }
			const char* GetFolder( uint32_t index ) { if( index >= m_iNumFolders ) return 0; else return m_pFolders[index].GetStr(); }
	};

	class cFile
	{
		protected:
			FILE *pFile;
			void *pFilePtr;
			uString sFullPath;
			int mode; //0=read, 1=write
			char readbuf[AGK_FILE_READ_BUFFER];

		public:
			cFile();
			~cFile();

			static void DeleteFile( const char *szFilename );
			static bool Exists( const char *szFilename );
			static bool ExistsWrite( const char *szFilename );
			static bool ExistsRead( const char *szFilename, int *mode=0 );
			static bool ExistsRaw( const char *szFilename );
			static bool GetModified( const char *szFilename, int &time );
			static bool GetModified64( const char *szFilename, int64_t &time );
			static void SetModified( const char *szFilename, int time );
			static uint32_t GetFileSize( const char *szFilename );

			bool OpenToWrite( const char *szFilename, bool append=false );
			bool OpenToRead( const char *szFilename );

			bool IsOpen();
			void Close();
			void Rewind();
			bool IsEOF();
			uint32_t GetSize();
			uint32_t GetPos();
			void Seek( uint32_t pos );
			void Flush();

			void WriteByte( unsigned char b );
			void WriteInteger( int i );
			void WriteFloat( float f );
			void WriteString( const char *str );
			void WriteString2( const char *str );
			void WriteData( const char *str, uint32_t bytes );
			void WriteLine( const char *str );

			unsigned char ReadByte( );
			int ReadInteger( );
			float ReadFloat( );
			int ReadString( uString &str );
			int ReadString2( uString &str );
			int ReadData( char *str, uint32_t length );
			int ReadLine( uString &str );
			//int ReadLineFast( uString &str );
	};
}

#endif