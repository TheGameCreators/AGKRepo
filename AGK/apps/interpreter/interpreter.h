#ifndef _H_APP
#define _H_APP

// Link to GDK libraries
#include "agk.h"
#include "ProgramData.h"

#define DEVICE_WIDTH 1024
#define DEVICE_HEIGHT 768
#define DEVICE_POS_X 32
#define DEVICE_POS_Y 32
#define FULLSCREEN false

void GeneralError( const char *msg );

enum AppState
{
	APP_IDLE,
	APP_NETWORK_SETUP,
	APP_WAITING,
	APP_CONNECTED,
	APP_RECEIVING,
	APP_WAITING2,
	APP_PAUSED,
	APP_RUNNING,
	APP_ERROR,
	APP_NETWORK_ERROR,
	APP_RUNTIME_ERROR
};

// Global values for the app
class app
{
	public:

		// Interpreter graphics view
		cImage* g_pArialImage;
		cImage* g_pAsciiImage;
		struct sAGKHUD
		{
			cImage* pImage;
			cSprite* pSprite; 
		};
		sAGKHUD g_pAGKBackdrop;
		sAGKHUD g_pAGKBackdropSpinner; 
		sAGKHUD g_pAGKBackdropLogo; 
		sAGKHUD g_pAGKBackdropPower; 
		sAGKHUD g_pAGKBackdropHelp; 
		sAGKHUD g_pAGKBackdropHelpIcon; 
		cImage* g_pAGKInterpreterFont;
		int g_iShowingHelp;
		AGK::cText* g_pText[5];

		// networking
		Broadcaster m_DeviceBroadcaster; // used to tell the IDE this device exists
		Broadcaster m_DeviceBroadcasterv6; // used to tell the IDE this device exists
		cNetworkListener m_ConnectionListener; // used to receive the connection from the broadcaster
		cNetworkListener m_ConnectionListenerv6; // used to receive the connection from the broadcaster
		cFileReceiver m_FileReceiver; // used for transferring the remote app files and bytcode
		AGKSocket* volatile m_pConnection; // connection to broadcaster
		uString m_sListeningIP; // the IP address the listener is listening on
		uString m_sListeningIPv6; // the IP address the listener is listening on
		int m_iBroadcastPort; // the port used by m_DeviceBroadcaster
		int m_iConnectionPort; // the port used by m_ConnectionListener
		int m_iBroadcastPortv6; // the port used by m_DeviceBroadcaster
		int m_iConnectionPortv6; // the port used by m_ConnectionListener
		uString m_sIP;
		uString m_sIPv6;
		int m_iPingResponded;
		float m_fPingStart;

		// Early initialising variables
		char g_pWindowTitle [ 512 ];
		unsigned int g_dwResolutionMode;
		unsigned int g_dwBroadcastMode;
		int g_iPrevWidth;
		int g_iPrevHeight;

		// variables
		char m_pOriginalRootDir[512];
		AppState m_iAppControlStage;
		int m_iStandAlone;
		int m_iDebugMode;
		int m_iStepMode;
        int m_iRequestTrackingPermission;
        int m_iUseFirebase = 1;

		ProgramData m_sProgram;

		void CheckConnection();
		void CheckMessages();

	public:

		app();
		~app();

		void Begin( void );
		void Loop( void );
		void End( void );

		void CreateResources();
		void DeleteResources();

		void UpdateInterpreterAspect( void );
		void DrawBackground();
		void DrawText( const char* text1, const char* text2, const char* text3 );

		void AppPause();
		void AppResume();

		void AppFinished();
		void AppReload();
		void AppClose();
		void AppQuit();
        void AppSplash();

		void OnAppDebugInfo( const char *szType, const char *szData );
		void OnAppWarning( const char *szMsg );
		void OnAppError( const char *szMsg );
		void OnAppBreak( const char *szInclude, int line );
};

extern app App;
extern void PlatformAppQuit();

#endif

// Allow us to use the LoadImage function name
#ifdef LoadImage
 #undef LoadImage
#endif
