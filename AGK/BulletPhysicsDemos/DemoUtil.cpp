
#include "DemoUtil.h"

int floorTex;
int ballTex;
int crateTex; 
int concreteTex;
int dumpsterTex;
int testBall;

int HitObjID;
int pickJoint = -1;

int PickVec;

float WalkVelocity = 72.0;
float RunVelocity = 140.0;
float RotationInc = 1.0;

float rotationInc = 1.0;
float finalRotation;

//Rotate Camera with mouse
float rawMouseX = 0.0;
float rawMouseY = 0.0;
int lockMouse = 0;
int mouseSet = 0;
float cameraAngleX = 0.0;
float cameraAngleY = 0.0;
float oldCamAngleX = 0.0;
float oldCamAngleY = 0.0;


void DemoUtil::Setup()
{
	floorTex = agk::LoadImage("/media/steelPanelTiled.png");
	agk::SetImageWrapU(floorTex,0);
	agk::SetImageWrapV(floorTex,0);

	ballTex = agk::LoadImage("/media/soccerball2.jpg");
	agk::SetImageWrapU(ballTex,1);
	agk::SetImageWrapV(ballTex,1);

	crateTex = agk::LoadImage("/media/Crate_color.bmp");
	agk::SetImageWrapU(crateTex,1);
	agk::SetImageWrapV(crateTex,1);

	concreteTex = agk::LoadImage("/media/Concrete.png");
	agk::SetImageWrapU(concreteTex,1);
	agk::SetImageWrapV(concreteTex,1);

	dumpsterTex = agk::LoadImage("/media/dumpster_D.png");
	agk::SetImageWrapU(dumpsterTex,1);
	agk::SetImageWrapV(dumpsterTex,1);
}


void DemoUtil::CreateBoxStack(float boxSize, int rows, int columns)
{
	for ( int h = 0; h < rows; h++ )
	{
		for ( int i = 0; i < columns; i++ )
		{
			int objID = agk::CreateObjectBox( boxSize, boxSize, boxSize );
			agk::SetObjectImage( objID,crateTex,0 );
			agk::SetObjectPosition( objID, (boxSize*columns/2-boxSize/2)*-1 + (boxSize*i), boxSize/2 + (boxSize*h)+ 150.5, 50 );
			//3D Physics code
			agk::Create3DPhysicsDynamicBody( objID );
			//agk::SetObjectShapeConvexHull( objID );
			//agk::SetObjectShapeBox( objID );
			agk::SetObject3DPhysicsGroupAndMask( objID, group0, mask0 | mask1 | mask2 );
			agk::SetObject3DPhysicsMass( objID, 2.0 );
			agk::SetObject3DPhysicsFriction( objID, 0.3 );
		}
	}
}

void DemoUtil::ThrowBall( float initialSpeed, float mass )
{
	float directionX = agk::Get3DVectorXFromScreen( agk::GetPointerX(), agk::GetPointerY() );
	float directionY = agk::Get3DVectorYFromScreen( agk::GetPointerX(), agk::GetPointerY() );
	float directionZ = agk::Get3DVectorZFromScreen( agk::GetPointerX(), agk::GetPointerY() );
		
	int throwBall = agk::CreateObjectSphere( 12, 16, 32 );
	agk::SetObjectImage( throwBall, ballTex, 0 );
	agk::SetObjectPosition( throwBall, agk::GetCameraX( 1 ), agk::GetCameraY( 1 ),agk::GetCameraZ( 1 ) );
	//3D Physics code
	agk::Create3DPhysicsDynamicBody( throwBall );
	agk::SetObjectShapeSphere( throwBall );
	agk::SetObject3DPhysicsGroupAndMask( throwBall, group0, mask0 | mask1 | mask2 );
	agk::SetObject3DPhysicsMass( throwBall, mass );
	agk::SetObject3DPhysicsLinearVelocity( throwBall, directionX, directionY, directionZ, initialSpeed );
	agk::SetObject3DPhysicsFriction( throwBall, 0.8 );
	agk::SetObject3DPhysicsRollingFriction( throwBall, 0.7 );
}

void DemoUtil::DropCylinder()
{
	int objID = agk::CreateObjectCylinder( 70, 40, 16 );
	agk::SetObjectImage( objID, concreteTex, 0 );
	agk::SetObjectPosition( objID, 0, 400, 0 );
	agk::Create3DPhysicsDynamicBody( objID );
	//agk::SetObjectShapeCylinder( objID, 1 );
	//agk::SetObjectShapeCylinder( objID, 1, 70.0, 40.0 );
	agk::SetObjectShapeConvexHull( objID );
	agk::SetObject3DPhysicsGroupAndMask( objID, group0, mask0 | mask1 | mask2 );
}

void DemoUtil::DropCone()
{
	int objID = agk::CreateObjectCone( 70, 40, 16 );
	agk::SetObjectImage( objID, concreteTex, 0 );
	agk::SetObjectPosition( objID, 0, 400, 0 );
	agk::Create3DPhysicsDynamicBody( objID );
	//agk::SetObjectShapeCone( objID, 1  );
	agk::SetObjectShapeCone( objID, 1, 70, 40 );
	agk::SetObject3DPhysicsGroupAndMask( objID, group0, mask0 | mask1 | mask2 );
}

void DemoUtil::DropCapsule()
{
	//int objID = agk::LoadObject( "/media/Capsule.x", 80 );
	int objID = agk::CreateObjectCapsule( 8, 32.0, 1 );
	
	agk::SetObjectImage( objID, concreteTex, 0 );
	agk::SetObjectPosition( objID, 0, 100, 0 );

	agk::SetObjectLookAt( objID, -150, 6, 0, 0 );

	agk::Create3DPhysicsDynamicBody( objID );
	//agk::SetObjectShapeCapsule( objID,2 );
	agk::SetObjectShapeCapsule( objID, 1, 16.0, 32.0, 16.0 );
	agk::SetObject3DPhysicsGroupAndMask( objID, group0, mask0 | mask1 | mask2 );
}

void DemoUtil::DropConvexHull()
{
	int objID = agk::LoadObject( "/media/dumpster_3yardOrigin.x" );
	agk::SetObjectImage( objID, dumpsterTex, 0 );
	agk::SetObjectPosition( objID, 0, 400, 0 );
	agk::Create3DPhysicsDynamicBody( objID );
	agk::SetObjectShapeConvexHull( objID );
}

//			Original code created by mike max               //
//				Ported to T2 by TheZoq2						//
//http://forum.thegamecreators.com/?m=forum_view&t=203224&b=48&msg=2441546#m2441546
int DemoUtil::loadTerrain(char heightMap[128], char sdTexture[128], char hdTexture[128], float detailX, float detailY, float MaxHeight,int stepSize)
{
	int heightMapImg = agk::LoadImage(heightMap);
	
	std::vector<Vertex> v;

	//Getting the size of the heightmap
	int x = agk::GetImageWidth(heightMapImg) - 2; int y = agk::GetImageHeight(heightMapImg) - 2;
	
	std::vector< std::vector<Vertex> > vertices;

	for (int i = 0; i <= x; i++)
	{
		//v = std::vector<Vertex*> (y, new Vertex());
		vertices.push_back(v);
		for(int j = 0; j <= y; j++){
			Vertex tempVertex;
			vertices[i].push_back(tempVertex);
		}
	}

	int memblockID = agk::CreateMemblockFromImage(heightMapImg);

	//Checking if the object file exists and deleting it if it does
	if(agk::GetFileExists( "Map.obj" ))
			agk::DeleteFile( "Map.obj" );

	int ObjFile = agk::OpenToWrite( "Map.obj" );
	int VertNumber = 1; 
	//Generation loop
	for(int ly = 0; ly <= y; ly = ly + stepSize)
	{
		agk::PrintC( "Generating Vertices: " ); agk::Print(ly);
		agk::Render2DFront(); agk::Swap();

		for(int lx = 0; lx <= x; lx = lx + stepSize)
		{
			int offsetPix=12+(((lx)+(ly*(x+2))))*4;
			int r = agk::GetMemblockByte(memblockID, offsetPix + 0);
			int g = agk::GetMemblockByte(memblockID, offsetPix + 1);
			int b = agk::GetMemblockByte(memblockID, offsetPix + 2);
			int a = agk::GetMemblockByte(memblockID, offsetPix + 3);

			float h = ((r+g+b)/3.0f)/255.0f;
			vertices[lx][ly].x = lx;
			vertices[lx][ly].y = h*MaxHeight;
			vertices[lx][ly].z = y - ly;

			//UV cordinates
			vertices[lx][ly].u = float(lx)/ float(x);
			vertices[lx][ly].v = (float( y - ly)/float(y));

			//Vertex number
			vertices[lx][ly].vertexNumber = VertNumber;

			//Getting the line to write to the file
			char line[128];
			sprintf(line, "v %f %f %f",vertices[lx][ly].x, vertices[lx][ly].y, vertices[lx][ly].z);

			//Writing the lines to the file
			agk::WriteLine(ObjFile, line);

			//Getting the uv file
			char line2[128];
			sprintf(line2, "vt %f %f", vertices[lx][ly].u, vertices[lx][ly].v);
			agk::WriteLine(ObjFile, line2);

			VertNumber = VertNumber + 1;
		}
		agk::Print( "Loading Generated Terrain" );
	}

	//Creating the faces
	for(int ly = 0; ly <= y; ly = ly + stepSize)
	{
		//Printing the progress
		agk::PrintC("Generating faces: "); agk::Print(ly);
		agk::Render2DFront(); agk::Swap();
		for(int lx = 0; lx <= y; lx = lx + stepSize)
		{
			//
			if (lx>stepSize-1 && ly>stepSize-1)
			{
				//Converting indexes to form faces
				int v1 = vertices[lx][ly].vertexNumber;
				int v2 = vertices[lx - stepSize][ly].vertexNumber;
				int v3 = vertices[lx][ly - stepSize].vertexNumber;

				int v4 = vertices[lx][ly-stepSize].vertexNumber;
				int v5 = vertices[lx-stepSize][ly].vertexNumber;
				int v6 = vertices[lx-stepSize][ly-stepSize].vertexNumber;

				// VT indexes are the same as vertices ones.
				int vt1 = v1; int vt2=v2; int vt3=v3; int vt4=v4; int vt5=v5; int vt6=v6;

				char line[128];
				sprintf(line, "f %i/%i %i/%i %i/%i", v1, vt1, v2, vt2, v3, vt3);
				char line2[128];
				sprintf(line2, "f %i/%i %i/%i %i/%i", v4, vt4, v5, vt5, v6, vt6);

				agk::WriteLine(ObjFile, line);
				agk::WriteLine(ObjFile, line2);
			}
		}
		agk::Print( "Loading Generated Terrain" );
	}
	agk::CloseFile( ObjFile );
	//agk::Print( "Loading Generated Terrain" );
	int terrainID = agk::LoadObject( "Map.obj" );

	if(agk::GetFileExists( sdTexture ) ) 
	{
		int colorMap = agk::LoadImage( sdTexture );
		agk::SetObjectImage( terrainID, colorMap, 0 );
		agk::SetImageWrapU( colorMap,1 );
		agk::SetImageWrapV( colorMap, 1 );
	}

	if(agk::GetFileExists( hdTexture ) )
	{
		int detailMap = agk::LoadImage( hdTexture );
		agk::SetImageWrapU( detailMap, 1 );
		agk::SetImageWrapV( detailMap, 1 );
		agk::SetObjectImage( terrainID, detailMap, 1 );

		//Loading the shader
		int detailShader = agk::LoadShader( "/media/Terrain/vertexDetail.vs", "/media/Terrain/pixelDetail.ps" );
		agk::SetShaderConstantByName( detailShader, "ScaleDetailMap", detailX, detailY, 0, 0 );
		agk::SetObjectShader( terrainID, detailShader );
	}

	return terrainID;
}

void DemoUtil::PickJointCreateAndUpdate()
{
	//Left mouse btn pressed creates pick joint
	if ( agk::GetPointerPressed() )
	{
		float distance = 1500.0;
		float directionX = agk::Get3DVectorXFromScreen( agk::GetPointerX(), agk::GetPointerY() ) * distance + agk::GetCameraX(1);
		float directionY = agk::Get3DVectorYFromScreen( agk::GetPointerX(), agk::GetPointerY() ) * distance + agk::GetCameraY(1);
		float directionZ = agk::Get3DVectorZFromScreen( agk::GetPointerX(), agk::GetPointerY() ) * distance + agk::GetCameraZ(1);

		HitObjID =  agk::ObjectRayCast( 0, agk::GetCameraX(1), agk::GetCameraY(1), agk::GetCameraZ(1), 
															directionX, directionY, directionZ );
		if ( HitObjID > 0 )
		{
			PickVec = agk::CreateVector3( agk::GetObjectRayCastX( 0 ), agk::GetObjectRayCastY( 0 ), agk::GetObjectRayCastZ( 0 ) );
			pickJoint = agk::Create3DPhysicsPickJoint( HitObjID, PickVec );
		}
	}

	//Left Mouse btn held down holds pick joint
	if ( agk::GetPointerState() && pickJoint > 0 )
	{
		int updatedPosVec = agk::CreateVector3( agk::Get3DVectorXFromScreen( agk::GetPointerX(), agk::GetPointerY() ),
																		agk::Get3DVectorYFromScreen( agk::GetPointerX(), agk::GetPointerY() ),
																			agk::Get3DVectorZFromScreen( agk::GetPointerX(), agk::GetPointerY() ) );

		int CamVec = agk::CreateVector3( agk::GetCameraX(1), agk::GetCameraY(1), agk::GetCameraZ(1) );
		float origDistance = agk::GetVector3Distance( PickVec, CamVec );

		agk::GetVector3Multiply( updatedPosVec, origDistance );

		agk::GetVector3Add( updatedPosVec, CamVec );

		agk::Update3DPhysicsPickJoint( pickJoint, updatedPosVec );
		agk::DeleteVector3( updatedPosVec );
		agk::DeleteVector3( CamVec );
	}
	else
	{
		if ( pickJoint > 0 )
			agk::Delete3DPhysicsPickJoint( pickJoint );
		pickJoint = 0;
	}
}

void DemoUtil::ControlKinematicObjectWithArrowKeys( int objID )
{
	if ( agk::GetKeyboardExists() ){
		if ( agk::GetRawKeyState( 37 ) ){
			agk::MoveObjectLocalX( objID, 1.5 ) ;
		}
		if ( agk::GetRawKeyState( 39 ) ){
			agk::MoveObjectLocalX( objID, -1.5 ) ;
		}
		if ( agk::GetRawKeyState( 38 ) ){
			agk::MoveObjectLocalZ( objID, -1.5 ) ;
		}
		if ( agk::GetRawKeyState( 40 ) ){
			agk::MoveObjectLocalZ( objID, 1.5 ) ;
		}
	}
}

void DemoUtil::ControlCameraWithArrowKeys( int cameraID )
{
	if ( agk::GetKeyboardExists() ){
		if ( agk::GetRawKeyState( 37 ) ){
			agk::MoveCameraLocalX( cameraID, -1.5 ) ;
		}
		if ( agk::GetRawKeyState( 39 ) ){
			agk::MoveCameraLocalX( cameraID, 1.5 ) ;
		}
		if ( agk::GetRawKeyState( 38 ) ){
			agk::MoveCameraLocalZ( cameraID, 1.5 ) ;
		}
		if ( agk::GetRawKeyState( 40 ) ){
			agk::MoveCameraLocalZ( cameraID, -1.5 ) ;
		}
	}
}

void DemoUtil::RotateCameraWithMouse( int cameraID, float speed )
{
	if ( agk::GetMouseExists() && lockMouse == 0 )
	{
		rawMouseX = 0.0;
		rawMouseY = 0.0;
		if ( mouseSet == 1){
			rawMouseX = ( agk::GetDeviceWidth() / 2 ) -  agk::GetRawMouseX() ;
			rawMouseY = ( agk::GetDeviceHeight() / 2 ) -  agk::GetRawMouseY() ;
			oldCamAngleX = cameraAngleX;
			oldCamAngleY = cameraAngleY;
			cameraAngleY = WrapAngle( cameraAngleY - rawMouseX  *0.2 );
			cameraAngleX = WrapAngle( cameraAngleX - rawMouseY *0.2 );
			if ( cameraAngleX <= 290.0 && cameraAngleX > 180.0 )
					 cameraAngleX = 290.0;
			if ( cameraAngleX >= 70.0 && cameraAngleX <180.0 )
					cameraAngleX = 70.0;
			cameraAngleX = CurveAngle( cameraAngleX, oldCamAngleX, speed );
			cameraAngleY = CurveAngle( cameraAngleY, oldCamAngleY, speed );
		}
		agk::SetRawMousePosition( agk::GetDeviceWidth() / 2, agk::GetDeviceHeight() / 2 );
		//agk::SetRawMouseVisible( 0 );
		mouseSet = 1;
	}
	agk::SetCameraRotation( cameraID, cameraAngleX, cameraAngleY, 0.0 );
	if ( agk::GetRawKeyReleased( 27 ) ){
		lockMouse = lockMouse + 1;
		if ( lockMouse ==  2 )
				lockMouse = 0;
		if ( agk::GetMouseExists()  && lockMouse == 1 ){
			//agk::SetRawMouseVisible(1);
			mouseSet = 0;
		}
	}
}

void DemoUtil::CreateCharacterController( int objID )
{
	int characterOffsetVec = agk::CreateVector3( 0.0, 36.0, 0.0 ); 
	int objectOrientationVec = agk::CreateVector3( 0.0, 0.0, -1.0 );
	agk::Create3DPhysicsCharacterController( objID, 1, characterOffsetVec, objectOrientationVec, 0.75 );
	agk::DeleteVector3( characterOffsetVec );
	agk::DeleteVector3( objectOrientationVec );
}

void DemoUtil::CharacterControllerInput( int objID )
{
	if ( agk::GetKeyboardExists() )
	{
		//Stops movement when no key is pressed.
		agk::Move3DPhysicsCharacterController( objID, 0, WalkVelocity );
		
		//W Key Forwards
		if ( agk::GetRawKeyState( 87 ) )
		{
			agk::Move3DPhysicsCharacterController( objID, 1, WalkVelocity );
		}
		if ( agk::GetRawKeyState( 67 ) &&  agk::GetRawKeyPressed( 87 ) ||  agk::GetRawKeyState( 87 ) &&  agk::GetRawKeyPressed( 67 ) ) 
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 2073, 2101, 1, 0.25 );
		}
		else if ( agk::GetRawKeyPressed( 87 ) || (agk::GetRawKeyReleased( 67 ) &&  agk::GetRawKeyState( 87 )))
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 1290, 1320, 1, 0.25 );
		}
		if ( agk::GetRawKeyReleased( 87 ) )
		{
			agk::PlayObjectAnimation( objID, "Animation", 3002, 3547, 1, 0.25 );
		}
		//Strafe Held and forward released
		if ( agk::GetRawKeyPressed( 87 ) &&  agk::GetRawKeyState( 65 ) )
		{

		}

		//S key Backwards
		if ( agk::GetRawKeyState( 83 ) )
		{
			agk::Move3DPhysicsCharacterController( objID, 2, WalkVelocity );
		}
		if ( agk::GetRawKeyPressed( 83 ) )
		{
			agk::SetObjectAnimationSpeed( objID, -30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 1290, 1320, 1, 0.25 );
		}
		if ( agk::GetRawKeyReleased( 83 ) )
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 3002, 3547, 1, 0.25 );
		}

		//A Key Strafe Left
		if ( agk::GetRawKeyState( 65 ) )
		{
			if ( agk::GetObjectAnimationTime( objID ) > 635 && agk::GetObjectAnimationTime( objID ) < 643 )
			{
			agk::Move3DPhysicsCharacterController( objID, 3, WalkVelocity - 65.0 );
			}
			else{
			agk::Move3DPhysicsCharacterController( objID, 3, WalkVelocity - 40.0 );
			}	
		}
		if ( agk::GetRawKeyPressed( 65 ) )
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 611, 643, 1, 0.25 );
		}
		if ( agk::GetRawKeyReleased( 65 ) )
		{
			agk::PlayObjectAnimation( objID, "Animation", 3002, 3547, 1, 0.25 );
		}
		//To handle release of left strafe with forward still held
		if ( agk::GetRawKeyReleased( 65 ) &&  agk::GetRawKeyState( 87 ) )
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 1290, 1320, 1, 0.25 );
		}
		//To handle release of left strafe with backwards still held
		if ( agk::GetRawKeyReleased( 65 ) && agk::GetRawKeyState( 83 ) )
		{
			agk::SetObjectAnimationSpeed( objID, -30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 1290, 1320, 1, 0.25 );
		}
		//Strafe Left Held and forward released
		if ( agk::GetRawKeyState( 65 ) && agk::GetRawKeyReleased( 87 ) )
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 611, 643, 1, 0.25 );
		}
		//Strafe left Held and backwards released
		if ( agk::GetRawKeyState( 65 ) && agk::GetRawKeyReleased( 83 ) )
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 611, 643, 1, 0.25 );
		}


		//D Key Strafe Right
		if ( agk::GetRawKeyState( 68 ) )
		{
			if ( agk::GetObjectAnimationTime( objID ) > 635 && agk::GetObjectAnimationTime( objID ) < 643 )
			{
			agk::Move3DPhysicsCharacterController( objID, 4, WalkVelocity - 65.0 );
			}
			else{
				agk::Move3DPhysicsCharacterController( objID, 4, WalkVelocity - 40.0 );
			}
		}
		if ( agk::GetRawKeyPressed( 68 ) )
		{
			agk::SetObjectAnimationSpeed( objID, -30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 611, 643, 1, 0.25 );
		}
		if ( agk::GetRawKeyReleased( 68 ) )
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 3002, 3547, 1, 0.25 );
		}
		//To handle release of Right strafe with forward still held
		if ( agk::GetRawKeyReleased( 68 ) &&  agk::GetRawKeyState( 87 ) )
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 1290, 1320, 1, 0.25 );
		}
		//To handle release of Right strafe with backwards still held
		if ( agk::GetRawKeyReleased( 68 ) && agk::GetRawKeyState( 83 ) )
		{
			agk::SetObjectAnimationSpeed( objID, -30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 1290, 1320, 1, 0.25 );
		}
		//Strafe Right Held and forward released
		if ( agk::GetRawKeyState( 68 ) && agk::GetRawKeyReleased( 87 ) )
		{
			agk::SetObjectAnimationSpeed( objID, -30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 611, 643, 1, 0.25 );
		}
		//Strafe Right Held and backwards released
		if ( agk::GetRawKeyState( 68 ) && agk::GetRawKeyReleased( 83 ) )
		{
			agk::SetObjectAnimationSpeed( objID, -30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 611, 643, 1, 0.25 );
		}

		//SpaceBar Jump
		if ( agk::GetRawKeyPressed( 32 ) )
		{
			agk::Jump3DPhysicsCharacterController( objID );
			//agk::PlayObjectAnimation( objID, "Animation", 2283, 2333, 0, 0.25 );
		}

		//C_Key And W_Key Crouch and move forward or just crouch
		if ( agk::GetRawKeyState( 67 ) &&  agk::GetRawKeyState( 87 ) ) 
		{
			if ( agk::GetObjectAnimationTime( objID ) > 2094 && agk::GetObjectAnimationTime( objID ) < 2104 )
			{
				agk::Move3DPhysicsCharacterController( objID, 1, WalkVelocity - 65.0 );
			}
			else{
				agk::Move3DPhysicsCharacterController( objID, 1, WalkVelocity - 40.0 );
			}
		}
		else if ( agk::GetRawKeyPressed( 67 ) || (agk::GetRawKeyReleased( 87 ) &&  agk::GetRawKeyState( 67 )))
		{
			agk::Crouch3DPhysicsCharacterController( objID );
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 1672, 1912, 0, 0.25 );
		}
		else if (agk::GetRawKeyReleased( 67 ) &&  agk::GetRawKeyState( 87 ))
		{
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 1290, 1320, 1, 0.25 );
		}
		else if ( agk::GetRawKeyReleased( 67 ) )
		{
			agk::Stand3DPhysicsCharacterController( objID );
			agk::SetObjectAnimationSpeed( objID, 30.0 );
			agk::PlayObjectAnimation( objID, "Animation", 3002, 3547, 1, 0.25 );
		}

		//Q Key For Rotate
		if ( agk::GetRawKeyState( 81 ) )
		{
			finalRotation = finalRotation - rotationInc;
		}
		//E Key For Rotate
		if ( agk::GetRawKeyState( 69 ) )
		{
			finalRotation = finalRotation + rotationInc;
		}
		agk::Rotate3DPhysicsCharacterController( objID, finalRotation );
	}
}

//This command will return a value that does not exceed the range of 0 to 360. 
float DemoUtil::WrapAngle( float angle  ) 
{
	//Taken From DBPro Source
	int iChunkOut = ( int )angle;
	iChunkOut = iChunkOut - ( iChunkOut % int( 360 ) );
	angle = angle - iChunkOut;
	int breakout = 10000;
	while ( angle < 0.0f || angle >= 360.0 ){
		if ( angle < 0.0f ) angle = angle + 360.0;
		if ( angle >= 360.0 ) angle = angle - 360.0;
		breakout--;
		if ( breakout == 0 ) break;
	}
	if ( breakout == 0 ) angle = 0.0f;
	return angle;
}

//This command will return an auto-interpolated value based on a given speed. 
float DemoUtil::CurveValue( float destination, float current, float speed )
{
	//Taken From DBPro Source
	if ( speed <1.0f) speed = 1.0f;
	float diff = destination - current;
	return current = current + ( diff / speed );
}

//This command will return an auto-interpolated angle based on a given speed. 
float DemoUtil::CurveAngle( float destination, float current, float speed )
{
	//Taken From DBPro Source
	if ( speed < 1.0f ) speed = 1.0f;
	destination = DemoUtil::WrapAngle( destination );
	current = DemoUtil::WrapAngle( current );
	float diff = destination - current;
	if ( diff <- 180.0f) diff = ( destination + 360.0f ) - current;
	if( diff > 180.0f ) diff = destination - ( current + 360.0f );
	current = current + ( diff / speed );
	return current = DemoUtil::WrapAngle( current );
}


