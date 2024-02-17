
// Project: Plane Floor 
// Created: 2017-10-04

// show all errors
SetErrorMode(2)

// Get in the habit of splitting source into seperate files. Makes it SO much easier to read!

#insert "globals.agc"
#insert "text_backgrounds.agc"
#include "exit_button.agc"
#include "titles.agc"

Init_Text_Backgrounds()


// set window properties
SetWindowTitle( "Plane Floor" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 100, 100 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 60, 0 )
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

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

dw#=GetDeviceWidth()

display_exit()

maketext()


// Make sure our camera is pointing straight ahead
SetCameraRotation(1,0,0,0)

// Left joystick to look around
AddVirtualJoystick(1,15,85,20)
// Right joystick to move
AddVirtualJoystick(2,85,85,20)

do
	//
	// implement a very simple control system
	//
	
	if dw# <> GetDeviceWidth() then maketext()

	// look around with the left virtual joystick
	RotateCameraLocalX(1,GetVirtualJoystickY(1))
	// keep x axis within certain bounds
	if GetCameraAngleX(1) > 50 then RotateCameraLocalX(1,0-GetVirtualJoystickY(1))
	if GetCameraAngleX(1) < -50 then RotateCameraLocalX(1,0-GetVirtualJoystickY(1))

	RotateCameraLocalY(1,GetVirtualJoystickX(1))
	SetCameraRotation( 1, GetCameraAngleX(1), GetCameraAngleY(1), 0 ) // This is to keep the camera upright

	// Move around with the right virtual joystick
	MoveCameraLocalZ(1,0-GetVirtualJoystickY(2))
	MoveCameraLocalX(1,GetVirtualJoystickX(2))
	SetCameraPosition(1,getcamerax(1),10,getcameraz(1)) // Keep camera at the same height above the ground
	
	SetTextString(2,"FPS:"+str(ScreenFPS(),0)) // update the fps

	check_exit()
//    Print( ScreenFPS() )
    Sync()
loop
















