#include "AppErrors.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

namespace AGK
{
	class agk
	{
		public:
			static void Warning( const char *szMsg );
			static void Error( const char *szMsg );
	};
}

using namespace AGK;

void ReportAppMessage( const char* type, const char* file, int line, const char* msg, ... )
{
	va_list argList;

	// get the size needed for the final string
	va_start(argList, msg);
	int newLength = vsnprintf( 0, 0, msg, argList);
    va_end(argList);

	char *msg2 = (char*)msg; // promise I won't modify it
	if ( newLength >= 0 )
	{
		msg2 = new char[ newLength + 1 ];
		va_start(argList, msg);
		vsnprintf( msg2, newLength+1, msg, argList);
		va_end(argList);
	}

	const char* szFormat = "%s:%d: %s";
	const char* slash = strrchr( file, '\\' );
	if ( slash ) file = slash + 1;

	// get the size needed for the final string
	newLength = snprintf( 0, 0, szFormat, file, line, msg2 );
    if ( newLength < 0 )
	{
		// don't know how much to allocate
		switch( *type )
		{
			case 'E': agk::Error( msg2 ); break;
			case 'W': agk::Warning( msg2 ); break;
			case 'I': agk::Warning( msg2 ); break;
		}
		
		if ( msg2 != msg ) delete [] msg2;
		return;
	}

	char *msg3 = new char[ newLength + 1 ];
	snprintf( msg3, newLength+1, szFormat, file, line, msg2 );
	switch( *type )
	{
		case 'E': agk::Error( msg2 ); break;
		case 'W': agk::Warning( msg2 ); break;
		case 'I': agk::Warning( msg2 ); break;
	}
	delete [] msg3;
	if ( msg2 != msg ) delete [] msg2;
}

int CheckAppErrors()
{
	int numEntries = sizeof(g_AppErrorDescs)/sizeof(AppErrorDesc);
	
	for( int i = 0; i < APP_ERROR_NUM && i < numEntries; i++ )
	{
		if ( g_AppErrorDescs[ i ].ID != i ) 
		{
			AppError( "Error \"%s\" (%d) is in the wrong place (%d)", g_AppErrorDescs[ i ].szDesc, g_AppErrorDescs[ i ].ID, i );
			return 1;
		}
	}

	if ( numEntries != APP_ERROR_NUM ) 
	{
		AppError( "Expected %d errors, found %d", APP_ERROR_NUM, numEntries );
		return 1;
	}

	return 0;
}