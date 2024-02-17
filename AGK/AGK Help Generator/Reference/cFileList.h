
#ifndef _CFILELIST_H_
#define _CFILELIST_H_

#include <windows.h>
#include <stdio.h>
#include <vector>

class cFileList
{
	private:
		std::vector < char* > m_List;
		std::vector < char* > m_MatchingList;
		
	public:
		 cFileList ( );
		~cFileList ( );

		bool Search ( char* szFilter );

		DWORD GetCount			( void );
		char* GetName			( int iIndex );
		char* GetMatchingFile   ( int iIndex );
		DWORD FindAllFiles		( void );
		DWORD FindMatchingFiles ( char* szMatch );
};

#endif _CFILELIST_H_