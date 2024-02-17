//#include "cTouch.h"
//#include "Wrapper.h"
#include "agk.h"

using namespace AGK;

int cTouch::g_iIterIndex = 0;
bool cTouch::g_bIncludeUnknown = false;
cTouch cTouch::g_pTouchEvents[ MAX_AGK_TOUCH_COUNT ];
float cTouch::g_fX = 0;
float cTouch::g_fY = 0;
int cTouch::g_iMoveSensitivity = 36;

void cTouch::Reset() 
{ 
	m_bActive = false;
	m_iUniqueID = 0;
	m_fStartTime = 0.0f;
	m_fTotalTime = 0.0f;
	m_fStartX = 0; 
	m_fStartY = 0; 
	m_fCurrentX = 0; 
	m_fCurrentY = 0; 
	m_fLastX = 0; 
	m_fLastY = 0; 
	m_eType = eUnknown; 
	m_bReleased = true; 
	m_bDelete = false;
	m_iData = 0;
	m_bCaptured = false;
}

void cTouch::Moved( float x, float y )
{
	m_fLastX = m_fCurrentX;
	m_fLastY = m_fCurrentY;
	m_fCurrentX = x;
	m_fCurrentY = y;
	
	float fDiffX = m_fCurrentX - m_fStartX;
	float fDiffY = m_fCurrentY - m_fStartY;
	if ( fDiffX*fDiffX + fDiffY*fDiffY > g_iMoveSensitivity )
	{
		m_eType = eDrag;
	}

	// global last position
	if ( !m_bCaptured )
	{
		g_fX = x;
		g_fY = y;
	}
}

void cTouch::Pressed( float x, float y )
{
	m_eType = eUnknown;
	m_fStartX = x;
	m_fStartY = y;
	m_fCurrentX = x;
	m_fCurrentY = y;
	m_fLastX = x; 
	m_fLastY = y; 
	m_bReleased = false;
	m_bDelete = false;
	m_fStartTime = agk::GetRunTime();

	// global last position
	if ( !m_bCaptured )
	{
		g_fX = x;
		g_fY = y;
	}
}

void cTouch::Released( float x, float y )
{
	if ( m_eType == eUnknown ) m_eType = eShort;
	m_bReleased = true;
	m_fLastX = m_fCurrentX; 
	m_fLastY = m_fCurrentY; 
	m_fCurrentX = x;
	m_fCurrentY = y;
	m_fTotalTime = agk::GetRunTime() - m_fStartTime;

	// global last position
	if ( !m_bCaptured )
	{
		g_fX = x;
		g_fY = y;
	}
}

void cTouch::SetData( int data )
{
	m_iData = data;
}

int cTouch::GetData()
{
	return m_iData;
}

float cTouch::GetAnyX( )
{
	return g_fX;
}

float cTouch::GetAnyY( )
{
	return g_fY;
}

bool cTouch::GetAny( )
{
	for ( uint32_t i = 0; i < MAX_AGK_TOUCH_COUNT; i++ )
	{
		if ( g_pTouchEvents[ i ].m_bActive && !g_pTouchEvents[ i ].m_bCaptured ) 
		{
			// change detection to give short taps 1 frame of grace to be seen
			//if ( !g_pTouchEvents[ i ].m_bReleased ) return true;
			//if ( !g_pTouchEvents[ i ].m_bDelete ) return true;
            return true;
		}
	}
	
	return false;
}


uint32_t cTouch::GetCount( bool bIncludeUnknown )
{
	uint32_t count = 0;
	
	for ( uint32_t i = 0; i < MAX_AGK_TOUCH_COUNT; i++ )
	{
		if ( g_pTouchEvents[ i ].m_bActive && !g_pTouchEvents[ i ].m_bCaptured ) 
		{
			if ( bIncludeUnknown || g_pTouchEvents[ i ].m_eType != eUnknown ) count++;
		}
	}
	
	return count;
}

cTouch* cTouch::GetFirstEvent( bool bIncludeUnknown )
{
	g_bIncludeUnknown = bIncludeUnknown;
	
	for ( int i = 0; i < MAX_AGK_TOUCH_COUNT; i++ )
	{
		if ( g_pTouchEvents[ i ].m_bActive && !g_pTouchEvents[ i ].IsCaptured() ) 
		{
			if ( g_bIncludeUnknown || g_pTouchEvents[ i ].m_eType != eUnknown ) 
			{
				g_iIterIndex = i+1;
				return (g_pTouchEvents + i);
			}
		}
	}
	
	return 0;
}

cTouch* cTouch::GetNextEvent( )
{
	for ( int i = g_iIterIndex; i < MAX_AGK_TOUCH_COUNT; i++ )
	{
		if ( g_pTouchEvents[ i ].m_bActive && !g_pTouchEvents[ i ].IsCaptured() ) 
		{
			if ( g_bIncludeUnknown || g_pTouchEvents[ i ].m_eType != eUnknown ) 
			{
				g_iIterIndex = i+1;
				return (g_pTouchEvents + i);
			}
		}
	}
	
	return 0;
}

void cTouch::UpdateEvents()
{
	// detect hold events
	for ( int i = 0; i < MAX_AGK_TOUCH_COUNT; i++ )
	{
		if ( g_pTouchEvents[ i ].m_bActive )
		{
			g_pTouchEvents[ i ].m_fTotalTime = agk::GetRunTime() - g_pTouchEvents[ i ].m_fStartTime;

			if ( !g_pTouchEvents[ i ].m_bReleased && g_pTouchEvents[ i ].m_eType == eUnknown ) 
			{
				if ( g_pTouchEvents[ i ].m_fTotalTime > 1 ) g_pTouchEvents[ i ].m_eType = eHold;
			}
		}
	}
	
	// delete touch immediately as a bug in bRead meant they were always deleted immediate anyway
    // this function is called near the end of the screen update in Swap so new events should appear
    // after, and the use code will then run before this deletes them, meaning they are seen for one frame
	for ( int i = 0; i < MAX_AGK_TOUCH_COUNT; i++ )
	{
		if ( g_pTouchEvents[ i ].m_bActive && g_pTouchEvents[ i ].m_bReleased ) g_pTouchEvents[ i ].Reset();
	}
}

cTouch* cTouch::NewPoint( uint32_t iNewID )
{
	if ( iNewID == 0 ) iNewID = 0x0fffffff;
	cTouch *pFound = GetEvent( iNewID );

	if ( pFound )
	{
		pFound->Reset();
	}
	else
	{
		for ( int i = 0; i < MAX_AGK_TOUCH_COUNT; i++ )
		{
			if ( !g_pTouchEvents[ i ].m_bActive ) pFound = (g_pTouchEvents + i);
		}
	}

	if ( pFound ) 
	{
		pFound->m_iUniqueID = iNewID;
		pFound->m_bActive = true;
	}
	return pFound;
}

cTouch* cTouch::GetEvent( uint32_t iIndex )
{
	if ( iIndex == 0 ) iIndex = 0x0fffffff;

	// search array
	for ( int i = 0; i < MAX_AGK_TOUCH_COUNT; i++ )
	{
		if ( g_pTouchEvents[ i ].m_bActive && g_pTouchEvents[ i ].m_iUniqueID == iIndex ) 
		{
			return (g_pTouchEvents + i);
		}
	}

	return 0;
}

void cTouch::ClearAll()
{
	for ( int i = 0; i < MAX_AGK_TOUCH_COUNT; i++ )
	{
		g_pTouchEvents[ i ].Reset();
	}
}
