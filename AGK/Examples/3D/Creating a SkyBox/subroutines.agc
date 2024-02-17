

DoMouseLook:
	If GetPointerState()
		Camera_Angle_X# = Camera_Angle_X# + ((GetPointerY()-OldPointerY#)*0.5)
		Camera_Angle_Y# = Camera_Angle_Y# + ((GetPointerX()-OldPointerX#)*0.5)
		if Camera_Angle_X# < -90 then Camera_Angle_X# = -90
		if Camera_Angle_X# > 90 then Camera_Angle_X# = 90
		SetCameraRotation(1,Camera_Angle_X#,Camera_Angle_Y#,0)
	endif
	OldPointerX# = GetPointerX()
	OldPointerY# = GetPointerY()
return


ChangeSky:
	if GetTweenCustomPlaying(SkyTween) 
		StopTweenCustom(SkyTween)
	endif
	SetTweenCustomInteger1(SkyTween,Sky_Red,random(0,255),TweenSmooth1())
	SetTweenCustomInteger2(SkyTween,Sky_Green,random(0,255),TweenSmooth1())
	SetTweenCustomInteger3(SkyTween,Sky_Blue,random(0,255),TweenSmooth1())
	PlayTweenCustom(SkyTween,0)
return

ChangeHorizon:
	if GetTweenCustomPlaying(HorizonTween) 
		StopTweenCustom(HorizonTween)
	endif
	SetTweenCustomInteger1(HorizonTween,Sky_Horiz_Red,random(0,255),TweenSmooth1())
	SetTweenCustomInteger2(HorizonTween,Sky_Horiz_Green,random(0,255),TweenSmooth1())
	SetTweenCustomInteger3(HorizonTween,Sky_Horiz_Blue,random(0,255),TweenSmooth1())
	PlayTweenCustom(HorizonTween,0)
return

ChangeSun:
	if GetTweenCustomPlaying(SunTween) 
		StopTweenCustom(SunTween)
	endif
	SetTweenCustomInteger1(SunTween,Sky_Sun_Red,random(0,255),TweenSmooth1())
	SetTweenCustomInteger2(SunTween,Sky_Sun_Green,random(0,255),TweenSmooth1())
	SetTweenCustomInteger3(SunTween,Sky_Sun_Blue,random(0,255),TweenSmooth1())
	PlayTweenCustom(SunTween,0)
return

position_virtual_buttons:
	SetVirtualButtonPosition(1,GetScreenBoundsLeft()+25,GetScreenBoundsBottom()-25)
	SetVirtualButtonPosition(2,50,GetScreenBoundsBottom()-25)
	SetVirtualButtonPosition(3,GetScreenBoundsRight()-25,GetScreenBoundsBottom()-25)
return

create_buttons:
	// Create a virtual button to change skybox colours
	AddVirtualButton(1,GetScreenBoundsLeft()+25,GetScreenBoundsBottom()-25,20)
	SetVirtualButtonText(1,"Sky")
	AddVirtualButton(2,50,GetScreenBoundsBottom()-25,20)
	SetVirtualButtonText(2,"Horizon")
	AddVirtualButton(3,GetScreenBoundsRight()-25,GetScreenBoundsBottom()-25,20)
	SetVirtualButtonText(3,"Sun")
return

create_city:
	// Create a block city landscape
	for t = 1 to 80
		boxheight = random(10,40)
		box = CreateObjectBox(10,boxheight,10)
		SetObjectPosition(box,random(0,800)-400,boxheight/2,random(0,800)-400)
		SetObjectColor(box,random(0,255),random(0,255),random(0,255),255)
		sphere = CreateObjectSphere(6,8,8)
		SetObjectPosition(sphere,random(0,800)-400,3,random(0,800)-400)
		SetObjectColor(sphere,random(0,255),random(0,255),random(0,255),255)
	next t
	// Create a floor
	plane_floor = CreateObjectPlane(2000,2000)
	RotateObjectLocalX(plane_floor,90)
	SetObjectColor(plane_floor,30,30,30,255)
return
