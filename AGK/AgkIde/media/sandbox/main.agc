backdrop = CreateSprite ( LoadImage ( "background1b.jpg" ) )
SetSpriteColorAlpha ( backdrop, 100 )
SetSpriteSize ( backdrop, 100, 100 )


sound = LoadSound ( "countdown.wav" )


do
    Print ( "Touch or click the screen" )
    Print ( "to play a sound" )


    if GetPointerPressed ( ) = 1
        PlaySound ( sound )
    endif


    Sync ( )
loop
