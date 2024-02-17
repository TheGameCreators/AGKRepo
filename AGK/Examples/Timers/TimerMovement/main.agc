
// use of timers

// nice fonts
UseNewDefaultFonts( 1 )

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background2.jpg" ) )

// load an image and create a sprite
LoadImage ( 1, "blue.png" )
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 0, 200 )

// set 60 fps in CPU friendly mode
SetSyncRate( 60, 0 )

// vsync state
fullspeed = 1

// store time
lastFrame# = Timer ( )

// main loop
do
        // store time data
        lastFrame# = GetFrameTime()
        
        // move the sprite and take into account the frame time
        // the box will move at 20 units per second
        // i.e. if the last frame took 0.1 seconds to render the box would 
        // be moved 2 units, if the frame took 1 second the box would 
        // move 20 units to catch up to where it should be
        x# = GetSpriteX ( 1 )
        x# = x# + 20.0 * lastFrame#
        SetSpriteX ( 1, x# )

        // display instructions and information
        SetPrintSize(16)
        Print ( "Touch or click the screen to switch" )
        Print ( "between 60fps and 20fps" )
        Print ( "The box should move at the same speed" )
        Print ( "regardless of frame rate" )
        Print ( "" )
        if ( fullspeed = 1 ) then Print ( "Target Speed = 60fps" ) else Print ( "Target Speed = 20fps" )
        Print ( "Frame rate = " + str ( screenFPS ( ) ) )

        // change vsync on input
        if GetPointerPressed ( ) = 1
                if fullspeed = 1
                        SetSyncRate(20,0)
                        fullspeed = 0
                else
                        SetSyncRate(60,0)
                        fullspeed = 1
                endif
        endif

        // update the screen
        Sync ( )
loop

