//GUI

#define NOMINMAX // disable min max from include files, we have our own.

#include "stdio.h"
#include "agk.h"
#include "Ide.h"
#include "gui.h"
#include "project.h"

#ifdef AGK_WINDOWS
extern HWND g_agkhWnd;
#endif

#include "boxer.h"


#ifdef AGK_WINDOWS
#include <direct.h>
#include "shlobj.h"
#endif
#include "imgui/imguial_fonts.cpp"
#ifdef ADDIMGUIDEMO
//#include  "imgui/imgui_demo.cpp"
#ifndef DEMOLOAD
#define DEMOLOAD
#include "imgui/imgui_demo.cpp"
#endif
#endif

#ifndef AGK_WINDOWS
    #include <dirent.h>
#endif

Preferences pref;

//Key remapping list.
const char* key_values[] = {
	"##0","##1","##2","##3","##4","##5","##6","##7",
	"KEY_BACK", //8
	"KEY_TAB", //9
	"##10","##11","##12",
	"KEY_ENTER", //13
	"##14","##15",
	"KEY_SHIFT", //16
	"KEY_CONTROL", //17
	"KEY_ALT", //18
	"KEY_PAUSE", //19
	"KEY_CAPSLOCK", //20
	"##21","##22","##23","##24","##25","##26",
	"KEY_ESCAPE", //27
	"##28","##29","##30","##31",
	"KEY_SPACE", //32
	"KEY_PAGEUP", //33
	"KEY_PAGEDOWN", //34
	"KEY_END", //35
	"KEY_HOME", //36
	"KEY_LEFT", //37
	"KEY_UP", //38
	"KEY_RIGHT", //39
	"KEY_DOWN", //40
	"##41","##42","##43","##44",
	"KEY_INSERT", //45
	"KEY_DELETE", //46
	"##47",
	"KEY_0", //48 (triggered by both top row number keys and numpad keys)
	"KEY_1", //49
	"KEY_2", //50
	"KEY_3", //51
	"KEY_4", //52
	"KEY_5", //53
	"KEY_6", //54
	"KEY_7", //55
	"KEY_8", //56
	"KEY_9", //57
	"##58","##59","##60","##61","##62","##63","##64",
	"KEY_A", //65
	"KEY_B", //66
	"KEY_C", //67
	"KEY_D", //68
	"KEY_E", //69
	"KEY_F", //70
	"KEY_G", //71
	"KEY_H", //72
	"KEY_I", //73
	"KEY_J", //74
	"KEY_K", //75
	"KEY_L", //76
	"KEY_M", //77
	"KEY_N", //78
	"KEY_O", //79
	"KEY_P", //80
	"KEY_Q", //81
	"KEY_R", //82
	"KEY_S", //83
	"KEY_T", //84
	"KEY_U", //85
	"KEY_V", //86
	"KEY_W", //87
	"KEY_X", //88
	"KEY_Y", //89
	"KEY_Z", //90
	"KEY_WIN_LEFT", //91
	"KEY_WIN_RIGHT", //92
	"##93","##94","##95",
	"KEY_NUMPAD_0", //96
	"KEY_NUMPAD_1", //97
	"KEY_NUMPAD_2", //98
	"KEY_NUMPAD_3", //99
	"KEY_NUMPAD_4", //100
	"KEY_NUMPAD_5", //101
	"KEY_NUMPAD_6", //102
	"KEY_NUMPAD_7", //103
	"KEY_NUMPAD_8", //104
	"KEY_NUMPAD_9", //105
	"KEY_ASTERISK", //106
	"KEY_PLUS", //107
	"##108",
	"KEY_SUBTRACT", //109
	"KEY_DECIMAL", //110
	"KEY_DIVIDE", //111
	"KEY_F1", //112
	"KEY_F2", //113
	"KEY_F3", //114
	"KEY_F4", //115
	"KEY_F5", //116
	"KEY_F6", //117
	"KEY_F7", //118
	"KEY_F8", //119
	"KEY_F9", //120
	"KEY_F10", //121
	"KEY_F11", //122
	"KEY_F12", //123
	"##124","##125","##126","##127","##128","##129",
	"##130","##131","##132","##133","##134","##135","##136","##137","##138","##139",
	"##140","##141","##142","##143",
	"KEY_NUMLOCK", //144
	"KEY_SCROLLLOCK", //145
	"##146","##147","##148","##149",
	"##150","##151","##152","##153","##154","##155","##156","##157","##158","##159",
	"##160","##161","##162","##163","##164","##165","##166","##167","##168","##169",
	"##170","##171","##172",
	"KEY_VOLUME_MUTE", //173
	"KEY_VOLUME_DOWN", //174
	"KEY_VOLUME_UP", //175
	"KEY_MEDIA_NEXT", //176
	"KEY_MEDIA_PREV", //177
	"KEY_MEDIA_STOP", //178
	"KEY_MEDIA_PLAY", //179
	"##180","##181","##182","##183","##184","##185",
	";", //186
	"=", //187
	",", //188
	"-", //189
	".", //190
	"/", //191
	"'", //192
	"##193","##194","##195","##196","##197","##198","##199",
	"##200","##201","##202","##203","##204","##205","##206","##207","##208","##209",
	"##210","##211","##212","##213","##214","##215","##216","##217","##218",
	"[", //219
	"\\", //220
	"]", //221
	"#", //222
	"`", //223
	"##224 Dont need below",
	"##KEY_LEFT_SHIFT", //257
	"##KEY_RIGHT_SHIFT", //258
	"##KEY_LEFT_CTRL", //259
	"##KEY_RIGHT_CTRL", //260
	"##KEY_LEFT_ALT", //261
	"##KEY_RIGHT_ALT", //262
	"##KEY_TOP_0", //263
	"##KEY_TOP_1", //264
	"##KEY_TOP_2", //265
	"##KEY_TOP_3", //266
	"##KEY_TOP_4", //267
	"##KEY_TOP_5", //268
	"##KEY_TOP_6", //269
	"##KEY_TOP_7", //270
	"##KEY_TOP_8", //271
	"##KEY_TOP_9", //272
};


cProjectItem::sProjectFiles * FindFileFromEditor(TextEditor * m_ActiveEditor);
cProjectItem::sProjectFiles * FindFileFromEditorMatchSource(TextEditor * m_FindActiveEditor);
extern cProjectItem * pCurrentSelectedProject;
extern char * cNextWindowFocus;
extern bool bFocusActiveEditor;
extern uString szNextWindowFocus;
extern cProjectItem * allProjects;
extern cProjectItem * mCompiledProject;
extern cFolderItem *m_pClassicDLCFolder;
extern TextEditor * m_ActiveEditor;
extern TextEditor * m_CompiledEditor;
extern int debug_is_running;
extern int remote_debug_is_running;
extern bool bBreakJustStarted;
extern char customFolders[10][MAX_PATH];
extern int iCustomFolders;

ImFont* defaultfont;
ImFont* customfont;
ImFont* editorfont;
ImFont* agkfont;
extern ImFont* previewFont;
extern char cPreviewFont[MAX_PATH];
extern bool bPreviewFontLoaded;
extern bool bDTSPossible;
int refresh_gui_docking = 0;
cFolderItem *m_pSelectedFolder = NULL;
char JDKError[1024];
void GetJDKPath(void);

int iCollectListActive = 1, iCollectListUpdate = 0;
int iUpdateIndex = 0, iDelayedUpdateStart = 0;
bool bCollectSuggestKeywords = true; //Make this delayed
bool show_gotoline_window = false;
bool show_gotoline_window_state = false;
bool show_keystore_window = false;
bool show_additionalfiles_window = false;
bool show_keystore_window_state = false;
bool show_androidexport_window = false;
bool show_html5export_window = false;
bool show_iosexport_window = false;
bool show_iosexport_window_state = false;
bool show_androidexport_window_state = false;
int iGotoLine = 0;
bool bSetVariableActive = false;
bool updateWatchCalled = false;
bool bEnableSeedStyleChanged = true;
uString szRecentProjects[MAXRECENTLIST];
uString szRecentFiles[MAXRECENTLIST];
int iResentProjects = 0, iResentFiles = 0;
char import_error_msg[MAX_PATH];

bool enable_debug_gadgets = false;
char output_logfile[MAX_PATH];
char output_debug[2048];
bool ios_generate_log = false;
bool utils_add_folder_to_html5_data_file(FILE *pHTML5Data, const char* srcfull, const char* src, char* load_package_string, char* additional_folders_string, int* currpos);

bool dis_all_breakpoints = false;

#include "TextEditor.cpp"

//#define AGKFONTSIZE 64
#define AGKFONTSIZE 32


//#define FONTUPSCALE 1.0 // 1.40 - for better looking font icons. needed if we use larger font icons.

//#ifdef AGK_MACOS
//#endif

//#####################################
//#### Process Preferences window. ####
//#####################################

void ProcessPreferences(void) {
	
	if (!pref.show_preferences_window)
		return;

	float fs = ImGui::CalcTextSize("#").x;

	ImGui::SetNextWindowSize(ImVec2(36*ImGui::GetFontSize(), 32* ImGui::GetFontSize()), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);

	ImGuiWindowFlags window_flags = 0;
	ImGui::Begin("Preferences", &pref.show_preferences_window, window_flags);

	int inputcolx = fs * 18;
	//Tabs
	if (ImGui::BeginTabBar("preferencestabbar"))
	{
		if (ImGui::BeginTabItem(" Editor "))
		{
			ImGui::Checkbox("Enable Symbols Lists", &pref.bEnableSymboldList);
			ImGui::Checkbox("Display Line Numbers", &pref.editor_display_linenumbers);
			ImGui::Checkbox("Enable Auto Completion", &pref.bEnableAutoCompletion);
			ImGui::SliderInt("Tab Size", &pref.mTabSize, 1, 48);
			if (ImGui::SliderInt("Font Size", &pref.iEditorFontSize, 8, 48)) {
				extern int changefonttype;
				changefonttype = 999;
			}
			
			//Add select custom Editor Font.
			//char cCustomEditorFont[MAX_PATH];
			if (ImGui::InputText("Custom Editor Font", &pref.cCustomEditorFont[0], MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue)) {
				uString tmp = pref.cCustomEditorFont;
				tmp.ReplaceStr("\\", "/");
				strcpy(pref.cCustomEditorFont, tmp.GetStr());
				extern int changefonttype;
				changefonttype = 999;
			}

			ImGui::SameLine();
			if (ImGui::Button("...##customeditorfont")) {

				char * cFileSelected;
				cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "ttf\0*.ttf\0", NULL, NULL);
				if (cFileSelected && strlen(cFileSelected) > 0) {
					strcpy(pref.cCustomEditorFont, cFileSelected);
					uString tmp = pref.cCustomEditorFont;
					tmp.ReplaceStr("\\", "/");
					strcpy(pref.cCustomEditorFont, tmp.GetStr());
					//Enable new font.
					extern int changefonttype;
					changefonttype = 999;
				}

			}

			//When enter is pressed place cursor at same [tab][space] indentation as previous line.
			ImGui::Checkbox("Enable Auto Indentation", &pref.bAutoIndentation);
			ImGui::Checkbox("Enable Smart Indentation", &pref.bSmartAutoIndentation);
			ImGui::Checkbox("Remove Path From Tab Names", &pref.bRemovePathFromTabName);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("You must restart AppGameKit-Studio for this to take effect.");

			ImGui::Checkbox("Enable Code Properties", &pref.bUseCodeProperties);

			bool bTmp = pref.iHoverSyntaxHelp;
			ImGui::Checkbox("Mouse Hover Over Command, Display Help Syntax", &bTmp);
			pref.iHoverSyntaxHelp = bTmp;

			bTmp = pref.iTabHideDropdown;
			if (ImGui::Checkbox("Hide Tab Dropdown Button", &bTmp)) {
				pref.iTabHideDropdown = bTmp;
				ImGuiIO& io = ImGui::GetIO(); // (void)io;
				if(pref.iTabHideDropdown)
					io.ConfigFlags |= ImGuiConfigFlags_DockNoCollapsButton;
				else
					io.ConfigFlags &= ~ImGuiConfigFlags_DockNoCollapsButton;

			}
			pref.iTabHideDropdown = bTmp;
			
			bTmp = pref.iEnableCodeFolding;
			ImGui::Checkbox("Enable Code Folding", &bTmp);
			pref.iEnableCodeFolding = bTmp;

			bTmp = pref.iDisableMouseWheelZoom;
			ImGui::Checkbox("Disable Mouse Wheel Zoom", &bTmp);
			pref.iDisableMouseWheelZoom = bTmp;

			bTmp = pref.iRememberTabOrder;
			ImGui::Checkbox("Remember Editor Tab Order", &bTmp);
			pref.iRememberTabOrder = bTmp;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Will remember the order of Visible tabs.\nNon-visible tabs will be in a-z order.");

#ifdef AGK_WINDOWS
			bTmp = pref.bBrowserHelp;
			ImGui::Checkbox("Enable F1 Browser Help", &bTmp);
			pref.bBrowserHelp = bTmp;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Use browser help instead of the built in help");
#endif

			ImGui::Separator();

			ImGui::SliderInt("Scene: Float Decimal Precision", &pref.iSceneLowFloatPrecision, 0, 8);
			//ImGui::Checkbox("Scene Editor: Use 2 Decimal Precision For Float", &pref.bSceneLowFloatPrecision);

			ImGui::Separator();
			if (ImGui::InputText("Default Project Folder", &pref.cDefaultProjectFolder[0], MAX_PATH)) {
				uString nfolder = pref.cDefaultProjectFolder;
				nfolder.ReplaceStr("\\", "/");
				strcpy(pref.cDefaultProjectFolder, nfolder.GetStr());
			}

			ImGui::SameLine();
			if (ImGui::Button("...")) {
				char *selectedfolder;
				//				selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_DIR, "agk\0*.agk\0", NULL, NULL);
				//Mac test. or will always include .agk
				selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_DIR, NULL, NULL, NULL);
				if (selectedfolder) {
					if (strlen(selectedfolder) > 0) {
						uString nfolder = selectedfolder;
						nfolder.ReplaceStr("\\", "/");
						strcpy(pref.cDefaultProjectFolder, nfolder.GetStr());
					}
				}
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(" Ide "))
		{
			if (ImGui::SliderInt("Ide Font Size", &pref.iIDEFontSize, 8, 64)) {
				extern int changefonttype;
				changefonttype = 999;
			}

			if (ImGui::Checkbox("Enable Toolbar Large Icons", &pref.bEnableToolbarLargeIcons)) {
				pref.bEnableToolbarExtraLargeIcons = false;
				ChangeIconSet();
				ResetLayout();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Note: This will reset your current layout.");

			ImGui::SameLine();
			if (ImGui::Checkbox("Extra Large Icons", &pref.bEnableToolbarExtraLargeIcons)) {
				pref.bEnableToolbarLargeIcons = false;
				ChangeIconSet();
				ResetLayout();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Note: This will reset your current layout.");

			ImGui::Checkbox("Only Display Active Project Files", &pref.bDisplayActiveProjectFilesOnly);

			ImGui::Checkbox("Display Projects Media Folder in Assets Browser ", &pref.bDisplayProjectinAssetsBrowser);

			ImGui::Checkbox("Load Classic DLC on Startup", &pref.bLoadClassicDLConStartup);

			if (ImGui::Checkbox("Upscaling Remove Blurred Look", &pref.bUseUpscaling)) {
				//Change font.
				extern int changefonttype;
				changefonttype = 999;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Remove blurred look on systems using scaled resolutions (Retina).");


			ImGui::Checkbox("Use Internal Mouse Pointer", &pref.bUseInternalMousePointer);
			if (ImGui::IsItemHovered()) {
				ImGui::SetNextWindowContentSize(ImVec2(350, 0));
				ImGui::BeginTooltip();
				ImGui::TextWrapped("Only use this option if your OS doesn't have a mouse pointer. You must restart AppGameKit Studio for this to take effect.");
				ImGui::EndTooltip();
			}

			ImGui::Checkbox("Restore Layout on Startup.", &pref.save_layout);

			bool bTmp;
			bTmp = pref.bAppGameKitNews;
			ImGui::Checkbox("Show AppGameKit News", &bTmp);
			pref.bAppGameKitNews = bTmp;

			bTmp = pref.iCancelQuitDialog;
			ImGui::Checkbox("Ask Before Quitting AppGameKit Studio.", &bTmp);
			pref.iCancelQuitDialog = bTmp;

			ImGui::Text("Toolbar Icon Set:");
			ImGui::SameLine();
			bool rb_change = false;
			if (ImGui::RadioButton("Set 1 ", &pref.iCurrentIconSet, 0))
				rb_change = true;
			ImGui::SameLine();
			if (ImGui::RadioButton("Set 2 ", &pref.iCurrentIconSet, 1))
				rb_change = true;
			ImGui::SameLine();
			if (ImGui::RadioButton("Set 3", &pref.iCurrentIconSet, 2))
				rb_change = true;

			if (rb_change)
				ChangeIconSet();


			ImGui::Spacing();
			ImGui::Text("Media/Preview Icon Background Color:");
			//ImGui::SameLine();
			//ImGui::SetCursorPos(ImVec2(150, ImGui::GetCursorPos().y));
			//ImGui::ColorEdit4("##mediaiconspreviewback", (float*)&pref.icon_background_color, ImGuiColorEditFlags_NoInputs);
			ImGui::ColorEdit4("Color##mediaiconspreviewback", (float*)&pref.icon_background_color, ImGuiColorEditFlags_AlphaPreview);


			ImGui::Separator();

			ImGui::TextWrapped("Current IDE FPS: %.1f , (ms/f %.3f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
			ImGui::Text("IDE Update Interval:");
			ImGui::SameLine();
			rb_change = false;
			if (ImGui::RadioButton("30 FPS", &pref.iIDEUpdateFPS, 0))
				rb_change = true;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("You might have to restart AppGameKit-Studio for this to take effect.");
			ImGui::SameLine();
			if (ImGui::RadioButton("60 FPS", &pref.iIDEUpdateFPS, 1))
				rb_change = true;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("You might have to restart AppGameKit-Studio for this to take effect.");
			ImGui::SameLine();
			if (ImGui::RadioButton("Vertical Sync", &pref.iIDEUpdateFPS, 2))
				rb_change = true;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("You might have to restart AppGameKit-Studio for this to take effect.");
			ImGui::SameLine();
			if (ImGui::RadioButton("Full Speed", &pref.iIDEUpdateFPS, 3))
				rb_change = true;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("You might have to restart AppGameKit-Studio for this to take effect.");
	
			extern unsigned int os_messages_sleeptime;
			bool bState = pref.iIDEUpdateEventSleep;
			ImGui::Checkbox("Event based rendering.", &pref.iIDEUpdateEventSleep);

			if (bState != pref.iIDEUpdateEventSleep) {
				rb_change = true;
				if(pref.iIDEUpdateEventSleep)
					agk::SetSyncRate(0, 0); // PAUL: drawing on demand instead of at a regular sync rate
			}


//			if (pref.iIDEUpdateEventSleep) {
//				ImGui::SameLine();
//				ImGui::Text(" Current Wake Timer: %ld", os_messages_sleeptime);
//			}
			if (rb_change) {
				if (pref.iIDEUpdateFPS == 0) {
					agk::SetSyncRate(30, 0);
					agk::SetVSync(1);
					agk::Sync();
				}
				if (pref.iIDEUpdateFPS == 1) {
					agk::SetSyncRate(60, 0);
					agk::SetVSync(1);
					agk::Sync();
				}
				if (pref.iIDEUpdateFPS == 2) {
					agk::SetVSync(1);
					agk::Sync();
				}
				if (pref.iIDEUpdateFPS == 3) {
					agk::SetSyncRate(60 * 20, 0);
					agk::Sync();
				}
			}

			//pref.iCurrentIconSet

//			ImGui::Checkbox("Load Studio DLC on Startup", &pref.bLoadClassicDLConStartup);


			ImGui::Spacing();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(" Build Options "))
		{
			ImGui::Text("Build Options:");

			//ImGui::Checkbox("Windows 64-bit", &pref.bWindows64Bit); // now always 64-bit
			ImGui::Checkbox("Windows Timestamp exe for Faster 'Run'.", &pref.bTimestampExe);
			
			ImGui::Text("");
			ImGui::Text("Broadcast:");

			//broadcast port ?
			ImGui::Text("  Device IP Address:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(inputcolx, ImGui::GetCursorPos().y));
			ImGui::InputText("      ##pref.bip", &pref.cBroadcastIp[0], 30, 0);
			//use steam integration ?


			ImGui::Text("");
			ImGui::Text("Debug:");
			ImGui::Text("  Device IP Address:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(inputcolx, ImGui::GetCursorPos().y));
			ImGui::InputText("      ##pref.dip", &pref.cDebugIp[0], 30, 0);
			ImGui::Checkbox("Auto Hide Debug Window", &pref.bAutoHideDebugWindows);

//			ImGui::Checkbox("On Debug Start, Bring Debugger to Front", &pref.bDebugBringToFront);
			ImGui::Checkbox("On Debug Start, Update Watches and Status.", &pref.bDebugBringToFront);
#ifdef AGK_WINDOWS
			ImGui::Checkbox("On Debug Try to Bring App to Front.", &pref.bDebugBringAppToFront);
#endif

			//debug_hwnd
			//bDebugBringToFront

			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(" Style Generator "))
		{

			ImGui::Checkbox("Enable Custom GUI Colors", &pref.bEnableCustomStyle);

			if (pref.bEnableCustomStyle) {
				pref.bEnableSeedStyle = false;
				bEnableSeedStyleChanged = false;

				ImGuiStyle& style = ImGui::GetStyle();

				//recall preferences
				static float col_main_hue = pref.gui_col_main_hue;
				static float col_main_sat = pref.gui_col_main_sat;
				static float col_main_val = pref.gui_col_main_val;
				static float col_area_hue = pref.gui_col_area_hue;
				static float col_area_sat = pref.gui_col_area_sat;
				static float col_area_val = pref.gui_col_area_val;
				static float col_back_hue = pref.gui_col_back_hue;
				static float col_back_sat = pref.gui_col_back_sat;
				static float col_back_val = pref.gui_col_back_val;
				static float col_text_hue = pref.gui_col_text_hue;
				static float col_text_sat = pref.gui_col_text_sat;
				static float col_text_val = pref.gui_col_text_val;
				static float col_scroll_hue = pref.gui_col_scroll_hue;
				static float col_scroll_sat = pref.gui_col_scroll_sat;
				static float col_scroll_val = pref.gui_col_scroll_val;

				//set hues
				ImVec4 col_main = ImColor::HSV(col_main_hue, col_main_sat, col_main_val);
				ImVec4 col_area = ImColor::HSV(col_area_hue, col_area_sat, col_area_val);
				ImVec4 col_back = ImColor::HSV(col_back_hue, col_back_sat, col_back_val);
				ImVec4 col_text = ImColor::HSV(col_text_hue, col_text_sat, col_text_val);
				ImVec4 col_scroll = ImColor::HSV(col_scroll_hue, col_scroll_sat, col_scroll_val);

				ImVec4 rgb;
				//ImGui::ColorEditMode(ImGuiColorEditMode_HSV);
			
				//main 
				ImGui::ColorConvertHSVtoRGB(col_main_hue, col_main_sat, col_main_val, rgb.x, rgb.y, rgb.z);
				ImGui::ColorEdit3("Main RGB", &rgb.x);
				ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_main_hue, col_main_sat, col_main_val);
				style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
				style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
				style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.65f);
				style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
				style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
				style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
				style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
				style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
				style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
				style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
				style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
				style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
				style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
				style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
				style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
				style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
				style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
				style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
				style.Colors[ImGuiCol_Tab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
				style.Colors[ImGuiCol_TabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
				style.Colors[ImGuiCol_TabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
				style.Colors[ImGuiCol_TabUnfocused] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
				style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);

				//area
				ImGui::ColorConvertHSVtoRGB(col_area_hue, col_area_sat, col_area_val, rgb.x, rgb.y, rgb.z);
				ImGui::ColorEdit3("Area RGB", &rgb.x);
				ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_area_hue, col_area_sat, col_area_val);
				style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
				style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
				style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
				style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
				//style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	
				//background
				ImGui::ColorConvertHSVtoRGB(col_back_hue, col_back_sat, col_back_val, rgb.x, rgb.y, rgb.z);
				ImGui::ColorEdit3("Background RGB", &rgb.x);
				ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_back_hue, col_back_sat, col_back_val);
				style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);

				//text
				ImGui::ColorConvertHSVtoRGB(col_text_hue, col_text_sat, col_text_val, rgb.x, rgb.y, rgb.z);
				ImGui::ColorEdit3("Text RGB", &rgb.x);
				ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_text_hue, col_text_sat, col_text_val);
				style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
				style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
				style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
				style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
				style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
				style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
				style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
				style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
				style.Colors[ImGuiCol_CheckMark] = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
				//style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
				//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
				//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);

				//scroll & slider
				ImGui::ColorConvertHSVtoRGB(col_scroll_hue, col_scroll_sat, col_scroll_val, rgb.x, rgb.y, rgb.z);
				ImGui::ColorEdit3("Scroll & Slider RGB", &rgb.x);
				ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_scroll_hue, col_scroll_sat, col_scroll_val);
				style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 0.50f); //0.31
				style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 0.78f);
				style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 1.00f);
				style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 0.50f); //0.24
				style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 1.00f);

				//fixed colours
				style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
				style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
				style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
				style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
				style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
				style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.6f);

				//save changes to prefs
				pref.gui_col_main_hue = col_main_hue;
				pref.gui_col_main_sat = col_main_sat;
				pref.gui_col_main_val = col_main_val;
				pref.gui_col_area_hue = col_area_hue;
				pref.gui_col_area_sat = col_area_sat;
				pref.gui_col_area_val = col_area_val;
				pref.gui_col_back_hue = col_back_hue;
				pref.gui_col_back_sat = col_back_sat;
				pref.gui_col_back_val = col_back_val;
				pref.gui_col_text_hue = col_text_hue;
				pref.gui_col_text_sat = col_text_sat;
				pref.gui_col_text_val = col_text_val;
				pref.gui_col_scroll_hue = col_scroll_hue;
				pref.gui_col_scroll_sat = col_scroll_sat;
				pref.gui_col_scroll_val = col_scroll_val;

				//iIDESelectedStyle
			}

			bool bOldState = pref.bEnableSeedStyle;
			ImGui::Checkbox("Enable Seed Generated Style", &pref.bEnableSeedStyle);

			if (pref.bEnableSeedStyle) {
				pref.bEnableCustomStyle = false;
				bEnableSeedStyleChanged = false;

				if (ImGui::InputInt("Seed Value", &pref.iSeedStyle, 1, 10)) {
					SetSeedStyleColors();
				}
				ImGui::SameLine();
				if (ImGui::Button("Random Seed")) {
					agk::SetRandomSeed( (int) (agk::Timer()*1234.0) );
					pref.iSeedStyle = agk::Random(-999999, 999999);
					SetSeedStyleColors();
				}
				if(bOldState != pref.bEnableSeedStyle)
					SetSeedStyleColors();
			
				ImGui::Spacing();
				ImGui::TextWrapped("A seed generated style will randomly select colors based on a seed value, this means that the number and style will always be the same. Any value you enter will be a 100 percent unique style. So if you find a great looking SEED value please share your seed and post a screenshot of your style on the forum.");
			
			}


			if(!pref.bEnableSeedStyle && !pref.bEnableCustomStyle){
				if (!bEnableSeedStyleChanged ) {
					myDefaultStyle(NULL);
					pref.idePalette = 0;
					bEnableSeedStyleChanged = true;
				}
			}

			enum class PaletteIndex
			{
				Default,
				Keyword,
				Number,
				String,
				CharLiteral,
				Punctuation,
				Preprocessor,
				Identifier,
				KnownIdentifier,
				PreprocIdentifier,
				Comment,
				MultiLineComment,
				Background,
				Cursor,
				Selection,
				ErrorMarker,
				Breakpoint,
				LineNumber,
				CurrentLineFill,
				CurrentLineFillInactive,
				CurrentLineEdge,
				BackgroundLine,
				Max
			};
			ImGui::Separator();
			bool bCustomEditorColor;
			if (pref.editorPalette == 4)
				bCustomEditorColor = true;
			else bCustomEditorColor = false;

			if (ImGui::Checkbox("Enable Custom Editor Colors", &bCustomEditorColor)) {
				if (!bCustomEditorColor) {
					if (m_ActiveEditor) m_ActiveEditor->SetPalette(TextEditor::GetAGKPalette());
					pref.editorPalette = 0;
				}
			}

			if (bCustomEditorColor) {
				if (m_ActiveEditor) m_ActiveEditor->SetPalette(TextEditor::GetCustomPalette());
				pref.editorPalette = 4;
			}

			if (bCustomEditorColor) {
				ImVec4 tmpvec4;

				if (strlen(import_error_msg) > 0) {
					ImGui::Separator();
					ImGui::TextWrapped("%s", import_error_msg);
					ImGui::Separator();
				}
				if (ImGui::Button("Export As Text To Clipboard")) {
					strcpy(import_error_msg, "");
					uString toClipboard = "AppGameKit Studio Editor Style:\r\n";
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Identifier]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Identifier]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Keyword]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Number]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::String]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::CharLiteral]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Punctuation]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Preprocessor]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::KnownIdentifier]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::PreprocIdentifier]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Comment]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Background]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Cursor]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Selection]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::ErrorMarker]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::Breakpoint]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::LineNumber]); toClipboard.Append("\r\n");
					toClipboard.AppendUInt(pref.EditorColors[(int)PaletteIndex::CurrentLineFill]); toClipboard.Append("\r\n");
					toClipboard.Append("Copy this text to your clipboard and in preferences use 'Import From Clipboard'.\r\n");
					toClipboard.Append("End Editor Style:\r\n");

					ImGui::SetClipboardText(toClipboard.GetStr());
					strcpy(import_error_msg, "Style was exported to clipboard.");
				}
				ImGui::SameLine();
				if (ImGui::Button("Import From Clipboard")) {
					const char * clipText = ImGui::GetClipboardText();
					uString fromClipboard,tmp,tmp2;
					fromClipboard.SetStr(clipText);
					strcpy(import_error_msg, "");
					if (fromClipboard.GetLength() > 0) {
						int pos = fromClipboard.FindStr("AppGameKit Studio Editor Style:", 1);
						if (pos >= 0) {
							//"AppGameKit Studio Editor Style:"
							fromClipboard.SubString(tmp, pos+31);
							//Clean.
							tmp.ReplaceStr("\r", "");
							tmp.ReplaceStr("\n\n", "\n");
							tmp.ReplaceStr(":", "");
							tmp.ReplaceStr(";", "");
							tmp.ReplaceStr("&", "");
							tmp.ReplaceStr("\n", ";");
							unsigned int colval[22];
							int foundvals = 0;
							pos = 0;
							for (int i = 0; i < 19; i++) {
								pos = tmp.FindStr(";");
								if (pos >= 0) {
									uString myval;
									tmp.SubString(myval, pos+1, 10);
									
									int pos2 = myval.FindStr(";");
									if (pos2 > 0) {
										myval.SubString(tmp2, 0, pos2 );
										myval = tmp2;
									}
									myval.ReplaceStr(";", "");

									tmp.SubString(fromClipboard, pos + 1);
									if (atoi(myval) != 0) {
										try
										{
											unsigned long toul = std::stoul(myval.GetStr(), nullptr, 0);
											colval[foundvals++] = (unsigned int)toul;
										}
										catch (std::exception & e)
										{
											//Ignore exceptions.
										}
									}
									else {
										colval[foundvals++] = 0;
									}
									tmp = fromClipboard;
								}
							}

							if (foundvals >= 18) {
								//All should have a hit.

								pref.EditorColors[(int)PaletteIndex::Identifier] = colval[0];
								pref.EditorColors[(int)PaletteIndex::Identifier] = colval[1];
								pref.EditorColors[(int)PaletteIndex::Keyword] = colval[2];
								pref.EditorColors[(int)PaletteIndex::Number] = colval[3];
								pref.EditorColors[(int)PaletteIndex::String] = colval[4];
								pref.EditorColors[(int)PaletteIndex::CharLiteral] = colval[5];
								pref.EditorColors[(int)PaletteIndex::Punctuation] = colval[6];
								pref.EditorColors[(int)PaletteIndex::Preprocessor] = colval[7];
								pref.EditorColors[(int)PaletteIndex::KnownIdentifier] = colval[8];
								pref.EditorColors[(int)PaletteIndex::PreprocIdentifier] = colval[9];
								pref.EditorColors[(int)PaletteIndex::Comment] = colval[10];
								pref.EditorColors[(int)PaletteIndex::Background] = colval[11];
								pref.EditorColors[(int)PaletteIndex::Cursor] = colval[12];
								pref.EditorColors[(int)PaletteIndex::Selection] = colval[13];
								pref.EditorColors[(int)PaletteIndex::ErrorMarker] = colval[14];
								pref.EditorColors[(int)PaletteIndex::Breakpoint] = colval[15];
								pref.EditorColors[(int)PaletteIndex::LineNumber] = colval[16];
								pref.EditorColors[(int)PaletteIndex::CurrentLineFill] = colval[17];

								strcpy(import_error_msg, "Style Imported!");
							}

						}
						else {
							strcpy(import_error_msg, "Error: Was not able to find text 'AppGameKit Studio Editor Style:' in the clipboard. Make sure you copy all the style text including all text.");
						}
					}
					else {
						strcpy(import_error_msg, "Error: Clipboard was empty ?");
					}

				}

				if (ImGui::Button("Reset##Identifier")) { pref.EditorColors[(int)PaletteIndex::Identifier] = 0xffcccccc; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Identifier]);
				if (ImGui::ColorEdit4("Identifier##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Identifier] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##Keyword")) { pref.EditorColors[(int)PaletteIndex::Keyword] = 0xffd79c56; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Keyword]);
				if (ImGui::ColorEdit4("Keyword##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Keyword] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##Number")) { pref.EditorColors[(int)PaletteIndex::Number] = 0xff00ff00; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Number]);
				if (ImGui::ColorEdit4("Number##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Number] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}


				if (ImGui::Button("Reset##String")) { pref.EditorColors[(int)PaletteIndex::String] = 0xff4778ff; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::String]);
				if (ImGui::ColorEdit4("String##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::String] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##CharLiteral")) { pref.EditorColors[(int)PaletteIndex::CharLiteral] = 0xff70a0e0; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::CharLiteral]);
				if (ImGui::ColorEdit4("Char Literal##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::CharLiteral] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				//Used ?
//				if (ImGui::Button("Reset##Punctuation")) { pref.EditorColors[(int)PaletteIndex::Punctuation] = 0xffffffff; } ImGui::SameLine();
//				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Punctuation]);
//				if (ImGui::ColorEdit4("Punctuation##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
//					pref.EditorColors[(int)PaletteIndex::Punctuation] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
//				}

				if (ImGui::Button("Reset##Preprocessor")) { pref.EditorColors[(int)PaletteIndex::Preprocessor] = 0xff409090; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Preprocessor]);
				if (ImGui::ColorEdit4("Block##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Preprocessor] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##KnownIdentifier")) { pref.EditorColors[(int)PaletteIndex::KnownIdentifier] = 0xffb2c856; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::KnownIdentifier]);
				if (ImGui::ColorEdit4("Known Identifier##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::KnownIdentifier] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				//inactiver
//				if (ImGui::Button("Reset##PreprocIdentifier")) { pref.EditorColors[(int)PaletteIndex::PreprocIdentifier] = 0xffc040a0; } ImGui::SameLine();
//				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::PreprocIdentifier]);
//				if (ImGui::ColorEdit4("PreprocIdentifier##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
//					pref.EditorColors[(int)PaletteIndex::PreprocIdentifier] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
//				}

				if (ImGui::Button("Reset##Comment")) { pref.EditorColors[(int)PaletteIndex::Comment] = 0xff4aaa56; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Comment]);
				if (ImGui::ColorEdit4("Comment##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Comment] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##Background")) { pref.EditorColors[(int)PaletteIndex::Background] = 0xff181818; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Background]);
				if (ImGui::ColorEdit4("Background##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Background] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##Cursor")) { pref.EditorColors[(int)PaletteIndex::Cursor] = 0xffe0e0e0; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Cursor]);
				if (ImGui::ColorEdit4("Cursor##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Cursor] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##Selection")) { pref.EditorColors[(int)PaletteIndex::Selection] = 0x80808080; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Selection]);
				if (ImGui::ColorEdit4("Selection##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Selection] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}


				if (ImGui::Button("Reset##ErrorMarker")) { pref.EditorColors[(int)PaletteIndex::ErrorMarker] = 0x800020ff; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::ErrorMarker]);
				if (ImGui::ColorEdit4("ErrorMarker / BreakPoint##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::ErrorMarker] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##Breakpoint")) { pref.EditorColors[(int)PaletteIndex::Breakpoint] = 0x40f08000; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::Breakpoint]);
				if (ImGui::ColorEdit4("Debug Line##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::Breakpoint] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##LineNumber")) { pref.EditorColors[(int)PaletteIndex::LineNumber] = 0xff808080; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::LineNumber]);
				if (ImGui::ColorEdit4("LineNumber##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::LineNumber] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}

				if (ImGui::Button("Reset##CurrentLineFill")) { pref.EditorColors[(int)PaletteIndex::CurrentLineFill] = 0xff808080; } ImGui::SameLine();
				tmpvec4 = ImGui::ColorConvertU32ToFloat4(pref.EditorColors[(int)PaletteIndex::CurrentLineFill]);
				if (ImGui::ColorEdit4("Similar Keywords##ecol", (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
					pref.EditorColors[(int)PaletteIndex::CurrentLineFill] = ImGui::ColorConvertFloat4ToU32(tmpvec4);
				}
				
				//Inactive , dups.
				pref.EditorColors[(int)PaletteIndex::PreprocIdentifier] = pref.EditorColors[(int)PaletteIndex::Identifier];
				pref.EditorColors[(int)PaletteIndex::MultiLineComment] = pref.EditorColors[(int)PaletteIndex::Comment];
				pref.EditorColors[(int)PaletteIndex::Default] = pref.EditorColors[(int)PaletteIndex::Identifier];
				pref.EditorColors[(int)PaletteIndex::CurrentLineFillInactive] = 0x40808080; //inactive
				pref.EditorColors[(int)PaletteIndex::CurrentLineEdge] = 0x40a0a0a0; //inactive
				pref.EditorColors[(int)PaletteIndex::BackgroundLine] = pref.EditorColors[(int)PaletteIndex::Background];

				if (1 == 2) { //not really needed now.
					if (ImGui::Button("Reset All Editor Colors")) {
						pref.EditorColors[(int)PaletteIndex::Identifier] = 0xffcccccc;
						pref.EditorColors[(int)PaletteIndex::Keyword] = 0xffd79c56;
						pref.EditorColors[(int)PaletteIndex::Number] = 0xff00ff00;
						pref.EditorColors[(int)PaletteIndex::String] = 0xff4778ff;
						pref.EditorColors[(int)PaletteIndex::CharLiteral] = 0xff70a0e0;
						pref.EditorColors[(int)PaletteIndex::Punctuation] = 0xffffffff;
						pref.EditorColors[(int)PaletteIndex::Preprocessor] = 0xff409090;
						pref.EditorColors[(int)PaletteIndex::KnownIdentifier] = 0xffb2c856;
						pref.EditorColors[(int)PaletteIndex::PreprocIdentifier] = 0xffc040a0;
						pref.EditorColors[(int)PaletteIndex::Comment] = 0xff4aaa56;
						pref.EditorColors[(int)PaletteIndex::Background] = 0xff181818;
						pref.EditorColors[(int)PaletteIndex::Cursor] = 0xffe0e0e0;
						pref.EditorColors[(int)PaletteIndex::Selection] = 0x80808080;
						pref.EditorColors[(int)PaletteIndex::ErrorMarker] = 0x800020ff;
						pref.EditorColors[(int)PaletteIndex::Breakpoint] = 0x40f08000;
						pref.EditorColors[(int)PaletteIndex::LineNumber] = 0xff808080;
						pref.EditorColors[(int)PaletteIndex::CurrentLineFill] = 0xff808080;  // last keyword mark.

						//inactive dups.
						pref.EditorColors[(int)PaletteIndex::MultiLineComment] = pref.EditorColors[(int)PaletteIndex::Comment];
						pref.EditorColors[(int)PaletteIndex::Default] = pref.EditorColors[(int)PaletteIndex::Identifier];
						pref.EditorColors[(int)PaletteIndex::CurrentLineFillInactive] = 0x40808080; //inactive
						pref.EditorColors[(int)PaletteIndex::CurrentLineEdge] = 0x40a0a0a0; //inactive
						pref.EditorColors[(int)PaletteIndex::BackgroundLine] = pref.EditorColors[(int)PaletteIndex::Background];

					}
				}
			}
			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(" Keyboard Shortcuts "))
		{
			int cpos = fs * 18;
			ImGuiIO& io = ImGui::GetIO();
			bool is_osx = io.ConfigMacOSXBehaviors;
			char ctrltext[8];
			if (is_osx)
				strcpy(ctrltext, "Cmd");
			else
				strcpy(ctrltext, "Ctrl");

			//This is so stupid but UNDO...
			bool key_changed = false;
			ImGui::Text("Undo: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Undo", &pref.bUndoCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Undo", &pref.bUndoCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Undo", &pref.bUndoShift)) key_changed = true;
			ImGui::SameLine();
			if(ImGui::Checkbox("alt##Undo", &pref.bUndoAlt) ) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs*16);

			if (pref.iUndoKey > IM_ARRAYSIZE(key_values))
				pref.iUndoKey = 90;

			if (ImGui::BeginCombo("##comboundovalues", key_values[ pref.iUndoKey ] , 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iUndoKey == n);
					if(strncmp(key_values[n],"##",2) != 0 )
					if (ImGui::Selectable(key_values[n] , is_selected)) {
						pref.iUndoKey = n;
						key_changed = true;
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iUndoKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cUndoText, "");
				if (pref.bUndoCtrl) {
					if (is_osx)
						strcat(pref.cUndoText, "Cmd+");
					else
						strcat(pref.cUndoText, "Ctrl+");
				}
				if (pref.bUndoShift)
					strcat(pref.cUndoText, "Shift+");
				if (pref.bUndoAlt)
					strcat(pref.cUndoText, "Alt+");
				strcat(pref.cUndoText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cUndoText);


			//##############
			//#### REDO ####
			//##############
			key_changed = false;
			ImGui::Text("Redo: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Redo", &pref.bRedoCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Redo", &pref.bRedoCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Redo", &pref.bRedoShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Redo", &pref.bRedoAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iRedoKey > IM_ARRAYSIZE(key_values))
				pref.iRedoKey = 89;

			if (ImGui::BeginCombo("##comboRedovalues", key_values[pref.iRedoKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iRedoKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iRedoKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iRedoKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cRedoText, "");
				if (pref.bRedoCtrl) {
					if (is_osx)
						strcat(pref.cRedoText, "Cmd+");
					else
						strcat(pref.cRedoText, "Ctrl+");
				}
				if (pref.bRedoShift)
					strcat(pref.cRedoText, "Shift+");
				if (pref.bRedoAlt)
					strcat(pref.cRedoText, "Alt+");
				strcat(pref.cRedoText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cRedoText);

			//##############
			//#### COPY ####
			//##############
			key_changed = false;
			ImGui::Text("Copy: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Copy", &pref.bCopyCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Copy", &pref.bCopyCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Copy", &pref.bCopyShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Copy", &pref.bCopyAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iCopyKey > IM_ARRAYSIZE(key_values))
				pref.iCopyKey = 89;

			if (ImGui::BeginCombo("##comboCopyvalues", key_values[pref.iCopyKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iCopyKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iCopyKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iCopyKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cCopyText, "");
				if (pref.bCopyCtrl) {
					if (is_osx)
						strcat(pref.cCopyText, "Cmd+");
					else
						strcat(pref.cCopyText, "Ctrl+");
				}
				if (pref.bCopyShift)
					strcat(pref.cCopyText, "Shift+");
				if (pref.bCopyAlt)
					strcat(pref.cCopyText, "Alt+");
				strcat(pref.cCopyText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cCopyText);


			//###############
			//#### PASTE ####
			//###############
			key_changed = false;
			ImGui::Text("Paste: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Paste", &pref.bPasteCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Paste", &pref.bPasteCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Paste", &pref.bPasteShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Paste", &pref.bPasteAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iPasteKey > IM_ARRAYSIZE(key_values))
				pref.iPasteKey = 89;

			if (ImGui::BeginCombo("##comboPastevalues", key_values[pref.iPasteKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iPasteKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iPasteKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iPasteKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cPasteText, "");
				if (pref.bPasteCtrl) {
					if (is_osx)
						strcat(pref.cPasteText, "Cmd+");
					else
						strcat(pref.cPasteText, "Ctrl+");
				}
				if (pref.bPasteShift)
					strcat(pref.cPasteText, "Shift+");
				if (pref.bPasteAlt)
					strcat(pref.cPasteText, "Alt+");
				strcat(pref.cPasteText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cPasteText);


			//#############
			//#### CUT ####
			//#############
			key_changed = false;
			ImGui::Text("Cut: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Cut", &pref.bCutCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Cut", &pref.bCutCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Cut", &pref.bCutShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Cut", &pref.bCutAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iCutKey > IM_ARRAYSIZE(key_values))
				pref.iCutKey = 89;

			if (ImGui::BeginCombo("##comboCutvalues", key_values[pref.iCutKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iCutKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iCutKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iCutKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cCutText, "");
				if (pref.bCutCtrl) {
					if (is_osx)
						strcat(pref.cCutText, "Cmd+");
					else
						strcat(pref.cCutText, "Ctrl+");
				}
				if (pref.bCutShift)
					strcat(pref.cCutText, "Shift+");
				if (pref.bCutAlt)
					strcat(pref.cCutText, "Alt+");
				strcat(pref.cCutText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cCutText);


			//##############
			//#### FIND ####
			//##############
			key_changed = false;
			ImGui::Text("Find: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Find", &pref.bFindCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Find", &pref.bFindCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Find", &pref.bFindShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Find", &pref.bFindAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iFindKey > IM_ARRAYSIZE(key_values))
				pref.iFindKey = 89;

			if (ImGui::BeginCombo("##comboFindvalues", key_values[pref.iFindKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iFindKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iFindKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iFindKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cFindText, "");
				if (pref.bFindCtrl) {
					if (is_osx)
						strcat(pref.cFindText, "Cmd+");
					else
						strcat(pref.cFindText, "Ctrl+");
				}
				if (pref.bFindShift)
					strcat(pref.cFindText, "Shift+");
				if (pref.bFindAlt)
					strcat(pref.cFindText, "Alt+");
				strcat(pref.cFindText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cFindText);



			//###################
			//#### FIND NEXT ####
			//###################
			key_changed = false;
			ImGui::Text("Find Next: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##FindNext", &pref.bFindNextCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##FindNext", &pref.bFindNextCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##FindNext", &pref.bFindNextShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##FindNext", &pref.bFindNextAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iFindNextKey > IM_ARRAYSIZE(key_values))
				pref.iFindNextKey = 89;

			if (ImGui::BeginCombo("##comboFindNextvalues", key_values[pref.iFindNextKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iFindNextKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iFindNextKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iFindNextKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cFindNextText, "");
				if (pref.bFindNextCtrl) {
					if (is_osx)
						strcat(pref.cFindNextText, "Cmd+");
					else
						strcat(pref.cFindNextText, "Ctrl+");
				}
				if (pref.bFindNextShift)
					strcat(pref.cFindNextText, "Shift+");
				if (pref.bFindNextAlt)
					strcat(pref.cFindNextText, "Alt+");
				strcat(pref.cFindNextText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cFindNextText);

			//#################
			//#### REPLACE ####
			//#################
			key_changed = false;
			ImGui::Text("Replace: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Replace", &pref.bReplaceCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Replace", &pref.bReplaceCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Replace", &pref.bReplaceShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Replace", &pref.bReplaceAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iReplaceKey > IM_ARRAYSIZE(key_values))
				pref.iReplaceKey = 89;

			if (ImGui::BeginCombo("##comboReplacevalues", key_values[pref.iReplaceKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iReplaceKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iReplaceKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iReplaceKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cReplaceText, "");
				if (pref.bReplaceCtrl) {
					if (is_osx)
						strcat(pref.cReplaceText, "Cmd+");
					else
						strcat(pref.cReplaceText, "Ctrl+");
				}
				if (pref.bReplaceShift)
					strcat(pref.cReplaceText, "Shift+");
				if (pref.bReplaceAlt)
					strcat(pref.cReplaceText, "Alt+");
				strcat(pref.cReplaceText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cReplaceText);


			//#################
			//#### ZOOM IN ####
			//#################
			key_changed = false;
			ImGui::Text("Zoom In: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##ZoomIn", &pref.bZoomInCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##ZoomIn", &pref.bZoomInCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##ZoomIn", &pref.bZoomInShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##ZoomIn", &pref.bZoomInAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iZoomInKey > IM_ARRAYSIZE(key_values))
				pref.iZoomInKey = 89;

			if (ImGui::BeginCombo("##comboZoomInvalues", key_values[pref.iZoomInKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iZoomInKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iZoomInKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iZoomInKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cZoomInText, "");
				if (pref.bZoomInCtrl) {
					if (is_osx)
						strcat(pref.cZoomInText, "Cmd+");
					else
						strcat(pref.cZoomInText, "Ctrl+");
				}
				if (pref.bZoomInShift)
					strcat(pref.cZoomInText, "Shift+");
				if (pref.bZoomInAlt)
					strcat(pref.cZoomInText, "Alt+");
				strcat(pref.cZoomInText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cZoomInText);


			//##################
			//#### ZOOM OUT ####
			//##################
			key_changed = false;
			ImGui::Text("Zoom Out: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##ZoomOut", &pref.bZoomOutCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##ZoomOut", &pref.bZoomOutCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##ZoomOut", &pref.bZoomOutShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##ZoomOut", &pref.bZoomOutAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iZoomOutKey > IM_ARRAYSIZE(key_values))
				pref.iZoomOutKey = 89;

			if (ImGui::BeginCombo("##comboZoomOutvalues", key_values[pref.iZoomOutKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iZoomOutKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iZoomOutKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iZoomOutKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cZoomOutText, "");
				if (pref.bZoomOutCtrl) {
					if (is_osx)
						strcat(pref.cZoomOutText, "Cmd+");
					else
						strcat(pref.cZoomOutText, "Ctrl+");
				}
				if (pref.bZoomOutShift)
					strcat(pref.cZoomOutText, "Shift+");
				if (pref.bZoomOutAlt)
					strcat(pref.cZoomOutText, "Alt+");
				strcat(pref.cZoomOutText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cZoomOutText);


			//###################
			//#### GOTO LINE ####
			//###################
			key_changed = false;
			ImGui::Text("Go to Line: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##GotoLine", &pref.bGotoLineCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##GotoLine", &pref.bGotoLineCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##GotoLine", &pref.bGotoLineShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##GotoLine", &pref.bGotoLineAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iGotoLineKey > IM_ARRAYSIZE(key_values))
				pref.iGotoLineKey = 89;

			if (ImGui::BeginCombo("##comboGotoLinevalues", key_values[pref.iGotoLineKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iGotoLineKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iGotoLineKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iGotoLineKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cGotoLineText, "");
				if (pref.bGotoLineCtrl) {
					if (is_osx)
						strcat(pref.cGotoLineText, "Cmd+");
					else
						strcat(pref.cGotoLineText, "Ctrl+");
				}
				if (pref.bGotoLineShift)
					strcat(pref.cGotoLineText, "Shift+");
				if (pref.bGotoLineAlt)
					strcat(pref.cGotoLineText, "Alt+");
				strcat(pref.cGotoLineText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cGotoLineText);

			//#############
			//#### RUN ####
			//#############
			key_changed = false;
			ImGui::Text("Run: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Run", &pref.bRunCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Run", &pref.bRunCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Run", &pref.bRunShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Run", &pref.bRunAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iRunKey > IM_ARRAYSIZE(key_values))
				pref.iRunKey = 89;

			if (ImGui::BeginCombo("##comboRunvalues", key_values[pref.iRunKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iRunKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iRunKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iRunKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cRunText, "");
				if (pref.bRunCtrl) {
					if (is_osx)
						strcat(pref.cRunText, "Cmd+");
					else
						strcat(pref.cRunText, "Ctrl+");
				}
				if (pref.bRunShift)
					strcat(pref.cRunText, "Shift+");
				if (pref.bRunAlt)
					strcat(pref.cRunText, "Alt+");
				strcat(pref.cRunText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cRunText);

			//#################
			//#### COMPILE ####
			//#################
			key_changed = false;
			ImGui::Text("Compile: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Compile", &pref.bCompileCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Compile", &pref.bCompileCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Compile", &pref.bCompileShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Compile", &pref.bCompileAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iCompileKey > IM_ARRAYSIZE(key_values))
				pref.iCompileKey = 89;

			if (ImGui::BeginCombo("##comboCompilevalues", key_values[pref.iCompileKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iCompileKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iCompileKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iCompileKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cCompileText, "");
				if (pref.bCompileCtrl) {
					if (is_osx)
						strcat(pref.cCompileText, "Cmd+");
					else
						strcat(pref.cCompileText, "Ctrl+");
				}
				if (pref.bCompileShift)
					strcat(pref.cCompileText, "Shift+");
				if (pref.bCompileAlt)
					strcat(pref.cCompileText, "Alt+");
				strcat(pref.cCompileText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cCompileText);


			//###################
			//#### BROADCAST ####
			//###################
			key_changed = false;
			ImGui::Text("Broadcast: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##BroadCast", &pref.bBroadCastCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##BroadCast", &pref.bBroadCastCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##BroadCast", &pref.bBroadCastShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##BroadCast", &pref.bBroadCastAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iBroadCastKey > IM_ARRAYSIZE(key_values))
				pref.iBroadCastKey = 89;

			if (ImGui::BeginCombo("##comboBroadCastvalues", key_values[pref.iBroadCastKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iBroadCastKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iBroadCastKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iBroadCastKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cBroadCastText, "");
				if (pref.bBroadCastCtrl) {
					if (is_osx)
						strcat(pref.cBroadCastText, "Cmd+");
					else
						strcat(pref.cBroadCastText, "Ctrl+");
				}
				if (pref.bBroadCastShift)
					strcat(pref.cBroadCastText, "Shift+");
				if (pref.bBroadCastAlt)
					strcat(pref.cBroadCastText, "Alt+");
				strcat(pref.cBroadCastText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cBroadCastText);


			//###############
			//#### DEBUG ####
			//###############
			key_changed = false;
			ImGui::Text("Debug: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##Debug", &pref.bDebugCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##Debug", &pref.bDebugCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##Debug", &pref.bDebugShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##Debug", &pref.bDebugAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iDebugKey > IM_ARRAYSIZE(key_values))
				pref.iDebugKey = 89;

			if (ImGui::BeginCombo("##comboDebugvalues", key_values[pref.iDebugKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iDebugKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iDebugKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iDebugKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cDebugText, "");
				if (pref.bDebugCtrl) {
					if (is_osx)
						strcat(pref.cDebugText, "Cmd+");
					else
						strcat(pref.cDebugText, "Ctrl+");
				}
				if (pref.bDebugShift)
					strcat(pref.cDebugText, "Shift+");
				if (pref.bDebugAlt)
					strcat(pref.cDebugText, "Alt+");
				strcat(pref.cDebugText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cDebugText);

			//####################
			//#### DEBUG STEP ####
			//####################

			key_changed = false;
			ImGui::Text("Debug Step: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##DebugStep", &pref.bDebugStepCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##DebugStep", &pref.bDebugStepCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##DebugStep", &pref.bDebugStepShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##DebugStep", &pref.bDebugStepAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iDebugStepKey > IM_ARRAYSIZE(key_values))
				pref.iDebugStepKey = 89;

			if (ImGui::BeginCombo("##comboDebugStepvalues", key_values[pref.iDebugStepKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iDebugStepKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iDebugStepKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iDebugStepKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cDebugStepText, "");
				if (pref.bDebugStepCtrl) {
					if (is_osx)
						strcat(pref.cDebugStepText, "Cmd+");
					else
						strcat(pref.cDebugStepText, "Ctrl+");
				}
				if (pref.bDebugStepShift)
					strcat(pref.cDebugStepText, "Shift+");
				if (pref.bDebugStepAlt)
					strcat(pref.cDebugStepText, "Alt+");
				strcat(pref.cDebugStepText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cDebugStepText);


			//###################
			//#### OPEN FILE ####
			//###################
			key_changed = false;
			ImGui::Text("Open Source File: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##OpenFile", &pref.bOpenFileCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##OpenFile", &pref.bOpenFileCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##OpenFile", &pref.bOpenFileShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##OpenFile", &pref.bOpenFileAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iOpenFileKey > IM_ARRAYSIZE(key_values))
				pref.iOpenFileKey = 89;

			if (ImGui::BeginCombo("##comboOpenFilevalues", key_values[pref.iOpenFileKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iOpenFileKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iOpenFileKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iOpenFileKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cOpenFileText, "");
				if (pref.bOpenFileCtrl) {
					if (is_osx)
						strcat(pref.cOpenFileText, "Cmd+");
					else
						strcat(pref.cOpenFileText, "Ctrl+");
				}
				if (pref.bOpenFileShift)
					strcat(pref.cOpenFileText, "Shift+");
				if (pref.bOpenFileAlt)
					strcat(pref.cOpenFileText, "Alt+");
				strcat(pref.cOpenFileText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cOpenFileText);


			//######################
			//#### OPEN PROJECT ####
			//######################
			key_changed = false;
			ImGui::Text("Open Project: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##OpenProject", &pref.bOpenProjectCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##OpenProject", &pref.bOpenProjectCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##OpenProject", &pref.bOpenProjectShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##OpenProject", &pref.bOpenProjectAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iOpenProjectKey > IM_ARRAYSIZE(key_values))
				pref.iOpenProjectKey = 89;

			if (ImGui::BeginCombo("##comboOpenProjectvalues", key_values[pref.iOpenProjectKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iOpenProjectKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iOpenProjectKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iOpenProjectKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cOpenProjectText, "");
				if (pref.bOpenProjectCtrl) {
					if (is_osx)
						strcat(pref.cOpenProjectText, "Cmd+");
					else
						strcat(pref.cOpenProjectText, "Ctrl+");
				}
				if (pref.bOpenProjectShift)
					strcat(pref.cOpenProjectText, "Shift+");
				if (pref.bOpenProjectAlt)
					strcat(pref.cOpenProjectText, "Alt+");
				strcat(pref.cOpenProjectText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cOpenProjectText);


			//##########################
			//#### GO TO DEFINITION ####
			//##########################

			key_changed = false;
			ImGui::Text("Go To Definition: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##GoToDefinition", &pref.bGoToDefinitionCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##GoToDefinition", &pref.bGoToDefinitionCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##GoToDefinition", &pref.bGoToDefinitionShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##GoToDefinition", &pref.bGoToDefinitionAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iGoToDefinitionKey > IM_ARRAYSIZE(key_values))
				pref.iGoToDefinitionKey = 89;

			if (ImGui::BeginCombo("##comboGoToDefinitionvalues", key_values[pref.iGoToDefinitionKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iGoToDefinitionKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iGoToDefinitionKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iGoToDefinitionKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cGoToDefinitionText, "");
				if (pref.bGoToDefinitionCtrl) {
					if (is_osx)
						strcat(pref.cGoToDefinitionText, "Cmd+");
					else
						strcat(pref.cGoToDefinitionText, "Ctrl+");
				}
				if (pref.bGoToDefinitionShift)
					strcat(pref.cGoToDefinitionText, "Shift+");
				if (pref.bGoToDefinitionAlt)
					strcat(pref.cGoToDefinitionText, "Alt+");
				strcat(pref.cGoToDefinitionText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cGoToDefinitionText);


			//########################
			//#### DUPLICATE LINE ####
			//##########################

			key_changed = false;
			ImGui::Text("Duplicate Line: ");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(cpos, ImGui::GetCursorPos().y));
			if (is_osx) {
				if (ImGui::Checkbox("cmd##DuplicateLine", &pref.bDuplicateLineCtrl)) key_changed = true;
			}
			else {
				if (ImGui::Checkbox("ctrl##DuplicateLine", &pref.bDuplicateLineCtrl)) key_changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("shift##DuplicateLine", &pref.bDuplicateLineShift)) key_changed = true;
			ImGui::SameLine();
			if (ImGui::Checkbox("alt##DuplicateLine", &pref.bDuplicateLineAlt)) key_changed = true;

			ImGui::SameLine();
			ImGui::PushItemWidth(fs * 16);

			if (pref.iDuplicateLineKey > IM_ARRAYSIZE(key_values))
				pref.iDuplicateLineKey = 89;

			if (ImGui::BeginCombo("##comboDuplicateLinevalues", key_values[pref.iDuplicateLineKey], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(key_values); n++)
				{
					bool is_selected = (pref.iDuplicateLineKey == n);
					if (strncmp(key_values[n], "##", 2) != 0)
						if (ImGui::Selectable(key_values[n], is_selected)) {
							pref.iDuplicateLineKey = n;
							key_changed = true;
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
				}
				ImGui::EndCombo();
			}
			if (key_changed) {
				uString ktext = key_values[pref.iDuplicateLineKey];
				ktext.ReplaceStr("KEY_", "");
				ktext.ReplaceStr("SUBTRACT", "-");
				ktext.ReplaceStr("PLUS", "+");
				strcpy(pref.cDuplicateLineText, "");
				if (pref.bDuplicateLineCtrl) {
					if (is_osx)
						strcat(pref.cDuplicateLineText, "Cmd+");
					else
						strcat(pref.cDuplicateLineText, "Ctrl+");
				}
				if (pref.bDuplicateLineShift)
					strcat(pref.cDuplicateLineText, "Shift+");
				if (pref.bDuplicateLineAlt)
					strcat(pref.cDuplicateLineText, "Alt+");
				strcat(pref.cDuplicateLineText, ktext);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::Text(pref.cDuplicateLineText);


			CheckKeyboardShortcuts();
			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}

	ImGui::Separator();
	ImVec2 ws = ImGui::GetWindowSize();

	//ImGui::SetCursorPos(ImVec2(ws.x-(fs*19), ws.y-(fs*4) + ImGui::GetScrollY() ));
	ImGui::Indent();
	if( ImGui::GetCursorPosY() < ws.y - (fs * 4) )
		ImGui::SetCursorPos(ImVec2( ImGui::GetCursorPosX() , ws.y-(fs*4) + ImGui::GetScrollY() ));

	if (ImGui::Button(" Reset ")) {
		pref.show_preferences_window = false;
		ResetPreferences(false);
		myDefaultStyle(NULL);
		pref.idePalette = 0;
		extern int changefonttype;
		changefonttype = 999;
	}
	ImGui::SameLine();
	if (ImGui::Button(" Close ")) {
		pref.show_preferences_window = false;
	}

	ImGui::End();

}


void ProcessCodeProperties(void)
{
	//m_ActiveEditor
	if (!m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].empty()) {

		for (std::map<std::int32_t, TextEditor::SymbolsValues>::iterator it = m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].begin(); it != m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].end(); ++it)
		{
			char *found;
			if (it->second.iSymbolType == 1) {
				//global;myvarx#;=;0.0;//[IDEGUIADD],float,Adjust x position of object.
				int iInPutType = 0, iInputPos = 0;
				if ((found = (char *)pestrcasestr(it->second.cParameters, ",float,"))) {
					iInPutType = 1;
					iInputPos = 7;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",integer,"))) {
					iInPutType = 2;
					iInputPos = 9;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",string,"))) {
					iInPutType = 3;
					iInputPos = 8;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",selectfile,"))) {
					iInPutType = 7;
					iInputPos = 12;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",selectfolder,"))) {
					iInPutType = 8;
					iInputPos = 14;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",vec4color,"))) {
					iInPutType = 4;
					iInputPos = 11;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",vec3,"))) {
					iInPutType = 5;
					iInputPos = 6;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",vec4,"))) {
					iInPutType = 6;
					iInputPos = 6;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",header,"))) {
					found += 8;
					uString msg = found;
					msg.ReplaceStr(";", " ");
					msg.ReplaceStr("\"", "");
					//msg.ReplaceStr(")", "");
					//msg.ReplaceStr("(", "");
					ImVec4* style_colors = ImGui::GetStyle().Colors;
					ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
					ImGui::SetWindowFontScale(1.35);
					style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
					ImGui::TextWrapped(msg);
					style_colors[ImGuiCol_Text] = oldTextColor;
					ImGui::SetWindowFontScale(1.0);
					found = NULL;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",message,"))) {
					found += 9;
					uString msg = found;
					msg.ReplaceStr(";", " ");
					msg.ReplaceStr("\"", "");
					//msg.ReplaceStr(")", "");
					//msg.ReplaceStr("(", "");

					ImGui::TextWrapped(msg);
					found = NULL;
				}
				//Ups not spelled correct , keep for now.
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",seperator,"))) {
					found += 11;
					uString msg = found;
					msg.ReplaceStr(";", " ");
					msg.ReplaceStr("\"", "");
					//msg.ReplaceStr(")", "");
					//msg.ReplaceStr("(", "");

					ImGui::Separator();
					found = NULL;
				}
				else if ((found = (char *)pestrcasestr(it->second.cParameters, ",separator,"))) {
					found += 11;
					uString msg = found;
					msg.ReplaceStr(";", " ");
					msg.ReplaceStr("\"", "");
					//msg.ReplaceStr(")", "");
					//msg.ReplaceStr("(", "");

					ImGui::Separator();
					found = NULL;
				}


				if( found ) {
					uString msg,var,cmd,rest,value,newcmd;
					msg.SetStr(found + iInputPos);
					cmd.SetStr(it->second.cCommand);
					int pos = cmd.FindStr("global;", 1);
					if (pos >= 0) {
						cmd.SubString(rest, pos + 7);
						int pos2 = rest.FindStr(";=");
						if( pos2 < 0 ) pos2 = rest.FindStr("=");

						if (pos2 < 0) pos2 = rest.FindStr(";as;");

						if (pos2 > 0) {
							rest.SubString(var, 0, pos2);
							//Clean var.
							var.ReplaceStr(";as;string", "");
							var.ReplaceStr(";as;integer", "");
							var.ReplaceStr(";as;float", "");
							var.ReplaceStr(";as;String", "");
							var.ReplaceStr(";as;Integer", "");
							var.ReplaceStr(";as;Float", "");
							var.ReplaceStr(";as;STRING", "");
							var.ReplaceStr(";as;INTEGER", "");
							var.ReplaceStr(";as;FLOAT", "");

							int pos3_length = 3;
							int pos3 = cmd.FindStr(";=;");
							if (pos3 < 0) {
								pos3 = cmd.FindStr(";=");
								if(pos3 > 0)
									pos3_length = 2;
							}
							if (pos3 < 0) {
								pos3 = cmd.FindStr("=");
								if (pos3 > 0)
									pos3_length = 1;
							}
							if (pos3 < 0) {
								pos3 = cmd.FindStr(";as;");
								pos3_length = 3;
							}

							int pos4 = cmd.GetLength();
							if( pos4 > 0 && cmd[pos4 - 1] == ';')
								pos4--;
							if (pos4 > 0 && cmd[pos4 - 1] == ';')
								pos4--;

							if (pos3 > 0 && pos4 > 0) {
								cmd.SubString(value, pos3+ pos3_length, pos4 - (pos3 + pos3_length) );
								//Ready.
								float tmpfloat = 0.0;
								int tmpint = 0;
								ImVec4 tmpvec4;
								uString tmpstring;
								char tmpchar[MAX_PATH];
								strcpy(tmpchar, "");

								if (iInPutType == 1)
									tmpfloat = atof(value.GetStr());
								if (iInPutType == 2)
									tmpint = atoi(value.GetStr());
								if (value.GetLength() >= MAX_PATH - 10)
									iInPutType == 0;
								if (iInPutType == 3 || iInPutType == 7 || iInPutType == 8) {
									value.ReplaceStr(";", " ");
									tmpstring = value;
									tmpstring.Trim("\"");
									strcpy(tmpchar, tmpstring.GetStr());
								}
								if (iInPutType == 4 || iInPutType == 5 || iInPutType == 6) {
									//Get all vector variables.
									//cmd.SubString(newcmd, 0, pos3 + 3);

									for (std::map<std::int32_t, TextEditor::SymbolsValues>::iterator it2 = m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].begin(); it2 != m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].end(); ++it2)
									{
										//
										char *found2;
										if ((found2 = (char *)pestrcasestr(it2->second.cParameters, ",variable,"))) {
											//iInputPos = 10;
										}
										if (found2) {
											uString findVar;
											for (int lf = 0; lf < 4; lf++) {
												findVar = ";";
												findVar.Append(var);
												if(lf == 0)
													findVar.Append(".x;=;");
												else if (lf == 1)
													findVar.Append(".y;=;");
												else if (lf == 2)
													findVar.Append(".z;=;");
												else if (lf == 3)
													findVar.Append(".w;=;");

												if ((found2 = (char *)pestrcasestr(it2->second.cCommand, findVar.GetStr()))) {
													found2 += findVar.GetLength();
													uString value2 = found2;
													value2.ReplaceStr(";", "");
													value2.ReplaceStr(" ", "");
													value2.ReplaceStr("\"", "");
													if (lf == 0)
														tmpvec4.x = atof(value2);
													else if (lf == 1)
														tmpvec4.y = atof(value2);
													else if (lf == 2)
														tmpvec4.z = atof(value2);
													else if (lf == 3)
														tmpvec4.w = atof(value2);

												}
											}
										}
									}

								}

								msg.ReplaceStr(";", " ");
								msg.ReplaceStr("\"", "");
								//msg.ReplaceStr(")", "");
								//msg.ReplaceStr("(", "");

								bool bSetVec4 = false;
								uString uniqueLabel = msg; //var;
								uniqueLabel.Append("##IDEGUIinput");
								uniqueLabel.AppendInt((int)it->second.lineno);
								if(iInPutType >= 4 && iInPutType <= 6)
									ImGui::PushItemWidth(200);
								else if(iInPutType == 7 || iInPutType == 8)
									ImGui::PushItemWidth(-50);
								else
									ImGui::PushItemWidth(130);
								if (iInPutType == 1) {
									if (ImGui::InputFloat(uniqueLabel, &tmpfloat, 0.01, 0.1, 6)) {
										//Update value.
										cmd.SubString(newcmd, 0, pos3 + pos3_length);
										cmd.SubString(rest, pos4);
										newcmd.AppendFloat(tmpfloat);
										newcmd.Append(rest);
										strcpy(it->second.cCommand, newcmd.GetStr());
										//Replace in update.
										std::map<std::int32_t, TextEditor::SymbolsValues>::iterator it2 = m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].find(it->first);
										if (it2 != m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].end()) {
											strcpy(it2->second.cCommand, newcmd.GetStr());
										}

										//Replace
										uString replace, replacewith;
										if (newcmd.FindStr(";as;float;=;", 1) > 0) {
											replace = var;
											replace.Append(" as float = ");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as float = ");
											replacewith.AppendFloat(tmpfloat);
										}
										else if (newcmd.FindStr(";as;float;=", 1) > 0) {
											replace = var;
											replace.Append(" as float =");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as float =");
											replacewith.AppendFloat(tmpfloat);
										}
										else if (newcmd.FindStr(";as;float=", 1) > 0) {
											replace = var;
											replace.Append(" as float=");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as float=");
											replacewith.AppendFloat(tmpfloat);
										}
										else {
											if (pos3_length == 1) {
												replace = var;
												replace.Append("=");
												replace.Append(value);
												replacewith = var;
												replacewith.Append("=");
												replacewith.AppendFloat(tmpfloat);
											}
											else if (pos3_length == 2) {
												replace = var;
												replace.Append(" =");
												replace.Append(value);
												replacewith = var;
												replacewith.Append(" =");
												replacewith.AppendFloat(tmpfloat);
											}
											else {
												replace = var;
												replace.Append(" = ");
												replace.Append(value);
												replacewith = var;
												replacewith.Append(" = ");
												replacewith.AppendFloat(tmpfloat);
											}
										}
										m_ActiveEditor->DirectReplace((char *)replace.GetStr(), (char *)replacewith.GetStr(), (int)it->second.lineno);
										if (debug_is_running && bSetVariableActive) {
											char szTmp[MAX_PATH + 80];
											sprintf(szTmp, "setvariable %s %f", var.GetStr(), tmpfloat);
											uString bc_err;
											BroadcasterCommand(szTmp,bc_err);
										}
										else if (debug_is_running && !updateWatchCalled && pref.bDebugBringToFront) {
												uString bc_err;
												BroadcasterCommand("pause",bc_err);
												BroadcasterCommand("continue",bc_err);
										}
									}
								}
								else if (iInPutType == 2) {
									if (ImGui::InputInt(uniqueLabel, &tmpint, 1, 10 )) {
										//Update value.
										cmd.SubString(newcmd, 0, pos3 + pos3_length);
										cmd.SubString(rest, pos4);
										newcmd.AppendInt(tmpint);
										newcmd.Append(rest);
										strcpy(it->second.cCommand, newcmd.GetStr());

										//Replace in update.
										std::map<std::int32_t, TextEditor::SymbolsValues>::iterator it2 = m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].find(it->first);
										if (it2 != m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].end()) {
											strcpy(it2->second.cCommand, newcmd.GetStr());
										}

										//Replace
										//newcmd ;global;myint;as;integer;=;6;
										uString replace, replacewith;
										if (newcmd.FindStr(";as;integer;=;", 1) > 0 ) {
											replace = var;
											replace.Append(" as integer = ");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as integer = ");
											replacewith.AppendInt(tmpint);
										}
										else if (newcmd.FindStr(";as;integer;=", 1) > 0) {
											replace = var;
											replace.Append(" as integer =");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as integer =");
											replacewith.AppendInt(tmpint);
										}
										else if (newcmd.FindStr(";as;integer=", 1) > 0) {
											replace = var;
											replace.Append(" as integer=");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as integer=");
											replacewith.AppendInt(tmpint);
										}
										else {
											if (pos3_length == 1) {
												replace = var;
												replace.Append("=");
												replace.Append(value);
												replacewith = var;
												replacewith.Append("=");
												replacewith.AppendInt(tmpint);
											}
											else if (pos3_length == 2) {
												replace = var;
												replace.Append(" =");
												replace.Append(value);
												replacewith = var;
												replacewith.Append(" =");
												replacewith.AppendInt(tmpint);
											}
											else {
												replace = var;
												replace.Append(" = ");
												replace.Append(value);
												replacewith = var;
												replacewith.Append(" = ");
												replacewith.AppendInt(tmpint);
											}
										}
										m_ActiveEditor->DirectReplace((char *)replace.GetStr(), (char *)replacewith.GetStr(), (int)it->second.lineno);

										if (debug_is_running && bSetVariableActive) {
											char szTmp[MAX_PATH + 80];
											sprintf(szTmp, "setvariable %s %d", var.GetStr(), tmpint);
											uString bc_err;
											BroadcasterCommand(szTmp,bc_err);
										}
										else if (debug_is_running && !updateWatchCalled && pref.bDebugBringToFront) {
											uString bc_err;
											BroadcasterCommand("pause",bc_err);
											BroadcasterCommand("continue",bc_err);
										}

									}
								}
								else if ( (iInPutType == 3 || iInPutType == 7 || iInPutType == 8) && value.GetLength() < MAX_PATH ) {
									if (iInPutType == 7 || iInPutType == 8) {
										ImGui::Text("%s:", msg.GetStr());
										ImGui::Indent(20);
										uniqueLabel = "##IDEGUIstringinput";
										uniqueLabel.AppendInt((int)it->second.lineno);
									}
									if (ImGui::InputText(uniqueLabel, &tmpchar[0],MAX_PATH)) {
										//Update value.
										cmd.SubString(newcmd, 0, pos3 + pos3_length);
										cmd.SubString(rest, pos4);
										newcmd.Append("\"");
										newcmd.Append(tmpchar);
										newcmd.Append("\"");
										newcmd.Append(rest);
										strcpy(it->second.cCommand, newcmd.GetStr());
										//Replace in update.
										std::map<std::int32_t, TextEditor::SymbolsValues>::iterator it2 = m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].find(it->first);
										if (it2 != m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].end()) {
											strcpy(it2->second.cCommand, newcmd.GetStr());
										}

										//Replace
										uString replace, replacewith;
										if (newcmd.FindStr(";as;string;=;", 1) > 0) {
											replace = var;
											replace.Append(" as string = ");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as string = ");
											replacewith.Append("\"");
											replacewith.Append(tmpchar);
											replacewith.Append("\"");
										}
										else if (newcmd.FindStr(";as;string;=", 1) > 0) {
											replace = var;
											replace.Append(" as string =");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as string =");
											replacewith.Append("\"");
											replacewith.Append(tmpchar);
											replacewith.Append("\"");
										}
										else if (newcmd.FindStr(";as;string=", 1) > 0) {
											replace = var;
											replace.Append(" as string=");
											replace.Append(value);
											replacewith = var;
											replacewith.Append(" as string=");
											replacewith.Append("\"");
											replacewith.Append(tmpchar);
											replacewith.Append("\"");
										}
										else {
											if (pos3_length == 1) {
												replace = var;
												replace.Append("=");
												replace.Append(value);
												replacewith = var;
												replacewith.Append("=");
												replacewith.Append("\"");
												replacewith.Append(tmpchar);
												replacewith.Append("\"");
											}
											else if (pos3_length == 2) {
												replace = var;
												replace.Append(" =");
												replace.Append(value);
												replacewith = var;
												replacewith.Append(" =");
												replacewith.Append("\"");
												replacewith.Append(tmpchar);
												replacewith.Append("\"");
											}
											else {
												replace = var;
												replace.Append(" = ");
												replace.Append(value);
												replacewith = var;
												replacewith.Append(" = ");
												replacewith.Append("\"");
												replacewith.Append(tmpchar);
												replacewith.Append("\"");
											}
										}
										m_ActiveEditor->DirectReplace((char *)replace.GetStr(), (char *)replacewith.GetStr(), (int)it->second.lineno);
										if (debug_is_running && bSetVariableActive) {
											char szTmp[MAX_PATH + 80];
											if(strlen(tmpchar) == 0)
												sprintf(szTmp, "setvariable %s \"\"", var.GetStr());
											else
												sprintf(szTmp, "setvariable %s %s", var.GetStr(), tmpchar);
											uString bc_err;
											BroadcasterCommand(szTmp,bc_err);
										}
										else if (debug_is_running && !updateWatchCalled && pref.bDebugBringToFront) {
											uString bc_err;
											BroadcasterCommand("pause",bc_err);
											BroadcasterCommand("continue",bc_err);
										}

									}
									if (iInPutType == 7 || iInPutType == 8) {
										ImGui::SameLine();
										uniqueLabel = "...##IDEGUIfileinput";
										uniqueLabel.AppendInt((int)it->second.lineno);

										if (ImGui::Button(uniqueLabel)) {
											char * cFileSelected;
											if(iInPutType == 8)
												cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_DIR, NULL, NULL, NULL);
											else
												cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, NULL, NULL, NULL);
											if (cFileSelected && strlen(cFileSelected) > 0) {
												//Update value.
												uString selectedname = cFileSelected;
												selectedname.ReplaceStr("\\", "/");
												strcpy(tmpchar,"raw:");
												strcat(tmpchar, selectedname.GetStr() );
												//Check if we are able to resolve it as a relative path.
												if (pCurrentSelectedProject) {
													uString sTmp = selectedname;
													sTmp.ReplaceStr("/", "\\");

													uString pfolder = ";:;";
													uString projectpath = pCurrentSelectedProject->m_sProjectFileFullPath;
													projectpath.ReplaceStr("\\", "/");
													int pos = projectpath.RevFindStr("/");
													if (pos > 0) {
														projectpath.SubString(pfolder, 0, pos + 1);
													}

													uString fname = sTmp;
													fname.ReplaceStr("\\", "/");

													uString relative = fname;

													relative.ReplaceStr(pfolder, "");
													uString szCompares = "";
													int countslash = pfolder.Count('/');
													int i, slashpos = 0;
													for (i = 0; i < countslash; i++) {
														slashpos = pfolder.FindStr("/", 1, slashpos);
														if (slashpos >= 0) {
															if (strncmp(pfolder.GetStr(), relative.GetStr(), slashpos) == 0) {
																pfolder.SubString(szCompares, 0, slashpos);
																slashpos++;
															}
															else {
																break;
															}
														}
														else {
															break;
														}
													}
													//i==0 Must be on another drive. so keep name as is.
													if (i > 0 && i < countslash) {
														//Make relative.
														int iRemainingSlash = countslash - i;
														szCompares.Append("/"); //Add missing /
														relative.ReplaceStr(szCompares, ""); // remove similar path.
														uString szFinal = "";
														for (int iAddBackPath = 0; iAddBackPath < iRemainingSlash; iAddBackPath++) {
															szFinal.Append("../");
														}
														szFinal.Append(relative);
														//szFinal.ReplaceStr("media/", "");
														//strcpy(tmpchar, "raw:");

														//AGK cant load from outside folder , so use raw:
														//strcpy(tmpchar, szFinal.GetStr());
													}
													else {
														//folder select always raw:
														if (iInPutType != 8) {
															relative.ReplaceStr("media/", "");
															strcpy(tmpchar, relative.GetStr());
														}
													}
												}

												if (iInPutType == 8) {
													if (tmpchar[strlen(tmpchar) - 1] != '/')
														strcat(tmpchar, "/");
												}
												cmd.SubString(newcmd, 0, pos3 + pos3_length);

												cmd.SubString(rest, pos4);
												newcmd.Append("\"");
												newcmd.Append(tmpchar);
												newcmd.Append("\"");
												newcmd.Append(rest);
												strcpy(it->second.cCommand, newcmd.GetStr());
												//Replace in update.
												std::map<std::int32_t, TextEditor::SymbolsValues>::iterator it2 = m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].find(it->first);
												if (it2 != m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].end()) {
													strcpy(it2->second.cCommand, newcmd.GetStr());
												}

												//Replace
												uString replace, replacewith;
												replace = var;
												if (newcmd.FindStr(";as;string;=;", 1) > 0) {
													replace.Append(" as string = ");
													replace.Append(value);
													replacewith = var;
													replacewith.Append(" as string = ");
												}
												else if (newcmd.FindStr(";as;string;=", 1) > 0) {
													replace.Append(" as string =");
													replace.Append(value);
													replacewith = var;
													replacewith.Append(" as string =");
												}
												else if (newcmd.FindStr(";as;string=", 1) > 0) {
													replace.Append(" as string=");
													replace.Append(value);
													replacewith = var;
													replacewith.Append(" as string=");
												}
												else if (pos3_length == 1) {
													replace.Append("=");
													replace.Append(value);
													replacewith = var;
													replacewith.Append("=");
												}
												else if (pos3_length == 2) {
													replace.Append(" =");
													replace.Append(value);
													replacewith = var;
													replacewith.Append(" =");
												}
												else {
													replace.Append(" = ");
													replace.Append(value);
													replacewith = var;
													replacewith.Append(" = ");
												}
												replacewith.Append("\"");
												replacewith.Append(tmpchar);
												replacewith.Append("\"");

												m_ActiveEditor->DirectReplace((char *)replace.GetStr(), (char *)replacewith.GetStr(), (int)it->second.lineno);
												if (debug_is_running && bSetVariableActive) {
													char szTmp[MAX_PATH + 80];
													sprintf(szTmp, "setvariable %s %s", var.GetStr(), tmpchar);
													uString bc_err;
													BroadcasterCommand(szTmp,bc_err);
												}
												else if (debug_is_running && !updateWatchCalled && pref.bDebugBringToFront) {
													uString bc_err;
													BroadcasterCommand("pause",bc_err);
													BroadcasterCommand("continue",bc_err);
												}

											}
										}
										ImGui::Indent(-20);
									}
								}
								else if (iInPutType == 4 && value.GetLength() < MAX_PATH) {

									if (ImGui::ColorEdit4(uniqueLabel, (float*)&tmpvec4, ImGuiColorEditFlags_AlphaPreview)) {
										bSetVec4 = true;
									}
								}
								else if (iInPutType == 5 && value.GetLength() < MAX_PATH) {
									ImGui::Text("%s:",msg.GetStr());
									ImGui::Indent(20);
									uniqueLabel = " X"; //var;
									uniqueLabel.Append("##IDEGUIinput");
									uniqueLabel.AppendInt((int)it->second.lineno);
									if (ImGui::InputFloat(uniqueLabel, (float*)&tmpvec4.x,0.01,0.1,6)) {
										bSetVec4 = true;
									}
									uniqueLabel = " Y"; //var;
									uniqueLabel.Append("##IDEGUIinput");
									uniqueLabel.AppendInt((int)it->second.lineno);
									if (ImGui::InputFloat(uniqueLabel, (float*)&tmpvec4.y, 0.01, 0.1, 6)) {
										bSetVec4 = true;
									}
									uniqueLabel = " Z"; //var;
									uniqueLabel.Append("##IDEGUIinput");
									uniqueLabel.AppendInt((int)it->second.lineno);
									if (ImGui::InputFloat(uniqueLabel, (float*)&tmpvec4.z, 0.01, 0.1, 6)) {
										bSetVec4 = true;
									}
									ImGui::Indent(-20);
								}
								else if (iInPutType == 6 && value.GetLength() < MAX_PATH) {

									ImGui::Text("%s:", msg.GetStr());
									ImGui::Indent(20);
									uniqueLabel = " X"; //var;
									uniqueLabel.Append("##IDEGUIinput");
									uniqueLabel.AppendInt((int)it->second.lineno);
									if (ImGui::InputFloat(uniqueLabel, (float*)&tmpvec4.x, 0.01, 0.1, 6)) {
										bSetVec4 = true;
									}
									uniqueLabel = " Y"; //var;
									uniqueLabel.Append("##IDEGUIinput");
									uniqueLabel.AppendInt((int)it->second.lineno);
									if (ImGui::InputFloat(uniqueLabel, (float*)&tmpvec4.y, 0.01, 0.1, 6)) {
										bSetVec4 = true;
									}
									uniqueLabel = " Z"; //var;
									uniqueLabel.Append("##IDEGUIinput");
									uniqueLabel.AppendInt((int)it->second.lineno);
									if (ImGui::InputFloat(uniqueLabel, (float*)&tmpvec4.z, 0.01, 0.1, 6)) {
										bSetVec4 = true;
									}
									uniqueLabel = " W"; //var;
									uniqueLabel.Append("##IDEGUIinput");
									uniqueLabel.AppendInt((int)it->second.lineno);
									if (ImGui::InputFloat(uniqueLabel, (float*)&tmpvec4.w, 0.01, 0.1, 6)) {
										bSetVec4 = true;
									}
									ImGui::Indent(-20);


								}

								if (bSetVec4) {
									//Update vec4
									for (std::map<std::int32_t, TextEditor::SymbolsValues>::iterator it2 = m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].begin(); it2 != m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].end(); ++it2)
									{
										char *found2;
										found2 = (char *)pestrcasestr(it2->second.cParameters, ",variable,");
										if (found2) {
											uString findVar;
											for (int lf = 0; lf < 4; lf++) {
												findVar = var;
												if (lf == 0)
													findVar.Append(".x;=;");
												else if (lf == 1)
													findVar.Append(".y;=;");
												else if (lf == 2)
													findVar.Append(".z;=;");
												else if (lf == 3)
													findVar.Append(".w;=;");

												if ((found2 = (char *)pestrcasestr(it2->second.cCommand, findVar.GetStr()))) {
													found2 += findVar.GetLength();
													uString value2 = found2;
													value2.ReplaceStr(";", "");
													value2.ReplaceStr(" ", "");
													value2.ReplaceStr("\"", "");

													uString replace, replacewith;
													replace = findVar;
													replace.Append(value2);
													replace.ReplaceStr(";", " ");

													uString replaceValue = "";
													if (lf == 0)
														replaceValue.AppendFloat(tmpvec4.x);
													else if (lf == 1)
														replaceValue.AppendFloat(tmpvec4.y);
													else if (lf == 2)
														replaceValue.AppendFloat(tmpvec4.z);
													else if (lf == 3)
														replaceValue.AppendFloat(tmpvec4.w);
													uString cmdReplace = it2->second.cCommand;
													cmdReplace.ReplaceStr(value2, replaceValue);
													strcpy(it2->second.cCommand, cmdReplace.GetStr());
													//Replace in update.
													std::map<std::int32_t, TextEditor::SymbolsValues>::iterator it3 = m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].find(it2->first);
													if (it3 != m_ActiveEditor->CodeProperties[m_ActiveEditor->updateCodePropertieslist].end()) {
														strcpy(it3->second.cCommand, cmdReplace.GetStr());
													}


													replacewith = findVar;
													replacewith.ReplaceStr(";", " ");
													replacewith.Append(replaceValue);
//													if (lf == 0)
//														replacewith.AppendFloat(tmpvec4.x);
//													else if (lf == 1)
//														replacewith.AppendFloat(tmpvec4.y);
//													else if (lf == 2)
//														replacewith.AppendFloat(tmpvec4.z);
//													else if (lf == 3)
//														replacewith.AppendFloat(tmpvec4.w);

													m_ActiveEditor->DirectReplace((char *)replace.GetStr(), (char *)replacewith.GetStr(), (int)it2->second.lineno);

													if (debug_is_running && bSetVariableActive) {
														uString szVar = "", szValue = "";
														findVar.SubString(szVar, 0, findVar.GetLength() - 3);
														if (lf == 0)
															szValue.AppendFloat(tmpvec4.x);
														else if (lf == 1)
															szValue.AppendFloat(tmpvec4.y);
														else if (lf == 2)
															szValue.AppendFloat(tmpvec4.z);
														else if (lf == 3)
															szValue.AppendFloat(tmpvec4.w);

														char szTmp[MAX_PATH + 80];
														if ( szValue.GetLength() > 0 && szVar.GetLength() > 0 ) {
															sprintf(szTmp, "setvariable %s %s", szVar.GetStr(), szValue.GetStr());
															uString bc_err;
															BroadcasterCommand(szTmp,bc_err);
														}
													}
													else if (debug_is_running && !updateWatchCalled && pref.bDebugBringToFront) {
														uString bc_err;
														BroadcasterCommand("pause",bc_err);
														BroadcasterCommand("continue",bc_err);
													}

												}
											}
										}
									}

								} //update vec4

								ImGui::PopItemWidth();
							}
						}
					}
				}
				//ImGui::Text("cmd: %s par: %s line: %d", it->second.cCommand, it->second.cParameters , it->second.lineno);
			}
		}
	}

	return;
}


char keystore_errormsg[MAX_PATH + 80];
char keystore_password1[MAX_PATH];
char keystore_password2[MAX_PATH];


//#### IOS Export Window ####
//###########################

/*
[ipa_settings]
app_icon_path=
app_name=
build_number=
device_type=0
facebook_id=
orientation=0
output_path=
prov_profile_path=
splash_logo=
splash_color=
uses_ads=0
version_number=
firebase_config_path=
url_scheme=
deep_link=
*/

int hextoint( unsigned char letter )
{
	if ( letter >= 48 && letter <= 57 ) return letter - 48;
	if ( letter >= 65 && letter <= 70 ) return letter - 55;
	if ( letter >= 97 && letter <= 102 ) return letter - 87;
	return 0;
}

char ios_error[1024];
void ProcessIOSExport(void)
{
	if (!show_iosexport_window)
		return;
	if (!pCurrentSelectedProject)
		return;

	struct TextFilters {
		static int TextFiltersAZ1(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", (char)data->EventChar)) return 0; return 1; }
		static int TextFiltersAZDOT(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_.0123456789", (char)data->EventChar)) return 0; return 1; }
		static int TextFiltersDIGITS(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("0123456789", (char)data->EventChar)) return 0; return 1; }
		static int TextFiltersVERSION(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("0123456789.", (char)data->EventChar)) return 0; return 1; }
	};

	int input_indent = 12 * ImGui::GetFontSize();
	int item_dot_width = -37;
	ImGuiWindowFlags window_flags = 0;
	ImGui::SetNextWindowSize(ImVec2(67*ImGui::GetFontSize(), 40 *ImGui::GetFontSize()), ImGuiCond_Once);
	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);;
	ImGui::Begin("IPA export.", &show_iosexport_window, window_flags);
	if (show_iosexport_window_state != show_iosexport_window) {
		show_iosexport_window_state = show_iosexport_window;
		ImGui::SetKeyboardFocusHere();
		strcpy(ios_error, "");
	}

	ImGui::Indent(8.0);
	ImGui::Spacing();

	ImVec4* style_colors = ImGui::GetStyle().Colors;
	ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
	ImGui::SetWindowFontScale(1.1);
	style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
	ImGui::TextWrapped("IPA Export:");
	style_colors[ImGuiCol_Text] = oldTextColor;
	ImGui::SetWindowFontScale(1.0);

	ImGui::Indent(10.0);

	ImGui::TextWrapped("This will build an IPA from your media and bytecode that will run on any iOS device running iOS 9 or greater. You must have the correct distribution certificate and corresponding private key in your keychain for the provisioning profile you are using. Use an AdHoc profile to run it on devices that have been added to your Apple Developer account, or a Store profile to submit the exported IPA to the AppStore. You will need to install the Transporter app to submit the IPA to iTunes Connect. To run an AdHoc IPA on your device, connect it to your Mac, open iTunes, and drag the IPA onto the iOS device in iTunes. Note that you must have XCode installed for this exporter to work. If you export the same app with a different splash screen logo then the old logo will continue to show until you uninstall the old version of the app, restart the device, and install the new version. This is due to iOS caching the logo image for fast app switching");


	ImGui::Columns(2, "myIPAexportDialog", false);  // 3-ways, no border

	ImGui::Indent(-10.0);
	ImGui::Text("App Settings:");
	ImGui::Indent(10.0);

	ImGui::Text("App Name:"); // max 30 A-Z 0-9 space underscore
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##apk_app_name", &pCurrentSelectedProject->ipa_app_name[0], 30, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersAZ1);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Only A-Z,0-9,spaces or underscore allowed.");

	ImGui::Text("App Icon:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##ipa_icon_path", &pCurrentSelectedProject->ipa_app_icon_path[0], 255);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Must be at least 1024x1024 pixels, PNG only, must not contain transparency.");
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##ipa_icon_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "png\0*.png\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->ipa_app_icon_path, cFileSelected);
		}
	}


	ImGui::Text("Provisioning Profile:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##ipa_prov_profile_path", &pCurrentSelectedProject->ipa_prov_profile_path[0], 255);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Provisioning profile downloaded from your Apple developer account containing the list of devices this build will work on (or the App Store).");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##ipa_prov_profile_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, NULL, NULL, NULL); //"\0*\0"
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->ipa_prov_profile_path, cFileSelected);
		}
	}



	ImGui::Text("Splash Screen Logo:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##ipa_splash_logo", &pCurrentSelectedProject->ipa_splash_logo[0], 255);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Should be a square PNG image of any size that will be centered on screen during start up. The logo will be scaled so it touches the edges of the screen but will not be stretched to fill the whole screen");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##ipa_splash_logo")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "png\0*.png\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->ipa_splash_logo, cFileSelected);
		}
	}


	ImGui::Text("Splash Screen Color:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##ipa_splash_color", &pCurrentSelectedProject->ipa_splash_color[0], 7);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The background color of the splash screen during startup, this will be used anywhere that the splash screen logo does not cover. Must be a six digit hex value where the first 2 digits represent red, the middle 2 for green, and the final 2 for blue, e.g. E86C2B");
		ImGui::EndTooltip();
	}


	ImGui::Text("Orientation:");
	const char* ipa_orintation_array[] = { "Landscape", "Portrait", "Both" };
	ImGui::SameLine();
	ImGui::PushItemWidth(-10);
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::Combo("##comboapk_orientation", &pCurrentSelectedProject->ipa_orientation, ipa_orintation_array, IM_ARRAYSIZE(ipa_orintation_array));
	ImGui::PopItemWidth();


	ImGui::Text("Version Name:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##ipa_version_number", &pCurrentSelectedProject->ipa_version_number[0], 30, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersVERSION);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("0-9 and . only, this must match the version number you use in iTunes Connect");
		ImGui::EndTooltip();
	}


	ImGui::Text("Build Number:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##ipa_build_number", &pCurrentSelectedProject->ipa_build_number[0], 30, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersVERSION);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("0-9 and . only");
		ImGui::EndTooltip();
	}



	ImGui::NextColumn();
	float startline = ImGui::GetCursorPos().x;


	ImGui::Text("Device Type:");
	const char* ipa_device_type_array[] = { "iPhone and iPad", "iPhone only", "iPad only" };
	ImGui::SameLine();
	ImGui::PushItemWidth(-10);
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::Combo("##comboapk_ipa_device_type", &pCurrentSelectedProject->ipa_device_type, ipa_device_type_array, IM_ARRAYSIZE(ipa_device_type_array));
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Choose what type of devices will be able to see this app on the AppStore");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();

	/*
	ImGui::Text("Facebook App ID:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##ipa_facebook_id", &pCurrentSelectedProject->ipa_facebook_id[0], 255, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersDIGITS);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("(optional) if you use the Facebook commands you must put the Facebook App ID here.");
		ImGui::EndTooltip();
	}
	*/



	ImGui::Text("Firebase Config File (.plist):");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##ipa_firebase_config_path", &pCurrentSelectedProject->ipa_firebase_config_path[0], 255);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("(Optional) The GoogleService-Info.plist file you downloaded when setting up your Firebase project, this is different for every app.");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##ipa_firebase_config_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "plist\0*.plist\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->ipa_firebase_config_path, cFileSelected);
		}
	}


	ImGui::Text("URL Scheme:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##ipa_url_scheme", &pCurrentSelectedProject->ipa_url_scheme[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The URL scheme that can be used to open your app from a browser link, e.g. a URL scheme of \"myapp\" would allow links to myapp://sometext to open your app with \"sometext\" being available to your app through GetURLSchemeText(). On iOS this must be unique to your app.");
		ImGui::EndTooltip();
	}


	ImGui::Text("Universal Link:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##ipa_deep_link", &pCurrentSelectedProject->ipa_deep_link[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("A UniversalLlink can be used to open your app from a URL link, e.g. \"https://www.appgamekit.com/app/\" will open the AGK Player. In this field you only need to enter the domain, e.g. \"www.appgamekit.com\" and Apple will fetch the valid links from the \"https://www.appgamekit.com/apple-app-site-association\" file.");
		ImGui::EndTooltip();
	}


	//advert
	ImGui::Text("Uses Adverts: ");
	ImGui::SameLine();
	//ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	bool bAdvertOn = pCurrentSelectedProject->ipa_uses_ads;
	ImGui::Checkbox("##ipa_uses_ads", &bAdvertOn);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Tick this box if your app uses the AdMob or Chartboost commands. This will include the Advertising Identifier (IDFA) in your exported app.");
		ImGui::EndTooltip();
	}
	if (bAdvertOn)
		pCurrentSelectedProject->ipa_uses_ads = 1;
	else
		pCurrentSelectedProject->ipa_uses_ads = 0;

	ImGui::Text("AdMob App ID:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##ipa_admob_app_id", &pCurrentSelectedProject->ipa_admob_app_id[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The AdMob App ID for this app, this can be found in the apps page of your AdMob account and should look like this \"ca-app-pub-3940256099942544~3347511713\". App IDs have the ~ character whereas Ad IDs have a forward slash");
		ImGui::EndTooltip();
	}

	ImGui::Text("SnapChat Client ID:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##ipa_snapchat_client_id", &pCurrentSelectedProject->ipa_snapchat_client_id[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The SnapChat Client ID provided by your SnapChat developer account for this app. Required if you use the ShareSnapChatImage command");
		ImGui::EndTooltip();
	}


	static bool ios_exporting_player = false;

	//advert
	//ImGui::Separator();
	ImGui::Text("");
	ImGui::Text("Export AGK Player: ");
	ImGui::SameLine();
	ImGui::Checkbox("##ios_exporting_player", &ios_exporting_player);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Check this if you want to generate the AGK Player App.");
		ImGui::EndTooltip();
	}

	//ios_leave_buildtmp
	
	static bool ios_leave_buildtmp = false;
	if (enable_debug_gadgets) {
		ImGui::Text("");
		ImGui::Text("Keep build_tmp After Export: ");
		ImGui::SameLine();
		ImGui::Checkbox("##ios_leave_buildtmp", &ios_leave_buildtmp);
		if (ImGui::IsItemHovered()) {
			ImGui::SetNextWindowContentSize(ImVec2(400, 0));
			ImGui::BeginTooltip();
			ImGui::TextWrapped("Check this to not remove build_tmp files after export.");
			ImGui::EndTooltip();
		}
	}

	static bool ios_ignore_push = false;
	if (enable_debug_gadgets) {
		ImGui::Text("");
		ImGui::Text("Ignore Push Notificationst: ");
		ImGui::SameLine();
		ImGui::Checkbox("##ios_ignore_push", &ios_ignore_push);
		if (ImGui::IsItemHovered()) {
			ImGui::SetNextWindowContentSize(ImVec2(400, 0));
			ImGui::BeginTooltip();
			ImGui::TextWrapped("Dont setup push notificationst permissions.");
			ImGui::EndTooltip();
		}
	}

	
	if (enable_debug_gadgets) {
		ImGui::Text("");
		ImGui::Text("Generate Log File: ");
		ImGui::SameLine();
		ImGui::Checkbox("##ios_generate_log", &ios_generate_log);
		if (ImGui::IsItemHovered()) {
			ImGui::SetNextWindowContentSize(ImVec2(400, 0));
			ImGui::BeginTooltip();
			ImGui::TextWrapped("Check this to generate a log file.");
			ImGui::EndTooltip();
		}
	}

	ImGui::NextColumn();

	ImGui::Indent(-10.0);
	ImGui::Text("Output:");
	ImGui::Indent(10.0);

	ImGui::Text("Output File Location:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##ipa_output_path", &pCurrentSelectedProject->ipa_output_path[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The location to save the final IPA file.");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##ipa_output_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "ipa\0*.ipa\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->ipa_output_path, cFileSelected);
		}
	}

	if (strlen(ios_error) > 0) {
		//
		ImGui::TextWrapped("Error: %s", ios_error);
	}

	ImGui::NextColumn();

	ImGui::Indent(-10.0);
	ImGui::Text("");
	ImVec2 ws = ImGui::GetWindowSize();
//	ImGui::SetCursorPos(ImVec2(ws.x - 140, ws.y - 26 + ImGui::GetScrollY()));
	if (ImGui::Button(" Cancel ")) {
		show_iosexport_window = false;
	}
	ImGui::SameLine();
	if (ImGui::Button(" Export "))
	{
		//Save new preferences before exporting.
		SaveProjectFile(pCurrentSelectedProject);

		PleaseWait();


		//apk_output_path
		char output_file[MAX_PATH];

		strcpy(output_file, pCurrentSelectedProject->ipa_output_path);
		strcpy(output_logfile, pCurrentSelectedProject->ipa_output_path);
		strcat(output_logfile, ".log");

		if (ios_generate_log) {
			if (file_exists(output_logfile)) {
				remove(output_logfile);
//				agk::DeleteFile(output_logfile);
			}
			cp_appendtofile(output_logfile, "LogFile:\n");
		}

		char *percent = 0;
		while ((percent = strchr(&output_file[0], '%')) != 0)
		{
			if (strncmp(percent + 1, "[version]", strlen("[version]")) == 0)
			{
				*percent = 0;
				percent += strlen("[version]") + 1;
				strcat(output_file, pCurrentSelectedProject->ipa_build_number);
				strcat(output_file, percent);
				continue;
			}
			break;
		}

		bool Valid = true;
		if (strlen(output_file) < 2) { strcpy(ios_error, "You must choose an output location to save your APK"); Valid = false; }
		if (Valid && strchr(output_file, '.') == NULL) { strcpy(ios_error, "The output location must be a file not a directory"); Valid = false; }
		if (Valid && strlen(pCurrentSelectedProject->ipa_app_name) < 1) { strcpy(ios_error, "You must enter an app name"); Valid = false; }

		if (file_exists(output_file)) {
			Valid = overWriteFileBox((char*) &output_file[0]);
		}

		if (Valid) {

			if (Valid && strlen(pCurrentSelectedProject->ipa_app_icon_path) > 0)
			{
				if (pestrcasestr(pCurrentSelectedProject->ipa_app_icon_path, ".png") == NULL) {
					strcpy(ios_error, "App icon must be a PNG file");
					Valid = false;
				}
				if (Valid && !file_exists(pCurrentSelectedProject->ipa_app_icon_path)) {
					strcpy(ios_error, "Could not find app icon location");
					Valid = false;
				}
			}

			if (Valid && strlen(pCurrentSelectedProject->ipa_firebase_config_path) > 0)
			{

				if (Valid && pestrcasestr(pCurrentSelectedProject->ipa_firebase_config_path, ".plist") == NULL) {
					strcpy(ios_error, "Firebase config file must be a .plist file");
					Valid = false;
				}
				if (Valid && !file_exists(pCurrentSelectedProject->ipa_firebase_config_path)) {
					strcpy(ios_error, "Could not find Firebase config file");
					Valid = false;
				}
			}

			if (Valid && strlen(pCurrentSelectedProject->ipa_splash_logo) > 0)
			{
				if (pestrcasestr(pCurrentSelectedProject->ipa_splash_logo, ".png") == NULL) {
					strcpy(ios_error, "Splash Screen Logo must be a PNG file");
					Valid = false;
				}
				if (Valid && !file_exists(pCurrentSelectedProject->ipa_splash_logo)) {
					strcpy(ios_error, "Could not find Splash Screen Logo location");
					Valid = false;
				}
			}

			if (Valid && strlen(pCurrentSelectedProject->ipa_splash_color) > 0)
			{
				if ( strlen( pCurrentSelectedProject->ipa_splash_color ) != 6 ) 
				{ 
					strcpy(ios_error, "Splash Screen Color must be a 6 digit hex value, e.g. E86C2B");
					Valid = false;
				}
				else
				{
					char* str = pCurrentSelectedProject->ipa_splash_color;
					for( int i = 0; i < strlen( str ); i++ )
					{
						if ( str[i] >= 97 && str[i] <= 102 ) str[i] -= 32; // change to upper case
						if ( (str[i] < 48 || str[i] > 57) && (str[i] < 65 || str[i] > 70) ) 
						{ 
							strcpy(ios_error, "Splash Screen Color must be a 6 digit hex value, e.g. E86C2B");
							Valid = false;
						}
					}
				}
			}

			if (Valid) {
				if (Valid && strlen(pCurrentSelectedProject->ipa_prov_profile_path) <= 1) {
					strcpy(ios_error, "You must select a provisioning profile");
					Valid = false;
				}
				if (Valid && !file_exists(pCurrentSelectedProject->ipa_prov_profile_path)) {
					strcpy(ios_error, "Could not find provisioning profile location");
					Valid = false;
				}
				if (pestrcasestr(pCurrentSelectedProject->ipa_prov_profile_path, ".mobileprovision") == NULL) {
					strcpy(ios_error, "Provisioning profile must have .mobileprovision extension");
					Valid = false;
				}

			}

			if (Valid && strlen(pCurrentSelectedProject->ipa_version_number) <= 1) {
				strcpy(ios_error, "You must enter a version number, e.g. 1.0.0");
				Valid = false;
			}



			if (Valid && strlen(pCurrentSelectedProject->ipa_url_scheme) > 0) {
				if (strchr(pCurrentSelectedProject->ipa_url_scheme, ':') || strchr(pCurrentSelectedProject->ipa_url_scheme, '/'))
				{
					strcpy(ios_error, "URL scheme must not contain : or /");
					Valid = false;
				}
			}

			if (Valid && strlen(pCurrentSelectedProject->ipa_deep_link) > 0)
			{
				if (strchr(pCurrentSelectedProject->ipa_deep_link, '.') == 0 )
				{
					strcpy(ios_error, "Universal link must be a domain, e.g. www.appgamekit.com");
					Valid = false;
				}
			}

			if (Valid && strlen(pCurrentSelectedProject->ipa_admob_app_id) > 0)
			{
				if (strchr(pCurrentSelectedProject->ipa_admob_app_id, '~') == 0 )
				{
					strcpy(ios_error, "AdMob App ID must have a ~ character");
					Valid = false;
				}
			}

#ifndef AGK_WINDOWS
			if (Valid) {
				if (!file_exists(pCurrentSelectedProject->ipa_prov_profile_path)) {
					strcpy(ios_error, "As of iOS 11 you must install XCode to export iOS apps from the AGK IDE. XCode can be downloaded from the Mac AppStore.");
					Valid = false;
				}
			}
#endif


			if (Valid) {
				//Start export.
				char* path_to_codesign = strdup("/usr/bin/codesign");
				char* path_to_security = strdup("/usr/bin/security");
				char* path_to_actool = strdup("/Applications/XCode.app/Contents/Developer/usr/bin/actool");


				char curDir[MAX_PATH];
				extern char startupFolder[MAX_PATH];

#ifdef AGK_WINDOWS
				_getcwd(&curDir[0], MAX_PATH);
				_chdir(startupFolder);
#else
				getcwd(&curDir[0], MAX_PATH);
				chdir(startupFolder);
#endif
				// make temporary folder
				extern char defaultWriteFolder[MAX_PATH];
				uString tmp_folder = defaultWriteFolder;
				tmp_folder.Append("build_tmp");

				uString ios_folder = startupFolder;
				ios_folder.Append("/media/data/ios/");

				uString app_datadir = startupFolder;
				app_datadir.Append("/media/data/");

				uString app_folder = tmp_folder;
				app_folder.Append("/");
				app_folder.Append(pCurrentSelectedProject->ipa_app_name);
				app_folder.Append(".app");

				uString app_folder_name = pCurrentSelectedProject->ipa_app_name;
				app_folder_name.Append(".app");

				ios_folder.ReplaceStr("\\", "/");
				tmp_folder.ReplaceStr("\\", "/");

				uString src_folder = app_datadir;
				src_folder.Append("ios/source/AppGameKit Player.app");
				src_folder.ReplaceStr("\\", "/");

				uString no_ads_binary = app_datadir;
				no_ads_binary.Append("ios/source/AppGameKit Player No Ads");
				no_ads_binary.ReplaceStr("\\", "/");

				uString icons_src_folder = app_datadir;
				icons_src_folder.Append("ios/source/Icons.xcassets");
				icons_src_folder.ReplaceStr("\\", "/");

				uString icons_dst_folder = tmp_folder;
				icons_dst_folder.Append("/Icons.xcassets");
				icons_dst_folder.ReplaceStr("\\", "/");

				uString icons_sub_folder = tmp_folder;
				icons_sub_folder.Append("/Icons.xcassets/AppIcon.appiconset");
				icons_sub_folder.ReplaceStr("\\", "/");

				char output_file_zip[MAX_PATH];
				strcpy(output_file_zip, output_file);
				char *ext = strrchr(&output_file_zip[0], '.');
				if (ext)
					*ext = 0;
				strcat(output_file_zip, ".zip");


				// declarations.
				char newcontents[32000];
				char *contents = NULL;
				int  length = 0;
				char *certificate_data = NULL;
				char *bundle_id = NULL;
				char *app_group_data = 0;
				char *team_id = NULL;
				char *cert_hash = NULL;
				char *cert_temp = NULL;
				char **argv = NULL;
				char *str_out = NULL;
				int status = 0;
				//Error *error = NULL;
				char *entitlements_file = NULL;
				char *expanded_entitlements_file = NULL;
				char *temp_filename1 = NULL;
				char *temp_filename2 = NULL;
				char *version_string = NULL;
				char *build_string = NULL;
				char *bundle_id2 = NULL; // don't free, pointer to sub string
				char *image_filename = NULL;
				char *temp_image_filename = NULL;
				//GdkPixbuf *icon_scaled_image = NULL;
				//GdkPixbuf *icon_image = NULL;
				//GdkPixbuf *splash_image = NULL;
				char *user_name = NULL;
				char *group_name = NULL;

				//mz_zip_archive zip_archive;
				//memset(&zip_archive, 0, sizeof(zip_archive));

				//Delete all content of build_tmp if exists.
				utils_remove_folder_recursive(tmp_folder.GetStr());

#ifdef AGK_WINDOWS
				_mkdir(tmp_folder.GetStr());
#else
				mkdir(tmp_folder.GetStr(), S_IRWXU);
#endif
				if (ios_generate_log) {
					sprintf(output_debug, "mkdir(%s)\n", tmp_folder.GetStr());
					cp_appendtofile(output_logfile, output_debug);
				}

				if (!CopyDirTo(src_folder.GetStr(), app_folder.GetStr()))
				{
					strcpy(ios_error, "Failed to copy source folder");
					Valid = false;
				}

				if (Valid && !pCurrentSelectedProject->ipa_uses_ads)
				{
					uString binary_path = app_folder;
					binary_path.Append("/AppGameKit Player");
					if (!cp_copyfile((char *)no_ads_binary.GetStr(), (char *)binary_path.GetStr())) {
						if (ios_generate_log) {
							sprintf(output_debug, "copy failed: %s,%s\n", no_ads_binary.GetStr(), binary_path.GetStr());
							cp_appendtofile(output_logfile, output_debug);
						}
						strcpy(ios_error, "Failed to copy /AppGameKit Player");
						Valid = false;
					}
				}

#ifdef AGK_MACOS
				if (Valid) {
					// rename executable
					chdir(app_folder);
					rename("AppGameKit Player", pCurrentSelectedProject->ipa_app_name);
					chmod( pCurrentSelectedProject->ipa_app_name, 0775 );
				}
#endif

				if (Valid) {
					// open provisioning profile and extract certificate
					if (!(contents = g_file_get_contents((char *)&pCurrentSelectedProject->ipa_prov_profile_path[0], contents, &length, NULL)))
					{
						strcpy(ios_error, "Failed to read provisioning profile");
						Valid = false;
					}
				}

				char* certificate;
				char* certificate_end;
				if (Valid) {
					// provisioning profile starts as binary, so skip 100 bytes to get to text
					certificate = strstr(contents + 100, "<key>DeveloperCertificates</key>");
					if (!certificate)
					{
						strcpy(ios_error, "Failed to read certificate from provisioning profile");
						Valid = false;
					}

					if (Valid) {
						certificate = strstr(certificate, "<data>");
						if (!certificate)
						{
							strcpy(ios_error, "Failed to read certificate data from provisioning profile");
							Valid = false;
						}
					}
					if (Valid) {
						certificate += strlen("<data>");
						certificate_end = strstr(certificate, "</data>");
						if (!certificate_end)
						{
							strcpy(ios_error, "Failed to read certificate end data from provisioning profile");
							Valid = false;
						}
					}
				}
				int pushNotifications = 0;
				int betaReports = 0;
				int cert_length;

				if (Valid) {


					// copy certificate into local storage
					cert_length = (int)(certificate_end - certificate);
					certificate_data = new char[cert_length + 1];
					strncpy(certificate_data, certificate, cert_length);
					certificate_data[cert_length] = 0;
					int d = 0; //utils_str_remove_chars(certificate_data, "\n\r");
					for (int i = 0; i < cert_length;i++) {
						if (certificate_data[i] != '\n' && certificate_data[i] != '\r')
							certificate_data[d++] = certificate_data[i];
					}
					certificate_data[d] = 0;

					// extract bundle ID, reuse variables
					certificate = strstr(contents + 100, "<key>application-identifier</key>");
					if (!certificate)
					{
						strcpy(ios_error, "Failed to read bundle ID from provisioning profile");
						Valid = false;
					}

					if (Valid) {
						certificate = strstr(certificate, "<string>");
						if (!certificate)
						{
							strcpy(ios_error, "Failed to read bundle ID data from provisioning profile");
							Valid = false;
						}
					}

					if (Valid) {
						certificate += strlen("<string>");
						certificate_end = strstr(certificate, "</string>");
						if (!certificate_end)
						{
							strcpy(ios_error, "Failed to read bundle ID end data from provisioning profile");
							Valid = false;
						}
					}

					if (Valid) {
						// copy bundle ID to local storage
						cert_length = (int)(certificate_end - certificate);
						bundle_id = new char[cert_length + 1];
						strncpy(bundle_id, certificate, cert_length);
						bundle_id[cert_length] = 0;

						if (ios_generate_log) {
							cp_appendtofile(output_logfile, "bundle_id: (");
							cp_appendtofile(output_logfile, bundle_id );
							cp_appendtofile(output_logfile, ")\n");
						}


						// look for beta entitlement
						if (strstr(contents + 100, "<key>beta-reports-active</key>") != 0)
						{
							betaReports = 1;
						}

						// look for push notification entitlement
						char* pushStr = strstr(contents + 100, "<key>aps-environment</key>");
						if (pushStr != 0)
						{
							char* pushType = strstr(pushStr, "<string>");
							if (strncmp(pushType, "<string>development</string>", strlen("<string>development</string>")) == 0)
								pushNotifications = 1;
							else
								pushNotifications = 2;
						}
					}

					if (Valid) {
						// look for app groups
						certificate = strstr(contents + 100, "<key>com.apple.security.application-groups</key>");
						if (certificate)
						{
							certificate = strstr(certificate, "<array>");
							if (!certificate)
							{
								strcpy(ios_error, "Failed to read App Group data from provisioning profile");
								Valid = false;
							}
							if (Valid) {
								app_group_data = strstr(certificate, "</array>");
								if (!app_group_data)
								{
									strcpy(ios_error, "Failed to read App Group end data from provisioning profile");
									Valid = false;
								}
							}

							if (Valid) {
								// quick hack to prevent next search going beyond the array list
								*app_group_data = 0;

								// check there is at least one string
								certificate_end = strstr(certificate, "<string>");

								// repair the string
								*app_group_data = '<';
								app_group_data = 0;

								if (certificate_end)
								{
									// find the end of the list
									certificate_end = strstr(certificate, "</array>");
									if (!certificate_end)
									{
										strcpy(ios_error, "Failed to read App Group end data from provisioning profile");
										Valid = false;
									}
									if (Valid) {
										certificate_end += strlen("</array>");

										// copy App Group strings to local storage
										cert_length = (int)(certificate_end - certificate);
										app_group_data = new char[cert_length + 1];
										strncpy(app_group_data, certificate, cert_length);
										app_group_data[cert_length] = 0;
									}
								}
							}
						}
					}

				}
				int cloudKit = 0;
				if (Valid) {

					//#### HERE NOW ####

					// look for cloud kit
					if (strstr(contents + 100, "<key>com.apple.developer.ubiquity-kvstore-identifier</key>") != 0)
					{
						cloudKit = 1;
					}

					// extract team ID, reuse variables
					certificate = strstr(contents + 100, "<key>com.apple.developer.team-identifier</key>");
					if (!certificate)
					{
						certificate = strstr(contents + 100, "<key>TeamIdentifier</key>");
						if (!certificate)
						{
							strcpy(ios_error, "Failed to read team ID from provisioning profile");
							Valid = false;
						}
					}
					if (Valid) {
						certificate = strstr(certificate, "<string>");
						if (!certificate)
						{
							strcpy(ios_error, "Failed to read team ID data from provisioning profile");
							Valid = false;
						}
					}
					if (Valid) {
						certificate += strlen("<string>");
						certificate_end = strstr(certificate, "</string>");
						if (!certificate_end)
						{
							strcpy(ios_error, "Failed to read team ID end data from provisioning profile");
							Valid = false;
						}
					}

					if (Valid) {
						// copy team ID to local storage
						cert_length = (int)(certificate_end - certificate);
						team_id = new char[cert_length + 1];
						strncpy(team_id, certificate, cert_length);
						team_id[cert_length] = 0;

						if (strncmp(team_id, bundle_id, strlen(team_id)) == 0)
						{
							// remove team ID
							bundle_id2 = strchr(bundle_id, '.');
							if (bundle_id2) bundle_id2++;
							else bundle_id2 = bundle_id;
						}
						else
						{
							bundle_id2 = bundle_id;
						}
					}

				}
				#define CERT_DATA_LEN 262144
				char tmp[CERT_DATA_LEN];
				str_out = &tmp[0];

				if (Valid) {

					uString sParameters = "", sCmd = "";

					// find all certificates, the identity is just the hash of the certificate
					sCmd = path_to_security;
					sParameters.Append("find-certificate -a -p -Z"); // certificate not guaranteed to have "iPhone" in the name

					static int iStdOutLength, iStdErrLength;
					int appid = run_command(path_to_security, sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", path_to_security, sParameters.GetStr() );
						cp_appendtofile(output_logfile, output_debug);
					}


					if (appid == false) {
						strcpy(ios_error, "Failed to start security program.");
						Valid = false;
					}
					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], CERT_DATA_LEN - 90, NULL, 0);
							tmp[512] = 0;
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								//cp_appendtofile(output_logfile, tmp); // this will overflow the output_logfile string
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
							sprintf(ios_error, "Timeout security program, (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						if (Valid) {
							//Anything returned ?
							///usr/bin/security find-certificate -a -p -Z
							//Return nothing if none exists.

							int rs = read_buffers(&tmp[0], CERT_DATA_LEN - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								//cp_appendtofile(output_logfile, tmp); // this will overflow the output_logfile string
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}

							if (strstr(tmp, "SHA-1")) {
								bAaptDone = true;
							}
						}
						if (!bAaptDone) {
							tmp[512] = 0;
							//Just display for now!
							sprintf(ios_error, "Failed to get code signing identities, did you add a distribution certificate to your keychain? (output: %s)", tmp);
							Valid = false;
						}
					}
				}
				char *sha;
				if (Valid) {
					// cycle through each certificate looking for one that matches provisioning profile
					sha = strstr(str_out, "SHA-1 hash: ");
					while (sha)
					{
						sha += strlen("SHA-1 hash: ");
						char *sha_end = strchr(sha, '\n');
						if (!sha_end)
						{
							strcpy(ios_error, "Failed to read code signing identity from certificate list");
							Valid = false;
							break;
						}

						int length = (int)(sha_end - sha);

						// save hash for later, if this is the correct certificate then this will be the codesigning identity
						if (cert_hash) free(cert_hash);
						cert_hash = new char [length + 1];
						strncpy(cert_hash, sha, length);
						cert_hash[length] = 0;

						sha = sha_end + 1;
						sha = strstr(sha, "-----BEGIN CERTIFICATE-----");
						if (!sha)
						{
							strcpy(ios_error, "Failed to read certificate data from certificate list");
							Valid = false;
							break;
						}

						sha += strlen("-----BEGIN CERTIFICATE-----") + 1;
						sha_end = strstr(sha, "-----END CERTIFICATE-----");
						if (!sha_end)
						{
							strcpy(ios_error, "Failed to read certificate end data from certificate list");
							Valid = false;
							break;
						}

						length = (int)(sha_end - sha);

						// copy certificate to temp variable and check it
						if (cert_temp) free(cert_temp);
						cert_temp = new char[length + 1];
						strncpy(cert_temp, sha, length);
						cert_temp[length] = 0;

						// remove new line characters
						int d = 0; //utils_str_remove_chars(cert_temp, "\n\r");
						for (int i = 0; i < length;i++) {
							if (cert_temp[i] != '\n' && cert_temp[i] != '\r')
								cert_temp[d++] = cert_temp[i];
						}
						cert_temp[d] = 0;

						if (strcmp(cert_temp, certificate_data) == 0) break; // we found the certificate

						if (cert_hash) free(cert_hash);
						cert_hash = 0;

						// look for next certificate
						sha = sha_end + 1;
						sha = strstr(sha, "SHA-1 hash: ");
					}
					if (Valid) {
						if (!cert_hash)
						{
							strcpy(ios_error, "Could not find the certificate used to create the provisioning profile, have you added the certificate to your keychain?");
							Valid = false;
						}
					}

				}


				if (Valid) {

					uString sParameters = "", sCmd = "";

					// find all certificates, the identity is just the hash of the certificate
					sCmd = path_to_security;
					sParameters.Append("find-identity -p codesigning -v");


					static int iStdOutLength, iStdErrLength;
					int appid = run_command(path_to_security, sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", path_to_security, sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run security program.");
						Valid = false;
					}
					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], CERT_DATA_LEN - 90, NULL, 0);
							tmp[512] = 0;
							sprintf(ios_error, "Timeout security program, (output: %s)", tmp);
							Valid = false;
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								//cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}

							agk::TerminateApp(appid);
						}
						if (Valid) {
							//Anything returned ?
							///usr/bin/security find-certificate -a -p -Z
							//Return nothing if none exists.

							int rs = read_buffers(&tmp[0], CERT_DATA_LEN - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								//cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}

							if( strncmp(str_out, "  1) ", strlen("  1) ")) == 0) {
								bAaptDone = true;
							}
						}
						if (!bAaptDone) {
							tmp[512] = 0;
							//Just display for now!
							sprintf(ios_error, "Failed to get code signing identities (output: %s)", tmp);
							Valid = false;
						}

						if (Valid) {
							// parse identities, look for the identity we found earlier
							if (strstr(str_out, cert_hash) == 0)
							{
								strcpy(ios_error, "Signing certificate is not valid, either the private key is missing from your keychain, or the certificate has expired.");
								Valid = false;
							}

						}
					}
				}


				if (Valid) {

					// write entitlements file
					strcpy(newcontents, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n\
<plist version=\"1.0\">\n<dict>\n	<key>application-identifier</key>\n	<string>");
					strcat(newcontents, bundle_id);
					strcat(newcontents, "</string>\n	<key>com.apple.developer.team-identifier</key>\n	<string>");
					strcat(newcontents, team_id);
					strcat(newcontents, "</string>\n");

				

					if (betaReports)
						strcat(newcontents, "	<key>beta-reports-active</key>\n	<true/>\n");

					//Debug try force production.
					//strcat(newcontents, "	<key>aps-environment</key>\n	<string>production</string>\n");

					if (!ios_ignore_push) {
						if (pushNotifications == 1)
							strcat(newcontents, "	<key>aps-environment</key>\n	<string>development</string>\n");
						else if (pushNotifications == 2)
							strcat(newcontents, "	<key>aps-environment</key>\n	<string>production</string>\n");
					}

					strcat(newcontents, "	<key>get-task-allow</key>\n	<false/>\n");

					if (app_group_data)
					{
						strcat(newcontents, "	<key>com.apple.security.application-groups</key>\n");
						strcat(newcontents, app_group_data);
						strcat(newcontents, "\n");
					}

					if (cloudKit)
					{
						strcat(newcontents, "  <key>com.apple.developer.icloud-container-identifiers</key>\n	<array/>");
						strcat(newcontents, "  <key>com.apple.developer.ubiquity-kvstore-identifier</key>\n	<string>");
						strcat(newcontents, bundle_id);
						strcat(newcontents, "</string>\n");
					}

					if ( strlen(pCurrentSelectedProject->ipa_deep_link) > 0 )
					{
						char *szDomain = strstr(pCurrentSelectedProject->ipa_deep_link, "://");
						if (szDomain) szDomain += 3;
						else szDomain = &pCurrentSelectedProject->ipa_deep_link[0];

						char *szSlash = strchr(szDomain, '/');
						if (szSlash) *szSlash = 0;
						strcat(newcontents, "  <key>com.apple.developer.associated-domains</key>\n <array>\n  <string>applinks:");
						strcat(newcontents, szDomain);
						strcat(newcontents, "</string>\n</array>\n");
					}

					strcat(newcontents, "</dict>\n</plist>");

					uString ent_file = tmp_folder;
					ent_file.Append("/entitlements.xcent");

					// write entitlements file
					FILE* entfile = fopen(ent_file, "wb+");
					if (!entfile)
						entfile = AGKfopen(ent_file, "wb+");

					if (entfile) {
						fwrite(newcontents, 1, strlen(newcontents), entfile);
						fclose(entfile);

						if (ios_generate_log) {
							cp_appendtofile(output_logfile, "entitlements.xcent start:\n---\n");
							cp_appendtofile(output_logfile, newcontents);
							cp_appendtofile(output_logfile, "\n---\nentitlements.xcent end:\n");
						}

					}
					else {
						strcpy(ios_error, "Failed to write entitlements file. entitlements.xcent");
						Valid = false;
					}

				}


				if (Valid) {

					// write archived expanded entitlements file
					strcpy(newcontents, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n\
<plist version=\"1.0\">\n<dict>\n	<key>application-identifier</key>\n	<string>");
					strcat(newcontents, bundle_id);
					strcat(newcontents, "</string>\n");

					if (app_group_data)
					{
						strcat(newcontents, "	<key>com.apple.security.application-groups</key>\n");
						strcat(newcontents, app_group_data);
						strcat(newcontents, "\n");
					}

					strcat(newcontents, "</dict>\n</plist>");


					uString ent_file = app_folder;
					ent_file.Append("/archived-expanded-entitlements.xcent");

					// write expanded entitlements file
					FILE* entfile = fopen(ent_file, "wb+");
					if (!entfile)
						entfile = AGKfopen(ent_file, "wb+");

					if (entfile) {
						fwrite(newcontents, 1, strlen(newcontents), entfile);
						fclose(entfile);
					}
					else {
						strcpy(ios_error, "Failed to write expanded entitlements file. archived-expanded-entitlements.xcent");
						Valid = false;
					}
				}

				uString dest_file;
				if (Valid) {
				
					// copy Firebase config file
					if ( strlen(pCurrentSelectedProject->ipa_firebase_config_path) > 0 )
					{
						dest_file = app_folder;
						dest_file.Append("/GoogleService-Info.plist");
						cp_copyfile(pCurrentSelectedProject->ipa_firebase_config_path, (char *) dest_file.GetStr());
					}

					// copy provisioning profile
					dest_file = app_folder;
					dest_file.Append("/embedded.mobileprovision");

					cp_copyfile(pCurrentSelectedProject->ipa_prov_profile_path , (char *)dest_file.GetStr());

					// edit Info.plist
					dest_file = app_folder;
					dest_file.Append("/Info.plist");

					if (contents) free(contents);
					contents = 0;
					if (!(contents = g_file_get_contents( (char *) dest_file.GetStr() , contents, &length, NULL)))
					{
						strcpy(ios_error, "Failed to read Info.plist file");
						Valid = false;
					}

					uString uScontents = contents;

					// mike - quick test to see if this solves export problems on iOS
					if ( pCurrentSelectedProject->ipa_uses_ads )
					{

						uString advert = "<key>NSUserTrackingUsageDescription</key>\n";

						advert += "<string>This identifier will be used to deliver personalized ads to you.</string>\n";

						advert += "<key>SKAdNetworkItems</key>\n";
						advert += "<array>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>2u9pt9hc89.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>44jx6755aq.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>4fzdc2evr5.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>4pfyvq9l8r.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>737z793b9f.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>7ug5zh24hu.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>8s468mfl3y.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>9rd848q2bz.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>9t245vhmpl.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>av6w8kgt66.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>c6k4g5qg8m.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>cj5566h2ga.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>F38H382JLK.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>glqzh8vgby.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>hs6bdukanm.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>mlmmfzh3r3.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>prcb7njmu6.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>t38b2kh725.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>tl55sbb4fm.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>w9q455wk68.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>wg4vff78zm.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>yclnxrl5pm.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>ydx93a7ass.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>s39g8k73mm.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>m8dbw4sv7c.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>4468km3ulz.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>8s468mfl3y.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>lr83yxwka7.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>9nlqeag3gk.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>mls7yz5dvl.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>p78axxw29g.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>v72qych5uu.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>6xzpu9s2p8.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>g28c52eehv.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>523jb4fst2.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>ggvn48r87g.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>3sh42y64q3.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>24t9a8vw3c.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>ejvt5qm6ak.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>cstr6suwn9.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>wzmmz9fp6w.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>5lm9lj6jb7.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>zmvfpc5aq8.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>7rz58n8ntl.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>feyaarzu9v.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>n9x2a789qt.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>5a6flpkh64.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>424m5254lk.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>5l3tpt7t6e.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>uw77j35x4d.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>gta9lk7p23.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>kbd757ywx3.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>cg4yq2srnc.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>ludvb6z3bs.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>22mmun2rn5.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>su67r6k2v3.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>4w7y6s5ca2.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>klf5c3l5u5.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>dzg6xy7pwj.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>y45688jllp.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>hdw39hrw9y.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>ppxm28t8ap.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>4dzt52r2t5.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>mtkv5xtk9e.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>5tjdwbrq8w.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>3rd42ekr43.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>klf5c3l5u5.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>x44k69ngh6.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>pwa73g5rt2.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>mp6xlyr22a.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>qqp299437r.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>xy9t38ct57.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>54nzkqm89y.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>9b89h5y424.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>32z4fx6l9h.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>79pbpufp6p.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>275upjj5gd.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>294l99pt4k.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>2fnua5tdw4.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>32z4fx6l9h.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>3qcr597p9d.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>44n7hlldy6.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>54nzkqm89y.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>578prtvx9j.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>6g9af3uyq4.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>6p4ks3rnbw.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>79pbpufp6p.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>97r2b46745.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>9b89h5y424.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>e5fvkxwrpn.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>f73kdq92p3.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>g2y4y55b64.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>k674qkevps.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>mp6xlyr22a.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>n6fk4nfna4.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>qqp299437r.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>rx5hdcabgc.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>s69wq72ugq.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>u679fj5vs4.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>xy9t38ct57.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>kbmxgpxpgc.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "<dict>\n";
						advert += "<key>SKAdNetworkIdentifier</key>\n";
						advert += "<string>rvh3l7un93.skadnetwork</string>\n";
						advert += "</dict>\n";
						advert += "</array>\n";
							
						uScontents.ReplaceStr ( "<key>${ADVERT_ITEMS}</key><string></string>", advert );
						//uScontents.ReplaceStr ( "${ADVERT_ITEMS}", advert );
					}
					//

					uScontents.ReplaceStr("${PRODUCT_NAME}", pCurrentSelectedProject->ipa_app_name);
					uScontents.ReplaceStr("${EXECUTABLE_NAME}", pCurrentSelectedProject->ipa_app_name);
					if ( *pCurrentSelectedProject->ipa_admob_app_id ) uScontents.ReplaceStr("${ADMOB_APP_ID}", pCurrentSelectedProject->ipa_admob_app_id);
					else uScontents.ReplaceStr("${ADMOB_APP_ID}", "ca-app-pub-3940256099942544~1458002511" ); // needs something, use TGC Admob value
					if ( *pCurrentSelectedProject->ipa_snapchat_client_id ) uScontents.ReplaceStr("${SNAPCHAT_ID}", pCurrentSelectedProject->ipa_snapchat_client_id);

					uScontents.ReplaceStr("com.thegamecreators.agk2player", bundle_id2);

					if( strlen(pCurrentSelectedProject->ipa_facebook_id) > 0 )
						uScontents.ReplaceStr("358083327620324", pCurrentSelectedProject->ipa_facebook_id);

					const char* urlschemereplacement = "<string>${URLSCHEMES}</string>\n";
					if (strstr(contents, urlschemereplacement) == 0) urlschemereplacement = "<string>${URLSCHEMES}</string>\r\n";
					if (strlen(pCurrentSelectedProject->ipa_url_scheme) > 0 )
					{
						char* newUrlSchemes = new char[strlen(pCurrentSelectedProject->ipa_url_scheme) + 30];
						strcpy(newUrlSchemes, "<string>");
						strcat(newUrlSchemes, pCurrentSelectedProject->ipa_url_scheme);
						strcat(newUrlSchemes, "</string>\n");

						uScontents.ReplaceStr(urlschemereplacement, newUrlSchemes);
						free(newUrlSchemes);
					}
					else
					{
						uScontents.ReplaceStr(urlschemereplacement, "");
					}

					switch (pCurrentSelectedProject->ipa_orientation)
					{
					case 0:
					{
						uScontents.ReplaceStr("<string>UIInterfaceOrientationPortrait</string>", "");
						uScontents.ReplaceStr("<string>UIInterfaceOrientationPortraitUpsideDown</string>", "");
						uScontents.ReplaceStr("${InitialInterfaceOrientation}", "UIInterfaceOrientationLandscapeLeft");
						break;
					}
					case 1:
					{
						uScontents.ReplaceStr("<string>UIInterfaceOrientationLandscapeLeft</string>", "");
						uScontents.ReplaceStr("<string>UIInterfaceOrientationLandscapeRight</string>", "");
						uScontents.ReplaceStr("${InitialInterfaceOrientation}", "UIInterfaceOrientationPortrait");
						break;
					}
					case 2:
					{
						uScontents.ReplaceStr("${InitialInterfaceOrientation}", "UIInterfaceOrientationPortrait");
						break;
					}
					}

					uScontents.ReplaceStr("${VERSION}", pCurrentSelectedProject->ipa_version_number);
					uScontents.ReplaceStr("${BUILD}", pCurrentSelectedProject->ipa_build_number);

					if (pCurrentSelectedProject->ipa_device_type == 1)
						uScontents.ReplaceStr("\t\t<integer>2</integer>\n", "");
					else if (pCurrentSelectedProject->ipa_device_type == 2)
						uScontents.ReplaceStr("\t\t<integer>1</integer>\n", "");


					// write Info.plist file
					FILE* entfile = fopen(dest_file.GetStr(), "wb+");
					if (!entfile)
						entfile = AGKfopen(dest_file.GetStr(), "wb+");

					if (entfile) {
						fwrite(uScontents.GetStr(), 1, uScontents.GetLength(), entfile);
						fclose(entfile);
					}
					else {
						strcpy(ios_error, "Failed to write Info.plist file");
						Valid = false;
					}

				}

				if (Valid)	{
					// convert plist to binary
					uString sParameters = "", sCmd = "";
					sCmd = "/usr/bin/plutil";
					sParameters.Append("-convert binary1 ");
					sParameters.Append("\"");
					sParameters.Append(dest_file.GetStr());
					sParameters.Append("\"");

					static int iStdOutLength, iStdErrLength;
					int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run userid program.");
						Valid = false;
					}
					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], CERT_DATA_LEN - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}


							sprintf(ios_error, "Timeout userid program, (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								int rs = read_buffers(&tmp[0], CERT_DATA_LEN - 90, NULL, 0);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
						}

					}
				}

				if (Valid) {

					// load icon file
					if (strlen(pCurrentSelectedProject->ipa_app_icon_path) > 0 )
					{
						// write Icons.xcassets file
						if (!CopyDirTo(icons_src_folder.GetStr(), icons_dst_folder.GetStr()))
						{
							strcpy(ios_error, "Failed to create icon asset catalogr");
							Valid = false;
						}

#ifdef AGK_WINDOWS
						_mkdir(icons_sub_folder.GetStr());
#else
						mkdir(icons_sub_folder.GetStr(), S_IRWXU);
#endif
					

						uString icon_file = "raw:";
						icon_file.Append(pCurrentSelectedProject->ipa_app_icon_path);
						int icon_image = agk::LoadImage(icon_file.GetStr());
						if (!icon_image)
						{
							sprintf(ios_error, "Failed to load image icon: %s", icon_file.GetStr());
							Valid = false;
						}

						uString temp_image_filename = "raw:";
						temp_image_filename.Append(icons_sub_folder);
						temp_image_filename.Append("/temp.png");


						uString image_filename = icons_sub_folder;
						image_filename.Append("/icon-1024.png");
						Valid = GenerateIPAResizedImage(icon_image, (char *)temp_image_filename.GetStr(), 1024, 1024);
						if (Valid) {
							Valid = ios_convert_to_cgbi_png(temp_image_filename.GetStr(), image_filename.GetStr());
						}

						if (Valid) {
							image_filename = icons_sub_folder;
							image_filename.Append("/icon-180.png");
							Valid = GenerateIPAResizedImage(icon_image, (char *)temp_image_filename.GetStr(), 180, 180);
							if (Valid) {
								Valid = ios_convert_to_cgbi_png(temp_image_filename.GetStr(), image_filename.GetStr());
							}
						}
						if (Valid) {
							image_filename = icons_sub_folder;
							image_filename.Append("/icon-167.png");
							Valid = GenerateIPAResizedImage(icon_image, (char *)temp_image_filename.GetStr(), 167, 167);
							if (Valid) {
								Valid = ios_convert_to_cgbi_png(temp_image_filename.GetStr(), image_filename.GetStr());
							}
						}
						if (Valid) {
							image_filename = icons_sub_folder;
							image_filename.Append("/icon-152.png");
							Valid = GenerateIPAResizedImage(icon_image, (char *)temp_image_filename.GetStr(), 152, 152);
							if (Valid) {
								Valid = ios_convert_to_cgbi_png(temp_image_filename.GetStr(), image_filename.GetStr());
							}
						}

						if (Valid) {
							image_filename = icons_sub_folder;
							image_filename.Append("/icon-120.png");
							Valid = GenerateIPAResizedImage(icon_image, (char *)temp_image_filename.GetStr(), 120, 120);
							if (Valid) {
								Valid = ios_convert_to_cgbi_png(temp_image_filename.GetStr(), image_filename.GetStr());
							}
						}
						if (Valid) {
							image_filename = icons_sub_folder;
							image_filename.Append("/icon-76.png");
							Valid = GenerateIPAResizedImage(icon_image, (char *)temp_image_filename.GetStr(), 76, 76);
							if (Valid) {
								Valid = ios_convert_to_cgbi_png(temp_image_filename.GetStr(), image_filename.GetStr());
							}
						}
						agk::DeleteImage(icon_image);


						if (Valid) {
							/*
							One user got:
							<key>com.apple.actool.errors</key>
							<array>
							<dict>
							<key>description</key>
							<string>NSString *IBCurrentDirectoryPath(NSError *__autoreleasing *) - currentDirectoryPath is unexpectedly nil</string>
							<key>failure-reason</key>
							<string>No such file or directory</string>
							</dict>

							Strange ? , we could use full path.
							*/


							// run actool to compile asset catalog, it will copy the app icons to the app_folder and create the Assets.car file
							uString sParameters = "", sCmd = "";
							sCmd = path_to_actool;
							sParameters.Append("--output-partial-info-plist temp.plist --app-icon AppIcon ");
							sParameters.Append("--target-device iphone --target-device ipad ");

							sParameters.Append("--minimum-deployment-target 8.0 --platform iphoneos ");
							sParameters.Append("--product-type com.apple.product-type.application ");
							sParameters.Append("--compress-pngs --compile ");
							//Old
//							sParameters.Append("\"");
//							sParameters.Append(app_folder_name.GetStr());
//							sParameters.Append("\"");
//							sParameters.Append(" Icons.xcassets");
							
							//Test using full path.
							sParameters.Append("\"");
							sParameters.Append(app_folder.GetStr());
							sParameters.Append("\"");
							sParameters.Append(" \"");
							sParameters.Append(tmp_folder);
							sParameters.Append("/Icons.xcassets");
							sParameters.Append("\"");


							//tmp_folder
							chdir(tmp_folder);

#ifdef AGK_WINDOWS 			//Short delay.
							Sleep(200);
#else
							usleep(200);
#endif

							static int iStdOutLength, iStdErrLength;
							int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

							if (ios_generate_log) {
								sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
								cp_appendtofile(output_logfile, output_debug);
							}

							if (appid == false) {
								strcpy(ios_error, "Failed to run actool program.");
								Valid = false;
							}
							if (Valid) {
								float timestart = agk::Timer();
								int still_running;
								still_running = agk::GetAppRunning(appid);
								strcpy(tmp, "");
								bool bAaptDone = false;

								while (still_running == 1 && agk::Timer() - timestart < 300.0)
								{
#ifdef AGK_WINDOWS
									Sleep(200);
#else
									usleep(200);
#endif
									still_running = agk::GetAppRunning(appid);
								}
								if (still_running) {
									//Timeout.
									int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
									if (ios_generate_log) {
										cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
										cp_appendtofile(output_logfile, tmp);
										cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
									}

									sprintf(ios_error, "Timeout actool program (output: %s)", tmp);
									Valid = false;
									agk::TerminateApp(appid);
								}

								if (Valid) {
									int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);

									if (ios_generate_log) {
										cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
										cp_appendtofile(output_logfile, tmp);
										cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
									}


									if (strstr(str_out, "actool.errors") != 0 || strstr(str_out, "actool.warnings") != 0 || strstr(str_out, "actool.notices") != 0) {
										sprintf(ios_error, "Failed to compile asset catalog (output: %s)", tmp);
										Valid = false;

									}

								}

							}
						}
					}
				}

				if ( Valid )
				{
					// copy splash logo
					if ( pCurrentSelectedProject->ipa_splash_logo && *pCurrentSelectedProject->ipa_splash_logo )
					{
						uString splash_path = app_folder;
						splash_path.Append("/SplashLogo.png");
						if (!cp_copyfile(pCurrentSelectedProject->ipa_splash_logo, (char *)splash_path.GetStr())) {
							if (ios_generate_log) {
								sprintf(output_debug, "copy failed: %s,%s\n", pCurrentSelectedProject->ipa_splash_logo, splash_path.GetStr());
								cp_appendtofile(output_logfile, output_debug);
							}
							strcpy(ios_error, "Failed to copy SplashLogo.png");
							Valid = false;
						}
					}
				}
				
				if ( Valid )
				{
					if ( pCurrentSelectedProject->ipa_splash_color && *pCurrentSelectedProject->ipa_splash_color )
					{
						char *splash_color = pCurrentSelectedProject->ipa_splash_color;
						int iRed = hextoint( splash_color[0] ) * 16 + hextoint( splash_color[1] );
						int iGreen = hextoint( splash_color[2] ) * 16 + hextoint( splash_color[3] );
						int iBlue = hextoint( splash_color[4] ) * 16 + hextoint( splash_color[5] );

						float red = iRed / 255.0f;
						float green = iGreen / 255.0f;
						float blue = iBlue / 255.0f;

						uString newContents = "";
						char newcolor[ 256 ];
						char* start;
						FILE* fp;

						// edit UntitledViewController
						uString temp_path = app_folder;
						temp_path.Append("/UntitledViewController.xib");
					
						if ( contents ) free(contents);
						contents = 0;
						if ( !(contents = g_file_get_contents( (char*)temp_path.GetStr(), contents, &length, NULL )) )
						{
							strcpy(ios_error, "Failed to read UntitledViewController.xib file" );
							goto splash_error;
						}

						start = strstr( contents, "backgroundColor\"" );
						if ( !start )
						{
							strcpy(ios_error, "Failed to modify UntitledViewController.xib, backgroundColor not found" );
							goto splash_error;
						}

						start += strlen("backgroundColor\"");
						*start = 0;
						newContents.Append( contents );
						*start = ' ';
												
						*newcolor = 0;
						sprintf( newcolor, " red=\"%f\" green=\"%f\" blue=\"%f\" ", red, green, blue );
						newContents.Append( newcolor );

						start = strstr( start, "alpha=" );
						if ( !start )
						{
							strcpy(ios_error, "Failed to modify UntitledViewController.xib, alpha not found" );
							goto splash_error;
						}

						newContents.Append( start );

						fp = fopen(temp_path.GetStr(), "wb+");
						if (!fp)
							fp = AGKfopen(temp_path.GetStr(), "wb+");

						if (fp) {
							fwrite(newContents.GetStr(), 1, newContents.GetLength(), fp);
							fclose(fp);
						}
						else {
							strcpy(ios_error, "Failed to write UntitledViewController.xib file");
							goto splash_error;
						}


						// edit storyboard
						temp_path = app_folder;
						temp_path.Append("/LaunchScreen.storyboard");
						
						if ( contents ) free(contents);
						contents = 0;
						if ( !(contents = g_file_get_contents( (char*)temp_path.GetStr(), contents, &length, NULL )) )
						{
							strcpy(ios_error, "Failed to read LaunchScreen.storyboard file" );
							goto splash_error;
						}	

						newContents = "";
						start = strstr( contents, "backgroundColor\"" );
						if ( !start )
						{
							strcpy(ios_error, "Failed to modify LaunchScreen.storyboard, backgroundColor not found" );
							goto splash_error;
						}

						start += strlen("backgroundColor\"");
						*start = 0;
						newContents.Append( contents );
						*start = ' ';

						*newcolor = 0;
						sprintf( newcolor, " red=\"%f\" green=\"%f\" blue=\"%f\" ", red, green, blue );
						newContents.Append( newcolor );

						start = strstr( start, "alpha=" );
						if ( !start )
						{
							strcpy(ios_error, "Failed to modify LaunchScreen.storyboard, alpha not found" );
							goto splash_error;
						}

						newContents.Append( start );

						fp = fopen(temp_path.GetStr(), "wb+");
						if (!fp)
							fp = AGKfopen(temp_path.GetStr(), "wb+");

						if (fp) {
							fwrite(newContents.GetStr(), 1, newContents.GetLength(), fp);
							fclose(fp);
						}
						else {
							strcpy(ios_error, "Failed to write LaunchScreen.storyboard file");
							goto splash_error;
						}

						goto splash_end;

					splash_error:			
						Valid = false;

					splash_end:				
						{}
					}
				}

				if ( Valid )
				{
					// compile UntitledViewController
					uString temp_path1 = app_folder;
					temp_path1.Append("/UntitledViewController.xib");

					uString temp_path2 = app_folder;
					temp_path2.Append("/UntitledViewController.nib");

					uString sParameters = "";
					sParameters.Append("--errors ");
					sParameters.Append("--warnings ");
					sParameters.Append("--notices ");
					sParameters.Append("--module ");
					sParameters.Append("AppGameKit_Player ");
					sParameters.Append("--auto-activate-custom-fonts ");
					sParameters.Append("--target-device ");
					sParameters.Append("iphone ");
					sParameters.Append("--target-device ");
					sParameters.Append("ipad ");
					sParameters.Append("--minimum-deployment-target ");
					sParameters.Append("13.0 ");
					sParameters.Append("--output-format ");
					sParameters.Append("human-readable-text ");
					sParameters.Append("--compile ");
					sParameters.Append("\"");
					sParameters.Append(temp_path2);
					sParameters.Append("\" ");
					sParameters.Append("\"");
					sParameters.Append(temp_path1);
					sParameters.Append("\"");

					static int iStdOutLength, iStdErrLength;
					int appid = run_command("/Applications/XCode.app/Contents/Developer/usr/bin/ibtool", sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: ibtool %s\n", sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run ibtool program.");
						Valid = false;
					}
					
					if (Valid) 
					{
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
							
							sprintf(ios_error, "Timeout userid program, (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
						}
					}
				}

				if ( Valid )
				{
					// delete old UntitledViewController.xib
					uString temp_path = app_folder;
					temp_path.Append( "/UntitledViewController.xib" );
					if (file_exists((char*)temp_path.GetStr())) {
						remove(temp_path);
					}
				}

				if ( Valid )
				{
					// compile storyboard
					uString temp_path1 = app_folder;
					temp_path1.Append("/LaunchScreen.storyboard");
					
					uString sParameters = "";
					sParameters.Append("--errors ");
					sParameters.Append("--warnings ");
					sParameters.Append("--notices ");
					sParameters.Append("--module ");
					sParameters.Append("AppGameKit_Player ");
					sParameters.Append("--auto-activate-custom-fonts ");
					sParameters.Append("--target-device ");
					sParameters.Append("iphone ");
					sParameters.Append("--target-device ");
					sParameters.Append("ipad ");
					sParameters.Append("--minimum-deployment-target ");
					sParameters.Append("13.0 ");
					sParameters.Append("--output-format ");
					sParameters.Append("human-readable-text ");
					sParameters.Append("--compilation-directory ");
					sParameters.Append("\"");
					sParameters.Append(app_folder);
					sParameters.Append("\" ");
					sParameters.Append("\"");
					sParameters.Append(temp_path1);
					sParameters.Append("\"");
			
					static int iStdOutLength, iStdErrLength;
					int appid = run_command("/Applications/XCode.app/Contents/Developer/usr/bin/ibtool", sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: ibtool %s\n", sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run ibtool program.");
						Valid = false;
					}
					
					if (Valid) 
					{
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
							
							sprintf(ios_error, "Timeout userid program, (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
						}
					}
				}

				if ( Valid )
				{
					// delete old storyboard
					uString temp_path = app_folder;
					temp_path.Append( "/LaunchScreen.storyboard" );
					if (file_exists((char*)temp_path.GetStr())) {
						remove(temp_path);
					}
				}

				if (Valid) {
					// copy media folder
					if (!ios_exporting_player )
					{
						//Add Project Media folder.
						uString sMediaPath = "";
						uString m_szParse, m_szPathName;
						m_szParse = pCurrentSelectedProject->m_sProjectFileFullPath;
						m_szParse.ReplaceStr("\\\\", "/");
						m_szParse.ReplaceStr("\\", "/");
						int pos = m_szParse.RevFind('/');
						if (pos >= 0) {
							m_szParse.SubString(sMediaPath, 0, pos + 1);
							sMediaPath.Append("media");

							uString dest = app_folder;
							dest.Append("/media");

							if (!CopyDirTo(sMediaPath.GetStr(), dest.GetStr()))
							{
								strcpy(ios_error, "Failed to copy media folder.");
								Valid = false;
							}
						}
					}
				}


				if (Valid) {
					// find user name

					uString sParameters = "", sCmd = "";
					sCmd = "/usr/bin/id";
					sParameters.Append("-u -n");

					static int iStdOutLength, iStdErrLength;
					int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run userid program. (/usr/bin/id -u -n)");
						Valid = false;
					}
					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
							sprintf(ios_error, "Timeout userid program. /usr/bin/id -u -n (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}

					}

					if (Valid) {

						int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
						if (ios_generate_log) {
							cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
							cp_appendtofile(output_logfile, tmp);
							cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
						}
						if (strlen(tmp) <= 0) {
							strcpy(ios_error, "Failed to get user name . (/usr/bin/id -u -n)");
							Valid = false;
						}
						user_name = strdup(str_out);
						user_name[strlen(user_name) - 1] = 0;
					}
				}

				if (Valid) {
					// find group name

					uString sParameters = "", sCmd = "";
					sCmd = "/usr/bin/id";
					sParameters.Append("-g -n");

					static int iStdOutLength, iStdErrLength;
					int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run groupid program. (/usr/bin/id -u -n)");
						Valid = false;
					}
					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}

							sprintf(ios_error, "Timeout groupid program. /usr/bin/id -g -n (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
					}

					if (Valid) {
						int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
						if (ios_generate_log) {
							cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
							cp_appendtofile(output_logfile, tmp);
							cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
						}

						if (strlen(tmp) <= 0) {
							strcpy(ios_error, "Failed to get group name . (/usr/bin/id -g -n)");
							Valid = false;
						}
						group_name = strdup(str_out);
						group_name[strlen(group_name) - 1] = 0;
					}
				}


				if (Valid) {

					// prepare bundle
					uString sParameters = "", sCmd = "";
					sCmd = "/usr/sbin/chown";
					sParameters.Append("-RH ");
					sParameters.Append("\"");
					sParameters.Append(user_name);
					sParameters.Append(":");
					sParameters.Append(group_name);
					sParameters.Append("\"");
					sParameters.Append(" ");
					sParameters.Append("\"");
					sParameters.Append(app_folder);
					sParameters.Append("\"");

					chdir(tmp_folder);

					static int iStdOutLength, iStdErrLength;
					int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run chown program. (/usr/sbin/chown )");
						Valid = false;
					}
					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
							sprintf(ios_error, "Timeout chown program. /usr/sbin/chown -g -n (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
						}
					}
				}


				if (Valid) {
					// prepare bundle 2

					uString sParameters = "", sCmd = "";
					sCmd = "/bin/chmod";
					sParameters.Append("-RH u+w,go-w,a+rX ");
					sParameters.Append("\"");
					sParameters.Append(app_folder);
					sParameters.Append("\"");

					chdir(tmp_folder);

					static int iStdOutLength, iStdErrLength;
					int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run chmod program. (/usr/sbin/chown )");
						Valid = false;
					}
					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}

							sprintf(ios_error, "Timeout chmod program. /usr/sbin/chown -g -n (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
						}
					}
				}

				//Some files had extended attributes or resource forks , Remove all.
				if (Valid)
				{
					uString sParameters;
					uString sCmd = "/usr/bin/xattr";
					chdir(tmp_folder);
					sParameters.SetStr("-cr ");
					sParameters.Append("\"");
					sParameters.Append(app_folder);
					sParameters.Append("\"");

					static int iStdOutLength, iStdErrLength;
					int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run xattr program.");
						if (ios_generate_log) {
							sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
							cp_appendtofile(output_logfile, "Failed to run xattr program.\n");
						}
						//Dont fail, just log it.
					}
					else {
					
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
							sprintf(ios_error, "Timeout xattr program. (output: %s)", tmp);
							//Valid = false; //Dont fail just log it.
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
						}
					}
				}

				if (Valid) {
					uString sParameters;
					uString sCmd = path_to_codesign;

					if (chdir(tmp_folder) < 0) {
						if (ios_generate_log) {
							sprintf(output_debug, "chdir failed: %s\n", tmp_folder.GetStr());
							cp_appendtofile(output_logfile, output_debug);
						}
						//Try in quotes.
						uString sChdir = "\"";
						sChdir.Append(tmp_folder);
						sChdir.Append("\"");
						if (chdir(sChdir.GetStr()) < 0) {
							if (ios_generate_log) {
								sprintf(output_debug, "chdir2 failed: %s\n", sChdir.GetStr());
								cp_appendtofile(output_logfile, output_debug);
							}
						}
					}

#ifdef AGK_MACOS
					// make frameworks executable
					uString sFrameworkPath = app_folder;
					sFrameworkPath.Append( "/Frameworks/SCSDKCoreKit.framework/SCSDKCoreKit" );
					chmod( sFrameworkPath.GetStr(), 0775 );

					sFrameworkPath = app_folder;
					sFrameworkPath.Append( "/Frameworks/SCSDKCreativeKit.framework/SCSDKCreativeKit" );
					chmod( sFrameworkPath.GetStr(), 0775 );
#endif

					// sign SnapChat
					sParameters.SetStr("--force --sign ");
					sParameters.Append("\"");
					sParameters.Append(cert_hash);
					sParameters.Append("\"");
					sParameters.Append(" \"");
					sParameters.Append(app_folder);
					sParameters.Append("/Frameworks/SCSDKCoreKit.framework\"");

					static int iStdOutLength, iStdErrLength;
					int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run codesign program.");
						Valid = false;
					}

					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
							sprintf(ios_error, "Timeout codesign program. (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
						}
					}

					// sign SnapChat2
					sParameters.SetStr("--force --sign ");
					sParameters.Append("\"");
					sParameters.Append(cert_hash);
					sParameters.Append("\"");
					sParameters.Append(" \"");
					sParameters.Append(app_folder);
					sParameters.Append("/Frameworks/SCSDKCreativeKit.framework\"");

					appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run codesign program 2.");
						Valid = false;
					}

					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 60.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
							sprintf(ios_error, "Timeout codesign program. (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
							}
						}
					}

					// sign bundle
					//argv = g_new0(gchar*, 10);
					//argv[0] = g_strdup(path_to_codesign);
					//argv[1] = g_strdup("--force");
					//argv[2] = g_strdup("--sign");
					//argv[3] = g_strdup(cert_hash);
					////argv[4] = g_strdup("--resource-rules"); // Apple stopped using resource rules?
					////argv[5] = g_strconcat( app_folder, "/ResourceRules.plist", NULL );
					//argv[4] = g_strdup("--entitlements");
					//argv[5] = g_strdup(entitlements_file);
					//argv[6] = g_strdup(app_folder);
					//argv[7] = NULL;

					uString ent_file = tmp_folder;
					ent_file.Append("/entitlements.xcent");
					
					sParameters.SetStr("--force --sign ");
					sParameters.Append("\"");
					sParameters.Append(cert_hash);
					sParameters.Append("\"");
					sParameters.Append(" --entitlements ");
					sParameters.Append("\"");
					sParameters.Append(ent_file.GetStr());
					sParameters.Append("\"");
					sParameters.Append(" \"");
					sParameters.Append(app_folder);
					sParameters.Append("\"");
					//codesign always sent out to stderr (not stdout) even with below redirect.
					//sParameters.Append(" 2>&1"); //This do not seams to work , stderr > stdout still cant read output ?
					//When we now also log stderr it can contain contain garbage like debug info ... so study it carefully not everything is errors.

					//Short delay before last signing.
#ifdef AGK_WINDOWS
					Sleep(200);
#else
					usleep(200);
#endif

					appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (ios_generate_log) {
						sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
						cp_appendtofile(output_logfile, output_debug);
					}

					if (appid == false) {
						strcpy(ios_error, "Failed to run codesign program 3.");
						Valid = false;
					}

					if (Valid) {
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						strcpy(tmp, "");
						bool bAaptDone = false;

						while (still_running == 1 && agk::Timer() - timestart < 180.0) // increased timeout, takes a long time with large DTS projects
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
						}
						if (still_running) {
							//Timeout.
							//Also read stderr
							char tmp_err[32768];
							int rs = read_buffers(&tmp[0], 32768 - 90, &tmp_err[0], 32768 - 90);
							if (ios_generate_log) {
								cp_appendtofile(output_logfile, "TIMEOUT OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
								cp_appendtofile(output_logfile, "OutPutERRStart:\n---\n");
								cp_appendtofile(output_logfile, tmp_err);
								cp_appendtofile(output_logfile, "\n---\nOutPutERREND:\n");
							}
							sprintf(ios_error, "Timeout codesign program. (output: %s)", tmp);
							Valid = false;
							agk::TerminateApp(appid);
						}
						else {
							if (ios_generate_log) {
								//Also read stderr
								char tmp_err[32768];
								int rs = read_buffers(&tmp[0], 32768 - 90, &tmp_err[0], 32768 - 90);
								cp_appendtofile(output_logfile, "OutPutStart:\n---\n");
								cp_appendtofile(output_logfile, tmp);
								cp_appendtofile(output_logfile, "\n---\nOutPutEND:\n");
								cp_appendtofile(output_logfile, "OutPutERRStart:\n---\n");
								cp_appendtofile(output_logfile, tmp_err);
								cp_appendtofile(output_logfile, "\n---\nOutPutERREND:\n");
							}
						}

					}
				}


				if (Valid) {

					// create IPA zip file
					uString zipfile = "raw:";
					zipfile.Append(output_file_zip);

					int zipid = 101;

					agk::SetCurrentDir(""); //PE: Needed or .ipa will fail after a debug session.

					zipid = agk::CreateZip(zipfile.GetStr()); // no append. , 2

					if (zipid == 0) {
						strcpy(ios_error, "Failed to initialise zip file for writing");
						Valid = false;
					}

					if (Valid) {
						// copy in extra files
						//uString zip_add_file = "raw:";
						//zip_add_file.Append(src_folder);
						//zip_add_file.Append("/classes.dex");
						//agk::AddZipEntry(zipid, zip_add_file.GetStr(), "classes.dex");
						
						uString zip_name_file = "Payload/";
						zip_name_file.Append(pCurrentSelectedProject->ipa_app_name);
						zip_name_file.Append(".app");

						//temp_filename1 = g_strconcat("Payload/", pCurrentSelectedProject->ipa_app_name, ".app", NULL);
						//if (!utils_add_folder_to_zip(&zip_archive, app_folder, temp_filename1, TRUE, FALSE))
						Valid = utils_add_folder_to_zip(zipid, (char*)app_folder.GetStr(), (char *) zip_name_file.GetStr());

					}

					agk::CloseZip(zipid);

					rename(output_file_zip, output_file);

					if (!file_exists(output_file)) {
						strcpy(ios_error, "Final zip->ipa failed.");
						Valid = false;
					}

				}


				if (Valid) {

					//Clean up.
					if (path_to_codesign) free(path_to_codesign);
					if (path_to_security) free(path_to_security);
					if (contents) free(contents);
					if (certificate_data) free(certificate_data);
					if (team_id) free(team_id);
					if (bundle_id) free(bundle_id);
					if (cert_hash) free(cert_hash);
					if (cert_temp) free(cert_temp);
					if (app_group_data) free(app_group_data);
					if (entitlements_file) free(entitlements_file);
					if (expanded_entitlements_file) free(expanded_entitlements_file);
					if (temp_filename1) free(temp_filename1);
					if (temp_filename2) free(temp_filename2);
					if (version_string) free(version_string);
					if (build_string) free(build_string);
					if (image_filename) free(image_filename);
					if (user_name) free(user_name);
					if (group_name) free(group_name);

					if(!ios_leave_buildtmp )
						utils_remove_folder_recursive(tmp_folder.GetStr());

					//Success :)
					show_iosexport_window = false;

				}

#ifdef AGK_WINDOWS
				_chdir(curDir);
#else
				chdir(curDir);
#endif

			}


		}
		PleaseWaitEnd();
	}

	ImGui::Columns(1);

	ImGui::End();

}


static int ios_convert_to_cgbi_png(const char* oldfilename, const char* newfilename)
{
	uString ofilename = oldfilename;
	ofilename.ReplaceStr("raw:", "");

	uString sParameters = "", sCmd = "";
	sCmd = "/usr/bin/xcrun";
//	sParameters.Append("-sdk iphoneos pngcrush -iphone ");
	//Force it to write (would skip if larger then source) , fix any crc errors , make it quiet.
	sParameters.Append("-sdk iphoneos pngcrush -iphone -force -fix -q ");
	sParameters.Append("\"");
	sParameters.Append(ofilename.GetStr());
	sParameters.Append("\"");
	sParameters.Append(" \"");
	sParameters.Append(newfilename);
	sParameters.Append("\"");

	if (ios_generate_log) {
		sprintf(output_debug, "running: %s %s\n", sCmd.GetStr(), sParameters.GetStr());
		cp_appendtofile(output_logfile, output_debug);
	}

	static int iStdOutLength, iStdErrLength;
	int appid = run_command(sCmd.GetStr(), sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);
	if (appid == false) {
		strcpy(ios_error, "Failed to run pngcrush program.");
		return(false);
	}

	float timestart = agk::Timer();
	int still_running;
	still_running = agk::GetAppRunning(appid);
	char tmp[32768];
	strcpy(tmp, "");

	while (still_running == 1 && agk::Timer() - timestart < 120.0)
	{
#ifdef AGK_WINDOWS
		Sleep(200);
#else
		usleep(200);
#endif
		still_running = agk::GetAppRunning(appid);
	}
	if (still_running) {
		//Timeout.
		int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
		sprintf(ios_error, "Timeout pngcrush program, (output: %s)", tmp);
		agk::TerminateApp(appid);
		return(false);
	}
	else {
		if (ios_generate_log) {
			//Also read stderr
			char tmp_err[32768];
			int rs = read_buffers(&tmp[0], 32768 - 90, NULL, 0);
			cp_appendtofile(output_logfile, "stdout:");
			cp_appendtofile(output_logfile, tmp);
			cp_appendtofile(output_logfile, "stderr:");
			cp_appendtofile(output_logfile, tmp_err);
			cp_appendtofile(output_logfile, "\n");
		}

	}
	if (!file_exists((char *) newfilename)) {
		sprintf(ios_error, "Failed to convert png file \"%s\" to Apple's CgBI format", newfilename);
		return false;
	}
	return true;
}


//###############################
//#### Android Export Window ####
//###############################

/*
Read from agk
[apk_settings]
alias=
app_icon_path=D:\\div\\pe-granite.png
notif_icon_path=D:\\div\\pe-granite.png
app_name=TerrainAppName
app_type=0
game_circle_api_key=
keystore_path=D:\\div\\test4.keystore
orientation=0
output_path=D:\\github\\AGKTier2\\AgkIde\\Final\\testprojects\\Terrain\\Terrain.apk
ouya_icon_path=
package_name=com.plemsoft.terrain
permission_flags=7
play_app_id=
sdk_version=1
version_name=1.0.0
version_number=1
firebase_config_path=
url_scheme=
deep_link=
arcore=0

*/
/*
char apk_alias[MAX_PATH];
char apk_icon_path[MAX_PATH];
char apk_notif_icon_path[MAX_PATH];
char apk_app_name[MAX_PATH];
int  apk_app_type = 0;
char game_circle_api_key[MAX_PATH];
char apk_keystore_path[MAX_PATH];
int  apk_orientation = 0;
char apk_output_path[MAX_PATH];
char apk_ouya_icon_path[MAX_PATH];
char apk_package_name[MAX_PATH];
int  apk_permission_flags = 0;
char apk_play_app_id[MAX_PATH]; 
int  apk_sdk_version = 0;
char apk_version_name[MAX_PATH];
int  apk_version_number = 1;
char apk_firebase_config_path[MAX_PATH];
char apk_url_scheme[MAX_PATH];
char apk_deep_link[MAX_PATH];
int  apk_arcore = 0;
*/

extern int AndroidExportHTTP;
extern float AndroidExportHTTPTimer;
extern char defaultWriteFolder[MAX_PATH];

//#define DEBUG_EXPORT	1

char android_error[32768];
void ProcessAndroidExport(void)
{
	if ( AndroidExportHTTP > 0 )
	{
		int result = agk::GetHTTPResponseReady(AndroidExportHTTP);
		if ( result < 0 )
		{
			BoxerInfo( "Failed to download Android export files", "Download Failed" );

			agk::DeleteHTTPConnection( AndroidExportHTTP );
			AndroidExportHTTP = -1;
			AndroidExportHTTPTimer = 0;
		}
		else if ( result > 0 )
		{
			agk::DeleteHTTPConnection( AndroidExportHTTP );
			AndroidExportHTTP = -1;
			AndroidExportHTTPTimer = 0;

			uString sExportPath = "raw:";
			sExportPath.Append( defaultWriteFolder );
			sExportPath.AppendUTF8( "AndroidExport.zip" );

			uString sExportPath2 = "raw:";
			sExportPath2.Append( defaultWriteFolder );
			sExportPath2.Trim( "/" );
			agk::ExtractZip( sExportPath, sExportPath2 );

			BoxerInfo( "Download finished, you can now export Android APKs", "Download Finished" );
		}
		else if ( agk::Timer() - AndroidExportHTTPTimer > 300 )
		{
			BoxerInfo( "Downloading Android export files timed out", "Download Failed" );

			agk::DeleteHTTPConnection( AndroidExportHTTP );
			AndroidExportHTTP = -1;
			AndroidExportHTTPTimer = 0;
		}
	}

	if (!show_androidexport_window)
		return;
	if(!pCurrentSelectedProject)
		return;
	
	struct TextFilters {
		static int TextFiltersAZ1(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", (char)data->EventChar)) return 0; return 1; }
		static int TextFiltersAZDOT(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_.0123456789", (char)data->EventChar)) return 0; return 1; }
		static int TextFiltersDIGITS(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("0123456789", (char)data->EventChar)) return 0; return 1; }
		static int TextFiltersVERSION(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("0123456789.", (char)data->EventChar)) return 0; return 1; }
	};
	//*ImGui::GetFontSize()
	int input_indent = 12*ImGui::GetFontSize();
	int item_dot_width = -32;
	ImGuiWindowFlags window_flags = 0;
	//ImGui::SetNextWindowSize(ImVec2(67 * ImGui::GetFontSize(), 44 * ImGui::GetFontSize()), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(70 * ImGui::GetFontSize(), 46 * ImGui::GetFontSize()), ImGuiCond_Once);
	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
	ImGui::Begin("Android export.", &show_androidexport_window, window_flags);
	if (show_androidexport_window_state != show_androidexport_window) {
		show_androidexport_window_state = show_androidexport_window;
		ImGui::SetKeyboardFocusHere();
		strcpy(android_error, "");
	}

	ImGui::Indent(8.0);
	ImGui::Spacing();

	ImVec4* style_colors = ImGui::GetStyle().Colors;
	ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
	ImGui::SetWindowFontScale(1.1);
	style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
	ImGui::TextWrapped("APK Export:");
	style_colors[ImGuiCol_Text] = oldTextColor;
	ImGui::SetWindowFontScale(1.0);

	ImGui::Indent(10.0);
	ImGui::TextWrapped("This will build an APK from your media and bytecode that will run on any Android device running 4.1 or above. It can also be used to produce an APK suitable for the Google Play, Amazon, or Ouya store if you also provide a keystore file for signing. If you don't yet have a keystore file you can generate one from the Tools menu or in Android Studio.");


	ImGui::Columns(2, "myAPKexportDialog", false);  // 3-ways, no border


	ImGui::Indent(-10.0);
	ImGui::Text("APK Settings:");
	ImGui::Indent(10.0);

	ImGui::Text("APK Type:");
	const char* apk_type[ 10 ];
	int index = 0;
	int DTSBundleIndex = -1;
	int DTSAPKIndex = -1;
	// first 4 must be in this order to detect chosen type later
	apk_type[ index++ ] = "Google Bundle";
	apk_type[ index++ ] = "Google APK";
	apk_type[ index++ ] = "Amazon APK";
	apk_type[ index++ ] = "Ouya APK";
	//if ( bNoAdsPossible ) apk_type[ index++ ] = "Google Bundle No Ads";
	if ( bDTSPossible ) 
	{
		DTSBundleIndex = index;
		apk_type[ index++ ] = "DTS Bundle";
		DTSAPKIndex = index;
		apk_type[ index++ ] = "DTS APK";
	}
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::Combo("##comboapk_app_type", &pCurrentSelectedProject->apk_app_type, apk_type, index);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Choose Google for apps using Google Play Services or ARCore, choose Amazon for apps using Amazon IAP, choose Ouya for apps that use the Ouya SDK. Choose Bundle if the result will be uploaded to Google Play, or choose APK if the result will be sideloaded onto an Android device");
		ImGui::EndTooltip();
	}

	bool bIsGoogle = false;
	bool bIsAmazon = false;
	bool bIsOuya = false;
	bool bUseAds = true;
	bool bIsBundle = false;
	bool bIsDTS = false;
	if ( pCurrentSelectedProject->apk_app_type == 0 ||pCurrentSelectedProject->apk_app_type == DTSBundleIndex ) bIsBundle = true;
	if ( pCurrentSelectedProject->apk_app_type == 0 || pCurrentSelectedProject->apk_app_type == 1 ) bIsGoogle = true;
	if ( pCurrentSelectedProject->apk_app_type == DTSBundleIndex || pCurrentSelectedProject->apk_app_type == DTSAPKIndex ) 
	{
		bIsGoogle = true;
		bIsDTS = true;
	}
	if ( pCurrentSelectedProject->apk_app_type == 2 ) bIsAmazon = true;
	if ( pCurrentSelectedProject->apk_app_type == 3 ) bIsOuya = true;

	ImGui::Text("App Name:"); // max 30 A-Z 0-9 space underscore
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##apk_app_name", &pCurrentSelectedProject->apk_app_name[0], 30, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersAZ1);
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Only A-Z,0-9,spaces or underscore allowed.");

	ImGui::Text("Package Name:"); // max 30 A-Z 0-9 space underscore
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##apk_package_name", &pCurrentSelectedProject->apk_package_name[0], 100, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersAZDOT);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Only A-Z . and underscore allowed.\nShould be in format: com.mycompany.my_app");
		ImGui::EndTooltip();
	}

	ImGui::Text("App Icon:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##apk_new_icon_path", &pCurrentSelectedProject->apk_new_icon_path[0], 255);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(600, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Should be at least 432x432 pixels with all important elements within a 288 pixel diameter circle in the centre. PNG only. Should not contain transparency. Used on Google devices running Android 8 and above. If this is left blank then only the legacy icon will be used");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##apk_new_icon_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "png\0*.png\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->apk_new_icon_path, cFileSelected);
		}
	}

	ImGui::Text("App Icon (Legacy):");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##apk_icon_path", &pCurrentSelectedProject->apk_icon_path[0], 255);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(600, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Should be at least 192x192 pixels. PNG only. Should contain transparency for rounded edges. Used on all Amazon and Ouya devices, and on Google devices running Android 7 and below. If this is left blank then the default AGK icon will be used");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##apk_icon_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "png\0*.png\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->apk_icon_path, cFileSelected);
		}
	}

	ImGui::Text("Notification Icon:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##apk_notif_icon_path", &pCurrentSelectedProject->apk_notif_icon_path[0], 255);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used in the Android notification bar if you display local or push notifications. Should be at least 96x96. PNG only. It must be all white with the shape defined by the alpha channel. If this is left blank then the default AGK icon will be used");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##apk_notif_icon_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "png\0*.png\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->apk_notif_icon_path, cFileSelected);
		}
	}

	if ( bIsOuya ) {
		ImGui::Text("Ouya Large Icon:");
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
		ImGui::PushItemWidth(item_dot_width);
		ImGui::InputText("##apk_ouya_icon_path", &pCurrentSelectedProject->apk_ouya_icon_path[0], 255);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Must be 732x412 pixels, PNG only");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("...##apk_ouya_icon_path")) {
			char * cFileSelected;
			cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "png\0*.png\0", NULL, NULL);
			if (cFileSelected && strlen(cFileSelected) > 0) {
				strcpy(pCurrentSelectedProject->apk_ouya_icon_path, cFileSelected);
			}
		}
	}


	ImGui::Text("Orientation:");
	const char* apk_orintation_array[] = { "Landscape", "Portrait", "All" };
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::Combo("##comboapk_orientation", &pCurrentSelectedProject->apk_orientation, apk_orintation_array, IM_ARRAYSIZE(apk_orintation_array));


	ImGui::Text("Minimum Android Version:");
	const char* apk_version_array[] = { "4.1 (API 16)", "4.2 (API 17)", "4.3 (API 18)", "4.4 (API 19)", "5.0 (API 21)", "5.1 (API 22)", "6.0 (API 23)"
		, "7.0 (API 24)", "7.1 (API 25)", "8.0 (API 26)", "8.1 (API 27)", "9.0 (API 28)", "10.0 (API 29)", "11.0 (API 30)", "12.0 (API 31)", "13.0 (API 33)", "14.0 (API 34)" };
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::Combo("##comboapk_sdk_version", &pCurrentSelectedProject->apk_sdk_version, apk_version_array, IM_ARRAYSIZE(apk_version_array));
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("For maximum compatibility choose 4.1, if ARCore is required then this must be at least 7.0");
		ImGui::EndTooltip();
	}

	if ( bIsGoogle ) { //Google only.

		ImGui::Text("ARCore:");
		const char* apk_arcore_array[] = { "Not Used", "Optional", "Required" };
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
		ImGui::Combo("##comboapk_arcore", &pCurrentSelectedProject->apk_arcore, apk_arcore_array, IM_ARRAYSIZE(apk_arcore_array));
		if (ImGui::IsItemHovered()) {
			ImGui::SetNextWindowContentSize(ImVec2(400, 0));
			ImGui::BeginTooltip();
			ImGui::TextWrapped("Sets whether your app uses or requires ARCore to be installed on the user's device. Requiring ARCore will limit your app to Android 7.0 or above.");
			ImGui::EndTooltip();
		}
		if (pCurrentSelectedProject->apk_arcore == 2) { // Required
			if (pCurrentSelectedProject->apk_sdk_version < 7)
				pCurrentSelectedProject->apk_sdk_version = 7; // 7.0 API 24
		}
	}


	ImGui::Indent(-10.0);
	ImGui::Text("Permissions:");
	ImGui::Indent(10.0);


#define AGK_ANDROID_PERMISSION_WRITE                0x001
#define AGK_ANDROID_PERMISSION_INTERNET             0x002
#define AGK_ANDROID_PERMISSION_WAKE                 0x004
#define AGK_ANDROID_PERMISSION_GPS                  0x008
#define AGK_ANDROID_PERMISSION_IAP                  0x010
#define AGK_ANDROID_PERMISSION_LOCAL_NOTIFICATIONS  0x020
#define AGK_ANDROID_PERMISSION_LOCATION             0x040
#define AGK_ANDROID_PERMISSION_PUSH                 0x080
#define AGK_ANDROID_PERMISSION_CAMERA               0x100
#define AGK_ANDROID_PERMISSION_VIBRATE              0x200
#define AGK_ANDROID_PERMISSION_RECORD_AUDIO         0x400
#define AGK_ANDROID_PERMISSION_ADVERTS		        0x800


	//ImGui::BeginChild("##chrildpermissioncols",ImVec2(0,110));
	ImGui::BeginChild("##chrildpermissioncols", ImVec2(0, 8 * ImGui::GetFontSize() ));
	ImGui::Columns(3, "mypermissioncolumns3", false);  // 3-ways, no border

	bool permission_flag;

	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_WRITE) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Write External Storage", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_WRITE; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_WRITE;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Only needed if writing files directly to the external storage using \"raw:\" file paths on Android 9 or below");
		ImGui::EndTooltip();
	}


	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_INTERNET) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Internet/Network", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_INTERNET; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_INTERNET;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used for network or internet access.");
		ImGui::EndTooltip();
	}

	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_WAKE) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Wake Lock", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_WAKE; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_WAKE;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used to prevent the device from sleeping.");
		ImGui::EndTooltip();
	}


	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_VIBRATE) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Vibrate", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_VIBRATE; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_VIBRATE;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used to allow vibration of the device.");
		ImGui::EndTooltip();
	}

	ImGui::NextColumn();
	
	
	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_GPS) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Precise Location", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_GPS; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_GPS;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used by the GPS commands for exact location.");
		ImGui::EndTooltip();
	}

	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_IAP) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("In App Purchase", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_IAP; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_IAP;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used by the In App Purchase commands.");
		ImGui::EndTooltip();
	}

	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_LOCAL_NOTIFICATIONS) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Local Notifications", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_LOCAL_NOTIFICATIONS; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_LOCAL_NOTIFICATIONS;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Required to create local notifications on the device");
		ImGui::EndTooltip();
	}

	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_RECORD_AUDIO) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Record Audio", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_RECORD_AUDIO; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_RECORD_AUDIO;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used to allow recording from the microphone when recording the screen.");
		ImGui::EndTooltip();
	}


	ImGui::NextColumn();


	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_LOCATION) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Coarse Location", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_LOCATION; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_LOCATION;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used by the GPS commands for rough location.");
		ImGui::EndTooltip();
	}

	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_PUSH) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Push Notifications", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_PUSH; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_PUSH;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used by the Push Notification command.");
		ImGui::EndTooltip();
	}

	if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_CAMERA) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox("Camera", &permission_flag);
	if (permission_flag) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_CAMERA; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_CAMERA;
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Used by the SetDeviceCameraToImage command.");
		ImGui::EndTooltip();
	}

	//
	if ( pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_ADVERTS ) permission_flag = true; else permission_flag = false;
	ImGui::Checkbox ( "Advertising ID", &permission_flag );
	if ( permission_flag ) pCurrentSelectedProject->apk_permission_flags |= AGK_ANDROID_PERMISSION_ADVERTS; else pCurrentSelectedProject->apk_permission_flags &= ~AGK_ANDROID_PERMISSION_ADVERTS;
	if ( ImGui::IsItemHovered ( ) ) {
		ImGui::SetNextWindowContentSize ( ImVec2 ( 400, 0 ) );
		ImGui::BeginTooltip ( );
		ImGui::TextWrapped ( "If your app uses an advertising ID this box must be checked." );
		ImGui::EndTooltip ( );
	}

	ImGui::Columns(1);
	ImGui::EndChild();




	ImGui::NextColumn();


	ImGui::Indent(-10.0);
	ImGui::Text("Additional Settings (optional):");
	ImGui::Indent(10.0);

	ImGui::Text("AdMob App ID:");
	float startline = ImGui::GetCursorPos().x;

	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline+input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##admob_app_id", &pCurrentSelectedProject->apk_admob_app_id[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The AdMob App ID for this app, this can be found in the apps page of your AdMob account and should look like this \"ca-app-pub-3940256099942544~3347511713\". App IDs have the ~ character whereas Ad IDs have a forward slash");
		ImGui::EndTooltip();
	}

	ImGui::Text("SnapChat Client ID:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline+input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##snapchat_client_id", &pCurrentSelectedProject->apk_snapchat_client_id[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The Snapchat Client ID provided by your SnapChat developer account for this app. Required if you use the ShareSnapChatImage function");
		ImGui::EndTooltip();
	}

	//apk_play_app_id
	ImGui::Text("Google Game Service App ID:"); // max 30 A-Z 0-9 space underscore
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline+input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##apk_play_app_id", &pCurrentSelectedProject->apk_play_app_id[0], 20, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersDIGITS);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The 12 to 19 digit application ID assigned by Google Game Services to your app.");
		ImGui::EndTooltip();
	}


	ImGui::Text("Firebase Config File (.json):");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline+input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##apk_firebase_config_path", &pCurrentSelectedProject->apk_firebase_config_path[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The google-services.json file that you downloaded when setting up your Firebase project, this is different for every app.");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##apk_firebase_config_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "json\0*.json\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->apk_firebase_config_path, cFileSelected);
		}
	}


	ImGui::Text("URL Scheme:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##apk_url_scheme", &pCurrentSelectedProject->apk_url_scheme[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The URL scheme that can be used to open your app from a browser link, e.g. a URL scheme of \"myapp\" would allow links to myapp://sometext to open your app with \"sometext\" being available to your app through GetURLSchemeText().");
		ImGui::EndTooltip();
	}


	ImGui::Text("Deep Link:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##apk_deep_link", &pCurrentSelectedProject->apk_deep_link[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The deep link that can be used to open your app from a browser link, e.g. a deep link of \"https://www.appgamekit.com/app\" would allow all links beginning with those characters to open your app with the entire URL being available to your app through GetURLSchemeText().");
		ImGui::EndTooltip();
	}
	

	ImGui::Indent(-10.0);
	ImGui::Text("Signing (optional):");
	ImGui::Indent(10.0);
	ImGui::TextWrapped("The following must be filled out if you want an APK suitable for submitting to the Google Play, Amazon, or Ouya store. If you are submitting a Bundle to Google Play then this must be the Upload Key");

	ImGui::Text("Keystore File:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##apk_keystore_path", &pCurrentSelectedProject->apk_keystore_path[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("If you don't have this you can generate one from the Tools menu.");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##apk_keystore_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, NULL, NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->apk_keystore_path, cFileSelected);
		}
	}

	ImGui::Text("Keystore Password:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##keystore_password1", &keystore_password1[0], 255, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);


	ImGui::Text("Version Name:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##apk_version_name", &pCurrentSelectedProject->apk_version_name[0], 255, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersVERSION);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("e.g. 1.0.1");
		ImGui::EndTooltip();
	}


	ImGui::Text("Version Number:"); // max 30 A-Z 0-9 space underscore
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputInt("##apk_version_number", &pCurrentSelectedProject->apk_version_number, 0, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Must be an integer, must be higher than any value already submitted to the store.");
		ImGui::EndTooltip();
	}


	ImGui::Text("Advanced:");
	ImGui::TextWrapped("If you are providing a keystore generated by Android Studio please enter the alias name and password used to access it.");

	ImGui::Text("Alias:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##apk_alias", &pCurrentSelectedProject->apk_alias[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The name of the alias used when the keystore file was generated.");
		ImGui::EndTooltip();
	}


	ImGui::Text("Alias Password:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(startline + input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##alias_password1", &keystore_password2[0], 255, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);


	ImGui::NextColumn();

	ImGui::Indent(-10.0);
	ImGui::Text("Output:");
	ImGui::Indent(10.0);
	

	ImGui::Text("Output File Location:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	ImGui::InputText("##apk_output_path", &pCurrentSelectedProject->apk_output_path[0], 255, 0);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("The location to save the final APK file.");
		ImGui::EndTooltip();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("...##apk_output_path")) {
		char * cFileSelected;
		cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "apk\0*.apk\0", NULL, NULL);
		if (cFileSelected && strlen(cFileSelected) > 0) {
			strcpy(pCurrentSelectedProject->apk_output_path, cFileSelected);
		}
	}

	if (strlen(android_error) > 0) {
		//
//		if (ImGui::Button("Copy Error Text To Clipboard")) {
//			ImGui::SetClipboardText(android_error);
//		}
		ImGui::TextWrapped("Error: %s", android_error);
	}

	ImGui::NextColumn();

	ImGui::Indent(-10.0);
	ImGui::Text("");
	ImVec2 ws = ImGui::GetWindowSize();
	ImGui::SetCursorPos(ImVec2(ws.x - (10 * ImGui::GetFontSize()), ws.y - (2 * ImGui::GetFontSize()) + ImGui::GetScrollY()));
	if (ImGui::Button(" Cancel ")) {
		show_androidexport_window = false;
	}
	ImGui::SameLine();
	if (ImGui::Button(" Export "))
	{
	  if ( AndroidExportHTTP > 0 )
	  {
		BoxerInfo( "Android export files are still downloading, please wait and try again", "Android export files downloading" );
	  }
	  else
	  {
		//Save new preferences before exporting.
		SaveProjectFile(pCurrentSelectedProject);

		PleaseWait();

		int includeFirebase;
		int includePushNotify;
		int includeGooglePlay;
		int includeAdMob;

		int orientation = 10;
		if (pCurrentSelectedProject->apk_orientation == 0) orientation = 6;
		else if (pCurrentSelectedProject->apk_orientation == 1) orientation = 7;
		char szOrientation[20];
		sprintf(szOrientation, "%d", orientation);

		int sdk = 16;
		if (pCurrentSelectedProject->apk_sdk_version == 0) sdk = 16;
		if (pCurrentSelectedProject->apk_sdk_version == 1) sdk = 17;
		if (pCurrentSelectedProject->apk_sdk_version == 2) sdk = 18;
		if (pCurrentSelectedProject->apk_sdk_version == 3) sdk = 19;
		if (pCurrentSelectedProject->apk_sdk_version == 4) sdk = 21; // 20 deliberately missing
		if (pCurrentSelectedProject->apk_sdk_version == 5) sdk = 22;
		if (pCurrentSelectedProject->apk_sdk_version == 6) sdk = 23;
		if (pCurrentSelectedProject->apk_sdk_version == 7) sdk = 24;
		if (pCurrentSelectedProject->apk_sdk_version == 8) sdk = 25;
		if (pCurrentSelectedProject->apk_sdk_version == 9) sdk = 26;
		if (pCurrentSelectedProject->apk_sdk_version == 10) sdk = 27;
		if (pCurrentSelectedProject->apk_sdk_version == 11) sdk = 28;
		if (pCurrentSelectedProject->apk_sdk_version == 12) sdk = 29;
		if (pCurrentSelectedProject->apk_sdk_version == 13) sdk = 30;
		if (pCurrentSelectedProject->apk_sdk_version == 14) sdk = 31;
		if (pCurrentSelectedProject->apk_sdk_version == 15) sdk = 33; // 32 deliberately missing
		if (pCurrentSelectedProject->apk_sdk_version == 16) sdk = 34;
		char szSDK[20];
		sprintf(szSDK, "%d", sdk);

		if (strlen(pCurrentSelectedProject->apk_version_name) == 0) { //*version_number
			strcpy(pCurrentSelectedProject->apk_version_name, "1.0.0");
		}

		if (pCurrentSelectedProject->apk_version_number == 0) pCurrentSelectedProject->apk_version_number = 1;
		char szBuildNum[20];
		sprintf(szBuildNum, "%d", pCurrentSelectedProject->apk_version_number);

		//apk_output_path
		char output_file[MAX_PATH];
		strcpy(output_file, pCurrentSelectedProject->apk_output_path);
		char *percent = 0;
		while ((percent = strchr(&output_file[0], '%')) != 0)
		{
			char tmp[MAX_PATH]; //PE: We can crash without this.
			if (strncmp(percent + 1, "[version]", strlen("[version]")) == 0)
			{
				*percent = 0;
				percent += strlen("[version]") + 1;
				strcpy(tmp, percent);
//				strcat(output_file, pCurrentSelectedProject->apk_version_name);
				strcat(output_file, szBuildNum); //From Mike change to buildnum.
				strcat(output_file, tmp);
				continue;
			}
			if (strncmp(percent + 1, "[type]", strlen("[type]")) == 0)
			{
				*percent = 0;
				percent += strlen("[type]") + 1;
				strcpy(tmp, percent);
				strcat(output_file, apk_type[pCurrentSelectedProject->apk_app_type] );
				//PE: got crash here, as percent is also overwriting when strcat to output_file, so perent < output_file
				strcat(output_file, tmp);
				
				continue;
			}
			break;
		}

		bool Valid = true;
		if(strlen(output_file) < 2 ) {	strcpy(android_error, "You must choose an output location to save your APK"); Valid = false; }

		const char* desiredExt = ".apk";
		if ( bIsBundle ) desiredExt = ".aab";

		char* ext = strrchr( output_file, '.' );
		if ( ext && strlen(ext) < 6 ) strcpy( ext, desiredExt );
		else strcat( output_file, desiredExt );
		
		if (Valid && strchr(output_file, '.') == NULL ) { strcpy(android_error, "The output location must be a file not a directory"); Valid = false; }
		if (Valid && strlen(pCurrentSelectedProject->apk_app_name) < 1 ) { strcpy(android_error, "You must enter an app name"); Valid = false; }
		if (Valid && strlen(pCurrentSelectedProject->apk_package_name) < 1) { strcpy(android_error, "You must enter a package name"); Valid = false; }
		if (Valid && strchr(pCurrentSelectedProject->apk_package_name, '.') == NULL) { strcpy(android_error, "Package name must contain at least one dot character"); Valid = false; }
		if (Valid && (pCurrentSelectedProject->apk_package_name[0] < 65 || pCurrentSelectedProject->apk_package_name[0] > 90) && (pCurrentSelectedProject->apk_package_name[0] < 97 || pCurrentSelectedProject->apk_package_name[0] > 122) ) { strcpy(android_error, "Package name must begin with a letter"); Valid = false; }
		if (Valid && pCurrentSelectedProject->apk_package_name[strlen(pCurrentSelectedProject->apk_package_name) - 1] == '.' ) { strcpy(android_error, "Package name must not end with a dot"); Valid = false; }
		
		if (file_exists(output_file)) {
			Valid = overWriteFileBox((char*) &output_file[0]);
		}
		
		if (Valid) {
			// delete any old file
			remove(output_file);

			char last = 0;
			for (int i = 0; i < strlen(pCurrentSelectedProject->apk_package_name); i++)
			{
				if (last == '.' && (pCurrentSelectedProject->apk_package_name[i] < 65 || pCurrentSelectedProject->apk_package_name[i] > 90) && (pCurrentSelectedProject->apk_package_name[i] < 97 || pCurrentSelectedProject->apk_package_name[i] > 122))
				{
					strcpy(android_error, "Package name invalid, a dot must be followed by a letter");
					Valid = false;
					break;
				}
				last = pCurrentSelectedProject->apk_package_name[i];
			}
		}

		if (Valid && strlen(pCurrentSelectedProject->apk_url_scheme) > 0) {
			if (strchr(pCurrentSelectedProject->apk_url_scheme, ':') || strchr(pCurrentSelectedProject->apk_url_scheme, '/'))
			{
				strcpy(android_error, "URL scheme must not contain : or /");
				Valid = false;
			}
		}

		if ( Valid && strlen(pCurrentSelectedProject->apk_deep_link) > 0)
		{
			if (strncmp(pCurrentSelectedProject->apk_deep_link, "https://", strlen("https://")) != 0 && strncmp(pCurrentSelectedProject->apk_deep_link, "http://", strlen("http://")) != 0)
			{
				strcpy(android_error, "Deep link must start with http:// or https://");
				Valid = false;
			}

			if (Valid && (strcmp(pCurrentSelectedProject->apk_deep_link, "https://") == 0 || strcmp(pCurrentSelectedProject->apk_deep_link, "http://") == 0) )
			{
				strcpy(android_error, "Deep link must have a domain after http:// or https://");
				Valid = false;
			}
		}

		if ( Valid && strlen(pCurrentSelectedProject->apk_icon_path) > 0)
		{
			if (pestrcasestr(pCurrentSelectedProject->apk_icon_path, ".png") == NULL) {
				strcpy(android_error, "App legacy icon must be a PNG file");
				Valid = false;
			}
			if (Valid && !file_exists(pCurrentSelectedProject->apk_icon_path)) {
				strcpy(android_error, "Could not find app legacy icon location");
				Valid = false;
			}
		}

		if ( Valid && strlen(pCurrentSelectedProject->apk_new_icon_path) > 0)
		{
			if (pestrcasestr(pCurrentSelectedProject->apk_new_icon_path, ".png") == NULL) {
				strcpy(android_error, "App icon must be a PNG file");
				Valid = false;
			}
			if (Valid && !file_exists(pCurrentSelectedProject->apk_new_icon_path)) {
				strcpy(android_error, "Could not find app icon location");
				Valid = false;
			}
		}

		if (Valid && strlen(pCurrentSelectedProject->apk_notif_icon_path) > 0)
		{
			if (pestrcasestr(pCurrentSelectedProject->apk_notif_icon_path, ".png") == NULL) {
				strcpy(android_error, "Notification icon must be a PNG file");
				Valid = false;
			}
			if (Valid && !file_exists(pCurrentSelectedProject->apk_notif_icon_path)) {
				strcpy(android_error, "Could not find notification icon location");
				Valid = false;
			}
		}

		if (Valid && bIsOuya)
		{
			if ( *(pCurrentSelectedProject->apk_ouya_icon_path) )
			{
				if ( !file_exists(pCurrentSelectedProject->apk_ouya_icon_path) ) 
				{
					strcpy(android_error, "Could not find ouya large icon location");
					Valid = false;
				}

				if (Valid && pestrcasestr(pCurrentSelectedProject->apk_ouya_icon_path, ".png") == NULL) 
				{
					strcpy(android_error, "Ouya large icon must be a PNG file");
					Valid = false;
				}
			}
		}

		if( Valid && strlen(pCurrentSelectedProject->apk_firebase_config_path ) > 0 )
		{

			if (Valid && pestrcasestr(pCurrentSelectedProject->apk_firebase_config_path, ".json") == NULL) {
				strcpy(android_error, "Google services config file must be a .json file");
				Valid = false;
			}
			if (Valid && !file_exists(pCurrentSelectedProject->apk_firebase_config_path)) {
				strcpy(android_error, "Could not find Google services config file");
				Valid = false;
			}
		}

		// check keystore
		if (Valid && strlen(pCurrentSelectedProject->apk_keystore_path) > 0 )
		{
			if (Valid && !file_exists(pCurrentSelectedProject->apk_keystore_path)) {
				strcpy(android_error, "Could not find keystore file location");
				Valid = false;
			}
			if (Valid && strlen(keystore_password1) < 1) {
				strcpy(android_error, "You must enter your keystore password when using your own keystore");
				Valid = false;
			}
			if (Valid && strchr(keystore_password1, '"') ) {
				strcpy(android_error, "Keystore password cannot contain double quotes");
				Valid = false;
			}
		}

		// check alias
		if (Valid && strlen(pCurrentSelectedProject->apk_alias) > 0)
		{
			if (Valid && strlen(keystore_password2) < 1) {
				strcpy(android_error, "You must enter your alias password when using a custom alias");
				Valid = false;
			}
			if (Valid && strchr(keystore_password2, '"')) {
				strcpy(android_error, "Alias password cannot contain double quotes");
				Valid = false;
			}
		}

		if (Valid && strlen(pCurrentSelectedProject->apk_admob_app_id) > 0)
		{
			if (Valid && strchr(pCurrentSelectedProject->apk_admob_app_id, '~') == 0) {
				strcpy(android_error, "Admob App ID should contain a ~ character");
				Valid = false;
			}
		}

		if (Valid) {
			includeFirebase = (strlen(pCurrentSelectedProject->apk_firebase_config_path) > 0 && !bIsOuya) ? 1 : 0;
			includePushNotify = ((pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_PUSH) && bIsGoogle) ? 1 : 0;
			includeGooglePlay = (strlen(pCurrentSelectedProject->apk_play_app_id) > 0 && bIsGoogle) ? 1 : 0;
			includeAdMob = (strlen(pCurrentSelectedProject->apk_admob_app_id) > 0 && bIsGoogle && bUseAds) ? 1 : 0;

			if (includePushNotify && !includeFirebase)
			{
				strcpy(android_error, "Push Notifications on Android now use Firebase, so you must include a Firebase config file to use them");
				Valid = false;
			}
		}


		if (Valid) {
			char curDir[MAX_PATH];
			extern char startupFolder[MAX_PATH];
			//const char* androidJar = "android33.jar";
			const char* androidJar = "android34.jar";

#if defined(AGK_WINDOWS)
			_getcwd(&curDir[0], MAX_PATH);
			_chdir(startupFolder);

			uString path_to_aapt2 = startupFolder;
			uString path_to_bundletool = startupFolder;
			uString path_to_android_jar = startupFolder;
			uString path_to_apksigner = startupFolder;
			uString path_to_zipalign = startupFolder;
			
			// everything now uses 34.0.0
			path_to_aapt2.Append("/media/data/android/aapt2-bundle.exe");
			path_to_bundletool.Append("/media/data/android/bundletool.jar");
			path_to_android_jar.Append("/media/data/android/");
			path_to_android_jar.Append( androidJar );
			path_to_apksigner.Append("/media/data/android/apksigner.jar");
			path_to_zipalign.Append("/media/data/android/zipalign.exe");
			
			// convert forward slashes to backward slashes for parameters that will be passed to aapt2
			path_to_android_jar.ReplaceStr("/", "\\");
			char *pathPtr = &output_file[0];
			while (*pathPtr) { if (*pathPtr == '/') *pathPtr = '\\'; pathPtr++; }

			uString android_folder = startupFolder;
			android_folder.Append("/media/data/android");

			uString sWhichCmd = "where";
			
#elif defined(AGK_MACOS)
			uString path_to_aapt2 = defaultWriteFolder;
			//uString path_to_bundletool = startupFolder;
			uString path_to_bundletool = defaultWriteFolder;
			uString path_to_android_jar = defaultWriteFolder;
			uString path_to_apksigner = defaultWriteFolder;
			uString path_to_zipalign = defaultWriteFolder;

			path_to_aapt2.Append("AndroidExport/aapt2-bundle");
			path_to_bundletool.Append("AndroidExport/bundletool.jar");
			path_to_android_jar.Append("AndroidExport/");
			path_to_android_jar.Append( androidJar );
			path_to_apksigner.Append("AndroidExport/apksigner.jar");
			path_to_zipalign.Append("AndroidExport/zipalign");
            
            chmod( path_to_aapt2, 0755 );
            chmod( path_to_zipalign, 0755 );

			uString android_folder = defaultWriteFolder;
			android_folder.AppendUTF8( "AndroidExport" );

			uString sWhichCmd = "which";
#else
			getcwd(&curDir[0], MAX_PATH);
			chdir(startupFolder);
			uString path_to_aapt2 = startupFolder;
			uString path_to_bundletool = startupFolder;
			uString path_to_android_jar = startupFolder;
			uString path_to_apksigner = startupFolder;
			uString path_to_zipalign = startupFolder;

			path_to_aapt2.Append("/media/data/android/aapt2-bundle");
			path_to_bundletool.Append("/media/data/android/bundletool.jar");
			path_to_android_jar.Append("/media/data/android/");
			path_to_android_jar.Append( androidJar );
			path_to_apksigner.Append("/media/data/android/apksigner.jar");
			path_to_zipalign.Append("/media/data/android/zipalign");

			uString android_folder = startupFolder;
			android_folder.Append("/media/data/android");

			uString sWhichCmd = "which";
#endif

			#ifdef DEBUG_EXPORT
				agk::Message ( uString ( "aapt2: " ) += path_to_aapt2 );
				agk::Message ( uString ( "bundletool: " ) += path_to_bundletool );
				agk::Message ( uString ( "android jar: " ) += path_to_android_jar );
				agk::Message ( uString ( "apk signer: " ) += path_to_apksigner );
				agk::Message ( uString ( "zip align: " ) += path_to_zipalign );
			#endif

			int iStdOutLength, iStdErrLength;
			uString java_path;
			uString jarsigner_path;

			// check for java
			int appid = run_command(sWhichCmd, "java", &iStdOutLength, &iStdErrLength, false);
			if (appid == false) 
			{
				strcpy(android_error, "Failed to check for java");
				Valid = false;
			}
			if (Valid)
			{
				float timestart = agk::Timer();
				do
				{
					agk::Sleep( 100 );
				} while (agk::GetAppRunning(appid) == 1 && agk::Timer() - timestart < 10.0);
				
				if ( agk::GetAppRunning(appid) )
				{
					strcpy(android_error, "Java check timed out");
					Valid = false;
					agk::TerminateApp(appid);
				}
				else
				{
					char tmp[1024] = { 0 };
					tmp[0] = 0;
					tmp[1] = 0;
					int rs = read_buffers(tmp, 1024 - 90, NULL, 0);

#ifdef AGK_WINDOWS
					if ( tmp[1] == ':')
#else
					if ( tmp[0] == '/' )
#endif
					{
						java_path = "java";
					}
				}
			}

			if (Valid)
			{
				// check for jarsigner
				appid = run_command(sWhichCmd, "jarsigner", &iStdOutLength, &iStdErrLength, false);
				if (appid == false) 
				{
					strcpy(android_error, "Failed to check for Jarsigner");
					Valid = false;
				}
				if (Valid)
				{
					float timestart = agk::Timer();
					do
					{
						agk::Sleep( 100 );
					} while (agk::GetAppRunning(appid) == 1 && agk::Timer() - timestart < 10.0);
					
					if ( agk::GetAppRunning(appid) )
					{
						strcpy( android_error, "Jarsigner check timed out" );
						Valid = false;
						agk::TerminateApp(appid);
					}
					else
					{
						char tmp[1024] = { 0 };
						tmp[0] = 0;
						tmp[1] = 0;
						int rs = read_buffers(tmp, 1024 - 90, NULL, 0);

#ifdef AGK_WINDOWS
						if ( tmp[1] == ':' )
#else
						if ( tmp[0] == '/' )
#endif
						{
							jarsigner_path = "jarsigner";
						}
					}
				}
			}

			if ( Valid && java_path.GetLength() == 0 )
			{
				const char* java_home = getenv( "JAVA_HOME" );
				if ( !java_home )
				{
					strcpy(android_error, "Could not find Java, make sure you have the Java Development Kit (JDK) 8 or above installed. If it is installed then make sure that the JAVA_HOME environment variable is defined or that the Java bin folder is in your PATH environment variable" );
					Valid = false;
				}
				else
				{
					java_path.SetStr( java_home );
					java_path.Replace( '\\', '/' );
					if ( java_path.CharAt( java_path.GetNumChars()-1 ) != '/' ) java_path.Append( "/" );
					java_path.Append( "bin/" );

	#if defined(AGK_WINDOWS)
					java_path.Append( "java.exe" );
	#else 
					java_path.Append( "java" );
	#endif

					uString java_path_raw = "raw:";
					java_path_raw.Append( java_path );
					if ( !agk::GetFileExists(java_path_raw.GetStr()) )
					{
						sprintf(android_error, "Could not find java program, expected to find it in \"%s\", make sure your JAVA_HOME environment variable is correct", java_path.GetStr() );
						Valid = false;
					}
				}
			}

			if ( Valid && jarsigner_path.GetLength() == 0 )
			{
				const char* java_home = getenv( "JAVA_HOME" );
				if ( !java_home )
				{
					strcpy(android_error, "Could not find Jarsigner, make sure you have the Java Development Kit (JDK) 8 or above installed. If it is installed then make sure that the JAVA_HOME environment variable is defined or that the Java bin folder is in your PATH environment variable" );
					Valid = false;
				}
				else
				{
					jarsigner_path.SetStr( java_home );
					jarsigner_path.Replace( '\\', '/' );
					if ( jarsigner_path.CharAt( jarsigner_path.GetNumChars()-1 ) != '/' ) jarsigner_path.Append( "/" );
					jarsigner_path.Append( "bin/" );

	#if defined(AGK_WINDOWS)
					jarsigner_path.Append( "jarsigner.exe" );
	#else 
					jarsigner_path.Append( "jarsigner" );
	#endif

					uString jarsigner_path_raw = "raw:";
					jarsigner_path_raw.Append( jarsigner_path );
					if ( !agk::GetFileExists(jarsigner_path_raw.GetStr()) )
					{
						sprintf(android_error, "Could not find Jarsigner program, expected to find it in \"%s\", make sure your JAVA_HOME environment variable is correct", jarsigner_path.GetStr() );
						Valid = false;
					}
				}
			}
			
			// make temporary folder
			extern char defaultWriteFolder[MAX_PATH];
			//sprintf(cIniFile, "%sagkide.ini", defaultWriteFolder);

			uString tmp_folder = defaultWriteFolder;
			tmp_folder.Append("build_tmp");

			android_folder.ReplaceStr("\\", "/");
			tmp_folder.ReplaceStr("\\", "/");

			//PE: Make sure tmp folder is empty before we start.
			if (strlen(defaultWriteFolder) > 4) { //Just in case :)
				utils_remove_folder_recursive(tmp_folder.GetStr());
			}

			uString src_folder = android_folder;

			if ( pCurrentSelectedProject->apk_app_type == 0 || pCurrentSelectedProject->apk_app_type == 1 ) src_folder.Append("/sourceGoogle");  //= g_build_path("/", app->datadir, "android", "sourceGoogle", NULL);
			else if (pCurrentSelectedProject->apk_app_type == 2) src_folder.Append("/sourceAmazon"); // = g_build_path("/", app->datadir, "android", "sourceAmazon", NULL);
			else if (pCurrentSelectedProject->apk_app_type == 3) src_folder.Append("/sourceOuya"); // = g_build_path("/", app->datadir, "android", "sourceOuya", NULL);
			//else if ( pCurrentSelectedProject->apk_app_type == 4 ) src_folder.Append("/sourceGoogleNoAds");
			else if ( pCurrentSelectedProject->apk_app_type == DTSAPKIndex || pCurrentSelectedProject->apk_app_type == DTSBundleIndex ) src_folder.Append("/sourceGoogleDTS");
			else
			{
				strcpy(android_error, "Unrecognised APK Type");
				Valid = false;
			}
			
			src_folder.ReplaceStr("\\", "/");

			char output_file_zip[MAX_PATH];

			//CreateZip will fail if using d:\\name and not d:\
							
			uString tmp;
			tmp.SetStr(output_file);
			tmp.ReplaceStr(":\\\\", ":\\");
			strcpy(output_file, tmp.GetStr());


			strcpy(output_file_zip , output_file);
			char *ext = strrchr(&output_file_zip[0], '.');
			if ( ext && strlen(ext) < 6 ) strcpy(ext, ".zip");
			else strcat( output_file_zip, ".zip" );

			uString keystore_file = pCurrentSelectedProject->apk_keystore_path;
			uString keystore_password = keystore_password1;
			uString alias_name = pCurrentSelectedProject->apk_alias;
			uString alias_password = keystore_password2;

			if ( strlen(pCurrentSelectedProject->apk_keystore_path) == 0 )
			{

				keystore_file = android_folder;
				keystore_file.Append("/debug.keystore");
				keystore_password = "android";

				alias_name = "androiddebugkey";
				alias_password = "android";
			}
			else
			{
				if ( strlen(pCurrentSelectedProject->apk_alias) == 0 )
				{
					alias_name = "mykeystore";
					alias_password = keystore_password1;
				}
			}


			//#####################
			//#### Direct copy ####
			//#####################

#define AGK_NEW_CONTENTS_SIZE 1000000

			// declarations
			char *newcontents = new char[AGK_NEW_CONTENTS_SIZE];
			char *newcontents2 = new char[AGK_NEW_CONTENTS_SIZE];
			//char* manifest_file = NULL;
			char *contents = NULL;
			char *contents2 = NULL;
			char *contents3 = NULL;
			char *contentsOther = NULL;
			char *contentsOther2 = NULL;
			char *contentsOther3 = NULL;
			int length = 0;
			char* resources_file = NULL;
			//GError *error = NULL;
			//GdkPixbuf *icon_image = NULL;
			char *image_filename = NULL;
			//GdkPixbuf *icon_scaled_image = NULL;
			char **argv = NULL;
			char **argv2 = NULL;
			char **argv3 = NULL;
			int status = 0;
			//mz_zip_archive zip_archive;
			//memset(&zip_archive, 0, sizeof(zip_archive));
			char *zip_add_file = 0;
			char *str_out = NULL;
			int resLength = 0;
			int package_count = 0;
			int package_index = 0;

#ifdef AGK_LINUX
			//DebugInfo( (char *) src_folder.GetStr(), "Copy Folder From");
			//DebugInfo( (char *) tmp_folder.GetStr(), "Copy Folder To");
#endif

			if (!CopyDirTo(src_folder.GetStr(), tmp_folder.GetStr()))
			{
				strcpy(android_error, "Failed to copy source folder");
				Valid = false;
#ifdef AGK_LINUX
				//DebugInfo("FAILED", "Copy Folder");
#endif
			}

#ifdef AGK_LINUX
			//DebugInfo("OK", "Copy Folder");
#endif

			if (Valid) {

				//#############################
				//#### AndroidManifest.xml ####
				//#############################

				uString manifest_file = tmp_folder;
				manifest_file.Append("/AndroidManifest.xml");
				//Must Free "contents"
				contents = g_file_get_contents( (char *) manifest_file.GetStr(), contents, &length, NULL);
				if (!contents || length == 0) {
					//error
					strcpy(android_error, "Failed to read AndroidManifest.xml file");
					Valid = false;
				}
				if (Valid) {
					strcpy(newcontents, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\
<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n\
      android:versionCode=\"");
					strcat(newcontents, szBuildNum);
					strcat(newcontents, "\"\n      android:versionName=\"");
					strcat(newcontents, pCurrentSelectedProject->apk_version_name);
					strcat(newcontents, "\" package=\"");
					strcat(newcontents, pCurrentSelectedProject->apk_package_name);
					strcat(newcontents, "\"");
					strcat(newcontents, " android:installLocation=\"auto\">\n\
    <uses-feature android:glEsVersion=\"0x00020000\"></uses-feature>\n\
    <uses-sdk android:minSdkVersion=\"");

					strcat(newcontents, szSDK);
					
					strcat(newcontents, "\" android:targetSdkVersion=\"");
					if ( bIsOuya ) strcat(newcontents, "16");
					//else strcat ( newcontents, "33" );
					else strcat(newcontents, "34");
					strcat(newcontents, "\" />\n\n");


					if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_WRITE) strcat(newcontents, "    <uses-permission android:name=\"android.permission.WRITE_EXTERNAL_STORAGE\"></uses-permission>\n");
					if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_INTERNET)
					{
						strcat(newcontents, "    <uses-permission android:name=\"android.permission.INTERNET\"></uses-permission>\n");
						strcat(newcontents, "    <uses-permission android:name=\"android.permission.ACCESS_NETWORK_STATE\"></uses-permission>\n");
						strcat(newcontents, "    <uses-permission android:name=\"android.permission.ACCESS_WIFI_STATE\"></uses-permission>\n");
					}
					if (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_WAKE) strcat(newcontents, "    <uses-permission android:name=\"android.permission.WAKE_LOCK\"></uses-permission>\n");
					
					if ( bIsGoogle )
					{
						bool finelocation = (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_GPS) != 0;
						bool coarselocation = (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_LOCATION) != 0;
						bool iap = (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_IAP) != 0;
						bool pushnotify = (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_PUSH) != 0;
						bool localnotify = (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_LOCAL_NOTIFICATIONS) != 0;

						// location
						if ( finelocation ) strcat(newcontents, "    <uses-permission android:name=\"android.permission.ACCESS_FINE_LOCATION\" />\n");
						if ( finelocation || coarselocation ) strcat(newcontents, "    <uses-permission android:name=\"android.permission.ACCESS_COARSE_LOCATION\" />\n"); 

						if ( iap ) strcat(newcontents, "    <uses-permission android:name=\"com.android.vending.BILLING\"></uses-permission>\n");

						if ( (strlen(pCurrentSelectedProject->apk_play_app_id) > 0) || pushnotify ) strcat(newcontents, "    <uses-permission android:name=\"com.google.android.c2dm.permission.RECEIVE\" />\n");

						// push and local notifications
						if ((pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_PUSH))
						{
							strcat(newcontents, "    <permission android:name=\"");
							strcat(newcontents, pCurrentSelectedProject->apk_package_name);
							strcat(newcontents, ".permission.C2D_MESSAGE\" android:protectionLevel=\"signature\" />\n");
							strcat(newcontents, "    <uses-permission android:name=\"");
							strcat(newcontents, pCurrentSelectedProject->apk_package_name);
							strcat(newcontents, ".permission.C2D_MESSAGE\" />\n");
						}

						if ( pushnotify || localnotify ) strcat(newcontents, "<uses-permission android:name=\"android.permission.POST_NOTIFICATIONS\" />\n" );
					}
					
					if ((pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_CAMERA)) strcat(newcontents, "    <uses-permission android:name=\"android.permission.CAMERA\"></uses-permission>\n");
					if ((pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_VIBRATE)) strcat(newcontents, "    <uses-permission android:name=\"android.permission.VIBRATE\"></uses-permission>\n");
					if ((pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_RECORD_AUDIO)) strcat(newcontents, "    <uses-permission android:name=\"android.permission.RECORD_AUDIO\"></uses-permission>\n");

					if ( ( pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_ADVERTS ) ) strcat ( newcontents, "    <uses-permission android:name=\"com.google.android.gms.permission.AD_ID\"/>\n" );
					
					// mike - this is needed for tracking
					strcat ( newcontents, "    <uses-permission android:name=\"com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE\"/>\n" );

					/*<uses-permission android:name="com.google.android.gms.permission.AD_ID" /> Required on Android 13*/

					// supports FireTV
					if (0)
					{
						strcat(newcontents, "    <uses-feature android:name=\"android.hardware.touchscreen\" android:required=\"false\" />\n");
					}

					// if ARCore required
					if (pCurrentSelectedProject->apk_arcore == 2)
					{
						strcat(newcontents, "    <uses-feature android:name=\"android.hardware.camera.ar\" android:required=\"true\" />");
					}

					contents2 = contents;
					contents3 = 0;

					// the order of these relacements is important, they must occur in the same order as they occur in the file

					// package queries
					contents3 = strstr(contents2, "<!--ADDITIONAL_QUERIES-->");
					if (contents3)
					{
						*contents3 = 0;
						contents3 += strlen("<!--ADDITIONAL_QUERIES-->");

						strcat(newcontents, contents2);
					
						if ( *pCurrentSelectedProject->apk_snapchat_client_id )
						{
							strcat(newcontents, "<package android:name=\"com.snapchat.android\" />\n" );
						}

						if (pCurrentSelectedProject->apk_arcore > 0)
						{
							strcat(newcontents, "<package android:name=\"com.google.ar.core\" />\n" );
						}

						contents2 = contents3;
					}

					// GOOGLE_PLAY_APPLICATION_ID is now already filled in

					// replace orientation
					contents3 = strstr(contents2, "screenOrientation=\"fullSensor\"");
					if (contents3)
					{
						*contents3 = 0;
						contents3 += strlen("screenOrientation=\"fullSensor");

						strcat(newcontents, contents2);

						switch (orientation)
						{
						case 6: strcat(newcontents, "screenOrientation=\"sensorLandscape"); break;
						case 7:
						{
							// all now use API 23 with correct spelling
							strcat(newcontents, "screenOrientation=\"sensorPortrait");
							break;
						}
						default: strcat(newcontents, "screenOrientation=\"fullSensor"); break;
						}

						contents2 = contents3;
					}

					// add intent filters
					contents3 = strstr(contents2, "<!--ADDITIONAL_INTENT_FILTERS-->");
					if (contents3)
					{
						*contents3 = 0;
						contents3 += strlen("<!--ADDITIONAL_INTENT_FILTERS-->");

						strcat(newcontents, contents2);

						if (strlen(pCurrentSelectedProject->apk_url_scheme) > 0)
						{
							strcat(newcontents, "<intent-filter android:autoVerify=\"true\">\n\
			<action android:name=\"android.intent.action.VIEW\" />\n\
			<category android:name=\"android.intent.category.DEFAULT\" />\n\
			<category android:name=\"android.intent.category.BROWSABLE\" />\n\
			<data android:scheme=\"");

							strcat(newcontents, pCurrentSelectedProject->apk_url_scheme);
							strcat(newcontents, "\" />\n    </intent-filter>\n");
						}

						if (strlen(pCurrentSelectedProject->apk_deep_link) > 0)
						{
							char *szScheme = 0;
							char *szHost = 0;
							char *szPath = 0;
							char *szTemp = strstr(pCurrentSelectedProject->apk_deep_link, "://");
							if (szTemp)
							{
								*szTemp = 0;
								szScheme = strdup(pCurrentSelectedProject->apk_deep_link);
								*szTemp = ':';

								szTemp += 3;
								char *szTemp2 = strstr(szTemp, "/");
								if (szTemp2)
								{
									szPath = strdup(szTemp2);
									*szTemp2 = 0;
									szHost = strdup(szTemp);
									*szTemp2 = '/';
								}
								else szHost = strdup(szTemp);
							}

							if (szScheme && *szScheme)
							{
								strcat(newcontents, "<intent-filter android:autoVerify=\"true\">\n\
			<action android:name=\"android.intent.action.VIEW\" />\n\
			<category android:name=\"android.intent.category.DEFAULT\" />\n\
			<category android:name=\"android.intent.category.BROWSABLE\" />\n\
			<data android:scheme=\"");

								strcat(newcontents, szScheme);
								strcat(newcontents, "\" />\n");
							}

							if (szHost && *szHost)
							{
								strcat(newcontents, "<data android:host=\"");
								strcat(newcontents, szHost);
								
								if (szPath && *szPath)
								{
									strcat(newcontents, "\" android:pathPrefix=\"");
									strcat(newcontents, szPath);
								}

								strcat(newcontents, "\" />\n");
							}

							strcat(newcontents, "</intent-filter>\n");

							if (szScheme) free(szScheme);
							if (szHost) free(szHost);
							if (szPath) free(szPath);
						}

						contents2 = contents3;
					}

					// replace package name
					contents3 = strstr(contents2, "YOUR_PACKAGE_NAME_HERE");
					if (contents3)
					{
						*contents3 = 0;
						contents3 += strlen("YOUR_PACKAGE_NAME_HERE");

						strcat(newcontents, contents2);
						strcat(newcontents, pCurrentSelectedProject->apk_package_name);
						contents2 = contents3;
					}

					// replace application ID
					contents3 = strstr(contents2, "${applicationId}");
					while (contents3)
					{
						*contents3 = 0;
						contents3 += strlen("${applicationId}");

						strcat(newcontents, contents2);
						strcat(newcontents, pCurrentSelectedProject->apk_package_name);
						contents2 = contents3;
						contents3 = strstr(contents2, "${applicationId}");
					}

					// write the rest of the manifest file
					strcat(newcontents, contents2);

					if ( (pCurrentSelectedProject->apk_permission_flags & AGK_ANDROID_PERMISSION_IAP) && bIsGoogle )
					{
						strcat(newcontents, "\n\
						<meta-data\n\
							android:name=\"com.google.android.play.billingclient.version\"\n\
							android:value=\"5.2.1\" />\n\
						<activity\n\
							android:name=\"com.android.billingclient.api.ProxyBillingActivity\"\n\
							android:configChanges=\"keyboard|keyboardHidden|screenLayout|screenSize|orientation\"\n\
							android:exported=\"false\"\n\
							android:theme=\"@android:style/Theme.Translucent.NoTitleBar\" />\n");
					}

					// Google sign in
					if (bIsGoogle)
					{
						strcat(newcontents, "\n\
		<activity android:name=\"com.google.android.gms.auth.api.signin.internal.SignInHubActivity\"\n\
            android:excludeFromRecents=\"true\"\n\
            android:exported=\"false\"\n\
            android:theme=\"@android:style/Theme.Translucent.NoTitleBar\" />\n\
        <service android:name=\"com.google.android.gms.auth.api.signin.RevocationBoundService\"\n\
            android:exported=\"true\"\n\
            android:permission=\"com.google.android.gms.auth.api.signin.permission.REVOCATION_NOTIFICATION\" />\n");
					}

					
					// Google API Activity - for Game Services
					if (includeGooglePlay)
					{
						strcat(newcontents, "\n\
        <activity android:name=\"com.google.android.gms.common.api.GoogleApiActivity\" \n\
                  android:exported=\"false\" \n\
                  android:theme=\"@android:style/Theme.Translucent.NoTitleBar\" />");
					}

					// Firebase Init Provider - for Game Services and Firebase
					if (includeGooglePlay || includeFirebase || includePushNotify)
					{
						strcat(newcontents, "\n        <provider android:authorities=\"");
						strcat(newcontents, pCurrentSelectedProject->apk_package_name);
						strcat(newcontents, ".firebaseinitprovider\"\n\
                  android:name=\"com.google.firebase.provider.FirebaseInitProvider\"\n\
                  android:directBootAware=\"true\"\n\
                  android:exported=\"false\"\n\
                  android:initOrder=\"100\" />\n");
					}

					// Firebase activities
					if (includeFirebase)
					{
						strcat(newcontents, "\n\
        <receiver\n\
            android:name=\"com.google.android.gms.measurement.AppMeasurementReceiver\"\n\
            android:enabled=\"true\"\n\
            android:exported=\"false\" >\n\
        </receiver>\n\
\n\
        <service android:name=\"com.google.android.gms.measurement.AppMeasurementService\"\n\
                 android:enabled=\"true\"\n\
                 android:exported=\"false\"/>\n\
        <service\n\
            android:name=\"com.google.android.gms.measurement.AppMeasurementJobService\"\n\
            android:enabled=\"true\"\n\
            android:exported=\"false\"\n\
            android:permission=\"android.permission.BIND_JOB_SERVICE\" />\n\
		<service\n\
            android:name=\"com.google.firebase.components.ComponentDiscoveryService\"\n\
			android:directBootAware=\"true\"\n\
            android:exported=\"false\" >\n\
			<meta-data\n\
                android:name=\"com.google.firebase.components:com.google.firebase.messaging.FirebaseMessagingRegistrar\"\n\
                android:value=\"com.google.firebase.components.ComponentRegistrar\" />\n\
            <meta-data\n\
                android:name=\"com.google.firebase.components:com.google.firebase.analytics.connector.internal.AnalyticsConnectorRegistrar\"\n\
                android:value=\"com.google.firebase.components.ComponentRegistrar\" />\n\
            <meta-data\n\
                android:name=\"com.google.firebase.components:com.google.firebase.datatransport.TransportRegistrar\"\n\
                android:value=\"com.google.firebase.components.ComponentRegistrar\" />\n\
            <meta-data\n\
                android:name=\"com.google.firebase.components:com.google.firebase.installations.FirebaseInstallationsRegistrar\"\n\
                android:value=\"com.google.firebase.components.ComponentRegistrar\" />\n\
        </service>\n\
		<service\
            android:name=\"com.google.android.datatransport.runtime.backends.TransportBackendDiscovery\"\n\
            android:exported=\"false\" >\n\
            <meta-data\n\
                android:name=\"backend:com.google.android.datatransport.cct.CctBackendFactory\"\n\
                android:value=\"cct\" />\n\
        </service>\n\
        <service\n\
            android:name=\"com.google.android.datatransport.runtime.scheduling.jobscheduling.JobInfoSchedulerService\"\n\
            android:exported=\"false\"\n\
            android:permission=\"android.permission.BIND_JOB_SERVICE\" >\n\
        </service>\n\
        <receiver\n\
            android:name=\"com.google.android.datatransport.runtime.scheduling.jobscheduling.AlarmManagerSchedulerBroadcastReceiver\"\n\
            android:exported=\"false\" />" );
					}

					if (includeFirebase || includePushNotify)
					{
						strcat(newcontents, "\n\
        <receiver android:name=\"com.google.firebase.iid.FirebaseInstanceIdReceiver\" \n\
                  android:exported=\"true\" \n\
                  android:permission=\"com.google.android.c2dm.permission.SEND\" > \n\
            <intent-filter> \n\
                <action android:name=\"com.google.android.c2dm.intent.RECEIVE\" /> \n\
            </intent-filter> \n\
        </receiver>");
					}

					if (includePushNotify)
					{
						strcat(newcontents, "\n\
		<meta-data android:name=\"com.google.firebase.messaging.default_notification_icon\"\n\
            android:resource=\"@drawable/icon_white\" />\n\
		<service android:name=\"com.google.firebase.messaging.FirebaseMessagingService\" \n\
            android:exported=\"true\" > \n\
            <intent-filter android:priority=\"-500\" > \n\
                <action android:name=\"com.google.firebase.MESSAGING_EVENT\" /> \n\
            </intent-filter> \n\
        </service>");
					}

					if ( includeAdMob )
					{
			strcat( newcontents, "\n\
        <provider\n\
            android:name=\"com.google.android.gms.ads.MobileAdsInitProvider\"\n\
            android:authorities=\"" );
			strcat( newcontents, pCurrentSelectedProject->apk_package_name );
			strcat( newcontents, ".mobileadsinitprovider\"\n\
            android:exported=\"false\"\n\
            android:initOrder=\"100\" />" );

			

					}

					// arcore activity
					if (pCurrentSelectedProject->apk_arcore > 0)
					{
						strcat(newcontents, "\n\
		<meta-data android:name=\"com.google.ar.core\" android:value=\"");
						if (pCurrentSelectedProject->apk_arcore == 1) strcat(newcontents, "optional");
						else strcat(newcontents, "required");
						strcat(newcontents, "\" />\n\
		<meta-data android:name=\"com.google.ar.core.min_apk_version\" android:value=\"190519000\" />\n\
        <activity\n\
            android:name=\"com.google.ar.core.InstallActivity\"\n\
            android:configChanges=\"keyboardHidden|orientation|screenSize\"\n\
            android:excludeFromRecents=\"true\"\n\
            android:exported=\"false\"\n\
            android:launchMode=\"singleTop\"\n\
            android:theme=\"@android:style/Theme.Material.Light.Dialog.Alert\" />");
					}

					strcat(newcontents, "\n    </application>\n</manifest>\n");


					// write new Android Manifest.xml file
					FILE* manifestfile = fopen(manifest_file, "wb+");
					if (!manifestfile)
						manifestfile = AGKfopen(manifest_file, "wb+");

					if (manifestfile) {
						fwrite(newcontents, 1, strlen(newcontents), manifestfile);
						fclose(manifestfile);
					}
					else {
						strcpy(android_error, "Failed to write AndroidManifest.xml file.");
						Valid = false;
					}

					if (contents) free(contents);
					contents = 0;

					//#################################
					//#### AndroidManifest.xml END ####
					//#################################

					int resLength;
					uString resources_file = tmp_folder;
					resources_file.Append("/resOrig/values/values.xml");

					if (Valid) {

						//####################
						//#### values.xml ####
						//####################

						if (!(contents = g_file_get_contents((char *)resources_file.GetStr(), contents, &resLength, NULL)))
						{
							strcpy(android_error, "Failed to read resource values.xml file");
							Valid = false;
						}
						if (!contents || resLength == 0) {
							//error
							strcpy(android_error, "Failed to read resource values.xml file");
							Valid = false;
						}

						if (Valid) {
							//ok continue.
							contents2 = strstr(contents, "<string name=\"app_name\">");
							if (!contents2)
							{
								strcpy(android_error, "Could not find app name entry in values.xml file");
								Valid = false;
							}
						}

					}

					if (Valid) {
						//#### values.xml ####
						contents2 += strlen("<string name=\"app_name\"");
						*contents2 = 0;
						contents3 = contents2;
						contents3++;
						contents3 = strstr(contents3, "</string>");
						if (!contents3)
						{
							strcpy(android_error, "Could not find end of app name entry in values.xml file");
							Valid = false;
						}
					}

					if (Valid) {


						// write resources file
						strcpy(newcontents, contents);
						strcat(newcontents, ">");
						strcat(newcontents, pCurrentSelectedProject->apk_app_name);
						strcat(newcontents, contents3);

						// repair original file
						*contents2 = '>';

						if (bIsGoogle && strlen(pCurrentSelectedProject->apk_play_app_id) > 0)
						{
							memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
							contents2 = strstr(newcontents2, "<string name=\"games_app_id\">");
							if (!contents2)
							{
								strcpy(android_error, "Could not find games_app_id entry in values.xml file");
								Valid = false;

							}
							if (Valid) {
								contents2 += strlen("<string name=\"games_app_id\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr(contents3, "</string>");
								if (!contents3)
								{
									strcpy(android_error, "Could not find end of games_app_id entry in values.xml file");
									Valid = false;
								}
								if (Valid) {
									// write resources file
									strcpy(newcontents, newcontents2);
									strcat(newcontents, ">");
									strcat(newcontents, pCurrentSelectedProject->apk_play_app_id);
									strcat(newcontents, contents3);

									// repair original file
									*contents2 = '>';
								}
							}
						}
					}

					if ( Valid )
					{
						// admob app id
						if (bIsGoogle && *(pCurrentSelectedProject->apk_admob_app_id))
						{
							memcpy( newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE );
							contents2 = strstr( newcontents2, "<string name=\"admob_app_id\">" );
							if ( !contents2 )
							{
								strcpy(android_error, "Could not find admob_app_id entry in values.xml file" );
								Valid = false;
							}
							if (Valid) {
								contents2 += strlen("<string name=\"admob_app_id\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr( contents3, "</string>" );
								if ( !contents3 )
								{
									strcpy(android_error, "Could not find end of admob_app_id entry in values.xml file" );
									Valid = false;
								}

								if (Valid) {
									// write resources file
									strcpy( newcontents, newcontents2 );
									strcat( newcontents, ">" );
									strcat( newcontents, pCurrentSelectedProject->apk_admob_app_id );
									strcat( newcontents, contents3 );

									// repair original file
									*contents2 = '>';
								}
							}
						}
					}

					if ( Valid )
					{
						// snapchat client id
						if (bIsGoogle && *(pCurrentSelectedProject->apk_snapchat_client_id))
						{
							memcpy( newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE );
							contents2 = strstr( newcontents2, "<string name=\"snap_chat_id\">" );
							if ( !contents2 )
							{
								strcpy(android_error, "Could not find snap_chat_id entry in values.xml file" );
								Valid = false;
							}
							if (Valid) {
								contents2 += strlen("<string name=\"snap_chat_id\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr( contents3, "</string>" );
								if ( !contents3 )
								{
									strcpy(android_error, "Could not find end of snap_chat_id entry in values.xml file" );
									Valid = false;
								}

								if (Valid) {
									// write resources file
									strcpy( newcontents, newcontents2 );
									strcat( newcontents, ">" );
									strcat( newcontents, pCurrentSelectedProject->apk_snapchat_client_id );
									strcat( newcontents, contents3 );

									// repair original file
									*contents2 = '>';
								}
							}
						}
					}

					if( Valid) {
						// firebase
						if (strlen(pCurrentSelectedProject->apk_firebase_config_path) > 0 && !bIsOuya) // Google and Amazon only
						{

							// read json values
							if (!(contentsOther = g_file_get_contents(pCurrentSelectedProject->apk_firebase_config_path, contentsOther, &resLength, NULL)))
							{
								strcpy(android_error, "Failed to read firebase config file");
								Valid = false;
							}
							if (!contentsOther || resLength == 0) {
								//error
								strcpy(android_error, "Failed to read firebase config file");
								Valid = false;
							}

							if( Valid ) {
								memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
								// find project_number value
								contentsOther2 = strstr(contentsOther, "\"project_number\": \"");
								if (!contentsOther2)
								{
									strcpy(android_error, "Could not find project_number entry in Firebase config file");
									Valid = false;
								}
							}
							if (Valid) {

								contentsOther2 += strlen("\"project_number\": \"");
								contentsOther3 = strstr(contentsOther2, "\"");
								if (!contentsOther3)
								{
									strcpy(android_error, "Could not find end of project_number entry in Firebase config file");
									Valid = false;
								}
							}
							if (Valid) {
								*contentsOther3 = 0;

								// find entry in newcontents2
								contents2 = strstr(newcontents2, "<string name=\"gcm_defaultSenderId\" translatable=\"false\"");
								if (!contents2)
								{
									strcpy(android_error, "Could not find gcm_defaultSenderId entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {
								contents2 += strlen("<string name=\"gcm_defaultSenderId\" translatable=\"false\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr(contents3, "</string>");
								if (!contents3)
								{
									strcpy(android_error, "Could not find end of gcm_defaultSenderId entry in values.xml file");
									Valid = false;
								}
							}
							bool bUsingStorageBucket = false;
							if (Valid) {
								// write resources file
								strcpy(newcontents, newcontents2);
								strcat(newcontents, ">");
								strcat(newcontents, contentsOther2);
								strcat(newcontents, contents3);

								*contents2 = '>'; // repair file
								*contentsOther3 = '"'; // repair file
								memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);


								// find firebase_url value

								contentsOther2 = strstr(contentsOther, "\"firebase_url\": \"");
								if (!contentsOther2)
								{
									contentsOther2 = strstr(contentsOther, "\"storage_bucket\": \"");
									if (!contentsOther2)
									{
										//storage_bucket
										strcpy(android_error, "Could not find firebase_url entry in Firebase config file");
										Valid = false;
									}
									else
									{
										bUsingStorageBucket = true;
									}
								}
							}

							if (Valid) {

								if (bUsingStorageBucket)
								{
									contentsOther2 += strlen("\"storage_bucket\": \"");
								}
								else
								{
									contentsOther2 += strlen("\"firebase_url\": \"");
								}
								contentsOther3 = strstr(contentsOther2, "\"");
								if (!contentsOther3)
								{
									strcpy(android_error, "Could not find end of firebase_url entry in Firebase config file");
									Valid = false;
								}
							}
							if (Valid) {

								*contentsOther3 = 0;

								// find entry in newcontents2
								contents2 = strstr(newcontents2, "<string name=\"firebase_database_url\" translatable=\"false\"");
								if (!contents2)
								{
									strcpy(android_error, "Could not find firebase_database_url entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								contents2 += strlen("<string name=\"firebase_database_url\" translatable=\"false\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr(contents3, "</string>");
								if (!contents3)
								{
									strcpy(android_error, "Could not find end of firebase_database_url entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								// write resources file
								strcpy(newcontents, newcontents2);
								strcat(newcontents, ">");
								strcat(newcontents, contentsOther2);
								strcat(newcontents, contents3);

								*contents2 = '>'; // repair file
								*contentsOther3 = '"'; // repair file
								memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);

								// find project_id value

								contentsOther2 = strstr(contentsOther, "\"project_id\": \"");
								if (!contentsOther2)
								{
									strcpy(android_error, "Could not find project_id entry in Firebase config file");
									Valid = false;
								}
							}

							if (Valid) {
								contentsOther2 += strlen("\"project_id\": \"");
								contentsOther3 = strstr(contentsOther2, "\"");
								if (!contentsOther3)
								{
									strcpy(android_error, "Could not find end of project_id entry in Firebase config file");
									Valid = false;
								}
							}
							if (Valid) {

								*contentsOther3 = 0;

								// find entry in newcontents2
								contents2 = strstr(newcontents2, "<string name=\"project_id\" translatable=\"false\"");
								if (!contents2)
								{
									strcpy(android_error, "Could not find project_id entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								contents2 += strlen("<string name=\"project_id\" translatable=\"false\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr(contents3, "</string>");
								if (!contents3)
								{
									strcpy(android_error, "Could not find end of project_id entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								// write resources file
								strcpy(newcontents, newcontents2);
								strcat(newcontents, ">");
								strcat(newcontents, contentsOther2);
								strcat(newcontents, contents3);

								*contents2 = '>'; // repair file
								*contentsOther3 = '"'; // repair file
								memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
							

								// find mobilesdk_app_id value
								// if the config file contains multiple Android apps then there will be multiple mobilesdk_app_id's, and only the corect one will work
								// look for the corresponding package_name that matches this export
							
								package_count = 0;
								contentsOther2 = contentsOther;
								while (*contentsOther2 && (contentsOther2 = strstr(contentsOther2, "\"mobilesdk_app_id\": \"")))
								{
									package_count++;
									contentsOther2 += strlen("\"mobilesdk_app_id\": \"");
									contentsOther3 = strstr(contentsOther2, "\"");
									if (!contentsOther3)
									{
										strcpy(android_error, "Could not find end of mobilesdk_app_id entry in Firebase config file");
										Valid = false;
										break;
									}
									*contentsOther3 = 0;

									// look for the package_name for this mobilesdk_app_id
									char* contentsOther4 = strstr(contentsOther3 + 1, "\"package_name\": \"");
									if (!contentsOther4)
									{
										strcpy(android_error, "Could not find package_name for mobilesdk_app_id entry in Firebase config file");
										Valid = false;
										break;
									}
									contentsOther4 += strlen("\"package_name\": \"");
									if (strncmp(contentsOther4, pCurrentSelectedProject->apk_package_name, strlen(pCurrentSelectedProject->apk_package_name)) == 0)
									{
										contentsOther4 += strlen(pCurrentSelectedProject->apk_package_name);
										if (*contentsOther4 == '\"')
										{
											break;
										}
									}

									*contentsOther3 = '"'; // repair file
								}
							}
							if (Valid) {

								if (!contentsOther2 || !*contentsOther2)
								{
									strcpy(android_error, "Could not find mobilesdk_app_id for android package_name in the Firebase config file");
									Valid = false;
								}
							}
							if (Valid) {

								// find entry in newcontents2
								contents2 = strstr(newcontents2, "<string name=\"google_app_id\" translatable=\"false\"");
								if (!contents2)
								{
									strcpy(android_error, "Could not find google_app_id entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								contents2 += strlen("<string name=\"google_app_id\" translatable=\"false\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr(contents3, "</string>");
								if (!contents3)
								{
									strcpy(android_error, "Could not find end of google_app_id entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								// write resources file
								strcpy(newcontents, newcontents2);
								strcat(newcontents, ">");
								strcat(newcontents, contentsOther2);
								strcat(newcontents, contents3);

								*contents2 = '>'; // repair file
								*contentsOther3 = '"'; // repair file
								memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
							}

							// find current_key value
							if (Valid) {
								contentsOther2 = strstr(contentsOther, "\"current_key\": \"");
								if (!contentsOther2)
								{
									strcpy(android_error, "Could not find current_key entry in Firebase config file");
									Valid = false;
								}
							}
							if (Valid) {

								contentsOther2 += strlen("\"current_key\": \"");
								contentsOther3 = strstr(contentsOther2, "\"");
								if (!contentsOther3)
								{
									strcpy(android_error, "Could not find end of current_key entry in Firebase config file");
									Valid = false;
								}
							}
							if (Valid) {

								*contentsOther3 = 0;

								// find entry in newcontents2
								contents2 = strstr(newcontents2, "<string name=\"google_api_key\" translatable=\"false\"");
								if (!contents2)
								{
									strcpy(android_error, "Could not find google_api_key entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								contents2 += strlen("<string name=\"google_api_key\" translatable=\"false\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr(contents3, "</string>");
								if (!contents3)
								{
									strcpy(android_error, "Could not find end of google_api_key entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								// write resources file
								strcpy(newcontents, newcontents2);
								strcat(newcontents, ">");
								strcat(newcontents, contentsOther2);
								strcat(newcontents, contents3);

								*contents2 = '>'; // repair file
								memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);

								// also copy it to google_crash_reporting_api_key
								contents2 = strstr(newcontents2, "<string name=\"google_crash_reporting_api_key\" translatable=\"false\"");
								if (!contents2)
								{
									strcpy(android_error, "Could not find google_crash_reporting_api_key entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {

								contents2 += strlen("<string name=\"google_crash_reporting_api_key\" translatable=\"false\"");
								*contents2 = 0;
								contents3 = contents2;
								contents3++;
								contents3 = strstr(contents3, "</string>");
								if (!contents3)
								{
									strcpy(android_error, "Could not find end of google_crash_reporting_api_key entry in values.xml file");
									Valid = false;
								}
							}
							if (Valid) {
								// write resources file
								strcpy(newcontents, newcontents2);
								strcat(newcontents, ">");
								strcat(newcontents, contentsOther2);
								strcat(newcontents, contents3);

								*contents2 = '>'; // repair file
								*contentsOther3 = '"'; // repair file
								memcpy(newcontents2, newcontents, AGK_NEW_CONTENTS_SIZE);
							}

						}
						if (contentsOther) free(contentsOther);
						contentsOther = 0;

						if (Valid) {

							// write new Android values.xml file
							FILE* resourcefile = fopen(resources_file, "wb+");
							if (!resourcefile)
								resourcefile = AGKfopen(resources_file, "wb+");

							if (resourcefile) {
								fwrite(newcontents, 1, strlen(newcontents), resourcefile);
								fclose(resourcefile);
							}
							else {
								strcpy(android_error, "Failed to write resource values.xml file.");
								Valid = false;
							}

						}

					}

					//########################
					//#### END VALUES.XML ####
					//########################

					if (contents) free(contents);
					contents = 0;

					//############################
					//#### Compile values.xml ####
					//############################

#ifdef AGK_WINDOWS
					_chdir(tmp_folder); //startupFolder
#else
					chdir(tmp_folder);
#endif
					char *aaptcommand = new char[1000000];
					int appid;
					if (Valid) {
						// start packaging app
						if (!file_exists( (char *) path_to_aapt2.GetStr()))
						{
							strcpy(android_error, "Failed to export project, AAPT2 program not found.");
							Valid = false;
						}
					}
					if (Valid) 
					{
						appid = run_command(path_to_aapt2.GetStr(), "m", &iStdOutLength, &iStdErrLength, false);
						if (appid == false) {
							strcpy(android_error, "Failed to start AAPT2 program.");
							Valid = false;
						}
					}
					if (Valid) {

#ifdef AGK_WINDOWS
						strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\values\\values.xml\n\n");
#else
						strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/values/values.xml\n\n");
#endif

#ifdef AGK_WINDOWS
						Sleep(200);
#else
						usleep(200);
#endif
						int ws = write_buffers(&aaptcommand[0], strlen(aaptcommand));

						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						char tmp[8192];
						strcpy(tmp, "");
						bool bAaptDone = false;
						while (still_running == 1 && agk::Timer() - timestart < 180.0) // increased timeout, takes a long time with large DTS projects
						{
#ifdef AGK_WINDOWS
							Sleep(200);
							still_running = agk::GetAppRunning(appid);
							int rs = read_buffers(&tmp[0], 1023 - 80, NULL, 0);
							if (strstr(tmp, "error:") || strstr(tmp, "Error")) {
								bAaptDone = false;
								break;
							}
							//On Mac i dont get this ?
							if (strstr(tmp, "Done")) {
								bAaptDone = true;
								break;
							}
#else
							usleep(200);
							//Mac dont allow reading the stdout while sending to stdin ?
							//but this also works.
							bAaptDone = true;
							break;
#endif
						}

#ifdef AGK_LINUX
						still_running = agk::GetAppRunning(appid);
						if (still_running != 1 ) {
							char tmp_err[32768];
							int rs = read_buffers(&tmp[0], 8000, &tmp_err[0], 32700);
							tmp[8000] = 0;
							tmp_err[32700] = 0;
							sprintf(android_error, "Exit early aaptcommand: %s \nreturn: %s\nstderr: %s", aaptcommand, tmp , tmp_err);
							Valid = false;
						}
#endif

						if (Valid && !bAaptDone) {
							tmp[1023-80] = 0;
							//Just display for now!
							sprintf(android_error, " timeout aaptcommand: %s \nreturn: %s\n", aaptcommand, tmp);
							//strcpy(android_error, tmp);
							Valid = false;
						}
					}

					//################################
					//#### End compile values.xml ####
					//################################

					if (Valid && !bIsOuya) 
					{

						//########################
						//#### load icon file ####
						//########################

						if ( *(pCurrentSelectedProject->apk_new_icon_path) == 0 )
						{
							uString nicon_file = "raw:";
							nicon_file.Append(tmp_folder);
							nicon_file.Append("/resMerged/mipmap-anydpi-v26_ic_launcher.xml.flat");
							agk::DeleteFile( nicon_file );
						}
						else
						{
							uString icon_file = "raw:";
							icon_file.Append(pCurrentSelectedProject->apk_new_icon_path);
							int icon_image = agk::LoadImage(icon_file.GetStr());
							if (!icon_image)
							{
								sprintf(android_error, "Failed to load image icon: %s", icon_file.GetStr());
								Valid = false;
							}

							const char* szMipmapFolder[] = { "mipmap-xxxhdpi", "mipmap-xxhdpi", "mipmap-xhdpi", "mipmap-hdpi", "mipmap-mdpi" };
							int iIconSize[] = { 432, 324, 216, 162, 108 };
							const char* szMainIcon = "ic_launcher_foreground.png";
							char cicon_file[MAX_PATH];

							int numIcons = sizeof(szMipmapFolder) / sizeof(const char*);
							for( int i = 0; i < numIcons; i++ )
							{
								// resize image
								sprintf(cicon_file, "raw:%s/resOrig/%s/%s", tmp_folder.GetStr(), szMipmapFolder[i], szMainIcon);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szMipmapFolder[i]); strcat(aaptcommand, "\\");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szMipmapFolder[i]); strcat(aaptcommand, "/");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
								int resized_image = agk::CopyImage( icon_image, 0, 0, agk::GetImageWidth(icon_image), agk::GetImageHeight(icon_image) );
								if ( !resized_image )
								{
									Valid = false;
									break;
								}

								Valid = GenerateResizedImage(resized_image, &cicon_file[0], iIconSize[i], iIconSize[i], &aaptcommand[0]);
								agk::DeleteImage( resized_image );
								if ( !Valid ) break;
							}

							if (agk::GetImageExists(icon_image))
								agk::DeleteImage(icon_image);

						} // end load icon file
					}

					if (Valid) 
					{

						//###############################
						//#### load legacy icon file ####
						//###############################

						if ( *(pCurrentSelectedProject->apk_icon_path) != 0 )
						{
							uString icon_file = "raw:";
							icon_file.Append(pCurrentSelectedProject->apk_icon_path);
							int icon_image = agk::LoadImage(icon_file.GetStr());
							if (!icon_image)
							{
								sprintf(android_error, "Failed to load legacy image icon: %s", icon_file.GetStr());
								Valid = false;
							}

							if (Valid) {
								// scale it and save it
								if (!bIsOuya)
								{
									uString nicon_file = "raw:";
									nicon_file.Append(tmp_folder);
									nicon_file.Append("/resOrig/mipmap-xxxhdpi/ic_launcher.png");
#ifdef AGK_WINDOWS
									strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\mipmap-xxxhdpi\\ic_launcher.png\n\n");
#else
									strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/mipmap-xxxhdpi/ic_launcher.png\n\n");
#endif
									Valid = GenerateResizedImage(icon_image, (char *) nicon_file.GetStr(), 192, 192, &aaptcommand[0]);

									if (Valid) {

										uString nicon_file = "raw:";
										nicon_file.Append(tmp_folder);
										nicon_file.Append("/resOrig/mipmap-xxhdpi/ic_launcher.png");
										// 144x144
#ifdef AGK_WINDOWS
										strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\mipmap-xxhdpi\\ic_launcher.png\n\n");
#else
										strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/mipmap-xxhdpi/ic_launcher.png\n\n");
#endif
										Valid = GenerateResizedImage(icon_image, (char *) nicon_file.GetStr(), 144, 144, &aaptcommand[0]);
									}
								}
							}
							const char* szDrawable_xhdpi = (bIsOuya) ? "drawable-xhdpi-v4" : "mipmap-xhdpi";
							const char* szDrawable_hdpi = (bIsOuya) ? "drawable-hdpi-v4" : "mipmap-hdpi";
							const char* szDrawable_mdpi = (bIsOuya) ? "drawable-mdpi-v4" : "mipmap-mdpi";
							const char* szDrawable_ldpi = (bIsOuya) ? "drawable-ldpi-v4" : "mipmap-ldpi";

							const char* szMainIcon = (bIsOuya) ? "app_icon.png" : "ic_launcher.png";
							char cicon_file[MAX_PATH];

							if (Valid) {
								// 96x96
								sprintf(cicon_file, "raw:%s/resOrig/%s/%s", tmp_folder.GetStr(), szDrawable_xhdpi, szMainIcon);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_xhdpi); strcat(aaptcommand, "\\");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_xhdpi); strcat(aaptcommand, "/");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 96, 96, &aaptcommand[0]);
							}
							if( Valid ) {

								// 72x72
								sprintf(cicon_file, "raw:%s/resOrig/%s/%s", tmp_folder.GetStr(), szDrawable_hdpi, szMainIcon);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_hdpi); strcat(aaptcommand, "\\");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_hdpi); strcat(aaptcommand, "/");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 72, 72, &aaptcommand[0]);
							}
							if (Valid) {


								// 48x48
								sprintf(cicon_file, "raw:%s/resOrig/%s/%s", tmp_folder.GetStr(), szDrawable_mdpi, szMainIcon);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_mdpi); strcat(aaptcommand, "\\");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_mdpi); strcat(aaptcommand, "/");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 48, 48, &aaptcommand[0]);
							}
							if (Valid) {


								// 36x36
								sprintf(cicon_file, "raw:%s/resOrig/%s/%s", tmp_folder.GetStr(), szDrawable_ldpi, szMainIcon);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_ldpi); strcat(aaptcommand, "\\");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_ldpi); strcat(aaptcommand, "/");
								strcat(aaptcommand, szMainIcon); strcat(aaptcommand, "\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 36, 36, &aaptcommand[0]);

							}

							if (agk::GetImageExists(icon_image))
								agk::DeleteImage(icon_image);

						} // end load icon file
					}

					if (Valid) 
					{

						//#####################################
						//#### load notification icon file ####
						//#####################################

						if ( strlen(pCurrentSelectedProject->apk_notif_icon_path) > 0 && !bIsOuya)
						{
							uString icon_file = "raw:";
							icon_file.Append(pCurrentSelectedProject->apk_notif_icon_path);
							int icon_image = agk::LoadImage(icon_file.GetStr());
							if (!icon_image)
							{
								sprintf(android_error, "Failed to load notification icon: %s", icon_file.GetStr());
								Valid = false;
							}
							char cicon_file[MAX_PATH];
							if (Valid) {
								// scale it and save it
								// 96x96
								sprintf(cicon_file, "raw:%s/resOrig/drawable-xxxhdpi/icon_white.png", tmp_folder.GetStr());
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable-xxxhdpi\\icon_white.png\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable-xxxhdpi/icon_white.png\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 96, 96, &aaptcommand[0]);

							}
							if (Valid) {
								// 72x72
								sprintf(cicon_file, "raw:%s/resOrig/drawable-xxhdpi/icon_white.png", tmp_folder.GetStr());
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable-xxhdpi\\icon_white.png\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable-xxhdpi/icon_white.png\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 72, 72, &aaptcommand[0]);
							}


							const char* szDrawable_xhdpi = (bIsOuya) ? "drawable-xhdpi-v4" : "drawable-xhdpi";
							const char* szDrawable_hdpi = (bIsOuya) ? "drawable-hdpi-v4" : "drawable-hdpi";
							const char* szDrawable_mdpi = (bIsOuya) ? "drawable-mdpi-v4" : "drawable-mdpi";
							const char* szDrawable_ldpi = (bIsOuya) ? "drawable-ldpi-v4" : "drawable-ldpi";


							if (Valid) {
								// 48x48
								sprintf(cicon_file, "raw:%s/resOrig/%s/icon_white.png", tmp_folder.GetStr(), szDrawable_xhdpi);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_xhdpi);
								strcat(aaptcommand, "\\icon_white.png\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_xhdpi);
								strcat(aaptcommand, "/icon_white.png\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 48, 48, &aaptcommand[0]);
							}

							if (Valid) {
								// 36x36
								sprintf(cicon_file, "raw:%s/resOrig/%s/icon_white.png", tmp_folder.GetStr(), szDrawable_hdpi);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_hdpi);
								strcat(aaptcommand, "\\icon_white.png\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_hdpi);
								strcat(aaptcommand, "/icon_white.png\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 36, 36, &aaptcommand[0]);
							}
								
							if (Valid) {
								// 24x24
								sprintf(cicon_file, "raw:%s/resOrig/%s/icon_white.png", tmp_folder.GetStr(), szDrawable_mdpi);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_mdpi);
								strcat(aaptcommand, "\\icon_white.png\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_mdpi);
								strcat(aaptcommand, "/icon_white.png\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 24, 24, &aaptcommand[0]);
							}

							if (Valid) {
								// 24x24
								sprintf(cicon_file, "raw:%s/resOrig/%s/icon_white.png", tmp_folder.GetStr(), szDrawable_ldpi);
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\"); strcat(aaptcommand, szDrawable_ldpi);
								strcat(aaptcommand, "\\icon_white.png\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/"); strcat(aaptcommand, szDrawable_ldpi);
								strcat(aaptcommand, "/icon_white.png\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 24, 24, &aaptcommand[0]);
							}

							if (agk::GetImageExists(icon_image))
								agk::DeleteImage(icon_image);
						}
					}

					if (Valid) 
					{

						//#######################################
						//#### load ouya icon and check size ####
						//#######################################

						if (bIsOuya && *(pCurrentSelectedProject->apk_ouya_icon_path) )
						{
							//pCurrentSelectedProject->apk_ouya_icon_path
							uString icon_file = "raw:";
							icon_file.Append(pCurrentSelectedProject->apk_ouya_icon_path);
							int icon_image = agk::LoadImage(icon_file.GetStr());
							if (!icon_image)
							{
								sprintf(android_error, "Failed to load Ouya large icon: %s", icon_file.GetStr());
								Valid = false;
							}
							char cicon_file[MAX_PATH];

							//Allow resize.
							if (1==2 && Valid) {
								if (agk::GetImageWidth(icon_image) != 732 || agk::GetImageHeight(icon_image) != 412)
								{
									strcpy(android_error, "Ouya large icon must be 732x412 pixels.");
									Valid = false;
								}
							}

							if (Valid) {
								// copy it to the res folder
								sprintf(cicon_file, "raw:%s/resOrig/drawable-xhdpi-v4/ouya_icon.png", tmp_folder.GetStr());
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable-xhdpi-v4\\ouya_icon.png\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable-xhdpi-v4/ouya_icon.png\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 732, 412, &aaptcommand[0]);
							}
							if (Valid) {
								// 320x180
								sprintf(cicon_file, "raw:%s/resOrig/drawable/icon.png", tmp_folder.GetStr());
#ifdef AGK_WINDOWS
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\drawable\\icon.png\n\n");
#else
								strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig/drawable/icon.png\n\n");
#endif
								Valid = GenerateResizedImage(icon_image, &cicon_file[0], 320, 180, &aaptcommand[0]);
							}

							if (agk::GetImageExists(icon_image))
								agk::DeleteImage(icon_image);

						}

					}

					if (Valid) 
					{

						//######################
						//#### Finish AAPT2 ####
						//######################


						strcpy(aaptcommand, "l\n-I\n");
						strcat(aaptcommand, path_to_android_jar);
						strcat(aaptcommand, "\n--manifest\n");
						strcat(aaptcommand, tmp_folder);
						strcat(aaptcommand, "/AndroidManifest.xml\n-o\n");
						strcat(aaptcommand, output_file);
						strcat(aaptcommand, "\n--auto-add-overlay\n--no-version-vectors\n");

						if ( bIsBundle )
						{
							strcat(aaptcommand, "--proto-format\n");
						}

						uString resMergedPath = tmp_folder;
						resMergedPath.Append("/resMerged");

						uint32_t folderID = agk::OpenRawFolder( resMergedPath );
						uint32_t numFiles = agk::GetRawFolderNumFiles( folderID );
						for( int fileID = 0; fileID < numFiles; fileID++ )
						{
							char* fileName = agk::GetRawFolderFileName( folderID, fileID );
							uString full_path = resMergedPath;
							full_path.Append("/");
							full_path.Append( fileName );
							delete [] fileName;

							if (file_exists( (char *) full_path.GetStr())) {
								strcat(aaptcommand, "-R\n");
								strcat(aaptcommand, full_path.GetStr() );
								strcat(aaptcommand, "\n");
							}
						}
						agk::CloseRawFolder( folderID );

						strcat(aaptcommand, "\nquit\n\n");

#ifdef AGK_WINDOWS
						char *ptr = aaptcommand;
						while (*ptr)
						{
							if (*ptr == '/') *ptr = '\\';
							ptr++;
						}
#endif

						//gchar* logpath = g_build_filename( tmp_folder, "log.txt", NULL );
						//FILE *pFile = fopen( logpath, "wb" );
						//fputs( aaptcommand, pFile );
						//fclose( pFile );

						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						char tmp[8192];
						strcpy(tmp, "");

#ifdef AGK_LINUX
						if (still_running != 1) {
							char tmp_err[32768];
							int rs = read_buffers(&tmp[0], 8000, &tmp_err[0], 32700);
							tmp[8000] = 0;
							tmp_err[32700] = 0;
							sprintf(android_error, "AAPT2 is not running: return: %s\nstderr: %s\n", tmp, tmp_err);
							Valid = false;
						}
#endif

						int ws = write_buffers(&aaptcommand[0], strlen(aaptcommand));

						while (still_running == 1 && agk::Timer() - timestart < 180.0)
						{
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif
							still_running = agk::GetAppRunning(appid);
							//No need to check for exit deamon, should end no matter what.
						}

						if (still_running) {
							//We have a time out ?
							int rs = read_buffers(&tmp[0], 8000, NULL, 0);
							tmp[8000] = 0;
							strcpy(android_error, "AAPT2: ");
							strcat(android_error, tmp);
							Valid = false;
						}


						if (Valid) {
#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif

							if (!file_exists(output_file)) {
								//
#ifdef AGK_LINUX
								char tmp_err[32768];
								int rs = read_buffers(&tmp[0], 8000, &tmp_err[0], 32700);
								tmp[8000] = 0;
								tmp_err[32700] = 0;
								strcpy(android_error, "AAPT2 return: ");
								strcat(android_error, tmp);
								strcat(android_error, "\nstderr:\n");
								strcat(android_error, tmp_err);
								strcat(android_error, "\n");
								strcat(android_error, "Failed to write output files, check that your project directory is not in a write protected location.\n");
								strcat(android_error, output_file);
									
								Valid = false;
#else

								int rs = read_buffers(&tmp[0], 8000, NULL, 0);
								tmp[8000] = 0;
								strcpy(android_error, "AAPT2 return: ");
								strcat(android_error, tmp);
								strcat(android_error, "\n");

								strcat(android_error, "Failed to write output files, check that your project directory is not in a write protected location.");
								Valid = false;
#endif
							}
						}

					}

#ifdef AGK_WINDOWS
					Sleep(200);
#else
					usleep(200);
#endif

					int still_running = agk::GetAppRunning(appid);
					if (still_running)
						agk::TerminateApp(appid);

					if (Valid) {

						//###############################
						//#### Add files to APK zip. ####
						//###############################

						if (file_exists((char*)output_file_zip)) {
							//Backup old zip file if exists (and delete), can cause dub .zip entrys.
							uString sBackFile = output_file_zip;
							sBackFile.Append(".bak");
							cp_copyfile((char *)output_file_zip, (char *)sBackFile.GetStr());
							sBackFile = "raw:";
							sBackFile.Append(output_file_zip);
							agk::DeleteFile(sBackFile);
						}


						rename(output_file, output_file_zip);
						uString zipfile = "raw:";
						zipfile.Append(output_file_zip);

						int zipid = 101;

						//char *checkcurpath = agk::GetCurrentDir();
						//OutputDebugStringA(checkcurpath);
						//OutputDebugStringA("\r");

						//PE: Found it :)  , utils_add_folder_to_zip() will fail if m_sCurrentDir != "" , debugger will change m_sCurrentDir.
						agk::SetCurrentDir("");

						if ( bIsBundle )
						{
							// need to extract and recreate zip file to move AndroidManifest.xml file
							uString sBundleFilePath = defaultWriteFolder;
							sBundleFilePath.AppendUTF8( "bundle_tmp" );
							uString sBundleFilePathRaw = "raw:";
							sBundleFilePathRaw += sBundleFilePath;
							utils_remove_folder_recursive( sBundleFilePath );

							agk::ExtractZip( zipfile.GetStr(), sBundleFilePathRaw );

							// move manifest
							uString oldManifest = sBundleFilePathRaw;
							oldManifest.AppendUTF8( "/AndroidManifest.xml" );
							uString newManifest = sBundleFilePathRaw;
							newManifest.AppendUTF8( "/manifest/AndroidManifest.xml" );
							uint32_t memID = agk::CreateMemblockFromFile( oldManifest );
							agk::CreateFileFromMemblock( newManifest, memID );
							agk::DeleteMemblock( memID );

							agk::DeleteFile( oldManifest );

							// zip everything back up
							agk::CreateZip( zipid, zipfile );
							utils_add_folder_to_zip( zipid, sBundleFilePath, "" ); 

							// remove temp folder
                            utils_remove_folder_recursive( sBundleFilePath );
                    	}
						else
						{						
							agk::CreateZip( zipid, zipfile.GetStr(), 2 );
						}

						if (zipid == 0) {
							strcpy(android_error, "Failed to initialise zip file for writing");
							Valid = false;
						}

						if (Valid) 
						{
							// copy in extra files
							uString zip_add_file = "raw:";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/classes.dex");
							if ( bIsBundle ) agk::AddZipEntry(zipid, zip_add_file.GetStr(), "dex/classes.dex");
							else agk::AddZipEntry(zipid, zip_add_file.GetStr(), "classes.dex");

							zip_add_file = "raw:";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/classes2.dex");
							if ( agk::GetFileExists( zip_add_file ) ) 
							{
								if ( bIsBundle ) agk::AddZipEntry(zipid, zip_add_file.GetStr(), "dex/classes2.dex");
								else agk::AddZipEntry(zipid, zip_add_file.GetStr(), "classes2.dex");
							}

							zip_add_file = "raw:";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/classes3.dex");
							if ( agk::GetFileExists( zip_add_file ) ) 
							{
								if ( bIsBundle ) agk::AddZipEntry(zipid, zip_add_file.GetStr(), "dex/classes3.dex");
								else agk::AddZipEntry(zipid, zip_add_file.GetStr(), "classes3.dex");
							}

							zip_add_file = "raw:";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/classes4.dex");
							if ( agk::GetFileExists( zip_add_file ) ) 
							{
								if ( bIsBundle ) agk::AddZipEntry(zipid, zip_add_file.GetStr(), "dex/classes4.dex");
								else agk::AddZipEntry(zipid, zip_add_file.GetStr(), "classes4.dex");
							}

							/*
							//androidsupportmultidexversion.txt
							zip_add_file = "raw:";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/androidsupportmultidexversion.txt");
							if ( agk::GetFileExists( zip_add_file ) ) agk::AddZipEntry(zipid, zip_add_file.GetStr(), "androidsupportmultidexversion.txt");
							*/

							zip_add_file = "raw:";
							zip_add_file.Append(android_folder);
							zip_add_file.Append("/lib/arm64-v8a/libandroid_player.so");
							agk::AddZipEntry(zipid, zip_add_file.GetStr(), "lib/arm64-v8a/libandroid_player.so");

							zip_add_file = "raw:";
							zip_add_file.Append(android_folder);
							zip_add_file.Append("/lib/armeabi-v7a/libandroid_player.so");
							agk::AddZipEntry(zipid, zip_add_file.GetStr(), "lib/armeabi-v7a/libandroid_player.so");
							
							if (pCurrentSelectedProject->apk_arcore > 0)
							{
								// use real ARCore lib
								zip_add_file = "raw:";
								zip_add_file.Append(android_folder);
								zip_add_file.Append("/lib/arm64-v8a/libarcore_sdk.so");
								agk::AddZipEntry(zipid, zip_add_file.GetStr(), "lib/arm64-v8a/libarcore_sdk.so");

								zip_add_file = "raw:";
								zip_add_file.Append(android_folder);
								zip_add_file.Append("/lib/armeabi-v7a/libarcore_sdk.so");
								agk::AddZipEntry(zipid, zip_add_file.GetStr(), "lib/armeabi-v7a/libarcore_sdk.so");
							}

							if (pCurrentSelectedProject->apk_snapchat_client_id[0] != 0)
							{
								zip_add_file = "raw:";
								zip_add_file.Append(android_folder);
								zip_add_file.Append("/lib/arm64-v8a/libpruneau.so");
								agk::AddZipEntry(zipid, zip_add_file.GetStr(), "lib/arm64-v8a/libpruneau.so");

								zip_add_file = "raw:";
								zip_add_file.Append(android_folder);
								zip_add_file.Append("/lib/armeabi-v7a/libpruneau.so");
								agk::AddZipEntry(zipid, zip_add_file.GetStr(), "lib/armeabi-v7a/libpruneau.so");
							}

							//Copy folder to zip under assets.
							zip_add_file = "";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/assets");
							Valid = utils_add_folder_to_zip(zipid, (char*)zip_add_file.GetStr(), "assets");
							
							zip_add_file = "";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/com");
							utils_add_folder_to_zip(zipid, (char*)zip_add_file.GetStr(), bIsBundle ? "root/com" : "com");

							zip_add_file = "";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/kotlin");
							utils_add_folder_to_zip(zipid, (char*)zip_add_file.GetStr(), bIsBundle ? "root/kotlin" : "kotlin");
							
							zip_add_file = "";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/okhttp3");
							utils_add_folder_to_zip(zipid, (char*)zip_add_file.GetStr(), bIsBundle ? "root/okhttp3" : "okhttp3");

							zip_add_file = "";
							zip_add_file.Append(src_folder);
							zip_add_file.Append("/extra_root");
							utils_add_folder_to_zip(zipid, (char*)zip_add_file.GetStr(), bIsBundle ? "root" : "");
							
							//Add Project Media folder.
							uString sMediaPath = "";
							uString m_szParse, m_szPathName;
							m_szParse = pCurrentSelectedProject->m_sProjectFileFullPath;
							m_szParse.ReplaceStr("\\\\", "/");
							m_szParse.ReplaceStr("\\", "/");
							int pos = m_szParse.RevFind('/');
							if (pos >= 0) {
								m_szParse.SubString(sMediaPath, 0, pos + 1);
								sMediaPath.Append("media");
								Valid = utils_add_folder_to_zip(zipid, (char*)sMediaPath.GetStr(), "assets/media");
							}

							agk::CloseZip(zipid);
						}

						if (Valid && bIsBundle) 
						{

							//########################
							//#### run bundletool ####
							//########################
							
							uString sParameters = "";
							sParameters.Append("-jar ");
							sParameters.Append("\"");
							sParameters.Append(path_to_bundletool);
							sParameters.Append("\" build-bundle --modules=\"");
							sParameters.Append(output_file_zip);
                            
							sParameters.Append("\" ");

							uString mappingfile = src_folder;
							mappingfile.AppendUTF8("/mapping/mapping.txt");
							if ( file_exists( mappingfile ) )
							{
                                #ifdef AGK_WINDOWS
                                    sParameters.Append("--metadata-file=com.android.tools.build.obfuscation/proguard.map:\"");
                                    sParameters.Append(mappingfile);
                                    sParameters.Append("\" ");
                                #else
                                    sParameters.Append("--metadata-file=com.android.tools.build.obfuscation/proguard.map:");
                                    mappingfile.ReplaceStr( " ", "\\ " );                                
                                    sParameters.Append(mappingfile);
                                    sParameters.Append(" ");
                                #endif
							}

							// mike - hack
							uString configFile = src_folder;
							configFile.AppendUTF8 ( "/BundleConfig.json" );

							if ( file_exists ( mappingfile ) )
							{
								#ifdef AGK_WINDOWS
									sParameters.Append ( "--config=\"" );
									sParameters.Append ( configFile );
									sParameters.Append ( "\" " );
								#else
									sParameters.Append ( "--config=" );
									configFile.ReplaceStr ( " ", "\\ " );
									sParameters.Append ( configFile );
									sParameters.Append ( " " );
								#endif

							}

							//sParameters.Append ( "--uncompressed " );
							

							sParameters.Append("--output=\"");
							sParameters.Append(output_file);
							sParameters.Append("\"");
                            
                            #ifdef DEBUG_EXPORT
                                agk::Message ( sParameters );
                            #endif

							int appid = run_command(java_path, sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);
							if (appid == false) {
								strcpy(android_error, "Failed to start bundletool program.");
								Valid = false;
							}
							if (Valid) {
								float timestart = agk::Timer();
								int still_running = agk::GetAppRunning(appid);
								char tmp[1024] = {0};
								while (still_running == 1 && agk::Timer() - timestart < 360.0)
								{
#ifdef AGK_WINDOWS
									Sleep(400);
#else
									usleep(400);
#endif
									still_running = agk::GetAppRunning(appid);
								}
								
                                // std output
                                int rs = read_buffers(&tmp[0], 1024 - 90, NULL, 0);
                                
                                // std error - this is more useful but crashes on Windows
                                //int rs = read_buffers(NULL, 0, &tmp[0], 1024 - 90 );
                                
								if (still_running) 
								{
									sprintf(android_error, "Bundletool timed out, (output: %s)", tmp);
									Valid = false;
									agk::TerminateApp(appid);
								}
								else
								{
                                  	if( strlen(tmp) != 0 )
									{
										sprintf(android_error, "Failed to run bundletool, output: %s", tmp);
										Valid = false;
									}
								}
							}

						}

						if (Valid) 
						{

							//##################
							//#### sign apk ####
							//##################

							#ifdef DEBUG_EXPORT
								agk::Message ( "Signing APK" );
							#endif

#ifdef AGK_WINDOWS
							Sleep(200);
#else
							usleep(200);
#endif

							uString sParameters ="";
                            
                            //sParameters.Append("-verbose ");
                            
							if ( bIsBundle ) sParameters.Append("-storepass ");
							else sParameters.Append("-sigalg MD5withRSA -digestalg SHA1 -storepass ");

#ifdef AGK_WINDOWS
							sParameters.Append("\"");
							sParameters.Append(keystore_password);
							sParameters.Append("\"");
#else
							sParameters.Append(keystore_password);
#endif

							sParameters.Append(" -keystore ");
							sParameters.Append("\"");
							sParameters.Append(keystore_file);
							sParameters.Append("\"");
							sParameters.Append(" ");
							sParameters.Append("\"");
							if ( bIsBundle ) sParameters.Append(output_file);
							else sParameters.Append(output_file_zip);
                            sParameters.Append("\"");


							sParameters.Append(" ");
							sParameters.Append("\"");
							sParameters.Append(alias_name);
							sParameters.Append("\"");

							sParameters.Append(" -keypass ");
#ifdef AGK_WINDOWS
							sParameters.Append("\"");
							sParameters.Append(alias_password);
							sParameters.Append("\"");
#else
							sParameters.Append(alias_password);
#endif

							#ifdef DEBUG_EXPORT
								agk::Message ( uString ( "parameters: " ) += sParameters );
                                agk::Message ( output_file );
							#endif

							int appid = run_command(jarsigner_path, sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);
							if (appid == false) {
								strcpy(android_error, "Failed to start jarsigner program.");
								Valid = false;
							}
							if (Valid) {
								float timestart = agk::Timer();
								int still_running;
								still_running = agk::GetAppRunning(appid);
								char tmp[1024] = {0};
								//Could take some time on large .apk's set for 5 min.
								while (still_running == 1 && agk::Timer() - timestart < 360.0)
								{
#ifdef AGK_WINDOWS
									Sleep(400);
#else
									usleep(400);
#endif
									still_running = agk::GetAppRunning(appid);
								}
								int rs = read_buffers(&tmp[0], 1024 - 90, NULL, 0);
								if (still_running) 
								{
									sprintf(android_error, "Jarsigner timed out signing APK, (output: %s)", tmp);
									Valid = false;
									agk::TerminateApp(appid);
								}
								else
								{
                                    
                                    
                                    
									if( strstr(tmp, "jar signed") == 0 && tmp[0] != 0 ) 
									{
										sprintf(android_error, "Failed to sign APK, is your keystore password and alias correct? (error: %s)", tmp);
										Valid = false;
									}
								}
							}
						}

						if (Valid && !bIsBundle) 
						{

							//###################
							//#### align apk ####
							//###################

							#ifdef DEBUG_EXPORT
								agk::Message ( "Aligning APK" );
							#endif

							uString sParameters = "";
							sParameters.Append("4 ");
							sParameters.Append("\"");
							sParameters.Append(output_file_zip);
							sParameters.Append("\"");
							sParameters.Append(" ");
							sParameters.Append("\"");
							sParameters.Append(output_file);
							sParameters.Append("\"");

							int appid = run_command(path_to_zipalign, sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);
							if (appid == false) {
								strcpy(android_error, "Failed to start zipalign tool.");
								Valid = false;
							}
							if (Valid) {
								float timestart = agk::Timer();
								int still_running = agk::GetAppRunning(appid);
								char tmp[1024] = {0};
								while (still_running == 1 && agk::Timer() - timestart < 240.0)
								{
#ifdef AGK_WINDOWS
									Sleep(400);
#else
									usleep(400);
#endif
									still_running = agk::GetAppRunning(appid);
								}
								int rs = read_buffers(&tmp[0], 1024 - 90, NULL, 0);
								if (still_running) 
								{
									sprintf(android_error, "Zip align timed out, (output: %s)", tmp);
									Valid = false;
									agk::TerminateApp(appid);
								}
								else
								{
									if ( tmp[0] != 0 ) 
									{
										sprintf(android_error, "Failed to zip align APK, (error: %s)", tmp);
										Valid = false;
									}
								}
							}
						}

						if ( Valid && !bIsBundle ) 
						{

							//#####################
							//#### sign apk V2 ####
							//#####################

							#ifdef DEBUG_EXPORT
								agk::Message ( "Signing APK V2" );
							#endif
							
							uString sParameters ="-jar \"";
							sParameters.Append( path_to_apksigner );
							sParameters.Append( "\" sign --ks \"" );
							sParameters.Append( keystore_file );
							sParameters.Append( "\" --ks-pass pass:\"" );
							sParameters.Append( keystore_password );
							sParameters.Append( "\" --ks-key-alias \"" );
							sParameters.Append( alias_name );
							sParameters.Append( "\" --key-pass pass:\"" );
							sParameters.Append( alias_password );
							sParameters.Append( "\" \"" );
							sParameters.Append( output_file );
							sParameters.Append( "\"");

							int appid = run_command(java_path, sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);
							if (appid == false) {
								strcpy(android_error, "Failed to start apksigner program.");
								Valid = false;
							}
							if (Valid) 
							{
								float timestart = agk::Timer();
								int still_running = agk::GetAppRunning(appid);
								char tmp[1024] = {0};
								while (still_running == 1 && agk::Timer() - timestart < 360.0)
								{
#ifdef AGK_WINDOWS
									Sleep(400);
#else
									usleep(400);
#endif
									still_running = agk::GetAppRunning(appid);
								}
								int rs = read_buffers(&tmp[0], 1024 - 90, NULL, 0);
								if (still_running) 
								{
									sprintf(android_error, "Apksigner timed out signing APK, (output: %s)", tmp);
									Valid = false;
									agk::TerminateApp(appid);
								}
								else
								{
									if( tmp[0] != 0 ) 
									{
										sprintf(android_error, "Failed to sign APK, is your keystore password and alias correct? (error: %s)", tmp);
										Valid = false;
									}
								}
							}
						}

						//Cleanup.
						remove(output_file_zip);

						#ifndef DEBUG_EXPORT
							utils_remove_folder_recursive ( tmp_folder.GetStr ( ) );
						#endif
					}

				}

			}

#ifdef AGK_WINDOWS
			_chdir(curDir);
#else
			chdir(curDir);
#endif

		}

		PleaseWaitEnd();
		if (Valid)
			show_androidexport_window = false;
	  }
	}

	ImGui::Columns(1);

	ImGui::End();
}

int utils_remove_folder_recursive(const char *src)
{
#ifdef AGK_WINDOWS
	char currDir[ 1024 ];
	GetCurrentDirectoryA( 1024, currDir );
	if ( !SetCurrentDirectoryA( src ) ) return 1;
	
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA( "*.*", &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE) return 0;

	do
	{
		if ( strcmp ( FindFileData.cFileName, "." ) != 0 && strcmp ( FindFileData.cFileName, ".." ) != 0 )
		{
			if ( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				if ( !utils_remove_folder_recursive( newSrc ) ) return 0;
				if ( !RemoveDirectoryA( newSrc ) )
				{
					return 0;
				}
			}
			else
			{
				// file
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "\\" );
				strcat( newSrc, FindFileData.cFileName );

				if ( !DeleteFileA( newSrc ) ) 
				{
					return 0;
				}
			}
		}
	} while ( FindNextFileA( hFind, &FindFileData ) );

	FindClose( hFind );

	SetCurrentDirectoryA( currDir );

#else
	// Mac and Linux
	char currDir[ 1024 ];
	getcwd( currDir, 1024 );
	if ( chdir( src ) != 0 ) return 1;
	
	DIR *d = opendir( src );
	dirent *dent = 0;
	
	while( (dent = readdir( d )) )
	{
		if ( strcmp ( dent->d_name, "." ) != 0 && strcmp ( dent->d_name, ".." ) != 0 )
		{
			if ( dent->d_type == DT_DIR )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "/" );
				strcat( newSrc, dent->d_name );

				if ( !utils_remove_folder_recursive( newSrc ) ) return 0;
				if ( rmdir( newSrc ) != 0 )
				{
					return 0;
				}
			}
			else
			{
				// file
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "/" );
				strcat( newSrc, dent->d_name );

				if ( unlink( newSrc ) != 0 ) 
				{
					return 0;
				}
			}
		}
	}

	closedir( d );
	
	chdir( currDir );	
#endif
	return 1;
}

bool utils_add_folder_to_zip( int zipid, const char *folder, const char *zipfolder)
{
	if (strlen(folder) == 0) return false;

	uString sSrcPath;
	uString sPrefix;
	sPrefix.AppendN( folder, 4 );
	if ( strchr( folder, ':' ) || folder[0] == '/' ) 
	{
		if ( sPrefix.CompareCaseTo( "raw:" ) != 0 ) sSrcPath = "raw:";
	}
	sSrcPath.Append( folder );
	sSrcPath.Replace( '\\', '/' );

	uint32_t folderID = agk::OpenRawFolder( sSrcPath );
	uint32_t numFiles = agk::GetRawFolderNumFiles( folderID );
	for( int i = 0; i < numFiles; i++ )
	{
		char* fileName = agk::GetRawFolderFileName( folderID, i );

		uString fullFilePath = sSrcPath;
		fullFilePath.AppendAscii( '/' );
		fullFilePath.Append(fileName);
		
		uString fullFileZipPath = zipfolder;
		if ( fullFileZipPath.GetLength() > 0 ) fullFileZipPath.AppendAscii( '/' );
		fullFileZipPath.Append(fileName);

		delete [] fileName;
		
		agk::AddZipEntry(zipid, fullFilePath.GetStr(), fullFileZipPath.GetStr() );
	}

	uint32_t numFolders = agk::GetRawFolderNumFolders( folderID );
	for( int i = 0; i < numFolders; i++ )
	{
		char* folderName = agk::GetRawFolderFolderName( folderID, i );

		uString sNewFolder = sSrcPath;
		sNewFolder.AppendAscii( '/' );
		sNewFolder.Append( folderName );
		
		uString sNewZipFolder = zipfolder;
		if ( sNewZipFolder.GetLength() > 0 ) sNewZipFolder.AppendAscii( '/' );
		sNewZipFolder.Append( folderName );

		delete [] folderName;

		utils_add_folder_to_zip( zipid, sNewFolder, sNewZipFolder );
	}

	return true;
}


bool GenerateResizedImage(int icon,char *filename,int sizew,int sizeh, char *aaptcmd)
{
	agk::ResizeImage(icon, sizew, sizeh);
	agk::SaveImage(icon, filename);
	// 192x192
	if (!agk::GetFileExists(filename)) {
		sprintf(android_error, "Failed to save icon: %s , cmd: ", filename/*, aaptcmd*/);
		return(false);
	}

	int ws = write_buffers(aaptcmd, strlen(aaptcmd));
#ifdef AGK_WINDOWS
	Sleep(500);
#else
	usleep(200);
#endif
	int rs = read_buffers(&android_error[0], 1023, NULL, 0);
	android_error[1023] = 0;
	return true;
}

bool GenerateIPAResizedImage(int icon, char *filename, int sizew, int sizeh)
{
	agk::ResizeImage(icon, sizew, sizeh);
	agk::SaveImage(icon, filename);
	if (!agk::GetFileExists(filename)) {
		sprintf(ios_error, "Failed to save icon: %s", filename);
		return(false);
	}
	return true;
}


char afiles_errormsg[MAX_PATH + 80];
int installprocesscount = 0;

bool utils_copy_folder( char *folder, char *destfolder)
{
	char *oldpath, *oldfirstpath, *ctmp, *cftmp;
	oldfirstpath = agk::GetWritePath();
	if (strlen(folder) > 0)
		agk::SetRawWritePath(folder);

	uString path = folder;
	uString destpath = destfolder;
	if (path.GetLength() > 0)
	{
		oldpath = agk::GetWritePath();
		agk::SetRawWritePath(path.GetStr());

		int mcf = 0;
		cftmp = agk::GetFirstFile(1);
		while (cftmp && strlen(cftmp) > 0 && mcf < 9999) {

			uString fullFilePath = ""; //"raw:";
			fullFilePath.Append(path);
			fullFilePath.Append("/");
			fullFilePath.Append(cftmp);
			fullFilePath.ReplaceStr("\\", "/");

			uString fullFileZipPath = destpath;
			fullFileZipPath.Append("/");
			fullFileZipPath.Append(cftmp);
			fullFileZipPath.ReplaceStr("\\", "/");

			//Add file to zip.
			//Copy file , from to.
			if (file_exists( (char *) fullFileZipPath.GetStr() )) {
				//
				remove((char *)fullFileZipPath.GetStr());
			}
			if (!cp_copyfile((char*)fullFilePath.GetStr(), (char *)fullFileZipPath.GetStr())) {
				//Failed. display error.
				sprintf(afiles_errormsg, "Failed: %s", fullFileZipPath.GetStr());
			}

			delete[] cftmp;
			cftmp = agk::GetNextFile();
			mcf++;
		}

		agk::SetRawWritePath(oldpath);
	}


	int mc = 0;
	ctmp = agk::GetFirstFolder(1);

	//precache all strings.
	char *cached_str[999];
	cached_str[0] = 0;
	while (ctmp && strlen(ctmp) > 0 && mc < 999) {
		cached_str[mc] = ctmp;
		mc++;
		cached_str[mc] = 0;
		ctmp = agk::GetNextFolder();
	}

	mc = 0;
	ctmp = cached_str[mc];
	while (ctmp && strlen(ctmp) > 0 && mc < 999 - 1) {
		uString fullpath = agk::GetWritePath();
		fullpath.Append(ctmp);
		fullpath.ReplaceStr("\\", "/");
		uString fullzippath = destfolder;
		fullzippath.Append("/");
		fullzippath.Append(ctmp);
		fullzippath.ReplaceStr("\\", "/");
		//Create Dest folder if not exists.
#ifdef AGK_WINDOWS
		_mkdir(fullzippath.GetStr());
#else
		mkdir(fullzippath.GetStr(), S_IRWXU);
#endif
		agk::Sleep(1); // To prevent "app dont respond"
		utils_copy_folder( (char *)fullpath.GetStr(), (char*)fullzippath.GetStr());

		delete[] cached_str[mc];
		mc++;
		ctmp = cached_str[mc];
	}

	agk::SetRawWritePath(oldfirstpath);
	return true;
}

//######################
//#### HTML5 Export ####
//######################

void ProcessHTML5Export(void)
{
	if (!show_html5export_window)
		return;
	if (!pCurrentSelectedProject)
		return;

	static char output_folder[MAX_PATH];
	static int iHTML5State = -2;

	if (iHTML5State != show_html5export_window) {
		//Setup default folder.
		iHTML5State = show_html5export_window;
		if (pCurrentSelectedProject) {
			uString tmp = pCurrentSelectedProject->m_sProjectFileFullPath;
			tmp.ReplaceStr("\\", "/");
			int pos = tmp.RevFindStr("/");
			if (pos > 0) {
				uString finals;
				tmp.SubString(finals, 0, pos);
				tmp = finals;
			}
			strcpy(output_folder, tmp.GetStr());
			if (output_folder[strlen(output_folder) - 1] == '/')
				strcat(output_folder, "HTML5");
			else
				strcat(output_folder, "/HTML5");

			if (strlen(pCurrentSelectedProject->html_output_path) <= 0)
			{
				strcpy(pCurrentSelectedProject->html_output_path, output_folder);
			}
		}
	}


	int input_indent = 12 * ImGui::GetFontSize();
	int item_dot_width = -32;
	ImGuiWindowFlags window_flags = 0;
	ImGui::SetNextWindowSize(ImVec2(40 * ImGui::GetFontSize(), 29 * ImGui::GetFontSize()), ImGuiCond_Once);
	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
	ImGui::Begin("HTML5 export.", &show_html5export_window, window_flags);

	ImGui::Indent(8.0f);
	ImGui::Spacing();

	ImVec4* style_colors = ImGui::GetStyle().Colors;
	ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
	ImGui::SetWindowFontScale(1.1);
	style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
	ImGui::TextWrapped("HTML5 Export:");
	style_colors[ImGuiCol_Text] = oldTextColor;
	ImGui::SetWindowFontScale(1.0);

	ImGui::Indent(10.0f);

	ImGui::TextWrapped("This will build the HTML5 files necessary to host your app on a website. You will need to upload these files somewhere to make your game accessible to others.");

	ImGui::TextWrapped("For the best experience we recommend you use Firefox or Chrome to view the HTML5 app. When testing the files locally, without uploading them to a server, Chrome will not work. When testing on an Android device, Chrome is the best choice as Firefox has trouble when the app goes full screen. iOS devices do not support full screen at all, and tend to have much lower HTML5 performance than other platforms.");

	ImGui::TextWrapped("Note that HTML5 only has partial support for the AGK command set so not all commands will work. In particular the Networking commands HostNetwork and JoinNetwork are not functional in HTML5 apps. Also the file commands will work but are not persistent, so any saved files will be lost when the user closes or reloads the app.");

	ImGui::TextWrapped("File size is very important when hosting apps online, so you should remove any unnecessary files from the project's media folder before exporting.");

	ImGui::Indent(-10.0f);

	ImGui::Separator();

	static int commands_used = 0;
	ImGui::Text("Commands Used:");
	const char* commands_used_array[] = { "2D Only", "2D and 3D" };
	ImGui::SameLine();
	ImGui::PushItemWidth(-10);
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::Combo("##html5commands_used", &pCurrentSelectedProject->html_commands_used, commands_used_array, IM_ARRAYSIZE(commands_used_array));
	ImGui::PopItemWidth();
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Selecting 2D Only will reduce the file size which will improve loading times, but all 3D commands will be disabled.");
		ImGui::EndTooltip();
	}
	commands_used = pCurrentSelectedProject->html_commands_used;

	static bool bDynamic = false;
	ImGui::Text("Dynamic Memory: ");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	bDynamic = pCurrentSelectedProject->html_dynamic_memory;
	ImGui::Checkbox("##html5bDynamic", &bDynamic);
	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowContentSize(ImVec2(400, 0));
		ImGui::BeginTooltip();
		ImGui::TextWrapped("Disables some optimizations, but allows your app to use more than 256MB of RAM.");
		ImGui::EndTooltip();
	}
	pCurrentSelectedProject->html_dynamic_memory = bDynamic;


	ImGui::Text("Output Folder:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(item_dot_width);
	if (ImGui::InputText("##HTML5OutputFolder", &pCurrentSelectedProject->html_output_path[0], MAX_PATH)) {
		uString nfolder = pCurrentSelectedProject->html_output_path;
		nfolder.ReplaceStr("\\", "/");
		strcpy(pCurrentSelectedProject->html_output_path, nfolder.GetStr());
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if (ImGui::Button("...")) {
		char *selectedfolder;
		selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_DIR, NULL, NULL, NULL);
		if (selectedfolder) {
			if (strlen(selectedfolder) > 0) {
				uString nfolder = selectedfolder;
				nfolder.ReplaceStr("\\", "/");
				strcpy(pCurrentSelectedProject->html_output_path, nfolder.GetStr());
			}
		}
	}

	if (ImGui::Button("Export")) {

		char curDir[MAX_PATH];
		extern char startupFolder[MAX_PATH];

		strcpy(output_folder, pCurrentSelectedProject->html_output_path);
		
#ifdef AGK_WINDOWS
		_getcwd(&curDir[0], MAX_PATH);
		_chdir(startupFolder);
#else
		getcwd(&curDir[0], MAX_PATH);
		chdir(startupFolder);
#endif

		// CHECKS COMPLETE, START EXPORT
		// make temporary folder
		extern char defaultWriteFolder[MAX_PATH];
		uString tmp_folder = defaultWriteFolder;
		tmp_folder.Append("build_tmp");
		tmp_folder.ReplaceStr("\\", "/");

		//Delete all content of build_tmp if exists.
		utils_remove_folder_recursive(tmp_folder.GetStr());


		uString html5_folder = startupFolder;
		html5_folder.Append("/media/data/html5/");
		html5_folder.ReplaceStr("\\", "/");

		const char *szCommandsFolder = "";
		if (bDynamic) szCommandsFolder = commands_used ? "3Ddynamic" : "2Ddynamic";
		else szCommandsFolder = commands_used ? "3D" : "2D";

		uString src_folder = html5_folder;
		src_folder.Append(szCommandsFolder);

		if (output_folder[strlen(output_folder) - 1] == '/')
			output_folder[strlen(output_folder) - 1] = 0;
		if (output_folder[strlen(output_folder) - 1] == '\\')
			output_folder[strlen(output_folder) - 1] = 0;
		uString output_file = output_folder;

		uString media_folder = "[notfound]";
		uString m_szParse;

		// decalrations
		char sztemp[30];
		char *newcontents = 0;
		char *load_package_string = new char[200000];
		char *additional_folders_string = new char[200000];
		char *contents = NULL;
		char *contents2 = NULL;
		char *contents3 = NULL;
		int length = 0;

		strcpy(load_package_string, "");
		strcpy(additional_folders_string, "");

		FILE *pHTML5File = 0;
		//char *media_folder = 0;

		//mz_zip_archive zip_archive;
		//memset(&zip_archive, 0, sizeof(zip_archive));
		//gchar *str_out = NULL;

		bool Valid = true;

		uString testfile = output_folder;
		testfile.Append("/AGKPlayer.data");

		if (file_exists((char*)testfile.GetStr())) {
			Valid = overWriteFileBox((char*)testfile.GetStr());
		}

		if (Valid) {

#ifdef AGK_WINDOWS
			_mkdir(tmp_folder.GetStr());
#else
			mkdir(tmp_folder.GetStr(), S_IRWXU);
#endif

			if (!utils_copy_folder((char *)src_folder.GetStr(), (char *)tmp_folder.GetStr()))
			{
				BoxerInfo("Failed to copy source folder", "Export Error");
				Valid = false;
			}
		}

		// create HTML5 data file that we'll add all the media files to
		uString html5data_file = tmp_folder;
		html5data_file.Append("/AGKPlayer.data");

		if (Valid) {

			pHTML5File = fopen(html5data_file.GetStr(), "wb");
			if (!pHTML5File)
			{
				BoxerInfo("Failed to open HTML5 data file for writing", "Export Error");
				Valid = false;
			}
		}

		int currpos = 0;

		if (Valid) {
			// start the load package string that will store the list of files, it will be built at the same time as adding the media files
			strcpy(load_package_string, "loadPackage({\"files\":[");
			strcpy(additional_folders_string, "Module[\"FS_createPath\"](\"/\", \"media\", true, true);");

			//Add Project Media folder.
			m_szParse = pCurrentSelectedProject->m_sProjectFileFullPath;
			m_szParse.ReplaceStr("\\", "/");
			int pos = m_szParse.RevFind('/');
			if (pos >= 0) {
				m_szParse.SubString(media_folder, 0, pos + 1);
				media_folder.Append("media");
			}

			// add the media files and construct the load package string, currpos will have the total data size afterwards
			if (!utils_add_folder_to_html5_data_file(pHTML5File, media_folder.GetStr(), "/media", load_package_string, additional_folders_string, &currpos))
			{
				fclose(pHTML5File);
				pHTML5File = 0;

				BoxerInfo("Failed to write HTML5 data file", "Export Error");
				Valid = false;
			}

		}

		// edit AGKplayer.js to add our load package string
		uString agkplayer_file = tmp_folder;
		agkplayer_file.Append("/AGKPlayer.js");

		if (Valid) {
			fclose(pHTML5File);
			pHTML5File = 0;

			// remove the final comma that was added
			if (*load_package_string && load_package_string[strlen(load_package_string) - 1] == ',') load_package_string[strlen(load_package_string) - 1] = 0;

			// finsh the load package string 
			strcat(load_package_string, "],\"remote_package_size\":");
			sprintf(sztemp, "%d", currpos);
			strcat(load_package_string, sztemp);
			strcat(load_package_string, ",\"package_uuid\":\"e3c8dd30-b68a-4332-8c93-d0cf8f9d28a0\"})");


			if (!(contents = g_file_get_contents((char *)agkplayer_file.GetStr(), contents, &length, NULL)))
			{
				BoxerInfo("Failed to read AGKPlayer.js file:", "Export Error");
				Valid = false;
			}
		}
		if (Valid) {
			newcontents = new char[length + 400000];
			strcpy(newcontents, "");

			contents2 = contents;
			contents3 = 0;

			// the order of these relacements is important (if more than one), they must occur in the same order as they occur in the file

			// replace %%ADDITIONALFOLDERS%%
			contents3 = strstr(contents2, "%%ADDITIONALFOLDERS%%");
			if (contents3)
			{
				*contents3 = 0;
				contents3 += strlen("%%ADDITIONALFOLDERS%%");

				strcat(newcontents, contents2);
				strcat(newcontents, additional_folders_string);

				contents2 = contents3;
			}
			else
			{
				BoxerInfo("AGKPlayer.js is corrupt, it is missing the %%ADDITIONALFOLDERS%% variable", "Export Error");
				Valid = false;
			}
		}
		if (Valid) {
			// replace %%LOADPACKAGE%%
			contents3 = strstr(contents2, "%%LOADPACKAGE%%");
			if (contents3)
			{
				*contents3 = 0;
				contents3 += strlen("%%LOADPACKAGE%%");

				strcat(newcontents, contents2);
				strcat(newcontents, load_package_string);

				contents2 = contents3;
			}
			else
			{
				BoxerInfo("AGKPlayer.js is corrupt, it is missing the %%LOADPACKAGE%% variable", "Export Error");
				Valid = false;
			}
		}
		if (Valid) {
			// write the rest of the file
			strcat(newcontents, contents2);

			if (file_exists((char *) agkplayer_file.GetStr())) {
				remove(agkplayer_file.GetStr());
			}
			
			FILE* entfile = fopen(agkplayer_file.GetStr(), "wb+");
			if (!entfile)
				entfile = AGKfopen(agkplayer_file.GetStr(), "wb+");

			if (entfile) {
				fwrite(newcontents, 1, strlen(newcontents), entfile);
				fclose(entfile);
			}
			else {
				BoxerInfo("Failed to write AGKPlayer.js file:", "Export Error");
				Valid = false;
			}
		}
		if (Valid) {
			// reuse variables
			//if (html5data_file) g_free(html5data_file);
			//if (agkplayer_file) g_free(agkplayer_file);

			//      // create zip file
			//      if ( !mz_zip_writer_init_file( &zip_archive, output_file, 0 ) )
			//      {
			//         SHOW_ERR( "Failed to initialise zip file for writing" );
			//         goto html5_dialog_cleanup2;
			//      }
			//
			//      // copy files to zip
			//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.asm.js", NULL );
			//      mz_zip_writer_add_file( &zip_archive, "AGKPlayer.asm.js", html5data_file, NULL, 0, 9 );
			//      g_free( html5data_file );
			//
			//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.js", NULL );
			//      mz_zip_writer_add_file( &zip_archive, "AGKPlayer.js", html5data_file, NULL, 0, 9 );
			//      g_free( html5data_file );
			//
			//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.data", NULL );
			//      mz_zip_writer_add_file( &zip_archive, "AGKPlayer.data", html5data_file, NULL, 0, 9 );
			//      g_free( html5data_file );
			//
			//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.html.mem", NULL );
			//      mz_zip_writer_add_file( &zip_archive, "AGKPlayer.html.mem", html5data_file, NULL, 0, 9 );
			//      g_free( html5data_file );
			//
			//      // create main html5 file with project name so it stands out as the file to run
			//      agkplayer_file = g_new0( gchar, 1024 );
			//      strcpy( agkplayer_file, app->project->name );
			//      utils_str_replace_char( agkplayer_file, ' ', '_' );
			//      strcat( agkplayer_file, ".html" );
			//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.html", NULL );
			//      mz_zip_writer_add_file( &zip_archive, agkplayer_file, html5data_file, NULL, 0, 9 );


#ifdef AGK_WINDOWS
			_mkdir(output_file.GetStr());
#else
			mkdir(output_file.GetStr(), S_IRWXU);
#endif

			// copy files to folder
			html5data_file = tmp_folder; html5data_file.Append("/AGKPlayer.asm.js");
			agkplayer_file = output_file; agkplayer_file.Append("/AGKPlayer.asm.js");
			cp_copyfile((char *)html5data_file.GetStr(), (char *)agkplayer_file.GetStr());

			html5data_file = tmp_folder; html5data_file.Append("/AGKPlayer.js");
			agkplayer_file = output_file; agkplayer_file.Append("/AGKPlayer.js");
			cp_copyfile((char *)html5data_file.GetStr(), (char *)agkplayer_file.GetStr());

			html5data_file = tmp_folder; html5data_file.Append("/AGKPlayer.data");
			agkplayer_file = output_file; agkplayer_file.Append("/AGKPlayer.data");
			cp_copyfile((char *)html5data_file.GetStr(), (char *)agkplayer_file.GetStr());

			html5data_file = tmp_folder; html5data_file.Append("/AGKPlayer.html.mem");
			agkplayer_file = output_file; agkplayer_file.Append("/AGKPlayer.html.mem");
			cp_copyfile((char *)html5data_file.GetStr(), (char *)agkplayer_file.GetStr());

			html5data_file = tmp_folder; html5data_file.Append("/agks-logo-white.svg");
			agkplayer_file = output_file; agkplayer_file.Append("/agks-logo-white.svg");
			cp_copyfile((char *)html5data_file.GetStr(), (char *)agkplayer_file.GetStr());

			html5data_file = tmp_folder; html5data_file.Append("/dots.png");
			agkplayer_file = output_file; agkplayer_file.Append("/dots.png");
			cp_copyfile((char *)html5data_file.GetStr(), (char *)agkplayer_file.GetStr());


			// create main html5 file with project name so it stands out as the file to run
			html5data_file = pCurrentSelectedProject->m_sProjectFile;
			int pos = html5data_file.RevFindStr(".");
			if (pos > 0) {
				uString tmp;
				html5data_file.SubString(tmp, 0, pos);
				html5data_file = tmp;
			}
			html5data_file.ReplaceStr(" ", "_");
			html5data_file.Append(".html");
			agkplayer_file = output_file; agkplayer_file.Append("/");
			agkplayer_file.Append(html5data_file);
			html5data_file = tmp_folder; html5data_file.Append("/AGKPlayer.html");
			cp_copyfile((char *)html5data_file.GetStr(), (char *)agkplayer_file.GetStr());


			//      if ( !mz_zip_writer_finalize_archive( &zip_archive ) )
			//      {
			//         SHOW_ERR( _("Failed to finalize zip file") );
			//         goto html5_dialog_cleanup2;
			//      }
			//      if ( !mz_zip_writer_end( &zip_archive ) )
			//      {
			//         SHOW_ERR( _("Failed to end zip file") );
			//         goto html5_dialog_cleanup2;
			//      }

		}

		if (newcontents) free(newcontents);
		if (contents) free(contents);
		if (load_package_string) free(load_package_string);
		if (additional_folders_string) free(additional_folders_string);
		if (pHTML5File) fclose(pHTML5File);

#ifdef AGK_WINDOWS
		_chdir(curDir);
#else
		chdir(curDir);
#endif

		//Delete all content of build_tmp if exists.
		utils_remove_folder_recursive(tmp_folder.GetStr());

		if (Valid) {
			show_html5export_window = false;
		}
	}
	
	ImGui::Indent(-8.0f);

	ImGui::End();

}



bool utils_add_folder_to_html5_data_file(FILE *pHTML5Data, const char* srcfull, const char* src, char* load_package_string, char* additional_folders_string, int* currpos)
{
	if (pHTML5Data == NULL) return(false);
	if (srcfull == NULL) return(false);
	if (src == NULL) return(false);
	if (load_package_string == NULL) return(false);
	if (additional_folders_string == NULL) return(false);
	if (currpos == NULL) return(false);


	char *oldpath, *oldfirstpath, *ctmp, *cftmp;
	oldfirstpath = agk::GetWritePath();
	if (strlen(srcfull) > 0)
		agk::SetRawWritePath(srcfull);


	uString path = srcfull;

	if (path.GetLength() > 0)
	{
		oldpath = agk::GetWritePath();
		agk::SetRawWritePath(path.GetStr());

		int mcf = 0;
		cftmp = agk::GetFirstFile(1);
		while (cftmp && strlen(cftmp) > 0 && mcf < 9999) {

			uString fullFilePath = ""; //"raw:";
			fullFilePath.Append(path);
			fullFilePath.Append("/");
			fullFilePath.Append(cftmp);
			fullFilePath.ReplaceStr("\\", "/");

			//Add file

			if (file_exists((char *)fullFilePath.GetStr())) {

				//char* filepath = g_build_path("/", srcfull, filename, NULL);
				//char* shortfilepath = g_build_path("/", src, filename, NULL);
				uString shortfilepath = src;
				shortfilepath.Append("/");
				shortfilepath.Append(cftmp);

				char *contents = 0;
				int length = 0;

				if (!(contents=g_file_get_contents((char *)fullFilePath.GetStr(), contents, &length, NULL)))
				{
					if (contents) free(contents);
					BoxerInfo((char *)fullFilePath.GetStr(),"Failed to open file ");
					return(false);
				}

				if (length > 0 && fwrite(contents, 1, length, pHTML5Data) != length)
				{
					if (contents) free(contents);
					BoxerInfo((char *)fullFilePath.GetStr(), "Failed to convert file ");
					return(false);
				}

				if (contents) free(contents);

				int audio = 0;
				char *ext = strrchr((char *)fullFilePath.GetStr(), '.');
				if (ext)
				{
					if (pestrcasestr(ext, ".mp3") ) audio = 1;
					if (pestrcasestr(ext, ".m4a") ) audio = 1;
					if (pestrcasestr(ext, ".wav") ) audio = 1;
					if (pestrcasestr(ext, ".ogg") ) audio = 1;
				}

				// append file data to load packing string
				char str[30];
				strcat(load_package_string, "{\"audio\":"); strcat(load_package_string, audio ? "1" : "0");
				strcat(load_package_string, ",\"start\":"); sprintf(str, "%d", *currpos); strcat(load_package_string, str);
				strcat(load_package_string, ",\"crunched\":0");
				strcat(load_package_string, ",\"end\":"); sprintf(str, "%d", *currpos + length); strcat(load_package_string, str);
				strcat(load_package_string, ",\"filename\":\""); strcat(load_package_string, shortfilepath); strcat(load_package_string, "\"},");

				*currpos += length;

			}

			delete[] cftmp;
			cftmp = agk::GetNextFile();
			mcf++;
		}

		agk::SetRawWritePath(oldpath);
	}


	int mc = 0;
	ctmp = agk::GetFirstFolder(1);

	//precache all strings.
	char *cached_str[999];
	cached_str[0] = 0;
	while (ctmp && strlen(ctmp) > 0 && mc < 999) {
		cached_str[mc] = ctmp;
		mc++;
		cached_str[mc] = 0;
		ctmp = agk::GetNextFolder();
	}

	mc = 0;
	ctmp = cached_str[mc];
	while (ctmp && strlen(ctmp) > 0 && mc < 999 - 1) {
		uString fullpath = agk::GetWritePath();
		fullpath.Append(ctmp);
		fullpath.ReplaceStr("\\", "/");

		agk::Sleep(1); // To prevent "app dont respond"

		uString shortfilepath = src;
		shortfilepath.Append("/");
		shortfilepath.Append(ctmp);

		strcat(additional_folders_string, "Module[\"FS_createPath\"](\"");
		strcat(additional_folders_string, src);
		strcat(additional_folders_string, "\", \"");
		strcat(additional_folders_string, ctmp);
		strcat(additional_folders_string, "\", true, true);");

		if (!utils_add_folder_to_html5_data_file(pHTML5Data, fullpath.GetStr(), shortfilepath.GetStr(), load_package_string, additional_folders_string, currpos))
		{
			delete[] cached_str[mc];
			return(false);
		}

		delete[] cached_str[mc];
		mc++;
		ctmp = cached_str[mc];
	}

	agk::SetRawWritePath(oldfirstpath);

	return true;
}


/*

static void on_html5_dialog_response(GtkDialog *dialog, gint response, gpointer user_data)
{
   static int running = 0;
   if ( running ) return;

   running = 1;

   // save current values
   if ( app->project )
   {
      GtkWidget *widget;
      widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_commands_combo");
      app->project->html5_settings.commands_used = gtk_combo_box_get_active(GTK_COMBO_BOX_TEXT(widget));
      
      widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_dynamic_memory");
      app->project->html5_settings.dynamic_memory = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(widget) );
            
      // output
      widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_output_file_entry");
      AGK_CLEAR_STR(app->project->html5_settings.output_path) = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
   }

   if ( response != 1 )
   {
      gtk_widget_hide(GTK_WIDGET(dialog));
   }
   else
   {
      int i;
      GtkWidget *widget;

      gtk_widget_set_sensitive( ui_lookup_widget(ui_widgets.html5_dialog, "html5_export1"), FALSE );
      gtk_widget_set_sensitive( ui_lookup_widget(ui_widgets.html5_dialog, "button12"), FALSE );
      
      while (gtk_events_pending())
         gtk_main_iteration();

      // app details
      widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_commands_combo");
      int html5_command_int = gtk_combo_box_get_active(GTK_COMBO_BOX_TEXT(widget));
      int commands_mode = -1;
      if ( html5_command_int == 1 ) commands_mode = 1;
      else if ( html5_command_int == 0 ) commands_mode = 0;

      widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_dynamic_memory");
      int dynamic_memory = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(widget) );
            
      // output
      widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_output_file_entry");
      gchar *output_file = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));

      // START CHECKS

      if ( !output_file || !*output_file ) { SHOW_ERR(_("You must choose an output location to save your HTML5 files")); goto html5_dialog_clean_up; }
      if ( commands_mode < 0 ) { SHOW_ERR(_("Unrecognised choice for 'commands used' drop down box")); goto html5_dialog_clean_up; }

      goto html5_dialog_continue;

html5_dialog_clean_up:
      if ( output_file ) g_free(output_file);

      gtk_widget_set_sensitive( ui_lookup_widget(ui_widgets.html5_dialog, "html5_export1"), TRUE );
      gtk_widget_set_sensitive( ui_lookup_widget(ui_widgets.html5_dialog, "button12"), TRUE );
      running = 0;
      return;

html5_dialog_continue:

      while (gtk_events_pending())
         gtk_main_iteration();

      // CHECKS COMPLETE, START EXPORT

      // make temporary folder
      gchar* tmp_folder = g_build_filename( app->project->base_path, "build_tmp", NULL );
      utils_str_replace_char( tmp_folder, '\\', '/' );
      
      const gchar *szCommandsFolder = "";
      if ( dynamic_memory ) szCommandsFolder = commands_mode ? "3Ddynamic" : "2Ddynamic";
      else szCommandsFolder = commands_mode ? "3D" : "2D";

      gchar* src_folder = g_build_path( "/", app->datadir, "html5", szCommandsFolder, NULL );
      utils_str_replace_char( src_folder, '\\', '/' );

      // decalrations
      gchar sztemp[30];
      gchar *newcontents = 0;
      gchar *load_package_string = g_new0( gchar, 200000 );
      gchar *additional_folders_string = g_new0( gchar, 200000 );
      gchar* agkplayer_file = NULL;
      gchar* html5data_file = NULL;
      gchar *contents = NULL;
      gchar *contents2 = NULL;
      gchar *contents3 = NULL;
      gsize length = 0;
      GError *error = NULL;
      FILE *pHTML5File = 0;
      gchar *media_folder = 0;

      mz_zip_archive zip_archive;
      memset(&zip_archive, 0, sizeof(zip_archive));
      gchar *str_out = NULL;
            
      if ( !utils_copy_folder( src_folder, tmp_folder, TRUE, NULL ) )
      {
         SHOW_ERR( _("Failed to copy source folder") );
         goto html5_dialog_cleanup2;
      }

      while (gtk_events_pending())
         gtk_main_iteration();

      // create HTML5 data file that we'll add all the media files to
      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.data", NULL );
      pHTML5File = fopen( html5data_file, "wb" );
      if ( !pHTML5File )
      {
         SHOW_ERR( _("Failed to open HTML5 data file for writing") );
         goto html5_dialog_cleanup2;
      }

      // start the load package string that will store the list of files, it will be built at the same time as adding the media files
      strcpy( load_package_string, "loadPackage({\"files\":[" );
      strcpy( additional_folders_string, "Module[\"FS_createPath\"](\"/\", \"media\", true, true);" );
      media_folder = g_build_path( "/", app->project->base_path, "media", NULL );
      int currpos = 0;

      if ( g_file_test (media_folder, G_FILE_TEST_EXISTS) )
      {
         // add the media files and construct the load package string, currpos will have the total data size afterwards
         if ( !utils_add_folder_to_html5_data_file( pHTML5File, media_folder, "/media", load_package_string, additional_folders_string, &currpos ) )
         {
            fclose( pHTML5File );
            pHTML5File = 0;

            SHOW_ERR( _("Failed to write HTML5 data file") );
            goto html5_dialog_cleanup2;
         }
      }

      fclose( pHTML5File );
      pHTML5File = 0;

      // remove the final comma that was added
      if ( *load_package_string && load_package_string[strlen(load_package_string)-1] == ',' ) load_package_string[ strlen(load_package_string) - 1 ] = 0;

      // finsh the load package string 
      strcat( load_package_string, "],\"remote_package_size\":" );
      sprintf( sztemp, "%d", currpos );
      strcat( load_package_string, sztemp );
      strcat( load_package_string, ",\"package_uuid\":\"e3c8dd30-b68a-4332-8c93-d0cf8f9d28a0\"})" );

      
      // edit AGKplayer.js to add our load package string
      agkplayer_file = g_build_path( "/", tmp_folder, "AGKPlayer.js", NULL );

      if ( !g_file_get_contents( agkplayer_file, &contents, &length, &error ) )
      {
         SHOW_ERR1( _("Failed to read AGKPlayer.js file: %s"), error->message );
         g_error_free(error);
         error = NULL;
         goto html5_dialog_cleanup2;
      }

      newcontents = g_new0( gchar, length + 400000 );

      contents2 = contents;
      contents3 = 0;

      // the order of these relacements is important (if more than one), they must occur in the same order as they occur in the file

      // replace %%ADDITIONALFOLDERS%%
      contents3 = strstr( contents2, "%%ADDITIONALFOLDERS%%" );
      if ( contents3 )
      {
         *contents3 = 0;
         contents3 += strlen("%%ADDITIONALFOLDERS%%");

         strcat( newcontents, contents2 );
         strcat( newcontents, additional_folders_string );
                  
         contents2 = contents3;
      }
      else
      {
         SHOW_ERR( _("AGKPlayer.js is corrupt, it is missing the %%ADDITIONALFOLDERS%% variable") );
         goto html5_dialog_cleanup2;
      }

      // replace %%LOADPACKAGE%%
      contents3 = strstr( contents2, "%%LOADPACKAGE%%" );
      if ( contents3 )
      {
         *contents3 = 0;
         contents3 += strlen("%%LOADPACKAGE%%");

         strcat( newcontents, contents2 );
         strcat( newcontents, load_package_string );
                  
         contents2 = contents3;
      }
      else
      {
         SHOW_ERR( _("AGKPlayer.js is corrupt, it is missing the %%LOADPACKAGE%% variable") );
         goto html5_dialog_cleanup2;
      }

      // write the rest of the file
      strcat( newcontents, contents2 );
   
      // write new AGKPlayer.js file
      if ( !g_file_set_contents( agkplayer_file, newcontents, strlen(newcontents), &error ) )
      {
         SHOW_ERR1( _("Failed to write AGKPlayer.js file: %s"), error->message );
         g_error_free(error);
         error = NULL;
         goto html5_dialog_cleanup2;
      }

      while (gtk_events_pending())
         gtk_main_iteration();

      // reuse variables
      if ( html5data_file ) g_free(html5data_file);
      if ( agkplayer_file ) g_free(agkplayer_file);

//      // create zip file
//      if ( !mz_zip_writer_init_file( &zip_archive, output_file, 0 ) )
//      {
//         SHOW_ERR( "Failed to initialise zip file for writing" );
//         goto html5_dialog_cleanup2;
//      }
//
//      // copy files to zip
//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.asm.js", NULL );
//      mz_zip_writer_add_file( &zip_archive, "AGKPlayer.asm.js", html5data_file, NULL, 0, 9 );
//      g_free( html5data_file );
//
//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.js", NULL );
//      mz_zip_writer_add_file( &zip_archive, "AGKPlayer.js", html5data_file, NULL, 0, 9 );
//      g_free( html5data_file );
//
//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.data", NULL );
//      mz_zip_writer_add_file( &zip_archive, "AGKPlayer.data", html5data_file, NULL, 0, 9 );
//      g_free( html5data_file );
//
//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.html.mem", NULL );
//      mz_zip_writer_add_file( &zip_archive, "AGKPlayer.html.mem", html5data_file, NULL, 0, 9 );
//      g_free( html5data_file );
//
//      // create main html5 file with project name so it stands out as the file to run
//      agkplayer_file = g_new0( gchar, 1024 );
//      strcpy( agkplayer_file, app->project->name );
//      utils_str_replace_char( agkplayer_file, ' ', '_' );
//      strcat( agkplayer_file, ".html" );
//      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.html", NULL );
//      mz_zip_writer_add_file( &zip_archive, agkplayer_file, html5data_file, NULL, 0, 9 );


      utils_mkdir( output_file, TRUE );

      // copy files to folder
      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.asm.js", NULL );
      agkplayer_file = g_build_path( "/", output_file, "AGKPlayer.asm.js", NULL );
      utils_copy_file( html5data_file, agkplayer_file, TRUE, NULL );
      g_free( agkplayer_file );
      g_free( html5data_file );

      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.js", NULL );
      agkplayer_file = g_build_path( "/", output_file, "AGKPlayer.js", NULL );
      utils_copy_file( html5data_file, agkplayer_file, TRUE, NULL );
      g_free( agkplayer_file );
      g_free( html5data_file );

      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.data", NULL );
      agkplayer_file = g_build_path( "/", output_file, "AGKPlayer.data", NULL );
      utils_copy_file( html5data_file, agkplayer_file, TRUE, NULL );
      g_free( agkplayer_file );
      g_free( html5data_file );

      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.html.mem", NULL );
      agkplayer_file = g_build_path( "/", output_file, "AGKPlayer.html.mem", NULL );
      utils_copy_file( html5data_file, agkplayer_file, TRUE, NULL );
      g_free( agkplayer_file );
      g_free( html5data_file );

      html5data_file = g_build_path( "/", tmp_folder, "background.jpg", NULL );
      agkplayer_file = g_build_path( "/", output_file, "background.jpg", NULL );
      utils_copy_file( html5data_file, agkplayer_file, TRUE, NULL );
      g_free( agkplayer_file );
      g_free( html5data_file );

      html5data_file = g_build_path( "/", tmp_folder, "made-with-appgamekit.png", NULL );
      agkplayer_file = g_build_path( "/", output_file, "made-with-appgamekit.png", NULL );
      utils_copy_file( html5data_file, agkplayer_file, TRUE, NULL );
      g_free( agkplayer_file );
      g_free( html5data_file );

      // create main html5 file with project name so it stands out as the file to run
      html5data_file = g_new0( gchar, 1024 );
      strcpy( html5data_file, app->project->name );
      utils_str_replace_char( html5data_file, ' ', '_' );
      strcat( html5data_file, ".html" );
      agkplayer_file = g_build_path( "/", output_file, html5data_file, NULL );
      g_free( html5data_file );
      html5data_file = g_build_path( "/", tmp_folder, "AGKPlayer.html", NULL );
      utils_copy_file( html5data_file, agkplayer_file, TRUE, NULL );

      while (gtk_events_pending())
         gtk_main_iteration();

      
//      if ( !mz_zip_writer_finalize_archive( &zip_archive ) )
//      {
//         SHOW_ERR( _("Failed to finalize zip file") );
//         goto html5_dialog_cleanup2;
//      }
//      if ( !mz_zip_writer_end( &zip_archive ) )
//      {
//         SHOW_ERR( _("Failed to end zip file") );
//         goto html5_dialog_cleanup2;
//      }
      

      while (gtk_events_pending())
         gtk_main_iteration();

      gtk_widget_hide(GTK_WIDGET(dialog));

html5_dialog_cleanup2:
        
        gtk_widget_set_sensitive( ui_lookup_widget(ui_widgets.html5_dialog, "html5_export1"), TRUE );
      gtk_widget_set_sensitive( ui_lookup_widget(ui_widgets.html5_dialog, "button12"), TRUE );

      utils_remove_folder_recursive( tmp_folder );

      if ( newcontents ) g_free(newcontents);
      if ( contents ) g_free(contents);
      if ( load_package_string ) g_free(load_package_string);
      if ( additional_folders_string ) g_free(additional_folders_string);
      if ( agkplayer_file ) g_free(agkplayer_file);
      if ( html5data_file ) g_free(html5data_file);
      if ( media_folder ) g_free(media_folder);
      if ( pHTML5File ) fclose(pHTML5File);

      if ( error ) g_error_free(error);
      
      if ( tmp_folder ) g_free(tmp_folder);
      if ( src_folder ) g_free(src_folder);
      if ( output_file ) g_free(output_file);
   }

   running = 0;
}

void project_export_html5()
{
   static gchar *last_proj_path = 0;

   if ( !app->project ) 
   {
      SHOW_ERR( _("You must have a project open to export it") );
      return;
   }

   // make sure the project is up to date
   build_compile_project(0);

   if (ui_widgets.html5_dialog == NULL)
   {
      ui_widgets.html5_dialog = create_html5_dialog();
      gtk_widget_set_name(ui_widgets.html5_dialog, _("Export HTML5"));
      gtk_window_set_transient_for(GTK_WINDOW(ui_widgets.html5_dialog), GTK_WINDOW(main_widgets.window));

      g_signal_connect(ui_widgets.html5_dialog, "response", G_CALLBACK(on_html5_dialog_response), NULL);
        g_signal_connect(ui_widgets.html5_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

      //ui_setup_open_button_callback_html5(ui_lookup_widget(ui_widgets.html5_dialog, "html5_app_icon_path"), NULL,
      //   GTK_FILE_CHOOSER_ACTION_OPEN, GTK_ENTRY(ui_lookup_widget(ui_widgets.html5_dialog, "html5_app_icon_entry")));
      
      ui_setup_open_button_callback_html5(ui_lookup_widget(ui_widgets.html5_dialog, "html5_output_file_path"), NULL,
         GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_ENTRY(ui_lookup_widget(ui_widgets.html5_dialog, "html5_output_file_entry")));

      gtk_combo_box_set_active( GTK_COMBO_BOX(ui_lookup_widget(ui_widgets.html5_dialog, "html5_commands_combo")), 0 );
   }

   if ( strcmp( FALLBACK(last_proj_path,""), FALLBACK(app->project->file_name,"") ) != 0 )
   {
      if ( last_proj_path ) g_free(last_proj_path);
      last_proj_path = g_strdup( FALLBACK(app->project->file_name,"") );
   
      GtkWidget *widget;

      // set defaults for this project
      widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_commands_combo");
      gtk_combo_box_set_active( GTK_COMBO_BOX(widget), app->project->html5_settings.commands_used );

      widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_dynamic_memory");
      gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(widget), app->project->html5_settings.dynamic_memory ? 1 : 0 );
      
      if ( !app->project->html5_settings.output_path || !*app->project->html5_settings.output_path )
      {
         gchar* html5_path = g_build_filename( app->project->base_path, "HTML5", NULL );
         widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_output_file_entry");
         gtk_entry_set_text( GTK_ENTRY(widget), html5_path );
         g_free(html5_path);
      }
      else
      {
         widget = ui_lookup_widget(ui_widgets.html5_dialog, "html5_output_file_entry");
         gtk_entry_set_text( GTK_ENTRY(widget), app->project->html5_settings.output_path );
      }
   }

   gtk_window_present(GTK_WINDOW(ui_widgets.html5_dialog));
}



gboolean utils_add_folder_to_html5_data_file( FILE *pHTML5Data, const gchar* srcfull, const gchar* src, gchar* load_package_string, gchar* additional_folders_string, int* currpos )
{
   g_return_val_if_fail (pHTML5Data != NULL, FALSE);
   g_return_val_if_fail (srcfull != NULL, FALSE);
   g_return_val_if_fail (src != NULL, FALSE);
    g_return_val_if_fail (load_package_string != NULL, FALSE);
   g_return_val_if_fail (additional_folders_string != NULL, FALSE);
   g_return_val_if_fail (currpos != NULL, FALSE);

   if (!g_file_test (srcfull, G_FILE_TEST_EXISTS)) {
        g_critical (G_STRLOC ": Location '%s' not found.", srcfull);
        return FALSE;
    }

   if (!g_file_test (srcfull, G_FILE_TEST_IS_DIR)) {
        g_critical (G_STRLOC ": Location '%s' is not a directory.", srcfull);
        return FALSE;
    }

   // scan the directory
   const gchar *filename;
   GDir *dir = g_dir_open(srcfull, 0, NULL);
   if (dir == NULL)
   {
      g_critical (G_STRLOC ": Failed to open directory '%s'", srcfull);
      return FALSE;
   }

   foreach_dir(filename, dir)
   {
      gchar* filepath = g_build_path( "/", srcfull, filename, NULL );
      gchar* shortfilepath = g_build_path( "/", src, filename, NULL );
         
      if ( g_file_test( filepath, G_FILE_TEST_IS_DIR ) )
      {
         strcat( additional_folders_string, "Module[\"FS_createPath\"](\"" ); 
         strcat( additional_folders_string, src ); 
         strcat( additional_folders_string, "\", \"" );
         strcat( additional_folders_string, filename );
         strcat( additional_folders_string, "\", true, true);" );

         if ( !utils_add_folder_to_html5_data_file( pHTML5Data, filepath, shortfilepath, load_package_string, additional_folders_string, currpos ) ) 
         {
            g_dir_close(dir);
            g_free(filepath);
            g_free(shortfilepath);
            return FALSE;
         }
      }
      else if ( g_file_test( filepath, G_FILE_TEST_IS_REGULAR ) )
      {
         gchar *contents = 0;
         gsize length = 0;

         if ( !g_file_get_contents( filepath, &contents, &length, NULL ) )
         {
            g_dir_close(dir);
            g_free(filepath);
            g_free(shortfilepath);
            if ( contents ) g_free(contents);

            g_critical (G_STRLOC ": Failed to open file '%s'", filepath);
            return FALSE;
         }
         
         if ( length > 0 && fwrite( contents, 1, length, pHTML5Data ) != length )
         {
            g_dir_close(dir);
            g_free(filepath);
            g_free(shortfilepath);
            g_free(contents);

            g_critical (G_STRLOC ": Failed to convert file '%s' to HTML5 data", filepath);
            return FALSE;
         }

         g_free(contents);

         int audio = 0;
         gchar *ext = strrchr( filename, '.' );
         if ( ext )
         {
            if ( utils_str_casecmp( ext, ".mp3" ) == 0 ) audio = 1;
            if ( utils_str_casecmp( ext, ".m4a" ) == 0 ) audio = 1;
            if ( utils_str_casecmp( ext, ".wav" ) == 0 ) audio = 1;
            if ( utils_str_casecmp( ext, ".ogg" ) == 0 ) audio = 1;
         }

         // append file data to load packing string
         char str[ 30 ];
         strcat( load_package_string, "{\"audio\":" ); strcat( load_package_string, audio ? "1" : "0" );
         strcat( load_package_string, ",\"start\":" ); sprintf( str, "%d", *currpos ); strcat( load_package_string, str );
         strcat( load_package_string, ",\"crunched\":0" ); 
         strcat( load_package_string, ",\"end\":" ); sprintf( str, "%d", *currpos+length ); strcat( load_package_string, str );
         strcat( load_package_string, ",\"filename\":\"" ); strcat( load_package_string, shortfilepath ); strcat( load_package_string, "\"}," );

         *currpos += length;
      }

      g_free(filepath);
      g_free(shortfilepath);
   }

   g_dir_close(dir);
   
   return TRUE;
}


*/





void ProcessAdditionalFiles(void)
{
	if (!show_additionalfiles_window)
		return;

	int input_indent = 13 * ImGui::GetFontSize();;
	ImGuiWindowFlags window_flags = 0;
	ImGui::SetNextWindowSize(ImVec2(38 * ImGui::GetFontSize(), 27 * ImGui::GetFontSize()), ImGuiCond_Once);
	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);;
	ImGui::Begin("Additional Files.", &show_additionalfiles_window, window_flags);

	//if (show_keystore_window_state != show_keystore_window) {
	//	show_keystore_window_state = show_keystore_window;
	//	ImGui::SetKeyboardFocusHere();
	//}

	ImGui::Indent(8.0);
	ImGui::Spacing();

	ImVec4* style_colors = ImGui::GetStyle().Colors;
	ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
	ImGui::SetWindowFontScale(1.1);
	style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
	ImGui::TextWrapped("Additional Files:");
	style_colors[ImGuiCol_Text] = oldTextColor;
	ImGui::SetWindowFontScale(1.0);

	ImGui::Indent(10.0);

/*
AGK comes with a set of example projects for BASIC and a set of native libraries for development with C++. By default these are located in the AGK installation folder but it might be more convenient to store them elsewhere, and in the case of Windows the Program Files directory is not writeable so is not suitable for projects. This dialog will let you copy these additional files to a location of your choice, and optionally keep those locations up to date as the local files are updated.
You can close this dialog and come back to it at any time from the Tools-&gt;Install Additional Files menu option. You can leave either path blank if you do not want to install these files.
*/

#if defined(AGK_WINDOWS)
	ImGui::TextWrapped("AppGameKit Studio comes with a set of example projects for BASIC and a set of native libraries for development with C++. By default these are located in the AGK Studio installation folder but it might be more convenient to store them elsewhere, and if AGK Studio has been installed in the Program Files directory then this location is not writeable so is not suitable for projects. This dialog will let you copy these additional files to a location of your choice, and optionally keep those locations up to date as the files changed from version to version.");
#elif defined(AGK_MACOS)
	ImGui::TextWrapped("AppGameKit Studio comes with a set of example projects bundled inside the AppGameKitStudio.app file, but to use them the projects must be copied somewhere else on your machine. This dialog will let you copy these additional files to a location of your choice, and optionally keep those locations up to date as the files changed from version to version.");
#elif defined(AGK_LINUX)
	ImGui::TextWrapped("AppGameKit Studio comes with a set of example projects for BASIC and a set of native libraries for development with C++. By default these are located in the AGK Studio installation folder but it might be more convenient to store them elsewhere. This dialog will let you copy these additional files to a location of your choice, and optionally keep those locations up to date as the files changed from version to version.");
#else
	#error Unrecognised platform
#endif

	ImGui::TextWrapped("You can close this dialog and come back to it at any time from the Tools->Install Additional Files menu option. You can leave either path blank if you do not want to install these files.");

	ImGui::TextWrapped("");

	struct TextFilters { static int TextFiltersAZ1(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", (char)data->EventChar)) return 0; return 1; } };

	ImGui::Indent(-10.0);
	ImGui::Text("AGK Studio Projects");
	ImGui::Indent(10.0);

	if (ImGui::InputText("##AGK Studio Projects", &pref.cInstallProjectFolder[0], MAX_PATH)) {
		uString nfolder = pref.cInstallProjectFolder;
		nfolder.ReplaceStr("\\", "/");
		strcpy(pref.cInstallProjectFolder, nfolder.GetStr());
	}

	ImGui::SameLine();
	if (ImGui::Button("...")) {
		char *selectedfolder;
		selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_DIR, NULL, NULL, NULL);
		if (selectedfolder) {
			if (strlen(selectedfolder) > 0) {
				uString nfolder = selectedfolder;
				nfolder.ReplaceStr("\\", "/");
				strcpy(pref.cInstallProjectFolder, nfolder.GetStr());
			}
		}
	}


	ImGui::Indent(-10.0);
	ImGui::Text("C++ Libraries");
	ImGui::Indent(10.0);

	char curDir[MAX_PATH];
	getcwd(&curDir[0], MAX_PATH);
	
#ifdef AGK_MACOS
	uString sTier2Path = curDir;
	sTier2Path.Append("/../../../Tier 2");
#else
	extern char appFolder[MAX_PATH];
	uString sTier2Path = appFolder;
	sTier2Path.ReplaceStr("\\", "/");
	sTier2Path.Append("/Tier 2");
#endif

	if ( chdir( sTier2Path ) != 0 )
	{
		ImGui::Indent(20.0);
		ImGui::TextWrapped("The IDE cannot find the Tier 2 files, you should be able to find them in the installation folder");
		ImGui::Indent(10.0);
		pref.cInstallLibsFolder[0] = 0;
	}
	else
	{
		chdir( curDir );

		if (ImGui::InputText("##C++ Libraries", &pref.cInstallLibsFolder[0], MAX_PATH)) {
			uString nfolder = pref.cInstallLibsFolder;
			nfolder.ReplaceStr("\\", "/");
			strcpy(pref.cInstallLibsFolder, nfolder.GetStr());
		}

		ImGui::SameLine();
		if (ImGui::Button("...##second")) {
			char *selectedfolder;
			selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_DIR, NULL, NULL, NULL);
			if (selectedfolder) {
				if (strlen(selectedfolder) > 0) {
					uString nfolder = selectedfolder;
					nfolder.ReplaceStr("\\", "/");
					strcpy(pref.cInstallLibsFolder, nfolder.GetStr());
				}
			}
		}
	}

	bool btmp;
	if (pref.iInstallUpdate == 0)
		btmp = false;
	else
		btmp = true;

	ImGui::Checkbox("Keep files updated when a new version is available:", &btmp);
	
	if (btmp)
		pref.iInstallUpdate = 1;
	else
		pref.iInstallUpdate = 0;

	if (strlen(afiles_errormsg) > 0) {
		ImGui::Separator();
		ImGui::TextWrapped("ERROR: %s", afiles_errormsg);
	}


//	ImVec2 ws = ImGui::GetWindowSize();
//	ImGui::SetCursorPos(ImVec2(ws.x - (11 * ImGui::GetFontSize()), ws.y - (2 * ImGui::GetFontSize()) + ImGui::GetScrollY()));
	ImGui::Text("");
	ImGui::Separator();
	if (ImGui::Button(" Cancel ")) {
		show_additionalfiles_window = false;
		strcpy(afiles_errormsg, "");
	}
	ImGui::SameLine();
	if (ImGui::Button(" Install ")) {
		bool okinstall = true;
		if (strlen(pref.cInstallProjectFolder) < 1 && strlen(pref.cInstallLibsFolder) < 1) {
			strcpy(afiles_errormsg,"You should specify at least one folder before you install.");
			okinstall = false;
		}
		if (okinstall) {
			installprocesscount = 10;
			show_additionalfiles_window = false;
		}
	}
	
	ImGui::Indent(-10.0);
	ImGui::End();
}


void StartInstallProcess( void )
{
	if (installprocesscount > 0) {
		ImGui::OpenPopup("Installing Please Wait.");

		ImGui::SetNextWindowSize(ImVec2(34 * ImGui::GetFontSize(), 13 * ImGui::GetFontSize()), ImGuiCond_Always);
		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);

		bool openinstall = true;
		if (ImGui::BeginPopupModal("Installing Please Wait.", &openinstall, 0)) //ImGuiWindowFlags_AlwaysAutoResize
		{
			ImGui::Text("");
			if (strlen(pref.cInstallProjectFolder) > 1) {
				ImGui::Text("Installing Projects To:");
				ImGui::Text("%s", pref.cInstallProjectFolder);
			}
			if (strlen(pref.cInstallLibsFolder) > 1) {
				ImGui::Text("");
				ImGui::Text("Installing C++ Libraries To:");
				ImGui::Text("%s", pref.cInstallLibsFolder);
			}
			ImGui::Text("");
			ImGui::TextWrapped("The install process can take several minutes on some systems.");

			if (installprocesscount <= 1) {
				//Install.
				if (strlen(pref.cInstallProjectFolder) > 1) {
					//Install Projects.

					extern char appFolder[MAX_PATH];
					uString sFromPath = appFolder;
					sFromPath.ReplaceStr("\\", "/");
					uString sToPath = pref.cInstallProjectFolder;

#ifdef AGK_MACOS
					char curDir[MAX_PATH];
					getcwd(&curDir[0], MAX_PATH);
					sFromPath = curDir;
					sFromPath.Append("/media/Projects");
#else
					sFromPath.Append("/media/Projects");
#endif
					utils_copy_folder((char *)sFromPath.GetStr(), (char *)sToPath.GetStr());

					if (pref.iInstallUpdate == 1) {
						pref.cInstallVersion = INTVERSION;
					}

				}
				if (strlen(pref.cInstallLibsFolder) > 1) {
					//Install Libs.

					extern char appFolder[MAX_PATH];
					uString sFromPath = appFolder;
					sFromPath.ReplaceStr("\\", "/");
					uString sToPath = pref.cInstallLibsFolder;

#ifdef AGK_MACOS
					char curDir[MAX_PATH];
					getcwd(&curDir[0], MAX_PATH);
					sFromPath = curDir;
					sFromPath.Append("/../../../Tier 2");
#else
					sFromPath.Append("/Tier 2");
#endif
					utils_copy_folder((char *)sFromPath.GetStr(), (char *)sToPath.GetStr());

					if (pref.iInstallUpdate == 1) {
						pref.cInstallVersion = INTVERSION;
					}

				}

				installprocesscount = 0;
			}
			ImGui::EndPopup();
		}
		installprocesscount--;
	}
}



void ProcessKeyStore(void)
{
	if (!show_keystore_window )
		return;

	int input_indent = 13 * ImGui::GetFontSize();;
	ImGuiWindowFlags window_flags = 0;
	ImGui::SetNextWindowSize(ImVec2(42 * ImGui::GetFontSize(), 39 * ImGui::GetFontSize()), ImGuiCond_Once);
	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);;
	ImGui::Begin("Generate Keystore File.", &show_keystore_window, window_flags);
	if (show_keystore_window_state != show_keystore_window) {
		show_keystore_window_state = show_keystore_window;
		ImGui::SetKeyboardFocusHere();
	}

	ImGui::Indent(8.0);
	ImGui::Spacing();

	ImVec4* style_colors = ImGui::GetStyle().Colors;
	ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
	ImGui::SetWindowFontScale(1.1);
	style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
	ImGui::TextWrapped("Keystore Details:");
	style_colors[ImGuiCol_Text] = oldTextColor;
	ImGui::SetWindowFontScale(1.0);

	ImGui::Indent(10.0);
	ImGui::TextWrapped("A keystore is a container file for a private key that is used to sign your app so that it can be submitted to the Google Play or Amazon store. If you are distributing outside either of these app stores, or you just want to test on your device, then you do not need this.");
	ImGui::TextWrapped("Once you have submitted your signed app, all updates to it must be signed with the same keystore. Keystores are protected with a password, if you lose your keystore file or forget its password then you will NOT be able to submit updates to any apps that were signed with it.");

//	style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
//	ImGui::TextWrapped("If you lose your keystore file or forget its password then you will not be able to submit updates to any apps that were signed with it.");
//	style_colors[ImGuiCol_Text] = oldTextColor;

	ImGui::TextWrapped("Each of your apps could use a separate keystore, but for simplicity it is easier to have a single keystore for all your apps. For those of you that want to use this keystore with other programs the alias name will be ""mykeystore"" and the alias password will be the same as the one you type below.");
	ImGui::TextWrapped("To generate a keystore file you need to submit some information about you or your company that will tell people who signed it. You may leave these blank (except the password) but anyone inspecting the key used to sign an app may be less likely to trust it if all the fields are blank.");
	ImGui::TextWrapped("");
	ImGui::TextWrapped("You can only use A-Z and space, except for password, password cannot use double quotes.");
	ImGui::TextWrapped("");

	struct TextFilters { static int TextFiltersAZ1(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", (char)data->EventChar)) return 0; return 1; } };

	ImGui::Indent(-10.0);
	ImGui::Text("Your Full Name:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##keystore_full_name", &pref.keystore_full_name[0], 30, ImGuiInputTextFlags_CallbackCharFilter , TextFilters::TextFiltersAZ1);

	ImGui::Text("Company Name:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##keystore_company_name", &pref.keystore_company_name[0], 30, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersAZ1 );

	ImGui::Text("City:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##keystore_city_name", &pref.keystore_city_name[0], 30, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersAZ1 );

	ImGui::Text("Two Letter Country Code:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##keystore_country_code", &pref.keystore_country_code[0], 3, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::TextFiltersAZ1 );

	ImGui::Separator();

	ImGui::Text("Create a Password:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##keystore_password1", &keystore_password1[0], 30, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);

	ImGui::Text("Re-enter Password:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::InputText("##keystore_password2", &keystore_password2[0], 30, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);

	ImGui::Separator();

	ImGui::Text("Output File Location:");
	ImGui::SameLine();
	ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
	ImGui::PushItemWidth(-40);
	ImGui::InputText("##keystore_file_location", &pref.keystore_file_location[0], MAX_PATH, 0);
	ImGui::PopItemWidth();
	
	ImGui::SameLine();
	if (ImGui::Button("...")) {
		char *selectedfolder;
		selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_DIR, NULL, NULL, NULL);
		if (selectedfolder) {
			if (strlen(selectedfolder) > 0) {
				uString sTmp = &pref.keystore_file_location[0], sAddName = &pref.keystore_file_location[0];
				int pos = sTmp.RevFindStr("\\");
				if(pos < 0 )
						pos = sTmp.RevFindStr("/");
				if (pos >= 0) {
					sTmp.SubString(sAddName, pos + 1);
				}
				sTmp = selectedfolder;
				sTmp.ReplaceStr("\\", "/");
				sTmp.Append("/");
				sTmp.Append(sAddName);
				strcpy(&pref.keystore_file_location[0], sTmp.GetStr() );
			}
		}
	}

	if (strlen(keystore_errormsg) > 0) {
		ImGui::Separator();
		ImGui::TextWrapped("ERROR: %s" , keystore_errormsg);
//		ImGui::InputTextMultiline("error:", keystore_errormsg, strlen(keystore_errormsg));
	}
	//ImGui::Separator();

	ImVec2 ws = ImGui::GetWindowSize();
	ImGui::SetCursorPos(ImVec2(ws.x - (11 * ImGui::GetFontSize()), ws.y - (2 * ImGui::GetFontSize()) + ImGui::GetScrollY()));
	if (ImGui::Button(" Cancel ")) {
		show_keystore_window = false;
		show_keystore_window_state = false;
		strcpy(keystore_errormsg, "");
	}
	ImGui::SameLine();
	if (ImGui::Button(" Generate ")) {
		//Validate
		bool oksave = true;
		strcpy(keystore_errormsg, "");
		if (file_exists((char*)pref.keystore_file_location)) {
			oksave = overWriteFileBox((char*)pref.keystore_file_location);
		}
		if (strcmp(keystore_password2, keystore_password1) != 0) {
			strcpy(keystore_errormsg, "Password's do not match.");
		}
		else if (strlen(keystore_password1) < 6) {
			strcpy(keystore_errormsg, "Password must be at least 6 charecters long.");
		}
		else if (strstr( keystore_password1 , "\"") ) {
			strcpy(keystore_errormsg, "Password cannot contain double quotes.");
		}
		else if(oksave) {
			//Generate
			char curDir[MAX_PATH];
			extern char startupFolder[MAX_PATH];

			// mike - 021221 - get the path for the tool
			GetJDKPath ( );
			

#ifdef AGK_WINDOWS
			_getcwd(&curDir[0], MAX_PATH);
			_chdir(startupFolder);
			uString path_to_keytool = startupFolder;

			// mike - 021221 - old code
			//path_to_keytool.Append("/media/data/android/jre/bin/keytool.exe");

			// mike - 021221 - new location for keytool
			path_to_keytool = pref.cJDKPath;
			path_to_keytool.Append ( "keytool.exe" );
			
#else
			getcwd(&curDir[0], MAX_PATH);
			chdir(startupFolder);
		#ifdef AGK_MACOS
			uString path_to_keytool = "/usr/bin/keytool";
		#else
			//uString path_to_keytool = startupFolder;
			//path_to_keytool.Append("/media/data/android/jre/bin/keytool");

			// mike - 021221 - new location for keytool
			uString path_to_keytool = startupFolder;

			path_to_keytool = pref.cJDKPath;
			path_to_keytool.Append ( "keytool" );
		#endif
#endif
			path_to_keytool.ReplaceStr("\\", "/");

			if (file_exists((char*) path_to_keytool.GetStr() )) {

				//Parameters.
				uString sParameters;
				char cParameters[MAX_PATH];
				sprintf(cParameters, "CN=%s, O=%s, L=%s, C=%s", pref.keystore_full_name, pref.keystore_company_name, pref.keystore_city_name, pref.keystore_country_code);

				//path_to_keytool
				sParameters = "";
				//sParameters.Append("\"");
				//sParameters.Append(path_to_keytool);
				//sParameters.Append("\" ");

				sParameters.Append("-genkey -keystore ");
				sParameters.Append("\"");
				sParameters.Append(pref.keystore_file_location);
				sParameters.Append("\"");
				sParameters.Append(" -alias \"mykeystore\"");
				sParameters.Append(" -keyalg \"RSA\"");
				sParameters.Append(" -keysize \"2048\"");
				sParameters.Append(" -validity \"20000\"");
				sParameters.Append(" -storepass ");
				sParameters.Append("\"");
				sParameters.Append(keystore_password1);
				sParameters.Append("\"");
				sParameters.Append(" -keypass ");
				sParameters.Append("\"");
				sParameters.Append(keystore_password1);
				sParameters.Append("\"");
				sParameters.Append(" -dname ");
				sParameters.Append("\"");
				sParameters.Append(cParameters);
				sParameters.Append("\"");

				if (file_exists((char*)pref.keystore_file_location)) {
					//Backup old file.
					uString sBackFile = pref.keystore_file_location;
					sBackFile.Append(".bak");
					cp_copyfile((char *)pref.keystore_file_location, (char *)sBackFile.GetStr());
					sBackFile = "raw:";
					sBackFile.Append(pref.keystore_file_location);
					agk::DeleteFile(sBackFile);
				}

				bool bGotStdError = false;

				static int iStdOutLength, iStdErrLength;
				int appid = run_command(path_to_keytool, sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);
				if (appid > 0) {
					float timestart = agk::Timer();
					int still_running;
//#ifdef AGK_WINDOWS
					still_running = agk::GetAppRunning(appid);
					PleaseWait();
					while (still_running == 1 && agk::Timer() - timestart < 20.0 )
					{
						//agk::Swap();
#ifdef AGK_WINDOWS
						SetForegroundWindow(g_agkhWnd);
						SetFocus(g_agkhWnd);
						Sleep(200);
#else
						usleep(200);
#endif
						still_running = agk::GetAppRunning(appid);
					}
					PleaseWaitEnd();
//#else
//#endif

					if (agk::Timer() - timestart >= 20.0) {
						sprintf(keystore_errormsg, "Timeout running: \"%s\".", path_to_keytool.GetStr());
						agk::TerminateApp(appid);
					}
					else {
						//Validate.
						if (file_exists((char*)pref.keystore_file_location)) {
							//Everything ok just close window.
							show_keystore_window = false;
							show_keystore_window_state = false;
						}
						else {
							///Users/pe/Desktop/share/agklibs/apps/AGKTier2/AgkIde/build/Release/My App Name.app/Contents/Resources/media/data/android/jre/bin/keytool
							//Failed: -genkey -keystore "/Users/pe/Desktop/release.keystore" -alias "mykeystore" -keyalg "RSA" -keysize "2048" -validity "20000" -storepass "top1934" -keypass "top1934" -dname "CN=Preben Eriksen, O=PLEMSOFT, L=Koege, C=DK"
							//sParameters.GetStr()
							sprintf(keystore_errormsg, "Failed: \"%s\".", path_to_keytool.GetStr());
							//sprintf(keystore_errormsg, "Failed: \"%s\".", sParameters.GetStr() );
						}
					}

				}
				else {
					if(!bGotStdError)
						sprintf(keystore_errormsg, "Failed to start \"%s\".", path_to_keytool.GetStr());
				}
			}
			else {
				sprintf(keystore_errormsg, "Could not find keytool program, the path \"%s\" is incorrect.", path_to_keytool.GetStr());
			}

#ifdef AGK_WINDOWS
			_chdir(curDir);
#else
			chdir(curDir);
#endif

		}
	}

	ImGui::End();

}

//Process Goto Line number window.
void ProcessGotoLine(void)
{
	if (!show_gotoline_window || !m_ActiveEditor)
		return;

	ImGuiWindowFlags window_flags = 0;
	ImGui::SetNextWindowSize(ImVec2(26 * ImGui::GetFontSize(), 4 * ImGui::GetFontSize()), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);;
	ImGui::Begin("Goto Line Number", &show_gotoline_window, window_flags);
	if (show_gotoline_window_state != show_gotoline_window) {
		show_gotoline_window_state = show_gotoline_window;
		ImGui::SetKeyboardFocusHere();
		iGotoLine = 0;
	}
	if (ImGui::InputInt(" Goto line number ##tmpunique", &iGotoLine, 0, 0, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
		//Goto line.
		if (m_ActiveEditor->GetTotalLines() < iGotoLine)
			iGotoLine = m_ActiveEditor->GetTotalLines();
		if (iGotoLine < 1)
			iGotoLine = 1;

		m_ActiveEditor->SetCursorPosition(TextEditor::Coordinates(iGotoLine - 1, 0));
		m_ActiveEditor->EnsureCursorVisible();
		m_ActiveEditor->mScrollToCursor = true;
		show_gotoline_window = false;
		show_gotoline_window_state = false;
	}
	ImGui::End();
}

//Process Info box
char szInfoBoxTitle[MAX_PATH];
char szInfoBoxMsg[MAX_PATH];
bool show_infobox_window;

void ProcessInfoBox(void)
{
	if (!show_infobox_window)
		return;

	ImGuiWindowFlags window_flags = 0;
	ImGui::SetNextWindowSize(ImVec2(12 * ImGui::GetFontSize(), 9 * ImGui::GetFontSize()), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowPosCenter(ImGuiCond_Once);;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos( ImVec2( viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.35f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowViewport(viewport->ID);


	ImGui::Begin(szInfoBoxTitle, &show_infobox_window, window_flags);

	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x , ImGui::GetCursorPos().y + 30 ));
	ImGui::Indent();

	ImGui::TextWrapped(szInfoBoxMsg);
	ImGui::NewLine();
	ImVec2 ws = ImGui::GetWindowSize();
	ImGui::SetCursorPos(ImVec2( (ws.x*0.5) - 20, ImGui::GetCursorPosY() ));
	if (ImGui::Button(" OK ")) {
		show_infobox_window = false;
	}

	ImGui::End();
}

bool show_aboutbox_window = false;
char *eula = NULL;
char *eula2 = NULL;

void ProcessAboutBox(void)
{
	if (!show_aboutbox_window)
		return;

	if (!eula) {
		//Read file.
		eula = read_eula();
	}

	ImGuiWindowFlags window_flags = 0;
	ImGui::SetNextWindowSize(ImVec2(28 * ImGui::GetFontSize(), 25 * ImGui::GetFontSize()), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowPosCenter(ImGuiCond_Once);;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.35f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("About", &show_aboutbox_window, window_flags);

	ImVec4* style_colors = ImGui::GetStyle().Colors;
	ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
	ImGui::SetWindowFontScale(1.35);
	style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);

	ImVec2 ws = ImGui::GetWindowSize();
	ImGui::Spacing();
#ifdef TRIALVERSIONEXPIRES
	ImGui::Text(WINDOW_TRIALTITLE);
#else
	ImGui::Text(WINDOW_TITLE);
#endif
	style_colors[ImGuiCol_Text] = oldTextColor;
	ImGui::SetWindowFontScale(1.0);


	if (ImGui::BeginTabBar("aboutboxtabbar"))
	{
		if (ImGui::BeginTabItem(" About "))
		{
			ImGui::NewLine();
			//ImGui::Indent();

#ifdef TRIALVERSIONEXPIRES
			ImVec2 tw = ImGui::CalcTextSize(WINDOW_TRIALTITLE);
#else
			ImVec2 tw = ImGui::CalcTextSize(WINDOW_TITLE);
#endif
			ImGui::SetCursorPos(ImVec2((ws.x*0.5) - (tw.x*0.5), ImGui::GetCursorPosY()));
#ifdef TRIALVERSIONEXPIRES
			ImGui::Text(WINDOW_TRIALTITLE);
#else
			ImGui::Text(WINDOW_TITLE);
#endif

			ImGui::NewLine();

			tw = ImGui::CalcTextSize("AppGameKit Studio (c) 2019 TheGameCreators Ltd.");
			ImGui::SetCursorPos(ImVec2((ws.x*0.5) - (tw.x*0.5), ImGui::GetCursorPosY()));
			ImGui::Text("AppGameKit Studio (c) 2019 TheGameCreators Ltd.");

			tw = ImGui::CalcTextSize("All Rights Reserved.");
			ImGui::SetCursorPos(ImVec2((ws.x*0.5) - (tw.x*0.5), ImGui::GetCursorPosY()));
			ImGui::Text("All Rights Reserved.");
			ImGui::NewLine();


			tw = ImGui::CalcTextSize("AppGameKit Dev Team:");
			ImGui::SetCursorPos(ImVec2((ws.x*0.5) - (tw.x*0.5), ImGui::GetCursorPosY()));
			ImGui::Text("AppGameKit Dev Team:");

			ImGui::SetCursorPos(ImVec2((ws.x*0.5) - (tw.x*0.5), ImGui::GetCursorPosY()));
			ImGui::Text("Paul Johnston - Developer");

			ImGui::SetCursorPos(ImVec2((ws.x*0.5) - (tw.x*0.5), ImGui::GetCursorPosY()));
			ImGui::Text("Preben Eriksen - Developer");

			ImGui::SetCursorPos(ImVec2((ws.x*0.5) - (tw.x*0.5), ImGui::GetCursorPosY()));
			ImGui::Text("Rick Vanner - Producer");

			ImGui::SetCursorPos(ImVec2((ws.x*0.5) - (tw.x*0.5), ImGui::GetCursorPosY()));
			ImGui::Text("Gabor Denes - PBR Demos");

			ImGui::NewLine();
			ImGui::EndTabItem();

		}

		if (ImGui::BeginTabItem(" Contributors "))
		{


#ifdef TRIALVERSIONEXPIRES
			ImVec2 tw = ImGui::CalcTextSize(WINDOW_TRIALTITLE);
#else
			ImVec2 tw = ImGui::CalcTextSize(WINDOW_TITLE);
#endif
			ImGui::SetCursorPos(ImVec2((ws.x * 0.5) - (tw.x * 0.5), ImGui::GetCursorPosY()));
			
			ImGui::NewLine();
			tw = ImGui::CalcTextSize("Community Dev Team:");
			ImGui::SetCursorPos(ImVec2((ws.x * 0.5) - (tw.x * 0.5), ImGui::GetCursorPosY()));
			ImGui::Text("Community Dev Team:");
			
			ImGui::SetCursorPos(ImVec2((ws.x * 0.5) - (tw.x * 0.5), ImGui::GetCursorPosY()));
			ImGui::Text("David Walker - Developer");

			ImGui::NewLine();
			ImGui::EndTabItem();

		}

		if (ImGui::BeginTabItem(" EULA "))
		{
			ImGui::PushTextWrapPos(0.0f);
			ImGui::TextUnformatted(eula);
			ImGui::PopTextWrapPos();
			//ImGui::TextWrapped(eula);
			ImGui::NewLine();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::SetCursorPos(ImVec2((ws.x*0.5) - 20, ImGui::GetCursorPosY()));
	if (ImGui::Button(" OK ")) {
		show_aboutbox_window = false;
	}

	ImGui::End();
}


//########################################################
//#### Add files to recent projects and recent files. ####
//########################################################

void AddRecentFiles(const char * project, const char * file)
{
	bool bFound = false;
	if (project) {
		for (int i = 0; i < iResentProjects; i++) {
			if (szRecentProjects[i].CompareCaseTo(project) == 0)
				return;
		}

		if (iResentProjects >= MAXRECENTLIST) {
			//move up.
			for (int i = 0; i < iResentProjects - 1; i++) {
				szRecentProjects[i] = szRecentProjects[i + 1];
			}
			szRecentProjects[iResentProjects - 1] = project;
		}
		else {
			szRecentProjects[iResentProjects++] = project;
		}
	}
	if (file) {
		for (int i = 0; i < iResentProjects; i++) {
			if (szRecentFiles[i].CompareCaseTo(file) == 0)
				return;
		}

		if (iResentFiles >= MAXRECENTLIST) {
			//move up.
			for (int i = 0; i < iResentFiles - 1; i++) {
				szRecentFiles[i] = szRecentFiles[i + 1];
			}
			szRecentFiles[iResentFiles - 1] = file;
		}
		else {
			szRecentFiles[iResentFiles++] = file;
		}

	}
}


//Reset layout. Set everything to the default layout.
void ResetLayout( void )
{
	refresh_gui_docking = 0;

	//m_editor->firstrun = true;
	cProjectItem *findProjects;
	findProjects = allProjects;
	while (findProjects)
	{
		cProjectItem::sProjectFiles * m_pCurrentFile;
		m_pCurrentFile = findProjects->m_pFirstFile;

		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_bOpenForEdit && m_pCurrentFile->m_editor && m_pCurrentFile->m_editor->bEditorVisible ) {
				m_pCurrentFile->m_editor->firstrun = true;
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
		findProjects = findProjects->m_pNext;
	}
}


//Recursive go into all folders and filter entrys.
//Need a better way to display filters.
bool GetTreePopFilter(cFolderItem *m_pmSubFolders, ImGuiTextFilter Filter)
{
	cFolderItem *m_pmNextSubFolders;
	while (m_pmSubFolders)
	{
		if (Filter.PassFilter(m_pmSubFolders->m_sFolder)) {
			//if (m_pmSubFolders->m_pFirstFile) {
			ImGui::Selectable(m_pmSubFolders->m_sFolder, false);

			//			if( ImGui::IsItemClicked() )
			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
				m_pSelectedFolder = m_pmSubFolders;
			//if (isitemselect()) m_pSelectedFolder = m_pmSubFolders;
			//}
		}
		m_pmNextSubFolders = getNextFolder(m_pmSubFolders);
		GetTreePopFilter(m_pmNextSubFolders, Filter);
		m_pmSubFolders = m_pmSubFolders->m_pNext;
	}
	return true;
}

//Recursive go into all folders display tree view.
bool GetTreePop(cFolderItem *m_pmSubFolders)
{
	cFolderItem *m_pmNextSubFolders;
	while (m_pmSubFolders)
	{
		if ( m_pmSubFolders->visible == true || pref.bDisplayProjectinAssetsBrowser) {
			if (m_pmSubFolders->deletethisentry == false) {

				if (!(m_pClassicDLCFolder == m_pmSubFolders && !pref.bLoadClassicDLConStartup)) {
					if (myAssetsTreenode(m_pmSubFolders))
					{
						m_pmNextSubFolders = getNextFolder(m_pmSubFolders);

						GetTreePop(m_pmNextSubFolders);
						ImGui::TreePop();
					}
				}

			}
		}

		m_pmSubFolders = m_pmSubFolders->m_pNext;
	}
	return true;
}


cFolderItem * getNextFolder(cFolderItem * m_pCurFolder)
{
	if (m_pCurFolder && m_pCurFolder->m_pSubFolder && m_pCurFolder->m_pSubFolder->m_pNext)
		return m_pCurFolder->m_pSubFolder->m_pNext;
	else
		return NULL;
}
ImGuiTreeNodeFlags setTreeFlags(cFolderItem * cfolder)
{
	//ImGuiTreeNodeFlags_Bullet
	//ImGuiTreeNodeFlags_Leaf
	//ImGuiTreeNodeFlags_OpenOnDoubleClick
	//ImGuiTreeNodeFlags_OpenOnArrow
	//ImGuiTreeNodeFlags_Selected
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
	if (cfolder && cfolder->m_pSubFolder) {
		if (!cfolder->m_pSubFolder->m_pNext)
			node_flags = ImGuiTreeNodeFlags_Leaf;
	}
	return node_flags;
}

bool myAssetsTreenode(cFolderItem * cfolder)
{
	ImGuiTreeNodeFlags node_flags = setTreeFlags(cfolder);

	//bool is_open = ImGui::TreeNodeBehaviorIsOpen(cfolder->count, node_flags);
	char test[256];
	if (!(cfolder->iFlags & FI_FOLDERISOPEN))
		sprintf(test, ICON_MD_FOLDER " %s", cfolder->m_sFolder.GetStr());
	else
		sprintf(test, ICON_MD_FOLDER_OPEN " %s", cfolder->m_sFolder.GetStr());

	if (m_pSelectedFolder == cfolder) {
		//Selected draw background.
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	unsigned int oldflag = cfolder->iFlags;
	bool ret_value = ImGui::TreeNodeEx((void*)(intptr_t)cfolder->count, node_flags, "%s",test);
	if (ret_value)
		cfolder->iFlags |= FI_FOLDERISOPEN;
	else
		cfolder->iFlags &= ~FI_FOLDERISOPEN;

	//ImGui::IsItemClicked()
	//	if (ImGui::IsItemFocused()) {
	
	//if (ImGui::IsItemClicked()) {
	if (oldflag == cfolder->iFlags) {
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) {
			m_pSelectedFolder = cfolder;
			ImGui::SetWindowFocus(ICON_MD_PERM_MEDIA " Media files");
		}
	}

//	if (isitemselect()) {
//		m_pSelectedFolder = cfolder;
//		ImGui::SetWindowFocus(ICON_MD_PERM_MEDIA " Media files");
//	}

	if (cfolder->bIsCustomFolder) {

		char cTmp[MAX_PATH];
		sprintf(cTmp, "-##%s", cfolder->m_sFolder.GetStr());
		ImGuiWindow *pwin;

		pwin = ImGui::GetCurrentWindow();
		int iwidth = pwin->Size.x - pwin->ScrollbarSizes.x; //SizeContents.x;
		ImGui::SameLine(iwidth - 18.0f);

		if (ImGui::Button(cTmp,ImVec2(0,16))) {
			cfolder->alwaysvisible = false;
			cfolder->visible = false;
			cfolder->deletethisentry = true;
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Remove Folder");
	}


	//TEST
	if (ImGui::BeginPopupContextItemAGK()) //"project context menu"
	{
#ifdef AGK_WINDOWS
		//m_sFolderFullPath
		if (ImGui::MenuItem("Open In Explorer", "")) {
			uString sTmp = cfolder->m_sFolderFullPath;
			sTmp.ReplaceStr("/", "\\");
			ITEMIDLIST *pidl = ILCreateFromPathA(sTmp.GetStr());
			if (pidl) {
				SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
				ILFree(pidl);
			}
		}
#endif
		if (ImGui::MenuItem("Refresh Files", "")) {
			extern bool bRefreshNextFolder;
			bRefreshNextFolder = 128;
			m_pSelectedFolder = cfolder;
			ImGui::SetWindowFocus(ICON_MD_PERM_MEDIA " Media files");

		}

		ImGui::EndPopup();

	}


	return ret_value;
}

//This will not work on scaled resolutions.
bool isitemselect()
{
	if (agk::GetPointerReleased()) {

		ImVec2 item_target_window_size;
		ImVec2 item_target_window_pos;
		item_target_window_pos.x = ImGui::GetItemRectMin().x;
		item_target_window_pos.y = ImGui::GetItemRectMin().y;
		item_target_window_size.x = ImGui::GetItemRectMax().x - item_target_window_pos.x;
		item_target_window_size.y = ImGui::GetItemRectMax().y - item_target_window_pos.y;
		item_target_window_pos.x += 20.0; //Arrow up down size.
		float mx, my;
		mx = agk::GetPointerX();
		my = agk::GetPointerY();

		if (mx >= ImGui::GetWindowPos().x && my >= ImGui::GetWindowPos().y && mx <= (ImGui::GetWindowPos().x + ImGui::GetItemRectMax().x) && my <= (ImGui::GetWindowPos().y + ImGui::GetItemRectMax().y))
		{
			if (mx >= item_target_window_pos.x && mx <= (item_target_window_pos.x + item_target_window_size.x)) {
				if (my >= item_target_window_pos.y && my <= (item_target_window_pos.y + item_target_window_size.y)) {
					return true;
				}
			}
		}
	}
	return false;
}

//#################################################################
//#### PE: ImgBtn                                              ####
//#### Used to add image buttons directly from a AGK image id. ####
//#################################################################


namespace ImGui {

//PE: already used so not needed here.
//	static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x*rhs, lhs.y*rhs); }
//	static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
//	static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x*rhs.x, lhs.y*rhs.y); }


bool BeginPopupContextItemAGK(const char* str_id, int mouse_button)
{
	ImGuiWindow* window = GImGui->CurrentWindow;
	ImGuiID id = str_id ? window->GetID(str_id) : window->DC.LastItemId; // If user hasn't passed an ID, we can use the LastItemID. Using LastItemID as a Popup ID won't conflict!
	IM_ASSERT(id != 0);                                                  // You cannot pass a NULL str_id if the last item has no identifier (e.g. a Text() item)
	if (IsMouseReleased(mouse_button) && IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
		OpenPopupEx(id);
	return BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking);
}


bool windowTabVisible( void )
{
	if (!ImGui::IsWindowDocked()) return(true);
	ImGuiWindow* window = GetCurrentWindow();
	if (!window->DockNode) return(true);
	return window->DockTabIsVisible;
}
int windowTabFlags(void)
{
	//DockTabItemStatusFlags
	ImGuiWindow* window = GetCurrentWindow();
	return (int) window->DockTabItemStatusFlags;
}
int windowDockNodeId(void)
{
	//DockTabItemStatusFlags
	ImGuiWindow* window = GetCurrentWindow();
	if (!window->DockNode) return(0);
	return (int)window->DockNode->ID;
}


bool ImgBtn(int iImageID, const ImVec2& btn_size, const ImVec4& bg_col,
		const ImVec4& drawCol_normal,
		const ImVec4& drawCol_hover,
		const ImVec4& drawCol_Down, int frame_padding, int atlasindex, int atlasrows , int atlascolumns )
{

		cImage *pImage = agk::GetImagePtr(iImageID);
		if (!pImage) return false;
		int iTexID = pImage->GetTextureID() + 100000; //100000 = Transparent with colors.


		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;
		ImVec2 size = ImVec2(btn_size.x, btn_size.y);
		if (btn_size.x == 0 && btn_size.y == 0) {
			size.x = agk::GetImageWidth(iImageID);
			size.y = agk::GetImageHeight(iImageID);
		}

		ImVec2 uv0 = ImVec2(0, 0);
		ImVec2 uv1 = ImVec2(1, 1);
		if (atlasindex > 0) {
			//atlasrows
			//atlascolumns
			float asx = agk::GetImageWidth(iImageID);
			float asy = agk::GetImageHeight(iImageID);

			float uvratiox = 1.0 / (asx);
			float uvratioy = 1.0 / (asy);
			float imgsizex = asx / atlasrows;
			float imgsizey = asy / atlascolumns;

			int index_x = (int) fmod(atlasindex-1,atlasrows);
			int index_y = (atlasindex - 1) / atlasrows; //atlascolumns;

			float uvborderx = uvratiox;
			float uvbordery = uvratioy;
			uvborderx *= (imgsizex / 32);
			uvbordery *= (imgsizey / 32);

			float atlasstartx = (index_x * (imgsizex)) * uvratiox + (uvborderx*0.5);
			float atlasstarty = (index_y * (imgsizey)) * uvratioy + (uvbordery*0.5);
			float atlassizex = (imgsizex)* uvratiox - (uvborderx); //0.987
			float atlassizey = (imgsizey)* uvratioy - (uvbordery);



			uv0 = ImVec2(atlasstartx, atlasstarty);
			uv1 = ImVec2(atlasstartx+atlassizex, atlasstarty+atlassizey);
		}
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		PushID(iTexID);
		const ImGuiID id = window->GetID("#image");
		PopID();

		const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
		const ImRect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);

		ItemSize(bb);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held);
		
		//PE: Add the background color. not really needed as most buttons are transparent.
		if (bg_col.w > 0.0f)
				window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));

		window->DrawList->AddImage((ImTextureID)iTexID, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(
			(hovered && held) ? drawCol_Down : hovered ? drawCol_hover : drawCol_normal));

		if (pressed) {
			return(true);
		}

		return(pressed);
	}



bool ImgBtnBack(int iImageID, const ImVec2& btn_size, const ImVec4& bg_col,
	const ImVec4& drawCol_normal,
	const ImVec4& drawCol_hover,
	const ImVec4& drawCol_Down, int frame_padding)
	{

		cImage *pImage = agk::GetImagePtr(iImageID);
		if (!pImage) return false;
		int iTexID = pImage->GetTextureID() + 300000;


		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;
		ImVec2 size = ImVec2(btn_size.x, btn_size.y);
		if (btn_size.x == 0 && btn_size.y == 0) {
			size.x = agk::GetImageWidth(iImageID);
			size.y = agk::GetImageHeight(iImageID);
		}

		const ImVec2& uv0 = ImVec2(0, 0);
		const ImVec2& uv1 = ImVec2(1, 1);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		PushID(iTexID);
		const ImGuiID id = window->GetID("#image");
		PopID();

		const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
		const ImRect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);

		ItemSize(bb);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held);

		//PE: Add the background color. not really needed as most buttons are transparent.
		//if (bg_col.w > 0.0f)
		//	window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));

		window->DrawList->AddImage((ImTextureID)iTexID, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(
			(hovered && held) ? drawCol_Down : hovered ? drawCol_hover : drawCol_normal));

		if (pressed) {
			return(true);
		}

		return(pressed);
	}


	void ToggleButton(const char* str_id, bool* v)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec4 *style_colors = ImGui::GetStyle().Colors;

		float height = ImGui::GetFrameHeight() * 0.8;
		float bordery = ImGui::GetFrameHeight() * 0.12;
//		float width = height * 1.55f;
		float width = height * 1.7f;
		float radius = height * 0.50f;

		p.y += bordery;

		ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
//		if (ImGui::IsItemHovered())
//			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
//		else
//			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

		if (ImGui::IsItemHovered())
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(style_colors[ImGuiCol_ButtonHovered]), ImVec4(style_colors[ImGuiCol_PlotHistogram]) , t));
		else
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(style_colors[ImGuiCol_FrameBg]), ImVec4(style_colors[ImGuiCol_PlotHistogram]), t));

//		if (ImGui::IsItemHovered())
//			col_bg = ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_FrameBg]));
//		else
//			col_bg = ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotLines]));
		

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
	}


	//ImRotateStart();
	//ImGui::Text(__FUNCTION__);
	//ImRotateEnd(0.0005f*::GetTickCount());
	int rotation_start_index;

	void ImRotateStart()
	{
		rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
	}

	

	ImVec2 ImRotationCenter()
	{
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

		const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
	}
	

	//ImVec2 operator-(const ImVec2& l, const ImVec2& r) { return{ l.x - r.x, l.y - r.y }; }
	

	void ImRotateEnd(float rad, ImVec2 center)
	{
		float s = sin(rad), c = cos(rad);
		center = ImRotate(center, s, c) - center;

		auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
	}


}

ImGuiMouseCursor old_cursor;

void PleaseWait(void)
{
#ifdef AGK_WINDOWS
	old_cursor = ImGui::GetMouseCursor();
	SetCursor(::LoadCursor(NULL, IDC_WAIT));
#else

#endif
}
void PleaseWaitEnd(void)
{
#ifdef AGK_WINDOWS
	LPTSTR win32_cursor = IDC_ARROW;
	ImGuiIO& io = ImGui::GetIO();
	if (io.MouseDrawCursor) {
		ImGui::SetMouseCursor(old_cursor);
		SetCursor(NULL);
	}
	else {
		switch (old_cursor)
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
#else
#endif

}
//##########################################################################
//#### PE: AddAllFonts
//#### Used to change all fonts and sent the new font atlas to the GPU. ####
//##########################################################################

const char * oldcpcustomfont, *oldcpeditorfont;
void AddAllFonts(const char *cpcustomfont,int iIDEFontSize, const char *cpeditorfont,int iEditorFontSize)
{
	extern bool ide_force_rendering_delayed;
	ide_force_rendering_delayed = true;

	if (cpcustomfont == NULL) {
		cpcustomfont = oldcpcustomfont;
	}
	if (cpeditorfont == NULL) {
		cpeditorfont = oldcpeditorfont;
	}
	oldcpeditorfont = cpeditorfont;
	oldcpcustomfont = cpcustomfont;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->Clear();

#ifdef MINIMUMFONTSIZE
	static const ImWchar Generic_ranges_all[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0100, 0x017F,	//0100 - 017F  	Latin Extended-A
		0x0180, 0x024F,	//0180 - 024F  	Latin Extended-B
		0,
	};
#else
	static const ImWchar Generic_ranges_all[] =
	{
		0x0020, 0xFFFF, // Everything test.
		0,
	};
#endif


	static const ImWchar Generic_ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0100, 0x017F,	//0100 - 017F  	Latin Extended-A
		0x0180, 0x024F,	//0180 - 024F  	Latin Extended-B
		//0x0370, 0x03FF, //0370 - 03FF  	Greek and Coptic
						//1F00 - 1FFF  	Greek Extended
						//
		0,
	};

	static const ImWchar Generic_ranges_code[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0100, 0x017F,	//0100 - 017F  	Latin Extended-A
		0x0180, 0x024F,	//0180 - 024F  	Latin Extended-B
		0x0300, 0x036F, //Diacritical Marks : 0300 - 036F
		0x0370, 0x03FF, //0370 - 03FF  	Greek and Coptic
						//1F00 - 1FFF  	Greek Extended
		0x2500,0x257F,	//Box Drawing : 2500 - 257F
						0,
	};

	static const ImWchar ranges_Cyrillic[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0,
	};

	static const ImWchar ranges_thai[] =
	{
		0x0020, 0x00FF, // Basic Latin
		0x2010, 0x205E, // Punctuations
		0x0E00, 0x0E7F, // Thai
		0,
	};

	static const ImWchar rangesChineseFull[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x2000, 0x206F, // General Punctuation
		0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
		0x31F0, 0x31FF, // Katakana Phonetic Extensions
		0xFF00, 0xFFEF, // Half-width characters
		0x4e00, 0x9FAF, // CJK Ideograms
		0,
	};
	static const ImWchar ranges_Korean[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x3131, 0x3163, // Korean alphabets
		0xAC00, 0xD79D, // Korean characters
		0,
	};
	char curDir[MAX_PATH];
	extern char startupFolder[MAX_PATH];
#ifdef AGK_WINDOWS
	_getcwd(&curDir[0], MAX_PATH);
	_chdir(startupFolder);
#else
	getcwd(&curDir[0], MAX_PATH);
	chdir(startupFolder);
#endif
	float FONTUPSCALE;
	if (pref.bUseUpscaling) {
		FONTUPSCALE = 2.0;
	}
	else {
		FONTUPSCALE = 1.0;
	}

//file_exists
//	if (agk::GetFileExists(cpcustomfont)) {
	if (file_exists((char*) cpcustomfont)) {

		customfont = io.Fonts->AddFontFromFileTTF(cpcustomfont, iIDEFontSize*FONTUPSCALE, NULL, &Generic_ranges[0]); //Set as default font.

#ifdef IMGUIAL_FONTS_MATERIAL_DESIGN
		int ttf_size;
		const void* ttf_data = ImGuiAl::Fonts::GetCompressedData(ImGuiAl::Fonts::kMaterialDesign, &ttf_size);

		if (ttf_data) {
			static const ImWchar ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
			ImFontConfig config;
			config.MergeMode = true;
			config.PixelSnapH = true;
			if (pref.bUseUpscaling) 
				config.GlyphOffset.y += 7.0f;
			else
				config.GlyphOffset.y += 3.5f;
			customfont = io.Fonts->AddFontFromMemoryCompressedTTF(ttf_data, ttf_size, iIDEFontSize*FONTUPSCALE, &config, ranges);
		}

#endif
#ifdef IMGUIAL_FONTS_FONT_AWESOME
		//Not used yet.
		static const ImWchar ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig config;
		config.MergeMode = true;
		config.PixelSnapH = true;
		config.GlyphOffset.y += 0.5f;
		ttf_data = ImGuiAl::Fonts::GetCompressedData(ImGuiAl::Fonts::kFontAwesome, &ttf_size);
		if (ttf_data)
			io.Fonts->AddFontFromMemoryCompressedTTF(ttf_data, ttf_size, iIDEFontSize*FONTUPSCALE, &config, ranges);
#endif
		io.FontGlobalScale = 1.0f / FONTUPSCALE;
	}
	else
	{
		customfont = io.Fonts->AddFontDefault();
	}
	defaultfont = io.Fonts->AddFontDefault();


	if (strlen(pref.cCustomEditorFont) > 3 && file_exists((char *) &pref.cCustomEditorFont[0] )) {
		editorfont = io.Fonts->AddFontFromFileTTF(pref.cCustomEditorFont, iEditorFontSize*FONTUPSCALE, NULL, &Generic_ranges_all[0]); //Set as default font.
	}
	else if (file_exists((char *)cpeditorfont)) {
		editorfont = io.Fonts->AddFontFromFileTTF(cpeditorfont, iEditorFontSize*FONTUPSCALE, NULL, &Generic_ranges_all[0]); //Set as default font.

//	editorfont = io.Fonts->AddFontFromFileTTF("d:\\github\\AGKTier2\\AgkIde\\Final\\media\\fonts\\source-code-pro\\regular.ttf", iEditorFontSize*FONTUPSCALE, NULL, &Generic_ranges_all[0]); //Set as default font.

//d:\\github\\AGKTier2\\AgkIde\\Final\\media\\fonts\\source-code-pro\\regular.ttf
//best
//		editorfont = io.Fonts->AddFontFromFileTTF("media/fonts/GenJyuuGothicMonospace-Regular.ttf", iEditorFontSize*FONTUPSCALE, NULL, &Generic_ranges_all[0]); //Set as default font.

//media/fonts/cour.ttf
//		editorfont = io.Fonts->AddFontFromFileTTF(cpeditorfont, iEditorFontSize*FONTUPSCALE, NULL, io.Fonts->GetGlyphRangesChineseFull() ); //Set as default font.
	}
	else {
		editorfont = customfont;
	}


	if (file_exists("media/fonts/NotoSans-Regular.ttf")) {
		agkfont = io.Fonts->AddFontFromFileTTF("media/fonts/NotoSans-Regular.ttf", AGKFONTSIZE, NULL, &Generic_ranges[0]);
	}
	else {
		agkfont = customfont;
	}

	//Add AGK default font.
	//"NotoSans-Regular.ttf"

#ifdef AGK_WINDOWS
	_chdir(curDir);
#else
	chdir(curDir);
#endif

	if (strlen(cPreviewFont) > 0) {
		if (file_exists((char *)cPreviewFont)) {
			static const ImWchar Generic_ranges_small[] =
			{
				0x0020, 0x007F, // min.
				0,
			};
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			previewFont = io.Fonts->AddFontFromFileTTF(cPreviewFont, 20, NULL, Generic_ranges_small);
			ImGui_ImplOpenGL2_DestroyFontsTexture();
			ImGui_ImplOpenGL2_CreateFontsTexture();

		}
		else
		{
			previewFont = customfont;
		}
		bPreviewFontLoaded = true;
	}

	//Add font to scene files.
	cProjectItem * firstProjectsSearch = allProjects;
	cProjectItem::sProjectFiles * m_pCurrentFile;
	while (firstProjectsSearch)
	{
		m_pCurrentFile = firstProjectsSearch->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_bOpenForEdit && m_pCurrentFile->m_editor && m_pCurrentFile->m_editor->bEditorVisible ) {
				if (m_pCurrentFile->m_scene) {
					MediaScene * pUseScene = NULL;
					if (m_pCurrentFile->m_scene->m_pCurrentScene)
						pUseScene = m_pCurrentFile->m_scene->m_pCurrentScene;
					else
						pUseScene = m_pCurrentFile->m_scene;

					if (pUseScene && pUseScene->Fonts.size() > 0) {
						//
						for (std::map<std::int32_t, std::string>::iterator it = pUseScene->Fonts.begin(); it != pUseScene->Fonts.end(); ++it)
						{
							uString FontPath = it->second.c_str();
							//Always load here. // && !pUseScene->customfonts[it->first]
							if (it->first < MAXUSERFONTS ) {

								if (!file_exists((char *)FontPath.GetStr())) {

									//Not found , perhaps path is from a Mac or the other way around.
									//Try to resolve using relative path.
									uString fullfilename, relativeName, ext, relativeNameNoExt;
									fullfilename = FontPath.GetStr();
									fullfilename.ReplaceStr("\\", "/");

									int pos = pecasepos(fullfilename.GetStr(), "/media/");
									if (pos >= 0)
										fullfilename.SubString(relativeName, pos + 7);
									else
										relativeName = fullfilename;

									pos = relativeName.RevFindStr(".");
									ext = "na";
									if (pos > 0) {
										relativeName.SubString(ext, pos);
									}
									relativeNameNoExt = relativeName;
									pos = relativeName.RevFindStr(".");
									if (pos > 0) {
										relativeName.SubString(relativeNameNoExt, 0, pos);
									}
									uString sIName;
									if (pCurrentSelectedProject) {
										uString nfp = pCurrentSelectedProject->m_sProjectFileFullPath;
										nfp.ReplaceStr("\\", "/");
										int pos = nfp.RevFindStr("/");
										if (pos > 0) {
											uString tmp;
											nfp.SubString(tmp, 0, pos);
											tmp.Append("/media/");
											tmp.Append(relativeName);
											//sIName = "raw:";
											sIName.Append(tmp);
										}

										if (file_exists((char *)sIName.GetStr())) {
											//Update to new fullpath.
											FontPath = sIName;
										}
									}

								}

								if (file_exists((char *)FontPath.GetStr())) {
									static const ImWchar Generic_ranges[] =
									{
										0x0020, 0x00FF, // Basic Latin + Latin Supplement
										0x0100, 0x017F,	//0100 - 017F  	Latin Extended-A
										0x0180, 0x024F,	//0180 - 024F  	Latin Extended-B
										0x0370, 0x03FF, //0370 - 03FF  	Greek and Coptic
														//1F00 - 1FFF  	Greek Extended
														0,
									};
									ImGuiIO& io = ImGui::GetIO(); (void)io;
									pUseScene->customfonts[it->first] = io.Fonts->AddFontFromFileTTF(FontPath.GetStr(), 48, NULL, NULL); // &Generic_ranges[0]
									 //Rebuild atlas.
									//ImGui_ImplOpenGL2_DestroyFontsTexture();
									//ImGui_ImplOpenGL2_CreateFontsTexture();

								}
								else {
									//Not found just delete font.
									pUseScene->customfonts[it->first] = NULL;
									pUseScene->Fonts.erase(it->first);
									break;
								}

							}
						}
					}
				}

			}
			if (m_pCurrentFile)
				m_pCurrentFile = m_pCurrentFile->m_pNext;
		}

		if (firstProjectsSearch)
			firstProjectsSearch = firstProjectsSearch->m_pNext;

	}




	ImGui_ImplOpenGL2_DestroyFontsTexture();
	ImGui_ImplOpenGL2_CreateFontsTexture();

}


//##########################
//#### Seed based style ####
//##########################

void SetSeedStyleColors()
{
	extern bool ide_force_rendering_delayed;
	ide_force_rendering_delayed = true;

	ImGuiStyle& style = ImGui::GetStyle();

	static int hue = 140;
	static float col_main_sat = 180.f / 255.f;
	static float col_main_val = 161.f / 255.f;
	static float col_area_sat = 124.f / 255.f;
	static float col_area_val = 100.f / 255.f;
	static float col_back_sat = 59.f / 255.f;
	static float col_back_val = 40.f / 255.f;

	static float col_text_sat = 80.f / 255.f;
	static float col_text_val = 220.f / 255.f;

	ImVec4 col_text = ImColor::HSV(hue / 255.f, col_text_sat, col_text_val);
	ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
	ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
	ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

	agk::SetRandomSeed(pref.iSeedStyle);

	hue = agk::Random(0, 255);

	col_main_sat = agk::Random(0, 255) / 255.0f;
	col_area_sat = agk::Random(0, 255) / 255.0f;
	col_back_sat = agk::Random(0, 255) / 255.0f;
	col_text_sat = agk::Random(0, 255) / 255.0f;

	float rndval = agk::Random(50, 128) / 255.f;
	if (agk::Random(1, 2) == 1) {
		col_text = ImColor::HSV(hue / 255.f, col_text_sat, 0.5 - rndval);
		col_back = ImColor::HSV(hue / 255.f, col_back_sat, 0.5 + rndval);
	}
	else {
		col_text = ImColor::HSV(hue / 255.f, col_text_sat, 0.5 + rndval);
		col_back = ImColor::HSV(hue / 255.f, col_back_sat, 0.5 - rndval);
	}

	rndval = agk::Random(0, 128) / 255.f;
	if (agk::Random(1, 2) == 1) {
		col_main = ImColor::HSV(hue / 255.f, col_main_sat, 0.5 + rndval);
		col_area = ImColor::HSV(hue / 255.f, col_area_sat, 0.5 - rndval);
	}
	else {
		col_main = ImColor::HSV(hue / 255.f, col_main_sat, 0.5 - rndval);
		col_area = ImColor::HSV(hue / 255.f, col_area_sat, 0.5 + rndval);
	}

	style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);

	style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.65f);

	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.88f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);

	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);

	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_area.x, col_area.y, col_area.z, 0.96f);
//	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.96f);

	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);

	//style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
	//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
	//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);

	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);


	//style.Colors[ImGuiCol_TooltipBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);

	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);


	style.Colors[ImGuiCol_Tab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.66f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	
	
	style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.6f);
}



//PE: You can make custom styles like this!
void myDarkStyle(ImGuiStyle* dst)
{
	ImGui::StyleColorsDark();
	//Small overwrites to dark style.
	ImGuiStyle &st = ImGui::GetStyle();
	st.WindowBorderSize = 2.0f;
	st.WindowPadding = { 4.0f,4.0f };
	st.ScrollbarSize = 18.0;
	st.Colors[ImGuiCol_Separator] = { 0.16f, 0.29f, 0.48f, 0.60f };
	st.Colors[ImGuiCol_Tab] = { 0.29f, 0.29f, 0.29f, 0.86f };
	st.Colors[ImGuiCol_DockingPreview] = ImVec4(0.36f, 0.49f, 0.68f, 0.80f);
	st.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.46f, 0.59f, 0.78f, 0.90f);
	st.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.247f, 0.353f, 0.507f, 0.90f);
	st.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	st.Colors[ImGuiCol_PopupBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

	st.WindowRounding = 3.0f;
	st.ChildRounding = 3.0f;
	st.FrameRounding = 3.0f;

	//st.TabBorderSize = 0.0f;
	//st.TabRounding = 8.0f;
	
	st.FramePadding = ImVec2(4.0f, 4.0f);

	st.Colors[ImGuiCol_Tab] = { 0.161f, 0.290f, 0.478f, 1.000f };
	st.Colors[ImGuiCol_TabUnfocused] = { 0.161f, 0.290f, 0.478f, 1.000f };
	st.Colors[ImGuiCol_TabUnfocusedActive] = { 0.200f, 0.410f, 0.680f, 1.000f };
	st.Colors[ImGuiCol_TitleBg] = { 0.160f, 0.290f, 0.480f, 1.000f };
}

void myLightStyle(ImGuiStyle* dst)
{
	ImGui::StyleColorsLight();
	//Small overwrites to light style.
	ImGuiStyle &st = ImGui::GetStyle();
	ImVec4* Colors = st.Colors;
	st.WindowBorderSize = 2.0f;
	st.WindowPadding = { 4.0f,4.0f };
	st.ScrollbarSize = 18.0;

	st.WindowRounding = 3.0f;
	st.ChildRounding = 3.0f;
	st.FrameRounding = 3.0f;

	Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.20f, 0.00f, 1.00f); //Also <h1> tags in help.
}
/*
void myStyle(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* Colors = style->Colors;

	Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	//Colors[ImGuiCol_TextHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	//Colors[ImGuiCol_TextActive] = ImVec4(1.00f, 1.00f, 0.00f, 1.00f);
	Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	//Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	//Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	//Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	//Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f); //Also <h1> tags in help.
	Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	//Colors[ImGuiCol_TooltipBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);

}

*/
void myDefaultStyle(ImGuiStyle* dst)
{
	auto *style = (dst ? dst : &ImGui::GetStyle());
	style->WindowRounding = 5.3f;
	style->GrabRounding = style->FrameRounding = 2.3f;
	style->ScrollbarRounding = 5.0f;
	style->FrameBorderSize = 1.0f;
	style->ItemSpacing.y = 6.5f;
	
	style->ScrollbarSize = 18.0;

	style->Colors[ImGuiCol_Text] = { 0.78f, 0.78f, 0.78f, 1.00f };
	style->Colors[ImGuiCol_TextDisabled] = { 0.55f, 0.55f, 0.55f, 1.00f };

	style->Colors[ImGuiCol_WindowBg] = { 0.23f, 0.23f, 0.23f, 0.98f }; //0.94
	style->Colors[ImGuiCol_ChildBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.00f };

	style->Colors[ImGuiCol_PopupBg] = { 0.31f, 0.32f, 0.34f, 1.0f }; //0.94

	style->Colors[ImGuiCol_Border] = { 0.33333334f, 0.33333334f, 0.33333334f, 0.50f };
	style->Colors[ImGuiCol_BorderShadow] = { 0.15686275f, 0.15686275f, 0.15686275f, 0.00f };
	
	style->Colors[ImGuiCol_FrameBg] = { 0.16862746f, 0.16862746f, 0.16862746f, 0.64f };
	style->Colors[ImGuiCol_FrameBgHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.77f };
	style->Colors[ImGuiCol_FrameBgActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.77f };

	style->Colors[ImGuiCol_TitleBg] = { 0.04f, 0.04f, 0.04f, 1.00f };
	style->Colors[ImGuiCol_TitleBgCollapsed] = { 0.16f, 0.29f, 0.48f, 1.00f };
	style->Colors[ImGuiCol_TitleBgActive] = { 0.00f, 0.00f, 0.00f, 1.0f };

	style->Colors[ImGuiCol_MenuBarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.92f };

	style->Colors[ImGuiCol_ScrollbarBg] = { 0.195f, 0.195f, 0.195f, 0.60f };
	style->Colors[ImGuiCol_ScrollbarGrab] = { 0.39f, 0.39f, 0.39f, 0.51f };

	style->Colors[ImGuiCol_ScrollbarGrabHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
	style->Colors[ImGuiCol_ScrollbarGrabActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 0.91f };
	// style->Colors[ImGuiCol_ComboBg]               = {0.1f, 0.1f, 0.1f, 0.99f};
	style->Colors[ImGuiCol_CheckMark] = { 0.90f, 0.90f, 0.90f, 0.83f };
	style->Colors[ImGuiCol_SliderGrab] = { 0.70f, 0.70f, 0.70f, 0.62f };
	style->Colors[ImGuiCol_SliderGrabActive] = { 0.30f, 0.30f, 0.30f, 0.84f };
	style->Colors[ImGuiCol_Button] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.49f };
	style->Colors[ImGuiCol_ButtonHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
	style->Colors[ImGuiCol_ButtonActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 1.00f };
	style->Colors[ImGuiCol_Header] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.53f };
	style->Colors[ImGuiCol_HeaderHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
	style->Colors[ImGuiCol_HeaderActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
	style->Colors[ImGuiCol_Separator] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_SeparatorHovered] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_SeparatorActive] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_ResizeGrip] = { 1.00f, 1.00f, 1.00f, 0.85f };
	style->Colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.00f, 1.00f, 0.60f };
	style->Colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.00f, 0.90f };
	style->Colors[ImGuiCol_PlotLines] = { 0.61f, 0.61f, 0.61f, 1.00f };
	style->Colors[ImGuiCol_PlotLinesHovered] = { 1.00f, 0.43f, 0.35f, 1.00f };
	style->Colors[ImGuiCol_PlotHistogram] = { 0.90f, 0.70f, 0.00f, 1.00f }; //Also <h1> tags in help.
	style->Colors[ImGuiCol_PlotHistogramHovered] = { 1.00f, 0.60f, 0.00f, 1.00f };
	style->Colors[ImGuiCol_TextSelectedBg] = { 0.18431373f, 0.39607847f, 0.79215693f, 0.90f };

	//Still need to be set.
	style->Colors[ImGuiCol_Tab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style->Colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style->Colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
	style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
	style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.6f);
	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);

}


void myDarkGreyStyle(ImGuiStyle* dst)
{
	ImGuiStyle &st = ImGui::GetStyle();
	st.FrameBorderSize = 1.0f;
	st.FramePadding = ImVec2(4.0f, 2.0f);
	st.ItemSpacing = ImVec2(8.0f, 2.0f);
	st.WindowBorderSize = 2.0f;
	//	st.TabBorderSize = 1.0f;
	st.WindowRounding = 1.0f;
	st.ChildRounding = 1.0f;
	st.FrameRounding = 1.0f;
	st.ScrollbarRounding = 1.0f;
	st.ScrollbarSize = 18.0;
	st.GrabRounding = 1.0f;
	//	st.TabRounding = 1.0f;

//	st.TabBorderSize = 5.0f;
//	st.TabRounding = 2.0f;


	// Setup style
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.941f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.031f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.53f, 0.53f, 0.53f, 0.25f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
	
	colors[ImGuiCol_TitleBg] = ImVec4(0.114f, 0.191f, 0.199f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.203f, 0.335f, 0.348f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
	
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
	colors[ImGuiCol_Button] = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
	colors[ImGuiCol_Header] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
	colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.5f, 0.5f, 0.5f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_Tab] = ImVec4(0.114f, 0.161f, 0.200f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.134f, 0.181f, 0.220f, 0.86f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.114f, 0.161f, 0.200f, 0.86f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.124f, 0.171f, 0.210f, 1.0f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void CustomStyleColors(ImGuiStyle* dst)
{
ImGuiStyle& style = ImGui::GetStyle();

//recall preference values
static float col_main_hue = pref.gui_col_main_hue;
static float col_main_sat = pref.gui_col_main_sat;
static float col_main_val = pref.gui_col_main_val;
static float col_area_hue = pref.gui_col_area_hue;
static float col_area_sat = pref.gui_col_area_sat;
static float col_area_val = pref.gui_col_area_val;
static float col_back_hue = pref.gui_col_back_hue;
static float col_back_sat = pref.gui_col_back_sat;
static float col_back_val = pref.gui_col_back_val;
static float col_text_hue = pref.gui_col_text_hue;
static float col_text_sat = pref.gui_col_text_sat;
static float col_text_val = pref.gui_col_text_val;
static float col_scroll_hue = pref.gui_col_scroll_hue;
static float col_scroll_sat = pref.gui_col_scroll_sat;
static float col_scroll_val = pref.gui_col_scroll_val;

//set colours
ImVec4 col_main = ImColor::HSV(col_main_hue, col_main_sat, col_main_val);
ImVec4 col_area = ImColor::HSV(col_area_hue, col_area_sat, col_area_val);
ImVec4 col_back = ImColor::HSV(col_back_hue, col_back_sat, col_back_val);
ImVec4 col_text = ImColor::HSV(col_text_hue, col_text_sat, col_text_val);
ImVec4 col_scroll = ImColor::HSV(col_scroll_hue, col_scroll_sat, col_scroll_val);

//main 
style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.65f);
style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
style.Colors[ImGuiCol_Tab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
style.Colors[ImGuiCol_TabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
style.Colors[ImGuiCol_TabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
style.Colors[ImGuiCol_TabUnfocused] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);

//area
style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
//style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);

//background
style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);

//text
style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
style.Colors[ImGuiCol_CheckMark] = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
//style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);

//scroll & sliders
style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 0.50f); //0.31
style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 0.78f);
style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 1.00f);
style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 0.50f); //0.24
style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_scroll.x, col_scroll.y, col_scroll.z, 1.00f);

//fixed colours
style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.6f);

}


/*
void SetupStyleFromHue()
{
#if 1
	// FIXME: those should become parameters to the function
	static int hue = 140;
	static float col_main_sat = 180.f / 255.f;
	static float col_main_val = 161.f / 255.f;
	static float col_area_sat = 124.f / 255.f;
	static float col_area_val = 100.f / 255.f;
	static float col_back_sat = 59.f / 255.f;
	static float col_back_val = 40.f / 255.f;

	ImGui::Begin("Hue Style");
	ImGui::SliderInt("master hue", &hue, 0, 255);

	float dummy;
	ImVec4 rgb;
//	ImGui::ColorEditMode(ImGuiColorEditMode_HSV);

	ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_main_sat, col_main_val, rgb.x, rgb.y, rgb.z);
	ImGui::ColorEdit3("main", &rgb.x);
	ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_main_sat, col_main_val);

	ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_area_sat, col_area_val, rgb.x, rgb.y, rgb.z);
	ImGui::ColorEdit3("area", &rgb.x);
	ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_area_sat, col_area_val);

	ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_back_sat, col_back_val, rgb.x, rgb.y, rgb.z);
	ImGui::ColorEdit3("back", &rgb.x);
	ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_back_sat, col_back_val);

	ImGui::End();
#endif

	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
	ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
	ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
	ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

	style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	//style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
	//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
	//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	//style.Colors[ImGuiCol_TooltipBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);


	style.Colors[ImGuiCol_Tab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.6f);


}
*/

//###################
//#### Debug log ####
//###################

static int last_selected_logentry = -1;

struct CompilerLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset
	ImVector<int>       LineColor; // Index to lines offset
	bool                ScrollToBottom;
	bool                ScrollToTop;

	void    Clear() { last_selected_logentry = -1; Buf.clear(); LineOffsets.clear(); LineColor.clear(); }

	void    AddLog(int LCol,const char* fmt, ... ) // IM_FMTARGS(2)
	{
		int old_size = Buf.size();

		//Mike's search bug , double parsing , so content was parsed again.
		//AddToCompilerLog has already done this.
		//and contrent like "%444334d" could be executed and generate errors.
		//PE: removed double parsing.
//		va_list args;
//		va_start(args, fmt);
//		Buf.appendfv(fmt, args);
//		va_end(args);

		Buf.append(fmt);

		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n') {
				LineColor.push_back(LCol);
				LineOffsets.push_back(old_size);
			}

		ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* p_open = NULL)
	{
		ImGui::SetNextWindowSize(ImVec2(34 * ImGui::GetFontSize(), 27 * ImGui::GetFontSize()), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}
		if (ImGui::Button("Clear")) { Clear(); ClearAllEditorMarkers(); };
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
//		ImGui::SameLine();
//		Filter.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy) {
			if (Buf.size() > 0) {
				const char* buf_begin = Buf.begin();
				const char* line = buf_begin;
				if (buf_begin)
					ImGui::SetClipboardText(buf_begin);
				//ImGui::LogToClipboard();
			}
		}

//		if (Filter.IsActive())
//		{
//			const char* buf_begin = Buf.begin();
//			const char* line = buf_begin;
//			for (int line_no = 0; line != NULL; line_no++)
//			{
//				const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
//				if (Filter.PassFilter(line, line_end)) {
//					ImGui::TextUnformatted(line, line_end);
//					//ImGui::Selectable(line);
//					//Selectable
//				}
//				line = line_end && line_end[1] ? line_end + 1 : NULL;
//			}
//		}
//		else
		{
			const char* buf_begin = Buf.begin();
			const char* line = buf_begin;
			char uid[80];
			for (int line_no = 0; line != NULL; line_no++)
			{
				const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
				if (line_end != NULL) {

					//ImGui::TextUnformatted(line, line_end);
					char tmpline[1024];
					int tmplinesize = line_end - line;
					if (tmplinesize > 0 && tmplinesize < 1024) {
						strncpy(&tmpline[0], line, line_end - line);
						tmpline[line_end - line] = 0;

						ImVec4* style_colors = ImGui::GetStyle().Colors;
						ImVec4 oldcolor = style_colors[ImGuiCol_Text];

						if (LineColor[line_no] == 1) {
							style_colors[ImGuiCol_Text] = ImVec4(0.95f, 0.3f, 0.3f, 0.90f);
						}
						else if (LineColor[line_no] == 2) {
							//style_colors[ImGuiCol_Text] = ImVec4(0.6f, 0.6f, 1.0f, 1.0f);
						}

						sprintf(uid, "loguid%ld", line_no);
						ImGui::PushID(uid);

						bool isSelected = false;
						if (last_selected_logentry == line_no)
							isSelected = true;
						if (ImGui::Selectable(tmpline, isSelected)) {
							last_selected_logentry = line_no;
							ParseCompilerLog(tmpline,false);
						}
						ImGui::PopID();
						style_colors[ImGuiCol_Text] = oldcolor;

						if (ImGui::BeginPopupContextItemAGK()) //"project context menu"
						{
							if (ImGui::MenuItem("Copy to Clipboard", nullptr, nullptr))
								ImGui::SetClipboardText(tmpline);

							ImGui::EndPopup();
						}

					}
				}
				line = line_end && line_end[1] ? line_end + 1 : NULL;
				//delete myline;
			}
			//	ImGui::TextUnformatted(Buf.begin());
		}

		if (ScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		if (ScrollToTop)
			ImGui::SetScrollY(0.0);
		ScrollToTop = false;
		ScrollToBottom = false;
		ImGui::EndChild();
		ImGui::End();
	}
};


bool bAutoContinue = false;
bool bAutoState;
float fLastBreakTime = 0;
float fSendPauseAgain = 0;
uString debugCallStack;
//std::map<std::string, WatchValues> debugWatch;
//std::unordered_map<std::string, WatchValues> debugWatch;
char addWatchVariable[MAX_PATH];

char cHoverGetVariable[MAX_PATH];
char cHoverVariable[MAX_PATH];
#define MAXVALUESIZE 2048
char cHoverValue[MAXVALUESIZE + 1];
float fHoverTime;
static uString callstack_items[11];
static int combo_callstack_entrys = 0;
static char * current_callstack_search = NULL;

void processDebugger(void)
{

	static char cEditingVariable[MAX_PATH] = "";
	static char cEditingStringValue[MAX_PATH] = "";
	static float fEditingFloat = 0.0;
	static int iEditingInt = 0;
	static int iEditingType = 0;
	static bool bEditingFirstRun = false;
	bool bIsEditing = false;

	if (ImGui::Button("break")) {
		bBreakJustStarted = true;
		bAutoContinue = false;
		uString bc_err;
		BroadcasterCommand("pause", bc_err);
		if (bc_err.GetLength() > 0)
			AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
	}
	ImGui::SameLine();

	if (ImGui::Button("continue")) {
		uString bc_err;
		BroadcasterCommand("continue",bc_err);
		if (bc_err.GetLength() > 0)
			AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
		ClearExecuteLine();
	}
	ImGui::SameLine();

	if (ImGui::Button("step")) {
		bBreakJustStarted = false;
		uString bc_err;
		BroadcasterCommand("step",bc_err);
		if (bc_err.GetLength() > 0)
			AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
	}
	ImGui::SameLine();

	if (ImGui::Button("step over")) {
		if (bBreakJustStarted) {
			uString bc_err;
			BroadcasterCommand("step",bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
		}
		else {
			uString bc_err;
			BroadcasterCommand("stepover", bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
		}
		bBreakJustStarted = false;
	}
	ImGui::SameLine();

	if (ImGui::Button("step out")) {
		if (bBreakJustStarted) {
			uString bc_err;
			BroadcasterCommand("step",bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
		}
		else {
			uString bc_err;
			BroadcasterCommand("stepout",bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
		}
		bBreakJustStarted = false;
	}

	ImGui::Text("Call Stack:\n");

	if (combo_callstack_entrys >= 0) {

		if (combo_callstack_entrys == 0 || current_callstack_search == NULL) {
			if (!callstack_items[0].GetStr())
				callstack_items[0] = "";
			current_callstack_search = (char*)callstack_items[0].GetStr();
		}
		ImGui::PushItemWidth(-1);
		int comboflags = 0; //ImGuiComboFlags_NoPreview;
		if (ImGui::BeginCombo("##combolastsearch", current_callstack_search, comboflags))
		{
			for (int n = 0; n < combo_callstack_entrys; n++)
			{
				if (!callstack_items[n].GetStr())
					callstack_items[n] = "";
				bool is_selected = (current_callstack_search == callstack_items[n].GetStr());
				if (ImGui::Selectable(callstack_items[n].GetStr(), is_selected)) {
					current_callstack_search = (char *)callstack_items[n].GetStr();

					uString szParse = callstack_items[n];
					int pos = szParse.RevFindStr(" at line ");
					if (pos > 0) {
						uString szLineNo;
						szParse.SubString(szLineNo, pos + 9);
						int lineno = atoi(szLineNo);
						if (lineno > 0) {

							//if (szParse.RevFindStr("<main>", 1) < 0)
							//	lineno--;

							int pos2 = szParse.RevFindStr(" in ");
							if (pos2 >= 0) {
								uString szFile;
								szParse.SubString(szFile, pos2 + 4);
								int pos3 = szFile.RevFindStr(" at line ");
								if (pos3 > 0) {
									uString szTmp;
									szFile.SubString(szTmp, 0, pos3);
									szFile = szTmp;
								}
								TextEditor * m_foundeditor;
								m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)szFile.GetStr());

								if (m_foundeditor) {
									szNextWindowFocus = szFile;
									if (pref.bRemovePathFromTabName) {
										uString newTabname = szNextWindowFocus;
										int ntb_pos = newTabname.RevFindStr("/");
										if (ntb_pos <= 0)
											ntb_pos = newTabname.RevFindStr("\\");
										if (ntb_pos > 0)
											newTabname.SubString(szNextWindowFocus, ntb_pos + 1);
									}
									szNextWindowFocus.Append("##");
									szNextWindowFocus.Append(pCurrentSelectedProject->m_sProjectFile);
									cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
									ImGui::SetWindowFocus(cNextWindowFocus);

									//Check if file is open in editor.
									cProjectItem::sProjectFiles * m_pCurrentFile;
									m_pCurrentFile = FindFileFromEditor(m_foundeditor);
									if (m_pCurrentFile) {
										if (!m_pCurrentFile->m_bOpenForEdit || (m_pCurrentFile->m_editor && !m_pCurrentFile->m_editor->bEditorVisible )  ) {
											//Not loaded open so we can see error.
											vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
											m_pCurrentFile->m_bOpenForEdit = true;
											cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
										}
									}
									m_foundeditor->SetCursorPosition(TextEditor::Coordinates(lineno - 1, 0));
									m_foundeditor->EnsureCursorVisible();
									m_foundeditor->mScrollToCursor = true;
								}
							}
						}
					}
					//find lineno. at line
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}

	ImGui::Separator();
//	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), (7.4*ImGui::GetFontSize()) ));
	ImGui::Text("Variables:\n");
	//ImGui::Separator();
	int i = 600;
	int iCol = 15 * ImGui::GetFontSize();
	int iMaxVariableSize = 28;
	bool bAddthewatch = false;
	ImVec2 ws = ImGui::GetWindowSize();
	iCol = ws.x * 0.45;
	static float largest_cursor_x = 0;
	if (iCol < 100)
		iCol = 100;
	float fs = (ImGui::GetFontSize()*0.60);
	if (fs <= 0)
		fs = 1.0;
	iMaxVariableSize = iCol / fs;
	if (largest_cursor_x < iCol + 10.0) iCol = largest_cursor_x;


	largest_cursor_x = 0;

	for (std::map<std::string, WatchValues>::iterator it = pCurrentSelectedProject->debugWatch.begin(); it != pCurrentSelectedProject->debugWatch.end(); ++it)
	{
		if (!it->second.bHoverVariable && strcmp(it->first.c_str(),"@Version") != 0 ) {
			
			//Is this a variable that can be unfolled.
			if (it->second.value[0] == '{' && it->second.value[1] == ' ' && it->second.value[2] == '[') {
				
				uString szParse;
				szParse = it->second.value;

				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap;
//				ImGuiTreeNodeFlags_Selected;

				bool multidimarray = false;
				if (it->first[strlen(it->first.c_str()) - 1] == ']')
					multidimarray = true;

				ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() );

				bool TreeNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i++, node_flags, "%s", it->first.c_str());
				ImGui::SameLine();

				if (ImGui::GetCursorPosX() > largest_cursor_x) largest_cursor_x = ImGui::GetCursorPosX();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, -3.0));

				ImGuiWindow *pwin;
				pwin = ImGui::GetCurrentWindow();
				int iwidth = pwin->Size.x - pwin->ScrollbarSizes.x; //SizeContents.x;
				ImGui::Spacing();
				ImGui::SameLine(iwidth - (ImGui::GetFontSize()+1.0));

				ImGui::PushID(i+5000);
				if (ImGui::Button("X")) {
					deleteWatch(it->first.c_str());
					ImGui::PopID();
					if (TreeNodeOpen)
						ImGui::TreePop();
					ImGui::PopStyleVar();
					break;
				}
				ImGui::PopID();

				if (TreeNodeOpen)
				{
					int startpos = 0;

					int nextpos = szParse.FindStr(",");
					if (nextpos == -1) {
						nextpos = szParse.FindStr("\" }"); //" }
						if (nextpos > 0)
							nextpos += 2;
					}
					if (nextpos == -1) {
						nextpos = szParse.FindStr("\"}"); //" }
						if (nextpos > 0)
							nextpos++;
					}
					if (nextpos == -1)
						nextpos = szParse.FindStr("}");
					int nextadder = 1;
					if (nextpos > 0) {
						if (szParse[nextpos - 1] == '"') {
							nextpos--;
							nextadder = 2;
						}
					}

					while (nextpos > 0) {
						uString szEntry, szRest;
						szParse.SubString(szEntry, 0, nextpos+(nextadder-1));
						szParse.SubString(szRest, nextpos + nextadder);

						//Parse entry.
						int pos = szEntry.FindStr("[");
						int pos2 = szEntry.FindStr("]");
						if (pos >= 0 && pos2 > pos) {
							uString szArrayNumber, szArrayValue,szFinal;
							szEntry.SubString(szArrayNumber, pos + 1, pos2 - pos - 1);
							szEntry.SubString(szArrayValue, pos2 + 2);

							char tmp[MAX_PATH + 7];
							char hover[MAX_PATH + 7];
							if (szArrayNumber[0] == '0' || atoi(szArrayNumber.GetStr()) > 0) {
								if (!multidimarray) {
									sprintf(tmp, "%s[%s]", it->first.c_str(), szArrayNumber.GetStr());
								}
								else {
									sprintf(tmp, "%s%s]", it->first.c_str(), szArrayNumber.GetStr());
									tmp[strlen(it->first.c_str()) - 1] = ',';
								}
							} else {
								//this is a type returned.
								sprintf(tmp, "%s.%s", it->first.c_str(), szArrayNumber.GetStr());
							}
							strcpy(hover, tmp);
							if (strlen(tmp) > iMaxVariableSize) {
								tmp[iMaxVariableSize-1] = 0;
								strcat(tmp, "...");
							}
							
							bIsEditing = false;
							if (strcmp(cEditingVariable, hover) == 0) {
								bIsEditing = true;
								if (strlen(tmp) > iMaxVariableSize-3) {
									tmp[iMaxVariableSize-4] = 0;
									strcat(tmp, "...");
								}
							}


							int leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnDoubleClick;
							//	leafFlags |= ImGuiTreeNodeFlags_Bullet;

							if (ImGui::TreeNodeEx((void*)(intptr_t)i++, leafFlags, "%s", tmp )) {
								//if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
								//}
								ImGui::TreePop();
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip(hover);
						
							ImGui::SameLine();
							if (ImGui::GetCursorPosX() > largest_cursor_x) largest_cursor_x = ImGui::GetCursorPosX();
							ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, -3.0));

							if (bIsEditing)
								ImGui::SetCursorPos(ImVec2(iCol - (2* ImGui::GetFontSize()), ImGui::GetCursorPos().y));
							else
								ImGui::SetCursorPos(ImVec2(iCol, ImGui::GetCursorPos().y));

							//<Add a separate watch for this variable>
							if( szArrayValue.CompareToN("<Add a separate watch for this variable>",40) == 0) {
								//Try to locate watch.
								auto it2 = pCurrentSelectedProject->debugWatch.find(hover);
								if (it2 == pCurrentSelectedProject->debugWatch.end()) {
									if (strlen(hover) > 0 && hover[strlen(hover) - 1] != ']') {
										char cTmp[MAX_PATH + 7];
										strcpy(cTmp, hover);
										strcat(cTmp, "[0]");
										it2 = pCurrentSelectedProject->debugWatch.find(cTmp);
									}
								}

								if (it2 != pCurrentSelectedProject->debugWatch.end())
								{
									int ti_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly;
									sprintf(tmp, "##at%d", i++);
									ImGui::PushItemWidth(-(1.33*ImGui::GetFontSize()));
									ImGui::InputText(tmp, (char *)it2->second.value, MAXVALUESIZE, ti_flags);
									if (ImGui::IsItemHovered()) {
										if ( strlen(it2->second.value) > 20) {
											ImGui::SetNextWindowContentSize(ImVec2(400, 0));
											ImGui::BeginTooltip();
											ImGui::TextWrapped("Size [%d]\n%s", strlen(it2->second.value), it2->second.value);
											ImGui::EndTooltip();
										}
									}

								}
								else {
									ImGui::PushID(i + 8000); //need unique id.
									if (ImGui::Button(" Add Watch ")) {
										strcpy(addWatchVariable, hover);
										bAddthewatch = true;
									}
									ImGui::PopID();
								}
							}
							else {


								int ti_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly;
								sprintf(tmp, "##a3t%d", i++);
								if (bIsEditing)
									ImGui::PushItemWidth(-(6* ImGui::GetFontSize()));
								else
									ImGui::PushItemWidth(-(2* ImGui::GetFontSize()));
								bool bQuickChange = false;
								if (bIsEditing) {
									if (bEditingFirstRun)
										ImGui::SetKeyboardFocusHere();
									bEditingFirstRun = false;

									if (iEditingType == 1) {
										ImGui::InputText(tmp, &cEditingStringValue[0], MAX_PATH - 60, 0); //ImGuiInputTextFlags_EnterReturnsTrue
									}
									else if (iEditingType == 2) {
										if (ImGui::InputFloat(tmp, &fEditingFloat, 0.01, 0.10, 6, 0)) { //ImGuiInputTextFlags_EnterReturnsTrue
											bQuickChange = true;
										}
									}
									else {
										if (ImGui::InputInt(tmp, &iEditingInt, 1, 10, 0)) { //ImGuiInputTextFlags_EnterReturnsTrue
											bQuickChange = true;
										}
									}
								}
								else {
									ImGui::InputText(tmp, (char *)szArrayValue.GetStr(), MAX_PATH, ti_flags);
								}

								if (ImGui::IsItemHovered()) {
									if (szArrayValue.GetLength() > 20) {
										ImGui::SetNextWindowContentSize(ImVec2(27*ImGui::GetFontSize(), 0));
										ImGui::BeginTooltip();
										ImGui::TextWrapped("Size [%d]\n%s", szArrayValue.GetLength(), szArrayValue.GetStr());
										ImGui::EndTooltip();
									}
								}

								if (bIsEditing) {
									ImGui::SameLine();
									ImGui::PushID(i + 12000);
									if (ImGui::Button("Set") || bQuickChange) {
										char szTmp[MAX_PATH];
										if ((strlen(cEditingVariable) + strlen(cEditingStringValue) + 15) < MAX_PATH) {
											if (iEditingType == 1)
												sprintf(szTmp, "setvariable %s %s", cEditingVariable, cEditingStringValue);
											else if (iEditingType == 2)
												sprintf(szTmp, "setvariable %s %f", cEditingVariable, fEditingFloat);
											else
												sprintf(szTmp, "setvariable %s %d", cEditingVariable, iEditingInt);

											uString bc_err;
											BroadcasterCommand(szTmp,bc_err);
										}
										if (!bQuickChange) {
											strcpy(cEditingVariable, "");
											strcpy(cEditingStringValue, "");
										}
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Variable");

									ImGui::PopID();

									ImGui::SameLine();
									ImGui::PushID(i + 2000);

									if (ImGui::Button("Cancel")) {
										strcpy(cEditingVariable, "");
										strcpy(cEditingStringValue, "");
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cancel Set Variable");
									
									ImGui::PopID();

								}
								else {
									ImGui::SameLine();
									ImGui::PushID(i + 2000);
									if (ImGui::Button(ICON_MD_CHANGE_HISTORY)) {
										if (bSetVariableActive) {
											uString szClean = szArrayValue.GetStr();
											strcpy(cEditingVariable, hover); //Hover got the full resolved variable. //it->first.c_str()
											if (szClean.FindStr("\"") >= 0) {
												iEditingType = 1; // string
												strcpy(cEditingStringValue, szArrayValue.GetStr());
												if (cEditingStringValue[0] == '"')
													strcpy(&cEditingStringValue[0], &cEditingStringValue[1]);
												if (cEditingStringValue[strlen(cEditingStringValue) - 1] == '"')
													cEditingStringValue[strlen(cEditingStringValue) - 1] = 0;
											}
											else if (szClean.FindStr(".") >= 0) {
												iEditingType = 2; // float
												fEditingFloat = atof(szArrayValue.GetStr());
											}
											else {
												iEditingType = 3; // int
												iEditingInt = atoi(szArrayValue.GetStr());
											}
											bEditingFirstRun = true;
										}
									}
									ImGui::PopID();
									if (bSetVariableActive) {
										if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Variable");
									}
									else {
										if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Variable\nOnly Active if \"Player\" Support This.");
									}
								}


							}
//							agk::Print(szEntry);
						}

						szParse = szRest;
						nextpos = szParse.FindStr(",");
						if( nextpos == -1)
							nextpos = szParse.FindStr("}");
						nextadder = 1;
						if (nextpos > 0) {
							if (szParse[nextpos - 1] == '"') {
								nextpos--;
								nextadder = 2;
							}
						}

					}

					ImGui::TreePop();
				}

				ImGui::PopStyleVar();

			}
			else {
				bIsEditing = false;
				if (strcmp(cEditingVariable, it->first.c_str()) == 0)
					bIsEditing = true;

				char tmp[MAX_PATH + 7];
				int ti_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly;
				sprintf(tmp, "%s:", it->first.c_str());
				if (strlen(tmp) > iMaxVariableSize+2) {
					tmp[iMaxVariableSize+1] = 0;
					strcat(tmp, "...");
				}
				ImGui::Text(tmp);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(it->first.c_str());

				ImGui::SameLine();
				if (ImGui::GetCursorPosX() > largest_cursor_x) largest_cursor_x = ImGui::GetCursorPosX();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, -3.0));

				if (bIsEditing)
					ImGui::SetCursorPos(ImVec2(iCol-30, ImGui::GetCursorPos().y));
				else
					ImGui::SetCursorPos(ImVec2(iCol, ImGui::GetCursorPos().y));
				sprintf(tmp, "##wu%d", i++);
				if (bIsEditing)
					ImGui::PushItemWidth(-(6* ImGui::GetFontSize()));
				else
					ImGui::PushItemWidth(-(3.4*ImGui::GetFontSize()));

				bool bQuickChange = false;
				if (bIsEditing) {
					if (bEditingFirstRun)
						ImGui::SetKeyboardFocusHere();
					bEditingFirstRun = false;

					if (iEditingType == 1) {
						ImGui::InputText(tmp, &cEditingStringValue[0], MAX_PATH-60, 0); //ImGuiInputTextFlags_EnterReturnsTrue
					}
					else if (iEditingType == 2) {
						if (ImGui::InputFloat(tmp, &fEditingFloat, 0.01, 0.1, 6, 0)) { //ImGuiInputTextFlags_EnterReturnsTrue
							bQuickChange = true;
						}
					}
					else {
						if (ImGui::InputInt(tmp, &iEditingInt, 1, 10, 0)) { //ImGuiInputTextFlags_EnterReturnsTrue
							bQuickChange = true;
						}
					}
				}
				else {
					ImGui::InputText(tmp, &it->second.value[0], MAXVALUESIZE, ti_flags);
				}
				if (ImGui::IsItemHovered()) {
					if (strlen(it->second.value) > 20) {

						ImGui::SetNextWindowContentSize(ImVec2(27* ImGui::GetFontSize(), 0));
						ImGui::BeginTooltip();
						ImGui::TextWrapped("Size [%d]\n%s", strlen(it->second.value), it->second.value);
						ImGui::EndTooltip();
					}

				}

				ImGui::SameLine();
				ImGui::PushID(i + 12000);
				if (bIsEditing) {
					if (ImGui::Button("Set") || bQuickChange ) {
						char szTmp[MAX_PATH];
						if ((strlen(cEditingVariable) + strlen(cEditingStringValue) + 15) < MAX_PATH) {
							if (iEditingType == 1)
								sprintf(szTmp, "setvariable %s %s", cEditingVariable, cEditingStringValue);
							else if (iEditingType == 2)
								sprintf(szTmp, "setvariable %s %f", cEditingVariable, fEditingFloat);
							else
								sprintf(szTmp, "setvariable %s %d", cEditingVariable, iEditingInt);

							uString bc_err;
							BroadcasterCommand(szTmp,bc_err);
						}
						if (!bQuickChange) {
							strcpy(cEditingVariable, "");
							strcpy(cEditingStringValue, "");
						}
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Variable");
				}
				else {

					if (ImGui::Button(ICON_MD_CHANGE_HISTORY)) {
						if (bSetVariableActive) {
							uString szClean = it->second.value;
							strcpy(cEditingVariable, it->first.c_str());
							if (szClean.FindStr("\"") >= 0) {
								iEditingType = 1; // string
								strcpy(cEditingStringValue, it->second.value);
								if (cEditingStringValue[0] == '"')
									strcpy(&cEditingStringValue[0], &cEditingStringValue[1]);
								if (cEditingStringValue[strlen(cEditingStringValue) - 1] == '"')
									cEditingStringValue[strlen(cEditingStringValue) - 1] = 0;
							}
							else if (szClean.FindStr(".") >= 0) {
								iEditingType = 2; // float
								fEditingFloat = atof(it->second.value);
							}
							else {
								iEditingType = 3; // int
								iEditingInt = atoi(it->second.value);
							}
							bEditingFirstRun = true;
						}
					}
					if (bSetVariableActive) {
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Variable");
					}
					else {
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Variable\nOnly Active if \"Player\" Support This.");
					}

				}
				ImGui::PopID();
				ImGui::SameLine();
				ImGui::PushID(i+2000);
				
				if (bIsEditing) {
					if (ImGui::Button("Cancel")) {
						strcpy(cEditingVariable, "");
						strcpy(cEditingStringValue, "");
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cancel Set Variable");
				}
				else {
					if (ImGui::Button("X")) {
						deleteWatch(it->first.c_str());
						ImGui::PopID();
						break;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Delete Watch");
				}
				
				ImGui::PopID();

			}
		}
	}
	if (i > 600 && pCurrentSelectedProject) {
		if (ImGui::Button("Delete all watches")) {
			uString bc_err;
			BroadcasterCommand("delete all watches",bc_err);
			pCurrentSelectedProject->debugWatch.clear();
		}
	}
	ImGui::Text("\n");
	ImGui::Separator();
	
	ImGui::PushItemWidth(-(6.2*ImGui::GetFontSize()));
	if (ImGui::InputText("  ##Add watch: ", &addWatchVariable[0], MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		bAddthewatch = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Add watch") || bAddthewatch ) {
		if (strlen(addWatchVariable) > 0) {
			addWatch(addWatchVariable);
			strcpy(addWatchVariable, "");
		}
	}

	ImGui::Separator();

	if (ImGui::Checkbox("Disable all breakpoints", &dis_all_breakpoints)) {
		//
		if (dis_all_breakpoints)
		{
			uString bc_err;
			BroadcasterCommand("delete all breakpoints",bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "delete breakpoints: %s\n", bc_err.GetStr());
		}
		else
		{
			if (mCompiledProject) {
				addAllBreakPoints((char*)mCompiledProject->m_sProjectFile.GetStr());
			}
		}
	}

	ImGui::Checkbox("Auto Update", &bAutoContinue);

	//ImGui::Text("Interval:");
	//ImGui::SameLine();
	static int autoDebugRun = 0;

	ImGui::RadioButton("60 fps ", &autoDebugRun, 0); ImGui::SameLine();
	ImGui::RadioButton("30 fps ", &autoDebugRun, 1); ImGui::SameLine();
	ImGui::RadioButton("10 fps", &autoDebugRun, 2); ImGui::SameLine();
	ImGui::RadioButton("1 fps", &autoDebugRun, 3);

	if (bAutoState != bAutoContinue) {
		//Change.
		if (!bAutoContinue) {
			//pause.
			bBreakJustStarted = true;
			uString bc_err;
			BroadcasterCommand("pause",bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());

			fSendPauseAgain = agk::Timer();
		}
		bAutoState = bAutoContinue;
	}

	if (fSendPauseAgain > 0) {
		if (agk::Timer() - fSendPauseAgain >= 0.5) { // 1.5 sec to give time to flush everything.
			bBreakJustStarted = true;
			uString bc_err;
			BroadcasterCommand("pause",bc_err);
			fSendPauseAgain = 0;
		}
	}

	if (pref.bDebugBringToFront && !updateWatchCalled ) {
		if (agk::Timer() - fLastBreakTime >= 2.0) {
			fLastBreakTime = agk::Timer();
			//pause.
			bBreakJustStarted = true;
			uString bc_err;
			BroadcasterCommand("pause",bc_err);
			BroadcasterCommand("continue",bc_err);
		}
	}
	
	if (bAutoContinue) {
		float updateTime;
		if (autoDebugRun == 0) {
			updateTime = 0.015;
		}
		else if (autoDebugRun == 1) {
			updateTime = 0.032;
		}
		else if (autoDebugRun == 2) {
			updateTime = 0.1;
		}
		else  {
			updateTime = 1.0;
		}
		if (agk::Timer()-fLastBreakTime >= updateTime) {
			fLastBreakTime = agk::Timer();
			//pause.
			uString bc_err;
			bBreakJustStarted = true;
			BroadcasterCommand("pause",bc_err);
			BroadcasterCommand("continue",bc_err);
		}
	}
	ImGui::TextWrapped("\nAuto Update: Please remove all breakpoint for max speed. You will not be able to use music while debugging. Depending on your network you might have to select 1 fps when using remote debugging.\n");

	//Check hover variables.
	if (strlen(cHoverGetVariable) > 0 && pCurrentSelectedProject ) {
		if (strcmp(cHoverVariable, cHoverGetVariable) != 0) {
			//Update variable.

			if (strlen(cHoverVariable) > 0) {
				//Delete old.
				auto it = pCurrentSelectedProject->debugWatch.find(cHoverVariable);
				if (it != pCurrentSelectedProject->debugWatch.end())
				{
					if (it->second.bHoverVariable) {
						//Delete old hover.
						deleteWatch(it->first.c_str());
					}
				}
			}

			strcpy(cHoverVariable, cHoverGetVariable);

			//if not already there.
			auto it = pCurrentSelectedProject->debugWatch.find(cHoverGetVariable);
			if (it != pCurrentSelectedProject->debugWatch.end())
			{
				//This convert it to utf8 and give some strange results.
				//uString szParsed = it->second.value;
				//szParsed.ReplaceStr("<Not Found>", "undefined");
				//strcpy(cHoverValue, szParsed.GetStr());

				//Just update variable.
				strcpy(cHoverValue, it->second.value);
			}
			else {
				addWatch(cHoverVariable, true);
			}
		}
		else {
			//Update hover variable.
			auto it = pCurrentSelectedProject->debugWatch.find(cHoverGetVariable);
			if (it != pCurrentSelectedProject->debugWatch.end())
			{
				//uString szParsed = it->second.value;
				//szParsed.ReplaceStr("<Not Found>", "undefined");
				//strcpy(cHoverValue, szParsed.GetStr());
				strcpy(cHoverValue, it->second.value);
			}
		}

	}
}

void addAllWatches(void)
{
	char tmp[MAX_PATH + 7];
	uString bc_err;
	BroadcasterCommand("delete all watches",bc_err);
	if (bc_err.GetLength() > 0)
		AddToCompilerLog(2, "addAllWatches: %s\n", bc_err.GetStr());

	sprintf(tmp, "watch @Version");
	BroadcasterCommand(tmp,bc_err);

	addWatch("@Version");
	if (pCurrentSelectedProject) {
		for (auto it = pCurrentSelectedProject->debugWatch.begin(); it != pCurrentSelectedProject->debugWatch.end(); ++it)
		{
			sprintf(tmp, "watch %s", it->first.c_str());
			uString bc_err;
			BroadcasterCommand(tmp,bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "addAllWatches: %s\n", bc_err.GetStr());
		}
	}
}


void updateWatch(const char *watch, const char *value)
{
	updateWatchCalled = true;
	if (watch && value && pCurrentSelectedProject)	{
		auto it = pCurrentSelectedProject->debugWatch.find(watch);
		if (it != pCurrentSelectedProject->debugWatch.end())
		{
			if (strcmp(watch, "@Version") == 0) {
				//For debug only.
				sprintf(it->second.value, "%s", value);
				int iVal = atoi(value);
				if (iVal > 0) {
					bSetVariableActive = true;
					strcpy(it->second.value, "[Active]");
				}
				return;
			}

//			uString szParsed = value;
//			if (it->second.bHoverVariable) {
//				szParsed.ReplaceStr("<Not Found>", "undefined");
//			}
			if (strlen(value) < MAXVALUESIZE) {
//				strcpy(it->second.value, szParsed.GetStr());
				strcpy(it->second.value, value);
				if (!it->second.bHoverVariable) {
					if (strncmp(value, "<Cannot display a multidimensional array>", 40) == 0) {
						if (it->first.length() > 0) {
							if (it->first[it->first.length() - 1] != ']') {
								char cTmp[MAX_PATH + 8];
								strncpy(cTmp, watch, MAX_PATH);
								strcat(cTmp, "[0]");
								//Try to change it into a multidimensional array.
								deleteWatch(it->first.c_str());
								addWatch(cTmp, false);
							}
						}
					}
				}
			}
		}
	}
}
void addWatch(const char *watch,bool hovervariable)
{
	if (watch && pCurrentSelectedProject ) {
		if (strlen(watch) < MAX_PATH) {
			uString szTrim = watch;
			szTrim.ReplaceStr(" ", "");
			szTrim.ReplaceStr("\t", "");
			szTrim.ReplaceStr("\n", "");
			szTrim.ReplaceStr(";", "");
			char tmp[MAX_PATH+7];
			WatchValues wtmp;
			strcpy(wtmp.value, "na");
			if (!hovervariable) {
				//Convert from hover to non hover variable.
				auto it = pCurrentSelectedProject->debugWatch.find(szTrim.GetStr());
				if (it != pCurrentSelectedProject->debugWatch.end())
				{
					it->second.bHoverVariable = false;

					if (strncmp(it->second.value, "<Cannot display a multidimensional array>", 40) == 0) {
						if (it->first.length() > 0) {
							if (it->first[it->first.length() - 1] != ']') {
								char cTmp[MAX_PATH + 8];
								strncpy(cTmp, watch, MAX_PATH);
								strcat(cTmp, "[0]");
								//Try to change it into a multidimensional array.
								deleteWatch(it->first.c_str());
								addWatch(cTmp, false);
							}
						}
					}

				}
			}
			wtmp.bHoverVariable = hovervariable;
			pCurrentSelectedProject->debugWatch.insert(std::make_pair(std::string(szTrim.GetStr()), wtmp));
			sprintf(tmp, "watch %s", szTrim.GetStr());
			uString bc_err;
			BroadcasterCommand(tmp,bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "addWatch: %s\n", bc_err.GetStr());
		}
	}
}
void deleteWatch(const char *watch)
{
	if (watch && pCurrentSelectedProject) {
		uString szTrim = watch;
		szTrim.ReplaceStr(" ", "");
		szTrim.ReplaceStr("\t", "");
		szTrim.ReplaceStr("\n", "");
		szTrim.ReplaceStr(";", "");
		if (pCurrentSelectedProject->debugWatch.count(szTrim.GetStr()) != 0) {
			pCurrentSelectedProject->debugWatch.erase(szTrim.GetStr());
			char tmp[MAX_PATH+7];
			sprintf(tmp, "delete watch %s", szTrim.GetStr());
			uString bc_err;
			BroadcasterCommand(tmp,bc_err);
			if (bc_err.GetLength() > 0)
				AddToCompilerLog(2, "deleteWatch: %s\n", bc_err.GetStr());
		}
	}
}

/*

			auto it = mLanguageDefinition.mIdentifiers.find(id);
				if (it != mLanguageDefinition.mIdentifiers.end())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(it->second.mDeclaration.c_str());

if (bpts.count(mState.mCursorPosition.mLine + 1) != 0)
{
	//Already there , remove.
	//bpts.find()

	bpts.erase(mState.mCursorPosition.mLine + 1);
	//bpts.clear();
	bpts.insert(mState.mCursorPosition.mLine + 1);
*/

static CompilerLog error_log;
extern bool compiler_done;

void ClearCompilerLog(void)
{
	error_log.Clear();
}

//Max log size 10mb. 10485760
//Test using 1mb first should hold plenty.

#define MAXDEBUGLOGSIZE 1048576

ImGuiTextBuffer logBuf;
void AddToCompilerLog(int mcolor, const char *logfmt, ... )
{
	logBuf.clear();
	va_list args;
	va_start(args, logfmt);
	logBuf.appendfv(logfmt, args);
	va_end(args);
	error_log.AddLog(mcolor, logBuf.c_str() );

	if (error_log.Buf.size() > MAXDEBUGLOGSIZE) {
		error_log.Clear();
	}

}

#ifdef AGK_WINDOWS
#include <io.h>
#endif
static int first_time_debugger_used = true;
void ShowCompilerLog(const char *window , bool* p_open)
{

	if (compiler_done && AGK_Compiler::GetCompilerErrorCount() > 0) {

		char * compilerErrors = AGK_Compiler::GetCompileErrors();
		error_log.AddLog(1,compilerErrors);
		AGK_Compiler::CompilerDeleteString( compilerErrors );

		//Clear old errors.
		ClearAllEditorMarkers();

		//Parse log and set editor errors.
		const char* buf_begin = error_log.Buf.begin();
		const char* line = buf_begin;
		for (int line_no = 0; line != NULL; line_no++)
		{
			const char* line_end = (line_no < error_log.LineOffsets.Size) ? buf_begin + error_log.LineOffsets[line_no] : NULL;
			if (line_end != NULL) {
				char tmpline[1024];
				int tmplinesize = line_end - line;
				if (tmplinesize > 0 && tmplinesize < 1024) {
					strncpy(&tmpline[0], line, line_end - line);
					tmpline[line_end - line] = 0;
					ParseCompilerLog(tmpline, true);
				}
			}
			line = line_end && line_end[1] ? line_end + 1 : NULL;
		}

		compiler_done = false;
		AddToCompilerLog(2, "Compilation failed.\n");
	}
	else if (compiler_done && AGK_Compiler::GetCompilerErrorCount() == 0) {
		AddToCompilerLog(2, "Compilation finished successfully.\n");

		char * compilerInfo = AGK_Compiler::GetCompileInfo();
		AddToCompilerLog(2,"%s",compilerInfo);
		AGK_Compiler::CompilerDeleteString( compilerInfo );
		
		compiler_done = false;
	}


	const char * tmp = GetLastDebugMessageA();
	while (tmp) {

		unsigned int length = strlen(tmp);
		
		//Placing the logcopy outside the heap seam to cause some problems.
		char *logcopy = new char[ length + 2 ]; // Try a heap copy.
		strcpy(logcopy, tmp);

		if (logcopy[length - 1] != '\n') {
			logcopy[length - 1] = '\n';
			logcopy[length] = 0;
		}

		bool bNoLog = false;
		bool bBreak = false;
		if (strstr(logcopy, "Frame:"))
			bNoLog = true;
		else if (strstr(logcopy, "Variable:"))
			bNoLog = true;
		else if (strstr(logcopy, "Break:")) {
			bNoLog = true;
			bBreak = true;
		}
		if (!bNoLog) {
			if (strncmp(logcopy, "Error:", 6) == 0)
				AddToCompilerLog(1, logcopy);
			else
				AddToCompilerLog(2, logcopy);
		}

		int lastcount = GetDebugMessageCountA();
		if (lastcount > 30) {
			ResetDebugMessageLogA();
			delete [] logcopy;
			break;
		}

		if (bBreak && first_time_debugger_used && pref.bDebugBringToFront) {
			//Ignore first time break, if we did it.
			first_time_debugger_used = false;
		}
		else
			reactToDebugLog(logcopy);

		delete [] logcopy;

		tmp = GetLastDebugMessageA();



/* Give exception problems in uString.Append
		uString clean;
		clean.SetStr(tmp);

		clean.ReplaceStr("\n", ""); // can generate endless loop.
		clean.Append("\n"); // can generate exception free_heap.
		if (clean.FindStr("Frame:") < 0 ) { //&& !bAutoContinue
			if(clean.FindStr("Variable:") < 0 )
				if (clean.FindStr("Break:") < 0)
					AddToCompilerLog(2, clean.GetStr());
		}
		reactToDebugLog(clean.GetStr());
		tmp = (char *) GetLastDebugMessageA();
*/
	}

	error_log.Draw(window, p_open);
}

void error_log_scroll_to_top( void )
{
	pref.bDisplayMessageWindow = true;
	error_log.ScrollToTop = true;
}
void reactToDebugLog(const char * tmp)
{
	uString szCommand, szFull, szRest, szFile,szLineNo, szErrorWindow;
	TextEditor * m_foundeditor;

	if( (remote_debug_is_running || debug_is_running) && pCurrentSelectedProject ) {
		szFull = tmp;
		szFull.ReplaceStr("\n", "");
		szFull.ReplaceStr("\\", "/"); //PE: we should use tmp directly further down.

		int pos = szFull.Find(':');
		if (pos >= 0) {
			szFull.SubString(szRest, pos + 1);
			szFull.SubString(szCommand, 0, pos);
			if (szCommand.CompareTo("Variable") == 0 ) {
				//Variable should use raw tmp.
				szFull = tmp;
				szFull.ReplaceStr("\n", "");
				szFull.SubString(szRest, pos + 1);

				pos = szRest.Find(':');
				if (pos >= 0) {
					uString szVariable, szValue;
					szRest.SubString(szVariable, 0, pos);
					szRest.SubString(szValue, pos + 1);
					szValue.ReplaceStr("\x1", ":"); // Strange this happens somewhere in the player app ?
					szValue.ReplaceStr("<Not Found>", "undefined"); // Change not found to undefined.
					updateWatch(szVariable.GetStr(), szValue.GetStr());
				}
			}
			else if (szCommand.CompareTo("Frame") == 0) {
				pos = szRest.Find(':');
				if (pos >= 0) {
					uString szFrame, szFunction , szCode, szLineno ,szValue;
					char cTmp[MAX_PATH];
					//Frame:0:update_particle:code/particle.agc:71
					szRest.SubString(szFrame, 0, pos);
					szRest.SubString(szValue, pos + 1);
					if (szFrame[0] == '0' || atoi(szFrame.GetStr()) >= 1) {
						int pos2 = szValue.Find(':');
						if (pos2 >= 0) {
							szValue.SubString(szFunction, 0, pos2);
							szValue.SubString(szRest, pos2 + 1);
							int pos3 = szRest.Find(':');
							if (pos3 >= 0) {
								szRest.SubString(szCode, 0, pos3);
								szRest.SubString(szLineno, pos3 + 1);

								int lineno = atoi(szLineno);
								if (lineno >= 0) {
//									if(lineno > 0)
//										sprintf(cTmp, "%d", lineno-1 );
//									else
										sprintf(cTmp, "%d", lineno);
								}
								else {
									strcpy(cTmp, "na");
								}

								if (atoi(szFrame.GetStr()) == 0) {

									debugCallStack = szFunction;
									debugCallStack.Append(" in ");
									debugCallStack.Append(szCode);
									debugCallStack.Append(" at line ");
									debugCallStack.Append(cTmp);
									combo_callstack_entrys = 0;
									callstack_items[combo_callstack_entrys] = szFunction;
									if(callstack_items[combo_callstack_entrys].FindStr("<main>",1) < 0)
										callstack_items[combo_callstack_entrys].Append("()");
									callstack_items[combo_callstack_entrys].Append(" in ");
									callstack_items[combo_callstack_entrys].Append(szCode);
									callstack_items[combo_callstack_entrys].Append(" at line ");
									callstack_items[combo_callstack_entrys].Append(cTmp);
									combo_callstack_entrys++;
									current_callstack_search = NULL;

								}
								else {
									if (combo_callstack_entrys < 10) {
										debugCallStack.Append("\n");
										debugCallStack.Append("Called from ");
										debugCallStack.Append(szFunction);
										debugCallStack.Append(" in ");
										debugCallStack.Append(szCode);
										debugCallStack.Append(" at line ");
										debugCallStack.Append(cTmp);

										callstack_items[combo_callstack_entrys] = szFunction;
										if (callstack_items[combo_callstack_entrys].FindStr("<main>", 1) < 0)
											callstack_items[combo_callstack_entrys].Append("()");
										callstack_items[combo_callstack_entrys].Append(" in ");
										callstack_items[combo_callstack_entrys].Append(szCode);
										callstack_items[combo_callstack_entrys].Append(" at line ");
										callstack_items[combo_callstack_entrys].Append(cTmp);
										combo_callstack_entrys++;
									}
								}
								//agk::Print(szRest);
							}
						}
					}
//					updateWatch(szVariable.GetStr(), szValue.GetStr());
				}
			}
			else if (!bAutoContinue && szCommand.CompareTo("Break") == 0) {

				pos = szRest.Find(':');
				if (pos >= 0) {
					szRest.SubString(szFile, 0, pos);
					szRest.SubString(szLineNo, pos+1);
					int lineno = atoi(szLineNo.GetStr());

					if (lineno > 0) {
						m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)szFile.GetStr());

						if (m_foundeditor) {
							szNextWindowFocus = szFile;
							if (pref.bRemovePathFromTabName) {
								uString newTabname = szNextWindowFocus;
								int ntb_pos = newTabname.RevFindStr("/");
								if (ntb_pos <= 0)
									ntb_pos = newTabname.RevFindStr("\\");
								if (ntb_pos > 0)
									newTabname.SubString(szNextWindowFocus, ntb_pos + 1);
							}
							szNextWindowFocus.Append("##");
							szNextWindowFocus.Append(pCurrentSelectedProject->m_sProjectFile);
							cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
							ImGui::SetWindowFocus(cNextWindowFocus);
							bFocusActiveEditor = true;

							//Check if file is open in editor.
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = FindFileFromEditor(m_foundeditor);
							if (m_pCurrentFile) {
									if (!m_pCurrentFile->m_bOpenForEdit || (m_pCurrentFile->m_editor && !m_pCurrentFile->m_editor->bEditorVisible )  ) {
										//Not loaded open so we can see error.
										vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
										m_pCurrentFile->m_bOpenForEdit = true;
										cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									}
							}
							m_foundeditor->SetCursorPosition(TextEditor::Coordinates(lineno - 1, 0));
							m_foundeditor->EnsureCursorVisible();
							m_foundeditor->mScrollToCursor = true;
							ClearExecuteLine();
							m_foundeditor->executeLine = lineno;
#ifdef AGK_WINDOWS
							SetForegroundWindow(g_agkhWnd);
							SetFocus(g_agkhWnd);
							//SetActiveWindow(hwnd); //not needed?
							//EnableWindow(hwnd, TRUE); //not needed ?
#endif
						}
					}

				}
				//szFile
				//agk::Print(szRest);
			}
		}
	}
}

void ParseCompilerLog(char *logline,bool seteditorerror)
{
	uString m_szParse, m_szFileName, m_szRest , m_szLineno ,m_szErrorWindow;
	m_szParse = logline;
	TextEditor * m_foundeditor;
	bool gotolinefound = false;
	m_szParse.ReplaceStr("ERROR: 0:", "0("); // Mac glsl.

	int pos = m_szParse.Find(':');
	if (pos >= 0) {
		m_szParse.SubString(m_szRest, pos + 1);
		m_szParse.SubString(m_szFileName, 0, pos);

		m_szLineno = "";
		//+		m_pData	0x16eb63f8 "0(28) "	char *
		//mCompiledProject == NULL &&
		if ( m_CompiledEditor && m_szFileName.FindStr("0(") >= 0) {
			//GLSL compiler log.
			m_szFileName.ReplaceStr("0(","");
			m_szFileName.ReplaceStr(")","");
			m_szFileName.ReplaceStr(" ","");
			int lineno = atoi(m_szFileName.GetStr());
			if (lineno > 0) {
				m_foundeditor = m_CompiledEditor;
				m_foundeditor->editor_markers.insert(std::make_pair(lineno-1, m_szRest.GetStr()));
				m_foundeditor->SetErrorMarkers(m_foundeditor->editor_markers);
				if(lineno >= 2)
					m_foundeditor->SetCursorPosition(TextEditor::Coordinates(lineno - 2, 0));
				m_foundeditor->EnsureCursorVisible();
				m_foundeditor->mScrollToCursor = true;
			}
		}
		else {
			int pos = m_szRest.Find(':');
			if (pos >= 0) {
				m_szRest.SubString(m_szLineno, 0, pos);
				m_szRest.SubString(m_szRest, pos + 1);
			}
		}

		int lineno = atoi(m_szLineno.GetStr());

		if(seteditorerror) {
			if( strlen(m_szLineno.GetStr()) > 0 && pCurrentSelectedProject ) {

				if (lineno > 0) { // should also be able to use zero.

					//Set new error in editor.
					m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)m_szFileName.GetStr());

					if (mCompiledProject == NULL && m_CompiledEditor) {
						m_foundeditor = m_CompiledEditor;
					}

					if (m_foundeditor) {
						m_foundeditor->editor_markers.insert(std::make_pair(lineno, m_szRest.GetStr()));
						m_foundeditor->SetErrorMarkers(m_foundeditor->editor_markers);
					}
				}
			}
			return;
		}

		//Set focus on clicked error.
		bool markaserror = true;
		if (pCurrentSelectedProject) {
			TextEditor * m_foundeditor;
			m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)m_szFileName.GetStr());

			if (mCompiledProject == NULL && m_CompiledEditor) {
				cProjectItem::sProjectFiles * m_pCurrentFile;
				m_pCurrentFile = FindFileFromEditor(m_CompiledEditor);
				
				if (m_pCurrentFile) {
					m_foundeditor = m_CompiledEditor;
					ImGui::SetWindowFocus(m_pCurrentFile->m_sEditName.GetStr());
				}

			}
			else
			{
				if (!m_foundeditor) {
					//Not found , try to parse from line.
					//m_szParse = {m_pData=0x1544afc0 "Error:Virtual button 2 does not exist in main.agc at line 109" m_iNumChars=61 m_iLength=...}

					//Check if this is a debugger error.
					if( m_szParse.FindStr("Error:", 1) >= 0 || m_szParse.FindStr("Warning:", 1) >= 0) {
						int pos = m_szParse.FindStr(" at line ", 1);
						if (pos >= 0) {
							//
							m_szParse.SubString(m_szLineno, pos + 8);
							lineno = atoi(m_szLineno.GetStr());
							int pos2 = m_szParse.RevFindStr(" in ", 1);
							//int pos2 = m_szParse.RevFindStr(" in ", 1);
							if (pos2 >= 0) {
								//m_szParse = {m_pData=0x143ddab0 "Error:Virtual button 1 does not exist in main.agc at line 86" m_iNumChars=60 m_iLength=...}
								m_szParse.SubString(m_szFileName, pos2+4);
								pos = m_szFileName.FindStr(" at line ", 1);
								if (pos >= 0) {
									m_szFileName.SubString(m_szRest, 0 , pos );
									m_szFileName = m_szRest;
								}

								if (m_szParse.FindStr("Warning:", 1) >= 0) {
									m_szParse.SubString(m_szRest, 8);
									markaserror = false;
								}
								else
									m_szParse.SubString(m_szRest, 6);
								//Support relative path here:
								//m_szFileName = {m_pData=0x1456b6e8 "media/particle/particle.agc" m_iNumChars=27 m_iLength=27 ...}
								m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)m_szFileName.GetStr());

								if (m_foundeditor) {
									cProjectItem::sProjectFiles * m_pCurrentFile;
									m_pCurrentFile = FindFileFromEditor(m_foundeditor);
									if (m_pCurrentFile)
										m_szFileName = m_pCurrentFile->m_sName;

									szNextWindowFocus = m_szFileName;
									if (pref.bRemovePathFromTabName) {
										uString newTabname = szNextWindowFocus;
										int ntb_pos = newTabname.RevFindStr("/");
										if (ntb_pos <= 0)
											ntb_pos = newTabname.RevFindStr("\\");
										if (ntb_pos > 0)
											newTabname.SubString(szNextWindowFocus, ntb_pos + 1);
									}
									szNextWindowFocus.Append("##");
									szNextWindowFocus.Append(pCurrentSelectedProject->m_sProjectFile);

									//PE: 030320 , use found files edit name directly.
									szNextWindowFocus = m_pCurrentFile->m_sEditName;

									cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
									gotolinefound = true;
									ImGui::SetWindowFocus(cNextWindowFocus);
									bFocusActiveEditor = true;
								}
							}

						}
					}
					//agk::Print(m_szParse.GetStr());
				}
				else {
					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = FindFileFromEditor(m_foundeditor);
					if (m_pCurrentFile)
						m_szFileName = m_pCurrentFile->m_sName;

					szNextWindowFocus = m_szFileName;
					if (pref.bRemovePathFromTabName) {
						uString newTabname = szNextWindowFocus;
						int ntb_pos = newTabname.RevFindStr("/");
						if (ntb_pos <= 0)
							ntb_pos = newTabname.RevFindStr("\\");
						if (ntb_pos > 0)
							newTabname.SubString(szNextWindowFocus, ntb_pos + 1);
					}
					//m_sEditName = { m_pData = 0x16141138 "particle.agc##tests16.agk" m_iNumChars = 25 m_iLength = 25 ... }
					szNextWindowFocus.Append("##");
					szNextWindowFocus.Append(pCurrentSelectedProject->m_sProjectFile);

					//PE: 030320 , use found files edit name directly.
					szNextWindowFocus = m_pCurrentFile->m_sEditName;

					cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
					ImGui::SetWindowFocus(cNextWindowFocus);
					bFocusActiveEditor = true; //gotolinefound
				}
			}
			bool bSearchResult = false;
			int iColumn = 0, iColumnEnd = 0;
			if (m_szRest.FindStr("Search:") >= 0) {
				markaserror = false;
				bSearchResult = true;
				uString szColumn;
				int pos = m_szRest.FindStr(":",0,7);
				if (pos > 0) {
					m_szRest.SubString(szColumn, 7, pos-7);
					iColumn = atoi(szColumn.GetStr());

					pos++;
					int pos2 = m_szRest.FindStr(":", 0, 7+(pos - 7));
					m_szRest.SubString(szColumn, 7 + (pos - 7), pos2 - (7 + (pos - 7)) );
					iColumnEnd = atoi(szColumn.GetStr());


				}


			}
			if (m_foundeditor && lineno > 0 ) {
				//Check if file is open in editor.
				cProjectItem::sProjectFiles * m_pCurrentFile;
				m_pCurrentFile = FindFileFromEditor(m_foundeditor);
				if (m_pCurrentFile) {
					if (!m_pCurrentFile->m_bOpenForEdit || (m_pCurrentFile->m_editor && !m_pCurrentFile->m_editor->bEditorVisible)  ) {
						//Not loaded open so we can see error.
						vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
						m_pCurrentFile->m_bOpenForEdit = true;
						//refresh_gui_docking = 1; //Not needed any more.
						cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
						bFocusActiveEditor = true;
					}
					else {
						if (bSearchResult) {
							cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
							bFocusActiveEditor = true;
							m_pCurrentFile->m_editor->bSearchActive = false;
						}
					}
				}
				if (markaserror) {
					m_foundeditor->editor_markers.insert(std::make_pair(lineno, m_szRest.GetStr()));
					m_foundeditor->SetErrorMarkers(m_foundeditor->editor_markers);
				}
				if (iColumn >= 0) {
					m_foundeditor->SetCursorPosition(TextEditor::Coordinates(lineno - 1, iColumn));
					if (iColumnEnd > iColumn) {
						TextEditor::Coordinates sstart,send;
						sstart.mLine = lineno - 1;
						sstart.mColumn = iColumn;
						send = sstart;
						send.mColumn = iColumnEnd;
						m_foundeditor->SetSelection(sstart, send, TextEditor::SelectionMode::Normal );
					}
				}
				else
					m_foundeditor->SetCursorPosition(TextEditor::Coordinates(lineno - 1, 0));
				m_foundeditor->EnsureCursorVisible();
				m_foundeditor->mScrollToCursor = true;

			}
			else {
				if (lineno > 0) {
					//Must be a file outside the project
					//Try to open it directly.
					uString szFinalName = pCurrentSelectedProject->m_sProjectFileFullPath;
					szFinalName.ReplaceStr("\\", "/");
					int pos = szFinalName.RevFindStr("/");
					if (pos >= 0) {
						uString szPath;
						char cRealPath[MAX_PATH];
						szFinalName.SubString(szPath, 0, pos + 1);

						szFinalName = szPath;
						szFinalName.Append(m_szFileName);
						//szFinalName = {m_pData=0x12fea530 "D:/github/AGKTier2/AgkIde/Final/testprojects/Terrain/../../testcode/particle.agc" ...}
						int retval = cp_realpath((char *)szFinalName.GetStr(), &cRealPath[0], true);
						if (retval > 0) {
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = CreateNewAGCFile(cRealPath, (char *)m_szFileName.GetStr());
							if (m_pCurrentFile) {
								vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
								m_pCurrentFile->m_bOpenForEdit = true;
								cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
								bFocusActiveEditor = true;
								if (m_pCurrentFile->m_editor) {
									if (markaserror) {
										m_pCurrentFile->m_editor->editor_markers.insert(std::make_pair(lineno, m_szRest.GetStr()));
										m_pCurrentFile->m_editor->SetErrorMarkers(m_pCurrentFile->m_editor->editor_markers);
									}
									m_pCurrentFile->m_editor->SetCursorPosition(TextEditor::Coordinates(lineno - 1, 0));
									m_pCurrentFile->m_editor->EnsureCursorVisible();
									m_pCurrentFile->m_editor->mScrollToCursor = true;
								}
							}
							//agk::Print(cRealPath);
						}
					}
				}

/*
				if (cFileSelected && strlen(cFileSelected) > 0) {

					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = CreateNewAGCFile(cFileSelected);
					if (m_pCurrentFile) {
						vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
						m_pCurrentFile->m_bOpenForEdit = true;
						cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
					}
				}
*/
				//pCurrentSelectedProject
				//m_szFileName.GetStr()
			}

		}

	}

}


//Noc File dialog cross platform.

const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name);

#include <stdlib.h>
#include <string.h>

static char *g_noc_file_dialog_ret = NULL;


#ifdef AGK_WINDOWS

#include "windows.h"
#include "winuser.h"
//#include <commdlg.h>
#include <shlobj.h>
#include <conio.h>

static int __stdcall BrowseCallbackProcW(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
#ifdef UNICODE
		SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)pData);
#else
		SendMessageA(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)pData);
#endif
	}
	return 0;
}

const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name)
{
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[270];       // buffer for file name
	int ret;
	szFile[0] = '\0';

	if (flags & NOC_FILE_DIALOG_OPEN) {
		if (default_path)
			strcpy(szFile, default_path);
	}


	if (flags & NOC_FILE_DIALOG_DIR) {

		static wchar_t lBuff[MAX_PATH];
		wchar_t aTitle[MAX_PATH];
		BROWSEINFOW bInfo;
		LPITEMIDLIST lpItem;
		HRESULT lHResult;

		CoUninitialize();
		lHResult = CoInitialize(NULL); //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

		wsprintf( aTitle, L"%s" , L"Select folder");

		//bInfo.hwndOwner = GetForegroundWindow();
		bInfo.hwndOwner = g_agkhWnd;
		
		bInfo.pidlRoot = NULL;
		lBuff[0] = '\0';
		bInfo.pszDisplayName = lBuff;
		bInfo.lpszTitle = aTitle && wcslen(aTitle) ? aTitle : NULL;
		if (lHResult == S_OK || lHResult == S_FALSE)
		{
			bInfo.ulFlags = BIF_USENEWUI;

			bInfo.lpfn = NULL; //BrowseCallbackProcW;
			if (default_path)
				bInfo.lParam = (LPARAM)default_path;
			else
				bInfo.lParam = (LPARAM)NULL;
			bInfo.iImage = -1;

			lpItem = SHBrowseForFolderW(&bInfo);
			if (lpItem)
			{
				SHGetPathFromIDListW(lpItem, lBuff);
			}

			if (lHResult == S_OK || lHResult == S_FALSE)
			{
				CoUninitialize();
			}
		}
		sprintf(szFile, "%ws", lBuff);

		//Make sure ther blocking dialog did not skip some keys, reset.
		ImGuiIO& io = ImGui::GetIO();
		io.KeySuper = false;
		io.KeyCtrl = false;
		io.KeyAlt = false;
		io.KeyShift = false;
		agk::KeyUp(13); // reset enter key.
		agk::KeyUp(16); // reset shift key.
		agk::KeyUp(17); // reset ctrl key.
		agk::KeyUp(18); // reset alt key.
		agk::KeyUp(19); // reset pause key.

		io.KeysDown[13] = false; //also reset imgui keys.
		io.KeysDown[16] = false;
		io.KeysDown[17] = false;
		io.KeysDown[18] = false;
		io.KeysDown[19] = false;

		return strdup(szFile);
	}


	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.hwndOwner = g_agkhWnd;

	if (flags & NOC_FILE_DIALOG_DIR) {
		
		ofn.Flags = OFN_CREATEPROMPT;
		ret = GetOpenFileNameA(&ofn);
	}
	else if (flags & NOC_FILE_DIALOG_OPEN ) // || flags
		ret = GetOpenFileNameA(&ofn);
	else
		ret = GetSaveFileNameA(&ofn);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;

	agk::KeyUp(13); // reset enter key.
	agk::KeyUp(16); // reset shift key.
	agk::KeyUp(17); // reset ctrl key.
	agk::KeyUp(18); // reset alt key.
	agk::KeyUp(19); // reset pause key.

	io.KeysDown[13] = false; //also reset imgui keys.
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;

	if(g_noc_file_dialog_ret != NULL)
		free(g_noc_file_dialog_ret);
	g_noc_file_dialog_ret = ret ? strdup(szFile) : NULL;
	return g_noc_file_dialog_ret;
}

#else
#ifdef AGK_MACOS

#include <AppKit/AppKit.h>

const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name)
{
	NSURL *url;
	const char *utf8_path;
	NSSavePanel *panel;
	NSOpenPanel *open_panel;
	NSMutableArray *types_array;
	NSURL *default_url;
	char buf[256], *patterns;
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	if (flags & NOC_FILE_DIALOG_OPEN) {
		panel = open_panel = [NSOpenPanel openPanel];
	}
	else {
		panel = [NSSavePanel savePanel];
	}

	if (flags & NOC_FILE_DIALOG_DIR) {
		[open_panel setCanChooseDirectories : YES];
		[open_panel setCanChooseFiles : NO];
	}

	if (default_path) {
		default_url = [NSURL fileURLWithPath :
		[NSString stringWithUTF8String : default_path]];
		[panel setDirectoryURL : default_url];
		[panel setNameFieldStringValue : default_url.lastPathComponent];
	}

	if (filters) {
		types_array = [NSMutableArray array];
		while (*filters) {
			filters += strlen(filters) + 1; // skip the name
			strcpy(buf, filters);
			buf[strlen(buf) + 1] = '\0';
			for (patterns = buf; *patterns; patterns++)
				if (*patterns == ';') *patterns = '\0';
			patterns = buf;
			while (*patterns) {
				assert(strncmp(patterns, "*.", 2) == 0);
				patterns += 2; // Skip the "*."
				[types_array addObject : [NSString stringWithUTF8String : patterns]];
				patterns += strlen(patterns) + 1;
			}
			filters += strlen(filters) + 1;
		}
		[panel setAllowedFileTypes : types_array];
	}

	if (g_noc_file_dialog_ret != NULL)
		free(g_noc_file_dialog_ret);

	g_noc_file_dialog_ret = NULL;
	if ([panel runModal] == NSModalResponseOK) {
		url = [panel URL];
		utf8_path = [[url path] UTF8String];
		g_noc_file_dialog_ret = strdup(utf8_path);
	}

	[pool release];
	return g_noc_file_dialog_ret;
}

#else
//Linux.
#include <gtk/gtk.h>

static char selected_char[1024];


const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name)
{
	GtkWidget *dialog;
	GtkFileFilter *filter;
	GtkFileChooser *chooser;
	GtkFileChooserAction action;
	gint res;
	char buf[128], *patterns;

	if (flags & NOC_FILE_DIALOG_DIR)
	{
		strcpy(selected_char, "");
		FILE *f = popen("zenity --file-selection --directory", "r");
		fgets(selected_char, 1024, f);
		pclose(f);
		int length = strlen(selected_char);
		if (length < 2)
		{
			return NULL;
		}
		if (selected_char[length - 1] == '\n' || selected_char[length - 1] == '\r') selected_char[length - 1] = 0;

		return &selected_char[0];
	}

	if (flags & NOC_FILE_DIALOG_OPEN) {
		char cmd[1024],cmd1[1024];
		//
		if (default_path) {
			if (default_name)
				sprintf(cmd, "zenity --title \"Open File\" --file-selection --filename=\"%s/%s\"", default_path, default_name);
			else
				sprintf(cmd, "zenity --title \"Open File\" --file-selection --filename=\"%s\"", default_path);
		}
		else {
			strcpy(cmd, "zenity --title \"Open File\" --file-selection");
		}


		if (filters) {
			sprintf(cmd1, " --file-filter='(%s) | *.%s'", filters, filters);
			strcat(cmd, cmd1);
		}

		strcpy(selected_char, "");
		FILE *f = popen(cmd, "r");
		fgets(selected_char, 1024, f);
		pclose(f);
		int length = strlen(selected_char);
		if (length < 2)
		{
			return NULL;
		}
		if (selected_char[length - 1] == '\n' || selected_char[length - 1] == '\r') selected_char[length - 1] = 0;

		return &selected_char[0];
	}


	if (flags & NOC_FILE_DIALOG_SAVE) {
		char cmd[1024], cmd1[1024];
		//
		if (default_path) {
			if (default_name)
				sprintf(cmd, "zenity --save --title \"Save File\" --file-selection --filename=\"%s/%s\"", default_path, default_name);
			else {
				if(default_path[ strlen(default_path) - 1] == '/')
					sprintf(cmd, "zenity --save --title \"Save File\" --file-selection --filename=\"%s\"", default_path);
				else
					sprintf(cmd, "zenity --save --title \"Save File\" --file-selection --filename=\"%s/\"", default_path);
			}
		}
		else {
			strcpy(cmd, "zenity --save --title \"Save File\" --file-selection");
		}


		if (filters) {
			sprintf(cmd1, " --file-filter='(%s) | *.%s'", filters, filters);
			strcat(cmd, cmd1);
		}

		strcpy(selected_char, "");
		FILE *f = popen(cmd, "r");
		fgets(selected_char, 1024, f);
		pclose(f);
		int length = strlen(selected_char);
		if (length < 2)
		{
			return NULL;
		}
		if (selected_char[length - 1] == '\n' || selected_char[length - 1] == '\r') selected_char[length - 1] = 0;

		return &selected_char[0];
	}

	//zenity --file-selection --file-filter='PDF files (pdf) | *.pdf'
#ifdef USEGTKDIRECTLY
	action = flags & NOC_FILE_DIALOG_SAVE ? GTK_FILE_CHOOSER_ACTION_SAVE :
		GTK_FILE_CHOOSER_ACTION_OPEN;
	if (flags & NOC_FILE_DIALOG_DIR)
		action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;

	gtk_init_check(NULL, NULL);
	dialog = gtk_file_chooser_dialog_new(
		flags & NOC_FILE_DIALOG_SAVE ? "Save File" : "Open File",
		NULL,
		action,
		"_Cancel", GTK_RESPONSE_CANCEL,
		flags & NOC_FILE_DIALOG_SAVE ? "_Save" : "_Open", GTK_RESPONSE_ACCEPT,
		NULL);
	chooser = GTK_FILE_CHOOSER(dialog);
	if (flags & NOC_FILE_DIALOG_OVERWRITE_CONFIRMATION)
		gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

	if (default_path)
		gtk_file_chooser_set_filename(chooser, default_path);
	if (default_name)
		gtk_file_chooser_set_current_name(chooser, default_name);

	while (filters && *filters) {
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, filters);
		filters += strlen(filters) + 1;

		// Split the filter pattern with ';'.
		strcpy(buf, filters);
		buf[strlen(buf)] = '\0';
		for (patterns = buf; *patterns; patterns++)
			if (*patterns == ';') *patterns = '\0';
		patterns = buf;
		while (*patterns) {
			gtk_file_filter_add_pattern(filter, patterns);
			patterns += strlen(patterns) + 1;
		}

		gtk_file_chooser_add_filter(chooser, filter);
		filters += strlen(filters) + 1;
	}

	res = gtk_dialog_run(GTK_DIALOG(dialog));

	free(g_noc_file_dialog_ret);
	g_noc_file_dialog_ret = NULL;

	if (res == GTK_RESPONSE_ACCEPT)
		g_noc_file_dialog_ret = gtk_file_chooser_get_filename(chooser);
	gtk_widget_destroy(dialog);
	while (gtk_events_pending()) gtk_main_iteration();
	return g_noc_file_dialog_ret;
#endif

}

#endif
#endif


bool CancelQuit()
{
	if (pref.iCancelQuitDialog)
	{
		boxer::Selection selection;
		selection = boxer::show("Are you sure you want to quit ?", " Warning!", boxer::Style::Question, boxer::Buttons::OKCancel);

		//Make sure ther blocking dialog did not skip some keys, reset.
		ImGuiIO& io = ImGui::GetIO();
		io.KeySuper = false;
		io.KeyCtrl = false;
		io.KeyAlt = false;
		io.KeyShift = false;
		agk::KeyUp(13); // reset enter key
		agk::KeyUp(16); // reset shift key.
		agk::KeyUp(17); // reset ctrl key.
		agk::KeyUp(18); // reset alt key.
		agk::KeyUp(19); // reset pause key.
		io.KeysDown[13] = false;
		io.KeysDown[17] = false;
		io.KeysDown[18] = false;
		io.KeysDown[19] = false;

		if (selection == boxer::Selection::Cancel) {
			return true;
		}
	}
	return false;
}

bool overWriteFileBox(char * file)
{
	boxer::Selection selection;
	selection = boxer::show("Output file already exists, do you want to overwrite it?", " Confirm", boxer::Style::Question, boxer::Buttons::YesNo);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;
	agk::KeyUp(13); // reset enter key.
	agk::KeyUp(16); // reset shift key.
	agk::KeyUp(17); // reset ctrl key.
	agk::KeyUp(18); // reset alt key.
	agk::KeyUp(19); // reset pause key.
	io.KeysDown[13] = false;
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;

	if (selection == boxer::Selection::Yes) return(true);

	return(false);
}
bool askBox(char * ask , char *title)
{
	boxer::Selection selection;
	selection = boxer::show(ask, title, boxer::Style::Question, boxer::Buttons::YesNo);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;
	agk::KeyUp(13); // reset enter key
	agk::KeyUp(16); // reset shift key.
	agk::KeyUp(17); // reset ctrl key.
	agk::KeyUp(18); // reset alt key.
	agk::KeyUp(19); // reset pause key.

	io.KeysDown[13] = false; //also reset imgui keys.
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;

	if (selection == boxer::Selection::Yes) return(true);

	return(false);
}

bool changedFileBox(char * file)
{
	boxer::Selection selection;
	char msg[1024];
	//
	//sprintf(msg, "File have unsaved changes, do you want to save file?\n%s", file);
	sprintf(msg, "The file has unsaved changes. Do you want to save it?\n%s", file);
	selection = boxer::show(msg, " Warning!", boxer::Style::Question, boxer::Buttons::YesNo);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;
	agk::KeyUp(13); // reset enter key
	agk::KeyUp(16); // reset shift key.
	agk::KeyUp(17); // reset ctrl key.
	agk::KeyUp(18); // reset alt key.
	agk::KeyUp(19); // reset pause key.

	io.KeysDown[13] = false;
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;

	if (selection == boxer::Selection::Yes) return(true);

	return(false);
}

void BoxerInfo(char * text,const char *heading)
{
	boxer::show(text, heading);
	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;
	agk::KeyUp(13); // reset enter key
	agk::KeyUp(16); // reset shift key.
	agk::KeyUp(17); // reset ctrl key.
	agk::KeyUp(18); // reset alt key.
	agk::KeyUp(19); // reset pause key.

	io.KeysDown[13] = false;
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;

}

void DebugInfo(char * text, const char *heading)
{
#ifndef DEVVERSION
	return;
#else
	boxer::show(text, heading);
	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;
	agk::KeyUp(16); // reset shift key.
	agk::KeyUp(17); // reset ctrl key.
	agk::KeyUp(18); // reset alt key.
	agk::KeyUp(19); // reset pause key.
#endif
}


char JDKJaveExe[MAX_PATH];
char JDKJaveExe2[MAX_PATH];
char JDKJaveExe3[MAX_PATH];
char JDKBestPath[MAX_PATH];

void GetJDKPath(void)
{
	//Try locating jarsigner , as java executeable is also part of jre.
	#if defined(AGK_WINDOWS)
	uString sWhichCmd = "where";
	#else
	uString sWhichCmd = "which";
	#endif

	bool Valid = true;
	int iStdOutLength, iStdErrLength;
	int appid = run_command(sWhichCmd, "jarsigner", &iStdOutLength, &iStdErrLength, false);
	if (appid == false) {
		//strcpy(JDKError, "Could not find JDK java");
		Valid = false;
	}
	if (Valid)
	{
		float timestart = agk::Timer();
		int still_running = agk::GetAppRunning(appid);
		char tmp[1024] = { 0 };
		while (still_running == 1 && agk::Timer() - timestart < 3.0) //Max  3 sec.
		{
			#ifdef AGK_WINDOWS
			Sleep(100);
			#else
			usleep(100);
			#endif
			still_running = agk::GetAppRunning(appid);
		}
		tmp[0] = 0;
		tmp[1] = 0;
		int rs = read_buffers(tmp, 1024 - 90, NULL, 0);
		if (still_running)
		{
			//sprintf(JDKError, "Java check timed out, output: %s", tmp);
			Valid = false;
			agk::TerminateApp(appid);
		}
		else
		{
			#ifdef AGK_WINDOWS
			if (tmp[0] == 0 || tmp[1] != ':')
			#else
			if (tmp[0] != '/')
			#endif
			{
				//sprintf(JDKError, "Could not find Java, make sure you have the Java Development Kit (JDK) 8 or above installed", tmp);
				Valid = false;
			}
			else
			{
				char *find = (char *) pestrcasestr(tmp, "\r");
				if(!find) find = (char *) pestrcasestr(tmp, "\n");
				if (!find) find = (char *)pestrcasestr(tmp, "\0");
				if (find && find-&tmp[0] < 1024)
				{
					*find = 0;
					strcpy(JDKJaveExe, tmp);
					find++;
					if (*find == 0xd || *find == 0xa) find++;
					char *find2 = (char *)pestrcasestr(find, "\r");
					if (!find2) find2 = (char *)pestrcasestr(find, "\n");
					if (!find2) find2 = (char *)pestrcasestr(find, "\0");
					if (find2 && find2 - &tmp[0] < 1024)
					{
						*find2 = 0;
						strcpy(JDKJaveExe2, find);
					}
				}
			}
		}
	}

	char* env_val = getenv("JAVA_HOME");
	if (env_val)
	{
		strcpy(JDKJaveExe3, env_val);
	}

	//Find a path where both jarsigner and java is available. priority JAVA_HOME first.
	bool bFoundBoth = false;
	if (strlen(JDKJaveExe3) > 0)
	{
		uString mPath = "raw:";
		mPath.Append(JDKJaveExe3);
		mPath.ReplaceStr("\\", "/");
		if(JDKJaveExe3[strlen(JDKJaveExe3)-1] != '/' && JDKJaveExe3[strlen(JDKJaveExe3) - 1] != '\\')
			mPath.Append("/");
		#if defined(AGK_WINDOWS)
		mPath.Append("bin/java.exe");
		#else
		mPath.Append("bin/java");
		#endif
		if (agk::GetFileExists(mPath.GetStr()))
		{
			mPath.ReplaceStr("/java", "/jarsigner");
			if (agk::GetFileExists(mPath.GetStr()))
			{
				//Perfect use this path.
				strcpy(JDKBestPath, mPath.GetStr());
				bFoundBoth = true;
			}
		}
	}

	if (!bFoundBoth && strlen(JDKJaveExe) > 0 )
	{
		//Try from the where witch path returned. first exe.
		uString mPath = "raw:";
		mPath.Append(JDKJaveExe);
		mPath.ReplaceStr("\\", "/");
		if (agk::GetFileExists(mPath.GetStr()))
		{
			mPath.ReplaceStr("/jarsigner", "/java");
			if (agk::GetFileExists(mPath.GetStr()))
			{
				//Perfect use this path.
				strcpy(JDKBestPath, mPath.GetStr());
				bFoundBoth = true;
			}
		}
	}

	if (!bFoundBoth && strlen(JDKJaveExe2) > 0)
	{
		//Try from the where witch path returned. first exe.
		uString mPath = "raw:";
		mPath.Append(JDKJaveExe2);
		mPath.ReplaceStr("\\", "/");
		if (agk::GetFileExists(mPath.GetStr()))
		{
			mPath.ReplaceStr("/jarsigner", "/java");
			if (agk::GetFileExists(mPath.GetStr()))
			{
				//Perfect use this path.
				strcpy(JDKBestPath, mPath.GetStr());
				bFoundBoth = true;
			}
		}
	}
	if (!bFoundBoth)
	{
		#if !defined(AGK_WINDOWS)
		//On Max linux try default location.
		uString mPath = "raw:/usr/bin/jarsigner";
		if (agk::GetFileExists(mPath.GetStr()))
		{
			mPath.ReplaceStr("/jarsigner", "/java");
			if (agk::GetFileExists(mPath.GetStr()))
			{
				//Perfect use this path.
				strcpy(JDKBestPath, mPath.GetStr());
				bFoundBoth = true;
			}
		}
		#endif
	}

	if (!bFoundBoth)
	{
		strcpy(JDKBestPath, ""); //Not found.
	}
	else
	{
		//Parse.
		uString mPath = JDKBestPath;
		mPath.ReplaceStr("raw:", "");
		mPath.ReplaceStr("\\", "/");
		strcpy(JDKBestPath, mPath.GetStr());
		if (JDKBestPath[strlen(JDKBestPath) - 1] == '/')
			JDKBestPath[strlen(JDKBestPath) - 1] = 0;
		char *find = (char *)pestrcasestr(JDKBestPath, "/bin");
		if (find)
		{
			find += 4;
			*find = 0;
		}
		if (JDKBestPath[strlen(JDKBestPath) - 1] == '/')
			JDKBestPath[strlen(JDKBestPath) - 1] = 0;
		strcat(JDKBestPath, "/");
		#if defined(AGK_WINDOWS)
		mPath = JDKBestPath;
		mPath.ReplaceStr("/", "\\");
		strcpy(JDKBestPath, mPath.GetStr());
		#endif
		//PE: Everything fine , put into pref.
		strcpy(pref.cJDKPath, JDKBestPath);
	}

}

