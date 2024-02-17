#ifndef _H_AGK_BROADCASTER
#define _H_AGK_BROADCASTER

#include "agk.h"
#include <stdio.h>
#include <iostream>

#define MAXBACKLOG 40
#define MAXVALUESIZE 2048

#ifdef GetLastDebugMessage
	#undef GetLastDebugMessage
#endif

namespace AGKBroadcaster
{
	void BroadcasterSetup();
	int BroadcasterCommand( const char* cmd, uString& sResponse );
	int RunBroadcaster( const char* cmdline );
	int ReadStdInput( char* output, int maxlength );
	void WriteStdOutput( const char* output );
	const char * GetLastDebugMessage(void);
	int GetDebugMessageCount(void);
	int ResetDebugMessageLog(void);
	void AddBackLog(const char *blog);
	void WriteStdOutputPrompt();

	void WriteError( const char *msg );
	void WriteWarning( const char *msg );
	void WriteInfo( const char *msg );
	void WriteDebug( const char *type, const char *data );
	void WriteVariable( const char *varname, const char *value );
	void WriteBreak( const char *szInclude, int line );

} // AGKBroadcaster namespace 

using namespace AGK;

#endif // _H_AGK_BROADCASTER