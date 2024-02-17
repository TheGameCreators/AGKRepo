
// Project: GameCenter 
// Created: 2015-08-12

// set window properties
SetWindowTitle( "GameCenter" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )

GameCenterSetup() 
GameCenterLogin() 

do
    if ( GetPointerReleased() )
        GameCenterAchievementsShow()
        //GameCenterShowLeaderBoard("CgkIz4OlxJoaEAIQBg")
    endif
    
    if ( name$ = "" and GetGameCenterLoggedIn() = 1 )
		name$ = GetGameCenterPlayerID()
	endif
	
	Print( "Player ID: " + name$ )
    Print("Logged In: " + Str(GetGameCenterLoggedIn()) )
    print(screenfps())
    Sync()
loop
