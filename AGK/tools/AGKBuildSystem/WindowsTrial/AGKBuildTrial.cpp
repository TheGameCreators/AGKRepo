#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <wininet.h>

#include "..\\Windows\\Common.h"

int main( int argc, char* argv[] )
{
	// set some path variables
	const char* szVS2015 = "C:\\Programs\\Visual Studio 14.0\\Common7\\IDE\\devenv.exe";
	//const char* szVS2017 = "E:\\Programs\\Visual Studio 2017\\Common7\\IDE\\devenv.exe";
	const char* szDstFolderWinTrial = "C:\\TGC\\AGKStudioBuild\\AGKStudioWindowsTrial";
	const char* szSharedFolder = "E:\\Receive";
	const char* szTortoiseSVN = "C:\\Programs\\TortoiseSVN\\bin\\TortoiseProc.exe";
	const char* szTemp = "E:\\Temp";

	SetCurrentDirectoryWithCheck( "..\\..\\..\\.." ); // AGKTrunk

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

	if ( bListCommands ) Message( "Note: Help and command list must be done by the full build first!" );

	// Update SVN
	if ( !bListCommands )
	{
		int status = 0;
		status = RunCmd( indexCheck, szTortoiseSVN, "/command:update /path:\".\" /closeonend:3" );
		if ( status != 0 ) Error( "Failed" );
		else Message( "  Success" );
	}

	// VS2015 (full version is for the broadcaster)
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile VS2015 Release (for broadcaster)", indexCheck );
		else
		{
			int status = 0;
			status = RunCmd( indexCheck, szVS2015, "AGKWindows2015.sln /rebuild Release" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// VS2015 Free
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile VS2015 ReleaseFree", indexCheck );
		else
		{
			int status = 0;
			status = RunCmd( indexCheck, szVS2015, "AGKWindows2015.sln /rebuild ReleaseFree" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// VS2015 Free 64-bit
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile VS2015 64-bit ReleaseFree", indexCheck );
		else
		{
			int status = 0;
			status = RunCmd( indexCheck, szVS2015, "AGKWindows2015-64.sln /rebuild ReleaseFree" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Compiler
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile AGK Compiler Free", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "CompilerNew" );
			int status = 0;
			status = RunCmd( indexCheck, szVS2015, "AGKCompiler2.sln /rebuild ReleaseFree" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( ".." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Broadcaster (for debugging)
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile AGK Broadcaster (for debugging)", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "Broadcaster\\AGKBroadcaster" );
			int status = 0;
			status = RunCmd( indexCheck, szVS2015, "AGKBroadcaster.sln /rebuild Release" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// interpreter
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile Windows interpreter ReleaseFree", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "apps\\interpreter" );
			int status = 0;
			status = RunCmd( indexCheck, szVS2015, "interpreter.sln /rebuild \"ReleaseFree|Win32\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// interpreter 64-bit
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile Windows interpreter 64-bit ReleaseFree", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "apps\\interpreter" );
			int status = 0;
			status = RunCmd( indexCheck, szVS2015, "interpreter.sln /rebuild \"ReleaseFree|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// IDE
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build IDE", indexCheck );
		else
		{
			Message( "Building IDE" );
			SetCurrentDirectoryWithCheck( "IDE\\Geany-1.24.1" );

			// edit geany.h
			Message( "  Editing header" );
			char *data = 0;
			int size = GetFileContents( "src\\geany.h", &data );
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

				FILE *fp = fopen( "src\\geany.h", "wb" );
				if ( !fp ) Error( "Failed to open geany.h for writing" );
				fwrite( data, 1, size, fp );
				fclose( fp );
			}
			
			Message( "  Compiling" );
			int status = RunCmd( indexCheck, "make", "TRIAL=1 -f makefile.win32" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			Message( "Building IDE - install" );
			status = RunCmd( indexCheck, "make", "install -f makefile.win32" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			Message( "Editing header" );
			Message( "  Commenting out free version define" );
			define[ 0 ] = '/';
			define[ 1 ] = '/';

			FILE *fp = fopen( "src\\geany.h", "wb" );
			if ( !fp ) Error( "  Failed to open geany.h for writing" );
			fwrite( data, 1, size, fp );
			fclose( fp );

			delete [] data;

			SetCurrentDirectoryWithCheck( "..\\.." );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// copy to build folders
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Copy files to build folders", indexCheck );
		else
		{
			char srcFolder[ 1024 ];
			char dstFolder[ 1024 ];
			char backupFolder[ 1024 ];
			FileRecord files;

			Message( "Copying to Windows Trial build" );
			Message( "  Copying Tier 2 files" );

			// update apps folder
			Message( "    Copying apps folder" );
			GetCurrentDirectory( 1024, srcFolder ); strcat( srcFolder, "\\apps" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 2\\apps" );
			// make a copy of the files in case we ever lose the folder structure
			//files.Reset(); RecordFiles( dstFolder, &files ); strcpy( backupFolder, "tools\\AGKBuildSystem\\Backup\\WindowsTrial\\appFiles.txt" ); files.Save( backupFolder );
			UpdateFolder( srcFolder, dstFolder );
			
			// update common folder
			Message( "    Copying common folder" );
			GetCurrentDirectory( 1024, srcFolder ); strcat( srcFolder, "\\common\\include" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 2\\common\\include" );
			//files.Reset(); RecordFiles( dstFolder, &files ); sprintf( backupFolder, "tools\\AGKBuildSystem\\Backup\\%s\\commonIncludeFiles.txt", szBuildPlatform[b] ); files.Save( backupFolder );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			GetCurrentDirectory( 1024, srcFolder ); strcat( srcFolder, "\\common\\Collision" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 2\\common\\Collision" );
			//files.Reset(); RecordFiles( dstFolder, &files ); sprintf( backupFolder, "tools\\AGKBuildSystem\\Backup\\%s\\commonCollisionFiles.txt", szBuildPlatform[b] ); files.Save( backupFolder );
			DeleteFolder( dstFolder );
			const char *szIgnore[] = { ".cpp", ".c", ".CPP", ".C" };
			CopyFolder( srcFolder, dstFolder, 4, szIgnore );

			// update bullet folder
			Message( "    Copying bullet folder" );
			GetCurrentDirectory( 1024, srcFolder ); strcat( srcFolder, "\\bullet" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 2\\bullet" );
			//files.Reset(); RecordFiles( dstFolder, &files ); sprintf( backupFolder, "tools\\AGKBuildSystem\\Backup\\%s\\bulletFiles.txt", szBuildPlatform[b] ); files.Save( backupFolder );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder, 4, szIgnore );

			// update platform folder
			Message( "    Copying platform folder" );
			GetCurrentDirectory( 1024, srcFolder ); strcat( srcFolder, "\\platform" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 2\\platform" );
			//files.Reset(); RecordFiles( dstFolder, &files ); strcpy( backupFolder, "tools\\AGKBuildSystem\\Backup\\WindowsTrial\\platformFiles.txt" ); files.Save( backupFolder );
			UpdateFolder( srcFolder, dstFolder );

			Message( "    Copying free libs" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 2\\platform\\windows\\Lib\\VS2015\\Release\\AGKWindows.lib" );
			CopyFile2( "platform\\windows\\Lib\\VS2015\\Free\\AGKWindows.lib", dstFolder );			
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 2\\platform\\windows\\Lib\\VS2015\\Release64\\AGKWindows.lib" );
			CopyFile2( "platform\\windows\\Lib\\VS2015\\Free64\\AGKWindows64.lib", dstFolder );

			// copy changelog
			Message( "  Copying other files" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\ChangeLog.txt" );
			CopyFile2( "AGK.txt", dstFolder );

			// copy compiler
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Compiler\\AGKCompiler.exe" );
			CopyFile2( "CompilerNew\\Final\\AGKCompiler.exe", dstFolder );

			// copy commandlist.dat
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Compiler\\CommandList.dat" );
			CopyFile2( "CompilerNew\\CommandList.dat", dstFolder );

			// copy plugins
			GetCurrentDirectory( 1024, srcFolder ); strcat( srcFolder, "\\plugins\\Plugins" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Compiler\\Plugins" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			// copy broadcaster
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Compiler\\AGKBroadcaster.exe" );
			CopyFile2( "Broadcaster\\AGKBroadcaster\\Release\\AGKBroadcaster.exe", dstFolder );

			// copy interpreter
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Compiler\\interpreters\\Windows.exe" );
			CopyFile2( "apps\\interpreter\\Final\\Windows.exe", dstFolder );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Compiler\\interpreters\\Windows64.exe" );
			CopyFile2( "apps\\interpreter\\Final\\Windows64.exe", dstFolder );

			// copy image joiner
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Utilities\\ImageJoiner\\ImageJoiner.exe" );
			CopyFile2( "tools\\ImageJoiner.exe", dstFolder );

			Message( "  Copying IDE" );
			GetCurrentDirectory( 1024, srcFolder ); strcat( srcFolder, "\\IDE\\Geany_Compiled" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Editor" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			// delete unnecessary IDE folders
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Editor\\data\\android" );
			DeleteFolder( dstFolder );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Editor\\data\\ios" );
			DeleteFolder( dstFolder );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Tier 1\\Editor\\data\\html5" );
			DeleteFolder( dstFolder );

			// copying example projects
			Message( "  Copying Example Projects" );
			GetCurrentDirectory( 1024, srcFolder ); strcat( srcFolder, "\\Examples" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\AGK\\Projects" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	if ( bListCommands ) 
	{
		bListCommands = false;
		goto startPoint;
	}

	// Commit SVN
	int status = 0;
	Message( "Comitting SVN" );
	status = RunCmd( indexCheck, szTortoiseSVN, "/command:commit /path:\".\"" );
	
endPoint:
	system("pause");
	return 0;
}
