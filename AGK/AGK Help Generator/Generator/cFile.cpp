
#include "cFile.h"

cFile::cFile ( char* szFile, char* szDelimiters )
{
	// the constructor will get things started and
	// extract the data from the specified file

	// set up all our member variables
	m_pFile		  = NULL;
	m_iTokenCount = 0;
	m_ppszTokens  = NULL;

	// open the file
	if ( !Open ( szFile ) )
		return;

	// determine how large the file is so we know
	// how many tokens we are dealing with
	if ( !GetSize ( szDelimiters ) ) 
		return;

	// extract all tokens from the file
	if ( !Extract ( szDelimiters ) )
		return;

	// now close the file
	if ( !Close ( ) )
		return;
}

cFile::~cFile ( )
{
	// reset everything on destruction
	Reset ( );
}

bool cFile::Reset ( void )
{
	// remove all tokens
	for ( int i = 0; i < m_iTokenCount; i++ )
	{
		// check the token is valid
		if ( m_ppszTokens [ i ] )
		{
			// delete the token and set it to null
			delete m_ppszTokens [ i ];
			m_ppszTokens [ i ] = NULL;
		}
	}

	// remove main token array
	delete [ ] m_ppszTokens;

	// reset all values
	m_pFile		  = NULL;
	m_iTokenCount = 0;
	m_ppszTokens  = NULL;

	return true;
}

bool cFile::Open ( char* szFile )
{
	// open the specified file in text mode
	if ( !( m_pFile = fopen ( szFile, "rt" ) ) )
		return false;

	return true;
}

bool cFile::GetSize ( char* szDelimiters )
{
	// find out how many tokens are in the file, this information
	// is used so we can allocate the memory needed

	// set up a temporary character array
	char szTemp [ 4096 ];

	// run through the file
	while ( fgets ( szTemp, sizeof ( szTemp ), m_pFile ) )
	{
		// get the token
		char* token = strtok ( szTemp, szDelimiters );

		// loop round getting tokens
		while ( token )
		{
			// increment token count
			m_iTokenCount++;

			// get the next token
			token = strtok ( 0, szDelimiters );
		}
	}

	// return to the start of the file
	fseek ( m_pFile, 0, SEEK_SET );

	// allocate memory to store the tokens and reset
	// the actual count to 0
	m_ppszTokens  = new char* [ m_iTokenCount ];
	m_iTokenCount = 0;

	return true;
}

bool cFile::Extract ( char* szDelimiters )
{
	// now we extract tokens from the file and place
	// into the newly allocated array

	// temporary string array
	fpos_t pos;
	fgetpos( m_pFile, &pos );
	fseek( m_pFile, 0, SEEK_END );
	UINT size = ftell( m_pFile );
	fsetpos( m_pFile, &pos );
	char *szTemp = new char[ size+1 ];

	// loop round
	while ( fgets ( szTemp, size+1, m_pFile ) )
	{
		// get token
		char* token = strtok ( szTemp, szDelimiters );

		// get rest
		while ( token )
		{
			// allocate token
			m_ppszTokens [ m_iTokenCount ] = new char [ 17000 ];

			// get token and increment count
			strcpy ( m_ppszTokens [ m_iTokenCount++ ], token );

			// get next token
			token = strtok ( 0, szDelimiters );
		}
	}

	delete [] szTemp;

	return true;
}

bool cFile::Close ( void )
{
	// close the file
	fclose ( m_pFile );

	return true;
}

int cFile::GetTokenCount ( void )
{
	// return the number of tokens
	return m_iTokenCount;
}

char* cFile::GetToken ( int iIndex )
{
	// get the token
	return m_ppszTokens [ iIndex ];
}