
// using compound shapes for extra control over collision

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a backdrop
CreateSprite ( LoadImage ( "background4.jpg" ) )

// load a penguin image
LoadImage ( 1, "penguin.png" )

// create sprite 1 - use regular convex shape
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 0, 20 )
SetSpritePhysicsOn ( 1, 2 )
SetSpriteShape ( 1, 3 )

// sprite 2 also uses a regular convex shape and also has an additional box added to it
CreateSprite ( 2, 1 )
SetSpritePosition ( 2, 200, 20 )
SetSpritePhysicsOn ( 2, 2 )
SetSpriteShape ( 2, 3 )
AddSpriteShapeBox ( 2, -40, 62, 40, 64, 0 )

// turn physics debug on
SetPhysicsDebugOn ( )

// main loop
do
    // update the screen
    Sync ( )
loop
