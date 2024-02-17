
// find out which sprite has been hit or clicked

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
CreateSprite ( 1, LoadImage ( "background4.jpg" ) )
SetSpriteColorAlpha ( 1, 200 )

// load two images
LoadImage ( 1, "chip5.png" )
LoadImage ( 2, "chip25.png" )

// create sprites using image
CreateSprite ( 2, 1 )
CreateSprite ( 3, 2 )

// set their positions
SetSpritePosition ( 2, 80, 150 )
SetSpritePosition ( 3, 200, 150 )

// main loop
do
    // instructions
    SetPrintSize(16)
    Print ( "Click on a sprite" )
    Print ( "" )

    // display information
    Print ( "Clicked on sprite " + str ( sprite ) )

    // find out which sprite gets hit
    if ( GetPointerPressed ( ) = 1 )
        sprite = GetSpriteHit ( GetPointerX ( ), GetPointerY ( ) )
    endif

    // update the screen
    Sync ( )
loop
