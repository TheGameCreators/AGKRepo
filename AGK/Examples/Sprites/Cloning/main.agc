
// shows how to clone a sprite

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// nice fonts
UseNewDefaultFonts( 1 )
SetPrintSize(26)

// display a background
CreateSprite ( LoadImage ( "background2.jpg" ) )

// load a penguin image
LoadImage ( 1, "penguin.png" )

// create the base sprite and set a few properties
CreateSprite      ( 1, 1 )
SetSpritePosition ( 1, 200, 200 )
SetSpriteScale    ( 1, 0.6, 0.6 )
SetSpriteAngle    ( 1, 30 )
SetSpriteColor    ( 1, 0, 255, 255, 200 )
SetSpriteShape    ( 1, 3 )

// turn debugging on
SetPhysicsDebugOn ( )

// set print color to red
SetPrintColor ( 255, 0, 0, 255 )

// main loop
do
    Print ( "Click or touch the screen to" )
	Print ( "clone a sprite at that point" )

    // check for input
    if ( GetPointerPressed ( ) = 1 )
        // clone sprite 1 and store its ID in newSprite
        newSprite = CloneSprite ( 1 )

        // set the cloned sprite's position to the location of input
        SetSpritePosition ( newSprite, GetPointerX ( ), GetPointerY ( ) )
    endif

    // update the screen
    Sync ( )
loop
