
// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "UDP" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts


CreateUDPListener( 1, "anyip4", 46789 )
lastmsgID = 0

do
	if ( GetPointerPressed() )
		msg = CreateNetworkMessage()
		AddNetworkMessageInteger( msg, 1234 )
		SendUDPNetworkMessage( 1, msg, "192.168.0.123", 45000 )
	endif
	
	msgID = GetUDPNetworkMessage( 1 )
	if ( msgID )
		// read message data
		DeleteNetworkMessage( msgID )
		lastmsgID = msgID
	endif
	
	Print( lastmsgID )
    Sync()
loop
