
// place a sprite on screen using the virtual resolution

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// load an image named blue.png and store its ID in the
// variable image
image = LoadImage ( "blue.png" )

// create a sprite using the previously loaded image and
// store its ID in the variable sprite
sprite = CreateSprite ( image )

// our main loop
do
    // update the screen
    sync ( )
loop
