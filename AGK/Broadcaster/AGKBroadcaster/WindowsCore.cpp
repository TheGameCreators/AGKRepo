#include "AGKBroadcaster.h"

namespace AGKBroadcaster
{
#ifdef AGK_STATIC
	int interactive = 0;
#else
	int interactive = 1;
#endif

	HANDLE hStdIn = 0;
	HANDLE hStdOut = 0;

	cLock g_lockOutput;
}

#ifndef AGK_STATIC
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdline, int showCmd )
{
#ifdef _DEBUG
	MessageBoxA( NULL, "Attach Now", "AGKBroadcaster", 0 );
#endif

    if ( strncmp( cmdline, "-nowindow", strlen("-nowindow") ) == 0 ) AGKBroadcaster::interactive = 0;
    
	if ( AGKBroadcaster::interactive )
	{
		AllocConsole();
		AGKBroadcaster::hStdIn = GetStdHandle ( STD_INPUT_HANDLE );
		AGKBroadcaster::hStdOut = GetStdHandle ( STD_OUTPUT_HANDLE );
	}
    
    return AGKBroadcaster::RunBroadcaster( cmdline );
}
#endif

namespace AGKBroadcaster
{
	int ReadStdInput( char* output, int maxlength )
	{
		if ( !interactive )
		{
			return fgets( output, maxlength, stdin ) ? 1 : 0;
		}
		else
		{
			DWORD written;
			if ( ReadConsole( hStdIn, output, maxlength, &written, 0 ) ) return 1;
			else return 0;
		}
	}


	void WriteStdOutput( const char* output )
	{
		cAutoLock lock(&g_lockOutput);

		if ( !interactive )
		{
			fputs( output, stdout );
			fputs( "\n", stdout );
			fflush( stdout );
			if (strlen(output) < MAXVALUESIZE) {
				AddBackLog(output);
			}
		}
		else
		{
			DWORD written;
			WriteConsole( hStdOut, output, (int)strlen(output), &written, 0 );
			WriteConsole( hStdOut, "\n", (int)strlen("\n"), &written, 0 );
		}
	}

	void WriteStdOutputPrompt()
	{
		if ( !interactive ) return;

		cAutoLock lock(&g_lockOutput);

		DWORD written;
		WriteConsole( hStdOut, "(AGK) ", (int)strlen("(AGK) "), &written, 0 );
	}
}