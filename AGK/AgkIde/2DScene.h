#pragma once

#include "agk.h"
#include "TextEditor.h"


#define MEDIATYPE_SPRITE 1
#define MEDIATYPE_TEXT 2
#define MEDIATYPE_BUTTON 3
#define MEDIATYPE_EDITBOX 4
#define RESETZOOM 0.0

#define MAXUSERFONTS 20

class MediaScene
{
public:
	struct Vec4
	{
		float x;
		float y;
		float z;
		float w;
	};
	struct Resolutions
	{
		int m_iWidth;
		int m_iHeight;
		bool m_bOrintation;
		uString m_sName;
	};
	struct Images
	{
		int m_iImageId;
		int m_iWidth;
		int m_iHeight;
		int m_iImageIndex;
		bool m_bRemoved;
		bool m_bAtlas = false;
		int m_iRows = 1;
		int m_iColumns = 1;
		uString m_sName;
		uString m_sRelativePath;
		uString m_sFullPath;
		Images * m_pNext = NULL;
	};
	struct MediaObject
	{
		uString m_sName;
		uString m_sMediaFullPath;
		uString m_sMediaRelativePath;
		int m_iMediaType = MEDIATYPE_SPRITE;
		Vec4 m_Position;
		Vec4 m_Rotation;
		Vec4 m_Scale;
		Vec4 m_Color;
		Vec4 m_Size;
		int m_iSprite;
		bool m_bRemoved;
		bool m_bVisible = true;
		int m_iImageId;
		int m_iImageIndex;
		int m_Layer = 0;
		int m_Depth;
		float oposx;
		float oposy;
		bool bIsSelected = false;
		bool bLocked = false;
		bool bOnlyVisibleInAGK = false;
		bool bAnimated = false;
		int iAnimationFrom = 0;
		int iAnimationTo = 0;
		bool bStretchToDevice = false;
		bool bFixToScreen = false;
		bool bUVOffset = false;
		float fUVSpeedX = 0.0;
		float fUVSpeedY = 0.0;
		float fUVCountX = 0.0;
		float fUVCountY = 0.0;
		bool bNotProcessed = false;
		bool bEnablePhysics = false;
		int iPhysicsShape = 3;
		int iPhysicsMode = 0;
		bool bPhysicsCanRotate = true;
		bool bPhysicsIsBullet = false;
		float fPhysicsRestitution = 0.0;
		float fPhysicsCOMX = 0.0;
		float fPhysicsCOMY = 0.0;
		float fPhysicsMass = 100.0;
		float fPhysicsFriction = 0.5;
		float fPhysicsLinearDamping = 0.0;
		float fPhysicsAngularDamping = 0.0;
		bool bPhysicsInit = false;
		Vec4 m_OldPosition;
		Vec4 m_OldColor;
		Vec4 m_TextColor = { 0,0,0,255 };
		Vec4 m_BorderColor = { 0,0,0,255 };
		Vec4 m_CursorColor = { 0,0,0,255 };
		int m_iBorderSize = 2;
		int m_iCursorSize = 2;
		int m_iTextSize = 0;
		int m_iEditMaxChars = 0;
		bool bEditMultiLines = false;
		bool bEditPassword = false;
		bool bEditWrap = false;
		bool bUndoAdded = false;
		uString m_sVariable;
		uString m_sArrayGroup;
		uString m_Text;
		//uString m_sFont;
		int m_iFontId;
		int m_iAgkFontId;
		int m_iLoadFontId;
		int m_iImageDownId;
		int m_iImageDownIndex;
		Images * m_pUsedImage = NULL;
		Images * m_pUsedDownImage = NULL;
		MediaObject * m_pNext = NULL;
	};
	struct MediaObjectUndo
	{
		MediaObject * m_pObj = NULL;
		Vec4 m_Position;
		Vec4 m_Rotation;
		Vec4 m_Scale;
		Vec4 m_Color;
		Vec4 m_Size;
		bool m_bRemoved;
		int m_Layer;
		int m_Depth;
		int m_count;
	};
	

	uString m_sSceneFullPath;
	uString m_sSceneName;
	bool bDisplayAsText = false;
	int iRenderTarget = 0;
	int iScreenWidth = 1024;
	int iScreenHeight = 768;
	int iBaseWidth = 1024;
	int iBaseHeight = 768;
	bool bFollowDeviceResolution = false;
	//int iDeviceHeight = 0;
	//int iDeviceWidth = 0;
	int iContentWide = 10000;
	int iContentHeight = 10000;
	bool bAGKBorder = true;
	bool bDisplayPhone = true;
	bool bGridOn = true;
	bool bFirstRun = true;
	float zoomAll = RESETZOOM;
	bool bSnapToGrid = false;
	bool bSnapResizeToGrid = false;
	bool bPhysicsDebug = false;
	float fPhysicsGravityX = 0.0;
	float fPhysicsGravityY = 50.0;
	int iPhysicsMaxPolygonPoints = 8;
	bool bPhysicsWallLeft = true;
	bool bPhysicsWallRight = true;
	bool bPhysicsWallTop = true;
	bool bPhysicsWallBottom = true;

	float fGridX = 32.0;
	float fGridY = 32.0;
	float fGridXOffset = 0.0;
	float fGridYOffset = 0.0;
	ImVec4 vGridColor;
	int iSceneBackColor[4] = { 0,0,0,255 };
	char cSceneMenuText[MAX_PATH] = "Scenes";
	char cResolutionMenuText[MAX_PATH] = "L: 1024x768 (1.33)   iPad/iPad 2/iPad Mini";

	bool bLayerVisible[11];
	int iLayerDepth[11];
	int iLayerOpacity[11];
	uString sLayerName[11];
	int iCurrentSelectedLayer = 5;

	std::map<std::string, Resolutions> Resolution;
	std::map<std::int32_t, std::string> Fonts;
	ImFont* customfonts[MAXUSERFONTS];
	MediaObject * m_pFirstObject = NULL;
	Images * m_pFirstImage = NULL;
	MediaScene * m_pNextScene = NULL;
	MediaScene * m_pCurrentScene = NULL;
	MediaObject * m_pSelectedObject = NULL;
	MediaObject * m_pNavObject = NULL;
	MediaObject * m_pPropertiesObject = NULL;
	MediaObject * m_pUnlockObject = NULL;
	MediaObject * m_pUnlockObject2 = NULL;
	MediaObject * m_pUnlockObject3 = NULL;
	MediaObjectUndo m_PropertiesUndo;
	MediaObject * m_pChangingObject = NULL;
	MediaObject * m_pChangeTo = NULL;
	MediaObject m_pCopyObject;
	bool bCopyObjectActive = false;
	bool bFirstSelection = false;
	TextEditor * m_SceneEditor;
	int iImageCount = 1;
	int iSpriteCount = 1;
	int iTextCount = 1;
	int iButtonCount = 1;
	int iEditBoxCount = 1;
	int iActiveSpriteCount = 1;
	int iFontCount = 1;
	int iSelectedImageId = 0;
	Images * m_pSelectedImage = NULL;
	int m_iSelectedSubImage = 0;
	int iDelayAction = 0;
	bool bDisabled = false;
	bool add_new_resolution = false;
	bool bMousePresset = false;
	bool bMiddleMousePresset = false;
	ImVec2 vMouseStart;
	bool bIsAnythingHovered = false;
	int iDelayChanges = 0;
	bool bInChangingMode = false;
	bool bIsAnythingSelected = false;
	bool bTestMode = false;
	bool bProcessAtlas = false;
	bool bDrawModeFitGrid = false;
	float fDrawModeSizeX = 0;
	float fDrawModeSizeY = 0;
	bool bUsePercentage = false;
	uString WindowName;

	bool firstmousepress = false;
	bool Multifirstmousepress = false;
	bool mMultipleSelectActive = false;
	int iActiveSelections = 0;
	ImVec2 mmousestartpos;
	ImVec2 mmouseendpos;
	ImVec2 cursoradd;
	bool bCursorChanges = false;
	bool bColorChanged = false;
	Vec4 newColor = { 255,255,255,255 };
	//aspect
	float fScissorX, fScissorXScaled;
	float fScissorY, fScissorYScaled;
	float fStretchValue;
	float fDisplayAspect;
	float fScreenAspect;
	float fRatio;
	float fRatioAuto;

	float centerSceneX;
	float centerSceneY;
	float fCenterX;
	float fCenterY;

	float fScaleToScreenX;
	float fScaleToScreenY;
	int m_iDisplayExtraX;
	int m_iDisplayExtraY;


	bool bMultiSelect = false;
	ImVec2 MultiA[16];

	int iUndoIndex = 0;
	std::map<std::int32_t, MediaObjectUndo *> m_Undo;

	bool Render(const char* aTitle, TextEditor * m_editor, void * m_pCurFile = NULL, MediaScene * m_pMainScene = NULL);
	void RenderScene(void);
	void AddUndo(MediaScene::MediaObject * m_pUndoObj, bool remove, int count = 0);
	bool CanUndo(void);
	bool CanRedo(void);
	bool Undo(void);
	bool Redo(void);
	bool Delete(void);
	void ProcessDragDrop(MediaScene * m_ActiveScene, const char* aTitle, MediaScene * m_pMainScene);
	void GenerateCode( MediaScene * m_pMainScene = NULL, TextEditor * m_editor = NULL,bool save = false);
	void AddSceneSetupFunction(MediaScene * m_pSearchScene, TextEditor * m_editor, MediaScene * m_pMainScene);
	void AddSceneSyncFunction(MediaScene * m_pSearchScene, TextEditor * m_editor, MediaScene * m_pMainScene);
	void AddSceneDialogFunction(MediaScene * m_pSearchScene, TextEditor * m_editor);
	void AddSceneGlobals(MediaScene * m_pSearchScene, TextEditor * m_editor);
	void AddSceneFade(MediaScene * m_pSearchScene, TextEditor * m_editor, MediaScene * m_pMainScene);
	void AddSceneCleanup(MediaScene * m_pSearchScene, TextEditor * m_editor, MediaScene * m_pMainScene);
	void LoadCode(MediaScene * m_pMainScene, TextEditor * m_editor);
	void ProcessNewResolution(MediaScene * m_pMainScene);
	bool DrawAGKSprite(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle, const ImVec4& drawCol_normal = ImColor(255, 255, 255, 255), const ImVec4& drawCol_hover = ImColor(200, 200, 200, 200), bool selected = false, MediaObject *Obj = NULL, bool drawselection = false, bool multipleselect = false);
	void TestSprite(void);
	void TestDraw(void);
	void GetAspect(void);
	float myScreenToWorldY(float y);
	float myScreenToWorldX(float x);
	void CopyObject(MediaScene::MediaObject *obj);
	MediaScene::MediaObject * PasteObject(void);
	void UpdateObject(MediaScene::MediaObject *Obj, bool force = false);
	void ProcessAtlas(TextEditor * m_editor);
	float AlignXtoGrid(float x, bool bsize = false);
	float AlignYtoGrid(float y, bool bsize = false);

	MediaScene();
	~MediaScene();


private:

};

void ProcessSceneManager(MediaScene * m_ActiveScene);
void ProcessSceneProperties(MediaScene * m_ActiveScene);


