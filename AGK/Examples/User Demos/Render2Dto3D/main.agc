
// Project: Render2Dto3D by baxslash

// set window properties
#constant DeviceWidth GetDeviceWidth()
#constant DeviceHeight GetDeviceHeight()
SetWindowTitle( "Render2Dto3D" )
SetScreenResolution( DeviceWidth, DeviceHeight )					`Set the size of the application window
SetVirtualResolution( DeviceWidth, DeviceHeight )					`Set the "virtual" screen size
SetPrintSize( 24 )

// create a render image to render 2D scene onto
renderImage = CreateRenderImage( 1024, 512, 0, 0 )					`Grab the render image
																	`The resolution rendered depends on the size of this image
// create a 3D box to draw to
box = CreateObjectPlane( DeviceWidth, DeviceHeight )				`Create a plane object to apply the rendered image to
SetObjectPosition( box, 0, DeviceHeight*0.5, 0 )					`Set it's position
SetObjectImage( box, renderImage, 0 )								`Apply the render image to the object (no need to re-apply it)

// set the 3D camera position
cx# = sin(30)*DeviceHeight*2
cy# = DeviceHeight*1.25
cz# = cos(30)*DeviceHeight*1.5
SetCameraPosition( 1, cx#, cy#, cz# )
SetCameraLookAt( 1, 0, DeviceHeight*0.5, 0, 0 )
SetCameraRange( 1, 1, DeviceHeight*4 )

// create some Physics Sprites
// first set the physics scale
SetPhysicsScale( 0.01 )
for i=1 to 100
	spr = CreateSprite( 0 )
	SetSpriteSize( spr, Random( 20, 40 ), Random( 20, 40 ) )
	SetSpritePositionByOffset( spr, Random( 0, DeviceWidth ), Random( 0, DeviceHeight ) )
	SetSpritePhysicsOn( spr, 2 )
next

// main game loop
do
	Print( "Rotate the 3D object by tilting your device or pressing Left/Right" )
	
	// Get a value to represent how much the device is tilting in the X direction
	dirx# = GetDirectionX() * 90.0
	dx# = dirx# - moveX#
	moveX# = moveX# + (dx#*0.1)
	
	// Apply the mouse movement to the object
	SetObjectRotation( box, 0, 0, moveX# )
	
	// Apply the object angle to the physics scene in 2D
	SetPhysicsGravity( Sin( moveX# )*DeviceWidth, Cos( moveX# )*DeviceWidth )
	
    // First Render 2D scene to an image
    depthImage = -1 								`not required as we are not rendering depth, just a 2D scene
    SetRenderToImage( renderImage, depthImage )		`This tells AGK to render only to the render image applied to the box as a texture
	SetClearColor( 128, 128, 128 )					`Setting the clear / background colour for the 2D render
    ClearScreen()									`Clear the render image before rendering
    Update2D( GetFrameTime() )						`Update 2D scene (including physics)
    Render2DFront()									`Render only 2D front items (in this case just our skeleton)
    
    // Now Render 3D
    SetRenderToScreen()								`This tells AGK to render back to the screen
	SetClearColor( 0, 0, 0 )						`Setting the clear colour for the final 3D render to white
    Update3D( GetFrameTime() )						`Update 3D objects only
    Render3D()										`Render only 3D objects (our box)
    Swap()											`Send the final render to the screen
    ClearScreen()									`Clear the screen ready for the next loop
loop

end
