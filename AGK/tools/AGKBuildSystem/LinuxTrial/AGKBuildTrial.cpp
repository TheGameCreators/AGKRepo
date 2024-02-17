#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "../Linux/Common.h"

int main( int argc, char* argv[] )
{
	// set some path variables
	const char* szSharedFolder = "/home/paul/Shares/"; // for Mac and Linux access
	
	SetCurrentDirectoryWithCheck( "../../.." ); // AGKTrunk

	int index = -1;
	bool bSingleCommand = false;
	bool bListCommands = true;
	int iAndroidSigningIndex = 0;
	char szPassword[ 256 ];	

startPoint:

	if ( !bListCommands )
	{
		printf( "Enter start point (use 's' to do a single step): " );

		char input[256];
		if ( !fgets(input, 256, stdin) ) Error( "Failed to read input" );
		if ( *input == 's' )
		{
			bSingleCommand = true;
			index = atoi( input+1 );
		}
		else
		{
			index = atoi( input );
		}
	}

	int indexCheck = 0;

	// Update SVN
	if ( !bListCommands )
	{
		int status = 0;
		status = RunCmd( indexCheck, "svn", "update" );
		if ( status != 0 ) Error( "  Failed" );
		else Message( "  Success" );
	}
	
	// Clean Linux
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Clean Linux lib", indexCheck );
		else
		{
			Message( "Cleaning Linux" );
			int status = RunCmd( indexCheck, "make", "clean" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Build Linux
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux Trial lib", indexCheck );
		else
		{
			Message( "Building Linux Trial" );
			int status = RunCmd( indexCheck, "make", "-j2 trial" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Build Linux interpreter
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux Trial interpreter", indexCheck );
		else
		{
			Message( "Building Linux Trial interpreter" );
			SetCurrentDirectoryWithCheck( "apps/interpreter_linux" );
			
			int status = RunCmd( indexCheck, "make", "clean" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			status = RunCmd( indexCheck, "make", "-j2 trial" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			SetCurrentDirectoryWithCheck( "../.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Build Linux compiler
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux Trial compiler", indexCheck );
		else
		{
			Message( "Building Linux Trial compiler" );
			SetCurrentDirectoryWithCheck( "CompilerNew" );
			
			int status = RunCmd( indexCheck, "make", "clean" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			status = RunCmd( indexCheck, "make", "-j2 trial" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			SetCurrentDirectoryWithCheck( ".." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Build Linux broadcaster
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux broadcaster", indexCheck );
		else
		{
			Message( "Building Linux broadcaster" );
			SetCurrentDirectoryWithCheck( "Broadcaster/AGKBroadcaster" );
			
			int status = RunCmd( indexCheck, "make", "clean" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			status = RunCmd( indexCheck, "make", "-j2" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			SetCurrentDirectoryWithCheck( "../.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Build Linux IDE
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux Trial IDE", indexCheck );
		else
		{
			Message( "Building Linux Trial IDE" );
			SetCurrentDirectoryWithCheck( "IDE/Geany-1.24.1" );
			
			// edit geany.h
			Message( "  Editing header" );
			char *data = 0;
			int size = GetFileContents( "src/geany.h", &data );
			if ( !data || !size ) Error( "    Failed to read geany.h" );
			char *define = strstr( data, "//#define AGK_FREE_VERSION" );
			if ( !define )
			{
				Message( "    Couldn't find commented out define, looking for active define" );
				define = strstr( data, "  #define AGK_FREE_VERSION" );
				if ( !define ) Error( "    Failed to find free version define in geany.h" );
				else Message( "    Define is already active" );
			}
			else
			{
				Message( "    Making free version define active" );
				define[ 0 ] = ' ';
				define[ 1 ] = ' ';

				FILE *fp = fopen( "src/geany.h", "wb" );
				if ( !fp ) Error( "Failed to open geany.h for writing" );
				fwrite( data, 1, size, fp );
				fclose( fp );
			}
			
			Message( "  Compiling" );
			int status = RunCmd( indexCheck, "make", "-j2" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			status = RunCmd( indexCheck, "make", "install" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			Message( "Editing header" );
			Message( "  Commenting out free version define" );
			define[ 0 ] = '/';
			define[ 1 ] = '/';

			FILE *fp = fopen( "src/geany.h", "wb" );
			if ( !fp ) Error( "  Failed to open geany.h for writing" );
			fwrite( data, 1, size, fp );
			fclose( fp );

			delete [] data;
			
			SetCurrentDirectoryWithCheck( "../.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Copy Linux build files
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Copy Linux build files", indexCheck );
		else
		{
			Message( "Copying Linux build files" );
			int status = system("tools/AGKBuildSystem/LinuxTrial/CopyLinuxTrial64.sh");
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	if ( bListCommands ) 
	{
		bListCommands = false;
		goto startPoint;
	}
	
endPoint:
	return 0;
}
