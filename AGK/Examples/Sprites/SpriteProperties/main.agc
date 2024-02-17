
// set properties of sprites

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background5.jpg" ) )

// load the image blue.png into ID slot 1
LoadImage ( 1, "blue.png" )

// create 5 sprites that all use image 1
CreateSprite ( 1, 1 )
CreateSprite ( 2, 1 )
CreateSprite ( 3, 1 )
CreateSprite ( 4, 1 )
CreateSprite ( 5, 1 )

// align sprites in a row
SetSpritePosition ( 1,   0, 200 )
SetSpritePosition ( 2,  70, 200 )
SetSpritePosition ( 3, 140, 200 )
SetSpritePosition ( 4, 210, 200 )
SetSpritePosition ( 5, 280, 200 )

// our main loop
do
    // set sprite 2 to a random color
    SetSpriteColor ( 2, Random ( 1, 255 ), Random ( 1, 255 ), Random ( 1, 255 ), 255 )

    // control alpha on sprite 3
    SetSpriteColor ( 3, 255, 255, 255, alpha )

    // move alpha for sprite 3 down
    if ( direction = 0 )
        alpha = alpha - 4

        if ( alpha < 0 )
            direction = 1
        endif
    endif

    // move alpha for sprite 3 up
    if ( direction = 1 )
        alpha = alpha + 4

        if ( alpha > 255 )
            direction = 0
        endif
    endif

    // set the angle of sprite 4
    SetSpriteAngle ( 4, angle )
    angle = angle + 1

    // change the size of sprite 5
    SetSpriteSize ( 5, GetSpriteWidth ( 5 ) - 1, GetSpriteHeight ( 5 ) - 1 )

    // update the screen
    sync ( )
loop
