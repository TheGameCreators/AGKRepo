
// Project: Catch the balls!
// Created: 2017-01-31

SetErrorMode(2)

// Include files


global exit_button as integer
global dw# as float
dw# = GetDeviceWidth()

#include "exit_button.agc"

// set window properties
SetWindowTitle( "Catch the balls" )
SetWindowSize( 1024, 768, 0 )

// set display properties
SetVirtualResolution( 1024, 768 )
SetOrientationAllowed( 1, 1, 1, 1 )
UseNewDefaultFonts( 1 )
SetScissor(0,0,0,0)
SetRawMouseVisible(0) 
//SetPhysicsDebugOn()

// Fixed values
besttime=0
bat=1
boards=100
ball=2
ball_total=15
Gosub make_sprites

scoreline = CreateText("Game Time: "+str(time)+" / Best Time: "+str(besttime)+" / Number of Balls: "+str(balls))
SetTextSize(scoreline,40)

display_exit() // Show the exit button in top-right

do
//Restart values
ball2move= ball
hitcount = 0
hitforce=-400



ResetTimer()

repeat 
	If GetRawKeyReleased(27) Then End

	if dw# <> GetDeviceWidth() then display_exit()

	Gosub move_bat
	Gosub Shoot_a_ball

	check_exit()

	Sync()

 // After Sync, check to see if any of the balls have hit the bat   
 // This means checking every ball against the bat, so we use a loop
    for i=0 to ball_total-1
		hit=GetPhysicsCollision(bat,ball+i)
		if hit=1
			// Yes this ball has his the bat, apply a negative velocity force to the ball to kick it up
			// Work out where the bat is in relation to the ball, apply a force in the x axis based on this angle.

			batmiddle=getspritex(bat)+(GetSpriteWidth(bat)/2)

			batangle=getspritex(ball+i)+GetSpriteWidth(ball+i)/2

			SetSpritePhysicsVelocity ( ball+i, (batangle-batmiddle)*8, hitforce )

			// Increase the hitforce for the next collision (makes the game harder)
			hitforce=hitforce-5
			
			//Play kick sound
			PlaySound(kick)
		endif
	next i

	//Spin the boards

	SetSpriteAngle(boards,GetSpriteAngle(boards)+1)
	SetSpriteAngle(boards+1,GetSpriteAngle(boards+1)-1)
	time=timer()
	
	
	//Check how many balls are still on screen
	balls=0
	for i=0 to ball_total-1
		if GetSpriteY(ball+i)<768
			balls=balls+1
		endif
	next i
	
	if time>besttime
		besttime=time
	endif
	
	//Print the time and number of balls
	//print ("Game Time: "+str(time)+" / Best Time: "+str(besttime)+" / Number of Balls: "+str(balls))
	SetTextString(scoreline,("Game Time: "+str(time)+" / Best Time: "+str(besttime)+" / Number of Balls: "+str(balls)))
until balls=0


loop




make_sprites:
// Create a red bat image
CreateImageColor(1,255,0,0,255)
CreateSprite(bat,1)
SetSpritesize(bat,100,20)
SetSpritePosition(bat,512,740)

//Turn physics on for this sprite - use mode 3 so it's not affected by forces and collisions (Kinematic)
SetSpritePhysicsOn ( bat, 3 )

// Create "ball_total" of balls 
LoadImage(2,"small_ball.png")
for i=0 to ball_total-1
	CreateSprite(ball+i,2)
	SetSpriteX(ball+i,-100)
	SetSpritePhysicsOn(ball+i,2 )
next i


// Turn off bottom floor
SetPhysicsWallBottom(0)



//Create two rotating boards
// Create a green image
CreateImageColor(3,0,255,0,255)
// Make the left board
CreateSprite(boards,3)
// Set it's size 160 in the x axis, 20 in the y
SetSpritesize(boards,160,20)
// Position on the left side of the screen
SetSpritePosition(boards,40,400)
// Turn on physics and set as a Kinematic object
SetSpritePhysicsOn (boards, 3)

// Make the right side board, similar settings
CreateSprite(boards+1,3)
SetSpritesize(boards+1,160,20)
SetSpritePosition(boards+1,1024-200,400)
SetSpritePhysicsOn (boards+1, 3)

//Load a bounce sound
kick=LoadSound("kick.wav")

Return


Shoot_a_ball:
// If random number out of 60 = 5, then create a ball in the scene
if random(1,10)=5 and ball2move<=ball_total+1
		//Position it in the middle
		SetSpritePosition(ball2move,512,300)
		
		// Make sure it has circular collision (as it's a ball!)
		SetSpriteShapeCircle(ball2move,0,0,22)
		
		// Turn on Physics for this object - dynamic body
		SetSpritePhysicsOn(ball2move,2 )
		
		// Give the ball a random rotation
		SetSpritePhysicsAngularVelocity ( ball2move, random ( 0, 10 ) )
		
		// Apply a mass to the ball
		SetSpritePhysicsMass(ball2move,0.1)
		
		// Apply a random negative force to push the ball up the screen at the start
		SetSpritePhysicsVelocity ( ball2move, 0, -random(90,180) )
		
		ball2move=ball2move+1
endif
Return


move_bat:
// Get the mouse's x position EDIT:(slightly modified for mobile)
batx=GetPointerX()
// Check to ensure bat is not off the right of side of the screen
If batx>1024-GetSpriteWidth(bat)
	batx=1024-GetSpriteWidth(bat)
endif
// Position the bat based on the mouse position
SetSpritePosition(bat,batx,getspritey(bat))
Return



