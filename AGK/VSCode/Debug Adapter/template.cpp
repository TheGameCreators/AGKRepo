// Includes
#include "template.h"
#include "AGKBroadcaster.h"

using namespace AGKBroadcaster;

#define APP_TYPE "DebugAdapter"

// Namespace
using namespace AGK;

app App;

int prevWidth = -1;
int prevHeight = -1;
HANDLE hStdIn = 0;
HANDLE hStdOut = 0;

#define DEBUG_MODE_IDLE        0
#define DEBUG_MODE_DEBUG       1
#define DEBUG_MODE_BROADCAST   2

int debugMode = DEBUG_MODE_IDLE;

void app::Begin(void)
{
	if ( agk::GetRenderer() )
	{
		prevWidth = agk::GetDeviceWidth();
		prevHeight = agk::GetDeviceHeight();

		agk::SetVirtualResolution( prevWidth, prevHeight );
		agk::SetClearColor( 70,85,102 );
		agk::SetVSync(1);
		agk::SetScissor(0,0,0,0);
		agk::UseNewDefaultFonts( 1 );
		agk::SetPrintSize( 20 );
		agk::SetWindowTitle( APP_TYPE );
	}

	BroadcasterSetup();

	hStdIn = ::GetStdHandle( STD_INPUT_HANDLE );
	hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE );
}

#define BUFFER_LEN 1024
char input[BUFFER_LEN+1] = { 0 };
char* pData = 0;
uint32_t iDataLength = 0;
uint32_t iDataTotalLength = 0;

enum ReadState
{
	READ_HEADER = 0,
	READ_CONTENT
};

ReadState readState = READ_HEADER;
int contentLength = 0;
int currOffset = 0;

void SendResponse( int id, const char* command, const char* body )
{
	uString response;
	response.Format( "{ \"type\":\"response\", \"request_seq\":%d, \"success\":true, \"command\":\"%s\"", id, command );
	if ( body ) 
	{
		response.Append( ",\"body\":" );
		response.Append( body );
	}
	response.Append( "}" );
	uString responseFull;
	responseFull.Format( "Content-Length: %d\r\n\r\n", response.GetLength() );
	responseFull.Append( response );
	WriteFile( hStdOut, responseFull.GetStr(), responseFull.GetLength(), 0, 0 );
}

void SendEvent( const char* event, const char* body )
{
	uString response;
	response = "{ \"type\":\"event\", \"event\":\"";
	response.Append( event );
	response.Append( "\"" );
	if ( body && *body ) 
	{
		response.Append( ",\"body\":" );
		response.Append( body );
	}
	response.Append( "}" );
	uString responseFull;
	responseFull.Format( "Content-Length: %d\r\n\r\n", response.GetLength() );
	responseFull.Append( response );
	WriteFile( hStdOut, responseFull.GetStr(), responseFull.GetLength(), 0, 0 );
}

void ProcessJSON( const char* json )
{
	uString sBroadcastReply;

	JSONElement* root = JSONElement::LoadJSONFromData( json );

	if ( root->GetType() != AGK_JSON_OBJECT ) goto cleanup;
	JSONObject* rootObject = (JSONObject*) root;

	// get message type
	JSONElement* type = rootObject->GetElement( "type" );
	if ( !type || type->GetType() != AGK_JSON_STRING ) goto cleanup;
	JSONString* typeString = (JSONString*) type;
	if ( typeString->m_sValue.CompareCaseTo( "request" ) != 0 ) goto cleanup;

	// get sequence number
	JSONElement* seq = rootObject->GetElement( "seq" );
	if ( !seq || seq->GetType() != AGK_JSON_NUMBER ) goto cleanup;
	JSONNumber* seqNum = (JSONNumber*) seq;
	if ( seqNum->m_iIsInt == 0 ) goto cleanup;

	JSONElement* command = rootObject->GetElement( "command" );
	if ( !command || command->GetType() != AGK_JSON_STRING ) goto cleanup;
	JSONString* commandString = (JSONString*) command;
	if ( commandString->m_sValue.CompareCaseTo( "initialize" ) == 0 )
	{
		SendResponse( seqNum->m_iValue, commandString->m_sValue, "{}" );
		//SendEvent( "initialized", 0 );
	}
	else if ( commandString->m_sValue.CompareCaseTo( "launch" ) == 0 )
	{
		// get arguments
		JSONElement* args = rootObject->GetElement( "arguments" );
		if ( !args || args->GetType() != AGK_JSON_OBJECT ) goto cleanup;
		JSONObject* argsObject = (JSONObject*) args;

		// ip address
		JSONElement* ip = argsObject->GetElement( "targetIP" );
		if ( !ip || ip->GetType() != AGK_JSON_STRING ) goto cleanup;
		JSONString* ipString = (JSONString*) ip;
		
		// get project path
		JSONElement* project = argsObject->GetElement( "project" );
		if ( !project || project->GetType() != AGK_JSON_STRING ) goto cleanup;
		JSONString* projectString = (JSONString*) project;
		const char* szCurrDir = projectString->m_sValue.GetStr();

		// get mode
		debugMode = DEBUG_MODE_DEBUG;
		JSONElement* mode = argsObject->GetElement( "mode" );
		if ( mode && mode->GetType() == AGK_JSON_STRING ) 
		{
			JSONString* modeString = (JSONString*) mode;
			if ( modeString->m_sValue.CompareTo( "broadcast" ) == 0 ) debugMode = DEBUG_MODE_BROADCAST;
		}

		// stop any old broadcast
		BroadcasterCommand( "stop", sBroadcastReply );
		BroadcasterCommand( "disconnectall", sBroadcastReply );

		int localDebug = 0;
		if ( ipString->m_sValue.GetLength() == 0 ) localDebug = 1;

		// if debugging locally then also need to start an interpreter
		if ( debugMode == DEBUG_MODE_DEBUG && localDebug )
		{
			PROCESS_INFORMATION piProcInfo2;
			STARTUPINFOA siStartInfo2;
			ZeroMemory(&piProcInfo2, sizeof(PROCESS_INFORMATION));
			ZeroMemory(&siStartInfo2, sizeof(STARTUPINFOA));
			siStartInfo2.cb = sizeof(STARTUPINFOA);
			char szInterpreterPath[ 1024 ];
			strcpy( szInterpreterPath, "..\\media\\interpreters\\Windows64.exe" );
			BOOL result = CreateProcessA( NULL, szInterpreterPath, NULL, NULL, TRUE, 0, NULL, 0, &siStartInfo2, &piProcInfo2 );
			CloseHandle( piProcInfo2.hProcess );
			CloseHandle( piProcInfo2.hThread );
		}

		// start new broadcast
		uString sCmd;
		sCmd = "setproject ";
		sCmd.Append( szCurrDir );
		BroadcasterCommand( sCmd, sBroadcastReply );

		if ( ipString->m_sValue.GetLength() > 0 )
		{
			sCmd = "connect ";
			sCmd.Append( ipString->m_sValue );
			BroadcasterCommand( sCmd, sBroadcastReply );
		}
		else if ( debugMode == DEBUG_MODE_DEBUG && localDebug )
		{
			BroadcasterCommand( "connect 127.0.0.1", sBroadcastReply );
		}
		
		// only connect to all if not debugging
		if ( debugMode == DEBUG_MODE_BROADCAST ) BroadcasterCommand( "connectall", sBroadcastReply );
		BroadcasterCommand( "run", sBroadcastReply );
		
		SendResponse( seqNum->m_iValue, commandString->m_sValue, 0 );
		//SendEvent( "process", "{\"name\":\"AGK Broadcaster\"}" );
	}
	else if ( commandString->m_sValue.CompareCaseTo( "disconnect" ) == 0 )
	{
		// get arguments
		JSONElement* args = rootObject->GetElement( "arguments" );
		if ( !args || args->GetType() != AGK_JSON_OBJECT ) goto cleanup;
		JSONObject* argsObject = (JSONObject*) args;

		// restart
		JSONElement* restart = argsObject->GetElement( "restart" );
		if ( !restart || restart->GetType() != AGK_JSON_BOOL ) goto cleanup;
		JSONBool* restartBool = (JSONBool*) restart;

		BroadcasterCommand( "stop", sBroadcastReply );
		BroadcasterCommand( "disconnectall", sBroadcastReply );

		SendResponse( seqNum->m_iValue, commandString->m_sValue, 0 );
	}
	else
	{
		// do nothing for now
		uString sResponse = "Unrecognised command";
	}

cleanup:
	delete root;
}

int app::Loop (void)
{
	if ( agk::GetRenderer() )
	{
		int width = agk::GetDeviceWidth();
		int height = agk::GetDeviceHeight();

		if ( width != prevWidth || height != prevHeight )
		{
			prevWidth = agk::GetDeviceWidth();
			prevHeight = agk::GetDeviceHeight();
			agk::SetVirtualResolution( prevWidth, prevHeight );
		}
	}

	unsigned long avail = 0;
	PeekNamedPipe( hStdIn, NULL, 0, NULL, &avail, NULL );
	while( avail > 0 )
	{
		uint32_t bytes = avail;
		DWORD bytesRead = 0;
		if ( bytes > BUFFER_LEN ) bytes = BUFFER_LEN;
		int tryCount = 0;
		while( tryCount < 10 && !ReadFile( hStdIn, input, bytes, &bytesRead, NULL ) ) 
		{
			agk::Sleep( 5 );
			tryCount++;
		}
		if ( tryCount == 10 ) exit(0);
			
		if ( !pData || iDataTotalLength < iDataLength + bytesRead )
		{
			uint32_t newLength = iDataLength + bytesRead;
			char* pNewData = new char[ newLength ];
			if ( pData ) 
			{
				memcpy( pNewData, pData, iDataLength );
				delete [] pData;
			}
			pData = pNewData;
			iDataTotalLength = newLength;
		}

		memcpy( pData+iDataLength, input, bytesRead );
		iDataLength += bytesRead;

		avail -= bytesRead;
	}

	switch( readState )
	{
		case READ_HEADER:
		{
			if ( ((int)iDataLength) - currOffset > 0 )
			{
				const char* lineEnd = strchr( pData + currOffset, '\r' );
				if ( lineEnd != 0 )
				{
					int newOffset = (int) (lineEnd - pData);
					int length = newOffset - currOffset;
					if ( length == 0 )
					{
						readState = READ_CONTENT;
					}
					else
					{
						uString header;
						header.SetStrN( pData + currOffset, length );
						const char* szContentLength = "Content-Length:";
						if ( header.CompareToN( szContentLength, (int)strlen(szContentLength) ) == 0 )
						{
							uString sLength;
							header.SubString( sLength, (int)strlen(szContentLength) ); 
							sLength.Trim( " \t\r\n" );
							contentLength = sLength.ToInt();
						}
					}
					currOffset = newOffset + 2;
				}
			}
		} break;

		case READ_CONTENT:
		{
			int length = ((int)iDataLength) - currOffset;
			if ( length >= contentLength )
			{
				uString sJSON;
				sJSON.SetStrN( pData + currOffset, contentLength );

				ProcessJSON( sJSON.GetStr() );
				
				if ( length == contentLength )
				{
					iDataLength = 0;
					// shrink it if it is using a lot of memory
					if ( pData && iDataTotalLength > 2000000 )
					{
						iDataTotalLength = 2000000;
						delete [] pData;
						pData = new char[ iDataTotalLength ];
					}
				}
				else
				{
					uint32_t remaining = iDataLength - currOffset - contentLength; // above checks ensure this will greater than 0
					for( uint32_t i = 0; i < remaining; i++ )
					{
						pData[ i ] = pData[ i + currOffset + contentLength ];
					}
					iDataLength = remaining;
				}

				contentLength = 0;
				currOffset = 0;
				readState = READ_HEADER;
			}
		} break;
	}

	if ( agk::GetRenderer() )
	{
		agk::Print( agk::ScreenFPS() );
		switch( debugMode )
		{
			case DEBUG_MODE_IDLE: agk::Print( "Initializing" ); break;
			case DEBUG_MODE_DEBUG: agk::Print( "Debugging" ); break;
			case DEBUG_MODE_BROADCAST: agk::Print( "Broadcasting" ); break;
			default: agk::Print( "Unknown" ); break;
		}

		agk::Sync();
	}
	else
	{
		agk::Sleep( 10 );
	}

	return 0; // return 1 to close app
}


void app::End (void)
{

}
