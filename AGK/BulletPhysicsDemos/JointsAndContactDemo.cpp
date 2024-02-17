

#include "JointsAndContactDemo.h"
#include "DemoUtil.h"

using namespace AGK;
int agkLogo;
int bulletLogo;

int floorBox;
int kinematicBox;
int capsule5;
int polyDumpster;

app App;

void app::Begin(void)
{
	agk::SetErrorMode( 2 );
	agk::SetWindowTitle( "Bullet Physics for AGK Basic Demo" );
	agk::SetVirtualResolution ( 1280, 800 );
	agk::SetClearColor( 0,170,204 ); // light blue
	agk::SetSyncRate( 60, 0 );
	agk::SetScissor( 0, 0, 0, 0 );
	agk::SetOrientationAllowed( 0, 0,1, 0 );
	agk::SetCameraRange( 1, 1, 50000 );
	agk::SetPrintSize( 22.0 );
	agk::SetPrintColor( 255, 255, 0 );
	agk::SetSunDirection( 0, -1, 0 );
	//agk::CreatePointLight( 1, 0, 400, 300, 250000, 255, 255, 255 );


	//3D Physics code
	agk::Create3DPhysicsWorld();
	DemoUtil::Setup();

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


	kinematicBox = agk::CreateObjectBox( 100, 12, 100 );
	agk::SetObjectImage( kinematicBox, floorTex, 0 );
	agk::SetObjectPosition( kinematicBox, 0.0, 300.0, 0.0 );
	//3D Physics code
	agk::Create3DPhysicsKinematicBody( kinematicBox );

	floorBox = agk::CreateObjectBox( 1000, 12, 1000 );
	agk::SetObjectImage( floorBox, floorTex, 0 );
	//3D Physics code
	agk::Create3DPhysicsStaticBody( floorBox );



	//int capsule1 = agk::LoadObject( "/media/CapsuleSmall.x" );
	int capsule1 = agk::CreateObjectCapsule( 20, 45, 1 );
	agk::SetObjectImage( capsule1, concreteTex, 0 );
	agk::SetObjectPosition( capsule1, 0, agk::GetObjectWorldY( kinematicBox ) - 24 - 3, 0 );
	agk::Create3DPhysicsDynamicBody( capsule1 );
	agk::SetObject3DPhysicsCanSleep( capsule1, false );
	agk::SetObjectShapeCapsule( capsule1, 1 );
	int jointPosVec = agk::CreateVector3( 0.0, 297.0, 0.0 );
	int jointRotVec = agk::CreateVector3( 0.0, 1.0, 0.0 );
	int topJointID = agk::Create3DPhysicsFixedJoint( kinematicBox, capsule1, jointPosVec );

	//*****************************************
	//int capsule2 = agk::LoadObject( "/media/CapsuleSmall.x" );
	int capsule2 = agk::CreateObjectCapsule( 20, 45, 1 );
	agk::SetObjectImage( capsule2, concreteTex, 0 );
	//agk::SetObjectVisible( capsule2, 1 );
	agk::SetObjectPosition( capsule2, 0, agk::GetObjectWorldY( capsule1 ) - 45, 0 );
	agk::Create3DPhysicsDynamicBody( capsule2 );
	agk::SetObject3DPhysicsCanSleep( capsule2, false );
	agk::SetObjectShapeCapsule( capsule2, 1 );
	agk::GetObjects3DPhysicsContactPositionVector( capsule1, capsule2, jointPosVec );
	int jointOne = agk::Create3DPhysics6DOFJoint( capsule1, capsule2, jointPosVec, jointRotVec );

	//***********************************
	//int capsule3 = agk::LoadObject( "/media/CapsuleSmall.x" );
	int capsule3 = agk::CreateObjectCapsule( 20, 45, 1 );
	agk::SetObjectImage( capsule3, concreteTex, 0 );
	agk::SetObjectPosition( capsule3, 0, agk::GetObjectWorldY( capsule2 ) - 45, 0 );
	agk::Create3DPhysicsDynamicBody( capsule3 );
	agk::SetObject3DPhysicsCanSleep( capsule3, false );
	agk::SetObjectShapeCapsule( capsule3, 1 );
	agk::GetObjects3DPhysicsContactPositionVector( capsule2, capsule3, jointPosVec );
	int jointTwo = agk::Create3DPhysics6DOFJoint( capsule2, capsule3, jointPosVec, jointRotVec );

	//***********************************
	//int capsule4 = agk::LoadObject( "/media/CapsuleSmall.x" );
	int capsule4 = agk::CreateObjectCapsule( 20, 45, 1 );
	agk::SetObjectImage( capsule4, concreteTex, 0 );
	agk::SetObjectPosition( capsule4, 0, agk::GetObjectWorldY( capsule3 ) - 45, 0 );
	agk::Create3DPhysicsDynamicBody( capsule4 );
	agk::SetObject3DPhysicsCanSleep( capsule4, false );
	agk::SetObjectShapeCapsule( capsule4, 1 );
	agk::GetObjects3DPhysicsContactPositionVector( capsule3, capsule4, jointPosVec );
	int jointThree = agk::Create3DPhysics6DOFJoint( capsule3, capsule4, jointPosVec, jointRotVec );

	//***********************************
	//capsule5 = agk::LoadObject( "/media/CapsuleSmall.x" );
	capsule5 = agk::CreateObjectCapsule( 20, 45, 1 );
	agk::SetObjectImage( capsule5, concreteTex, 0 );
	agk::SetObjectPosition( capsule5, 0, agk::GetObjectWorldY( capsule4 ) - 45, 0 );
	agk::Create3DPhysicsDynamicBody( capsule5 );
	agk::SetObject3DPhysicsCanSleep( capsule5, false );
	agk::SetObjectShapeCapsule( capsule5, 1 );
	agk::GetObjects3DPhysicsContactPositionVector( capsule4, capsule5, jointPosVec );
	int jointFour = agk::Create3DPhysics6DOFJoint( capsule4, capsule5, jointPosVec, jointRotVec );


	// Hinge joint
	int jamCylinder = agk::CreateObjectCylinder( 180.0, 8.0, 12 );
	agk::SetObjectImage( jamCylinder, floorTex, 0 );
	agk::SetObjectPosition( jamCylinder, 244, 120.0, 0.0 );
	agk::Create3DPhysicsStaticBody( jamCylinder );

	int doorBox = agk::CreateObjectBox( 80.0,160.0, 6.0 );
	agk::SetObjectImage( doorBox, floorTex, 0 );
	agk::SetObjectPosition( doorBox, 200.0, 120.0, 0.0 );
	agk::Create3DPhysicsDynamicBody( doorBox );
	agk::SetObject3DPhysicsCanSleep( doorBox, false );
	
	int hingePos = agk::CreateVector3();
	int hingeRot = agk::CreateVector3( 0.0, 1.0, 0.0 );

	agk::GetObjects3DPhysicsContactPositionVector( jamCylinder, doorBox, hingePos );
	int hingeJoint = agk::Create3DPhysicsHingeJoint( jamCylinder, doorBox, hingePos, hingeRot, 1 );
	agk::Set3DPhysicsJointHingeLimits( hingeJoint, -180.0, 0.0 );

	//Cone Twist Joint
	//int pivotCapsule = agk::LoadObject( "/media/CapsuleSmall.x");
	int pivotCapsule = agk::CreateObjectCapsule( 20, 45, 1 );
	agk::SetObjectImage( pivotCapsule, concreteTex, 0 );
	agk::SetObjectPosition( pivotCapsule, 100, 200, 80 );
	agk::Create3DPhysicsStaticBody( pivotCapsule );

	int pivotCone = agk::CreateObjectCone( 50 ,20, 12 );
	agk::SetObjectImage( pivotCone, concreteTex, 0 );
	agk::SetObjectPosition( pivotCone, 100, 160, 80 );
	//agk::SetObjectRotation(  pivotCone, 0.0, 0.0, 180.0 );
	agk::Create3DPhysicsDynamicBody( pivotCone );
	agk::SetObjectShapeCone( pivotCone, 1 );
	agk::SetObject3DPhysicsCanSleep( pivotCone, false );

	int CTPos = agk::CreateVector3();
	int CTRot = agk::CreateVector3(0,1,0);

	agk::GetObjects3DPhysicsContactPositionVector( pivotCapsule, pivotCone, CTPos );
	int coneTwistJoint = agk::Create3DPhysicsConeTwistJoint( pivotCapsule, pivotCone, CTPos, CTRot, 1 );
	agk::Set3DPhysicsJointConeTwistLimits( coneTwistJoint, 45, 45, 360 );

	//Slider Joint
	int railCylinder = agk::CreateObjectCylinder( 300.0, 8.0, 12 );
	agk::SetObjectImage( railCylinder, floorTex, 0 );
	agk::SetObjectPosition( railCylinder, 0, 60.0, 140.0 );
	agk::SetObjectRotation( railCylinder, 0, 0, 90 );
	agk::Create3DPhysicsStaticBody( railCylinder );

	int sliderBox = agk::CreateObjectBox( 40, 40, 40 );
	agk::SetObjectImage( sliderBox, crateTex, 0 );
	agk::SetObjectPosition( sliderBox, 0, 60, 140.0 );
	agk::Create3DPhysicsDynamicBody( sliderBox );
	agk::SetObject3DPhysicsCanSleep( sliderBox, false );

	int slidePos = agk::CreateVector3();
	int slideRot = agk::CreateVector3(1,0,0);

	agk::GetObjects3DPhysicsContactPositionVector( railCylinder, sliderBox, slidePos );
	int sliderJoint = agk::Create3DPhysicsSliderJoint( railCylinder, sliderBox, slidePos, slideRot );

	//agk::Set3DPhysicsJointSliderAngularLimits( sliderJoint, 0.0, 45.0 );
	agk::Set3DPhysicsJointSliderLinearLimits( sliderJoint, -250.0, 250.0 );


	polyDumpster = agk::LoadObject( "/media/dumpster_3yardOnFloor.x", 150 );
	agk::SetObjectLightMode( polyDumpster, 1 );
	agk::SetObjectImage( polyDumpster, dumpsterTex, 0 );
	agk::SetObjectPosition( polyDumpster, -150, 0, 0 );
	agk::SetObjectRotation( polyDumpster, 0, 90, 0 );
	//3D Physics code
	agk::Create3DPhysicsStaticBody( polyDumpster );

	//DemoUtil::CreateBoxStack(32, 4, 14);

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

	if ( agk::GetRawMouseRightPressed() )
	{
		DemoUtil::ThrowBall( 1000.0, 20.0 );
	}

	DemoUtil::PickJointCreateAndUpdate();

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
	agk::Print( results3 );
	uString results4( "Active Physics Objects:");
	results4.Append( agk::Get3DPhysicsActiveObjects() );
	agk::Print( results4 );
	agk::Print( "Keys 1 - 4 to drop physics objects" );
	agk::Print( "Lmb to throw sphere object " );
	agk::Print( "Arrow Keys to move kinematic platform" );

	int contactObjB = -1;
	float x = 0.0f, y = 0.0f, z = 0.0f;

	if( agk::GetObject3DPhysicsFirstContact( floorBox ) )
	{
		do
		{
			contactObjB  = agk::GetObject3DPhysicsContactObjectB();
			x = agk::GetObject3DPhysicsContactX();
			y = agk::GetObject3DPhysicsContactY();
			z = agk::GetObject3DPhysicsContactZ();
	
			uString contactResults1( "First Contact Obj ID:" );
			contactResults1.Append( contactObjB );
			agk::Print( contactResults1 );

			uString contactResults2( "Contact Position:" );
			contactResults2.Append( x );
			contactResults2.Append( ", " );
			contactResults2.Append( y );
			contactResults2.Append( ", " );
			contactResults2.Append( z );
			agk::Print( contactResults2 );
		}while ( agk::GetObject3DPhysicsNextContact() );
	}

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

