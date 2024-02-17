
// Project: Baws 
// Created: 2014-10-25

// nice fonts
UseNewDefaultFonts( 1 )

Gosub SetVariables
Gosub SetScreen
Gosub LoadStuff
Gosub MakeText
Gosub MakeBackground
Gosub MakeBaws
Gosub Makecup

MainLoop:
	do
		Gosub Startscreen
		Gosub Game
	loop
	End

Game:
	Do
		Gosub movecup
		Gosub checkbaws
		Gosub Input
		if endofgame=1 then Exit
		Sync()
	loop
	`update Scores
	if Score>Val(HiScore$)
		HiScore$=Str(Score)
		Gosub Savefile
	endif
	
	`cleanup 
`cups
	for temp=1 to 6
		if GetSpriteExists(CupSprite[temp])=1
			SetSpritePhysicsOff(CupSprite[temp])
			DeleteSprite(CupSprite[temp])
		endif
	next temp
`Baws
	for temp=1 to 100
		if GetSpriteExists(BawSprite[temp])=1
			SetSpritePhysicsOff(BawSprite[temp])
			DeleteSprite(BawSprite[temp])
		endif
	next temp
`reset
	cupspeed#=0.4
	For temp=1 to 30
		baw[temp]=1
	Next temp
	Bawcount=30
	Cup = 6
	for temp=1 to 6
		CupAng#[temp]=0
		Cupstatus[temp]=1
	next temp
	totalbaws=100
	endofgame=0
	Gosub Makecup
	Gosub MakeBaws
	
Return

MakeBaws:

`Baws
	for temp=1 to 100
		BawSprite[temp]= CreateSprite( BawImage ) 
		SetSpriteSize(BawSprite[temp],4,-1)
		SetSpriteOffset(BawSprite[Temp],2,(2*Aspect))
		SetSpriteShapeCircle( BawSprite[Temp], 0, 0, 1.9 ) 
	 
	next temp
	
	Count=0
	For sy=1 to 8
		for sx=1 to 4 
			Count=Count+1
			If count<29 then SetSpritePositionByOffset(BawSprite[count],(sx*4)+40,((sy*4)+20)*Aspect)
			if Count=29 or Count=30 then SetSpritePositionByOffset(BawSprite[count],(sx*4)+44,((sy*4)+20)*Aspect)
			if Count<31 then SetSpritePhysicsOn( BawSprite[Count], 2 ) 
			if count<31 then SetSpritePhysicsRestitution( BawSprite[Count], 0.4 )
		next sx
	next sy

	Count=30
	For sy=7 to 1 step -1
		for sx=1 to 5 
			Count=Count+1
			SetSpritePositionByOffset(BawSprite[count],(sx*4)+16,((sy*4)+20)*Aspect)
		next sx
	next sy

	For sy=7 to 1 step -1
		for sx=1 to 5 
			Count=Count+1
			SetSpritePositionByOffset(BawSprite[count],(sx*4)+60,((sy*4)+20)*Aspect)
		next sx
	next sy

Return

MakeBackground:
`effect
	CreateParticles ( 1, -100, -100 )

`Background
	CreateSprite( 1000,1000 ) 
	SetSpriteSize(1000,100,100)
	
`Base 
	BaseSprite= CreateSprite( BaseImage ) 
	SetSpriteSize(BaseSprite,100,-1)
	SetSpritePosition(BaseSprite,0,95)
	SetSpriteShape(BaseSprite,3)
	SetSpritePhysicsOn(BaseSprite, 1 ) 
	
`holder
	LHolderSprite	= CreateSprite( HolderImage ) 
	SetSpriteSize(LHolderSprite,30,-1)
	SetSpritePosition(LHolderSprite,15,32)
	SetSpriteShape(LHolderSprite,3)
	SetSpritePhysicsOn(LHolderSprite, 1 ) 
	RHolderSprite	= CreateSprite( HolderImage ) 
	SetSpriteSize(RHolderSprite,30,-1)
	SetSpritePosition(RHolderSprite,55,32)
	SetSpriteFlip ( RHolderSprite, 1, 0 ) 
	SetSpriteShape(RHolderSprite,3)
	SetSpritePhysicsOn(RHolderSprite, 1 )
	
	LStraitSprite	= CreateSprite( StraitImage ) 
	SetSpriteSize(LStraitSprite,-1,20)
	SetSpritePosition(LStraitSprite,15,12)
	SetSpritePhysicsOn(LStraitSprite, 1 )
	
	RStraitSprite	= CreateSprite( StraitImage ) 
	SetSpriteSize(RStraitSprite,-1,20)
	SetSpritePosition(RStraitSprite,82.8,12)
	SetSpritePhysicsOn(RStraitSprite, 1 )
	
	LtopSprite = CreateSprite( StraitImage ) 
	SetSpriteSize(LtopSprite,-1,15)
	SetSpritePosition(LtopSprite,27.2,4.6)
	SetSpriteAngle(LtopSprite,90)
	SetSpritePhysicsOn(LtopSprite, 1 ) 
	
	RtopSprite = CreateSprite( StraitImage ) 
	SetSpriteSize(RtopSprite,-1,15)
	
	SetSpritePosition(RtopSprite,71,4.6)
	SetSpriteAngle(RtopSprite,90)
	SetSpritePhysicsOn(RtopSprite, 1 ) 	
	
	CLStraitSprite	= CreateSprite( StraitImage ) 
	SetSpriteSize(CLStraitSprite,-1,20)
	SetSpritePosition(CLStraitSprite,38.4,12)
	SetSpritePhysicsOn(CLStraitSprite, 1 )
	
	CRStraitSprite	= CreateSprite( StraitImage ) 
	SetSpriteSize(CRStraitSprite,-1,20)
	SetSpritePosition(CRStraitSprite,58.8,12)
	SetSpritePhysicsOn(CRStraitSprite, 1 )	
	
	`Gate
	LGateSprite = CreateSprite( StraitImage ) 
	SetSpriteSize(LGateSprite,2,4)
	SetSpriteOffset(LGateSprite,1,4)
	SetSpritePosition(LGateSprite,43,46.5)
	SetSpriteAngle(LGateSprite,90)
	SetSpriteShapeBox( LGateSprite, -0.2, 0, 0.2, -3, 0 ) 
	SetSpritePhysicsOn(LGateSprite, 3 ) 	

	RGateSprite = CreateSprite( StraitImage ) 
	SetSpriteSize(RGateSprite,2,4)
	SetSpriteOffset(RGateSprite,1,0)
	SetSpritePosition(RGateSprite,55,50.5)
	SetSpriteAngle(RGateSprite,90)
	SetSpriteShapeBox( RGateSprite, -0.2, 0, 0.2, 3, 0 ) 
	SetSpritePhysicsOn(RGateSprite, 3 ) 
	
return

MakeText:
`maketext
	CreateText(1,"Score : 0")
	SetTextAlignment(1,1)
	SetTextPosition(1,50,55)

	CreateText(2,"Last Score : 0")
	SetTextAlignment(2,1)
	SetTextPosition(2,50,60)

	CreateText(3,"Hi-Score : 0")
	SetTextAlignment(3,1)
	SetTextPosition(3,50,65)

Return

LoadStuff:
`Load Stuff
	LoadImage(1000,"back.jpg")
	BawImage=LoadImage("Ball.png")
	BaseImage=LoadImage("Base.png")
	HolderImage=LoadImage("Holder.png")
	StraitImage=LoadImage("Strait.png")
	CupImage=LoadImage("Cup.png")

	LoadImage(900,"BawTitle.png")
	LoadImage(901,"Music.png")
	LoadImage(902,"On.png")
	LoadImage(903,"Off.png")
	LoadImage(904,"Start.png")
	LoadImage(905,"Quit.png")

	bawmusic=LoadMusicOGG("baws.ogg")

`Check File Exists - if not create It
	If GetFileExists( "hi.txt" )=0
		OpenToWrite(1,"hi.txt")
			WriteString(1,"1,0,0")
		CloseFile(1)
	endif 

`Load Saved Scores and Options
	OpenToRead(1,"hi.txt")
		Data$=ReadString(1)
		MusicOption$=GetStringToken( Data$, ",",1) `music setting
		LastScore$=GetStringToken( Data$, ",",2) `Last Score
		HiScore$=GetStringToken( Data$, ",",3) `HighScore
	CloseFile(1)
	
return

SetScreen:
// set window properties
	`SetPhysicsDebugOn() 
	SetWindowTitle( "Baws by JammySoft" )
	SetWindowSize( 400, 600, 0 )  `for PC
	SetDisplayAspect( Aspect ) 
	SetOrientationAllowed( 1, 1,0,0 ) `only allow portrate view
    SetPhysicsWallBottom(0) `walls not used in this game
    SetPhysicsWallLeft(0)
    SetPhysicsWallRight(0)
    SetPhysicsWallTop(0)
    `SetPhysicsSleeping( 0) `can enable if physics problems occur 
   `SetPhysicsGravity(0,40)
    SetPhysicsCCD( 1 ) ` Continuous Collision Detection helps stop balls falling through cups
	SetPhysicsScale( 0.05 ) 
	
Return

SetVariables:
` Set Variables
	Global Score as Integer
	
	Global endofgame as Integer
	endofgame=0
	
	global Aspect as float
	Aspect=0.625
	Global Count as integer
	Count=0
	Global Level as integer
	Level=1
	Global Start as Integer
	global Bawcount as Integer
	Bawcount=30
	Global totalbaws as Integer
	totalbaws=100
	Dim Baw[100] as integer `state of baw, 0= out of play, 1= ready ,2= falling, 3=cought
	For temp=1 to 30
		baw[temp]=1
	Next temp
	Dim BawSprite[100]
	Dim Cupstatus[6] as Integer `0=cup dead, 1=cup alive no baws cought, 2=cup has cought a ball
	Global Cup as integer
	cup=0
	Dim Cupsprite[6]
	For temp=1 to 6
		Cupsprite[temp]=0
	Next temp
	Dim CupPosition#[6,2] as float
	cupspeed#=0.4
	Dim CupAng#[6] as float
	
return

Startscreen:
	`title
	createsprite(900,900)
	SetSpriteSize(900,90,30)
	SetSpritePosition(900,5,5)
	`music
	createsprite(901,901)
	SetSpriteSize(901,20,8)
	SetSpritePosition(901,5,88)
	`on
	createsprite(902,902)
	SetSpriteSize(902,15,8)
	SetSpritePosition(902,25,88)
	`Start
	CreateSprite(904,904)
	SetSpriteSize(904,35,17)
	SetSpritePosition(904,35,68)
	`quit
	createsprite(905,905)
	SetSpriteSize(905,20,8)
	SetSpritePosition(905,75,88)

	if MusicOption$="1" 
		PlayMusicOGG(bawmusic,1)
		SetSpriteImage(902,902) `changes Music option to either off or on
	else
		SetSpriteImage(902,903)
	endif

	SetTextString(2,"Last Score :"+LastScore$)
	SetTextString(3,"Hi-Score :"+HiScore$)

`StartScreenLoop
	Repeat
		pointx#=GetPointerX()
		Pointy#=GetPointerY()
		if GetPointerReleased()=1
			
			if GetSpriteHitTest ( 901, pointx#,pointy#) =1 or GetSpriteHitTest ( 902, pointx#,pointy#)=1 `music
				if MusicOption$="0" 
					MusicOption$="1"
					SetSpriteImage(902,902)
					PlayMusicOGG(bawmusic,1)
				else 
					MusicOption$="0"
					SetSpriteImage(902,903)
					StopMusicOGG(bawmusic)
				endif
			endif
			
			if GetSpriteHitTest ( 904, pointx#,pointy#) =1 `start
				Start=1
			endif
		
			if GetSpriteHitTest ( 905, pointx#,pointy#) =1 ` Quit
					if Score>0 then LastScore$=Str(Score)
					gosub Savefile `update save file with new lastscore and music options
				end
			endif
	
		endif
		sync()
	Until Start=1
	
	`cleanup StartScreen
	Start=0
	
	For temp=900 to 905
		if GetSpriteExists(Temp)=1 then DeleteSprite(temp)
	next temp
	
	if Score>0 then LastScore$=Str(Score)
	SetTextString(2,"Last Score :"+LastScore$)
	Score=0
	SetTextString(1,"Score : 0")
	gosub Savefile `update save file with new lastscore and music options
	
return

Savefile:

	OpenToWrite(1,"hi.txt")
	Data$=MusicOption$+","+LastScore$+","+HiScore$
	WriteString(1,Data$)
	CloseFile(1)
	
return

checkbaws:

	for temp=1 to 100
		if baw[temp]>0
			bawy#=GetSpriteY( bawsprite[temp] )
			bawx#=GetSpriteX( bawsprite[temp] ) 
			
			if bawy#>55 and bawy#<100 `If ball in play check for collision with cup
				if baw[temp]=1 then	baw[temp]=2 
					if baw[temp]=2 
						for tempcup = 1 to 6						
							bawincup=GetSpriteInBox(BawSprite[temp],CupPosition#[tempcup,1]+2,CupPosition#[tempcup,2]+3,CupPosition#[tempcup,1]+12,CupPosition#[tempcup,2]+10)
							
							if bawincup=1
								Score=Score+1
								SetTextString( 1,"Score : " + str(score) ) 
								baw[temp]=3
								Cupstatus[tempcup]=2
							endif
							
						next tempcup
					endif
				else 
				if bawy#<55 then baw[temp]=1
			endif		
		endif
	
		if bawy#>105 and baw[temp]>0 `check if baw is off screen
			SetSpritePhysicsOff(temp)
			DeleteSprite(BawSprite[temp])
			baw[temp]=0
			totalbaws=totalbaws-1
			if totalbaws<1 then endofgame=1
			if bawcount<100 then bawcount=bawcount+1
			setspritephysicson(bawsprite[bawcount],2)
			baw[bawcount]=1
		endif
	next temp
	
return




movecup:
	for temp=1 to 6
		if Cupstatus[temp]>0
			CupPosition#[temp,1]= GetSpriteX(Cupsprite[temp])
			CupPosition#[temp,2]= GetSpriteY(Cupsprite[temp])
			
`move along the bottom
			if CupPosition#[temp,2] >= 81 
				if CupPosition#[temp,1] >=0.5 
					CupPosition#[temp,1]=CupPosition#[temp,1]-cupspeed#
					if CupPosition#[temp,1] <=0.5 
						CupPosition#[temp,2]=CupPosition#[temp,2]-(cupspeed#/Aspect)
						if Cupstatus[temp]=1 `if cup has no balls destroy it
							Cupstatus[temp]=0
							
							SetParticlesPosition ( 1, CupPosition#[temp,1],CupPosition#[temp,2])
							ResetParticleCount ( 1 )
							SetParticlesFrequency ( 1, 250 )
							SetParticlesLife ( 1, 3.0 )
							SetParticlesSize ( 1, 1.1 )
							SetParticlesStartZone ( 1, 1, 1, 11, 11 )
							SetParticlesImage ( 1, BawImage )
							SetParticlesDirection ( 1, 10, 10 )
							SetParticlesAngle ( 1, 360 )
							SetParticlesVelocityRange ( 1, 0.8, 2.5 )
							SetParticlesMax ( 1, 500 )
							AddParticlesColorKeyFrame ( 1, 0.0, 255, 255, 255, 125 )
							AddParticlesColorKeyFrame ( 1, 0.5, 155, 155, 125, 255 )
							AddParticlesColorKeyFrame ( 1, 2.8, 55, 55, 55, 55 )
							AddParticlesForce ( 1, 2.0, 2.8, 25, -25 )

							setspritephysicsoff (Cupsprite[temp])
							deletesprite(Cupsprite[temp])
							cup=cup-1
							if cup=0 then endofgame=1
						else 
							Cupstatus[temp]=1
						endif
					endif
				endif
			endif
			
`move up the left
			if CupPosition#[temp,1] <=0.5
				if CupPosition#[temp,2] >=0
					CupPosition#[temp,2]=CupPosition#[temp,2]-(cupspeed#*Aspect)
					if CupPosition#[temp,2] <=0 
						CupPosition#[temp,1]=CupPosition#[temp,1]+cupspeed#
					endif
				endif
			endif
		
`move Along the Top
			if CupPosition#[temp,2] <=0 
				if CupPosition#[temp,1] <85.5
					CupPosition#[temp,1]=CupPosition#[temp,1]+cupspeed#
					if CupPosition#[temp,1] >85.5 then CupPosition#[temp,2]=CupPosition#[temp,2]`+(cupspeed#/Aspect)
				
					if CupPosition#[temp,1]<45 `tilt cup
						if CupAng#[temp]<180 then CupAng#[temp]=CupAng#[temp]+10
						if CupAng#[temp]>180 then CupAng#[temp]=180
					endif
				
					if CupPosition#[temp,1]>55
						if CupAng#[temp]>0 then	CupAng#[temp]=CupAng#[temp]+10
						if CupAng#[temp]>360 then CupAng#[temp]=0
					endif
				
				endif	
				SetSpriteAngle(CupSprite[temp],CupAng#[temp])
				if  CupPosition#[temp,1] >85.5 then cupspeed#=cupspeed#+(0.04/cup)			
			endif
		
`move Down the right
			if CupPosition#[temp,1] >=85.5
				if CupPosition#[temp,2] <=81
					CupPosition#[temp,2]=CupPosition#[temp,2]+(cupspeed#*Aspect)
					if CupPosition#[temp,2] >=81 
						CupPosition#[temp,1]=CupPosition#[temp,1]-cupspeed#	
					endif
				endif
			endif
		
			setspriteposition(Cupsprite[temp],CupPosition#[temp,1],CupPosition#[temp,2])
		endif
	next temp
return

Makecup:
	for cup=1 to 6
		CupSprite[Cup]	= CreateSprite( CupImage ) 
		SetSpriteSize(CupSprite[Cup],14,12)
		SetSpriteOffset(CupSprite[Cup],7,5.5)
		SetSpritePosition(CupSprite[Cup],85.5,30+(cup*-45))
		SetSpriteShapeBox( CupSprite[Cup], -6, 5, 6, 6,0)
	
		SetSpritePhysicsOn(CupSprite[Cup], 3 ) 
		AddSpriteShapeBox( CupSprite[Cup], -6, -5,-5,6,0 ) 
		AddSpriteShapeBox( CupSprite[Cup], 6, -5,5,6,0 )
		Cupstatus[Cup]=1
	next cup
	cup=6
return	

Input:
	if GetPointerPressed()=1 or GetRawKeyPressed( 32 )=1
		SetSpriteAngle(RGateSprite,-10)
		SetSpriteAngle(LGateSprite,190)
	endif
	
	if GetPointerReleased()=1 or GetRawKeyReleased( 32 )=1
		SetSpriteAngle(RGateSprite,90)
		SetSpriteAngle(LGateSprite,90)
	endif
return
