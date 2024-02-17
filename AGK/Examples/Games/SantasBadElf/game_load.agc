
function Game_Begin ( )
        g_iGameBackground  = CreateSprite ( LoadImage ( "background.png" ) )
        g_iGameBackgroundA = CreateSprite ( GetSpriteImageID ( g_iGameBackground ) )

        SetSpritePosition ( g_iGameBackgroundA, -960.0, 0.0 )

        iA     = LoadImage ( "foreground1.png" )
        iB     = LoadImage ( "foreground2.png" )
        iC     = LoadImage ( "foreground3.png" )
        iImage = 0

        for i = 1 to 3
                if ( i = 1 ) then iImage = iA
                if ( i = 2 ) then iImage = iB
                if ( i = 3 ) then iImage = iC

                g_iForegrounds  [ i ] = CreateSprite ( iImage )
                g_iForegroundsA [ i ] = CreateSprite ( iImage )

                SetSpritePosition ( g_iForegrounds  [ i ],    0.0, 320.0 - 136.0 )
                SetSpritePosition ( g_iForegroundsA [ i ], -960.0, 320.0 - 136.0 )
        next i



        g_iTime = CreateText ( "" )
        SetTextSize ( g_iTime, 18.0 )
        SetTextColor ( g_iTime, 255, 255, 255, 0 )
        SetTextDepth ( g_iTime, 1 )

        g_iScore = CreateText ( "" )
        SetTextSize ( g_iScore, 18.0 )
        SetTextColor ( g_iScore, 255, 255, 255, 0 )
        SetTextDepth ( g_iScore, 1 )

        g_iFloatingScore = CreateText ( "" )
        SetTextSize ( g_iFloatingScore, 28.0 )
        SetTextDepth ( g_iFloatingScore, 1 )
        g_iFloatingScoreState = 0

        g_iMessageText = CreateText ( "" )
        SetTextSize ( g_iMessageText, 20.0 )
        g_iMessageState = 0

        g_iCloseButton = CreateSprite ( LoadImage ( "close.png" ) )
        SetSpritePosition ( g_iCloseButton, 440.0, 5.0 )

        g_iScoreText = CreateSprite ( LoadImage ( "score.png" ) )
        SetSpritePosition ( g_iScoreText, -100.0, -100.0 )
        SetSpriteColor ( g_iScoreText, 255, 255, 255, 0 )

        g_iTimeText = CreateSprite ( LoadImage ( "time.png" ) )
        SetSpritePosition ( g_iTimeText, -100.0, -100.0 )
        SetSpriteColor ( g_iTimeText, 255, 255, 255, 0 )

        g_iDialog = CreateSprite ( LoadImage ( "dialog.png" ) )
        SetSpritePosition ( g_iDialog, 60.0, 40.0 )
        SetSpriteColor ( g_iDialog, 255, 255, 255, 0 )

        iStar = LoadImage ( "star.png" )

        for i = 1 to 32
                g_Stars [ i ].iSprite = CreateSprite ( iStar )

                SetSpritePosition ( g_Stars [ i ].iSprite, -100.0, -100.0 )

                g_Stars [ i ].iState = 0
                g_Stars [ i ].fSpeed = 0.0
        next i


        g_iGameState     = 0
        g_iGameScore     = 0
        g_iReleaseTimer  = 150
        g_iReleaseTarget = 200
        g_iScoreTarget   = 0

        g_Clouds [ 1 ].iSprite = CreateSprite ( LoadImage ( "cloud1.png" ) )
        g_Clouds [ 1 ].iState = 0

        g_Clouds [ 2 ].iSprite = CreateSprite ( LoadImage ( "cloud2.png" ) )
        g_Clouds [ 2 ].iState = 0

        g_Clouds [ 3 ].iSprite = CreateSprite ( LoadImage ( "cloud3.png" ) )
        g_Clouds [ 3 ].iState = 0

        Game_LoadAnimationImages ( )
endfunction

function Game_LoadAnimationImages ( )



        dim directory [ 8 ] as string



        directory [ 1 ] = "bad_elf/"
        directory [ 2 ] = "good_elf/"
        directory [ 3 ] = "mrs_claus/"
        directory [ 4 ] = "penguin/"
        directory [ 5 ] = "reindeer/"
        directory [ 6 ] = "santa/"
        directory [ 7 ] = "snowman/"
        directory [ 8 ] = ""



        iDir    = 1
        iIndex  = 0

        for i = 1 to 49
                image$ = directory [ iDir ] + str ( iIndex ) + ".png"
                iIndex = iIndex + 1

                if ( i = 1 )
                        g_iImages [ i ] = LoadImage ( image$ )
                elseif ( i < 8 )
                        g_iImages [ i ] = g_iImages [ i - 1 ]
                else
                        g_iImages [ i ] = LoadImage ( image$ )
                endif

                if mod(i, 7) = 0
                        iIndex = 0
                        iDir = iDir + 1
                endif
        next i



        for i = 1 to 6
                g_iSmokeImages [ i ] = LoadImage ( "smoke/" + str ( i ) + ".png" )
                g_iSnowImages  [ i ] = LoadImage ( "snow/"  + str ( i ) + ".png" )
        next i


        for i = 1 to 100
                a# =  Random ( 0, 200 )
                b# =  Random ( 0,  20 )
                c# =  Random ( 0,  50 )
                d# =  Random ( 0, 480 )
                e# = -Random ( 0, 320 )

                g_Snow [ i ].iSprite = CreateSprite ( g_iSnowImages [ Random ( 1, 5 ) ] )
                g_Snow [ i ].fMove   = a#
                g_Snow [ i ].fSpeed  = b# / 10.0
                g_Snow [ i ].fAngle  = c# / 10.0

                SetSpritePosition ( g_Snow [ i ].iSprite, d#, e# )
                SetSpriteColor    ( g_Snow [ i ].iSprite, 255, 255, 255, Random ( 0, 255 ) )
        next i

        Game_CreateSprites ( )

        g_iBadElfTarget = Random ( 0, 50 )

        g_iOverlay = CreateSprite ( LoadImage ( "black.png" ) )
        SetSpriteSize ( g_iOverlay, 480.0, 320.0 )
        SetSpriteColor ( g_iOverlay, 255, 255, 255, 0 )
endfunction

function Game_CreateSprites ( )
        dim iFrames [ 24 ]

        iFrames [  1 ] = 0 + 1
        iFrames [  2 ] = 1 + 1
        iFrames [  3 ] = 2 + 1
        iFrames [  4 ] = 3 + 1
        iFrames [  5 ] = 2 + 1
        iFrames [  6 ] = 1 + 1
        iFrames [  7 ] = 0 + 1
        iFrames [  8 ] = 4 + 1
        iFrames [  9 ] = 5 + 1
        iFrames [ 10 ] = 6 + 1
        iFrames [ 11 ] = 5 + 1
        iFrames [ 12 ] = 4 + 1
        iFrames [ 13 ] = 0 + 1
        iFrames [ 14 ] = 4 + 1
        iFrames [ 15 ] = 5 + 1
        iFrames [ 16 ] = 6 + 1
        iFrames [ 17 ] = 5 + 1
        iFrames [ 18 ] = 4 + 1
        iFrames [ 19 ] = 0 + 1
        iFrames [ 20 ] = 1 + 1
        iFrames [ 21 ] = 2 + 1
        iFrames [ 22 ] = 3 + 1
        iFrames [ 23 ] = 2 + 1
        iFrames [ 24 ] = 1 + 1

        for i = 1 to 5
                g_Item [ i ].iSprite               = CreateSprite ( g_iImages [ 1 ] )
                g_Item [ i ].iState                =   0
                g_Item [ i ].iTime                 =   0
                g_Item [ i ].iFrame                =   0
                g_Item [ i ].iTimeBetweenNextFrame =   0
                g_Item [ i ].iType                 =  -1
                g_Item [ i ].iFrameCount           =  24

                for j = 1 to 24
                        g_FrameList [ i, j ] = iFrames [ j ]
                next j

                for j = 1 to 24
                        g_FrameList [ i, j ] = g_FrameList [ i, j ] + ( g_Item [ i ].iType * 7 )
                next j

                SetSpriteX ( g_Item [ i ].iSprite, 10000.0 )
        next i

endfunction
















