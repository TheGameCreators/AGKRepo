// includes
#include "agk.h"
#include "interpreter.h"

#define GLFW_INCLUDE_ES2
#include "GLFW/glfw3.h"
#include <SDL.h>
#include "emscripten.h"
#include "emscripten/html5.h"

void PlatformAppQuit()
{
    exit(0);
}

// platform specific
unsigned int TranslateKey( unsigned int key )
{
	switch( key )
	{
		case GLFW_KEY_ESCAPE: key = 27; break; // Esc
			
		// F1 - F8
		case GLFW_KEY_F1: key = 112; break;
		case GLFW_KEY_F2: key = 113; break;
		case GLFW_KEY_F3: key = 114; break;
		case GLFW_KEY_F4: key = 115; break;
		case GLFW_KEY_F5: key = 116; break;
		case GLFW_KEY_F6: key = 117; break;
		case GLFW_KEY_F7: key = 118; break;
		case GLFW_KEY_F8: key = 119; break;
			
		case 96: key = 223; break; // `
		case 45: key = 189; break; // -
		case 61: key = 187; break; // =
		case GLFW_KEY_HOME: key = 36; break; // Home
		case GLFW_KEY_PAGE_UP: key = 33; break; // PgUp
		case GLFW_KEY_PAGE_DOWN: key = 34; break; // PgDown
		case GLFW_KEY_END: key = 35; break; // End
		case GLFW_KEY_DELETE: key = 46; break; // Del
		//case 160: key = 223; break;  // +-
		case GLFW_KEY_INSERT: key = 45; break; // Insert
			
		case GLFW_KEY_LEFT_SHIFT: key = 16; break; // Shift (left)
		case GLFW_KEY_RIGHT_SHIFT: key = 16; break; // Shift (right)
		case GLFW_KEY_LEFT_CONTROL: key = 17; break; // Ctrl (left)
		case GLFW_KEY_RIGHT_CONTROL: key = 17; break; // Ctrl (right)
		case GLFW_KEY_LEFT_ALT: key = 0; break;  // Alt/Cmd (left)
		case GLFW_KEY_RIGHT_ALT: key = 18; break;  // Cmd (right)
			
		case GLFW_KEY_TAB: key = 9; break; // Tab
		case GLFW_KEY_ENTER: key = 13; break; // Enter
		case GLFW_KEY_BACKSPACE: key = 8; break; // Backspace
			
		case 91: key = 219; break; // [
		case 93: key = 221; break; // ]
		case 59: key = 186; break; // ;
		case 39: key = 192; break; // '
			
		case GLFW_KEY_LEFT: key = 37; break; // Left
		case GLFW_KEY_UP: key = 38; break; // Up
		case GLFW_KEY_RIGHT: key = 39; break; // Right
		case GLFW_KEY_DOWN: key = 40; break; // Down
			
		case 44: key = 188; break; // ,
		case 46: key = 190; break; // .
		case 47: key = 191; break; // /
		case 92: key = 220; break; // \
			
		case GLFW_KEY_KP_ENTER: key = 13; break; // Num pad enter
		case GLFW_KEY_KP_DIVIDE: key = 111; break; // Num pad divide 
		case GLFW_KEY_KP_MULTIPLY: key = 106; break; // Num pad multiply
		case GLFW_KEY_KP_SUBTRACT: key = 109; break; // Num pad subtract
		case GLFW_KEY_KP_ADD: key = 107; break; // Num pad add
		case GLFW_KEY_KP_DECIMAL: key = 110; break; // Num pad decimcal
		case GLFW_KEY_PAUSE: key = 0; break; // Pause/Break
		case GLFW_KEY_CAPS_LOCK: key = 0; break; // Caps lock 
		case GLFW_KEY_KP_0: key = 45; break; // Num pad 0
		case GLFW_KEY_KP_1: key = 35; break; // Num pad 1
		case GLFW_KEY_KP_2: key = 40; break; // Num pad 2
		case GLFW_KEY_KP_3: key = 34; break; // Num pad 3
		case GLFW_KEY_KP_4: key = 37; break; // Num pad 4
		case GLFW_KEY_KP_5: key = 12; break; // Num pad 5
		case GLFW_KEY_KP_6: key = 39; break; // Num pad 6
		case GLFW_KEY_KP_7: key = 36; break; // Num pad 7
		case GLFW_KEY_KP_8: key = 38; break; // Num pad 8
		case GLFW_KEY_KP_9: key = 33; break; // Num pad 9
	}
	
	return key;
}

static void keyboard_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if ( action == GLFW_PRESS || action == GLFW_REPEAT )
	{
		if ( action == GLFW_PRESS ) agk::KeyDown( TranslateKey(key) );
		
		if ( key >= 32 && key <= 93 )
		{
			if ( mods & GLFW_MOD_SHIFT )
			{
				if ( key >= 65 && key <= 90 ) agk::CharDown( key ); // A-Z
				else
				{
					switch( key )
					{
						case 32: agk::CharDown(32); break;
						case 39: agk::CharDown(64); break;
						case 44: agk::CharDown(60); break;
						case 45: agk::CharDown(95); break;
						case 46: agk::CharDown(62); break;
						case 47: agk::CharDown(63); break;
						case 48: agk::CharDown(41); break;
						case 49: agk::CharDown(33); break;
						case 50: agk::CharDown(34); break;
						case 51: agk::CharDown(35); break;
						case 52: agk::CharDown(36); break;
						case 53: agk::CharDown(37); break;
						case 54: agk::CharDown(94); break;
						case 55: agk::CharDown(38); break;
						case 56: agk::CharDown(42); break;
						case 57: agk::CharDown(40); break;
						case 59: agk::CharDown(58); break;
						case 61: agk::CharDown(43); break;
						case 91: agk::CharDown(123); break;
						case 92: agk::CharDown(124); break;
						case 93: agk::CharDown(125); break;
						
					}
				}
			}
			else
			{
				if ( key >= 65 && key <= 90 ) agk::CharDown( key+32 ); // A-Z
				else if ( key == 94 ) agk::CharDown( 96 );
				else agk::CharDown( key );
			}
		}
		
		/*
		static int first = 1;
		if ( first )
		{
			first = 0;
			agk::CreateText(1,"");
			agk::SetTextPosition(1,100,500);
			agk::SetTextSize(1,20);
		}
	
		uString str;
		//str.Format( "Key: %d", agk::GetLastChar() );
		str.Format( "Key: %d, Scancode: %d", key, scancode );
		agk::SetTextString( 1, str );
		*/
	}
	else if ( action == GLFW_RELEASE )
	{
		agk::KeyUp( TranslateKey(key) );
	}

}

static void char_callback( GLFWwindow* window, unsigned int key )
{
	agk::CharDown( key );
}

static void mouse_pos_callback( GLFWwindow* window, double x, double y )
{
	agk::MouseMove( 0, agk::Round(x), agk::Round(y) );
}

static void mouse_button_callback( GLFWwindow* window, int button, int action, int mods )
{
	if ( button == GLFW_MOUSE_BUTTON_LEFT )
	{
		if ( action == GLFW_PRESS ) agk::MouseLeftButton( 0, 1 );
		else if ( action == GLFW_RELEASE ) agk::MouseLeftButton( 0, 0 );
	}
	else if ( button == GLFW_MOUSE_BUTTON_RIGHT )
	{
		if ( action == GLFW_PRESS ) agk::MouseRightButton( 0, 1 );
		else if ( action == GLFW_RELEASE ) agk::MouseRightButton( 0, 0 );
	}
	else if ( button == GLFW_MOUSE_BUTTON_MIDDLE )
	{
		if ( action == GLFW_PRESS ) agk::MouseMiddleButton( 0, 1 );
		else if ( action == GLFW_RELEASE ) agk::MouseMiddleButton( 0, 0 );
	}
}

static void mouse_wheel_callback( GLFWwindow* window, double x, double y )
{
    agk::MouseWheel(0, -y);
}

static void window_size_callback( GLFWwindow* window, int width, int height)
{
	agk::UpdateDeviceSize();
}

static void close_callback( GLFWwindow* window )
{
    App.AppSplash();
    glfwSetWindowShouldClose( window, 1 );
}

void set_audio_unlock_listener()
{
	EM_ASM({
		var unlockSoundContext = function()
		{
			if( AL.currentContext )
	    	{
		    	if ( AL.currentContext.ctx.currentTime == 0 )
		    	{
					var buffer = AL.currentContext.ctx.createBuffer(1, 1, 22050);
					var source = AL.currentContext.ctx.createBufferSource();
					source.buffer = buffer;
					source.connect(AL.currentContext.ctx.destination);
					if (typeof(source.start) !== 'undefined') source.start(0);
      				else if (typeof(source.noteOn) !== 'undefined') source.noteOn(0);
				}
				window.removeEventListener('touchstart', unlockSoundContext, false);
			}
		};
		
		var unlockMusicContext = function()
		{
			if( SDL.audioContext )
			{
				if ( SDL.audioContext.currentTime == 0 )
				{
					var buffer = SDL.audioContext.createBuffer(1, 1, 22050);
					var source = SDL.audioContext.createBufferSource();
					source.buffer = buffer;
					source.connect(SDL.audioContext.destination);
					if (typeof(source.start) !== 'undefined') source.start(0);
      				else if (typeof(source.noteOn) !== 'undefined') source.noteOn(0);
				}
				window.removeEventListener('touchstart', unlockMusicContext, false);
			}
		};
		
		window.addEventListener('touchstart', unlockSoundContext, false);
    	window.addEventListener('touchstart', unlockMusicContext, false);
    });
}

static int touch_callback( int event_type, const EmscriptenTouchEvent *touchEvent, void *userData )
{
	switch( event_type )
	{
		case EMSCRIPTEN_EVENT_TOUCHSTART:
		{
			for(int i = 0; i < touchEvent->numTouches; i++ )
			{
				if ( touchEvent->touches[i].isChanged )
				{
					int isFullscreen = EM_ASM_INT_V({return Browser.isFullScreen ? 1 : 0});
					int X = touchEvent->touches[i].canvasX;
					int Y = touchEvent->touches[i].canvasY;
					if ( isFullscreen )
					{
						float scale = EM_ASM_DOUBLE_V({return window.devicePixelRatio});
						X = (int) X*scale;
						Y = (int) Y*scale;
					}
					agk::TouchPressed( touchEvent->touches[i].identifier, X, Y );
				}
			}
			break;
		}
		case EMSCRIPTEN_EVENT_TOUCHEND:
		{
			for(int i = 0; i < touchEvent->numTouches; i++ )
			{
				if ( touchEvent->touches[i].isChanged )
				{
					int isFullscreen = EM_ASM_INT_V({return Browser.isFullScreen ? 1 : 0});
					int X = touchEvent->touches[i].canvasX;
					int Y = touchEvent->touches[i].canvasY;
					if ( isFullscreen )
					{
						float scale = EM_ASM_DOUBLE_V({return window.devicePixelRatio});
						X = (int) X*scale;
						Y = (int) Y*scale;
					}
					agk::TouchReleased( touchEvent->touches[i].identifier, X, Y );
				}
			}
			break;
		}
		case EMSCRIPTEN_EVENT_TOUCHMOVE:
		{
			for(int i = 0; i < touchEvent->numTouches; i++ )
			{
				if ( touchEvent->touches[i].isChanged )
				{
					int isFullscreen = EM_ASM_INT_V({return Browser.isFullScreen ? 1 : 0});
					int X = touchEvent->touches[i].canvasX;
					int Y = touchEvent->touches[i].canvasY;
					if ( isFullscreen )
					{
						float scale = EM_ASM_DOUBLE_V({return window.devicePixelRatio});
						X = (int) X*scale;
						Y = (int) Y*scale;
					}
					agk::TouchMoved( touchEvent->touches[i].identifier, X, Y );
				}
			}
			break;
		}
		case EMSCRIPTEN_EVENT_TOUCHCANCEL:
		{
			for(int i = 0; i < touchEvent->numTouches; i++ )
			{
				if ( touchEvent->touches[i].isChanged )
				{
					int isFullscreen = EM_ASM_INT_V({return Browser.isFullScreen ? 1 : 0});
					int X = touchEvent->touches[i].canvasX;
					int Y = touchEvent->touches[i].canvasY;
					if ( isFullscreen )
					{
						float scale = EM_ASM_DOUBLE_V({return window.devicePixelRatio});
						X = (int) X*scale;
						Y = (int) Y*scale;
					}
					agk::TouchReleased( touchEvent->touches[i].identifier, X, Y );
				}
			}
			break;
		}
		default: return 0;
	}
	
	return 1;
}

struct initdata
{
    GLFWwindow* window;
    unsigned int width;
    unsigned int height;
    const char* title;
    GLFWmonitor* monitor;
};

void loop()
{
	// clear screen here since swap isn't done until after we hand back control, so a clear at the end (as on other platforms) would erase eveything we've drawn
	agk::ClearScreen();
	App.Loop();
}

int main (int argc, char **argv)
{
	agk::InitFilePaths();

	// load company name from bytecode file to make write folder path more unique, used in InitGraphics
	if ( agk::GetFileExists("media/bytecode.byc") )
	{
		App.m_iStandAlone = 1;
		if ( App.m_sProgram.LoadBytecodeHeader("media/bytecode.byc") )
		{
			agk::SetCompanyName( App.m_sProgram.GetCompanyName() );
			agk::InitFilePaths(); // redo file paths with the company name
		}
	}

    int width = EM_ASM_INT_V({return Module["canvas"].width});
	int height = EM_ASM_INT_V({return Module["canvas"].height});
	int fullscreen = 0;
    
	// create a window for the app
	if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }
    
    GLFWwindow* window = 0;
    
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if ( !fullscreen ) monitor = 0;
	
    // initialise graphics API for app
    agk::SetExtraAGKPlayerAssetsMode ( 1 );
		
	// call app begin
	int done = 0;
	try
	{
		initdata data;
        data.window = 0; // will be returned
        data.width = width;
        data.height = height;
        data.title = App.g_pWindowTitle;
        data.monitor = monitor;
        
        agk::InitGraphics( &data, App.m_sProgram.GetRendererMode(), App.m_sProgram.GetFlags() );
        
        window = data.window;
        if ( !window ) throw 1;
		
		glfwSetWindowSizeCallback( window, window_size_callback );
		glfwSetKeyCallback( window, keyboard_callback );
		glfwSetCursorPosCallback( window, mouse_pos_callback );
		glfwSetMouseButtonCallback( window, mouse_button_callback );
		glfwSetScrollCallback( window, mouse_wheel_callback );
		//glfwSetCharCallback( window, char_callback );
		glfwSetWindowCloseCallback( window, close_callback );

		emscripten_set_touchstart_callback( "#canvas", 0, false, touch_callback );
		emscripten_set_touchend_callback( "#canvas", 0, false, touch_callback );
		emscripten_set_touchmove_callback( "#canvas", 0, false, touch_callback );
		emscripten_set_touchcancel_callback( "#canvas", 0, false, touch_callback );
    
		set_audio_unlock_listener();

		App.Begin();
	}
	catch( ... )
	{
		uString err = agk::GetLastError();
		err.Prepend( "Uncaught exception: \n\n" );
		done = 1;
	}
	
	emscripten_set_main_loop(loop, 0, 1);
	
	// close app
	App.End();
    agk::CleanUp();
	
	// close window system
	if ( window ) glfwDestroyWindow(window);
    glfwTerminate();
	
	// success    
    return 0;
}
