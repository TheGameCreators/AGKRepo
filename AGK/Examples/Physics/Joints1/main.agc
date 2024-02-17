

// joining sprites together using a weld

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background3.jpg" ) )

// load two images
LoadImage ( 1, "green.png" )
LoadImage ( 2, "small_ball.png" )

// sprite 1
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 100, 0 )
SetSpritePhysicsOn ( 1, 2 )

// sprite 2
CreateSprite ( 2, 1 )
SetSpritePosition ( 2, 164, 0 )
SetSpritePhysicsOn ( 2, 2 )

// weld 1 and 2 together
CreateWeldJoint ( 1, 1, 2, 164, 32, 0 )

// sprite 3
CreateSprite ( 3, 1 )
SetSpritePosition ( 3, 100, 140 )
SetSpritePhysicsOn ( 3, 2 )

// sprite 4
CreateSprite ( 4, 1 )
SetSpritePosition ( 4, 164, 140 )
SetSpritePhysicsOn ( 4, 2 )

// sprite 5
CreateSprite ( 5, 2 )
SetSpritePosition ( 5, 132, 250 )
SetSpritePhysicsOn ( 5, 1 )
SetSpriteShape ( 5, 1 )

// enable physics debug
SetPhysicsDebugOn ( )

// main loop
do
    // update the screen
    Sync ( )
loop
