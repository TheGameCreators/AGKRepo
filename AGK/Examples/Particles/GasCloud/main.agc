
// gas cloud using particles

// this example program may run slower on certain platforms due
// to the number and size of particles being used

// set a virtual resolution
SetVirtualResolution ( 320, 480 )

// display a backdrop
CreateSprite ( LoadImage ( "alien_backdrop.jpg" ) )

// load particle image
LoadImage ( 1, "particle.png" )

// create the gas cloud
CreateParticles ( 1, 150, 250 )
SetParticlesFrequency ( 1, 500 )
SetParticlesLife ( 1, 3 )
SetParticlesSize ( 1, 64 )
SetParticlesStartZone ( 1, -2, 0, 2, 0 )
SetParticlesImage ( 1, 1 )
SetParticlesDirection ( 1, 25, 50 )
SetParticlesAngle ( 1, 360 )

AddParticlesForce ( 1, 2.5, 3.0, 0, 200 )

AddParticlesColorKeyFrame ( 1, 0.0, 0, 0, 0, 0 )
AddParticlesColorKeyFrame ( 1, 0.5, 0, 0, 0, 0 )
AddParticlesColorKeyFrame ( 1, 1.5, Random ( 0, 255 ), Random ( 0, 255 ), Random ( 0, 255 ), 255 )
AddParticlesColorKeyFrame ( 1, 3.0, Random ( 0, 255 ), Random ( 0, 255 ), Random ( 0, 255 ), 0 )

// main loop
do
    // update the screen
    sync ( )
loop
