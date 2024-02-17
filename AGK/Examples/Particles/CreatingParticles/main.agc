
// particles

// virtual resolution
SetVirtualResolution ( 320, 480 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background1.jpg" ) )
SetSpriteColorAlpha ( backdrop, 100 )

// load image for particles
LoadImage ( 1, "particle.png" )

// create a block of particles and set properties
CreateParticles           ( 1, 150, 10 )
SetParticlesImage         ( 1, 1 )
SetParticlesStartZone     ( 1, -100, -10, 100, 10 )
SetParticlesDirection     ( 1, 0, 50.0 )
SetParticlesLife          ( 1, 9 )
SetParticlesSize          ( 1, 24 )
SetParticlesAngle         ( 1, 15 )
SetParticlesFrequency     ( 1, 60 )
SetParticlesVelocityRange ( 1, 1, 4 )
AddParticlesColorKeyFrame ( 1, 0, 0, 100, 255, 0 )
AddParticlesColorKeyFrame ( 1, 0.5, 0, 100, 255, 255 )
AddParticlesColorKeyFrame ( 1, 8.0, 150, 50, 100, 255 )
AddParticlesColorKeyFrame ( 1, 9.0, 0, 0, 0, 0 )

// main loop
do
    // update the screen
    Sync ( )
loop
