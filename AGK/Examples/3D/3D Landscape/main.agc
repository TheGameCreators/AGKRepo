
// Project: 3D Landscape 
// Created: 2018-03-26
#insert "setup_display.agc"
#insert "globals.agc"
#include "text_backgrounds.agc"
#include "titles.agc"
#include "exit_button.agc"
#include "subroutines.agc"

dw#=GetDeviceWidth()

landscape = CreateObjectFromHeightMap("heightmap4.png",8000,512,8000,1,2)
diffuse = loadimage("diff3.png")
SetGenerateMipmaps(1)
normal = loadimage("grassdry256n.png")
SetImageWrapU(normal,1)
SetImageWrapV(normal,1)
setobjectimage(landscape,diffuse,0)
SetObjectNormalMap(landscape,normal)
SetObjectNormalMapScale(landscape,300,300)
SetGenerateMipmaps(0)

// Create a cube object as a placeholder for our player position 
player = CreateObjectBox(5,5,5) : SetObjectVisible(player,0)

px# = 4768.0 // Player start x position
pz# = 3906.0 // Player start z position

// initialise player and camera position
SetObjectPosition(player,px#,GetObjectHeightMapHeight(landscape,400,400),pz#)
SetCameraPosition(1,GetObjectX(player),GetObjectY(player)+15,GetObjectZ(player))

// Create our control buttons
AddVirtualButton(1,GetScreenBoundsLeft()+10,GetScreenBoundsBottom()-30,20)
AddVirtualButton(2,GetScreenBoundsLeft()+10,GetScreenBoundsBottom()-10,20)
AddVirtualButton(3,GetScreenBoundsRight()-30,GetScreenBoundsBottom()-10,20)
AddVirtualButton(4,GetScreenBoundsRight()-10,GetScreenBoundsBottom()-10,20)
SetVirtualButtonText(1,"Forward")
SetVirtualButtonText(2,"Back")
SetVirtualButtonText(3,"Turn Left")
SetVirtualButtonText(4,"Turn Right")

gosub create_skybox
gosub create_water

Init_Text_Backgrounds()
display_exit()
maketext()

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

    Sync()
loop

