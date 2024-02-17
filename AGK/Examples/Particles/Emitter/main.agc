
// creating an emitter

// set the virtual resolution
SetVirtualResolution ( 320, 480 )

// display a backdrop
CreateSprite ( LoadImage ( "alien_backdrop.jpg" ) )

// load an image for the particles
LoadImage ( 1, "test2.png" )

// create the particles
CreateParticles ( 1, 150, 250 )
SetParticlesImage ( 1, 1 )
SetParticlesStartZone ( 1, -5, 0, 5, 0 )
SetParticlesDirection ( 1, 0, -145 )
SetParticlesAngle ( 1, 15 )
SetParticlesFrequency ( 1, 60 )
SetParticlesLife ( 1, 1.5 )
SetParticlesSize ( 1, 16 )
AddParticlesColorKeyFrame ( 1, 0, 255, 255, 255, 255 )
AddParticlesColorKeyFrame ( 1, 1.5, 255, 255, 255, 0 )

// main loop
do
    // update the screen
    Sync ( )
loop
