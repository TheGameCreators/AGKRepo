// Virtual Reality in AGK v2
// by Chris Chetwood with help from Steve Holding and Sean HockeyKid
// This app produces a fully functioning VR experiance solely using AGK.
// The app needs to be executed on a PC and a mobile device running Android over a local network at the same time.
// The PC needs a 360 controller (or equivalent) or a keyboard connected.
// The Android device needs to be held in landscape, ideally within a Google Cardboard headset (or equivalent).

//Create a custom type to store data about the two cameras, one for each eye.
type EyeType
    RenderImage as integer
    Sprite as integer
    PosX as float
    PosY as float
    PosZ as float
endtype

//define the two eyes
LeftEye as EyeType
RightEye as EyeType
//define the distance between the two eyes
global EyeDistance as float : EyeDistance = 10.0

//set the Network ID as a global variable
global NetworkID as integer

//Since this app will be executed on a PC and an Android device at the same time we need to determine which one is which
OSName$ = GetDeviceName( )
if OSName$="windows"
    RunningPC = 1
else
    RunningPC = 0
endif

//set global display settings
SetVirtualResolution( 1280,720 )
//force the Android device to only work in Ladscape mode.
SetOrientationAllowed( 0,0,1,0 )
SetViewOffset(0.0,0.0)
SetPrintSize( 32 )
SetGlobal3DDepth(10000)
SetScissor( 0,0,0,0 )
SetSyncRate( 60, 1 )

SetWindowTitle("AGK-VR: the occulus thrift")

//Set default Camera/Eye positions
LeftEye.PosX = -(EyeDistance / 2.0)
LeftEye.PosY = 0.0
LeftEye.PosZ = 0.0
RightEye.PosX = (EyeDistance / 2.0)
RightEye.PosY = 0.0
RightEye.PosZ = 0.0

//set camera field of view angle and view distance
SetCameraFOV(1,90.0)
SetCameraRange(1,5,2000)

//set up images for each eye to render the camera to
LeftEye.RenderImage = CreateRenderImage(getvirtualwidth()/2,getvirtualheight(),0,0 )
LeftEye.Sprite = CreateSprite(0)
SetSpriteTransparency( LeftEye.Sprite,0 )
SetSpriteDepth(LeftEye.Sprite,1)
FixSpriteToScreen(LeftEye.Sprite,1)

RightEye.RenderImage = CreateRenderImage(getvirtualwidth()/2,getvirtualheight(),0,0 )
RightEye.Sprite = CreateSprite(0)
SetSpriteTransparency( RightEye.Sprite,0 )
SetSpriteDepth(RightEye.Sprite,1)
FixSpriteToScreen(RightEye.Sprite,1)

//load font for text objects
customimg = LoadImage("Fnt_Illuminate.png")

//display sync message depending on the device
if RunningPC=1
    CreateText(1,"Waiting for mobile to connect...")
else
    CreateText(1,"Waiting for PC to host...")
endif

//define text object properties
SetTextFontImage(1,customimg)
SetTextAlignment(1,1)
SetTextPosition(1,getvirtualwidth()/2.0,getvirtualheight()/2.0)
SetTextSize(1,80)

CreateText(2,"The Oculus Thrift")
SetTextFontImage(2,customimg)
SetTextAlignment(2,1)
SetTextPosition(2,getvirtualwidth()/2.0,80.0)
SetTextSize(2,50)

//update the screen
Sync()

//Connecting to the LAN
if RunningPC=1
	//Set the PC version as the HOST
    NetworkID = HostNetwork("Cardboard_UG","PC_Joystick",2000)
    //Check that the PC has a joystick plugged in
    CompleteRawJoystickDetection()
    
    
    if GetJoystickExists()=0
		if GetKeyboardExists()=1
			//if no joystick warn player about the contols
			Message("Could not detect a joystick.  Use WASD on the keyboard.")
		else
			//quit if no joystick or keyboard connected quit the app
			Message("Could not detect a joystick or a keyboard.  Connect one and restart.")
			end
		endif
	endif
else
	//Set the Android device as a CLIENT.
    NetworkID = JoinNetwork("Cardboard_UG","Headset")
    
    //quit if no vector sensor exists on the device
    if GetRotationVectorSensorExists()=0
		Message("Could not detect a rotation sensor.")
		end
	endif
endif

//Wait for the current device to connect to the network - abort if this takes longer than 20 seconds
timeout# = timer() + 20.0
repeat
    sync()
    //determine if device is connected to the network
    Connected = IsNetworkActive(NetworkID)
until Connected>0 or timer()>timeout#

if timer()>timeout#
	Message("Connection timed out.")
    end
endif


//Hold point until second device is connected
//There is no time-out here, but process can be aborted by pressing BACK/ESC.
repeat
    Clients = GetNetworkNumClients(NetworkID)
    
    //Quit the process if BACK/ESC key is pressed
    if GetRawKeyPressed( 27 )
        if IsNetworkActive(NetworkID)=1
            CloseNetwork(NetworkID)
        endif

        end
    endif

    sync()
until Clients=2


//Both devices are now connected so record Host and Client IDs
if RunningPC=1
	//PC is always the HOST
    HostID = GetNetworkMyClientID(NetworkID)
    tempID = GetNetworkFirstClient(NetworkID)

	//Find Android device CLIENT ID
    while tempID<>0
        if tempID<>HostID
			//Android has been found
            ClientID = tempID
        endif
        tempID = GetNetworkNextClient(NetworkID)
    endwhile

else
	//Android is always the CLIENT
    ClientID = GetNetworkMyClientID(NetworkID)
    tempID = GetNetworkFirstClient(NetworkID)

	//Find the HOST ID
    while tempID<>0
        if tempID<>ClientID
			//HOST has been found
            HostID = tempID
        endif
        tempID = GetNetworkNextClient(NetworkID)
    endwhile

endif


// Load 3D world - in this example the scene from the 3D demo has been used.
settextstring(1,"Loading Metro Theatre Scene")
settextstring(2,"Artwork by Mark Blosser")

gosub _load_world

settextvisible(1,0)
settextvisible(2,0)

//set player's starting position
plrx#=1850 : plrz#=-3650 : plry#=560

if RunningPC=1
	//If this is the PC - broadcast these values to the network.
    SetNetworkLocalFloat(NetworkID,"PlayerX",plrx#)
    SetNetworkLocalFloat(NetworkID,"PlayerY",plry#)
    SetNetworkLocalFloat(NetworkID,"PlayerZ",plrz#)
endif

//MAIN LOOP
Do
    if RunningPC=0
		//Use the Android device to get the rotational vector of the player's head.
		
        //get the rotational Quaterion vector from the device
        RotSensorW# = GetRawRotationVectorW2()
        RotSensorX# = GetRawRotationVectorX2()
        RotSensorY# = GetRawRotationVectorY2()
        RotSensorZ# = GetRawRotationVectorZ2()

        //Broadcast the values to the network.
        SetNetworkLocalFloat(NetworkID,"RotQuatX",RotSensorX#)
        SetNetworkLocalFloat(NetworkID,"RotQuatY",RotSensorY#)
        SetNetworkLocalFloat(NetworkID,"RotQuatZ",RotSensorZ#)
        SetNetworkLocalFloat(NetworkID,"RotQuatW",RotSensorW#)

        //get player's position from the network (Broadcast by the PC)
        plrx# = GetNetworkClientFloat(NetworkID,HostID,"PlayerX")
        plrz# = GetNetworkClientFloat(NetworkID,HostID,"PlayerZ")

    else
		//Use the PC to get the player's position
		
        //move player with joystick
        if GetJoystickExists()=1
            //strife
            plrx# = plrx# + (GetRawJoystickX(1)*50.0*cos(Angle_Y#)*getframetime())
            plrz# = plrz# - (GetRawJoystickX(1)*50.0*sin(Angle_Y#)*getframetime())

            //forward
            plrx# = plrx# + (GetRawJoystickY(1)*50.0*sin(-Angle_Y#)*getframetime())
            plrz# = plrz# - (GetRawJoystickY(1)*50.0*cos(-Angle_Y#)*getframetime())

			//Broadcast player's new position
            SetNetworkLocalFloat(NetworkID,"PlayerX",plrx#)
            SetNetworkLocalFloat(NetworkID,"PlayerZ",plrz#)
            
        else
            //move player with keyboard

            //Right - D
            plrx# = plrx# + (GetRawKeyState(68)*50.0*cos(Angle_Y#)*getframetime())
            plrz# = plrz# - (GetRawKeyState(68)*50.0*sin(Angle_Y#)*getframetime())

            //Left - A
            plrx# = plrx# - (GetRawKeyState(65)*50.0*cos(Angle_Y#)*getframetime())
            plrz# = plrz# + (GetRawKeyState(65)*50.0*sin(Angle_Y#)*getframetime())

            //foward - W
            plrx# = plrx# - (GetRawKeyState(87)*50.0*sin(-Angle_Y#)*getframetime())
            plrz# = plrz# + (GetRawKeyState(87)*50.0*cos(-Angle_Y#)*getframetime())

            //back - S
            plrx# = plrx# + (GetRawKeyState(83)*50.0*sin(-Angle_Y#)*getframetime())
            plrz# = plrz# - (GetRawKeyState(83)*50.0*cos(-Angle_Y#)*getframetime())

			//Broadcast player's new position
            SetNetworkLocalFloat(NetworkID,"PlayerX",plrx#)
            SetNetworkLocalFloat(NetworkID,"PlayerZ",plrz#)
        endif

        //get values of the head's rotation from the network
        RotSensorX# = GetNetworkClientFloat(NetworkID,ClientID,"RotQuatX")
        RotSensorY# = GetNetworkClientFloat(NetworkID,ClientID,"RotQuatY")
        RotSensorZ# = GetNetworkClientFloat(NetworkID,ClientID,"RotQuatZ")
        RotSensorW# = GetNetworkClientFloat(NetworkID,ClientID,"RotQuatW")

    endif

    //set the camera's rotation based on the rotational vector
    SetCameraRotationQuat(1,RotSensorW#,RotSensorX#,RotSensorY#,RotSensorZ#)

    //Obtain the head angle from the camera
    Angle_X# = GetCameraAngleX(1)
    Angle_Y# = GetCameraAngleY(1)
    Angle_Z# = GetCameraAngleZ(1)
	
	//Calculate left and right eye positions based on head angle and eye distance
    InPlaneDist# = EyeDistance * cos(Angle_Z#)
    LeftEye.PosY = plry# + (EyeDistance * sin(Angle_Z#))
    RightEye.PosY = plry# - (EyeDistance * sin(Angle_Z#))

    LeftEye.PosX = plrx# - ((InPlaneDist# / 2.0) * cos(Angle_Y#))
    RightEye.PosX = plrx# + ((InPlaneDist# / 2.0) * cos(Angle_Y#))

    LeftEye.PosZ = plrz# - ((InPlaneDist# / 2.0) * sin(Angle_Y#))
    RightEye.PosZ = plrz# + ((InPlaneDist# / 2.0) * sin(Angle_Y#))

    //Quit app if BACK/ESC key is pressed
    if GetRawKeyPressed( 27 )
        if IsNetworkActive(NetworkID)=1
            CloseNetwork(NetworkID)
        endif

        end
    endif
	
	//The next steps use a single camera to create both left and right eye images for the 3D effect.
	
	//update the position of all the objects and camera
    Update(0)

    if RunningPC=0
		//Render both LEFT and rIGTH eyes if running on the Android device
        
        //LEFT EYE
        //position the camera for the left eye
        SetCameraPosition(1,LeftEye.PosX,LeftEye.PosY,LeftEye.PosZ)
        //set the render target of the camera to the LEFT EYE image.
        SetRenderToImage( LeftEye.RenderImage,0 )
        //Clear the old image and render the camera to the target image.
        ClearScreen( )
        Render( )

        //RIGHT EYE
        //position the camera for the right eye
        SetCameraPosition(1,RightEye.PosX,RightEye.PosY,RightEye.PosZ)
        //set the render target of the camera to the RIGHT EYE image.
        SetRenderToImage( RightEye.RenderImage,0 )
        //Clear the old image and render the camera to the target image.
        ClearScreen( )
        Render( )
    else
		//For the PC, position the camera for the left eye.
		//The camera is set to render to the screen by default so the render target does not need to be changed for the PC.
        SetCameraPosition(1,LeftEye.PosX,LeftEye.PosY,LeftEye.PosZ)
    endif

	//Set the render target as the screen (for the PC, this is already the target)
    SetRenderToScreen( )

    if RunningPC=0
		//For the android device, the two rendered images need to be positioned in front of each eye.
		
        //LEFT EYE
        //update Eye sprite with new image
        SetSpriteImage( LeftEye.Sprite,LeftEye.RenderImage)
        //set the sprite size to cover half of the screen.
        SetSpriteSize( LeftEye.Sprite,getvirtualwidth()/2,getvirtualheight()*1.3 )
        //position the sprite in the top left corner
        SetSpritePosition( LeftEye.Sprite,0,0 )
        //make the sprite visible
        SetSpriteVisible( LeftEye.Sprite,1 )

        //RIGHT EYE
        //update Eye sprite with new image
        SetSpriteImage( RightEye.Sprite,RightEye.RenderImage)
        //set the sprite size to cover half of the screen.
        SetSpriteSize( RightEye.Sprite,getvirtualwidth()/2,getvirtualheight()*1.3 )
        //position the sprite in the right half of the screen
        SetSpritePosition( RightEye.Sprite,getvirtualwidth()/2.0,0 )
        //make the sprite visible
        SetSpriteVisible( RightEye.Sprite,1 )
		
		//draw a line between the two images
        drawline(getvirtualwidth()/2.0,0.0,getvirtualwidth()/2.0,720,255,255,255)
    endif
	
	//Render everything to the screen
    Render( )
    //Displays the back buffer to the screen and clears the backbuffer for the next frame, updates global time variables
    Swap( )
	
	//hide the Eye sprites so that the scene can be captured by the camera
    SetSpriteVisible( LeftEye.Sprite,0 )
    SetSpriteVisible( RightEye.Sprite,0 )

Loop


//This code is taken directly from the 3D-FPS demo in AGKv1.
_load_world:
 `
 rem Shaders
 shaderindex=1 : LoadShader(shaderindex,"vertex.vs","pixel.ps")
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
  SetObjectImage(obj,lm[val(lm$)],1)
  SetObjectShader(obj,shaderindex)
  `
  //SetObjectRotation(obj,0,0,90)
  `
  rem Progress status
  perc#=obj : perc#=perc#/objmax : perc=perc#*100
  SetTextString(1,"Metro Theatre - "+str(perc)+"%") : Sync()

 next obj
 `
return
