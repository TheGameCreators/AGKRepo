
// testing whether a sprite is in a zone

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background6.jpg" ) )

// load image and create a sprite from it
LoadImage ( 1, "peach.png" )
CreateSprite ( 1, 1 )
SetSpritePosition ( 1, 0, 0 )

// main loop
do
    // move the sprite right
    SetSpriteX ( 1, GetSpriteX ( 1 ) + 1 )

    // when it is in this box change its color to green
    if GetSpriteInBox ( 1, 200, 0, 300, 50 ) = 1
        SetSpriteColor ( 1, 0, 255, 0, 255 )
    endif

    // update the screen
    Sync ( )
loop
