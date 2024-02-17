//Physical joystick demo 

// nice fonts
UseNewDefaultFonts( 1 )

// set window properties
SetWindowTitle( "Raw Joysticks" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )

SetPrintSize( 25 )

do
	// find out which joysticks have been detected
	printC( "Exists:    " )
	printC(GetRawJoystickExists(1)) : PrintC( ", " )
	printC(GetRawJoystickExists(2)) : PrintC( ", " )
	printC(GetRawJoystickExists(3)) : PrintC( ", " )
	print(GetRawJoystickExists(4))
	
	// find out which of those are connected
	printC( "Connected: " )
	printC(GetRawJoystickConnected(1)) : PrintC( ", " )
	printC(GetRawJoystickConnected(2)) : PrintC( ", " )
	printC(GetRawJoystickConnected(3)) : PrintC( ", " )
	print(GetRawJoystickConnected(4))
	
	// print the details of joystick 1 if it exists
	if GetRawJoystickExists(1)
		// left analog stick
		Print( "X1: " + str(GetRawJoystickX(1)) )
		Print( "Y1: " + str(GetRawJoystickY(1)) )
		Print( "Z1: " + str(GetRawJoystickZ(1)) )
		
		// right analog stick
		Print( "X2: " + str(GetRawJoystickRX(1)) )
		Print( "Y2: " + str(GetRawJoystickRY(1)) )
		Print( "Z2: " + str(GetRawJoystickRZ(1)) )
		
		// AGK supports up to 32 buttons, but most joysticks don't go beyond 16
		for i = 1 to 16
			PrintC( "Button " + str(i) + ": " )
			Print( GetRawJoystickButtonState( 1, i ) )
		next i
	endif
	
    Sync()
loop
