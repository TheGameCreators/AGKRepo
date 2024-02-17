
// set window properties
SetWindowTitle( "AR Demo" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetScissor(0,0,0,0)
SetClearColor( 101,120,154 )
SetGenerateMipmaps( 1 )
UseNewDefaultFonts(1)
SetPrintSize(40)

permission$ = "Camera"
if ( CheckPermission( permission$ ) <= 0 )
	RequestPermission( permission$ )
	while( CheckPermission( permission$ ) = 1 )
			Sync()
	endwhile
endif

// camera range from 0.1 meters to 40 meters
SetCameraRange( 1, 0.1, 40 )
SetAmbientColor( 128,128,128 )
SetSunColor( 255,255,255 )

// load knight model
LoadObjectWithChildren( 1, "Knight.X" )
SetObjectPosition( 1, 0,0,0 )
LoadImage( 1, "Knight_color.jpg" )
SetObjectImage( 1, 1, 0 ) 

// texture sword and shield
LoadImage( 2, "weapons_color.jpg" )
SetObjectImage( GetObjectChildID(1,1), 2, 0 ) // shield
SetObjectImage( GetObjectChildID(1,2), 2, 0 ) // sword

function ShowModel( show as integer )
	SetObjectVisible( 1, show )
	SetObjectVisible( GetObjectChildID(1,1), show ) // shield
	SetObjectVisible( GetObjectChildID(1,2), show ) // sword
endfunction

ShowModel( 0 )

function ScaleModel( amount as float )
	SetObjectScalePermanent( 1, amount, amount, amount )
	SetObjectScalePermanent( GetObjectChildID(1,1), amount, amount, amount )
	SetObjectScalePermanent( GetObjectChildID(1,2), amount, amount, amount )
endfunction

ScaleModel( 0.01 )

// play idle animation from frames 10 to 90
PlayObjectAnimation( 1, "", 10, 90, 1, 0 )
SetObjectAnimationSpeed( 1, 20 )

// create some planes to show detected surfaces, initially hidden
for i = 101 to 150
	CreateObjectPlane( i, 1,1 )
	SetObjectRotation( i, 90,0,0 )
	FixObjectPivot( i )
	SetObjectVisible( i, 0 )
	SetObjectColor( i, 255,255,255,128 ) // 50% transparent
	SetObjectTransparency( i, 1 )
next i

// add some buttons to control various features
AddVirtualButton( 1, 100,465,200 )
AddVirtualButton( 2, 100,665,200 )
SetVirtualButtonText( 1, "Scale +" )
SetVirtualButtonText( 2, "Scale -" )

AddVirtualButton( 4, 924,665,200 )
SetVirtualButtonText( 4, "Hide" )

AddVirtualButton( 5, 305,465,200 )
AddVirtualButton( 6, 305,665,200 )
SetVirtualButtonText( 5, "Light +" )
SetVirtualButtonText( 6, "Light -" )

function ShowHUD( show as integer )
	SetVirtualButtonVisible( 1, show )
	SetVirtualButtonVisible( 2, show )
	SetVirtualButtonVisible( 4, show )
	SetVirtualButtonVisible( 5, show )
	SetVirtualButtonVisible( 6, show )
	SetVirtualButtonActive( 1, show )
	SetVirtualButtonActive( 2, show )
	SetVirtualButtonActive( 4, show )
	SetVirtualButtonActive( 5, show )
	SetVirtualButtonActive( 6, show )
endfunction

// initialize AR, if possible
ARSetup()
while( ARGetStatus() = 1 )
	// wait while user is being prompted to install ARCore
	Sync()
endwhile

AnchorID as integer = 0
ShowPlanes as integer = 1
ambientScale# = 1.0

do
	// get light estimation
	ambient = ARGetLightEstimate() * 255 * ambientScale#
	SetAmbientColor( ambient,ambient,ambient )
	
	// check screen tap for plane hits, but only if buttons are visible
	if ( GetPointerReleased() and ShowPlanes = 1 )
		// check the point that the user tapped on the screen
		numHits = ARHitTest( GetPointerX(), GetPointerY() )
		if ( numHits > 0 )
			ShowModel( 1 )
			// delete any previous anchor, could keep it around instead
			if ( AnchorID > 0 ) then ARDeleteAnchor( AnchorID )
			// hit test results are ordered from closest to furthest
			// place the object at result 1, the closest
			AnchorID = ARCreateAnchorFromHitTest( 1 )
			// fix the knight to this anchor
			ARFixObjectToAnchor( 1, AnchorID )
		else
			// if the user didn't tap on any planes then hide the object
			ShowModel( 0 )
		endif
		// clean up some internal resources
		ARHitTestFinish()
	endif
	
	// place the buttons at the edge of the screen
	// needs to be done regularly in case orientation changes
	SetVirtualButtonPosition( 1, GetScreenBoundsLeft()+105, GetScreenBoundsBottom()-310 )
	SetVirtualButtonPosition( 2, GetScreenBoundsLeft()+105, GetScreenBoundsBottom()-105 )
	SetVirtualButtonPosition( 4, GetScreenBoundsRight()-105, GetScreenBoundsBottom()-105 )
	SetVirtualButtonPosition( 5, GetScreenBoundsLeft()+310, GetScreenBoundsBottom()-310 )
	SetVirtualButtonPosition( 6, GetScreenBoundsLeft()+310, GetScreenBoundsBottom()-105 )
	
	// detect button presses if they are visible
	if ( ShowPlanes = 1 )
		if ( GetVirtualButtonPressed(1) )
			ScaleModel( 1.05 )
		endif
		if ( GetVirtualButtonPressed(2) )
			ScaleModel( 0.95 )
		endif
		if ( GetVirtualButtonPressed(4) )
			ShowPlanes = 1 - ShowPlanes
			ShowHUD( 0 )
		endif
		if ( GetVirtualButtonPressed(5) )
			inc ambientScale#, 0.1
		endif
		if ( GetVirtualButtonPressed(6) )
			dec ambientScale#, 0.1
		endif
	else
		// screen tap whilst button are hidden shows them again
		if ( GetPointerReleased() )
			ShowPlanes = 1 - ShowPlanes
			ShowHUD( 1 )
		endif
	endif
	
	// hide old planes
    for i = 101 to 150
		SetObjectVisible( i, 0 )
	next i
	
	// show detected planes
	if ( ShowPlanes )
		numPlanes = ARGetPlanes(0)
		// this demo stops at 50 planes, but there is no internal limit
		if numPlanes > 50 then numPlanes = 50
		for i = 1 to numPlanes
			SetObjectPosition( i+100, ARGetPlaneX(i), ARGetPlaneY(i), ARGetPlaneZ(i) )
			SetObjectRotation( i+100, ARGetPlaneAngleX(i), ARGetPlaneAngleY(i), ARGetPlaneAngleZ(i) )
			SetObjectScale( i+100, ARGetPlaneSizeX(i), 1, ARGetPlaneSizeZ(i) )
			SetObjectVisible( i+100, 1 )
		next i
		ARGetPlanesFinish()
	endif
    
    if ( ShowPlanes )
		Print( "FPS: " + str(ScreenFPS()) )
		select( ARGetStatus() )
			case 2 :  Print( "AR Active" ) : endcase
			case -1 :  Print( "AR Not Available" ) : endcase
			case -2 :  Print( "AR Install Rejected" ) : endcase
		endselect
		Print( "Number of Planes Detected: " + str(numPlanes) )
		Print( "Light Estimation: " + str(ARGetLightEstimate()) )
		Print( "Light Boost: " + str(ambientScale#,1) )
	endif
		
	// draw the camera feed, and then the rest of the scene
	ARDrawBackground()
    Sync()
loop
