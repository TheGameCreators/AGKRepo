#ifndef _H_PARSER1
#define _H_PARSER1

#include "ProgramData.h"

namespace AGK_Compiler
{

	struct stKeyword
	{
		const char* szKeyword;
		AGKToken iToken;
	};

	const char* GetTokenString( AGKToken token );

	enum IncludeMode
	{
		INCLUDE_IGNORE = 0,
		INCLUDE_FORBIDDEN,
		INCLUDE_NORMAL
	};

	class Parser1
	{
		protected:
			int iCommentBlock;
			int iFunctionBlock;
			int iSelectBlock;
			int iTypeBlock;
			int iFunctionLine;
			int iTypeLine;
			IncludeMode includeMode;

			static int g_iSorted;
			static int g_iNumKeywords;
			static stKeyword g_pKeywords[];

		public:
			static void Parser1Init();

			static int GetNumKeywords() { return g_iNumKeywords; }
			static const stKeyword* GetKeywords() { return g_pKeywords; }
			
			Parser1();
			~Parser1();

			void Reset();
			void SetIncludeMode( IncludeMode mode );
			AGKToken ParseKeyword( const char* str );
			AGKToken ParseTokenCommentBlock( AGKToken prevToken, const char* str, int &consumed, uString &error );
			AGKToken ParseToken( AGKToken prevprevToken, AGKToken prevToken, const char* str, int &consumed, uString &error );
			int ParseLine( int includeFileID, const uString &sLine, const uString &sLineRaw, int iLineNum, uString &error, cProgramData *pProgram=0 );
			int ParseFile( const char* filename, int includeFileID, cProgramData *pProgram=0 );
			int ParseSource( const char* source, cProgramData *pProgram=0 );
			int HasUnclosedBlocks();
	};
}

#endif