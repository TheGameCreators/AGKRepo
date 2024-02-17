
// shows how to perform ray casting on sprites

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background5.jpg" ) )

// load two images
LoadImage ( 1, "gold.png" )
LoadImage ( 2, "silver.png" )

// a dynamic sprite
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 130, 0 )
SetSpriteShape ( 1, 1 )

// static sprite on the left
CreateSprite ( 2, 2 )
SetSpritePosition ( 2, 0, 200 )

// static sprite on the right
CreateSprite ( 3, 2 )
SetSpritePosition ( 3, 260, 200 )

// direction variables
direction = 0

// main loop
do
    // if a ray cast takes place change colors of sprites
    if ( SpriteRayCast ( 30, 200, 260, 200 ) = 1 )
        SetSpriteColor ( 2, 255, 0, 0, 255 )
        SetSpriteColor ( 3, 255, 0, 0, 255 )
    else
        SetSpriteColor ( 2, 255, 255, 255, 255 )
        SetSpriteColor ( 3, 255, 255, 255, 255 )
    endif

    // move the sprite down the screen
    if ( direction = 0 )
        SetSpriteY ( 1, GetSpriteY ( 1 ) + 1 )

        if ( GetSpriteY ( 1 ) > 400 )
            direction = 1
        endif
    endif

    // move the sprite up the screen
    if ( direction = 1 )
        SetSpriteY ( 1, GetSpriteY ( 1 ) - 1 )

        if ( GetSpriteY ( 1 ) < 30 )
            direction = 0
        endif
    endif

    // update the screen
    sync ( )
loop
