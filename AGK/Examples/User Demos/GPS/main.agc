
// Project: GeoCompass by JammySoft

// set window properties
SetWindowTitle( "GeoCopass" )
// set display properties
SetWindowSize( 400,640, 0 ) `this sets the window size on a PC
Aspect#=0.625 
SetOrientationAllowed( 1, 0, 0,0 ) ` For simplicity dont allow screen rotation
SetDisplayAspect( Aspect# ) `the aspect ratio of my nexus 7

Gosub MakeButtons
Gosub DeviceCheck `Check Device Abilities
Gosub MakeSpritesAndText `Make Sprites and Text

` set some variables
Maptype=1
maptype$="hybrid"
Zoom=14
global Latitude#=0 
global Longitude#=0
Dim AveAngleY# [100]
Dim AveAngleX# [100]
count as integer
tune#=-90.0 ` I think my calculations were 90 degrees out - this sorts it
scale=60

Gosub MakeCompass
Gosub MakeSpiritLevel

`Main Loop

do
	if SensorAvailable=1 `get magnetic sensor readings
	
		MagX# = GetRawMagneticX() 
		MagY# = GetRawMagneticY() 
		`MagZ# = GetRawMagneticZ()  - Not used in this example but could be used to make a 3D compass
	
		` Work out angle and turn Compass
		angle#= ATanFull( MagY#, MagX# ) 
		resultangle#=angle#+tune#
		SetSpriteAngle(CompassSprite,resultangle#)
	
	endif				
	
	if AccelSensor=1`  Get Accelerometer Sensor readings 
		DevRotX# = GetRawAccelX()*Scale 
		DevRotY# = (GetRawAccely() *0.625)*scale
		DevRotZ# = GetRawAccelz() *scale

		`Work out an Average of Accelerometer readings to smooth movement
		count=count+1
		if count=61 then count=1
		AveAngleY#[count]=DevRotY#
		AveAngleX#[count]=DevRotX#
		totangleY#=0
		totangleX#=0
		
		for temp=1 to 60
			totangleY#=totangleY#+ AveAngleY#[temp]
			totangleX#=totangleX#+ AveAngleX#[temp]
		next temp

		resultangleY#=totangleY#/120
		resultangleX#=totangleX#/120

		`Position ball spirit level
		SetSpritePositionByOffset(BallSprite,85+resultangleX#,66+resultangleY#*0.625)
	Endif
		
		`Check GPS Buttons
	if GetPointerPressed() =1
		x#=Getpointerx()
		y#=GetPointerY()
		ButTap=GetSpriteHit( x#, y# ) 
	
		`Zoom Level
		if ButTap = ZoomSprite
			Zoom=Zoom+1
			If Zoom>20 then Zoom=1
			SetTextString(5,str(Zoom))
		endif
		
		`Map Type
		If ButTap=(MaptypeSprite)
			Maptype=Maptype+1
			if MapType>4 then Maptype=1
			if Maptype=1 then maptype$="hybrid"
			if Maptype=2 then maptype$="roadmap"
			if Maptype=3 then maptype$="satellite"
			if Maptype=4 then maptype$="terrain"
			SetTextString(4,maptype$)
		endif
		
		if GPSexists = 1 and Internet =1		
			If ButTap=(GetGPSSprite)
				Gosub getGPS
				Gosub GetMap
				Gosub MakeCompass
			endif
		endif
	endif

`Check Exit button
	if GetVirtualButtonPressed( 1 ) =1 then end
	If GetVirtualButtonPressed( 2 ) =1 then Gosub Information
	Sync()
loop


Information:
	
	Repeat
		sync()
	until GetVirtualButtonReleased( 2 ) =1 `Check virtual button is let go

	`Hide Main Screen

	SetSpriteVisible(CompassSprite,0)
	SetSpriteVisible(MaptypeSprite,0)
	SetSpriteVisible(CompassSprite,0)
	SetSpriteVisible(ZoomSprite,0)
	SetSpriteVisible(GetGPSSprite,0)
	SetSpriteVisible(TargetSprite,0)
	SetSpriteVisible(BallSprite,0)
	
	for temp=1 to 5
		SetTextVisible(temp,0)
	next temp

	SetVirtualButtonImageUp(2,backimage1) `change info to back image
	SetVirtualButtonImageDown(2,backimage2)
	
	repeat
		Print("")
		Print("         Information")
		Print("")
		Print("    GeoCompass by Jammysoft")
		Print("")
		Print("For the compass to be accurate")
		Print("your device has to be flat.")
		Print("Use the spirit level and keep")
		Print("the ball within its target.")
		Print("")
		Print("The Gps map has two options,")
		Print("")
		Print("1, Map type")
		Print("these are roadmap, hybrid, ")
		Print("satellite and terrain")
		Print("")
		Print("2, Zoom Level")
		Print("Choose level 1 - 20")
		Print("1 - is the whole world.")
		Print("20 - is zoomed right in to your")
		Print("location.")
		Print("")
		Print("Google only allow 1000 Map")
		Print("requests per IP address per day")
		Print("and 50 Maps per minute.")
		Print("")
		Print("Enjoy,  Jammy")
	
		if GetVirtualButtonReleased( 1 ) =1 then end
		Sync()
	until GetVirtualButtonReleased( 2 ) =1
	
	`reset the main screen
	SetSpriteVisible(CompassSprite,1)
	SetSpriteVisible(MaptypeSprite,1)
	SetSpriteVisible(CompassSprite,1)
	SetSpriteVisible(ZoomSprite,1)
	SetSpriteVisible(GetGPSSprite,1)
	SetSpriteVisible(TargetSprite,1)
	SetSpriteVisible(BallSprite,1)
	
	for temp=1 to 5
		SetTextVisible(temp,1)
	next temp

	SetVirtualButtonImageUp( 2, InfoUpImage ) 
	SetVirtualButtonImageDown( 2, InfoDownImage) 
	
return		


getGPS:
	
	Latitude#=1000
	Longitude#=1000
	SetSpriteVisible(WorkingSprite,1)
	StartGPSTracking() `start the gps system
		repeat
			Latitude#=GetRawGPSLatitude() 
			Longitude#=GetRawGPSLongitude() 
			Gosub Work
		until Latitude#<>1000 and Longitude#<>1000 ` sometimes my Desktop can give funny answeres
		if Latitude#>90 or Latitude#<-90  then Latitude#=0 `this keeps it within normality
		if Longitude#>180 or Longitude#<-180 then Longitude#=0
	StopGPSTracking() 
	
	
Return




GetMap:

remstart

Taken From 

https://developers.google.com/maps/documentation/staticmaps/index#MapTypes

Map Parameters

size (required) defines the rectangular dimensions of the map image. This parameter takes a string of the form {horizontal_value}x{vertical_value}. For example, 500x400 defines a map 500 pixels wide by 400 pixels high. Maps smaller than 180 pixels in width will display a reduced-size Google logo. This parameter is affected by the scale parameter, described below; the final output size is the product of the size and scale values.
scale (optional) affects the number of pixels that are returned. scale=2 returns twice as many pixels as scale=1 while retaining the same coverage area and level of detail (i.e. the contents of the map don't change). This is useful when developing for high-resolution displays, or when generating a map for printing. The default value is 1. Accepted values are 2 and 4 (4 is only available to Google Maps API for Work customers.) See Scale Values for more information.
format (optional) defines the format of the resulting image. By default, the Static Maps API creates PNG images. There are several possible formats including GIF, JPEG and PNG types. Which format you use depends on how you intend to present the image. JPEG typically provides greater compression, while GIF and PNG provide greater detail. For more information, see Image Formats.
maptype (optional) defines the type of map to construct. There are several possible maptype values, including roadmap, satellite, hybrid, and terrain. For more information, see Static Maps API Maptypes below. 
language (optional) defines the language to use for display of labels on map tiles. Note that this parameter is only supported for some country tiles; if the specific language requested is not supported for the tile set, then the default language for that tileset will be used.
region (optional) defines the appropriate borders to display, based on geo-political sensitivities. Accepts a region code specified as a two-character ccTLD ('top-level domain') value.
maptypes, roadmap, hybrid,satellite,terrain
 remend
 
	Size$="512x512"
	Zoom$=str(Zoom)
	Scale$="1"
	`maptype$="hybrid"
	format$="png"
	
	`this just makes for nicer maps
	If zoom=1
		Latitude# = 0
		Longitude# = 0
    endif
    
    if Latitude# = 0 and Latitude# = 0
		if zoom > 5 then Zoom$="1"
	endif
    
    iHTTP= CreateHTTPConnection() `create a connection 
    SetHTTPHost(iHTTP, "maps.googleapis.com", 0 ) ` set who you are connecting to
    `this is the format google maps uses for filenames
    szServerFile$="maps/api/staticmap?center="+str(Latitude#)+","+str(Longitude#)+"&zoom="+Zoom$+"&scale"+Scale$+"&format="+format$+"&size="+Size$+"&maptype="+maptype$
	
    file=GetHTTPFile(iHTTP, szServerFile$,"Map.png", "" ) `get the file
    
    Downloaded=0
    repeat
		Gosub Work ` makes a flower turn while we are waiting
        Downloaded=GetHTTPFileComplete(iHTTP)
        Sync()
    Until Downloaded<>0

    CloseHTTPConnection( iHTTP ) ` close the connection

    DeleteHTTPConnection( iHTTP ) `tidy up
	
	SetSpriteVisible(WorkingSprite,0)
 Return
   
 MakeCompass:
	
	SetVirtualButtonVisible(1,0) ` hide things in the way of a screen grab
	SetVirtualButtonVisible(2,0)
	
	SetSpriteVisible(TargetSprite, 0 ) 
	SetSpriteVisible(BallSprite, 0 ) 
	
	if GetSpriteExists(CompassSprite)=1 then DeleteSprite(CompassSprite)
    
    If GetFileExists("map.png")=0 `load the default map if no map has ever been downloaded
		MapSprite=LoadSprite("DefaultMap.png")
	else
		MapSprite=LoadSprite("map.png") `if one is there use the map found
    endif
    
    setspritesize(MapSprite,100,-1) ` set map properties
    setspriteposition(MapSprite,0,0)
    
	RingSprite=LoadSprite("ring4.png") ` position a frame for the map
	setspritesize(RingSprite,100,-1)
    setspriteposition(RingSprite,0,0)
    
    North=CreateText("N")  ` Put text on the Frame
    SetTextAlignment(North,1)
    SetTextPosition(North,50,-0.5)
    South=CreateText("S")
    SetTextAlignment(South,1)
    SetTextPosition(South,50,95*Aspect#)
    East=CreateText ("E")
    SetTextAlignment(East,1)
    SetTextPosition(East,97.8,(50*Aspect#)-2.5)
    West=CreateText ("W")
    SetTextAlignment(West,1)
    SetTextPosition(West,2.2,(50*Aspect#)-2.5)
    
    Render() ` Draw the images to the buffer not the screen
    
    CompassImage=GetImage(0,0,100,100*Aspect#) ` grab the image
    
    ClearScreen() `clean up
    DeleteText(North)
    DeleteText(South)
    DeleteText(East)
    DeleteText(West)
    DeleteSprite(MapSprite)
    DeleteSprite(RingSprite)
    
    CompassSprite=CreateSprite(CompassImage) `make the compass sprite from our new image
	setspritesize(CompassSprite,100,-1)
    SetSpritePositionByOffset(CompassSprite,50,50*Aspect#)
    
    SetVirtualButtonVisible(1,1) `reset the screen
	SetVirtualButtonVisible(2,1)
	SetSpriteVisible(TargetSprite, 1 ) 
	SetSpriteVisible(BallSprite, 1 ) 
    
Return

MakeSpiritLevel:
	
	TargetSprite=LoadSprite("Target.png")
	setspritesize(TargetSprite,25,-1)
    SetSpritePositionByOffset(TargetSprite,85,66)
	SetSpriteDepth ( TargetSprite, 9 ) 
	BallSprite=LoadSprite("Ball.png")
	setspritesize(BallSprite,15,-1)
    SetSpritePositionByOffset(BallSprite,85,66)
	SetSpriteDepth ( BallSprite, 8 )
Return
	

MakeButtons:

	ButDownImage=loadImage("Power.png")
	ButUpImage=loadImage("Power2.png")
	AddVirtualButton( 1, 92, 8*Aspect#, 15 ) 
	SetVirtualButtonImageUp( 1, ButUpImage ) 
	SetVirtualButtonImageDown( 1, ButDownImage)
 
	BackImage1=loadImage("Back1.png")
	BackImage2=loadImage("Back2.png")
	InfoDownImage=loadImage("Info2.png")
	InfoUpImage=loadImage("Info1.png")
	AddVirtualButton( 2, 8, 8*Aspect#, 15 ) 
	SetVirtualButtonImageUp( 2, InfoUpImage ) 
	SetVirtualButtonImageDown( 2, InfoDownImage) 

Return	

MakeSpritesAndText:


	CreateText(1,"For compass to be")
	settextsize(1,3.5)
	SetTextColor(1,255,255,155,255)
	settextposition(1,3,63)
	
	CreateText(2,"accurate. Keep the")
	settextsize(2,3.5)
	SetTextColor(2,255,255,155,255)
	settextposition(2,3,66)

	CreateText(3,"ball in the centre.")
	settextsize(3,3.5)
	SetTextColor(3,255,255,155,255)
	settextposition(3,3,69)
	
	CreateText(4,"hybrid")
	settextsize(4,5)
	SetTextColor(4,255,255,155,255)
	settextposition(4,50,75)
	
	CreateText(5,"14")
	settextsize(5,5)
	SetTextColor(5,255,255,155,255)
	settextposition(5,50,82)
	
	MaptypeSprite=LoadSprite("MapType.png")
	SetSpriteSize(MaptypeSprite,32,-1)
	SetSpritePosition(MaptypeSprite,8,75)
	
	ZoomSprite=LoadSprite("Zoom.png")
	SetSpriteSize(ZoomSprite,32,-1)
	SetSpritePosition(ZoomSprite,8,82)
	
	GetGPSSprite=LoadSprite("GetGPS.png")
	SetSpriteSize(GetGPSSprite,50,-1)
	SetSpritePosition(GetGPSSprite,25,90)	
	
	WorkingSprite=LoadSprite("Working.png")
	SetSpriteSize(WorkingSprite,25,-1)
	SetSpritePosition(WorkingSprite,75,75)
	SetSpriteVisible(WorkingSprite,0)
	
Return

DeviceCheck:
	`Check for required sensors etc
	dev$=GetDeviceName( )
	GPSexists = GetGPSSensorExists() 
	Internet = GetInternetState()
	SensorAvailable=GetMagneticSensorExists() 
	AccelSensor=GetAccelerometerExists() 

	if GPSexists=0 or Internet=0 or SensorAvailable=0 or AccelSensor=0
	`change the info to a back button
	SetVirtualButtonImageUp(2,BackImage1)
	SetVirtualButtonImageDown(2,BackImage2)

		Do
			Print("")
			Print("")
			Print("")
			Print("")
			Print("")
			Print(dev$)
			if GPSexists=0
				
				Print("")
				Print("Please turn your GPS or ")
				Print("location services on. ")
				endif
		
			if Internet=0
				Print("")
				Print("Please Connect to ")
				Print("the Internet. ")
			endif
		
			if SensorAvailable=0
				Print("")
				Print("Your Device does not have ")
				Print("a magnetic Sensor.")
			endif
		
			if AccelSensor=0
				Print("")
				Print("Your Device does not have ")
				Print("an Accelerometer.")
			endif
			
			Print("")
			Print("Also ensure `GeoCompass` has ")
			Print("the appropriate permisions. ")
			Print("")
			Print("Exit and Restart the APP. ")
			Print("")
			Print("or press 'Back' to continue")
			Print("with limited Funtionality.")
		
			if GetVirtualButtonReleased( 1 ) =1 then end `ends program
			if GetVirtualButtonReleased( 2 ) =1 then Exit`exits loop
			sync()
		loop
		
		Repeat
			sync()
		until GetVirtualButtonPressed( 2 ) =0
			`reset the info button
		SetVirtualButtonImageUp(2,InfoUpImage)
		SetVirtualButtonImageDown(2,InfoDownImage)
	endif	
return

Work:
	workang=workang+1
	if workang=360 then workang=0
	SetSpriteAngle(WorkingSprite,Workang)
	Sync()
return
