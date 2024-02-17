
SetErrorMode(2)

// set window properties
SetWindowTitle( "Display Cutouts" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // enable drawing outside the chosen virtual resolution
UseNewDefaultFonts( 1 )

global oldWidth as integer = 0
global oldHeight as integer = 0

global numCutouts as integer
global safeSpriteID as integer = 0
global cutoutSpriteIDs as integer[]

do
	// if the screen size changes then update everything
	// oldWidth and oldHeight start at 0 to trigger this on the first loop
	if ( oldWidth <> GetDeviceWidth() or oldHeight <> GetDeviceHeight() )
		numCutouts = GetDisplayNumCutouts()
		for i = 0 to numCutouts-1
			top# = GetDisplayCutoutTop(0)
			bottom# = GetDisplayCutoutBottom(0)
			left# = GetDisplayCutoutLeft(0)
			right# = GetDisplayCutoutRight(0)
			
			if ( i > cutoutSpriteIDs.length ) then cutoutSpriteIDs.insert( CreateSprite(0) )
			SetSpriteSize( cutoutSpriteIDs[i], right# - left#, bottom# - top# )
			SetSpritePositionByOffset( cutoutSpriteIDs[i], (left#+right#)/2, (top#+bottom#)/2 )
			SetSpriteColor( cutoutSpriteIDs[i], 255,0,0,255 )
		next i

		// get safe area that avoids all cutouts
		// these values could be outside the chosen virtual resolution
		// e.g. the top of the screen may be at Y = -200
		// in this case you can safely place items at these negative values
		// but note that this value will be different for every device
		safeTop# = GetScreenBoundsSafeTop()
		safeBottom# = GetScreenBoundsSafeBottom()
		safeLeft# = GetScreenBoundsSafeLeft()
		safeRight# = GetScreenBoundsSafeRight()
		
		if ( safeSpriteID = 0 ) then safeSpriteID = CreateSprite( 0 )
		SetSpritePositionByOffset( safeSpriteID, (safeLeft#+safeRight#)/2, (safeTop#+safeBottom#)/2 )
		SetSpriteSize( safeSpriteID, safeRight# - safeLeft#, safeBottom# - safeTop# )
		SetSpriteColor( safeSpriteID, 0, 128, 0, 255 )
		
		oldWidth = GetDeviceWidth()
		oldHeight = GetDeviceHeight()
	endif
	
	Print( ScreenFPS() )
	Print( "Num Cutouts: " + str(numCutouts) )
	Print( "Display cutouts highlighted in red" )
	Print( "Safe area highlighted in green" )
	Sync()
loop
