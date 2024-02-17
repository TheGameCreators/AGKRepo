
function display_exit()
	exit_button = createsprite(loadimage("power.png"))
	if GetDeviceWidth() > GetDeviceHeight() 
		es_size# = GetDeviceHeight() * 0.1
	else
		es_size# = GetDeviceWidth() * 0.1
	endif

	SetSpriteAnimation(exit_button,256,256,2)
	SetSpriteSize(exit_button,es_size#,es_size#)
	SetSpritePosition(exit_button,GetScreenBoundsright()-GetSpriteWidth(exit_button),GetScreenBoundsTop())
endfunction

function check_exit()
	If GetRawKeyReleased(27) Then End
	if GetPointerPressed() // Check if the the screen has been touched
		if GetSpriteHit(getpointerx(),getpointery()) = exit_button // check if the exit button was touched
			SetSpriteFrame(exit_button,2) // show a change in the exit button so it appears pressed
			//VibrateDevice(0.05) // A little haptic feedback
			sync() 		
			end // End the program - No need to tidy up by deleting everything used, as AppGameKit is very efficient at cleaning up after itself.
		endif
	endif
endfunction

