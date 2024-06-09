#ifndef _H_AGK_GUI_
#define _H_AGK_GUI_

//#define ADDIMGUIDEMO //Add demo app to quickly test different widgets.
//#define IMGUIAL_FONTS_FONT_AWESOME //Not used yet.
#define IMGUIAL_FONTS_MATERIAL_DESIGN //Add all our font icons.

#define MAXRECENTLIST 15

void AddAllFonts(const char *cpcustomfont, int iIDEFontSize, const char *cpeditorfont, int iEditorFontSize);

#include "files.h"
#include "project.h"
#include "imgui/imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_agk.h"
#include "imgui/imguial_fonts.h"
#include "../CompilerNew/AGKCompiler2.h"

#include <iostream>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include "TextEditor.h"

bool GetTreePopFilter(cFolderItem *m_pmSubFolders, ImGuiTextFilter Filter);
void ResetLayout(void);
void ProcessPreferences(void);
void AddRecentFiles(const char * project, const char * file);
void ProcessGotoLine(void);
void ProcessKeyStore(void);
void ProcessAdditionalFiles(void);
void StartInstallProcess(void);
bool GenerateResizedImage(int icon, char *filename, int sizew, int sizeh, char *aaptcmd);
bool GenerateIPAResizedImage(int icon, char *filename, int sizew, int sizeh);
bool utils_add_folder_to_zip(int zipid, const char *folder, const char *zipfolder);
int utils_remove_folder_recursive(const char *src);
static int ios_convert_to_cgbi_png(const char* oldfilename, const char* newfilename);
void ProcessAndroidExport(void);
void ProcessHTML5Export(void);
void ProcessIOSExport(void);
void ProcessCodeProperties(void);
int test_pipes(char *cmd);
void ProcessInfoBox(void);
void ProcessAboutBox(void);
bool GetTreePop(cFolderItem *m_pmSubFolders);
bool isitemselect(void);
bool myAssetsTreenode(cFolderItem * cfolder);
ImGuiTreeNodeFlags setTreeFlags(cFolderItem * cfolder);
cFolderItem * getNextFolder(cFolderItem * m_pCurFolder);
TextEditor * FindEditorFromName(char *projectname, char *sourcefile);
TextEditor * FindEditorFromWindowName(char *windowname);
void ClearAllEditorMarkers(void);
void ClearExecuteLine(void);
char * RandomString(void);
void SetSeedStyleColors(void);
void myDarkGreyStyle(ImGuiStyle* dst);
void CustomStyleColors(ImGuiStyle* dst);
void myDefaultStyle(ImGuiStyle* dst);
//void myStyle(ImGuiStyle* dst);
void myDarkStyle(ImGuiStyle* dst);
void myLightStyle(ImGuiStyle* dst);
void ShowCompilerLog(const char *window, bool* p_open);
void ClearCompilerLog(void);
void processDebugger(void);
void addWatch(const char *watch, bool hovervariable = false);
void updateWatch(const char *watch, const char *value);
void addAllWatches(void);
void deleteWatch(const char *watch);
void ParseCompilerLog(char *logline, bool seteditorerror);
void AddToCompilerLog(int mcolor, const char *logfmt, ...);
void reactToDebugLog(const char * tmp);
void error_log_scroll_to_top(void);
bool overWriteFileBox(char * file);
bool CancelQuit();
bool askBox(char * ask, char *title);
bool changedFileBox(char * file);
void BoxerInfo(char * text, const char *heading);
void DebugInfo(char * text, const char *heading);
void PleaseWait(void);
void PleaseWaitEnd(void);
void ChangeIconSet(void);


enum {
	NOC_FILE_DIALOG_OPEN = 1 << 0,   // Create an open file dialog.
	NOC_FILE_DIALOG_SAVE = 1 << 1,   // Create a save file dialog.
	NOC_FILE_DIALOG_DIR = 1 << 2,   // Open a directory.
	NOC_FILE_DIALOG_OVERWRITE_CONFIRMATION = 1 << 3,
};
const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name);

namespace ImGui {
	bool BeginPopupContextItemAGK(const char* str_id = NULL, int mouse_button = 1);
	bool windowTabVisible(void);
	int windowTabFlags(void);
	int windowDockNodeId(void);

	bool ImgBtn(int iImageID, const ImVec2& btn_size = ImVec2(0, 0), const ImVec4& bg_col = ImColor(255, 255, 255, 0),
		const ImVec4& drawCol_normal = ImColor(220, 220, 220, 220),
		const ImVec4& drawCol_hover = ImColor(255, 255, 255, 255),
		const ImVec4& drawCol_Down = ImColor(180, 180, 160, 255), int frame_padding = 0, int atlasindex = 0, int atlasrows = 0, int atlascolumns = 0);

	bool ImgBtnBack(int iImageID, const ImVec2& btn_size = ImVec2(0, 0), const ImVec4& bg_col = ImColor(255, 255, 255, 0),
	const ImVec4& drawCol_normal = ImColor(220, 220, 220, 220),
	const ImVec4& drawCol_hover = ImColor(255, 255, 255, 255),
	const ImVec4& drawCol_Down = ImColor(180, 180, 160, 255), int frame_padding = 0);

	void ToggleButton(const char* str_id, bool* v);
	void ImRotateStart(void);
	ImVec2 ImRotationCenter(void);
	void ImRotateEnd(float rad, ImVec2 center);
}

struct SymbolsValues
{
	int iSymbolType; // 1 = functions.
	char cCommand[MAX_PATH + 1];
	char cParameters[MAX_PATH + 1];
	char cType[MAX_PATH + 1];
	int lineno;
	bool bVisible = true;
	TextEditor * m_InsideEditor;
};

struct Preferences
{
	char szCheckFile[10] = "IDE-PREFS";
	bool show_preferences_window = false;
	bool editor_display_linenumbers = true;
	bool bEnableSymboldList = true;
	bool bEnableAutoCompletion = true;
	int mTabSize = 4;
	int iEditorFontSize = 15;
	int iIDEFontSize = 15;
	bool bEnableToolbarLargeIcons = true;
	bool bAutoIndentation = true;
	bool bSmartAutoIndentation = true;
	//"Build options"
	bool bWindows64Bit = false;
	char cPathToCompiler[MAX_PATH];
	char cBroadcastPort[MAX_PATH];
	char cBroadcastIp[MAX_PATH];
	bool bBroadcastUseSteam = false;
	char cDebugIp[MAX_PATH];
	bool bAutoHideDebugWindows = true;
	bool bDisplayActiveProjectFilesOnly = false;

	char keystore_full_name[40];
	char keystore_company_name[40];
	char keystore_city_name[40];
	char keystore_country_code[4];
	char keystore_file_location[MAX_PATH];

	bool bTimestampExe = false;
	unsigned int launched = 0;
	unsigned int first_launched = 0;
	ImVec4 icon_background_color;
	bool bDisplayProjectinAssetsBrowser = false;
	bool bEnableCustomStyle = false;
	bool bEnableSeedStyle = false;
	int iSeedStyle = 1;
	int iIDESelectedStyle = 0;
	bool bLoadClassicDLConStartup = true;
	bool bLoadStudioDLConStartup = true;
	bool bRemovePathFromTabName = true;

	bool bDisplayHelpWindow = true;
	bool bDisplayMessageWindow = true;
	bool bDisplayProjectWindow = true;
	bool bDisplayDebuggerWindow = true;
	bool bDisplayMediaFilesWindow = true;
	bool bDisplayPreviewWindow = true;
	bool bDisplayAssetsBrowserWindow = true;

	int iCurrentIconSet = 2;

	bool bUseInternalMousePointer = false;
	bool bUseCodeProperties = true;

	bool bUndoCtrl = true;
	bool bUndoShift = false;
	bool bUndoAlt = false;
	int iUndoKey = 90;
	char cUndoText[32];

	bool bRedoCtrl = true;
	bool bRedoShift = false;
	bool bRedoAlt = false;
	int iRedoKey = 89;
	char cRedoText[32];

	bool bCopyCtrl = true;
	bool bCopyShift = false;
	bool bCopyAlt = false;
	int iCopyKey = 67;
	char cCopyText[32];

	bool bPasteCtrl = true;
	bool bPasteShift = false;
	bool bPasteAlt = false;
	int iPasteKey = 86;
	char cPasteText[32];

	bool bCutCtrl = true;
	bool bCutShift = false;
	bool bCutAlt = false;
	int iCutKey = 88;
	char cCutText[32];

	bool bFindCtrl = true;
	bool bFindShift = false;
	bool bFindAlt = false;
	int iFindKey = 70;
	char cFindText[32];

	bool bFindNextCtrl = false;
	bool bFindNextShift = false;
	bool bFindNextAlt = false;
	int iFindNextKey = 114;
	char cFindNextText[32];

	bool bReplaceCtrl = true;
	bool bReplaceShift = false;
	bool bReplaceAlt = false;
#ifdef AGK_WINDOWS
	int iReplaceKey = 72; //MAC 82
#else
	int iReplaceKey = 82;
#endif
	char cReplaceText[32];

	bool bZoomInCtrl = true;
	bool bZoomInShift = false;
	bool bZoomInAlt = false;
	int iZoomInKey = 107;
	char cZoomInText[32];

	bool bZoomOutCtrl = true;
	bool bZoomOutShift = false;
	bool bZoomOutAlt = false;
	int iZoomOutKey = 109;
	char cZoomOutText[32];

	bool bGotoLineCtrl = true;
	bool bGotoLineShift = false;
	bool bGotoLineAlt = false;
	int iGotoLineKey = 76;
	char cGotoLineText[32];

	bool bRunCtrl = false;
	bool bRunShift = false;
	bool bRunAlt = false;
	int iRunKey = 116;
	char cRunText[32];

	bool bCompileCtrl = false;
	bool bCompileShift = false;
	bool bCompileAlt = false;
	int iCompileKey = 118;
	char cCompileText[32];

	bool bBroadCastCtrl = false;
	bool bBroadCastShift = false;
	bool bBroadCastAlt = false;
	int iBroadCastKey = 117;
	char cBroadCastText[32];

	bool bDebugCtrl = false;
	bool bDebugShift = false;
	bool bDebugAlt = false;
	int iDebugKey = 119;
	char cDebugText[32];

	bool bOpenFileCtrl = true;
	bool bOpenFileShift = false;
	bool bOpenFileAlt = false;
	int iOpenFileKey = 79;
	char cOpenFileText[32];

	bool bOpenProjectCtrl = true;
	bool bOpenProjectShift = false;
	bool bOpenProjectAlt = false;
	int iOpenProjectKey = 80;
	char cOpenProjectText[32];

	bool bDebugStepCtrl = false;
	bool bDebugStepShift = false;
	bool bDebugStepAlt = false;
	int iDebugStepKey = 121;
	char cDebugStepText[32];

	bool bDebugBringToFront = false;
	int iIDEUpdateFPS = 2;
	bool bDebugBringAppToFront = false;
	bool bUseUpscaling = false;

	char	fillerboolfillertmp; //to make sure we align bitwise in file after a ending bool.

	bool bGoToDefinitionCtrl = true;
	bool bGoToDefinitionShift = false;
	bool bGoToDefinitionAlt = false;
	int iGoToDefinitionKey = 84;
	char cGoToDefinitionText[32];

	bool iIDEUpdateEventSleep = true;

	bool bDisplaySceneManager = true;
	bool bDisplaySceneProperties = true;


	char cDefaultProjectFolder[MAX_PATH];

	bool bDuplicateLineCtrl = true;
	bool bDuplicateLineShift = false;
	bool bDuplicateLineAlt = false;
	int iDuplicateLineKey = 68;
	char cDuplicateLineText[32];

	bool bSceneLowFloatPrecision = true;
	int iSceneLowFloatPrecision = 2;
	char releasefillertoByteAlign; // MUST STAY 0.49.
	
	unsigned int trialversion = 1234567; //Changed for use as first time trial and current trial version number.
	//Relased v0.5 with no pref changes, just keep this for next version.

	int media_icon_size = 64;
	bool save_layout = true;

	unsigned int trialdate = 0; // changed to trial date for current trial version.

	bool bEnableToolbarExtraLargeIcons = false;

	char cCustomEditorFont[MAX_PATH];

	unsigned int neverendwithabool70 = 0; // for version 0.70

	int iInstallUpdate = 0;
	int cInstallVersion = 0;
	char cInstallProjectFolder[MAX_PATH];
	char cInstallLibsFolder[MAX_PATH];
	int editorPalette = 0;

	TextEditor::Palette EditorColors = { {
			0xffcccccc,	// None
			0xffd79c56,	// Keyword, do loop while for next 	0xffb2c856
			0xff00ff00,	// Number
			0xff4778ff,	// String //0xff7070e0
			0xff70a0e0, // Char literal
			0xffffffff, // Punctuation
			0xff409090,	// Preprocessor
			0xffcccccc, // Identifier
			0xffb2c856, // Known identifier, agk commands
			0xffc040a0, // Preproc identifier
			0xff4aaa56, // Comment (single line) 0xff206020
			0xff4aaa56, // Comment (multi line) 0xff406020
			0xff181818, // Background
			0xffe0e0e0, // Cursor
			0x80808080, // Selection 0x80a06020.
			0x800020ff, // ErrorMarker
			0x40f08000, // Breakpoint
			0xff808080, // Line number
			0x70b0b0b0, // last keyword mark.
			0x40808080, // Current line fill (inactive)
			0x40a0a0a0, // Current line edge
			0xff1e1e1e, // Background Line
		} };

	int idePalette = 0;
	int iHoverSyntaxHelp = 0;
	int iTabHideDropdown = 0;
	int iCancelQuitDialog = 0;
	int iEnableCodeFolding = 0;
	int iLastFontType = 1;

	int iAndroidExportVersion = 0;
	int iDisableMouseWheelZoom = 0;
	int iRememberTabOrder = 0;

	char cJDKPath[MAX_PATH] = "\0";

	//custom style colors
	float gui_col_main_hue = 140.f / 255.f;
	float gui_col_main_sat = 196.f / 255.f;
	float gui_col_main_val = 142.f / 255.f;
	float gui_col_area_hue = 140.f / 255.f;
	float gui_col_area_sat = 41.f / 255.f; 
	float gui_col_area_val = 81.f / 255.f; 
	float gui_col_back_hue = 136.f / 255.f; 
	float gui_col_back_sat = 22.f / 255.f; 
	float gui_col_back_val = 57.f / 255.f; 
	float gui_col_text_hue = 0.f / 255.f; 
	float gui_col_text_sat = 0.f / 255.f; 
	float gui_col_text_val = 255.f / 255.f; 
	float gui_col_scroll_hue = 140.f / 255.f; 
	float gui_col_scroll_sat = 196.f / 255.f; 
	float gui_col_scroll_val = 142.f / 255.f; 

	//F1 browser help
	bool bBrowserHelp = false;

	//appgamekit news
	bool bAppGameKitNews = true;

};

#endif

