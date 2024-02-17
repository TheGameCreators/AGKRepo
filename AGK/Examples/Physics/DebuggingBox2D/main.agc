
// debug mode for box2D

// set a resolution
SetVirtualResolution ( 320, 480 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background2.jpg" ) )
SetSpriteColorAlpha ( backdrop, 100 )

// load an image
LoadImage ( 1, "agksicon.jpg" )

// create 5 sprites
CreateSprite ( 1, 1 )
CreateSprite ( 2, 1 )
CreateSprite ( 3, 1 )
CreateSprite ( 4, 1 )
CreateSprite ( 5, 1 )

// position all sprites
SetSpritePosition ( 1, 50, 0 )
SetSpritePosition ( 2, 100, 0 )
SetSpritePosition ( 3, 140, 360 )
SetSpritePosition ( 4, 170, 0 )
SetSpritePosition ( 5, 220, 0 )

// set dynamic and static states
SetSpritePhysicsOn ( 1, 2 )
SetSpritePhysicsOn ( 2, 2 )
SetSpritePhysicsOn ( 3, 1 )
SetSpritePhysicsOn ( 4, 2 )
SetSpritePhysicsOn ( 5, 2 )

// turn debug mode on
SetPhysicsDebugOn ( )

// main loop
do
    // update the screen
    Sync ( )
loop
