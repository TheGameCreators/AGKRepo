#include "CompilerCommon.h"
#include "AGKCompiler2.h"
#include "cFile.h"
#include "Parser1.h"
#include "Parser2.h"

#ifdef IDE_MAC
extern NSFileManager *g_pFileManager;
#endif

void AGKDisplayMessage( const char* msg );

namespace AGK_Compiler
{
	int error_count = 0;
	int max_error_count = 30;
	uString g_sAGKErrors;
	uString g_sAGKInfo;
}

#ifdef AGK_STATIC_LIB

namespace AGK_Compiler
{
	void Error( const char *msg, int lineNum, const char *includeFile )
	{
		error_count++;
		if ( error_count > max_error_count ) return;

		char szOutput[ 1024 ];
		if ( includeFile ) sprintf( szOutput, "%s:%d:Error: %s\n", includeFile, lineNum, msg );
		else sprintf( szOutput, "Error: %s\n", msg );
		g_sAGKErrors.AppendUTF8( szOutput );
	}

	void Warning( const char *msg, int lineNum, const char *includeFile )
	{
		char szOutput[ 1024 ];
		if ( includeFile ) sprintf( szOutput, "%s:%d:Warning: %s\n", includeFile, lineNum, msg );
		else sprintf( szOutput, "Warning: %s\n", msg );
		g_sAGKErrors.AppendUTF8( szOutput );
	}

	void CompilerInfo( const char *msg )
	{
		char szOutput[ 1024 ];
		sprintf( szOutput, "%s\n", msg );
		g_sAGKInfo.AppendUTF8( szOutput );
	}
}

#else

namespace AGK_Compiler
{
	void Error( const char *msg, int lineNum, const char *includeFile )
	{
		error_count++;
		if ( error_count > max_error_count ) return;

		if ( includeFile )
		{
			uString sInclude = includeFile;
			sInclude.Replace( '\\', '/' );

			printf( "%s:%d: error: %s\n", sInclude.GetStr(), lineNum, msg );
		}
		else
		{
			printf( "error: %s\n", msg );
			exit(1);
		}
	}

	void Warning( const char *msg, int lineNum, const char *includeFile )
	{
		if ( includeFile )
		{
			uString sInclude = includeFile;
			sInclude.Replace( '\\', '/' );

			printf( "%s:%d: warning: %s\n", sInclude.GetStr(), lineNum, msg );
		}
		else
		{
			printf( "warning: %s\n", msg );
		}
	}

	void CompilerInfo( const char *msg )
	{
		printf( "info: %s\n", msg );
	}
}

#endif //AGK_STATIC_LIB

int AGK_Compiler::RunCompiler( const char* lpCmdLine )
{
	ResetCompilerErrors();

	bool bCompile = true;
	bool bRun = false; // this shouldn't be done by the compiler in future
	bool b64Bit = false;
	bool bShowStats = false;
	uString sProjectPath;

	const char* ptr = lpCmdLine;
	while( *ptr )
	{
		while( *ptr == ' ' || *ptr == '\t' ) ptr++;

		if ( *ptr == '-' )
		{
			ptr++;
			if ( strncmp(ptr, "run", 3) == 0 && (*(ptr+3) == 0 || *(ptr+3) == ' ') )
			{
				bRun = true;
				ptr += 3;
			}
			else if ( strncmp(ptr, "64", 2) == 0 && (*(ptr+2) == 0 || *(ptr+2) == ' ') )
			{
				b64Bit = true;
				ptr += 2;
			}
			else if ( strncmp(ptr, "stats", 2) == 0 && (*(ptr+5) == 0 || *(ptr+5) == ' ') )
			{
				bShowStats = true;
				ptr += 5;
			}
		}
		else
		{
			char stopChar = ' ';
			if ( *ptr == '"' ) 
			{
				ptr++;
				stopChar = '"';
			}

			const char* start = ptr;
			while( *ptr && *ptr != stopChar )
			{
				ptr++;
			}

			if ( sProjectPath.GetLength() == 0 )
			{
				int length = (int) (ptr - start);
				sProjectPath.SetStrN( start, length );
			}

			ptr++;
		}
	}

	cFile::Init( sProjectPath );
    
    //printf( "%s\n", cFile::GetWritePath() );

	// get project name from path
	uString sProjectName;
	cFile::GetProjectName( sProjectName );
#ifdef _WINDOWS
	sProjectName.Append( ".exe" );
#elif IDE_MAC
    sProjectName.Append( ".app" );
#endif

	if ( bCompile )
	{
		Parser1::Parser1Init();
		Parser1 parser1;
		parser1.SetIncludeMode( INCLUDE_NORMAL );

		uString error;
		if ( !cProgramData::LoadCommandsArrayGlobal( error ) )
		{
			AGKDisplayMessage( error.GetStr() );
			return 1;
		}
		
		cProgramData programData;

		void* start = 0;
		if ( bShowStats ) start = StartTimer();

		programData.AddIncludeFile( "main.agc", 0, 0 );
		//parser1.ParseFile( g_pIncludeFiles.m_pData[ iFileCount ], 0, &programData );

		int iFileCount = 0;
		while ( iFileCount < programData.m_pIncludeFiles.m_iCount )
		{
			//sprintf(outputStr,"Parsing File: %s\n", g_pIncludeFiles.m_pData[ iFileCount ]);
			//OutputDebugString(outputStr);

			// don't parse inserted files (type 2)
			if ( programData.m_pIncludeFiles.m_pData[ iFileCount ]->type < 2 )
			{
				int result = parser1.ParseFile( programData.m_pIncludeFiles.m_pData[ iFileCount ]->sNameOrig.GetStr(), iFileCount, &programData );
				if ( result == 0 )
				{
					if ( error_count > max_error_count ) return 1;
				}
			}
			
			++iFileCount;
		}

		// don't move on to step 2 if there were errors
		if ( error_count > 0 ) return 1;

		//programData.AddToken( AGK_TOKEN_PROGRAM_END, "", "", 0, 0 );

		Parser2 parser2;
		int result = parser2.FirstPass( &programData );
		if ( result <= 0 ) 
		{
			if ( error_count > max_error_count ) return 1;
		}

		result = parser2.ParseFunctions( &programData );
		if ( result <= 0 ) 
		{
			if ( error_count > max_error_count ) return 1;
		}

		programData.m_iEntryPoint = programData.GetInstructionPosition() + 1;

		int consumed = 0;
		int tokenID = 0;
		while ( tokenID < (int) programData.m_iNumTokens )
		{
			consumed = 0;
			int result = parser2.ParseStatement( 0, &programData, tokenID, consumed );
			if ( result <= 0 ) 
			{
				if ( error_count > max_error_count ) return 1;
				else
				{
					while( programData.GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) programData.m_iNumTokens )
					{
						tokenID++;
					}
				}
			}
			else
			{
				tokenID += consumed;
			}
		}

		// don't continue if there were errors
		if ( error_count > 0 ) return 1;

		// check variables are all initialised somewhere
		result = programData.FinalCheck();
		if ( result <= 0 ) 
		{
			if ( error_count > max_error_count ) return 1;
		}

		// don't continue if there were errors
		if ( error_count > 0 ) return 1;

		result = programData.BuildBytecodeFile( "media/bytecode.byc", 1 );
		if ( result <= 0 ) 
		{
			return 1;
		}

		if ( bShowStats )
		{
			float timeTaken = GetTime( start );
			char msg[ 1024 ];
			sprintf( msg, "Parse Time: %.2fms, Total Lines: %d, Total Tokens: %d", timeTaken*1000, programData.iLineCountTotal, programData.m_iNumTokens );
			CompilerInfo( msg );
		}

		//sprintf(outputStr,"Time Taken: %.4f, Lines: %d\n", timeTaken, iLineCountTotal);
		//OutputDebugString(outputStr);
		//MessageBox( NULL, outputStr, "Info", 0 );

// Mac compiler won't know where the plugins are if it is sandboxed
#ifndef IDE_MAC

		const char* szSrcPluginsFolder;
#if defined(AGK_STATIC_LIB_IDE)
		szSrcPluginsFolder = "Plugins";
#elif defined(AGK_STATIC_LIB)
		szSrcPluginsFolder = "../Plugins";
#else
		szSrcPluginsFolder = "../../Plugins";
#endif
        const char* szDstPluginsFolder = "Plugins";

		// delete existing plugins
		cFile::DeleteFolder( szDstPluginsFolder );

		// copy plugins
		for( int i = 0; i < programData.m_pPlugins.m_iCount; i++ )
		{
			uString srcPluginPath, dstPluginPath;
			srcPluginPath.Format( "%s/%s/Windows.dll", szSrcPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
            dstPluginPath.Format( "%s/%s/Windows.dll", szDstPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
			if ( cFile::ExistsRead( srcPluginPath ) ) cFile::CopyFile( srcPluginPath, dstPluginPath );
            
			srcPluginPath.Format( "%s/%s/Windows64.dll", szSrcPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
            dstPluginPath.Format( "%s/%s/Windows64.dll", szDstPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
			if ( cFile::ExistsRead( srcPluginPath ) ) cFile::CopyFile( srcPluginPath, dstPluginPath );
            
			srcPluginPath.Format( "%s/%s/Linux64.so", szSrcPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
            dstPluginPath.Format( "%s/%s/Linux64.so", szDstPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
			if ( cFile::ExistsRead( srcPluginPath ) ) cFile::CopyFile( srcPluginPath, dstPluginPath );
            
			srcPluginPath.Format( "%s/%s/Linux32.so", szSrcPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
            dstPluginPath.Format( "%s/%s/Linux32.so", szDstPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
			if ( cFile::ExistsRead( srcPluginPath ) ) cFile::CopyFile( srcPluginPath, dstPluginPath );
            
			srcPluginPath.Format( "%s/%s/Mac64.dylib", szSrcPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
            dstPluginPath.Format( "%s/%s/Mac64.dylib", szDstPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
			if ( cFile::ExistsRead( srcPluginPath ) ) cFile::CopyFile( srcPluginPath, dstPluginPath );
            
			srcPluginPath.Format( "%s/%s/Android.so", szSrcPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
            dstPluginPath.Format( "%s/%s/Android.so", szDstPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
			if ( cFile::ExistsRead( srcPluginPath ) ) cFile::CopyFile( srcPluginPath, dstPluginPath );
            
			srcPluginPath.Format( "%s/%s/iOS.so", szSrcPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
            dstPluginPath.Format( "%s/%s/iOS.so", szDstPluginsFolder, programData.m_pPlugins.m_pData[ i ]->sName.GetStr() );
			if ( cFile::ExistsRead( srcPluginPath ) ) cFile::CopyFile( srcPluginPath, dstPluginPath );
		}

#endif

#ifndef AGK_STATIC_LIB
	#ifdef _WINDOWS
		const char* WindowsEXE = "../interpreters/Windows.exe";
		if ( b64Bit ) WindowsEXE = "../interpreters/Windows64.exe";
		if ( !cFile::CopyFile( WindowsEXE, sProjectName ) )
	#elif IDE_MAC
        if ( !cFile::CopyFile( "../interpreters/Mac.app", sProjectName ) )
	#elif IDE_LINUX
	#ifdef __x86_64__
		if ( !cFile::CopyFile( "../interpreters/LinuxPlayer64", sProjectName ) )
	#else
		if ( !cFile::CopyFile( "../interpreters/LinuxPlayer32", sProjectName ) )
	#endif
	#elif IDE_PI
		if ( !cFile::CopyFile( "../interpreters/PiPlayer", sProjectName ) )
	#else 
		if( 0 )
	#endif
		{
			return 1;
		}

	#ifdef IDE_MAC
        uString sFromPath( cFile::GetWritePath() );
        uString sToPath( cFile::GetWritePath() );
        
        sFromPath.Append( "media" );
        sToPath.Append( sProjectName );
        sToPath.Append( "/Contents/Resources/media" );
        
        //printf( "%s\n", sFromPath.GetStr() );
        //printf( "%s\n", sToPath.GetStr() );
        
        NSString* pFromMedia = [ [ NSString alloc ] initWithUTF8String:sFromPath.GetStr() ];
        NSString* pToMedia = [ [ NSString alloc ] initWithUTF8String:sToPath.GetStr() ];
        
        NSError *fileerror;
        [ g_pFileManager removeItemAtPath:pToMedia error:nil ];
        if ( ![ g_pFileManager copyItemAtPath:pFromMedia toPath:pToMedia error:&fileerror ] )
        {
            printf( "%s", [[ fileerror localizedDescription] UTF8String] );
        }
        
        [ pFromMedia release ];
        [ pToMedia release ];
        
		// unsign the app as it will become invalid when media is added
		uString sParameter = sProjectName;
		sParameter.Append("/Contents/MacOS/AppGameKit Player");

		// codesign appears to have a bug in 10.13 or below
		NSOperatingSystemVersion version = [[NSProcessInfo processInfo] operatingSystemVersion];
		if ( version.majorVersion > 10 || (version.majorVersion == 10 && version.minorVersion >= 14) )
		{	
			char **vparams = new char*[ 4 ];
			vparams[ 0 ] = new char[20]; strcpy( vparams[0], "/usr/bin/codesign" );
			vparams[ 1 ] = new char[20]; strcpy( vparams[1], "--remove-signature" );
			vparams[ 2 ] = new char[sParameter.GetLength()+1]; strcpy( vparams[2], sParameter.GetStr() );
			vparams[ 3 ] = 0;
    
			// create child process
			pid_t pid = fork();
			if ( pid < 0 ) pid = 0; // failed
			else if ( pid == 0 ) // child
			{
				execvp( "/usr/bin/codesign", vparams ); 
				_exit(127); // failed to start child process, don't call exit() without the underscore
			}
			else // parent
			{
				for( int i = 0; i < 3; i++ ) delete [] vparams[i];
				delete [] vparams;
			}
					
			if (pid == 0) {
				printf( "Failed to remove interpreter signature, the app may fail to run" );
			}
			else
			{
				int result = 1;
				int count = 0;
				while( result && count < 5000 )
				{
					result = kill( pid, 0 ); // doesn't actually kill the process, just checks it
					if ( result == 0 ) 
					{
						// child exists, check if it has stopped
						int status;
						int result2 = waitpid( pid, &status, WNOHANG );
						if ( result2 == pid ) result = 0;
						else if ( result2 == 0 ) result = 1;
						else result = 0;
					}
					else result = 0;
                
					usleep( 1000 );
					count++;
				}
            
				if ( count >= 5000 )
				{
					printf( "signature removal timed out, the app may fail to run" );
				}
			}

			// wait just in case the above finished too soon, one report of a possible race condition
			usleep( 200 * 1000 );


			// adhoc sign it for arm64 requirement
			sParameter = sProjectName;
			vparams = new char*[ 6 ];
			vparams[ 0 ] = new char[20]; strcpy( vparams[0], "/usr/bin/codesign" );
			vparams[ 1 ] = new char[20]; strcpy( vparams[1], "--force" );
			vparams[ 2 ] = new char[20]; strcpy( vparams[2], "-s" );
			vparams[ 3 ] = new char[20]; strcpy( vparams[3], "-" );
			vparams[ 4 ] = new char[sParameter.GetLength()+1]; strcpy( vparams[4], sParameter.GetStr() );
			vparams[ 5 ] = 0;
    
			// create child process
			pid = fork();
			if ( pid < 0 ) pid = 0; // failed
			else if ( pid == 0 ) // child
			{
				execvp( "/usr/bin/codesign", vparams ); 
				_exit(127); // failed to start child process, don't call exit() without the underscore
			}
			else // parent
			{
				for( int i = 0; i < 5; i++ ) delete [] vparams[i];
				delete [] vparams;
			}
					
			if (pid == 0) {
				printf( "Failed to adhoc sign the app, the app may fail to run" );
			}
			else
			{
				int result = 1;
				int count = 0;
				while( result && count < 5000 )
				{
					result = kill( pid, 0 ); // doesn't actually kill the process, just checks it
					if ( result == 0 ) 
					{
						// child exists, check if it has stopped
						int status;
						int result2 = waitpid( pid, &status, WNOHANG );
						if ( result2 == pid ) result = 0;
						else if ( result2 == 0 ) result = 1;
						else result = 0;
					}
					else result = 0;
                
					usleep( 1000 );
					count++;
				}
            
				if ( count >= 5000 )
				{
					printf( "app signing timed out, the app may fail to run" );
				}
			}

			// wait just in case the above finished too soon, one report of a possible race condition
			usleep( 200 * 1000 );
		}

		// Delete code signature folder
		uString sCodeSignaturePath = sProjectName;
		sCodeSignaturePath.AppendUTF8( "/Contents/_CodeSignature" );
		[g_pFileManager removeItemAtPath: [NSString stringWithUTF8String: sCodeSignaturePath.GetStr()] error: nil];
		sCodeSignaturePath = sProjectName;
		sCodeSignaturePath.AppendUTF8( "/Contents/CodeResources" );
		[g_pFileManager removeItemAtPath: [NSString stringWithUTF8String: sCodeSignaturePath.GetStr()] error: nil];
        
        // Plugins
        sFromPath.SetStr( cFile::GetWritePath() );
        sToPath.SetStr( cFile::GetWritePath() );
        
        sFromPath.Append( "Plugins" );
        sToPath.Append( sProjectName );
        sToPath.Append( "/Contents/Resources/Plugins" );
        
        pFromMedia = [ [ NSString alloc ] initWithUTF8String:sFromPath.GetStr() ];
        pToMedia = [ [ NSString alloc ] initWithUTF8String:sToPath.GetStr() ];
        
        if ( [g_pFileManager fileExistsAtPath:pFromMedia] )
        {
            [ g_pFileManager removeItemAtPath:pToMedia error:nil ];
            if ( ![ g_pFileManager copyItemAtPath:pFromMedia toPath:pToMedia error:&fileerror ] )
            {
                printf( "%s", [[ fileerror localizedDescription] UTF8String] );
            }
        }
        
        [ pFromMedia release ];
        [ pToMedia release ];
	#endif
#endif // AGK_STATIC_LIB
    }
    
#ifndef AGK_STATIC_LIB
    if ( bRun )
	{
		cFile::RunFile( sProjectName, "" );
	}
      
	if ( error_count > 0 ) return 1;
	else return 0;
#else
	return error_count;
#endif // AGK_STATIC_LIB
}

int AGK_Compiler::GetCompilerErrorCount()
{
	return error_count;
}

void AGK_Compiler::SetCompilerMaxErrors( int maxErrors )
{
	max_error_count = maxErrors;
}

void AGK_Compiler::ResetCompilerErrors()
{
	g_sAGKErrors.SetStr( "" );
	g_sAGKInfo.SetStr( "" );
	error_count = 0;
}

char* AGK_Compiler::GetCompileInfo()
{
	char *szInfo = new char[ g_sAGKInfo.GetLength()+1 ];
	strcpy( szInfo, g_sAGKInfo.GetStr() );
	return szInfo;
}

char* AGK_Compiler::GetCompileErrors()
{
	char *szErrors = new char[ g_sAGKErrors.GetLength()+1 ];
	strcpy( szErrors, g_sAGKErrors.GetStr() );
	return szErrors;
}

void AGK_Compiler::CompilerDeleteString( char* str )
{
	delete [] str;
}

int AGK_Compiler::RunCompilerString( const char* szSource )
{
	ResetCompilerErrors();
	
	Parser1::Parser1Init();
	Parser1 parser1;
	parser1.SetIncludeMode( INCLUDE_FORBIDDEN );

	uString error;
	if ( !cProgramData::LoadCommandsArrayGlobal( error ) )
	{
		AGKDisplayMessage( error.GetStr() );
		return 1;
	}
		
	cProgramData programData;

	programData.AddIncludeFile( "main.agc", 0, 0 );
	
	parser1.ParseSource( szSource, &programData );
		
	// don't move on to step 2 if there were errors
	if ( error_count > 0 ) return error_count;

	//programData.AddToken( AGK_TOKEN_PROGRAM_END, "", "", 0, 0 );

	Parser2 parser2;
	parser2.FirstPass( &programData );
	if ( error_count > max_error_count ) return error_count;

	parser2.ParseFunctions( &programData );
	if ( error_count > max_error_count ) return error_count;
	
	programData.m_iEntryPoint = programData.GetInstructionPosition() + 1;

	int consumed = 0;
	int tokenID = 0;
	while ( tokenID < (int) programData.m_iNumTokens )
	{
		consumed = 0;
		int result = parser2.ParseStatement( 0, &programData, tokenID, consumed );
		if ( result <= 0 ) 
		{
			if ( error_count > max_error_count ) 
			{
				return error_count;
			}
			else
			{
				while( programData.GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) programData.m_iNumTokens )
				{
					tokenID++;
				}
			}
		}
		else
		{
			tokenID += consumed;
		}
	}

	// don't continue if there were errors
	if ( error_count > 0 ) return error_count;

	// check variables are all initialised somewhere
	programData.FinalCheck();

    return error_count;
}
