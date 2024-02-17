
setvirtualresolution(1024,768)
SetClearColor( 151,170,204 )
SetScissor(0,0,0,0)
SetGenerateMipmaps(1)
SetPrintSize( 20 )
UseNewDefaultFonts( 1 )

CreateRenderImage( 2, 512, 512, 0, 0 )

CreateObjectPlane( 7, 5,5 )
SetObjectLightMode( 7, 0 )
SetObjectPosition( 7, 0,0,7 )
RotateObjectGlobalY( 7, 0 )
SetObjectColor( 7, 240,240,240, 255 )

// controls setup
addvirtualbutton( 1, 970,715,100 )
addvirtualbutton( 2, 970,610,100 )
SetVirtualButtonText(1,"Back")
SetVirtualButtonText(2,"Forward")

startx# = GetPointerX()
starty# = GetPointerY()

do
    // control the camera
    if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, 0.2 )
    if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -0.2 )

	if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -0.2 )
	if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, 0.2 )

	if ( GetRawKeyState( 81 ) ) then MoveCameraLocalY( 1, -0.2 )
	if ( GetRawKeyState( 69 ) ) then MoveCameraLocalY( 1, 0.2 )

    if ( getvirtualbuttonstate(1) = 1 ) then MoveCameraLocalZ( 1, -0.2 )
    if ( getvirtualbuttonstate(2) = 1 ) then MoveCameraLocalZ( 1, 0.2 )

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
	SetPrintSize(26)
    print( "Click and drag to rotate the camera, W, A, S, D, Q, E to move" )
    print( screenfps() )

    // update the scene, should only do this once
    Update(0)

    // render to image 2
    SetRenderToImage( 2, -1 ) // -1 for depth to use an internal buffer, we could set an image if we wanted (not supported on all devices)
    ClearScreen() // clear the image before drawing to it
    SetObjectImage( 7, 0, 0 ) // clear the image on our plane so we don't get visual corruption
    Render() // draw the entire scene, we could pick and choose what we wanted to draw to the image instead

    // render to screen
    SetRenderToScreen()
    SetObjectImage( 7, 2, 0 ) // apply the rendered image to object 7
    // don't use Sync() here as it will call Update() again.
    Render()
    Swap()
loop
