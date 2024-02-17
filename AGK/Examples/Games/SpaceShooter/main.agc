
/////////////////////////////////////////
// TYPES ////////////////////////////////
/////////////////////////////////////////

// aliens
type sAlien
	iSprite as integer
	iEngine as integer

	fSpeed#

	fValueA#
	fValueB#
endtype

// weapons
type sWeapon
	iID as integer
	iActive as integer
	fLife as float
	iExplosion as integer
endtype

// explosions
type sExplosion
	iID as integer
	iActive as integer
endtype

// scenery
global dim g_iScenery [ 1000 ] as integer
global g_iSceneryCount = 0

// 10 aliens
global dim g_Aliens [ 10 ] as sAlien

// player data
global g_iShip = 0
global g_iParticles = 0
global g_iPlayerShip   = 0
global g_iPlayerEngine = 0
global g_iPlayerExplosion = 0
global g_iPlayerWeapon = 0
global g_fPlayerShoot as float = 0.0
global g_iPlayerState = 0
global dim g_Bullets [ 10 ] as sWeapon

// explosions pool
global dim g_Explosions [ 5 ] as sExplosion

// other globals
global g_iGameState = 0
global g_iScore = 0
global g_iScoreTarget = 0
global g_iLives = 3
global g_iDelay = 0

// includes
#include "player.agc"
#include "aliens.agc"
#include "level.agc"
#include "misc.agc"

// start the game
Begin ( )

// main loop
do
    // check the state
    if g_iGameState = 0 then HandleStart ( )
    if g_iGameState = 1 then HandleGame  ( )
    if g_iGameState = 2 then HandleEnd   ( )

    // update the screen
    Sync ( )
loop

function Begin ( )
    // load everything for the game
    SetVirtualResolution ( 550, 400 )

    CreateBackground      ( )
    CreateStarField       ( )
    CreateLevel           ( )
    CreateAliens          ( )
    CreatePlayer          ( )
    CreateAlienExplosions ( )
    CreateTextEntities    ( )
    CreateMusic           ( )
    HidePlayer            ( )
endfunction

function HandleStart ( )
    // start screen

    // fade in text
    SetTextColorAlpha ( 3, GetTextColorAlpha ( 3 ) + 2 )
	SetTextVisible ( 3, 1 )

    // have aliens moving across
    UpdateAliens ( )

    // wait for input
    if GetPointerPressed ( ) = 1

        // reset aliens
		ResetAliens ( )

		// show the player
		ShowPlayer ( )

        // set game over text for later on
		SetTextColorAlpha ( 3, 0 )
		SetTextVisible ( 3, 0 )
		SetTextPosition ( 3, 130, 160 )
		SetTextString ( 3, "GAME OVER" )
		SetTextVisible ( 10, 1 )
		SetTextVisible ( 2, 1 )

        // switch to in game state
        g_iGameState = 1
	endif
endfunction

function HandleGame ( )
    // main game

    // fade in on screen text
    SetTextColorAlpha ( 10, GetTextColorAlpha ( 10 ) + 2 )
	SetTextColorAlpha ( 2, GetTextColorAlpha ( 2 ) + 2 )

    // update the player, aliens and text
	UpdatePlayer ( )
	UpdateAliens ( )
	UpdateText   ( )
endfunction

function HandleEnd ( )
    // handle the end of the game

	// update aliens
    UpdateAliens ( )

    // fade in text
    SetTextColorAlpha ( 3, GetTextColorAlpha ( 3 ) + 2 )
	SetTextVisible ( 3, 1 )

    // act on input
	if ( GetPointerPressed ( ) )
        // reset scrolling
		SetViewOffset ( 0, 0 )

        // kill off all bullets
		DestroyAllBullets ( )

        // reset player, bullets and aliens
		ResetPlayer ( )
		ResetBullets ( )
		ResetAliens ( )

        // hide the player
		HidePlayer ( )

        // set start game text
		SetTextColorAlpha ( 3, 0 )
		SetTextVisible ( 3, 1 )
		SetTextPosition ( 3, 100, 160 )
		SetTextString ( 3, "START GAME" )
		SetTextVisible ( 2, 1 )
		SetTextColorAlpha ( 10, 0 )
		SetTextColorAlpha ( 2, 0 )

        // reset globals
        g_iDelay = 0
		g_iPlayerState = 0
        g_iLives = 3
		g_iScore = 0
		g_iScoreTarget = 0
		g_iGameState = 0
    endif
endfunction


