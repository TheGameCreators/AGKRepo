

function MainMenu_Begin ( )

	`SetOrientationAllowed ( 0, 0, 1, 1 )
	SetVirtualResolution  ( 480, 320 )

	g_iBackground   = CreateSprite ( LoadImage ( "main_menu.jpg"    ) )
	g_iInstructions = CreateSprite ( LoadImage ( "instructions.jpg" ) )

	SetSpriteVisible ( g_iInstructions, 0 )

	LoadMusicOGG ( 1, "music.ogg" )
	PlayMusicOGG ( 1 )

	g_iSounds [ 1 ] = LoadSound ( "button.wav" )
	g_iSounds [ 2 ] = LoadSound ( "escape.wav" )
	g_iSounds [ 3 ] = LoadSound ( "welcome.wav" )
	g_iSounds [ 4 ] = LoadSound ( "hit1.wav" )
	g_iSounds [ 5 ] = LoadSound ( "hit2.wav" )
	g_iSounds [ 6 ] = LoadSound ( "hit3.wav" )
	g_iSounds [ 7 ] = LoadSound ( "hit4.wav" )
	g_iSounds [ 8 ] = LoadSound ( "gong.wav" )

	Game_Begin ( )



	SetSpriteDepth ( g_iBackground, 0 )
	SetSpriteDepth ( g_iInstructions, 0 )

	g_iState = 0
	g_iTap   = 0
	g_iTapX  = 0
	g_iTapY  = 0

	g_iCountDownNumber = 5
	g_iCountDownAlpha = 0
endfunction



function MainMenu_Loop ( )
	CheckInput ( )

	select ( g_iState )

		case 0:

			if g_iTap = 1

				// start game
				if ( g_iTapX > 160 and g_iTapX < 310 and g_iTapY > 105 and g_iTapY < 200 )
					PlaySound ( g_iSounds [ 1 ] )
					g_iMessageState = 0
					g_iGameState = 0
					g_iState = 5
				endif

				// show instructions
				if ( g_iTapX > 160 and g_iTapX < 310 and g_iTapY > 210 and g_iTapY < 260 )
					PlaySound ( g_iSounds [ 1 ] )
					SetSpriteVisible ( g_iInstructions, 1 )
					SetSpriteColor ( g_iInstructions, 255, 255, 255, 0 )
					g_iState = 2
				endif

				g_iTap = 0

			endif
		endcase

		case 2:
			MainMenu_Transition( g_iBackground, g_iInstructions, 8 )

			if ( g_iReturn = 1 )
				g_iReturn = 0
				g_iState = 3
			endif
		endcase

		case 3:
			if g_iTap = 1
				PlaySound ( g_iSounds [ 1 ] )

				g_iState = 4
				g_iTap = 0
			endif
		endcase

		case 4:
			MainMenu_Transition( g_iInstructions, g_iBackground, 8 )

			if ( g_iReturn = 1 )
				g_iReturn = 0
				g_iState = 0
			endif
		endcase

		case 5:
			Game_Loop ( )

			if ( g_iGameOver = 1 )
				Game_End ( )
				SetSpriteColor ( g_iBackground, 255, 255, 255, 255 )
				g_iState = 0
			endif

			iAlpha = GetSpriteColorAlpha ( g_iBackground )

			if ( iAlpha >= 8 )
				iAlpha = iAlpha - 8
			else
				iAlpha = 0
			endif

			SetSpriteColor ( g_iBackground, 255, 255, 255, iAlpha )
		endcase

	endselect
endfunction

function CheckInput ( )
	if ( GetPointerPressed ( ) )
		g_iTap  = 1
		g_iTapX = GetPointerX ( )
		g_iTapY = GetPointerY ( )
	endif
endfunction

function MainMenu_Transition( iA, iB, iSpeed )

	iAlphaA = GetSpriteColorAlpha ( iA )
	iAlphaB = GetSpriteColorAlpha ( iB )
	g_iReturn = 0

	if ( iAlphaB < 255 - iSpeed )
		iAlphaB = iAlphaB + iSpeed
		iAlphaA = iAlphaA - iSpeed
	else
		iAlphaB = 255
		iAlphaA = 0
		g_iReturn = 1
	endif

	SetSpriteColor ( iA, 255, 255, 255, iAlphaA )
	SetSpriteColor ( iB, 255, 255, 255, iAlphaB )
endfunction

