// Project: PBRtest
// Created: 19-02-01

// show all errors
SetErrorMode(2)

local tmr# = 0.0
local spd# = 1.0

local gamma# = 3.55

gosub _init


do
	loco = ScreenFPS()
	tmr# = 1.0 / loco
	Print("FPS: "+str(loco)+"   CamPos: "+str(GetCameraX(1))+" / "+str(GetCameraY(1))+" / "+str(GetCameraZ(1))+" ( WASD )   Gamma: "+str(gamma#)+" ( 1 / 2 )" )
	
	gosub _camControls
	
	gosub _controls
	
	Sync()
	
	if (GetRawKeyState(27))
		exit
	endif
loop

end


_camControls:
	spd# = tmr# * 3.0
	if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, spd# )
	if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -spd# )
	if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -spd# )
	if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, spd# )
	
	if ( GetPointerPressed() )
		startx# = GetPointerX()
		starty# = GetPointerY()
		angx# = GetCameraAngleX(1)
		angy# = GetCameraAngleY(1)
		pressed = 1
	endif

	if ( GetPointerState() = 1 )
		fDiffX# = (GetPointerX() - startx#)/1.0
		fDiffY# = (GetPointerY() - starty#)/1.0

		newX# = angx# + fDiffY#
		if ( newX# > 89 ) then newX# = 89
		if ( newX# < -89 ) then newX# = -89
		SetCameraRotation( 1, newX#, angy# + fDiffX#, 0 )
	endif
return


_controls:
	if (GetRawKeyState(49))  // Y
		mll# = 1.0+tmr#*1.2
		gamma# = gamma# / mll#
		for i = 1 to 5
			SetShaderConstantByName(i, "gamma", gamma#, 0,0,0)
		next
	endif
	
	if (GetRawKeyState(50))  // X
		mll# = 1.0+tmr#*1.2
		gamma# = gamma# * mll#
		if gamma# > 82 then gamma# = 82
		for i = 1 to 5
			SetShaderConstantByName(i, "gamma", gamma#, 0,0,0)
		next
	endif
return


_init:
	// set window properties
	SetAntialiasMode( 1 )
	SetWindowTitle("PBRtest")
	SetWindowSize(1800, 900, 0)
	SetWindowAllowResize(1) // allow the user to resize the window
	
	// set display properties
	//SetVirtualResolution(1024, 768) // doesn't have to match the window
	SetOrientationAllowed(1, 1, 1, 1) // allow both portrait and landscape on mobile devices
	SetSyncRate(0, 0) // 30fps instead of 60 to save battery
	SetScissor(0, 0, 0, 0) // use the maximum available screen space, no black borders
	UseNewDefaultFonts(1) // since version 2.0.22 we can use nicer default fonts
	SetPrintSize( 2.5 )

	SetCameraPosition( 1, 2.8, 4.25, -7.7 )
	SetCameraLookAt( 1, -1.0, 0.8, 0.0, 0 )
	
	// main object top
	LoadObject(1, "m_probe.fbx")
	LoadShader(1, "s_base.vs", "s_pbr.ps")
	LoadImage(1, "t_test_d.png")
	LoadImage(2, "t_test_n.png")
	LoadImage(3, "0_pr_p1.png")
	LoadImage(4, "0_lm_p1.png")
	
	SetObjectImage(1, 1, 0)
	SetObjectImage(1, 2, 1)
	SetObjectImage(1, 3, 2)
	SetObjectImage(1, 4, 3)
	SetObjectShader(1, 1)
	
	
	// main object bottom
	LoadObject(2, "m_probe2.fbx")
	LoadImage(5, "t_test2_d.png")
	LoadImage(6, "t_test2_n.png")
	LoadImage(7, "0_pr_p2.png")
	LoadImage(8, "0_lm_p2.png")
	
	SetObjectImage(2, 5, 0)
	SetObjectImage(2, 6, 1)
	SetObjectImage(2, 7, 2)
	SetObjectImage(2, 8, 3)
	SetObjectShader(2, 1)
	
	
	// floor
	LoadObject(3, "m_floor.fbx")
	LoadImage(9, "0_lm_floor.png")
	LoadShader(2, "s_base.vs", "s_simple.ps")
	SetObjectImage(3, 9, 0)
	SetObjectShader(3, 2)

	
	// sphere left
	LoadObject(4, "m_sphere1.fbx")
	LoadImage(10, "t_sphere1_d.png")
	LoadImage(11, "t_sphere1_n.png")
	LoadImage(12, "0_pr_s1.png")
	LoadShader(3, "s_base.vs", "s_simple_metal.ps")
	SetObjectImage(4, 10, 0)
	SetObjectImage(4, 11, 1)
	SetObjectImage(4, 12, 2)
	SetObjectShader(4, 3)
	SetShaderConstantByName(3, "roughness", 0.1, 0,0,0)
	
	// sphere right
	LoadObject(5, "m_sphere2.fbx")
	LoadImage(13, "t_sphere2_d.png")
	LoadImage(14, "t_sphere2_n.png")
	LoadImage(15, "0_pr_s2.png")
	LoadImage(16, "0_lm_s2.png")
	LoadShader(4, "s_base.vs", "s_simple_dielectric.ps")
	SetObjectImage(5, 13, 0)
	SetObjectImage(5, 14, 1)
	SetObjectImage(5, 15, 2)
	SetObjectImage(5, 16, 3)
	SetObjectShader(5, 4)
	SetShaderConstantByName(4, "roughness", 0.1, 0,0,0)
	
	// load the skybox
	LoadObject(6, "m_skybox.fbx")
	LoadShader(5, "s_base.vs", "s_sky.ps")
	LoadImage(17, "0_sky.png")
	SetObjectImage(6, 17, 0)
	SetObjectShader(6, 5)
	
	for i = 1 to 5
		SetShaderConstantByName(i, "gamma", gamma#, 0,0,0)
	next

return
