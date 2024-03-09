#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <wininet.h>

#include "miniz.h"
#include "Common.h"

class ResourceFile
{
	public:
		char* m_pString = 0;

		ResourceFile() {}
		~ResourceFile() { if ( m_pString ) delete [] m_pString; }

		void SetStr( const char* str )
		{
			if ( m_pString ) delete [] m_pString;
			int length = (int)strlen( str );
			m_pString = new char[ length+1 ];
			strcpy( m_pString, str );
		}
};

int CompareResourceItems( const void* a, const void* b )
{
	ResourceFile* pA = *((ResourceFile**) a);
	ResourceFile* pB = *((ResourceFile**) b);

	if ( pA->m_pString == 0 ) return -1;
	if ( pB->m_pString == 0 ) return 1;

	return strcmp( pA->m_pString, pB->m_pString );
}

template<class T> class DynArrayObject
{
	public:
		T **m_pData;
		int m_iSize;
		int m_iCount;
		int m_iSorted;
		int (*CompareItems)(const void*, const void*);

		DynArrayObject( int size=0 )
		{
			CompareItems = 0;
			m_pData = 0;
			m_iSize = 0;
			m_iCount = 0;
			m_iSorted = 0;
			if ( size > 0 )
			{
				int newSize = 1;
				while ( newSize < size ) newSize <<= 1;
				m_pData = new T*[ newSize ];
				m_iSize = newSize;
			}
		}

		~DynArrayObject()
		{
			if ( m_pData ) 
			{
				for( int i = 0; i < m_iCount; i++ )
				{
					delete m_pData[ i ];
				}
					
				delete [] m_pData;
			}
		}

		void Clear()
		{
			if ( m_pData ) 
			{
				for( int i = 0; i < m_iCount; i++ )
				{
					delete m_pData[ i ];
				}
					
				m_pData = 0;
			}
			m_iSize = 0;
			m_iCount = 0;
		}

		void SetSorted( bool flag, int (*Compare)(const void*, const void*) )
		{
			CompareItems = Compare;
			if ( Compare == 0 ) flag = false;
			m_iSorted = flag ? 1 : 0;

			if ( m_iSorted && m_iCount > 0 )
			{
				// sort existing array
				qsort( m_pData, m_iCount, sizeof(T*), Compare );
			}
		}

		int Add( T *item, bool duplicates=true )
		{
			if ( m_iCount+1 > m_iSize )
			{
				m_iSize *= 2;
				if ( m_iSize == 0 ) m_iSize = 4;
				T** newArray = new T*[m_iSize];
				for ( int i = 0; i < m_iCount; i++ )
				{
					newArray[ i ] = m_pData[ i ];
				}
				if ( m_pData ) delete [] m_pData;
				m_pData = newArray;
			}

			T *storeItem = new T();
			*storeItem = *item;

			if ( m_iSorted == 0 )
			{
				m_pData[ m_iCount++ ] = storeItem;
				return m_iCount-1;
			}
			else
			{
				int high = m_iCount-1;
				int low = 0;
					
				// binary search
				int result = 0;
				int mid = 0;
				while( high >= low )
				{
					mid = (high+low)/2;
					result = CompareItems( &(m_pData[ mid ]), &storeItem );
					if( result > 0 ) high = mid-1;
					else if ( result < 0 ) low = mid+1;
					else 
					{
						if ( duplicates ) break;
						else return -1; // not allowed dulplicates
					}
				}

				if ( result < 0 ) mid++;
				if ( mid >= m_iCount )
				{
					m_pData[ m_iCount++ ] = storeItem;
					return m_iCount-1;
				}
				else
				{
					// insert new element at mid index
					for( int i = m_iCount-1; i >= mid; i-- )
					{
						m_pData[ i+1 ] = m_pData[ i ];
					}
					m_pData[ mid ] = storeItem;
					m_iCount++;
					return mid;
				}
			}
		}

		int Find( T *item )
		{
			if ( !m_iSorted ) return -1;

			int high = m_iCount-1;
			int low = 0;
				
			// binary search
			int result = 0;
			while( high >= low )
			{
				int mid = (high+low)/2;
				result = CompareItems( &(m_pData[ mid ]), &item );
				if( result > 0 ) high = mid-1;
				else if ( result < 0 ) low = mid+1;
				else return mid;
			}

			return -1;
		}
};

DynArrayObject<ResourceFile> g_allResources;


int ExtractFile( mz_zip_archive* zip_archive, const char* file, const char* dst_path )
{
	size_t dataLength = 0;
	char* pData = (char*)mz_zip_reader_extract_file_to_heap( zip_archive, file, &dataLength, 0 );
	if ( !pData || dataLength <= 0 ) 
	{
		return 0;
	}

	CreatePath( dst_path );
				
	// write it to the build folder
	FILE *fp = fopen( dst_path, "wb" );
	if ( !fp )
	{
		free(pData);
		char err[ 1024 ];
		sprintf_s( err, "Failed to open \"%s\" for writing", dst_path );
		Error( err );
		return -1;
	}
	fwrite( pData, 1, dataLength, fp );
	fclose( fp );
	free(pData);

	return 1;
}

int ExtractFolder( mz_zip_archive* zip_archive, const char* folder, const char* base_path )
{
	int numFiles = mz_zip_reader_get_num_files( zip_archive );

	char szFilename[ 1024 ];
	char szDstPath[ 1024 ];
	char szMatch[ 1024 ];
	strcpy_s( szMatch, folder );
	char* ptr = szMatch;
	// replace back slashes with forward slashes
	while( *ptr )
	{
		if ( *ptr == '\\' ) *ptr = '/';
		ptr++;
	}
	// append a forward slash at the end
	*ptr = '/';
	*(ptr+1) = 0;

	// find files that match the folder prefix
	for( int i = 0; i < numFiles; i++ )
	{
		mz_zip_reader_get_filename( zip_archive, i, szFilename, 1024 );
		if ( _strnicmp( folder, szFilename, strlen(folder) ) == 0 )
		{
			// matches folder, create destination path
			strcpy_s( szDstPath, base_path );
			strcat_s( szDstPath, "/" );
			strcat_s( szDstPath, szFilename );

			// extract it to destination
			CreatePath( szDstPath );
			if ( !mz_zip_reader_extract_file_to_file( zip_archive, szFilename, szDstPath, 0 ) )
			{
				char err[ 1024 ];
				sprintf_s( err, "Failed to open \"%s\" for writing", szDstPath );
				Error( err );
				return -1;	
			}
		}
	}

	return 1;
}

int main( int argc, char* argv[] )
{
	// prepare for relative pathing
	char szBuildToolStartDir[1024];
	GetCurrentDirectory( 1024, szBuildToolStartDir); // D:\DEV\AGKREPO\AGK\tools\AGKBuildSystem\Windows\Final
	char szRepoRoot[1024];
	strcpy(szRepoRoot, szBuildToolStartDir);
	strcat(szRepoRoot, "\\..\\..\\..\\..\\..\\");
	SetCurrentDirectory(szRepoRoot);
	GetCurrentDirectory(1024, szRepoRoot);
	char szAGKTrunkDir[1024];
	sprintf(szAGKTrunkDir, "%s\\AGK\\", szRepoRoot); 
	//MessageBoxA(NULL, szAGKTrunkDir, szAGKTrunkDir, MB_OK);
	SetCurrentDirectoryWithCheck(szAGKTrunkDir);// ("D:\\AGK\\"); // AGKTrunk

	// Repo folders
	char szDstFolderWin[1024]; sprintf(szDstFolderWin, "%s\\AGK_Build\\Builds\\Studio\\AGKStudioWindows", szRepoRoot);
	char szDstFolderMac[1024]; sprintf(szDstFolderMac, "%s\\AGK_Build\\Builds\\Studio\\AGKStudioMac\\AGK", szRepoRoot);
	char szDstFolderLinux[1024]; sprintf(szDstFolderLinux, "%s\\AGK_Build\\Builds\\Studio\\AGKStudioLinux\\AGK", szRepoRoot);
	char szDstFolderWinTrial[1024]; sprintf(szDstFolderWinTrial, "%s\\AGK_Build\\Builds\\Studio\\AGKStudioWindowsTrial", szRepoRoot);
	char szSharedFolder[1024]; sprintf(szSharedFolder, "%s\\AGK_Build\\Shared\\WindowsReceive", szRepoRoot);
	char szKeyStore[1024]; sprintf(szKeyStore, "%s\\AGK_Build\\Signing\\keystore.keystore", szRepoRoot);
	char szTemp[1024]; sprintf(szTemp, "%s\\AGK_Build\\Temp", szRepoRoot);

	// Installed tools
	char szJarSigner[1024]; sprintf(szJarSigner, "C:\\Program Files\\Android\\Android Studio\\jre\\bin\\jarsigner.exe");
	char szVisualStudio[1024]; sprintf(szVisualStudio, "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\devenv.exe");
	char szJava[1024]; sprintf(szJava, "C:\\Program Files\\Java\\jdk-17\\bin\\java.exe");

	// Android Studio files in AppData
	char szAPKSigner[1024]; strcpy(szAPKSigner, "");
	char szZipAlign[1024]; strcpy(szZipAlign, "");
	char szGradleRes[1024];  strcpy(szGradleRes, "");
	LPSTR pUserName = getenv("USERNAMEFORAGK");
	if (pUserName)
	{
		sprintf(szAPKSigner, "C:\\Users\\%s\\AppData\\Local\\Android\\Sdk\\build-tools\\33.0.3\\lib\\apksigner.jar", pUserName);
		sprintf(szZipAlign, "C:\\Users\\%s\\AppData\\Local\\Android\\Sdk\\build-tools\\33.0.3\\zipalign.exe", pUserName);
		sprintf(szGradleRes, "C:\\Users\\%s\\.gradle", pUserName);
	}

	/* old method using absolute paths per developer
	// set some path variables
	#if defined(MIKE_BUILD)
	// mike - 110621 - an option for the future is to modify the app so that you select a text file
	// when it launches that contains all of the relevant paths
	//const char* szVisualStudio = "D:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\Common7\\IDE\\devenv.exe"; // can be any version as long as it has platform tools v140 installed
	const char* szVS2015 = "D:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\Common7\\IDE\\devenv.exe";
	const char* szVS2017 = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\Common7\\IDE\\devenv.exe";
    const char* szDstFolderWin = "D:\\AGK_Build\\Builds\\Studio\\AGKStudioWindows";
	const char* szDstFolderMac = "D:\\AGK_Build\\Builds\\Studio\\AGKStudioMac\\AGK";
	const char* szDstFolderLinux = "D:\\AGK_Build\\Builds\\Studio\\AGKStudioLinux\\AGK";
	const char* szDstFolderWinTrial = "D:\\AGK_Build\\Builds\\Studio\\AGKStudioWindowsTrial";
	//const char* szSharedFolder = "D:\\AGK\\Shared"; // for Mac and Linux access
	const char* szSharedFolder = "D:\\AGK_Build\\Shared\\WindowsReceive"; // for Mac and Linux access
	//const char* szZipAlign = "D:\\Projects\\Android\\SDK\\build-tools\\31.0.0\\zipalign.exe";
	//const char* szAPKSigner = "D:\\Projects\\Android\\SDK\\build-tools\\31.0.0\\lib\\apksigner.jar";
	//const char* szJarSigner = "D:\\Projects\\Android\\Java\\bin\\jarsigner.exe";
	const char* szJarSigner = "E:\\AndroidStudio\\jre\\bin\\jarsigner.exe";
	const char* szKeyStore = "D:\\AGK_Build\\Signing\\keystore.keystore";
	const char* szTemp = "D:\\AGK_Build\\Temp";
	//const char* szGradleRes = "D:\\Documents\\.gradle";
	const char* szGradleRes = "C:\\Users\\Mike\\.gradle";
	//const char* szJava = "d:\\Projects\\Java\\jdk-11\\bin\\java.exe";
	const char* szVisualStudio = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\devenv.exe"; // can be any version as long as it has platform tools v140 installed	
	const char* szJava = "C:\\Program Files\\Java\\jdk-17.0.2\\bin\\java.exe";
	const char* szTortoiseSVN = "C:\\Program Files\\TortoiseSVN\\bin\\TortoiseProc.exe";
	const char* szAPKSigner = "C:\\Users\\Mike\\AppData\\Local\\Android\\Sdk\\build-tools\\31.0.0\\lib\\apksigner.jar";
	const char* szZipAlign = "C:\\Users\\Mike\\AppData\\Local\\Android\\Sdk\\build-tools\\31.0.0\\zipalign.exe";
	//SetCurrentDirectoryWithCheck ( "D:\\AGK\\Studio" ); // AGKTrunk
	SetCurrentDirectoryWithCheck ("D:\\AGK\\"); // AGKTrunk
	#elif defined(PAUL_BUILD)
	const char* szVisualStudio = "C:\\Programs\\Visual Studio 2022\\Common7\\IDE\\devenv.exe"; // can be any version as long as it has platform tools v140 installed
	const char* szDstFolderWin = "C:\\TGC\\AGKStudioBuild\\AGKStudioWindows";
	const char* szDstFolderMac = "C:\\TGC\\AGKStudioBuild\\AGKStudioMac\\AGK";
	const char* szDstFolderLinux = "C:\\TGC\\AGKStudioBuild\\AGKStudioLinux\\AGK";
	const char* szDstFolderWinTrial = "C:\\TGC\\AGKStudioBuild\\AGKStudioWindowsTrial";
	const char* szSharedFolder = "E:\\Receive"; // for Mac and Linux access
	const char* szTortoiseSVN = "C:\\Programs\\TortoiseSVN\\bin\\TortoiseProc.exe";
	const char* szZipAlign = "E:\\Data\\AndroidSDK\\build-tools\\31.0.0\\zipalign.exe";
	const char* szAPKSigner = "E:\\Data\\AndroidSDK\\build-tools\\31.0.0\\lib\\apksigner.jar";
	const char* szJarSigner = "C:\\Programs\\jdk-11.0.15.1\\bin\\jarsigner.exe";
	const char* szJava = "C:\\Programs\\jdk-11.0.15.1\\bin\\java.exe";
	const char* szKeyStore = "C:\\Paul\\TGC\\keystore\\keystore.keystore";
	const char* szTemp = "E:\\Temp";
	const char* szGradleRes = "C:\\Users\\PSJoh\\.gradle";
	SetCurrentDirectoryWithCheck ( "..\\..\\..\\.." ); // AGKTrunk
	#else
	#error No build user defined, add the Visual Studio User Macro $(BUILD_USER)
	#endif
	*/

	char rootFolder[ 1024 ];
	GetCurrentDirectory( 1024, rootFolder );

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

		bool keystorePasswordRequired = false;
		if ( !bSingleCommand )
		{
			if ( index <= iAndroidSigningIndex ) keystorePasswordRequired = true;
		}
		else
		{
			if ( index == iAndroidSigningIndex )
				keystorePasswordRequired = true;
		}

		
		if ( keystorePasswordRequired ) 
		{
			printf( "Enter keystore password: " );
			DWORD mode = 0;
			HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
			GetConsoleMode(hStdin, &mode);
			SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
			char* result = fgets(szPassword, 256, stdin);
			SetConsoleMode(hStdin, mode | ENABLE_ECHO_INPUT);
			if ( !result ) Error( "Failed to read input" );
			char* szEnd = szPassword + strlen(szPassword) - 1;
			while( *szEnd == '\n' || *szEnd == '\r' ) 
			{
				*szEnd = 0;
				szEnd--;
			}
		}
	}

	int indexCheck = 0;

	// Update SVN
	if ( !bListCommands )
	{
		//int status = 0;
		//status = RunCmd( indexCheck, szTortoiseSVN, "/command:update /path:\".\" /closeonend:3" );
		//if ( status != 0 ) Error( "Failed" );
		//else Message( "  Success" );
	}
	
	// What's new file
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Update What's New File", indexCheck );
		else
		{
			Message( "Updating What's New file" );
			if ( !UpdateWhatsNewFile() ) Error( "Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Command list
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Command Parser", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "CommandParserNew" );

			Message( "Compiling CommandParser project" );
			int status = RunCmd( indexCheck, szVisualStudio, "CommandParser.sln /rebuild \"Release|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			SetCurrentDirectoryWithCheck( "Final" );

			Message( "Running CommandParser" );
			status = RunCmd( indexCheck, "CommandParser.exe", "nopause" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." ); // AGKTrunk
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Visual Studio Windows Release version
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile Windows Core Release", indexCheck );
		else
		{
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "AGKWindows.sln /rebuild \"Release|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}
	
	// Visual Studio Windows DebugCompatible version, this is a debug version compiled with optimization
	// This is to allow users to compile debug versions of their apps without having a slow debug core library
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile Windows Core DebugCompatible", indexCheck );
		else
		{
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "AGKWindows.sln /rebuild \"DebugCompatible|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Compiler Static Lib (for VSCode Langauge Server and Studio IDE)
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile AGK Compiler Static Lib", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "CompilerNew" );
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "AGKCompiler2.sln /rebuild \"ReleaseStaticLib|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			status = RunCmd( indexCheck, szVisualStudio, "AGKCompiler2.sln /rebuild \"ReleaseStaticLibIDE|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( ".." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Compiler EXE (for VSCode)
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile AGK Compiler EXE", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "CompilerNew" );
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "AGKCompiler2.sln /rebuild \"Release|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( ".." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Broadcaster Static Lib (for VSCode Debug Adapter and Studio IDE)
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile AGK Broadcaster Static Lib", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "Broadcaster\\AGKBroadcaster" );
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "AGKBroadcaster.sln /rebuild \"ReleaseStaticLib|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			status = RunCmd( indexCheck, szVisualStudio, "AGKBroadcaster.sln /rebuild \"ReleaseStaticLibIDE|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// VSCode Debug Adapter
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile VSCode Debug Adapter", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "VSCode\\Debug Adapter" );
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "DebugAdapter.sln /rebuild \"Release|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// VSCode Language Server
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile VSCode Language Server", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "VSCode\\Language Server Protocol" );
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "LanguageServer.sln /rebuild \"Release|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );
			
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// interpreter 64-bit
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile Windows interpreter 64-bit ReleaseSteam", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "apps\\interpreter" );
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "interpreter.sln /rebuild \"ReleaseSteam|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Tier 2 template (to check it works, does not contribute to the final build)
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile template_windows project", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "apps\\template_windows" );
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "Template.sln /rebuild Release" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Build IDE
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile Windows IDE", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "AgkIde" );
			int status = 0;
			status = RunCmd( indexCheck, szVisualStudio, "Ide.sln /rebuild \"Release\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( ".." );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// must be done before anything modifies the build folder, otherwise those changes will be overwritten
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Copy IDE files to build folders", indexCheck );
		else
		{
			char msg[ 128 ];
			sprintf( msg, "%d: Copying IDE media folder and exe to build folder", indexCheck );
			Message( msg );

			// copy media folder
			char srcFolder[ 1024 ];
			strcpy( srcFolder, rootFolder );
			strcat( srcFolder, "\\AgkIde\\media" );
							
			char dstFolder[ 1024 ];
			strcpy( dstFolder, szDstFolderWin ); 
			strcat( dstFolder, "\\media" ); 
			
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			// copy executable
			strcpy( dstFolder, szDstFolderWin ); 
			strcat( dstFolder, "\\Ide.exe" ); 
			CopyFile2( "AgkIde\\Final\\Ide.exe", dstFolder );

			Message( "Deleting iOS files" );
            strcpy( dstFolder, szDstFolderWin ); strcat( dstFolder, "\\media\\data\\ios" ); 
            DeleteFolder( dstFolder );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// ***************
	// ANDROID
	// ***************

	// Android lib
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile Android lib", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "libprojects\\android_base" );
			int status = 0;
			status = RunCmd ( indexCheck, "cmd.exe", "/C jniCompile.bat nopause" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			status = RunCmd( indexCheck, "cmd.exe", "/C copylibs-nopause.bat" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			SetCurrentDirectoryWithCheck( "..\\.." );
						
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Android interpreter
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Compile Android interpreter", indexCheck );
		else
		{
			SetCurrentDirectoryWithCheck( "apps\\interpreter_android_google\\AGKPlayer2\\src\\main" );
			int status = 0;
			status = RunCmd ( indexCheck, "cmd.exe", "/C jniCompile.bat nopause" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			SetCurrentDirectoryWithCheck( "..\\..\\.." ); // interpreter_android_google

			status = RunCmd( indexCheck, "cmd.exe", "/C copylibs-nopause.bat" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );
			
			SetCurrentDirectoryWithCheck( "..\\.." ); // AGKTrunk

			// copy libs to shared folder
			char dstFolder[ 1024 ];
			char srcFolder[ 1024 ];
			strcpy( dstFolder, szSharedFolder ); strcat( dstFolder, "\\Studio\\Android\\lib" );
			strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\apps\\interpreter_android_google\\AGKPlayer2\\src\\main\\jniLibs" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			// copy SnapChat libs to shared folder
			strcpy( dstFolder, szSharedFolder ); 
			strcat( dstFolder, "\\Studio\\Android\\lib\\arm64-v8a\\libpruneau.so" ); 
			CopyFile2( "platform\\android\\SnapChat\\arm64-v8a\\libpruneau.so", dstFolder );

			strcpy( dstFolder, szSharedFolder ); 
			strcat( dstFolder, "\\Studio\\Android\\lib\\armeabi-v7a\\libpruneau.so" ); 
			CopyFile2( "platform\\android\\SnapChat\\armeabi-v7a\\libpruneau.so", dstFolder );
						
			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Export Android APK projects
	if ( index <= ++indexCheck )
	{
		iAndroidSigningIndex = indexCheck;

		if ( bListCommands ) Message1( "%d: Export Android Google/Amazon/Ouya interpreter projects", indexCheck );
		else
		{
			const char *szAndroidIDEFolders[] = { "sourceGoogle", "sourceAmazon", "sourceOuya" };
			const char *szAndroidProjects[] = { "interpreter_android_google", "interpreter_android_amazon", "interpreter_android_ouya" };
			
			char szAndroidBuildPathDefault[ 1024 ];
			strcpy( szAndroidBuildPathDefault, szDstFolderWin );
			strcat( szAndroidBuildPathDefault, "\\media\\data\\android\\" );

			const char *szAndroidBuildPath[] = { szAndroidBuildPathDefault, szAndroidBuildPathDefault, szAndroidBuildPathDefault };

			char srcFolder[ 1024 ];
			char dstFolder[ 1024 ];

			g_allResources.SetSorted( true, CompareResourceItems );

			const int numApps = sizeof(szAndroidIDEFolders) / sizeof(const char*);
			for ( int i = 0; i < numApps; i++ )
			{
				char msg[ 256 ]; sprintf( msg, "Exporting project %s", szAndroidProjects[i] );
				Message( msg );

				char path[1024];
				strcpy( path, "apps\\" );
				strcat( path, szAndroidProjects[i] );
				SetCurrentDirectoryWithCheck( path );
				
				// delete old gradle execution log
				DeleteFile( ".gradle\\8.0\\executionHistory\\executionHistory.bin" );
				DeleteFile( ".gradle\\8.0\\executionHistory\\executionHistory.lock" );
				FILE* pFile = fopen( ".gradle\\8.0\\executionHistory\\executionHistory.bin", "rb" );
				if ( pFile ) 
				{
					fclose( pFile );
					Error( "Failed to delete execution history" );
				}

				// clean build
				int status = 0;
				status = RunCmd( indexCheck, "cmd.exe", "/C gradlew.bat clean" );
				if ( status != 0 ) Error( "Failed" );
				else Message( "  Success" );

				// export unsigned unaligned apk
				status = RunCmd( indexCheck, "cmd.exe", "/C gradlew.bat assembleRelease" );
				if ( status != 0 ) Error( "Failed" );
				else Message( "  Success" );
				
				// sign apk, must be done first when using jarsigner
				SetCurrentDirectoryWithCheck( "AGKPlayer2\\build\\outputs\\apk" );
				
				char str[ 1000 ];
				sprintf( str, "-sigalg MD5withRSA -digestalg SHA1 -storepass \"%s\" -keystore \"%s\" release\\AGKPlayer2-release-unsigned.apk tgc -keypass \"%s\"", szPassword, szKeyStore, szPassword );
				status = RunCmd( indexCheck, szJarSigner, str );
				if ( status != 0 ) Error( "Failed" );
				else Message( "  Success" );

				// align apk
				DeleteFile( "AGKPlayer2-release-automated.apk" );
				status = RunCmd( indexCheck, szZipAlign, "-f 4 release\\AGKPlayer2-release-unsigned.apk AGKPlayer2-release-automated.apk" );
				if ( status != 0 ) Error( "Failed" );
				else Message( "  Success" );

				// sign APK V2
				sprintf( str, "-jar \"%s\" sign --ks \"%s\" --ks-pass pass:\"%s\" --ks-key-alias tgc --key-pass pass:\"%s\" AGKPlayer2-release-automated.apk", szAPKSigner, szKeyStore, szPassword, szPassword );
				status = RunCmd( indexCheck, szJava, str );
				if ( status != 0 ) Error( "Failed" );
				else Message( "  Success" );

				strcpy( dstFolder, szAndroidBuildPath[i] ); 
				strcat( dstFolder, szAndroidIDEFolders[i] ); 

				// extract classes.dex
				mz_zip_archive zip_archive;
				memset(&zip_archive, 0, sizeof(zip_archive));
				if ( !mz_zip_reader_init_file( &zip_archive, "AGKPlayer2-release-automated.apk", 0 ) ) Error( "Failed to open apk as a zip file" );
				size_t dataLength = 0;
				int classesDexNum = 1;
				char classesDexStr[ 32 ];
				do
				{
					if ( classesDexNum == 1 ) strcpy( classesDexStr, "classes.dex" );
					else sprintf( classesDexStr, "classes%d.dex", classesDexNum );

					strcpy( dstFolder, szAndroidBuildPath[i] ); 
					strcat( dstFolder, szAndroidIDEFolders[i] );
					strcat( dstFolder, "\\" ); 
					strcat( dstFolder, classesDexStr );

					int result = ExtractFile( &zip_archive, classesDexStr, dstFolder );
					if ( result < 0 ) break;
					else if ( result == 0 )
					{
						if ( classesDexNum == 1 ) Error( "Failed to extract classes.dex from APK" );
						break;
					}

					classesDexNum++;
					if ( classesDexNum > 30 ) break;
				} while( 1 );

				// copy additional folders
				strcpy( dstFolder, szAndroidBuildPath[i] ); 
				strcat( dstFolder, szAndroidIDEFolders[i] );
				ExtractFolder( &zip_archive, "com", dstFolder );
				ExtractFolder( &zip_archive, "kotlin", dstFolder );
				ExtractFolder( &zip_archive, "assets", dstFolder );
				strcat( dstFolder, "\\extra_root\\core.properties" );
				ExtractFile( &zip_archive, "core.properties", dstFolder );

				mz_zip_reader_end( &zip_archive );

				SetCurrentDirectoryWithCheck( "..\\..\\..\\..\\..\\.." ); // AGKTrunk

				// copy mapping file
				if ( i == 0 || i == 3 )
				{
					strcpy( srcFolder, rootFolder );
					strcat( srcFolder, "\\apps\\" );
					strcat( srcFolder, szAndroidProjects[i] );
					strcat( srcFolder, "\\AGKPlayer2\\build\\outputs\\mapping\\release\\mapping.txt" );
				
					strcpy( dstFolder, szAndroidBuildPath[i] );
					strcat( dstFolder, szAndroidIDEFolders[i] );
					strcat( dstFolder, "\\mapping" );
					CreateDirectory( dstFolder, NULL );
					strcat( dstFolder, "\\mapping.txt" );
					CopyFile2( srcFolder, dstFolder );
				}

				// backup AndroidManifest.xml so we can check changes
				strcpy( srcFolder, rootFolder );
				strcat( srcFolder, "\\apps\\" );
				strcat( srcFolder, szAndroidProjects[i] );
				strcat( srcFolder, "\\AGKPlayer2\\build\\intermediates\\merged_manifest\\release\\AndroidManifest.xml" );
				unsigned char* manifestData = 0;
				size_t length = GetFileContents( srcFolder, (char**)&manifestData );
				if ( length == 0 ) Error( "Failed to open merged AndroidManifest.xml" );

				const char* manifestStart = strstr( (const char*) manifestData, "<uses-" );

				strcpy( dstFolder, rootFolder ); 
				strcat( dstFolder, "\\tools\\AGKBuildSystem\\Backup\\Android\\" );
				strcat( dstFolder, szAndroidIDEFolders[i] );
				strcat( dstFolder, "\\AndroidManifest.xml" );
				CreatePath( dstFolder );
				FILE *fp = fopen( dstFolder, "wb" );
				if ( !fp ) Error( "Failed to open backup AndroidManifest.xml for writing" );
				fwrite( manifestStart, 1, strlen(manifestStart), fp );

				delete [] manifestData;

				// copy resMerged folder
				strcpy( srcFolder, rootFolder );
				strcat( srcFolder, "\\apps\\" );
				strcat( srcFolder, szAndroidProjects[i] );
				strcat( srcFolder, "\\AGKPlayer2\\build\\intermediates\\merged_res\\release" );
				
				strcpy( dstFolder, szAndroidBuildPath[i] );
				strcat( dstFolder, szAndroidIDEFolders[i] );
				strcat( dstFolder, "\\resMerged" );
				
				DeleteFolder( dstFolder );
				CreateDirectory( dstFolder, NULL );
				CopyFolder( srcFolder, dstFolder );

				// collect gradle resources
				strcpy( srcFolder, "apps\\" ); 
				strcat( srcFolder, szAndroidProjects[i] );
				strcat( srcFolder, "\\.gradle\\8.0\\executionHistory\\executionHistory.bin" );
				unsigned char* data = 0;
				length = GetFileContents( srcFolder, (char**) &data );
				if ( length == 0 ) Error( "Failed to open executionHistory.bin" );

				int count = 0;
				unsigned char* ptr = data;
				const char* find = "caches\\transforms";
				int findLen = (int) strlen( find );
				while( ptr < (data + length - findLen - 1) )
				{
					if ( strncmp( (char*)ptr, find, findLen ) != 0 ) 
					{
						ptr++;
					}
					else
					{
						count++;
						ptr += findLen;
						
						// look for the end of the string, not null terminated so look for non-ascii character
						unsigned char* slash = ptr;
						while( *slash >= 32 && *slash <= 127 && slash < (data + length - 1) ) slash++;
				
						slash -= 5;
						if ( strncmp( (char*)slash, ".flat", 5 ) == 0 )
						{
							slash += 5;
							unsigned char old = *slash;
							*slash = 0;

							ResourceFile* pNewFile = new ResourceFile();
							pNewFile->SetStr( (char*)ptr );
							*slash = old;

							if ( g_allResources.Add( pNewFile, false ) < 0 ) delete pNewFile;
						}				

						ptr = slash;
					}
				}

				char dstFile[ 1024 ];
				
				for( int i = 0; i < g_allResources.m_iCount; i++ )
				{
					strcpy( srcFolder, szGradleRes );
					strcat( srcFolder, "\\caches\\transforms" );
					strcat( srcFolder, g_allResources.m_pData[ i ]->m_pString );

					char* slash = strrchr( g_allResources.m_pData[ i ]->m_pString, '\\' );
					if ( slash ) slash++;
					else slash = g_allResources.m_pData[ i ]->m_pString;

					strcpy( dstFile, dstFolder );
					strcat( dstFile, "\\" );
					strcat( dstFile, slash );

					CopyFile2( srcFolder, dstFile );
				}

				delete [] data;
				g_allResources.Clear();
				
				// create resOrig files
				strcpy( srcFolder, rootFolder );
				strcat( srcFolder, "\\apps\\" );
				strcat( srcFolder, szAndroidProjects[i] );
				strcat( srcFolder, "\\AGKPlayer2\\build\\intermediates\\incremental\\release\\mergeReleaseResources\\merged.dir\\values\\values.xml" );
				
				strcpy( dstFolder, szAndroidBuildPath[i] );
				strcat( dstFolder, szAndroidIDEFolders[i] );
				strcat( dstFolder, "\\resOrig" );
				CreateDirectory( dstFolder, 0 );
				strcat( dstFolder, "\\values" ); 
				CreateDirectory( dstFolder, 0 );
				strcat( dstFolder, "\\values.xml" );

				CopyFile2( srcFolder, dstFolder );

				if ( i != 2 )
				{
					strcpy( srcFolder, rootFolder );
					strcat( srcFolder, "\\apps\\" );
					strcat( srcFolder, szAndroidProjects[i] );
					strcat( srcFolder, "\\AGKPlayer2\\src\\main\\res\\mipmap-anydpi-v26\\ic_launcher.xml" );
				
					strcpy( dstFolder, szAndroidBuildPath[i] );
					strcat( dstFolder, szAndroidIDEFolders[i] );
					strcat( dstFolder, "\\resOrig\\mipmap-anydpi-v26" );
					CreateDirectory( dstFolder, 0 );
					strcat( dstFolder, "\\ic_launcher.xml" );

					CopyFile2( srcFolder, dstFolder );
				}
			}

			// copy Android players to temp folder
			Message( "  Copying Android APKs to temp folder" );
			strcpy( dstFolder, szTemp ); strcat( dstFolder, "\\AppGameKitStudio-Google.apk" );
			CopyFile2( "apps\\interpreter_android_google\\AGKPlayer2\\build\\outputs\\apk\\AGKPlayer2-release-automated.apk", dstFolder );
			strcpy( dstFolder, szTemp ); strcat( dstFolder, "\\AppGameKitStudio-Amazon.apk" );
			CopyFile2( "apps\\interpreter_android_amazon\\AGKPlayer2\\build\\outputs\\apk\\AGKPlayer2-release-automated.apk", dstFolder );
			strcpy( dstFolder, szTemp ); strcat( dstFolder, "\\AppGameKitStudio-Ouya.apk" );
			CopyFile2( "apps\\interpreter_android_ouya\\AGKPlayer2\\build\\outputs\\apk\\AGKPlayer2-release-automated.apk", dstFolder );

			// copy Android files to shared folder
			strcpy( srcFolder, szDstFolderWin ); strcat( srcFolder, "\\media\\data\\android\\sourceAmazon" ); 
			strcpy( dstFolder, szSharedFolder ); strcat( dstFolder, "\\Studio\\Android\\sourceAmazon" ); 
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			strcpy( srcFolder, szDstFolderWin ); strcat( srcFolder, "\\media\\data\\android\\sourceGoogle" ); 
			strcpy( dstFolder, szSharedFolder ); strcat( dstFolder, "\\Studio\\Android\\sourceGoogle" ); 
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			strcpy( srcFolder, szDstFolderWin ); strcat( srcFolder, "\\media\\data\\android\\sourceOuya" ); 
			strcpy( dstFolder, szSharedFolder ); strcat( dstFolder, "\\Studio\\Android\\sourceOuya" ); 
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// Build help files
	if ( index <= ++indexCheck )
	{
		if ( bListCommands ) Message1( "%d: Build help files", indexCheck );
		else
		{
			char msg[ 128 ];
			sprintf( msg, "%d: Build help files", indexCheck );
			Message( msg );

			Message( "Compiling help generator" );

			SetCurrentDirectoryWithCheck( "AGK Help Generator\\Generator" );

			// Build help generator
			int status = RunCmd( indexCheck, szVisualStudio, "Generator.sln /rebuild \"Release|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			Message( "Running help generator" );

			// run help generator
			status = RunCmd( indexCheck, "Generator.exe", "" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			SetCurrentDirectoryWithCheck( "..\\Reference" );

			Message( "Compiling Reference project" );

			// build Reference builder
			status = RunCmd( indexCheck, szVisualStudio, "Reference.sln /rebuild \"Release|x64\"" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			Message( "Running Reference project" );

			if ( !SetCurrentDirectory("Output") )
			{
				CreateDirectory( "Output", NULL );
				SetCurrentDirectory( "Output" );
			}

			// clear existing command lists
			char srcFolder[ 1024 ];
			GetCurrentDirectory( 1024, srcFolder );
			DeleteFolder( srcFolder );
			
			// build command lists
			status = RunCmd( indexCheck, "..\\Reference.exe", "" );
			if ( status != 0 ) Error( "Failed" );
			else Message( "  Success" );

			Message( "Copying commands to main help folder" );

			// copy to main folder
			char dstFolder[ 1024 ];
			GetCurrentDirectory( 1024, dstFolder );
			strcat( dstFolder, "\\..\\..\\AGK Txt\\Reference" );

			DeleteFolder( dstFolder );
			const char *szIgnoreExt[] = { ".txt" };
			CopyFolder( srcFolder, dstFolder, 1, szIgnoreExt );

			// copy keyword files to main folder
			CopyFile2( "keywordlinks.txt", "..\\..\\AGK Txt\\keywordlinks.txt" );

			// edit search.html
			Message( "Editing search.html" );
			char *pSearchData = 0;
			int searchSize = GetFileContents( "..\\..\\AGK Txt\\search.html", &pSearchData );
			if ( !searchSize || !pSearchData ) Error( "Failed to read search.html file" );

			char *szCommands = strstr( pSearchData, "var commands = [" );
			if ( !szCommands ) Error( "Failed to find command list in search.html" );
			szCommands += strlen( "var commands = [" );

			char *szParameters = strstr( pSearchData, "var parameters = [" );
			if ( !szParameters ) Error( "Failed to find parameter list in search.html" );
			szParameters = strstr( szParameters, "];" );
			if ( !szParameters ) Error( "Failed to find end of parameter list in search.html" );

			*szCommands = 0;

			// get new data
			char *pCommandList = 0;
			int commandListSize = GetFileContents( "commands1.txt", &pCommandList );
			if ( !commandListSize || !pCommandList ) Error( "Failed to read new command list from commands1.txt" );

			char *pCategoryList = 0;
			int categoryListSize = GetFileContents( "commands2.txt", &pCategoryList );
			if ( !categoryListSize || !pCategoryList ) Error( "Failed to read new category list from commands2.txt" );

			char *pParameterList = 0;
			int ParameterListSize = GetFileContents( "commands3.txt", &pParameterList );
			if ( !ParameterListSize || !pParameterList ) Error( "Failed to read new parameter list from commands3.txt" );

			// create new search file data
			char *pNewSearchData = new char[ searchSize + 10000 ];
			strcpy( pNewSearchData, pSearchData );
			strcat( pNewSearchData, pCommandList );
			strcat( pNewSearchData, "];\n            var categories = [" );
			strcat( pNewSearchData, pCategoryList );
			strcat( pNewSearchData, "];\n            var parameters = [" );
			strcat( pNewSearchData, pParameterList );
			strcat( pNewSearchData, szParameters );

			FILE *fp = fopen( "..\\..\\AGK Txt\\search.html", "wb" );
			if ( !fp ) Error( "Failed to open earch.html for writing" );
			fwrite( pNewSearchData, 1, strlen(pNewSearchData), fp );
			fclose( fp );

			delete [] pSearchData;
			delete [] pCommandList;
			delete [] pCategoryList;
			delete [] pParameterList;

			Message( "Copying help files to shared folder" );
			
			// copy to shared folder
			SetCurrentDirectoryWithCheck( "..\\..\\AGK Txt" );
			GetCurrentDirectory( 1024, srcFolder );
			
			strcpy( dstFolder, szSharedFolder );
			strcat( dstFolder, "\\Studio\\Help" );

			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder, 1, szIgnoreExt );

			// plus skipped files
			strcat( dstFolder, "\\keywordlinks.txt" );
			CopyFile2( "keywordlinks.txt", dstFolder );

			strcpy( dstFolder, szSharedFolder );
			strcat( dstFolder, "\\Studio\\Help\\corekeywordlinks.txt" );
			CopyFile2( "corekeywordlinks.txt", dstFolder );

			Message( "Copying help files to Windows build" );

			// copy to Windows build
			strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "\\Studio\\Help" );
			strcpy( dstFolder, szDstFolderWin ); strcat( dstFolder, "\\media\\Help" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			Message( "Copying help files to Windows Trial build" );

			// copy to Windows build
			strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "\\Studio\\Help" );
			strcpy( dstFolder, szDstFolderWinTrial ); strcat( dstFolder, "\\media\\Help" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );
			
			Message( "Copying help files to Mac build" );

			// copy to Mac build
			strcpy( dstFolder, szDstFolderMac ); strcat( dstFolder, "\\Tier 1\\Help" );
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			Message( "Copying help files to Linux build" );

			// copy to Linux build
			strcpy( dstFolder, szDstFolderLinux ); strcat( dstFolder, "\\Tier1\\Help" ); // purposefully missing a space in Tier1
			DeleteFolder( dstFolder );
			CopyFolder( srcFolder, dstFolder );

			SetCurrentDirectoryWithCheck( "..\\.." ); // AGKTrunk

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

			// copy ARCore libs to build folder
			Message( "Copying Android libs to Windows build" );
			strcpy( dstFolder, szDstFolderWin ); 
			strcat( dstFolder, "\\media\\data\\android\\lib\\arm64-v8a\\libarcore_sdk.so" ); 
			CopyFile2( "platform\\android\\ARCore\\libs\\arm64-v8a\\libarcore_sdk.so", dstFolder );

			strcpy( dstFolder, szDstFolderWin ); 
			strcat( dstFolder, "\\media\\data\\android\\lib\\armeabi-v7a\\libarcore_sdk.so" ); 
			CopyFile2( "platform\\android\\ARCore\\libs\\armeabi-v7a\\libarcore_sdk.so", dstFolder );

			// copy SnapChat libs to build folder
			strcpy( dstFolder, szDstFolderWin ); 
			strcat( dstFolder, "\\media\\data\\android\\lib\\arm64-v8a\\libpruneau.so" ); 
			CopyFile2( "platform\\android\\SnapChat\\arm64-v8a\\libpruneau.so", dstFolder );

			strcpy( dstFolder, szDstFolderWin ); 
			strcat( dstFolder, "\\media\\data\\android\\lib\\armeabi-v7a\\libpruneau.so" ); 
			CopyFile2( "platform\\android\\SnapChat\\armeabi-v7a\\libpruneau.so", dstFolder );

			// main Android libs
			strcpy( dstFolder, szDstFolderWin ); 
			strcat( dstFolder, "\\media\\data\\android\\lib\\arm64-v8a\\libandroid_player.so" ); 
			CopyFile2( "apps\\interpreter_android_google\\AGKPlayer2\\src\\main\\jniLibs\\arm64-v8a\\libandroid_player.so", dstFolder );

			strcpy( dstFolder, szDstFolderWin ); 
			strcat( dstFolder, "\\media\\data\\android\\lib\\armeabi-v7a\\libandroid_player.so" ); 
			CopyFile2( "apps\\interpreter_android_google\\AGKPlayer2\\src\\main\\jniLibs\\armeabi-v7a\\libandroid_player.so", dstFolder );


			const char* szBuildFolder[3] = { szDstFolderWin, szDstFolderMac, szDstFolderLinux };
			const char* szBuildPlatform[3] = { "Windows", "Mac", "Linux" };
			const char* szTier2[3] = { "\\Tier 2", "\\Tier 2", "\\Tier2" };

			for ( int b = 0; b < 3; b++ ) 
			{
				if ( b == 0 ) Message( "Copying to Windows build" );
				if ( b == 1 ) Message( "Copying to Mac build" );
				if ( b == 2 ) Message( "Copying to Linux build" );

				Message( "  Copying Tier 2 files" );

				// Firebase frameworks for iOS are copied separately to the iOS platform Source folder
				
				// update apps folder
				Message( "    Copying apps folder" );
				strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\apps" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, szTier2[b] ); 
				// make a copy of the files in case we ever lose the folder structure
				files.Reset(); RecordFiles( dstFolder, "apps", &files ); sprintf( backupFolder, "tools\\AGKBuildSystem\\Backup\\%s\\appFiles.txt", szBuildPlatform[b] ); files.Save( backupFolder );
				strcat( dstFolder, "\\apps" );

				// MIKE - HAD TO COMMENT THIS OUT - IT TRIES TO FIND FILES THAT ARE NO LONGER RELEVANT
				//UpdateFolder( srcFolder, dstFolder );
				
				// update common folder
				Message( "    Copying common folder" );
				strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\common\\include" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, szTier2[b] ); 
				//files.Reset(); RecordFiles( dstFolder, "common\\include", &files ); sprintf( backupFolder, "tools\\AGKBuildSystem\\Backup\\%s\\commonIncludeFiles.txt", szBuildPlatform[b] ); files.Save( backupFolder );
				strcat( dstFolder, "\\common\\include" );
				DeleteFolder( dstFolder );
				CopyFolder( srcFolder, dstFolder );

				strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\common\\Collision" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, szTier2[b] ); 
				//files.Reset(); RecordFiles( dstFolder, "common\\Collision", &files ); sprintf( backupFolder, "tools\\AGKBuildSystem\\Backup\\%s\\commonCollisionFiles.txt", szBuildPlatform[b] ); files.Save( backupFolder );
				strcat( dstFolder, "\\common\\Collision" );
				DeleteFolder( dstFolder );
				const char *szIgnore[] = { ".cpp", ".c", ".CPP", ".C" };
				CopyFolder( srcFolder, dstFolder, 4, szIgnore );

				strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\renderer\\Renderer.h" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, szTier2[b] ); strcat( dstFolder, "\\renderer\\Renderer.h" );
				CopyFile2( srcFolder, dstFolder );

				// update bullet folder
				Message( "    Copying bullet folder" );
				strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\bullet" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, szTier2[b] ); 
				//files.Reset(); RecordFiles( dstFolder, "bullet", &files ); sprintf( backupFolder, "tools\\AGKBuildSystem\\Backup\\%s\\bulletFiles.txt", szBuildPlatform[b] ); files.Save( backupFolder );
				strcat( dstFolder, "\\bullet" );
				DeleteFolder( dstFolder );
				CopyFolder( srcFolder, dstFolder, 4, szIgnore );

				// update platform folder
				Message( "    Copying platform folder" );
				strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\platform" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, szTier2[b] ); 
				files.Reset(); RecordFiles( dstFolder, "platform", &files ); sprintf( backupFolder, "tools\\AGKBuildSystem\\Backup\\%s\\platformFiles.txt", szBuildPlatform[b] ); files.Save( backupFolder );
				strcat( dstFolder, "\\platform" );
				UpdateFolder( srcFolder, dstFolder );

				// example plugin
				Message( "    Copying Example Plugin folder" );
				strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\plugins\\Source" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\Example Plugin" );
				UpdateFolder( srcFolder, dstFolder );
				strcat( dstFolder, "\\Commands.txt" );
				CopyFile2( "plugins\\Plugins\\ExamplePlugin\\Commands.txt", dstFolder );

				Message( "  Copying Windows and Android players" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\Players\\Windows\\Windows64.exe" );
				CopyFile2( "apps\\interpreter\\Final\\Windows64.exe", dstFolder );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\Players\\Android\\AppGameKit Player.apk" );
				CopyFile2( "apps\\interpreter_android_google\\AGKPlayer2\\build\\outputs\\apk\\AGKPlayer2-release-automated.apk", dstFolder );
								
				// copy changelog
				Message( "  Copying other files" );
				strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\ChangeLog.txt" );
				CopyFile2( "AGK.txt", dstFolder );

				if ( b == 0 )
				{
					// copy plugins
					strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\plugins\\Plugins" );
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\Plugins" );
					DeleteFolder( dstFolder );
					CopyFolder( srcFolder, dstFolder );
					 
					// copy interpreter
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\media\\interpreters\\Windows64.exe" );
					CopyFile2( "apps\\interpreter\\Final\\Windows64.exe", dstFolder );

					// copy compiler
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\media\\compiler\\AGKCompiler64.exe" );
					CreatePath( dstFolder );
					CopyFile2( "CompilerNew\\Release64\\AGKCompiler64.exe", dstFolder );

					// copy VSCode binaries
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\VSCode\\DebugAdapter64.exe" );
					CreatePath( dstFolder );
					CopyFile2( "VSCode\\Debug Adapter\\Final\\DebugAdapter64.exe", dstFolder );
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\VSCode\\LanguageServer64.exe" );
					CreatePath( dstFolder );
					CopyFile2( "VSCode\\Language Server Protocol\\Final\\LanguageServer64.exe", dstFolder );
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\VSCode\\AGK VSCode Extension.vsix" );
					CreatePath( dstFolder );
					CopyFile2( "VSCode\\Extension\\agklang-1.0.0.vsix", dstFolder );

					// copy steam files
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\media\\interpreters\\steam\\steam_api.dll" );
					CopyFile2( "apps\\interpreter\\Final\\steam\\steam_api.dll", dstFolder );
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\media\\interpreters\\steam\\steam_api64.dll" );
					CopyFile2( "apps\\interpreter\\Final\\steam\\steam_api64.dll", dstFolder );
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\media\\interpreters\\steam\\steam_appid.txt" );
					CopyFile2( "apps\\interpreter\\Final\\steam\\steam_appid.txt", dstFolder );

					// copy image joiner
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\Utilities\\ImageJoiner.exe" );
					CopyFile2( "tools\\ImageJoiner.exe", dstFolder );
					
					// copying example projects
					Message( "  Copying Example Projects" );
					strcpy( srcFolder, rootFolder ); strcat( srcFolder, "\\Examples" );
					strcpy( dstFolder, szBuildFolder[b] ); strcat( dstFolder, "\\media\\Projects" );
					DeleteFolder( dstFolder );
					CopyFolder( srcFolder, dstFolder );
				}
			}

			if ( bSingleCommand ) goto endPoint;
		}
	}

	// copy shared files from other platforms
	// this must only be done after the other platforms have finished so limit it to single command mode
	if ( index <= ++indexCheck ) 
	{
		if ( bListCommands ) Message1( "%d: Copy shared files from other platforms", indexCheck );
		else
		{
			if ( bSingleCommand )
			{
				char srcFolder[ 1024 ];
				char dstFolder[ 1024 ];
			
				// copy HTML5 files from shared folder
				strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "\\Studio\\HTML5" );
				strcpy( dstFolder, szDstFolderWin ); strcat( dstFolder, "\\media\\data\\html5" );
				DeleteFolder( dstFolder );
				const char *szIgnore[] = { ".DS_Store" };
				CopyFolder( srcFolder, dstFolder, 1, szIgnore );

				// copy Mac player
				strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "\\Studio\\AppGameKit Player.app.zip" );
				strcpy( dstFolder, szDstFolderWin ); strcat( dstFolder, "\\Players\\Mac\\AppGameKit Player.app.zip" );
				CopyFile2( srcFolder, dstFolder );

				// copy Linux player
				strcpy( srcFolder, szSharedFolder ); strcat( srcFolder, "\\Studio\\LinuxPlayer64" );
				strcpy( dstFolder, szDstFolderWin ); strcat( dstFolder, "\\Players\\Linux\\LinuxPlayer64" );
				CopyFile2( srcFolder, dstFolder );

				if ( bSingleCommand ) goto endPoint;
			}
		}
	}

	if ( bListCommands ) 
	{
		bListCommands = false;
		goto startPoint;
	}

	// Commit SVN
	//int status = 0;
	//Message( "Comitting SVN" );
	//status = RunCmd( indexCheck, szTortoiseSVN, "/command:commit /path:\".\"" );
	
endPoint:
	system("pause");
	return 0;
}
