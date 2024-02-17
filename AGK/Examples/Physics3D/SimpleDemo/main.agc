// set window properties
SetWindowTitle( "Simple 3D Physics" )
SetWindowSize( 1024, 768, 0 )

// nice fonts
UseNewDefaultFonts( 1 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetScissor(0,0,0,0)

Create3DPhysicsWorld()
Set3DPhysicsGravity(0,-1,0)

CreateObjectBox( 1, 10,10,10 )

Create3DPhysicsStaticBody( 1 )
SetObjectShapeBox(1)

CreateObjectBox( 2, 5,5,5 )
SetObjectPosition( 2, 5.1,30,0 )
Create3DPhysicsDynamicBody( 2 )
SetObjectShapeBox(2)

SetCameraPosition( 1, 0,30,-80 )
SetCameraLookAt( 1, 0,10,0, 0 )

do
	
	
	Print( ScreenFPS() )

	Step3DPhysicsWorld()
	Sync()
loop
