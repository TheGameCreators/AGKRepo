// this example shows how to scroll the screen

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// load a football pitch, it is split into two images
LoadImage ( 1, "pitch_top.jpg" )
LoadImage ( 2, "pitch_bottom.jpg" )

// create sprites from the pitch images
CreateSprite ( 1, 1 )
CreateSprite ( 2, 2 )

// position the pitch sprites
SetSpritePosition ( 1, 0, 0 )
SetSpritePosition ( 2, 0, 593 )

// add a logo to the screen and fix its position
LoadImage ( 3, "goals_logo.png" )
CreateSprite ( 3, 3 )
FixSpriteToScreen ( 3, 1 )

// the main loop
do
    // scroll the screen
    SetViewOffset ( x, y )

    // move down and to the right
    if ( direction = 0 )
        x = x + 1
        y = y + 2

        if ( y > 740 )
            direction = 1
        endif
    endif

    // move up and to the left
    if ( direction = 1 )
        x = x - 1
        y = y - 2

        if ( y < 0 )
            direction = 0
        endif
    endif

    // update the screen
    sync ( )
loop