#include "CompilerCommon.h"
#include "cFile.h"
#include <dirent.h>

#ifdef AGK_STEAM_WEEKEND
	#include "steam/steam_api.h"
#endif

using namespace AGK_Compiler;

#ifndef AGK_STATIC_LIB
namespace AGK_Compiler
{ 
	int RunCompiler( const char* lpCmdLine ); 
}

int main( int argc, char** argv )
{
#ifdef AGK_STEAM_WEEKEND
	if ( !SteamAPI_Init() )
	{
		SteamAPI_Shutdown();
		printf( "error: The free weekend version must be launched from within Steam\n" );
		return 1;
	}

	if ( !SteamApps()->BIsSubscribedApp( 325180 ) )
	{
		SteamAPI_Shutdown();
		printf( "error: Steam free weekend has finished\n" );
		return 1;
	}
#endif

    uString sCmdLine;
    for( int i = 1; i < argc; i++ )
    {
        if ( i != 1 ) sCmdLine.AppendAscii( ' ' );
        sCmdLine.Append( argv[i] );
    }
    int result = RunCompiler( sCmdLine.GetStr() );
    
#ifdef AGK_STEAM_WEEKEND
	SteamAPI_Shutdown();
#endif

	return result;
}
#endif

void* AGK_Compiler::StartTimer( void )
{
	timespec* start = new timespec;
	clock_gettime(CLOCK_MONOTONIC, start);
	return start;
}

float AGK_Compiler::GetTime( void* start )
{
    timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	
	timespec* startTime = (timespec*) start;
	double time = now.tv_sec - startTime->tv_sec;
	time += (now.tv_nsec - startTime->tv_nsec) * 1e-9;
	
	delete startTime;
	return (float)time;
}

void AGKDisplayMessage( const char* msg )
{
    // not possible on linux?
    
    fprintf( stderr, "%s", msg );
}

void cFile::Init( const char* projectPath )
{
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

	for ( int i = 0; i < MAX_PATH; i++ ) g_szRootDir[ i ] = 0;

	readlink( "/proc/self/exe", g_szRootDir, MAX_PATH );
	char* name = strrchr( g_szRootDir, '/' );
	if ( name ) *(name+1) = 0;
}

bool cFile::PlatformCreatePath( uString path )
{
    path.Replace( '\\', '/' );
	if ( path.FindStr( "../" ) >= 0 )
	{
		uString sub;
		path.SubString( sub, path.RevFind( '/' )+1 );
		path.SetStr( sub );
	}
    
    chdir( g_szWriteDir );
	
	char checkPath[ 1024 ];
	if ( path.ByteAt(0) == '/' )
	{
		strcpy( checkPath, path.GetStr()+1 );
	}
	else
	{
		strcpy( checkPath, g_sCurrentDir );
		strcat( checkPath, path.GetStr() );
	}
    
	const char *szRemaining = checkPath;
	const char *szSlash;
	char szFolder[ MAX_PATH ];
	while ( szSlash = strpbrk( szRemaining, "/" ) )
	{
		uint32_t length = (uint32_t)(szSlash-szRemaining);
		if ( length == 0 )
		{
			return false;
		}
		
		strncpy( szFolder, szRemaining, length );
		szFolder[ length ] = '\0';
        
		if ( chdir( szFolder ) < 0 )
		{
			mkdir( szFolder, 0755 );
			chdir( szFolder );
		}
        
		szRemaining = szSlash+1;
	}
	chdir( g_szRootDir );

	return true;
}

bool cFile::ExistsWrite( const char *szFilename )
{
	if ( !szFilename ) return false;

	uint32_t length = (uint32_t) strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;

	uString sPath( szFilename, MAX_PATH );
	PlatformGetFullPathWrite( sPath );
	
	FILE *pFile = fopen( sPath.GetStr(), "rb" );
	if ( !pFile ) return false;

	fclose( pFile );
	return true;
}

bool cFile::ExistsRead( const char *szFilename )
{
	if ( !szFilename ) return false;

	uint32_t length = (uint32_t) strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;

	uString sPath( szFilename, MAX_PATH );
	PlatformGetFullPathRead( sPath );
	
	FILE *pFile = fopen( sPath.GetStr(), "rb" );
	if ( !pFile ) return false;

	fclose( pFile );
	return true;
}

bool cFile::Exists( const char *szFilename )
{
	if ( !szFilename ) return false;
	if ( strncmp(szFilename,"raw:",4) == 0 ) 
	{
		FILE *pFile = fopen( szFilename+4, "rb" );
		if ( !pFile ) return false;

		fclose( pFile );
		return true;
	}

	uint32_t length = (uint32_t) strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;

	uString sPath( szFilename, MAX_PATH );
	PlatformGetFullPathWrite( sPath );

	FILE *pFile = fopen( sPath.GetStr(), "rb" );
	if ( !pFile )
	{
		sPath.SetStr( szFilename );
		PlatformGetFullPathRead( sPath );

		pFile = fopen( sPath.GetStr(), "rb" );
	}

	if ( !pFile ) return false;
	fclose( pFile );

	return true;
}

bool cFile::GetModified( const char *szFilename, int &time )
{
	if ( !szFilename ) return false;
	
	uint32_t length = (uint32_t) strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;
	
	uString sPath( szFilename );
    PlatformGetFullPathWrite(sPath);
	
	struct stat fileInfo;
	int result = stat( sPath.GetStr(), &fileInfo );
	if ( result != 0 )
	{
		sPath.SetStr(szFilename);
        PlatformGetFullPathRead(sPath);
		
		result = stat( sPath, &fileInfo );
		if ( result != 0 )
		{
			return false;
		}
	}
	
	time = (int) fileInfo.st_mtime;
	return true;
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
	if ( !szFilename ) return;

	/*
	if ( strstr(szFilename, "..\\") || strstr(szFilename, "../") )
	{
		Error( "Invalid path for DeleteFile, must not traverse backwards up the directory tree using ..\\", 0, 0 );
		return;
	}
	*/

	uString sPath( szFilename );
	PlatformGetFullPathWrite( sPath );
	
	remove( sPath.GetStr() );
}

int DeleteFolderAbsolute( const char *src )
{
	char currDir[ 1024 ];
	getcwd( currDir, 1024 );
	if ( chdir( src ) != 0 ) return 0;
	
	DIR *d = opendir( src );
	dirent *dent = 0;
	
	while( (dent = readdir( d )) )
	{
		if ( strcmp ( dent->d_name, "." ) != 0 && strcmp ( dent->d_name, ".." ) != 0 )
		{
			if ( dent->d_type == DT_DIR )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "/" );
				strcat( newSrc, dent->d_name );

				if ( !DeleteFolderAbsolute( newSrc ) ) return 0;
				if ( rmdir( newSrc ) != 0 )
				{
					char err[1024]; sprintf( err, "Failed to remove directory: %s", newSrc );
					Error( err, 0 ,0 );
					return 0;
				}
			}
			else
			{
				// file
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "/" );
				strcat( newSrc, dent->d_name );

				if ( unlink( newSrc ) != 0 ) 
				{
					char err[1024]; sprintf( err, "Failed to delete file: %s", newSrc );
					Error( err, 0, 0 );
					return 0;
				}
			}
		}
	}

	closedir( d );
	
	chdir( currDir );
	
	return 1;
}

void cFile::DeleteFolder( const char *szFilename )
{
	if ( !szFilename ) return;

	/*
	if ( strstr(szFilename, "..\\") || strstr(szFilename, "../") )
	{
		Error( "Invalid path for DeleteFolder, must not traverse backwards up the directory tree using ..\\", 0, 0 );
		return;
	}
	*/

	uString sPath( szFilename );
	PlatformGetFullPathWrite( sPath );
	
	DeleteFolderAbsolute( sPath.GetStr() );
}


bool cFile::OpenToWrite( const char *szFilename, bool append )
{
	if ( !szFilename ) return false;
	if ( pFile ) Close();
	mode = 1;
	
	uint32_t length = (uint32_t) strlen( szFilename );
	if ( szFilename[ length-1 ] == '/' || szFilename[ length-1 ] == '\\' )
	{
		Error( "Invalid path for OpenToWrite file, must not end in a forward or backward slash",0,0 );
		return false;
	}
	
	// check if directory exists
    PlatformCreatePath(szFilename);
    
	// write to my documents folder only
	//char path[ MAX_PATH ];
	//strcpy( path, szWriteDir );
	//strcat( path, agk::m_sCurrentDir );
	//strcat( path, szFilename );
    uString sPath( szFilename );
    PlatformGetFullPathWrite(sPath);
	
	if ( append ) pFile = fopen( sPath, "ab" );
	else pFile = fopen( sPath, "wb" );
	
	if ( !pFile )
	{
		uString err = "Failed to open file for writing ";
		err += sPath;
		Error( err,0,0 );
		return false;
	}
	
	return true;
}

bool cFile::OpenToRead( const char *szFilename )
{
	if ( !szFilename ) return false;
	if ( pFile ) Close();
	mode = 0;

	uint32_t length = (uint32_t) strlen( szFilename );
	if ( szFilename[ length-1 ] == '/' || szFilename[ length-1 ] == '\\' )
	{
		Error( "Invalid path for OpenToRead file, must not end in a forward or backward slash", 0, 0 );
		return false;
	}

	if ( strncmp(szFilename,"raw:",4) == 0 )
	{
		pFile = fopen( szFilename+4, "rb" );
		if ( !pFile )
		{
			uString err; err.Format( "Failed to open file \"%s\" for reading", szFilename+4 );
			Error( err, 0, 0 );
			return false;
		}
		return true;
	}

	// attempt read from my documents folder first, then exe directory, otherwise fail
	uString sPath ( g_szWriteDir );
	if ( szFilename[0] == '/' || szFilename[0] == '\\' )
	{
		sPath.Append( szFilename+1 );
	}
	else
	{
		sPath.Append( g_sCurrentDir );
		sPath.Append( szFilename );
	}
	
	pFile = fopen( sPath, "rb" );
	if ( !pFile )
	{
		uString sPath ( g_szRootDir );
		if ( szFilename[0] == '/' || szFilename[0] == '\\' )
		{
			sPath.Append( szFilename+1 );
		}
		else
		{
			sPath.Append( g_sCurrentDir );
			sPath.Append( szFilename );
		}
		
		pFile = fopen( sPath, "rb" );
	}

	if ( !pFile )
	{
		uString err = "Failed to open file for reading ";
		err += szFilename;
		Error( err, 0, 0 );
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
	uint32_t length = (uint32_t) strlen( fromFile );
	if ( fromFile[ length-1 ] == '/' || fromFile[ length-1 ] == '\\' )
	{
		Error( "Invalid path for CopyFile, must not end in a forward or backward slash", 0, 0 );
		return false;
	}

	uint32_t lengthTo = (uint32_t) strlen( toFile );
	if ( toFile[ lengthTo-1 ] == '/' || toFile[ lengthTo-1 ] == '\\' )
	{
		Error( "Invalid path for CopyFile, must not end in a forward or backward slash", 0, 0 );
		return false;
	}

	PlatformCreatePath( toFile );

	uString sPathFrom;
	uString sPathTo ( g_szWriteDir );
	if ( toFile[0] == '/' || toFile[0] == '\\' )
	{
		sPathTo.Append( toFile+1 );
	}
	else
	{
		sPathTo.Append( g_sCurrentDir );
		sPathTo.Append( toFile );
	}

	if ( ExistsWrite( fromFile ) )
	{
		sPathFrom.SetStr( g_szWriteDir );
		if ( fromFile[0] == '/' || fromFile[0] == '\\' )
		{
			sPathFrom.Append( fromFile+1 );
		}
		else
		{
			sPathFrom.Append( g_sCurrentDir );
			sPathFrom.Append( fromFile );
		}

		FILE* source = fopen(sPathFrom.GetStr(), "rb");
		FILE* dest = fopen(sPathTo.GetStr(), "wb");

		char buf[8192];
		size_t size;

		while (size = fread(buf, 1, 8192, source)) {
			fwrite(buf, 1, size, dest);
		}

		fclose(source);
		fclose(dest);

		chmod( sPathTo.GetStr(), 0755 );
	}
	else if ( ExistsRead( fromFile ) )
	{
		sPathFrom.SetStr( g_szRootDir );
		if ( fromFile[0] == '/' || fromFile[0] == '\\' )
		{
			sPathFrom.Append( fromFile+1 );
		}
		else
		{
			sPathFrom.Append( g_sCurrentDir );
			sPathFrom.Append( fromFile );
		}
        
        FILE* source = fopen(sPathFrom.GetStr(), "rb");
		FILE* dest = fopen(sPathTo.GetStr(), "wb");

		char buf[8192];
		size_t size;

		while (size = fread(buf, 1, 8192, source)) {
			fwrite(buf, 1, size, dest);
		}

		fclose(source);
		fclose(dest);

		chmod( sPathTo.GetStr(), 0755 );
	}
	else
	{
		uString err = "Could not find file to copy: ";
		err += g_sCurrentDir;
		err += fromFile;
		Error( err, 0, 0 );
		return false;
	}

	return true;
}

bool cFile::RunFile( const char* filename, const char* cmdline )
{
	uString sPath;
	if ( ExistsWrite( filename ) )
	{
		sPath.SetStr( g_szWriteDir );
		if ( filename[0] == '/' || filename[0] == '\\' )
		{
			sPath.Append( filename+1 );
		}
		else
		{
			sPath.Append( g_sCurrentDir );
			sPath.Append( filename );
		}
	}
	else if ( ExistsRead( filename ) )
	{
		sPath.SetStr( g_szRootDir );
		if ( filename[0] == '/' || filename[0] == '\\' )
		{
			sPath.Append( filename+1 );
		}
		else
		{
			sPath.Append( g_sCurrentDir );
			sPath.Append( filename );
		}
	}
	else
	{
		uString err = "Could not find file to run: ";
		err += g_sCurrentDir;
		err += filename;
		Error( err, 0, 0 );
		return false;
	}

    pid_t pid = fork();
	if ( pid == 0 ) execl( sPath.GetStr(), sPath.GetStr(), NULL );

	return true;
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
	//i = agk::PlatformLittleEndian( i );
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
	return i;// = agk::PlatformLocalEndian( i );
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

	/*
	char tempstr[ 1024 ];
	if ( !fgets( tempstr, 1024, pFile ) ) return 0;
	str.SetStr( tempstr );
	str.Trunc2( 1 );
	return str.GetLength();
	*/
	
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

	/* I am sure this code is okay, but I wanted to copy in something I've tested as its used in the interpreter now
	// read until a newline is found, or eof.
	bool bNullFound = false;
	do
	{
		long lPos = ftell( pFile );
		uint32_t written = fread( readbuf, 1, 256, pFile ); 
		for ( uint32_t i = 0; i < written; i++ )
		{
			if ( readbuf[ i ] == '\n' ) 
			{
				bNullFound = true;
				fseek( pFile, lPos+i+1, SEEK_SET );
				if ( i > 0 ) str.AppendN( readbuf, i );
				break;
			}
		}
		
		if ( !bNullFound ) str.AppendN( readbuf, written );
		else break;

	} while( !feof( pFile ) );

	return str.GetLength();
	*/
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
