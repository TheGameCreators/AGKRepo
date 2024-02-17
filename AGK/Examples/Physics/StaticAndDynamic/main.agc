
// static and dynamic sprites

// virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background7.jpg" ) )

// load an image
LoadImage ( 1, "small_blue.png" )

// create 5 sprites
CreateSprite ( 1, 1 )
CreateSprite ( 2, 1 )
CreateSprite ( 3, 1 )
CreateSprite ( 4, 1 )
CreateSprite ( 5, 1 )

// position our sprites
SetSpritePosition ( 1, 50, 0 )
SetSpritePosition ( 2, 100, 0 )
SetSpritePosition ( 3, 140, 200 )
SetSpritePosition ( 4, 170, 0 )
SetSpritePosition ( 5, 220, 0 )

// set 1 and 2 to be dynamic, 3 to be static and 4 and 5 to be dynamic
SetSpritePhysicsOn ( 1, 2 )
SetSpritePhysicsOn ( 2, 2 )
SetSpritePhysicsOn ( 3, 1 )
SetSpritePhysicsOn ( 4, 2 )
SetSpritePhysicsOn ( 5, 2 )

// main loop
do
    // update the screen
    Sync ( )
loop
