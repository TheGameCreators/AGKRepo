#include "agk.h"

using namespace AGK;

cImage *AGKSkyBox::g_pGradientImage = 0;

void AGKSkyBox::BuildGradientImage()
{
	if ( g_pGradientImage ) return;

	unsigned char *data = new unsigned char[ 256*256*4 ];
	for( int y = 0; y < 256; y++ )
	{
		for( int x = 0; x < 256; x++ )
		{
			float diffX = x - 127.0f;
			float diffY = y - 127.0f;
			float dist = agk::Sqrt( diffX*diffX + diffY*diffY );
			dist *= 2.0078125f;
			if ( dist > 255 ) dist = 255;
			int color = 255 - agk::Round(dist);

			int index = y*256 + x;
			data[ index*4 + 0 ] = color;
			data[ index*4 + 1 ] = color;
			data[ index*4 + 2 ] = color;
			data[ index*4 + 3 ] = color;
		}
	}

	g_pGradientImage = new cImage();
	g_pGradientImage->LoadFromData( 256, 256, (unsigned int*)data );

	delete [] data;
}

AGKSkyBox::AGKSkyBox( int visible )
{
	if ( !g_pGradientImage ) BuildGradientImage();

	m_iFlags = 0;
	if ( visible ) m_iFlags |= AGK_SKYBOX_ACTIVE;
	
	m_pSkyObject = new cObject3D();
	m_pSkyObject->CreateSphere( 1, 50, 50 );
	m_pSkyObject->SetScale( -4000,-4000,-4000 );
	m_pSkyObject->SetLightMode( 0 );
	m_pSkyObject->SetCollisionMode( 0 );
	m_pSkyObject->SetImage( g_pGradientImage, 0 );
	m_pSkyObject->SetScreenCulling( 0 );
	m_pSkyObject->SetDepthReadMode( AGK_DEPTH_FUNC_LEQUAL );
	m_pSkyObject->SetDepthWrite( 0 );
	m_pSkyObject->SetDepthRange( 0, 1 );
	m_pSkyObject->SetShadowReceiveMode( 0 );

	m_pNoSunShader = agk::GetRenderer()->LoadDefaultShaderSkyBoxNoSun();
	m_pSunShader = agk::GetRenderer()->LoadDefaultShaderSkyBoxSun();
	
	m_pSkyObject->SetShader( m_pSunShader );

	Reset();
}

AGKSkyBox::~AGKSkyBox()
{
	if ( m_pSkyObject ) delete m_pSkyObject;
	if ( m_pNoSunShader ) delete m_pNoSunShader;
	if ( m_pSunShader ) delete m_pSunShader;
}

void AGKSkyBox::Reset()
{
	SetSkyColor( 0.63f, 0.72f, 0.82f );
	SetHorizonColor( 1.0f, 1.0f, 1.0f );
	SetSunColor( 1.0f, 0.9f, 0.7f );
	SetSunSize( 5.0f, 50.0f );
	SetHorizonSize( 4.0f, 0.0f );
	SetSunVisible( 1 );
}

void AGKSkyBox::SetActive( int active )
{
	if ( active ) m_iFlags |= AGK_SKYBOX_ACTIVE;
	else m_iFlags &= ~AGK_SKYBOX_ACTIVE;
}

void AGKSkyBox::SetSkyColor( float red, float green, float blue )
{
	m_fSkyColorR = red; m_fSkyColorG = green; m_fSkyColorB = blue;

	if ( m_pNoSunShader ) m_pNoSunShader->SetConstantByName( "skyColor", m_fSkyColorR, m_fSkyColorG, m_fSkyColorB, 0 );
	if ( m_pSunShader ) m_pSunShader->SetConstantByName( "skyColor", m_fSkyColorR, m_fSkyColorG, m_fSkyColorB, 0 );
}

void AGKSkyBox::SetHorizonColor( float red, float green, float blue )
{
	m_fHorizonColorR = red; m_fHorizonColorG = green; m_fHorizonColorB = blue;

	if ( m_pNoSunShader ) m_pNoSunShader->SetConstantByName( "horizonColor", m_fHorizonColorR, m_fHorizonColorG, m_fHorizonColorB, 0 );
	if ( m_pSunShader ) m_pSunShader->SetConstantByName( "horizonColor", m_fHorizonColorR, m_fHorizonColorG, m_fHorizonColorB, 0 );
}

void AGKSkyBox::SetSunColor( float red, float green, float blue )
{
	m_fSunColorR = red; m_fSunColorG = green; m_fSunColorB = blue;

	if ( m_pSunShader ) m_pSunShader->SetConstantByName( "sunColor", m_fSunColorR, m_fSunColorG, m_fSunColorB, 0 );
}

void AGKSkyBox::SetSunSize( float sun, float halo )
{
	if ( sun > 100 ) sun = 100;
	if ( sun < 1 ) sun = 1;
	if ( halo > 100 ) halo = 100;
	if ( halo < 1 ) halo = 1;

	m_fSunSize = sun; m_fHaloSize = halo;

	float realSunSize = 200.0f / (m_fSunSize*2.0f);
	float realHaloSize = 200.0f / (m_fHaloSize*2.0f);

	if ( m_pNoSunShader ) m_pNoSunShader->SetConstantByName( "sunSize", realSunSize, realHaloSize, 0, 0 );
	if ( m_pSunShader ) m_pSunShader->SetConstantByName( "sunSize", realSunSize, realHaloSize, 0, 0 );
}

void AGKSkyBox::SetHorizonSize( float size, float height )
{
	if ( size < 0.1f ) size = 0.1f;

	m_fHorizonSize = size;
	m_fHorizonHeight = height;

	if ( m_pNoSunShader ) 
	{
		m_pNoSunShader->SetConstantByName( "horizonSize", -24.0f / m_fHorizonSize, 0, 0, 0 );
		m_pNoSunShader->SetConstantByName( "horizonHeight", m_fHorizonHeight, 0, 0, 0 );
	}
	if ( m_pSunShader ) 
	{
		m_pSunShader->SetConstantByName( "horizonSize", -24.0f / m_fHorizonSize, 0, 0, 0 );
		m_pSunShader->SetConstantByName( "horizonHeight", m_fHorizonHeight, 0, 0, 0 );
	}
}

void AGKSkyBox::SetSunVisible( int visible )
{
	if ( visible ) 
	{
		m_iFlags |= AGK_SKYBOX_SUN_VISIBLE;
		m_pSkyObject->SetShader( m_pSunShader );
	}
	else 
	{
		m_iFlags &= ~AGK_SKYBOX_SUN_VISIBLE;
		m_pSkyObject->SetShader( m_pNoSunShader );
	}
}

void AGKSkyBox::Draw()
{
	if ( (m_iFlags & AGK_SKYBOX_ACTIVE) == 0 ) return;

	cCamera *pCamera = agk::GetCurrentCamera();
	if ( !pCamera ) return;

	float scale = -1.9f * pCamera->GetFarRange();
	m_pSkyObject->SetScale( scale,scale,scale );
	if ( m_pNoSunShader ) m_pNoSunShader->SetConstantByName( "objectScale", 1.0f / scale, 0, 0, 0 );
	if ( m_pSunShader ) m_pSunShader->SetConstantByName( "objectScale", 1.0f / scale, 0, 0, 0 );

	m_pSkyObject->SetPosition( pCamera->GetWorldX(), pCamera->GetWorldY(), pCamera->GetWorldZ() );

	if ( (m_iFlags & AGK_SKYBOX_SUN_VISIBLE) == 0 )
	{
		m_pSkyObject->SetRotationQuat( AGKQuaternion(1,0,0,0) );
	}
	else
	{
		float dirX = agk::m_cDirectionalLight.m_direction.x;
		float dirY = agk::m_cDirectionalLight.m_direction.y;
		float dirZ = agk::m_cDirectionalLight.m_direction.z;

		float dirXZ = agk::Sqrt( dirX*dirX + dirZ*dirZ );
		float angX = agk::ATanFull( dirXZ, dirY );
		float angY = agk::ATanFull( dirX, dirZ );

		m_pSkyObject->SetRotationEuler( angX, -angY, 0 );
	}

	m_pSkyObject->Draw();
}