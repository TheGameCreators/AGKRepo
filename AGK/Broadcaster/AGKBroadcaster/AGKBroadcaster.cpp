
#include "AGKBroadcaster.h"
#include "Listener.h"

namespace AGKBroadcaster
{

extern int interactive;

AGKListener g_Listener;
int project_set = 0;

bool last_debug_msg_trigger[MAXBACKLOG] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
											false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
char last_debug_msg[MAXBACKLOG][MAXVALUESIZE+100];
int last_entry = 0;

int ResetDebugMessageLog(void)
{
	int count = 0;
	for (int i = 0; i < MAXBACKLOG; i++) {
		if (last_debug_msg_trigger[i]) {
			last_debug_msg_trigger[i] = false;
			count++;
		}
	}
	return count;
}

int GetDebugMessageCount(void)
{
	int count = 0;
	for (int i = 0; i < MAXBACKLOG; i++) {
		if (last_debug_msg_trigger[i]) {
			count++;
		}
	}
	return count;
}

const char * GetLastDebugMessage(void)
{
	for (int i = 0; i < MAXBACKLOG; i++) {
		if (last_debug_msg_trigger[i]) {
			last_debug_msg_trigger[i] = false;
			return &last_debug_msg[i][0];
		}
	}
	return NULL;
}

void AddBackLog(const char *blog)
{
	last_entry++;
	if (last_entry >= MAXBACKLOG)
		last_entry = 0;

	for (int i = last_entry; i < MAXBACKLOG; i++)
	{
		if (!last_debug_msg_trigger[i]) {
			//strcpy(last_debug_msg[i], blog);
			strncpy(last_debug_msg[i], blog, MAXVALUESIZE -8);
			last_debug_msg[i][MAXVALUESIZE - 8] = 0;
			strcat(last_debug_msg[i], "\n");
			last_debug_msg_trigger[i] = true;
			return;
		}
	}
	for (int i = 0; i < MAXBACKLOG; i++)
	{
		if (!last_debug_msg_trigger[i]) {
			strncpy(last_debug_msg[i], blog, MAXVALUESIZE - 8);
			last_debug_msg[i][MAXVALUESIZE - 8] = 0;
			strcat(last_debug_msg[i], "\n");
			last_debug_msg_trigger[i] = true;
			return;
		}
	}
	return;
}

void WriteError( const char *msg )
{
	if ( interactive )
	{
		uString sMsg( msg );
		sMsg.Prepend( "Error: " );
		WriteStdOutput( sMsg.GetStr() );
	}
	else
	{
		uString sMsg( msg );
		sMsg.Prepend( "Error:" );
		WriteStdOutput( sMsg.GetStr() );
	}
}

void WriteWarning( const char *msg )
{
	if ( interactive )
	{
		uString sMsg( msg );
		sMsg.Prepend( "Warning: " );
		WriteStdOutput( sMsg.GetStr() );
	}
	else
	{
		uString sMsg( msg );
		sMsg.Prepend( "Warning:" );
		WriteStdOutput( sMsg.GetStr() );
	}
}

void WriteInfo( const char *msg )
{
	if ( interactive )
	{
		WriteStdOutput( msg );
	}
	else
	{
		uString sMsg( msg );
		sMsg.Prepend( "Log:" );
		WriteStdOutput( sMsg.GetStr() );
	}
}

void WriteDebug( const char *type, const char *data )
{
	//MessageBox( NULL, "Don't know what to do with debug messages yet", "Error", 0 );
	if ( strcmp( type, "Log" ) == 0 )
	{
		WriteInfo( data );
	}
	else if ( strcmp( type, "Variable" ) == 0 )
	{
		const char *colon = strchr( data, ':' );
		if ( colon && colon != data && *(colon+1) ) // colon exists and it is not at the beginning or end of the string
		{
			UINT length = (UINT)(colon-data);
			uString varName;
			varName.SetStrN( data, length );
			WriteVariable( varName, data+length+1 );
		}
	}
	else if ( strcmp( type, "Frame" ) == 0 )
	{
		if ( interactive )
		{
			uString sFrame( "Frame " );
			sFrame.Append( data );
			WriteStdOutput( sFrame.GetStr() );
		}
		else
		{
			uString sFrame( "Frame:" );
			sFrame.Append( data );
			WriteStdOutput( sFrame.GetStr() );
		}
	}
}

void WriteVariable( const char *varname, const char *value )
{
	if ( interactive )
	{
		uString sMsg( varname );
		sMsg.Append( " = " );
		sMsg.Append( value );
		WriteStdOutput( sMsg.GetStr() );
	}
	else
	{
		uString sMsg( "Variable:" );
		sMsg.Append( varname );
		sMsg.Append( ":" );
		sMsg.Append( value );
		WriteStdOutput( sMsg.GetStr() );
	}
}

void WriteBreak( const char *szInclude, int line )
{
	if ( interactive )
	{
		uString sMsg;
		sMsg.Format( "Breakpoint hit in %s on line %d", szInclude, line );
		WriteStdOutput( sMsg.GetStr() );
	}
	else
	{
		uString sMsg;
		sMsg.Format( "Break:%s:%d", szInclude, line );
		WriteStdOutput( sMsg.GetStr() );
	}
}


// ***********
// Main
// ***********

int BroadcasterCommand( const char* cmd, uString& sResponse )
{
	sResponse.SetStr( "" );

	if ( strcmp(cmd,"list") == 0 )
	{
		uString devices;
		g_Listener.GetDeviceList( devices );
		sResponse.SetStr( devices );
	}
	else if ( strncmp(cmd,"setproject",strlen("setproject")) == 0 )
	{
		const char* path = cmd + strlen("setproject");
		if ( *path != ' ' )
		{
			sResponse.SetStr( "setproject must be followed by a space and a project path" );
			return 1;
		}

		path++;	
		uString sPath( path );
		sPath.Trim( " " );
		if ( sPath.GetLength() == 0 )
		{
			sResponse.SetStr( "setproject must be followed by a space and a project path" );
			return 1;
		}

		sPath.Replace( '\\', '/' );
		if ( sPath.ByteAt( sPath.GetLength()-1 ) != '/' ) sPath.AppendAscii( '/' );

		//agk::SetRawWritePath( sPath );
		agk::OverrideDirectories( sPath, 0 );
		agk::SetFolder( "" );
		if ( agk::GetFileExists( "main.agc" ) == 0 )
		{
			sResponse.SetStr( "Not a valid AGK project path" );
			return 1;
		}
			
		if ( agk::GetFileExists( "media/bytecode.byc" ) == 0 )
		{
			sResponse.SetStr( "Project has not yet been compiled" );
			return 1;
		}

		agk::SetFolder( "" );
		//SetCurrentDirectory( sPath.GetStr() );

		if ( sPath.CharAt(sPath.GetLength()-1) == '/' ) sPath.Trunc2(1);
		const char *szSlashF = strrchr( sPath.GetStr(), '/' );
		if ( szSlashF )
		{
			g_Listener.SetAppName( szSlashF+1 );
		}
		else g_Listener.SetAppName( "Unknown" );

		project_set = 1;
		g_Listener.ReadProjectFiles();
	}
	else if ( strcmp(cmd,"connectall") == 0 )
	{
		if ( !project_set )
		{
			sResponse.SetStr( "Cannot connect until a project path is set, use \"setproject <path>\" first" );
			return 1;
		}

		g_Listener.SetConnectAll( 1 );
		if ( !g_Listener.IsRunning() ) g_Listener.Start();
	}
	else if ( strncmp(cmd,"connect",strlen("connect")) == 0 )
	{
		const char* szIP = cmd + strlen("connect");
		if ( *szIP != ' ' )
		{
			sResponse.SetStr( "connect must be followed by a space and an IP address" );
			return 1;
		}

		szIP++;	
		uString sIP( szIP );
		sIP.Trim( " " );
		if ( sIP.GetLength() == 0 )
		{
			sResponse.SetStr( "connect must be followed by a space and an IP address" );
			return 1;
		}

		if ( !project_set )
		{
			sResponse.SetStr( "Cannot connect until a project path is set, use \"setproject <path>\" first" );
			return 1;
		}

		// loopback address doesn't appear to work, discover local IP instead
		// correction: loopback works if the receiver is bound to INADDR_ANY
		/*
		if ( sIP.CompareTo( "127.0.0.1" ) == 0 )
		{
			char* temp = agk::GetDeviceIP();
			sIP.SetStr( temp );
			delete [] temp;
		}
		*/

		if ( strchr(szIP, ':') != 0 ) g_Listener.AddDevice( sIP, 5692 ); // IPv6
		else g_Listener.AddDevice( sIP, 5690 ); // IPv4
		if ( !g_Listener.IsRunning() ) g_Listener.Start();
	}
	else if ( strcmp(cmd,"disconnectall") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.Stop();
		g_Listener.Join();
		g_Listener.Disconnect();
	}
	else if ( strncmp(cmd,"breakpoint",strlen("breakpoint")) == 0 )
	{
		const char* szBreakpoint = cmd + strlen("breakpoint");
		if ( *szBreakpoint != ' ' )
		{
			sResponse.SetStr( "breakpoint must be followed by a space then a filename and a line number like so, breakpoint myFile.agc:102" );
			return 1;
		}

		szBreakpoint++;
		uString sBreakpoint( szBreakpoint );
		sBreakpoint.Trim( " " );
		if ( sBreakpoint.GetLength() == 0 || sBreakpoint.Find( ':' ) < 0 || sBreakpoint.CountTokens2( ':' ) != 2 )
		{
			sResponse.SetStr( "breakpoint must be followed by a space then a filename and a line number like so, breakpoint myFile.agc:102" );
			return 1;
		}

		uString *params = 0;
		sBreakpoint.SplitTokens2( ':', params );

		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.AddBreakpoint( params[0], params[1].ToInt() );
	}
	else if ( strncmp(cmd,"delete breakpoint",strlen("delete breakpoint")) == 0 )
	{
		const char* szBreakpoint = cmd + strlen("delete breakpoint");
		if ( *szBreakpoint != ' ' )
		{
			sResponse.SetStr( "delete breakpoint must be followed by a space then a filename and a line number like so, delete breakpoint myFile.agc:102" );
			return 1;
		}

		szBreakpoint++;
		uString sBreakpoint( szBreakpoint );
		sBreakpoint.Trim( " " );
		if ( sBreakpoint.GetLength() == 0 || sBreakpoint.Find( ':' ) < 0 || sBreakpoint.CountTokens2( ':' ) != 2 )
		{
			sResponse.SetStr( "delete breakpoint must be followed by a space then a filename and a line number like so, delete breakpoint myFile.agc:102" );
			return 1;
		}

		uString *params = 0;
		sBreakpoint.SplitTokens2( ':', params );

		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.RemoveBreakpoint( params[0], params[1].ToInt() );
	}
	else if ( strcmp(cmd,"delete all breakpoints") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.RemoveAllBreakpoints();
	}
	else if (strncmp(cmd, "setvariable", strlen("setvariable")) == 0)
	{
		const char* szVar = cmd + strlen("setvariable");
		if (*szVar != ' ')
		{
			sResponse.SetStr( "setvariable must be followed by a space then a variable expression like so, setvariable myArray[9] NewValue" );
		}

		szVar++;
		uString sTmp( szVar );
		sTmp.Trim(" ");

		uString sValue = "";
		uString sVar = "";

		int pos = sTmp.FindStr(" ");
		if ( pos > 0 ) 
		{
			sTmp.SubString( sVar, 0, pos );
			sTmp.SubString( sValue, pos+1 );
		}

		if ( sVar.GetLength() == 0 || sValue.GetLength() == 0 )
		{
			sResponse.SetStr("setvariable must be followed by a space then a variable expression like so, setvariable myArray[9] NewValue");
		}

		if ( !g_Listener.IsRunning() )
		{
			sResponse.SetStr("Broadcaster is not currently connected");
		}

		g_Listener.SetVariable( sVar, sValue );
	}
	else if ( strncmp(cmd,"watch",strlen("watch")) == 0 )
	{
		const char* szVar = cmd + strlen("watch");
		if ( *szVar != ' ' )
		{
			sResponse.SetStr( "watch must be followed by a space then a variable expression like so, watch myArray[9]" );
			return 1;
		}

		szVar++;
		uString sVar( szVar );
		sVar.Trim( " " );
		if ( sVar.GetLength() == 0 )
		{
			sResponse.SetStr( "watch must be followed by a space then a variable expression like so, watch myArray[9]" );
			return 1;
		}

		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.AddVariableWatch( sVar );
	}
	else if ( strncmp(cmd,"delete watch",strlen("delete watch")) == 0 )
	{
		const char* szVar = cmd + strlen("delete watch");
		if ( *szVar != ' ' )
		{
			sResponse.SetStr( "delete watch must be followed by a space then a variable expression like so, delete watch myArray[9]" );
			return 1;
		}

		szVar++;
		uString sVar( szVar );
		sVar.Trim( " " );
		if ( sVar.GetLength() == 0 )
		{
			sResponse.SetStr( "delete watch must be followed by a space then a variable expression like so, delete watch myArray[9]" );
			return 1;
		}

		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.RemoveVariableWatch( sVar );
	}
	else if ( strcmp(cmd,"delete all watches") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.RemoveAllWatchVariables();
	}
	else if ( strcmp(cmd,"run") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.RunApp();
	}
	else if ( strcmp(cmd,"debug") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.DebugApp();
	}
	else if ( strcmp(cmd,"stop") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.StopApp();
	}
	else if ( strcmp(cmd,"pause") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.PauseApp();
	}
	else if ( strcmp(cmd,"continue") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.ResumeApp();
	}
	else if ( strcmp(cmd,"step") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.Step();
	}
	else if ( strcmp(cmd,"stepover") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.StepOver();
	}
	else if ( strcmp(cmd,"stepout") == 0 )
	{
		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.StepOut();
	}
	else if ( strncmp(cmd,"set frame",strlen("set frame")) == 0 )
	{
		const char *szFrame = cmd + strlen("set frame");
		if ( *szFrame != ' ' )
		{
			sResponse.SetStr( "set frame must be followed by a frame number, e.g. set frame 3" );
			return 1;
		}

		szFrame++;
		uString sFrame( szFrame );
		sFrame.Trim( " " );
		if ( sFrame.GetLength() == 0 )
		{
			sResponse.SetStr( "set frame must be followed by a frame number, e.g. set frame 3" );
			return 1;
		}

		int iFrame = sFrame.ToInt();
		if ( iFrame == 0 && sFrame.CharAt(0) != '0' )
		{
			sResponse.SetStr( "set frame must be followed by a frame number, e.g. set frame 3" );
			return 1;
		}

		if ( !g_Listener.IsRunning() ) 
		{
			sResponse.SetStr( "Broadcaster is not currently connected" );
			return 1;
		}

		g_Listener.SetFrame( iFrame );
	}
	else if ( strcmp(cmd,"help") == 0 )
	{
		sResponse.SetStr( "Commands:\n\
help - show this message\n\
list - show connected devices\n\
exit - disconnect from all devices and close this program\n\
setproject <path> - set the path to the AGK project that should be broadcast\n\
connectall - looks for devices, connects to them, and uploads the project\n\
connect <ip> - connect directly to specified device and attempt to broadcast to it\n\
disconnectall - disconnect from all devices\n\
run - run the uploaded project on all connected devices\n\
debug - run the uploaded project on all connected devices in debug mode\n\
pause - pause the devices running the current project\n\
continue - continue any paused devices from a pause command or a breakpoint\n\
stop - stop the devices running the current project\n\
breakpoint <filename>:<linenum> - add a breakpoint at the given filename and line number\n\
delete breakpoint <filename>:<linenum> - remove a breakpoint from the given filename and line number\n\
delete all breakpoints - remove all breakpoints\n\
watch <variable expression> - watch a variable value\n\
delete watch <variable expression> - stop watching a variable\n\
delete all watches - stop watching all variables\n\
set frame <num> - sets the current call stack frame when paused to view other local variables" );
	}
	else if ( strcmp(cmd,"exit") == 0 )
	{
		return 0;
	}
	else
	{
		sResponse = "Unrecognised command: ";
		sResponse.Append( cmd );
	}

	return 1;
}

void BroadcasterSetup()
{
	g_Listener.SetPort( 5689, 5691 );
}

int RunBroadcaster( const char* cmdline )
{
	agk::InitConsole();

	BroadcasterSetup();
		
	char data[ 1024 ];

	do
	{
		WriteStdOutputPrompt();

		if ( !ReadStdInput( data, 1024 ) ) break;
		
		char* newline = strchr( data, '\r' );
		if ( !newline ) newline = strchr( data, '\n' );
		if ( newline ) *newline = 0;

		uString sResponse;
		if ( !BroadcasterCommand( data, sResponse ) ) break;
		if ( sResponse.GetLength() > 0 )
		{
			WriteStdOutput( sResponse );
		}
		
		//sprintf( output, "You entered: %s\n", data );
		//WriteStdOutput( output );
	} while ( 1 );

	//system("pause");
	return 0;
}

} // AGKBroadcaster namespace