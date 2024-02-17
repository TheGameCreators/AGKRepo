
// Project: Rotating The Box 
// Created: 2017-07-08

// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "Rotating The Box" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

#insert "globals.agc" // define some global variables used throughout the program
#insert "exit_button.agc" // functions to display exit button and check for exit button press
#insert "text_backgrounds.agc" // a small library of functions to create a see-through background on the title and instruction text
#insert "titles.agc" // functions and variables used to display the titles and instructions

Init_Text_Backgrounds()

SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 60, 0 ) // 60 FPS for smooth animation
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

texture = LoadImage("Back_015.png") // Load image from the media folder called lime.png and assigned its ID to an integer variable called "texture"

MyBox = CreateObjectBox(10,10,10) // Creates a 3D cube with dimensions of 5 for width, height, and depth, and assign the object ID to an integer variable called MyBox
RotateObjectLocalY(MyBox,25) // Rotate the box so we can see more sides
SetObjectImage(MyBox, texture, 0) // Assigns the texture image to MyBox in texture layer 0 

SetAmbientColor(5,5,5) // Make the scene nice and dark to show off the point lights better

makebuttons() // This is a custom function that sets the screen resolution and creates some virtual buttons

// Create a light that'll be moved around with the pointer (touch the screen)
CreatePointLight(4,10,10,10,300,200,200,200)
SetPointLightMode(4,1)

display_exit()
maketext()


do // Start of main loop

	if GetDeviceWidth() <> dw# // check if device orientation has changed
		makebuttons() // call makebuttons to position the virtual buttons in the correct position for the new layout
		maketext()
	endif

	SetTextString(2,"fps:"+str(ScreenFPS(),0)) // update the FPS text that appears under the exit button
	
	// Convert the pointer x/y position into something we can use in the 3D world
	gpx# = getpointerx()
	gpy# = getpointery()
	lx# = Get3DVectorXFromScreen(gpx#,gpy#)*150.0
	ly# = Get3DVectoryFromScreen(gpx#,gpy#)*150.0

	// Position the 4th pointlight using the coordinates we calculated above
	SetPointLightPosition(4,lx#,ly#+80,-30)

	// Checked if the user has pressed any of the virtual buttons to toggle the lights on and off
	if GetVirtualButtonPressed(1)
		if redlight = 0 // redlight is an integer variable used as a flag to identify if the light is on or off
			// Our box is located at 3d world position 0,0,0 so we'll position our point lights 
			CreatePointLight(1,0,30,-10,100,255,0,0) // Create a red point at pos 0,30,-10 - above and to the left of our box
			redlight = 1
		else
			DeletePointLight(1)
			redlight = 0
		endif
	endif

	if GetVirtualButtonPressed(2)
		if greenlight = 0
			CreatePointLight(2,20,30,-10,100,0,255,0) // Create a green point light above and to the right of the box
			greenlight = 1
		else
			DeletePointLight(2)
			greenlight = 0
		endif
	endif

	if GetVirtualButtonPressed(3)
		if bluelight = 0
			CreatePointLight(3,0,10,-20,100,0,0,255) // Create a blue point light in front of the box
			bluelight = 1
		else
			DeletePointLight(3)
			bluelight = 0
		endif
	endif

	if GetVirtualButtonState(4) = 1
		RotateObjectLocaly(MyBox,2) // Spin the box quickly whilst button 4 is pressed (Notice we used GetVirtualButtonState so it remains true whilst the button is held down)
	endif

	// slowly rotate the box around its local x axis
	RotateObjectLocalx(MyBox,0.05)
	
	// Check if exit button has been pressed. This function can be found in the exit_button.agc file.
	check_exit()

    Sync() // Renders everything to the screen
loop // End of the continuous loop


function makebuttons()

	// get the device width and height and set the virtual resolution
	dw# = GetDeviceWidth()
	dh# = GetDeviceHeight()
	SetVirtualResolution(dw#,dh#)

	// Check if the buttons already exists and if so, delete them (this is so we can use the same function for switching between landscape and portrait)
	if GetVirtualButtonExists(1)
		for t = 1 to 4
			DeleteVirtualButton(t)
		next t
	endif
	// Create 3 virtual buttons that we'll use as light switches
	AddVirtualButton(1,dw#*0.1,dh#*0.8,dw#*0.15)
	SetVirtualButtonText(1,"red")

	AddVirtualButton(2,dw#*0.3,dh#*0.8,dw#*0.15)
	SetVirtualButtonText(2,"green")

	AddVirtualButton(3,dw#*0.5,dh#*0.8,dw#*0.15)
	SetVirtualButtonText(3,"blue")

	//Create another virtual button to rotate the box
	AddVirtualButton(4,dw#*0.8,dh#*0.8,dw#*0.15)
	SetVirtualButtonText(4,"Rotate")

endfunction


