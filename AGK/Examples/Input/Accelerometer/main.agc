
// move a sprite around using the accelerometer, platforms like
// Windows, Mac and Meego can fall back to the keyboard

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
CreateSprite ( LoadImage ( "background3.jpg" ) )

// create a sprite and position it in the center of the screen
CreateSprite ( 1, LoadImage ( "agksicon.jpg" ) )
SetSpritePosition ( 1, 160, 200 )

// main loop
do
    // display instructions
    SetPrintSize(16)
    Print ( "Tilt the device to move the sprite" )
    Print ( "Or use the arrow keys if a keyboard" )
    Print ( "is present" )
    Print ( "" )

    // obtain information about the accelerometer
    x# = GetDirectionX ( )
    y# = GetDirectionY ( )

    // print accelerometer data on screen
    Print ( x# )
    Print ( y# )

    // add accelerometer data to our sprites position
    SetSpritePosition ( 1, GetSpriteX ( 1 ) + x#, GetSpriteY ( 1 ) + y# )

    // make sure sprite can't move past the left of the screen
    if ( GetSpriteX ( 1 ) < 10 )
        SetSpriteX ( 1, 10 )
    endif

    // make sure sprite can't move past the right of the screen
    if ( GetSpriteX ( 1 ) > 260 )
        SetSpriteX ( 1, 260 )
    endif

    // make sure sprite can't move past the top of the screen
    if ( GetSpriteY ( 1 ) < 10 )
        SetSpriteY ( 1, 10 )
    endif

    // make sure sprite can't move past the bottom of the screen
    if ( GetSpriteY ( 1 ) > 430 )
        SetSpriteY ( 1, 430 )
    endif

    // update the contents of the screen
    sync ( )
loop
