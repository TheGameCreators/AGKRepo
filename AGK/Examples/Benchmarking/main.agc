
// using the benchmarking commands

// arrays for sprites
dim speed# [ 500 ]
dim move#  [ 500 ]

// set the resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background5.jpg" ) )

//Use ttf fonts
UseNewDefaultFonts( 1 )

// load an image for the particles and sprites
LoadImage ( 1, "shrapnel3.png" )
LoadImage ( 2, "ball1.png" )

// create particles off screen
CreateParticles ( 1, -100, -100 )

// create a batch of 500 sprites
for i = 1 to 500
	CreateSprite ( i, 2 )
	SetSpritePosition ( i, Random ( 320, 2000 ), Random ( 60, 480 ) )
	SetSpriteColor ( i, Random ( 0, 255 ), Random ( 0, 255 ), Random ( 0, 255 ), Random ( 50, 255 ) )
	speed# [ i ] = Random ( 10, 40 ) / 10.0
	move# [ i ] = 0.0
next i

// set fire variable to 1
fire = 1

// main loop
do
	// show statistics
	Print ( "Drawing set up time      = " + str ( GetDrawingSetupTime ( ) ) )
	Print ( "Drawing time             = " + str ( GetDrawingTime ( ) ) )
	Print ( "Sprites drawn            = " + str ( GetManagedSpriteDrawnCount ( ) ) )
	Print ( "Particles drawn          = " + str ( GetParticleDrawnQuadCount ( ) ) )
	Print ( "Frame rate               = " + str ( screenFPS ( ) ) )

    // build 107 additions
	Print ( "Get pixels drawn         = " + str ( GetPixelsDrawn ( ) ) )
	Print ( "Get managed sprite calls = " + str ( GetManagedSpriteDrawCalls ( ) ) )

	// move all sprites
	for i = 1 to 500
		x# = GetSpriteX ( i )
		y# = GetSpriteY ( i )

		y# = y# + cos ( move# [ i ] )
		move# [ i ] = move# [ i ] + speed# [ i ]

		SetSpritePosition ( i, x# - speed# [ i ], y# )
		SetSpriteAngle ( i, GetSpriteAngle ( i ) + speed# [ i ] )

		if x# < -100
			SetSpritePosition ( i, Random ( 320, 2000 ), Random ( 60, 480 ) )
		endif
	next i

	// fire particles when ready
    if ( fire = 1 )
        // set up particles
        SetParticlesPosition ( 1, Random ( 100, 200 ), Random ( 100, 300 ) )

        ResetParticleCount ( 1 )
        SetParticlesFrequency ( 1, 250 )
        SetParticlesLife ( 1, 3.0 )
        SetParticlesSize ( 1, 64 )
        SetParticlesStartZone ( 1, -10, 0, 10, 0 )
        SetParticlesImage ( 1, 1 )
        SetParticlesDirection ( 1, 30, 30 )
        SetParticlesAngle ( 1, 360 )
        SetParticlesVelocityRange ( 1, 0.8, 2.5 )
        SetParticlesMax ( 1, 500 )

        AddParticlesColorKeyFrame ( 1, 0.0, 0, 0, 0, 0 )
        AddParticlesColorKeyFrame ( 1, 0.5, 255, 255, 0, 255 )
        AddParticlesColorKeyFrame ( 1, 2.8, 255, 0, 0, 0 )

        AddParticlesForce ( 1, 2.0, 2.8, 25, -25 )

        // reset fire value
        fire = 0
    endif

    // when the explosion has finished it is safe to fire once more
    if ( GetParticlesMaxReached ( 1 ) = 1 )
        fire = 1
    endif

    // update the screen
    Sync ( )
loop
