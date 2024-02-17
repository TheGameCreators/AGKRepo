// Includes
#include "template.h"
#include "AGKCommands.h"
#include "AGKCompiler2.h"
#include "Parser1.h"
#include "Parser2.h"

// Namespace
using namespace AGK;
using namespace AGK_Compiler;

#define SERVER_VERSION "2022.12.11"

app App;

int prevWidth = -1;
int prevHeight = -1;
HANDLE hStdIn = 0;
HANDLE hStdOut = 0;

// logging functions
const uint32_t maxLog = 30;
uString* sLog[ maxLog ];
uint32_t iCurrLogEntry = 0;

// AGK commands
static const uint32_t numAGKCommands = sizeof(agk_commands) / sizeof(sAGKCommand);

void TypeToString( uString& output, cProgramData* pProgramData, int type, int typeIndex )
{
	int isArray = 0;
	if ( type > AGK_VARIABLE_ARRAY_0 ) 
	{
		isArray = 1;
		type -= AGK_VARIABLE_ARRAY_0;
	}

	switch( type )
	{
		case AGK_VARIABLE_STRING: output = "String"; break;
		case AGK_VARIABLE_INTEGER: output = "Integer"; break;
		case AGK_VARIABLE_FLOAT: output = "Float"; break;
		case AGK_VARIABLE_TYPE:
		{
			if ( !pProgramData || typeIndex < 0 || typeIndex >= pProgramData->m_pTypes.m_iCount ) output = "UnknownType";
			else
			{
				output = pProgramData->m_pTypes.m_pData[ typeIndex ]->sNameOrig;
			}
		} break;
		default: output = "";
	}

	if ( isArray ) output.AppendAscii( "[]" );
}

const char* ReturnTypeToString( int type )
{
	switch( type )
	{
		case AGK_VARIABLE_STRING: return "String ";
		case AGK_VARIABLE_INTEGER: return "Integer ";
		case AGK_VARIABLE_FLOAT: return "Float ";
	}

	return "";
}

const sAGKCommand* FindFirstAGKCommand( const char *name )
{
	int high = numAGKCommands-1;
	int low = 0;

	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = strcmp( agk_commands[ mid ].szCommandNameLow, name );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else 
		{
			while( mid > 0 && strcmp( agk_commands[ mid-1 ].szCommandNameLow, name ) == 0 ) mid--;
			return &(agk_commands[ mid ]);
		}
	}

	return 0;
}

void AddToLog( const char* msg, const char* msg2 )
{
#ifdef LSP_SHOW_WINDOW
	uString* pString = 0;

	if ( iCurrLogEntry < maxLog ) 
	{
		pString = new uString();
		sLog[ iCurrLogEntry++ ] = pString;
	}
	else
	{
		pString = sLog[ 0 ];
		for( uint32_t i = 0; i < maxLog-1; i++ ) sLog[ i ] = sLog[ i + 1 ];
		sLog[ maxLog-1 ] = pString;
	}

	pString->Format( msg, msg2 );
#endif
}

void ClearLog()
{
	for( uint32_t i = 0; i < iCurrLogEntry; i++ ) 
	{
		delete sLog[ i ];
		sLog[ i ] = 0;
	}
	iCurrLogEntry = 0;
}

#define FILE_NEEDS_PARSING   0x01

class AGKFile
{
public:
	uString sFilePath;

	uString** m_pLines = 0;
	uint32_t m_iNumLines = 0;
	uint32_t m_iLineArraySize = 0;

	cProgramData* m_pProgramData = 0;
	uint32_t m_iFlags = FILE_NEEDS_PARSING;
	int64_t m_iModifiedTime = 0;
	float m_fPrevStartTime = -100;
	float m_fPrevTotalTime = -100;

	AGKFile( const char* path ) 
	{
		sFilePath.SetStr( path );
		sFilePath.Replace( '\\', '/' );
	}

	~AGKFile() 
	{
		DeleteAllLines();
	}

	// returns the token that covers the given line and char position
	// only works on file program data since it is ordered by line number with no includes
	int FindRawToken( int line, int character, int closest )
	{
		int high = m_pProgramData->m_iNumTokens-1;
		int low = 0;
		int mid = 0;

		// binary search
		while( high >= low )
		{
			mid = (high+low)/2;
			int result = m_pProgramData->GetToken( mid )->iLine - line;
			if( result > 0 ) high = mid-1;
			else if ( result < 0 ) low = mid+1;
			else 
			{
				int startChar = m_pProgramData->GetToken( mid )->iCharStart;
				int endChar = m_pProgramData->GetToken( mid )->iCharEnd;
				if ( character < startChar ) high = mid-1;
				else if ( character >= endChar ) low = mid+1;
				else return mid;
			}
		}

		if ( closest ) return mid;
		else return -1;
	}

	// returns the function name, if any, that contains the given token
	// returns 0 if not in a function, otherwise 1
	int GetInFunctionName( int token, uString& sFunctionName )
	{
		if ( !m_pProgramData ) return 0;
		if ( token >= (int)m_pProgramData->m_iNumTokens ) return 0;

		while( token > 0 )
		{
			token--;
			if ( m_pProgramData->GetToken( token )->iType == AGK_TOKEN_DEC_FUNCTION_END ) return 0;
			else if ( m_pProgramData->GetToken( token )->iType == AGK_TOKEN_DEC_FUNCTION )
			{
				if ( m_pProgramData->GetToken( token+1 )->iType != AGK_TOKEN_IDENTIFIER ) return 0;
				else
				{
					sFunctionName = m_pProgramData->GetToken( token+1 )->sValueRaw;
					return 1;
				}
			}
		}

		return 0;
	}

	void CheckLineArraySpace( uint32_t numNewLines )
	{
		if ( m_iLineArraySize < m_iNumLines+numNewLines )
		{
			if ( m_iLineArraySize <= 1 ) m_iLineArraySize = 2;
			else m_iLineArraySize = m_iLineArraySize + m_iLineArraySize/2;
			if ( m_iLineArraySize < numNewLines ) m_iLineArraySize = numNewLines + 1;
			uString** newArray = new uString*[ m_iLineArraySize ];
			if ( m_pLines )
			{
				memcpy( newArray, m_pLines, sizeof(uString*) * m_iNumLines );
				delete [] m_pLines;
			}
			m_pLines = newArray;
		}
	}

	void AppendLine( uString* pString )
	{
		CheckLineArraySpace( 1 );

		m_pLines[ m_iNumLines ] = pString;
		m_iNumLines++;
	}

	void InsertLine( uint32_t index, uString* pString )
	{
		if ( index >= m_iNumLines ) index = m_iNumLines;
		
		CheckLineArraySpace( 1 );

		for( int i = (int)m_iNumLines; i > (int)index; i-- ) 
		{
			m_pLines[ i ] = m_pLines[ i-1 ];
		}

		m_pLines[ index ] = pString;
		m_iNumLines++;
	}

	void DeleteLines( uint32_t first, uint32_t last )
	{
		if ( last < first ) return;
		if ( first >= m_iNumLines ) first = m_iNumLines - 1;
		if ( last >= m_iNumLines ) last = m_iNumLines - 1;

		uint32_t diff = (last - first) + 1;
		uint32_t newEnd = m_iNumLines - diff;

		for( uint32_t i = first; i < newEnd; i++ ) 
		{
			if ( i <= last ) delete m_pLines[ i ];
			m_pLines[ i ] = m_pLines[ i + diff ];
		}

		for( uint32_t i = newEnd; i < m_iNumLines; i++ ) 
		{
			m_pLines[ i ] = 0;
		}

		m_iNumLines = newEnd;
	}

	void DeleteAllLines()
	{
		for( uint32_t i = 0; i < m_iNumLines; i++ ) delete m_pLines[ i ];
		delete [] m_pLines;
		m_pLines = 0;
		m_iLineArraySize = 0;
		m_iNumLines = 0;
	}

	void ReplaceContents( uint32_t startLine, uint32_t startChar, uint32_t endLine, uint32_t endChar, uString& text )
	{
		if ( endLine >= m_iNumLines ) endLine = m_iNumLines - 1;
		if ( startLine > endLine ) return;

		if ( startLine == endLine )
		{
			if ( startChar != endChar )
			{
				// delete partial line
				uString sEnd;
				if ( endChar < m_pLines[ endLine ]->GetNumChars() )
				{
					m_pLines[ endLine ]->SubString( sEnd, endChar );
				}
				m_pLines[ startLine ]->Keep( startChar );
				m_pLines[ startLine ]->Append( sEnd );
			}
		}
		else
		{
			// delete lines
			uString sEnd;
			if ( endChar < m_pLines[ endLine ]->GetNumChars() )
			{
				m_pLines[ endLine ]->SubString( sEnd, endChar );
			}
			m_pLines[ startLine ]->Keep( startChar );
			m_pLines[ startLine ]->Append( sEnd );

			DeleteLines( startLine + 1, endLine );
		}

		// add new contents
		uString sEnd;
		if ( startChar < m_pLines[ startLine ]->GetNumChars() )
		{
			m_pLines[ startLine ]->SubString( sEnd, startChar );
			m_pLines[ startLine ]->Keep( startChar );
		}

		uint32_t currLine = startLine;
		const char* begin = text.GetStr();
		const char* pos = begin;
		int numBytes;
		while ( *pos )
		{
			int c = uString::DecodeUTF8Char( pos, &numBytes );
		
			if ( c == '\n' )
			{
				unsigned int newLength = (unsigned int) (pos - begin);
				uString* pString = m_pLines[ currLine ];
				if ( newLength > 0 ) 
				{
					pString->AppendN( begin, newLength );
					if ( pString->CharAt( pString->GetNumChars()-1 ) == '\r' ) pString->Trunc2( 1 );
				}
				currLine++;
				pString = new uString();
				InsertLine( currLine, pString );
				begin = pos + numBytes;
			}

			pos += numBytes;
		}

		unsigned int newLength = (unsigned int) (pos - begin);
		uString* pString = m_pLines[ currLine ];
		if ( newLength > 0 ) 
		{
			pString->AppendN( begin, newLength );
			if ( pString->CharAt( pString->GetNumChars()-1 ) == '\r' ) pString->Trunc2( 1 );
		}
		pString->Append( sEnd );

		//for( uint32_t i = startLine; i <= currLine; i++ ) AddToLog( "Line: \"%s\"", m_pLines[ i ]->GetStr() );

		m_iFlags |= FILE_NEEDS_PARSING;
		m_iModifiedTime = agk::GetUnixTime64();
	}

	void ParseLines( Parser1* parser, int includeID, cProgramData* pProgram )
	{
		uString sError;
		uString sLine;
		uString sLineLower;
		for( uint32_t i = 0; i < m_iNumLines; i++ )
		{
			sLine.SetStr( *(m_pLines[i]) );
			sLineLower = sLine;
			sLineLower.Lower();
			
			if ( !parser->ParseLine( includeID, sLineLower, sLine, i+1, sError, pProgram ) ) 
			{
				//goto parser_error;
			}

			pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", i + 1, includeID, sLine.GetLength(), sLine.GetLength());
		}
	}

	void UpdateContents( uString& sContents )
	{
		DeleteAllLines();

		const char* begin = sContents.GetStr();
		const char* pos = begin;
		int numBytes;
		while ( *pos )
		{
			int c = uString::DecodeUTF8Char( pos, &numBytes );
		
			if ( c == '\n' )
			{
				unsigned int newLength = (unsigned int) (pos - begin);
				uString* pString = new uString();
				if ( newLength > 0 ) 
				{
					pString->SetStrN( begin, newLength );
					if ( pString->CharAt( pString->GetNumChars()-1 ) == '\r' ) pString->Trunc2( 1 );
				}
				AppendLine( pString );
				begin = pos + numBytes;
			}

			pos += numBytes;
		}

		unsigned int newLength = (unsigned int) (pos - begin);
		uString* pString = new uString();
		if ( newLength > 0 ) 
		{
			pString->SetStrN( begin, newLength );
			if ( pString->CharAt( pString->GetNumChars()-1 ) == '\r' ) pString->Trunc2( 1 );
		}
		AppendLine( pString );

		m_iFlags |= FILE_NEEDS_PARSING;
		m_iModifiedTime = agk::GetUnixTime64();
	}

	void Parse( int ignoreTimeLimits=0 )
	{
		if ( (m_iFlags & FILE_NEEDS_PARSING) == 0 ) return;

		if ( !ignoreTimeLimits )
		{
			if ( m_fPrevTotalTime > 0.1 )
			{
				if ( agk::Timer() - m_fPrevStartTime < 10 ) return;
			}
			else 
			{
				if ( agk::Timer() - m_fPrevStartTime < 2 ) return;
			}
		}

		uString sTime;
		m_fPrevStartTime = agk::Timer();

		ResetCompilerErrors();
		SetCompilerMaxErrors( 30 );

		Parser1 parser1;
		Parser2 parser2;
		parser1.SetIncludeMode( INCLUDE_IGNORE );

		uString sFilename = sFilePath;
		int slash = sFilePath.RevFind( '/' );
		if ( slash >= 0 )
		{
			sFilePath.SubString( sFilename, slash+1 );
		}

		cProgramData* pNewProgramData = new cProgramData();
		pNewProgramData->SetSkipInstructions( 1 );
		pNewProgramData->AddIncludeFile( sFilename, 0, 0 );

		ParseLines( &parser1, 0, pNewProgramData );

		if ( parser1.HasUnclosedBlocks() )
		{
			goto parser_error;
		}

#ifdef LSP_SHOW_WINDOW
		float time = agk::Timer() - m_fPrevStartTime;
		sTime.Format( "%f", time * 1000 );
		AddToLog( "Parse1 Time: %sms", sTime );
		float newStart = agk::Timer();
#endif

		parser2.FirstPass( pNewProgramData, 0 );
		//if ( GetCompilerErrorCount() > 0 ) goto parser_error;

		parser2.ParseFunctions( pNewProgramData );
		//if ( GetCompilerErrorCount() > 0 ) goto parser_error;
		
		int consumed = 0;
		int tokenID = 0;
		while ( tokenID < (int) pNewProgramData->m_iNumTokens )
		{
			consumed = 0;
			int result = parser2.ParseStatement( 0, pNewProgramData, tokenID, consumed );
			if ( result <= 0 ) 
			{
				while( pNewProgramData->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pNewProgramData->m_iNumTokens )
				{
					tokenID++;
				}
			}
			else
			{
				tokenID += consumed;
			}
		}
		

#ifdef LSP_SHOW_WINDOW
		time = agk::Timer() - newStart;
		sTime.Format( "%f", time * 1000 );
		AddToLog( "Parse2 Time: %sms", sTime );
#endif

		m_fPrevTotalTime = agk::Timer() - m_fPrevStartTime;
		m_fPrevStartTime = agk::Timer();

		// success, replace old program data
		if ( m_pProgramData ) delete m_pProgramData;
		m_pProgramData = pNewProgramData;

		m_iFlags &= ~FILE_NEEDS_PARSING;
		return;

parser_error:
		delete pNewProgramData;
	}
};

cHashedList<AGKFile*> g_AGKFiles;

#define FOLDER_NEEDS_PARSING   0x01

class AGKFolder
{
public:

	uString sFolderPath;
	uint32_t m_iFlags = FOLDER_NEEDS_PARSING;
	cProgramData* m_pProgramData = 0;
	AGKSortValue64* m_pSortedTokens = 0;
	float m_fPrevStartTime = -100;
	float m_fPrevTotalTime = -100;
	int64_t m_iLastParseTime = 0;

	static DynArrayObject<AGKFolder> g_AGKFolders;
	static int g_iLastParsed;
	static float g_fLastSlowUpdateTime;

	static int CompareFolder( const void *a, const void *b )
	{
		AGKFolder* f1 = *((AGKFolder**)a);
		AGKFolder* f2 = *((AGKFolder**)b);

		return f1->sFolderPath.CompareTo( f2->sFolderPath );
	}

	static int CompareToken( const void *a, const void *b )
	{
		stToken* t1 = *((stToken**)a);
		stToken* t2 = *((stToken**)b);

		if ( t1->iIncludeFile < t2->iIncludeFile ) return -1;
		else if ( t1->iIncludeFile > t2->iIncludeFile ) return 1;
		else
		{
			if ( t1->iLine < t2->iLine ) return -1;
			else if ( t1->iLine > t2->iLine ) return -1;
			else
			{
				if ( t1->iCharStart < t2->iCharStart ) return -1;
				else if ( t1->iCharStart > t2->iCharStart ) return 1;
				else return 0;
			}
		}
	}

	static void Initialise()
	{
		g_AGKFolders.SetSorted( true, CompareFolder );
	}

	static int GetBestMatch( uString& sPath )
	{
		int longestMatch = 0;
		int index = -1;
		for ( int i = 0; i < g_AGKFolders.m_iCount; i++ )
		{
			int matchLength = g_AGKFolders.GetItem(i)->sFolderPath.GetMatchLength( sPath );
			if ( matchLength > longestMatch ) 
			{
				longestMatch = matchLength;
				index = i;
			}
		}

		return index;
	}

	static void SlowUpdate()
	{
		if ( g_AGKFolders.m_iCount == 0 ) return;
		if ( agk::Timer() - g_fLastSlowUpdateTime < 2 ) return;

		if ( g_iLastParsed >= g_AGKFolders.m_iCount ) g_iLastParsed = 0;
	
		int startLoop = g_iLastParsed;
		do
		{
			int parsed = g_AGKFolders.GetItem( g_iLastParsed )->Parse();
			g_iLastParsed++;
			if ( g_iLastParsed >= g_AGKFolders.m_iCount ) g_iLastParsed = 0;
			if ( parsed ) 
			{
				g_fLastSlowUpdateTime = agk::Timer();
				return;
			}
		} while( g_iLastParsed != startLoop );

		g_fLastSlowUpdateTime = agk::Timer();
	}

	AGKFolder() {}
	AGKFolder( const char* path ) 
	{
		sFolderPath.SetStr( path );
		sFolderPath.Replace( '\\', '/' );
	}
	~AGKFolder() {}

	AGKFolder& operator=( AGKFolder& other )
	{
		sFolderPath = other.sFolderPath;
		m_pProgramData = 0;
		m_pSortedTokens = 0;
		return *this;
	}

	// uses a sorted list, so can't return an index into the token array
	// returns 1 if found, 0 if not
	int FindRawToken( uString& includeFile, int line, int character, stToken& token )
	{
		if ( !m_pProgramData || !m_pSortedTokens ) return 0;
		uString sIncludeFile = includeFile;
		sIncludeFile.Lower();

		int includeIndex = -1;
		for ( int i = 0; i < m_pProgramData->m_pIncludeFiles.m_iCount; i++ )
		{
			uString sFullPath = sFolderPath;
			sFullPath.Lower();
			sFullPath.AppendAscii( "/" );
			sFullPath.Append( m_pProgramData->m_pIncludeFiles.m_pData[ i ]->sName );
			if ( !sFullPath.SimplifyPath() ) return 0;

			if ( sFullPath.CompareTo( sIncludeFile ) == 0 )
			{
				includeIndex = i;
				break;
			}
		}

		if ( includeIndex < 0 ) return 0;

		int high = m_pProgramData->m_iNumTokens-1;
		int low = 0;
		int mid = 0;

		uint64_t searchValue = includeIndex & 0xFFFF;
		searchValue <<= 32;
		searchValue |= (line & 0xFFFFFFFF);
		searchValue <<= 16;

		// binary search
		while( high >= low )
		{
			mid = (high+low)/2;
			uint64_t filteredValue = m_pSortedTokens[ mid ].iValue & 0xFFFFFFFFFFFF0000;
			if( filteredValue > searchValue ) high = mid-1;
			else if ( filteredValue < searchValue ) low = mid+1;
			else 
			{
				stToken* pToken = (stToken*) m_pSortedTokens[ mid ].ptr;
				int startChar = pToken->iCharStart;
				int endChar = pToken->iCharEnd;
				if ( character < startChar ) high = mid-1;
				else if ( character > endChar ) low = mid+1;
				else 
				{
					// if token is an identifier then be more lenient on the end point  
					if ( character == endChar && pToken->iType != AGK_TOKEN_IDENTIFIER ) low = mid+1;
					else
					{
						token = *pToken;
						return 1;
					}
				}
			}
		}

		return 0;
	}

	int Parse() 
	{
		// always needs parsing since files may have changed that aren't open in VSCode
		//if ( (m_iFlags & FOLDER_NEEDS_PARSING) == 0 ) return;

		// limit amount of time we spend parsing
		if ( m_fPrevTotalTime > 0.1 )
		{
			if ( agk::Timer() - m_fPrevStartTime < 10 ) return 0;
		}
		else 
		{
			if ( agk::Timer() - m_fPrevStartTime < 2 ) return 0;
		}

		if ( m_pProgramData )
		{
			int anythingChanged = 0;

			// check if files have changed
			for( int i = 0; i < m_pProgramData->m_pIncludeFiles.m_iCount; i++ )
			{
				stIncludeFile* pIncludeFile = m_pProgramData->m_pIncludeFiles.m_pData[ i ];
				uString sFullPath = sFolderPath;
				sFullPath.Append( "/" );
				sFullPath.Append( pIncludeFile->sNameOrig );
				if ( !sFullPath.SimplifyPath() ) 
				{
					anythingChanged = 1;
					break;
				}

				AGKFile* pFile = g_AGKFiles.GetItem( sFullPath.GetStr() );
				if ( pFile ) 
				{
					if ( pFile->m_iModifiedTime > m_iLastParseTime ) 
					{
						anythingChanged = 1;
						break;
					}
				}
				else 
				{
					int64_t modifiedTime = 0;
					uString sRawPath = "raw:";
					sRawPath.Append( sFullPath );
					if ( !cFile::GetModified64( sRawPath, modifiedTime ) || modifiedTime > m_iLastParseTime ) 
					{
						anythingChanged = 1;
						break;
					}
				}
			}

			if ( !anythingChanged ) return 0;
		}

		// do this before accessing any of the files, so any changes close to parse time will be detected
		m_iLastParseTime = agk::GetUnixTime64();

		uString sTime;
		m_fPrevStartTime = agk::Timer();

		ResetCompilerErrors();
		SetCompilerMaxErrors( 30 );
		
		Parser1 parser1;
		Parser2 parser2;
		
		parser1.SetIncludeMode( INCLUDE_NORMAL );
		parser2.SetSkipInstructions( 1 );

		uString sMainFile = sFolderPath;
		sMainFile.Append( "/main.agc" );

		cProgramData* pNewProgramData = new cProgramData( m_pProgramData ? m_pProgramData->m_iNumTokens + 1000 : 0 );
		pNewProgramData->SetSkipInstructions( 1 );
		pNewProgramData->AddIncludeFile( "main.agc", 0, 0 );

		uString sError;
		uString sLine;
		uString sLineLower;
		int iFileCount = 0;
		while ( iFileCount < pNewProgramData->m_pIncludeFiles.m_iCount )
		{
			stIncludeFile* pIncludeFile = pNewProgramData->m_pIncludeFiles.m_pData[ iFileCount ];
			uString sFullPath = sFolderPath;
			sFullPath.Append( "/" );
			sFullPath.Append( pIncludeFile->sNameOrig );
			if ( !sFullPath.SimplifyPath() )
			{
				goto parser_error;
			}

			// don't parse inserted files (type 2)
			if ( pIncludeFile->type < 2 )
			{
				AGKFile* pFile = g_AGKFiles.GetItem( sFullPath.GetStr() );
				if ( !pFile ) parser1.ParseFile( sFullPath.GetStr(), iFileCount, pNewProgramData );
				else pFile->ParseLines( &parser1, iFileCount, pNewProgramData );
			}
			
			++iFileCount;
		}

		if ( parser1.HasUnclosedBlocks() )
		{
			goto parser_error;
		}

#ifdef LSP_SHOW_WINDOW
		float time = agk::Timer() - m_fPrevStartTime;
		sTime.Format( "Time: %fms, Lines: %d, Tokens: %d", time * 1000, pNewProgramData->iLineCountTotal, pNewProgramData->m_iNumTokens );
		AddToLog( "Folder Parse1 %s", sTime );
		float newStart = agk::Timer();
#endif
		
		parser2.FirstPass( pNewProgramData );
		//if ( GetCompilerErrorCount() > 0 ) goto parser_error;
		
		parser2.ParseFunctions( pNewProgramData );
		//if ( GetCompilerErrorCount() > 0 ) goto parser_error;
		
		int consumed = 0;
		int tokenID = 0;
		while ( tokenID < (int) pNewProgramData->m_iNumTokens )
		{
			consumed = 0;
			int result = parser2.ParseStatement( 0, pNewProgramData, tokenID, consumed );
			if ( result <= 0 ) 
			{
				while( pNewProgramData->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pNewProgramData->m_iNumTokens )
				{
					tokenID++;
				}
			}
			else
			{
				tokenID += consumed;
			}
		}
		
#ifdef LSP_SHOW_WINDOW
		time = agk::Timer() - newStart;
		sTime.Format( "%f", time * 1000 );
		AddToLog( "Folder Parse2 Time: %sms", sTime );
		newStart = agk::Timer();
#endif

		m_fPrevTotalTime = agk::Timer() - m_fPrevStartTime;
		m_fPrevStartTime = agk::Timer();

		// success, replace old program data
		if ( m_pProgramData ) delete m_pProgramData;
		if ( m_pSortedTokens ) delete [] m_pSortedTokens;
		m_pProgramData = pNewProgramData;
		m_pSortedTokens = new AGKSortValue64[ m_pProgramData->m_iNumTokens ];
		for( uint32_t i = 0 ; i < m_pProgramData->m_iNumTokens; i++ )
		{
			stToken* pToken = m_pProgramData->m_pTokens[ i ];
			uint64_t value = pToken->iIncludeFile & 0xFFFF;
			value <<= 32;
			value |= (pToken->iLine & 0xFFFFFFFF);
			value <<= 16;
			value |= (pToken->iCharStart & 0xFFFF);

			m_pSortedTokens[ i ].iValue = value;
			m_pSortedTokens[ i ].ptr = pToken;
		}

		agk::SortArray64( m_pSortedTokens, m_pProgramData->m_iNumTokens );

#ifdef LSP_SHOW_WINDOW
		time = agk::Timer() - newStart;
		sTime.Format( "%f", time * 1000 );
		AddToLog( "Folder Sort Time: %sms", sTime );
#endif

		m_iFlags &= ~FOLDER_NEEDS_PARSING;
		return 1;

parser_error:
		delete pNewProgramData;
		return 1;
	}
};

DynArrayObject<AGKFolder> AGKFolder::g_AGKFolders;
int AGKFolder::g_iLastParsed = 0;
float AGKFolder::g_fLastSlowUpdateTime = 0;

int PraseDotLine( cProgramData* pProgramData, stFunction* pFunction, uString* pLine, uint32_t charNum, AGKTokenExtra& tokenExtra, void*& pExtraData )
{
	ResetCompilerErrors();
	SetCompilerMaxErrors( 30 );
		
	Parser1 parser1;
	Parser2 parser2;
		
	parser1.SetIncludeMode( INCLUDE_IGNORE );
	parser2.SetSkipInstructions( 1 );

	// create temporary program data to store line tokens
	cProgramData* pTempProgramData = new cProgramData();
	pTempProgramData->SetSkipInstructions( 1 );
	pTempProgramData->AddIncludeFile( "main.agc", 0, 0 );

	uString sError;
	uString sLine = *pLine;
	uString sLineLower = sLine;
	sLineLower.Lower();
	
	// parse line into tokens
	parser1.ParseLine( 0, sLineLower, sLine, 1, sError, pTempProgramData );
	if ( pTempProgramData->m_iNumTokens == 0 ) goto parser_error;
	
	// look for the specified dot token
	int dotToken = -1;
	for( int i = 0; i < (int)pTempProgramData->m_iNumTokens; i++ )
	{
		stToken* pToken = pTempProgramData->GetToken( i );
		if ( pToken->iCharStart < charNum && pToken->iCharEnd >= charNum && pToken->iType == AGK_TOKEN_DOT ) 
		{
			dotToken = i;
			break;
		}
	}

	if ( dotToken < 0 ) goto parser_error;

	// add tokens to main program to parse context them remove them later, it's a bit of a hack
	uint32_t oldNumTokens = pProgramData->m_iNumTokens;
	uint32_t lastEnd = 0;
	for( int i = 0; i <= dotToken; i++ )
	{
		stToken* pToken = pTempProgramData->GetToken( i );
		pProgramData->AddToken( pToken->iType, pToken->sValueRaw, pToken->sValueRawOrig, 1, 0, pToken->iCharStart, pToken->iCharEnd );
		lastEnd = pToken->iCharEnd;
	}

	// add some end of line tokens for parsing that looks ahead so it doesn't go off the end
	pProgramData->AddToken( AGK_TOKEN_EOL, "\n", "\\n", 1, 0, lastEnd+1, lastEnd+2 );
	pProgramData->AddToken( AGK_TOKEN_EOL, "\n", "\\n", 1, 0, lastEnd+1, lastEnd+2 );
	pProgramData->AddToken( AGK_TOKEN_EOL, "\n", "\\n", 1, 0, lastEnd+1, lastEnd+2 );

	// set option explicit to avoid adding variables
	int oldOptionExplicit = pProgramData->m_iOptionExplicit;
	pProgramData->m_iOptionExplicit = 1;

	uint32_t currToken = oldNumTokens;
	while ( currToken < pProgramData->m_iNumTokens )
	{
		int consumed = 0;
		int result = parser2.ParseStatement( pFunction, pProgramData, (int)currToken, consumed );
		if ( result <= 0 ) 
		{
			while( pProgramData->GetToken( currToken )->iType != AGK_TOKEN_EOL && currToken < pProgramData->m_iNumTokens )
			{
				currToken++;
			}
		}
		else
		{
			currToken += consumed;
		}
	}

	// set option explicit back to what it was
	pProgramData->m_iOptionExplicit = oldOptionExplicit;

	// get our dot token
	stToken* pToken = pProgramData->GetToken( oldNumTokens + dotToken );
	tokenExtra = pToken->iTypeExtra;
	pExtraData = pToken->pExtraData;
	
	// restore old token count, new tokens will be recycled by the program data automatically
	pProgramData->m_iNumTokens = oldNumTokens;
	delete pTempProgramData;
	return 1;

parser_error:
	delete pTempProgramData;
	return 0;
}

int FindDefinition( cProgramData* pProgram, uString& sToken, AGKTokenExtra agkTokenExtra, void* pExtraData )
{
	if ( !pProgram ) return -1;
	uString sTokenLower = sToken;
	sTokenLower.Lower();

	switch( agkTokenExtra )
	{
		case AGK_TOKEN_EXTRA_AGK_COMMAND:
		{
			return -1; // not defined in the user code, maybe go to a help page?
		} break;

		case AGK_TOKEN_EXTRA_USER_FUNCTION:
		{
			stFunction func; func.sName = sTokenLower;
			int index = pProgram->m_pFunctions.Find( &func );
			if ( index >= 0 ) return pProgram->m_pFunctions.m_pData[ index ]->iTokenID+1;
			else return -1;
		} break;

		case AGK_TOKEN_EXTRA_CONSTANT_NAME:
		{
			stConstant constant; constant.sName = sTokenLower;
			int index = pProgram->m_pConstants.Find( &constant );
			if ( index >= 0 ) return pProgram->m_pConstants.m_pData[ index ]->tokenID-1;
			else return -1;
		} break;

		case AGK_TOKEN_EXTRA_LABEL_NAME:
		{
			stLabel label; label.sName = sTokenLower;
			int index = pProgram->m_pLabels.Find( &label );
			if ( index >= 0 ) return pProgram->m_pLabels.m_pData[ index ]->iTokenID;
			else return -1;
		} break;

		case AGK_TOKEN_EXTRA_PLUGIN:
		{
			if ( !pExtraData ) return -1;
			stPlugin* pPlugin = (stPlugin*) pExtraData;
			return pPlugin->iTokenID;
		} break;

		case AGK_TOKEN_EXTRA_PLUGIN_FUNC:
		{
			if ( !pExtraData ) return -1;
			stPlugin* pPlugin = (stPlugin*) pExtraData;
			return pPlugin->iTokenID;
			// plugin commands aren't defined in tokens, so can only go to the import line
		} break;

		case AGK_TOKEN_EXTRA_VARIABLE:
		{
			stVariable var; var.sName = sTokenLower;
			int index = pProgram->m_pVariables.Find( &var );
			if ( index >= 0 && pProgram->m_pVariables.m_pData[ index ]->iInitialised ) 
			{
				return pProgram->m_pVariables.m_pData[ index ]->iTokenID;
			}
		} break;

		case AGK_TOKEN_EXTRA_TYPE:
		{
			stType type; type.sName = sTokenLower;
			int index = pProgram->m_pTypes.Find( &type );
			if ( index >= 0 ) return pProgram->m_pTypes.m_pData[ index ]->iTokenID+1;
			else return -1;
		} break;

		case AGK_TOKEN_EXTRA_ARRAY_FUNC: // array function statement e.g. .save(): .fromjson()
		{
			return -1; // not a token, maybe go to a help page?
		} break;

		case AGK_TOKEN_EXTRA_ARRAY_FUNC_EXP: // array function expression e.g. .tojson(): .find()
		{
			return -1; // not a token, maybe go to a help page?
		} break;

		case AGK_TOKEN_EXTRA_TYPE_FUNC: // e.g. type function statement .fromjson()
		{
			return -1; // not a token, maybe go to a help page?
		} break;

		case AGK_TOKEN_EXTRA_TYPE_FUNC_EXP: // type function expression e.g. .tojson()
		{
			return -1; // not a token, maybe go to a help page?
		} break;

		case AGK_TOKEN_EXTRA_TYPE_SUB_VARIABLE:
		{
			if ( !pExtraData ) return -1;
			stType* pType = (stType*) pExtraData;

			stVariable var; var.sName = sTokenLower;
			int index = pType->m_pSubVars.Find( &var );
			if ( index >= 0 ) return pType->m_pSubVars.m_pData[ index ]->iTokenID;
			else return -1;
		} break;

		case AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE:
		{
			if ( !pExtraData ) return -1;
			stFunction* pFunction = (stFunction*) pExtraData;

			stVariable var; var.sName = sTokenLower;
			int index = pFunction->pFuncVars.Find( &var );
			if ( index >= 0 ) return pFunction->pFuncVars.m_pData[ index ]->iTokenID;
			else return -1;
		} break;

		default:
		{
			stFunction func; func.sName = sTokenLower;
			int index = pProgram->m_pFunctions.Find( &func );
			if ( index >= 0 ) return pProgram->m_pFunctions.m_pData[ index ]->iTokenID+1;

			stType type; type.sName = sTokenLower;
			index = pProgram->m_pTypes.Find( &type );
			if ( index >= 0 ) return pProgram->m_pTypes.m_pData[ index ]->iTokenID+1;

			stLabel label; label.sName = sTokenLower;
			index = pProgram->m_pLabels.Find( &label );
			if ( index >= 0 ) return pProgram->m_pLabels.m_pData[ index ]->iTokenID;

			stConstant constant; constant.sName = sTokenLower;
			index = pProgram->m_pConstants.Find( &constant );
			if ( index >= 0 ) return pProgram->m_pConstants.m_pData[ index ]->tokenID-1;

			stVariable var; var.sName = sTokenLower;
			index = pProgram->m_pVariables.Find( &var );
			if ( index >= 0 && pProgram->m_pVariables.m_pData[ index ]->iInitialised ) 
			{
				return pProgram->m_pVariables.m_pData[ index ]->iTokenID;
			}
		}
	}

	return -1;
}

enum AGKTokenType
{
	TOKEN_TYPE_UNKNOWN = 0,
	TOKEN_TYPE_COMMAND,
	TOKEN_TYPE_KEYWORD,
	TOKEN_TYPE_FUNCTION,
	TOKEN_TYPE_TYPE,
	TOKEN_TYPE_LABEL,
	TOKEN_TYPE_CONSTANT,
	TOKEN_TYPE_VARIABLE
};

void* FindTokenDetailed( cProgramData* pProgram, uString& sToken, AGKTokenType& tokenType )
{
	if ( !pProgram ) return 0;
	uString sTokenLower = sToken;
	sTokenLower.Lower();

	stFunction func; func.sName = sTokenLower;
	int index = pProgram->m_pFunctions.Find( &func );
	if ( index >= 0 ) 
	{
		tokenType = TOKEN_TYPE_FUNCTION;
		return pProgram->m_pFunctions.m_pData[ index ];
	}

	stType type; type.sName = sTokenLower;
	index = pProgram->m_pTypes.Find( &type );
	if ( index >= 0 ) 
	{
		tokenType = TOKEN_TYPE_TYPE;
		return pProgram->m_pTypes.m_pData[ index ];
	}

	stLabel label; label.sName = sTokenLower;
	index = pProgram->m_pLabels.Find( &label );
	if ( index >= 0 ) 
	{
		tokenType = TOKEN_TYPE_LABEL;
		return pProgram->m_pLabels.m_pData[ index ];
	}

	stConstant constant; constant.sName = sTokenLower;
	index = pProgram->m_pConstants.Find( &constant );
	if ( index >= 0 ) 
	{
		tokenType = TOKEN_TYPE_CONSTANT;
		return pProgram->m_pConstants.m_pData[ index ];
	}

	stVariable var; var.sName = sTokenLower;
	index = pProgram->m_pVariables.Find( &var );
	if ( index >= 0 ) 
	{
		if ( pProgram->m_pVariables.m_pData[ index ]->iInitialised )
		{
			tokenType = TOKEN_TYPE_VARIABLE;
			return pProgram->m_pVariables.m_pData[ index ];
		}
	}

	return 0;
}

void app::Begin(void)
{
	if ( agk::GetRenderer() )
	{
		prevWidth = agk::GetDeviceWidth();
		prevHeight = agk::GetDeviceHeight();

		agk::SetVirtualResolution( prevWidth, prevHeight );
		agk::SetClearColor( 70,85,102 );
		agk::SetVSync(1);
		agk::SetScissor(0,0,0,0);
		agk::UseNewDefaultFonts( 1 );
		agk::SetPrintSize( 20 );
		agk::SetWindowTitle( "AppGameKit Language Server" );
	}

	hStdIn = ::GetStdHandle( STD_INPUT_HANDLE );
	hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE );

	AGKFolder::Initialise();

	Parser1::Parser1Init();

	uString error;
	if ( !cProgramData::LoadCommandsArrayGlobal( error ) )
	{
		AddToLog( "Failed to load AGK command list: %s", error.GetStr() );
	}
}

#define BUFFER_LEN 1024
char input[BUFFER_LEN+1] = { 0 };
char* pData = 0;
uint32_t iDataLength = 0;
uint32_t iDataTotalLength = 0;

enum ReadState
{
	READ_HEADER = 0,
	READ_CONTENT
};

ReadState readState = READ_HEADER;
int contentLength = 0;
int currOffset = 0;

// error codes
// reserved: -32099 to -32000
// reserved: -32899 to -32800
#define SERVER_ERROR_PARSE_ERROR      -32700
#define SERVER_ERROR_INVALID_REQUEST  -32600
#define SERVER_ERROR_METHOD_NOT_FOUND -32601
#define SERVER_ERROR_INVALID_PARAMS   -32602
#define SERVER_ERROR_INTERNAL_ERROR   -32603

#define SERVER_ERROR_NOT_INITIALIZED  -32002
#define SERVER_ERROR_UNKNOWN          -32001

#define SERVER_ERROR_REQUEST_FAILED   -32803  // correct request but failed to execute
#define SERVER_ERROR_CANCELLED        -32802  // should only be used if the request supports being cancelled
#define SERVER_ERROR_CONTENT_MODIFIED -32801  // something changed that shouldn't have
#define SERVER_ERROR_CLIENT_CANCELLED -32800  // request was cancelled by the client

#define SERVER_ERROR_CUSTOM           -31000

// methods
enum ServerMethod
{
	SERVER_METHOD_UNKNOWN = 0,
	SERVER_METHOD_INITIALIZE,
	SERVER_METHOD_INITIALIZE_DONE,
	SERVER_METHOD_FILE_OPEN,
	SERVER_METHOD_FILE_CLOSE,
	SERVER_METHOD_FILE_CHANGE,
	SERVER_METHOD_DOC_SYMBOLS,
	SERVER_METHOD_CODE_COMPLETE,
	SERVER_METHOD_WORKSPACE_CHANGE,
	SERVER_METHOD_DECLARATION,
	SERVER_METHOD_HOVER,
	SERVER_METHOD_SIGNATURE,
	SERVER_METHOD_SHUTDOWN
};

void SendError( int id, int errorCode, const char* errorMsg )
{
	// escape error message for JSON string
	uString sMsg = errorMsg;
	sMsg.ReplaceStr( "\\", "\\\\" );
	sMsg.ReplaceStr( "\n", "\\n" );
	sMsg.ReplaceStr( "\r", "\\r" );
	sMsg.ReplaceStr( "\t", "\\t" );
	sMsg.ReplaceStr( "\"", "\\\"" );

	uString response;
	response.Format( "{ \"id\":%d, \"error\":{ \"code\":%d, \"message\": \"%s\" } }", id, errorCode, sMsg.GetStr() );
	uString responseFull;
	responseFull.Format( "Content-Length: %d\r\n\r\n", response.GetLength() );
	responseFull.Append( response );
	WriteFile( hStdOut, responseFull.GetStr(), responseFull.GetLength(), 0, 0 );
}

void SendResponse( int id, const char* text )
{
	uString response;
	response.Format( "{ \"id\":%d, \"result\": %s }", id, text );
	uString responseFull;
	responseFull.Format( "Content-Length: %d\r\n\r\n", response.GetLength() );
	responseFull.Append( response );
	WriteFile( hStdOut, responseFull.GetStr(), responseFull.GetLength(), 0, 0 );
}

void OutputRange( uString& sResponse, int lineStart, int charStart, int lineEnd, int charEnd )
{
	sResponse.Append( "{\"start\":{\"line\":" );
	sResponse.AppendInt( lineStart );
	sResponse.Append( ",\"character\":" );
	sResponse.AppendInt( charStart ); 
	sResponse.Append( "},\"end\":{ \"line\":" );
	sResponse.AppendInt( lineEnd ); 
	sResponse.Append( ",\"character\":" );
	sResponse.AppendInt( charEnd ); 
	sResponse.Append( "}}" );
}

void OutputVariable( uString& sResponse, cProgramData* pProgramData, stVariable* pVar, int kind )
{
	int tokenID = pVar->iTokenID;
	uint32_t charStart = pProgramData->GetToken( tokenID )->iCharStart;
	uint32_t charEnd = pProgramData->GetToken( tokenID )->iCharEnd;
	int line = pProgramData->GetToken( tokenID )->iLine - 1;
	if ( line < 0 ) line = 0;

	sResponse.Append( "{\"name\": \"" ); 
	sResponse.Append( pVar->sNameOrig );
	sResponse.Append( "\",\"kind\":" );
	//if ( pVar->iType > AGK_VARIABLE_ARRAY_0 ) sResponse.AppendInt( 18 );
	//else if ( pVar->iType == AGK_VARIABLE_TYPE ) sResponse.AppendInt( 19 );
	//else sResponse.AppendInt( 13 );
	sResponse.AppendInt( kind );
	sResponse.Append( ",\"detail\":\"");
	switch( pVar->iType )
	{
		case AGK_VARIABLE_INTEGER:
		{
			sResponse.Append( "integer" );
		} break;
		case AGK_VARIABLE_FLOAT:
		{
			sResponse.Append( "float" );
		} break;
		case AGK_VARIABLE_STRING:
		{
			sResponse.Append( "string" );
		} break;
		case AGK_VARIABLE_TYPE:
		{
			sResponse.Append( pProgramData->m_pTypes.m_pData[ pVar->iTypeIndex ]->sNameOrig );
		} break;
		case AGK_VARIABLE_ARRAY_INT:
		{
			sResponse.Append( "integer [ ]" );
		} break;
		case AGK_VARIABLE_ARRAY_FLOAT:
		{
			sResponse.Append( "float [ ]" );
		} break;
		case AGK_VARIABLE_ARRAY_STRING:
		{
			sResponse.Append( "string [ ]" );
		} break;
		case AGK_VARIABLE_ARRAY_TYPE:
		{
			sResponse.Append( pProgramData->m_pTypes.m_pData[ pVar->iTypeIndex ]->sNameOrig );
			sResponse.Append( " [ ]" );
		} break;
	}
	if ( pVar->iScope == AGK_VARIABLE_GLOBAL )
	{
		sResponse.Append( " (global)" );
	}
	sResponse.Append( "\",\"range\":" );
	OutputRange( sResponse, line, charStart, line, charEnd );
	sResponse.Append( ",\"selectionRange\":" );
	OutputRange( sResponse, line, charStart, line, charEnd );
	sResponse.Append( "}" );
}

bool IsIdentifierChar( char c );
bool IsIdentifierCharFirst( char c );
bool IsIdentifierCharLast( char c );
bool IsWhiteSpace( char c );

int ParseJSONPosition( JSONObject* pObject, uint32_t* lineNum, uint32_t* charPos )
{
	// get position
	JSONElement* position = pObject->GetElement( "position" );
	if ( !position || position->GetType() != AGK_JSON_OBJECT ) return 0;
	JSONObject* positionObject = (JSONObject*) position;

	// get line
	JSONElement* line = positionObject->GetElement( "line" );
	if ( !line || line->GetType() != AGK_JSON_NUMBER ) return 0;
	JSONNumber* lineNumj = (JSONNumber*) line;
	if ( !lineNumj->m_iIsInt ) return 0;
	if ( lineNumj->m_iValue < 0 ) return 0;
	if ( lineNum ) *lineNum = lineNumj->m_iValue;

	// get character
	JSONElement* character = positionObject->GetElement( "character" );
	if ( !character || character->GetType() != AGK_JSON_NUMBER ) return 0;
	JSONNumber* characterNum = (JSONNumber*) character;
	if ( !characterNum->m_iIsInt ) return 0;
	if ( characterNum->m_iValue < 0 ) return 0;
	if ( charPos ) *charPos = characterNum->m_iValue;

	return 1;
}

void ProcessJSON( const char* json )
{
	JSONElement* root = JSONElement::LoadJSONFromData( json );

	if ( root->GetType() != AGK_JSON_OBJECT ) goto cleanup;
	JSONObject* rootObject = (JSONObject*) root;

	// get sequence number (if present)
	uint32_t idNum = 0;
	JSONElement* id = rootObject->GetElement( "id" );
	if ( id && id->GetType() == AGK_JSON_NUMBER ) 
	{
		JSONNumber* pID = (JSONNumber*) id;
		if ( pID->m_iIsInt != 0 ) idNum = pID->m_iValue;
	}

	ServerMethod iMethod = SERVER_METHOD_UNKNOWN;

	JSONElement* method = rootObject->GetElement( "method" );
	if ( !method || method->GetType() != AGK_JSON_STRING ) goto cleanup;
	JSONString* methodString = (JSONString*) method;

	if ( methodString->m_sValue.CompareCaseTo( "$/cancelRequest" ) == 0 ) goto cleanup;

	if ( methodString->m_sValue.CompareCaseTo( "initialize" ) == 0 ) iMethod = SERVER_METHOD_INITIALIZE;
	else if ( methodString->m_sValue.CompareCaseTo( "initialized" ) == 0 ) iMethod = SERVER_METHOD_INITIALIZE_DONE;
	else if ( methodString->m_sValue.CompareCaseTo( "textDocument/didOpen" ) == 0 ) iMethod = SERVER_METHOD_FILE_OPEN;
	else if ( methodString->m_sValue.CompareCaseTo( "textDocument/didClose" ) == 0 ) iMethod = SERVER_METHOD_FILE_CLOSE;
	else if ( methodString->m_sValue.CompareCaseTo( "textDocument/didChange" ) == 0 ) iMethod = SERVER_METHOD_FILE_CHANGE;
	else if ( methodString->m_sValue.CompareCaseTo( "textDocument/documentSymbol" ) == 0 ) iMethod = SERVER_METHOD_DOC_SYMBOLS;
	else if ( methodString->m_sValue.CompareCaseTo( "textDocument/completion" ) == 0 ) iMethod = SERVER_METHOD_CODE_COMPLETE;
	else if ( methodString->m_sValue.CompareCaseTo( "workspace/didChangeWorkspaceFolders" ) == 0 ) iMethod = SERVER_METHOD_WORKSPACE_CHANGE;
	else if ( methodString->m_sValue.CompareCaseTo( "textDocument/declaration" ) == 0 ) iMethod = SERVER_METHOD_DECLARATION;
	else if ( methodString->m_sValue.CompareCaseTo( "textDocument/hover" ) == 0 ) iMethod = SERVER_METHOD_HOVER;
	else if ( methodString->m_sValue.CompareCaseTo( "textDocument/signatureHelp" ) == 0 ) iMethod = SERVER_METHOD_SIGNATURE;
	else if ( methodString->m_sValue.CompareCaseTo( "shutdown" ) == 0 ) iMethod = SERVER_METHOD_SHUTDOWN;

	switch( iMethod )
	{
		case SERVER_METHOD_INITIALIZE:
		{
			AddToLog( "Initialize", "" );

			// get parameters
			JSONElement* params = rootObject->GetElement( "params" );
			if ( !params || params->GetType() != AGK_JSON_OBJECT ) goto cleanup;
			JSONObject* paramsObject = (JSONObject*) params;

			// get workspace folders
			JSONElement* folders = paramsObject->GetElement( "workspaceFolders" );
			if ( !folders || folders->GetType() != AGK_JSON_ARRAY ) goto cleanup;
			JSONArray* foldersArray = (JSONArray*) folders;
			for( uint32_t i = 0; i < foldersArray->m_iNumElements; i++ )
			{
				JSONElement* folder = foldersArray->GetElement( i );
				if ( !folder || folder->GetType() != AGK_JSON_OBJECT ) continue;
				JSONObject* folderObject = (JSONObject*) folder;

				// get folder uri
				JSONElement* uri = folderObject->GetElement( "uri" );
				if ( !uri || uri->GetType() != AGK_JSON_STRING ) continue;
				JSONString* uriString = (JSONString*) uri;
				uString sFolder;
				if ( uriString->m_sValue.CompareToN( "file:///", 8 ) == 0 ) uriString->m_sValue.SubString( sFolder, 8 );
				else sFolder = uriString->m_sValue;

				uString sFolderDecoded;
				agk::HTTPDecode( sFolder.GetStr(), sFolderDecoded );

				AGKFolder* pFolder = new AGKFolder( sFolderDecoded );
				if ( AGKFolder::g_AGKFolders.Add( pFolder, false, false ) >= 0 )
				{
					AddToLog( "Found Workspace Folder: %s", sFolderDecoded );
				}
				else
				{
					delete pFolder;
				}
			}

			SendResponse( idNum, "{ \"serverInfo\": { \"name\": \"AppGameKit Language Server\", \"version\": \"" SERVER_VERSION "\" }, \"capabilities\": \
			{ \
				\"textDocumentSync\": { \"openClose\": true, \"change\": 2 },\
				\"declarationProvider\": true,\
				\"completionProvider\": { \"triggerCharacters\":[\".\"] },\
				\"documentSymbolProvider\": true,\
				\"workspaceSymbolProvider\": false,\
				\"workspace\": { \"workspaceFolders\": { \"supported\": true, \"changeNotifications\": true } },\
				\"hoverProvider\": true,\
				\"signatureHelpProvider\": {\"triggerCharacters\": \"(,\"}\
			} }" );
		} break;

		case SERVER_METHOD_INITIALIZE_DONE:
		{
			// ignore
		} break;

		case SERVER_METHOD_WORKSPACE_CHANGE:
		{
			// get parameters
			JSONElement* params = rootObject->GetElement( "params" );
			if ( !params || params->GetType() != AGK_JSON_OBJECT ) goto cleanup;
			JSONObject* paramsObject = (JSONObject*) params;

			// get event
			JSONElement* event = paramsObject->GetElement( "event" );
			if ( !event || event->GetType() != AGK_JSON_OBJECT ) goto cleanup;
			JSONObject* eventObject = (JSONObject*) event;

			// get added folders
			JSONElement* folders = eventObject->GetElement( "added" );
			if ( !folders || folders->GetType() != AGK_JSON_ARRAY ) goto cleanup;
			JSONArray* foldersArray = (JSONArray*) folders;
			for( uint32_t i = 0; i < foldersArray->m_iNumElements; i++ )
			{
				JSONElement* folder = foldersArray->GetElement( i );
				if ( !folder || folder->GetType() != AGK_JSON_OBJECT ) continue;
				JSONObject* folderObject = (JSONObject*) folder;

				// get folder uri
				JSONElement* uri = folderObject->GetElement( "uri" );
				if ( !uri || uri->GetType() != AGK_JSON_STRING ) continue;
				JSONString* uriString = (JSONString*) uri;
				uString sFolder;
				if ( uriString->m_sValue.CompareToN( "file:///", 8 ) == 0 ) uriString->m_sValue.SubString( sFolder, 8 );
				else sFolder = uriString->m_sValue;

				uString sFolderDecoded;
				agk::HTTPDecode( sFolder.GetStr(), sFolderDecoded );

				AGKFolder* pFolder = new AGKFolder( sFolderDecoded );
				if ( AGKFolder::g_AGKFolders.Add( pFolder, false ) >= 0 )
				{
					AddToLog( "Added Workspace Folder: %s", sFolderDecoded );
				}
				else
				{
					delete pFolder;
				}
			}

			// get removed folders
			folders = eventObject->GetElement( "removed" );
			if ( !folders || folders->GetType() != AGK_JSON_ARRAY ) goto cleanup;
			foldersArray = (JSONArray*) folders;
			for( uint32_t i = 0; i < foldersArray->m_iNumElements; i++ )
			{
				JSONElement* folder = foldersArray->GetElement( i );
				if ( !folder || folder->GetType() != AGK_JSON_OBJECT ) continue;
				JSONObject* folderObject = (JSONObject*) folder;

				// get folder uri
				JSONElement* uri = folderObject->GetElement( "uri" );
				if ( !uri || uri->GetType() != AGK_JSON_STRING ) continue;
				JSONString* uriString = (JSONString*) uri;
				uString sFolder;
				if ( uriString->m_sValue.CompareToN( "file:///", 8 ) == 0 ) uriString->m_sValue.SubString( sFolder, 8 );
				else sFolder = uriString->m_sValue;

				uString sFolderDecoded;
				agk::HTTPDecode( sFolder.GetStr(), sFolderDecoded );

				AGKFolder pFolder( sFolderDecoded );
				int index = AGKFolder::g_AGKFolders.Find( &pFolder );
				if ( index >= 0 ) 
				{
					AGKFolder::g_AGKFolders.Remove( index );
					AddToLog( "Removed Workspace Folder: %s", sFolderDecoded );
				}
				else
				{
					AddToLog( "Removed Missing Workspace Folder: %s", sFolderDecoded );
				} 
			}
		} break;

		case SERVER_METHOD_FILE_OPEN:
		case SERVER_METHOD_FILE_CLOSE:
		case SERVER_METHOD_FILE_CHANGE:
		case SERVER_METHOD_DOC_SYMBOLS:
		case SERVER_METHOD_CODE_COMPLETE:
		case SERVER_METHOD_DECLARATION:
		case SERVER_METHOD_HOVER:
		case SERVER_METHOD_SIGNATURE:
		{
			// get parameters
			JSONElement* params = rootObject->GetElement( "params" );
			if ( !params || params->GetType() != AGK_JSON_OBJECT ) goto cleanup;
			JSONObject* paramsObject = (JSONObject*) params;

			// document
			JSONElement* document = paramsObject->GetElement( "textDocument" );
			if ( !document || document->GetType() != AGK_JSON_OBJECT ) goto cleanup;
			JSONObject* documentObject = (JSONObject*) document;
		
			// document URI
			JSONElement* uri = documentObject->GetElement( "uri" );
			if ( !uri || uri->GetType() != AGK_JSON_STRING ) goto cleanup;
			JSONString* uriString = (JSONString*) uri;
			uString sFile;
			if ( uriString->m_sValue.CompareToN( "file:///", 8 ) == 0 ) uriString->m_sValue.SubString( sFile, 8 );
			else sFile = uriString->m_sValue;

			uString sFileDecoded;
			agk::HTTPDecode( sFile.GetStr(), sFileDecoded );

			if ( iMethod == SERVER_METHOD_FILE_OPEN )
			{
				AddToLog( "Opened File: %s", sFileDecoded );

				// document contents
				JSONElement* content = documentObject->GetElement( "text" );
				if ( !content || content->GetType() != AGK_JSON_STRING ) goto cleanup;
				JSONString* contentString = (JSONString*) content;

				AGKFile* pFile = g_AGKFiles.GetItem( sFileDecoded.GetStr() );
				if ( !pFile ) 
				{
					pFile = new AGKFile( sFileDecoded );
					pFile->UpdateContents( contentString->m_sValue );
					g_AGKFiles.AddItem( pFile, sFileDecoded.GetStr() );
				}
			}
			else if ( iMethod == SERVER_METHOD_FILE_CLOSE )
			{
				AddToLog( "Closed File: %s", sFileDecoded );
				// keep the local file data for parsing
			}
			else if ( iMethod == SERVER_METHOD_FILE_CHANGE )
			{
				AddToLog( "Changed File: %s", sFileDecoded );

				AGKFile* pFile = g_AGKFiles.GetItem( sFileDecoded.GetStr() );
				if ( !pFile ) goto cleanup;

				// get change data
				JSONElement* changes = paramsObject->GetElement( "contentChanges" );
				if ( !changes || changes->GetType() != AGK_JSON_ARRAY ) goto cleanup;
				JSONArray* changesArray = (JSONArray*) changes;

				for( uint32_t i = 0; i < changesArray->m_iNumElements; i++ )
				{
					JSONElement* item = changesArray->GetElement( i );
					if ( !item || item->GetType() != AGK_JSON_OBJECT ) continue;
					JSONObject* itemObject = (JSONObject*) item;

					JSONElement* range = itemObject->GetElement( "range" );
					if ( !range || range->GetType() != AGK_JSON_OBJECT ) continue;
					JSONObject* rangeObject = (JSONObject*) range;

					JSONElement* start = rangeObject->GetElement( "start" );
					if ( !start || start->GetType() != AGK_JSON_OBJECT ) continue;
					JSONObject* startObject = (JSONObject*) start;

					JSONElement* startLine = startObject->GetElement( "line" );
					if ( !startLine || startLine->GetType() != AGK_JSON_NUMBER ) continue;
					JSONNumber* startLineNum = (JSONNumber*) startLine;
					if ( !startLineNum->m_iIsInt ) continue;

					JSONElement* startChar = startObject->GetElement( "character" );
					if ( !startChar || startChar->GetType() != AGK_JSON_NUMBER ) continue;
					JSONNumber* startCharNum = (JSONNumber*) startChar;
					if ( !startCharNum->m_iIsInt ) continue;

					JSONElement* end = rangeObject->GetElement( "end" );
					if ( !end || end->GetType() != AGK_JSON_OBJECT ) continue;
					JSONObject* endObject = (JSONObject*) end;

					JSONElement* endLine = endObject->GetElement( "line" );
					if ( !endLine || endLine->GetType() != AGK_JSON_NUMBER ) continue;
					JSONNumber* endLineNum = (JSONNumber*) endLine;
					if ( !endLineNum->m_iIsInt ) continue;

					JSONElement* endChar = endObject->GetElement( "character" );
					if ( !endChar || endChar->GetType() != AGK_JSON_NUMBER ) continue;
					JSONNumber* endCharNum = (JSONNumber*) endChar;
					if ( !endCharNum->m_iIsInt ) continue;

					JSONElement* text = itemObject->GetElement( "text" );
					if ( !text || text->GetType() != AGK_JSON_STRING ) continue;
					JSONString* textString = (JSONString*) text;

					uint32_t iStartLine = startLineNum->m_iValue;
					uint32_t iStartChar = startCharNum->m_iValue;
					uint32_t iEndLine = endLineNum->m_iValue;
					uint32_t iEndChar = endCharNum->m_iValue;

					pFile->ReplaceContents( iStartLine, iStartChar, iEndLine, iEndChar, textString->m_sValue );
				}
			}
			else if ( iMethod == SERVER_METHOD_DOC_SYMBOLS )
			{
				AddToLog( "Update Document Symbols", "" );

				// kind: 5=class, 11=interface, 12=function, 13=variable, 14=constant, 23=struct, 26=type param

				uString sResponse( "[" );

				AGKFile* pFile = g_AGKFiles.GetItem( sFileDecoded.GetStr() );
				if ( pFile ) 
				{
					pFile->Parse();
					if ( pFile->m_pProgramData )
					{
						int hasItems = 0;

						// global variables
						int varCount = pFile->m_pProgramData->m_pVariables.m_iCount;
						for( int i = 0; i < varCount; i++ )
						{
							stVariable* pVar = pFile->m_pProgramData->m_pVariables.m_pData[ i ];
							if ( !pVar->iInitialised ) continue;
							if ( hasItems ) sResponse.Append( "," );
							hasItems = 1;
							OutputVariable( sResponse, pFile->m_pProgramData, pVar, 13 );
						}

						// functions
						int funcCount = pFile->m_pProgramData->m_pFunctions.m_iCount;
						for( int i = 0; i < funcCount; i++ )
						{
							stFunction* pFunc = pFile->m_pProgramData->m_pFunctions.m_pData[ i ];
							int tokenID = pFunc->iTokenID;
							uint32_t charStart = pFile->m_pProgramData->GetToken( tokenID )->iCharStart;
							uint32_t charEnd = pFile->m_pProgramData->GetToken( tokenID )->iCharEnd;
							int line = pFile->m_pProgramData->GetToken( tokenID )->iLine - 1;
							if ( line < 0 ) line = 0;

							if ( hasItems ) sResponse.Append( "," );
							hasItems = 1;
							sResponse.Append( "{\"name\": \"" ); 
							sResponse.Append( pFunc->sNameOrig );
							sResponse.Append( "\",\"kind\":12,\"range\":" );
							OutputRange( sResponse, line, charStart, line, charEnd );
							sResponse.Append( ",\"selectionRange\":" );
							OutputRange( sResponse, line, charStart, line, charEnd );
							if ( pFunc->pFuncVars.m_iCount > 0 )
							{
								// function local variables
								sResponse.Append( ",\"children\":[" );
								for( int j = 0; j < pFunc->pFuncVars.m_iCount; j++ )
								{
									stVariable* pVar = pFunc->pFuncVars.m_pData[ j ];
									if ( j > 0 ) sResponse.Append( "," );
									OutputVariable( sResponse, pFile->m_pProgramData, pVar, 13 );
								}
								sResponse.Append( "]" );
							}
							sResponse.Append( "}" );
						}

						// types
						int typeCount = pFile->m_pProgramData->m_pTypes.m_iCount;
						for( int i = 0; i < typeCount; i++ )
						{
							stType* pType = pFile->m_pProgramData->m_pTypes.m_pData[ i ];
							int tokenID = pType->iTokenID;
							uint32_t charStart = pFile->m_pProgramData->GetToken( tokenID )->iCharStart;
							uint32_t charEnd = pFile->m_pProgramData->GetToken( tokenID )->iCharEnd;
							int line = pFile->m_pProgramData->GetToken( tokenID )->iLine - 1;
							if ( line < 0 ) line = 0;

							if ( hasItems ) sResponse.Append( "," );
							hasItems = 1;
							sResponse.Append( "{\"name\": \"" ); 
							sResponse.Append( pType->sNameOrig );
							sResponse.Append( "\",\"kind\":5,\"range\":" );
							OutputRange( sResponse, line, charStart, line, charEnd );
							sResponse.Append( ",\"selectionRange\":" );
							OutputRange( sResponse, line, charStart, line, charEnd );
							if ( pType->m_pSubVars.m_iCount > 0 )
							{
								// type variables
								sResponse.Append( ",\"children\":[" );
								for( int j = 0; j < pType->m_pSubVars.m_iCount; j++ )
								{
									stVariable* pVar = pType->m_pSubVars.m_pData[ j ];
									if ( j > 0 ) sResponse.Append( "," );
									OutputVariable( sResponse, pFile->m_pProgramData, pVar, 26 );
								}
								sResponse.Append( "]" );
							}
							sResponse.Append( "}" );
						}

						// constants
						int constantCount = pFile->m_pProgramData->m_pConstants.m_iCount;
						for( int i = 0; i < constantCount; i++ )
						{
							stConstant* pConstant = pFile->m_pProgramData->m_pConstants.m_pData[ i ];
							int tokenID = pConstant->tokenID;
							uint32_t charStart = pFile->m_pProgramData->GetToken( tokenID )->iCharStart;
							uint32_t charEnd = pFile->m_pProgramData->GetToken( tokenID )->iCharEnd;
							int line = pFile->m_pProgramData->GetToken( tokenID )->iLine - 1;
							if ( line < 0 ) line = 0;

							if ( hasItems ) sResponse.Append( "," );
							hasItems = 1;
							sResponse.Append( "{\"name\": \"" ); 
							sResponse.Append( pConstant->sNameOrig );
							sResponse.Append( "\",\"kind\":14,\"range\":" );
							OutputRange( sResponse, line, charStart, line, charEnd );
							sResponse.Append( ",\"selectionRange\":" );
							OutputRange( sResponse, line, charStart, line, charEnd );
							sResponse.Append( "}" );
						}

						// labels
						int labelCount = pFile->m_pProgramData->m_pLabels.m_iCount;
						for( int i = 0; i < labelCount; i++ )
						{
							stLabel* pLabel = pFile->m_pProgramData->m_pLabels.m_pData[ i ];
							int tokenID = pLabel->iTokenID;
							uint32_t charStart = pFile->m_pProgramData->GetToken( tokenID )->iCharStart;
							uint32_t charEnd = pFile->m_pProgramData->GetToken( tokenID )->iCharEnd;
							int line = pFile->m_pProgramData->GetToken( tokenID )->iLine - 1;
							if ( line < 0 ) line = 0;

							if ( hasItems ) sResponse.Append( "," );
							hasItems = 1;
							sResponse.Append( "{\"name\": \"" ); 
							sResponse.Append( pLabel->sNameOrig );
							sResponse.Append( "\",\"kind\":20,\"range\":" );
							OutputRange( sResponse, line, charStart, line, charEnd );
							sResponse.Append( ",\"selectionRange\":" );
							OutputRange( sResponse, line, charStart, line, charEnd );
							sResponse.Append( "}" );
						}
					}
				}

				sResponse.Append( "]" );

				SendResponse( idNum, sResponse );
			}
			else if ( iMethod == SERVER_METHOD_CODE_COMPLETE )
			{
				AddToLog( "Code Completion", sFileDecoded );

				// kind: 1=keyword(abc), 3=user function(red box), 5=unused(blue box), 6=variable(enclosed blue box), 7=type(tree), 8=keyword(circle), 9=command(curly brackets), 21=constant(text lines)

				int isDot = 0;
				do
				{
					JSONElement* context = paramsObject->GetElement( "context" );
					if ( !context || context->GetType() != AGK_JSON_OBJECT ) break;
					JSONObject* contextObject = (JSONObject*) context;

					JSONElement* trigger = contextObject->GetElement( "triggerCharacter" );
					if ( !trigger || trigger->GetType() != AGK_JSON_STRING ) break;
					JSONString* triggerObject = (JSONString*) trigger;

					if ( triggerObject->m_sValue.CompareTo( "." ) == 0 ) isDot = 1;
				} while(0);

				// need the file program data for detecting current function
				AGKFile* pFile = g_AGKFiles.GetItem(sFileDecoded.GetStr());
				if ( !pFile ) 
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}
					
				pFile->Parse( 1 );
				if ( !pFile->m_pProgramData )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				// get folder program data for all the definitions
				uString sFolder = sFileDecoded;
				sFolder.Replace( '\\', '/' );
				sFolder.Trunc( '/' );
				cProgramData* pProgramData = 0;

				int index = AGKFolder::GetBestMatch( sFolder );
				if ( index >= 0 ) 
				{
					// shouldn't need to parse the folder as it will be regularly parsed in the background
					// and don't need it to be completely up to date to use the definitions
					AGKFolder* pFolder = AGKFolder::g_AGKFolders.GetItem( index );
					if ( pFolder->m_pProgramData )
					{
						pProgramData = pFolder->m_pProgramData;
					}
				}
				
				if ( !pProgramData ) pProgramData = pFile->m_pProgramData;

				if ( !pProgramData )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				// get current token from file
				uint32_t lineNum, charNum;
				if ( !ParseJSONPosition( paramsObject, &lineNum, &charNum ) ) goto cleanup;

				if ( lineNum >= pFile->m_iNumLines ) goto cleanup;

				int token = pFile->FindRawToken( lineNum+1, charNum, 1 );
				if ( token < 0 )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				// find the function that contains this token, if any
				stFunction* pFunction = 0;
				uString sFunctionName;
				int inFunction = pFile->GetInFunctionName( token, sFunctionName );
				if ( inFunction )
				{
					stFunction findFunc; findFunc.sName = sFunctionName;
					int index = pProgramData->m_pFunctions.Find( &findFunc );
					if ( index >= 0 )
					{
						pFunction = pProgramData->m_pFunctions.GetItem( index );
					}
				}

				if ( isDot )
				{
					uString sLocation; sLocation.Format( "Line: %d, Char: %d", lineNum, charNum );
					AddToLog( "Dot Location: %s", sLocation );

					uString* pLine = pFile->m_pLines[ lineNum ];

					AGKTokenExtra tokenExtra;
					void* pExtraData;
					if ( !PraseDotLine( pProgramData, pFunction, pLine, charNum, tokenExtra, pExtraData ) )
					{
						SendResponse( idNum, "null" );
						goto cleanup;
					}

					uString sResponse;
					sResponse.SetStr( "{ \"isIncomplete\": false, \"items\": [" );

					switch( tokenExtra )
					{
						case AGK_TOKEN_EXTRA_ARRAY_DOT: // array statement dot
						{
							sResponse.AppendAscii("{\"label\":\"length\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"swap(index1,index2)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"insert(item)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"insert(item,index)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"insertSorted(item)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"remove(index)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"sort()\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"reverse()\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"save(filename)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"load(filename)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"fromJSON(string)\", \"kind\":9}");

							sResponse.Append( "]}" );
						} break;

						case AGK_TOKEN_EXTRA_ARRAY_EXP_DOT: // array expression dot
						{
							sResponse.AppendAscii("{\"label\":\"length\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"find(item)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"indexOf(item)\", \"kind\":9}");
							sResponse.AppendAscii(",{\"label\":\"toJSON()\", \"kind\":9}");

							sResponse.Append( "]}" );
						} break;

						case AGK_TOKEN_EXTRA_TYPE_DOT: // type statement dot
						{
							if ( !pExtraData ) 
							{
								sResponse = "null";
								break;
							}

							sResponse.AppendAscii("{\"label\":\"fromJSON(string)\", \"kind\":9}");

							stType* pType = (stType*) pExtraData;
							for( int i = 0; i < pType->m_pSubVars.m_iCount; i++ )
							{
								sResponse.AppendAscii(",");
								sResponse.AppendAscii("{\"label\":\"");
								sResponse.Append( pType->m_pSubVars.GetItem(i)->sNameOrig );
								sResponse.AppendAscii("\", \"kind\":6}");
							}

							sResponse.Append( "]}" );
						} break;

						case AGK_TOKEN_EXTRA_TYPE_EXP_DOT: // type expression dot
						{
							if ( !pExtraData ) 
							{
								sResponse = "null";
								break;
							}

							sResponse.AppendAscii("{\"label\":\"toJSON()\", \"kind\":9}");

							stType* pType = (stType*) pExtraData;
							for( int i = 0; i < pType->m_pSubVars.m_iCount; i++ )
							{
								sResponse.AppendAscii(",");
								sResponse.AppendAscii("{\"label\":\"");
								sResponse.Append( pType->m_pSubVars.GetItem(i)->sNameOrig );
								sResponse.AppendAscii("\", \"kind\":6}");
							}

							sResponse.Append( "]}" );
						} break;

						case AGK_TOKEN_EXTRA_PLUGIN_DOT:
						{
							if ( !pExtraData ) 
							{
								sResponse = "null";
								break;
							}

							uString sPrevName;
							stPlugin* pPlugin = (stPlugin*) pExtraData;
							for( int i = 0; i < pPlugin->m_iNumCommands; i++ )
							{
								if ( i != 0 ) sResponse.AppendAscii(",");

								if ( pPlugin->m_pCommands[ i ].sTier1Name.CompareTo( sPrevName ) == 0 ) continue;
								sPrevName = pPlugin->m_pCommands[ i ].sTier1Name;

								sResponse.AppendAscii("{\"label\":\"");
								sResponse.Append( pPlugin->m_pCommands[ i ].sTier1NameOrig );
								sResponse.AppendAscii("\", \"kind\":6}");
							}

							sResponse.Append( "]}" );
						} break;

						default:
						{
							sResponse = "null";
							break;
						}
					}

					SendResponse( idNum, sResponse );
				}
				else
				{
					uString sResponse;
					uString sPrevCmd;
					sResponse.SetStr( "{ \"isIncomplete\": false, \"items\": [" );
					int added = 0;
					for( uint32_t i = 0; i < numAGKCommands; i++ )
					{
						if ( sPrevCmd.CompareTo( agk_commands[ i ].szCommandNameLow ) == 0 ) continue;
						sPrevCmd = agk_commands[ i ].szCommandNameLow;
						if ( added > 0 ) sResponse.AppendAscii( "," );
						added++;
						sResponse.AppendAscii( "{\"label\":\"" );
						sResponse.Append( agk_commands[ i ].szCommandName );
						sResponse.AppendAscii( "\", \"kind\":9}" );
					}
				
					const stKeyword* pKeywords = Parser1::GetKeywords();
					int numKeywords = Parser1::GetNumKeywords();
					for( int i = 0; i < numKeywords; i++ )
					{
						sResponse.AppendAscii( "," );
						sResponse.AppendAscii( "{\"label\":\"" );
						sResponse.Append( pKeywords[ i ].szKeyword );
						sResponse.AppendAscii( "\", \"kind\":1}" );
					}
					
					// global variables
					int varCount = pProgramData->m_pVariables.m_iCount;
					for (int i = 0; i < varCount; i++)
					{
						stVariable* pVar = pProgramData->m_pVariables.m_pData[i];
						if ( pVar->iInitialised )
						{
							sResponse.AppendAscii(",");
							sResponse.AppendAscii("{\"label\":\"");
							sResponse.Append(pVar->sNameOrig);
							sResponse.AppendAscii("\", \"kind\":6}");
						}
					}

					// functions
					int funcCount = pProgramData->m_pFunctions.m_iCount;
					for (int i = 0; i < funcCount; i++)
					{
						stFunction* pFunc = pProgramData->m_pFunctions.m_pData[i];
						sResponse.AppendAscii(",");
						sResponse.AppendAscii("{\"label\":\"");
						sResponse.Append(pFunc->sNameOrig);
						sResponse.AppendAscii("\", \"kind\":3}");
					}

					// types
					int typeCount = pProgramData->m_pTypes.m_iCount;
					for (int i = 0; i < typeCount; i++)
					{
						stType* pType = pProgramData->m_pTypes.m_pData[i];
						sResponse.AppendAscii(",");
						sResponse.AppendAscii("{\"label\":\"");
						sResponse.Append(pType->sNameOrig);
						sResponse.AppendAscii("\", \"kind\":7}");
					}

					// constants
					int constantCount = pProgramData->m_pConstants.m_iCount;
					for (int i = 0; i < constantCount; i++)
					{
						stConstant* pConst = pProgramData->m_pConstants.m_pData[i];
						sResponse.AppendAscii(",");
						sResponse.AppendAscii("{\"label\":\"");
						sResponse.Append(pConst->sNameOrig);
						sResponse.AppendAscii("\", \"kind\":21}");
					}

					// plugins
					int pluginCount = pProgramData->m_pPluginRefs.m_iCount;
					for (int i = 0; i < pluginCount; i++)
					{
						stPluginRef* pPluginRef = pProgramData->m_pPluginRefs.m_pData[i];
						sResponse.AppendAscii(",");
						sResponse.AppendAscii("{\"label\":\"");
						sResponse.Append(pPluginRef->sRefNameOrig);
						sResponse.AppendAscii("\", \"kind\":21}");
					}

					if ( inFunction )
					{
						stFunction findFunc; findFunc.sName = sFunctionName;
						int index = pProgramData->m_pFunctions.Find( &findFunc );
						if ( index >= 0 )
						{
							stFunction* pFunction = pProgramData->m_pFunctions.GetItem( index );
							int varCount = pFunction->pFuncVars.m_iCount;
							for (int i = 0; i < varCount; i++)
							{
								stVariable* pVar = pFunction->pFuncVars.m_pData[i];
								if ( pVar->iInitialised )
								{
									sResponse.AppendAscii(",");
									sResponse.AppendAscii("{\"label\":\"");
									sResponse.Append(pVar->sNameOrig);
									sResponse.AppendAscii("\", \"kind\":6}");
								}
							}
						}
					}

					sResponse.Append( "]}" );

					SendResponse( idNum, sResponse );
				}
			}
			else if ( iMethod == SERVER_METHOD_DECLARATION )
			{
				AddToLog( "Get Declaration %s", sFileDecoded );

				uint32_t lineNum, charNum;
				if ( !ParseJSONPosition( paramsObject, &lineNum, &charNum ) ) goto cleanup;

				uString sLocation; sLocation.Format( "Line: %d, Char: %d", lineNum, charNum );
				AddToLog( "Location: %s", sLocation );

				AGKFile* pFile = g_AGKFiles.GetItem( sFileDecoded.GetStr() );
				if ( !pFile ) goto cleanup;

				if ( lineNum >= pFile->m_iNumLines ) goto cleanup;

				// we must use the programData to find the token instead of just reading the line text 
				// so we can determine if we are in a comment or string

				pFile->Parse( 1 );

				if ( !pFile->m_pProgramData )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				// find folder that most closely matches this file
				uString sFolder = sFileDecoded;
				sFolder.Replace( '\\', '/' );
				sFolder.Trunc( '/' );
				AGKFolder* pFolder = 0;
				int index = AGKFolder::GetBestMatch( sFolder );
				if ( index >= 0 ) 
				{
					pFolder = AGKFolder::g_AGKFolders.GetItem( index );
				}

				// we must use the programData to find the token instead of just reading the line text 
				// so we can determine if we are in a comment or string

				// must get the identifier from the file since if it is a constant it will get replaced during folder parsing
				int token = pFile->FindRawToken( lineNum+1, charNum, 0 );
				if ( token < 0 )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				AGKToken agkToken = pFile->m_pProgramData->GetToken( token )->iType;
				AGKToken agkPrevToken = AGK_TOKEN_UNKNOWN;
				if ( token > 0 ) agkPrevToken = pFile->m_pProgramData->GetToken( token-1 )->iType;

				// check for include path as a special case
				if ( pFolder && agkToken == AGK_TOKEN_STRING && (agkPrevToken == AGK_TOKEN_INCLUDE || agkPrevToken == AGK_TOKEN_INSERT) )
				{
					uString sIncludePath = pFile->m_pProgramData->GetToken( token )->sValueRaw;
					sIncludePath.Replace( '\\', '/' );

					uString sResponse;
					sResponse = "{\"uri\":\"file:///";
					sResponse.Append( pFolder->sFolderPath );
					sResponse.Append( "/" );
					sResponse.Append( sIncludePath );
					sResponse.Append( "\",\"range\":" );
					OutputRange( sResponse, 1, 0, 1, 0 );
					sResponse.Append( "}" );

					SendResponse( idNum, sResponse );
					goto cleanup;
				}

				// identifier could be variable, command, function, label, or constant
				if ( agkToken != AGK_TOKEN_IDENTIFIER )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				uString sIdentifier = pFile->m_pProgramData->GetToken( token )->sValueRawOrig;
				AddToLog( "File Identifier: %s", sIdentifier );
				sIdentifier.Lower();

				// look for command first as it is easy to check
				if ( FindFirstAGKCommand( sIdentifier ) ) 
				{
					// maybe go to help?

					SendResponse( idNum, "null" );
					goto cleanup;
				}

				int tokenFound = 0;
				stToken sToken;
				
				// must search the folder program data for the token as well so we can get more detailed information
				if ( pFolder ) 
				{
					pFolder->Parse();
					if ( pFolder->m_pProgramData )
					{
						// search for constant separately as it won't be a token after a folder parse
						stConstant constant; constant.sName = sIdentifier;
						int index = pFolder->m_pProgramData->m_pConstants.Find( &constant );
						if ( index >= 0 ) 
						{
							sToken.iTypeExtra = AGK_TOKEN_EXTRA_CONSTANT_NAME;
							sToken.pExtraData = 0;
							tokenFound = 1;
						}
						else
						{
							// find token that matches the line number
							tokenFound = pFolder->FindRawToken( sFileDecoded, lineNum+1, charNum, sToken );
						}
					}
				}
				
				// if not found in current folder, search all folders
				if ( !tokenFound )
				{
					for( int i = 0; i < AGKFolder::g_AGKFolders.m_iCount; i++ )
					{
						pFolder = AGKFolder::g_AGKFolders.GetItem( i );
						pFolder->Parse();
						if ( pFolder->m_pProgramData )
						{
							stConstant constant; constant.sName = sIdentifier;
							int index = pFolder->m_pProgramData->m_pConstants.Find( &constant );
							if ( index >= 0 ) 
							{
								sToken.iTypeExtra = AGK_TOKEN_EXTRA_CONSTANT_NAME;
								sToken.pExtraData = 0;
								tokenFound = 1;
							}
							else
							{
								// find token that matches the line number
								tokenFound = pFolder->FindRawToken( sFileDecoded, lineNum+1, charNum, sToken );
								if ( tokenFound ) break;
							}
						}
					}
				}

				if ( !tokenFound || !pFolder ) 
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				// extract token information
				AGKTokenExtra agkTokenExtra = sToken.iTypeExtra;
				void* pExtraPtr = sToken.pExtraData;

				// find token that defines the identifier
				token = FindDefinition( pFolder->m_pProgramData, sIdentifier, agkTokenExtra, pExtraPtr );
				if ( token < 0 )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				// output the location of the defintion
				int includeID = pFolder->m_pProgramData->GetToken( token )->iIncludeFile;
				int line = pFolder->m_pProgramData->GetToken( token )->iLine;
				if ( line > 0 ) line--;
				int startChar = pFolder->m_pProgramData->GetToken( token )->iCharStart;
				int endChar = pFolder->m_pProgramData->GetToken( token )->iCharEnd;
				if ( line == lineNum )
				{
					// work around VSCode bug that won't show declaration if it covers selection point
					endChar = startChar-1;
					startChar = 0;
				}

				uString sFullPath = pFolder->sFolderPath;
				sFullPath.AppendAscii( "/" );
				sFullPath.Append( pFolder->m_pProgramData->m_pIncludeFiles.m_pData[ includeID ]->sNameOrig );
				if ( !sFullPath.SimplifyPath() ) 
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				uString sResponse;
				sResponse = "{\"uri\":\"file:///";
				sResponse.Append( sFullPath );
				sResponse.Append( "\",\"range\":" );
				OutputRange( sResponse, line, startChar, line, endChar );
				sResponse.Append( "}" );

				SendResponse( idNum, sResponse );
			}
			else if ( iMethod == SERVER_METHOD_HOVER )
			{
				AddToLog( "Hover", sFileDecoded );

				uint32_t lineNum, charNum;
				if ( !ParseJSONPosition( paramsObject, &lineNum, &charNum ) ) goto cleanup;

				uString sLocation; sLocation.Format( "Line: %d, Char: %d", lineNum, charNum );
				AddToLog( "Location: %s", sLocation );

				AGKFile* pFile = g_AGKFiles.GetItem( sFileDecoded.GetStr() );
				if ( !pFile ) goto cleanup;

				if ( lineNum >= pFile->m_iNumLines ) goto cleanup;

				pFile->Parse( 1 );

				if ( !pFile->m_pProgramData )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				// we must use the programData to find the token instead of just reading the line text 
				// so we can determine if we are in a comment or string

				int token = pFile->FindRawToken( lineNum+1, charNum, 0 );
				if ( token < 0 )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				AGKToken agkToken = pFile->m_pProgramData->GetToken( token )->iType;

				// identifier could be variable, command, function, label, or constant
				// guaranteed not to be a string or comment
				if ( agkToken != AGK_TOKEN_IDENTIFIER )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				uString sIdentifier = pFile->m_pProgramData->GetToken( token )->sValueRawOrig;
				AddToLog( "File Identifier: %s", sIdentifier );
				sIdentifier.Lower();

				// look for constant in file first, file parsing won't replace constants whereas folder parsing does
				// so file is the only way to detect contants
				AGKTokenExtra agkTokenExtra = AGK_TOKEN_EXTRA_UNKNOWN;
				void* pExtraPtr = 0;
				const sAGKCommand* pCommand = 0;

				stConstant constant; constant.sName = sIdentifier;
				int index = pFile->m_pProgramData->m_pConstants.Find( &constant );
				if ( index >= 0 ) 
				{
					agkTokenExtra = AGK_TOKEN_EXTRA_CONSTANT_NAME;
				}
				else
				{
					// could also be a command, easy to check
					pCommand = FindFirstAGKCommand( sIdentifier );
					if ( pCommand ) agkTokenExtra = AGK_TOKEN_EXTRA_AGK_COMMAND;
				}
				
				// find folder that most closely matches this file
				uString sFolder = sFileDecoded;
				sFolder.Replace( '\\', '/' );
				sFolder.Trunc( '/' );
				
				int foundToken = 0;
				stToken sToken;
				AGKFolder* pFolder = 0;
				if ( agkTokenExtra != AGK_TOKEN_EXTRA_AGK_COMMAND )
				{
					index = AGKFolder::GetBestMatch( sFolder );
					if ( index >= 0 ) 
					{
						pFolder = AGKFolder::g_AGKFolders.GetItem( index );
						pFolder->Parse();
						if ( pFolder->m_pProgramData )
						{
							stConstant constant; constant.sName = sIdentifier;
							int index = pFolder->m_pProgramData->m_pConstants.Find( &constant );
							if ( index >= 0 ) 
							{
								agkTokenExtra = AGK_TOKEN_EXTRA_CONSTANT_NAME;
								foundToken = 1;
							}
							else
							{
								// find token that matches the line number
								foundToken = pFolder->FindRawToken( sFileDecoded, lineNum+1, charNum, sToken );
							}
						}
					}
				
					// if not found in current folder, search all folders
					if ( !foundToken )
					{
						for( int i = 0; i < AGKFolder::g_AGKFolders.m_iCount; i++ )
						{
							pFolder = AGKFolder::g_AGKFolders.GetItem( i );
							pFolder->Parse();
							if ( pFolder->m_pProgramData )
							{
								stConstant constant; constant.sName = sIdentifier;
								int index = pFolder->m_pProgramData->m_pConstants.Find( &constant );
								if ( index >= 0 ) 
								{
									agkTokenExtra = AGK_TOKEN_EXTRA_CONSTANT_NAME;
									foundToken = 1;
								}
								else
								{
									// find token that matches the line number
									foundToken = pFolder->FindRawToken( sFileDecoded, lineNum+1, charNum, sToken );
									if ( foundToken >= 0 ) break;
								}
							}
						}
					}

					if ( !foundToken || !pFolder ) 
					{
						SendResponse( idNum, "null" );
						goto cleanup;
					}

					if ( agkTokenExtra == AGK_TOKEN_EXTRA_UNKNOWN )
					{
						// extract token information
						agkTokenExtra = sToken.iTypeExtra;
						pExtraPtr = sToken.pExtraData;

						sIdentifier = sToken.sValueRawOrig;
						AddToLog( "Folder Identifier: %s", sIdentifier );
						sIdentifier.Lower();
					}
				}
				
				uString sResponse;
							
				switch( agkTokenExtra )
				{
					case AGK_TOKEN_EXTRA_AGK_COMMAND: // AGK command
					{
						if ( !pCommand ) pCommand = FindFirstAGKCommand( sIdentifier );
						if ( !pCommand )
						{
							sResponse = "null";
							break;
						}

						// can have multiple commands with the same name but different parameters, so use an array
						sResponse.SetStr( "{\"contents\": [" );
						unsigned int index = (unsigned int) (pCommand - agk_commands);
						unsigned startIndex = index;

						// go through all matching commands
						while( index < numAGKCommands && sIdentifier.CompareTo( agk_commands[ index ].szCommandNameLow ) == 0 )
						{ 
							pCommand = &(agk_commands[ index ]);
							if ( index != startIndex ) sResponse.AppendAscii( "," );
							sResponse.AppendAscii( "{\"language\": \"agksrc\", \"value\": \"" );

							// write command return type
							sResponse.Append( ReturnTypeToString(pCommand->iReturnType) );

							// write command name
							sResponse.Append( pCommand->szCommandName );
							sResponse.AppendAscii( "(" );

							// write command parameters
							for (int param = 0; param < pCommand->iNumParams; param++)
							{
								if ( param > 0 ) sResponse.Append( "," );
								sResponse.AppendAscii( " " );
								sResponse.Append( pCommand->szParamsName[param] );
								sResponse.AppendAscii( " as " );
								uString sType;
								TypeToString( sType, 0, pCommand->iParamsTypes[param], 0 ); // AGK commands currently don't accept AGK types, so can use 0
								sResponse.Append( sType );
							}

							// close command
							if ( pCommand->iNumParams > 0 ) sResponse.AppendAscii( " " );
							sResponse.AppendAscii( ")\"}" );

							index++;
						}
						sResponse.AppendAscii( "]}" );
					} break;

					case AGK_TOKEN_EXTRA_USER_FUNCTION: 
					{
						stFunction func; func.sName = sIdentifier;
						int index = pFolder->m_pProgramData->m_pFunctions.Find( &func );
						if ( index < 0 ) 
						{
							sResponse = "null";
							return;
						}

						stFunction* pFunc = pFolder->m_pProgramData->m_pFunctions.m_pData[ index ];
						int returnType = pFunc->iReturnType & 0x1f;
						int returnTypeType = pFunc->iReturnType >> 8;
						sResponse.SetStr( "{\"contents\": {\"language\": \"agksrc\", \"value\": \"" );
						uString sType;
									
						// write return type
						TypeToString( sType, pFolder->m_pProgramData, returnType, returnTypeType );
						sResponse.Append( sType );
						if ( sType.GetLength() > 0 ) sResponse.AppendAscii( " " );

						sResponse.AppendAscii( "function " );

						// write function name
						sResponse.Append( pFunc->sNameOrig );
						sResponse.AppendAscii( "(" );

						// write parameters
						for (int param = 0; param < pFunc->iNumParams; param++)
						{
							if ( param > 0 ) sResponse.Append( "," );
							sResponse.AppendAscii( " " );
							sResponse.Append( pFunc->iParamNames[param] );
							if ( pFunc->iParamRef[param] ) sResponse.AppendAscii( " ref as ");
							else sResponse.AppendAscii( " as " );
							int paramType = pFunc->iParamsTypes[param] & 0x1f;
							int paramTypeType = pFunc->iParamsTypes[param] >> 8;
							TypeToString( sType, pFolder->m_pProgramData, paramType, paramTypeType );
							sResponse.Append( sType );
						}

						// close function
						if ( pFunc->iNumParams > 0 ) sResponse.AppendAscii( " " );
						sResponse.AppendAscii( ")\"}}" );
					} break;

					case AGK_TOKEN_EXTRA_CONSTANT_NAME: 
					{
						stConstant constant; constant.sName = sIdentifier;
						int index = pFolder->m_pProgramData->m_pConstants.Find( &constant );
						if ( index < 0 ) 
						{
							sResponse = "null";
							break;
						}

						stConstant* pConst = pFolder->m_pProgramData->m_pConstants.m_pData[ index ];
						if (!pConst->valid)
						{
							sResponse = "null";
							break;
						}

						sResponse.SetStr( "{\"contents\": {\"language\": \"agksrc\", \"value\": \"" );

						// write constant name
						sResponse.Append( "#constant " );
						sResponse.Append( pConst->sNameOrig );

						// write constant contents
						for (int i = 0; i < pConst->length; i++)
						{
							sResponse.AppendAscii( " " );
							int tokenID = pConst->tokenID + i;
							if ( pFolder->m_pProgramData->GetToken( tokenID )->iType == AGK_TOKEN_STRING )
							{
								sResponse.AppendAscii( "\\\"" );
								sResponse.Append( pFolder->m_pProgramData->GetToken( tokenID )->sValueRaw );
								sResponse.AppendAscii( "\\\"" );
							}
							else
							{
								sResponse.Append( pFolder->m_pProgramData->GetToken( tokenID )->sValueRawOrig );
							}
						}

						// close constant
						sResponse.AppendAscii( "\"}}" );
					} break;

					case AGK_TOKEN_EXTRA_LABEL_NAME: 
					{
						sResponse = "null";
					} break;

					case AGK_TOKEN_EXTRA_TYPE: 
					{
						sResponse = "null";
					} break;

					case AGK_TOKEN_EXTRA_PLUGIN: 
					{
						sResponse.SetStr( "null" ); // todo
					} break;

					case AGK_TOKEN_EXTRA_PLUGIN_FUNC: 
					{
						sResponse.SetStr( "null" ); // todo
					} break;

					case AGK_TOKEN_EXTRA_VARIABLE: 
					{
						stVariable var; var.sName = sIdentifier;
						int index = pFolder->m_pProgramData->m_pVariables.Find( &var );
						if ( index < 0 ) 
						{
							sResponse = "null";
							break;
						}

						stVariable* pVar = pFolder->m_pProgramData->m_pVariables.m_pData[ index ];
						if ( !pVar->iInitialised )
						{
							sResponse = "null";
							break;
						}

						sResponse.SetStr( "{\"contents\": {\"language\": \"agksrc\", \"value\": \"" );

						if ( pVar->iScope = AGK_VARIABLE_GLOBAL ) sResponse.AppendAscii( "global " );
						else sResponse.AppendAscii( "local " );

						// write variable name
						sResponse.Append( pVar->sNameOrig );

						sResponse.AppendAscii( " as " );
									
						// write variable type
						uString sType;
						TypeToString( sType, pFolder->m_pProgramData, pVar->iType, pVar->iTypeIndex );
						sResponse.Append( sType );

						// close variable
						sResponse.AppendAscii( "\"}}" );
					} break;

					case AGK_TOKEN_EXTRA_ARRAY_FUNC: 
					{
						sResponse.SetStr( "null" ); // todo
					} break;

					case AGK_TOKEN_EXTRA_ARRAY_FUNC_EXP: 
					{
						sResponse.SetStr( "null" ); // todo
					} break;

					case AGK_TOKEN_EXTRA_TYPE_FUNC: 
					{
						sResponse.SetStr( "null" ); // todo
					} break;

					case AGK_TOKEN_EXTRA_TYPE_FUNC_EXP: 
					{
						sResponse.SetStr( "null" ); // todo
					} break;

					case AGK_TOKEN_EXTRA_TYPE_SUB_VARIABLE: 
					{
						if ( !pExtraPtr ) 
						{
							sResponse = "null";
							break;
						}

						stType* pType = (stType*) pExtraPtr;

						stVariable var; var.sName = sIdentifier;
						int index = pType->m_pSubVars.Find( &var );
						if ( index < 0 ) 
						{
							sResponse = "null";
							break;
						}

						stVariable* pVar = pType->m_pSubVars.m_pData[ index ];

						sResponse.SetStr( "{\"contents\": {\"language\": \"agksrc\", \"value\": \"" );

						sResponse.AppendAscii( "Type " );
						sResponse.Append( pType->sNameOrig );
						sResponse.AppendAscii( " -> " );

						// write variable name
						sResponse.Append( pVar->sNameOrig );

						sResponse.AppendAscii( " as " );
									
						// write variable type
						uString sType;
						TypeToString( sType, pFolder->m_pProgramData, pVar->iType, pVar->iTypeIndex );
						sResponse.Append( sType );

						// close variable
						sResponse.AppendAscii( "\"}}" );
					} break;

					case AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE: 
					{
						if ( !pExtraPtr ) 
						{
							sResponse = "null";
							break;
						}

						stFunction* pFunction = (stFunction*) pExtraPtr;

						stVariable var; var.sName = sIdentifier;
						int index = pFunction->pFuncVars.Find( &var );
						if ( index < 0 ) 
						{
							sResponse = "null";
							break;
						}

						stVariable* pVar = pFunction->pFuncVars.m_pData[ index ];
						if ( !pVar->iInitialised )
						{
							sResponse = "null";
							break;
						}

						sResponse.SetStr( "{\"contents\": {\"language\": \"agksrc\", \"value\": \"" );

						if ( pVar->iVarID < 0 || pVar->iCopyFrom < 0 ) sResponse.AppendAscii( "(parameter) " );
						else sResponse.AppendAscii( "(function local) " );

						// write variable name
						sResponse.Append( pVar->sNameOrig );

						sResponse.AppendAscii( " as " );
									
						// write variable type
						uString sType;
						TypeToString( sType, pFolder->m_pProgramData, pVar->iType, pVar->iTypeIndex );
						sResponse.Append( sType );

						// close variable
						sResponse.AppendAscii( "\"}}" );
					} break;

					default:
					{
						sResponse.SetStr( "null" );
					}
				}							

				SendResponse( idNum, sResponse );
			}
			else if ( iMethod == SERVER_METHOD_SIGNATURE )
			{
				AddToLog( "Signature Request", sFileDecoded );

				uint32_t lineNum, charNum;
				if ( !ParseJSONPosition( paramsObject, &lineNum, &charNum ) ) goto cleanup;

				uString sLocation; sLocation.Format( "Line: %d, Char: %d", lineNum, charNum );
				AddToLog( "Location: %s", sLocation );

				AGKFile* pFile = g_AGKFiles.GetItem( sFileDecoded.GetStr() );
				if ( !pFile ) goto cleanup;

				if ( lineNum >= pFile->m_iNumLines ) goto cleanup;
				
				pFile->Parse( 1 );
				if ( !pFile->m_pProgramData )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				int token = pFile->FindRawToken( lineNum+1, charNum-1, 1 );
				if ( token < 0 )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				// traverse back through the tokens to find the function name
				int nestCount = 0;
				int paramCount = 0;
				while ( token > 0 && (nestCount > 0 || pFile->m_pProgramData->GetToken(token)->iType != AGK_TOKEN_OPEN_BRACKET) )
				{
					switch (pFile->m_pProgramData->GetToken(token)->iType)
					{
						case AGK_TOKEN_COMMA: if ( nestCount == 0 ) paramCount++; break;
						case AGK_TOKEN_CLOSE_BRACKET: nestCount++; break;
						case AGK_TOKEN_OPEN_BRACKET: nestCount--; break;
					}
					token--;
				}

				if ( pFile->m_pProgramData->GetToken(token)->iType != AGK_TOKEN_OPEN_BRACKET )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				token--;

				if ( pFile->m_pProgramData->GetToken(token)->iType != AGK_TOKEN_IDENTIFIER )
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				uString sIdentifier = pFile->m_pProgramData->GetToken(token)->sValueRawOrig;
				uString msg = sIdentifier;
				msg.Append( " Parameter " );
				msg.AppendInt( paramCount );
				AddToLog( "Function: %s", msg );

				sIdentifier.Lower();

				// look for AGK command first, then look up AGK symbols like tokens or variables
				cProgramData* pProgramData = 0;
				AGKTokenType tokenType = TOKEN_TYPE_UNKNOWN;
				const void* tokenPtr = FindFirstAGKCommand( sIdentifier );
				if ( tokenPtr ) tokenType = TOKEN_TYPE_COMMAND;

				stFunction func; 
				func.sName = sIdentifier;
				
				// check file
				if ( !tokenPtr )
				{
					// check file first
					int index = pFile->m_pProgramData->m_pFunctions.Find( &func );
					if ( index >= 0 ) 
					{
						tokenType = TOKEN_TYPE_FUNCTION;
						tokenPtr = pFile->m_pProgramData->m_pFunctions.m_pData[ index ];
						pProgramData = pFile->m_pProgramData;
					}
				}

				// check nearest folder
				if ( !tokenPtr )
				{
					uString sFolder = sFileDecoded;
					sFolder.Replace( '\\', '/' );
					sFolder.Trunc( '/' );

					int index = AGKFolder::GetBestMatch( sFolder );
					if ( index >= 0 ) 
					{
						AGKFolder* pFolder = AGKFolder::g_AGKFolders.GetItem( index );
						//pFolder->Parse();
						if ( pFolder->m_pProgramData )
						{
							int index = pFolder->m_pProgramData->m_pFunctions.Find( &func );
							if ( index >= 0 ) 
							{
								tokenType = TOKEN_TYPE_FUNCTION;
								tokenPtr = pFolder->m_pProgramData->m_pFunctions.m_pData[ index ];
								pProgramData = pFolder->m_pProgramData;
							}
						}
					}
				}

				// check all folders
				if ( !tokenPtr )
				{
					for( int i = 0; i < AGKFolder::g_AGKFolders.m_iCount; i++ )
					{
						AGKFolder* pFolder = AGKFolder::g_AGKFolders.GetItem( i );
						//pFolder->Parse();
						if ( pFolder->m_pProgramData )
						{
							int index = pFolder->m_pProgramData->m_pFunctions.Find( &func );
							if ( index >= 0 ) 
							{
								tokenType = TOKEN_TYPE_FUNCTION;
								tokenPtr = pFolder->m_pProgramData->m_pFunctions.m_pData[ index ];
								pProgramData = pFolder->m_pProgramData;
								break;
							}
						}
					}
				}

				if ( !tokenPtr ) 
				{
					SendResponse( idNum, "null" );
					goto cleanup;
				}

				uString sResponse;

				switch( tokenType )
				{
					case TOKEN_TYPE_COMMAND: // AGK command
					{
						// can have multiple commands with the same name but different parameters, so use an array
						sResponse.SetStr( "{\"signatures\": [" );
						const sAGKCommand* pCommand = (sAGKCommand*) tokenPtr;
						unsigned int index = (unsigned int) (pCommand - agk_commands);
						unsigned startIndex = index;

						// go through all matching commands
						while( index < numAGKCommands && sIdentifier.CompareTo( agk_commands[ index ].szCommandNameLow ) == 0 )
						{ 
							pCommand = &(agk_commands[ index ]);

							if ( index != startIndex ) sResponse.AppendAscii( "," );
							sResponse.AppendAscii( "{\"label\": \"" );

							// write command return type
							sResponse.Append( ReturnTypeToString(pCommand->iReturnType) );

							// write command name
							sResponse.Append( pCommand->szCommandName );
							sResponse.AppendAscii( "(" );

							// write command parameters
							for (int param = 0; param < pCommand->iNumParams; param++)
							{
								if ( param > 0 ) sResponse.Append( "," );
								sResponse.AppendAscii( " " );
								sResponse.Append( pCommand->szParamsName[param] );
								sResponse.AppendAscii( " as " );
								uString sType;
								TypeToString( sType, 0, pCommand->iParamsTypes[param], 0 ); // AGK commands currently don't accept AGK types, so can use 0
								sResponse.Append( sType );
							}

							// close command
							if ( pCommand->iNumParams > 0 ) sResponse.AppendAscii( " " );
							sResponse.AppendAscii( ")\"" );  

							// write command parameters again
							if (pCommand->iNumParams > 0)
							{
								sResponse.AppendAscii( ",\"parameters\": [" ); 
									
								for (int param = 0; param < pCommand->iNumParams; param++)
								{
									if ( param > 0 ) sResponse.Append( "," );
									sResponse.AppendAscii( "{\"label\": \"" );
									sResponse.Append( pCommand->szParamsName[param] );
									sResponse.AppendAscii( " as " );
									uString sType;
									TypeToString( sType, 0, pCommand->iParamsTypes[param], 0 ); // AGK commands currently don't accept AGK types, so can use 0
									sResponse.Append( sType );
									sResponse.AppendAscii( "\"}" );
								}

								// close parameters
								sResponse.AppendAscii( "]" ); 
							}

							// close signature
							sResponse.AppendAscii( "}" ); 

							index++;
						}

						// close signature array, add active parameter
						sResponse.AppendAscii( "],\"activeParameter\":" );
						sResponse.AppendInt( paramCount );
						sResponse.AppendAscii( "}" );
					} break;

					case TOKEN_TYPE_FUNCTION: // user defined function
					{
						if ( !pProgramData )
						{
							SendResponse( idNum, "null" );
							goto cleanup;
						}

						stFunction* pFunc = (stFunction*) tokenPtr;
						int returnType = pFunc->iReturnType & 0x1f;
						int returnTypeType = pFunc->iReturnType >> 8;
						uString sType;
							
						sResponse.SetStr( "{\"signatures\": [{\"label\": \"" );
							
						// write return type
						TypeToString( sType, pProgramData, returnType, returnTypeType );
						sResponse.Append( sType );
						if ( sType.GetLength() > 0 ) sResponse.AppendAscii( " " );

						// write function name
						sResponse.Append( pFunc->sNameOrig );
						sResponse.AppendAscii( "(" );

						// write parameters
						for (int param = 0; param < pFunc->iNumParams; param++)
						{
							if ( param > 0 ) sResponse.Append( "," );
							sResponse.AppendAscii( " " );
							sResponse.Append( pFunc->iParamNames[param] );
							if ( pFunc->iParamRef[param] ) sResponse.AppendAscii( " ref as ");
							else sResponse.AppendAscii( " as " );
							int paramType = pFunc->iParamsTypes[param] & 0x1f;
							int paramTypeType = pFunc->iParamsTypes[param] >> 8;
							TypeToString( sType, pProgramData, paramType, paramTypeType );
							sResponse.Append( sType );
						}

						// close function
						if ( pFunc->iNumParams > 0 ) sResponse.AppendAscii( " " );
						sResponse.AppendAscii( ")\"" );

						// write parameters again
						if (pFunc->iNumParams > 0)
						{
							sResponse.AppendAscii( ",\"parameters\": [" ); 

							for (int param = 0; param < pFunc->iNumParams; param++)
							{
								if ( param > 0 ) sResponse.Append( "," );
								sResponse.AppendAscii( "{\"label\": \"" );
								sResponse.Append( pFunc->iParamNames[param] );
								if ( pFunc->iParamRef[param] ) sResponse.AppendAscii( " ref as ");
								else sResponse.AppendAscii( " as " );
								int paramType = pFunc->iParamsTypes[param] & 0x1f;
								int paramTypeType = pFunc->iParamsTypes[param] >> 8;
								TypeToString( sType, pProgramData, paramType, paramTypeType );
								sResponse.Append( sType );
								sResponse.AppendAscii( "\"}" );
							}

							// close parameters
							sResponse.AppendAscii( "]" ); 
						}

						// close signature array, add active parameter
						sResponse.AppendAscii( "}],\"activeParameter\":" );
						sResponse.AppendInt( paramCount );
						sResponse.AppendAscii( "}" );
					} break;

					default:
					{
						sResponse.SetStr( "null" );
					}
				}							

				SendResponse( idNum, sResponse );
			}
			else
			{
				uString methodID; methodID.Format( "%d", iMethod );
				AddToLog( "Missing file handler: ", methodID );
			}
		} break;

		case SERVER_METHOD_SHUTDOWN:
		{
			exit(0);
		}
	
		default:
		{
			AddToLog( "Unrecognised method: %s", methodString->m_sValue );

			uString sMsg; sMsg.Format( "Feature \"%s\" is not supported", methodString->m_sValue.GetStr() );
			SendError( idNum, SERVER_ERROR_METHOD_NOT_FOUND, sMsg );
		}
	}

cleanup:
	delete root;
}

int app::Loop (void)
{
	if ( agk::GetRenderer() )
	{
		int width = agk::GetDeviceWidth();
		int height = agk::GetDeviceHeight();

		if ( width != prevWidth || height != prevHeight )
		{
			prevWidth = agk::GetDeviceWidth();
			prevHeight = agk::GetDeviceHeight();
			agk::SetVirtualResolution( prevWidth, prevHeight );
		}
	}

	unsigned long avail = 0;
	PeekNamedPipe( hStdIn, NULL, 0, NULL, &avail, NULL );
	while( avail > 0 )
	{
		uint32_t bytes = avail;
		DWORD bytesRead = 0;
		if ( bytes > BUFFER_LEN ) bytes = BUFFER_LEN;
		int tryCount = 0;
		while( tryCount < 10 && !ReadFile( hStdIn, input, bytes, &bytesRead, NULL ) ) 
		{
			agk::Sleep( 5 );
			tryCount++;
		}
		if ( tryCount == 10 ) exit(0);
			
		if ( !pData || iDataTotalLength < iDataLength + bytesRead )
		{
			uint32_t newLength = iDataLength + bytesRead;
			char* pNewData = new char[ newLength ];
			if ( pData ) 
			{
				memcpy( pNewData, pData, iDataLength );
				delete [] pData;
			}
			pData = pNewData;
			iDataTotalLength = newLength;
		}

		memcpy( pData+iDataLength, input, bytesRead );
		iDataLength += bytesRead;

		avail -= bytesRead;
	}

	switch( readState )
	{
		case READ_HEADER:
		{
			if ( ((int)iDataLength) - currOffset > 0 )
			{
				const char* lineEnd = strchr( pData + currOffset, '\r' );
				if ( lineEnd != 0 )
				{
					int newOffset = (int) (lineEnd - pData);
					int length = newOffset - currOffset;
					if ( length == 0 )
					{
						readState = READ_CONTENT;
					}
					else
					{
						uString header;
						header.SetStrN( pData + currOffset, length );
						const char* szContentLength = "Content-Length:";
						if ( header.CompareToN( szContentLength, (int)strlen(szContentLength) ) == 0 )
						{
							uString sLength;
							header.SubString( sLength, (int)strlen(szContentLength) ); 
							sLength.Trim( " \t\r\n" );
							contentLength = sLength.ToInt();
						}
					}
					currOffset = newOffset + 2;
				}
			}
		} break;

		case READ_CONTENT:
		{
			int length = ((int)iDataLength) - currOffset;
			if ( length >= contentLength )
			{
				uString sJSON;
				sJSON.SetStrN( pData + currOffset, contentLength );

				ProcessJSON( sJSON.GetStr() );
				
				if ( length == contentLength )
				{
					iDataLength = 0;
					// shrink it if it is using a lot of memory
					if ( pData && iDataTotalLength > 2000000 )
					{
						iDataTotalLength = 2000000;
						delete [] pData;
						pData = new char[ iDataTotalLength ];
					}
				}
				else
				{
					uint32_t remaining = iDataLength - currOffset - contentLength; // above checks ensure this will greater than 0
					for( uint32_t i = 0; i < remaining; i++ )
					{
						pData[ i ] = pData[ i + currOffset + contentLength ];
					}
					iDataLength = remaining;
				}

				contentLength = 0;
				currOffset = 0;
				readState = READ_HEADER;
			}
		} break;
	}

	AGKFolder::SlowUpdate();
	
	if ( agk::GetRenderer() )
	{
		agk::Print( agk::ScreenFPS() );
		for( uint32_t i = 0; i < iCurrLogEntry; i++ ) 
		{
			agk::Print( *(sLog[ i ]) );
		}

		agk::Sync();
	}
	else
	{
		agk::Sleep( 10 );
	}

	return 0; // return 1 to close app
}


void app::End (void)
{

}
