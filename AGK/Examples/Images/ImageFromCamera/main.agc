
// Capture image from camera and show it

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background5.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// get camera image
if GetCameraExists()=1
 if ShowImageCaptureScreen()=1
  while IsCapturingImage()=1
   Sync()
  endwhile
  image=GetCapturedImage()
 endif
endif

// main loop
do
	// show image here
	if GetSpriteExists(1)=0
	 CreateSprite(1,image)
	 SetSpritePosition(1,10,10)
	 SetSpriteSize(1,80,-1)
    endif

    // update the screen
    Print("Result of capture..")
    Sync ( )
loop
