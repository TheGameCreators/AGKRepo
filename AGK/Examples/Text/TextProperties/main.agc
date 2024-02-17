
// setting properties of text

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background5.jpg" ) )
SetSpriteColorAlpha ( backdrop, 100 )

// create five text objects
CreateText ( 1, "HELLO" )
CreateText ( 2, "HELLO" )
CreateText ( 3, "HELLO" )
CreateText ( 4, "HELLO" )
CreateText ( 5, "Text!" )

// set properties for text 1
SetTextPosition ( 1, 150, 100 )
SetTextSize ( 1, 12 )

// set properties for text 2
SetTextPosition ( 2, 150, 100 + GetTextTotalHeight ( 1 ) )
SetTextAlignment ( 2, 1 )
SetTextSize ( 2, 12 )

// set properties for text 3
SetTextPosition ( 3, 150, 100 + GetTextTotalHeight ( 1 ) + GetTextTotalHeight ( 2 ) )
SetTextAlignment ( 3, 2 )
SetTextSize ( 3, 12 )

// set properties for text 4
SetTextPosition ( 4, 50, 200 )
SetTextSize ( 4, 12 )
SetTextSpacing ( 4, 10.0 )

// set properties for text 5
SetTextPosition ( 5, 100, 250 )
SetTextSize ( 5, 40 )
SetTextCharColor ( 5, 0, 255, 0, 0, 255 )
SetTextCharColor ( 5, 1, 0, 255, 0, 255 )
SetTextCharColor ( 5, 2, 0, 0, 255, 255 )

// main loop
do
    // update the screen
    Sync ( )
loop
