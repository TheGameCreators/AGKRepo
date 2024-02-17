
// revolution joint

// virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background1.jpg" ) )

// load two images
LoadImage ( 1, "green.png" )
LoadImage ( 2, "small_ball.png" )

// create a static sprite
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 130, 200 )
SetSpritePhysicsOn ( 1, 1 )

// create a dynamic sprite
CreateSprite ( 2, 2 )
SetSpritePosition ( 2, 130, 300 )
SetSpritePhysicsOn ( 2, 2 )
SetSpriteShape ( 2, 1 )

// attach them both together using a revolute joint
CreateRevoluteJoint ( 1, 1, 2, 162, 232, 0 )

// apply an impulse to the dynamic sprite
SetSpritePhysicsImpulse ( 2, 130, 300, 10000, 0 )

// turn physics debug on
SetPhysicsDebugOn ( )

// main loop
do
    // update the screen
    Sync ( )
loop
