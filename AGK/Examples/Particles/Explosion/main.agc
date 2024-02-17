
// explosions with particles

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a backdrop
CreateSprite ( LoadImage ( "alien_backdrop.jpg" ) )

// load an image for the particles
LoadImage ( 1, "shrapnel3.png" )

// create particles off screen
CreateParticles ( 1, -100, -100 )

// set fire variable to 1
fire = 1

// main loop
do
    // fire when ready
    if ( fire = 1 )
        // set up particles
        SetParticlesPosition ( 1, Random ( 100, 200 ), Random ( 100, 300 ) )

        ResetParticleCount ( 1 )
        SetParticlesFrequency ( 1, 250 )
        SetParticlesLife ( 1, 3.0 )
        SetParticlesSize ( 1, 64 )
        SetParticlesStartZone ( 1, -10, 0, 10, 0 )
        SetParticlesImage ( 1, 1 )
        SetParticlesDirection ( 1, 10, 10 )
        SetParticlesAngle ( 1, 360 )
        SetParticlesVelocityRange ( 1, 0.8, 2.5 )
        SetParticlesMax ( 1, 500 )

        AddParticlesColorKeyFrame ( 1, 0.0, 0, 0, 0, 0 )
        AddParticlesColorKeyFrame ( 1, 0.5, 255, 255, 0, 255 )
        AddParticlesColorKeyFrame ( 1, 2.8, 255, 0, 0, 0 )

        AddParticlesForce ( 1, 2.0, 2.8, 25, -25 )

        // reset fire value
        fire = 0
    endif

    // when the explosion has finished it is safe to fire once more
    if ( GetParticlesMaxReached ( 1 ) )
        fire = 1
    endif

    // update the screen
    sync ( )
loop
