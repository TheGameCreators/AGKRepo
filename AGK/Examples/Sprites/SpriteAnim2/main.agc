
// animate a sprite part 2

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background.jpg" ) )

// load an image that contains several frames of animation
LoadImage ( 1, "mole.png" )

// create a sprite that uses the image
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 125, 200 )

// set up sprite data
SetSpriteAnimation ( 1, 82, 88, 11 )

// play the sprite
PlaySprite ( 1, 10, 1, 1, 11 )

// our main loop
do
    // update the screen
    Sync ( )
loop
