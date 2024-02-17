
// custom text in AGK

// display a background
backdrop = CreateSprite ( LoadImage ( "background4.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// create a text entity
CreateText ( 2, "HELLO AGK!" )

// load an image
LoadImage ( 2, "custom.png" )

// apply this image to our text
SetTextFontImage ( 2, 2 )

// main loop
do
    // update the screen
    Sync ( )
loop
