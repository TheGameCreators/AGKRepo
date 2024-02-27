// dear imgui: Platform Binding for AGK.
//
// 2018 - Preben Eriksen, Version 0.5 ( BETA )
// 
// CHANGELOG
//  2018-01-11: First version.
//
#include "imgui\imgui.h"
#include "imgui\imgui_draw.cpp"
#include "imgui\imgui_widgets.cpp"
#include "imgui\imconfig.h"
#include "imgui\imgui.cpp"
#include "imgui\imgui_impl_agk.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>
#include <gl\gl.h>
#include <gl\glu.h>

// Win32 Data
bool					g_WantUpdateMonitors = true;
static INT64                g_Time = 0;
static INT64                g_TicksPerSecond = 0;
static ImGuiMouseCursor     g_LastMouseCursor = ImGuiMouseCursor_COUNT;
static HCURSOR				g_LastWinMouseCursor = NULL;

// Functions
bool    ImGui_ImplAGK_Init(void* hwnd)
{
    if (!::QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
        return false;
    if (!::QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
        return false;

    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    //io.ImeWindowHandle = hwnd; //PE: not needed anymore.
	//
//NEWIMGUI
//    io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
//    io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can set io.MouseHoveredViewport correctly (optional, not easy)
//    io.BackendPlatformName = "imgui_impl_win32";

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
//    g_hWnd = (HWND)hwnd;
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = (void*)hwnd;
//    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//        ImGui_ImplWin32_InitPlatformInterface();

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Space] = VK_SPACE;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    return true;
}

void    ImGui_ImplAGL_Shutdown()
{
	//
}

bool ImGui_ImplAGL_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) {
		return false;
	}
	//GetCursor
    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else
    {
        // Show OS mouse cursor
        LPTSTR win32_cursor = IDC_ARROW;
        switch (imgui_cursor)
        {
        case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
        case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
        case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
        case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
        case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
        case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
        case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
        case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
        }
		::SetCursor(::LoadCursor(NULL, win32_cursor));
	}
	//agk::Print("Change mouse pointer:");
    return true;
}

static void ImGui_ImplAGL_UpdateMousePos()
{
    ImGuiIO& io = ImGui::GetIO();

    // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io.WantSetMousePos)
    {
        POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
		agk::SetRawMousePosition(pos.x, pos.y);
    }

    // Set mouse position
    io.MousePos = ImVec2((float)agk::GetPointerX(), (float)agk::GetPointerX());
}

//PE: imGUI needs Unicode for input, so convert char to unicode.
int GetUnicodeFromChar(unsigned int c)
{
	char *cptr = (char*)&c;
	int result = 0xFFFD;

	if (c < 0xFF) {
		if (cptr[0] < 0) return 0xFFFD; // invalid UTF8 
		return c;
	}
	else if (c < 0xFFFF) {
		if ((cptr[1] & 0xE0) != 0xC0) return 0xFFFD; // invalid UTF8
		if ((cptr[0] & 0xC0) != 0x80) return 0xFFFD; // invalid UTF8

		result = (cptr[1] & 0x1F);
		result <<= 6;
		result |= (cptr[0] & 0x3F);
	}
	else if (c < 0xFFFFFF) {
		if ((cptr[2] & 0xF0) != 0xE0) return 0xFFFD; // invalid UTF8
		if ((cptr[1] & 0xC0) != 0x80) return 0xFFFD; // invalid UTF8
		if ((cptr[0] & 0xC0) != 0x80) return 0xFFFD; // invalid UTF8

		result = (cptr[2] & 0x0F);
		result <<= 6;
		result |= (cptr[1] & 0x3F);
		result <<= 6;
		result |= (cptr[0] & 0x3F);

		if (result < 0x800) return 0xFFFD; // overlong encoding
		if (result >= 0xD800 && result <= 0xDFFF) return 0xFFFD; // reserved for UTF16
	}
	else {
		if ((cptr[3] & 0xF8) != 0xF0) return 0xFFFD; // invalid UTF8
		if ((cptr[2] & 0xC0) != 0x80) return 0xFFFD; // invalid UTF8
		if ((cptr[1] & 0xC0) != 0x80) return 0xFFFD; // invalid UTF8
		if ((cptr[0] & 0xC0) != 0x80) return 0xFFFD; // invalid UTF8

		result = (cptr[3] & 0x07);
		result <<= 6;
		result |= (cptr[2] & 0x3F);
		result <<= 6;
		result |= (cptr[1] & 0x3F);
		result <<= 6;
		result |= (cptr[0] & 0x3F);

		if (result < 0x10000) return 0xFFFD; // overlong encoding
		if (result > 0x10FFFF) return 0xFFFD; // outside valid character range
	}

	return result;
}

//PE: Called all frames.
void    ImGui_ImplAGL_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    RECT rect;
	rect.right = agk::GetDeviceWidth();
	rect.left = 0;
	rect.bottom = agk::GetDeviceHeight();
	rect.top = 0;

    io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

//PAUL rem	io.DeltaTime = agk::GetFrameTime(); //PE: Deltatime same as AGK frame time.

	// Read keyboard modifiers inputs
	io.KeyCtrl = agk::GetRawKeyState(VK_CONTROL); //VK_CONTROL same as in AGK.
	io.KeyShift = agk::GetRawKeyState(VK_SHIFT);
	io.KeyAlt = agk::GetRawKeyState(VK_MENU);
	io.KeySuper = false;

    // Update OS mouse position
    ImGui_ImplAGL_UpdateMousePos();

    // Update OS mouse cursor with the cursor requested by imgui
    ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
    if (g_LastMouseCursor != mouse_cursor)
    {
        g_LastMouseCursor = mouse_cursor;
        ImGui_ImplAGL_UpdateMouseCursor();
	}
	//agk::Print(g_LastMouseCursor);

	//PE: Other input.
//	if (agk::GetPointerPressed()) {
	if (agk::GetPointerState()) {
		io.MouseDown[0] = true;
	}
	if (agk::GetRawMouseRightPressed()) {
		io.MouseDown[1] = true;
	}
	if (agk::GetRawMouseMiddlePressed()) {
		io.MouseDown[2] = true;
	}
	if (agk::GetPointerReleased()) {
		io.MouseDown[0] = false;
	}
	if (agk::GetRawMouseRightReleased()) {
		io.MouseDown[1] = false;
	}
	if (agk::GetRawMouseMiddleReleased()) {
		io.MouseDown[2] = false;
	}

	io.MouseWheel += agk::GetRawMouseWheelDelta() * 0.20; // GetRawMouseWheelDelta to fast so *0.2

	for (int a = 0; a < 255; a++) {
		io.KeysDown[a] = agk::GetRawKeyState(a);
		if (a == 9 && agk::GetRawKeyPressed(a) ) {
			io.AddInputCharacter((unsigned short) 9 );	//Add tab to char input.
		}
	}

	//PE: AGK GetCharBuffer same as WM_CHAR just converted to char , so convert back to unicode.
//	if (agk::GetCharBufferLength() > 0) {
//		char * mys = agk::GetCharBuffer();
//		for (int a = 0; a < strlen(mys); a++) {
//			io.AddInputCharacter((unsigned short)GetUnicodeFromChar((unsigned int)mys[a]));
//		}
//		delete mys;
//	}

	if (agk::GetCharBufferLength() > 0) {
		//PE: convert charbuffer to UTF8
		char * mys = agk::GetCharBuffer();
		int output_size = MultiByteToWideChar(CP_UTF8, 0, mys, -1, NULL, 0);
		WCHAR *wstring = new WCHAR[output_size+1];
		int size = MultiByteToWideChar(CP_UTF8, 0, mys, -1, wstring, output_size);
		for (int a = 0; a < wcslen(wstring); a++) {
			io.AddInputCharacter((unsigned short)wstring[a]);
		}
		delete mys;
	}

}


// OpenGL Data
static GLuint       g_FontTexture = 0;

// Functions
bool    ImGui_ImplOpenGL2_Init()
{
	return true;
}

void    ImGui_ImplOpenGL2_Shutdown()
{
	ImGui_ImplOpenGL2_DestroyDeviceObjects();
}

void    ImGui_ImplOpenGL2_NewFrame()
{
	if (!g_FontTexture)
		ImGui_ImplOpenGL2_CreateDeviceObjects();
}

//PE: OpenGL2 Render function. ( Done after AGK rendering , so we are on top of all ).
//Done really simple , so at some point we can convert it into AGK platform functions
//and make custom shaders so this will work on all platforms. 
void ImGui_ImplAGK_RenderDrawData(ImDrawData* draw_data)
{
	if (!draw_data)
		return;
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;



	//draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	//PE: OpenGL fixed pipeline.
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_SCISSOR_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	//agk::PlatformSetViewport(0, 0, fb_width, fb_height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(draw_data->DisplayPos.x, draw_data->DisplayPos.x + draw_data->DisplaySize.x, draw_data->DisplayPos.y + draw_data->DisplaySize.y, draw_data->DisplayPos.y, -1.0f, +1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//if (glGetError() != GL_NO_ERROR)
	//{
	//	agk::Print("glError:");
	//}

	//PE: We already do this in the main loop. and are needed.
	//AGKShader *pShader = AGKShader::g_pShaderColor;
	//if (pShader)
	//	pShader->NoShader();

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)


	//PE: Get imGUI command lists
	//ImVec2 pos = draw_data->DisplayPos;

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
		glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, pos)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, uv)));
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, col)));

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				// User callback (registered via ImDrawList::AddCallback)
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
				{
					// Apply scissor/clipping rectangle
					glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

					//PE: At some point.
					//pShader->DrawIndices( (GLsizei)pcmd->ElemCount , (unsigned short *) idx_buffer , GL_TRIANGLES );

					// Bind textures
					GLuint useTexture = (GLuint)(intptr_t)pcmd->TextureId;



					//Not used yet.
					if (useTexture >= 300000) {
						//PE: Render using no alpha and without color array.
						useTexture -= 300000;
						glDisableClientState(GL_COLOR_ARRAY); //GL_COLOR_ARRAY dont mix with RGBA textue2D.
						glBlendFunc(GL_ONE, GL_NONE);
						glBindTexture(GL_TEXTURE_2D, useTexture);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

						glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
						glEnableClientState(GL_COLOR_ARRAY);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					}
					else if (useTexture >= 200000) {
						//PE: Render using no alpha and without color array.
						useTexture -= 200000;
						glDisableClientState(GL_COLOR_ARRAY); //GL_COLOR_ARRAY dont mix with RGBA textue2D.
						glBlendFunc(GL_ONE, GL_NONE);
						glBindTexture(GL_TEXTURE_2D, useTexture);
						//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

						glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
						glEnableClientState(GL_COLOR_ARRAY);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					}
					else if (useTexture >= 100000) {
						//PE: Render using alpha, without color array.
						useTexture -= 100000;
						//glDisableClientState(GL_COLOR_ARRAY);
						//GL_COLOR_ARRAY dont mix with RGBA textue2D.
						glBindTexture(GL_TEXTURE_2D, useTexture);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

						glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
						//glEnableClientState(GL_COLOR_ARRAY);
					}
					else {
						//PE: Render using color array.
						glBindTexture(GL_TEXTURE_2D, useTexture);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST
						//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

						glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
					}

				}
			}
			idx_buffer += pcmd->ElemCount;
		}
	}
	// Restore modified state
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
    glPolygonMode(GL_FRONT, (GLenum)last_polygon_mode[0]); glPolygonMode(GL_BACK, (GLenum)last_polygon_mode[1]);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

bool ImGui_ImplOpenGL2_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (ImTextureID)(intptr_t)g_FontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);

	return true;
}

void ImGui_ImplOpenGL2_DestroyFontsTexture()
{
	if (g_FontTexture)
	{
		ImGuiIO& io = ImGui::GetIO();
		glDeleteTextures(1, &g_FontTexture);
		io.Fonts->TexID = 0;
		g_FontTexture = 0;
	}
}

bool    ImGui_ImplOpenGL2_CreateDeviceObjects()
{
	return ImGui_ImplOpenGL2_CreateFontsTexture();
}

void    ImGui_ImplOpenGL2_DestroyDeviceObjects()
{
	ImGui_ImplOpenGL2_DestroyFontsTexture();
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

static void ImGui_ImplOpenGL2_RenderWindow(ImGuiViewport* viewport, void*)
{
    if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
    {
        ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
    }
	ImGui_ImplAGK_RenderDrawData(viewport->DrawData);
}

static void ImGui_ImplOpenGL2_InitPlatformInterface()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_RenderWindow = ImGui_ImplOpenGL2_RenderWindow;
}

static void ImGui_ImplOpenGL2_ShutdownPlatformInterface()
{
    ImGui::DestroyPlatformWindows();
}
