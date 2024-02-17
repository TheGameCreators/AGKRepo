
// printing text on screen

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background3.jpg" ) )
SetSpriteColorAlpha ( backdrop, 200 )
SetSpriteSize ( backdrop, 100, 100 )

// main loop
do
    // display this text
    Print ( "some text" )
    Print ( 123 )
    Print ( 9.473 )

    // update the screen
    Sync ( )
loop
