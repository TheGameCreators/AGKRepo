
// set window properties
SetWindowTitle( "Bone Animation" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetScissor(0,0,0,0)
SetClearColor( 101,120,154 )
SetCameraRange(1,10,2000)

UseNewDefaultFonts( 1 )
SetPrintSize( 10 )
SetPrintColor( 255,255,255 )

SetAmbientColor( 128,128,128 )
SetSunColor( 255,255,255 )

LoadObjectWithChildren(1,"Original_Soldier.X")
LoadImage(1,"Soldier_body_1_D.png")
setobjectimage(1,1,0)

CreatePointLight( 1, 0,50,50, 100, 255,0,0 )

// texture gun
LoadImage(2,"gun_D.png")
setobjectimage( GetObjectChildID(1,2), 2, 0 )

// hide fire spot object, a cube that was added to the model to show the gun anchor point
SetObjectVisible( GetObjectChildID(1,1), 0 )

SetCameraPosition( 1,0,75,-100 )
SetCameraLookAt( 1,0,37,0,0 )

addvirtualbutton( 1, 970,715,100 )
addvirtualbutton( 2, 970,610,100 )
SetVirtualButtonText(2,"Forward")
SetVirtualButtonText(1,"Back")


addvirtualbutton( 3, 50,715,100 )
SetVirtualButtonText(3,"Shoot")

// play idle animation
PlayObjectAnimation( 1, "", 1000, 1282, 1, 0 )
SetObjectAnimationSpeed( 1, 20 )

/*
// some other animations
PlayObjectAnimation( 1, "", 1000, 1282, 1, 0 )
PlayObjectAnimation( 1, "", 3744, 3828, 1, 0 )
PlayObjectAnimation( 1, "", 685, 707, 1, 0 )
PlayObjectAnimation( 1, "", 2160, 2216, 1, 0 )
*/

// get the bone that represents the head
// turn off animation on it so we can move it ourselves
headBone = GetObjectBoneByName( 1, "Bip01_Head" )
SetObjectBoneCanAnimate( 1, headBone, 0 )

SetPrintSize(15)

do
	// hold shift to move slower
	speed# = 4
	if ( GetRawKeyState(16) ) then speed# = 0.5
	
    // control the camera with WASD
    if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, speed# )
	if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -speed# )

	if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -speed# )
	if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, speed# )

	if ( GetRawKeyState( 81 ) ) then MoveCameraLocalY( 1, -speed# )
	if ( GetRawKeyState( 69 ) ) then MoveCameraLocalY( 1, speed# )
	
	if ( getvirtualbuttonstate(1) = 1 ) then MoveCameraLocalZ( 1, -speed# )
	if ( getvirtualbuttonstate(2) = 1 ) then MoveCameraLocalZ( 1, speed# )
	
	// play death animation, transition over 0.3 seconds
	if ( GetRawKeyPressed( 32 ) or getvirtualbuttonpressed(3) )
		SetObjectBoneCanAnimate( 1, headBone, 1 )
		PlayObjectAnimation( 1, "", 4971, 5021, 0, 0.3 )
	endif
	
	// if the death animation has stopped go back to original animation, transition over 0.5 seconds
	if ( GetObjectIsAnimating(1) = 0 ) 
		SetObjectBoneCanAnimate( 1, headBone, 0 )
		PlayObjectAnimation( 1, "", 1000, 1282, 1, 0.5 )
	endif
	
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
    
    // rotate the head to always look at the camera
    // head is backwards on the bone so rotate the vector by 180 degrees
    bonex# = 2*GetObjectBoneWorldX( 1, headBone ) - GetCameraX(1)
    boney# = 2*GetObjectBoneWorldY( 1, headBone ) - GetCameraY(1)
    bonez# = 2*GetObjectBoneWorldZ( 1, headBone ) - GetCameraZ(1)
    setObjectBoneLookAt( 1, headBone, bonex#, boney#, bonez#, 0 )
    
    Print( "FPS: " + str(ScreenFPS()) )
    Sync()
loop
