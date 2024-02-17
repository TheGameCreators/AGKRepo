// Project: Particles and Arrays 
// Created: 2017-02-08

// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "Particles and Arrays" )
SetWindowSize( 1024, 768, 0 )
SetScissor(0,0,0,0)

// for our exit button sprite ID
global exit_button as integer

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetSyncRate( 30, 0 ) // 30fps to save battery
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

// Create an integer array 10x3 to store the blob x and y co-ordinates & circle radius
blobs as integer [10,3]

// Create 10 sets of random x,y positions and random radius
for x=1 to 10
	blobs[x,1]=random(100,850)
	blobs[x,2]=random(100,600)
	blobs[x,3]=random(20,200)
next x

//Load Paticle Image
LoadImage ( 2, "particle.png" )

//Create 10 sprites & 10 particles
CreateImageColor(1,255,0,0,255)
for x=1 to 10 
	CreateSprite(x,1)
	SetSpriteSize(x,10,10)
	SetSpriteVisible(x,0)
	CreateParticles(x,0,0 )
	SetParticlesImage( x, 2 )

	AddParticlesColorKeyFrame ( x, 1, 255, 0, 0, 255 )
	AddParticlesColorKeyFrame ( x, 2, 0,255, 0, 255 )
	AddParticlesColorKeyFrame ( x, 3, 0, 0,255, 255 )
	AddParticlesColorKeyFrame ( x, 6, 0, 0,0, 0 )

next x


// Rotate each blob around a point 
do


for d=1 to 360 step 1
	for i=1 to 10
		x=sin(d)*blobs[i,3]
		y=cos(d)*blobs[i,3]

		SetSpritePosition(i,blobs[i,1]+x,blobs[i,2]+y)

		SetParticlesPosition(i,blobs[i,1]+x,blobs[i,2]+y)
		
		SetParticlesStartZone     ( i, 0, 0, 0, 0 )
		SetParticlesDirection     ( i, sin(d)*10, cos(d)*10 )
		SetParticlesLife          ( i, 6 )
		SetParticlesSize          ( i, 20 )
		SetParticlesAngle         ( i, 15 )
		SetParticlesFrequency     ( i, 55 )
		SetParticlesVelocityRange ( i, 1, 4 )
		
	next i
//	if dw# <> GetDeviceWidth() then maketext()
	//check_exit()
	Sync()
Next d

loop




