// imgui: Platform Binding for Windows

IMGUI_IMPL_API bool     ImGui_ImplAGL_Init(void* hwnd);
IMGUI_IMPL_API void     ImGui_ImplAGL_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplAGL_NewFrame();


IMGUI_IMPL_API bool     ImGui_ImplOpenGL2_Init();
IMGUI_IMPL_API void     ImGui_ImplOpenGL2_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplOpenGL2_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplAGK_RenderDrawData(ImDrawData* draw_data);

// Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool     ImGui_ImplOpenGL2_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplOpenGL2_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplOpenGL2_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplOpenGL2_DestroyDeviceObjects();


