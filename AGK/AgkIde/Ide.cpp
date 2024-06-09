//PE: AGK IDE.


//#define ADDIMGUIDEMO
//#define RENDERTARGET
//#define DISABLESAVE

#define NASSUPPORT

#ifndef _chdir
	#define _chdir chdir
#endif

#ifndef _getcwd
	#define _getcwd getcwd
#endif

// Includes
#include "Ide.h"
#include "agk.h"
#include "gui.h"
#include "files.h"
#include "project.h"
#include "TextEditor.h"
#include "2DScene.h"

#ifdef ADDIMGUIDEMO
#ifndef DEMOLOAD
#define DEMOLOAD
#include "imgui/imgui_demo.cpp"
#endif
#endif
#ifdef AGK_WINDOWS
#include <direct.h>
#include "shlobj.h"
#endif

// Platform includes.
#ifdef AGK_WINDOWS
	#include "imgui/imgui_impl_agk.cpp"
#endif

#ifdef AGK_MACOS
	#include "imgui/imgui_impl_agk_glfw.cpp"
	#include "imgui/imgui_impl_agk_mac.cpp"
	#include <dirent.h>
#endif

#ifdef AGK_LINUX
	//PE: could also work on linux , not tested.
	#include "imgui/imgui_impl_agk_glfw.cpp"
	#include "imgui/imgui_impl_agk_mac.cpp"
#endif
#ifdef AGK_LINUX
	#include <gtk/gtk.h>
#endif

#include "imgui/imgui_memory_editor.h"

// Namespace
using namespace AGK;

app App;

extern Preferences pref;

// Functions
bool vTextEditor(char *winname, TextEditor * m_editor, char * cName, char * cPath, bool bUseSaveAs = false, cProjectItem::sProjectFiles * m_pCurFile = NULL);
cProjectItem::sProjectFiles * FindFileFromEditor(TextEditor * m_ActiveEditor);
cProjectItem::sProjectFiles * FindFileFromEditorMatchSource(TextEditor * m_FindActiveEditor);
static void displayAssetsBrowser(const char* windowsname, char *path);
cFolderItem * ProcessFolderFiles(cFolderItem * m_pSelectedFolder, const char * cpFilter,bool bDisplayAll = false);
bool ProcessPreviewFile(cFolderItem::sFolderFiles * selectedfile);
void processproperties(int iProperties_type);
void isProjectInAssetsBrowser(void);
void setVisibleProjectsInAssetBrowser(void);
float RenderRotate3DObject(int obj, int rendertarget, int zoom = 0,bool rotatex = false, bool rotatey = true, bool rotatez = false);
void FixAndCenterObjectPivot(int obj);
int pecasepos(const char *arg1, const char *arg2);

// Global variables.
ImGuiViewport* viewport;
cProjectItem * firstProjects = NULL;
cFolderItem *m_pCurrentFolder = NULL;
cFolderItem *m_pClassicDLCFolder = NULL;
cFolderItem *m_pStartFolder = NULL;
cFolderItem::sFolderFiles *m_pSelectedFile = NULL;
cFolderItem::sFolderFiles *pDragDropFile = NULL;
cProjectItem * pCurrentSelectedProject = NULL;
cProjectItem * pCurrentSelectedProjectState = NULL;
cProjectItem * mCompiledProject = NULL;
cProjectItem * pRepeatRemoveProject = NULL;
cProjectItem * renderingProject;

cFolderItem::sFolderFiles * m_pLastSelectedFile;
ImGuiID dock_main_tabs;
int iFirstPreviewAction = 0;
char cPreviewFont[MAX_PATH];
ImFont* previewFont = 0;
bool bPreviewFontLoaded = false;
bool bRepeatRemoveProjectAll = false;

char *pDragDropText = NULL;
float toOne = 1.0 / 256.0;
float lastKeyTime = 0;
int AgkClearColor[4];
bool compiler_done = false;
bool create_new_project = false;
char * cNextWindowFocus = NULL;
uString szNextWindowFocus;
bool bFocusActiveEditor = false;
int cNextWindowFocusCount = 0;
uString usAssetsFolder;
int playmusic_icon; 
int folder_icon;
int object3d_icon;
int codeglsl_icon;
int agcfile_icon;
int agkfile_icon;
int luafile_icon;
int directx_icon;
int agkbutton_image;
int agkbuttondown_image;
#ifdef TRIALVERSIONEXPIRES
int trial_logo = 0;
#endif
bool bRefreshNextFolder = false;
static int generic_3d = 0;

int toolbar_size;
int iToolbarImages[30]; //For toolbar icons.
int iToolbarImagesDown[30]; //For toolbar icons.
bool bToolbarImagesState[30]; //For toolbar icons.
const char * iToolbarText[30]; //For toolbar icons tooltips.

char customFolders[10][MAX_PATH];
int iCustomFolders = 0;
static bool modal_window_active = false;
bool first_editor_processed = true;
int game_is_running = 0;
int debug_is_running = 0;
int remote_debug_is_running = 0;
bool bBreakJustStarted = true;
int keyboard_layout = 0;
static int iWordCount = 0;
static int delaydialog = 0;
static bool bMemTextOnly = false;
int broadcast_is_running = 0;
bool bRemote_broadcast_is_running = 0;
bool broadcaster_initialized = false;
int iProperties_type = 0;
int iProperties_id = 0;
int iRenderedScenes = 0;
bool dlc_initialized = false;
//bool bUseDockFamily = false;
//ImGuiDockFamily editorfamily;
bool bAllProjectsHasBeenProcessed = false;

int changefonttype = 0;
TextEditor * m_ActiveEditor = NULL;
MediaScene * m_ActiveScene = NULL;
TextEditor * m_CompiledEditor = NULL;
int iDeviceWidth = 1920, iDeviceHeight = 1080;
int renderTarget = 0;
int iPreviewRenderTarget = 0;
bool renderTargetActive = true;
static bool initial_setup = true;
static bool quit_app = 0;
char startupFolder[MAX_PATH];
char appFolder[MAX_PATH];
char defaultWriteFolder[MAX_PATH];
char cSteamFolder[MAX_PATH];
ImVec2 render_target_window_size;
ImVec2 render_target_window_pos;
bool agk_focus;
bool agkfullscreen = 0;
int agkOldDeviceWeight = 0;
int agkOldDeviceHeight = 0;
float agkFullscreenTimer = 0;
bool bAgkFullscreenTrigger = false;

ImGuiWindow* agk_window;
bool ide_sleeping = false;
bool ide_force_rendering = true;
bool ide_force_rendering_delayed = true;
int  ide_force_rendering_times = 0;
bool scene_editor_testmode = false;
bool add_new_scene = false;
bool add_new_project_file = false;
char cLayoutFile[MAX_PATH];
int set_default_focus = 0;

//Server News System
int ServerNewsHttp = -1;
bool ServerNewsRunning = false;
bool ServerNewsGetImage = false;
int ServerNewsGetImageId = 0;
float ServerNewsTimer = 0;
char *ServerNewsString = NULL;
uString ServerNews = "";
uString ServerNewsMsg = "";
uString ServerNewsUrl = "";
uString ServerNewsImageUrl = "";
uString ServerNewsUpdate;
bool ServerDisplayNewsWindow = false;

// Android export download
int AndroidExportHTTP = -1;
float AndroidExportHTTPTimer = 0;

bool bDTSPossible = false;

//########################
//#### Record history ####
//########################

TextEditor * m_LastRecordEditor = NULL;
int iLastRecordLine = 0;
int iLastRecordCol = 0;

TextEditor * m_IgnoreLastRecordEditor = NULL;
int iIgnoreLastRecordLine = 0;
int iIgnoreLastRecordCol = 0;

//New bookmark code.
int active_bookmark_list = 1;
int collect_bookmark_list = 0;
int count_bookmark_list = 1;
int current_bookmark = 0;
struct BookmarkValues
{
	int lineno;
	TextEditor * m_InsideEditor;
};
std::unordered_map<std::int32_t, BookmarkValues> AllBookmark[2];
//

int iRecordIndex = 0;
struct RecordValues
{
	int lineno;
	int colno;
	TextEditor * m_RecordEditor; //Remember: Check if it has been closed.
};

RecordValues DelayRecord;
float fDelayRecordTimer = 0;
bool bDelayRecordActive = true;
cProjectItem * pDelayRecordProject = NULL;
std::map<std::int32_t, RecordValues> posRecord;

std::vector<std::string> dlc_files;

ImGuiWindow* edit_area_parent;


// Externals.
extern ImFont* editorfont;
extern ImFont* defaultfont;
extern int iTotalFolders; // we need unique id's for all folder and files.
extern int iTotalFiles; // we need unique id's for all folder and files.
extern cFolderItem *m_pSelectedFolder;
extern cProjectItem * allProjects;
extern cProjectItem::sProjectFiles *allFiles;
extern int refresh_gui_docking;
extern int uniqID; // used for unique ids.
extern int largest_path;
extern bool bAutoContinue;
extern bool bAutoState;
SYMBOLS_MAP symbolsCollectList[2];
extern int iCollectListActive, iCollectListUpdate;
extern int iUpdateIndex, iDelayedUpdateStart;
extern bool bCollectSuggestKeywords; //Make this delayed
extern uString szRecentProjects[MAXRECENTLIST];
extern uString szRecentFiles[MAXRECENTLIST];
extern int iResentProjects, iResentFiles;
extern int trialaction;


//#################################################################
//#### app::Begin setup needed fonts,icons,rendertargets,media ####
//#################################################################

#ifdef AGK_WINDOWS

HWND g_agkhWnd;

void app::Begin(HWND agk_hWnd)
#else
GLFWwindow* g_agkhWnd;
void app::Begin(GLFWwindow* window)
#endif
{
	//Reset preferences if it never have been saved or it is in a old format.
	ResetPreferences();
	pref.launched = 0;
	pref.first_launched = time(0);

//	agk::SetErrorMode(1); //PE: https://github.com/TheGameCreators/AGK-Studio/issues/413#issuecomment-523488699 test
	agk::SetErrorMode(0); //No popup messages.

	strcpy(cPreviewFont, "");

#ifdef AGK_WINDOWS
	//#############################################################
	//#### Setup Windows , keyboard layout / startup folders.  ####
	//#############################################################

	g_agkhWnd = agk_hWnd;
	_getcwd(&startupFolder[0], MAX_PATH);
	strcpy(appFolder, startupFolder);
	if (appFolder[strlen(appFolder) - 1] == '\\') appFolder[strlen(appFolder) - 1] = 0;
	if (appFolder[strlen(appFolder) - 1] == '/') appFolder[strlen(appFolder) - 1] = 0;

	keyboard_layout = 0;
#else

#ifdef AGK_LINUX
	g_agkhWnd = window;
	keyboard_layout = 0;
	getcwd(&startupFolder[0], MAX_PATH); // inside app folder.

	uString startf;
	startf = startupFolder;
	strcpy(appFolder, startupFolder);

	int pos = startf.FindStr("/Contents/Resources", 1);
	if (pos  > 0) {
		appFolder[pos] = 0;
		startf = appFolder;
		pos = startf.RevFindStr("/");
		if (pos > 0)
			appFolder[pos] = 0;
	}
	if (appFolder[strlen(appFolder) - 1] == '\\') appFolder[strlen(appFolder) - 1] = 0;
	if (appFolder[strlen(appFolder) - 1] == '/') appFolder[strlen(appFolder) - 1] = 0;

#else
	g_agkhWnd = window;
	//#############################################################
	//#### Setup Mac , keyboard / startup folders / appfolder. ####
	//#############################################################

	keyboard_layout = 1;
	// /Users/pe/Library/Developer/Xcode/DerivedData/agkinterpreter-gjhnickrwemyxgducpfiypxdpylp/Build/Products/Release/My App Name.app/Contents/Resources
	getcwd(&startupFolder[0], MAX_PATH); // inside app folder.

	uString startf;
	startf = startupFolder;
	strcpy(appFolder, startupFolder);

	int pos = startf.FindStr("/Contents/Resources", 1);
	if (pos  > 0) {
		appFolder[pos] = 0;
		startf = appFolder;
		pos = startf.RevFindStr("/");
		if (pos > 0)
			appFolder[pos] = 0;
	}
	if (appFolder[strlen(appFolder) - 1] == '\\') appFolder[strlen(appFolder) - 1] = 0;
	if (appFolder[strlen(appFolder) - 1] == '/') appFolder[strlen(appFolder) - 1] = 0;

/* for now keep everything inside app
	char szTmp[64];
	sprintf(szTmp, "/proc/%d/exe", getpid());
	int bytes = readlink(szTmp, startupFolder, MAX_PATH);
	if (bytes >= 0)
		startupFolder[bytes] = '\0';
*/
#endif
#endif
	if (startupFolder[strlen(startupFolder) - 1] == '\\') startupFolder[strlen(startupFolder) - 1] = 0;
	if (startupFolder[strlen(startupFolder) - 1] == '/') startupFolder[strlen(startupFolder) - 1] = 0;

	strcpy(defaultWriteFolder, "");
	char *tmp = agk::GetWritePath();
	if (tmp)
		strcpy(defaultWriteFolder, tmp);

	//defaultWriteFolder = 0x01e01a28 "C:/Users/Preben/AppData/Local/AGKApps/TheGameCreators/Ide/"
	GetSteamFolder();

	LoadAgkIdeIni(1);

	CheckPlayBoxFolder();

	//	agk::SetWindowSize(iDeviceWidth, iDeviceHeight, 1);
	agk::SetVirtualResolution(iDeviceWidth, iDeviceHeight);
	agk::UseNewDefaultFonts(1);
	agk::SetPrintSize(20);
	AgkClearColor[0] = 151;
	AgkClearColor[1] = 170;
	AgkClearColor[2] = 204;


	if (pref.iIDEUpdateEventSleep) {
		agk::SetSyncRate(0, 0); // PAUL: drawing on demand instead of at a regular sync rate
	}
	else {
		//Set refresh interval.
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
	//agk::SetClearColor(AgkClearColor[0], AgkClearColor[1], AgkClearColor[2]); // light blue

	//agk::SetScissor(0, 0, 0, 0);
	agk::SetCameraRange(1, 1.0, 100000);

	agk::SetGenerateMipmaps(0); // generate and use mipmaps
	agk::SetAntialiasMode(1); // set anti-aliasing on back buffer.
	//agk::SetImageSavePixels(0) // only GPU has textures. save some memory.
	agk::SetResolutionMode(1); // slower but will look better.

#ifdef AGK_WINDOWS
	usAssetsFolder = "../"; //GetGameGuruFolder();
#else
	usAssetsFolder = "../";
#endif

	if (iDeviceHeight != agk::GetDeviceHeight() || iDeviceWidth != agk::GetDeviceWidth()) {
		iDeviceWidth = agk::GetDeviceWidth();
		iDeviceHeight = agk::GetDeviceHeight();
		agk::SetVirtualResolution(iDeviceWidth, iDeviceHeight);
	}

	playmusic_icon = agk::LoadImage("media/playmusic.png");
	codeglsl_icon = agk::LoadImage("media/codeglsl.png");
	agcfile_icon = agk::LoadImage("media/agcfile.png");
	agkfile_icon = agk::LoadImage("media/agkfile.png");
	luafile_icon = agk::LoadImage("media/luafile.png");
	folder_icon = agk::LoadImage("media/folder.png");
	object3d_icon = agk::LoadImage("media/3dobject.png");
	directx_icon = agk::LoadImage("media/directx.png");
	
	agkbuttondown_image = agk::LoadImage("media/AgkButtonDown.png");
	agkbutton_image = agk::LoadImage("media/AgkButton.png");


	for (int i = 0; i < 20; i++) {
		iToolbarImages[i] = 0;
		iToolbarImagesDown[i] = 0;
	}

	//Load Toolbar icons.
	ChangeIconSet();


/*
media/icons/32-pixels/regular/continue.png
media/icons/32-pixels/regular/settings.png
media/icons/32-pixels/regular/step.png
media/icons/32-pixels/regular/step-out.png
media/icons/32-pixels/regular/step-over.png
media/icons/32-pixels/regular/stop.png
*/


	if (agk::GetImageExists(renderTarget) == 1)
		agk::DeleteImage(renderTarget);
	renderTarget = agk::CreateRenderImage(iDeviceWidth, iDeviceHeight, 0, 0);

	if (agk::GetImageExists(iPreviewRenderTarget) == 1)
		agk::DeleteImage(iPreviewRenderTarget);
	iPreviewRenderTarget = agk::CreateRenderImage(iDeviceWidth, iDeviceHeight, 0, 0);


	agk::SetFolder("");
	agk::SetRawWritePath(agk::GetReadPath());

#ifdef RENDERTARGET
	static int boxobj;
	static int boxmem, boxmem2;
	boxobj = agk::CreateObjectBox(30, 30, 30);
	agk::SetObjectCollisionMode(boxobj, 1); //Enable collision.
	boxmem = agk::CreateMemblockFromObjectMesh(boxobj, 1);
	boxmem2 = agk::CreateMemblockFromObjectMesh(boxobj, 1);
	int numvert = agk::GetMemblockInt(boxmem, 0);
	for( int vindex = 0 ;vindex < numvert  ; vindex++ ) {
		float vx = agk::GetMeshMemblockVertexX(boxmem, vindex);
		float vy = agk::GetMeshMemblockVertexY(boxmem, vindex);
		float vz = agk::GetMeshMemblockVertexZ(boxmem, vindex);
		vx += 40.0;
		agk::SetMeshMemblockVertexPosition(boxmem, vindex, vx, vy, vz);
		vx += 40.0;
		agk::SetMeshMemblockVertexPosition(boxmem2, vindex, vx, vy, vz);
	}
	agk::AddObjectMeshFromMemblock(boxobj, boxmem);
	agk::AddObjectMeshFromMemblock(boxobj, boxmem2);
	agk::SetObjectColor(boxobj, 255, 128, 0, 255);
	agk::SetObjectPosition(boxobj, 0, 50, -100);
	agk::SetObjectMeshCollisionMode(boxobj, 2, 0); //PE: Disable collision on mesh 2 only

	agk::SetCameraPosition(1, 0, 50.0, 0);
	agk::RotateCameraLocalX(1, 7);
	agk::MoveCameraLocalZ(1, -230.0);

	agk::SetSunColor(128, 128, 128);
	agk::SetSunDirection(1.0, -0.75, 0.0);

	agk::CreatePointLight(1, 50, 50, 50, 250, 0, 255, 0);
	agk::CreatePointLight(2, -50, 50, 30, 250, 0, 0, 255);
	agk::CreatePointLight(3, -50, -30, -70, 250, 255, 0, 0);

#endif
	
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); // (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	if (pref.iTabHideDropdown)
		io.ConfigFlags |= ImGuiConfigFlags_DockNoCollapsButton;
	else
		io.ConfigFlags &= ~ImGuiConfigFlags_DockNoCollapsButton;


	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	if (pref.save_layout) {
		sprintf(cLayoutFile, "%sagkide.layout", defaultWriteFolder);
#ifdef AGK_LINUX
		//DebugInfo(cLayoutFile, "tempfolder");
#endif
		io.IniFilename = &cLayoutFile[0]; //Enable saving.
		ImGuiContext& g = *GImGui;
		if (file_exists(cLayoutFile)) {
			refresh_gui_docking = 4; // dont update layout.
		}
	}
	else {
		io.IniFilename = NULL; //Disable saving imgui.ini
	}

	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	if(pref.bUseInternalMousePointer)
		io.MouseDrawCursor = true; //Test

	//io.KeyRepeatDelay = 0.250f; //0.250
	io.KeyRepeatRate = 0.035f; //0.050f

#ifdef AGK_WINDOWS
	ImGui_ImplAGK_Init(agk_hWnd);
#else
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplAGK_Init();
#endif
	myDarkStyle(NULL); //for bordersize,padding ...
	myDefaultStyle(NULL); //additional settings before change.

	if (pref.idePalette == 4) {
		myDarkGreyStyle(NULL);
	}
	else if (pref.idePalette == 3) {
		myLightStyle(NULL);
	}
	else if (pref.idePalette == 2) {
		ImGui::StyleColorsClassic();
	}
	else if (pref.idePalette == 1) {
		myDarkStyle(NULL);
	}
	else {
		if (pref.bEnableSeedStyle) {
			myDefaultStyle(NULL);
		}
		else if (pref.bEnableCustomStyle) {
			CustomStyleColors(NULL); //set custom style colors
		}
	}
	if(pref.bEnableSeedStyle)
		SetSeedStyleColors();

	switch (pref.iLastFontType) {
		case 1:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize, "media/fonts/Hack-Regular.ttf", pref.iEditorFontSize);
			break;
		case 2:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize, "media/fonts/Hack-Bold.ttf", pref.iEditorFontSize);
			break;
		case 3:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize, "media/fonts/Hack-Italic.ttf", pref.iEditorFontSize);
			break;
		case 4:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize, "media/fonts/DejaVuSansMono.ttf", pref.iEditorFontSize);
			break;
		case 5:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize, "media/fonts/DejaVuSansMono-Bold.ttf", pref.iEditorFontSize);
			break;
		default:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize, "media/fonts/Hack-Regular.ttf", pref.iEditorFontSize); //const
			break;
	}


	initial_setup = true;

	//################################
	//#### Locate any Studio DLCs ####
	//################################

	if (!dlc_initialized) {
		//
		uString steamDLC = startupFolder; // cSteamFolder (use for classic) ;
		steamDLC.Append("/DLC/");
		char *oldfirstpath, *ctmp;
		oldfirstpath = agk::GetWritePath();

		if (steamDLC.GetLength() > 0) {
			agk::SetRawWritePath(steamDLC.GetStr());
			ctmp = agk::GetFirstFolder(1);
			while (ctmp && strlen(ctmp) > 0) {
				if (!pestrcasestr(ctmp, "tutorial guide 1") && !pestrcasestr(ctmp, "tutorial guide 2")) {
					dlc_files.push_back(ctmp);
				}
				ctmp = agk::GetNextFolder();
			}
		}

		agk::SetRawWritePath(oldfirstpath);
		if (!dlc_files.empty()) {
			std::sort(dlc_files.begin(), dlc_files.end(), NoCaseLess);
		}
		dlc_initialized = true;
	}

	//##############################
	//#### Lee server code init ####
	//##############################

//	DebugInfo("Begin", "Startup");
#ifdef DIS_AGK_LINUX

	gtk_init_check(NULL, NULL);

	GtkWidget *parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new("Open File",
		NULL,
		action,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Open",
		GTK_RESPONSE_ACCEPT,
		NULL);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		filename = gtk_file_chooser_get_filename(chooser);
		//open_file(filename);
		DebugInfo(filename, "FileName");
		g_free(filename);
	}

	gtk_widget_destroy(dialog);

	DebugInfo("end", "end");
#endif



	char installfile[MAX_PATH];
	sprintf(installfile, "%sinstallcode.dat", defaultWriteFolder);

	char pUniqueCode[33];
	memset(pUniqueCode, 33, 0);
	FILE *file = fopen(installfile, "r");
	if (!file)
		file = AGKfopen(installfile, "r");

	if (file == NULL)
	{
		//Not found - Check if a old version exists.
		uString tmpfile = defaultWriteFolder;
		tmpfile.ReplaceStr("\\", "/");
		tmpfile.ReplaceStr("AGKApps/TheGameCreators/Ide/", "agk/");
		tmpfile.Append("installcode.dat");
		FILE *file = fopen(tmpfile.GetStr(), "r");
		if (!file)
			file = AGKfopen(tmpfile.GetStr(), "r");

		if (file != NULL)
		{
			// read
			fread(pUniqueCode, 1, 32, file);
			fclose(file);
			//Write copy.
			pUniqueCode[32] = 0;
			FILE* fp = fopen(installfile, "w");
			if (!fp)
				fp = AGKfopen(installfile, "w");

			if (fp != NULL) {
				fwrite(pUniqueCode, 1, 32, fp);
				fclose(fp);
			}
			else {
				//Make sure dir exists and try again.
				uString cdir = defaultWriteFolder;
				cdir.ReplaceStr("\\", "/");
				cdir.ReplaceStr("AGKApps/TheGameCreators/Ide/", "AGKApps");
#ifdef AGK_WINDOWS
				mkdir(cdir.GetStr());
#else
				mkdir(cdir.GetStr(), S_IRWXU | S_IRWXG | S_IRWXO );
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
				FILE* fp = fopen(installfile, "w");
				if (!fp)
					fp = AGKfopen(installfile, "w");

				if (fp != NULL) {
					fwrite(pUniqueCode, 1, 32, fp);
					fclose(fp);
				}
			}
		}
		else {
			// generate
			time_t mtime;
			mtime = time(0);
			srand(mtime);
			int n = 0;
			for (; n < 32; n++)
			{
				pUniqueCode[n] = 65 + (rand() % 22);
			}
			pUniqueCode[32] = 0;
			FILE* fp = fopen(installfile, "w");
			if (!fp)
				fp = AGKfopen(installfile, "w");

			if (fp != NULL) {
				fwrite(pUniqueCode, 1, 32, fp);
				fclose(fp);
			}
			else {
				//Make sure dir exists and try again.
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
				FILE* fp = fopen(installfile, "w");
				if (!fp)
					fp = AGKfopen(installfile, "w");

				if (fp != NULL) {
					fwrite(pUniqueCode, 1, 32, fp);
					fclose(fp);
				}
			}
		}
	}
	else
	{
		// read
		fread(pUniqueCode, 1, 32, file);
		fclose(file);
	}
	pUniqueCode[32] = 0;

#ifdef NONEWSSYSTEM
	ServerNewsRunning = false;
#else
	#ifndef DEVVERSION
		//Start Async code;
		ServerNewsHttp = agk::CreateHTTPConnection();
		agk::SetHTTPHost(ServerNewsHttp, "www.thegamecreators.com", 1);
		char postdata[MAX_PATH];
		#ifdef AGK_WINDOWS
			#ifdef DISPLAYCLASSICNEWS
				sprintf(postdata, "app=agkc&os=windows&k=vIo3sc2z&uid=%s", pUniqueCode);
			#else
				sprintf(postdata, "app=agks&os=windows&k=vIo3sc2z&uid=%s", pUniqueCode);
			#endif
		#endif
		#ifdef AGK_MACOS
			sprintf(postdata, "app=agks&os=mac&k=vIo3sc2z&uid=%s", pUniqueCode);
		#endif
		#ifdef AGK_LINUX
			sprintf(postdata, "app=agks&os=linux&k=vIo3sc2z&uid=%s", pUniqueCode);
		#endif

		agk::SendHTTPRequestASync(ServerNewsHttp, "api/agk/ide/announcement", postdata);
		ServerNewsRunning = true;
		ServerNewsTimer = agk::Timer();
	#else
		ServerNewsRunning = false;
	#endif
#endif

	//DebugInfo(defaultWriteFolder, "wf:");

	uString src_folder = startupFolder;
	src_folder.Append("/media/data/android/sourceGoogleDTS/classes.dex");
	if (file_exists( (char *) src_folder.GetStr())) {
		bDTSPossible = true;
	}

	//Auto Update Files if selected.
	if (pref.iInstallUpdate == 1) {
		if (pref.cInstallVersion != INTVERSION) {
			extern int installprocesscount;
			installprocesscount = 120;
		}
	}

#ifdef AGK_MACOS
	// delete old Android export files
	if ( pref.iAndroidExportVersion != INTVERSION )
	{
		uString sExportPath = defaultWriteFolder;
		sExportPath.AppendUTF8( "AndroidExport" );
		utils_remove_folder_recursive( sExportPath );

		pref.iAndroidExportVersion = INTVERSION;
	}
#endif

	extern char import_error_msg[MAX_PATH];
	strcpy(import_error_msg, "");

}




void ChangeIconSet(void)
{
#ifdef AGK_WINDOWS
	_chdir(startupFolder);
#else
	chdir(startupFolder);
#endif
	agk::SetFolder("");

	for (int i = 0; i < 20; i++) {
		if (agk::GetImageExists(iToolbarImages[i]))
			agk::DeleteImage(iToolbarImages[i]);
		iToolbarImages[i] = 0;
		if (agk::GetImageExists(iToolbarImagesDown[i]))
			agk::DeleteImage(iToolbarImagesDown[i]);
		iToolbarImagesDown[i] = 0;
	}

	if (pref.iCurrentIconSet == 0) {
		if (agk::GetImageExists(iToolbarImages[0])) agk::DeleteImage(iToolbarImages[0]);
		iToolbarImages[0] = agk::LoadImage("media/icons/new_window.png");
		iToolbarText[0] = "New Project";

		if (agk::GetImageExists(iToolbarImages[1])) agk::DeleteImage(iToolbarImages[1]);
		iToolbarImages[1] = agk::LoadImage("media/icons/folder_vertical_open.png");
		iToolbarText[1] = "Open Project";

		iToolbarImages[2] = agk::LoadImage("media/icons/page_save.png");
		iToolbarText[2] = "Save the current file";

		iToolbarImages[3] = agk::LoadImage("media/icons/page_save_all.png");
		iToolbarText[3] = "Save all open files";

		iToolbarImages[4] = agk::LoadImage("media/icons/undo.png");
		iToolbarText[4] = "Undo";

		iToolbarImages[5] = agk::LoadImage("media/icons/redo.png");
		iToolbarText[5] = "Redo";

		iToolbarImages[6] = agk::LoadImage("media/icons/cog_go.png");
		iToolbarText[6] = "Compile";

		iToolbarImages[7] = agk::LoadImage("media/icons/computer_go.png");
		iToolbarImagesDown[7] = agk::LoadImage("media/icons/computer_go_cancel.png");
		bToolbarImagesState[7] = false;
		iToolbarText[7] = "Run";

		iToolbarImages[8] = agk::LoadImage("media/icons/world_go.png");
		iToolbarImagesDown[8] = agk::LoadImage("media/icons/world_go_cancel.png");
		bToolbarImagesState[8] = false;
		iToolbarText[8] = "Broadcast";

		iToolbarImages[9] = agk::LoadImage("media/icons/bug_go.png");
		iToolbarImagesDown[9] = agk::LoadImage("media/icons/bug_go_cancel.png");
		bToolbarImagesState[9] = false;
		iToolbarText[9] = "Debug";

		iToolbarImages[10] = agk::LoadImage("media/icons/page_find.png");
		iToolbarText[10] = "Find";

		iToolbarImages[11] = agk::LoadImage("media/icons/playbox.png");
		iToolbarImagesDown[11] = agk::LoadImage("media/icons/playbox_cancel.png");
		iToolbarText[11] = "Run in SandBox";

		iToolbarImages[12] = agk::LoadImage("media/icons/keyboardmac.png");
		iToolbarImagesDown[12] = agk::LoadImage("media/icons/keyboardpc.png");
		iToolbarText[12] = "Change Keyboard Layout";

		iToolbarImages[13] = agk::LoadImage("media/icons/32-pixels/regular/settings.png");
		iToolbarImagesDown[13] = agk::LoadImage("media/icons/32-pixels/regular/settings.png");
		bToolbarImagesState[13] = false;
		iToolbarText[13] = "Settings";

		iToolbarImages[14] = agk::LoadImage("media/icons/32-pixels/regular/center-stage.png");
		iToolbarImagesDown[14] = agk::LoadImage("media/icons/32-pixels/regular/center-stage.png");
		bToolbarImagesState[14] = false;
		iToolbarText[14] = "Reset View";

	}
	else if( pref.iCurrentIconSet ==  1) {

		if (pref.bEnableToolbarExtraLargeIcons) {
			iToolbarImages[0] = agk::LoadImage("media/icons/128-pixels/regular/new.png");
			iToolbarText[0] = "New Project";

			iToolbarImages[1] = agk::LoadImage("media/icons/128-pixels/regular/open.png");
			iToolbarText[1] = "Open Project";

			iToolbarImages[2] = agk::LoadImage("media/icons/128-pixels/regular/save.png");
			iToolbarText[2] = "Save the current file";

			iToolbarImages[3] = agk::LoadImage("media/icons/128-pixels/regular/save-all.png");
			iToolbarText[3] = "Save all open files";

			iToolbarImages[4] = agk::LoadImage("media/icons/128-pixels/regular/undo.png");
			iToolbarText[4] = "Undo";

			iToolbarImages[5] = agk::LoadImage("media/icons/128-pixels/regular/redo.png");
			iToolbarText[5] = "Redo";

			iToolbarImages[6] = agk::LoadImage("media/icons/128-pixels/regular/compile.png");
			iToolbarText[6] = "Compile";

			iToolbarImages[7] = agk::LoadImage("media/icons/128-pixels/regular/run.png");
			iToolbarImagesDown[7] = agk::LoadImage("media/icons/128-pixels/regular/run.png");
			bToolbarImagesState[7] = false;
			iToolbarText[7] = "Run";

			iToolbarImages[8] = agk::LoadImage("media/icons/128-pixels/regular/broadcast.png");
			iToolbarImagesDown[8] = agk::LoadImage("media/icons/128-pixels/regular/broadcast.png");
			bToolbarImagesState[8] = false;
			iToolbarText[8] = "Broadcast";

			iToolbarImages[9] = agk::LoadImage("media/icons/128-pixels/regular/debug.png");
			iToolbarImagesDown[9] = agk::LoadImage("media/icons/128-pixels/regular/debug.png");
			bToolbarImagesState[9] = false;
			iToolbarText[9] = "Debug";

			iToolbarImages[10] = agk::LoadImage("media/icons/128-pixels/regular/find.png");
			iToolbarText[10] = "Find";

			iToolbarImages[11] = agk::LoadImage("media/icons/playbox.png");
			iToolbarImagesDown[11] = agk::LoadImage("media/icons/playbox_cancel.png");
			iToolbarText[11] = "Run in SandBox";

			iToolbarImages[12] = agk::LoadImage("media/icons/keyboardmac.png");
			iToolbarImagesDown[12] = agk::LoadImage("media/icons/keyboardpc.png");
			iToolbarText[12] = "Change Keyboard Layout";

			iToolbarImages[13] = agk::LoadImage("media/icons/128-pixels/regular/settings.png");
			iToolbarImagesDown[13] = agk::LoadImage("media/icons/128-pixels/regular/settings.png");
			bToolbarImagesState[13] = false;
			iToolbarText[13] = "Settings";

			iToolbarImages[14] = agk::LoadImage("media/icons/128-pixels/regular/center-stage.png");
			iToolbarImagesDown[14] = agk::LoadImage("media/icons/128-pixels/regular/center-stage.png");
			bToolbarImagesState[14] = false;
			iToolbarText[14] = "Reset View";
		}
		else {

			iToolbarImages[0] = agk::LoadImage("media/icons/32-pixels/regular/new.png");
			iToolbarText[0] = "New Project";

			iToolbarImages[1] = agk::LoadImage("media/icons/32-pixels/regular/open.png");
			iToolbarText[1] = "Open Project";

			iToolbarImages[2] = agk::LoadImage("media/icons/32-pixels/regular/save.png");
			iToolbarText[2] = "Save the current file";

			iToolbarImages[3] = agk::LoadImage("media/icons/32-pixels/regular/save-all.png");
			iToolbarText[3] = "Save all open files";

			iToolbarImages[4] = agk::LoadImage("media/icons/32-pixels/regular/undo.png");
			iToolbarText[4] = "Undo";

			iToolbarImages[5] = agk::LoadImage("media/icons/32-pixels/regular/redo.png");
			iToolbarText[5] = "Redo";

			iToolbarImages[6] = agk::LoadImage("media/icons/32-pixels/regular/compile.png");
			iToolbarText[6] = "Compile";

			iToolbarImages[7] = agk::LoadImage("media/icons/32-pixels/regular/run.png");
			iToolbarImagesDown[7] = agk::LoadImage("media/icons/32-pixels/regular/run.png");
			bToolbarImagesState[7] = false;
			iToolbarText[7] = "Run";

			iToolbarImages[8] = agk::LoadImage("media/icons/32-pixels/regular/broadcast.png");
			iToolbarImagesDown[8] = agk::LoadImage("media/icons/32-pixels/regular/broadcast.png");
			bToolbarImagesState[8] = false;
			iToolbarText[8] = "Broadcast";

			iToolbarImages[9] = agk::LoadImage("media/icons/32-pixels/regular/debug.png");
			iToolbarImagesDown[9] = agk::LoadImage("media/icons/32-pixels/regular/debug.png");
			bToolbarImagesState[9] = false;
			iToolbarText[9] = "Debug";

			iToolbarImages[10] = agk::LoadImage("media/icons/32-pixels/regular/find.png");
			iToolbarText[10] = "Find";

			iToolbarImages[11] = agk::LoadImage("media/icons/playbox.png");
			iToolbarImagesDown[11] = agk::LoadImage("media/icons/playbox_cancel.png");
			iToolbarText[11] = "Run in SandBox";

			iToolbarImages[12] = agk::LoadImage("media/icons/keyboardmac.png");
			iToolbarImagesDown[12] = agk::LoadImage("media/icons/keyboardpc.png");
			iToolbarText[12] = "Change Keyboard Layout";

			iToolbarImages[13] = agk::LoadImage("media/icons/32-pixels/regular/settings.png");
			iToolbarImagesDown[13] = agk::LoadImage("media/icons/32-pixels/regular/settings.png");
			bToolbarImagesState[13] = false;
			iToolbarText[13] = "Settings";

			iToolbarImages[14] = agk::LoadImage("media/icons/32-pixels/regular/center-stage.png");
			iToolbarImagesDown[14] = agk::LoadImage("media/icons/32-pixels/regular/center-stage.png");
			bToolbarImagesState[14] = false;
			iToolbarText[14] = "Reset View";
		}
	}
	else
	{

		if (pref.bEnableToolbarExtraLargeIcons) {
			iToolbarImages[0] = agk::LoadImage("media/icons/128-pixels/thin/new.png");
			iToolbarText[0] = "New Project";

			iToolbarImages[1] = agk::LoadImage("media/icons/128-pixels/thin/open.png");
			iToolbarText[1] = "Open Project";

			iToolbarImages[2] = agk::LoadImage("media/icons/128-pixels/thin/save.png");
			iToolbarText[2] = "Save the current file";

			iToolbarImages[3] = agk::LoadImage("media/icons/128-pixels/thin/save-all.png");
			iToolbarText[3] = "Save all open files";

			iToolbarImages[4] = agk::LoadImage("media/icons/128-pixels/thin/undo.png");
			iToolbarText[4] = "Undo";

			iToolbarImages[5] = agk::LoadImage("media/icons/128-pixels/thin/redo.png");
			iToolbarText[5] = "Redo";

			iToolbarImages[6] = agk::LoadImage("media/icons/128-pixels/thin/compile.png");
			iToolbarText[6] = "Compile";

			iToolbarImages[7] = agk::LoadImage("media/icons/128-pixels/thin/run.png");
			iToolbarImagesDown[7] = agk::LoadImage("media/icons/128-pixels/thin/run.png");
			bToolbarImagesState[7] = false;
			iToolbarText[7] = "Run";

			iToolbarImages[8] = agk::LoadImage("media/icons/128-pixels/thin/broadcast.png");
			iToolbarImagesDown[8] = agk::LoadImage("media/icons/128-pixels/thin/broadcast.png");
			bToolbarImagesState[8] = false;
			iToolbarText[8] = "Broadcast";

			iToolbarImages[9] = agk::LoadImage("media/icons/128-pixels/thin/debug.png");
			iToolbarImagesDown[9] = agk::LoadImage("media/icons/128-pixels/thin/debug.png");
			bToolbarImagesState[9] = false;
			iToolbarText[9] = "Debug";

			iToolbarImages[10] = agk::LoadImage("media/icons/128-pixels/thin/find.png");
			iToolbarText[10] = "Find";

			iToolbarImages[11] = agk::LoadImage("media/icons/playbox.png");
			iToolbarImagesDown[11] = agk::LoadImage("media/icons/playbox_cancel.png");
			iToolbarText[11] = "Run in SandBox";

			iToolbarImages[12] = agk::LoadImage("media/icons/keyboardmac.png");
			iToolbarImagesDown[12] = agk::LoadImage("media/icons/keyboardpc.png");
			iToolbarText[12] = "Change Keyboard Layout";

			iToolbarImages[13] = agk::LoadImage("media/icons/128-pixels/thin/settings.png");
			iToolbarImagesDown[13] = agk::LoadImage("media/icons/128-pixels/thin/settings.png");
			bToolbarImagesState[13] = false;
			iToolbarText[13] = "Settings";

			iToolbarImages[14] = agk::LoadImage("media/icons/128-pixels/thin/center-stage.png");
			iToolbarImagesDown[14] = agk::LoadImage("media/icons/128-pixels/thin/center-stage.png");
			bToolbarImagesState[14] = false;
			iToolbarText[14] = "Reset View";
		}
		else {
			iToolbarImages[0] = agk::LoadImage("media/icons/32-pixels/thin/new.png");
			iToolbarText[0] = "New Project";

			iToolbarImages[1] = agk::LoadImage("media/icons/32-pixels/thin/open.png");
			iToolbarText[1] = "Open Project";

			iToolbarImages[2] = agk::LoadImage("media/icons/32-pixels/thin/save.png");
			iToolbarText[2] = "Save the current file";

			iToolbarImages[3] = agk::LoadImage("media/icons/32-pixels/thin/save-all.png");
			iToolbarText[3] = "Save all open files";

			iToolbarImages[4] = agk::LoadImage("media/icons/32-pixels/thin/undo.png");
			iToolbarText[4] = "Undo";

			iToolbarImages[5] = agk::LoadImage("media/icons/32-pixels/thin/redo.png");
			iToolbarText[5] = "Redo";

			iToolbarImages[6] = agk::LoadImage("media/icons/32-pixels/thin/compile.png");
			iToolbarText[6] = "Compile";

			iToolbarImages[7] = agk::LoadImage("media/icons/32-pixels/thin/run.png");
			iToolbarImagesDown[7] = agk::LoadImage("media/icons/32-pixels/thin/run.png");
			bToolbarImagesState[7] = false;
			iToolbarText[7] = "Run";

			iToolbarImages[8] = agk::LoadImage("media/icons/32-pixels/thin/broadcast.png");
			iToolbarImagesDown[8] = agk::LoadImage("media/icons/32-pixels/thin/broadcast.png");
			bToolbarImagesState[8] = false;
			iToolbarText[8] = "Broadcast";

			iToolbarImages[9] = agk::LoadImage("media/icons/32-pixels/thin/debug.png");
			iToolbarImagesDown[9] = agk::LoadImage("media/icons/32-pixels/thin/debug.png");
			bToolbarImagesState[9] = false;
			iToolbarText[9] = "Debug";

			iToolbarImages[10] = agk::LoadImage("media/icons/32-pixels/thin/find.png");
			iToolbarText[10] = "Find";

			iToolbarImages[11] = agk::LoadImage("media/icons/playbox.png");
			iToolbarImagesDown[11] = agk::LoadImage("media/icons/playbox_cancel.png");
			iToolbarText[11] = "Run in SandBox";

			iToolbarImages[12] = agk::LoadImage("media/icons/keyboardmac.png");
			iToolbarImagesDown[12] = agk::LoadImage("media/icons/keyboardpc.png");
			iToolbarText[12] = "Change Keyboard Layout";

			iToolbarImages[13] = agk::LoadImage("media/icons/32-pixels/thin/settings.png");
			iToolbarImagesDown[13] = agk::LoadImage("media/icons/32-pixels/thin/settings.png");
			bToolbarImagesState[13] = false;
			iToolbarText[13] = "Settings";

			iToolbarImages[14] = agk::LoadImage("media/icons/32-pixels/thin/center-stage.png");
			iToolbarImagesDown[14] = agk::LoadImage("media/icons/32-pixels/thin/center-stage.png");
			bToolbarImagesState[14] = false;
			iToolbarText[14] = "Reset View";
		}
	}
}


void ProcessNewsWindow(void)
{
	if (!ServerDisplayNewsWindow)
		return;

	ImGuiWindowFlags window_flags = 0;

	if (ServerNewsGetImageId > 0)
		ImGui::SetNextWindowSize(ImVec2(37 * ImGui::GetFontSize(), 27 * ImGui::GetFontSize()), ImGuiCond_Once);
	else
		ImGui::SetNextWindowSize(ImVec2(25 * ImGui::GetFontSize(), 13 * ImGui::GetFontSize()), ImGuiCond_Once);

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.40f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("AppGameKit Studio", &ServerDisplayNewsWindow, window_flags);

	ImVec2 ws = ImGui::GetWindowSize();
	ImVec4* style_colors = ImGui::GetStyle().Colors;
	ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
	ImGui::SetWindowFontScale(1.35);
	style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
	ImGui::Spacing();
	ImGui::Text("News:");

	style_colors[ImGuiCol_Text] = oldTextColor;
	ImGui::SetWindowFontScale(1.0);
	ImGui::Separator();


	if (ServerNewsGetImageId > 0) {
		//Display image.
		float asx = agk::GetImageWidth(ServerNewsGetImageId);
		float asy = agk::GetImageHeight(ServerNewsGetImageId);
		float icon_ratio;
		int icon_size = 36 * ImGui::GetFontSize();

		if ((icon_size / (float)asx) < (icon_size / (float)asy))
			icon_ratio = icon_size / (float)asx;
		else
			icon_ratio = icon_size / (float)asy;
		float bw = asx*icon_ratio;
		float bh = asy*icon_ratio;

		if (ImGui::ImgBtn(ServerNewsGetImageId, ImVec2(bw, bh), ImColor(235, 235, 235, 235),ImColor(255, 255, 255, 255)))
		{
			//clicked.
			agk::OpenBrowser(ServerNewsUrl.GetStr());
		}

	}

	ImGui::TextWrapped("\n%s\n\n",ServerNewsMsg.GetStr());
//	ImGui::InputText("url:", (char *) ServerNewsUrl.GetStr(), ServerNewsUrl.GetLength(), ImGuiInputTextFlags_ReadOnly);
	ImGui::Separator();
	if (ImGui::Button(" Goto Link Page ")) {
		agk::OpenBrowser(ServerNewsUrl.GetStr());
	}
	ImGui::SameLine();

	if (ImGui::Button(" Close ")) {
		ServerDisplayNewsWindow = false;
		//Make sure it is not displayed again.
		char installfile[MAX_PATH];
		sprintf(installfile, "%sinstallstamp.dat", defaultWriteFolder);
		FILE* fp = fopen(installfile, "w");
		if (!fp)
			fp = AGKfopen(installfile, "w");
		if (fp) {
			fwrite(ServerNewsUpdate.GetStr(), 1, 19, fp);
			fclose(fp);
		}
	}

	ImGui::End();
}


ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
ImGuiID dockspaceID = 0;
ImVec4 my_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
#ifdef ADDIMGUIDEMO
bool show_demo_window = false;
#endif
int myTextureID; //Used to setup image buttons.
bool bLostFocus = false;
int count_resets = 0;
int pressedCTRLkey = 0;
int pressedALTkey = 0;
int iExternalpressedCTRLkey = 0;
int processCTRLkey = 0;
extern bool show_gotoline_window;
extern bool show_keystore_window;
extern bool show_additionalfiles_window;
extern bool show_androidexport_window;
extern bool show_html5export_window;
extern bool show_iosexport_window;


//###########################################
//#### Add project file to open projects ####
//###########################################

void AddProjectFile(char * cFileSelected)
{
	if (cFileSelected && strlen(cFileSelected) > 0) {
		LoadProjectFile(cFileSelected);
		isProjectInAssetsBrowser();
	}
}
//#####################################################
//#### Add source file to editor , outside project ####
//#### Used by "Open Source File"                  ####
//#####################################################

void AddSourceFile(char * cFileSelected)
{
	if (cFileSelected && strlen(cFileSelected) > 0) {

		cProjectItem::sProjectFiles * m_pCurrentFile;
		m_pCurrentFile = CreateNewAGCFile(cFileSelected);
		AddRecentFiles(NULL, cFileSelected);
		if (m_pCurrentFile) {
			vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
			m_pCurrentFile->m_bOpenForEdit = true;
			cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
		}
	}
}

//###################
//#### Main loop ####
//###################

struct EventTimes // PAUL:
{
	float fPrevTime;
	float fCurrTime;
	float fLastRenderTime;
	float fLastPreviewTime;
};

EventTimes eventTimes = {};

int app::Loop (void)
{
	eventTimes.fPrevTime = eventTimes.fCurrTime; // PAUL:
	eventTimes.fCurrTime = agk::Timer(); 
	float timeDelta = eventTimes.fCurrTime - eventTimes.fPrevTime;
	
	if ( eventTimes.fCurrTime > 3600 ) // fix accuracy drift
	{
		agk::ResetTimer();
		eventTimes = {}; // reset everything to 0
	}
	ImVec4* style_colors;

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.DeltaTime = timeDelta; //PAUL:  set this here instead of in ImGui_ImplAGL_NewFrame()


	//News system
	if (ServerNewsRunning) {
		//Only check every 1 sec. first time check after 4 sec.
		if (agk::Timer() - ServerNewsTimer > 4.0) {

			//ServerNewsGetImage


			//Check after 1 sec.
			int ret = agk::GetHTTPResponseReady(ServerNewsHttp);
			if (ret == -1) {
				//Failed.
				ServerNewsRunning = false;
				agk::CloseHTTPConnection(ServerNewsHttp);
				agk::DeleteHTTPConnection(ServerNewsHttp);
			}
			if (ret > 0) {
				//Success.
				ServerNewsString = agk::GetHTTPResponse(ServerNewsHttp);

				agk::CloseHTTPConnection(ServerNewsHttp);
				agk::DeleteHTTPConnection(ServerNewsHttp);

				ServerNews = ServerNewsString;
				agk::DeleteString(ServerNewsString);
				if (ServerNews.FindStr("success", 1) > 0) {
					//Find update date.
					int pos = ServerNews.FindStr("\"date\":\"", 1);
					if (pos > 0) {
						ServerNews.SubString(ServerNewsUpdate, pos + 8, 19);
						//Check last news.
						char installfile[MAX_PATH];
						sprintf(installfile, "%sinstallstamp.dat", defaultWriteFolder);
						FILE *file = fopen(installfile, "r");
						if (!file)
							file = AGKfopen(installfile, "r");

						char lastdisplay[20];
						strcpy(lastdisplay, "na");
						if (file != NULL) {
							fread(lastdisplay, 1, 19, file);
							lastdisplay[19] = 0;
							fclose(file);
						}

						if (ServerNewsUpdate.CompareCaseTo(lastdisplay) != 0) {
							//unique display
							uString tmp;
							int pos1 = ServerNews.FindStr("message\":\"", 1);
							int pos2 = ServerNews.FindStr("url\":\"", 1);
							if (pos1 > 0 && pos2 > 0) {
								ServerNews.SubString(tmp, pos1 + 10);
								int pos3 = tmp.FindStr("\",\"", 1);
								if (pos3 > 0) {
									tmp.SubString(ServerNewsMsg, 0, pos3);
									ServerNews.SubString(tmp, pos2 + 6);
									pos3 = tmp.FindStr("\",\"", 1);
									if (pos3 > 0) {
										tmp.SubString(ServerNewsUrl, 0, pos3);
										ServerNewsUrl.ReplaceStr("\\", "");
										ServerDisplayNewsWindow = true;
										ServerNewsGetImageId = agk::LoadImage("media/agk-studio-news-header.png");

										//Checdk if we need to download a image.
										//ServerNewsImageUrl
										int pos4 = ServerNews.FindStr("image_url\":", 1);
										if (pos4 > 0) {

											ServerNews.SubString(tmp, pos4 + 11);
											int pos3 = tmp.FindStr(",", 1);
											if (pos3 > 0) {
												tmp.SubString(ServerNewsImageUrl, 0, pos3);

												
												if (1 == 2) {
													//ONLY TEST CODE DO NOT USE!

													//NEED to parse this:
													ServerNewsImageUrl = "https://www.thegamecreators.com/media/ide/agks/1557927910.png";

													ServerNewsImageUrl.ReplaceStr("\"", "");
													ServerNewsImageUrl.ReplaceStr("\\", "/");
													ServerNewsGetImage = true;

													int securehost = 0;
													uString imgHost;
													if (ServerNewsImageUrl.FindStr("https", 1) >= 0) {
														securehost = 1;
														ServerNewsImageUrl.SubString(imgHost, 6);

													}
													else {
														ServerNewsImageUrl.SubString(imgHost, 5);
													}
													imgHost.ReplaceStr("//", "");

													int pos6 = imgHost.FindStr("/");
													if (pos6 >= 0) {
														tmp = imgHost;
														tmp.SubString(imgHost, 0, pos6 );

														uString newsimgpath;
														tmp.SubString(newsimgpath, pos6 + 1);

														ServerNewsHttp = agk::CreateHTTPConnection();
														agk::SetHTTPHost(ServerNewsHttp, imgHost, securehost);
														agk::SetRawWritePath(defaultWriteFolder);
														agk::DeleteFile("fromnews025687.png");
														agk::DeleteFile("fromnews025687.jpg");
														if(ServerNewsImageUrl.FindStr(".png",1) >= 0 )
															agk::GetHTTPFile(ServerNewsHttp, newsimgpath, "fromnews025687.png");
														else
															agk::GetHTTPFile(ServerNewsHttp, newsimgpath, "fromnews025687.jpg");

														//Dont display yet wait until image is done downloading.
														ServerDisplayNewsWindow = false;
													}

												}
												else {
													if (ServerNewsImageUrl.CompareCaseTo("null") == 0) {

														//Just diplay without image.
														ServerDisplayNewsWindow = true;
													}
													else {

														ServerNewsImageUrl.ReplaceStr("\"", "");
														ServerNewsImageUrl.ReplaceStr("\\", "/");
														ServerNewsGetImage = true;

														int securehost = 0;
														uString imgHost;
														if (ServerNewsImageUrl.FindStr("https", 1) >= 0) {
															securehost = 1;
															ServerNewsImageUrl.SubString(imgHost, 6);

														}
														else {
															ServerNewsImageUrl.SubString(imgHost, 5);
														}
														//PE: Tested using classic image , seams format was a little different so.
														imgHost.ReplaceStr("////", "");
														imgHost.ReplaceStr("//", "/");

														int pos6 = imgHost.FindStr("/");
														if (pos6 >= 0) {
															tmp = imgHost;
															tmp.SubString(imgHost, 0, pos6);

															uString newsimgpath;
															tmp.SubString(newsimgpath, pos6 + 1);

															ServerNewsHttp = agk::CreateHTTPConnection();
															agk::SetHTTPHost(ServerNewsHttp, imgHost, securehost);
															agk::SetRawWritePath(defaultWriteFolder);
															agk::DeleteFile("fromnews025687.png");
															agk::DeleteFile("fromnews025687.jpg");
															if (ServerNewsImageUrl.FindStr(".png", 1) >= 0)
																agk::GetHTTPFile(ServerNewsHttp, newsimgpath, "fromnews025687.png");
															else
																agk::GetHTTPFile(ServerNewsHttp, newsimgpath, "fromnews025687.jpg");

															//Dont display yet wait until image is done downloading.
															ServerDisplayNewsWindow = false;
														}

													}
												}

											}

										}

									}

								}
							}

						}
						else {
							//Already seen
							ServerDisplayNewsWindow = false;
						}

					}
				}
				ServerNewsRunning = false;
			}
			ServerNewsTimer = agk::Timer()-3.0;
		}
	}
	else if (ServerNewsGetImage)
	{
		//Download image.
		int ret = agk::GetHTTPFileComplete(ServerNewsHttp);
		if (ret == -1) {
			//Failed.
			ServerNewsRunning = false;
			agk::CloseHTTPConnection(ServerNewsHttp);
			agk::DeleteHTTPConnection(ServerNewsHttp);

			//Failed to get image , just display text message.
			//ServerNewsGetImageId = 0;

			ServerDisplayNewsWindow = true;
			ServerNewsGetImage = false;
			agk::SetRawWritePath(agk::GetReadPath());

		}
		if (ret > 0) {
			//Image should be ready check.

			if (agk::GetImageExists(ServerNewsGetImageId)) {
				agk::DeleteImage(ServerNewsGetImageId);
				ServerNewsGetImageId = 0;
			}

			if (ServerNewsImageUrl.FindStr(".png", 1) >= 0)
				ServerNewsGetImageId = agk::LoadImage("fromnews025687.png");
			else
				ServerNewsGetImageId = agk::LoadImage("fromnews025687.jpg");

			if (!agk::GetImageExists(ServerNewsGetImageId)) {
				ServerNewsGetImageId = agk::LoadImage("media/agk-studio-news-header.png");
			}
			ServerDisplayNewsWindow = true;
			ServerNewsGetImage = false;
			agk::SetRawWritePath(agk::GetReadPath());
		}

	}

#ifdef AGK_WINDOWS

	//To solve problem with ALT-TAB away and key "release" are sent to another app.

	HWND tmpHwnd = GetFocus();
	if(bLostFocus == false && tmpHwnd != g_agkhWnd) {
		bLostFocus = true;
		io.KeySuper = false;
		io.KeyCtrl = false;
		io.KeyAlt = false;
		io.KeyShift = false;
		agk::KeyUp(16); // reset shift key.
		agk::KeyUp(17); // reset ctrl key.
		agk::KeyUp(18); // reset alt key.
		agk::KeyUp(19); // reset pause key.
		for (int i = 8; i < 273; i++) {
			agk::KeyUp(i);
		}
		//Reset keys.
		count_resets++;
	}
	if (tmpHwnd == g_agkhWnd) {
		if (bLostFocus) {
			//We got focus again.
			//Looks like its fine to reset keys on lost only.
			//If there is a problem, they can also be reset here.
		}
		bLostFocus = false;
	}

#endif
	
	if (pCurrentSelectedProject && pCurrentSelectedProjectState != pCurrentSelectedProject) {
		//Set title.
#ifdef TRIALVERSIONEXPIRES
		uString WindowTitle = WINDOW_TRIALTITLE;
#else
		uString WindowTitle = WINDOW_TITLE;
#endif
		WindowTitle.Append(" - ");
		WindowTitle.Append(pCurrentSelectedProject->m_sProjectFile);
		agk::SetWindowTitle(WindowTitle);
		pCurrentSelectedProjectState = pCurrentSelectedProject;
	}

	//PE: Display FPS
#ifdef DISPLAYFPSTITLE
	if (pCurrentSelectedProject) {
		if (agk::Random(0, 10) == 1) {
#ifdef TRIALVERSIONEXPIRES
			uString WindowTitle = WINDOW_TRIALTITLE;
#else
			uString WindowTitle = WINDOW_TITLE;
#endif
			WindowTitle.Append(" - ");
			WindowTitle.Append(pCurrentSelectedProject->m_sProjectFile);
			WindowTitle.Append(" - ");
			WindowTitle.Append("FPS: ");
			WindowTitle.AppendFloat(ImGui::GetIO().Framerate);
			agk::SetWindowTitle(WindowTitle);
			//ImGui::TextWrapped("Current IDE FPS: %.1f , (ms/f %.3f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		}
	}
#endif

	//Before a "NewFrame" change any fonts.

	//Remember last font selection. and save in pref.
	if (changefonttype >= 1 && changefonttype <= 5)
		pref.iLastFontType = changefonttype;

	//Change font type.
	switch (changefonttype) {

		case 1:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize , "media/fonts/Hack-Regular.ttf", pref.iEditorFontSize);
			changefonttype = 0;
			break;
		case 2:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize , "media/fonts/Hack-Bold.ttf", pref.iEditorFontSize);
			changefonttype = 0;
			break;
		case 3:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize , "media/fonts/Hack-Italic.ttf", pref.iEditorFontSize);
			changefonttype = 0;
			break;
		case 4:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize , "media/fonts/DejaVuSansMono.ttf", pref.iEditorFontSize);
			changefonttype = 0;
			break;
		case 5:
			AddAllFonts("media/Roboto-Medium.ttf", pref.iIDEFontSize , "media/fonts/DejaVuSansMono-Bold.ttf", pref.iEditorFontSize);
			changefonttype = 0;
			break;
		case 999:
			//Only fontsize change.
			AddAllFonts(NULL, pref.iIDEFontSize, NULL, pref.iEditorFontSize);
			changefonttype = 0;
			break;
		default:
			changefonttype = 0;
			break;
	}


	if (!bPreviewFontLoaded) {
		if (strlen(cPreviewFont) > 0) {

			extern ImFont* customfont;

			if(previewFont && previewFont != customfont) {
				//Must fre old font.
				//This is the only way must remove everything and recreate the atlas.
				AddAllFonts(NULL, pref.iIDEFontSize, NULL, pref.iEditorFontSize);
			}
			else {
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
			}
			bPreviewFontLoaded = true;
		}
	}


	//Add fonts to scene files.
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

							if (it->first < MAXUSERFONTS && !pUseScene->customfonts[it->first] ) {

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
										//0x0370, 0x03FF, //0370 - 03FF  	Greek and Coptic
														//1F00 - 1FFF  	Greek Extended
														0,
									};
									ImGuiIO& io = ImGui::GetIO(); (void)io;
									pUseScene->customfonts[it->first] = io.Fonts->AddFontFromFileTTF(FontPath.GetStr(), 48, NULL, NULL); // &Generic_ranges[0]
									//Rebuild atlas.
									ImGui_ImplOpenGL2_DestroyFontsTexture();
									ImGui_ImplOpenGL2_CreateFontsTexture();

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


	// Start ImGui frame. Copy keys/mouse/everyting from AGK to ImGiu.
	ImGui_ImplAGL_NewFrame();
#ifndef AGK_WINDOWS
	// Add additional setup from GLFW like windowsize/framebuffersize...
	ImGui_ImplGlfw_NewFrame();
#endif

#ifdef AGK_WINDOWS
	//This dont work on scaled resolutions.
	io.MousePos = ImVec2((float)agk::GetPointerX(), (float)agk::GetPointerY());
#endif

	//Check if we need to setup and bind a font texture.
	ImGui_ImplOpenGL2_NewFrame();

	//ImGui start a new frame.
	ImGui::NewFrame();

	ImVec2 viewPortPos = ImGui::GetMainViewport()->Pos;

	// imGui setup gui.
	{
		static bool dockingopen = true;

		//###################################################
		//#### Toolbar windows , includes all top menu's ####
		//###################################################

		float fsy = ImGui::CalcTextSize("#").y;

//		if (pref.bEnableToolbarLargeIcons)
//			toolbar_size = 78 - 16 ;
//		else
//			toolbar_size = 48 ;
		//64
		if (pref.bEnableToolbarExtraLargeIcons) {
			toolbar_size = 80 + fsy + 2;
		}
		else if (pref.bEnableToolbarLargeIcons)
			toolbar_size = 62 - 16 + fsy + 2;
		else
			toolbar_size = 32 + fsy + 2;


		int iOldRounding = ImGui::GetStyle().WindowRounding;
		ImGui::GetStyle().WindowRounding = 0.0f;

		if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(0, 0) + viewPortPos, ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, toolbar_size));
		ImGui::Begin("Toolbar", &dockingopen, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);


		ImGui::GetStyle().WindowRounding = iOldRounding;

		//Get hotkeys.
		int pressedFkey = 0;
		if (ImGui::GetTime() - lastKeyTime >= 0.125) {

#ifdef AGK_WINDOWS
			if (ImGui::IsKeyPressed(0x73)) { // 0x73 = F4
				if (io.KeyAlt) {
#ifdef TRIALVERSIONEXPIRES
					trialaction = 9;
#else
					quit_app = 1;
#endif
				}
			}
#endif
//
			if(!(debug_is_running || remote_debug_is_running)) {
				if (ImGui::IsKeyPressed(0x7A)) { // 0x7A = F11
					lastKeyTime = (float)ImGui::GetTime();

					if (!agkfullscreen) {
						agkfullscreen = 1 - agkfullscreen;
						agkOldDeviceWeight = agk::GetDeviceWidth();
						agkOldDeviceHeight = agk::GetDeviceHeight();
						agkFullscreenTimer = agk::Timer();
						bAgkFullscreenTrigger = true;
						agk::SetWindowSize(agk::GetDeviceWidth(), agk::GetDeviceHeight(), agkfullscreen);
						agk::SetRawMouseVisible(1); //count internal windows mouse visible counter up.
					}
					else {
						agkfullscreen = 1 - agkfullscreen;
						if(agkOldDeviceWeight > 0 && agkOldDeviceHeight > 0)
							agk::SetWindowSize(agkOldDeviceWeight, agkOldDeviceHeight, agkfullscreen);
						else
							agk::SetWindowSize(agk::GetDeviceWidth(), agk::GetDeviceHeight(), agkfullscreen);
						agk::SetRawMouseVisible(1); //count internal windows mouse visible counter up.
					}

					io.KeysDown[0x7A] = false; //Reset keys.
					agk::KeyUp(0x7A); //Reset keys.
				}
			}

			if (ImGui::IsKeyPressed(0x75)) { // 0x75 = F6
			}
			if (ImGui::IsKeyPressed(0x77)) { // 0x77 = F8
			}
			//if (ImGui::IsKeyPressed(0x78)) { // 0x78 = F9
			//	pressedFkey = 9;
			//	lastKeyTime = (float)ImGui::GetTime();
			//	io.KeysDown[0x78] = false; //Reset keys.
			//	agk::KeyUp(0x78); //Reset keys.
			//}
			if (ImGui::IsKeyPressed(0x79)) { // 0x79 = F10
			}
			if ((debug_is_running || remote_debug_is_running)) {
				if (ImGui::IsKeyPressed(0x7A)) { // 0x7A = F11
					pressedFkey = 11;
					lastKeyTime = (float)ImGui::GetTime();
					io.KeysDown[0x7A] = false; //Reset keys.
					agk::KeyUp(0x7A); //Reset keys.
				}
			}
		}
		pressedCTRLkey = 0;
		pressedALTkey = 0;
		if (iExternalpressedCTRLkey != 0) {
			pressedCTRLkey = iExternalpressedCTRLkey;
			iExternalpressedCTRLkey = 0;
		}
		if (ImGui::GetTime() - lastKeyTime >= 0.125) {
			auto ctrl = (io.ConfigMacOSXBehaviors && keyboard_layout == 1) ? io.KeySuper : io.KeyCtrl;
			auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
			auto shift = io.KeyShift;
			if (keyboard_layout == 0) {
				ctrl = io.KeyCtrl;
				alt = io.KeyAlt;
			}
			//PE: Special +- 187,189 now set to respect preferences shortcut setting for shift,alt,ctrl.
			if (ctrl == pref.bZoomInCtrl && shift == pref.bZoomInShift && alt == pref.bZoomInAlt && ( ImGui::IsKeyPressed(pref.iZoomInKey) || ImGui::IsKeyPressed(187)) ) {
				pressedCTRLkey = '+';
				lastKeyTime = (float)ImGui::GetTime();
			}
			if (ctrl == pref.bZoomOutCtrl && shift == pref.bZoomOutShift && alt == pref.bZoomOutAlt && ( ImGui::IsKeyPressed(pref.iZoomOutKey) || ImGui::IsKeyPressed(189)) ) {
				pressedCTRLkey = '-';
				lastKeyTime = (float)ImGui::GetTime();
			}
			if (ctrl == pref.bGotoLineCtrl && shift == pref.bGotoLineShift && alt == pref.bGotoLineAlt && ImGui::IsKeyPressed(pref.iGotoLineKey)) {
				pressedCTRLkey = 'L';
				lastKeyTime = (float)ImGui::GetTime();
			}

			if (ctrl == pref.bRunCtrl && shift == pref.bRunShift && alt == pref.bRunAlt && ImGui::IsKeyPressed(pref.iRunKey)) {
				pressedFkey = 5;
				lastKeyTime = (float)ImGui::GetTime();
				io.KeysDown[pref.iRunKey] = false; //Reset keys.
				agk::KeyUp(pref.iRunKey); //Reset keys.
			}

			if (ctrl == pref.bCompileCtrl && shift == pref.bCompileShift && alt == pref.bCompileAlt && ImGui::IsKeyPressed(pref.iCompileKey)) {
				pressedFkey = 7;
				lastKeyTime = (float)ImGui::GetTime();
				io.KeysDown[pref.iCompileKey] = false; //Reset keys.
				agk::KeyUp(pref.iCompileKey); //Reset keys.
			}

			if (ctrl == pref.bBroadCastCtrl && shift == pref.bBroadCastShift && alt == pref.bBroadCastAlt && ImGui::IsKeyPressed(pref.iBroadCastKey)) {
				pressedFkey = 6;
				lastKeyTime = (float)ImGui::GetTime();
				io.KeysDown[pref.iBroadCastKey] = false; //Reset keys.
				agk::KeyUp(pref.iBroadCastKey); //Reset keys.
			}

			if (ctrl == pref.bDebugCtrl && shift == pref.bDebugShift && alt == pref.bDebugAlt && ImGui::IsKeyPressed(pref.iDebugKey)) {
				pressedFkey = 8;
				lastKeyTime = (float)ImGui::GetTime();
				io.KeysDown[pref.iDebugKey] = false; //Reset keys.
				agk::KeyUp(pref.iDebugKey); //Reset keys.
			}


			if (ctrl == pref.bDebugStepCtrl && shift == pref.bDebugStepShift && alt == pref.bDebugStepAlt && ImGui::IsKeyPressed(pref.iDebugStepKey)) {
				pressedFkey = 10;
				lastKeyTime = (float)ImGui::GetTime();
				io.KeysDown[pref.iDebugStepKey] = false; //Reset keys.
				agk::KeyUp(pref.iDebugStepKey); //Reset keys.
			}
			if (debug_is_running || remote_debug_is_running) {
				if (ctrl == pref.bDebugStepCtrl && shift && alt == pref.bDebugStepAlt && ImGui::IsKeyPressed(pref.iDebugStepKey)) {
					pressedFkey = 10;
					lastKeyTime = (float)ImGui::GetTime();
					io.KeysDown[pref.iDebugStepKey] = false; //Reset keys.
					agk::KeyUp(pref.iDebugStepKey); //Reset keys.
				}
			}

			if (ctrl == pref.bOpenFileCtrl && shift == pref.bOpenFileShift && alt == pref.bOpenFileAlt && ImGui::IsKeyPressed(pref.iOpenFileKey)) {
				pressedCTRLkey = 'O';
				lastKeyTime = (float)ImGui::GetTime();
				io.KeysDown[pref.iOpenFileKey] = false; //Reset keys.
				agk::KeyUp(pref.iOpenFileKey); //Reset keys.
			}

			if (ctrl == pref.bOpenProjectCtrl && shift == pref.bOpenProjectShift && alt == pref.bOpenProjectAlt && ImGui::IsKeyPressed(pref.iOpenProjectKey)) {
				pressedCTRLkey = 'P';
				lastKeyTime = (float)ImGui::GetTime();
				io.KeysDown[pref.iOpenProjectKey] = false; //Reset keys.
				agk::KeyUp(pref.iOpenProjectKey); //Reset keys.
			}

			if (ctrl && !alt) {
				//if (ImGui::IsKeyPressed(187)) { //187 - normal +
				//	pressedCTRLkey = '+';
				//	lastKeyTime = (float)ImGui::GetTime();
				//}
				//if (ImGui::IsKeyPressed(189)) { //189 - normal -
				//	pressedCTRLkey = '-';
				//	lastKeyTime = (float)ImGui::GetTime();
				//}
				if (ImGui::IsKeyPressed(9)) { //9 = tab.
					pressedCTRLkey = '\t';
					lastKeyTime = (float)ImGui::GetTime();
				}
				//if (ImGui::IsKeyPressed(76)) { //76 = L
				//	pressedCTRLkey = 'L';
				//	lastKeyTime = (float)ImGui::GetTime();
				//}
			}
		}

		if (m_ActiveEditor && pressedCTRLkey == 'L')
		{
			show_gotoline_window = true;
		}
		//Editor zoom in and out hotkeys.
		if (pressedCTRLkey == '+') {
			if (pref.iEditorFontSize < 64)
				pref.iEditorFontSize++;
			changefonttype = 999;
		}
		if (pressedCTRLkey == '-') {
			if (pref.iEditorFontSize > 6)
				pref.iEditorFontSize--;
			changefonttype = 999;
		}
		//open source file.
		if (pressedCTRLkey == 'O') {

			char * cFileSelected;
			cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, NULL, NULL, NULL);

			AddSourceFile(cFileSelected);
/*
			if (cFileSelected && strlen(cFileSelected) > 0) {

				cProjectItem::sProjectFiles * m_pCurrentFile;
				m_pCurrentFile = CreateNewAGCFile(cFileSelected);
				AddRecentFiles(NULL, cFileSelected);
				if (m_pCurrentFile) {
					vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
					m_pCurrentFile->m_bOpenForEdit = true;
					cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
				}
			}
*/
		}

		//open project
		if (pressedCTRLkey == 'P') {
			OpenProject();
			isProjectInAssetsBrowser();
			setVisibleProjectsInAssetBrowser();
		}

		//Display toolbar icons.
		ImVec2 iToolbarIconSize = ImVec2(48 - 16, 48 - 16);

		if (pref.bEnableToolbarExtraLargeIcons) {
			iToolbarIconSize = ImVec2(64, 64);
		}
		else if (pref.bEnableToolbarLargeIcons)
			iToolbarIconSize = ImVec2(48 - 16, 48 - 16);
		else
			iToolbarIconSize = ImVec2(48 - 16 - 12, 48 - 16 - 12);


		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));
		
		ImVec4 style_back = ImGui::GetStyle().Colors[ImGuiCol_Text];
		style_back = style_back * ImVec4(1.2, 1.2, 1.2, 1.2);
		ImVec4 drawCol_back = ImColor(255, 255, 255, 0)*style_back; // Not really used as we have transparent icons.
		ImVec4 drawCol_normal = ImColor(220, 220, 220, 220)*style_back;
		ImVec4 drawCol_hover = ImColor(255, 255, 255, 255)*style_back;
		ImVec4 drawCol_Down = ImColor(180, 180, 160, 255)*style_back;
		ImVec4 drawCol_active = ImColor(120, 220, 120, 220)*style_back;
		if (pref.iCurrentIconSet == 0) {
			//No need in this set, they have cancel symbol.
			drawCol_active = ImColor(220, 220, 220, 220)*style_back;
		}
		//New project.
		if (ImGui::ImgBtn(iToolbarImages[0], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover , drawCol_Down )) {
			extern char projectfolder[MAX_PATH];
			if (strlen(pref.cDefaultProjectFolder) > 1)
				strcpy(projectfolder, pref.cDefaultProjectFolder);
			create_new_project = true;
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[0]);
		ImGui::SameLine();

		//Open Project.
		if (ImGui::ImgBtn(iToolbarImages[1], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down)) {
			OpenProject();
			isProjectInAssetsBrowser();
			setVisibleProjectsInAssetBrowser();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[1]);
		ImGui::SameLine();

		//Save file.
		if (ImGui::ImgBtn(iToolbarImages[2], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down)) {
			if (m_ActiveEditor) {
				cProjectItem::sProjectFiles * m_pCurrentFile;
				m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
				if (m_pCurrentFile) {
					if (m_pCurrentFile->m_bUseSaveAs) {
						m_ActiveEditor->saveonreturn = true;
					}
					else {
						auto textToSave = m_ActiveEditor->GetText();
#ifndef DISABLESAVE
						std::ofstream myfile;
						myfile.open((char*)m_pCurrentFile->m_sFullPath.GetStr());
						myfile << textToSave;
						myfile.close();
#endif
						m_ActiveEditor->filechanged = false;
					}
				}
			}
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[2]);
		ImGui::SameLine();

		//Save all open files.
		if (ImGui::ImgBtn(iToolbarImages[3], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down)) {
			if (allProjects) {
				cProjectItem * searchProject;
				searchProject = allProjects;
				while (searchProject) {
					SaveAllEditedProjectFiles(searchProject);
					searchProject = searchProject->m_pNext;
				}
			}
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[3]);
		ImGui::SameLine();

		//Icon seperator
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 30, ImGui::GetCursorPos().y));

		//Get ReadOnly state.
		bool ro = true;
		if (m_ActiveEditor)
			ro = m_ActiveEditor->IsReadOnly();

		//Undo
		if (!m_ActiveEditor) {
			if (ImGui::ImgBtn(iToolbarImages[4], iToolbarIconSize, drawCol_back, drawCol_Down, drawCol_Down, drawCol_Down)) { //Undo
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("No active editor.");
		}
		else {
			if (ImGui::ImgBtn(iToolbarImages[4], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down)) { //Undo
				if (m_ActiveEditor) {
					if (!ro && m_ActiveEditor->CanUndo())
						m_ActiveEditor->Undo();
				}
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[4]);
		}
		ImGui::SameLine();

		//Redo.
		if (!m_ActiveEditor) {
			if (ImGui::ImgBtn(iToolbarImages[5], iToolbarIconSize, drawCol_back, drawCol_Down, drawCol_Down, drawCol_Down)) { //Redo
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("No active editor.");
		}
		else {
			if (ImGui::ImgBtn(iToolbarImages[5], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down)) { //Redo
				if (m_ActiveEditor) {
					if (!ro && m_ActiveEditor->CanRedo())
						m_ActiveEditor->Redo();
				}
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[5]);
		}

		ImGui::SameLine();

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 30, ImGui::GetCursorPos().y));


		//#### Compile ####
		if (ImGui::ImgBtn(iToolbarImages[6], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down) || pressedFkey == 7) {
			//Compile
			//Save all edited agc files.
			if (pCurrentSelectedProject) CompileProject(pCurrentSelectedProject, false, false, false); // flags: run,broadcast,debug.
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[6]);
		ImGui::SameLine();


		//#### Compile + run ####
		bool bF5Pressed = false;
		if (pressedFkey == 5) bF5Pressed = true;
		if (debug_is_running || remote_debug_is_running) bF5Pressed = false;

		if (bToolbarImagesState[7]) {
			//Is running.
			int still_running = agk::GetAppRunning(game_is_running);
			if (!still_running) {
				bToolbarImagesState[7] = false;
				game_is_running = 0;

				//Set window focus on Linux?

				//PE: Restore window focus after running app.
				if (m_ActiveEditor) {
					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
					if (m_pCurrentFile) {
						cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
						bFocusActiveEditor = true;
					}
				}

			}
			if (ImGui::ImgBtn(iToolbarImagesDown[7], iToolbarIconSize, drawCol_back, drawCol_active, drawCol_hover, drawCol_Down) || bF5Pressed) {
				//Cancel Run.
				if (game_is_running > 0)
					agk::TerminateApp(game_is_running);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cancel");

		}
		else {
			if (ImGui::ImgBtn(iToolbarImages[7], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down) || bF5Pressed) {
				//Run
				if (pCurrentSelectedProject) {
					CompileProject(pCurrentSelectedProject, true, false, false); // flags: run,broadcast,debug.
					bToolbarImagesState[7] = true;
				}
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[7]);
		}

		ImGui::SameLine();

		//#### Compile + Broadcast ####
		if (broadcast_is_running) {
			if (ImGui::ImgBtn(iToolbarImagesDown[8], iToolbarIconSize, drawCol_back, drawCol_active, drawCol_hover, drawCol_Down) || pressedFkey == 6) {
				//Cancel broadcast.
				broadcast_is_running = false;

				uString bc_err;
				BroadcasterCommand("stop",bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());

				 BroadcasterCommand("disconnectall",bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());

				//broadcaster_initialized = false;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cancel Broadcast");
		}
		else {
			if (ImGui::ImgBtn(iToolbarImages[8], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down) || pressedFkey == 6) {
				//Broadcast
#ifdef TRIALVERSIONEXPIRES
				trialaction = 1;
#else
				if (pCurrentSelectedProject) {
					CompileProject(pCurrentSelectedProject, false, true, false); // flags: run,broadcast,debug.
				}
#endif

			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[8]);
		}

		ImGui::SameLine();

		//#########################
		//#### Compile + Debug ####
		//#########################

		if (!remote_debug_is_running) {
			if (bToolbarImagesState[9]) {
				//Is running.
				int still_running = agk::GetAppRunning(debug_is_running);
				if (!still_running) {
					//Try to remove device, so no SIGPIPE is generated, might be to late.
					uString bc_err;
					BroadcasterCommand("stop",bc_err);
					BroadcasterCommand("disconnectall",bc_err);
					ClearExecuteLine();
					bToolbarImagesState[9] = false;
					debug_is_running = 0;
				}
				if (ImGui::ImgBtn(iToolbarImagesDown[9], iToolbarIconSize, drawCol_back, drawCol_active, drawCol_hover, drawCol_Down) || pressedFkey == 8) {
					//Cancel Run.
					if (debug_is_running > 0) {
						//Disconnect device before closing down app.
						uString bc_err;
						BroadcasterCommand("stop",bc_err);
						BroadcasterCommand("disconnectall",bc_err);

						agk::TerminateApp(debug_is_running);
					}
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cancel");

			}
			else {
				if (ImGui::ImgBtn(iToolbarImages[9], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down) || pressedFkey == 8) {
					//Run
					if (pCurrentSelectedProject) {

						if (!bRemote_broadcast_is_running && broadcast_is_running) {
							//Cancel a local broadcast session before starting a debug session.
							broadcast_is_running = false;
							uString bc_err;
							BroadcasterCommand("stop",bc_err);
							if (bc_err.GetLength() > 0)
								AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
							BroadcasterCommand("disconnectall",bc_err);
							if (bc_err.GetLength() > 0)
								AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
							//Give it some time to clean up.
#ifdef AGK_WINDOWS
							Sleep(500);
#else
							usleep(500);
#endif

						}

						bAutoState = bAutoContinue = false; // disable autorun when starting debugger.
						CompileProject(pCurrentSelectedProject, false, false, true); // flags: run,broadcast,debug.
						bToolbarImagesState[9] = true;
						szNextWindowFocus = "Debugger";
						cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
					}
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[9]);
			}
		}

		if (1 == 2) { // Menu item only ?
			if (!debug_is_running) {
				if (strlen(pref.cDebugIp) > 7) {

					if (remote_debug_is_running) {
						ImGui::SameLine();
						char szTmp[MAX_PATH];
						sprintf(szTmp, "Stop remote debug on: %s", pref.cDebugIp);
						if (ImGui::Button(szTmp)) {
							uString bc_err;
							BroadcasterCommand("stop",bc_err);
							BroadcasterCommand("disconnectall",bc_err);
							remote_debug_is_running = 0;
						}
					}
					else {
						ImGui::SameLine();
						char szTmp[MAX_PATH];
						sprintf(szTmp, "Remote debug on: %s", pref.cDebugIp);
						if (ImGui::Button(szTmp)) {
							bAutoState = bAutoContinue = false; // disable autorun when starting debugger.
							CompileProject(pCurrentSelectedProject, false, false, true, pref.cDebugIp); // flags: run,broadcast,debug.
							szNextWindowFocus = "Debugger";
							cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
						}
					}
				}
			}
		}
		ImGui::SameLine();

		//Additional buttons if debugger is running.
		if (debug_is_running || remote_debug_is_running) {

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 30, ImGui::GetCursorPos().y));

			if (ImGui::Button("break")) {
				bBreakJustStarted = true;
				bAutoState = bAutoContinue = false;
				uString bc_err;
				BroadcasterCommand("pause",bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
			}
			ImGui::SameLine();

			if (ImGui::Button("continue") || pressedFkey == 5 ) {
				uString bc_err;
				BroadcasterCommand("continue",bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
				ClearExecuteLine();
			}
			ImGui::SameLine();

			if (ImGui::Button("step") || (pressedFkey == 10 && !io.KeyShift) ) {
				bBreakJustStarted = false;
				uString bc_err;
				BroadcasterCommand("step",bc_err);
				if (bc_err.GetLength() > 0)
					AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
			}
			ImGui::SameLine();

			if (ImGui::Button("stepover") || (pressedFkey == 11 && !io.KeyShift) ) {
				if (bBreakJustStarted) {
					uString bc_err;
					BroadcasterCommand("step",bc_err);
					if (bc_err.GetLength() > 0)
						AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
				}
				else {
					uString bc_err;
					BroadcasterCommand("stepover",bc_err);
					if (bc_err.GetLength() > 0)
						AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
				}
				bBreakJustStarted = false;
			}
			ImGui::SameLine();

			if (ImGui::Button("stepout") || (pressedFkey == 10 && io.KeyShift )) {
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
			ImGui::SameLine();


		}

		//Icon seperator
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 30, ImGui::GetCursorPos().y));

		if (ImGui::ImgBtn(iToolbarImages[10], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down)) {
			//Find
			if (m_ActiveEditor) {
				m_ActiveEditor->OpenFind();
			}
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[10]);
		ImGui::SameLine();

		//#######################
		//#### PLAYBOX       ####
		//#### Compile + run ####
		//#######################

		if (m_ActiveEditor) {

			cProjectItem::sProjectFiles * m_pCurrentFile;
			m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
			if (m_pCurrentFile && strcmp(m_pCurrentFile->m_sProjectName.GetStr(), "na-noproject") == 0 && m_pCurrentFile->m_sName.FindStr(".agc",1) > 0 ) {

				//ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 30, ImGui::GetCursorPos().y));

				if (bToolbarImagesState[11]) {
					//Is running.
					int still_running = agk::GetAppRunning(game_is_running);
					if (!still_running) {
						bToolbarImagesState[11] = false;
						game_is_running = 0;
						//PE: Restore window focus after running app.
#ifdef AGK_WINDOWS
						SetForegroundWindow(g_agkhWnd);
						SetFocus(g_agkhWnd);
						Sleep(100);
#endif
						if (m_ActiveEditor) {
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
							if (m_pCurrentFile) {
								cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
								bFocusActiveEditor = true;
							}
						}

					}
					if (ImGui::ImgBtn(iToolbarImagesDown[11], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down) || pressedFkey == 4) {
						//Cancel Run.
						if (game_is_running > 0)
							agk::TerminateApp(game_is_running);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cancel SandBox");
				}
				else {
					if (ImGui::ImgBtn(iToolbarImages[11], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down) || pressedFkey == 4) {
						//Run in sandbox
#ifdef AGK_WINDOWS
						_chdir(startupFolder);
#else
						chdir(startupFolder);
#endif

//#ifndef AGK_MACOS
						//PE: Copy everything to a writeable folder.

						uString src_folder, dest_folder, check_folder;

						src_folder = startupFolder;
						src_folder.Append("/media/sandbox");

						dest_folder = defaultWriteFolder;
						dest_folder.Append("sandbox");

						check_folder = dest_folder;
						check_folder.Append("/media/bytecode.byc");
						//PE: If already there dont copy.
						if(!file_exists( (char *) check_folder.GetStr() )) {
							CopyDirTo(src_folder.GetStr(), dest_folder.GetStr());
						}
//#endif

						auto textToSave = m_ActiveEditor->GetText();
#ifndef DISABLESAVE
						std::ofstream myfile;
//#ifdef AGK_WINDOWS
//#ifndef AGK_MACOS
						dest_folder.Append("/main.agc");
						myfile.open( dest_folder.GetStr() );
//#else
//						myfile.open("media/sandbox/main.agc");
//#endif

						myfile << textToSave;
						myfile.close();
#endif

						textToSave = "";
						CompileProject(NULL, true, false, false); // flags: run,broadcast,debug.

						//PE: Restore window focus after running app.
						if (m_ActiveEditor) {
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
							if (m_pCurrentFile) {
								cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
								bFocusActiveEditor = true;
							}
						}

						bToolbarImagesState[11] = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[11]);
				}

				ImGui::SameLine();
			}
		}


		//Mac only , change keyboard layout.
#ifdef AGK_MACOS
		//Keyboard layout toggle selection.
		//ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 30, ImGui::GetCursorPos().y));
		if (keyboard_layout == 0) {
			//keyboard_layout = 0, current PC keylayout.
			if (ImGui::ImgBtn(iToolbarImagesDown[12], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down)) {
				keyboard_layout = 1;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[12]);
			ImGui::SameLine();
		}
		else {
			//keyboard_layout = 1, current Mac keylayout.
			if (ImGui::ImgBtn(iToolbarImages[12], iToolbarIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down)) {
				keyboard_layout = 0;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", iToolbarText[12]);
			ImGui::SameLine();
		}
#endif

		if (m_ActiveEditor) {

			//Test shaders.
			cProjectItem::sProjectFiles * m_pCurrentFile;
			m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);

			if (m_pCurrentFile && strcmp(m_pCurrentFile->m_sProjectName.GetStr(), "na-noproject") == 0 && m_pCurrentFile->m_sName.FindStr(".ps", 1) > 0) {

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 30, ImGui::GetCursorPos().y));
				if (ImGui::Button(" Test Fragment Shader ", ImVec2(0, 32))) {
					m_CompiledEditor = m_ActiveEditor;
					CompileShader(NULL, m_ActiveEditor->GetText().c_str());
				}
				ImGui::SameLine();
			}
			if (m_pCurrentFile && strcmp(m_pCurrentFile->m_sProjectName.GetStr(), "na-noproject") == 0 && m_pCurrentFile->m_sName.FindStr(".vs", 1) > 0) {

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 30, ImGui::GetCursorPos().y));
				if (ImGui::Button(" Test Vertex Shader ", ImVec2(0, 32))) {
					m_CompiledEditor = m_ActiveEditor;
					CompileShader(m_ActiveEditor->GetText().c_str(), NULL);
				}
				ImGui::SameLine();
			}
		}

		//###################
		//#### Main menu ####
		//###################

		if (ImGui::BeginMenuBar())
		{

			if (ImGui::BeginMenu("File")  )
			{
				//Printing tests.
//				if (ImGui::MenuItem("TEST")) {
//					ShellExecuteA(g_agkhWnd, "open", "d:\\test.html", NULL, NULL, SW_SHOWNORMAL);//open in browser.
//					CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
//					ShellExecuteA(g_agkhWnd, "print", "d:\\test.txt", NULL, NULL, SW_NORMAL); //Works
//					ShellExecuteA(g_agkhWnd, "print", "d:\\test.txt", NULL, NULL, SW_NORMAL); //dont work with html.
//				}


				if (ImGui::MenuItem("New Project")) {
					extern char projectfolder[MAX_PATH];
					if (strlen(pref.cDefaultProjectFolder) > 1)
						strcpy(projectfolder, pref.cDefaultProjectFolder);

					create_new_project = true;
				}
				if (ImGui::MenuItem("New Source File")) {


					if ( pCurrentSelectedProject  && askBox("Add Source File To Current Project ?", "New Source File")) {
						add_new_project_file = true;
					}
					else {
						cProjectItem::sProjectFiles * m_pCurrentFile;
						m_pCurrentFile = CreateNewAGCFile(NULL);
						if (m_pCurrentFile) {
							vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
							m_pCurrentFile->m_bOpenForEdit = true;
							m_pCurrentFile->m_editor->SetText(""); //Always make it blank.
							cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
							bFocusActiveEditor = true;
						}
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Open Project", pref.cOpenProjectText)) {
					OpenProject();
					isProjectInAssetsBrowser();
					setVisibleProjectsInAssetBrowser();
				}

				if (ImGui::MenuItem("Open Source File", pref.cOpenFileText) ) {

					char * cFileSelected;
					cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, NULL, NULL, NULL);
					AddSourceFile(cFileSelected);
/*
					if (cFileSelected && strlen(cFileSelected) > 0) {

						cProjectItem::sProjectFiles * m_pCurrentFile;
						m_pCurrentFile = CreateNewAGCFile(cFileSelected);
						AddRecentFiles(NULL, cFileSelected);
						if (m_pCurrentFile) {
							vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
							m_pCurrentFile->m_bOpenForEdit = true;
							cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
						}
					}
*/
				}


				if (iResentProjects > 0 || iResentFiles > 0)
					ImGui::Separator();

				if (iResentProjects > 0) {
					if (ImGui::BeginMenu("Recent Projects"))
					{
						//Invert order.
						if (iResentProjects > 0) {
							for (int i = iResentProjects - 1; i >= 0; i--) {
								if (szRecentProjects[i].GetLength() > 0) {
									if (ImGui::MenuItem(szRecentProjects[i])) {
										//open project.
										LoadProjectFile((char *)szRecentProjects[i].GetStr());
										isProjectInAssetsBrowser();
										setVisibleProjectsInAssetBrowser();
									}
								}
							}
						}
						/*
						for (int i = 0; i < iResentProjects; i++) {
							if (ImGui::MenuItem(szRecentProjects[i])) {
								//open project.
								LoadProjectFile((char *)szRecentProjects[i].GetStr());
								isProjectInAssetsBrowser();
								setVisibleProjectsInAssetBrowser();
							}
						}
						*/
						ImGui::EndMenu();
					}
				}
				if (iResentFiles > 0) {
					if (ImGui::BeginMenu("Recent Files"))
					{
						//Invert order.
						if (iResentFiles > 0) {
							for (int i = iResentFiles - 1; i >= 0; i--) {
								if (szRecentFiles[i].GetLength() > 0) {
									if (ImGui::MenuItem(szRecentFiles[i])) {
										//open file.
										cProjectItem::sProjectFiles * m_pCurrentFile;
										m_pCurrentFile = CreateNewAGCFile((char *)szRecentFiles[i].GetStr());
										if (m_pCurrentFile) {
											vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
											m_pCurrentFile->m_bOpenForEdit = true;
											cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
										}
									}
								}
							}
						}

						/*
						for (int i = 0; i < iResentFiles; i++) {
							if (ImGui::MenuItem(szRecentFiles[i])) {
								//open file.
								cProjectItem::sProjectFiles * m_pCurrentFile;
								m_pCurrentFile = CreateNewAGCFile( (char *) szRecentFiles[i].GetStr() );
								if (m_pCurrentFile) {
									vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
									m_pCurrentFile->m_bOpenForEdit = true;
									cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
								}
							}
						}
						*/
						ImGui::EndMenu();
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Export Project to Android")) {
					//From Mike , compile before export.
					if (pCurrentSelectedProject) CompileProject(pCurrentSelectedProject, false, false, false); // flags: run,broadcast,debug.

#ifdef TRIALVERSIONEXPIRES
					trialaction = 1;
#else
				#ifdef AGK_MACOS
					// Mac must check for Java and download additional export files to get around Notarization problems
					if ( !agk::GetFileExists( "raw:/usr/bin/jarsigner" ) )
					{
						if ( askBox("Due to changes by Apple you must install JDK 14 or above to export for Android. JDK 14 can be found on the Oracle website. Would you like to open their downloads page?", "Download JDK") )
						{
							agk::OpenBrowser( "https://www.oracle.com/java/technologies/downloads/" );
						}
					}
					else
					{
						// check additional files
						uString sExportPath = "raw:";
						sExportPath.Append( defaultWriteFolder );
						sExportPath.AppendUTF8( "AndroidExport/aapt2-bundle" );

						if ( !agk::GetFileExists( sExportPath ) )
						{
							if ( AndroidExportHTTP < 0 )
							{
								if ( askBox("The Android export files must be downloaded separately. Would you like to download them now?", "Download Android Export Files") )
								{
									BoxerInfo( "The files will download in the background", "Download Android Export Files" );
									AndroidExportHTTPTimer = agk::Timer();
									AndroidExportHTTP = agk::CreateHTTPConnection();
									agk::SetHTTPHost( AndroidExportHTTP, "www.appgamekit.com", 1 );
									agk::SetHTTPTimeout( AndroidExportHTTP, 10000 ); // 10 seconds

									sExportPath = "raw:";
									sExportPath.Append( defaultWriteFolder );
									sExportPath.AppendUTF8( "AndroidExport.zip" );

									uString sURLPath = "agkstudiofiles/AndroidExport";
									sURLPath.AppendUInt( INTVERSION );
									sURLPath.AppendUTF8( ".zip" );
									agk::GetHTTPFile( AndroidExportHTTP, sURLPath, sExportPath );

									show_androidexport_window = true;
								}
							}
						}
						else
						{
							show_androidexport_window = true;
						}						
					}
				#else
					show_androidexport_window = true;
				#endif
#endif
				}

#ifdef AGK_MACOS
				if (ImGui::MenuItem("Export Project to iOS")) {

					//PE: Compile before export.
					if (pCurrentSelectedProject) CompileProject(pCurrentSelectedProject, false, false, false); // flags: run,broadcast,debug.

#ifdef TRIALVERSIONEXPIRES
					trialaction = 1;
#else
					show_iosexport_window = true;
#endif
				}
#endif

				if (ImGui::MenuItem("Export Project to HTML5")) {
					//Compile before export.
					if (pCurrentSelectedProject) CompileProject(pCurrentSelectedProject, false, false, false); // flags: run,broadcast,debug.

#ifdef TRIALVERSIONEXPIRES
					trialaction = 1;
#else
					show_html5export_window = true;
#endif
				}

//				if (ImGui::MenuItem("Publish to workshop")) {
//				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					// save text....
					if (m_ActiveEditor) {
						cProjectItem::sProjectFiles * m_pCurrentFile;
						m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);

						if (m_pCurrentFile) {

							if (m_pCurrentFile->m_bUseSaveAs) {
								cProjectItem::sProjectFiles * m_pCurrentFile;
								m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
								if (m_pCurrentFile) {

									uString mPath = m_pCurrentFile->m_sFullPath;
									int pos = mPath.RevFindStr("/");
									if (pos < 0) pos = mPath.RevFindStr("\\");
									if (pos > 0) {
										m_pCurrentFile->m_sFullPath.SubString(mPath, 0, pos);
									}
									char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "agc\0*.agc\0", mPath.GetStr(), NULL);
									if (cFileSelected && strlen(cFileSelected) > 0) {

										uString checkfile = cFileSelected;
										pos = checkfile.RevFindStr(".agc");
										if (pos < 0)
											checkfile.Append(".agc");

										bool oksave = true;
										if (file_exists((char*)checkfile.GetStr())) {
											oksave = overWriteFileBox((char*)checkfile.GetStr());
										}
										if (oksave) {
											auto textToSave = m_ActiveEditor->GetText();
											std::ofstream myfile;

											m_pCurrentFile->m_sFullPath = checkfile;
											uString newname = "na";
											int pos = checkfile.RevFindStr("/");
											if (pos <= 0) pos = checkfile.RevFindStr("\\");
											if (pos > 0) {
												checkfile.SubString(newname, pos + 1);
												//m_pCurrentFile->m_sProjectName = "na-noproject";
												m_pCurrentFile->m_sName = newname;
												m_pCurrentFile->m_sEditName = m_pCurrentFile->m_sName;
												if (pref.bRemovePathFromTabName) {
													uString newTabname = m_pCurrentFile->m_sEditName;
													int ntb_pos = newTabname.RevFindStr("/");
													if(ntb_pos<=0)
														ntb_pos = newTabname.RevFindStr("\\");
													if (ntb_pos > 0)
														newTabname.SubString(m_pCurrentFile->m_sEditName, ntb_pos + 1);
												}
												m_pCurrentFile->m_sEditName.Append("##");
												m_pCurrentFile->m_sEditName.Append(m_pCurrentFile->m_sProjectName);
												m_pCurrentFile->m_sEditName.Append(RandomString());
												cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
											}

											m_pCurrentFile->m_bUseSaveAs = false;
#ifndef DISABLESAVE
											myfile.open(checkfile);
											myfile << textToSave;
											myfile.close();
#endif
											m_ActiveEditor->filechanged = false;
											m_ActiveEditor->firstrun = true;
										}
									}
								}
							}
							else {

								auto textToSave = m_ActiveEditor->GetText();
#ifndef DISABLESAVE
								std::ofstream myfile;
								myfile.open((char*)m_pCurrentFile->m_sFullPath.GetStr());
								myfile << textToSave;
								myfile.close();
								m_ActiveEditor->filechanged = false;
#endif
							}
						}
					}

				}
				if (ImGui::MenuItem("Save As"))
				{
					// save text as....
					if (m_ActiveEditor) {
						cProjectItem::sProjectFiles * m_pCurrentFile;
						m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
						if (m_pCurrentFile) {
							uString mPath = m_pCurrentFile->m_sFullPath;
							int pos = mPath.RevFindStr("/");
							if( pos < 0) pos = mPath.RevFindStr("\\");
							if (pos > 0) {
								m_pCurrentFile->m_sFullPath.SubString(mPath,0, pos);
							}
							char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "agc\0*.agc\0", mPath.GetStr(), NULL);
							if (cFileSelected && strlen(cFileSelected) > 0) {
								uString checkfile = cFileSelected;
								pos = checkfile.RevFindStr(".agc");
								if (pos < 0)
									checkfile.Append(".agc");

								bool oksave = true;
								if( file_exists((char*) checkfile.GetStr())) {
									oksave = overWriteFileBox((char*)checkfile.GetStr());
								}
								if(oksave) {
									auto textToSave = m_ActiveEditor->GetText();
#ifndef DISABLESAVE
									std::ofstream myfile;
									myfile.open(checkfile);
									myfile << textToSave;
									myfile.close();
#endif
									//close this.
									m_ActiveEditor->filechanged = false;
									//open file.
									cProjectItem::sProjectFiles * m_pCurrentFile;
									m_pCurrentFile = CreateNewAGCFile((char *)checkfile.GetStr());
									if (m_pCurrentFile) {
										m_ActiveEditor->quiteditor = true;
										vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
										m_pCurrentFile->m_bOpenForEdit = true;
										cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									}

								}
							}
						}
					}
				}

				ImGui::Separator();

				bool pactive = true;
				uString sMenuName;
				if (pCurrentSelectedProject == NULL) {
					sMenuName = "Close Project";
					pactive = false;
				}
				else {
					sMenuName = "Close Project";
					sMenuName.Append(" '");
					sMenuName.Append(pCurrentSelectedProject->m_sProjectFile);
					sMenuName.Append("'");
				}

				if (ImGui::MenuItem(sMenuName, nullptr, nullptr, pactive)) {
					if (pCurrentSelectedProject) SaveProjectFile(pCurrentSelectedProject);
					if (pCurrentSelectedProject) RemoveProject(pCurrentSelectedProject);
					setVisibleProjectsInAssetBrowser();
				}
				if (ImGui::MenuItem("Close All Projects", nullptr, nullptr, pactive)) {
					if (pCurrentSelectedProject) SaveProjectFile(NULL);
					if (pCurrentSelectedProject) RemoveProject(NULL);
					setVisibleProjectsInAssetBrowser();
				}

				ImGui::Separator();

				if (m_ActiveEditor) {
					if (ImGui::MenuItem("Close Source File")) {
						m_ActiveEditor->quiteditor = true;
					}
				}

				if (ImGui::MenuItem("Close All Source Files")) {
					CloseAllOpenProjectEditedFiles();
				}

				if (m_ActiveEditor) {
					if (ImGui::MenuItem("Close All, Keep Active")) {
						CloseAllOpenProjectEditedFiles();
						m_ActiveEditor->quiteditor = false;
					}
				}


				ImGui::Separator();

				if (ImGui::MenuItem("Quit")) {
#ifdef TRIALVERSIONEXPIRES
					trialaction = 9;
#else
					quit_app = 1;
#endif
					//Quit code.
				}
				
				ImGui::EndMenu();

			}


			//Is current active window a 2D scene ?
			if (m_ActiveScene && m_ActiveScene->m_SceneEditor && m_ActiveScene->m_SceneEditor == m_ActiveEditor && !m_ActiveScene->bDisplayAsText ) {

				//!m_pCurFile->m_scene->bDisplayAsText
				if (ImGui::BeginMenu("Edit"))
				{
					bool ro = false;
					int mi_flag = false;

					mi_flag = true; //m_ActiveScene->CanUndo();
					if (ImGui::MenuItem("Undo", pref.cUndoText, nullptr, !ro && mi_flag))
						m_ActiveScene->Undo();

					mi_flag = true; //m_ActiveEditor->CanRedo();
					if (ImGui::MenuItem("Redo", pref.cRedoText, nullptr, !ro && mi_flag))
						m_ActiveScene->Redo();

					ImGui::Separator();

					mi_flag = m_ActiveScene->m_pSelectedObject!=NULL;
					if (ImGui::MenuItem("Copy", pref.cCopyText, nullptr, mi_flag)) {
						m_ActiveScene->CopyObject(m_ActiveScene->m_pSelectedObject);
					}
					if (ImGui::MenuItem("Cut", pref.cCutText, nullptr, !ro && mi_flag)) {
						m_ActiveScene->Delete();
					}
					if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && mi_flag)) {
						m_ActiveScene->Delete();
					}

					mi_flag = m_ActiveScene->bCopyObjectActive;
					if (ImGui::MenuItem("Paste", pref.cPasteText, nullptr, !ro && mi_flag ))
						m_ActiveScene->PasteObject();

					ImGui::Separator();

					if (ImGui::MenuItem("Preferences", nullptr, nullptr)) {
						pref.show_preferences_window = true;
					}
					
					ImGui::EndMenu();
				}

			}
			else
			{

				if (ImGui::BeginMenu("Edit"))
				{
					bool ro = true;
					if (m_ActiveEditor) {
						ro = m_ActiveEditor->IsReadOnly();
					}
					int mi_flag = false;
					mi_flag = false; if (m_ActiveEditor) mi_flag = m_ActiveEditor->CanUndo();
					if (ImGui::MenuItem("Undo", pref.cUndoText, nullptr, !ro && mi_flag))
						if (m_ActiveEditor) m_ActiveEditor->Undo();

					mi_flag = false; if (m_ActiveEditor) mi_flag = m_ActiveEditor->CanRedo();
					if (ImGui::MenuItem("Redo", pref.cRedoText, nullptr, !ro && mi_flag))
						if (m_ActiveEditor) m_ActiveEditor->Redo();

					ImGui::Separator();

					mi_flag = false; if (m_ActiveEditor) mi_flag = m_ActiveEditor->HasSelection();
					if (ImGui::MenuItem("Copy", pref.cCopyText, nullptr, mi_flag))
						if (m_ActiveEditor) m_ActiveEditor->Copy();
					if (ImGui::MenuItem("Cut", pref.cCutText, nullptr, !ro && mi_flag))
						if (m_ActiveEditor) m_ActiveEditor->Cut();
					if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && mi_flag))
						if (m_ActiveEditor) m_ActiveEditor->Delete();

					if (ImGui::MenuItem("Paste", pref.cPasteText, nullptr, !ro && ImGui::GetClipboardText() != nullptr))
						if (m_ActiveEditor) m_ActiveEditor->Paste();

					ImGui::Separator();

					if (ImGui::MenuItem("Select All", nullptr, nullptr))
						if (m_ActiveEditor) m_ActiveEditor->SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(m_ActiveEditor->GetTotalLines(), 0));

					ImGui::Separator();

					if (ImGui::MenuItem("Preferences", nullptr, nullptr)) {
						pref.show_preferences_window = true;
					}
					
					ImGui::EndMenu();
				}
			}

			if (ImGui::BeginMenu("Search"))
			{

				if (ImGui::MenuItem("Find", pref.cFindText)) {
					if (m_ActiveEditor) m_ActiveEditor->OpenFind();
				}
				if (ImGui::MenuItem("Find Next", pref.cFindNextText)) {
					if (m_ActiveEditor) m_ActiveEditor->FindNext();
				}
				if (ImGui::MenuItem("Find Previous")) {
					if (m_ActiveEditor) m_ActiveEditor->FindPrev();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Replace", pref.cReplaceText )) {
					if (m_ActiveEditor) m_ActiveEditor->Replace();
				}

				ImGui::Separator();

				//if (ImGui::MenuItem("Go to Next Mark")) {
				//}
				//if (ImGui::MenuItem("Go to Previous Mark")) {
				//}

				ImGui::Separator();

				if (ImGui::MenuItem("Go to Line", pref.cGotoLineText)) {
					show_gotoline_window = true;
				}
				
				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("View"))
			{

				if (ImGui::BeginMenu("Change Font")) {

					if (ImGui::SliderInt("Editor Font Size", &pref.iEditorFontSize, 8, 64)) {
						if (changefonttype == 0)
							changefonttype = 999;
					}
//					if (ImGui::MenuItem("Editor: Set Font Size")) {
//						changefonttype = 999;
//					}

					if (ImGui::MenuItem("Hack Regular")) {
						changefonttype = 1;
						strcpy(pref.cCustomEditorFont, "");
					}
					if (ImGui::MenuItem("Hack Bold")) {
						changefonttype = 2;
						strcpy(pref.cCustomEditorFont, "");
					}
					if (ImGui::MenuItem("Hack Italic")) {
						changefonttype = 3;
						strcpy(pref.cCustomEditorFont, "");
					}
					if (ImGui::MenuItem("Deja Vu Sans Mono Regular")) {
						changefonttype = 4;
						strcpy(pref.cCustomEditorFont, "");
					}
					if (ImGui::MenuItem("Deja Vu Sans Mono Bold")) {
						changefonttype = 5;
						strcpy(pref.cCustomEditorFont, "");
					}

					ImGui::Separator();

					if (ImGui::SliderInt("IDE Font Size", &pref.iIDEFontSize, 8, 64))
					{
						//This will change the position of the menu, so
						//if (changefonttype == 0)
						//	changefonttype = 99;
					}
					if (ImGui::MenuItem("IDE: Set Font Size")) {
						changefonttype = 999;
					}
					ImGui::EndMenu();
				}


				if (ImGui::BeginMenu("Change Color Scheme")) {
					extern bool bEnableSeedStyleChanged;
					if (ImGui::MenuItem("Default Style")) {
						pref.bEnableSeedStyle = false;
						bEnableSeedStyleChanged = true;
						myDefaultStyle(NULL);
						ide_force_rendering_delayed = true;
						pref.idePalette = 0;
					}
					if (ImGui::MenuItem("Dark Style")) {
						pref.bEnableSeedStyle = false;
						bEnableSeedStyleChanged = true;
						myDarkStyle(NULL);
						ide_force_rendering_delayed = true;
						pref.idePalette = 1;
					}
					if (ImGui::MenuItem("Classic Style")) {
						pref.bEnableSeedStyle = false;
						bEnableSeedStyleChanged = true;
						ImGui::StyleColorsClassic();
						ide_force_rendering_delayed = true;
						pref.idePalette = 2;
					}

					if (ImGui::MenuItem("Light Style")) {
						pref.bEnableSeedStyle = false;
						bEnableSeedStyleChanged = true;
						myLightStyle(NULL);
						ide_force_rendering_delayed = true;
						pref.idePalette = 3;
					}
					if (ImGui::MenuItem("Dark Grey Style")) {
						pref.bEnableSeedStyle = false;
						bEnableSeedStyleChanged = true;
						myDarkGreyStyle(NULL);
						ide_force_rendering_delayed = true;
						pref.idePalette = 4;
					}

					ImGui::Separator();
					if (ImGui::MenuItem("Editor: Default Style"))
						if (m_ActiveEditor) m_ActiveEditor->SetPalette(TextEditor::GetAGKPalette());
					if (ImGui::MenuItem("Editor: Dark Style"))
						if (m_ActiveEditor) m_ActiveEditor->SetPalette(TextEditor::GetDarkPalette());
					if (ImGui::MenuItem("Editor: Light Style"))
						if (m_ActiveEditor) m_ActiveEditor->SetPalette(TextEditor::GetLightPalette());
					if (ImGui::MenuItem("Editor: Retro Blue Style"))
						if (m_ActiveEditor) m_ActiveEditor->SetPalette(TextEditor::GetRetroBluePalette());
					
					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Reset Layout")) {

					if (askBox("Are you sure ?", "Reset Layout")) {
						pref.bDisplayHelpWindow = true;
						pref.bDisplayMessageWindow = true;
						pref.bDisplayProjectWindow = true;
						pref.bDisplayDebuggerWindow = true;
						pref.bDisplayMediaFilesWindow = true;
						pref.bDisplayPreviewWindow = true;
						pref.bDisplayAssetsBrowserWindow = true;
						pref.bDisplaySceneManager = true;
						pref.bDisplaySceneProperties = true;
						ResetLayout();
					}

				}

//				if (ImGui::MenuItem("Save Layout")) {
//					char cmLayoutFile[MAX_PATH];
//					sprintf(cmLayoutFile, "%sagkide.layout", defaultWriteFolder);
//					ImGui::SaveIniSettingsToDisk(cmLayoutFile);
//				}


				ImGui::Separator();
/*
				if (ImGui::MenuItem("Show Markers Margin")) {
				}
*/
				if (pref.editor_display_linenumbers) {
					if (ImGui::MenuItem("Hide Line Numbers")) {
						pref.editor_display_linenumbers = false;
					}
				}
				else {
					if (ImGui::MenuItem("Show Line Numbers")) {
						pref.editor_display_linenumbers = true;
					}
				}
/*
				if (ImGui::MenuItem("Show White Space")) {
				}
				if (ImGui::MenuItem("Show Line Endings")) {
				}
				if (ImGui::MenuItem("Show Indentation Guides")) {
				}
*/
				ImGui::Separator();
				if (!agkfullscreen) {
					if (ImGui::MenuItem("Full Screen")) {
						agkfullscreen = 1 - agkfullscreen;
						agkOldDeviceWeight = agk::GetDeviceWidth();
						agkOldDeviceHeight = agk::GetDeviceHeight();
						agkFullscreenTimer = agk::Timer();
						bAgkFullscreenTrigger = true;
						agk::SetWindowSize(agk::GetDeviceWidth(), agk::GetDeviceHeight(), agkfullscreen);
						agk::SetRawMouseVisible(1); //count internal windows mouse visible counter up.
					}
				}
				else {
					if (ImGui::MenuItem("Exit Full Screen")) {
						agkfullscreen = 1 - agkfullscreen;
						if(agkOldDeviceWeight > 0 && agkOldDeviceHeight > 0 )
							agk::SetWindowSize(agkOldDeviceWeight, agkOldDeviceHeight, agkfullscreen);
						else
							agk::SetWindowSize(agk::GetDeviceWidth(), agk::GetDeviceHeight(), agkfullscreen);
						agk::SetRawMouseVisible(1); //count internal windows mouse visible counter up.
					}
				}

				if (ImGui::BeginMenu("Windows"))
				{

					if (pref.bDisplayMessageWindow) {
						if (ImGui::MenuItem("Hide Message Window")) {
							pref.bDisplayMessageWindow = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Message Window")) {
							pref.bDisplayMessageWindow = true;
						}
					}

					if (pref.bDisplayHelpWindow) {
						if (ImGui::MenuItem("Hide Help Window")) {
							pref.bDisplayHelpWindow = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Help Window")) {
							pref.bDisplayHelpWindow = true;
						}
					}

					if (pref.bDisplayProjectWindow) {
						if (ImGui::MenuItem("Hide Project Window")) {
							pref.bDisplayProjectWindow = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Project Window")) {
							pref.bDisplayProjectWindow = true;
						}
					}

					if (pref.bDisplayDebuggerWindow) {
						if (ImGui::MenuItem("Hide Debug Window")) {
							pref.bDisplayDebuggerWindow = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Debug Window")) {
							pref.bDisplayDebuggerWindow = true;
						}
					}

					if (pref.bDisplayMediaFilesWindow) {
						if (ImGui::MenuItem("Hide Media Files Window")) {
							pref.bDisplayMediaFilesWindow = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Media Files Window")) {
							pref.bDisplayMediaFilesWindow = true;
						}
					}


					if (pref.bDisplayPreviewWindow) {
						if (ImGui::MenuItem("Hide Preview Window")) {
							pref.bDisplayPreviewWindow = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Preview Window")) {
							pref.bDisplayPreviewWindow = true;
						}
					}

					if (pref.bDisplayAssetsBrowserWindow) {
						if (ImGui::MenuItem("Hide Assets Browser Window")) {
							pref.bDisplayAssetsBrowserWindow = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Assets Browser Window")) {
							pref.bDisplayAssetsBrowserWindow = true;
						}
					}

					if (pref.bDisplaySceneManager) {
						if (ImGui::MenuItem("Hide Scene Manager")) {
							pref.bDisplaySceneManager = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Scene Manager")) {
							pref.bDisplaySceneManager = true;
						}
					}

					if (pref.bDisplaySceneProperties) {
						if (ImGui::MenuItem("Hide Scene Properties")) {
							pref.bDisplaySceneProperties = false;
						}
					}
					else {
						if (ImGui::MenuItem("Show Scene Properties")) {
							pref.bDisplaySceneProperties = true;
						}
					}


					

					ImGui::EndMenu();
				}

/*
				if (ImGui::MenuItem("Show Toolbar")) {
				}
				if (ImGui::MenuItem("Show Sidebar")) {
				}
*/

				ImGui::Separator();

				if (ImGui::MenuItem("Zoom In", pref.cZoomInText) ) {
					if (pref.iEditorFontSize < 64)
						pref.iEditorFontSize++;
					changefonttype = 999;
				}
				if (ImGui::MenuItem("Zoom Out", pref.cZoomOutText ) ) {
					if (pref.iEditorFontSize > 6)
						pref.iEditorFontSize--;
					changefonttype = 999;
				}
				if (ImGui::MenuItem("Normal Size")) {
					pref.iEditorFontSize = 15;
					changefonttype = 999;
				}
				
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Build"))
			{
				if (ImGui::MenuItem("Compile", pref.cCompileText )) {
					if (pCurrentSelectedProject) CompileProject(pCurrentSelectedProject, false, false, false); // flags: run,broadcast,debug.
				}


				//#### Compile + run ####
				if (bToolbarImagesState[7]) {
					//Is running.
					int still_running = agk::GetAppRunning(game_is_running);
					if (!still_running) {
						bToolbarImagesState[7] = false;
						game_is_running = 0;

						//PE: Restore window focus after running app.
#ifdef AGK_WINDOWS
						SetForegroundWindow(g_agkhWnd);
						SetFocus(g_agkhWnd);
						Sleep(100);
#endif
						if (m_ActiveEditor) {
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
							if (m_pCurrentFile) {
								cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
								bFocusActiveEditor = true;
							}
						}

					}
					if (ImGui::MenuItem("Cancel Run", pref.cRunText)) {
						//Cancel Run.
						if (game_is_running > 0)
							agk::TerminateApp(game_is_running);
					}
				}
				else {
					if (ImGui::MenuItem("Run", pref.cRunText )) {
						//Run
						if (pCurrentSelectedProject) {
							CompileProject(pCurrentSelectedProject, true, false, false); // flags: run,broadcast,debug.
							bToolbarImagesState[7] = true;
						}
					}
					if (ImGui::MenuItem("FPS Run")) {
						//FPS Run - Freeze IDE while app is running to give it full speed.
						if (pCurrentSelectedProject) {
							CompileProject(pCurrentSelectedProject, true, false, false); // flags: run,broadcast,debug.
							bToolbarImagesState[7] = true;

							int still_running = agk::GetAppRunning(game_is_running);
							while (still_running) {
#ifdef AGK_WINDOWS
								Sleep(1200);
#else
								usleep(1200);
#endif
								still_running = agk::GetAppRunning(game_is_running);
							}
						}
					}
				}

				//#### Compile + Broadcast ####
				if (broadcast_is_running) {
					if (ImGui::MenuItem("Cancel Broadcast", pref.cBroadCastText )) {
						//Cancel broadcast.
						broadcast_is_running = false;
						uString bc_err;
						BroadcasterCommand("stop",bc_err);
						if (bc_err.GetLength() > 0)
							AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
						BroadcasterCommand("disconnectall",bc_err);
						if (bc_err.GetLength() > 0)
							AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
					}
				}
				else {
					if (ImGui::MenuItem("Broadcast", pref.cBroadCastText)) {
						//broadcast
#ifdef TRIALVERSIONEXPIRES
						trialaction = 1;
#else
						if (pCurrentSelectedProject) {
							CompileProject(pCurrentSelectedProject, false, true, false); // flags: run,broadcast,debug.
							bToolbarImagesState[8] = true;
						}
#endif
					}
				}

				if (strlen(pref.cBroadcastIp) > 7) {
					if (broadcast_is_running) {
						char szTmp[MAX_PATH];
						sprintf(szTmp, "Stop broadcast on: %s", pref.cBroadcastIp);
						if (ImGui::MenuItem( szTmp, "")) {
							//Cancel broadcast.
							broadcast_is_running = false;
							uString bc_err;
							BroadcasterCommand("stop",bc_err);
							if (bc_err.GetLength() > 0)
								AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
							BroadcasterCommand("disconnectall",bc_err);
							if (bc_err.GetLength() > 0)
								AddToCompilerLog(2, "Broadcaster: %s\n", bc_err.GetStr());
						}
					}
					else {
						char szTmp[MAX_PATH];
						sprintf(szTmp, "Broadcast on: %s", pref.cBroadcastIp);
						if (ImGui::MenuItem(szTmp, "")) {
							//broadcast
#ifdef TRIALVERSIONEXPIRES
							trialaction = 1;
#else
							if (pCurrentSelectedProject) {
								CompileProject(pCurrentSelectedProject, false, true, false, pref.cBroadcastIp); // flags: run,broadcast,debug.
								bToolbarImagesState[8] = true;
							}
#endif
						}
					}

				}

				if (!remote_debug_is_running) {
					if (debug_is_running) {
						if (ImGui::MenuItem("Cancel Debug", pref.cDebugText)) {
							if (debug_is_running > 0) {
								//Disconnect device before closing down app.
								uString bc_err;
								BroadcasterCommand("stop",bc_err);
								BroadcasterCommand("disconnectall",bc_err);
								agk::TerminateApp(debug_is_running);
							}
						}
					}
					else {
						if (ImGui::MenuItem("Debug", pref.cDebugText)) {
							if (pCurrentSelectedProject) {
								bAutoState = bAutoContinue = false; // disable autorun when starting debugger.
								CompileProject(pCurrentSelectedProject, false, false, true); // flags: run,broadcast,debug.
								bToolbarImagesState[9] = true;
							}
						}
					}
				}

				if (!debug_is_running) {
					if (strlen(pref.cDebugIp) > 7) {

						if (remote_debug_is_running) {
							char szTmp[MAX_PATH];
							sprintf(szTmp, "Stop debug on: %s", pref.cDebugIp);
							if (ImGui::MenuItem(szTmp, "")) {
							//if (ImGui::Button(szTmp)) {
								uString bc_err;
								BroadcasterCommand("stop",bc_err);
								BroadcasterCommand("disconnectall",bc_err);
								remote_debug_is_running = 0;
							}
						}
						else {
							char szTmp[MAX_PATH];
							sprintf(szTmp, "Remote debug on: %s", pref.cDebugIp);
							//if (ImGui::Button(szTmp)) {
							if (ImGui::MenuItem(szTmp, "")) {
								bAutoState = bAutoContinue = false; // disable autorun when starting debugger.
								CompileProject(pCurrentSelectedProject, false, false, true, pref.cDebugIp); // flags: run,broadcast,debug.
								szNextWindowFocus = "Debugger";
								cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
							}
						}
					}
				}

				//if (ImGui::MenuItem("Build Options")) {
				//	pref.show_preferences_window = true;
				//}
				//ImGui::Separator();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Word Count")) {
					 iWordCount = 0;
					if (m_ActiveEditor) {
						iWordCount = m_ActiveEditor->WordCount();
						delaydialog = 4;
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("<Android> Generate Keystore File")) {
					show_keystore_window = true;
				}

#ifdef AGK_WINDOWS
				if (ImGui::MenuItem("<Android> View AGK Player")) {

					uString openfolder;
#ifdef AGK_WINDOWS
					//openfolder = cSteamFolder;
					//openfolder.Append("Players\\Android");

					// \Players\Android\AppGameKit Player.apk
					openfolder = startupFolder;
					openfolder.Append("\\Players\\Android");
#else
					//Mac , Linux ?.
#endif
					agk::RunApp("explorer", openfolder.GetStr() );
				}
#endif

				ImGui::Separator();

				if (ImGui::MenuItem("Install Additional Files...")) {
					show_additionalfiles_window = true;
				}

				//if (ImGui::MenuItem("<IOS>Export AGK Player")) {
				//}

				ImGui::EndMenu();
			}

			if (!dlc_files.empty()) {
				if (ImGui::BeginMenu("DLC"))
				{
					for (std::vector<std::string>::iterator it = dlc_files.begin() ; it != dlc_files.end(); ++it) {
						if (it->size() > 0) {
							uString dlcname = it->c_str();
							if (ImGui::MenuItem(dlcname)) {

								uString steamDLC = startupFolder; // cSteamFolder (use for classic) ;
								steamDLC.Append("/DLC/");
								steamDLC.Append(dlcname);
#ifdef AGK_WINDOWS
								steamDLC.ReplaceStr("/", "\\");
								ITEMIDLIST *pidl = ILCreateFromPathA(steamDLC.GetStr());
								if (pidl) {
									SHELLEXECUTEINFO sei = { 0 };
									sei.cbSize = sizeof(sei);
									sei.fMask = SEE_MASK_IDLIST;
									sei.hwnd = NULL;
									sei.lpVerb = TEXT("explore");
									sei.lpIDList = pidl;
									sei.nShow = SW_SHOW;
									ShellExecuteEx(&sei);
//									SHOpenFolderAndSelectItems(pidl, 0, 0, 0); //Will only select the item , not go inside it.
									ILFree(pidl);
								}
#else
#ifdef AGK_MACOS
								//Mac cant open,locate folders in finder ? Test.
								steamDLC.ReplaceStr("\\", "/");
								NSString* path = [NSString stringWithUTF8String : steamDLC.GetStr()];
								NSArray *fileURLs = [NSArray arrayWithObjects : [NSURL fileURLWithPath : path], nil];
								[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];

#endif
#endif

							}
						}
					}
					ImGui::EndMenu();
				}
			}
			if (ImGui::BeginMenu("Help"))
			{

				if (ImGui::MenuItem("User Guide")) {
#ifdef AGK_WINDOWS
					uString usDoc = startupFolder;
					usDoc.Append("/media/Help/AppGameKit Studio User Guide.pdf");
					usDoc.ReplaceStr("\\", "/");
					agk::OpenBrowser(usDoc.GetStr());
					//agk::RunApp("open" , usDoc.GetStr() );
#else
					agk::OpenBrowser("http://publicdata.thegamecreators.com/Docs/AppGameKit%20Studio%20User%20Guide.pdf");
#endif
				}
				if (ImGui::MenuItem("Command Reference")) {
#ifdef AGK_WINDOWS
					uString usDoc = startupFolder;
					usDoc.Append("/media/Help/AppGameKit Studio Command References.pdf");
					usDoc.ReplaceStr("\\", "/");
					agk::OpenBrowser(usDoc.GetStr());
#else
					agk::OpenBrowser("http://publicdata.thegamecreators.com/Docs/AppGameKit%20Studio%20Command%20References.pdf");
#endif
				}

				if (ImGui::MenuItem("AppGameKit Documentation")) {
					agk::OpenBrowser("https://www.appgamekit.com/documentation-studio/home.html");
				}

				if (ImGui::MenuItem("Command Help")) {
					pref.bDisplayHelpWindow = true;
					processhelp((char*)"media/Help/commands.html", true);
					ImGui::SetWindowFocus( ICON_MD_HELP " Help");
				}

				if (ImGui::MenuItem("Help Home")) {
					pref.bDisplayHelpWindow = true;
					processhelp((char*)"media/Help/home.html", true);
					ImGui::SetWindowFocus( ICON_MD_HELP " Help");
				}
				if (ImGui::MenuItem("Video Tutorials")) {
					agk::OpenBrowser("https://www.youtube.com/playlist?list=PLlLxfPB9MrBtN8vFOZIUhotV9cs3VCV5Z");
				}
				//if (ImGui::MenuItem("Keyboard Shortcuts")) {
				//}

				ImGui::Separator();

				if (ImGui::MenuItem("AppGameKit Website")) {
					agk::OpenBrowser("https://www.appgamekit.com/");
				}
				if (ImGui::MenuItem("TheGameCreators Website")) {
					agk::OpenBrowser("https://www.thegamecreators.com/");
				}
				if (ImGui::MenuItem("Community Forum")) {
					agk::OpenBrowser("https://forum.thegamecreators.com/");
				}

				if (ImGui::MenuItem("AppGameKit Discord Group")) {
					agk::OpenBrowser("https://discord.gg/7ENUvnE");
				}

				//AppGameKit Discord Group

				ImGui::Separator();

				if (ImGui::MenuItem("AppGameKit Player for Android")) {
					agk::OpenBrowser("https://play.google.com/store/apps/details?id=com.thegamecreators.agk_player2");
				}
				if (ImGui::MenuItem("AppGameKit Player for IOS")) {
					agk::OpenBrowser("https://itunes.apple.com/us/app/appgamekit-player/id1071731293?mt=8");
				}


#ifdef TRIALVERSIONEXPIRES
				ImGui::Separator();
				if (ImGui::MenuItem("Upgrade To Full Version")) {
					//agk::OpenBrowser("https://www.appgamekit.com/studio");
					agk::OpenBrowser("https://store.steampowered.com/app/1024640/AppGameKit_Studio"); // required by Steam
				}
#endif

				ImGui::Separator();

				if (ImGui::MenuItem("About")) {
					extern bool enable_debug_gadgets;
					if ( (pref.show_preferences_window || show_iosexport_window) && !enable_debug_gadgets) {
						enable_debug_gadgets = true;
					}
					else {
						extern bool show_aboutbox_window;
						show_aboutbox_window = true;
					}
				}

				ImGui::EndMenu();
			}
			
			ImGui::EndMenuBar();
		}
		ImGui::End();


		//######################################################################
		//#### Default dockspace setup, how is our windows split on screen. ####
		//######################################################################

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking; //ImGuiWindowFlags_MenuBar
		viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, toolbar_size));
		ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, toolbar_size));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpaceAGK", &dockingopen, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		//We cant make all windows dock if all windows is NOT undocked first (.ini setup problem ), so refresh_gui_docking == 2
		if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockspace")) == NULL || refresh_gui_docking == 2)
		{
			//Default docking setup.
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
			//ImGui::DockBuilderAddNode(dockspace_id, viewport->Size); // Add empty node
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_Dockspace ); // Add empty node
			ImGui::DockBuilderSetNodePos(dockspace_id, viewport->Pos + ImVec2(0, toolbar_size) );
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size - ImVec2(0, toolbar_size) );

			ImGuiID dock_main_id = dockspace_id;
			if( pref.bEnableToolbarExtraLargeIcons )
				ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.10f, NULL, &dock_main_id);
			else if(pref.bEnableToolbarLargeIcons)
				ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.10f, NULL, &dock_main_id);
			else
				ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.10f, NULL, &dock_main_id);

			ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
			ImGuiID dock_id_rightDown = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.28f, NULL, &dock_id_right);
			ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, NULL, &dock_main_id);
			ImGuiID dock_id_leftDown = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.28f, NULL, &dock_id_left);
			ImGuiID dock_id_leftMiddle = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.60f, NULL, &dock_id_left);
			ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.285f, NULL, &dock_main_id);

			//ImGui::DockBuilderDockWindow("level1.scene", dock_main_id);
			ImGui::DockBuilderDockWindow(ICON_MD_HELP " Help", dock_id_right);
			ImGui::DockBuilderDockWindow("Properties", dock_id_right);
			ImGui::DockBuilderDockWindow("Code Properties", dock_id_right);

			ImGui::DockBuilderDockWindow("Project", dock_id_left);
			ImGui::DockBuilderDockWindow("Debugger", dock_id_left);
			ImGui::DockBuilderDockWindow("Scene Manager", dock_id_leftMiddle);

			ImGui::DockBuilderDockWindow(ICON_MD_PERM_MEDIA " Preview", dock_id_rightDown);
			ImGui::DockBuilderDockWindow(ICON_MD_PERM_MEDIA " Assets Browser", dock_id_leftDown);

			ImGui::DockBuilderDockWindow(ICON_MD_PERM_MEDIA " Media files", dock_id_bottom);
			ImGui::DockBuilderDockWindow("Message window", dock_id_bottom);
			
			//ImGui::DockBuilderDockWindow("main.agc", dock_main_id);
			//ImGui::DockBuilderDockWindow("ggfunc.agc", dock_main_id);
			//ImGui::DockBuilderDockWindow("ggshadersetup.agc", dock_main_id);


			//ImGui::DockBuilderDockWindow("##DummyRootWindow", dock_main_id);

			dock_main_tabs = dock_main_id;
			/*
			//Moved this to single tab docking instead. keep here if needed.
			firstProjects = allProjects;
			while (firstProjects) {
				cProjectItem::sProjectFiles * m_pCurrentFile;
				m_pCurrentFile = firstProjects->m_pFirstFile;
				while (m_pCurrentFile)
				{
					if (m_pCurrentFile->m_bOpenForEdit) {
						ImGui::DockBuilderDockWindow(m_pCurrentFile->m_sEditName.GetStr(), dock_main_id);
					}
					m_pCurrentFile = m_pCurrentFile->m_pNext;
				}
				firstProjects = firstProjects->m_pNext;
			}
			*/

			ImGui::DockBuilderFinish(dockspace_id);
		}
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		//ImGuiDockNodeFlags_NoSplit , ImGuiDockNodeFlags_PassthruDockspace , ImGuiDockNodeFlags_NoDockingInCentralNode
		//static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		//ImGuiTabBarFlags_NoTabListPopupButton
		//ImGuiDockNodeFlags_AutoHideTabBar , dont work many errors.
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::End();
		if (dock_main_tabs == 0)
			dock_main_tabs = dockspace_id;

//		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
//		ImGui::Begin("##DummyRootWindow", &dockingopen, 0);
//		ImGui::End();


#ifdef TRIALVERSIONEXPIRES

		if (trialaction > 0) {

			if (!agk::GetImageExists(trial_logo)) {
				trial_logo = agk::LoadImage("media/trial.png");
			}

			//Show Trial dialog.
			ImGui::SetNextWindowSize(ImVec2(30 * ImGui::GetFontSize(), 27 * ImGui::GetFontSize()), ImGuiCond_Once);

//			ImGui::SetNextWindowPosCenter(ImGuiCond_Once);

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.40f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGui::Begin(ICON_MD_HELP " Please Read!", &dockingopen, 0);

			ImVec4* style_colors = ImGui::GetStyle().Colors;
			ImVec4 oldTextColor = style_colors[ImGuiCol_Text];
			ImGui::SetWindowFontScale(1.35);
			style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);

			ImVec2 ws = ImGui::GetWindowSize();
			ImGui::Spacing();
			ImGui::Text(WINDOW_TRIALTITLE);

			style_colors[ImGuiCol_Text] = oldTextColor;
			ImGui::SetWindowFontScale(1.0);


			if (trial_logo > 0) {
				//Display image.
				float asx = agk::GetImageWidth(trial_logo);
				float asy = agk::GetImageHeight(trial_logo);
				float icon_ratio;
				int icon_size = 29 * ImGui::GetFontSize();

				if ((icon_size / (float)asx) < (icon_size / (float)asy))
					icon_ratio = icon_size / (float)asx;
				else
					icon_ratio = icon_size / (float)asy;
				float bw = asx*icon_ratio;
				float bh = asy*icon_ratio;

				if (ImGui::ImgBtn(trial_logo, ImVec2(bw, bh), ImColor(235, 235, 235, 235), ImColor(255, 255, 255, 255)))
				{
					//clicked.
					//agk::OpenBrowser("https://www.appgamekit.com/studio");
					agk::OpenBrowser("https://store.steampowered.com/app/1024640/AppGameKit_Studio"); // required by Steam
				}

			}

			//Days left.
			unsigned int timeleft = time(0) - pref.trialdate;

			timeleft /= 86400;
			timeleft = TRIALEXPIREDAYS - timeleft;
			if (timeleft < 0)
				timeleft = 0;
			ImGui::Text("Days left: %d", timeleft);

			ImGui::Separator();

			if (ImGui::Button("Upgrade Now")) {
				//agk::OpenBrowser("https://www.appgamekit.com/studio");
				agk::OpenBrowser("https://store.steampowered.com/app/1024640/AppGameKit_Studio"); // required by Steam
			}

			ImGui::SameLine();

			if( ImGui::Button("Continue With Trial") || !dockingopen ) {

				if (trialaction == 9) {
					//Quit Studio.
					quit_app = 1;
				}
				trialaction = 0;
			}
			ImGui::End();
			dockingopen = true;
		}


		extern bool trialexpired;
		if (trialexpired && agk::Timer() > 5.0 ) {
			//ImGui::SetNextWindowSize(ImVec2(340, 300), ImGuiCond_FirstUseEver);
			//ImGui::SetNextWindowPosCenter(ImGuiCond_Once);

			if (!agk::GetImageExists(trial_logo)) {
				trial_logo = agk::LoadImage("media/trial.png");
			}

			//Show Trial dialog.
			ImGui::SetNextWindowSize(ImVec2(30 * ImGui::GetFontSize(), 27 * ImGui::GetFontSize()), ImGuiCond_Once);

			//			ImGui::SetNextWindowPosCenter(ImGuiCond_Once);

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.40f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowViewport(viewport->ID);


			ImGui::Begin(ICON_MD_HELP " Please Read!", &dockingopen, 0);

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


			if (trial_logo > 0) {
				//Display image.
				float asx = agk::GetImageWidth(trial_logo);
				float asy = agk::GetImageHeight(trial_logo);
				float icon_ratio;
				int icon_size = 29 * ImGui::GetFontSize();

				if ((icon_size / (float)asx) < (icon_size / (float)asy))
					icon_ratio = icon_size / (float)asx;
				else
					icon_ratio = icon_size / (float)asy;
				float bw = asx*icon_ratio;
				float bh = asy*icon_ratio;

				if (ImGui::ImgBtn(trial_logo, ImVec2(bw, bh), ImColor(235, 235, 235, 235), ImColor(255, 255, 255, 255)))
				{
					//clicked.
					//agk::OpenBrowser("https://www.appgamekit.com/studio");
					agk::OpenBrowser("https://store.steampowered.com/app/1024640/AppGameKit_Studio"); // required by Steam
				}

			}

			ImGui::TextWrapped("\nThis trial version has expired! Please visit:\n\nhttps://www.appgamekit.com/\n\n");

			ImGui::End();

			if (trialexpired && agk::Timer() > 16.0) {
				quit_app = 1;
				//agk::OpenBrowser("https://www.appgamekit.com/");
				agk::OpenBrowser("https://store.steampowered.com/app/1024640/AppGameKit_Studio"); // required by Steam
			}
		}

#endif


		//###############################
		//#### Process Scene Manager ####
		//###############################

		if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(200, 200) + viewPortPos, ImGuiSetCond_Once);
		if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(200, 300));
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		//ImGui::Begin(ICON_MD_HELP " Help", &dockingopen, 0);
		if (pref.bDisplaySceneManager && iRenderedScenes > 0 ) {
			if (m_ActiveScene) {
				ImGui::Begin("Scene Manager", &pref.bDisplaySceneManager, 0);
				ProcessSceneManager(m_ActiveScene);
				ImGui::End();
			}

		}


		//######################
		//#### Process Help ####
		//######################

		if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(200, 200) + viewPortPos, ImGuiSetCond_Once);
		if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(200, 300));
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		//ImGui::Begin(ICON_MD_HELP " Help", &dockingopen, 0);
		extern bool bHelpSetup;
		if (pref.bDisplayHelpWindow || !bHelpSetup ) {
			ImGui::Begin(ICON_MD_HELP " Help", &pref.bDisplayHelpWindow, 0);
			processhelp("media/Help/home.html", false);
			ImGui::End();
		}


		//###########################################
		//#### Properties for current selection. ####
		//###########################################

		if (m_ActiveScene && pref.bDisplaySceneProperties  && iRenderedScenes > 0 ) {
			if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(200, 200) + viewPortPos, ImGuiSetCond_Once);
			if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(40, 40));
			ImGui::Begin("Properties", &pref.bDisplaySceneProperties, 0);
			//		//ImGui::Text("Properties");
			ProcessSceneProperties(m_ActiveScene);
			ImGui::End();
		}

		static bool code_properties_first_run = true;
		static float code_properties_open_time = 0.0;
		bool dis_code_prop = true;

		if (pref.bUseCodeProperties && m_ActiveEditor) {
//			if(!m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].empty()) {

				if (!m_ActiveEditor->CodeProperties[m_ActiveEditor->activeCodePropertieslist].empty())
					code_properties_open_time = agk::Timer();

				if (agk::Timer() - code_properties_open_time < 4.0) {
					if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(200, 200) + viewPortPos, ImGuiSetCond_Once);
					if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(40, 40));
					static int ofidc = -1;
					ImGui::Begin("Code Properties", &dockingopen, 0);

					ImGuiWindow* window = ImGui::GetCurrentWindow();
					//window->FocusIdxAllRequestNext
					ProcessCodeProperties();
					if (code_properties_first_run || (!ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ofidc != window->FocusIdxAllCounter)) {
						//ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
						m_ActiveEditor->mScrollToCursor = true;
						if(!code_properties_first_run)
							ofidc = window->FocusIdxAllCounter;
					}
					ImGui::End();
					code_properties_first_run = false;
				}
				else {
					code_properties_first_run = true;
				}
		}
		else {
			code_properties_first_run = true;
		}

		//###########################################################
		//#### Process Preferences Window, Goto Line windows ... ####
		//###########################################################

		if (pref.bAppGameKitNews == true) {
		ProcessNewsWindow();
		}
		//if (bUseDockFamily)
		
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		ProcessPreferences();
	
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		ProcessGotoLine();
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		ProcessInfoBox();
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		ProcessAboutBox();
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		ProcessKeyStore();
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		ProcessAndroidExport();

		ProcessHTML5Export();

		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		ProcessIOSExport();

		//Additional Files Dialog.
		ProcessAdditionalFiles();
		StartInstallProcess(); //Message Install processing.

		//##############################
		//#### Media preview window ####
		//##############################

		if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(250, 250) + viewPortPos, ImGuiSetCond_Once);
		if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(300, 200));
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		if (pref.bDisplayPreviewWindow) {
			ImGui::Begin(ICON_MD_PERM_MEDIA " Preview",&pref.bDisplayPreviewWindow); //, &dockingopen, 0);
			ImVec2 window_pos = ImGui::GetWindowPos();
			//agk::Print(window_pos.x);
			//agk::Print(window_pos.y);

			style_colors = ImGui::GetStyle().Colors;
			//ImGui::GetWindowDrawList()->AddRectFilled(window_pos - ImVec2(2, 2), window_pos + ImGui::GetWindowSize(), ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_MenuBarBg])));
			ImGui::GetWindowDrawList()->AddRectFilled(window_pos - ImVec2(2, 2), window_pos + ImGui::GetWindowSize(), ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_WindowBg])));
			if (m_pSelectedFile)
			{
				ProcessPreviewFile(m_pSelectedFile);
			}
			ImGui::End();
		}

		//#######################################################
		//#### Media files , display files using thumbnails. ####
		//#######################################################

		if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(300, 300) + viewPortPos, ImGuiSetCond_Once);
		if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(300, 200));
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		if (pref.bDisplayMediaFilesWindow) {
			ImGui::Begin(ICON_MD_PERM_MEDIA " Media files", &pref.bDisplayMediaFilesWindow, 0); // window_flags & ImGuiWindowFlags_NoDocking & ImGuiWindowFlags_NoNavFocus); //ImGuiWindowFlags_NoTitleBar

			if (ImGui::BeginTabBar("assetstabbar"))
			{
				if (ImGui::BeginTabItem(ICON_MD_ALL_INCLUSIVE " All Media"))
				{
					if (m_pSelectedFolder) {
						m_pSelectedFolder = ProcessFolderFiles(m_pSelectedFolder, ".fpe,.png,.jpg,.dds,.jpeg,.gif,.tif,.wav,.ogg,.mp3,.obj,.x,.fbx,.3ds,.dae,.b3d,.3d,.lwo,.ago,.ps,.vs,.agc,.agk,.lua,.fx,.folder", true); //".obj,.x,.dbo,.fbx,.3ds,.dae,.b3d,.3d,.lwo,.ago");
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(ICON_MD_LANGUAGE " Models"))
				{
					if (m_pSelectedFolder) {
						m_pSelectedFolder = ProcessFolderFiles(m_pSelectedFolder, ".fpe,.obj,.x,.fbx,.3ds,.dae,.b3d,.3d,.lwo,.ago"); //".obj,.x,.dbo,.fbx,.3ds,.dae,.b3d,.3d,.lwo,.ago");
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(ICON_MD_PHOTO_ALBUM " Textures"))
				{
					if (m_pSelectedFolder) {
						m_pSelectedFolder = ProcessFolderFiles(m_pSelectedFolder, ".png,.jpg,.dds,.jpeg,.gif,.tif"); //.bmp,
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(ICON_MD_MUSIC_NOTE  " Sound"))
				{
					if (m_pSelectedFolder) {
						m_pSelectedFolder = ProcessFolderFiles(m_pSelectedFolder, ".wav,.ogg,.mp3");
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(ICON_MD_CODE  " Shaders"))
				{
					if (m_pSelectedFolder) {
						m_pSelectedFolder = ProcessFolderFiles(m_pSelectedFolder, ".ps,.vs,.fx");
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(ICON_MD_CODE  " Code"))
				{
					if (m_pSelectedFolder) {
						m_pSelectedFolder = ProcessFolderFiles(m_pSelectedFolder, ".agc,.agk,.lua");
					}
					ImGui::EndTabItem();
				}

				//pref.media_icon_size
				ImGui::SameLine();
				ImGui::Separator();
				ImGui::SameLine();
				if (ImGui::Button(ICON_MD_PHOTO_SIZE_SELECT_SMALL)) //  "Small"
					pref.media_icon_size = 32;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Small icons");
				ImGui::SameLine();
				if (ImGui::Button(ICON_MD_PHOTO_SIZE_SELECT_LARGE )) // "Normal"
					pref.media_icon_size = 64;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Medium icons");
				ImGui::SameLine();
				if( ImGui::Button(ICON_MD_PHOTO_SIZE_SELECT_ACTUAL ) ) //"Large"
					pref.media_icon_size = 128;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Large icons");

				ImGui::SameLine();
				if (ImGui::Button(ICON_MD_REFRESH " Refresh" )) //"Large"
					bRefreshNextFolder = 128;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Refresh files");

				ImGui::EndTabBar();
			}
			ImGui::End();
		}
		if (refresh_gui_docking == 3) ImGui::SetWindowFocus(ICON_MD_PERM_MEDIA " Media files");

		//###################################
		//#### Compiler error log window ####
		//###################################

		if (refresh_gui_docking >= 2) { //Make sure media files is already created , so we dock after it.
			//if (bUseDockFamily)
			//	ImGui::SetNextWindowDockFamily(&editorfamily);
			if(pref.bDisplayMessageWindow)
				ShowCompilerLog("Message window", &pref.bDisplayMessageWindow);
		}

		//#################################
		//#### AGK rendertarget window ####
		//#################################
#ifdef RENDERTARGET
		if (agk::GetImageExists(renderTarget)) {

			if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(100, 100)+ viewPortPos, ImGuiSetCond_Once);
			if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(640, 480));
			//if (bUseDockFamily)
			//	ImGui::SetNextWindowDockFamily(&editorfamily);

			ImVec4* colors = ImGui::GetStyle().Colors;
			ImVec4 oldcol = colors[ImGuiCol_WindowBg];
			colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			ImGui::Begin("level1.scene", &dockingopen, 0);
			renderTargetActive = false;

			//Only display if visible.
			if (ImGui::windowTabVisible()) {
				renderTargetActive = true;
				ImGuiWindow* agk_window = ImGui::GetCurrentWindow();
				ImGuiContext& g = *GImGui;
				if (g.NavWindow == agk_window)
					agk_focus = true;
				else
					agk_focus = false;

				//auto *style = &ImGui::GetStyle();
				int agk_target_border = 6; //6
				int titlesize = 23; //24
				render_target_window_size = ImGui::GetWindowSize();
				render_target_window_size.x -= (agk_target_border + 2.0); // 2 style->ItemSpacing.x
				render_target_window_size.y -= (agk_target_border + titlesize); //+4 for myDefaultmystyle (style->ItemSpacing.y)

				render_target_window_pos = ImGui::GetWindowPos();
				render_target_window_pos.y += titlesize + 2.0;

				cImage *pImage = agk::GetImagePtr(renderTarget);
				myTextureID = pImage->GetTextureID() + 100000; // 100000+ = No Color Array , 200000+ = No transparent
				//ImGui::Image((void*) myTextureID, ImVec2(render_target_window_size.x, render_target_window_size.y-titlesize));

				//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.00f)); //Change color of imagebuttom background to nothing.
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
				ImGui::ImageButton((void*)myTextureID, ImVec2(render_target_window_size.x, render_target_window_size.y), ImVec2(0, 0), ImVec2(1, 1), -5, ImVec4(AgkClearColor[0] * toOne, AgkClearColor[1] * toOne, AgkClearColor[2] * toOne, 1), ImVec4(1, 1, 1, 1)); //No window move in game area.
				ImGui::PopStyleVar();
				//ImGui::PopStyleColor(); //Get old colors back.

				//Drop target from icons.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MODEL_DROP_TARGET"))
					{
						if (pDragDropFile) {
							IM_ASSERT(payload->DataSize == sizeof(cFolderItem::sFolderFiles *));
							cFolderItem::sFolderFiles * payload_n = (cFolderItem::sFolderFiles *)payload->Data;
							agk::Print(payload_n->m_sName.GetStr());
							//						if (mode == Mode_Copy)
							//						{
							//							names[n] = names[payload_n];
							//						}
							pDragDropFile = NULL;
						}
					}
					ImGui::EndDragDropTarget();
				}


				ImGui::SetItemAllowOverlap(); //Allow overlapping widgets
				ImGui::SameLine(render_target_window_size.x - 18.0); //18//64.0
				uString fullscreentext;
				if (!agkfullscreen) fullscreentext = ICON_MD_FULLSCREEN;
				if (agkfullscreen) fullscreentext = ICON_MD_FULLSCREEN_EXIT;

				if (ImGui::Button(fullscreentext)) {
					agkfullscreen = 1 - agkfullscreen;
					agk::SetWindowSize(1280, 800, agkfullscreen);
				}

			}
			ImGui::End();
			colors[ImGuiCol_WindowBg] = oldcol;
		}
#endif

		//#####################################################
		//#### Project window , display all open projects. ####
		//#####################################################

		if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(10, 10)+ viewPortPos, ImGuiSetCond_Once); // undock window.
		if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(200, 300));
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);
		
		if (pref.bDisplayProjectWindow) {
			ImGui::Begin("Project",&pref.bDisplayProjectWindow);

			style_colors = ImGui::GetStyle().Colors;
			//ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(ImGui::GetWindowSize().x, 50.0), ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_MenuBarBg])));

			cProjectItem * items[MAXOPNEDPROJECTS];

			firstProjects = allProjects;
			int mc = 0;

			while (firstProjects && mc < MAXOPNEDPROJECTS) {
				items[mc] = firstProjects;
				if (pCurrentSelectedProject == NULL) {
					//Somehow we need to set ActiveEditor.
					processCTRLkey = '\t';
					pCurrentSelectedProject = firstProjects;
				}
				mc++;
				firstProjects = firstProjects->m_pNext;
			}
			items[mc] = NULL;

			if (mc > 0 && pCurrentSelectedProject != NULL)
			{
				ImVec2 cpos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(cpos.x, cpos.y + 3.0));
				ImGui::Text("Current project: ");
				ImGui::SameLine();
				cpos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(cpos.x, cpos.y - 3.0));
				ImGui::PushItemWidth(-1.0);

				if (ImGui::BeginCombo("##Current project: ", pCurrentSelectedProject->m_sProjectFile.GetStr(), 0))
				{
					for (int n = 0; n < mc; n++)
					{
						bool is_selected = (pCurrentSelectedProject == items[n]);
						if (ImGui::Selectable(items[n]->m_sProjectFile.GetStr(), is_selected)) {
							pCurrentSelectedProject = items[n];
							//Find m_pSelectedFolder and set.
							if (pCurrentSelectedProject->m_pMediaFolder) {
								m_pSelectedFolder = pCurrentSelectedProject->m_pMediaFolder;
								m_pSelectedFile = NULL;
							}

							if (pref.bDisplayActiveProjectFilesOnly) {
								//TODO: This will change the last active tab, need another way.
								processCTRLkey = '\t';
							}
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
					}
					ImGui::EndCombo();
				}

				ImGui::Separator();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 4.0));


			}

			ImGui::BeginChild("##curprojectnavigationsplit", ImVec2(0, 0)); //, false, ImGuiWindowFlags_HorizontalScrollbar);


			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

			//########################################
			//#### Display files with no project. ####
			//########################################

			cProjectItem::sProjectFiles * m_pCurrentFile;
			m_pCurrentFile = allFiles;
			while (m_pCurrentFile) {
				if (!m_ActiveEditor && !pCurrentSelectedProject && m_pCurrentFile->m_editor) {
					//No active editor, just set the first.
					m_ActiveEditor = m_pCurrentFile->m_editor;
					cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
					ImGui::SetWindowFocus(m_pCurrentFile->m_sEditName.GetStr());
				}
				int leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				if (m_pCurrentFile->m_editor->filechanged)
					leafFlags |= ImGuiTreeNodeFlags_Bullet;

				if (ImGui::TreeNodeEx((void*)(intptr_t)m_pCurrentFile->m_id, leafFlags, ICON_MD_APPS " %s", m_pCurrentFile->m_sName.GetStr())) { // ICON_MD_FORMAT_COLOR_TEXT // ICON_MD_CODE // ICON_MD_EDIT // ICON_MD_APPS
					 //Open in editor.
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
						if (!m_pCurrentFile->m_bOpenForEdit || (m_pCurrentFile->m_editor && !m_pCurrentFile->m_editor->bEditorVisible)) {
							//Not loaded.
							vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
							m_pCurrentFile->m_bOpenForEdit = true;
							//refresh_gui_docking = 1; //Not needed any more.
							cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
						}
						ImGui::SetWindowFocus(m_pCurrentFile->m_sEditName.GetStr());
					}
					ImGui::TreePop();
				}

				m_pCurrentFile = m_pCurrentFile->m_pNext;
			}

				//IF HERE.
			if (mc > 0 && pCurrentSelectedProject != NULL)
			{

				//#### Display projects with there files ####
				firstProjects = allProjects;
				while (firstProjects) {

					m_pCurrentFile = firstProjects->m_pFirstFile;

					if (firstProjects->m_bDisplayUnfolded || set_default_focus == 10 ) {
						//Only open all nodes first time.
						firstProjects->m_bDisplayUnfolded = false;
						ImGui::SetNextTreeNodeOpen(true);
					}
					if (firstProjects->m_bDisplayCollapsed) {
						ImGui::SetNextTreeNodeOpen(false);
						firstProjects->m_bDisplayCollapsed = false;
					}


					if (pCurrentSelectedProject->m_sProjectFile.GetStr() == firstProjects->m_sProjectFile.GetStr())
						node_flags |= ImGuiTreeNodeFlags_Selected;
					else
						node_flags &= ~ImGuiTreeNodeFlags_Selected;

					bool TreeNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)firstProjects->m_id, node_flags, ICON_MD_FOLDER " %s", firstProjects->m_sProjectFile.GetStr());

					if (ImGui::BeginPopupContextItemAGK()) //"project context menu"
					{
						if (ImGui::MenuItem("Select Current Project")) {
							pCurrentSelectedProject = firstProjects;
							//Find m_pSelectedFolder and set.
							if (pCurrentSelectedProject->m_pMediaFolder) {
								m_pSelectedFolder = pCurrentSelectedProject->m_pMediaFolder;
								m_pSelectedFile = NULL;
							}

							//processCTRLkey = '\t';
							ImGui::EndPopup();
							if (TreeNodeOpen) ImGui::TreePop();
							break; //need redraw.
						}

#ifdef AGK_WINDOWS
						if (ImGui::MenuItem("Open In Explorer", "")) {
							uString sTmp = firstProjects->m_sProjectFileFullPath;
							sTmp.ReplaceStr("/", "\\");
							ITEMIDLIST *pidl = ILCreateFromPathA(sTmp.GetStr());
							if (pidl) {
								SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
								ILFree(pidl);
							}
						}
#else
#ifdef AGK_MACOS_DISABLED
						//Mac cant open,locate folders in finder ?
						//Disable for now, works fine for files.

						if (ImGui::MenuItem("Open In Finder", "")) {
							uString sTmp = firstProjects->m_sProjectFileFullPath;
							sTmp.ReplaceStr("/", "\\");

							//Test add additional slash
							sTmp.Append("/");

							NSString* path = [NSString stringWithUTF8String : sTmp.GetStr()];
							NSArray *fileURLs = [NSArray arrayWithObjects : [NSURL fileURLWithPath : path], nil];
							[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];

						}
#endif
#endif

						if (ImGui::MenuItem("Move Up")) {
							//move project up in list.
							ProjectMoveUp(firstProjects);
						}
						if (ImGui::MenuItem("Move Down")) {
							//move project down in list.
							ProjectMoveDown(firstProjects);
						}

						ImGui::Separator();
						if (ImGui::MenuItem("Close Project")) {
							//We need to switch to this project so it can ask to save files.
							pCurrentSelectedProject = firstProjects;
							//Find m_pSelectedFolder and set.
							if (pCurrentSelectedProject->m_pMediaFolder) {
								m_pSelectedFolder = pCurrentSelectedProject->m_pMediaFolder;
								m_pSelectedFile = NULL;
							}

							if (firstProjects) SaveProjectFile(firstProjects);
							if (firstProjects) RemoveProject(firstProjects);
							setVisibleProjectsInAssetBrowser();
							ImGui::EndPopup();
							if (TreeNodeOpen) ImGui::TreePop();
							break;
						}

						ImGui::Separator();
						if (ImGui::MenuItem("Add New Scene to Project")) {
							pCurrentSelectedProject = firstProjects;
							processCTRLkey = '\t';
							add_new_scene = true;
							ImGui::EndPopup();
							if (TreeNodeOpen) ImGui::TreePop();
							break; //need redraw.

						}
						ImGui::Separator();
						if (ImGui::MenuItem("Add New Source File to Project")) {
							pCurrentSelectedProject = firstProjects;
							processCTRLkey = '\t';
							add_new_project_file = true;
							ImGui::EndPopup();
							if (TreeNodeOpen) ImGui::TreePop();
							break; //need redraw.
						}

						ImGui::Separator();
						if (ImGui::MenuItem("Add File to Project")) {
							uString pfolder = ";:;";
							uString projectpath = firstProjects->m_sProjectFileFullPath;
							projectpath.ReplaceStr("\\", "/");
							int pos = projectpath.RevFindStr("/");
							if (pos > 0) {
								projectpath.SubString(pfolder, 0, pos + 1);
							}

							//Select file.
							char * cFileSelected;
							cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, NULL, NULL, NULL);
							if (cFileSelected && strlen(cFileSelected) > 0) {
								uString fname = cFileSelected;
								fname.ReplaceStr("\\", "/");

								uString relative = fname;
								cProjectItem::sProjectFiles *m_pSearchFile;

								relative.ReplaceStr(pfolder, "");
								//Test directly.
								//uString testdir = pfolder;
								//testdir.Append(relative);

								//if (pfolder.Count('/') <= 0) {
								//	//No forslash must be in project folder.
								//}
								//resolve relative path.
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
									//agk::Print(iRemainingSlash);
								}
								//pfolder  m_pData = 0x14407c68 "D:/github/AGKTier2/AgkIde/Final/testprojects/Terrain/"
								//relative m_pData = 0x14762070 "D:/github/AGKTier2/AgkIde/Final/testprojects/particle.agc" "../particle.agc"
								//relative m_pData = 0x13ae0fc8 "D:/github/AGKTier2/AgkIde/Final/testcode/particle.agc" "../../testcode/particle.agc"
								bool alreadyexist = false;
								m_pSearchFile = firstProjects->m_pFirstFile;
								if (m_pSearchFile) {
									while (m_pSearchFile) {
										if (relative.CompareCaseTo(m_pSearchFile->m_sName) == 0) {
											//File is already in project.
											BoxerInfo("File is already in project.", "Add file to project.");
											alreadyexist = true;
										}
										m_pSearchFile = m_pSearchFile->m_pNext;
									}
								}

								if (!alreadyexist) {
									//Add to project.
									if (firstProjects->m_pFirstFile) {
										m_pSearchFile = firstProjects->m_pFirstFile;
										if (m_pSearchFile->m_pNext) {
											while (m_pSearchFile->m_pNext) {
												m_pSearchFile = m_pSearchFile->m_pNext;
											}
										}
										//Found last. add new.
										if (m_pSearchFile) {
											m_pSearchFile->m_pNext = new cProjectItem::sProjectFiles();
											m_pSearchFile = m_pSearchFile->m_pNext;
											m_pSearchFile->m_sName = relative;
											m_pSearchFile->m_sFullPath = fname;
											m_pSearchFile->m_bUseSaveAs = false;
											m_pSearchFile->m_id = ++uniqID;
											m_pSearchFile->m_editor = new TextEditor();
											if (relative.RevFindStr(".scene", 1) > 0) {
												m_pSearchFile->m_scene = new MediaScene();
												m_pSearchFile->m_scene->m_sSceneFullPath = fname;
											}
											else
												m_pSearchFile->m_scene = NULL;

											m_pSearchFile->m_editor->quiteditor = false;
											m_pSearchFile->m_bOpenForEdit = false;
											m_pSearchFile->m_sProjectName = firstProjects->m_sProjectFile;
											m_pSearchFile->m_sEditName = relative;
											if (pref.bRemovePathFromTabName) {
												uString newTabname = m_pSearchFile->m_sEditName;
												int ntb_pos = newTabname.RevFindStr("/");
												if (ntb_pos <= 0)
													ntb_pos = newTabname.RevFindStr("\\");
												if (ntb_pos > 0)
													newTabname.SubString(m_pSearchFile->m_sEditName, ntb_pos + 1);
											}

											m_pSearchFile->m_sEditName.Append("##");
											m_pSearchFile->m_sEditName.Append(firstProjects->m_sProjectFile);
											m_pSearchFile->m_pNext = NULL;

											//Open file in editor.
											vTextEditorInit(m_pSearchFile->m_editor, (char *)m_pSearchFile->m_sFullPath.GetStr());
											m_pSearchFile->m_bOpenForEdit = true;
											cNextWindowFocus = (char *)m_pSearchFile->m_sEditName.GetStr();
											ImGui::SetWindowFocus(m_pSearchFile->m_sEditName.GetStr());

											//Save new project settings.
											SaveProjectFile(firstProjects);
										}

									}
								}

							}

						}

						ImGui::Separator();
						if (ImGui::MenuItem("Expand")) {
							//Expand all
							firstProjects->m_bDisplayUnfolded = true;
						}
						if (ImGui::MenuItem("Collapse")) {
							//Expand all
							firstProjects->m_bDisplayCollapsed = true;
						}

						ImGui::EndPopup();
					}

					if (TreeNodeOpen)
					{
						if (firstProjects->m_pMediaFolder) { //EXCEPTION m_pMediaFolder not set to ZERO ?. selected close project.
							cFolderItem *oldnext = firstProjects->m_pMediaFolder->m_pNext;
							uString oldname = firstProjects->m_pMediaFolder->m_sFolder;
							//Only display this projects media.
							firstProjects->m_pMediaFolder->m_pNext = NULL;
							firstProjects->m_pMediaFolder->m_sFolder = "media";
							firstProjects->m_pMediaFolder->visible = true;
							GetTreePop(firstProjects->m_pMediaFolder);
							firstProjects->m_pMediaFolder->visible = false;
							firstProjects->m_pMediaFolder->m_sFolder = oldname;
							firstProjects->m_pMediaFolder->m_pNext = oldnext;
						}

						while (m_pCurrentFile) {

							int leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnDoubleClick;
							if (m_pCurrentFile->m_editor->filechanged)
								leafFlags |= ImGuiTreeNodeFlags_Bullet;
							uString szNewName, szParse;
							szParse = m_pCurrentFile->m_sName;
							szNewName = szParse;
							int pos = szParse.RevFindStr("/");
							if (pos > 0) {
								szParse.SubString(szNewName, pos + 1);
							}
							if (ImGui::TreeNodeEx((void*)(intptr_t)m_pCurrentFile->m_id, leafFlags, ICON_MD_APPS " %s", szNewName.GetStr())) { // ICON_MD_FORMAT_COLOR_TEXT // ICON_MD_CODE // ICON_MD_EDIT // ICON_MD_APPS
								//Open in editor.
								if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
									if (m_pCurrentFile->m_bOpenForEdit) {
										cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									}
								}
								if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
									if (!m_pCurrentFile->m_bOpenForEdit || (m_pCurrentFile->m_editor && !m_pCurrentFile->m_editor->bEditorVisible) ) {
										//Not loaded.
										vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
										m_pCurrentFile->m_bOpenForEdit = true;
										//refresh_gui_docking = 1; //Not needed any more.
										cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									}
									if (pref.bDisplayActiveProjectFilesOnly) {
										pCurrentSelectedProject = firstProjects;
										//Find m_pSelectedFolder and set.
										if (pCurrentSelectedProject->m_pMediaFolder) {
											m_pSelectedFolder = pCurrentSelectedProject->m_pMediaFolder;
											m_pSelectedFile = NULL;
										}

										cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									}
									ImGui::SetWindowFocus(m_pCurrentFile->m_sEditName.GetStr());
								}

								if (ImGui::BeginPopupContextItemAGK()) //"project context menu"
								{
									if (ImGui::MenuItem("Open")) {
										if (!m_pCurrentFile->m_bOpenForEdit || (m_pCurrentFile->m_editor && !m_pCurrentFile->m_editor->bEditorVisible) ) {
											//Not loaded.
											vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
											m_pCurrentFile->m_bOpenForEdit = true;
											//refresh_gui_docking = 1; //Not needed any more.
											cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
										}
										ImGui::SetWindowFocus(m_pCurrentFile->m_sEditName.GetStr());
									}
#ifdef AGK_WINDOWS
									if (ImGui::MenuItem("Open In Explorer", "")) {
										uString sTmp = m_pCurrentFile->m_sFullPath;
										sTmp.ReplaceStr("/", "\\");
										char newpath[MAX_PATH];
										cp_realpath((char *)sTmp.GetStr(), newpath, false);
										ITEMIDLIST *pidl = ILCreateFromPathA(newpath);
										if (pidl) {
											SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
											ILFree(pidl);
										}
									}
#else
#ifdef AGK_MACOS
									//Mac cant open,locate folders in finder ?
									//Disable for now, works fine for files.

									if (ImGui::MenuItem("Open In Finder", "")) {
										uString sTmp = m_pCurrentFile->m_sFullPath;
										sTmp.ReplaceStr("/", "\\");

										//Test add additional slash
										sTmp.Append("/");

										NSString* path = [NSString stringWithUTF8String : sTmp.GetStr()];
										NSArray *fileURLs = [NSArray arrayWithObjects : [NSURL fileURLWithPath : path], nil];
										[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];

									}
#endif
#endif

									if (ImGui::MenuItem("Remove from project")) {
										uString fname;
										fname = m_pCurrentFile->m_sName;
										if (fname.CompareCaseTo("main.agc") != 0) {
											uString ask = " Remove file ";
											ask.Append(fname);
											ask.Append(" from project?");

											if (askBox((char *)ask.GetStr(), "Remove from project.")) {
												//Remove it.
												removeFileFromProject(firstProjects, m_pCurrentFile);
												//Save new project settings.
												SaveProjectFile(firstProjects);
												//Must update so.
												ImGui::EndPopup();
												ImGui::TreePop();
												break;
											}

										}
										else {
											BoxerInfo("Projects must have a main.agc.", "Remove from project.");
										}
									}
									ImGui::EndPopup();
								}
								ImGui::TreePop();
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip(szParse.GetStr());


							m_pCurrentFile = m_pCurrentFile->m_pNext;
						}

						ImGui::TreePop();
					}

					firstProjects = firstProjects->m_pNext;
				}

			}


			//ImGuiContext& g = *GImGui;
			//ImGui::ShowDockingDebug();

			//PE: To be used for improving , ctrl+tab display to find the "real" next window.
/*
			ImGuiContext* ctx = GImGui;
			ImGuiContext& g = *ctx;
			ImGuiDockContext* dc = ctx->DockContext;
			if (edit_area_parent) {
				ImGuiID parent_id = edit_area_parent->DockId;
				ImGuiDockNode* node = ImGui::DockContextFindNodeByID(ctx, parent_id);
				if (node && node->TabBar) {
					ImGuiTabBar* tab_bar = node->TabBar;

					for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
					{
						const ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
						ImGui::Text("%02d%c Tab 0x%08X '%s'", tab_n, (tab->ID == tab_bar->SelectedTabId) ? '*' : ' ', tab->ID, tab->Window ? tab->Window->Name : "N/A");
//						ImGui::Text("%s", tab->Window ? tab->Window->Name : "N/A");
					}
				}
			}
*/
/*
			//PE: To be Used to locate save tab order, find "projectname" in tab->Window->Name
			//PE: We must take a backup of the order when loading a project,
			//PE: We must update this list when there are changes (data is not available if project is hidden).

			for (int n = 0; n < dc->Nodes.Data.Size; n++) {
				if (ImGuiDockNode* node = (ImGuiDockNode*)dc->Nodes.Data[n].val_p) {

					if (node->TabBar) {
						ImGuiTabBar* tab_bar = node->TabBar;

						for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
						{
							const ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
							ImGui::Text("%02d%c Tab 0x%08X '%s'", tab_n, (tab->ID == tab_bar->SelectedTabId) ? '*' : ' ', tab->ID, tab->Window ? tab->Window->Name : "N/A");
						}
					}
				}
			}
*/

//			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 20.0));
//			ImGui::TextWrapped("ms/f %.3f (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

//PAUL: rem.
/*
			if (pref.iIDEUpdateEventSleep) {
				extern bool os_messages_support;
				extern unsigned int os_messages;
				extern unsigned int os_messages_old;
				extern unsigned int os_messages_wait;
				extern unsigned int os_messages_sleeptime;
				ide_sleeping = false;
				if (os_messages_support) {
					if (agk::GetPointerState()) {
						os_messages++;
					}
					if (os_messages_wait <= 0) {
						if (os_messages > os_messages_old) {
#ifdef AGK_WINDOWS
							os_messages_wait = os_messages - os_messages_old * 0.5;
#else
							//Mac fire way less events.
							os_messages_wait = os_messages - os_messages_old * 4;
#endif
							os_messages = 0;
							os_messages_old = os_messages;
							if (os_messages_wait > 100)
								os_messages_wait = 100;
							if (os_messages_wait < 0)
								os_messages_wait = 0;
						}
						else {
							//Sleep
							ide_sleeping = true;
#ifdef AGK_WINDOWS
							//ret = MsgWaitForMultipleObjects(1, handles, FALSE, INFINITE, QS_ALLINPUT);
							MsgWaitForMultipleObjects(0, NULL, false, os_messages_sleeptime * 0.5, QS_ALLINPUT);
//							Sleep(os_messages_sleeptime*0.5);
//							Sleep(os_messages_sleeptime*0.75);
							os_messages_sleeptime += 5;
#else
							usleep(os_messages_sleeptime*200);
//							usleep(os_messages_sleeptime * 310);
							os_messages_sleeptime += 3;
#endif
							if (os_messages_sleeptime > 300)
								os_messages_sleeptime = 300;
						}
					}
					else {
						os_messages_sleeptime -= 7;
						if (os_messages_sleeptime < 30)
							os_messages_sleeptime = 30;
						os_messages_wait--;
					}
					if (ide_sleeping)
						ImGui::Text("SLEEP: %ld , %ld", os_messages_sleeptime, os_messages);
					else
						ImGui::Text("messg: %ld , %ld", os_messages_sleeptime, os_messages);

				}
			}
*/
#ifdef ADDIMGUIDEMO
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
#endif
			ImGui::EndChild();
			ImGui::End();
		}


		//######################
		//#### Debug window ####
		//######################

		if (remote_debug_is_running || debug_is_running || !pref.bAutoHideDebugWindows) {
			if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(10, 10) + viewPortPos, ImGuiSetCond_Once); // undock window.
			if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(340, 300));
			//if (bUseDockFamily)
			//	ImGui::SetNextWindowDockFamily(&editorfamily);
			if (pref.bDisplayDebuggerWindow) {
				ImGui::Begin("Debugger",&pref.bDisplayDebuggerWindow);
				if (pCurrentSelectedProject)
					processDebugger();
				ImGui::End();
			}
		}


		//#####################################################################
		//#### Assets browser , display all folders from projects and DLC. ####
		//#####################################################################

		if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(ImVec2(400, 400)+ viewPortPos, ImGuiSetCond_Once);
		if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(300, 200));
		//if (bUseDockFamily)
		//	ImGui::SetNextWindowDockFamily(&editorfamily);

		if (pref.bDisplayAssetsBrowserWindow) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin(ICON_MD_PERM_MEDIA " Assets Browser",&pref.bDisplayAssetsBrowserWindow);
			ImGui::PopStyleVar();

			displayAssetsBrowser("Files", (char *)usAssetsFolder.GetStr());

			ImVec2 oldCursor = ImGui::GetCursorPos();
			ImVec2 ws = ImGui::GetWindowSize() - ImVec2(30, 30);
			ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY()));
			ImGui::Dummy(ws);

			if (ImGui::BeginPopupContextItemAGK("AB Add folder to assets browser"))
			{
				if (ImGui::MenuItem("Add folder location")) {
					char *selectedfolder;
					selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_DIR, NULL, NULL, NULL);
					AddToAssetsBrowser(selectedfolder);
				}
				ImGui::EndPopup();
			}
			ImGui::SetCursorPos(oldCursor);


			ImGui::End();
		}

		//######################################################
		//#### Process all active code editors in projects. ####
		//######################################################

		//PE: first time here we might have to display in correct order before sorting.
		if (pref.iRememberTabOrder)
		{
			static int iReadyToSort = 4;
			if (iReadyToSort > 0) iReadyToSort--;
			if (iReadyToSort == 1)
			{
				//Sort list.

				firstProjects = allProjects;
				while (firstProjects)
				{
					std::vector<std::string> sorted_project_files;
					m_pCurrentFile = firstProjects->m_pFirstFile;
					while (m_pCurrentFile)
					{
						uString newEditName = m_pCurrentFile->m_sEditName;
						//newEditName.Append("##");
						//newEditName.Append(firstProjects->m_sProjectFile);
						sorted_project_files.push_back(newEditName.GetStr());
						m_pCurrentFile = m_pCurrentFile->m_pNext;
					}

					//###################
					//#### Sort now. ####
					//###################

					if (sorted_project_files.size() >= 1) {
						cProjectItem::sProjectFiles  *m_pSearchFile, *m_pSortedFile, *m_ptmpFile;

						m_pSortedFile = NULL;
						m_ptmpFile = firstProjects->m_pFirstFile;
						std::sort(sorted_project_files.begin(), sorted_project_files.end(), NoCaseLess);
						for (std::vector<std::string>::iterator it = sorted_project_files.begin(); it != sorted_project_files.end(); ++it)
						{
							m_pSearchFile = m_ptmpFile;
							while (m_pSearchFile)
							{
								if (it->size() > 0)
								{
									uString newEditName = m_pSearchFile->m_sEditName;
									if (strcmp(it->c_str(), newEditName.GetStr()) == 0) {
										if (m_pSortedFile == NULL) {
											//first entry.
											firstProjects->m_pFirstFile = m_pSearchFile;
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
						m_ptmpFile = firstProjects->m_pFirstFile;
						m_pSearchFile = m_ptmpFile;
						while (m_pSearchFile) {
							m_pSearchFile->m_pSortedNext = m_pSearchFile->m_pNext;
							m_pSearchFile = m_pSearchFile->m_pNext;
						}

					}
					sorted_project_files.clear();

					//#######################
					//#### Sort now END. ####
					//#######################

					firstProjects = firstProjects->m_pNext;

				}
			}
		}
		bool gotDialog = false;
		modal_window_active = false;
		first_editor_processed = false;
		iRenderedScenes = 0;
		//Mark if modal window is active in any of the code editors.
		cProjectItem::sProjectFiles * m_pCurrentFile;
		m_pCurrentFile = allFiles;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_bOpenForEdit) {
				if (m_pCurrentFile->m_editor->quiteditor)
					modal_window_active = true;
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
		firstProjects = allProjects;
		while (firstProjects)
		{
			m_pCurrentFile = firstProjects->m_pFirstFile;
			while (m_pCurrentFile)
			{
				if (m_pCurrentFile->m_bOpenForEdit && m_pCurrentFile->m_editor && m_pCurrentFile->m_editor->bEditorVisible ) {
					if (m_pCurrentFile->m_editor->quiteditor)
						modal_window_active = true;
				}
				m_pCurrentFile = m_pCurrentFile->m_pNext;
			}
			firstProjects = firstProjects->m_pNext;
		}

		int iCountUpdates = 0;
		bool bUpdatedOneEditor = false;
		//iUpdateIndex

		//Process files with no project.
		m_pCurrentFile = allFiles;
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_bOpenForEdit) {

				if (pref.bEnableSymboldList) {
					bCollectSuggestKeywords = false;
					if (iUpdateIndex == iCountUpdates++) {
						bCollectSuggestKeywords = true;
						bUpdatedOneEditor = true;
					}
				}

				//if (bUseDockFamily)
				//	ImGui::SetNextWindowDockFamily(&editorfamily);
				if( vTextEditor((char *)m_pCurrentFile->m_sEditName.GetStr(), m_pCurrentFile->m_editor, (char*)m_pCurrentFile->m_sName.GetStr(), (char*)m_pCurrentFile->m_sFullPath.GetStr(), m_pCurrentFile->m_bUseSaveAs) ) {
					//Quit this editor.
					m_pCurrentFile->m_bOpenForEdit = false;
					gotDialog = true;
					processCTRLkey = '\t';
					//Remove from list if this is a file with no project.
					if (m_pCurrentFile->m_sProjectName.CompareTo("na-noproject") == 0) {
						
						//Change pos of window in focus list. for CTRL+TAB to ignore.
						ImGuiContext& g = *GImGui;
						ImGuiWindow* current = NULL;
						for (int n = 0; n < g.WindowsFocusOrder.Size - 1; n++)
						{
							ImGuiWindow* window = g.WindowsFocusOrder[n];
							if (window->Name && strstr(window->Name, "/TextEditor")) {
								if (strncmp(m_pCurrentFile->m_sEditName.GetStr(), window->Name, m_pCurrentFile->m_sEditName.GetLength()) == 0) {
									current = window;
									current->Pos.x = -9999;
								}
							}
						}

						RemoveAGKFile(m_pCurrentFile);
						break;
					}
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}

		//Process project files , code editors.
		cProjectItem * firstProjectsSearch = allProjects;
		
		static int renderallwindows = 70;//On first start we need to dock all editor windows so.
		static cProjectItem * pLastActiveSelectedProject = NULL;
		static bool bRedocWindows = false;
		if (pLastActiveSelectedProject != pCurrentSelectedProject) {
			pLastActiveSelectedProject = pCurrentSelectedProject;
			bRedocWindows = true;
		}
		
		while (firstProjectsSearch)
		{
			renderingProject = firstProjectsSearch;
			m_pCurrentFile = firstProjectsSearch->m_pFirstFile;

			while(m_pCurrentFile)
			{
				bool bThisEditorActive = true;
				if (renderallwindows == 0) {
					if (pref.bDisplayActiveProjectFilesOnly && pCurrentSelectedProject) {
						if (firstProjectsSearch != pCurrentSelectedProject) {
							bThisEditorActive = false;
						}
					}
				}
				else {
					renderallwindows--;
				}

				if(bRedocWindows)
					m_pCurrentFile->m_editor->firstrun = true;

				if (m_pCurrentFile->m_editor && m_pCurrentFile->m_editor->quiteditor)
					bThisEditorActive = true;

				if (bThisEditorActive ) { //&& m_pCurrentFile->m_bOpenForEdit


					//Open invisible if not open.
					if (!m_pCurrentFile->m_bOpenForEdit && m_pCurrentFile->m_editor ) {

						vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
						m_pCurrentFile->m_bOpenForEdit = true;
						m_pCurrentFile->m_editor->bEditorVisible = false;
						//Change pos of window in focus list. for CTRL+TAB to ignore.
						ImGuiContext& g = *GImGui;
						ImGuiWindow* current = NULL;
						for (int n = 0; n < g.WindowsFocusOrder.Size - 1; n++)
						{
							ImGuiWindow* window = g.WindowsFocusOrder[n];
							if (window->Name && strstr(window->Name, "/TextEditor")) {
								if (strncmp(m_pCurrentFile->m_sEditName.GetStr(), window->Name, m_pCurrentFile->m_sEditName.GetLength()) == 0) {
									current = window;
									current->Pos.x = -9999;
									//Reset cursor location.
									if (m_pCurrentFile->m_editor) {
										m_pCurrentFile->m_editor->SetCursorPosition(TextEditor::Coordinates(0, 0));
									}

								}
							}
						}

					}

					if (pref.bEnableSymboldList) {
						bCollectSuggestKeywords = false;
						if (iUpdateIndex == iCountUpdates++) {
							bCollectSuggestKeywords = true;
							bUpdatedOneEditor = true;
						}
					}

					//if (bUseDockFamily)
					//	ImGui::SetNextWindowDockFamily(&editorfamily);
					//if(bDisplayAsText)

					if (vTextEditor((char *)m_pCurrentFile->m_sEditName.GetStr(), m_pCurrentFile->m_editor, (char*)m_pCurrentFile->m_sName.GetStr(), (char*)m_pCurrentFile->m_sFullPath.GetStr(), m_pCurrentFile->m_bUseSaveAs, m_pCurrentFile))
					{
						//Quit this editor.

						if(m_pCurrentFile)
							m_pCurrentFile->m_bOpenForEdit = false;

						bool waslastactive = false;
						if (m_ActiveEditor == m_pCurrentFile->m_editor) {
							m_ActiveEditor = NULL;
							waslastactive = true;
						}

						if (waslastactive) {
							processCTRLkey = '\t';
							//Change pos of window in focus list. for CTRL+TAB to ignore.
							ImGuiContext& g = *GImGui;
							ImGuiWindow* current = NULL, *prev = NULL;
							for (int n = 0; n < g.WindowsFocusOrder.Size - 1; n++)
							{
								ImGuiWindow* window = g.WindowsFocusOrder[n];
								if (window->Name && strstr(window->Name, "/TextEditor"))
								{

									if (strncmp(m_pCurrentFile->m_sEditName.GetStr(), window->Name, m_pCurrentFile->m_sEditName.GetLength()) == 0) {
										current = window;
										current->Pos.x = -9999;
									}
									if (!current && window->Pos.x != -9999.0) {
										prev = window;
									}
								}
							}
							if (prev) {
								//Set window focus.
								//Select new window.
								uString szTmp = prev->Name;
								int pos = szTmp.RevFindStr("/");
								if (pos > 0)
								{
									szTmp.SubString(szNextWindowFocus, 0, pos);
									cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
									ImGui::SetWindowFocus(cNextWindowFocus);

									//find m_ActiveEditor from prev->Name
									m_ActiveEditor = FindEditorFromWindowName((char *) szNextWindowFocus.GetStr());

									//Cancel tab.
									processCTRLkey = 0;
								}

							}
						}
						gotDialog = true;
					}
				}
				if(m_pCurrentFile)
					m_pCurrentFile = m_pCurrentFile->m_pNext;
			}
			if(firstProjectsSearch)
				firstProjectsSearch = firstProjectsSearch->m_pNext;
		}
		bRedocWindows = false;

		if (pref.bEnableSymboldList) {
			if (bUpdatedOneEditor == false) {
				// skip 120 frames before we update again. around every 2 sec.
				if (iDelayedUpdateStart++ > 120 || symbolsCollectList[iCollectListActive].size() == 0) {
					//Nothing updated in this run. reset everything.
					if (iCollectListUpdate == 0) {
						iCollectListUpdate = 1;
						iCollectListActive = 0;
					}
					else {
						iCollectListUpdate = 0;
						iCollectListActive = 1;
					}
					symbolsCollectList[iCollectListUpdate].clear();
					iUpdateIndex = 0;
					iDelayedUpdateStart = 0;
				}
			}
			else {
				//Update the next open editor.
				iUpdateIndex++;
			}
		}


		//##########################################################
		//#### Record a navigation history of cursor positions. ####
		//##########################################################

		//Resets navigation history when project change.
		//TODO: ONLY record on goto definition etc... like geany but very limited ?

		//Timer based recording, cursor must stay in same place for 0.6 sec before record.
		if (m_ActiveEditor) {
			if (bDelayRecordActive) {
				if (agk::Timer() - fDelayRecordTimer > 0.6) {
					//Still the same cords ?
					if (DelayRecord.m_RecordEditor == m_ActiveEditor && DelayRecord.lineno == m_ActiveEditor->mState.mCursorPosition.mLine) {

						iRecordIndex++;
						
						if (posRecord.size() > 3000 || pDelayRecordProject != pCurrentSelectedProject) {
							//Reset.
							iRecordIndex = 1;
							posRecord.clear();
							pDelayRecordProject = pCurrentSelectedProject;
						}
						//If already exists, first delete all => iRecordIndex; (new route)
						auto it = posRecord.find(iRecordIndex);
						if (it != posRecord.end())
						{
							int deletestart = iRecordIndex;
							while (it != posRecord.end()) {
								posRecord.erase(deletestart++);
								it = posRecord.find(deletestart);
							}
						}

						posRecord.insert(std::make_pair(std::int32_t(iRecordIndex), DelayRecord));
						bDelayRecordActive = false;
					}

				}
			}
		}
		if (m_ActiveEditor) {
			if (m_LastRecordEditor != m_ActiveEditor) {
				m_LastRecordEditor = m_ActiveEditor;
				
				if (m_LastRecordEditor == m_IgnoreLastRecordEditor && iLastRecordLine == iIgnoreLastRecordLine && iLastRecordCol == iIgnoreLastRecordCol) {
					//Must ignore.
					iIgnoreLastRecordLine = -1;
				}
				else {

					//New active editor. make new entry.
					//Include Editor and line and col.

					bool bIgnore = false;
					auto oit = posRecord.find(iRecordIndex);
					if (oit != posRecord.end()) {
						if (oit->second.lineno == m_ActiveEditor->mState.mCursorPosition.mLine) {
							if (oit->second.colno == m_ActiveEditor->mState.mCursorPosition.mColumn) {
								if (oit->second.m_RecordEditor == m_ActiveEditor) {
									//Same ignore.
									bIgnore = true;
								}
							}
						}
					}

					if (!bIgnore) {

						RecordValues stmp;
						stmp.lineno = m_ActiveEditor->mState.mCursorPosition.mLine;
						stmp.colno = m_ActiveEditor->mState.mCursorPosition.mColumn;
						stmp.m_RecordEditor = m_ActiveEditor;
						iRecordIndex++;

						if (posRecord.size() > 3000 || pDelayRecordProject != pCurrentSelectedProject) {
							//Reset.
							iRecordIndex = 1;
							posRecord.clear();
							pDelayRecordProject = pCurrentSelectedProject;
						}

						//If already exists, first delete all => iRecordIndex; (new route)
						auto it = posRecord.find(iRecordIndex);
						if (it != posRecord.end())
						{
							int deletestart = iRecordIndex;
							while (it != posRecord.end()) {
								posRecord.erase(deletestart++);
								it = posRecord.find(deletestart);
							}
						}

						posRecord.insert(std::make_pair(std::int32_t(iRecordIndex), stmp));
						bDelayRecordActive = false;
					}
				}
				iLastRecordLine = m_ActiveEditor->mState.mCursorPosition.mLine;
				iLastRecordCol = m_ActiveEditor->mState.mCursorPosition.mColumn;
			}
			else {


				//Same editor , check for changes.
				if (m_LastRecordEditor == m_IgnoreLastRecordEditor && iLastRecordLine == iIgnoreLastRecordLine && iLastRecordCol == iIgnoreLastRecordCol) {
					//Must ignore.
					iIgnoreLastRecordLine = -1;
				}
				else {

//					if (iLastRecordLine != m_ActiveEditor->mState.mCursorPosition.mLine || iLastRecordCol != m_ActiveEditor->mState.mCursorPosition.mColumn) {
					//Only when line change.
					if (iLastRecordLine != m_ActiveEditor->mState.mCursorPosition.mLine) {

						//Got a change record it.
						bool bIgnore = false;
						auto oit = posRecord.find(iRecordIndex);
						if (oit != posRecord.end()) {
							if (oit->second.lineno == m_ActiveEditor->mState.mCursorPosition.mLine) {
								if (oit->second.colno == m_ActiveEditor->mState.mCursorPosition.mColumn) {
									//Same ignore.
									bIgnore = true;
								}
							}
						}
						if (!bIgnore) {

							fDelayRecordTimer = agk::Timer();
							bDelayRecordActive = true;
							DelayRecord.lineno = m_ActiveEditor->mState.mCursorPosition.mLine;
							DelayRecord.colno = m_ActiveEditor->mState.mCursorPosition.mColumn;
							DelayRecord.m_RecordEditor = m_ActiveEditor;


/* Moved to delayed recording.
							RecordValues stmp;
							stmp.lineno = m_ActiveEditor->mState.mCursorPosition.mLine;
							stmp.colno = m_ActiveEditor->mState.mCursorPosition.mColumn;
							stmp.m_RecordEditor = m_ActiveEditor;
							iRecordIndex++;

							//If already exists, first delete all => iRecordIndex; (new route)
							auto it = posRecord.find(iRecordIndex);
							if (it != posRecord.end())
							{
								int deletestart = iRecordIndex;
								while (it != posRecord.end()) {
									posRecord.erase(deletestart++);
									it = posRecord.find(deletestart);
								}
							}

							posRecord.insert(std::make_pair(std::int32_t(iRecordIndex), stmp));
*/
						}
						iLastRecordLine = m_ActiveEditor->mState.mCursorPosition.mLine;
						iLastRecordCol = m_ActiveEditor->mState.mCursorPosition.mColumn;

					}
				}
			}
		}
		//############################
		//#### Delayed functions. ####
		//############################

		if (delaydialog > 0) delaydialog--;

		if (gotDialog) {

			if (pRepeatRemoveProject != NULL) {
				if (bRepeatRemoveProjectAll) {
					bRepeatRemoveProjectAll = false;
					pRepeatRemoveProject = NULL;
					RemoveProject(NULL);
				}
				else {
					//Bug: This is a single project so dont use NULL.
					//pRepeatRemoveProject = NULL;
					RemoveProject(pRepeatRemoveProject);
				}
				
			}
		}

		if (cNextWindowFocus != NULL && refresh_gui_docking == 4 ) {
			if (cNextWindowFocusCount == 3) { //Wait 3 frames so we are sure its open/created.

				ImGui::SetWindowFocus(cNextWindowFocus);
				if(!bFocusActiveEditor) {
					cNextWindowFocusCount = 0;
					cNextWindowFocus = NULL;
				}

			}
			else if (cNextWindowFocusCount >= 8) {
				//We need some frames before we can activate cursor.
				if (bFocusActiveEditor && m_ActiveEditor) {
					m_ActiveEditor->mScrollToCursor = true;
				}
				cNextWindowFocus = NULL;
				cNextWindowFocusCount = 0;
				bFocusActiveEditor = false;
			}
			cNextWindowFocusCount++;
			if (cNextWindowFocusCount > 10)
				cNextWindowFocusCount = 10;
		}

		if(delaydialog == 0 && iWordCount > 0)
		{
			char tmp[MAX_PATH];
//			sprintf(tmp, "Word Count: %d", iWordCount);
//			BoxerInfo(tmp, "Word Count.");
			extern char szInfoBoxTitle[MAX_PATH];
			extern char szInfoBoxMsg[MAX_PATH];
			extern bool show_infobox_window;
			strcpy(szInfoBoxTitle, "Word Count.");
			sprintf(szInfoBoxMsg, "Word Count: %d", iWordCount);
			show_infobox_window = true;
			iWordCount = 0;
		}

		if (refresh_gui_docking < 4) {
			refresh_gui_docking++;
		}
		else {
			if (initial_setup) {
				//Load agkide.ini for any initial settings.
				PleaseWait();
				LoadAgkIdeIni(2);
				PleaseWaitEnd();
				initial_setup = false;
				bAllProjectsHasBeenProcessed = true;
			}
		}
	}

	//Other dialogs.
	CreateNewProject();
	CreateNewScene();
	CreateNewProjectFile();
	//ICON_MD_HELP " Help"

	
	if (set_default_focus == 5)
		ImGui::SetWindowFocus(ICON_MD_PERM_MEDIA " Media files");
	if (set_default_focus == 6)
		ImGui::SetWindowFocus(ICON_MD_HELP " Help");
	if (set_default_focus == 7)
		ImGui::SetWindowFocus("Project");

	set_default_focus++;

	if (agkfullscreen) {
		if (bAgkFullscreenTrigger && agk::Timer() - agkFullscreenTimer < 4.0) {

			ImVec2 wpos = ImVec2((agk::GetDeviceWidth() - 460) * 0.5, 50) + ImGui::GetMainViewport()->Pos;
			ImGui::SetNextWindowPos(wpos, ImGuiSetCond_Once);
			bool winopen = true;

			ImVec4* style_colors = ImGui::GetStyle().Colors;
			ImVec4 oldBgColor = style_colors[ImGuiCol_WindowBg];
			ImVec4 oldTextColor = style_colors[ImGuiCol_Text];

			float fader = (agk::Timer() - agkFullscreenTimer);
			fader -= 1.0;
			if (fader < 0) {
				fader = 0.0001;
			}
			fader /= 3.0;

			fader = 1.0 - fader;
			if (fader < 0.1) {
				bAgkFullscreenTrigger = false;
			}
			style_colors[ImGuiCol_WindowBg].x = 0.0;
			style_colors[ImGuiCol_WindowBg].y = 0.0;
			style_colors[ImGuiCol_WindowBg].z = 0.0;
			style_colors[ImGuiCol_WindowBg].w *= fader;

			style_colors[ImGuiCol_Text].x = 1.0;
			style_colors[ImGuiCol_Text].y = 1.0;
			style_colors[ImGuiCol_Text].z = 1.0;
			style_colors[ImGuiCol_Text].w *= fader;

			ImGui::Begin("##f11info", &winopen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);
			ImGui::SetWindowFontScale(2.0);
			ImGui::Text(" ");
			ImGui::Text("       Press F11 to exit fullscreen       ");
			ImGui::Text(" ");
			ImGui::SetWindowFontScale(1.0);
			ImGui::End();
			style_colors[ImGuiCol_WindowBg] = oldBgColor;
			style_colors[ImGuiCol_Text] = oldTextColor;
			ide_force_rendering_delayed = true;
		
		}
	}
	

#ifdef ADDIMGUIDEMO
	if( show_demo_window )
		ImGui::ShowDemoWindow();
#endif


	if (iDeviceHeight != agk::GetDeviceHeight() || iDeviceWidth != agk::GetDeviceWidth()) {
		iDeviceWidth = agk::GetDeviceWidth();
		iDeviceHeight = agk::GetDeviceHeight();
		agk::SetVirtualResolution(iDeviceWidth, iDeviceHeight);

		if (agk::GetImageExists(renderTarget) == 1)
			agk::DeleteImage(renderTarget);
		renderTarget = agk::CreateRenderImage(iDeviceWidth, iDeviceHeight, 0, 0);

		if (agk::GetImageExists(iPreviewRenderTarget) == 1)
			agk::DeleteImage(iPreviewRenderTarget);
		iPreviewRenderTarget = agk::CreateRenderImage(iDeviceWidth, iDeviceHeight, 0, 0);

	}

#ifdef RENDERTARGET
	//agk::PrintC("FPS: "); agk::Print(agk::ScreenFPS());
	static float rendertargettime = agk::Timer();
	static float fUpdateRenderTarget = 1.0/31.0; // 30 fps.
	static float myfFps;
	static int skippedframes;

	float mDelta = agk::Timer() - rendertargettime;

	if(1==1 || mDelta > fUpdateRenderTarget) { //Update at 30 fps.
		float fFps = 1.0f / mDelta;
		myfFps = 0.95f*myfFps + 0.05f*fFps;
		agk::Print(""); // Needed ?
		rendertargettime = agk::Timer();

		//Only update ALL agk stuff here.

		if (renderTargetActive) {
			agk::SetRenderToImage(renderTarget, -1);
			agk::ClearScreen();
			agk::SetClearColor(AgkClearColor[0], AgkClearColor[1], AgkClearColor[2]);
			agk::DrawLine(0, 0, iDeviceWidth, iDeviceHeight, 255, 255, 0);
			agk::DrawLine(iDeviceWidth, 0, 0, iDeviceHeight, 255, 0, 0);

			agk::Update(); // 1.0/30.0
			agk::Render2DBack();
			agk::ClearDepthBuffer();
			agk::Render3D();
			agk::ClearDepthBuffer();
			agk::Render2DFront();

			agk::SetRenderToScreen();
		}
		else {
			//Only update.
			agk::Update(); // 1.0/30.0
		}
		skippedframes = 0;
	}
	else {
		skippedframes++;
	}


	AGKShader *pShader = AGKShader::g_pShaderTexColor;
	if (pShader)
		pShader->NoShader();

	ImGui::Render();
	ImGui_ImplAGK_RenderDrawData(ImGui::GetDrawData());
	agk::Swap();

#else

	//PAUL: may be an imGUI way of doing this, I only know the AGK way
	int anyKey = 0;
	int hangingkey = 0;
	if (pref.iIDEUpdateEventSleep) {
		for (int i = 0; i < 512; i++)
		{
			if (agk::GetRawKeyState(i))
			{
				hangingkey = i;
				anyKey = 1;
			}
			if (agk::GetRawKeyPressed(i) || agk::GetRawKeyReleased(i))
			{
				hangingkey = i;
				anyKey = 2;
				break;
			}
		}

		// refresh at 15fps when moving mouse or key held
		static int prevMouseX = -1;
		static int prevMouseY = -1;
		// without (int) we get rounding errors that can trigger this always.
		if ( (int) agk::GetPointerX() != prevMouseX || (int) agk::GetPointerY() != prevMouseY || anyKey == 1)
		{
			prevMouseX = agk::GetPointerX();
			prevMouseY = agk::GetPointerY();
			//if (eventTimes.fLastRenderTime + 0.065f <= eventTimes.fCurrTime) ide_force_rendering = true; // check if 65ms since last frame, 15fps
			ide_force_rendering = true; //PE: Always update when changed. or we get a "no reaction" feeling. Still max 30 as we have Sleep(33);
			ide_force_rendering_delayed = true; //Update 2 frames, so action for keys also get rendered.

		}

		// immediate refresh when mouse or key pressed
		// GetPointerState for drag drop op.
		ImGuiIO& io = ImGui::GetIO();
		if (io.MouseWheel != 0.0f || agk::GetRawMouseRightState() || agk::GetPointerState() || agk::GetPointerPressed() || agk::GetPointerReleased() || anyKey == 2)
		{
			ide_force_rendering = true;
			ide_force_rendering_delayed = true; //Update 2 frames, so action for mouseclick also get rendered.
			ide_force_rendering_times = 5; //PE: 5 updates to prevent delays on event acting on "release", could take some frames.
		}

	}
	else
		ide_force_rendering = true;

	//Always render for fist 3 sec. so it dont look like windows is hanging before docking.
	if( agk::Timer() < 3.0)
		ide_force_rendering = true;

	//PE: https://github.com/TheGameCreators/AGK-Studio/issues/413#issuecomment-523488699 test
//	try
//	{

		if (ide_force_rendering || eventTimes.fLastRenderTime + 0.5f <= eventTimes.fCurrTime) { // will always refresh at least every 500ms
			eventTimes.fLastRenderTime = eventTimes.fCurrTime;
			ide_force_rendering = false;


			//os_messages

			//PE: No rendertarget directly.

			//Update Animations ...
			if (!scene_editor_testmode)
				agk::Update();

			/* PAUL: rem
					// I don't think AGK needs to update and draw here, could save some time
					agk::ClearScreen();
					agk::Print("@@@@#### 1111 $$$$#####");

					agk::Update();

					agk::Render();
			*/
			AGKShader::NoShader(); // PAUL: static function

	//		AGKShader *pShader = AGKShader::g_pShaderTexColor;
	//		if (pShader)
	//			pShader->NoShader();

			//PAUL: AGK needs to be told that the currently bound resources may have changed, new function added to the Tier2 lib

			//Need to add this for MAC.
#ifdef AGK_WINDOWS
			agk::ResetRenderState();
#endif

			ImGui::Render();

			ImGui_ImplAGK_RenderDrawData(ImGui::GetDrawData());


			// Update and Render additional Platform Windows
		//	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//	{
		//		ImGui::UpdatePlatformWindows();
		//		ImGui::RenderPlatformWindowsDefault();
		//	}

	//		if (pShader)
	//			pShader->MakeActive();

#ifdef STARTUPPROBLEM

		//@Paul: Do this go directly to the SVN when i only commit (and not merge)
		//Or do you have to merge/confirm it ?

			MessageBoxA(NULL, "GUI 36", "Running", 0);
			//PE: A User got a crash in first call to agk::Swap() on startup why ? missing drivers ?
			// https://github.com/TheGameCreators/AGK-Studio/issues/413#issuecomment-521330798
#endif
			//PE: https://github.com/TheGameCreators/AGK-Studio/issues/413#issuecomment-523488699 test
			/*
			try
			{
				//PE: Try full sync for 5 sec the app is running.
				if (agk::Timer() < 5.0)
					agk::Sync();
				else
					agk::Swap();
			}
			catch (...)
			{
				uString err = agk::GetLastError();
				err.Prepend("Swap: Uncaught exception: \n\n");
				MessageBoxA(NULL, err.GetStr(), "Error", 0);
				quit_app = 1;
			}
			*/
			agk::Swap();
		}
		else {
			ImGui::Render();
			agk::UpdateInput(); //PAUL: if swap isn't called then this must be called
		}

		//PE: https://github.com/TheGameCreators/AGK-Studio/issues/413#issuecomment-523488699 test
//	}
//	catch (...)
//	{
//		uString err = agk::GetLastError();
//		err.Prepend("GUI37 Uncaught exception: \n\n");
//		MessageBoxA(NULL, err.GetStr(), "Error", 0);
//		quit_app = 1;
//	}



	if (ide_force_rendering_delayed) {
		ide_force_rendering = ide_force_rendering_delayed;
		ide_force_rendering_delayed = false;
	}
	if (ide_force_rendering_times > 0) {
		ide_force_rendering_times--;
		ide_force_rendering = true;
	}

#endif
#ifdef RENDERTARGET
	//render_target_window_pos
	float mouseX = agk::GetPointerX();
	float mouseY = agk::GetPointerY();
	ImGuiContext& g = *GImGui;
	if (agk_focus && !g.DragDropActive && mouseX > render_target_window_pos.x && mouseX < render_target_window_pos.x + render_target_window_size.x && mouseY > render_target_window_pos.y && mouseY < render_target_window_pos.y + render_target_window_size.y) {
		MoveCam();
	} else {
		MousePressedOutSideAGK();
	}
#endif


#ifdef AGK_WINDOWS
	//PE: https://github.com/TheGameCreators/AGK-Studio/issues/413#issuecomment-523488699 test

//	try
//	{

		if (debug_is_running && pref.bDebugBringAppToFront) {
			HWND tmpHwnd = GetFocus();
			extern HWND debug_hwnd;
			if (debug_hwnd) {
				if (debug_hwnd != tmpHwnd) {
					static float debughwnd_Timer;
					if (agk::Timer() - debughwnd_Timer > 1.0) {
						debughwnd_Timer = agk::Timer();
						HWND hCurWnd = ::GetForegroundWindow();
						DWORD dwExStyle = ::GetWindowLong(debug_hwnd, GWL_EXSTYLE);

						if ((dwExStyle & WS_EX_TOPMOST) == 0)
						{
							// We loose keyboard focus when doing this, so only if not set.
							//It will change if app resize or set new windows settings.
							::SetWindowPos(debug_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
						}
					}
				}
			}
		}
		//PE: https://github.com/TheGameCreators/AGK-Studio/issues/413#issuecomment-523488699 test
//	}
//	catch (...)
//	{
//		uString err = agk::GetLastError();
//		err.Prepend("GUI38 Uncaught exception: \n\n");
//		MessageBoxA(NULL, err.GetStr(), "Error", 0);
//		quit_app = 1;
//	}

#endif

	//PAUL: since sync rate is no longer being used we sleep explicitly
	if (pref.iIDEUpdateEventSleep) {
		if (!scene_editor_testmode) {
#ifdef AGK_LINUX
			//PE: Slow on some linux system , so wake up when we get a event.
			//PE: Not very precise. this seam to be ok.
			glfwWaitEventsTimeout(0.25);
#else
			agk::Sleep(33); // 30 fps.
#endif
		}
	}

	return quit_app; // return 1 to close app
}



float startx, starty, angx, angy;
int pressed;
void MousePressedOutSideAGK(void)
{
	startx = agk::GetPointerX();
	starty = agk::GetPointerY();
	angx = agk::GetCameraAngleX(1);
	angy = agk::GetCameraAngleY(1);
}

void MoveCam(void)
{
	float  oldx1, oldy1, oldz1, newx, newy, newz;
	oldx1 = agk::GetCameraX(1);
	oldy1 = agk::GetCameraY(1);
	oldz1 = agk::GetCameraZ(1);

	// move the camera
	float FPS = 1.0 / agk::GetFrameTime();

	float speed = 60.0 / FPS;

	if (agk::GetRawKeyState(16)) {
		speed = ( 120.0) / FPS;
	}

	if (agk::GetRawKeyState(87))
	{
		agk::MoveCameraLocalZ(1, speed);
	}

	if (agk::GetRawKeyState(83)) {
		agk::MoveCameraLocalZ(1, -speed);
	}

	if (agk::GetRawKeyState(65)) {
		agk::MoveCameraLocalX(1, -speed);
	}

	if (agk::GetRawKeyState(68)) {
		agk::MoveCameraLocalX(1, speed);
	}
	if (agk::GetRawKeyState(81)) {
		agk::MoveCameraLocalY(1, -speed);
	}

	if (agk::GetRawKeyState(69)) {
		agk::MoveCameraLocalY(1, speed);
	}

	// rotate the camera
	if (agk::GetPointerPressed()) {
		startx = agk::GetPointerX();
		starty = agk::GetPointerY();
		angx = agk::GetCameraAngleX(1);
		angy = agk::GetCameraAngleY(1);
		pressed = 1;
	}


	// rotate camera when mouse is pressed.
	if (agk::GetPointerState() == 1) {
		float fDiffX = (agk::GetPointerX() - startx) / 8.0;
		float fDiffY = (agk::GetPointerY() - starty) / 8.0;
		float newX = angx + fDiffY;
		if (newX > 89) {
			newX = 89;
		}
		if (newX < -89) {
			newX = -89;
		}
		agk::SetCameraRotation(1, newX, angy + fDiffX, 0);
	}

	newx = agk::GetCameraX(1);
	newy = agk::GetCameraY(1);
	newz = agk::GetCameraZ(1);

	if( agk::ObjectSphereSlide(0, oldx1, oldy1, oldz1, newx, newy, newz, 10.0) > 0 )
	{
		//PE: Test collision.
		newx = agk::GetObjectRayCastSlideX(0);
		newy = agk::GetObjectRayCastSlideY(0);
		newz = agk::GetObjectRayCastSlideZ(0);
		agk::SetCameraPosition(1, newx, newy, newz);
	}

}


void app::End (void)
{
	//Save project setting , and files.
	if (pref.save_layout) {
		char cmLayoutFile[MAX_PATH];
		sprintf(cmLayoutFile, "%sagkide.layout", defaultWriteFolder);
		ImGui::SaveIniSettingsToDisk(cmLayoutFile);
	}
	SaveAllOpenProjectEditedFiles(); //Blocking dialogs.
	SaveAgkIdeIni();
	SaveProjectFile(NULL);
	if (debug_is_running > 0 || remote_debug_is_running ) {
		//Disconnect device before closing down app.
		uString bc_err;
		BroadcasterCommand("stop",bc_err);
		BroadcasterCommand("disconnectall",bc_err);
		if (debug_is_running)
			agk::TerminateApp(debug_is_running);
	}
	//StopBroadcaster();
	//Wait a few milli. so all threads is stopped.
	//Sleep(100);
}

static int progresscounter = 0;
static int objzoom = 0;
static bool objrotate = true;
static bool objrotatey = false;
static bool objrotatez = false;
static int diffuse_image = 0;
static int normal_image = 0;

#define MAXMEMBLOCKVIEW 64700
static MemoryEditor mem_view_edit;
static char cMemBuffer[MAXMEMBLOCKVIEW+8];

bool ProcessPreviewFile(cFolderItem::sFolderFiles * selectedfile)
{
	uString m_szParse, ext ,  rest, usTmp;

	ext = "";
	if (selectedfile != m_pLastSelectedFile)
	{
		//Get preview.

		//Free old preview media.
		if (m_pLastSelectedFile) {

			m_szParse = m_pLastSelectedFile->m_sName;
			int pos = m_szParse.RevFind('.');
			if (pos >= 0) {
				m_szParse.SubString(ext, pos); // ? jpeg = .jpe
				m_szParse.SubString(rest, 0, pos);
			}
			else {
				rest = m_szParse;
			}
			ext.Lower();

			//Images.
			if (ext.CompareTo(".fpe") == 0 || ext.CompareTo(".png") == 0 || ext.CompareTo(".jpg") == 0 || ext.CompareTo(".jpeg") == 0 || ext.CompareTo(".bmp") == 0 || ext.CompareTo(".gif") == 0) {
				if (m_pLastSelectedFile->iBigPreview > 0 &&
					m_pLastSelectedFile->iBigPreview != playmusic_icon &&
					m_pLastSelectedFile->iBigPreview != folder_icon &&
					m_pLastSelectedFile->iBigPreview != object3d_icon &&
					m_pLastSelectedFile->iBigPreview != agcfile_icon &&
					m_pLastSelectedFile->iBigPreview != luafile_icon &&
					m_pLastSelectedFile->iBigPreview != directx_icon &&
					m_pLastSelectedFile->iBigPreview != agkfile_icon &&
					m_pLastSelectedFile->iBigPreview != codeglsl_icon
					)
				{

					if (agk::GetImageExists(m_pLastSelectedFile->iBigPreview)) {
						agk::DeleteImage(m_pLastSelectedFile->iBigPreview);
						m_pLastSelectedFile->iBigPreview = 0;
					}
				}
			}
			if ((ext.CompareTo(".x") == 0 || ext.CompareTo(".dbo") == 0 || ext.CompareTo(".fbx") == 0 || ext.CompareTo(".obj") == 0 || ext.CompareTo(".3ds") == 0 || ext.CompareTo(".dae") == 0 || ext.CompareTo(".b3d") == 0 || ext.CompareTo(".3d") == 0 || ext.CompareTo(".lwo") == 0 || ext.CompareTo(".ago") == 0 )) {
				if (m_pLastSelectedFile->iBigPreview > 0) {
					if (agk::GetObjectExists(m_pLastSelectedFile->iBigPreview)) {
						agk::SetObjectVisible(m_pLastSelectedFile->iBigPreview, 0);


//						if (agk::GetImageExists(normal_image)) {
//							agk::SetObjectImage(m_pLastSelectedFile->iBigPreview, normal_image, 2); // remove flag , dont work.
//						}

						if (agk::GetImageExists(normal_image))
							agk::DeleteImage(normal_image);
						normal_image = 0;

						if (agk::GetImageExists(diffuse_image))
							agk::DeleteImage(diffuse_image);
						diffuse_image = 0;

						//If we use SetObjectNormalMap and delete the object and delete the normal_image , agk do strange things.
						//					if (agk::GetImageExists(normal_image))
						//						agk::DeleteImage(normal_image);
						//					normal_image = 0;

						agk::LoadImage("ddssdds"); // This fix the issue ?

						agk::DeleteObject(m_pLastSelectedFile->iBigPreview);
						m_pLastSelectedFile->iBigPreview = 0;
					}
					objzoom = 0;
					//objrotate = true;

				}
			}
			if (ext.CompareTo(".wav") == 0) {
				if (m_pLastSelectedFile->iBigPreview > 0) {
					if (agk::GetSoundExists(m_pLastSelectedFile->iBigPreview)) {
						agk::DeleteSound(m_pLastSelectedFile->iBigPreview);
						m_pLastSelectedFile->iBigPreview = 0;
					}
				}
			}
			if (ext.CompareTo(".mp3") == 0) {
				if (m_pLastSelectedFile->iBigPreview > 0) {
					if (agk::GetMusicExists(m_pLastSelectedFile->iBigPreview)) {
						agk::DeleteMusic(m_pLastSelectedFile->iBigPreview);
						m_pLastSelectedFile->iBigPreview = 0;
					}
				}
			}
			if (ext.CompareTo(".ogg") == 0) {
				if (m_pLastSelectedFile->iBigPreview > 0) {
					if (agk::GetMusicExistsOGG(m_pLastSelectedFile->iBigPreview)) {
						agk::DeleteMusicOGG(m_pLastSelectedFile->iBigPreview);
						m_pLastSelectedFile->iBigPreview = 0;
					}
				}
			}
		}

		//Load new preview media.

		if (selectedfile) {
			bool bUnknown = true;
			ext = "";
			m_szParse = selectedfile->m_sName;
			int pos = m_szParse.RevFind('.');
			if (pos >= 0) {
				m_szParse.SubString(ext, pos); // ? jpeg = .jpe
				m_szParse.SubString(rest, 0, pos);
			}
			else {
				rest = m_szParse;
			}
			ext.Lower();

			usTmp = "raw:";
			usTmp.Append(m_pSelectedFolder->m_sFolderFullPath);
			usTmp.Append("/");
			usTmp.Append(rest);
			usTmp.Append(ext);
#ifdef AGK_WINDOWS
#ifdef NASSUPPORT
			//NAS support.
			usTmp.ReplaceStr("/", "\\");
#endif
#endif

			if (ext.CompareTo(".png") == 0 || ext.CompareTo(".jpg") == 0 || ext.CompareTo(".jpeg") == 0 || ext.CompareTo(".bmp") == 0 || ext.CompareTo(".gif") == 0)
			{
				selectedfile->iBigPreview = agk::LoadImage(usTmp); //LoadImageResized(usTmp,0.5,0.5,0)
				bUnknown = false;
			}
			if (ext.CompareTo(".ogg") == 0) {
				selectedfile->iBigPreview = agk::LoadMusicOGG(usTmp); //LoadImageResized(usTmp,0.5,0.5,0)
				bUnknown = false;
			}
			if (ext.CompareTo(".mp3") == 0 ) {
				selectedfile->iBigPreview = agk::LoadMusic(usTmp); //LoadImageResized(usTmp,0.5,0.5,0)
				bUnknown = false;
			}
			if (ext.CompareTo(".wav") == 0 ) {
				selectedfile->iBigPreview = agk::LoadSound(usTmp); //LoadImageResized(usTmp,0.5,0.5,0)
				bUnknown = false;
			}
			if (ext.CompareTo(".ttf") == 0) {
				selectedfile->iBigPreview = 1; //previewFont;

				//Font path
				uString sFontPath = m_pSelectedFolder->m_sFolderFullPath;
				sFontPath.Append("/");
				sFontPath.Append(rest);
				sFontPath.Append(ext);
				strcpy(cPreviewFont, sFontPath.GetStr());
				bPreviewFontLoaded = false;
				bUnknown = false;
			}

			//|| ext.CompareTo(".dbo") == 0 
			//loading a dbo can really make strange things in AGK.

			if ((ext.CompareTo(".x") == 0 || ext.CompareTo(".fbx") == 0 || ext.CompareTo(".obj") == 0 || ext.CompareTo(".3ds") == 0 || ext.CompareTo(".dae") == 0 || ext.CompareTo(".b3d") == 0 || ext.CompareTo(".3d") == 0 || ext.CompareTo(".lwo") == 0 || ext.CompareTo(".ago") == 0 )) {
				
				//LoadObject("dd.dbo") // dont fail.
				//if you used a normalmap before the screen looks compleate wrong ?

				//iPreviewRenderTarget
				//ext.CompareTo(".dbo") == 0 
				PleaseWait();
				//Some FBX can cause assert so always "release".
				selectedfile->iBigPreview = agk::LoadObject(usTmp);
				PleaseWaitEnd();
				if (agk::GetObjectExists(selectedfile->iBigPreview)) {
					FixAndCenterObjectPivot(selectedfile->iBigPreview);
					agk::SetObjectVisible(selectedfile->iBigPreview, 1);
				}
				else
					selectedfile->iBigPreview = 0;
				bUnknown = false;
			}

			if (bUnknown) {
				usTmp = m_pSelectedFolder->m_sFolderFullPath;
				usTmp.Append("/");
				usTmp.Append(rest);
				usTmp.Append(ext);
				//usTmp = {m_pData=0x06cd94d8 "D:/div/SteamLibrary/steamapps/common/Game Guru/Files/scriptbank/gfx/gfx folder.txt" ...}
				FILE* datafile = fopen(usTmp.GetStr(), "rb+");
				if (!datafile)
					datafile = AGKfopen(usTmp.GetStr(), "rb+");

				if (datafile) {
					selectedfile->iBigPreview = fread(&cMemBuffer[0], 1, MAXMEMBLOCKVIEW , datafile);
					cMemBuffer[selectedfile->iBigPreview] = 0;
					fclose(datafile);
					ImGui::BeginChild("##scrolling");
					ImGui::SetScrollY(0.0);
					//ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + (GotoAddr / Cols) * ImGui::GetTextLineHeight());
					ImGui::EndChild();
					mem_view_edit.DataEditingAddr = 0x0;
					mem_view_edit.DataPreviewAddr = 0x0;

					if (selectedfile->iBigPreview <= 0) {
						strcpy(cMemBuffer, "File is empty.");
						selectedfile->iBigPreview = strlen(cMemBuffer);
						bMemTextOnly = true;
					}

					//bMemTextOnly = false;
				}
				else {
					//long fs = cp_getfilesize((char *) usTmp.GetStr());
					sprintf(cMemBuffer, "Cant open file: %s", usTmp.GetStr());
					selectedfile->iBigPreview = strlen(cMemBuffer);
					bMemTextOnly = true;
				}
			}
		}

		m_pLastSelectedFile = selectedfile;
	}

	if (selectedfile)
	{
		int iBorderSize = 15; //11
		int TitleSize = 24;
		int iTextHeight = 1.4*ImGui::GetFontSize();
		float fPreviewWidth = agk::Round(ImGui::GetWindowSize().x)- iBorderSize;
		float fPreviewHeight = agk::Round(ImGui::GetWindowSize().y)- (iBorderSize +TitleSize + iTextHeight);

		//ImGui::TextWrapped(selectedfile->m_sName);
		bool copymediabut = false;
		bool bUnknown = true;
		ImVec2 vUnknownPos;

		if (selectedfile->iBigPreview > 0 ) {

			m_szParse = selectedfile->m_sName;
			ext = "";
			int pos = m_szParse.RevFind('.');
			if (pos >= 0) {
				m_szParse.SubString(ext, pos); // ? jpeg = .jpe
				m_szParse.SubString(rest, 0, pos);
			}
			else {
				rest = m_szParse;
			}
			ext.Lower();

			if ((ext.CompareTo(".x") == 0 || ext.CompareTo(".dbo") == 0 || ext.CompareTo(".fbx") == 0 || ext.CompareTo(".obj") == 0 || ext.CompareTo(".3ds") == 0 || ext.CompareTo(".dae") == 0 || ext.CompareTo(".b3d") == 0 || ext.CompareTo(".3d") == 0 || ext.CompareTo(".lwo") == 0 || ext.CompareTo(".ago") == 0 )) {
				static float fMaxSide = 1.0;

				cImage *pImage = agk::GetImagePtr(iPreviewRenderTarget);
				int iImgW = iDeviceWidth;
				int iImgH = iDeviceHeight;
				char cText[256];

				sprintf(cText, "%s: ", selectedfile->m_sName.GetStr());

				vUnknownPos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(vUnknownPos.x, vUnknownPos.y + 1));

				int iTextWidth = ImGui::CalcTextSize(cText).x + (8.7*ImGui::GetFontSize()) + (4.7*ImGui::GetFontSize());
				if (iTextWidth > fPreviewWidth) {
					ImGui::Text("%s", cText);
				}
				else {
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((fPreviewWidth - iTextWidth) * 0.5), ImGui::GetCursorPos().y));
					ImGui::Text("%s", cText);
				}
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), vUnknownPos.y - 1));
				//ImGui::Spacing();
				//ImGui::SameLine();
				ImGui::PushItemWidth((5.2*ImGui::GetFontSize()));
				int zoomstep = (fMaxSide / 100.0);
				if (zoomstep < 1)
					zoomstep = 1;
				ImGui::InputInt("##zoomobj", &objzoom, zoomstep, zoomstep*10, 0);
				
				ImGuiIO& io = ImGui::GetIO();
				if( ImGui::IsWindowHovered() && io.MouseWheel != 0.0f) {
					float add = io.MouseWheel;
					if (add > 0) {
						objzoom += zoomstep;
					}
					else {
						objzoom -= zoomstep;
					}
				}


				ImGui::PopItemWidth();

				ImGui::SameLine();
				ImGui::PushItemWidth(15);
				ImGui::Button("D");
				ImGui::PopItemWidth();
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag/Drop your diffuse texture here.");

				//Drag/Drop Diffuse.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MODEL_DROP_TARGET", 0)) // ImGuiDragDropFlags_AcceptNoDrawDefaultRect
					{
						if (pDragDropFile) {

							IM_ASSERT(payload->DataSize == sizeof(cFolderItem::sFolderFiles *));
							cFolderItem::sFolderFiles * payload_n = (cFolderItem::sFolderFiles *) payload->Data;
							payload_n = payload_n->m_dropptr;
							if (payload_n) {
								uString fullfilename;
								fullfilename = "raw:";
								fullfilename.Append(payload_n->m_sPath);
								int pos = fullfilename.RevFindStr("/");
								if (!(fullfilename.GetLength() - 1 == pos)) {
									fullfilename.Append("/");
								}
								fullfilename.Append(payload_n->m_sName);
#ifdef AGK_WINDOWS
#ifdef NASSUPPORT
								//NAS support.
								fullfilename.ReplaceStr("/", "\\");
#endif
#endif

								if (agk::GetImageExists(diffuse_image))
									agk::DeleteImage(diffuse_image);
								agk::SetGenerateMipmaps(1); // generate and use mipmaps
								diffuse_image = agk::LoadImage(fullfilename.GetStr());
								agk::SetImageWrapU(diffuse_image, 1);
								agk::SetImageWrapV(diffuse_image, 1);

								agk::SetGenerateMipmaps(0);
								if (agk::GetImageExists(diffuse_image)) {
									agk::SetObjectImage(selectedfile->iBigPreview, diffuse_image, 0);
								}
							}

						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 4, ImGui::GetCursorPosY()));
				ImGui::PushItemWidth(ImGui::GetFontSize());
				ImGui::Button("N");
				ImGui::PopItemWidth();
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag/Drop your normal texture here.");

				//Drag/Drop Normal map.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MODEL_DROP_TARGET", 0)) // ImGuiDragDropFlags_AcceptNoDrawDefaultRect
					{
						if (pDragDropFile) {

							IM_ASSERT(payload->DataSize == sizeof(cFolderItem::sFolderFiles *));
							cFolderItem::sFolderFiles * payload_n = (cFolderItem::sFolderFiles *) payload->Data;
							payload_n = payload_n->m_dropptr;
							if (payload_n) {
								uString fullfilename;
								fullfilename = "raw:";
								fullfilename.Append(payload_n->m_sPath);
								int pos = fullfilename.RevFindStr("/");
								if (!(fullfilename.GetLength() - 1 == pos)) {
									fullfilename.Append("/");
								}
								fullfilename.Append(payload_n->m_sName);
#ifdef AGK_WINDOWS
#ifdef NASSUPPORT
								//NAS support.
								fullfilename.ReplaceStr("/", "\\");
#endif
#endif

								if (agk::GetImageExists(normal_image))
									agk::DeleteImage(normal_image);
								agk::SetGenerateMipmaps(1); // generate and use mipmaps
								normal_image = agk::LoadImage(fullfilename.GetStr());
								agk::SetImageWrapU(normal_image, 1);
								agk::SetImageWrapV(normal_image, 1);
								agk::SetGenerateMipmaps(0); // disable use mipmaps
								if (agk::GetImageExists(normal_image)) {
									if(!diffuse_image == 0 )
										agk::SetObjectNormalMap(selectedfile->iBigPreview, normal_image);
								}
							}

						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				ImGui::PushItemWidth((ImGui::GetFontSize()*0.8));
				ImGui::Checkbox("##X", &objrotate);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rotate X");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 6, ImGui::GetCursorPosY()));
				ImGui::Checkbox("##Y", &objrotatey);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rotate Y");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 6, ImGui::GetCursorPosY()));
				ImGui::Checkbox("##Z", &objrotatez);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rotate Z");
				ImGui::PopItemWidth();

				float fRatio;
				if ((fPreviewWidth / iImgW) < (fPreviewHeight / iImgH))
					fRatio = fPreviewWidth / iImgW;
				else
					fRatio = fPreviewHeight / iImgH;

				if (pImage)
					myTextureID = pImage->GetTextureID() + 200000; // 100000+ = No Color Array , 200000+ = No transparent
				float fCenterX = (fPreviewWidth - iImgW*fRatio) * 0.5;
				float fCenterY = (fPreviewHeight - iImgH*fRatio) * 0.5;
				ImGui::SetCursorPos(ImVec2(fCenterX + 2, fCenterY + TitleSize + iTextHeight + 1)); //TitleSize
				if (ImGui::ImgBtn(iPreviewRenderTarget, ImVec2(iImgW*fRatio, iImgH*fRatio), ImColor(255, 255, 255, 0) , ImColor(255, 255, 255, 255) , ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 255) ) ) {
					//3d but pressed.
				}
				//Always update mouse rotation.
				static ImVec2 lastmousepos;
				static bool startpressing = false;
				static ImVec2 slowstoprotation;

				//ImGui::IsItemFocused(), ImGui::IsWindowFocused() ,&& ImGui::IsMouseHoveringWindow()
				if (ImGui::IsItemHovered()) {
//					if (ImGui::IsMouseClicked(0, true)) {
					if (agk::GetPointerState() ) {
						//agk::Print("PRESSED");
						ImVec2 mousechange;
						if (startpressing == false)
							lastmousepos = ImGui::GetMousePos();

						mousechange = lastmousepos - ImGui::GetMousePos();

						if (mousechange.x != 0) {
							float changex = mousechange.x * 0.1;
							agk::RotateObjectLocalY(selectedfile->iBigPreview, changex);
						}
						if (mousechange.y != 0) {
							float changey = mousechange.y * 0.1;
							agk::RotateObjectLocalX(selectedfile->iBigPreview, changey);
						}

						//lastmousepos = ImGui::GetMousePos();
						startpressing = true;
					}
					else {
						//ImVec2 lastmousepos = ImGui::GetMousePos();
						startpressing = false;
					}
				}

				if (pref.iIDEUpdateEventSleep) {
					if (eventTimes.fLastPreviewTime + 0.033f <= eventTimes.fCurrTime) //PAUL: 0.065f = 15fps 0.033 = 30fps.
					{
						eventTimes.fLastPreviewTime = eventTimes.fCurrTime;
						ide_force_rendering = true; // force screen update
						fMaxSide = RenderRotate3DObject(selectedfile->iBigPreview, iPreviewRenderTarget, objzoom, objrotate, objrotatey, objrotatez);
					}
				}
				else {
					fMaxSide = RenderRotate3DObject(selectedfile->iBigPreview, iPreviewRenderTarget, objzoom, objrotate, objrotatey, objrotatez);
				}
				//Drag/Drop Diffuse.
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MODEL_DROP_TARGET", 0)) // ImGuiDragDropFlags_AcceptNoDrawDefaultRect
					{
						if (pDragDropFile) {

							IM_ASSERT(payload->DataSize == sizeof(cFolderItem::sFolderFiles *));
							cFolderItem::sFolderFiles * payload_n = (cFolderItem::sFolderFiles *) payload->Data;
							payload_n = payload_n->m_dropptr;
							if (payload_n) {
								uString fullfilename;
								fullfilename = "raw:";
								fullfilename.Append(payload_n->m_sPath);
								int pos = fullfilename.RevFindStr("/");
								if (!(fullfilename.GetLength() - 1 == pos)) {
									fullfilename.Append("/");
								}
								fullfilename.Append(payload_n->m_sName);
#ifdef AGK_WINDOWS
#ifdef NASSUPPORT
								//NAS support.
								fullfilename.ReplaceStr("/", "\\");
#endif
#endif


								if (agk::GetImageExists(diffuse_image))
									agk::DeleteImage(diffuse_image);
								agk::SetGenerateMipmaps(1); // generate and use mipmaps
								diffuse_image = agk::LoadImage(fullfilename.GetStr());
								agk::SetImageWrapU(diffuse_image, 1);
								agk::SetImageWrapV(diffuse_image, 1);

								agk::SetGenerateMipmaps(0);
								if (agk::GetImageExists(diffuse_image)) {
									agk::SetObjectImage(selectedfile->iBigPreview, diffuse_image, 0);
								}
							}

						}
					}
					ImGui::EndDragDropTarget();
				}

				copymediabut = true;
				bUnknown = false;
			}

			if (ext.CompareTo(".png") == 0 || ext.CompareTo(".jpg") == 0 || ext.CompareTo(".jpeg") == 0 || ext.CompareTo(".bmp") == 0 || ext.CompareTo(".gif") == 0)
			{
				cImage *pImage = agk::GetImagePtr(selectedfile->iBigPreview);
				int iImgW = agk::GetImageWidth(selectedfile->iBigPreview);
				int iImgH = agk::GetImageHeight(selectedfile->iBigPreview);
				char cText[256];
				sprintf(cText, "(%ldx%ld) %s", iImgW, iImgH, selectedfile->m_sName.GetStr());
				int iTextWidth = ImGui::CalcTextSize(cText).x;
				if (iTextWidth > fPreviewWidth) {
					ImGui::Text("%s",cText);
				}
				else {
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((fPreviewWidth - iTextWidth) * 0.5) , ImGui::GetCursorPos().y));
					ImGui::Text("%s", cText);
				}

				float fRatio;
				if( (fPreviewWidth / iImgW) < (fPreviewHeight / iImgH) )
					fRatio = fPreviewWidth / iImgW;
				else
					fRatio = fPreviewHeight / iImgH;

				if (pImage) {
					myTextureID = pImage->GetTextureID() + 100000; // 100000+ = No Color Array , 200000+ = No transparent
					float fCenterX = (fPreviewWidth - iImgW*fRatio) * 0.5;
					float fCenterY = (fPreviewHeight - iImgH*fRatio) * 0.5;
					ImGui::SetCursorPos(ImVec2(fCenterX + 2, fCenterY + TitleSize + iTextHeight + 1)); //TitleSize
					ImGui::ImageButton((void*)myTextureID, ImVec2(iImgW*fRatio, iImgH*fRatio), ImVec2(0, 0), ImVec2(1, 1), -5, pref.icon_background_color, ImVec4(1, 1, 1, 1));
				}
				copymediabut = true;
				bUnknown = false;
			}

			if (ext.CompareTo(".ttf") == 0) {
				if (bPreviewFontLoaded) {
					if (previewFont && previewFont->FontSize > 0.0f ) {
						if (!previewFont->IsLoaded())
							ImGui::PushFont(defaultfont);  //defaultfont
						else
							ImGui::PushFont(previewFont);

						ImGui::Text(selectedfile->m_sName);
						ImGui::Text("0123456789 0 1 2 3 4 5 6 7 8 9");
						ImGui::Text("abcdefghijklmnopqrstuvwxyz");
						ImGui::Text("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
						ImGui::Text("The quick brown fox jumped over the lazy dog.");
						ImGui::Text("The quick brown fox jumped over the lazy dog.");
						ImGui::Text("The quick brown fox jumped over the lazy dog.");
						ImGui::Text("The quick brown fox jumped over the lazy dog.");
						//ImGui::Text(cPreviewFont);
						ImGui::PopFont();
					}
				}
				bUnknown = false;
			}

			if (ext.CompareTo(".ogg") == 0 || ext.CompareTo(".mp3") == 0 || ext.CompareTo(".wav") == 0)
			{
				cImage *pImage = agk::GetImagePtr(playmusic_icon);
				int iImgW = 256;
				int iImgH = 256;
				char cText[256];
				int playing = 0;

				if (ext.CompareTo(".wav") == 0) {
					playing = agk::GetSoundsPlaying(selectedfile->iBigPreview);
				}
				if (ext.CompareTo(".mp3") == 0) {
					if (playing <= 0) playing = agk::GetMusicPlaying();
				}
				if (ext.CompareTo(".ogg") == 0) {
					//selectedfile->iBigPreview < 300
					if (playing <= 0 ) playing = agk::GetMusicPlayingOGG(selectedfile->iBigPreview);
				}
				if(playing)
					sprintf(cText, "Playing %s", selectedfile->m_sName.GetStr());
				else
					sprintf(cText, "%s", selectedfile->m_sName.GetStr());

				int iTextWidth = ImGui::CalcTextSize(cText).x;
				if (iTextWidth > fPreviewWidth) {
					ImGui::Text("%s", cText);
				}
				else {
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((fPreviewWidth - iTextWidth) * 0.5), ImGui::GetCursorPos().y));
					ImGui::Text("%s", cText);
				}

				float fRatio;
				if ((fPreviewWidth / iImgW) < (fPreviewHeight / iImgH))
					fRatio = fPreviewWidth / iImgW;
				else
					fRatio = fPreviewHeight / iImgH;

				if (pImage)
					myTextureID = pImage->GetTextureID() + 200000; // 100000+ = No Color Array , 200000+ = No transparent
				float fCenterX = (fPreviewWidth - iImgW*fRatio) * 0.5;
				float fCenterY = (fPreviewHeight - iImgH*fRatio) * 0.5;
				ImGui::SetCursorPos(ImVec2(fCenterX + 2, fCenterY + TitleSize + iTextHeight + 1)); //TitleSize
				if (ImGui::ImgBtn(playmusic_icon, ImVec2(iImgW*fRatio, iImgH*fRatio) ) || iFirstPreviewAction == 1 ) {
					//Play music
					if (playing) {
						agk::StopSound(selectedfile->iBigPreview);
						agk::StopMusic();
						agk::StopMusicOGG(selectedfile->iBigPreview);
					}
					else {
						if (ext.CompareTo(".ogg") == 0)
							agk::PlayMusicOGG(selectedfile->iBigPreview);
						else if (ext.CompareTo(".mp3") == 0)
							agk::PlayMusic(selectedfile->iBigPreview);
						else if (ext.CompareTo(".wav") == 0)
							agk::PlaySound(selectedfile->iBigPreview);
					}
					iFirstPreviewAction = 0;
				}
				copymediabut = true;
				bUnknown = false;
			}

			if (bUnknown)
			{
				vUnknownPos = ImGui::GetCursorPos();
				ImGui::SetCursorPos( ImVec2(vUnknownPos.x, vUnknownPos.y + 1));
				ImGui::Text("%.22s: ", selectedfile->m_sName.GetStr());
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), vUnknownPos.y - 1));
				ImGui::Checkbox("Text Only", &bMemTextOnly);
				ImGui::SameLine();
				vUnknownPos = ImGui::GetCursorPos();
				ImGui::Spacing();

				if (!editorfont->IsLoaded())
					ImGui::PushFont(defaultfont);  //defaultfont
				else
					ImGui::PushFont(editorfont);

				//fPreviewWidth -= 10;
				float fFontSize = ImGui::GetFontSize();
				mem_view_edit.Cols = fPreviewWidth / (ImGui::GetFontSize() *2.90);
				if (mem_view_edit.Cols <= 1)
					mem_view_edit.Cols = 1;
				if (fPreviewWidth > 320)
					mem_view_edit.Cols++;
				if (fPreviewWidth > 420)
					mem_view_edit.Cols++;
				if (fPreviewWidth > 620)
					mem_view_edit.Cols++;
				if (fPreviewWidth > 720)
					mem_view_edit.Cols++;
				if (fPreviewWidth > 820)
					mem_view_edit.Cols++;
				if (fPreviewWidth > 920)
					mem_view_edit.Cols++;
				if (fPreviewWidth > 1120)
					mem_view_edit.Cols++;

				if (bMemTextOnly) {
					cMemBuffer[MAXMEMBLOCKVIEW] = 0;
					//ImGuiWindowFlags_HorizontalScrollbar
					ImGui::BeginChild("##previewtextsscrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::TextUnformatted(cMemBuffer);
					ImGui::EndChild();
//					ImGui::TextWrapped(cMemBuffer);
				}
				else {
					mem_view_edit.ReadOnly = true;
					mem_view_edit.OptShowOptions = false;
					mem_view_edit.Open = true;
					mem_view_edit.DrawContents(&cMemBuffer[0], selectedfile->iBigPreview, 0x0000);
					mem_view_edit.Open = false;
				}
				ImGui::PopFont();

				copymediabut = true;

				//ImGui::SetItemAllowOverlap(); //Allow overlapping widgets
				//float yoff = ImGui::CalcTextSize("#").y;
				//ImGui::SetCursorPos(ImVec2( 6.0, ImGui::GetWindowSize().y - (yoff)-12.0 + ImGui::GetScrollY() ));
				//ImGui::Checkbox("Text Only", &bMemTextOnly);

			}
		}
		else {

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + iTextHeight+4));
			ImGui::Button(selectedfile->m_sName, ImVec2(fPreviewWidth, fPreviewHeight));
			copymediabut = true;
		}


		//Copy media to current active project media folder.
		if (copymediabut || iFirstPreviewAction == 2 ) {
			//Media not in current project , suggest to copy.
			if (pCurrentSelectedProject && selectedfile->m_sFolder.CompareCaseTo(pCurrentSelectedProject->m_sProjectFile) != 0) {
				ImGui::SetItemAllowOverlap(); //Allow overlapping widgets
											  //ImGui::SameLine(fPreviewWidth - 20.0); //18//64.0
				float yoff = ImGui::CalcTextSize("#").y;
				float xoff = ImGui::CalcTextSize("#").x;
				//float sSsize = 0.0;
				if (bUnknown) {
					ImGuiWindow *pwin = ImGui::GetCurrentWindow();
					//ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - (xoff * 6) - 6.0 - pwin->ScrollbarSizes.x, vUnknownPos.y));
					ImGui::SetCursorPos(vUnknownPos);
				}
				else
					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - (xoff * 6) - 8.0 , ImGui::GetWindowSize().y - (yoff)-12.0));

				char copytext[20];
				if (progresscounter > 0) {
					if (progresscounter > 50)
						strcpy(copytext, ".   ");
					else if (progresscounter > 40)
						strcpy(copytext, " .  ");
					else if (progresscounter > 30)
						strcpy(copytext, "  . ");
					else if (progresscounter > 20)
						strcpy(copytext, "   .");
					else if (progresscounter > 10)
						strcpy(copytext, "done");
					else
						strcpy(copytext, " ok ");
					progresscounter--;
				}
				else
					strcpy(copytext, "Copy");

				if (ImGui::Button(copytext, ImVec2(xoff*6.0, yoff + 6.0)) || iFirstPreviewAction == 2 ) {
					cProjectItem * searchProject;
					uString usDestination;
					usDestination = "";
					if (iFirstPreviewAction == 2)
						iFirstPreviewAction = 0;

					if (allProjects) {
						//Check if project is open.
						searchProject = allProjects;
						while (searchProject) {
							if (searchProject->m_sProjectFile.CompareCaseTo(pCurrentSelectedProject->m_sProjectFile) == 0)
							{
								usDestination = searchProject->m_sProjectFileFullPath;
								int pos = usDestination.RevFindStr("/");
								if (pos <= 0) pos = usDestination.RevFindStr("\\");
								if (pos > 0) {
									uString usFinal;
									usDestination.SubString(usFinal, 0, pos + 1);
									usFinal.Append("media/");
									usFinal.Append(selectedfile->m_sName);

									uString usSource;
									usSource = selectedfile->m_sPath;
									usSource.Append("/");
									usSource.Append(selectedfile->m_sName);

									bool oksave = true;
									if (file_exists((char*)usFinal.GetStr())) {
										oksave = overWriteFileBox((char*)usFinal.GetStr());
									}
									if (oksave) {
										cp_copyfile((char*)usSource.GetStr(), (char*)usFinal.GetStr());
										progresscounter = 50;
									}
									//agk::Print(usFinal.GetStr());
									break;
								}

							}
							searchProject = searchProject->m_pNext;
						}
					}

				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Copy file to current\nprojects media folder.");
			}
		}


	}

	return true;
}

void FixAndCenterObjectPivot(int obj)
{
	float maxX = agk::GetObjectSizeMaxX(obj);
	float maxY = agk::GetObjectSizeMaxY(obj);
	float maxZ = agk::GetObjectSizeMaxZ(obj);

	float minX = agk::GetObjectSizeMinX(obj);
	float minY = agk::GetObjectSizeMinY(obj);
	float minZ = agk::GetObjectSizeMinZ(obj);

	if (minX >= 0)
		minX = 0;
	if (minY >= 0)
		minY = 0;
	if (minZ >= 0)
		minZ = 0;

	float objsizex = (maxX + minX) * 0.5;
	float objsizey = (maxY + minY) * 0.5;
	float objsizez = (maxZ + minZ) * 0.5;

	agk::SetObjectRotation(obj, 0, 0, 0);
	agk::SetObjectPosition(obj, -objsizex, -objsizey, -objsizez);
	agk::FixObjectPivot(obj);
	return;
}

float GetObjectMaxSize(int obj)
{
	//Should be absolute centered so we can do this.
	float maxX = agk::GetObjectSizeMaxX(obj);
	float maxY = agk::GetObjectSizeMaxY(obj);
	float maxZ = agk::GetObjectSizeMaxZ(obj);
	float retsize = 0;
	if (maxX > retsize)
		retsize = maxX;
	if (maxY > retsize)
		retsize = maxY;
	if (maxZ > retsize)
		retsize = maxZ;
	return retsize*2.0;
}

float RenderRotate3DObject(int obj,int rendertarget, int zoom,bool rotatex, bool rotatey , bool rotatez )
{
/*PAUL: rem 
	if (pref.iIDEUpdateEventSleep) {
		//Disable event sleeping.
		extern unsigned int os_messages_wait;
		os_messages_wait = 14;
	}
*/
	agk::SetErrorMode(0); //No popup messages.
	agk::Print(""); // Needed ?
	agk::SetRenderToImage(rendertarget, -1);
	//agk::SetClearColor(AgkClearColor[0], AgkClearColor[1], AgkClearColor[2]);
	agk::ClearScreen();
	agk::SetCameraPosition(1, 0, 0, 0);
	
	float maxsize = GetObjectMaxSize(obj);
	if(maxsize < 2)
		agk::MoveCameraLocalZ(1, zoom -(20+(maxsize * 2.0)));
	else
		agk::MoveCameraLocalZ(1, zoom -((10 + maxsize * 2.0)));

//	agk::SetPrintSize(100);
//	agk::Print(maxsize);
//	agk::DrawLine(0, 0, iDeviceWidth, iDeviceHeight, 255, 255, 0);
//	agk::DrawLine(iDeviceWidth, 0, 0, iDeviceHeight, 255, 0, 0);

	//agk::Update(); // 1.0/30.0
	float tween = 60.0 / (1.0 / agk::GetFrameTime());
	if (rotatex)
		agk::RotateObjectLocalY(obj, 0.3*tween);
	if (rotatey)
		agk::RotateObjectLocalX(obj, 0.25*tween);
	if (rotatez)
		agk::RotateObjectLocalZ(obj, 0.3*tween);
	
	
	agk::SetObjectLightMode(obj, 1);
	agk::SetObjectVisible(obj, 1);
	agk::DrawObject(obj);
	agk::SetObjectVisible(obj, 0);

	agk::Render2DBack();
	agk::Render3D();
	agk::Render2DFront();
	agk::SetRenderToScreen();

	return maxsize;
}


cFolderItem * ProcessFolderFiles(cFolderItem * m_pSelectedFolder,const char * cpFilter,bool bDisplayAll)
{
	static cFolderItem * m_pLastSelectedFolder;
	char *oldpath, *ctmp;

	if (m_pSelectedFolder->m_sFolderFullPath.GetLength() > 0 ) {
		//Change this to only every 2 sec.
		if (m_pSelectedFolder && (agk::Timer() - m_pSelectedFolder->m_fLastTimeUpdate) >= 2.0 || bRefreshNextFolder ) {
			m_pSelectedFolder->m_fLastTimeUpdate = agk::Timer();

			struct stat sb;
			//char foldertime[MAX_PATH];
			if (stat(m_pSelectedFolder->m_sFolderFullPath, &sb) == 0 || bRefreshNextFolder ) {
				//strftime(foldertime, MAX_PATH, "%d-%m-%y %H:%M:%S", localtime(&(sb.st_mtime))); //st_mtime,st_ctime
				//Time stamp ok use it.
				//Check if we need to read files, if there are any deleted or new files.
				if ( sb.st_mtime != m_pSelectedFolder->m_tFolderModify || bRefreshNextFolder ) {

					bRefreshNextFolder = false;
					m_pSelectedFolder->m_tFolderModify = sb.st_mtime;

					//make sure deleted files are not the drag source.
					pDragDropFile = NULL;

					//Delete all old files.
					cFolderItem::sFolderFiles * m_pnextFiles, *m_pFiles = m_pSelectedFolder->m_pFirstFile;

					while (m_pFiles) {

						m_pnextFiles = m_pFiles->m_pNext;
						if (m_pFiles->bPreviewProcessed) {
							//Exclude our own images.
							if (m_pFiles->iPreview > 0 && 
								m_pFiles->iPreview != playmusic_icon &&
								m_pFiles->iPreview != folder_icon &&
								m_pFiles->iPreview != object3d_icon &&
								m_pFiles->iPreview != agcfile_icon &&
								m_pFiles->iPreview != luafile_icon &&
								m_pFiles->iPreview != directx_icon &&						
								m_pFiles->iPreview != agkfile_icon &&
								m_pFiles->iPreview != codeglsl_icon
								) {
								if (agk::GetImageExists(m_pFiles->iPreview)) {
									agk::DeleteImage(m_pFiles->iPreview);
									m_pFiles->iPreview = 0;
								}
							}
						}
						//Free any largepreviews in preview window.
						if (m_pFiles == m_pLastSelectedFile) {
							ProcessPreviewFile(NULL);
							m_pSelectedFile = NULL;
						}
						delete m_pFiles;
						m_pFiles = m_pnextFiles;
					}

					m_pSelectedFolder->m_pFirstFile = NULL;

					//Make sure to read changes.
					m_pSelectedFolder->m_bFilesRead = false;
					m_pLastSelectedFolder = NULL;
				}
			}
		}
		if (m_pSelectedFolder && m_pLastSelectedFolder != m_pSelectedFolder && !m_pSelectedFolder->m_bFilesRead)
		{
			oldpath = agk::GetWritePath();
			agk::SetRawWritePath(m_pSelectedFolder->m_sFolderFullPath);
			agk::SetFolder("");

			//Check/Read files.
			m_pSelectedFolder->m_pFirstFile = NULL;
			int mc = 0;
			cFolderItem::sFolderFiles * m_pFiles;

			std::vector<std::string> sorted_files;
			ctmp = agk::GetFirstFile(1);
			while (ctmp && strlen(ctmp) > 0 && mc < 9999) {

				//Looks stupid when everything else is sorted by name , remove for now.
//				if (pestrcasestr(ctmp, ".agk")) {
//					uString ftmp = "![]!";
//					ftmp.Append(ctmp);
//					sorted_files.push_back(ftmp.GetStr());			}
//				else
					sorted_files.push_back(ctmp);
				delete[] ctmp;
				ctmp = NULL;
				ctmp = agk::GetNextFile();
				mc++;
			}
			if (!sorted_files.empty()) {
				std::sort(sorted_files.begin(), sorted_files.end() , NoCaseLess);
				//				std::reverse(sorted_files.begin(), sorted_files.end());

				std::vector<std::string>::const_iterator it = sorted_files.begin();
				if (it->size() > 0) {
					cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles();
					pNewItem->m_sName = "...";
					pNewItem->m_sPath = "";
					pNewItem->m_sFolder = "[na]";
					pNewItem->iFlags = 0;
					pNewItem->iPreview = 0;
					pNewItem->iBigPreview = 0;
					pNewItem->id = iTotalFiles++;
					pNewItem->bPreviewProcessed = false;
					pNewItem->m_pNext = NULL;
					m_pSelectedFolder->m_pFirstFile = pNewItem;
					m_pFiles = pNewItem;
				}
				//++it;
				for (; it != sorted_files.end(); ++it) {
					if (it->size() > 0) {
						cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles();
						pNewItem->m_sName.SetStr(it->c_str());
						pNewItem->m_sPath.SetStr(m_pSelectedFolder->m_sFolderFullPath.GetStr());
						pNewItem->m_sFolder = m_pSelectedFolder->m_sFolder;
						pNewItem->m_sFolder.ReplaceStr("-> ", "");
						pNewItem->m_sFolder.ReplaceStr("'", "");
						pNewItem->m_sFolder.ReplaceStr(" Project Media", "");
						pNewItem->iFlags = 0;
						pNewItem->iPreview = 0;
						pNewItem->iBigPreview = 0;
						pNewItem->id = iTotalFiles++;
						pNewItem->bPreviewProcessed = false;
						pNewItem->m_pNext = NULL;
						m_pFiles->m_pNext = pNewItem;
						m_pFiles = pNewItem;
					}
				}
				sorted_files.clear();
			}


/*
			ctmp = agk::GetFirstFile(1);
			if (ctmp) {
				if (strlen(ctmp) > 0) {
					cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles();
					pNewItem->m_sName = "...";
					pNewItem->m_sPath = "";
					pNewItem->m_sFolder = "[na]";
					pNewItem->iFlags = 0;
					pNewItem->iPreview = 0;
					pNewItem->iBigPreview = 0;
					pNewItem->id = iTotalFiles++;
					pNewItem->bPreviewProcessed = false;

					pNewItem->m_pNext = NULL;
					m_pSelectedFolder->m_pFirstFile = pNewItem;
					m_pFiles = pNewItem;
				}

				while (strlen(ctmp) > 0 && mc < 9999) {
					cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles();
					pNewItem->m_sName.SetStr(ctmp);
					pNewItem->m_sPath.SetStr(m_pSelectedFolder->m_sFolderFullPath.GetStr());
					pNewItem->m_sFolder = m_pSelectedFolder->m_sFolder;
					pNewItem->m_sFolder.ReplaceStr("-> ", "");
					pNewItem->m_sFolder.ReplaceStr("'", "");
					pNewItem->m_sFolder.ReplaceStr(" Project Media", "");
					pNewItem->iFlags = 0;
					pNewItem->iPreview = 0;
					pNewItem->iBigPreview = 0;
					pNewItem->id = iTotalFiles++;
					pNewItem->bPreviewProcessed = false;
					pNewItem->m_pNext = NULL;

					m_pFiles->m_pNext = pNewItem;
					m_pFiles = pNewItem;

					ctmp = agk::GetNextFile();
					if (!ctmp)
						break;
					mc++;
				}
			}
*/


			m_pSelectedFolder->m_bFilesRead = true;

			//Sort entryes.

			agk::SetRawWritePath(oldpath);
			m_pLastSelectedFolder = m_pSelectedFolder;
			pDragDropFile = NULL;
		}

	}
	ImGuiTextFilter Filter(cpFilter);

//	ImGui::BeginChild("##ChrildTime1");
//		ImGui::Text("TIME: ");
//		ImGui::SameLine();
//		ImGui::Text(foldertime);
//	ImGui::EndChild();


	if (m_pSelectedFolder ) {
		cFolderItem::sFolderFiles * m_pFiles = NULL;

		if(m_pSelectedFolder->m_pFirstFile && m_pSelectedFolder->m_pFirstFile->m_pNext)
			m_pFiles = m_pSelectedFolder->m_pFirstFile->m_pNext;

		int preview_count = 0;
		
		int iColumns = agk::Round(ImGui::GetWindowSize().x / (26+ pref.media_icon_size));
		if (iColumns <= 1)
			iColumns = 1;
		ImGui::BeginChild("##Chrild1");
		ImGui::Columns(iColumns, "mycolumns3", false);  // 3-ways, no border

		if(pref.media_icon_size == 32)
			ImGui::SetWindowFontScale(0.5);

		if (Filter.PassFilter(".folder")) {
			//Display Folders.
			if (m_pSelectedFolder->m_pSubFolder) {
				//static cFolderItem * m_pLastSelectedFolderParent = NULL;
				cFolderItem * m_pOtherFolders = m_pSelectedFolder->m_pSubFolder;
				int folderid = 15000;
				//.folder
				while (m_pOtherFolders) {

					if (!(m_pOtherFolders->m_sFolder.CompareTo("...") == 0 && !m_pOtherFolders->m_pSubFolder )) {
						ImGui::PushID(folderid++);
						cImage *pImage = agk::GetImagePtr(folder_icon);
						if (pImage)
							myTextureID = pImage->GetTextureID() + 100000; // 200000 // 100000+ = No Color Array , 200000+ = No transparent
						if (ImGui::ImageButton((void*)myTextureID, ImVec2(pref.media_icon_size, pref.media_icon_size), ImVec2(0, 0), ImVec2(1, 1), -5, pref.icon_background_color, ImVec4(1, 1, 1, 1)))
						{
							//m_pSelectedFile = m_pFiles;
						}
						if (ImGui::IsItemHovered()) {
							ImGui::SetTooltip("%s", m_pOtherFolders->m_sFolder.GetStr());
							if (ImGui::IsMouseDoubleClicked(0)) {
								//Change folder.
								//m_pSelectedFolder = m_pOtherFolders;
								ImGui::PopID();
								//ImGui::SetWindowFontScale(1.00);
								ImGui::Columns(1);
								ImGui::EndChild();

								//								if (m_pOtherFolders && m_pOtherFolders->m_pSubFolder && m_pOtherFolders->m_pSubFolder->m_pSubFolder)
								if (m_pOtherFolders && m_pOtherFolders->m_pSubFolder)
								{
									//if(m_pOtherFolders->m_pSubFolder != m_pStartFolder->m_pNext )
									//	if (m_pOtherFolders->m_pSubFolder != m_pStartFolder)
									if (m_pOtherFolders->m_sFolder.CompareTo("...") == 0) {
										if (m_pOtherFolders->m_pSubFolder)
											return(m_pOtherFolders->m_pSubFolder);
									}
									return(m_pOtherFolders);
								}
								return(m_pSelectedFolder);
							}
						}

						if (m_pOtherFolders->m_sFolder.CompareTo("...") != 0) {
//Folders can be located in finder ?
#ifndef AGK_MACOS
							if (ImGui::BeginPopupContextItemAGK("abrowserfoldercontextmenu")) //"folder context menu"
							{
#ifdef AGK_WINDOWS
								ImGui::Separator();
								if (ImGui::MenuItem("Open In Explorer", "")) {
									uString sTmp = m_pOtherFolders->m_sFolderFullPath;
									sTmp.ReplaceStr("/", "\\");
									ITEMIDLIST *pidl = ILCreateFromPathA(sTmp.GetStr());
									if (pidl) {
										SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
										ILFree(pidl);
									}
								}
								ImGui::Separator();
								ImGui::EndPopup();

#else
#ifdef AGK_MACOS
								//Mac cant open,locate folders in finder ?
								//Disable for now, works fine for files.

								ImGui::Separator();
								if (ImGui::MenuItem("Open In Finder", "")) {
									uString sTmp = m_pOtherFolders->m_sFolderFullPath;
									sTmp.ReplaceStr("/", "\\");

									//Test add additional slash
									sTmp.Append("/");

									NSString* path = [NSString stringWithUTF8String : sTmp.GetStr()];
									NSArray *fileURLs = [NSArray arrayWithObjects : [NSURL fileURLWithPath : path], nil];
									[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];

								}
								ImGui::Separator();
								ImGui::EndPopup();
#endif
#endif
							}
#endif
						}

						int iTextWidth = ImGui::CalcTextSize(m_pOtherFolders->m_sFolder.GetStr()).x;
						if(iTextWidth < (8+ pref.media_icon_size) )
							ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + ((3 + (pref.media_icon_size*0.5)) - (iTextWidth*0.5) ) , ImGui::GetCursorPosY()));
						//ImGui::SetCursorPos( ImVec2(ImGui::GetCursorPosX() + (45-(iTextWidth*0.5)), ImGui::GetCursorPosY()));
							//center
						ImGui::TextWrapped("%s", m_pOtherFolders->m_sFolder.GetStr());

						preview_count++;
						ImGui::PopID();
						ImGui::NextColumn();
					}
					m_pOtherFolders = m_pOtherFolders->m_pNext;

				}

			}
		}
		//Display files.
		if (m_pFiles && m_pSelectedFolder->m_pFirstFile && m_pSelectedFolder->m_pFirstFile->m_pNext) {

			while (m_pFiles) {
				if (bDisplayAll || Filter.PassFilter(m_pFiles->m_sName)) {

					ImGui::PushID(m_pFiles->id);

					uString m_szParse, ext, usTmp;
					ext = "na";

					if (m_pFiles->bPreviewProcessed && m_pFiles->iPreview > 0) {

						cImage *pImage = agk::GetImagePtr(m_pFiles->iPreview);
						if (pImage)
							myTextureID = pImage->GetTextureID() + 100000; // 100000+ = No Color Array , 200000+ = No transparent

						m_szParse = m_pFiles->m_sName;
						int pos = m_szParse.RevFind('.');
						if (pos >= 0) {
							m_szParse.SubString(ext, pos); // ? jpeg = .jpe
						}
						ext.Lower();
						if (ext.CompareTo(".ps") == 0 || ext.CompareTo(".vs") == 0 || ext.CompareTo(".agc") == 0 || ext.CompareTo(".agk") == 0 || ext.CompareTo(".lua") == 0 || ext.CompareTo(".fx") == 0 ) {

							if (ImGui::ImageButton((void*)myTextureID, ImVec2(pref.media_icon_size, pref.media_icon_size), ImVec2(0, 0), ImVec2(1, 1), -5, pref.icon_background_color, ImVec4(1, 1, 1, 1)))
							{
								//No need to preview.
								m_pSelectedFile = m_pFiles;
								bMemTextOnly = true;
							}
							if (ImGui::IsItemHovered()) {
								if (ImGui::IsMouseDoubleClicked(0)) {
									if (ext.CompareTo(".agk") == 0) {
										//Open project.
										usTmp = m_pFiles->m_sPath;
										usTmp.Append("/");
										usTmp.Append(m_pFiles->m_sName);
										LoadProjectFile((char *)usTmp.GetStr());
										//AddRecentFiles(usTmp.GetStr(), NULL);
										isProjectInAssetsBrowser();
									}
									else {
										cProjectItem::sProjectFiles * m_pCurrentFile;
										usTmp = m_pFiles->m_sPath;
										usTmp.Append("/");
										usTmp.Append(m_pFiles->m_sName);
										m_pCurrentFile = CreateNewAGCFile((char *)usTmp.GetStr());
										if (m_pCurrentFile) {
											vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
											m_pCurrentFile->m_bOpenForEdit = true;
											cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
										}
									}
								}
							}

						}
						else {

							bool bImgButRet = false;

							//Uniform and center image.
							float iImgW = agk::GetImageWidth(m_pFiles->iPreview);
							float iImgH = agk::GetImageHeight(m_pFiles->iPreview);
							float icon_size = pref.media_icon_size;
							float icon_ratio;
							if ((icon_size / (float)iImgW) < (icon_size / (float)iImgH))
								icon_ratio = icon_size / (float)iImgW;
							else
								icon_ratio = icon_size / (float)iImgH;

							float bw = (iImgW)*icon_ratio;
							float bh = (iImgH)*icon_ratio;
							

							{
								//Custom imagebutton. with centered image inside button.

								ImGuiWindow* window = ImGui::GetCurrentWindow();
								if (!window->SkipItems) {

									ImGuiContext& g = *GImGui;
									ImGuiStyle& style = g.Style;

									// Default to using texture ID as ID. User can still push string/integer prefixes.
									// We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
									ImGui::PushID((void*)(intptr_t)myTextureID);
									const ImGuiID id = window->GetID("#image");
									ImGui::PopID();

									const ImVec2 padding = style.FramePadding; //ImVec2(0.0, 0.0); //style.FramePadding;
									ImVec2 adder = ImVec2(0.0,0.0);
									if (bw < pref.media_icon_size)
										adder.x = (float)(pref.media_icon_size - bw) * 0.5;
									if (bh < pref.media_icon_size)
										adder.y = (float)(pref.media_icon_size - bh) * 0.5;
									//ImVec2(bw, bh)
									const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(pref.media_icon_size, pref.media_icon_size) + padding * 2 ); //+ padding * 2
									const ImRect image_bb(window->DC.CursorPos + adder + padding, window->DC.CursorPos + adder + padding  + ImVec2(bw, bh));
									ImGui::ItemSize(bb);
									if (ImGui::ItemAdd(bb, id)) {

										bool hovered, held;
										bImgButRet = ImGui::ButtonBehavior(bb, id, &hovered, &held);

										// Render
										const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
										ImGui::RenderNavHighlight(bb, id);
										ImGui::RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
										if (pref.icon_background_color.w > 0.0f)
											window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, ImGui::GetColorU32(pref.icon_background_color));
										window->DrawList->AddImage((ImTextureID) myTextureID, image_bb.Min, image_bb.Max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
									}
								}

							}
//							bImgButRet = ImGui::ImageButton((void*)myTextureID, ImVec2(bw, bh), ImVec2(0, 0), ImVec2(1, 1), -5, pref.icon_background_color, ImVec4(1, 1, 1, 1));


//							bImgButRet = ImGui::ImageButton((void*)myTextureID, ImVec2(pref.media_icon_size, pref.media_icon_size), ImVec2(0, 0), ImVec2(1, 1), -5, pref.icon_background_color, ImVec4(1, 1, 1, 1));

							if (bImgButRet) {
								m_pSelectedFile = m_pFiles;
								bMemTextOnly = false;
							}
							if (ImGui::IsItemHovered()) {
								if (ImGui::IsMouseDoubleClicked(0)) {
									if (ext.CompareTo(".mp3") == 0 || ext.CompareTo(".ogg") == 0 || ext.CompareTo(".wav") == 0) {
										iFirstPreviewAction = 1;
									}
								}
							}
						}

						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("%s", m_pFiles->m_sName.GetStr());
					}
					else {
						uString m_szParse, ext, rest, usTmp;
						m_szParse = m_pFiles->m_sName;
						int pos = m_szParse.RevFind('.');
						if (pos >= 0) {
							m_szParse.SubString(ext, pos + 1);
							m_szParse.SubString(rest, 0, pos);
						}
						ext.Upper();

						ImGuiContext& g = *GImGui;
						ImGuiStyle& style = g.Style;
						ImVec2 padding = style.FramePadding;
						if (ImGui::Button(ext, ImVec2(pref.media_icon_size, pref.media_icon_size) + (padding*2) )) {
							m_pSelectedFile = m_pFiles;
							if(ext.CompareTo("TXT") == 0 || ext.CompareTo("CPP") == 0 || ext.CompareTo("C") == 0 || ext.CompareTo("H") == 0 || ext.CompareTo("HPP") == 0)
								bMemTextOnly = true;
							else
								bMemTextOnly = false;
						}
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("%s", m_pFiles->m_sName.GetStr());
					}

					if (m_pFiles->m_sDropTarget) {
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							m_pFiles->m_dropptr = m_pFiles;
							m_pFiles->iAnimationFrom = 0;
							ImGui::SetDragDropPayload("DND_MODEL_DROP_TARGET", m_pFiles, sizeof(void *));
							ImGui::Text("%s", m_pFiles->m_sName.GetStr());
							pDragDropFile = m_pFiles;
							ImGui::EndDragDropSource();
						}
					}

					if (ImGui::BeginPopupContextItemAGK("abrowsercontextmenu")) //"project context menu"
					{
#ifdef AGK_WINDOWS
						ImGui::Separator();
						if (ImGui::MenuItem("Show In Explorer", "")) {
							uString sTmp = m_pFiles->m_sPath;
							sTmp.ReplaceStr("/", "\\");
							sTmp.Append("\\");
							sTmp.Append(m_pFiles->m_sName);
							ITEMIDLIST *pidl = ILCreateFromPathA(sTmp.GetStr());
							if (pidl) {
								SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
								ILFree(pidl);
							}
						}
						if (ext.CompareTo(".ps") == 0 || ext.CompareTo(".vs") == 0 || ext.CompareTo(".agc") == 0 || ext.CompareTo(".agk") == 0 || ext.CompareTo(".lua") == 0 || ext.CompareTo(".fx") == 0) {
							if (ImGui::MenuItem("Open In Preview", "")) {
								m_pSelectedFile = m_pFiles;
								bMemTextOnly = false;
							}
						}

						if (pCurrentSelectedProject && m_pFiles->m_sFolder.CompareCaseTo(pCurrentSelectedProject->m_sProjectFile) != 0) {
							uString fp = pCurrentSelectedProject->m_sProjectFileFullPath;
							fp.ReplaceStr("\\", "/");
							int pos = fp.RevFindStr("/");
							uString fn = "/";
							if (pos > 0) {
								fp.SubString(fn, 0, pos);
							}
							int find = m_pFiles->m_sPath.FindStr(fn.GetStr(), 1);
							if (find < 0) {

								if (ImGui::MenuItem("Copy to Current Projects Media Folder", "")) {

									m_pSelectedFile = m_pFiles;
									//iFirstPreviewAction = 2;

									cProjectItem * searchProject;
									uString usDestination;
									usDestination = "";
									if (iFirstPreviewAction == 2)
										iFirstPreviewAction = 0;

									if (allProjects) {
										//Check if project is open.
										searchProject = allProjects;
										while (searchProject) {
											if (searchProject->m_sProjectFile.CompareCaseTo(pCurrentSelectedProject->m_sProjectFile) == 0)
											{
												usDestination = searchProject->m_sProjectFileFullPath;
												int pos = usDestination.RevFindStr("/");
												if (pos <= 0) pos = usDestination.RevFindStr("\\");
												if (pos > 0) {
													uString usFinal;
													usDestination.SubString(usFinal, 0, pos + 1);
													usFinal.Append("media/");
													usFinal.Append(m_pSelectedFile->m_sName);

													uString usSource;
													usSource = m_pSelectedFile->m_sPath;
													usSource.Append("/");
													usSource.Append(m_pSelectedFile->m_sName);

													bool oksave = true;
													if (file_exists((char*)usFinal.GetStr())) {
														oksave = overWriteFileBox((char*)usFinal.GetStr());
													}
													if (oksave) {
														cp_copyfile((char*)usSource.GetStr(), (char*)usFinal.GetStr());
														progresscounter = 110;
													}
													//agk::Print(usFinal.GetStr());
													break;
												}

											}
											searchProject = searchProject->m_pNext;
										}
									}


									//bMemTextOnly = false;
								}
							}
						}

						if (!(m_pFiles->bPreviewProcessed && m_pFiles->iPreview > 0)) {

							if (ImGui::MenuItem("Open In Editor", "")) {

								cProjectItem::sProjectFiles * m_pCurrentFile;
								uString sTmp = m_pFiles->m_sPath;
								sTmp.ReplaceStr("/", "\\");
								sTmp.Append("\\");
								sTmp.Append(m_pFiles->m_sName);

								m_pCurrentFile = CreateNewAGCFile((char *)sTmp.GetStr());
								if (m_pCurrentFile) {
									vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
									m_pCurrentFile->m_bOpenForEdit = true;
									cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
								}

							}
						}
						ImGui::Separator();
#else
#ifdef AGK_MACOS
						ImGui::Separator();
						if (ImGui::MenuItem("Show In Finder", "")) {
							uString sTmp = m_pFiles->m_sPath;
							sTmp.ReplaceStr("\\", "/");
							sTmp.Append("/");
							sTmp.Append(m_pFiles->m_sName);

							NSString* path = [NSString stringWithUTF8String : sTmp.GetStr()];
							NSArray *fileURLs = [NSArray arrayWithObjects : [NSURL fileURLWithPath : path], nil];
							[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];

						}
						ImGui::Separator();
#endif
#endif
						if (ImGui::MenuItem("Copy Full Path to Clipboard", "")) {
							uString sTmp = m_pFiles->m_sPath;
							sTmp.ReplaceStr("\\", "/");
							sTmp.Append("/");
							sTmp.Append(m_pFiles->m_sName);
							ImGui::SetClipboardText(sTmp.GetStr());
						}
						if (pCurrentSelectedProject) {
							if (ImGui::MenuItem("Copy Relative Path to Clipboard", "")) {
								uString sTmp = m_pFiles->m_sPath;
								sTmp.ReplaceStr("/", "\\");
								sTmp.Append("\\");
								sTmp.Append(m_pFiles->m_sName);
								ImGui::SetClipboardText(sTmp.GetStr());

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
								else {
									relative.ReplaceStr("media/", "");
									//strcpy(tmpchar, relative.GetStr());
								}
								ImGui::SetClipboardText(relative.GetStr());

							}
						}
						ImGui::EndPopup();

					}

					//				if (ImGui::BeginDragDropTarget())
					//				{
					//					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
					//					{
					//						IM_ASSERT(payload->DataSize == sizeof(int));
					//						int payload_n = *(const int*)payload->Data;
					//						if (mode == Mode_Copy)
					//						{
					//							names[n] = names[payload_n];
					//						}
					//					}
					//					ImGui::EndDragDropTarget();
					//				}

					int iTextWidth = ImGui::CalcTextSize(m_pFiles->m_sName.GetStr()).x;

					if (iTextWidth < (8 + pref.media_icon_size))
						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + ((3 + (pref.media_icon_size*0.5)) - (iTextWidth*0.5)), ImGui::GetCursorPosY()));

//					if (iTextWidth < 70)
//						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (35 - (iTextWidth*0.5)), ImGui::GetCursorPosY()));

					ImGui::TextWrapped("%s", m_pFiles->m_sName.GetStr());

					ImGui::PopID();

					ImGui::NextColumn();
					//ImGui::PushID(n);
					//ImGui::Button("Box", button_sz);
					//float last_button_x2 = ImGui::GetItemRectMax().x;
					//float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
					//if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
					//	ImGui::SameLine();
					//ImGui::PopID();

					preview_count++;
				}
				m_pFiles = m_pFiles->m_pNext;
			}
		}
		ImGui::SetWindowFontScale(1.00);
		ImGui::Columns(1);
		ImGui::EndChild();
	}



	//Start by generating ALL fast preview images.
	if (m_pSelectedFolder && m_pSelectedFolder->m_pFirstFile && m_pSelectedFolder->m_pFirstFile->m_pNext) {
		cFolderItem::sFolderFiles * m_pFiles = m_pSelectedFolder->m_pFirstFile->m_pNext;
		while (m_pFiles) {

			if (m_pFiles->bPreviewProcessed == false && Filter.PassFilter(m_pFiles->m_sName) ) {

				uString m_szParse, ext, rest, usTmp;
				m_szParse = m_pFiles->m_sName;
				int pos = m_szParse.RevFind('.');
				if (pos >= 0) {
					m_szParse.SubString(ext, pos); // ? jpeg = .jpe
					m_szParse.SubString(rest, 0, pos);
				}
				ext.Lower();
				if (strlen(ext) > 5)
					ext = "NOSUPPORT";

				if (ext.CompareTo(".fpe") == 0) {
					//GameGuru FPE files , load .bmp as preview.
					usTmp = "raw:";
					usTmp.Append(m_pSelectedFolder->m_sFolderFullPath);
					usTmp.Append("/");
					usTmp.Append(rest);
					usTmp.Append(".bmp");
#ifdef AGK_WINDOWS
#ifdef NASSUPPORT
					//NAS support.
					usTmp.ReplaceStr("/", "\\");
#endif
#endif

					m_pFiles->iPreview = agk::LoadImage(usTmp);
					m_pFiles->bPreviewProcessed = true;
					m_pFiles->m_sDropTarget = "DND_MODEL_DROP_TARGET";
				}
				//".fpe,.obj,.x,.fbx,.3ds,.dae,.b3d,.3d,.lwo,.ago"
				if ((ext.CompareTo(".x") == 0 || ext.CompareTo(".dbo") == 0 || ext.CompareTo(".fbx") == 0)) {
					//usTmp = "raw:";
					//usTmp.Append(m_pSelectedFolder->m_sFolderFullPath);
					//usTmp.Append("/");
					//usTmp.Append(rest);
					//usTmp.Append(".bmp");
					//if (agk::GetFileExists(usTmp.GetStr())) {
					//	m_pFiles->iPreview = agk::LoadImage(usTmp);
					//}
					//else {
						m_pFiles->iPreview = object3d_icon;
					//}
					m_pFiles->bPreviewProcessed = true;
					m_pFiles->m_sDropTarget = "DND_MODEL_DROP_TARGET";
				}
				if ((ext.CompareTo(".obj") == 0 || ext.CompareTo(".3ds") == 0 || ext.CompareTo(".dae") == 0 || ext.CompareTo(".b3d") == 0 || ext.CompareTo(".3d") == 0 || ext.CompareTo(".lwo") == 0 || ext.CompareTo(".ago") == 0)) {
					m_pFiles->iPreview = object3d_icon;
					m_pFiles->bPreviewProcessed = true;
					m_pFiles->m_sDropTarget = "DND_MODEL_DROP_TARGET";
				}

				if ((ext.CompareTo(".wav") == 0 || ext.CompareTo(".ogg") == 0 || ext.CompareTo(".mp3") == 0)) {
					m_pFiles->iPreview = playmusic_icon;
					m_pFiles->bPreviewProcessed = true;
					m_pFiles->m_sDropTarget = "DND_MUSIC_DROP_TARGET";
				}

				if (ext.CompareTo(".ps") == 0 || ext.CompareTo(".vs") == 0) {
					m_pFiles->iPreview = codeglsl_icon;
					m_pFiles->bPreviewProcessed = true;
					m_pFiles->m_sDropTarget = "DND_CODE_DROP_TARGET";
				}

				if (ext.CompareTo(".agc") == 0 || ext.CompareTo(".agk") == 0) {

					if (ext.CompareTo(".agk") == 0)
						m_pFiles->iPreview = agkfile_icon;
					else
						m_pFiles->iPreview = agcfile_icon;
					m_pFiles->bPreviewProcessed = true;
					m_pFiles->m_sDropTarget = "DND_CODE_DROP_TARGET";
				}
				if (ext.CompareTo(".lua") == 0 ) {

					m_pFiles->iPreview = luafile_icon;
					m_pFiles->bPreviewProcessed = true;
					m_pFiles->m_sDropTarget = "DND_CODE_DROP_TARGET";
				}
				if (ext.CompareTo(".fx") == 0) {
					m_pFiles->iPreview = directx_icon;
					m_pFiles->bPreviewProcessed = true;
					m_pFiles->m_sDropTarget = "DND_CODE_DROP_TARGET";
				}
			}

			m_pFiles = m_pFiles->m_pNext;
		}
	}

	//Only make 1 preview per sync.

	int iMaxPreviewPerSync = 1;
	if (m_pSelectedFolder && m_pSelectedFolder->m_pFirstFile && m_pSelectedFolder->m_pFirstFile->m_pNext) {
		cFolderItem::sFolderFiles * m_pFiles = m_pSelectedFolder->m_pFirstFile->m_pNext;
		while (m_pFiles) {
			
			if (m_pFiles->bPreviewProcessed == false) {
				if (Filter.PassFilter(m_pFiles->m_sName)) {

					uString m_szParse, ext, rest, usTmp;
					m_szParse = m_pFiles->m_sName;
					int pos = m_szParse.RevFind('.');
					if (pos >= 0) {
						m_szParse.SubString(ext, pos); // ? jpeg = .jpe
						m_szParse.SubString(rest, 0, pos);
					}
					ext.Lower();
					if (strlen(ext) > 5)
						ext = "NOSUPPORT";

					//|| ext.CompareTo(".bmp") == 0 , dont process bmp yet.
					if (( ext.CompareTo(".png") == 0 || ext.CompareTo(".jpg") == 0 || ext.CompareTo(".jpeg") == 0  || ext.CompareTo(".gif") == 0)) {
						
						usTmp = "raw:";
						usTmp.Append(m_pSelectedFolder->m_sFolderFullPath);
						usTmp.Append("/");
						usTmp.Append(rest);
						usTmp.Append(ext);

#ifdef AGK_WINDOWS
#ifdef NASSUPPORT
						//NAS support.
						usTmp.ReplaceStr("/", "\\");
#endif
#endif

#ifdef AGK_LINUX
						unsigned int filewh = 1024;
						int filew = 1024, fileh = 1024;
						bool ignore_pic = false;
#else
						unsigned int filewh = cImage::GetImageSizeFromFile(usTmp.GetStr());
						int filew = filewh >> 16, fileh = filewh & 0xFFFF;
						bool ignore_pic = false;
#endif
						if (filew > 8192 || fileh > 8192) {
							//Sometimes agk can crash if using images > 8192 ?
							ignore_pic = true;
						}
						if (!ignore_pic) {
							if (ext.CompareTo(".gif") != 0)
								m_pFiles->iPreview = agk::LoadImage(usTmp); //LoadImageResized(usTmp,0.5,0.5,0)
							else
								m_pFiles->iPreview = agk::LoadImageResized(usTmp, 1.0, 1.0, 0);

							float iImgW = agk::GetImageWidth(m_pFiles->iPreview);
							float iImgH = agk::GetImageHeight(m_pFiles->iPreview);
							if (iImgW > pref.media_icon_size || iImgH > pref.media_icon_size) {
								if (ext.CompareTo(".gif") != 0) {
									//Uniform and center image.
									float icon_size = 128;
									float icon_ratio;
									if ((icon_size / (float)iImgW) < (icon_size / (float)iImgH))
										icon_ratio = icon_size / (float)iImgW;
									else
										icon_ratio = icon_size / (float)iImgH;

									float bw = (iImgW)*icon_ratio;
									float bh = (iImgH)*icon_ratio;

									agk::ResizeImage(m_pFiles->iPreview, bw, bh); //Calculate with org ratio.
								}
							}
							//m_pFiles->m_sName.Append("(");
							//m_pFiles->m_sName.Append( agk::Str(m_pFiles->iPreview) );
							//m_pFiles->m_sName.Append(")");
						}
						m_pFiles->bPreviewProcessed = true;
						//PE: Real slow , perhaps make a cache. for now only 1 per sync.
						iMaxPreviewPerSync -= 1;
						m_pFiles->m_sDropTarget = "DND_TEXTURE_DROP_TARGET";
					}

					//NO SUPPORT YET!

					if ((ext.CompareTo(".dds") == 0 || ext.CompareTo("NOSUPPORT") == 0 )) {
						usTmp = "raw:";
						usTmp.Append(m_pSelectedFolder->m_sFolderFullPath);
						usTmp.Append("/");
						usTmp.Append(rest);
						usTmp.Append(ext);
#ifdef AGK_WINDOWS
#ifdef NASSUPPORT
						//NAS support.
						usTmp.ReplaceStr("/", "\\");
#endif
#endif

						//Use nconvert. or new dds loader.
						//m_pFiles->iPreview = agk::LoadImage(usTmp); //LoadImageResized(usTmp,0.5,0.5,0)
						//agk::ResizeImage(m_pFiles->iPreview, pref.media_icon_size, pref.media_icon_size); //Calculate with org ratio.
						//m_pFiles->m_sName.Append("(");
						//m_pFiles->m_sName.Append(agk::Str(m_pFiles->iPreview));
						//m_pFiles->m_sName.Append(")");
						m_pFiles->bPreviewProcessed = true;
						//PE: Real slow , perhaps make a cache. for now only 1 per sync.
						//iMaxPreviewPerSync -= 1;
						m_pFiles->m_sDropTarget = "";
						iMaxPreviewPerSync -= 1;
					}

					if (iMaxPreviewPerSync <= 0) break;

				}
			}
			m_pFiles = m_pFiles->m_pNext;
		}
	}

	return m_pSelectedFolder;
}


void setVisibleProjectsInAssetBrowser(void)
{
	cFolderItem *m_psearchFolder;
	firstProjects = allProjects;

	//Set all invisible.
	if (m_pStartFolder && m_pStartFolder->m_pNext) {
		m_psearchFolder = m_pStartFolder->m_pNext;
		while (m_psearchFolder) {
			m_psearchFolder->visible = false;
			if (m_psearchFolder->alwaysvisible == true)
				m_psearchFolder->visible = true;
			m_psearchFolder = m_psearchFolder->m_pNext;
		}
	}

	//Mark active projects.
	while (firstProjects) {
		uString sSearch;
		sSearch = "-> "; //Project Media"
		sSearch.Append(firstProjects->m_sProjectFile.GetStr());
		sSearch.Append(" Project Media");

		if (m_pStartFolder && m_pStartFolder->m_pNext) {
			m_psearchFolder = m_pStartFolder->m_pNext;
			while (m_psearchFolder) {
				if (m_psearchFolder->alwaysvisible == false) {
					if (m_psearchFolder->m_sFolder == sSearch) {
						m_psearchFolder->visible = true;
						//break;
					}
				}
				else {
					m_psearchFolder->visible = true;
				}
				m_psearchFolder = m_psearchFolder->m_pNext;
			}
		}
		firstProjects = firstProjects->m_pNext;
	}
}
//char * project, char * path
void isProjectInAssetsBrowser( void)
{
	cFolderItem *m_psearchFolder;

	setVisibleProjectsInAssetBrowser();

	firstProjects = allProjects;
	while (firstProjects) {

		uString sMediaPath;
		sMediaPath = firstProjects->m_sProjectFileFullPath;

		//Check if media folder is already in assets browser.
		uString sSearch;
		sSearch = "-> "; //Project Media"
		sSearch.Append(firstProjects->m_sProjectFile.GetStr());
		sSearch.Append(" Project Media");
		m_psearchFolder = NULL;
		if (m_pStartFolder && m_pStartFolder->m_pNext) {
			m_psearchFolder = m_pStartFolder->m_pNext;
			while (m_psearchFolder) {
				if (m_psearchFolder->m_sFolder == sSearch)
					break;
				m_psearchFolder = m_psearchFolder->m_pNext;
			}
		}
		if (m_psearchFolder) {
			if(!firstProjects->m_pMediaFolder)
				firstProjects->m_pMediaFolder = m_psearchFolder;
			firstProjects = firstProjects->m_pNext;
			continue; // found.
		}

		//Not found create.
		if (m_pStartFolder && m_pStartFolder->m_pNext) {
			m_psearchFolder = m_pStartFolder->m_pNext;
			while (m_psearchFolder) {

				if (!m_psearchFolder->m_pNext) { //Last.

					uString m_szParse, m_szFileName, m_szPathName;
					m_szParse = sMediaPath;
					m_szParse.ReplaceStr("\\\\", "/");
					m_szParse.ReplaceStr("\\", "/");
					int pos = m_szParse.RevFind('/');
					if (pos >= 0) {
						m_szParse.SubString(m_szFileName, pos + 1);
						m_szParse.SubString(m_szPathName, 0, pos + 1);
						m_szPathName.Append("media");
						cFolderItem *pNewItem = new cFolderItem();
						pNewItem->m_sFolder.SetStr(sSearch);
						strcpy(pNewItem->cfolder, pNewItem->m_sFolder.GetStr());

						//PE: We need project->media folder here.
						pNewItem->m_sFolderFullPath.SetStr(m_szPathName);
						pNewItem->m_pSubFolder = NULL;
						pNewItem->count = ++iTotalFolders;
						pNewItem->m_pNext = NULL;
						pNewItem->m_pFirstFile = NULL;
						pNewItem->visible = true;
						pNewItem->alwaysvisible = false;
						pNewItem->bIsCustomFolder = false;
						pNewItem->deletethisentry = false;

						m_psearchFolder->m_pNext = pNewItem;

						pNewItem = new cFolderItem();
						pNewItem->m_sFolder = "...";
						strcpy(pNewItem->cfolder, pNewItem->m_sFolder.GetStr());
						pNewItem->count = 0;
						pNewItem->m_pNext = NULL;
						pNewItem->m_pSubFolder = NULL;
						pNewItem->m_pFirstFile = NULL;
						pNewItem->bIsCustomFolder = false;

						m_psearchFolder->m_pNext->m_pSubFolder = pNewItem;
						agk::SetFolder("");
						PleaseWait();
						getalldirs(m_psearchFolder->m_pNext->m_pSubFolder, m_psearchFolder->m_pNext, (char *) m_szPathName.GetStr());
						PleaseWaitEnd();
//						getalldirs(m_psearchFolder->m_pNext->m_pSubFolder, NULL, (char *)m_szPathName.GetStr());
						firstProjects->m_pMediaFolder = m_psearchFolder->m_pNext;
					}
					setVisibleProjectsInAssetBrowser();
					return;
				}
				m_psearchFolder = m_psearchFolder->m_pNext;
			}
		}

	firstProjects = firstProjects->m_pNext;
	}

}

//######################################
//#### AddToAssetsBrowser           ####
//#### Add folder to assets browser ####
//######################################

void AddToAssetsBrowser(char *selectedfolder )
{
	//Add new folder to assets browser.
	if (selectedfolder && strlen(selectedfolder) > 0 && m_pStartFolder && m_pStartFolder->m_pNext) {
		cFolderItem *m_psearchFolder;
		m_psearchFolder = m_pStartFolder->m_pNext;

		uString m_szPathName, szShortName;
		m_szPathName = selectedfolder;
		//m_szPathName.ReplaceStr("\\\\", "/"); //26-04-2019: Needed ?
		m_szPathName.ReplaceStr("\\", "/");

		while (m_psearchFolder) {

			if (m_psearchFolder->m_sFolderFullPath.CompareTo(m_szPathName) == 0) {
				//Already added. ignore. just make it visible.
				m_psearchFolder->alwaysvisible = true;
				m_psearchFolder->visible = true;
				m_psearchFolder->deletethisentry = false;
				//m_psearchFolder->visible
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
				//					getalldirs(m_psearchFolder->m_pNext->m_pSubFolder, NULL, (char *)m_szPathName.GetStr());
				PleaseWait();
				getalldirs(m_psearchFolder->m_pNext->m_pSubFolder, m_psearchFolder->m_pNext, (char *)m_szPathName.GetStr());
				PleaseWaitEnd();
				if (iCustomFolders < 10) {
					strcpy(customFolders[iCustomFolders++], m_szPathName.GetStr());
				}

				setVisibleProjectsInAssetBrowser();
				return;
			}
			m_psearchFolder = m_psearchFolder->m_pNext;
		}
	}

}
static bool gotAssetFolders = false;
static void displayAssetsBrowser(const char* windowsname,char *path)
{
	char *oldpath;

	if (!gotAssetFolders) {

		oldpath = agk::GetReadPath();
		agk::SetRawWritePath(path);
		agk::SetFolder("");


		m_pCurrentFolder = new cFolderItem();
		m_pCurrentFolder->m_sFolder = "...";
		strcpy(m_pCurrentFolder->cfolder, m_pCurrentFolder->m_sFolder.GetStr());
		m_pCurrentFolder->count = 0;
		m_pCurrentFolder->m_pSubFolder = NULL;
		m_pStartFolder = m_pCurrentFolder;
		m_pStartFolder->m_pNext = NULL;

		// Add additional folder for DLC.
		cFolderItem *pNewItem;

		pNewItem = new cFolderItem();
		pNewItem->m_sFolder.SetStr("-> Classic DLC Media");
		strcpy(pNewItem->cfolder, pNewItem->m_sFolder.GetStr());

		pNewItem->m_sFolderFullPath = cSteamFolder;
		pNewItem->m_sFolderFullPath.Append("DLC");

		pNewItem->m_pSubFolder = NULL;
		pNewItem->visible = true;
		pNewItem->alwaysvisible = true;
		pNewItem->deletethisentry = false;
		pNewItem->bIsCustomFolder = false;
		pNewItem->m_pFirstFile = NULL;
		pNewItem->count = ++iTotalFolders;
		m_pCurrentFolder->m_pNext = pNewItem;
		m_pClassicDLCFolder = m_pCurrentFolder->m_pNext;

		pNewItem = new cFolderItem();
		pNewItem->m_sFolder = "...";
		strcpy(pNewItem->cfolder, pNewItem->m_sFolder.GetStr());
		pNewItem->count = 0;
		pNewItem->m_pNext = NULL;
		pNewItem->m_pFirstFile = NULL;
		pNewItem->m_pSubFolder = NULL;
		m_pCurrentFolder->m_pNext->m_pSubFolder = pNewItem;

		if (pref.bLoadClassicDLConStartup)
		{


			//PE: Get all files and folder , including subfolders/files.
			//PE: Somehow make this faster , or only do some at each sync.
			//getalldirs(m_pCurrentFolder->m_pNext->m_pSubFolder, NULL , (char *) m_pCurrentFolder->m_pNext->m_sFolderFullPath.GetStr());
			PleaseWait();
			getalldirs(m_pCurrentFolder->m_pNext->m_pSubFolder, m_pCurrentFolder->m_pNext, (char *)m_pCurrentFolder->m_pNext->m_sFolderFullPath.GetStr());
			PleaseWaitEnd();

		}
		else {
			m_pCurrentFolder->m_pNext->m_pSubFolder->visible = false;
			m_pCurrentFolder->m_pNext->m_pSubFolder->alwaysvisible = false;
			m_pCurrentFolder->m_pNext->visible = false;
			m_pCurrentFolder->m_pNext->alwaysvisible = false;
		}
		if (m_pStartFolder && m_pStartFolder->m_pNext) m_pCurrentFolder = m_pStartFolder->m_pNext;

		agk::SetRawWritePath(oldpath);
		delete oldpath;

		gotAssetFolders = true;
	}

	ImGuiWindow *pwin;
	pwin = ImGui::GetCurrentWindow();
	int i = pwin->Size.x - pwin->ScrollbarSizes.x; //SizeContents.x;
	ImGui::Spacing();
	ImGui::SameLine(i - 18.0f);

	if (ImGui::Button("...")) {
		char *selectedfolder;
		selectedfolder = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN|NOC_FILE_DIALOG_DIR, NULL, NULL, NULL);

		AddToAssetsBrowser(selectedfolder);
/*
		//Add new folder to assets browser.
		if (selectedfolder && strlen(selectedfolder) > 0 && m_pStartFolder && m_pStartFolder->m_pNext) {
			cFolderItem *m_psearchFolder;
			m_psearchFolder = m_pStartFolder->m_pNext;

			uString m_szPathName, szShortName;
			m_szPathName = selectedfolder;
			//m_szPathName.ReplaceStr("\\\\", "/"); //26-04-2019: Needed ?
			m_szPathName.ReplaceStr("\\", "/");

			while (m_psearchFolder) {

				if( m_psearchFolder->m_sFolderFullPath.CompareTo(m_szPathName) == 0 ) {
					//Already added. ignore. just make it visible.
					m_psearchFolder->alwaysvisible = true;
					m_psearchFolder->visible = true;
					//m_psearchFolder->visible
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
//					getalldirs(m_psearchFolder->m_pNext->m_pSubFolder, NULL, (char *)m_szPathName.GetStr());
					PleaseWait();
					getalldirs(m_psearchFolder->m_pNext->m_pSubFolder, m_psearchFolder->m_pNext , (char *)m_szPathName.GetStr());
					PleaseWaitEnd();
					if (iCustomFolders < 10) {
						strcpy(customFolders[iCustomFolders++], m_szPathName.GetStr());
					}

					setVisibleProjectsInAssetBrowser();
					return;
				}
				m_psearchFolder = m_psearchFolder->m_pNext;
			}
		}
*/

	}
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add Folder");

	//##################################################
	//#### Display tree view of folders/subfolders. ####
	//##################################################

//	ImGuiTextFilter     Filter;
//	Filter.Draw("Filter", -100); //ImGui::GetWindowSize().x-22.
//	if(Filter.IsActive() && ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsMouseClicked(0) )
//		ImGui::SetKeyboardFocusHere(-1);

	//ImGuiTreeNodeFlags node_flags;
//	if (Filter.IsActive()) {
//		if (m_pStartFolder->m_pNext) m_pCurrentFolder = m_pStartFolder->m_pNext;
//		GetTreePopFilter(m_pCurrentFolder, Filter);
//	}
//	else
//	{
		if (m_pStartFolder && m_pStartFolder->m_pNext) m_pCurrentFolder = m_pStartFolder->m_pNext;
		 GetTreePop(m_pCurrentFolder);
//	}

		 ImGui::Text("\n");


}

//#####################
//#### TEXT EDITOR ####
//#####################

void vTextEditorInit(TextEditor * m_editor, char *filename)
{
	TextEditor::LanguageDefinition lang;
	
	uString ext,tmp;
	ext = filename;
	ext.Lower();
	int pos = ext.RevFindStr(".");
	if (pos > 0) {
		ext.SubString(tmp, pos);
		ext = tmp;
	}
	
	if( ext.CompareTo(".cpp") == 0 )
		lang = TextEditor::LanguageDefinition::CPlusPlus();
	else if (ext.CompareTo(".hpp") == 0)
		lang = TextEditor::LanguageDefinition::CPlusPlus();
	else if (ext.CompareTo(".fx") == 0)
		lang = TextEditor::LanguageDefinition::HLSL();
	else if (ext.CompareTo(".ps") == 0)
		lang = TextEditor::LanguageDefinition::GLSL();
	else if (ext.CompareTo(".vs") == 0)
		lang = TextEditor::LanguageDefinition::GLSL();
	else if (ext.CompareTo(".c") == 0)
		lang = TextEditor::LanguageDefinition::C();
	else if (ext.CompareTo(".h") == 0)
		lang = TextEditor::LanguageDefinition::C();
	else if (ext.CompareTo(".sql") == 0)
		lang = TextEditor::LanguageDefinition::SQL();
	else if (ext.CompareTo(".lua") == 0)
		lang = TextEditor::LanguageDefinition::Lua();
	else {
		lang = TextEditor::LanguageDefinition::AGK();
		m_editor->isAGKsource = true;
	}

	//first run.
	m_editor->firstrun = true;
	strcpy(m_editor->cTypeName, "");
	m_editor->withinString = false;
	m_editor->withinTypeDefination = false;
	m_editor->bEditorVisible = true;

	for (int a = 0; a < MAXPRECHECKLINES; a++) {
		m_editor->m_bCheckLine[a] = false;
		m_editor->m_bFirstCheck[a] = false;
		m_editor->m_bLineVisible[a] = true;
		m_editor->m_bFoldingActive[a] = false;
		m_editor->m_bFoldingPossible[a] = false;
		m_editor->m_bFoldingStartWord[a] = false;
		m_editor->m_bFoldingEndWord[a] = false;
		m_editor->m_iNewLineNo[a] = 0;
		m_editor->m_iHiddenLineNo[a] = 0;
	}
	m_editor->SetLanguageDefinition(lang);

	strcpy(m_editor->cnSearch, "");
	strcpy(m_editor->cnSearchReplace, "");
	strcpy(m_editor->cTypeName, "");
	
	m_editor->m_RenderCount = 0;
	{
		std::ifstream t(filename);
		if (t.good())
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			m_editor->SetText(str);
		}
		else
		{
			//Just create a empty.
			m_editor->SetText("");
		}
	}

}


//TextEditor * m_foundeditor;
//m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)szFile.GetStr());
cProjectItem::sProjectFiles * FindFileFromEditorMatchSource(TextEditor * m_FindActiveEditor)
{
	//pri on m_ActiveEditor
	//Then search all in current project.
	//pCurrentSelectedProject

	cProjectItem::sProjectFiles * m_pCurrentFile;
	cProjectItem::sProjectFiles * m_pSecondFile;

	//scan open files in current project.
	if (pCurrentSelectedProject) {
		//Search files in current project.
		m_pCurrentFile = pCurrentSelectedProject->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_editor->bEditorVisible) {
				if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
					return(m_pCurrentFile);
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	//Next pri on already open files.
	firstProjects = allProjects;
	while (firstProjects) {
		m_pCurrentFile = firstProjects->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_editor->bEditorVisible) {
				if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
					return(m_pCurrentFile);
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
		firstProjects = firstProjects->m_pNext;
	}


	//scan everyting in current project.
	if (pCurrentSelectedProject) {
		//Search files in current project.
		m_pCurrentFile = pCurrentSelectedProject->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_editor == m_FindActiveEditor) {

				//Check if we got the same file but open in another tab.
				m_pSecondFile = NULL;
				TextEditor * m_foundeditor;
				m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)m_pCurrentFile->m_sFullPath.GetStr());
				if (m_foundeditor) {
					m_pSecondFile = FindFileFromEditor(m_foundeditor);
				}
				if (m_pSecondFile)
					return(m_pSecondFile);

				return(m_pCurrentFile);
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	//Search files with no project, open only.
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_editor->bEditorVisible) {
				if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
					return(m_pCurrentFile);
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}


	//Search files with no project.
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_editor == m_FindActiveEditor) {

				//Check if we got the same file but open in another tab.
				m_pSecondFile = NULL;
				TextEditor * m_foundeditor;
				m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)m_pCurrentFile->m_sFullPath.GetStr());
				if (m_foundeditor) {
					m_pSecondFile = FindFileFromEditor(m_foundeditor);
				}
				if (m_pSecondFile)
					return(m_pSecondFile);

				return(m_pCurrentFile);
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}


	//Search files with projects.
	firstProjects = allProjects;
	while (firstProjects) {
		m_pCurrentFile = firstProjects->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
				//Check if we got the same file but open in another tab.
				m_pSecondFile = NULL;
				TextEditor * m_foundeditor;
				m_foundeditor = FindEditorFromName((char*)pCurrentSelectedProject->m_sProjectFile.GetStr(), (char*)m_pCurrentFile->m_sFullPath.GetStr());
				if (m_foundeditor) {
					m_pSecondFile = FindFileFromEditor(m_foundeditor);
				}
				if (m_pSecondFile)
					return(m_pSecondFile);

				return(m_pCurrentFile);
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}

		firstProjects = firstProjects->m_pNext;
	}
	return NULL;
}


cProjectItem::sProjectFiles * FindFileFromEditor(TextEditor * m_FindActiveEditor)
{
	//pri on m_ActiveEditor
	//Then search all in current project.
	//pCurrentSelectedProject

	cProjectItem::sProjectFiles * m_pCurrentFile;

	//scan open files in current project.
	if (pCurrentSelectedProject) {
		//Search files in current project.
		m_pCurrentFile = pCurrentSelectedProject->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_editor->bEditorVisible) {
				if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
					return(m_pCurrentFile);
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	//Next pri on already open files.
	firstProjects = allProjects;
	while (firstProjects) {
		m_pCurrentFile = firstProjects->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_editor->bEditorVisible) {
				if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
					return(m_pCurrentFile);
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
		firstProjects = firstProjects->m_pNext;
	}


	//scan everyting in current project.
	if (pCurrentSelectedProject) {
		//Search files in current project.
		m_pCurrentFile = pCurrentSelectedProject->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
				return(m_pCurrentFile);
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	//Search files with no project, open only.
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_editor->bEditorVisible) {
				if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
					return(m_pCurrentFile);
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}


	//Search files with no project.
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
				return(m_pCurrentFile);
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}


	//Search files with projects.
	firstProjects = allProjects;
	while (firstProjects) {
		m_pCurrentFile = firstProjects->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_editor == m_FindActiveEditor) {
				return(m_pCurrentFile);
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}

		firstProjects = firstProjects->m_pNext;
	}
	return NULL;
}

void ClearExecuteLine(void)
{
	cProjectItem::sProjectFiles * m_pCurrentFile;

	//Search files with no project.
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_bOpenForEdit) {
				m_pCurrentFile->m_editor->executeLine = -1;
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	firstProjects = allProjects;
	while (firstProjects) {
		m_pCurrentFile = firstProjects->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if (m_pCurrentFile->m_bOpenForEdit) {
				m_pCurrentFile->m_editor->executeLine = -1;
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
		firstProjects = firstProjects->m_pNext;
	}
}

void ClearAllEditorMarkers(void)
{
	cProjectItem::sProjectFiles * m_pCurrentFile;

	//Search files with no project.
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_bOpenForEdit) {
				m_pCurrentFile->m_editor->editor_markers.clear();
				m_pCurrentFile->m_editor->SetErrorMarkers(m_pCurrentFile->m_editor->editor_markers);
				m_pCurrentFile->m_editor->executeLine = -1;
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	firstProjects = allProjects;
	while (firstProjects) {
		m_pCurrentFile = firstProjects->m_pFirstFile;
		while (m_pCurrentFile)
		{
			if(m_pCurrentFile->m_bOpenForEdit) {
				m_pCurrentFile->m_editor->editor_markers.clear();
				m_pCurrentFile->m_editor->SetErrorMarkers(m_pCurrentFile->m_editor->editor_markers);
				m_pCurrentFile->m_editor->executeLine = -1;
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
		firstProjects = firstProjects->m_pNext;
	}
}

TextEditor * FindEditorFromWindowName(char *windowname)
{
	cProjectItem::sProjectFiles * m_pCurrentFile;
	firstProjects = allProjects;
	uString projectpath = "";
	while (firstProjects) {

			projectpath = firstProjects->m_sProjectFileFullPath;
			cProjectItem::sProjectFiles * m_pCurrentFile;
			m_pCurrentFile = firstProjects->m_pFirstFile;
			while (m_pCurrentFile)
			{
				if (m_pCurrentFile->m_sEditName.GetLength() > 0) {
#ifdef AGK_WINDOWS
					if (stricmp(m_pCurrentFile->m_sEditName.GetStr(), windowname) == 0) {
#else
					if (strcasecmp(m_pCurrentFile->m_sEditName.GetStr(), windowname) == 0) {
#endif
						return(m_pCurrentFile->m_editor);
					}
				}
				m_pCurrentFile = m_pCurrentFile->m_pNext;
			}
		firstProjects = firstProjects->m_pNext;
	}


	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_sEditName.GetLength() > 0) {
#ifdef AGK_WINDOWS
				if (stricmp(m_pCurrentFile->m_sEditName.GetStr(), windowname) == 0) {
#else
				if (strcasecmp(m_pCurrentFile->m_sEditName.GetStr(), windowname) == 0) {
#endif
					return(m_pCurrentFile->m_editor);
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}
	return NULL;
}


TextEditor * FindEditorFromName(char *projectname, char *sourcefile)
{
	//New priority, open files first.

	//Search for open files.

	//resolve full path to file and also search for that.
	//m_sFullPath = {m_pData=0x13bd7460 "D:/AGK2/testprojects/Terrain/code/buttons.agc" m_iNumChars=45 m_iLength=45 ...}
	char fullpath[MAX_PATH];
	
	cProjectItem::sProjectFiles * m_pCurrentFile;
	firstProjects = allProjects;
	uString projectpath = "";
	char curDir[MAX_PATH];
	_getcwd(&curDir[0], MAX_PATH);

	uString m_szParse, m_szFileName, m_szPathName;
	while (firstProjects) {

		if (strcmp(firstProjects->m_sProjectFile.GetStr(), projectname) == 0) {

			m_szPathName = "";
			m_szParse = firstProjects->m_sProjectFileFullPath;
			m_szParse.ReplaceStr("\\\\", "//"); //To allow network drive using //
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
			int ret = _chdir(m_szPathName.GetStr());
			cp_realpath(sourcefile, &fullpath[0], false);
			m_szFileName = fullpath;
			m_szFileName.ReplaceStr("\\", "/");

			projectpath = firstProjects->m_sProjectFileFullPath;
			cProjectItem::sProjectFiles * m_pCurrentFile;
			m_pCurrentFile = firstProjects->m_pFirstFile;

			while (m_pCurrentFile)
			{

				if (m_pCurrentFile->m_editor->bEditorVisible) {
					cp_realpath((char *)m_pCurrentFile->m_sFullPath.GetStr(), &fullpath[0], false);
					uString cFileName = fullpath;
					cFileName.ReplaceStr("\\", "/");

#ifdef AGK_WINDOWS
					if (stricmp(m_pCurrentFile->m_sName.GetStr(), sourcefile) == 0) {
#else
					if (strcasecmp(m_pCurrentFile->m_sName.GetStr(), sourcefile) == 0) {
#endif
						//if (strcmp(m_pCurrentFile->m_sName.GetStr(), sourcefile) == 0) {
						ret = _chdir(curDir);
						return(m_pCurrentFile->m_editor);
					}

#ifdef AGK_WINDOWS
					if (stricmp(cFileName.GetStr(), m_szFileName.GetStr()) == 0) {
#else
					if (strcasecmp(cFileName.GetStr(), m_szFileName.GetStr()) == 0) {
#endif
						ret = _chdir(curDir);
						return(m_pCurrentFile->m_editor);
					}

				}
				m_pCurrentFile = m_pCurrentFile->m_pNext;
			}
		}
		firstProjects = firstProjects->m_pNext;
	}

	firstProjects = allProjects;
	while (firstProjects) {

		if (!strcmp(firstProjects->m_sProjectFile.GetStr(), projectname) == 0) {
			//Check if full name is found in another project, and open.

			cProjectItem::sProjectFiles * m_pCurrentFile;
			m_pCurrentFile = firstProjects->m_pFirstFile;

			while (m_pCurrentFile)
			{
				if (m_pCurrentFile->m_editor->bEditorVisible) {

					cp_realpath((char *)m_pCurrentFile->m_sFullPath.GetStr(), &fullpath[0], false);
					uString cFileName = fullpath;
					cFileName.ReplaceStr("\\", "/");

#ifdef AGK_WINDOWS
					if (stricmp(cFileName.GetStr(), m_szFileName.GetStr()) == 0) {
#else
					if (strcasecmp(cFileName.GetStr(), m_szFileName.GetStr()) == 0) {
#endif
						_chdir(curDir);
						return(m_pCurrentFile->m_editor);
					}

				}
				m_pCurrentFile = m_pCurrentFile->m_pNext;
			}

		}
		firstProjects = firstProjects->m_pNext;
	}

	_chdir(curDir);

	//sourcefile = 0x1531b168 "media/particle/particle.agc"
	//Search files outside projects.
	projectpath.ReplaceStr("\\", "/");
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (m_pCurrentFile->m_editor->bEditorVisible) {
				if (strcmp(m_pCurrentFile->m_sName.GetStr(), sourcefile) == 0) {
					return(m_pCurrentFile->m_editor);
				}

				if (strstr(sourcefile, "/") != NULL) {
					uString fname = m_pCurrentFile->m_sFullPath;
					fname.ReplaceStr("\\", "/");
					if (fname.FindStr(sourcefile) >= 0) {
						return(m_pCurrentFile->m_editor);
					}
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}


	//search for closed files.
	//cProjectItem::sProjectFiles * m_pCurrentFile;
	firstProjects = allProjects;
	projectpath = "";
	while (firstProjects) {
		if (strcmp(firstProjects->m_sProjectFile.GetStr(), projectname) == 0) {
			projectpath = firstProjects->m_sProjectFileFullPath;
			cProjectItem::sProjectFiles * m_pCurrentFile;
			m_pCurrentFile = firstProjects->m_pFirstFile;
			while (m_pCurrentFile)
			{
#ifdef AGK_WINDOWS
				if (stricmp(m_pCurrentFile->m_sName.GetStr(), sourcefile) == 0) {
#else
				if (strcasecmp(m_pCurrentFile->m_sName.GetStr(), sourcefile) == 0) {
#endif
					//if (strcmp(m_pCurrentFile->m_sName.GetStr(), sourcefile) == 0) {
					return(m_pCurrentFile->m_editor);
				}
				m_pCurrentFile = m_pCurrentFile->m_pNext;
				}
			}
		firstProjects = firstProjects->m_pNext;
		}

	//sourcefile = 0x1531b168 "media/particle/particle.agc"
	//Search files outside projects.
	projectpath.ReplaceStr("\\", "/");
	m_pCurrentFile = allFiles;
	if (m_pCurrentFile) {
		while (m_pCurrentFile) {
			if (strcmp(m_pCurrentFile->m_sName.GetStr(), sourcefile) == 0) {
				return(m_pCurrentFile->m_editor);
			}
			if (strstr(sourcefile, "/") != NULL) {
				uString fname = m_pCurrentFile->m_sFullPath;
				fname.ReplaceStr("\\", "/");
				if (fname.FindStr(sourcefile) >= 0) {
					return(m_pCurrentFile->m_editor);
				}
			}
			m_pCurrentFile = m_pCurrentFile->m_pNext;
		}
	}

	return NULL;
}

static char randomstring[256];
char * RandomString(void)
{
	sprintf(randomstring, "%d", agk::Random(0, 10000));
	return &randomstring[0];
}


bool vTextEditor(char *winname, TextEditor * m_editor, char * cName, char * cPath, bool bUseSaveAs, cProjectItem::sProjectFiles * m_pCurFile)
{
	if (!m_editor)
		return false;

	if (m_editor->quiteditor) {
		if (m_editor->filechanged) { //m_editor->CanUndo()) {

			ImGui::OpenPopup("File changed.");

			ImGui::SetNextWindowSize(ImVec2(38 * ImGui::GetFontSize(), 10 * ImGui::GetFontSize()), ImGuiCond_Once);
			ImGui::SetNextWindowPosCenter(ImGuiCond_Once);

			if (ImGui::BeginPopupModal("File changed.", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				if( strcmp(cName, cPath) == 0 )
					ImGui::Text("Save changes to source file?\n%s\n", cName);
				else
					ImGui::Text("Save changes to source file?\n%s\n%s\n", cName, cPath);
				ImGui::Separator();

				if (ImGui::Button("SAVE", ImVec2(120, 0))) {
					if (bUseSaveAs) {

						cProjectItem::sProjectFiles * m_pCurrentFile;
						m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
						if (m_pCurrentFile) {
							uString mPath = m_pCurrentFile->m_sFullPath;
							int pos = mPath.RevFindStr("/");
							if (pos < 0) pos = mPath.RevFindStr("\\");
							if (pos > 0) {
								m_pCurrentFile->m_sFullPath.SubString(mPath, 0, pos);
							}
							char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "agc\0*.agc\0", mPath.GetStr(), NULL);
							if (cFileSelected && strlen(cFileSelected) > 0) {

								uString checkfile = cFileSelected;
								pos = checkfile.RevFindStr(".agc");
								if (pos < 0)
									checkfile.Append(".agc");

								bool oksave = true;
								if (file_exists((char*)checkfile.GetStr())) {
									oksave = overWriteFileBox((char*)checkfile.GetStr());
								}
								if (oksave) {
									auto textToSave = m_editor->GetText();
#ifndef DISABLESAVE
									std::ofstream myfile;
									myfile.open(checkfile);
									myfile << textToSave;
									myfile.close();
#endif
									m_editor->filechanged = false;
									m_editor->SetText("");
									m_editor->quiteditor = false;
									ImGui::CloseCurrentPopup();
									ImGui::EndPopup();
									return true;
								}
							}
						}
						m_editor->quiteditor = false;
						ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
						return false;
					}
					else {

						//Save file:
						auto textToSave = m_editor->GetText();
#ifndef DISABLESAVE
						std::ofstream myfile;
						myfile.open(cPath);
						myfile << textToSave;
						myfile.close();
#endif
						m_editor->filechanged = false;
						m_editor->SetText("");
						m_editor->quiteditor = false;
						ImGui::CloseCurrentPopup();
						ImGui::EndPopup();
						return true;
					}
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("DON'T SAVE", ImVec2(120, 0))) {
					m_editor->SetText("");
					m_editor->filechanged = false;
					m_editor->quiteditor = false;
					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
					return true;
				}
				ImGui::EndPopup();
			}
		}
		else {
			//No changes just quit.
			m_editor->SetText("");
			m_editor->quiteditor = false;
			return true;
		}
	}


	auto cpos = m_editor->GetCursorPosition();

	//ImGuiWindowFlags_DockNodeHost , ImGuiWindowFlags_NoDocking
	int editWinFlags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoNavInputs;
	//if (m_editor->filechanged)
	//	editWinFlags |= ImGuiWindowFlags_UnsavedDocument;

	//Before docking, try to hide windows.
	//if (m_editor->firstrun) ImGui::SetNextWindowSize(ImVec2(500, 300));
	if (m_editor->firstrun) ImGui::SetNextWindowSize(ImVec2(1, 1));
	if (m_editor->resizewindownext) {
		m_editor->resizewindownext = false;
		ImGui::SetNextWindowPos(ImVec2(40, 40));
		ImGui::SetNextWindowSize(ImVec2(600, 400));
	}
	if (m_editor->firstrun) ImGui::SetNextWindowPos(ImVec2(1140, 1140) , ImGuiSetCond_Once);

	//ImGuiWindowFlags_NoScrollbar
	if (m_pCurFile && m_pCurFile->m_scene && !m_pCurFile->m_scene->bDisplayAsText) {
		editWinFlags &= ~ImGuiWindowFlags_HorizontalScrollbar;
		editWinFlags |= ImGuiWindowFlags_NoScrollbar;
	}


	if (!m_editor->bEditorVisible) {
		if (!m_editor->symbolsList[m_editor->activeSymbolslist].empty()) {

			if (bCollectSuggestKeywords && pref.bEnableSymboldList) {
				if (!m_editor->symbolsList[m_editor->activeSymbolslist].empty()) {

					for (SYMBOLS_MAP::iterator it = m_editor->symbolsList[m_editor->activeSymbolslist].begin(); it != m_editor->symbolsList[m_editor->activeSymbolslist].end(); ++it)
					{
						TextEditor::SymbolsValues stmp;
						stmp.iSymbolType = it->second.iSymbolType; //Types
						stmp.lineno = it->second.lineno;
						strcpy(stmp.cCommand, it->second.cCommand);
						strcpy(stmp.cParameters, it->second.cParameters);
						strcpy(stmp.cType, it->second.cType);
						stmp.bVisible = it->second.bVisible;
						stmp.m_InsideEditor = m_editor;
						symbolsCollectList[iCollectListUpdate].insert(std::make_pair(std::string(it->first.c_str()), stmp));
					}
				}
			}
			return false;
		}
		m_editor->ColorizeInternal(0, 1);
		return false;
	}

	if (first_editor_processed == false)
	{
		if (collect_bookmark_list == 0) {
			collect_bookmark_list = 1;
			active_bookmark_list = 0;
		}
		else {
			collect_bookmark_list = 0;
			active_bookmark_list = 1;
		}
		AllBookmark[collect_bookmark_list].clear();
		count_bookmark_list = 1;
		first_editor_processed = true;
	}
	//Add all bookmarks.
	//mBookMarks
	//for (std::set<int>::iterator it = m_editor->bmarks.begin(); it != m_editor->bmarks.end(); ++it) {
	for (std::set<int>::iterator it = m_editor->mBookMarks.begin(); it != m_editor->mBookMarks.end(); ++it) {

		BookmarkValues stmp;
		stmp.lineno = *it;
		stmp.m_InsideEditor = m_editor;

		AllBookmark[collect_bookmark_list].insert(std::make_pair(std::int32_t(count_bookmark_list) , stmp));
		count_bookmark_list++;
	}
//	int test = 0;
//	for (std::unordered_map<std::int32_t, BookmarkValues>::iterator it = AllBookmark[active_bookmark_list].begin(); it != AllBookmark[active_bookmark_list].end(); ++it)
//	{
//		test++;
//	}

	ImGui::Begin(winname, &m_editor->closenotclicked, editWinFlags );
	if (!m_editor->closenotclicked) {
		m_editor->quiteditor = true;
		m_editor->closenotclicked = true;
	}
	

	if (ImGui::BeginPopupContextItemAGK("TabContentMenuForEditors"))
	{
		if (cName) {
			ImGui::Text(cName);
			ImGui::Separator();
		}
		if (ImGui::MenuItem("Close##TabContentMenuForEditors")) {
			m_editor->quiteditor = true;
		}
		if (ImGui::MenuItem("Close All Other##TabContentMenuForEditors")) {
			CloseAllOpenProjectEditedFiles();
			m_editor->quiteditor = false;
		}
		if (ImGui::MenuItem("Close All##TabContentMenuForEditors")) {
			CloseAllOpenProjectEditedFiles();
		}
		ImGui::EndPopup();
	}

	//Moved here so above context will work.
	edit_area_parent = ImGui::GetCurrentWindow();

//	if (ImGui::IsItemHovered()) {
//		//
////		ImGui::SetTooltip("Testing Teb tooltip.");
//	}


	ImGuiWindow* agk_window;
	ImGuiContext& g = *GImGui;

	if (pressedCTRLkey == '\t' || processCTRLkey == '\t' ) {

		ImGuiWindow* first=NULL, *best = NULL,*current = NULL,*select = NULL;
		cProjectItem::sProjectFiles * m_pCurrentFile = NULL;
		
		if (m_ActiveEditor)
			m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);

		//PE: https://github.com/TheGameCreators/AGK-Studio/issues/673
		//Ignore: main.agc##GGLoader.agk/TextEditor_38D09F3D/##SearchFindReplaceDialog_CE59DCF3

		for (int n = 0; n < g.WindowsFocusOrder.Size - 1; n++)
		{
			ImGuiWindow* window = g.WindowsFocusOrder[n];
			uString project_search = pCurrentSelectedProject->m_sProjectFile.GetStr();
			project_search.Append("/");

			if (window->Name && strstr(window->Name, "/TextEditor")) {

				if (!strstr(window->Name, "##SearchFindReplaceDialog")) {

					if (pref.bDisplayActiveProjectFilesOnly && pCurrentSelectedProject) {
						if (strstr(window->Name, project_search.GetStr()) || strstr(window->Name, "na-noproject")) {
							if (!first && window->Pos.x != -9999.0)
								first = window;
						}
					}
					else {
						if (!first && window->Pos.x != -9999.0)
							first = window;
					}

					if (pref.bDisplayActiveProjectFilesOnly && pCurrentSelectedProject) {
						if (strstr(window->Name, project_search.GetStr()) || strstr(window->Name, "na-noproject")) {
							if (current && !best && window->Pos.x != -9999.0) {
								best = window;
							}
						}
					}
					else if (current && !best && window->Pos.x != -9999.0) {
						best = window;
					}
					if (m_pCurrentFile) {
						if (strncmp(m_pCurrentFile->m_sEditName.GetStr(), window->Name, m_pCurrentFile->m_sEditName.GetLength()) == 0) {
							current = window;
						}
					}
				}
			}
		}
		if (processCTRLkey == '\t' && first) {
			select = first;
			processCTRLkey = 0;
		}
		else if (best)
			select = best;
		else if (first)
			select = first;
		if (select) {
			//Select new window.
			uString szTmp = select->Name;
			int pos = szTmp.RevFindStr("/");
			if (pos > 0) {
				szTmp.SubString(szNextWindowFocus, 0, pos);

				bFocusActiveEditor = true;
				cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
			}

		}

		agk::KeyUp(9);
		pressedCTRLkey = 0;
	}

	if (ImGui::windowTabVisible()) {
		if (m_ActiveEditor == NULL && processCTRLkey == '\t') {
			m_ActiveEditor = m_editor;
		}

		agk_window = ImGui::GetCurrentWindow();
		if (g.NavWindow == agk_window) {
			m_ActiveEditor = m_editor;
			if (m_pCurFile && m_pCurFile->m_scene && m_pCurFile->m_scene->m_pCurrentScene)
				m_ActiveScene = m_pCurFile->m_scene->m_pCurrentScene;
			else if (m_pCurFile && m_pCurFile->m_scene )
				m_ActiveScene = m_pCurFile->m_scene;
		}
	}


	if (bCollectSuggestKeywords && pref.bEnableSymboldList) {
		if (!m_editor->symbolsList[m_editor->activeSymbolslist].empty()) {
			
			for(SYMBOLS_MAP::iterator it = m_editor->symbolsList[m_editor->activeSymbolslist].begin(); it != m_editor->symbolsList[m_editor->activeSymbolslist].end(); ++it)
			{
				TextEditor::SymbolsValues stmp;
				stmp.iSymbolType = it->second.iSymbolType; //Types
				stmp.lineno = it->second.lineno;
				strcpy(stmp.cCommand, it->second.cCommand);
				strcpy(stmp.cParameters, it->second.cParameters);
				strcpy(stmp.cType, it->second.cType);
				stmp.bVisible = it->second.bVisible;
				stmp.m_InsideEditor = m_editor;
				symbolsCollectList[iCollectListUpdate].insert(std::make_pair(std::string(it->first.c_str()), stmp));
			}
		}
	}


	if (m_pCurFile && m_pCurFile->m_scene && !m_pCurFile->m_scene->bDisplayAsText) {
		//Scene file.
//		if (m_editor->firstrun) {
//			//Init all scenes
//			MediaScene * m_pSearchScene;
//			m_pSearchScene = m_pCurFile->m_scene;
//			while (m_pSearchScene) {
//				//m_pSearchScene->Render(winname, m_editor, m_pCurFile, m_pCurFile->m_scene);
//				m_pSearchScene = m_pSearchScene->m_pNextScene;
//
//			}
//		}

		//extern cFolderItem *m_pSelectedFolder;
		iRenderedScenes++;
		if(m_pCurFile->m_scene->m_pCurrentScene)
			m_pCurFile->m_scene->m_pCurrentScene->Render(winname, m_editor, m_pCurFile, m_pCurFile->m_scene);
		else
			m_pCurFile->m_scene->Render(winname, m_editor, m_pCurFile, m_pCurFile->m_scene);
	}
	else {

		if (ImGui::BeginMenuBar())
		{
			//Menu active check if we selected this window.
			if (agk::GetPointerPressed()) {
				float mx, my;
				mx = agk::GetPointerX();
				my = agk::GetPointerY();
				if (mx >= ImGui::GetWindowPos().x && my >= ImGui::GetWindowPos().y && mx <= (ImGui::GetWindowPos().x + ImGui::GetWindowSize().x) && my <= (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y))
				{
					agk_window = ImGui::GetCurrentWindow();
					if (g.NavWindow == agk_window) {

						if (m_ActiveEditor != m_editor)
							m_editor->mScrollToCursor = true;
						m_ActiveEditor = m_editor;
						if (m_pCurFile && m_pCurFile->m_scene && m_pCurFile->m_scene->m_pCurrentScene)
							m_ActiveScene = m_pCurFile->m_scene->m_pCurrentScene;
						else if (m_pCurFile && m_pCurFile->m_scene)
							m_ActiveScene = m_pCurFile->m_scene;
					}
				}
			}

			if (m_pCurFile && m_pCurFile->m_scene) {
				if (ImGui::Button("2D")) {
					m_pCurFile->m_scene->bDisplayAsText = false;
				}
//				ImGui::SameLine();
//				if (ImGui::Button("Script")) {
//					m_pCurFile->m_scene->bDisplayAsText = true;
//				}
				ImGui::SameLine();
				ImGui::Separator();
				ImGui::SameLine();
			}


			if (ImGui::BeginMenu("File"))
			{
				/*
				Need: && m_pCurrentFile->m_sProjectName.FindStr(".agc",1) > 0
							if (ImGui::MenuItem("Run in SandBox"))
							{
				#ifdef AGK_WINDOWS
								_chdir(startupFolder);
				#else
								chdir(startupFolder);
				#endif
								auto textToSave = m_editor->GetText();
								std::ofstream myfile;
								myfile.open("media/sandbox/main.agc");
								myfile << textToSave;
								myfile.close();
								textToSave = "";
								CompileProject(NULL, true, false, false); // flags: run,broadcast,debug.
							}
				*/
				if (ImGui::MenuItem("Save"))
				{
					if (bUseSaveAs) {
						cProjectItem::sProjectFiles * m_pCurrentFile;
						m_pCurrentFile = FindFileFromEditor(m_editor);
						if (m_pCurrentFile) {

							uString mPath = m_pCurrentFile->m_sFullPath;
							int pos = mPath.RevFindStr("/");
							if (pos < 0) pos = mPath.RevFindStr("\\");
							if (pos > 0) {
								m_pCurrentFile->m_sFullPath.SubString(mPath, 0, pos);
							}
							char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "agc\0*.agc\0", mPath.GetStr(), NULL);
							if (cFileSelected && strlen(cFileSelected) > 0) {

								uString checkfile = cFileSelected;
								pos = checkfile.RevFindStr(".agc");
								if (pos < 0)
									checkfile.Append(".agc");

								bool oksave = true;
								if (file_exists((char*)checkfile.GetStr())) {
									oksave = overWriteFileBox((char*)checkfile.GetStr());
								}
								if (oksave) {
									auto textToSave = m_editor->GetText();
									std::ofstream myfile;
									m_pCurrentFile->m_sFullPath = checkfile;
									m_pCurrentFile->m_bUseSaveAs = false;
									uString newname = "na";
									int pos = checkfile.RevFindStr("/");
									if (pos <= 0) pos = checkfile.RevFindStr("\\");
									if (pos > 0) {
										checkfile.SubString(newname, pos + 1);
										//m_pCurrentFile->m_sProjectName = "na-noproject";
										m_pCurrentFile->m_sName = newname;
										m_pCurrentFile->m_sEditName = m_pCurrentFile->m_sName;
										if (pref.bRemovePathFromTabName) {
											uString newTabname = m_pCurrentFile->m_sEditName;
											int ntb_pos = newTabname.RevFindStr("/");
											if (ntb_pos <= 0)
												ntb_pos = newTabname.RevFindStr("\\");
											if (ntb_pos > 0)
												newTabname.SubString(m_pCurrentFile->m_sEditName, ntb_pos + 1);
										}

										m_pCurrentFile->m_sEditName.Append("##");
										m_pCurrentFile->m_sEditName.Append(m_pCurrentFile->m_sProjectName);
										m_pCurrentFile->m_sEditName.Append(RandomString());
										cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									}
#ifndef DISABLESAVE
									myfile.open(checkfile);
									myfile << textToSave;
									myfile.close();
#endif
									m_editor->filechanged = false;
									m_editor->firstrun = true;
									ImGui::EndMenu();
									ImGui::EndMenuBar();
									ImGui::End();
									return false;
								}
							}
						}
					}
					else {

						auto textToSave = m_editor->GetText();
#ifndef DISABLESAVE
						std::ofstream myfile;
						myfile.open(cPath);
						myfile << textToSave;
						myfile.close();
						m_editor->filechanged = false;
#else
						uString UFTTEST= cPath;
						UFTTEST.Append(".utf8");
						std::ofstream myfile;
						myfile.open(UFTTEST);
						myfile << textToSave;
						myfile.close();
						m_editor->filechanged = false;

#endif

					}

				}
				if (ImGui::MenuItem("Save As"))
				{
					// save text as....
					uString mPath = cPath;
					uString mFullPath = mPath;
					int pos = mPath.RevFindStr("/");
					if (pos < 0) pos = mPath.RevFindStr("\\");
					if (pos > 0) {
						mFullPath.SubString(mPath, 0, pos);
					}
					char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "agc\0*.agc\0", mPath.GetStr(), NULL);
					if (cFileSelected && strlen(cFileSelected) > 0) {
						uString checkfile = cFileSelected;
						pos = checkfile.RevFindStr(".agc");
						if (pos < 0)
							checkfile.Append(".agc");

						bool oksave = true;
						if (file_exists((char*)checkfile.GetStr())) {
							oksave = overWriteFileBox((char*)checkfile.GetStr());
						}
						if (oksave) {
							auto textToSave = m_editor->GetText();
#ifndef DISABLESAVE
							std::ofstream myfile;
							myfile.open(checkfile);
							myfile << textToSave;
							myfile.close();
							m_editor->filechanged = false;
#endif
							//close this and load the new one.
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = CreateNewAGCFile((char *)checkfile.GetStr());
							if (m_pCurrentFile) {
								m_editor->quiteditor = true;
								vTextEditorInit(m_pCurrentFile->m_editor, (char *)m_pCurrentFile->m_sFullPath.GetStr());
								m_pCurrentFile->m_bOpenForEdit = true;
								cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
							}


						}
					}
				}

				if (ImGui::MenuItem("Close")) {
					//Do we need to save ?
					m_editor->quiteditor = true;
					//Quit code.
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				bool ro = m_editor->IsReadOnly();
				if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
					m_editor->SetReadOnly(ro);

				ImGui::Separator();

				if (ImGui::MenuItem("Find", pref.cFindText)) {
					m_editor->OpenFind();
				}

				if (ImGui::MenuItem("Replace", pref.cReplaceText)) {
					m_editor->Replace();
				}

				if (ImGui::MenuItem("Toggle Line Comment", "Ctrl-E")) {
					m_editor->ToggleLineComments();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Undo", pref.cUndoText, nullptr, !ro && m_editor->CanUndo()))
					m_editor->Undo();
				if (ImGui::MenuItem("Redo", pref.cRedoText, nullptr, !ro && m_editor->CanRedo()))
					m_editor->Redo();

				ImGui::Separator();

				if (ImGui::MenuItem("Copy", pref.cCopyText, nullptr, m_editor->HasSelection()))
					m_editor->Copy();
				if (ImGui::MenuItem("Cut", pref.cCutText, nullptr, !ro && m_editor->HasSelection()))
					m_editor->Cut();
				//if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && m_editor->HasSelection()))
				//	m_editor->Delete();
				if (ImGui::MenuItem("Paste", pref.cPasteText, nullptr, !ro && ImGui::GetClipboardText() != nullptr))
					m_editor->Paste();

				ImGui::Separator();

				if (ImGui::MenuItem("Select All", nullptr, nullptr))
					m_editor->SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(m_editor->GetTotalLines(), 0));

				ImGui::Separator();


				if (ImGui::MenuItem("Bookmark toggle")) {
					
					//if (m_editor->bmarks.count(m_editor->mState.mCursorPosition.mLine + 1) != 0)
					if (m_editor->mBookMarks.count(m_editor->mState.mCursorPosition.mLine + 1) != 0)
					{
						//Already there , remove.
//						m_editor->bmarks.erase(m_editor->mState.mCursorPosition.mLine + 1);
//						m_editor->SetBookMarks(m_editor->bmarks);
						m_editor->mBookMarks.erase(m_editor->mState.mCursorPosition.mLine + 1);
						m_editor->SetBookMarks(m_editor->mBookMarks);
					}
					else {
						//Not set , Add
//						m_editor->bmarks.insert(m_editor->mState.mCursorPosition.mLine + 1);
//						m_editor->SetBookMarks(m_editor->bmarks);
						m_editor->mBookMarks.insert(m_editor->mState.mCursorPosition.mLine + 1);
						m_editor->SetBookMarks(m_editor->mBookMarks);
					}
				}

				if (ImGui::MenuItem("Breakpoint toggle")) {
					if (m_editor->bpts.count(m_editor->mState.mCursorPosition.mLine + 1) != 0)
					{
						//Already there , remove.
						//bpts.find()

						m_editor->bpts.erase(m_editor->mState.mCursorPosition.mLine + 1);
						m_editor->SetBreakpoints(m_editor->bpts);
						cProjectItem::sProjectFiles * m_pCurrentFile;
						m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
						if (m_pCurrentFile) {
							char tmp[MAX_PATH];
							sprintf(tmp, "delete breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), m_editor->mState.mCursorPosition.mLine + 1);
							uString bc_err;
							BroadcasterCommand(tmp,bc_err);
						}
						//addAllBreakPoints(NULL);
					}
					else {
						//Not set , Add
						//bpts.clear();
						m_editor->bpts.insert(m_editor->mState.mCursorPosition.mLine + 1);
						m_editor->SetBreakpoints(m_editor->bpts);
						cProjectItem::sProjectFiles * m_pCurrentFile;
						m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
						if (m_pCurrentFile) {
							char tmp[MAX_PATH];
							sprintf(tmp, "breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), m_editor->mState.mCursorPosition.mLine + 1);
							uString bc_err;
							BroadcasterCommand(tmp,bc_err);
						}
						//addAllBreakPoints(NULL);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Clear All Breakpoints", nullptr, nullptr)) {
					if( debug_is_running )
					{
						uString bc_err;
						BroadcasterCommand("delete all breakpoints",bc_err);
					}
					m_editor->mBreakpoints.clear();
					m_editor->bpts.clear();
				}
				if (ImGui::MenuItem("Clear All Bookmarks", nullptr, nullptr)) {
					m_editor->mBookMarks.clear();
					m_editor->bmarks.clear();
					if (pCurrentSelectedProject) {
						cProjectItem::sProjectFiles * allProjectFiles;
						allProjectFiles = pCurrentSelectedProject->m_pFirstFile;
						while (allProjectFiles) {
							if (allProjectFiles->m_editor && !allProjectFiles->m_scene) {
								allProjectFiles->m_editor->mBookMarks.clear();
								allProjectFiles->m_editor->bmarks.clear();
							}
							allProjectFiles = allProjectFiles->m_pNext;
						}
					}
				}


				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View "))
			{
				if (pref.editor_display_linenumbers) {
					if (ImGui::MenuItem("Hide Line Numbers")) {
						pref.editor_display_linenumbers = false;
					}
				}
				else {
					if (ImGui::MenuItem("Show Line Numbers")) {
						pref.editor_display_linenumbers = true;
					}
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Default Style"))
					m_editor->SetPalette(TextEditor::GetAGKPalette());
				if (ImGui::MenuItem("Dark Style"))
					m_editor->SetPalette(TextEditor::GetDarkPalette());
				if (ImGui::MenuItem("Light Style"))
					m_editor->SetPalette(TextEditor::GetLightPalette());
				if (ImGui::MenuItem("Retro Blue Style"))
					m_editor->SetPalette(TextEditor::GetRetroBluePalette());
				
				ImGui::EndMenu();
			}


			ImGui::SameLine();
			ImGui::Separator();
			ImGui::SameLine();

			if (ImGui::Button("<")) {
				//Go back in position record.
				//iRecordIndex
				if (m_ActiveEditor) {
					auto it = posRecord.find(iRecordIndex - 1);
					if (it != posRecord.end())
					{
						iRecordIndex--;

						if (it->second.m_RecordEditor) {
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = FindFileFromEditor(it->second.m_RecordEditor);
							if (m_pCurrentFile) {

								if (m_pCurrentFile->m_editor && m_ActiveEditor != m_pCurrentFile->m_editor) {

									if (!m_pCurrentFile->m_editor->bEditorVisible) {
										//Make sure its docked.
										m_pCurrentFile->m_editor->firstrun = true;
									}
									m_pCurrentFile->m_editor->bEditorVisible = true;

									cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									bFocusActiveEditor = true;
								}
							}
							//Focus.
							it->second.m_RecordEditor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno, it->second.colno));
							it->second.m_RecordEditor->EnsureCursorVisible();
							it->second.m_RecordEditor->mScrollToCursor = true;
						}

						//iLastRecordLine = it->second.m_RecordEditor->mState.mCursorPosition.mLine;
						//iLastRecordCol = it->second.m_RecordEditor->mState.mCursorPosition.mColumn;
						//m_LastRecordEditor = it->second.m_RecordEditor; // cant change will trigger a new record.

						m_IgnoreLastRecordEditor = it->second.m_RecordEditor;
						iIgnoreLastRecordLine = it->second.m_RecordEditor->mState.mCursorPosition.mLine;;
						iIgnoreLastRecordCol = it->second.m_RecordEditor->mState.mCursorPosition.mColumn;
						//m_ActiveEditor = it->second.m_RecordEditor; //?
					}
					else {
						//Just enable cursor at current pos.
						if (m_ActiveEditor) {
							m_ActiveEditor->EnsureCursorVisible();
							m_ActiveEditor->mScrollToCursor = true;
						}
					}
				}

			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Navigate Backward");

			ImGui::SameLine();
			//ImGuiButtonFlags_Disabled
			//if (ImGui::ButtonEx(">",ImVec2(0,0), ImGuiButtonFlags_Disabled)) {
			if (ImGui::Button(">")) {
				//Go forward in position record.
				if (posRecord.size() > iRecordIndex && m_ActiveEditor ) {

					auto it = posRecord.find(iRecordIndex + 1);
					if (it != posRecord.end())
					{
						iRecordIndex++;

						if (it->second.m_RecordEditor) {
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = FindFileFromEditor(it->second.m_RecordEditor);
							if (m_pCurrentFile) {

								if (m_pCurrentFile->m_editor && m_ActiveEditor != m_pCurrentFile->m_editor ) {

									if (!m_pCurrentFile->m_editor->bEditorVisible) {
										//Make sure its docked.
										m_pCurrentFile->m_editor->firstrun = true;
									}
									m_pCurrentFile->m_editor->bEditorVisible = true;

									cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									bFocusActiveEditor = true;
								}
							}
							//Focus.
							it->second.m_RecordEditor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno, it->second.colno));
							it->second.m_RecordEditor->EnsureCursorVisible();
							it->second.m_RecordEditor->mScrollToCursor = true;
						}

						//iLastRecordLine = it->second.m_RecordEditor->mState.mCursorPosition.mLine;
						//iLastRecordCol = it->second.m_RecordEditor->mState.mCursorPosition.mColumn;
						//m_LastRecordEditor = it->second.m_RecordEditor; // cant change will trigger a new record.

						m_IgnoreLastRecordEditor = it->second.m_RecordEditor;
						iIgnoreLastRecordLine = it->second.m_RecordEditor->mState.mCursorPosition.mLine;;
						iIgnoreLastRecordCol = it->second.m_RecordEditor->mState.mCursorPosition.mColumn;
						//m_ActiveEditor = it->second.m_RecordEditor; //?
					}
				}
				else {
					//Just enable cursor at current pos.
					if (m_ActiveEditor) {
						m_ActiveEditor->EnsureCursorVisible();
						m_ActiveEditor->mScrollToCursor = true;
					}
				}
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Navigate Forward");

//			{
//				//Debug only.
//				ImGui::Text("iRId: %d s(%d)", iRecordIndex, posRecord.size());
//				ImGui::SameLine();
//			}

			ImGui::SameLine();
			ImGui::Separator();
			ImGui::SameLine();

			if (pref.bEnableSymboldList && !m_editor->symbolsList[m_editor->activeSymbolslist].empty()) {
				ImGuiWindow* window = ImGui::GetCurrentWindowRead();
				int iComboWidth = ImGui::GetWindowSize().x;
				//iComboWidth -= 11 * ImGui::GetFontSize(); // 164; // menu size.
				iComboWidth -= 15 * ImGui::GetFontSize(); // Added additional buttons.
				if (m_pCurFile && m_pCurFile->m_scene) {
					iComboWidth -= 5 * ImGui::GetFontSize(); // 164; // menu size.
				}
				//Dive by?
				int DevideBy = 0;
				if (m_editor->activeFunctions > 0)
					DevideBy++;
				if (m_editor->activeVariables > 0)
					DevideBy++;
				if (m_editor->activeConstants > 0)
					DevideBy++;
				if (m_editor->activeTypes > 0)
					DevideBy++;

				if (iComboWidth > 0 && DevideBy > 0)
					iComboWidth /= DevideBy;
				else
					iComboWidth = 20; // smallest possible,

				int iSCount = m_editor->activeFunctions;
				int iMenuItems = 0;
				//ImGui::SetNextWindowContentWidth(200);
				if (iSCount > 0)
				{
					//PE: Display what function we are in.
					//PE: We might need a list of all endfunctions for this to be perfect, but for now.
					static int iLastCursorPosY = -1;
					static char cMenuDefaultText[MAX_PATH] = " Functions \0";
					if (m_editor->GetCursorPosition().mLine != iLastCursorPosY)
					{
						int min_functionline = -1;
						for (SYMBOLS_MAP::iterator it = m_editor->symbolsList[m_editor->activeSymbolslist].begin(); it != m_editor->symbolsList[m_editor->activeSymbolslist].end(); ++it)
						{
							if (it->second.iSymbolType == 1)
							{
								if (m_editor->GetCursorPosition().mLine >= it->second.lineno && min_functionline < it->second.lineno)
								{
									sprintf(cMenuDefaultText, " %.40s%.40s {%d} ", it->second.cCommand, it->second.cParameters, it->second.lineno);
									min_functionline = it->second.lineno;
								}
							}
						}
						if (min_functionline < 0)
						{
							strcpy(cMenuDefaultText," Functions ");
						}

						iLastCursorPosY = m_editor->GetCursorPosition().mLine;
					}
					ImGui::PushItemWidth(iComboWidth);
					if (ImGui::BeginCombo("##comboFunctions", cMenuDefaultText, ImGuiComboFlags_HeightLarge)) //" Functions "
					{
						char cMenuText[MAX_PATH], cTmp[MAX_PATH];
						for (SYMBOLS_MAP::iterator it = m_editor->symbolsList[m_editor->activeSymbolslist].begin(); it != m_editor->symbolsList[m_editor->activeSymbolslist].end(); ++it)
						{
							if (it->second.iSymbolType == 1)
							{
								sprintf(cMenuText, " %.40s%.40s {%d} ", it->second.cCommand, it->second.cParameters, it->second.lineno);
								bool is_selected = false;
								if (ImGui::Selectable(cMenuText, is_selected))
								{
									//selected goto lineno.
									m_editor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									m_editor->EnsureCursorVisible();
									m_editor->mScrollToCursor = true;
								}
							}
						}
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();
				}

				//Variables.
				iSCount = m_editor->activeVariables;
				if (iSCount > 0)
				{
					ImGui::SameLine();
					ImGui::PushItemWidth(iComboWidth);
					if (ImGui::BeginCombo("##comboVariables", " Variables ", ImGuiComboFlags_HeightLarge))
					{
						char cMenuText[MAX_PATH], cTmp[MAX_PATH];
						for (SYMBOLS_MAP::iterator it = m_editor->symbolsList[m_editor->activeSymbolslist].begin(); it != m_editor->symbolsList[m_editor->activeSymbolslist].end(); ++it)
						{
							if (it->second.iSymbolType == 2 && it->second.bVisible ) {

								if (strlen(it->second.cType) > 0) {
									sprintf(cMenuText, " %.40s%.40s Type: %s {%d}  ", it->second.cCommand, it->second.cParameters, it->second.cType, it->second.lineno);
								}
								else
									sprintf(cMenuText, " %.40s%.40s {%d} ", it->second.cCommand, it->second.cParameters, it->second.lineno);

								//sprintf(cMenuText, "%.40s%s", it->second.cCommand, it->second.cParameters);
								bool is_selected = false;
								if (ImGui::Selectable(cMenuText, is_selected)) {
									//selected goto lineno.
									m_editor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									m_editor->EnsureCursorVisible();
									m_editor->mScrollToCursor = true;
								}
							}
						}
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();
				}

				//Constants.
				iSCount = m_editor->activeConstants;
				if (iSCount > 0) {
					ImGui::SameLine();
					ImGui::PushItemWidth(iComboWidth);
					if (ImGui::BeginCombo("##comboConstants", " Constants ", ImGuiComboFlags_HeightLarge))
					{
						char cMenuText[MAX_PATH], cTmp[MAX_PATH];
						for (SYMBOLS_MAP::iterator it = m_editor->symbolsList[m_editor->activeSymbolslist].begin(); it != m_editor->symbolsList[m_editor->activeSymbolslist].end(); ++it)
						{
							if (it->second.iSymbolType == 3) {

								sprintf(cMenuText, " %.40s %.40s {%d} ", it->second.cCommand, it->second.cParameters, it->second.lineno);
								//sprintf(cMenuText, "%.40s%s", it->second.cCommand, it->second.cParameters);
								bool is_selected = false;
								if (ImGui::Selectable(cMenuText, is_selected)) {
									//selected goto lineno.
									m_editor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									m_editor->EnsureCursorVisible();
									m_editor->mScrollToCursor = true;
								}
							}
						}
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();
				}

				//Types.
				iSCount = m_editor->activeTypes;
				if (iSCount > 0) {
					ImGui::SameLine();
					ImGui::PushItemWidth(iComboWidth);
					if (ImGui::BeginCombo("##comboTypes", " Types ", ImGuiComboFlags_HeightLarge))
					{
						char cMenuText[MAX_PATH], cTmp[MAX_PATH];
						for (SYMBOLS_MAP::iterator it = m_editor->symbolsList[m_editor->activeSymbolslist].begin(); it != m_editor->symbolsList[m_editor->activeSymbolslist].end(); ++it)
						{
							if (it->second.iSymbolType == 4) {

								if (strstr(it->second.cCommand, "."))
									sprintf(cMenuText, "     %.40s %.40s {%d} ", it->second.cCommand, it->second.cParameters, it->second.lineno);
								else
									sprintf(cMenuText, " %.40s %.40s {%d} ", it->second.cCommand, it->second.cParameters, it->second.lineno);
								//sprintf(cMenuText, "%.40s%s", it->second.cCommand, it->second.cParameters);

								bool is_selected = false;
								if (ImGui::Selectable(cMenuText, is_selected)) {
									//selected goto lineno.
									m_editor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									m_editor->EnsureCursorVisible();
									m_editor->mScrollToCursor = true;
								}
							}
						}
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();
				}
			}

			ImGui::EndMenuBar();
		}

		uString fullName = cPath;
		uString displayName;
		displayName = cPath;
		int pos = 0;
		for (int a = 0; a < 3;a++) {
			pos = displayName.RevFindStr("/");
			if (pos < 0) displayName.RevFindStr("\\");
			else fullName.SubString(displayName, 0, pos);
		}

		if (pos > 0) {
			fullName.SubString(displayName, pos + 1);
			fullName = "...";
			fullName.Append(displayName);
		}

		//TEST DEBUG Display key states.
	/*
		ImGuiIO& io = ImGui::GetIO();
		auto shift = io.KeyShift;
		auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
		auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
		if (keyboard_layout == 0) {
			ctrl = io.KeyCtrl;
			alt = io.KeyAlt;
		}
		char debugmsg[256];
		sprintf(debugmsg, "CTRL %d , ALT %d , SHIFT %d, CMD %d", ctrl, alt, shift, io.KeySuper);
	*/
	//debugmsg
		ImGui::Text("%6d/%-6d %5d lines  | %s | %s %s", cpos.mLine + 1, cpos.mColumn + 1, m_editor->GetTotalLines(),
			m_editor->IsOverwrite() ? "Ovr" : "Ins",
			fullName.GetStr(),
			m_editor->filechanged ? "| *" : " ");

		/*
			if (g.NavWindow->Name)
				ImGui::Text(g.NavWindow->Name);
			if (m_ActiveEditor == m_editor) {
				ImGui::Text("ACTIVE!");
				if (ImGui::IsWindowHovered()) {
					ImGui::SameLine();
					ImGui::Text(" HOVERED!");
				}
				if (ImGui::IsWindowAppearing() ) {
					ImGui::SameLine();
					ImGui::Text(" APPERING!");
				}
				if (ImGui::IsWindowFocused()) {
					ImGui::SameLine();
					ImGui::Text(" FOCUS!");
				}

			}
		*/

		//Drag drop active.
		//Only allow drag drops to files from own project.
		char *projectName = NULL;
		bool dragdropactive = false;
		if (pDragDropFile) {
			projectName = (char *)pDragDropFile->m_sFolder.GetStr();
			if (strstr(winname, projectName) != NULL) {
				dragdropactive = true;
				pDragDropText = NULL;
			}
			else {
				uString folder, sfinal;
				folder = cPath;
				int pos = folder.RevFindStr("/");
				if (pos < 0) folder.RevFindStr("\\");
				if (pos > 0) {
					folder.SubString(sfinal, 0, pos);
					if (strstr(pDragDropFile->m_sPath.GetStr(), sfinal) != NULL) {
						dragdropactive = true;
						pDragDropText = NULL;
					}
				}
			}
		}

		if (pDragDropText) {
			dragdropactive = true;
			pDragDropFile = NULL;
		}

		m_editor->dragdropactive = dragdropactive;
		m_editor->display_linenumbers = pref.editor_display_linenumbers;

		if (ImGui::windowTabVisible()) {
			if (!editorfont->IsLoaded())
				ImGui::PushFont(defaultfont);  //defaultfont
			else
				ImGui::PushFont(editorfont);  //defaultfont
			m_editor->Render("TextEditor", ImVec2(), false, modal_window_active, projectName);
			ImGui::PopFont();
		}
		else {
			//Keep slowly rendering if not active to get all color set done in the background.
			//Changed from 4 to 6 after adding symbols list for more speed, need testing.
			if (m_editor->m_RenderCount < 0 )
				m_editor->m_RenderCount = 0;
			if (m_editor->m_RenderCount++ > 9) {
				if(!editorfont->IsLoaded())
					ImGui::PushFont(defaultfont);  //defaultfont
				else
					ImGui::PushFont(editorfont);  //defaultfont
				m_editor->Render("TextEditor", ImVec2(), false, modal_window_active, projectName);
				ImGui::PopFont();
				m_editor->m_RenderCount = 0;
			}
		}

	}
	
	if (refresh_gui_docking >= 4 && m_editor->firstrun) {

		m_editor->m_RenderCount = agk::Random(0, 8);

		//dock it to the main window tab.
		int winNodeId = ImGui::GetWindowDockID();
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

		ImGui::DockBuilderDockWindow(winname, dock_main_tabs);
		int winNodeId2 = ImGui::GetWindowDockID();
		if (winNodeId != 0 && winNodeId2 != 0 && winNodeId != winNodeId2) {
			//Somthing wrong we cant rebuild.
			m_editor->firstrun = false;
			
			//Change window size to normal:
			m_editor->resizewindownext = true;

			//Cant redock windows if it has been attached to another window ?

			//ImGui::DockBuilderDockWindow(winname, winNodeId);
			//ImGui::DockBuilderFinish(dockspace_id);

			//ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_Dockspace); // Add empty node
			//ImGui::DockBuilderSetNodePos(dockspace_id, viewport->Pos + ImVec2(0, toolbar_size));
			//ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size - ImVec2(0, toolbar_size));
			//ImGui::DockBuilderDockWindow(winname, dock_main_tabs);
			//ImGui::DockBuilderFinish(dockspace_id);

		}
		else {
			ImGui::DockBuilderFinish(dockspace_id);
		}

		m_editor->firstrun = false;
	}


	ImGui::End();


	if (m_editor->saveonreturn) {
		//Save file , from editor input like CTRL+S
		cProjectItem::sProjectFiles * m_pCurrentFile;
		m_pCurrentFile = FindFileFromEditor(m_editor);
		if (m_pCurrentFile) {

			if (bUseSaveAs) {

				uString mPath = m_pCurrentFile->m_sFullPath;
				int pos = mPath.RevFindStr("/");
				if (pos < 0) pos = mPath.RevFindStr("\\");
				if (pos > 0) {
					m_pCurrentFile->m_sFullPath.SubString(mPath, 0, pos);
				}
				char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "agc\0*.agc\0", mPath.GetStr(), NULL);

				ImGuiIO& io = ImGui::GetIO();
				io.KeySuper = false;
				io.KeyCtrl = false;
				io.KeyAlt = false;
				io.KeyShift = false;
				agk::KeyUp(16); // reset shift key.
				agk::KeyUp(17); // reset ctrl key.
				agk::KeyUp(18); // reset alt key.
				agk::KeyUp(19); // reset pause key.
				for (int i = 8; i < 273; i++) {
					agk::KeyUp(i);
				}

				if (cFileSelected && strlen(cFileSelected) > 0) {

					uString checkfile = cFileSelected;
					pos = checkfile.RevFindStr(".agc");
					if (pos < 0)
						checkfile.Append(".agc");

					bool oksave = true;
					if (file_exists((char*)checkfile.GetStr())) {
						oksave = overWriteFileBox((char*)checkfile.GetStr());
					}
					if (oksave) {
						auto textToSave = m_editor->GetText();
						std::ofstream myfile;
						m_pCurrentFile->m_sFullPath = checkfile;
						m_pCurrentFile->m_bUseSaveAs = false;
						uString newname = "na";
						int pos = checkfile.RevFindStr("/");
						if( pos <= 0 ) pos = checkfile.RevFindStr("\\");
						if (pos > 0) {
							checkfile.SubString(newname, pos + 1);
							//m_pCurrentFile->m_sProjectName = "na-noproject";
							m_pCurrentFile->m_sName = newname;
							m_pCurrentFile->m_sEditName = m_pCurrentFile->m_sName;
							if (pref.bRemovePathFromTabName) {
								uString newTabname = m_pCurrentFile->m_sEditName;
								int ntb_pos = newTabname.RevFindStr("/");
								if (ntb_pos <= 0)
									ntb_pos = newTabname.RevFindStr("\\");
								if (ntb_pos > 0)
									newTabname.SubString(m_pCurrentFile->m_sEditName, ntb_pos + 1);
							}
							m_pCurrentFile->m_sEditName.Append("##");
							m_pCurrentFile->m_sEditName.Append(m_pCurrentFile->m_sProjectName);
							m_pCurrentFile->m_sEditName.Append(RandomString());
							cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
						}
#ifndef DISABLESAVE
						myfile.open(checkfile);
						myfile << textToSave;
						myfile.close();
#endif
						m_editor->filechanged = false;
						m_editor->firstrun = true;
					}
				}
				m_editor->saveonreturn = false;
			}
			else {

				auto textToSave = m_editor->GetText();
#ifndef DISABLESAVE
				std::ofstream myfile;
				myfile.open((char*)m_pCurrentFile->m_sFullPath.GetStr());
				myfile << textToSave;
				myfile.close();
				m_editor->filechanged = false;
#endif
			}
		}
		m_editor->saveonreturn = false;
	}

	return false;

}

void processproperties( int iProperties_type)
{
	if (iProperties_type == 0)
		return;
	if (iProperties_type == 1) {
		//Camera
		static float fCamera[4];
		static int iCamera[4];

		ImGui::Text("Camera 1:");
		fCamera[0] = agk::GetCameraX(iProperties_id);
		fCamera[1] = agk::GetCameraY(iProperties_id);
		fCamera[2] = agk::GetCameraZ(iProperties_id);
		ImGui::DragFloat3("Position", fCamera,1.0,-100000.0,100000);
		agk::SetCameraPosition(iProperties_id, fCamera[0], fCamera[1], fCamera[2]);

		fCamera[0] = agk::GetCameraAngleX(iProperties_id);
		fCamera[1] = agk::GetCameraAngleY(iProperties_id);
		fCamera[2] = agk::GetCameraAngleZ(iProperties_id);
		ImGui::DragFloat3("Angle", fCamera, 1.0, -100000.0, 100000);
		agk::SetCameraRotation(iProperties_id, fCamera[0], fCamera[1], fCamera[2]);

		iCamera[0] = agk::GetCameraFOV(iProperties_id);
		ImGui::SliderInt("Camera FOV", &iCamera[0], 0, 100);
		agk::SetCameraFOV(iProperties_id, iCamera[0]);
	}

	if (iProperties_type == 2) {
		//Directional light
		static ImVec4 dir_light = {1.0,1.0,1.0,1.0};
		static ImVec4 sky_color = {40* toOne,50* toOne,60* toOne,1.0 };
		static ImVec4 sky_horizon_color = { 168 * toOne,128 * toOne,64 * toOne,1.0 };

		static float fSunDirection[4] = {-1,-1,-1,1};
		static bool bDirVisible = 1;
		static bool bSunVisible = 0;
		static int iSunSize = 20;
		static int iSunHaloSize = 4;
		ImGui::Text("Directional light 1:");


		ImGui::ColorPicker3("MyDirColor##1", (float*)&dir_light, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		agk::SetSunColor(dir_light.x*256.0, dir_light.y*256.0, dir_light.z*256.0);
		agk::SetSkyBoxSunColor(dir_light.x*256.0, dir_light.y*256.0, dir_light.z*256.0);

		ImGui::DragFloat3("Sun Direction", fSunDirection, 0.05, -1.0, 1.0, "%.2f");
		agk::SetSunDirection(fSunDirection[0], fSunDirection[1], fSunDirection[2]);

		ImGui::Checkbox("Enable Sun Light", &bDirVisible);
		agk::SetSunActive(bDirVisible);

		ImGui::Checkbox("Display Sun", &bSunVisible);
		agk::SetSkyBoxSunVisible(bSunVisible);
		agk::SetSkyBoxVisible(bSunVisible);

		if (bSunVisible) {
			ImGui::ColorEdit3("Sky Box Color", (float*)&sky_color, 0);
			agk::SetSkyBoxSkyColor(sky_color.x*256.0, sky_color.y*256.0, sky_color.z*256.0);

			ImGui::ColorEdit3("Sky Horizon Color", (float*)&sky_horizon_color, 0);
			agk::SetSkyBoxHorizonColor(sky_horizon_color.x*256.0, sky_horizon_color.y*256.0, sky_horizon_color.z*256.0);

			ImGui::SliderInt("Sun Size", &iSunSize, 0, 100);
			ImGui::SliderInt("Sun Halo Size", &iSunHaloSize, 0, 100);
			agk::SetSkyBoxSunSize(iSunSize, iSunHaloSize); // 22,4
		}

	}
}


void sort_project_files_order(cProjectItem * searchProject)
{
	//Only sort the current open project.
	if (pCurrentSelectedProject == searchProject) {

		std::vector<std::string> sorted_project_files;
		extern ImGuiWindow* edit_area_parent;

		ImGuiContext* ctx = GImGui;
		ImGuiContext& g = *ctx;
		ImGuiDockContext* dc = ctx->DockContext;
		if (edit_area_parent) {
			ImGuiID parent_id = edit_area_parent->DockId;

			ImGuiDockNode* node = ImGui::DockContextFindNodeByID(ctx, parent_id);
			if (node && node->TabBar) {
				ImGuiTabBar* tab_bar = node->TabBar;

				for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
				{
					const ImGuiTabItem* tab = &tab_bar->Tabs[tab_n];
					if (tab->Window)
					{
						sorted_project_files.push_back(tab->Window->Name);
					}
				}
			}
		}

		cProjectItem::sProjectFiles *m_pSearchFile, *m_pSortedFile, *m_ptmpFile;

		//Sort files inside project.
		if (sorted_project_files.size() >= 1) {

			m_pSortedFile = NULL;
			m_ptmpFile = searchProject->m_pFirstFile;
			//std::sort(sorted_project_files.begin(), sorted_project_files.end(), NoCaseLess);
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
								searchProject->m_pFirstFile = m_pSearchFile;
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
			m_ptmpFile = searchProject->m_pFirstFile;
			m_pSearchFile = m_ptmpFile;
			while (m_pSearchFile) {
				m_pSearchFile->m_pSortedNext = m_pSearchFile->m_pNext;
				m_pSearchFile = m_pSearchFile->m_pNext;
			}


		}
		sorted_project_files.clear();
	}
}
