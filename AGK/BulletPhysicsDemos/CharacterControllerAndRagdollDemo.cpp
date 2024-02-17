

#include "CharacterControllerAndRagdollDemo.h"
#include "RagdollUtil.h"
#include "DemoUtil.h"


using namespace AGK;
int agkLogo;
int bulletLogo;
bool showInstructions = true;

int floorBox;
int kinematicBox;
int capsule5;
int polyDumpster;
int maskedSoldierTex;
int maskedSoldier;
int AkioTex;
int OriginalAiko;
int FragAiko;


cObject3D* pObject;
app App;
btClock m_clock;
float maxElapsed;
float rotate = 2.5; 

void app::Begin(void)
{
	//agk::SetErrorMode( 1 );
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

	kinematicBox = agk::CreateObjectBox( 50, 12, 50 );
	agk::SetObjectImage( kinematicBox, concreteTex, 0 );
	agk::SetObjectPosition( kinematicBox, -60.0, 12.0, 0.0 );
	agk::Create3DPhysicsStaticBody( kinematicBox );
	agk::SetObject3DPhysicsGroupAndMask( kinematicBox, group0, -1 );

	int stairsObj = agk::LoadObject( "/media/Stairs.x" );
	agk::SetObjectImage( stairsObj, concreteTex, 0 );
	agk::SetObjectPosition( stairsObj, 0.0, 6.0, 200.0 );
	agk::RotateObjectGlobalY( stairsObj, 180.0 );
	agk::Create3DPhysicsStaticBody( stairsObj );
	agk::SetObject3DPhysicsGroupAndMask( stairsObj, group0, -1 );

	floorBox = agk::CreateObjectBox( 1000, 12, 1000 );
	agk::SetObjectImage( floorBox, floorTex, 0 );
	agk::SetObjectPosition( floorBox, 0, 0, 0 );
	agk::Create3DPhysicsStaticBody( floorBox );
	//agk::Create3DPhysicsDynamicBody( floorBox );
	//agk::SetObjectShapeBox( floorBox );
	//agk::SetObject3DPhysicsMass( floorBox, 0.0f );
	agk::SetObject3DPhysicsGroupAndMask( floorBox, group0, -1 );

	maskedSoldierTex = agk::LoadImage( "/media/Masked Soldier.png" );
	agk::SetImageWrapU(maskedSoldierTex,1);
	agk::SetImageWrapV(maskedSoldierTex,1);


	maskedSoldier = agk::LoadObjectWithChildren( "/media/Masked SoldierFrag.X" );
	agk::SetObjectLightMode( maskedSoldier, 1 );
	agk::SetObjectImage( maskedSoldier, maskedSoldierTex, 0 );
	//agk::SetObjectPosition( maskedSoldier, 0.0, 10.0, 0.0 );
	agk::SetObjectRotation( maskedSoldier, 0, 90, 0 );
	agk::SetObjectAnimationSpeed( maskedSoldier, 30.0 );
	agk::PlayObjectAnimation( maskedSoldier, "Animation", 3002, 3547, 1, 0.0 );

	DemoUtil::CreateCharacterController( maskedSoldier );
	//agk::Debug3DPhysicsCharacterController( maskedSoldier,1  );
	agk::Stand3DPhysicsCharacterController( maskedSoldier );
	agk::Set3DPhysicsCharacterControllerStepHeight( maskedSoldier,12.0 );
	agk::Set3DPhysicsCharacterControllerJumpSpeed( maskedSoldier, 6.0 );


	polyDumpster = agk::LoadObject( "/media/dumpster_3yardOnFloor.x" );
	agk::SetObjectLightMode( polyDumpster, 1 );
	agk::SetObjectImage( polyDumpster, dumpsterTex, 0 );
	agk::SetObjectPosition( polyDumpster, -150, 6, 0 );
	agk::SetObjectRotation( polyDumpster, 0, 180.0, 0 );
	agk::Create3DPhysicsStaticBody( polyDumpster );
	agk::SetObject3DPhysicsGroupAndMask( polyDumpster, group0, -1 );
	
	RagdollUtil::CreateRagdollVectors();

	//DemoUtil::CreateBoxStack(14, 4, 14);

	agk::SetCameraPosition( 1, 0, 120.0, -200.0 );
	agk::SetCameraLookAt( 1, 0, 0, 0, 0.0 );

	////agk::AddVirtualJoystick( 1, 60.0, agk::GetScreenBoundsBottom() - 60.0, 120.0 );
	////agk::AddVirtualJoystick( 2, agk::GetScreenBoundsRight() -60, agk::GetScreenBoundsBottom() - 60.0, 120.0 );

}

void app::Loop (void)
{
	//DemoUtil::ControlKinematicObjectWithArrowKeys( kinematicBox );
	
	DemoUtil::ControlCameraWithArrowKeys( 1 );
	DemoUtil::RotateCameraWithMouse( 1, 6.0  );

	if ( agk::GetRawMouseRightPressed() )
	{
		DemoUtil::ThrowBall( 500.0, 3.0 );
	}

	DemoUtil::PickJointCreateAndUpdate();

	//Press R to create ragdoll
	if ( agk::GetRawKeyPressed( 82 ) )
	{	
		if ( agk::Get3DPhysicsRagdollExist( maskedSoldier ) == 0)
				RagdollUtil::CreateRagdollSkelton( maskedSoldier );
		agk::Delete3DPhysicsCharacterController( maskedSoldier );
	}

	if ( agk::Get3DPhysicsCharacterControllerExists( maskedSoldier ) )
	{
		DemoUtil::CharacterControllerInput( maskedSoldier );
	}

	if ( agk::GetRawKeyPressed( 70 ) )
	{
		if ( agk::Get3DPhysicsRagdollExist( maskedSoldier ) ){
			agk::Delete3DPhysicsRagdoll( maskedSoldier );
			agk::SetObjectAnimationFrame ( maskedSoldier, "Animation", 3002, 0.75 );
			
			DemoUtil::CreateCharacterController( maskedSoldier );
			//agk::Debug3DPhysicsCharacterController( maskedSoldier,1  );
			agk::Stand3DPhysicsCharacterController( maskedSoldier );
			agk::Set3DPhysicsCharacterControllerStepHeight( maskedSoldier,12.0 );
			agk::Set3DPhysicsCharacterControllerJumpSpeed( maskedSoldier, 6.0 );
		}
	}
	if ( agk::GetRawKeyPressed( 56 ) )
	{
		//if ( agk::Get3DPhysicsCharacterControllerExists( maskedSoldier ) ){
		//agk::Set3DPhysicsCharacterControllerPosition( maskedSoldier, 0.0, 40.0, 0.0 );
		//}
	}



 //   float joystickX = agk::GetVirtualJoystickX( 1 );
 //   float joystickY = agk::GetVirtualJoystickY( 1 );
	////TODO: need to limit rotation
	//agk::RotateCameraLocalX( 1, joystickY /2);
	//agk::RotateCameraLocalY( 1, joystickX /2);

 //   float rJoystickX = agk::GetVirtualJoystickX( 2 );
 //   float rJoystickY = agk::GetVirtualJoystickY( 2 );
	//agk::MoveCameraLocalX( 1, rJoystickX );
	//agk::MoveCameraLocalZ( 1, rJoystickY * -1.0 );
	
	agk::Print( agk::ScreenFPS() );
	uString results3( "Physics Objects:" );
	results3.Append( agk::Get3DPhysicsTotalObjects() );
	agk::Print( results3 );
	uString results4( "Active Physics Objects:");
	results4.Append( agk::Get3DPhysicsActiveObjects() );
	agk::Print( results4 );

if ( showInstructions ){
	agk::Print( "Rmb to throw sphere object " );
	agk::Print( "Lmb to pick Dynamic objects " );
	agk::Print( "Arrow Keys to move camera, Mouse to Rotate" );
	agk::Print( "WASD To Move Character, Q & E to Rotate, " );
	agk::Print( "SpaceBar to jump and C to crouch " );
	agk::Print( "The R Key will Ragdoll the Character , F key will Reset" );
	agk::Print( "The Escape Key will release the mouse to exit" );
	agk::Print( agk::Get3DPhysicsTotalJoints() );
}
	if ( agk::GetRawKeyPressed( 112 ) ){
		showInstructions = !showInstructions;
	}
	float elapsedTime = (btScalar)m_clock.getTimeMilliseconds();

	agk::Step3DPhysicsWorld();

	elapsedTime = (btScalar)m_clock.getTimeMilliseconds() - elapsedTime;
	maxElapsed = maxElapsed < elapsedTime ? elapsedTime : maxElapsed;
	agk::Print( maxElapsed );
	agk::Sync();
	//agk::Debug3DPhysicsWorld();
}

void app::End (void)
{
	agk::Delete3DPhysicsWorld();
	agk::DeleteAllImages();
	agk::DeleteAllObjects();

}

