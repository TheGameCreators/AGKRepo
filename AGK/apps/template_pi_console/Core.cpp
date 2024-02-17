// includes
#include "agk.h"
#include "template.h"

#include <signal.h>

volatile int done = 0;

void sig_handler(int sig)
{
	done = 1;
}

// platform specific
unsigned int TranslateKey( unsigned int key )
{
	switch( key )
	{
		case 9: key = 27; break; // Esc
			
		// F1 - F8
		case 67: key = 112; break;
		case 68: key = 113; break;
		case 69: key = 114; break;
		case 70: key = 115; break;
		case 71: key = 116; break;
		case 72: key = 117; break;
		case 73: key = 118; break;
		case 74: key = 119; break;
			
		case 49: key = 223; break; // `
		case 20: key = 189; break; // -
		case 21: key = 187; break; // =
		case 110: key = 36; break; // Home
		case 112: key = 33; break; // PgUp
		case 117: key = 34; break; // PgDown
		case 115: key = 35; break; // End
		case 119: key = 46; break; // Del
		//case 127: key = 223; break;  // +-
		case 118: key = 45; break; // Insert
			
		case 50: key = 16; break; // Shift (left)
		case 62: key = 16; break; // Shift (right)
		case 37: key = 17; break; // Ctrl (left)
		//case : key = 17; break; // Ctrl (right)
		case 64: key = 18; break;  // Alt (left)
		case 135: key = 18; break;  // Alt Gr (right)
			
		case 23: key = 9; break; // Tab
		case 36: key = 13; break; // Enter
		case 22: key = 8; break; // Backspace
			
		case 34: key = 219; break; // [
		case 35: key = 221; break; // ]
		case 47: key = 186; break; // ;
		case 48: key = 192; break; // '
			
		case 113: key = 37; break; // Left
		case 111: key = 38; break; // Up
		case 114: key = 39; break; // Right
		case 116: key = 40; break; // Down
			
		case 59: key = 188; break; // ,
		case 60: key = 190; break; // .
		case 61: key = 191; break; // /
		case 94: key = 220; break; // \ 
		case 51: key = 222; break; // #
			
			/*
		case : key = 13; break; // Num pad enter
		case : key = 111; break; // Num pad divide 
		case : key = 106; break; // Num pad multiply
		case : key = 109; break; // Num pad subtract
		case : key = 107; break; // Num pad add
		case : key = 110; break; // Num pad decimcal
		case : key = 0; break; // Pause/Break
		case : key = 0; break; // Caps lock 
		case : key = 45; break; // Num pad 0
		case : key = 35; break; // Num pad 1
		case : key = 40; break; // Num pad 2
		case : key = 34; break; // Num pad 3
		case : key = 37; break; // Num pad 4
		case : key = 12; break; // Num pad 5
		case : key = 39; break; // Num pad 6
		case : key = 36; break; // Num pad 7
		case : key = 38; break; // Num pad 8
		case : key = 33; break; // Num pad 9
		*/
		
		case 19: key = 48; break; // 0
		case 10: key = 49; break; // 1
		case 11: key = 50; break; // 2
		case 12: key = 51; break; // 3
		case 13: key = 52; break; // 4
		case 14: key = 53; break; // 5
		case 15: key = 54; break; // 6
		case 16: key = 55; break; // 7
		case 17: key = 56; break; // 8
		case 18: key = 57; break; // 9
		
		case 38: key = 65; break; // A
		case 56: key = 66; break; // B
		case 54: key = 67; break; // C
		case 40: key = 68; break; // D
		case 26: key = 69; break; // E
		case 41: key = 70; break; // F
		case 42: key = 71; break; // G
		case 43: key = 72; break; // H
		case 31: key = 73; break; // I
		case 44: key = 74; break; // J
		case 45: key = 75; break; // K
		case 46: key = 76; break; // L
		case 58: key = 77; break; // M
		case 57: key = 78; break; // N
		case 32: key = 79; break; // O
		case 33: key = 80; break; // P
		case 24: key = 81; break; // Q
		case 27: key = 82; break; // R
		case 39: key = 83; break; // S
		case 28: key = 84; break; // T
		case 30: key = 85; break; // U
		case 55: key = 86; break; // V
		case 25: key = 87; break; // W
		case 53: key = 88; break; // X
		case 29: key = 89; break; // Y
		case 52: key = 90; break; // Z
	}
	
	return key;
}

int main (int argc, char **argv)
{
	struct sigaction sig_struct;
	sig_struct.sa_handler = sig_handler;
	sig_struct.sa_flags = 0;
	sigemptyset( &sig_struct.sa_mask );
	
	if ( sigaction(SIGINT, &sig_struct, NULL) == -1 )
	{
		printf( "Failed to set signal handler\n" );
	}
	
	if ( sigaction(SIGTERM, &sig_struct, NULL) == -1 )
	{
		printf( "Failed to set term signal handler\n" );
	}
	
    agk::SetExtraAGKPlayerAssetsMode ( 0 );
		
	// call app begin
    try
    {
        agk::InitConsole();

        App.Begin();
    }
    catch( ... )
    {
        uString err = agk::GetLastError();
		printf( "Uncaught exception: %s\n", err.GetStr() );
        done = 1;
    }

    while ( !done )
    {
    	// run AGK app loop
	    try
	    {
	        if ( App.Loop() == 1 ) done = 1;
	    }
	    catch( ... )
	    {
	        uString err = agk::GetLastError();
			printf( "Uncaught exception: %s\n", err.GetStr() );
	        done = 1;
	    }
        
    }
	
    // close app
    App.End();
    agk::CleanUp();

    // success    
    return 0;
}
