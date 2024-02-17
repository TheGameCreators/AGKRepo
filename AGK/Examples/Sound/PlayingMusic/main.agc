
// play music

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
image = LoadImage( "background5.jpg" )
backdrop = CreateSprite( image )
SetSpriteSize( backdrop, 100, 100 )

// load music into ID slot 1
LoadMusicOGG ( 1, "music.ogg" )

// play and loop the music
PlayMusicOGG( 1, 1 )

// main loop
do
    // update the screen
    Print( "Duration: " + str(GetMusicDurationOGG(1)) )
    Print( "Poisiton: " + str(GetMusicPositionOGG(1)) )
    Print( "Loop Count: " + str(GetMusicLoopCountOGG(1)) )
    Sync ( )
loop
