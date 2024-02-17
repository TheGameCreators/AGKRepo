
// show all errors
SetErrorMode(2)

// set window properties
SetWindowTitle( "JSONArraysTypes" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window

// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts
SetPrintSize( 20 )

type spritetype
	ID as integer
	x as float
	y as float
	width as float
	height as float
endtype

type scenetype
	name as string
	ID as integer
	time as float
	sprites as spritetype[1]
	indices as integer[5]
endtype

// create an array
MyArray as integer[5]
MyArray = [10, 53, 2, 678, 3, 2]

// convert it to a JSON string
MyArrayJSON as string
MyArrayJSON = MyArray.toJSON()

// save it as a JSON file
MyArray.save( "MyArray.json" )

// create a string array and populate it from a JSON file
MyArrayString as string[]
MyArrayString.load( "arrayString.txt" )

// create a type
MyScene as scenetype

// populate it from a JSON string, the string contains an extra field "user" 
// that will be ignored, and is missing the indices field which will be zeroed
MyScene.fromJSON( '{"name": "scene1", "ID": 1, "time": 5.674, "sprites": [{ "ID": 1, "x": 89.5, "y": 56.8, "width": 50, "height": 50 }, { "ID": 2, "x": 24, "y": 12.5, "width": 20, "height": 30 }, { "ID": 3, "x": 65.2, "y": 78.1, "width": 40, "height": 20 }], "user": { "name": "bob", "ID": 4 }}' )

// convert it to a string to check it
MySceneJSON as string
MySceneJSON = MyScene.toJSON()

do
    
	Print( "MyArray = " + MyArrayJSON )
	Print( "MyArrayString[0] = " + MyArrayString[0] )
	Print( "MyScene.name = " + MyScene.name )
	Print( "MyScene = " + chr(10) + MySceneJSON )
    Sync()
loop
