// Show all errors
SetErrorMode(2) 

// Set some display properties
SetVirtualResolution( 1024, 768 )
SetClearColor( 101, 120, 154 )
SetScissor( 0,0,0,0 )
SetGenerateMipmaps( 1 )
SetCameraRange( 1, 0.1, 100 )
SetWindowAllowResize( 1 )

UseNewDefaultFonts( 1 )
SetPrintSize( 20 )
SetPrintColor( 255,0,0 )

shadowMode = 3 // start with cascade shadow mapping which gives the best quality
SetShadowMappingMode( shadowMode )
SetShadowSmoothing( 2 ) // random sampling
SetShadowMapSize( 1024, 1024 )
SetShadowRange( -1 ) // use the full camera range
SetShadowBias( 0.0012 ) // offset shadows slightly to avoid shadow artifacts

// show the sun, not necessary but improves the experience
SetSkyBoxVisible(1)

dx# = -0.3714
dy# = -0.7428
dz# = 0.5571
SetSunDirection( dx#, dy#, dz# )
SetAmbientColor( 30,30,30 )

// load a 3D object
LoadImage( 1, "Models/chapel_D.png" )
LoadObject( 1, "Models/chapel.obj", 10 )
SetObjectImage( 1, 1, 0 )
SetObjectTransparency( 1, 0 )
SetObjectCastShadow( 1, 1 )

// create a sphere object
CreateObjectSphere( 2, 1, 10, 10 )
SetObjectColor( 2, 0,255,0,128 )
SetObjectTransparency( 2, 0 )
SetObjectCastShadow( 2, 1 )

// make ground
CreateObjectPlane( 3, 100,100 )
SetObjectRotation( 3, 90,0,0 )
SetObjectPosition( 3, 0,0,0 ) 
SetObjectCastShadow( 3, 1 ) // this isn't necessary but improves the small low quality shadows
LoadImage( 2, "grass.png" )
SetImageWrapU( 2, 1 )
SetImageWrapV( 2, 1 )
SetObjectImage( 3, 2, 0 )
SetObjectUVScale( 3, 0, 10, 10 )

// make wall
CreateObjectPlane( 4, 20,20 )
SetObjectPosition( 4, 0,9,-20 )
SetObjectCastShadow( 4, 1 )

// optional bone animated character
useCharacter = 1
if useCharacter = 1
	LoadObjectWithChildren( 10, "Original_Soldier.X" )
	SetObjectVisible( GetObjectChildID(10, 1), 0 )
	SetObjectScalePermanent( 10, 0.05, 0.05, 0.05 )
	SetObjectPosition( 10, 6, 0, -6 )
	SetObjectRotation( 10, 0, 270, 0 )
	LoadImage( 3, "Soldier_body_1_D.png" )
	setobjectimage( 10, 3, 0 )
	SetObjectCastShadow( 10, 1 )

	LoadImage( 4, "gun_D.png" )
	SetObjectMeshImage( GetObjectChildID(10, 2), 1, 4, 0 )
	SetObjectScalePermanent( GetObjectChildID(10, 2), 0.05,0.05,0.05 )
	SetObjectCastShadow( GetObjectChildID(10, 2), 1 )
		
	SetObjectAnimationSpeed( 10, 20 )
	PlayObjectAnimation( 10, "", 1000, 1282, 1, 0.5 )
endif

// point camera towards the object
SetCameraPosition( 1, 15,10,0 )
SetCameraLookAt( 1, 0,5,0,0 )

// controls setup
AddVirtualButton( 1, 970,715,100 )
AddVirtualButton( 2, 970,610,100 )
SetVirtualButtonText( 2, "Forward" )
SetVirtualButtonText( 1, "Backward" )
startx# = GetPointerX()
starty# = GetPointerY()
time# = 0

// light controls
AddVirtualButton( 3, 54,715,100 )
AddVirtualButton( 4, 54,610,100 )
AddVirtualButton( 5, 178,715,100 )
SetVirtualButtonText( 4, "Light +" )
SetVirtualButtonText( 3, "Light -" )
SetVirtualButtonText( 5, "Mode" )

do
	// move the camera with keys
    if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, 0.2 )
	if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -0.2 )

	if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -0.2 )
	if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, 0.2 )

	if ( GetRawKeyState( 81 ) ) then MoveCameraLocalY( 1, -0.2 )
	if ( GetRawKeyState( 69 ) ) then MoveCameraLocalY( 1, 0.2 )
	
	// move the cmaera with virtual buttons
	if ( GetVirtualButtonState(1) = 1 ) then MoveCameraLocalZ( 1, -0.2 )
	if ( GetVirtualButtonState(2) = 1 ) then MoveCameraLocalZ( 1, 0.2 )
    
    // rotate the camera
	if ( GetPointerPressed() )
		startx# = GetPointerX()
		starty# = GetPointerY()
		angx# = GetCameraAngleX(1)
		angy# = GetCameraAngleY(1)
		pressed = 1
	endif

	if ( GetPointerState() = 1 )
		fDiffX# = (GetPointerX() - startx#)/4.0
		fDiffY# = (GetPointerY() - starty#)/4.0

		newX# = angx# + fDiffY#
		if ( newX# > 89 ) then newX# = 89
		if ( newX# < -89 ) then newX# = -89
		SetCameraRotation( 1, newX#, angy# + fDiffX#, 0 )
	endif
	
	// change shadow mode
	if ( GetRawKeyPressed(32) or GetVirtualButtonPressed(5) = 1 )
		inc shadowMode
		if ( shadowMode > 3 ) then shadowMode = 0
		SetShadowMappingMode( shadowMode )
	endif
    
	// move the green sphere
	time# = time# + GetFrameTime()
	x# = Sin( time#*70 ) * 7
	z# = Cos( time#*70 ) * 7
	SetObjectPosition( 2, x#, 2, z# )

	// move directional light
	if ( GetRawKeyState( 90 ) or GetVirtualButtonState(3) ) then inc dx#, 0.02
	if ( GetRawKeyState( 88 ) or GetVirtualButtonState(4) ) then dec dx#, 0.02
	SetSunDirection( dx#, dy#, dz# )

	// display some useful information
	Print( "Click and drag to rotate the camera, W, A, S, D, Q, E to move" )
	Print( "Press X or Z to move the sun, press Space to change shadow mapping mode" )
	Print( "FPS: " + str(screenfps(),2) )
	PrintC( "Shadow Mode: " )
	select shadowMode
		case 0 : Print( "None" ) : endcase
		case 1 : Print( "Uniform shadow mapping" ) : endcase
		case 2 : Print( "Light Space Perspective shadow mapping" ) : endcase
		case 3 : Print( "Cascade shadow mapping" ) : endcase
	endselect

	Sync()
loop
