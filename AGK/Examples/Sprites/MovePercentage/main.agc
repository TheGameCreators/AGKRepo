
// move a sprite on screen using the percentage system

// load an image named blue.png and store its ID
// in the variable image
image = LoadImage ( "blue.png" )

// display a background
backdrop = CreateSprite ( LoadImage ( "background3.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// create a sprite using the previously loaded image
// and store its ID in the variable sprite
sprite = CreateSprite ( image )

// set the size of our sprite to be 20% on the X axis
// and work out the Y size automatically
SetSpriteSize ( sprite, 20, -1 )

// our main loop
do
    // set the position of the sprite
    SetSpritePosition ( sprite, x#, 0 )

    // increment the X variable by 0.25%
    x# = x# + 0.25

    // update the screen
    sync ( )
loop
