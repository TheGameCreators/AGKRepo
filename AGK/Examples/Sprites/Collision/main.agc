// check collision between two sprites

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
CreateSprite ( LoadImage ( "background2.jpg" ) )

// load two images
imageA = LoadImage ( "blue.png" )
imageB = LoadImage ( "purple.png" )

// create a sprite from imageA and a sprite from imageB
spriteA = CreateSprite ( imageA )
spriteB = CreateSprite ( imageB )

// set the position of spriteA on the left of the screeen,
// while spriteB is positioned over to the right
SetSpritePosition ( spriteA, 0, 200 )
SetSpritePosition ( spriteB, 200, 200 )

// add collision shapes for both sprites (using boxes)
SetSpriteShape ( spriteA, 2 )
SetSpriteShape ( spriteB, 2 )

// our main loop
do
    // move spriteA over to the right
    SetSpritePosition ( spriteA, GetSpriteX ( spriteA ) + 1, 200 )

    // check for a collision taking place
    if GetSpriteCollision ( spriteA, spriteB ) = 1
        // set both sprites color to green
        SetSpriteColor ( spriteA, 0, 255, 0, 255 )
        SetSpriteColor ( spriteB, 0, 255, 0, 255 )
    endif

    // update the screen
    Sync ( )
loop
