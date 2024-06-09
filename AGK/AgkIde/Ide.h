#ifndef _H_AGK_TEMPLATE_
#define _H_AGK_TEMPLATE_

#define NOMINMAX // disable min max from include files, we have our own.

// Link to GDK libraries
#include "agk.h"
#ifdef AGK_MACOS
#include "../../platform/mac/GLFW/glfw3.h"
#endif
#ifdef AGK_LINUX
#include "GLFW/glfw3.h"
#endif


#define DEVICE_WIDTH 1280
#define DEVICE_HEIGHT 1024

#define TRIALVERSION 4 //New trial version gives another 14 days trial to current trial users.
#define VERSION "v2024.06.09"
#define INTVERSION 114 //Used to trigger auto update "additional files" and update Android export files

#define WINDOW_TITLE "AppGameKit Studio " VERSION
#define LWINDOW_TITLE L"AppGameKit Studio " VERSION
#define WINDOW_TRIALTITLE "AppGameKit Studio " VERSION " Trial"
#define LWINDOW_TRIALTITLE L"AppGameKit Studio " VERSION " Trial"


//#define WINDOW_TITLE "AppGameKit Studio - Debug Version."
//#define LWINDOW_TITLE L"AppGameKit Studio - Debug Version."


//#define DEVVERSION
//#define DISPLAYFPSTITLE
//#define STARTUPPROBLEM
//#define MINIMUMFONTSIZE
//#define DISPLAYCLASSICNEWS // for debug only.
//#define NONEWSSYSTEM

#define MINIMUM_WIDTH 900
#define MINIMUM_HEIGHT 500

#define TRIALEXPIREDAYS 14

#define DEVICE_POS_X 32
#define DEVICE_POS_Y 32
#define FULLSCREEN false

void MoveCam(void);
void MousePressedOutSideAGK(void);
void AddToAssetsBrowser(char *selectedfolder);
void AddSourceFile(char * cFileSelected);
void AddProjectFile(char * cFileSelected);

// used to make a more unique folder for the write path
#define COMPANY_NAME "TheGameCreators"

// Global values for the app
class app
{
	public:

		// constructor
		app() { memset ( this, 0, sizeof(app)); }

		// main app functions - mike to experiment with a derived class for this..
#ifdef AGK_WINDOWS
		void Begin(HWND agk_hWnd);
#else
		void Begin(GLFWwindow* window);
#endif
		int Loop( void );
		void End( void );
};

extern app App;

#endif

// Allow us to use the LoadImage function name
#ifdef LoadImage
 #undef LoadImage
#endif
