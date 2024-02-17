
// customising convex shapes

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background6.jpg" ) )

// load the penguin image
LoadImage ( 1, "penguin.png" )

// set the x position to 0
x# = 0

// create three sprites
for i = 1 to 3

    // change maximum number of polygon points for each convex shape
    if i = 1 then SetPhysicsMaxPolygonPoints ( 5 )
    if i = 2 then SetPhysicsMaxPolygonPoints ( 8 )
    if i = 3 then SetPhysicsMaxPolygonPoints ( 12 )

    // create the sprite
    sprite = CreateSprite ( 1 )

    // set its position and other properties
    SetSpritePosition           ( sprite, x#, 0 )
    SetSpriteShape              ( sprite, 3 )
    SetSpritePhysicsOn          ( sprite, 2 )
    SetSpriteScale              ( sprite, 0.6, 0.6 )
    SetSpritePhysicsRestitution ( sprite, 0.5 )

    // increment the x position
    x# = x# + 100
next i

// turn physics debug on
SetPhysicsDebugOn ( )

// main loop
do
    // update the screen
    Sync ( )
loop

