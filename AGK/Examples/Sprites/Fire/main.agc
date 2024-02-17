
// an example showing how to launch a ship


SetGenerateMipmaps ( 0 )

// set a resolution
SetVirtualResolution ( 640, 480 )

// nice fonts
UseNewDefaultFonts( 1 )

// load our main images
LoadImage ( 1, "MetalBlock1.png" )
LoadImage ( 2, "playership.png" )

// sort out a backdrop
backdrop = CreateSprite ( LoadImage ( "space.jpg" ) )
FixSpriteToScreen ( backdrop, 1 )

// launch pad
launchX# = 50
launchY# = 200

CreateSprite ( 1, 1 )
SetSpritePosition ( 1, launchX#, launchY# )
SetSpriteVisible ( 1, 0 )

// ship
CreateSprite ( 2, 2 )

// floor sprites
for j = 1 to 100
	i = CreateSprite ( 1 )
	SetSpritePosition ( i, j * 64, 400 )
	SetSpritePhysicsOn ( i, 1 )
next j

// fire status
fire = 0

// main loop
do
	// display instructions
	Print ( "Firing a ship" )
	Print ( "" )
	Print ( "Use arrow keys or the accelerometer to adjust" )
	Print ( "direction of the ship" )
	Print ( "Touch or click the screen to fire" )

	// set view offset to the ship
	SetViewOffset ( GetSpriteX ( 2 ) - 150, 0 )

	// allow movement and firing
	if fire = 0
		// update angle of ship and pad
		SetSpriteAngle ( 1, GetSpriteAngle ( 1 ) + GetDirectionX ( ) )
		SetSpriteAngle ( 2, GetSpriteAngle ( 1 ) - 90 )

		// work out the direction
		angle# = GetSpriteAngle ( 1 ) - 90

		x# = cos ( angle# )
		y# = sin ( angle# )

		newX# = launchX# + x#
		newY# = launchY# + y#

		// set the ships position
		SetSpritePosition ( 2, newX#, newY# )

		// fire on input
		if GetPointerPressed ( ) = 1
			force#     = 400.0
			velocityX# = ( newX# - launchX# ) * force#
            velocityY# = ( newY# - launchY# ) * force#

			SetSpritePhysicsOn ( 2, 2 )
			SetSpritePhysicsVelocity ( 2, velocityX#, velocityY# )
			SetSpritePhysicsAngularVelocity ( 2, 0.3 )
			SetSpritePhysicsDamping ( 2, 0.1 )
			fire = 1
		endif
	endif

	// update the screen
	Sync ( )
loop










