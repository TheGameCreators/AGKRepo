#ifndef _H_AGKCOMPILER
#define _H_AGKCOMPILER

#include "CompilerCommon.h"
#include "DynArray.h"

namespace AGK_Compiler
{
	int GetCompilerErrorCount();
	void SetCompilerMaxErrors( int maxErrors );
	void ResetCompilerErrors();
	char* GetCompileInfo();
	char* GetCompileErrors();
	void CompilerDeleteString( char* str );
	int RunCompiler( const char* lpCmdLine );
	int RunCompilerString( const char* szSource );
}

using namespace AGK_Compiler;

#endif


