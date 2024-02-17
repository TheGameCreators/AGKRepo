#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "Common.h"
#include "Paths.h" // user specific file, not on SVN

int main( int argc, char* argv[] )
{
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
         // no longer works after OS upgrade
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
		if ( bListCommands ) Message1( "%d: Build Mac lib", indexCheck );
		else
		{
			Message( "Building Mac lib" );
			int status = RunCmd( indexCheck, "xcodebuild", "-project AGKMac.xcodeproj -target AGKMac -configuration Release -jobs 4" );
			if ( status != 0 ) Error( "  Failed" );
			else Message( "  Success" );

			if ( bSingleCommand ) goto endPoint;
		}
	}

    // Build iOS Release Device
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build iOS Release Device lib", indexCheck );
        else
        {
            Message( "Building iOS Release Device lib" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project AGKiOS.xcodeproj -target AGKiOS -configuration Release -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            if ( bSingleCommand ) goto endPoint;
        }
    }
    /*
    // Build iOS Release Sim
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build iOS Release Sim lib", indexCheck );
        else
        {
            Message( "Building iOS Release Sim lib" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project AGKiOS.xcodeproj -target AGKiOS -configuration Release -sdk iphonesimulator14.2 -arch i386 -arch x86_64 ONLY_ACTIVE_ARCH=NO \"VALID_ARCHS=i386 x86_64\" -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            if ( bSingleCommand ) goto endPoint;
        }
    }
    */
    // Build iOS ReleaseLite Device
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build iOS ReleaseLite Device lib", indexCheck );
        else
        {
            Message( "Building iOS ReleaseLite Device lib" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project AGKiOS.xcodeproj -target AGKiOS -configuration ReleaseLite -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            if ( bSingleCommand ) goto endPoint;
        }
    }
    /*
    // Build iOS ReleaseLite Sim
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build iOS ReleaseLite Sim lib", indexCheck );
        else
        {
            Message( "Building iOS ReleaseLite Sim lib" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project AGKiOS.xcodeproj -target AGKiOS -configuration ReleaseLite -sdk iphonesimulator14.2 -arch i386 -arch x86_64 ONLY_ACTIVE_ARCH=NO \"VALID_ARCHS=i386 x86_64\" -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            if ( bSingleCommand ) goto endPoint;
        }
    }
    */
    // Build/Archive iOS interpreter_ios project
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build/Archive iOS interpreter", indexCheck );
        else
        {
            Message( "Building and Archiving iOS interpreter" );
            SetCurrentDirectoryWithCheck( "apps/interpreter_ios" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project agkinterpreter.xcodeproj -scheme agk_interpreter -destination \"generic/platform=iOS\" -configuration Release archive -archivePath \"Build/Release/AppGameKit Player.xcarchive\" -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            // strip debug symbols
            Message( "Copying binary to IDE" );
            status = RunCmd( indexCheck, "cp", "\"Build/Release/AppGameKit Player.xcarchive/Products/Applications/AppGameKit Player.app/AppGameKit Player\" \"../../AgkIde/media/data/ios/source/AppGameKit Player.app/AppGameKit Player\"" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk

            if ( bSingleCommand ) goto endPoint;
        }
    }

    // Export iOS interpreter IPA
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Export iOS interpreter IPA", indexCheck );
        else
        {
            Message( "Exporting iOS interpreter IPA" );
            SetCurrentDirectoryWithCheck( "apps/interpreter_ios" );
            int status = RunCmd( indexCheck, "xcodebuild", "-exportArchive -archivePath \"Build/Release/AppGameKit Player.xcarchive\" -exportOptionsPlist \"exportOptions.txt\" -exportPath ~/Desktop/AGKPlayeriOS" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk

            if ( bSingleCommand ) goto endPoint;
        }
    }

    // Build/Archive iOS interpreter_ios_noads project
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build/Archive iOS No Ads interpreter", indexCheck );
        else
        {
            Message( "Building and Archiving iOS No Ads interpreter" );
            SetCurrentDirectoryWithCheck( "apps/interpreter_ios_noads" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project agkinterpreter.xcodeproj -scheme agk_interpreter -destination \"generic/platform=iOS\" -configuration Release archive -archivePath \"Build/Release/AppGameKit Player.xcarchive\" -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            // strip debug symbols
            Message( "Copying binary to IDE" );
            status = RunCmd( indexCheck, "cp", "\"Build/Release/AppGameKit Player.xcarchive/Products/Applications/AppGameKit Player.app/AppGameKit Player\" \"../../AgkIde/media/data/ios/source/AppGameKit Player No Ads\"" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk

            if ( bSingleCommand ) goto endPoint;
        }
    }

    // Build/Archive Mac interpreter_mac project
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build/Archive Mac interpreter", indexCheck );
        else
        {
            Message( "Building and Archiving Mac interpreter" );
            SetCurrentDirectoryWithCheck( "apps/interpreter_mac" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project agkinterpreter.xcodeproj -scheme agk_interpreter -configuration Release archive -archivePath \"Build/Release/AppGameKit Player.xcarchive\" -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk

            if ( bSingleCommand ) goto endPoint;
        }
    }

    // Export/Copy Mac interpreter_mac project
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Export/Copy Mac interpreter to shared folder", indexCheck );
        else
        {
            // copy Mac interpreter to desktop
            SetCurrentDirectoryWithCheck( "apps/interpreter_mac" );
            Message( "Copying Mac Player to Desktop" );
            char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/Build/Release/AppGameKit Player.xcarchive/Products/Users/" USERNAME "/Applications/AppGameKit Player.app" );
            strcpy( dstFolder, "/Users/" USERNAME "/Desktop/MacPlayer-Studio/AppGameKit Player.app" );
            DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

            Message( "Making binary executable" );
            int status = system("chmod 0755 \"/Users/" USERNAME "/Desktop/MacPlayer-Studio/AppGameKit Player.app/Contents/MacOS/AppGameKit Player\"");
            if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
            else Message( "Success" );

            Message( "Zipping up Mac Player" );
            getcwd( srcFolder, 1024 );
            SetCurrentDirectoryWithCheck( "/Users/" USERNAME "/Desktop/MacPlayer-Studio" );
            status = system("rm -f \"AppGameKit Player.app.zip\"");
            if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
            else Message( "Success" );

            status = system("zip -r \"AppGameKit Player.app.zip\" \"AppGameKit Player.app\"");
            if ( WEXITSTATUS(status) != 0 ) Error( "Failed" );
            else Message( "Success" );
            SetCurrentDirectoryWithCheck( srcFolder );

            Message( "Copying Mac Player to shared folder" );
            strcpy( dstFolder, szSharedFolder ); strcat( dstFolder, "/Studio/AppGameKit Player.app.zip" );
            CopyFile2( "/Users/" USERNAME "/Desktop/MacPlayer-Studio/AppGameKit Player.app.zip", dstFolder );

            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk

            if ( bSingleCommand ) goto endPoint;

            Message( "\n** Windows build can be completed now **\n" );
            usleep( 2000 );
        }
    }

    // Build Mac Compiler
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build Mac Compiler", indexCheck );
        else
        {
            Message( "Building Mac Compiler" );
            SetCurrentDirectoryWithCheck( "CompilerNew" );
            int status = RunCmd( indexCheck, "xcodebuild", "-project AGKCompiler.xcodeproj -scheme AGKCompiler -configuration ReleaseStaticIDE -jobs 4" );
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
            int status = RunCmd( indexCheck, "xcodebuild", "-project AGKBroadcaster.xcodeproj -scheme AGKBroadcaster -configuration ReleaseStaticIDE -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

            SetCurrentDirectoryWithCheck( "../.." ); // AGKTrunk

            if ( bSingleCommand ) goto endPoint;
        }
    }

    // remove old build
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Delete old build", indexCheck );
        else
        {
            Message( "Deleting old AppGameKitStudio.app" );
            char srcFolder[ 1024 ]; getcwd( srcFolder, 1024 );
            char dstFolder[ 1024 ];
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app" );
            if ( chdir( dstFolder) == 0 )
            {
                DeleteFolder( dstFolder );
                rmdir( dstFolder );
            }
            SetCurrentDirectoryWithCheck( srcFolder );

            if ( bSingleCommand ) goto endPoint;
        }
    }

    // Build IDE
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Build Mac IDE", indexCheck );
        else
        {
            Message( "Building Mac IDE" );
            SetCurrentDirectoryWithCheck( "AgkIde" );

            int status = RunCmd( indexCheck, "xcodebuild", "-project agkinterpreter.xcodeproj -scheme agk_interpreter -configuration Release -jobs 4" );
            if ( status != 0 ) Error( "  Failed" );
            else Message( "  Success" );

			// copy to build folder
			char srcFolder[ 1024 ];
            char dstFolder[ 1024 ];
			getcwd( srcFolder, 1024 ); strcat( srcFolder, "/build/Release/AgkStudio.app" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app" );
            DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

            SetCurrentDirectoryWithCheck( ".." ); // AGKTrunk

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

			// delete android files, not currently supported by Apple
			Message( "Deleting Android files" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app/Contents/Resources/media/data/android" );
            DeleteFolder( dstFolder );

			// copy HTML5 files
            Message( "Copying HTML5 files" );
            strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/HTML5" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app/Contents/Resources/media/data/html5" );
            DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy help
            Message( "Copying help files" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Tier 1/Help" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app/Contents/Resources/media/Help" );
            CopyFolder( srcFolder, dstFolder );

			// copy SnapChat SDK to IDE (must copy from the exported cut down versions that don't have x86_64 architecture)
            Message( "Copying SnapChat Core SDK to IDE" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/apps/interpreter_ios/build/Release/AppGameKit Player.xcarchive/Products/Applications/AppGameKit Player.app/Frameworks/SCSDKCoreKit.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app/Contents/Resources/media/data/ios/source/AppGameKit Player.app/Frameworks/SCSDKCoreKit.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			Message( "Copying SnapChat Creative SDK to IDE" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/apps/interpreter_ios/build/Release/AppGameKit Player.xcarchive/Products/Applications/AppGameKit Player.app/Frameworks/SCSDKCreativeKit.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app/Contents/Resources/media/data/ios/source/AppGameKit Player.app/Frameworks/SCSDKCreativeKit.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			// copy interpreter
            Message( "Copying interpreter" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/apps/interpreter_mac/Build/Release/AppGameKit Player.xcarchive/Products/Users/" USERNAME "/Applications/AppGameKit Player.app" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app/Contents/Resources/media/interpreters/Mac.app" );
			mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			// copy projects
            Message( "Copying projects" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/Examples" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/AppGameKitStudio.app/Contents/Resources/Projects" );
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

            // copy players
			Message( "Copying Players" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Players" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Players" );
			mkdir( dstFolder, 0755 );
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy new Mac player
			Message( "Copying new Mac player" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Players/Mac/AppGameKit Player.app.zip" );
            CopyFile2( "/Users/" USERNAME "/Desktop/MacPlayer-Studio/AppGameKit Player.app.zip", dstFolder );

			/*
			// copy interpreter
            Message( "Copying interpreter" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/apps/interpreter_mac/Build/Release/AppGameKit Player.xcarchive/Products/Users/paultgc/Applications/AppGameKit Player.app" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Interpreter/Mac.app" );
			mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );
			*/

			// copy android libs
            Message( "Copying Android libs" );
            strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/Android/lib" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AndroidExport/lib" );
            DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

            // copy android source folders
			Message( "Copying Android sourceAmazon" );
			strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/Android/sourceAmazon" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AndroidExport/sourceAmazon" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			Message( "Copying Android sourceGoogle" );
			Message ( srcFolder );
            Message ( szBuildFolder );
			strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/Android/sourceGoogle" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AndroidExport/sourceGoogle" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			Message( "Copying Android sourceOuya" );
			strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "/Studio/Android/sourceOuya" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AndroidExport/sourceOuya" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );


			// copy example plugin
			Message( "Copying example plugin" );
			strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Example Plugin" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Example Plugin" );
			mkdir( dstFolder, 0755 );
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy plugins
			Message( "Copying Plugins" );
			getcwd( srcFolder, 1024 ); strcat( srcFolder, "/plugins/Plugins" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Plugins" );
			mkdir( dstFolder, 0755 );
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy Tier 2 files
			Message( "Copying Tier 2" );
			strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/Tier 2" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2" );
			mkdir( dstFolder, 0755 );
			DeleteFolder( dstFolder );
            CopyFolder( srcFolder, dstFolder );

			// copy Mac lib
            Message( "Copying Mac lib" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/mac/Lib/Release/libAGKMac.a" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/mac/Lib/Release/libAGKMac.a" );
            CopyFile2( srcFolder, dstFolder );

            // copy iOS libs
            Message( "Copying iOS libs" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Lib/Release/libAGKiOS.a" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Lib/Release/libAGKiOS.a" );
            CopyFile2( srcFolder, dstFolder );

            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Lib/ReleaseLite/libAGKiOS.a" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Lib/ReleaseLite/libAGKiOS.a" );
            CopyFile2( srcFolder, dstFolder );

			// copy MoltenVK libs
            Message( "Copying MoltenVK libs" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/renderer/MoltenVK/Lib/libMoltenVKiOS.a" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/renderer/MoltenVK" );
			mkdir( dstFolder, 0777 );
			strcat( dstFolder, "/Lib" );
			mkdir( dstFolder, 0777 );
			strcat( dstFolder, "/libMoltenVKiOS.a" );
            CopyFile2( srcFolder, dstFolder );

			getcwd( srcFolder, 1024 ); strcat( srcFolder, "/renderer/MoltenVK/Lib/libMoltenVKMacOS.a" );
			strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/renderer/MoltenVK/Lib/libMoltenVKMacOS.a" );
			CopyFile2( srcFolder, dstFolder );

			// copy Chartboost SDK
            Message( "Copying Chartboost SDK" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/Chartboost.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/Chartboost.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

            // copy AdMob SDK
            Message( "Copying AdMob SDK" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/GoogleMobileAds.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/GoogleMobileAds.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			Message( "Copying AdMob SDK2" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/GoogleAppMeasurement.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/GoogleAppMeasurement.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			Message( "Copying AdMob SDK3" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/GoogleUtilities.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/GoogleUtilities.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			Message( "Copying AdMob SDK4" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/nanopb.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/nanopb.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			Message( "Copying UMP Consent SDK" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/UserMessagingPlatform.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/UserMessagingPlatform.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

            // Copy SnapChat SDK
			Message( "Copying SnapChat Core SDK" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/SCSDKCoreKit.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/SCSDKCoreKit.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			Message( "Copying SnapChat Creative SDK" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/SCSDKCreativeKit.framework" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/SCSDKCreativeKit.framework" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

            // copy Firebase SDK
            Message( "Copying Firebase SDK" );
            getcwd( srcFolder, 1024 ); strcat( srcFolder, "/platform/apple/Source/Social Plugins/Firebase" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/Tier 2/platform/apple/Source/Social Plugins/Firebase" );
            DeleteFolder( dstFolder );
            mkdir( dstFolder, 0755 );
            CopyFolder( srcFolder, dstFolder );

			// change log
			Message( "Copying change log" );
            strcpy( srcFolder, szMacBuildFiles ); strcat( srcFolder, "/AGK/ChangeLog.txt" );
            strcpy( dstFolder, szBuildFolder ); strcat( dstFolder, "/AppGameKitStudio/ChangeLog.txt" );
            CopyFile2( srcFolder, dstFolder );

            if ( bSingleCommand ) goto endPoint;
        }
    }

    // Sign bundle
    if ( index <= ++indexCheck )
    {
        if ( bListCommands ) Message1( "%d: Sign bundle", indexCheck );
        else
        {
            Message( "Signing bundle" );
            int status = system( "/Users/" USERNAME "/AGKStudioBuild/sign.sh" );
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
