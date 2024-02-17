// General Purpose Input Output - for The Raspberry Pi
// set window properties
SetWindowTitle( "GPIO" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetScissor(0,0,0,0)

// nice fonts
UseNewDefaultFonts( 1 )

// decalre some variables
inputPin as integer
outputPin as integer
outputValue as integer
time as float
stop as integer

// initialise the variables
inputPin = OpenToRead( "gpio:4" ) // GPIO pin 4
outputPin = OpenToWrite( "gpio:18" ) // GPIO pin 18
outputValue = 1
time = timer() + 3 // 3 seconds in the future
stop = 0

// set the initial output pin value
WriteByte( outputPin, outputValue )

// loop until the user pressees the escape key
while stop = 0
	
	// flip the output pin every 3 seconds
	if ( timer() > time )
		outputValue = 1 - outputValue // flip the output value
		WriteByte( outputPin, outputValue ) // set the pin value
		time = timer() + 3 // reset the time check to 3 seconds in the future
	endif
    
	Print( "Input: " + str(ReadByte(inputPin)) )
    Print( "FPS: " + str(ScreenFPS()) )
    Sync()
    
    // check for escape key and exit
    if ( GetRawKeyPressed( 27 ) ) then stop = 1
endwhile

// close the GPIO pins, this will only be reached if the user presses escape.
// if the window is closed this will not be reached, but AGK will close the pins for you
CloseFile( inputPin )
CloseFile( outputPin )
