
// animate a sprite part 1

// set a virtual resolution of 320 x 480
SetVirtualResolution ( 320, 480 )

// display a background
CreateSprite ( LoadImage ( "background.jpg" ) )

// create a sprite with ID that has no image
CreateSprite ( 1, 0 )
SetSpritePosition ( 1, 130, 200 )

// add individual images into an animation list
AddSpriteAnimationFrame ( 1, LoadImage ( "item0.png" ) )
AddSpriteAnimationFrame ( 1, LoadImage ( "item1.png" ) )
AddSpriteAnimationFrame ( 1, LoadImage ( "item2.png" ) )
AddSpriteAnimationFrame ( 1, LoadImage ( "item3.png" ) )
AddSpriteAnimationFrame ( 1, LoadImage ( "item4.png" ) )

// play the sprite at 10 fps, looping, going from frame 1 to 5
PlaySprite ( 1, 10, 1, 1, 5 )

// our main loop
do
    // update the screen
    Sync ( )
loop
