
// setting sprites to groups and finding out which one was hit or clicked

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background2.jpg" ) )
SetSpriteColorAlpha ( backdrop, 100 )

// load two images
LoadImage ( 1, "chip5.png" )
LoadImage ( 2, "chip25.png" )

// create 4 sprites and assign group 1
for i = 1 to 4
    sprite = CreateSprite ( 1 )

    SetSpritePosition  ( sprite, Random ( 10, 280 ), Random ( 50, 400 ) )
    SetSpritePhysicsOn ( sprite, 1 )
    SetSpriteGroup     ( sprite, 1 )
next i

// create 4 sprites and assign group 2
for i = 1 to 4
    sprite = CreateSprite ( 2 )

    SetSpritePosition  ( sprite, Random ( 10, 280 ), Random ( 50, 400 ) )
    SetSpritePhysicsOn ( sprite, 1 )
    SetSpriteGroup     ( sprite, 2 )
next i

sprite = 0

// main loop
do
    // display instructions
    SetPrintSize(16)
    Print ( "Click on a group 2 sprite (blue)" )
    Print ( "" )

    // display information
    Print ( "Group 2 sprite ID:" )
    Print ( sprite )

    // check for input and find out which group 2 sprite was hit
    if ( GetPointerPressed ( ) = 1 )
        sprite = GetSpriteHitGroup ( 2, GetPointerX ( ), GetPointerY ( ) )
    endif

    // update the screen
    Sync ( )
loop
