
// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "CloudData" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

variableName as string = "MyVar"
counter as integer = 0

// initialise the cloud data process, prompts the user to sign in on Android
// parameter is reserved, must be an empty string
SetupCloudData("")

do
	if ( GetPointerPressed() )
		inc counter
		SetCloudDataVariable( variableName, str(counter) )
	endif
	
	if( GetCloudDataChanged() )
		// get the updated value, will equal -1 if the variable was deleted
		counter = val(GetCloudDataVariable( variableName, "-1" ))
	endif
	
	Print( "Tap to increase counter and save it to the cloud" )	
	Print( "The counter will update automatically if the cloud value changes" )	
	Print( "Cloud Status: " + str(GetCloudDataAllowed()) )
	Print( "Counter: " + str(counter) )
	
	Sync()
loop

