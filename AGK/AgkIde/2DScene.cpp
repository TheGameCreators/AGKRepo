//import test project.

#include "agk.h"
#include "Ide.h"
#include "gui.h"
#include "TextEditor.h"
#include "2DScene.h"
#include "project.h"


static MediaScene::MediaObject * test;
static char newgrouparray[MAX_PATH];


MediaScene::MediaScene()
	: bGridOn(true)
	, bAGKBorder(true)
	, bDisplayAsText(false)
	, fGridX(32)
	, fGridY(32)
	, bFirstRun(true)
{
	//Set Defaults.
	Resolution.clear();
	
	Resolution.insert(std::make_pair(std::string("P 360x640"), Resolutions{ 360, 640,true, "Most Used 2019 Mobile" }));
	Resolution.insert(std::make_pair(std::string("P 640x960"), Resolutions{ 640, 960,true, "iPhone 4/4S" }) );
//	Resolution.insert(std::make_pair(std::string("P 640x1136"), Resolutions{ 640, 1136,true, "iPhone 5S" }));
	Resolution.insert(std::make_pair(std::string("P 768x1024"), Resolutions{ 768, 1024,true, "iPad/iPad 2/iPad Mini" }));
	Resolution.insert(std::make_pair(std::string("P 720x1280"), Resolutions{ 720, 1280,true, "Samsung Galaxy S3" }));
	Resolution.insert(std::make_pair(std::string("P 750x1134"), Resolutions{ 750, 1134,true, "iPhone 6/7/8" }));
	Resolution.insert(std::make_pair(std::string("P 1080x1920"), Resolutions{ 1080, 1920,true, "iPhone 8 Plus" }));
//	Resolution.insert(std::make_pair(std::string("P 1200x1920"), Resolutions{ 1200, 1920,true, "Kindle Fire HDX" }));
	Resolution.insert(std::make_pair(std::string("P 1536x2048"), Resolutions{ 1536, 2048,true, "iPad Air/2017/2018" }));
	Resolution.insert(std::make_pair(std::string("P 1125x2436"), Resolutions{ 1125, 2436,true, "iPhone X" }));

	Resolution.insert(std::make_pair(std::string("L 640x360"), Resolutions{ 640, 360,false, "Most Used 2019 Mobile" }));
	Resolution.insert(std::make_pair(std::string("L 1366x768"), Resolutions{ 1366,768,false, "Most Used 2019 Desktop" }));
	Resolution.insert(std::make_pair(std::string("L 960x640"), Resolutions{ 960, 640,false, "iPhone 4/4S" }));
//	Resolution.insert(std::make_pair(std::string("L 1136x640"), Resolutions{ 1136, 640,false, "iPhone 5S" }));
	Resolution.insert(std::make_pair(std::string("L 1024x768"), Resolutions{ 1024, 768,false, "iPad/iPad 2/iPad Mini" }));
	Resolution.insert(std::make_pair(std::string("L 1280x720"), Resolutions{ 1280, 720,false, "Samsung Galaxy S3" }));
	Resolution.insert(std::make_pair(std::string("L 1134x750"), Resolutions{ 1134, 750,false, "iPhone 6/7/8" }));
	Resolution.insert(std::make_pair(std::string("L 1920x1080"), Resolutions{ 1920, 1080,false, "iPhone 8 Plus" }));
//	Resolution.insert(std::make_pair(std::string("L 1920x1200"), Resolutions{ 1920, 1200,false, "Kindle Fire HDX" }));
	Resolution.insert(std::make_pair(std::string("L 2048x1536"), Resolutions{ 2048, 1536,false, "iPad Air/2017/2018" }));
	Resolution.insert(std::make_pair(std::string("L 2436x1125"), Resolutions{ 2436, 1125,false, "iPhone X" }));

	ImVec4 *style_colors = ImGui::GetStyle().Colors;
	vGridColor = ImVec4(style_colors[ImGuiCol_MenuBarBg]);
	vGridColor.x *= 1.25f;
	if (vGridColor.x > 1.0f) vGridColor.x = 1.0f;
	vGridColor.y *= 1.25f;
	if (vGridColor.y > 1.0f) vGridColor.y = 1.0f;
	vGridColor.z *= 1.25f;
	if (vGridColor.z > 1.0f) vGridColor.z = 1.0f;

	strcpy(newgrouparray, "");

	for (int i = 0; i < 11; i++) {
		bLayerVisible[i] = true;
		iLayerDepth[i] = 10000- i * 1000;
		iLayerOpacity[i] = 255;
		sLayerName[i] = "--";
	}

	sLayerName[0] = "Background";
	sLayerName[3] = "Middle-Back";
	sLayerName[5] = "Middle";
	sLayerName[7] = "Middle-Front";
	sLayerName[10] = "Front";

	for (int i = 0; i < MAXUSERFONTS; i++) {
		customfonts[i] = NULL;
	}

	iCurrentSelectedLayer = 5;

}


MediaScene::~MediaScene()
{
}


struct TextFilters {
	static int TextFiltersVariable(ImGuiInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", (char)data->EventChar)) return 0; return 1; }
};



int pecasepos(const char *arg1, const char *arg2);
const char *pestrcasestr(const char *arg1, const char *arg2);
extern bool ide_force_rendering_delayed;
extern Preferences pref;
extern int iToolbarImages[30];
extern int iToolbarImagesDown[30];
extern cProjectItem * pCurrentSelectedProject;
extern cProjectItem * renderingProject;
extern int agkbutton_image;
extern int agkbuttondown_image;
extern bool scene_editor_testmode;
extern ImFont* defaultfont;


bool MediaScene::Render(const char* aTitle, TextEditor * m_editor, void * pTmp , MediaScene * m_pMainScene )
{
	cProjectItem::sProjectFiles * m_pCurFile = (cProjectItem::sProjectFiles *) pTmp;

	if (!m_pCurFile)
		return(true);
	if( !m_editor) 
		return(true);
	if(!aTitle)
		return(true);
	if (!m_pMainScene)
		return(true);

	m_SceneEditor = m_editor;

	bIsAnythingHovered = false;
	bIsAnythingSelected = false;

	bool wtabvisible = ImGui::windowTabVisible();

	extern int keyboard_layout;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	auto ctrl = (io.ConfigMacOSXBehaviors && keyboard_layout == 1) ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
	auto shift = io.KeyShift;
	if (keyboard_layout == 0) {
		ctrl = io.KeyCtrl;
		alt = io.KeyAlt;
	}
	bool is_osx = io.ConfigMacOSXBehaviors;

	//ImDrawList* draw_list = ImGui::GetWindowDrawList();

	if (m_pMainScene->m_pCurrentScene == NULL)
		m_pMainScene->m_pCurrentScene = m_pMainScene;
	if (m_pMainScene->m_pCurrentScene == m_pMainScene && m_pMainScene->m_pCurrentScene->m_sSceneName.GetLength() <= 0) {
		uString newname = "Scene_0",tmp;

		tmp = m_pMainScene->m_pCurrentScene->m_sSceneFullPath;
		tmp.ReplaceStr("\\", "/");
		int pos = tmp.RevFindStr("/");
		if (pos > 0) {
			tmp.SubString(newname, pos + 1);
		}
		newname.ReplaceStr(".scene", "");
		m_pMainScene->m_pCurrentScene->m_sSceneName = newname;
	}
	
	bool bCloseScene = false;
	static int iAction = 0;
	int iToolHeight = 64;
	int iBorderSize = 8;

	if (iDelayAction > 0) {
		iAction = iDelayAction;
		iDelayAction = 0;
	}
	if (bFirstRun) {
		if (m_pMainScene->m_pCurrentScene == m_pMainScene) {
			LoadCode(m_pMainScene, m_editor);
		}
		//TestSprite(); // load a test sprite.
		iAction = 1;
		bFirstRun = false;
		//ImGui::SetWindowFocus("Properties");
	}

	centerSceneX = (iContentWide * 0.5f) - (iScreenWidth *0.5f);
	centerSceneY = iContentHeight * 0.5f - (iScreenHeight *0.5f);

//	ImGui::Begin(aTitle, &bCloseScene, 0);
	ImVec2 ws = ImGui::GetWindowSize(), wso;
	wso = ws;
	ws.y -= iToolHeight;
	ws.y -= iBorderSize;
	ws.y -= iBorderSize;
	ImVec2 wp = ImGui::GetWindowPos();

	ImVec4 *style_colors = ImGui::GetStyle().Colors;

	int iOldRounding = (int) ImGui::GetStyle().WindowRounding;
	ImGui::GetStyle().WindowRounding = 0.0f;

	//ImGui::GetWindowDrawList()->AddRectFilled(wp - ImVec2(2, 2), wp + ImVec2(ws.x, iToolHeight), ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_MenuBarBg]))); // ImGuiCol_MenuBarBg,ImGuiCol_WindowBg
	//ImGui::Dummy(ImVec2(ws.x, iToolHeight));

	if (ImGui::BeginMenuBar())
	{
		//Menu active check if we selected this window.
		if (agk::GetPointerPressed()) {
			float mx, my;
			mx = agk::GetPointerX();
			my = agk::GetPointerY();
			if (mx >= ImGui::GetWindowPos().x && my >= ImGui::GetWindowPos().y && mx <= (ImGui::GetWindowPos().x + ImGui::GetWindowSize().x) && my <= (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y))
			{
				ImGuiWindow* agk_window;
				ImGuiContext& g = *GImGui;
				extern TextEditor * m_ActiveEditor;
				extern MediaScene * m_ActiveScene;
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


		if (m_pSelectedImage) {
			if (ImGui::Button("Exit Draw Mode")) {
				m_pSelectedImage = NULL;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Exit Draw Mode.");
		}
		else {

			//ImGuiButtonFlags_Disabled
			if (m_pCurFile->m_scene->bDisplayAsText) {
				if (ImGui::Button("2D")) {
					m_pCurFile->m_scene->bDisplayAsText = false;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Switch to 2D Visual Editor.");
			}

//			ImGui::SameLine();

			if (!m_pCurFile->m_scene->bDisplayAsText) {
				if (ImGui::Button("Script")) {
					GenerateCode(m_pMainScene, m_editor);
					m_pCurFile->m_scene->bDisplayAsText = true;
					m_editor->SetReadOnly(true);
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Switch to Text Mode.");
			}
		}
		ImGui::SameLine();

		if (ImGui::Button("Save")) {
			GenerateCode(m_pMainScene, m_editor, true);
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Save Changes.");

		ImGui::SameLine();

		ImGui::Separator();
		ImGui::SameLine();

		if (bAGKBorder)
			ImGui::Text(ICON_MD_BORDER_CLEAR);
		else
			ImGui::Text(ICON_MD_BORDER_OUTER);
		ImGui::ToggleButton("AGKBorder:", &bAGKBorder);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Device Screen Outline On/Off.");

		if (bGridOn)
			ImGui::Text(ICON_MD_GRID_OFF);
		else
			ImGui::Text(ICON_MD_GRID_ON);
		ImGui::ToggleButton("Grid:", &bGridOn);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Grid On/Off");

		if (bGridOn) {
			ImGui::SameLine();
			ImGui::PushItemWidth(40);
			ImGui::InputFloat("X", &fGridX, 0.0f, 0.0f, 2);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Value");
			if (fGridX < 4.0f)
				fGridX = 4.0f;
			if (fGridY < 4.0f)
				fGridY = 4.0f;
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PushItemWidth(40);
			ImGui::InputFloat("Y", &fGridY, 0.0f, 0.0f, 2);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Y Value");

			ImGui::PopItemWidth();
		}

		if (ImGui::Button(" Reset View ")) {
			zoomAll = RESETZOOM;
			iAction = 1;
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset Current View");


		ImGui::PushItemWidth(9.5f*ImGui::GetFontSize());
		if (ImGui::BeginCombo("##comboResolutiuon", cResolutionMenuText, ImGuiComboFlags_HeightLargest)) //ImGuiComboFlags_HeightLarge
		{
			int count = 100;
			for (std::map<std::string, Resolutions>::iterator it = m_pMainScene->Resolution.begin(); it != m_pMainScene->Resolution.end(); ++it)
			{
				uString Entry = it->second.m_sName;
				if (Entry.GetLength() > 0) {
					char cMenuText[MAX_PATH];

					if (Entry.CompareTo("Custom") == 0)
						sprintf(cMenuText, "C: %dx%d (%.2f)   %.40s ", (int)it->second.m_iWidth, (int)it->second.m_iHeight, (float)it->second.m_iWidth / it->second.m_iHeight, it->second.m_sName.GetStr());
					else if (it->second.m_bOrintation)
						sprintf(cMenuText, "P: %dx%d (%.2f)   %.40s ", (int)it->second.m_iWidth, (int)it->second.m_iHeight, (float)it->second.m_iWidth / it->second.m_iHeight, it->second.m_sName.GetStr());
					else
						sprintf(cMenuText, "L: %dx%d (%.2f)   %.40s ", (int)it->second.m_iWidth, (int)it->second.m_iHeight, (float)it->second.m_iWidth / it->second.m_iHeight, it->second.m_sName.GetStr());
					bool is_selected = false;
					if (strcmp(cMenuText, cResolutionMenuText) == 0)
						is_selected = true;

					//ImGui::PushItemWidth(270.0f);
					ImGui::PushItemWidth(18*ImGui::GetFontSize());
					if (ImGui::Selectable(cMenuText, is_selected, 0, ImVec2(18 * ImGui::GetFontSize(), 0))) {
						//selected goto lineno.
						strcpy(cResolutionMenuText, cMenuText);
						iScreenWidth = it->second.m_iWidth;
						iScreenHeight = it->second.m_iHeight;
						zoomAll = RESETZOOM;
						iDelayAction = 1;
						m_editor->filechanged = true;
					}
					ImGui::PopItemWidth();

					bool baseres = false;
					if (!bFollowDeviceResolution && iBaseWidth == it->second.m_iWidth && iBaseHeight == it->second.m_iHeight)
						baseres = true;
					count++;
					ImGui::PushID(count);
					ImGui::SameLine();
					ImGui::SetItemAllowOverlap();
					//ImGui::PushItemWidth(-30);
					bool changeres = baseres;
					if (ImGui::Checkbox("Base", &changeres)) {
						if (baseres && !changeres) {
							//change base resolution.
							if (askBox("Are you sure you want to remove the base resolution ?\nThis will change your setup to follow the device resolution.", "Set Base Resolution")) {
								//switch to follow device resolution.
								bFollowDeviceResolution = true;
								m_editor->filechanged = true;

							}
						}
						else {
							if (changeres) {
								if (askBox("Are you sure you want to change base resolution ?\nThis will change your current scene layout.\nNote: You can always switch back to the original base resolution.", "Set Base Resolution")) {
									//switch base
									bFollowDeviceResolution = false;
									iBaseWidth = it->second.m_iWidth;
									iBaseHeight = it->second.m_iHeight;
									//change res.
									strcpy(cResolutionMenuText, cMenuText);
									iScreenWidth = it->second.m_iWidth;
									iScreenHeight = it->second.m_iHeight;
									zoomAll = RESETZOOM;
									iDelayAction = 1;
									m_editor->filechanged = true;
								}
							}
						}
					}
					//ImGui::PopItemWidth();
					ImGui::PopID();

				}

			}

			if (ImGui::Selectable("Add New Resolution...", false)) {
				add_new_resolution = true;
			}

			ImGui::EndCombo();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Resolution");

		ImGui::PopItemWidth();


		//		ImGui::SameLine();
		//		if (ImGui::Button("Scene: +")) {
		//			int scenes = 1;
		//			MediaScene * m_pSearchScene;
		//			m_pSearchScene = m_pMainScene;
		//			while (m_pSearchScene->m_pNextScene) {
		//				scenes++;
		//				m_pSearchScene = m_pSearchScene->m_pNextScene;
		//			}
		//			MediaScene * pNewScene = new MediaScene();
		//			pNewScene->m_sSceneName = "Scene_";
		//			pNewScene->m_sSceneName.AppendInt(scenes);
		//			m_pSearchScene->m_pNextScene = pNewScene;
		//			m_pMainScene->m_pCurrentScene = pNewScene;
		//			strcpy(cSceneMenuText, pNewScene->m_sSceneName.GetStr());
		//		}
		//		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Create New Scene");

		ImGui::SameLine();
		ImGui::PushItemWidth(6 * ImGui::GetFontSize());
		char tmpbuf[MAX_PATH];
		strcpy(tmpbuf, m_sSceneName.GetStr());
		if (ImGui::InputText("##thesscenename", &tmpbuf[0], MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue, TextFilters::TextFiltersVariable)) {
			if (strlen(tmpbuf) > 0) {
				m_sSceneName = tmpbuf;
			}
			m_editor->filechanged = true;
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Scene Name");

		ImGui::PushItemWidth(20);
		ImGui::SameLine();
		if (ImGui::BeginCombo("##cSceneMenuText", cSceneMenuText, ImGuiComboFlags_HeightLarge | ImGuiComboFlags_NoPreview))
		{
			MediaScene * m_pSearchScene;
			m_pSearchScene = m_pMainScene;
			int scenes = 0;
			while (m_pSearchScene) {
				char cMenuText[MAX_PATH];
				uString Entry = m_pSearchScene->m_sSceneName;
				if (!m_pSearchScene->bDisabled && Entry.GetLength() > 0) {
					sprintf(cMenuText, "%s", m_pSearchScene->m_sSceneName.GetStr());
					bool is_selected = false;
					if (strcmp(cMenuText, cSceneMenuText) == 0)
						is_selected = true;
					if (m_pSearchScene != m_pMainScene)
						ImGui::PushItemWidth(120.0f);
					if (ImGui::Selectable(cMenuText, is_selected, 0, ImVec2(120, 0))) {
						//selected 
						strcpy(cSceneMenuText, cMenuText);
						m_pMainScene->m_pCurrentScene = m_pSearchScene;
						m_pSearchScene->iDelayAction = 1;
					}
					if (m_pSearchScene != m_pMainScene)
						ImGui::PopItemWidth();
					if (m_pSearchScene != m_pMainScene) {
						ImGui::SameLine();
						ImGui::PushItemWidth(20.0f);
						sprintf(cMenuText, "-##scenedelete%d", scenes);
						ImGui::SetItemAllowOverlap();
						if (ImGui::Button(cMenuText)) {
							if (askBox("Are you sure ?", "Delete scene")) {
								//Delete entry.
								m_pSearchScene->bDisabled = true;
								strcpy(cSceneMenuText, m_pSearchScene->m_sSceneName.GetStr());
								m_pMainScene->m_pCurrentScene = m_pMainScene;
							}
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Delete Scene");

						ImGui::PopItemWidth();
					}
					scenes++;
				}
				m_pSearchScene = m_pSearchScene->m_pNextScene;
			}
			if (ImGui::Selectable("Add New Scene...", false)) {
				int scenes = 1;
				MediaScene * m_pSearchScene;
				m_pSearchScene = m_pMainScene;
				while (m_pSearchScene->m_pNextScene) {
					scenes++;
					m_pSearchScene = m_pSearchScene->m_pNextScene;
				}
				MediaScene * pNewScene = new MediaScene();
				pNewScene->m_sSceneName = "Scene_";
				pNewScene->m_sSceneName.AppendInt(scenes);
				m_pSearchScene->m_pNextScene = pNewScene;
				m_pMainScene->m_pCurrentScene = pNewScene;
				strcpy(cSceneMenuText, pNewScene->m_sSceneName.GetStr());
			}

			ImGui::EndCombo();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Scene");

		ImGui::SameLine();
		ImVec4 colorsel = { 0,0,0,0 };
		if (iSceneBackColor[0] > 0)
			colorsel.x = iSceneBackColor[0] / 255.0f;
		if (iSceneBackColor[1] > 0)
			colorsel.y = iSceneBackColor[1] / 255.0f;
		if (iSceneBackColor[2] > 0)
			colorsel.z = iSceneBackColor[2] / 255.0f;
		if (iSceneBackColor[3] > 0)
			colorsel.w = iSceneBackColor[3] / 255.0f;

		if (ImGui::ColorEdit4("##agkbackgroundpicker", (float*)&colorsel, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs))//ImGuiColorEditFlags_AlphaPreview
		{
			iSceneBackColor[0] = (int) (colorsel.x * 255);
			iSceneBackColor[1] = (int) (colorsel.y * 255);
			iSceneBackColor[2] = (int) (colorsel.z * 255);
			iSceneBackColor[3] = (int) (colorsel.w * 255);
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Background Color");

		ImGui::ColorEdit4("##agkgridcolorpicker", (float*)&vGridColor, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Color");



		//ImGui::SameLine();
		//ImGui::Text("MediaScene: %s", aTitle);

		ImGui::EndMenuBar();
	}

//	ImGui::SameLine();
//	//ImGui::Text("MediaScene: %s", aTitle);
	//ImGuiWindowFlags_NoScrollbar
	ImGui::SetNextWindowContentSize(ImVec2((float)iContentWide, (float)iContentHeight));
	ImGui::BeginChild("scene_scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	wtabvisible = ImGui::windowTabVisible(); //PE 26-03-2019

	ImGui::PushAllowKeyboardFocus(true);
	//Global Keys.

	if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(27)) { //esc exit draw mode.
		if(m_pSelectedImage)
			m_pSelectedImage = NULL;
	}

	if (ImGui::IsWindowFocused() && wtabvisible)
	{
		if (ctrl == pref.bUndoCtrl && shift == pref.bUndoShift && alt == pref.bUndoAlt && ImGui::IsKeyPressed(pref.iUndoKey)) {
			Undo();
		}
		else if (!ctrl && !shift && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace))) {
			Undo();
		}
		else if (ctrl == pref.bRedoCtrl && shift == pref.bRedoShift && alt == pref.bRedoAlt && ImGui::IsKeyPressed(pref.iRedoKey)) {
			Redo();
		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
			if (m_pSelectedObject) {
				Delete();
			}

			//To risky ?
/*
			if (mMultipleSelectActive) {
				MediaScene::MediaObject * m_pSearchObj = NULL;
				m_pSearchObj = w->m_pFirstObject;

				while (m_pSearchObj) {
					if (m_pSearchObj->bIsSelected) {
						//m_pSearchObj->m_OldPosition = m_pSearchObj->m_Position;
						w->AddUndo(m_pSearchObj, false);
						agk::SetSpriteVisible(m_pSearchObj->m_iSprite, 0);
						m_pSearchObj->m_bRemoved = true;
						ide_force_rendering_delayed = true;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}
*/

		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(27) ) { //esc
			//Reset.
			m_pSelectedImage = NULL;
		}
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert))) {
			if(m_pSelectedObject)
				CopyObject(m_pSelectedObject);
		}
		else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert))) {
			PasteObject();
			m_editor->filechanged = true;
		}
		//MAC: Emulate pc "insert" by using "delete" key instead. only for CTRL+SHIFT operations.
		else if (is_osx && keyboard_layout == 0 && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
			if (m_pSelectedObject)
				CopyObject(m_pSelectedObject);
		}
		else if (is_osx && keyboard_layout == 0 && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
			PasteObject();
			m_editor->filechanged = true;
		}
		//
		else if (ctrl == pref.bCopyCtrl && shift == pref.bCopyShift && alt == pref.bCopyAlt && ImGui::IsKeyPressed(pref.iCopyKey)) {
			if (m_pSelectedObject)
				CopyObject(m_pSelectedObject);
		}
		else if (ctrl == pref.bPasteCtrl && shift == pref.bPasteShift && alt == pref.bPasteAlt && ImGui::IsKeyPressed(pref.iPasteKey)) {
			PasteObject();
		}
		if (!ctrl && !alt && m_pSelectedObject && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow))) {
			m_pSelectedObject->m_Position.x--;
			m_editor->filechanged = true;
		}
		if (!ctrl && !alt && m_pSelectedObject && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow))) {
			m_pSelectedObject->m_Position.x++;
			m_editor->filechanged = true;
		}
		if (!ctrl && !alt && m_pSelectedObject && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {
			m_pSelectedObject->m_Position.y--;
			m_editor->filechanged = true;
		}
		if (!ctrl && !alt && m_pSelectedObject && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
			m_pSelectedObject->m_Position.y++;
			m_editor->filechanged = true;
		}


		if (agk::GetPointerState()) {
			extern MediaScene * m_ActiveScene;
			m_ActiveScene = this;
			if (!m_ActiveScene)
				m_ActiveScene = m_pMainScene->m_pCurrentScene;
		}

	}


	//####################
	//#### Scene Area ####
	//####################

	ImGuiWindow *pwin = ImGui::GetCurrentWindow();
	float scroll_bar_x = pwin->ScrollbarSizes.x;
	float scroll_bar_y = pwin->ScrollbarSizes.y;

	if (iAction == 1) {
		ImGui::SetScrollX(centerSceneX);
		ImGui::SetScrollY(centerSceneY);
	}

	iToolHeight = 0;
	ws = ImGui::GetWindowSize(), wso;
	wso = ws;
	ws.y -= iToolHeight;
	ws.y -= iBorderSize;
	ws.y -= iBorderSize;
	ws.y -= scroll_bar_y;
	ws.x -= iBorderSize;
	ws.x -= iBorderSize;
	ws.x -= scroll_bar_x;
	if (bDisplayPhone) {
		ws.x -= 50.0f;
		ws.y -= 50.0f;
	}
	wp = ImGui::GetWindowPos();
	wp.x -= ImGui::GetScrollX();
	wp.y -= ImGui::GetScrollY();


	extern int iDeviceWidth, iDeviceHeight;

	if (!agk::GetImageExists(iRenderTarget))
		iRenderTarget = agk::CreateRenderImage(iScreenWidth, iScreenHeight, 0, 0);

	if (iScreenWidth != agk::GetImageWidth(iRenderTarget) || iScreenHeight != agk::GetImageHeight(iRenderTarget)) {
		if (agk::GetImageExists(iRenderTarget))
			agk::DeleteImage(iRenderTarget);
		iRenderTarget = agk::CreateRenderImage(iScreenWidth, iScreenHeight, 0, 0);
	}

	cImage *pImage = agk::GetImagePtr(iRenderTarget);
	int iImgW = iScreenWidth;
	int iImgH = iScreenHeight;

	if ((ws.x / iImgW) < (ws.y / iImgH))
		fRatio = ws.x / iImgW;
	else
		fRatio = ws.y / iImgH;

	//Scene area try to fit 1.0f ratio.
	if (fRatio > 1.0f)
		fRatio = 1.0f;
	if (fRatio < 0.25f)
		fRatio = 0.25f;
	fRatioAuto = fRatio;
	fRatio += zoomAll;
	//Include zoom.
	if (fRatio > 7.0f)
		fRatio = 7.0f;
	if (fRatio < 0.125f) //0.25f
		fRatio = 0.125f;

	//Get aspect ratio.
	GetAspect();


//	if (pImage)
//		myTextureID = pImage->GetTextureID() + 200000; // 100000+ = No Color Array , 200000+ = No transparent

	fCenterX = (ws.x - iImgW*fRatio) * 0.5f;
	fCenterY = (ws.y - iImgH*fRatio) * 0.5f;
	fCenterX += iBorderSize;
	fCenterY += (iToolHeight + iBorderSize );
	if (bDisplayPhone) {
		fCenterX += 25.0f;
		fCenterY += 25.0f;
	}

	ImGui::SetCursorPos(ImVec2(fCenterX + centerSceneX, fCenterY + centerSceneY)); //TitleSize

	//Draw screen area.
	ImVec2 a[4];
	if (bDisplayPhone) {
		float pRat =  (float) iScreenWidth / 1024.0f;
		float linesize = 80.0f * fRatio * pRat;
		float rounding = 40.0f * fRatio * pRat;
		a[0].x = wp.x + centerSceneX + (fCenterX - linesize);
		a[0].y = wp.y + centerSceneY + (fCenterY - linesize);
		a[2].x = wp.x + centerSceneX + (fCenterX + (iImgW*fRatio) + linesize);
		a[2].y = wp.y + centerSceneY + (fCenterY + (iImgH*fRatio) + linesize);

		ImGui::GetWindowDrawList()->AddRectFilled(a[0], a[2], ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_Separator])) , rounding);//, 20.0f

	}
	if (!bTestMode && bAGKBorder) {
		float linesize = 2.0f;
		a[0].x = wp.x + centerSceneX + (fCenterX - 2);
		a[0].y = wp.y + centerSceneY + (fCenterY - 2);
		a[1].x = wp.x + centerSceneX + (fCenterX + (iImgW*fRatio) + 2);
		a[1].y = wp.y + centerSceneY + (fCenterY - 2);
		a[2].x = wp.x + centerSceneX + (fCenterX + (iImgW*fRatio) + 2);
		a[2].y = wp.y + centerSceneY + (fCenterY + (iImgH*fRatio) + 2);
		a[3].x = wp.x + centerSceneX + (fCenterX - 2);
		a[3].y = wp.y + centerSceneY + (fCenterY + (iImgH*fRatio) + 2);
		ImGui::GetWindowDrawList()->AddQuad(a[0], a[1], a[2], a[3], ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram])), linesize+ linesize);
	}

	//If we use multiply scene editors we need to disable the sprites.
	MediaScene::MediaObject * m_pSearchObj = NULL;
	m_pSearchObj = m_pFirstObject;
	mMultipleSelectActive = false;
	iActiveSelections = 0;
	while (m_pSearchObj) {
		if (!m_pSearchObj->m_bRemoved) {
			if (m_pSearchObj->bIsSelected)
				iActiveSelections++;
			
			if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
				agk::SetSpriteVisible(m_pSearchObj->m_iSprite, 1);

				if (m_pSearchObj->bEnablePhysics) {
					if (bTestMode) {
						ide_force_rendering_delayed = true; //Update every frame when physics and testmode.

						if (!m_pSearchObj->bPhysicsInit) {

							if (bUsePercentage) {
								agk::SetVirtualResolution(100, 100);
								float aspect = ((float)iBaseWidth / (float)iBaseHeight);
								agk::SetDisplayAspect(aspect);
							}
							//agk::DeleteSprite(m_pSearchObj->m_iSprite);
							//m_pSearchObj->m_iSprite = agk::CreateSprite(m_pSearchObj->m_iImageId);
							UpdateObject(m_pSearchObj, true);

//							if (bUsePercentage) {
//								//Physics do not follow the normal percentage system , compensate.
//								agk::SetSpritePosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
//								agk::SetSpriteSize(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Size.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Size.y / (float)iBaseHeight)*100.0f);
//							}

							agk::SetPhysicsMaxPolygonPoints(iPhysicsMaxPolygonPoints);

							agk::SetSpriteShape(m_pSearchObj->m_iSprite, m_pSearchObj->iPhysicsShape);
							agk::SetSpritePhysicsOn(m_pSearchObj->m_iSprite, m_pSearchObj->iPhysicsMode + 1);
							if (m_pSearchObj->bPhysicsCanRotate)
								agk::SetSpritePhysicsCanRotate(m_pSearchObj->m_iSprite, 1);
							else
								agk::SetSpritePhysicsCanRotate(m_pSearchObj->m_iSprite, 0);

							if (m_pSearchObj->bPhysicsIsBullet)
								agk::SetSpritePhysicsIsBullet(m_pSearchObj->m_iSprite, 1);
							else
								agk::SetSpritePhysicsIsBullet(m_pSearchObj->m_iSprite, 0);
							agk::SetSpritePhysicsRestitution(m_pSearchObj->m_iSprite, m_pSearchObj->fPhysicsRestitution);

							if (bUsePercentage) {
								agk::SetPhysicsGravity((fPhysicsGravityX / (float)iBaseWidth)*100.0f, (fPhysicsGravityY / (float)iBaseHeight)*100.0f);
							}
							else {
								agk::SetPhysicsGravity(fPhysicsGravityX, fPhysicsGravityY);
							}
							if (bPhysicsWallBottom)
								agk::SetPhysicsWallBottom(1);
							else
								agk::SetPhysicsWallBottom(0);
							if (bPhysicsWallTop)
								agk::SetPhysicsWallTop(1);
							else
								agk::SetPhysicsWallTop(0);
							if (bPhysicsWallLeft)
								agk::SetPhysicsWallLeft(1);
							else
								agk::SetPhysicsWallLeft(0);
							if (bPhysicsWallRight)
								agk::SetPhysicsWallRight(1);
							else
								agk::SetPhysicsWallRight(0);

							if (bPhysicsDebug)
								agk::SetPhysicsDebugOn();
							else
								agk::SetPhysicsDebugOff();

							if (m_pSearchObj->fPhysicsCOMX != 0.0f || m_pSearchObj->fPhysicsCOMY != 0.0f) {
								if (bUsePercentage) {
									agk::SetSpritePhysicsCOM(m_pSearchObj->m_iSprite, (m_pSearchObj->fPhysicsCOMX / (float) iBaseWidth)*100.0f, (m_pSearchObj->fPhysicsCOMY / (float) iBaseHeight)*100.0f);
								}
								else {
									agk::SetSpritePhysicsCOM(m_pSearchObj->m_iSprite, m_pSearchObj->fPhysicsCOMX, m_pSearchObj->fPhysicsCOMY);
								}
							}

							if (m_pSearchObj->fPhysicsMass != 100.0f) {
								if (bUsePercentage) {
									agk::SetSpritePhysicsMass(m_pSearchObj->m_iSprite, (m_pSearchObj->fPhysicsMass / (float)iBaseWidth)*100.0f);
								}
								else {
									agk::SetSpritePhysicsMass(m_pSearchObj->m_iSprite, m_pSearchObj->fPhysicsMass);
								}
							}

							if(m_pSearchObj->fPhysicsFriction != 0.5f )
								agk::SetSpritePhysicsFriction(m_pSearchObj->m_iSprite, m_pSearchObj->fPhysicsFriction);

							if (m_pSearchObj->fPhysicsLinearDamping != 0.0f)
								agk::SetSpritePhysicsDamping(m_pSearchObj->m_iSprite, m_pSearchObj->fPhysicsLinearDamping);

							if (m_pSearchObj->fPhysicsAngularDamping != 0.0f)
								agk::SetSpritePhysicsAngularDamping(m_pSearchObj->m_iSprite, m_pSearchObj->fPhysicsAngularDamping);

							agk::SetSpritePhysicsOn(m_pSearchObj->m_iSprite);
							agk::SetSpriteActive(m_pSearchObj->m_iSprite, 1);

							m_pSearchObj->bPhysicsInit = true;

						}
						
					}
					else {
						//disable physics if not in testmode.
						if (m_pSearchObj->bPhysicsInit) {
							agk::SetSpritePhysicsOn(m_pSearchObj->m_iSprite, 0 );
							//agk::SetSpriteShape(m_pSearchObj->m_iSprite, 0);//Delete Shape
							agk::SetSpriteShape(m_pSearchObj->m_iSprite,0);
							agk::SetSpritePhysicsOff(m_pSearchObj->m_iSprite);
							agk::SetSpritePhysicsDelete(m_pSearchObj->m_iSprite);
							agk::SetSpriteActive(m_pSearchObj->m_iSprite,0);
							agk::SetObjectCollisionMode(m_pSearchObj->m_iSprite, 0);
							//UpdateObject(m_pSearchObj);
							agk::SetSpriteShape(m_pSearchObj->m_iSprite, 0, 0);
							agk::SetPhysicsDebugOff();

							m_pSearchObj->bPhysicsInit = false;
						}
					}
				}
			}

			UpdateObject(m_pSearchObj); // Might remove this later for speed.

			if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE && bTestMode && !m_pSearchObj->m_bVisible) {
				agk::SetSpriteVisible(m_pSearchObj->m_iSprite, 0);
			}
			if (m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT && bTestMode ) {
				if(!m_pSearchObj->m_bVisible)
					agk::SetTextVisible(m_pSearchObj->m_iSprite, 0);
				else
					agk::SetTextVisible(m_pSearchObj->m_iSprite, 1);
			}
			if (m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON && bTestMode) {
				if (!m_pSearchObj->m_bVisible) {
					agk::SetVirtualButtonVisible(m_pSearchObj->m_iSprite, 0);
					agk::SetVirtualButtonActive(m_pSearchObj->m_iSprite, 0);
				}
				else {
					agk::SetVirtualButtonVisible(m_pSearchObj->m_iSprite, 1);
					agk::SetVirtualButtonActive(m_pSearchObj->m_iSprite, 1);
				}
			}
			if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX && bTestMode) {
				if (!m_pSearchObj->m_bVisible) {
					agk::SetEditBoxVisible(m_pSearchObj->m_iSprite, 0);
					agk::SetEditBoxActive(m_pSearchObj->m_iSprite, 0);
				}
				else {
					agk::SetEditBoxVisible(m_pSearchObj->m_iSprite, 1);
					agk::SetEditBoxActive(m_pSearchObj->m_iSprite, 1);
				}
			}

		}
		m_pSearchObj = m_pSearchObj->m_pNext;
	}
	if (iActiveSelections > 0) {
		mMultipleSelectActive = true;
		m_pSelectedObject = NULL;
	}

	RenderScene();

	ImGui::SetItemAllowOverlap();
	if (ImGui::ImgBtnBack(iRenderTarget, ImVec2(iImgW*fRatio, iImgH*fRatio), ImColor(0, 0, 0, 0), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 255))) {
		//3d but pressed.
	}

	//On Top. Display actual AGK area, remove borders.
	if(!bFollowDeviceResolution) {
		if (!bTestMode && bAGKBorder) {
			float linesize = 1.0f;
			a[0].x = wp.x + centerSceneX + (fCenterX - linesize);
			a[0].y = wp.y + centerSceneY + (fCenterY - linesize);
			a[1].x = wp.x + centerSceneX + (fCenterX + (iImgW*fRatio) + linesize);
			a[1].y = wp.y + centerSceneY + (fCenterY - linesize);
			a[2].x = wp.x + centerSceneX + (fCenterX + (iImgW*fRatio) + linesize);
			a[2].y = wp.y + centerSceneY + (fCenterY + (iImgH*fRatio) + linesize);
			a[3].x = wp.x + centerSceneX + (fCenterX - linesize);
			a[3].y = wp.y + centerSceneY + (fCenterY + (iImgH*fRatio) + linesize);

			a[0].x += fScissorXScaled;
			a[1].x -= fScissorXScaled;
			a[2].x -= fScissorXScaled;
			a[3].x += fScissorXScaled;

			a[0].y += fScissorYScaled;
			a[1].y += fScissorYScaled;
			a[2].y -= fScissorYScaled;
			a[3].y -= fScissorYScaled;

			ImGui::GetWindowDrawList()->AddQuad(a[0], a[1], a[2], a[3], ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram])), linesize+ linesize);
		}
	}

	float scalex;
	float scaley;
	if (!bFollowDeviceResolution) {
		scalex = (float)(iScreenWidth - fScissorX - fScissorX) / (float)iBaseWidth;
		scaley = (float)(iScreenHeight - fScissorY - fScissorY) / (float)iBaseHeight;

		//scalex = fScaleToScreenX;
		//scaley = fScaleToScreenY;
	}
	else {
		scalex = 1.0f;
		scaley = 1.0f;
	}


	iActiveSpriteCount = 0;
	std::map<std::int32_t, MediaObject *> agkDrawList;
	MediaScene::MediaObject * m_pLastObj = NULL;
	m_pSearchObj = m_pFirstObject;
	while (m_pSearchObj) {
		if (!m_pSearchObj->m_bRemoved  ) { //&& m_pSearchObj != m_pSelectedObject
			//We need to disable all sprites used in this scene, if we have more scene editors running.
			if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
				agk::SetSpriteVisible(m_pSearchObj->m_iSprite, 0);

				//			extern MediaScene * m_ActiveScene;
				//			if( m_ActiveScene != this)

				if (m_pSearchObj->bEnablePhysics) {
					if (!bTestMode) {
						agk::SetSpritePosition(m_pSearchObj->m_iSprite, -100000, -100000);
					}
				}

			}
			if (m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT) {
				agk::SetTextVisible(m_pSearchObj->m_iSprite, 0);
			}
			if (m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {
				agk::SetVirtualButtonVisible(m_pSearchObj->m_iSprite, 0);
				agk::SetVirtualButtonActive(m_pSearchObj->m_iSprite, 0);
			}
			if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
				agk::SetEditBoxVisible(m_pSearchObj->m_iSprite, 0);
				agk::SetEditBoxActive(m_pSearchObj->m_iSprite, 0);
			}

//			agkDrawList.insert(std::make_pair(std::int32_t(((10000-m_pSearchObj->m_Depth)*10000+ iActiveSpriteCount)+ (m_pSearchObj->m_iImageIndex) ), m_pSearchObj ));
			agkDrawList.insert(std::make_pair(std::int32_t(((10000 - m_pSearchObj->m_Depth) * 10000 + iActiveSpriteCount) ), m_pSearchObj));
			iActiveSpriteCount++;
		}
		m_pSearchObj = m_pSearchObj->m_pNext;
	}
	//Always draw selection last , so its on top of everything.
	//Not always good when working with depth , so disable for now.
//	if (m_pSelectedObject && !m_pSelectedObject->m_bRemoved) {
//		agk::SetSpriteVisible(m_pSelectedObject->m_iSprite, 0);
//		agkDrawList.insert(std::make_pair(std::int32_t(10001*10000 + iActiveSpriteCount), m_pSelectedObject));
//		iActiveSpriteCount++;
//	}

	//Draw all sprites sorted by depth.
	//MediaObject *OldSelection = m_pSelectedObject;
	MediaObject * m_pCurrentSelection = m_pSelectedObject;
	bool haschanged = false;
	bool bSelectedFocus = false;

	m_pChangeTo = NULL;
	int focusobjects = 0;
	
	//For select ONLY draw.
	unsigned int dl_tex_id = 0;
	ImVec2 dl_scenter, dl_pos;
	float dl_rot;
	ImColor dl_sCol,dl_shCol;
	MediaObject *dl_Obj = NULL;


	if (!bTestMode) {
		for (std::map<std::int32_t, MediaObject *>::iterator it = agkDrawList.begin(); it != agkDrawList.end(); ++it)
		{
			//ImGui::Text("Sprite: %d (%s)", it->first,it->second->m_sName.GetStr());
			MediaObject *Obj = it->second;
			pImage = agk::GetImagePtr(Obj->m_iImageId);

			int iTexID;
			if(pImage)
				iTexID = pImage->GetTextureID() + 300000; //100000 = Transparent with colors.

				ImVec2 spos = { wp.x + fCenterX + centerSceneX , wp.y + fCenterY + centerSceneY };

				spos.x += fScissorXScaled;
				spos.y += fScissorYScaled;

				float oposx = (Obj->m_Position.x * scalex) * fRatio;
//				float oposy = (Obj->m_Position.y-0.2f * scaley) * fRatio;
				float oposy = (Obj->m_Position.y * scaley) * fRatio;
				float osizex = (Obj->m_Size.x * scalex) * fRatio * Obj->m_Scale.x;
				float osizey = (Obj->m_Size.y * scaley) * fRatio * Obj->m_Scale.y;
				float ofsizex = (osizex * (float) 0.5f);
				float ofsizey = (osizey * (float) 0.5f);

				Obj->oposx = spos.x + oposx + ofsizex;
				Obj->oposy = spos.y + oposy + ofsizey;

				ImVec2 scenter = { spos.x + oposx + ofsizex ,  spos.y + oposy + ofsizey };
				ImGui::SetItemAllowOverlap();
//				float rot = 1.0f / 57.2675f; //Follow agk.
				float rot = 1.0f / ( (360.0f / PI) * 0.5f);
				rot = rot * Obj->m_Rotation.x;

				//For the Layer system.
				float objAlpha = Obj->m_Color.w;
				//if (m_pSelectedImage) {
					for (int i = 0; i < 11; i++) {
						if (Obj->m_Depth == iLayerDepth[i]) {
							if (iLayerOpacity[i] != 255) {
								objAlpha = (float) iLayerOpacity[i];
								break;
							}
							if (!bLayerVisible[i])
								objAlpha = 0.0f;
						}
					}
				//}

				ImColor sCol, shCol;
				if (Obj->bOnlyVisibleInAGK) {
					//Locked sprite are only visible inside AGK area, hover stillworks.
					sCol = ImColor((int)Obj->m_Color.x, (int)Obj->m_Color.y, (int)Obj->m_Color.z, 0);
					shCol = ImColor((int)(Obj->m_Color.x*0.75f), (int)(Obj->m_Color.y*0.75f), (int)(Obj->m_Color.z*0.75f), 0);


					//shCol = ImColor((int)(Obj->m_Color.x*0.75f), (int)(Obj->m_Color.y*0.75f), (int)(Obj->m_Color.z*0.75f), (int)(objAlpha*0.25f));
				}
				else {
					sCol = ImColor((int)Obj->m_Color.x, (int)Obj->m_Color.y, (int)Obj->m_Color.z, (int)objAlpha);
					shCol = ImColor((int)(Obj->m_Color.x*0.75f), (int)(Obj->m_Color.y*0.75f), (int)(Obj->m_Color.z*0.75f), (int)(objAlpha*0.75f));
					if (shift)
						shCol = sCol;
				}


				if (Obj->m_iMediaType == MEDIATYPE_BUTTON) {
					rot = 0.0f;
					scenter = { spos.x + oposx ,  spos.y + oposy  }; //+ ofsizey

					if (Obj->m_iImageIndex > 0) {
						//Ratio:
						float ratioy = Obj->m_Size.y / Obj->m_Size.x;

						if (Obj->m_pUsedImage) {
							ratioy = (float)(float)Obj->m_pUsedImage->m_iHeight / (float) Obj->m_pUsedImage->m_iWidth;
						}
						float ratiox = Obj->m_Size.x / Obj->m_Size.y;
//						if(Obj->m_Size.x < Obj->m_Size.y)
//							ratioy = Obj->m_Size.x / Obj->m_Size.y;
						osizey *= ratioy; //0.33f;

					}
				}

				//rot = Obj->m_Rotation.x;
				//ImGui::IsWindowFocused() && wtabvisible
				if (Obj->m_iMediaType == MEDIATYPE_TEXT || Obj->m_iMediaType == MEDIATYPE_EDITBOX) {
					//Draw Text Control.
					if (Obj->bIsSelected) {
						shCol = ImColor((int)(Obj->m_Color.x*0.65f), (int)(Obj->m_Color.y*0.65f), (int)(Obj->m_Color.z*0.65f), (int)(Obj->m_Color.w*0.65f));
						if( DrawAGKSprite(0, scenter, ImVec2(osizex, osizey), rot, shCol, shCol, true, Obj) )
						{
							bSelectedFocus = true;
						}
					}
					else if ((Obj != m_pSelectedObject || m_pSelectedImage || shift))
						DrawAGKSprite(0, scenter, ImVec2(osizex, osizey), rot, sCol, shCol, false, Obj);
					else {
						if (DrawAGKSprite(0, scenter, ImVec2(osizex, osizey), rot, sCol, shCol, true, Obj)) {
							focusobjects++;
							bSelectedFocus = true;
						}
					}
				}
				else if(!pImage) {
					//Missing image cant be a sprite.
				}
				else if (Obj->bIsSelected) {
					shCol = ImColor((int)(Obj->m_Color.x*0.65f), (int)(Obj->m_Color.y*0.65f), (int)(Obj->m_Color.z*0.65f), (int)(objAlpha*0.65f));
					if (DrawAGKSprite((ImTextureID)pImage->GetTextureID(), scenter, ImVec2(osizex, osizey), rot, shCol, shCol, true, Obj, false, true))
					{
						bSelectedFocus = true;
					}
				}
				else if( ( Obj != m_pSelectedObject || m_pSelectedImage || shift  ) ) { // !ImGui::IsWindowFocused()
					DrawAGKSprite((ImTextureID)pImage->GetTextureID(), scenter, ImVec2(osizex, osizey), rot , sCol, shCol,false, Obj);
				}
				else {
					if (DrawAGKSprite((ImTextureID)pImage->GetTextureID(), scenter, ImVec2(osizex, osizey), rot, sCol, shCol, true, Obj)) {
						
						dl_tex_id = pImage->GetTextureID();
						dl_scenter = scenter;
						dl_pos = ImVec2(osizex, osizey);
						dl_rot = rot;
						dl_sCol = sCol;
						dl_shCol = shCol;
						dl_Obj = Obj;

						focusobjects++;
						bSelectedFocus = true;
					}
				}

				if (!m_pSelectedObject && ctrl && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
					if (!Obj->bLocked) {
						if (Obj->bIsSelected)
							Obj->bIsSelected = false;
						else {
							//Make sure layes is not disabled.
							bool bCanBeSelected = true;
							for (int il = 0; il < 11; il++) {
								if (il == 0 || il == 3 || il == 5 || il == 7 || il == 10) {
									if (Obj->m_Depth == iLayerDepth[il]) {
										if (!bLayerVisible[il]) {
											bCanBeSelected = false;
											break;
										}
									}
								}
							}
							if(bCanBeSelected)
								Obj->bIsSelected = true;
						}
					}
				}

				//agk::SetSpriteAngle();
				if (!shift && ImGui::IsItemHovered()) {
					if(!Obj->bLocked)
						focusobjects++;
//					if (agk::GetPointerPressed() && !bInChangingMode) { // && iDelayChanges <= 0 GetPointerReleased,GetPointerPressed
					if ( !bInChangingMode && !Obj->bLocked ) { // && iDelayChanges <= 0 GetPointerReleased,GetPointerPressed

						//Check if layer is disabled.
						bool bCanBeSelected = true;
						for (int il = 0; il < 11; il++) {
							if (il == 0 || il == 3 || il == 5 || il == 7 || il == 10) {
								if (Obj->m_Depth == iLayerDepth[il]) {
									if (!bLayerVisible[il]) {
										bCanBeSelected = false;
										break;
									}
								}
							}
						}
						if (bCanBeSelected) {
							if (haschanged && m_pCurrentSelection) {
								//Allow toggle selection behind current selection.

								//if(Obj->m_Position)
								ImVec2 mpos = ImGui::GetMousePos();
								ImVec2 opos = { Obj->oposx,Obj->oposy };
								float dist1 = sqrt(pow(mpos.x - opos.x, 2) + pow(mpos.y - opos.y, 2));
								ImVec2 copos = { m_pCurrentSelection->oposx,m_pCurrentSelection->oposy };
								float dist2 = sqrt(pow(mpos.x - copos.x, 2) + pow(mpos.y - copos.y, 2));

								if (dist1 < dist2) { //|| !haschanged
									m_pChangeTo = Obj;
									haschanged = true;
								}
							}
							else {
								//m_pSelectedObject = Obj;
								m_pChangeTo = Obj;
								haschanged = true;
							}
							ide_force_rendering_delayed = true;
						}
					}
				}
			
		}
	}
	if (iDelayChanges > 0) iDelayChanges = iDelayChanges - 1;

	if(dl_Obj)
	{
		DrawAGKSprite((ImTextureID)dl_tex_id, dl_scenter, dl_pos, dl_rot, dl_sCol, dl_shCol, true, dl_Obj,true);
	}

	if (!mMultipleSelectActive) {
		if (agk::GetPointerState() && ImGui::IsWindowFocused() && iDelayChanges) {
			if (focusobjects <= 1 && !bInChangingMode && m_pChangeTo) {
				MediaScene::MediaObject * m_pSearchObj = NULL;
				m_pSearchObj = m_pFirstObject;
				while (m_pSearchObj) {
					if (!m_pSearchObj->m_bRemoved) {
						m_pSearchObj->bIsSelected = false;;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
				if (!bFirstSelection) {
					bFirstSelection = true;
					ImGui::SetWindowFocus("Properties");
				}
				m_pSelectedObject = m_pChangeTo;
			}
		}
		if (agk::GetPointerReleased() && ImGui::IsWindowFocused()) {
			if (!bInChangingMode && m_pChangeTo && iDelayChanges == 0) {
				MediaScene::MediaObject * m_pSearchObj = NULL;
				m_pSearchObj = m_pFirstObject;
				while (m_pSearchObj) {
					if (!m_pSearchObj->m_bRemoved) {
						m_pSearchObj->bIsSelected = false;;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
				m_pSelectedObject = m_pChangeTo;
				if (!bFirstSelection) {
					bFirstSelection = true;
					ImGui::SetWindowFocus("Properties");
				}
			}
		}


		if (!bIsAnythingHovered && !bInChangingMode && !m_pChangeTo && ImGui::IsWindowHovered() && ImGui::IsWindowFocused()) {
			if (agk::GetPointerReleased() && iDelayChanges == 0) { // && iDelayChanges <= 0 && !haschanged
				MediaScene::MediaObject * m_pSearchObj = NULL;
				m_pSearchObj = m_pFirstObject;
				while (m_pSearchObj) {
					if (!m_pSearchObj->m_bRemoved) {
						m_pSearchObj->bIsSelected = false;;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}

				m_pSelectedObject = NULL;
				ide_force_rendering_delayed = true;
			}
		}
	}

	if (agk::GetPointerReleased())
		bInChangingMode = false;

	agkDrawList.clear();

	//#### Draw Grid. ####
	if (!bTestMode && (bGridOn  ) ) { //|| m_pSelectedImage

		//Align to Agk screen zoom ratio.
		float xRatio = ( (fGridX)*fRatio*scalex);
		float yRatio = ( (fGridY)*fRatio*scaley);
		wp.x += ImGui::GetScrollX();
		wp.y += ImGui::GetScrollY();

		float alignToGridX = fmod(ImGui::GetScrollX() - centerSceneX - fScissorXScaled, xRatio);
		float alignToGridY = fmod(ImGui::GetScrollY() - centerSceneY - fScissorYScaled, yRatio);

		wp.x += fGridXOffset;
		wp.y += fGridYOffset;

		for (float i = -600; i < 600; i++) {

			a[0].x = wp.x + fCenterX - alignToGridX + (xRatio*i);
			if (a[0].x > wp.x && a[0].x < (wp.x + wso.x)) {
				a[0].y = wp.y;
				a[1].x = wp.x + fCenterX - alignToGridX + (xRatio*i);
				a[1].y = wp.y + wso.y;
				ImGui::GetWindowDrawList()->AddPolyline(&a[0], 2, ImGui::GetColorU32(vGridColor), false, 1.0f);
			}

			a[0].y = wp.y + fCenterY - alignToGridY + (yRatio*i);
			if (a[0].y > wp.y && a[0].y < (wp.y + wso.y)) {
				a[0].x = wp.x;
				a[1].x = wp.x + wso.x;
				a[1].y = wp.y + fCenterY - alignToGridY + (yRatio*i);;
//				ImGui::GetWindowDrawList()->AddPolyline(&a[0], 2, ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_MenuBarBg])), false, 1.0f);
				ImGui::GetWindowDrawList()->AddPolyline(&a[0], 2, ImGui::GetColorU32(vGridColor), false, 1.0f);
			}

		}
		wp.x -= fGridXOffset;
		wp.y -= fGridYOffset;

		wp.x -= ImGui::GetScrollX();
		wp.y -= ImGui::GetScrollY();
	}

	iAction = 0;
//	if (agk::GetRawMouseRightState()) {
//		//exit draw mode.
//		m_pSelectedImage = NULL;
//	}


	if(!ctrl && !shift && !firstmousepress && !m_pSelectedImage && !m_pSelectedObject && ImGui::IsWindowHovered() ) { //!bSelectedFocus
		//Multiple select.

		mmouseendpos = ImGui::GetMousePos();
		ImVec2 diff;
		float scalex, scaley;

		if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow))) {
			cursoradd.x--;
		}
		if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow))) {
			cursoradd.x++;
		}
		if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {
			cursoradd.y--;
		}
		if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
			cursoradd.y++;
		}
		if (cursoradd.x != 0 || cursoradd.y != 0) {

			bCursorChanges = true;

			//Move Sprites.
			MediaScene::MediaObject * m_pSearchObj = NULL;
			m_pSearchObj = m_pFirstObject;
			while (m_pSearchObj) {
				if (m_pSearchObj->bIsSelected) {
					m_pSearchObj->m_Position.x = m_pSearchObj->m_Position.x + cursoradd.x;
					m_pSearchObj->m_Position.y = m_pSearchObj->m_Position.y + cursoradd.y;
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
						if (bUsePercentage) {
							agk::SetSpritePosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
						}
						else {
							agk::SetSpritePosition(m_pSearchObj->m_iSprite, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
						}
					}
					m_editor->filechanged = true;
				}
				m_pSearchObj = m_pSearchObj->m_pNext;
			} // while
			cursoradd.x = 0;
			cursoradd.y = 0;
		}

		if (agk::GetPointerState()) {
			if(Multifirstmousepress || (bSelectedFocus && !bMultiSelect) ) {

				//Move sprite.
				if (!Multifirstmousepress) {
					Multifirstmousepress = true;
					MediaScene::MediaObject * m_pSearchObj = NULL;
					m_pSearchObj = m_pFirstObject;
					while (m_pSearchObj) {
						if (m_pSearchObj->bIsSelected) {
							m_pSearchObj->m_OldPosition = m_pSearchObj->m_Position;
							m_pSearchObj->m_OldColor = m_pSearchObj->m_Color;
						}
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
					bCursorChanges = false;
					bColorChanged = false;
					newColor = { 255,255,255,255 };
					mmousestartpos = ImGui::GetMousePos();
					cursoradd.x = 0;
					cursoradd.y = 0;
				}
				else {

					diff = mmouseendpos - mmousestartpos;
					scalex = fScaleToScreenX;
					scaley = fScaleToScreenY;
					diff.x /= fRatio;
					diff.y /= fRatio;
					diff.x *= scalex;
					diff.y *= scaley;

					if (mmouseendpos.x != mmousestartpos.x || mmouseendpos.y != mmousestartpos.y) {

						//Move Sprites.
						MediaScene::MediaObject * m_pSearchObj = NULL;
						m_pSearchObj = m_pFirstObject;
						while (m_pSearchObj) {
							if (m_pSearchObj->bIsSelected) {


								m_pSearchObj->m_Position.x = m_pSearchObj->m_OldPosition.x + diff.x;
								m_pSearchObj->m_Position.y = m_pSearchObj->m_OldPosition.y + diff.y;

								if (bSnapToGrid && bGridOn) {
									int align = (int) (round(m_pSearchObj->m_Position.x) / fGridX);
									if (m_pSearchObj->m_Position.x < 0.0f)
										align -= 1;
									m_pSearchObj->m_Position.x = align * fGridX;
									m_pSearchObj->m_Position.x += fGridXOffset;
									align = (int) (round(m_pSearchObj->m_Position.y) / fGridY);
									if (m_pSearchObj->m_Position.y < 0.0f)
										align -= 1;
									m_pSearchObj->m_Position.y = align * fGridY;
									m_pSearchObj->m_Position.y += fGridYOffset;
								}

								if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
									if (bUsePercentage) {
										agk::SetSpritePosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
									}
									else {
										agk::SetSpritePosition(m_pSearchObj->m_iSprite, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
									}
								}
								m_editor->filechanged = true;
							}
							m_pSearchObj = m_pSearchObj->m_pNext;
						} // while

					} // startm != newmouse

				} // Multifirstmousepress

			}
			else if(!ctrl) {
				if (!bMultiSelect) {

					//Check for cursor changes here.
					if (bCursorChanges) {
						//check old pos.
						int undo_count = 0;
						MediaScene::MediaObject * m_pSearchObj = NULL;
						m_pSearchObj = m_pFirstObject;
						while (m_pSearchObj) {
							if (m_pSearchObj->bIsSelected) {
								if (m_pSearchObj->m_Position.x != m_pSearchObj->m_OldPosition.x || m_pSearchObj->m_Position.y != m_pSearchObj->m_OldPosition.y) {
									Vec4 newpos = m_pSearchObj->m_Position;
									m_pSearchObj->m_Position = m_pSearchObj->m_OldPosition;
									AddUndo(m_pSearchObj, false);
									undo_count++;
									m_pSearchObj->m_Position = newpos;
									if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
										if (bUsePercentage) {
											agk::SetSpritePosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
										}
										else {
											agk::SetSpritePosition(m_pSearchObj->m_iSprite, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
										}
									}
									m_editor->filechanged = true;
								}
							}
							m_pSearchObj = m_pSearchObj->m_pNext;
						} // while
						if (undo_count > 0)
							AddUndo(m_pSearchObj, false, undo_count);
					}

					//Check for color changes.
					if (bColorChanged) {
						int undo_count = 0;
						MediaScene::MediaObject * m_pSearchObj = NULL;
						m_pSearchObj = m_pFirstObject;
						while (m_pSearchObj) {
							if (m_pSearchObj->bIsSelected) {
								if (m_pSearchObj->m_Color.x != m_pSearchObj->m_OldColor.x || m_pSearchObj->m_Color.y != m_pSearchObj->m_OldColor.y || m_pSearchObj->m_Color.z != m_pSearchObj->m_OldColor.z || m_pSearchObj->m_Color.w != m_pSearchObj->m_OldColor.w) {
									Vec4 newcolor = m_pSearchObj->m_Color;
									m_pSearchObj->m_Color = m_pSearchObj->m_OldColor;
									AddUndo(m_pSearchObj, false);
									undo_count++;
									m_pSearchObj->m_Color = newcolor;
									if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE)
										agk::SetSpriteColor(m_pSearchObj->m_iSprite, (int) m_pSearchObj->m_Color.x, (int) m_pSearchObj->m_Color.y, (int) m_pSearchObj->m_Color.z, (int) m_pSearchObj->m_Color.w);
									m_editor->filechanged = true;
								}
							}
							m_pSearchObj = m_pSearchObj->m_pNext;
						} // while
						if (undo_count > 0)
							AddUndo(m_pSearchObj, false, undo_count);

					}

					bCursorChanges = false;
					bColorChanged = false;
					newColor = { 255,255,255,255 };
					//set start pos.
					MultiA[0] = ImGui::GetMousePos();
					MediaScene::MediaObject * m_pSearchObj = NULL;
					m_pSearchObj = m_pFirstObject;
					while (m_pSearchObj) {
						if (!m_pSearchObj->m_bRemoved) {
							m_pSearchObj->bIsSelected = false;
						}
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
					m_pSelectedObject = NULL;
				}
				else {

					ImVec2 mp = ImGui::GetMousePos();
					MultiA[2] = mp;

					MultiA[1] = MultiA[0];
					MultiA[1].x = MultiA[2].x;

					MultiA[3] = MultiA[0];
					MultiA[3].y = MultiA[2].y;

					ImGui::GetWindowDrawList()->AddQuad(MultiA[0], MultiA[1], MultiA[2], MultiA[3], ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram])), 3.0f);

					//Find lowest corner.
//					if (MultiA[2].x < MultiA[0].x && MultiA[2].y < MultiA[0].y) {
//						MultiA[4] = MultiA[2];
//						MultiA[5] = MultiA[0];
//					}
//					else {
//						MultiA[4] = MultiA[0];
//						MultiA[5] = MultiA[2];
//					}
					MultiA[4] = MultiA[0];
					MultiA[5] = MultiA[2];

					if (MultiA[0].x <= MultiA[4].x && MultiA[0].y <= MultiA[4].y) {
						MultiA[4] = MultiA[0];
					}
					if (MultiA[1].x <= MultiA[4].x && MultiA[1].y <= MultiA[4].y) {
						MultiA[4] = MultiA[1];
					}
					if (MultiA[2].x <= MultiA[4].x && MultiA[2].y <= MultiA[4].y) {
						MultiA[4] = MultiA[2];
					}
					if (MultiA[3].x <= MultiA[4].x && MultiA[3].y <= MultiA[4].y) {
						MultiA[4] = MultiA[3];
					}


					if (MultiA[3].x >= MultiA[5].x && MultiA[3].y >= MultiA[5].y) {
						MultiA[5] = MultiA[3];
					}
					if (MultiA[0].x >= MultiA[5].x && MultiA[0].y >= MultiA[5].y) {
						MultiA[5] = MultiA[0];
					}
					if (MultiA[2].x >= MultiA[5].x && MultiA[2].y >= MultiA[5].y) {
						MultiA[5] = MultiA[2];
					}
					if (MultiA[1].x >= MultiA[5].x && MultiA[1].y >= MultiA[5].y) {
						MultiA[5] = MultiA[1];
					}



					//Reselect all entries.

					MediaScene::MediaObject * m_pSearchObj = NULL;
					m_pSearchObj = m_pFirstObject;
					while (m_pSearchObj) {
						if (!m_pSearchObj->m_bRemoved && !m_pSearchObj->bLocked ) {
							bool doselect = false;
							float ox = m_pSearchObj->oposx;
							float oy = m_pSearchObj->oposy;

							//Make sure layes is not disabled.
							bool bCanBeSelected = true;
							for (int il = 0; il < 11; il++) {
								if (il == 0 || il == 3 || il == 5 || il == 7 || il == 10) {
									if (m_pSearchObj->m_Depth == iLayerDepth[il]) {
										if (!bLayerVisible[il]) {
											bCanBeSelected = false;
											break;
										}
									}
								}
							}

							if (ox > MultiA[4].x && oy > MultiA[4].y)
							{
								if (ox < MultiA[5].x && oy < MultiA[5].y)
								{
									doselect = true;
								}
							}

							if (bCanBeSelected && doselect) {
								m_pSearchObj->m_OldPosition = m_pSearchObj->m_Position;
								m_pSearchObj->m_OldColor = m_pSearchObj->m_Color;
								m_pSearchObj->bIsSelected = true;
							}
							else
								m_pSearchObj->bIsSelected = false;
						}
						m_pSearchObj = m_pSearchObj->m_pNext;
					}


				}
				bMultiSelect = true;
			}
		}
		else {
			if (Multifirstmousepress) {
				//Make the undo and real move.

				//Reset for now.
				MediaScene::MediaObject * m_pSearchObj = NULL;
				m_pSearchObj = m_pFirstObject;
				int undo_count = 0;
				while (m_pSearchObj) {
					if (m_pSearchObj->bIsSelected) {

						if (m_pSearchObj->m_OldPosition.x != m_pSearchObj->m_Position.x || m_pSearchObj->m_OldPosition.y != m_pSearchObj->m_Position.y)
						{
							Vec4 newposition = m_pSearchObj->m_Position;
							m_pSearchObj->m_Position = m_pSearchObj->m_OldPosition;
							AddUndo(m_pSearchObj, false);
							undo_count++;
							m_pSearchObj->m_Position = newposition;
							if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
								if (bUsePercentage) {
									agk::SetSpritePosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
								}
								else {
									agk::SetSpritePosition(m_pSearchObj->m_iSprite, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
								}
							}
							iDelayChanges = 3;
						}

					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
				if(undo_count > 0)
					AddUndo(m_pSearchObj, false, undo_count);
			}
			Multifirstmousepress = false;
			bMultiSelect = false;
		}
	}

	if (agk::GetRawMouseMiddleState() && ImGui::IsWindowHovered()) {
		if (!bMiddleMousePresset) {
			vMouseStart = ImGui::GetMousePos() - ImGui::GetWindowPos();
		}
		bMiddleMousePresset = true;
		ImVec2 vMouseDiff = ImGui::GetMousePos() - ImGui::GetWindowPos();
		vMouseDiff = vMouseStart - vMouseDiff;
//		ImVec2 vMouseDiffpure = vMouseDiff * 0.80f;
//		vMouseDiff *= 0.125f;

		if (vMouseDiff.x != 0.0f || vMouseDiff.y != 0.0f) {
			if (vMouseDiff.x != 0.0f) {
				float scx = ImGui::GetScrollX();
				scx += vMouseDiff.x;
				ImGui::SetScrollX(scx);
			}
			if (vMouseDiff.y != 0.0f) {
				float scy = ImGui::GetScrollY();
				scy += vMouseDiff.y;
				ImGui::SetScrollY(scy);
			}
			vMouseStart = ImGui::GetMousePos() - ImGui::GetWindowPos();
		}
	}
	else {
		bMiddleMousePresset = false;
	}

	if( (shift) && ImGui::IsWindowHovered() && (!bIsAnythingHovered || shift ) && (agk::GetPointerState()) ) { //|| agk::GetRawMouseRightState()
		if (!bMousePresset) {
			vMouseStart = ImGui::GetMousePos()-ImGui::GetWindowPos();
		}
		bMousePresset = true;
		ImVec2 vMouseDiff = ImGui::GetMousePos() - ImGui::GetWindowPos();
		vMouseDiff = vMouseStart - vMouseDiff;
		ImVec2 vMouseDiffpure = vMouseDiff * 0.80f;
		vMouseDiff *= 0.125f;

		if( vMouseDiff.x != 0.0f || vMouseDiff.y != 0.0f ) {
			if (vMouseDiff.x != 0.0f) {
				float scx = ImGui::GetScrollX();
				scx += vMouseDiff.x;
				ImGui::SetScrollX(scx);
			}
			if (vMouseDiff.y != 0.0f) {
				float scy = ImGui::GetScrollY();
				scy += vMouseDiff.y;
				ImGui::SetScrollY(scy);
			}
		}
		float linesize = 4.0f;
		a[0].x = (wp.x + vMouseStart.x + ImGui::GetScrollX()) - linesize;
		a[0].y = (wp.y + vMouseStart.y + ImGui::GetScrollY()) - linesize;
		a[2].x = (wp.x + vMouseStart.x - vMouseDiffpure.x + ImGui::GetScrollX()) + linesize;
		a[2].y = (wp.y + vMouseStart.y - vMouseDiffpure.y + ImGui::GetScrollY()) + linesize;

		ImGui::GetWindowDrawList()->AddLine(a[0], a[2], ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram])), linesize);//, 20.0f

	}
	else {
		bMousePresset = false;
	}

	extern TextEditor * m_ActiveEditor;
	if( wtabvisible && (ImGui::IsWindowHovered() || ImGui::IsWindowFocused() ) ) {
		//Floating controls.
		int scrollbarsize = 18;
		ImVec2 cp = ImGui::GetWindowSize();
		cp.x += ImGui::GetScrollX() - scrollbarsize;
		cp.y += ImGui::GetScrollY() - scrollbarsize;
		//Zoom
		ImGui::PushItemWidth(130);
		cp.x -= 136 + (ImGui::GetFontSize() * 4);
		cp.y -= ImGui::GetFontSize()*1.55f;
		ImGui::SetCursorPos(cp);
		ImGui::SetItemAllowOverlap();
		if (ImGui::SliderFloat("##zoom", &zoomAll, -1.0f, 5.0f, "")) {
		//if (ImGui::SliderFloat("##zoom", &fRatio, -1.0f, 5.0f, "%.3f")) {
			ide_force_rendering_delayed = true;
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("1:1")) {
			zoomAll = -fRatioAuto + 1.0f;
			iDelayAction = 1;
		}


		//Test buttom

		ImVec4 style_back = ImGui::GetStyle().Colors[ImGuiCol_Text];
		style_back = style_back * ImVec4(1.2f, 1.2f, 1.2f, 1.2f);
		ImVec4 drawCol_back = ImColor(255, 255, 255, 0)*style_back; // Not really used as we have transparent icons.
		ImVec4 drawCol_normal = ImColor(220, 220, 220, 220)*style_back;
		ImVec4 drawCol_hover = ImColor(255, 255, 255, 255)*style_back;
		ImVec4 drawCol_Down = ImColor(180, 180, 160, 255)*style_back;
		ImVec4 drawCol_active = ImColor(120, 220, 120, 220)*style_back;
		if (pref.iCurrentIconSet == 0) {
			drawCol_active = ImColor(220, 220, 220, 220)*style_back;
		}
		float test_size = 32.0f;// = ImGui::GetFontSize()*2.0f;

		cp = ImGui::GetWindowSize();
		cp.x += ImGui::GetScrollX() - scrollbarsize;
		cp.x -= (test_size + 4)*4.0f;
//		cp.x = ImGui::GetScrollX() + 2.0f;
//		cp.y += ImGui::GetFontSize()*1.55f;
//		cp.y -= test_size;
		cp.y = ImGui::GetScrollY();
		ImGui::SetCursorPos(cp);


		if (ImGui::ImgBtn(iToolbarImages[4], ImVec2(test_size, test_size), drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down))
		{
			iDelayChanges = 4;
			Undo();
		}
		if (ImGui::IsItemHovered()) 
			ImGui::SetTooltip("Undo");


		cp.x += test_size + 4;
		ImGui::SetCursorPos(cp);

		if (ImGui::ImgBtn(iToolbarImages[5], ImVec2(test_size, test_size), drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down))
		{
			iDelayChanges = 4;
			Redo();
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Redo");

		
		cp.x += test_size + 4;
		ImGui::SetCursorPos(cp);

		if (ImGui::ImgBtn(iToolbarImages[14], ImVec2(test_size, test_size), drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down))
		{
			iDelayChanges = 4;
			zoomAll = RESETZOOM;
			iDelayAction = 1;
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Reset View");


		cp.x += test_size + 4;
		ImGui::SetCursorPos(cp);


		if (ImGui::ImgBtn(!bTestMode ? iToolbarImages[7]: iToolbarImagesDown[7], ImVec2(test_size,test_size), drawCol_back, !bTestMode ? drawCol_normal : drawCol_active, !bTestMode ? drawCol_hover : drawCol_active, drawCol_Down))
		{
			iDelayChanges = 4;
			if (bTestMode) {
				bTestMode = false;
				scene_editor_testmode = false;
			}
			else {
				bTestMode = true;
				scene_editor_testmode = true;
			}
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip(!bTestMode ? "Start Test Mode" : "Stop Test Mode");
		}

/* TEST CODE 
		float centerx = (wso.x) * 0.5f;
		float centery = (wso.y) * 0.5f;
		float mousex = ImGui::GetMousePos().x - ImGui::GetWindowPos().x;
		float mousey = ImGui::GetMousePos().y - ImGui::GetWindowPos().y;
		float alignx = mousex - centerx;
		float aligny = mousey - centery;

		ImGui::SetCursorPos(cp+ImVec2(-280,+30));
		ImGui::Text("fRatio: %.2f zoom: %.2f, %.2f (%.2f,%.2f)", fRatio, zoomAll, fRatio+ zoomAll+0.1f, alignx, aligny);

		if (ImGui::IsMouseDoubleClicked(0)) {
			float centerx = (wso.x) * 0.5f;
			float centery = (wso.y) * 0.5f;
			float mousex = ImGui::GetMousePos().x - ImGui::GetWindowPos().x;
			float mousey = ImGui::GetMousePos().y - ImGui::GetWindowPos().y;
			float alignx = mousex - centerx;
			float aligny = mousey - centery;

			zoomAll += 0.1f;

			if (fRatio+ zoomAll < -0.5f) {
				alignx *= (fRatio + 0.1f);
				aligny *= (fRatio + 0.1f);
				alignx *= 0.05f;
				aligny *= 0.05f;
			}
			else if (fRatio + zoomAll < -0.3f ) {
				alignx *= 0.7f;
				aligny *= 0.7f;
			}
			else {
				alignx *= 0.75f;
				aligny *= 0.75f;
			}

			ImGui::SetScrollX(ImGui::GetScrollX() + alignx);
			ImGui::SetScrollY(ImGui::GetScrollY() + aligny);


		}
*/

		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f) {
			bool minus = false;
			float add = io.MouseWheel;
			ide_force_rendering_delayed = true;
			if (add > 0) {
				zoomAll += 0.1f;
			}
			else {
				minus = true;
				zoomAll -= 0.1f;
			}

			//####################################################################
			//#### The below code make the zoom when you use the scrollwheel. ####
			//#### And it dont really work as it should.                      ####
			//#### If anyone got a hint how to fix it , please let me know :) ####
			//####################################################################
			//#### New to test: myScreenToWorldX() myScreenToWorldY()         ####
			//####################################################################
			//#### (iContentWide * 0.5f) = 0 - 10000 (center)
			//#### (iContentHeight * 0.5f) = 0 - 10000 (center)
			//#### ImGui::GetScrollX/Y() defaults to center (5000,5000)
			//#### (wso.x/y)  (wso = window size).
			//#### mx = ImGui::GetMousePos().x - ImGui::GetWindowPos().x
			//#### (mx) 0,0 = top,left of window.
			//#### fRatio = zoom value.
			//#####################################################################\

			if (1 == 2) { //converted Mike code to test.
				agk::SetViewZoomMode(1);
				float interp;
				if (add > 0)
					interp = 0.1f;
				else
					interp = -0.1f;
				agk::SetViewZoom(fRatio); //Used in ScreenToWorld. (fRatio)
				float centerx = agk::ScreenToWorldX((wso.x) * 0.5f);
				float centery = agk::ScreenToWorldY((wso.y) * 0.5f);
				float mousex = agk::ScreenToWorldX(ImGui::GetMousePos().x - ImGui::GetWindowPos().x);
				float mousey = agk::ScreenToWorldY(ImGui::GetMousePos().y - ImGui::GetWindowPos().y);
				float alignx = mousex - centerx;
				float aligny = mousey - centery;
				agk::SetViewZoom(1.0f);

				float addoffsetx = alignx*interp;
				float addoffsety = aligny*interp;
				ImGui::SetScrollX(ImGui::GetScrollX() + addoffsetx);
				ImGui::SetScrollY(ImGui::GetScrollY() + addoffsety);
			}

			if (1 == 2) { // new mixed way.

				agk::SetViewZoomMode(1);
				float interp;
				if (add > 0)
					interp = 0.1f; //*fRatio;
				else
					interp = -0.1f; //*fRatio;
				//interp /= fRatio;

				agk::SetViewZoom(fRatio); //Used in ScreenToWorld. (fRatio)
				float viewx = ImGui::GetScrollX();
				float viewy = ImGui::GetScrollY();
				viewx -= (iContentWide * 0.5f);
				viewy -= (iContentHeight * 0.5f);

				agk::SetViewOffset(viewx, viewy);

				float centerx = agk::ScreenToWorldX((wso.x) * 0.5f);
				float centery = agk::ScreenToWorldY((wso.y) * 0.5f);
				
				float mousex = agk::ScreenToWorldX(ImGui::GetMousePos().x - ImGui::GetWindowPos().x);
				float mousey = agk::ScreenToWorldY(ImGui::GetMousePos().y - ImGui::GetWindowPos().y);

				float alignx = mousex - centerx;
				float aligny = mousey - centery;
				agk::SetViewZoom(1.0f);
				agk::SetViewOffset(0, 0);

				if ((alignx > (wso.x*0.15f) || alignx < -(wso.x*0.15f)) || (aligny >(wso.y*0.15f) || aligny < -(wso.y*0.15f))) {

					float addoffsetx = alignx*interp;
					float addoffsety = aligny*interp;
					ImGui::SetScrollX(ImGui::GetScrollX() + addoffsetx);
					ImGui::SetScrollY(ImGui::GetScrollY() + addoffsety);
				}
			}
			

			if (1 == 1) { // old way. Current Working in 0.99f
					float centerx = (wso.x) * 0.5f;
					float centery = (wso.y) * 0.5f;
					float mousex = ImGui::GetMousePos().x - ImGui::GetWindowPos().x;
					float mousey = ImGui::GetMousePos().y - ImGui::GetWindowPos().y;
					float alignx = mousex - centerx;
					float aligny = mousey - centery;

					if ((alignx > (wso.x*0.15f) || alignx < -(wso.x*0.15f)) || (aligny > (wso.y*0.15f) || aligny < -(wso.y*0.15f))) {

						alignx *= fScaleToScreenX;
						aligny *= fScaleToScreenY;
						alignx /= (fRatio);
						aligny /= (fRatio);
						float addoffsetx = alignx*0.1875f;
						float addoffsety = aligny*0.1875f;
						if (add < 0) {
							addoffsetx = alignx*0.040875f;
							addoffsety = aligny*0.040875f;
						}
						ImGui::SetScrollX(ImGui::GetScrollX() + addoffsetx);
						ImGui::SetScrollY(ImGui::GetScrollY() + addoffsety);

					}
				}
		
		//}
		}
		if (zoomAll < -1.0f)
			zoomAll = -1.0f;
		if (zoomAll > 5.0f)
			zoomAll = 5.0f;
	}
	if (1 == 2)
	{
		//Debug view.
		ImVec2 cp = ImGui::GetCursorPos();
		cp.x = ImGui::GetScrollX() + 4;
		cp.y = ImGui::GetScrollY() + 4;
		ImGui::SetCursorPos(cp); //TitleSize

		ImGui::Text("SceneName: %s GridX: %f", m_sSceneName.GetStr(), fGridX);

		//cp.y += 18;	ImGui::SetCursorPos(cp);
		//ImGui::Text("size (%f,%f) pos (%f,%f) ", ws.x, ws.y, wp.x, wp.y);
		//ImGui::SameLine();
		//ImGui::Text(" scroll (%f,%f) ", ImGui::GetScrollX(), ImGui::GetScrollY());

		//cp.y += 18;	ImGui::SetCursorPos(cp);
		//ImGui::Text("fCenterXY (%f,%f) ", fCenterX, fCenterY);
		//ImGui::SameLine();
		//ImGui::Text(" centerSceneXY (%f,%f) ", centerSceneX, centerSceneY);

		//cp.y += 18;	ImGui::SetCursorPos(cp);
		//ImGui::Text("%f=agk::GetRawMouseWheel()", agk::GetRawMouseWheel());
	}


	if (m_pSelectedImage && !shift ) {

		//##########################
		//#### Image Draw mode. ####
		//##########################

		if (ImGui::IsWindowHovered() ) {

			float fDrawPositionX = 0;
			float fDrawPositionY = 0;

			ImVec2 MouseDrawStartPos;
			MouseDrawStartPos = ImGui::GetMousePos();

			ImVec2 wsp = ImGui::GetWindowPos();
			ImVec2 wp;
			wp.x = ImGui::GetScrollX() + (MouseDrawStartPos.x- wsp.x);
			wp.y = ImGui::GetScrollY() + (MouseDrawStartPos.y- wsp.y);

			float mouseToscreenx = (wp.x - centerSceneX) - fCenterX;// -fScissorX;
			float mouseToscreeny = (wp.y - centerSceneY) - fCenterY;// -fScissorY;

			mouseToscreenx /= fRatio;
			mouseToscreeny /= fRatio;
			mouseToscreenx -= fScissorX;
			mouseToscreeny -= fScissorY;
			mouseToscreenx *= fScaleToScreenX;
			mouseToscreeny *= fScaleToScreenY;


			fDrawPositionX = mouseToscreenx;
			fDrawPositionY = mouseToscreeny;

			
			//if (agk::GetPointerState()) {
			//	agk::Print("s");
			//}

			if (bSnapToGrid && (bGridOn || m_pSelectedImage)) {
				int align = (int) (round(fDrawPositionX) / fGridX);
				if (fDrawPositionX < 0.0f)
					align -= 1;
				fDrawPositionX = align * fGridX;
				fDrawPositionX += fGridXOffset;

				align = (int) (round(fDrawPositionY) / fGridY);
				if (fDrawPositionY < 0.0f)
					align -= 1;
				fDrawPositionY = align * fGridY;
				fDrawPositionY += fGridYOffset;
			}

			ImGui::SetCursorPos(wp+ImVec2(20.0f,20.0f));

			if (m_pSelectedImage->m_bAtlas)
				ImGui::ImgBtn(m_pSelectedImage->m_iImageId, ImVec2(32,32), pref.icon_background_color, ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), 2, m_iSelectedSubImage, m_pSelectedImage->m_iRows, m_pSelectedImage->m_iColumns);
			else
				ImGui::ImgBtn(m_pSelectedImage->m_iImageId, ImVec2(32, 32), pref.icon_background_color);

			if (agk::GetRawMouseRightState()) {
				//Delete mode.
				MediaScene::MediaObject * m_pSearchObj = NULL;
				bool bInsertObject = true;
				m_pSearchObj = m_pFirstObject;
				if (m_pSearchObj) {
					while (m_pSearchObj) {


						if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE ) {

							if (m_pSearchObj->m_Depth == iLayerDepth[iCurrentSelectedLayer]) {

								if (m_pSearchObj->m_Position.x == fDrawPositionX && m_pSearchObj->m_Position.y == fDrawPositionY) {
									//Delete Object.
									//m_pSearchObj->m_bRemoved = false;
									AddUndo(m_pSearchObj, false);
									m_pSearchObj->m_bRemoved = true;
									break;
								}
							}
						}
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
				}

			}
			else if (agk::GetPointerState()) {

				MediaScene::MediaObject * m_pSearchObj = NULL;
				bool bInsertObject = true;
				m_pSearchObj = m_pFirstObject;
				if (m_pSearchObj) {
					while (m_pSearchObj) {
						if (!m_pSearchObj->m_bRemoved) {

							if (m_pSearchObj->m_Depth == iLayerDepth[iCurrentSelectedLayer]) {

								if (m_pSearchObj->m_Position.x == fDrawPositionX && m_pSearchObj->m_Position.y == fDrawPositionY) {
									bInsertObject = false;
									break;
								}
							}
						}
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
				}

				if (bInsertObject) {
					//Create sprite object.
					if (m_pSelectedImage) {
						//Add a sprite object.
						//mousex
						MediaScene::MediaObject * m_pSearchObj = NULL;
						MediaScene::MediaObject * m_pLastObj = NULL;
						m_pSearchObj = m_pFirstObject;
						if (m_pSearchObj) {
							while (m_pSearchObj) {
								m_pLastObj = m_pSearchObj;
								m_pSearchObj = m_pSearchObj->m_pNext;
							}
						}

						MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
						Obj->m_bRemoved = false;
						Obj->m_bVisible = true;
						Obj->m_Color = { 255,255,255,255 };
						Obj->m_bVisible = true;

						Obj->m_Depth = iLayerDepth[iCurrentSelectedLayer];

						Obj->m_iImageIndex = m_pSelectedImage->m_iImageIndex;
						Obj->m_iImageId = m_pSelectedImage->m_iImageId;
						Obj->m_iImageDownIndex = 0;
						Obj->m_iImageDownId = 0;
						Obj->m_iMediaType = MEDIATYPE_SPRITE;
						Obj->m_iSprite = agk::CreateSprite(m_pSelectedImage->m_iImageId);
						Obj->m_Layer = 1;
						Obj->bAnimated = false;
						Obj->bFixToScreen = false;
						Obj->bStretchToDevice = false;
						Obj->bLocked = false;
						Obj->bOnlyVisibleInAGK = false;
						Obj->m_pUsedImage = m_pSelectedImage;
						Obj->m_pUsedDownImage = NULL;

						if (m_pSelectedImage->m_bAtlas) {
							Obj->iAnimationFrom = m_iSelectedSubImage;
							Obj->iAnimationTo = m_iSelectedSubImage;
						}
						else {
							Obj->iAnimationFrom = 0;
							Obj->iAnimationTo = 0;
						}
						Obj->m_pNext = NULL;
						Obj->m_Position = { fDrawPositionX,fDrawPositionY,0.0f,0.0f };
						if (bUsePercentage) {
							agk::SetSpritePosition(Obj->m_iSprite, (Obj->m_Position.x / (float)iBaseWidth)*100.0f, (Obj->m_Position.y / (float)iBaseHeight)*100.0f );
						}
						else {
							agk::SetSpritePosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
						}
						Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
						Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };
						if (bDrawModeFitGrid) {
							Obj->m_Size = { (float)fGridX, (float)fGridY ,0.0f,0.0f };
						}
						else {
							if (m_pSelectedImage->m_bAtlas) {
								int fw = (int)m_pSelectedImage->m_iWidth / m_pSelectedImage->m_iRows;
								int fh = (int)m_pSelectedImage->m_iHeight / m_pSelectedImage->m_iColumns;
								Obj->m_Size = { (float)fw, (float)fh ,0.0f,0.0f };
							}
							else {
								
//								Obj->m_Size = { (float)m_pSelectedImage->m_iWidth, (float)m_pSelectedImage->m_iHeight ,0.0f,0.0f };
								Obj->m_Size = { (float) fDrawModeSizeX, (float)fDrawModeSizeY ,0.0f,0.0f };
							}
						}
						Obj->m_sMediaFullPath = m_pSelectedImage->m_sFullPath;
						Obj->m_sMediaRelativePath = m_pSelectedImage->m_sRelativePath;
						
						Obj->bEnablePhysics = false;
						Obj->iPhysicsShape = 3;
						Obj->iPhysicsMode = 0;
						Obj->bPhysicsCanRotate = true;
						Obj->bPhysicsIsBullet = false;

						Obj->fPhysicsCOMX = 0.0f;
						Obj->fPhysicsCOMY = 0.0f;
						Obj->fPhysicsMass = 100.0f;
						Obj->fPhysicsFriction = 0.5f;
						Obj->fPhysicsLinearDamping = 0.0f;
						Obj->fPhysicsAngularDamping = 0.0f;

						Obj->m_iFontId = 0;
						Obj->m_iLoadFontId = 0;

						bool found = true;
						while (found) {
							found = false;
							uString sName = "Sprite ";
							sName.AppendInt(iSpriteCount++);
							Obj->m_sName = sName;

							m_pSearchObj = m_pFirstObject;
							if (m_pSearchObj) {
								while (m_pSearchObj) {
									if (m_pSearchObj->m_sName.CompareTo(sName) == 0) {
										found = true;
										break;
									}
									m_pSearchObj = m_pSearchObj->m_pNext;
								}
							}
						}

						Obj->m_sVariable = "";

						if (!m_pLastObj) {
							//No image found, set first entry.
							m_pFirstObject = Obj;
						}
						else {
							//Add the new image to the chain.
							m_pLastObj->m_pNext = Obj;
						}
						UpdateObject(Obj);

						AddUndo(Obj, true);

					}
				}

			}
		}
	}


	WindowName = aTitle;
	ProcessDragDrop(this, aTitle, m_pMainScene);

	//Context. 
	//Con text menu.
	//
	if (!m_pSelectedImage) {
		ImVec2 cp = ImGui::GetMousePos();
		cp -= ImGui::GetWindowPos();
		cp.x += ImGui::GetScrollX();
		cp.y += ImGui::GetScrollY();
		ImGui::SetCursorPos(cp);
		ImGui::Text(" "); //needed ?

		bool contextActive = false;
		if (!shift) { //&& ImGui::IsWindowFocused()  && ImGui::IsWindowHovered()
			//ImGui::SetNextWindowSize(ImVec2(200, 40));
			if (ImGui::BeginPopupContextItemAGK("SpriteContextMenu"))
			{
				contextActive = true;
				if (ImGui::MenuItem("Reset View")) {
					zoomAll = RESETZOOM;
					iDelayAction = 1;
				}
				if (m_pSelectedObject) {
					ImGui::Separator();
					if (ImGui::MenuItem("Reset Size")) {
						MediaScene::Images * m_pSearchImage;
						m_pSearchImage = m_pFirstImage;
						if (m_pSearchImage) {
							//search images.
							while (m_pSearchImage) {
								AddUndo(m_pSelectedObject, false);
								if (m_pSelectedObject->m_iImageId == m_pSearchImage->m_iImageId) {
									m_pSelectedObject->m_Size.x = (float) m_pSearchImage->m_iWidth;
									m_pSelectedObject->m_Size.y = (float) m_pSearchImage->m_iHeight;
									m_pSelectedObject->m_Scale.x = 1.0f;
									m_pSelectedObject->m_Scale.y = 1.0f;
									break;
								}
								m_pSearchImage = m_pSearchImage->m_pNext;
							}
						}
					}
					if (ImGui::MenuItem("Reset Rotation")) {
						AddUndo(m_pSelectedObject, false);
						m_pSelectedObject->m_Rotation.x = 0.0f;
					}
					if (ImGui::MenuItem("Send to Back")) {
						AddUndo(m_pSelectedObject, false);
						m_pSelectedObject->m_Depth = 10000;
					}
					if (ImGui::MenuItem("Move Backward")) {
						AddUndo(m_pSelectedObject, false);
						m_pSelectedObject->m_Depth++;
						if (m_pSelectedObject->m_Depth > 10000)
							m_pSelectedObject->m_Depth = 10000;
					}
					if (ImGui::MenuItem("Send to Front")) {
						AddUndo(m_pSelectedObject, false);
						m_pSelectedObject->m_Depth = 0;
					}
					if (ImGui::MenuItem("Move Forward")) {
						AddUndo(m_pSelectedObject, false);
						m_pSelectedObject->m_Depth--;
						if (m_pSelectedObject->m_Depth < 0)
							m_pSelectedObject->m_Depth = 0;
					}

					if (ImGui::MenuItem("Duplicate")) {
						CopyObject(m_pSelectedObject);
						PasteObject();
					}

					if (ImGui::MenuItem("Delete")) {
						Delete();
					}

					if (ImGui::MenuItem("Fit to Screen Size")) {
						AddUndo(m_pSelectedObject, false);
						m_pSelectedObject->m_Size.x = (float) iBaseWidth;
						m_pSelectedObject->m_Size.y = (float) iBaseHeight;
						m_pSelectedObject->m_Scale.x = 1.0f;
						m_pSelectedObject->m_Scale.y = 1.0f;
						m_pSelectedObject->m_Position.x = 0.0f;
						m_pSelectedObject->m_Position.y = 0.0f;
					}

					if (ImGui::MenuItem("Set Grid Size to Sprite Size")) {

						fGridX = m_pSelectedObject->m_Size.x * m_pSelectedObject->m_Scale.x;
						fGridY = m_pSelectedObject->m_Size.y * m_pSelectedObject->m_Scale.y;
						fGridXOffset = 0.0f;
						fGridYOffset = 0.0f;
						bGridOn = true;
					}

				}

				if (m_pSelectedObject) {
					ImGui::Separator();
					uString uLabel = "Lock: ";
					uLabel.Append(m_pSelectedObject->m_sName);
					if (ImGui::MenuItem(uLabel.GetStr())) {
						m_pSelectedObject->bLocked = true;
						m_pSelectedObject = NULL;
					}
				}
				if (m_pUnlockObject && m_pUnlockObject->bLocked) {
					ImGui::Separator();
					uString uLabel = "UnLock: ";
					uLabel.Append(m_pUnlockObject->m_sName);
					uLabel.Append("##1");
					if (ImGui::MenuItem(uLabel.GetStr())) {
						//Expand all
						m_pUnlockObject->bLocked = false;
						m_pUnlockObject = m_pUnlockObject2 = m_pUnlockObject3 = NULL;
					}
					if (m_pUnlockObject2) {
						uLabel = "UnLock: ";
						uLabel.Append(m_pUnlockObject2->m_sName);
						uLabel.Append("##2");
						if (ImGui::MenuItem(uLabel.GetStr())) {
							//Expand all
							m_pUnlockObject2->bLocked = false;
							m_pUnlockObject = m_pUnlockObject2 = m_pUnlockObject3 = NULL;
						}
					}
					if (m_pUnlockObject3) {
						uLabel = "UnLock: ";
						uLabel.Append(m_pUnlockObject3->m_sName);
						uLabel.Append("##3");
						if (ImGui::MenuItem(uLabel.GetStr())) {
							//Expand all
							m_pUnlockObject3->bLocked = false;
							m_pUnlockObject = m_pUnlockObject2 = m_pUnlockObject3 = NULL;
						}
					}
				}

				ImGui::EndPopup();
			}
		}
		if (!contextActive && agk::GetPointerState())
			m_pUnlockObject = m_pUnlockObject2 = m_pUnlockObject3 = NULL;
	}

	ImGui::GetStyle().WindowRounding = (float) iOldRounding;
	ImGui::EndChild();

	if (add_new_resolution)
		ProcessNewResolution(m_pMainScene);

	ProcessAtlas(m_editor);

	return(bCloseScene);
}


float MediaScene::myScreenToWorldX(float x)
{
	// center zooming
	x -= (iContentWide * 0.5f) / 2.0f;
	x /= agk::GetViewZoom();
	x += (iContentWide * 0.5f) + ImGui::GetScrollX();
	return x;
}
float MediaScene::myScreenToWorldY(float y)
{
	// center zooming
	y -= (iContentHeight * 0.5f);
	y /= agk::GetViewZoom();
	y += (iContentHeight * 0.5f) + ImGui::GetScrollY();
	return y;
}


void MediaScene::GetAspect( void )
{
	if(bFollowDeviceResolution) {
		fDisplayAspect = ((float)iScreenWidth / (float)iScreenHeight);
		fStretchValue = 1.0f;
		fScissorX = 0.0f;
		fScissorY = 0.0f;
		fScissorXScaled = 0.0f;
		fScissorYScaled = 0.0f;
		return;
	}
    fDisplayAspect = ((float)iBaseWidth / (float)iBaseHeight);
	fScreenAspect = ((float)iScreenWidth / (float)iScreenHeight);

	// work out if we need any borders to prevent image stretching
	float y = 0, yy = 0, height = (float)iScreenHeight, x = 0, xx = 0, width = (float)iScreenWidth;
	float deviceAspect = width / height;
	float aspect = fDisplayAspect;

	if (deviceAspect < aspect)
	{
		// top/bottom border needed
		height = iScreenWidth / aspect;

		// make height a whole number with the same evenness as the render height
		if (agk::Floor(height) % 2 == iScreenHeight % 2) height = (float)agk::Floor(height);
		else height = (float)agk::Ceil(height);

		y = (iScreenHeight - height) / 2.0f;
	}
	else if (deviceAspect > aspect)
	{
		// left/right border needed
		width = iScreenHeight * aspect;
		// make width a whole number with the same evenness as the render width
		if (agk::Floor(width) % 2 == iScreenWidth % 2) width = (float)agk::Floor(width);
		else width = (float)agk::Ceil(width);

		x = (iScreenWidth - width) / 2.0f;
	}

	//aspect = fScreenAspect;
/*
	if (deviceAspect < aspect)
	{
		height = iBaseWidth / aspect;
		if (agk::Floor(height) % 2 == iScreenHeight % 2) height = (float)agk::Floor(height);
		else height = (float)agk::Ceil(height);
		yy = (iScreenHeight - height) / 2.0f;
	}
	else if (deviceAspect > aspect)
	{
		width = iBaseHeight * aspect;
		if (agk::Floor(width) % 2 == iScreenWidth % 2) width = (float)agk::Floor(width);
		else width = (float)agk::Ceil(width);
		xx = (iScreenWidth - width) / 2.0f;
	}
*/
	fStretchValue = fDisplayAspect / ((float)iBaseWidth / (float)iBaseHeight);

	float diffX = iScreenWidth / (float) iBaseWidth;
	m_iDisplayExtraX = (int) (agk::Round(xx) * diffX);
	if (m_iDisplayExtraX > 0)
		m_iDisplayExtraX = -m_iDisplayExtraX;
	float diffY = iScreenHeight / (float) iBaseHeight;
	m_iDisplayExtraY = (int) (agk::Round(yy) * diffY);
	if (m_iDisplayExtraY > 0)
		m_iDisplayExtraY = -m_iDisplayExtraY;

	float m_fTargetViewportX = (float)agk::Round(x);
	float m_fTargetViewportY = (float)agk::Round(y);
	float m_fTargetViewportWidth = (float) iBaseWidth;
	float m_fTargetViewportHeight = (float) iBaseHeight;

	int m_iScissorX = agk::Round(m_fTargetViewportX);
	int m_iScissorY = agk::Round(m_fTargetViewportY);
	int m_iScissorWidth = agk::Round(m_fTargetViewportWidth + m_fTargetViewportX) - m_iScissorX;
	int m_iScissorHeight = agk::Round(m_fTargetViewportHeight + m_fTargetViewportY) - m_iScissorY;


	fScissorX = m_fTargetViewportX;
	fScissorY = m_fTargetViewportY;
	fScissorXScaled = m_fTargetViewportX * fRatio;
	fScissorYScaled = m_fTargetViewportY * fRatio;

	if (!bFollowDeviceResolution) {
		fScaleToScreenX = (float)iBaseWidth / (float)(iScreenWidth - fScissorX - fScissorX);
		fScaleToScreenY = (float)iBaseHeight / (float)(iScreenHeight - fScissorY - fScissorY);
	}
	else {
		fScaleToScreenX = 1.0f;
		fScaleToScreenY = 1.0f;
	}


}


bool MediaScene::DrawAGKSprite(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle, const ImVec4& drawCol_normal, const ImVec4& drawCol_hover, bool selected, MediaObject *Obj, bool drawselection, bool multipleselect)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGui::PushID(tex_id);
	const ImGuiID id = window->GetID("#image");
	ImGui::PopID();

	float cos_a = cosf(angle);
	float sin_a = sinf(angle);
	float rot_line_size = 50.0f*fRatio;
	if (rot_line_size > size.y*0.5f)
		rot_line_size = size.y*0.5f;

//	if (Obj->m_iMediaType == MEDIATYPE_TEXT) {
//		center -= (size);
//	}

	ImVec2 pos[10] =
	{
		center + ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a),

		center + ImRotate(ImVec2(+size.x * 0.5f, 0.0f), cos_a, sin_a),
		center + ImRotate(ImVec2(0.0f, +size.y * 0.5f), cos_a, sin_a),
		center + ImRotate(ImVec2(0.0f, -size.y*0.5f - (rot_line_size)), cos_a, sin_a),
		center + ImRotate(ImVec2(0.0f, -size.y*0.5f), cos_a, sin_a),
	};

	if (Obj->m_iMediaType == MEDIATYPE_TEXT) {
		pos[8] = pos[2];
		pos[9] = pos[0];
		pos[0] = pos[8] + ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a);
		pos[1] = pos[8] + ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a);
		pos[2] = pos[8] + ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a);
		pos[3] = pos[8] + ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a);


		pos[0] -= (size*0.5f);
		pos[1] -= (size*0.5f);
		pos[2] -= (size*0.5f);
		pos[3] -= (size*0.5f);
	}

	ImVec2 uvs[4] =
	{
		ImVec2(0.0f, 0.0f),
		ImVec2(1.0f, 0.0f),
		ImVec2(1.0f, 1.0f),
		ImVec2(0.0f, 1.0f)
	};

	if (Obj && Obj->m_pUsedImage && Obj->iAnimationFrom > 0) {
		float asx = agk::GetImageWidth(Obj->m_iImageId);
		float asy = agk::GetImageHeight(Obj->m_iImageId);

		float uvratiox = 1.0f / (asx);
		float uvratioy = 1.0f / (asy);
		float imgsizex = asx / Obj->m_pUsedImage->m_iRows;
		float imgsizey = asy / Obj->m_pUsedImage->m_iColumns;

		int index_x = (int)fmod(Obj->iAnimationFrom - 1, Obj->m_pUsedImage->m_iRows);
		int index_y = (Obj->iAnimationFrom - 1) / Obj->m_pUsedImage->m_iRows;


		float uvborderx = uvratiox;
		float uvbordery = uvratioy;
		uvborderx *= (imgsizex / 32);
		uvbordery *= (imgsizey / 32);


		float atlasstartx = (index_x * (imgsizex)) * uvratiox + (uvborderx*0.5f);
		float atlasstarty = (index_y * (imgsizey)) * uvratioy + (uvbordery*0.5f);

		float atlassizex = (imgsizex)* uvratiox -(uvborderx);
		float atlassizey = (imgsizey)* uvratioy -(uvbordery);

		uvs[0] = ImVec2(atlasstartx, atlasstarty);
		uvs[1] = ImVec2(atlasstartx + atlassizex, atlasstarty);
		uvs[2] = ImVec2(atlasstartx + atlassizex, atlasstarty + atlassizey);
		uvs[3] = ImVec2(atlasstartx, atlasstarty + atlassizey);
	}


	/*
	float test = 1.0f / 1280.0f;
	float imgsizex = 1280.0f / 5.0f;
	float imgsizey = 1280.0f / 5.0f;
	int atlasindex = 3;
	float atlasstartx = (atlasindex * imgsizex) * test;
	float atlasstarty = (atlasindex * imgsizey) * test;
	float atlassizex = (imgsizex-1.0f) * test;
	float atlassizey = (imgsizex-1.0f) * test;

	ImVec2 uvs[4] =
	{
		ImVec2(atlasstartx, atlasstarty),
		ImVec2(atlasstartx+ atlassizex, atlasstarty),
		ImVec2(atlasstartx + atlassizex, atlasstarty + atlassizey),
		ImVec2(atlasstartx, atlasstarty + atlassizey)
	};
	*/
	ImRect bb;
	
	bb = ImRect(center - (size*0.5f), center + (size*0.5f));

	for (int i = 0; i < 4; i++) {
		if (bb.Min.x > pos[i].x) bb.Min.x = pos[i].x;
		if (bb.Max.x < pos[i].x) bb.Max.x = pos[i].x;
		if (bb.Min.y > pos[i].y) bb.Min.y = pos[i].y;
		if (bb.Max.y < pos[i].y) bb.Max.y = pos[i].y;
	}

	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, id))
		return false;

//	draw_list->AddRect( ImVec2(bb.Min.x, bb.Min.y) , ImVec2(bb.Max.x, bb.Max.y) , ImGui::GetColorU32(drawCol_normal), 0);


	bool img_hovered = false;
	bool returnval = false;
	if( !ImGui::IsItemHovered() )
	{
		if(!drawselection && (Obj->m_iMediaType == MEDIATYPE_SPRITE ) )
			draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], ImGui::GetColorU32(drawCol_normal));
		if (!drawselection && Obj->m_iMediaType == MEDIATYPE_TEXT) {
			//Draw text
			ImGui::ImRotateStart();

			float usefontsize = size.y;
			ImGuiContext& g = *GImGui;
			extern ImFont* agkfont;

			//ImVec4 clipping = ImVec4(-8000.0f, -8000.0f, 8000.0f,  8000.0f );
//			if (Obj->m_iFontId > 0 && customfonts[Obj->m_iFontId] && customfonts[Obj->m_iFontId]->FontSize > 0.0f) {
//				draw_list->AddText(customfonts[Obj->m_iFontId], (size.y + 1.0f), ImVec2(pos[9].x, pos[9].y + 2.0f), ImGui::GetColorU32(drawCol_normal), Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());
//			}
//			else
//				draw_list->AddText(agkfont, (size.y + 1.5f), ImVec2(pos[9].x, pos[9].y - 1.0f), ImGui::GetColorU32(drawCol_normal), Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());


			ImVec4 clip_rect = { -2000.0f,-2000.0f,8000.0f,8000.0f };
			if (Obj->m_iFontId > 0 && customfonts[Obj->m_iFontId] && customfonts[Obj->m_iFontId]->FontSize > 0.0f && draw_list) {
				customfonts[Obj->m_iFontId]->RenderText(draw_list, (size.y + 1.0f), ImVec2(pos[9].x, pos[9].y + 2.0f), ImGui::GetColorU32(drawCol_normal), clip_rect, Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());
			}
			else if(agkfont->FontSize > 0.0f && draw_list)
				agkfont->RenderText(draw_list, (size.y + 1.5f), ImVec2(pos[9].x, pos[9].y - 1.0f), ImGui::GetColorU32(drawCol_normal), clip_rect, Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());

			//center
//			ImGui::ImRotateEnd(-angle + 33.0f, ImGui::ImRotationCenter());
			ImGui::ImRotateEnd(-angle + 32.99f , ImVec2(pos[9].x, pos[9].y) - (size*0.5f) );//center - (size*0.5f)
		}
		if (!drawselection && Obj->m_iMediaType == MEDIATYPE_EDITBOX) {
			//Draw text
			float usefontsize = size.y - 2.0f;
			ImGuiContext& g = *GImGui;
			extern ImFont* agkfont;

			ImColor dCol = ImColor((int)(Obj->m_BorderColor.x), (int)(Obj->m_BorderColor.y), (int)(Obj->m_BorderColor.z), (int)(Obj->m_BorderColor.w));

			ImVec2 bsize = ImVec2((float) Obj->m_iBorderSize, (float) Obj->m_iBorderSize) * fRatio;
			draw_list->AddRectFilled(pos[0]- bsize, pos[0] + ImVec2(size.x, size.y) + bsize, ImGui::GetColorU32((ImVec4)dCol), 0);

			draw_list->AddRectFilled(pos[0], pos[0] + ImVec2(size.x, size.y), ImGui::GetColorU32(drawCol_normal), 0);
			ImVec4 TextColor = { 0,0,0,255 };
			if (Obj) {
				float sc = 1.0f / 255.0f;
				TextColor.x = (int) Obj->m_TextColor.x * sc;
				TextColor.y = (int) Obj->m_TextColor.y * sc;
				TextColor.z = (int) Obj->m_TextColor.z * sc;
				TextColor.w = (int) Obj->m_TextColor.w * sc;
			}
			if (Obj->m_iTextSize > 0 && Obj->m_iTextSize < usefontsize)
				usefontsize = (float) Obj->m_iTextSize;

			draw_list->AddText(agkfont, usefontsize , ImVec2(pos[0].x, pos[0].y - 2.0f), ImGui::GetColorU32(TextColor) , Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());
		}
		if (!drawselection && Obj->m_iMediaType == MEDIATYPE_BUTTON) {

			draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], ImGui::GetColorU32(drawCol_normal));

			float mx = pos[0].x, my = pos[0].y;

			//Draw text
			ImGuiContext& g = *GImGui;
			extern ImFont* agkfont;
			float myfontsize = (size.x * 0.25f);
			if(size.y < size.x )
				myfontsize = (size.y * 0.25f);

			if (Obj->m_iImageIndex > 0) {
				if (Obj->m_pUsedImage && (Obj->m_pUsedImage->m_iWidth / Obj->m_pUsedImage->m_iHeight) <= 1.0f) {
					myfontsize = (size.y * 0.25f);
					//myfontsize++;
					//myfontsize++;
				}
				else {
					myfontsize = (size.y * 0.75f);
				}
			}
			else {
				myfontsize++;
				myfontsize++;
			}

			float mytextcenter = (Obj->m_Text.GetLength()*myfontsize) * 0.35f; //0.385f
			ImVec2 tcenter = pos[0] + ImVec2(size.x*0.5f, size.y*0.5f);
			tcenter.x -= ((mytextcenter*0.5f));
			tcenter.y -= myfontsize*0.5f;
			draw_list->AddText(agkfont, myfontsize, tcenter, IM_COL32_WHITE , Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());
		}

	}
	else
	{
		if (!drawselection && ( Obj->m_iMediaType == MEDIATYPE_SPRITE ) )
			draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], ImGui::GetColorU32(drawCol_hover));
		if (!drawselection && Obj->m_iMediaType == MEDIATYPE_TEXT) {
			//Draw text

			ImGui::ImRotateStart();

			ImGuiContext& g = *GImGui;
			extern ImFont* agkfont;
			//size.y
			//g.FontSize
//			ImVec4 clipping = ImVec4(0.0f, 0.0f, 2000.0f, 2000.0f);
//			if (Obj->m_iFontId > 0 && customfonts[Obj->m_iFontId] && customfonts[Obj->m_iFontId]->FontSize > 0.0f) {
//				draw_list->AddText(customfonts[Obj->m_iFontId], (size.y + 1.0f), ImVec2(pos[9].x, pos[9].y + 2.0f), ImGui::GetColorU32(drawCol_hover), Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength(),0.0f,&clipping);
//			}
//			else
//				draw_list->AddText(agkfont, (size.y + 1.5f ) , ImVec2(pos[9].x , pos[9].y-1.0f ), ImGui::GetColorU32(drawCol_hover), Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength(), 0.0f, &clipping);

			ImVec4 clip_rect = { -2000.0f,-2000.0f,8000.0f,8000.0f };
			if (Obj->m_iFontId > 0 && customfonts[Obj->m_iFontId] && customfonts[Obj->m_iFontId]->FontSize > 0.0f && draw_list) {
				customfonts[Obj->m_iFontId]->RenderText(draw_list, (size.y + 1.0f), ImVec2(pos[9].x, pos[9].y + 2.0f), ImGui::GetColorU32(drawCol_normal), clip_rect, Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());
			}
			else if (agkfont->FontSize > 0.0f && draw_list)
				agkfont->RenderText(draw_list, (size.y + 1.5f), ImVec2(pos[9].x, pos[9].y - 1.0f), ImGui::GetColorU32(drawCol_normal), clip_rect, Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());


//			ImGui::ImRotateEnd(-angle + 33.0f, ImGui::ImRotationCenter());
			ImGui::ImRotateEnd(-angle + 32.99f, ImVec2(pos[9].x, pos[9].y) - (size*0.5f));//center - (size*0.5f)
		}
		if (!drawselection && Obj->m_iMediaType == MEDIATYPE_EDITBOX) {
			//Draw text
			float usefontsize = size.y - 2.0f;
			ImGuiContext& g = *GImGui;
			extern ImFont* agkfont;

			ImColor dCol = ImColor((int)(Obj->m_BorderColor.x), (int)(Obj->m_BorderColor.y), (int)(Obj->m_BorderColor.z), (int)(Obj->m_BorderColor.w));

			ImVec2 bsize = ImVec2((float) Obj->m_iBorderSize, (float) Obj->m_iBorderSize) * fRatio;
			draw_list->AddRectFilled(pos[0] - bsize, pos[0] + ImVec2(size.x, size.y) + bsize, ImGui::GetColorU32((ImVec4)dCol), 0);

			draw_list->AddRectFilled(pos[0], pos[0] + ImVec2(size.x, size.y), ImGui::GetColorU32(drawCol_hover), 0);
			ImVec4 TextColor = { 0,0,0,255 };
			if (Obj) {
				TextColor.x = Obj->m_TextColor.x;
				TextColor.y = Obj->m_TextColor.y;
				TextColor.z = Obj->m_TextColor.z;
				TextColor.w = Obj->m_TextColor.w;
			}
			if (Obj->m_iTextSize > 0 && Obj->m_iTextSize < usefontsize)
				usefontsize = (float) Obj->m_iTextSize;

			draw_list->AddText(agkfont, usefontsize, ImVec2(pos[0].x, pos[0].y - 2.0f), ImGui::GetColorU32(TextColor), Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());
		}
		if (!drawselection && Obj->m_iMediaType == MEDIATYPE_BUTTON) {

			draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], ImGui::GetColorU32(drawCol_hover));

			//Draw text
			ImGuiContext& g = *GImGui;
			extern ImFont* agkfont;
			float myfontsize = (size.x * 0.25f);
			if (size.y < size.x)
				myfontsize = (size.y * 0.25f);

			if (Obj->m_iImageIndex > 0) {
				if (Obj->m_pUsedImage && (Obj->m_pUsedImage->m_iWidth / Obj->m_pUsedImage->m_iHeight) <= 1.0f) {
					myfontsize = (size.y * 0.25f);
					//myfontsize++;
					//myfontsize++;
				}
				else {
					myfontsize = (size.y * 0.75f);
				}
			}
			else {
				myfontsize++;
				myfontsize++;
			}
			float mytextcenter = (Obj->m_Text.GetLength()*myfontsize) * 0.35f;

			ImVec2 tcenter = pos[0] + ImVec2(size.x*0.5f, size.y*0.5f);
			tcenter.x -= ((mytextcenter*0.5f) );
			tcenter.y -= myfontsize*0.5f;
			draw_list->AddText(agkfont, myfontsize , tcenter, IM_COL32_WHITE , Obj->m_Text.GetStr(), Obj->m_Text.GetStr() + Obj->m_Text.GetLength());
		}

		img_hovered = true;
		returnval = true;
		if (!Obj->bLocked)
			bIsAnythingHovered = true;
	}

	if (img_hovered && Obj->bLocked )//img_hovered , !selected
	{
		if(!m_pUnlockObject)
			m_pUnlockObject = Obj;
		else if(!m_pUnlockObject2 && Obj != m_pUnlockObject )
			m_pUnlockObject2 = Obj;
		else if (!m_pUnlockObject3 && Obj != m_pUnlockObject && Obj != m_pUnlockObject2)
			m_pUnlockObject3 = Obj;
	}

	if (selected && (m_pSelectedObject || Obj->bIsSelected ) ) // && ImGui::IsWindowHovered()
	{
		ImVec2 oldCursor = ImGui::GetCursorPos();
		ImVec4 *style_colors = ImGui::GetStyle().Colors;
		float outlinesize = 0.1f;
		pos[0].x -= outlinesize;
		pos[0].y -= outlinesize;
		pos[1].x += outlinesize;
		pos[1].y -= outlinesize;
		pos[2].x += outlinesize;
		pos[2].y += outlinesize;
		pos[3].x -= outlinesize;
		pos[3].y += outlinesize;

		//ImGuiCol_ButtonHovered
		//ImGuiCol_Text
		float lsize = 4.0f*fRatio;
		if (lsize > 4.0f)
			lsize = 4.0f;
		draw_list->AddQuad(pos[0], pos[1], pos[2], pos[3], ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram])), lsize);

		if (!multipleselect && Obj->m_iMediaType == MEDIATYPE_SPRITE)
			draw_list->AddLine(pos[6], pos[7], ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram])), lsize);


		//Add controls.
		//SmallButton
		float fs = ImGui::GetFontSize();
		//		int butsize = fs * 1.5f  * fRatio;
		float butsize = ((fs*0.80f) * fRatio);
		//static bool firstmousepress = false;
		static int mouse_action = 0;

		if (!multipleselect) {

			//Change:
			//ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button

			ImVec4 oldbutA = style_colors[ImGuiCol_ButtonActive];
			ImVec4 oldbutH = style_colors[ImGuiCol_ButtonHovered];
			ImVec4 oldbutN = style_colors[ImGuiCol_Button];

			style_colors[ImGuiCol_Button] = vGridColor;
			style_colors[ImGuiCol_ButtonHovered] = vGridColor;
			style_colors[ImGuiCol_ButtonHovered].w *= 0.80f;
			style_colors[ImGuiCol_ButtonActive] = vGridColor;
			int butloop = 7;
			if (Obj->m_iMediaType == MEDIATYPE_TEXT)
				butloop = 4;
			if (Obj->m_iMediaType == MEDIATYPE_BUTTON || Obj->m_iMediaType == MEDIATYPE_EDITBOX)
				butloop = 6;
			for (int i = 0; i < butloop; i++) {
				if (butsize < fs*0.4f) //0.75f
					butsize = fs*0.4f;
				if (butsize > fs*1.25f)
					butsize = fs*1.25f;

				pos[i].x += ImGui::GetScrollX();
				pos[i].y += ImGui::GetScrollY();
				pos[i] -= ImGui::GetWindowPos();
				pos[i].x -= (butsize*0.5f);
				pos[i].y -= (butsize*0.5f);
				ImGui::SetCursorPos(pos[i]);

				uString uniqueId = " ";
				if (i <= 3) uniqueId = " "; //ICON_MD_FULLSCREEN
				if (i == 4) uniqueId = " "; //ICON_MD_KEYBOARD_ARROW_RIGHT;
				if (i == 5) uniqueId = " "; //ICON_MD_KEYBOARD_ARROW_DOWN;
				if (i == 6) uniqueId = " "; //ICON_MD_REDO; //ICON_MD_SCREEN_ROTATION;
				uniqueId.Append("##leftupadjust");

				uniqueId.AppendInt(i);
				ImGui::SetItemAllowOverlap();
				//FramePadding
				ImVec2 fp = ImGui::GetStyle().FramePadding;
				ImGui::GetStyle().FramePadding = ImVec2(0, 0);

				ImGui::ImRotateStart();

				if (ImGui::ButtonEx(uniqueId.GetStr(), ImVec2(butsize, butsize), 0)) { //ImGuiButtonFlags_Repeat
					mouse_action = i + 1;
					bInChangingMode = true;
					img_hovered = true;
				}

				ImGui::ImRotateEnd(-angle + 33.0f, ImGui::ImRotationCenter());

				if (ImGui::IsItemHovered() && !firstmousepress) {
					if (agk::GetPointerState()) {
						bInChangingMode = true;
						mouse_action = i + 1; //i-2
						img_hovered = true;
					}
					if (!Obj->bLocked)
						bIsAnythingHovered = true;
				}
				ImGui::GetStyle().FramePadding = fp;
			}

			style_colors[ImGuiCol_Button] = oldbutN;
			style_colors[ImGuiCol_ButtonHovered] = oldbutH;
			style_colors[ImGuiCol_ButtonActive] = oldbutA;
		}

		if (!Obj->bLocked && ImGui::IsWindowFocused() && !mMultipleSelectActive && m_pSelectedObject ) {
			static ImVec2 mousestartpos;
			static Vec4 oldposition, oldscale, oldsize, oldrotation;
			ImVec2 mouseendpos = ImGui::GetMousePos();
			ImVec2 diff;
			float scalex, scaley;

			if (img_hovered || firstmousepress) {
				if (agk::GetPointerState()) {
					if (!firstmousepress || m_pChangingObject != m_pSelectedObject) {
						firstmousepress = true;
						mousestartpos = ImGui::GetMousePos();
						oldposition = m_pSelectedObject->m_Position;
						oldscale = m_pSelectedObject->m_Scale;
						oldsize = m_pSelectedObject->m_Size;
						oldrotation = m_pSelectedObject->m_Rotation;

						m_pChangingObject = m_pSelectedObject;
					}
					else if (m_pChangingObject == m_pSelectedObject) {
						diff = mouseendpos - mousestartpos;
						scalex = fScaleToScreenX;
						scaley = fScaleToScreenY;
						diff.x /= fRatio;
						diff.y /= fRatio;
						diff.x *= scalex;
						diff.y *= scaley;
					}
				}
			}

			if ((img_hovered || firstmousepress) && mouse_action >= 1 && mouse_action <= 7 ) {
				//Resize.
				if (agk::GetPointerState()) {
					if (firstmousepress && m_pChangingObject == m_pSelectedObject) {
						if (mouseendpos.x != mousestartpos.x || mouseendpos.y != mousestartpos.y) {


							//( (360.0f / PI) * 0.5f)

							float fval = (diff.x + diff.y) * 0.01f;;

							if (mouse_action == 7) {

								//diff = center - mousestartpos;
								ImVec2 c = center;
								diff = c - mouseendpos; // -(mousestartpos);
								scalex = fScaleToScreenX;
								scaley = fScaleToScreenY;
								diff.x /= fRatio;
								diff.y /= fRatio;
								diff.x *= scalex;
								diff.y *= scaley;
								//diff = center - mousestartpos;
								//center

								float delta_y = diff.y; //origin_y - mouse_y;
								float delta_x = diff.x; //origin_x - mouse_x;
								float angle_deg = (atan2(delta_y, delta_x)*180.0000f) / PI;
								angle_deg -= 90.0f;
								if (angle_deg < 0.0f) angle_deg += 360.0f;
								if (angle_deg > 360.0f) angle_deg -= 360.0f;

								m_pSelectedObject->m_Rotation.x = angle_deg;
								if(m_pSelectedObject->m_iMediaType == MEDIATYPE_SPRITE)
									agk::SetSpriteAngle(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Rotation.x);
							}
							else {

								fval = diff.x;
								if (abs(diff.y) > abs(fval))
									fval = diff.y;

								if (fval < 0)
									fval = -AlignXtoGrid(abs(fval), true);
								else
									fval = AlignXtoGrid(fval, true);


								float fvalx = fval;// *oldscale.x;
								float fvaly = fval;// *oldscale.y;

								if (mouse_action == 3) {

									diff = mouseendpos - mousestartpos;
									diff.x *= fScaleToScreenX;
									diff.y *= fScaleToScreenY;
									diff /= fRatio;

									float imgratioy = (oldsize.x / oldsize.y);
									if ((oldsize.y / oldsize.x) < imgratioy)
										imgratioy = (oldsize.y / oldsize.x);
									float imgratiox = (oldsize.y / oldsize.x);
									if ((oldsize.y / oldsize.x) < imgratiox)
										imgratiox = (oldsize.y / oldsize.x);

									fval = (diff.x*imgratiox) + (diff.y*imgratioy);
									fval *= 0.5f;

									if (fval < 0)
										fval = -AlignXtoGrid(abs(fval), true);
									else
										fval = AlignXtoGrid(fval, true);

									fvalx = (oldsize.x / oldsize.y)*fval;
									if (fvalx < -(oldsize.x - 1.0f))
										fvalx = -(oldsize.x - 1.0f);
									fvaly = fval;
									if (fvaly < -(oldsize.y - 1.0f))
										fvaly = -(oldsize.y - 1.0f);
									if (bSnapResizeToGrid) {
										if (oldsize.x + fvalx < (fGridX + fGridXOffset))
											fvalx = -(oldsize.x - (fGridX + fGridXOffset));
										//											fvalx = 0.0f;
										if (oldsize.y + fvaly < (fGridY + fGridYOffset))
											fvaly = -(oldsize.y - (fGridY + fGridYOffset));
										//											fvaly = 0.0f;
									}
									m_pSelectedObject->m_Size.x = oldsize.x + fvalx;
									m_pSelectedObject->m_Size.y = oldsize.y + fvaly;

								}
								else if (mouse_action == 1) {

									diff = mousestartpos - mouseendpos;
									diff.x *= fScaleToScreenX;
									diff.y *= fScaleToScreenY;
									diff /= fRatio;

									float imgratioy = (oldsize.x / oldsize.y);
									if ((oldsize.y / oldsize.x) < imgratioy)
										imgratioy = (oldsize.y / oldsize.x);
									float imgratiox = (oldsize.y / oldsize.x);
									if ((oldsize.y / oldsize.x) < imgratiox)
										imgratiox = (oldsize.y / oldsize.x);

									fval = (diff.x*imgratiox) + (diff.y*imgratioy);
									fval *= 0.5f;

									if (fval < 0)
										fval = -AlignXtoGrid(abs(fval), true);
									else
										fval = AlignXtoGrid(fval, true);

									fvalx = (oldsize.x / oldsize.y)*fval;
									if (fvalx < -(oldsize.x - 1.0f))
										fvalx = -(oldsize.x - 1.0f);
									fvaly = fval;
									if (fvaly < -(oldsize.y - 1.0f))
										fvaly = -(oldsize.y - 1.0f);

									float fvalposx = fvalx;
									float fvalposy = fvaly;
									if (bSnapResizeToGrid) {
										if (oldsize.x + fvalx < (fGridX + fGridXOffset)) {
											fvalx = -(oldsize.x - (fGridX + fGridXOffset));
											fvalposx = -(oldsize.x - (fGridX + fGridXOffset));
										}
										if (oldsize.y + fvaly < (fGridY + fGridYOffset)) {
											fvaly = -(oldsize.y - (fGridY + fGridYOffset));
											fvalposy = -(oldsize.y - (fGridY + fGridYOffset));
										}
									}

									m_pSelectedObject->m_Position.x = oldposition.x - fvalposx;
									m_pSelectedObject->m_Position.y = oldposition.y - fvalposy;

									m_pSelectedObject->m_Size.x = oldsize.x + fvalx;
									m_pSelectedObject->m_Size.y = oldsize.y + fvaly;

								}
								else if (mouse_action == 2) {

									diff.x = mouseendpos.x - mousestartpos.x;
									diff.y = mousestartpos.y - mouseendpos.y;

									diff.x *= fScaleToScreenX;
									diff.y *= fScaleToScreenY;
									diff /= fRatio;

									float imgratioy = (oldsize.x / oldsize.y);
									if ((oldsize.y / oldsize.x) < imgratioy)
										imgratioy = (oldsize.y / oldsize.x);
									float imgratiox = (oldsize.y / oldsize.x);
									if ((oldsize.y / oldsize.x) < imgratiox)
										imgratiox = (oldsize.y / oldsize.x);

									fval = (diff.x*imgratiox) + (diff.y*imgratioy);
									fval *= 0.5f;

									if (fval < 0)
										fval = -AlignXtoGrid(abs(fval), true);
									else
										fval = AlignXtoGrid(fval, true);

									fvalx = (oldsize.x / oldsize.y)*fval;
									if (fvalx < -(oldsize.x - 1.0f))
										fvalx = -(oldsize.x - 1.0f);
									fvaly = fval;
									if (fvaly < -(oldsize.y - 1.0f))
										fvaly = -(oldsize.y - 1.0f);

									float fvalposx = fvalx;
									float fvalposy = fvaly;
									if (bSnapResizeToGrid) {
										if (oldsize.x + fvalx < (fGridX + fGridXOffset)) {
											fvalx = -(oldsize.x - (fGridX + fGridXOffset));
											fvalposx = -(oldsize.x - (fGridX + fGridXOffset));
										}
										if (oldsize.y + fvaly < (fGridY + fGridYOffset)) {
											fvaly = -(oldsize.y - (fGridY + fGridYOffset));
											fvalposy = -(oldsize.y - (fGridY + fGridYOffset));
										}
									}

									//m_pSelectedObject->m_Position.x = oldposition.x - fvalposx;
									m_pSelectedObject->m_Position.y = oldposition.y - fvalposy;

									m_pSelectedObject->m_Size.x = oldsize.x + fvalx;
									m_pSelectedObject->m_Size.y = oldsize.y + fvaly;

								}
								else if (mouse_action == 4) {

									diff.x = mousestartpos.x - mouseendpos.x;
									diff.y = mouseendpos.y - mousestartpos.y;

									diff.x *= fScaleToScreenX;
									diff.y *= fScaleToScreenY;
									diff /= fRatio;

									float imgratioy = (oldsize.x / oldsize.y);
									if ((oldsize.y / oldsize.x) < imgratioy)
										imgratioy = (oldsize.y / oldsize.x);
									float imgratiox = (oldsize.y / oldsize.x);
									if ((oldsize.y / oldsize.x) < imgratiox)
										imgratiox = (oldsize.y / oldsize.x);

									fval = (diff.x*imgratiox) + (diff.y*imgratioy);
									fval *= 0.5f;

									if (fval < 0)
										fval = -AlignXtoGrid(abs(fval), true);
									else
										fval = AlignXtoGrid(fval, true);

									fvalx = (oldsize.x / oldsize.y)*fval;
									if (fvalx < -(oldsize.x - 1.0f))
										fvalx = -(oldsize.x - 1.0f);
									fvaly = fval;
									if (fvaly < -(oldsize.y - 1.0f))
										fvaly = -(oldsize.y - 1.0f);

									float fvalposx = fvalx;
									float fvalposy = fvaly;
									if (bSnapResizeToGrid) {
										if (oldsize.x + fvalx < (fGridX + fGridXOffset)) {
											fvalx = -(oldsize.x - (fGridX + fGridXOffset));
											fvalposx = -(oldsize.x - (fGridX + fGridXOffset));
										}
										if (oldsize.y + fvaly < (fGridY + fGridYOffset)) {
											fvaly = -(oldsize.y - (fGridY + fGridYOffset));
											fvalposy = -(oldsize.y - (fGridY + fGridYOffset));
										}
									}

									m_pSelectedObject->m_Position.x = oldposition.x - fvalposx;
									//m_pSelectedObject->m_Position.y = oldposition.y - fvalposy;

									m_pSelectedObject->m_Size.x = oldsize.x + fvalx;
									m_pSelectedObject->m_Size.y = oldsize.y + fvaly;

								}
								//								if ( (oldsize.x + fval) > 0.1f ) {
								else if (1 == 1) {
									if (mouse_action <= 4 || mouse_action == 5)
										m_pSelectedObject->m_Size.x = AlignXtoGrid(oldsize.x + fvalx, true);
									if (mouse_action <= 4 || mouse_action == 6)
										m_pSelectedObject->m_Size.y = AlignYtoGrid(oldsize.y + fvaly, true);
								}
								else if (1 == 2 && mouse_action == 1) {

									if ((oldposition.x + fvalx) < ((oldposition.x + (oldsize.x*oldscale.x)) - 1.0f)) {
										m_pSelectedObject->m_Position.x = oldposition.x + fvalx; //*0.5f
										m_pSelectedObject->m_Position.y = oldposition.y + fvaly;
										float sx = oldposition.x + (oldsize.x*oldscale.x) - fvalx;
										float sy = oldposition.y + (oldsize.y*oldscale.y) - fvaly;
										m_pSelectedObject->m_Size.x = sx;
										m_pSelectedObject->m_Size.y = sy;

										if (m_pSelectedObject->m_Size.x < 1)
											m_pSelectedObject->m_Size.x = 1.0f;
										if (m_pSelectedObject->m_Size.y < 1)
											m_pSelectedObject->m_Size.y = 1.0f;
									}
								}
								else {
									if (mouse_action <= 4 || mouse_action == 5)
										m_pSelectedObject->m_Size.x = oldsize.x + fval;
									if (mouse_action <= 4 || mouse_action == 6)
										m_pSelectedObject->m_Size.y = oldsize.y + fval;
								}

								if (m_pSelectedObject->m_iMediaType == MEDIATYPE_SPRITE) {
									if (bUsePercentage) {
										agk::SetSpriteSize(m_pSelectedObject->m_iSprite, (m_pSelectedObject->m_Size.x / (float)iBaseWidth)*100.0f, (m_pSelectedObject->m_Size.y / (float)iBaseHeight)*100.0f );
										agk::SetSpritePosition(m_pSelectedObject->m_iSprite, (m_pSelectedObject->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSelectedObject->m_Position.y / (float)iBaseHeight)*100.0f);
									}
									else {
										agk::SetSpriteSize(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Size.x, m_pSelectedObject->m_Size.y);
										agk::SetSpritePosition(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Position.x, m_pSelectedObject->m_Position.y);
									}
								}

								//								}
							}
						}
					}
				}
				else {
					if (firstmousepress) {
						firstmousepress = false;
						if (mouse_action == 7) {
							//undo here (rotation)
							if (m_pChangingObject != m_pSelectedObject) {
								//Restore old rotation
								m_pChangingObject->m_Rotation.x = oldrotation.x;
								if (m_pChangingObject->m_iMediaType == MEDIATYPE_SPRITE)
									agk::SetSpriteAngle(m_pChangingObject->m_iSprite, m_pChangingObject->m_Rotation.x);
							}
							else if (oldrotation.x != m_pSelectedObject->m_Rotation.x) {
								Vec4 newrotation = m_pSelectedObject->m_Rotation;
								m_pSelectedObject->m_Rotation.x = oldrotation.x;
								AddUndo(m_pSelectedObject, false);
								m_pSelectedObject->m_Rotation = newrotation;
								if (m_pSelectedObject->m_iMediaType == MEDIATYPE_SPRITE)
									agk::SetSpriteAngle(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Rotation.x);

								iDelayChanges = 3;
							}

						}
						else {
							//Validate that this is the correct object.
							if (m_pChangingObject != m_pSelectedObject) {
								//Something changed restore old pos.
//								m_pChangingObject->m_Scale = oldscale;
//								agk::SetSpriteScale(m_pChangingObject->m_iSprite, m_pChangingObject->m_Scale.x, m_pChangingObject->m_Scale.y);
								m_pChangingObject->m_Size = oldsize;
								if (m_pChangingObject->m_iMediaType == MEDIATYPE_SPRITE) {
									if (bUsePercentage) {
										agk::SetSpriteSize(m_pChangingObject->m_iSprite, (m_pChangingObject->m_Size.x / (float)iBaseWidth)*100.0f, (m_pChangingObject->m_Size.y / (float)iBaseHeight)*100.0f);
									}
									else {
										agk::SetSpriteSize(m_pChangingObject->m_iSprite, m_pChangingObject->m_Size.x, m_pChangingObject->m_Size.y);
									}
								}
								m_pChangingObject->m_Position = oldposition;
								if (m_pChangingObject->m_iMediaType == MEDIATYPE_SPRITE) {
									if (bUsePercentage) {
										agk::SetSpritePosition(m_pChangingObject->m_iSprite, (m_pChangingObject->m_Position.x / (float)iBaseWidth)*100.0f, (m_pChangingObject->m_Position.y / (float)iBaseHeight)*100.0f);
									}
									else {
										agk::SetSpritePosition(m_pChangingObject->m_iSprite, m_pChangingObject->m_Position.x, m_pChangingObject->m_Position.y);
									}
								}
							}
							else if (oldsize.x != m_pSelectedObject->m_Size.x || oldsize.y != m_pSelectedObject->m_Size.y)
							{
//								Vec4 newscale = m_pSelectedObject->m_Scale;
//								m_pSelectedObject->m_Scale = oldscale;
//								AddUndo(m_pSelectedObject, false);
//								m_pSelectedObject->m_Scale = newscale;
//								agk::SetSpriteScale(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Scale.x, m_pSelectedObject->m_Scale.y);

								Vec4 newsize = m_pSelectedObject->m_Size;
								Vec4 newposition = m_pSelectedObject->m_Position;
								m_pSelectedObject->m_Size = oldsize;
								m_pSelectedObject->m_Position = oldposition;
								AddUndo(m_pSelectedObject, false);
								m_pSelectedObject->m_Size = newsize;
								if (m_pSelectedObject->m_iMediaType == MEDIATYPE_SPRITE) {
									if (bUsePercentage) {
										agk::SetSpriteSize(m_pSelectedObject->m_iSprite, (m_pSelectedObject->m_Size.x / (float)iBaseWidth)*100.0f, (m_pSelectedObject->m_Size.y / (float)iBaseHeight)*100.0f);
									}
									else {
										agk::SetSpriteSize(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Size.x, m_pSelectedObject->m_Size.y);
									}
								}
								m_pSelectedObject->m_Position = newposition;
								if (m_pSelectedObject->m_iMediaType == MEDIATYPE_SPRITE) {
									if (bUsePercentage) {
										agk::SetSpriteSize(m_pSelectedObject->m_iSprite, (m_pSelectedObject->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSelectedObject->m_Position.y / (float)iBaseHeight)*100.0f);
									}
									else {
										agk::SetSpriteSize(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Position.x, m_pSelectedObject->m_Position.y);
									}
								}

								iDelayChanges = 3;
							}
						}
						ide_force_rendering_delayed = true;
					}
				}

			}

			if (!agk::GetPointerState()) {
				mouse_action = 0;
				//			bIsAnythingHovered = false;
			}

			if ((img_hovered || firstmousepress) && mouse_action == 0) {
				//Move Sprite.
				if (agk::GetPointerState()) {
					if (firstmousepress && m_pChangingObject == m_pSelectedObject) {
						if (mouseendpos.x != mousestartpos.x || mouseendpos.y != mousestartpos.y) {
							bInChangingMode = true;
							m_pSelectedObject->m_Position.x = oldposition.x + diff.x;
							m_pSelectedObject->m_Position.y = oldposition.y + diff.y;

							if (bSnapToGrid && bGridOn) {
								float test = 32.0f;
								int align = (int) (round(m_pSelectedObject->m_Position.x) / fGridX); //fGridX
								if (m_pSelectedObject->m_Position.x < 0.0f)
									align -= 1;
								m_pSelectedObject->m_Position.x = align * fGridX;
								m_pSelectedObject->m_Position.x += fGridXOffset;
								
								align = (int) (round(m_pSelectedObject->m_Position.y) / fGridY); //fGridY
								if (m_pSelectedObject->m_Position.y < 0.0f)
									align -= 1;
								m_pSelectedObject->m_Position.y = align * fGridY;
								m_pSelectedObject->m_Position.y += fGridYOffset;
							}
							if (m_pSelectedObject->m_iMediaType == MEDIATYPE_SPRITE) {
								if (bUsePercentage) {
									agk::SetSpritePosition(m_pSelectedObject->m_iSprite, (m_pSelectedObject->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSelectedObject->m_Position.y / (float)iBaseHeight)*100.0f);
								}
								else {
									agk::SetSpritePosition(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Position.x, m_pSelectedObject->m_Position.y);
								}
							}
						}
					}

				}
				else {
					if (firstmousepress) {
						firstmousepress = false;
						//Validate that this is the correct object.
						if (m_pChangingObject != m_pSelectedObject) {
							//Something changed restore old pos.
							m_pChangingObject->m_Position = oldposition;
							if (m_pChangingObject->m_iMediaType == MEDIATYPE_SPRITE) {
								if (bUsePercentage) {
									agk::SetSpritePosition(m_pChangingObject->m_iSprite, (m_pChangingObject->m_Position.x / (float)iBaseWidth)*100.0f, (m_pChangingObject->m_Position.y / (float)iBaseHeight)*100.0f);
								}
								else {
									agk::SetSpritePosition(m_pChangingObject->m_iSprite, m_pChangingObject->m_Position.x, m_pChangingObject->m_Position.y);
								}
							}
						}
						else if (oldposition.x != m_pSelectedObject->m_Position.x || oldposition.y != m_pSelectedObject->m_Position.y)
						{
							Vec4 newposition = m_pSelectedObject->m_Position;
							m_pSelectedObject->m_Position = oldposition;
							AddUndo(m_pSelectedObject, false);
							m_pSelectedObject->m_Position = newposition;
							if (m_pSelectedObject->m_iMediaType == MEDIATYPE_SPRITE) {
								if (bUsePercentage) {
									agk::SetSpritePosition(m_pSelectedObject->m_iSprite, (m_pSelectedObject->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSelectedObject->m_Position.y / (float)iBaseHeight)*100.0f);
								}
								else {
									agk::SetSpritePosition(m_pSelectedObject->m_iSprite, m_pSelectedObject->m_Position.x, m_pSelectedObject->m_Position.y);
								}
							}
							iDelayChanges = 3;
						}
						ide_force_rendering_delayed = true;
					}
				}
			}
		}

//		ImGui::ArrowButton("SS1", 1);

		ImGui::SetCursorPos(oldCursor);
	}
	return(returnval);
}

void MediaScene::ProcessNewResolution(MediaScene * m_pMainScene)
{

	ImGui::OpenPopup("Create New Resolution.");
	if (ImGui::BeginPopupModal("Create New Resolution.", &add_new_resolution , ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("New Resolution:");
		static int new_resolution_width = 0;
		ImGui::InputInt("Resolution Width: ", &new_resolution_width, 8, 32);
		static int new_resolution_height = 0;
		ImGui::InputInt("Resolution Height: ", &new_resolution_height, 8, 32);

		if (ImGui::Button(" Cancel ")) {
			add_new_resolution = false;
		}
		ImGui::SameLine();
		if (ImGui::Button(" Add Resolution ")) {
			if (new_resolution_height > 0 && new_resolution_width > 0) {
				uString resname = "C ";
				resname.AppendInt(new_resolution_width);
				resname.Append("x");
				resname.AppendInt(new_resolution_height);
				m_pMainScene->Resolution.insert(std::make_pair(std::string( resname.GetStr() ), Resolutions{ new_resolution_width, new_resolution_height,true, "Custom" }));
				
				if (m_SceneEditor)
					m_SceneEditor->filechanged = true;
				add_new_resolution = false;
			}
		}

		ImGui::EndPopup();

	}
}

void AddEditorLine(TextEditor * m_editor, const char *add)
{
	TextEditor::Coordinates pos;
	pos.mColumn = 0;
	pos.mLine = m_editor->GetTotalLines();
	m_editor->SetCursorPosition(pos);
	m_editor->InsertText(add);
}

void MediaScene::AddSceneCleanup(MediaScene * m_pSearchScene, TextEditor * m_editor, MediaScene * m_pMainScene)
{
	if (!m_pSearchScene)
		return;
	if (!m_editor)
		return;

	char tmp[MAX_PATH];
	uString scenename;
	scenename = m_pSearchScene->m_sSceneName;
	scenename.Trim(" ");
	scenename.ReplaceStr(" ", "_");
	scenename.ReplaceStr(",", "");
	scenename.ReplaceStr("#", "");
	scenename.ReplaceStr("!", "");

	uString mainscenename;
	mainscenename = m_pMainScene->m_sSceneName;
	mainscenename.Trim(" ");
	mainscenename.ReplaceStr(" ", "_");
	mainscenename.ReplaceStr(",", "");
	mainscenename.ReplaceStr("#", "");
	mainscenename.ReplaceStr("!", "");


	sprintf(tmp, "function %s_cleanup()\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	AddEditorLine(m_editor, "\tlocal i as integer\n");

	sprintf(tmp, "\tif %s_loaded = 0 then exitfunction\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	
	MediaScene::MediaObject * m_pSearchObj = NULL;
	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	if (m_pSearchObj) {

		sprintf(tmp, "\tfor i = 0 to %s_sprites.length\n", scenename.GetStr());
		AddEditorLine(m_editor, tmp);

		sprintf(tmp, "\t\tif GetSpriteExists( %s_sprites[i] ) then DeleteSprite( %s_sprites[i] )\n", scenename.GetStr(), scenename.GetStr());
		AddEditorLine(m_editor, tmp);

		AddEditorLine(m_editor, "\tnext i\n");


		if (iTextCount > 1) {
			sprintf(tmp, "\tfor i = 0 to %s_text.length\n", scenename.GetStr());
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "\t\tif GetTextExists( %s_text[i] ) then DeleteText( %s_text[i] )\n", scenename.GetStr(), scenename.GetStr());
			AddEditorLine(m_editor, tmp);
			AddEditorLine(m_editor, "\tnext i\n");

		}

		MediaScene::MediaObject * m_pSearchObj = NULL;
		m_pSearchObj = m_pSearchScene->m_pFirstObject;
		bool buttonactive = false;
		bool editboxactive = false;
		while (m_pSearchObj) {
			if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {
				buttonactive = true;
			}

			if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
				editboxactive = true;
			}
			m_pSearchObj = m_pSearchObj->m_pNext;
		}

		if (buttonactive && iButtonCount > 1) {
			sprintf(tmp, "\tfor i = 1 to %s_button.length-1\n", scenename.GetStr());
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "\t\tif GetVirtualButtonExists( %s_button[i] ) then DeleteVirtualButton( %s_button[i] )\n", scenename.GetStr(), scenename.GetStr());
			AddEditorLine(m_editor, tmp);
			AddEditorLine(m_editor, "\tnext i\n");
		}
		if (editboxactive && iEditBoxCount > 1) {
			sprintf(tmp, "\tfor i = 0 to %s_editbox.length\n", scenename.GetStr());
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "\t\tif GetEditBoxExists( %s_editbox[i] ) then DeleteEditBox( %s_editbox[i] )\n", scenename.GetStr(), scenename.GetStr());
			AddEditorLine(m_editor, tmp);
			AddEditorLine(m_editor, "\tnext i\n");
		}


		sprintf(tmp, "\t\tif GetSpriteExists(%s_fadesprite) then DeleteSprite(%s_fadesprite)\n", mainscenename.GetStr(), mainscenename.GetStr());
		AddEditorLine(m_editor, tmp);
		sprintf(tmp, "\t\t%s_fadesprite = 0\n", mainscenename.GetStr());
		AddEditorLine(m_editor, tmp);
		sprintf(tmp, "\t\tif GetImageExists(%s_fadeimage) then DeleteImage(%s_fadeimage)\n", mainscenename.GetStr(), mainscenename.GetStr());
		AddEditorLine(m_editor, tmp);

	}

	//Add all scene images.
	MediaScene::Images * m_pSearchImage;
	m_pSearchImage = m_pSearchScene->m_pFirstImage;
	if (m_pSearchImage) {
		//delete images.
		sprintf(tmp, "\tfor i = 0 to %s_images.length\n", scenename.GetStr());
		AddEditorLine(m_editor, tmp);
		sprintf(tmp, "\t\tif GetImageExists( %s_images[i] ) then DeleteImage( %s_images[i] )\n", scenename.GetStr(), scenename.GetStr());
		AddEditorLine(m_editor, tmp);
		AddEditorLine(m_editor, "\tnext i\n");
	}

	if (m_pSearchScene->Fonts.size() > 0) {
		//delete fonts
		sprintf(tmp, "\tfor i = 0 to %s_fonts.length\n", scenename.GetStr());
		AddEditorLine(m_editor, tmp);
		sprintf(tmp, "\t\tif GetFontExists( %s_fonts[i] ) then DeleteFont( %s_fonts[i] )\n", scenename.GetStr(), scenename.GetStr());
		AddEditorLine(m_editor, tmp);
		AddEditorLine(m_editor, "\tnext i\n");
	}

	std::vector<std::string> array_groups;
	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	while (m_pSearchObj) {
		if (!m_pSearchObj->m_bRemoved) {
			if (m_pSearchObj->m_sArrayGroup.GetLength() > 0) {

				bool bAlreadyThere = false;
				for (std::vector<std::string>::iterator it = array_groups.begin(); it != array_groups.end(); ++it) {
					if (it->size() > 0) {
						uString cmp, cmp2;
						cmp = m_pSearchObj->m_sArrayGroup;
						cmp2 = it->c_str();
						if (cmp.CompareCaseTo(cmp2) == 0) {
							bAlreadyThere = true;
							break;
						}
					}
				}
				if (!bAlreadyThere)
					array_groups.push_back(m_pSearchObj->m_sArrayGroup.GetStr());
			}
		}
		m_pSearchObj = m_pSearchObj->m_pNext;
	}

	//Add group arrays.
	for (std::vector<std::string>::iterator it = array_groups.begin(); it != array_groups.end(); ++it) {
		if (it->size() > 0) {
			sprintf(tmp, "\t%s.length = -1\n", it->c_str());
			AddEditorLine(m_editor, tmp);
		}
	}

	sprintf(tmp, "\t%s_loaded = 0\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	AddEditorLine(m_editor, "endfunction\n\n");

}


void MediaScene::AddSceneFade(MediaScene * m_pSearchScene, TextEditor * m_editor, MediaScene * m_pMainScene)
{
	if (!m_pSearchScene)
		return;
	if (!m_editor)
		return;

	char tmp[MAX_PATH];
	uString scenename;
	scenename = m_pSearchScene->m_sSceneName;
	scenename.Trim(" ");
	scenename.ReplaceStr(" ", "_");
	scenename.ReplaceStr(",", "");
	scenename.ReplaceStr("#", "");
	scenename.ReplaceStr("!", "");


	uString mainscenename;
	mainscenename = m_pMainScene->m_sSceneName;
	mainscenename.Trim(" ");
	mainscenename.ReplaceStr(" ", "_");
	mainscenename.ReplaceStr(",", "");
	mainscenename.ReplaceStr("#", "");
	mainscenename.ReplaceStr("!", "");


	sprintf(tmp, "function %s_fade( percent as float )\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);
	AddEditorLine(m_editor, "\tlocal fMul as float\n");
	AddEditorLine(m_editor, "\tlocal i as integer\n");

	AddEditorLine(m_editor, "\tfMul = percent / 100.0\n"); //PE: this is AGK code :)
	AddEditorLine(m_editor, "\tif fMul > 1.0 then fMul = 1.0\n");
	AddEditorLine(m_editor, "\tif fMul < 0.0 then fMul = 0.0\n");

	AddEditorLine(m_editor, "\tif fMul = 1.0\n");
	//mainscenename

	sprintf(tmp, "\t\tif GetSpriteExists(%s_fadesprite) then DeleteSprite(%s_fadesprite)\n", mainscenename.GetStr() , mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "\t\t%s_fadesprite = 0\n", mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "\t\tif GetImageExists(%s_fadeimage) then DeleteImage(%s_fadeimage)\n", mainscenename.GetStr(), mainscenename.GetStr() );
	AddEditorLine(m_editor, tmp);
	AddEditorLine(m_editor, "\t\texitfunction\n");
	AddEditorLine(m_editor, "\tendif\n");

	sprintf(tmp, "\tif %s_loaded = 0 then %s_setup()\n", scenename.GetStr(), scenename.GetStr());
	AddEditorLine(m_editor, tmp);


	sprintf(tmp, "\tif not GetSpriteExists( %s_fadesprite )\n", mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "\t\t%s_fadeimage = CreateImageColor(0,0,0,255)\n", mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "\t\t%s_fadesprite = CreateSprite(%s_fadeimage)\n", mainscenename.GetStr(), mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "\t\tSetSpriteDepth(%s_fadesprite,0)\n", mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "\t\tSetSpritePosition(%s_fadesprite,0,0)\n", mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "\t\tSetSpriteSize(%s_fadesprite,GetDeviceWidth(),GetDeviceHeight())\n", mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);
	AddEditorLine(m_editor, "\tendif\n");
	sprintf(tmp, "\tSetSpriteColor(%s_fadesprite,0,0,0,255-255*fMul)\n", mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);

	MediaScene::MediaObject * m_pSearchObj = NULL;
	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	bool buttonactive = false;
	while (m_pSearchObj) {
		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {
			buttonactive = true;
			break;
		}
		m_pSearchObj = m_pSearchObj->m_pNext;
	}

	if (buttonactive && iButtonCount > 1) {
		sprintf(tmp, "\tfor i = 1 to %s_button.length-1\n", scenename.GetStr());
		AddEditorLine(m_editor, tmp);
		//Missing command to set virtual button text color ?
		sprintf(tmp, "\t\tif GetVirtualButtonExists( %s_button[i] ) then SetVirtualButtonAlpha(%s_button[i],255*fMul)\n", scenename.GetStr(), scenename.GetStr());
		AddEditorLine(m_editor, tmp);
		AddEditorLine(m_editor, "\tnext i\n");
	}

	AddEditorLine(m_editor, "endfunction\n\n");

	//Other needed functions.
	sprintf(tmp, "function %s_GetNextVirtualButton( )\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);
	AddEditorLine(m_editor, "\tlocal i as integer\n");
	AddEditorLine(m_editor, "\tlocal ret as integer\n");
	AddEditorLine(m_editor, "\tret = 1\n");
	AddEditorLine(m_editor, "\tfor i = 1 to 100\n");
	AddEditorLine(m_editor, "\t\tif not GetVirtualButtonExists( i )\n");
	AddEditorLine(m_editor, "\t\t\tret = i\n");
	AddEditorLine(m_editor, "\t\t\texit\n");
	AddEditorLine(m_editor, "\t\tendif\n");
	AddEditorLine(m_editor, "\tnext i\n");
	AddEditorLine(m_editor, "endfunction ret\n\n");

}

void MediaScene::AddSceneGlobals(MediaScene * m_pSearchScene, TextEditor * m_editor)
{
	if (!m_pSearchScene)
		return;
	if (!m_editor)
		return;

	char tmp[MAX_PATH];
	uString scenename;
	scenename = m_pSearchScene->m_sSceneName;
	scenename.Trim(" ");
	scenename.ReplaceStr(" ", "_");
	scenename.ReplaceStr(",", "");
	scenename.ReplaceStr("#", "");
	scenename.ReplaceStr("!", "");

	sprintf(tmp, "global %s_change_window = 1\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	sprintf(tmp, "global %s_loaded = 0\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);


	//Add all scene images.
	MediaScene::Images * m_pSearchImage;
	m_pSearchImage = m_pSearchScene->m_pFirstImage;
	if (m_pSearchImage) {
		sprintf(tmp, "global %s_images as integer[%d]\n", scenename.GetStr(), m_pSearchScene->iImageCount);
		AddEditorLine(m_editor, tmp);
	}

	//Add fonts
	if (m_pSearchScene->Fonts.size() > 0) {
		sprintf(tmp, "global %s_fonts as integer[%d]\n", scenename.GetStr(), m_pSearchScene->iFontCount);
		AddEditorLine(m_editor, tmp);
	}


	//Find real _sprites count.
	MediaScene::MediaObject * m_pSearchObj = NULL;
	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	int sprites = 1;
	int text = 1;
	int button = 1;
	int editbox = 1;
	while (m_pSearchObj) {
		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT) {
			text++;
		}
		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {
			button++;
		}
		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
			editbox++;
		}
		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
			sprites++;
		}
		m_pSearchObj = m_pSearchObj->m_pNext;
	}

	//MediaScene::MediaObject * m_pSearchObj = NULL;
	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	if (m_pSearchObj) {
		sprintf(tmp, "global %s_sprites as integer[%d]\n", scenename.GetStr(), sprites); //m_pSearchScene->iSpriteCount
		AddEditorLine(m_editor, tmp);

		sprintf(tmp, "global %s_sprites_count_x as float[%d]\n", scenename.GetStr(), sprites); //m_pSearchScene->iSpriteCount
		AddEditorLine(m_editor, tmp);
		sprintf(tmp, "global %s_sprites_count_y as float[%d]\n", scenename.GetStr(), sprites); //m_pSearchScene->iSpriteCount
		AddEditorLine(m_editor, tmp);

		if (iTextCount > 1) {
			sprintf(tmp, "global %s_text as integer[%d]\n", scenename.GetStr(), text); //m_pSearchScene->iTextCount
			AddEditorLine(m_editor, tmp);
		}

		int iButtonsAvail = 1;
		m_pSearchObj = m_pSearchScene->m_pFirstObject;
		while (m_pSearchObj) {
			if (!m_pSearchObj->m_bRemoved) {
				if (m_pSearchObj->m_iMediaType==MEDIATYPE_BUTTON)
					iButtonsAvail++;
			}
			m_pSearchObj = m_pSearchObj->m_pNext;
		}

		if (iButtonsAvail > 1) {
			//Recalc iButtonCount
			sprintf(tmp, "global %s_button as integer[%d]\n", scenename.GetStr(), iButtonsAvail);
			AddEditorLine(m_editor, tmp);
		}
		if (editbox > 1) { //iEditBoxCount
			sprintf(tmp, "global %s_editbox as integer[%d]\n", scenename.GetStr(), editbox); //m_pSearchScene->iEditBoxCount
			AddEditorLine(m_editor, tmp);
		}
	}



	std::vector<std::string> array_groups;
	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	while (m_pSearchObj) {
		if (!m_pSearchObj->m_bRemoved) {
			if (m_pSearchObj->m_sArrayGroup.GetLength() > 0) {

				bool bAlreadyThere = false;
				for (std::vector<std::string>::iterator it = array_groups.begin(); it != array_groups.end(); ++it) {
					if (it->size() > 0) {
						uString cmp, cmp2;
						cmp = m_pSearchObj->m_sArrayGroup;
						cmp2 = it->c_str();
						if (cmp.CompareCaseTo(cmp2) == 0) {
							bAlreadyThere = true;
							break;
						}
					}
				}
				if(!bAlreadyThere)
					array_groups.push_back(m_pSearchObj->m_sArrayGroup.GetStr());
			}
		}
		m_pSearchObj = m_pSearchObj->m_pNext;
	}

	//Add group arrays.
	for (std::vector<std::string>::iterator it = array_groups.begin(); it != array_groups.end(); ++it) {
		if (it->size() > 0) {
			sprintf(tmp, "global %s as integer[]\n", it->c_str());
			AddEditorLine(m_editor, tmp);
		}
	}

	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	while (m_pSearchObj) {
		if (!m_pSearchObj->m_bRemoved) {
			uString clean = m_pSearchObj->m_sVariable;
			if (clean.GetLength() > scenename.GetLength() ) {
				sprintf(tmp, "global %s as integer\n", m_pSearchObj->m_sVariable.GetStr());
				AddEditorLine(m_editor, tmp);
			}
		}
		m_pSearchObj = m_pSearchObj->m_pNext;
	}
	

}

void MediaScene::AddSceneSyncFunction(MediaScene * m_pSearchScene, TextEditor * m_editor, MediaScene * m_pMainScene)
{
	if (!m_pSearchScene)
		return;
	if (!m_editor)
		return;

	char tmp[MAX_PATH];
	uString scenename;
	scenename = m_pSearchScene->m_sSceneName;
	scenename.Trim(" ");
	scenename.ReplaceStr(" ", "_");
	scenename.ReplaceStr(",", "");
	scenename.ReplaceStr("#", "");
	scenename.ReplaceStr("!", "");

	uString mainscenename;
	mainscenename = m_pMainScene->m_sSceneName;
	mainscenename.Trim(" ");
	mainscenename.ReplaceStr(" ", "_");
	mainscenename.ReplaceStr(",", "");
	mainscenename.ReplaceStr("#", "");
	mainscenename.ReplaceStr("!", "");


	sprintf(tmp, "function %s_sync()\n", scenename.GetStr() );
	AddEditorLine(m_editor, tmp);

//	AddEditorLine(m_editor, "\t//Add sync code here.\n");
	sprintf(tmp, "\tif %s_loaded = 0 then %s_setup()\n", scenename.GetStr(), scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	sprintf(tmp, "\t%s_tween# = 60/(1.0/GetFrameTime())\n", mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);

	MediaScene::MediaObject * m_pSearchObj = NULL;
	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	int sprites = 1;
	int text = 1;
	int button = 1;
	int editbox = 1;
	while (m_pSearchObj) {

		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT) {
			if (m_pSearchObj->bFixToScreen) {
				if (bUsePercentage) {
					sprintf(tmp, "\tSetTextPosition(%s_text[%d],%f + GetViewOffsetX() , %f + GetViewOffsetY() )\n", scenename.GetStr(), text, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetTextPosition(%s_text[%d],%f + GetViewOffsetX() , %f + GetViewOffsetY() )\n", scenename.GetStr(), text, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
					AddEditorLine(m_editor, tmp);
				}
			}
			text++;
		}
		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {
			if (m_pSearchObj->bFixToScreen) {
				if (bUsePercentage) {
					sprintf(tmp, "\tSetVirtualButtonPosition(%s_text[%d],%f + GetViewOffsetX() , %f + GetViewOffsetY() )\n", scenename.GetStr(), button, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetVirtualButtonPosition(%s_text[%d],%f + GetViewOffsetX() , %f + GetViewOffsetY() )\n", scenename.GetStr(), button, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
					AddEditorLine(m_editor, tmp);
				}
			}
			button++;
		}
		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
			if (m_pSearchObj->bFixToScreen) {
				if (bUsePercentage) {
					sprintf(tmp, "\tSetEditBoxPosition(%s_editbox[%d],%f + GetViewOffsetX() , %f + GetViewOffsetY() )\n", scenename.GetStr(), editbox, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetEditBoxPosition(%s_editbox[%d],%f + GetViewOffsetX() , %f + GetViewOffsetY() )\n", scenename.GetStr(), editbox, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
					AddEditorLine(m_editor, tmp);
				}
			}
			editbox++;
		}
		if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
			if (m_pSearchObj->bFixToScreen) {

				if (bUsePercentage) {
					sprintf(tmp, "\tSetSpritePosition(%s_sprites[%d],%f + GetViewOffsetX() , %f + GetViewOffsetY() )\n", scenename.GetStr(), sprites, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetSpritePosition(%s_sprites[%d],%f + GetViewOffsetX() , %f + GetViewOffsetY() )\n", scenename.GetStr(), sprites, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
					AddEditorLine(m_editor, tmp);
				}

			}

			if (m_pSearchObj->bUVOffset && !(m_pSearchObj->fUVSpeedX == 0.0f && m_pSearchObj->fUVSpeedY == 0.0f))
			{
				sprintf(tmp, "\t%s_sprites_count_x[%d] = %s_sprites_count_x[%d] + ( %f * %s_tween# )\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites , m_pSearchObj->fUVSpeedX, mainscenename.GetStr());
				AddEditorLine(m_editor, tmp);
				sprintf(tmp, "\tif %s_sprites_count_x[%d] > 1.0 then %s_sprites_count_x[%d] = %s_sprites_count_x[%d] - 1.0\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites, scenename.GetStr(), sprites);
				AddEditorLine(m_editor, tmp);
				sprintf(tmp, "\tif %s_sprites_count_x[%d] < 1.0 then %s_sprites_count_x[%d] = %s_sprites_count_x[%d] + 1.0\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites, scenename.GetStr(), sprites);
				AddEditorLine(m_editor, tmp);

				sprintf(tmp, "\t%s_sprites_count_y[%d] = %s_sprites_count_y[%d] + ( %f * %s_tween# )\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites, m_pSearchObj->fUVSpeedY, mainscenename.GetStr());
				AddEditorLine(m_editor, tmp);
				sprintf(tmp, "\tif %s_sprites_count_y[%d] > 1.0 then %s_sprites_count_y[%d] = %s_sprites_count_y[%d] - 1.0\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites, scenename.GetStr(), sprites);
				AddEditorLine(m_editor, tmp);
				sprintf(tmp, "\tif %s_sprites_count_y[%d] < 1.0 then %s_sprites_count_y[%d] = %s_sprites_count_y[%d] + 1.0\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites, scenename.GetStr(), sprites);
				AddEditorLine(m_editor, tmp);

				if (m_pSearchObj->fUVSpeedY == 0.0f)
					sprintf(tmp, "\tSetSpriteUVOffset( %s_sprites[%d] , %s_sprites_count_x[%d] , 0.0 )\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites);
				else if (m_pSearchObj->fUVSpeedX == 0.0f)
					sprintf(tmp, "\tSetSpriteUVOffset( %s_sprites[%d] , 0.0 , %s_sprites_count_y[%d] )\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites);
				else
					sprintf(tmp, "\tSetSpriteUVOffset( %s_sprites[%d] , %s_sprites_count_x[%d] , %s_sprites_count_y[%d] )\n", scenename.GetStr(), sprites, scenename.GetStr(), sprites, scenename.GetStr(), sprites);
				AddEditorLine(m_editor, tmp);

			}

			sprites++;
		}
		m_pSearchObj = m_pSearchObj->m_pNext;
	}

	AddEditorLine(m_editor, "endfunction\n\n");
}

void MediaScene::AddSceneDialogFunction(MediaScene * m_pSearchScene, TextEditor * m_editor)
{
	if (!m_pSearchScene)
		return;
	if (!m_editor)
		return;

	char tmp[MAX_PATH];
	uString scenename;
	scenename = m_pSearchScene->m_sSceneName;
	scenename.Trim(" ");
	scenename.ReplaceStr(" ", "_");
	scenename.ReplaceStr(",", "");
	scenename.ReplaceStr("#", "");
	scenename.ReplaceStr("!", "");

	sprintf(tmp, "function %s_dialog()\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	AddEditorLine(m_editor, "\t//Add dialog code here.\n");

	sprintf(tmp, "\tif %s_loaded = 0 then %s_setup()\n", scenename.GetStr(), scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	AddEditorLine(m_editor, "endfunction\n\n");
}
//m_pMainScene->iBaseWidth
void MediaScene::AddSceneSetupFunction(MediaScene * m_pSearchScene, TextEditor * m_editor, MediaScene * m_pMainScene)
{
	if (!m_pSearchScene)
		return;
	if (!m_editor)
		return;
	if (!m_pMainScene)
		return;

	char tmp[MAX_PATH];
	uString scenename;
	scenename = m_pSearchScene->m_sSceneName;
	scenename.Trim(" ");
	scenename.ReplaceStr(" ", "_");
	scenename.ReplaceStr(",", "");
	scenename.ReplaceStr("#", "");
	scenename.ReplaceStr("!", "");

	uString mainscenename;
	mainscenename = m_pMainScene->m_sSceneName;
	mainscenename.Trim(" ");
	mainscenename.ReplaceStr(" ", "_");
	mainscenename.ReplaceStr(",", "");
	mainscenename.ReplaceStr("#", "");
	mainscenename.ReplaceStr("!", "");


	sprintf(tmp, "\nfunction %s_create_window( winflag as integer )\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "\t%s_change_window = winflag\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);
	AddEditorLine(m_editor, "endfunction\n\n");


	sprintf(tmp, "function %s_setup()\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	sprintf(tmp, "\tif %s_loaded = 1 then exitfunction\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	sprintf(tmp, "\t%s_loaded = 1\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	sprintf(tmp, "\tif %s_change_window = 1\n", scenename.GetStr());
	AddEditorLine(m_editor, tmp);

	if (bUsePercentage) {
		sprintf(tmp, "\t\tSetVirtualResolution(100,100)\n");
		AddEditorLine(m_editor, tmp);
		sprintf(tmp, "\t\tSetDisplayAspect(%f)\n" , (float) ((float)iBaseWidth / (float)iBaseHeight) );
		AddEditorLine(m_editor, tmp);
	}
	else {
		if (m_pMainScene->bFollowDeviceResolution) {
			sprintf(tmp, "\t\tSetVirtualResolution(%s_device_width,%s_device_height)\n", mainscenename.GetStr(), mainscenename.GetStr());
			AddEditorLine(m_editor, tmp);
		}
		else {
			sprintf(tmp, "\t\tSetVirtualResolution(%s_base_width,%s_base_height)\n", mainscenename.GetStr(), mainscenename.GetStr());
			AddEditorLine(m_editor, tmp);
			//	sprintf(tmp, "\tSetVirtualResolution(%s_device_width,%s_device_height)\n", mainscenename.GetStr(), mainscenename.GetStr());
			//	AddEditorLine(m_editor, tmp);
		}
	}
	sprintf(tmp, "\t\tSetWindowSize(%s_device_width,%s_device_height,0)\n", mainscenename.GetStr(), mainscenename.GetStr());
	AddEditorLine(m_editor, tmp);

//	sprintf(tmp, "\tSetWindowSize(%s_base_width,%s_base_height,0)\n", mainscenename.GetStr(), mainscenename.GetStr());
//	AddEditorLine(m_editor, tmp);

	sprintf(tmp, "\t\tSetClearColor(%d,%d,%d)\n", (int) m_pSearchScene->iSceneBackColor[0], (int) m_pSearchScene->iSceneBackColor[1], (int) m_pSearchScene->iSceneBackColor[2] );
	AddEditorLine(m_editor, tmp);

	AddEditorLine(m_editor, "\tendif\n");

	if (m_pSearchScene->fPhysicsGravityX != 0.0f || m_pSearchScene->fPhysicsGravityY != 50.0f)
	{
		if (bUsePercentage) {
			sprintf(tmp, "\tSetPhysicsGravity(%f,%f)\n", (float) (m_pSearchScene->fPhysicsGravityX / (float)iBaseWidth)*100.0f, (float) (m_pSearchScene->fPhysicsGravityY / (float)iBaseHeight)*100.0f);
			AddEditorLine(m_editor, tmp);
		}
		else {
			sprintf(tmp, "\tSetPhysicsGravity(%f,%f)\n", (float)m_pSearchScene->fPhysicsGravityX, (float)m_pSearchScene->fPhysicsGravityY);
			AddEditorLine(m_editor, tmp);
		}
	}

	if (m_pSearchScene->iPhysicsMaxPolygonPoints != 8) {
		sprintf(tmp, "\tSetPhysicsMaxPolygonPoints(%d)\n", (int)m_pSearchScene->iPhysicsMaxPolygonPoints);
		AddEditorLine(m_editor, tmp);
	}


	if (m_pSearchScene->bPhysicsWallTop)
		AddEditorLine(m_editor, "\tSetPhysicsWallTop(1)\n");
	else
		AddEditorLine(m_editor, "\tSetPhysicsWallTop(0)\n");
	if (m_pSearchScene->bPhysicsWallBottom)
		AddEditorLine(m_editor, "\tSetPhysicsWallBottom(1)\n");
	else
		AddEditorLine(m_editor, "\tSetPhysicsWallBottom(0)\n");
	if (m_pSearchScene->bPhysicsWallLeft)
		AddEditorLine(m_editor, "\tSetPhysicsWallLeft(1)\n");
	else
		AddEditorLine(m_editor, "\tSetPhysicsWallLeft(0)\n");
	if (m_pSearchScene->bPhysicsWallRight)
		AddEditorLine(m_editor, "\tSetPhysicsWallRight(1)\n");
	else
		AddEditorLine(m_editor, "\tSetPhysicsWallRight(0)\n");


	//Add all scene images.
	MediaScene::Images * m_pSearchImage;
	m_pSearchImage = m_pSearchScene->m_pFirstImage;
	if (m_pSearchImage) {
		//search images.
		while (m_pSearchImage) {
			bool used = false;
			//only save if used.
			MediaScene::MediaObject * m_pSearchObj = NULL;
			MediaScene::MediaObject * m_pLastObj = NULL;
			m_pSearchObj = m_pSearchScene->m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if (!m_pSearchObj->m_bRemoved) {
						if (m_pSearchObj->m_iImageIndex == m_pSearchImage->m_iImageIndex || m_pSearchObj->m_iImageDownIndex == m_pSearchImage->m_iImageIndex) {
							used = true;
							break;
						}
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}
			if (used) {
				sprintf(tmp, "\t%s_images[%d] = LoadImage(\"%s\")\n", scenename.GetStr(), m_pSearchImage->m_iImageIndex, m_pSearchImage->m_sRelativePath.GetStr());
				AddEditorLine(m_editor, tmp);

				if (m_pSearchImage->m_bAtlas) {

					sprintf(tmp, "\tSetImageMinFilter(%s_images[%d],0)\n", scenename.GetStr(), m_pSearchImage->m_iImageIndex);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\tSetImageMagFilter(%s_images[%d],0)\n", scenename.GetStr(), m_pSearchImage->m_iImageIndex);
					AddEditorLine(m_editor, tmp);

				}
			}

			m_pSearchImage = m_pSearchImage->m_pNext;
		}
	}


	//Add all fonts.
	if (m_pSearchScene->Fonts.size() > 0) {
		for (std::map<std::int32_t, std::string>::iterator it = m_pSearchScene->Fonts.begin(); it != m_pSearchScene->Fonts.end(); ++it)
		{

			bool fontinuse = false;

			//CHECK if this font is in use HERE!
			//Add all scene sprites.
			MediaScene::MediaObject * m_pSearchObj = NULL;
			m_pSearchObj = m_pSearchScene->m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if (!m_pSearchObj->m_bRemoved) {

						if (m_pSearchObj->m_iFontId == it->first) {
							//In use.
							fontinuse = true;
							break;
						}
					}

					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}

			if (fontinuse) {
				uString FontPath = it->second.c_str();
				FontPath.ReplaceStr("\\", "/");
				uString agk_path = FontPath;
				int pos = FontPath.FindStr("/media/", 1);
				if (pos > 0) {
					FontPath.SubString(agk_path, pos + 7);
				}

				sprintf(tmp, "\t%s_fonts[%d] = LoadFont(\"%s\")\n", scenename.GetStr(), it->first, agk_path.GetStr());
				AddEditorLine(m_editor, tmp);

			}
		}
	}



	//Add all scene sprites.
	MediaScene::MediaObject * m_pSearchObj = NULL;
	int sprites = 1;
	int text = 1;
	int button = 1;
	int editbox = 1;
	m_pSearchObj = m_pSearchScene->m_pFirstObject;
	if (m_pSearchObj) {
		while (m_pSearchObj) {
			if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT) {

				sprintf(tmp, "\t%s_text[%d] = CreateText( \"%s\" )\n", scenename.GetStr(), text, m_pSearchObj->m_Text.GetStr());
				AddEditorLine(m_editor, tmp);

				uString clean = m_pSearchObj->m_sVariable;
				if (clean.GetLength() > scenename.GetLength()) {
					sprintf(tmp, "\t%s = %s_text[%d]\n", m_pSearchObj->m_sVariable.GetStr(), scenename.GetStr(), text);
					AddEditorLine(m_editor, tmp);
				}

				clean = m_pSearchObj->m_sArrayGroup;
				if (m_pSearchObj->m_sArrayGroup.GetLength() > 0) {
					if (clean.GetLength() > scenename.GetLength()) {

						sprintf(tmp, "\t%s.insert( %s_text[%d] )\n", m_pSearchObj->m_sArrayGroup.GetStr(), scenename.GetStr(), text);
						AddEditorLine(m_editor, tmp);

					}
				}

				if (bUsePercentage) {
					sprintf(tmp, "\tSetTextPosition(%s_text[%d],%f,%f)\n", scenename.GetStr(), text, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);

					sprintf(tmp, "\tSetTextSize(%s_text[%d],%f)\n", scenename.GetStr(), text, (m_pSearchObj->m_Size.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetTextPosition(%s_text[%d],%f,%f)\n", scenename.GetStr(), text, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
					AddEditorLine(m_editor, tmp);

					sprintf(tmp, "\tSetTextSize(%s_text[%d],%f)\n", scenename.GetStr(), text, m_pSearchObj->m_Size.y);
					AddEditorLine(m_editor, tmp);
				}
				if (m_pSearchObj->m_Color.x != 255.0f || m_pSearchObj->m_Color.y != 255.0f || m_pSearchObj->m_Color.z != 255.0f || m_pSearchObj->m_Color.w != 255.0f) {
					sprintf(tmp, "\tSetTextColor(%s_text[%d],%d,%d,%d,%d)\n", scenename.GetStr(), (int)text, (int)m_pSearchObj->m_Color.x, (int)m_pSearchObj->m_Color.y, (int)m_pSearchObj->m_Color.z, (int)m_pSearchObj->m_Color.w);
					AddEditorLine(m_editor, tmp);
				}


				if (m_pSearchObj->m_Rotation.x != 0.0f) {
					sprintf(tmp, "\tSetTextAngle(%s_text[%d],%f)\n", scenename.GetStr(), (int)text, m_pSearchObj->m_Rotation.x);
					AddEditorLine(m_editor, tmp);

				}
				sprintf(tmp, "\tSetTextDepth(%s_text[%d],%d)\n", scenename.GetStr(), (int)text, m_pSearchObj->m_Depth);
				AddEditorLine(m_editor, tmp);

				if (m_pSearchObj->m_iFontId > 0) {
					sprintf(tmp, "\tSetTextFont(%s_text[%d],  %s_fonts[%d] )\n", scenename.GetStr(), (int)text, scenename.GetStr() , m_pSearchObj->m_iFontId );
					AddEditorLine(m_editor, tmp);
				}

				if (m_pSearchObj->m_bVisible) {
					sprintf(tmp, "\tSetTextVisible(%s_text[%d],1)\n", scenename.GetStr(), (int)text);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetTextVisible(%s_text[%d],0)\n", scenename.GetStr(), (int)text);
					AddEditorLine(m_editor, tmp);
				}

				text++;
			}

			if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {

				sprintf(tmp, "\t%s_button[%d] = %s_GetNextVirtualButton()\n", scenename.GetStr(), button, scenename.GetStr());
				AddEditorLine(m_editor, tmp);

				sprintf(tmp, "\tAddVirtualButton(%s_button[%d],20,20,40)\n", scenename.GetStr() , button);
				AddEditorLine(m_editor, tmp);

				sprintf(tmp, "\tSetVirtualButtonText(%s_button[%d],\"%s\")\n", scenename.GetStr(), button, m_pSearchObj->m_Text.GetStr() );
				AddEditorLine(m_editor, tmp);

				uString clean = m_pSearchObj->m_sVariable;
				if (clean.GetLength() > scenename.GetLength()) {
					sprintf(tmp, "\t%s = %s_button[%d]\n", m_pSearchObj->m_sVariable.GetStr(), scenename.GetStr(), button);
					AddEditorLine(m_editor, tmp);
				}

				if (bUsePercentage) {
					sprintf(tmp, "\tSetVirtualButtonPosition(%s_button[%d],%f,%f)\n", scenename.GetStr(), button, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\tSetVirtualButtonSize(%s_button[%d],%f,%f)\n", scenename.GetStr(), button, (float) (m_pSearchObj->m_Size.x / (float)iBaseWidth)*100.0f, (float) (m_pSearchObj->m_Size.y / (float)iBaseWidth)*100.0f);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetVirtualButtonPosition(%s_button[%d],%f,%f)\n", scenename.GetStr(), button, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
					AddEditorLine(m_editor, tmp);

					sprintf(tmp, "\tSetVirtualButtonSize(%s_button[%d],%f,%f)\n", scenename.GetStr(), button, (float) m_pSearchObj->m_Size.x , (float) m_pSearchObj->m_Size.y);
					AddEditorLine(m_editor, tmp);
				}

				if (m_pSearchObj->m_Color.x != 255.0f || m_pSearchObj->m_Color.y != 255.0f || m_pSearchObj->m_Color.z != 255.0f || m_pSearchObj->m_Color.w != 255.0f) {

					sprintf(tmp, "\tSetVirtualButtonAlpha(%s_button[%d],%d)\n", scenename.GetStr(), (int)button, (int)m_pSearchObj->m_Color.w);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\tSetVirtualButtonColor(%s_button[%d],%d,%d,%d)\n", scenename.GetStr(), (int)button, (int)m_pSearchObj->m_Color.x, (int)m_pSearchObj->m_Color.y, (int)m_pSearchObj->m_Color.z);
					AddEditorLine(m_editor, tmp);
				}

				//sprintf(tmp, "\tSetVirtualButtonDepth(%s_button[%d],%d)\n", scenename.GetStr(), (int)button, m_pSearchObj->m_Depth);
				//AddEditorLine(m_editor, tmp);

				if (m_pSearchObj->m_bVisible) {
					sprintf(tmp, "\tSetVirtualButtonVisible(%s_button[%d],1)\n", scenename.GetStr(), (int)button);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetVirtualButtonVisible(%s_button[%d],0)\n", scenename.GetStr(), (int)button);
					AddEditorLine(m_editor, tmp);
				}

				if (m_pSearchObj->m_iImageIndex > 0) {
					sprintf(tmp, "\tSetVirtualButtonImageUp(%s_button[%d],%s_images[%d])\n", scenename.GetStr(), (int)button , scenename.GetStr(), m_pSearchObj->m_iImageIndex);
					AddEditorLine(m_editor, tmp);
				}
				if (m_pSearchObj->m_iImageDownIndex > 0) {
					sprintf(tmp, "\tSetVirtualButtonImageDown(%s_button[%d],%s_images[%d])\n", scenename.GetStr(), (int)button, scenename.GetStr(), m_pSearchObj->m_iImageDownIndex);
					AddEditorLine(m_editor, tmp);
				}

				//SetVirtualButtonImageUp

				button++;
			}

			if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {

				sprintf(tmp, "\t%s_editbox[%d] = CreateEditBox()\n", scenename.GetStr(), editbox);
				AddEditorLine(m_editor, tmp);


				sprintf(tmp, "\tSetEditBoxText(%s_editbox[%d],\"%s\")\n", scenename.GetStr(), editbox, m_pSearchObj->m_Text.GetStr());
				AddEditorLine(m_editor, tmp);

				uString clean = m_pSearchObj->m_sVariable;
				if (clean.GetLength() > scenename.GetLength()) {
					sprintf(tmp, "\t%s = %s_editbox[%d]\n", m_pSearchObj->m_sVariable.GetStr(), scenename.GetStr(), editbox);
					AddEditorLine(m_editor, tmp);
				}

				if (bUsePercentage) {
					sprintf(tmp, "\tSetEditBoxPosition(%s_editbox[%d],%f,%f)\n", scenename.GetStr(), editbox, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\tSetEditBoxSize(%s_editbox[%d],%f,%f)\n", scenename.GetStr(), editbox, (m_pSearchObj->m_Size.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Size.y / (float)iBaseHeight)*100.0f);
					AddEditorLine(m_editor, tmp);

				}
				else {
					sprintf(tmp, "\tSetEditBoxPosition(%s_editbox[%d],%f,%f)\n", scenename.GetStr(), editbox, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\tSetEditBoxSize(%s_editbox[%d],%f,%f)\n", scenename.GetStr(), editbox, m_pSearchObj->m_Size.x, m_pSearchObj->m_Size.y);
					AddEditorLine(m_editor, tmp);
				}

				if (m_pSearchObj->m_Color.x != 255.0f || m_pSearchObj->m_Color.y != 255.0f || m_pSearchObj->m_Color.z != 255.0f || m_pSearchObj->m_Color.w != 255.0f) {

					sprintf(tmp, "\tSetEditBoxBackgroundColor(%s_editbox[%d],%d,%d,%d,%d)\n", scenename.GetStr(), (int)editbox, (int)m_pSearchObj->m_Color.x, (int)m_pSearchObj->m_Color.y, (int)m_pSearchObj->m_Color.z, (int)m_pSearchObj->m_Color.w);
					AddEditorLine(m_editor, tmp);
				}
				if (m_pSearchObj->m_TextColor.x != 0.0f || m_pSearchObj->m_TextColor.y != 0.0f || m_pSearchObj->m_TextColor.z != 0.0f || m_pSearchObj->m_TextColor.w != 255.0f) {

					sprintf(tmp, "\tSetEditBoxTextColor(%s_editbox[%d],%d,%d,%d)\n", scenename.GetStr(), (int)editbox, (int)m_pSearchObj->m_TextColor.x, (int)m_pSearchObj->m_TextColor.y, (int)m_pSearchObj->m_TextColor.z);
					AddEditorLine(m_editor, tmp);
				}
				if (m_pSearchObj->m_BorderColor.x != 0.0f || m_pSearchObj->m_BorderColor.y != 0.0f || m_pSearchObj->m_BorderColor.z != 0.0f || m_pSearchObj->m_BorderColor.w != 255.0f) {
					sprintf(tmp, "\tSetEditBoxBorderColor(%s_editbox[%d],%d,%d,%d,%d)\n", scenename.GetStr(), (int)editbox, (int)m_pSearchObj->m_BorderColor.x, (int)m_pSearchObj->m_BorderColor.y, (int)m_pSearchObj->m_BorderColor.z, (int)m_pSearchObj->m_BorderColor.w);
					AddEditorLine(m_editor, tmp);
				}

//				if (m_pSearchObj->m_CursorColor.x != 0.0f || m_pSearchObj->m_CursorColor.y != 0.0f || m_pSearchObj->m_CursorColor.z != 0.0f ) {
					sprintf(tmp, "\tSetEditBoxCursorColor(%s_editbox[%d],%d,%d,%d)\n", scenename.GetStr(), (int)editbox, (int)m_pSearchObj->m_CursorColor.x, (int)m_pSearchObj->m_CursorColor.y, (int)m_pSearchObj->m_CursorColor.z);
					AddEditorLine(m_editor, tmp);
//				}


				if (m_pSearchObj->m_iTextSize > 0) {
					if (bUsePercentage) {
						sprintf(tmp, "\tSetEditBoxTextSize(%s_editbox[%d],%f)\n", scenename.GetStr(), (int)editbox, (float)(m_pSearchObj->m_iTextSize / (float)iBaseHeight)*100.0f);
						AddEditorLine(m_editor, tmp);
					}
					else {
						sprintf(tmp, "\tSetEditBoxTextSize(%s_editbox[%d],%d)\n", scenename.GetStr(), (int)editbox, m_pSearchObj->m_iTextSize);
						AddEditorLine(m_editor, tmp);
					}
				}
				else {
					sprintf(tmp, "\tSetEditBoxTextSize(%s_editbox[%d],%f)\n", scenename.GetStr(), editbox, m_pSearchObj->m_Size.y);
					AddEditorLine(m_editor, tmp);
				}


				if (bUsePercentage) {
					sprintf(tmp, "\tSetEditBoxBorderSize(%s_editbox[%d],%f)\n", scenename.GetStr(), (int)editbox, (float) (m_pSearchObj->m_iBorderSize / (float)iBaseWidth)*100.0f);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetEditBoxBorderSize(%s_editbox[%d],%d)\n", scenename.GetStr(), (int)editbox, m_pSearchObj->m_iBorderSize);
					AddEditorLine(m_editor, tmp);
				}
				if (bUsePercentage) {
					sprintf(tmp, "\tSetEditBoxCursorWidth(%s_editbox[%d],%f)\n", scenename.GetStr(), (int)editbox, (float) (m_pSearchObj->m_iCursorSize / (float)iBaseWidth)*100.0f);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetEditBoxCursorWidth(%s_editbox[%d],%d)\n", scenename.GetStr(), (int)editbox, m_pSearchObj->m_iCursorSize);
					AddEditorLine(m_editor, tmp);
				}

				if (m_pSearchObj->m_iEditMaxChars > 0) {
					sprintf(tmp, "\tSetEditBoxMaxChars(%s_editbox[%d],%d)\n", scenename.GetStr(), (int)editbox, m_pSearchObj->m_iEditMaxChars);
					AddEditorLine(m_editor, tmp);
				}

				if (m_pSearchObj->bEditMultiLines) {
					sprintf(tmp, "\tSetEditBoxMultiLine(%s_editbox[%d],1)\n", scenename.GetStr(), (int)editbox);
					AddEditorLine(m_editor, tmp);
				}

				if (m_pSearchObj->bEditPassword) {
					sprintf(tmp, "\tSetEditBoxPasswordMode(%s_editbox[%d],1)\n", scenename.GetStr(), (int)editbox);
					AddEditorLine(m_editor, tmp);
				}
				if (m_pSearchObj->bEditWrap) {
					sprintf(tmp, "\tSetEditBoxWrapMode(%s_editbox[%d],1)\n", scenename.GetStr(), (int)editbox);
					AddEditorLine(m_editor, tmp);
				}

				sprintf(tmp, "\tSetEditBoxDepth(%s_editbox[%d],%d)\n", scenename.GetStr(), (int)editbox, m_pSearchObj->m_Depth);
				AddEditorLine(m_editor, tmp);

				if (m_pSearchObj->m_bVisible) {
					sprintf(tmp, "\tSetEditBoxVisible(%s_editbox[%d],1)\n", scenename.GetStr(), (int)editbox);
					AddEditorLine(m_editor, tmp);
				}
				else {
					sprintf(tmp, "\tSetEditBoxVisible(%s_editbox[%d],0)\n", scenename.GetStr(), (int)editbox);
					AddEditorLine(m_editor, tmp);
				}

				editbox++;
			}


			if (!m_pSearchObj->m_bRemoved && m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE ) {

				sprintf(tmp, "\t%s_sprites[%d] = CreateSprite( %s_images[%d] )\n", scenename.GetStr(), sprites, scenename.GetStr(), m_pSearchObj->m_iImageIndex);
				AddEditorLine(m_editor, tmp);

				uString clean = m_pSearchObj->m_sVariable;
				if (clean.GetLength() > scenename.GetLength()) {
					sprintf(tmp, "\t%s = %s_sprites[%d]\n", m_pSearchObj->m_sVariable.GetStr(), scenename.GetStr(), sprites);
					AddEditorLine(m_editor, tmp);
				}
				
				clean = m_pSearchObj->m_sArrayGroup;
				if (m_pSearchObj->m_sArrayGroup.GetLength() > 0) {
					if (clean.GetLength() > scenename.GetLength()) {

						sprintf(tmp, "\t%s.insert( %s_sprites[%d] )\n", m_pSearchObj->m_sArrayGroup.GetStr(), scenename.GetStr(), sprites);
						AddEditorLine(m_editor, tmp);

					}
				}

				//SetScissor(0, 0, 0, 0)

				if (m_pSearchObj->bStretchToDevice) {

					sprintf(tmp, "\tSetSpritePosition( %s_sprites[%d], GetScreenBoundsLeft(), GetScreenBoundsTop() )\n", scenename.GetStr(), sprites);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\tSetSpriteSize( %s_sprites[%d], GetScreenBoundsRight() - GetScreenBoundsLeft(), GetScreenBoundsBottom() - GetScreenBoundsTop() )\n", scenename.GetStr(), sprites );
					AddEditorLine(m_editor, tmp);

					//sprintf(tmp, "\tSetSpritePosition(%s_sprites[%d],(GetDeviceWidth() - GetVirtualWidth())*0.5,(GetDeviceHeight() - GetVirtualHeight())*0.5)\n", scenename.GetStr(), sprites);
					//AddEditorLine(m_editor, tmp);

					//sprintf(tmp, "\tSetSpriteSize(%s_sprites[%d],GetVirtualWidth(),GetVirtualHeight())\n", scenename.GetStr(), sprites );
					//AddEditorLine(m_editor, tmp);
				}
				else {

					if( bUsePercentage) {
						sprintf(tmp, "\tSetSpritePosition(%s_sprites[%d],%f,%f)\n", scenename.GetStr(), sprites, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
						AddEditorLine(m_editor, tmp);
						sprintf(tmp, "\tSetSpriteSize(%s_sprites[%d],%f,%f)\n", scenename.GetStr(), sprites, (m_pSearchObj->m_Size.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Size.y / (float)iBaseHeight)*100.0f);
						AddEditorLine(m_editor, tmp);
					}
					else {
						sprintf(tmp, "\tSetSpritePosition(%s_sprites[%d],%f,%f)\n", scenename.GetStr(), sprites, m_pSearchObj->m_Position.x, m_pSearchObj->m_Position.y);
						AddEditorLine(m_editor, tmp);

						sprintf(tmp, "\tSetSpriteSize(%s_sprites[%d],%f,%f)\n", scenename.GetStr(), sprites, m_pSearchObj->m_Size.x, m_pSearchObj->m_Size.y);
						AddEditorLine(m_editor, tmp);
					}
					if (m_pSearchObj->m_Scale.x != 1.0f || m_pSearchObj->m_Scale.y != 1.0f) {
						sprintf(tmp, "\tSetSpriteScale(%s_sprites[%d],%f,%f)\n", scenename.GetStr(), sprites, m_pSearchObj->m_Scale.x, m_pSearchObj->m_Scale.y);
						AddEditorLine(m_editor, tmp);
					}

					if (m_pSearchObj->m_Rotation.x != 0.0f) {
						sprintf(tmp, "\tSetSpriteAngle(%s_sprites[%d],%f)\n", scenename.GetStr(), sprites, m_pSearchObj->m_Rotation.x);
						AddEditorLine(m_editor, tmp);
					}
				}

				if (m_pSearchObj->m_Color.x != 255.0f || m_pSearchObj->m_Color.y != 255.0f || m_pSearchObj->m_Color.z != 255.0f || m_pSearchObj->m_Color.w != 255.0f) {
					sprintf(tmp, "\tSetSpriteColor(%s_sprites[%d],%d,%d,%d,%d)\n", scenename.GetStr(), (int)sprites, (int)m_pSearchObj->m_Color.x, (int)m_pSearchObj->m_Color.y, (int)m_pSearchObj->m_Color.z, (int)m_pSearchObj->m_Color.w);
					AddEditorLine(m_editor, tmp);
				}

				sprintf(tmp, "\tSetSpriteDepth(%s_sprites[%d],%d)\n", scenename.GetStr(), (int)sprites, m_pSearchObj->m_Depth);
				AddEditorLine(m_editor, tmp);


				if (m_pSearchObj->iAnimationFrom > 0 && m_pSearchObj->m_pUsedImage) {
					//Setup as animation.
					int fw = (int)m_pSearchObj->m_pUsedImage->m_iWidth / m_pSearchObj->m_pUsedImage->m_iRows;
					int fh = (int)m_pSearchObj->m_pUsedImage->m_iHeight / m_pSearchObj->m_pUsedImage->m_iColumns;

					sprintf(tmp, "\tSetSpriteAnimation(%s_sprites[%d],%d,%d,%d)\n", scenename.GetStr(), (int)sprites, (int)fw,(int)fh, (int) m_pSearchObj->m_pUsedImage->m_iRows*m_pSearchObj->m_pUsedImage->m_iColumns );
					AddEditorLine(m_editor, tmp);

					sprintf(tmp, "\tSetSpriteFrame(%s_sprites[%d],%d)\n", scenename.GetStr(), (int)sprites, m_pSearchObj->iAnimationFrom);
					AddEditorLine(m_editor, tmp);

					sprintf(tmp, "\tSetSpriteUVBorder(%s_sprites[%d], 0.5)\n", scenename.GetStr(), (int)sprites);
					AddEditorLine(m_editor, tmp);

					//sprintf(tmp, "\tSetSpriteSnap(%s_sprites[%d], 1)\n", scenename.GetStr(), (int)sprites);
					//AddEditorLine(m_editor, tmp);
				}
				if (m_pSearchObj->bUVOffset && !(m_pSearchObj->fUVSpeedX == 0.0f && m_pSearchObj->fUVSpeedY == 0.0f ) )
				{
					sprintf(tmp, "\tSetImageWrapU(%s_sprites[%d], 1 )\n", scenename.GetStr(), (int)sprites);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\tSetImageWrapV(%s_sprites[%d], 1 )\n", scenename.GetStr(), (int)sprites);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\tSetSpriteUVBorder(%s_sprites[%d], 0.5)\n", scenename.GetStr(), (int)sprites);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\t%s_sprites_count_x[%d] = 0.0\n", scenename.GetStr(), sprites);
					AddEditorLine(m_editor, tmp);
					sprintf(tmp, "\t%s_sprites_count_y[%d] = 0.0\n", scenename.GetStr(), sprites);
					AddEditorLine(m_editor, tmp);
				}


				if (m_pSearchObj->bEnablePhysics) {

					sprintf(tmp, "\tSetSpriteShape(%s_sprites[%d],%d)\n", scenename.GetStr(), (int)sprites, m_pSearchObj->iPhysicsShape);
					AddEditorLine(m_editor, tmp);

					sprintf(tmp, "\tSetSpritePhysicsOn(%s_sprites[%d],%d)\n", scenename.GetStr(), (int)sprites, m_pSearchObj->iPhysicsMode+1);
					AddEditorLine(m_editor, tmp);

					if (m_pSearchObj->bPhysicsCanRotate) {
						sprintf(tmp, "\tSetSpritePhysicsCanRotate(%s_sprites[%d],1)\n", scenename.GetStr(), (int)sprites);
						AddEditorLine(m_editor, tmp);
					}
					else {
						sprintf(tmp, "\tSetSpritePhysicsCanRotate(%s_sprites[%d],0)\n", scenename.GetStr(), (int)sprites);
						AddEditorLine(m_editor, tmp);
					}

					if (m_pSearchObj->bPhysicsIsBullet) {
						sprintf(tmp, "\tSetSpritePhysicsIsBullet(%s_sprites[%d],1)\n", scenename.GetStr(), (int)sprites);
						AddEditorLine(m_editor, tmp);
					}
					else {
						sprintf(tmp, "\tSetSpritePhysicsIsBullet(%s_sprites[%d],0)\n", scenename.GetStr(), (int)sprites);
						AddEditorLine(m_editor, tmp);
					}

					if (m_pSearchObj->fPhysicsRestitution != 0.0f) {
						sprintf(tmp, "\tSetSpritePhysicsRestitution(%s_sprites[%d],%f)\n", scenename.GetStr(), (int)sprites, (float)m_pSearchObj->fPhysicsRestitution);
						AddEditorLine(m_editor, tmp);
					}

					if (m_pSearchObj->fPhysicsCOMX != 0.0f || m_pSearchObj->fPhysicsCOMY != 0.0f) {

						if (bUsePercentage) {
							sprintf(tmp, "\tSetSpritePhysicsCOM(%s_sprites[%d],%f,%f)\n", scenename.GetStr(), (int)sprites, (float) (m_pSearchObj->fPhysicsCOMX / (float)iBaseWidth)*100.0f, (float) (m_pSearchObj->fPhysicsCOMY / (float)iBaseHeight)*100.0f);
							AddEditorLine(m_editor, tmp);
						}
						else {
							sprintf(tmp, "\tSetSpritePhysicsCOM(%s_sprites[%d],%f,%f)\n", scenename.GetStr(), (int)sprites, (float)m_pSearchObj->fPhysicsCOMX, (float)m_pSearchObj->fPhysicsCOMY);
							AddEditorLine(m_editor, tmp);
						}
					}

					if (m_pSearchObj->fPhysicsMass != 100.0f) {
						if (bUsePercentage) {
							sprintf(tmp, "\tSetSpritePhysicsMass(%s_sprites[%d],%f)\n", scenename.GetStr(), (int)sprites, (float) (m_pSearchObj->fPhysicsMass / (float)iBaseWidth)*100.0f);
							AddEditorLine(m_editor, tmp);
						}
						else {
							sprintf(tmp, "\tSetSpritePhysicsMass(%s_sprites[%d],%f)\n", scenename.GetStr(), (int)sprites, (float)m_pSearchObj->fPhysicsMass);
							AddEditorLine(m_editor, tmp);
						}
					}
					if (m_pSearchObj->fPhysicsFriction != 0.5f ) {
						sprintf(tmp, "\tSetSpritePhysicsFriction(%s_sprites[%d],%f)\n", scenename.GetStr(), (int)sprites, (float)m_pSearchObj->fPhysicsFriction);
						AddEditorLine(m_editor, tmp);
					}
					if (m_pSearchObj->fPhysicsLinearDamping != 0.0f) {
						sprintf(tmp, "\tSetSpritePhysicsDamping(%s_sprites[%d],%f)\n", scenename.GetStr(), (int)sprites, (float)m_pSearchObj->fPhysicsLinearDamping);
						AddEditorLine(m_editor, tmp);
					}
					if (m_pSearchObj->fPhysicsAngularDamping != 0.0f) {
						sprintf(tmp, "\tSetSpritePhysicsAngularDamping(%s_sprites[%d],%f)\n", scenename.GetStr(), (int)sprites, (float)m_pSearchObj->fPhysicsAngularDamping);
						AddEditorLine(m_editor, tmp);
					}

				}


				if (!m_pSearchObj->m_bVisible) {
					sprintf(tmp, "\tSetSpriteVisible(%s_sprites[%d],0)\n", scenename.GetStr(), (int)sprites);
					AddEditorLine(m_editor, tmp);
				}
				sprites++;
			}
			m_pSearchObj = m_pSearchObj->m_pNext;
		}
	}



	AddEditorLine(m_editor, "endfunction\n\n");
}

void remap_sprite_index(MediaScene * m_pSearchScene)
{
	if (!m_pSearchScene)
		return;
	int new_index = 1;
	//Remap image spirte index id's
	MediaScene::Images * m_pSearchImage;

	//Add 100000 to spriteindex. so we dont overwrite it later.
	m_pSearchImage = m_pSearchScene->m_pFirstImage;
	if (m_pSearchImage) {
		//search images.
		while (m_pSearchImage) {
			bool used = false;
			//only save if used.
			MediaScene::MediaObject * m_pSearchObj = NULL;
			MediaScene::MediaObject * m_pLastObj = NULL;
			m_pSearchObj = m_pSearchScene->m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if( m_pSearchObj->m_iImageIndex == m_pSearchImage->m_iImageIndex )
						m_pSearchObj->m_iImageIndex += 100000;
					if (m_pSearchObj->m_iImageDownIndex == m_pSearchImage->m_iImageIndex)
						m_pSearchObj->m_iImageDownIndex += 100000;
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}
			m_pSearchImage = m_pSearchImage->m_pNext;
		}
	}


	m_pSearchImage = m_pSearchScene->m_pFirstImage;
	if (m_pSearchImage) {
		//search images.
		while (m_pSearchImage) {
			bool used = false;
			//only save if used.
			MediaScene::MediaObject * m_pSearchObj = NULL;
			MediaScene::MediaObject * m_pLastObj = NULL;
			m_pSearchObj = m_pSearchScene->m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if (m_pSearchObj->m_iImageIndex == (m_pSearchImage->m_iImageIndex+ 100000)) {
						m_pSearchObj->m_iImageIndex = new_index;
					}
					if (m_pSearchObj->m_iImageDownIndex == (m_pSearchImage->m_iImageIndex + 100000)) {
						m_pSearchObj->m_iImageDownIndex = new_index;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}
			m_pSearchImage->m_iImageIndex = new_index;
			new_index++;
			m_pSearchImage = m_pSearchImage->m_pNext;
		}
	}

	m_pSearchScene->iImageCount = new_index;
}

void MediaScene::GenerateCode(MediaScene * m_pMainScene, TextEditor * m_editor, bool save)
{
	if (!m_pMainScene)
		return;
	if (!m_editor)
		return;

	m_editor->SetReadOnly(false);
	m_editor->SetText("//Scene file do not make any changes here.\n");


	//Setup Globals.

	uString fpath = m_pMainScene->m_sSceneFullPath;
	uString newname = m_pMainScene->m_sSceneName;


	//30-04-2019: Why did i do this, need test after remove.
	//This do you cant rename a scene ?. added "newname.GetLength() <= 0"
	char tmp[MAX_PATH];
	if (newname.GetLength() <= 0) {
		fpath.ReplaceStr("\\", "/");
		int pos = fpath.RevFindStr("/");
		if (pos > 0) {
			fpath.SubString(newname, pos + 1);
		}
		newname.ReplaceStr(".scene", "");
	}
	newname.Trim(" ");
	newname.ReplaceStr(" ", "_");
	newname.ReplaceStr(",", "");
	newname.ReplaceStr("#", "");
	newname.ReplaceStr("!", "");



	sprintf(tmp, "global %s_base_width = %d\n", newname.GetStr(), m_pMainScene->iBaseWidth);
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "global %s_base_height = %d\n", newname.GetStr(), m_pMainScene->iBaseHeight);
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "global %s_device_width = %d\n", newname.GetStr(), m_pMainScene->iScreenWidth);
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "global %s_device_height = %d\n", newname.GetStr(), m_pMainScene->iScreenHeight);
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "global %s_fadesprite, %s_fadeimage as integer\n", newname.GetStr(), newname.GetStr());
	AddEditorLine(m_editor, tmp);
	sprintf(tmp, "global %s_tween# as float\n",newname.GetStr());
	AddEditorLine(m_editor, tmp);



	MediaScene * m_pSearchScene;
	m_pSearchScene = m_pMainScene;
	while (m_pSearchScene) {
		//Remap index.
		remap_sprite_index(m_pSearchScene);

		uString Entry = m_pSearchScene->m_sSceneName;
		if (!m_pSearchScene->bDisabled && Entry.GetLength() > 0) {
			AddSceneGlobals(m_pSearchScene, m_editor);
		}
		m_pSearchScene = m_pSearchScene->m_pNextScene;
	}


	//Real code on top.
	//AddEditorLine(m_editor, "\n//Code goes here.\n");

	m_pSearchScene;
	m_pSearchScene = m_pMainScene;
	while (m_pSearchScene) {
		uString Entry = m_pSearchScene->m_sSceneName;
		if (!m_pSearchScene->bDisabled && Entry.GetLength() > 0) {
			AddSceneSetupFunction(m_pSearchScene, m_editor, m_pMainScene);
			AddSceneSyncFunction(m_pSearchScene, m_editor, m_pMainScene);
			//AddSceneDialogFunction(m_pSearchScene, m_editor);
			AddSceneCleanup(m_pSearchScene, m_editor, m_pMainScene);
			AddSceneFade(m_pSearchScene, m_editor, m_pMainScene);
		}
		m_pSearchScene = m_pSearchScene->m_pNextScene;
	}
	//Settings as the last.

	AddEditorLine(m_editor, "\nREMSTART\n");
	AddEditorLine(m_editor, "##################################################\n");
	AddEditorLine(m_editor, "#### Settings: Do not make any changes below. ####\n");
	AddEditorLine(m_editor, "##################################################\n");
	AddEditorLine(m_editor, "\n");

	for (std::map<std::string, Resolutions>::iterator it = m_pMainScene->Resolution.begin(); it != m_pMainScene->Resolution.end(); ++it)
	{
		uString Entry = it->second.m_sName;
		if (Entry.CompareTo("Custom") == 0) {
			char tmp[MAX_PATH];
			sprintf(tmp, "resolution: %d,%d\n", it->second.m_iWidth, it->second.m_iHeight);
			AddEditorLine(m_editor, tmp);
		}
	}

	m_pSearchScene = m_pMainScene;
	while (m_pSearchScene) {
		uString Entry = m_pSearchScene->m_sSceneName;

		if (!m_pSearchScene->bDisabled && Entry.GetLength() > 0) {

			char tmp[MAX_PATH];
			sprintf(tmp, "scenename: %s\n", m_pSearchScene->m_sSceneName.GetStr());
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "gridx: %f\n", m_pSearchScene->fGridX);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "gridy: %f\n", m_pSearchScene->fGridY);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "gridon: %d\n", (int) m_pSearchScene->bGridOn);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "sceneresolutiontext: %s\n", m_pSearchScene->cResolutionMenuText);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "screenwidth: %d\n", m_pSearchScene->iScreenWidth);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "screenheight: %d\n", m_pSearchScene->iScreenHeight);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenebackr: %d\n", m_pSearchScene->iSceneBackColor[0]);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenebackg: %d\n", m_pSearchScene->iSceneBackColor[1]);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenebackb: %d\n", m_pSearchScene->iSceneBackColor[2]);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenebacka: %d\n", m_pSearchScene->iSceneBackColor[3]);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "followdeviceres: %d\n", m_pSearchScene->bFollowDeviceResolution);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "basewidth: %d\n", m_pSearchScene->iBaseWidth);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "baseheight: %d\n", m_pSearchScene->iBaseHeight);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenegravityx: %f\n", m_pSearchScene->fPhysicsGravityX);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenegravityy: %f\n", m_pSearchScene->fPhysicsGravityY);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenewalltop: %d\n", m_pSearchScene->bPhysicsWallTop);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenewallbottom: %d\n", m_pSearchScene->bPhysicsWallBottom);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenewallleft: %d\n", m_pSearchScene->bPhysicsWallLeft);
			AddEditorLine(m_editor, tmp);
			sprintf(tmp, "scenewallright: %d\n", m_pSearchScene->bPhysicsWallRight);
			AddEditorLine(m_editor, tmp);

			sprintf(tmp, "scenemaxpolygon: %d\n", m_pSearchScene->iPhysicsMaxPolygonPoints);
			AddEditorLine(m_editor, tmp);

			sprintf(tmp, "scenepercentage: %d\n", m_pSearchScene->bUsePercentage);
			AddEditorLine(m_editor, tmp);


			//Add all scene images.
			MediaScene::Images * m_pSearchImage;
			m_pSearchImage = m_pSearchScene->m_pFirstImage;
			if (m_pSearchImage) {
				//search images.
				while (m_pSearchImage) {
					bool used = false;
					//only save if used.
					MediaScene::MediaObject * m_pSearchObj = NULL;
					MediaScene::MediaObject * m_pLastObj = NULL;
					m_pSearchObj = m_pSearchScene->m_pFirstObject;
					if (m_pSearchObj) {
						while (m_pSearchObj) {
							if (!m_pSearchObj->m_bRemoved) {
								if (m_pSearchObj->m_iImageIndex == m_pSearchImage->m_iImageIndex || m_pSearchObj->m_iImageDownIndex == m_pSearchImage->m_iImageIndex) {
									used = true;
									break;
								}
							}
							m_pSearchObj = m_pSearchObj->m_pNext;
						}
					}
					if (used) {
						sprintf(tmp, "imageindex: %d\n", m_pSearchImage->m_iImageIndex);
						AddEditorLine(m_editor, tmp);
						sprintf(tmp, "addimage: %s\n", m_pSearchImage->m_sFullPath.GetStr());
						AddEditorLine(m_editor, tmp);
						sprintf(tmp, "imageatlas: %d\n", m_pSearchImage->m_bAtlas);
						AddEditorLine(m_editor, tmp);
						sprintf(tmp, "imagerows: %d\n", m_pSearchImage->m_iRows);
						AddEditorLine(m_editor, tmp);
						sprintf(tmp, "imagecolumns: %d\n", m_pSearchImage->m_iColumns);
						AddEditorLine(m_editor, tmp);
					}
					m_pSearchImage = m_pSearchImage->m_pNext;
				}
			}

			//Add all scene fonts
			if (m_pSearchScene->Fonts.size() > 0) {
				for (std::map<std::int32_t, std::string>::iterator it = m_pSearchScene->Fonts.begin(); it != m_pSearchScene->Fonts.end(); ++it)
				{

					bool fontinuse = false;

					//CHECK if this font is in use HERE!
					//Add all scene sprites.
					MediaScene::MediaObject * m_pSearchObj = NULL;
					m_pSearchObj = m_pSearchScene->m_pFirstObject;
					if (m_pSearchObj) {
						while (m_pSearchObj) {
							if (!m_pSearchObj->m_bRemoved) {

								if (m_pSearchObj->m_iFontId == it->first) {
									//In use.
									fontinuse = true;
									break;
								}
							}

							m_pSearchObj = m_pSearchObj->m_pNext;
						}
					}

					if (fontinuse) {
						uString FontPath = it->second.c_str();
						FontPath.ReplaceStr("\\", "/");
						sprintf(tmp, "fontindex: %d\n", it->first);
						AddEditorLine(m_editor, tmp);
						sprintf(tmp, "addfont: %s\n", FontPath.GetStr());
						AddEditorLine(m_editor, tmp);
					}
				}
			}

			//Add all scene sprites.
			MediaScene::MediaObject * m_pSearchObj = NULL;
			m_pSearchObj = m_pSearchScene->m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if (!m_pSearchObj->m_bRemoved) {
						sprintf(tmp, "spriteindex: %d\n", m_pSearchObj->m_iImageIndex);
						AddEditorLine(m_editor, tmp);
						if (m_pSearchObj->m_iImageDownIndex > 0 && m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON ) {
							sprintf(tmp, "spriteindexdown: %d\n", m_pSearchObj->m_iImageDownIndex);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {
							sprintf(tmp, "addsprite: %s\n", m_pSearchObj->m_sName.GetStr());
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT) {
							sprintf(tmp, "addtext: %s\n", m_pSearchObj->m_sName.GetStr());
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {
							sprintf(tmp, "addbutton: %s\n", m_pSearchObj->m_sName.GetStr());
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
							sprintf(tmp, "addeditbox: %s\n", m_pSearchObj->m_sName.GetStr());
							AddEditorLine(m_editor, tmp);
						}

						sprintf(tmp, "mediatype: %d\n", m_pSearchObj->m_iMediaType);
						AddEditorLine(m_editor, tmp);

						sprintf(tmp, "mediavisible: %d\n", m_pSearchObj->m_bVisible);
						AddEditorLine(m_editor, tmp);
					

						//m_iMediaType
						if (m_pSearchObj->m_sVariable.GetLength() > 0) {
							sprintf(tmp, "spritevariable: %s\n", m_pSearchObj->m_sVariable.GetStr());
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_sArrayGroup.GetLength() > 0) {
							sprintf(tmp, "spritegroup: %s\n", m_pSearchObj->m_sArrayGroup.GetStr());
							AddEditorLine(m_editor, tmp);
						}
						//m_pData = 0x1222b580 ""
						if (m_pSearchObj->m_Text.GetLength() > 0) {
							sprintf(tmp, "mediatext: %s\n", m_pSearchObj->m_Text.GetStr());
							AddEditorLine(m_editor, tmp);
						}
						else {
							strcpy(tmp, "mediatext: \n");
							AddEditorLine(m_editor, tmp);
						}
						sprintf(tmp, "spriteposx: %f\n", m_pSearchObj->m_Position.x);
						AddEditorLine(m_editor, tmp);
						sprintf(tmp, "spriteposy: %f\n", m_pSearchObj->m_Position.y);
						AddEditorLine(m_editor, tmp);
						if (m_pSearchObj->m_Scale.x != 1.0f) {
							sprintf(tmp, "spritescalex: %f\n", m_pSearchObj->m_Scale.x);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_Scale.y != 1.0f) {
							sprintf(tmp, "spritescaley: %f\n", m_pSearchObj->m_Scale.y);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_Rotation.x != 0.0f) {
							sprintf(tmp, "spriteanglex: %f\n", m_pSearchObj->m_Rotation.x);
							AddEditorLine(m_editor, tmp);
						}
						sprintf(tmp, "spritesizex: %f\n", m_pSearchObj->m_Size.x);
						AddEditorLine(m_editor, tmp);
						sprintf(tmp, "spritesizey: %f\n", m_pSearchObj->m_Size.y);
						AddEditorLine(m_editor, tmp);
						if (m_pSearchObj->m_Color.x != 255.0f || m_pSearchObj->m_Color.y != 255.0f || m_pSearchObj->m_Color.z != 255.0f || m_pSearchObj->m_Color.w != 255.0f) {
							sprintf(tmp, "spritecolorx: %f\n", m_pSearchObj->m_Color.x);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "spritecolory: %f\n", m_pSearchObj->m_Color.y);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "spritecolorz: %f\n", m_pSearchObj->m_Color.z);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "spritecolorw: %f\n", m_pSearchObj->m_Color.w);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
							if (m_pSearchObj->m_TextColor.x != 0.0f || m_pSearchObj->m_TextColor.y != 0.0f || m_pSearchObj->m_TextColor.z != 0.0f || m_pSearchObj->m_TextColor.w != 255.0f) {
								sprintf(tmp, "edittextx: %f\n", m_pSearchObj->m_TextColor.x);
								AddEditorLine(m_editor, tmp);
								sprintf(tmp, "edittexty: %f\n", m_pSearchObj->m_TextColor.y);
								AddEditorLine(m_editor, tmp);
								sprintf(tmp, "edittextz: %f\n", m_pSearchObj->m_TextColor.z);
								AddEditorLine(m_editor, tmp);
								sprintf(tmp, "edittextw: %f\n", m_pSearchObj->m_TextColor.w);
								AddEditorLine(m_editor, tmp);
							}
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
							if (m_pSearchObj->m_BorderColor.x != 0.0f || m_pSearchObj->m_BorderColor.y != 0.0f || m_pSearchObj->m_BorderColor.z != 0.0f || m_pSearchObj->m_BorderColor.w != 255.0f) {
								sprintf(tmp, "editborderx: %f\n", m_pSearchObj->m_BorderColor.x);
								AddEditorLine(m_editor, tmp);
								sprintf(tmp, "editbordery: %f\n", m_pSearchObj->m_BorderColor.y);
								AddEditorLine(m_editor, tmp);
								sprintf(tmp, "editborderz: %f\n", m_pSearchObj->m_BorderColor.z);
								AddEditorLine(m_editor, tmp);
								sprintf(tmp, "editborderw: %f\n", m_pSearchObj->m_BorderColor.w);
								AddEditorLine(m_editor, tmp);
							}
							if (m_pSearchObj->m_CursorColor.x != 0.0f || m_pSearchObj->m_CursorColor.y != 0.0f || m_pSearchObj->m_CursorColor.z != 0.0f ) {
								sprintf(tmp, "editcursorx: %f\n", m_pSearchObj->m_BorderColor.x);
								AddEditorLine(m_editor, tmp);
								sprintf(tmp, "editcursory: %f\n", m_pSearchObj->m_BorderColor.y);
								AddEditorLine(m_editor, tmp);
								sprintf(tmp, "editcursorz: %f\n", m_pSearchObj->m_BorderColor.z);
								AddEditorLine(m_editor, tmp);
							}
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
							sprintf(tmp, "editbordersize: %d\n", m_pSearchObj->m_iBorderSize);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
							sprintf(tmp, "editcursorsize: %d\n", m_pSearchObj->m_iCursorSize);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX && m_pSearchObj->m_iTextSize > 0 ) {
							sprintf(tmp, "edittextsize: %d\n", m_pSearchObj->m_iTextSize);
							AddEditorLine(m_editor, tmp);
						}
						
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX && m_pSearchObj->m_iEditMaxChars > 0 ) {
							sprintf(tmp, "editmaxchars: %d\n", m_pSearchObj->m_iEditMaxChars);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX && m_pSearchObj->bEditMultiLines ) {
							sprintf(tmp, "editmultiline: %d\n", m_pSearchObj->bEditMultiLines);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX && m_pSearchObj->bEditPassword) {
							sprintf(tmp, "editpassword: %d\n", m_pSearchObj->bEditPassword);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX && m_pSearchObj->bEditWrap) {
							sprintf(tmp, "editwrap: %d\n", m_pSearchObj->bEditWrap);
							AddEditorLine(m_editor, tmp);
						}

						
						sprintf(tmp, "spritedepth: %d\n", m_pSearchObj->m_Depth);
						AddEditorLine(m_editor, tmp);
						if (m_pSearchObj->bLocked) {
							sprintf(tmp, "spritelocked: %d\n", m_pSearchObj->bLocked);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->bStretchToDevice) {
							sprintf(tmp, "spritestretch: %d\n", m_pSearchObj->bStretchToDevice);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->bFixToScreen) {
							sprintf(tmp, "spritefixtoscreen: %d\n", m_pSearchObj->bFixToScreen);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->bUVOffset) {
							sprintf(tmp, "spriteuvoffset: %d\n", m_pSearchObj->bUVOffset);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "spriteuvspeedx: %f\n", m_pSearchObj->fUVSpeedX);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "spriteuvspeedy: %f\n", m_pSearchObj->fUVSpeedY);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->bOnlyVisibleInAGK) {
							sprintf(tmp, "spriteagkvisible: %d\n", m_pSearchObj->bOnlyVisibleInAGK);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->bAnimated) {
							sprintf(tmp, "spriteanimated: %d\n", m_pSearchObj->bAnimated);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->iAnimationFrom > 0) {
							sprintf(tmp, "spriteanimfrom: %d\n", m_pSearchObj->iAnimationFrom);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->iAnimationTo > 0) {
							sprintf(tmp, "spriteanimto: %d\n", m_pSearchObj->iAnimationTo);
							AddEditorLine(m_editor, tmp);
						}
						if (m_pSearchObj->bEnablePhysics ) {
							sprintf(tmp, "physicsenable: %d\n", m_pSearchObj->bEnablePhysics);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "physicsmode: %d\n", m_pSearchObj->iPhysicsMode);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "physicsshape: %d\n", m_pSearchObj->iPhysicsShape);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "physicsrotate: %d\n", m_pSearchObj->bPhysicsCanRotate);
							AddEditorLine(m_editor, tmp);
							sprintf(tmp, "physicsbullet: %d\n", m_pSearchObj->bPhysicsIsBullet);
							AddEditorLine(m_editor, tmp);
							if (m_pSearchObj->fPhysicsRestitution != 0.0f) {
								sprintf(tmp, "physicsrestitution: %f\n", m_pSearchObj->fPhysicsRestitution);
								AddEditorLine(m_editor, tmp);
							}
							if (m_pSearchObj->fPhysicsCOMX != 0.0f) {
								sprintf(tmp, "physicscomx: %f\n", m_pSearchObj->fPhysicsCOMX);
								AddEditorLine(m_editor, tmp);
							}
							if (m_pSearchObj->fPhysicsCOMY != 0.0f) {
								sprintf(tmp, "physicscomy: %f\n", m_pSearchObj->fPhysicsCOMY);
								AddEditorLine(m_editor, tmp);
							}
							if (m_pSearchObj->fPhysicsMass != 100.0f) {
								sprintf(tmp, "physicsmass: %f\n", m_pSearchObj->fPhysicsMass);
								AddEditorLine(m_editor, tmp);
							}
							if (m_pSearchObj->fPhysicsFriction != 0.5f) {
								sprintf(tmp, "physicsfriction: %f\n", m_pSearchObj->fPhysicsFriction);
								AddEditorLine(m_editor, tmp);
							}
							if (m_pSearchObj->fPhysicsLinearDamping != 0.0f) {
								sprintf(tmp, "physicsliniardamping: %f\n", m_pSearchObj->fPhysicsLinearDamping);
								AddEditorLine(m_editor, tmp);
							}
							if (m_pSearchObj->fPhysicsAngularDamping != 0.0f) {
								sprintf(tmp, "physicsangulardamping: %f\n", m_pSearchObj->fPhysicsAngularDamping);
								AddEditorLine(m_editor, tmp);
							}

						}
						if (m_pSearchObj->m_iFontId != 0) {
							sprintf(tmp, "fontid: %d\n", m_pSearchObj->m_iFontId);
							AddEditorLine(m_editor, tmp);
						}


					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}

		}
		m_pSearchScene = m_pSearchScene->m_pNextScene;
	}
	AddEditorLine(m_editor, "\nREMEND\n");


	if (save) {
		//Save scene file.
		auto textToSave = m_editor->GetText();
		std::ofstream myfile;
		myfile.open(m_pMainScene->m_sSceneFullPath);
		myfile << textToSave;
		myfile.close();
		m_editor->filechanged = false;
	}

	TextEditor::Coordinates cpos;
	cpos.mColumn = 0;
	cpos.mLine = 0;
	m_editor->SetCursorPosition(cpos);
	m_editor->SetReadOnly(true);
}



void MediaScene::LoadCode(MediaScene * m_pMainScene, TextEditor * m_editor)
{
	std::string stdtmp;

	MediaScene * m_pChangeScene = m_pMainScene;
	if (!m_pChangeScene)
		return;

//	m_editor->filechanged = true;// Always ask for now.

	TextEditor::Coordinates cpos;
	cpos.mColumn = 0;
	cpos.mLine = 0;
	m_editor->SetCursorPosition(cpos);
	strcpy(m_editor->cnSearch, "#### Settings: Do not make any changes below. ####");
	m_editor->FindNext();

	cpos = m_editor->GetCursorPosition();
	cpos.mColumn = 0;
	cpos.mLine++;
	int CurrentLine = cpos.mLine;
	int TotalLines = m_editor->GetTotalLines();
	int nextimageid = 0;
	int nextfontid = 0;
	int nextspriteid = 0;
	int nextspriteiddown = 0;
	MediaScene::Images * m_pNextImage = NULL;
	MediaScene::Images * m_pNextImageDown = NULL;
	MediaScene::MediaObject * m_pNextObj = NULL;
	int numscenes = 0;
	for (int i = CurrentLine; i < TotalLines; i++) {
		cpos.mLine = i;
		m_editor->SetCursorPosition(cpos);
		stdtmp = m_editor->GetCurrentLineText();
		uString tmp = stdtmp.c_str();
		uString cmd, val;

		int pos = tmp.FindStr(":");
		if (pos > 0) {
			tmp.SubString(cmd, 0, pos);
			tmp.SubString(val, pos + 2); // also skip space.

			if (cmd.CompareTo("scenename") == 0) {
				//Create new scene.
				if (numscenes > 0) {
					//everything after this goes to:
					MediaScene * m_pLastScene = m_pChangeScene;
					m_pChangeScene = new MediaScene();
					m_pLastScene->m_pNextScene = m_pChangeScene;
				}

				m_pChangeScene->m_sSceneName = val.GetStr();
				numscenes++;
			}
			else if (cmd.CompareTo("gridx") == 0 && m_pChangeScene) {
				m_pChangeScene->fGridX = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("gridy") == 0 && m_pChangeScene) {
				m_pChangeScene->fGridY = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("gridon") == 0 && m_pChangeScene) {
				m_pChangeScene->bGridOn = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("resolution") == 0) {
				uString width, height;
				int pos = val.FindStr(",");
				if (pos > 0) {
					val.SubString(width, 0, pos);
					val.SubString(height, pos + 1);
					uString resname = "C ";
					resname.Append(width);
					resname.Append("x");
					resname.Append(height);
					m_pMainScene->Resolution.insert(std::make_pair(std::string(resname.GetStr()), Resolutions{ atoi(width) , atoi(height) ,true, "Custom" }));
				}
			}
			else if (cmd.CompareTo("sceneresolutiontext") == 0 && m_pChangeScene) {
				strcpy(m_pChangeScene->cResolutionMenuText, val.GetStr());
			}
			else if (cmd.CompareTo("screenwidth") == 0 && m_pChangeScene) {
				m_pChangeScene->iScreenWidth = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("screenheight") == 0 && m_pChangeScene) {
				m_pChangeScene->iScreenHeight = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("scenebackr") == 0 && m_pChangeScene) {
				m_pChangeScene->iSceneBackColor[0] = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("scenebackg") == 0 && m_pChangeScene) {
				m_pChangeScene->iSceneBackColor[1] = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("scenebackb") == 0 && m_pChangeScene) {
				m_pChangeScene->iSceneBackColor[2] = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("scenebacka") == 0 && m_pChangeScene) {
				m_pChangeScene->iSceneBackColor[3] = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("followdeviceres") == 0 && m_pChangeScene) {
				m_pChangeScene->bFollowDeviceResolution = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("basewidth") == 0 && m_pChangeScene) {
				m_pChangeScene->iBaseWidth = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("baseheight") == 0 && m_pChangeScene) {
				m_pChangeScene->iBaseHeight = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("scenegravityx") == 0 && m_pChangeScene) {
				m_pChangeScene->fPhysicsGravityX = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("scenegravityy") == 0 && m_pChangeScene) {
				m_pChangeScene->fPhysicsGravityY = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("scenewalltop") == 0 && m_pChangeScene) {
				m_pChangeScene->bPhysicsWallTop = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("scenewallbottom") == 0 && m_pChangeScene) {
				m_pChangeScene->bPhysicsWallBottom = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("scenewallleft") == 0 && m_pChangeScene) {
				m_pChangeScene->bPhysicsWallLeft = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("scenewallright") == 0 && m_pChangeScene) {
				m_pChangeScene->bPhysicsWallRight = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("scenemaxpolygon") == 0 && m_pChangeScene) {
				m_pChangeScene->iPhysicsMaxPolygonPoints = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("scenepercentage") == 0 && m_pChangeScene) {
				m_pChangeScene->bUsePercentage = atoi(val.GetStr()) != 0;
			}

			else if (cmd.CompareTo("fontindex") == 0) {
				nextfontid = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("addfont") == 0 && m_pChangeScene ) {
				//
				m_pChangeScene->Fonts.insert(std::make_pair(std::int32_t(nextfontid), std::string(val.GetStr())));
				if (nextfontid > m_pChangeScene->iFontCount)
					m_pChangeScene->iFontCount = nextfontid;
				m_pChangeScene->iFontCount++;

			}
			else if (cmd.CompareTo("imageindex") == 0) {
				nextimageid = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("addimage") == 0 && m_pChangeScene) {
				//nextimageid

				//Check if already there.
				MediaScene::Images * m_pSearchImage = NULL;
				MediaScene::Images * m_pLastImage = NULL;
				m_pSearchImage = m_pChangeScene->m_pFirstImage;
				if (m_pSearchImage) {
					while (m_pSearchImage) {
						if (m_pSearchImage->m_sFullPath.CompareCaseTo(val.GetStr()) == 0) {
							break;
						}
						m_pLastImage = m_pSearchImage;
						m_pSearchImage = m_pSearchImage->m_pNext;
					}
				}

				if (!m_pSearchImage) {
					//Not found continue add image

					uString fullfilename, relativeName, ext, relativeNameNoExt;
					fullfilename = val.GetStr();
					fullfilename.ReplaceStr("\\", "/");

					pos = pecasepos(fullfilename.GetStr(), "/media/");
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

					MediaScene::Images * Image = new MediaScene::Images();
					Image->m_sFullPath = val.GetStr();
					Image->m_sRelativePath = relativeName;
					Image->m_sName = relativeName;
					Image->m_pNext = NULL;
					Image->m_bRemoved = false;
					Image->m_bAtlas = false;
					Image->m_iRows = 1;
					Image->m_iColumns = 1;
					uString sIName;
					sIName = "raw:";
					sIName.Append(fullfilename);
					agk::SetGenerateMipmaps(0);
					if (!agk::GetFileExists(sIName.GetStr())) {
						sIName = relativeName;
						if (renderingProject) {
							uString nfp = renderingProject->m_sProjectFileFullPath;
							nfp.ReplaceStr("\\", "/");
							int pos = nfp.RevFindStr("/");
							if (pos > 0) {
								uString tmp;
								nfp.SubString(tmp, 0 , pos);
								tmp.Append("/media/");
								tmp.Append(relativeName);
								sIName = "raw:";
								sIName.Append(tmp);
							}

							if (agk::GetFileExists(sIName.GetStr())) {
								//Update to new fullpath.
								Image->m_sFullPath = sIName;
							}
						}

					}
					Image->m_iImageId = agk::LoadImage(sIName.GetStr());
					agk::SetImageWrapU(Image->m_iImageId, 1);
					agk::SetImageWrapV(Image->m_iImageId, 1);

					Image->m_iWidth = (int) agk::GetImageWidth(Image->m_iImageId);
					Image->m_iHeight = (int) agk::GetImageHeight(Image->m_iImageId);
					Image->m_iImageIndex = nextimageid;
					if (!m_pLastImage) {
						//No image found, set first entry.
						m_pChangeScene->m_pFirstImage = Image;
					}
					else {
						//Add the new image to the chain.
						m_pLastImage->m_pNext = Image;
					}

					m_pSearchImage = Image;
					m_pNextImage = Image;
					if (nextimageid > m_pChangeScene->iImageCount)
						m_pChangeScene->iImageCount = nextimageid;

					m_pChangeScene->iImageCount++;
				}
			}
			else if (cmd.CompareTo("imageatlas") == 0 && m_pNextImage ) {
				m_pNextImage->m_bAtlas = atoi(val.GetStr()) != 0;
				if (m_pNextImage->m_bAtlas) {
					//SetImageMinFilter(ImageID, 0) and SetImageMagFilter(ImageID, 0), along with SetSpriteUVBorder(SpriteID, 0)
					agk::SetImageWrapU(m_pNextImage->m_iImageId, 0);
					agk::SetImageWrapV(m_pNextImage->m_iImageId, 0);
					//agk::SetSpriteUVBorder(m_pNextImage->m_iImageId, 0);
					agk::SetImageMinFilter(m_pNextImage->m_iImageId, 0);
					agk::SetImageMagFilter(m_pNextImage->m_iImageId, 0);
				}

			}
			else if (cmd.CompareTo("imagerows") == 0 && m_pNextImage) {
				m_pNextImage->m_iRows = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("imagecolumns") == 0 && m_pNextImage) {
				m_pNextImage->m_iColumns = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("spriteindex") == 0 && m_pChangeScene) {
				nextspriteid = atoi(val.GetStr());
				//Find image.
				MediaScene::Images * m_pSearchImage = NULL;
				MediaScene::Images * m_pLastImage = NULL;
				m_pSearchImage = m_pChangeScene->m_pFirstImage;
				if (m_pSearchImage) {
					while (m_pSearchImage) {
						if (m_pSearchImage->m_iImageIndex == nextspriteid) {
							m_pNextImage = m_pSearchImage;
							break;
						}
						m_pSearchImage = m_pSearchImage->m_pNext;
					}
				}
			}
			else if (cmd.CompareTo("spriteindexdown") == 0 && m_pChangeScene) {
				nextspriteiddown = atoi(val.GetStr());
				//Find image.
				MediaScene::Images * m_pSearchImage = NULL;
				MediaScene::Images * m_pLastImage = NULL;
				m_pSearchImage = m_pChangeScene->m_pFirstImage;
				if (m_pSearchImage) {
					while (m_pSearchImage) {
						if (m_pSearchImage->m_iImageIndex == nextspriteiddown) {
							m_pNextImageDown = m_pSearchImage;
							break;
						}
						m_pSearchImage = m_pSearchImage->m_pNext;
					}
				}
			}
			else if (cmd.CompareTo("addsprite") == 0 && m_pNextImage && m_pChangeScene) {

				//Add a sprite object.
				MediaScene::MediaObject * m_pSearchObj = NULL;
				MediaScene::MediaObject * m_pLastObj = NULL;
				m_pSearchObj = m_pChangeScene->m_pFirstObject;
				if (m_pSearchObj) {
					while (m_pSearchObj) {
						m_pLastObj = m_pSearchObj;
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
				}

				//Just use defaults , so we dont have to save all info later, if something is not changed from default.
				MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
				Obj->m_bRemoved = false;
				Obj->m_bVisible = true;
				Obj->m_Color = { 255,255,255,255 };
//				if (m_pChangeScene->iSpriteCount < 3000)
//					Obj->m_Depth = 4000 - m_pChangeScene->iSpriteCount;
//				else
					Obj->m_Depth = 5000;
				Obj->m_iImageIndex = nextspriteid;
				Obj->m_iImageId = m_pNextImage->m_iImageId;
				Obj->m_iImageDownIndex;
				Obj->m_iImageDownId;
				Obj->m_iMediaType = MEDIATYPE_SPRITE;
				Obj->m_iSprite = agk::CreateSprite(m_pNextImage->m_iImageId);
				Obj->m_Layer = 1;
				Obj->bAnimated = false;
				Obj->iAnimationFrom = 0;
				Obj->iAnimationTo = 0;
				Obj->bFixToScreen = false;
				Obj->bStretchToDevice = false;
				Obj->bLocked = false;
				Obj->bOnlyVisibleInAGK = false;
				Obj->m_pNext = NULL;
				Obj->m_Position = { 0.0f,0.0f,0.0f,0.0f };
				agk::SetSpritePosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
				Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
				Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };
				Obj->m_Size = { (float)m_pNextImage->m_iWidth, (float)m_pNextImage->m_iHeight ,0.0f,0.0f };

				Obj->m_pUsedImage = m_pNextImage;
				Obj->m_pUsedDownImage = NULL;

				Obj->m_sMediaFullPath = m_pNextImage->m_sFullPath;
				Obj->m_sMediaRelativePath = m_pNextImage->m_sRelativePath;

				Obj->bEnablePhysics = false;
				Obj->iPhysicsShape = 3;
				Obj->iPhysicsMode = 0;
				Obj->bPhysicsCanRotate = true;
				Obj->bPhysicsIsBullet = false;

				Obj->fPhysicsCOMX = 0.0f;
				Obj->fPhysicsCOMY = 0.0f;
				Obj->fPhysicsMass = 100.0f;
				Obj->fPhysicsFriction = 0.5f;
				Obj->fPhysicsLinearDamping = 0.0f;
				Obj->fPhysicsAngularDamping = 0.0f;

				Obj->m_iFontId = 0;
				Obj->m_iLoadFontId = 0;


				Obj->m_sName = val;
				Obj->m_sVariable = "";
				if (!m_pLastObj) {
					//No image found, set first entry.
					m_pChangeScene->m_pFirstObject = Obj;
				}
				else {
					//Add the new image to the chain.
					m_pLastObj->m_pNext = Obj;
				}
				//Disable by default.
				agk::SetSpriteVisible(Obj->m_iSprite, 0);
				//For calls to additional settings.
				m_pNextObj = Obj;
				m_pChangeScene->iSpriteCount++;
			}
			else if (cmd.CompareTo("addtext") == 0 && m_pChangeScene) {

				//Add a text object.
				MediaScene::MediaObject * m_pSearchObj = NULL;
				MediaScene::MediaObject * m_pLastObj = NULL;
				m_pSearchObj = m_pChangeScene->m_pFirstObject;
				if (m_pSearchObj) {
					while (m_pSearchObj) {
						m_pLastObj = m_pSearchObj;
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
				}

				//Just use defaults , so we dont have to save all info later, if something is not changed from default.
				MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
				Obj->m_bRemoved = false;
				Obj->m_bVisible = true;
				Obj->m_Color = { 255,255,255,255 };
				Obj->m_Depth = 9;
				Obj->m_iImageIndex = 0;
				Obj->m_iImageId = 0;
				Obj->m_iImageDownIndex = 0;
				Obj->m_iImageDownId = 0;
				Obj->m_iMediaType = MEDIATYPE_TEXT;
				Obj->m_iSprite = agk::CreateText("Hello AppGameKit");
				Obj->m_Layer = 1;
				Obj->bAnimated = false;
				Obj->iAnimationFrom = 0;
				Obj->iAnimationTo = 0;
				Obj->bFixToScreen = false;
				Obj->bStretchToDevice = false;
				Obj->bLocked = false;
				Obj->bOnlyVisibleInAGK = false;
				Obj->m_pNext = NULL;
				Obj->m_Position = { 0.0f,0.0f,0.0f,0.0f };
				agk::SetTextPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
				Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
				Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };
				Obj->m_Size = { 200.0f, 40.0f ,0.0f,0.0f };

				Obj->m_pUsedImage = NULL;
				Obj->m_pUsedDownImage = NULL;

				Obj->m_sMediaFullPath = "";
				Obj->m_sMediaRelativePath = "";
				Obj->m_sName = val;
				Obj->m_sVariable = "";
				Obj->m_Text = "Hello AppGameKit";

				Obj->m_iFontId = 0;
				Obj->m_iLoadFontId = 0;

				if (!m_pLastObj) {
					//No object found, set first entry.
					m_pChangeScene->m_pFirstObject = Obj;
				}
				else {
					//Add the new text object to the chain.
					m_pLastObj->m_pNext = Obj;
				}
				//Disable by default.
				agk::SetTextVisible(Obj->m_iSprite, 0);
				//For calls to additional settings.
				m_pNextObj = Obj;
				m_pChangeScene->iTextCount++;

			}
			else if (cmd.CompareTo("addbutton") == 0 && m_pChangeScene) {

				//Add a button object.
				MediaScene::MediaObject * m_pSearchObj = NULL;
				MediaScene::MediaObject * m_pLastObj = NULL;
				m_pSearchObj = m_pChangeScene->m_pFirstObject;
				if (m_pSearchObj) {
					while (m_pSearchObj) {
						m_pLastObj = m_pSearchObj;
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
				}

				//Just use defaults , so we dont have to save all info later, if something is not changed from default.
				MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
				Obj->m_bRemoved = false;
				Obj->m_bVisible = true;
				Obj->m_Color = { 255,255,255,255 };
				Obj->m_Depth = 9;
				if (nextspriteid > 0) {
					Obj->m_iImageIndex = nextspriteid;
					Obj->m_iImageId = m_pNextImage->m_iImageId;
				}
				else {
					Obj->m_iImageIndex = 0;
					Obj->m_iImageId = agkbutton_image;
				}
				if (nextspriteiddown > 0 && m_pNextImageDown ) {
					Obj->m_iImageDownIndex = nextspriteiddown;
					Obj->m_iImageDownId = m_pNextImageDown->m_iImageId;
				}
				else {
					Obj->m_iImageDownIndex = 0;
					Obj->m_iImageDownId = agkbutton_image;
				}
				Obj->m_iMediaType = MEDIATYPE_BUTTON;
				Obj->m_iSprite = m_pChangeScene->iButtonCount;
				agk::AddVirtualButton(m_pChangeScene->iButtonCount,0,0,80);
				Obj->m_Layer = 1;
				Obj->bAnimated = false;
				Obj->iAnimationFrom = 0;
				Obj->iAnimationTo = 0;
				Obj->bFixToScreen = false;
				Obj->bStretchToDevice = false;
				Obj->bLocked = false;
				Obj->bOnlyVisibleInAGK = false;
				Obj->m_pNext = NULL;
				Obj->m_Position = { 0.0f,0.0f,0.0f,0.0f };
				agk::SetVirtualButtonPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
				Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
				Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };
				Obj->m_Size = { 80.0f, 80.0f ,0.0f,0.0f };

				if (nextspriteid > 0) {
					Obj->m_pUsedImage = m_pNextImage;
				}
				else {
					Obj->m_pUsedImage = NULL;
				}

				if (nextspriteiddown > 0 && m_pNextImageDown) {
					Obj->m_pUsedDownImage = m_pNextImageDown;
				}
				else {
					Obj->m_pUsedDownImage = NULL;
				}

				Obj->m_sMediaFullPath = "";
				Obj->m_sMediaRelativePath = "";
				Obj->m_sName = val;
				Obj->m_sVariable = "";
				Obj->m_Text = "Click";

				Obj->m_iFontId = 0;
				Obj->m_iLoadFontId = 0;

				if (Obj->m_iImageIndex > 0)
					agk::SetVirtualButtonImageUp(Obj->m_iSprite, Obj->m_iImageId);
				else
					agk::SetVirtualButtonImageUp(Obj->m_iSprite, 0);
				if (Obj->m_iImageDownIndex > 0)
					agk::SetVirtualButtonImageDown(Obj->m_iSprite, Obj->m_iImageDownId);
				else
					agk::SetVirtualButtonImageDown(Obj->m_iSprite, 0);

				agk::SetVirtualButtonText(m_pChangeScene->iButtonCount, "Click");
				if (!m_pLastObj) {
					//No object found, set first entry.
					m_pChangeScene->m_pFirstObject = Obj;
				}
				else {
					//Add the new text object to the chain.
					m_pLastObj->m_pNext = Obj;
				}
				//Disable by default.
				agk::SetVirtualButtonVisible(Obj->m_iSprite, 0);
				//For calls to additional settings.
				m_pNextObj = Obj;
				m_pChangeScene->iButtonCount++;

			}
			else if (cmd.CompareTo("addeditbox") == 0 && m_pChangeScene) {

				//Add a button object.
				MediaScene::MediaObject * m_pSearchObj = NULL;
				MediaScene::MediaObject * m_pLastObj = NULL;
				m_pSearchObj = m_pChangeScene->m_pFirstObject;
				if (m_pSearchObj) {
					while (m_pSearchObj) {
						m_pLastObj = m_pSearchObj;
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
				}

				//Just use defaults , so we dont have to save all info later, if something is not changed from default.
				MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
				Obj->m_bRemoved = false;
				Obj->m_bVisible = true;
				Obj->m_Color = { 255,255,255,255 };
				Obj->m_TextColor = { 0,0,0,255 };
				Obj->m_iBorderSize = 2;
				Obj->m_iCursorSize = 2;
				Obj->m_iTextSize = 0;
				Obj->m_iEditMaxChars = 0;
				Obj->bEditMultiLines = false;
				Obj->bEditPassword = false;
				Obj->bEditWrap = false;
				Obj->m_BorderColor = { 0,0,0,255 };
				Obj->m_CursorColor = { 0,0,0,255 };
				Obj->m_Depth = 9;
				Obj->m_iImageIndex = 0;
				Obj->m_iImageId = 0;
				Obj->m_iImageDownIndex = 0;
				Obj->m_iImageDownId = 0;
				Obj->m_iMediaType = MEDIATYPE_EDITBOX;
				Obj->m_iSprite = agk::CreateEditBox();

				Obj->m_Layer = 1;
				Obj->bAnimated = false;
				Obj->iAnimationFrom = 0;
				Obj->iAnimationTo = 0;
				Obj->bFixToScreen = false;
				Obj->bStretchToDevice = false;
				Obj->bLocked = false;
				Obj->bOnlyVisibleInAGK = false;
				Obj->m_pNext = NULL;
				Obj->m_Position = { 0.0f,0.0f,0.0f,0.0f };
				agk::SetEditBoxPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
				Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
				Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };
				Obj->m_Size = { 200.0f, 40.0f ,0.0f,0.0f };
				if (m_pChangeScene->bUsePercentage) {
					agk::SetEditBoxSize(Obj->m_iSprite, (Obj->m_Size.x / (float)m_pChangeScene->iBaseWidth)*100.0f, (Obj->m_Size.y / (float)m_pChangeScene->iBaseHeight)*100.0f);
				}
				else {
					agk::SetEditBoxSize(Obj->m_iSprite, Obj->m_Size.x, Obj->m_Size.y);
				}
				Obj->m_pUsedImage = NULL;
				Obj->m_pUsedDownImage = NULL;

				Obj->m_sMediaFullPath = "";
				Obj->m_sMediaRelativePath = "";
				Obj->m_sName = val;
				Obj->m_sVariable = "";
				Obj->m_Text = "Edit Box";

				Obj->m_iFontId = 0;
				Obj->m_iLoadFontId = 0;


				agk::SetEditBoxText(Obj->m_iSprite, "Edit Box");
				if (!m_pLastObj) {
					//No object found, set first entry.
					m_pChangeScene->m_pFirstObject = Obj;
				}
				else {
					//Add the new text object to the chain.
					m_pLastObj->m_pNext = Obj;
				}
				//Disable by default.
				agk::SetEditBoxVisible(Obj->m_iSprite, 0);
				//For calls to additional settings.
				m_pNextObj = Obj;
				m_pChangeScene->iEditBoxCount++;

			}
			else if (cmd.CompareTo("mediatype") == 0 && m_pNextObj) {
				m_pNextObj->m_iMediaType = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("mediavisible") == 0 && m_pNextObj) {
				m_pNextObj->m_bVisible = atoi(val.GetStr()) != 0;
			}

			else if (cmd.CompareTo("spriteposx") == 0 && m_pNextObj) {
				m_pNextObj->m_Position.x = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE) {
					if (m_pChangeScene->bUsePercentage) {
						agk::SetSpritePosition(m_pNextObj->m_iSprite, (m_pNextObj->m_Position.x / (float)m_pChangeScene->iBaseWidth)*100.0f, (m_pNextObj->m_Position.y / (float)m_pChangeScene->iBaseHeight)*100.0f);
					}
					else {
						agk::SetSpritePosition(m_pNextObj->m_iSprite, m_pNextObj->m_Position.x, m_pNextObj->m_Position.y);
					}
				}
			}
			else if (cmd.CompareTo("spriteposy") == 0 && m_pNextObj) {
				m_pNextObj->m_Position.y = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE) {
					if (m_pChangeScene->bUsePercentage) {
						agk::SetSpritePosition(m_pNextObj->m_iSprite, (m_pNextObj->m_Position.x / (float)m_pChangeScene->iBaseWidth)*100.0f, (m_pNextObj->m_Position.y / (float)m_pChangeScene->iBaseHeight)*100.0f);
					}
					else {
						agk::SetSpritePosition(m_pNextObj->m_iSprite, m_pNextObj->m_Position.x, m_pNextObj->m_Position.y);
					}
				}
			}
			else if (cmd.CompareTo("spritescalex") == 0 && m_pNextObj) {
				m_pNextObj->m_Scale.x = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE) {
					if (m_pChangeScene->bUsePercentage) {
						agk::SetSpriteSize(m_pNextObj->m_iSprite, (m_pNextObj->m_Size.x / (float)m_pChangeScene->iBaseWidth)*100.0f, (m_pNextObj->m_Size.y / (float)m_pChangeScene->iBaseHeight)*100.0f);
					}
					else {
						agk::SetSpriteSize(m_pNextObj->m_iSprite, m_pNextObj->m_Size.x, m_pNextObj->m_Size.y);
					}
				}
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteScale(m_pNextObj->m_iSprite, m_pNextObj->m_Scale.x, m_pNextObj->m_Scale.y);
			}
			else if (cmd.CompareTo("spritescaley") == 0 && m_pNextObj) {
				m_pNextObj->m_Scale.y = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE) {
					if (m_pChangeScene->bUsePercentage) {
						agk::SetSpriteSize(m_pNextObj->m_iSprite, (m_pNextObj->m_Size.x / (float)m_pChangeScene->iBaseWidth)*100.0f, (m_pNextObj->m_Size.y / (float)m_pChangeScene->iBaseHeight)*100.0f);
					}
					else {
						agk::SetSpriteSize(m_pNextObj->m_iSprite, m_pNextObj->m_Size.x, m_pNextObj->m_Size.y);
					}
				}
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteScale(m_pNextObj->m_iSprite, m_pNextObj->m_Scale.x, m_pNextObj->m_Scale.y);
			}
			else if (cmd.CompareTo("spriteanglex") == 0 && m_pNextObj) {
				m_pNextObj->m_Rotation.x = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteAngle(m_pNextObj->m_iSprite, m_pNextObj->m_Rotation.x);
			}
			else if (cmd.CompareTo("spritesizex") == 0 && m_pNextObj) {
				m_pNextObj->m_Size.x = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE) {
					if (m_pChangeScene->bUsePercentage) {
						agk::SetSpriteSize(m_pNextObj->m_iSprite, (m_pNextObj->m_Size.x / (float)m_pChangeScene->iBaseWidth)*100.0f, (m_pNextObj->m_Size.y / (float)m_pChangeScene->iBaseHeight)*100.0f);
					}
					else {
						agk::SetSpriteSize(m_pNextObj->m_iSprite, m_pNextObj->m_Size.x, m_pNextObj->m_Size.y);
					}
				}
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteScale(m_pNextObj->m_iSprite, m_pNextObj->m_Scale.x, m_pNextObj->m_Scale.y);
			}
			else if (cmd.CompareTo("spritesizey") == 0 && m_pNextObj) {
				m_pNextObj->m_Size.y = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE) {
					if (m_pChangeScene->bUsePercentage) {
						agk::SetSpriteSize(m_pNextObj->m_iSprite, (m_pNextObj->m_Size.x / (float)m_pChangeScene->iBaseWidth)*100.0f, (m_pNextObj->m_Size.y / (float)m_pChangeScene->iBaseHeight)*100.0f);
					}
					else {
						agk::SetSpriteSize(m_pNextObj->m_iSprite, m_pNextObj->m_Size.x, m_pNextObj->m_Size.y);
					}
				}
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteScale(m_pNextObj->m_iSprite, m_pNextObj->m_Scale.x, m_pNextObj->m_Scale.y);
			}
			else if (cmd.CompareTo("spritecolorx") == 0 && m_pNextObj) {
				m_pNextObj->m_Color.x = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteColor(m_pNextObj->m_iSprite, (int) m_pNextObj->m_Color.x, (int) m_pNextObj->m_Color.y, (int) m_pNextObj->m_Color.z, (int) m_pNextObj->m_Color.w);
			}
			else if (cmd.CompareTo("spritecolory") == 0 && m_pNextObj) {
				m_pNextObj->m_Color.y = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteColor(m_pNextObj->m_iSprite, (int) m_pNextObj->m_Color.x, (int) m_pNextObj->m_Color.y, (int) m_pNextObj->m_Color.z, (int) m_pNextObj->m_Color.w);
			}
			else if (cmd.CompareTo("spritecolorz") == 0 && m_pNextObj) {
				m_pNextObj->m_Color.z = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteColor(m_pNextObj->m_iSprite, (int) m_pNextObj->m_Color.x, (int) m_pNextObj->m_Color.y, (int) m_pNextObj->m_Color.z, (int) m_pNextObj->m_Color.w);
			}
			else if (cmd.CompareTo("spritecolorw") == 0 && m_pNextObj) {
				m_pNextObj->m_Color.w = (float) atof(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteColor(m_pNextObj->m_iSprite, (int) m_pNextObj->m_Color.x, (int) m_pNextObj->m_Color.y, (int) m_pNextObj->m_Color.z, (int) m_pNextObj->m_Color.w);
			}
			else if (cmd.CompareTo("editbordersize") == 0 && m_pNextObj) {
				m_pNextObj->m_iBorderSize = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("editcursorsize") == 0 && m_pNextObj) {
				m_pNextObj->m_iCursorSize = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("edittextsize") == 0 && m_pNextObj) {
				m_pNextObj->m_iTextSize = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("editmaxchars") == 0 && m_pNextObj) {
				m_pNextObj->m_iEditMaxChars = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("editmultiline") == 0 && m_pNextObj) {
				m_pNextObj->bEditMultiLines = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("editpassword") == 0 && m_pNextObj) {
				m_pNextObj->bEditPassword = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("editwrap") == 0 && m_pNextObj) {
				m_pNextObj->bEditWrap = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("edittextx") == 0 && m_pNextObj) {
				m_pNextObj->m_TextColor.x = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("edittexty") == 0 && m_pNextObj) {
				m_pNextObj->m_TextColor.y = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("edittextz") == 0 && m_pNextObj) {
				m_pNextObj->m_TextColor.z = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("edittextw") == 0 && m_pNextObj) {
				m_pNextObj->m_TextColor.w = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("editborderx") == 0 && m_pNextObj) {
				m_pNextObj->m_BorderColor.x = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("editbordery") == 0 && m_pNextObj) {
				m_pNextObj->m_BorderColor.y = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("editborderz") == 0 && m_pNextObj) {
				m_pNextObj->m_BorderColor.z = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("editborderw") == 0 && m_pNextObj) {
				m_pNextObj->m_BorderColor.w = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("editcursorx") == 0 && m_pNextObj) {
				m_pNextObj->m_CursorColor.x = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("editcursory") == 0 && m_pNextObj) {
				m_pNextObj->m_CursorColor.y = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("editcursorz") == 0 && m_pNextObj) {
				m_pNextObj->m_CursorColor.z = (float) atof(val.GetStr());
			}

			else if (cmd.CompareTo("spritedepth") == 0 && m_pNextObj) {
				m_pNextObj->m_Depth = atoi(val.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteDepth(m_pNextObj->m_iSprite, m_pNextObj->m_Depth);
			}
			else if (cmd.CompareTo("spritelocked") == 0 && m_pNextObj) {
				m_pNextObj->bLocked = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("spritestretch") == 0 && m_pNextObj) {
				m_pNextObj->bStretchToDevice = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("spritefixtoscreen") == 0 && m_pNextObj) {
				m_pNextObj->bFixToScreen = atoi(val.GetStr()) != 0;
			}

			else if (cmd.CompareTo("spriteuvoffset") == 0 && m_pNextObj) {
				m_pNextObj->bUVOffset = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("spriteuvspeedx") == 0 && m_pNextObj) {
				m_pNextObj->fUVSpeedX = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("spriteuvspeedy") == 0 && m_pNextObj) {
				m_pNextObj->fUVSpeedY = (float) atof(val.GetStr());
			}

			else if (cmd.CompareTo("spritevariable") == 0 && m_pNextObj) {
				m_pNextObj->m_sVariable = val;
			}
			else if (cmd.CompareTo("spritegroup") == 0 && m_pNextObj) {
				m_pNextObj->m_sArrayGroup = val;
			}	
			else if (cmd.CompareTo("mediatext") == 0 && m_pNextObj) {
				m_pNextObj->m_Text = val;
				if (m_pNextObj->m_iMediaType == MEDIATYPE_TEXT)
					agk::SetTextString(m_pNextObj->m_iSprite, m_pNextObj->m_Text.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_BUTTON)
					agk::SetVirtualButtonText(m_pNextObj->m_iSprite, m_pNextObj->m_Text.GetStr());
				if (m_pNextObj->m_iMediaType == MEDIATYPE_EDITBOX)
					agk::SetEditBoxText(m_pNextObj->m_iSprite, m_pNextObj->m_Text.GetStr());
			}
			else if (cmd.CompareTo("spriteagkvisible") == 0 && m_pNextObj) {
				m_pNextObj->bOnlyVisibleInAGK = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("spriteanimated") == 0 && m_pNextObj) {
				m_pNextObj->bAnimated = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("spriteanimfrom") == 0 && m_pNextObj) {
				m_pNextObj->iAnimationFrom = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("spriteanimto") == 0 && m_pNextObj) {
				m_pNextObj->iAnimationTo = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("physicsenable") == 0 && m_pNextObj) {
				m_pNextObj->bEnablePhysics = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("physicsmode") == 0 && m_pNextObj) {
				m_pNextObj->iPhysicsMode = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("physicsshape") == 0 && m_pNextObj) {
				m_pNextObj->iPhysicsShape = atoi(val.GetStr());
			}
			else if (cmd.CompareTo("physicsrotate") == 0 && m_pNextObj) {
				m_pNextObj->bPhysicsCanRotate = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("physicsbullet") == 0 && m_pNextObj) {
				m_pNextObj->bPhysicsIsBullet = atoi(val.GetStr()) != 0;
			}
			else if (cmd.CompareTo("physicsrestitution") == 0 && m_pNextObj) {
				m_pNextObj->fPhysicsRestitution = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("physicscomx") == 0 && m_pNextObj) {
				m_pNextObj->fPhysicsCOMX = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("physicscomy") == 0 && m_pNextObj) {
				m_pNextObj->fPhysicsCOMY = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("physicsmass") == 0 && m_pNextObj) {
				m_pNextObj->fPhysicsMass = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("physicsfriction") == 0 && m_pNextObj) {
				m_pNextObj->fPhysicsFriction = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("physicsliniardamping") == 0 && m_pNextObj) {
				m_pNextObj->fPhysicsLinearDamping = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("physicsangulardamping") == 0 && m_pNextObj) {
				m_pNextObj->fPhysicsAngularDamping = (float) atof(val.GetStr());
			}
			else if (cmd.CompareTo("fontid") == 0 && m_pNextObj) {
				m_pNextObj->m_iFontId = atoi(val.GetStr());
			}

			
			//spriteindex

		}

	}
	m_editor->filechanged = false;

}


static float fCountToOne = 0.0f;
void MediaScene::RenderScene( void )
{ 
	extern int iDeviceWidth, iDeviceHeight;
	agk::SetErrorMode(0); //No popup messages.

	//agk::SetAntialiasMode(1);
	//agk::SetVirtualResolution(iBaseWidth, iBaseHeight); // 360x640
	agk::SetScissor(0, 0, 0, 0);

	agk::SetRenderToImage(iRenderTarget, -1);

	if (bUsePercentage) {
		agk::SetVirtualResolution(100, 100);
		float aspect = ((float)iBaseWidth / (float)iBaseHeight);
		agk::SetDisplayAspect(aspect);

	}
	else {
		if (bFollowDeviceResolution) {
			agk::SetVirtualResolution(iScreenWidth, iScreenHeight);
		}
		else {
			agk::SetVirtualResolution(iBaseWidth, iBaseHeight);
	//		float aspect = ((float)iScreenWidth / (float)iScreenHeight) / fDisplayAspect ;
	//		agk::SetDisplayAspect(aspect);
		}
	}
	//TestDraw();

	agk::SetClearColor(iSceneBackColor[0], iSceneBackColor[1], iSceneBackColor[2]); //iSceneBackColor[0]

	agk::ClearScreen();
	agk::SetCameraPosition(1, 0, 0, 0);

	//agk::SetPrintSize(20);
	//agk::Print("AGK-Window");
	//agk::Print(iScreenWidth);
	//agk::Print(iScreenHeight);
	//agk::Print(iBaseWidth);
	//agk::Print(iBaseHeight);

	float tween = 60.0f / (1.0f / agk::GetFrameTime());

		MediaScene::MediaObject * m_pSearchObj = NULL;
		m_pSearchObj = m_pFirstObject;
		while (m_pSearchObj) {
			if (!m_pSearchObj->m_bRemoved) {
				if (m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT) {

					if (m_pSearchObj->bFixToScreen) {
						if (bUsePercentage) {
							agk::SetTextPosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
						}
						else {
							agk::SetTextPosition(m_pSearchObj->m_iSprite, m_pSearchObj->m_Position.x + agk::GetViewOffsetX(), m_pSearchObj->m_Position.y + agk::GetViewOffsetY());
						}
					}

				}
				if (m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {

					if (m_pSearchObj->bFixToScreen) {
						if (bUsePercentage) {
							agk::SetVirtualButtonPosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
						}
						else {
							agk::SetVirtualButtonPosition(m_pSearchObj->m_iSprite, m_pSearchObj->m_Position.x + agk::GetViewOffsetX(), m_pSearchObj->m_Position.y + agk::GetViewOffsetY());
						}
					}

				}
				if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {

					if (m_pSearchObj->bFixToScreen) {
						if (bUsePercentage) {
							agk::SetEditBoxPosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
						}
						else {
							agk::SetEditBoxPosition(m_pSearchObj->m_iSprite, m_pSearchObj->m_Position.x + agk::GetViewOffsetX(), m_pSearchObj->m_Position.y + agk::GetViewOffsetY());
						}
					}

				}
				if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE) {

					if (m_pSearchObj->bFixToScreen) {
						if (bUsePercentage) {
							agk::SetSpritePosition(m_pSearchObj->m_iSprite, (m_pSearchObj->m_Position.x / (float)iBaseWidth)*100.0f, (m_pSearchObj->m_Position.y / (float)iBaseHeight)*100.0f);
						}
						else {
							agk::SetSpritePosition(m_pSearchObj->m_iSprite, m_pSearchObj->m_Position.x + agk::GetViewOffsetX(), m_pSearchObj->m_Position.y + agk::GetViewOffsetY());
						}
					}

					if (m_pSearchObj->bUVOffset && (m_pSearchObj->fUVSpeedX != 0 || m_pSearchObj->fUVSpeedY != 0)) {

						int width = (int) agk::GetImageWidth(m_pSearchObj->m_iImageId);
						if ((width & (width - 1)) != 0) // check power of 2
						{
							//Not power of two.
						}
						else {

							if (bTestMode) {
								agk::SetImageWrapU(m_pSearchObj->m_iImageId, 1);
								agk::SetImageWrapV(m_pSearchObj->m_iImageId, 1);
								agk::SetSpriteUVBorder(m_pSearchObj->m_iSprite, 0.5f);

								m_pSearchObj->fUVCountX = m_pSearchObj->fUVCountX + (m_pSearchObj->fUVSpeedX * tween);
								if (m_pSearchObj->fUVCountX > 1.0f) m_pSearchObj->fUVCountX -= 1.0f;
								if (m_pSearchObj->fUVCountX < 1.0f) m_pSearchObj->fUVCountX += 1.0f;

								m_pSearchObj->fUVCountY = m_pSearchObj->fUVCountY + (m_pSearchObj->fUVSpeedY * tween);
								if (m_pSearchObj->fUVCountY > 1.0f) m_pSearchObj->fUVCountY -= 1.0f;
								if (m_pSearchObj->fUVCountY < 1.0f) m_pSearchObj->fUVCountY += 1.0f;

								if (m_pSearchObj->fUVSpeedY == 0.0f)
									agk::SetSpriteUVOffset(m_pSearchObj->m_iSprite, m_pSearchObj->fUVCountX, 0.0f);
								else if (m_pSearchObj->fUVSpeedX == 0.0f)
									agk::SetSpriteUVOffset(m_pSearchObj->m_iSprite, 0.0f, m_pSearchObj->fUVCountY);
								else
									agk::SetSpriteUVOffset(m_pSearchObj->m_iSprite, m_pSearchObj->fUVCountX, m_pSearchObj->fUVCountY);

							}
							else {
								agk::SetSpriteUVOffset(m_pSearchObj->m_iSprite, 0, 0);
							}
						}
						ide_force_rendering_delayed = true;
						//ide_force_rendering = true;
					}
				}
			}
			m_pSearchObj = m_pSearchObj->m_pNext;
		}


	if(scene_editor_testmode)
		agk::Update();
	
	agk::Render2DBack();
	agk::ClearDepthBuffer();
	agk::Render3D();
	agk::ClearDepthBuffer();
	agk::Render2DFront();
	agk::SetRenderToScreen();
	agk::SetClearColor(0, 0, 0);
	
	agk::SetVirtualResolution(iDeviceWidth, iDeviceHeight);
}

void MediaScene::ProcessDragDrop(MediaScene * m_ActiveScene, const char* winname, MediaScene * m_pMainScene)
{
	extern cFolderItem::sFolderFiles *pDragDropFile;
	extern char *pDragDropText;

	if (!m_ActiveScene)
		return;
	if (!winname)
		return;

	if (pDragDropText) {
		pDragDropFile = NULL;
		return;
	}

	if (!pDragDropFile)
		return;

	if ( 1==1 || ImGui::IsWindowHovered()) {

		float mousex = ImGui::GetMousePos().x -ImGui::GetWindowPos().x;
		float mousey = ImGui::GetMousePos().y -ImGui::GetWindowPos().y;

		ImVec2 cp;
		cp.x = ImGui::GetScrollX() + mousex;
		cp.y = ImGui::GetScrollY() + mousey;

		bool dragdropactive = false;
		char *projectName = NULL;
		projectName = (char *)pDragDropFile->m_sFolder.GetStr();
		if (strstr(winname, projectName) != NULL) {
			dragdropactive = true;
		}
		else {
			uString folder, sfinal;
			//m_pMainScene
//			folder = m_ActiveScene->m_sSceneFullPath;
			folder = m_pMainScene->m_sSceneFullPath;
			int pos = folder.RevFindStr("/");
			if (pos < 0) folder.RevFindStr("\\");
			if (pos > 0) {
				folder.SubString(sfinal, 0, pos);
				if (strstr( pDragDropFile->m_sPath.GetStr() , sfinal.GetStr() ) != NULL) {
					dragdropactive = true;
					pDragDropText = NULL;
				}
			}
		}
		

		if (!dragdropactive) {
			ImVec2 oldCursor = ImGui::GetCursorPos();

			ImGui::SetCursorPos(cp);
			ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y));

			if (ImGui::BeginDragDropTarget())
			{
				ImGui::BeginTooltip();
//				ImGui::Text("You can only drag/drop media that\nare placed inside your project folder.");
				ImGui::Text("Media will be copied to current project!");
				ImGui::EndTooltip();
				ImGui::EndDragDropTarget();
			}
			ImGui::SetCursorPos(oldCursor);
//			return;
		}

		float xRatio = (m_ActiveScene->fGridX*m_ActiveScene->fRatio);
		float yRatio = (m_ActiveScene->fGridY*m_ActiveScene->fRatio);

		float scalex = m_ActiveScene->fScaleToScreenX;
		float scaley = m_ActiveScene->fScaleToScreenY;

		ImVec2 wp;
		wp.x = ImGui::GetScrollX() + (mousex);
		wp.y = ImGui::GetScrollY() + (mousey);

//		float alignToGridX = 0.0f;
//		float alignToGridY = 0.0f;
//		if (m_ActiveScene->bGridOn) {
//			float alignToGridX = fmod(ImGui::GetScrollX() - m_ActiveScene->centerSceneX - fScissorXScaled, xRatio);
//			float alignToGridY = fmod(ImGui::GetScrollY() - m_ActiveScene->centerSceneY - fScissorYScaled, yRatio);
//		}

		float mouseToscreenx = (wp.x - m_ActiveScene->centerSceneX) - m_ActiveScene->fCenterX;// -fScissorX;
		float mouseToscreeny = (wp.y - m_ActiveScene->centerSceneY) - m_ActiveScene->fCenterY;// -fScissorY;

		if (m_ActiveScene->fRatio != 0.0f) {


			mouseToscreenx /= m_ActiveScene->fRatio;
			mouseToscreeny /= m_ActiveScene->fRatio;

			mouseToscreenx -= m_ActiveScene->fScissorX;
			mouseToscreeny -= m_ActiveScene->fScissorY;

			mouseToscreenx *= scalex;
			mouseToscreeny *= scaley;

			if (m_ActiveScene->bSnapToGrid && m_ActiveScene->bGridOn) {
				int align = (int) (mouseToscreenx / m_ActiveScene->fGridX);
				if (mouseToscreenx < 0.0f)
					align -= 1;
				mouseToscreenx = align * m_ActiveScene->fGridX;
				mouseToscreenx += fGridXOffset;
				align = (int) (mouseToscreeny / m_ActiveScene->fGridY);
				if (mouseToscreeny < 0.0f)
					align -= 1;
				mouseToscreeny = align * m_ActiveScene->fGridY;
				mouseToscreeny += fGridYOffset;
			}

		}

		ImGui::SetCursorPos(cp- ImVec2(20, 20)); //TitleSize
//		ImGui::Text("Mouse: %.2f,%.2f Screen: %.2f,%.2f", mousex, mousey, mouseToscreenx, mouseToscreeny);
//		ImGui::Text("Screen: %.2f,%.2f", mouseToscreenx, mouseToscreeny);

		ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x , ImGui::GetWindowSize().y));

		if (ImGui::BeginDragDropTarget())
		{

			ImVec4 *style_colors = ImGui::GetStyle().Colors;
			ImVec2 a[4];
			float linesize = 10.0f;

			ImVec2 dpos = ImGui::GetWindowPos();
			dpos.x += mousex;
			dpos.y += mousey;

			a[0].x = dpos.x - linesize;
			a[0].y = dpos.y - linesize;
			a[1].x = dpos.x + linesize;
			a[1].y = dpos.y + linesize;
			ImGui::GetWindowDrawList()->AddPolyline(&a[0], 2, ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram])), false, 2.0f);
			a[0].x = dpos.x - linesize;
			a[0].y = dpos.y + linesize;
			a[1].x = dpos.x + linesize;
			a[1].y = dpos.y - linesize;
			ImGui::GetWindowDrawList()->AddPolyline(&a[0], 2, ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram])), false, 2.0f);

			//g.DragDropAcceptIdPrev = g.DragDropAcceptIdCurr;

			ImGuiContext& g = *GImGui;
			//Always accept drag drop.
			g.DragDropAcceptIdPrev = g.DragDropTargetId;
			
//			const bool was_accepted_previously = (g.DragDropAcceptIdPrev == g.DragDropTargetId);
//			ImGui::SetCursorPos(cp-ImVec2(50,50)); //TitleSize
//			if (!was_accepted_previously)
//				ImGui::Text("@@@@");
//			ImGui::Text("drop: %d,%d", g.DragDropAcceptIdPrev, g.DragDropTargetId);
			

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MODEL_DROP_TARGET", ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) // ImGuiDragDropFlags_AcceptNoDrawDefaultRect
			{
				if (pDragDropFile) {
					IM_ASSERT(payload->DataSize == sizeof(cFolderItem::sFolderFiles *));
					cFolderItem::sFolderFiles * payload_n = (cFolderItem::sFolderFiles *) payload->Data;
					payload_n = payload_n->m_dropptr;
					if (payload_n) {
						uString fullfilename, relativeName, ext, relativeNameNoExt;
						fullfilename = payload_n->m_sPath;
						int pos = fullfilename.RevFindStr("/");
						if (!(fullfilename.GetLength() - 1 == pos)) {
							fullfilename.Append("/");
						}
						fullfilename.Append(payload_n->m_sName);

						if (!dragdropactive) {
							relativeName = payload_n->m_sName;
							pos = relativeName.RevFindStr(".");
							ext = "na";
							if (pos > 0) {
								relativeName.SubString(ext, pos);
							}
							//Supported media.
							if (pestrcasestr(ext.GetStr(), ".png") ||
								pestrcasestr(ext.GetStr(), ".jpg") ||
								pestrcasestr(ext.GetStr(), ".jpeg") ||
								pestrcasestr(ext.GetStr(), ".bmp") ||
								pestrcasestr(ext.GetStr(), ".gif") ||
								pestrcasestr(ext.GetStr(), ".ttf")
								) {

								//Media is outside project folder, copy and change fullfilename
								if (renderingProject) {
									uString usDestination = renderingProject->m_sProjectFileFullPath;
									int pos = usDestination.RevFindStr("/");
									if (pos <= 0) pos = usDestination.RevFindStr("\\");
									if (pos > 0) {
										uString usFinal;
										usDestination.SubString(usFinal, 0, pos + 1);
										usFinal.Append("media/");
										//usFinal.Append(m_pSelectedFile->m_sName);
										usFinal.Append(payload_n->m_sName);

										uString usSource = fullfilename;

										bool oksave = true;
										if (file_exists((char*)usFinal.GetStr())) {
											oksave = overWriteFileBox((char*)usFinal.GetStr());
										}
										if (oksave) {
											cp_copyfile((char*)usSource.GetStr(), (char*)usFinal.GetStr());

										}

										fullfilename = usFinal.GetStr();
									}
									else {
										//Failed to get filenames
										pDragDropFile = NULL;
										ImGui::EndDragDropTarget();
										return;
									}

								}
								else {
									//No rendering project.
									pDragDropFile = NULL;
									ImGui::EndDragDropTarget();
									return;
								}
							}
							else {
								//Not supported media.
								pDragDropFile = NULL;
								ImGui::EndDragDropTarget();
								return;
							}
						}

						pos = pecasepos(fullfilename.GetStr(), "/media/");
						if (pos >= 0)
							fullfilename.SubString(relativeName, pos + 7);
						else
							relativeName = payload_n->m_sName;

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

						//.fpe,.png,.jpg,.dds,.jpeg,.gif,.tif,.wav,.ogg,.mp3,.obj,.x,.fbx,.3ds,.dae,.b3d,.3d,.lwo,.ago

						uString variable;
						variable = payload_n->m_sName;
						variable.ReplaceStr(".", "_");
						variable.ReplaceStr(" ", "_");
						variable.ReplaceStr("-", "");
						variable.ReplaceStr("*", "");
						variable.ReplaceStr("+", "");
						variable.ReplaceStr("/", "");
						variable.ReplaceStr("\\", "");
						variable.ReplaceStr("|", "");
						variable.ReplaceStr("$", "");
						variable.ReplaceStr("#", "");
						variable.ReplaceStr("%", "");
						variable.ReplaceStr("(", "");
						variable.ReplaceStr(")", "");
						variable.ReplaceStr("[", "");
						variable.ReplaceStr("]", "");
						variable.ReplaceStr("?", "");
						variable.ReplaceStr("`", "");
						variable.ReplaceStr(",", "");
						variable.ReplaceStr("_ps", "");
						variable.ReplaceStr("_vs", "");
						variable.ReplaceStr("_png", "");
						variable.ReplaceStr("_jpg", "");
						variable.ReplaceStr("_jpeg", "");
						variable.ReplaceStr("_bmp", "");
						variable.ReplaceStr("_gif", "");
						variable.ReplaceStr("_wav", "");
						variable.ReplaceStr("_ogg", "");
						variable.ReplaceStr("_mp3", "");

						if (pestrcasestr(ext.GetStr(), ".png") ||
							pestrcasestr(ext.GetStr(), ".jpg") ||
							pestrcasestr(ext.GetStr(), ".jpeg") ||
							pestrcasestr(ext.GetStr(), ".bmp") ||
							pestrcasestr(ext.GetStr(), ".gif")) {


							//Check if already there.
							MediaScene::Images * m_pSearchImage = NULL;
							MediaScene::Images * m_pLastImage = NULL;
							m_pSearchImage = m_ActiveScene->m_pFirstImage;
							if (m_pSearchImage) {
								while (m_pSearchImage) {
									if (m_pSearchImage->m_sFullPath.CompareCaseTo(fullfilename) == 0) {
										break;
									}
									m_pLastImage = m_pSearchImage;
									m_pSearchImage = m_pSearchImage->m_pNext;
								}
							}

							if (!m_pSearchImage) {
								//Not found continue
								//Add image and a default sprite.
								MediaScene::Images * Image = new MediaScene::Images();
								Image->m_sFullPath = fullfilename;
								Image->m_sRelativePath = relativeName;
								Image->m_sName = relativeName;
								Image->m_pNext = NULL;
								Image->m_bRemoved = false;
								Image->m_bAtlas = false;
								Image->m_iRows = 1;
								Image->m_iColumns = 1;
								uString sIName;
								sIName = "raw:";
								sIName.Append(fullfilename);
								agk::SetGenerateMipmaps(0);
								Image->m_iImageId = agk::LoadImage(sIName.GetStr());
								agk::SetImageWrapU(Image->m_iImageId, 1);
								agk::SetImageWrapV(Image->m_iImageId, 1);

								Image->m_iWidth = (int) agk::GetImageWidth(Image->m_iImageId);
								Image->m_iHeight = (int) agk::GetImageHeight(Image->m_iImageId);
								Image->m_iImageIndex = m_ActiveScene->iImageCount;
								if (!m_pLastImage) {
									//No image found, set first entry.
									m_ActiveScene->m_pFirstImage = Image;
								}
								else {
									//Add the new image to the chain.
									m_pLastImage->m_pNext = Image;
								}

								m_pSearchImage = Image;
								m_ActiveScene->iImageCount++;
							}


							if (m_pSearchImage) {
								//Add a sprite object.
								//mousex
								MediaScene::MediaObject * m_pSearchObj = NULL;
								MediaScene::MediaObject * m_pLastObj = NULL;
								m_pSearchObj = m_ActiveScene->m_pFirstObject;
								if (m_pSearchObj) {
									while (m_pSearchObj) {
										m_pLastObj = m_pSearchObj;
										m_pSearchObj = m_pSearchObj->m_pNext;
									}
								}

								MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
								Obj->m_bRemoved = false;
								Obj->m_bVisible = true;
								Obj->m_Color = { 255,255,255,255 };
//								if(m_ActiveScene->iSpriteCount < 3000)
//									Obj->m_Depth = 4000- m_ActiveScene->iSpriteCount;
//								else
									Obj->m_Depth = 5000;
								Obj->m_iImageIndex = m_pSearchImage->m_iImageIndex;
								Obj->m_iImageId = m_pSearchImage->m_iImageId;
								Obj->m_iImageDownIndex = 0;
								Obj->m_iImageDownId = 0;
								Obj->m_iMediaType = MEDIATYPE_SPRITE;
								Obj->m_iSprite = agk::CreateSprite(m_pSearchImage->m_iImageId);
								Obj->m_Layer = 1;
								Obj->bFixToScreen = false;
								Obj->bStretchToDevice = false;
								Obj->bLocked = false;
								Obj->bOnlyVisibleInAGK = false;
								Obj->bAnimated = false;
								Obj->iAnimationFrom = payload_n->iAnimationFrom;
								Obj->iAnimationTo = 0;

								Obj->m_pUsedImage = m_pSearchImage;
								Obj->m_pUsedDownImage = NULL;

								Obj->m_pNext = NULL;
								Obj->m_Position = { mouseToscreenx,mouseToscreeny,0.0f,0.0f };
								if (bUsePercentage) {
									agk::SetSpritePosition(Obj->m_iSprite, (Obj->m_Position.x / (float)iBaseWidth)*100.0f, (Obj->m_Position.y / (float)iBaseHeight)*100.0f);
								}
								else {
									agk::SetSpritePosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
								}
								Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
								Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };

								if (m_pSearchImage->m_bAtlas) {
									int fw = (int)m_pSearchImage->m_iWidth / m_pSearchImage->m_iRows;
									int fh = (int)m_pSearchImage->m_iHeight / m_pSearchImage->m_iColumns;
									Obj->m_Size = { (float)fw, (float)fh ,0.0f,0.0f };
								}
								else {
									Obj->m_Size = { (float)m_pSearchImage->m_iWidth, (float)m_pSearchImage->m_iHeight ,0.0f,0.0f };
								}
								Obj->m_sMediaFullPath = fullfilename;
								Obj->m_sMediaRelativePath = relativeName;

								Obj->bEnablePhysics = false;
								Obj->iPhysicsShape = 3;
								Obj->iPhysicsMode = 0;
								Obj->bPhysicsCanRotate = true;
								Obj->bPhysicsIsBullet = false;
								Obj->fPhysicsCOMX = 0.0f;
								Obj->fPhysicsCOMY = 0.0f;

								Obj->fPhysicsMass = 100.0f;
								Obj->fPhysicsFriction = 0.5f;
								Obj->fPhysicsLinearDamping = 0.0f;
								Obj->fPhysicsAngularDamping = 0.0f;

								Obj->m_iFontId = 0;
								Obj->m_iLoadFontId = 0;

								//
								bool found = true;
								while (found) {
									found = false;
									uString sName = "Sprite ";
									sName.AppendInt(m_ActiveScene->iSpriteCount++);
									Obj->m_sName = sName;

									m_pSearchObj = m_ActiveScene->m_pFirstObject;
									if (m_pSearchObj) {
										while (m_pSearchObj) {
											if (m_pSearchObj->m_sName.CompareTo(sName) == 0) {
												found = true;
												break;
											}
											m_pSearchObj = m_pSearchObj->m_pNext;
										}
									}
								}

								Obj->m_sVariable = "";

								if (!m_pLastObj) {
									//No image found, set first entry.
									m_ActiveScene->m_pFirstObject = Obj;
								}
								else {
									//Add the new image to the chain.
									m_pLastObj->m_pNext = Obj;
								}

								AddUndo(Obj, true);

								m_pSelectedObject = Obj;

							}
						}

						if (pestrcasestr(ext.GetStr(), ".ttf")) {
							if (m_ActiveScene->iFontCount < MAXUSERFONTS) {
								bool fontexists = false;
								if (m_ActiveScene->Fonts.size() > 0) {
									for (std::map<std::int32_t, std::string>::iterator it = m_ActiveScene->Fonts.begin(); it != m_ActiveScene->Fonts.end(); ++it)
									{
										if (strcmp(fullfilename.GetStr(), it->second.c_str() ) == 0) {
											fontexists = true;
											break;
										}
									}
								}
								if(!fontexists)
									m_ActiveScene->Fonts.insert(std::make_pair(std::int32_t(m_ActiveScene->iFontCount++), std::string(fullfilename.GetStr())));
							}
						}

						if (pestrcasestr(ext.GetStr(), ".wav")) {
							//
						}
						if (pestrcasestr(ext.GetStr(), ".fbx") ||
							pestrcasestr(ext.GetStr(), ".obj") ||
							pestrcasestr(ext.GetStr(), ".x") ||
							pestrcasestr(ext.GetStr(), ".3ds") ||
							pestrcasestr(ext.GetStr(), ".dae") ||
							pestrcasestr(ext.GetStr(), ".fbx") ||
							pestrcasestr(ext.GetStr(), ".ago") ||
							pestrcasestr(ext.GetStr(), ".lwo")) {
							//
						}
						if (pestrcasestr(ext.GetStr(), ".ogg")) {
							//
						}
						if (pestrcasestr(ext.GetStr(), ".mp3")) {
							//
						}
						if (pestrcasestr(ext.GetStr(), ".ps")) {
							//
						}
						if (pestrcasestr(ext.GetStr(), ".vs")) {
							//
						}

					}
					pDragDropFile = NULL;
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}


void MediaScene::AddUndo(MediaScene::MediaObject * m_pUndoObj, bool remove, int count)
{
//	int iUndoIndex = 0;
//	std::map<std::int32_t, MediaObjectUndo *> m_Undo;
	if (m_pUndoObj == NULL && count > 0)
	{
		MediaObjectUndo * undoobj = new MediaObjectUndo();
		undoobj->m_count = count;
		m_Undo.insert(std::make_pair(std::int32_t((iUndoIndex++)), undoobj));
		return;
	}
	if(m_SceneEditor)
		m_SceneEditor->filechanged = true;
	if (!m_pUndoObj)
		return;

	MediaObjectUndo * undoobj = new MediaObjectUndo();
	if (remove)
		undoobj->m_bRemoved = remove;
	else
		undoobj->m_bRemoved = m_pUndoObj->m_bRemoved;
	undoobj->m_Color = m_pUndoObj->m_Color;
	undoobj->m_Depth = m_pUndoObj->m_Depth;
	undoobj->m_Layer = m_pUndoObj->m_Layer;
	undoobj->m_pObj = m_pUndoObj;
	undoobj->m_Position = m_pUndoObj->m_Position;
	undoobj->m_Rotation = m_pUndoObj->m_Rotation;
	undoobj->m_Scale = m_pUndoObj->m_Scale;
	undoobj->m_Size = m_pUndoObj->m_Size;
	undoobj->m_count = 0;

	//Delete anything >= iUndoIndex.
	bool is_deleting = true;
	while (is_deleting) {
		is_deleting = false;
		for (std::map<std::int32_t, MediaObjectUndo *>::iterator it = m_Undo.begin(); it != m_Undo.end(); ++it)
		{
			if (it->first >= iUndoIndex ) {
				MediaObjectUndo * mem;
				mem = it->second;
				m_Undo.erase(it->first);
				delete mem;
				is_deleting = true;
				break;
			}
		}
	}

	//TODO: If to large delete oldest.
	//|| m_Undo.size() > 10
	//Also rearrange it->first -- to fit new iUndoIndex

	m_Undo.insert(std::make_pair(std::int32_t((iUndoIndex++)), undoobj));

}

bool MediaScene::CanUndo( void )
{
	if( iUndoIndex > 0)
		return true;
	else
		return false;
}
bool MediaScene::CanRedo(void)
{
	if( (int)m_Undo.size() > iUndoIndex )
		return true;
	else
		return false;
}



bool MediaScene::Delete(void)
{
	if (!m_pSelectedObject)
		return false;
	AddUndo(m_pSelectedObject, false);
	if(m_pSelectedObject->m_iMediaType == MEDIATYPE_SPRITE )
		agk::SetSpriteVisible(m_pSelectedObject->m_iSprite,0);
	if (m_pSelectedObject->m_iMediaType == MEDIATYPE_TEXT)
		agk::SetTextVisible(m_pSelectedObject->m_iSprite, 0);
	if (m_pSelectedObject->m_iMediaType == MEDIATYPE_BUTTON)
		agk::SetVirtualButtonVisible(m_pSelectedObject->m_iSprite, 0);
	if (m_pSelectedObject->m_iMediaType == MEDIATYPE_EDITBOX)
		agk::SetEditBoxVisible(m_pSelectedObject->m_iSprite, 0);

	m_pSelectedObject->m_bRemoved = true;
	m_pSelectedObject = NULL;
	ide_force_rendering_delayed = true;
	return true;
}
bool MediaScene::Undo(void)
{
	if (!CanUndo())
		return false;
	if (m_SceneEditor)
		m_SceneEditor->filechanged = true;

	int repeat = 1;
	for (std::map<std::int32_t, MediaObjectUndo *>::iterator it = m_Undo.begin(); it != m_Undo.end(); ++it)
	{
		if (it->first >= iUndoIndex - 1) {
			MediaObject * undoobj = it->second->m_pObj;
			if (it->second->m_count > 0) {
				repeat = it->second->m_count;
				iUndoIndex--;
			}
			break;
		}
	}

	for (int i = 0; i < repeat; i++) {
		
		for (std::map<std::int32_t, MediaObjectUndo *>::iterator it = m_Undo.begin(); it != m_Undo.end(); ++it)
		{
			if (it->second->m_count > 0) {
//				iUndoIndex--;
			}
			else if (it->first >= iUndoIndex - 1) {

				MediaObjectUndo newredo;
				MediaObject * undoobj = it->second->m_pObj;

				newredo.m_bRemoved = undoobj->m_bRemoved;
				newredo.m_Color = undoobj->m_Color;
				newredo.m_Depth = undoobj->m_Depth;
				newredo.m_Layer = undoobj->m_Layer;
				newredo.m_Position = undoobj->m_Position;
				newredo.m_Rotation = undoobj->m_Rotation;
				newredo.m_Scale = undoobj->m_Scale;
				newredo.m_Size = undoobj->m_Size;

				undoobj->m_bRemoved = it->second->m_bRemoved;
				undoobj->m_Color = it->second->m_Color;
				undoobj->m_Depth = it->second->m_Depth;
				undoobj->m_Layer = it->second->m_Layer;
				undoobj->m_Position = it->second->m_Position;
				undoobj->m_Rotation = it->second->m_Rotation;
				undoobj->m_Scale = it->second->m_Scale;
				undoobj->m_Size = it->second->m_Size;
				if (it->second->m_bRemoved) {
					if (undoobj->m_iMediaType == MEDIATYPE_SPRITE)
						agk::SetSpriteVisible(undoobj->m_iSprite, 0);
					if (undoobj->m_iMediaType == MEDIATYPE_TEXT)
						agk::SetTextVisible(undoobj->m_iSprite, 0);
					if (undoobj->m_iMediaType == MEDIATYPE_BUTTON)
						agk::SetVirtualButtonVisible(undoobj->m_iSprite, 0);
					if (undoobj->m_iMediaType == MEDIATYPE_EDITBOX)
						agk::SetEditBoxVisible(undoobj->m_iSprite, 0);

				}
				else {
					if (undoobj->m_iMediaType == MEDIATYPE_SPRITE)
						agk::SetSpriteVisible(undoobj->m_iSprite, 1);
					if (undoobj->m_iMediaType == MEDIATYPE_TEXT)
						agk::SetTextVisible(undoobj->m_iSprite, 1);
					if (undoobj->m_iMediaType == MEDIATYPE_BUTTON)
						agk::SetVirtualButtonVisible(undoobj->m_iSprite, 1);
					if (undoobj->m_iMediaType == MEDIATYPE_EDITBOX)
						agk::SetEditBoxVisible(undoobj->m_iSprite, 1);
				}


				it->second->m_bRemoved = newredo.m_bRemoved;
				it->second->m_Color = newredo.m_Color;
				it->second->m_Depth = newredo.m_Depth;
				it->second->m_Layer = newredo.m_Layer;
				it->second->m_Position = newredo.m_Position;
				it->second->m_Rotation = newredo.m_Rotation;
				it->second->m_Scale = newredo.m_Scale;
				it->second->m_Size = newredo.m_Size;


				UpdateObject(undoobj);
//				agk::SetSpritePosition(undoobj->m_iSprite, undoobj->m_Position.x, undoobj->m_Position.y);
//				agk::SetSpriteSize(undoobj->m_iSprite, undoobj->m_Size.x, undoobj->m_Size.y);
//				agk::SetSpriteScale(undoobj->m_iSprite, undoobj->m_Scale.x, undoobj->m_Scale.y);
//				agk::SetSpriteAngle(undoobj->m_iSprite, undoobj->m_Rotation.x);

				iUndoIndex--;
				break;
			}
		}
	}
	ide_force_rendering_delayed = true;

	return true;
}


bool MediaScene::Redo(void)
{
	if (!CanRedo())
		return false;
	if (m_SceneEditor)
		m_SceneEditor->filechanged = true;

	for (std::map<std::int32_t, MediaObjectUndo *>::iterator it = m_Undo.begin(); it != m_Undo.end(); ++it)
	{
		if (it->first >= iUndoIndex && it->second->m_count > 0) {
			iUndoIndex++;
			break;
		}
		else if (it->first >= iUndoIndex) {

			MediaObject * undoobj = it->second->m_pObj;
			MediaObjectUndo newredo;

			newredo.m_bRemoved = undoobj->m_bRemoved;
			newredo.m_Color = undoobj->m_Color;
			newredo.m_Depth = undoobj->m_Depth;
			newredo.m_Layer = undoobj->m_Layer;
			newredo.m_Position = undoobj->m_Position;
			newredo.m_Rotation = undoobj->m_Rotation;
			newredo.m_Scale = undoobj->m_Scale;
			newredo.m_Size = undoobj->m_Size;


			//Redo always set not deleted.
			undoobj->m_bRemoved = false; //it->second->m_bRemoved;
			undoobj->m_Color = it->second->m_Color;
			undoobj->m_Depth = it->second->m_Depth;
			undoobj->m_Layer = it->second->m_Layer;
			undoobj->m_Position = it->second->m_Position;
			undoobj->m_Rotation = it->second->m_Rotation;
			undoobj->m_Scale = it->second->m_Scale;
			undoobj->m_Size = it->second->m_Size;


			it->second->m_bRemoved = newredo.m_bRemoved;
			it->second->m_Color = newredo.m_Color;
			it->second->m_Depth = newredo.m_Depth;
			it->second->m_Layer = newredo.m_Layer;
			it->second->m_Position = newredo.m_Position;
			it->second->m_Rotation = newredo.m_Rotation;
			it->second->m_Scale = newredo.m_Scale;
			it->second->m_Size = newredo.m_Size;


			if (undoobj->m_iMediaType == MEDIATYPE_SPRITE) {
				agk::SetSpriteVisible(undoobj->m_iSprite, 1);
				UpdateObject(undoobj);
			}
			if (undoobj->m_iMediaType == MEDIATYPE_TEXT) {
				agk::SetTextVisible(undoobj->m_iSprite, 1);
				UpdateObject(undoobj);
			}
			if (undoobj->m_iMediaType == MEDIATYPE_BUTTON) {
				agk::SetVirtualButtonVisible(undoobj->m_iSprite, 1);
				UpdateObject(undoobj);
			}
			if (undoobj->m_iMediaType == MEDIATYPE_EDITBOX) {
				agk::SetEditBoxVisible(undoobj->m_iSprite, 1);
				UpdateObject(undoobj);
			}

			iUndoIndex++;
			break;
		}
	}
	ide_force_rendering_delayed = true;

	return true;
}


void ProcessSceneManager(MediaScene * m_ActiveScene)
{
	if (!m_ActiveScene)
		return;

	ImGui::Text("Scene: %s, images (%d):", m_ActiveScene->m_sSceneName.GetStr(), (int) (m_ActiveScene->iImageCount-1));
//	ImGui::Text("Images: %d", m_ActiveScene->iImageCount);
//	ImGui::Text("Sprites: %d", m_ActiveScene->iSpriteCount);
//	ImGui::Text("Undo: %d", m_ActiveScene->m_Undo.size());
//	ImGui::Text("iUndoIndex: %d", m_ActiveScene->iUndoIndex);
	
	ImGui::Separator();

	ImGuiWindow *pwin = ImGui::GetCurrentWindow();
	float scroll_bar_x = pwin->ScrollbarSizes.x;
	float scroll_bar_y = pwin->ScrollbarSizes.y;

	ImVec2 ws = ImGui::GetWindowSize();
	//ws.x -= scroll_bar_x;
	ws.x -= 24.0f;

	int columns = (int) ((int)ws.x / (ImGui::GetFontSize()*3.0f) );
	if (columns <= 0)
		columns = 1;
	float icon_size = ws.x / columns;
	icon_size -= 6;
	if (icon_size < 8)
		icon_size = 8;
	ImGui::Columns(columns, "myiconcolumns3", false);  // 3-ways, no border

	MediaScene::Images * m_pSearchImage;
	for (int i = 0; i < 2; i++) {
		ImGui::Columns(columns, "myiconcolumns3", false);  // 3-ways, no border

		m_pSearchImage = m_ActiveScene->m_pFirstImage;
		if (m_pSearchImage) {
			//Display images.
			while (m_pSearchImage) {
				if((i == 1 && m_pSearchImage->m_bAtlas) || (i==0 )) {
					int loopuntil = 1;
					if (i == 1 && m_pSearchImage->m_bAtlas)
						loopuntil = m_pSearchImage->m_iRows * m_pSearchImage->m_iColumns;
					for (int subimage = 0; subimage < loopuntil; subimage++) {

						//float icon_ratio; // = (float)m_pSearchImage->m_iHeight / (float)m_pSearchImage->m_iWidth;
						ImVec2 cp = ImGui::GetCursorPos();

						float icon_ratio;
						float imgw = (float) m_pSearchImage->m_iWidth;
						float imgh = (float) m_pSearchImage->m_iHeight;
						
						if ( (i == 1 && m_pSearchImage->m_bAtlas)) {
							imgw = (float) (m_pSearchImage->m_iWidth / m_pSearchImage->m_iRows);
							imgh = (float) (m_pSearchImage->m_iHeight / m_pSearchImage->m_iColumns);
						}

						if ((icon_size / (float)imgw) < (icon_size / (float)imgh))
							icon_ratio = icon_size / (float)imgw;
						else
							icon_ratio = icon_size / (float)imgh;

						float bw = (imgw)*icon_ratio;
						float bh = (imgh)*icon_ratio;

						if (bh < icon_size) {
							cp.y += ((icon_size*0.45f) - (bh*0.5f));
						}
						if (bw < icon_size)
							cp.x += ((icon_size*0.45f) - (bw*0.5f));

						ImGui::SetCursorPos(cp);
						if(i == 1)
							ImGui::PushID(m_pSearchImage->m_iImageId + (subimage + 1 * 10000));

						if (m_ActiveScene->m_pSelectedImage == m_pSearchImage && (i==0 || m_ActiveScene->m_iSelectedSubImage == subimage+1) && !(i == 0 && m_pSearchImage->m_bAtlas) ) {
							ImVec4 background_color = pref.icon_background_color;
							ImVec4 background = pref.icon_background_color;
							background_color.w = 0.45f;
							background_color.x = 0.75f;
							background_color.y = 0.75f;
							background_color.z = 0.75f;
							background.w = 1.0f;

							bool ibclick = false;
							if (i == 1 && m_pSearchImage->m_bAtlas)
								ibclick = ImGui::ImgBtn(m_pSearchImage->m_iImageId, ImVec2(bw, bh), background, background_color, background_color, background_color, 2, subimage+1, m_pSearchImage->m_iRows, m_pSearchImage->m_iColumns);
							else
								ibclick = ImGui::ImgBtn(m_pSearchImage->m_iImageId, ImVec2(bw, bh), background, background_color, background_color, background_color, 2);
							if(ibclick) {
								if (m_ActiveScene->m_pSelectedImage == m_pSearchImage && (i==0 || m_ActiveScene->m_iSelectedSubImage == subimage+1) ) {
									m_ActiveScene->m_pSelectedImage = NULL;
									m_ActiveScene->m_iSelectedSubImage = 0;
								}
								else {
									if (!(i == 0 && m_pSearchImage->m_bAtlas)) {
										m_ActiveScene->m_pSelectedImage = m_pSearchImage;
										m_ActiveScene->m_iSelectedSubImage = subimage + 1;

										int fw = (int)m_ActiveScene->m_pSelectedImage->m_iWidth / m_ActiveScene->m_pSelectedImage->m_iRows;
										int fh = (int)m_ActiveScene->m_pSelectedImage->m_iHeight / m_ActiveScene->m_pSelectedImage->m_iColumns;
										m_ActiveScene->fDrawModeSizeX = (float) fw;
										m_ActiveScene->fDrawModeSizeY = (float) fh;

										//Remove multiple selections mode.
										MediaScene::MediaObject * m_pSearchObj = NULL;
										m_pSearchObj = m_ActiveScene->m_pFirstObject;
										while (m_pSearchObj) {
											if (!m_pSearchObj->m_bRemoved) {
												m_pSearchObj->bIsSelected = false;
											}
											m_pSearchObj = m_pSearchObj->m_pNext;
										}

									}
								}
							}
						}
						else {
							bool ibclick = false;
							if (i == 1 && m_pSearchImage->m_bAtlas)
								ibclick = ImGui::ImgBtn(m_pSearchImage->m_iImageId, ImVec2(bw, bh), pref.icon_background_color, ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255),2, subimage + 1, m_pSearchImage->m_iRows, m_pSearchImage->m_iColumns);
							else
								ibclick = ImGui::ImgBtn(m_pSearchImage->m_iImageId, ImVec2(bw, bh), pref.icon_background_color);

							if (ibclick) {
								if (m_ActiveScene->m_pSelectedImage == m_pSearchImage && (i == 0 || m_ActiveScene->m_iSelectedSubImage == subimage + 1)) {
									m_ActiveScene->m_pSelectedImage = NULL;
									m_ActiveScene->m_iSelectedSubImage = 0;
								}
								else {
									if (!(i == 0 && m_pSearchImage->m_bAtlas)) {
										m_ActiveScene->m_pSelectedImage = m_pSearchImage;
										m_ActiveScene->m_iSelectedSubImage = subimage + 1;
										int fw = (int)m_ActiveScene->m_pSelectedImage->m_iWidth / m_ActiveScene->m_pSelectedImage->m_iRows;
										int fh = (int)m_ActiveScene->m_pSelectedImage->m_iHeight / m_ActiveScene->m_pSelectedImage->m_iColumns;
										m_ActiveScene->fDrawModeSizeX = (float) fw;
										m_ActiveScene->fDrawModeSizeY = (float) fh;
										//Remove multiple selections mode.
										MediaScene::MediaObject * m_pSearchObj = NULL;
										m_pSearchObj = m_ActiveScene->m_pFirstObject;
										while (m_pSearchObj) {
											if (!m_pSearchObj->m_bRemoved) {
												m_pSearchObj->bIsSelected = false;
											}
											m_pSearchObj = m_pSearchObj->m_pNext;
										}

									}
								}
							}
						}

						if (ImGui::BeginPopupContextItemAGK())
						{

							if (i == 0) {
								if (m_pSearchImage->m_bAtlas) {
									if (ImGui::MenuItem("Remove Sprite Sheet")) {
										m_pSearchImage->m_bAtlas = false;
										agk::SetImageWrapU(m_pSearchImage->m_iImageId, 1);
										agk::SetImageWrapV(m_pSearchImage->m_iImageId, 1);

									}
								}
								else if (ImGui::MenuItem("Setup Sprite Sheet")) {

									m_ActiveScene->m_pSelectedImage = m_pSearchImage;
									m_ActiveScene->m_iSelectedSubImage = subimage;
									m_ActiveScene->bProcessAtlas = true;
								}
							}
							ImGui::EndPopup();
						}
						if (i == 1)
							ImGui::PopID();


						if (ImGui::IsItemHovered()) {

							ImVec2 cursor_pos = ImGui::GetIO().MousePos;
							ImVec2 tooltip_offset(10.0f, ImGui::GetFontSize());
							ImVec2 tooltip_position = cursor_pos;
							tooltip_position.x += tooltip_offset.x;
							tooltip_position.y += tooltip_offset.y;
							ImGui::SetNextWindowPos(tooltip_position);
							ImGui::SetNextWindowContentWidth(204.0f);
							ImGui::BeginTooltip();
							float icon_ratio;
							int icon_size = 200;

							float imgw = (float) m_pSearchImage->m_iWidth;
							float imgh = (float) m_pSearchImage->m_iHeight;

							if ((i == 1 && m_pSearchImage->m_bAtlas)) {
								imgw = (float) (m_pSearchImage->m_iWidth / m_pSearchImage->m_iRows);
								imgh = (float) (m_pSearchImage->m_iHeight / m_pSearchImage->m_iColumns);
							}


							if ((icon_size / (float)imgw) < (icon_size / (float)imgh))
								icon_ratio = icon_size / (float)imgw;
							else
								icon_ratio = icon_size / (float)imgh;
							float bw = imgw*icon_ratio;
							float bh = imgh*icon_ratio;
							ImVec2 cp = ImGui::GetCursorPos();
//							if (bh < icon_size) {
//								cp.y += ((icon_size*0.5f) - (bh*0.5f));
//							}
							if (bw < icon_size)
								cp.x += ((icon_size*0.5f) - (bw*0.5f));

							ImGui::SetCursorPos(cp);

							if (i == 1 && m_pSearchImage->m_bAtlas)
								ImGui::ImgBtn(m_pSearchImage->m_iImageId, ImVec2(bw, bh), pref.icon_background_color, ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), 2, subimage + 1, m_pSearchImage->m_iRows, m_pSearchImage->m_iColumns);
							else
								ImGui::ImgBtn(m_pSearchImage->m_iImageId, ImVec2(bw, bh), pref.icon_background_color);

							char hchar[MAX_PATH];

							if (i == 1 && m_pSearchImage->m_bAtlas) {
								int fw = (int)m_pSearchImage->m_iWidth / m_pSearchImage->m_iRows;
								int fh = (int)m_pSearchImage->m_iHeight / m_pSearchImage->m_iColumns;
								sprintf(hchar, "%s (%d,%d)", m_pSearchImage->m_sName.GetStr(), fw, fh);
							}
							else {
								sprintf(hchar, "%s (%d,%d)", m_pSearchImage->m_sName.GetStr(), m_pSearchImage->m_iWidth, m_pSearchImage->m_iHeight);
							}
							int iTextWidth = (int) ImGui::CalcTextSize(hchar).x;
							if (iTextWidth > icon_size) {
								ImGui::Text("%s", hchar);
							}
							else {
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((icon_size - iTextWidth) * 0.5f), ImGui::GetCursorPos().y));
								ImGui::Text("%s", hchar);
							}

							//ImGui::Text("%s (%d,%d)", m_pSearchImage->m_sName.GetStr(), m_pSearchImage->m_iWidth, m_pSearchImage->m_iHeight);
							ImGui::EndTooltip();

						}

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							//Fake drag drop source.
							static cFolderItem::sFolderFiles DragDropFile;
							uString tmp = m_pSearchImage->m_sFullPath;
							uString rep = "/";
							rep.Append(m_pSearchImage->m_sName);
							tmp.ReplaceStr(rep.GetStr(), "");

							DragDropFile.m_sPath = tmp;

							DragDropFile.m_sFolder = m_ActiveScene->WindowName;
							DragDropFile.m_dropptr = &DragDropFile;
							DragDropFile.m_sName = m_pSearchImage->m_sName;
							if (i == 1 && m_pSearchImage->m_bAtlas)
								DragDropFile.iAnimationFrom = subimage + 1;
							else
								DragDropFile.iAnimationFrom = 0;

							extern cFolderItem::sFolderFiles *pDragDropFile;
							ImGui::SetDragDropPayload("DND_MODEL_DROP_TARGET", &DragDropFile, sizeof(void *));
							ImGui::Text("%s", m_pSearchImage->m_sName.GetStr());
							pDragDropFile = &DragDropFile;
							ImGui::EndDragDropSource();
						}

						ImGui::NextColumn();
					}
				}
				m_pSearchImage = m_pSearchImage->m_pNext;
			}
		}
		ImGui::Columns(1);
	}


	if (m_ActiveScene->Fonts.size() > 0) {
		ImGui::Separator();
		ImGui::Text("Fonts (%d):",(int)m_ActiveScene->Fonts.size());
		for (std::map<std::int32_t, std::string>::iterator it = m_ActiveScene->Fonts.begin(); it != m_ActiveScene->Fonts.end(); ++it)
		{
			uString FontPath = it->second.c_str();
			uString FontName = "na";
			int pos = FontPath.RevFindStr("\\");
			if(pos < 1) pos = FontPath.RevFindStr("/");
			if (pos > 0) {
				FontPath.SubString(FontName, pos+1);
			}

			float fs = ImGui::GetFontSize();
			if (m_ActiveScene->customfonts[it->first] && m_ActiveScene->customfonts[it->first]->FontSize > 0.0f) {
				if (!m_ActiveScene->customfonts[it->first]->IsLoaded())
					ImGui::PushFont(defaultfont);  //defaultfont
				else
					ImGui::PushFont(m_ActiveScene->customfonts[it->first]);
			}

			float scale = (1.0f/(64.0f-fs) )*20.0f;
//			if(ImGui::GetFontSize() < 20)
//				ImGui::SetWindowFontScale(0.25f);
			ImGui::SetWindowFontScale(scale);
			ImGui::Text(FontName.GetStr());
			ImGui::SetWindowFontScale(1.0f);

			if (m_ActiveScene->customfonts[it->first] && m_ActiveScene->customfonts[it->first]->FontSize > 0.0f) {
				ImGui::PopFont();
			}
		}
	}

	//Display active sprites.
	ImGui::Separator();

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
	int unique_id = 4900;

	bool TreeNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)unique_id++, node_flags, ICON_MD_FOLDER " Scene Media (%d)", m_ActiveScene->iActiveSpriteCount);

	if (TreeNodeOpen) {

		ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::Columns(2, "myspritecolumns2", false);  // 3-ways, no border

		MediaScene::MediaObject * m_pSearchObj = NULL;
		m_pSearchObj = m_ActiveScene->m_pFirstObject;
		if (m_pSearchObj) {
			while (m_pSearchObj) {
				if (!m_pSearchObj->m_bRemoved) {
					//Display.

					int leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnDoubleClick;
					if (m_ActiveScene->m_pSelectedObject == m_pSearchObj)
						leafFlags |= ImGuiTreeNodeFlags_Selected; //ImGuiTreeNodeFlags_Bullet;

					if (ImGui::TreeNodeEx((void*)(intptr_t)unique_id++, leafFlags, ICON_MD_PERM_MEDIA " %s", m_pSearchObj->m_sName.GetStr())) { // ICON_MD_FORMAT_COLOR_TEXT // ICON_MD_CODE // ICON_MD_EDIT // ICON_MD_APPS
						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
							//change selection
							if (m_ActiveScene->m_pSelectedObject != m_pSearchObj) {
								m_ActiveScene->m_pSelectedObject = m_pSearchObj;
								if (!m_ActiveScene->bFirstSelection) {
									m_ActiveScene->bFirstSelection = true;
									ImGui::SetWindowFocus("Properties");
								}

							}
						}
						ImGui::TreePop();
					}

					ImGui::NextColumn();
				}
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}


		ImGui::Columns(1);
		ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::TreePop();

	}


	ImGui::Separator();

	if (ImGui::Button("Add Text")) {
		//Add

		MediaScene::MediaObject * m_pSearchObj = NULL;
		MediaScene::MediaObject * m_pLastObj = NULL;
		m_pSearchObj = m_ActiveScene->m_pFirstObject;
		if (m_pSearchObj) {
			while (m_pSearchObj) {
				m_pLastObj = m_pSearchObj;
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}

		MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
		Obj->m_bRemoved = false;
		Obj->m_bVisible = true;
		Obj->m_Color = { 255,255,255,255 };
		Obj->m_Depth = 9; // default 9 infront
		Obj->m_iImageIndex = 0;
		Obj->m_iImageId = 0;
		Obj->m_iImageDownIndex = 0;
		Obj->m_iImageDownId = 0;
		Obj->m_iMediaType = MEDIATYPE_TEXT;
		Obj->m_iSprite = agk::CreateText("Hello AppGameKit");
		Obj->m_Text = "Hello AppGameKit";
		Obj->m_Layer = 1;
		Obj->bAnimated = false;
		Obj->bFixToScreen = false;
		Obj->bStretchToDevice = false;
		Obj->bLocked = false;
		Obj->bOnlyVisibleInAGK = false;
		Obj->m_pUsedImage = NULL;
		Obj->m_pUsedDownImage = NULL;

		Obj->iAnimationFrom = 0;
		Obj->iAnimationTo = 0;

		Obj->m_pNext = NULL;
		Obj->m_Position = { 0.0f,0.0f,0.0f,0.0f };
		agk::SetTextPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
		Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
		Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };

		Obj->m_Size = { 200.0f, 40.0f ,0.0f,0.0f };
		Obj->m_Size.x = agk::GetTextTotalWidth(Obj->m_iSprite);

		Obj->m_sMediaFullPath = "";
		Obj->m_sMediaRelativePath = "";

		Obj->m_iFontId = 0;
		Obj->m_iLoadFontId = 0;


		bool found = true;
		while (found) {
			found = false;
			uString sName = "Text ";
			sName.AppendInt(m_ActiveScene->iTextCount++);
			Obj->m_sName = sName;

			m_pSearchObj = m_ActiveScene->m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if (m_pSearchObj->m_sName.CompareTo(sName) == 0) {
						found = true;
						break;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}
		}

		Obj->m_sVariable = "";

		if (!m_pLastObj) {
			//No image found, set first entry.
			m_ActiveScene->m_pFirstObject = Obj;
		}
		else {
			//Add the new image to the chain.
			m_pLastObj->m_pNext = Obj;
		}
		m_ActiveScene->UpdateObject(Obj);
		m_ActiveScene->AddUndo(Obj, true);
		if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
			m_ActiveScene->m_SceneEditor->filechanged = true;
	}

	ImGui::SameLine();

	if (ImGui::Button("Add Virtual Button")) {
		//Add button

		MediaScene::MediaObject * m_pSearchObj = NULL;
		MediaScene::MediaObject * m_pLastObj = NULL;
		m_pSearchObj = m_ActiveScene->m_pFirstObject;
		if (m_pSearchObj) {
			while (m_pSearchObj) {
				m_pLastObj = m_pSearchObj;
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}

		MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
		Obj->m_bRemoved = false;
		Obj->m_bVisible = true;
		Obj->m_Color = { 255,255,255,255 };
		Obj->m_Depth = 9; // default 9 infront
		Obj->m_iImageIndex = 0;
		Obj->m_iImageId = agkbutton_image;
		Obj->m_iImageDownIndex = 0;
		Obj->m_iImageDownId = agkbutton_image;
		Obj->m_iMediaType = MEDIATYPE_BUTTON;
		Obj->m_iSprite = m_ActiveScene->iButtonCount;
		agk::AddVirtualButton(m_ActiveScene->iButtonCount,0,0,80);
		Obj->m_Text = "Click";
		agk::SetVirtualButtonText(m_ActiveScene->iButtonCount, "Click");
		Obj->m_Layer = 1;
		Obj->bAnimated = false;
		Obj->bFixToScreen = false;
		Obj->bStretchToDevice = false;
		Obj->bLocked = false;
		Obj->bOnlyVisibleInAGK = false;
		Obj->m_pUsedImage = NULL;
		Obj->m_pUsedDownImage = NULL;

		Obj->iAnimationFrom = 0;
		Obj->iAnimationTo = 0;

		Obj->m_pNext = NULL;
		Obj->m_Position = { 0.0f,0.0f,0.0f,0.0f };
		agk::SetVirtualButtonPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
		Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
		Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };

		Obj->m_Size = { 80.0f, 80.0f ,0.0f,0.0f };

		Obj->m_sMediaFullPath = "";
		Obj->m_sMediaRelativePath = "";

		Obj->m_iFontId = 0;
		Obj->m_iLoadFontId = 0;

		bool found = true;
		while (found) {
			found = false;
			uString sName = "Button ";
			sName.AppendInt(m_ActiveScene->iButtonCount++);
			Obj->m_sName = sName;

			m_pSearchObj = m_ActiveScene->m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if (m_pSearchObj->m_sName.CompareTo(sName) == 0) {
						found = true;
						break;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}
		}

		Obj->m_sVariable = "";

		if (!m_pLastObj) {
			//No image found, set first entry.
			m_ActiveScene->m_pFirstObject = Obj;
		}
		else {
			//Add the new image to the chain.
			m_pLastObj->m_pNext = Obj;
		}
		m_ActiveScene->UpdateObject(Obj);
		m_ActiveScene->AddUndo(Obj, true);

		if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
			m_ActiveScene->m_SceneEditor->filechanged = true;

	}

//	ImGui::SameLine();

	if (ImGui::Button("Add Edit Box")) {
		//Add button

		MediaScene::MediaObject * m_pSearchObj = NULL;
		MediaScene::MediaObject * m_pLastObj = NULL;
		m_pSearchObj = m_ActiveScene->m_pFirstObject;
		if (m_pSearchObj) {
			while (m_pSearchObj) {
				m_pLastObj = m_pSearchObj;
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}

		MediaScene::MediaObject * Obj = new MediaScene::MediaObject();
		Obj->m_bRemoved = false;
		Obj->m_bVisible = true;
		Obj->m_Color = { 255,255,255,255 };
		Obj->m_TextColor = { 0,0,0,255 };
		Obj->m_iBorderSize = 2;
		Obj->m_iCursorSize = 2;
		Obj->m_iTextSize = 0;
		Obj->m_iEditMaxChars = 0;
		Obj->bEditMultiLines = false;
		Obj->bEditPassword = false;
		Obj->bEditWrap = false;
		Obj->m_BorderColor = { 0,0,0,255 };
		Obj->m_CursorColor = { 0,0,0,255 };
		Obj->m_Depth = 9; // default 9 infront
		Obj->m_iImageIndex = 0;
		Obj->m_iImageId = 0;
		Obj->m_iImageDownIndex = 0;
		Obj->m_iImageDownId = 0;
		Obj->m_iMediaType = MEDIATYPE_EDITBOX;
		Obj->m_iSprite = agk::CreateEditBox();
		Obj->m_Text = "Edit Box";
		agk::SetEditBoxText(Obj->m_iSprite, "Edit Box");
		Obj->m_Layer = 1;
		Obj->bAnimated = false;
		Obj->bFixToScreen = false;
		Obj->bStretchToDevice = false;
		Obj->bLocked = false;
		Obj->bOnlyVisibleInAGK = false;
		Obj->m_pUsedImage = NULL;
		Obj->m_pUsedDownImage = NULL;

		Obj->iAnimationFrom = 0;
		Obj->iAnimationTo = 0;

		Obj->m_pNext = NULL;
		Obj->m_Position = { 0.0f,0.0f,0.0f,0.0f };
		agk::SetEditBoxPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
		Obj->m_Rotation = { 0.0f,0.0f,0.0f,0.0f };
		Obj->m_Scale = { 1.0f,1.0f,1.0f,1.0f };

		Obj->m_Size = { 200.0f, 40.0f ,0.0f,0.0f };
		if (m_ActiveScene->bUsePercentage) {
			agk::SetEditBoxSize(Obj->m_iSprite, (Obj->m_Size.x / (float)m_ActiveScene->iBaseWidth)*100.0f, (Obj->m_Size.y / (float)m_ActiveScene->iBaseHeight)*100.0f);
		}
		else {
			agk::SetEditBoxSize(Obj->m_iSprite, Obj->m_Size.x, Obj->m_Size.y);
		}
		Obj->m_sMediaFullPath = "";
		Obj->m_sMediaRelativePath = "";

		Obj->m_iFontId = 0;
		Obj->m_iLoadFontId = 0;

		bool found = true;
		while (found) {
			found = false;
			uString sName = "Edit Box ";
			sName.AppendInt(m_ActiveScene->iEditBoxCount++);
			Obj->m_sName = sName;

			m_pSearchObj = m_ActiveScene->m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if (m_pSearchObj->m_sName.CompareTo(sName) == 0) {
						found = true;
						break;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}
		}

		Obj->m_sVariable = "";

		if (!m_pLastObj) {
			//No image found, set first entry.
			m_ActiveScene->m_pFirstObject = Obj;
		}
		else {
			//Add the new image to the chain.
			m_pLastObj->m_pNext = Obj;
		}
		m_ActiveScene->UpdateObject(Obj);
		m_ActiveScene->AddUndo(Obj, true);

		if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
			m_ActiveScene->m_SceneEditor->filechanged = true;

	}

//	ImGui::SameLine();
//	//iFontCount
//	if (ImGui::Button("Add Font")) {
//		if (m_ActiveScene->iFontCount < MAXUSERFONTS) {
//			//Dir or remove and only drag drop.
//			m_ActiveScene->Fonts.insert(std::make_pair(std::int32_t(m_ActiveScene->iFontCount++), std::string("D:/AGK2/deleteme2/media/crackman.ttf")));
//		}
//	}



	ImGui::Separator();
	if (ImGui::Checkbox("Percentage system", &m_ActiveScene->bUsePercentage)) {

		if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
			m_ActiveScene->m_SceneEditor->filechanged = true;
		m_ActiveScene->bTestMode = false;
		//Text cant change from using percentge to pixel mode without problems.

		//Obj->m_iSprite = agk::CreateText("Hello AppGameKit");
		MediaScene::MediaObject * m_pSearchObj = NULL;
		m_pSearchObj = m_ActiveScene->m_pFirstObject;
		if (m_pSearchObj) {
			while (m_pSearchObj) {
				if (!m_pSearchObj->m_bRemoved) {
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT) {
						//Recreate object.
						agk::DeleteText(m_pSearchObj->m_iSprite);
						m_pSearchObj->m_iSprite = agk::CreateText(m_pSearchObj->m_Text.GetStr());
					}
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {
						agk::DeleteVirtualButton(m_pSearchObj->m_iSprite);
						agk::AddVirtualButton(m_pSearchObj->m_iSprite, 0, 0, 80);
						agk::SetVirtualButtonText(m_pSearchObj->m_iSprite, m_pSearchObj->m_Text.GetStr());
						if (m_pSearchObj->m_iImageIndex > 0)
							agk::SetVirtualButtonImageUp(m_pSearchObj->m_iSprite, m_pSearchObj->m_iImageId);
						else
							agk::SetVirtualButtonImageUp(m_pSearchObj->m_iSprite, 0);
						if (m_pSearchObj->m_iImageDownIndex > 0)
							agk::SetVirtualButtonImageDown(m_pSearchObj->m_iSprite, m_pSearchObj->m_iImageDownId);
						else
							agk::SetVirtualButtonImageDown(m_pSearchObj->m_iSprite, 0);

					}
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
						agk::DeleteEditBox(m_pSearchObj->m_iSprite);
						m_pSearchObj->m_iSprite = agk::CreateEditBox();
						agk::SetEditBoxText(m_pSearchObj->m_iSprite, m_pSearchObj->m_Text.GetStr());
					}
				}
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}

	}

	//Getting to cluttered, moved to scene manager.

	if (ImGui::Checkbox("Debug Physics in Testmode", &m_ActiveScene->bPhysicsDebug)) {
		if (m_ActiveScene->bPhysicsDebug)
			agk::SetPhysicsDebugOn();
		else
			agk::SetPhysicsDebugOff();
	}

	float itemwidth = ImGui::GetWindowSize().x*0.5f; //(ImGui::GetFontSize() * 5);
	if (itemwidth < (ImGui::GetFontSize() * 5))
		itemwidth = (ImGui::GetFontSize() * 5);
	ImGui::PushItemWidth(itemwidth);
	if (ImGui::SliderInt("Physics Polygon Points", &m_ActiveScene->iPhysicsMaxPolygonPoints, 2, 12)) {
		agk::SetPhysicsMaxPolygonPoints(m_ActiveScene->iPhysicsMaxPolygonPoints);
	}

	int decimalprecision = 2;
	if (pref.iSceneLowFloatPrecision >= 0)
		decimalprecision = pref.iSceneLowFloatPrecision;

	if (m_ActiveScene->bUsePercentage) {
		//Size
		float percentx = (m_ActiveScene->fPhysicsGravityX / (float)m_ActiveScene->iBaseWidth)*100.0f;
		float percenty = (m_ActiveScene->fPhysicsGravityY / (float)m_ActiveScene->iBaseHeight)*100.0f;
		if (ImGui::InputFloat("Scene Gravity X", &percentx, 1.0f, 10.0f, decimalprecision)) {
			if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
				m_ActiveScene->m_SceneEditor->filechanged = true;
			m_ActiveScene->fPhysicsGravityX = (m_ActiveScene->iBaseWidth / 100.0f) * percentx;
			agk::SetPhysicsGravity((m_ActiveScene->fPhysicsGravityX / (float)m_ActiveScene->iBaseWidth)*100.0f, (m_ActiveScene->fPhysicsGravityY / (float)m_ActiveScene->iBaseHeight)*100.0f);
		}
		if (ImGui::InputFloat("Scene Gravity Y", &percenty, 1.0f, 10.0f, decimalprecision)) {
			if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
				m_ActiveScene->m_SceneEditor->filechanged = true;
			m_ActiveScene->fPhysicsGravityY = (m_ActiveScene->iBaseHeight / 100.0f) * percenty;
			agk::SetPhysicsGravity((m_ActiveScene->fPhysicsGravityX / (float)m_ActiveScene->iBaseWidth)*100.0f, (m_ActiveScene->fPhysicsGravityY / (float)m_ActiveScene->iBaseHeight)*100.0f);
		}
	}
	else {
		if (ImGui::InputFloat("Scene Gravity X", &m_ActiveScene->fPhysicsGravityX, 1.0f, 10.0f, decimalprecision)) {
			if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
				m_ActiveScene->m_SceneEditor->filechanged = true;
			agk::SetPhysicsGravity(m_ActiveScene->fPhysicsGravityX, m_ActiveScene->fPhysicsGravityY);
		}
		if (ImGui::InputFloat("Scene Gravity Y", &m_ActiveScene->fPhysicsGravityY, 1.0f, 10.0f, decimalprecision)) {
			if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
				m_ActiveScene->m_SceneEditor->filechanged = true;
			agk::SetPhysicsGravity(m_ActiveScene->fPhysicsGravityX, m_ActiveScene->fPhysicsGravityY);
		}
	}
	if (ImGui::Checkbox("Scene Physics Wall Left", &m_ActiveScene->bPhysicsWallLeft)) {
		if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
			m_ActiveScene->m_SceneEditor->filechanged = true;
		if (m_ActiveScene->bPhysicsWallLeft)
			agk::SetPhysicsWallLeft(1);
		else
			agk::SetPhysicsWallLeft(0);
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Right", &m_ActiveScene->bPhysicsWallRight)) {
		if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
			m_ActiveScene->m_SceneEditor->filechanged = true;
		if (m_ActiveScene->bPhysicsWallRight)
			agk::SetPhysicsWallRight(1);
		else
			agk::SetPhysicsWallRight(0);
	}

	if (ImGui::Checkbox("Scene Physics Wall Top", &m_ActiveScene->bPhysicsWallTop)) {
		if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
			m_ActiveScene->m_SceneEditor->filechanged = true;
		if (m_ActiveScene->bPhysicsWallTop)
			agk::SetPhysicsWallTop(1);
		else
			agk::SetPhysicsWallTop(0);
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Bottom", &m_ActiveScene->bPhysicsWallBottom)) {
		if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
			m_ActiveScene->m_SceneEditor->filechanged = true;
		if (m_ActiveScene->bPhysicsWallBottom)
			agk::SetPhysicsWallBottom(1);
		else
			agk::SetPhysicsWallBottom(0);
	}
	ImGui::PopItemWidth();



}

void ProcessSceneProperties(MediaScene * m_ActiveScene)
{
	if (!m_ActiveScene)
		return;
	MediaScene * w = m_ActiveScene;
	MediaScene::MediaObject * o = NULL;

	int decimalprecision = 2;
	float fsize = ImGui::GetFontSize();
	if (pref.iSceneLowFloatPrecision >= 0)
		decimalprecision = pref.iSceneLowFloatPrecision;


	/* moved to scene manager
	if (ImGui::Checkbox("Percentage system", &w->bUsePercentage)) {

		w->bTestMode = false;
		//Text cant change from using percentge to pixel mode without problems.

		//Obj->m_iSprite = agk::CreateText("Hello AppGameKit");
		MediaScene::MediaObject * m_pSearchObj = NULL;
		m_pSearchObj = m_ActiveScene->m_pFirstObject;
		if (m_pSearchObj) {
			while (m_pSearchObj) {
				if (!m_pSearchObj->m_bRemoved) {
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT) {
						//Recreate object.
						agk::DeleteText(m_pSearchObj->m_iSprite);
						m_pSearchObj->m_iSprite = agk::CreateText(m_pSearchObj->m_Text.GetStr());
					}
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON) {
						agk::DeleteVirtualButton(m_pSearchObj->m_iSprite);
						agk::AddVirtualButton(m_pSearchObj->m_iSprite, 0, 0, 80);
						agk::SetVirtualButtonText(m_pSearchObj->m_iSprite, m_pSearchObj->m_Text.GetStr());
					}
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX) {
						agk::DeleteEditBox(m_pSearchObj->m_iSprite);
						m_pSearchObj->m_iSprite = agk::CreateEditBox();
						agk::SetEditBoxText(m_pSearchObj->m_iSprite, m_pSearchObj->m_Text.GetStr());
					}
				}
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}

	}
	*/
	

	if (w->mMultipleSelectActive) {

		ImGui::Text("Multiple Selection Mode:");
		ImGui::Text("Selected Sprites: %d", w->iActiveSelections);
		ImGui::Separator();

		ImGui::Checkbox("Snap To Grid", &w->bSnapToGrid);

		ImGui::InputFloat("Grid X##dm", &w->fGridX, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Value");
		ImGui::InputFloat("Grid Y##dm", &w->fGridY, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Y Value");

		ImGui::InputFloat("Grid X Offset", &m_ActiveScene->fGridXOffset, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Offset Value");

		ImGui::InputFloat("Grid Y Offset", &m_ActiveScene->fGridYOffset, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Offset Value");

		if (w->fGridX < 4.0f)
			w->fGridX = 4.0f;
		if (w->fGridY < 4.0f)
			w->fGridY = 4.0f;


		ImGui::Separator();
/*
		ImVec2 diff;
		diff = w->mmouseendpos - w->mmousestartpos;
		diff.x /= w->fRatio;
		diff.y /= w->fRatio;
		diff.x *= w->fScaleToScreenX;
		diff.y *= w->fScaleToScreenY;

		ImGui::InputFloat("All Sprites X Move Offset", &m_ActiveScene->fGridYOffset, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Offset Value");
*/



		w->newColor.x = w->newColor.x / 255.0f;
		w->newColor.y = w->newColor.y / 255.0f;
		w->newColor.z = w->newColor.z / 255.0f;
		w->newColor.w = w->newColor.w / 255.0f;
		bool c_change = ImGui::ColorEdit4("Change All Sprite Colors##media_m_Color", (float*)&w->newColor, ImGuiColorEditFlags_AlphaPreview);
		w->newColor.x *= 255.0f;
		w->newColor.y *= 255.0f;
		w->newColor.z *= 255.0f;
		w->newColor.w *= 255.0f;
		if (c_change) {

			MediaScene::MediaObject * m_pSearchObj = NULL;
			m_pSearchObj = w->m_pFirstObject;
			while (m_pSearchObj) {
				if (m_pSearchObj->bIsSelected) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					w->bColorChanged = true;
					m_pSearchObj->m_Color = w->newColor;
					agk::SetSpriteColor(m_pSearchObj->m_iSprite, (int) w->newColor.x, (int) w->newColor.y, (int) w->newColor.z, (int) w->newColor.w);
				}
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}


		if (ImGui::InputText("Array Group##all", &newgrouparray[0], MAX_PATH, 0, TextFilters::TextFiltersVariable)) { //ImGuiInputTextFlags_EnterReturnsTrue
			uString ag;
			ag = newgrouparray;
			if (ag.GetLength() > 0) {
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;
				uString uSceneVar = w->m_sSceneName;
				uSceneVar.Append("_");
				ag.ReplaceStr(uSceneVar, "");
				uString nag = uSceneVar;
				nag.Append(ag);
				strcpy(newgrouparray, nag.GetStr());
			}
		}
		if (ImGui::Button("Set Array Group on All Selected Sprites")) {
			if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
				m_ActiveScene->m_SceneEditor->filechanged = true;
			uString ag = newgrouparray;
			uString uSceneVar = w->m_sSceneName;
			if (ag.GetLength() > uSceneVar.GetLength()) {
				//
				MediaScene::MediaObject * m_pSearchObj = NULL;
				m_pSearchObj = w->m_pFirstObject;

				while (m_pSearchObj) {
					if (m_pSearchObj->bIsSelected) {
						//No undo on Array Group.
						//w->AddUndo(m_pSearchObj, false);
						m_pSearchObj->m_sArrayGroup = newgrouparray;
						ide_force_rendering_delayed = true;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}

			}

		}


		if (ImGui::Button("Delete All Selected Sprites")) {
			//
			MediaScene::MediaObject * m_pSearchObj = NULL;
			m_pSearchObj = w->m_pFirstObject;

			while (m_pSearchObj) {
				if (m_pSearchObj->bIsSelected) {
					//m_pSearchObj->m_OldPosition = m_pSearchObj->m_Position;
					w->AddUndo(m_pSearchObj, false);
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_SPRITE)
						agk::SetSpriteVisible(m_pSearchObj->m_iSprite, 0);
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_TEXT)
						agk::SetTextVisible(m_pSearchObj->m_iSprite, 0);
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_BUTTON)
						agk::SetVirtualButtonVisible(m_pSearchObj->m_iSprite, 0);
					if (m_pSearchObj->m_iMediaType == MEDIATYPE_EDITBOX)
						agk::SetEditBoxVisible(m_pSearchObj->m_iSprite, 0);
					m_pSearchObj->m_bRemoved = true;
					ide_force_rendering_delayed = true;
				}
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}

		if (ImGui::Button("Duplicate All Selected Sprites")) {
			//
			MediaScene::MediaObject * m_pSearchObj = NULL;

			//Only process one time. not on new created objects.
			m_pSearchObj = w->m_pFirstObject;
			while (m_pSearchObj) {
				m_pSearchObj->bNotProcessed = true;
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
			m_pSearchObj = w->m_pFirstObject;
			while (m_pSearchObj) {
				if (m_pSearchObj->bNotProcessed && m_pSearchObj->bIsSelected && !m_pSearchObj->m_bRemoved ) {
					MediaScene::MediaObject * newobj;
					w->CopyObject(m_pSearchObj);
					newobj = w->PasteObject();
					m_pSearchObj->bIsSelected = false;
					if (newobj)
						newobj->bIsSelected = true;

					w->m_pSelectedObject = NULL;
					ide_force_rendering_delayed = true;
				}
				m_pSearchObj->bNotProcessed = false;
				m_pSearchObj = m_pSearchObj->m_pNext;
			}
		}

		ImGui::Separator();
		ImGui::Text("Warning: if all your selected sprites are not grid aligned,");
		ImGui::Text("you should disable 'Snap To Grid'.");

	}
	else if (w->m_pSelectedImage) {
		if (!w->bFirstSelection) {
			w->bFirstSelection = true;
			ImGui::SetWindowFocus("Properties");
		}

		ImGui::Text("Draw Mode:");
		ImGui::Separator();

		//Layers.
		// 0 , 3 , 5 , 7 , 10

		bool bSelected = false;
		int layer = 0;
		uString uName;
		for (int i = 0; i < 11; i++) {
			if (i == 0 || i == 3 || i == 5 || i == 7 || i == 10) {
				layer = i;
				bSelected = false;
				if (layer == w->iCurrentSelectedLayer)
					bSelected = true;
				ImVec2 cp = ImGui::GetCursorPos();

				uName = "##Layerselect";
				uName.AppendInt(layer);
				if (ImGui::Selectable(uName.GetStr(), bSelected)) {
					w->iCurrentSelectedLayer = layer;
				}
				cp.y -= fsize*0.15f;
				ImGui::SetCursorPos(cp);
				ImGui::SetItemAllowOverlap();
				uName = "##LayerVisible";
				uName.AppendInt(layer);
				ImGui::Indent(fsize*0.5f);
				ImGui::ToggleButton(uName.GetStr(), &w->bLayerVisible[layer]);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Layer Visible");
				ImGui::SameLine();
				ImGui::PushItemWidth(fsize*6.0f);
				ImGui::SetItemAllowOverlap();
				uName = "##LayerOpacity";
				uName.AppendInt(layer);
				ImGui::SliderInt(uName.GetStr(), &w->iLayerOpacity[layer], 0, 255);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Layer Opacity");
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::Text("Layer: %s (%d)", w->sLayerName[layer].GetStr(), w->iLayerDepth[layer]);
				ImGui::Indent(-(fsize*0.5f));
			}
		}

		ImGui::Separator();

		ImGui::Checkbox("Snap To Grid",&w->bSnapToGrid);
		
		ImGui::InputFloat("Grid X##dm", &w->fGridX, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Value");
		ImGui::InputFloat("Grid Y##dm", &w->fGridY, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Y Value");

		ImGui::InputFloat("Grid X Offset", &m_ActiveScene->fGridXOffset, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Offset Value");

		ImGui::InputFloat("Grid Y Offset", &m_ActiveScene->fGridYOffset, 0.0f, 0.0f, decimalprecision);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Offset Value");

		if (w->fGridX < 4.0f)
			w->fGridX = 4.0f;
		if (w->fGridY < 4.0f)
			w->fGridY = 4.0f;

		ImGui::Checkbox("Fit Sprite Size to Grid##dm", &w->bDrawModeFitGrid);
		if (!w->bDrawModeFitGrid) {
			ImGui::InputFloat("Draw Size X##dm", &w->fDrawModeSizeX, 0.0f, 0.0f, decimalprecision);
			ImGui::InputFloat("Draw Size Y##dm", &w->fDrawModeSizeY, 0.0f, 0.0f, decimalprecision);
			
		}
		ImGui::Separator();
		if (ImGui::Button("Exit Draw Mode")) {
			w->m_pSelectedImage = NULL;
		}
		ImGui::Separator();
		ImGui::Text("Use Left Mouse Button to Draw Sprites.");
		ImGui::Text("Use Right Mouse Button to Delete Sprites.");
		ImGui::Text("Press ESC to exit Draw Mode.");
	}
	else
	{

		//	m_PropertiesUndo
		bool unselected_object = false;
		if (!m_ActiveScene->m_pSelectedObject)
		{
			if (m_ActiveScene->m_pPropertiesObject) {
				o = m_ActiveScene->m_pPropertiesObject;
				unselected_object = true;
			}
		}
		else
			o = m_ActiveScene->m_pSelectedObject;

		if ((o && o != m_ActiveScene->m_pPropertiesObject) || unselected_object ) { 
			//Add undo.
			//o->m_PropertiesUndo

			if ((w->m_PropertiesUndo.m_pObj && w->m_PropertiesUndo.m_pObj->bUndoAdded &&
				m_ActiveScene->m_pPropertiesObject && (
				m_ActiveScene->m_pPropertiesObject->m_bRemoved != w->m_PropertiesUndo.m_bRemoved ||
				m_ActiveScene->m_pPropertiesObject->m_Color.x != w->m_PropertiesUndo.m_Color.x ||
				m_ActiveScene->m_pPropertiesObject->m_Color.y != w->m_PropertiesUndo.m_Color.y ||
				m_ActiveScene->m_pPropertiesObject->m_Color.z != w->m_PropertiesUndo.m_Color.z ||
				m_ActiveScene->m_pPropertiesObject->m_Color.w != w->m_PropertiesUndo.m_Color.w ||

				m_ActiveScene->m_pPropertiesObject->m_Depth != w->m_PropertiesUndo.m_Depth ||
				m_ActiveScene->m_pPropertiesObject->m_Layer != w->m_PropertiesUndo.m_Layer ||

				m_ActiveScene->m_pPropertiesObject->m_Position.x != w->m_PropertiesUndo.m_Position.x ||
				m_ActiveScene->m_pPropertiesObject->m_Position.y != w->m_PropertiesUndo.m_Position.y ||

				m_ActiveScene->m_pPropertiesObject->m_Rotation.x != w->m_PropertiesUndo.m_Rotation.x ||
				m_ActiveScene->m_pPropertiesObject->m_Rotation.y != w->m_PropertiesUndo.m_Rotation.y ||

				m_ActiveScene->m_pPropertiesObject->m_Scale.x != w->m_PropertiesUndo.m_Scale.x ||
				m_ActiveScene->m_pPropertiesObject->m_Scale.y != w->m_PropertiesUndo.m_Scale.y ||

				m_ActiveScene->m_pPropertiesObject->m_Size.x != w->m_PropertiesUndo.m_Size.x ||
				m_ActiveScene->m_pPropertiesObject->m_Size.y != w->m_PropertiesUndo.m_Size.y
				))) {

				//Object changed. save undo.
				MediaScene::MediaObjectUndo * undoobj = new MediaScene::MediaObjectUndo();

				undoobj->m_bRemoved = w->m_PropertiesUndo.m_bRemoved;
				undoobj->m_Color = w->m_PropertiesUndo.m_Color;
				undoobj->m_Depth = w->m_PropertiesUndo.m_Depth;
				undoobj->m_Layer = w->m_PropertiesUndo.m_Layer;
				undoobj->m_pObj = w->m_PropertiesUndo.m_pObj;
				undoobj->m_Position = w->m_PropertiesUndo.m_Position;
				undoobj->m_Rotation = w->m_PropertiesUndo.m_Rotation;
				undoobj->m_Scale = w->m_PropertiesUndo.m_Scale;
				undoobj->m_Size = w->m_PropertiesUndo.m_Size;
				undoobj->m_count = 0;
				undoobj->m_bRemoved = false;

				//PE: Bug make sure to delete everything with lerger index, or insert will fail.
				bool is_deleting = true;
				while (is_deleting) {
					is_deleting = false;
					for (std::map<std::int32_t, MediaScene::MediaObjectUndo *>::iterator it = w->m_Undo.begin(); it != w->m_Undo.end(); ++it)
					{
						if (it->first >= w->iUndoIndex) {
							MediaScene::MediaObjectUndo * mem;
							mem = it->second;
							w->m_Undo.erase(it->first);
							delete mem;
							is_deleting = true;
							break;
						}
					}
				}
				w->m_Undo.insert(std::make_pair(std::int32_t((w->iUndoIndex++)), undoobj));
				//w->AddUndo(o,false);
				w->m_PropertiesUndo.m_pObj->bUndoAdded = false;
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;

			}
			if (m_ActiveScene->m_pSelectedObject) {
				o->bUndoAdded = true;
				w->m_PropertiesUndo.m_pObj = o;
				w->m_PropertiesUndo.m_bRemoved = o->m_bRemoved;
				w->m_PropertiesUndo.m_Color = o->m_Color;
				w->m_PropertiesUndo.m_Depth = o->m_Depth;
				w->m_PropertiesUndo.m_Layer = o->m_Layer;
				w->m_PropertiesUndo.m_Position = o->m_Position;
				w->m_PropertiesUndo.m_Rotation = o->m_Rotation;
				w->m_PropertiesUndo.m_Scale = o->m_Scale;
				w->m_PropertiesUndo.m_Size = o->m_Size;
			}
		}

		m_ActiveScene->m_pPropertiesObject = o;

		if (o) {
			if (o->m_iMediaType == MEDIATYPE_SPRITE) {

				int icon_size = (int) (ImGui::GetFontSize()*3.0f);
				float icon_ratio;
				if ((icon_size / (float)o->m_Size.x) < (icon_size / (float)o->m_Size.y))
					icon_ratio = icon_size / (float)o->m_Size.x;
				else
					icon_ratio = icon_size / (float)o->m_Size.y;

				float bw = o->m_Size.x*icon_ratio;
				float bh = o->m_Size.y*icon_ratio;

				if (o && o->m_pUsedImage && o->m_pUsedImage->m_bAtlas && o->iAnimationFrom > 0)
					ImGui::ImgBtn(o->m_iImageId, ImVec2(bw, bh), pref.icon_background_color, ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), 2, o->iAnimationFrom, o->m_pUsedImage->m_iRows, o->m_pUsedImage->m_iColumns);
				else
					ImGui::ImgBtn(o->m_iImageId, ImVec2(bw, bh), pref.icon_background_color);
				ImGui::SameLine();

				ImGui::TextWrapped("Sprite Properties:\nSprite: %s\nMedia: %s", o->m_sName.GetStr(), o->m_sMediaRelativePath.GetStr());
			}
			if (o->m_iMediaType == MEDIATYPE_TEXT) {
				ImGui::TextWrapped("Text Properties:\n", o->m_sName.GetStr(), o->m_sMediaRelativePath.GetStr());
			}
			if (o->m_iMediaType == MEDIATYPE_BUTTON) {
				ImGui::TextWrapped("Button Properties:\n", o->m_sName.GetStr(), o->m_sMediaRelativePath.GetStr());
			}
			if (o->m_iMediaType == MEDIATYPE_EDITBOX) {
				ImGui::TextWrapped("Edit Box Properties:\n", o->m_sName.GetStr(), o->m_sMediaRelativePath.GetStr());
			}

			ImGui::Separator();
			char tmp[MAX_PATH];
			strcpy(tmp, o->m_sName.GetStr());
			uString uLabel = "Name##";
			uLabel.Append(o->m_sName);
			if (ImGui::InputText("Name", tmp, MAX_PATH, 0)) { //ImGuiInputTextFlags_EnterReturnsTrue
				o->m_sName = tmp;
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Name: For editor reference only, doesn't have to be unique");


			strcpy(tmp, o->m_sVariable.GetStr());
			uLabel = "Unique Variable##";
			uLabel.Append(o->m_sName);
			if (ImGui::InputText("Unique Variable", tmp, MAX_PATH, 0, TextFilters::TextFiltersVariable)) { //ImGuiInputTextFlags_EnterReturnsTrue
				o->m_sVariable = tmp;
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;
				if (o->m_sVariable.GetLength() > 0) {
					uString uSceneVar = w->m_sSceneName;
					uSceneVar.Append("_");
					o->m_sVariable.ReplaceStr(uSceneVar, "");
					uSceneVar.Append(o->m_sVariable);
					o->m_sVariable = uSceneVar;
				}
			}

			if (o->m_iMediaType == MEDIATYPE_SPRITE) {
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Assign a GLOBAL variable to this sprite.");
					ImGui::Text("You can directly access the sprite using this variable.");
					ImGui::EndTooltip();
					//			ImGui::SetTooltip("Variable: Assign a GLOBAL Variable To This Sprite.");
				}
			}
			else {
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Assign a GLOBAL variable to this object.");
					ImGui::Text("You can directly access the object using this variable.");
					ImGui::EndTooltip();
				}
			}

			if (o->m_iMediaType == MEDIATYPE_SPRITE || o->m_iMediaType == MEDIATYPE_TEXT) {

				strcpy(tmp, o->m_sArrayGroup.GetStr());
				uLabel = "Array Group##";
				uLabel.Append(o->m_sName);
				if (ImGui::InputText("Array Group", tmp, MAX_PATH, 0, TextFilters::TextFiltersVariable)) { //ImGuiInputTextFlags_EnterReturnsTrue
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					o->m_sArrayGroup = tmp;
					if (o->m_sArrayGroup.GetLength() > 0) {
						uString uSceneVar = w->m_sSceneName;
						uSceneVar.Append("_");
						o->m_sArrayGroup.ReplaceStr(uSceneVar, "");
						uSceneVar.Append(o->m_sArrayGroup);
						o->m_sArrayGroup = uSceneVar;
					}
				}

				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					if(o->m_iMediaType == MEDIATYPE_SPRITE)
						ImGui::Text("Assign a group array to this sprite.");
					else
						ImGui::Text("Assign a group array to this text.");
					ImGui::Text("You can use the group array directly in your code.");
					ImGui::EndTooltip();
				}

			}
			if (o->m_iMediaType == MEDIATYPE_TEXT || o->m_iMediaType == MEDIATYPE_BUTTON || o->m_iMediaType == MEDIATYPE_EDITBOX) {

				strcpy(tmp, o->m_Text.GetStr());
				uString uLabel = "Text##";
				uLabel.Append(o->m_sName);
				if (ImGui::InputText("Text", tmp, MAX_PATH, 0)) { //ImGuiInputTextFlags_EnterReturnsTrue
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					o->m_Text = tmp;
					if(o->m_iMediaType == MEDIATYPE_TEXT)
						agk::SetTextString(o->m_iSprite, o->m_Text.GetStr());
					if (o->m_iMediaType == MEDIATYPE_BUTTON) {
						agk::DeleteVirtualButton(o->m_iSprite);
						agk::AddVirtualButton(o->m_iSprite, 0, 0, 80);
						agk::SetVirtualButtonText(o->m_iSprite, o->m_Text.GetStr());
						if (o->m_iImageIndex > 0)
							agk::SetVirtualButtonImageUp(o->m_iSprite, o->m_iImageId);
						else
							agk::SetVirtualButtonImageUp(o->m_iSprite, 0);

						if (o->m_iImageDownIndex > 0)
							agk::SetVirtualButtonImageDown(o->m_iSprite, o->m_iImageDownId);
						else
							agk::SetVirtualButtonImageDown(o->m_iSprite, 0);

						//agk::SetVirtualButtonText(o->m_iSprite, o->m_Text.GetStr());
					}
					if (o->m_iMediaType == MEDIATYPE_EDITBOX) {
						agk::DeleteEditBox(o->m_iSprite);
						o->m_iSprite = agk::CreateEditBox();
						agk::SetEditBoxText(o->m_iSprite, o->m_Text.GetStr());
						//agk::SetEditBoxText(o->m_iSprite, o->m_Text.GetStr());
					}
					w->UpdateObject(o);
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Enter text to display in text control.");

			}

			if (o->m_iMediaType == MEDIATYPE_TEXT) {


				if (m_ActiveScene->Fonts.size() > 0) {
					bool comboret = false;
					uString CurrentFont = "Default Font";
					if (o->m_iFontId > 0) {
						std::map<std::int32_t, std::string>::iterator it = m_ActiveScene->Fonts.find(o->m_iFontId);
						if (it != m_ActiveScene->Fonts.end()) {
							uString FontPath = it->second.c_str();
							uString FontName = "na";
							int pos = FontPath.RevFindStr("\\");
							if (pos < 1) pos = FontPath.RevFindStr("/");
							if (pos > 0) {
								FontPath.SubString(FontName, pos + 1);
							}

							//Cant change font in combo.
							bool is_selected = (o->m_iFontId == it->first);
							comboret = ImGui::BeginCombo("##seletfontsused", FontName.GetStr() , 0);

						}
					}
					else {
						comboret = ImGui::BeginCombo("##seletfontsused", CurrentFont.GetStr(), 0);
					}



					if(comboret)
					{
						bool is_selected = (o->m_iFontId == 0);

						if (ImGui::Selectable("Default", is_selected)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;

							o->m_iFontId = 0;
							//Recreate text object.
//							agk::DeleteText(o->m_iSprite);
//							o->m_iSprite = agk::CreateText(o->m_Text.GetStr());
							o->m_iLoadFontId = 0;
							o->m_iAgkFontId = 0;
						}

						for (std::map<std::int32_t, std::string>::iterator it = m_ActiveScene->Fonts.begin(); it != m_ActiveScene->Fonts.end(); ++it)
						{

							uString FontPath = it->second.c_str();
							uString FontName = "na";
							int pos = FontPath.RevFindStr("\\");
							if (pos < 1) pos = FontPath.RevFindStr("/");
							if (pos > 0) {
								FontPath.SubString(FontName, pos + 1);
							}

							float fs = ImGui::GetFontSize();
							float scale = (1.0f / (64.0f - fs))*20.0f;

							if (m_ActiveScene->customfonts[it->first] && m_ActiveScene->customfonts[it->first]->FontSize > 0.0f) {
								if (!m_ActiveScene->customfonts[it->first]->IsLoaded())
									ImGui::PushFont(defaultfont);  //defaultfont
								else
									ImGui::PushFont(m_ActiveScene->customfonts[it->first]);
							}
							ImGui::SetWindowFontScale(scale);
							bool is_selected = (o->m_iFontId == it->first);

							if (ImGui::Selectable(FontName.GetStr(), is_selected)) {
								if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
									m_ActiveScene->m_SceneEditor->filechanged = true;
								o->m_iFontId = it->first;
								//Recreate text object.
								//agk::DeleteText(o->m_iSprite);
								//o->m_iSprite = agk::CreateText(o->m_Text.GetStr());
								o->m_iLoadFontId = 0;
								o->m_iAgkFontId = 0;
							}
							ImGui::SetWindowFontScale(1.0f);
							if (m_ActiveScene->customfonts[it->first] && m_ActiveScene->customfonts[it->first]->FontSize > 0.0f) {
								ImGui::PopFont();
							}


						}

						ImGui::EndCombo();
					}
					ImGui::SameLine();
					ImGui::Text("Select Font");
				}
				else {
					o->m_iFontId = 0;
				}

			}

			if (w->bUsePercentage) {
				float percentx = (o->m_Position.x / (float)w->iBaseWidth)*100.0f;
				float percenty = (o->m_Position.y / (float)w->iBaseHeight)*100.0f;
				char format[40];
				sprintf(format, "%%.%df%%%%", decimalprecision);
				if(ImGui::InputFloat("Position X", &percentx, 0.1f, 1.0f, format, 0) ) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					o->m_Position.x = (w->iBaseWidth / 100.0f) * percentx;
				}
				if (ImGui::InputFloat("Position Y", &percenty, 0.1f, 1.0f, format, 0)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					o->m_Position.y = (w->iBaseHeight / 100.0f) * percenty;
				}
			}
			else {
				if (ImGui::InputFloat("Position X", &o->m_Position.x, 0.1f, 1.0f, decimalprecision)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
				}
				if (ImGui::InputFloat("Position Y", &o->m_Position.y, 0.1f, 1.0f, decimalprecision)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
				}
			}
			//SetSpriteAngle(spr,angle)

			if (w->bUsePercentage) {
				//Size
				float percentx = (o->m_Size.x / (float)w->iBaseWidth)*100.0f;
				float percenty = (o->m_Size.y / (float)w->iBaseHeight)*100.0f;
				char format[40];
				sprintf(format, "%%.%df%%%%", decimalprecision);

				if (o->m_iMediaType == MEDIATYPE_SPRITE || o->m_iMediaType == MEDIATYPE_BUTTON || o->m_iMediaType == MEDIATYPE_EDITBOX) {
					if (ImGui::InputFloat("Size X", &percentx, 0.1f, 1.0f, format, 0)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
						o->m_Size.x = (w->iBaseWidth / 100.0f) * percentx;

						if (o->m_iMediaType == MEDIATYPE_SPRITE) {
							agk::SetSpriteSize(o->m_iSprite, (o->m_Size.x / (float)w->iBaseWidth)*100.0f, (o->m_Size.y / (float)w->iBaseHeight)*100.0f);
							agk::SetSpriteScale(o->m_iSprite, o->m_Scale.x, o->m_Scale.y);
						}
						if (o->m_iMediaType == MEDIATYPE_BUTTON)
							agk::SetVirtualButtonSize(o->m_iSprite, (o->m_Size.x / (float)w->iBaseWidth)*100.0f, (o->m_Size.y / (float)w->iBaseHeight)*100.0f);
						if (o->m_iMediaType == MEDIATYPE_EDITBOX)
							agk::SetEditBoxSize(o->m_iSprite, (o->m_Size.x / (float)w->iBaseWidth)*100.0f, (o->m_Size.y / (float)w->iBaseHeight)*100.0f);
					}
				}

				uString tmplabel = "Size Y";
				if (o->m_iMediaType == MEDIATYPE_TEXT) {
					tmplabel = "Size";
				}

				if (ImGui::InputFloat(tmplabel.GetStr() , &percenty, 0.1f, 1.0f, format, 0)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					o->m_Size.y = (w->iBaseHeight / 100.0f) * percenty;

					if (o->m_iMediaType == MEDIATYPE_SPRITE) {
						agk::SetSpriteSize(o->m_iSprite, (o->m_Size.x / (float)w->iBaseWidth)*100.0f, (o->m_Size.y / (float)w->iBaseHeight)*100.0f);
						agk::SetSpriteScale(o->m_iSprite, o->m_Scale.x, o->m_Scale.y);
					}
					if (o->m_iMediaType == MEDIATYPE_BUTTON)
						agk::SetVirtualButtonSize(o->m_iSprite, (o->m_Size.x / (float)w->iBaseWidth)*100.0f, (o->m_Size.y / (float)w->iBaseHeight)*100.0f);
					if (o->m_iMediaType == MEDIATYPE_EDITBOX)
						agk::SetEditBoxSize(o->m_iSprite, (o->m_Size.x / (float)w->iBaseWidth)*100.0f, (o->m_Size.y / (float)w->iBaseHeight)*100.0f);

				}
			}
			else {
				if (o->m_iMediaType == MEDIATYPE_SPRITE || o->m_iMediaType == MEDIATYPE_BUTTON || o->m_iMediaType == MEDIATYPE_EDITBOX) {
					if (ImGui::InputFloat("Size X", &o->m_Size.x, 0.1f, 1.0f, decimalprecision)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
						if (o->m_iMediaType == MEDIATYPE_SPRITE) {
							agk::SetSpriteSize(o->m_iSprite, o->m_Size.x, o->m_Size.y);
							agk::SetSpriteScale(o->m_iSprite, o->m_Scale.x, o->m_Scale.y);
						}
						if (o->m_iMediaType == MEDIATYPE_BUTTON)
							agk::SetVirtualButtonSize(o->m_iSprite, o->m_Size.x, o->m_Size.y);
						if (o->m_iMediaType == MEDIATYPE_EDITBOX)
							agk::SetEditBoxSize(o->m_iSprite, o->m_Size.x, o->m_Size.y);
					}
				}

				uString tmplabel = "Size Y";
				if (o->m_iMediaType == MEDIATYPE_TEXT) {
					tmplabel = "Size";
				}

				if (ImGui::InputFloat(tmplabel.GetStr() , &o->m_Size.y, 0.1f, 1.0f, decimalprecision)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					if (o->m_iMediaType == MEDIATYPE_SPRITE) {
						agk::SetSpriteSize(o->m_iSprite, o->m_Size.x, o->m_Size.y);
						agk::SetSpriteScale(o->m_iSprite, o->m_Scale.x, o->m_Scale.y);
					}
					if (o->m_iMediaType == MEDIATYPE_BUTTON)
						agk::SetVirtualButtonSize(o->m_iSprite, o->m_Size.x, o->m_Size.y);
					if (o->m_iMediaType == MEDIATYPE_EDITBOX)
						agk::SetEditBoxSize(o->m_iSprite, o->m_Size.x, o->m_Size.y);

				}
			}

			if (o->m_Size.x < 1.0f)
				o->m_Size.x = 1.0f;
			if (o->m_Size.y < 1.0f)
				o->m_Size.y = 1.0f;

			//		if (ImGui::InputFloat("Rotation Angle", &o->m_Rotation.x)) {
			//			agk::SetSpriteAngle(o->m_iSprite, o->m_Rotation.x);
			//		}

			if (o->m_iMediaType == MEDIATYPE_SPRITE || o->m_iMediaType == MEDIATYPE_TEXT ) {

				if (ImGui::SliderFloat("Rotation", &o->m_Rotation.x, 0.0f, 360.0f)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					if (o->m_iMediaType == MEDIATYPE_SPRITE)
						agk::SetSpriteAngle(o->m_iSprite, o->m_Rotation.x);
					else
						agk::SetTextAngle(o->m_iSprite, o->m_Rotation.x);
				}

				if (o->m_iMediaType == MEDIATYPE_SPRITE) {
					if (ImGui::InputFloat("Scale X", &o->m_Scale.x, 0.1f, 1.0f, decimalprecision)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
						agk::SetSpriteScale(o->m_iSprite, o->m_Scale.x, o->m_Scale.y);
					}
					if (ImGui::InputFloat("Scale Y", &o->m_Scale.y, 0.1f, 1.0f, decimalprecision)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
						agk::SetSpriteScale(o->m_iSprite, o->m_Scale.x, o->m_Scale.y);
					}
					if (o->m_Scale.x < 0)
						o->m_Scale.x = 0.0f;
					if (o->m_Scale.y < 0)
						o->m_Scale.y = 0.0f;
				}
			}

			//m_Color
			o->m_Color.x = o->m_Color.x / 255.0f;
			o->m_Color.y = o->m_Color.y / 255.0f;
			o->m_Color.z = o->m_Color.z / 255.0f;
			o->m_Color.w = o->m_Color.w / 255.0f;
			bool c_change = false;
			if (o->m_iMediaType == MEDIATYPE_EDITBOX) {
				c_change = ImGui::ColorEdit4("Background Color##media_m_Color", (float*)&o->m_Color, ImGuiColorEditFlags_AlphaPreview);
			}
			else if (o->m_iMediaType == MEDIATYPE_TEXT) {
				c_change = ImGui::ColorEdit4("Text Color##media_m_Color", (float*)&o->m_Color, ImGuiColorEditFlags_AlphaPreview);
			}
			else if (o->m_iMediaType == MEDIATYPE_BUTTON) {
				c_change = ImGui::ColorEdit4("Button Color##media_m_Color", (float*)&o->m_Color, ImGuiColorEditFlags_AlphaPreview);
			}
			else {
				c_change = ImGui::ColorEdit4("Sprite Color##media_m_Color", (float*)&o->m_Color, ImGuiColorEditFlags_AlphaPreview);
			}
			o->m_Color.x *= 255.0f;
			o->m_Color.y *= 255.0f;
			o->m_Color.z *= 255.0f;
			o->m_Color.w *= 255.0f;
			if (c_change) {
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;
				if (o->m_iMediaType == MEDIATYPE_SPRITE)
					agk::SetSpriteColor(o->m_iSprite, (int) o->m_Color.x, (int) o->m_Color.y, (int) o->m_Color.z, (int) o->m_Color.w);
				if (o->m_iMediaType == MEDIATYPE_TEXT)
					agk::SetTextColor(o->m_iSprite, (int) o->m_Color.x, (int) o->m_Color.y, (int) o->m_Color.z, (int) o->m_Color.w);
				if (o->m_iMediaType == MEDIATYPE_BUTTON) {
					agk::SetVirtualButtonAlpha(o->m_iSprite, (int) o->m_Color.w);
					agk::SetVirtualButtonColor(o->m_iSprite, (int) o->m_Color.x, (int) o->m_Color.y, (int) o->m_Color.z);
				}
				if (o->m_iMediaType == MEDIATYPE_EDITBOX) {
					agk::SetEditBoxBackgroundColor(o->m_iSprite, (int) o->m_Color.x, (int) o->m_Color.y, (int) o->m_Color.z, (int) o->m_Color.w);
				}
			}

			if (o->m_iMediaType == MEDIATYPE_EDITBOX) {

				o->m_TextColor.x = o->m_TextColor.x / 255.0f;
				o->m_TextColor.y = o->m_TextColor.y / 255.0f;
				o->m_TextColor.z = o->m_TextColor.z / 255.0f;
				o->m_TextColor.w = o->m_TextColor.w / 255.0f;
				c_change = ImGui::ColorEdit4("Text Color##media_m_Color", (float*)&o->m_TextColor, ImGuiColorEditFlags_AlphaPreview);
				o->m_TextColor.x *= 255.0f;
				o->m_TextColor.y *= 255.0f;
				o->m_TextColor.z *= 255.0f;
				o->m_TextColor.w *= 255.0f;
				if (c_change) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					agk::SetEditBoxTextColor(o->m_iSprite, (int) o->m_TextColor.x, (int) o->m_TextColor.y, (int) o->m_TextColor.z);
				}

			}

			if (o->m_iMediaType == MEDIATYPE_EDITBOX) {

				o->m_BorderColor.x = o->m_BorderColor.x / 255.0f;
				o->m_BorderColor.y = o->m_BorderColor.y / 255.0f;
				o->m_BorderColor.z = o->m_BorderColor.z / 255.0f;
				o->m_BorderColor.w = o->m_BorderColor.w / 255.0f;
				c_change = ImGui::ColorEdit4("Border Color##media_m_Color", (float*)&o->m_BorderColor, ImGuiColorEditFlags_AlphaPreview);
				o->m_BorderColor.x *= 255.0f;
				o->m_BorderColor.y *= 255.0f;
				o->m_BorderColor.z *= 255.0f;
				o->m_BorderColor.w *= 255.0f;
				if (c_change) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					agk::SetEditBoxBorderColor(o->m_iSprite, (int) o->m_BorderColor.x, (int) o->m_BorderColor.y, (int) o->m_BorderColor.z, (int) o->m_BorderColor.w);
				}


				o->m_CursorColor.x = o->m_CursorColor.x / 255.0f;
				o->m_CursorColor.y = o->m_CursorColor.y / 255.0f;
				o->m_CursorColor.z = o->m_CursorColor.z / 255.0f;
				o->m_CursorColor.w = o->m_CursorColor.w / 255.0f;
				c_change = ImGui::ColorEdit4("Cursor Color##media_c_Color", (float*)&o->m_CursorColor, 0); //ImGuiColorEditFlags_AlphaPreview
				o->m_CursorColor.x *= 255.0f;
				o->m_CursorColor.y *= 255.0f;
				o->m_CursorColor.z *= 255.0f;
				o->m_CursorColor.w *= 255.0f;
				if (c_change) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					agk::SetEditBoxCursorColor(o->m_iSprite, (int) o->m_CursorColor.x, (int) o->m_CursorColor.y, (int) o->m_CursorColor.z);
				}


			}


			if (o->m_iMediaType == MEDIATYPE_BUTTON) {

				float icon_size = ImGui::GetFontSize()*1.5f;
				uString ImgName;
				if (o->m_iImageIndex == 0) {
					o->m_iImageId = agkbutton_image;
					ImgName = "Default";
				}
				else {
					//find name.
					ImgName = "imgname";
					if (o->m_pUsedImage) {
						if(o->m_pUsedImage->m_sName.GetLength() > 0 )
							ImgName = o->m_pUsedImage->m_sName;
					}
				}
				float icon_ratio;
				float imgw = o->m_Size.x;
				float imgh = o->m_Size.y;
				if (o->m_pUsedImage) {
					imgw = (float) o->m_pUsedImage->m_iWidth;
					imgh = (float) o->m_pUsedImage->m_iHeight;
				}

				if ((icon_size / (float)imgw) < (icon_size / (float)imgh))
					icon_ratio = icon_size / (float)imgw;
				else
					icon_ratio = icon_size / (float)imgh;
				float bw = (imgw)*icon_ratio;
				float bh = (imgh)*icon_ratio;

				ImGui::ImgBtn(o->m_iImageId, ImVec2(bw, bh));

				ImGui::SameLine();
				ImGui::PushItemWidth(icon_size*5);
				bool is_selected = (o->m_iImageIndex == 0);
				bool comboret = ImGui::BeginCombo("##seletimgbuttonup", ImgName.GetStr(), 0);
				if (comboret) {

					MediaScene::Images * m_pSearchImage;
					m_pSearchImage = w->m_pFirstImage;
					if (m_pSearchImage) {
						//Display images.
						if (ImGui::Button("Default Image")) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							o->m_iImageIndex = 0;
							o->m_iImageId = 0;
							agk::SetVirtualButtonImageUp(o->m_iSprite, o->m_iImageId);
						}
						while (m_pSearchImage) {

							float icon_size = ImGui::GetFontSize()*2.5f;

							float icon_ratio;

							float imgw = (float) m_pSearchImage->m_iWidth;
							float imgh = (float) m_pSearchImage->m_iHeight;
							if ((icon_size / (float)imgw) < (icon_size / (float)imgh))
								icon_ratio = icon_size / (float)imgw;
							else
								icon_ratio = icon_size / (float)imgh;
							float bw = (imgw)*icon_ratio;
							float bh = (imgh)*icon_ratio;

							if (ImGui::ImgBtn(m_pSearchImage->m_iImageId, ImVec2(bw, bh))) {
								if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
									m_ActiveScene->m_SceneEditor->filechanged = true;
								o->m_iImageIndex = m_pSearchImage->m_iImageIndex;
								o->m_iImageId = m_pSearchImage->m_iImageId;
								o->m_Size.x = (float) m_pSearchImage->m_iWidth;
								o->m_Size.y = (float) m_pSearchImage->m_iHeight; // *2.5f;
								o->m_pUsedImage = m_pSearchImage;
								agk::SetVirtualButtonImageUp(o->m_iSprite, o->m_iImageId);
							}

							m_pSearchImage = m_pSearchImage->m_pNext;
						}
					}

					ImGui::EndCombo();
					ImGui::PopItemWidth();

				}
				ImGui::SameLine();
				ImGui::Text("Button Up Image");


				//#### DOWN IMAGE ####

				icon_size = ImGui::GetFontSize()*1.5f;
				ImgName;
				if (o->m_iImageDownIndex == 0) {
					o->m_iImageDownId = agkbutton_image;
					ImgName = "Default";
				}
				else {
					//find name.
					ImgName = "imgname";
					if (o->m_pUsedDownImage) {
						if (o->m_pUsedDownImage->m_sName.GetLength() > 0)
							ImgName = o->m_pUsedDownImage->m_sName;
					}
				}
				icon_ratio;
				imgw = o->m_Size.x;
				imgh = o->m_Size.y;
				if (o->m_pUsedDownImage) {
					imgw = (float) o->m_pUsedDownImage->m_iWidth;
					imgh = (float) o->m_pUsedDownImage->m_iHeight;
				}

				if ((icon_size / (float)imgw) < (icon_size / (float)imgh))
					icon_ratio = icon_size / (float)imgw;
				else
					icon_ratio = icon_size / (float)imgh;
				bw = (imgw)*icon_ratio;
				bh = (imgh)*icon_ratio;

				ImGui::ImgBtn(o->m_iImageDownId, ImVec2(bw, bh));

				ImGui::SameLine();
				ImGui::PushItemWidth(icon_size * 5);
				is_selected = (o->m_iImageDownIndex == 0);
				comboret = ImGui::BeginCombo("##seletimgbuttondown", ImgName.GetStr(), 0);
				if (comboret) {

					MediaScene::Images * m_pSearchImage;
					m_pSearchImage = w->m_pFirstImage;
					if (m_pSearchImage) {
						//Display images.
						if (ImGui::Button("Default Image")) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							o->m_iImageDownIndex = 0;
							o->m_iImageDownId = 0;
							agk::SetVirtualButtonImageDown(o->m_iSprite, o->m_iImageDownId);
						}
						while (m_pSearchImage) {

							float icon_size = ImGui::GetFontSize()*2.5f;

							float icon_ratio;

							float imgw = (float) m_pSearchImage->m_iWidth;
							float imgh = (float) m_pSearchImage->m_iHeight;
							if ((icon_size / (float)imgw) < (icon_size / (float)imgh))
								icon_ratio = icon_size / (float)imgw;
							else
								icon_ratio = icon_size / (float)imgh;
							float bw = (imgw)*icon_ratio;
							float bh = (imgh)*icon_ratio;

							if (ImGui::ImgBtn(m_pSearchImage->m_iImageId, ImVec2(bw, bh))) {
								if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
									m_ActiveScene->m_SceneEditor->filechanged = true;

								o->m_iImageDownIndex = m_pSearchImage->m_iImageIndex;
								o->m_iImageDownId = m_pSearchImage->m_iImageId;
								//o->m_Size.x = m_pSearchImage->m_iWidth;
								//o->m_Size.y = m_pSearchImage->m_iHeight; // *2.5f;
								o->m_pUsedDownImage = m_pSearchImage;
								agk::SetVirtualButtonImageDown(o->m_iSprite, o->m_iImageDownId);
							}

							m_pSearchImage = m_pSearchImage->m_pNext;
						}
					}

					ImGui::EndCombo();
					ImGui::PopItemWidth();

				}
				ImGui::SameLine();
				ImGui::Text("Button Down Image");


			}

			if (o->m_iMediaType == MEDIATYPE_EDITBOX) {

				//SetEditBoxTextSize(index, size)
				if (ImGui::InputInt("Text Height", &o->m_iTextSize, 1, 10)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					if(o->m_iTextSize > 0)
						agk::SetEditBoxTextSize(o->m_iSprite, (float) o->m_iTextSize);
					else {
						//Need to delete it to get old size calc.
						agk::DeleteEditBox(o->m_iSprite);
						o->m_iSprite = agk::CreateEditBox();
						agk::SetEditBoxText(o->m_iSprite, o->m_Text.GetStr());
					}
				}
				if (o->m_iTextSize < 0)
					o->m_iTextSize = 0;

				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Sets the height in world coordinates of the text in this edit box.");
					ImGui::Text("Set to 0 to use default that use the edit box's height minus 2.");
					ImGui::EndTooltip();
				}

				if (ImGui::InputInt("Cursor Width", &o->m_iCursorSize, 1, 10)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					agk::SetEditBoxCursorWidth(o->m_iSprite, (float) o->m_iCursorSize);
				}

				if (ImGui::InputInt("Border Size", &o->m_iBorderSize, 1, 10)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					agk::SetEditBoxBorderSize(o->m_iSprite, (float) o->m_iBorderSize);
				}

				if (ImGui::InputInt("Max Characters", &o->m_iEditMaxChars, 1, 10)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					agk::SetEditBoxMaxChars(o->m_iSprite, o->m_iEditMaxChars);
				}
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Sets the maximum number of characters that can");
					ImGui::Text("be entered in this edit box. Use 0 for unlimited.");
					ImGui::EndTooltip();
				}

				if (ImGui::Checkbox("Multi Lines", &o->bEditMultiLines)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					if(o->bEditMultiLines)
						agk::SetEditBoxMultiLine(o->m_iSprite, 1);
					else
						agk::SetEditBoxMultiLine(o->m_iSprite, 0);
				}
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Sets whether the edit box will wrap text to a ");
					ImGui::Text("new line when it reaches the edge of the edit box.");
					ImGui::EndTooltip();
				}

				ImGui::SameLine();

				if (ImGui::Checkbox("Password", &o->bEditPassword)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					if (o->bEditPassword)
						agk::SetEditBoxPasswordMode(o->m_iSprite, 1);
					else
						agk::SetEditBoxPasswordMode(o->m_iSprite, 0);
				}
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Sets whether the edit box display stars");
					ImGui::Text("instead of the text input.");
					ImGui::EndTooltip();
				}

				if (ImGui::Checkbox("Wrap Mode", &o->bEditWrap)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					if (o->bEditWrap)
						agk::SetEditBoxWrapMode(o->m_iSprite, 1);
					else
						agk::SetEditBoxWrapMode(o->m_iSprite, 0);
				}
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("When multi lines is not used this command sets whether");
					ImGui::Text("the single line of text will scroll to the right or");
					ImGui::Text("wrap to a new line when it over flows the edit box width.");
					ImGui::EndTooltip();
				}

				//SetEditBoxMaxLines

				//m_iEditMaxChars
				//SetEditBoxMaxChars( index, max )
			}
			

			if (o->m_iMediaType != MEDIATYPE_BUTTON) {
				//m_Depth
//				ImGui::Text("Depth: 0 = Front Most , 10000 = Back Most.");
				ImGui::Separator();

				//Layers.
				// 0 , 3 , 5 , 7 , 10

				bool bSelected = false;
				int layer = 0;
				uString uName;
				for (int i = 0; i < 11; i++) {
					if (i == 0 || i == 3 || i == 5 || i == 7 || i == 10) {
						layer = i;
						bSelected = false;
						if (o->m_Depth == w->iLayerDepth[layer] )
							bSelected = true;
						ImVec2 cp = ImGui::GetCursorPos();

						uName = "##Layerselect";
						uName.AppendInt(layer);
						if (ImGui::Selectable(uName.GetStr(), bSelected)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							o->m_Depth = w->iLayerDepth[layer];
							w->m_pSelectedObject = NULL; //Need to disable this as this always bring it to front.
							if (o->m_iMediaType == MEDIATYPE_SPRITE)
								agk::SetSpriteDepth(o->m_iSprite, o->m_Depth);
							if (o->m_iMediaType == MEDIATYPE_TEXT)
								agk::SetTextDepth(o->m_iSprite, o->m_Depth);
						}
						cp.y -= fsize*0.15;
						ImGui::SetCursorPos(cp);
						ImGui::SetItemAllowOverlap();
						uName = "##LayerVisible";
						uName.AppendInt(layer);
						ImGui::Indent(fsize*0.5);
						ImGui::ToggleButton(uName.GetStr(), &w->bLayerVisible[layer]);
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Layer Visible");
						ImGui::SameLine();
						ImGui::PushItemWidth(fsize*4.0);
						ImGui::SetItemAllowOverlap();
						uName = "##LayerOpacity";
						uName.AppendInt(layer);
						ImGui::SliderInt(uName.GetStr(), &w->iLayerOpacity[layer], 0, 255);
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Layer Opacity");
						ImGui::PopItemWidth();
						ImGui::SameLine();
						ImGui::Text("Layer: %s (%d)", w->sLayerName[layer].GetStr(), w->iLayerDepth[layer]);
						ImGui::Indent(-(fsize*0.5));
					}
				}

				
				if (ImGui::SliderInt("Depth##Depth 0=Front 10000=Back", &o->m_Depth, 0, 10000)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
					w->m_pSelectedObject = NULL; //Need to disable this as this always bring it to front.
					if (o->m_iMediaType == MEDIATYPE_SPRITE)
						agk::SetSpriteDepth(o->m_iSprite, o->m_Depth);
					if (o->m_iMediaType == MEDIATYPE_TEXT)
						agk::SetTextDepth(o->m_iSprite, o->m_Depth);
				}

				ImGui::Separator();
			}

			if (o->m_iMediaType == MEDIATYPE_SPRITE) {
				if (ImGui::Checkbox("Stretch To Device", &o->bStretchToDevice)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Always fit sprite to device width/height.");
				ImGui::SameLine();
			}
			
			if (ImGui::Checkbox("Fix to Screen", &o->bFixToScreen)) {
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Always stay in the same place.");

			if (ImGui::Checkbox("Lock", &o->bLocked)) {
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;
				if (o->bLocked)
					m_ActiveScene->m_pSelectedObject = NULL;
			}

			if( o->m_iMediaType == MEDIATYPE_SPRITE  ) {
				ImGui::SameLine();
				if (ImGui::Checkbox("Crop to Size of Visible Area", &o->bOnlyVisibleInAGK)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
				}
			}

			if (ImGui::Checkbox("Visible by Default", &o->m_bVisible)) {
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;
			}
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text("Set media visible by default. This can be used to");
				ImGui::Text("hide media until you set it visible in your own code.");
				ImGui::EndTooltip();
			}



			if (o->m_iMediaType == MEDIATYPE_SPRITE) {

				if (ImGui::Checkbox("Enable Sprite UV Scrolling", &o->bUVOffset)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Setup UV scrolling of sprite image.");

				if (o->bUVOffset) {

					int width = (int) agk::GetImageWidth(o->m_iImageId);
					if ((width & (width - 1)) != 0) // check power of 2
					{
						ImGui::TextWrapped("Image must be a power of two to use UV scrolling.");
					}
					else {
						ImGui::PushItemWidth(ImGui::GetFontSize()*4.0f);
						o->fUVSpeedX *= 100.0f;
						if (ImGui::InputFloat("UV Speed X", &o->fUVSpeedX, 0.0f, 0.0f, decimalprecision)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
						}
						if (o->fUVSpeedX != 0)
							o->fUVSpeedX /= 100.0f;
						ImGui::PopItemWidth();
						ImGui::SameLine();
						o->fUVSpeedY *= 100.0f;
						ImGui::PushItemWidth(ImGui::GetFontSize()*4.0f);
						if (ImGui::InputFloat("UV Speed Y", &o->fUVSpeedY, 0.0f, 0.0f, decimalprecision)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
						}
						if (o->fUVSpeedY != 0)
							o->fUVSpeedY /= 100.0f;
						ImGui::PopItemWidth();
					}
				}
			}
			ImGui::Separator();
			if (o->m_iMediaType == MEDIATYPE_SPRITE) {
				//bEnablePhysics = false;
				//iPhysicsShape = 3; // 0=no shape, 1=circle, 2=box, 3=polygon
				//iPhysicsMode = 0; // 1=static, 2=dynamic, 3=kinematic
				//bPhysicsCanRotate = true;
				//bPhysicsIsBullet

				if (ImGui::Checkbox("Enable Physics", &o->bEnablePhysics)) {
					if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
						m_ActiveScene->m_SceneEditor->filechanged = true;
				}

				if (o->bEnablePhysics) {
					//Shape.

					ImGui::Text("Shape:");
					const char* physics_shape_array[] = { "No Shape", "Circle", "Box", "Polygon" };
					ImGui::SameLine();
					//ImGui::PushItemWidth(-10);
					//ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
					if (ImGui::Combo("##combophysics_shape", &o->iPhysicsShape, physics_shape_array, IM_ARRAYSIZE(physics_shape_array))) {
						//agk::SetSpriteShape(o->m_iSprite, o->iPhysicsShape);
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
					}
					//ImGui::PopItemWidth();

					ImGui::Text("Mode:");
					const char* physics_mode_array[] = { "Static", "Dynamic", "Kinematic" };
					ImGui::SameLine();
					//ImGui::PushItemWidth(-10);
					//ImGui::SetCursorPos(ImVec2(input_indent, ImGui::GetCursorPos().y));
					if (ImGui::Combo("##combophysics_mode", &o->iPhysicsMode, physics_mode_array, IM_ARRAYSIZE(physics_mode_array))) {
						//not until test mode.
						//agk::SetSpritePhysicsOn(o->m_iSprite, o->iPhysicsMode+1);
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
					}
					//ImGui::PopItemWidth();

					if (ImGui::Checkbox("Can Rotate", &o->bPhysicsCanRotate)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
					}
					ImGui::SameLine();
					if (ImGui::Checkbox("Is Bullet", &o->bPhysicsIsBullet)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
					}

//					if (ImGui::InputFloat("Restitution", &o->fPhysicsRestitution, 0.01f, 0.10f, decimalprecision)) {
//						if (o->fPhysicsRestitution > 1.0f)
//							o->fPhysicsRestitution = 1.0f;
//						if (o->fPhysicsRestitution < 0.0f)
//							o->fPhysicsRestitution = 0.0f;
//					}

					if (ImGui::SliderFloat("Restitution", &o->fPhysicsRestitution, 0.0f, 1.0f)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
						agk::SetSpritePhysicsRestitution(o->m_iSprite, o->fPhysicsRestitution);
					}


					if (ImGui::SliderFloat("Friction", &o->fPhysicsFriction, 0.0f, 1.0f)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
						agk::SetSpritePhysicsFriction(o->m_iSprite, o->fPhysicsFriction);
					}

					if (ImGui::SliderFloat("Linear Damping", &o->fPhysicsLinearDamping, 0.0f, 40.0f)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
						agk::SetSpritePhysicsDamping(o->m_iSprite, o->fPhysicsLinearDamping);
					}
					if (ImGui::SliderFloat("Angular Damping", &o->fPhysicsAngularDamping, 0.0f, 40.0f)) {
						if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
							m_ActiveScene->m_SceneEditor->filechanged = true;
						agk::SetSpritePhysicsAngularDamping(o->m_iSprite, o->fPhysicsAngularDamping);
					}

					if (w->bUsePercentage) {
						//Size
						float percentx = (o->fPhysicsMass / (float) w->iBaseWidth)*100.0f;
						if (ImGui::InputFloat("Mass", &percentx, 1.0f, 10.0f, decimalprecision)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							o->fPhysicsMass = (w->iBaseWidth / 100.0f) * percentx;
							agk::SetSpritePhysicsMass(o->m_iSprite, (o->fPhysicsMass / (float)w->iBaseWidth)*100.0f);
						}

						percentx = (o->fPhysicsCOMX / (float)w->iBaseWidth)*100.0f;
						if (ImGui::InputFloat("Mass Center X", &percentx, 0.01f, 0.10f, decimalprecision)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							o->fPhysicsCOMX = (w->iBaseWidth / 100.0f) * percentx;
							agk::SetSpritePhysicsCOM(o->m_iSprite, (o->fPhysicsCOMX / (float)w->iBaseWidth)*100.0f, (o->fPhysicsCOMY / (float)w->iBaseHeight)*100.0f);
						}
						float percenty = (o->fPhysicsCOMY / (float)w->iBaseHeight)*100.0f;
						if (ImGui::InputFloat("Mass Center Y", &percenty, 0.01f, 0.10f, decimalprecision)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							o->fPhysicsCOMY = (w->iBaseHeight / 100.0f) * percenty;
							agk::SetSpritePhysicsCOM(o->m_iSprite, (o->fPhysicsCOMX / (float)w->iBaseWidth)*100.0f, (o->fPhysicsCOMY / (float)w->iBaseHeight)*100.0f);
						}
					}
					else {
						if (ImGui::InputFloat("Mass", &o->fPhysicsMass, 1.0f, 10.0f, decimalprecision)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							agk::SetSpritePhysicsMass(o->m_iSprite, o->fPhysicsMass);
						}
						if (ImGui::InputFloat("Mass Center X", &o->fPhysicsCOMX, 0.01f, 0.10f, decimalprecision)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							agk::SetSpritePhysicsCOM(o->m_iSprite, o->fPhysicsCOMX, o->fPhysicsCOMY);
						}
						if (ImGui::InputFloat("Mass Center Y", &o->fPhysicsCOMY, 0.01f, 0.10f, decimalprecision)) {
							if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
								m_ActiveScene->m_SceneEditor->filechanged = true;
							agk::SetSpritePhysicsCOM(o->m_iSprite, o->fPhysicsCOMX, o->fPhysicsCOMY);
						}
					}

					
				}
				//Can Rotate
				//Is Bullet
				//SetSpritePhysicsCanRotate( iSpriteIndex, rotate )
				//agk::SetSpritePhysicsMass();
				//SetSpritePhysicsMass(iSpriteIndex, mass)


				//Getting to cluttered, moved to scene manager.
/*
				if (ImGui::Checkbox("Debug Physics in Testmode", &w->bPhysicsDebug)) {
					if (w->bPhysicsDebug)
						agk::SetPhysicsDebugOn();
					else
						agk::SetPhysicsDebugOff();
				}


				if (ImGui::SliderInt("Physics Max Polygon Points", &w->iPhysicsMaxPolygonPoints, 2, 12)) {
					agk::SetPhysicsMaxPolygonPoints(w->iPhysicsMaxPolygonPoints);
				}
				

				if (w->bUsePercentage) {
					//Size
					float percentx = (w->fPhysicsGravityX / (float)w->iBaseWidth)*100.0f;
					float percenty = (w->fPhysicsGravityY / (float)w->iBaseHeight)*100.0f;
					if (ImGui::InputFloat("Scene Gravity X", &percentx, 1.0f, 10.0f, decimalprecision)) {
						w->fPhysicsGravityX = (w->iBaseWidth / 100.0f) * percentx;
						agk::SetPhysicsGravity((w->fPhysicsGravityX / (float)w->iBaseWidth)*100.0f, (w->fPhysicsGravityY / (float)w->iBaseHeight)*100.0f);
					}
					if (ImGui::InputFloat("Scene Gravity Y", &percenty, 1.0f, 10.0f, decimalprecision)) {
						w->fPhysicsGravityY = (w->iBaseHeight / 100.0f) * percenty;
						agk::SetPhysicsGravity((w->fPhysicsGravityX / (float)w->iBaseWidth)*100.0f, (w->fPhysicsGravityY / (float)w->iBaseHeight)*100.0f);
					}
				}
				else {
					if (ImGui::InputFloat("Scene Gravity X", &w->fPhysicsGravityX, 1.0f, 10.0f, decimalprecision)) {
						agk::SetPhysicsGravity(w->fPhysicsGravityX, w->fPhysicsGravityY);
					}
					if (ImGui::InputFloat("Scene Gravity Y", &w->fPhysicsGravityY, 1.0f, 10.0f, decimalprecision)) {
						agk::SetPhysicsGravity(w->fPhysicsGravityX, w->fPhysicsGravityY);
					}
				}
				if (ImGui::Checkbox("Scene Physics Wall Left", &w->bPhysicsWallLeft)) {
					if (w->bPhysicsWallLeft)
						agk::SetPhysicsWallLeft(1);
					else
						agk::SetPhysicsWallLeft(0);
				}
				ImGui::SameLine();
				if (ImGui::Checkbox("Right", &w->bPhysicsWallRight)) {
					if (w->bPhysicsWallRight)
						agk::SetPhysicsWallRight(1);
					else
						agk::SetPhysicsWallRight(0);
				}

				if (ImGui::Checkbox("Scene Physics Wall Top", &w->bPhysicsWallTop)) {
					if (w->bPhysicsWallTop)
						agk::SetPhysicsWallTop(1);
					else
						agk::SetPhysicsWallTop(0);
				}
				ImGui::SameLine();
				if (ImGui::Checkbox("Bottom", &w->bPhysicsWallBottom)) {
					if (w->bPhysicsWallBottom)
						agk::SetPhysicsWallBottom(1);
					else
						agk::SetPhysicsWallBottom(0);
				}


				//SetPhysicsGravity( x, y )
				//SetPhysicsWallRight(mode)
				//SetPhysicsWallLeft(mode)

				ImGui::Separator();
*/
			}

			ImGui::Separator();
			//agk::SetPhysicsDebugOn();

			ImGui::Checkbox("Snap To Grid", &w->bSnapToGrid);
			ImGui::SameLine();
			ImGui::Checkbox("Snap Resize To Grid", &w->bSnapResizeToGrid);

			ImGui::PushItemWidth(ImGui::GetFontSize()*4.0f);
			ImGui::InputFloat("Grid X           ", &m_ActiveScene->fGridX, 0.0f, 0.0f, decimalprecision);
			ImGui::PopItemWidth();
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Value");
			ImGui::SameLine();

			ImGui::PushItemWidth(ImGui::GetFontSize()*4.0f);
			ImGui::InputFloat("Grid Y", &m_ActiveScene->fGridY, 0.0f, 0.0f, decimalprecision);
			ImGui::PopItemWidth();
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Y Value");

			ImGui::PushItemWidth(ImGui::GetFontSize()*4.0f);
			ImGui::InputFloat("Grid X Offset", &m_ActiveScene->fGridXOffset, 0.0f, 0.0f, decimalprecision);
			ImGui::PopItemWidth();
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid X Offset Value");
			ImGui::SameLine();

			ImGui::PushItemWidth(ImGui::GetFontSize()*4.0f);
			ImGui::InputFloat("Grid Y Offset", &m_ActiveScene->fGridYOffset, 0.0f, 0.0f, decimalprecision);
			ImGui::PopItemWidth();
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Y Offset Value");
			

			if (m_ActiveScene->fGridX < 4.0f)
				m_ActiveScene->fGridX = 4.0f;
			if (m_ActiveScene->fGridY < 4.0f)
				m_ActiveScene->fGridY = 4.0f;

			if (ImGui::Button("UnLock All Sprites")) {
				if (m_ActiveScene && m_ActiveScene->m_SceneEditor)
					m_ActiveScene->m_SceneEditor->filechanged = true;
				MediaScene::MediaObject * m_pSearchObj = NULL;
				m_pSearchObj = m_ActiveScene->m_pFirstObject;
				if (m_pSearchObj) {
					while (m_pSearchObj) {
						m_pSearchObj->bLocked = false;
						m_pSearchObj = m_pSearchObj->m_pNext;
					}
				}
			}



		}
	}
}


void MediaScene::CopyObject(MediaScene::MediaObject *Obj)
{
	if (!Obj)
		return;
	//Setup copy object.
	m_pCopyObject.m_bRemoved = Obj->m_bRemoved;
	m_pCopyObject.m_Color = Obj->m_Color;
	m_pCopyObject.m_Depth = Obj->m_Depth;
	m_pCopyObject.m_iImageIndex = Obj->m_iImageIndex;

	m_pCopyObject.m_iImageId = Obj->m_iImageId;
	m_pCopyObject.m_iMediaType = Obj->m_iMediaType;
//	m_pCopyObject.m_iSprite = Obj->m_iSprite;
	m_pCopyObject.m_Layer = Obj->m_Layer;
	m_pCopyObject.bAnimated = Obj->bAnimated;
	m_pCopyObject.iAnimationFrom = Obj->iAnimationFrom;
	m_pCopyObject.iAnimationTo = Obj->iAnimationTo;
	m_pCopyObject.m_pUsedImage = Obj->m_pUsedImage;

//	m_pCopyObject.m_pNext = Obj->m_pNext;
	m_pCopyObject.m_Position = Obj->m_Position;

	m_pCopyObject.m_Rotation = Obj->m_Rotation;
	m_pCopyObject.m_Scale = Obj->m_Scale;
	m_pCopyObject.m_Size = Obj->m_Size;
	m_pCopyObject.m_sMediaFullPath = Obj->m_sMediaFullPath;
	m_pCopyObject.m_sMediaRelativePath = Obj->m_sMediaRelativePath;

	m_pCopyObject.bLocked = Obj->bLocked;
	m_pCopyObject.bOnlyVisibleInAGK = Obj->bOnlyVisibleInAGK;
	
	m_pCopyObject.m_sArrayGroup = Obj->m_sArrayGroup;

	m_pCopyObject.m_Text = Obj->m_Text;

	m_pCopyObject.bUVOffset = Obj->bUVOffset;
	m_pCopyObject.fUVSpeedX = Obj->fUVSpeedX;
	m_pCopyObject.fUVSpeedY = Obj->fUVSpeedY;

	m_pCopyObject.bEnablePhysics = Obj->bEnablePhysics;
	m_pCopyObject.iPhysicsShape = Obj->iPhysicsShape;
	m_pCopyObject.iPhysicsMode = Obj->iPhysicsMode;
	m_pCopyObject.bPhysicsCanRotate = Obj->bPhysicsCanRotate;
	m_pCopyObject.bPhysicsIsBullet = Obj->bPhysicsIsBullet;


	m_pCopyObject.fPhysicsCOMX = Obj->fPhysicsCOMX;
	m_pCopyObject.fPhysicsCOMY = Obj->fPhysicsCOMY;
	m_pCopyObject.fPhysicsMass = Obj->fPhysicsMass;
	m_pCopyObject.fPhysicsFriction = Obj->fPhysicsFriction;
	m_pCopyObject.fPhysicsLinearDamping = Obj->fPhysicsLinearDamping;
	m_pCopyObject.fPhysicsAngularDamping = Obj->fPhysicsAngularDamping;

	m_pCopyObject.m_iFontId = Obj->m_iFontId;
	m_pCopyObject.m_iLoadFontId = Obj->m_iLoadFontId;
	m_pCopyObject.m_bVisible = Obj->m_bVisible;

	m_pCopyObject.m_BorderColor = Obj->m_BorderColor;
	m_pCopyObject.m_iBorderSize = Obj->m_iBorderSize;

	m_pCopyObject.m_TextColor = Obj->m_TextColor;
	m_pCopyObject.m_CursorColor = Obj->m_CursorColor;
	m_pCopyObject.m_iCursorSize = Obj->m_iCursorSize;

	m_pCopyObject.m_iTextSize = Obj->m_iTextSize;

	m_pCopyObject.m_iImageDownId = Obj->m_iImageDownId;
	m_pCopyObject.m_iImageDownIndex = Obj->m_iImageDownIndex;
	m_pCopyObject.m_pUsedDownImage = Obj->m_pUsedDownImage;


	bCopyObjectActive = true;
}

MediaScene::MediaObject * MediaScene::PasteObject(void)
{
	if (!bCopyObjectActive)
		return NULL;

	//Find last object.
	MediaScene::MediaObject * m_pSearchObj = NULL;
	MediaScene::MediaObject * m_pLastObj = NULL;
	m_pSearchObj = m_pFirstObject;
	if (m_pSearchObj) {
		while (m_pSearchObj) {
			m_pLastObj = m_pSearchObj;
			m_pSearchObj = m_pSearchObj->m_pNext;
		}
	}

	if (m_pLastObj) {
		//Create new object.
		MediaScene::MediaObject * newobj = new MediaScene::MediaObject();

		newobj->m_bRemoved = m_pCopyObject.m_bRemoved;
		newobj->m_bVisible = m_pCopyObject.m_bVisible;
		newobj->m_Color = m_pCopyObject.m_Color;
		newobj->m_Depth = m_pCopyObject.m_Depth;
		newobj->m_iImageIndex = m_pCopyObject.m_iImageIndex;

		newobj->m_iImageId = m_pCopyObject.m_iImageId;
		newobj->m_iMediaType = m_pCopyObject.m_iMediaType;
		if (newobj->m_iMediaType == MEDIATYPE_BUTTON) {
			newobj->m_iSprite = iButtonCount;
			agk::AddVirtualButton(iButtonCount, 0, 0, 80);
			agk::SetVirtualButtonText(iButtonCount, m_pCopyObject.m_Text);
			if(newobj->m_iImageIndex > 0)
				agk::SetVirtualButtonImageUp(iButtonCount, newobj->m_iImageId);
			if (m_pCopyObject.m_iImageDownIndex > 0)
				agk::SetVirtualButtonImageDown(iButtonCount, m_pCopyObject.m_iImageDownId);
		}
		else if (newobj->m_iMediaType == MEDIATYPE_EDITBOX) {
			newobj->m_iSprite = agk::CreateEditBox();
			agk::AddVirtualButton(iButtonCount, 0, 0, 80);
		}
		else if (newobj->m_iMediaType == MEDIATYPE_TEXT) {
			newobj->m_iSprite = agk::CreateText(m_pCopyObject.m_Text);
		}
		else {
			newobj->m_iSprite = agk::CreateSprite(newobj->m_iImageId);
		}
		newobj->m_Layer = m_pCopyObject.m_Layer;
		newobj->bAnimated = m_pCopyObject.bAnimated;
		newobj->iAnimationFrom = m_pCopyObject.iAnimationFrom;
		newobj->iAnimationTo = m_pCopyObject.iAnimationTo;

		newobj->m_pUsedImage = m_pCopyObject.m_pUsedImage;
		newobj->bNotProcessed = false;
		newobj->m_pNext = NULL;

		newobj->m_Position = m_pCopyObject.m_Position;
		newobj->m_Position.x += 20.0f;
		newobj->m_Position.y += 20.0f;

		newobj->m_Rotation = m_pCopyObject.m_Rotation;
		newobj->m_Scale = m_pCopyObject.m_Scale;
		newobj->m_Size = m_pCopyObject.m_Size;
		newobj->m_sMediaFullPath = m_pCopyObject.m_sMediaFullPath;
		newobj->m_sMediaRelativePath = m_pCopyObject.m_sMediaRelativePath;

		newobj->m_sArrayGroup = m_pCopyObject.m_sArrayGroup;
		newobj->m_Text = m_pCopyObject.m_Text;

		newobj->bUVOffset = m_pCopyObject.bUVOffset;
		newobj->fUVSpeedX = m_pCopyObject.fUVSpeedX;
		newobj->fUVSpeedY = m_pCopyObject.fUVSpeedY;

		newobj->bEnablePhysics = m_pCopyObject.bEnablePhysics;
		newobj->iPhysicsShape = m_pCopyObject.iPhysicsShape;
		newobj->iPhysicsMode = m_pCopyObject.iPhysicsMode;
		newobj->bPhysicsCanRotate = m_pCopyObject.bPhysicsCanRotate;
		newobj->bPhysicsIsBullet = m_pCopyObject.bPhysicsIsBullet;

		newobj->fPhysicsCOMX = m_pCopyObject.fPhysicsCOMX;
		newobj->fPhysicsCOMY = m_pCopyObject.fPhysicsCOMY;
		newobj->fPhysicsMass = m_pCopyObject.fPhysicsMass;
		newobj->fPhysicsFriction = m_pCopyObject.fPhysicsFriction;
		newobj->fPhysicsLinearDamping = m_pCopyObject.fPhysicsLinearDamping;
		newobj->fPhysicsAngularDamping = m_pCopyObject.fPhysicsAngularDamping;

		newobj->m_iFontId = m_pCopyObject.m_iFontId;
		newobj->m_iLoadFontId = m_pCopyObject.m_iLoadFontId;

		newobj->m_BorderColor = m_pCopyObject.m_BorderColor;
		newobj->m_iBorderSize = m_pCopyObject.m_iBorderSize;
		newobj->m_TextColor = m_pCopyObject.m_TextColor;
		newobj->m_CursorColor = m_pCopyObject.m_CursorColor;
		newobj->m_iCursorSize = m_pCopyObject.m_iCursorSize;

		newobj->m_iTextSize = m_pCopyObject.m_iTextSize;


		newobj->m_iImageDownId = m_pCopyObject.m_iImageDownId;
		newobj->m_iImageDownIndex = m_pCopyObject.m_iImageDownIndex;
		newobj->m_pUsedDownImage = m_pCopyObject.m_pUsedDownImage;


		bool found = true;
		while (found) {
			found = false;
			uString sName;
			
			if (newobj->m_iMediaType == MEDIATYPE_BUTTON) {
				sName = "Button ";
				sName.AppendInt(iButtonCount++);
			}
			else if (newobj->m_iMediaType == MEDIATYPE_EDITBOX) {
				sName = "Edit Box ";
				sName.AppendInt(iEditBoxCount++);
			}
			else if (newobj->m_iMediaType == MEDIATYPE_TEXT) {
				sName = "Text ";
				sName.AppendInt(iTextCount++);
			}
			else {
				sName = "Sprite ";
				sName.AppendInt(iSpriteCount++);
			}
			newobj->m_sName = sName;

			m_pSearchObj = m_pFirstObject;
			if (m_pSearchObj) {
				while (m_pSearchObj) {
					if (m_pSearchObj->m_sName.CompareTo(sName) == 0) {
						found = true;
						break;
					}
					m_pSearchObj = m_pSearchObj->m_pNext;
				}
			}
		}

		newobj->m_sVariable = "";

		newobj->bLocked = m_pCopyObject.bLocked;
		newobj->bOnlyVisibleInAGK = m_pCopyObject.bOnlyVisibleInAGK;

		//Add the new image to the chain.
		m_pLastObj->m_pNext = newobj;

		AddUndo(newobj, true);
		UpdateObject(newobj);
		m_pSelectedObject = newobj;

		return newobj;
	}
	return NULL;
}


void MediaScene::UpdateObject(MediaScene::MediaObject *Obj, bool force )
{
	if (Obj->m_bRemoved)
		return;


	if (Obj->m_iMediaType == MEDIATYPE_EDITBOX) {
		if (bUsePercentage) {
			agk::SetEditBoxPosition(Obj->m_iSprite, (Obj->m_Position.x / (float)iBaseWidth)*100.0f, (Obj->m_Position.y / (float)iBaseHeight)*100.0f);
		}
		else {
			agk::SetEditBoxPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
		}
		agk::SetEditBoxBackgroundColor(Obj->m_iSprite, (int) Obj->m_Color.x, (int) Obj->m_Color.y, (int) Obj->m_Color.z, (int) Obj->m_Color.w);
		agk::SetEditBoxTextColor(Obj->m_iSprite, (int) Obj->m_TextColor.x, (int) Obj->m_TextColor.y, (int) Obj->m_TextColor.z);

		agk::SetEditBoxBorderColor(Obj->m_iSprite, (int) Obj->m_BorderColor.x, (int) Obj->m_BorderColor.y, (int) Obj->m_BorderColor.z, (int) Obj->m_BorderColor.w);
		agk::SetEditBoxCursorColor(Obj->m_iSprite, (int) Obj->m_CursorColor.x, (int) Obj->m_CursorColor.y, (int) Obj->m_CursorColor.z);

		agk::SetEditBoxDepth(Obj->m_iSprite, Obj->m_Depth);

		if (bUsePercentage) {
			agk::SetEditBoxSize(Obj->m_iSprite, (Obj->m_Size.x / (float)iBaseWidth)*100.0f, (Obj->m_Size.y / (float)iBaseHeight)*100.0f);
		}
		else {
			agk::SetEditBoxSize(Obj->m_iSprite, Obj->m_Size.x, Obj->m_Size.y);
		}


		if (Obj->m_iTextSize > 0) {
			if (bUsePercentage) {
				agk::SetEditBoxTextSize(Obj->m_iSprite, (Obj->m_iTextSize / (float)iBaseHeight)*100.0f);
			}
			else {
				agk::SetEditBoxTextSize(Obj->m_iSprite, (float) Obj->m_iTextSize);
			}
		}
		else {
			agk::SetEditBoxTextSize(Obj->m_iSprite, Obj->m_Size.y);
		}

		if (bUsePercentage) {
			agk::SetEditBoxBorderSize(Obj->m_iSprite, (Obj->m_iBorderSize / (float)iBaseWidth)*100.0f);
		}
		else {
			agk::SetEditBoxBorderSize(Obj->m_iSprite, (float) Obj->m_iBorderSize);
		}
		if (bUsePercentage) {
			agk::SetEditBoxCursorWidth(Obj->m_iSprite, (Obj->m_iCursorSize / (float)iBaseWidth)*100.0f);
		}
		else {
			agk::SetEditBoxCursorWidth(Obj->m_iSprite, (float) Obj->m_iCursorSize);
		}

	

//		if (bTestMode)
//			agk::SetEditBoxVisible(Obj->m_iSprite, 1);
//		else
//			agk::SetEditBoxVisible(Obj->m_iSprite, 0);
	}

		
	if (Obj->m_iMediaType == MEDIATYPE_BUTTON) {
		if (bUsePercentage) {
			agk::SetVirtualButtonPosition(Obj->m_iSprite, (Obj->m_Position.x / (float)iBaseWidth)*100.0f, (Obj->m_Position.y / (float)iBaseHeight)*100.0f);
		}
		else {
			agk::SetVirtualButtonPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
		}
		agk::SetVirtualButtonAlpha(Obj->m_iSprite, (int) Obj->m_Color.w);
		agk::SetVirtualButtonColor(Obj->m_iSprite, (int) Obj->m_Color.x, (int) Obj->m_Color.y, (int) Obj->m_Color.z);
		//agk::SetVirtualButtonDepth(Obj->m_iSprite, Obj->m_Depth);

		if (bUsePercentage) {
//			agk::SetVirtualButtonSize(Obj->m_iSprite, (Obj->m_Size.x / (float)iBaseWidth)*100.0f, (Obj->m_Size.y / (float)iBaseHeight)*100.0f);
			//Strange like it only follow width ?
			agk::SetVirtualButtonSize(Obj->m_iSprite, (Obj->m_Size.x / (float)iBaseWidth)*100.0f, (Obj->m_Size.y / (float)iBaseWidth)*100.0f);
		}
		else {
			agk::SetVirtualButtonSize(Obj->m_iSprite, Obj->m_Size.x, Obj->m_Size.y);
		}
		if (bTestMode)
			agk::SetVirtualButtonVisible(Obj->m_iSprite, 1);
		else
			agk::SetVirtualButtonVisible(Obj->m_iSprite, 0);

//		if(Obj->m_iImageIndex > 0)
//			agk::SetVirtualButtonImageUp(Obj->m_iSprite, Obj->m_iImageId);
//		else
//			agk::SetVirtualButtonImageUp(Obj->m_iSprite, 0);

	}

	if (Obj->m_iMediaType == MEDIATYPE_TEXT) {

		//agk::SetTextString(Obj->m_iSprite, "");
		//agk::SetTextString(Obj->m_iSprite, Obj->m_Text.GetStr());

		if (bUsePercentage) {
			agk::SetTextPosition(Obj->m_iSprite, (Obj->m_Position.x / (float)iBaseWidth)*100.0f, (Obj->m_Position.y / (float)iBaseHeight)*100.0f);
		}
		else {
			agk::SetTextPosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
		}
		if (bUsePercentage) {
			agk::SetTextSize( Obj->m_iSprite, (Obj->m_Size.y / (float)iBaseHeight)*100.0f);
		}
		else {
			agk::SetTextSize(Obj->m_iSprite, Obj->m_Size.y);
		}
		agk::SetTextColor(Obj->m_iSprite, (int) Obj->m_Color.x, (int) Obj->m_Color.y, (int) Obj->m_Color.z, (int) Obj->m_Color.w);

		agk::SetTextAngle(Obj->m_iSprite, Obj->m_Rotation.x);

		float tsize = Obj->m_Size.y * Obj->m_Text.GetLength() * 0.4f;


		if( Obj->m_iFontId > 0) {

			//Check font size.
			if (bUsePercentage) {
				tsize = (iBaseWidth / 100.0f) * agk::GetTextTotalWidth(Obj->m_iSprite) * 0.7f;
			}
			else {
				tsize = agk::GetTextTotalWidth(Obj->m_iSprite);
			}

//			if (customfonts[Obj->m_iFontId] && customfonts[Obj->m_iFontId]->FontSize > 0.0f) {
//				ImGui::PushFont(customfonts[Obj->m_iFontId]);
//				float fwidth = ImGui::CalcTextSize("#").x;
//				tsize = (fwidth * Obj->m_Text.GetLength()); //*Obj->m_Size.y * 0.1f;
//				ImGui::PopFont();
//			}

		}

//		if (bUsePercentage) {
//			float gtw = agk::GetTextTotalWidth(Obj->m_iSprite);
//			float add = (30.0f / (float)iBaseWidth)*100.0f;
//			Obj->m_Size.x = iBaseWidth / 100.0f*(gtw + add);
//		}
//		else {
			Obj->m_Size.x = tsize + 30.0f;
//		}

/*
		if (bUsePercentage) {
			float gtw = agk::GetTextTotalWidth(Obj->m_iSprite);
			float add = (30.0f / (float)iBaseWidth)*100.0f;
			Obj->m_Size.x = iBaseWidth/100.0f*(gtw + add);
		}
		else {
			Obj->m_Size.x = agk::GetTextTotalWidth(Obj->m_iSprite) + 30.0f;
		}
*/


		agk::SetTextDepth(Obj->m_iSprite, Obj->m_Depth);

		if (bTestMode)
			agk::SetTextVisible(Obj->m_iSprite, 1);
		else
			agk::SetTextVisible(Obj->m_iSprite, 0);


		if (Obj->m_iFontId != 0 && Obj->m_iLoadFontId == 0) {

			std::map<std::int32_t, std::string>::iterator it = Fonts.find(Obj->m_iFontId);
			if (it != Fonts.end()) {
				uString FontPath = it->second.c_str();
				FontPath.ReplaceStr("\\", "/");

				uString agk_path = FontPath;
				int pos = FontPath.FindStr("/media/", 1);
				if (pos > 0) {
					FontPath.SubString(agk_path, pos+7);
				}

				uString sIName;
				sIName = "raw:";
				sIName.Append(FontPath);
				if (!agk::GetFileExists(sIName.GetStr())) {
					sIName = agk_path;
					if (renderingProject) {
						uString nfp = renderingProject->m_sProjectFileFullPath;
						nfp.ReplaceStr("\\", "/");
						int pos = nfp.RevFindStr("/");
						if (pos > 0) {
							uString tmp;
							nfp.SubString(tmp, 0, pos);
							tmp.Append("/media/");
							tmp.Append(agk_path);
							sIName = "raw:";
							sIName.Append(tmp);
						}

//						if (agk::GetFileExists(sIName.GetStr())) {
//							//Update to new fullpath.
//							Image->m_sFullPath = sIName;
//						}
					}

				}
				if( !agk::GetFontExists(Obj->m_iAgkFontId) ) {
					Obj->m_iAgkFontId = agk::LoadFont(sIName.GetStr());
				}
				Obj->m_iLoadFontId = Obj->m_iAgkFontId;
			}

		}
		if (Obj->m_iLoadFontId > 0) {
			agk::SetTextFont(Obj->m_iSprite, Obj->m_iLoadFontId);
		}



	}
	if (Obj->m_iMediaType == MEDIATYPE_SPRITE) {
		if ( (!(Obj->bEnablePhysics && bTestMode)) || force  ) { //dont update when using physics and in testmode. || force
			if (Obj->bStretchToDevice) {

				if (bFollowDeviceResolution) {
					Obj->m_Position.x = 0.0f;
					Obj->m_Position.y = 0.0f;
					Obj->m_Size.x = (float) iScreenWidth;
					Obj->m_Size.y = (float) iScreenHeight;
					Obj->m_Rotation.x = 0.0f;
					Obj->m_Scale.x = 1.0f;
					Obj->m_Scale.y = 1.0f;

				}
				else {
					float scw = fScissorX*fScaleToScreenX;
					float sch = fScissorY*fScaleToScreenY;
					Obj->m_Position.x = -scw; // -m_iDisplayExtraX
					Obj->m_Position.y = -sch;
					Obj->m_Size.x = iBaseWidth + scw + scw;
					Obj->m_Size.y = iBaseHeight + sch + sch;
					Obj->m_Rotation.x = 0.0f;
					Obj->m_Scale.x = 1.0f;
					Obj->m_Scale.y = 1.0f;
				}

			}

			if (bUsePercentage) {
				agk::SetSpritePosition(Obj->m_iSprite, (Obj->m_Position.x / (float) iBaseWidth)*100.0f, (Obj->m_Position.y / (float)iBaseHeight)*100.0f );
				agk::SetSpriteSize(Obj->m_iSprite, (Obj->m_Size.x / (float)iBaseWidth)*100.0f, (Obj->m_Size.y / (float)iBaseHeight)*100.0f );
			}
			else {
				agk::SetSpritePosition(Obj->m_iSprite, Obj->m_Position.x, Obj->m_Position.y);
				agk::SetSpriteSize(Obj->m_iSprite, Obj->m_Size.x, Obj->m_Size.y);
			}
			agk::SetSpriteScale(Obj->m_iSprite, Obj->m_Scale.x, Obj->m_Scale.y);
			agk::SetSpriteAngle(Obj->m_iSprite, Obj->m_Rotation.x);

			agk::SetSpriteDepth(Obj->m_iSprite, Obj->m_Depth);
			agk::SetSpriteColor(Obj->m_iSprite, (int) Obj->m_Color.x, (int) Obj->m_Color.y, (int) Obj->m_Color.z, (int) Obj->m_Color.w);

			if (Obj->iAnimationFrom > 0 && Obj->m_pUsedImage) {
				//Setup as animation.
				int fw = (int)Obj->m_pUsedImage->m_iWidth / Obj->m_pUsedImage->m_iRows;
				int fh = (int)Obj->m_pUsedImage->m_iHeight / Obj->m_pUsedImage->m_iColumns;

				agk::SetSpriteAnimation(Obj->m_iSprite, fw, fh, Obj->m_pUsedImage->m_iRows*Obj->m_pUsedImage->m_iColumns);
				agk::SetSpriteFrame(Obj->m_iSprite, Obj->iAnimationFrom);
				agk::SetSpriteUVBorder(Obj->m_iSprite, 0.5f); // 1.0f //0.0f
			}

			//if (m_pSelectedImage) {
				for (int i = 0; i < 11; i++) {
					if (Obj->m_Depth == iLayerDepth[i]) {
						if (iLayerOpacity[i] != 255) {
							agk::SetSpriteColor(Obj->m_iSprite, (int) Obj->m_Color.x, (int) Obj->m_Color.y, (int) Obj->m_Color.z, iLayerOpacity[i]);
						}
						if (!bLayerVisible[i])
							agk::SetSpriteColor(Obj->m_iSprite, (int) Obj->m_Color.x, (int) Obj->m_Color.y, (int) Obj->m_Color.z, 0);
					}
				}
			//}
		}
	}
}


void MediaScene::ProcessAtlas(TextEditor * m_editor)
{
	if (!bProcessAtlas)
		return;
	if (!m_pSelectedImage)
		return;

	ImGui::OpenPopup("Setup Sprite Sheet.");
	if (ImGui::BeginPopupModal("Setup Sprite Sheet.", &bProcessAtlas, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Sprite Sheet Setup:");
		ImGui::InputInt("Sprite Sheet Rows: ", &m_pSelectedImage->m_iColumns, 8, 32);
		ImGui::InputInt("Sprite Sheet Columns: ", &m_pSelectedImage->m_iRows, 8, 32);

		if (ImGui::Button(" Cancel ")) {
			m_pSelectedImage = NULL;
			bProcessAtlas = false;
		}
		ImGui::SameLine();
		if (ImGui::Button(" Add Sprite Sheet ")) {
			if (m_editor)
				m_editor->filechanged = true;
			m_pSelectedImage->m_bAtlas = true;
			if (m_pSelectedImage->m_iColumns < 1)
				m_pSelectedImage->m_iColumns = 1;
			if (m_pSelectedImage->m_iRows < 1)
				m_pSelectedImage->m_iRows = 1;

			agk::SetImageWrapU(m_pSelectedImage->m_iImageId, 0);
			agk::SetImageWrapV(m_pSelectedImage->m_iImageId, 0);
			m_pSelectedImage = NULL;
			bProcessAtlas = false;
		}
		ImGui::EndPopup();
	}

	//m_pSelectedImage

}



float MediaScene::AlignXtoGrid(float x, bool bsize)
{
	if (bSnapResizeToGrid) {
		int align = (int) (round(x) / fGridX);
		if (x < 0.0f)
			align -= 1;
		x = align * fGridX;
		x += fGridXOffset;
		if (bsize && x < fGridX + fGridXOffset)
			x = fGridX + fGridXOffset;
	}
	return x;
}
float MediaScene::AlignYtoGrid(float y, bool bsize)
{
	if (bSnapResizeToGrid) {
		int align = (int) (round(y) / fGridX);
		if (y < 0.0f)
			align -= 1;
		y = align * fGridX;
		y += fGridXOffset;
		if (bsize && y < fGridX + fGridXOffset)
			y = fGridX + fGridXOffset;
	}
	return y;
}

