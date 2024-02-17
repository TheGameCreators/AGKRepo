
// Project: Loading a 3D model 
// Created: 2017-10-31

// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "treetest" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 60, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

#insert "globals.agc" // #insert will insert the contents of the file in this location
#insert "text_backgrounds.agc"
#include "exit_button.agc" // #include will append the contents of the file to the end of this source code
#include "titles.agc"

Init_Text_Backgrounds()

// Load an image to use as a texture for our floor
grass = LoadImage("GrassTexture.png")
// Allow us to wrap the image across its UV so that we can tile it
SetImageWrapU(grass,1)
SetImageWrapV(grass,1)

// Create a plane object to use as our floor
MyFloor = CreateObjectPlane(1000,1000)
// Rotate it so it lays flat
RotateObjectLocalX(MyFloor,90)
// Texture it with our grass image
SetObjectImage(MyFloor,Grass,0)
// scale the texture to tile it 30 times across the object
SetObjectUVScale(MyFloor,0,30,30)


// Loading 3D objects

//
// Here we are going to build a 3d scene made up of 4 churches showing some different methods of loading or cloning 3d objects:
// load a 3D into object ID 1 and image ID 1 for the texture
//
LoadImage( 1, "chapel_D.png" )
LoadObject( 1, "chapel.obj", 10 )
SetObjectImage( 1, 1, 0 )
SetObjectPosition(1,15,0,-15)

//
// load an object and texture but this time get AGK to assign the object and image number for us
//
churchtexture = LoadImage("chapel_D.png" )
churchobject = LoadObject("chapel.obj", 10 )

SetObjectImage( churchobject, churchtexture, 0 )
setobjectposition(churchobject,15,0,15)

//
// Clone object ID 1 into ID 2
//
CloneObject (2,1)
SetObjectPosition(2,-15,0,15)

//
// Clone churchobject and auto assign the ID to vriable churchclone
//
churchclone = CloneObject(churchobject)
SetObjectPosition(churchclone,-15,0,-15)



// and a few trees using an array to store the IDs
trees as integer[4]
SetRandomSeed(6)
treetexture = loadimage("tree02_d_result_mobile.png")
for t = 0 to 3
	// load our array of trees.  The 15 represents the height of the tree. The whole object will be scaled to match the height.
	//  This gives us a very convenient way to scale objects so they fit into our scene.
	trees[t] = LoadObject("tree02.x",15)
	SetObjectPosition(trees[t],random(0,60)-30,0,random(0,60)-30)
	SetObjectImage(trees[t],treetexture,0)
	SetObjectAlphaMask(trees[t],1)
next t

dw#=GetDeviceWidth()

display_exit()

maketext()

do

	if dw# <> GetDeviceWidth()
		maketext()
	endif

	SetTextString(2,"fps:"+str(ScreenFPS(),0)) 

	// pan the camera around our church scene
	t#=timer()
	cam_x# = sin(t#*15.0)*50.0
	cam_z# = cos(t#*11.2)*65.0
	cam_y# = sin(t#*21.0)*15.0
	SetCameraPosition(1,cam_x#,cam_y#+20.0,cam_z#)

	SetCameraLookAt(1,getobjectx(1),getobjecty(1),getobjectz(1),0)

	check_exit()
    Sync()
loop



