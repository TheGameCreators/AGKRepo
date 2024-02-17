
// set window properties
SetWindowTitle( "Local Notifications" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )

AddVirtualButton( 1, 512-60, 450, 100 )
SetVirtualButtonText( 1, "Set" )

AddVirtualButton( 2, 512+60, 450, 100 )
SetVirtualButtonText( 2, "Cancel" )

SetPrintSize(20)

do
    if ( GetVirtualButtonReleased(1) )
		SetLocalNotification( 1, GetUnixTime()+10, "Hello" ) // 10 seconds in the future
		SetLocalNotification( 2, GetUnixTime()+20, "Hello again!" ) // 20 seconds in the future
	endif
	
	if ( GetVirtualButtonReleased(2) )
		CancelLocalNotification(1)
		CancelLocalNotification(2)
	endif

	Print( "Press Set and then minimize the app to see the notifications" )
	Print( "Notification 1 exists: " + str(GetLocalNotificationExists(1)) )
	Print( "Notification 2 exists: " + str(GetLocalNotificationExists(2)) )
	
	if ( GetLocalNotificationExists(1) ) 
		timeLeft = GetLocalNotificationTime(1)-GetUnixTime()
		if ( timeLeft < 0 ) then timeLeft = 0
		Print( "Time until notification 1: " + str(timeLeft) )
	endif
	if ( GetLocalNotificationExists(2) ) 
		timeLeft = GetLocalNotificationTime(2)-GetUnixTime()
		if ( timeLeft < 0 ) then timeLeft = 0
		Print( "Time until notification 2: " + str(timeLeft) )
	endif
	
    Print( "FPS: " + str(ScreenFPS(),1) )
    Sync()
loop
