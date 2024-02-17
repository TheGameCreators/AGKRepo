SetOrientationAllowed(1,1,1,1)
SetVirtualResolution( 1024, 768 )
SetPrintSize(25)

// nice fonts
UseNewDefaultFonts( 1 )


StartGPSTracking()

do
	SetPrintSize(36)
	print( "Orientation: "+str(GetOrientation()) )
	print("Accel: "+str(GetAccelerometerExists()) + " X: "+formatnumber(GetRawAccelX()) + ", Y: " + formatnumber(GetRawAccelY()) + ", Z: " + formatnumber(GetRawAccelZ()) )
	print("Gyro: "+str(GetGyroSensorExists()) + " X: "+formatnumber(GetRawGyroVelocityX()) + ", Y: " + formatnumber(GetRawGyroVelocityY()) + ", Z: " + formatnumber(GetRawGyroVelocityZ()))
	print("Magnet: "+str(GetMagneticSensorExists()) + " X: "+formatnumber(GetRawMagneticX()) + ", Y: " + formatnumber(GetRawMagneticY()) + ", Z: " + formatnumber(GetRawMagneticZ()))
	print("Prox: "+str(GetProximitySensorExists()) + " : "+formatnumber(GetRawProximityDistance()) )
	print("Light: "+str(GetLightSensorExists()) + " : "+formatnumber(GetRawLightLevel()))
	print("RotVec: "+str(GetRotationVectorSensorExists()) + " X: "+formatnumber(GetRawRotationVectorX()) + ", Y: " + formatnumber(GetRawRotationVectorY()) + ", Z: " + formatnumber(GetRawRotationVectorZ()) + ", W: " + formatnumber(GetRawRotationVectorW()) )
	print("GPS: "+str(GetGPSSensorExists()))
	print("Longitude: "+formatnumber(GetRawGPSLongitude()))
	print("Latitude: "+formatnumber(GetRawGPSLatitude()))
	
	if GetPointerPressed()=1 then StopGPSTracking()
	
	Sync()
loop

function formatnumber( f as float )
	if ( f > 0 ) 
		s$ = " " + str(f)
	else
		s$ = str(f)
	endif
endfunction s$
	
