SetErrorMode(2) 

SetWindowSize(1024,768,0)
SetWindowAllowResize(1)
SetSyncRate( 0, 0 )


SetVirtualResolution(1024,768)
SetClearColor( 101,120,154 )
SetScissor(0,0,0,0)
SetPrintSize( 25 )
UseNewDefaultFonts(1)

for i = 1 to 1000
	CreateObjectBox( i, Random(2,10), Random(2,10), Random(2,10) )
	SetObjectPosition( i, Random(0,100)-50, Random(0,100)-50, Random(50,150) )
	SetObjectRotation( i, random(0,360), random(0,360), random(0,360) )
next i

SetCameraPosition( 1, 0,0,-50 )
SetCameraLookAt( 1, 0,0,0, 0 )
SetCameraRange( 1, 0.1, 1000 )

do    
	Print( ScreenFPS() )
	Print( GetUpdateTime() )  
	Print( GetDrawingSetupTime() )  
	Print( GetDrawingTime() )
    Sync()
loop
