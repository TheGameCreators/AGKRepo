
// setting the visibility of sprites

SetVirtualResolution ( 320, 480 )

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
CreateSprite ( LoadImage ( "background3.jpg" ) )

// penguin
LoadImage ( 1, "penguin.png" )
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 100, 300 )

// main loop
do
    // display instructions
    SetPrintSize(26)
    Print ( "Click or touch the screen to" )
    Print ( "hide or show the penguin" )

    // switch visibility on input
    if ( GetPointerPressed ( ) = 1 )
		if state = 0
			SetSpriteVisible ( 1, 0 )
			state = 1
		else
			SetSpriteVisible ( 1, 1 )
			state = 0
		endif
    endif

    // update the screen
    Sync ( )
loop
