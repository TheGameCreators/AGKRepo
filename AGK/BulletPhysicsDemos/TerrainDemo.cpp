
#include "TerrainDemo.h"
#include "DemoUtil.h"

using namespace AGK;

int terrainID;
int Logo;
app App;

void app::Begin(void)
{
	DemoUtil::Setup();

	agk::SetWindowTitle( "Bullet Physics for AGK Terrain Demo" );
	agk::SetVirtualResolution ( 1280, 800 );
	agk::SetClearColor( 0,170,204 ); // Sky Blue
	agk::SetSyncRate( 60, 0 );
	agk::SetScissor( 0, 0, 0, 0 );
	agk::SetOrientationAllowed( 0, 0,1, 0 );

	// Mr "aliasing" ! Go away ! :)
	agk::SetGenerateMipmaps(1);

	agk::SetCameraFOV(1,80);
	agk::SetCameraRange( 1,1, 5000 );

	agk::SetPrintSize( 22.0 );
	agk::SetPrintColor( 255, 255, 0 );

	//3D Physics code
	agk::Create3DPhysicsWorld();

	Logo = agk::LoadImage("/media/agk2_logokit/Made-With-AGK-128px.png");
	agk::SetImageWrapU(Logo,1);
	agk::SetImageWrapV(Logo,1);
	int LogoSprite = agk::CreateSprite( Logo );
	agk::SetSpritePosition( LogoSprite, agk::GetScreenBoundsRight() -128.0, agk::GetScreenBoundsTop() );

	terrainID = DemoUtil::loadTerrain( "/media/Terrain/mikeheight.jpg", "/media/Terrain/mikecolor.jpg", 
												"/media/Terrain/detail.png", 0.05, 0.05, 100, 4 );
	agk::SetObjectScale( terrainID, 12, 12, 12 );
	agk::SetObjectPosition( terrainID, -1400, -100, 400 );
	agk::SetObjectLightMode( terrainID, 1 );

	//3D Physics code
	agk::Create3DPhysicsStaticBody( terrainID );
	agk::SetObject3DPhysicsFriction( terrainID, 0.4 );
	agk::SetObject3DPhysicsRollingFriction( terrainID, 0.02 );

	agk::SetCameraPosition(1, 0, 450, 400);
	agk::SetCameraLookAt(1, 0, 350, 0, 0.0);
	
	agk::AddVirtualJoystick( 1, 60.0, agk::GetScreenBoundsBottom() - 60.0, 120.0 );
	agk::AddVirtualJoystick( 2, agk::GetScreenBoundsRight() -60, agk::GetScreenBoundsBottom() - 60.0, 120.0 );
}

void app::Loop (void)
{
	if ( agk::GetRawKeyPressed( 49 ) )
	{
		DemoUtil::DropCylinder();
	}
	if ( agk::GetRawKeyPressed( 50 ) )
	{
		DemoUtil::DropCone();
	}
	if ( agk::GetRawKeyPressed( 51 ) )
	{
		DemoUtil::DropCapsule();
	}
	if ( agk::GetRawKeyPressed( 52 ) )
	{
		DemoUtil::DropConvexHull();
	}
	if ( agk::GetPointerPressed() )
	{
		DemoUtil::ThrowBall( 1000.0, 20.0 );
	}



    float rotJoystickX = agk::GetVirtualJoystickX( 1 );
    float rotJoystickY = agk::GetVirtualJoystickY( 1 );

	agk::RotateCameraGlobalX( 1, rotJoystickY /-2);
	agk::RotateCameraGlobalY( 1, rotJoystickX /2);

    float posJoystickX = agk::GetVirtualJoystickX( 2 );
    float posJoystickY = agk::GetVirtualJoystickY( 2 );
	agk::MoveCameraLocalX( 1, posJoystickX * -2.0 );
	agk::MoveCameraLocalZ( 1, posJoystickY * -2.0 );
	
	agk::Print( agk::ScreenFPS() );
	//uString results1( "GetVerticesProcessed:" );
	//results1.Append( agk::GetVerticesProcessed() );
 //   agk::Print( results1 );
	//uString results2( "GetPolygonsDrawn:" );
	//results2.Append( agk::GetPolygonsDrawn() );
 //   agk::Print( results2 );
	uString results3( "Physics Objects:" );
	results3.Append( agk::Get3DPhysicsTotalObjects() );
	agk::Print( results3);
	uString results4( "Active Physics Objects:");
	results4.Append( agk::Get3DPhysicsActiveObjects() );
	agk::Print( results4 );
	agk::Print( "Keys 1 - 4 to drop physics objects" );
	agk::Print( "Lmb to throw sphere object " );

	agk::Step3DPhysicsWorld();

	agk::Sync();
	//agk::Debug3DPhysicsWorld();
}

void app::End (void)
{
	agk::Delete3DPhysicsWorld();
	agk::DeleteAllImages();
	agk::DeleteAllObjects();
}













