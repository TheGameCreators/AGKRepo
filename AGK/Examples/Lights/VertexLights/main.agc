// Full screen bloom shader demo

setvirtualresolution(1024,768)
SetClearColor( 0,0,0 )
SetScissor(0,0,0,0)
SetGenerateMipmaps(1)
SetPrintSize( 20 )
SetCameraRange( 1, 1, 1000 )

// nice fonts
UseNewDefaultFonts( 1 )


// AGK supports 8 vertex lights so we could add another if we had another color
color1 as integer[2] = [  0,255,  0]
color2 as integer[2] = [255,255,  0]
color3 as integer[2] = [255,  0,  0]
color4 as integer[2] = [255,  0,255]
color5 as integer[2] = [  0,  0,255]
color6 as integer[2] = [  0,255,255]
color7 as integer[2] = [255,255,255]

colors as integer[7,2]
colors[1] = color1
colors[2] = color2
colors[3] = color3
colors[4] = color4
colors[5] = color5
colors[6] = color6
colors[7] = color7

// load a 3D object
LoadImage( 1, "Models/chapel_D.png" )
LoadObject( 1, "Models/chapel.obj", 10 )
SetObjectImage( 1, 1, 0 )

// remove directional light, and reduce ambient
SetAmbientColor( 20,20,20 )
SetSunActive(0)

// create 7 vertex lights
for i=1 to 7
	posx# = sin( 360.0/7.0 * (i-1) ) * 8
	posz# = cos( 360.0/7.0 * (i-1) ) * 8
	CreatePointLight( i, posx#,2,posz#, 40, colors[i,0], colors[i,1], colors[i,2] )

	// make light marker so we can see where it is
	CreateObjectSphere( i+10, 0.4, 10, 10 )
	SetObjectColor( i+10, colors[i,0], colors[i,1], colors[i,2], 255 )
	SetObjectLightMode( i+10, 0 ) // light spheres shouldn't be affected by light
	SetObjectPosition( i+10, posx#,2,posz# )
next i

// point camera towards the object
SetCameraPosition( 1, 15,10,0 )
SetCameraLookAt( 1, 0,5,0,0 )

// controls setup
addvirtualbutton( 1, 970,715,100 )
addvirtualbutton( 2, 970,610,100 )

SetVirtualButtonText(2,"Forward")
SetVirtualButtonText(1,"Back")

startx# = GetPointerX()
starty# = GetPointerY()
time# = 0

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

    // move the lights
    for i = 1 to 7
		x# = Sin( timer()*35 + 360.0/7.0 * (i-1) ) * 8
		z# = Cos( timer()*35 + 360.0/7.0 * (i-1) ) * 8
		SetPointLightPosition( i, x#, 2, z# )
		SetObjectPosition( i+10, x#, 2, z# )
	next i

	SetPrintsize(26)
	print( "Click and drag to rotate the camera, W, A, S, D, Q, E to move" )
    print( screenfps() )

    Sync()
loop
