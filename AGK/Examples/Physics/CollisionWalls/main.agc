
// showing how collision walls work

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background2.jpg" ) )

// load a crate image
LoadImage ( 1, "agksicon.jpg" )

// create a sprite
CreateSprite ( 1, 1 )

// set the position and turn physics on
SetSpritePosition ( 1, 150, 50 )
SetSpritePhysicsOn ( 1, 2 )

// remove the bottom physics wall
SetPhysicsWallBottom ( 0 )

// apply an impulse
SetSpritePhysicsImpulse ( 1, 150, 50, 10000, 0 )

// main loop
do
    // update the screen
    Sync ( )
loop
