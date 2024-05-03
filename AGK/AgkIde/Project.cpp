//PE: project files.
//#define DISABLESAVE

#include "2DScene.h"
#include "project.h"
#ifdef AGK_WINDOWS
#include <direct.h>
#endif
#include <stdio.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "../CompilerNew/AGKCompiler2.h"

#define DISABLEFILESORTING

//#include "../CompilerNew/cFile.h"

cProjectItem * allProjects = NULL;
cProjectItem * nextProject = NULL;
cProjectItem::sProjectFiles *allFiles = NULL;

int bc_project_set = 0;
char bc_data[MAX_PATH];

char * testopenfiledialog(char * path);

int uniqID = 30000;

#ifdef AGK_MACOS
NSFileManager *g_pMacFileManager = 0;
#endif
//functions defination from ide.cpp should be moved.
void setVisibleProjectsInAssetBrowser(void);

// External 
extern int refresh_gui_docking;
extern cProjectItem * pCurrentSelectedProject;
extern cFolderItem * m_pSelectedFolder;
extern cProjectItem * pRepeatRemoveProject;
extern TextEditor * m_ActiveEditor;
extern TextEditor * m_CompiledEditor;
extern cProjectItem * mCompiledProject;
#ifdef AGK_WINDOWS
extern HWND g_agkhWnd;
HWND debug_hwnd;
#endif
#ifdef AGK_MACOS
#include "../../../platform/mac/GLFW/glfw3.h"
extern GLFWwindow* g_agkhWnd;
#endif

extern bool bRepeatRemoveProjectAll;
extern char * cNextWindowFocus;
extern bool bFocusActiveEditor;
extern bool compiler_done;
extern bool create_new_project;
extern int game_is_running;
extern int debug_is_running;
extern int remote_debug_is_running;
extern int broadcast_is_running;
extern bool bRemote_broadcast_is_running;
extern bool broadcaster_initialized;
extern void isProjectInAssetsBrowser(void);
extern char startupFolder[MAX_PATH];
extern int iTotalFolders;
extern char appFolder[MAX_PATH];
extern char customFolders[10][MAX_PATH];
extern int iCustomFolders;
extern cFolderItem *m_pStartFolder;
extern int iDeviceWidth, iDeviceHeight;
extern bool agkfullscreen;
extern char defaultWriteFolder[MAX_PATH];
extern uString szRecentProjects[MAXRECENTLIST];
extern uString szRecentFiles[MAXRECENTLIST];
extern int iResentProjects, iResentFiles;
extern Preferences pref;



void ResetPreferences(bool resetall)
{
	//Preferences defaults.
	pref.bEnableSymboldList = true;
	pref.show_preferences_window = false;
	pref.editor_display_linenumbers = true;
	pref.bEnableAutoCompletion = true;
	pref.mTabSize = 4;
	pref.iEditorFontSize = 15;
	pref.iIDEFontSize = 15;
	pref.bEnableToolbarLargeIcons = true;
	pref.bEnableToolbarExtraLargeIcons = false;
	pref.bAutoIndentation = true;
	pref.bSmartAutoIndentation = true;
	pref.bWindows64Bit = true;

	strcpy(pref.cPathToCompiler, "");
	strcpy(pref.cBroadcastPort, "");
	strcpy(pref.cBroadcastIp, "");
	pref.bBroadcastUseSteam = false;
	strcpy(pref.cDebugIp, "");
	pref.bAutoHideDebugWindows = false;

	pref.bDisplayActiveProjectFilesOnly = true;

	if (resetall) {
		strcpy(pref.keystore_full_name, "");
		strcpy(pref.keystore_company_name, "");
		strcpy(pref.keystore_city_name, "");
		strcpy(pref.keystore_country_code, "");
		strcpy(pref.keystore_file_location, "release.keystore");
	}
	pref.bTimestampExe = false;
	pref.icon_background_color = ImVec4(0, 0, 0, 0);
	pref.bDisplayProjectinAssetsBrowser = false;

	pref.bEnableCustomStyle = false;
	pref.bEnableSeedStyle = false;
	pref.iSeedStyle = 4152469;
	pref.iIDESelectedStyle = 0;

	pref.bLoadClassicDLConStartup = true;
	pref.bLoadStudioDLConStartup = true;

	pref.bRemovePathFromTabName = true;

	pref.bDisplayHelpWindow = true;
	pref.bDisplayMessageWindow = true;
	pref.bDisplayProjectWindow = true;
	pref.bDisplayDebuggerWindow = true;
	pref.bDisplayMediaFilesWindow = true;
	pref.bDisplayPreviewWindow = true;
	pref.bDisplayAssetsBrowserWindow = true;

	pref.iCurrentIconSet = 2;

	pref.bUseInternalMousePointer = false;
	pref.bUseCodeProperties = true;

	pref.bUndoCtrl = true;
	pref.bUndoShift = false;
	pref.bUndoAlt = false;
	pref.iUndoKey = 90; //Z
#ifdef AGK_MACOS
	strcpy(pref.cUndoText, "Cmd+Z");
#else
	strcpy(pref.cUndoText, "Ctrl+Z");
#endif

	pref.bRedoCtrl = true;
	pref.bRedoShift = false;
	pref.bRedoAlt = false;
	pref.iRedoKey = 89; //Y
#ifdef AGK_MACOS
	strcpy(pref.cRedoText, "Cmd+Y");
#else
	strcpy(pref.cRedoText, "Ctrl+Y");
#endif

	pref.bCopyCtrl = true;
	pref.bCopyShift = false;
	pref.bCopyAlt = false;
	pref.iCopyKey = 67; //C
#ifdef AGK_MACOS
	strcpy(pref.cCopyText, "Cmd+C");
#else
	strcpy(pref.cCopyText, "Ctrl+C");
#endif

	pref.bPasteCtrl = true;
	pref.bPasteShift = false;
	pref.bPasteAlt = false;
	pref.iPasteKey = 86; //V
#ifdef AGK_MACOS
	strcpy(pref.cPasteText, "Cmd+V");
#else
	strcpy(pref.cPasteText, "Ctrl+V");
#endif

	pref.bCutCtrl = true;
	pref.bCutShift = false;
	pref.bCutAlt = false;
	pref.iCutKey = 88; //X
#ifdef AGK_MACOS
	strcpy(pref.cCutText, "Cmd+X");
#else
	strcpy(pref.cCutText, "Ctrl+X");
#endif

	pref.bFindCtrl = true;
	pref.bFindShift = false;
	pref.bFindAlt = false;
	pref.iFindKey = 70; //F
#ifdef AGK_MACOS
	strcpy(pref.cFindText, "Cmd+F");
#else
	strcpy(pref.cFindText, "Ctrl+F");
#endif

	pref.bFindNextCtrl = false;
	pref.bFindNextShift = false;
	pref.bFindNextAlt = false;
	pref.iFindNextKey = 114; //F3
#ifdef AGK_MACOS
	strcpy(pref.cFindNextText, "F3");
#else
	strcpy(pref.cFindNextText, "F3");
#endif

	pref.bReplaceCtrl = true;
	pref.bReplaceShift = false;
	pref.bReplaceAlt = false;
#ifdef AGK_MACOS
	pref.iReplaceKey = 82; //R
	strcpy(pref.cReplaceText, "Cmd+R");
#else
	pref.iReplaceKey = 72; //H 
	strcpy(pref.cReplaceText, "Ctrl+H");
#endif

	pref.bZoomInCtrl = true;
	pref.bZoomInShift = false;
	pref.bZoomInAlt = false;
	pref.iZoomInKey = 107; //+
#ifdef AGK_MACOS
	strcpy(pref.cZoomInText, "Cmd++");
#else
	strcpy(pref.cZoomInText, "Ctrl++");
#endif

	pref.bZoomOutCtrl = true;
	pref.bZoomOutShift = false;
	pref.bZoomOutAlt = false;
	pref.iZoomOutKey = 109; //-
#ifdef AGK_MACOS
	strcpy(pref.cZoomOutText, "Cmd+-");
#else
	strcpy(pref.cZoomOutText, "Ctrl+-");
#endif

	pref.bGotoLineCtrl = true;
	pref.bGotoLineShift = false;
	pref.bGotoLineAlt = false;
	pref.iGotoLineKey = 76; //L
#ifdef AGK_MACOS
	strcpy(pref.cGotoLineText, "Cmd+L");
#else
	strcpy(pref.cGotoLineText, "Ctrl+L");
#endif


	pref.bRunCtrl = false;
	pref.bRunShift = false;
	pref.bRunAlt = false;
	pref.iRunKey = 116; // F5
	strcpy(pref.cRunText, "F5");

	pref.bCompileCtrl = false;
	pref.bCompileShift = false;
	pref.bCompileAlt = false;
	pref.iCompileKey = 118; // F7
	strcpy(pref.cCompileText, "F7");

	pref.bBroadCastCtrl = false;
	pref.bBroadCastShift = false;
	pref.bBroadCastAlt = false;
	pref.iBroadCastKey = 117; // F6
	strcpy(pref.cBroadCastText, "F6");

	pref.bDebugCtrl = false;
	pref.bDebugShift = false;
	pref.bDebugAlt = false;
	pref.iDebugKey = 119; // F8
	strcpy(pref.cDebugText, "F8");

	pref.bOpenFileCtrl = true;
	pref.bOpenFileShift = false;
	pref.bOpenFileAlt = false;
	pref.iOpenFileKey = 79; //O
#ifdef AGK_MACOS
	strcpy(pref.cOpenFileText, "Cmd+O");
#else
	strcpy(pref.cOpenFileText, "Ctrl+O");
#endif

	pref.bOpenProjectCtrl = true;
	pref.bOpenProjectShift = false;
	pref.bOpenProjectAlt = false;
	pref.iOpenProjectKey = 80; //P
#ifdef AGK_MACOS
	strcpy(pref.cOpenProjectText, "Cmd+P");
#else
	strcpy(pref.cOpenProjectText, "Ctrl+P");
#endif


	pref.bDebugStepCtrl = false;
	pref.bDebugStepShift = false;
	pref.bDebugStepAlt = false;
	pref.iDebugStepKey = 121; // F10
	strcpy(pref.cDebugStepText, "F10");

	pref.bDebugBringToFront = false;
	pref.iIDEUpdateFPS = 2;
	pref.bDebugBringAppToFront = false;

#ifdef AGK_MACOS
	pref.bUseUpscaling = true;
#else
	pref.bUseUpscaling = false;
#endif


	pref.bGoToDefinitionCtrl = true;
	pref.bGoToDefinitionShift = false;
	pref.bGoToDefinitionAlt = false;
	pref.iGoToDefinitionKey = 84; //T
#ifdef AGK_MACOS
	strcpy(pref.cGoToDefinitionText, "Cmd+T");
#else
	strcpy(pref.cGoToDefinitionText, "Ctrl+T");
#endif

	pref.iIDEUpdateEventSleep = true;

	pref.bDisplaySceneManager = true;
	pref.bDisplaySceneProperties = true;

	strcpy(pref.cDefaultProjectFolder, "");

	pref.bDuplicateLineCtrl = true;
	pref.bDuplicateLineShift = false;
	pref.bDuplicateLineAlt = false;
	pref.iDuplicateLineKey = 68; //D
#ifdef AGK_MACOS
	strcpy(pref.cDuplicateLineText, "Cmd+D");
#else
	strcpy(pref.cDuplicateLineText, "Ctrl+D");
#endif

	pref.bSceneLowFloatPrecision = true;
	pref.iSceneLowFloatPrecision = 2;

	pref.media_icon_size = 64;
	pref.save_layout = true;
	strcpy(pref.cCustomEditorFont, "");

	pref.iInstallUpdate = 0;
	pref.cInstallVersion = 0; //No default.
	strcpy(pref.cInstallProjectFolder, "");
	strcpy(pref.cInstallLibsFolder, "");
	pref.editorPalette = 0;
	pref.idePalette = 0;
	pref.iHoverSyntaxHelp = 0;
	pref.iTabHideDropdown = 0;
	pref.iCancelQuitDialog = 0;
	pref.iEnableCodeFolding = 0; //PE: Code Folding Beta disable code folding by default.

	pref.iAndroidExportVersion = 0; // will redownload android export files
	pref.iDisableMouseWheelZoom = 0;
	pref.iRememberTabOrder = 0;
	pref.iLastFontType = 1;

	strcpy(pref.cJDKPath, "");

	//HDPI default preference settings.
	int displaywidth = agk::GetMaxDeviceWidth();
	if (displaywidth > 3500) {
		//4K
		pref.iEditorFontSize = 24;
		pref.iIDEFontSize = 24;
		pref.bEnableToolbarLargeIcons = false;
		pref.bEnableToolbarExtraLargeIcons = true;
	}
	else if (displaywidth > 2500) {
		//Surface pro 2736 x 1824
		pref.iEditorFontSize = 22;
		pref.iIDEFontSize = 22;
		pref.bEnableToolbarLargeIcons = false;
		pref.bEnableToolbarExtraLargeIcons = true;
	}
}


void CheckKeyboardShortcuts(void)
{
	extern const char* key_values[];
	uString ktext;

	int key = pref.iUndoKey;
	if (!pref.bUndoCtrl && !pref.bUndoShift && !pref.bUndoAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223)) ) {
		pref.bUndoCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cUndoText, "Cmd+");
		strcat(pref.cUndoText, ktext.GetStr() );
#else
		strcpy(pref.cUndoText, "Ctrl+");
		strcat(pref.cUndoText, ktext.GetStr() );
#endif

	}
	key = pref.iRedoKey;
	if (!pref.bRedoCtrl && !pref.bRedoShift && !pref.bRedoAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bRedoCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cRedoText, "Cmd+");
		strcat(pref.cRedoText, ktext.GetStr());
#else
		strcpy(pref.cRedoText, "Ctrl+");
		strcat(pref.cRedoText, ktext.GetStr());
#endif
	}

	key = pref.iCopyKey;
	if (!pref.bCopyCtrl && !pref.bCopyShift && !pref.bCopyAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bCopyCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cCopyText, "Cmd+");
		strcat(pref.cCopyText, ktext.GetStr());
#else
		strcpy(pref.cCopyText, "Ctrl+");
		strcat(pref.cCopyText, ktext.GetStr());
#endif

	}

	key = pref.iPasteKey;
	if (!pref.bPasteCtrl && !pref.bPasteShift && !pref.bPasteAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bPasteCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cPasteText, "Cmd+");
		strcat(pref.cPasteText, ktext.GetStr());
#else
		strcpy(pref.cPasteText, "Ctrl+");
		strcat(pref.cPasteText, ktext.GetStr());
#endif

	}

	key = pref.iCutKey;
	if (!pref.bCutCtrl && !pref.bCutShift && !pref.bCutAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bCutCtrl = true;

		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cCutText, "Cmd+");
		strcat(pref.cCutText, ktext.GetStr());
#else
		strcpy(pref.cCutText, "Ctrl+");
		strcat(pref.cCutText, ktext.GetStr());
#endif

	}

	key = pref.iFindKey;
	if (!pref.bFindCtrl && !pref.bFindShift && !pref.bFindAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bFindCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cFindText, "Cmd+");
		strcat(pref.cFindText, ktext.GetStr());
#else
		strcpy(pref.cFindText, "Ctrl+");
		strcat(pref.cFindText, ktext.GetStr());
#endif

	}

	key = pref.iReplaceKey;
	if (!pref.bReplaceCtrl && !pref.bReplaceShift && !pref.bReplaceAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bReplaceCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cReplaceText, "Cmd+");
		strcat(pref.cReplaceText, ktext.GetStr());
#else
		strcpy(pref.cReplaceText, "Ctrl+");
		strcat(pref.cReplaceText, ktext.GetStr());
#endif

	}

	key = pref.iZoomInKey;
	if (!pref.bZoomInCtrl && !pref.bZoomInShift && !pref.bZoomInAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bZoomInCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cZoomInText, "Cmd+");
		strcat(pref.cZoomInText, ktext.GetStr());
#else
		strcpy(pref.cZoomInText, "Ctrl+");
		strcat(pref.cZoomInText, ktext.GetStr());
#endif

	}
	key = pref.iZoomOutKey;
	if (!pref.bZoomOutCtrl && !pref.bZoomOutShift && !pref.bZoomOutAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bZoomOutCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cZoomOutText, "Cmd+");
		strcat(pref.cZoomOutText, ktext.GetStr());
#else
		strcpy(pref.cZoomOutText, "Ctrl+");
		strcat(pref.cZoomOutText, ktext.GetStr());
#endif

	
	}

	key = pref.iGotoLineKey;
	if (!pref.bGotoLineCtrl && !pref.bGotoLineShift && !pref.bGotoLineAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bGotoLineCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cGotoLineText, "Cmd+");
		strcat(pref.cGotoLineText, ktext.GetStr());
#else
		strcpy(pref.cGotoLineText, "Ctrl+");
		strcat(pref.cGotoLineText, ktext.GetStr());
#endif

	}

	key = pref.iOpenFileKey;
	if (!pref.bOpenFileCtrl && !pref.bOpenFileShift && !pref.bOpenFileAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bOpenFileCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cOpenFileText, "Cmd+");
		strcat(pref.cOpenFileText, ktext.GetStr());
#else
		strcpy(pref.cOpenFileText, "Ctrl+");
		strcat(pref.cOpenFileText, ktext.GetStr());
#endif

	}
	key = pref.iOpenProjectKey;
	if (!pref.bOpenProjectCtrl && !pref.bOpenProjectShift && !pref.bOpenProjectAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bOpenProjectCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cOpenProjectText, "Cmd+");
		strcat(pref.cOpenProjectText, ktext.GetStr());
#else
		strcpy(pref.cOpenProjectText, "Ctrl+");
		strcat(pref.cOpenProjectText, ktext.GetStr());
#endif

	}
	key = pref.iGoToDefinitionKey;
	if (!pref.bGoToDefinitionCtrl && !pref.bGoToDefinitionShift && !pref.bGoToDefinitionAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bGoToDefinitionCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cGoToDefinitionText, "Cmd+");
		strcat(pref.cGoToDefinitionText, ktext.GetStr());
#else
		strcpy(pref.cGoToDefinitionText, "Ctrl+");
		strcat(pref.cGoToDefinitionText, ktext.GetStr());
#endif
	}
	key = pref.iDuplicateLineKey;
	if (!pref.bDuplicateLineCtrl && !pref.bDuplicateLineShift && !pref.bDuplicateLineAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bDuplicateLineCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cDuplicateLineText, "Cmd+");
		strcat(pref.cDuplicateLineText, ktext.GetStr());
#else
		strcpy(pref.cDuplicateLineText, "Ctrl+");
		strcat(pref.cDuplicateLineText, ktext.GetStr());
#endif

	}

	key = pref.iFindNextKey;
	if (!pref.bFindNextCtrl && !pref.bFindNextShift && !pref.bFindNextAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bFindNextCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cFindNextText, "Cmd+");
		strcat(pref.cFindNextText, ktext.GetStr());
#else
		strcpy(pref.cFindNextText, "Ctrl+");
		strcat(pref.cFindNextText, ktext.GetStr());
#endif

	}

	key = pref.iRunKey;
	if (!pref.bRunCtrl && !pref.bRunShift && !pref.bRunAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bRunCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cRunText, "Cmd+");
		strcat(pref.cRunText, ktext.GetStr());
#else
		strcpy(pref.cRunText, "Ctrl+");
		strcat(pref.cRunText, ktext.GetStr());
#endif

	}

	key = pref.iCompileKey;
	if (!pref.bCompileCtrl && !pref.bCompileShift && !pref.bCompileAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bCompileCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cCompileText, "Cmd+");
		strcat(pref.cCompileText, ktext.GetStr());
#else
		strcpy(pref.cCompileText, "Ctrl+");
		strcat(pref.cCompileText, ktext.GetStr());
#endif

	}


	key = pref.iBroadCastKey;
	if (!pref.bBroadCastCtrl && !pref.bBroadCastShift && !pref.bBroadCastAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bBroadCastCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cBroadCastText, "Cmd+");
		strcat(pref.cBroadCastText, ktext.GetStr());
#else
		strcpy(pref.cBroadCastText, "Ctrl+");
		strcat(pref.cBroadCastText, ktext.GetStr());
#endif

	}

	key = pref.iDebugKey;
	if (!pref.bDebugCtrl && !pref.bDebugShift && !pref.bDebugAlt && ((key >= 48 && key <= 111) || (key >= 186 && key <= 223))) {
		pref.bDebugCtrl = true;
		ktext = key_values[key];
		ktext.ReplaceStr("KEY_", "");

#ifdef AGK_MACOS
		strcpy(pref.cDebugText, "Cmd+");
		strcat(pref.cDebugText, ktext.GetStr());
#else
		strcpy(pref.cDebugText, "Ctrl+");
		strcat(pref.cDebugText, ktext.GetStr());
#endif

	}

	return;
}

extern bool add_new_scene;
extern bool add_new_project_file;

void CreateNewScene(void)
{
	static char scenename[MAX_PATH];
	static char cnc_error[1024];
	static bool create_new_scene_first_run = true;

#ifdef AGK_WINDOWS
	_chdir(startupFolder);
#else
	chdir(startupFolder);
#endif

	if (add_new_scene)
	{
		if (create_new_scene_first_run) {
			create_new_scene_first_run = false;
			strcpy(scenename, "");
			strcpy(cnc_error, "");

			//PE: somehow Rick got it set really small ? or off sceeen (GetMainViewport()->Pos) ? so. let imgui ImGuiWindowFlags_AlwaysAutoResize do it.
//			ImGui::SetNextWindowPos(ImVec2(100, 100) + ImGui::GetMainViewport()->Pos, ImGuiSetCond_Once);
//			ImGui::SetNextWindowSize(ImVec2(31 * ImGui::GetFontSize(), 8 * ImGui::GetFontSize()));
		}
	}


	if (add_new_scene)
	{
		ImGui::SetNextWindowPosCenter(ImGuiCond_Always); //PE: Center modal windows.
		ImGui::SetNextWindowSize(ImVec2(31 * ImGui::GetFontSize(), 8 * ImGui::GetFontSize()));

		ImGui::OpenPopup("Create New Scene.");
		if (ImGui::BeginPopupModal("Create New Scene.", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			float fIndent = 7 * ImGui::GetFontSize();

			ImGui::Text("Scene name:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(fIndent, ImGui::GetCursorPos().y));
			bool enterpressed = false;
			if (ImGui::InputText("      ", &scenename[0], MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue)) {
				enterpressed = true;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8, 0.2, 0.2, 1.0));
			ImGui::Text("%s", cnc_error);
			ImGui::PopStyleColor();

			if (ImGui::Button(" Cancel ")) {
				add_new_scene = false;
			}

			ImGui::SameLine();

			if (ImGui::Button(" Create Scene ") || enterpressed ) {
				//Create scene.
				//Validate parameters.
				if (pCurrentSelectedProject) {
					if (strlen(scenename) > 0) {
						uString newName;

						//Remove any file related stuff in project name.
						newName = scenename;
						newName.ReplaceStr(":", "");
						newName.ReplaceStr("\\", "");
						newName.ReplaceStr("/", "");
						newName.ReplaceStr("&", "");
						int pos = newName.FindStr(".scene", 1);
						if (pos <= 0) {
							newName.Append(".scene");
						}

						uString pfolder = "";
						uString projectpath = pCurrentSelectedProject->m_sProjectFileFullPath;
						projectpath.ReplaceStr("\\", "/");
						pos = projectpath.RevFindStr("/");
						if (pos > 0) {
							projectpath.SubString(pfolder, 0, pos + 1);
							uString ScenePath = pfolder;
							ScenePath.Append(newName);
							//Check if already exists.

							bool alreadyexist = false;
							cProjectItem::sProjectFiles *m_pSearchFile;
							m_pSearchFile = pCurrentSelectedProject->m_pFirstFile;
							if (m_pSearchFile) {
								while (m_pSearchFile) {
									if (newName.CompareCaseTo(m_pSearchFile->m_sName) == 0) {
										//File is already in project.
										alreadyexist = true;
										break;
									}
									m_pSearchFile = m_pSearchFile->m_pNext;
								}
							}
							if (!alreadyexist) {
								//Create as a text file.
								//ScenePath
								FILE* dest = fopen(ScenePath.GetStr(), "wb+");
								if (!dest)
									dest = AGKfopen(ScenePath.GetStr(), "wb+");

								if (dest) {
									fputs("//Scene file do not make any changes here.\n", dest);
									fclose(dest);

									//Find last entry.
									m_pSearchFile = pCurrentSelectedProject->m_pFirstFile;
									if (m_pSearchFile->m_pNext) {
										while (m_pSearchFile->m_pNext) {
											m_pSearchFile = m_pSearchFile->m_pNext;
										}
									}

									//Now create and open as text file.
									if (m_pSearchFile) {
										m_pSearchFile->m_pNext = new cProjectItem::sProjectFiles();
										m_pSearchFile = m_pSearchFile->m_pNext;
										m_pSearchFile->m_sName = newName;
										m_pSearchFile->m_sFullPath = ScenePath;
										m_pSearchFile->m_bUseSaveAs = false;
										m_pSearchFile->m_id = ++uniqID;
										m_pSearchFile->m_editor = new TextEditor();
										m_pSearchFile->m_scene = new MediaScene();
										m_pSearchFile->m_scene->m_sSceneFullPath = ScenePath;
										m_pSearchFile->m_editor->quiteditor = false;
										m_pSearchFile->m_bOpenForEdit = false;
										m_pSearchFile->m_sProjectName = pCurrentSelectedProject->m_sProjectFile;
										m_pSearchFile->m_sEditName = newName;
										if (pref.bRemovePathFromTabName) {
											uString newTabname = m_pSearchFile->m_sEditName;
											int ntb_pos = newTabname.RevFindStr("/");
											if (ntb_pos <= 0)
												ntb_pos = newTabname.RevFindStr("\\");
											if (ntb_pos > 0)
												newTabname.SubString(m_pSearchFile->m_sEditName, ntb_pos + 1);
										}

										m_pSearchFile->m_sEditName.Append("##");
										m_pSearchFile->m_sEditName.Append(pCurrentSelectedProject->m_sProjectFile);
										m_pSearchFile->m_pNext = NULL;

										//Open file in editor.
										vTextEditorInit(m_pSearchFile->m_editor, (char *)m_pSearchFile->m_sFullPath.GetStr());
										m_pSearchFile->m_bOpenForEdit = true;
										cNextWindowFocus = (char *)m_pSearchFile->m_sEditName.GetStr();
										bFocusActiveEditor = true;
										m_ActiveEditor = m_pSearchFile->m_editor;
										ImGui::SetWindowFocus(m_pSearchFile->m_sEditName.GetStr());

										//Save new project settings.
										SaveProjectFile(pCurrentSelectedProject);
										//Done close window.
										add_new_scene = false;
									}

								}
								else sprintf(cnc_error, "Cant create file: %s.", ScenePath.GetStr());
							}
							else sprintf(cnc_error, "Scene file is already in project.");
						}
						else sprintf(cnc_error, "Cant find active project path.");
					}
					else sprintf(cnc_error, "Please select a project name.");
				}
				else sprintf(cnc_error, "Cant find active project.");
			}

			ImGui::EndPopup();
		}

	}
}


void CreateNewProjectFile(void)
{
	static char newfilename[MAX_PATH];
	static char cnc_error[1024];
	static bool create_new_project_file_first_run = true;

#ifdef AGK_WINDOWS
	_chdir(startupFolder);
#else
	chdir(startupFolder);
#endif

	if (add_new_project_file)
	{
		if (create_new_project_file_first_run) {
			create_new_project_file_first_run = false;
			strcpy(newfilename, "");
			strcpy(cnc_error, "");
			//ImGui::SetNextWindowPos(ImVec2(100, 100) + ImGui::GetMainViewport()->Pos, ImGuiSetCond_Once);
			//ImGui::SetNextWindowSize(ImVec2(31 * ImGui::GetFontSize(), 8 * ImGui::GetFontSize())); //PE: Always set window size.
		}
	}

	if (add_new_project_file)
	{
		ImGui::SetNextWindowPosCenter(ImGuiCond_Always); //PE: Center modal windows.
		ImGui::SetNextWindowSize(ImVec2(31 * ImGui::GetFontSize(), 8 * ImGui::GetFontSize())); //PE: Always set window size.

		ImGui::OpenPopup("Create New Project Source File.");
		if (ImGui::BeginPopupModal("Create New Project Source File.", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			float fIndent = 7 * ImGui::GetFontSize();

			ImGui::Text("File name:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(fIndent, ImGui::GetCursorPos().y));
			bool enterpressed = false;
			if (ImGui::InputText("      ", &newfilename[0], MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue)) {
				enterpressed = true;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8, 0.2, 0.2, 1.0));
			ImGui::Text("%s", cnc_error);
			ImGui::PopStyleColor();

			if (ImGui::Button(" Cancel ")) {
				add_new_project_file = false;
			}

			ImGui::SameLine();

			if (ImGui::Button(" Create New Project Source File ") || enterpressed) {
				//Create scene.
				//Validate parameters.
				if (pCurrentSelectedProject) {
					if (strlen(newfilename) > 0) {
						uString newName;

						//Remove any file related stuff in project name.
						newName = newfilename;
						newName.ReplaceStr(":", "");
						newName.ReplaceStr("\\", "/"); // convert.
						//newName.ReplaceStr("/", "");
						newName.ReplaceStr("&", "");
						int pos = newName.FindStr(".agc", 1);
						if (pos <= 0) {
							newName.Append(".agc");
						}

						uString pfolder = "";
						uString projectpath = pCurrentSelectedProject->m_sProjectFileFullPath;
						projectpath.ReplaceStr("\\", "/");
						pos = projectpath.RevFindStr("/");
						if (pos > 0) {
							projectpath.SubString(pfolder, 0, pos + 1);
							uString ScenePath = pfolder;
							ScenePath.Append(newName);
							//Check if already exists.

							bool alreadyexist = false;
							cProjectItem::sProjectFiles *m_pSearchFile;
							m_pSearchFile = pCurrentSelectedProject->m_pFirstFile;
							if (m_pSearchFile) {
								while (m_pSearchFile) {
									if (newName.CompareCaseTo(m_pSearchFile->m_sName) == 0) {
										//File is already in project.
										alreadyexist = true;
										break;
									}
									m_pSearchFile = m_pSearchFile->m_pNext;
								}
							}

							bool oksave = true;
							if (!alreadyexist) {
								if (file_exists((char *)ScenePath.GetStr())) {
									//Ask overwrite.
									oksave = overWriteFileBox((char*)ScenePath.GetStr());

								}
							}

							if (!alreadyexist && oksave ) {
								//Create as a text file.
								//ScenePath
								FILE* dest = fopen(ScenePath.GetStr(), "wb+");
								if (!dest)
									dest = AGKfopen(ScenePath.GetStr(), "wb+");

								if (dest) {
									// File: deleteme30 
									// Created: 19-05-31
									char str[MAX_PATH], date[80];
									time_t curtime;
									time(&curtime);
									strftime(date, 80, "%y-%m-%d", localtime(&curtime)); // %H:%M:%S
									sprintf(str, "// File: %s\n", newName.GetStr());
									fputs(str, dest);
									sprintf(str, "// Created: %s\n", date );
									fputs(str, dest);
									fclose(dest);

									//Find last entry.
									m_pSearchFile = pCurrentSelectedProject->m_pFirstFile;
									if (m_pSearchFile->m_pNext) {
										while (m_pSearchFile->m_pNext) {
											m_pSearchFile = m_pSearchFile->m_pNext;
										}
									}

									//Now create and open as text file.
									if (m_pSearchFile) {
										m_pSearchFile->m_pNext = new cProjectItem::sProjectFiles();
										m_pSearchFile = m_pSearchFile->m_pNext;
										m_pSearchFile->m_sName = newName;
										m_pSearchFile->m_sFullPath = ScenePath;
										m_pSearchFile->m_bUseSaveAs = false;
										m_pSearchFile->m_id = ++uniqID;
										m_pSearchFile->m_editor = new TextEditor();
										m_pSearchFile->m_scene = NULL;
										//m_pSearchFile->m_scene->m_sSceneFullPath = ScenePath;
										m_pSearchFile->m_editor->quiteditor = false;
										m_pSearchFile->m_bOpenForEdit = false;
										m_pSearchFile->m_sProjectName = pCurrentSelectedProject->m_sProjectFile;
										m_pSearchFile->m_sEditName = newName;
										if (pref.bRemovePathFromTabName) {
											uString newTabname = m_pSearchFile->m_sEditName;
											int ntb_pos = newTabname.RevFindStr("/");
											if (ntb_pos <= 0)
												ntb_pos = newTabname.RevFindStr("\\");
											if (ntb_pos > 0)
												newTabname.SubString(m_pSearchFile->m_sEditName, ntb_pos + 1);
										}

										m_pSearchFile->m_sEditName.Append("##");
										m_pSearchFile->m_sEditName.Append(pCurrentSelectedProject->m_sProjectFile);
										m_pSearchFile->m_pNext = NULL;

										//Open file in editor.
										vTextEditorInit(m_pSearchFile->m_editor, (char *)m_pSearchFile->m_sFullPath.GetStr());
										m_pSearchFile->m_bOpenForEdit = true;
										cNextWindowFocus = (char *)m_pSearchFile->m_sEditName.GetStr();
										bFocusActiveEditor = true;
										m_ActiveEditor = m_pSearchFile->m_editor;
										ImGui::SetWindowFocus(m_pSearchFile->m_sEditName.GetStr());

										//Save new project settings.
										SaveProjectFile(pCurrentSelectedProject);
										//Done close window.
										add_new_project_file = false;
									}

								}
								else sprintf(cnc_error, "Cant create file: %s.", ScenePath.GetStr());
							}
							else sprintf(cnc_error, "File already exists.");
						}
						else sprintf(cnc_error, "Cant find active project path.");
					}
					else sprintf(cnc_error, "Please select a new file name.");
				}
				else sprintf(cnc_error, "Cant find active project.");
			}

			ImGui::EndPopup();
		}

	}
}



char projectfolder[MAX_PATH];

void CreateNewProject( void )
{
	static char projectname[MAX_PATH];
	static char cnp_error[1024];
	static bool create_new_project_first_run = true;

#ifdef AGK_WINDOWS
	_chdir(startupFolder);
#else
	chdir(startupFolder);
#endif

	if (create_new_project)
	{
		if (create_new_project_first_run) {
			create_new_project_first_run = false;
			strcpy(projectname, "");
			strcpy(projectfolder, "");
			if( strlen(pref.cDefaultProjectFolder) > 1 )
			strcpy(projectfolder, pref.cDefaultProjectFolder);
			strcpy(cnp_error, "");
			//ImGui::SetNextWindowPos(ImVec2(100, 100) + ImGui::GetMainViewport()->Pos, ImGuiSetCond_Once);
			//ImGui::SetNextWindowSize(ImVec2(33 * ImGui::GetFontSize(), 10 * ImGui::GetFontSize())); //PE: Always set window size.
		}
	}

	if (create_new_project)
	{
		ImGui::SetNextWindowPosCenter(ImGuiCond_Always); //PE: Center modal windows.
		ImGui::SetNextWindowSize(ImVec2(33 * ImGui::GetFontSize(), 10 * ImGui::GetFontSize())); //PE: Always set window size.

		ImGui::OpenPopup("Create New Project.");
		if (ImGui::BeginPopupModal("Create New Project.", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			float fIndent = 7 * ImGui::GetFontSize();
			ImGui::Text("Project name:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(fIndent, ImGui::GetCursorPos().y));
			if (ImGui::InputText("      ", &projectname[0], MAX_PATH, 0)) { //ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue
			
				if (strlen(pref.cDefaultProjectFolder) > 1) {
					if (strncmp(pref.cDefaultProjectFolder, projectfolder, strlen(pref.cDefaultProjectFolder)) == 0) {

						strcpy(projectfolder, pref.cDefaultProjectFolder);
						if (projectfolder[strlen(projectfolder) - 1] == '\\') projectfolder[strlen(projectfolder) - 1] = 0;
						if (projectfolder[strlen(projectfolder) - 1] == '/') projectfolder[strlen(projectfolder) - 1] = 0;
						strcat(projectfolder, "/");

						uString newName = projectname;
						newName.ReplaceStr(":", "");
						newName.ReplaceStr("\\", "");
						newName.ReplaceStr("/", "");
						newName.ReplaceStr("&", "");
						strcpy(projectname, newName.GetStr());
						strcat(projectfolder, projectname);
					}
				}
			}
			ImGui::Text("Folder:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(fIndent, ImGui::GetCursorPos().y));
			ImGui::InputText("  ", &projectfolder[0], MAX_PATH, 0); //ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue
			ImGui::SameLine();
			if (ImGui::Button("...")) {
				char *selectedfolder;
//				selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_DIR, "agk\0*.agk\0", NULL, NULL);
				//Mac test. or will always include .agk
				selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_DIR, NULL , NULL, NULL);
				if (selectedfolder) {
					if (strlen(selectedfolder) > 0) {
						strcpy(projectfolder, selectedfolder);
					}
				}
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8,0.2,0.2,1.0));
			ImGui::Text("%s", cnp_error);
			ImGui::PopStyleColor();

			if (ImGui::Button(" Cancel ")) {
				create_new_project = false;
				strcpy(projectname, "");
				strcpy(projectfolder, "");
			}
			
			ImGui::SameLine();

			if (ImGui::Button(" Create Project ")) {
				//Create project.
				//Validate parameters.
				if (strlen(projectname) > 0) {
					uString newName;

					//Remove any file related stuff in project name.
					newName = projectname;
					newName.ReplaceStr(":", "");
					newName.ReplaceStr("\\", "");
					newName.ReplaceStr("/", "");
					newName.ReplaceStr("&", "");

					if (strlen(projectfolder) > 0) {
						uString newFolder;
						if (projectfolder[strlen(projectfolder)-1] == '\\') projectfolder[strlen(projectfolder)-1] = 0;
						if (projectfolder[strlen(projectfolder)-1] == '/') projectfolder[strlen(projectfolder)-1] = 0;

						//Create full path.
						char fullpath[MAX_PATH];

						int retval = cp_realpath(projectfolder, fullpath,true);
						if (retval > 0) {
							strcpy(projectfolder, fullpath);
							if (projectfolder[strlen(projectfolder) - 1] == '\\') projectfolder[strlen(projectfolder) - 1] = 0;
							if (projectfolder[strlen(projectfolder) - 1] == '/') projectfolder[strlen(projectfolder) - 1] = 0;

//							newFolder = projectfolder;
//							int pos = newFolder.RevFindStr(".agk", 1);
//							if (pos > 0) {
//								projectfolder[pos] = 0;
//							}
							//remove .agk

						}

						struct stat sb;
						if( stat(projectfolder, &sb) != 0)
						{
							//Folder do not exist. create.
#ifdef AGK_WINDOWS
							mkdir(projectfolder);
#else
							mkdir(projectfolder, S_IRWXU);
#endif
						}
						//Folder ready , continue.

						//Check if there is already a project there.
						newFolder = projectfolder;
						newFolder.Append("/");
						newFolder.Append(newName);
						newFolder.Append(".agk");

						if( !file_exists((char*) newFolder.GetStr()) )
						{
							//Check if main.agc already exists.
							newFolder = projectfolder;
							newFolder.Append("/main.agc");
							if (!file_exists((char*) newFolder.GetStr()) )
							{
								//Everything ok create final project.
								newFolder = projectfolder;
								newFolder.Append("/");
								newFolder.Append(newName);
								newFolder.Append(".agk");
								cp_copyfile((char*) "media/newproject.agk", (char*)newFolder.GetStr());

								newFolder = projectfolder;
								newFolder.Append("/media");
#ifdef AGK_WINDOWS
								mkdir(newFolder.GetStr());

#else
								mkdir(newFolder.GetStr(), S_IRWXU);
#endif

								newFolder = projectfolder;
								newFolder.Append("/main.agc");
								CreateNewProjectMainFile((char*)newFolder.GetStr(), (char*)newName.GetStr());

								//Now open project.
								newFolder = projectfolder;
								newFolder.Append("/");
								newFolder.Append(newName);
								newFolder.Append(".agk");
								LoadProjectFile((char *)newFolder.GetStr());
								isProjectInAssetsBrowser();

								//Get ready to create another project :)
								create_new_project = false;
								strcpy(projectname, "");
								strcpy(projectfolder, "");
								//Add new project to recent list.
#ifdef AGK_WINDOWS
								//Just to make it look like the other entrys. (using backslash)
								newFolder.ReplaceStr("/", "\\", -1);
#endif
								AddRecentFiles(newFolder.GetStr(), NULL);

							}
							else sprintf(cnp_error, "Project main.agc already exists ?");
						}
						else sprintf(cnp_error, "Project already exists ?");
					}
					else sprintf(cnp_error, "Please select a folder.");
				}
				else sprintf(cnp_error, "Please select a project name.");
			}

			ImGui::EndPopup();
		}

	}
}


cProjectItem::sProjectFiles * CreateNewAGCFile(char *cFileSelected, char *NameIt)
{
	cProjectItem::sProjectFiles * m_pCurrentFile, *nextProject;
	int current_open_files = 0;

	//Find last entry.
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		current_open_files++;
		while (m_pCurrentFile->m_pNext) {
			current_open_files++;
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	uString m_szParse, m_szFileName, m_szPathName;
	bool useSaveAsDefault = false;
	if (cFileSelected) {
		m_szParse = cFileSelected;
		m_szParse.ReplaceStr("\\\\", "//"); //To support drag/drop open source from network drive.
		m_szParse.ReplaceStr("\\", "/");
		int pos = m_szParse.RevFind('/');
		if (pos >= 0) {
			m_szParse.SubString(m_szFileName, pos + 1);
			m_szParse.SubString(m_szPathName, 0, pos + 1);
		}
		else
		{
			m_szPathName = "";
			m_szFileName.Format("Default%d.agc", current_open_files);
			useSaveAsDefault = true;
		}
	}
	else {
		m_szPathName = "";
		m_szFileName.Format("Default%d.agc", current_open_files);
		useSaveAsDefault = true;
	}

	if (cFileSelected) {
		//Already open ?
		uString usSearch;
		usSearch = ""; //"raw:";
		usSearch.Append(m_szPathName);
		usSearch.Append(m_szFileName);

		//Also check open projects using full path.
		cProjectItem * searchProject;
		cProjectItem::sProjectFiles * m_pSearchFile;

		if (allProjects) {
			//Check if project is open.
			searchProject = allProjects;
			while (searchProject) {
				m_pSearchFile = searchProject->m_pFirstFile;
				while (m_pSearchFile) {
					//Check full path.
					uString fullpath = m_pSearchFile->m_sFullPath;
					fullpath.ReplaceStr("\\\\", "//"); //To support drag/drop open source from network drive.
					fullpath.ReplaceStr("\\", "/");
					if (usSearch == fullpath) {
						if (m_pSearchFile->m_editor) {
							//Make sure to make it visible if hidden.
							if (!m_pSearchFile->m_bOpenForEdit) {
								//Not loaded open so we can see error.
								vTextEditorInit(m_pSearchFile->m_editor, (char *)m_pSearchFile->m_sFullPath.GetStr());
								m_pSearchFile->m_bOpenForEdit = true;
							}
							if (!m_pSearchFile->m_editor->bEditorVisible) {
								//Make sure its docked.
								m_pSearchFile->m_editor->firstrun = true;
							}
							m_pSearchFile->m_editor->bEditorVisible = true;

							cNextWindowFocus = (char *)m_pSearchFile->m_sEditName.GetStr();
							bFocusActiveEditor = true;

							if (searchProject != pCurrentSelectedProject ) {
								pCurrentSelectedProject = searchProject;
								//Find m_pSelectedFolder and set.
								if (pCurrentSelectedProject->m_pMediaFolder) {
									m_pSelectedFolder = pCurrentSelectedProject->m_pMediaFolder;
								}
							}

							return NULL;
						}
					}
					m_pSearchFile = m_pSearchFile->m_pNext;
				}
				searchProject = searchProject->m_pNext;
			}
		}

		//Check files outside projects.
		nextProject = allFiles;
		if (nextProject) {
			current_open_files++;
			while (nextProject) {
				if (usSearch.CompareTo(nextProject->m_sFullPath) == 0) {
					cNextWindowFocus = (char *)nextProject->m_sEditName.GetStr();
					return NULL;
				}
				nextProject = nextProject->m_pNext;
			}
		}
	}

	//Create new or add to current chain.
	if (m_pCurrentFile == NULL) {
		nextProject = new cProjectItem::sProjectFiles();
		if(NameIt)
			nextProject->m_sName = NameIt;
		else
			nextProject->m_sName = m_szFileName;
		nextProject->m_sFullPath = ""; //"raw:";
		nextProject->m_sFullPath.Append(m_szPathName);
		nextProject->m_sFullPath.Append(m_szFileName);
		nextProject->m_bUseSaveAs = useSaveAsDefault;
		nextProject->m_editor = new TextEditor();
		nextProject->m_scene = NULL;
		nextProject->m_editor->quiteditor = false;
		nextProject->m_id = ++uniqID;
		nextProject->m_bOpenForEdit = false;
		nextProject->m_sProjectName = "na-noproject";
		nextProject->m_sEditName = nextProject->m_sName;
		nextProject->m_sEditName.Append("##");
		nextProject->m_sEditName.Append(nextProject->m_sProjectName);
		nextProject->m_pNext = NULL;
		allFiles = nextProject;
		return allFiles;
	}
	else {
		if (m_pCurrentFile) {
			m_pCurrentFile->m_pNext = new cProjectItem::sProjectFiles();
			m_pCurrentFile = m_pCurrentFile->m_pNext;
			if (NameIt)
				m_pCurrentFile->m_sName = NameIt;
			else
				m_pCurrentFile->m_sName = m_szFileName;
			m_pCurrentFile->m_sFullPath = ""; //"raw:";
			m_pCurrentFile->m_sFullPath.Append(m_szPathName);
			m_pCurrentFile->m_sFullPath.Append(m_szFileName);
			m_pCurrentFile->m_bUseSaveAs = useSaveAsDefault;
			m_pCurrentFile->m_id = ++uniqID;
			m_pCurrentFile->m_editor = new TextEditor();
			m_pCurrentFile->m_scene = NULL;
			m_pCurrentFile->m_editor->quiteditor = false;
			m_pCurrentFile->m_bOpenForEdit = false;
			m_pCurrentFile->m_sProjectName = "na-noproject";
			m_pCurrentFile->m_sEditName = m_pCurrentFile->m_sName;
			m_pCurrentFile->m_sEditName.Append("##");
			m_pCurrentFile->m_sEditName.Append(m_pCurrentFile->m_sProjectName);
			m_pCurrentFile->m_pNext = NULL;
			return m_pCurrentFile;
		}
	}
	return NULL;
}

void RemoveAGKFile(cProjectItem::sProjectFiles * premoveFile)
{
	cProjectItem::sProjectFiles * m_pCurrentFile, *lastProject;
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile == premoveFile) {
				
				if (m_pCurrentFile == allFiles) {
					lastProject = allFiles->m_pNext;
					if (m_ActiveEditor == allFiles->m_editor)
						m_ActiveEditor = NULL;
					delete allFiles->m_editor;
					delete m_pCurrentFile;
					allFiles = lastProject;
					return;
				}
				if (lastProject) {
					lastProject->m_pNext = m_pCurrentFile->m_pNext;
					if (m_ActiveEditor == m_pCurrentFile->m_editor)
						m_ActiveEditor = NULL;
					if (m_CompiledEditor == m_pCurrentFile->m_editor)
						m_CompiledEditor = NULL;
				
					delete m_pCurrentFile->m_editor;
					delete m_pCurrentFile;
					return;
				}
				
			}
			lastProject = m_pCurrentFile;
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

}
void CreateNewProjectMainFile(char *newfile,char *project)
{
	FILE *fp,*tfp;
	char codetext[MAX_PATH];

	tfp = fopen("media/data/templates/files/main.agc", "r");
	if (!tfp)
		tfp = AGKfopen("media/data/templates/files/main.agc", "r");

	if (tfp) {
		//Found template use it.
		fp = fopen(newfile, "w+");
		if (!fp)
			fp = AGKfopen(newfile, "w+");

		if (fp) {
			char str[2048],date[80];
			time_t curtime;
			time(&curtime);
			strftime(date, 80, "%y-%m-%d", localtime(&curtime)); // %H:%M:%S

			while (fgets(str, 2048, tfp) != NULL) {
				uString convert;
				convert = str;
				convert.ReplaceStr("{date}", date);
				convert.ReplaceStr("{project}", project);
				fputs(convert.GetStr(), fp);
			}
			fclose(fp);
		}
		fclose(tfp);
	}
	else {
		fp = fopen(newfile, "w+");
		if (!fp)
			fp = AGKfopen(newfile, "w+");

		if (fp) {
			sprintf(codetext, "// Project: %s\n", project);
			fputs(codetext, fp);

			time_t curtime;
			time(&curtime);
			strftime(codetext, MAX_PATH, "// Created: %y-%m-%d\n", localtime(&curtime)); // %H:%M:%S
			fputs(codetext, fp);
			fputs("\n", fp);
			fputs("// show all errors\n", fp);
			fputs("SetErrorMode(2)\n", fp);
			fputs("\n", fp);
			fputs("// set window properties\n", fp);
			sprintf(codetext, "SetWindowTitle(\"%s\")\n", project);
			fputs(codetext, fp);
			fputs("SetWindowSize(1024, 768, 0)\n", fp);
			fputs("SetWindowAllowResize(1) // allow the user to resize the window\n", fp);
			fputs("\n", fp);
			fputs("// set display properties\n", fp);
			fputs("SetVirtualResolution(1024, 768) // doesn't have to match the window\n", fp);
			fputs("SetOrientationAllowed(1, 1, 1, 1) // allow both portrait and landscape on mobile devices\n", fp);
			fputs("SetSyncRate(30, 0) // 30fps instead of 60 to save battery\n", fp);
			fputs("SetScissor(0, 0, 0, 0) // no cropping, enable drawing outside the chosen virtual resolution\n", fp);
			fputs("UseNewDefaultFonts(1)\n", fp);
			fputs("\n", fp);
			fputs("\n", fp);
			fputs("do\n", fp);
			fputs("	Print(ScreenFPS())\n", fp);
			fputs("	Sync()\n", fp);
			fputs("loop\n", fp);
			fputs("\n", fp);
			fputs("\n", fp);
			fclose(fp);
		}
	}
	return;
}

void CloseAllOpenProjectEditedFiles(void)
{
	cProjectItem * searchProject;
	cProjectItem::sProjectFiles * m_pSearchFile;

	if (allProjects) {
		//Check if project is open.
		searchProject = allProjects;
		while (searchProject) {
			m_pSearchFile = searchProject->m_pFirstFile;
			while (m_pSearchFile) {
				if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->filechanged && m_pSearchFile->m_editor->bEditorVisible) {

					bool oksave = true;
					oksave = changedFileBox((char*)m_pSearchFile->m_sFullPath.GetStr());

					if (oksave) {
						//Save changes
						auto textToSave = m_pSearchFile->m_editor->GetText();
#ifndef DISABLESAVE
						std::ofstream myfile;
						myfile.open((char*)m_pSearchFile->m_sFullPath.GetStr());
						myfile << textToSave;
						myfile.close();
						m_pSearchFile->m_editor->filechanged = false;
#endif
					}
				}

				if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->bEditorVisible )
					m_pSearchFile->m_editor->quiteditor = true;


				m_pSearchFile = m_pSearchFile->m_pNext;
			}
			searchProject = searchProject->m_pNext;
		}
	}
}

void SaveAllOpenProjectEditedFiles(void)
{
	cProjectItem * searchProject;
	cProjectItem::sProjectFiles * m_pSearchFile;

	if (allProjects) {
		//Check if project is open.
		searchProject = allProjects;
		while (searchProject) {
			m_pSearchFile = searchProject->m_pFirstFile;
			while (m_pSearchFile) {

				if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_scene && m_pSearchFile->m_editor->filechanged && m_pSearchFile->m_editor->bEditorVisible) {
					//Ask save changes ?
					bool oksave = true;
					oksave = changedFileBox((char*)m_pSearchFile->m_sFullPath.GetStr());
					if (oksave) {
						//Save changes
						m_pSearchFile->m_scene->GenerateCode(m_pSearchFile->m_scene, m_pSearchFile->m_editor,true);
					}
				}
				else if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->filechanged && m_pSearchFile->m_editor->bEditorVisible ) {

					bool oksave = true;
					oksave = changedFileBox((char*)m_pSearchFile->m_sFullPath.GetStr());

					if (oksave) {
						//Save changes
						auto textToSave = m_pSearchFile->m_editor->GetText();
#ifndef DISABLESAVE
						std::ofstream myfile;
						myfile.open((char*)m_pSearchFile->m_sFullPath.GetStr());
						myfile << textToSave;
						myfile.close();
						m_pSearchFile->m_editor->filechanged = false;
#endif
					}
				}
				m_pSearchFile = m_pSearchFile->m_pNext;
			}
			searchProject = searchProject->m_pNext;
		}
	}
}

void removeFileFromProject(cProjectItem * mProject, cProjectItem::sProjectFiles * m_pSearchFile)
{
	cProjectItem::sProjectFiles * m_pfindFile;
	//
	if (m_pSearchFile) {
		//Save file if any changes before closing and removing from project.
		if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->filechanged && m_pSearchFile->m_editor->bEditorVisible ) {
			//Save changes
			auto textToSave = m_pSearchFile->m_editor->GetText();
#ifndef DISABLESAVE
			std::ofstream myfile;
			myfile.open((char*)m_pSearchFile->m_sFullPath.GetStr());
			myfile << textToSave;
			myfile.close();
			m_pSearchFile->m_editor->filechanged = false;
#endif
		}
		//Remove close if open.
		m_pSearchFile->m_bOpenForEdit = false;


		//Change pos of window in focus list.
		ImGuiContext& g = *GImGui;
		ImGuiWindow* current = NULL;
		for (int n = 0; n < g.WindowsFocusOrder.Size - 1; n++)
		{
			ImGuiWindow* window = g.WindowsFocusOrder[n];
			if (window->Name && strstr(window->Name, "/TextEditor")) {
				if (strncmp(m_pSearchFile->m_sEditName.GetStr(), window->Name, m_pSearchFile->m_sEditName.GetLength()) == 0) {
					current = window;
					current->Pos.x = -9999;
				}
			}
		}



		//Find file entry.
		m_pfindFile = mProject->m_pFirstFile;
		if (m_pfindFile) {
			if (m_pfindFile == m_pSearchFile) {
				if (m_pfindFile->m_pNext) {
					mProject->m_pFirstFile = m_pfindFile->m_pNext;
					//Delete m_pfindFile
				}
			}
			else {
				if (m_pfindFile->m_pNext) {
					while (m_pfindFile->m_pNext) {
						if (m_pfindFile->m_pNext == m_pSearchFile) {
							m_pfindFile->m_pNext = m_pSearchFile->m_pNext;
							//Delete m_pSearchFile
							break;
						}
						m_pfindFile = m_pfindFile->m_pNext;
					}
				}
			}
		}
		//Removed from list.
	}
}


void SaveAllProjectsEditedFiles(void)
{
	cProjectItem * searchProject;
	cProjectItem::sProjectFiles * m_pSearchFile;

	if (allProjects) {
		//Check if project is open.
		searchProject = allProjects;
		while (searchProject) {
			m_pSearchFile = searchProject->m_pFirstFile;
			while (m_pSearchFile) {

				if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_scene && m_pSearchFile->m_editor->filechanged && m_pSearchFile->m_editor->bEditorVisible) {
					bool oksave = true;
					if (oksave) {
						//Save changes
						m_pSearchFile->m_scene->GenerateCode(m_pSearchFile->m_scene, m_pSearchFile->m_editor, true);
					}
				}
				else if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->filechanged && m_pSearchFile->m_editor->bEditorVisible) {

					bool oksave = true;
					if (oksave) {
						//Save changes
						auto textToSave = m_pSearchFile->m_editor->GetText();
#ifndef DISABLESAVE
						std::ofstream myfile;
						myfile.open((char*)m_pSearchFile->m_sFullPath.GetStr());
						myfile << textToSave;
						myfile.close();
						m_pSearchFile->m_editor->filechanged = false;
#endif
					}
				}
				m_pSearchFile = m_pSearchFile->m_pNext;
			}
			searchProject = searchProject->m_pNext;
		}
	}
}

bool SaveAllEditedProjectFiles(cProjectItem * mProject)
{
	cProjectItem * searchProject;
	cProjectItem::sProjectFiles * m_pSearchFile;

	//Save files outside projects.

	m_pSearchFile = allFiles;
	while (m_pSearchFile) {
		if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->filechanged && m_pSearchFile->m_editor->bEditorVisible ) {
			//Save changes
			if (m_pSearchFile->m_bUseSaveAs) {
				//This file have no Name.
				//when many you do not know what your saving.
				//m_pSearchFile->m_editor->saveonreturn = true;
			}
			else {
				auto textToSave = m_pSearchFile->m_editor->GetText();
#ifndef DISABLESAVE
				std::ofstream myfile;
				myfile.open((char*)m_pSearchFile->m_sFullPath.GetStr());
				myfile << textToSave;
				myfile.close();
				m_pSearchFile->m_editor->filechanged = false;
#endif
			}
		}
		m_pSearchFile = m_pSearchFile->m_pNext;
	}


	//Save ALL open projects edited files. (like classic did).
	SaveAllProjectsEditedFiles();

/*
	//Save files using the current project.
	if (mProject) {
		searchProject = mProject;
		m_pSearchFile = searchProject->m_pFirstFile;
		while (m_pSearchFile) {

			if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_scene && m_pSearchFile->m_editor && m_pSearchFile->m_editor->bEditorVisible ) { // && m_pSearchFile->m_editor->filechanged
				m_pSearchFile->m_scene->GenerateCode(m_pSearchFile->m_scene, m_pSearchFile->m_editor, true);
			}
			else if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->filechanged && m_pSearchFile->m_editor->bEditorVisible ) {
				//Save changes
				auto textToSave = m_pSearchFile->m_editor->GetText();
#ifndef DISABLESAVE
				std::ofstream myfile;
				myfile.open((char*)m_pSearchFile->m_sFullPath.GetStr());
				myfile << textToSave;
				myfile.close();
				m_pSearchFile->m_editor->filechanged = false;
#endif
			}
			m_pSearchFile = m_pSearchFile->m_pNext;
		}
	}
*/
	return true;
}


bool CompileProject(cProjectItem * mProject,bool run,bool broadcast,bool debug, char *useIP)
{
#ifdef AGK_WINDOWS
//	_chdir(startupFolder);
#else
//	chdir(startupFolder);
#endif

//	if (mProject) {

		mCompiledProject = mProject;
		if (!mCompiledProject && m_ActiveEditor)
			m_CompiledEditor = m_ActiveEditor;
		else
			m_CompiledEditor = NULL;

		if (mProject)
			SaveAllEditedProjectFiles(mProject);


		uString m_szParse, m_szFileName, m_szPathName;
		m_szPathName = "";

		if (mProject)
			m_szParse = mProject->m_sProjectFileFullPath;
		else {
//			m_szParse = startupFolder;
//			m_szParse.Append("/media/sandbox/main.agc");
//#ifdef AGK_WINDOWS
//#ifndef AGK_MACOS
			m_szParse = defaultWriteFolder;
			m_szParse.Append("sandbox/main.agc");
//#else
//			m_szParse = startupFolder;
//			m_szParse.Append("/media/sandbox/main.agc");
//#endif

		}

		m_szParse.ReplaceStr("\\\\", "//"); //To allow network drive using //
		m_szParse.ReplaceStr("\\", "/");
		int pos = m_szParse.RevFind('/');
		if (pos >= 0) {
			m_szParse.SubString(m_szFileName, pos + 1);
			m_szParse.SubString(m_szPathName, 0, pos);
		}
		else
		{
			m_szFileName = m_szParse;
			m_szPathName = "";
		}
		
		char curDir[MAX_PATH];

#ifdef AGK_WINDOWS
		// Get the current working directory:
		_getcwd(&curDir[0], MAX_PATH);
		int ret = _chdir(m_szPathName.GetStr());	
#else
		getcwd(&curDir[0], MAX_PATH);
		int ret = chdir(m_szPathName.GetStr());
#endif

		ClearAllEditorMarkers();
		ClearCompilerLog();
		AddToCompilerLog(2, "Compiling %s\n", m_szPathName.GetStr() );

		uString sParameters;
		sParameters.Format( "-stats \"%s\"", m_szPathName.GetStr() );
		AGK_Compiler::RunCompiler( sParameters ); //Generate bytecode file.
		compiler_done = true;
		ImGui::SetWindowFocus("Message window");

		if (AGK_Compiler::GetCompilerErrorCount() <= 0) {
			//If no errors , prepare to run or broadcast.

			uString m_szCopyFrom;
			uString m_szExeName;
			uString m_szBroadcaster;

			//Get .exe name.
			m_szExeName = m_szPathName.GetStr();
			m_szExeName.AppendAscii( '/' );
			if(mProject)
				m_szExeName.Append(mProject->m_sProjectFile);
			else
				m_szExeName.Append("sandbox");


			//Remove any .agk extensions.
			uString m_szFinalExeName;
			m_szFinalExeName = m_szExeName;
			pos = m_szFinalExeName.RevFindStr(".agk",1);
			if (pos > 0) {
				m_szFinalExeName.SubString(m_szExeName, 0, pos);
			}

#ifdef AGK_WINDOWS
			m_szExeName.Append(".exe");
#endif
#ifdef AGK_MACOS
			m_szExeName.Append(".app");
#endif

			m_szCopyFrom = curDir;
#ifdef AGK_WINDOWS
			//Get current .exe timestamp.
			char * fdate = getFileCreationTime((char *)m_szExeName.GetStr());
//			m_szCopyFrom.Append("/media/interpreters/Windows.exe");

			// now always 64-bit
			m_szCopyFrom.Append("/media/interpreters/Windows64.exe");
			//	m_szCopyFrom.Append("/media/interpreters/Windows.exe");
			
			//Get interpreter timestamp.
			char * fdatefrom = getFileCreationTime((char *)m_szCopyFrom.GetStr());
#else
#ifdef AGK_MACOS
			//m_szCopyFrom.Append("/../../../Interpreter/Mac.app");
			m_szCopyFrom.Append("/media/interpreters/Mac.app");
			if(g_pMacFileManager == 0) g_pMacFileManager = [[NSFileManager alloc] init];
#else
#ifdef AGK_LINUX
#ifdef __x86_64_
			m_szCopyFrom.Append("/media/interpreters/LinuxPlayer64");
#else
			m_szCopyFrom.Append("/media/interpreters/LinuxPlayer32");
#endif
#else
#ifdef AGK_RASPPI
			m_szCopyFrom.Append("/media/interpreters/PiPlayer");
#endif
#endif
#endif
#endif

			if (run) {

#ifdef AGK_MACOS

				//Always copy, we need to update the media inside the .app

				if (mProject) {
					
					NSString* pnsFromPath = [[NSString alloc] initWithUTF8String:m_szCopyFrom.GetStr()];
					NSString* pnsToPath = [[NSString alloc] initWithUTF8String:m_szExeName.GetStr()];
					[g_pMacFileManager removeItemAtPath : pnsToPath error : nil];
					[g_pMacFileManager copyItemAtPath : pnsFromPath toPath : pnsToPath error : nil];
					[pnsFromPath release];
					[pnsToPath release];

					// unsign the app as it will become invalid when media is added
					uString sParameters;
					sParameters.SetStr("--remove-signature ");
					sParameters.Append("\"");
					sParameters.Append( m_szExeName );
					sParameters.Append("/Contents/MacOS/AppGameKit Player\"");
					
					static int iStdOutLength, iStdErrLength;
					int appid = run_command("/usr/bin/codesign", sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (appid == false) {
						AddToCompilerLog(2, "Failed to remove interpreter signature, MacOS may refuse to run the app" );
					}
					else
					{
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						
						while (still_running == 1 && agk::Timer() - timestart < 5)
						{
							usleep(200 * 1000);
							still_running = agk::GetAppRunning(appid);
						}

						if (still_running) {
							AddToCompilerLog(2, "Removing interpreter signature timed out, MacOS may refuse to run the app" );
							agk::TerminateApp(appid);
						}
					}

					// Delete code signature folder
					uString sCodeSignaturePath = m_szExeName;
					sCodeSignaturePath.AppendUTF8( "/Contents/_CodeSignature" );
					[g_pMacFileManager removeItemAtPath: [NSString stringWithUTF8String: sCodeSignaturePath.GetStr()] error: nil];
					sCodeSignaturePath = m_szExeName;
					sCodeSignaturePath.AppendUTF8( "/Contents/CodeResources" );
					[g_pMacFileManager removeItemAtPath: [NSString stringWithUTF8String: sCodeSignaturePath.GetStr()] error: nil];
                    
                    // Adhoc sign the app for arm64 requirements
                    
                    sParameters.SetStr("--force -s - ");
                    sParameters.Append("\"");
                    sParameters.Append( m_szExeName );
                    sParameters.Append("\"");
                    
                    appid = run_command("/usr/bin/codesign", sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

                    if (appid == false) {
                        AddToCompilerLog(2, "Failed to adhoc sign the app, MacOS may refuse to run the app" );
                    }
                    else
                    {
                        float timestart = agk::Timer();
                        int still_running;
                        still_running = agk::GetAppRunning(appid);
                        
                        while (still_running == 1 && agk::Timer() - timestart < 5)
                        {
                            usleep(200 * 1000);
                            still_running = agk::GetAppRunning(appid);
                        }

                        if (still_running) {
                            AddToCompilerLog(2, "Adhoc signing timed out, MacOS may refuse to run the app" );
                            agk::TerminateApp(appid);
                        }
                    }

					//Now copy the media folder.

					uString sFromPath = m_szPathName;
					uString sToPath = m_szExeName;

					sFromPath.Append("/media");
					//sToPath.Append(sProjectName);
					sToPath.Append("/Contents/Resources/media");

					//printf( "%s\n", sFromPath.GetStr() );
					//printf( "%s\n", sToPath.GetStr() );

					NSString* pFromMedia = [[NSString alloc] initWithUTF8String:sFromPath.GetStr()];
					NSString* pToMedia = [[NSString alloc] initWithUTF8String:sToPath.GetStr()];

					NSError *fileerror = nil;
					[g_pMacFileManager removeItemAtPath : pToMedia error : nil];
					if (![g_pMacFileManager copyItemAtPath : pFromMedia toPath : pToMedia error : &fileerror])
					{
						AddToCompilerLog(2, "Err: %s\n", [[fileerror localizedDescription] UTF8String]);
					}

					[pFromMedia release];
					[pToMedia release];

					// Plugins
					sFromPath.SetStr(m_szPathName.GetStr());
					sToPath = m_szExeName;

					sFromPath.Append("/Plugins");
					//sToPath.Append(sProjectName);
					//sToPath = m_szExeName;
					sToPath.Append("/Contents/Resources/Plugins");

					pFromMedia = [[NSString alloc] initWithUTF8String:sFromPath.GetStr()];
					pToMedia = [[NSString alloc] initWithUTF8String:sToPath.GetStr()];

					if ([g_pMacFileManager fileExistsAtPath : pFromMedia])
					{
						[g_pMacFileManager removeItemAtPath : pToMedia error : nil];
						if (![g_pMacFileManager copyItemAtPath : pFromMedia toPath : pToMedia error : &fileerror])
						{
							NSLog(@"error %@",fileerror);
							AddToCompilerLog(2, "Err: %s\n", [[fileerror localizedDescription] UTF8String]);
							//printf("%s", [[fileerror localizedDescription] UTF8String]);
						}
					}

					[pFromMedia release];
					[pToMedia release];

				}
				else {

					//Sandbox never overwrite.

					//Camt run app inside app , so copy to outside.
//					uString usMacFinal = appFolder;
//					usMacFinal.Append("/sandbox.app");

					uString usMacFinal = defaultWriteFolder;
					usMacFinal.Append("sandbox.app");

					m_szExeName = usMacFinal;
					NSString* pnsFromPath = [[NSString alloc] initWithUTF8String:m_szCopyFrom.GetStr()];
					NSString* pnsToPath = [[NSString alloc] initWithUTF8String:usMacFinal.GetStr()];

					//Remove for now, find a way to not do this, perhaps compile directly to inside .app if exists.
					[g_pMacFileManager removeItemAtPath : pnsToPath error : nil];
					[g_pMacFileManager copyItemAtPath : pnsFromPath toPath : pnsToPath error : nil];

					[pnsFromPath release];
					[pnsToPath release];

					// unsign the app as it will become invalid when media is added
					uString sParameters;
					sParameters.SetStr("--remove-signature ");
					sParameters.Append("\"");
					sParameters.Append( usMacFinal );
					sParameters.Append("/Contents/MacOS/AppGameKit Player\"");
					
					static int iStdOutLength, iStdErrLength;
					int appid = run_command("/usr/bin/codesign", sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (appid == false) {
						AddToCompilerLog(2, "Failed to remove interpreter signature, MacOS may refuse to run the app" );
					}
					else
					{
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						
						while (still_running == 1 && agk::Timer() - timestart < 5)
						{
							usleep(200 * 1000);
							still_running = agk::GetAppRunning(appid);
						}

						if (still_running) {
							AddToCompilerLog(2, "Removing interpreter signature timed out, MacOS may refuse to run the app" );
							agk::TerminateApp(appid);
						}
					}

					// Delete code signature folder
					uString sCodeSignaturePath = usMacFinal;
					sCodeSignaturePath.AppendUTF8( "/Contents/_CodeSignature" );
					[g_pMacFileManager removeItemAtPath: [NSString stringWithUTF8String: sCodeSignaturePath.GetStr()] error: nil];
					sCodeSignaturePath = usMacFinal;
					sCodeSignaturePath.AppendUTF8( "/Contents/CodeResources" );
					[g_pMacFileManager removeItemAtPath: [NSString stringWithUTF8String: sCodeSignaturePath.GetStr()] error: nil];
					
					//Now copy the media folder.
					//We will not change the media so dont overwrite.
//					uString sFromPath = startupFolder;
//					sFromPath.Append("/media/sandbox/media");

//					uString sFromPath = startupFolder;
//					sFromPath.Append("/media/sandbox/media");

					uString sFromPath = defaultWriteFolder;
					sFromPath.Append("sandbox/media");



					uString sToPath = usMacFinal;

					//sToPath.Append(sProjectName);
					sToPath.Append("/Contents/Resources/media");

					//printf( "%s\n", sFromPath.GetStr() );
					//printf( "%s\n", sToPath.GetStr() );

					NSString* pFromMedia = [[NSString alloc] initWithUTF8String:sFromPath.GetStr()];
					NSString* pToMedia = [[NSString alloc] initWithUTF8String:sToPath.GetStr()];

					NSError *fileerror;
					[g_pMacFileManager removeItemAtPath : pToMedia error : nil];
					if (![g_pMacFileManager copyItemAtPath : pFromMedia toPath : pToMedia error : &fileerror])
					{
						AddToCompilerLog(2, "Err: %s\n", [[fileerror localizedDescription] UTF8String]);
					}

				}

				//.app/Contents/MacOS/AppGameKit Player
				uString usMacApp = m_szExeName;
				usMacApp.Append("/Contents/MacOS/AppGameKit Player");
				AddToCompilerLog(2, "Running %s\n", usMacApp.GetStr());
				game_is_running = agk::RunApp(usMacApp.GetStr(), "");

//				NSString* pPath = [[NSString alloc] initWithUTF8String:m_szExeName.GetStr()];
//				[[NSWorkspace sharedWorkspace] launchApplication:pPath];
//				[pPath release];

#else
#ifdef AGK_LINUX
				if (1==1) {
#else
				if (pref.bTimestampExe == false || fdate == NULL || fdatefrom == NULL) {
#endif
					//date error just copy.
#ifdef AGK_LINUX
//					DebugInfo((char*)m_szCopyFrom.GetStr(), "CopyFrom");
//					DebugInfo((char*)m_szExeName.GetStr(), "CopyTo");
#endif

					cp_copyfile((char*)m_szCopyFrom.GetStr(), (char*)m_szExeName.GetStr());

#ifdef AGK_LINUX
					chmod((char*)m_szExeName.GetStr(), 0777);
#endif

				}
#ifndef AGK_LINUX
				else {
					if (strcmp(fdatefrom, fdate) != 0) {
						//They are not the same , copy a new interpreter.
						cp_copyfile((char*)m_szCopyFrom.GetStr(), (char*)m_szExeName.GetStr());
						
						//Time stamp file.
						FILETIME thefiletime;
						HANDLE filehandle = CreateFileA((char *)m_szCopyFrom.GetStr(),
							GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL, OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL, NULL);

						if (GetFileTime(filehandle, NULL, NULL, &thefiletime)) {

							CloseHandle(filehandle);

							HANDLE filehandle = CreateFileA((char *)m_szExeName.GetStr(),
								FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, NULL);

							SetFileTime(filehandle, NULL, NULL, &thefiletime);
							CloseHandle(filehandle);
						}
						else {
							CloseHandle(filehandle);
						}
					}
				}
#endif
				AddToCompilerLog(2, "Running %s\n", m_szExeName.GetStr());

#ifdef AGK_LINUX
				//DebugInfo( (char *) m_szExeName.GetStr(), "Launch");
#endif

				game_is_running = agk::RunApp(m_szExeName.GetStr(), "");

#endif
				//Reset all keys.
				for (int i = 8; i < 273; i++) {
					agk::KeyUp(i);
				}

//				//Restore window focus.
//#ifdef AGK_WINDOWS
//				SetForegroundWindow(g_agkhWnd);
//				SetFocus(g_agkhWnd);
//				Sleep(100);
//#endif

			}

			if (broadcast) {

				//PE: Not neded anymore , keep here if buildin broadcaster make problems.
				m_szBroadcaster = curDir;
#ifdef AGK_WINDOWS
				m_szBroadcaster.Append("/interpreters/AGKBroadcaster.exe");
#else
#ifdef AGK_MACOS
				m_szBroadcaster.Append("/interpreters/AGKBroadcaster");
#else
#ifdef AGK_LINUX
				m_szBroadcaster.Append("/interpreters/AGKBroadcaster32");
#else
#ifdef AGK_RASPPI
				m_szBroadcaster.Append("/interpreters/AGKBroadcaster32");
#endif
#endif
#endif
#endif

				uString bc_err;
				char cmdline[MAX_PATH + MAX_PATH];
				//char *oldpath = agk::GetWritePath();

				//Just in case ... no error msg needed ...
				sprintf(cmdline, "stop");
				BroadcasterCommand(cmdline, bc_err);
				sprintf(cmdline, "disconnectall"); //
				BroadcasterCommand(cmdline, bc_err);

				InitBroadcaster();

				if (useIP) {
					bRemote_broadcast_is_running = true;
					AddToCompilerLog(2, "Broadcasting on: %s\n", useIP);
				}
				else {
					bRemote_broadcast_is_running = false;
				}

				AddToCompilerLog(2, "Broadcasting: %s\n", m_szPathName.GetStr() );

				sprintf(cmdline, "setproject %s", m_szPathName.GetStr());
				BroadcasterCommand(cmdline, bc_err);
				if( bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr() );


				if (!useIP) {
					if ( !debug ) sprintf(cmdline, "connectall");
				}
				else {
					sprintf(cmdline, "connect %s\n", useIP);
				}
				BroadcasterCommand(cmdline,bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());

				sprintf(cmdline, "run");
				BroadcasterCommand(cmdline,bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
				
				broadcast_is_running = true;

				//Give it time to sent. without changing folders.
				//Sleep(100);
				//We cant change folder yet , as sentfiles will fail. so do it later.
				//agk::SetRawWritePath(oldpath);
				//agk::SetFolder("");
				//delete oldpath;

				//No need to get the compiler ok message.
				//AGK_Compiler::ResetCompilerErrors();
			}


			if (debug) {

				//PE: Not neded anymore , keep here if buildin broadcaster make problems.
				m_szBroadcaster = curDir;

				//launch interpreter.

				//Need pipes here:
				//hStdError
				if (!useIP) {
#ifdef AGK_MACOS
					// must copy interpreter from the IDE .app folder
					uString usMacFinal = defaultWriteFolder;
					usMacFinal.Append("debug.app");

					NSString* pnsFromPath = [[NSString alloc] initWithUTF8String:m_szCopyFrom.GetStr()];
					NSString* pnsToPath = [[NSString alloc] initWithUTF8String:usMacFinal.GetStr()];

					//Remove for now, find a way to not do this, perhaps compile directly to inside .app if exists.
					[g_pMacFileManager removeItemAtPath : pnsToPath error : nil];
					[g_pMacFileManager copyItemAtPath : pnsFromPath toPath : pnsToPath error : nil];

					[pnsFromPath release];
					[pnsToPath release];

					// unsign the app as it will become invalid when media is added
					uString sParameters;
					sParameters.SetStr("--remove-signature ");
					sParameters.Append("\"");
					sParameters.Append( usMacFinal );
					sParameters.Append("/Contents/MacOS/AppGameKit Player\"");
					
					static int iStdOutLength, iStdErrLength;
					int appid = run_command("/usr/bin/codesign", sParameters.GetStr(), &iStdOutLength, &iStdErrLength, false);

					if (appid == false) {
						AddToCompilerLog(2, "Failed to remove interpreter signature, MacOS may refuse to run the app" );
					}
					else
					{
						float timestart = agk::Timer();
						int still_running;
						still_running = agk::GetAppRunning(appid);
						
						while (still_running == 1 && agk::Timer() - timestart < 5)
						{
							usleep(200 * 1000);
							still_running = agk::GetAppRunning(appid);
						}

						if (still_running) {
							AddToCompilerLog(2, "Removing interpreter signature timed out, MacOS may refuse to run the app" );
							agk::TerminateApp(appid);
						}
					}

					// Delete code signature folder
					uString sCodeSignaturePath = usMacFinal;
					sCodeSignaturePath.AppendUTF8( "/Contents/_CodeSignature" );
					[g_pMacFileManager removeItemAtPath: [NSString stringWithUTF8String: sCodeSignaturePath.GetStr()] error: nil];
					sCodeSignaturePath = usMacFinal;
					sCodeSignaturePath.AppendUTF8( "/Contents/CodeResources" );
					[g_pMacFileManager removeItemAtPath: [NSString stringWithUTF8String: sCodeSignaturePath.GetStr()] error: nil];


					uString usMacApp = usMacFinal;
					usMacApp.Append("/Contents/MacOS/AppGameKit Player");
					AddToCompilerLog(2, "Debugging %s\n", usMacApp.GetStr());
					debug_is_running = agk::RunApp(usMacApp.GetStr(), "");
#else
					debug_is_running = agk::RunApp(m_szCopyFrom.GetStr(), "");
#endif
					//Reset all keys.
					for (int i = 8; i < 273; i++) {
						agk::KeyUp(i);
					}

				}
				else {
					remote_debug_is_running = 1;
					AddToCompilerLog(2, "Remote debugging on: %s\n", useIP );
				}
				uString bc_err;
				char cmdline[MAX_PATH + MAX_PATH];
				//char *oldpath = agk::GetWritePath();

				//Just in case ... no error msg needed ...
				sprintf(cmdline, "stop");
				BroadcasterCommand(cmdline, bc_err);
				sprintf(cmdline, "disconnectall");
				BroadcasterCommand(cmdline, bc_err);

				InitBroadcaster();

				AddToCompilerLog(2, "Debugging: %s\n", m_szPathName.GetStr());
#ifdef AGK_WINDOWS
				if (debug_is_running && !useIP && pref.bDebugBringAppToFront) {
					debug_hwnd = FindWindowA(NULL, "AppGameKit Player");
					if(!debug_hwnd)
						debug_hwnd = FindWindowA(NULL, "AGK");
					if (!debug_hwnd) {
						Sleep(1000);
						debug_hwnd = FindWindowA(NULL, "AppGameKit Player");
						if (!debug_hwnd) {
							Sleep(1000);
							debug_hwnd = FindWindowA(NULL, "AppGameKit Player");
						}
					}
					if (debug_hwnd) {
						::SetWindowPos(debug_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					}
					if (agkfullscreen) {
						//This will not work in full screen.
						agkfullscreen = 1 - agkfullscreen;
						agk::SetWindowSize(agk::GetDeviceWidth(), agk::GetDeviceHeight(), agkfullscreen);
						agk::SetRawMouseVisible(1); //count internal windows mouse visible counter up.
					}
				}
#endif

				sprintf(cmdline, "setproject %s", m_szPathName.GetStr());
				BroadcasterCommand(cmdline,bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Debugging: %s\n", bc_err.GetStr());

				if(useIP)
					sprintf(cmdline, "connect %s\n", useIP);
				else
					sprintf(cmdline, "connect 127.0.0.1\n");

				BroadcasterCommand(cmdline,bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Debugging: %s\n", bc_err.GetStr());

				if(mProject)
					addAllBreakPoints((char*)mProject->m_sProjectFile.GetStr());
				addAllWatches();

				sprintf(cmdline, "debug");
				BroadcasterCommand(cmdline,bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Debugging: %s\n", bc_err.GetStr());

				if (pref.bDebugBringToFront) {
					BroadcasterCommand("pause",bc_err);
					BroadcasterCommand("continue",bc_err);
				}


			}



#ifdef AGK_WINDOWS
			delete [] fdate;
			delete [] fdatefrom;
#endif
		}
		else {
			//Compile errors , make sure window is open.
			pref.bDisplayMessageWindow = true;
		}

#ifdef AGK_WINDOWS
//		ret = _chdir(curDir);
#else
//		ret = chdir(curDir);
#endif

//	}

	return true;
}


//Add all breakpoints from all open files in project being debugged.
void addAllBreakPoints(char *projectname)
{
	char findProjectName[MAX_PATH];

	extern bool updateWatchCalled;
	if (!pref.bDebugBringToFront) {
		updateWatchCalled = true;
	}
	else {
		updateWatchCalled = false;
	}

	if (projectname == NULL) {
		//Locate project.
		if ( (remote_debug_is_running || debug_is_running) && mCompiledProject) {
			strcpy( findProjectName, mCompiledProject->m_sProjectFile.GetStr() );
		}
		else {
			//No project name found.
			return;
		}
	}
	else {
		strcpy(findProjectName, projectname);
	}
	//Delete all current breakpoints.
	uString bc_err;
	BroadcasterCommand("delete all breakpoints", bc_err);
	if (bc_err.GetLength() > 0)
		AddToCompilerLog(2, "Debugging: %s\n", bc_err.GetStr());

	//If all breakpoint is disabled, exit.
	extern bool dis_all_breakpoints;
	if (dis_all_breakpoints)
		return;
	cProjectItem::sProjectFiles * m_pCurrentFile;
	cProjectItem * firstProjects;
	firstProjects = allProjects;


	//Find files outside project with breakpoints.
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {

			if (m_pCurrentFile->m_bOpenForEdit && m_pCurrentFile->m_editor && m_pCurrentFile->m_editor->bEditorVisible) {

				for (auto it = m_pCurrentFile->m_editor->bpts.begin(); it != m_pCurrentFile->m_editor->bpts.end(); ++it)
				{
					int lineno;
					char tmp[MAX_PATH];

					lineno = *it;

					//Must be added as relative.
					bool bAddRelative = false;
					uString relative;
					if (pCurrentSelectedProject) {

						//Make it relative to the current selected project.
						uString fname = m_pCurrentFile->m_sFullPath;
						fname.ReplaceStr("\\", "/");
						relative = fname;

						uString pfolder = ";:;";
						uString projectpath = pCurrentSelectedProject->m_sProjectFileFullPath;
						projectpath.ReplaceStr("\\", "/");
						int pos = projectpath.RevFindStr("/");
						if (pos > 0) {
							projectpath.SubString(pfolder, 0, pos + 1);
						}

						cProjectItem::sProjectFiles *m_pSearchFile;

						relative.ReplaceStr(pfolder, "");
						bAddRelative = true;
						//resolve to relative path.
						uString szCompares = "";
						int countslash = pfolder.Count('/');
						int i, slashpos = 0;
						if (pfolder[0] != relative[0]) {
							i = 0;
						}
						else {
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
							relative = szFinal;
						}

						sprintf(tmp, "breakpoint %s:%d", relative.GetStr(), lineno);
						uString bc_err;
						BroadcasterCommand(tmp,bc_err);
						if (bc_err.GetLength() > 0)
							AddToCompilerLog(2, "Debugging: %s\n", bc_err.GetStr());
					}

				}

			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	//Find open files with breakpoints.
	while (firstProjects) {
		if (strcmp(firstProjects->m_sProjectFile.GetStr(), findProjectName) == 0) {
			cProjectItem::sProjectFiles * m_pCurrentFile;
			m_pCurrentFile = firstProjects->m_pFirstFile;
			while (m_pCurrentFile)
			{
				if (m_pCurrentFile->m_bOpenForEdit && m_pCurrentFile->m_editor && m_pCurrentFile->m_editor->bEditorVisible ) {

					for (auto it = m_pCurrentFile->m_editor->bpts.begin(); it != m_pCurrentFile->m_editor->bpts.end(); ++it)
					{
						int lineno;
						char tmp[MAX_PATH];

						lineno = *it;
						
						//breakpoint myFile.agc:102
						sprintf(tmp, "breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), lineno);
						uString bc_err;
						BroadcasterCommand(tmp,bc_err);
						if (bc_err.GetLength() > 0)
							AddToCompilerLog(2, "Debugging: %s\n", bc_err.GetStr());
					}

				}
				m_pCurrentFile = m_pCurrentFile->m_pNext;
			}
		}
		firstProjects = firstProjects->m_pNext;
	}
}


const char * GetLastDebugMessageA()
{
	const char * tmp = GetLastDebugMessage();
	return tmp;
}


int ResetDebugMessageLogA()
{
	return 0;
}

int GetDebugMessageCountA()
{
	return 0;
}

void InitBroadcaster(void)
{
#ifdef AGK_MACOS
	signal(SIGPIPE, SIG_IGN);
#endif

	BroadcasterSetup();
}

int screenWidth = 0, screenHeight = 0;
void LoadAgkIdeIni(int initPart)
{
	FILE *fp;
	char activeproject[MAX_PATH];
	char fpReadLine[MAX_PATH];
	char cIniFile[MAX_PATH];
	strcpy(activeproject, "");
	int WindowPosX=0, WindowPosY=0;
	sprintf(cIniFile, "%sagkide.ini", defaultWriteFolder);
	fp = fopen(cIniFile, "r");
	if (!fp)
		fp = AGKfopen(cIniFile, "r");

	if (fp) {
		while (!feof(fp)) {
			if (fgets( &fpReadLine[0], MAX_PATH, fp)) {
				if (strncmp(fpReadLine, "DeviceWidth:", 12) == 0) {
					if (fgets(&fpReadLine[0], MAX_PATH, fp))
						screenWidth = atoi(fpReadLine);
				}
				if (strncmp(fpReadLine, "DeviceHeight:", 13) == 0) {
					if (fgets(&fpReadLine[0], MAX_PATH, fp))
						screenHeight = atoi(fpReadLine);
				}
				if (strncmp(fpReadLine, "agkfullscreen:", 14) == 0) {
					if (fgets(&fpReadLine[0], MAX_PATH, fp))
						agkfullscreen = atoi(fpReadLine);
				}
				if (strncmp(fpReadLine, "WindowPosX:", 11) == 0) {
					if (fgets(&fpReadLine[0], MAX_PATH, fp))
						WindowPosX = atoi(fpReadLine);
				}
				if (strncmp(fpReadLine, "WindowPosY:", 11) == 0) {
					if (fgets(&fpReadLine[0], MAX_PATH, fp))
						WindowPosY = atoi(fpReadLine);
				}

				

#ifdef AGK_WINDOWS
				if (initPart >= 2) {
					if (strncmp(fpReadLine, "agkmaximized:", 12) == 0) {
						if (fgets(&fpReadLine[0], MAX_PATH, fp)) {
							int iMaximize = atoi(fpReadLine);
							if(iMaximize == 1)
								ShowWindow(g_agkhWnd, SW_MAXIMIZE);
						}
					}
				}
#endif


				if (initPart >= 2) {

					if (strncmp(fpReadLine, "ActiveProject:", 14) == 0) {
						if (fgets(&fpReadLine[0], MAX_PATH, fp)) {
							//Open project.
							if (fpReadLine[strlen(fpReadLine) - 1] == 0xa)
								fpReadLine[strlen(fpReadLine) - 1] = 0;
							if (fpReadLine[strlen(fpReadLine) - 1] == 0xd)
								fpReadLine[strlen(fpReadLine) - 1] = 0;
							strcpy(activeproject, fpReadLine);
						}
					}

					if (strncmp(fpReadLine, "OpenProject:", 12) == 0) {
						if (fgets(&fpReadLine[0], MAX_PATH, fp)) {
							//Open project.
							if (fpReadLine[strlen(fpReadLine) - 1] == 0xa)
								fpReadLine[strlen(fpReadLine) - 1] = 0;
							if (fpReadLine[strlen(fpReadLine) - 1] == 0xd)
								fpReadLine[strlen(fpReadLine) - 1] = 0;
							LoadProjectFile((char *)&fpReadLine[0]);
							isProjectInAssetsBrowser();
						}
					}
				}

				if (initPart >= 2) {
					if (strncmp(fpReadLine, "addfolder:", 10) == 0) {
						if (fgets(&fpReadLine[0], MAX_PATH, fp)) {

							//Open project.

							if (fpReadLine[strlen(fpReadLine) - 1] == 0xa)
								fpReadLine[strlen(fpReadLine) - 1] = 0;
							if (fpReadLine[strlen(fpReadLine) - 1] == 0xd)
								fpReadLine[strlen(fpReadLine) - 1] = 0;

							//strcpy(customFolders[iCustomFolders], fpReadLine);
							//Add folder.
							char * selectedfolder = &fpReadLine[0];
							//Add new folder to assets browser.
							if (selectedfolder && strlen(selectedfolder) > 0 && m_pStartFolder && m_pStartFolder->m_pNext) {
								cFolderItem *m_psearchFolder;
								m_psearchFolder = m_pStartFolder->m_pNext;

								uString m_szPathName, szShortName;
								m_szPathName = selectedfolder;
								m_szPathName.ReplaceStr("\\\\", "/");
								m_szPathName.ReplaceStr("\\", "/");

								while (m_psearchFolder) {

									if (m_psearchFolder->m_sFolderFullPath.CompareTo(m_szPathName) == 0) {
										//Already added. ignore.
										break;
									}

									if (!m_psearchFolder->m_pNext) { //Last.

										szShortName = "unknown";
										int pos = m_szPathName.RevFindStr("/");
										if (pos > 0) {
											m_szPathName.SubString(szShortName, pos + 1);
										}
										cFolderItem *pNewItem = new cFolderItem();
										pNewItem->m_sFolder.SetStr(szShortName);
										strcpy(pNewItem->cfolder, pNewItem->m_sFolder.GetStr());

										pNewItem->m_sFolderFullPath.SetStr(m_szPathName);
										pNewItem->m_pSubFolder = NULL;
										pNewItem->count = ++iTotalFolders;
										pNewItem->m_pNext = NULL;
										pNewItem->m_pFirstFile = NULL;
										pNewItem->visible = true;
										pNewItem->alwaysvisible = true;
										pNewItem->deletethisentry = false;
										pNewItem->bIsCustomFolder = true;

										m_psearchFolder->m_pNext = pNewItem;

										pNewItem = new cFolderItem();
										pNewItem->m_sFolder = "...";
										strcpy(pNewItem->cfolder, pNewItem->m_sFolder.GetStr());
										pNewItem->count = 0;
										pNewItem->m_pNext = NULL;
										pNewItem->m_pSubFolder = NULL;
										pNewItem->m_pFirstFile = NULL;
										m_psearchFolder->m_pNext->m_pSubFolder = pNewItem;
										agk::SetFolder("");
//										getalldirs(m_psearchFolder->m_pNext->m_pSubFolder, NULL, (char *)m_szPathName.GetStr());
										getalldirs(m_psearchFolder->m_pNext->m_pSubFolder, m_psearchFolder->m_pNext, (char *)m_szPathName.GetStr());

										if (iCustomFolders < 10) {
											strcpy(customFolders[iCustomFolders++], m_szPathName.GetStr());
										}

										setVisibleProjectsInAssetBrowser();
									}
									m_psearchFolder = m_psearchFolder->m_pNext;
								}
							}

							//isProjectInAssetsBrowser();
						}
					}
				}

				if (initPart >= 2) {
					if (strncmp(fpReadLine, "recentproject:", 13) == 0) {
						if (fgets(&fpReadLine[0], MAX_PATH, fp)) {
							szRecentProjects[iResentProjects] = fpReadLine;
							szRecentProjects[iResentProjects].ReplaceStr("\n", "");
							iResentProjects++;
						}
					}
				}
				if (initPart >= 2) {
					if (strncmp(fpReadLine, "recentfiles:", 12) == 0) {
						if (fgets(&fpReadLine[0], MAX_PATH, fp)) {
							szRecentFiles[iResentFiles] = fpReadLine;
							szRecentFiles[iResentFiles].ReplaceStr("\n", "");
							iResentFiles++;
						}
					}
				}


			}
		}
		fclose(fp);
	}

	if(initPart == 1 && screenWidth > 150 && screenHeight > 150 && screenWidth < 9999 && screenHeight < 9999 ) {
		agk::SetWindowSize(screenWidth, screenHeight, 0);
		if (iDeviceHeight != agk::GetDeviceHeight() || iDeviceWidth != agk::GetDeviceWidth()) {
			iDeviceWidth = agk::GetDeviceWidth();
			iDeviceHeight = agk::GetDeviceHeight();
			agk::SetVirtualResolution(iDeviceWidth, iDeviceHeight);
		}
	}
	else {
		if (initPart == 1 ) 
			agk::SetWindowSize(1280, 800, agkfullscreen);

		if (initPart == 2 && agkfullscreen && screenWidth > 150 && screenHeight > 150 && screenWidth < 9999 && screenHeight < 9999)
			agk::SetWindowSize(screenWidth, screenHeight, agkfullscreen);

	}
	if (initPart == 2 && (WindowPosY != 0 || WindowPosX != 0)) {

		//PE: When restoring and "minimized" , offset is way off and window was hidden.
		if (WindowPosX < -screenWidth && WindowPosY < -screenHeight)
		{
			//just ignore agk has already centered the window.
//			WindowPosX = 0;
//			WindowPosY = 0;
//			agk::SetWindowPosition(WindowPosX, WindowPosY);
		}
		else
			agk::SetWindowPosition(WindowPosX, WindowPosY);
	}

	if (initPart == 1) {
		//Also load preferences.

		//Start by resetting if this should be a older pref file.
		ResetPreferences();

		sprintf(cIniFile, "%sagkide.pref", defaultWriteFolder);
		FILE* preffile = fopen(cIniFile, "rb+");
		if (!preffile)
			preffile = AGKfopen(cIniFile, "rb+");

		if (preffile) {
			size_t size = fread(&pref, 1, sizeof(pref), preffile);
			//Valid pref:
			if (pref.szCheckFile[0] != 'I' && pref.szCheckFile[8] != 'S') {
				ResetPreferences();
			}
			pref.launched++;
			fclose(preffile);
		}
		//Dont start it as open.
		pref.show_preferences_window = false;

		CheckKeyboardShortcuts();
	}

	if (initPart >= 2 && strlen(activeproject) > 0 ) {
		
		//It will just locate it and select it.
		LoadProjectFile((char *)&activeproject[0]);
		isProjectInAssetsBrowser();
		if (pCurrentSelectedProject && pCurrentSelectedProject->m_pMediaFolder)
			m_pSelectedFolder = pCurrentSelectedProject->m_pMediaFolder;

	}


}

void SaveAgkIdeIni(void)
{
	cProjectItem * searchProject;
	FILE *fp;
	char cWriteLine[MAX_PATH+30];
	char cIniFile[MAX_PATH];
	sprintf(cIniFile, "%sagkide.ini", defaultWriteFolder);
	fp = fopen(cIniFile, "w+");
	if (!fp)
		fp = AGKfopen(cIniFile, "w+");

	if (fp == NULL) {
		uString cdir = defaultWriteFolder;
		cdir.ReplaceStr("\\", "/");
		cdir.ReplaceStr("AGKApps/TheGameCreators/Ide/", "AGKApps");
#ifdef AGK_WINDOWS
		mkdir(cdir.GetStr());
#else
		mkdir(cdir.GetStr(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
		cdir.Append("/TheGameCreators");
#ifdef AGK_WINDOWS
		mkdir(cdir.GetStr());
#else
		mkdir(cdir.GetStr(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
		cdir.Append("/Ide");
#ifdef AGK_WINDOWS
		mkdir(cdir.GetStr());
#else
		mkdir(cdir.GetStr(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
		fp = fopen(cIniFile, "w+");
		if (!fp)
			fp = AGKfopen(cIniFile, "w+");

	}
	if (fp) {
#ifdef AGK_WINDOWS
		int cxborder = (GetSystemMetrics(SM_CXEDGE) + GetSystemMetrics(SM_CXBORDER) + GetSystemMetrics(SM_CXPADDEDBORDER)) * 2;
		cxborder += GetSystemMetrics(SM_CXSIZEFRAME);
		cxborder += 2; //What im i missing im always 2 low ?
		int height = GetSystemMetrics(SM_CYCAPTION) +
			GetSystemMetrics(SM_CYSIZEFRAME) +
			((GetSystemMetrics(SM_CYEDGE) + GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CXPADDEDBORDER)) * 2);
		height += 2; //What! im also missing 2 here ?
		height += agk::GetDeviceHeight();
#else
		int cxborder = 0;
		int height = agk::GetDeviceHeight();
#endif

		sprintf(cWriteLine, "DeviceWidth:\n%d\n", agk::GetDeviceWidth() + cxborder);
		fputs(cWriteLine, fp);
		sprintf(cWriteLine, "DeviceHeight:\n%d\n", height);
		fputs(cWriteLine, fp);
		sprintf(cWriteLine, "agkfullscreen:\n%d\n", agkfullscreen);
		fputs(cWriteLine, fp);

#ifdef AGK_WINDOWS
		if (IsZoomed(g_agkhWnd)) {
			sprintf(cWriteLine, "agkmaximized:\n%d\n", 1);
			fputs(cWriteLine, fp);
		}
		else {
			sprintf(cWriteLine, "agkmaximized:\n%d\n", 0);
			fputs(cWriteLine, fp);
		}
#endif

#ifdef AGK_WINDOWS
		RECT rect = { NULL };
		GetWindowRect(g_agkhWnd, &rect);

		sprintf(cWriteLine, "WindowPosX:\n%d\n", rect.left);
		fputs(cWriteLine, fp);
		sprintf(cWriteLine, "WindowPosY:\n%d\n", rect.top);
		fputs(cWriteLine, fp);
#endif
#ifdef AGK_MACOS
		//Mac:
		int wx,wy;
		glfwGetWindowPos(g_agkhWnd,&wx,&wy);
		if (wx > 0 && wy > 0) {
			sprintf(cWriteLine, "WindowPosX:\n%d\n", wx);
			fputs(cWriteLine, fp);
			sprintf(cWriteLine, "WindowPosY:\n%d\n", wy);
			fputs(cWriteLine, fp);
		}
#endif


		if (allProjects) {
			//Check if project already open.
			searchProject = allProjects;
			while (searchProject) {
				if (pCurrentSelectedProject == searchProject) {
					//Current active project.
					fputs("ActiveProject:\n", fp);
					fputs(searchProject->m_sProjectFileFullPath.GetStr(), fp);
					fputs("\n", fp);
				}
				fputs("OpenProject:\n", fp);
				fputs(searchProject->m_sProjectFileFullPath.GetStr(), fp);
				fputs("\n", fp);

				searchProject = searchProject->m_pNext;
			}
		}

		//Custom folders.
		for (int iNdex = 0; iNdex < iCustomFolders; iNdex++)
		{
			//Only if visible.
			char * selectedfolder = &customFolders[iNdex][0];
			if (selectedfolder && strlen(selectedfolder) > 0 && m_pStartFolder && m_pStartFolder->m_pNext) {
				cFolderItem *m_psearchFolder;
				m_psearchFolder = m_pStartFolder->m_pNext;

				uString m_szPathName, szShortName;
				m_szPathName = selectedfolder;
				m_szPathName.ReplaceStr("\\\\", "/");
				m_szPathName.ReplaceStr("\\", "/");

				while (m_psearchFolder) {
					if (m_psearchFolder->m_sFolderFullPath.CompareTo(m_szPathName) == 0) {
						if (m_psearchFolder->visible && m_psearchFolder->deletethisentry == false ) {
							//visible so save.
							sprintf(cWriteLine, "addfolder:\n%s\n", customFolders[iNdex]);
							fputs(cWriteLine, fp);
						}
					}
					m_psearchFolder = m_psearchFolder->m_pNext;
				}
			}

		}


		for (int i = 0; i < iResentProjects; i++) {
			sprintf(cWriteLine, "recentproject:\n%s\n", szRecentProjects[i].GetStr() );
			fputs(cWriteLine, fp);
		}
		for (int i = 0; i < iResentFiles; i++) {
			sprintf(cWriteLine, "recentfiles:\n%s\n", szRecentFiles[i].GetStr());
			fputs(cWriteLine, fp);
		}

		fclose(fp);
	}

	//Also save preferences.

	sprintf(cIniFile, "%sagkide.pref", defaultWriteFolder);
	FILE* preffile = fopen(cIniFile, "wb+");
	if (!preffile)
		preffile = AGKfopen(cIniFile, "wb+");

	if (preffile) {
		fwrite(&pref, 1, sizeof(pref), preffile);
		fclose(preffile);
	}

}


bool OpenProject()
{
	char * cFileSelected;

	cFileSelected = (char *) noc_file_dialog_open(NOC_FILE_DIALOG_OPEN,"agk\0*.agk\0", NULL, NULL);

	if (cFileSelected && strlen(cFileSelected) > 0) {
		uString usProjectName;
		usProjectName = cFileSelected;

		AddRecentFiles(usProjectName.GetStr(), NULL);

		LoadProjectFile( (char *) usProjectName.GetStr() );
	}

	return true;
}

//Move project up in the project list.
void ProjectMoveUp(cProjectItem * moveProject)
{
	if (!allProjects)
		return;
	if (moveProject == allProjects)
		return;

	cProjectItem * searchProject = NULL, *searchLastProject = NULL, *searchNextProject = NULL;
	if (allProjects) {
		//Check if project is open.
		searchProject = allProjects;
		while (searchProject) {
			if (searchProject == moveProject) {
				if (searchLastProject) {
					//Swap entries.
					cProjectItem *m_pNext;
					
					//Swap next pointers.
					m_pNext = searchLastProject->m_pNext;
					searchLastProject->m_pNext = searchProject->m_pNext;
					searchProject->m_pNext = searchLastProject;

					if (searchLastProject == allProjects) {
						allProjects = searchProject;
					}
					else {
						//Find where next pointing to searchLastProject and set it to searchProject
						searchNextProject = allProjects;
						while (searchNextProject) {
							if (searchNextProject->m_pNext == searchLastProject) {
								searchNextProject->m_pNext = searchProject;
								break;
							}
							searchNextProject = searchNextProject->m_pNext;
						}
					}
					break;
				}
			}
			searchLastProject = searchProject;
			searchProject = searchProject->m_pNext;
		}
	}
}

//Move project down in the project list.
void ProjectMoveDown(cProjectItem * moveProject)
{
	if (!allProjects)
		return;
	if (!moveProject)
		return;
	if (!moveProject->m_pNext)
		return;

	cProjectItem * searchProject = NULL, *searchLastProject = NULL, *searchNextProject = NULL;
	if (allProjects) {
		//Check if project is open.
		searchProject = allProjects;
		while (searchProject) {

			if (searchProject == moveProject) {
				searchNextProject = searchProject->m_pNext;

				searchProject->m_pNext = searchNextProject->m_pNext;
				searchNextProject->m_pNext = searchProject;

				if (searchProject == allProjects) {
					allProjects = searchNextProject;
					break;
				}
				else {
					searchLastProject = allProjects;
					while (searchLastProject) {
						if (searchLastProject->m_pNext == searchProject) {
							searchLastProject->m_pNext = searchNextProject;
							break;
						}
						searchLastProject = searchLastProject->m_pNext;
					}
				}
				break;
			}

			searchProject = searchProject->m_pNext;
		}
	}

	return;
}

void RemoveProject(cProjectItem * removeProject)
{
	cProjectItem * searchProject, *searchLastProject;
	cProjectItem::sProjectFiles * m_pSearchFile, *m_pLastSearchFile;

	searchLastProject = NULL;
	if (allProjects) {
		//Check if project is open.
		searchProject = allProjects;
		while (searchProject) {

			if (removeProject == NULL || removeProject == searchProject) { //remove all.

				//Remove from assets browser.
				uString sSearch;
				sSearch = "-> "; //Project Media"
				sSearch.Append(searchProject->m_sProjectFile.GetStr());
				sSearch.Append(" Project Media");
				cFolderItem *m_psearchFolder = NULL, *m_pDeleteFolder = NULL, *m_pPrevFolder = NULL;
				//Bug, this can be called many times , if there are unsaved changes and we need to delay the removal.
				//Always check searchProject->m_pMediaFolder.
				if (searchProject->m_pMediaFolder) {
					if (m_pStartFolder && m_pStartFolder->m_pNext) {
						m_psearchFolder = m_pStartFolder->m_pNext;
						while (m_psearchFolder) {
							if (m_psearchFolder->m_sFolder == sSearch) {
								m_pDeleteFolder = m_psearchFolder;
								extern cFolderItem *m_pSelectedFolder;
								if (m_pSelectedFolder == m_pDeleteFolder)
									m_pSelectedFolder = NULL;
								break;
							}
							m_pPrevFolder = m_psearchFolder;
							m_psearchFolder = m_psearchFolder->m_pNext;
						}
						if (m_pDeleteFolder) {
							if (m_pPrevFolder) {
								m_pPrevFolder->m_pNext = m_pDeleteFolder->m_pNext;
								delete m_pDeleteFolder;
								m_pDeleteFolder = 0;
								searchProject->m_pMediaFolder = NULL;
							}
							else {
								if (m_pDeleteFolder == m_pStartFolder->m_pNext) {
									if (m_pDeleteFolder->m_pNext) {
										m_pStartFolder->m_pNext = m_pDeleteFolder->m_pNext;
										delete m_pDeleteFolder;
										m_pDeleteFolder = 0;
										searchProject->m_pMediaFolder = NULL;
									}
								}
							}
						}
					}
				}

				//Remove project searchProject
				if (searchProject == allProjects) {
					//First project , just delete.
					m_pSearchFile = searchProject->m_pFirstFile;
					while (m_pSearchFile) {
						if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->bEditorVisible ) {
							if (m_pSearchFile->m_editor->CanUndo()) {
								//Need to save before closing.
								m_pSearchFile->m_editor->quiteditor = true;
								pRepeatRemoveProject = searchProject;
								if(removeProject == NULL)
									bRepeatRemoveProjectAll = true;
								else
									bRepeatRemoveProjectAll = false;
								return;
							}
						}
						m_pSearchFile = m_pSearchFile->m_pNext;
					}

					//No files need saving , so remove project.
					m_pSearchFile = searchProject->m_pFirstFile;
					while (m_pSearchFile) {

						//Make sure window is disabled.
						//Change pos of window in focus list. for CTRL+TAB to ignore.
						ImGuiContext& g = *GImGui;
						ImGuiWindow* current = NULL;
						for (int n = 0; n < g.WindowsFocusOrder.Size - 1; n++)
						{
							ImGuiWindow* window = g.WindowsFocusOrder[n];
							if (window->Name && strstr(window->Name, "/TextEditor")) {
								if (strncmp(m_pSearchFile->m_sEditName.GetStr(), window->Name, m_pSearchFile->m_sEditName.GetLength()) == 0) {
									current = window;
									current->Pos.x = -9999;
								}
							}
						}

						m_pSearchFile->m_editor->SetText("");
						if(m_pSearchFile->m_editor)
							delete(m_pSearchFile->m_editor);
						m_pSearchFile->m_editor = NULL;
						m_pLastSearchFile = m_pSearchFile;
						m_pSearchFile = m_pSearchFile->m_pNext;
						delete(m_pLastSearchFile);
					}
					searchProject->m_pFirstFile = NULL;

					if (searchProject->m_pNext != NULL) {
						if (pCurrentSelectedProject == allProjects)
							pCurrentSelectedProject = searchProject->m_pNext;
						allProjects = searchProject->m_pNext;
						m_ActiveEditor = NULL;

					}
					else {
						pCurrentSelectedProject = NULL;
						allProjects = NULL;
						m_ActiveEditor = NULL;
					}

					searchLastProject = searchProject;
					searchProject = searchProject->m_pNext;
					delete(searchLastProject);
				}
				else {


					m_pSearchFile = searchProject->m_pFirstFile;
					while (m_pSearchFile) {
						if (m_pSearchFile->m_bOpenForEdit && m_pSearchFile->m_editor && m_pSearchFile->m_editor->bEditorVisible ) {
							if (m_pSearchFile->m_editor->CanUndo()) {
								//Need to save before closing.
								m_pSearchFile->m_editor->quiteditor = true;
								pRepeatRemoveProject = searchProject;
								if (removeProject == NULL)
									bRepeatRemoveProjectAll = true;
								else
									bRepeatRemoveProjectAll = false;
								return;
							}
						}
						m_pSearchFile = m_pSearchFile->m_pNext;
					}
					//No files need saving , so remove project.
					m_pSearchFile = searchProject->m_pFirstFile;
					while (m_pSearchFile) {

						//Make sure window is disabled.
						//Change pos of window in focus list. for CTRL+TAB to ignore.
						ImGuiContext& g = *GImGui;
						ImGuiWindow* current = NULL;
						for (int n = 0; n < g.WindowsFocusOrder.Size - 1; n++)
						{
							ImGuiWindow* window = g.WindowsFocusOrder[n];
							if (window->Name && strstr(window->Name, "/TextEditor")) {
								if (strncmp(m_pSearchFile->m_sEditName.GetStr(), window->Name, m_pSearchFile->m_sEditName.GetLength()) == 0) {
									current = window;
									current->Pos.x = -9999;
								}
							}
						}

						m_pSearchFile->m_editor->SetText("");
						delete(m_pSearchFile->m_editor);
						m_pSearchFile->m_editor = NULL;
						m_pLastSearchFile = m_pSearchFile;
						m_pSearchFile = m_pSearchFile->m_pNext;
						delete(m_pLastSearchFile);
					}

					if (searchProject->m_pNext != NULL) {
						if (pCurrentSelectedProject == searchProject)
							pCurrentSelectedProject = searchProject->m_pNext;
						if (searchLastProject == NULL)
							allProjects = searchProject->m_pNext;
						else
							searchLastProject->m_pNext = searchProject->m_pNext;
						m_ActiveEditor = NULL;
					}
					else {

						if (searchLastProject == NULL)
							allProjects->m_pNext = NULL;
						else
							searchLastProject->m_pNext = NULL;

						pCurrentSelectedProject = NULL;
						m_ActiveEditor = NULL;
					}


					searchLastProject = searchProject;
					searchProject = searchProject->m_pNext;
				}

			}
			else {
				searchLastProject = searchProject;
				searchProject = searchProject->m_pNext;
			}
		}

	}

}

#define MACAGKPROJECTLINES 1000
uString sAgkProjectFiles[MACAGKPROJECTLINES];
int iAgkProjectFiles;
uString sSaveAgkProjectFiles[MACAGKPROJECTLINES];
int iSaveAgkProjectFiles;

void SaveAgkProjectFile(char *file)
{
	FILE *fp;
	fp = fopen(file, "w+");
	if (!fp)
		fp = AGKfopen(file, "w+");

	if (fp) {
		for (int i = 0; i < iSaveAgkProjectFiles; i++) {
			//iSaveAgkProjectFiles
			fputs(sSaveAgkProjectFiles[i].GetStr(), fp);
		}
		fclose(fp);
	}
	else {
		//Cant update project file.
#ifdef _DEBUG
		BoxerInfo(file, "Cant write to file:");
#endif
	}
}

void LoadAgkProjectFile(char *file)
{
	iAgkProjectFiles = 0;
	FILE *fp;
	char fpTmp[MAX_PATH];
	fp = fopen(file, "r");
	if (!fp)
		fp = AGKfopen(file, "r");

	if (fp) {
		while (!feof(fp)) {
			uString m_szLeft, m_szCommand, m_szRest, m_szFile, m_szPar, m_szPar2;
			if (fgets(fpTmp, MAX_PATH, fp)) {
				sAgkProjectFiles[iAgkProjectFiles].SetStr(fpTmp);
				iAgkProjectFiles++;
			}
		}
		fclose(fp);
	}
}

void SaveProjectFile(cProjectItem * mProject)
{
	cProjectItem * searchProject;
	cProjectItem::sProjectFiles * saveProjectFiles;

	if (allProjects) {
		//Check if project is open.
		searchProject = allProjects;
		while (searchProject) {

			if (mProject == NULL || searchProject == mProject)
			{
				//Save project file.

				uString m_szParse, m_szFileName, m_szPathName;
				m_szParse = searchProject->m_sProjectFileFullPath;
				m_szParse.ReplaceStr("\\\\", "//"); //To allow network using //
				m_szParse.ReplaceStr("\\", "/");
				int pos = m_szParse.RevFind('/');
				if (pos >= 0) {
					m_szParse.SubString(m_szFileName, pos + 1);
					m_szParse.SubString(m_szPathName, 0, pos + 1);
				}
				else
				{
					m_szFileName = m_szParse;
					m_szPathName = "";
				}

				//find "[projectfiles]" //all files.
				//find "[files]" //open files.

				LoadAgkProjectFile((char *) m_szParse.GetStr());
				iSaveAgkProjectFiles = 0;
				bool bWatchVariablesSaved = false;
				bool bHTMLSaved = false;
				bool bAPKSaved = false;
				bool bIPASaved = false;
				bool bOpenFilesSaved = false;

				if (pref.iRememberTabOrder)
				{
					//Sort saveProjectFiles = searchProject->m_pFirstFile; by tab order.
					void sort_project_files_order(cProjectItem * searchProject);
					sort_project_files_order(searchProject);
					saveProjectFiles = searchProject->m_pFirstFile;
				}


				for (int i = 0; i < iAgkProjectFiles; i++) {

					sSaveAgkProjectFiles[iSaveAgkProjectFiles] = sAgkProjectFiles[i];

					if ( strstr( sAgkProjectFiles[i].GetStr() , "[projectfiles]")  ) {
						//Find next section.
						while (++i < iAgkProjectFiles && !strstr(sAgkProjectFiles[i].GetStr(), "[") );
						i--;
						//Write out all our project files.
						//FILE_0=main.agc;-1
						saveProjectFiles = searchProject->m_pFirstFile;
						int textcount = 0;
						char textdest[2048];
						iSaveAgkProjectFiles++; // Keep []
						while (saveProjectFiles) {
							uString tmpname = saveProjectFiles->m_sName;

							tmpname.ReplaceStr("/", "%2F");
							tmpname.ReplaceStr(" ", "%20");
							tmpname.ReplaceStr("(", "%28");
							tmpname.ReplaceStr(")", "%29");
							tmpname.ReplaceStr("-", "%2D");
							//tmpname.ReplaceStr(".", "%2E");
							tmpname.ReplaceStr(":", "%3A");
							tmpname.ReplaceStr("[", "%5B");
							tmpname.ReplaceStr("]", "%5D");

							sprintf(textdest, "FILE_%d=%s;-1\n", textcount++, tmpname.GetStr() );
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;
							saveProjectFiles = saveProjectFiles->m_pNext;
						}
						sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n";

					}
					//ipa settings.
					else if (strstr(sAgkProjectFiles[i].GetStr(), "[ipa_settings]")) {
						if (searchProject) {
							//Find next section.
							while (++i < iAgkProjectFiles && sAgkProjectFiles[i][0] != '[');
							if (i < iAgkProjectFiles)
								i--;
							iSaveAgkProjectFiles++; // Keep []

							char textdest[2048];
							uString newPath;
							sprintf(textdest, "app_name=%s\n", searchProject->ipa_app_name);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->ipa_app_icon_path;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "app_icon_path=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->ipa_build_number;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "build_number=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->ipa_version_number;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "version_number=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "device_type=%d\n", searchProject->ipa_device_type);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "facebook_id=%s\n", searchProject->ipa_facebook_id);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "orientation=%d\n", searchProject->ipa_orientation);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->ipa_output_path;
							newPath.ReplaceStr("\\", "\\\\");
							newPath.ReplaceStr("%", "%25");
							sprintf(textdest, "output_path=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "uses_ads=%d\n", searchProject->ipa_uses_ads);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "url_scheme=%s\n", searchProject->ipa_url_scheme);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "deep_link=%s\n", searchProject->ipa_deep_link);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "admob_app_id=%s\n", searchProject->ipa_admob_app_id);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "snapchat_client_id=%s\n", searchProject->ipa_snapchat_client_id);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->ipa_splash_logo;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "splash_logo=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->ipa_firebase_config_path;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "firebase_config_path=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->ipa_splash_color;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "splash_color=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->ipa_prov_profile_path;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "prov_profile_path=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n";
							bIPASaved = true;
						}
					}
					//[html5_settings]
					else if (strstr(sAgkProjectFiles[i].GetStr(), "[html5_settings]")) {
						if (searchProject) {
							//Find next section.
							while (++i < iAgkProjectFiles && sAgkProjectFiles[i][0] != '[');
							if (i < iAgkProjectFiles)
								i--;
							iSaveAgkProjectFiles++; // Keep []

							char textdest[2048];
							uString newPath;

							sprintf(textdest, "commands_used=%d\n", pCurrentSelectedProject->html_commands_used);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;
							sprintf(textdest, "dynamic_memory=%d\n", pCurrentSelectedProject->html_dynamic_memory);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->html_output_path;
							newPath.ReplaceStr("\\", "\\\\");
							newPath.ReplaceStr("%", "%25");
							sprintf(textdest, "output_path=%s\n", newPath.GetStr());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "\n");
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							bAPKSaved = true;

						}
					}
					//[apk_settings]
					else if (strstr(sAgkProjectFiles[i].GetStr(), "[apk_settings]")) {
						if (searchProject) {
							//Find next section.
							while (++i < iAgkProjectFiles && sAgkProjectFiles[i][0] != '[');
							if (i < iAgkProjectFiles)
								i--;
							iSaveAgkProjectFiles++; // Keep []

							char textdest[2048];
							uString newPath;
							sprintf(textdest, "alias=%s\n", searchProject->apk_alias);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->apk_new_icon_path;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "app_new_icon_path=%s\n", newPath.GetStr() );
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->apk_icon_path;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "app_icon_path=%s\n", newPath.GetStr() );
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->apk_notif_icon_path;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "notif_icon_path=%s\n", newPath.GetStr() );
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;
							
							
							sprintf(textdest, "app_name=%s\n", searchProject->apk_app_name);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;
							sprintf(textdest, "app_type=%d\n", searchProject->apk_app_type);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "game_circle_api_key=%s\n", searchProject->game_circle_api_key);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->apk_keystore_path;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "keystore_path=%s\n", newPath.GetStr() );
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "orientation=%d\n", searchProject->apk_orientation);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							newPath = searchProject->apk_output_path;
							newPath.ReplaceStr("\\", "\\\\");
							newPath.ReplaceStr("%", "%25");
							sprintf(textdest, "output_path=%s\n", newPath.GetStr() );
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "ouya_icon_path=%s\n", searchProject->apk_ouya_icon_path);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "package_name=%s\n", searchProject->apk_package_name);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "permission_flags=%d\n", searchProject->apk_permission_flags);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "play_app_id=%s\n", searchProject->apk_play_app_id);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "admob_app_id=%s\n", searchProject->apk_admob_app_id);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "snapchat_client_id=%s\n", searchProject->apk_snapchat_client_id);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "sdk_version=%d\n", searchProject->apk_sdk_version);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "version_name=%s\n", searchProject->apk_version_name);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "version_number=%d\n", searchProject->apk_version_number);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;
							
							newPath = searchProject->apk_firebase_config_path;
							newPath.ReplaceStr("\\", "\\\\");
							sprintf(textdest, "firebase_config_path=%s\n", newPath.GetStr() );
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "url_scheme=%s\n", searchProject->apk_url_scheme);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "deep_link=%s\n", searchProject->apk_deep_link);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "arcore=%d\n", searchProject->apk_arcore);
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							sprintf(textdest, "\n");
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;

							bAPKSaved = true;
						}
					}
					else if (strstr(sAgkProjectFiles[i].GetStr(), "[watchvariables]")) {
						//Find next section.
						while (++i < iAgkProjectFiles && sAgkProjectFiles[i][0] != '[' );
						if(i < iAgkProjectFiles)
							i--;
						
						iSaveAgkProjectFiles++; // Keep []

						//Add project watch variables.
						//sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n[watchvariables]\n";
						for (auto it = searchProject->debugWatch.begin(); it != searchProject->debugWatch.end(); ++it)
						{
							if (!it->second.bHoverVariable) {
								sSaveAgkProjectFiles[iSaveAgkProjectFiles] = "watch_variable=";
								sSaveAgkProjectFiles[iSaveAgkProjectFiles].Append(it->first.c_str());
								sSaveAgkProjectFiles[iSaveAgkProjectFiles].Append(";\n");
								iSaveAgkProjectFiles++;
							}
						}
						sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n";
						bWatchVariablesSaved = true;

					}
					else if (strstr(sAgkProjectFiles[i].GetStr(), "[files]")) {

						//Make sure we have newline between sections.
						//if(i > 2 && !(sAgkProjectFiles[i - 1][0] == '\n' && sAgkProjectFiles[i - 1][1] == 0 )) {
						//	sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n";
						//	sSaveAgkProjectFiles[iSaveAgkProjectFiles] = sAgkProjectFiles[i];
						//}

						//Find next section.
						while (++i < iAgkProjectFiles && !strstr(sAgkProjectFiles[i].GetStr(), "["));
						i--;
						//Write out all our project files.
						//FILE_0=main.agc;-1
						saveProjectFiles = searchProject->m_pFirstFile;
						int textcount = 0;
						char textdest[2048];
						iSaveAgkProjectFiles++; // Keep []

						//FILE_NAME_0=0;main.agc;8443;0 // 8443 = cursor position.

						while (saveProjectFiles) {
							if (saveProjectFiles->m_bOpenForEdit && saveProjectFiles->m_editor && saveProjectFiles->m_editor->bEditorVisible ) {
								uString tmpname = saveProjectFiles->m_sName;
								tmpname.ReplaceStr("/", "%2F");
								tmpname.ReplaceStr(" ", "%20");
								tmpname.ReplaceStr("(", "%28");
								tmpname.ReplaceStr(")", "%29");
								tmpname.ReplaceStr("-", "%2D");
								//tmpname.ReplaceStr(".", "%2E");
								tmpname.ReplaceStr(":", "%3A");
								tmpname.ReplaceStr("[", "%5B");
								tmpname.ReplaceStr("]", "%5D");
								int iEditorActive = 0;
								if(m_ActiveEditor && saveProjectFiles->m_editor && saveProjectFiles->m_editor== m_ActiveEditor )
									iEditorActive = 1;

								if (saveProjectFiles->m_editor)
									sprintf(textdest, "FILE_NAME_%d=%d;%s;%d;%d\n", textcount, iEditorActive, tmpname.GetStr(), saveProjectFiles->m_editor->GetCursorPosition().mLine, textcount);
								else
									sprintf(textdest, "FILE_NAME_%d=%d;%s;0;%d\n", textcount, iEditorActive, tmpname.GetStr(), textcount);
								textcount++;
								sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;
							}
							saveProjectFiles = saveProjectFiles->m_pNext;
						}
						sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n";
						bOpenFilesSaved = true;
					}
					else {
						iSaveAgkProjectFiles++;
					}
				}

				if (!bOpenFilesSaved) {
					//Create new [files] section.
					sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n[files]\n";
					saveProjectFiles = searchProject->m_pFirstFile;
					int textcount = 0;
					char textdest[2048];
					while (saveProjectFiles) {
						if (saveProjectFiles->m_bOpenForEdit && saveProjectFiles->m_editor && saveProjectFiles->m_editor->bEditorVisible ) {
							uString tmpname = saveProjectFiles->m_sName;
							tmpname.ReplaceStr("/", "%2F");
							tmpname.ReplaceStr(" ", "%20");
							tmpname.ReplaceStr("(", "%28");
							tmpname.ReplaceStr(")", "%29");
							tmpname.ReplaceStr("-", "%2D");
							//tmpname.ReplaceStr(".", "%2E");
							tmpname.ReplaceStr(":", "%3A");
							tmpname.ReplaceStr("[", "%5B");
							tmpname.ReplaceStr("]", "%5D");
							int iEditorActive = 0;
							if (m_ActiveEditor && saveProjectFiles->m_editor && saveProjectFiles->m_editor == m_ActiveEditor)
								iEditorActive = 1;

							if (saveProjectFiles->m_editor)
								sprintf(textdest, "FILE_NAME_%d=%d;%s;%d;%d\n", textcount, iEditorActive, tmpname.GetStr(), saveProjectFiles->m_editor->GetCursorPosition().mLine, textcount);
							else
								sprintf(textdest, "FILE_NAME_%d=%d;%s;0;%d\n", textcount, iEditorActive, tmpname.GetStr(), textcount);
							textcount++;
							sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = textdest;
						}
						saveProjectFiles = saveProjectFiles->m_pNext;
					}
					sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n";

				}

				if (!bWatchVariablesSaved) {
					//Create new [watchvariables] section.
					sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n[watchvariables]\n";
					for (auto it = searchProject->debugWatch.begin(); it != searchProject->debugWatch.end(); ++it)
					{
						if (!it->second.bHoverVariable) {
							sSaveAgkProjectFiles[iSaveAgkProjectFiles] = "watch_variable=";
							sSaveAgkProjectFiles[iSaveAgkProjectFiles].Append(it->first.c_str());
							sSaveAgkProjectFiles[iSaveAgkProjectFiles].Append(";\n");
							iSaveAgkProjectFiles++;
						}
					}
					sSaveAgkProjectFiles[iSaveAgkProjectFiles++] = "\n";
					bWatchVariablesSaved = true;

				}
				SaveAgkProjectFile((char *)m_szParse.GetStr());

			}
			searchProject = searchProject->m_pNext;
		}
	}
	//nextProject->m_sProjectFileFullPath = cFileSelected;

}
void LoadProjectFile( char * cFileSelected)
{
	cProjectItem * searchProject;
	int projectloaded = 0;
	if (allProjects) {
		//Check if project already open.
		searchProject = allProjects;
		while (searchProject) {
			if ( strcmp( searchProject->m_sProjectFileFullPath.GetStr() , cFileSelected ) == 0 ) {
				//Project already open so just select it.
				pCurrentSelectedProject = searchProject;
				//Find m_pSelectedFolder and set.
				return;
			}
			nextProject = searchProject; // nextProject = last known project.
			searchProject = searchProject->m_pNext;
		}

	}
	cProjectItem * newProject = new cProjectItem();
	uString usProject;

	uString m_szParse, m_szFileName, m_szPathName;
#ifdef AGK_WINDOWS
#endif

	m_szParse = cFileSelected;
	m_szParse.ReplaceStr("\\\\", "//");  //To allow network using //
	m_szParse.ReplaceStr("\\", "/");
	int pos = m_szParse.RevFind('/');
	if (pos >= 0) {
		m_szParse.SubString(m_szFileName, pos + 1);
		m_szParse.SubString(m_szPathName, 0, pos + 1);
	}
	else
	{
		m_szFileName = m_szParse;
		m_szPathName = "";
	}

	FILE *fp;
	fp = fopen(m_szParse.GetStr(), "r");
	if(!fp)
		fp = AGKfopen(m_szParse.GetStr(), "r");

	if (!fp) {
		//Not found ignore.
		return;
	}


	//Check if project name is already in list.
	int largestnewname = 0;
	if (allProjects) {
		//Check if project already open.
		searchProject = allProjects;
		while (searchProject) {
			
			int pos;
			pos = nextProject->m_sProjectFile.FindStr("(");
			if (pos > 0) {
				uString tmp;
				nextProject->m_sProjectFile.SubString(tmp, pos + 1, 1);
				if(atoi(tmp.GetStr()) > largestnewname)
					largestnewname = atoi(tmp.GetStr());
			}

			if (strcmp(searchProject->m_sProjectRealFile.GetStr(), m_szFileName.GetStr()) == 0) {
				projectloaded++;
			}
			nextProject = searchProject; // nextProject = last known project.
			searchProject = searchProject->m_pNext;
		}

	}

	uString m_szRealFileName = m_szFileName;
	if (projectloaded > 0) {
		//Project already in list so change name.
		m_szFileName.Append("(");
		if(largestnewname+1 > projectloaded)
			m_szFileName.AppendInt(largestnewname+1);
		else
			m_szFileName.AppendInt(projectloaded);
		m_szFileName.Append(")");
	}
	//m_sProjectFile

	if (allProjects == NULL) {
		allProjects = newProject;
		nextProject = newProject;
	}
	else {
		nextProject->m_pNext = newProject;
		nextProject = nextProject->m_pNext;
	}

	nextProject->m_id = ++uniqID;
	nextProject->m_sProjectFile = m_szFileName;
	nextProject->m_sProjectRealFile = m_szRealFileName;
	nextProject->m_sProjectFileFullPath = cFileSelected;
	nextProject->m_pFirstFile = NULL;
	nextProject->m_pMediaFolder = NULL;
	nextProject->m_bDisplayUnfolded = true;
	nextProject->m_bDisplayCollapsed = false;
	nextProject->m_pNext = NULL;

	pCurrentSelectedProject = nextProject;
	//Find m_pSelectedFolder and set.

	strcpy(pCurrentSelectedProject->apk_alias,"");
	strcpy(pCurrentSelectedProject->apk_new_icon_path, "");
	strcpy(pCurrentSelectedProject->apk_icon_path, "");
	strcpy(pCurrentSelectedProject->apk_notif_icon_path, "");
	strcpy(pCurrentSelectedProject->apk_app_name, "");
	pCurrentSelectedProject->apk_app_type = 0;
	strcpy(pCurrentSelectedProject->game_circle_api_key, "");
	strcpy(pCurrentSelectedProject->apk_keystore_path, "");
	pCurrentSelectedProject->apk_orientation = 0;
	strcpy(pCurrentSelectedProject->apk_output_path, "");
	strcpy(pCurrentSelectedProject->apk_ouya_icon_path, "");
	strcpy(pCurrentSelectedProject->apk_package_name, "");
	pCurrentSelectedProject->apk_permission_flags = 0;
	strcpy(pCurrentSelectedProject->apk_play_app_id, "");
	strcpy(pCurrentSelectedProject->apk_admob_app_id, "");
	strcpy(pCurrentSelectedProject->apk_snapchat_client_id, "");
	pCurrentSelectedProject->apk_sdk_version = 0;
	strcpy(pCurrentSelectedProject->apk_version_name, "");
	pCurrentSelectedProject->apk_version_number = 1;
	strcpy(pCurrentSelectedProject->apk_firebase_config_path, "");
	strcpy(pCurrentSelectedProject->apk_url_scheme, "");
	strcpy(pCurrentSelectedProject->apk_deep_link, "");
	pCurrentSelectedProject->apk_arcore = 0;
	
	strcpy(pCurrentSelectedProject->ipa_app_icon_path, "");
	strcpy(pCurrentSelectedProject->ipa_app_name, "");
	strcpy(pCurrentSelectedProject->ipa_build_number, "");
	pCurrentSelectedProject->ipa_device_type = 0;
	strcpy(pCurrentSelectedProject->ipa_facebook_id, "");
	pCurrentSelectedProject->ipa_orientation = 0;
	strcpy(pCurrentSelectedProject->ipa_output_path, "");
	strcpy(pCurrentSelectedProject->ipa_prov_profile_path, "");
	strcpy(pCurrentSelectedProject->ipa_splash_logo, "");
	strcpy(pCurrentSelectedProject->ipa_splash_color, "");
	pCurrentSelectedProject->ipa_uses_ads = 0;
	strcpy(pCurrentSelectedProject->ipa_version_number, "");
	strcpy(pCurrentSelectedProject->ipa_firebase_config_path, "");
	strcpy(pCurrentSelectedProject->ipa_url_scheme, "");
	strcpy(pCurrentSelectedProject->ipa_deep_link, "");
	strcpy(pCurrentSelectedProject->ipa_admob_app_id, "");
	strcpy(pCurrentSelectedProject->ipa_snapchat_client_id, "");

	strcpy(pCurrentSelectedProject->html_output_path, "");
	
	pCurrentSelectedProject->html_commands_used = 0;
	pCurrentSelectedProject->html_dynamic_memory = 0;

	int inSection = 0;
	cProjectItem::sProjectFiles * m_pCurrentFile, *m_pSearchFile, *m_pSortedFile, *m_ptmpFile;
	m_pCurrentFile = NULL;


	std::vector<std::string> sorted_project_files;

	char fpTmp[2048];
	//fp = fopen(m_szParse.GetStr(), "r");
	if (fp) {
		while (!feof(fp)) {
			uString m_szLeft, m_szCommand , m_szRest , m_szFile, m_szPar , m_szPar2;
			if (fgets(fpTmp, MAX_PATH, fp)) {
				//FILE_0=main.agc;-1
				//m_szParse = fpTmp;
				uString coded = fpTmp;
				//coded.ReplaceStr("%","%25");

				agk::HTTPDecode(coded.GetStr(), m_szParse);

				m_szCommand = m_szParse;
				if (strncmp(m_szCommand.GetStr(), "[apk_settings]", 14) == 0) {
					inSection = 1;
				}
				if (strncmp(m_szCommand.GetStr(), "[ipa_settings]", 14) == 0) {
					inSection = 2;
				}
				if (strncmp(m_szCommand.GetStr(), "[html5_settings]", 16) == 0) {
					inSection = 3;
				}

				if (strncmp(m_szCommand.GetStr(), "[watchvariables]", 16) == 0) {
					inSection = 4;
				}
				if (strncmp(m_szCommand.GetStr(), "[files]", 7) == 0) {
					inSection = 5;
				}
				if (strncmp(m_szCommand.GetStr(), "[projectfiles]", 14) == 0) {
					inSection = 6;
				}

				m_szCommand = "";
				int pos = m_szParse.Find('=');
				if (pos >= 0) {	
					m_szParse.SubString(m_szCommand, 0, pos);
					m_szParse.SubString(m_szRest, pos + 1);

					int pos = m_szRest.Find(';');
					if (pos >= 0) {
						m_szRest.SubString(m_szPar, pos + 1);
						m_szRest.SubString(m_szFile, 0, pos);
					}
					else {
						m_szFile = m_szRest;
					}

					if (inSection == 3) //HTML5
					{
						m_szRest.ReplaceStr("\n", "");
						m_szRest.ReplaceStr("\r", "");

						if (strncmp(m_szCommand.GetStr(), "commands_used", 13) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->html_commands_used = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "dynamic_memory", 14) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->html_dynamic_memory = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "output_path", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->html_output_path, m_szRest.GetStr());
						}

					}
					if (inSection == 2) {
						m_szRest.ReplaceStr("\n", "");
						m_szRest.ReplaceStr("\r", "");

						if (strncmp(m_szCommand.GetStr(), "app_icon_path", 13) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_app_icon_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "app_name", 8) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_app_name, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "build_number", 12) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_build_number, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "version_number", 14) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_version_number, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "device_type", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->ipa_device_type = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "facebook_id", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_facebook_id, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "orientation", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->ipa_orientation = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "output_path", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_output_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "uses_ads", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->ipa_uses_ads = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "url_scheme", 10) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_url_scheme, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "deep_link", 9) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_deep_link, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "admob_app_id", 9) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_admob_app_id, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "snapchat_client_id", 9) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_snapchat_client_id, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "splash_logo", 16) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_splash_logo, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "firebase_config_path", 20) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_firebase_config_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "splash_color", 15) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_splash_color, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "prov_profile_path", 17) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->ipa_prov_profile_path, m_szRest.GetStr());
						}
					}



					if (inSection == 1) {
						m_szRest.ReplaceStr("\n", "");
						m_szRest.ReplaceStr("\r", "");
						if (strncmp(m_szCommand.GetStr(), "alias", 5) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_alias, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "app_new_icon_path", 13) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_new_icon_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "app_icon_path", 13) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_icon_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "notif_icon_path", 15) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_notif_icon_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "app_name", 8) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_app_name, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "app_type", 8) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->apk_app_type = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "game_circle_api_key", 18) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->game_circle_api_key, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "keystore_path", 13) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_keystore_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "orientation", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->apk_orientation = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "output_path", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_output_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "ouya_icon_path", 14) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_ouya_icon_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "package_name", 12) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_package_name, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "permission_flags", 16) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->apk_permission_flags = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "play_app_id", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_play_app_id, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "admob_app_id", 12) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_admob_app_id, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "snapchat_client_id", 12) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_snapchat_client_id, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "sdk_version", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->apk_sdk_version = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "version_name", 11) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_version_name, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "version_number", 14) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->apk_version_number = atoi(m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "firebase_config_path", 20) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_firebase_config_path, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "url_scheme", 10) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_url_scheme, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "deep_link", 9) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							strcpy(pCurrentSelectedProject->apk_deep_link, m_szRest.GetStr());
						}
						if (strncmp(m_szCommand.GetStr(), "arcore", 6) == 0) {
							m_szRest.ReplaceStr("\\\\", "\\"); m_szRest.ReplaceStr("\n", "");
							pCurrentSelectedProject->apk_arcore = atoi(m_szRest.GetStr());
						}
					}

					m_szCommand.SubString(m_szLeft, 0, 5);
					//WATCH_VARIABLE:
					if (strncmp(m_szCommand.GetStr(), "watch_variable", 14) == 0) {
						//Open files.
						if (pos >= 0) {
							uString szTrim = m_szFile;
							szTrim.ReplaceStr(" ", "");
							szTrim.ReplaceStr("\t", "");
							szTrim.ReplaceStr("\n", "");
							WatchValues wtmp;
							wtmp.bHoverVariable = false;
							strcpy(wtmp.value, "na");
							nextProject->debugWatch.insert(std::make_pair(std::string(szTrim.GetStr()), wtmp));

						}
					}
					//nextProject->debugWatch

					//FILE_NAME_0
					if ( strncmp(m_szCommand.GetStr(), "FILE_NAME_" , 10 ) == 0 ) {
						//Open files.
						int pos = m_szPar.Find(';');
						if (pos >= 0) {
							bool bOpenWithFocus = atoi(m_szFile);
							m_szPar.SubString(m_szPar2, 0 , pos );
							uString szRest,szLineNo;
							m_szPar.SubString(szRest, pos+1);
							int pos2 = szRest.Find(';');
							int gotoline = 0;
							if (pos2 > 0) {
								szRest.SubString(szLineNo, 0, pos2);
								gotoline = atoi(szLineNo);
							}
							//m_szPar2 = main.agc
							//Open it in editor.
							//m_bOpenForEdit
							if (nextProject->m_pFirstFile) {
								m_pSearchFile = nextProject->m_pFirstFile;
								while (m_pSearchFile) {
									if (strcmp(m_szPar2.GetStr(), m_pSearchFile->m_sName.GetStr()) == 0) {
										
										vTextEditorInit(m_pSearchFile->m_editor, (char *) m_pSearchFile->m_sFullPath.GetStr());
										m_pSearchFile->m_bOpenForEdit = true;

										if (pref.iRememberTabOrder)
										{
											//Open it and dock it , to get correct tab order.
											bool vTextEditor(char *winname, TextEditor * m_editor, char * cName, char * cPath, bool bUseSaveAs = false, cProjectItem::sProjectFiles * m_pCurFile = NULL);
											vTextEditor((char *)m_pSearchFile->m_sEditName.GetStr(), m_pSearchFile->m_editor, (char*)m_pSearchFile->m_sName.GetStr(), (char*)m_pSearchFile->m_sFullPath.GetStr(), m_pSearchFile->m_bUseSaveAs);
										}

										if (bOpenWithFocus) {
											cNextWindowFocus = (char *)m_pSearchFile->m_sEditName.GetStr();
											m_ActiveEditor = m_pSearchFile->m_editor;
											bFocusActiveEditor = true;
										}

										if (gotoline > 0) {
											if (gotoline < m_pSearchFile->m_editor->GetTotalLines())
											m_pSearchFile->m_editor->SetCursorPosition(TextEditor::Coordinates(gotoline, 0));
											m_pSearchFile->m_editor->mScrollToCursor = true;
										}
										//refresh_gui_docking = 1; // not needed any more.
										break;
									}
									m_pSearchFile = m_pSearchFile->m_pNext;
								}
							}
						}
					}
					if (m_szLeft.CompareTo("FILE_") == 0 ) {
						int pos = m_szCommand.Find('_');
						if (pos >= 0) {
							uString tmp;
							m_szCommand.SubString(tmp, pos + 1);
							if (tmp[0] == '0' || atoi(tmp.GetStr()) > 0) {
								//File ok create project file.
								if (m_pCurrentFile == NULL) {
									nextProject->m_pFirstFile = new cProjectItem::sProjectFiles();
									nextProject->m_pFirstFile->m_sName = m_szFile;
									nextProject->m_pFirstFile->m_sFullPath = ""; //"raw:";
									nextProject->m_pFirstFile->m_sFullPath.Append(m_szPathName);
									nextProject->m_pFirstFile->m_sFullPath.Append(m_szFile);
									nextProject->m_pFirstFile->m_bUseSaveAs = false;
									nextProject->m_pFirstFile->m_editor = new TextEditor();

									if (m_szFile.RevFindStr(".scene", 1) > 0) {
										nextProject->m_pFirstFile->m_scene = new MediaScene();
										nextProject->m_pFirstFile->m_scene->m_sSceneFullPath = m_szPathName;
										nextProject->m_pFirstFile->m_scene->m_sSceneFullPath.Append(m_szFile);
									}
									else
										nextProject->m_pFirstFile->m_scene = NULL;
									nextProject->m_pFirstFile->m_editor->quiteditor = false;
									nextProject->m_pFirstFile->m_id = ++uniqID;
									nextProject->m_pFirstFile->m_bOpenForEdit = false;
									nextProject->m_pFirstFile->m_sProjectName = nextProject->m_sProjectFile;
									nextProject->m_pFirstFile->m_sEditName = m_szFile;

									uString newTabname = nextProject->m_pFirstFile->m_sEditName;
									uString newEditName = nextProject->m_pFirstFile->m_sEditName;

									int ntb_pos = newTabname.RevFindStr("/");
									if (ntb_pos <= 0)
										ntb_pos = newTabname.RevFindStr("\\");
									if (ntb_pos > 0) {
										newTabname.SubString(newEditName, ntb_pos + 1);
										if (pref.bRemovePathFromTabName)
											newTabname.SubString(nextProject->m_pFirstFile->m_sEditName, ntb_pos + 1);
									}
/*
									if (pref.bRemovePathFromTabName) {
										uString newTabname = nextProject->m_pFirstFile->m_sEditName;
										int ntb_pos = newTabname.RevFindStr("/");
										if (ntb_pos <= 0)
											ntb_pos = newTabname.RevFindStr("\\");
										if (ntb_pos > 0)
											newTabname.SubString(nextProject->m_pFirstFile->m_sEditName, ntb_pos + 1);
									}
*/
									nextProject->m_pFirstFile->m_sEditName.Append("##");
									nextProject->m_pFirstFile->m_sEditName.Append(nextProject->m_sProjectFile);

									newEditName.Append("##");
									newEditName.Append(nextProject->m_sProjectFile);

									sorted_project_files.push_back(newEditName.GetStr());

									m_pCurrentFile = nextProject->m_pFirstFile;
									m_pCurrentFile->m_pNext = NULL;
									m_pCurrentFile->m_pSortedNext = NULL;
								}
								else {
									if (m_pCurrentFile) {
										uString szShort = m_szFile;
										m_pCurrentFile->m_pNext = new cProjectItem::sProjectFiles();
										m_pCurrentFile->m_pSortedNext = m_pCurrentFile->m_pNext;
										m_pCurrentFile = m_pCurrentFile->m_pNext;
										m_pCurrentFile->m_sName = szShort;
										m_pCurrentFile->m_sFullPath = ""; //"raw:";
										m_pCurrentFile->m_sFullPath.Append(m_szPathName);
										if (m_szFile.Find(':') > 0) //relative ?
											m_pCurrentFile->m_sFullPath = m_szFile; // not relative
										else
											m_pCurrentFile->m_sFullPath.Append(m_szFile);
										m_pCurrentFile->m_bUseSaveAs = false;
										m_pCurrentFile->m_id = ++uniqID;
										m_pCurrentFile->m_editor = new TextEditor();
										if (m_szFile.RevFindStr(".scene", 1) > 0) {
											m_pCurrentFile->m_scene = new MediaScene();
											m_pCurrentFile->m_scene->m_sSceneFullPath = m_pCurrentFile->m_sFullPath;
										}
										else
											m_pCurrentFile->m_scene = NULL;
										m_pCurrentFile->m_editor->quiteditor = false;
										m_pCurrentFile->m_bOpenForEdit = false;
										m_pCurrentFile->m_sProjectName = nextProject->m_sProjectFile;
										m_pCurrentFile->m_sEditName = szShort;

										uString newTabname = m_pCurrentFile->m_sEditName;
										uString newEditName = m_pCurrentFile->m_sEditName;

										int ntb_pos = newTabname.RevFindStr("/");
										if (ntb_pos <= 0)
											ntb_pos = newTabname.RevFindStr("\\");
										if (ntb_pos > 0) {
											newTabname.SubString(newEditName, ntb_pos + 1);
											if (pref.bRemovePathFromTabName)
												newTabname.SubString(m_pCurrentFile->m_sEditName, ntb_pos + 1);
										}

										m_pCurrentFile->m_sEditName.Append("##");
										m_pCurrentFile->m_sEditName.Append(nextProject->m_sProjectFile);

										newEditName.Append("##");
										newEditName.Append(nextProject->m_sProjectFile);

										sorted_project_files.push_back(newEditName.GetStr());

										m_pCurrentFile->m_pNext = NULL;
										m_pCurrentFile->m_pSortedNext = NULL;
									}
								}
							}
						}
					}

				}
			}
		}
		fclose(fp);


		if (!pref.iRememberTabOrder)
		{
			//Sort files inside project.
			if (sorted_project_files.size() >= 1) {

				m_pSortedFile = NULL;
				m_ptmpFile = nextProject->m_pFirstFile;
				std::sort(sorted_project_files.begin(), sorted_project_files.end(), NoCaseLess);
				for (std::vector<std::string>::iterator it = sorted_project_files.begin(); it != sorted_project_files.end(); ++it) {

					m_pSearchFile = m_ptmpFile;
					while (m_pSearchFile) {
						if (it->size() > 0) {

							uString newTabname = m_pSearchFile->m_sEditName;
							uString newEditName = m_pSearchFile->m_sEditName;
							int ntb_pos = newTabname.RevFindStr("/");
							if (ntb_pos <= 0)
								ntb_pos = newTabname.RevFindStr("\\");
							if (ntb_pos > 0) {
								newTabname.SubString(newEditName, ntb_pos + 1);
							}

							if (strcmp(it->c_str(), newEditName.GetStr()) == 0) {
								if (m_pSortedFile == NULL) {
									//first entry.
									nextProject->m_pFirstFile = m_pSearchFile;
									m_pSortedFile = m_pSearchFile;
									m_pSortedFile->m_pNext = NULL;
								}
								else {
									m_pSortedFile->m_pNext = m_pSearchFile;
									m_pSortedFile = m_pSearchFile;
									m_pSortedFile->m_pNext = NULL;
								}
							}
						}

						m_pSearchFile = m_pSearchFile->m_pSortedNext;
					}

				}

				//Reorder m_pSortedNext so it can be used again.
				m_ptmpFile = nextProject->m_pFirstFile;
				m_pSearchFile = m_ptmpFile;
				while (m_pSearchFile) {
					m_pSearchFile->m_pSortedNext = m_pSearchFile->m_pNext;
					m_pSearchFile = m_pSearchFile->m_pNext;
				}

			}
			//agk::Print("");
			sorted_project_files.clear();
		}

	}

}


#ifdef AGK_WINDOWS

#include <windows.h>
#include <shobjidl.h> 

char * testopenfiledialog(char * path)
{
	HRESULT hr;
	IFileOpenDialog *pFileOpen;

	// Create the FileOpenDialog object.
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

	if (SUCCEEDED(hr))
	{
		// Show the Open dialog box.
		//pFileOpen->SetDefaultFolder();
		hr = pFileOpen->Show(NULL);

		// Get the file name from the dialog box.
		if (SUCCEEDED(hr))
		{
			IShellItem *pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

				// Display the file name to the user.
				if (SUCCEEDED(hr))
				{
					char *str = new char[wcslen(pszFilePath) + 1];
					wcstombs(str, pszFilePath, wcslen(pszFilePath));
					CoTaskMemFree(pszFilePath);
					return str;
//					MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
				}
				pItem->Release();
			}
		}
		pFileOpen->Release();
	}
	CoUninitialize();

	char *str = new char[1];
	*str = '\0';
	return str;
}
#endif

