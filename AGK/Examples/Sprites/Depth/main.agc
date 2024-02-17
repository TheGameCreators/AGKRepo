
// adjusting depth

SetVirtualResolution ( 320, 480 )

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
CreateSprite ( LoadImage ( "background3.jpg" ) )

// penguin
LoadImage ( 1, "penguin.png" )
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 100, 300 )

// football
LoadImage ( 2, "ball1.png" )
CreateSprite ( 2, 2 )
SetSpritePosition ( 2, 125, 300 )

// main loop
do
    // display instructions
    SetPrintSize(16)
    Print ( "Click or touch the screen to change" )
    Print ( "the order in which sprites are drawn" )

    // switch depth on input
    if ( GetPointerPressed ( ) = 1 )
        if ( state = 0 )
            SetSpriteDepth ( 1, 0 )
            SetSpriteDepth ( 2, 1 )
            state = 1
        else
            SetSpriteDepth ( 1, 1 )
            SetSpriteDepth ( 2, 0 )
                state = 0
        endif
    endif

    // update the screen
    Sync ( )
loop
