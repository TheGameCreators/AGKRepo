
setvirtualresolution(1024,768)
SetClearColor( 151,170,204 )
SetScissor(0,0,0,0)
SetGenerateMipmaps(1)
SetPrintSize( 20 )
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts


// create blank sprites (1001-1009) and text (1-9)
CreateText( 1, "Linear:" )    : SetTextPosition( 1, 10,107 ) : SetTextSize( 1, 15 )
CreateText( 2, "Smooth1:" )   : SetTextPosition( 2, 10,157 ) : SetTextSize( 2, 15 )
CreateText( 3, "Smooth2:" )   : SetTextPosition( 3, 10,207 ) : SetTextSize( 3, 15 )
CreateText( 4, "EaseIn1:" )   : SetTextPosition( 4, 10,257 ) : SetTextSize( 4, 15 )
CreateText( 5, "EaseIn2:" )   : SetTextPosition( 5, 10,307 ) : SetTextSize( 5, 15 )
CreateText( 6, "EaseOut1:" )  : SetTextPosition( 6, 10,357 ) : SetTextSize( 6, 15 )
CreateText( 7, "EaseOut2:" )  : SetTextPosition( 7, 10,407 ) : SetTextSize( 7, 15 )
CreateText( 8, "Bounce:" )    : SetTextPosition( 8, 10,457 ) : SetTextSize( 8, 15 )
CreateText( 9, "Overshoot:" ) : SetTextPosition( 9, 10,507 ) : SetTextSize( 9, 15 )

CreateSprite( 1001, 0 ) : SetSpritePosition( 1001, 110, 100 ) : SetSpriteSize( 1001, 30,30 )
CreateSprite( 1002, 0 ) : SetSpritePosition( 1002, 110, 150 ) : SetSpriteSize( 1002, 30,30 )
CreateSprite( 1003, 0 ) : SetSpritePosition( 1003, 110, 200 ) : SetSpriteSize( 1003, 30,30 )
CreateSprite( 1004, 0 ) : SetSpritePosition( 1004, 110, 250 ) : SetSpriteSize( 1004, 30,30 )
CreateSprite( 1005, 0 ) : SetSpritePosition( 1005, 110, 300 ) : SetSpriteSize( 1005, 30,30 )
CreateSprite( 1006, 0 ) : SetSpritePosition( 1006, 110, 350 ) : SetSpriteSize( 1006, 30,30 )
CreateSprite( 1007, 0 ) : SetSpritePosition( 1007, 110, 400 ) : SetSpriteSize( 1007, 30,30 )
CreateSprite( 1008, 0 ) : SetSpritePosition( 1008, 110, 450 ) : SetSpriteSize( 1008, 30,30 )
CreateSprite( 1009, 0 ) : SetSpritePosition( 1009, 110, 500 ) : SetSpriteSize( 1009, 30,30 )

// tween for sprite 1001, applied later
CreateTweenSprite( 1, 2.0 ) // TweenID, duration in seconds
SetTweenSpriteX( 1, 110,500, TweenLinear() ) // tweenID, startX, endX, interpolation method
SetTweenSpriteAngle( 1, 0,360, TweenLinear() ) // tweenID, startAng, endAng, interpolation method

// tween for sprite 1002
CreateTweenSprite( 2, 2.0 )
SetTweenSpriteX( 2, 110,500, TweenSmooth1() )
SetTweenSpriteAngle( 2, 0,360, TweenSmooth1() )

// tween for sprite 1003
CreateTweenSprite( 3, 2.0 )
SetTweenSpriteX( 3, 110,500, TweenSmooth2() )
SetTweenSpriteAngle( 3, 0,360, TweenSmooth2() )

// tween for sprite 1004
CreateTweenSprite( 4, 2.0 )
SetTweenSpriteX( 4, 110,500, TweenEaseIn1() )
SetTweenSpriteAngle( 4, 0,360, TweenEaseIn1() )

// tween for sprite 1005
CreateTweenSprite( 5, 2.0 )
SetTweenSpriteX( 5, 110,500, TweenEaseIn2() )
SetTweenSpriteAngle( 5, 0,360, TweenEaseIn2() )

// tween for sprite 1006
CreateTweenSprite( 6, 2.0 )
SetTweenSpriteX( 6, 110,500, TweenEaseOut1() )
SetTweenSpriteAngle( 6, 0,360, TweenEaseOut1() )

// tween for sprite 1007
CreateTweenSprite( 7, 2.0 )
SetTweenSpriteX( 7, 110,500, TweenEaseOut2() )
SetTweenSpriteAngle( 7, 0,360, TweenEaseOut2() )

// tween for sprite 1008
CreateTweenSprite( 8, 2.0 )
SetTweenSpriteX( 8, 110,500, TweenBounce() )
SetTweenSpriteAngle( 8, 0,360, TweenBounce() )

// tween for sprite 1009
CreateTweenSprite( 9, 2.0 )
SetTweenSpriteX( 9, 110,500, TweenOvershoot() )
SetTweenSpriteAngle( 9, 0,360, TweenOvershoot() )

// create the chain and apply to sprites
CreateTweenChain( 1 ) // chainID
AddTweenChainSprite( 1, 1, 1001, 1.0 ) // chainID, tweenID, spriteID, delay in seconds
AddTweenChainSprite( 1, 2, 1002, 1.0 )
AddTweenChainSprite( 1, 3, 1003, 1.0 )
AddTweenChainSprite( 1, 4, 1004, 1.0 )
AddTweenChainSprite( 1, 5, 1005, 1.0 )
AddTweenChainSprite( 1, 6, 1006, 1.0 )
AddTweenChainSprite( 1, 7, 1007, 1.0 )
AddTweenChainSprite( 1, 8, 1008, 1.0 )
AddTweenChainSprite( 1, 9, 1009, 1.0 )

// start the chain
PlayTweenChain( 1 ) // chainID

chainended = 0

do

    // if the chain has ended fade everything out
    if GetTweenChainPlaying( 1 ) = 0 and chainended = 0
        chainended = 1

        // tween to fade sprites over 3 seconds
        CreateTweenSprite( 10, 3.0 )
        SetTweenSpriteAlpha( 10, 255, 0, TweenEaseOut1() )

        // tween to fade text over 3 seconds
        CreateTweenText( 11, 3.0 )
        SetTweenTextAlpha( 11, 255, 0, TweenEaseOut1() )

        // apply the tweens to the sprites and text, delay each one to create a wave effect
        PlayTweenSprite( 10, 1001, 1.0 ) : PlayTweenText( 11, 1, 1.0 )
		PlayTweenSprite( 10, 1002, 1.1 ) : PlayTweenText( 11, 2, 1.1 )
		PlayTweenSprite( 10, 1003, 1.2 ) : PlayTweenText( 11, 3, 1.2 )
		PlayTweenSprite( 10, 1004, 1.3 ) : PlayTweenText( 11, 4, 1.3 )
		PlayTweenSprite( 10, 1005, 1.4 ) : PlayTweenText( 11, 5, 1.4 )
		PlayTweenSprite( 10, 1006, 1.5 ) : PlayTweenText( 11, 6, 1.5 )
		PlayTweenSprite( 10, 1007, 1.6 ) : PlayTweenText( 11, 7, 1.6 )
		PlayTweenSprite( 10, 1008, 1.7 ) : PlayTweenText( 11, 8, 1.7 )
		PlayTweenSprite( 10, 1009, 1.8 ) : PlayTweenText( 11, 9, 1.8 )
    endif

    print ( screenfps() )
    Print( "Playing Chain: " + str( GetTweenChainPlaying(1) ) )
	Print( "Playing Fade: " + str( GetTweenSpritePlaying(10,1009) ) )

	// this won't be done automatically!
	UpdateAllTweens( getframetime() )

    sync()
loop
