

function updateGUI()

	if GetOrientation() < 3
		SetVirtualResolution ( 320, 480 )
		SetEditBoxSize( 1, 240, 200)
		SetEditBoxUseAlternateInput(1, 0)
	else
		SetVirtualResolution ( 480, 320 )
		SetEditBoxSize( 1, 340, 200)
		SetEditBoxUseAlternateInput(1, 0)
	endif

	SetSpriteSize(exit_button, 60,60)
	SetSpritePosition(exit_button, GetVirtualWidth() - GetSpriteWidth(exit_button),0)
	SetVirtualButtonPosition(1, GetVirtualWidth() - 30, 100)
	
	SetEditBoxPosition( 1, 2,  70)

endfunction

