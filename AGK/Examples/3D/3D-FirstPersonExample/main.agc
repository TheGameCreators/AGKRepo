rem
rem 3D Lightmapping
rem Artwork by Mark Blosser
rem

rem Init app
SetSyncRate(60,0)
SetClearColor(128,64,0)
setcamerarange(1,15,1000)
SetScissor(0,0,0,0)

rem Loading status
customimg=LoadImage("custom.png")
UseNewDefaultFonts( 1 )
SetPrintSize( 6 )
SetPrintColor( 255,255,255 )
CreateText(1,"Loading Metro Theatre Scene")
SetTextFontImage(1,customimg)
SetTextAlignment(1,1)
SetTextPosition(1,50,45)
SetTextSize(1,10)
CreateText(2,"Artwork by Mark Blosser")
SetTextFontImage(2,customimg)
SetTextAlignment(2,1)
SetTextPosition(2,50,60)
SetTextSize(1,8)
Sync()

rem Load world
gosub _load_world

rem Setup camera
plrx#=1850 : plrz#=-3650 : plra#=0 : eyeheight#=553
SetCameraPosition(1,plrx#,eyeheight#,plrz#)
SetCameraRotation(1,0,plra#,0)

createpointlight( 1, plrx#,eyeheight#,plrz#, 600, 255,255,255 )
setpointlightmode(1,1)
CreateObjectSphere( 99, 10, 10,10 )
SetObjectLightMode( 99, 0 )

SetSunActive(0)
SetAmbientColor(50,50,50)

rem Start loop
DeleteText(1) : DeleteText(2)

setrawjoystickdeadzone(0.1)

addvirtualbutton( 1, 94.7,93.1,9.77 )
addvirtualbutton( 2, 94.7,79.4,9.77 )
SetVirtualButtonText(2,"Forward")
SetVirtualButtonText(1,"Back")


rem Main loop
do
 `
 rem Old position
 oldx#=GetCameraX(1)
 oldy#=GetCameraY(1)-45.0
 oldz#=GetCameraZ(1)
 `
 rem Move camera
 if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, 4 )
 if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -4 )
 if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -4 )
 if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, 4 )
 
 if ( getvirtualbuttonstate(1) = 1 ) then MoveCameraLocalZ( 1, -4 )
 if ( getvirtualbuttonstate(2) = 1 ) then MoveCameraLocalZ( 1, 4 )
	
 if ( GetPointerPressed() )
	startx# = GetPointerX()
	starty# = GetPointerY()
	angx# = GetCameraAngleX(1)
	angy# = GetCameraAngleY(1)
	pressed = 1
 endif

 if ( GetPointerState() = 1 )
	fDiffX# = (GetPointerX() - startx#)/1.0
	fDiffY# = (GetPointerY() - starty#)/1.0

	newX# = angx# + fDiffY#
	if ( newX# > 89 ) then newX# = 89
	if ( newX# < -89 ) then newX# = -89
	SetCameraRotation( 1, newX#, angy# + fDiffX#, 0 )
 endif

 if ( getrawjoystickexists(1) )
    MoveCameraLocalZ(1,-getrawjoysticky(1)*4)
    MoveCameraLocalX(1,getrawjoystickx(1)*4)
    RotateCameraGlobalY(1,getrawjoystickrx(1)*4)
    RotateCameraLocalX(1,getrawjoystickry(1)*4)
endif
 `
 rem Gravity on camera
 SetCameraPosition(1,getcamerax(1),getcameray(1)-4,getcameraz(1))
 `
 rem New position
 newx#=GetCameraX(1)
 newy#=GetCameraY(1)-45
 newz#=GetCameraZ(1)
 `
 rem Adjust Y to match floor
 if ObjectSphereSlide(0,oldx#,oldy#,oldz#,newx#,newy#,newz#,19.0)>0
  newx#=GetObjectRayCastSlideX(0)
  newy#=GetObjectRayCastSlideY(0)
  newz#=GetObjectRayCastSlideZ(0)
  SetCameraPosition(1,newx#,newy#+45.0,newz#)
 endif
 
 posx# = sin(timer()*40) * 250 + 2000
 SetPointLightPosition( 1, posx#, 565, -3600 )
 SetObjectPosition( 99, posx#, 565, -3600 )
 `
 rem Framerate prompt
 print("W A S D or joystick to move, click and drag to rotate")
 fps=ScreenFPS() : Print("FPS: " + str(fps))
 `
 rem Update screen
 Sync()
 `
loop

_load_world:
 `
 `
 rem Load lightmaps
 dim lm[10]
 lm[0]=LoadImage("0.png")
 lm[1]=LoadImage("1.png")
 for t=0 to 1
  SetImageWrapU(lm[t],1)
  SetImageWrapV(lm[t],1)
 next t
 `
 rem Load all OBJ making up world (created in FPS Creator)
 objmax=50
 for obj=1 to objmax
  obj$="mesh"+str(obj)+".obj"
  LoadObject(obj,obj$,0)
  if obj>=1 and obj<=7
   tex$="mesh"+str(obj)+"-1.jpg"
  else
   tex$="mesh"+str(obj)+"-0.jpg"
  endif
  texname$=left(tex$,len(tex$)-4)
  lm$=right(texname$,1)
  if lm$="-" then lm$="0"
  teximg=LoadImage(tex$)
  SetImageWrapU(teximg,1)
  SetImageWrapV(teximg,1)
  SetObjectImage(obj,teximg,0)
  SetObjectLightMap(obj,lm[val(lm$)])
  `
  rem Progress status
  perc#=obj : perc#=perc#/objmax : perc=perc#*100
  SetTextString(1,"Metro Theatre - "+str(perc)+"%") : Sync()

 next obj
 `
return
