
function maketext()

	dw# = GetDeviceWidth()
	if GetTextExists(1) // delete texts if they already exist
		for t = 1 to 4
			DeleteText(t)
		next t
	endif
	
	If GetDeviceWidth() > GetDeviceHeight()
		// Position the camera so it is facing our penny falls machine
		SetCameraPosition( 1, 0,190,-800 )
		SetCameraLookAt( 1, 0,10,0, 0 )
		SetCameraFOV(1,80)
		titlesize# = 60
		textsize# = 40
	else
		// Position the camera so it is facing our penny falls machine
		SetCameraPosition( 1, 0,250,-650 )
		SetCameraLookAt( 1, 0,10,0, 0 )
		SetCameraFOV(1,80)
		titlesize# = 50
		textsize# = 30
	endif


	//position the exit button
	SetSpritePosition(exit_button,GetScreenBoundsright()-GetSpriteWidth(exit_button),GetScreenBoundsTop())

	CreateText(1, "Penny Falls Physics")
	SetTextDepth(1,2)
	SetTextPosition(1,GetScreenBoundsLeft()+2,GetScreenBoundsBottom()-200)
	SetTextSize(1,titlesize#)
	DeleteTextBackground(tbg1)
	tbg1 = CreateTextBackground(1,1,5,5,100)
	SetTextBackgroundColor(tbg1,0,0,0,155)

	CreateText(2, "FPS: "+str(ScreenFPS(),0) ) // Display the frame rate
	SetTextDepth(2,2)
	SetTextAlignment(2,1)
	SetTextPosition(2,GetSpriteXByOffset(exit_button),GetScreenBoundsTop()+GetSpriteHeight(exit_button))
	SetTextSize(2,textsize#*0.8)

	CreateText(3, "Demo to simulate a Penny Falls arcade machine" ) // Display some simple instructions
	SetTextDepth(3,2)
	SetTextPosition(3,GetScreenBoundsLeft()+2,GetScreenBoundsBottom()-200+GetTextTotalHeight(1)+20)
	SetTextSize(3,textsize#)
	DeleteTextBackground(tbg2)
	tbg2 = CreateTextBackground(3,1,5,5,100)
	SetTextBackgroundColor(tbg2,0,0,0,155)

	CreateText(4, "Tap on screen to drop a coin" ) // Display some simple instructions
	SetTextDepth(4,2)
	SetTextPosition(4,GetScreenBoundsLeft()+2,GetScreenBoundsbottom()-200+GetTextTotalHeight(1)+20+GetTextTotalHeight(3)+20)
	SetTextSize(4,textsize#)
	DeleteTextBackground(tbg3)
	tbg3 = CreateTextBackground(4,1,5,5,100)
	SetTextBackgroundColor(tbg3,0,0,0,155)

endfunction
