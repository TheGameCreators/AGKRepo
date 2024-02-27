// imgui: Platform Binding for GLFW
//
//PE: We need to update agk here , as we are getting the callbacks.
//

#include "agk.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"

extern bool os_messages_support;
extern unsigned int os_messages;
extern unsigned int os_messages_old;
extern unsigned int os_messages_wait;
extern unsigned int os_messages_sleeptime;


// GLFW
//#include <GLFW/glfw3.h>
#ifdef AGK_MACOS
#include "../../../platform/mac/GLFW/glfw3.h"
#endif
#ifdef AGK_LINUX
#include "GLFW/glfw3.h"
#endif

#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>   // for glfwGetWin32Window
#endif
#define GLFW_HAS_WINDOW_TOPMOST     (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED     (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI    (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorContentScale
#define GLFW_HAS_VULKAN             (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwCreateWindowSurface
#define GLFW_HAS_FOCUS_WINDOW       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwFocusWindow

// Data
enum GlfwClientApi
{
    GlfwClientApi_Unknown,
    GlfwClientApi_OpenGL,
    GlfwClientApi_Vulkan
};
static GLFWwindow*      g_Window = NULL;
static GlfwClientApi    g_ClientApi = GlfwClientApi_Unknown;
static double           g_Time = 0.0;
static bool             g_MouseJustPressed[5] = { false, false, false, false, false };
static GLFWcursor*      g_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };
static bool             g_WantUpdateMonitors = true;

// Chain GLFW callbacks for main viewport: our callbacks will call the user's previously installed callbacks, if any.
static GLFWmousebuttonfun   g_PrevUserCallbackMousebutton = NULL;
static GLFWscrollfun        g_PrevUserCallbackScroll = NULL;
static GLFWkeyfun           g_PrevUserCallbackKey = NULL;
static GLFWcharfun          g_PrevUserCallbackChar = NULL;

// Forward Declarations
static void ImGui_ImplGlfw_InitPlatformInterface();
static void ImGui_ImplGlfw_ShutdownPlatformInterface();
static void ImGui_ImplGlfw_UpdateMonitors();

static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data)
{
    return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
{
    glfwSetClipboardString((GLFWwindow*)user_data, text);
}

void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (g_PrevUserCallbackMousebutton != NULL && window == g_Window)
        g_PrevUserCallbackMousebutton(window, button, action, mods);

    if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
        g_MouseJustPressed[button] = true;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS) agk::MouseLeftButton(0, 1);
		else if (action == GLFW_RELEASE) agk::MouseLeftButton(0, 0);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS) agk::MouseRightButton(0, 1);
		else if (action == GLFW_RELEASE) agk::MouseRightButton(0, 0);
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if (action == GLFW_PRESS) agk::MouseMiddleButton(0, 1);
		else if (action == GLFW_RELEASE) agk::MouseMiddleButton(0, 0);
	}
}

void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (g_PrevUserCallbackScroll != NULL && window == g_Window)
        g_PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

// platform specific
unsigned int iTranslateKey(unsigned int key)
{
	switch (key)
	{
	case GLFW_KEY_BACKSPACE: key = 8; break;
	case GLFW_KEY_TAB: key = 9; break;
	case GLFW_KEY_ENTER: key = 13; break;
	case GLFW_KEY_KP_ENTER: key = 13; break; // Num pad enter
	case GLFW_KEY_LEFT_SHIFT: key = 257; break; // 16
	case GLFW_KEY_RIGHT_SHIFT: key = 258; break; // 16
	case GLFW_KEY_LEFT_CONTROL: key = 259; break; // 17
	case GLFW_KEY_RIGHT_CONTROL: key = 260; break; // 17
	case GLFW_KEY_LEFT_ALT: key = 261; break;  // 18
	case GLFW_KEY_RIGHT_ALT: key = 262; break;  // 18
	case GLFW_KEY_PAUSE: key = 19; break;
	case GLFW_KEY_CAPS_LOCK: key = 20; break;
	case GLFW_KEY_ESCAPE: key = 27; break;

	case GLFW_KEY_SPACE: key = 32; break;
	case GLFW_KEY_PAGE_UP: key = 33; break;
	case GLFW_KEY_PAGE_DOWN: key = 34; break;
	case GLFW_KEY_END: key = 35; break;
	case GLFW_KEY_HOME: key = 36; break;

	case GLFW_KEY_LEFT: key = 37; break;
	case GLFW_KEY_UP: key = 38; break;
	case GLFW_KEY_RIGHT: key = 39; break;
	case GLFW_KEY_DOWN: key = 40; break;
	case GLFW_KEY_INSERT: key = 45; break;
	case GLFW_KEY_DELETE: key = 46; break;

	case GLFW_KEY_LEFT_SUPER: key = 91; break;
	case GLFW_KEY_RIGHT_SUPER: key = 92; break;

		// Numpad
	case GLFW_KEY_KP_0: key = 96; break;
	case GLFW_KEY_KP_1: key = 97; break;
	case GLFW_KEY_KP_2: key = 98; break;
	case GLFW_KEY_KP_3: key = 99; break;
	case GLFW_KEY_KP_4: key = 100; break;
	case GLFW_KEY_KP_5: key = 101; break;
	case GLFW_KEY_KP_6: key = 102; break;
	case GLFW_KEY_KP_7: key = 103; break;
	case GLFW_KEY_KP_8: key = 104; break;
	case GLFW_KEY_KP_9: key = 105; break;
	case GLFW_KEY_KP_MULTIPLY: key = 106; break;
	case GLFW_KEY_KP_ADD: key = 107; break;
	case GLFW_KEY_KP_SUBTRACT: key = 109; break;
	case GLFW_KEY_KP_DECIMAL: key = 110; break;
	case GLFW_KEY_KP_DIVIDE: key = 111; break;

		// F1 - F12
	case GLFW_KEY_F1: key = 112; break;
	case GLFW_KEY_F2: key = 113; break;
	case GLFW_KEY_F3: key = 114; break;
	case GLFW_KEY_F4: key = 115; break;
	case GLFW_KEY_F5: key = 116; break;
	case GLFW_KEY_F6: key = 117; break;
	case GLFW_KEY_F7: key = 118; break;
	case GLFW_KEY_F8: key = 119; break;
	case GLFW_KEY_F9: key = 120; break;
	case GLFW_KEY_F10: key = 121; break;
	case GLFW_KEY_F11: key = 122; break;
	case GLFW_KEY_F12: key = 123; break;

	case GLFW_KEY_NUM_LOCK: key = 144; break;
	case GLFW_KEY_SCROLL_LOCK: key = 145; break;

	case GLFW_KEY_SEMICOLON: key = 186; break; // ;
	case GLFW_KEY_EQUAL: key = 187; break;
	case GLFW_KEY_COMMA: key = 188; break;
	case GLFW_KEY_MINUS: key = 189; break;
	case GLFW_KEY_PERIOD: key = 190; break;
	case GLFW_KEY_SLASH: key = 191; break;
	case GLFW_KEY_APOSTROPHE: key = 192; break; // '
	case GLFW_KEY_LEFT_BRACKET: key = 219; break; // [
	case GLFW_KEY_BACKSLASH: key = 220; break;
	case GLFW_KEY_RIGHT_BRACKET: key = 221; break; // ]
	case GLFW_KEY_WORLD_1: key = 222; break; // # (maybe)
	case GLFW_KEY_WORLD_2: key = 222; break; // # (maybe)
	case GLFW_KEY_GRAVE_ACCENT: key = 223; break; // `
	}

	return key;
}

void ImGui_ImplGlfw_KeyCallback(GLFWwindow*, int key, int, int action, int mods)
{
	os_messages += 3;
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    (void)mods; // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	if (action == GLFW_PRESS)
	{
		int key2 = iTranslateKey(key);
		agk::KeyDown(key2);

		switch (key2)
		{
			// Top row 0-9
		case 48: agk::KeyDown(263); break;
		case 49: agk::KeyDown(264); break;
		case 50: agk::KeyDown(265); break;
		case 51: agk::KeyDown(266); break;
		case 52: agk::KeyDown(267); break;
		case 53: agk::KeyDown(268); break;
		case 54: agk::KeyDown(269); break;
		case 55: agk::KeyDown(270); break;
		case 56: agk::KeyDown(271); break;
		case 57: agk::KeyDown(272); break;

			// Num pad 0-9
		case 96: agk::KeyDown(48); break;
		case 97: agk::KeyDown(49); break;
		case 98: agk::KeyDown(50); break;
		case 99: agk::KeyDown(51); break;
		case 100: agk::KeyDown(52); break;
		case 101: agk::KeyDown(53); break;
		case 102: agk::KeyDown(54); break;
		case 103: agk::KeyDown(55); break;
		case 104: agk::KeyDown(56); break;
		case 105: agk::KeyDown(57); break;

			// shift
		case 257:
		case 258: agk::KeyDown(16); break;

			// control
		case 259:
		case 260: agk::KeyDown(17); break;

			// alt
		case 261:
		case 262: agk::KeyDown(18); break;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		int key2 = iTranslateKey(key);
		agk::KeyUp(key2);

		switch (key2)
		{
			// Top row 0-9
		case 48: agk::KeyUp(263); break;
		case 49: agk::KeyUp(264); break;
		case 50: agk::KeyUp(265); break;
		case 51: agk::KeyUp(266); break;
		case 52: agk::KeyUp(267); break;
		case 53: agk::KeyUp(268); break;
		case 54: agk::KeyUp(269); break;
		case 55: agk::KeyUp(270); break;
		case 56: agk::KeyUp(271); break;
		case 57: agk::KeyUp(272); break;

			// Num pad 0-9
		case 96: agk::KeyUp(48); break;
		case 97: agk::KeyUp(49); break;
		case 98: agk::KeyUp(50); break;
		case 99: agk::KeyUp(51); break;
		case 100: agk::KeyUp(52); break;
		case 101: agk::KeyUp(53); break;
		case 102: agk::KeyUp(54); break;
		case 103: agk::KeyUp(55); break;
		case 104: agk::KeyUp(56); break;
		case 105: agk::KeyUp(57); break;

			// shift
		case 257:
		case 258: agk::KeyUp(16); break;

			// control
		case 259:
		case 260: agk::KeyUp(17); break;

			// alt
		case 261:
		case 262: agk::KeyUp(18); break;
		}
	}

}

void ImGui_ImplGlfw_CharCallback(GLFWwindow* window, unsigned int c)
{
    if (g_PrevUserCallbackChar != NULL && window == g_Window)
        g_PrevUserCallbackChar(window, c);

    ImGuiIO& io = ImGui::GetIO();
    if (c > 0 && c < 0x10000)
        io.AddInputCharacter((unsigned short)c);

	agk::CharDown(c);

}

void ImGui_ImplGlfw_InstallCallbacks(GLFWwindow* window)
{
    glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
    glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
    glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
    glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
}

/* Missing

static void mouse_pos_callback( GLFWwindow* window, double x, double y )
{
agk::MouseMove( 0, agk::Round(x), agk::Round(y) );
}

static void mouse_wheel_callback( GLFWwindow* window, double x, double y )
{
agk::MouseWheel(0, y);
}

static void window_size_callback( GLFWwindow* window, int width, int height)
{
agk::UpdateDeviceSize();
}

static void focus_callback( GLFWwindow* window, int focus )
{
if ( focus == GLFW_TRUE ) agk::Resumed();
else agk::Paused();
}

*/


static bool ImGui_ImplGlfw_Init(GLFWwindow* window, bool install_callbacks, GlfwClientApi client_api)
{
    g_Window = window;
    g_Time = 0.0;

    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
//PE: 0.46 check if this is causing multipy monitors setup to draw black screen.
    io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
#if GLFW_HAS_GLFW_HOVERED && defined(_WIN32)
    io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can set io.MouseHoveredViewport correctly (optional, not easy)
#endif
    io.BackendPlatformName = "imgui_impl_glfw";

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io.ClipboardUserData = g_Window;

    g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);   // FIXME: GLFW doesn't have this.
    g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);  // FIXME: GLFW doesn't have this.
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);  // FIXME: GLFW doesn't have this.
    g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    
    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    g_PrevUserCallbackMousebutton = NULL;
    g_PrevUserCallbackScroll = NULL;
    g_PrevUserCallbackKey = NULL;
    g_PrevUserCallbackChar = NULL;
    if (install_callbacks)
    {
        g_PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
        g_PrevUserCallbackScroll = glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
        g_PrevUserCallbackKey = glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
        g_PrevUserCallbackChar = glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
    }

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = (void*)g_Window;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplGlfw_InitPlatformInterface();

    g_ClientApi = client_api;
    return true;
}

bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_OpenGL);
}

bool ImGui_ImplGlfw_InitForVulkan(GLFWwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Vulkan);
}

void ImGui_ImplGlfw_Shutdown()
{
    ImGui_ImplGlfw_ShutdownPlatformInterface();

    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    {
        glfwDestroyCursor(g_MouseCursors[cursor_n]);
        g_MouseCursors[cursor_n] = NULL;
    }
    g_ClientApi = GlfwClientApi_Unknown;
}

static void ImGui_ImplGlfw_UpdateMousePosAndButtons()
{
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    io.MouseHoveredViewport = 0;

    // Update buttons
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
        g_MouseJustPressed[i] = false;
    }

    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int n = 0; n < platform_io.Viewports.Size; n++)
    {
        ImGuiViewport* viewport = platform_io.Viewports[n];
        GLFWwindow* window = (GLFWwindow*)viewport->PlatformHandle;
        IM_ASSERT(window != NULL);
#ifdef __EMSCRIPTEN__
        const bool focused = true;
        IM_ASSERT(platform_io.Viewports.Size == 1);
#else
        const bool focused = glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;
#endif
        if (focused)
        {
            if (io.WantSetMousePos)
            {
                glfwSetCursorPos(window, (double)(mouse_pos_backup.x - viewport->Pos.x), (double)(mouse_pos_backup.y - viewport->Pos.y));
            }
            else
            {
                double mouse_x, mouse_y;
                glfwGetCursorPos(window, &mouse_x, &mouse_y);
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    // Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
                    int window_x, window_y;
                    glfwGetWindowPos(window, &window_x, &window_y);
                    io.MousePos = ImVec2((float)mouse_x + window_x, (float)mouse_y + window_y);
                }
                else
                {
                    // Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
                    io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
                }
            }
            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
                io.MouseDown[i] |= glfwGetMouseButton(window, i) != 0;
        }

        // (Optional) When using multiple viewports: set io.MouseHoveredViewport to the viewport the OS mouse cursor is hovering.
        // Important: this information is not easy to provide and many high-level windowing library won't be able to provide it correctly, because
        // - This is _ignoring_ viewports with the ImGuiViewportFlags_NoInputs flag (pass-through windows).
        // - This is _regardless_ of whether another viewport is focused or being dragged from.
        // If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the back-end, imgui will ignore this field and infer the information by relying on the
        // rectangles and last focused time of every viewports it knows about. It will be unaware of other windows that may be sitting between or over your windows.
        // [GLFW] FIXME: This is currently only correct on Win32. See what we do below with the WM_NCHITTEST, missing an equivalent for other systems.
        // See https://github.com/glfw/glfw/issues/1236 if you want to help in making this a GLFW feature.
#if GLFW_HAS_GLFW_HOVERED && defined(_WIN32)
        if (glfwGetWindowAttrib(window, GLFW_HOVERED) && !(viewport->Flags & ImGuiViewportFlags_NoInputs))
            io.MouseHoveredViewport = viewport->ID;
#endif
    }
}

static void ImGui_ImplGlfw_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(g_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int n = 0; n < platform_io.Viewports.Size; n++)
    {
        GLFWwindow* window = (GLFWwindow*)platform_io.Viewports[n]->PlatformHandle;
        if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else
        {
            // Show OS mouse cursor
            // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
            glfwSetCursor(window, g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void ImGui_ImplGlfw_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt());     // Font atlas needs to be built, call renderer _NewFrame() function e.g. ImGui_ImplOpenGL3_NewFrame() 

    // Setup display size
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(g_Window, &w, &h);
    glfwGetFramebufferSize(g_Window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);
    if (g_WantUpdateMonitors)
        ImGui_ImplGlfw_UpdateMonitors();

    // Setup time step
	//io.DeltaTime moved to ide.c
//    double current_time = glfwGetTime();
//    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
//    g_Time = current_time;

    ImGui_ImplGlfw_UpdateMousePosAndButtons();
    ImGui_ImplGlfw_UpdateMouseCursor();

    // Gamepad navigation mapping
    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)
    {
        // Update gamepad inputs
        #define MAP_BUTTON(NAV_NO, BUTTON_NO)       { if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) io.NavInputs[NAV_NO] = 1.0f; }
        #define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); if (v > 1.0f) v = 1.0f; if (io.NavInputs[NAV_NO] < v) io.NavInputs[NAV_NO] = v; }
        int axes_count = 0, buttons_count = 0;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
        MAP_BUTTON(ImGuiNavInput_Activate,   0);     // Cross / A
        MAP_BUTTON(ImGuiNavInput_Cancel,     1);     // Circle / B
        MAP_BUTTON(ImGuiNavInput_Menu,       2);     // Square / X
        MAP_BUTTON(ImGuiNavInput_Input,      3);     // Triangle / Y
        MAP_BUTTON(ImGuiNavInput_DpadLeft,   13);    // D-Pad Left
        MAP_BUTTON(ImGuiNavInput_DpadRight,  11);    // D-Pad Right
        MAP_BUTTON(ImGuiNavInput_DpadUp,     10);    // D-Pad Up
        MAP_BUTTON(ImGuiNavInput_DpadDown,   12);    // D-Pad Down
        MAP_BUTTON(ImGuiNavInput_FocusPrev,  4);     // L1 / LB
        MAP_BUTTON(ImGuiNavInput_FocusNext,  5);     // R1 / RB
        MAP_BUTTON(ImGuiNavInput_TweakSlow,  4);     // L1 / LB
        MAP_BUTTON(ImGuiNavInput_TweakFast,  5);     // R1 / RB
        MAP_ANALOG(ImGuiNavInput_LStickLeft, 0,  -0.3f,  -0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickRight,0,  +0.3f,  +0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickUp,   1,  +0.3f,  +0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickDown, 1,  -0.3f,  -0.9f);
        #undef MAP_BUTTON
        #undef MAP_ANALOG
        if (axes_count > 0 && buttons_count > 0)
            io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
        else
            io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
    }
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

struct ImGuiViewportDataGlfw
{
    GLFWwindow* Window;
    bool        WindowOwned;

    ImGuiViewportDataGlfw() { Window = NULL; WindowOwned = false; }
    ~ImGuiViewportDataGlfw() { IM_ASSERT(Window == NULL); }
};

static void ImGui_ImplGlfw_WindowCloseCallback(GLFWwindow* window)
{
    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        viewport->PlatformRequestClose = true;
}

static void ImGui_ImplGlfw_WindowPosCallback(GLFWwindow* window, int, int)
{
    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        viewport->PlatformRequestMove = true;
}

static void ImGui_ImplGlfw_WindowSizeCallback(GLFWwindow* window, int, int)
{
    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        viewport->PlatformRequestResize = true;
}

static void ImGui_ImplGlfw_CreateWindow(ImGuiViewport* viewport)
{
    ImGuiViewportDataGlfw* data = IM_NEW(ImGuiViewportDataGlfw)();
    viewport->PlatformUserData = data;

    // GLFW 3.2 unfortunately always set focus on glfwCreateWindow() if GLFW_VISIBLE is set, regardless of GLFW_FOCUSED
    glfwWindowHint(GLFW_VISIBLE, false);
    glfwWindowHint(GLFW_FOCUSED, false);
    glfwWindowHint(GLFW_DECORATED, (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? false : true);
#if GLFW_HAS_WINDOW_TOPMOST
    glfwWindowHint(GLFW_FLOATING, (viewport->Flags & ImGuiViewportFlags_TopMost) ? true : false);
#endif
    GLFWwindow* share_window = (g_ClientApi == GlfwClientApi_OpenGL) ? g_Window : NULL;
    data->Window = glfwCreateWindow((int)viewport->Size.x, (int)viewport->Size.y, "No Title Yet", NULL, share_window);
    data->WindowOwned = true;
    viewport->PlatformHandle = (void*)data->Window;
    glfwSetWindowPos(data->Window, (int)viewport->Pos.x, (int)viewport->Pos.y);

    // Install callbacks for secondary viewports
    glfwSetMouseButtonCallback(data->Window, ImGui_ImplGlfw_MouseButtonCallback);
    glfwSetScrollCallback(data->Window, ImGui_ImplGlfw_ScrollCallback);
    glfwSetKeyCallback(data->Window, ImGui_ImplGlfw_KeyCallback);
    glfwSetCharCallback(data->Window, ImGui_ImplGlfw_CharCallback);
    glfwSetWindowCloseCallback(data->Window, ImGui_ImplGlfw_WindowCloseCallback);
    glfwSetWindowPosCallback(data->Window, ImGui_ImplGlfw_WindowPosCallback);
    glfwSetWindowSizeCallback(data->Window, ImGui_ImplGlfw_WindowSizeCallback);
    if (g_ClientApi == GlfwClientApi_OpenGL)
    {
        glfwMakeContextCurrent(data->Window);
        glfwSwapInterval(0);
    }
}

static void ImGui_ImplGlfw_DestroyWindow(ImGuiViewport* viewport)
{
    if (ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData)
    {
        if (data->WindowOwned)
        {
#if GLFW_HAS_GLFW_HOVERED && defined(_WIN32)
            HWND hwnd = glfwGetWin32Window(data->Window);
            ::RemovePropA(hwnd, "IMGUI_VIEWPORT");
#endif
            glfwDestroyWindow(data->Window);
        }
        data->Window = NULL;
        IM_DELETE(data);
    }
    viewport->PlatformUserData = viewport->PlatformHandle = NULL;
}

#if defined(_WIN32) && GLFW_HAS_GLFW_HOVERED
static WNDPROC g_GlfwWndProc = NULL;
static LRESULT CALLBACK WndProcNoInputs(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCHITTEST)
    {
        // Let mouse pass-through the window. This will allow the back-end to set io.MouseHoveredViewport properly (which is OPTIONAL).
        // The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect the window behind the one we are dragging.
        // If you cannot easily access those viewport flags from your windowing/event code: you may manually synchronize its state e.g. in
        // your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
        ImGuiViewport* viewport = (ImGuiViewport*)::GetPropA(hWnd, "IMGUI_VIEWPORT");
        if (viewport->Flags & ImGuiViewportFlags_NoInputs)
            return HTTRANSPARENT;
    }
    return ::CallWindowProc(g_GlfwWndProc, hWnd, msg, wParam, lParam);
}
#endif

static void ImGui_ImplGlfw_ShowWindow(ImGuiViewport* viewport)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;

#if defined(_WIN32)
    // GLFW hack: Hide icon from task bar
    HWND hwnd = glfwGetWin32Window(data->Window);
    if (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon)
    {
        LONG ex_style = ::GetWindowLong(hwnd, GWL_EXSTYLE);
        ex_style &= ~WS_EX_APPWINDOW;
        ex_style |= WS_EX_TOOLWINDOW;
        ::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
    }

    // GLFW hack: install hook for WM_NCHITTEST message handler
#if GLFW_HAS_GLFW_HOVERED && defined(_WIN32)
    ::SetPropA(hwnd, "IMGUI_VIEWPORT", viewport);
    if (g_GlfwWndProc == NULL)
        g_GlfwWndProc = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProcNoInputs);
#endif

    // GLFW hack: GLFW 3.2 has a bug where glfwShowWindow() also activates/focus the window.
    // The fix was pushed to GLFW repository on 2018/01/09 and should be included in GLFW 3.3. See https://github.com/glfw/glfw/issues/1179
    if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
    {
        ::ShowWindow(hwnd, SW_SHOWNA);
        return;
    }
#endif

    glfwShowWindow(data->Window);
}

static ImVec2 ImGui_ImplGlfw_GetWindowPos(ImGuiViewport* viewport)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    int x = 0, y = 0;
    glfwGetWindowPos(data->Window, &x, &y);
    return ImVec2((float)x, (float)y);
}

static void ImGui_ImplGlfw_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    glfwSetWindowPos(data->Window, (int)pos.x, (int)pos.y);
}

static ImVec2 ImGui_ImplGlfw_GetWindowSize(ImGuiViewport* viewport)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    int w = 0, h = 0;
    glfwGetWindowSize(data->Window, &w, &h);
    return ImVec2((float)w, (float)h);
}

static void ImGui_ImplGlfw_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    glfwSetWindowSize(data->Window, (int)size.x, (int)size.y);
}

static void ImGui_ImplGlfw_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    glfwSetWindowTitle(data->Window, title);
}

static void ImGui_ImplGlfw_SetWindowFocus(ImGuiViewport* viewport)
{
#if GLFW_HAS_FOCUS_WINDOW
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    glfwFocusWindow(data->Window);
#else
    // FIXME: What are the effect of not having this function? At the moment imgui doesn't actually call SetWindowFocus - we set that up ahead, will answer that question later.
    (void)viewport;
#endif
}

static bool ImGui_ImplGlfw_GetWindowFocus(ImGuiViewport* viewport)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    return glfwGetWindowAttrib(data->Window, GLFW_FOCUSED) != 0;
}

static bool ImGui_ImplGlfw_GetWindowMinimized(ImGuiViewport* viewport)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    return glfwGetWindowAttrib(data->Window, GLFW_ICONIFIED) != 0;
}

#if GLFW_HAS_WINDOW_ALPHA
static void ImGui_ImplGlfw_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    glfwSetWindowOpacity(data->Window, alpha);
}
#endif

static void ImGui_ImplGlfw_RenderWindow(ImGuiViewport* viewport, void*)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    if (g_ClientApi == GlfwClientApi_OpenGL)
        glfwMakeContextCurrent(data->Window);
}

static void ImGui_ImplGlfw_SwapBuffers(ImGuiViewport* viewport, void*)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    if (g_ClientApi == GlfwClientApi_OpenGL)
        glfwSwapBuffers(data->Window);
}

//--------------------------------------------------------------------------------------------------------
// IME (Input Method Editor) basic support for e.g. Asian language users
//--------------------------------------------------------------------------------------------------------

// We provide a Win32 implementation because this is such a common issue for IME users
#if defined(_WIN32) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS) && !defined(IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS) && !defined(__GNUC__)
#define HAS_WIN32_IME   1
#include <imm.h>
#ifdef _MSC_VER
#pragma comment(lib, "imm32")
#endif
static void ImGui_ImplWin32_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos)
{
    COMPOSITIONFORM cf = { CFS_FORCE_POSITION, { (LONG)(pos.x - viewport->Pos.x), (LONG)(pos.y - viewport->Pos.y) }, { 0, 0, 0, 0 } };
    if (ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData)
        if (HWND hwnd = glfwGetWin32Window(data->Window))
            if (HIMC himc = ::ImmGetContext(hwnd))
            {
                ::ImmSetCompositionWindow(himc, &cf);
                ::ImmReleaseContext(hwnd, himc);
            }
}
#else
#define HAS_WIN32_IME   0
#endif

//--------------------------------------------------------------------------------------------------------
// Vulkan support (the Vulkan renderer needs to call a platform-side support function to create the surface)
//--------------------------------------------------------------------------------------------------------

// Avoid including <vulkan.h> so we can build without it
#if GLFW_HAS_VULKAN
#ifndef VULKAN_H_
#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;
#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;
#else
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#endif
VK_DEFINE_HANDLE(VkInstance)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSurfaceKHR)
struct VkAllocationCallbacks;
enum VkResult { VK_RESULT_MAX_ENUM = 0x7FFFFFFF };
#endif // VULKAN_H_
extern "C" { extern GLFWAPI VkResult glfwCreateWindowSurface(VkInstance instance, GLFWwindow* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface); }
static int ImGui_ImplGlfw_CreateVkSurface(ImGuiViewport* viewport, ImU64 vk_instance, const void* vk_allocator, ImU64* out_vk_surface)
{
    ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
    IM_ASSERT(g_ClientApi == GlfwClientApi_Vulkan);
    VkResult err = glfwCreateWindowSurface((VkInstance)vk_instance, data->Window, (const VkAllocationCallbacks*)vk_allocator, (VkSurfaceKHR*)out_vk_surface);
    return (int)err;
}
#endif // GLFW_HAS_VULKAN

// FIXME-PLATFORM: GLFW doesn't export monitor work area (see https://github.com/glfw/glfw/pull/989)
static void ImGui_ImplGlfw_UpdateMonitors()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    int monitors_count = 0;
    GLFWmonitor** glfw_monitors = glfwGetMonitors(&monitors_count);
    platform_io.Monitors.resize(0);
    for (int n = 0; n < monitors_count; n++)
    {
        ImGuiPlatformMonitor monitor;
        int x, y;
        glfwGetMonitorPos(glfw_monitors[n], &x, &y);
        const GLFWvidmode* vid_mode = glfwGetVideoMode(glfw_monitors[n]);
        monitor.MainPos = monitor.WorkPos = ImVec2((float)x, (float)y);
        monitor.MainSize = monitor.WorkSize = ImVec2((float)vid_mode->width, (float)vid_mode->height);
#if GLFW_HAS_PER_MONITOR_DPI
        // Warning: the validity of monitor DPI information on Windows depends on the application DPI awareness settings, which generally needs to be set in the manifest or at runtime.
        float x_scale, y_scale;
        glfwGetMonitorContentScale(glfw_monitors[n], &x_scale, &y_scale);
        monitor.DpiScale = x_scale;
#endif
        platform_io.Monitors.push_back(monitor);
    }
    g_WantUpdateMonitors = false;
}

static void ImGui_ImplGlfw_MonitorCallback(GLFWmonitor*, int)
{
    g_WantUpdateMonitors = true;
}

static void ImGui_ImplGlfw_InitPlatformInterface()
{
    // Register platform interface (will be coupled with a renderer interface)
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Platform_CreateWindow = ImGui_ImplGlfw_CreateWindow;
    platform_io.Platform_DestroyWindow = ImGui_ImplGlfw_DestroyWindow;
    platform_io.Platform_ShowWindow = ImGui_ImplGlfw_ShowWindow;
    platform_io.Platform_SetWindowPos = ImGui_ImplGlfw_SetWindowPos;
    platform_io.Platform_GetWindowPos = ImGui_ImplGlfw_GetWindowPos;
    platform_io.Platform_SetWindowSize = ImGui_ImplGlfw_SetWindowSize;
    platform_io.Platform_GetWindowSize = ImGui_ImplGlfw_GetWindowSize;
    platform_io.Platform_SetWindowFocus = ImGui_ImplGlfw_SetWindowFocus;
    platform_io.Platform_GetWindowFocus = ImGui_ImplGlfw_GetWindowFocus;
    platform_io.Platform_GetWindowMinimized = ImGui_ImplGlfw_GetWindowMinimized;
    platform_io.Platform_SetWindowTitle = ImGui_ImplGlfw_SetWindowTitle;
    platform_io.Platform_RenderWindow = ImGui_ImplGlfw_RenderWindow;
    platform_io.Platform_SwapBuffers = ImGui_ImplGlfw_SwapBuffers;
#if GLFW_HAS_WINDOW_ALPHA
    platform_io.Platform_SetWindowAlpha = ImGui_ImplGlfw_SetWindowAlpha;
#endif
#if GLFW_HAS_VULKAN
    platform_io.Platform_CreateVkSurface = ImGui_ImplGlfw_CreateVkSurface;
#endif
#if HAS_WIN32_IME
    platform_io.Platform_SetImeInputPos = ImGui_ImplWin32_SetImeInputPos;
#endif

    // Note: monitor callback are broken GLFW 3.2 and earlier (see github.com/glfw/glfw/issues/784)
    ImGui_ImplGlfw_UpdateMonitors();
    glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);

    // Register main window handle (which is owned by the main application, not by us)
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGuiViewportDataGlfw* data = IM_NEW(ImGuiViewportDataGlfw)();
    data->Window = g_Window;
    data->WindowOwned = false;
    main_viewport->PlatformUserData = data;
    main_viewport->PlatformHandle = (void*)g_Window;
}

static void ImGui_ImplGlfw_ShutdownPlatformInterface()
{
}
