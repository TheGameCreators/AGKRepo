
// Create and use an Edit Box

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background5.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// create edit boxes
CreateEditBox(1)
SetEditBoxText(1,"normal field")
SetEditBoxPosition(1,10,90)
SetEditBoxSize(1,80,8)

index=CreateEditBox()
SetEditBoxPosition(index,10,10)
SetEditBoxSize(index,50,20)
SetEditBoxDepth(index,10)
SetEditBoxBorderSize(index,5)
SetEditBoxBorderColor(index,255,128,100,255)
SetEditBoxBackgroundColor(index,192,192,192,192)
SetEditBoxText(index,"Hello world")
SetEditBoxTextColor(index,0,255,255)
SetEditBoxCursorColor(index,255,0,0)
SetEditBoxTextSize(index,6)
SetEditBoxFocus(index,1)
SetEditBoxActive(index,1)
SetEditBoxVisible(index,1)
SetEditBoxCursorBlinkTime(index,2.0)
SetEditBoxCursorWidth(index,2.0)
SetEditBoxMaxChars(index,5000)
SetEditBoxMaxLines(index,4)
SetEditBoxMultiLine(index,1)
FixEditBoxToScreen(index,1)

// main loop
do
	// main code
	if GetEditBoxExists(index)=1
	endif

	// get data from edit box
	for n=1 to 13 : Print("") : next n
	Print("CUSTOM EDIT BOX ABOVE")
	Print("GetCurrentEditBox="+str(GetCurrentEditBox()))
	Print("GetEditBoxHasFocus="+str(GetEditBoxHasFocus(index)))
	Print("GetEditBoxText="+GetEditBoxText(index))
	Print("GetEditBoxX="+str(GetEditBoxX(index)))
	Print("GetEditBoxY="+str(GetEditBoxY(index)))
	Print("GetEditBoxWidth="+str(GetEditBoxWidth(index)))
	Print("GetEditBoxHeight="+str(GetEditBoxHeight(index)))
	Print("GetEditBoxChanged="+str(GetEditBoxChanged(index)))
	Print("GetEditBoxActive="+str(GetEditBoxActive(index)))
	Print("GetEditBoxVisible="+str(GetEditBoxVisible(index)))
	Print("GetEditBoxLines="+str(GetEditBoxLines(index)))

    // update the screen
    Sync ( )
loop

// free usages
DeleteEditBox(index)
