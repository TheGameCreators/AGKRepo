
// Project: Creating a SkyBox 
// Purpose: To show the features of the AppGameKit built-in skybox commands
// Created: 2018-03-20

#insert "globals.agc"
#insert "setup_display.agc"
#include "exit_buttons.agc"
#include "titles.agc"
#include "exit_button.agc"
#include "subroutines.agc"

dw#=GetDeviceWidth()

Sky_Horiz_Red = 50
Sky_Horiz_Green = 60
Sky_Horiz_Blue = 10
Sky_Red = 60
Sky_Green = 40
Sky_Blue = 180
Sky_Sun_Red = 255
Sky_Sun_Green = 255
Sky_Sun_Blue = 200

SetSkyBoxHorizonColor(Sky_Horiz_Red,Sky_Horiz_Green,Sky_Horiz_Blue)
SetSkyBoxSkyColor(Sky_Red,Sky_Green,Sky_Blue)
SetSkyBoxSunColor(Sky_Sun_Red,Sky_Sun_Green,Sky_Sun_Blue)
SkyBox_Height = 0
SkyBox_Size = 5
SetSkyBoxHorizonSize(SkyBox_Size,SkyBox_Height)

sunpos = CreateObjectBox(5,5,5) // We will use this as a dummy object to control the sun position
RotateObjectlocalx(sunpos,30)
RotateObjectlocalZ(sunpos,80)
MoveObjectLocalY(sunpos,100)
SetSunDirection(GetObjectX(sunpos),GetObjectY(sunpos),GetObjectZ(sunpos))
SetObjectVisible(sunpos,0)

SetSkyBoxVisible(1)

SetCameraRotation(1,0,0,0)

// Create some custom tweens to fade skybox colours (1 sec duration)
HorizonTween = CreateTweenCustom(1)
SkyTween = CreateTweenCustom(1)
SunTween = CreateTweenCustom(1)

// Set initial values for OlderPointerX and OldPointerY
OldPointerX# = GetPointerX()
OldPointerY# = GetPointerY()

gosub create_buttons
gosub create_city

Init_Text_Backgrounds()
display_exit()
maketext()

do   

	if dw# <> GetDeviceWidth() 
		maketext() // Check if orientation has changed, and re-draw text for new layout
		Gosub position_virtual_buttons
	endif

	//mouselook
	Gosub DoMouseLook  // Got to love Gosub! 

	// Update Sky if fading between colours
	If GetTweenCustomPlaying(SkyTween)
		UpdateTweenCustom(SkyTween,GetFrameTime())
		Sky_Red = GetTweenCustomInteger1(SkyTween)
		Sky_Green = GetTweenCustomInteger2(SkyTween)
		Sky_Blue = GetTweenCustomInteger3(SkyTween)
		SetSkyBoxSkyColor(Sky_Red,Sky_Green,Sky_Blue)
	endif
	if GetTweenCustomPlaying(HorizonTween)
		UpdateTweenCustom(HorizonTween,GetFrameTime())
		Sky_Horiz_Red = GetTweenCustomInteger1(HorizonTween)
		Sky_Horiz_Green = GetTweenCustomInteger2(HorizonTween)
		Sky_Horiz_Blue = GetTweenCustomInteger3(HorizonTween)
		SetSkyBoxHorizonColor(Sky_Horiz_Red,Sky_Horiz_Green,Sky_Horiz_Blue)
	endif
	If GetTweenCustomPlaying(SunTween)
		UpdateTweenCustom(SunTween,GetFrameTime())
		Sky_Sun_Red = GetTweenCustomInteger1(SunTween)
		Sky_Sun_Green = GetTweenCustomInteger2(SunTween)
		Sky_Sun_Blue = GetTweenCustomInteger3(SunTween)
		SetSkyBoxSunColor(Sky_Sun_Red,Sky_Sun_Green,Sky_Sun_Blue)
	endif

	// Check if any buttons have been pressed
	if GetVirtualButtonPressed(1) then gosub ChangeSky
	If GetVirtualButtonPressed(2) then gosub ChangeHorizon
	if GetVirtualButtonPressed(3) then gosub ChangeSun

	SetTextString(2,"FPS: "+str(ScreenFPS(),0)) // update the frame-rate text
	check_exit()

	// rotate the sun
	MoveObjectLocalY(sunpos,-100)
	RotateObjectlocalZ(sunpos,0.1)
	MoveObjectLocalY(sunpos,100)
	SetSunDirection(GetObjectX(sunpos),GetObjectY(sunpos),GetObjectZ(sunpos))

    Sync()

loop
