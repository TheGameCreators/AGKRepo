// show all errors
SetErrorMode(2)

#insert "globals.agc"
#insert "text_backgrounds.agc"
#include "exit_button.agc"
#include "titles.agc"

Init_Text_Backgrounds()


SetOrientationAllowed( 1, 1, 1, 1 ) // allow both portrait and landscape on mobile devices
SetSyncRate( 60, 0 ) // 30fps instead of 60 to save battery
SetScissor( 0,0,0,0 ) // use the maximum available screen space, no black borders
UseNewDefaultFonts( 1 ) // since version 2.0.22 we can use nicer default fonts
SetVirtualResolution( 100, 100 )

dw#=GetDeviceWidth()

display_exit()
maketext()

// create floor
ground = CreateObjectPlane( 100,100 )
SetObjectRotation( ground, 90,0,0 )
SetObjectColor( ground, 128,128,128,255 )

// create marker to show emitter position
CreateObjectSphere( 1, 1, 10, 10 )
SetObjectPosition( 1, 0,1,0 )

freq# = 100 // per second
life# = 5.0 // seconds
additive = 1 // additive or alpha transparent particles

// create 3D particle emitterw
Create3DParticles( 1, 0,0,0 )
Set3DParticlesPosition( 1, 0,1,0 )
Set3DParticlesDirection( 1, 0,3,0, 0 )
Set3DParticlesFrequency( 1, freq# )
Set3DParticlesLife( 1, life# )
Set3DParticlesSize( 1, 2 )
Set3DParticlesVelocityRange( 1, 0.5, 3.0 )
//Set3DParticlesStartZone( 1, -5,-5,-5, 5,5,5 ) // appear anywhere in this box
Set3DParticlesDirectionRange( 1, 180, 180 ) // anywhere in a hemisphere

// two types of transparency, additive and alpha
if ( additive = 1 )
	LoadImage( 1, "particleBlack.png" )
	Set3DParticlesImage( 1, 1 )
	Set3DParticlesTransparency( 1, 2 )
	
	// fade the particles out before they die
	Add3DParticlesColorKeyFrame( 1, life#-1, 0,0,255,255 )
	Add3DParticlesColorKeyFrame( 1, life#, 0,0,0,255 )
else
	LoadImage( 1, "particle.png" )
	Set3DParticlesImage( 1, 1 )
	Set3DParticlesTransparency( 1, 1 )
	
	// fade the particles out before they die
	Add3DParticlesColorKeyFrame( 1, life#-1, 0,0,255,255 )
	Add3DParticlesColorKeyFrame( 1, life#, 0,0,255,0 )
endif

// position the camera to look at the particles
SetCameraPosition( 1, 0,15,-25 )
SetCameraLookAt( 1, 0,1,0, 0 )

// controls setup
startx# = GetPointerX()
starty# = GetPointerY()

// Left joystick to look around
AddVirtualJoystick(1,15,85,20)
// Right joystick to move
AddVirtualJoystick(2,85,85,20)


do

	if dw#<>GetDeviceWidth() then maketext()

	// look around with the left virtual joystick
	RotateCameraLocalX(1,GetVirtualJoystickY(1))
	// keep x axis within certain bounds
	if GetCameraAngleX(1) > 50 then RotateCameraLocalX(1,0-GetVirtualJoystickY(1))
	if GetCameraAngleX(1) < -50 then RotateCameraLocalX(1,0-GetVirtualJoystickY(1))

	RotateCameraLocalY(1,GetVirtualJoystickX(1))
	SetCameraRotation( 1, GetCameraAngleX(1), GetCameraAngleY(1), 0 ) // This is to keep the camera upright

	// Move around with the right virtual joystick
	MoveCameraLocalZ(1,0-GetVirtualJoystickY(2)*0.5)
	MoveCameraLocalX(1,GetVirtualJoystickX(2)*0.5)
	SetCameraPosition(1,getcamerax(1),10,getcameraz(1)) // Keep camera at the same height above the ground

    // control the camera
    if ( GetRawKeyState( 87 ) ) then MoveCameraLocalZ( 1, 0.2 )
	if ( GetRawKeyState( 83 ) ) then MoveCameraLocalZ( 1, -0.2 )

	if ( GetRawKeyState( 65 ) ) then MoveCameraLocalX( 1, -0.2 )
	if ( GetRawKeyState( 68 ) ) then MoveCameraLocalX( 1, 0.2 )

	if ( GetRawKeyState( 81 ) ) then MoveCameraLocalY( 1, -0.2 )
	if ( GetRawKeyState( 69 ) ) then MoveCameraLocalY( 1, 0.2 )

	SetTextString(2,"FPS:"+str(ScreenFPS(),0)) // update the fps displayed under the exit button
	check_exit()

	Sync()
loop






