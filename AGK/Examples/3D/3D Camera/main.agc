
// Project: 3D Camera 
// Created: 2018-03-31

#insert "setup_display.agc"
#insert "globals.agc"
#include "text_backgrounds.agc"
#include "titles.agc"
#include "exit_button.agc"
#include "subroutines.agc"

dw#=GetDeviceWidth()

gosub create_landscape
gosub create_player
gosub create_buttons
gosub create_skybox
gosub create_water // a "no shaders" water effect

SetCameraRange(1,0.1,1000)
SetCameraFOV(1,80) // You can change the Field Of View to create zoom effects like a sniper rifle

Init_Text_Backgrounds()
display_exit()
maketext()

// This variable is used to control the camera mode we will be implementing in the code below. 
// Mode 1) 1st Person mode
// Mode 2) 3rd Person mode
// Mode 3) Cinematic mode
// Challenge: Try adding a 4th "Top-Down" mode, and a 5th "Free Flight" mode :)
camera_mode = 1 
speed#=0.5

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

	if GetVirtualButtonState(1) or GetRawKeyState(87) 
		MoveObjectLocalZ(player,speed#)
		if moving = 0
			PlayObjectAnimation(player,"",795,811,1,0.15) // run forward
			moving=1
			idle=0
		endif
	endif
	if GetVirtualButtonState(2) or GetRawKeyState(83)
		MoveObjectLocalZ(player,-speed#*0.34)
		if moving = 0
			PlayObjectAnimation(player,"",709.8,735.1,1,0.2) // walk back
			moving = 1
			idle=0
		endif
	endif
	if GetVirtualButtonState(3) then RotateObjectLocalY(player,-1) 
	if GetVirtualButtonState(4) then RotateObjectLocalY(player,1) 
	if GetVirtualButtonState(1)=0 and GetVirtualButtonState(2)=0 and GetVirtualButtonState(3)=0 and GetVirtualButtonState(4)=0 then moving = 0
	if moving = 0 and idle=0
		PlayObjectAnimation(player, "", 1000, 1282, 1, 0.3 ) // idle?
		idle=1
	endif

	if GetVirtualButtonPressed(5) then camera_mode = 1
	if GetVirtualButtonPressed(6) then camera_mode = 2
	if GetVirtualButtonPressed(7) then camera_mode = 3

	SetObjectPosition(player,GetObjectX(player),GetObjectHeightMapHeight(landscape,GetObjectX(player),GetObjectZ(player)),GetObjectZ(player)) // This will adjust the height of the player to follow the contour of the landscape

	if camera_mode = 1
		SetCameraPosition(1,GetObjectBoneWorldX(player,headbone),GetObjectBoneWorldY(player,headbone),GetObjectBoneWorldZ(player,headbone))
		SetCameraRotation(1,360-(GetObjectAngleX(player)+GetObjectBoneAngleX(player,headBone)),(GetObjectAngleY(player)+GetObjectBoneAngleY(player,headBone)),(GetObjectAngleZ(player)+GetObjectBoneAngleZ(player,headBone)))
		MoveCameraLocalY(1,0.83) // Attempt to align camera with model eyes
		MoveCameraLocalZ(1,0.37) // Attempt to align camera with model eyes
	elseif camera_mode = 2
		SetCameraPosition(1,GetObjectX(player)+GetObjectBoneX(player,headBone),GetObjectY(player)+GetObjectBoneY(player,headBone),GetObjectZ(player)+GetObjectBoneZ(player,headBone))
		SetCameraRotation(1,360-(GetObjectAngleX(player)+GetObjectBoneAngleX(player,headBone)),(GetObjectAngleY(player)+GetObjectBoneAngleY(player,headBone)),360-(GetObjectAngleZ(player)+GetObjectBoneAngleZ(player,headBone)))
		MoveCameraLocalZ(1,-25)
		if getcameray(1) < (GetObjectHeightMapHeight(landscape,GetCamerax(1),GetCameraz(1))+2) then SetCameraPosition(1,getcamerax(1),GetObjectHeightMapHeight(landscape,GetCamerax(1),GetCameraz(1))+2,getcameraz(1))
		SetCameraLookAt(1,getobjectboneworldx(player,headBone),getobjectboneworldy(player,headBone),getobjectboneworldz(player,headBone),0)
	elseif camera_mode = 3 // cinematic mode
		SetCameraPosition(1,round(getobjectx(player)/80)*80,GetObjectHeightMapHeight(landscape,round(getobjectx(player)/80)*80,round(getobjectz(player)/80)*80)+10,round(getobjectz(player)/80)*80)
		SetCameraLookAt(1,getobjectboneworldx(player,headBone),getobjectboneworldy(player,headBone),getobjectboneworldz(player,headBone),0)
	endif

	SetTextString(2,"FPS: "+str(ScreenFPS(),0)) // update the frame-rate text

	check_exit()

    Sync()
loop
