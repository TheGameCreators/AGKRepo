// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "Screen Recording" )
SetWindowSize( 768, 1024, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 768, 1024 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts
SetPrintSize( 50 )

// only needed if StartScreenRecording() microphone parameter is set to 1
if ( CheckPermission("RecordAudio") <> 2 )
	RequestPermission( "RecordAudio" )
	while( CheckPermission("RecordAudio") = 1 )
		Sync()
	endwhile
endif

// make some sprites to create something to record
for i = 1 to 10
	CreateSprite( i, 0 )
	SetSpriteSize( i, 75, 75 )
	SetSpriteColor( i, Random(30,255), Random(30,255), Random(30,255), 255 )
	SetSpritePositionByOffset( i, Random(100,668), Random(100,924) )
	SetSpriteAngle( i, Random(0,360) )
next i

do
	// spin the sprites
    for i = 1 to 10
		SetSpriteAngle( i, GetSpriteAngle(i)+1 )
	next i
	
	// start/stop recording
	if ( GetPointerPressed() )
		if ( IsScreenRecording() ) 
			StopScreenRecording()
			// video is saved locally, can prompt the user to share it
			ShareFile( "/ScreenRecording.mp4" )
		else
			StartScreenRecording( "/ScreenRecording.mp4", 0 )
		endif
	endif

	Print( "Press space to start/stop recording" )
	Print( "Is Recording: " + str(IsScreenRecording()) )
	Print( "Path: " + GetDocumentsPath() )
	Print( "FPS: " + str(ScreenFPS(),2) )
        
    Sync()
loop
