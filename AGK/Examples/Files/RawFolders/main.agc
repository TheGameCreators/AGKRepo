SetErrorMode(2)
 
// set window properties
SetWindowTitle( "RawFolders" )
SetWindowSize( 1024, 768, 0 )
SetWindowAllowResize( 1 ) // allow the user to resize the window
 
// set display properties
SetVirtualResolution( 1024, 768 ) // doesn't have to match the window
SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 30, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts

path as string
path = GetDocumentsPath()
folderID = OpenRawFolder( path )
folderCount = GetRawFolderNumFolders(folderID)
fileCount = GetRawFolderNumFiles(folderID)

do
	Print( "Path: " + path )
	Print( "Files: " + str(fileCount) )
	for i = 0 to fileCount-1
		Print( "    " + GetRawFolderFileName(folderID,i) )
	next i
	print( " " )
	Print( "Folders: " + str(folderCount) )
	for i = 0 to folderCount-1
		Print( "    " + GetRawFolderFolderName(folderID,i) )
	next i
	Sync()
loop
