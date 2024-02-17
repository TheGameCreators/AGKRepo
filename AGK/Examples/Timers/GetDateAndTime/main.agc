
// Get Date and Time from system
// Introduced in build 107

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background5.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// main loop
do
    // get values from date and time
    year=val(left(GetCurrentDate(),4))
    month=val(left(right(GetCurrentDate(),5),2))
    days=val(right(GetCurrentDate(),2))
    hours=val(left(GetCurrentTime(),2))
    minutes=val(right(left(GetCurrentTime(),5),2))
    seconds=val(right(GetCurrentTime(),2))

	// print date and time data values
	Print("GetCurrentDate="+GetCurrentDate())
	Print("GetCurrentTime="+GetCurrentTime())
	Print("GetDayOfWeek="+str(GetDayOfWeek()))
	Print("")
	Print("CONVERT Y="+str(year)+" M="+str(month)+" D="+str(days))
	unixtime=GetUnixFromDate(year, month, days, hours, minutes, seconds)
	Print("GetUnixFromDate="+str(unixtime))
	Print("GetYearFromUnix="+str(GetYearFromUnix(unixtime)))
	Print("GetMonthFromUnix="+str(GetMonthFromUnix(unixtime)))
	Print("GetDaysFromUnix="+str(GetDaysFromUnix(unixtime)))
	Print("GetHoursFromUnix="+str(GetHoursFromUnix(unixtime)))
	Print("GetMinutesFromUnix="+str(GetMinutesFromUnix(unixtime)))
	Print("GetSecondsFromUnix="+str(GetSecondsFromUnix(unixtime)))
	Print("GetLeapYear="+str(GetLeapYear(year)))

    // update the screen
    Sync ( )
loop

