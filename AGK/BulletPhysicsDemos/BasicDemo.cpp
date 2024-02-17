

#include "BasicDemo.h"
#include "DemoUtil.h"
using namespace AGK;
int agkLogo;
int bulletLogo;
int wallPlaneID;
int floorPlaneID;
int kinematicBox;
app App;

void app::Begin(void)
{
	agk::SetWindowTitle( "Bullet Physics for AGK Basic Demo" );
	agk::SetVirtualResolution ( 1280, 800 );
	agk::SetClearColor( 0,170,204 ); // light blue
	agk::SetSyncRate( 60, 0 );
	agk::SetScissor( 0, 0, 0, 0 );
	agk::SetOrientationAllowed( 0, 0,1, 0 );

	agk::SetCameraRange( 1, 1, 50000 );

	//For debugging to throw errors.
	agk::SetPrintSize( 22.0 );
	agk::SetPrintColor( 255, 255, 0 );

	agk::SetSunDirection( 0, -1, 0 );
	agk::CreatePointLight( 1, 0, 400, 300, 250000, 255, 255, 255 );

	DemoUtil::Setup();
	//3D Physics code
	agk::Create3DPhysicsWorld();

	agkLogo = agk::LoadImage("/media/agk2_logokit/Made-With-AGK-128px.png");
	agk::SetImageWrapU(agkLogo,1);
	agk::SetImageWrapV(agkLogo,1);
	int agkSprite = agk::CreateSprite( agkLogo );
	agk::SetSpritePosition( agkSprite, agk::GetScreenBoundsRight() -128.0, agk::GetScreenBoundsTop() );

	bulletLogo = agk::LoadImage("/media/agk2_logokit/bullet_Logo_small.png");
	agk::SetImageWrapU(bulletLogo,1);
	agk::SetImageWrapV(bulletLogo,1);
	int bulletSprite = agk::CreateSprite( bulletLogo );
	agk::SetSpritePosition( bulletSprite, agk::GetScreenBoundsRight() -310.0, agk::GetScreenBoundsTop() );

	floorTex = agk::LoadImage("/media/steelPanelTiled.png");
	agk::SetImageWrapU(floorTex,1);
	agk::SetImageWrapV(floorTex,1);

	dumpsterTex = agk::LoadImage("/media/dumpster_D.png");
	agk::SetImageWrapU(dumpsterTex,1);
	agk::SetImageWrapV(dumpsterTex,1);

	crateTex = agk::LoadImage("/media/Crate_color.bmp");
	agk::SetImageWrapU(crateTex,1);
	agk::SetImageWrapV(crateTex,1);

	int backWall = agk::CreateObjectPlane( 1000, 1000 );
	agk::SetObjectImage( backWall, floorTex, 0 );
	agk::SetObjectPosition( backWall, 0, 0, -300.0 );

	//3D Physics code
	wallPlaneID = agk::Create3DPhysicsStaticPlane( 0, 1, 0, 0);
	agk::Set3DPhysicsStaticPlanePosition( wallPlaneID, 0, 0, -300.0 );
	agk::Set3DPhysicsStaticPlaneRotation( wallPlaneID, 90.0, 0.0, 0.0 );

	int floor = agk::CreateObjectPlane( 1000, 1000 );
	agk::SetObjectImage( floor, floorTex, 0 );
	agk::SetObjectRotation( floor, 90, 0, 0 );
	//3D Physics code
	 floorPlaneID = agk::Create3DPhysicsStaticPlane( 0, 1, 0, 0 );
	//agk::SetObject3DPhysicsFriction( floor, 0.4 );
	//agk::SetObject3DPhysicsRollingFriction( floor, 0.02 );

	int polyDumpster = agk::LoadObject( "/media/dumpster_3yardOnFloor.x", 150 );
	agk::SetObjectLightMode( polyDumpster, 1 );
	agk::SetObjectImage( polyDumpster, dumpsterTex, 0 );
	agk::SetObjectPosition( polyDumpster, -150, 0, 0 );
	agk::SetObjectRotation( polyDumpster, 0, 90, 0 );
	//3D Physics code
	agk::Create3DPhysicsStaticBody( polyDumpster );

	kinematicBox = agk::CreateObjectBox(450, 2, 64 );
	agk::SetObjectImage( kinematicBox, crateTex,0 );
	agk::SetObjectPosition( kinematicBox, 0, 150, 50);
	agk::Create3DPhysicsKinematicBody( kinematicBox );

	DemoUtil::CreateBoxStack( 32.0, 4, 14 );

	int testBox = agk::CreateObjectBox(50, 50, 50 );
	agk::SetObjectPosition( testBox, 0, 70, 0 );
	agk::Create3DPhysicsDynamicBody( testBox );
	int instanceObj = agk::InstanceObject( testBox );
	agk::SetObjectPosition( instanceObj,70, 70, 0 );
	//agk::Create3DPhysicsDynamicBody( instanceObj );
	
	agk::SetCameraPosition(1, 0, 200, 500);
	agk::SetCameraLookAt(1, 0, 120, 0, 0.0);
	
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

	if ( agk::GetKeyboardExists() )
	{
		if ( agk::GetRawKeyState( 37 ) )
		{
			agk::MoveObjectLocalX( kinematicBox, 1.5 ) ;
		}
		if ( agk::GetRawKeyState( 39 ) )
		{
			agk::MoveObjectLocalX( kinematicBox, -1.5 ) ;
		}
		if ( agk::GetRawKeyState( 38 ) )
		{
			agk::MoveObjectLocalZ( kinematicBox, -1.5 ) ;
		}
		if ( agk::GetRawKeyState( 40 ) )
		{
			agk::MoveObjectLocalZ( kinematicBox, 1.5 ) ;
		}
	}

	if ( agk::GetPointerPressed() )
	{
		DemoUtil::ThrowBall( 1000.0, 20.0 );
	}


    float joystickX = agk::GetVirtualJoystickX( 1 );
    float joystickY = agk::GetVirtualJoystickY( 1 );
	//TODO: need to limit rotation
	agk::RotateCameraLocalX( 1, joystickY /2);
	agk::RotateCameraLocalY( 1, joystickX /2);

    float rJoystickX = agk::GetVirtualJoystickX( 2 );
    float rJoystickY = agk::GetVirtualJoystickY( 2 );
	agk::MoveCameraLocalX( 1, rJoystickX );
	agk::MoveCameraLocalZ( 1, rJoystickY * -1.0 );
	
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
	agk::Print( "Arrow Keys to move kinematic platform" );

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
