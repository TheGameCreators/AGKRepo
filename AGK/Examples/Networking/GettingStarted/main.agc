SetDisplayAspect ( 4.0 / 3.0 )


LoadImage ( 1, "chip5.png" )
LoadImage ( 2, "chip25.png" )
backdrop=CreateSprite ( LoadImage ( "background1b.jpg" ) )
SetSpriteSize (backdrop,100,100)


CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 10, 20 )
SetSpriteSize ( 1, 10, -1 )


CreateSprite ( 2, 2 )
SetSpritePosition ( 2, 50, 20 )
SetSpriteSize ( 2, 10, -1 )


State = 0
Typ = 0
NetworkID = 0


do
    if State = 0
        Print ( "Select yellow chip to host a game" )
        Print ( "Select blue chip to join a game" )
        

        if GetPointerPressed ( ) = 1
            hit = GetSpriteHit ( GetPointerX ( ), GetPointerY ( ) )
        

            if ( hit = 1 )
                NetworkID = HostNetwork ( "AGK Test Game", "Player 1", 48230 )
                Typ = 0
            endif
        

            if ( hit = 2 )
                NetworkID = JoinNetwork ( "AGK Test Game", "Player 2" )
                Typ = 1
            endif
            State = 1
        endif
    endif


    if State = 1 and IsNetworkActive ( NetworkID ) <> 0
        id = GetNetworkFirstClient ( NetworkID )
        

        while id <> 0
            Print ( GetNetworkClientName ( NetworkID, id ) )
        

            id = GetNetworkNextClient ( NetworkID )
        endwhile
        

        if Typ = 0
            x# = GetSpriteX ( 1 ) + GetDirectionX ( )
            y# = GetSpriteY ( 1 ) + GetDirectionY ( )
        

            SetSpritePosition ( 1, x#, y# )
        

            message = CreateNetworkMessage ( )
            

            AddNetworkMessageFloat ( message, x# )
            AddNetworkMessageFloat ( message, y# )
            

            SendNetworkMessage ( NetworkID, 0, message )
        endif
        

        if Typ = 1
            message = GetNetworkMessage ( NetworkID )
            

            while message <> 0 
                x# = GetNetworkMessageFloat ( message )
                y# = GetNetworkMessageFloat ( message )
                

                SetSpritePosition ( 1, x#, y# )
                

                DeleteNetworkMessage ( message )
                

                message = GetNetworkMessage ( NetworkID )
            endwhile
        endif
    endif


    Sync ( )
loop