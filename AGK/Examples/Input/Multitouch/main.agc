setvirtualresolution(GetDeviceWidth(),GetDeviceHeight())

// nice fonts
UseNewDefaultFonts( 1 )

rem create sprites to represent touch points
for i = 1 to 10
	CreateSprite( i, 0 )
	SetSpriteColor( i, random(50,255),random(50,255),random(50,255),255 )
	SetSpriteSize( i, GetDeviceWidth()/10.0, GetDeviceWidth()/10.0 )
	SetSpriteVisible( i, 0 )
next i

assigned as integer[10]

do
	for i = 1 to 10
		assigned[i] = 0
		SetSpriteVisible( i, 0 )
    next i

    rem find out which sprites are currently taken
    touchID = GetRawFirstTouchEvent(1)
    while touchID > 0
        sprite = GetRawTouchValue( touchID )
        if ( sprite > 0 ) then assigned[sprite] = 1
        touchID = GetRawNextTouchEvent()
    endwhile

    rem move assigned sprites to touch locations
    touchID = GetRawFirstTouchEvent(1)
    while touchID > 0
        x# = GetRawTouchCurrentX( touchID )
        y# = GetRawTouchCurrentY( touchID )
        sprite = GetRawTouchValue( touchID )

        if ( sprite = 0 )
            rem assign this new touch point a sprite
            for i = 1 to 10
				if ( assigned[i] = 0 )
					assigned[i] = 1
					sprite = i
					exit
				endif
			next i
            
            SetRawTouchValue( touchID, sprite )
        endif

        if ( sprite > 0 )
            SetSpritePositionByOffset( sprite, x#, y# )
            SetSpriteVisible( sprite, 1 )
        endif

        touchID = GetRawNextTouchEvent()
    endwhile
	SetPrintSize(46)
	Print( "Press the screen with more than one finger")
    Print( "Touch Events: " + str(GetRawTouchCount(1)) )

    Sync()
loop
