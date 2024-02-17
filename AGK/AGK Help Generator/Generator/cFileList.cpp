
#include <time.h>
#include "cFileList.h"


cFileList::cFileList ( )
{
	
}

cFileList::~cFileList ( )
{

}


bool cFileList::Search ( char* szFilter )
{
	// set up our local variables
	WIN32_FIND_DATA	FindFileData;
	HANDLE			hFind				= NULL;
	char			szPattern [ 256 ]	= "";
	BOOL			bRC					= TRUE;
	char			szDirectory [ 256 ] = "";

	// get the full path
	GetCurrentDirectory ( 256, szDirectory );

	// set up the file pattern to look for
	sprintf_s ( szPattern, "%s\\%s", szDirectory, szFilter );

	// find the first file
	hFind = FindFirstFile ( szPattern, &FindFileData );

	if ( hFind == INVALID_HANDLE_VALUE )
	{
		DWORD dwError = GetLastError ( );

		char szBuffer [ 256 ] = "";

		FormatMessage ( 0, 0, dwError, 0, szBuffer, 256, NULL );

		return false;
	}

	// continue searching for files
	while ( bRC )
	{
		if ( strcmp ( FindFileData.cFileName, "." ) != 0 && strcmp ( FindFileData.cFileName, ".." ) != 0 )
		{
			char* szName = new char [ 256 ];
			strcpy_s ( szName, sizeof ( char ) * 256, FindFileData.cFileName );
			m_List.push_back ( szName );
		}

		// search for our next file
		bRC = FindNextFile ( hFind, &FindFileData );
	}

	// finish the searching process
	FindClose ( hFind );

	return true;
}

DWORD cFileList::GetCount ( void )
{
	return m_List.size ( );
}

char* cFileList::GetName ( int iIndex )
{
	if ( iIndex < ( int ) m_List.size ( ) )
		return m_List [ iIndex ];

	return NULL;
}

DWORD cFileList::FindMatchingFiles ( char* szMatch )
{
	DWORD dwCount = 0;

	for ( DWORD dwFile = 0; dwFile < GetCount ( ); dwFile++ )
	{
		if ( strstr ( GetName ( dwFile ), szMatch ) )
		{
			m_MatchingList.push_back ( GetName ( dwFile ) );
			dwCount++;
		}
	}

	return dwCount;
}

DWORD cFileList::FindAllFiles ( void )
{
	DWORD dwCount = 0;

	for ( DWORD dwFile = 0; dwFile < GetCount ( ); dwFile++ )
	{
		m_MatchingList.push_back ( GetName ( dwFile ) );
		dwCount++;
	}

	return dwCount;
}

char* cFileList::GetMatchingFile ( int iIndex )
{
	if ( iIndex < ( int ) m_MatchingList.size ( ) )
		return m_MatchingList [ iIndex ];

	return NULL;
}