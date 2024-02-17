REM *********************************************************************************
REM ********************* AGK2 SENSORS EXAMPLE ***********************
REM *********************************************************************************
REM ************** MADE BY STEPHAN GUENTHER ************************
REM *********************************************************************************

SetWindowTitle( "PilotsSixPack" )
SetWindowSize( 1024, 768, 0 )

global opsys$
global oldtime#
global time#

gosub setupdevice
gosub loadassets
gosub setuppanel

REM **************** MAIN LOOP **********************
do
    secondssincelastloop# = calctime() 
	gosub updateinstruments

    Sync()
loop
REM *******************************************************

REM ******************** LOAD MEDIA ****************
loadassets:
	panelimg = LoadImage("panel.png")
	compassimg = LoadImage("compass.png")
	compassinopimg = LoadImage("compassinop.png")
	horizonimg = LoadImage("horizon.png")
	horizoninopimg = LoadImage("horizoninop.png")
	althand1img = LoadImage("althand1.png")
	althand2img = LoadImage("althand2.png")
	althand2inopimg = LoadImage("althand2inop.png")
	turnplaneimg = LoadImage("turnplane.png")
	turnplaneinopimg = LoadImage("turnplaneinop.png")
	ballturnimg = LoadImage("ballturn.png")
return

REM **************** BUILD MAIN SCREEN **********
setuppanel:
	SetClearColor(102,102,102) // SET BACKGROUND COLOR
	panelspr = CreateSprite(panelimg)
	SetSpriteSize(panelspr,-1,100)
	SetSpritePositionByOffset(panelspr,50.0,50.0)
	SetSpriteDepth(panelspr,5)
	panelw# = GetSpriteWidth(panelspr)
	
	compassspr = CreateSprite(compassimg)
	SetSpriteSize(compassspr,-1,41.5)
	SetSpritePositionByOffset(compassspr,50.0,76.2)
	SetSpriteDepth(compassspr,6)
	
	horizonspr = CreateSprite(horizonimg)
	SetSpriteSize(horizonspr,panelw#*0.345,41.5)
	SetSpritePositionByOffset(horizonspr,50.0,23.4)
	SetSpriteDepth(horizonspr,7)
	
	althand1spr = CreateSprite(althand1img)
	SetSpriteSize(althand1spr,-1,41.5)
	SetSpritePositionByOffset(althand1spr,50.0+panelw#*0.327,27.0)
	SetSpriteDepth(althand1spr,4)
	
	althand2spr = CreateSprite(althand2img)
	SetSpriteSize(althand2spr,-1,41.5)
	SetSpritePositionByOffset(althand2spr,50.0+panelw#*0.327,27.0)
	SetSpriteDepth(althand2spr,3)
	
	variohandspr = CreateSprite(althand2img)
	SetSpriteSize(variohandspr,-1,41.5)
	SetSpritePositionByOffset(variohandspr,50.0+panelw#*0.327,73.0)
	SetSpriteDepth(variohandspr,3)
	
	speedohandspr = CreateSprite(althand2img)
	SetSpriteSize(speedohandspr,-1,41.5)
	SetSpritePositionByOffset(speedohandspr,50.0-panelw#*0.327,27.0)
	SetSpriteDepth(speedohandspr,3)
	
	turnplanespr = CreateSprite(turnplaneimg)
	SetSpriteSize(turnplanespr,-1,41.5)
	SetSpritePositionByOffset(turnplanespr,50.0-panelw#*0.327,73.0)
	SetSpriteDepth(turnplanespr,2)
	
	ballturnspr = CreateSprite(ballturnimg)
	SetSpriteSize(ballturnspr,-1,41.5)
	SetSpritePositionByOffset(ballturnspr,50.0-panelw#*0.327,73.0)
	SetSpriteDepth(ballturnspr,3)
	
	latitudetext = CreateText("0.0")
	longitudetext = CreateText("0.0")
	
	SetTextDepth(latitudetext,2)		// place text infront of panel background
	SetTextDepth(longitudetext,2)	// place text infront of panel background
	
	textsize# = 3.7
	SetTextSize(latitudetext,textsize#)
	SetTextSize(longitudetext,textsize#)
	
	SetTextSpacing(latitudetext,-0.3)
	SetTextSpacing(longitudetext,-0.3)
	
	SetTextColor(latitudetext,178,178,178,255)
	SetTextColor(longitudetext,178,178,178,255)
	
	SetTextPosition(latitudetext,50.0+panelw#*0.173,46.59)
	SetTextPosition(longitudetext,50.0+panelw#*0.173,49.59)
	
	REM ************ SET INSTRUMENT STATES **********
	horizonstate = 1
	oldhorizonstate = -1
	compassstate = 1
	oldcompassstate = -1
	altimeterstate = 1
	oldaltimeterstate = -1
	variometerstate = 1
	oldvariometerstate = -1
	speedometerstate = 1
	oldspeedometerstate = -1
	turncoordinatorstate = 1
	oldturncoordinatorstate = -1
	REM ***********************************************************
return

setupdevice:
	SetOrientationAllowed(0,0,1,0) // SET DISPLAY TO LANDSCAPE MODE (BUTTON RIGHT)
	devwidth# = GetDeviceWidth()
	devheight# = GetDeviceHeight()
	SetDisplayAspect(devwidth#/devheight#)
	
    if devwidth# < devheight#
        tempstore# = devheight#
        devheight# = devwidth#
        devwidth# = tempstore#
    endif
    
    dev$ = getdevicename()
    opsys$ = getdevicebasename()

    devicetype$ = "iPad"
    if left(dev$,8)="ios|iPad"
        devicetype$ = "iPad"
    endif
    if left(dev$,10)="ios|iPhone"
        devicetype$ = "iPhone"
    endif
    if left(dev$,7)="android"
        devicetype$ = "android"
    endif
    if dev$ = "windows"
        devicetype$ = "windows"
    endif

    if GetGPSSensorExists()
		StartGPSTracking() 
	endif
return

updateinstruments:
	REM ******* ARTIFICIAL HORIZON *******
		horizonstate = GetGyroSensorExists()
		
		if horizonstate <> oldhorizonstate
			select horizonstate
				case 1
						SetSpriteImage(horizonspr,horizonimg)
				endcase
				case default
						SetSpriteImage(horizonspr,horizoninopimg)
						ResetSpriteUV(horizonspr)
						SetSpriteUV(horizonspr,0.0,0.27,0.0,0.73,1.0,0.27,1.0,0.73)
				endcase
			endselect
			oldhorizonstate = horizonstate
		endif
		
		if horizonstate = 1
			rem pitchangle# = GetRawAccelZ()*0.205 // GRAVITY, FORCE RELATED
			pitchangle# = GetRawRotationVectorZ2()*0.29 // GYRO RELATED
			
			REM *********** show pitch angle on horizon by modifying uv data of the sprite ******************************************
			ResetSpriteUV(horizonspr)
			SetSpriteUV(horizonspr,0.0,0.27+pitchangle#,0.0,0.73+pitchangle#,1.0,0.27+pitchangle#,1.0,0.73+pitchangle#)
			
			rem rollangle# = -GetRawAccelX()*90.0 // GRAVITY, FORCE RELATED
			rollangle# = -GetRawRotationVectorX2()*100.0 // GYRO RELATED
			
			SetSpriteAngle(horizonspr,rollangle#) // roll horizon sprite by rollangle
		endif
	REM ********************************************
	
	REM ******* COMPASS *******
		compassstate = GetMagneticSensorExists()
		
		if compassstate <> oldcompassstate
			select compassstate
				case 1
						SetSpriteImage(compassspr,compassimg)
				endcase
				case default
						SetSpriteImage(compassspr,compassinopimg)
				endcase
			endselect
			oldcompassstate = compassstate
		endif
		
		if compassstate = 1
			heading# = getheading()
			SetSpriteAngle(compassspr,heading#)
		endif
	REM ********************************************
	
	REM ******* ALTIMETER ********************
		altimeterstate = GetGPSSensorExists()
		
		if altimeterstate <> oldaltimeterstate
			select altimeterstate
				case 1
						SetSpriteImage(althand2spr,althand2img)
				endcase
				case default
						SetSpriteImage(althand2spr,althand2inopimg)
				endcase
			endselect
			oldaltimeterstate = altimeterstate
		endif
		
		if altimeterstate = 1
			altitude# = GetRawGPSAltitude()
			altt# = (altitude#/10000.0)*360.0
			alth# = ((altitude#-trunc(altitude#/1000.0))/1000.0)*360.0
			SetSpriteAngle(althand1spr,altt#)
			SetSpriteAngle(althand2spr,alth#)
		endif
	REM ********************************************
	
	REM ******* VARIOMETER *****************
		variometerstate = GetGPSSensorExists()
		
		if variometerstate <> oldvariometerstate
			select variometerstate
				case 1
						SetSpriteImage(variohandspr,althand2img)
						SetSpriteAngle(variohandspr,270.0)
				endcase
				case default
						SetSpriteImage(variohandspr,althand2inopimg)
						SetSpriteAngle(variohandspr,0.0)
				endcase
			endselect
			oldvariometerstate = variometerstate
		endif
		
		if variometerstate = 1
			timesincelastaltcall# = time#-lastaltcalltime#
			if timesincelastaltcall# > 0.2 // REM MINIMUM TIME BETWEEN GPS ALTITUDE REQUESTS IN SECONDS
				lastaltcalltime# = time#
				varioalt# = GetRawGPSAltitude()
				climbinfeetperminute# = (varioalt#-oldvarioalt#)*60.0/timesincelastaltcall#
				oldvarioalt# = varioalt#
			endif
			SetSpriteAngle(variohandspr,270.0+40.0*climbinfeetperminute#/500.0)
		endif
	REM ********************************************
	
	REM ******* SPEEDOMETER *****************
		speedometerstate = GetGPSSensorExists()
		
		if speedometerstate <> oldspeedometerstate
			select speedometerstate
				case 1
						SetSpriteImage(speedohandspr,althand2img)
						SetSpriteAngle(speedohandspr,0.0)
				endcase
				case default
						SetSpriteImage(speedohandspr,althand2inopimg)
						SetSpriteAngle(speedohandspr,0.0)
				endcase
			endselect
			oldspeedometerstate = speedometerstate
		endif
		
		if speedometerstate = 1
			deltatime# = time# - lastgpspostime#
			if deltatime# > 1.0 // request gps data and calculate it in a one second interval
				lat2# = GetRawGPSLatitude()
				long2# = GetRawGPSLongitude()
				
				currentspeed# = gpsspeed(lat1#,long1#,lat2#,long2#,deltatime#)
				
				lastgpspostime# = time#
				lat1# = lat2#
				long1# = long2#
			endif
			SetSpriteAngle(speedohandspr,currentspeed#/100.0*360.0)
		endif
	REM ********************************************
	
	REM ******* TURN COORDINATOR ********************
		turncoordinatorstate = GetGyroSensorExists() 
		
		if turncoordinatorstate <> oldturncoordinatorstate
			select turncoordinatorstate
				case 1
						SetSpriteImage(turnplanespr,turnplaneimg)
				endcase
				case default
						SetSpriteImage(turnplanespr,turnplaneinopimg)
				endcase
			endselect
			oldturncoordinatorstate = turncoordinatorstate
		endif
		
		if turncoordinatorstate = 1
			turnspeed# = -GetRawGyroVelocityY()*20.0
			SetSpriteAngle(turnplanespr,turnspeed#)
		endif
		
		if GetAccelerometerExists() = 1
			centerofgravityangle# = -GetRawAccelX()*90.0 // GRAVITY, FORCE RELATED
			if centerofgravityangle# > 60.0
				centerofgravityangle# = 60.0
			endif
			if centerofgravityangle# < -60.0
				centerofgravityangle# = -60.0
			endif
			SetSpriteAngle(ballturnspr,centerofgravityangle#)
		endif
	REM ********************************************
	
	REM ******* SHOW GPS POSITION COORDINATES ********************
		showlat$ = str(GetRawGPSLatitude())
		showlat$ = left(showlat$,6) // limit text lenght to 6 chars
		
		showlong$ = str(GetRawGPSLongitude())
		showlong$ = left(showlong$,6) // limit text lenght to 6 chars
		
		SetTextString(latitudetext,showlat$)
		SetTextString(longitudetext,showlong$)
	REM ******************************************************************************
return

function getheading()
		if opsys$ = "ios"
			REM get magnetic sensor data
			_xc# = GetRawMagneticX()
			_yc# = GetRawMagneticY()
			_zc# = GetRawMagneticZ()
		else
			REM modify and change axes for other operating systems if required
			_xc# = GetRawMagneticX()
			_yc# = GetRawMagneticY()
			_zc# = GetRawMagneticZ()
		endif
		
		_alpha# = atan(_zc#/_yc#) // calculate raw heading angle
		
		REM **************** fine tune heading values *********************
		if _yc# >= 0.0
			_alpha# = _alpha# + 90.0
		else
			_alpha# = _alpha# + 270.0
		endif
		
		if _alpha# < 0.0
			_alpha# = 360.0 + _alpha#
		endif
		
		headingreturn# = 360.0-_alpha#
		REM *******************************************************************
endfunction headingreturn#

REM ********************** CALCULATE DISTANCE BETWEEN TWO GPS COORDINATES **********************
function gpsdistance(lat1#,long1#,lat2#,long2#)
		earthradius# = 6371.0 // in km
		
		deltalat# = degtorad(lat2#-lat1#)
		deltalong# = degtorad(long2#-long1#)
		lat1# = degtorad(lat1#)
		lat2# = degtorad(lat2#)
		
		a# = (sinrad(deltalat#/2.0)*sinrad(deltalat#/2.0))+(sinrad(deltalong#/2.0)*sinrad(deltalong#/2.0))*cosrad(lat1#)*cosrad(lat2#)
		c# = 2.0*ATan2rad(sqrt(a#),sqrt(1-a#))
		gpsdist# = earthradius# * c#
endfunction gpsdist#

function degtorad(degval#)
	pi# = 3.141592653589793 
	radval# = degval#*pi#/180.0
endfunction radval#
REM ******************************************************************************************************************************

REM ********************** CALCULATE SPEED BETWEEN TWO GPS POINTS AND GIVEN DELTATIME *****
function gpsspeed(lat1#,long1#,lat2#,long2#,deltatime#)
	distance# = gpsdistance(lat1#,long1#,lat2#,long2#)
	speedcalc# = distance#/deltatime# // km/sec
	speedkmperhour# = speedcalc#*(3600.0/deltatime#) 
	speedknots# = speedkmperhour#/1.852 // convertion into knots (nautical miles per hour)
endfunction speedknots#
REM ******************************************************************************************************************************

REM *********************** CALCULATE TIME SINCE LAST LOOP ***************************************
REM ************* TO ADJUST LOOP VALUES AT VARIOUS FRAME RATES IF NEEDED **********
function calctime()
    time# = timer()
    deltatime# = (timer()-oldtime#)*100.0
    oldtime# = timer()
endfunction deltatime#
REM *****************************************************************************************************************
