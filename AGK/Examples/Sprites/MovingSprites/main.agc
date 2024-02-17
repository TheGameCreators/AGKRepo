
// move a sprite when the user touches or clicks the screen

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background4.jpg" ) )

// load an image for the sprite
image = LoadImage ( "lime.png" )

// create a sprite using the image
sprite = CreateSprite ( image )

// the main loop
do
    // display instructions
    Print ( "Touch or click the screen to move the" )
    Print ( "sprite to that location" )

    // when the user touches or clicks the screen
    if GetPointerPressed ( ) = 1

        // store the location of input
        x = GetPointerX ( )
        y = GetPointerY ( )

        // if it's okay to move the sprite
        if ( move = 0 )
            // allow movement
            move = 1

            // save the current position
            originalX# = GetSpriteX ( sprite )
            originalY# = GetSpriteY ( sprite )

            // work out the destination
            destinationX# = x
            destinationY# = y
            distanceX# = destinationX# - originalX#
            distanceY# = destinationY# - originalY#
            distanceFromAtoB# = sqrt ( ( distanceX# * distanceX# ) + ( distanceY# * distanceY# ) )

            if ( distanceFromAtoB# <> 0.0 )
                directionX# = distanceX# / distanceFromAtoB#
                directionY# = distanceY# / distanceFromAtoB#
            endif
        endif
    endif

    // check if it's okay to move
    if ( move > 0 )
        // work out new location
        newX# = originalX# + directionX# * move
        newY# = originalY# + directionY# * move

        // increment our move variable as long as we haven't reached our destination
        if ( move < distanceFromAtoB# )
            move = move + 2
        else
            // reset variables
            move = 0
        endif

        // update position of our sprite
        SetSpritePosition ( sprite, newX#, newY# )
    endif

    // update the screen
    Sync ( )
loop
