
// use of joysticks or keyboards

// set a resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background6.jpg" ) )
SetSpriteColorAlpha ( backdrop, 200 )

// load a sprite and place it near the center of the screen
CreateSprite ( 1, LoadImage ( "agksicon.jpg" ) )
SetSpritePosition ( 1, 160, 200 )

// set the joystick position for platforms that need it
SetJoystickScreenPosition ( 50, 300, 64 )

// main loop
do
    // display instructions
    SetPrintSize( 16 )
    Print ( "Use a real or virtual joystick or" )
    Print ( "use the w, a, s and d keys if a" )
    Print ( "keyboard is present" )
    Print ( "" )

    // obtain joystick input from either a real joystick, keyboard (w, a, s and d) or virtual joystick
    x# = GetJoystickX ( )
    y# = GetJoystickY ( )

    // show joystick input on screen
    Print ( x# )
    Print ( y# )

    // add input to sprites position
    SetSpritePosition ( 1, GetSpriteX ( 1 ) + x#, GetSpriteY ( 1 ) + y# )

    // ensure sprite cannot move past left of screen
    if ( GetSpriteX ( 1 ) < 10 )
        SetSpriteX ( 1, 10 )
    endif

    // ensure sprite cannot move past right of screen
    if ( GetSpriteX ( 1 ) > 260 )
        SetSpriteX ( 1, 260 )
    endif

    // ensure sprite cannot move past top of screen
    if ( GetSpriteY ( 1 ) < 10 )
        SetSpriteY ( 1, 10 )
    endif

    // ensure sprite cannot move past bottom of screen
    if ( GetSpriteY ( 1 ) > 430 )
        SetSpriteY ( 1, 430 )
    endif

    // update contents of screen
    Sync ( )
loop
