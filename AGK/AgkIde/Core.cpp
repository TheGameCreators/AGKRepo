// Common Includes (for Win32)

// windows 7 for touch commands
#define _WIN32_WINNT 0x0601

#include "AGK.h"
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>								// Header File For The GLu32 Library
#include "direct.h"

// App specific include for Core (so core can be distributed between apps without editing inside it)
#include "ide.h"
#include "resource.h"

#define WM_SOUND_EVENT 0x8001
#define WM_VIDEO_EVENT 0x8002
#define TEST_DYNAMIC_RESIZING

// comment out to revert to single mouse
//#define MULTI_MOUSE

extern bool ImGui_ImplAGL_UpdateMouseCursor();

// Namespace
using namespace AGK;

// Globals for core
HANDLE hDebugConsoleOut = NULL;
HWND g_hWnd = NULL;
HWND agk_hWnd = NULL;
HCURSOR hCursorArrow;
bool bExitLoop = false;
bool bAGKInitialized = false;
bool bAGKInMainLoop = false;

bool os_messages_support = true;
unsigned int os_messages = 0;
unsigned int os_messages_old = 0;
unsigned int os_messages_wait = 0;
unsigned int os_messages_sleeptime = 200;
extern bool bAllProjectsHasBeenProcessed;
bool bCmdLineProcessed = false;

HANDLE hMutex;
struct myCOPYDATASTRUCT {
	DWORD dwData;
	DWORD cbData;
	PVOID lpData;
};


bool CancelQuit();

//
// Windows Application STUB requires entry function, create window and message pump
//

#define AGK_MOUSE_LEFTDOWN		0x00000001
#define AGK_MOUSE_RIGHTDOWN		0x00000002
#define AGK_MOUSE_MIDDLEDOWN	0x00000004

class cMouseDevice
{
	protected:

	public:
		cMouseDevice() { ID = 0; hDevice = 0; x = 0; y = 0; pNext = 0; sprite = 0; m_bFlags = 0; }
		~cMouseDevice();

		UINT ID;
		HANDLE hDevice;
		int x;
		int y;
		UINT m_bFlags;
		cMouseDevice *pNext;
		UINT sprite;
};

cMouseDevice *pMouseList = 0;

void Output( const char *szMsg )
{
	if ( !hDebugConsoleOut ) 
	{
		AllocConsole ( );
		HWND hConsole = GetConsoleWindow();
		MoveWindow( hConsole, 32, 500, 800, 350, TRUE );
		hDebugConsoleOut = GetStdHandle ( STD_OUTPUT_HANDLE );
	}
	
	DWORD dwWritten = 0;
	WriteConsole ( hDebugConsoleOut, "> ", 2, &dwWritten, NULL );
	WriteConsole ( hDebugConsoleOut, szMsg, (DWORD) strlen( szMsg ), &dwWritten, NULL );
	WriteConsole ( hDebugConsoleOut, "\n", 1, &dwWritten, NULL );
}

unsigned int TranslateKey( unsigned int key )
{
	switch( key )
	{
		case 20: key = 0; break; // Caps lock
		//case 120: key = 0; break; // F9 ? why ?
		//case 121: key = 0; break; // F10 ? why ?
		//case 122: key = 0; break; // F11
		//case 123: key = 0; break; // F12
		case 18: key = 18; break; // Alt Gr
		case 91: key = 0; break; // Windows key (left)
		case 92: key = 0; break; // Windows key (right)

		// Num pad 0-9
		case 96:
		case 97: 
		case 98: 
		case 99: 
		case 100: 
		case 101: 
		case 102: 
		case 103: 
		case 104: 
		case 105: key -= 48; break;

		case 111: key = 192; break; // Num pad /
		case 109: key = 189; break; // Num pad -
		case 110: key = 190; break; // Num pad .

		case 144: key = 0; break;  // Num lock
		case 19: key = 0; break;  // pause/break
	}

	return key;
}

bool g_bShouldBeTopMost = false;

// delay the loading of these functions so Windows XP and Vista don't complain
int (__stdcall *CloseTouchInputHandleDelayed)( HTOUCHINPUT ) = 0;
int (__stdcall *GetTouchInputInfoDelayed)( HTOUCHINPUT, unsigned int, PTOUCHINPUT, int ) = 0;
BOOL (__stdcall *RegisterTouchWindowDelay)(HWND, ULONG) = 0;

void LoadDelayedFunctions()
{
	// only checks once
	static int first = 1;
	if ( first == 1 )
	{
		first = 0;
		HMODULE user32 = LoadLibraryA( "USER32.dll" );
		CloseTouchInputHandleDelayed = (int(__stdcall *)(HTOUCHINPUT)) GetProcAddress( user32, "CloseTouchInputHandle" );
		GetTouchInputInfoDelayed = (int(__stdcall *)(HTOUCHINPUT, unsigned int, PTOUCHINPUT, int)) GetProcAddress( user32, "GetTouchInputInfo" );
		RegisterTouchWindowDelay = (BOOL(__stdcall *)(HWND, ULONG)) GetProcAddress( user32, "RegisterTouchWindow" );
		FreeLibrary( user32 );
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool bLeftDown = false;
	static bool Tracking = false;
	os_messages++;
	switch (message)
	{
#ifdef WM_TOUCH
		// only works on Windows 7 or above
	case WM_TOUCH:
	{
		LoadDelayedFunctions();

		if (CloseTouchInputHandleDelayed)
		{
			// windows based touch device
			UINT cInputs = LOWORD(wParam);
			if (cInputs > 0)
			{
				// compensate for window (which may not be in top left)
				RECT rect;
				::GetWindowRect(g_hWnd, &rect);

				PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
				if (GetTouchInputInfoDelayed((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT)))
				{
					// process pInputs
					for (UINT i = 0; i < cInputs; i++)
					{
						pInputs[i].x -= (rect.left * 100);
						pInputs[i].y -= (rect.top * 100);
						pInputs[i].x /= 100;
						pInputs[i].y /= 100;

						if (pInputs[i].dwFlags & TOUCHEVENTF_DOWN) agk::TouchPressed(pInputs[i].dwID, pInputs[i].x, pInputs[i].y);
						if (pInputs[i].dwFlags & TOUCHEVENTF_MOVE) agk::TouchMoved(pInputs[i].dwID, pInputs[i].x, pInputs[i].y);
						if (pInputs[i].dwFlags & TOUCHEVENTF_UP) agk::TouchReleased(pInputs[i].dwID, pInputs[i].x, pInputs[i].y);
					}

					CloseTouchInputHandleDelayed((HTOUCHINPUT)lParam);
				}

				delete[] pInputs;
			}
		}

		break;
	}

#endif

#ifdef MULTI_MOUSE
	case WM_INPUT:
	{
		if (!bCapturePointer) break;
		// raw input from registered devices (in this case mice)
		UINT dwSize;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* raw = (RAWINPUT*)lpb;

		// find mouse device
		int count = 0;
		cMouseDevice *pMouse = pMouseList;
		while (pMouse)
		{
			count++;
			if (pMouse->hDevice == raw->header.hDevice) break;
			pMouse = pMouse->pNext;
		}

		// is this a new mouse device?
		if (!pMouse)
		{
			pMouse = new cMouseDevice();
			pMouse->hDevice = raw->header.hDevice;
			pMouse->pNext = pMouseList;
			pMouse->ID = count + 1;
			pMouse->x = lastX;
			pMouse->y = lastY;
			pMouse->sprite = agk::CreateSprite(99);
			agk::SetSpriteSize(pMouse->sprite, 4, -1);
			agk::SetSpriteTransparency(pMouse->sprite, 2);
			switch (count)
			{
			case 0: agk::SetSpriteColor(pMouse->sprite, 255, 0, 0, 255); break;
			case 1: agk::SetSpriteColor(pMouse->sprite, 0, 255, 0, 255); break;
			case 2: agk::SetSpriteColor(pMouse->sprite, 0, 0, 255, 255); break;
			case 3: agk::SetSpriteColor(pMouse->sprite, 255, 255, 0, 255); break;
			}
			pMouseList = pMouse;
		}

		// relative or absolute positioning?
		if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
		{
			pMouse->x = raw->data.mouse.lLastX;
			pMouse->y = raw->data.mouse.lLastY;
		}
		else
		{
			pMouse->x += raw->data.mouse.lLastX;
			pMouse->y += raw->data.mouse.lLastY;
		}

		// check buttons
		if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		{
			if ((pMouse->m_bFlags & AGK_MOUSE_LEFTDOWN) == 0)
			{
				agk::TouchPressed(pMouse->ID, pMouse->x, pMouse->y);
			}

			pMouse->m_bFlags |= AGK_MOUSE_LEFTDOWN;
		}

		if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
		{
			if ((pMouse->m_bFlags & AGK_MOUSE_LEFTDOWN) != 0)
			{
				agk::TouchReleased(pMouse->ID, pMouse->x, pMouse->y);
			}

			pMouse->m_bFlags &= ~AGK_MOUSE_LEFTDOWN;
		}

		// if holding left button send drag event
		if ((pMouse->m_bFlags & AGK_MOUSE_LEFTDOWN) != 0)
		{
			if (raw->data.mouse.lLastX != 0 && raw->data.mouse.lLastY != 0)
			{
				agk::TouchMoved(pMouse->ID, pMouse->x, pMouse->y);
			}
		}

		if (pMouse->x > DEVICE_WIDTH) pMouse->x = DEVICE_WIDTH;
		if (pMouse->y > DEVICE_HEIGHT) pMouse->y = DEVICE_HEIGHT;
		if (pMouse->x < 0) pMouse->x = 0;
		if (pMouse->y < 0) pMouse->y = 0;

		agk::SetSpritePosition(pMouse->sprite, agk::DeviceToScreenX(pMouse->x), agk::DeviceToScreenY(pMouse->y));

		//char str[ 256 ];
		//sprintf( str, "X: %d, Y: %d", pMouse->x, pMouse->y );
		//Output( str );
		if (bCapturePointer)
		{
			RECT rect;
			::GetWindowRect(g_hWnd, &rect);
			int centerX = (rect.left + rect.right) / 2;
			int centerY = (rect.top + rect.bottom) / 2;
			SetCursorPos(centerX, centerY);
		}
		break;
	}
#endif

	case WM_LBUTTONDOWN:
	{
		bLeftDown = true;
		int x = lParam & 0x0000ffff;
		int y = (lParam >> 16) & 0x0000ffff;
		if (x > 32767) x = x - 65536;
		if (y > 32767) y = y - 65536;

		// detect pen touch
		//if ( (GetMessageExtraInfo() & 0xFFFFFF00) != 0xFF515700 ) 
		agk::MouseLeftButton(0, 1);

#ifdef MULTI_MOUSE
		if (!bCapturePointer)
		{
			bCapturePointer = true;
			ShowCursor(FALSE);
			lastX = x;
			lastY = y;
		}
#else
		SetCapture(g_hWnd);
		//agk::TouchPressed( 1, x,y );
#endif
		break;
	}

		case WM_LBUTTONUP:
		{
			bLeftDown = false;
			int x = lParam & 0x0000ffff;
			int y = (lParam >> 16) & 0x0000ffff;
			if ( x > 32767 ) x = x - 65536;
			if ( y > 32767 ) y = y - 65536;

			// detect pen touch
			//if ( (GetMessageExtraInfo() & 0xFFFFFF00) != 0xFF515700 ) 
				agk::MouseLeftButton( 0, 0 );

#ifndef MULTI_MOUSE
			ReleaseCapture();
			//agk::TouchReleased( 1, x,y );
#endif
			break;
		}

		case WM_RBUTTONDOWN:
		{
			agk::MouseRightButton( 0, 1 );

#ifdef MULTI_MOUSE
			if ( bCapturePointer ) // Alt Gr
			{
				bCapturePointer = false;
				ShowCursor( TRUE );
			}
#endif
			break;
		}

		case WM_RBUTTONUP:
		{
			agk::MouseRightButton( 0, 0 );
			break;
		}

		case WM_MBUTTONDOWN:
		{
			agk::MouseMiddleButton( 0, 1 );
			break;
		}

		case WM_MBUTTONUP:
		{
			agk::MouseMiddleButton( 0, 0 );
			break;
		}

		case WM_MOUSEWHEEL:
		{
			int w = (wParam >> 16) & 0x0000ffff;
			if ( w > 32767 ) w = w - 65536;

			float fW = w / 40.0f;

			agk::MouseWheel( 0, fW );
			break;
		}

		case WM_MOUSELEAVE:
			Tracking = false;
			break;

		case WM_MOUSEMOVE:
		{
			if (!Tracking)
			{
				TRACKMOUSEEVENT tme = { sizeof(tme) };
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				::TrackMouseEvent(&tme);
				Tracking = true;
				ImGui_ImplAGL_UpdateMouseCursor();
			}
			int x = lParam & 0x0000ffff;
			int y = (lParam >> 16) & 0x0000ffff;
			if ( x > 32767 ) x = x - 65536;
			if ( y > 32767 ) y = y - 65536;

			agk::MouseMove( 0, x, y );

			if ( bLeftDown )
			{
#ifndef MULTI_MOUSE
				//agk::TouchMoved( 1, x,y );
#endif
			}
			break;
		}

		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = MINIMUM_WIDTH;
			lpMMI->ptMinTrackSize.y = MINIMUM_HEIGHT;
		}
		case WM_SIZE:
		{
			// if maximizing then set window to topmost to prevent it overlapping the start button when "use small icons" is ticked
			// which seems to cause image stutter
			
			//PE: Removed when we run a app it start in the background.
			//g_bShouldBeTopMost = (wParam == SIZE_MAXIMIZED);
			g_bShouldBeTopMost = false;

			if ( g_bShouldBeTopMost ) ::SetWindowPos( g_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOSIZE );
			else ::SetWindowPos( g_hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOSIZE );

			agk::UpdateDeviceSize();
			agk::WindowMoved();
			break;
		}
#ifdef TEST_DYNAMIC_RESIZING
		case WM_SIZING:
		{
			try
			{
				if (bAGKInitialized && !bAGKInMainLoop)
				{
					if (!agk::IsCapturingImage())
					{
						if (App.Loop() == 1) bExitLoop = true;
					}
					else
					{
						agk::Sleep(10);
					}
				}
			}
			catch (...)
			{
				uString err = agk::GetLastError();
				err.Prepend("(WM_SIZING) Uncaught exception: \n\n");
				MessageBoxA(NULL, err.GetStr(), "Error", 0);
				bExitLoop = true;
			}
			break;
		}
#endif

		case WM_NCHITTEST:
		{
			LRESULT result = DefWindowProc(hWnd, message, wParam, lParam);

			if ( agk::CanResize() ) return result;
			else
			{
				// filter out resize options
				if ( result == HTBOTTOM || result == HTBOTTOMLEFT || result == HTBOTTOMRIGHT 
				  || result == HTTOP || result == HTTOPLEFT || result == HTTOPRIGHT
				  || result == HTLEFT || result == HTRIGHT
				  || result == HTSIZE )
				{
					return HTBORDER;
				}
				else 
				{
					return result;
				}
			}
		}

		case WM_WINDOWPOSCHANGED:
		case WM_MOVE:
		{
			agk::WindowMoved();
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		case WM_ACTIVATE:
		{
			// if being made inactive and currently we are topmost temporarily remove topmost until we are active again.
			if ( LOWORD(wParam) != WA_INACTIVE ) 
			{
				if ( g_bShouldBeTopMost ) ::SetWindowPos( g_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOSIZE );
				agk::Resumed();
			}
			else
			{
				agk::MouseLeftButton( 0, 0 );
				agk::MouseRightButton( 0, 0 );
				agk::MouseMiddleButton( 0, 0 );
				if ( g_bShouldBeTopMost ) ::SetWindowPos( g_hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOSIZE );
				agk::Paused();
			}
			agk::WindowMoved();
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		case WM_NCACTIVATE:
		{
			if ( wParam ) agk::WindowMoved();
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		case WM_SETFOCUS:
		{
			agk::WindowMoved();
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		case WM_KEYDOWN:
		{
#ifdef MULTI_MOUSE
			if ( wParam == 17 && bCapturePointer ) // Alt Gr
			{
				bCapturePointer = false;
				ShowCursor( TRUE );
			}
#endif
			unsigned int key = TranslateKey( wParam );
			if ( key > 0 )
			{
				agk::KeyDown( key );
			}
			
			/*
			uString sKey;
			sKey.Format( "Key: %d", wParam );
			agk::Warning( sKey );
			*/

			//if ( wParam == VK_ESCAPE ) PostQuitMessage(0);
			break;
		}

		case WM_KEYUP:
		{
			unsigned int key = TranslateKey( wParam );
			if ( key > 0 )
			{
				agk::KeyUp( key );
			}
			break;
		}

		case WM_CHAR:
		{
			agk::CharDown( wParam );
			break;
		}

		case WM_UNICHAR:
		{
			if ( wParam == UNICODE_NOCHAR ) return TRUE;
			agk::CharDown( wParam );
			break;
		}

		case WM_SYSKEYDOWN:
		{
			//if ( wParam == VK_MENU ) agk::KeyDown( 18 ); // Alt
			unsigned int key = TranslateKey( wParam );
			if ( key > 0 )
			{
				agk::KeyDown( key );
			}
			break;
		}

		case WM_SYSKEYUP:
		{
			//if ( wParam == VK_MENU ) agk::KeyUp( 18 ); // Alt
			unsigned int key = TranslateKey( wParam );
			if ( key > 0 )
			{
				agk::KeyUp( key );
			}
			break;
		}

		case WM_SYSCOMMAND:
		{
			if ( wParam == SC_SIZE && !agk::CanResize() ) return 0;
			else return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}

		case WM_SOUND_EVENT:
		{
			agk::HandleMusicEvents( (void*) lParam );
			break;
		}

		case WM_VIDEO_EVENT:
		{
			agk::HandleVideoEvents();
			break;
		}
			
		case WM_COMMAND:
		{
			if ( (wParam & 0xffff) == 124 )
			{
				// capture
				agk::CaptureImage();
				agk::CancelCapture();
			}
			else if ( (wParam & 0xffff) == 125 )
			{
				// cancel
				agk::CancelCapture();
			}
			break;
		}

		// Use WM_CLOSE not WM_DESTROY to avoid a crash
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}

		case WM_ERASEBKGND:
		{
			break;
		}

		case WM_PAINT:
		{
			ValidateRect( g_hWnd, NULL );
			break;
		}

		case 0x031E: //WM_DWMCOMPOSITIONCHANGED
		{
			agk::CompositionChanged();
			break;
		}

		case WM_DISPLAYCHANGE:
		{
			extern bool g_WantUpdateMonitors;
			g_WantUpdateMonitors = true;
			break;
		}

		
		case WM_DROPFILES:
		case WM_COPYDATA:
		{
			if (message == WM_COPYDATA) {
				
				COPYDATASTRUCT * cds = (COPYDATASTRUCT *) lParam;

				char * myString = (char *)(cds->lpData);

				if (cds->cbData > 0 ) {
					uString sCopyName = myString; //make a copy asap.
					sCopyName.ReplaceStr("\"", ""); // remove quate
					char *pszFileName = (char *) sCopyName.GetStr();
					//PE: file validate it.
					uString ext = "na";
					uString m_szParse = sCopyName;
					int pos = m_szParse.RevFind('.');
					if (pos >= 0) {
						m_szParse.SubString(ext, pos); // ? jpeg = .jpe
						ext.Lower();
						if (ext.CompareTo(".txt") == 0 || ext.CompareTo(".ini") == 0 || ext.CompareTo(".log") == 0 || ext.CompareTo(".me") == 0) {
							AddSourceFile(pszFileName);
						}
						if (ext.CompareTo(".cpp") == 0 || ext.CompareTo(".c") == 0 || ext.CompareTo(".h") == 0 || ext.CompareTo(".hpp") == 0) {
							AddSourceFile(pszFileName);
						}
						if (ext.CompareTo(".fx") == 0 || ext.CompareTo(".ps") == 0 || ext.CompareTo(".vs") == 0) {
							AddSourceFile(pszFileName);
						}
						if (ext.CompareTo(".sql") == 0 || ext.CompareTo(".lua") == 0) {
							AddSourceFile(pszFileName);
						}
						if (ext.CompareTo(".agc") == 0) {
							AddSourceFile(pszFileName);
						}
						if (ext.CompareTo(".agk") == 0) {
							//Add as project.
							AddProjectFile(pszFileName);
						}

					}

//					char* CharStr = new char[strlen(myString) + 1];
//					strcpy(CharStr, myString);
//					CHAR *pszFileName = CharStr;
//					MessageBoxA(NULL, pszFileName, "COPYDATE", 0);
//					printf("Test %s", pszFileName);
//					delete CharStr;

				}
			}
			else {
				HDROP   hDrop;
				hDrop = (HDROP)wParam;
				UINT nCnt = DragQueryFileA(hDrop, (UINT)-1, NULL, 0);
				for (int nIndex = 0; nIndex < (int)nCnt; ++nIndex) {
					UINT nSize;
					if (0 == (nSize = DragQueryFileA(hDrop, nIndex, NULL, 0)))
						continue;

					CHAR *pszFileName = new CHAR[++nSize];
					if (DragQueryFileA(hDrop, nIndex, pszFileName, nSize))
					{

						if (pszFileName && strlen(pszFileName) > 0) {
							struct stat s;
							if (stat(pszFileName, &s) == 0)
							{
								if (s.st_mode & S_IFDIR)
								{
									//PE: directory add to assets browser.
									AddToAssetsBrowser(pszFileName);
								}
								else if (s.st_mode & S_IFREG)
								{
									//PE: file validate it.
									uString ext = "na";
									uString m_szParse = pszFileName;
									int pos = m_szParse.RevFind('.');
									if (pos >= 0) {
										m_szParse.SubString(ext, pos); // ? jpeg = .jpe
										ext.Lower();

										if (ext.CompareTo(".txt") == 0 || ext.CompareTo(".ini") == 0 || ext.CompareTo(".log") == 0 || ext.CompareTo(".me") == 0) {
											AddSourceFile(pszFileName);
										}
										if (ext.CompareTo(".cpp") == 0 || ext.CompareTo(".c") == 0 || ext.CompareTo(".h") == 0 || ext.CompareTo(".hpp") == 0) {
											AddSourceFile(pszFileName);
										}
										if (ext.CompareTo(".fx") == 0 || ext.CompareTo(".ps") == 0 || ext.CompareTo(".vs") == 0) {
											AddSourceFile(pszFileName);
										}
										if (ext.CompareTo(".sql") == 0 || ext.CompareTo(".lua") == 0) {
											AddSourceFile(pszFileName);
										}
										if (ext.CompareTo(".agc") == 0) {
											AddSourceFile(pszFileName);
										}
										if (ext.CompareTo(".agk") == 0) {
											//Add as project.
											AddProjectFile(pszFileName);
										}

									}
								}
							}
						}
					}
					delete[] pszFileName;
				}
				DragFinish(hDrop);
			}
			break;
		}

		default:
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}

HWND CreateWin32Window( HINSTANCE hInstance, int width, int height, uString &szTitle, int x, int y, bool fullscreen )
{
	HANDLE hIcon = LoadImageA(NULL, "agkstudio.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

	// register the class
	WNDCLASSEXW wcex;

	// hardcoded resource IDs for icons
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= hIcon ? (HICON)hIcon : LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	wcex.hCursor = NULL; //PE: Must be NULL so we can change th mouse in the GUI. //LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= L"OPENGLWINDOWAGKSTUDIO";
	wcex.hIconSm		= NULL;
	RegisterClassExW(&wcex);

	hCursorArrow = LoadCursor(NULL, IDC_ARROW);
	// check the given values against the desktop resolution and center if requested (x=-1 or y=-1)
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);

	if ( width < 1 ) width = 1;
	if ( height < 1 ) height = 1;

	float appAspect = width / (float) height;
	float windowAspect = (rc.right-rc.left-15) / (float) (rc.bottom-rc.top-80);

	if ( appAspect > windowAspect )
	{
		if ( width > rc.right-rc.left-15 )
		{
			float ratio = (rc.right-rc.left-15) / (float)width;
			width = rc.right-rc.left-15;
			height = (int) (height*ratio);
		}
	}
	else
	{
		if ( height > rc.bottom-rc.top-80 )
		{
			float ratio = (rc.bottom-rc.top-80) / (float)height;
			height = rc.bottom-rc.top-80;
			width = (int) (width*ratio);
		}
	}
	
	if ( x == -1 ) x = ((rc.right-rc.left)-width)/2 + rc.left;
	if ( y == -1 ) y = ((rc.bottom-rc.top-50)-height)/2 + rc.top;
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;
	if ( x+width > rc.right ) x = rc.right-width;
	if ( y+height > rc.bottom ) y = rc.bottom-height;

	if ( fullscreen )												
    {
		width = rc.right-rc.left;
		height = rc.bottom-rc.top;
		x = 0;
		y = 0;

		/*
    	DEVMODE dmScreenSettings;								// Device Mode
    	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
    	dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
    	dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
    	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    	// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
    	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
    	{
    		fullscreen = false;
    	}
		*/
    }

	DWORD dwExStyle;
	DWORD dwStyle;

	if ( fullscreen )											
    {
    	dwExStyle = WS_EX_APPWINDOW;							
    	dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;	
    }
    else
    {
    	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	
    	dwStyle = WS_OVERLAPPEDWINDOW;// & (~WS_SIZEBOX);	// removing WS_SIZEBOX reduces the size of the title bar, which then increases in size itself on the first window resize
    }


	// adjust the window and make it bigger for borders, window title, etc
	RECT WindowRect;							
	WindowRect.left=(long)0;					
	WindowRect.right=(long)width;				
	WindowRect.top=(long)0;						
	WindowRect.bottom=(long)height;				
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	// initialise window
#ifdef TRIALVERSIONEXPIRES
	HWND hWnd = CreateWindowW(L"OPENGLWINDOWAGKSTUDIO", LWINDOW_TRIALTITLE, dwStyle, x, y,
		WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, hInstance, NULL);
#else
	HWND hWnd = CreateWindowW(L"OPENGLWINDOWAGKSTUDIO", LWINDOW_TITLE, dwStyle, x, y,
		WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, NULL, NULL, hInstance, NULL);
#endif

	// terminate if failed to create window
	if (!hWnd) return FALSE;
	g_hWnd = hWnd;
	// show window
	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

#ifdef MULTI_MOUSE
	// Register that we want raw input from mice
	RAWINPUTDEVICE pDevices;
	pDevices.usUsage = 2;
	pDevices.usUsagePage = 1;
	pDevices.hwndTarget = hWnd;
	pDevices.dwFlags = 0;
	RegisterRawInputDevices( &pDevices, 1, sizeof(RAWINPUTDEVICE) );
#endif

	LoadDelayedFunctions();
	if ( RegisterTouchWindowDelay ) RegisterTouchWindowDelay(hWnd,TWF_WANTPALM);

	// success
	return hWnd;
}

void LoadAgkIdeIni(int initPart = 1);
extern int screenWidth, screenHeight;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	//Only allow one instance of app to run.
	try {
		// Try to open the mutex.
		hMutex = OpenMutex(
			MUTEX_ALL_ACCESS, 0, L"AppGameKitStudioUniqueId20427509String");

		if (!hMutex)
			// Mutex doesn’t exist. This is
			// the first instance so create
			// the mutex.
			hMutex =
			CreateMutex(0, 0, L"AppGameKitStudioUniqueId20427509String");
		else {
			// The mutex exists so this is the the second instance.
			HWND hWnd = FindWindow(
				L"OPENGLWINDOWAGKSTUDIO", NULL);
			if (hWnd != NULL) {
				SetForegroundWindow(hWnd);

				if (lpCmdLine && _tcslen(lpCmdLine) > 0) {
					COPYDATASTRUCT cds;
					size_t size = wcstombs(NULL, lpCmdLine, 0);
					char* CharStr = new char[size + 1];
					wcstombs(CharStr, lpCmdLine, size + 1);
					CharStr[size] = 0;
					cds.cbData = strlen(CharStr); //_tcslen(lpCmdLine) + 1;
					cds.lpData = CharStr; //lpCmdLine;
					SendMessageA(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
					Sleep(1000); //Small delay before exit. just in case SendMessageA return early for some reason.
					delete CharStr;
				}

				return 0;
			}
			//If we cant find the CLASS name , just allow it to run.
		}

	}
	catch (...)
	{
		//Just continue.
	}
	

	int dwCenterWindowX = (GetSystemMetrics(SM_CXSCREEN) - (int)DEVICE_WIDTH) / 2;
	int dwCenterWindowY = (GetSystemMetrics(SM_CYSCREEN) - (int)DEVICE_HEIGHT) / 2;

#ifdef STARTUPPROBLEM
	MessageBoxA(NULL, "_tWinMain", "Error", 0);
#endif

	// create a win32 window for the app
	HWND hWnd = CreateWin32Window( hInstance, DEVICE_WIDTH, DEVICE_HEIGHT, uString("AGK"), dwCenterWindowX, dwCenterWindowY, FULLSCREEN );
	agk::SetCompanyName( COMPANY_NAME );

	// call app begin
	try
	{
		// initialise graphics API (win32 openGL) for app
#ifdef STARTUPPROBLEM
		MessageBoxA(NULL, "InitGL", "Error", 0);
#endif

		agk::InitGL( (void*) hWnd );

/*
GLES version                  GLSL version
2.0                            1.00 ES
3.0                            3.00 ES
3.1                            3.10 ES
GL version                     GLSL version
2.0                            1.10
2.1                            1.20
3.0                            1.30
3.1                            1.40
3.2                            1.50
3.3                            3.30
4.0                            4.00
4.1                            4.10
4.2                            4.20
4.3                            4.30
4.4                            4.40
4.5                            4.50

So GLES 3.1: gl_maj_version=3+ , gl_min_version=1

*/
//		static int gl_maj_version, gl_min_version, gl_profile, gl_flags;
//		agk::glGetVersionInfo(&gl_maj_version, &gl_min_version, &gl_profile, &gl_flags);

		agk_hWnd = hWnd;
#ifdef STARTUPPROBLEM
		MessageBoxA(NULL, "App.Begin", "Error", 0);
#endif

		App.Begin(agk_hWnd);

		if (screenWidth > 150 && screenHeight > 150 && screenWidth < 9999 && screenHeight < 9999) {
			dwCenterWindowX = (GetSystemMetrics(SM_CXSCREEN) - (int)screenWidth) / 2;
			dwCenterWindowY = (GetSystemMetrics(SM_CYSCREEN) - (int)screenHeight) / 2;
			::SetWindowPos(g_hWnd, 0, dwCenterWindowX, dwCenterWindowY, screenWidth, screenHeight , SWP_NOZORDER);

		}
		ShowWindow(hWnd, SW_SHOW);

	}
	catch(...)
	{
		uString err = agk::GetLastError();
		err.Prepend( "Uncaught exception: \n\n" );
		MessageBoxA( NULL, err.GetStr(), "Error", 0 );
		bExitLoop = true;
	}


	// message pump
	MSG msg;
	bAGKInitialized = true;

#ifdef STARTUPPROBLEM
	MessageBoxA(NULL, "bAGKInitialized", "Error", 0);
#endif

	DragAcceptFiles(agk_hWnd, TRUE);

	while ( !bExitLoop )
	{
		if ( PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) )
		{	
			if (msg.message == WM_QUIT) {

#ifdef TRIALVERSIONEXPIRES
				extern int trialaction;
				trialaction = 9;
#else
				bExitLoop = true;
#endif

				// message 799 is sent just after a change to fullscreen mode and for some reason sets it back to windowed mode.
				// update: but it doesn't seem to cause a problem after reinstating the WS_SIZEBOX style
				//if ( msg.message == 799 ) continue;
#ifdef STARTUPPROBLEM
				MessageBoxA(NULL, "PM_REMOVE,WM_QUIT", "Error", 0);
#endif

				if ( CancelQuit() ) {
					bExitLoop = false;
				}
			}
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else 
		{
			// call app each frame
			try
			{
				if (bAllProjectsHasBeenProcessed && !bCmdLineProcessed ) {
					//Send commandline parameters to ourself.
					if (lpCmdLine && _tcslen(lpCmdLine) > 0) {
						COPYDATASTRUCT cds;
						size_t size = wcstombs(NULL, lpCmdLine, 0);
						char* CharStr = new char[size + 1];
						wcstombs(CharStr, lpCmdLine, size + 1);
						CharStr[size] = 0;
						cds.cbData = strlen(CharStr); //_tcslen(lpCmdLine) + 1;
						cds.lpData = CharStr; //lpCmdLine;
						SendMessageA(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
						delete CharStr;
					}
					bCmdLineProcessed = true;
				}
				bAGKInMainLoop = true;
				if ( !agk::IsCapturingImage() ) 
				{
#ifdef STARTUPPROBLEM
					MessageBoxA(NULL, "App.Loop", "Error", 0);
#endif

					if ( App.Loop() == 1 ) bExitLoop = true;
				}
				else
				{
#ifdef STARTUPPROBLEM
					MessageBoxA(NULL, "IsCapturingImage", "Error", 0);
#endif

					agk::Sleep( 10 );
				}
				bAGKInMainLoop = false;
			}
			catch(...)
			{
				uString err = agk::GetLastError();
				err.Prepend( "Uncaught exception: \n\n" );
				MessageBoxA( NULL, err.GetStr(), "Error", 0 );
				bExitLoop = true;
			}
		}
	}

#ifdef STARTUPPROBLEM
	MessageBoxA(NULL, "App.End", "Error", 0);
#endif

	DragAcceptFiles(agk_hWnd, FALSE);

	// call app end
	App.End();

	//closing so release the mutex.
	ReleaseMutex(hMutex);

	agk::CleanUp();

	// finished
	return 0;
}

