// Project: Space Demo 
// Created: 2014-10-15


//-- This demo demonstrates how to achieve a selective bloom effect.

//-- Bloom is an effect which allows objects to bleed out beyond their bounds, this is perfect 
//-- for creating the illusion of something glowing really bright. If you look at the sun through 
//-- some branches of a tree it can often be the case that the sun blocks out the branches even though
//-- they are in the foreground, this is the effect we achieve with bloom.

//-- The standard bloom example takes what it interprets as the brightest parts of 
//-- your scene and applies bloom to these areas. This is a good general way to achieve
//-- bloom but can not give sufficient control in some circumstances.

//-- You may desire that bloom is only applied on direct light sources such as any light
//-- bulbs in your scene, or just the sun. In our example we draw a sun which needs the bloom to be 
//-- so extreme that we would not want to apply it to any other objects, this is because the extreme 
//-- blurring of the effect is also required to give the sun a lava surface effect.
//-- This requires a more controlled and selective approach to bloom.

//-- To achieve this we first draw the whole scene, any objects with bloom get drawn normally,
//-- all other objects get drawn black. We need to draw these other objects in case they are
//-- in front of an object with bloom, this allows them to effectively block out or mask some 
//-- places where bloom should not be applied.

//-- The rest of the effect is the same as the standard bloom example. We reduce this
//-- first image/scene we just created to a much smaller image, we then apply horizontal
//-- and then vertical blurring. The final step is to combine the scene drawn completely nomally
//-- with our final bloom image/scene, this results in a scene with bloom applied selectively to
//-- certain objects. 

// set window properties
SetWindowTitle( "Space Demo" )
SetScissor(0,0,0,0)


// set display properties
SetCameraRange( 1, 1.0, 50000 )
SetCameraPosition( 1, 0.0, 0.0, -1000 )
SetCameraLookAt( 1, 0.0, 0.0, 0.0, 0.0 )

//-- LIGHTS --//
//-- sun stays positioned at 0,0,0 so we position our point light here,
//-- for this demo the point light effects the planet orbiting the sun 
CreatePointLight( 1, 0.0, 0.0, 0.0, 750000.0, 255, 255, 255 )

//-- IMAGES --//
//-- render target images
imgRenderBloomMask  = CreateRenderImage( 1024, 1024, 0, 0 ) // initial render with just bloom objects drawn
imgRenderRed1      	= CreateRenderImage( 512, 512, 0, 0 )   // reduction 1 512
imgRenderRed2      	= CreateRenderImage( 256, 256, 0, 0 )   // reduction 2 256
imgRenderRed3      	= CreateRenderImage( 128, 128, 0, 0 )   // reduction 3 128
imgRenderBlurHor   	= CreateRenderImage( 128, 128, 0, 0 )   // blur horizontal 128
imgRenderBlurVert  	= CreateRenderImage( 128, 128, 0, 0 )   // blur vertical 128
imgRenderStandard   = CreateRenderImage( 1024, 1024, 0, 0 ) // full scene rendered normally
imgRenderFinal     	= CreateRenderImage( 1024, 1024, 0, 0 ) // final image real scene with blurred image overlayed
//-- loaded images
//-- lava image created in gimp: Filters->Render->Lava then Filters->Noise->HSV Noise, you can create some great
//-- effects for stars and planets using these filters with different colours/values.
imgSurface = LoadImage( "lava.png" )
//-- the uv values need to be wrapped since the shader increases the uv values with time
SetImageWrapU( imgSurface, 1 )
SetImageWrapV( imgSurface, 1 )

//-- EFFECTS/MATERIALS --//
//-- load the shaders for each stage, 
//-- quad.vs      - does a simply vertex transformation to make the 3D plane perfectly fit the entire screen
//-- standard.vs  - texture only
//-- sun.vs       - 2 texture coordinates varied with timer
//-- planet.vs    - single point light
//-- standard.ps  - texture only
//-- blur_hor.ps  - texture, blurred horizontally 
//-- blur_vert.ps - texture, blurred vertically
//-- black.ps     - colour object black
//-- overlay.ps   - two textures, blended together
//-- sun.ps       - 2 texture coordinates blended together
//-- planet.ps    - ambient light and point light
fxReduction = LoadShader( "quad.vs", "standard.ps" ) // same shader for all three reduction steps
fxBlurHor   = LoadShader( "quad.vs", "blur_hor.ps" )
fxBlurVert  = LoadShader( "quad.vs", "blur_vert.ps" )
fxBlack     = LoadShader( "standard.vs", "black.ps" )
fxStandard  = LoadShader( "standard.vs", "standard.ps" )
fxOverlay   = LoadShader( "quad.vs", "overlay.ps" )
fxSun       = LoadShader( "sun.vs", "sun.ps" )
fxPlanet    = LoadShader( "planet.vs", "planet.ps" )

//-- OBJECTS --//
// make a plane to display images to the screen so they can be modified by shaders.
// planes used for this purpose are generally called quads, AGK has a special Quad primitive
// that means it can take some shortcuts in the rendering pipeline, Quads will never be
// rendered to the screen unless you specifically draw it with DrawObject()
objQuad = CreateObjectQuad()
//-- planet object with simple diffuse colour/point light shader
objPlanet = CreateObjectSphere( 25, 24, 24 )
SetObjectColor( objPlanet, 100, 125, 255, 255 )
SetObjectShader( objPlanet, fxPlanet )
//-- sun object with shader which blends the same texture with itself using different
//-- uv coordinates, the effect is complete when the bloom effect blurs the result.
objSun = CreateObjectSphere( 500, 24, 24 )
SetObjectImage(objSun, imgSurface, 0 )
SetObjectShader( objSun, fxSun )

//-- we move the planet around the sun using the timer so we must be sure we
//-- are starting at 0.0
ResetTimer()

//-- we keep track of last camera rotations so we can smooth the camera movement 
lax# = 0.0
lay# = 0.0

do
	//-- exit if pointer pressed
	if GetPointerPressed() = 1
		exit
	endif
	
	//-- movement is timer based, so consistent when run using different frame rates 
	time# = Timer()
	
	//-- position planet in orbit around the sun, speed is determined my multiplier inside bracket, distance from
	//-- the sun is determined by multiplier outside the bracket
	x# = sin( -time# * 6.0 ) * 600.0
	z# = cos( -time# * 6.0 ) * 600.0
	SetObjectPosition( objPlanet, x#, 0.0, z# )	

	//-- Camera controlled by accelerometer or mouse
    if GetAccelerometerExists() = 1
		//-- camera is static but user can look around if they lay their device flat
		if time# < 4.0
			Print( "place device flat" )
		endif 
		ax# = GetRawAccelX()	
		ay# = GetRawAccelY()
		//-- smooth the values by factor of 0.25
		sax# = lax# + (ax# - lax#)*0.25
		say# = lay# + (ay# - lay#)*0.25
		lax# = sax#
		lay# = say#
		camy# = -sax# * 180
		camx# = -say# * 180
		SetCameraRotation( 1, camx#, camy#, 0 )
    endif
    
	//-- flying camera, starts close to the planet and moves backwards
	//SetCameraPosition( 1, GetObjectX(objPlanet), GetObjectY(objPlanet), GetObjectZ(objPlanet) - 75.0 - time#*time# )

    // update the scene, this should only be called once
    Update(0)

    //-- draw our bloom mask
		SetRenderToImage( imgRenderBloomMask, -1 )
		ClearScreen()
		//-- anything with bloom gets drawn normally
		DrawObject( objSun )
		//-- everything that does not require bloom gets drawn black
		SetObjectShader ( objPlanet, fxBlack )
		DrawObject( objPlanet )
		SetObjectShader( objPlanet, fxPlanet )  //change this to normal shader with lighting etc

	//-- reduce our bloom mask
	    //-- set the image to quad object so we can draw it
		SetObjectImage( objQuad, imgRenderBloomMask, 0 )
		SetObjectShader( objQuad, fxReduction )
		//-- reduce to 512x512
		SetRenderToImage( imgRenderRed1, 0 )
		ClearScreen()
        DrawObject( objQuad )
        
        //-- set the newer (512x512) image to quad object so we can draw it
		SetObjectImage( objQuad, imgRenderRed1, 0 )
		SetObjectShader( objQuad, fxReduction )
        //-- reduce to 256x256
        SetRenderToImage( imgRenderRed2, 0 )
        ClearScreen()
        DrawObject( objQuad )
        
        //-- set the newer (256x256) image to quad object so we can draw it
		SetObjectImage( objQuad, imgRenderRed1, 0 )
		SetObjectShader( objQuad, fxReduction )
        //-- reduce to 128x128
        SetRenderToImage( imgRenderRed3, 0 )
        ClearScreen()
        DrawObject( objQuad )
  
    //-- blur our smallest image (128x128), 
  	    //-- set the smallest image to quad object
		SetObjectImage( objQuad, imgRenderRed3, 0 )  
		//-- HOR
		SetObjectShader( objQuad, fxBlurHor )
        //-- set to blur image
        SetRenderToImage( imgRenderBlurHor, 0 )
        ClearScreen()
        DrawObject( objQuad )  
  		//-- VERT
  		SetObjectImage( objQuad, imgRenderBlurHor, 0 ) 
		SetObjectShader( objQuad, fxBlurVert )
        //-- we are still set to draw to smallest image
        SetRenderToImage( imgRenderBlurVert, 0 )
        ClearScreen()
        DrawObject( objQuad )         
    
        //-- render scene normally, and to internal depth buffer (-1)
        SetRenderToImage( imgRenderStandard, -1 )
        ClearScreen()
        Render()  
    
    //-- render normal scene above with blurred scene overlayed
        SetObjectImage( objQuad, imgRenderStandard, 0 )
        SetObjectImage( objQuad, imgRenderBlurVert, 1 )         
        SetObjectShader( objQuad, fxOverlay )
        SetRenderToScreen()
        ClearScreen()       
        DrawObject( objQuad )
 
    //-- don't use Sync() here as it will redraw the entire scene again,
    //-- we have already drawn the scene and post processed it
    Swap()	
	
loop








