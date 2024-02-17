
// an example showing how to use include files

// nice fonts
UseNewDefaultFonts( 1 )

#include "functions.agc"

do
	Print ( "Calling a function from" )
	Print ( "an include file" )
	Print ( "" )

	myFunction ( 1, 2, 3 )

    Sync ( )
loop
