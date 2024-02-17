
function maketext()

	// This demo uses the a virtual resolution of 100x100
	dw# = GetDeviceWidth()
	if GetTextExists(1) // delete texts if they already exist
		for t = 1 to 4
			DeleteText(t)
		next t
	endif
	
	titlesize# = 80
	textsize# = 50

	//position the exit button
	SetSpritePosition(exit_button,GetScreenBoundsright()-GetSpriteWidth(exit_button),GetScreenBoundsTop())

	CreateText(1, "Exploring Other Primitives")
	SetTextPosition(1,GetScreenBoundsLeft()+2,GetScreenBoundsTop()+2)
	SetTextSize(1,titlesize#)
	DeleteTextBackground(tbg1)
	tbg1 = CreateTextBackground(1,1,10,5,100)
	SetTextBackgroundColor(tbg1,0,0,0,155)

	CreateText(2, "FPS: 60" ) // Display the frame rate
	SetTextAlignment(2,1)
	SetTextPosition(2,GetSpriteXByOffset(exit_button),GetScreenBoundsTop()+GetSpriteHeight(exit_button))
	SetTextSize(2,textsize#*0.8)

	CreateText(3, "Demo to show 3D primitives" ) // Display some simple instructions
	SetTextPosition(3,GetScreenBoundsLeft()+2,GetScreenBoundsTop()+GetTextTotalHeight(1)+5)
	SetTextSize(3,textsize#)
	DeleteTextBackground(tbg2)
	tbg2 = CreateTextBackground(3,1,10,5,100)
	SetTextBackgroundColor(tbg2,0,0,0,155)

	CreateText(4, "Random primitives" ) // Display some simple instructions
	SetTextPosition(4,GetScreenBoundsLeft()+2,GetScreenBoundsTop()+GetTextTotalHeight(1)+5+GetTextTotalHeight(3)+5)
	SetTextSize(4,textsize#)
	DeleteTextBackground(tbg3)
	tbg3 = CreateTextBackground(4,1,10,5,100)
	SetTextBackgroundColor(tbg3,0,0,0,155)

endfunction
