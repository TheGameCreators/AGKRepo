Rem Project: Puzzle Me This
Rem Created: 2014-10-17
Rem Programming: Sean Regan
Rem Artwork: Brendan Regan

Rem set window properties
SetScreenResolution( 1280, 720 )
SetWindowTitle( "Puzzle Me This" )

Rem set display properties
SetOrientationAllowed( 0, 0, 1, 1 )
SetVirtualResolution( 1280, 720 )
SetScissor( 0, 0, 0, 0 )

#Constant YELLOW_COLOR MakeColor( 255, 255, 0 )

Rem Loaded flags - These flags will tell us if we have loaded certain sprites yet (that way we don't reload them)
Global MENU_LOADED : MENU_LOADED = 0
Global GAME_LOADED : GAME_LOADED = 0

Rem *For Information on Variables, Arrays, and User Defined Types read here: http://www.appgamekit.com/documentation/principles/1_datatypes.htm

Rem menuType - *User Defined Type (UDT for short) 
Type menuType
	titleId    		as integer
	cameraId   		as integer
	uploadId   		as integer
	backdropId 	    as integer
	panelId	        as integer
	blackBackdropId as integer
	cropAcceptId	as integer
	cropCancelId    as integer
Endtype
Rem decalre a *variable - "menu" as menuType - Ensure it is global so we can access it from anywhere
Global menu as menuType

Rem shakeType - Type used to hold variables necessary for detecting a shake (for scrambling puzzle) 
Type shakeType
	lastX      as float 
	lastY      as float 
	lastZ 	   as float
	lastTime   as integer
	timeOffset as integer
Endtype
Global shake as shakeType

Rem extension of gameType - Holds the Ids for the panel sprites
Type panelType
	leftId    as integer
	rightId   as integer
	bottomId  as integer
	topId	  as integer
	cornerId  as integer
Endtype
Rem extension of gameType - Holds the sprite Ids for the difficulty popup window
Type difficultyType
	backdropId as integer
	choiceId   as integer[ 3 ]
Endtype
Type gameType
	panel  	       as panelType
	menuId 	       as integer
	scrambleId     as integer
	puzzlePlateId  as integer
	difficulty	   as difficultyType
	puzzleBorderId as integer
	blurbId		   as integer
	timeId		   as integer
	bestTimeId	   as integer
	textBestTimeId as integer
	textTimeId 	   as integer
	startTime	   as float
	winBackdropId  as integer
	winMenuId	   as integer
	winImgId	   as integer
	winTimeImgId   as integer
	endTime	   	   as float
	winParticlesId as integer
	starImgId 	   as integer
	bestTime  	   as float[ 3 ]
Endtype
Rem decalre a *variable - "game" as gameType - Ensure it is global so we can access it from anywhere
Global game as gameType

Rem soundType - Holds the ids for our sounds
Type soundType
	clickId   as integer
	shuffleId as integer
	slideId   as integer
	successId as integer
Endtype
Global sound as soundType

Rem puzzleIconType - *User Defined Type (UDT for short) 
Type puzzleIconType
	iconId   as integer
	borderId as integer
Endtype
Rem declare a *variable - "puzzleIconMax" - Used to track the size of the puzzleIcon[] array - Ensure it is global so we can access it from anywhere
Global puzzleIconMax : puzzleIconMax = 8
Rem declare an *array - "puzzleIcon[]" as puzzleIconType - This array will hold Sprite IDs for the puzzle icons on the main menu
Dim puzzleIcon[ puzzleIconMax ] as puzzleIconType

Rem this will hold the Ids of the Sprite pieces
Dim puzzlePiece[ 0,0 ]
Rem this will hold the Id of the Sprite piece that is currently within its grid
Dim puzzleGrid[ 0,0 ]

Rem Global variable to hold the id of the completed puzzle
Global fullPuzzleId : fullPuzzleId = 0
Rem Global variable for the overall puzzle size
Global puzzleSize# : puzzleSize# = 500.0
Global puzzleCompleted : puzzleCompleted = 0

Rem Global variables used for controlling the puzzle pieces
Global pieceSelectedX : pieceSelectedX = -1
Global pieceSelectedY : pieceSelectedY = -1
Global pieceEmptyX : pieceEmptyX = 0
Global pieceEmptyY : pieceEmptyY = 0
Global pieceDivisor : pieceDivisor = 0
Global pieceOffsetX# : pieceOffsetX# = 0.0
Global pieceOffsetY# : pieceOffsetY# = 0.0
Global pieceAxis : pieceAxis = 0
Global pieceStartX# : pieceStartX# = 0.0
Global pieceStartY# : pieceStartY# = 0.0

Global timeFont : timeFont = LoadImage( "game/time_font.png" )

Rem Load in all of our sounds
LoadSounds( )

Rem load in the main menu
LoadMainMenu( )


`fullPuzzleId = 4
`LoadGame( )
`PlayAnimation( 3 )

Do
	Rem if we don't have a puzzle, we should be in the menu
	if fullPuzzleId = 0
		fullPuzzleId = ControlMenu( )
		if fullPuzzleId > 0
			puzzleCompleted = 0
			
			Rem load in the game field
			LoadGame( )
			
			Rem Wait for the player to select their difficulty
			Rem offset by 2 to get the right diviser
			Rem Easy = 1 (+2 = 3 pieces), Medium = 2 (+2 = 4 pieces), Hard = 3 (+2 = 5 pieces)
			pieceDivisor = ControlDifficulty( ) + 2
			
			SetBestTime( )
			
			CreatePuzzlePieces( fullPuzzleId, GetSpriteX( game.puzzlePlateId ) + 33, GetSpriteY( game.puzzlePlateId ) + 33 )
			
			game.startTime = Timer( )
		endif
	else
		Rem Player in main game

		Rem if player shook their device, scramble the puzzle
		if CheckForShake( ) = 1 then ScramblePuzzle( GetSpriteX( game.puzzlePlateId ) + 33, GetSpriteY( game.puzzlePlateId ) + 33 )
		
		Rem Check if the player has completed the puzzle
		if CheckForWin( ) = 1
			if puzzleCompleted = 0
				Rem grab the time here so we know exactly how long it took
				game.endTime = Timer( )
				
				Rem show the completed puzzle
				SetSpriteDepth( fullPuzzleId, 9 )
				SetSpriteVisible( fullPuzzleId, 1 )
				
				Rem Hide the last piece that sits off to the side
				SetSpriteVisible( puzzlePiece[ pieceDivisor, pieceDivisor ], 0 )
				
				PlaySound( sound.successId )
				
				CreateStarParticles( )
				
				Rem check for a new best time
				totalTime# = Floor( game.endTime - game.startTime )
				if game.bestTime[ pieceDivisor - 2 ] > 0 and totalTime# < game.bestTime[ pieceDivisor - 2 ] or Floor( game.bestTime[ pieceDivisor - 2 ] ) <= 0
					Rem set the new best time
					game.bestTime[ pieceDivisor - 2 ] = totalTime#
					Rem choose appropriate win prompt
					SetSpriteImage( game.winBackdropId, game.winTimeImgId )
					
					Rem write the new time to the file
					f = OpenToWrite( "best_time.dat" )
					For b = 1 to 3
						WriteFloat( f, game.bestTime[ b ] )
					Next b
					CloseFile( f )
				else
					Rem choose appropriate win prompt
					SetSpriteImage( game.winBackdropId, game.winImgId )
				endif
				
				PlayAnimation( 6 )
				
				puzzleCompleted = 1
			endif
			
			Rem main menu button
			if GetPointerPressed( )
				clickedId = GetSpriteHit( GetPointerX( ), GetPointerY( ) )
				if clickedId = game.winMenuId
					PlaySound( sound.clickId )
					
					CleanupPuzzle( )
					DeleteParticles( game.winParticlesId )
					
					Rem Fly out the game field
					PlayAnimation( 4 )
					
					Rem run the menu fly in animation Id = 1
					LoadMainMenu( )
				endif
			endif
		else
			Rem update the time text
			UpdateGameTimer( )
			
			Rem control the sliding of the puzzle
			ControlPuzzle( )
			
				Rem control the Main Menu and Scramble buttons
			ControlGameButtons( )
		endif
	endif
	
	Sync( )
Loop

Function ControlPuzzle( )
	Rem if a puzzle piece isn't currently selected
	if pieceSelectedX = -1
		Rem if the player clicks their pointer
		if GetPointerPressed( )
			Rem Get the sprite that was clicked on
			clickedId = GetSpriteHit( GetPointerX( ), GetPointerY( ) )

			Rem see if the sprite that was clicked is one of the ones that surrounds our empty space
			Rem check piece to the left
			if pieceEmptyX - 1 > 0
				Rem if clicking this piece, and it's next to the empty space call SelectPiece to select it
				if puzzleGrid[ pieceEmptyX - 1, pieceEmptyY ] = clickedId then SelectPiece( pieceEmptyX - 1, pieceEmptyY, 1 )
			endif
			Rem check piece to the right
			if pieceEmptyX + 1 <= pieceDivisor
				Rem if clicking this piece, and it's next to the empty space call SelectPiece to select it
				if puzzleGrid[ pieceEmptyX + 1, pieceEmptyY ] = clickedId then SelectPiece( pieceEmptyX + 1, pieceEmptyY, 1 )
			endif
			Rem check piece above
			if pieceEmptyY - 1 > 0
				Rem if clicking this piece, and it's next to the empty space call SelectPiece to select it
				if puzzleGrid[ pieceEmptyX, pieceEmptyY - 1 ] = clickedId then SelectPiece( pieceEmptyX, pieceEmptyY - 1, 2 )
			endif
			Rem check piece below
			if pieceEmptyY + 1 <= pieceDivisor
				Rem if clicking this piece, and it's next to the empty space call SelectPiece to select it
				if puzzleGrid[ pieceEmptyX, pieceEmptyY + 1 ] = clickedId then SelectPiece( pieceEmptyX, pieceEmptyY + 1, 2 )
			endif
		endif
	else
		Rem A PIECE IS SELECTED
		
		Rem move where the pointer drags it
		Rem offset by pieceOffset, we do this so we can move it from this point (or else the piece would "jump" to the mouse location)
		moveX# = GetPointerX( ) + pieceOffsetX#
		moveY# = GetPointerY( ) + pieceOffsetY#
		
		Rem bounds checking - Ensure the player can't move the puzzle piece out of bounds
		Rem Moving along the X axis (left to right)
		if pieceAxis = 1
			Rem make sure we can't slide through another piece or off of the puzzle area
			Rem if the piece to the right is the empty piece
			if pieceSelectedX + 1 = pieceEmptyX 
				Rem if we are trying to move the wrong direction stop the piece
				if moveX# < pieceStartX#
					moveX# = pieceStartX#
				elseif moveX# - pieceStartX# > GetSpriteWidth( puzzleGrid[ pieceSelectedX, pieceSelectedY ] )
					Rem if we are trying to move to far past the empty space stop the piece
					moveX# = pieceStartX# + GetSpriteWidth( puzzleGrid[ pieceSelectedX, pieceSelectedY ] )
				endif
			endif
			Rem if the piece to the left is the empty piece
			Rem if the piece to the left is the empty piece
			if pieceSelectedX - 1 = pieceEmptyX 
				Rem if we are trying to move the wrong direction stop the piece
				if moveX# > pieceStartX# 
					moveX# = pieceStartX#
				elseif pieceStartX# - moveX# > GetSpriteWidth( puzzleGrid[ pieceSelectedX, pieceSelectedY ] )
					Rem if we are trying to move to far past the empty space stop the piece
					moveX# = pieceStartX# - GetSpriteWidth( puzzleGrid[ pieceSelectedX, pieceSelectedY ] )
				endif
			endif
			
			Rem Set the piece's new position to moveX#
			SetSpriteX( puzzleGrid[ pieceSelectedX, pieceSelectedY ], moveX# )
		else
			Rem Moving along the Y axis (Up and Down)
			
			Rem make sure we can't slide through another piece or off of the puzzle area
			Rem if the piece above is the empty piece
			if pieceSelectedY + 1 = pieceEmptyY
				Rem if we are trying to move the wrong direction stop the piece
				if moveY# < pieceStartY# 
					moveY# = pieceStartY#
				elseif moveY# - pieceStartY# > GetSpriteHeight( puzzleGrid[ pieceSelectedX, pieceSelectedY ] )
					Rem if we are trying to move to far past the empty space stop the piece
					moveY# = pieceStartY# + GetSpriteHeight( puzzleGrid[ pieceSelectedX, pieceSelectedY ] )
				endif
			endif
			Rem if the piece below is the empty piece
			if pieceSelectedY - 1 = pieceEmptyY
				Rem if we are trying to move the wrong direction stop the piece
				if moveY# > pieceStartY# 
					moveY# = pieceStartY#
				elseif pieceStartY# - moveY# > GetSpriteHeight( puzzleGrid[ pieceSelectedX, pieceSelectedY ] )
					Rem if we are trying to move to far past the empty space stop the piece
					moveY# = pieceStartY# - GetSpriteHeight( puzzleGrid[ pieceSelectedX, pieceSelectedY ] )
				endif
			endif
			
			Rem Set the piece's new position to moveY#
			SetSpriteY( puzzleGrid[ pieceSelectedX, pieceSelectedY ], moveY# )
		endif
		
		Rem if the player releases their pointer, deselect the piece
		Rem snap the selected piece into a space and update our array accordingly
		if GetPointerState( ) = 0
			PlaySound( sound.slideId )
			
			Rem Moving along the X axis (left to right)
			if pieceAxis = 1
				Rem if the piece is mostly in the empty space, snap it in and update our array
				if Abs( GetSpriteX( puzzleGrid[ pieceSelectedX, pieceSelectedY ] ) - pieceStartX# ) >= GetSpriteWidth( puzzleGrid[ pieceSelectedX, pieceSelectedY ] ) / 3.5
					finalX# = pieceStartX# + ( GetSpriteWidth( puzzleGrid[ pieceSelectedX, pieceSelectedY ] ) * ( pieceEmptyX - pieceSelectedX ) )
					SetSpriteX( puzzleGrid[ pieceSelectedX, pieceSelectedY ], finalX# )
					MovePieceToEmpty( pieceSelectedX, pieceSelectedY )
				else
					Rem if not, snap it back to where it started
					finalX# = pieceStartX#
					SetSpriteX( puzzleGrid[ pieceSelectedX, pieceSelectedY ], finalX# )
				endif
			else
				Rem Moving along the Y axis (Up and Down)
				
				Rem if the piece is mostly in the empty space, snap it in and update our array
				if Abs( GetSpriteY( puzzleGrid[ pieceSelectedX, pieceSelectedY ] ) - pieceStartY# ) >= GetSpriteHeight( puzzleGrid[ pieceSelectedX, pieceSelectedY ] ) / 3.5
					finalY# = pieceStartY# + ( GetSpriteHeight( puzzleGrid[ pieceSelectedX, pieceSelectedY ] ) * ( pieceEmptyY - pieceSelectedY ) )
					SetSpriteY( puzzleGrid[ pieceSelectedX, pieceSelectedY ], finalY# )
					MovePieceToEmpty( pieceSelectedX, pieceSelectedY )
				else
					Rem if not, snap it back to where it started
					finalY# = pieceStartY#
					SetSpriteY( puzzleGrid[ pieceSelectedX, pieceSelectedY ], finalY# )
				endif
				
			endif
			
			Rem set these back to -1, which means nothing is selected
			pieceSelectedX = -1
			pieceSelectedY = -1
		endif
	endif
Endfunction

Function UpdateGameTimer( )
	Rem update the text string
	SetTextString( game.textTimeId, ConvertTime( Floor( Timer( ) - game.startTime ) ) )
Endfunction

Function SetBestTime( )
	Rem set the best time text string
	SetTextString( game.textBestTimeId, ConvertTime( game.bestTime[ pieceDivisor - 2 ] ) )
Endfunction

Function ConvertTime( timeInSeconds# as float )
	Rem convert from seconds to minutes:seconds
	Rem divide by 60 since there are 60 seconds in a minute. (90 seconds / 60.0 = 1.5)
	conversion# = ( timeInSeconds# / 60.0 )
	Rem Truncate conversion# so we can get the amount of minutes from it
	Rem 90 seconds / 60.0 = 1.5, Trunc( 1.5 ) = 1
	minutes = Trunc( conversion# )
	Rem subtract the minutes from the conversion# to find whatevers left and multiply by 60.0 to get the amount of seconds
	Rem 90 seconds / 60.0 = 1.5 - 1 = 0.5, 0.5 * 60.0 = 30 seconds 
	seconds = ( conversion# - minutes ) * 60.0
	Rem if our seconds are less than 10, add a zero (so 1 minute and 9 seconds will say 1:09 rather than 1:9)
	zero$ = ""
	if seconds < 10 then zero$ = "0"
	
	time$ = Str( minutes ) + ":" + zero$ + Str( seconds )
Endfunction time$

Function CheckForWin( )
	Rem see if the player has completed the puzzle
	
	For x = 1 to pieceDivisor
		For y = 1 to pieceDivisor
			if x <> pieceDivisor or y <> pieceDivisor
				if puzzleGrid[ x, y ] <> puzzlePiece[ x, y ] then exitfunction 0
			endif
		Next y
	Next x
Endfunction 1

Function ControlGameButtons( )
	if GetPointerPressed( )
		Rem Get the sprite that was clicked on
		clickedId = GetSpriteHit( GetPointerX( ), GetPointerY( ) )
		
		Rem main menu button selected
		if clickedId = game.menuId
			PlaySound( sound.clickId )
			
			CleanupPuzzle( )
			Rem Fly out the game field
			PlayAnimation( 4 )
			
			Rem run the menu fly in animation Id = 1
			LoadMainMenu( )
		elseif clickedId = game.scrambleId
			PlaySound( sound.clickId )
			
			ScramblePuzzle( GetSpriteX( game.puzzlePlateId ) + 33, GetSpriteY( game.puzzlePlateId ) + 33 )
		endif
		
	endif
Endfunction

Function MovePieceToEmpty( pieceX as integer, pieceY as integer )
	puzzleGrid[ pieceEmptyX, pieceEmptyY ] = puzzleGrid[ pieceX, pieceY ]
	puzzleGrid[ pieceX, pieceY ] = 0
	pieceEmptyX = pieceX
	pieceEmptyY = pieceY
Endfunction

Function SelectPiece( pieceX as integer, pieceY as integer, axis as integer )
	Rem fill our global variables with the piece that the player has selected
	pieceSelectedX = pieceX
	pieceSelectedY = pieceY
	
	Rem record the position the piece started at
	pieceStartX# = GetSpriteX( puzzleGrid[ pieceX, pieceY ] )
	pieceStartY# = GetSpriteY( puzzleGrid[ pieceX, pieceY ] )
	
	Rem find the pointer offset from the puzzle piece
	Rem we do this so we can move it from this point (or else the piece would "jump" to the mouse location)
	pieceOffsetX# = pieceStartX# - GetPointerX( )
	pieceOffsetY# = pieceStartY# - GetPointerY( )
	
	Rem fill out global variable. This will tell us which axis to move on (1 for X and 2 for Y)
	pieceAxis = axis
Endfunction

Function CreatePuzzlePieces( puzzleId as integer, startX# as float, startY# as float )
	SetSpritePosition( puzzleId, 0, 0 )
	SetSpriteVisible( puzzleId, 1 )
	SetSpriteDepth( puzzleId, 1 )
	
	Rem setup the array that will hold the Sprite Ids for our puzzle pieces 
	UnDim puzzlePiece[ ]
	Dim puzzlePiece[ pieceDivisor, pieceDivisor ]
	Rem this will hold the Id of the Sprite piece that is currently within its grid
	UnDim puzzleGrid[ ]
	Dim puzzleGrid[ pieceDivisor, pieceDivisor ]
	
	Rem render the puzzle image to the screen
	Render( )
	
	Rem puzzleSize (currently 500 ) divided by the amount of pieces acrross will tell use how big each piece is
	Rem If the player chooses meduim mode, it would me 500.0 / 4 which is = 125x125 pixels for each puzzle piece 
	stepValue# = ( puzzleSize# / pieceDivisor )
	
	Rem For loop up, down, left and right of the image using GetImage to take "snapshots" of the puzzle pieces
	For x = 0 to pieceDivisor - 1
		For y = 0 to pieceDivisor - 1
			Rem We are starting our index at 1, so offset the x and y accordingly
			pieceX = x + 1
			pieceY = y + 1
			
			Rem this will take a "snapshot" of the puzzle piece which we can then turn into a sprite
			pieceImg = GetImage( x * stepValue#, y * stepValue#, stepValue#, stepValue# )
			puzzlePiece[ pieceX, pieceY ] = CreateSprite( pieceImg )
			
			Rem Set the piece size just in case it isn't perfectly size
			SetSpriteSize( puzzlePiece[ pieceX, pieceY ], stepValue#, stepValue# )
			SetSpritePosition( puzzlePiece[ pieceX, pieceY ], startX# + x * stepValue#, startY# + y * stepValue# )
			SetSpriteDepth( puzzlePiece[ pieceX, pieceY ], 10 )
			
			Rem The last piece is invisible
			if pieceDivisor - 1 = x and pieceDivisor - 1 = y
				SetSpritePosition( puzzlePiece[ pieceX, pieceY ], GetSpriteX( game.panel.rightId ) - stepValue#, GetSpriteY( game.panel.bottomId ) - stepValue# - 10 )
				`SetSpriteAngle( puzzleGrid[ pieceX, pieceY ], 15 )
				`SetSpriteVisible( puzzleGrid[ pieceX, pieceY ], 0 )
			endif
		Next y
	Next x
	
	Rem Make the full puzzle image invisible
	SetSpriteVisible( puzzleId, 0 )
	Rem Set its position to the startX# and startY# we'll use this later when the player wins
	SetSpritePosition( puzzleId, startX#, startY# )
	Rem set its depth layer back so it doesn't interfere with GetSpriteHit
	SetSpriteDepth( puzzleId, 900 )
	
	Rem position the border for the puzzle
	SetSpritePosition( game.puzzleBorderId, startX# - 6, startY# - 6 )
	Rem set the border depth so it's below our puzzle pieces
	SetSpriteDepth( game.puzzleBorderId, 11 )
	Rem show the border
	SetSpriteVisible( game.puzzleBorderId, 1 )
	
	Rem scramble the puzzle
	ScramblePuzzle( startX#, startY# )
Endfunction

Function ScramblePuzzle( startX# as float, startY# as float )
	if GetSoundsPlaying( sound.shuffleId ) = 0 then PlaySound( sound.shuffleId )
	
	scrambleMax = 750
	scrambleCount = 0
	
	pieceEmptyX = pieceDivisor
	pieceEmptyY = pieceDivisor
	
	Rem set the puzzleGrid[ ] = puzzlePiece. puzzlePiece[ ] holds the sprite Ids in the correct (completed puzzle) order
	Rem puzzleGrid[ ] will tell us which piece is currently in its grid
	For x = 1 to pieceDivisor
		For y = 1 to pieceDivisor
			puzzleGrid[ x, y ] = puzzlePiece[ x, y ]
		Next y
	Next x
	Rem set the last piece to be empty
	Rem Grid space is empty when puzzleGrid[ ] = 0
	puzzleGrid[ pieceEmptyX, pieceEmptyY ] = 0
	
	emptyX = pieceDivisor
	emptyY = pieceDivisor
	
	moveX = 0
	moveY = 0
	
	Rem Repeat atleast scrambleMax amount of times
	Repeat
		
		Rem Repeat until we have a movement direction that doesn't go our of bounds
		Repeat
			Rem this will help ensure that Random( ) doesn't keep giving us the same numbers
			RandomizeSeed( )
			
			Rem choose to move either left or right
			moveX = Random( 0, 2 ) - 1
			moveY = 0
			Rem if neither, move on the Y axis
			if moveX = 0 then moveY = Random( 0, 2 ) - 1
			
			Rem Tell AGK to update itself so the application doesn't freeze while we are mixing up the puzzle
			Update( 0.0 )
		Until emptyX + moveX <= pieceDivisor and emptyY + moveY <= pieceDivisor and emptyX + moveX > 0 and emptyY + moveY > 0 
		
		Rem move the piece omtp the empty space
		puzzleGrid[ emptyX, emptyY ] = puzzleGrid[ emptyX + moveX, emptyY + moveY ]
		puzzleGrid[ emptyX + moveX, emptyY + moveY ] = 0
		Rem update the empty variables
		emptyX = emptyX + moveX
		emptyY = emptyY + moveY
	
		Rem Tell AGK to update itself so the application doesn't freeze while we are mixing up the puzzle
		Update( 0.0 )
		
		Rem increase the scramble count
		scrambleCount = scrambleCount + 1
	Until scrambleCount >= scrambleMax and puzzleGrid[ pieceDivisor, pieceDivisor ] = 0 and CheckForWin( ) = 0
	
	Rem puzzleSize (currently 500 ) divided by the amount of pieces acrross will tell use how big each piece is
	Rem If the player chooses meduim mode, it would me 500.0 / 4 which is = 125x125 pixels for each puzzle piece 
	stepValue# = ( puzzleSize# / pieceDivisor )

	Rem loop through each puzzle piece and position it according to its new grid space
	For x = 0 to pieceDivisor - 1
		For y = 0 to pieceDivisor - 1
			if puzzleGrid[ x + 1, y + 1 ] <> 0
				SetSpritePosition( puzzleGrid[ x + 1, y + 1 ], startX# + x * stepValue#, startY# + y * stepValue# )
			endif
		Next y
	Next x
Endfunction

Function CleanupPuzzle( )
	Rem Delete the puzzle and all of its pieces
	DeleteSprite( fullPuzzleId )
	For x = 1 to pieceDivisor
		For y = 1 to pieceDivisor
			if  puzzlePiece[ x, y ] <> 0
				if GetSpriteExists(  puzzlePiece[ x, y ] ) then DeleteSprite(  puzzlePiece[ x, y ] )
				 puzzlePiece[ x, y ] = 0
			endif
		Next y
	Next x
	
	Rem hide the border and set its depth back to 900
	SetSpriteDepth( game.puzzleBorderId, 900 )
	SetSpriteVisible( game.puzzleBorderId, 0 )
	
	Rem clear this so the main loop knows we are going back to the main menu
	fullPuzzleId = 0
Endfunction

Function PlayAnimation( animNum as integer )
	Rem setup the animation and being
	Rem tweenTime# holds the amount of time that the animation will run for
	tweenTime# = ExecuteAnimation( Abs( animNum ) )
	Rem get the start time of the tween
	tweenStart# = Timer( )
	
	Rem loop and update tweens until we reach the tweenTime#
	Repeat
		UpdateAllTweens( GetFrameTime( ) )
		Sync( )
	Until ( Timer( ) - tweenStart# > tweenTime# + 0.1 )
	
	Rem cleanup the tween
	CleanupAnimation( animNum )
Endfunction

Function ControlMenu( )
	puzzleId = 0
		
	Rem if the user has pressed the "pointer" (Left mouse click on a computer - Screen touch on a touchscreen device)
	if GetPointerPressed( )
		Rem Check GetSpriteHit - returns the Id of the Sprite at the give X and Y cordinates
		Rem In this case, we check the Sprite at the X and Y of the pointer (where the user clicked)
		pointerClicked = GetSpriteHit( GetPointerX( ),GetPointerY( ) )
		
		Rem check to see if the user has clicked on a puzzle icon
		For p = 1 to puzzleIconMax
			Rem check both the puzzle icon and the puzzle border (Just in case it picked up the border instead of the icon)
			if pointerClicked = puzzleIcon[ p ].iconId or pointerClicked = puzzleIcon[ p ].borderId
				PlaySound( sound.clickId )
				
				Rem create a new sprite from the puzzleIcon image
				Rem can't use CloneSprite here because puzzleIcon is scaled down so use GetSpriteImageID to get the original image Id
				puzzleId = CreateSprite( GetSpriteImageID( puzzleIcon[ p ].iconId ) )
				exit
			endif
		Next p
		
		Rem if the user clicks the "Upload From Device" button
		if pointerClicked = menu.uploadId
			PlaySound( sound.clickId )
			
			Rem Call up the choose image screen
			hasChooseScreen = ShowChooseImageScreen( )		
			Rem Ensure that the device has a choose image screen (and the app has been given permission)	
			if hasChooseScreen > 0

				Rem Loop while the player is choosing their image 
				While IsChoosingImage( )
					Sync( )
				Endwhile
				
				Rem Get the image that the user chose
				newImg = GetChosenImage( )
				if newImg > 0
					Rem images need to be sqaure for our puzzle - Bring up crop image screen
					puzzleId = CropImage( newImg )
				endif
			endif
			Rem Elseif the user clicks the "Take A Picture" button
		elseif pointerClicked = menu.cameraId
			PlaySound( sound.clickId )
			
			Rem Call up the camera
			hasCamera = ShowImageCaptureScreen( )
			Rem Ensure that the device has a camera
			if hasCamera > 0
				
				Rem Loop while the player is taking a picture
				While IsCapturingImage( )
					Sync( )
				Endwhile
				
				Rem get the image that the user captured
				newImg = GetCapturedImage( )
				if newImg > 0
					Rem images need to be sqaure for our puzzle - Bring up crop image screen
					puzzleId = CropImage( newImg )
				endif
			endif
		endif
	endif
	
	Rem puzzle has been selected, fly out menu
	if puzzleId > 0
		Rem size the puzzle image to the puzzle size
		SetSpriteSize( puzzleId, puzzleSize#, puzzleSize# )
		Rem hide the sprite for now
		SetSpriteVisible( puzzleId, 0 )
		
		Rem run the menu fly out animation Id = 2
		PlayAnimation( 2 )
	endif
Endfunction puzzleId

Function ControlDifficulty( )
	difficultySelected = 0
	
	Rem loop until player selects a difficulty
	Repeat
		if GetPointerPressed( )
			Rem Get the sprite Id of the clicked sprite
			clickedId = GetSpriteHit( GetPointerX( ), GetPointerY( ) )
			
			Rem loop through our three difficulty buttons when the pointer is clicked
			For c = 1 to 3
				Rem see if a difficulty was selected
				if clickedId = game.difficulty.choiceId[ c ] 
					PlaySound( sound.clickId )
					
					Rem if so, set flag and exit the for loop
					difficultySelected = c
					exit
				endif
			Next c
		endif
		
		Sync( )
	Until difficultySelected <> 0

	Rem fly the difficulty screen out
	PlayAnimation( 5 )
Endfunction difficultySelected

Function CropImage( fullImg as integer )
	Rem image can't be smaller than puzzleSize# or else it would stretch
	Rem if the player trys to use an image that is to small, tell them they can't
	if GetImageWidth( fullImg ) < puzzleSize# or GetImageHeight( fullImg ) < puzzleSize#
		Message( "That picture is too small, please choose a different one." )
		DeleteImage( fullImg )
		exitfunction 0
	endif
	
	Rem Make the black backdrop visible and make it the top layer
	Rem Ensure we set its depth back to 900 once we're done with it or else it will interfere with GetSpriteHit!
	SetSpriteDepth( menu.blackBackdropId, 3 )
	SetSpriteVisible( menu.blackBackdropId, 1 )
	
	SetSpriteDepth( menu.cropAcceptId, 2 )
	SetSpriteVisible( menu.cropAcceptId, 1 )
	SetSpriteDepth( menu.cropCancelId, 2 )
	SetSpriteVisible( menu.cropCancelId, 1 )
	
	Rem turn the fullImg Image into a sprite and give it a depth of 1 so it sits ontop of the black backdrop
	fullSpr = CreateSprite( fullImg )
	SetSpriteDepth( fullSpr, 2 )
	
	scaledSpr = CloneSprite( fullSpr )
	Rem Ensure we set its depth back to 900 once we're done with it or else it will interfere with GetSpriteHit!
	SetSpriteDepth( fullSpr, 900 )
	SetSpriteVisible( fullSpr, 0 )

	newAspect# = SpriteScaleToMax( scaledSpr, GetVirtualWidth( ), GetVirtualHeight( ) - 200 )

	Rem Limit the size of the crop box to the smallest dimension of the image, Add 2 to account for the 2 pixel border.
	Rem This will ensure the user can only crop out a square image
	if GetSpriteWidth( scaledSpr ) > GetSpriteHeight( scaledSpr )
		cropSize# = GetSpriteHeight( scaledSpr )
	else
		cropSize# = GetSpriteWidth( scaledSpr )
	endif
	
	Rem position the new scaledSpr to the center of the screen for cropping. Leave 100 pixels on top and bottom for buttons
	SetSpritePositionByOffset( scaledSpr, GetVirtualWidth( ) / 2.0, ( GetVirtualHeight( ) - 200 ) / 2.0 + 100 )
	
	croppedSpr = 0
	boxPosX# = 0
	boxPosY# = 0
	Repeat
		
		Rem check if either crop or cancel were pressed
		clickedId = GetSpriteHit( GetPointerX( ), GetPointerY( ) )
		if GetPointerPressed( )
			Rem crop was pressed
			if clickedId = menu.cropAcceptId
				PlaySound( sound.clickId )
				
				Rem find the unscaled position (so we can crop out the full resolution image for maximum resolution)
				cropX# = ( boxPosX# - GetSpriteX( scaledSpr ) ) / newAspect#
				cropY# = ( boxPosY# - GetSpriteY( scaledSpr ) ) / newAspect#
				cropSizeFull# = cropSize# / newAspect#
				
				Rem hide the scaled sprite - We aren't snapshotting it
				SetSpriteVisible( scaledSpr, 0 )

				Rem Show the full resolution Sprite - This is the one we are snapshotting
				SetSpriteDepth( fullSpr, 1 )
				SetSpriteVisible( fullSpr, 1 )
				
				Rem Make sure that the image fits within the screen area - Cannot use GetImage outside of the screen bounds
				Rem If the image is to big, scale it down just small enough to fit
				fitAspect# = SpriteScaleToMax( fullSpr, GetVirtualWidth( ), GetVirtualHeight( ) )

				Rem render the fullSpr image to the screen so we can crop it
				Render( )
				
				Rem Use GetImage to take a "snapshot" of our fullSpr
				Rem offset the crop coordinates according to how we scaled the sprite 
				Rem If the fullSpr Sprite was scaled down, the crop coordinates should too
				croppedImg = GetImage( cropX# * fitAspect#, cropY# * fitAspect#, cropSizeFull# * fitAspect#, cropSizeFull# * fitAspect# )
				croppedSpr = CreateSprite( croppedImg )
				SetSpriteDepth( croppedSpr, 1 )
				SetSpriteVisible( croppedSpr, 0 )
				
				Rem We cropped our image, so leave the Repeat loop
				exit
			elseif clickedId = menu.cropCancelId
				PlaySound( sound.clickId )
				
				Rem Cancel was pressed, so leave the Repeat loop
				exit
			endif
		endif
		
		Rem if the player is touching the screen, move the box accordingly
		if GetPointerState( ) = 1
			Rem Set this to the new pointer X, Offset by cropSize# / 2.0 so we move the box by its center
			boxPosX# = GetPointerX( ) - cropSize# / 2.0
			Rem Set this to the new pointer Y, Offset by cropSize# / 2.0 so we move the box by its center
			boxPosY# = GetPointerY( ) - cropSize# / 2.0
			
			Rem ensure the box does not go out of bounds
			Rem Make sure it can't go to high
			if boxPosY# < GetSpriteY( scaledSpr ) then boxPosY# = GetSpriteY( scaledSpr )
			Rem Make sure it can't go to low
			if boxPosY# + cropSize# > GetSpriteY( scaledSpr ) + GetSpriteHeight( scaledSpr ) then boxPosY# = GetSpriteY( scaledSpr ) + GetSpriteHeight( scaledSpr )- cropSize#
			Rem Make sure it can't go to far left
			if boxPosX# < GetSpriteX( scaledSpr ) then boxPosX# = GetSpriteX( scaledSpr )
			Rem Make sure it can't go to far right
			if boxPosX# + cropSize# > GetSpriteX( scaledSpr ) + GetSpriteWidth( scaledSpr ) then boxPosX# = GetSpriteX( scaledSpr ) + GetSpriteWidth( scaledSpr ) - cropSize#
		endif
		
		Rem use the DrawBox command to draw the yellow box outline for the crop area
		DrawBox( boxPosX#, boxPosY#, boxPosX# + cropSize#, boxPosY# + cropSize#, YELLOW_COLOR, YELLOW_COLOR, YELLOW_COLOR, YELLOW_COLOR, 0  )
		
		Sync( )
	Until croppedSpr > 0
	
	
	Rem Delete the sprites that we no longer need and hide the black backdrop
	DeleteSprite( scaledSpr )
	DeleteSprite( fullSpr )
	DeleteImage( fullImg )
	Rem Also make sure we put our depths back to 900 so they don't interfere with GetSpriteHit 
	SetSpriteDepth( menu.blackBackdropId, 900 )
	SetSpriteVisible( menu.blackBackdropId, 0 )
	SetSpriteDepth( menu.cropAcceptId, 900 )
	SetSpriteVisible( menu.cropAcceptId, 0 )
	SetSpriteDepth( menu.cropCancelId, 900 )
	SetSpriteVisible( menu.cropCancelId, 0 )
Endfunction croppedSpr

Function SpriteScaleToMax( scaleId as integer, maxWidth# as float, maxHeight# as float )
	Rem Check if the Sprite is bigger than the max resolution if so, we need to scale it down
	if GetSpriteWidth( scaleId ) > maxWidth# or GetSpriteHeight( scaleId ) > maxHeight#
		Rem find the aspect ratio of the width (maxWidth# / SpriteWidth)
		widthAspect# = maxWidth# / GetSpriteWidth( scaleId )
		Rem find the aspect ratio of the height (maxHeight# / SpriteHeight)
		heightAspect# = ( maxHeight# ) / GetSpriteHeight( scaleId )
		
		Rem Which ever aspect ratio is smaller will be our scale value
		if widthAspect# > heightAspect#
			newAspect# = heightAspect#
		else
			newAspect# = widthAspect#
		endif
		
		Rem Scale the Sprite down to the smaller aspect ratio
		Rem We use the same aspect ratio for the width and height so the Sprite will scale down without stretching
		SetSpriteScale( scaleId, newAspect#, newAspect# )
	else
		Rem no need to scale, image is already the right scale
		newAspect# = 1.0
	endif
Endfunction newAspect#

Function ExecuteAnimation( animNumber as integer )
	
	Rem This is where we setup our animations which uses tweening
	Rem Each animation has an "ID" number
	Remstart
	
	Animation Numbers:
		ID 1 = Menu fly In
		ID 2 = Menu fly Out
	Remend
	Select animNumber
		Rem Menu fly in
		Case 1:
			Rem Aniamtion settings
			Rem Animation type - See "Tweening" section of AGK documentation for more info
			animType = TweenEaseOut1( )
			Rem Speed of animation (In terms of seconds)
			animSpeed# = 2.8
			Rem Animation delay (In terms of seconds)
			animDelay# = 0.0
			
			Rem Create the tweens using Ids 1 to 8 
			For t = 1 to 8
				if GetTweenExists( t ) = 0 then CreateTweenSprite( t, animSpeed# )
			Next t
			
			Rem Everything in this animation sits ontop of the "panel"
			Rem This *variable holds the off screen position (where it will fly in from) of the panel
			Rem Everything that sits on the panel will be offset accordingly
			panelStartY# = GetScreenBoundsTop( ) - ( GetVirtualHeight( ) - GetSpriteY( menu.panelId ) ) - 500
			
			Rem We can use GetSpriteY( ) for the "animate to" parameter because we positioned the sprite in the LoadMenu( ) function
			
			Rem Y position tween for the panel - Animate from panelStartY# position to GetSpriteY( ) position
			SetTweenSpriteY( 1, panelStartY#, GetSpriteY( menu.panelId ), animType )
			
			Rem Y position tween for the top row of icons - Animate from panelStartY# + GetSpriteY( ) position to GetSpriteY( ) position
			SetTweenSpriteY( 2, panelStartY# + GetSpriteY( puzzleIcon[ 1 ].iconId ), GetSpriteY( puzzleIcon[ 1 ].iconId ), animType )
			
			Rem Y position tween for the bottom row of icons - Animate from panelStartY# + GetSpriteY( ) position to GetSpriteY( ) position
			SetTweenSpriteY( 3, panelStartY# + GetSpriteY( puzzleIcon[ 5 ].iconId ), GetSpriteY( puzzleIcon[ 5 ].iconId ), animType )
			
			Rem Y position tween for the top row of icon borders - Animate from panelStartY# + GetSpriteY( ) position to GetSpriteY( ) position
			SetTweenSpriteY( 4, panelStartY# + GetSpriteY( puzzleIcon[ 1 ].borderId ), GetSpriteY( puzzleIcon[ 1 ].borderId ), animType )
			
			Rem Y position tween for the bottom row of icon borders - Animate from panelStartY# + GetSpriteY( ) position to GetSpriteY( ) position
			SetTweenSpriteY( 5, panelStartY# + GetSpriteY( puzzleIcon[ 5 ].borderId ), GetSpriteY( puzzleIcon[ 5 ].borderId ), animType )
			
			Rem Y position tween for the "Select An Option" Sprite - Animate from panelStartY# + GetSpriteY( ) position to GetSpriteY( ) position
			SetTweenSpriteY( 6, panelStartY# + GetSpriteY( menu.titleId ), GetSpriteY( menu.titleId ), animType )
			
			Rem Y position tween for the "Upload From Deivce" Sprite - Animate from panelStartY# + GetSpriteY( ) position to GetSpriteY( ) position
			SetTweenSpriteY( 7, panelStartY# + GetSpriteY( menu.uploadId ), GetSpriteY( menu.uploadId ), animType )
			
			Rem Y position tween for the "Take A Picture" Sprite - Animate from panelStartY# + GetSpriteY( ) position to GetSpriteY( ) position
			SetTweenSpriteY( 8, panelStartY# + GetSpriteY( menu.cameraId ), GetSpriteY( menu.cameraId ), animType )
			
			Rem Begin playing the animations
			PlayTweenSprite( 1, menu.panelId, animDelay# )
			For p = 1 to puzzleIconMax
				Rem Ceil( ) will round a decimal to the nearst integer. For example Ceil( 0.2 ) = 1
				Rem We use Ceil( ) here to mathematically find the correct tween Id number
				PlayTweenSprite( Ceil( p / 4.0 ) + 1, puzzleIcon[ p ].iconId, animDelay# )
				PlayTweenSprite( Ceil( p / 4.0 ) + 3, puzzleIcon[ p ].borderId, animDelay# )
			Next p
			PlayTweenSprite( 6, menu.titleId, animDelay# )
			PlayTweenSprite( 7, menu.uploadId, animDelay# )
			PlayTweenSprite( 8, menu.cameraId, animDelay# )
			
			Rem this is the sprite Id that PlayAnimation will check to see if the tween has finished
			maxTweenTime# = animSpeed# + animDelay#
		Endcase
		Rem Menu fly out
		Case 2:
			Rem Aniamtion settings
			Rem Animation type - See "Tweening" section of AGK documentation for more info
			animType = TweenEaseIn1( )
			Rem Speed of animation (In terms of seconds)
			animSpeed# = 2.8
			Rem Animation delay (In terms of seconds)
			animDelay# = 0.0
			
			Rem Create the tweens using Ids 1 to 8 
			For t = 1 to 8
				if GetTweenExists( t ) = 0 then CreateTweenSprite( t, animSpeed# )
			Next t
			
			Rem Everything in this animation sits ontop of the "panel"
			Rem This *variable holds the off screen position (where it will fly out to) of the panel
			Rem Everything that sits on the panel will be offset accordingly
			panelStartY# = GetScreenBoundsTop( ) - ( GetVirtualHeight( ) - GetSpriteY( menu.panelId ) ) - 500
			
			Rem Y position tween for the panel - Animate from GetSpriteY( ) (Current Position) position to panelStartY# position
			SetTweenSpriteY( 1, GetSpriteY( menu.panelId ), panelStartY#, animType )
			
			Rem Y position tween for the top row of icons - Animate from GetSpriteY( ) (Current Position) position to panelStartY# + GetSpriteY( ) position
			SetTweenSpriteY( 2, GetSpriteY( puzzleIcon[ 1 ].iconId ), panelStartY# + GetSpriteY( puzzleIcon[ 1 ].iconId ), animType )
			
			Rem Y position tween for the bottom row of icons - Animate from GetSpriteY( ) (Current Position) position to panelStartY# + GetSpriteY( ) position
			SetTweenSpriteY( 3, GetSpriteY( puzzleIcon[ 5 ].iconId ), panelStartY# + GetSpriteY( puzzleIcon[ 5 ].iconId ), animType )
			
			Rem Y position tween for the top row of icon borders - Animate from GetSpriteY( ) (Current Position) position to panelStartY# + GetSpriteY( ) position
			SetTweenSpriteY( 4, GetSpriteY( puzzleIcon[ 1 ].borderId ), panelStartY# + GetSpriteY( puzzleIcon[ 1 ].borderId ), animType )
			
			Rem Y position tween for the bottom row of icon borders - Animate from GetSpriteY( ) (Current Position) position to panelStartY# + GetSpriteY( ) position
			SetTweenSpriteY( 5, GetSpriteY( puzzleIcon[ 5 ].borderId ), panelStartY# + GetSpriteY( puzzleIcon[ 5 ].borderId ), animType )
			
			Rem Y position tween for the "Select An Option" Sprite - Animate from GetSpriteY( ) (Current Position) position to panelStartY# + GetSpriteY( ) position
			SetTweenSpriteY( 6, GetSpriteY( menu.titleId ), panelStartY# + GetSpriteY( menu.titleId ), animType )
			
			Rem Y position tween for the "Upload From Device" Sprite - Animate from GetSpriteY( ) (Current Position) position to panelStartY# + GetSpriteY( ) position
			SetTweenSpriteY( 7, GetSpriteY( menu.uploadId ), panelStartY# + GetSpriteY( menu.uploadId ), animType )
			
			Rem Y position tween for the "Take A Picture" Sprite - Animate from GetSpriteY( ) (Current Position) position to panelStartY# + GetSpriteY( ) position
			SetTweenSpriteY( 8, GetSpriteY( menu.cameraId ), panelStartY# + GetSpriteY( menu.cameraId ), animType )
			
			Rem Begin playing the animations
			PlayTweenSprite( 1, menu.panelId, animDelay# )
			For p = 1 to puzzleIconMax
				Rem Ceil( ) will round a decimal to the nearst integer. For example Ceil( 0.2 ) = 1
				Rem We use Ceil( ) here to mathematically find the correct tween Id number
				PlayTweenSprite( Ceil( p / 4.0 ) + 1, puzzleIcon[ p ].iconId, animDelay# )
				PlayTweenSprite( Ceil( p / 4.0 ) + 3, puzzleIcon[ p ].borderId, animDelay# )
			Next p
			PlayTweenSprite( 6, menu.titleId, animDelay# )
			PlayTweenSprite( 7, menu.uploadId, animDelay# )
			PlayTweenSprite( 8, menu.cameraId, animDelay# )
			
			Rem this is the sprite Id that PlayAnimation will check to see if the tween has finished
			maxTweenTime# = animSpeed# + animDelay#
		Endcase
		Rem game field animate in
		Case 3:
			Rem Aniamtion settings
			Rem Animation type - See "Tweening" section of AGK documentation for more info
			animType = TweenEaseOut1( )
			Rem Speed of animation (In terms of seconds)
			animSpeed# = 1.8
			Rem Animation delay (In terms of seconds)
			animDelay# = 0.0
			
			offScreenDist# = 50
			
			Rem Ensure these sprites start off screen
			Rem These sprites are on a timed delay, so we need to position them off screen while we wait for the animation to play
			Rem Otherwise, you would see the sprite sitting on screen
			SetSpritePosition( game.puzzlePlateId, GetSpriteX( game.panel.leftId ) + GetSpriteWidth( game.panel.leftId ) + 20, GetScreenBoundsTop( ) - GetSpriteHeight( game.puzzlePlateId ) - 50 )
			SetSpritePosition( game.difficulty.backdropId, GetScreenBoundsLeft( ) - GetSpriteWidth( game.difficulty.backdropId ) - 50, GetVirtualHeight( ) / 2.0 - GetSpriteHeight( game.difficulty.backdropId ) / 2.0 )
			For c = 1 to 3
				game.difficulty.choiceId[ c ] = CreateSprite( LoadImage( "game/difficulty_" + Str( c ) + ".png" ) )
				SetSpritePosition( game.difficulty.choiceId[ c ], GetSpriteX( game.difficulty.backdropId ) + 170, GetSpriteY( game.difficulty.backdropId ) + 160 + ( GetSpriteHeight( game.difficulty.choiceId[ c ] ) + 10 ) * ( c - 1 ) )
			Next c
			
			
			Rem Create the tweens using Ids 1 to 8 
			For t = 1 to 12
				if GetTweenExists( t ) = 0 then CreateTweenSprite( t, animSpeed# )
			Next t
			For t = 13 to 14
				if GetTweenExists( t ) = 0 then CreateTweenText( t, animSpeed# )
			Next t
			
			Rem X position tween for the left panel - Animate from off the left screen to GetSpriteX( ) (Current Position)
			SetTweenSpriteX( 1, GetScreenBoundsLeft( ) - GetSpriteWidth( game.panel.leftId ) - offScreenDist#, GetSpriteX( game.panel.leftId ), animType )
			
			Rem X position tween for the right panel - Animate from off the right screen to GetSpriteX( ) (Current Position)
			SetTweenSpriteX( 2, GetScreenBoundsRight( ) + offScreenDist#, GetSpriteX( game.panel.rightId ), animType )
			
			Rem Y position tween for the top panel - Animate from off the top of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 3, GetScreenBoundsTop( ) - GetSpriteHeight( game.panel.topId ) - offScreenDist#, GetSpriteY( game.panel.topId ), animType )
			
			Rem Y position tween for the top panel - Animate from off the bottom of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 4, GetScreenBoundsBottom( ) + offScreenDist#, GetSpriteY( game.panel.bottomId ), animType )
			
			Rem X position tween for the corner panel - Animate from off the right of the screen to GetSpriteX( ) (Current Position)
			SetTweenSpriteX( 5, GetScreenBoundsRight( ) + offScreenDist#, GetSpriteX( game.panel.cornerId ), animType )
			Rem Y position tween for the corner panel - Animate from off the bottom of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 5, GetScreenBoundsBottom( ) + offScreenDist#, GetSpriteY( game.panel.cornerId ), animType )
			
			Rem X position tween for the right panel buttons - Animate from off the right screen to GetSpriteX( ) (Current Position)
			SetTweenSpriteX( 6, GetScreenBoundsRight( ) + offScreenDist# + GetScreenBoundsRight( ) - GetSpriteX( game.menuId ), GetSpriteX( game.menuId ), animType )
			
			Rem Y position tween for the puzzle plate - Animate from off the top of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 7, GetSpriteY( game.puzzlePlateId ), GetSpriteY( game.panel.topId ) + 50, animType )
			
			Rem X position tween for the difficulty choice  - Animate from GetSpriteX( ) to the center of the screen
			SetTweenSpriteX( 8, GetSpriteX( game.difficulty.backdropId ), GetVirtualWidth( ) / 2.0  - GetspriteWidth( game.difficulty.backdropId ) / 2.0 , animType )
			
			Rem X position tween for the difficulty choice buttons  - Animate from GetSpriteX( ) to the center of the screen
			SetTweenSpriteX( 9, GetSpriteX( game.difficulty.choiceId[ 1 ] ), GetVirtualWidth( ) / 2.0  - GetspriteWidth( game.difficulty.backdropId ) / 2.0 + 170 , animType )
			
			Rem X position tween for the blurb image  - Animate from off screen to GetSpriteX (current position)
			SetTweenSpriteX( 10, GetScreenBoundsRight( ) + offScreenDist#, GetSpriteX( game.blurbId ) , animType )
			
			Rem Y position tween for the best time sprite - Animate from off the top of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 11, GetScreenBoundsTop( ) - offScreenDist# - GetSpriteHeight( game.bestTimeId ) * 2, GetSpriteY( game.bestTimeId ), animType )
			
			Rem Y position tween for the time sprite - Animate from off the top of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 12, GetScreenBoundsTop( ) - offScreenDist# - GetSpriteHeight( game.timeId ), GetSpriteY( game.timeId ), animType )
			
			Rem Y position tween for the best time text - Animate from off the top of the screen to GetSpriteY( ) (Current Position)
			SetTweenTextY( 13, GetScreenBoundsTop( ) - offScreenDist# - GetTextTotalHeight( game.textBestTimeId ) * 2, GetTextY( game.textBestTimeId ), animType )
			
			Rem Y position tween for the time text - Animate from off the top of the screen to GetSpriteY( ) (Current Position)
			SetTweenTextY( 14, GetScreenBoundsTop( ) - offScreenDist# - GetTextTotalHeight( game.textTimeId ), GetTextY( game.textTimeId ), animType )
			
			Rem Begin playing the animations
			PlayTweenSprite( 1, game.panel.leftId, animDelay# )
			PlayTweenSprite( 2, game.panel.rightId, animDelay# )
			PlayTweenSprite( 3, game.panel.topId, animDelay# )
			PlayTweenSprite( 4, game.panel.bottomId, animDelay# )
			PlayTweenSprite( 5, game.panel.cornerId, animDelay# )
			PlayTweenSprite( 6, game.menuId, animDelay# )
			PlayTweenSprite( 6, game.scrambleId, animDelay# )
			PlayTweenSprite( 7, game.puzzlePlateId, animDelay# + 1.8 )
			PlayTweenSprite( 8, game.difficulty.backdropId, animDelay# + 1.8 * 2 )
			For c = 1 to 3
				PlayTweenSprite( 9, game.difficulty.choiceId[ c ], animDelay# + 1.8 * 2 )
			Next c
			PlayTweenSprite( 10, game.blurbId, animDelay# )
			PlayTweenSprite( 11, game.bestTimeId, animDelay# )
			PlayTweenSprite( 12, game.timeId, animDelay# )
			PlayTweenText( 13, game.textBestTimeId, animDelay# )
			PlayTweenText( 14, game.textTimeId, animDelay# )
			
			Rem this is the sprite Id that PlayAnimation will check to see if the tween has finished
			maxTweenTime# = animSpeed# + animDelay# + 1.8 * 2
		Endcase
		Rem game field animate out
		Case 4:
			Rem Aniamtion settings
			Rem Animation type - See "Tweening" section of AGK documentation for more info
			animType = TweenEaseIn1( )
			Rem Speed of animation (In terms of seconds)
			animSpeed# = 1.8
			Rem Animation delay (In terms of seconds)
			animDelay# = 0.0
			
			offScreenDist# = 50
			
			Rem Create the tweens using Ids 1 to 8 
			For t = 1 to 11
				if GetTweenExists( t ) = 0 then CreateTweenSprite( t, animSpeed# )
			Next t
			For t = 12 to 13
				if GetTweenExists( t ) = 0 then CreateTweenText( t, animSpeed# )
			Next t
			
			Rem X position tween for the left panel - Animate from off the left screen to GetSpriteX( ) (Current Position)
			SetTweenSpriteX( 1, GetSpriteX( game.panel.leftId ), GetScreenBoundsLeft( ) - GetSpriteWidth( game.panel.leftId ) - offScreenDist#, animType )
			
			Rem X position tween for the right panel - Animate from off the right screen to GetSpriteX( ) (Current Position)
			SetTweenSpriteX( 2, GetSpriteX( game.panel.rightId ), GetScreenBoundsRight( ) + offScreenDist#, animType )
			
			Rem Y position tween for the top panel - Animate from off the top of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 3, GetSpriteY( game.panel.topId ), GetScreenBoundsTop( ) - GetSpriteHeight( game.panel.topId ) - offScreenDist#, animType )
			
			Rem Y position tween for the top panel - Animate from off the bottom of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 4, GetSpriteY( game.panel.bottomId ), GetScreenBoundsBottom( ) + offScreenDist#, animType )
			
			Rem X position tween for the corner panel - Animate from off the right of the screen to GetSpriteX( ) (Current Position)
			SetTweenSpriteX( 5, GetSpriteX( game.panel.cornerId ), GetScreenBoundsRight( ) + offScreenDist#, animType )
			Rem Y position tween for the corner panel - Animate from off the bottom of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 5, GetSpriteY( game.panel.cornerId ), GetScreenBoundsBottom( ) + offScreenDist#, animType )
			
			Rem X position tween for the right panel buttons - Animate from off the right screen to GetSpriteX( ) (Current Position)
			SetTweenSpriteX( 6, GetSpriteX( game.menuId ), GetScreenBoundsRight( ) + offScreenDist# + 25, animType )
			
			Rem Y position tween for the puzzle plate - Animate from off the top of the screen to GetSpriteY( ) (Current Position)
			SetTweenSpriteY( 7, GetSpriteY( game.puzzlePlateId ), GetScreenBoundsTop( ) - GetSpriteHeight( game.panel.topId ) - GetSpriteHeight( game.puzzlePlateId ) - offScreenDist# - 50, animType )
			
			Rem X position tween for the blurb image  - Animate from off screen to GetSpriteX (current position)
			SetTweenSpriteX( 8, GetSpriteX( game.blurbId ), GetScreenBoundsRight( ) + offScreenDist#, animType )
			
			Rem Y position tween for the time text - Animate from GetSpriteY( ) (Current Position) to off the top of the screen 
			SetTweenSpriteY( 9, GetSpriteY( game.bestTimeId ), GetSpriteY( game.bestTimeId ) - GetScreenBoundsTop( ) - GetSpriteHeight( game.panel.topId ) - offScreenDist#, animType )
			
			Rem Y position tween for the time text - Animate from GetSpriteY( ) (Current Position) to off the top of the screen 
			SetTweenSpriteY( 10, GetSpriteY( game.timeId ), GetSpriteY( game.timeId ) - GetScreenBoundsTop( ) - GetSpriteHeight( game.panel.topId ) - offScreenDist#, animType )
			
			Rem Alpha fade for win sprite
			SetTweenSpriteAlpha( 11, GetSpriteColorAlpha( game.winBackdropId ), 0, animType )
			
			Rem Y position tween for the time text - Animate from GetSpriteY( ) (Current Position) to off the top of the screen 
			SetTweenTextY( 12, GetSpriteY( game.bestTimeId ), GetSpriteY( game.bestTimeId ) - GetScreenBoundsTop( ) - GetSpriteHeight( game.panel.topId ) - offScreenDist#, animType )
			
			Rem Y position tween for the time text - Animate from GetSpriteY( ) (Current Position) to off the top of the screen 
			SetTweenTextY( 13, GetSpriteY( game.timeId ), GetSpriteY( game.timeId ) - GetScreenBoundsTop( ) - GetSpriteHeight( game.panel.topId ) - offScreenDist#, animType )
			
			Rem Begin playing the animations
			PlayTweenSprite( 1, game.panel.leftId, animDelay# )
			PlayTweenSprite( 2, game.panel.rightId, animDelay# )
			PlayTweenSprite( 3, game.panel.topId, animDelay# )
			PlayTweenSprite( 4, game.panel.bottomId, animDelay# )
			PlayTweenSprite( 5, game.panel.cornerId, animDelay# )
			PlayTweenSprite( 6, game.menuId, animDelay# )
			PlayTweenSprite( 6, game.scrambleId, animDelay# )
			PlayTweenSprite( 7, game.puzzlePlateId, animDelay# )
			PlayTweenSprite( 8, game.blurbId, animDelay# )
			PlayTweenSprite( 9, game.bestTimeId, animDelay# )
			PlayTweenSprite( 10, game.timeId, animDelay# )
			PlayTweenSprite( 11, game.winBackdropId, animDelay# )
			PlayTweenSprite( 11, game.winMenuId, animDelay# )
			PlayTweenText( 12, game.textBestTimeId, animDelay# )
			PlayTweenText( 13, game.textTimeId, animDelay# )
			
			Rem this is the sprite Id that PlayAnimation will check to see if the tween has finished
			maxTweenTime# = animSpeed# + animDelay# `+ 1.8 * 2
		Endcase
		Rem difficulty animate out
		Case 5:
			Rem Aniamtion settings
			Rem Animation type - See "Tweening" section of AGK documentation for more info
			animType = TweenEaseIn1( )
			Rem Speed of animation (In terms of seconds)
			animSpeed# = 1.8
			Rem Animation delay (In terms of seconds)
			animDelay# = 0.0

			Rem Create the tweens using Ids 1 to 8 
			For t = 1 to 2
				if GetTweenExists( t ) = 0 then CreateTweenSprite( t, animSpeed# )
			Next t

			Rem X position tween for the difficulty choice  - Animate from GetSpriteX( ) to the center of the screen
			SetTweenSpriteX( 1, GetSpriteX( game.difficulty.backdropId ), GetScreenBoundsRight( ) + 50 , animType )
			SetTweenSpriteX( 2, GetSpriteX( game.difficulty.backdropId ) + 170, GetScreenBoundsRight( ) + 50 + 170 , animType )
			
			Rem Begin playing the animations
			PlayTweenSprite( 1, game.difficulty.backdropId, animDelay# )
			For c = 1 to 3
				PlayTweenSprite( 2, game.difficulty.choiceId[ c ], animDelay# )
			Next c
			
			
			Rem this is the sprite Id that PlayAnimation will check to see if the tween has finished
			maxTweenTime# = animSpeed# + animDelay#
		Endcase
		Rem win fade in
		Case 6:
			Rem Aniamtion settings
			Rem Animation type - See "Tweening" section of AGK documentation for more info
			animType = TweenEaseIn1( )
			Rem Speed of animation (In terms of seconds)
			animSpeed# = 3.0
			Rem Animation delay (In terms of seconds)
			animDelay# = 0.0

			Rem show the prompts and set their alphas to 0
			SetSpriteColorAlpha( game.winBackdropId, 0 )
			SetSpriteColorAlpha( game.winMenuId, 0 )
			SetSpriteDepth( game.winBackdropId, 2 )
			SetSpriteDepth( game.winMenuId, 1 )
			SetSpriteVisible( game.winBackdropId, 1 )
			SetSpriteVisible( game.winMenuId, 1 )

			Rem Create the tweens using Ids 1 to 8 
			For t = 1 to 1
				if GetTweenExists( t ) = 0 then CreateTweenSprite( t, animSpeed# )
			Next t

			Rem X position tween for the difficulty choice  - Animate from GetSpriteX( ) to the center of the screen
			SetTweenSpriteAlpha( 1, 0, 255 , animType )
			
			Rem Begin playing the animations
			PlayTweenSprite( 1, game.winBackdropId, animDelay# )
			PlayTweenSprite( 1, game.winMenuId, animDelay# )
			
			Rem this is the sprite Id that PlayAnimation will check to see if the tween has finished
			maxTweenTime# = animSpeed# + animDelay#
		Endcase
	Endselect
Endfunction maxTweenTime#

Function CleanupAnimation( animNumber as integer )
	For t = 1 to 14
		if GetTweenExists( t ) then DeleteTween( t )
	Next t
Endfunction

Function LoadGame( )
	Rem load all of our in game assets and set them up
	
	if GAME_LOADED = 0 then game.panel.leftId = CreateSprite( LoadImage( "game/panel_left.png" ) )
	SetSpriteSize( game.panel.leftId, GetSpriteWidth( game.panel.leftId ) + Abs( GetScreenBoundsLeft( ) ),Abs( GetScreenBoundsTop( ) ) + GetScreenBoundsBottom( ) )
	SetSpritePosition( game.panel.leftId, GetScreenBoundsLeft( ), GetScreenBoundsTop( ) )
	SetSpriteDepth( game.panel.leftId, 898 )
	
	if GAME_LOADED = 0 then game.panel.rightId = CreateSprite( LoadImage( "game/panel_right.png" ) )
	SetSpriteSize( game.panel.rightId, GetSpriteWidth( game.panel.rightId ) + ( GetScreenBoundsRight( ) - GetVirtualWidth( ) ),Abs( GetScreenBoundsTop( ) ) + GetScreenBoundsBottom( ) )
	SetSpritePosition( game.panel.rightId, GetScreenBoundsRight( ) - GetSpriteWidth( game.panel.rightId ), GetScreenBoundsTop( ) )
	SetSpriteDepth( game.panel.rightId, 898 )
	
	if GAME_LOADED = 0 then game.panel.bottomId = CreateSprite( LoadImage( "game/panel_bottom.png" ) )
	SetSpriteSize( game.panel.bottomId, Abs( GetScreenBoundsLeft( ) ) + GetScreenBoundsRight( ), GetSpriteHeight( game.panel.bottomId ) )
	SetSpritePosition( game.panel.bottomId, GetScreenBoundsLeft( ), GetScreenBoundsBottom( ) - GetSpriteHeight( game.panel.bottomId ) )
	SetSpriteDepth( game.panel.bottomId, 897 )
	
	if GAME_LOADED = 0 then game.panel.topId = CreateSprite( LoadImage( "game/panel_top.png" ) )
	SetSpriteSize( game.panel.topId, Abs( GetScreenBoundsLeft( ) ) + GetScreenBoundsRight( ), GetSpriteHeight( game.panel.topId ) )
	SetSpritePosition( game.panel.topId, GetScreenBoundsLeft( ), GetScreenBoundsTop( ) )
	SetSpriteDepth( game.panel.topId, 897 )
	
	if GAME_LOADED = 0 then game.panel.cornerId = CreateSprite( LoadImage( "game/panel_corner.png" ) )
	SetSpriteScale( game.panel.cornerId, ( Abs( GetScreenBoundsLeft( ) ) + GetScreenBoundsRight( ) ) / GetVirtualWidth( ), ( Abs( GetScreenBoundsTop( ) ) + GetScreenBoundsBottom( ) ) / GetVirtualHeight( ) )
	SetSpritePosition( game.panel.cornerId, GetScreenBoundsRight( ) - GetSpriteWidth( game.panel.cornerId ), GetSpriteY( game.panel.bottomId ) - 250 )
	SetSpriteDepth( game.panel.cornerId, 899 )
	
	if GAME_LOADED = 0 then game.menuId = CreateSprite( LoadImage( "game/main_menu.png" ) )
	SetSpritePosition( game.menuId, GetSpriteX( game.panel.rightId ) + GetSpriteWidth( game.panel.rightId ) - GetSpriteWidth( game.menuId ), GetSpriteY( game.panel.rightId ) + 100 )
	SetSpriteDepth( game.menuId, 896 )
	
	if GAME_LOADED = 0 then game.scrambleId = CreateSprite( LoadImage( "game/scramble_puzzle.png" ) )
	SetSpritePosition( game.scrambleId, GetSpriteX( game.menuId ) - 20, GetSpriteY( game.menuId ) + GetSpriteHeight( game.menuId ) + 10 )
	SetSpriteDepth( game.scrambleId, 896 )
	
	if GAME_LOADED = 0 then game.puzzlePlateId = CreateSprite( LoadImage( "game/puzzle_background.png" ) )
	SetSpritePosition( game.puzzlePlateId, GetSpriteX( game.panel.leftId ) + GetSpriteWidth( game.panel.leftId ) + 20, GetScreenBoundsTop( ) - GetSpriteHeight( game.puzzlePlateId ) - 50 )
	SetSpriteDepth( game.puzzlePlateId, 896 )
	
	if GAME_LOADED = 0 then game.difficulty.backdropId = CreateSprite( LoadImage( "game/difficulty_backdrop.png" ) )
	SetSpritePosition( game.difficulty.backdropId, GetScreenBoundsLeft( ) - GetSpriteWidth( game.difficulty.backdropId ) - 50, GetVirtualHeight( ) / 2.0 - GetSpriteHeight( game.difficulty.backdropId ) / 2.0 )
	SetSpriteDepth( game.difficulty.backdropId, 895 )
	
	For c = 1 to 3
		if GAME_LOADED = 0 then game.difficulty.choiceId[ c ] = CreateSprite( LoadImage( "game/difficulty_" + Str( c ) + ".png" ) )
		SetSpritePosition( game.difficulty.choiceId[ c ], GetSpriteX( game.difficulty.backdropId ) + 170, GetSpriteY( game.difficulty.backdropId ) + 160 + ( GetSpriteHeight( game.difficulty.choiceId[ c ] ) + 10 ) * ( c - 1 ) )
	Next c
	
	if GAME_LOADED = 0 then game.puzzleBorderId = CreateSprite( LoadImage( "game/puzzle_border.png" ) )
	SetSpritePosition( game.puzzleBorderId, GetSpriteX( game.puzzlePlateId ) + 33, GetSpriteY( game.puzzlePlateId ) + 33 )
	SetSpriteDepth( game.puzzleBorderId, 900 )
	
	if GAME_LOADED = 0 then game.blurbId = CreateSprite( LoadImage( "game/blurb.png" ) )
	SetSpritePosition( game.blurbId, GetSpriteX( game.panel.rightId ) - GetSpriteWidth( game.blurbId ) - 20, GetSpriteY( game.panel.topId ) + GetSpriteHeight( game.panel.topId ) - 10 )
	SetSpriteDepth( game.blurbId, 899 )
	
	if GAME_LOADED = 0 then game.timeId = CreateSprite( LoadImage( "game/time.png" ) )
	SetSpritePosition( game.timeId, GetSpriteX( game.puzzlePlateId ) + GetSpriteWidth( game.puzzlePlateId ) + 10, GetScreenBoundsTop( ) + GetSpriteHeight( game.timeId ) + 20 )
	SetSpriteDepth( game.timeId, 896 )
	
	if GAME_LOADED = 0 then game.bestTimeId = CreateSprite( LoadImage( "game/best_time.png" ) )
	SetSpritePosition( game.bestTimeId, GetSpriteX( game.timeId ), GetScreenBoundsTop( ) + 10 )
	SetSpriteDepth( game.bestTimeId, 896 )
		
	if GAME_LOADED = 0 then game.textBestTimeId = CreateText( "0:00" )
	SetTextString( game.textBestTimeId, "0:00" )
	SetTextPosition( game.textBestTimeId, GetSpriteX( game.bestTimeId ) + GetSpriteWidth( game.bestTimeId ), GetSpriteY( game.bestTimeId ) )
	SetTextFontImage( game.textBestTimeId, timeFont ) 
	SetTextSize( game.textBestTimeId, 32 )
	SetTextDepth( game.textBestTimeId, 896 )
	
	if GAME_LOADED = 0 then game.textTimeId = CreateText( "0:00" )
	SetTextString( game.textTimeId, "0:00" )
	SetTextPosition( game.textTimeId, GetSpriteX( game.bestTimeId ) + GetSpriteWidth( game.timeId ), GetSpriteY( game.timeId ) )
	SetTextFontImage( game.textTimeId, timeFont ) 
	SetTextSize( game.textTimeId, 32 )
	SetTextDepth( game.textTimeId, 896 )
	
	if GAME_LOADED = 0 
		game.winImgId = LoadImage( "game/win_backdrop.png" )
		game.winTimeImgId = LoadImage( "game/win_time_backdrop.png" )
		game.winBackdropId = CreateSprite( game.winImgId )
	endif
	SetSpritePositionByOffset( game.winBackdropId, GetVirtualWidth( ) / 2.0, GetVirtualHeight( ) / 2.0 )
	SetSpriteDepth( game.winBackdropId, 900 )
	SetSpriteVisible( game.winBackdropId, 0 )
	
	if GAME_LOADED = 0 then game.winMenuId = CreateSprite( LoadImage( "game/main_menu_win.png" ) )
	SetSpritePositionByOffset( game.winMenuId, GetSpriteXByOffset( game.winBackdropId ) + 40, GetSpriteYByOffset( game.winBackdropId ) + 150 )
	SetSpriteDepth( game.winMenuId, 900 )
	SetSpriteVisible( game.winMenuId, 0 )
	
	if GAME_LOADED = 0 then game.starImgId = LoadImage( "game/star.png" )
	
	GAME_LOADED = 1
	
	Rem fly in the game
	PlayAnimation( 3 )
Endfunction

Function LoadMainMenu( )
	Rem load all of our menu assets and set them up
	
	LoadPuzzleIcons( )
	
	if MENU_LOADED = 0 then menu.blackBackdropId = CreateSprite( 0 )
	SetSpriteSize( menu.blackBackdropId, Abs( GetScreenBoundsLeft( ) ) + GetScreenBoundsRight( ), Abs( GetScreenBoundsTop( ) ) + GetScreenBoundsBottom( ) )
	SetSpritePosition( menu.blackBackdropId, GetScreenBoundsLeft( ), GetScreenBoundsTop( ) )
	SetSpriteColor( menu.blackBackdropId, 0, 0, 0, 255 )
	SetSpriteVisible( menu.blackBackdropId, 0 )
	SetSpriteDepth( menu.blackBackdropId, 900 )
	
	if MENU_LOADED = 0 then menu.backdropId = CreateSprite( LoadImage( "backdrop.png" ) )
	SetSpriteDepth( menu.backdropId, 900 )
	SetSpriteSize( menu.backdropId, Abs( GetScreenBoundsLeft( ) ) + GetScreenBoundsRight( ), Abs( GetScreenBoundsTop( ) ) + GetScreenBoundsBottom( ) )
	SetSpritePosition( menu.backdropId, GetScreenBoundsLeft( ), GetScreenBoundsTop( ) )
	
	if MENU_LOADED = 0 then menu.panelId = CreateSprite( LoadImage( "center.png" ) )
	SetSpritePosition( menu.panelId, GetScreenBoundsLeft( ), 32 )
	SetSpriteSize( menu.panelId, Abs( GetScreenBoundsLeft( ) ) + GetScreenBoundsRight( ), GetSpriteHeight( menu.panelId ) )
	SetSpriteDepth( menu.panelId, 899 )
	
	if MENU_LOADED = 0 then menu.titleId = CreateSprite( LoadImage( "select.png" ) )
	SetSpritePositionByOffset( menu.titleId, GetVirtualWidth( ) / 2.0, GetSpriteHeight( menu.titleId ) / 2.0 + 56 )
	SetSpriteDepth( menu.titleId, 14 )
	
	if MENU_LOADED = 0 then menu.uploadId = CreateSprite( LoadImage( "upload.png" ) )
	SetSpritePosition( menu.uploadId, GetSpriteX( puzzleIcon[ 2 ].borderId ) + GetSpriteWidth( puzzleIcon[ 2 ].borderId ) + 19, GetSpriteYByOffset( puzzleIcon[ 2 ].iconId ) - GetSpriteHeight( menu.uploadId ) / 2.0 )
	SetSpriteDepth( menu.uploadId, 14 )

	if MENU_LOADED = 0 then menu.cameraId = CreateSprite( LoadImage( "takepic.png" ) )
	SetSpritePosition( menu.cameraId, GetSpriteXByOffset( menu.uploadId ) - GetSpriteWidth( menu.cameraId ) / 2.0, GetSpriteYByOffset( puzzleIcon[ 5 ].iconId ) - GetSpriteHeight( menu.cameraId ) / 2.0 )
	SetSpriteDepth( menu.cameraId, 14 )
	
	if MENU_LOADED = 0 then menu.cropAcceptId = CreateSprite( LoadImage( "crop.png" ) )
	SetSpritePosition( menu.cropAcceptId, GetVirtualWidth( ) / 2.0 - GetSpriteWidth( menu.cropAcceptId ), GetScreenBoundsTop( ) )
	SetSpriteVisible( menu.cropAcceptId, 0 )
	SetSpriteDepth( menu.cropAcceptId, 900 )
	
	if MENU_LOADED = 0 then menu.cropCancelId = CreateSprite( LoadImage( "cancel.png" ) )
	SetSpritePosition( menu.cropCancelId, GetVirtualWidth( ) / 2.0, GetScreenBoundsTop( ) )
	SetSpriteVisible( menu.cropCancelId, 0 )
	SetSpriteDepth( menu.cropCancelId, 900 )
	
	if GAME_LOADED = 1
		Rem make sure this has its depth set back so it doesn't interfere with the main menu
		SetSpriteDepth( game.winBackdropId, 900 )
		SetSpriteDepth( game.winMenuId, 900 )
	endif
	
	Rem get the best time
	if GetFileExists( "best_time.dat" )
		f = OpenToRead( "best_time.dat" )
		For b = 1 to 3
			game.bestTime[ b ] = 0
			game.bestTime[ b ] = ReadFloat( f )
		Next b
		CloseFile( f )
	endif
	
	MENU_LOADED = 1
	
	Rem fly the main menu in
	PlayAnimation( 1 )
Endfunction

Function LoadPuzzleIcons( )
	img$ = ""
	across = 0
	down = 0
	maxAcross = 4
	iconWidth = 182
	iconHeight = 182
	
	if MENU_LOADED = 0 then borderImg = LoadImage( "border.png" )
	
	img$ = "puzzles/puzzle_1.jpg"
	For p = 1 to puzzleIconMax
		img$ = "puzzles/puzzle_" + Str( p ) + ".jpg"

		if MENU_LOADED = 0 then puzzleIcon[ p ].iconId = CreateSprite( LoadImage( img$ ) )
		SetSpriteDepth( puzzleIcon[ p ].iconId, 15 )
		SetSpriteSize( puzzleIcon[ p ].iconId, iconWidth, iconHeight )
		
		if MENU_LOADED = 0 then puzzleIcon[ p ].borderId = CreateSprite( borderImg )
		SetSpriteDepth( puzzleIcon[ p ].borderId, 14 )
		
		if p <= 2 or p >= 5 and p <= 6
			SetSpritePosition( puzzleIcon[ p ].iconId, 50 + iconWidth * across + across * 16, 130 + 16 * ( down + 1 ) + iconHeight * down )
		else
			SetSpritePosition( puzzleIcon[ p ].iconId, 466 + iconWidth * across + across * 16, 130 + 16 * ( down + 1 ) + iconHeight * down )
		endif
		
		across = across + 1
		if across = maxAcross
			down = down + 1
			across = 0
		endif
		
		SetSpritePosition( puzzleIcon[ p ].borderId, GetSpriteX( puzzleIcon[ p ].iconId ) - 5, GetSpriteY( puzzleIcon[ p ].iconId ) - 5 )
	Next p
Endfunction

Function LoadSounds( )
	sound.clickId = LoadSound( "sounds/click.wav" )
	sound.shuffleId = LoadSound( "sounds/shuffle.wav" )
	sound.slideId = LoadSound( "sounds/slide.wav" )
	sound.successId = LoadSound( "sounds/success.wav" )
Endfunction

Function RandomizeSeed( )
	rand = Random( 1, 65535 )
    SetRandomSeed( Random( rand, rand^2 ) * Timer( ) )
Endfunction

Function CheckForShake( )
    shook = 0
    Rem see if the device has an accelerometer
    if GetAccelerometerExists( )
		Rem get the current time in milliseconds
        curTime = GetMilliSeconds( )
        Rem see if we are within 100 milliseconds
        if curTime - shake.lastTime >= 100 + shake.timeOffset or curTime - shake.lastTime <= 0
            diffTime# = curTime - shake.lastTime
            shake.lastTime = curTime

			Rem get acclerometer values
            x# = GetRawAccelX( )
            y# = GetRawAccelY( )
            z# = GetRawAccelZ( )

            if shake.timeOffset > 0
				Rem if our lastX,Y,Z values haven't been filled, fill them
                diffTime# = 0
                shake.lastX = x#
                shake.lastY = y#
                shake.lastZ = z#
            endif
            shake.timeOffset = 0

            if diffTime# = 0 then diffTime# = 1
            Rem find the "Speed" of the player's shake
            speed# = Abs( x# + y# + z# - shake.lastX - shake.lastY - shake.lastZ ) / diffTime# * 10000.0
            Rem see if the speed# is greater than or equal to our sensitivity. 350 is our sensitivity value - the higher the value the less sensitive
            if speed# >= 350
				Rem report as a shake occured
                shake.lastTime = curTime
                shake.timeOffset = 1000
                shook = 1
            endif
            
            Rem record lastX,Y,Z values
            shake.lastX = x#
            shake.lastY = y#
            shake.lastZ = z#
        endif
    endif
Endfunction shook

Function CreateStarParticles( )
	// create a block of particles and set properties
	game.winParticlesId = CreateParticles( GetScreenBoundsLeft( ), GetScreenBoundsTop( ) )
	SetParticlesImage( game.winParticlesId, game.starImgId )
	SetParticlesStartZone( game.winParticlesId, GetScreenBoundsTop( ) - 24, GetScreenBoundsLeft( ), GetScreenBoundsRight( ), GetScreenBoundsTop( ) - 24 )
	SetParticlesDirection( game.winParticlesId, 0, 50.0 )
	SetParticlesLife( game.winParticlesId, 20 )
	SetParticlesSize( game.winParticlesId, 24 )
	SetParticlesAngle( game.winParticlesId, 15 )
	SetParticlesFrequency( game.winParticlesId, 35 )
	SetParticlesVelocityRange( game.winParticlesId, 2, 5 )
	AddParticlesColorKeyFrame( game.winParticlesId, 0, 255, 255, 255, 255 )
	SetParticlesRotationRange( game.winParticlesId, -45, 45 )
	AddParticlesColorKeyFrame( game.winParticlesId, 9.0, 255, 255, 255, 0 )
	SetParticlesDepth( game.winParticlesId, 4 )
Endfunction
