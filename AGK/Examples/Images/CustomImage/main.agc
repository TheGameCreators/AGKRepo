
// Create an image, then save it
// Introduced in build 107

// display a background
backdrop = CreateSprite ( LoadImage ( "background5.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// nice fonts
UseNewDefaultFonts( 1 )

// allow render to draw current sprite to the backbuffer
Render()

// then we can grab the backbuffer (rendered screen)
grabbed = GetImage(5,5,80,80)

// now copy a sub set of that grabbed image to another image
image = CopyImage(grabbed,5,5,10,10)

// main loop
do
	// main code
	if GetSpriteExists(1)=0
	 SetImageMask(image,backdrop,1,1,0,0)
	 CreateSprite(1,image)
	 SetSpritePosition(1,10,10)
	 SetSpriteSize(1,50,-1)
    endif

    // click anywhere to save the new image
    if GetPointerPressed()=1 and saved=0
     SaveImage(image,"newimage.png")
     saved=1
    endif

    // update the screen
    Print("filename="+GetImageFilename(backdrop))
    if saved=1 then Print("newimage.png saved.")
    Sync ( )

loop
