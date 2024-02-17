
// Show the system timer in milliseconds, and show how to reset it per app
// Introduced in build 107

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background7.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// init code
ResetTimer()

// main loop
do
	// main code
	print("GetMilliseconds() = "+str(GetMilliseconds()))

    // update the screen
    Sync ( )
loop
