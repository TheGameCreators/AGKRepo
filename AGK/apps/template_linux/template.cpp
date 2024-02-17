// Includes
#include "template.h"

// Namespace
using namespace AGK;

app App;

void app::Begin(void)
{
	agk::SetVirtualResolution (1024, 768);
	agk::SetClearColor( 151,170,204 ); // light blue
	agk::SetSyncRate(60,0);
	agk::SetScissor(0,0,0,0);
}

int app::Loop (void)
{
	agk::Print( agk::ScreenFPS() );
	agk::Sync();

	return 0; // return 1 to close app
}


void app::End (void)
{

}
