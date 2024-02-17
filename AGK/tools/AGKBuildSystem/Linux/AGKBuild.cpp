#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "Common.h"
#include "Paths.h"

int main( int argc, char* argv[] )
{
	// set some path variables
	//const char* szSharedFolder = "/home/paul/Shares/WindowsReceive";
	//const char* szLinuxFiles = "/home/paul/Shares/AGKStudioLinux";
	//const char* szBuildFolder = "/home/paul/AGKStudioBuild";
	
	SetCurrentDirectoryWithCheck( "../../.." ); // AGKTrunk

	int index = -1;
	bool bSingleCommand = false;
	bool bListCommands = true;

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

	bool bHTML5Init = false;
	
	// Clean HTML5
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Clean HTML5", indexCheck );
		else
		{
			Message( "Cleaning HTML5" );
			int status = RunCmd( indexCheck, "make", "clean -f Makefile_html5" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Build HTML5
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build HTML5", indexCheck );
		else
		{
			Message( "Building HTML5" );
			SetCurrentDirectoryWithCheck( "tools/AGKBuildSystem/Linux" );
			int status = system("./HTML5Build.sh");
			if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
			else Message( "Success" );
			
			SetCurrentDirectoryWithCheck( "../../.." ); // AGKTrunk
			
			const char *szPaths[] = { "3D", "3Ddynamic", "2D", "2Ddynamic" };
			
			for ( int i = 0; i < 4; i++ )
			{
				char str[ 256 ]; sprintf( str, "Editing AGKPlayer.js for %s", szPaths[i] );
				Message( str );
				
				char path[ 1024 ];
				strcpy( path, "AgkIde/media/data/html5/" );
				strcat( path, szPaths[i] );
				strcat( path, "/AGKPlayer.js" );
				
				char *data = 0;
				int size = GetFileContents( path, &data );
				if ( !size || !data ) Error( "Failed to get AGKPlayer.js data" );
				
				if ( strstr( data, "%%LOADPACKAGE%%" ) != 0 ) Error( "Failed - File already has %%LOADPACKAGE%%" );
				if ( strstr( data, "%%ADDITIONALFOLDERS%%" ) != 0 ) Error( "Failed - File already has %%ADDITIONALFOLDERS%%" );
				
				char *module = strstr( data, "Module[\"FS_createPath\"](" );
				if ( !module ) 
				{
					module = strstr( data, "Module['FS_createPath'](" );
					if ( !module ) Error( "Failed to find Module[FS_createPath]" );
				}
				char *endModule = strstr( module, ");" );
				if ( !endModule ) Error( "Failed to find end of Module[FS_createPath]" );
			
				char *loadPackage = strstr( endModule, "loadPackage({" );
				if ( !loadPackage ) Error( "Failed to find loadPackage" );
				char *endLoadPackage = strstr( loadPackage, "})" );
				if ( !endLoadPackage ) Error( "Failed to find end of loadPackage" );
				
				*module = 0;
				endModule += 2;
				
				*loadPackage = 0;
				endLoadPackage += 2;
				
				char *newData = new char[ size + 100 ];
				strcpy( newData, data );
				strcat( newData, "%%ADDITIONALFOLDERS%%" );
				strcat( newData, endModule );
				strcat( newData, "%%LOADPACKAGE%%" );
				strcat( newData, endLoadPackage );
				
				FILE *fp = fopen( path, "wb" );
				if ( !fp ) Error( "Failed to open AGKPlayer.js for writing" );
				fwrite( newData, 1, strlen(newData), fp );
				fclose( fp );
				
				delete [] newData;
				delete [] data;
			}
			
			if ( bSingleCommand ) goto endPoint;
		}
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
		if ( bListCommands ) Message1( "%d: Build Linux lib", indexCheck );
		else
		{
			Message( "Building Linux" );
			int status = RunCmd( indexCheck, "make", "-j2" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Build Linux interpreter
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux interpreter", indexCheck );
		else
		{
			Message( "Building Linux interpreter" );
			SetCurrentDirectoryWithCheck( "apps/interpreter_linux" );
			
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
	
	// Build Linux template (just to test, doesn't influence final build)
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux template", indexCheck );
		else
		{
			Message( "Building Linux template" );
			SetCurrentDirectoryWithCheck( "apps/template_linux" );
			
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
	
	// Build Linux compiler
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux compiler static lib", indexCheck );
		else
		{
			Message( "Building Linux compiler static lib" );
			SetCurrentDirectoryWithCheck( "CompilerNew" );
			
			int status = RunCmd( indexCheck, "make", "clean" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			status = RunCmd( indexCheck, "make", "-j2 staticide" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			SetCurrentDirectoryWithCheck( ".." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Build Linux broadcaster
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux broadcaster static lib", indexCheck );
		else
		{
			Message( "Building Linux broadcaster static lib" );
			SetCurrentDirectoryWithCheck( "Broadcaster/AGKBroadcaster" );
			
			int status = RunCmd( indexCheck, "make", "clean" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			status = RunCmd( indexCheck, "make", "-j2 staticide" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			SetCurrentDirectoryWithCheck( "../.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// remove old build
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Delete old build", indexCheck );
        else
        {
            Message( "Deleting old build folder" );
            char srcFolder[ 1024 ]; getcwd( srcFolder, 1024 );
            char dstFolder[ 1024 ];
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio" );
            DeleteFolder( dstFolder );
            SetCurrentDirectoryWithCheck( srcFolder );
            
            if ( bSingleCommand ) goto endPoint;
        }
    }
	
	// Build Linux IDE
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Linux IDE", indexCheck );
		else
		{
			Message( "Building Linux IDE" );
			SetCurrentDirectoryWithCheck( "AgkIde" );
			
			int status = RunCmd( indexCheck, "make", "clean" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );

			status = RunCmd( indexCheck, "make", "-j2" );
			if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
			else Message( "  Success" );

			// copy media folder
			Message( "Copying media folder" );
			char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];
			getcwd( srcFolder, 1024 ); strcat( srcFolder, "/media" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/media" ); 
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy binary
			Message( "Copying binary" );
			getcwd( srcFolder, 1024 ); strcat( srcFolder, "/linuxbuild/AgkStudio64" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AgkStudio64" ); 
            CopyFile2( srcFolder, dstFolder );

			Message( "Making binary executable" );
			char szTemp[ 1024 ];
			sprintf( szTemp, "chmod 0755 \"%s/AppGameKitStudio/AgkStudio64\"", szBuildFolder );
            status = system( szTemp );
            if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
            else Message( "Success" );

			Message( "Making aapt2 binary executable" );
			sprintf( szTemp, "chmod 0755 \"%s/AppGameKitStudio/media/data/android/aapt2-bundle\"", szBuildFolder );
            status = system( szTemp );
            if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
            else Message( "Success" );

			Message( "Making zipalign binary executable" );
			sprintf( szTemp, "chmod 0755 \"%s/AppGameKitStudio/media/data/android/zipalign\"", szBuildFolder );
            status = system( szTemp );
            if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
            else Message( "Success" );
			
			SetCurrentDirectoryWithCheck( ".." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Copy internal files into IDE
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Copy internal files into IDE", indexCheck );
        else
        {
            Message( "Copying internal files into IDE" );

			char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];

			// copy android libs
            Message( "Copying Android libs" );
            strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/Android/lib" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/media/data/android/lib" ); 
            DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );
            
            // copy android source folders
			Message( "Copying Android sourceAmazon" );
			strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/Android/sourceAmazon" ); 
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/media/data/android/sourceAmazon" ); 
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			Message( "Copying Android sourceGoogle" );
			strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/Android/sourceGoogle" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/media/data/android/sourceGoogle" ); 
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			Message( "Copying Android sourceOuya" );
			strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/Android/sourceOuya" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/media/data/android/sourceOuya" ); 
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );
			            
             Message( "Copying interpreter" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/apps/interpreter_linux/build/LinuxPlayer64" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/media/interpreters/LinuxPlayer64" );
            CopyFile2( srcFolder, dstFolder );

			// make interpreter executable
			Message( "Making binary executable" );
			char szTemp[ 1024 ];
			sprintf( szTemp, "chmod 0755 \"%s/AppGameKitStudio/media/interpreters/LinuxPlayer64\"", szBuildFolder );
            int status = system( szTemp );
            if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
            else Message( "Success" );

			// copy help
            Message( "Copying help files" );
            strcpy( srcFolder, szLinuxFiles ); strcat( srcFolder, "/AGK/Tier1/Help" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/media/Help" );
            CopyFolder( srcFolder, dstFolder );

			// copy projects
            Message( "Copying projects" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/Examples" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Projects" );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
            
            if ( bSingleCommand ) goto endPoint;
        }
    }

	// Copy external files
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Copy external files", indexCheck );
        else
        {
            Message( "Copying external files" );
            char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];
            
            // copy players from shared folder
			Message( "Copying Players" );
            strcpy( srcFolder, szLinuxFiles ); strcat( srcFolder, "/AGK/Players" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Players" );
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy new Linux player
			Message( "Copying Linux player" );
			getcwd( srcFolder, 1024 ); strcat( srcFolder, "/apps/interpreter_linux/build/LinuxPlayer64" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Players/Linux/LinuxPlayer64" );
            CopyFile2( srcFolder, dstFolder );

			// make interpreter executable
			Message( "Making Linux player executable" );
			char szTemp[ 1024 ];
			sprintf( szTemp, "chmod 0755 \"%s/AppGameKitStudio/Players/Linux/LinuxPlayer64\"", szBuildFolder );
            int status = system( szTemp );
            if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
            else Message( "Success" );

			// copy example plugin
			Message( "Copying example plugin" );
			strcpy( srcFolder, szLinuxFiles ); strcat( srcFolder, "/AGK/Example Plugin" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Example Plugin" );
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy plugins
			Message( "Copying Plugins" );
			getcwd( srcFolder, 1024 ); strcat( srcFolder, "/plugins/Plugins" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Plugins" );
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy Tier 2 files
			Message( "Copying Tier 2" );
			strcpy( srcFolder, szLinuxFiles ); strcat( srcFolder, "/AGK/Tier2" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier2" );
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			Message( "Copying Linux Release64 Lib" );
			getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/linux/Lib/Release64/libAGKLinux.a" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier2/platform/linux/Lib/Release64/libAGKLinux.a" );
            CopyFile2( srcFolder, dstFolder );
            
			// change log
			Message( "Copying change log" );
            strcpy( srcFolder, szLinuxFiles ); strcat( srcFolder, "/AGK/ChangeLog.txt" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/ChangeLog.txt" );
            CopyFile2( srcFolder, dstFolder );
            
            if ( bSingleCommand ) goto endPoint;
        }
    }

	// Copy files to shared folder
	if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Copy files to shared folder", indexCheck );
        else
        {
            char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];

			// copy HTML5 files
            Message( "Copying HTML5 files" );
            strcpy( srcFolder, szBuildFolder ); strcat( srcFolder, "/AppGameKitStudio/media/data/html5" ); 
			strcpy( dstFolder, szSharedFolder ); strcat( dstFolder, "/Studio/HTML5" );
            DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );
			
			// copy interpreter
            Message( "Copying interpreter" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/apps/interpreter_linux/build/LinuxPlayer64" );
			strcpy( dstFolder, szSharedFolder ); strcat( dstFolder, "/Studio/LinuxPlayer64" );
            CopyFile2( srcFolder, dstFolder );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	if ( bListCommands ) 
	{
		bListCommands = false;
		goto startPoint;
	}
	
endPoint:
	Message( "Finished" );
	return 0;
}
