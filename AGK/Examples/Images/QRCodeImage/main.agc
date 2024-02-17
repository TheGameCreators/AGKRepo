
// Encode and recode a QR Code Image

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background5.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// encode a QR Code (turn a string into a QR image)
mystring$="hello this is a string"
image=EncodeQRCode(mystring$,3)

// main loop
do
	// show code
	if GetSpriteExists(1)=0
	 CreateSprite(1,image)
	 SetSpritePosition(1,10,10)
	 SetSpriteSize(1,50,-1)
    endif

	// decoded back to string
	Print(DecodeQRCode(image))

    // update the screen
    Sync ( )
loop
