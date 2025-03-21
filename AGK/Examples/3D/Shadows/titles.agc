

function maketext()

	// This demo uses the a virtual resolution of 100x100
	dw# = GetDeviceWidth()
	if GetTextExists(1) // delete texts if they already exist
		for t = 1 to 4
			DeleteText(t)
		next t
	endif
	
	titlesize# = 25
	textsize# = 25

	//position the exit button
	SetSpritePosition(exit_button,GetScreenBoundsright()-GetSpriteWidth(exit_button),GetScreenBoundsTop())

	CreateText(1, "Left virtual joystick (or click and drag) to look around, right virtual joystick to move")
	SetTextDepth(1,2)
	SetTextPosition(1,GetScreenBoundsLeft()+5,GetScreenBoundsTop()+5)
	SetTextSize(1,titlesize#)
	DeleteTextBackground(tbg1)
	tbg1 = CreateTextBackground(1,1,5,5,100)
	SetTextBackgroundColor(tbg1,0,0,0,155)

	CreateText(2, "FPS: "+str(ScreenFPS(),0) ) // Display the frame rate
	SetTextDepth(2,2)
	SetTextAlignment(2,1)
	SetTextPosition(2,GetSpriteXByOffset(exit_button),GetScreenBoundsTop()+GetSpriteHeight(exit_button))
	SetTextSize(2,textsize#*0.8)

	CreateText(3, "Press the left buttons to move the sun, press right button to change shadow mapping mode" ) // Display some simple instructions
	SetTextDepth(3,2)
	SetTextPosition(3,GetScreenBoundsLeft()+5,GetScreenBoundsTop()+55)
	SetTextSize(3,textsize#)
	DeleteTextBackground(tbg2)
	tbg2 = CreateTextBackground(3,1,5,5,100)
	SetTextBackgroundColor(tbg2,0,0,0,155)

	CreateText(4, "Shadow Mode: Cascade shadow mapping" ) // Display shaddow mapping mode
	SetTextDepth(4,2)
	SetTextPosition(4,GetScreenBoundsLeft()+5,GetScreenBoundsTop()+105)
	SetTextSize(4,textsize#)
	DeleteTextBackground(tbg3)
	tbg3 = CreateTextBackground(4,1,5,5,100)
	SetTextBackgroundColor(tbg3,0,0,0,155)

endfunction
