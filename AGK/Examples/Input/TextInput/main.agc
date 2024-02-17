
// input text on screen

// nice fonts
UseNewDefaultFonts( 1 )


// display a background
backdrop = CreateSprite ( LoadImage ( "background1.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// start the process of accepting text input
StartTextInput ( )

// set text string to blank
text$ = ""

// main loop
do
    // once input has finished store the text
    if GetTextInputCompleted ( ) = 1
        text$ = GetTextInput ( )
    endif

    // print text input on screen
    Print ( text$ )

    // update the screen
    Sync ( )
loop
