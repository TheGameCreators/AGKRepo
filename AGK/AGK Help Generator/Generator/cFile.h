
#ifndef _CFILE_H_
#define _CFILE_H_

#include <windows.h>
#include <stdio.h>

class cFile
{
	// this file class will read in a text file and extract
	// all of its tokens into an array

	private:

		// file pointer, number of tokens and array for tokens
		FILE*		m_pFile;
		int			m_iTokenCount;
		char**		m_ppszTokens;

		// functions to open, determine size, extract and close
		bool Open    ( char* szFile );
		bool GetSize ( char* szDelimiters );
		bool Extract ( char* szDelimiters );
		bool Close   ( void );
		bool Reset   ( void );

	public:

		// constructor and destructor
		 cFile ( char* szFile, char* szDelimiters );
		~cFile ( );

		// functions to get the number of tokens and
		// extract a specific token
		int   GetTokenCount ( void );
		char* GetToken      ( int iIndex );
};

#endif _CFILE_H_