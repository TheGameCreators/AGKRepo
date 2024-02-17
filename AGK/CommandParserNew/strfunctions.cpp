#include <windows.h>
#include <stdio.h>
#include "strfunctions.h"

void strupper( char* str )
{
	if ( !str ) return;
	while ( *str ) { int ch = *str; ch = toupper( ch ); *str = ch; str++; }
}

void strlower( char* str )
{
	if ( !str ) return;
	while ( *str ) { int ch = *str; ch = tolower( ch ); *str = ch; str++; }
}

//remove single and double quotes from the ends of a string
void strtrimquotes( char *str )
{
	char *origstr = str;
	if ( !str ) return;

	if ( *str == '"' || *str == '\'' )
	{
		char *str2 = new char [ strlen(str) + 1 ];
		while ( *str && (*str == '"' || *str == '\'') ) str++;
		
		strcpy( str2, str );
		strcpy( origstr, str2 );
		str = origstr;
		delete [] str2;
	}

	DWORD dwLength = (DWORD) strlen( str );
	if ( dwLength <= 0 ) return;

	if ( *(str + (dwLength-1)) == '"' || *(str + (dwLength-1)) == '\'' )
	{
		int i = dwLength - 1;
		while ( i > 0 && ( *(str + i) == '"' || *(str + i) == '\'' ) ) i--;

		*(str + i + 1) = '\0';
	}
}

//remove specified tokens from the ends of a string
void strtrim( char *str, const char *tokens )
{
	char *origstr = str;
	if ( !str ) return;

	if ( strchr(tokens,*str) )
	{
		char *str2 = new char [ strlen(str) + 1 ];
		while ( *str && strchr(tokens,*str) ) str++;
		
		strcpy( str2, str );
		strcpy( origstr, str2 );
		str = origstr;
		delete [] str2;
	}

	DWORD dwLength = (DWORD) strlen( str );
	if ( dwLength <= 0 ) return;

	if ( strchr( tokens,*(str + (dwLength-1)) ) )
	{
		int i = dwLength - 1;
		while ( i > 0 && strchr( tokens,*(str + i) ) ) i--;

		*(str + i + 1) = '\0';
	}
}

void strstrip( char *str, const char *tokens )
{
	if ( !str ) return;
	char *origstr = str;
	
	char *str2 = new char [ strlen(str) + 1 ];
	int iIndex = 0;

	while ( *str && strchr(tokens,*str) ) str++;

	//copy across any non-space character
	while ( *str )
	{
		str2[ iIndex++ ] = *str;
		str++;
		while ( *str && strchr(tokens,*str) ) str++;
	}

	str2[ iIndex++ ] = '\0';
		
	strcpy( origstr, str2 );
	delete [] str2;
}

//returns the next non token character
char nextrealchar( const char *str, const char *tokens )
{
	if ( !str ) return 0;
	while ( *str && strchr( tokens, *str ) ) str++;
	return *str;
}

int ConvertType( const char *szStr )
{
	// raw strings
	if ( strncmp( szStr, "const char*", strlen("const char*") ) == 0 ) return AGK_DATA_TYPE_STRING;
	if ( strncmp( szStr, "const char *", strlen("const char *") ) == 0 ) return AGK_DATA_TYPE_STRING;
	if ( strncmp( szStr, "char*", strlen("char*") ) == 0 ) return AGK_DATA_TYPE_STRING;
	if ( strncmp( szStr, "char *", strlen("char *") ) == 0 ) return AGK_DATA_TYPE_STRING;
	
	// arrays
	if ( strncmp( szStr, "int*", strlen("int*") ) == 0 ) return AGK_DATA_TYPE_ARRAY_INT;
	if ( strncmp( szStr, "int *", strlen("int *") ) == 0 ) return AGK_DATA_TYPE_ARRAY_INT;
	if ( strncmp( szStr, "float*", strlen("float*") ) == 0 ) return AGK_DATA_TYPE_ARRAY_FLOAT;
	if ( strncmp( szStr, "float *", strlen("float *") ) == 0 ) return AGK_DATA_TYPE_ARRAY_FLOAT;
	if ( strncmp( szStr, "uString*", strlen("uString*") ) == 0 ) return AGK_DATA_TYPE_ARRAY_STRING;
	if ( strncmp( szStr, "uString *", strlen("uString *") ) == 0 ) return AGK_DATA_TYPE_ARRAY_STRING;

	// basic types
	if ( strncmp( szStr, "int", strlen("int") ) == 0 ) return AGK_DATA_TYPE_INT;
	if ( strncmp( szStr, "unsigned int", strlen("unsigned int") ) == 0 ) return AGK_DATA_TYPE_INT;
	if ( strncmp( szStr, "UINT", strlen("UINT") ) == 0 ) return AGK_DATA_TYPE_INT;
	if ( strncmp( szStr, "uint32_t", strlen("uint32_t") ) == 0 ) return AGK_DATA_TYPE_INT;
	if ( strncmp( szStr, "DWORD", strlen("DWORD") ) == 0 ) return AGK_DATA_TYPE_INT;
	if ( strncmp( szStr, "float", strlen("float") ) == 0 ) return AGK_DATA_TYPE_FLOAT;
	//if ( strncmp( szStr, "bool", strlen("bool") ) == 0 ) return "B";
	//if ( strncmp( szStr, "void*", strlen("void*") ) == 0 ) return AGK_DATA_TYPE_INT;
	//if ( strncmp( szStr, "void *", strlen("void *") ) == 0 ) return AGK_DATA_TYPE_INT;
	if ( strncmp( szStr, "void", strlen("void") ) == 0 ) return AGK_DATA_TYPE_VOID;

	if ( strchr( szStr, '*' ) != 0 ) return AGK_DATA_TYPE_POINTER;

	// unsupported types
	/*
	if ( strncmp( szStr, "cImage ", 6 ) == 0 ) return -1;
	if ( strncmp( szStr, "cImage*", 6 ) == 0 ) return -1;
	if ( strncmp( szStr, "cSprite ", 6 ) == 0 ) return -1;
	if ( strncmp( szStr, "cSprite*", 6 ) == 0 ) return -1;
	if ( strncmp( szStr, "uString ", 6 ) == 0 ) return -1;
	if ( strncmp( szStr, "uString*", 6 ) == 0 ) return -1;
	*/

	Error( szStr );
	return -1;
}

const char* ConvertTypeName( const char *szStr )
{
	const char* orig = szStr;

	const char* star = strrchr( szStr, '*' ); // assume star is the last character of the parameter type
	if ( star ) return star+1;

	while( *szStr == ' ' ) szStr++;
	if ( strncmp(szStr, "const", strlen("const")) == 0 ) szStr += strlen("const");
	while( *szStr == ' ' ) szStr++;

	if ( strncmp(szStr, "unsigned", strlen("unsigned")) == 0 ) szStr += strlen("unsigned");
	while( *szStr == ' ' ) szStr++;

	if ( strncmp(szStr, "char", strlen("char")) == 0 ) szStr += strlen("char");
	if ( strncmp(szStr, "int", strlen("int")) == 0 ) szStr += strlen("int");
	if ( strncmp(szStr, "float", strlen("float")) == 0 ) szStr += strlen("float");
	if ( strncmp(szStr, "uString", strlen("uString")) == 0 ) szStr += strlen("uString");
	if ( strncmp(szStr, "UINT", strlen("UINT")) == 0 ) szStr += strlen("UINT");
	if ( strncmp(szStr, "uint32_t", strlen("uint32_t")) == 0 ) szStr += strlen("uint32_t");
	if ( strncmp(szStr, "DWORD", strlen("DWORD")) == 0 ) szStr += strlen("DWORD");
	if ( strncmp(szStr, "void", strlen("void")) == 0 ) szStr += strlen("void");
	
	if ( szStr == orig ) Error( szStr );
	return szStr;
}

const char* TypeString1( int type )
{
	switch( type )
	{
		case AGK_DATA_TYPE_STRING: return "S";
		case AGK_DATA_TYPE_INT: return "L";
		case AGK_DATA_TYPE_FLOAT: return "F";
		case AGK_DATA_TYPE_ARRAY_INT: return "A";
		case AGK_DATA_TYPE_ARRAY_FLOAT: return "B";
		case AGK_DATA_TYPE_ARRAY_STRING: return "C";
		case AGK_DATA_TYPE_POINTER: return "P";
		case AGK_DATA_TYPE_VOID: return "0";
	}

	return "0";
}

const char* TypeString2( int type )
{
	switch( type )
	{
		case AGK_DATA_TYPE_STRING: return "String";
		case AGK_DATA_TYPE_INT: return "Integer";
		case AGK_DATA_TYPE_FLOAT: return "Float";
		case AGK_DATA_TYPE_ARRAY_INT: return "Integer Array";
		case AGK_DATA_TYPE_ARRAY_FLOAT: return "Float Array";
		case AGK_DATA_TYPE_ARRAY_STRING: return "String Array";
		case AGK_DATA_TYPE_POINTER: return "Pointer";
		case AGK_DATA_TYPE_VOID: return "";
	}

	return "0";
}

const char* TypeStringC( int type )
{
	switch( type )
	{
		case AGK_DATA_TYPE_STRING: return "char*";
		case AGK_DATA_TYPE_INT: return "int";
		case AGK_DATA_TYPE_FLOAT: return "float";
		case AGK_DATA_TYPE_ARRAY_INT: return "int*";
		case AGK_DATA_TYPE_ARRAY_FLOAT: return "float*";
		case AGK_DATA_TYPE_ARRAY_STRING: return "uString*";
		case AGK_DATA_TYPE_POINTER: return "void*";
		case AGK_DATA_TYPE_VOID: return "void";
	}

	return "void";
}

//returns true if szStr is in szArray
bool inarray( const char **szArray, int iArraycount, const char *szStr )
{
	if ( !szArray || !szStr ) return false;

	for ( int i = 0; i < iArraycount; i++ )
	{
		if ( szArray[ i ] && strcmp(szArray[ i ], szStr) == 0 ) return true;
	}

	return false;
}

int csvCount( const char *str )
{
	if (!str) return 0;
	if (*str == '\0' ) return 0;

	int iCount = 1;
	const char *szCurr = str;
	const char *szComma;

	szComma = strchr( szCurr, ',' );
	while ( szComma )
	{
		if ( nextrealchar(szCurr) == '"' )	//is this value quoted?
		{
			szCurr = strchr( szCurr, '"' );		//start quote
			szCurr = strchr( szCurr+1, '"' );	//end quote
			if ( !szCurr ) 
			{
				//mismatched quote
				szCurr = str;
				//possible broken line?
				return iCount;
			}
			else szCurr++;
		}

		szComma = strchr( szCurr, ',' );
		if ( !szComma ) return iCount;
		else
		{
			iCount++;
			szCurr = szComma+1;
			szComma = strchr( szCurr, ',' );
		}
	}

	return iCount;
}

//explode a string of csv values into an array of strings
int csvExplode( const char *str, char** out, int maxCount ) 
{
	if (!str) return 0;
	if (*str == '\0' ) return 0;

	int iCount = 0;
	const char *szCurr = str;
	const char *szColStart = str;
	const char *szComma;

	szComma = strchr( szCurr, ',' );
	while ( szComma )
	{
		szColStart = szCurr;

		if ( nextrealchar(szCurr) == '"' )	//is this value quoted?
		{
			szCurr = strchr( szCurr, '"' );		//start quote
			szCurr = strchr( szCurr+1, '"' );	//end quote
			if ( !szCurr ) 
			{
				//mismatched quote
				szCurr = str;
				//possible broken line?
				return iCount;
			}
			else szCurr++;
		}

		szComma = strchr( szCurr, ',' );
		if ( szComma )
		{
			if ( iCount >= maxCount ) return iCount+1;

			DWORD dwLength = (DWORD) (szComma - szColStart);
			out[ iCount ] = new char[ dwLength + 1 ];
			strncpy_s( out[ iCount ], dwLength+1, szColStart, dwLength );
			out[ iCount ][ dwLength ] = '\0';
			strtrim( out[ iCount ], " \"\t\n\r" );
			strstrip( out[ iCount ], "&" );

			iCount++;
			szCurr = szComma+1;
			szComma = strchr( szCurr, ',' );
		}
		else
		{
			szCurr = szColStart;
		}
	}

	if ( iCount >= maxCount ) return iCount+1;

	//last column
	if ( *szCurr )
	{
		DWORD dwLength = (DWORD) strlen(szCurr);
		out[ iCount ] = new char[ dwLength + 1 ];
		strncpy_s( out[ iCount ], dwLength+1, szColStart, dwLength );
		out[ iCount ][ dwLength ] = '\0';
		strtrim( out[ iCount ], " \"\t\n\r" );
		strstrip( out[ iCount ], "&" );
	}
	else
	{
		out[ iCount ] = new char[ 1 ];
		out[ iCount ][ 0 ] = '\0';
	}

	iCount++;

	return iCount;
}