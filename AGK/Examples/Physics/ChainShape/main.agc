
// Project: ChainShape 
// Created: 2017-12-11

// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "ChainShape" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

SetPhysicsScale( 0.1 )
SetPhysicsDebugOn ( )

// create a ball
LoadImage( 1, "ball-64.png" )
CreateSprite( 1, 1 )
SetSpriteSize( 1, 50, 50 )
SetSpritePosition( 1, 50, 100 )
// make it a circel
SetSpriteShape( 1, 1 )
// make it dynamic
SetSpritePhysicsOn( 1, 2 )

// create a terrain
CreateDummySprite( 2 )
// place it at 0,0 so we can use world coordinates for the vertices
SetSpritePositionByOffset( 2, 0, 0 ) 
SetSpriteShapeChain( 2, 10, 0, 0, 20, 400, 1 )
SetSpriteShapeChain( 2, 10, 1, 0, 80, 500, 1 )
SetSpriteShapeChain( 2, 10, 2, 0, 210, 550, 1 )
SetSpriteShapeChain( 2, 10, 3, 0, 300, 570, 1 )
SetSpriteShapeChain( 2, 10, 4, 0, 360, 570, 1 )
SetSpriteShapeChain( 2, 10, 5, 0, 430, 535, 1 )
SetSpriteShapeChain( 2, 10, 6, 0, 484, 500, 1 )
SetSpriteShapeChain( 2, 10, 7, 0, 532, 520, 1 )
SetSpriteShapeChain( 2, 10, 8, 0, 600, 600, 1 )
SetSpriteShapeChain( 2, 10, 9, 0, 715, 626, 1 )
// make it static
SetSpritePhysicsOn( 2, 1 )

do
    Sync()
loop
