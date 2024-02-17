
// use of joint motors

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background5.jpg" ) )

// load two images
LoadImage ( 1, "chip5.png" )
LoadImage ( 2, "small_ball.png" )

// create sprite 1 using the crate
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 130, 200 )
SetSpritePhysicsOn ( 1, 1 )

// create sprite 2 using the chip
CreateSprite ( 2, 2 )
SetSpritePosition ( 2, 130, 300 )
SetSpriteShape ( 2, 1 )
SetSpritePhysicsOn ( 2, 2 )

// connect them together using a revolute joint
CreateRevoluteJoint ( 1, 1, 2, 155, 225, 0 )

// turn the motor on for this joint
SetJointMotorOn ( 1, 0.5, 20000 )

// enable physics debug
SetPhysicsDebugOn ( )

// main loop
do
    // update the screen
    Sync ( )
loop
