
// use of timers

// nice fonts
UseNewDefaultFonts( 1 )

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background3.jpg" ) )

// load an image and create a sprite
image = LoadImage ( "button.png" )
sprite = CreateSprite ( image )
SetSpritePosition ( sprite, 100, 170 )

// store the current time
initialTime = GetSeconds ( )

// turn print color to green
SetPrintColor ( 0, 255, 0 )

// main loop
do
    // display instructions
    SetPrintSize(16)
    Print ( "After 4 seconds the button" )
    Print ( "will turn green" )

    // once 4 seconds have elapsed change the sprite's color to green
    if ( GetSeconds ( ) > initialTime + 4 )
        SetSpriteColor ( sprite, 0, 255, 0, 255 )
    endif

    // update the screen
    sync ( )
loop
