#include "cFile.h"

using namespace AGK_Compiler;

char cFile::g_szRootDir[ MAX_PATH ] = {0};
char cFile::g_szWriteDir[ MAX_PATH ] = {0};
uString cFile::g_sCurrentDir;

cFile::cFile()
{
	pFile = 0;
	pFilePtr = 0;
	mode = 0;
}

cFile::~cFile()
{
	Close();
}

bool cFile::IsOpen()
{
	return ( pFile || pFilePtr );
}

void cFile::SetCurrentFolder( const char* folder )
{
	g_sCurrentDir.SetStr( folder );
}

void cFile::GetProjectName( uString &output )
{
	char path[ MAX_PATH ];
	strcpy( path, g_szWriteDir );

	int length = (int) strlen( path );
	if ( path[ length-1 ] == '/' ) path[ length-1 ] = 0;
	
	char* slash = strrchr( path, '/' );
	
	if ( slash ) output.SetStr( slash+1 );
}

void cFile::PlatformGetFullPathWrite( uString &inout )
{
	inout.Replace( '\\', '/' );
	if ( inout.FindStr( "../" ) >= 0 ) 
	{
		uString sub;
		inout.SubString( sub, inout.RevFind( '/' )+1 );
		inout.SetStr( sub );
	}

	if ( inout.ByteAt( 0 ) == '/' ) 
	{
		uString sub;
		inout.SubString( sub, 1 );
		inout.SetStr( sub );
		inout.Prepend( g_szWriteDir );
	}
	else 
	{
		inout.Prepend( g_sCurrentDir );
		inout.Prepend( g_szWriteDir );
	}

}

void cFile::PlatformGetFullPathRead( uString &inout )
{
	inout.Replace( '\\', '/' );
	/*
	if ( inout.FindStr( "../" ) >= 0 ) 
	{
		uString sub;
		inout.SubString( sub, inout.RevFind( '/' )+1 );
		inout.SetStr( sub );
	}
	*/

	if ( inout.ByteAt( 0 ) == '/' ) 
	{
		uString sub;
		inout.SubString( sub, 1 );
		inout.SetStr( sub );
		inout.Prepend( g_szRootDir );
	}
	else 
	{
		inout.Prepend( g_sCurrentDir );
		inout.Prepend( g_szRootDir );
	}

}
