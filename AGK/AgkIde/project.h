#ifndef _H_AGK_IDEPROJECT_
#define _H_AGK_IDEPROJECT_

#include "agk.h"
#include "TextEditor.h"
#include "2DScene.h"
#include "gui.h"

#include "../Broadcaster/AGKBroadcaster/AGKBroadcaster.h"
using namespace AGKBroadcaster;

//Debugger info per project.
#define MAXVALUESIZE 2048
struct WatchValues
{
	char value[MAXVALUESIZE + 1];
	char newvariable[MAX_PATH + 1];
	bool bHoverVariable;
	int lineno;
};

class cProjectItem
{
public:

	struct sProjectFiles {
		uString m_sName;
		uString m_sProjectName;
		uString m_sEditName;
		uString m_sFullPath;
		bool m_bOpenForEdit;
		bool m_bUseSaveAs;
		UINT m_iFlags;
		int m_id;
		TextEditor * m_editor;
		MediaScene * m_scene;
		sProjectFiles * m_pNext;
		sProjectFiles * m_pSortedNext;
	};

	uString m_sProjectFile;
	uString m_sProjectRealFile;

	uString m_sProjectFileFullPath;
	sProjectFiles * m_pFirstFile;
	cFolderItem *m_pMediaFolder;
	std::map<std::string, WatchValues> debugWatch;

	//Settings.

	//[files]
	//FILE_NAME_0=0;main.agc;8420;0

	//[project]
	//description =
	uString m_sdescription;

	UINT iFlags;
	int m_iProjectCount;
	bool m_bDisplayUnfolded;
	bool m_bDisplayCollapsed;
	int m_id;

	//Android settings.
	char apk_alias[MAX_PATH];
	char apk_icon_path[MAX_PATH];
	char apk_new_icon_path[MAX_PATH];
	char apk_notif_icon_path[MAX_PATH];
	char apk_app_name[MAX_PATH];
	int  apk_app_type = 0;
	char game_circle_api_key[2048];
	char apk_keystore_path[MAX_PATH];
	int  apk_orientation = 0;
	char apk_output_path[MAX_PATH];
	char apk_ouya_icon_path[MAX_PATH];
	char apk_package_name[MAX_PATH];
	int  apk_permission_flags = 0;
	char apk_play_app_id[MAX_PATH];
	char apk_admob_app_id[MAX_PATH];
	char apk_snapchat_client_id[MAX_PATH];
	int  apk_sdk_version = 0;
	char apk_version_name[MAX_PATH];
	int  apk_version_number = 1;
	char apk_firebase_config_path[MAX_PATH];
	char apk_url_scheme[MAX_PATH];
	char apk_deep_link[MAX_PATH];
	int  apk_arcore = 0;

	//IOS Settings.
	char ipa_app_icon_path[MAX_PATH];
	char ipa_app_name[MAX_PATH];
	char ipa_build_number[MAX_PATH];
	int  ipa_device_type = 0;
	char ipa_facebook_id[MAX_PATH];
	int ipa_orientation = 0;
	char ipa_output_path[MAX_PATH];
	char ipa_prov_profile_path[MAX_PATH];
	char ipa_splash_logo[MAX_PATH];
	char ipa_splash_color[MAX_PATH];
	int  ipa_uses_ads = 0;
	char ipa_version_number[MAX_PATH];
	char ipa_firebase_config_path[MAX_PATH];
	char ipa_url_scheme[MAX_PATH];
	char ipa_deep_link[MAX_PATH];
	char ipa_admob_app_id[MAX_PATH];
	char ipa_snapchat_client_id[MAX_PATH];


	char html_output_path[MAX_PATH];
	int  html_commands_used = 0;
	int  html_dynamic_memory = 0;

	cProjectItem *m_pNext;

	cProjectItem() { m_pNext = 0; iFlags = 0; m_iProjectCount = 0; m_pMediaFolder = NULL; m_pFirstFile = NULL;  m_pNext = NULL; }
	~cProjectItem() { }
};

#define MAXOPNEDPROJECTS 50
bool OpenProject(void);
void RemoveProject(cProjectItem * removeProject);
void ProjectMoveDown(cProjectItem * moveProject);
void ProjectMoveUp(cProjectItem * moveProject);
void SaveProjectFile(cProjectItem * mProject);
bool CompileProject(cProjectItem * mProject, bool run, bool broadcast, bool debug, char *useIP = NULL);
void SaveAllOpenProjectEditedFiles(void);
void CloseAllOpenProjectEditedFiles(void);
void removeFileFromProject(cProjectItem * mProject, cProjectItem::sProjectFiles * m_pSearchFile);
bool SaveAllEditedProjectFiles(cProjectItem * mProject);
void SaveAllProjectsEditedFiles(void);
void CreateNewProjectMainFile(char *newfile, char *project);
void LoadAgkIdeIni(int initPart=1);
void SaveAgkIdeIni(void);
void ResetPreferences(bool resetall=true);
const char * GetLastDebugMessageA();
int GetDebugMessageCountA();
int ResetDebugMessageLogA();
void addAllBreakPoints(char *projectname);
void InitBroadcaster(void);
void CreateNewProject(void);
void CreateNewScene(void);
void CreateNewProjectFile(void);
void CheckKeyboardShortcuts(void);

void LoadProjectFile(char * cFileSelected);
void SaveProjectFile(cProjectItem * mProject);

cProjectItem::sProjectFiles * CreateNewAGCFile(char *cFileSelected = NULL, char *NameIt = NULL );
void RemoveAGKFile(cProjectItem::sProjectFiles * m_pCurrentFile);


/*
GGLoader.agk:

[projectfiles]
FILE_0=main.agc;-1
FILE_1=ggfunc.agc;-1
FILE_2=ggskybox.agc;-1
FILE_3=ggwater.agc;-1
FILE_4=ggguns.agc;-1
FILE_5=ggshadersetup.agc;-1
FILE_6=ggcar.agc;-1
FILE_7=edit.agc;-1
FILE_8=math.agc;-1
FILE_9=envmap.agc;-1
FILE_10=ggparticle.agc;-1
FILE_11=physics.agc;-1
FILE_12=shaders.agc;-1
FILE_13=mygame.agc;-1

[files]
FILE_NAME_0=0;main.agc;8420;0
FILE_NAME_1=1;ggfunc.agc;161812;1
FILE_NAME_2=0;ggskybox.agc;0;3

[project]
description=

[apk_settings]
alias=
app_icon_path=
app_name=testfps
app_type=0
game_circle_api_key=
keystore_path=D:\\AGK2\\release.keystore
orientation=0
output_path=D:\\AGK2\\testfps.apk
ouya_icon_path=
package_name=com.plemsoft.testfps
permission_flags=7
play_app_id=
sdk_version=1
shared_user_id=
version_name=1.0.0
version_number=1
notif_icon_path=
firebase_config_path=
arcore=0
url_scheme=
deep_link=

[ipa_settings]
app_icon_path=
app_name=
build_number=
device_type=0
facebook_id=
orientation=0
output_path=/Users/pe/AGK2/ggloader/GGLoader.ipa
prov_profile_path=
splash_logo=
splash_color=
uses_ads=0
version_number=
firebase_config_path=
url_scheme=
deep_link=

[html5_settings]
commands_used=1
dynamic_memory=1
output_path=D:\\AGK2\\ggloader\\tbe

*/

#endif



