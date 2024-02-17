
function maketext()

	if GetTextExists(1) // delete texts if they already exist
		for t = 1 to 4
			DeleteText(t)
		next t
	endif
	
	if dw#>dh#
		titlesize# = dw#*0.07
		textsize# = dw#*0.04
	else
		titlesize# = dw#*0.07
		textsize# = dw#*0.05
	endif

	CreateText(1, "3D Lighting and Rotating a Box")
	SetTextPosition(1,dw#*0.02,dh#*0.02)
	SetTextSize(1,titlesize#)
	`SetTextColor(1,255,100,100,255)
	DeleteTextBackground(tbg1)
	tbg1 = CreateTextBackground(1,1,10,5,100)
	SetTextBackgroundColor(tbg1,0,0,0,155)

	CreateText(2, "FPS: "+str(ScreenFPS(),0) ) // Display the frame rate
	SetTextAlignment(2,1)
	SetTextPosition(2,GetSpriteXByOffset(exit_button),GetScreenBoundsTop()+GetSpriteHeight(exit_button))
	SetTextSize(2,textsize#*0.8)

	CreateText(3, "Tap the buttons to toggle lights and rotate the box" ) // Display some simple instructions
	SetTextPosition(3,dw#*0.02,dh#*0.15)
	SetTextSize(3,textsize#)
	`SetTextColor(3,255,100,100,255)
	DeleteTextBackground(tbg3)
	tbg3 = CreateTextBackground(3,1,10,5,100)
	SetTextBackgroundColor(tbg3,0,0,0,155)

	CreateText(4, "touch anywhere else to position a point light" )
	SetTextPosition(4,dw#*0.02,dh#*0.23)
	SetTextSize(4,textsize#)
	`SetTextColor(4,255,100,100,255)
	DeleteTextBackground(tbg4)
	tbg4 = CreateTextBackground(4,1,10,5,100)
	SetTextBackgroundColor(tbg4,0,0,0,155)

	//position the exit button
	//SetSpriteSize(exit_button,GetDeviceWidth()*0.1,GetDeviceWidth()*0.1)
	SetSpritePosition(exit_button,GetScreenBoundsright()-GetSpriteWidth(exit_button),GetScreenBoundsTop())
endfunction


