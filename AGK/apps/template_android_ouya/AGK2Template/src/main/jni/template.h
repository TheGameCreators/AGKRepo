#ifndef _H_APP
#define _H_APP

// Include AGK libraries
#include "agk.h"

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
