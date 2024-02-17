






function Game_Loop ( )

        select g_iGameState

                case 0:
                        Game_ScrollBackgrounds   ( )
                        Game_UpdateStars         ( )
                        Game_UpdateSnow          ( )
                        Game_UpdateFloatingScore ( )
                        Game_UpdateClouds        ( )
                        Game_ShowStartMessage    ( )
                endcase

                case 1:
                        g_iTimer = g_iTimer + 1
                        g_iGameScore = g_iGameScore + 1

                        Game_CheckInput          ( )

                        Game_ScrollBackgrounds   ( )
                        Game_UpdateStars         ( )
                        Game_UpdateSnow          ( )
                        Game_UpdateFloatingScore ( )
                        Game_UpdateClouds        ( )
                        Game_UpdateScore         ( )

                        Game_UpdateItems         ( )









                        //x= 12343905u43096u43096u3409u60
                endcase

                case 2:
                        Game_HandlePause ( )
                endcase

                case 4:
                        Game_ScrollBackgrounds   ( )
                        Game_UpdateStars         ( )
                        Game_UpdateSnow          ( )
                        Game_UpdateFloatingScore ( )
                        Game_UpdateClouds        ( )

                        for i = 1 to 5
                                iSprite = g_Item [ i ].iSprite

                                alpha = GetSpriteColorAlpha ( iSprite )

                                if ( alpha >= 8 )
                                        alpha = alpha - 8
                                else
                                        alpha = 0
                                endif

                                SetSpriteColorAlpha ( iSprite, alpha )
                        next i

                        if ( g_iMessageState = 0 )

                                PlaySound ( g_iSounds [ 2 ] )

                                SetTextSize ( g_iMessageText, 24.0 )
                                SetTextString ( g_iMessageText, "THE BAD ELF ESCAPED!" )
                                SetTextPosition ( g_iMessageText, 25, 150 )
                                SetTextColorAlpha ( g_iMessageText, 0 )

                                g_iMessageState = 1

                        elseif ( g_iMessageState = 1 )

                                alpha = GetTextColorAlpha ( g_iMessageText ) + 2

                                SetTextColorAlpha ( g_iMessageText, alpha )

                                if ( alpha > 255 )
                                        g_iMessageState = 2
                                endif

                        elseif ( g_iMessageState = 2 )

                                alpha = GetTextColorAlpha ( g_iMessageText ) - 2

                                SetTextColorAlpha ( g_iMessageText, alpha )

                                if ( alpha < 0 )
                                        SetTextString ( g_iMessageText, "GAME OVER" )
                                        SetTextPosition ( g_iMessageText, 140, 150 )
                                        SetTextColorAlpha ( g_iMessageText, 0 )

                                        g_iMessageState = 3
                                endif

                        elseif ( g_iMessageState = 3 )

                                alpha = GetTextColorAlpha ( g_iMessageText ) + 2

                                SetTextColorAlpha ( g_iMessageText, alpha )

                                if ( alpha > 255 )
                                        g_iMessageState = 4
                                endif

                        elseif ( g_iMessageState = 4 )

                                alpha = GetTextColorAlpha ( g_iMessageText ) - 2

                                SetTextColorAlpha ( g_iMessageText, alpha )

                                if ( alpha < 0 )
                                        g_iGameOver = 1
                                endif

                        endif
                endcase

        endselect


endfunction

function Game_ScrollBackgrounds ( )
        SetSpritePosition ( g_iGameBackground,  GetSpriteX ( g_iGameBackground  ) + 1.0, GetSpriteY ( g_iGameBackground  ) )
        SetSpritePosition ( g_iGameBackgroundA, GetSpriteX ( g_iGameBackgroundA ) + 1.0, GetSpriteY ( g_iGameBackgroundA ) )

        if ( GetSpriteX ( g_iGameBackground ) > 480 )
                SetSpritePosition ( g_iGameBackground, GetSpriteX ( g_iGameBackgroundA ) - 960.0, GetSpriteY ( g_iGameBackground ) )
        endif

        if ( GetSpriteX ( g_iGameBackgroundA ) > 480 )
                SetSpritePosition ( g_iGameBackgroundA, GetSpriteX ( g_iGameBackground ) - 960.0, GetSpriteY ( g_iGameBackgroundA ) )
        endif

        fSpeed# = 5.0

        for i = 1 to 3
                if ( i = 1 ) then fSpeed# = 2.0
                if ( i = 2 ) then fSpeed# = 4.0
                if ( i = 3 ) then fSpeed# = 6.0

                SetSpritePosition ( g_iForegrounds  [ i ], GetSpriteX ( g_iForegrounds  [ i ] ) + fSpeed#, GetSpriteY ( g_iForegrounds  [ i ] ) )
                SetSpritePosition ( g_iForegroundsA [ i ], GetSpriteX ( g_iForegroundsA [ i ] ) + fSpeed#, GetSpriteY ( g_iForegroundsA [ i ] ) )
        next i

        for i = 1 to 3
                if ( GetSpriteX ( g_iForegrounds [ i ] ) > 480 )
                        SetSpritePosition ( g_iForegrounds [ i ], GetSpriteX ( g_iForegroundsA [ i ] ) - 960.0, GetSpriteY ( g_iForegrounds [ i ] ) )
                endif

                if ( GetSpriteX ( g_iForegroundsA [ i ] ) > 480 )
                        SetSpritePosition ( g_iForegroundsA [ i ], GetSpriteX ( g_iForegrounds [ i ] ) - 960.0, GetSpriteY ( g_iForegroundsA [ i ] ) )
                endif
        next i
endfunction

function Game_UpdateStars ( )
        for i = 1 to 32
                iSprite = g_Stars [ i ].iSprite
                iState  = g_Stars [ i ].iState
                fSpeed# = g_Stars [ i ].fSpeed

                iAlpha = GetSpriteColorAlpha ( iSprite )

                select ( iState )
                        case 0:
                                if ( Random ( 0, 200 ) = 100 )
                                        x = Random ( 0, 480 )
                                        y = Random ( 20, 240 )

                                        SetSpritePosition ( iSprite, x, y )
                                        SetSpriteColor ( iSprite, 255, 255, 255, 0 )

                                        iState = 1
                                        fSpeed# = 12 + Random ( 0, 16 )
                                endif
                        endcase

                        case 1:
                                if ( iAlpha <= 255 - fSpeed# )
                                        iAlpha = iAlpha + fSpeed#
                                else
                                        iState = 2
                                endif
                        endcase

                        case 2:
                                if ( iAlpha >= fSpeed# )
                                        iAlpha = iAlpha - fSpeed#
                                else
                                        iState = 3
                                endif
                        endcase

                        case 3:
                                iAlpha = 0
                                iState = 0
                        endcase
                endselect

                SetSpriteColor ( iSprite, 255, 255, 255, iAlpha )

                g_Stars [ i ].fSpeed = fSpeed#
                g_Stars [ i ].iState = iState
        next i
endfunction

function Game_UpdateSnow ( )
        for i = 1 to 100
                iSprite = g_Snow [ i ].iSprite
                fMove#  = g_Snow [ i ].fMove
                fSpeed# = g_Snow [ i ].fSpeed
                fAngle# = g_Snow [ i ].fAngle

                fMove# = fMove# + 4.0

                SetSpriteAngle ( iSprite, GetSpriteAngle ( iSprite ) + fAngle# )

                x# = GetSpriteX ( iSprite ) + sin ( ( fMove# * 0.017 ) * ( 180.0 / 3.14 ) ) * 0.25

                SetSpritePosition ( iSprite, x#, GetSpriteY ( iSprite ) + fSpeed# )

                if ( GetSpriteY ( iSprite ) > 200 and GetSpriteColorAlpha ( iSprite ) > 8 )
                        SetSpriteColor ( iSprite, 255, 255, 255, GetSpriteColorAlpha ( iSprite ) - 8 )
                endif

                if ( GetSpriteY ( iSprite ) > 320 )
                        x# =  Random ( 0, 480 )
                        y# = -Random ( 0, 320 )
                        z# =  Random ( 0, 255 )

                        SetSpritePosition ( iSprite, x#, y# )

                        fMove# = z#

                        SetSpriteColor ( iSprite, 255, 255, 255, Random ( 0, 255 ) )
                endif

                g_Snow [ i ].fMove = fMove#

        next i
endfunction

function Game_UpdateFloatingScore ( )
        fY#    = GetTextY          ( g_iFloatingScore )
        iAlpha = GetTextColorAlpha ( g_iFloatingScore )

        select ( g_iFloatingScoreState )
                case 1:
                        if ( iAlpha < 255 - 16 )
                                iAlpha = iAlpha + 16
                        endif

                        fY# = fY# - 2

                        if ( fY# < 100 )
                                g_iFloatingScoreState = 2
                        endif
                endcase

                case 2:
                        fY# = fY# - 2

                        if ( iAlpha > 16 )
                                iAlpha = iAlpha - 16
                        else
                                iAlpha = 0
                                g_iFloatingScoreState = 0
                        endif
                endcase
        endselect

        SetTextY          ( g_iFloatingScore, fY# )
        SetTextColorAlpha ( g_iFloatingScore, iAlpha )
endfunction

function Game_UpdateClouds ( )
        for i = 1 to 3
                iSprite = g_Clouds [ i ].iSprite

                select ( g_Clouds [ i ].iState )
                        case 0:
                                j  = Random ( 0, 3 )
                                k# = 1.5
                                z# = 15 + Random ( 0, 140 )

                                if ( i = 1 ) then SetSpritePosition ( iSprite, -100.0, z# )
                                if ( i = 2 ) then SetSpritePosition ( iSprite, -250.0, z# )
                                if ( i = 3 ) then SetSpritePosition ( iSprite, -400.0, z# )

                                if ( j = 2 ) then k# = 1.6
                                if ( j = 3 ) then k# = 1.7

                                g_Clouds [ i ].fSpeed = k#
                                g_Clouds [ i ].iState = 1
                        endcase

                        case 1:
                                SetSpritePosition ( iSprite, GetSpriteX ( iSprite ) + g_Clouds [ i ].fSpeed, GetSpriteY ( iSprite ) )

                                if ( GetSpriteX ( iSprite ) > 550 )
                                        g_Clouds [ i ].iState = 0
                                endif
                        endcase
                endselect
        next i
endfunction

function Game_ShowStartMessage ( )

        if ( g_iMessageState = 0 )

                SetTextSize     ( g_iMessageText, 32.0 )
                SetTextString   ( g_iMessageText, "GET READY" )
                SetTextPosition ( g_iMessageText, 100.0, 150.0 )
                SetTextColor    ( g_iMessageText, 255, 255, 255, 0 )

                g_iReturn          = 0
                g_iCountDownNumber = 5
                g_iMessageState    = 1



        elseif ( g_iMessageState = 1 )

                if ( g_iCountDownAlpha < 255 - 4 )
                        g_iCountDownAlpha = g_iCountDownAlpha + 4
                        SetTextColor ( g_iMessageText, 255, 255, 255, g_iCountDownAlpha )
                else
                        g_iMessageState = 2
                endif

        elseif ( g_iMessageState = 2 )

                if ( g_iCountDownAlpha > 8 )
                        g_iCountDownAlpha = g_iCountDownAlpha - 8
                        SetTextColor ( g_iMessageText, 255, 255, 255, g_iCountDownAlpha )
                else
                        g_iTextAlpha = 0
                        SetTextColor ( g_iMessageText, 255, 255, 255, 0 )
                        g_iMessageState = 3
                endif

        elseif ( g_iMessageState = 3 )

                SetTextSize     ( g_iMessageText, 40.0 )
                SetTextString   ( g_iMessageText, str ( g_iCountDownNumber ) )
                SetTextPosition ( g_iMessageText, 220.0, 130.0 )

                Game_FadeIn ( g_iMessageText, 16 )

                if g_iReturn = 1
                        g_iReturn = 0
                        g_iMessageState = 4
                else
                        SetTextColor ( g_iMessageText, 255, 255, 255, g_iTextAlpha )
                endif

        elseif ( g_iMessageState = 4 )

                Game_FadeOut ( g_iMessageText, 16 )

                if g_iReturn = 1
                        g_iReturn = 0
                        g_iCountDownNumber = g_iCountDownNumber - 1
                        g_iMessageState = 3
                else
                        SetTextColor ( g_iMessageText, 255, 255, 255, g_iTextAlpha )
                endif

                if ( g_iCountDownNumber = 0 )
                PlaySound ( g_iSounds [ 3 ] )

                        SetTextColor ( g_iMessageText, 255, 255, 255, 0 )
                        g_iMessageState = 0
                        g_iGameState = 1
                        g_iMessageState = 5
                endif

        endif

endfunction

function Game_Fade ( iText, iDirection, iSpeed )
        g_iReturn = 0

        if ( iDirection = 0 )
                if ( g_iTextAlpha < 255 - iSpeed )
                        g_iTextAlpha = g_iTextAlpha + iSpeed
                else
                        //g_iTextAlpha = 255

                        g_iReturn = 1
                endif
        else
                if ( g_iTextAlpha > 0 + iSpeed )
                        g_iTextAlpha = g_iTextAlpha - iSpeed
                else
                        //g_iTextAlpha = 0
                        g_iReturn = 1
                endif
        endif
endfunction

function Game_FadeIn ( iText, iSpeed )
        Game_Fade ( iText, 0, iSpeed )
endfunction

function Game_FadeOut ( iText, iSpeed )
        Game_Fade ( iText, 1, iSpeed )
endfunction

function Game_UpdateScore ( )

        if ( g_iGameScoreAlpha < 255 - 4 )
                g_iGameScoreAlpha = g_iGameScoreAlpha + 4
        endif

        SetSpritePosition ( g_iTimeText, 0.0, 5.0 )
        SetSpritePosition ( g_iScoreText, 170.0, 5.0 )
        SetSpriteColor    ( g_iScoreText, 255, 255, 255, g_iGameScoreAlpha )
        SetSpriteColor    ( g_iTimeText, 255, 255, 255, g_iGameScoreAlpha )
        SetTextColor      ( g_iTime, 255, 255, 255, g_iGameScoreAlpha )
        SetTextColor      ( g_iScore, 255, 255, 255, g_iGameScoreAlpha )

        g_iThisTime = GetSeconds ( )

        if ( g_iThisTime > g_iLastTime )
                g_iRealTime = g_iRealTime + 1
                g_iLastTime = g_iThisTime
        endif

        SetTextString ( g_iTime, str ( g_iRealTime ) )
        SetTextPosition ( g_iTime, 54, 10 )

        if ( g_iScoreTarget < g_iGameScore - 8 )
                g_iScoreTarget = g_iScoreTarget + 8
        else
                g_iScoreTarget = g_iGameScore
        endif

        SetTextString ( g_iScore, str ( g_iScoreTarget ) )
        SetTextPosition ( g_iScore, 170 + 70, 10 )

endfunction


function Game_FindFreeItem ( )

        index = 0

        for i = 1 to 5
                if ( g_Item [ i ].iState = 0 )
                        g_iReturn = i
                        index = i
                        exit
                endif
        next i

endfunction index

function Game_CheckForItem ( )

        g_iBadElfTime = g_iBadElfTime + 1
        g_iReleaseTimer = g_iReleaseTimer + 1

        if ( g_iReleaseTimer > g_iReleaseTarget )
                i = Game_FindFreeItem( )
                i = g_iReturn
                iSprite = g_Item [ i ].iSprite


                // check all other items, we can't have two bad elfs at once

                iBadElf = 0

                for j = 1 to 5
                        if ( g_Item [ j ].iType = 0 )
                                iBadElf = 1
                                exit
                        endif
                next j

                g_Item [ i ].iType = Random ( 0, 6 )

                //g_Item [ i ].iType = 0

                if ( iBadElf = 0 )
                        if ( g_iBadElfTime > g_iBadElfTarget )
                                g_Item [ i ].iType = 0

                                g_iBadElfTime   = 0
                                g_iBadElfTarget = 100 + Random ( 0, 500 )
                        endif
                endif

                SetSpritePosition ( iSprite, 500, 120 )
                SetSpriteVisible  ( iSprite, 1 )

                g_Item [ i ].iFrame = 0
                g_Item [ i ].iState = 1
                g_Item [ i ].iTimeBetweenNextFrame = 0

                if ( g_fSpeed < 10.0 )
                        g_fSpeed = g_fSpeed + 0.2
                else
                        g_fSpeed = g_fSpeed + 0.05
                endif

                if ( g_iReleaseTarget > 50 )
                        g_iReleaseTarget = g_iReleaseTarget - 5
                endif

                g_iReleaseTimer = 0

                Game_UpdateFrameList ( i )
        endif

endfunction

function Game_UpdateFrameList ( iIndex )

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

        for j = 1 to 24
                g_FrameList [ iIndex, j ] = iFrames [ j ]
        next j

        for j = 1 to 24
                g_FrameList [ iIndex, j ] = g_FrameList [ iIndex, j ] + ( g_Item [ iIndex ].iType * 7 )
        next j

        SetSpriteImage ( g_Item [ iIndex ].iSprite, g_iImages [ g_FrameList [ iIndex, 1 ] ] )
        //SetSpriteImage ( g_Item [ iIndex ].iSprite, g_iImages [ 1 ] )
endfunction


function Game_UpdateItems ( )

        Game_CheckForItem ( )

        for i = 1 to 5
                iSprite = g_Item [ i ].iSprite

                select g_Item [ i ].iState

                        case 1:
                                inc g_Item [ i ].iTime

                                if ( g_Item [ i ].iTime > g_Item [ i ].iTimeBetweenNextFrame + 1 )
                                        g_Item [ i ].iTime = 0

                                        inc g_Item [ i ].iFrame

                                        if ( g_Item [ i ].iFrame >= g_Item [ i ].iFrameCount )
                                                g_Item [ i ].iFrame = 1
                                        endif

                                        SetSpriteImage ( iSprite, g_iImages [ g_FrameList [ i, g_Item [ i ].iFrame ] ] )
                                endif

                                SetSpriteX ( iSprite, GetSpriteX ( iSprite ) - g_fSpeed )

                                if ( GetSpriteX ( iSprite ) < -192 )
                                        g_Item [ i ].iState = 2

                                        if ( g_Item [ i ].iType = 0 )
                                                // we have an escapee
                                                g_iLose = ( g_iGameScore / 100.0 ) * 30.0
                                                g_iGameScore = g_iGameScore - g_iLose
                                                g_iMessageState = 0
                                                g_iGameState = 4
                                        endif

                                endif
                        endcase

                        case 2:
                                g_Item [ i ].iType = -1
                                g_Item [ i ].iFrame = 0
                                g_Item [ i ].iState = 0

                                SetSpriteColor ( iSprite, 255, 255, 255, 255 )
                        endcase

                        case 3:
                                SetSpriteImage ( iSprite, g_iSmokeImages [ g_Item [ i ].iFrame ] )

                                inc g_Item [ i ].iTime

                                if ( g_Item [ i ].iTime > g_Item [ i ].iTimeBetweenNextFrame + 1 )
                                        g_Item [ i ].iTime = 0

                                        inc g_Item [ i ].iFrame

                                        if ( g_Item [ i ].iFrame >= 5 )
                                                SetSpriteVisible ( iSprite, 0 )
                                                g_Item [ i ].iState = 2
                                        endif
                                endif
                        endcase

                endselect
        next i
endfunction


function Game_CheckInput ( )

        if ( g_iTap = 1 )

                iHit = 0

                for i = 1 to 5
                        if ( g_Item [ i ].iState = 1 and g_Item [ i ].iType = 0 and g_iTapY > 40 )
                                iIndex = Random ( 0, 4 )

                                iPlay  = 3 + iIndex

                                g_Item [ i ].iFrame = 1
                                g_Item [ i ].iTime = 0
                                g_Item [ i ].iTimeBetweenNextFrame = 2

                                SetSpriteImage ( g_Item [ i ].iSprite, g_iSmokeImages [ g_Item [ i ].iFrame ] )

                                g_Item [ i ].iState = 3
                                iHit = 1

                                PlaySound ( g_iSounds [ Random ( 4, 7 ) ] )

                                if ( g_iFloatingScoreState = 0 )
                                        // get the position as a percentage of the screen
                                        iSpriteX =  GetSpriteX ( g_Item [ i ].iSprite ) + ( GetSpriteWidth ( g_Item [ i ].iSprite ) / 2.0 )

                                        if ( iSpriteX > 480 )
                                                iSpriteX = 479
                                        endif

                                        if ( iSpriteX <= 0 )
                                                iSpriteX = 1
                                        endif

                                        fX# = ( iSpriteX / 480.0 ) * 100.0
                                        g_iGameScore = g_iGameScore + fX# * 4

                                        iDisplayScore = fX# * 4

                                        SetTextColor ( g_iFloatingScore, 255, 255, 255, 0 )
                                        SetTextString ( g_iFloatingScore, str ( iDisplayScore ) )

                                        SetTextPosition ( g_iFloatingScore, GetSpriteX ( g_Item [ i ].iSprite ) + 20 + 30, GetSpriteY ( g_Item [ i ].iSprite ) + 40 + 30 )

                                        g_iFloatingScoreState = 1
                                endif
                        endif
                next i

                if ( iHit = 0 and g_iTapY > 40 )
                        if ( g_iFloatingScoreState = 0 )
                                // get the position as a percentage of the screen
                                g_iLose = ( g_iGameScore / 100.0 ) * 30.0
                                g_iGameScore = g_iGameScore - g_iLose

                                SetTextColor ( g_iFloatingScore, 255, 0, 0, 0 )
                                SetTextString ( g_iFloatingScore, "-" + str ( g_iLose ) )
                                SetTextPosition ( g_iFloatingScore, 200.0, 150.0 )

                                PlaySound ( g_iSounds [ 8 ] )

                                g_iFloatingScoreState = 1
                        endif
                endif

                if ( g_iTapX > 433 and g_iTapY < 35 )
                        g_iPauseState = 1
                        g_iGameState  = 2
                endif

                g_iTap = 0
        endif

endfunction


function Game_End ( )
        g_iRealTime = 0

        SetTextString ( g_iTime, "" )
        SetTextSize  ( g_iTime, 18.0 )
        SetTextColor ( g_iTime, 255, 255, 255, 0 )
        SetTextPosition ( g_iTime, -100.0, -100.0 )

        SetTextString ( g_iScore, "" )
        SetTextSize  ( g_iScore, 18.0 )
        SetTextColor ( g_iScore, 255, 255, 255, 0 )
        SetTextPosition ( g_iScore, -100.0, -100.0 )

        SetTextString ( g_iFloatingScore, "" )
        SetTextSize  ( g_iFloatingScore, 20.0 )

        g_iFloatingScoreState = 0


        SetSpriteColor ( g_iScoreText, 255, 255, 255, 0 )
        SetSpriteColor ( g_iTimeText, 255, 255, 255, 0 )

        g_iMessageState = 0

        SetSpritePosition ( g_iGameBackground,     0.0, 0.0 )
        SetSpritePosition ( g_iGameBackgroundA, -960.0, 0.0 )

        for i = 1 to 3
                SetSpritePosition ( g_iForegrounds  [ i ],    0.0, 320.0 - 136.0 )
                SetSpritePosition ( g_iForegroundsA [ i ], -960.0, 320.0 - 136.0 )
        next i

        SetSpritePosition ( g_iCloseButton, 440.0, 5.0 )

        SetSpriteColor ( g_iOverlay, 255, 255, 255, 0 )

        SetSpritePosition ( g_iDialog, 60.0, 40.0 )
        SetSpriteColor ( g_iDialog, 255, 255, 255, 0 )


        g_iState = 0

        g_iTap = 0
        g_iTapX = 0
        g_iTapY = 0
        g_iGameScore = 0
        g_iReleaseTimer  = 150
        g_iReleaseTarget = 200
        g_iScoreTarget = 0


        for i = 1 to 5
                g_Item [ i ].iType = -1
                g_Item [ i ].iState = 0
                g_Item [ i ].iTime = 0
                g_Item [ i ].iFrame = 0
                g_Item [ i ].iTimeBetweenNextFrame = 0

                SetSpriteColor ( g_Item [ i ].iSprite, 255, 255, 255, 255 )

                SetSpriteX ( g_Item [ i ].iSprite, 10000.0 )
        next i


        g_iTimer                = 201
        g_iDelayBeforeRelease   = 200
        g_fSpeed                = 2.5
        g_iRealTime             = 0
        g_iPause                = 0
        g_iPauseState           = 0
        g_iBadElfTime           = 0
        g_iBadElfTarget         = Random ( 0, 50 )

        //g_iIntroSound = 0

        //g_iLose = 0

        g_iGameOver = 0

        g_iMessageState = 0
        g_iGameState = 0

endfunction






















