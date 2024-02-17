
// create a scrolling star field

// array to hold speed values
dim speed [ 1000 ] as float

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// load a background and create a sprite
LoadImage ( 1, "space.jpg" )
CreateSprite ( 1, 1 )

// load a star image
LoadImage ( 2, "star.png" )

// make a load of stars
for i = 2 to 1000
    CreateSprite ( i, 2 )
    SetSpritePosition ( i, Random ( 0, 320 ), Random ( 0, 480 ) )

    // set a random size
    size = Random ( 3, 10 ) / 10.0
    SetSpriteScale ( i, size, size )

    // set a random color
    SetSpriteColor ( i, Random ( 100, 255 ), Random ( 100, 255 ), Random ( 100, 255 ), Random ( 100, 255 ) )

    // sort out a random speed for the stars
    speed [ i ] = Random ( 1, 30 ) / 10.0
next i

// main loop
do
    // update all stars
    for i = 2 to 1000
        // move all stars left
        SetSpriteX ( i, GetSpriteX ( i ) - speed [ i ] )

        // when sprite has left the screen reset it
        if ( GetSpriteX ( i ) < -20 )
            SetSpritePosition ( i, Random ( 330, 400 ), Random ( 0, 480 ) )
            speed [ i ] = Random ( 1, 30 ) / 10.0
        endif
    next i

    // update the screen
    sync ( )
loop
