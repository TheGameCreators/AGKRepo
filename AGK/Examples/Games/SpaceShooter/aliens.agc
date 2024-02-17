
function CreateAliens ( )
    // create 10 aliens

    // loadi mages
    g_iShip      = LoadImage ( "space/alienshipleft.png" )
	g_iParticles = LoadImage ( "space/shrapnel3.png" )

    // set initial data
    for i = 1 to 10
		g_Aliens [ i ].iSprite = 0
		g_Aliens [ i ].iEngine = 0

		g_Aliens [ i ].fSpeed#  = 0.0
		g_Aliens [ i ].fValueA# = 0.0
		g_Aliens [ i ].fValueB# = 0.0
	next i

    // this will build them
    ResetAliens ( )
endfunction

function CreateAlienExplosions ( )
    // create 5 explosions
    for i = 1 to 5
		g_Explosions [ i ].iID = CreateParticles ( -1000, -1000 )
		g_Explosions [ i ].iActive = 0
	next i
endfunction

function UpdateAliens ( )
    // update the aliens
	for i = 1 to 10
        // get the new location
		fShipX#   = GetSpriteX    ( g_Aliens [ i ].iSprite ) - g_Aliens [ i ].fSpeed#
		fShipY#   = GetSpriteY    ( g_Aliens [ i ].iSprite )
		fEngineX# = GetParticlesX ( g_Aliens [ i ].iEngine ) - g_Aliens [ i ].fSpeed#
		fEngineY# = GetParticlesY ( g_Aliens [ i ].iEngine )

        // apply wavy movement
		g_Aliens [ i ].fValueB# = g_Aliens [ i ].fValueB# + g_Aliens [ i ].fValueA#
		fShipY# = fShipY# + cos ( g_Aliens [ i ].fValueB# * 180 / 3.14 )
        fEngineY# = fEngineY# + cos ( g_Aliens [ i ].fValueB# * 180 / 3.14 )

        // reset once they have moved off the screen
		if ( WorldToScreenX ( fEngineX# ) < -200 )
            // new position off to the right
			fShipX# = 550 + Random ( 20, 600 )
			fShipY# =       Random ( 100, 300 )

            // update engine position
			fEngineX# = fShipX# + 30
			fEngineY# = fShipY# + 10

            // convert to world position instead of screen (because of scrolling)
			fShipX#   = ScreenToWorldX ( fShipX# )
			fShipY#   = ScreenToWorldY ( fShipY# )
			fEngineX# = ScreenToWorldX ( fEngineX# )
			fEngineY# = ScreenToWorldY ( fEngineY# )

            // generate new values for movement
			speed# = Random ( 10, 20 )
			g_Aliens [ i ].fSpeed# = speed# / 10.0
			speed# = Random ( 1, 10 )
			g_Aliens [ i ].fValueA# = speed# / 100.0
			g_Aliens [ i ].fValueB# = 0.0

            // clear particle colors and add new keyframes
			ClearParticlesColors ( g_Aliens [ i ].iEngine )
			AddParticlesColorKeyFrame ( g_Aliens [ i ].iEngine, 0.0, Random ( 0, 255 ), Random ( 0, 255 ), 0, 255 )
			AddParticlesColorKeyFrame ( g_Aliens [ i ].iEngine, 0.4, 255, 255, 0, 0 )
		endif

        // update position of alien and engine
		SetSpritePosition    ( g_Aliens [ i ].iSprite, fShipX#, fShipY# )
		SetParticlesPosition ( g_Aliens [ i ].iEngine, fEngineX#, fEngineY# )
	next i
endfunction

function ResetAliens ( )
    // reset all aliens

    // run through them
    for i = 1 to 10
        // set position
        iX = 550 + Random ( 20, 600 )
		iY = Random ( 100, 300 )

        // delete alien if it exists
		if GetSpriteExists ( g_Aliens [ i ].iSprite ) = 1
			DeleteSprite ( g_Aliens [ i ].iSprite )
        endif

        // delete engine if it exists
		if GetParticlesExists ( g_Aliens [ i ].iEngine ) = 1
			DeleteParticles ( g_Aliens [ i ].iEngine )
        endif

        // create alien and engine
        g_Aliens [ i ].iSprite = CreateSprite ( g_iShip )
		g_Aliens [ i ].iEngine = CreateParticles ( iX + 30, iY + 10 )

        // give it a box shape
		SetSpriteShape    ( g_Aliens [ i ].iSprite, 2 )
		SetSpritePosition ( g_Aliens [ i ].iSprite, iX, iY )

        // set up engine
		SetParticlesImage         ( g_Aliens [ i ].iEngine, g_iParticles )
		SetParticlesStartZone     ( g_Aliens [ i ].iEngine, 0.0, 0.0, 0.0, 0.0 )
		SetParticlesDirection     ( g_Aliens [ i ].iEngine, 50.0, 0.0 )
		SetParticlesLife          ( g_Aliens [ i ].iEngine, 0.4 )
		SetParticlesSize          ( g_Aliens [ i ].iEngine, 16.0 )
		SetParticlesAngle         ( g_Aliens [ i ].iEngine, 20.0 )
		SetParticlesFrequency     ( g_Aliens [ i ].iEngine, 24.0 )
		SetParticlesVelocityRange ( g_Aliens [ i ].iEngine, 0.5, 2.0 )
		AddParticlesColorKeyFrame ( g_Aliens [ i ].iEngine, 0.0, Random ( 0, 255 ), Random ( 0, 255 ), 0, 255 )
		AddParticlesColorKeyFrame ( g_Aliens [ i ].iEngine, 0.4, 255, 255, 0, 0 )

        // add one of four forces
		mode = Random ( 1, 4 )

		if ( mode = 1 ) then AddParticlesForce ( g_Aliens [ i ].iEngine, 0.2, 0.4, -1500.0, -1550.0 )
		if ( mode = 2 ) then AddParticlesForce ( g_Aliens [ i ].iEngine, 0.2, 0.4,  1500.0,  1550.0 )
		if ( mode = 3 ) then AddParticlesForce ( g_Aliens [ i ].iEngine, 0.2, 0.4,   100.0,   100.0 )
		if ( mode = 4 ) then AddParticlesForce ( g_Aliens [ i ].iEngine, 0.2, 0.4,  -100.0,  -100.0 )

        // set up for movement
		speed# = Random ( 10, 20 )
		g_Aliens [ i ].fSpeed# = speed# / 10.0
		speed# = Random ( 1, 10 )
		g_Aliens [ i ].fValueA# = speed# / 100.0
		g_Aliens [ i ].fValueB# = 0.0
    next i
endfunction

function ExplodeAlien ( x#, y# )

    // destroy an alien

    // find which explosion from the 5 is available
    for i = 1 to 5
		if ( GetParticlesMaxReached ( g_Explosions [ i ].iID ) = 1 )
			ClearParticlesColors ( g_Explosions [ i ].iID )
			ClearParticlesForces ( g_Explosions [ i ].iID )
			g_Explosions [ i ].iActive = 0
		endif
	next i

    ID = 0

    for i = 1 to 5
        if g_Explosions [ i ].iActive = 0
            ID = i
            exit
        endif
    next i

    // back out if all 5 are being used
    if ID = 0
        return
    endif

    // store the ID
    explode = g_Explosions [ ID ].iID

    // reset
    ResetParticleCount ( explode )

    // set position
    SetParticlesPosition ( explode, x#, y# )

    // set properties
    SetParticlesFrequency ( explode, 250 )
    SetParticlesLife ( explode, 1.0 )
    SetParticlesSize ( explode, 32 )
    SetParticlesStartZone( explode, -10, 0, 10, 0 )
    SetParticlesImage ( explode, 4 )
    SetParticlesDirection( explode, 20, 20.0 )
    SetParticlesAngle ( explode, 360.0 )
    AddParticlesColorKeyFrame ( explode, 0.0, 0, 0, 0, 0 )
    AddParticlesColorKeyFrame ( explode, 0.2, Random ( 0, 100 ), Random ( 100, 255 ), Random ( 100, 255 ), 255 )
    AddParticlesColorKeyFrame ( explode, 1.0, 0, 100, 255, 0 )
    AddParticlesForce ( explode, 0.2, 0.8, 400.0, 0.0 )

    // set up a random force for the particles
    a = Random ( 200, 1000 )

    if ( Random ( 1, 2 ) = 1 )
        a = -a
    endif

    b = Random ( 500, 5000 )
    c = 0

    if ( Random ( 1, 2 ) = 1 )
        b = -b
    endif

    if ( Random ( 1, 3 ) = 1 )
        c = b
    endif

    // add force near the end of its life
    AddParticlesForce ( explode, 0.6, 1.0, c, a )

    // random kind of explosion
    if ( Random ( 1, 3 ) = 1 )
        ClearParticlesColors ( explode )
        ClearParticlesForces ( explode )

        col = Random ( 1, 3 )

        if ( col = 1 )
            AddParticlesColorKeyFrame ( explode, 0.0, 255,   0, 0, 255 )
            AddParticlesColorKeyFrame ( explode, 0.4, 255, 100, 0, 255 )
            AddParticlesColorKeyFrame ( explode, 0.8, 255, 200, 0, 255 )
            AddParticlesColorKeyFrame ( explode, 1.0, 255, 255, 0,   0 )
        elseif ( col = 2 )
            AddParticlesColorKeyFrame ( explode, 0.0, 0,   0, 255, 255 )
            AddParticlesColorKeyFrame ( explode, 0.4, 0, 100, 255, 255 )
            AddParticlesColorKeyFrame ( explode, 0.8, 0, 200, 255, 255 )
            AddParticlesColorKeyFrame ( explode, 1.0, 0, 255, 255,   0 )
        else
            AddParticlesColorKeyFrame ( explode, 0.0,   0, 0, 255, 255 )
            AddParticlesColorKeyFrame ( explode, 0.4, 100, 0, 255, 255 )
            AddParticlesColorKeyFrame ( explode, 0.8, 200, 0, 255, 255 )
            AddParticlesColorKeyFrame ( explode, 1.0, 255, 0, 255,   0 )
        endif

        SetParticlesDirection ( explode, 40.0, 40.0 )

        up = Random ( 200, 1000 )

        AddParticlesForce ( explode, 0.2, 0.4, 0.0, -up )
        AddParticlesForce ( explode, 0.4, 1.0, 0.0,  up )
    endif

    SetParticlesMax ( explode, 100 )
endfunction


function CheckBulletsWithAliens ( )
    // run through all bullets and see if they collide with an alien
	for i = 1 to 10
		if g_Bullets [ i ].iActive = 1
			for j = 1 to 10
				if ( GetSpriteColorAlpha ( g_Bullets [ i ].iID ) > 16 )
					if ( GetSpriteCollision ( g_Bullets [ i ].iID, g_Aliens [ j ].iSprite ) )
						g_iScoreTarget = g_iScoreTarget + 15

						x# = GetSpriteX ( g_Aliens [ j ].iSprite )
						y# = GetSpriteY ( g_Aliens [ j ].iSprite )
						x1# = WorldToScreenX ( x# )
						y1# = WorldToScreenY ( y# )

						if ( x1# >= 20 && x1# <= 500 )
							ExplodeAlien ( x#, y# )

							SetSpritePosition    ( g_Aliens [ j ].iSprite, -300, -300 )
							SetParticlesPosition ( g_Aliens [ j ].iEngine, -300, -300 )

							PlaySound ( Random ( 2, 4 ) )

							g_Bullets [ i ].iActive = 0
							SetSpriteColorAlpha ( g_Bullets [ i ].iID, 0 )
							SetSpritePosition ( g_Bullets [ i ].iID, -100.0, -100.0 )
							g_Bullets [ i ].fLife = 0.0
						endif
					endif
				endif
			next j
		endif
	next i
endfunction

