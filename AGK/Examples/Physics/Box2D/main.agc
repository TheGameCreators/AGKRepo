
// getting started with box 2d physics

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background3.jpg" ) )

// load an image
LoadImage ( 1, "small_ball.png" )

// create 5 sprites
CreateSprite ( 1, 1 )
CreateSprite ( 2, 1 )
CreateSprite ( 3, 1 )
CreateSprite ( 4, 1 )
CreateSprite ( 5, 1 )

// position the sprites in a row
SetSpritePosition ( 1, 0, 0 )
SetSpritePosition ( 2, 60, 0 )
SetSpritePosition ( 3, 120, 0 )
SetSpritePosition ( 4, 180, 0 )
SetSpritePosition ( 5, 240, 0 )

// turn physics on for all of them
SetSpritePhysicsOn ( 1, 2 )
SetSpritePhysicsOn ( 2, 2 )
SetSpritePhysicsOn ( 3, 2 )
SetSpritePhysicsOn ( 4, 2 )
SetSpritePhysicsOn ( 5, 2 )

// main loop
do
    // update the screen
    Sync ( )
loop
