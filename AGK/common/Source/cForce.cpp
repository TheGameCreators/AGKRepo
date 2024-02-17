#include "cForce.h"
#include "agk.h"
#include "Box2D/Box2D.h"

using namespace AGK;

cForce::cForce()
{
	m_fX = 0;
	m_fY = 0;
	m_fRadius = -1;
	m_fPower = 1;
	m_fLimit = 0;
	m_bFade = true;
}

cForce::cForce( float x, float y, float power, float limit, float radius, bool fade )
{
	// radius < 0 equals infinite
	m_fX = x;
	m_fY = y;
	m_fRadius = radius;
	m_fPower = power;
	m_fLimit = limit;
	m_bFade = fade;
}

cForce::~cForce()
{

}
