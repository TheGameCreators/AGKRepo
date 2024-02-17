
// Project: Adding Fog 
// Created: 2018-03-30

#insert "globals.agc"
#insert "setup_display.agc"
#include "text_backgrounds.agc"
#include "titles.agc"
#include "exit_button.agc"
#include "subroutines.agc"

dw#=GetDeviceWidth()

gosub create_landscape
gosub create_player
gosub create_buttons
gosub create_skybox
gosub create_water

// We don't need to see too far into the distance as we will be applying some heavy fog.
SetCameraRange(1,1,600)

Init_Text_Backgrounds()
display_exit()
maketext()

// Add some heavy fog to your 3D scene
SetFogColor(200,200,200)
SetFogRange(10,600) // Fog will be so thick we will not be able to see beyond 600 units away from camera
SetFogMode(1)

do

	// Check if orientation has changed, and re-draw text and buttons for new layout
	if dw# <> GetDeviceWidth() 
		maketext() 
		Gosub position_virtual_buttons
	endif

	// animate the water (using a render image to texture the water plane and two sprites with alpha blending and UV scrolling for the water effect)
    ripples#=ripples#+1.1
	if ripples#>360 then ripples#=ripples#-360
	SetRenderToImage(water_ri,0)
	SetVirtualResolution(100,100)
	water_motion# = water_motion# + 0.0012
	water_motion2# = water_motion2# + 0.0006
	SetSpriteUVOffset(water_s,water_motion#*0.2,water_motion#)
	SetSpriteUVOffset(water_s2,water_motion2#*0.2,water_motion2#)
	SetSpriteColorAlpha(water_s2,(sin(ripples#)+1)*127)
	SetSpritePositionByOffset(water_s,50,50)
	SetSpritePositionByOffset(water_s2,50,50)
	DrawSprite(water_s)
	DrawSprite(water_s2)
	SetSpritePositionByOffset(water_s,-2000,-2000)
	SetSpritePositionByOffset(water_s2,-2000,-2000)
	SetRenderToScreen()

	// user input
	Gosub DoMouseLook // This subroutine also allow user to touch and drag on a touchscreen device
	if GetRawKeyState(16) then speed#=9.5 else speed#=0.8
	if GetVirtualButtonState(1) or GetRawKeyState(87) then MoveObjectLocalZ(player,speed#)
	if GetVirtualButtonState(2) or GetRawKeyState(83) then MoveObjectLocalZ(player,-speed#)
	if GetVirtualButtonState(3) then RotateCameraGlobalY(1,-2) 
	if GetVirtualButtonState(4) then RotateCameraGlobalY(1,2) 
	if getrawkeystate(65) then MoveObjectLocalX(player,-speed#)
	if getrawkeystate(68) then MoveObjectLocalX(player,speed#)

	SetObjectRotation(player,0,GetCameraAngleY(1),0)
	SetObjectPosition(player,GetObjectX(player),GetObjectHeightMapHeight(landscape,GetObjectX(player),GetObjectZ(player)),GetObjectZ(player)) // This will adjust the height of the player to follow the contour of the landscape
	SetCameraPosition(1,GetObjectX(player),GetObjectY(player)+15,GetObjectZ(player)) 

	SetTextString(2,"FPS: "+str(ScreenFPS(),0)) // update the frame-rate text

	check_exit()

	`print("x: "+str(getobjectx(player))+"  z: "+str(getobjectz(player))) // Show player coordinates

    Sync()
loop
