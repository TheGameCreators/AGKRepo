
//
// String tokens

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background5.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// init code
stringtouse$="hello,world,this,is,my,cut,up,string"

// main loop
do
	// main code
    counttokens = CountStringTokens(stringtouse$,",")
    for n=1 to counttokens
     Print(GetStringToken(stringtouse$,",",n))
    next n

    // update the screen
    Sync ( )
loop
