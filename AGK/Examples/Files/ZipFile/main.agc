
// Create a zip file and extract from it
// Introduced in build 107

// display a background
backdrop = CreateSprite ( LoadImage ( "background2.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

//Use ttf Fonts
UseNewDefaultFonts( 1 )

// init code
SetCurrentDir("")
stage=1

// main loop
do
	// main code
	select stage
 	 case 1:
      CreateZip(1,"myzip.zip")
      Print("Creating zip..")
      stage=2
     endcase
 	 case 2:
      AddZipEntry(1,"media\background5.jpg","internalname.jpg")
      Print("Adding to zip file..")
      stage=3
     endcase
 	 case 3:
      CloseZip(1)
      Print("Closing zip file.")
      stage=4
     endcase
 	 case 4:
      MakeFolder("output")
      ExtractZip("myzip.zip","output")
      Print("Extract the file as new file.")
      stage=5
     endcase
    endselect

    // update the screen
    Sleep(1000)
    Sync()
loop
