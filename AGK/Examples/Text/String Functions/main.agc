// nice fonts
UseNewDefaultFonts( 1 )


// set window properties
SetWindowTitle( "String Functions" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetPrintSize(36)

mystr as string = "Hello World"
mystr1 as string, mystr2 as string, mystr3 as string, mystr4 as string
findstr as string = "o"

count = FindStringCount( mystr, findstr )
pos = FindString( mystr, findstr )
revpos = FindStringReverse( mystr, findstr )

mystr1 = ReplaceString( mystr, "lo", "d", -1 )
mystr2 = StripString( mystr1, "l" )
mystr3 = TrimString( mystr2, "d" )
mystr4 = TruncateString( mystr3, " " )

do
	// single quotes can be used to display double quotes in the string
    print( 'Using the string "' + mystr + '"' )
    Print( 'It has ' + str(count) + ' instances of "' + findstr +'" within it' )
    print( 'The first instance is at position ' + str(pos) )
    print( 'The last instance is at position ' + str(revpos) )
	print( '' )
	print( 'Replacing all instances of "lo" with "d": "' + mystr1 +'"' )
	print( 'Stripping all instances of "l": "' + mystr2 +'"' )
	print( 'Triming all instances of "d" from both ends: "' + mystr3 +'"' )
	print( 'Truncating everything after the last space: "' + mystr4 +'"' )

    Sync()
loop
