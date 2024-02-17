#ifndef _H_AGK_COMPILER
#define _H_AGK_COMPILER

namespace AGK_Compiler
{
	int RunCompiler( const char* lpCmdLine );
	int RunCompilerString( const char* source );
	void CompilerDeleteString( char* str );
	char* GetCompileErrors();
}

#endif
