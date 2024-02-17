
// Project: Bubbles with Types 
// Created: 2017-02-10

// set window properties
SetWindowTitle( "Bubbles with Types" )
SetWindowSize( 640, 960, 0 )

// set display properties
SetVirtualResolution( 640, 960 )
SetOrientationAllowed( 1, 1, 1, 1 )

SetScissor(0,0,0,0)

SetSyncRate( 60, 0 ) 
UseNewDefaultFonts( 1 )

#constant max_bubbles 2

#include "text_backgrounds.agc"
#include "titles.agc"
#include "exit_button.agc"


// Variable setup
score=0
highscore=0
speedup=1
popcount=0
gameover=1

// Create a Type container to deal with the bubbles
type bubble
 x as integer
 y as integer
 speed as integer
 alive as integer
 spriteID as integer
endtype

// Create an array using the bubble Type
bubbles as bubble[max_bubbles]

// Set the values in this Type Array
	for i=1 to max_bubbles
		bubbles[i].x=random(1,500)
		bubbles[i].y=960
		bubbles[i].speed=random(1,5)
		bubbles[i].alive=0
	next i

// Call the Load Media Function
Gosub LoadMedia

//Start the music
PlayMusicOGG(1)

Init_Text_Backgrounds()

// a few global variables to store our text background IDs
global tbg1 as integer
global tbg2 as integer
global tbg3 as integer

// for our exit button sprite ID
global exit_button as integer
// set some global variables that we will use for our virtual resolution display properties
global dw# as float

global score as integer
global highscore as integer

dw#=GetDeviceWidth()

display_exit()

maketext()

do
	
if dw# <> GetDeviceWidth() then maketext()

	if gameover=1
		gameover=0 : score =0
		gosub reset_bubbles
	endif
	
	// Randomly start a bubble if conditions are right
for i=1 to max_bubbles
	if bubbles[i].alive=0
		if random(1,50)=10
			bubbles[i].alive=1
		
		endif
	endif
next i

//Check each bubble and move it if it's alive
    for i=1 to max_bubbles
		if bubbles[i].alive=1
			bubbles[i].y=bubbles[i].y-bubbles[i].speed
			// Check if bubble if off the top of the screen
			if bubbles[i].y<-140
				gameover=1
				popcount=0 : speedup=1
			endif
			SetSpritePosition(bubbles[i].spriteID,bubbles[i].x,bubbles[i].y)
		endif
	next i

// Check to see if the user has tried to pop a bubble
	if GetPointerPressed()=1
		hitsprite=GetSpriteHit(GetPointerX(),GetPointerY())
	endif
	
// Has the player hit a bubble?

	for i=1 to max_bubbles
		if bubbles[i].spriteID=hitsprite
			
			// Yes a hit!
			playsound(1)
			inc score
			
			// Do we speed up the game?
			popcount=popcount+1
			if popcount=5
				inc speedup
				popcount=0
			endif
			
			// Clear the popped bubble
			bubbles[i].alive=0
			bubbles[i].x=random(1,500)
			bubbles[i].y=GetScreenBoundsBottom()
			bubbles[i].speed=random(1+speedup,5+speedup)
			hitsprite=0
			SetSpritePosition(bubbles[i].spriteID,bubbles[i].x,bubbles[i].y)

		endif
	next i

//Update score and hi score text

	SetTextString(4,"Score "+str(score)+"   Hi-Score "+str(highscore))
	DeleteTextBackground(tbg3)
	tbg3 = CreateTextBackground(4,1,8,5,100)
	SetTextBackgroundColor(tbg3,0,0,0,155)

// Update the fps text
	SetTextString(2,"FPS: "+str(ScreenFPS(),0))


// Hi Score check

	if score>highscore
		highscore=score
	endif

// End of music check

	if GetMusicPlayingOGG( 1 )=0
		playmusicogg(1)
	endif

// call function to check if exit button has been pressed
	check_exit()

    Sync()
loop


loadmedia:
	// Load the backdrop image and re-size it to the full window size
	loadimage(1,"background.png")
	createsprite(1,1)
	setspritesize(1,GetScreenBoundsRight()-GetScreenBoundsLeft(), GetScreenBoundsBottom() - GetScreenBoundsTop())
	SetSpritePosition(1,GetScreenBoundsLeft(),GetScreenBoundsTop())
	//Load the bubble image and make the bubbles
	loadimage(2,"bubble.png")
	for i=1 to max_bubbles
		bubbles[i].spriteID=CreateSprite(2)
		randsize=random(60,140)
		setspritesize(bubbles[i].spriteID,randsize,randsize)
		SetSpritePosition(bubbles[i].spriteID,-500,-100)
	next i
	//Load the music and sound
	LoadMusicOGG(1,"Bubbles.ogg")
	loadsound(1,"pop.wav")
Return

reset_bubbles:
// Reset all the bubbles
	for i=1 to max_bubbles
		bubbles[i].x=random(1,500)
		bubbles[i].y=GetScreenBoundsBottom()
		bubbles[i].speed=random(1,5)
		bubbles[i].alive=0
		SetSpritePosition(bubbles[i].spriteID,bubbles[i].x,bubbles[i].y)
	next i
return






