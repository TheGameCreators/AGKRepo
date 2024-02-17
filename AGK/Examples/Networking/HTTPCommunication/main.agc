
// Call HTTP commands

// nice fonts
UseNewDefaultFonts( 1 )

// display a background
backdrop = CreateSprite ( LoadImage ( "background7.jpg" ) )
SetSpriteSize ( backdrop, 100, 100 )

// connect to server
g_Net_Connection = CreateHTTPConnection ( )
g_Net_Connected  = SetHTTPHost          ( g_Net_Connection, "files.thegamecreators.com", 0 )

// state machine
stage=1

// main loop
do

	// main code
	select stage
     case 1 :
         Print("Getting file from HTTP...")
         download$ = "betafiles/Hands_On_AGK_Chapter_14_Text.pdf"
         GetHTTPFile ( g_Net_Connection, download$, "AGKFreeChapter.pdf" )
         stage=2
     endcase
     case 2 :
         perc#=GetHTTPFileProgress ( g_Net_Connection ) : perc=perc#
         Print ( "Waiting for download..."+str(perc) )
         if ( perc# >= 100.0 )
            stage=3
         endif
     endcase
     case 3 :
         Print ( "The file is in your write folder" )
     endcase
	endselect

    // update the screen
    Sync ( )

loop

// free usages
CloseHTTPConnection(g_Net_Connection)
DeleteHTTPConnection(g_Net_Connection)

// links to try
// http://www.thegamecreators.com/agktest/posttest.php
// http://www.thegamecreators.com/agktest/itworks.html
