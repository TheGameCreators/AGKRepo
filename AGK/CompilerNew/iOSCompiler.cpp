#include "CompilerCommon.h"

namespace AGK_Compiler
{
    NSFileManager *g_pFileManager = NULL;
}

void* AGK_Compiler::StartTimer( void )
{
	double* start = new double;
	start = CACurrentMediaTime();
	return start;
}

float AGK_Compiler::GetTime( void* start )
{
	double* starttime = (double*) start;
	double time = CACurrentMediaTime() - *startTime;
	delete startTime;
	return time;
}

void AGKDisplayMessage( const char* msg )
{
    
}

#include "cFile.h"

using namespace AGK_Compiler;

void cFile::Init( const char* projectPath )
{
	// g_szRootDir doesn't matter, set to current directory

	strcpy( g_szWriteDir, projectPath );
	char* ptr = g_szWriteDir;
	while( *ptr )
	{
		if ( *ptr == '\\' ) *ptr = '/';
		ptr++;
	}

	int length = (int) strlen( g_szWriteDir );
	if ( g_szWriteDir[ length-1 ] != '/' )
	{
		g_szWriteDir[ length ] = '/';
		g_szWriteDir[ length+1 ] = 0;
	}

	*g_szRootDir = 0;
    
    g_pFileManager = [[NSFileManager alloc] init];
}

bool cFile::PlatformCreatePath( uString sOrigPath )
{
	const char *path = sOrigPath.GetStr();

	if ( !path || !*path ) return false;
	if ( path[0] != '/' ) 
	{
		uString err; err.Format( "Invalid path \"%s\", must be absolute", path );
		Error( err );
		return false;
	}

	char* withoutFile = new char[ strlen(path)+1 ];
	strcpy( withoutFile, path );
	char *szSlash = strrchr( withoutFile, '/' );
    if ( szSlash ) *szSlash = '\0';
	
	// create directory
    NSString* pWriteDir = [ [ NSString alloc ] initWithUTF8String:withoutFile ];
    [ g_pFileManager createDirectoryAtPath:pWriteDir withIntermediateDirectories:YES attributes:nil error:NULL ];
    [ pWriteDir release ];
    
    delete [] withoutFile;
    return true;
}

bool cFile::ExistsWrite( const char *szFilename )
{
	if ( !szFilename || !*szFilename ) return false;
	if ( strncmp(szFilename, "raw:", 4) == 0 ) return false;
	
	if ( strchr( szFilename, ':' ) ) return false;
	if ( strstr(szFilename, "..\\") || strstr(szFilename, "../") ) return false;
	
	size_t length = (uint32_t) strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;
	
    uString sPath( szFilename );
    PlatformGetFullPathWrite(sPath);
    	
	FILE *pFile = fopen( sPath.GetStr(), "rb" );
	if ( !pFile ) return false;
	fclose( pFile );

	return true;
}

bool cFile::ExistsRead( const char *szFilename )
{
	return false;
}

bool cFile::Exists( const char *szFilename )
{
	return ExistsWrite( szFilename );
}

bool cFile::GetModified( const char *szFilename, int &time )
{
	return false;
}

uint32_t cFile::GetFileSize( const char *szFilename )
{
	cFile pFile;
	if ( !pFile.OpenToRead( szFilename ) ) return 0;
	uint32_t size = pFile.GetSize();
	pFile.Close();
	
	return size;
}

void cFile::DeleteFile( const char *szFilename )
{
	if ( !szFilename || !*szFilename ) return;
	
	uint32_t length = (uint32_t) strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' )
	{
		Error( "Invalid path for DeleteFile file, must not end in a forward or backward slash" );
		return;
	}
	
	uString sPath( szFilename );
	if ( cFile::ExistsWrite( szFilename ) ) PlatformGetFullPathWrite(sPath);
	else return;
	
	NSString* pPath = [ [ NSString alloc ] initWithUTF8String:sPath ];
	[ g_pFileManager removeItemAtPath:pPath error:nil ];
	[ pPath release ];
}

int DeleteFolderAbsolute( const char *src )
{
	return 0;
}

void cFile::DeleteFolder( const char *szFilename )
{
	
}

bool cFile::OpenToWrite( const char *szFilename, bool append )
{
	if ( !szFilename || !*szFilename ) return false;
	if ( pFile ) Close();
	mode = 1;
	
	int raw = 0;
	uString sPath( szFilename );
	PlatformGetFullPathWrite(sPath);

	if ( !PlatformCreatePath( sPath ) ) return false;
	
	if ( append ) pFile = fopen( sPath, "ab" );
	else pFile = fopen( sPath, "wb" );
	
	if ( !pFile )
	{
		uString err = "Failed to open file for writing ";
		err += sPath;
		Error( err );
		return false;
	}
    
    return true;
}

bool cFile::OpenToRead( const char *szFilename )
{
	if ( !szFilename || !*szFilename ) return false;
	if ( pFile ) Close();
	mode = 0;

	uString sPath( szFilename );
	if ( !cFile::ExistsWrite( szFilename ) ) 
	{
		uString err = "Failed to open file for reading (doesn't exist) ";
		err.Append( szFilename );
		Error( err );
		return false;
	}
	
	PlatformGetFullPathWrite(sPath);

	pFilePtr = 0;
	pFile = fopen( sPath, "rb" );
	if ( !pFile )
	{
		uString err = "Failed to open file for reading ";
		err += sPath.GetStr();
		Error( err );
		return false;
	}

	return true;
}

void cFile::Close()
{
	if ( pFile ) fclose( pFile );
	pFile = 0;
}

bool cFile::CopyFile( const char* fromFile, const char* toFile )
{
	return false;
}

bool cFile::RunFile( const char* filename, const char* cmdline )
{
	return false;
}

uint32_t cFile::GetSize()
{
	if ( !pFile ) return 0;
	fpos_t pos;
	fgetpos( pFile, &pos );
	fseek( pFile, 0, SEEK_END );
	uint32_t size = ftell( pFile );
	fsetpos( pFile, &pos );
	return size;
}

uint32_t cFile::GetPos()
{
	if ( !pFile ) return 0;
	return ftell( pFile );
}

void cFile::Seek( uint32_t pos )
{
	if ( !pFile ) return;
	fseek( pFile, pos, SEEK_SET );
}

void cFile::Rewind()
{
	if ( !pFile ) return;
	rewind( pFile );
}

bool cFile::IsEOF()
{
	if ( !pFile ) return true;
	return feof( pFile ) != 0;
}

void cFile::WriteByte( unsigned char b )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		Error( "Cannot not write to file opened for reading", 0, 0 );
		return;
	}

	fwrite( &b, 1, 1, pFile );
}

void cFile::WriteInteger( int i )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		Error( "Cannot not write to file opened for reading", 0, 0 );
		return;
	}

	//convert everything to little endian for cross platform compatibility
	//i = PlatformLittleEndian( i );
	fwrite( &i, 4, 1, pFile );
}

void cFile::WriteFloat( float f )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		Error( "Cannot not write to file opened for reading", 0, 0 );
		return;
	}
	fwrite( &f, 4, 1, pFile );
}

void cFile::WriteString( const char *str )
{
	if ( !str ) return;
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		Error( "Cannot not write to file opened for reading", 0, 0 );
		return;
	}
	uint32_t length = (uint32_t) strlen( str );
	fwrite( str, 1, length + 1, pFile );
}

void cFile::WriteData( const char *str, uint32_t bytes )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		Error( "Cannot not write to file opened for reading", 0, 0 );
		return;
	}
	
	fwrite( str, 1, bytes, pFile );
}

void cFile::WriteLine( const char *str )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		Error( "Cannot not write to file opened for reading", 0, 0 );
		return;
	}
	uint32_t length = (uint32_t) strlen( str );
	fwrite( str, 1, length, pFile );

	// strings terminate with CR (13,10) - so it resembles a regular text file when file viewed
	char pCR[2];
	pCR[0]=13;
	pCR[1]=10;
	fwrite( &pCR[0], 1, 1, pFile );
	fwrite( &pCR[1], 1, 1, pFile );
}

unsigned char cFile::ReadByte( )
{
	if ( !pFile ) return 0;
	if ( mode != 0 )
	{
		Error( "Cannot not read from file opened for writing", 0, 0 );
		return 0;
	}
	unsigned char b = 0;
	fread( &b, 1, 1, pFile );
	return b;
}

int cFile::ReadInteger( )
{
	if ( !pFile ) return 0;
	if ( mode != 0 )
	{
		Error( "Cannot not read from file opened for writing", 0, 0 );
		return 0;
	}
	int i = 0;
	fread( &i, 4, 1, pFile );
	// convert back to local endian, everything in the file is little endian.
	return i;// = PlatformLocalEndian( i );
}

float cFile::ReadFloat( )
{
	if ( !pFile ) return 0;
	if ( mode != 0 )
	{
		Error( "Cannot not read from file opened for writing", 0, 0 );
		return 0;
	}
	float f;
	fread( &f, 4, 1, pFile );
	return f;
}

int cFile::ReadString( uString &str )
{
	if ( !pFile ) 
	{
		Error( "Cannot not read from file, file not open", 0, 0 );
		return -1;
	}

	if ( mode != 0 )
	{
		Error( "Cannot not read from file opened for writing", 0, 0 );
		return -1;
	}

	str.SetStr( "" );
	
	// read until a null terminator is found, or eof. (use readline for CR detection)
	bool bNullFound = false;
	do
	{
		long lPos = ftell( pFile );
		uint32_t written = fread( readbuf, 1, 256, pFile ); 
		for ( uint32_t i = 0; i < written; i++ )
		{
			if ( readbuf[ i ] == 0 )
			{
				bNullFound = true;
				fseek( pFile, lPos+i+1, SEEK_SET );
				if ( i > 0 ) str.AppendN ( readbuf, i );
				break;
			}
		}
		
		if ( !bNullFound ) str.AppendN( readbuf, written );
		else break;

	} while( !feof( pFile ) );

	return str.GetLength();
}

int cFile::ReadLine( uString &str )
{
	if ( !pFile ) 
	{
		Error( "Cannot not read from file, file not open", 0, 0 );
		return -1;
	}

	if ( mode != 0 )
	{
		Error( "Cannot not read from file opened for writing", 0, 0 );
		return -1;
	}

	str.SetStr( "" );

	// read until a CR (13,10) line terminator is found, or eof.
	// so it resembles a regular text file when file viewed
	bool bNullFound = false;
	do
	{
		long lPos = ftell( pFile );
		uint32_t written = fread( readbuf, 1, 256, pFile ); 
		for ( uint32_t i = 0; i < written; i++ )
		{
			//if ( readbuf[ i ] == 0 ) // CRs terminate string lines
			if ( readbuf[ i ] == '\r' || readbuf[ i ] == '\n' ) 
			{
				int i2 = i;
				if ( readbuf[ i ] == '\r' ) i++;
				if ( readbuf[ i ] == '\n' ) i++;
				bNullFound = true;
				fseek( pFile, lPos+i, SEEK_SET );
				if ( i > 0 ) str.AppendN ( readbuf, i2 ); // don't copy the newline characters into the string
				break;
			}
		}
		
		if ( !bNullFound ) str.AppendN( readbuf, written );
		else break;

	} while( !feof( pFile ) );

	return str.GetLength();
}

int cFile::ReadLineFast( uString &str )
{
	if ( !pFile ) 
	{
		Error( "Cannot not read from file, file not open", 0, 0 );
		return -1;
	}

	if ( mode != 0 )
	{
		Error( "Cannot not read from file opened for writing", 0, 0 );
		return -1;
	}

	str.SetStr( "" );

	char tempstr[ 1024 ];
	if ( !fgets( tempstr, 1024, pFile ) ) return 0;
	str.SetStr( tempstr );

	while ( !IsEOF() && str.ByteAt( str.GetLength()-1 ) != '\n' )
	{
		if ( !fgets( tempstr, 1024, pFile ) ) break;
		str.Append( tempstr );
	}

	//str.Trim( "\r\n" );
	return str.GetLength();
}

int cFile::ReadData( char *str, uint32_t length )
{
	if ( !pFile ) 
	{
		Error( "cFile::ReadData() cannot not read from file, file not open", 0, 0 );
		return -1;
	}

	if ( mode != 0 )
	{
		Error( "Cannot not read from file opened for writing", 0, 0 );
		return -1;
	}

	return fread( str, 1, length, pFile );
}
