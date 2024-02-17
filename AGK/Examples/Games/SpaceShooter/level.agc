

function CreateBackground( )
    LoadImage ( 1, "space/space.jpg" )
	CreateSprite ( 1, 1 )
	FixSpriteToScreen ( 1, 1 )
endfunction

function CreateStarField( )
    LoadImage ( 4, "space/particle.png" )

    CreateParticles           ( 1, 550.0, 0.0 )
	SetParticlesImage         ( 1, 4 )
	SetParticlesStartZone     ( 1, 0, 10, 0, 400 )
	SetParticlesDirection     ( 1, -50.0, 0.0 )
	SetParticlesLife          ( 1, 15 )
	SetParticlesSize          ( 1, 4 )
	SetParticlesAngle         ( 1, 0 )
	SetParticlesFrequency     ( 1, 60 )
	SetParticlesVelocityRange ( 1, 0.5, 4.0 )
	FixParticlesToScreen      ( 1, 1 )
endfunction

function CreateLevel( )
    j = LoadImage ( "space/MetalBlock1.png" )

	// top random
	CreateStrip ( j, 32.0, 0.0, 0 )

	// bottom random
	CreateStrip ( j, 25.0, 0.0, 1 )

	// top and bottom straight
	CreateStrip ( j, 0.0, 0.0, 2 )
endfunction

function CreateStrip ( iImage as integer, fX as float, fY as float, iMode as integer )
    for i = 0 to 250
        built = 0
        iSprite = -1
        iSpriteB = -1

        if iMode = 0
            if Random ( 1, 2 ) = 1
                iSprite = CreateSprite ( iImage )
                SetSpritePosition ( iSprite, fX, ( 368 - Random ( 64, 64 + 32 ) ) )
                //SetSpriteUVBorder ( iSprite, 0 )
                built = 1
            endif

            if fX > 300 and Random ( 1, 5 ) = 3
                iSpriteB = CreateSprite ( iImage )
                SetSpritePosition ( iSpriteB, fX, Random ( 120, 200 ) )
                SetSpriteColor ( iSpriteB, 255, 255, 255, 200 )
                SetSpriteAngle ( iSpriteB, Random ( 0, 360 ) )
                //SetSpriteUVBorder ( iSpriteB, 0 )
                built = 1
            endif
        endif

        if iMode = 1
			if Random ( 1, 3 ) = 1
				iSprite = CreateSprite ( iImage )
				SetSpritePosition ( iSprite, fX, Random ( 32, 64 ) )
				//SetSpriteUVBorder ( iSprite, 0 )
				built = 1
			endif
		endif

		if iMode = 2
			iSprite = CreateSprite ( iImage )
			SetSpritePosition ( iSprite, fX, 0.0 )

			iSpriteB = CreateSprite ( iImage )
			SetSpritePosition ( iSpriteB, fX, 400 - 64 )

			//SetSpriteUVBorder ( iSpriteB, 0 )
			built = 1
		endif

        if built = 1

            if iSprite <> -1
                g_iScenery [ g_iSceneryCount ] = iSprite
				inc g_iSceneryCount
            endif

            if iSpriteB <> -1
                g_iScenery [ g_iSceneryCount ] = iSpriteB
				inc g_iSceneryCount
            endif
        endif

        fX = fX + 64
    next i
endfunction
