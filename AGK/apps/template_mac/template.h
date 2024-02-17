#ifndef _H_APP
#define _H_APP

// Include AGK libraries
#include "agk.h"

// used in Core.mm to set the window properties
#define DEVICE_WIDTH 1024
#define DEVICE_HEIGHT 768
#define WINDOW_TITLE "My App Title"
#define FULLSCREEN 0

// used to make a more unique folder for the write path
#define COMPANY_NAME "My Company"

// Global values for the app
class app
{
	public:

		// global game vars

	public:

		// constructor
		app() {}
        ~app() {}

		// main app functions - mike to experiment with a derived class for this..
        void Begin( void );
        int Loop( void );
        void End( void );
};

extern app App;

#endif
