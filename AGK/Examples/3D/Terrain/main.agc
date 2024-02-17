 
// set window properties
SetWindowTitle( "Landscape Demo" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize(1)
SetSyncRate( 60,0 )
SetAntiAliasMode(1)

UseNewDefaultFonts( 1 )
 
// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetScissor(0,0,0,0)
SetCameraRange( 1, 0.1, 2000 )

// enable the sky box
SetSkyBoxVisible( 1 )

// load terrain detail texture with mipmapping enabled
SetGenerateMipmaps(1)
LoadImage( 1, "detail.png" )
SetImageWrapU( 1, 1 )
SetImageWrapV( 1, 1 )

// load the terrain shader to give the terrain color based on height
LoadShader( 1, "Terrain.vs", "Terrain.ps" )

// create the terrain object from a height map
CreateObjectFromHeightMap( 1, "Mexico.png", 768, 50, 768, 1, 16 )
SetObjectImage( 1, 1, 0 )
SetObjectUVScale( 1, 0, 128, 128 ) // scale the detail texture so it repeats
SetObjectShader( 1, 1 )

SetCameraPosition(1, 383, 30, 383)
SetCameraLookAt( 1, 0,40,0,0 )

SetAmbientColor( 128,128,128 )

// buttons for mobile devices to control the camera 
addvirtualbutton( 1, GetScreenBoundsRight()-45,GetScreenBoundsBottom()-45,80 )
addvirtualbutton( 2, GetScreenBoundsRight()-45,GetScreenBoundsBottom()-130,80 )
addvirtualbutton( 3, GetScreenBoundsLeft()+45,GetScreenBoundsBottom()-45,80 )
SetVirtualButtonText(2,"Forward")
SetVirtualButtonText(1,"Back")
SetVirtualButtonText(3,"Ground")


SetPrintSize(20)
SetPrintColor( 255,0,0 )

// add some atmospheric fog
SetFogMode( 1 )
SetFogColor( 161,183,209 )
SetFogRange( 50, 700 )
SetFogSunColor( 255,230,179 )

// camera starts in free mode
cameraMode = 0

do
	// move the camera
	speed# = 1.0
	if ( GetRawKeyState( 16 ) ) then speed# = 0.1
	if ( cameraMode = 1 ) then speed# = 0.25
	
	if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, speed# )
	if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -speed# )

	if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -speed# )
	if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, speed# )

	if ( GetRawKeyState( 81 ) ) then MoveCameraLocalY( 1, -speed# )
	if ( GetRawKeyState( 69 ) ) then MoveCameraLocalY( 1, speed# )
	
	if ( getvirtualbuttonstate(1) = 1 ) then MoveCameraLocalZ( 1, -speed# )
    if ( getvirtualbuttonstate(2) = 1 ) then MoveCameraLocalZ( 1, speed# )
	
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
    
    // change camera mode
    if ( GetVirtualButtonPressed(3) = 1 ) then cameraMode = 1-cameraMode
    if ( cameraMode = 1 )
		// stick camera to the terrain
		x# = GetCameraX(1)
		z# = GetCameraZ(1)
		SetCameraPosition( 1, x#, GetObjectHeightMapHeight(1,x#,z#)+0.5, z# )
	endif
    
    // move the sun 
    time# = time# + GetFrameTime()*0.5
    xdir# = sin(time#)
    ydir# = -0.5
    zdir# = cos(time#)
    SetSunDirection( xdir#, ydir#, zdir# )
    
	// show some information
	Print( "FPS: " + str(ScreenFPS(),1) )
	Print( "Polygons: " + str(GetPolygonsDrawn()) )
	
    Sync()
loop
