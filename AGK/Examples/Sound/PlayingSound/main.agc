// shows how to load and play a sound

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background2.jpg" ) )
SetSpriteColorAlpha ( backdrop, 100 )
SetSpriteSize ( backdrop, 100, 100 )

// load a sound file
sound = LoadSound ( "countdown.wav" )

// main loop
do
    // display instructions
    Print ( "Touch or click the screen" )
    Print ( "to play a sound" )

    // when there is input play the sound
    if GetPointerPressed ( ) = 1
        PlaySound ( sound )
    endif

    // update the screen
    sync ( )
loop
