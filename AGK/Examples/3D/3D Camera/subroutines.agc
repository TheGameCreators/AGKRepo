

DoMouseLook:
	If GetPointerState()
		Head_Angle_X# = Head_Angle_X# + ((GetPointerY()-OldPointerY#)*0.5)
		Head_Angle_Y# = Head_Angle_Y# + ((GetPointerX()-OldPointerX#)*0.5)
		if Head_Angle_X# < -90 then Head_Angle_X# = -90
		if Head_Angle_X# > 90 then Head_Angle_X# = 90
		SetObjectBoneRotation(player,headBone,0-Head_Angle_X#,Head_Angle_Y#,0)
	endif
	OldPointerX# = GetPointerX()
	OldPointerY# = GetPointerY()
return

position_virtual_buttons:
	SetVirtualButtonPosition(1,GetScreenBoundsLeft()+10,GetScreenBoundsBottom()-30)
	SetVirtualButtonPosition(2,GetScreenBoundsLeft()+10,GetScreenBoundsBottom()-10)
	SetVirtualButtonPosition(3,GetScreenBoundsRight()-30,GetScreenBoundsBottom()-10)
	SetVirtualButtonPosition(4,GetScreenBoundsRight()-10,GetScreenBoundsBottom()-10)
	SetVirtualButtonPosition(5,GetScreenBoundsRight()-10,GetScreenBoundsTop()+30) // First Person Camera
	SetVirtualButtonPosition(6,GetScreenBoundsRight()-10,GetScreenBoundsTop()+50) // 3rd Person Camera
	SetVirtualButtonPosition(7,GetScreenBoundsRight()-10,GetScreenBoundsTop()+70) // Cinematic Camera
return

create_landscape:
	landscape = CreateObjectFromHeightMap("heightmap4.png",8000,512,8000,1,2)
	diffuse = loadimage("diff3.png")
	SetGenerateMipmaps(1)
	normal = loadimage("grassdry256n.png")
	SetImageWrapU(normal,1)
	SetImageWrapV(normal,1)
	setobjectimage(landscape,diffuse,0)
	SetObjectNormalMap(landscape,normal)
	SetObjectNormalMapScale(landscape,500,500)
	SetGenerateMipmaps(0)
return

create_player:
	// Load soldier character for our player
	player = LoadObjectWithChildren("Original_Soldier.X")
	player_texture = LoadImage("Soldier_body_1_D.png")
	setobjectimage(player,player_texture,0)
	// texture gun
	gun_texture = LoadImage("gun_D.png")
	setobjectimage( GetObjectChildID(player,2), gun_texture, 0 )
	// hide fire spot object, a cube that was added to the model to show the gun anchor point
	SetObjectVisible( GetObjectChildID(player,1), 0 )
	SetObjectScalePermanent(player,0.1,0.1,0.1)
	SetObjectScalePermanent(GetObjectChildID(player,2),0.1,0.1,0.1)
	// play idle animation
	RotateObjectLocalY(player,180)
	PlayObjectAnimation( player, "", 1000, 1282, 1, 0 )
	SetObjectAnimationSpeed( player, 20 )
	FixObjectPivot(player)
	// Control head with mouselook
	headBone = GetObjectBoneByName( player, "Bip01_Head" )
	SetObjectBoneCanAnimate( player, headBone, 0 )
	Head_Angle_X# = GetObjectBoneAngleX(player,headBone)
	Head_Angle_Y# = GetObjectBoneAngleY(player,headBone)
	px# = 4768.0 // Player start x position
	pz# = 3906.0 // Player start z position
	// initialise player and camera position
	SetObjectPosition(player,px#,GetObjectHeightMapHeight(landscape,400,400),pz#)
	SetCameraPosition(1,GetObjectX(player),GetObjectY(player)+15,GetObjectZ(player))
return

create_buttons:
	// Create our control buttons
	AddVirtualButton(1,GetScreenBoundsLeft()+10,GetScreenBoundsBottom()-30,20) // Move forward button
	AddVirtualButton(2,GetScreenBoundsLeft()+10,GetScreenBoundsBottom()-10,20) // Move backward button
	AddVirtualButton(3,GetScreenBoundsRight()-30,GetScreenBoundsBottom()-10,20) // turn left
	AddVirtualButton(4,GetScreenBoundsRight()-10,GetScreenBoundsBottom()-10,20) // turn right
	AddvirtualButton(5,GetScreenBoundsRight()-10,GetScreenBoundsTop()+30,15) // First Person Camera
	AddvirtualButton(6,GetScreenBoundsRight()-10,GetScreenBoundsTop()+50,15) // 3rd Person Camera
	AddvirtualButton(7,GetScreenBoundsRight()-10,GetScreenBoundsTop()+70,15) // Cinematic Camera
	SetVirtualButtonText(1,"Forward")
	SetVirtualButtonText(2,"Back")
	SetVirtualButtonText(3,"Turn Left")
	SetVirtualButtonText(4,"Turn Right")
	SetVirtualButtonText(5,"1st")
	SetVirtualButtonText(6,"3rd")
	SetVirtualButtonText(7,"Cine")
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
