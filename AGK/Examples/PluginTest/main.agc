#import_plugin ExamplePlugin as EP

// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "PluginTest" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

f as float
f = EP.AddFloat( 5, 7 )

s as string = "Hello World"
s = EP.ModifyString( s )

EP.SetI( 11 )

EP.CreateRedSquare()

do
    Print( f )
    EP.DLLPrint( s )
    Print( EP.GetI() )

    Sync()
loop
