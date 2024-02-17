
// Project: Exploring Other Primitives 
// Created: 2017-07-22

// show all errors
SetErrorMode(2)

#insert "globals.agc"
#insert "text_backgrounds.agc"
#include "exit_button.agc"
#include "titles.agc"

Init_Text_Backgrounds()

dw#=GetDeviceWidth()


// set window properties
SetWindowTitle( "Exploring Other Primitives" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 60, 0 ) // 60fps for smooth animation
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

display_exit()

maketext()

// Call the function that creates our 3D scene full of primitives
SetScene()

scene = 1
moveon# = timer()

do

	if dw# <> GetDeviceWidth() then maketext()

	SetTextString(2,"FPS: "+str(ScreenFPS(),0))

	// We want to change the scene every 5 seconds
	if timer()-moveon# > 5 
		moveon# = timer()
		scene=scene+1 
		if scene>5 then scene=1 // We have only got 5 scenes so go back to the first
	endif

	Select scene
		case 1:
			// The next 3 lines will pan the camera around so we can get a good look at our new primitives
			SetTextString(4,"Random primitives")
			DeleteTextBackground(tbg3)
			tbg3 = CreateTextBackground(4,1,10,5,100)
			SetTextBackgroundColor(tbg3,0,0,0,155)
			SetCameraPosition(1,300,0,300) // Position the camera in the middle of our random primitives
			RotateCameraLocalY(1,0.5) // Turn the camera slightly to pan around
			MoveCameraLocalZ(1,-270) // move the camera back to get a good view of the 3d objects
		endcase
		case 2:
			setcameraposition(1,1145,40,-200)
			SetCameraLookAt(1,1150,0,0,0)
			SetTextString(4,"Examples of Cylinders")
			DeleteTextBackground(tbg3)
			tbg3 = CreateTextBackground(4,1,10,5,100)
			SetTextBackgroundColor(tbg3,0,0,0,155)
		endcase
		case 3:
			setcameraposition(1,2150,40,-200)
			SetCameraLookAt(1,2150,0,0,0)
			SetTextString(4,"Examples of Cones")
			DeleteTextBackground(tbg3)
			tbg3 = CreateTextBackground(4,1,10,5,100)
			SetTextBackgroundColor(tbg3,0,0,0,155)
		endcase
		case 4:
			setcameraposition(1,3150,40,-200)
			SetCameraLookAt(1,3150,0,0,0)
			SetTextString(4,"Examples of Capsules")
			DeleteTextBackground(tbg3)
			tbg3 = CreateTextBackground(4,1,10,5,100)
			SetTextBackgroundColor(tbg3,0,0,0,155)
		endcase
		case 5:
			setcameraposition(1,4150,40,-200)
			SetCameraLookAt(1,4150,0,0,0)
			SetTextString(4,"Examples of Spheres")
			DeleteTextBackground(tbg3)
			tbg3 = CreateTextBackground(4,1,10,5,100)
			SetTextBackgroundColor(tbg3,0,0,0,155)
		endcase
	endselect

	check_exit()

    Sync()
loop



function SetScene()
//Create an "Arena" for a bunch of objects
leftside = CreateObjectBox(10,10,600)
SetObjectPosition(leftside,0,0,300)
rightside = CreateObjectBox(10,10,600)
SetObjectPosition(rightside,600,0,300)
frontside = CreateObjectBox(600,10,10)
SetObjectPosition(frontside,300,0,0)
farside = CreateObjectBox(600,10,10)
SetObjectPosition(farside,300,0,600)

For t = 1 to 50
	selector = random2(1,4)
	Select selector
		case 1:
			CreateObjectCapsule(t,random(10,30),random(10,50),random(0,2))
			SetObjectPosition(t,random(0,500),0,random(0,500))
			SetObjectColor(t,random(0,255),random(0,255),Random(0,255),255)
		endcase
		case 2:
			CreateObjectCone(t,random(10,50),random(10,30),random(3,10))
			SetObjectPosition(t,random(0,500),0,random(0,500))
			SetObjectColor(t,random(0,255),random(0,255),Random(0,255),255)
		endcase
		case 3:
			CreateObjectSphere(t,random(10,30),random(4,10),random(4,10))
			SetObjectPosition(t,random(0,500),0,random(0,500))
			SetObjectColor(t,random(0,255),random(0,255),Random(0,255),255)
		endcase
		case 4:
			CreateObjectCylinder(t,random(10,50),random(10,30),random(3,10))
			SetObjectPosition(t,random(0,500),0,random(0,500))
			SetObjectColor(t,random(0,255),random(0,255),Random(0,255),255)
		endcase
	endselect
next t


// Create a row of cylinders
for t = 1 to 8
	CreateObjectCylinder(t+200,t*5,20,t+2)
	SetObjectPosition(t+200,1000+t*30,0,0)
	SetObjectColor(t+200,random(0,255),random(0,255),Random(0,255),255)
next t

// Create a row of cones
for t = 1 to 8
	CreateObjectCone(t+300,t*5,20,t+2)
	SetObjectPosition(t+300,2000+t*30,0,0)
	SetObjectColor(t+300,random(0,255),random(0,255),Random(0,255),255)
next t

// Create a row of capsules
for t = 1 to 8
	CreateObjectCapsule(t+400,20,t*7,1)
	SetObjectPosition(t+400,3000+t*30,0,0)
	SetObjectColor(t+400,random(0,255),random(0,255),Random(0,255),255)
next t

// Create a row of spheres
for t = 1 to 8
	CreateObjectSphere(t+500,random(15,30),t+3,t+3)
	SetObjectPosition(t+500,4000+t*30,0,0)
	SetObjectColor(t+500,random(0,255),random(0,255),Random(0,255),255)
next t

endfunction

