// Full screen bloom shader demo

setvirtualresolution(1024,768)
SetClearColor( 101,120,154 )
SetScissor(0,0,0,0)
SetGenerateMipmaps(1)
SetPrintSize( 20 )
setcamerarange(1,1,1000)
SetWindowAllowResize(1)

// nice fonts
UseNewDefaultFonts( 1 )

// load a 3D object
LoadImage( 1, "Models/chapel_D.png" )
LoadObject( 1, "Models/chapel.obj", 10 )
SetObjectImage( 1, 1, 0 )

// create some light
CreatePointLight( 1, 0,0,-6, 100, 0,255,0 )

// create a light marker object
CreateObjectSphere( 2, 1, 10, 10 )
SetObjectColor( 2, 0,255,0,128 )
SetObjectTransparency(1,0)
SetObjectTransparency(2,0)

// point camera towards the object
SetCameraPosition( 1, 15,10,0 )
SetCameraLookAt( 1, 0,5,0,0 )

width = GetDeviceWidth()
height = GetDeviceHeight()

// create some intermediate images for the full screen shader
CreateRenderImage( 2, width, height, 0, 0 ) // initial render
CreateRenderImage( 3, width/2, height/2, 0, 0 ) // reduction 1
CreateRenderImage( 4, width/4, height/4, 0, 0 ) // reduction 2
CreateRenderImage( 5, width/8, height/8, 0, 0 ) // reduction 3
CreateRenderImage( 6, width/8, height/8, 0, 0 ) // blur 1
// we then blur a second time with image 5 as the output, as it is the right size and we don't need it anymore
// image 5 (blurry) is then blended with image 2 (sharp) to produce the final output shown on the screen

// create some colored planes to show some light bleeding
CreateObjectPlane( 6, 5,5 )
SetObjectLightMode( 6, 0 )
SetObjectPosition( 6, 0,7.5,7 )
RotateObjectGlobalY( 6, 90 )
SetObjectColor( 6, 255,0,0,255 )

CreateObjectPlane( 7, 5,5 )
SetObjectLightMode( 7, 0 )
SetObjectPosition( 7, 0,1.5,7 )
RotateObjectGlobalY( 7, 90 )

// load the shaders for each stage, Quad.vs does a simply vertex transformation to make the 3D plane perfectly fit the entire screen
LoadShader( 1, "Quad.vs", "Initial.ps" )
LoadShader( 2, "Quad.vs", "Reduction.ps" ) // same shader for all three reduction steps
LoadShader( 3, "Quad.vs", "Blur1.ps" )
LoadShader( 4, "Quad.vs", "Blur2.ps" )
LoadShader( 5, "Quad.vs", "Bloom.ps" ) // combines image 5 and 2 into the final output

// make a plane to display images to the screen so they can be modified by shaders.
// planes used for this purpose are generally called quads, AGK has a special Quad primitive
// that means it can take some shortcuts in the rendering pipeline, Quads will never be
// rendered to the screen unless you specifically draw it with DrawObject()
CreateObjectQuad( 8 )

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

    // move the green light
    time# = time# + GetFrameTime()
	x# = Sin( time#*70 ) * 7
	z# = Cos( time#*70 ) * 7
	SetPointLightPosition( 1, x#, 2, z# )
	SetObjectPosition( 2, x#, 2, z# )
	SetPrintSize(26)
	print( "Click and drag to rotate the camera, W, A, S, D, Q, E to move" )
    print( screenfps() )
    
    // update the scene, this should only be called once
    Update(0)

    // full screen bloom process
    // first Render() as normal to image 2
    SetRenderToImage( 2, -1 ) // render into image 2, using an internal depth buffer
	ClearScreen() // make sure the image is clean
	Render() // draw the scene as normal

    // do some post processing on our newly rendered image 2
    SetObjectImage( 8, 2, 0 ) // apply image 2 to the quad
	SetObjectShader( 8, 1 ) // set our first shader to the quad, this does some processing on the image to make it suitable for bloom
	SetRenderToImage( 3, 0 ) // we will output to image 3 which is half the size, so this is also a reduction whilst shader 1 does some processing
	ClearScreen() // clean image 3
	DrawObject( 8 ) // draw the quad and nothing else, this makes it nice and fast

    // take our newly rendered image 3 and reduce it further
	SetObjectImage( 8, 3, 0 ) // apply image 3 to the quad, this removes image 2 which will remain unchanged from now on
	SetObjectShader( 8, 2 ) // set the reduction shader which does nothing but reduce the image size
	SetRenderToImage( 4, 0 ) // output to image 4 which is half the size of image 3
	ClearScreen() // clean image 4
	DrawObject( 8 ) // draw the quad and nothing else

    // take our newly rendered image 4 and reduce it further
	SetObjectImage( 8, 4, 0 ) // apply image 4 to the quad
	SetObjectShader( 8, 2 ) // added for clarity, the quad is already using shader 2
	SetRenderToImage( 5, 0 ) // output to image 5 which is half the size of image 4
	ClearScreen() // clean image 5
	DrawObject( 8 ) // draw the quad and nothing else

    // we now have a low quality 128x128 copy of our scene in image 5, lets blur this so it creates a nice bloom effect

	SetObjectImage( 8, 5, 0 ) // apply image 5 to the quad
	SetObjectShader( 8, 3 ) // set the first blur shader, this does a horizontal blur across the pixels
	SetRenderToImage( 6, 0 ) // output to image 6, which is the same size as image 5
	ClearScreen() // clean image 6
	DrawObject( 8 ) // draw the quad and nothing else

	SetObjectImage( 8, 6, 0 ) // apply image 6 to the quad
	SetObjectShader( 8, 4 ) // set the second blur shader, this does a vertical blur across the pixels to finish the blur effect
	SetRenderToImage( 5, 0 ) // output to image 5 as it is no longer being used and is the right size for this step
	ClearScreen() // clean image 5
	DrawObject( 8 ) // draw the quad and nothing else

	// we now have a very blurry image 5 and a nice sharp image 2 from the very beginning (plus some intermediate
    // images 3,4,6 that we don't need anymore), we can combine image 5 and 2 to create a nice bloom effect

    SetObjectImage( 8, 2, 0 ) // set image 2 to texture stage 0 on the quad
	SetObjectImage( 8, 5, 1 ) // set image 5 to texture stage 1 on the quad
	SetObjectShader( 8, 5 ) // apply the bloom shader to the quad, this will do the work of blending image 5 and image 2
	SetRenderToScreen() // send this output to the screen, this is the only thing the user will see from the entire process
	DrawObject( 8 ) // draw the quad and nothing else, remember image 2 already contains our original scene so we don't need to draw that again

    // don't use Sync() here as it will redraw the entire scene again, we have already drawn the scene and post processed it
    Swap()
loop
