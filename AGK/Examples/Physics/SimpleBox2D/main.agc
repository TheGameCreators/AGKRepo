
// simple example showing plenty of sprites using box2d

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// stop screen from clearing - be aware that this command will NOT work
// on some devices (Android) where the backbuffer is not retained each cycle
if Left(Lower(GetDeviceName()),Len("android"))<>"android"
 EnableClearColor ( 0 )
endif

// display a background
backdrop = CreateSprite ( LoadImage ( "background2.jpg" ) )
SetSpriteColorAlpha ( backdrop, 40 )

// load crate image
LoadImage ( 2, "silver.png" )

// set default gravity
SetPhysicsGravity ( 0, 0.5 )

// set up time and index variables
time = 0
index = 10

// main loop
do
    // increment time
    time = time + 1

    // release a new sprite every so often
    if ( time >= 75 )
        CreateSprite ( index, 2 )
        SetSpritePosition ( index, 50, 0 )
        SetSpritePhysicsOn ( index, 2 )

        // set properties
        size = 20 + Random ( 0, 30 )
        SetSpriteSize ( index, size, size )
        SetSpriteShape ( index, 2 )
        SetSpritePhysicsAngularImpulse ( index, 10000 + Random ( 0, 350 ) )
        SetSpritePhysicsVelocity ( index, 100 + Random ( 0, 100 ), 100 + Random ( 0, 100 ) )
        SetSpriteColor ( index, Random ( 0, 255 ), Random ( 0, 255 ), Random ( 0, 255 ), 255 )
        SetSpritePhysicsRestitution ( index, 0.5 )

        // increment index and reset time
        index = index + 1
        time = 0
    endif

    // update the screen
    Sync ( )
loop
