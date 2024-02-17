
function CreateTextEntities ( )
    font = LoadImage ( "space/ascii.png" )

    CreateText ( 10, "SCORE: 0" )
	SetTextPosition ( 10, 0, 0 )
	SetTextSize ( 10, 20 )
	SetTextFontImage ( 10, font )
	SetTextColorAlpha ( 10, 0 )
	SetTextVisible ( 10, 0 )
	FixTextToScreen ( 10, 1 )

	CreateText ( 2, "LIVES: 3" )
	SetTextPosition ( 2, 300, 0 )
	SetTextSize ( 2, 20 )
	SetTextFontImage ( 2, font )
	SetTextColorAlpha ( 2, 0 )
	SetTextVisible ( 2, 0 )
	FixTextToScreen ( 2, 1 )

	CreateText ( 3, "START GAME" )
	SetTextPosition ( 3, 100, 160 )
	SetTextSize ( 3, 40 )
	SetTextFontImage ( 3, font )
	FixTextToScreen ( 3, 1 )
	SetTextColorAlpha ( 3, 0 )
	SetTextVisible ( 3, 0 )
endfunction

function UpdateText ( )
   if ( g_iScore < g_iScoreTarget )
		inc g_iScore
    endif

	line$ = "SCORE: " + str ( g_iScore )
	SetTextString ( 10, line$ )

	line$ = "LIVES: " + str ( g_iLives )
    SetTextString ( 2, line$ )
endfunction

function CreateMusic( )
    LoadMusicOGG ( 1, "space/track1.ogg" )
	SetMusicVolumeOGG ( 1,70 )
	PlayMusicOGG ( 1, 1 )

	LoadSound ( 1, "space/laser.wav" )

	LoadSound ( 2, "space/explode1.wav" )
	LoadSound ( 3, "space/explode2.wav" )
	LoadSound ( 4, "space/explode3.wav" )

	LoadSound ( 5, "space/playerhit.wav" )
endfunction
