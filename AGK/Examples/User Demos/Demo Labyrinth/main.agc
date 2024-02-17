
// Project: Demo Labyrinth 
// Created: 2014-10-15

// set window properties
SetWindowTitle( "Demo Labyrinth" )
SetScreenResolution(800,600)

// set display properties
SetVirtualResolution(100,100)
// set the allowed orientation, we want only the landscape orientation, so set it to 1
SetOrientationAllowed(0,0,1,0)
// Un-Comment it to see the app running with full speed
// (but you should not notice anny big speed chages in movment, because we will use time based movment)
// SetSyncRate(0,0)

//****************************<UDT & Array>****************************
// we create an UDT(user defined type) wich for now only contains an 3 dimensional Array
// with a size of X=100 Y=1 and Z=100, just enough to hold our World data
// beware you can use the index 0 too, so you have acces to 0-100, 101 indices in number.
type LevelData
	Cube as integer[100,1,100]
endtype

// here we create an array of LevelData with the size of 0
// because we dont know how large it have to be and resize it to our needs later
// make the array global so we can use it in the whole project
global Level as LevelData[0]

//****************************<Sky>****************************
// load an Image wich is in our Media/Images/ folder and is called skysphere.jpg
SkySphereImageID=loadimage("Images/skysphere.jpg")
// now this is our first 3D object and represents our skysphere
// make it very large so you don't notice that it is just a sphere.
// 4 rows and 8 columns
SkySphereObjectID=createobjectsphere(999,4,8)
// give it the previously loaded image
// the last parameter is the stage index we use 0 to texture the object
// stage indices over 0 is primarily used to work with shaders
setobjectimage(SkySphereObjectID,SkySphereImageID,0)
// the cull mode is very important for our skysphere
// if we don't set it to 0 we won't see the sky
// with Culling you determine if you can see the face or not, if the normal on the polygon (wich is made out of vertices) is facing away it will not get rendered
SetObjectCullMode(SkySphereObjectID,0)
// and to be sure we cand colide with the sky set the collision mode to 0
SetObjectCollisionMode(SkySphereObjectID,0)

//****************************<Load some Images>****************************
// here we load the image to texure the cubes and set the ImageID global
// the "_C" in the file name stands for Color
global CubeImageID
CubeImageID=loadimage("Images/Cube_C.png")

// the "_I" in the file name stands for Illuminance, wich our shaders needs, to know were not to darken the cubes, but i'll explain that later
global IluminanceImageID
IluminanceImageID=loadimage("Images/Cube_I.png")

// the "_D" in the file name stands for Desintegrate, dark pixels disappear first
global DesintegrateImageID
DesintegrateImageID=loadimage("Images/Cube_D.png")

//****************************<Load Shader>****************************
// here we load the shader wich is applied first to the cubes
// it light the cubes and calculate the area where not to light/darken it
global CubeShaderID
CubeShaderID=LoadShader("Shader/Light.vs", "Shader/Light.ps" )
// pass "PLightCol" to the shader with the parameters 64,128,192 wich makes a lightblue light
setShaderConstantByName(CubeShaderID,"PLightCol",64,128,192,0)

// this is the shader wich is applied to the cubes if you pass a level
// we pass the "PLightCol" and "DiscardDistance" for the little blue area wich makes a kind of halo effect
global DesintegrateShaderID
DesintegrateShaderID=LoadShader("Shader/Light.vs", "Shader/DesintegrateLight.ps" )
setShaderConstantByName(DesintegrateShaderID,"PLightCol",64,128,192,0)
SetShaderConstantByName(DesintegrateShaderID,"DiscardDistance",0.2,0,0,0)


//****************************<Create the ball>****************************
// first we load images again, the Color and the Illuminance Image for the ball
BallImageID=loadimage("Images/Ball_C.png")
BallIluminanceImageID=loadimage("Images/Ball_I.png")

// just create the ball with a size of a half unit make it not collide and set the images
global BallObjectID
BallObjectID=CreateObjectSphere(0.5,8,16)
setobjectcollisionmode(BallObjectID,0)
// we apply the images to the ball, like we did it before
SetObjectImage(BallObjectID,BallImageID,0)
setobjectimage(BallObjectID,BallIluminanceImageID,1)
// now we give the ball our shader, the shader without the discarding feature
SetObjectShader(BallObjectID,CubeShaderID)

//****************************<Settings>****************************
// here we call the function ReadWorld() and pass the parameters "Level/Level.txt"
// you will find it below in the Functions section
// functions are great for repeated code wich you don't write again and again
// or just for simplicity and clarity in this case
ReadWorld("Level.txt")

// if we play the demo on windows-PC we want Bloom if not we better avoid it because it is expensive
if GetDeviceBaseName()="windows" then InitBloom()

// some starting settings
CameraDistance#=7
PlayerSpeed#=10
Gravity#=-8

// the position of our ball at the beginning
BallX#=1
BallY#=1
BallZ#=1

// here we capture the time to calculate the time needed to solve the mazes
StartTime#=Timer()
//****************************<Main Loop>****************************
// After some setups needed for the game we come to the main loop
do
	// just print some text and the framerate
	print ("Swipe over the screen to rotate the Camera")
	print ("Use the Arrowkeys/Tilt the device to move")
	print ("Find the Exit")
	print (ScreenFPS())
	// the frame time is very usefull
	// multiply it with everithing that moves or chnages over time
	// so we make sure even on low frame rates it changes in the same time as it would on heigh frame rates
    FrameTime#=GetFrameTime()
    
    // here we get a snapshot of the pointer at the beginning of our click/touch
    if GetPointerPressed()=1
		PointerStartX#=GetPointerX()
		PointerStartY#=GetPointerY()
    endif
    
    if GetPointerState()=1
		PointerX#=GetPointerX()
		PointerY#=GetPointerY()
		//calculate the distance the pointer traveled while pressing
		PointerDistX#=PointerX#-PointerStartX#
		PointerDistY#=PointerY#-PointerStartY#
		// here we calculate if we swipe vertically or horizontally
		if abs(PointerDistX#)>abs(PointerDistY#)
			// change the camera angle in dependence of the frametime
			CameraAngleY#=CameraAngleY#-PointerDistX#*1.5*FrameTime#
		else
			// change the camera distance in dependence of the frametime
			CameraDistance#=CameraDistance#-PointerDistY#*0.1*FrameTime#
		endif
    endif
    
    //****************************<Ball & Camera positions>****************************
    //sin and cos of the camera angle
	Sin#=sin(CameraAngleY#)
	Cos#=cos(CameraAngleY#)
	// calculate the x and z component for the ball movement multiplyed by the Speed
	MoveZ1#=(GetDirectionY()*Cos#)*PlayerSpeed#
	MoveX1#=(GetDirectionY()*Sin#)*PlayerSpeed#
	MoveZ2#=(GetDirectionX()*Sin#)*PlayerSpeed#
	MoveX2#=(GetDirectionX()*-Cos#)*PlayerSpeed#
    
    // curvevalue provides a smooth movement
    // here we calculate the velocity, the destination velocity is reached in 10 steps
	VelocityX#=curvevalue((MoveX1#+MoveX2#)*FrameTime#,VelocityX#,10)
	VelocityY#=curvevalue(Gravity#*FrameTime#,VelocityY#,10)
	VelocityZ#=curvevalue((MoveZ1#+MoveZ2#)*FrameTime#,VelocityZ#,10)

	// capture the old Ball position
	OldBallX#=BallX#
	OldBallY#=BallY#
	OldBallZ#=BallZ#
	// apply the velocity
	BallX#=BallX#+VelocityX#
	BallY#=BallY#+VelocityY#
	BallZ#=BallZ#+VelocityZ#
	
	
	//****************************<Collision>****************************
	// The variable BallOnGround helps us to keep track of being on the ground(set to 1) or not(set to 0)
	// Here we define the default value to be 0 since we don't know if we are on the ground or not at this point in the code
	BallOnGround=0
	// At this point we code the collision... all we need is to call the command ObjectSphereSlide and get the resulting positions on the x,y and z axis
	// ObjectSphereSlide needs the Old position, the new position and the radius of our ball 
	// we can set the first parameter to 0 to check the collision for all objects
	if ObjectSphereSlide(0,OldBallX#,OldBallY#,OldBallZ#,BallX#,BallY#,BallZ#,0.25)>0
		// at this point we get the destination position when everything goes well
		BallX#=GetObjectRayCastSlideX(0)
		BallY#=GetObjectRayCastSlideY(0)
		BallZ#=GetObjectRayCastSlideZ(0)
		
		for Index=0 to 3
			// if the ball hits an object and the normal of the collision point is larger than 0.5,
			// means that the ground where the ball hits the object is more horizontally than vertically, we set BallOnGround variable to 1
			if GetObjectRayCastNormalY(Index)>0.5 then BallOnGround=1
		next
	endif
	if BallOnGround=0
		// if we don't touch the ground we want the ball not to change the X and Z position
		BallX#=OldBallX#
		BallZ#=OldBallZ#
	endif
	
	// here we set the position skysphere and rotate it, we use the frametime for timer based movment again
	SkySphereAngleX#=SkySphereAngleX#+0.6*FrameTime#
	SkySphereAngleY#=SkySphereAngleY#+0.4*FrameTime#
	SetObjectPosition(SkySphereObjectID,BallX#,BallY#,BallZ#)
	SetObjectRotation(SkySphereObjectID,SkySphereAngleX#,SkySphereAngleY#,0)
	
	// set the final ball position
	SetObjectPosition(BallObjectID,BallX#,BallY#,BallZ#)
	// set the light position
	SetShaderConstantByName(CubeShaderID,"PLightPos",BallX#,BallY#,BallZ#,0.005)
	// call RotateBall wich calculates the ball angle
	RotateBall(BallX#-OldBallX#,BallZ#-OldBallZ#)
	
	// calculate the camera position, with curvevalue for the smothe movement
	CameraX#=curvevalue(sin(CameraAngleY#),CameraX#,3)
	CameraZ#=curvevalue(cos(CameraAngleY#),CameraZ#,3)
	setcameraposition(1,BallX#+CameraDistance#*0.6*CameraX#,BallY#+CameraDistance#,BallZ#+CameraDistance#*0.6*CameraZ#)
	// set the camera to look at the balls position
	setcameralookat(1,BallX#,BallY#,BallZ#,0)
	
	//****************************<Level changes>****************************
	// if we fall throught the hole in the current level we change the level
	// falling is a change of the y postion in a negative direction
	// and in 2 units is the next level, so we can calculate the current level
	// trunc cuts of the decimals
	LevelID=trunc(BallY#/-2)
	// if the level changes and we don't fall throught all levels (Level.Length is the array size of Level)
	if LevelID<>OldLevelID and LevelID<=Level.Length
		OldLevelID=LevelID
		// Reset the Parameter for the discard shader
		DiscardColor#=0
		
		// set the completet level the discard shader
		// Cube.length is the array size of the first index (X)
		for X=0 to Level[LevelID].Cube.length
			// we only have a world height of 2 so 0 to 1
			for Y=0 to 1
				// Cube[0,0].length is the array size of the last index (Z)
				for Z=0 to Level[LevelID].Cube[0,0].length
					if getobjectexists(Level[LevelID].Cube[X,Y,Z])=1
						// here we make use of the stage idex 2
						setobjectimage(Level[LevelID].Cube[X,Y,Z],DesintegrateImageID,2)
						// we need the cubes to be transparent for the discard shader
						SetObjectTransparency(Level[LevelID].Cube[X,Y,Z],1)
						setobjectshader(Level[LevelID].Cube[X,Y,Z],DesintegrateShaderID)
					endif
				next Z
			next Y
		next X
		
		// delete the level before the completed one
		for X=0 to Level[LevelID].Cube.length
			for Y=0 to 1
				for Z=0 to Level[LevelID].Cube[0,0].length
					if getobjectexists(Level[LevelID-1].Cube[X,Y,Z])=1
						deleteobject(Level[LevelID-1].Cube[X,Y,Z])
					endif
				next Z
			next Y
		next X
		
		// if we reached the last level we won the game
		if LevelID=Level.Length-1
			// capture the end time
			EndTime#=Timer()
			// create the text for the winner... chr(10) is a new line
			TextID=CreateText("You Win!"+chr(10)+"In "+str(EndTime#-StartTime#,0)+" sec")
			// since we use the percentage mode 50 for x of the text position is in the middle of the screen/window
			SetTextPosition(TextID,50,0)
			SetTextSize(TextID,8)
			// center the text
			SetTextAlignment(TextID,1)
			// create a tween wich you can change some atributes over time
			// here we want the winner text to move from top to the middle with an elastic affect at the end of the interpolation
			TweenID=CreateTweenText(1)
			SetTweenTextY(TweenID,0,50,TweenOvershoot())
			PlayTweenText(TweenID,TextID,0)
		endif
	endif
	
	//****************************<Updates>****************************
	// update the discard parameter for the shader
	DiscardColor#=DiscardColor#+0.5*GetFrameTime()
	SetShaderConstantByName(DesintegrateShaderID,"DiscardColor",DiscardColor#,0,0,0)
    
    // update all tweens in dependence of FrameTime#
    UpdateAllTweens(FrameTime#)
    
    // if we play it on windows-PC then we call SyncBloom() else we want the regular sync()
    if GetDeviceBaseName()="windows"
		SyncBloom()
	else
		sync()
	endif
loop
//****************************<Functions>****************************
// Here we define all functions, wich could be done in an external file(for the overall viw) but we keep it in one file for now

//****************************<Initialize the bloom effect>****************************
function InitBloom()
	global RenderImageID
	global Reduction1ImageID
	global Reduction2ImageID
	global Reduction3ImageID
	global Blur1ImageID
	global InitialShaderID
	global ReductionShaderID
	global Blur1ShaderID
	global Blur2ShaderID
	global BloomShaderID
	global QuadObj
	
	// set the area of the screen that will be drawn for the bloom effect
	SetScissor(0,0,0,0)
	
	// create some intermediate images for the full screen shader
	RenderImageID=CreateRenderImage(1024, 1024, 0, 0 ) // initial render
	Reduction1ImageID=CreateRenderImage(512, 512, 0, 0 ) // reduction 1
	Reduction2ImageID=CreateRenderImage(256, 256, 0, 0 ) // reduction 2
	Reduction3ImageID=CreateRenderImage(128, 128, 0, 0 ) // reduction 3
	Blur1ImageID=CreateRenderImage(128, 128, 0, 0 ) // blur 1
	// we then blur a second time with image 5 as the output, as it is the right size and we don't need it anymore
	// image 5 (blurry) is then blended with image 2 (sharp) to produce the final output shown on the screen

	// load the shaders for each stage, Quad.vs does a simply vertex transformation to make the 3D plane perfectly fit the entire screen
	InitialShaderID=LoadShader("Shader/Quad.vs", "Shader/Initial.ps" )
	ReductionShaderID=LoadShader("Shader/Quad.vs", "Shader/Reduction.ps" ) // same shader for all three reduction steps
	Blur1ShaderID=LoadShader("Shader/Quad.vs", "Shader/Blur1.ps" )
	Blur2ShaderID=LoadShader("Shader/Quad.vs", "Shader/Blur2.ps" )
	BloomShaderID=LoadShader("Shader/Quad.vs", "Shader/Bloom.ps" ) // combines image 5 and 2 into the final output
	// make a plane to display images to the screen so they can be modified by shaders.
	// planes used for this purpose are generally called quads, AGK has a special Quad primitive
	// that means it can trake some shortcuts in the rendering pipeline, Quads will never be
	// rendered to the screen unless you specifically draw it with DrawObject()
	QuadObj=CreateObjectQuad()
endfunction

//****************************<Sync the bloom effect>****************************
function SyncBloom()
    Update(0)
    // full screen bloom process
    // first Render() as normal to image 2
    SetRenderToImage(RenderImageID,-1) // render into image 2, using an internal depth buffer
	ClearScreen() // make sure the image is clean
	Render() // draw the scene as normal
	
	/* draw the scene but don't blur the text
	Render2DBack()
	ClearDepthBuffer()
	Render3D()
	ClearDepthBuffer()
	*/

    // do some post processing on our newly rendered image 2
    SetObjectImage(QuadObj,RenderImageID, 0 ) // apply image 2 to the quad
	SetObjectShader(QuadObj,InitialShaderID) // set our first shader to the quad, this does some processing on the image to make it suitable for bloom
	SetRenderToImage(Reduction1ImageID,0) // we will output to image 3 which is half the size, so this is also a reduction whilst shader 1 does some processing
	ClearScreen() // clean image 3
	DrawObject(QuadObj) // draw the quad and nothing else, this makes it nice and fast

    // take our newly rendered image 3 and reduce it further
	SetObjectImage(QuadObj,Reduction1ImageID,0) // apply image 3 to the quad, this removes image 2 which will remain unchanged from now on
	SetObjectShader(QuadObj,ReductionShaderID) // set the reduction shader which does nothing but reduce the image size
	SetRenderToImage(Reduction2ImageID,0) // output to image 4 which is half the size of image 3
	ClearScreen() // clean image 4
	DrawObject(QuadObj) // draw the quad and nothing else

    // take our newly rendered image 4 and reduce it further
	SetObjectImage(QuadObj,Reduction2ImageID,0) // apply image 4 to the quad
	SetObjectShader(QuadObj,ReductionShaderID) // added for clarity, the quad is already using shader 2
	SetRenderToImage(Reduction3ImageID,0) // output to image 5 which is half the size of image 4
	ClearScreen() // clean image 5
	DrawObject(QuadObj) // draw the quad and nothing else

    // we now have a low quality 128x128 copy of our scene in image 5, lets blur this so it creates a nice bloom effect

	SetObjectImage(QuadObj,Reduction3ImageID,0) // apply image 5 to the quad
	SetObjectShader(QuadObj,Blur1ShaderID) // set the first blur shader, this does a horizontal blur across the pixels
	SetRenderToImage(Blur1ImageID,0) // output to image 6, which is the same size as image 5
	ClearScreen() // clean image 6
	DrawObject(QuadObj) // draw the quad and nothing else

	SetObjectImage(QuadObj,Blur1ImageID,0) // apply image 6 to the quad
	SetObjectShader(QuadObj,Blur2ShaderID) // set the second blur shader, this does a vertical blur across the pixels to finish the blur effect
	SetRenderToImage(Reduction3ImageID,0) // output to image 5 as it is no longer being used and is the right size for this step
	ClearScreen() // clean image 5
	DrawObject(QuadObj) // draw the quad and nothing else

	// we now have a very blurry image 5 and a nice sharp image 2 from the very beginning (plus some intermediate
    // images 3,4,6 that we don't need anymore), we can combine image 5 and 2 to create a nice bloom effect

    SetObjectImage(QuadObj,RenderImageID, 0 ) // set image 2 to texture stage 0 on the quad
	SetObjectImage(QuadObj,Reduction3ImageID, 1 ) // set image 2 to texture stage 1 on the quad
	SetObjectShader(QuadObj,BloomShaderID) // apply the bloom shader to the quad, this will do the work of blending image 5 and image 2
	SetRenderToScreen() // send this output to the screen, this is the only thing the user will see from the entire process
	DrawObject(QuadObj) // draw the qaud and nothing else, remember image 2 already contains our original scene so we don't need to draw that again
    // don't use Sync() here as it will redraw the entire scene again, we have already drawn the scene and post processed it
    Render2DFront()
    Swap()
endfunction

//****************************<Calculate the angle of the ball>****************************
function RotateBall(DistX#,DistZ#)
	BallAngleX#=(DistZ#)/(0.4*PI()/180)
	BallAngleZ#=-(DistX#)/(0.4*PI()/180)
	RotateObjectglobalX(BallObjectID,BallAngleX#)
	RotateObjectglobalZ(BallObjectID,BallAngleZ#)
endfunction

//****************************<Read the Level file>****************************
function ReadWorld(File$ as string)
	if getfileexists(File$)=1
		// open the file for reading
		FileID=opentoread(File$)
		// repeat the steps until the file ends
		repeat
			// capture the whole line
			String$=readline(FileID)
			if String$<>""
				// extract the key from the line and make it lowecase
				// we cut it by the "=" token
				if lower(GetStringToken(String$,"=",1))="level"
					// resize the Level array
					Level.length=Level.length+1
					// extract the parameter wich ist were we find the maze image file
					ImageFile$=GetStringToken(String$,"=",2)
					ImageID=loadimage(ImageFile$)
					// create a memblock from the image to extract the pixeldata
					MemblockID=CreateMemblockFromImage(ImageID)
					// at offset 0 we get the image Width
					Width=GetMemblockInt(MemblockID,0)
					// at offset 4 we get the image Height
					Height=GetMemblockInt(MemblockID,4)
					// at offset 8 we get the image bit Depth, but for now bit depth can be assumed to be 32 bits
					
					for X=0 to Width-1
						for Z=0 to Height-1
							// the size can be calculatet with width*height*4 in bytes
							Offset=12+(((Width*Z)+X)*4)
							Red=getmemblockbyte(MemblockID,Offset)
							Green=getmemblockbyte(MemblockID,Offset+1)
							Blue=getmemblockbyte(MemblockID,Offset+2)
							Alpha=getmemblockbyte(MemblockID,Offset+3)
							// if the pixel we are at is white we make a floor cube... y component=0
							if Red=255 and Green=255 and Blue=255 then AddCube(Level.length,X,0,Z)
							// if the pixel we are at is black we make a wall cube... y component=1
							if Red=0 and Green=0 and Blue=0 then AddCube(Level.length,X,1,Z)
						next Z
					next X
					// delete the memblock
					DeleteMemblock(MemblockID)
				endif
			endif
		until FileEOF(FileID)
		CloseFile(FileID)
	else
		message(File$+" - File not found")
	endif
endfunction

//****************************<Create a Cube>****************************
function AddCube(LevelID,X,Y,Z)
	// just create a cube in LevelID and at position X,Y,Z
	Level[LevelID].Cube[X,Y,Z]=CreateObjectBox(1,1,1)
	// set the cubes position
	SetObjectPosition(Level[LevelID].Cube[X,Y,Z],X,Y-(2*LevelID),Z)
	// apply the color image
	setobjectimage(Level[LevelID].Cube[X,Y,Z],CubeImageID,0)
	// apply the Iluminance image for the shader
	setobjectimage(Level[LevelID].Cube[X,Y,Z],IluminanceImageID,1)
	// apply shader
	SetObjectShader(Level[LevelID].Cube[X,Y,Z],CubeShaderID)
endfunction

function curvevalue(Destination#,Current#,Smooth#)
	Current#=Current#+((Destination#-Current#)/Smooth#)
endfunction Current#

function PI()
endfunction 3.14159265
