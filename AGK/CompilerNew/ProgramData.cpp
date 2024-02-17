#include "AGKCompiler2.h"
#include "ProgramData.h"
#include "cFile.h"

#include "CommandList.h"

using namespace AGK_Compiler;

int cProgramData::m_iNumCommands = 0;
stCommand* cProgramData::m_pCommands = 0;


const char* getDataTypeName( int var )
{
	var = var & 0x1f;
	switch( var )
	{
		case 0: return "Void";
		case AGK_VARIABLE_INTEGER: return "Integer";
		case AGK_VARIABLE_FLOAT: return "Float";
		case AGK_VARIABLE_STRING: return "String";
		case AGK_VARIABLE_TYPE: return "Type";
		case AGK_VARIABLE_ARRAY_INT: return "Integer Array";
		case AGK_VARIABLE_ARRAY_FLOAT: return "Float Array";
		case AGK_VARIABLE_ARRAY_STRING: return "String Array";
		case AGK_VARIABLE_ARRAY_TYPE: return "Type Array";
		default: Error( "Unrecognised data type", 0,0 ); return "";
	}
}

int cProgramData::AddIncludeFile( const char* filename, int type, uString *error )
{
	uString sFileOrig( filename );
	sFileOrig.Replace( '\\', '/' );

	uString sFile( sFileOrig );
	sFile.Lower();	

	for ( int i = 0; i < m_pIncludeFiles.m_iCount; i++ )
	{
		if ( strcmp( m_pIncludeFiles.m_pData[ i ]->sName.GetStr(), sFile ) == 0 )
		{
			//if ( error ) error->Format( "Include File %s has already been included", filename );
			return 0x01000000 | i; // use most significant byte as a flag for already included
		}
	}

	stIncludeFile newFile;
	newFile.sName.SetStr( sFile );
	newFile.sNameOrig.SetStr( sFileOrig );
	newFile.type = type;
	int index = m_pIncludeFiles.Add( &newFile );

	return index;
}

cProgramData::cProgramData( int size ) : m_pFunctions( 128 ), m_pVariables( 512 ), m_pTypes( 128 ), m_pLabels( 128 )
{
	m_iRendererMode = 0; // AGK_RENDERER_MODE_PREFER_BEST
	m_iFlags = 0;

	m_iNumTokens = 0;
	m_iSizeTokens = 0;
	m_pTokens = 0;
	
	if ( size > 0 )
	{
		m_pTokens = new stToken*[ size ];
		memset( m_pTokens, 0, sizeof(stToken*) * size );
		m_iSizeTokens = size;
	}

	m_pFunctions.SetSorted( true, CompareFunction );
	m_pVariables.SetSorted( true, CompareVariable );
	m_pTypes.SetSorted( true, CompareType );
	m_pLabels.SetSorted( true, CompareLabel );
	m_pConstants.SetSorted( true, CompareConstant );
	m_pStringTable.SetSorted( true, CompareString );

	m_pPlugins.SetSorted( true, ComparePlugin );
	m_pPluginRefs.SetSorted( true, ComparePluginRef );

	m_pModules.SetSorted( true, CompareModule );
	m_pModuleRefs.SetSorted( true, CompareModuleRef );

	m_iCountInts = 0;
	m_iCountFloats = 0;
	m_iCountStrings = 0;
	m_iCountTypes = 0;
	m_iCountArrays = 0;

	m_iOptionExplicit = 0;
	m_iEntryPoint = 0;
}

cProgramData::~cProgramData()
{
	if (m_pMemTokenBlocks)
	{
		for (uint32_t i = 0; i < m_iMemNumBlocks; i++)
		{
			delete [] m_pMemTokenBlocks[ i ];
		}
		delete [] m_pMemTokenBlocks;
	}
	else
	{
		if ( m_pTokens ) 
		{
			for( unsigned int i = 0; i < m_iSizeTokens; i++ ) 
			{
				if ( m_pTokens[ i ] ) delete m_pTokens[ i ];
			}
		}
	}

	if ( m_pTokens ) delete [] m_pTokens;
}

void cProgramData::SetSkipInstructions( int skip )
{
	m_iSkipInstructions = skip;
}

stToken* cProgramData::NewToken()
{
	//return new stToken();

	if ( m_pMemTokenBlocks == 0 || m_iMemCurrBlockTotal >= iMaxTokensPerBlock )
	{
		stToken** pNewBlocks = new stToken*[ m_iMemNumBlocks + 1 ];
		if ( m_pMemTokenBlocks ) 
		{
			memcpy( pNewBlocks, m_pMemTokenBlocks, sizeof(stToken*) * m_iMemNumBlocks );
			delete [] m_pMemTokenBlocks;
		}

		pNewBlocks[ m_iMemNumBlocks ] = new stToken[ iMaxTokensPerBlock ];

		m_pMemTokenBlocks = pNewBlocks;
		m_iMemNumBlocks++;
		m_iMemCurrBlockTotal = 0;
	}

	stToken* pNewToken = &(m_pMemTokenBlocks[ m_iMemNumBlocks-1 ][ m_iMemCurrBlockTotal ]);
	m_iMemCurrBlockTotal++;

	return pNewToken;
}

void cProgramData::TokenError( const char* msg, int tokenID )
{
    const char* szInclude = m_pIncludeFiles.m_pData[ GetToken( tokenID )->iIncludeFile ]->sNameOrig.GetStr();
	int lineNum = GetToken( tokenID )->iLine;
	Error( msg, lineNum, szInclude );
}

void cProgramData::TokenWarning( const char* msg, int tokenID )
{
	const char* szInclude = m_pIncludeFiles.m_pData[ GetToken( tokenID )->iIncludeFile ]->sNameOrig.GetStr();
	int lineNum = GetToken( tokenID )->iLine;
	Warning( msg, lineNum, szInclude );
}

int cProgramData::AddToken( AGKToken type, const char* raw, const char* rawOrig, int line, int includeFileID, uint32_t charStart, uint32_t charEnd )
{
	if ( m_iNumTokens+1 > m_iSizeTokens )
	{
		unsigned int newSize = m_iSizeTokens * 3 / 2;
		if ( newSize < 50000 ) newSize = 50000;
		stToken **pNewArray = new stToken*[ newSize ];
		uint32_t oldBytes = sizeof(stToken*) * m_iSizeTokens;
		uint32_t newBytes = sizeof(stToken*) * newSize;
		memcpy( pNewArray, m_pTokens, oldBytes );
		memset( pNewArray + m_iSizeTokens, 0, newBytes-oldBytes );

		delete [] m_pTokens;
		m_pTokens = pNewArray;
		m_iSizeTokens = newSize;
	}

	if ( m_pTokens[ m_iNumTokens ] == 0 ) m_pTokens[ m_iNumTokens ] = NewToken();

	GetToken( m_iNumTokens )->iIncludeFile = includeFileID & 0xFFFFFF;
	GetToken( m_iNumTokens )->iLine = line;
	GetToken( m_iNumTokens )->iCharStart = charStart;
	GetToken( m_iNumTokens )->iCharEnd = charEnd;
	GetToken( m_iNumTokens )->iType = type;
	if ( type == AGK_TOKEN_INTEGER ) 
	{
		if ( *raw == '%' ) GetToken( m_iNumTokens )->iValueInt = (int) strtoul( raw+1, 0, 2 ); // binary number
		else if ( *raw == '0' && *(raw+1) == 'x' ) GetToken( m_iNumTokens )->iValueInt = (int) strtoul( raw+2, 0, 16 ); // hex number
		else GetToken( m_iNumTokens )->iValueInt = atoi( raw ); // base 10
	}
	else if ( type == AGK_TOKEN_FLOAT ) GetToken( m_iNumTokens )->fValueFloat = (float) atof( raw );

	// no "else if" here, sValueRaw must always be set
	if ( type == AGK_TOKEN_STRING ) 
	{
		GetToken( m_iNumTokens )->sValueRaw.SetStr( raw+1 );

		if ( *raw == '\'' )
		{
			GetToken( m_iNumTokens )->sValueRaw.ReplaceStr( "\\'", "'" );
			GetToken( m_iNumTokens )->sValueRaw.ReplaceStr( "\\n", "\n" );
			GetToken( m_iNumTokens )->sValueRaw.ReplaceStr( "\\r", "\r" );
			GetToken( m_iNumTokens )->sValueRaw.ReplaceStr( "\\t", "\t" );
			GetToken( m_iNumTokens )->sValueRaw.ReplaceStr( "\\\\", "\\" );
		}

		int index = AddString( GetToken( m_iNumTokens )->sValueRaw.GetStr() );
		if ( index < 0 ) 
		{
			TokenError( "Failed to add string to string table", m_iNumTokens );
			return m_iNumTokens-1;
		}
		GetToken( m_iNumTokens )->iStringIndex = index;
	}
	else if ( type == AGK_TOKEN_EOL ) 
	{
		GetToken( m_iNumTokens )->sValueRaw.SetStr( "End Of Line" );
	}
	else GetToken( m_iNumTokens )->sValueRaw.SetStr( raw );

	if ( rawOrig ) GetToken( m_iNumTokens )->sValueRawOrig.SetStr( rawOrig );

	// remove colon at the end of the label
	if ( type == AGK_TOKEN_LABEL ) 
	{
		GetToken( m_iNumTokens )->sValueRaw.Trunc2(1);
		GetToken( m_iNumTokens )->sValueRawOrig.Trunc2(1);
	}

	m_iNumTokens++;

	return m_iNumTokens-1;
}

int cProgramData::AddLabel( const char *raw, int tokenID )
{
	stLabel newLabel;
	newLabel.sNameOrig.SetStr( raw );
	newLabel.sName.SetStr( raw );
	newLabel.sName.Lower();
	newLabel.iTokenID = tokenID;
	newLabel.iInstructionPtr = 0;
	return m_pLabels.Add( &newLabel, false );
}

int cProgramData::FindLabel( const char *name )
{
	stLabel newLabel;
	newLabel.sName.SetStr( name );
	return m_pLabels.Find( &newLabel );
}

int cProgramData::AddFunction( const char *raw, int tokenID )
{
	stFunction newFunc;
	newFunc.sNameOrig.SetStr( raw );
	newFunc.sName.SetStr( raw );
	newFunc.sName.Lower();
	newFunc.iTokenID = tokenID;
	return m_pFunctions.Add( &newFunc, false );
}

int cProgramData::AddType( const char *raw, int tokenID )
{
	stType newType;
	newType.sNameOrig.SetStr( raw );
	newType.sName.SetStr( raw );
	newType.sName.Lower();
	newType.iTokenID = tokenID;
	return m_pTypes.Add( &newType, false );
}

int cProgramData::GetFunctionLineNum( const char *name )
{
	stFunction newFunc;
	newFunc.sName.SetStr( name );
	int index = m_pFunctions.Find( &newFunc );
	if ( index < 0 ) return -1;

	int tokenID = m_pFunctions.m_pData[ index ]->iTokenID;
	return GetToken( tokenID )->iLine;
}

int cProgramData::FindFunction( const char *name )
{
	stFunction newFunc;
	newFunc.sName.SetStr( name );
	return m_pFunctions.Find( &newFunc );
}

int cProgramData::GetTypeLineNum( const char *name )
{
	stType newType;
	newType.sName.SetStr( name );
	int index = m_pTypes.Find( &newType );
	if ( index < 0 ) return -1;

	int tokenID = m_pTypes.m_pData[ index ]->iTokenID;
	return GetToken( tokenID )->iLine;
}

int cProgramData::FindType( const char *name )
{
	stType newType;
	newType.sName.SetStr( name );
	return m_pTypes.Find( &newType );
}

int cProgramData::AddConstant( const char *raw, int tokenID )
{
	stConstant newConstant;
	newConstant.sNameOrig.SetStr( raw );
	newConstant.sName.SetStr( raw );
	newConstant.sName.Lower();
	newConstant.tokenID = tokenID;
	return m_pConstants.Add( &newConstant, false );
}

int cProgramData::FindConstant( const char *name )
{
	stConstant newConstant;
	newConstant.sName.SetStr( name );
	return m_pConstants.Find( &newConstant );
}

int cProgramData::AddVariable( stVariable &var )
{
	switch( var.iType )
	{
		case AGK_VARIABLE_INTEGER:	var.iVarID = m_iCountInts++; break;
		case AGK_VARIABLE_FLOAT:	var.iVarID = m_iCountFloats++; break;
		case AGK_VARIABLE_STRING:	var.iVarID = m_iCountStrings++; break;
		case AGK_VARIABLE_TYPE:		var.iVarID = m_iCountTypes++; break;
		case AGK_VARIABLE_ARRAY_INT:	
		case AGK_VARIABLE_ARRAY_FLOAT:	
		case AGK_VARIABLE_ARRAY_STRING:	
		case AGK_VARIABLE_ARRAY_TYPE:	var.iVarID = m_iCountArrays++; break;
		default: Error( "Unrecognised variable type", 0,0 );
	}

	int result = m_pVariables.Add( &var, false );
	if ( result < 0 )
	{
		// the variable already existed so didn't increase the variable count
		switch( var.iType )
		{
			case AGK_VARIABLE_INTEGER:	m_iCountInts--; break;
			case AGK_VARIABLE_FLOAT:	m_iCountFloats--; break;
			case AGK_VARIABLE_STRING:	m_iCountStrings--; break;
			case AGK_VARIABLE_TYPE:		m_iCountTypes--; break;
			case AGK_VARIABLE_ARRAY_INT:	
			case AGK_VARIABLE_ARRAY_FLOAT:	
			case AGK_VARIABLE_ARRAY_STRING:	
			case AGK_VARIABLE_ARRAY_TYPE:	m_iCountArrays--; break;
			default: Error( "Unrecognised variable type", 0,0 );
		}
	}

	return result;
}

int cProgramData::FindVariable( const char *name )
{
	stVariable newVar;
	newVar.sName.SetStr( name );
	int index = m_pVariables.Find( &newVar );
	return index;
}

int cProgramData::AddInstruction( AGKInstruction instruction, int lineNum, int includeFile, int extra1, int extra2 )
{
	if ( m_iSkipInstructions ) return -1;

	stInstruction newInstruction;
	newInstruction.iInstruction = instruction;
	newInstruction.iLineNum = lineNum;
	newInstruction.iIncludeFile = includeFile;
	newInstruction.i = extra1;
	newInstruction.i2 = extra2;
	return m_pInstructions.Add( &newInstruction );
}

void cProgramData::EditInstruction( int instructionID, AGKInstruction instruction, int extra1, int extra2 )
{	
	if ( m_iSkipInstructions ) return;
	if ( instructionID < 0 || instructionID >= m_pInstructions.m_iCount ) return;

	if ( instruction != AGKI_UNKNOWN ) m_pInstructions.m_pData[ instructionID ]->iInstruction = instruction;
	m_pInstructions.m_pData[ instructionID ]->i = extra1;
	m_pInstructions.m_pData[ instructionID ]->i2 = extra2;
}

int cProgramData::AddString( const char *value )
{
	stString findString;
	findString.sName.SetStr( value );
	int index = m_pStringTable.Find( &findString );
	if ( index >= 0 ) return m_pStringTable.m_pData[ index ]->iID;

	findString.iID = m_pStringTable.m_iCount;
	index = m_pStringTable.Add( &findString, false );
	return findString.iID;
}

int cProgramData::AddPlugin( const char* name, const char* asname, int tokenID, uString *error )
{
	uString sName( name );
	sName.Strip( "\\/" );

	uString sRefNameOrig = asname;

	uString sRefName( asname );
	sRefName.Strip( "\\/" );
	sRefName.Lower();

	int found = 0;
	for ( int i = 0; i < m_pPlugins.m_iCount; i++ )
	{
		if ( strcmp( m_pPlugins.m_pData[ i ]->sName.GetStr(), sName ) == 0 )
		{
			found = 1;
		}
	}

	if ( !found )
	{
		// load plugin file
		uString pluginPath;
#if defined(AGK_STATIC_LIB_IDE)
		pluginPath = "/";
#elif defined(AGK_STATIC_LIB)
		pluginPath = "../";
#else
		pluginPath = "../../";
#endif

#ifdef IDE_MAC
        // might not be possible with Mac sandboxing, perhaps ask user for a path to plugins instead
		pluginPath.Append( "../../../" );
#endif
		pluginPath.Append( "Plugins/" );
		pluginPath.Append( sName );
		pluginPath.AppendUTF8( "/Commands.txt" );

		cFile pluginFile;
		if ( !pluginFile.OpenToRead( pluginPath ) )
		{
			if ( error ) error->Format( "Failed to load plugin command list from %s for plugin %s", pluginPath.GetStr(), name );
			return 0;
		}

		stPlugin newPlugin;
		newPlugin.sName.SetStr( sName );
		newPlugin.iTokenID = tokenID;

		// count commands
		uString sLine;
		int commandCount = 0;
		while ( pluginFile.ReadLineFast( sLine ) )
		{
			if ( sLine.CharAt( 0 ) == '#' ) continue;

			int tokenCount = sLine.CountTokens2( ',' );
			if ( tokenCount < 8 )
			{
				if ( error ) error->Format( "Failed to load plugin %s, command list %s does not have enough tokens per line", name, pluginPath.GetStr() );
				return 0;
			}

			commandCount++;
		}

		newPlugin.m_iNumCommands = commandCount;
		newPlugin.m_pCommands = new stPluginCommand[ commandCount ];

		pluginFile.Rewind();

		commandCount = 0;
		while ( pluginFile.ReadLineFast( sLine ) )
		{
			if ( sLine.CharAt( 0 ) == '#' ) continue;
			sLine.Trim( "\n\r " );

			int tokenCount = sLine.CountTokens2( ',' );
			if ( tokenCount < 8 )
			{
				if ( error ) error->Format( "Failed to load plugin %s, command list %s does not have enough tokens per line", name, pluginPath.GetStr() );
				return 0;
			}

			newPlugin.m_pCommands[ commandCount ].iFlags = 0;

			// read command name
			sLine.GetToken2( ',', 1, newPlugin.m_pCommands[ commandCount ].sTier1NameOrig );
			newPlugin.m_pCommands[ commandCount ].sTier1Name = newPlugin.m_pCommands[ commandCount ].sTier1NameOrig;
			newPlugin.m_pCommands[ commandCount ].sTier1Name.Lower();

			// read return type
			uString temp;
			sLine.GetToken2( ',', 2, temp );
			if ( strcmp( temp, "0" ) == 0 ) newPlugin.m_pCommands[ commandCount ].iReturnType = 0;
			else if ( strcmp( temp, "I" ) == 0 ) newPlugin.m_pCommands[ commandCount ].iReturnType = AGK_VARIABLE_INTEGER;
			else if ( strcmp( temp, "F" ) == 0 ) newPlugin.m_pCommands[ commandCount ].iReturnType = AGK_VARIABLE_FLOAT;
			else if ( strcmp( temp, "S" ) == 0 ) newPlugin.m_pCommands[ commandCount ].iReturnType = AGK_VARIABLE_STRING;
			else
			{
				if ( error ) error->Format( "Plugin command %s has an invalid return type %s", newPlugin.m_pCommands[ commandCount ].sTier1NameOrig.GetStr(), temp.GetStr() );
				return 0;
			}

			// read parameter types
			sLine.GetToken2( ',', 3, temp );
			newPlugin.m_pCommands[ commandCount ].iNumParams = temp.GetNumChars();
			if ( newPlugin.m_pCommands[ commandCount ].iNumParams == 0 )
			{
				if ( error ) error->Format( "Plugin command %s has an invalid parameter type, use 0 if there are no parameters", newPlugin.m_pCommands[ commandCount ].sTier1NameOrig.GetStr() );
				return 0;
			}

			if ( temp.CharAt( 0 ) == '0' )
			{
				newPlugin.m_pCommands[ commandCount ].iNumParams = 0;
			}

			if ( newPlugin.m_pCommands[ commandCount ].iNumParams > 32 )
			{
				if ( error ) error->Format( "Plugin command %s has too many parameters, max is 32", newPlugin.m_pCommands[ commandCount ].sTier1NameOrig.GetStr() );
				return 0;
			}

			for ( int i = 0; i < newPlugin.m_pCommands[ commandCount ].iNumParams; i++ )
			{
				int param = temp.CharAt( i );
				if ( param == '0' ) newPlugin.m_pCommands[ commandCount ].iParamsTypes[ i ] = 0;
				else if ( param == 'I' ) newPlugin.m_pCommands[ commandCount ].iParamsTypes[ i ] = AGK_VARIABLE_INTEGER;
				else if ( param == 'F' ) newPlugin.m_pCommands[ commandCount ].iParamsTypes[ i ] = AGK_VARIABLE_FLOAT;
				else if ( param == 'S' ) newPlugin.m_pCommands[ commandCount ].iParamsTypes[ i ] = AGK_VARIABLE_STRING;
				else
				{
					if ( error ) error->Format( "Plugin command %s has an invalid parameter type %c", newPlugin.m_pCommands[ commandCount ].sTier1NameOrig.GetStr(), param );
					return 0;
				}
			}

			sLine.GetToken2( ',', 4, newPlugin.m_pCommands[ commandCount ].sWindowsName );
			sLine.GetToken2( ',', 5, newPlugin.m_pCommands[ commandCount ].sLinuxName );
			sLine.GetToken2( ',', 6, newPlugin.m_pCommands[ commandCount ].sMacName );
			sLine.GetToken2( ',', 7, newPlugin.m_pCommands[ commandCount ].sAndroidName );
			sLine.GetToken2( ',', 8, newPlugin.m_pCommands[ commandCount ].siOSName );
			if ( tokenCount > 8 ) sLine.GetToken2( ',', 9, newPlugin.m_pCommands[ commandCount ].sWindows64Name );

			commandCount++;
		}

		pluginFile.Close();

		qsort( newPlugin.m_pCommands, newPlugin.m_iNumCommands, sizeof(stPluginCommand), ComparePluginCommand );

		m_pPlugins.Add( &newPlugin );
	}

	found = 0;
	for ( int i = 0; i < m_pPluginRefs.m_iCount; i++ )
	{
		if ( strcmp( m_pPluginRefs.m_pData[ i ]->sName.GetStr(), sName ) == 0
		  && strcmp( m_pPluginRefs.m_pData[ i ]->sRefName.GetStr(), sRefName ) == 0)
		{
			found = 1;
		}
	}

	if ( !found )
	{
		stPluginRef newPluginRef;
		newPluginRef.sName.SetStr( sName );
		newPluginRef.sRefName.SetStr( sRefName );
		newPluginRef.sRefNameOrig.SetStr( sRefNameOrig );
		int index = m_pPluginRefs.Add( &newPluginRef, false );
		if ( index < 0 )
		{
			if ( error ) error->Format( "Plugin reference %s is already being used by another plugin", sRefNameOrig.GetStr() );
			return 0;
		}
	}

	return 1;
}

int cProgramData::FindPlugin( const char *name )
{
	stPluginRef findRef;
	findRef.sRefName.SetStr( name );
	int index =  m_pPluginRefs.Find( &findRef );
	if ( index < 0 ) return index;

	stPlugin findPlugin;
	findPlugin.sName.SetStr( m_pPluginRefs.m_pData[ index ]->sName );
	return m_pPlugins.Find( &findPlugin );
}

int cProgramData::FindPluginCommandName( int pluginIndex, const char *name )
{
	int numCommands = m_pPlugins.m_pData[ pluginIndex ]->m_iNumCommands;
	stPluginCommand *pCommands = m_pPlugins.m_pData[ pluginIndex ]->m_pCommands;

	int high = numCommands-1;
	int low = 0;
	
	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = pCommands[ mid ].sTier1Name.CompareTo( name );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else return mid;
	}

	return -1;
}

int cProgramData::FindPluginCommand( int pluginIndex, stPluginCommand *pCommand )
{
	int numCommands = m_pPlugins.m_pData[ pluginIndex ]->m_iNumCommands;
	stPluginCommand *pCommands = m_pPlugins.m_pData[ pluginIndex ]->m_pCommands;

	int high = numCommands-1;
	int low = 0;
	
	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = ComparePluginCommand( pCommands + mid, pCommand );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else return mid;
	}

	return -1;
}

int cProgramData::FindPluginCommandCompatible( int pluginIndex, stPluginCommand *pCommand )
{
	int numCommands = m_pPlugins.m_pData[ pluginIndex ]->m_iNumCommands;
	stPluginCommand *pCommands = m_pPlugins.m_pData[ pluginIndex ]->m_pCommands;
	
	int high = numCommands-1;
	int low = 0;
	
	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = ComparePluginCommandCompatible( pCommands + mid, pCommand );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else return mid;
	}

	return -1;
}

int cProgramData::AddModule( const char *name, const char *asname, uString *error )
{
	return 0;
}

int cProgramData::FindModule( const char *name )
{
	stModuleRef findRef;
	findRef.sRefName.SetStr( name );
	int index =  m_pModuleRefs.Find( &findRef );
	if ( index < 0 ) return index;

	stModule findModule;
	findModule.sName.SetStr( m_pModuleRefs.m_pData[ index ]->sName );
	return m_pModules.Find( &findModule );
}

int cProgramData::GetInstructionPosition()
{
	return m_pInstructions.m_iCount-1;
}

int AGK_Compiler::CompareFunction( const void *a, const void *b )
{
	stFunction* f1 = *((stFunction**)a);
	stFunction* f2 = *((stFunction**)b);

	return strcmp( f1->sName.GetStr(), f2->sName.GetStr() );
}

int AGK_Compiler::CompareLabel( const void *a, const void *b )
{
	stLabel* l1 = *((stLabel**)a);
	stLabel* l2 = *((stLabel**)b);

	return strcmp( l1->sName.GetStr(), l2->sName.GetStr() );
}

int AGK_Compiler::CompareVariable( const void *a, const void *b )
{
	stVariable* v1 = *((stVariable**)a);
	stVariable* v2 = *((stVariable**)b);

	return strcmp( v1->sName.GetStr(), v2->sName.GetStr() );
}

int AGK_Compiler::CompareVariableType( const void *a, const void *b )
{
	stVariable* v1 = *((stVariable**)a);
	stVariable* v2 = *((stVariable**)b);

	int type1 = 0;
	switch( v1->iType )
	{
		case AGK_VARIABLE_ARRAY_INT: type1 = -4; break;
		case AGK_VARIABLE_ARRAY_FLOAT: type1 = -4; break;
		case AGK_VARIABLE_ARRAY_STRING: type1 = -4; break;
		case AGK_VARIABLE_ARRAY_TYPE: type1 = -4; break;
		case AGK_VARIABLE_TYPE: type1 = -3; break;
		case AGK_VARIABLE_STRING: type1 = -2; break;
		case AGK_VARIABLE_FLOAT: type1 = -1; break;
	}

	int type2 = 0;
	switch( v2->iType )
	{
		case AGK_VARIABLE_ARRAY_INT: type2 = -4; break;
		case AGK_VARIABLE_ARRAY_FLOAT: type2 = -4; break;
		case AGK_VARIABLE_ARRAY_STRING: type2 = -4; break;
		case AGK_VARIABLE_ARRAY_TYPE: type2 = -4; break;
		case AGK_VARIABLE_TYPE: type2 = -3; break;
		case AGK_VARIABLE_STRING: type2 = -2; break;
		case AGK_VARIABLE_FLOAT: type2 = -1; break;
	}

	if ( type1 < type2 ) return -1;
	else if ( type1 > type2 ) return 1;
	else return 0;
}

int AGK_Compiler::CompareVariableVarID( const void *a, const void *b )
{
	stVariable* v1 = *((stVariable**)a);
	stVariable* v2 = *((stVariable**)b);

	if ( v1->iVarID < v2->iVarID ) return -1;
	else if ( v1->iVarID > v2->iVarID ) return 1;
	else return 0;
}

int AGK_Compiler::CompareType( const void *a, const void *b )
{
	stType* t1 = *((stType**)a);
	stType* t2 = *((stType**)b);

	return strcmp( t1->sName.GetStr(), t2->sName.GetStr() );
}

int AGK_Compiler::CompareConstant( const void *a, const void *b )
{
	stConstant* c1 = *((stConstant**)a);
	stConstant* c2 = *((stConstant**)b);

	return strcmp( c1->sName.GetStr(), c2->sName.GetStr() );
}

int AGK_Compiler::CompareCommand( const void *a, const void *b )
{
	const stCommand* c1 = (const stCommand*) a;
	const stCommand* c2 = (const stCommand*) b;

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

int AGK_Compiler::CompareCommandCompatible( const void *a, const void *b )
{
	const stCommand* c1 = (const stCommand*) a;
	const stCommand* c2 = (const stCommand*) b;

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
				int param1 = c1->iParamsTypes[ i ];
				int param2 = c2->iParamsTypes[ i ];
				if ( param1 == AGK_VARIABLE_FLOAT ) param1 = AGK_VARIABLE_INTEGER;
				if ( param2 == AGK_VARIABLE_FLOAT ) param2 = AGK_VARIABLE_INTEGER;

				if ( param1 < param2 ) return -1;
				else if ( param1 > param2 ) return 1;
			}

			return 0;
		}
	}
}

int AGK_Compiler::CompareString( const void *a, const void *b )
{
	stString* s1 = *((stString**)a);
	stString* s2 = *((stString**)b);

	return s1->sName.CompareTo( s2->sName.GetStr() );
}

int AGK_Compiler::CompareStringID( const void *a, const void *b )
{
	stString* s1 = *((stString**)a);
	stString* s2 = *((stString**)b);

	if ( s1->iID < s2->iID ) return -1;
	else if ( s1->iID > s2->iID ) return 1;
	else return 0;
}

// AGK Plugins

int AGK_Compiler::ComparePluginRef( const void *a, const void *b )
{
	stPluginRef* p1 = *((stPluginRef**)a);
	stPluginRef* p2 = *((stPluginRef**)b);

	return strcmp( p1->sRefName.GetStr(), p2->sRefName.GetStr() );
}

int AGK_Compiler::ComparePlugin( const void *a, const void *b )
{
	stPlugin* p1 = *((stPlugin**)a);
	stPlugin* p2 = *((stPlugin**)b);

	return strcmp( p1->sName.GetStr(), p2->sName.GetStr() );
}

int AGK_Compiler::ComparePluginCommand( const void *a, const void *b )
{
	stPluginCommand* c1 = (stPluginCommand*) a;
	stPluginCommand* c2 = (stPluginCommand*) b;

	int result = c1->sTier1Name.CompareTo( c2->sTier1Name );
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

int AGK_Compiler::ComparePluginCommandCompatible( const void *a, const void *b )
{
	stPluginCommand* c1 = (stPluginCommand*) a;
	stPluginCommand* c2 = (stPluginCommand*) b;

	int result = c1->sTier1Name.CompareTo( c2->sTier1Name );
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
				int param1 = c1->iParamsTypes[ i ];
				int param2 = c2->iParamsTypes[ i ];
				if ( param1 == AGK_VARIABLE_FLOAT ) param1 = AGK_VARIABLE_INTEGER;
				if ( param2 == AGK_VARIABLE_FLOAT ) param2 = AGK_VARIABLE_INTEGER;

				if ( param1 < param2 ) return -1;
				else if ( param1 > param2 ) return 1;
			}

			return 0;
		}
	}
}

// AGK Modules

int AGK_Compiler::CompareModuleRef( const void *a, const void *b )
{
	stModuleRef* m1 = *((stModuleRef**)a);
	stModuleRef* m2 = *((stModuleRef**)b);

	return strcmp( m1->sRefName.GetStr(), m2->sRefName.GetStr() );
}

int AGK_Compiler::CompareModule( const void *a, const void *b )
{
	stModule* m1 = *((stModule**)a);
	stModule* m2 = *((stModule**)b);

	return strcmp( m1->sName.GetStr(), m2->sName.GetStr() );
}

int AGK_Compiler::CompareModuleFunction( const void *a, const void *b )
{
	stModuleFunction* f1 = *((stModuleFunction**)a);
	stModuleFunction* f2 = *((stModuleFunction**)b);

	return strcmp( f1->sName.GetStr(), f2->sName.GetStr() );
}

bool cProgramData::LoadCommandsArrayGlobal( uString &error )
{
	m_iNumCommands = g_iAGKNumCommands;
	if ( m_iNumCommands <= 0 ) return true;

	if ( m_pCommands ) delete [] m_pCommands;
	m_pCommands = new stCommand[ m_iNumCommands ];

	uString temp;

	for( int i = 0; i < m_iNumCommands; i++ )
	{
		m_pCommands[i].instructionID = (AGKInstruction) ((i+1) + AGKI_COMMAND_0);
		strcpy( m_pCommands[i].szCommandName, g_pAGKCommands[i].szCommandName );
		temp.SetStr( m_pCommands[i].szCommandName );
		temp.Lower();
		strcpy( m_pCommands[i].szCommandName, temp.GetStr() );
		m_pCommands[i].iReturnType = g_pAGKCommands[i].iReturnType;
		m_pCommands[i].iNumParams = g_pAGKCommands[i].iNumParams;
		if ( m_pCommands[i].iNumParams > 32 )
		{
			error.SetStr( "Failed to load commands array, too many parameters" );
			return false;
		}

		for ( int p = 0; p < m_pCommands[i].iNumParams; p++ )
		{
			m_pCommands[i].iParamsTypes[p] = g_pAGKCommands[i].iParamsTypes[p];
		}
	}

	qsort( m_pCommands, m_iNumCommands, sizeof(stCommand), CompareCommand );

	return true;
}

int cProgramData::FindCommandName( const char *name )
{
	int high = m_iNumCommands-1;
	int low = 0;
	
	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = strcmp( m_pCommands[ mid ].szCommandName, name );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else return mid;
	}

	return -1;
}

int cProgramData::FindCommand( stCommand *pCommand )
{
	int high = m_iNumCommands-1;
	int low = 0;
	
	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = CompareCommand( m_pCommands + mid, pCommand );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else return mid;
	}

	return -1;
}

int cProgramData::FindCommandCompatible( stCommand *pCommand )
{
	int high = m_iNumCommands-1;
	int low = 0;
	
	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = CompareCommandCompatible( m_pCommands + mid, pCommand );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else return mid;
	}

	return -1;
}

int cProgramData::FinalCheck()
{
	int has_error = 0;

	// check all global and local variables were assigned a value before being used
	for ( int i = 0; i < m_pVariables.m_iCount; i++ )
	{
		if ( m_pVariables.m_pData[ i ]->iInitialised == 0 )
		{
			int tokenID = m_pVariables.m_pData[ i ]->iTokenID;
			uString error;
			error.Format( "Variable \"%s\" is used without being defined or initialised", m_pVariables.m_pData[ i ]->sNameOrig.GetStr() );
			TokenError( error, tokenID );
			//return 0;
			has_error = 1;
		}
	}

	// check all function local variables were assigned a value before being used
	for ( int i = 0; i < m_pFunctions.m_iCount; i++ )
	{
		stFunction *pFunction = m_pFunctions.m_pData[ i ];
		for ( int v = 0; v < pFunction->pFuncVars.m_iCount; v++ )
		{
			if ( pFunction->pFuncVars.m_pData[ v ]->iInitialised == 0 )
			{
				int tokenID = pFunction->pFuncVars.m_pData[ v ]->iTokenID;
				uString error;
				error.Format( "Variable \"%s\" is used without being defined or initialised", pFunction->pFuncVars.m_pData[ v ]->sNameOrig.GetStr() );
				TokenError( error, tokenID );
				//return 0;
				has_error = 1;
			}
		}
	}

	// replace function calls with actual pointers
	for ( int i = 0; i < m_pInstructions.m_iCount; i++ )
	{
		if ( m_pInstructions.m_pData[ i ]->iInstruction == AGKI_FUNCTION_CALL )
		{
			int index = m_pInstructions.m_pData[ i ]->i;
			m_pInstructions.m_pData[ i ]->i = m_pFunctions.m_pData[ index ]->iInstructionPtr;
		}
	}

	if ( has_error ) return 0;
	else return 1;
}

int cProgramData::BuildBytecodeFile( const char *filename, int debugMode )
{
	cFile oBytecode;
	if ( !oBytecode.OpenToWrite( filename ) )
	{
		Error("Failed to open bytecode file for writing",0,0);
		return 0;
	}

	// write bytecode version
	oBytecode.WriteInteger( AGK_BYTECODE_VERSION );
	oBytecode.WriteInteger( debugMode );

	// header
	int company_length = m_sCompanyName.GetLength();
	oBytecode.WriteInteger( 4 + company_length + 1 + 4 ); // header size = string length + string data + renderer mode + flags
	oBytecode.WriteInteger( company_length ); // string length
	if ( company_length > 0 ) oBytecode.WriteData( m_sCompanyName.GetStr(), company_length );
	oBytecode.WriteByte( (unsigned char) m_iRendererMode );
	oBytecode.WriteInteger( m_iFlags );

	// write plugin commands
	oBytecode.WriteInteger( m_pPlugins.m_iCount );
	for ( int i = 0; i < m_pPlugins.m_iCount; i++ )
	{
		int length = m_pPlugins.m_pData[ i ]->sName.GetLength();
		oBytecode.WriteInteger( length );
		if ( length > 0 ) oBytecode.WriteData( m_pPlugins.m_pData[ i ]->sName.GetStr(), length );

		oBytecode.WriteInteger( m_pPlugins.m_pData[ i ]->m_iNumCommands );
		for ( int j = 0; j < m_pPlugins.m_pData[ i ]->m_iNumCommands; j++ )
		{
			stPluginCommand *pCommand = m_pPlugins.m_pData[ i ]->m_pCommands + j;
			if ( (pCommand->iFlags & AGK_PLUGIN_COMMAND_USED) == 0 )
			{
				oBytecode.WriteByte( 0 );
				continue;
			}
			oBytecode.WriteByte( 1 ); // used
			oBytecode.WriteByte( pCommand->iReturnType );
			oBytecode.WriteByte( pCommand->iNumParams );
			for( int k = 0; k < pCommand->iNumParams; k++ )
			{
				oBytecode.WriteByte( pCommand->iParamsTypes[k] );
			}
			
			length = pCommand->sWindowsName.GetLength();
			oBytecode.WriteInteger( length );
			if ( length > 0 ) oBytecode.WriteData( pCommand->sWindowsName.GetStr(), length );
			length = pCommand->sLinuxName.GetLength();
			oBytecode.WriteInteger( length );
			if ( length > 0 ) oBytecode.WriteData( pCommand->sLinuxName.GetStr(), length );
			length = pCommand->sMacName.GetLength();
			oBytecode.WriteInteger( length );
			if ( length > 0 ) oBytecode.WriteData( pCommand->sMacName.GetStr(), length );
			length = pCommand->sAndroidName.GetLength();
			oBytecode.WriteInteger( length );
			if ( length > 0 ) oBytecode.WriteData( pCommand->sAndroidName.GetStr(), length );
			length = pCommand->siOSName.GetLength();
			oBytecode.WriteInteger( length );
			if ( length > 0 ) oBytecode.WriteData( pCommand->siOSName.GetStr(), length );
			length = pCommand->sWindows64Name.GetLength();
			oBytecode.WriteInteger( length );
			if ( length > 0 ) oBytecode.WriteData( pCommand->sWindows64Name.GetStr(), length );
		}
	}

	// write out include file names
	oBytecode.WriteInteger( m_pIncludeFiles.m_iCount );
	for ( int i = 0; i < m_pIncludeFiles.m_iCount; i++ )
	{
		int length = m_pIncludeFiles.m_pData[i]->sName.GetLength();
		oBytecode.WriteInteger( length );
		oBytecode.WriteData( m_pIncludeFiles.m_pData[i]->sName.GetStr(), length );
	}

	// write out string table
	m_pStringTable.SetSorted( true, CompareStringID );
	int checksum = 0;
	oBytecode.WriteInteger( m_pStringTable.m_iCount );
	for ( int i = 0; i < m_pStringTable.m_iCount; i++ )
	{
		int length = m_pStringTable.m_pData[ i ]->sName.GetLength();
		oBytecode.WriteInteger( length );
		oBytecode.WriteData( m_pStringTable.m_pData[ i ]->sName.GetStr(), length );

		int flip = -1;
		for ( int j = 0; j < length; j++ )
		{
			checksum += m_pStringTable.m_pData[ i ]->sName.ByteAt(j)*j*flip;
			flip = 0-flip;
		}
	}

	// write string table check sum
	oBytecode.WriteInteger( checksum );

	// write type structures
	oBytecode.WriteInteger( m_pTypes.m_iCount );
	for ( int i = 0; i < m_pTypes.m_iCount; i++ )
	{
		// sort variables by VarID
		m_pTypes.m_pData[ i ]->m_pSubVars.SetSorted( true, CompareVariableVarID );
		int firstIndex = -1;
		
		// write variable count for this type
		oBytecode.WriteInteger( m_pTypes.m_pData[ i ]->m_pSubVars.m_iCount );
		for ( int v = 0; v < m_pTypes.m_pData[ i ]->m_pSubVars.m_iCount; v++ )
		{
			// write each variable data
			oBytecode.WriteInteger( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->iType );
			oBytecode.WriteInteger( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->iTypeIndex );
			oBytecode.WriteInteger( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->iTypeOrder );
			oBytecode.WriteInteger( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->iNumDimensions );
			if ( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->iNumDimensions > 0 )
			{
				// write array dimensions
				for ( int d = 0; d < m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->iNumDimensions; d++ )
				{
					if ( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->pDefaultLengths ) oBytecode.WriteInteger( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->pDefaultLengths[ d ] );
					else oBytecode.WriteInteger( -1 );
				}
			}

			// write variable name
			//if ( debugMode > 0 ) // type variable names must now be written in all cases due to JSON load/save support
			{
				uString encoded( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->sName );
				encoded.XOR( 0xFF ); // this will be obvious to undo as the interpreter is open source, but no harm in trying to protect peoples code
				int length = encoded.GetLength();
				oBytecode.WriteInteger( length );
				if ( length > 0 )
				{
					if ( length >= 1024 )
					{
						Error( "Error writing bytecode, type field name is too long, max 1024 characters",0,0 );
						return 0;
					}
					oBytecode.WriteData( encoded.GetStr(), length );
				}
			}

			if ( m_pTypes.m_pData[ i ]->m_pSubVars.m_pData[ v ]->sName.CompareTo( m_pTypes.m_pData[ i ]->sFirstVar ) == 0 )
			{
				firstIndex = v;
			}
		}

		if ( firstIndex < 0 )
		{
			Error( "Error writing bytecode, could not find the first declared variable for type",0,0 );
			return 0;
		}

		// write first variable index, variables have been shuffled for implementation reasons, this was the first var defined by the user
		oBytecode.WriteInteger( firstIndex );
	}

	// write label offsets
	oBytecode.WriteInteger( m_pLabels.m_iCount );
	for ( int i = 0; i < m_pLabels.m_iCount; i++ )
	{
		oBytecode.WriteInteger( m_pLabels.m_pData[ i ]->iInstructionPtr );
	}

	// sort variables by ID
	m_pVariables.SetSorted( true, CompareVariableVarID );

	// integer variables
	oBytecode.WriteInteger( m_iCountInts );
	int count = 0;
	for ( int i = 0; i < m_pVariables.m_iCount; i++ )
	{
		if ( m_pVariables.m_pData[ i ]->iType == AGK_VARIABLE_INTEGER )
		{
			if ( m_pVariables.m_pData[ i ]->iVarID != count )
			{
				Error( "Error writing bytecode, integer variable index does not match previously assigned value",0,0 );
				return 0;
			}
			count++;

			// write default value if present
			if ( m_pVariables.m_pData[ i ]->pDefaultI ) oBytecode.WriteInteger( m_pVariables.m_pData[ i ]->pDefaultI[ 0 ] );
			else oBytecode.WriteInteger( 0 );

			// write variable name
			if ( debugMode > 0 )
			{
				uString encoded( m_pVariables.m_pData[ i ]->sName );
				encoded.XOR( 0xFF ); // this will be obvious to undo as the interpreter is open source, but no harm in trying to protect peoples code
				int length = encoded.GetLength();
				oBytecode.WriteInteger( length );
				if ( length > 0 )
				{
					if ( length >= 1024 )
					{
						Error( "Error writing bytecode, integer variable name is too long, max 1024 characters",0,0 );
						return 0;
					}
					oBytecode.WriteData( encoded.GetStr(), length );
				}
			}
		}
	}

	if ( count != m_iCountInts )
	{
		Error( "Error writing bytecode, number of integer variables does not match previous number",0,0 );
		return 0;
	}

	// float variables
	oBytecode.WriteInteger( m_iCountFloats );
	count = 0;
	for ( int i = 0; i < m_pVariables.m_iCount; i++ )
	{
		if ( m_pVariables.m_pData[ i ]->iType == AGK_VARIABLE_FLOAT )
		{
			if ( m_pVariables.m_pData[ i ]->iVarID != count )
			{
				Error( "Error writing bytecode, float variable index does not match previously assigned value",0,0 );
				return 0;
			}
			count++;

			// write default value if present
			if ( m_pVariables.m_pData[ i ]->pDefaultF ) oBytecode.WriteFloat( m_pVariables.m_pData[ i ]->pDefaultF[ 0 ] );
			else oBytecode.WriteFloat( 0 );

			// write variable name
			if ( debugMode > 0 )
			{
				uString encoded( m_pVariables.m_pData[ i ]->sName );
				encoded.XOR( 0xFF ); // this will be obvious to undo as the interpreter is open source, but no harm in trying to protect peoples code
				int length = encoded.GetLength();
				oBytecode.WriteInteger( length );
				if ( length > 0 )
				{
					if ( length >= 1024 )
					{
						Error( "Error writing bytecode, float variable name is too long, max 1024 characters",0,0 );
						return 0;
					}
					oBytecode.WriteData( encoded.GetStr(), length );
				}
			}
		}
	}

	if ( count != m_iCountFloats )
	{
		Error( "Error writing bytecode, number of integer variables does not match previous number",0,0 );
		return 0;
	}

	// string variables
	oBytecode.WriteInteger( m_iCountStrings );
	count = 0;
	for ( int i = 0; i < m_pVariables.m_iCount; i++ )
	{
		if ( m_pVariables.m_pData[ i ]->iType == AGK_VARIABLE_STRING )
		{
			if ( m_pVariables.m_pData[ i ]->iVarID != count )
			{
				Error( "Error writing bytecode, string variable index does not match previously assigned value",0,0 );
				return 0;
			}
			count++;

			// write default value if present, index into string table
			if ( m_pVariables.m_pData[ i ]->pDefaultI ) oBytecode.WriteInteger( m_pVariables.m_pData[ i ]->pDefaultI[ 0 ] );
			else oBytecode.WriteInteger( -1 );

			// write variable name
			if ( debugMode > 0 )
			{
				uString encoded( m_pVariables.m_pData[ i ]->sName );
				encoded.XOR( 0xFF ); // this will be obvious to undo as the interpreter is open source, but no harm in trying to protect peoples code
				int length = encoded.GetLength();
				oBytecode.WriteInteger( length );
				if ( length > 0 )
				{
					if ( length >= 1024 )
					{
						Error( "Error writing bytecode, string variable name is too long, max 1024 characters",0,0 );
						return 0;
					}
					oBytecode.WriteData( encoded.GetStr(), length );
				}
			}
		}
	}

	if ( count != m_iCountStrings )
	{
		Error( "Error writing bytecode, number of string variables does not match previous number",0,0 );
		return 0;
	}

	// type variables
	oBytecode.WriteInteger( m_iCountTypes );
	count = 0;
	for ( int i = 0; i < m_pVariables.m_iCount; i++ )
	{
		if ( m_pVariables.m_pData[ i ]->iType == AGK_VARIABLE_TYPE )
		{
			if ( m_pVariables.m_pData[ i ]->iVarID != count )
			{
				Error( "Error writing bytecode, type variable index does not match previously assigned value",0,0 );
				return 0;
			}
			count++;

			// write type index
			if ( m_pVariables.m_pData[ i ]->iTypeIndex < 0 )
			{
				Error( "Error writing bytecode, type variable does not have a valid type index",0,0 );
				return 0;
			}

			oBytecode.WriteInteger( m_pVariables.m_pData[ i ]->iTypeIndex );

			// write variable name
			if ( debugMode > 0 )
			{
				uString encoded( m_pVariables.m_pData[ i ]->sName );
				encoded.XOR( 0xFF ); // this will be obvious to undo as the interpreter is open source, but no harm in trying to protect peoples code
				int length = encoded.GetLength();
				oBytecode.WriteInteger( length );
				if ( length > 0 )
				{
					if ( length >= 1024 )
					{
						Error( "Error writing bytecode, type variable name is too long, max 1024 characters",0,0 );
						return 0;
					}
					oBytecode.WriteData( encoded.GetStr(), length );
				}
			}
		}
	}

	if ( count != m_iCountTypes )
	{
		Error( "Error writing bytecode, number of type variables does not match previous number",0,0 );
		return 0;
	}

	// array variables
	oBytecode.WriteInteger( m_iCountArrays );
	count = 0;
	for ( int i = 0; i < m_pVariables.m_iCount; i++ )
	{
		if ( m_pVariables.m_pData[ i ]->iNumDimensions > 0 )
		{
			if ( m_pVariables.m_pData[ i ]->iVarID != count )
			{
				Error( "Error writing bytecode, array variable index does not match previously assigned value",0,0 );
				return 0;
			}
			count++;

			// write array type
			oBytecode.WriteInteger( m_pVariables.m_pData[ i ]->iType - AGK_VARIABLE_ARRAY_0 );
			oBytecode.WriteInteger( m_pVariables.m_pData[ i ]->iTypeIndex ); // only needed for type arrays

			// write array dimensions
			oBytecode.WriteInteger( m_pVariables.m_pData[ i ]->iNumDimensions );
			if ( m_pVariables.m_pData[ i ]->pDefaultLengths )
			{
				for ( int j = 0; j < m_pVariables.m_pData[ i ]->iNumDimensions; j++ ) oBytecode.WriteInteger( m_pVariables.m_pData[ i ]->pDefaultLengths[ j ] );
			}
			else 
			{
				for ( int j = 0; j < m_pVariables.m_pData[ i ]->iNumDimensions; j++ ) oBytecode.WriteInteger( -1 );
			}

			if ( m_pVariables.m_pData[ i ]->pDefaultI )
			{
				oBytecode.WriteInteger(1);
				for ( int j = 0; j <= m_pVariables.m_pData[ i ]->pDefaultLengths[0]; j++ ) oBytecode.WriteInteger( m_pVariables.m_pData[ i ]->pDefaultI[ j ] );
			}
			else
			{
				oBytecode.WriteInteger(0);
			}

			// write variable name
			if ( debugMode > 0 )
			{
				uString encoded( m_pVariables.m_pData[ i ]->sName );
				encoded.XOR( 0xFF ); // this will be obvious to undo as the interpreter is open source, but no harm in trying to protect peoples code
				int length = encoded.GetLength();
				oBytecode.WriteInteger( length );
				if ( length > 0 )
				{
					if ( length >= 1024 )
					{
						Error( "Error writing bytecode, array variable name is too long, max 1024 characters",0,0 );
						return 0;
					}
					oBytecode.WriteData( encoded.GetStr(), length );
				}
			}
		}
	}

	if ( count != m_iCountArrays )
	{
		Error( "Error writing bytecode, number of array variables does not match previous number",0,0 );
		return 0;
	}

	// write num instructions
	oBytecode.WriteInteger( m_pInstructions.m_iCount );

	// write entry point
	oBytecode.WriteInteger( m_iEntryPoint );

	// write instructions
	for ( int i = 0; i < m_pInstructions.m_iCount; i++ )
	{
		oBytecode.WriteInteger( m_pInstructions.m_pData[ i ]->iInstruction );
		oBytecode.WriteInteger( m_pInstructions.m_pData[ i ]->i );
		oBytecode.WriteInteger( m_pInstructions.m_pData[ i ]->i2 );
		oBytecode.WriteInteger( m_pInstructions.m_pData[ i ]->iLineNum );
		oBytecode.WriteInteger( m_pInstructions.m_pData[ i ]->iIncludeFile );

		if ( m_pInstructions.m_pData[ i ]->iInstruction == AGKI_FUNCTION_CALL )
		{
			int instruction = m_pInstructions.m_pData[ i ]->i;
			if ( instruction == 0 )
			{
				Error( "Error writing bytecode, function pointer should not be 0",0,0 );
				return 0;
			}
		}
	}

	// write function details for debugging
	if ( debugMode > 0 )
	{
		oBytecode.WriteInteger( m_pFunctions.m_iCount );
		for ( int i = 0; i < m_pFunctions.m_iCount; i++ )
		{
			// write function name
			uString encoded( m_pFunctions.m_pData[ i ]->sName );
			encoded.XOR( 0xFF ); // this will be obvious to undo as the interpreter is open source, but no harm in trying to protect peoples code
			int length = encoded.GetLength();
			oBytecode.WriteInteger( length );
			if ( length > 0 )
			{
				if ( length >= 1024 )
				{
					Error( "Error writing bytecode, function name is too long, max 1024 characters",0,0 );
					return 0;
				}
				oBytecode.WriteData( encoded.GetStr(), length );
			}

			// write the instruction index for the first line of this function
			oBytecode.WriteInteger( m_pFunctions.m_pData[ i ]->iInstructionPtr );

			// write variable data
			oBytecode.WriteInteger( m_pFunctions.m_pData[ i ]->pFuncVars.m_iCount );
			for ( int j = 0; j < m_pFunctions.m_pData[ i ]->pFuncVars.m_iCount; j++ )
			{
				// write var name
				encoded.SetStr( m_pFunctions.m_pData[ i ]->pFuncVars.m_pData[ j ]->sName );
				encoded.XOR( 0xFF ); // this will be obvious to undo as the interpreter is open source, but no harm in trying to protect peoples code
				int length = encoded.GetLength();
				oBytecode.WriteInteger( length );
				if ( length > 0 )
				{
					if ( length >= 1024 )
					{
						Error( "Error writing bytecode, function parameter or variable name is too long, max 1024 characters",0,0 );
						return 0;
					}
					oBytecode.WriteData( encoded.GetStr(), length );
				}

				unsigned int type = m_pFunctions.m_pData[ i ]->pFuncVars.m_pData[ j ]->iType;
				type &= 0xff;
				if ( type > AGK_VARIABLE_ARRAY_0 ) type -= AGK_VARIABLE_ARRAY_0;
				type |= ((m_pFunctions.m_pData[ i ]->pFuncVars.m_pData[ j ]->iNumDimensions & 0xff) << 8);
				type |= ((m_pFunctions.m_pData[ i ]->pFuncVars.m_pData[ j ]->iTypeIndex & 0xffff) << 16);
				oBytecode.WriteInteger( type );
				oBytecode.WriteInteger( m_pFunctions.m_pData[ i ]->pFuncVars.m_pData[ j ]->iVarID );
			}
		}
	}

	oBytecode.Close();

	return 1;
}
