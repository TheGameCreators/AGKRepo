// Includes
#include "template.h"

// Namespace
using namespace AGK;

app App;

void BuildCaseFoldingData( FILE *fp )
{
	unsigned int *startValues = 0;
	unsigned int *endValues = 0;
	unsigned int **mappings = 0;
	int arrayCount = 0;

	char *szLine = 0;
	char *szPtr = 0;
	int prev = -1000;
	char hex[ 10 ];

	// count groups, should be about 17 as of Unicode 9.0
	agk::OpenToRead( 1, "CaseFolding.txt" );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			// check folding type, only use common and full
			szPtr = field+2;
			if ( *szPtr != 'C' && *szPtr != 'F' ) continue;

			if ( iCharCode > prev+200 ) 
			{
				arrayCount++;
			}

			prev = iCharCode;
		}
	}

	// reset
	if ( szLine ) delete [] szLine;
	szLine = 0;

	startValues = new unsigned int[ arrayCount ];
	endValues = new unsigned int[ arrayCount ];
	mappings = new unsigned int*[ arrayCount ];
	prev = -1000;
	arrayCount = 0;
	
	// count characters per group, max group is 579 characters as of Unicode 9.0
	agk::SetFilePos( 1, 0 );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			// check folding type, only use common and full
			szPtr = field+2;
			if ( *szPtr != 'C' && *szPtr != 'F' ) continue;

			if ( iCharCode > prev+200 ) 
			{
				startValues[ arrayCount ] = iCharCode;
				if ( arrayCount > 0 )
				{
					endValues[ arrayCount-1 ] = prev;
					int count = prev - startValues[ arrayCount-1 ] + 1;
					mappings[ arrayCount-1 ] = new unsigned int[ count ];
					for ( int i = 0; i < count; i++ ) mappings[ arrayCount-1 ][ i ] = 0;
				}
				arrayCount++;
			}

			prev = iCharCode;
		}
	}

	if ( arrayCount > 0 )
	{
		endValues[ arrayCount-1 ] = prev;
		int count = prev - startValues[ arrayCount-1 ] + 1;
		mappings[ arrayCount-1 ] = new unsigned int[ count ];
		for ( int i = 0; i < count; i++ ) mappings[ arrayCount-1 ][ i ] = 0;
	}

	// reset
	if ( szLine ) delete [] szLine;
	szLine = 0;

	prev = startValues[0];
	arrayCount = 0;

	// add character mappings to the mappings array
	agk::SetFilePos( 1, 0 );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );
			
			// check folding type, only use common and full
			szPtr = field+2;
			if ( *szPtr != 'C' && *szPtr != 'F' ) continue;

			if ( iCharCode > prev+200 ) 
			{
				arrayCount++;
			}

			prev = iCharCode;

			int index = iCharCode - startValues[ arrayCount ];
			if ( mappings[ arrayCount ][ index ] != 0 )
			{
				agk::Message( "Something went wrong, the mapping already has a value" );
				return;
			}

			// find the replacements
			field = strchr( szPtr, ';' );
			if ( !field ) continue;
			szPtr = field+1;
			if ( *szPtr == ' ' ) szPtr++; // start of replacements
			field = strchr( szPtr, ';' ); // end of replacements
			if ( !field ) continue;
			*field = 0;

			unsigned int replaceChars[ 3 ] = {0,0,0};
			unsigned int replaceCount = 0;
			char *space = szPtr;
			while ( space = strchr(szPtr, ' ') ) 
			{
				*space = 0;
				strcpy( hex, szPtr );
				replaceChars[ replaceCount < 4 ? replaceCount : 3 ] = (unsigned int)strtol( hex, 0, 16 );
				replaceCount++;
				szPtr = space+1;
			}

			strcpy( hex, szPtr );
			replaceChars[ replaceCount < 4 ? replaceCount : 3 ] = (unsigned int)strtol( hex, 0, 16 );
			replaceCount++;

			if ( replaceCount > 3 )
			{
				agk::Message( "Replacement count is great than 3, changes are needed to handle this case" );
				return;
			}

			mappings[ arrayCount ][ index ] = (replaceCount-1) << 30;
			switch( replaceCount )
			{
				case 1: 
				{
					mappings[ arrayCount ][ index ] |= replaceChars[ 0 ];
					break;
				}
				case 2: 
				{
					if ( replaceChars[ 0 ] > 0x7FFF || replaceChars[ 1 ] > 0x7FFF )
					{
						agk::Message( "2 replace chars has a value greater than 0x7FFF, can't handle this case" );
						return;
					}
					mappings[ arrayCount ][ index ] |= replaceChars[ 0 ];
					mappings[ arrayCount ][ index ] |= (replaceChars[ 1 ] << 15);
					break;
				}
				case 3: 
				{
					if ( replaceChars[ 0 ] > 0x3FF || replaceChars[ 1 ] > 0x3FF || replaceChars[ 2 ] > 0x3FF )
					{
						agk::Message( "3 replace chars has a value greater than 0x3FF, can't handle this case" );
						return;
					}
					mappings[ arrayCount ][ index ] |= replaceChars[ 0 ];
					mappings[ arrayCount ][ index ] |= (replaceChars[ 1 ] << 10);
					mappings[ arrayCount ][ index ] |= (replaceChars[ 2 ] << 20);
					break;
				}
			}
		}
	}

	if ( szLine ) delete [] szLine;
	szLine = 0;

	arrayCount++;

	agk::CloseFile( 1 );

	// write data to header file
	
	char str[ 256 ];
	// start values
	strcpy( str, "int g_iCaseFoldingStartValues[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++ )
	{
		sprintf( str, "%d", startValues[ arrayIndex ] );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount-1 ) fwrite( ", ", 1, 2, fp );
	}
	fwrite( " };\n\n", 1, 5, fp );

	// end values
	strcpy( str, "int g_iCaseFoldingEndValues[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++ )
	{
		sprintf( str, "%d", endValues[ arrayIndex ] );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount-1 ) fwrite( ", ", 1, 2, fp );
	}
	fwrite( " };\n\n", 1, 5, fp );

	// replacements
	for ( int arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++ )
	{
		sprintf( str, "int g_iCaseFoldingReplacements%d[] = { ", arrayIndex );
		fwrite( str, 1, strlen(str), fp );

		int count = endValues[arrayIndex] - startValues[arrayIndex] + 1;
		for ( int i = 0; i < count; i++ )
		{
			sprintf( str, "%d", mappings[ arrayIndex ][ i ] );
			fwrite( str, 1, strlen(str), fp );
			if ( i < count-1 ) fwrite( ", ", 1, 2, fp );

			if ( i > 0 && i % 35 == 0 ) fwrite( " \\\n", 1, 3, fp );
		}
		fwrite( " };\n\n", 1, 5, fp );
	}
	
	strcpy( str, "int *g_iCaseFoldingReplacements[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++ )
	{
		sprintf( str, "g_iCaseFoldingReplacements%d", arrayIndex );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount-1 ) fwrite( ", ", 1, 2, fp );

		if ( arrayIndex > 0 && arrayIndex % 4 == 0 ) fwrite( " \\\n", 1, 3, fp );
	}
	fwrite( " };\n", 1, 3, fp );
}

void BuildLowercaseData( FILE *fp )
{
	unsigned int *startValues2 = 0;
	unsigned int *endValues2 = 0;
	unsigned int **mappings2 = 0;
	int arrayCount2 = 0;

	char *szLine = 0;
	char *szPtr = 0;
	int prev = -1000;
	char hex[ 10 ];

	agk::OpenToRead( 1, "UnicodeData.txt" );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			int fieldCount = 0;
			szPtr = field+1;
			while( fieldCount < 12 && (field = strchr(szPtr,';')) )
			{
				fieldCount++;
				szPtr = field+1;
			}

			if ( fieldCount != 12 )
			{
				agk::Message( "Could not find lower case field" );
				return;
			}

			field = strchr(szPtr,';');
			if ( field ) *field = 0;
			if ( field == szPtr || *szPtr == 0 ) continue;
			
			strcpy( hex, szPtr );
			int iLowerCode = strtol( hex, 0, 16 );
			if ( iLowerCode == iCharCode ) continue;
			
			if ( iCharCode > prev+200 ) 
			{
				arrayCount2++;
			}

			prev = iCharCode;
		}
	}

	if ( szLine ) delete [] szLine;
	szLine = 0;

	startValues2 = new unsigned int[ arrayCount2 ];
	endValues2 = new unsigned int[ arrayCount2 ];
	mappings2 = new unsigned int*[ arrayCount2 ];

	// reset
	arrayCount2 = 0;
	prev = -1000;

	agk::SetFilePos( 1, 0 );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			int fieldCount = 0;
			szPtr = field+1;
			while( fieldCount < 12 && (field = strchr(szPtr,';')) )
			{
				fieldCount++;
				szPtr = field+1;
			}

			if ( fieldCount != 12 )
			{
				agk::Message( "Could not find lower case field" );
				return;
			}

			field = strchr(szPtr,';');
			if ( field ) *field = 0;
			if ( field == szPtr || *szPtr == 0 ) continue;

			strcpy( hex, szPtr );
			int iLowerCode = strtol( hex, 0, 16 );
			if ( iLowerCode == iCharCode ) continue;
						
			if ( iCharCode > prev+200 ) 
			{
				startValues2[ arrayCount2 ] = iCharCode;
				if ( arrayCount2 > 0 )
				{
					endValues2[ arrayCount2-1 ] = prev;
					int count = prev - startValues2[ arrayCount2-1 ] + 1;
					mappings2[ arrayCount2-1 ] = new unsigned int[ count ];
					for ( int i = 0; i < count; i++ ) mappings2[ arrayCount2-1 ][ i ] = 0;
				}
				arrayCount2++;
			}

			prev = iCharCode;
		}
	}

	if ( arrayCount2 > 0 )
	{
		endValues2[ arrayCount2-1 ] = prev;
		int count = prev - startValues2[ arrayCount2-1 ] + 1;
		mappings2[ arrayCount2-1 ] = new unsigned int[ count ];
		for ( int i = 0; i < count; i++ ) mappings2[ arrayCount2-1 ][ i ] = 0;
	}

	if ( szLine ) delete [] szLine;
	szLine = agk::ReadLine( 1 );

	// reset
	prev = startValues2[0];
	arrayCount2 = 0;

	// add character mappings to the mappings array
	agk::SetFilePos( 1, 0 );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			int fieldCount = 0;
			szPtr = field+1;
			while( fieldCount < 12 && (field = strchr(szPtr,';')) )
			{
				fieldCount++;
				szPtr = field+1;
			}

			if ( fieldCount != 12 )
			{
				agk::Message( "Could not find lower case field" );
				return;
			}

			field = strchr(szPtr,';');
			if ( field ) *field = 0;
			if ( field == szPtr || *szPtr == 0 ) continue;

			strcpy( hex, szPtr );
			int iLowerCode = strtol( hex, 0, 16 );
			if ( iLowerCode == iCharCode ) continue;

			if ( iCharCode > prev+200 ) 
			{
				arrayCount2++;
			}

			prev = iCharCode;

			int index = iCharCode - startValues2[ arrayCount2 ];
			mappings2[ arrayCount2 ][ index ] = iLowerCode;
		}
	}

	if ( szLine ) delete [] szLine;
	szLine = 0;

	arrayCount2++;

	agk::CloseFile( 1 );

	// write data to header file
	char str[ 256 ];
	// start values
	strcpy( str, "\n\n// Lower Case\nint g_iLowerCaseStartValues[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount2; arrayIndex++ )
	{
		sprintf( str, "%d", startValues2[ arrayIndex ] );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount2-1 ) fwrite( ", ", 1, 2, fp );
	}
	fwrite( " };\n\n", 1, 5, fp );

	// end values
	strcpy( str, "int g_iLowerCaseEndValues[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount2; arrayIndex++ )
	{
		sprintf( str, "%d", endValues2[ arrayIndex ] );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount2-1 ) fwrite( ", ", 1, 2, fp );
	}
	fwrite( " };\n\n", 1, 5, fp );

	// replacements
	for ( int arrayIndex = 0; arrayIndex < arrayCount2; arrayIndex++ )
	{
		sprintf( str, "int g_iLowerCaseReplacements%d[] = { ", arrayIndex );
		fwrite( str, 1, strlen(str), fp );

		int count = endValues2[arrayIndex] - startValues2[arrayIndex] + 1;
		for ( int i = 0; i < count; i++ )
		{
			sprintf( str, "%d", mappings2[ arrayIndex ][ i ] );
			fwrite( str, 1, strlen(str), fp );
			if ( i < count-1 ) fwrite( ", ", 1, 2, fp );

			if ( i > 0 && i % 35 == 0 ) fwrite( " \\\n", 1, 3, fp );
		}
		fwrite( " };\n\n", 1, 5, fp );
	}
	
	strcpy( str, "int *g_iLowerCaseReplacements[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount2; arrayIndex++ )
	{
		sprintf( str, "g_iLowerCaseReplacements%d", arrayIndex );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount2-1 ) fwrite( ", ", 1, 2, fp );

		if ( arrayIndex > 0 && arrayIndex % 4 == 0 ) fwrite( " \\\n", 1, 3, fp );
	}
	fwrite( " };\n", 1, 3, fp );
}

struct specialEntry
{
	unsigned int charCode;
	unsigned int replacement;
};

int specialCompare( const void* a, const void* b )
{
	specialEntry *c = (specialEntry*)a;
	specialEntry *d = (specialEntry*)b;

	if ( c->charCode > d->charCode ) return 1;
	else if ( c->charCode < d->charCode ) return -1;
	else return 0;
}

void BuildUppercaseData( FILE *fp )
{
	specialEntry *specialCasings = 0;
	int specialCasingsCount = 0;

	char *szLine = 0;
	char *szPtr = 0;
	char hex[ 10 ];

	agk::OpenToRead( 1, "SpecialCasing.txt" );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			int fieldCount = 0;
			szPtr = field+1;
			while( fieldCount < 2 && (field = strchr(szPtr,';')) )
			{
				fieldCount++;
				szPtr = field+1;
				while( *szPtr == ' ' ) szPtr++;
			}

			if ( fieldCount != 2 )
			{
				agk::Message( "Could not find special upper case field" );
				return;
			}

			field = strchr(szPtr,';');
			if ( field ) *field = 0;
			if ( field == szPtr || *szPtr == 0 ) continue;
			if ( strchr(field+1,';') != 0 ) continue; // skip conditional replacements

			specialCasingsCount++;
		}
	}

	specialCasings = new specialEntry[ specialCasingsCount ];

	// reset
	if ( szLine ) delete [] szLine;
	szLine = 0;

	specialCasingsCount = 0;

	agk::SetFilePos( 1, 0 );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			int fieldCount = 0;
			szPtr = field+1;
			while( fieldCount < 2 && (field = strchr(szPtr,';')) )
			{
				fieldCount++;
				szPtr = field+1;
			}

			if ( fieldCount != 2 )
			{
				agk::Message( "Could not find special upper case field" );
				return;
			}

			field = strchr(szPtr,';');
			if ( field ) *field = 0;
			if ( field == szPtr || *szPtr == 0 ) continue;
			if ( strchr(field+1,';') != 0 ) continue; // skip conditional replacements
			if ( *szPtr == ' ' ) szPtr++;

			unsigned int replaceChars[ 3 ] = {0,0,0};
			unsigned int replaceCount = 0;
			char *space = szPtr;
			while ( space = strchr(szPtr, ' ') ) 
			{
				*space = 0;
				strcpy( hex, szPtr );
				replaceChars[ replaceCount < 4 ? replaceCount : 3 ] = (unsigned int)strtol( hex, 0, 16 );
				replaceCount++;
				szPtr = space+1;
			}

			strcpy( hex, szPtr );
			replaceChars[ replaceCount < 4 ? replaceCount : 3 ] = (unsigned int)strtol( hex, 0, 16 );
			replaceCount++;

			if ( replaceCount > 3 )
			{
				agk::Message( "Upper case replacement count is great than 3, changes are needed to handle this case" );
				return;
			}

			specialCasings[ specialCasingsCount ].charCode = iCharCode;

			specialCasings[ specialCasingsCount ].replacement = (replaceCount-1) << 30;
			switch( replaceCount )
			{
				case 1: 
				{
					specialCasings[ specialCasingsCount ].replacement |= replaceChars[ 0 ];
					break;
				}
				case 2: 
				{
					if ( replaceChars[ 0 ] > 0x7FFF || replaceChars[ 1 ] > 0x7FFF )
					{
						agk::Message( "2 upper case replace chars has a value greater than 0x7FFF, can't handle this case" );
						return;
					}
					specialCasings[ specialCasingsCount ].replacement |= replaceChars[ 0 ];
					specialCasings[ specialCasingsCount ].replacement |= (replaceChars[ 1 ] << 15);
					break;
				}
				case 3: 
				{
					if ( replaceChars[ 0 ] > 0x3FF || replaceChars[ 1 ] > 0x3FF || replaceChars[ 2 ] > 0x3FF )
					{
						agk::Message( "3 upper case replace chars has a value greater than 0x3FF, can't handle this case" );
						return;
					}
					specialCasings[ specialCasingsCount ].replacement |= replaceChars[ 0 ];
					specialCasings[ specialCasingsCount ].replacement |= (replaceChars[ 1 ] << 10);
					specialCasings[ specialCasingsCount ].replacement |= (replaceChars[ 2 ] << 20);
					break;
				}
			}

			specialCasingsCount++;
		}
	}

	if ( szLine ) delete [] szLine;
	szLine = 0;

	qsort( specialCasings, specialCasingsCount, sizeof(specialEntry), specialCompare );

	agk::CloseFile( 1 );

	// read main file
	unsigned int *startValues3 = 0;
	unsigned int *endValues3 = 0;
	unsigned int **mappings3 = 0;
	int arrayCount3 = 0;
	int specialIndex = 0;

	int prev = -1000;
	agk::OpenToRead( 1, "UnicodeData.txt" );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			int skip = 0;
			while ( iCharCode >= (int)specialCasings[ specialIndex ].charCode && specialIndex < specialCasingsCount )
			{
				if ( (int)specialCasings[ specialIndex ].charCode > prev+200 ) 
				{
					arrayCount3++;
				}

				prev = specialCasings[ specialIndex ].charCode;
				specialIndex++;
				if ( iCharCode == specialCasings[ specialIndex-1 ].charCode ) skip = 1;
			}
			if ( skip ) continue;

			int fieldCount = 0;
			szPtr = field+1;
			while( fieldCount < 11 && (field = strchr(szPtr,';')) )
			{
				fieldCount++;
				szPtr = field+1;
			}

			if ( fieldCount != 11 )
			{
				agk::Message( "Could not find upper case field" );
				return;
			}

			field = strchr(szPtr,';');
			if ( field ) *field = 0;
			if ( field == szPtr || *szPtr == 0 ) continue;
			
			strcpy( hex, szPtr );
			int iLowerCode = strtol( hex, 0, 16 );
			if ( iLowerCode == iCharCode ) continue;
			
			if ( iCharCode > prev+200 ) 
			{
				arrayCount3++;
			}

			prev = iCharCode;
		}
	}

	if ( szLine ) delete [] szLine;
	szLine = 0;
	
	startValues3 = new unsigned int[ arrayCount3 ];
	endValues3 = new unsigned int[ arrayCount3 ];
	mappings3 = new unsigned int*[ arrayCount3 ];

	// reset
	arrayCount3 = 0;
	prev = -1000;
	specialIndex = 0;

	agk::SetFilePos( 1, 0 );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			int skip = 0;
			while ( iCharCode >= (int)specialCasings[ specialIndex ].charCode && specialIndex < specialCasingsCount )
			{
				if ( (int)specialCasings[ specialIndex ].charCode > prev+200 ) 
				{
					startValues3[ arrayCount3 ] = specialCasings[ specialIndex ].charCode;
					if ( arrayCount3 > 0 )
					{
						endValues3[ arrayCount3-1 ] = prev;
						int count = prev - startValues3[ arrayCount3-1 ] + 1;
						mappings3[ arrayCount3-1 ] = new unsigned int[ count ];
						for ( int i = 0; i < count; i++ ) mappings3[ arrayCount3-1 ][ i ] = 0;
					}
					arrayCount3++;
				}

				prev = specialCasings[ specialIndex ].charCode;
				specialIndex++;
				if ( iCharCode == specialCasings[ specialIndex-1 ].charCode ) skip = 1;
			}
			if ( skip ) continue;

			int fieldCount = 0;
			szPtr = field+1;
			while( fieldCount < 11 && (field = strchr(szPtr,';')) )
			{
				fieldCount++;
				szPtr = field+1;
			}

			if ( fieldCount != 11 )
			{
				agk::Message( "Could not find upper case field" );
				return;
			}

			field = strchr(szPtr,';');
			if ( field ) *field = 0;
			if ( field == szPtr || *szPtr == 0 ) continue;

			strcpy( hex, szPtr );
			int iUpperCode = strtol( hex, 0, 16 );
			if ( iUpperCode == iCharCode ) continue;
						
			if ( iCharCode > prev+200 ) 
			{
				startValues3[ arrayCount3 ] = iCharCode;
				if ( arrayCount3 > 0 )
				{
					endValues3[ arrayCount3-1 ] = prev;
					int count = prev - startValues3[ arrayCount3-1 ] + 1;
					mappings3[ arrayCount3-1 ] = new unsigned int[ count ];
					for ( int i = 0; i < count; i++ ) mappings3[ arrayCount3-1 ][ i ] = 0;
				}
				arrayCount3++;
			}

			prev = iCharCode;
		}
	}

	if ( arrayCount3 > 0 )
	{
		endValues3[ arrayCount3-1 ] = prev;
		int count = prev - startValues3[ arrayCount3-1 ] + 1;
		mappings3[ arrayCount3-1 ] = new unsigned int[ count ];
		for ( int i = 0; i < count; i++ ) mappings3[ arrayCount3-1 ][ i ] = 0;
	}

	if ( szLine ) delete [] szLine;
	szLine = agk::ReadLine( 1 );

	// reset
	prev = startValues3[0];
	arrayCount3 = 0;
	specialIndex = 0;

	// add character mappings to the mappings array
	agk::SetFilePos( 1, 0 );
	while ( !agk::FileEOF(1) )
	{
		if ( szLine ) delete [] szLine;
		szLine = agk::ReadLine( 1 );
		if ( szLine && *szLine && *szLine != '#' ) 
		{
			char* field = strchr( szLine, ';' );
			if ( !field ) continue;
			*field = 0;
			
			strcpy( hex, szLine );
			int iCharCode = strtol( hex, 0, 16 );

			int skip = 0;
			while ( iCharCode >= (int)specialCasings[ specialIndex ].charCode && specialIndex < specialCasingsCount )
			{
				if ( (int)specialCasings[ specialIndex ].charCode > prev+200 ) 
				{
					arrayCount3++;
				}

				prev = specialCasings[ specialIndex ].charCode;

				int index = specialCasings[ specialIndex ].charCode - startValues3[ arrayCount3 ];
				mappings3[ arrayCount3 ][ index ] = specialCasings[ specialIndex ].replacement;

				specialIndex++;
				if ( iCharCode == specialCasings[ specialIndex-1 ].charCode ) skip = 1;
			}
			if ( skip ) continue;

			int fieldCount = 0;
			szPtr = field+1;
			while( fieldCount < 11 && (field = strchr(szPtr,';')) )
			{
				fieldCount++;
				szPtr = field+1;
			}

			if ( fieldCount != 11 )
			{
				agk::Message( "Could not find upper case field" );
				return;
			}

			field = strchr(szPtr,';');
			if ( field ) *field = 0;
			if ( field == szPtr || *szPtr == 0 ) continue;

			strcpy( hex, szPtr );
			int iUpperCode = strtol( hex, 0, 16 );
			if ( iUpperCode == iCharCode ) continue;

			if ( iCharCode > prev+200 ) 
			{
				arrayCount3++;
			}

			prev = iCharCode;

			int index = iCharCode - startValues3[ arrayCount3 ];
			mappings3[ arrayCount3 ][ index ] = iUpperCode;
		}
	}

	if ( szLine ) delete [] szLine;
	szLine = 0;

	arrayCount3++;

	agk::CloseFile( 1 );

	// write data to header file
	char str[ 256 ];
	// start values
	strcpy( str, "\n\n// Upper Case\nint g_iUpperCaseStartValues[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount3; arrayIndex++ )
	{
		sprintf( str, "%d", startValues3[ arrayIndex ] );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount3-1 ) fwrite( ", ", 1, 2, fp );
	}
	fwrite( " };\n\n", 1, 5, fp );

	// end values
	strcpy( str, "int g_iUpperCaseEndValues[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount3; arrayIndex++ )
	{
		sprintf( str, "%d", endValues3[ arrayIndex ] );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount3-1 ) fwrite( ", ", 1, 2, fp );
	}
	fwrite( " };\n\n", 1, 5, fp );

	// replacements
	for ( int arrayIndex = 0; arrayIndex < arrayCount3; arrayIndex++ )
	{
		sprintf( str, "int g_iUpperCaseReplacements%d[] = { ", arrayIndex );
		fwrite( str, 1, strlen(str), fp );

		int count = endValues3[arrayIndex] - startValues3[arrayIndex] + 1;
		for ( int i = 0; i < count; i++ )
		{
			sprintf( str, "%d", mappings3[ arrayIndex ][ i ] );
			fwrite( str, 1, strlen(str), fp );
			if ( i < count-1 ) fwrite( ", ", 1, 2, fp );

			if ( i > 0 && i % 35 == 0 ) fwrite( " \\\n", 1, 3, fp );
		}
		fwrite( " };\n\n", 1, 5, fp );
	}
	
	strcpy( str, "int *g_iUpperCaseReplacements[] = { " );
	fwrite( str, 1, strlen(str), fp );
	for ( int arrayIndex = 0; arrayIndex < arrayCount3; arrayIndex++ )
	{
		sprintf( str, "g_iUpperCaseReplacements%d", arrayIndex );
		fwrite( str, 1, strlen(str), fp );
		if ( arrayIndex < arrayCount3-1 ) fwrite( ", ", 1, 2, fp );

		if ( arrayIndex > 0 && arrayIndex % 4 == 0 ) fwrite( " \\\n", 1, 3, fp );
	}
	fwrite( " };\n", 1, 3, fp );
}

void app::Begin(void)
{
	agk::SetVirtualResolution (1024, 768);
	agk::SetClearColor( 151,170,204 ); // light blue
	agk::SetSyncRate(60,0);
	agk::SetScissor(0,0,0,0);

	FILE *fp = fopen( "../../../common/include/UnicodeMappings.h", "wb" );
	if ( !fp ) 
	{
		agk::Message( "Failed to open UnicodeMappings.h" );
		return;
	}

	fputs( "#ifdef AGK_COMPILER\nnamespace AGK_Compiler {\n#else\nnamespace AGK {\n#endif", fp );
	
	BuildCaseFoldingData(fp);
	BuildLowercaseData(fp);
	BuildUppercaseData(fp);

	fputs( "}", fp );

	fclose(fp);
}

void app::Loop (void)
{
	/*
	for ( int i = 0; i < arrayCount3; i++ )
	{
		//int count = endValues2[i] - startValues2[i] + 1;
		char str[100];
		sprintf( str, "%04X = %04X", specialCasings[i].charCode, specialCasings[i].replacement );
		agk::Print( str );
	}
	*/

	agk::Print( agk::ScreenFPS() );
	agk::Sync();
}


void app::End (void)
{
	
}
