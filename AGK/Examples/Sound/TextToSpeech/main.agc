// Text to Speech demo 
// Only works on iOS and Android devices

SetErrorMode(2)

// set window properties
SetWindowTitle( "TextToSpeech" )
SetWindowSize( 768, 1024, 0 )

// set display properties
SetVirtualResolution( 768, 1300 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 0, 0 ) 
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts
SetClearColor( 100,120,155 )
SetPrintSize(40)

// create an edit box to type in
CreateEditBox( 1 )
SetEditBoxSize( 1, 748,500 )
SetEditBoxPosition( 1, 10,100 )
SetEditBoxTextSize( 1, 80 )
SetEditBoxMultiLine( 1, 1 )
SetEditBoxText(1,"Welcome to the Text to Speech demo. Just touch Speak to here AppGameKit Studio talk! You can also edit the text and hear your own text speaking.")
SetEditBoxTextSize(1,50)
// add a button to speak the text
AddVirtualButton( 1, 690,50,100 )
SetVirtualButtonText( 1, "Speak" )

// setup text to speech on this device
TextToSpeechSetup()

do
	if( GetVirtualButtonPressed(1) )
		text$ = GetEditBoxText(1)
		// optionally set the language
		//SetSpeechLanguage("en_US") 
		Speak( text$ )
	endif
	
	// touch anywhere to stop
	if( GetPointerPressed() )
		StopSpeaking()
	endif
	
	Print( "Ready: " + str(GetTextToSpeechReady()) )
    Print( "Speaking: " + str(IsSpeaking()) )
    
    Sync()
loop
