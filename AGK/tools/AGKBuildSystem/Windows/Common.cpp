#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <wininet.h>

#include "Common.h"

void Error( const char* err )
{
	printf( err );
	printf( "\n" );
	system("pause");
	exit(1);
}

void Message( const char* str )
{
	printf( str );
	printf( "\n" );
}

void Message1( const char* str, int num )
{
	printf( str, num );
	printf( "\n" );
}

FileRecord::FileRecord() 
{ 
	m_szFileNames = 0; m_iNumFiles = 0; m_iArrayLength = 0; 
}

FileRecord::~FileRecord() 
{
	Reset();
}

void FileRecord::AddFile( const char *path )
{
	if ( m_iNumFiles >= m_iArrayLength )
	{
		int newLength = (m_iArrayLength*3)/2 + 4;
		char **szNewArray = new char*[ newLength ];
		for ( int i = 0; i < m_iArrayLength; i++ ) szNewArray[i] = m_szFileNames[i];
		for ( int i = m_iArrayLength; i < newLength; i++ ) szNewArray[i] = 0;
		delete [] m_szFileNames;
		m_szFileNames = szNewArray;
		m_iArrayLength = newLength;
	}

	m_szFileNames[ m_iNumFiles ] = new char[ MAX_PATH ];
	strcpy( m_szFileNames[ m_iNumFiles ], path );
	m_iNumFiles++;
}

void FileRecord::Save( const char* filename )
{
	FILE *fp = fopen( filename, "wb" );
	if ( !fp ) Error( "Failed to open file for writing" );
	fprintf( fp, "%d\n", m_iNumFiles );
	for ( int i = 0; i < m_iNumFiles; i++ )
	{
		fwrite( m_szFileNames[i], 1, strlen(m_szFileNames[i]), fp );
		fwrite( "\n", 1, 1, fp );
	}
	fclose( fp );
}

void FileRecord::Reset() 
{
	for( int i = 0; i < m_iArrayLength; i++ ) 
	{
		if ( m_szFileNames[i] ) delete [] m_szFileNames[i];
	}
	if ( m_szFileNames ) delete [] m_szFileNames;

	m_szFileNames = 0; 
	m_iNumFiles = 0;
	m_iArrayLength = 0;
}

int FileRecord::GetFileCount() 
{ 
	return m_iNumFiles; 
}

char* FileRecord::GetFile( int index ) 
{
	if ( index < 0 || index >= m_iNumFiles ) return 0;
	return m_szFileNames[index];
}

void SetCurrentDirectoryWithCheck( const char* path )
{
	if ( !SetCurrentDirectory( path ) ) 
	{
		char str[1000];
		sprintf( str, "Failed to set path to: %s", path );
		Error( str );
	}
}

int RunCmd( int index, const char* cmd, const char* params )
{
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

	siStartInfo.cb = sizeof(STARTUPINFO);

	char szCmd[1024];
	strcpy( szCmd, "\"" );
	strcat( szCmd, cmd ); 
	strcat( szCmd, "\" " );
	strcat( szCmd, params );

	printf( "Running %d: ", index );
	printf( szCmd );
	printf( "\n" );
	
	BOOL result = CreateProcess( NULL, szCmd, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo );
	if ( !result )
	{
		Error( "Failed to run command" );
	}

	DWORD ret = WaitForSingleObject( piProcInfo.hProcess, INFINITE );

	DWORD dwStatus = 0;
	GetExitCodeProcess( piProcInfo.hProcess, &dwStatus );
	int status = dwStatus;

	CloseHandle( piProcInfo.hProcess );
	CloseHandle( piProcInfo.hThread );

	return status;
}

int GetFileContents( const char* filepath, char **data )
{
	FILE *fp = fopen( filepath, "rb" );
	if ( !fp ) return 0;
	fseek( fp, 0, SEEK_END );
	size_t size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	*data = new char[ size+1 ];
	size_t total = 0;
	while( total < size )
	{
		size_t bytes = fread( *(data+total), 1, size-total, fp );
		if ( bytes == 0 ) return 0;
		total += bytes;
	}
	fclose( fp );

	(*data)[size] = 0;
	return (int)size;
}

int CopyFile2( const char *src, const char *dst )
{
	char buf[ 4096 ];
	size_t bytes = 0;
	FILE *fpSrc = fopen( src, "rb" );
	if ( !fpSrc ) 
	{
		char err[1000]; sprintf( err, "Failed to open file for reading: %s", src );
		Error( err );
		return 0;
	}

	FILE *fpDst = fopen( dst, "wb" );
	if ( !fpDst ) 
	{
		char err[1000]; sprintf( err, "Failed to open file for writing: %s", dst );
		Error( err );
		return 0;
	}

	do
	{
		bytes = fread( buf, 1, 4096, fpSrc );
		if ( bytes > 0 ) fwrite( buf, 1, bytes, fpDst );
	} while( bytes > 0 );

	fclose( fpSrc );
	fclose( fpDst );

	return 1;
}

int GetPathExistsUTF8( const char *szPath )
{
	int size = MultiByteToWideChar( CP_UTF8, 0, szPath, -1, 0, 0 );
	wchar_t *wzPath = new wchar_t[ size ];
	MultiByteToWideChar( CP_UTF8, 0, szPath, -1, wzPath, size );

	unsigned int result = GetFileAttributesW( wzPath );
	delete [] wzPath;

	if ( result == INVALID_FILE_ATTRIBUTES ) return 0;
	else if ( result & FILE_ATTRIBUTE_DIRECTORY ) return 1;
	else return 2;
}

BOOL CreateDirectoryUTF8( const char* szPath, LPSECURITY_ATTRIBUTES sec )
{
	int size = MultiByteToWideChar( CP_UTF8, 0, szPath, -1, 0, 0 );
	wchar_t *wzPath = new wchar_t[ size ];
	MultiByteToWideChar( CP_UTF8, 0, szPath, -1, wzPath, size );

	BOOL result = CreateDirectoryW( wzPath, sec );
	delete [] wzPath;
	return result;
}

int CreatePath( const char *path )
{
	if ( !path || !*path ) return 0;
	if ( path[1] != ':' ) 
	{
		char err[1000]; sprintf( err, "Invalid path \"%s\", must be absolute", path );
		Error( err );
		return 0;
	}

	char *newPath = new char[ strlen(path) + 1 ];
	strcpy( newPath, path );
	char *origPath = newPath;

	// skip drive letter
	newPath += 3;

	// convert backslashes to forward slashes
	char *ptr = newPath;
	while( *ptr ) 
	{
		if ( *ptr == '\\' ) *ptr = '/';
		ptr++;
	}

	char *szPrev = newPath;
	char *szSlash = 0;
	while( (szSlash = strchr( szPrev, '/' )) )
	{
		unsigned int length = (unsigned int)(szSlash-szPrev);
		if ( length == 0 )
		{
			char err[1000]; sprintf( err, "Invalid path \"%s\", folder names must have at least one character", origPath );
			delete [] origPath;
			Error( err );
			return 0;
		}

		*szSlash = 0;

		if ( !GetPathExistsUTF8( origPath ) && !CreateDirectoryUTF8( origPath, NULL ) )
		{
			char err[1000]; sprintf( err, "Failed to create path \"%s\", the app may not have permission to create it", origPath );
			delete [] origPath;
			Error( err );
			return 0;
		}

		*szSlash = '/';
		
		szPrev = szSlash+1;
	}

	delete [] origPath;
	return 1;
}

// must be absolute paths
int CopyFolder( const char *src, const char *dst, int numIgnore, const char **szIgnoreExt )
{
	if ( strchr( src, ':' ) == 0 ) Error( "Failed to copy, source path must be absolute" );
	if ( strchr( dst, ':' ) == 0 ) Error( "Failed to copy, destination path must be absolute" );

	char currDir[ 1024 ];
	GetCurrentDirectory( 1024, currDir );
	SetCurrentDirectoryWithCheck( src );
	
	WIN32_FIND_DATA	FindFileData;
	HANDLE hFind = FindFirstFile ( "*.*", &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE) return 0;

	do
	{
		if ( strcmp ( FindFileData.cFileName, "." ) != 0 && strcmp ( FindFileData.cFileName, ".." ) != 0 )
		{
			if ( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				char newDst[ 1024 ];
				strcpy( newDst, dst );
				strcat( newDst, "\\" );
				strcat( newDst, FindFileData.cFileName );
				
				CreateDirectory( newDst, 0 );
				if ( !CopyFolder( newSrc, newDst, numIgnore, szIgnoreExt ) ) return 0;
			}
			else
			{
				// file
				bool bSkip = false;
				char *szExt = strrchr( FindFileData.cFileName, '.' );
				for ( int i = 0; i < numIgnore; i++ )
				{
					if ( strcmp(szIgnoreExt[i], szExt) == 0 )
					{
						bSkip = true;
						break;
					}
				}

				if ( bSkip ) continue;

				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				char newDst[ 1024 ];
				strcpy( newDst, dst );
				strcat( newDst, "\\" );
				strcat( newDst, FindFileData.cFileName );

				if ( !CopyFile2( newSrc, newDst ) ) return 0;
			}
		}
	} while ( FindNextFile ( hFind, &FindFileData ) );

	FindClose( hFind );

	SetCurrentDirectoryWithCheck( currDir );
	return 1;
}

// will only copy files that already exist in the destination folder
int UpdateFolder( const char *src, const char *dst, int numIgnore, const char **szIgnoreExt )
{
	if ( strchr( src, ':' ) == 0 ) Error( "Failed to update folder, source path must be absolute" );
	if ( strchr( dst, ':' ) == 0 ) Error( "Failed to update folder, destination path must be absolute" );

	char currDir[ 1024 ];
	GetCurrentDirectory( 1024, currDir );
	SetCurrentDirectoryWithCheck( dst );
	
	WIN32_FIND_DATA	FindFileData;
	HANDLE hFind = FindFirstFile ( "*.*", &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE) return 0;

	do
	{
		if ( strcmp ( FindFileData.cFileName, "." ) != 0 && strcmp ( FindFileData.cFileName, ".." ) != 0 )
		{
			if ( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				char newDst[ 1024 ];
				strcpy( newDst, dst );
				strcat( newDst, "\\" );
				strcat( newDst, FindFileData.cFileName );
				
				if ( !UpdateFolder( newSrc, newDst, numIgnore, szIgnoreExt ) ) return 0;
			}
			else
			{
				// file
				bool bSkip = false;
				char *szExt = strrchr( FindFileData.cFileName, '.' );
				for ( int i = 0; i < numIgnore; i++ )
				{
					if ( strcmp(szIgnoreExt[i], szExt) == 0 )
					{
						bSkip = true;
						break;
					}
				}

				if ( bSkip ) continue;

				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				char newDst[ 1024 ];
				strcpy( newDst, dst );
				strcat( newDst, "\\" );
				strcat( newDst, FindFileData.cFileName );

				if ( !CopyFile2( newSrc, newDst ) ) return 0;
			}
		}
	} while ( FindNextFile ( hFind, &FindFileData ) );

	FindClose( hFind );

	SetCurrentDirectoryWithCheck( currDir );
	return 1;
}

int RecordFiles( const char *base, const char* sub, FileRecord* fileStore, int numIgnore, const char **szIgnoreExt )
{
	if ( strchr( base, ':' ) == 0 ) Error( "Failed to record files, source path must be absolute" );
	if ( !fileStore ) Error( "File store is null" );

	char currDir[ 1024 ];
	GetCurrentDirectory( 1024, currDir );
	SetCurrentDirectoryWithCheck( base );
	SetCurrentDirectoryWithCheck( sub );
	
	WIN32_FIND_DATA	FindFileData;
	HANDLE hFind = FindFirstFile ( "*.*", &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE) return 0;

	do
	{
		if ( strcmp ( FindFileData.cFileName, "." ) != 0 && strcmp ( FindFileData.cFileName, ".." ) != 0 )
		{
			if ( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, sub );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				if ( !RecordFiles( base, newSrc, fileStore, numIgnore, szIgnoreExt ) ) return 0;
			}
			else
			{
				// file
				bool bSkip = false;
				char *szExt = strrchr( FindFileData.cFileName, '.' );
				for ( int i = 0; i < numIgnore; i++ )
				{
					if ( strcmp(szIgnoreExt[i], szExt) == 0 )
					{
						bSkip = true;
						break;
					}
				}

				if ( bSkip ) continue;

				char newSrc[ 1024 ];
				strcpy( newSrc, sub );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				fileStore->AddFile( newSrc );
			}
		}
	} while ( FindNextFile ( hFind, &FindFileData ) );

	FindClose( hFind );

	SetCurrentDirectoryWithCheck( currDir );
	return 1;
}

int DeleteFolder( const char *src )
{
	char currDir[ 1024 ];
	GetCurrentDirectory( 1024, currDir );
	if ( !SetCurrentDirectory( src ) ) return 1;
	
	WIN32_FIND_DATA	FindFileData;
	HANDLE hFind = FindFirstFile ( "*.*", &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE) return 0;

	do
	{
		if ( strcmp ( FindFileData.cFileName, "." ) != 0 && strcmp ( FindFileData.cFileName, ".." ) != 0 )
		{
			if ( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				if ( !DeleteFolder( newSrc ) ) return 0;
				if ( !RemoveDirectory( newSrc ) )
				{
					char err[1024]; sprintf( err, "Failed to remove directory: %s", newSrc );
					Error( err );
					return 0;
				}
			}
			else
			{
				// file
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				if ( !DeleteFile( newSrc ) ) 
				{
					char err[1024]; sprintf( err, "Failed to delete file: %s", newSrc );
					Error( err );
					return 0;
				}
			}
		}
	} while ( FindNextFile ( hFind, &FindFileData ) );

	FindClose( hFind );

	SetCurrentDirectoryWithCheck( currDir );
	return 1;
}

int UpdateWhatsNewFile()
{
	char *whats_new_text = 0;
	int whats_new_size = GetFileContents( "AGK Help Generator\\AGK Txt\\whats_new.html", &whats_new_text );
	if ( !whats_new_size ) Error( "Failed to read whats new file" );
	
	char *agk_text = 0;
	int agk_size = GetFileContents( "AGK.txt", &agk_text );
	if ( !agk_size ) Error( "Failed to read AGK change log" );

	char *whats_new_header = strstr( whats_new_text, "<h1>Change Log</h1>" );
	if ( !whats_new_header ) Error( "Failed to find What's New file header" );
	whats_new_header += strlen("<h1>Change Log</h1>");
	
	// find latest build number in What's New file
	char *build_num = strstr( whats_new_header, "<h2>" );
	if ( !build_num ) Error( "Failed to find build number in What's New file" );
	build_num += strlen("<h2>");
	char *build_num_end = strstr( build_num, "</" );
	if ( !build_num_end ) Error( "Failed to find build number end in What's New file" );
	unsigned int buildLength = (unsigned int)(build_num_end - build_num);

	// find latest build number in AGK.txt
	char *curr_build_num_end = strstr( agk_text, "-------" );
	if ( !curr_build_num_end ) Error( "Failed to find build number end in AGK.txt" );
	curr_build_num_end--;
	if ( *curr_build_num_end != '\n' ) Error( "Unexpected build number end in AGK.txt, expected a new line character" );
	while( strchr( " \n\r", *curr_build_num_end ) ) curr_build_num_end--;
	char *curr_build_num = curr_build_num_end;
	while( curr_build_num > agk_text && *curr_build_num != '\n' ) curr_build_num--;
	if ( curr_build_num <= agk_text ) Error( "Failed to find build number beginning in AGK.txt" );
	while( strchr( " \n\r", *curr_build_num ) ) curr_build_num++;
	unsigned int buildLength2 = (unsigned int)(curr_build_num_end - curr_build_num) + 1;
	if ( buildLength2 > buildLength ) buildLength = buildLength2;

	char szNewChanges[ 100000 ];
	*szNewChanges = 0;
	bool bNewHeader = false;

	if ( strncmp(curr_build_num, build_num, buildLength) != 0 )
	{
		// build heading has not been added
		bNewHeader = true;
		strcpy( szNewChanges, "<h2>" );
		strncat( szNewChanges, curr_build_num, buildLength2 );
		strcat( szNewChanges, "</h2>\n\n<ul>\n" );
	}

	// find end of current build changes
	build_num_end = strstr( build_num_end, "<h2>" );
	if ( !build_num_end ) Error( "Failed to find end of build changes" );
	*build_num_end = 0;

	// find end of new build changes
	curr_build_num_end++;
	while( strchr( " \n\r", *curr_build_num_end ) ) curr_build_num_end++;
	curr_build_num_end = strstr( curr_build_num_end, "\n" );
	char *nextBuild = strstr( curr_build_num_end, "-------" );
	if ( !nextBuild ) Error( "Failed to find start of next build text" );
	*nextBuild = 0;

	char *nextChange = strstr( curr_build_num_end+1, "\n-" );
	while( nextChange )
	{
		nextChange += strlen("\n-");
		while( *nextChange == ' ' ) nextChange++;

		char *changeEnd = strstr( nextChange, "\n-" );
		if ( !changeEnd ) changeEnd = strstr( nextChange, "\n" );
		if ( !changeEnd ) Error( "Failed to find change end" );
		while( strchr( " \n\r", *changeEnd ) ) changeEnd--;
		changeEnd++;
		*changeEnd = 0;

		char changeText[ 1000 ];
		strcpy( changeText, nextChange );

		if ( strstr( whats_new_header, changeText ) )
		{
			// change already exists
			*changeEnd = '\n';
			nextChange = strstr( changeEnd, "\n-" );
			continue;
		}

		strcat( szNewChanges, "<li>" );
		strcat( szNewChanges, changeText );
		strcat( szNewChanges, "</li>\n" );

		*changeEnd = '\n';
		nextChange = strstr( changeEnd, "\n-" );
	}

	// repair file
	*build_num_end = '<';

	char *szNewData = new char[ whats_new_size + strlen(szNewChanges) + 1000 ];
	*szNewData = 0;

	char *breakPoint = 0;
	if ( bNewHeader )
	{
		strcat( szNewChanges, "</ul>\n\n" );
		breakPoint = strstr( whats_new_header, "<h2>" );
	}
	else
	{
		breakPoint = strstr( whats_new_header, "</ul>" );
	}

	if ( !breakPoint ) Error( "Failed to find break point in whats new text with header" );
	*breakPoint = 0;

	strcpy( szNewData, whats_new_text );
	strcat( szNewData, szNewChanges );
	*breakPoint = '<';
	strcat( szNewData, breakPoint );

	FILE *whats_new_file = fopen( "AGK Help Generator\\AGK Txt\\whats_new.html", "wb" );
	fwrite( szNewData, 1, strlen(szNewData), whats_new_file );
	fclose( whats_new_file );

	delete [] agk_text;
	delete [] whats_new_text;
	delete [] szNewData;

	return 1;
}
