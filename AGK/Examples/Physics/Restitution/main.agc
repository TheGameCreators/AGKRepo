
// restitution example

// set virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background7.jpg" ) )

// load an image
LoadImage ( 1, "small_ball.png" )

// create 5 sprites
CreateSprite ( 1, 1 )
CreateSprite ( 2, 1 )
CreateSprite ( 3, 1 )
CreateSprite ( 4, 1 )
CreateSprite ( 5, 1 )

// position in a row
SetSpritePosition ( 1,   0, 0 )
SetSpritePosition ( 2,  60, 0 )
SetSpritePosition ( 3, 120, 0 )
SetSpritePosition ( 4, 180, 0 )
SetSpritePosition ( 5, 240, 0 )

// set their shapes
SetSpriteShape ( 1, 1 )
SetSpriteShape ( 2, 1 )
SetSpriteShape ( 3, 1 )
SetSpriteShape ( 4, 1 )
SetSpriteShape ( 5, 1 )

// turn physics on
SetSpritePhysicsOn ( 1, 2 )
SetSpritePhysicsOn ( 2, 2 )
SetSpritePhysicsOn ( 3, 2 )
SetSpritePhysicsOn ( 4, 2 )
SetSpritePhysicsOn ( 5, 2 )

// alter restitution values to see the effect it has
SetSpritePhysicsRestitution ( 1, 0.0 )
SetSpritePhysicsRestitution ( 2, 0.2 )
SetSpritePhysicsRestitution ( 3, 0.4 )
SetSpritePhysicsRestitution ( 4, 0.8 )
SetSpritePhysicsRestitution ( 5, 1.0 )

// main loop
do
    // update screen
    Sync ( )
loop
