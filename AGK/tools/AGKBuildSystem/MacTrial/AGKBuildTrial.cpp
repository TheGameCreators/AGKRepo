#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "../Mac/Common.h"

int main( int argc, char* argv[] )
{
	// set some path variables
	const char* szSharedFolder = "/Volumes/Receive";
    const char* szMacBuildFiles = "/Volumes/AGKStudioMac";
    const char* szBuildFolder = "/Users/paulj/Projects";
    
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
        /*
        Message( "Updating SVN" );
		int status = RunCmd( indexCheck, "svn", "update" );
		if ( status != 0 ) Error( "  Failed" );
		else Message( "  Success" );
         */
        
        if ( !bSingleCommand )
        {
            // check shared folder paths
            Message( "Checking shared folder paths" );
            char currPath[ 1024 ];
            getcwd( currPath, 1024 );
            SetCurrentDirectoryWithCheck( szSharedFolder );
            SetCurrentDirectoryWithCheck( szMacBuildFiles );
            SetCurrentDirectoryWithCheck( currPath );
        }
	}
	
	// Build Mac
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build Mac Trial lib", indexCheck );
		else
		{
			Message( "Building Mac Trial lib" );
			int status = RunCmd( indexCheck, "xcodebuild", "-project AGKMac.xcodeproj -target AGKMac -configuration ReleaseFree -jobs 4" );
			if ( status != 0 ) Error( "  Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
    
    // Build/Archive Mac interpreter_mac project
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build/Archive Mac Trial interpreter", indexCheck );
        else
        {
            Message( "Building and Archiving Mac Trial interpreter" );
            SetCurrentDirectoryWithCheck( "apps/interpreter_mac" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project agkinterpreter.xcodeproj -scheme agk_interpreter -configuration ReleaseFree archive -archivePath \"Build/ReleaseFree/AppGameKit Player.xcarchive\" -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );
            
            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk
            
            if ( bSingleCommand ) goto endPoint;
        }
    }
    
    // Build Mac Compiler
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build Mac Trial Compiler", indexCheck );
        else
        {
            Message( "Building Mac Trial Compiler" );
            SetCurrentDirectoryWithCheck( "CompilerNew" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project AGKCompiler.xcodeproj -scheme AGKCompiler -configuration ReleaseFree -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );
            
            SetCurrentDirectoryWithCheck( ".." ); // AGKTrunk
            
            if ( bSingleCommand ) goto endPoint;
        }
    }
    
    // Build Mac Broadcaster
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build Mac Broadcaster", indexCheck );
        else
        {
            Message( "Building Mac Broadcaster" );
            SetCurrentDirectoryWithCheck( "Broadcaster/AGKBroadcaster" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project AGKBroadcaster.xcodeproj -scheme AGKBroadcaster -configuration Release -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );
            
            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk
            
            if ( bSingleCommand ) goto endPoint;
        }
    }
    
    // Build IDE
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build Mac Trial IDE", indexCheck );
        else
        {
            Message( "Building Mac Trial IDE" );
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
            int status = RunCmd( indexCheck, "make", "-j4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );
            
            Message( "Deleting any existing Geany-Compiled folder" );
            char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];
            getcwd( srcFolder, 1024 );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled" );
            if ( chdir( dstFolder) == 0 )
            {
                DeleteFolder( dstFolder );
                rmdir( dstFolder );
            }
            SetCurrentDirectoryWithCheck( srcFolder );
            
            Message( "Creating Geany-Compiled folder" );
            status = RunCmd( indexCheck, "make", "install" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );
            
            Message( "Editing header" );
            Message( "  Commenting out free version define" );
            define[ 0 ] = '/';
            define[ 1 ] = '/';
            
            FILE *fp = fopen( "src/geany.h", "wb" );
            if ( !fp ) Error( "Failed to open geany.h for writing" );
            fwrite( data, 1, size, fp );
            fclose( fp );
            
            delete [] data;
            
            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk
            
            if ( bSingleCommand ) goto endPoint;
        }
    }
    
    // remove old build
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Delete old trial build", indexCheck );
        else
        {
            Message( "Deleting old AppGameKit.app" );
            char srcFolder[ 1024 ]; getcwd( srcFolder, 1024 );
            char dstFolder[ 1024 ];
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app" );
            if ( chdir(dstFolder) == 0 )
            {
                DeleteFolder( dstFolder );
                rmdir( dstFolder );
            }
            SetCurrentDirectoryWithCheck( srcFolder );
            
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
            
            Message( "Copying change log" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/ChangeLog.txt" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/ChangeLog.txt" );
            CopyFile2( srcFolder, dstFolder );
            
            // copy to steam folder
            Message( "Copying to Steam build" );
            strcpy( srcFolder, szBuildFolder ); strcat( srcFolder, "/AGKMacTrial" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/SteamTrial" );
            DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );
            
            if ( bSingleCommand ) goto endPoint;
        }
    }
    
    // Create app bundle
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Create app bundle", indexCheck );
        else
        {
            char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];
            
            // copy compiler
            Message( "Copying compiler" );
            strcpy( srcFolder, "CompilerNew/build/ReleaseFree/AGKCompiler" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled/share/applications/AGKCompiler" );
            CopyFile2( srcFolder, dstFolder );
            
            strcpy( srcFolder, "CompilerNew/CommandList.dat" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled/share/applications/CommandList.dat" );
            CopyFile2( srcFolder, dstFolder );
            
            // copy broadcaster
            Message( "Copying broadcaster" );
            strcpy( srcFolder, "Broadcaster/AGKBroadcaster/build/Release/AGKBroadcaster" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled/share/applications/AGKBroadcaster" );
            CopyFile2( srcFolder, dstFolder );
            
            // copy Mac interpreter
            Message( "Copying Mac interpreter" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/apps/interpreter_mac/Build/ReleaseFree/AppGameKit Player.xcarchive/Products/Users/paulj/Applications/AppGameKit Player.app" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled/share/applications/interpreters" );
            mkdir( dstFolder, 0755 );
            strcat( dstFolder, "/Mac.app" );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
            
            // copy help
            Message( "Copying help files" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Tier 1/Help" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled/share/Help" );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
            
            // delete android/ios/html5 files
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled/share/geany/android" );
            DeleteFolder( dstFolder );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled/share/geany/ios" );
            DeleteFolder( dstFolder );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled/share/geany/html5" );
            DeleteFolder( dstFolder );
            
            // bundle files
            Message( "Creating bundle" );
            int status = system( "IDE/Geany-1.24.1/bundleTrial.sh" );
            if ( WEXITSTATUS(status) != 0 ) Error( "  Failed" );
            else Message( "  Success" );
            
            // cleaning up
            Message( "Cleaning up" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/Geany-Compiled" );
            DeleteFolder( dstFolder );
            rmdir( dstFolder );
            
            if ( bSingleCommand ) goto endPoint;
        }
    }
    
    // copy internal files
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Copy internal files (those inside AppGameKit.app)", indexCheck );
        else
        {
            Message( "Copying internal files" );
            char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];
            
            // copy projects
            Message( "Copying projects" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/Examples" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Projects" );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
            
            // copy Tier 2
            Message( "Copying Tier 2 app files" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Tier 2/apps/template_mac" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Tier 2" ); mkdir( dstFolder, 0755 );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Tier 2/apps" ); mkdir( dstFolder, 0755 );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Tier 2/apps/template_mac" );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
            
            Message( "Copying Tier 2 bullet files" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Tier 2/bullet" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Tier 2/bullet" ); mkdir( dstFolder, 0755 );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
            
            Message( "Copying Tier 2 common files" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Tier 2/common" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Tier 2/common" ); mkdir( dstFolder, 0755 );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
            
            Message( "Copying Tier 2 platform files" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Tier 2/platform/mac" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Tier 2/platform" ); mkdir( dstFolder, 0755 );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Tier 2/platform/mac" );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
            
            // copy Mac lib
            Message( "Copying Mac lib" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/mac/Lib/ReleaseFree/libAGKMac.a" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AGKMacTrial/AppGameKit.app/Contents/Resources/Tier 2/platform/mac/Lib/Release/libAGKMac.a" );
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
	return 0;
}
