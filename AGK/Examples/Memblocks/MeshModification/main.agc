// Memblock demo

// nice fonts
UseNewDefaultFonts( 1 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetScissor(0,0,0,0)

SetCameraPosition( 1, 0,20,-50 )
SetCameraLookAt( 1, 0,0,0, 0 )

// create object and dump its mesh data into a memblock so we can modify it
CreateObjectBox( 1, 10,10,10 )
CreateMemblockFromObjectMesh( 1, 1, 1 )

do
	// normal vertex 0 position is -5,5,-5
	posx# = sin( timer()*70 ) * 2 - 5
	posy# = cos( timer()*70 ) * 2 + 5
	posz# = -5
	
	// note that an AGK cube is made up of 6 separate faces, so moving one vertex 
	// only affects a single face and will create a hole in the mesh unless the other 
	// vertices belonging to the other faces are moved in the same way
	SetMeshMemblockVertexPosition( 1, 0, posx#,posy#,posz# )
	
	// copy the memblock data back into the object mesh
	// if your mesh contains a lot of vertices then this could be slow, 
	// be sure to test it on the lowest spec device you target
	SetObjectMeshFromMemblock( 1, 1, 1 )
	
	Print( ScreenFPS() )
	Sync()
loop
