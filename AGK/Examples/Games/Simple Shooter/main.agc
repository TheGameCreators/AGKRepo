
SetVirtualResolution( 768, 1024 )
UseNewDefaultFonts(1)

playerx as float
playery as float
lazerx as float = -100
lazery as float
lazer_fired = 0

enemyx as float
enemyy as float
enemy_direction = 4
enemy_fired=0
gameover=1

score=0
hiscore=0

Gosub loader
Gosub make_stars
Gosub Make_text
Gosub Load_sounds
Gosub Load_Music

do
	if gameover=1
		Gosub MainMenu
		score=0
	endif
    
	Gosub PlayerMove
	Gosub Player_shoots
	Gosub Enemy_move
	Gosub Enemy_Shoot

	Gosub Move_stars


	SetTextString(1,"SCORE: "+str(score))
	if score>hiscore
		hiscore=score
	endif
	SetTextString(3,"HiScore: "+str(hiscore))

	Gosub collision

	if GetRawKeyState(27) = 1
		end
	endif

	If GetRawKeyReleased(27) Then End
    Sync()
loop


loader:

// Player Ship =1
// Player Lazer = 2
// Enemy Ship = 3
// Stars = 5 to 104
// Enemy Bullets = 110 to 114

loadimage(1,"player_ship.png")
loadimage(2,"lazer.png")
loadimage(3,"enemyship.png")
loadimage(4,"enemyfire.png")

// Create the player ship and place at the bottom of the screen
CreateSprite(1,1)
playerx=GetVirtualWidth()/2 - GetSpriteWidth(1)/2
playery=GetVirtualHeight()-GetSpriteHeight(1)

SetSpritePosition(1,playerx,playery)
// Set the collision shape to polygon to improve collision detection
SetSpriteShape(1,3)

//Create the lazer and move it off screen
CreateSprite(2,2)
SetSpritePosition(2,-100,-100)

// Create the enemy ship and place at the top of the screen

CreateSprite(3,3)
SetSpritePosition(3,100,20)
SetSpriteShape(3,3)

// Create 5 enemy bullets
for i=0 to 4
	CreateSprite(110+i,4)
	SetSpriteShape(110+i,3)
next i

Return


PlayerMove:

// Move the ship in the X direction using GetDirectionX()
playerx=playerx+GetDirectionX()*12

// Check if the player ship is at the edge of the left side
if playerx<0
	playerx=0
endif

// Check if the player ship is at the right side of the screen

if playerx>GetVirtualWidth()-GetSpriteWidth(1)
	playerx=GetVirtualWidth()-GetSpriteWidth(1)
endif

// Set the player ship's position
SetSpritePosition(1,playerx,playery)

Return



Player_shoots:

// Check if player has fired
if  GetPointerPressed()=1 and lazer_fired=0
	playsound(lazer)
	lazer_fired=1
	lazerx=GetSpriteX(1)+GetSpriteWidth(1)/2-GetSpriteWidth(2)/2
	lazery=getspriteY(1)-40
endif

// If lazer has been shot, move it up the screen
if lazer_fired=1
	lazery=lazery-12
endif

//If the lazer has moved off the screen, hide it
If lazery<-GetSpriteHeight(2)
	lazer_fired=0
endif
	

// Set the position of the lazer
SetSpritePosition(2,lazerx,lazery)

return



Enemy_move:
// Move enemy down the screen at same rate
enemyy=enemyy+1


//Move the enemy left or right
If enemy_direction=4 and enemyx>GetVirtualWidth()-GetSpriteWidth(3)
	enemy_direction=-4
endif

//enemyx=enemyx+enemy_direction

If enemy_direction=-4 and enemyx<0
	enemy_direction=4
endif

enemyx=enemyx+enemy_direction

SetSpritePosition(3,enemyx,enemyy)

return


Enemy_Shoot:
if enemy_fired=0
	if random(1,100)=5
		enemy_fired=1
		for i=0 to 4
			SetSpritePosition(110+i,getspritex(3)+60,getspritey(3)+70)
		next i
	endif
endif

if enemy_fired=1
		SetSpritePosition(110,getspritex(110)-3,getspritey(110)+3)
		SetSpritePosition(111,getspritex(111)-1,getspritey(111)+3)
		SetSpritePosition(112,getspritex(112),getspritey(112)+3)
		SetSpritePosition(113,getspritex(113)+1,getspritey(113)+3)
		SetSpritePosition(114,getspritex(114)+3,getspritey(114)+3)
endif

if getspritey(110)>1024
	enemy_fired=0
endif
Return



// Check sprite collisions
collision:

if GetSpriteCollision( 2, 3 ) =1

	playsound(explosion)
	score=score+10
	enemyx=100: enemyy=-50
	lazer_fired=0
	lazerx=-50 :lazery=-50
endif


//Check to see if any of the enemy bullets have hit the player ship
for i=0 to 4
	if GetSpriteCollision(110+i,1) =1
		playsound(explosion)
		gameover=1
		SetSpritePosition(110,-100,-100)
		SetSpritePosition(111,-100,-100)
		SetSpritePosition(112,-100,-100)
		SetSpritePosition(113,-100,-100)
		SetSpritePosition(114,-100,-100)
		enemyx=100: enemyy=-50


	endif
next i

return





Make_stars:
	CreateImageColor(5,255,255,255,255)
	for i=1 to 100
		CreateSprite(i+5,5)
		SetSpriteSize(i+5,2,2)
		SetSpritePosition(i+5,random(1,768),random(1,1024))
	next i
return

Move_stars:
for i=1 to 100
	StarY=GetSpriteY(5+i)+2
	If StarY>1024
		StarY=0
	EndIf
	SetSpritePosition(i+5,GetSpritex(i+5),StarY)
next i
Return



Make_text:
CreateText(1,"SCORE: "+str(score))
SetTextSize(1,60)
SetTextPosition(1,0,0)
Return




Load_sounds:
explosion=loadsound("explosion.wav")
lazer=loadsound("lazer.wav")
Return

Load_Music:
music=LoadMusicOgg("bensound-scifi.ogg")
PlayMusicOgg(music)
Return




MainMenu:

Createtext(2,"Simple Shooter")
SetTextSize(2,100)
SetTextPosition(2,(768-GetTexttotalwidth(2))/2,180)

Createtext(3,"High Score: ")
SetTextSize(3,60)
SetTextPosition(3,768-GetTexttotalwidth(3),0)

Createtext(4, "Fire to start game")
SetTextSize(4,60)
Settextposition(4,(768-GetTexttotalwidth(4))/2,900)

gosub Hidegamesprites
gosub showmenutext

repeat 
	if GetRawKeyState(27) = 1
		end
	endif
	sync()
until GetPointerPressed()=1 

gameover=0
gosub Showgamesprites
gosub Hidemenutext

Return

// Player Ship =1
// Player Lazer = 2
// Enemy Ship = 3
// Stars = 5 to 104
// Enemy Bullets = 110 to 114

Hidegamesprites:
for i=1 to 114
	SetSpriteVisible(i,0)
next i
Return

Showgamesprites:
for i=1 to 114
	SetSpriteVisible(i,1)
next i
Return

Hidemenutext:
SetTextVisible(2,0)
SetTextVisible(4,0)
Return

Showmenutext:
SetTextVisible(2,1)
SetTextVisible(4,1)
Return
