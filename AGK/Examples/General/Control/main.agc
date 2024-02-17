
// these commands are for general control e.g. setting
// the clear color of the screen

// set the allowed orientations - landscape only
SetOrientationAllowed ( 0, 0, 1, 1 )

// change the clear color
SetClearColor ( 255, 0, 0 )

// nice fonts
UseNewDefaultFonts( 1 )

// main loop
do
    // find out how fast our program runs
    fps = ScreenFPS ( )

    // print the frame rate on screen
    Print ( fps )

    // update the screen
    Sync ( )
loop
