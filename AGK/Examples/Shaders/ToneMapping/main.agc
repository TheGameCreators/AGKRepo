
// Project: ToneMapping 
// Created: 2017-01-20

// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "ToneMapping" )
SetWindowSize( 1800, 1200, 0 )
SetWindowAllowResize( 1 )
SetScissor( 0,0,0,0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts
SetClearColor( 255,255,255 )

// set a high sun and ambient color
SetSunColor( 300,300,300 )
SetSunDirection( 2,-5,2 )
SetAmbientColor( 100,100,100 )

SetShadowMappingMode( 3 )
SetShadowSmoothing( 1 )
SetShadowMapSize( 1024,1024 )
SetShadowBias( 0.001 )

// a light for inside the room, dark relative to the sun
CreatePointLight( 1, 0,30,0, 150, 20,20,20 )
SetPointLightMode( 1, 1 ) // make it a pixel shader light, better quality, and we can separate it from the ambient light in the vertex shader

width = GetDeviceWidth()
height = GetDeviceHeight()
sceneImg = CreateRenderImage( width, height, 0, 0 ) // render to this instead of the screen so we can post process the scene

quad = CreateObjectQuad()
defaultQuadShader = LoadFullScreenShader( "Default.ps" ) // makes no changes to the image, just passes it through to the render image
luminanceShader = LoadFullScreenShader( "Luminance.ps" ) // calculates the luminance of each scene pixel, could be modified to do log luminance

// Luminance reduction, create a series of successively smaller images until we get down to 3x3
// this will convert our scene image into a single 3x3 pixel image which will be the average luminance around the scene
lumImage = 101
while( width > 3 or height > 3 )
	width = ceil(width / 2.0)
	height = ceil(height / 2.0)
	if ( width < 3 ) then width = 3
	if ( height < 3 ) then height = 3
	
	CreateRenderImage( lumImage, width, height, 0, 0 )
	inc lumImage, 1
endwhile
numLumImages = lumImage - 101
finalLumImg = lumImage - 1

// setup tone map images
// the tone image will tell the objects how much to alter their light calculations
// it is just a single pixel, modified by the GPU only
// we also need a temp image so we can read the old value into a shader whilst also writing out a new tone map value
toneImg = lumImage
toneImgTemp = lumImage+1
CreateRenderImage( toneImg, 1, 1, 0, 0 )
CreateRenderImage( toneImgTemp, 1, 1, 0, 0 )
toneShader = LoadFullScreenShader( "ToneMap.ps" ) // will take the 3x3 pixel luminance image and the old tone map image and produce a new tone map image
exposure# = 0.65
SetShaderConstantByName( toneShader, "targetExposure", exposure#, 0,0,0 )
SetShaderConstantByName( toneShader, "toneMapSpeed", 0.1, 0,0,0 )
addvirtualbutton( 3, 970,54,100 )
addvirtualbutton( 4, 970,158,100 )
SetVirtualButtonText(3,"Exp+")
SetVirtualButtonText(4,"Exp-")

// set default tone map value to 0.5 light (128)
SetRenderToImage( toneImg, 0 )
SetClearColor( 128,128,128 )
ClearScreen()
SetRenderToScreen()
SetClearColor( 255,255,255 )

// create our scene, everything must be assigned the tone map image so it knows how to adjust its light calculations
// all objects will also need to be using a shader that knows how to use it
CreateSkybox( toneImg )
CreateGround( toneImg )
CreateRoom( toneImg )

// position the camera in the room
SetCameraPosition( 1, 15, 20, 0 )
SetCameraLookAt( 1, -30, 0, 0, 0 )

// controls setup
addvirtualbutton( 1, 970,715,100 )
addvirtualbutton( 2, 970,610,100 )
SetVirtualButtonText(2,"Forward")
SetVirtualButtonText(1,"Back")
startx# = GetPointerX()
starty# = GetPointerY()
time# = 0

// for debugging luminance image
//CreateSprite( 1, 0 )
//SetSpriteVisible( 1, 0 )
//SetSpritePosition( 1, 4, 734 )
//SetSPriteSize( 1, 30, 30 )

do
	// control the camera
	speed# = 1.0
	if ( GetRawKeyState( 16 ) ) then speed# = 0.1
	
    if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, speed# )
	if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -speed# )

	if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -speed# )
	if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, speed# )

	if ( GetRawKeyState( 81 ) ) then MoveCameraLocalY( 1, -speed# )
	if ( GetRawKeyState( 69 ) ) then MoveCameraLocalY( 1, speed# )

    if ( GetVirtualButtonState(1) = 1 ) then MoveCameraLocalZ( 1, -speed# )
    if ( GetVirtualButtonState(2) = 1 ) then MoveCameraLocalZ( 1, speed# )

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
    
    // change exposure
    if ( GetRawKeyState( 33 ) or GetVirtualButtonState(3) ) then inc exposure#, 0.01
    if ( GetRawKeyState( 34 ) or GetVirtualButtonState(4) ) then dec exposure#, 0.01
    SetShaderConstantByName( toneShader, "targetExposure", exposure#, 0,0,0 )

	Print( "FPS: " + str(ScreenFPS(),2) )  
	Print( "Target Exposure: " + str(exposure#) )
   
    // only call this once, updates the scene, physics, animation, etc
    Update(0)
    
    // render scene to image so we can get luminance data from it
    SetRenderToImage( sceneImg, -1 )
    ClearScreen()
    Render()
    
    // render the scene to the first luminance image, the first luminance image is also smaller so it will start reducing the size
    SetRenderToImage( 101, 0 ) // first luminance image
    ClearScreen()
    SetObjectShader( quad, luminanceShader )
    SetObjectImage( quad, sceneImg, 0 )
    DrawObject( quad )
    
    // luminance reduction, reduce the first luminance image to a 1x1 image containing the average luminance
    SetObjectShader( quad, defaultQuadShader ) // even though this makes no changes to the image, the render image is smaller than the source image, so it reduces it in the process of passing through
    for i = 2 to numLumImages
		SetRenderToImage( i+100, 0 )
		ClearScreen()
		SetObjectImage( quad, i+99, 0 )
		DrawObject( quad )
	next i
	
	// start tone mapping, first make a copy of the existing tone image so we use it as an input to the shader whilst writing the new value
	SetRenderToImage( toneImgTemp, 0 )
	ClearScreen()
	SetObjectShader( quad, defaultQuadShader )
	SetObjectImage( quad, toneImg, 0 )
	DrawObject( quad )
	
	// now modify tone image, this will only take effect when we draw all the objects next frame
	SetRenderToImage( toneImg, 0 )
	ClearScreen()
	SetObjectShader( quad, toneShader )
	SetObjectImage( quad, toneImgTemp, 0 )
	SetObjectImage( quad, finalLumImg, 1 )
	DrawObject( quad )	
	
	// draw the scene image to the screen
	SetRenderToScreen() // no need to clear as it was already done by Swap() at the end of the last frame
	SetObjectImage( quad, sceneImg, 0 )
	SetObjectShader( quad, defaultQuadShader ) // no changes to the image, just draw it to the screen
	DrawObject( quad )
	
	// for debugging luminance image
	//SetSpriteImage( 1, finalLumImg )
	//SetSpriteVisible( 1, 1 )
	//DrawSprite( 1 )
	//SetSpriteVisible( 1, 0 )
	
	Swap()
loop

function CreateSkybox( toneImg as integer )
	front = CreateObjectPlane( 1000,1000 )
	back = CreateObjectPlane( 1000,1000 )
	left = CreateObjectPlane( 1000,1000 )
	right = CreateObjectPlane( 1000,1000 )
	top = CreateObjectPlane( 1000,1000 )
	bottom = CreateObjectPlane( 1000,1000 )
	
	SetObjectPosition( front, 0,0,500 )
	SetObjectPosition( back, 0,0,-500 )
	SetObjectPosition( left, -500,0,0 )
	SetObjectPosition( right, 500,0,0 )
	SetObjectPosition( top, 0,500,0 )
	SetObjectPosition( bottom, 0,-500,0 )
	
	SetObjectRotation( back, 0,180,0 )
	SetObjectRotation( left, 0,-90,0 )
	SetObjectRotation( right, 0,90,0 )
	SetObjectRotation( top, -90,0,0 )
	SetObjectRotation( bottom, 90,0,0 )
	
	frontImg = LoadImage( "100000.png" )
	backImg = LoadImage( "100002.png" )
	leftImg = LoadImage( "100003.png" )
	rightImg = LoadImage( "100001.png" )
	topImg = LoadImage( "100004.png" )
	bottomImg = LoadImage( "100005.png" )
	
	SetObjectImage( front, frontImg, 0 )
	SetObjectImage( back, backImg, 0 )
	SetObjectImage( left, leftImg, 0 )
	SetObjectImage( right, rightImg, 0 )
	SetObjectImage( top, topImg, 0 )
	SetObjectImage( bottom, bottomImg, 0 )
	
	SetObjectColor( front, 400,400,400,255 )
	SetObjectColor( back, 400,400,400,255 )
	SetObjectColor( left, 400,400,400,255 )
	SetObjectColor( right, 400,400,400,255 )
	SetObjectColor( top, 400,400,400,255 )
	SetObjectColor( bottom, 400,400,400,255 )
	
	objectTexShader = LoadShader( "SkyBox.vs", "SkyBox.ps" )
	SetObjectShader( front, objectTexShader )
	SetObjectShader( back, objectTexShader )
	SetObjectShader( left, objectTexShader )
	SetObjectShader( right, objectTexShader )
	SetObjectShader( top, objectTexShader )
	SetObjectShader( bottom, objectTexShader )	
	
	// all objects must have the tone map image, and a shader that knows how to use it
	SetObjectImage( front, toneImg, 1 )
	SetObjectImage( back, toneImg, 1 )
	SetObjectImage( left, toneImg, 1 )
	SetObjectImage( right, toneImg, 1 )
	SetObjectImage( top, toneImg, 1 )
	SetObjectImage( bottom, toneImg, 1 )
endfunction

function CreateGround( toneImg as integer )
	objectShader = LoadShader( "Object.vs", "Object.ps" )
	ground = CreateObjectPlane( 200,200 )
	SetObjectPosition( ground, 0,-0.4,0 )
	SetObjectRotation( ground, 90, 0, 0 )
	SetObjectImage( ground, toneImg, 0 )
	SetObjectShader( ground, objectShader )
	SetObjectCastShadow( ground, 1 )

	for i = 1 to 50
		objType = Random( 0, 3 )
		select objType
			case 0 : CreateObjectBox( i, 10,10,10 ) : endcase
			case 1 : CreateObjectSphere( i, 10,20,20 ) : endcase
			case 2 : CreateObjectCone( i, 10,10,20 ) : endcase
			case 3 : CreateObjectCylinder( i, 10,10,20 ) : endcase
		endselect
		
		// keep objects outside the room
		repeat
			x = Random(0,200)-100
			z = Random(0,200)-100
		until x > 30 or x < -30 or z > 30 or z < -30
		
		SetObjectPosition( i, x, Random(0,7)-2, z )
		SetObjectRotation( i, Random(0,180)-90, Random(0,360), Random(0,180)-90 )
		
		// all objects must have the tone map image, and a shader that knows how to use it
		SetObjectImage( i, toneImg, 0 )
		SetObjectShader( i, objectShader )
		SetObjectCastShadow( i, 1 )
	next i
endfunction

function CreateRoom( toneImg as integer )
	left1 = CreateObjectBox( 1, 40, 20 )
	left2 = CreateObjectBox( 1, 40, 20 )
	left3 = CreateObjectBox( 1, 20, 10 )
	right = CreateObjectBox( 1, 40, 50 )
	front = CreateObjectBox( 50, 40, 1 )
	back = CreateObjectBox( 50, 40, 1 )
	top = CreateObjectBox( 50, 1, 50 )
	ground = CreateObjectPlane( 50,50 )
	
	SetObjectCastShadow( left1, 1 )
	SetObjectCastShadow( left2, 1 )
	SetObjectCastShadow( left3, 1 )
	SetObjectCastShadow( right, 1 )
	SetObjectCastShadow( front, 1 )
	SetObjectCastShadow( back, 1 )
	SetObjectCastShadow( top, 1 )
	SetObjectCastShadow( ground, 1 )
		
	SetObjectPosition( left1, -25, 20, -15 )
	SetObjectPosition( left2, -25, 20, 15 )
	SetObjectPosition( left3, -25, 30, 0 )
	SetObjectPosition( right, 25, 20, 0 )
	SetObjectPosition( front, 0, 20, 25 )
	SetObjectPosition( back, 0, 20, -25 )
	SetObjectPosition( top, 0, 40, 0 )
	SetObjectPosition( ground, 0, -0.01, 0 )
	
	SetObjectRotation( ground, 90, 0, 0 )
	
	// room shader checks the normal of every face, if it points towards the world origin then it ignores the vertex shader lighting
	// thisi s a quick hack to get it to ignore sun and ambient light, shadows would be a better way
	roomShader = LoadShader( "Room.vs", "Room.ps" )
	SetObjectShader( left1, roomShader )
	SetObjectShader( left2, roomShader )
	SetObjectShader( left3, roomShader )
	SetObjectShader( right, roomShader )
	SetObjectShader( front, roomShader )
	SetObjectShader( back, roomShader )
	SetObjectShader( top, roomShader )
	SetObjectShader( ground, roomShader )
	
	// all objects must have the tone map image, and a shader that knows how to use it
	SetObjectImage( left1, toneImg, 0 )
	SetObjectImage( left2, toneImg, 0 )
	SetObjectImage( left3, toneImg, 0 )
	SetObjectImage( right, toneImg, 0 )
	SetObjectImage( front, toneImg, 0 )
	SetObjectImage( back, toneImg, 0 )
	SetObjectImage( top, toneImg, 0 )
	SetObjectImage( ground, toneImg, 0 )
endfunction
