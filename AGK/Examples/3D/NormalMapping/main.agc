// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "NormalMapping" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts
SetGenerateMipmaps( 1 )

// create a spher with a brick texture
CreateObjectSphere( 1, 10, 20, 20 )
LoadImage( 1, "diffuseMap.png" )
SetImageWrapU( 1, 1 )
SetImageWrapV( 1, 1 )
SetObjectImage( 1, 1, 0 )

// load the normal map
LoadImage( 2, "normalMap.png" )
SetImageWrapU( 2, 1 )
SetImageWrapV( 2, 1 )
SetObjectNormalMap( 1, 2 )

// scale the texture 
SetObjectUVScale( 1, 0, 4, 4 )

// create some light
CreatePointLight( 1, 0,0,-6, 200, 255,255,255 )
SetPointLightMode( 1, 1 ) // only pixel lights will be affected by the normal map

// create a light marker object
CreateObjectSphere( 2, 0.5, 10, 10 )
SetObjectLightMode( 2, 0 )

// controls setup
addvirtualbutton( 1, 970,715,100 )
addvirtualbutton( 2, 970,610,100 )
SetVirtualButtonText(1,"Back")
SetVirtualButtonText(2,"Forward")

startx# = GetPointerX()
starty# = GetPointerY()

// button to turn the base texture on and off
addvirtualbutton( 3, 54,715,100 )
SetVirtualButtonText(3,"On/Off")

textureMode = 1

do
    // control the camera
    if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, 0.5 )
	if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -0.5 )

	if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -0.5 )
	if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, 0.5 )

	if ( GetRawKeyState( 81 ) ) then MoveCameraLocalY( 1, -0.5 )
	if ( GetRawKeyState( 69 ) ) then MoveCameraLocalY( 1, 0.5 )
	
	if ( getvirtualbuttonstate(1) = 1 ) then MoveCameraLocalZ( 1, -0.5 )
    if ( getvirtualbuttonstate(2) = 1 ) then MoveCameraLocalZ( 1, 0.5 )
	
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
    
    // toggle the base texture
    if ( GetVirtualButtonPressed(3) = 1 or GetRawKeyPressed(32) ) 
		textureMode = 1 - textureMode
		if ( textureMode = 1 ) 
			SetObjectImage( 1, 1, 0 )
		else
			SetObjectImage( 1, 0, 0 ) 
		endif
	endif
    
    // move the point light
    time# = time# + GetFrameTime()
	x# = Sin( time#*20 ) * 15
	z# = Cos( time#*20 ) * 15
	SetPointLightPosition( 1, x#, 2, z# )
	SetObjectPosition( 2, x#, 2, z# )
	
	Print( "Click and drag to rotate the camera, W, A, S, D, Q, E to move" )
	Print( "Press Space to toggle the base texture" )
    Print( "FPS: " + str(ScreenFPS(),2) )
    Sync()
loop
