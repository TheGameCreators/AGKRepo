
function CreatePlayer ( )
    g_iPlayerShip = CreateSprite ( LoadImage ( "space/playership.png" ) )

        SetSpriteUVBorder ( g_iPlayerShip, 1 )

        SetImageWrapU ( g_iPlayerShip, 0 )
        SetImageWrapV ( g_iPlayerShip, 0 )

        for i = 1 to 10
                g_Bullets [ i ].iID = 0
                g_Bullets [ i ].iExplosion = 0
        next i

        ResetPlayer ( )

        g_iPlayerWeapon = LoadImage ( "space/laser.png" )

        ResetBullets ( )
endfunction

function ResetPlayer ( )
        SetSpritePosition ( g_iPlayerShip, 200, 200 )

        SetSpriteVisible ( g_iPlayerShip, 1 )

        if GetParticlesExists ( g_iPlayerEngine ) = 1
        DeleteParticles ( g_iPlayerEngine )
    endif

    g_iPlayerEngine = CreateParticles ( 200, 208 )

    SetParticlesImage         ( g_iPlayerEngine, 4 )
    SetParticlesStartZone     ( g_iPlayerEngine, 0, 0, 0, 0 )
    SetParticlesDirection     ( g_iPlayerEngine, -50, 0 )
    SetParticlesLife          ( g_iPlayerEngine, 0.3 )
    SetParticlesSize          ( g_iPlayerEngine, 16 )
    SetParticlesAngle         ( g_iPlayerEngine, 80 )
    SetParticlesFrequency     ( g_iPlayerEngine, 60 )
    AddParticlesColorKeyFrame ( g_iPlayerEngine, 0.0, 221, 136, 5, 255 )
    AddParticlesColorKeyFrame ( g_iPlayerEngine, 0.3, 221, 136, 5, 0 )

    if GetParticlesExists ( g_iPlayerExplosion ) = 1
        DeleteParticles ( g_iPlayerExplosion )
    endif

    g_iPlayerExplosion = CreateParticles ( -1000, -1000 )
endfunction

function ResetBullets ( )
    for i = 1 to 10
                if GetSpriteExists ( g_Bullets [ i ].iID ) = 0
                        g_Bullets [ i ].iID = CreateSprite ( g_iPlayerWeapon )
        endif

                if GetParticlesExists ( g_Bullets [ i ].iExplosion ) = 1
                        DeleteParticles ( g_Bullets [ i ].iExplosion )
        endif

                SetSpritePosition ( g_Bullets [ i ].iID, -100.0, -100.0 )
                SetSpriteColorAlpha ( g_Bullets [ i ].iID, 0 )

                g_Bullets [ i ].iActive = 0
                g_Bullets [ i ].fLife = 0
        next i
endfunction

function HidePlayer ( )
        SetSpriteVisible ( g_iPlayerShip, 0 )
        SetParticlesVisible ( g_iPlayerEngine, 0 )
endfunction

function ShowPlayer ( )
        SetSpriteVisible ( g_iPlayerShip, 1 )
        SetParticlesVisible ( g_iPlayerEngine, 1 )
endfunction

function UpdatePlayer ( )
    select ( g_iPlayerState )

        case 0:
            MovePlayer ( )
            CheckPlayerWithScenery ( )
            CheckPlayerWithAliens ( )
            FireBullets ( )
            UpdateBullets ( )
            CheckBulletsWithAliens ( )
        endcase

        case 1:
            DestroyPlayer ( )
        endcase

    endselect
endfunction

function MovePlayer ( )
    x# = 1.0
    y# = 0.0

    if ( GetViewOffsetX ( ) > 15300 )
                HidePlayer ( )
                g_iGameState = 2
        endif

        SetViewOffset ( GetViewOffsetX ( ) + x#, 0 )

        fScreenX# = WorldToScreenX ( GetSpriteX ( g_iPlayerShip ) )

        x1# = GetDirectionX ( )
        y1# = GetDirectionY ( )

        x# = x# + x1# * 2
        y# = y# + y1# * 2

        if fScreenX# > 500 and x1# > 0.0
                x# = 1.0
    endif

        if fScreenX# < 20 and x1# < 0.0
                x# = 1.0
    endif

    SetSpritePosition    ( g_iPlayerShip, GetSpriteX ( g_iPlayerShip ) + x#, GetSpriteY ( g_iPlayerShip ) + y# )
        SetParticlesPosition ( g_iPlayerEngine, GetParticlesX ( g_iPlayerEngine ) + x#, GetParticlesY ( g_iPlayerEngine ) + y# )
endfunction

function CheckPlayerWithScenery ( )

    for i = 1 to g_iSceneryCount

                if GetSpriteExists ( g_iScenery [ i ] ) = 1
                        if GetSpriteCollision ( g_iPlayerShip, g_iScenery [ i ] ) = 1
                                KillPlayer ( )
                                PlaySound ( 5 )
                                exit
                        endif
                endif
    next i

endfunction

function CheckPlayerWithAliens ( )
    for j = 1 to 10
                if ( GetSpriteCollision ( g_iPlayerShip, g_Aliens [ j ].iSprite ) = 1 )
                        x# = GetSpriteX ( g_Aliens [ j ].iSprite )
                        y# = GetSpriteY ( g_Aliens [ j ].iSprite )

                        ExplodeAlien ( x#, y# )

                        SetSpritePosition    ( g_Aliens [ j ].iSprite, -300, -300 )
                        SetParticlesPosition ( g_Aliens [ j ].iEngine, -300, -300 )

                        KillPlayer ( )

                        PlaySound ( 5 )
                        exit
        endif
        next j
endfunction

function FireBullets ( )
    // fire a bullet
        g_fPlayerShoot = g_fPlayerShoot + 0.1

        if g_fPlayerShoot >= 2.0
        for i = 1 to 10
                        if g_Bullets [ i ].iActive = 0
                                PlaySound ( 1 )

                                SetSpritePosition ( g_Bullets [ i ].iID, GetSpriteX ( g_iPlayerShip ) + 28, GetSpriteY ( g_iPlayerShip ) )
                                g_Bullets [ i ].iActive = 1
                                goto FireBulletsEnd
                        endif
                next i

FireBulletsEnd:
                g_fPlayerShoot = 0.0
        endif
endfunction

function UpdateBullets ( )
    for i = 1 to 10
                if ( g_Bullets [ i ].iActive = 1 )
                        SetSpritePosition ( g_Bullets [ i ].iID, GetSpriteX ( g_Bullets [ i ].iID ) + 4.5, GetSpriteY ( g_Bullets [ i ].iID ) )
                        g_Bullets [ i ].fLife = g_Bullets [ i ].fLife + 0.1

                        if ( g_Bullets [ i ].fLife < 10.0 )
                                if ( GetSpriteColorAlpha ( g_Bullets [ i ].iID ) < 200 )
                                        SetSpriteColorAlpha ( g_Bullets [ i ].iID, GetSpriteColorAlpha ( g_Bullets [ i ].iID ) + 32 )
                endif
                        endif

                        if ( g_Bullets [ i ].fLife >= 1.5 )
                                if ( GetSpriteColorAlpha ( g_Bullets [ i ].iID ) > 32 )
                                        SetSpriteColorAlpha ( g_Bullets [ i ].iID, GetSpriteColorAlpha ( g_Bullets [ i ].iID ) - 32 )
                                else
                                        SetSpriteColorAlpha ( g_Bullets [ i ].iID, 0 )
                endif
                        endif

                        if ( g_Bullets [ i ].fLife > 6.0 )
                                SetSpriteColorAlpha ( g_Bullets [ i ].iID, 0 )
                                SetSpritePosition ( g_Bullets [ i ].iID, -100.0, -100.0 )
                                g_Bullets [ i ].fLife = 0.0
                                g_Bullets [ i ].iActive = 0
                        endif
                endif
        next i
endfunction

function KillPlayer ( )
    SetSpriteVisible          ( g_iPlayerShip, 0 )
        SetParticlesVisible       ( g_iPlayerEngine, 0 )
        SetParticlesPosition      ( g_iPlayerExplosion, GetSpriteX ( g_iPlayerShip ) + 22, GetSpriteY ( g_iPlayerShip ) )
        SetParticlesImage         ( g_iPlayerExplosion, 4 )
        SetParticlesStartZone     ( g_iPlayerExplosion, 0.0, 0.0, 0.0, 0.0 )
        SetParticlesDirection     ( g_iPlayerExplosion, -32.0, -32.0 )
        SetParticlesLife          ( g_iPlayerExplosion, 2.0 )
        SetParticlesSize          ( g_iPlayerExplosion, 64 )
        SetParticlesAngle         ( g_iPlayerExplosion, 360 )
        SetParticlesFrequency     ( g_iPlayerExplosion, 120 )
        AddParticlesColorKeyFrame ( g_iPlayerExplosion, 0.0, 121, 1, 255, 255 )
        AddParticlesColorKeyFrame ( g_iPlayerExplosion, 2.0, 1, 252, 255, 0 )
        AddParticlesForce         ( g_iPlayerExplosion, 1.1, 2.0, 0.0, 250.0 )
        AddParticlesForce         ( g_iPlayerExplosion, 1.5, 2.0, -250.0, 0.0 )
        SetParticlesMax           ( g_iPlayerExplosion, 250 )

        DestroyAllBullets ( )

        dec g_iLives
    g_iPlayerState = 1
endfunction

function DestroyAllBullets ( )
    for i = 1 to 10
                if g_Bullets [ i ].iActive = 1
                        fX# = GetSpriteX ( g_Bullets [ i ].iID )
                        fY# = GetSpriteY ( g_Bullets [ i ].iID )

                        SetSpritePosition ( g_Bullets [ i ].iID, -100.0, -100.0 )

                        explode = CreateParticles ( fX#, fY# )

                        g_Bullets [ i ].iExplosion = explode

                        SetParticlesImage         ( explode, 4 )
                        SetParticlesStartZone     ( explode, -20, 20, -20, 20 )
                        SetParticlesDirection     ( explode, 8, 8 )
                        SetParticlesLife          ( explode, 2.0 )
                        SetParticlesSize          ( explode, 16 )
                        SetParticlesAngle         ( explode, 360 )
                        SetParticlesFrequency     ( explode, 120 )
                        AddParticlesColorKeyFrame ( explode, 0.0, 120, 0, 255, 255 )
                        AddParticlesColorKeyFrame ( explode, 2.0, 255, 100, 255, 0 )

                        AddParticlesForce ( explode, 1.5, 2.0, 100, 100 )
                        AddParticlesForce ( explode, 1.5, 2.0, 250, 0 )

                        SetParticlesMax ( explode, 64 )
                endif
        next i
endfunction

function DestroyPlayer ( )
    if ( GetParticlesMaxReached ( g_iPlayerExplosion ) and g_iDelay = 0 )
        g_iDelay = Timer ( )
    endif

    if ( g_iDelay <> 0 and Timer ( ) + g_iDelay + 2 <> 0 )
        SetViewOffset ( 0, 0 )

                ResetPlayer ( )
                ResetBullets ( )
                ResetAliens ( )

                g_iDelay = 0
                g_iPlayerState = 0

                g_iScoreTarget = 0
                g_iScore = 0
    endif

    if ( g_iLives = 0 )
        g_iDelay = 0
                g_iGameState = 2
        endif
endfunction

