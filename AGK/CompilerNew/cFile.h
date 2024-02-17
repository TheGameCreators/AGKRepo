#ifndef _H_FILE_
#define _H_FILE_

// Common includes
#include "CompilerCommon.h"

#define MAX_FILES 0xffffffff
#define AGK_FILE_READ_BUFFER 256

// Namespace
namespace AGK_Compiler
{
	class cFile
	{
		protected:
			FILE *pFile;
			void *pFilePtr;
			int mode; //0=read, 1=write
			char readbuf[AGK_FILE_READ_BUFFER];

			static char g_szRootDir[ MAX_PATH ];
			static char g_szWriteDir[ MAX_PATH ];
			static uString g_sCurrentDir;

		public:
			cFile();
			~cFile();
        
            static const char* GetReadPath() { return g_szRootDir; }
            static const char* GetWritePath() { return g_szWriteDir; }

			static void Init( const char* projectPath );
			static void SetCurrentFolder( const char* folder );
			static void GetProjectName( uString &output );

            static bool PlatformCreatePath( uString path );
			static void PlatformGetFullPathWrite( uString &inout );
			static void PlatformGetFullPathRead( uString &inout );

			static void DeleteFile( const char *szFilename );
			static void DeleteFolder( const char *szFilename );
			static bool Exists( const char *szFilename );
			static bool ExistsWrite( const char *szFilename );
			static bool ExistsRead( const char *szFilename );
			static bool GetModified( const char *szFilename, int &time );
			static uint32_t GetFileSize( const char *szFilename );

			static bool CopyFile( const char* fromFile, const char* toFile );
			static bool RunFile( const char* filename, const char* cmdline );

			bool OpenToWrite( const char *szFilename, bool append=false );
			bool OpenToRead( const char *szFilename );

			bool IsOpen();
			void Close();
			void Rewind();
			bool IsEOF();
			uint32_t GetSize();
			uint32_t GetPos();
			void Seek( uint32_t pos );

			void WriteByte( unsigned char b );
			void WriteInteger( int i );
			void WriteFloat( float f );
			void WriteString( const char *str );
			void WriteData( const char *str, uint32_t bytes );
			void WriteLine( const char *str );

			unsigned char ReadByte( );
			int ReadInteger( );
			float ReadFloat( );
			int ReadString( uString &str );
			int ReadData( char *str, uint32_t length );
			int ReadLine( uString &str );
			int ReadLineFast( uString &str );
	};
}

#endif