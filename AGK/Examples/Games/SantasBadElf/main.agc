

global g_iBackground   = 0
global g_iInstructions = 0

global g_iTap   = 0
global g_iTapX  = 0
global g_iTapY  = 0

global dim g_iSounds [ 10 ]

global g_iGameBackground = 0
global g_iGameBackgroundA = 0

global dim g_iForegrounds  [ 3 ]
global dim g_iForegroundsA [ 3 ]

global g_iTime
global g_iScore
global g_iFloatingScore
global g_iFloatingScoreState
global g_iMessageText
global g_iMessageState
global g_iCloseButton
global g_iScoreText
global g_iTimeText
global g_iDialog

global dim g_iImages [ 50 ]
global dim g_iSmokeImages [ 9 ]
global dim g_iSnowImages [ 6 ]

type sStar
        iSprite as integer
        iState as integer
        fSpeed as float
endtype

global dim g_Stars [ 32 ] as sStar

global g_iState = 0
global g_iGameState = 0
global g_iReleaseTimer  = 150
global g_iReleaseTarget = 200
global g_iScoreTarget = 0

global dim g_Clouds [ 3 ] as sStar

type sSnow
        iSprite as integer

        fMove as float
        fSpeed as float
        fAngle as float
endtype

global dim g_Snow [ 100 ] as sSnow

global g_iBadElfTime = 0
global g_iBadElfTarget = 0
global g_iIntroSound = 0

global g_iOverlay = 0

global g_iCountDownNumber = 5
global g_iCountDownAlpha = 0

global g_iTextAlpha = 0

global g_iReturn = 0

global g_iTimer = 0
global g_iGameScore = 0

global g_iGameScoreAlpha = 0

global g_iLastTime = 0
global g_iThisTime = 0
global g_iRealTime = 0

type sItem
        iState as integer
        iType as integer
        iSprite as integer
        iFrameCount as integer

        iFrame as integer
        iTime as integer
        iTimeBetweenNextFrame as integer
endtype

global dim g_Item [ 5 ] as sItem

global dim g_FrameList [ 5, 32 ]

global g_fSpeed as float

global g_iPauseState = 0

global g_iGameOver = 0

// use a new font for the default text
defaultfontimage = LoadImage ( "ascii.png" )
SetTextDefaultFontImage ( defaultfontimage )

// includes
#include "main_menu.agc"
#include "game_load.agc"
#include "game_loop.agc"
#include "game_pause.agc"


g_iLastTime = GetSeconds ( )
g_fSpeed = 2.5

MainMenu_Begin ( )

// main loop
do

        MainMenu_Loop ( )

    // update the screen
    Sync ( )
loop






