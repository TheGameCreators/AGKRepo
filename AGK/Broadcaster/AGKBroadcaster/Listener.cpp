#include "Listener.h"

namespace AGKBroadcaster
{

AGKListener::AGKListener()
{
	m_pDeviceList = 0;
	m_iNumFiles = 0;
	m_pFiles = 0;
	m_iRun = 0;
	m_iDebug = 0;
	m_iAllMode = 0;

	m_pBreakpoints = 0;
	m_pWatchVariables = 0;
}

AGKListener::~AGKListener()
{
	Stop();
	Join();

	{
		cAutoSLock lock( &m_DeviceLock );

		while( m_pDeviceList )
		{
			AGKDevice *pDevice = m_pDeviceList;
			m_pDeviceList = m_pDeviceList->m_pNext;
			delete pDevice;
		}
	}

	{
		cAutoLock autolock(&m_lockBreakpoint);

		while ( m_pBreakpoints )
		{
			Breakpoint *pNext = m_pBreakpoints->m_pNext;
			delete m_pBreakpoints;
			m_pBreakpoints = pNext;
		}
	}

	{
		cAutoLock autolock(&m_lockVariableWatch);

		while ( m_pWatchVariables )
		{
			VariableWatch *pNext = m_pWatchVariables->m_pNext;
			delete m_pWatchVariables;
			m_pWatchVariables = pNext;
		}
	}
}

void AGKListener::SetConnectAll( int mode )
{
	m_iAllMode = mode;
}

void AGKListener::RunApp()
{
	cAutoSLock lock( &m_DeviceLock );

	m_iRun = 1;
	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->RunApp();
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::DebugApp()
{
	cAutoSLock lock( &m_DeviceLock );

	m_iDebug = 1;
	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->DebugApp();
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::PauseApp()
{
	cAutoSLock lock( &m_DeviceLock );

	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->PauseApp();
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::ResumeApp()
{
	cAutoSLock lock( &m_DeviceLock );

	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->ResumeApp();
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::Step()
{
	cAutoSLock lock( &m_DeviceLock );

	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->Step();
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::StepOver()
{
	cAutoSLock lock( &m_DeviceLock );

	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->StepOver();
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::StepOut()
{
	cAutoSLock lock( &m_DeviceLock );

	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->StepOut();
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::SetFrame( int iFrame )
{
	cAutoSLock lock( &m_DeviceLock );

	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->SetFrame( iFrame );
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::StopApp()
{
	cAutoSLock lock( &m_DeviceLock );

	m_iRun = 0;
	m_iDebug = 0;
	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->StopApp();
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::Disconnect()
{
	cAutoSLock lock( &m_DeviceLock );

	m_iRun = 0;
	m_iDebug = 0;
	while( m_pDeviceList )
	{
		AGKDevice *pDevice = m_pDeviceList;
		m_pDeviceList = m_pDeviceList->m_pNext;
		delete pDevice;
	}
}

void AGKListener::AddBreakpoint( const char *szFile, int line )
{
	{
		cAutoLock lock( &m_lockBreakpoint );

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

	{
		cAutoSLock lock( &m_DeviceLock );

		AGKDevice *pDevice = m_pDeviceList;
		while( pDevice )
		{
			pDevice->AddBreakpoint( szFile, line );
			pDevice = pDevice->m_pNext;
		}
	}
}

void AGKListener::RemoveBreakpoint( const char *szFile, int line )
{
	{
		cAutoLock autolock(&m_lockBreakpoint);

		Breakpoint *pBreakpoint = m_pBreakpoints;
		while ( pBreakpoint )
		{
			if ( pBreakpoint->m_sIncludeFile.CompareCaseTo( szFile ) == 0 && pBreakpoint->m_iLine == line )
			{
				Breakpoint *pNext = pBreakpoint->m_pNext;

				if ( pBreakpoint->m_pLast ) pBreakpoint->m_pLast->m_pNext = pBreakpoint->m_pNext;
				else m_pBreakpoints = pBreakpoint->m_pNext;
				if ( pBreakpoint->m_pNext ) pBreakpoint->m_pNext->m_pLast = pBreakpoint->m_pLast;
				delete pBreakpoint;

				pBreakpoint = pNext;
			}
			else
			{
				pBreakpoint = pBreakpoint->m_pNext;
			}
		}
	}

	{
		cAutoSLock lock( &m_DeviceLock );

		AGKDevice *pDevice = m_pDeviceList;
		while( pDevice )
		{
			pDevice->RemoveBreakpoint( szFile, line );
			pDevice = pDevice->m_pNext;
		}
	}
}

void AGKListener::RemoveAllBreakpoints()
{
	{
		cAutoLock autolock(&m_lockBreakpoint);

		while ( m_pBreakpoints )
		{
			Breakpoint *pNext = m_pBreakpoints->m_pNext;
			delete m_pBreakpoints;
			m_pBreakpoints = pNext;
		}
	}

	{
		cAutoSLock lock( &m_DeviceLock );

		AGKDevice *pDevice = m_pDeviceList;
		while( pDevice )
		{
			pDevice->RemoveAllBreakpoints();
			pDevice = pDevice->m_pNext;
		}
	}
}


void AGKListener::AddVariableWatch( const char *szVar )
{
	{
		cAutoLock lock( &m_lockVariableWatch );

		// check for duplicates
		VariableWatch *pVar = m_pWatchVariables;
		while ( pVar )
		{
			if ( pVar->m_sExpression.CompareCaseTo( szVar ) == 0 ) return;
			pVar = pVar->m_pNext;
		}

		pVar = new VariableWatch(); 
		pVar->m_sExpression.SetStr( szVar );
		pVar->m_pNext = m_pWatchVariables;
		if ( pVar->m_pNext )
		{
			pVar->m_pNext->m_pLast = pVar;
		}

		m_pWatchVariables = pVar;
	}

	{
		cAutoSLock lock( &m_DeviceLock );

		AGKDevice *pDevice = m_pDeviceList;
		while( pDevice )
		{
			pDevice->AddVariableWatch( szVar );
			pDevice = pDevice->m_pNext;
		}
	}
}

void AGKListener::RemoveVariableWatch( const char *szVar )
{
	{
		cAutoLock autolock(&m_lockVariableWatch);

		VariableWatch *pVar = m_pWatchVariables;
		while ( pVar )
		{
			if ( pVar->m_sExpression.CompareCaseTo( szVar ) == 0 )
			{
				VariableWatch *pNext = pVar->m_pNext;

				if ( pVar->m_pLast ) pVar->m_pLast->m_pNext = pVar->m_pNext;
				else m_pWatchVariables = pVar->m_pNext;
				if ( pVar->m_pNext ) pVar->m_pNext->m_pLast = pVar->m_pLast;
				delete pVar;

				pVar = pNext;
			}
			else
			{
				pVar = pVar->m_pNext;
			}
		}
	}

	{
		cAutoSLock lock( &m_DeviceLock );

		AGKDevice *pDevice = m_pDeviceList;
		while( pDevice )
		{
			pDevice->RemoveVariableWatch( szVar );
			pDevice = pDevice->m_pNext;
		}
	}
}

void AGKListener::RemoveAllWatchVariables()
{
	{
		cAutoLock autolock(&m_lockVariableWatch);

		while ( m_pWatchVariables )
		{
			VariableWatch *pNext = m_pWatchVariables->m_pNext;
			delete m_pWatchVariables;
			m_pWatchVariables = pNext;
		}
	}

	{
		cAutoSLock lock( &m_DeviceLock );

		AGKDevice *pDevice = m_pDeviceList;
		while( pDevice )
		{
			pDevice->RemoveAllWatchVariables();
			pDevice = pDevice->m_pNext;
		}
	}
}

void AGKListener::SetVariable( const char *szVar, const char *szValue )
{
	cAutoSLock lock( &m_DeviceLock );

	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		pDevice->SetVariable( szVar, szValue );
		pDevice = pDevice->m_pNext;
	}
}

void AGKListener::SetPort( UINT port, UINT portv6 )
{
	cAutoSLock lock(&m_ListenerLock);

	m_Listener.SetListenPort( "anyip4", port );
	m_Listenerv6.SetListenPort( "anyip6", portv6 ); // link local multicast
}

int AGKListener::GetDeviceList( uString &output )
{
	cAutoSLock lock( &m_DeviceLock );

	output.ClearTemp();

	int count = 0;
	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		if ( pDevice->m_Connection.IsConnected() )
		{
			output.Append( pDevice->GetIP() );
			if ( pDevice->GetDisconnected() == 1 ) output.Append( " - Maybe Disconnected, Awaiting Response" );
			else output.Append( " - Active" );
			output.AppendAscii( '\n' );
		}
		pDevice = pDevice->m_pNext;
		count++;
	}

	return count;
}

UINT AGKListener::Run()
{
	if ( !m_pFiles ) return 0;

	char fromIP[ 128 ];
	UINT fromPort;
	UINT devicePort;

	while( !m_bTerminate )
	{	
		// contain the auto lock in a scope so it unlocks
		{
			cAutoSLock lock( &m_DeviceLock );

			// check existing devices are still connected
			AGKDevice *pDevice = m_pDeviceList;
			AGKDevice *pLast = 0;
			while( pDevice )
			{
				AGKDevice *pNext = pDevice->m_pNext;

				if ( pDevice->GetDisconnected() > 1 ) 
				{
					pDevice->Stop();
					pDevice->Join();
					if ( pLast ) pLast->m_pNext = pDevice->m_pNext;
					else m_pDeviceList = pDevice->m_pNext;
					delete pDevice;
				}
				else pLast = pDevice;

				pDevice = pNext;
			}
		}

		if ( m_iAllMode == 1 )
		{
			cAutoSLock lock( &m_ListenerLock );
			while ( 1 )
			{
				AGKPacket packet;
				if ( !m_Listener.ReceivedBroadcast() || !m_Listener.GetPacket( packet, fromPort, fromIP ) )
				{
					if ( !m_Listenerv6.ReceivedBroadcast() || !m_Listenerv6.GetPacket( packet, fromPort, fromIP ) ) 
					{
						// both failed
						break;
					}
				}

				devicePort = packet.GetUInt();
				
				cAutoSLock lock( &m_DeviceLock );

				AGKDevice *pDevice = m_pDeviceList;
				while( pDevice )
				{
					if ( pDevice->Matches( fromIP ) ) 
					{
						// broadcast received from device already connected, has it previously gone quiet
						if ( pDevice && pDevice->GetUnresponsive() > 0 )
						{
							// yes, close old connection
							pDevice->m_Connection.ForceClose();
						}
						else
						{
							// no, ignore broadcast
							break;
						}
					}
					pDevice = pDevice->m_pNext;
				}

				// add device to local store
				if ( !pDevice )
				{
					pDevice = new AGKDevice( fromIP, devicePort );
					pDevice->SetProjectFiles( m_sAppName, m_pFiles, m_iNumFiles );
					pDevice->m_pNext = m_pDeviceList;
					m_pDeviceList = pDevice;
					pDevice->Start(); // connect to the device

					// send it any breakpoints
					{
						cAutoLock lock2( &m_lockBreakpoint );
						Breakpoint *pBreakpoint = m_pBreakpoints;
						while( pBreakpoint )
						{
							pDevice->AddBreakpoint( pBreakpoint->m_sIncludeFile, pBreakpoint->m_iLine );
							pBreakpoint = pBreakpoint->m_pNext;
						}
					}

					// send it any watch variables
					{
						cAutoLock lock2( &m_lockVariableWatch );
						VariableWatch *pVar = m_pWatchVariables;
						while( pVar )
						{
							pDevice->AddVariableWatch( pVar->m_sExpression );
							pVar = pVar->m_pNext;
						}
					}

					if ( m_iDebug ) pDevice->DebugApp();
					else if ( m_iRun ) pDevice->RunApp();
				}
			}
		}

		SleepSafe( 50 );
	}

	return 0;
}

void AGKListener::AddDevice( const char* IP, unsigned int port )
{
	cAutoSLock lock( &m_DeviceLock );

	AGKDevice *pDevice = m_pDeviceList;
	while( pDevice )
	{
		if ( pDevice->Matches( IP ) ) break;
		pDevice = pDevice->m_pNext;
	}

	// add device to local store
	if ( !pDevice )
	{
		pDevice = new AGKDevice( IP, port );
		pDevice->SetProjectFiles( m_sAppName, m_pFiles, m_iNumFiles );
		pDevice->m_pNext = m_pDeviceList;
		m_pDeviceList = pDevice;
		pDevice->Start(); // connect to the device
		
		// send it any breakpoints
		{
			cAutoLock lock2( &m_lockBreakpoint );
			Breakpoint *pBreakpoint = m_pBreakpoints;
			while( pBreakpoint )
			{
				pDevice->AddBreakpoint( pBreakpoint->m_sIncludeFile, pBreakpoint->m_iLine );
				pBreakpoint = pBreakpoint->m_pNext;
			}
		}

		// send it any watch variables
		{
			cAutoLock lock2( &m_lockVariableWatch );
			VariableWatch *pVar = m_pWatchVariables;
			while( pVar )
			{
				pDevice->AddVariableWatch( pVar->m_sExpression );
				pVar = pVar->m_pNext;
			}
		}

		if ( m_iDebug ) pDevice->DebugApp();
		else if ( m_iRun ) pDevice->RunApp();
	}
}

void AGKListener::SetAppName( const char *name )
{
	m_sAppName.SetStr( name );
}

int AGKListener::ReadProjectFiles()
{
	//if ( m_pFiles ) return 0;
	if ( m_pFiles ) delete [] m_pFiles;
	m_pFiles = 0;
	m_iNumFiles = 0;

	//SetCurrentDirectory( "media" );
    agk::SetFolder( "/media" );
    int count = ReadProjectFilesInternal( 1, "" );

	agk::SetFolder( "/Plugins" );
	count += ReadProjectFilesInternal( 1, "/Plugins" );

	if ( count == 0 ) return 0;
	m_pFiles = new uString[ count ];

	agk::SetFolder( "/media" );
	ReadProjectFilesInternal( 0, "" );

	agk::SetFolder( "/Plugins" );
	ReadProjectFilesInternal( 0, "/Plugins" );

	agk::SetFolder( "/media" );
	return count;
}

int AGKListener::ReadProjectFilesInternal( int countonly, const char* path )
{
	int count = 0;
    
    // files
    char *filename = agk::GetFirstFile(1);
    while( filename && *filename )
    {
        bool bFileTypeAllowed = true;
        const char *ext = strrchr( filename, '.' );
        if ( ext )
        {
            uString sExt( ext );
            //if ( sExt.CompareCaseTo( ".agc" ) == 0 ) bFileTypeAllowed = false;
            //if ( sExt.CompareCaseTo( ".dll" ) == 0 ) bFileTypeAllowed = false;
            if ( sExt.CompareCaseTo( ".app" ) == 0 ) bFileTypeAllowed = false;
            if ( sExt.CompareCaseTo( ".agk" ) == 0 ) bFileTypeAllowed = false;
            //if ( sExt.CompareCaseTo( ".lib" ) == 0 ) bFileTypeAllowed = false;
            //if ( sExt.CompareCaseTo( ".so" ) == 0 ) bFileTypeAllowed = false;
            if ( sExt.CompareCaseTo( ".a" ) == 0 ) bFileTypeAllowed = false;
        }
        if ( bFileTypeAllowed==true )
        {
            if ( !countonly )
            {
                char fullpath[ MAX_PATH ];
                strcpy( fullpath, path );
                if ( *path != 0 ) strcat( fullpath, "/" );
                strcat( fullpath, filename );
                m_pFiles[ m_iNumFiles++ ].SetStr( fullpath );
            }
            count++;
        }
        
        delete [] filename;
        filename = agk::GetNextFile();
    }
    
    if ( filename ) delete [] filename;
    
    // recurse folders
    int foldercount = agk::GetFolderCount(1);
    if ( foldercount == 0 ) return count;
    
    char** sFolders = new char*[ foldercount ];
    sFolders[ 0 ] = agk::GetFirstFolder(1);
    for ( int i = 1; i < foldercount; i++ )
    {
        sFolders[ i ] = agk::GetNextFolder();
    }
    
    for ( int i = 0; i < foldercount; i++ )
    {
	    char newpath[ MAX_PATH ];
        strcpy( newpath, path );
        if ( *path != 0 ) strcat( newpath, "/" );
        strcat( newpath, sFolders[i] );
        agk::SetFolder( sFolders[i] );
        count += ReadProjectFilesInternal( countonly, newpath );
        agk::SetFolder( ".." );
        
        delete [] sFolders[i];
    }
    
    delete [] sFolders;
	return count;
}

} // AGKBroadcaster namespace