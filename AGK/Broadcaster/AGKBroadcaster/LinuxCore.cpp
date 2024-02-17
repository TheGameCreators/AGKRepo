#include "AGKBroadcaster.h"

namespace AGKBroadcaster
{
#ifdef AGK_STATIC
	int interactive = 0;
#else
	int interactive = 1;
#endif

	cLock g_lockOutput;
}

#ifndef AGK_STATIC
int main( int argc, char** argv )
{
	if ( argc > 1 && strcmp( argv[1], "-nowindow" ) == 0 ) AGKBroadcaster::interactive = 0;

    uString sCmdLine;
    for( int i = 1; i < argc; i++ )
    {
        if ( i != 1 ) sCmdLine.AppendAscii( ' ' );
        sCmdLine.Append( argv[i] );
    }
    return AGKBroadcaster::RunBroadcaster( sCmdLine.GetStr() );
}
#endif

namespace AGKBroadcaster
{
	int ReadStdInput( char* output, int maxlength )
	{
		return fgets( output, maxlength, stdin ) ? 1 : 0;
	}

	void WriteStdOutput( const char* output )
	{
		cAutoLock lock(&g_lockOutput);

		fputs( output, stdout );
		fputs( "\n", stdout );
		fflush( stdout );
		if (strlen(output) < MAXVALUESIZE) {
			AddBackLog(output);
		}
	}

	void WriteStdOutputPrompt()
	{
		if ( !interactive ) return;

		cAutoLock lock(&g_lockOutput);

		fputs( "(AGK) ", stdout );
		fflush( stdout );
	}
} // namespace AGKBroadcaster