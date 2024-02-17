#ifndef _H_AGK_BROADCASTER_DEVICE
#define _H_AGK_BROADCASTER_DEVICE

#include "agk.h"

using namespace AGK;

#define BREAKPOINT_FLAG_UPLOADED	0x01
#define BREAKPOINT_FLAG_DELETE		0x02

#define WATCH_FLAG_UPLOADED	0x01
#define WATCH_FLAG_DELETE		0x02

namespace AGKBroadcaster
{

class Breakpoint
{
	static int m_iCount;

	public:
		int m_iID;
		uString m_sIncludeFile;
		int m_iLine;
		Breakpoint *m_pLast;
		Breakpoint *m_pNext;
		unsigned int m_iFlags;

		Breakpoint() { m_pLast = 0; m_pNext = 0; m_iID = ++m_iCount; m_iLine = 0; m_iFlags = 0; }
		~Breakpoint() {}
};

class VariableWatch
{
	public:
		uString m_sExpression;
		VariableWatch *m_pLast;
		VariableWatch *m_pNext;
		unsigned int m_iFlags;

		VariableWatch() { m_pLast = 0; m_pNext = 0; m_iFlags = 0; }
		~VariableWatch() {}
};

class VariableSet
{
	public:
		uString m_sExpression;
		uString m_sValue;
		VariableSet *m_pNext;

		VariableSet() { m_pNext = 0; }
		~VariableSet() {}
};

class AGKDevice : public AGKThread
{
	protected:
		uString m_sIP;
		UINT m_iPort;
		float m_fPingStart;
		int m_iPingResponded;
		float m_TransferProgress;
		int m_iRun;
		int m_iDebug;
		int m_iStop;
		int m_iPause;
		int m_iResume;
		int m_iStepMode;
		int m_iSetFrame;
		int m_iMaybeDisconnected;

		uString m_sAppName;
		cFileSender *m_pFileSender;

		cLock m_lockBreakpoint;
		Breakpoint *m_pBreakpoints;
		void DeleteBreakpoint( Breakpoint *pBreakpoint );

		cLock m_lockVariableWatch;
		VariableWatch *m_pWatchVariables;
		void DeleteVariableWatch( VariableWatch *pVar );

		cLock m_lockVariableSet;
		VariableSet *m_pSetVariables;
		VariableSet *m_pSetVariablesLast;

		UINT Run();
		void CheckConnection();
		void CheckMessages();

	public:
		AGKSocket m_Connection;
		AGKDevice *m_pNext;

		void Stop();

		AGKDevice( const char* ip, UINT port );
		~AGKDevice();

		int Matches( const char* ip );
		const char* GetIP() { return m_sIP.GetStr(); }
		void SetProjectFiles( const char* name, const uString* sFiles, UINT numFiles );
		void RunApp();
		void DebugApp();
		void PauseApp();
		void ResumeApp();
		void Step();
		void StepOver();
		void StepOut();
		void SetFrame( int iFrame );
		void StopApp();
		
		void AddBreakpoint( const char *szFile, int line );
		void RemoveBreakpoint( const char *szFile, int line );
		void RemoveAllBreakpoints();

		void AddVariableWatch( const char *szVar );
		void RemoveVariableWatch( const char *szVar );
		void RemoveAllWatchVariables();

		void SetVariable( const char *szVar, const char *szValue );

		int GetDisconnected();
		int GetUnresponsive();
};

} // AGKBroadcaster namespace


#endif