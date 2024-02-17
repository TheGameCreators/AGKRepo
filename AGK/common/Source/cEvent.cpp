//#include "cEvent.h"
#include "agk.h"

using namespace AGK;

cEvent* cEvent::g_pIter = 0;
cEvent* cEvent::g_pEvents[ EVENT_LIST_SIZE ] = { 0 };
