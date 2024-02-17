#include "agk.h"

// interpolations 

float Tween::InterpLinear( float start, float end, float t )
{
	return start + (end-start)*t;
}

float Tween::InterpSmooth1( float start, float end, float t )
{
	t = t * t * (3.0f - 2.0f * t);
	return start + (end-start)*t;
}

float Tween::InterpSmooth2( float start, float end, float t )
{
	if ( t < 0.5f ) t = 8 * t*t*t*t;
	else t = 1.0f - (8 * (1-t)*(1-t)*(1-t)*(1-t) );

	return start + (end-start)*t;
}

float Tween::InterpEaseIn1( float start, float end, float t )
{
	t = t*t;
	return start + (end-start)*t;
}

float Tween::InterpEaseOut1( float start, float end, float t )
{
	t = 1 - (1-t)*(1-t);
	return start + (end-start)*t;
}

float Tween::InterpEaseIn2( float start, float end, float t )
{
	t = agk::Pow(256,t-1)*t;
	return start + (end-start)*t;
}

float Tween::InterpEaseOut2( float start, float end, float t )
{
	t = 1 - agk::Pow(256,-t)*(1-t);
	return start + (end-start)*t;
}

float Tween::InterpBounce( float start, float end, float t )
{
	t = (agk::Pow(2.0f,(-3.0f*t))*(1-t)) * -agk::Abs(agk::SinRad(t*PI*2.5f - PI*0.5f)) + 1.0f;
	return start + (end-start)*t;
}

float Tween::InterpOvershoot( float start, float end, float t )
{
	t = (agk::Pow(2.0f,(-5.0f*t))*(1-t)) * agk::SinRad(t*PI*3 - PI*0.5f) + 1.0f;
	return start + (end-start)*t;
}

// tweens

TweenCustom::TweenCustom ( )
{
	Reset ( );
}

void TweenCustom::Reset ( void )
{
	m_fValue1 = 0; m_fBegin1 = 0; m_fEnd1 = 0; m_iInterp1 = -1;
	m_fValue2 = 0; m_fBegin2 = 0; m_fEnd2 = 0; m_iInterp2 = -1;
	m_fValue3 = 0; m_fBegin3 = 0; m_fEnd3 = 0; m_iInterp3 = -1;
	m_fValue4 = 0; m_fBegin4 = 0; m_fEnd4 = 0; m_iInterp4 = -1;
	m_iValue1 = 0; m_iBegin1 = 0; m_iEnd1 = 0; m_iInterpI1 = -1;
	m_iValue2 = 0; m_iBegin2 = 0; m_iEnd2 = 0; m_iInterpI2 = -1;
	m_iValue3 = 0; m_iBegin3 = 0; m_iEnd3 = 0; m_iInterpI3 = -1;
	m_iValue4 = 0; m_iBegin4 = 0; m_iEnd4 = 0; m_iInterpI4 = -1;
}

TweenSprite::TweenSprite ( )
{
	Reset ( );
}

void TweenSprite::Reset ( void )
{
	m_fBeginX = 0; m_fEndX = 0; m_iInterpX = -1;
	m_fBeginY = 0; m_fEndY = 0; m_iInterpY = -1;
	m_fBeginXByOffset = 0; m_fEndXByOffset = 0; m_iInterpXByOffset = -1;
	m_fBeginYByOffset = 0; m_fEndYByOffset = 0; m_iInterpYByOffset = -1;
	m_fBeginAngle = 0; m_fEndAngle = 0; m_iInterpAngle = -1;
	m_fBeginSizeX = 0; m_fEndSizeX = 0; m_iInterpSizeX = -1;
	m_fBeginSizeY = 0; m_fEndSizeY = 0; m_iInterpSizeY = -1;
	m_iBeginRed = 0; m_iEndRed = 0; m_iInterpRed = -1;
	m_iBeginGreen = 0; m_iEndGreen = 0; m_iInterpGreen = -1;
	m_iBeginBlue = 0; m_iEndBlue = 0; m_iInterpBlue = -1;
	m_iBeginAlpha = 0; m_iEndAlpha = 0; m_iInterpAlpha = -1;
}

void TweenText::Reset ( void )
{
	m_fBeginX = 0; m_fEndX = 0; m_iInterpX = -1;
	m_fBeginY = 0; m_fEndY = 0; m_iInterpY = -1;
	m_fBeginAngle = 0; m_fEndAngle = 0; m_iInterpAngle = -1;
	m_fBeginSize = 0; m_fEndSize = 0; m_iInterpSize = -1;
	m_fBeginSpacing = 0; m_fEndSpacing = 0; m_iInterpSpacing = -1;
	m_fBeginLineSpacing = 0; m_fEndLineSpacing = 0; m_iInterpLineSpacing = -1;
	m_iBeginRed = 0; m_iEndRed = 0; m_iInterpRed = -1;
	m_iBeginGreen = 0; m_iEndGreen = 0; m_iInterpGreen = -1;
	m_iBeginBlue = 0; m_iEndBlue = 0; m_iInterpBlue = -1;
	m_iBeginAlpha = 0; m_iEndAlpha = 0; m_iInterpAlpha = -1;
}

TweenText::TweenText ( )
{
	Reset ( );
}

void TweenChar::Reset ( void )
{
	m_fBeginX = 0; m_fEndX = 0; m_iInterpX = -1;
	m_fBeginY = 0; m_fEndY = 0; m_iInterpY = -1;
	m_fBeginAngle = 0; m_fEndAngle = 0; m_iInterpAngle = -1;
	m_iBeginRed = 0; m_iEndRed = 0; m_iInterpRed = -1;
	m_iBeginGreen = 0; m_iEndGreen = 0; m_iInterpGreen = -1;
	m_iBeginBlue = 0; m_iEndBlue = 0; m_iInterpBlue = -1;
	m_iBeginAlpha = 0; m_iEndAlpha = 0; m_iInterpAlpha = -1;
}

TweenChar::TweenChar ( )
{
	Reset ( );
}

void TweenObject::Reset ( void )
{
	m_fBeginX = 0; m_fEndX = 0; m_iInterpX = -1;
	m_fBeginY = 0; m_fEndY = 0; m_iInterpY = -1;
	m_fBeginZ = 0; m_fEndZ = 0; m_iInterpZ = -1;
	m_fBeginAngleX = 0; m_fEndAngleX = 0; m_iInterpAngleX = -1;
	m_fBeginAngleY = 0; m_fEndAngleY = 0; m_iInterpAngleY = -1;
	m_fBeginAngleZ = 0; m_fEndAngleZ = 0; m_iInterpAngleZ = -1;
	m_fBeginScaleX = 0; m_fEndScaleX = 0; m_iInterpScaleX = -1;
	m_fBeginScaleY = 0; m_fEndScaleY = 0; m_iInterpScaleY = -1;
	m_fBeginScaleZ = 0; m_fEndScaleZ = 0; m_iInterpScaleZ = -1;
	m_iBeginRed = 0; m_iEndRed = 0; m_iInterpRed = -1;
	m_iBeginGreen = 0; m_iEndGreen = 0; m_iInterpGreen = -1;
	m_iBeginBlue = 0; m_iEndBlue = 0; m_iInterpBlue = -1;
	m_iBeginAlpha = 0; m_iEndAlpha = 0; m_iInterpAlpha = -1;
}

TweenObject::TweenObject ( )
{
	Reset ( );
}

void TweenCamera::Reset ( void )
{
	m_fBeginX = 0; m_fEndX = 0; m_iInterpX = -1;
	m_fBeginY = 0; m_fEndY = 0; m_iInterpY = -1;
	m_fBeginZ = 0; m_fEndZ = 0; m_iInterpZ = -1;
	m_fBeginAngleX = 0; m_fEndAngleX = 0; m_iInterpAngleX = -1;
	m_fBeginAngleY = 0; m_fEndAngleY = 0; m_iInterpAngleY = -1;
	m_fBeginAngleZ = 0; m_fEndAngleZ = 0; m_iInterpAngleZ = -1;
	m_fBeginFOV = 0; m_fEndFOV = 0; m_iInterpFOV = -1;
}

TweenCamera::TweenCamera ( )
{
	Reset ( );
}

void TweenCustom::Update( float fTime, void* target, int subtarget )
{
	if ( fTime < 0 ) fTime = 0;
	if ( fTime > m_fDuration ) fTime = m_fDuration;
	float t = fTime / m_fDuration;

	switch( m_iInterp1 )
	{
		case AGK_TWEEN_INTERP_LINEAR: m_fValue1 = Tween::InterpLinear( m_fBegin1, m_fEnd1, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: m_fValue1 = Tween::InterpSmooth1( m_fBegin1, m_fEnd1, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: m_fValue1 = Tween::InterpSmooth2( m_fBegin1, m_fEnd1, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: m_fValue1 = Tween::InterpEaseIn1( m_fBegin1, m_fEnd1, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: m_fValue1 = Tween::InterpEaseIn2( m_fBegin1, m_fEnd1, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: m_fValue1 = Tween::InterpEaseOut1( m_fBegin1, m_fEnd1, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: m_fValue1 = Tween::InterpEaseOut2( m_fBegin1, m_fEnd1, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: m_fValue1 = Tween::InterpBounce( m_fBegin1, m_fEnd1, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: m_fValue1 = Tween::InterpOvershoot( m_fBegin1, m_fEnd1, t ); break;
		default: break;
	}

	switch( m_iInterp2 )
	{
		case AGK_TWEEN_INTERP_LINEAR: m_fValue2 = Tween::InterpLinear( m_fBegin2, m_fEnd2, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: m_fValue2 = Tween::InterpSmooth1( m_fBegin2, m_fEnd2, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: m_fValue2 = Tween::InterpSmooth2( m_fBegin2, m_fEnd2, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: m_fValue2 = Tween::InterpEaseIn1( m_fBegin2, m_fEnd2, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: m_fValue2 = Tween::InterpEaseIn2( m_fBegin2, m_fEnd2, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: m_fValue2 = Tween::InterpEaseOut1( m_fBegin2, m_fEnd2, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: m_fValue2 = Tween::InterpEaseOut2( m_fBegin2, m_fEnd2, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: m_fValue2 = Tween::InterpBounce( m_fBegin2, m_fEnd2, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: m_fValue2 = Tween::InterpOvershoot( m_fBegin2, m_fEnd2, t ); break;
		default: break;
	}

	switch( m_iInterp3 )
	{
		case AGK_TWEEN_INTERP_LINEAR: m_fValue3 = Tween::InterpLinear( m_fBegin3, m_fEnd3, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: m_fValue3 = Tween::InterpSmooth1( m_fBegin3, m_fEnd3, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: m_fValue3 = Tween::InterpSmooth2( m_fBegin3, m_fEnd3, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: m_fValue3 = Tween::InterpEaseIn1( m_fBegin3, m_fEnd3, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: m_fValue3 = Tween::InterpEaseIn2( m_fBegin3, m_fEnd3, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: m_fValue3 = Tween::InterpEaseOut1( m_fBegin3, m_fEnd3, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: m_fValue3 = Tween::InterpEaseOut2( m_fBegin3, m_fEnd3, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: m_fValue3 = Tween::InterpBounce( m_fBegin3, m_fEnd3, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: m_fValue3 = Tween::InterpOvershoot( m_fBegin3, m_fEnd3, t ); break;
		default: break;
	}

	switch( m_iInterp4 )
	{
		case AGK_TWEEN_INTERP_LINEAR: m_fValue4 = Tween::InterpLinear( m_fBegin4, m_fEnd4, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: m_fValue4 = Tween::InterpSmooth1( m_fBegin4, m_fEnd4, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: m_fValue4 = Tween::InterpSmooth2( m_fBegin4, m_fEnd4, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: m_fValue4 = Tween::InterpEaseIn1( m_fBegin4, m_fEnd4, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: m_fValue4 = Tween::InterpEaseIn2( m_fBegin4, m_fEnd4, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: m_fValue4 = Tween::InterpEaseOut1( m_fBegin4, m_fEnd4, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: m_fValue4 = Tween::InterpEaseOut2( m_fBegin4, m_fEnd4, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: m_fValue4 = Tween::InterpBounce( m_fBegin4, m_fEnd4, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: m_fValue4 = Tween::InterpOvershoot( m_fBegin4, m_fEnd4, t ); break;
		default: break;
	}

	switch( m_iInterpI1 )
	{
		case AGK_TWEEN_INTERP_LINEAR: m_iValue1 = agk::Round( Tween::InterpLinear( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: m_iValue1 = agk::Round( Tween::InterpSmooth1( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: m_iValue1 = agk::Round( Tween::InterpSmooth2( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: m_iValue1 = agk::Round( Tween::InterpEaseIn1( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: m_iValue1 = agk::Round( Tween::InterpEaseIn2( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: m_iValue1 = agk::Round( Tween::InterpEaseOut1( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: m_iValue1 = agk::Round( Tween::InterpEaseOut2( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: m_iValue1 = agk::Round( Tween::InterpBounce( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: m_iValue1 = agk::Round( Tween::InterpOvershoot( (float)m_iBegin1, (float)m_iEnd1, t ) ); break;
		default: break;
	}
	
	switch( m_iInterpI2 )
	{
		case AGK_TWEEN_INTERP_LINEAR: m_iValue2 = agk::Round( Tween::InterpLinear( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: m_iValue2 = agk::Round( Tween::InterpSmooth1( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: m_iValue2 = agk::Round( Tween::InterpSmooth2( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: m_iValue2 = agk::Round( Tween::InterpEaseIn1( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: m_iValue2 = agk::Round( Tween::InterpEaseIn2( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: m_iValue2 = agk::Round( Tween::InterpEaseOut1( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: m_iValue2 = agk::Round( Tween::InterpEaseOut2( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: m_iValue2 = agk::Round( Tween::InterpBounce( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: m_iValue2 = agk::Round( Tween::InterpOvershoot( (float)m_iBegin2, (float)m_iEnd2, t ) ); break;
		default: break;
	}

	switch( m_iInterpI3 )
	{
		case AGK_TWEEN_INTERP_LINEAR: m_iValue3 = agk::Round( Tween::InterpLinear( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: m_iValue3 = agk::Round( Tween::InterpSmooth1( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: m_iValue3 = agk::Round( Tween::InterpSmooth2( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: m_iValue3 = agk::Round( Tween::InterpEaseIn1( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: m_iValue3 = agk::Round( Tween::InterpEaseIn2( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: m_iValue3 = agk::Round( Tween::InterpEaseOut1( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: m_iValue3 = agk::Round( Tween::InterpEaseOut2( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: m_iValue3 = agk::Round( Tween::InterpBounce( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: m_iValue3 = agk::Round( Tween::InterpOvershoot( (float)m_iBegin3, (float)m_iEnd3, t ) ); break;
		default: break;
	}

	switch( m_iInterpI4 )
	{
		case AGK_TWEEN_INTERP_LINEAR: m_iValue4 = agk::Round( Tween::InterpLinear( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: m_iValue4 = agk::Round( Tween::InterpSmooth1( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: m_iValue4 = agk::Round( Tween::InterpSmooth2( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: m_iValue4 = agk::Round( Tween::InterpEaseIn1( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: m_iValue4 = agk::Round( Tween::InterpEaseIn2( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: m_iValue4 = agk::Round( Tween::InterpEaseOut1( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: m_iValue4 = agk::Round( Tween::InterpEaseOut2( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: m_iValue4 = agk::Round( Tween::InterpBounce( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: m_iValue4 = agk::Round( Tween::InterpOvershoot( (float)m_iBegin4, (float)m_iEnd4, t ) ); break;
		default: break;
	}
}

void TweenSprite::Update( float fTime, void* target, int subtarget )
{
	if ( target == 0 ) return;
	cSprite *pSprite = (cSprite*) target; // trust the sender, no way to check

	if ( fTime < 0 ) fTime = 0;
	if ( fTime > m_fDuration ) fTime = m_fDuration;
	float t = fTime / m_fDuration;

	switch( m_iInterpX )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetX( Tween::InterpLinear( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetX( Tween::InterpSmooth1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetX( Tween::InterpSmooth2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetX( Tween::InterpEaseIn1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetX( Tween::InterpEaseIn2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetX( Tween::InterpEaseOut1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetX( Tween::InterpEaseOut2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetX( Tween::InterpBounce( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetX( Tween::InterpOvershoot( m_fBeginX, m_fEndX, t ) ); break;
		default: break;
	}

	switch( m_iInterpY )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetY( Tween::InterpLinear( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetY( Tween::InterpSmooth1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetY( Tween::InterpSmooth2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetY( Tween::InterpEaseIn1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetY( Tween::InterpEaseIn2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetY( Tween::InterpEaseOut1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetY( Tween::InterpEaseOut2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetY( Tween::InterpBounce( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetY( Tween::InterpOvershoot( m_fBeginY, m_fEndY, t ) ); break;
		default: break;
	}

	switch( m_iInterpXByOffset )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetPositionByOffset( Tween::InterpLinear( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetPositionByOffset( Tween::InterpSmooth1( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetPositionByOffset( Tween::InterpSmooth2( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetPositionByOffset( Tween::InterpEaseIn1( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetPositionByOffset( Tween::InterpEaseIn2( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetPositionByOffset( Tween::InterpEaseOut1( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetPositionByOffset( Tween::InterpEaseOut2( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetPositionByOffset( Tween::InterpBounce( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetPositionByOffset( Tween::InterpOvershoot( m_fBeginXByOffset, m_fEndXByOffset, t ), pSprite->GetYByOffset() ); break;
		default: break;
	}

	switch( m_iInterpYByOffset )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpLinear( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpSmooth1( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpSmooth2( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpEaseIn1( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpEaseIn2( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpEaseOut1( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpEaseOut2( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpBounce( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetPositionByOffset( pSprite->GetXByOffset(), Tween::InterpOvershoot( m_fBeginYByOffset, m_fEndYByOffset, t ) ); break;
		default: break;
	}

	switch( m_iInterpAngle )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetAngle( Tween::InterpLinear( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetAngle( Tween::InterpSmooth1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetAngle( Tween::InterpSmooth2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetAngle( Tween::InterpEaseIn1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetAngle( Tween::InterpEaseIn2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetAngle( Tween::InterpEaseOut1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetAngle( Tween::InterpEaseOut2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetAngle( Tween::InterpBounce( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetAngle( Tween::InterpOvershoot( m_fBeginAngle, m_fEndAngle, t ) ); break;
		default: break;
	}

	switch( m_iInterpSizeX )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetSize( Tween::InterpLinear( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetSize( Tween::InterpSmooth1( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetSize( Tween::InterpSmooth2( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetSize( Tween::InterpEaseIn1( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetSize( Tween::InterpEaseIn2( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetSize( Tween::InterpEaseOut1( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetSize( Tween::InterpEaseOut2( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetSize( Tween::InterpBounce( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetSize( Tween::InterpOvershoot( m_fBeginSizeX, m_fEndSizeX, t ), pSprite->GetHeight() ); break;
		default: break;
	}

	switch( m_iInterpSizeY )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpLinear( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpSmooth1( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpSmooth2( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpEaseIn1( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpEaseIn2( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpEaseOut1( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpEaseOut2( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpBounce( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetSize( pSprite->GetWidth(), Tween::InterpOvershoot( m_fBeginSizeY, m_fEndSizeY, t ) ); break;
		default: break;
	}

	switch( m_iInterpRed )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetRed( agk::Round( Tween::InterpLinear( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetRed( agk::Round( Tween::InterpSmooth1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetRed( agk::Round( Tween::InterpSmooth2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetRed( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetRed( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetRed( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetRed( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetRed( agk::Round( Tween::InterpBounce( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetRed( agk::Round( Tween::InterpOvershoot( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		default: break;
	}
	
	switch( m_iInterpGreen )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetGreen( agk::Round( Tween::InterpLinear( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetGreen( agk::Round( Tween::InterpSmooth1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetGreen( agk::Round( Tween::InterpSmooth2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetGreen( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetGreen( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetGreen( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetGreen( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetGreen( agk::Round( Tween::InterpBounce( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetGreen( agk::Round( Tween::InterpOvershoot( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpBlue )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetBlue( agk::Round( Tween::InterpLinear( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetBlue( agk::Round( Tween::InterpSmooth1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetBlue( agk::Round( Tween::InterpSmooth2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetBlue( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetBlue( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetBlue( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetBlue( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetBlue( agk::Round( Tween::InterpBounce( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetBlue( agk::Round( Tween::InterpOvershoot( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpAlpha )
	{
		case AGK_TWEEN_INTERP_LINEAR: pSprite->SetAlpha( agk::Round( Tween::InterpLinear( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pSprite->SetAlpha( agk::Round( Tween::InterpSmooth1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pSprite->SetAlpha( agk::Round( Tween::InterpSmooth2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pSprite->SetAlpha( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pSprite->SetAlpha( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pSprite->SetAlpha( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pSprite->SetAlpha( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pSprite->SetAlpha( agk::Round( Tween::InterpBounce( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pSprite->SetAlpha( agk::Round( Tween::InterpOvershoot( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		default: break;
	}
}

void TweenText::Update( float fTime, void* target, int subtarget )
{
	if ( target == 0 ) return;
	cText *pText = (cText*) target; // trust the sender, no way to check

	if ( fTime < 0 ) fTime = 0;
	if ( fTime > m_fDuration ) fTime = m_fDuration;
	float t = fTime / m_fDuration;

	switch( m_iInterpX )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetX( Tween::InterpLinear( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetX( Tween::InterpSmooth1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetX( Tween::InterpSmooth2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetX( Tween::InterpEaseIn1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetX( Tween::InterpEaseIn2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetX( Tween::InterpEaseOut1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetX( Tween::InterpEaseOut2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetX( Tween::InterpBounce( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetX( Tween::InterpOvershoot( m_fBeginX, m_fEndX, t ) ); break;
		default: break;
	}

	switch( m_iInterpY )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetY( Tween::InterpLinear( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetY( Tween::InterpSmooth1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetY( Tween::InterpSmooth2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetY( Tween::InterpEaseIn1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetY( Tween::InterpEaseIn2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetY( Tween::InterpEaseOut1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetY( Tween::InterpEaseOut2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetY( Tween::InterpBounce( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetY( Tween::InterpOvershoot( m_fBeginY, m_fEndY, t ) ); break;
		default: break;
	}

	switch( m_iInterpAngle )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetAngle( Tween::InterpLinear( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetAngle( Tween::InterpSmooth1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetAngle( Tween::InterpSmooth2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetAngle( Tween::InterpEaseIn1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetAngle( Tween::InterpEaseIn2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetAngle( Tween::InterpEaseOut1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetAngle( Tween::InterpEaseOut2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetAngle( Tween::InterpBounce( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetAngle( Tween::InterpOvershoot( m_fBeginAngle, m_fEndAngle, t ) ); break;
		default: break;
	}

	switch( m_iInterpSize )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetSize( Tween::InterpLinear( m_fBeginSize, m_fEndSize, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetSize( Tween::InterpSmooth1( m_fBeginSize, m_fEndSize, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetSize( Tween::InterpSmooth2( m_fBeginSize, m_fEndSize, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetSize( Tween::InterpEaseIn1( m_fBeginSize, m_fEndSize, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetSize( Tween::InterpEaseIn2( m_fBeginSize, m_fEndSize, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetSize( Tween::InterpEaseOut1( m_fBeginSize, m_fEndSize, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetSize( Tween::InterpEaseOut2( m_fBeginSize, m_fEndSize, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetSize( Tween::InterpBounce( m_fBeginSize, m_fEndSize, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetSize( Tween::InterpOvershoot( m_fBeginSize, m_fEndSize, t ) ); break;
		default: break;
	}

	switch( m_iInterpSpacing )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetSpacing( Tween::InterpLinear( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetSpacing( Tween::InterpSmooth1( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetSpacing( Tween::InterpSmooth2( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetSpacing( Tween::InterpEaseIn1( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetSpacing( Tween::InterpEaseIn2( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetSpacing( Tween::InterpEaseOut1( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetSpacing( Tween::InterpEaseOut2( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetSpacing( Tween::InterpBounce( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetSpacing( Tween::InterpOvershoot( m_fBeginSpacing, m_fEndSpacing, t ) ); break;
		default: break;
	}

	switch( m_iInterpLineSpacing )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetLineSpacing( Tween::InterpLinear( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetLineSpacing( Tween::InterpSmooth1( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetLineSpacing( Tween::InterpSmooth2( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetLineSpacing( Tween::InterpEaseIn1( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetLineSpacing( Tween::InterpEaseIn2( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetLineSpacing( Tween::InterpEaseOut1( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetLineSpacing( Tween::InterpEaseOut2( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetLineSpacing( Tween::InterpBounce( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetLineSpacing( Tween::InterpOvershoot( m_fBeginLineSpacing, m_fEndLineSpacing, t ) ); break;
		default: break;
	}

	switch( m_iInterpRed )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetRed( agk::Round( Tween::InterpLinear( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetRed( agk::Round( Tween::InterpSmooth1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetRed( agk::Round( Tween::InterpSmooth2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetRed( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetRed( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetRed( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetRed( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetRed( agk::Round( Tween::InterpBounce( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetRed( agk::Round( Tween::InterpOvershoot( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpGreen )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetGreen( agk::Round( Tween::InterpLinear( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetGreen( agk::Round( Tween::InterpSmooth1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetGreen( agk::Round( Tween::InterpSmooth2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetGreen( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetGreen( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetGreen( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetGreen( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetGreen( agk::Round( Tween::InterpBounce( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetGreen( agk::Round( Tween::InterpOvershoot( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpBlue )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetBlue( agk::Round( Tween::InterpLinear( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetBlue( agk::Round( Tween::InterpSmooth1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetBlue( agk::Round( Tween::InterpSmooth2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetBlue( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetBlue( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetBlue( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetBlue( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetBlue( agk::Round( Tween::InterpBounce( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetBlue( agk::Round( Tween::InterpOvershoot( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpAlpha )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetAlpha( agk::Round( Tween::InterpLinear( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetAlpha( agk::Round( Tween::InterpSmooth1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetAlpha( agk::Round( Tween::InterpSmooth2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetAlpha( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetAlpha( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetAlpha( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetAlpha( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetAlpha( agk::Round( Tween::InterpBounce( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetAlpha( agk::Round( Tween::InterpOvershoot( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		default: break;
	}
}

void TweenChar::Update( float fTime, void* target, int subtarget )
{
	if ( target == 0 ) return;
	cText *pText = (cText*) target; // trust the sender, no way to check

	if ( fTime < 0 ) fTime = 0;
	if ( fTime > m_fDuration ) fTime = m_fDuration;
	float t = fTime / m_fDuration;

	switch( m_iInterpX )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetCharX( subtarget, Tween::InterpLinear( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetCharX( subtarget, Tween::InterpSmooth1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetCharX( subtarget, Tween::InterpSmooth2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetCharX( subtarget, Tween::InterpEaseIn1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetCharX( subtarget, Tween::InterpEaseIn2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetCharX( subtarget, Tween::InterpEaseOut1( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetCharX( subtarget, Tween::InterpEaseOut2( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetCharX( subtarget, Tween::InterpBounce( m_fBeginX, m_fEndX, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetCharX( subtarget, Tween::InterpOvershoot( m_fBeginX, m_fEndX, t ) ); break;
		default: break;
	}

	switch( m_iInterpY )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetCharY( subtarget, Tween::InterpLinear( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetCharY( subtarget, Tween::InterpSmooth1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetCharY( subtarget, Tween::InterpSmooth2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetCharY( subtarget, Tween::InterpEaseIn1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetCharY( subtarget, Tween::InterpEaseIn2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetCharY( subtarget, Tween::InterpEaseOut1( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetCharY( subtarget, Tween::InterpEaseOut2( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetCharY( subtarget, Tween::InterpBounce( m_fBeginY, m_fEndY, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetCharY( subtarget, Tween::InterpOvershoot( m_fBeginY, m_fEndY, t ) ); break;
		default: break;
	}

	switch( m_iInterpAngle )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetCharAngle( subtarget, Tween::InterpLinear( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetCharAngle( subtarget, Tween::InterpSmooth1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetCharAngle( subtarget, Tween::InterpSmooth2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetCharAngle( subtarget, Tween::InterpEaseIn1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetCharAngle( subtarget, Tween::InterpEaseIn2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetCharAngle( subtarget, Tween::InterpEaseOut1( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetCharAngle( subtarget, Tween::InterpEaseOut2( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetCharAngle( subtarget, Tween::InterpBounce( m_fBeginAngle, m_fEndAngle, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetCharAngle( subtarget, Tween::InterpOvershoot( m_fBeginAngle, m_fEndAngle, t ) ); break;
		default: break;
	}

	switch( m_iInterpRed )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetCharRed( subtarget, agk::Round( Tween::InterpLinear( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetCharRed( subtarget, agk::Round( Tween::InterpSmooth1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetCharRed( subtarget, agk::Round( Tween::InterpSmooth2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetCharRed( subtarget, agk::Round( Tween::InterpEaseIn1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetCharRed( subtarget, agk::Round( Tween::InterpEaseIn2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetCharRed( subtarget, agk::Round( Tween::InterpEaseOut1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetCharRed( subtarget, agk::Round( Tween::InterpEaseOut2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetCharRed( subtarget, agk::Round( Tween::InterpBounce( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetCharRed( subtarget, agk::Round( Tween::InterpOvershoot( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpGreen )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpLinear( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpSmooth1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpSmooth2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpEaseIn1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpEaseIn2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpEaseOut1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpEaseOut2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpBounce( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetCharGreen( subtarget, agk::Round( Tween::InterpOvershoot( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpBlue )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpLinear( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpSmooth1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpSmooth2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpEaseIn1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpEaseIn2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpEaseOut1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpEaseOut2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpBounce( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetCharBlue( subtarget, agk::Round( Tween::InterpOvershoot( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpAlpha )
	{
		case AGK_TWEEN_INTERP_LINEAR: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpLinear( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpSmooth1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpSmooth2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpEaseIn1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpEaseIn2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpEaseOut1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpEaseOut2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpBounce( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pText->SetCharAlpha( subtarget, agk::Round( Tween::InterpOvershoot( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		default: break;
	}
}

void TweenObject::Update( float fTime, void* target, int subtarget )
{
	if ( target == 0 ) return;
	cObject3D *pObject = (cObject3D*) target; // trust the sender, no way to check

	if ( fTime < 0 ) fTime = 0;
	if ( fTime > m_fDuration ) fTime = m_fDuration;
	float t = fTime / m_fDuration;

	float newX = pObject->GetX();
	float newY = pObject->GetY();
	float newZ = pObject->GetZ();

	float newAngX = pObject->GetEulerX();
	float newAngY = pObject->GetEulerY();
	float newAngZ = pObject->GetEulerZ();

	float newScaleX = pObject->GetScaleX();
	float newScaleY = pObject->GetScaleY();
	float newScaleZ = pObject->GetScaleZ();

	switch( m_iInterpX )
	{
		case AGK_TWEEN_INTERP_LINEAR: newX = Tween::InterpLinear( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newX = Tween::InterpSmooth1( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newX = Tween::InterpSmooth2( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newX = Tween::InterpEaseIn1( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newX = Tween::InterpEaseIn2( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newX = Tween::InterpEaseOut1( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newX = Tween::InterpEaseOut2( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newX = Tween::InterpBounce( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newX = Tween::InterpOvershoot( m_fBeginX, m_fEndX, t ); break;
		default: break;
	}

	switch( m_iInterpY )
	{
		case AGK_TWEEN_INTERP_LINEAR: newY = Tween::InterpLinear( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newY = Tween::InterpSmooth1( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newY = Tween::InterpSmooth2( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newY = Tween::InterpEaseIn1( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newY = Tween::InterpEaseIn2( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newY = Tween::InterpEaseOut1( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newY = Tween::InterpEaseOut2( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newY = Tween::InterpBounce( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newY = Tween::InterpOvershoot( m_fBeginY, m_fEndY, t ); break;
		default: break;
	}

	switch( m_iInterpZ )
	{
		case AGK_TWEEN_INTERP_LINEAR: newZ = Tween::InterpLinear( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newZ = Tween::InterpSmooth1( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newZ = Tween::InterpSmooth2( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newZ = Tween::InterpEaseIn1( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newZ = Tween::InterpEaseIn2( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newZ = Tween::InterpEaseOut1( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newZ = Tween::InterpEaseOut2( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newZ = Tween::InterpBounce( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newZ = Tween::InterpOvershoot( m_fBeginZ, m_fEndZ, t ); break;
		default: break;
	}

	switch( m_iInterpAngleX )
	{
		case AGK_TWEEN_INTERP_LINEAR: newAngX = Tween::InterpLinear( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newAngX = Tween::InterpSmooth1( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newAngX = Tween::InterpSmooth2( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newAngX = Tween::InterpEaseIn1( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newAngX = Tween::InterpEaseIn2( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newAngX = Tween::InterpEaseOut1( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newAngX = Tween::InterpEaseOut2( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newAngX = Tween::InterpBounce( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newAngX = Tween::InterpOvershoot( m_fBeginAngleX, m_fEndAngleX, t ); break;
		default: break;
	}

	switch( m_iInterpAngleY )
	{
		case AGK_TWEEN_INTERP_LINEAR: newAngY = Tween::InterpLinear( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newAngY = Tween::InterpSmooth1( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newAngY = Tween::InterpSmooth2( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newAngY = Tween::InterpEaseIn1( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newAngY = Tween::InterpEaseIn2( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newAngY = Tween::InterpEaseOut1( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newAngY = Tween::InterpEaseOut2( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newAngY = Tween::InterpBounce( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newAngY = Tween::InterpOvershoot( m_fBeginAngleY, m_fEndAngleY, t ); break;
		default: break;
	}

	switch( m_iInterpAngleZ )
	{
		case AGK_TWEEN_INTERP_LINEAR: newAngZ = Tween::InterpLinear( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newAngZ = Tween::InterpSmooth1( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newAngZ = Tween::InterpSmooth2( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newAngZ = Tween::InterpEaseIn1( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newAngZ = Tween::InterpEaseIn2( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newAngZ = Tween::InterpEaseOut1( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newAngZ = Tween::InterpEaseOut2( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newAngZ = Tween::InterpBounce( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newAngZ = Tween::InterpOvershoot( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		default: break;
	}

	switch( m_iInterpScaleX )
	{
		case AGK_TWEEN_INTERP_LINEAR: newScaleX = Tween::InterpLinear( m_fBeginScaleX, m_fEndScaleX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newScaleX = Tween::InterpSmooth1( m_fBeginScaleX, m_fEndScaleX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newScaleX = Tween::InterpSmooth2( m_fBeginScaleX, m_fEndScaleX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newScaleX = Tween::InterpEaseIn1( m_fBeginScaleX, m_fEndScaleX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newScaleX = Tween::InterpEaseIn2( m_fBeginScaleX, m_fEndScaleX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newScaleX = Tween::InterpEaseOut1( m_fBeginScaleX, m_fEndScaleX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newScaleX = Tween::InterpEaseOut2( m_fBeginScaleX, m_fEndScaleX, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newScaleX = Tween::InterpBounce( m_fBeginScaleX, m_fEndScaleX, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newScaleX = Tween::InterpOvershoot( m_fBeginScaleX, m_fEndScaleX, t ); break;
		default: break;
	}

	switch( m_iInterpScaleY )
	{
		case AGK_TWEEN_INTERP_LINEAR: newScaleY = Tween::InterpLinear( m_fBeginScaleY, m_fEndScaleY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newScaleY = Tween::InterpSmooth1( m_fBeginScaleY, m_fEndScaleY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newScaleY = Tween::InterpSmooth2( m_fBeginScaleY, m_fEndScaleY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newScaleY = Tween::InterpEaseIn1( m_fBeginScaleY, m_fEndScaleY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newScaleY = Tween::InterpEaseIn2( m_fBeginScaleY, m_fEndScaleY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newScaleY = Tween::InterpEaseOut1( m_fBeginScaleY, m_fEndScaleY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newScaleY = Tween::InterpEaseOut2( m_fBeginScaleY, m_fEndScaleY, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newScaleY = Tween::InterpBounce( m_fBeginScaleY, m_fEndScaleY, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newScaleY = Tween::InterpOvershoot( m_fBeginScaleY, m_fEndScaleY, t ); break;
		default: break;
	}

	switch( m_iInterpScaleZ )
	{
		case AGK_TWEEN_INTERP_LINEAR: newScaleZ = Tween::InterpLinear( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newScaleZ = Tween::InterpSmooth1( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newScaleZ = Tween::InterpSmooth2( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newScaleZ = Tween::InterpEaseIn1( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newScaleZ = Tween::InterpEaseIn2( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newScaleZ = Tween::InterpEaseOut1( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newScaleZ = Tween::InterpEaseOut2( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newScaleZ = Tween::InterpBounce( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newScaleZ = Tween::InterpOvershoot( m_fBeginScaleZ, m_fEndScaleZ, t ); break;
		default: break;
	}

	switch( m_iInterpRed )
	{
		case AGK_TWEEN_INTERP_LINEAR: pObject->SetRed( agk::Round( Tween::InterpLinear( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pObject->SetRed( agk::Round( Tween::InterpSmooth1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pObject->SetRed( agk::Round( Tween::InterpSmooth2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pObject->SetRed( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pObject->SetRed( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pObject->SetRed( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pObject->SetRed( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pObject->SetRed( agk::Round( Tween::InterpBounce( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pObject->SetRed( agk::Round( Tween::InterpOvershoot( (float)m_iBeginRed, (float)m_iEndRed, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpGreen )
	{
		case AGK_TWEEN_INTERP_LINEAR: pObject->SetGreen( agk::Round( Tween::InterpLinear( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pObject->SetGreen( agk::Round( Tween::InterpSmooth1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pObject->SetGreen( agk::Round( Tween::InterpSmooth2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pObject->SetGreen( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pObject->SetGreen( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pObject->SetGreen( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pObject->SetGreen( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pObject->SetGreen( agk::Round( Tween::InterpBounce( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pObject->SetGreen( agk::Round( Tween::InterpOvershoot( (float)m_iBeginGreen, (float)m_iEndGreen, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpBlue )
	{
		case AGK_TWEEN_INTERP_LINEAR: pObject->SetBlue( agk::Round( Tween::InterpLinear( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pObject->SetBlue( agk::Round( Tween::InterpSmooth1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pObject->SetBlue( agk::Round( Tween::InterpSmooth2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pObject->SetBlue( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pObject->SetBlue( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pObject->SetBlue( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pObject->SetBlue( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pObject->SetBlue( agk::Round( Tween::InterpBounce( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pObject->SetBlue( agk::Round( Tween::InterpOvershoot( (float)m_iBeginBlue, (float)m_iEndBlue, t ) ) ); break;
		default: break;
	}

	switch( m_iInterpAlpha )
	{
		case AGK_TWEEN_INTERP_LINEAR: pObject->SetAlpha( agk::Round( Tween::InterpLinear( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pObject->SetAlpha( agk::Round( Tween::InterpSmooth1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pObject->SetAlpha( agk::Round( Tween::InterpSmooth2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pObject->SetAlpha( agk::Round( Tween::InterpEaseIn1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pObject->SetAlpha( agk::Round( Tween::InterpEaseIn2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pObject->SetAlpha( agk::Round( Tween::InterpEaseOut1( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pObject->SetAlpha( agk::Round( Tween::InterpEaseOut2( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pObject->SetAlpha( agk::Round( Tween::InterpBounce( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pObject->SetAlpha( agk::Round( Tween::InterpOvershoot( (float)m_iBeginAlpha, (float)m_iEndAlpha, t ) ) ); break;
		default: break;
	}

	pObject->SetPosition( newX, newY, newZ );
	pObject->SetRotationEuler( newAngX, newAngY, newAngZ );
	pObject->SetScale( newScaleX, newScaleY, newScaleZ );
}



void TweenCamera::Update( float fTime, void* target, int subtarget )
{
	if ( target == 0 ) return;
	cCamera *pCamera = (cCamera*) target; // trust the sender, no way to check

	if ( fTime < 0 ) fTime = 0;
	if ( fTime > m_fDuration ) fTime = m_fDuration;
	float t = fTime / m_fDuration;

	float newX = pCamera->GetX();
	float newY = pCamera->GetY();
	float newZ = pCamera->GetZ();

	float newAngX = pCamera->GetEulerX();
	float newAngY = pCamera->GetEulerY();
	float newAngZ = pCamera->GetEulerZ();

	switch( m_iInterpX )
	{
		case AGK_TWEEN_INTERP_LINEAR: newX = Tween::InterpLinear( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newX = Tween::InterpSmooth1( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newX = Tween::InterpSmooth2( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newX = Tween::InterpEaseIn1( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newX = Tween::InterpEaseIn2( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newX = Tween::InterpEaseOut1( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newX = Tween::InterpEaseOut2( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newX = Tween::InterpBounce( m_fBeginX, m_fEndX, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newX = Tween::InterpOvershoot( m_fBeginX, m_fEndX, t ); break;
		default: break;
	}

	switch( m_iInterpY )
	{
		case AGK_TWEEN_INTERP_LINEAR: newY = Tween::InterpLinear( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newY = Tween::InterpSmooth1( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newY = Tween::InterpSmooth2( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newY = Tween::InterpEaseIn1( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newY = Tween::InterpEaseIn2( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newY = Tween::InterpEaseOut1( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newY = Tween::InterpEaseOut2( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newY = Tween::InterpBounce( m_fBeginY, m_fEndY, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newY = Tween::InterpOvershoot( m_fBeginY, m_fEndY, t ); break;
		default: break;
	}

	switch( m_iInterpZ )
	{
		case AGK_TWEEN_INTERP_LINEAR: newZ = Tween::InterpLinear( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newZ = Tween::InterpSmooth1( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newZ = Tween::InterpSmooth2( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newZ = Tween::InterpEaseIn1( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newZ = Tween::InterpEaseIn2( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newZ = Tween::InterpEaseOut1( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newZ = Tween::InterpEaseOut2( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newZ = Tween::InterpBounce( m_fBeginZ, m_fEndZ, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newZ = Tween::InterpOvershoot( m_fBeginZ, m_fEndZ, t ); break;
		default: break;
	}

	switch( m_iInterpAngleX )
	{
		case AGK_TWEEN_INTERP_LINEAR: newAngX = Tween::InterpLinear( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newAngX = Tween::InterpSmooth1( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newAngX = Tween::InterpSmooth2( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newAngX = Tween::InterpEaseIn1( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newAngX = Tween::InterpEaseIn2( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newAngX = Tween::InterpEaseOut1( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newAngX = Tween::InterpEaseOut2( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newAngX = Tween::InterpBounce( m_fBeginAngleX, m_fEndAngleX, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newAngX = Tween::InterpOvershoot( m_fBeginAngleX, m_fEndAngleX, t ); break;
		default: break;
	}

	switch( m_iInterpAngleY )
	{
		case AGK_TWEEN_INTERP_LINEAR: newAngY = Tween::InterpLinear( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newAngY = Tween::InterpSmooth1( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newAngY = Tween::InterpSmooth2( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newAngY = Tween::InterpEaseIn1( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newAngY = Tween::InterpEaseIn2( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newAngY = Tween::InterpEaseOut1( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newAngY = Tween::InterpEaseOut2( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newAngY = Tween::InterpBounce( m_fBeginAngleY, m_fEndAngleY, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newAngY = Tween::InterpOvershoot( m_fBeginAngleY, m_fEndAngleY, t ); break;
		default: break;
	}

	switch( m_iInterpAngleZ )
	{
		case AGK_TWEEN_INTERP_LINEAR: newAngZ = Tween::InterpLinear( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: newAngZ = Tween::InterpSmooth1( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: newAngZ = Tween::InterpSmooth2( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN1: newAngZ = Tween::InterpEaseIn1( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEIN2: newAngZ = Tween::InterpEaseIn2( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: newAngZ = Tween::InterpEaseOut1( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: newAngZ = Tween::InterpEaseOut2( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_BOUNCE: newAngZ = Tween::InterpBounce( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: newAngZ = Tween::InterpOvershoot( m_fBeginAngleZ, m_fEndAngleZ, t ); break;
		default: break;
	}

	switch( m_iInterpFOV )
	{
		case AGK_TWEEN_INTERP_LINEAR: pCamera->SetFOV( Tween::InterpLinear( m_fBeginFOV, m_fEndFOV, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH1: pCamera->SetFOV( Tween::InterpSmooth1( m_fBeginFOV, m_fEndFOV, t ) ); break;
		case AGK_TWEEN_INTERP_SMOOTH2: pCamera->SetFOV( Tween::InterpSmooth2( m_fBeginFOV, m_fEndFOV, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN1: pCamera->SetFOV( Tween::InterpEaseIn1( m_fBeginFOV, m_fEndFOV, t ) ); break;
		case AGK_TWEEN_INTERP_EASEIN2: pCamera->SetFOV( Tween::InterpEaseIn2( m_fBeginFOV, m_fEndFOV, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT1: pCamera->SetFOV( Tween::InterpEaseOut1( m_fBeginFOV, m_fEndFOV, t ) ); break;
		case AGK_TWEEN_INTERP_EASEOUT2: pCamera->SetFOV( Tween::InterpEaseOut2( m_fBeginFOV, m_fEndFOV, t ) ); break;
		case AGK_TWEEN_INTERP_BOUNCE: pCamera->SetFOV( Tween::InterpBounce( m_fBeginFOV, m_fEndFOV, t ) ); break;
		case AGK_TWEEN_INTERP_OVERSHOOT: pCamera->SetFOV( Tween::InterpOvershoot( m_fBeginFOV, m_fEndFOV, t ) ); break;
		default: break;
	}

	pCamera->SetPosition( newX, newY, newZ );
	pCamera->SetRotationEuler( newAngX, newAngY, newAngZ );
}

// instances

TweenInstance* TweenInstance::g_pInstances = 0;

void TweenInstance::DeleteTween( Tween* tween )
{
	TweenInstance *pTween = g_pInstances;
	TweenInstance *pLast = 0;

	while ( pTween )
	{
		// find any instances using this tween
		if ( pTween->m_pTween == tween )
		{
			if ( pLast ) pLast->m_pNext = pTween->m_pNext;
			else g_pInstances = pTween->m_pNext;

			TweenInstance *pNext = pTween->m_pNext;
			delete pTween;
			pTween = pNext;
		}
		else 
		{
			pLast = pTween;
			pTween = pTween->m_pNext;
		}
	}
}

void TweenInstance::DeleteInstance( Tween* tween, void* target, int subtarget )
{
	TweenInstance *pTween = g_pInstances;
	TweenInstance *pLast = 0;

	while ( pTween )
	{
		if ( pTween->m_pTween == tween && pTween->m_pTarget == target && (subtarget<0 || pTween->m_iSubTarget == subtarget) )
		{
			if ( pLast ) pLast->m_pNext = pTween->m_pNext;
			else g_pInstances = pTween->m_pNext;

			TweenInstance *pNext = pTween->m_pNext;
			delete pTween;
			pTween = pNext;
		}
		else 
		{
			pLast = pTween;
			pTween = pTween->m_pNext;
		}
	}
}

// called when the target is being deleted
void TweenInstance::DeleteTarget( void* target )
{
	TweenInstance *pTween = g_pInstances;
	TweenInstance *pLast = 0;

	while ( pTween )
	{
		if ( pTween->m_pTarget == target )
		{
			if ( pLast ) pLast->m_pNext = pTween->m_pNext;
			else g_pInstances = pTween->m_pNext;

			TweenInstance *pNext = pTween->m_pNext;
			delete pTween;
			pTween = pNext;
		}
		else 
		{
			pLast = pTween;
			pTween = pTween->m_pNext;
		}
	}
}

void TweenInstance::ClearAll()
{
	TweenInstance *pTween = g_pInstances;
	while ( g_pInstances )
	{
		pTween = g_pInstances;
		g_pInstances = pTween->m_pNext;
		delete pTween;
	}
}

TweenInstance* TweenInstance::GetInstance( Tween* tween, void* target, int subtarget )
{
	TweenInstance *pTween = g_pInstances;

	while ( pTween )
	{
		if ( pTween->m_pTween == tween && pTween->m_pTarget == target && (subtarget<0 || pTween->m_iSubTarget == subtarget) )
		{
			return pTween;
		}

		pTween = pTween->m_pNext;
	}

	return 0;
}

TweenInstance::TweenInstance() 
{ 
	m_pTween = 0; 
	m_pNext = 0; 
	m_pTarget = 0; 
	m_iSubTarget = -1;
	m_fCurrentTime = 0; 
	m_fBeginTime = 0;
	m_fDelay = 0;
	m_iFlags = 0;
}

void TweenInstance::Update( float fTime )
{
	if ( m_iFlags & AGK_TWEEN_PAUSED ) return;

	if ( m_fDelay > 0 )
	{
		m_fDelay -= fTime;
		if ( m_fDelay > 0 ) return;
		else 
		{
			fTime = -m_fDelay;
			m_fDelay = 0;
		}
	}

	m_fCurrentTime += fTime;
	if ( m_fCurrentTime < 0 ) m_fCurrentTime = 0;
	if ( m_fCurrentTime > m_pTween->m_fDuration ) m_fCurrentTime = m_pTween->m_fDuration;

	m_pTween->Update( m_fCurrentTime, m_pTarget, m_iSubTarget );
}

void TweenInstance::UpdateFromChain( float fTime )
{
	float fNewTime = fTime - m_fDelay;
	if ( fNewTime < 0 ) return; //fNewTime = 0;
	if ( fNewTime > m_pTween->m_fDuration ) fNewTime = m_pTween->m_fDuration;

	m_pTween->Update( fNewTime, m_pTarget, m_iSubTarget );
}

// chains

TweenChain::TweenChain()
{
	m_pTweenList = 0;
	m_pLastTween = 0;
	m_pCurrTween = 0;
	m_fCurrTime = 0;
	m_fTotalTime = 0;
	m_bFlags = 0;
}

TweenChain::~TweenChain()
{
	ClearAll();
}

void TweenChain::UpdateInstanceTimes()
{
	TweenInstance *pTween = m_pTweenList;
	float fTime = 0;
	
	while ( pTween )
	{
		pTween->m_fBeginTime = fTime;
		fTime += pTween->m_pTween->m_fDuration + pTween->m_fDelay;
		pTween = pTween->m_pNext;
	}

	m_fTotalTime = fTime;
}

void TweenChain::DeleteTween( Tween* tween )
{
	if ( m_pCurrTween && m_pCurrTween->m_pTween == tween ) m_pCurrTween = 0;

	TweenInstance *pTween = m_pTweenList;
	TweenInstance *pLast = 0;

	while ( pTween )
	{
		// find any instances using this tween
		if ( pTween->m_pTween == tween )
		{
			if ( pLast ) pLast->m_pNext = pTween->m_pNext;
			else m_pTweenList = pTween->m_pNext;

			TweenInstance *pNext = pTween->m_pNext;
			delete pTween;
			pTween = pNext;
		}
		else 
		{
			pLast = pTween;
			pTween = pTween->m_pNext;
		}
	}

	m_pLastTween = pLast;

	UpdateInstanceTimes();
}

void TweenChain::ChangedTweenDuration( Tween* tween )
{
	bool bFound = false;
	TweenInstance *pTween = m_pTweenList;
	while ( pTween )
	{
		// find any instances using this tween
		if ( pTween->m_pTween == tween ) 
		{
			bFound = true;
			break;
		}
		pTween = pTween->m_pNext;
	}

	if ( bFound ) UpdateInstanceTimes();
}

void TweenChain::ClearAll()
{
	TweenInstance *pTween;
	
	while ( m_pTweenList )
	{
		pTween = m_pTweenList;
		m_pTweenList = m_pTweenList->m_pNext;
		delete pTween;
	}

	m_fTotalTime = 0;
	m_pCurrTween = 0;
	m_pLastTween = 0;
}

void TweenChain::AddTweenForCustom( Tween *pTween, float delay )
{
	TweenInstance *pInstance = new TweenInstance();
	pInstance->m_pTarget = 0;
	pInstance->m_pTween = pTween;
	pInstance->m_fDelay = delay;
	pInstance->m_pNext = 0;
	pInstance->m_fBeginTime = m_fTotalTime;
	m_fTotalTime += delay + pTween->m_fDuration;

	if ( m_pLastTween ) m_pLastTween->m_pNext = pInstance;
	else m_pTweenList = pInstance;
	m_pLastTween = pInstance;
}

void TweenChain::AddTweenForSprite( Tween *pTween, cSprite *pSprite, float delay )
{
	TweenInstance *pInstance = new TweenInstance();
	pInstance->m_pTarget = pSprite;
	pInstance->m_pTween = pTween;
	pInstance->m_fDelay = delay;
	pInstance->m_pNext = 0;
	pInstance->m_fBeginTime = m_fTotalTime;
	m_fTotalTime += delay + pTween->m_fDuration;

	if ( m_pLastTween ) m_pLastTween->m_pNext = pInstance;
	else m_pTweenList = pInstance;
	m_pLastTween = pInstance;
}

void TweenChain::AddTweenForText( Tween *pTween, cText *pText, float delay )
{
	TweenInstance *pInstance = new TweenInstance();
	pInstance->m_pTarget = pText;
	pInstance->m_pTween = pTween;
	pInstance->m_fDelay = delay;
	pInstance->m_pNext = 0;
	pInstance->m_fBeginTime = m_fTotalTime;
	m_fTotalTime += delay + pTween->m_fDuration;

	if ( m_pLastTween ) m_pLastTween->m_pNext = pInstance;
	else m_pTweenList = pInstance;
	m_pLastTween = pInstance;
}

void TweenChain::AddTweenForChar( Tween *pTween, cText *pText, int subtarget, float delay )
{
	TweenInstance *pInstance = new TweenInstance();
	pInstance->m_pTarget = pText;
	pInstance->m_iSubTarget = subtarget;
	pInstance->m_pTween = pTween;
	pInstance->m_fDelay = delay;
	pInstance->m_pNext = 0;
	pInstance->m_fBeginTime = m_fTotalTime;
	m_fTotalTime += delay + pTween->m_fDuration;

	if ( m_pLastTween ) m_pLastTween->m_pNext = pInstance;
	else m_pTweenList = pInstance;
	m_pLastTween = pInstance;
}

void TweenChain::AddTweenForObject( Tween *pTween, cObject3D *pObject, float delay )
{
	TweenInstance *pInstance = new TweenInstance();
	pInstance->m_pTarget = pObject;
	pInstance->m_pTween = pTween;
	pInstance->m_fDelay = delay;
	pInstance->m_pNext = 0;
	pInstance->m_fBeginTime = m_fTotalTime;
	m_fTotalTime += delay + pTween->m_fDuration;

	if ( m_pLastTween ) m_pLastTween->m_pNext = pInstance;
	else m_pTweenList = pInstance;
	m_pLastTween = pInstance;
}

void TweenChain::AddTweenForCamera( Tween *pTween, cCamera *pCamera, float delay )
{
	TweenInstance *pInstance = new TweenInstance();
	pInstance->m_pTarget = pCamera;
	pInstance->m_pTween = pTween;
	pInstance->m_fDelay = delay;
	pInstance->m_pNext = 0;
	pInstance->m_fBeginTime = m_fTotalTime;
	m_fTotalTime += delay + pTween->m_fDuration;

	if ( m_pLastTween ) m_pLastTween->m_pNext = pInstance;
	else m_pTweenList = pInstance;
	m_pLastTween = pInstance;
}

void TweenChain::Play()
{
	if ( IsPlaying() )
	{
		if ( IsPaused() ) m_bFlags &= ~AGK_TWEEN_CHAIN_PAUSED;
		return;
	}

	m_fCurrTime = 0;
	m_bFlags |= AGK_TWEEN_CHAIN_PLAYING;
}

void TweenChain::Pause()
{
	if ( IsPlaying() ) m_bFlags |= AGK_TWEEN_CHAIN_PAUSED;
}

void TweenChain::Stop()
{
	if ( !IsPlaying() ) return;
	m_bFlags &= ~AGK_TWEEN_CHAIN_PAUSED;
	m_bFlags &= ~AGK_TWEEN_CHAIN_PLAYING;

	m_pCurrTween = 0;
	m_fCurrTime = 0;
}

void TweenChain::SetTime( float fTime )
{
	if ( fTime < 0 ) fTime = 0;
	if ( fTime > m_fTotalTime ) fTime = m_fTotalTime;
	m_fCurrTime = fTime;
	Update( 0 );
}

int TweenChain::IsCurrentTween( Tween *pTween, void *target, int subtarget )
{
	if ( !m_pCurrTween ) return 0;
	if ( m_pCurrTween->m_pTween != pTween ) return 0;
	if ( m_pCurrTween->m_pTarget != target ) return 0;
	if ( subtarget >= 0 && m_pCurrTween->m_iSubTarget != subtarget ) return 0;
	return 1;
}

void TweenChain::Update( float fTime )
{
	if ( !IsPlaying() || IsPaused() ) return;
	if ( !m_pTweenList ) return;

	m_fCurrTime += fTime;
	if ( m_fCurrTime < 0 ) 
	{
		m_fCurrTime = 0;
		if ( fTime < 0 ) 
		{
			// force any current tween to its start position
			if ( m_pCurrTween ) m_pCurrTween->UpdateFromChain( m_pCurrTween->m_fDelay );
			m_bFlags &= ~AGK_TWEEN_CHAIN_PLAYING;
			m_pCurrTween = 0;
			return;
		}
	}
	if ( m_fCurrTime > m_fTotalTime ) 
	{
		m_fCurrTime = m_fTotalTime;
		if ( fTime > 0 ) 
		{
			// force any current tween to its end position
			if ( m_pCurrTween ) m_pCurrTween->UpdateFromChain( m_pCurrTween->m_fDelay + m_pCurrTween->m_pTween->m_fDuration );
			m_bFlags &= ~AGK_TWEEN_CHAIN_PLAYING;
			m_pCurrTween = 0;
			return;
		}
	}

	// check if a previous tween has reached the end
	if ( fTime > 0 && m_pCurrTween && m_pCurrTween->m_fBeginTime + m_pCurrTween->m_fDelay + m_pCurrTween->m_pTween->m_fDuration < m_fCurrTime )
	{
		// force it to its final position
		m_pCurrTween->UpdateFromChain( m_pCurrTween->m_fDelay + m_pCurrTween->m_pTween->m_fDuration );
	}

	// only applies if time is going backwards
	if ( fTime < 0 && m_pCurrTween && m_pCurrTween->m_fBeginTime + m_pCurrTween->m_fDelay > m_fCurrTime )
	{
		// force it to its first position
		m_pCurrTween->UpdateFromChain( m_pCurrTween->m_fDelay );
	}

	if ( !m_pCurrTween || m_pCurrTween->m_fBeginTime > m_fCurrTime ) m_pCurrTween = m_pTweenList;
	while ( m_pCurrTween->m_pNext && m_pCurrTween->m_pNext->m_fBeginTime < m_fCurrTime ) m_pCurrTween = m_pCurrTween->m_pNext;

	m_pCurrTween->UpdateFromChain( m_fCurrTime - m_pCurrTween->m_fBeginTime );
}