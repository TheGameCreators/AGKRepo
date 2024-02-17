// move a player around the screen by using a virtual joystick

// nice fonts
UseNewDefaultFonts( 1 )

// set the display aspect
SetDisplayAspect ( 4.0 / 3.0 )

// top half of pitch
LoadImage     ( 1, "pitch_top.jpg" )
CreateSprite  ( 1, 1 )
SetSpriteSize ( 1, 100, -1 )

// bottom half of pitch
LoadImage     ( 2, "pitch_bottom.jpg" )
CreateSprite  ( 2, 2 )
SetSpriteSize ( 2, 100, -1 )
SetSpriteY    ( 2, GetSpriteHeight ( 1 ) )

// player
LoadImage         ( 3, "blue-standing.png" )
CreateSprite      ( 3, 3 )
SetSpriteSize     ( 3, 2.5, -1 )
SetSpritePosition ( 3, 50, 50 )

// player animation
AddSpriteAnimationFrame ( 3, LoadImage ( "blue-running-1.png" ) )
AddSpriteAnimationFrame ( 3, LoadImage ( "blue-running-2.png" ) )
AddSpriteAnimationFrame ( 3, LoadImage ( "blue-running-3.png" ) )
AddSpriteAnimationFrame ( 3, LoadImage ( "blue-running-4.png" ) )
AddSpriteAnimationFrame ( 3, LoadImage ( "blue-running-5.png" ) )
AddSpriteAnimationFrame ( 3, LoadImage ( "blue-running-6.png" ) )

// a virtual joystick
AddVirtualJoystick ( 1, 8, 90, 10 )

// main loop
do
    // display instructions
    Print ( "Use the virtual joystick to" )
    Print ( "control the player" )

    // get input from joystick
    joystickX# = GetVirtualJoystickX ( 1 )
    joystickY# = GetVirtualJoystickY ( 1 )

    // find player position
    x# = GetSpriteX ( 3 )
    y# = GetSpriteY ( 3 )

    // work out movement direction
    x1# = x# - joystickX#
    y1# = y# - joystickY#

    // face the correct angle
    angle# = ATanFull ( x1# - x#, y1# - y# )
    SetSpriteAngle ( 3, angle# )

    // update player position
    SetSpritePosition ( 3, GetSpriteX ( 3 ) + ( joystickX# / 5.0 ), GetSpriteY ( 3 ) + ( joystickY# / 5.0 ) )

    // scroll the screen
    SetViewOffset ( 0.0, y# - 50 )

    // when there is no input stop animation
    if ( joystickX# = 0.0 and joystickY# = 0.0 )
        StopSprite ( 3 )
        SetSpriteFrame ( 3, 3 )
    else
        // play animation
        if ( GetSpritePlaying ( 3 ) = 0 )
            PlaySprite ( 3, 10, 1, 2, 7 )
        endif
    endif

    // update the screen
    sync ( )
loop
