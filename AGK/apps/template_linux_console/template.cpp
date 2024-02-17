// Includes
#include "template.h"

extern "C"{
int* alcOpenDevice(const char*){return 0;}
}

// Namespace
using namespace AGK;

app App;

void app::Begin(void)
{
	
}

void app::Loop (void)
{
	agk::UpdateInput();
	AGKMusicOGG::UpdateAll();
	
	agk::Sleep(33); // sleep for 33 milliseconds
}


void app::End (void)
{

}
