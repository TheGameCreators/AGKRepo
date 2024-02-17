#ifndef _H_AGK_BROADCASTER_LISTENER
#define _H_AGK_BROADCASTER_LISTENER

#include "agk.h"
#include "Device.h"

#ifdef SetPort
	#undef SetPort
#endif

using namespace AGK;

namespace AGKBroadcaster
{

class AGKListener : public AGKThread
{
	protected:
		BroadcastListener m_Listener;
		BroadcastListener m_Listenerv6;
		cSpinLock m_ListenerLock;
		cSpinLock m_DeviceLock;
		AGKDevice *m_pDeviceList;
		int m_iRun;
		int m_iDebug;

		uString m_sAppName;
		uString* m_pFiles;
		UINT m_iNumFiles;
		int m_iAllMode;

		cLock m_lockBreakpoint;
		Breakpoint *m_pBreakpoints;

		cLock m_lockVariableWatch;
		VariableWatch *m_pWatchVariables;

		UINT Run();

		int ReadProjectFilesInternal( int countonly, const char* path );

	public:
		AGKListener();
		~AGKListener();

		void SetPort( UINT port, UINT portv6 );
		int GetDeviceList( uString &output );

		void SetAppName( const char *name );
		int ReadProjectFiles();
		void SetConnectAll( int mode );

		void RunApp();
		void DebugApp();
		void PauseApp();
		void StopApp();
		void ResumeApp();
		void Step();
		void StepOver();
		void StepOut();
		void SetFrame( int iFrame );
		void Disconnect();

		void AddBreakpoint( const char *szFile, int line );
		void RemoveBreakpoint( const char *szFile, int line );
		void RemoveAllBreakpoints();

		void AddVariableWatch( const char *szVar );
		void RemoveVariableWatch( const char *szVar );
		void RemoveAllWatchVariables();

		void SetVariable( const char *szVar, const char *szValue );

		void AddDevice( const char* IP, unsigned int port );
};

} // AGKBroadcaster namespace

#endif