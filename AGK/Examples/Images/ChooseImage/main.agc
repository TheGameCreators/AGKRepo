
// Choose image from system and show it
// Introduced in build 107

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background5.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// get choose image
if ShowChooseImageScreen()=1
 while IsChoosingImage()=1
  Sync()
 endwhile
 image=GetChosenImage()
endif

// main loop
do
	// show image here
	if GetSpriteExists(1)=0
	 CreateSprite(1,image)
	 SetSpritePosition(1,10,10)
	 SetSpriteSize(1,80,-1)
    endif

    // update the screen
    Print("Result of chooser..")
    Sync ( )
loop
