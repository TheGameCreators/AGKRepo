// Simple Penny Falls Demo (not a full game)
// set window properties
SetWindowTitle( "Simple Penny Falls" )
SetWindowSize( 600, 900, 0 )
UseNewDefaultFonts( 1 )
SetSyncRate( 60, 0 ) // 30fps instead of 60 to save battery

#insert "globals.agc"
#insert "text_backgrounds.agc"
#include "titles.agc"
#include "exit_button.agc"

gameover_image = loadimage("GameOver.png")
gameover_sprite = createsprite(gameover_image)

highscores_image = loadimage("HighScores.png")
highscores_sprite = createsprite(highscores_image)

score_image = loadimage("Score.png")
score_sprite = createsprite(score_image)

// Move all these sprites out of view
SetSpritePositionByOffset(gameover_sprite,-1000,0)
SetSpritePositionByOffset(highscores_sprite,-1000,0)
SetSpritePositionByOffset(score_sprite,-1000,0)

// load the hi-score data from the hiscores.json file
hiscorearray.load("hiscores.json")

Init_Text_Backgrounds()

dw#=GetDeviceWidth()
 
// set display properties
SetVirtualResolution( 600, 900 )
SetOrientationAllowed( 1, 1, 0, 0 )
SetScissor(0,0,0,0)

// Tell AppGameKit we are going to be using 3D physics, and tweak the scale to suit our requirements
Create3DPhysicsWorld(80)
  
// Create and position a box (the 'pusher' to our penny falls machine)
machine_pusher = createObjectBox( 600,120,400 )
SetObjectColor( machine_pusher,50,50,255,255)
SetObjectPosition( machine_pusher,0,-15,5)
SetObject3DPhysicsFriction( machine_pusher, 8)

// Let the physics engine know we want this box to interact with the physics world
// This command also sets the physics shape to a box.
// A KinematicBody can interact with dynamic bodies, but will pass through static bodies.
Create3DPhysicsKinematicBody( machine_pusher )

// Another box for the base of our machine
machine_base = createObjectBox( 600,10,900 )
SetObjectColor(machine_base,20,200,200,255)
SetObjectPosition(machine_base,0,-20,100)
Create3DPhysicsStaticBody( machine_base )
SetObjectShapeBox(machine_base)
SetObject3DPhysicsRestitution(machine_base,0.8)
SetObject3DPhysicsFriction(machine_base,0.5)

// a box for the back of our machine
machine_back = createObjectBox( 600,840,20)
SetObjectColor(machine_back,60,222,200,255)   
setobjectposition(machine_back,0,-10,190)
Create3DPhysicsStaticBody( machine_back )
setobjectshapebox(machine_back)
// We don't want our coins to get stuck on the back of the machine so we can make it super slidey
SetObject3DPhysicsFriction(machine_back,0)

// A layer of 'glass' to stop the coins falling forwards into the machine
machine_glass = createObjectBox( 600,330,20)
SetObjectColor(machine_glass,222,222,222,65)
setobjectposition(machine_glass,0,260,155)
Create3DPhysicsStaticBody( machine_glass )
SetObject3DPhysicsFriction(machine_glass,0)
setobjectshapebox(machine_glass)
SetObjectTransparency(machine_glass,1)

// Create a series of pegs to fit behind the glass
toppegs as integer[5]
for t = 0 to 4
	toppegs[t] = CreateObjectCylinder(40,10,10)
	SetObjectPosition(toppegs[t],-200+(t*100),350,180)
	RotateObjectLocalx(toppegs[t],90)
	Create3DPhysicsStaticBody( toppegs[t] )
	SetObject3DPhysicsRestitution(toppegs[t],0.9)
next t

midpegs as integer[4]
for t = 0 to 3
	midpegs[t] = CreateObjectCylinder(40,10,10)
	SetObjectPosition(midpegs[t],-150+(t*100),260,180)
	RotateObjectLocalx(midpegs[t],90)
	Create3DPhysicsStaticBody( midpegs[t] )
	SetObject3DPhysicsRestitution(midpegs[t],0.9)
next t

lowpegs as integer[5]
for t = 0 to 4
	lowpegs[t] = CreateObjectCylinder(40,10,10)
	SetObjectPosition(lowpegs[t],-200+(t*100),170,180)
	RotateObjectLocalx(lowpegs[t],90)
	Create3DPhysicsStaticBody( lowpegs[t] )
	SetObject3DPhysicsRestitution(lowpegs[t],0.9)
next t

  
// The sides
machine_left = CreateObjectBox( 10,840,1000 )
SetObjectColor(machine_left,20,200,200,255)
setobjectposition(machine_left,-300,0,100)
Create3DPhysicsStaticBody( machine_left )
setobjectshapebox(machine_left)
machine_right = CreateObjectBox( 10,840,1000 )
SetObjectColor(machine_right,20,200,200,255)
setobjectposition(machine_right,300,0,100)
Create3DPhysicsStaticBody( machine_right )
setobjectshapebox(machine_right)


// Position the camera so it is facing our penny falls machine
SetCameraPosition( 1, 0,250,-650 )
SetCameraLookAt( 1, 0,10,0, 0 )
SetCameraFOV(1,80)

// Set a few variables that we'll be using in the main loop
pusherpos# 	= 130
speed#		= 55.5
coins		= 1
Wait 		= 0

pusher = CreateTweenCustom(4)

do
	SetScene1() // go to the first scene (game-over and high scores)
	display_exit()
	maketext()

	Wait_For_Click() // Does what it says on the tin

	SetScene2() // go to the 2nd scene (the actual game)
	sync()
	// A flag used to determine whether the game has finished
	gameover = 0
	
	playercoins = 20 // This is how many coins the user starts with
	score = 0
	scoretext = createtext(str(score))
	SetTextSize (scoretext, GetVirtualHeight()*0.1)
	SetTextPosition(scoretext,GetVirtualWidth()/2,GetScreenBoundsTop()+10)

	// pre-load the penny-falls with 50 coins
	for t= 1 to 50

		CreateObjectCylinder( coins, 6,65,14 )
		SetObjectPosition( coins, -300+random(0,600),30+random(1,4),-280+random(0,450) )
		Create3DPhysicsDynamicBody( coins )
		//Print("Angular: "+str(GetObject3DPhysicsAngularSleepingThreshold(coins))+" Liner: "+str(GetObject3DPhysicsLinearSleepingThreshold(coins)))
		setObjectShapeCylinder(coins,1)
		SetObject3DPhysicsRestitution(coins, 0.8)
		SetObject3DPhysicsSleepingThreshold(coins,8,66)
		SetObject3DPhysicsFriction(coins,friction#)
		SetObject3DPhysicsMass(coins,2.0)
		SetObjectColor(coins,random(20,255),random(20,255),random(20,255),255)
		coins = coins + 1
	next t

	toggle_pusher=0 	// setup our pusher toggle flag so the tween know which direction they should be going

	// Create a customer tween used to control the pusher part of the penny-falls machine
	SetTweenCustomFloat1(pusher,5.0,225.0,TweenSmooth1())
	PlayTweenCustom(pusher,0)
	
	while gameover = 0
		if dw# <> GetDeviceWidth() then maketext()
		//New pusher method using custom tweens! Yippee
		if GetTweenCustomPlaying(pusher) = 0
			if toggle_pusher = 0
				toggle_pusher = 1
				SetTweenCustomFloat1(pusher,235.0,5.0,TweenSmooth1())
				PlayTweenCustom(pusher,0)
			else
				toggle_pusher = 0
				SetTweenCustomFloat1(pusher,5.0,235.0,TweenSmooth1())
				PlayTweenCustom(pusher,0)
			endif
		endif
		UpdateAllTweens(GetFrameTime())

		SetObjectPosition(machine_pusher,0,-15,GetTweenCustomFloat1(pusher))

		// Create a new coin every time the user clicks
		if GetPointerPressed()
			CreateObjectCylinder( coins, 6,65,14 )					// Create the coin object
			SetObjectPosition( coins, getpointerx()-300,430,180 )	// position it based on where the player tapped the screen
			RotateObjectLocalX(coins,90)							// rotate the object so it fits into the slot between the back of the machine, and the glass front
			Create3DPhysicsDynamicBody( coins )						// Add the object to the physics engine
			setObjectShapeCylinder(coins,1)							// tell the physics engine what shape the coin is

			SetObject3DPhysicsRestitution(coins, 0.8)				// Restitution is the bounciness of an object.  We don't want our coins to be very bouncy.
			SetObject3DPhysicsSleepingThreshold(coins,3,60) 		// SleepingThreshold deterimines how still an object needs to be before it stops being influenced by the physics engine.
			SetObject3DPhysicsFriction(coins,friction#)				// Friction is how slippery the object is (you need to set the friction on both touching objects)
			SetObject3DPhysicsMass(coins,0.5)						// Set the mass of our coins (they are not very heavy)
			SetObjectColor(coins,random(20,255),random(20,255),random(20,255),255) // Setting the coins to random colours for now - later versions will have proper modelled coins
			coins = coins + 1										// Now that we have created a new coin, we must increment our coin counter to keep track of the object number.
			playercoins = playercoins - 1							// This will be used so we now when the player has run out of coins (not yet implemented)
		endif
		  
		Step3DPhysicsWorld()    // Make the physics engine do its thing

		
		check_exit()

		SetTextString(2,"FPS:"+str(ScreenFPS(),0)) // update the fps
		Sync()

		// This is where we check if any coins have fallen off our game board. 
		// For now we will check every possible coin that has ever been created.
		// This process could possibly be improved using a dynamic array to keep track of all the coins in play
		for t = 1 to coins
			if GetObjectExists(t)
				if getobjecty(t) < -250 // This is a simple test to see if the object has fallen of the penny-falls machine.
					DeleteObject(t) 	// If it has, then we can delete that coin as we no longer want to process it in the physics or render engines
					score=score+1 		// increment the player score 
					SetTextString(scoretext,str(score))	// update the score text
				endif
			endif
		next t

	endwhile

loop

function Wait_For_Click()
	while GetPointerPressed() = 0
		check_exit()
		if dw# <> GetDeviceWidth() then maketext()
		SetTextString(2,"FPS:"+str(ScreenFPS(),0)) // update the fps
		sync()
	endwhile
endfunction

// SetScene1 Will display a Game Over image and hi-score table
Function SetScene1()
	SetSpriteSize(gameover_sprite,GetVirtualWidth()*0.8,GetVirtualHeight()*0.17) 		// Sets the size of the sprite based on resolution so it looks the same on any device
	SetSpritePositionByOffset(gameover_sprite,GetVirtualWidth()/2,GetVirtualHeight()/5) // Position sprite based on virtual resolution so it looks the same on any device
	SetSpriteSize(highscores_sprite,GetVirtualWidth()*0.6,GetVirtualHeight()*0.13)
	SetSpritePositionByOffset(highscores_sprite,GetVirtualWidth()/2,GetVirtualHeight()/3)
	SetSpritePositionByOffset(score_sprite, -1000,0) 									// Position off screen as a simple way to hide it.
	for t=0 to 9 																		// Create the high score table. 10 Scores, with a simple drop-shadow effect with the top score being the biggest
		hs_text[t] = CreateText(str(t+1) + "  ...  " + str(hiscorearray[t])) 
		hs_text_shadow[t] = CreateText(str(t+1) + "  ...  " + str(hiscorearray[t]))
		SetTextSize(hs_text[t],50-(t*3))
		SetTextSize(hs_text_shadow[t],50-(t*3))
		SetTextAlignment(hs_text[t],1) 						// Centre the text
		SetTextAlignment(hs_text_shadow[t],1) 				// Centre the shadow
		SetTextPosition(hs_text[t],GetVirtualWidth()/2,(GetVirtualHeight()*0.40)+t*37)
		SetTextPosition(hs_text_shadow[t],(GetVirtualWidth()/2)+1,((GetVirtualHeight()*0.40)+t*37)+1)
		SetTextColor(hs_text_shadow[t],0,0,0,255) 			// Colour the shadow black
		SetTextDepth(hs_text[t],7) 							// Set the white text depth so it appears in front of the shadow
		SetTextDepth(hs_text_shadow[t],8)					// Set black shadow to appear behind the white text
	next t
endfunction

Function SetScene2()
	SetSpritePositionByOffset(gameover_sprite,-1000,0) 		// hide it off screen
	SetSpritePositionByOffset(highscores_sprite,-1000,0)
	SetSpriteSize(score_sprite,GetVirtualWidth()*0.3,GetVirtualHeight()*0.1)
	SetSpritePosition(score_sprite, GetScreenBoundsLeft()+50, GetScreenBoundsTop()+10)
	for t=0 to 9
		SetTextPosition(hs_text[t],-1000,0) // hide the hi-score table text off screen
		SetTextPosition(hs_text_shadow[t],-1000,0)
	next t
endfunction





