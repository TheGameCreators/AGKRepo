
// create and display text

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background7.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// create text
CreateText ( 10, "HELLO AppGameKit Studio!" )

// set its size
SetTextSize ( 10, 10 )


// main loop
do
    // apply a random color to the text entity
    SetTextColor ( 10, Random ( 1, 255 ), Random ( 1, 255 ), Random ( 1, 255 ), 255 )


    // update the screen
    Sync ( )
loop
