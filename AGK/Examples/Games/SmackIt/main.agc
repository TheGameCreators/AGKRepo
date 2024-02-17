//Smack It Game Demo!

UseNewDefaultFonts( 1 )

type sItem
    sprite as integer
    state as integer
    direction as integer
endtype

type sEnemy
    sprite as integer
    state as integer
    delay as integer
    time as integer
    bottomY as integer
    topY as integer
    topDelay as integer
    speedUp as integer
    speedDown as integer
    waitAtTop as integer
    temp as integer
endtype

type sLevel
        background as integer
        foreground as integer
        pause as integer
        play as integer
        pauseButton as integer

        overlay1 as integer
        overlay2 as integer
        overlay3 as integer

        TimerIncrement as integer
        Timer as integer
        Limit as integer
        TimeLimit as integer
        EnemiesUp as integer
        lastTime as integer

        miss as integer

        scoreText as integer
        timerText as integer
        miscText as integer
        continueText as integer
        mainMenuText as integer

        score as integer

        targetHit as integer
        targetAccuracy as integer

        hitAttempts as integer
        enemiesMissed as integer
        missTimer as integer
        enemiesHit as integer
endtype

global g_Level as sLevel

global g_Item as sItem
global dim g_Clouds [ 3 ]
global dim g_MoleImages [ 11 ]
global dim g_Text [ 5 ]
global g_LastPopup = 0
global g_Overlay = 0
global dim g_Enemies [ 12 ] as sEnemy
global dim g_StatsText [ 12 ] as integer
global eDisplayFadeIn = 0
global eDisplayMainMenu = 1
global eDisplayStartGame = 2
global eLoadLevel = 3
global ePlayLevel = 4
global ePause = 5
global eLevelWon = 6
global eLevelLost = 7
global g_State = 0
global g_PreviousState = 0
global g_PauseState = 0
global g_GameOverState = 0
global g_Alarm = 0

SetupMainMenu ( )

do
    select g_State:
        case 0:
                        DisplayFadeIn ( )
        endcase

        case 1:
                DisplayMainContents ( )
                endcase

        case 2:
                        DisplayStartGame ( )
                endcase

                case 3:
                        LoadLevel ( 0 )
                endcase

                case 4:
                        PlayLevel ( )
                endcase

                case 5:
                        HandlePause ( )
                endcase

                case 6:
                        HandleWin ( )
                endcase

                case 7:
                        HandleLose ( )
                endcase
    endselect

    Sync ( )
loop

function SetupMainMenu ( )
        SetVirtualResolution ( 320, 480 )

    // use a new font for the default text
        defaultfontimage = LoadImage ( "ascii.png" )
        SetTextDefaultFontImage ( defaultfontimage )

    // background image
    background = CreateSprite ( LoadImage ( "main_menu/background.png" ) )
    SetSpritePosition ( background, 0, 0 )

    // foreground image
    foreground = CreateSprite ( LoadImage ( "main_menu/foreground.png" ) )
    SetSpritePosition ( foreground, 0, 370 )

    // smack it logo
    logo = CreateSprite ( LoadImage ( "main_menu/logo.png" ) )
    SetSpritePosition ( logo, 69, 0 )

    // balloon moving across the screen
    g_Item.sprite = CreateSprite ( 0 )
    g_Item.state = 0

    for i = 1 to 5
        AddSpriteAnimationFrame ( g_Item.sprite, LoadImage ( "items/item" + str ( i - 1 ) + ".png" ) )
    next i

    for i = 1 to 6
        AddSpriteAnimationFrame ( g_Item.sprite, LoadImage ( "items/smoke" + str ( i - 1 ) + ".png" ) )
    next i

    SetSpritePosition ( g_Item.sprite, -150, 30 )

        g_MoleImages [  1 ] = LoadImage ( "mole/looking0.png" )
        g_MoleImages [  2 ] = LoadImage ( "mole/looking1.png" )
        g_MoleImages [  3 ] = LoadImage ( "mole/looking2.png" )
        g_MoleImages [  4 ] = LoadImage ( "mole/looking3.png" )
        g_MoleImages [  5 ] = LoadImage ( "mole/wave0.png" )
        g_MoleImages [  6 ] = LoadImage ( "mole/wave1.png" )
        g_MoleImages [  7 ] = LoadImage ( "mole/wave2.png" )
        g_MoleImages [  8 ] = LoadImage ( "mole/wave3.png" )
        g_MoleImages [  9 ] = LoadImage ( "mole/hit0.png" )
        g_MoleImages [ 10 ] = LoadImage ( "mole/hit1.png" )
        g_MoleImages [ 11 ] = LoadImage ( "mole/hit2.png" )

    for i = 10 to 12
        g_Enemies [ i ].sprite    = CreateSprite ( 0 )
        g_Enemies [ i ].state     = 0
        g_Enemies [ i ].delay     = 0
        g_Enemies [ i ].time      = 0
        g_Enemies [ i ].bottomY   = 431
        g_Enemies [ i ].topY      = 386
        g_Enemies [ i ].speedUp   = 1
        g_Enemies [ i ].speedDown = 1
        g_Enemies [ i ].topDelay  = 0
        g_Enemies [ i ].waitAtTop = 100
        g_Enemies [ i ].temp      = 0

        if i = 10 then SetSpritePosition ( g_Enemies [ i ].sprite,  -2, 431 )
        if i = 11 then SetSpritePosition ( g_Enemies [ i ].sprite, 118, 431 )
        if i = 12 then SetSpritePosition ( g_Enemies [ i ].sprite, 238, 431 )

        for j = 1 to 11
            AddSpriteAnimationFrame ( g_Enemies [ i ].sprite, g_MoleImages [ j ] )
        next j

        PlaySprite ( g_Enemies [ i ].sprite, 12, 1, 1, 4 )
        SetSpriteColorAlpha ( g_Enemies [ i ].sprite, 0 )
    next i

    // overlay at bottom of the screen
    overlay = CreateSprite ( LoadImage ( "main_menu/overlay.png" ) )
    SetSpritePosition ( overlay, 0, 450 )

    // clouds
    g_Clouds [ 1 ] = CreateSprite ( LoadImage ( "main_menu/cloud1.png" ) )
    g_Clouds [ 2 ] = CreateSprite ( LoadImage ( "main_menu/cloud2.png" ) )
    g_Clouds [ 3 ] = CreateSprite ( LoadImage ( "main_menu/cloud3.png" ) )

    SetSpritePosition ( g_Clouds [ 1 ], 350, 330 )
    SetSpritePosition ( g_Clouds [ 2 ], 500, 200 )
    SetSpritePosition ( g_Clouds [ 3 ], 400, 130 )

    // text
    g_Text [ 1 ] = CreateText ( "SMACK IT" )
        g_Text [ 2 ] = CreateText ( "START" )
        g_Text [ 3 ] = CreateText ( "" )
        g_Text [ 4 ] = CreateText ( "" )
        g_Text [ 5 ] = CreateText ( "" )

        for i = 1 to 5
                SetTextSize ( g_Text [ i ], 56.0 )
        next i

        space = 50

        SetTextPosition ( g_Text [ 1 ], 15.0, -230.0 )
        SetTextPosition ( g_Text [ 2 ], 90.0, 200.0 )
        SetTextPosition ( g_Text [ 3 ], 70.0, 160.0 + ( space * 1 ) )
        SetTextPosition ( g_Text [ 4 ], 30.0, 160.0 + ( space * 2 ) )
        SetTextPosition ( g_Text [ 5 ], 30.0, 160.0 + ( space * 3 ) )

        g_Overlay = CreateSprite ( LoadImage ( "black.jpg" ) )
        SetSpritePosition ( g_Overlay, 0.0, 0.0 )
        SetSpriteSize ( g_Overlay, 320, 480 )

        LoadMusicOGG ( 1, "rag.ogg" )
        PlayMusicOGG ( 1, 1 )

        LoadSound ( 1, "hit1.wav" )
        LoadSound ( 2, "hit2.wav" )
        LoadSound ( 3, "hit3.wav" )
        LoadSound ( 4, "hit4.wav" )
        LoadSound ( 5, "hit5.wav" )

        LoadSound ( 6, "hitPlane.wav" )
        LoadSound ( 7, "miss.wav" )
        LoadSound ( 8, "warning.wav" )
endfunction

function UpdateItems ( )
    // update the item scrolling across the screen (balloon for main menu)

    // idle state
    if g_Item.state = 0

        // get ready to launch the balloon
        if Random ( 0, 50 ) = 25

            // move right or left
            if Random ( 0, 2 ) = 0
                SetSpritePosition ( g_Item.sprite, -150, 15 )
                SetSpriteFlip ( g_Item.sprite, 0, 0 )
                g_Item.direction = 0
            else
                SetSpritePosition ( g_Item.sprite, 360, 15 )
                SetSpriteFlip ( g_Item.sprite, 1, 0 )
                g_Item.direction = 1
            endif

            PlaySprite ( g_Item.sprite, 12, 1, 1, 5 )

            g_Item.state = 1
        endif
    endif

    // flying state
    if g_Item.state = 1

        x# = GetSpriteX ( g_Item.sprite )

        if g_Item.direction = 0
            x# = x# + 2
        else
            x# = x# - 2
        endif

        SetSpriteX ( g_Item.sprite, x# )

        // check for tap at this point
        if GetPointerPressed ( ) = 1

            if GetSpriteHitTest ( g_Item.sprite, GetPointerX ( ), GetPointerY ( ) ) = 1

                                PlaySound ( 6 )

                PlaySprite ( g_Item.sprite, 5, 0, 6, 11 )
                g_Item.state = 2

                if g_State = ePlayLevel
                                        g_Level.score = g_Level.Score + 25
                                endif

            endif

        endif

        // check for moving off screen
        if g_Item.direction = 0 and x# >  420 then g_Item.state = 0
        if g_Item.direction = 1 and x# < -150 then g_Item.state = 0
    endif

    if g_Item.state = 2
        SetSpriteColorAlpha ( g_Item.sprite, GetSpriteColorAlpha ( g_Item.sprite ) - 2 )

        if GetSpriteCurrentFrame ( g_Item.sprite ) = 10
            g_Item.state = 3
        endif
    endif

    if g_Item.state = 3
        SetSpriteColorAlpha ( g_Item.sprite, 255 )
        SetSpritePosition ( g_Item.sprite, -150, 30 )

        g_Item.state = 0
    endif
endfunction

function UpdateClouds ( )
        SetSpriteX ( g_Clouds [ 1 ], GetSpriteX ( g_Clouds [ 1 ] ) - 2.0 )
        SetSpriteX ( g_Clouds [ 2 ], GetSpriteX ( g_Clouds [ 2 ] ) - 1.0 )
        SetSpriteX ( g_Clouds [ 3 ], GetSpriteX ( g_Clouds [ 3 ] ) - 3.0 )

        if ( GetSpriteX ( g_Clouds [ 1 ] ) < -100.0 ) then SetSpriteX ( g_Clouds [ 1 ], 350.0 + Random ( 0, 200 ) )
        if ( GetSpriteX ( g_Clouds [ 2 ] ) < -100.0 ) then SetSpriteX ( g_Clouds [ 2 ], 500.0 + Random ( 0, 200 ) )
        if ( GetSpriteX ( g_Clouds [ 3 ] ) < -100.0 ) then SetSpriteX ( g_Clouds [ 3 ], 400.0 + Random ( 0, 200 ) )
endfunction

function UpdateEnemies ( )
    count = 0

        for i = 10 to 12
                if ( g_Enemies [ i ].state = 0 )
                        count = count + 1
                endif
        next i

        if ( count = 3 and Random ( 0, 20 ) = 10 )
                i = Random ( 10, 12 )

        if ( i <> g_LastPopup )
                        g_Enemies [ i ].topDelay = 0
                        g_Enemies [ i ].state = 1
                        g_LastPopup = i
                endif
        endif

        for i = 10 to 12
        if g_Enemies [ i ].state = 1 then HandleEnemyUp   ( i )
        if g_Enemies [ i ].state = 2 then HandleEnemyTop  ( i )
        if g_Enemies [ i ].state = 3 then HandleEnemyDown ( i )
        if g_Enemies [ i ].state = 4 then HandleEnemyHit  ( i )
    next i

    if GetPointerPressed ( ) = 1
        for i = 10 to 12
           if ( g_Enemies [ i ].state = 1 or g_Enemies [ i ].state = 2 or g_Enemies [ i ].state = 3 )

                if GetSpriteColorAlpha ( g_Enemies [ i ].sprite ) < 20
                    exit
                endif

                if ( GetSpriteHitTest ( g_Enemies [ i ].sprite, GetPointerX ( ), GetPointerY ( ) ) = 1 )

                                        PlaySound ( Random ( 1, 5 ) )

                    StopSprite ( g_Enemies [ i ].sprite )
                    g_Enemies [ i ].temp = 0
                    g_Enemies [ i ].state = 4
                endif
           endif
        next i
    endif
endfunction

function HandleEnemyUp ( id as integer )
    sprite = g_Enemies [ id ].sprite

        alpha = GetSpriteColorAlpha ( sprite )

        if ( alpha < 255 - 48 )
                alpha = alpha + 48
        endif

        x# = GetSpriteX ( sprite )
        y# = GetSpriteY ( sprite )

        if ( GetSpritePlaying ( sprite ) = 0 )
                PlaySprite ( sprite, 5, 1, 1, 4 )
        endif

        SetSpriteColorAlpha ( sprite, alpha )

    // see if we need to continue moving the sprite up
        if ( y# > g_Enemies [ id ].topY + g_Enemies [ id ].speedUp )
                SetSpritePosition ( sprite, x#, y# - g_Enemies [ id ].speedUp )
        else
                // we are the top, move onto the next phase
                SetSpriteColorAlpha ( sprite, 255 )

        // stop animation
                StopSprite ( sprite )

        g_Enemies [ id ].state = 2
                g_Enemies [ id ].time  = 0
        endif
endfunction

function HandleEnemyTop ( id as integer )
    sprite = g_Enemies [ id ].sprite

        if ( GetSpritePlaying ( sprite ) = 0 )
                PlaySprite ( sprite, 5, 1, 5, 7 )
        endif

    g_Enemies [ id ].topDelay = g_Enemies [ id ].topDelay + 1

        if ( g_Enemies [ id ].topDelay > g_Enemies [ id ].waitAtTop )
                StopSprite ( sprite )

                g_Enemies [ id ].state = 3
                g_Enemies [ id ].time  = 0
        endif

endfunction

function HandleEnemyDown ( id as integer )
    sprite = g_Enemies [ id ].sprite

        alpha = GetSpriteColorAlpha ( sprite )

        x# = GetSpriteX ( sprite )
        y# = GetSpriteY ( sprite )

        if ( GetSpritePlaying ( sprite ) = 0 )
                PlaySprite ( sprite, 5, 1, 1, 4 )
        endif

        if ( y# < g_Enemies [ id ].bottomY - g_Enemies [ id ].speedDown )
                SetSpritePosition ( sprite, x#, y# + g_Enemies [ id ].speedDown )
        else
                if ( alpha > 48 )
                        alpha = alpha - 48
                        SetSpriteColorAlpha ( sprite, alpha )
                else
                        SetSpriteColorAlpha ( sprite, 0 )

                        g_Enemies [ id ].state = 0
        endif
    endif
endfunction

function HandleEnemyHit ( id as integer )
    sprite = g_Enemies [ id ].sprite
    y# = GetSpriteY ( sprite )

    alpha = GetSpriteColorAlpha ( sprite )

    SetSpriteColor ( sprite, 200, 100, 100, alpha )

        if ( GetSpritePlaying ( sprite ) = 0 )
                PlaySprite ( sprite, 5, 1, 8, 10 )
    endif

    if ( y# < g_Enemies [ id ].bottomY - 20.0 )
                SetSpriteY ( sprite, y# + 6.0 )
        endif

        if ( GetSpriteCurrentFrame ( sprite ) = 10 )
                inc g_Enemies [ id ].temp
    endif

        if ( g_Enemies [ id ].temp > 24 )
                if ( alpha > 24 )
                        alpha = alpha - 24
                        SetSpriteColor ( sprite, 200, 100, 100, alpha )
        else
            g_Enemies [ id ].temp = 0
            g_Enemies [ id ].state = 0
            g_Enemies [ id ].time = 0
            g_Enemies [ id ].delay = 12

                        StopSprite ( sprite )

                        SetSpriteColor ( sprite, 255, 255, 255, 0 )

                        SetSpriteY ( sprite, g_Enemies [ id ].bottomY )
                endif
        endif
endfunction

function DisplayMainContents ( )
    if GetPointerPressed ( ) = 1
        x# = GetPointerX ( )
        y# = GetPointerY ( )

        if x# >= 80 and x# < 220 and y# > 190 and y# < 240
            g_State = eDisplayStartGame
        endif
    endif

    UpdateItems   ( )
        UpdateClouds  ( )
        UpdateEnemies ( )
endfunction

function DisplayFadeIn ( )
        // fade in
        alpha = GetSpriteColorAlpha ( g_Overlay )

        if alpha > 4
                alpha = alpha - 4
                SetSpriteColor ( g_Overlay, 255, 255, 255, alpha )
        else
                g_State = eDisplayMainMenu
        endif

        UpdateItems   ( )
        UpdateClouds  ( )
        UpdateEnemies ( )
endfunction

function DisplayStartGame ( )
        // fade in
        alpha = GetSpriteColorAlpha ( g_Overlay )

        if alpha < 255 - 4
                alpha = alpha + 4
                SetSpriteColor ( g_Overlay, 255, 255, 255, alpha )
        else
                for i = 1 to 5
                        SetTextVisible ( g_Text [ i ], 0 )
                next i

                g_State = eLoadLevel
        endif

        UpdateItems   ( )
        UpdateClouds  ( )
        UpdateEnemies ( )
endfunction

function LoadLevel ( level as integer )
        // load background
        g_Level.background = CreateSprite ( LoadImage ( "mole/background.png" ) )

        // load foreground
        g_Level.foreground = CreateSprite ( LoadImage ( "mole/foreground.png" ) )
        SetSpritePosition ( g_Level.foreground, 0.0, 80.0 )

        // pause and play
        g_Level.pause = LoadImage ( "iconpause.png" )
        g_Level.play  = LoadImage ( "iconplay.png" )

        g_Level.pauseButton = CreateSprite ( g_Level.pause )
        SetSpritePosition ( g_Level.pauseButton, 5.0, 5.0 )

        // enemies
        dim x [ 10 ] as float
        dim y [ 10 ] as float

        x [ 1 ] = 1.0
        x [ 2 ] = 121.0
        x [ 3 ] = 241.0
        x [ 4 ] = 1.0
        x [ 5 ] = 121.0
        x [ 6 ] = 241.0
        x [ 7 ] = 1.0
        x [ 8 ] = 121.0
        x [ 9 ] = 241.0

        y [ 1 ] = 231.0
        y [ 2 ] = 231.0
        y [ 3 ] = 231.0
        y [ 4 ] = 331.0
        y [ 5 ] = 331.0
        y [ 6 ] = 331.0
        y [ 7 ] = 431.0
        y [ 8 ] = 431.0
        y [ 9 ] = 431.0

        enemy = 1

        for j = 1 to 3
                for i = 1 to 3
                        g_Enemies [ enemy ].sprite    = CreateSprite ( 0 )
                        g_Enemies [ enemy ].state     = 0
                        g_Enemies [ enemy ].delay     = 0
                        g_Enemies [ enemy ].time      = 0
                        g_Enemies [ enemy ].bottomY   = y [ enemy ]
                        g_Enemies [ enemy ].topY      = y [ enemy ] - 45
                        g_Enemies [ enemy ].speedUp   = 1
                        g_Enemies [ enemy ].speedDown = 1
                        g_Enemies [ enemy ].topDelay  = 0
                        g_Enemies [ enemy ].waitAtTop = 100
                        g_Enemies [ enemy ].temp      = 0

                        SetSpritePosition ( g_Enemies [ enemy ].sprite, x [ enemy ], y [ enemy ] )
                        SetSpriteColorAlpha ( g_Enemies [ enemy ].sprite, 0 )

                        for k = 1 to 11
                                AddSpriteAnimationFrame ( g_Enemies [ enemy ].sprite, g_MoleImages [ k ] )
                        next k

                        if ( enemy = 1 or enemy = 2 or enemy = 3 ) then SetSpriteDepth ( g_Enemies [ enemy ].sprite, 7 )
                        if ( enemy = 4 or enemy = 5 or enemy = 6 ) then SetSpriteDepth ( g_Enemies [ enemy ].sprite, 5 )
                        if ( enemy = 7 or enemy = 8 or enemy = 9 ) then SetSpriteDepth ( g_Enemies [ enemy ].sprite, 3 )

                        enemy = enemy + 1
                next i
    next j

        // load overlays
        g_Level.overlay1 = CreateSprite ( LoadImage ( "mole/overlay1.png" ) )
        SetSpritePosition ( g_Level.overlay1, 0.0, 250.0 )
        SetSpriteDepth    ( g_Level.overlay1, 6 )

        g_Level.overlay2 = CreateSprite ( LoadImage ( "mole/overlay2.png" ) )
        SetSpritePosition ( g_Level.overlay2, 0.0, 350.0 )
        SetSpriteDepth    ( g_Level.overlay2, 4 )

        g_Level.overlay3 = CreateSprite ( LoadImage ( "mole/overlay3.png" ) )
        SetSpritePosition ( g_Level.overlay3, 0.0, 450.0 )
        SetSpriteDepth    ( g_Level.overlay3, 2 )

        // move item to front of screen
        g_Item.state = 0
        SetSpritePosition ( g_Item.sprite, -150, 15 )
        SetSpriteDepth ( g_Item.sprite, 0 )

        // miss icon
        g_Level.miss = CreateSprite ( LoadImage ( "miss.png" ) )
        SetSpritePosition ( g_Level.miss, -200.0, -200.0 )
        SetSpriteDepth ( g_Level.miss, 1 )

        // score text
        g_Level.scoreText = CreateText ( "0" )
        SetTextSize     ( g_Level.scoreText, 24.0 )
        SetTextPosition ( g_Level.scoreText, 188.0, 10.0 )

        // timer text
        g_Level.timerText = CreateText ( "60" )
        SetTextSize     ( g_Level.timerText, 24.0 )
        SetTextPosition ( g_Level.timerText, 85.0, 10.0 )

        // misc text
        g_Level.miscText = CreateText ( "" )
        SetTextSize     ( g_Level.miscText, 24.0 )
        SetTextPosition ( g_Level.miscText, -200.0, -200.0 )

        // continue text
        g_Level.continueText = CreateText ( "CONTINUE" )
        SetTextSize     ( g_Level.continueText, 56.0 )
        SetTextPosition ( g_Level.continueText, -200.0, -200.0 )
        SetTextDepth    ( g_Level.continueText, 1 )
        SetTextColor    ( g_Level.continueText, 255, 255, 255, 0 )

        // main menu text
        g_Level.mainMenuText = CreateText ( "MAIN MENU" )
        SetTextSize     ( g_Level.mainMenuText, 56.0 )
        SetTextPosition ( g_Level.mainMenuText, -200.0, -200.0 )
        SetTextDepth    ( g_Level.mainMenuText, 1 )
        SetTextColor    ( g_Level.mainMenuText, 255, 255, 255, 0 )

        g_Level.TimeLimit = 60
        g_Level.Limit     = 40
        g_Level.Timer     = ( g_Level.Limit / 100 ) * 95
        g_LastPopup      = -1

        g_Level.targetHit      = 40
        g_Level.targetAccuracy = 40

        g_State = ePlayLevel
endfunction

function UnloadLevel ( )
        DeleteSprite ( g_Level.background )
        DeleteSprite ( g_Level.foreground )
        DeleteSprite ( g_Level.pauseButton )

        for i = 1 to 9
                DeleteSprite ( g_Enemies [ i ].sprite )
        next i

        DeleteSprite ( g_Level.overlay1 )
        DeleteSprite ( g_Level.overlay2 )
        DeleteSprite ( g_Level.overlay3 )

        DeleteSprite ( g_Level.miss )

        DeleteText ( g_Level.scoreText )
        DeleteText ( g_Level.timerText )
        DeleteText ( g_Level.miscText )
        DeleteText ( g_Level.continueText )
        DeleteText ( g_Level.mainMenuText )

        for i = 1 to 12
                DeleteText ( g_StatsText [ i ] )
        next i
endfunction

function SetEnemyProperties ( limit as integer, index as integer, speed as integer, waitAtTop as integer, delay as integer )

        g_Level.limit = limit

        g_Enemies [ index ].speedUp = speed
        g_Enemies [ index ].speedDown = speed
        g_Enemies [ index ].waitAtTop = waitAtTop
        g_Enemies [ index ].delay = delay
endfunction

function UpdateEnemiesInGame ( )

        inc g_Level.Timer

        if ( g_Level.Timer > g_Level.Limit and g_Level.TimeLimit > 2 )

                // set up a random list
                dim pick [ 9 ] as integer

                start = Random ( 1, 9 )

                for i = 1 to 9
                        pick [ i ] = start

                        inc start

                        if start > 9
                                start = 1
                        endif
                next i

                for i = 1 to 9
                        if ( g_Enemies [ pick [ i ] ].state = 0 and pick [ i ] <> g_LastPopup )

                                index = pick [ i ]

                                g_Enemies [ index ].state    = 1
                                g_Enemies [ index ].topDelay = 0

                                SetSpriteColor ( g_Enemies [ index ].sprite, 255, 255, 255, 0 )

                                if ( g_Level.TimerIncrement <=  10                                   ) then SetEnemyProperties ( 40, index, 1, 100, 8 )
                                if ( g_Level.TimerIncrement >=  20 and g_Level.TimerIncrement <=  40 ) then SetEnemyProperties ( 25, index, Random ( 1, 1 ), Random ( 50, 100 ), 8 )
                                if ( g_Level.TimerIncrement >=  40 and g_Level.TimerIncrement <=  60 ) then SetEnemyProperties ( 20, index, Random ( 2, 4 ), Random ( 40, 90 ), 8 )
                                if ( g_Level.TimerIncrement >=  60 and g_Level.TimerIncrement <=  80 ) then SetEnemyProperties ( 15, index, Random ( 2, 5 ), Random ( 20, 70 ), 6 )
                                if ( g_Level.TimerIncrement >=  80 and g_Level.TimerIncrement <= 100 ) then SetEnemyProperties ( 10, index, Random ( 2, 5 ), Random ( 20, 70 ), 6 )
                                if ( g_Level.TimerIncrement >= 100 and g_Level.TimerIncrement <= 120 ) then SetEnemyProperties ( 10, index, Random ( 4, 5 ), Random ( 10, 60 ), 4 )
                                if ( g_Level.TimerIncrement >= 120                                   ) then SetEnemyProperties ( 10, index, Random ( 4, 5 ), Random ( 10, 60 ), 3 )


                                g_LastPopup = i
                                exit
                        endif
                next i

                inc g_Level.EnemiesUp

                g_Level.Timer = 0
                g_Level.TimerIncrement = g_Level.TimerIncrement + 2
        endif

    for i = 1 to 9
                if g_Enemies [ i ].state = 1 then HandleEnemyUp   ( i )
        if g_Enemies [ i ].state = 2 then HandleEnemyTop  ( i )
        if g_Enemies [ i ].state = 3 then HandleEnemyDown ( i )
        if g_Enemies [ i ].state = 4 then HandleEnemyHit  ( i )
    next i
endfunction

function CheckForPause ( )
        if GetPointerPressed ( ) = 1
                x# = GetPointerX ( )
                y# = GetPointerY ( )

                if x# >= 0 and x# <= 60 and y# >= 0 and y# <= 50
                        for i = 1 to 9
                                StopSprite ( g_Enemies [ i ].sprite )
                        next i

                        StopSprite ( g_Item.sprite )

                        SetSpriteImage ( g_Level.pauseButton, g_Level.play )

                        g_PreviousState = g_State
                        g_PauseState = 0
                        g_State = ePause
                endif
        endif
endfunction

function PlayLevel ( )
        UpdateItems   ( )
        UpdateEnemiesInGame ( )
        CheckForPause ( )
        CheckForInput ( )
        UpdateInfo ( )
        UpdateTime ( )
endfunction

function HandlePause ( )
        if ( g_PauseState = 0 )
                SetTextPosition ( g_Level.continueText, 65.0, 200.0 )
                SetTextPosition ( g_Level.mainMenuText, 55.0, 280.0 )

                if ( GetTextColorAlpha ( g_Level.continueText ) < 255 - 4 )
                        SetTextColorAlpha ( g_Level.continueText, GetTextColorAlpha ( g_Level.continueText ) + 4 )
                        SetTextColorAlpha ( g_Level.mainMenuText, GetTextColorAlpha ( g_Level.mainMenuText ) + 4 )
                endif

                if GetPointerPressed ( ) = 1
                        x# = GetPointerX ( )
                        y# = GetPointerY ( )

                        // pause / play button
                        if ( x# >= 0 and x# <= 40 and y# >= 0 and y# <= 40 )
                                g_PauseState = 1
                        endif

                        // continue button
                        if ( x# >= 50 and x# <= 270 and y# >= 185 and y# <= 240 )
                                g_PauseState = 1
                        endif

                        // main menu button
                        if ( x# >= 40 and x# <= 280 and y# >= 265 and y# <= 320 )
                                g_PauseState = 2
                        endif
                endif
        else
                if ( GetTextColorAlpha ( g_Level.continueText ) > 4 )
                        SetTextColorAlpha ( g_Level.continueText, GetTextColorAlpha ( g_Level.continueText ) - 4 )
                        SetTextColorAlpha ( g_Level.mainMenuText, GetTextColorAlpha ( g_Level.mainMenuText ) - 4 )
                else
                        SetTextColorAlpha ( g_Level.continueText, 0 )
                        SetTextColorAlpha ( g_Level.mainMenuText, 0 )

                        SetTextPosition ( g_Level.continueText, -200.0, -200.0 )
                        SetTextPosition ( g_Level.mainMenuText, -200.0, -200.0 )

                        SetSpriteImage ( g_Level.pauseButton, g_Level.pause )

                        if ( g_PauseState = 1 )
                                g_State = g_PreviousState
                        else
                                alpha = GetSpriteColorAlpha ( g_Overlay )

                                if ( alpha < 255 - 4 )
                                        alpha = alpha + 4
                                        SetSpriteColor ( g_Overlay, 255, 255, 255, alpha )
                                else
                                        UnloadLevel ( )

                                        for i = 1 to 5
                                                SetTextVisible ( g_Text [ i ], 1 )
                                        next i

                                        g_Item.state = 0
                                        SetSpritePosition ( g_Item.sprite, -150, 15 )

                                        g_State = eDisplayFadeIn
                                endif
                        endif
                endif
        endif
endfunction

function UpdateInfo ( )
        SetTextString ( g_Level.scoreText, str ( g_Level.score ) )
        SetTextString ( g_Level.timerText, str ( g_Level.timeLimit ) )
endfunction

function UpdateTime ( )
        if ( GetSeconds ( ) > g_Level.lastTime )
                if ( g_Level.timeLimit > 0 )
                        dec g_Level.timeLimit
                endif

                g_Level.lastTime = GetSeconds ( )
        endif

        if ( g_Level.timeLimit < 5 )
                if g_Alarm = 0
                        PlaySound ( 8, 100, 1 )
                        g_Alarm = 1
                endif
        else
                StopSound ( 8 )
        endif

        if ( g_Level.timeLimit = 0 )
                g_Alarm = 0
                StopSound ( 8 )

                fAccuracy# = ( g_Level.enemiesHit /  g_Level.hitAttempts ) * 100.0

                for i = 1 to 9
                        StopSprite ( g_Enemies [ i ].sprite )
                next i

                if ( g_Level.enemiesHit >= g_Level.targetHit and fAccuracy# >= g_Level.targetAccuracy )
                        g_State = eLevelWon
                else
                        g_State = eLevelLost
                endif

                g_GameOverState = 0

                SetSpritePosition ( g_Level.miss, -100.0, -100.0 )
        endif
endfunction

function EndLevel ( mode as integer )
        if g_GameOverState = 0
                count = 0

                SetSpritePosition ( g_Level.pauseButton, -100.0, -100.0 )

                for i = 1 to 9
                        HandleEnemyDown ( i )

                        if ( g_Enemies [ i ].state = 0 )
                                inc count
                        endif
                next i

                if ( GetSpriteColorAlpha ( g_Item.sprite ) > 12 )
                        SetSpriteColorAlpha ( g_Item.sprite, GetSpriteColorAlpha ( g_Item.sprite ) - 12 )
                else
                        inc count
                endif

                if ( count = 10 )
                        g_GameOverState = 1
                endif
        endif

        if g_GameOverState = 1
                SetTextVisible ( g_Level.scoreText, 0 )
                SetTextVisible ( g_Level.timerText, 0 )

                percent# = ( g_Level.enemiesHit * 100.0 ) / g_Level.hitAttempts
                percent = percent#

                y# = 180

                for i = 1 to 12
                        g_StatsText [ i ] = CreateText ( "" )

                        SetTextPosition ( g_StatsText [ i ], -100.0, -100.0 )
                        SetTextColor    ( g_StatsText [ i ], 255, 255, 255, 0 )
                        SetTextDepth    ( g_StatsText [ i ], 0 )

                        if ( i = 1 )
                                SetTextPosition ( g_StatsText [ i ], 55.0, 50.0 )
                                SetTextSize     ( g_StatsText [ i ], 24.0 )
                        elseif ( i = 10 )
                                SetTextPosition ( g_StatsText [ i ], 23.0, 395.0 )
                                SetTextSize     ( g_StatsText [ i ], 18.0 )
                        else
                                SetTextSize     ( g_StatsText [ i ], 12.0 )
                        endif
                next i

                if ( mode = 1 ) then SetTextString ( g_StatsText [ 1 ], "WELL DONE" )
                if ( mode = 2 ) then SetTextString ( g_StatsText [ 1 ], "GAME OVER" )

                SetTextPosition ( g_StatsText [ 1 ], 55.0, 50.0 )

                text$ = "SCORE:_____________" + str ( g_Level.score )
                SetTextString ( g_StatsText [ 2 ], text$ )
                SetTextPosition ( g_StatsText [ 2 ], 10.0, 180.0 )

                text$ = "CRITTERS_UP:_______" + str ( g_Level.enemiesUp )
                SetTextString ( g_StatsText [ 3 ], text$ )
                SetTextPosition ( g_StatsText [ 3 ], 10.0, 200.0 )

                text$ = "HIT_ATTEMPTS:______" + str ( g_Level.hitAttempts )
                SetTextString ( g_StatsText [ 4 ], text$ )
                SetTextPosition ( g_StatsText [ 4 ], 10.0, 220.0 )

                text$ = "CRITTERS_HIT:______" + str ( g_Level.enemiesHit )
                SetTextString ( g_StatsText [ 5 ], text$ )
                SetTextPosition ( g_StatsText [ 5 ], 10.0, 240.0 )

                text$ = "CRITTERS_MISSED:___" + str ( g_Level.enemiesMissed )
                SetTextString ( g_StatsText [ 6 ], text$ )
                SetTextPosition ( g_StatsText [ 6 ], 10.0, 260.0 )

                text$ = "ACCURACY:__________" + str ( percent ) + "%"
                SetTextString ( g_StatsText [ 7 ], text$ )
                SetTextPosition ( g_StatsText [ 7 ], 10.0, 280.0 )

                text$ = "TARGET_HIT:________" + str ( g_Level.targetHit )
                SetTextString ( g_StatsText [ 8 ], text$ )
                SetTextPosition ( g_StatsText [ 8 ], 10.0, 300.0 )

                text$ = "TARGET_ACCURACY:___" + str ( g_Level.targetAccuracy ) + "%"
                SetTextString ( g_StatsText [ 9 ], text$ )
                SetTextPosition ( g_StatsText [ 9 ], 10.0, 320.0 )

                SetTextString ( g_StatsText [ 10 ], "TAP TO CONTINUE" )
                SetTextPosition ( g_StatsText [ 10 ], 23.0, 395.0 )

                g_GameOverState = 2
        endif

        if g_GameOverState = 2
                for i = 1 to 11
                        alpha = GetTextColorAlpha ( g_StatsText [ i ] )

                        if ( alpha < 255 - 4 )
                                alpha = alpha + 4
                        endif

                        SetTextColorAlpha ( g_StatsText [ i ], alpha )
                next i

                if GetPointerPressed ( ) = 1
                        UnloadLevel ( )

                        for i = 1 to 5
                                SetTextVisible ( g_Text [ i ], 1 )
                        next i

                        g_Item.state = 0
                        SetSpritePosition ( g_Item.sprite, -150, 15 )

                        g_State = eDisplayFadeIn
                endif
        endif
endfunction

function CheckForInput ( )
        if ( GetSpriteX ( g_Level.miss ) > 0 )
                if ( GetSpriteColorAlpha ( g_Level.miss ) > 2 )
                        SetSpriteColor ( g_Level.miss, 255, 255, 255, GetSpriteColorAlpha ( g_Level.miss ) - 2 )
                        SetSpriteY     ( g_Level.miss, GetSpriteY ( g_Level.miss ) + 1 )
                endif

                inc g_Level.missTimer

                if ( g_Level.missTimer > 120 )
                        SetSpritePosition ( g_Level.miss, -100.0, -100.0 )
                endif
        endif

        if GetPointerPressed ( ) = 1
                inc g_Level.hitAttempts

                hitOne = 0

                for i = 1 to 9
                        if ( g_Enemies [ i ].state = 1 or g_Enemies [ i ].state = 2 or g_Enemies [ i ].state = 3 )

                                if GetSpriteColorAlpha ( g_Enemies [ i ].sprite ) < 20
                                        exit
                                endif

                                if ( GetSpriteHitTest ( g_Enemies [ i ].sprite, GetPointerX ( ), GetPointerY ( ) ) = 1 )
                                        PlaySound ( Random ( 1, 5 ) )

                                        StopSprite ( g_Enemies [ i ].sprite )
                                        g_Enemies [ i ].temp = 0
                                        g_Enemies [ i ].state = 4

                                        inc g_Level.enemiesHit
                                        g_Level.score = g_Level.score + 5

                                        hitOne = 1
                                endif
                        endif
                next i

                if hitOne = 0
                        if GetPointerY ( ) > 100
                                // add to enemies missed
                                inc g_Level.enemiesMissed

                                PlaySound ( 7 )

                                // reduce time if it's possible
                                if ( g_Level.timeLimit > 5 )
                                        g_Level.timeLimit = g_Level.timeLimit - 5
                                endif

                                // show our miss sprite
                                SetSpriteColor    ( g_Level.miss, 255, 255, 255, 255 )
                                SetSpritePosition ( g_Level.miss, GetPointerX ( ) - 16.0, GetPointerY ( ) - 16.0 )

                                // reset miss timer
                                g_Level.missTimer = 0
                        endif
                endif
        endif
endfunction

function HandleWin ( )
        EndLevel ( 1 )
endfunction

function HandleLose ( )
        EndLevel ( 2 )
endfunction



















