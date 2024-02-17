#include "Device.h"
#include "AGKBroadcaster.h"

namespace AGKBroadcaster
{

int Breakpoint::m_iCount = 0;

AGKDevice::AGKDevice( const char* ip, UINT port )
{
	m_sIP.SetStr( ip );
	m_iPort = port;
	m_pNext = 0;
	m_iPingResponded = 1;
	m_fPingStart = 0;
	m_pFileSender = 0;
	m_TransferProgress = 0;
	m_iRun = 0;
	m_iDebug = 0;
	m_iPause = 0;
	m_iStop = 0;
	m_iResume = 0;
	m_iStepMode = 0;
	m_iSetFrame = -1;
	m_iMaybeDisconnected = 0;

	m_pBreakpoints = 0;
	m_pWatchVariables = 0;
	m_pSetVariables = 0;
	m_pSetVariablesLast = 0;
}

void AGKDevice::Stop()
{
	AGKThread::Stop();

	if ( m_pFileSender->IsRunning() ) 
	{
		m_pFileSender->Stop();
		m_Connection.Close();
	}
}

AGKDevice::~AGKDevice()
{
	Stop();
	Join();

	if ( m_pFileSender ) delete m_pFileSender;
	
	while( m_pBreakpoints )
	{
		Breakpoint* pBreakpoint = m_pBreakpoints;
		m_pBreakpoints = m_pBreakpoints->m_pNext;
		delete pBreakpoint;
	}

	while( m_pWatchVariables )
	{
		VariableWatch* pVar = m_pWatchVariables;
		m_pWatchVariables = m_pWatchVariables->m_pNext;
		delete pVar;
	}
}

int AGKDevice::Matches( const char* ip )
{
	return (m_sIP.CompareTo( ip ) == 0) ? 1 : 0;
}

UINT AGKDevice::Run()
{
	if ( m_sIP.GetLength() == 0 ) return 0;
	if ( m_iPort == 0 || m_iPort > 65535 ) return 0;
	if ( !m_pFileSender ) return 0;

	// connect
	if ( !m_Connection.Connect( m_sIP.GetStr(), m_iPort, 6000 ) ) 
	{
		// AGK will already have sent a warning
		//uString sInfo;
		//sInfo.Format( "Failed to connect to %s", m_sIP.GetStr() );
		//WriteWarning( sInfo );
		return 0;
	}

	m_Connection.SendString( m_sAppName.GetStr() );
	m_Connection.Flush();

	// transfer project files
	m_pFileSender->SetReceiver( &m_Connection, false );
	m_pFileSender->Start();

	while ( !m_bTerminate && m_pFileSender->IsRunning() )
	{
		m_TransferProgress = m_pFileSender->GetProgress();
		SleepSafe( 100 );
	}

	if ( m_bTerminate ) return 0;

	m_Connection.Flush();

	// transition into command mode
	m_iPingResponded = 1;
	m_fPingStart = agk::GetRunTime();

	while ( !m_bTerminate && !m_Connection.GetDisconnected() )
	{
		SleepSafe( 50 );

		// check for breakpoint changes
		{
			// scope the auto lock so it releases as soon as possible
			cAutoLock autolock(&m_lockBreakpoint);

			Breakpoint *pBreakpoint = m_pBreakpoints;
			while( pBreakpoint )
			{
				if ( (pBreakpoint->m_iFlags & BREAKPOINT_FLAG_DELETE) != 0 )
				{
					// tell the attached device to delete the breakpoint
					m_Connection.SendUInt( 6 ); // delete breakpoint
					//m_Connection.SendInt( pBreakpoint->m_iID );
					m_Connection.SendString( pBreakpoint->m_sIncludeFile.GetStr() );
					m_Connection.SendInt( pBreakpoint->m_iLine );

					Breakpoint *pNext = pBreakpoint->m_pNext;
					DeleteBreakpoint( pBreakpoint );
					pBreakpoint = pNext;
				}
				else if ( (pBreakpoint->m_iFlags & BREAKPOINT_FLAG_UPLOADED) == 0 )
				{
					// upload breakpoint to device
					m_Connection.SendUInt( 5 ); // add breakpoint
					//m_Connection.SendInt( pBreakpoint->m_iID );
					m_Connection.SendString( pBreakpoint->m_sIncludeFile.GetStr() );
					m_Connection.SendInt( pBreakpoint->m_iLine );

					pBreakpoint->m_iFlags |= BREAKPOINT_FLAG_UPLOADED;
					pBreakpoint = pBreakpoint->m_pNext;
				}
				else
				{
					pBreakpoint = pBreakpoint->m_pNext;
				}
			}
		}

		// check for variable watch changes
		{
			// scope the auto lock so it releases as soon as possible
			cAutoLock autolock(&m_lockVariableWatch);

			VariableWatch *pVar = m_pWatchVariables;
			while( pVar )
			{
				if ( (pVar->m_iFlags & WATCH_FLAG_DELETE) != 0 )
				{
					// tell the attached device to delete the watch
					m_Connection.SendUInt( 13 ); // delete watch
					m_Connection.SendString( pVar->m_sExpression.GetStr() );

					VariableWatch *pNext = pVar->m_pNext;
					DeleteVariableWatch( pVar );
					pVar = pNext;
				}
				else if ( (pVar->m_iFlags & WATCH_FLAG_UPLOADED) == 0 )
				{
					// upload watch to device
					m_Connection.SendUInt( 12 ); // add watch
					m_Connection.SendString( pVar->m_sExpression.GetStr() );
					
					pVar->m_iFlags |= WATCH_FLAG_UPLOADED;
					pVar = pVar->m_pNext;
				}
				else
				{
					pVar = pVar->m_pNext;
				}
			}
		}

		// check for set variable changes
		{
			// scope the auto lock so it releases as soon as possible
			cAutoLock autolock(&m_lockVariableSet);

			VariableSet *pVar = m_pSetVariables;
			while( pVar )
			{
				m_Connection.SendUInt( 15 ); // set variable
				m_Connection.SendString( pVar->m_sExpression.GetStr() );
				m_Connection.SendString( pVar->m_sValue.GetStr() );

				VariableSet *pNext = pVar->m_pNext;
				delete pVar;
				pVar = pNext;
			}

			m_pSetVariables = 0;
			m_pSetVariablesLast = 0;
		}

		if ( m_iRun )
		{
			m_iRun = 0;
			m_Connection.SendUInt( 2 ); // run app
		}

		if ( m_iDebug )
		{
			m_iDebug = 0;
			m_Connection.SendUInt( 8 ); // debug app
		}

		if ( m_iStop )
		{
			m_iStop = 0;
			m_Connection.SendUInt( 3 ); // stop app
		}

		if ( m_iPause )
		{
			m_iPause = 0;
			m_Connection.SendUInt( 4 ); // pause app
		}

		if ( m_iResume )
		{
			m_iResume = 0;
			m_Connection.SendUInt( 7 ); // resume app
		}

		if ( m_iStepMode == 1 )
		{
			m_iStepMode = 0;
			m_Connection.SendUInt( 9 ); // step in
		}

		if ( m_iStepMode == 2 )
		{
			m_iStepMode = 0;
			m_Connection.SendUInt( 10 ); // step over
		}

		if ( m_iStepMode == 3 )
		{
			m_iStepMode = 0;
			m_Connection.SendUInt( 11 ); // step out
		}

		if ( m_iSetFrame >= 0 )
		{
			m_Connection.SendUInt( 14 ); // set frame
			m_Connection.SendInt( m_iSetFrame );
			m_iSetFrame = -1;
		}

		m_Connection.Flush();

		CheckMessages();
		CheckConnection();
	}
	
	if ( !m_Connection.GetDisconnected() )
	{
		if ( m_iStop ) 
		{
			m_iStop = 0;
			m_Connection.SendUInt( 3 ); // stop app
		}
		m_Connection.SendUInt( 99 ); // disconnect
		m_Connection.Flush();
		m_Connection.Close();
	}

	return 0;
}

void AGKDevice::SetProjectFiles( const char* name, const uString* sFiles, UINT numFiles )
{
	if ( m_pFileSender ) return;

	m_pFileSender = new cFileSender( numFiles );
	m_sAppName.SetStr( name );
	
	for ( UINT i = 0; i < numFiles; i++ )
	{
		m_pFileSender->AddFile( sFiles[ i ] );
	}
}

void AGKDevice::RunApp()
{
	m_iRun = 1;
}

void AGKDevice::DebugApp()
{
	m_iDebug = 1;
}

void AGKDevice::PauseApp()
{
	m_iPause = 1;
}

void AGKDevice::ResumeApp()
{
	m_iResume = 1;
}

void AGKDevice::Step()
{
	m_iStepMode = 1;
}

void AGKDevice::StepOver()
{
	m_iStepMode = 2;
}

void AGKDevice::StepOut()
{
	m_iStepMode = 3;
}

void AGKDevice::SetFrame( int iFrame )
{
	m_iSetFrame = iFrame;
}

void AGKDevice::StopApp()
{
	m_iStop = 1;
}

void AGKDevice::AddBreakpoint( const char *szFile, int line )
{
	cAutoLock autolock(&m_lockBreakpoint);

	Breakpoint *pBreakpoint = new Breakpoint(); 
	pBreakpoint->m_iLine = line;
	pBreakpoint->m_sIncludeFile.SetStr( szFile );
	pBreakpoint->m_pNext = m_pBreakpoints;
	if ( pBreakpoint->m_pNext )
	{
		pBreakpoint->m_pNext->m_pLast = pBreakpoint;
	}

	m_pBreakpoints = pBreakpoint;
}

void AGKDevice::RemoveBreakpoint( const char *szFile, int line )
{
	cAutoLock autolock(&m_lockBreakpoint);

	Breakpoint *pBreakpoint = m_pBreakpoints;
	while ( pBreakpoint )
	{
		if ( pBreakpoint->m_sIncludeFile.CompareCaseTo( szFile ) == 0 && pBreakpoint->m_iLine == line )
		{
			// if the breakpoint has not yet been uploaded then delete it from the list, otherwise mark it for deletion
			if ( (pBreakpoint->m_iFlags & BREAKPOINT_FLAG_UPLOADED) == 0 ) 
			{
				Breakpoint *pNext = pBreakpoint->m_pNext;
				DeleteBreakpoint( pBreakpoint );
				pBreakpoint = pNext;
			}
			else
			{
				pBreakpoint->m_iFlags |= BREAKPOINT_FLAG_DELETE;
				pBreakpoint = pBreakpoint->m_pNext;
			}
		}
		else
		{
			pBreakpoint = pBreakpoint->m_pNext;
		}
	}
}

void AGKDevice::RemoveAllBreakpoints()
{
	cAutoLock autolock(&m_lockBreakpoint);

	Breakpoint *pBreakpoint = m_pBreakpoints;
	while ( pBreakpoint )
	{
		// if the breakpoint has not yet been uploaded then delete it from the list, otherwise mark it for deletion
		if ( (pBreakpoint->m_iFlags & BREAKPOINT_FLAG_UPLOADED) == 0 ) 
		{
			Breakpoint *pNext = pBreakpoint->m_pNext;
			DeleteBreakpoint( pBreakpoint );
			pBreakpoint = pNext;
		}
		else
		{
			pBreakpoint->m_iFlags |= BREAKPOINT_FLAG_DELETE;
			pBreakpoint = pBreakpoint->m_pNext;
		}
	}
}

// must hold the breakpoint lock before calling, not checked
void AGKDevice::DeleteBreakpoint( Breakpoint *pBreakpoint )
{
	if ( pBreakpoint->m_pLast ) pBreakpoint->m_pLast->m_pNext = pBreakpoint->m_pNext;
	else m_pBreakpoints = pBreakpoint->m_pNext;

	if ( pBreakpoint->m_pNext ) pBreakpoint->m_pNext->m_pLast = pBreakpoint->m_pLast;

	delete pBreakpoint;
}

void AGKDevice::AddVariableWatch( const char *szVar )
{
	cAutoLock autolock(&m_lockVariableWatch);

	VariableWatch *pVar = new VariableWatch(); 
	pVar->m_sExpression.SetStr( szVar );
	pVar->m_pNext = m_pWatchVariables;
	if ( pVar->m_pNext )
	{
		pVar->m_pNext->m_pLast = pVar;
	}

	m_pWatchVariables = pVar;
}

void AGKDevice::RemoveVariableWatch( const char *szVar )
{
	cAutoLock autolock(&m_lockVariableWatch);

	VariableWatch *pVar = m_pWatchVariables;
	while ( pVar )
	{
		if ( pVar->m_sExpression.CompareCaseTo( szVar ) == 0 )
		{
			// if the watch has not yet been uploaded then delete it from the list, otherwise mark it for deletion
			if ( (pVar->m_iFlags & WATCH_FLAG_UPLOADED) == 0 ) 
			{
				VariableWatch *pNext = pVar->m_pNext;
				DeleteVariableWatch( pVar );
				pVar = pNext;
			}
			else
			{
				pVar->m_iFlags |= WATCH_FLAG_DELETE;
				pVar = pVar->m_pNext;
			}
		}
		else
		{
			pVar = pVar->m_pNext;
		}
	}
}

void AGKDevice::RemoveAllWatchVariables()
{
	cAutoLock autolock(&m_lockVariableWatch);

	VariableWatch *pVar = m_pWatchVariables;
	while ( pVar )
	{
		// if the watch has not yet been uploaded then delete it from the list, otherwise mark it for deletion
		if ( (pVar->m_iFlags & WATCH_FLAG_UPLOADED) == 0 ) 
		{
			VariableWatch *pNext = pVar->m_pNext;
			DeleteVariableWatch( pVar );
			pVar = pNext;
		}
		else
		{
			pVar->m_iFlags |= WATCH_FLAG_DELETE;
			pVar = pVar->m_pNext;
		}
	}
}

// must hold the variable lock before calling, not checked
void AGKDevice::DeleteVariableWatch( VariableWatch *pVar )
{
	if ( pVar->m_pLast ) pVar->m_pLast->m_pNext = pVar->m_pNext;
	else m_pWatchVariables = pVar->m_pNext;

	if ( pVar->m_pNext ) pVar->m_pNext->m_pLast = pVar->m_pLast;

	delete pVar;
}

void AGKDevice::SetVariable( const char *szVar, const char *szValue )
{
	cAutoLock autolock(&m_lockVariableSet);

	VariableSet *pVar = new VariableSet(); 
	pVar->m_sExpression.SetStr( szVar );
	pVar->m_sValue.SetStr( szValue );
	pVar->m_pNext = 0;
	if ( m_pSetVariablesLast )
	{
		m_pSetVariablesLast->m_pNext = pVar;
	}
	else
	{
		m_pSetVariables = pVar;
	}

	m_pSetVariablesLast = pVar;
}

void AGKDevice::CheckConnection()
{
	if ( m_iPingResponded )
	{
		m_iMaybeDisconnected = 0;

		if ( m_fPingStart + 2 < agk::GetRunTime() )
		{
			m_iPingResponded = 0;
			m_fPingStart = agk::GetRunTime();
			m_Connection.SendUInt( 0 ); //ping
			m_Connection.Flush();
		}
	}
	else
	{
		if ( m_fPingStart + 4 < agk::GetRunTime() )
		{
			// client hasn't responded in 4 seconds, mark as unresponsive.
			m_iMaybeDisconnected++;
			m_fPingStart = agk::GetRunTime();
		}
	}
}

void AGKDevice::CheckMessages()
{
	while ( m_Connection.GetBytes() >= 4 )
	{
		UINT type = m_Connection.RecvUInt();
		if ( m_Connection.GetDisconnected() ) break;

		switch( type )
		{
			case 0: // ping response
			{
				m_iPingResponded = 1;
				break;
			}

			case 1: // client ping
			{
				m_Connection.SendUInt( 1 ); // respond
				break;
			}

			case 2: // warning message
			{
				uString sMsg;
				m_Connection.RecvString( sMsg );
				WriteWarning( sMsg );
				break;
			}

			case 3: // error message
			{
				uString sMsg;
				m_Connection.RecvString( sMsg );
				WriteError( sMsg );
				break;
			}

			case 4: // debug message
			{
				uString sType, sData;
				m_Connection.RecvString( sType );
				m_Connection.RecvString( sData );
				WriteDebug( sType, sData );
				break;
			}

			case 5: // breakpoint hit
			{
				uString sInclude;
				m_Connection.RecvString( sInclude );
				int line = m_Connection.RecvInt();
				WriteBreak( sInclude, line );
				break;
			}
		}

		m_Connection.Flush();
	}
}

int AGKDevice::GetDisconnected()
{
	if ( !IsRunning() || m_Connection.GetDisconnected() ) return 2;
	if ( m_iMaybeDisconnected > 0 ) return 1;
	return 0;
}

int AGKDevice::GetUnresponsive()
{
	if ( !IsRunning() || m_Connection.GetDisconnected() ) return 99;
	return m_iMaybeDisconnected;
}

} // AGKBroadcaster namespace