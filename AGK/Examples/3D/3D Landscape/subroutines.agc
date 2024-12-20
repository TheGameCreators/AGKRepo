
DoMouseLook:
	If GetPointerState()
		Camera_Angle_X# = GetCameraAngleX(1) + ((GetPointerY()-OldPointerY#)*0.5)
		Camera_Angle_Y# = GetCameraAngleY(1) + ((GetPointerX()-OldPointerX#)*0.5)
		if Camera_Angle_X# < -90 then Camera_Angle_X# = -90
		if Camera_Angle_X# > 90 then Camera_Angle_X# = 90
	`	SetObjectRotation(player,Camera_Angle_X#,Camera_Angle_Y#,0)
		SetCameraRotation(1,Camera_Angle_X#,Camera_Angle_Y#,0)
	endif
	OldPointerX# = GetPointerX()
	OldPointerY# = GetPointerY()
return

position_virtual_buttons:
	SetVirtualButtonPosition(1,GetScreenBoundsLeft()+10,GetScreenBoundsBottom()-30)
	SetVirtualButtonPosition(2,GetScreenBoundsLeft()+10,GetScreenBoundsBottom()-10)
	SetVirtualButtonPosition(3,GetScreenBoundsRight()-30,GetScreenBoundsBottom()-10)
	SetVirtualButtonPosition(4,GetScreenBoundsRight()-10,GetScreenBoundsBottom()-10)
return


create_skybox:
	// Create a simple skybox
	SetSkyBoxVisible(1)
	SetSkyBoxHorizonColor(160,180,210)
	SetSkyBoxSkyColor(90,80,190)
return

create_water:
	// A simple transparent plane for the water
	water = CreateObjectPlane(50000,50000)
	RotateObjectLocalX(water,90)
	SetObjectPosition(water,0,90,0)
	SetObjectColor(water,0,20,120,180)
	SetObjectTransparency(water,1)
	// and another for the "floor"
	floorplane = CreateObjectPlane(50000,50000)
	RotateObjectLocalX(floorplane,90)
	SetObjectPosition(floorplane,0,0,0)
	SetObjectColor(floorplane,0,0,0,255)
	// Create a nice water effect (without shaders - all built in commands)
	water_i = loadimage("water7.png")
	SetImageWrapU(water_i,1)
	SetImageWrapV(water_i,1)
	water_ri= CreateRenderImage(256,256,0,0)
	SetImageWrapU(water_ri,1)
	SetImageWrapV(water_ri,1)
	SetRenderToImage(water_ri,0)
	water_s = CreateSprite(water_i)
	water_s2 = CreateSprite(water_i)
	SetSpriteSize(water_s,100,100)
	SetSpriteSize(water_s2,100,100)
	SetSpriteColorAlpha(water_s2,126)
	SetVirtualResolution(100,100)
	SetSpritePositionByOffset(water_s,50,50)
	SetSpritePositionByOffset(water_s2,50,50)
	DrawSprite(water_s)
	DrawSprite(water_s2)
	SetRenderToScreen()
	SetSpritePositionByOffset(water_s,-2000,-2000)
	SetSpritePositionByOffset(water_s2,-2000,-2000)
	SetObjectNormalMap(water,water_ri)
	SetObjectNormalMapScale(water,550,550)
	// some variables to control the water animation
	water_motion# = 0.1
	water_motion2# = 0.1
	ripples#=0.1
return
