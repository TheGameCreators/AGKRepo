
// Project: Unicode 
// Created: 2016-12-09
// This file should be encoded in UTF-8

// nice fonts
UseNewDefaultFonts( 1 )

// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "Unicode" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
SetSyncRate( 60, 0 )
UseNewDefaultFonts( 1 ) // use new fonts that can display more characters

CreateText( 1, "Iñtërnâtiônàlizætiøn" )
SetTextPosition( 1, 512,300 )
SetTextSize( 1, 50 )
SetTextAlignment( 1, 1 )

do
    Print( ScreenFPS() )
    Sync()
loop
