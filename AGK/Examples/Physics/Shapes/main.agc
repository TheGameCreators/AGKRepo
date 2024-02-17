
// different kinds of collision shapes

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background3.jpg" ) )

// load a penguin image
LoadImage ( 1, "penguin.png" )

// create sprite 1 - convex
CreateSprite                ( 1, 1 )
SetSpritePosition           ( 1, 0, 0 )
SetSpriteShape              ( 1, 3 )
SetSpritePhysicsOn          ( 1, 2 )
SetSpriteScale              ( 1, 0.6, 0.6 )
SetSpritePhysicsRestitution ( 1, 0.5 )

// create sprite 2 - circle
CreateSprite                ( 2, 1 )
SetSpritePosition           ( 2, 100, 0 )
SetSpriteShape              ( 2, 1 )
SetSpritePhysicsOn          ( 2, 2 )
SetSpriteScale              ( 2, 0.6, 0.6 )
SetSpritePhysicsRestitution ( 2, 0.5 )

// create sprite 3 - box
CreateSprite                ( 3, 1 )
SetSpritePosition           ( 3, 200, 0 )
SetSpriteShape              ( 3, 2 )
SetSpritePhysicsOn          ( 3, 2 )
SetSpriteScale              ( 3, 0.6, 0.6 )
SetSpritePhysicsRestitution ( 3, 0.5 )

// turn physics debug on
SetPhysicsDebugOn ( )

// main loop
do
    // update the screen
    Sync ( )
loop
