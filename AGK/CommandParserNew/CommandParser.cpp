// CommandParser.cpp : Defines the entry point for the console application.
//

#ifndef _WIN32_WINNT				// Specifies that the minimum required platform is Windows Vista.
	#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "strfunctions.h"

bool bFirst = true;
bool bCnopDone = false;
bool bCnop2Done = false;
char szPrevCommand[ 200 ] = "";

struct sCommand
{
	char szCommandName[ 200 ];
	unsigned char iParamsTypes[ 32 ]; // max 32 parameters
	char szParamsName[ 32 ][ 100 ]; // max 32 parameters
	char szParamsTypeText[ 32 ][ 100 ]; // max 32 parameters
	char szReturnTypeText[ 100 ];
	unsigned char iExtra; // 0 = Tier1, 1 = Tier2, 2+ = reserved
	unsigned char iReturnType;
	unsigned char iNumParams;
};

#define MAX_COMMANDS 3000
int g_iNumCommands = 0;
sCommand g_pCommands[ MAX_COMMANDS ];
sCommand g_pSortedVSCommands[ MAX_COMMANDS ]; // sorted by lower case
sCommand g_pSortedPluginCommands[ MAX_COMMANDS ]; // sorted with command and parameter string

int CommandCompare( const void* a, const void* b )
{
	sCommand* c1 = (sCommand*) a;
	sCommand* c2 = (sCommand*) b;

	int result = strcmp( c1->szCommandName, c2->szCommandName );
	if ( result != 0 ) return result;
	else
	{
		// same command name, compare by num parameters
		if ( c1->iNumParams < c2->iNumParams ) return -1;
		else if ( c1->iNumParams > c2->iNumParams ) return 1;
		else 
		{
			for ( int i = 0; i < c1->iNumParams; i++ )
			{
				if ( c1->iParamsTypes[ i ] < c2->iParamsTypes[ i ] ) return -1;
				else if ( c1->iParamsTypes[ i ] > c2->iParamsTypes[ i ] ) return 1;
			}

			return 0;
		}
	}
}

void GetCommandPluginName( sCommand* pCmd, char* output, int size )
{
	sprintf( output, "%s_%s", pCmd->szCommandName, TypeString1(pCmd->iReturnType) );
	strupper( output );
	for ( int j = 0; j < pCmd->iNumParams; j++ )
	{
		strcat( output, "_" );
		strcat( output, TypeString1(pCmd->iParamsTypes[j]) );
	}
	if ( pCmd->iNumParams == 0 ) strcat( output, "_0" );
}

int CommandComparePlugin( const void* a, const void* b )
{
	sCommand* c1 = (sCommand*) a;
	sCommand* c2 = (sCommand*) b;

	char szOutput1[ 256 ];
	GetCommandPluginName( c1, szOutput1, 256 );

	char szOutput2[ 256 ];
	GetCommandPluginName( c2, szOutput2, 256 );

	int result = strcmp( szOutput1, szOutput2 );
	if ( result == 0 ) 
	{
		char err[ 1024 ];
		sprintf( err, "Two plugin commands share the same name: %s", szOutput1 );
		Error( err );
	}

	return result;
}

int CommandVSCompare( const void* a, const void* b )
{
	sCommand* c1 = (sCommand*) a;
	sCommand* c2 = (sCommand*) b;

	char* szName1 = new char[ strlen(c1->szCommandName) + 1 ];
	char* szName2 = new char[ strlen(c2->szCommandName) + 1 ];

	strcpy( szName1, c1->szCommandName );
	strcpy( szName2, c2->szCommandName );

	strlower( szName1 );
	strlower( szName2 );

	int result = strcmp( szName1, szName2 );
	
	delete [] szName1;
	delete [] szName2;

	if ( result != 0 ) return result;
	else
	{
		// same command name, compare by num parameters
		if ( c1->iNumParams < c2->iNumParams ) return -1;
		else if ( c1->iNumParams > c2->iNumParams ) return 1;
		else 
		{
			for ( int i = 0; i < c1->iNumParams; i++ )
			{
				if ( c1->iParamsTypes[ i ] < c2->iParamsTypes[ i ] ) return -1;
				else if ( c1->iParamsTypes[ i ] > c2->iParamsTypes[ i ] ) return 1;
			}

			return 0;
		}
	}
}

bool bPause = true;

void Error( const char *str )
{
	printf( str ); printf( "\n" );
	if ( bPause ) system("pause");
	exit(1);
}

int SearchSortedArray( sCommand* pCommands, int numCommands, sCommand* pOtherCmd )
{
	int high = numCommands-1;
	int low = 0;
	
	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = CommandCompare( pCommands + mid, pOtherCmd );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else return mid;
	}

	return -1;
}

void ParseFile( sCommand* pCommands, int &numCommands, FILE *pFile )
{
	if ( !pFile ) return;

	char szPrevLine[ 1024 ]; strcpy_s( szPrevLine, 1024, "" );
	char szLine[ 1024 ]; strcpy_s( szLine, 1024, "" );

	do
	{
		strcpy_s( szPrevLine, 1024, szLine );
		if ( !fgets( szLine, 1024, pFile ) ) break;
		strtrim( szLine );

		if ( strcmp( szLine, "//****" ) == 0 ) pCommands[ numCommands ].iExtra = 0;
		else if ( strcmp( szLine, "//***2" ) == 0 ) pCommands[ numCommands ].iExtra = 1;
		else continue;

		// found a command in szPrevLine (hopefully)
		//char *szChar = strchr( szPrevLine, ' ' );
		char *szChar = strstr( szPrevLine, "agk::" );
		if ( !szChar ) continue;

		// get return type
		char szReturn[ 100 ];
		strncpy( szReturn, szPrevLine, szChar-szPrevLine );
		szReturn[ szChar-szPrevLine ] = 0;
		strtrim( szReturn );
		strcpy( pCommands[ numCommands ].szReturnTypeText, szReturn );

		int returntype = ConvertType(szReturn);
		if ( returntype < 0 ) continue;

		pCommands[ numCommands ].iReturnType = returntype;		

		char *szRemaining = szChar;
		strtrim( szRemaining );

		// get command name (with attached agk::)
		szChar = strchr( szRemaining, '(' );
		if ( !szChar ) continue;

		char szCommand[ 200 ];
		strncpy( szCommand, szRemaining, szChar-szRemaining );
		szCommand[ szChar-szRemaining ] = 0;
		strtrim( szCommand );

		//remove agk::
		char *szRealCommand = strchr( szCommand, ':' );
		if ( szRealCommand ) szRealCommand += 2;
		else szRealCommand = szCommand;

		strcpy( pCommands[numCommands].szCommandName, szRealCommand );

		// skip bracket character
		szRemaining = szChar+1;
		strtrim( szRemaining );

		int iParameters = 0;
		char szParameter[ 100 ];

		// find parameters
		bool bEnd = false;
		while ( iParameters < 32 && (szChar = strchr( szRemaining, ',' )) )
		{
			// copy parameter with its type
			strncpy( szParameter, szRemaining, szChar-szRemaining );
			szParameter[ szChar-szRemaining ] = 0;
			strtrim( szParameter );
			int type = ConvertType(szParameter);
			if ( type < 0 ) 
			{
				char pPrompt[512];
				sprintf(pPrompt,"Failed to ConvertType =%s=",szParameter);
				MessageBox ( NULL, pPrompt, szRealCommand, MB_OK );
				bEnd = true;
				break;
			}

			// param name
			const char* paramName = ConvertTypeName(szParameter);
			strcpy( pCommands[ numCommands ].szParamsName[ iParameters ], paramName );
			strtrim( pCommands[ numCommands ].szParamsName[ iParameters ] );

			// param type
			strncpy( pCommands[ numCommands ].szParamsTypeText[ iParameters ], szParameter, paramName-szParameter );
			pCommands[ numCommands ].szParamsTypeText[ iParameters ][ paramName-szParameter ] = 0;
			strtrim( pCommands[ numCommands ].szParamsTypeText[ iParameters ] );
			pCommands[ numCommands ].iParamsTypes[ iParameters ] = type;

			// skip comma
			szRemaining = szChar+1;
			strtrim( szRemaining );
			iParameters++;
		}
		if ( iParameters >= 32 )
		{
			MessageBox ( NULL, "Exceeded 32 parameters", szRealCommand, MB_OK );
			continue;
		}
		if ( bEnd ) continue;

		// find last parameter before the closing bracket
		szChar = strchr( szRemaining, ')' );
		if ( !szChar ) continue;

		strncpy( szParameter, szRemaining, szChar-szRemaining );
		szParameter[ szChar-szRemaining ] = 0;
		strtrim( szParameter );
		if ( strlen(szParameter) > 0 ) 
		{
			int type = ConvertType(szParameter);
			if ( type < 0 ) continue;

			const char* paramName = ConvertTypeName(szParameter);
			strcpy( pCommands[ numCommands ].szParamsName[ iParameters ], paramName );
			strtrim( pCommands[ numCommands ].szParamsName[ iParameters ] );

			strncpy( pCommands[ numCommands ].szParamsTypeText[ iParameters ], szParameter, paramName-szParameter );
			pCommands[ numCommands ].szParamsTypeText[ iParameters ][ paramName-szParameter ] = 0;
			strtrim( pCommands[ numCommands ].szParamsTypeText[ iParameters ] );
			pCommands[ numCommands ].iParamsTypes[ iParameters ] = type;
			iParameters++;
		}

		pCommands[ numCommands ].iNumParams = iParameters;

		// check if it already exists
		if ( SearchSortedArray( g_pSortedVSCommands, g_iNumCommands, pCommands+numCommands ) >= 0 ) continue;

		numCommands++;
		if ( numCommands >= MAX_COMMANDS ) 
		{
			MessageBox( NULL, "Exceeded max command limit", "Error", 0 );
			return;
		}

	} while( !feof(pFile) );
}

void OutputCommandTable( sCommand* pCommands, int numCommands, FILE *pFileOut )
{
	for ( int i = 0; i < numCommands; i++ )
	{
		// only output Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;

		// simple command output
		char szOutput[ 1024 ];
		sprintf( szOutput, "%s, %s, ", pCommands[i].szCommandName, TypeString1(pCommands[i].iReturnType) );
		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			strcat( szOutput, TypeString1(pCommands[i].iParamsTypes[j]) );
		}
		if ( pCommands[i].iNumParams == 0 ) strcat( szOutput, "0" );
		strcat( szOutput, "\n" );

		fputs( szOutput, pFileOut );
		fflush( pFileOut );
	}
}

void OutputCommandEnums( sCommand* pCommands, int numCommands, FILE *pFileOut )
{
	fputs( "#ifndef _H_AGKINSTRUCTIONS\n", pFileOut );
	fputs( "#define _H_AGKINSTRUCTIONS\n\n", pFileOut );
	fputs( "#define AGK_COMMANDS ", pFileOut );

	int first = 1;
	for ( int i = 0; i < numCommands; i++ )
	{
		// only output Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;

		if ( first == 0 ) fputs( ",\\\n", pFileOut );
		first = 0;

		// command enum output
		char szOutput[ 1024 ];
		sprintf( szOutput, "    AGKI_%s_%s", pCommands[i].szCommandName, TypeString1(pCommands[i].iReturnType) );
		strupper( szOutput );
		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			strcat( szOutput, "_" );
			strcat( szOutput, TypeString1(pCommands[i].iParamsTypes[j]) );
		}
		if ( pCommands[i].iNumParams == 0 ) strcat( szOutput, "_0" );
		
		fputs( szOutput, pFileOut );
		fflush( pFileOut );
	}
	fputs( "\n\n", pFileOut );
	fputs( "#endif", pFileOut );
}

void OutputCommandSwitch( sCommand* pCommands, int numCommands, FILE *pFileOut, FILE *pFileOut2D )
{
	fputs( "\n", pFileOut );

	char temp[ 32 ];
	
	for ( int i = 0; i < numCommands; i++ )
	{
		// only output Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;

		// command switch case output
		char szOutput[ 1024 ];
		char szCommandUpper[ 256 ];
		strcpy( szCommandUpper, pCommands[i].szCommandName );
		strupper( szCommandUpper );
		sprintf( szOutput, "case AGKI_%s_%s", szCommandUpper, TypeString1(pCommands[i].iReturnType) );
		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			strcat( szOutput, "_" );
			strcat( szOutput, TypeString1(pCommands[i].iParamsTypes[j]) );
		}
		if ( pCommands[i].iNumParams == 0 ) strcat( szOutput, "_0" );
		strcat( szOutput, ":\n{\n" );
		
		// pop parameters off the stack
		for ( int j = pCommands[i].iNumParams-1; j >= 0 ; j-- )
		{
			sprintf( temp, "%d", j );
			const char* type = TypeStringC(pCommands[i].iParamsTypes[j]);
			strcat( szOutput, "    " );
			if ( pCommands[i].iParamsTypes[j] == AGK_DATA_TYPE_STRING ) strcat( szOutput, "const " );
			strcat( szOutput, type );
			strcat( szOutput, " param" );
			strcat( szOutput, temp );
			
			switch( pCommands[i].iParamsTypes[j] )
			{
				case AGK_DATA_TYPE_INT: strcat( szOutput, " = m_pStack[ --m_iStackPtr ].i;\n" ); break;
				case AGK_DATA_TYPE_FLOAT: strcat( szOutput, " = m_pStack[ --m_iStackPtr ].f;\n" ); break;
				case AGK_DATA_TYPE_STRING: strcat( szOutput, " = m_pStrStack[ --m_iStrStackPtr ].GetStr();\n" ); break;
				case AGK_DATA_TYPE_ARRAY_INT: strcat( szOutput, " = ((stArray*)(m_pStack[ --m_iStackPtr ].p))->m_pI;\n" ); break;
				case AGK_DATA_TYPE_ARRAY_FLOAT: strcat( szOutput, " = ((stArray*)(m_pStack[ --m_iStackPtr ].p))->m_pF;\n" ); break;
				case AGK_DATA_TYPE_ARRAY_STRING: strcat( szOutput, " = ((stArray*)(m_pStack[ --m_iStackPtr ].p))->m_pS;\n" ); break;
				default: 
				{
					MessageBox( NULL, "Unrecognised type", "Error", 0 );
					return;
				}
			}
		}

		switch( pCommands[i].iReturnType )
		{
			case AGK_DATA_TYPE_INT: strcat( szOutput, "    m_pStack[ m_iStackPtr++ ].i = " ); break;
			case AGK_DATA_TYPE_FLOAT: strcat( szOutput, "    m_pStack[ m_iStackPtr++ ].f = " ); break;
			case AGK_DATA_TYPE_STRING: 
			{
				// temporarily store in a char*
				strcat( szOutput, "    char *szReturnStr = " ); break;
				break;
			}
			default: strcat( szOutput, "    " );
		}

		// call the function
		strcat( szOutput, "agk::" );
		strcat( szOutput, pCommands[i].szCommandName );
		strcat( szOutput, "(" );
		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			sprintf( temp, "%d", j );
			strcat( szOutput, " param" );
			strcat( szOutput, temp );
			if ( j < pCommands[i].iNumParams-1 ) strcat( szOutput, "," );
		}
		strcat( szOutput, " );\n" );

		if ( pCommands[i].iReturnType == AGK_DATA_TYPE_STRING )
		{
			// convert char* into uString
			strcat( szOutput, "    m_pStrStack[ m_iStrStackPtr++ ].SetStrUTF8( szReturnStr );\n" );
			strcat( szOutput, "    delete [] szReturnStr;\n" );
		}
		
		strcat( szOutput, "    break;\n}\n" );

		fputs( szOutput, pFileOut );
		fflush( pFileOut );

		// check whether the add it to the 2D version
		int add2D = 1;
		if ( strstr(pCommands[i].szCommandName, "Object") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "Camera") != 0 && strstr(pCommands[i].szCommandName, "DeviceCamera") == 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "Mesh") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "PointLight") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "3DPhysics") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "3dPhysics") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "3Dphysics") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "Vector3") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "3DVector") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "Global3D") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "From3D") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "SetSun") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "AmbientColor") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "From3D") != 0 ) add2D = 0;
		else if ( strstr(pCommands[i].szCommandName, "SkyBox") != 0 ) add2D = 0;
		else if ( strncmp(pCommands[i].szCommandName, "SetFog", 6) == 0 ) add2D = 0;
		else if ( strncmp(pCommands[i].szCommandName, "GetFog", 6) == 0 ) add2D = 0;

		if ( add2D ) 
		{
			fputs( szOutput, pFileOut2D );
			fflush( pFileOut2D );
		}
	}
	fputs( "\n\n", pFileOut );
}

void OutputCommandPluginInterpreter( sCommand* pCommands, int numCommands, FILE *pFileOut )
{
	fputs( "#ifndef _H_AGK_PLUGIN_COMMANDS\n#define _H_AGK_PLUGIN_COMMANDS\n\n", pFileOut );
	fputs( "typedef void(*AGKVoidFunc)(void);\n\n", pFileOut );
	fputs( "struct AGKPluginFunction\n{\n\tconst char *szName;\n\tAGKVoidFunc pFunc;\n};\n\n", pFileOut );

	fprintf( pFileOut, "int g_iNumAGKFunctions = %d;\n", numCommands );
	fputs( "AGKPluginFunction g_ppAGKFunctionList[] = {\n", pFileOut );

	for ( int i = 0; i < numCommands; i++ )
	{
		// include all commands

		char szName[ 256 ];
		GetCommandPluginName( pCommands+i, szName, 256 );

		char szParameters[ 256 ];
		strcpy( szParameters, "(" );
		strcat( szParameters, pCommands[i].szReturnTypeText );
		strcat( szParameters, "(*)(" );

		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			if ( j > 0 ) strcat( szParameters, "," );
			strcat( szParameters, pCommands[i].szParamsTypeText[j] );
		}
		strcat( szParameters, "))" );

		fprintf( pFileOut, "\t{\"%s\", (AGKVoidFunc) %s (agk::%s) }", szName, szParameters, pCommands[i].szCommandName );
		if ( i < numCommands-1 ) fputs( ",", pFileOut );
		fputs( "\n", pFileOut );
	}
	fputs( "};\n\n", pFileOut );

	fputs( "AGKVoidFunc GetAGKFunction( const char* func )\n\
{\n\
	if ( !func )\n\
	{\n\
		agk::SetErrorMode( 2 );\n\
		agk::Error( \"Plugin tried to request AGK function with a null pointer\" );\n\
	}\n\
	int high = g_iNumAGKFunctions-1;\n\
	int low = 0;\n\
	while( high >= low )\n\
	{\n\
		int mid = (high+low)/2;\n\
		int result = strcmp( g_ppAGKFunctionList[mid].szName, func );\n\
		if( result > 0 ) high = mid-1;\n\
		else if ( result < 0 ) low = mid+1;\n\
		else return g_ppAGKFunctionList[mid].pFunc;\n\
	}\n\
	if ( strcmp( func, \"GETSOCKETLISTNERCONNECTION_L_L\" ) == 0 ) return (AGKVoidFunc) agk::GetSocketListenerConnection;\n\
	if ( strcmp( func, \"SET3DPHYSICSRADOLLBONESVISIBLE_0_L_L\" ) == 0 ) return (AGKVoidFunc)  (void(*)(UINT,int)) (agk::Set3DPhysicsRagdollBonesVisible);\n\
	if ( strcmp( func, \"SET3DPHYSICSHINGEJOINTMAXMOTORIMPLUSE_0_L_F\" ) == 0 ) return (AGKVoidFunc) (void(*)(UINT,float)) (agk::Set3DPhysicsHingeJointMaxMotorImpulse);\n\
	if ( strcmp( func, \"SET3DPHYSICSTWISTJOINTMAXMOTORIMPLUSE_0_L_F\" ) == 0 ) return (AGKVoidFunc) (void(*)(UINT,float)) (agk::Set3DPhysicsTwistJointMaxMotorImpulse);\n\
	\n\
	agk::SetErrorMode( 2 );\n\
	uString err;\n\
	err.Format( \"Plugin tried to request AGK function that doesn't exist: %s\", func );\n\
	agk::Error( err );\n\
	return 0;\n\
}\n\n", pFileOut );

	fputs( "#endif\n", pFileOut );
}

void OutputCommandPluginExternal( sCommand* pCommands, int numCommands, FILE *pFileOut, FILE *pFileOut2 )
{
	// header
	fputs( "#ifndef _H_AGK_LIBRARY_COMMANDS\n#define _H_AGK_LIBRARY_COMMANDS\n\n", pFileOut );
	fputs( "\
#ifdef _MSC_VER\n\
	#define DLL_EXPORT __declspec(dllexport)\n\
#else\n\
	#pragma GCC visibility push(hidden)\n\
	#define DLL_EXPORT __attribute__((visibility(\"default\")))\n\
#endif\n\n", pFileOut );
	fputs( "class cImage;\nclass cSprite;\n\n", pFileOut );

	// write raw function pointers, names must be unique
	for ( int i = 0; i < numCommands; i++ )
	{
		char szParameters[ 256 ];
		*szParameters = 0;
		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			if ( j > 0 ) strcat( szParameters, ", " );
			if ( strcmp(pCommands[i].szParamsTypeText[j], "UINT") == 0 || strcmp(pCommands[i].szParamsTypeText[j], "uint32_t") == 0 )
				strcat( szParameters, "unsigned int" );
			else
				strcat( szParameters, pCommands[i].szParamsTypeText[j] );
		}

		const char* szReturn = pCommands[i].szReturnTypeText;
		if ( strcmp( szReturn, "UINT" ) == 0 || strcmp( szReturn, "uint32_t" ) == 0 ) szReturn = "unsigned int";
		fprintf( pFileOut, "extern %s(*AGKCommand%d)( %s );\n", szReturn, i, szParameters );
	}
	fputs( "\n", pFileOut );

	// write agk class that uses the function pointers
	fputs( "class agk\n{\n\tpublic:\n", pFileOut );
	for ( int i = 0; i < numCommands; i++ )
	{
		char szParameters[ 256 ];
		char szParameters2[ 256 ];
		*szParameters = 0;
		*szParameters2 = 0;
		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			if ( j > 0 ) 
			{
				strcat( szParameters, ", " );
				strcat( szParameters2, ", " );
			}

			if ( strcmp(pCommands[i].szParamsTypeText[j], "UINT") == 0 || strcmp(pCommands[i].szParamsTypeText[j], "uint32_t") == 0 )
				strcat( szParameters, "unsigned int" );
			else
				strcat( szParameters, pCommands[i].szParamsTypeText[j] );

			strcat( szParameters, " " );
			strcat( szParameters, pCommands[i].szParamsName[j] );

			strcat( szParameters2, pCommands[i].szParamsName[j] );
		}

		const char* szReturnType = pCommands[i].szReturnTypeText;
		if ( strcmp( szReturnType, "UINT" ) == 0 || strcmp( szReturnType, "uint32_t" ) == 0 ) szReturnType = "unsigned int";
		const char *szReturn = "";
		if ( strcmp(szReturnType,"void") != 0 ) szReturn = "return ";
		fprintf( pFileOut, "\t\tstatic inline %s %s( %s ) { %sAGKCommand%d( %s ); }\n", szReturnType, pCommands[i].szCommandName, szParameters, szReturn, i, szParameters2 );
	}
	fputs( "};\n\n#endif\n", pFileOut );

	// cpp file
	fputs( "#include \"AGKLibraryCommands.h\"\n\n", pFileOut2 );

	// write raw function pointers, names must be unique
	for ( int i = 0; i < numCommands; i++ )
	{
		char szParameters[ 256 ];
		*szParameters = 0;
		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			if ( j > 0 ) strcat( szParameters, ", " );
			if ( strcmp(pCommands[i].szParamsTypeText[j], "UINT") == 0 || strcmp(pCommands[i].szParamsTypeText[j], "uint32_t") == 0 )
				strcat( szParameters, "unsigned int" );
			else
				strcat( szParameters, pCommands[i].szParamsTypeText[j] );
		}

		const char* szReturn = pCommands[i].szReturnTypeText;
		if ( strcmp( szReturn, "UINT" ) == 0 || strcmp( szReturn, "uint32_t" ) == 0 ) szReturn = "unsigned int";
		fprintf( pFileOut2, "%s(*AGKCommand%d)( %s ) = 0;\n", szReturn, i, szParameters );
	}
	fputs( "\n", pFileOut2 );

	fputs( "typedef void(*AGKVoidFunc)(void);\n", pFileOut2 );
	fputs( "AGKVoidFunc(*GetAGKFunction)( const char* ) = 0;\n\n", pFileOut2 );
	fputs( "extern \"C\" DLL_EXPORT void ReceiveAGKPtr( AGKVoidFunc ptr )\n{\n\
	GetAGKFunction = (AGKVoidFunc(*)(const char*)) ptr;\n\n", pFileOut2 );

	for ( int i = 0; i < numCommands; i++ )
	{
		char szName[ 256 ];
		GetCommandPluginName( pCommands+i, szName, 256 );

		char szParameters[ 256 ];
		strcpy( szParameters, "(" );
		const char* szReturnType = pCommands[i].szReturnTypeText;
		if ( strcmp( szReturnType, "UINT" ) == 0 || strcmp( szReturnType, "uint32_t" ) == 0 ) szReturnType = "unsigned int";
		strcat( szParameters, szReturnType );
		strcat( szParameters, "(*)(" );

		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			if ( j > 0 ) strcat( szParameters, "," );

			if ( strcmp(pCommands[i].szParamsTypeText[j], "UINT") == 0 || strcmp(pCommands[i].szParamsTypeText[j], "uint32_t") == 0 )
				strcat( szParameters, "unsigned int" );
			else
				strcat( szParameters, pCommands[i].szParamsTypeText[j] );
		}
		strcat( szParameters, "))" );

		fprintf( pFileOut2, "\tAGKCommand%d = %s GetAGKFunction( \"%s\" );\n", i, szParameters, szName );
	}
	fputs( "}\n", pFileOut2 );
}	

void OutputKeywords( sCommand* pCommands, int numCommands, FILE *pFileOut )
{
	for ( int i = 0; i < numCommands; i++ )
	{
		// only output Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;

		char szLower[ 200 ];
		strcpy( szLower, pCommands[i].szCommandName );
		strlower( szLower );
		fputs( szLower, pFileOut );
		fputc( ' ', pFileOut );
		fflush( pFileOut );
	}
}

void OutputCommandTags( sCommand* pCommands, int numCommands, FILE *pFileOut )
{
	fputs( "String|String|(String Data Type)|\n", pFileOut );

	for ( int i = 0; i < numCommands; i++ )
	{
		// only output Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;

		char szOutput[ 1024 ];
		sprintf( szOutput, "%s|%s|(", pCommands[i].szCommandName, TypeString2(pCommands[i].iReturnType) );
		for ( int j = 0; j < pCommands[i].iNumParams; j++ )
		{
			//if ( j == 0 ) strcat( " " );
			strcat( szOutput, TypeString2(pCommands[i].iParamsTypes[j]) );
			strcat( szOutput, " " );
			strcat( szOutput, pCommands[i].szParamsName[j] );
			if ( j < pCommands[i].iNumParams-1 ) strcat( szOutput, ", " );
		}
		strcat( szOutput, ")|\n" );

		fputs( szOutput, pFileOut );
		fflush( pFileOut );
	}
}

void OutputIDECommands( sCommand* pCommands, int numCommands, FILE *pFileOut )
{
	fputs( "static const char* const agk_identifiers[] = { \n", pFileOut );

	char command[ 1024 ];
	int lineLength = 0;
	for ( int i = 0; i < numCommands; i++ )
	{
		// only output Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;

		int length = (int) strlen(pCommands[i].szCommandName);
		lineLength += length;

		strcpy_s( command, pCommands[i].szCommandName );
		strlower( command );

		fwrite( "\"", 1, 1, pFileOut );
		fwrite( command, 1, length, pFileOut );
		fwrite( "\", ", 1, 3, pFileOut );

		if ( lineLength > 150 )
		{
			lineLength = 0;
			fwrite( "\n", 1, 1, pFileOut );
		}
	}

	fwrite( "\n};", 1, 3, pFileOut );
	fflush( pFileOut );
}

void OutputVSCodeCommands( sCommand* pCommands, int numCommands, FILE *pFileOut )
{
	fputs( "struct sAGKCommand {\n", pFileOut );
	fputs( "	const char* szCommandNameLow;\n", pFileOut );
	fputs( "	const char* szCommandName;\n", pFileOut );
	fputs( "	const char* szParamsName[ 32 ];\n", pFileOut );
	fputs( "	unsigned char iParamsTypes[ 32 ];\n", pFileOut );
	fputs( "	unsigned char iReturnType;\n", pFileOut );
	fputs( "	unsigned char iNumParams;\n", pFileOut );
	fputs( "};\n\n", pFileOut );

	fputs( "static const sAGKCommand agk_commands[] = { \n\t", pFileOut );

	char command[ 1024 ];
	for ( int i = 0; i < numCommands; i++ )
	{
		// only output Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;
		
		if ( i > 0 ) fputs( ",\n\t", pFileOut );
		fputs( "{\"", pFileOut );
		strcpy_s( command, pCommands[i].szCommandName );
		strlower( command );
		fputs( command, pFileOut );
		fputs( "\",", pFileOut );

		fputs( "\"", pFileOut );
		fputs( pCommands[i].szCommandName, pFileOut );
		fputs( "\",{", pFileOut );

		for (int j = 0; j < pCommands[i].iNumParams; j++)
		{
			if ( j > 0 ) fputs( ",", pFileOut );
			fputs( "\"", pFileOut );
			fputs( pCommands[i].szParamsName[j], pFileOut );
			fputs( "\"", pFileOut );
		}

		fputs( "},{", pFileOut );

		for (int j = 0; j < pCommands[i].iNumParams; j++)
		{
			if ( j > 0 ) fputs( ",", pFileOut );
			fprintf( pFileOut, "%d", pCommands[i].iParamsTypes[j] );
		}

		fputs( "},", pFileOut );
		fprintf( pFileOut, "%d,%d}", pCommands[i].iReturnType, pCommands[i].iNumParams );
	}

	fwrite( "\n};", 1, 3, pFileOut );
	fflush( pFileOut );
}

void SaveArray( sCommand *pCommands, int numCommands, FILE *pFile )
{
	fwrite( &numCommands, 4, 1, pFile ); // num commands

	for( int i = 0; i < numCommands; i++ )
	{
		// command name
		int length = (int) strlen(pCommands[i].szCommandName);
		fwrite( &length, 4, 1, pFile ); // name length
		fwrite( pCommands[i].szCommandName, 1, strlen(pCommands[i].szCommandName), pFile ); // name

		// return type
		fwrite( &(pCommands[i].iReturnType), 1, 1, pFile );
		length = (int) strlen(pCommands[i].szReturnTypeText);
		fwrite( &length, 4, 1, pFile );
		fwrite( pCommands[i].szReturnTypeText, 1, strlen(pCommands[i].szReturnTypeText), pFile );
		
		// parameters
		fwrite( &(pCommands[i].iNumParams), 1, 1, pFile );
		for ( int p = 0; p < pCommands[i].iNumParams; p++ )
		{
			fwrite( &(pCommands[i].iParamsTypes[p]), 1, 1, pFile ); // param type
			int length = (int) strlen(pCommands[i].szParamsName[p]);
			fwrite( &length, 4, 1, pFile ); // param name length
			fwrite( pCommands[i].szParamsName[p], 1, strlen(pCommands[i].szParamsName[p]), pFile ); // param name
			length = (int) strlen(pCommands[i].szParamsTypeText[p]);
			fwrite( &length, 4, 1, pFile ); 
			fwrite( pCommands[i].szParamsTypeText[p], 1, strlen(pCommands[i].szParamsTypeText[p]), pFile );
		}

		// extra
		fwrite( &(pCommands[i].iExtra), 1, 1, pFile );
	}
}

void SaveArrayCompiler( sCommand *pCommands, int numCommands, FILE *pFile )
{
	int count = 0;
	for( int i = 0; i < numCommands; i++ )
	{
		// only include Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;
		count++;
	}

	fwrite( &count, 4, 1, pFile ); // num commands

	for( int i = 0; i < numCommands; i++ )
	{
		// only include Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;

		// command name
		int length = (int) strlen(pCommands[i].szCommandName);
		fwrite( &length, 4, 1, pFile ); // name length
		fwrite( pCommands[i].szCommandName, 1, strlen(pCommands[i].szCommandName), pFile ); // name

		// return type
		fwrite( &(pCommands[i].iReturnType), 1, 1, pFile );
		
		// parameters
		fwrite( &(pCommands[i].iNumParams), 1, 1, pFile );
		for ( int p = 0; p < pCommands[i].iNumParams; p++ )
		{
			fwrite( &(pCommands[i].iParamsTypes[p]), 1, 1, pFile ); // param type
		}
	}
}

void SaveArrayCompilerSource( sCommand *pCommands, int numCommands, FILE *pFile )
{
	int count = 0;
	for( int i = 0; i < numCommands; i++ )
	{
		// only include Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;
		count++;
	}

	fprintf( pFile, "int g_iAGKNumCommands = %d;\n", count );
	fprintf( pFile, "stCommandOrig g_pAGKCommands[%d] = {\n\t", count );

	count = 0;
	for( int i = 0; i < numCommands; i++ )
	{
		// only include Tier 1 commands
		if ( pCommands[i].iExtra != 0 ) continue;
		count++;

		if ( count > 1 ) fputs( ",\n\t", pFile );

		fprintf( pFile, "{\"%s\",{", pCommands[i].szCommandName );
		
		for ( int p = 0; p < pCommands[i].iNumParams; p++ )
		{
			if ( p == 0 ) fprintf( pFile, "%d", pCommands[i].iParamsTypes[p] );
			else fprintf( pFile, ",%d", pCommands[i].iParamsTypes[p] );
		}
		
		fprintf( pFile, "},%d,%d}", pCommands[i].iReturnType, pCommands[i].iNumParams );		
	}

	fputs( "\n};", pFile );
}

bool LoadArray( sCommand *pCommands, int *numCommands, FILE *pFile )
{
	fread( numCommands, 4, 1, pFile );
	if ( *numCommands >= MAX_COMMANDS ) 
	{
		MessageBox( NULL, "Failed to load commands array, too many commands", "Error", 0 );
		return false;
	}

	for( int i = 0; i < *numCommands; i++ )
	{
		int length = 0;

		// command name
		fread( &length, 4, 1, pFile );
		if ( length >= 199 )
		{
			MessageBox( NULL, "Failed to load commands array, command name too long", "Error", 0 );
			return false;
		}
		fread( pCommands[i].szCommandName, 1, length, pFile );
		pCommands[i].szCommandName[ length ] = 0;

		// return type
		fread( &(pCommands[i].iReturnType), 1, 1, pFile );
		fread( &length, 4, 1, pFile );
		if ( length >= 99 )
		{
			MessageBox( NULL, "Failed to load commands array, return type text too long", "Error", 0 );
			return false;
		}
		fread( pCommands[i].szReturnTypeText, 1, length, pFile );
		pCommands[i].szReturnTypeText[ length ] = 0;

		// num parameters
		fread( &(pCommands[i].iNumParams), 1, 1, pFile ); 
		if ( pCommands[i].iNumParams > 32 )
		{
			MessageBox( NULL, "Failed to load commands array, too many parameters", "Error", 0 );
			return false;
		}

		// parameters
		for ( int p = 0; p < pCommands[i].iNumParams; p++ )
		{
			fread( &(pCommands[i].iParamsTypes[p]), 1, 1, pFile ); 

			// parameter name
			int length = 0;
			fread( &length, 4, 1, pFile );
			if ( length >= 99 )
			{
				MessageBox( NULL, "Failed to load commands array, parameter name too long", "Error", 0 );
				return false;
			}
			fread( pCommands[i].szParamsName[p], 1, length, pFile );
			pCommands[i].szParamsName[p][ length ] = 0;

			// parameter type text
			fread( &length, 4, 1, pFile );
			if ( length >= 99 )
			{
				MessageBox( NULL, "Failed to load commands array, parameter type text too long", "Error", 0 );
				return false;
			}
			fread( pCommands[i].szParamsTypeText[p], 1, length, pFile );
			pCommands[i].szParamsTypeText[p][ length ] = 0;
		}

		// extra
		fread( &(pCommands[i].iExtra), 1, 1, pFile );
	}

	return true;
}

int main(int argc, char* argv[])
{
	if ( argc > 1 && strcmp(argv[1], "nopause") == 0 ) bPause = false;

	g_iNumCommands = 0;
	
	// load existing command table
	FILE* pOldFile = fopen( "CommandList.dat", "rb" );
	if ( pOldFile )
	{
		if ( !LoadArray( g_pCommands, &g_iNumCommands, pOldFile ) ) return 0;

		for ( int i = 0; i < g_iNumCommands; i++ )
		{
			g_pSortedVSCommands[ i ] = g_pCommands[ i ];
		}

		// sort old commands, to check if they already exist when parsing files
		qsort( g_pSortedVSCommands, g_iNumCommands, sizeof(sCommand), CommandCompare );

		fclose( pOldFile );
	}

	int newNumCommands = g_iNumCommands;

	// Wrapper.cpp input
	FILE *pFile = fopen( "..\\..\\common\\source\\Wrapper.cpp", "r" );
	if ( !pFile )
	{
		Error( "Failed to open Wrapper.cpp" );
	}

	ParseFile( g_pCommands, newNumCommands, pFile );
	fclose( pFile );

	// WindowsCore.cpp input
	pFile = fopen( "..\\..\\platform\\windows\\source\\WindowsCore.cpp", "r" );
	if ( !pFile )
	{
		Error( "Failed to open WindowsCore.cpp" );
	}

	ParseFile( g_pCommands, newNumCommands, pFile );
	fclose( pFile );

	// OpenGL2.cpp input
	pFile = fopen( "..\\..\\renderer\\OpenGL2\\OpenGL2.cpp", "r" );
	if ( !pFile )
	{
		Error( "Failed to open OpenGL2.cpp" );
	}

	ParseFile( g_pCommands, newNumCommands, pFile );
	fclose( pFile );

	g_iNumCommands = newNumCommands;

	// ****************
	// do not parse new files after this
	// ****************

	// sort all commands, including new ones
	for ( int i = 0; i < g_iNumCommands; i++ ) 
	{
		g_pSortedVSCommands[ i ] = g_pCommands[ i ];
		g_pSortedPluginCommands[ i ] = g_pCommands[ i ];
	}
	qsort( g_pSortedPluginCommands, g_iNumCommands, sizeof(sCommand), CommandComparePlugin );
	qsort( g_pSortedVSCommands, g_iNumCommands, sizeof(sCommand), CommandVSCompare );

	// output enum command list
	FILE *pFileOut = fopen( "../../apps/interpreter/AGKCommandEnums.h", "w" );
	if ( !pFileOut )
	{
		Error( "Failed to open AGKCommandEnums.h" );
	}
	
	OutputCommandEnums( g_pCommands, g_iNumCommands, pFileOut );
	fclose( pFileOut );

	// output interpreter switch statement list
	pFileOut = fopen( "../../apps/interpreter/AGKCommandSwitch.h", "w" );
	if ( !pFileOut )
	{
		Error( "Failed to open AGKCommandSwitch.h" );
	}

	FILE *pFileOut2D = fopen( "../../apps/interpreter/AGKCommandSwitch2D.h", "w" );
	if ( !pFileOut2D )
	{
		Error( "Failed to open AGKCommandSwitch2D.h" );
	}
	
	OutputCommandSwitch( g_pCommands, g_iNumCommands, pFileOut, pFileOut2D );
	fclose( pFileOut );
	fclose( pFileOut2D );

	// output interpreter plugin commands
	pFileOut = fopen( "../../apps/interpreter/AGKPluginCommandList.h", "w" );
	if ( !pFileOut )
	{
		Error( "Failed to open AGKPluginCommandList.h" );
	}
	OutputCommandPluginInterpreter( g_pSortedPluginCommands, g_iNumCommands, pFileOut );
	fclose( pFileOut );

	// output DLL plugin commands
	pFileOut = fopen( "../../plugins/Source/AGKLibraryCommands.h", "w" );
	if ( !pFileOut )
	{
		Error( "Failed to open AGKLibraryCommands.h" );
	}
	FILE *pFileOut2 = fopen( "../../plugins/Source/AGKLibraryCommands.cpp", "w" );
	if ( !pFileOut2 )
	{
		Error( "Failed to open AGKLibraryCommands.cpp" );
	}
	OutputCommandPluginExternal( g_pCommands, g_iNumCommands, pFileOut, pFileOut2 );
	fclose( pFileOut );
	fclose( pFileOut2 );

	/*
	// read keyword template files
	pFile = fopen( "../../IDE/filetypes.agk1", "rb" );
	if ( !pFile )
	{
		Error( "Failed to open filetypes.agk1" );
	}

	fpos_t pos = 0;
	fgetpos( pFile, &pos );
	fseek( pFile, 0, SEEK_END );
	UINT size1 = ftell( pFile );
	fsetpos( pFile, &pos );

	char* szKeywordBegin = new char[ size1 + 1 ];
	fread( szKeywordBegin, 1, size1, pFile );
	szKeywordBegin[ size1 ] = 0;
	fclose( pFile );

	// and the second half
	pFile = fopen( "../../IDE/filetypes.agk2", "rb" );
	if ( !pFile )
	{
		Error( "Failed to open filetypes.agk2" );
	}

	pos = 0;
	fgetpos( pFile, &pos );
	fseek( pFile, 0, SEEK_END );
	UINT size2 = ftell( pFile );
	fsetpos( pFile, &pos );

	char* szKeywordEnd = new char[ size2 + 1 ];
	fread( szKeywordEnd, 1, size2, pFile );
	szKeywordEnd[ size2 ] = 0;
	fclose( pFile );

	// output IDE keywords
	pFileOut = fopen( "../../IDE/Geany-1.24.1/data/filetypes.agk", "wb" );
	if ( !pFileOut )
	{
		Error( "Failed to open keywords.txt" );
	}
	
	fwrite( szKeywordBegin, 1, size1, pFileOut );
	OutputKeywords( g_pCommands, g_iNumCommands, pFileOut );
	fwrite( szKeywordEnd, 1, size2, pFileOut );
	fclose( pFileOut );

	delete [] szKeywordBegin;
	delete [] szKeywordEnd;
	*/

	// output IDE tags
	pFileOut = fopen( "../../AgkIde/media/main.agc.tags", "w" );
	if ( !pFileOut )
	{
		Error( "Failed to open main.agc.tags" );
	}
	
	OutputCommandTags( g_pCommands, g_iNumCommands, pFileOut );
	fclose( pFileOut );

	// output IDE commands
	pFileOut = fopen( "../../AgkIde/AGKCommands.h", "wb" );
	if ( !pFileOut )
	{
		Error( "Failed to open AGKCommands.h" );
	}

	OutputIDECommands( g_pCommands, g_iNumCommands, pFileOut );
	fclose( pFileOut );

	// output VSCode language server commands
	pFileOut = fopen( "../../VSCode/Language Server Protocol/AGKCommands.h", "wb" );
	if ( !pFileOut )
	{
		Error( "Failed to open language server AGKCommands.h" );
	}
	
	OutputVSCodeCommands( g_pSortedVSCommands, g_iNumCommands, pFileOut );
	fclose( pFileOut );

	// save command list for next run
	FILE* pNewFile = fopen( "CommandList.dat", "wb" );
	SaveArray( g_pCommands, g_iNumCommands, pNewFile );
	fclose( pNewFile );

	// save command list for compiler
	pNewFile = fopen( "../../CompilerNew/CommandList.h", "wb" );
	SaveArrayCompilerSource( g_pCommands, g_iNumCommands, pNewFile );
	fclose( pNewFile );

	// finish
	if ( bPause ) system("pause");
	return 0;
}

