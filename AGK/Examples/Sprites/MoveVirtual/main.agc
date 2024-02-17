
// move a sprite on using the virtual resolution

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background1.jpg" ) )

// load an image named blue.png and store its ID
// in the variable image
image = LoadImage ( "blue.png" )

// create a sprite using the previously loaded
// image and store its ID in the variable sprite
sprite = CreateSprite ( image )

// our main loop
do
    // set the position of our sprite 
    // based on the variable x#
    SetSpritePosition ( sprite, x#, 0 )

    // add 0.5 to the variable x#
    x# = x# + 0.5

    // update the screen
    sync ( )
loop
