
setvirtualresolution(1024,768)
SetClearColor( 151,170,204 )
SetScissor(0,0,0,0)
SetGenerateMipmaps(1)
SetPrintSize( 20 )
// nice fonts
UseNewDefaultFonts( 1 )

// load JSON exported Spine file
LoadSkeleton2DFromSpriterFile( 1, "GreyGuy.scon", 1, 0 )
// position it near the bottom of the screen
SetSkeleton2DPosition( 1, 512, 760 )

// play the walk animation
PlaySkeleton2DAnimation( 1, "walk", 0, 1, 0.3 )

speed# = 1.0
tweentime# = 0.2
angle# = 10

do
    // Space key
    if GetRawKeyPressed( 32 ) = 1
		// loop it 2 times then stop
        if ( speed# < 0 ) 
			PlaySkeleton2DAnimation( 1, "jump_loop", 10, 2, tweentime# )
        else
            PlaySkeleton2DAnimation( 1, "jump_loop", 0, 2, tweentime# )
        endif
    endif

    // when the animation stops playing after a jump restart the walk animation
    if GetSkeleton2DIsAnimating(1) = 0
        PlaySkeleton2DAnimation( 1, "walk", 0, 1, tweentime# )
    endif

    // W key
    if GetRawKeyState( 87 ) = 1
        head = GetSkeleton2DBone( 1, "head" )
        angle# = angle# - 1.5
        SetSkeleton2DBoneAngle( 1, head, angle# )
    endif
    // S key
    if GetRawKeyState( 83 ) = 1
        head = GetSkeleton2DBone( 1, "head" )
        angle# = angle# + 1.5
        SetSkeleton2DBoneAngle( 1, head, angle# )
    endif

    // A key
    if GetRawKeyState( 65 ) = 1
        speed# = speed# - 0.01
        SetSkeleton2DAnimationSpeed( 1, speed# )
    endif
    // D key
    if GetRawKeyState( 68 ) = 1
        speed# = speed# + 0.01
        SetSkeleton2DAnimationSpeed( 1, speed# )
    endif

    // Q key
    if GetRawKeyState( 81 ) = 1 then tweentime# = 0
    // E key
    if GetRawKeyState( 69 ) = 1 then tweentime# = 0.2

    Print( "Press W and S to tilt the head" )
    Print( "Press A and D to modify the speed" )
    Print( "Press Q and E to turn tweening on and off" )
    Print( "Press Space to jump" )
    print ( screenfps() )
    Print( "Speed: " + str(speed#) )
    if tweentime# > 0
        Print( "Tweening: On" )
    else
        Print( "Tweening: Off" )
    endif

    sync()
loop
