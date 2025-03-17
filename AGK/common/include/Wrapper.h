// Wrapper Header
#ifndef _H_WRAPPER_
#define _H_WRAPPER_

// Includes
#include "Common.h"
#include "agk.h"

#define AGK_MAX_VIRTUAL_BUTTONS 101

#define AGK_FLAG_PORTRAIT			0x0001
#define AGK_FLAG_PORTRAIT2			0x0002
#define AGK_FLAG_LANDSCAPE			0x0004
#define AGK_FLAG_LANDSCAPE2			0x0008
#define AGK_FLAG_CAN_RESIZE			0x0010

#define AGK_SENSOR_ACTIVATE_ACCEL	0x01
#define AGK_SENSOR_ACTIVATE_GYRO	0x02
#define AGK_SENSOR_ACTIVATE_PROX	0x04
#define AGK_SENSOR_ACTIVATE_LIGHT	0x08
#define AGK_SENSOR_ACTIVATE_MAG		0x10
#define AGK_SENSOR_ACTIVATE_ROT		0x20

// capability flags
#define AGK_CAP_UINT_INDICES		0x0001

// Bullet physics forward declarations
//class btBroadphaseInterface;
//class btDefaultCollisionConfiguration;
//class btCollisionDispatcher;
//class btSequentialImpulseConstraintSolver;
//class btDiscreteDynamicsWorld;

#define AGK_FILE_TYPE_READ			0x01
#define AGK_FILE_TYPE_WRITE			0x02

class SimplexNoise;

namespace AGK
{
	class MyDestructionListener;

	class cFileEntry
	{
		protected:
			uString m_sReal;
			uString m_sLowercase;
			cFileEntry *m_pNext;

			static cHashedList<cFileEntry*> g_cFileList;
			static void TraverseDirectory( const char* dir );

		public:
			cFileEntry() { m_pNext = 0; }
			~cFileEntry() {}

			static void InitFileList();
			static void AddNewFile( const char *szFullPath );
			static void ConvertToReal( uString &path );
			static void ClearAll();
	};
	
	class cDirectoryItem
	{
		public:
			uString m_sItem;
			cDirectoryItem *m_pNext;
			uint32_t iFlags; 

			cDirectoryItem() { m_pNext = 0; iFlags = 0; }
			~cDirectoryItem() { }
	};

	class cMemblock
	{
		public:
			uint32_t m_iID;
			unsigned char* m_pData;
			uint32_t m_iSize;
			uint32_t m_iUser1;
			uint32_t m_iUser2;
			uint32_t m_iUser3;
			uint32_t m_iUser4;

			cMemblock() { m_iID = 0; m_pData = 0; m_iSize = 0; m_iUser1 = 0xffffffff; m_iUser2 = 0xffffffff; m_iUser3 = 0xffffffff; m_iUser4 = 0xffffffff; }
			~cMemblock() { if ( m_pData ) delete [] m_pData; }
	};

	class agk
	{
		private:
			// this prevents the creation of an instance of this class by other classes.
			agk();

			// this is messy, but we're stuck with it now, it's not worth cleaning up
			friend class cText;
			friend class cSprite;
			friend class cImage;
			friend class Physics;
			friend class cNetCon;
			friend class AGKSocket;
			friend class AGKPacket;
			friend class cFile;
			friend class cMusicMgr;
			friend class cSpriteMgr;
			friend class cSpriteMgrEx;
			friend class cParticleEmitter;
			friend class cSoundMgr;
			friend class cVirtualButton;
			friend class cVirtualJoystick;
			friend class cJoystick;
			friend class cEditBox;
			friend class ZipFile;
			friend class AGKShader;
			friend class cMesh;
			friend class cObject3D;
			friend class cObjectMgr;
			friend class DebugDraw;
			friend class Skeleton2D;
			friend class GLDebugDrawer;
			friend class btIDebugDraw;
			friend class AGKSkyBox;
			friend class AGK3DParticleEmitter;

		protected:

			// *********************
			// internal variables
			// *********************

			static Renderer *g_pRenderer;

			static bool g_bIsExternal;
			static void (*SwapExternal)(void*);
			static void *g_pSwapParam;

			// 3D Physics
			/*static btBroadphaseInterface *m_pBroadphase;
			static btDefaultCollisionConfiguration* m_pCollisionConfiguration;
			static btCollisionDispatcher* m_pDispatcher;
			static btSequentialImpulseConstraintSolver* m_pSolver;
			static btDiscreteDynamicsWorld* m_pDynamicsWorld;*/

			// calculated viewport dimensions
			static float m_fTargetViewportX;
			static float m_fTargetViewportY;
			static float m_fTargetViewportWidth;
			static float m_fTargetViewportHeight;

			static int m_iScissorX;
			static int m_iScissorY;
			static int m_iScissorWidth;
			static int m_iScissorHeight;
			static bool m_bScissorUser;
			static bool m_bScissorEnabled;
			static float m_fScissorUserX;
			static float m_fScissorUserY;
			static float m_fScissorUserX2;
			static float m_fScissorUserY2;

			static uint32_t m_iDefaultFBCleared;

			// app start time in seconds
			static double m_fTimeStart; 
			// current app time in seconds
			static double m_fTimeCurr;
			// time current frame started in seconds
			static double m_fTimeFrameStart;
			static double m_fTimeLimitFrameStart;
			// time spent processing the last frame in seconds 
			static float m_fTimeDelta; 
			static uint32_t m_iTimeMilliseconds;
			static float m_fFps;

			static uint32_t m_iPolygonsDrawn;
			static uint32_t m_iShadowPolygonsDrawn;
			static uint32_t m_iVerticesProcessed;

			// width and height of the device (current state of device target) (i.e. desktop resolution)
			static int m_iRealDeviceWidth;
			static int m_iRealDeviceHeight;

			// current render target size that may not be equal to the device size.
			static int m_iRenderWidth;
			static int m_iRenderHeight;
			
			static int m_iUseNewDefaultFonts;
			
			// fog variables
			static int m_iFogMode; // 0=off, 1=pixel fog
			static uint32_t m_iFogColor1; // used for comparison with m_iFogColor2 only
			static uint32_t m_iFogColor2; // used for comparison with m_iFogColor1 only
			static int m_iFogColor2IsSet; // 1=The user has specifically set m_iFogColor2
			static float m_fFogMinDist;
			static float m_fFogMaxDist;

			// shadow variables
			static int m_iShadowMode; // 0=Off, 1=Uniform, 2=Lightspace Perspective (LiPSM), 3=Cascade
			static int m_iShadowSmoothing; // 0=none, 1=multisampled
			static int m_iShadowMapWidth;
			static int m_iShadowMapHeight;
			static float m_fShadowRange;
			static float m_fShadowBias;
			static float m_fShadowLightStep;
			static float m_fShadowCascade1;
			static float m_fShadowCascade2;
			static float m_fShadowCascade3;
			static AGKFrameBuffer *m_pShadowMapFBO;
			static AGKFrameBuffer *m_pShadowMapFBO2;
			static AGKFrameBuffer *m_pShadowMapFBO3;
			static AGKFrameBuffer *m_pShadowMapFBO4;
			static cImage *m_pShadowMap;
			static cImage *m_pShadowMap2;
			static cImage *m_pShadowMap3;
			static cImage *m_pShadowMap4;
		
			// width and height of the workspace (display), as requested by the user in InitGL()
			static int m_iDisplayWidth;
			static int m_iDisplayHeight;
			static float m_iDisplayExtraX;
			static float m_iDisplayExtraY;
			static float m_fDisplayAspect;
			static int m_iDisplayIsLandscape;
			static bool m_bUsingDisplayAspect;
			static bool m_bUsingDeviceAspect;
			static float m_fStretchValue;
			static int m_iResolutionMode;
			static int m_iExtraAGKPlayerAssetsMode;
			static int m_iOrientation;
			static uint32_t m_bAGKFlags;
			static uint32_t m_iDefaultMagFilter;
			static uint32_t m_iDefaultMinFilter;
			static uint32_t m_iDefaultWrapU;
			static uint32_t m_iDefaultWrapV;
			static uString m_sCompanyName;
			static uString m_sAppName;
			static float m_fClearRed;
			static float m_fClearGreen;
			static float m_fClearBlue;
			static float m_fBorderRed;
			static float m_fBorderGreen;
			static float m_fBorderBlue;
			static bool m_bIsCapturing;
			static uint32_t m_iUncollectedCaptureImage;
            static bool m_bIsChoosing;
            static cImage* m_pUncollectedChosenImage;

			static uint32_t m_iPulleySprite1;
			static uint32_t m_iPulleySprite2;
			static float m_iPulleyRatio;
			static int m_iPulleyColConnected;
			
			static int wasMusicPlaying;
			static int wasVideoPlaying;
			static int m_iPaused;
			static int m_iResumed;
			static int m_iResumed2;

			static int m_iVideoPlayMode;
			static uint32_t m_iVideoTextureRaw;
			static cImage *m_pVideoTexture;
			static cObject3D *m_pVideoTextureQuad;
			static AGKShader *m_pVideoTextureShader;
			static AGKFrameBuffer *m_pVideoTextureFBO;

			static cImage *m_pCameraTexture;
			static int m_iDeviceCameraID;
			static int m_iDeviceCameraType;

			static int m_iSGX540GPU;

			// input variables
			// mouse
			static cMouse *m_pMouse[ 4 ];

			// accelerometer
			static float m_fAccelX;
			static float m_fAccelY;
			static float m_fAccelZ;
			static float m_fFakeAccelX;
			static float m_fFakeAccelY;

			static float m_fGyroX;
			static float m_fGyroY;
			static float m_fGyroZ;

			static float m_fProximityDist;

			static float m_fLightLevel;

			static float m_fMagneticX;
			static float m_fMagneticY;
			static float m_fMagneticZ;
			
			static float m_fRotVecX;
			static float m_fRotVecY;
			static float m_fRotVecZ;
			static float m_fRotVecW;

			static float m_fGPSLong;
			static float m_fGPSLat;
			static float m_fGPSAltitude;

			static uint32_t m_bSensorFlags;

			// keyboard
			#define AGK_MAX_KEYS 512
			static unsigned char m_iPrevKeyDown[ AGK_MAX_KEYS ];
			static unsigned char m_iKeyDown[ AGK_MAX_KEYS ];
			static unsigned char m_iResetKey[ AGK_MAX_KEYS ];
			static unsigned int m_iLastKey;
			static int m_iKeyboardMode;

			// joystick
			static cJoystick* m_pJoystick[ AGK_NUM_JOYSTICKS ];
			static cVirtualJoystick *m_pVirtualJoystick[ 4 ];

			// virtual buttons
			static cVirtualButton *m_pVirtualButton[ AGK_MAX_VIRTUAL_BUTTONS ];

			// pointer state
			static int m_iLastPointerDown;
			static int m_iPointerDown;
			static float m_fPointerX;
			static float m_fPointerY;

			// AGK joystick
			static float m_fFakeJoystickX;
			static float m_fFakeJoystickY;
			static float m_fJoystickDeadZone;

			// text input variables
			static bool m_bInputStartedPrev;
			static bool m_bInputStarted;
			static bool m_bInputCancelled;
			static uString m_sCurrInput;
			static int m_iLastChar;
			static int m_iCurrChar;
			static uString m_sCharBuffer;
			static float m_fCursorBlinkDelay;
			static uint32_t m_iInputMaxChars;
			static uString m_sInputLabel;
			
			// single music manager for agk functions
			static cMusicMgr m_cMusicMgr;
			static int m_iDeviceVolume;

			// directory variables
			static cDirectoryItem *m_pCurrentDirectories;
			static cDirectoryItem *m_pCurrentFiles;

			static cDirectoryItem *m_pCurrentDirectoryIter;
			static cDirectoryItem *m_pCurrentFileIter;
			static int m_iCurrentDirectoryMode; // 0=read, 1= write, 2=both
			static int m_iCurrentFileMode; // 0=read, 1= write, 2=both
            static bool m_bUpdateFileLists;

			// date/time variables
			static int m_pMonthDays[ 12 ];

			// advert variables
			static uString m_sAdMobCode;
			static uString m_sAdMobRewardAdCode;
			static uString m_sInneractiveCode;
			static uString m_sChartboostCode1;
			static uString m_sChartboostCode2;
			static uString m_sAdClientID;
			static uString m_sInneractiveURL;
			static uString m_sInneractiveImage;
			static cImage *m_pAdImage;
			static cSprite *m_pAdSprite;
			static cHTTPConnection *pHTTPInneractive;
			static float m_fAdStartTime;
			static int m_iInneractiveStage;
			static float m_fAdX;
			static float m_fAdY;
			static float m_fAdWidth;
            static float m_fAdOffsetX;
            static float m_fAdOffsetY;
			static int m_iAdType;
			static int m_iAdHorz;
			static int m_iAdVert;
            static int m_iAdTest;
            static int m_iAdVisible;

			// video variables
			static float m_fVideoX;
			static float m_fVideoY;
			static float m_fVideoWidth;
			static float m_fVideoHeight;

			// 3D variables
			static AGKMatrix4 m_matOrtho;
			static cCamera *m_pCurrentCamera;
			static int m_i3DDepth;
			
			// drawing variables
			static float* m_pLineVertexData;
			static uint32_t m_iNumLineVertices;
			static uint32_t m_iLineArraySize;
			static void* m_pInternalLinesVertices;
			static void* m_pInternalLinesPipeline;
			static void* m_pInternalLinesConstants;
			static AGKRenderState m_linesRenderState;
			static AGKVertexLayout m_linesVertexLayout;
			static AGKShader *m_pLinesShader;

			// collision variables
			static CollisionResults m_cCollisionResults;

			// ***********************
			// internal functions
			// ***********************
			static void PlatformSetDevicePtr( void* ptr );
			static void InitGraphicsCommon();
			static void PlatformInitGraphicsCommon();
			static void InitNonGraphicsCommon();
			static void PlatformInitNonGraphicsCommon();
			static int  PlatformNeedsNewRenderer( AGKRendererMode rendererMode, uint32_t flags );
			static void PlatformInitExternal( void *ptr, int width, int height, AGKRenderer renderer );
            static void PlatformInitGraphics( void *ptr, AGKRendererMode rendererMode, uint32_t flags );
			static void PlatformDeleteRenderer();
			static void PlatformInitConsole();
			static void PlatformInitFilePaths();
			static void RecalculateDisplay();
			static void PlatformSync();
			static void PlatformUpdateDeviceSize();
			static void PlatformCleanUp();
			static void PlatformDeviceVolume();
			static void PlatformUpdateTime();
			static void PlatformResetTime();
			static void PlatformReportError( const uString &sMsg );
			static void PlatformCompleteInputInit();
			static int PlatformInputPointerPressed(float x, float y);
			static void PlatformStartTextInput( const char *sInitial );
			static void PlatformStopTextInput();
            static void PlatformChangeTextInput( const char *str );
            static void PlatformUpdateTextInput();
			static void PlatformDrawTextInput();
			static void PlatformMessage( const char* msg );
			static void PlatformUpdateWritePath();
			static void PlatformRestoreWritePath();
			static bool PlatformShowCaptureScreen();
			static void PlatformHideCaptureScreen();
            static void PlatformShowChooseScreen();
			static bool PlatformChooseFile( uString &out, const char *ext, int fullPath );
			static void PlatformResumed();
			static void PlatformResumedOpenGL();
			static int PlatformGetAdPortal();
			static void PlatformSetOrientationAllowed( int p, int p2, int l, int l2 );
			static int PlatformGetNumProcessors();
			static bool PlatformGetDeviceID( uString &out );
			static float PlatformDeviceScale();
			

			// social plugins
			static void  PlatformSocialPluginsSetup           ( void );
			static void  PlatformSocialPluginsDestroy         ( void );

			// RATING COMMANDS
			static void  PlatformRateApp                      ( const char* szID, const char* title, const char* message );

			// IN APP PURCHASE COMMANDS
			static void  PlatformInAppPurchaseReset           ();
			static void  PlatformInAppPurchaseSetKeys		  ( const char *szData1, const char *szData2 );
			static void  PlatformInAppPurchaseSetTitle        ( const char* szTitle );
			static void  PlatformInAppPurchaseAddProductID    ( const char* szID, int type );
			static void  PlatformInAppPurchaseSetup           ( void );
			static void  PlatformInAppPurchaseActivate        ( int iID );
			static void  PlatformInAppPurchaseActivateWithPlan( int iID, const char* planToken );
			static void  PlatformInAppPurchaseResetPurchase   ( const char* token );
			static void  PlatformInAppPurchaseRedeemOffer     ();
			static int   PlatformGetInAppPurchaseState        ( void );
			static int   PlatformGetInAppPurchaseAvailable    ( int iID );
			static int   PlatformGetInAppPurchaseAvailable2   ( int iID );
            static char* PlatformGetInAppPurchaseLocalPrice   ( int iID );
            static char* PlatformGetInAppPurchaseDescription  ( int iID );
			static int   PlatformGetInAppPurchaseIsRenewing   ( int iID );
			static bool  PlatformHasInAppPurchase             ( void );
			static void  PlatformInAppPurchaseRestore		  ();
			static char* PlatformGetInAppPurchaseSignature    ( int iID );
			static char* PlatformGetInAppPurchaseToken        ( int iID );
			static int   PlatformGetInAppPurchaseSubNumPlans  ( int iID );
			static int   PlatformGetInAppPurchaseSubPlanNumPeriods( int iID, int planIndex );
			static char* PlatformGetInAppPurchaseSubPlanPrice ( int iID, int planIndex, int periodIndex );
			static int   PlatformGetInAppPurchaseSubPlanDuration( int iID, int planIndex, int periodIndex );
			static char* PlatformGetInAppPurchaseSubPlanDurationUnit( int iID, int planIndex, int periodIndex );
			static int   PlatformGetInAppPurchaseSubPlanPaymentType( int iID, int planIndex, int periodIndex );
			static char* PlatformGetInAppPurchaseSubPlanTags  ( int iID, int planIndex );
			static char* PlatformGetInAppPurchaseSubPlanToken ( int iID, int planIndex );
            static char* PlatformGetAppReceipt();

			// ADMOB COMMANDS
			static void  PlatformAdMobSetupRelative			  ( const char* szID, int horz, int vert, float offsetX, float offsetY, int type );
			static void  PlatformAdMobFullscreen			  ();
			static void  PlatformAdMobCacheFullscreen		  ();
            static void  PlatformAdMobPosition                ( int horz, int vert, float offsetx, float offsety );
			static void  PlatformSetAdMobVisible              ( int iVisible );
			static void  PlatformAdMobRequestNewAd            ( void );
			static void  PlatformAdMobDestroy                 ( void );
			static bool  PlatformHasAdMob		              ( void );
			static int   PlatformAdMobGetFullscreenLoaded	  ( void );
			static void  PlatformAdMobRewardAd				  ();
			static void  PlatformAdMobCacheRewardAd			  ();
			static int   PlatformAdMobGetRewardAdLoaded		  ( void );
			static int   PlatformAdMobGetRewardAdRewarded	  ( void );
			static int   PlatformAdMobGetRewardAdValue  	  ( void );
			static void  PlatformAdMobResetRewardAd			  ( void );
			static void  PlatformAdMobSetTesting			  ( int testing );
			static void  PlatformAdMobSetChildRating		  ( int rating );

			// CHARTBOOST COMMANDS
			static void  PlatformChartboostSetup			  ( void );
			static void  PlatformChartboostFullscreen		  ( void );
			static int   PlatformChartboostGetFullscreenLoaded( void );
			static void  PlatformChartboostRewardAd			  ();
			static void  PlatformChartboostCacheRewardAd	  ();
			static int   PlatformChartboostGetRewardAdLoaded  ( void );
			static int   PlatformChartboostGetRewardAdRewarded( void );
			static void  PlatformChartboostResetRewardAd	  ( void );

			// FACEBOOK COMMANDS
			static void  PlatformFacebookSetup                ( const char* szID );
			static void  PlatformFacebookLogin                ( void );
			static void  PlatformFacebookLogout               ( void );
			static void  PlatformFacebookShowLikeButton       ( const char* szURL, int iX, int iY, int iWidth, int iHeight );
			static void  PlatformFacebookDestroyLikeButton    ( void );
			static void  PlatformFacebookPostOnMyWall         ( const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription );
			static void  PlatformFacebookPostOnFriendsWall    ( const char* szID, const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription );
			static void  PlatformFacebookInviteFriend         ( const char* szID, const char* szMessage );
			static void  PlatformFacebookGetFriends           ( void );
			static int   PlatformFacebookGetFriendsState      ( void );
			static int   PlatformFacebookGetFriendsCount      ( void );
			static char* PlatformFacebookGetFriendsName       ( int iIndex );
			static char* PlatformFacebookGetFriendsID         ( int iIndex );
			static void  PlatformFacebookDownloadFriendsPhoto ( int iIndex );
			static int   PlatformGetFacebookDownloadState     ( void );
			static char* PlatformGetFacebookDownloadFile      ( void );
			static int   PlatformGetFacebookLoggedIn          ( void );
			static bool  PlatformHasFacebook	              ( void );
			static char* PlatformFacebookGetUserID			  ( void );
			static char* PlatformFacebookGetUserName		  ( void );
			static char* PlatformFacebookGetAccessToken		  ( void );

			// TWITTER COMMANDS
			static void  PlatformTwitterSetup                 ( const char* szKey, const char* szSecret );
			static void  PlatformTwitterMessage               ( const char* szMessage );
			static bool  PlatformHasTwitter		              ( void );

			// local notifications
			static void PlatformCreateLocalNotification( int iID, int datetime, const char *szMessage, const char *szDeepLink );
			static void PlatformCancelLocalNotification( int iID );

			static int PlatformGetIP( uString &sIP );
			static int PlatformGetIPv6( uString &sIP, int *iInterface=0 );
			
			static void UpdateAd();
			static void DrawAd();
			
			static void CreateDefaultImage( const char* filename, uint32_t size, uint32_t* pData );
			static bool GetDeviceCanRotate();

			static uint32_t PlatformLittleEndian( uint32_t u );
			static int PlatformLittleEndian( int i );
			static uint32_t PlatformLocalEndian( uint32_t u );
			static int PlatformLocalEndian( int i );

			static void UpdatePhysicsWalls();
			static void ParseCurrentDirectory();
		
			//static void ConvertMediaPath( uString &sPath );

			static cHashedList<cImage*> m_cImageList;
			static cHashedList<cSprite*> m_cSpriteList;
			static cHashedList<cText*> m_cTextList;
			static cHashedList<AGKFont*> m_cFontList;
			static cHashedList<b2Joint*> m_cJointList;
			static cHashedList<cForce*> m_cForceList;
			static cHashedList<cFile*> m_cFileList;
			static cHashedList<cFolder*> m_cFolderList;
			static cHashedList<cNetwork*> m_cNetworkList;
			static cHashedList<cNetworkMessage*> m_cNetMessageList;
			static cHashedList<cHTTPConnection*> m_cHTTPList;
			static cHashedList<BroadcastListener*> m_cBroadcastListenerList;
			static cHashedList<AGKSocket*> m_cSocketList;
			static cHashedList<cNetworkListener*> m_cSocketListenerList;
			static cHashedList<UDPManager*> m_cUDPListenerList;
			static cHashedList<cParticleEmitter*> m_cParticleEmitterList;
			static cHashedList<cEditBox*> m_cEditBoxList;
			static cHashedList<ZipFile*> m_cZipFileList;
			static cHashedList<cMemblock*> m_cMemblockList;
			static cHashedList<Skeleton2D*> m_cSkeleton2DList;
			static cHashedList<Tween*> m_cTweenList;
			static cHashedList<TweenChain*> m_cTweenChainList;

			static cHashedList<cObject3D*> m_cObject3DList;
			static cHashedList<cCamera*> m_cCameraList;
			static cHashedList<AGKShader*> m_cShaderList;
			static cHashedList<AGK3DParticleEmitter*> m_3DParticleEmitterList;

			// music
			static cHashedList<AGKMusicOGG*> m_cMusicOGGList;

			// lights
			static cHashedList<AGKPointLight*> m_cPointLightList;
			static AGKDirectionalLight m_cDirectionalLight;
			static AGKVector m_AmbientColor;

			// skybox
			static AGKSkyBox *m_pSkyBox;

			//static cSpriteMgr m_cSpriteMgr;
			static cSpriteMgrEx m_cSpriteMgrFront;
			static cObjectMgr m_cObjectMgr;

			// error variables
			static int m_iErrorMode;
			static uString m_sLastError;
			static bool m_bErrorOccurred;
			static cLock m_kErrorLock;
			static void(*m_fWarningCallback)(const char*); 
			static void(*m_fErrorCallback)(const char*);
			static void(*m_fDebugCallback)(const char*, const char*);

			// misc variables
			static int m_iPrintSizeChanged;
			static cImage *m_pPrintDefaultFont;
			static cImage *m_pPrintDefaultFontExt;
			static cText *m_pPrintText;
			static uString m_cPrintStr;
			static char m_szConvStr[ 256 ]; // string used to temporarily hold floats and ints converted to strings
			static bool m_bGenerateMipmaps;
			static cSprite *m_pLastColCheck;
			static cSprite *m_pLastColCheck2;
			static uint32_t m_iRandSeed;
			static float m_fViewOffsetX;
			static float m_fViewOffsetY;
			static float m_fViewZoom;
			static int m_iViewZoomMode;
			static cSprite *m_pSplashSprite;
			static bool m_bFirstSync;
			static float m_fSyncTime;
			static int m_iSyncMode;
			static bool m_bOrientationSet;
			static int m_iCurrentOrientationMode;
			static uString m_sPNToken;

			static AGKUnorderedArray<uString*> m_pVulkanDeviceExtensions;
			static AGKUnorderedArray<uString*> m_pVulkanInstanceExtensions;

			// mersenne twister RNG
			static uint32_t m_iRandMTArray[624];
			static int m_iRandMTIndex;

			static int m_iDrawingCount;

			static uString m_sCurrentDir;
			static uString m_sOrigSetDir;

			static b2World *m_phyWorld;
			static b2Body *m_phyGroundBody;
			static MyDestructionListener *m_DestructionListener;

			static b2Body *groundBodyBottom;
			static b2Body *groundBodyRight;
			static b2Body *groundBodyLeft;
			static b2Body *groundBodyTop;

			static float m_phyScale;
			static bool m_bDrawDebug;
			static int m_iVelIterations;
			static int m_iPosIterations;
			static bool m_bPhysicsStepped;
			static b2Contact *m_pContactIter;
			static cSprite *m_pSpriteContactIter;

			static int m_iMaxPolygonPoints;

			// benchmarking
			static float m_fUpdateTime;
			static float m_fPhysicsTime;
			static float m_fDrawingSetupTime;
			static float m_f3DDrawingSetupTime;
			static float m_fDrawingTime;

			static int m_iNumProcessors;

			// noise
			static SimplexNoise* m_pNoise;

		public:

			static int IsSGX540GPU() { return m_iSGX540GPU; }
        
			static int GetRealPath( uString &inout );
            static void PlatformGetFullPathWrite( uString &inout );
            static void PlatformGetFullPathRead( uString &inout, int mode=0 );
			static int PlatformCreateRawPath( const char *path );
					
			// *********************************
			// public items, not for general use
			// *********************************

			static void SetCompanyName( const char* company );
			static void SetAppName( const char* name );
			static void RestoreWriteDir( void );
			static void OverrideDirectories( const char *szWrite, int useRead );
			static void MasterReset();
			static void FlipOrthoMatrix();

			static char* CreateString( unsigned int size );
			static void DeleteString( char* str );

			// internal public functions, called by automated processes on the device
			static void SetResolutionMode( int mode );
			static void SetExtraAGKPlayerAssetsMode ( int mode );
            static void InitJoysticks();
			static void InitExternal( void *ptr, AGKRenderer renderer, int width, int height, void(*swap)(void*), void* swapParam ); // ptr is reserved, must be 0
			static void InitGraphics( void* ptr, AGKRendererMode rendererMode, uint32_t flags );
			static void InitConsole();
			//static void ChangeRenderer( uint32_t flags, AGKRendererMode rendererMode );
			static void InitFilePaths();
			static void UpdatePtr( void* ptr );
			static void UpdatePtr2( void* ptr ); // does minimal amount of changes
			static void WindowMoved();
			static void ResetRenderState();
			static void ResetAllStates ( void );
			static void CleanUp();
			static void CompositionChanged();
			static void PNToken( const char* token );
			static int GetInternalDataI( int index );
		
			static void OrientationChanged( int mode );
			static int CanOrientationChange( int mode );

			static void AppStart( void* activity ); // only used on Android
			static void AppStop(); // only used on Android
			static void AppPausing();
			static void AppResuming();
			static void LosingContext();
			static void Paused();
			static void Resumed( int mode=1 );
			static void ResumedOpenGL( int mode=1 );

			static int CanResize() { return (m_bAGKFlags & AGK_FLAG_CAN_RESIZE) ? 1 : 0; }

			// touch input commands, the device stub file will need to call these to notify the AGK about input events
			static void TouchPressed(uint32_t ID, int x, int y);
			static void TouchMoved(uint32_t ID, int x, int y);
			static void TouchReleased(uint32_t ID, int x, int y);
		
			static void MouseLeftButton(uint32_t ID, int state);
			static void MouseRightButton(uint32_t ID, int state);
			static void MouseMiddleButton(uint32_t ID, int state);
			static void MouseFourthButton(uint32_t ID, int state);
			static void MouseFifthButton(uint32_t ID, int state);
			static void MouseMove(uint32_t ID, int x, int y);
			static void MouseWheel(uint32_t ID, float w);
		
			static void KeyDown( uint32_t index );
			static void KeyUp( uint32_t index );
			static void CharDown( uint32_t c );

			// set physical or virtual keyboard
			static void KeyboardMode( int mode );

			// sensors
			static void Accelerometer( float x, float y, float z );
			static void Gyro( float x, float y, float z );
			static void Proximity( float distance );
			static void LightSensor( float light );
			static void Magnetic( float x, float y, float z );
			static void RotationSensor( float x, float y, float z, float w );
			static void GPS( float longitude, float latitude, float altitude );
		
			// joysticks
			static void JoystickAxis( void* pDevice, int stick, int axis, float value );
			static void JoystickButton( void* pDevice, int button, int state );
			
			// input related functions
			static float DeviceToDisplayRatioX();
			static float DeviceToDisplayRatioY();
			static float DeviceToScreenX( int x );
			static float DeviceToScreenY( int y );
			static int ScreenToDeviceX( float x );
			static int ScreenToDeviceY( float y );
			static float ViewportToScreenX( int x );
			static float ViewportToScreenY( int y );
			static int ScreenToViewportX( float x );
			static int ScreenToViewportY( float y );

			static float ScreenToWorldX( float x );
			static float ScreenToWorldY( float y );
			static float WorldToScreenX( float x );
			static float WorldToScreenY( float y );

			static float GetStretchValue() { return m_fStretchValue; }

			// tell the AGK about the current device volume (if, as in the case of Samsung, the AGK is required to handle this itself)
			static void DeviceVolume( int vol );
			static int GetDeviceVolume() { return m_iDeviceVolume; }

			// sound input commands, the device stub file will need to call these to notify the GDK about sound events
			static void HandleMusicEvents( void *ptr );
			static void HandleVideoEvents();
			static void ChangeVideoPointer( void *ptr );

			static float PhyToWorldX( float value ) { return value / m_phyScale; }
			static float PhyToWorldY( float value ) { return (value * m_fStretchValue) / m_phyScale; }
			static float WorldToPhyX( float value ) { return value * m_phyScale; }
			static float WorldToPhyY( float value ) { return (value / m_fStretchValue) * m_phyScale; }

			static void ClearJoint( b2Joint *pJoint );

			// social command helpers
            static int FacebookHandleOpenURL( void* url );
            static void HandleDeepLink( const char* link );
			        
		public:

			// *************************
			// public items
			// *************************

			static int m_iAccelerometerExists;
			static int m_iGyroSensorExists;
			static int m_iProximitySensorExists;
			static int m_iLightSensorExists;
			static int m_iMagneticSensorExists;
			static int m_iRotationSensorExists;
			static int m_iGPSSensorExists;

			// core functions
			static void AddVulkanDeviceExtensions( const char* szExtensions );
			static void AddVulkanInstanceExtensions( const char* szExtensions );
			static Renderer* GetRenderer();
			static char* GetRendererName();
			static void SetScreenResolution( int width, int height );
			static void SetWindowPosition( int x, int y );
			static void SetWindowSize( int width, int height, int fullscreen );
			static void SetWindowSize( int width, int height, int fullscreen, int allowOverSized );
			static void SetWindowAllowResize( int mode );
			static void MaximizeWindow();
			static void MinimizeApp();
			static void RestoreApp();
			static int IsPinAppAvailable();
			static void PinApp( int enable );
			static int GetWindowWidth();
			static int GetWindowHeight();
			static void SetImmersiveMode( int mode );
			static int GetDeviceWidth();
			static int GetRealDeviceWidth();
			static int GetDeviceHeight();
			static int GetRealDeviceHeight();
			static int GetMaxDeviceWidth();
			static int GetMaxDeviceHeight();
			static int GetDeviceDPI();
			static char* GetAppPackageName();
			static int GetDevicePlatform();
			static void SetVSync( int mode );
			static void SetSyncRate( float fps, int mode );
			static void SetVirtualResolution( int iWidth, int iHeight );
			static void SetDisplayAspect( float aspect );
			static int GetDisplayNumCutouts();
			static float GetDisplayCutoutTop( int index );
			static float GetDisplayCutoutBottom( int index );
			static float GetDisplayCutoutLeft( int index );
			static float GetDisplayCutoutRight( int index );
			static int GetVirtualWidth();
			static int GetVirtualHeight();
			static float GetScreenBoundsLeft();
			static float GetScreenBoundsRight();
			static float GetScreenBoundsTop();
			static float GetScreenBoundsBottom();
			static float GetScreenBoundsSafeTop();
			static float GetScreenBoundsSafeBottom();
			static float GetScreenBoundsSafeLeft();
			static float GetScreenBoundsSafeRight();
			static float GetDisplayAspect();
			static int GetOrientation();
			static int GetPaused();
			static int GetResumed();
			static int GetResumed2();
			static int IsDarkTheme();
			static char* GetURLSchemeText();
			static void ClearURLSchemeText();
			static char* GetDeviceBaseName();
			static char* GetDeviceType();
			static int GetDeviceNetworkType();
			static int GetStorageRemaining( const char *path );
			static int GetStorageTotal( const char *path );
			static char* GetAppName();
			static void GetAppName( uString &sString );
			static void SetWindowTitle( const char* szTitle );
			static void UpdateDeviceSize(uint32_t w=0, uint32_t h=0);
			static void StepPhysics( float time );
			static void Sync();
			static void Update( float time=0 );
			static void Update2D( float time=0 );
			static void Update3D( float time=0 );
			static void DrawAllLines();
			static void Render2DBack();
			static void Render2DFront();
			static void Render3D();
			static void RenderShadowMap();
			static void Render();
			static void Swap();
			static void SetPresentMode( int mode );
			static void ForcePresent();
			static void UpdateInput();
			static void ClearScreen();
			static void Sleep( uint32_t milliseconds );
			static float Timer();
			static void ResetTimer();
			static double PlatformGetRawTime();
			static float GetRunTime();
			static int GetSeconds();
			static float GetFrameTime();
			static uint32_t GetMilliseconds();
			static char* GetDeviceLanguage();
			static void SetSleepMode( int mode );
			static void SetAntialiasMode( int mode );

			static void SetExpansionFileKey( const char *key );
			static void SetExpansionFileVersion(int version);
			static int GetExpansionFileState();
			static int GetExpansionFileError();
			static void DownloadExpansionFile();
			static float GetExpansionFileProgress();
			static bool ExtractExpansionFile( const char* localFile, const char* expansionFile );

			static char* GetDeviceID();
			static void GetDeviceID( uString &out );
			static char* Sha1( const char* str );
			static void Sha1( const char* str, uString &out );
			static char* Sha256( const char* str );
			static char* Sha512( const char* str );
			static char* HTTPEncode( const char* str );
			static void HTTPEncode( const char* str, uString &out );
			static char* HTTPDecode( const char* str );
			static void HTTPDecode( const char* str, uString &out );
			static int GetNumProcessors();

			static int SortArray( AGKSortValue* pArray, uint32_t size );
			static int SortArray64( AGKSortValue64* pArray, uint32_t size );
			static uint32_t SortFloatToUINT( float f ) { uint32_t i = *((int*)&f); uint32_t mask = -int(i >> 31) | 0x80000000; return i ^ mask; }
			static float SortUINTToFloat( uint32_t u ) { uint32_t mask = ((u >> 31) - 1) | 0x80000000; u = u ^ mask; return *((float*)&u); }
			static uint32_t SortIntToUINT( int i ) { return i ^ 0x80000000; }
			static int SortUINTToInt( uint32_t u ) { return u ^ 0x80000000; }

			// math functions
			static void SetRandomSeed( uint32_t seed );
			static uint32_t Random( );
			static uint32_t Random( uint32_t from, uint32_t to );
			static int RandomSign( int value );
			static void SetRandomSeed2( uint32_t seed );
			static int Random2( );
			static int Random2( int from, int to );
			static float Sin( float a );
			static float Cos( float a );
			static float Tan( float a );
			static float SinRad( float a );
			static float CosRad( float a );
			static float TanRad( float a );
			static float ASin( float a );
			static float ACos( float a );
			static float ATan( float a );
			static float ATan2( float y, float x );
			static float ATanFull( float x, float y );
			static float ASinRad( float a );
			static float ACosRad( float a );
			static float ATanRad( float a );
			static float ATan2Rad( float y, float x );
			static float ATanFullRad( float x, float y );
			static int Trunc( float a );
			static int Floor( float a );
			static int Ceil( float a );
			static int Round( float a );
			static float Sqrt( float a );
			static float Abs( float a );
			static int Mod( int a, int b );
			static float FMod( float a, float b );
			static float Pow( float a, float b );
			static float Lerp(float a, float b, float c);
			static float InverseLerp(float a, float b, float c);
			static float Map(float a, float b, float c, float d, float e);
			static float Clamp(float a, float b, float c);
			static float Min(float a, float b);
			static float Max(float a, float b);
			static float Wrap(float a, float b, float c);
			static float WrapAngle(float a);
			static int Sign(float a);
			static float CurveValue(float a, float b, float c);
			static float CurveAngle(float a, float b, float c);
			static float Log( float a );

			// control functions
			static void SetGenerateMipmaps( int generate );
			static void SetOrientationAllowed( int portrait, int portrait2, int landscape, int landscape2 );
			static float ScreenFPS();
			static uint32_t GetPolygonsDrawn();
			static uint32_t GetShadowPolygonsDrawn();
			static uint32_t GetVerticesProcessed();
			static void SetBorderColor( uint32_t red, uint32_t green, uint32_t blue );
			static void SetClearColor( uint32_t red, uint32_t green, uint32_t blue );
			static void SetDefaultMagFilter( uint32_t filter );
			static void SetDefaultMinFilter( uint32_t filter );
			static void SetDefaultWrapU( uint32_t mode );
			static void SetDefaultWrapV( uint32_t mode );
			static void SetViewOffset( float x, float y );
			static void SetViewZoom( float zoom );
			static void SetViewZoomMode( int mode );
			static float GetViewOffsetX();
			static float GetViewOffsetY();
			static float GetViewZoom();
			static void SetScissor( float x, float y, float x2, float y2 );
			static void UseNewDefaultFonts( int mode );
			
			// functions that fail call this function
			static void DebugInfo( const char *type, const char *data );
			static void PluginError( const char* szErr );
			static void Error( const char *szMsg );
			static void Warning( const char* szMsg );
			static void SetErrorMode( int mode );
			static int GetErrorMode ( );
			static int GetLastError( uString &sOut );
			static char* GetLastError();
			static int GetErrorOccurred();
			static void SetWarningCallback( void(*callback)(const char*) );
			static void SetErrorCallback( void(*callback)(const char*) );
			static void SetDebugCallback( void(*callback)(const char*, const char*) );

			static void Message( const char* msg );
			
			// image functions
			static uint32_t GetImageSizeFromFile( const char* filename );
			static cImage* GetImagePtr ( uint32_t iImageIndex );
			static uint32_t GetImageTextureID ( uint32_t iImageIndex );
			static uint32_t GetOpenGLImageID ( uint32_t iImageIndex );
			static void* GetVulkanVRImageData ( uint32_t iImageIndex );
			static void SetVRImage ( uint32_t iImageIndex, int mode );
			static void ResetVRImages ();
			static void LoadImage ( uint32_t iImageIndex, const char* sImageFilename, int bBlackToAlpha );
			static uint32_t LoadImage ( const char* sImageFilename, int bBlackToAlpha );
			static void LoadImage ( uint32_t iImageIndex, const char* sImageFilename );
			static uint32_t LoadImage ( const char* sImageFilename );
			static void LoadSubImage ( uint32_t iImageIndex, uint32_t iParentIndex, const char* sImageFilename );
			static uint32_t LoadSubImage ( uint32_t iParentIndex, const char* sImageFilename );
			static void LoadImageResized( uint32_t iImageID, const char* szFilename, float scaleX, float scaleY, int cache );
			static uint32_t LoadImageResized( const char* szFilename, float scaleX, float scaleY, int cache );
			static void PreCacheImage( const char* szFilename, float scaleX, float scaleY );
			static int GetCachingFinished();
			static void CreateImageColor( uint32_t imageID, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			static uint32_t CreateImageColor( uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			static void CreateRenderImage( uint32_t imageID, uint32_t width, uint32_t height, uint32_t format, uint32_t mipmap );
			static uint32_t CreateRenderImage( uint32_t width, uint32_t height, uint32_t format, uint32_t mipmap );
			static uint32_t GetImageExists ( uint32_t iImageIndex );
			static void DeleteImage ( uint32_t iImageIndex );
			static void DeleteAllImages();
			static void SetImageSubImages( uint32_t iImageIndex, const char* sSubImageFilename );
			static float GetImageWidth ( uint32_t iImageIndex );
			static float GetImageHeight ( uint32_t iImageIndex );
			static void SetImageMinFilter( uint32_t iImageIndex, uint32_t mode );
			static void SetImageMagFilter( uint32_t iImageIndex, uint32_t mode );
			static void SetImageWrapU( uint32_t iImageIndex, uint32_t mode );
			static void SetImageWrapV( uint32_t iImageIndex, uint32_t mode );
			static void SetImageMask( uint32_t iDstImage, uint32_t iSrcImage, int dst, int src, int x, int y );
			static void SaveImage( uint32_t iImageIndex, const char* filename );
			static void CopyImage( uint32_t newImage, uint32_t fromImage, int x, int y, int width, int height );
			static uint32_t CopyImage( uint32_t fromImage, int x, int y, int width, int height );
			static uint32_t GetImage( float x, float y, float width, float height );
			static void GetImage( uint32_t imageID, float x, float y, float width, float height );
			static char* GetImageFilename( uint32_t imageID );
            static void SetImageSavePixels( int mode );
			static void SetImageTransparentColor( uint32_t iImage, int r, int g, int b );
			static void ResizeImage( uint32_t image, int width, int height );

            static uint32_t ShowChooseImageScreen();
            static void ChooseInternal(int value);
            static uint32_t GetChosenImage();
            static uint32_t IsChoosingImage();
		
            static uint32_t ShowImageCaptureScreen();
			static void CaptureImage();
            static void CaptureInternal(int value);
			static void CancelCapture();
			static uint32_t GetCapturedImage();
			static uint32_t IsCapturingImage();

			static int GetNumDeviceCameras();
			static int SetDeviceCameraToImage( uint32_t cameraID, uint32_t imageID );
			static void DeviceCameraUpdate();
			static void DeviceCameraResumed();
			static int GetDeviceCameraType( uint32_t cameraID );
            static cImage* GetDeviceCameraImage() { return m_pCameraTexture; }
        
			static char* DecodeQRCode( uint32_t image );
			static uint32_t EncodeQRCode( const char* text, int errormode );
        
			static void PrintImage( uint32_t image, float size );

			// render functions
			static void SetRenderToImage( int colorImage, int depthImage );
			static void SetRenderToScreen();
			static uint32_t IsSupportedDepthTexture();
			static int GetSupportedShaderVaryings();
			static int IsInvertedDepth();
			static int GetClipSpaceMode();
			static int IsTopLeftOrigin();
			
			// sprite functions
			static cSprite* GetSpritePtr ( uint32_t iSpriteIndex );
			static uint32_t GetSpriteImageID ( uint32_t iSpriteIndex );
			static uint32_t CreateSprite ( uint32_t iImageIndex );
			static uint32_t LoadSprite ( const char* imagefile );
			static void CreateSprite ( uint32_t iSpriteIndex, uint32_t iImageIndex );
			static void LoadSprite ( uint32_t iSpriteIndex, const char* imagefile );
			static void CreateDummySprite ( uint32_t iSpriteIndex );
			static uint32_t CreateDummySprite ( );
			static void CloneSprite( uint32_t iSpriteIndex, uint32_t iOtherSprite );
			static uint32_t CloneSprite( uint32_t iOtherSprite );
			static void DeleteSprite ( uint32_t iSpriteIndex );
			static void DeleteAllSprites();
			static void SetSpritePosition ( uint32_t iSpriteIndex, float fX, float fY );
			static void SetSpritePositionByOffset ( uint32_t iSpriteIndex, float fX, float fY );
			static void SetSpriteX ( uint32_t iSpriteIndex, float fX );
			static void SetSpriteY ( uint32_t iSpriteIndex, float fY );
			static void SetSpriteAngle ( uint32_t iSpriteIndex, float fAng );
			static void SetSpriteAngleRad ( uint32_t iSpriteIndex, float fAng );
			static void SetSpriteDepth ( uint32_t iSpriteIndex, int iDepth );
			static void SetSpriteFlip ( uint32_t iSpriteIndex, int horz, int vert );
			static void SetSpriteSnap ( uint32_t iSpriteIndex, int snap );
			static void SetSpriteVisible ( uint32_t iSpriteIndex, int bVisible );
			static void SetSpriteActive ( uint32_t iSpriteIndex, int bActive );
			static void SetSpriteImage ( uint32_t iSpriteIndex, uint32_t iImageIndex );
			static void SetSpriteImage ( uint32_t iSpriteIndex, uint32_t iImageIndex, int bUpdateShape );
			static void SetSpriteAdditionalImage ( uint32_t iSpriteIndex, uint32_t iImageIndex, int iStage );
			static void SetSpriteUV ( uint32_t iSpriteIndex, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4 );
			static void ResetSpriteUV( uint32_t iSpriteIndex );
			
			static uint32_t GetSpriteExists ( uint32_t iSpriteIndex );
			static float GetSpriteX ( uint32_t iSpriteIndex );
			static float GetSpriteY ( uint32_t iSpriteIndex );
			static float GetSpriteXByOffset ( uint32_t iSpriteIndex );
			static float GetSpriteYByOffset ( uint32_t iSpriteIndex );
			static float GetSpriteOffsetX ( uint32_t iSpriteIndex );
			static float GetSpriteOffsetY ( uint32_t iSpriteIndex );
			static float GetSpriteWidth ( uint32_t iSpriteIndex );
			static float GetSpriteHeight ( uint32_t iSpriteIndex );
			static float GetSpriteAngle ( uint32_t iSpriteIndex );
			static float GetSpriteAngleRad ( uint32_t iSpriteIndex );
			static float GetSpriteScaleX ( uint32_t iSpriteIndex );
			static float GetSpriteScaleY ( uint32_t iSpriteIndex );
			static int GetSpriteHitTest ( uint32_t iSpriteIndex, float x, float y );
			static int GetSpriteDepth( uint32_t iSpriteIndex );
			static uint32_t GetSpriteColorRed( uint32_t iSpriteIndex );
			static uint32_t GetSpriteColorGreen( uint32_t iSpriteIndex );
			static uint32_t GetSpriteColorBlue( uint32_t iSpriteIndex );
			static uint32_t GetSpriteColorAlpha( uint32_t iSpriteIndex );
			static int GetSpritePlaying( uint32_t iSpriteIndex );
			static int GetSpriteVisible( uint32_t iSpriteIndex );
			static int GetSpriteActive( uint32_t iSpriteIndex );
			static int GetSpriteGroup( uint32_t iSpriteIndex );
			static int GetSpriteTransparency( uint32_t iSpriteIndex );
			static int GetSpriteFlippedH(uint32_t iSpriteIndex);
			static int GetSpriteFlippedV(uint32_t iSpriteIndex);
			static int GetSpriteInScreen( uint32_t iSpriteIndex );

			static float GetSpriteXFromPixel( uint32_t iSpriteIndex, int x );
			static float GetSpriteYFromPixel( uint32_t iSpriteIndex, int y );
			static int GetSpritePixelFromX( uint32_t iSpriteIndex, float x );
			static int GetSpritePixelFromY( uint32_t iSpriteIndex, float y );

			static float GetWorldXFromSprite( uint32_t iSpriteIndex, float x, float y );
			static float GetWorldYFromSprite( uint32_t iSpriteIndex, float x, float y );
			static float GetSpriteXFromWorld( uint32_t iSpriteIndex, float x, float y );
			static float GetSpriteYFromWorld( uint32_t iSpriteIndex, float x, float y );
			
			static void SetSpriteSize( uint32_t iSpriteIndex, float width=-1, float height=-1 );
			static void SetSpriteAnimation( uint32_t iSpriteIndex, int iFrameWidth, int iFrameHeight, int iFrameCount );
			static void AddSpriteAnimationFrame( uint32_t iSpriteIndex, uint32_t iImageIndex );
			static void ClearSpriteAnimationFrames( uint32_t iSpriteIndex );
			static void SetSpriteSpeed( uint32_t iSpriteIndex, float fFps );
			static void PlaySprite( uint32_t iSpriteIndex );
			static void PlaySprite( uint32_t iSpriteIndex, float fFps );
			static void PlaySprite( uint32_t iSpriteIndex, float fFps, int iLoop );
			static void PlaySprite( uint32_t iSpriteIndex, float fFps, int iLoop, int iFromFrame, int iToFrame );
			static void StopSprite( uint32_t iSpriteIndex );
			static void ResumeSprite( uint32_t iSpriteIndex );
			static void SetSpriteFrame( uint32_t iSpriteIndex, int iFrame );
			static int GetSpriteCurrentFrame( uint32_t iSpriteIndex );
			static int GetSpriteFrameCount( uint32_t iSpriteIndex );
			static void SetSpriteTransparency( uint32_t iSpriteIndex, int mode );
			static void SetSpriteBlendModes( uint32_t iSpriteIndex, int src, int dst );
			static void SetSpriteOffset( uint32_t iSpriteIndex, float x, float y );
			static void SetSpriteColor( uint32_t iSpriteIndex, int iRed, int iGreen, int iBlue, int iAlpha );
			static void SetSpriteColorRed( uint32_t iSpriteIndex, int iRed );
			static void SetSpriteColorGreen( uint32_t iSpriteIndex, int iGreen );
			static void SetSpriteColorBlue( uint32_t iSpriteIndex, int iBlue );
			static void SetSpriteColorAlpha( uint32_t iSpriteIndex, int iAlpha );
			static void SetSpriteUVBorder( uint32_t iSpriteIndex, float border );
			static void SetSpriteUVOffset( uint32_t iSpriteIndex, float u, float v );
			static void SetSpriteUVScale( uint32_t iSpriteIndex, float scaleU, float scaleV );
			static void FixSpriteToScreen( uint32_t iSpriteIndex, int mode );

			static void SetSpriteScale( uint32_t iSpriteIndex, float x, float y );
			static void SetSpriteScaleByOffset( uint32_t iSpriteIndex, float x, float y );
			static void SetSpriteScissor( uint32_t iSpriteIndex, float x, float y, float x2, float y2 );
			static void SetSpriteShader( uint32_t spriteID, uint32_t shaderID );

			static void SetSpriteGroup( uint32_t iSpriteIndex, int group );
			static void SetSpriteCategoryBits( uint32_t iSpriteIndex, uint32_t categories );
			static void SetSpriteCategoryBit( uint32_t iSpriteIndex, uint32_t category, int flag );
			static void SetSpriteCollideBits( uint32_t iSpriteIndex, uint32_t mask );
			static void SetSpriteCollideBit( uint32_t iSpriteIndex, uint32_t category, int flag );

			static void SetSpriteGroup( uint32_t iSpriteIndex, int group, int shapeID );
			static void SetSpriteCategoryBits( uint32_t iSpriteIndex, uint32_t categories, int shapeID );
			static void SetSpriteCategoryBit( uint32_t iSpriteIndex, uint32_t category, int flag, int shapeID );
			static void SetSpriteCollideBits( uint32_t iSpriteIndex, uint32_t mask, int shapeID );
			static void SetSpriteCollideBit( uint32_t iSpriteIndex, uint32_t category, int flag, int shapeID );

			static void DrawSprite( uint32_t iSpriteIndex );
			
			// physics commands
			static void SetPhysicsScale( float scale );
			static void SetPhysicsGravity( float x, float y );
			static void SetPhysicsDebugOn();
			static void SetPhysicsDebugOff();
			static void SetPhysicsThreading( int threads );
			static void SetPhysicsCCD( int mode );
			static void SetPhysicsSleeping( int mode );
			static void SetPhysicsMaxPolygonPoints( int points );
			static b2World* GetPhysicsWorld();

			//physics benchmarking
			static float GetPhysicsSolveTime();
			static int GetPhysicsIslandCount();

			static void SetPhysicsWallTop( int mode );
			static void SetPhysicsWallLeft( int mode );
			static void SetPhysicsWallRight( int mode );
			static void SetPhysicsWallBottom( int mode );
			
			// global forces
			static uint32_t CreatePhysicsForce( float x, float y, float power, float limit, float range, int fade );
			static void DeletePhysicsForce( uint32_t iForceIndex );
			static void SetPhysicsForcePosition( uint32_t iForceIndex, float x, float y );
			static void SetPhysicsForcePower( uint32_t iForceIndex, float power );
			static void SetPhysicsForceRange( uint32_t iForceIndex, float range );

			// sprites
			static void SetSpriteShape( uint32_t iSpriteIndex, int shape );
			static void SetSpriteShapeBox( uint32_t iSpriteIndex, float x, float y, float x2, float y2, float angle );
			static void SetSpriteShapeCircle( uint32_t iSpriteIndex, float x, float y, float radius );
			static void SetSpriteShapePolygon( uint32_t iSpriteIndex, uint32_t numPoints, uint32_t index, float x, float y );
			
			static void SetSpriteShape( uint32_t iSpriteIndex, int shape, int shapeID );
			static void SetSpriteShapeBox( uint32_t iSpriteIndex, float x, float y, float x2, float y2, float angle, int shapeID );
			static void SetSpriteShapeCircle( uint32_t iSpriteIndex, float x, float y, float radius, int shapeID );
			static void SetSpriteShapePolygon( uint32_t iSpriteIndex, uint32_t numPoints, uint32_t index, float x, float y, int shapeID );
			static void SetSpriteShapeChain( uint32_t iSpriteIndex, uint32_t numPoints, uint32_t index, int loop, float x, float y, int shapeID );
			
			static void AddSpriteShapeBox( uint32_t iSpriteIndex, float x, float y, float x2, float y2, float angle=0 );
			static void AddSpriteShapeCircle( uint32_t iSpriteIndex, float x, float y, float radius );
			static void AddSpriteShapePolygon( uint32_t iSpriteIndex, uint32_t numPoints, uint32_t index, float x, float y );
			static void AddSpriteShapeChain( uint32_t iSpriteIndex, uint32_t numPoints, uint32_t index, int loop, float x, float y );

			static void ClearSpriteShapes( uint32_t iSpriteIndex );
			static int GetSpriteNumShapes( uint32_t iSpriteIndex );

			static int GetSpriteShapeNumVertices( uint32_t iSpriteIndex, int shapeID );
			static float GetSpriteShapeVertexX( uint32_t iSpriteIndex, int shapeID, int vertex );
			static float GetSpriteShapeVertexY( uint32_t iSpriteIndex, int shapeID, int vertex );

			static void SetSpritePhysicsCOM( uint32_t iSpriteIndex, float x, float y );
			static void CalculateSpritePhysicsCOM( uint32_t iSpriteIndex );
			static float GetSpritePhysicsCOMX( uint32_t iSpriteIndex );
			static float GetSpritePhysicsCOMY( uint32_t iSpriteIndex );

			static void PrepareToDeleteSpriteContacts( cSprite *pSprite );
			static void SetSpritePhysicsOn( uint32_t iSpriteIndex, int mode=2 );
			static void SetSpritePhysicsOff( uint32_t iSpriteIndex );
			static void SetSpritePhysicsDelete( uint32_t iSpriteIndex );
			static void SetSpritePhysicsFriction( uint32_t iSpriteIndex, float friction );
			static void SetSpritePhysicsRestitution( uint32_t iSpriteIndex, float restitution );
			static void SetSpritePhysicsFriction( uint32_t iSpriteIndex, float friction, int shapeID );
			static void SetSpritePhysicsRestitution( uint32_t iSpriteIndex, float restitution, int shapeID );
			static void SetSpritePhysicsDensity( uint32_t iSpriteIndex, float density, int shapeID );
			static void SetSpritePhysicsCanRotate( uint32_t iSpriteIndex, int rotate );
			static void SetSpritePhysicsVelocity( uint32_t iSpriteIndex, float vx, float vy );
			static void SetSpritePhysicsAngularVelocity( uint32_t iSpriteIndex, float va );
			static void SetSpritePhysicsDamping( uint32_t iSpriteIndex, float damp );
			static void SetSpritePhysicsAngularDamping( uint32_t iSpriteIndex, float damp );
			static void SetSpritePhysicsIsBullet( uint32_t iSpriteIndex, int bullet );
			static void SetSpritePhysicsMass( uint32_t iSpriteIndex, float mass );
			static void SetSpritePhysicsIsSensor( uint32_t iSpriteIndex, int sensor );
			static void SetSpritePhysicsIsSensor( uint32_t iSpriteIndex, int sensor, int shapeID );
			
			static void SetSpritePhysicsForce( uint32_t iSpriteIndex, float x, float y, float vx, float vy );
			static void SetSpritePhysicsTorque( uint32_t iSpriteIndex, float torque );
			static void SetSpritePhysicsImpulse( uint32_t iSpriteIndex, float x, float y, float vx, float vy );
			static void SetSpritePhysicsAngularImpulse( uint32_t iSpriteIndex, float impulse );

			static float GetSpritePhysicsVelocityX( uint32_t iSpriteIndex );
			static float GetSpritePhysicsVelocityY( uint32_t iSpriteIndex );
			static float GetSpritePhysicsAngularVelocity( uint32_t iSpriteIndex );
			static float GetSpritePhysicsMass( uint32_t iSpriteIndex );

			static void SetSpritePhysicsGravityScale ( UINT iSpriteIndex, float scale );
			static float GetSpritePhysicsGravityScale ( UINT iSpriteIndex );
			static void SetSpritePhysicsInitiallyAwake ( UINT iSpriteIndex, int awake );
			static void SetSpritePhysicsAllowSleep ( UINT iSpriteIndex, int sleep );
			static float GetSpritePhysicsInertia ( UINT iSpriteIndex );
			static int GetSpritePhysicsIsBullet ( UINT iSpriteIndex );
			static int GetSpritePhysicsIsAwake ( UINT iSpriteIndex );

			// joints
			static void DeleteJoint( uint32_t iJointIndex );
			static int GetJointExists( uint32_t iJointIndex );
			static void CreateDistanceJoint( uint32_t iJointIndex, cSprite *pSprite1, cSprite *pSprite2, float x, float y, float x2, float y2, int colConnected );
			static void CreateRevoluteJoint( uint32_t iJointIndex, cSprite *pSprite1, cSprite *pSprite2, float x, float y, int colConnected );
			static void CreatePrismaticJoint( uint32_t iJointIndex, cSprite *pSprite1, cSprite *pSprite2, float x, float y, float vx, float vy, int colConnected );
			static void CreatePulleyJoint( uint32_t iJointIndex, cSprite *pSprite1, cSprite *pSprite2, float gnd1x, float gnd1y, float gnd2x, float gnd2y, float a1x, float a1y, float a2x, float a2y, float ratio, int colConnected );
			static void CreateMouseJoint( uint32_t iJointIndex, cSprite *pSprite1, float x, float y, float maxForce );
			static void CreateLineJoint( uint32_t iJointIndex, cSprite *pSprite1, cSprite *pSprite2, float x, float y, float vx, float vy, int colConnected );
			static void CreateWeldJoint( uint32_t iJointIndex, cSprite *pSprite1, cSprite *pSprite2, float x, float y, int colConnected );
			static void CreateRopeJoint( uint32_t iJointIndex, cSprite *pSprite1, cSprite *pSprite2, float x, float y, float x2, float y2, float maxLength, int colConnected );
			
			static void CreateDistanceJoint( uint32_t iJointIndex, uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, float x2, float y2, int colConnected );
			static void CreateRevoluteJoint( uint32_t iJointIndex, uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, int colConnected );
			static void CreatePrismaticJoint( uint32_t iJointIndex, uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, float vx, float vy, int colConnected );
			static void CreatePulleyJoint( uint32_t iJointIndex, uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float gnd1x, float gnd1y, float gnd2x, float gnd2y, float a1x, float a1y, float a2x, float a2y, float ratio, int colConnected );
			static void CreateMouseJoint( uint32_t iJointIndex, uint32_t iSpriteIndex1, float x, float y, float maxForce );
			static void CreateLineJoint( uint32_t iJointIndex, uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, float vx, float vy, int colConnected );
			static void CreateWeldJoint( uint32_t iJointIndex, uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, int colConnected );
			static void CreateRopeJoint( uint32_t iJointIndex, uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, float x2, float y2, float maxLength, int colConnected );

			// special joint that joins two joints
			static void CreateGearJoint( uint32_t iJointIndex, uint32_t iJoint1, uint32_t iJoint2, float ratio );

			// returns joint ID
			static uint32_t CreateDistanceJoint( cSprite *pSprite1, cSprite *pSprite2, float x, float y, float x2, float y2, int colConnected );
			static uint32_t CreateRevoluteJoint( cSprite *pSprite1, cSprite *pSprite2, float x, float y, int colConnected );
			static uint32_t CreatePrismaticJoint( cSprite *pSprite1, cSprite *pSprite2, float x, float y, float vx, float vy, int colConnected );
			static uint32_t CreatePulleyJoint( cSprite *pSprite1, cSprite *pSprite2, float gnd1x, float gnd1y, float gnd2x, float gnd2y, float a1x, float a1y, float a2x, float a2y, float ratio, int colConnected );
			static uint32_t CreateMouseJoint( cSprite *pSprite1, float x, float y, float maxForce );
			static uint32_t CreateLineJoint( cSprite *pSprite1, cSprite *pSprite2, float x, float y, float vx, float vy, int colConnected );
			static uint32_t CreateWeldJoint( cSprite *pSprite1, cSprite *pSprite2, float x, float y, int colConnected );
			static uint32_t CreateRopeJoint( cSprite *pSprite1, cSprite *pSprite2, float x, float y, float x2, float y2, float maxLength, int colConnected );
			
			static uint32_t CreateDistanceJoint( uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, float x2, float y2, int colConnected );
			static uint32_t CreateRevoluteJoint( uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, int colConnected );
			static uint32_t CreatePrismaticJoint( uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, float vx, float vy, int colConnected );
			static uint32_t CreatePulleyJoint( uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float gnd1x, float gnd1y, float gnd2x, float gnd2y, float a1x, float a1y, float a2x, float a2y, float ratio, int colConnected );
			static uint32_t CreateMouseJoint( uint32_t iSpriteIndex1, float x, float y, float maxForce );
			static uint32_t CreateLineJoint( uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, float vx, float vy, int colConnected );
			static uint32_t CreateWeldJoint( uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, int colConnected );
			static uint32_t CreateRopeJoint( uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float x, float y, float x2, float y2, float maxLength, int colConnected );

			static void CreatePulleyJoint2( uint32_t iSpriteIndex1, uint32_t iSpriteIndex2, float ratio, int colConnected );
			static uint32_t FinishPulleyJoint( float gnd1x, float gnd1y, float gnd2x, float gnd2y, float a1x, float a1y, float a2x, float a2y );

			// special joint that joins two joints
			static uint32_t CreateGearJoint( uint32_t iJoint1, uint32_t iJoint2, float ratio );

			static void SetJointLimitOn( uint32_t iJointIndex, float lowerLimit, float upperLimit );
			static void SetJointLimitOff( uint32_t iJointIndex );
			static void SetJointMotorOn( uint32_t iJointIndex, float speed, float maxForce );
			static void SetJointMotorOff( uint32_t iJointIndex );
			static void SetJointMouseTarget( uint32_t iJointIndex, float x, float y );
			static void SetJointMouseMaxForce( uint32_t iJointIndex, float maxForce );

			static float GetJointReactionForceX( uint32_t iJointIndex );
			static float GetJointReactionForceY( uint32_t iJointIndex );
			static float GetJointReactionTorque( uint32_t iJointIndex );

			static void SetJointDamping( uint32_t iJointIndex, float dampingRatio, float frequency );

			// ray casts
			static int PhysicsRayCast( float x, float y, float x2, float y2 );
			static int PhysicsRayCastGroup( int group, float x, float y, float x2, float y2 );
			static int PhysicsRayCastCategory( uint32_t category, float x, float y, float x2, float y2 );

			static int SpriteRayCast( float x, float y, float x2, float y2 );
			static int SpriteRayCastGroup( int group, float x, float y, float x2, float y2 );
			static int SpriteRayCastCategory( uint32_t category, float x, float y, float x2, float y2 );

			static int SpriteRayCastSingle( uint32_t sprite, float x, float y, float x2, float y2 );
			static int SpriteRayCastSingle( cSprite* sprite, float x, float y, float x2, float y2 );
			
			// ray cast return
			static uint32_t GetRayCastSpriteID( );
			static cSprite* GetRayCastSprite( );
			static float GetRayCastX();
			static float GetRayCastY();
			static float GetRayCastNormalX();
			static float GetRayCastNormalY();
			static float GetRayCastFraction();

			// non physics collision functions
			static int GetSpriteInBox( uint32_t iSprite1, float x1, float y1, float x2, float y2 );
			static int GetSpriteInCircle( uint32_t iSprite1, float x1, float y1, float radius );
			static int GetSpriteCollision( uint32_t iSprite1, uint32_t iSprite2 );

			static int GetSpriteHit( float x, float y );
			static int GetSpriteHitGroup( int group, float x, float y );
			static int GetSpriteHitCategory( uint32_t category, float x, float y );

			// non physics distance functions
			static float GetSpriteDistance( uint32_t iSprite1, uint32_t iSprite2 );
			static float GetSpriteDistancePoint1X( );
			static float GetSpriteDistancePoint1Y( );
			static float GetSpriteDistancePoint2X( );
			static float GetSpriteDistancePoint2Y( );

			// physics collision functions
			static int GetFirstContact();
			static int GetNextContact();
			static float GetContactWorldX();
			static float GetContactWorldY();
			static uint32_t GetContactSpriteID1();
			static uint32_t GetContactSpriteID2();
			static cSprite* GetContactSprite1();
			static cSprite* GetContactSprite2();

			// body contacts
			static int GetSpriteFirstContact( uint32_t iSprite1 );
			static int GetSpriteNextContact( );
			static float GetSpriteContactWorldX( );
			static float GetSpriteContactWorldY( );
			static uint32_t GetSpriteContactSpriteID2( );
			static cSprite* GetSpriteContactSprite2( );

			static int GetPhysicsCollision( uint32_t iSprite1, uint32_t iSprite2 );
			static float GetPhysicsCollisionX( );
			static float GetPhysicsCollisionY( );
			static float GetPhysicsCollisionWorldX( );
			static float GetPhysicsCollisionWorldY( );


			// particle commands
			static void CreateParticles( uint32_t ID, float x, float y );
			static uint32_t CreateParticles( float x, float y );
			static int GetParticlesExists( uint32_t ID );
			static void DeleteParticles( uint32_t ID );
			static void SetParticlesPosition( uint32_t ID, float x, float y );
			static void SetParticlesDepth( uint32_t ID, int depth );
			static void SetParticlesFrequency( uint32_t ID, float freq );
			static void SetParticlesStartZone( uint32_t ID, float x1, float y1, float x2, float y2 );
			static void SetParticlesDirection( uint32_t ID, float vx, float vy );
			static void SetParticlesVelocityRange( uint32_t ID, float v1, float v2 );
			static void SetParticlesAngle( uint32_t ID, float angle );
			static void SetParticlesAngleRad( uint32_t ID, float angle );
			static void SetParticlesRotationRange( uint32_t ID, float angle1, float angle2 );
			static void SetParticlesRotationRangeRad( uint32_t ID, float angle1, float angle2 );
			static void SetParticlesFaceDirection( uint32_t ID, int mode );
			static void SetParticlesSize( uint32_t ID, float size );
			static void SetParticlesLife( uint32_t ID, float time );
			static void SetParticlesMax( uint32_t ID, int max );
			static void ResetParticleCount( uint32_t ID );
			static void SetParticlesImage( uint32_t ID, uint32_t imageID );
			static void SetParticlesColorInterpolation( uint32_t ID, int mode );
			static void SetParticlesVisible( uint32_t ID, int visible );
			static void SetParticlesActive( uint32_t ID, int active );
			static void SetParticlesTransparency( uint32_t ID, int mode );

			static float GetParticlesX( uint32_t ID );
			static float GetParticlesY( uint32_t ID );
			static int GetParticlesDepth( uint32_t ID );
			static int GetParticlesVisible( uint32_t ID );
			static int GetParticlesActive( uint32_t ID );
			static float GetParticlesFrequency( uint32_t ID );
			static float GetParticlesDirectionX( uint32_t ID );
			static float GetParticlesDirectionY( uint32_t ID );
			static float GetParticlesAngle( uint32_t ID );
			static float GetParticlesAngleRad( uint32_t ID );
			static float GetParticlesSize( uint32_t ID );
			static float GetParticlesLife( uint32_t ID );
			static int GetParticlesMaxReached( uint32_t ID );

			static void AddParticlesForce( uint32_t ID, float starttime, float endtime, float x, float y );
			static void ClearParticlesForces( uint32_t ID );
			static void AddParticlesColorKeyFrame( uint32_t ID, float time, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			static void ClearParticlesColors( uint32_t ID );
			static void AddParticlesScaleKeyFrame( uint32_t ID, float time, float scale );
			static void ClearParticlesScales( uint32_t ID );
			static void FixParticlesToScreen( uint32_t ID, int mode );

			static void UpdateParticles( uint32_t ID, float time );
			static void OffsetParticles( uint32_t ID, float x, float y );

			static void DrawParticles( uint32_t ID );

			// text commands
			static void SetTextDefaultFontImage( uint32_t iImageID );
			static void SetTextDefaultExtendedFontImage( uint32_t iImageID );
			static void SetTextDefaultMinFilter( uint32_t mode );
			static void SetTextDefaultMagFilter( uint32_t mode );

			static void CreateText( uint32_t iTextIndex, const char *string );
			static uint32_t CreateText( const char *string );
			static void DeleteText( uint32_t iTextIndex );
			static void DeleteAllText();
			static void SetTextString( uint32_t iTextIndex, const char *string );
			static void SetTextPosition( uint32_t iTextIndex, float fX, float fY );
			static void SetTextX( uint32_t iTextIndex, float fX );
			static void SetTextY( uint32_t iTextIndex, float fY );
			static void SetTextAngle( uint32_t iTextIndex, float fAngle );
			static void SetTextAngleRad( uint32_t iTextIndex, float fAngleRad );
			static void SetTextSize( uint32_t iTextIndex, float fSize );
			static void SetTextSpacing( uint32_t iTextIndex, float fSpacing );
			static void SetTextLineSpacing( uint32_t iTextIndex, float fSpacing );
			static void SetTextDepth( uint32_t iTextIndex, int iDepth );
			static void SetTextVisible( uint32_t iTextIndex, int bVisible );
			static void SetTextAlignment( uint32_t iTextIndex, int mode );
			static void SetTextColor( uint32_t iTextIndex, uint32_t iRed, uint32_t iGreen, uint32_t iBlue, uint32_t iAlpha=255 );
			static void SetTextColorRed( uint32_t iTextIndex, uint32_t iRed );
			static void SetTextColorGreen( uint32_t iTextIndex, uint32_t iGreen );
			static void SetTextColorBlue( uint32_t iTextIndex, uint32_t iBlue );
			static void SetTextColorAlpha( uint32_t iTextIndex, uint32_t iAlpha );
			static void SetTextFontImage( uint32_t iTextIndex, uint32_t iImageID );
			static void SetTextExtendedFontImage( uint32_t iTextIndex, uint32_t iImageID );
			static void SetTextFont( uint32_t iTextIndex, uint32_t iFontID );
			static void FixTextToScreen( uint32_t iTextIndex, int mode );
			static void SetTextMaxWidth( uint32_t iTextIndex, float width );
			static void SetTextScissor( uint32_t iTextIndex, float x, float y, float x2, float y2 );
			static void SetTextTransparency( uint32_t iTextIndex, int mode );
			static void SetTextBold( uint32_t iTextIndex, uint32_t bold );

			static void SetTextShader(UINT iTextIndex, UINT shaderID);
			static void SetTextShaderConstantByName(UINT iTextIndex, const char* szName, float value1, float value2, float value3, float value4);
			static void SetTextShaderConstantArrayByName(UINT iTextIndex, const char* szName, UINT arrayIndex, float value1, float value2, float value3, float value4);
			static void SetTextShaderConstantDefault(UINT iTextIndex, const char* szName);

			static uint32_t GetTextColorRed( uint32_t iTextIndex );
			static uint32_t GetTextColorGreen( uint32_t iTextIndex );
			static uint32_t GetTextColorBlue( uint32_t iTextIndex );
			static uint32_t GetTextColorAlpha( uint32_t iTextIndex );
			static uint32_t GetTextExists( uint32_t iTextIndex );
			static float GetTextX( uint32_t iTextIndex );
			static float GetTextY( uint32_t iTextIndex );
			static uint32_t GetTextLength( uint32_t iTextIndex );
			static float GetTextTotalWidth( uint32_t iTextIndex );
			static float GetTextTotalHeight( uint32_t iTextIndex );
			static int GetTextHitTest( uint32_t iTextIndex, float x, float y );
			static int GetTextVisible( uint32_t iTextIndex );
			static int GetTextDepth( uint32_t iTextIndex );
			static float GetTextSize( uint32_t iTextIndex );
			static char* GetTextString( uint32_t iTextIndex );
			static float GetTextSpacing( uint32_t iTextIndex );
			static float GetTextLineSpacing( uint32_t iTextIndex );
			static int GetTextAlignment( uint32_t iTextIndex );
			
			static void SetTextCharPosition( uint32_t iTextIndex, uint32_t iCharIndex, float x, float y );
			static void SetTextCharX( uint32_t iTextIndex, uint32_t iCharIndex, float x );
			static void SetTextCharY( uint32_t iTextIndex, uint32_t iCharIndex, float y );
			static void SetTextCharAngle( uint32_t iTextIndex, uint32_t iCharIndex, float angle );
			static void SetTextCharAngleRad( uint32_t iTextIndex, uint32_t iCharIndex, float angle );
			static void SetTextCharColor( uint32_t iTextIndex, uint32_t iCharIndex, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			static void SetTextCharColorRed( uint32_t iTextIndex, uint32_t iCharIndex, uint32_t red );
			static void SetTextCharColorGreen( uint32_t iTextIndex, uint32_t iCharIndex, uint32_t green );
			static void SetTextCharColorBlue( uint32_t iTextIndex, uint32_t iCharIndex, uint32_t blue );
			static void SetTextCharColorAlpha( uint32_t iTextIndex, uint32_t iCharIndex, uint32_t alpha );
			static void SetTextCharBold( uint32_t iTextIndex, uint32_t iCharIndex, uint32_t bold );
			
			static float GetTextCharX( uint32_t iTextIndex, uint32_t iCharIndex );
			static float GetTextCharY( uint32_t iTextIndex, uint32_t iCharIndex );
			static float GetTextCharAngle( uint32_t iTextIndex, uint32_t iCharIndex );
			static float GetTextCharAngleRad( uint32_t iTextIndex, uint32_t iCharIndex );
			static uint32_t GetTextCharColorRed( uint32_t iTextIndex, uint32_t iCharIndex );
			static uint32_t GetTextCharColorGreen( uint32_t iTextIndex, uint32_t iCharIndex );
			static uint32_t GetTextCharColorBlue( uint32_t iTextIndex, uint32_t iCharIndex );
			static uint32_t GetTextCharColorAlpha( uint32_t iTextIndex, uint32_t iCharIndex );

			static void DrawText( uint32_t iTextIndex );

			// Font commands
			static uint32_t LoadFont( const char *szFontFile );
			static void LoadFont( uint32_t iFontID, const char *szFontFile );
			static int GetFontExists( uint32_t iFontID );
			static int GetSystemFontExists( const char *szFontFile );
			static void DeleteFont( uint32_t iFontID );

			// 2D Skeleton commands
			static void CreateSkeleton2D( uint32_t iSkeleton );
			static uint32_t CreateSkeleton2D();
			static int GetSkeleton2DExists( uint32_t iSkeleton );
			static void DeleteSkeleton2D( uint32_t iSkeleton );
			static void LoadSkeleton2DFromSpineFile( uint32_t iSkeleton, const char* filename, float scale, uint32_t atlasImage, int loadAnim );
			static uint32_t LoadSkeleton2DFromSpineFile( const char* filename, float scale, uint32_t atlasImage, int loadAnim );
			static void LoadSkeleton2DFromSpriterFile( uint32_t iSkeleton, const char* filename, float scale, uint32_t atlasImage );
			static uint32_t LoadSkeleton2DFromSpriterFile( const char* filename, float scale, uint32_t atlasImage );
			static void SetSkeleton2DPosition( uint32_t iSkeleton, float x, float y );
			static void SetSkeleton2DRotation( uint32_t iSkeleton, float r );
			static void SetSkeleton2DFlip( uint32_t iSkeleton, int flipH, int flipV );
			static void SetSkeleton2DDepth( uint32_t iSkeleton, uint32_t depth );
			static void FixSkeleton2DToScreen( uint32_t iSkeleton, int mode );
			static void SetSkeleton2DVisible( uint32_t iSkeleton, int mode );

			static float GetSkeleton2DX( uint32_t iskeleton );
			static float GetSkeleton2DY( uint32_t iskeleton );
			static float GetSkeleton2DAngle( uint32_t iskeleton );
			static int GetSkeleton2DDepth(uint32_t iskeleton);

			//static uint32_t AddSkeleton2DBone( uint32_t iSkeleton, uint32_t parent, const char* name );
			static int GetSkeleton2DBone( uint32_t iSkeleton, const char* name );
			static int GetSkeleton2DBoneParent( uint32_t iSkeleton, int bone );
			static float GetSkeleton2DBoneX( uint32_t iSkeleton, int bone );
			static float GetSkeleton2DBoneY( uint32_t iSkeleton, int bone );
			static float GetSkeleton2DBoneAngle( uint32_t iSkeleton, int bone );
			static float GetSkeleton2DBoneCurrX( uint32_t iSkeleton, int bone );
			static float GetSkeleton2DBoneCurrY( uint32_t iSkeleton, int bone );
			static float GetSkeleton2DBoneCurrAngle( uint32_t iSkeleton, int bone );
			static void SetSkeleton2DBonePosition( uint32_t iSkeleton, int bone, float x, float y );
			static void SetSkeleton2DBoneAngle( uint32_t iSkeleton, int bone, float r );
			static void SetSkeleton2DBoneScale( uint32_t iSkeleton, int bone, float sx, float sy );
			static void SetSkeleton2DBoneMode( uint32_t iSkeleton, int bone, int mode );

			static void FixSpriteToSkeleton2D( uint32_t spriteID, uint32_t iSkeletonID, int bone, int zorder );
			
			static void PlaySkeleton2DAnimation( uint32_t iSkeleton, const char* anim, float starttime, int loop, float tweentime );
			static void SetSkeleton2DAnimationFrame( uint32_t iSkeleton, const char* anim, float time, float tweentime );
			static void SetSkeleton2DAnimationSpeed( uint32_t iSkeleton, float speed );
			static float GetSkeleton2DCurrentTime( uint32_t iSkeleton );
			static void StopSkeleton2DAnimation( uint32_t iSkeleton );
			static int GetSkeleton2DIsAnimating( uint32_t iSkeleton );
			static int GetSkeleton2DIsTweening( uint32_t iSkeleton );
			static float GetSkeleton2DAnimationTime( uint32_t iSkeleton, const char* anim );

			// tween commands

			static void DeleteTween( uint32_t tweenID );
			static int GetTweenExists( uint32_t tweenID );
			static void SetTweenDuration( uint32_t tweenID, float duration );
			static int TweenLinear();
			static int TweenSmooth1();
			static int TweenSmooth2();
			static int TweenEaseIn1();
			static int TweenEaseIn2();
			static int TweenEaseOut1();
			static int TweenEaseOut2();
			static int TweenBounce();
			static int TweenOvershoot();

			// custom tweens
			static void CreateTweenCustom( uint32_t tweenID, float duration );
			static uint32_t CreateTweenCustom( float duration );
			static int GetTweenCustomExists( uint32_t tweenID );
			static void SetTweenCustomFloat1( uint32_t tweenID, float begin, float end, int interpolation );
			static void SetTweenCustomFloat2( uint32_t tweenID, float begin, float end, int interpolation );
			static void SetTweenCustomFloat3( uint32_t tweenID, float begin, float end, int interpolation );
			static void SetTweenCustomFloat4( uint32_t tweenID, float begin, float end, int interpolation );
			static void SetTweenCustomInteger1( uint32_t tweenID, int begin, int end, int interpolation );
			static void SetTweenCustomInteger2( uint32_t tweenID, int begin, int end, int interpolation );
			static void SetTweenCustomInteger3( uint32_t tweenID, int begin, int end, int interpolation );
			static void SetTweenCustomInteger4( uint32_t tweenID, int begin, int end, int interpolation );
			static float GetTweenCustomFloat1( uint32_t tweenID );
			static float GetTweenCustomFloat2( uint32_t tweenID );
			static float GetTweenCustomFloat3( uint32_t tweenID );
			static float GetTweenCustomFloat4( uint32_t tweenID );
			static int GetTweenCustomInteger1( uint32_t tweenID );
			static int GetTweenCustomInteger2( uint32_t tweenID );
			static int GetTweenCustomInteger3( uint32_t tweenID );
			static int GetTweenCustomInteger4( uint32_t tweenID );

			static void PlayTweenCustom( uint32_t tweenID, float delay );
			static void PauseTweenCustom( uint32_t tweenID );
			static void ResumeTweenCustom( uint32_t tweenID );
			static void UpdateTweenCustom( uint32_t tweenID, float time );
			static void StopTweenCustom( uint32_t tweenID );
			static int GetTweenCustomPlaying( uint32_t tweenID );

			static float GetTweenCustomTime ( UINT tweenID );
			static float GetTweenCustomEndTime ( UINT tweenID );

			// sprite tweening
			static void CreateTweenSprite( uint32_t tweenID, float duration );
			static uint32_t CreateTweenSprite( float duration );
			static int GetTweenSpriteExists( uint32_t tweenID );
			static void SetTweenSpriteX( uint32_t tweenID, float beginX, float endX, int interpolation );
			static void SetTweenSpriteY( uint32_t tweenID, float beginY, float endY, int interpolation );
			static void SetTweenSpriteXByOffset( uint32_t tweenID, float beginX, float endX, int interpolation );
			static void SetTweenSpriteYByOffset( uint32_t tweenID, float beginY, float endY, int interpolation );
			static void SetTweenSpriteAngle( uint32_t tweenID, float beginA, float endA, int interpolation );
			static void SetTweenSpriteSizeX( uint32_t tweenID, float beginSX, float endSX, int interpolation );
			static void SetTweenSpriteSizeY( uint32_t tweenID, float beginSY, float endSY, int interpolation );
			static void SetTweenSpriteRed( uint32_t tweenID, int beginR, int endR, int interpolation );
			static void SetTweenSpriteGreen( uint32_t tweenID, int beginG, int endG, int interpolation );
			static void SetTweenSpriteBlue( uint32_t tweenID, int beginB, int endB, int interpolation );
			static void SetTweenSpriteAlpha( uint32_t tweenID, int beginA, int endA, int interpolation );

			static void PlayTweenSprite( uint32_t tweenID, uint32_t spriteID, float delay );
			static void PauseTweenSprite( uint32_t tweenID, uint32_t spriteID );
			static void ResumeTweenSprite( uint32_t tweenID, uint32_t spriteID );
			static void UpdateTweenSprite( uint32_t tweenID, uint32_t spriteID, float time );
			static void StopTweenSprite( uint32_t tweenID, uint32_t spriteID );
			static int GetTweenSpritePlaying( uint32_t tweenID, uint32_t spriteID );

			static float GetTweenSpriteTime ( UINT tweenID, UINT spriteID );
			static float GetTweenSpriteEndTime ( UINT tweenID, UINT spriteID );

			// text tweening
			static void CreateTweenText( uint32_t tweenID, float duration );
			static uint32_t CreateTweenText( float duration );
			static int GetTweenTextExists( uint32_t tweenID );
			static void SetTweenTextX( uint32_t tweenID, float beginX, float endX, int interpolation );
			static void SetTweenTextY( uint32_t tweenID, float beginY, float endY, int interpolation );
			static void SetTweenTextAngle( uint32_t tweenID, float beginA, float endA, int interpolation );
			static void SetTweenTextSize( uint32_t tweenID, float beginS, float endS, int interpolation );
			static void SetTweenTextSpacing( uint32_t tweenID, float beginSP, float endSP, int interpolation );
			static void SetTweenTextLineSpacing( uint32_t tweenID, float beginLSP, float endLSP, int interpolation );
			static void SetTweenTextRed( uint32_t tweenID, int beginR, int endR, int interpolation );
			static void SetTweenTextGreen( uint32_t tweenID, int beginG, int endG, int interpolation );
			static void SetTweenTextBlue( uint32_t tweenID, int beginB, int endB, int interpolation );
			static void SetTweenTextAlpha( uint32_t tweenID, int beginA, int endA, int interpolation );

			static void PlayTweenText( uint32_t tweenID, uint32_t textID, float delay );
			static void PauseTweenText( uint32_t tweenID, uint32_t textID );
			static void ResumeTweenText( uint32_t tweenID, uint32_t textID );
			static void UpdateTweenText( uint32_t tweenID, uint32_t textID, float time );
			static void StopTweenText( uint32_t tweenID, uint32_t textID );
			static int GetTweenTextPlaying( uint32_t tweenID, uint32_t textID );

			static float GetTweenTextTime ( UINT tweenID, UINT textID );
			static float GetTweenTextEndTime ( UINT tweenID, UINT textID );

			// text char tweening
			static void CreateTweenChar( uint32_t tweenID, float duration );
			static uint32_t CreateTweenChar( float duration );
			static int GetTweenCharExists( uint32_t tweenID );
			static void SetTweenCharX( uint32_t tweenID, float beginX, float endX, int interpolation );
			static void SetTweenCharY( uint32_t tweenID, float beginY, float endY, int interpolation );
			static void SetTweenCharAngle( uint32_t tweenID, float beginA, float endA, int interpolation );
			static void SetTweenCharRed( uint32_t tweenID, int beginR, int endR, int interpolation );
			static void SetTweenCharGreen( uint32_t tweenID, int beginG, int endG, int interpolation );
			static void SetTweenCharBlue( uint32_t tweenID, int beginB, int endB, int interpolation );
			static void SetTweenCharAlpha( uint32_t tweenID, int beginA, int endA, int interpolation );

			static void PlayTweenChar( uint32_t tweenID, uint32_t textID, uint32_t charID, float delay );
			static void PauseTweenChar( uint32_t tweenID, uint32_t textID, uint32_t charID );
			static void ResumeTweenChar( uint32_t tweenID, uint32_t textID, uint32_t charID );
			static void UpdateTweenChar( uint32_t tweenID, uint32_t textID, uint32_t charID, float time );
			static void StopTweenChar( uint32_t tweenID, uint32_t textID, uint32_t charID );
			static int GetTweenCharPlaying( uint32_t tweenID, uint32_t textID, uint32_t charID );

			static float GetTweenCharTime ( UINT tweenID, UINT textID, UINT charID );
			static float GetTweenCharEndTime ( UINT tweenID, UINT textID, UINT charID );

			// object tweening
			static void CreateTweenObject( uint32_t tweenID, float duration );
			static uint32_t CreateTweenObject( float duration );
			static int GetTweenObjectExists( uint32_t tweenID );
			static void SetTweenObjectX( uint32_t tweenID, float beginX, float endX, int interpolation );
			static void SetTweenObjectY( uint32_t tweenID, float beginY, float endY, int interpolation );
			static void SetTweenObjectZ( uint32_t tweenID, float beginZ, float endZ, int interpolation );
			static void SetTweenObjectAngleX( uint32_t tweenID, float beginAX, float endAX, int interpolation );
			static void SetTweenObjectAngleY( uint32_t tweenID, float beginAY, float endAY, int interpolation );
			static void SetTweenObjectAngleZ( uint32_t tweenID, float beginAZ, float endAZ, int interpolation );
			static void SetTweenObjectScaleX( uint32_t tweenID, float beginSX, float endSX, int interpolation );
			static void SetTweenObjectScaleY( uint32_t tweenID, float beginSY, float endSY, int interpolation );
			static void SetTweenObjectScaleZ( uint32_t tweenID, float beginSZ, float endSZ, int interpolation );
			static void SetTweenObjectRed( uint32_t tweenID, int beginR, int endR, int interpolation );
			static void SetTweenObjectGreen( uint32_t tweenID, int beginG, int endG, int interpolation );
			static void SetTweenObjectBlue( uint32_t tweenID, int beginB, int endB, int interpolation );
			static void SetTweenObjectAlpha( uint32_t tweenID, int beginA, int endA, int interpolation );

			static void PlayTweenObject( uint32_t tweenID, uint32_t objectID, float delay );
			static void PauseTweenObject( uint32_t tweenID, uint32_t objectID );
			static void ResumeTweenObject( uint32_t tweenID, uint32_t objectID );
			static void UpdateTweenObject( uint32_t tweenID, uint32_t objectID, float time );
			static void StopTweenObject( uint32_t tweenID, uint32_t objectID );
			static int GetTweenObjectPlaying( uint32_t tweenID, uint32_t objectID );

			static float GetTweenObjectTime ( UINT tweenID, UINT objectID );
			static float GetTweenObjectEndTime ( UINT tweenID, UINT objectID );

			// camera tweening
			static void CreateTweenCamera( uint32_t tweenID, float duration );
			static uint32_t CreateTweenCamera( float duration );
			static int GetTweenCameraExists( uint32_t tweenID );
			static void SetTweenCameraX( uint32_t tweenID, float beginX, float endX, int interpolation );
			static void SetTweenCameraY( uint32_t tweenID, float beginY, float endY, int interpolation );
			static void SetTweenCameraZ( uint32_t tweenID, float beginZ, float endZ, int interpolation );
			static void SetTweenCameraAngleX( uint32_t tweenID, float beginAX, float endAX, int interpolation );
			static void SetTweenCameraAngleY( uint32_t tweenID, float beginAY, float endAY, int interpolation );
			static void SetTweenCameraAngleZ( uint32_t tweenID, float beginAZ, float endAZ, int interpolation );
			static void SetTweenCameraFOV( uint32_t tweenID, float beginF, float endF, int interpolation );
			
			static void PlayTweenCamera( uint32_t tweenID, uint32_t cameraID, float delay );
			static void PauseTweenCamera( uint32_t tweenID, uint32_t cameraID );
			static void ResumeTweenCamera( uint32_t tweenID, uint32_t cameraID );
			static void UpdateTweenCamera( uint32_t tweenID, uint32_t cameraID, float time );
			static void StopTweenCamera( uint32_t tweenID, uint32_t cameraID );
			static int GetTweenCameraPlaying( uint32_t tweenID, uint32_t cameraID );

			// tween chaining
			static void CreateTweenChain( uint32_t chainID );
			static uint32_t CreateTweenChain();
			static void DeleteTweenChain( uint32_t chainID );
			static void ClearTweenChain( uint32_t chainID );
			static void AddTweenChainCustom( uint32_t chainID, uint32_t tweenID, float delay );
			static void AddTweenChainSprite( uint32_t chainID, uint32_t tweenID, uint32_t spriteID, float delay );
			static void AddTweenChainText( uint32_t chainID, uint32_t tweenID, uint32_t textID, float delay );
			static void AddTweenChainChar( uint32_t chainID, uint32_t tweenID, uint32_t textID, uint32_t charID, float delay );
			static void AddTweenChainObject( uint32_t chainID, uint32_t tweenID, uint32_t objectID, float delay );
			static void AddTweenChainCamera( uint32_t chainID, uint32_t tweenID, uint32_t cameraID, float delay );

			static void PlayTweenChain( uint32_t chainID );
			static void PauseTweenChain( uint32_t chainID );
			static void StopTweenChain( uint32_t chainID );
			static int GetTweenChainPlaying( uint32_t chainID );
			static void SetTweenChainTime( uint32_t chainID, float time );
			static float GetTweenChainEndTime( uint32_t chainID );

			static void UpdateAllTweens( float time );
			static void UpdateTweenChain( uint32_t chainID, float time );
		
			static void ClearTweenSprite ( UINT tweenID );
			static void ClearTweenCustom ( UINT tweenID );
			static void ClearTweenText ( UINT tweenID );
			static void ClearTweenChar ( UINT tweenID );
			static void ClearTweenObject ( UINT tweenID );
			static void ClearTweenCamera ( UINT tweenID );

			// print commands
			static void Print( const uString &string );
			static void Print( const char* szString );
			static void Print( int i );
			static void Print( uint32_t u );
			static void Print( float f );
			static void PrintC( const uString &string );
			static void PrintC( const char* szString );
			static void PrintC( int i );
			static void PrintC( uint32_t u );
			static void PrintC( float f );
			static void SetPrintSize( float fSize );
			static float GetPrintSize( void ) { return m_pPrintText->GetSize(); } // 251011
			static void SetPrintSpacing( float fSpacing );
			static void SetPrintColor( uint32_t iRed, uint32_t iGreen, uint32_t iBlue );
			static void SetPrintColor( uint32_t iRed, uint32_t iGreen, uint32_t iBlue, uint32_t iAlpha );
			static void SetPrintFont( int fontID );

			// raw input commands
			static int GetMultiTouchExists();
			static int GetMouseExists();
			static int GetKeyboardExists();
			static int GetJoystickExists();
			static int GetCameraExists();

			// sensors
			static int GetAccelerometerExists();
			static int GetGyroSensorExists();
			static int GetProximitySensorExists();
			static int GetLightSensorExists();
			static int GetMagneticSensorExists();
			static int GetRotationVectorSensorExists();
			static int GetGPSSensorExists();

			// touch commands
			static uint32_t GetRawTouchCount( int bIncludeUnknown=0 );
			static uint32_t GetRawFirstTouchEvent( int bIncludeUnknown=0 );
			static uint32_t GetRawNextTouchEvent( );
			static int GetRawTouchType( uint32_t iIndex );
			static float GetRawTouchStartX( uint32_t iIndex );
			static float GetRawTouchStartY( uint32_t iIndex );
			static float GetRawTouchCurrentX( uint32_t iIndex );
			static float GetRawTouchCurrentY( uint32_t iIndex );
			static float GetRawTouchLastX( uint32_t iIndex );
			static float GetRawTouchLastY( uint32_t iIndex );
			static int GetRawTouchReleased( uint32_t iIndex );
			static void SetRawTouchValue( uint32_t iIndex, int value );
			static int GetRawTouchValue( uint32_t iIndex );
			static float GetRawTouchTime( uint32_t iIndex );
			static void SetRawTouchMoveSensitivity( int distance );

			// mouse
			static float GetRawMouseX();
			static float GetRawMouseY();
			static float GetRawMouseWheel();
			static float GetRawMouseWheelDelta();
			static int GetRawMouseLeftPressed();
			static int GetRawMouseLeftState();
			static int GetRawMouseLeftReleased();
			static int GetRawMouseRightPressed();
			static int GetRawMouseRightState();
			static int GetRawMouseRightReleased();
			static int GetRawMouseMiddlePressed();
			static int GetRawMouseMiddleState();
			static int GetRawMouseMiddleReleased();
			static int GetRawMouseFourthPressed();
			static int GetRawMouseFourthState();
			static int GetRawMouseFourthReleased();
			static int GetRawMouseFifthPressed();
			static int GetRawMouseFifthState();
			static int GetRawMouseFifthReleased();
			static void SetRawMouseVisible( int visible );
			static void SetRawMousePosition( float x, float y );
		
			// sensors

			// accelerometer
			static void FixOrientationByDefault();
			static float GetRawAccelX();
			static float GetRawAccelY();
			static float GetRawAccelZ();

			// gyroscope
			static float GetRawGyroVelocityX();
			static float GetRawGyroVelocityY();
			static float GetRawGyroVelocityZ();

			// proximity
			static float GetRawProximityDistance();

			// light sensor
			static float GetRawLightLevel();

			// magnetic
			static float GetRawMagneticX();
			static float GetRawMagneticY();
			static float GetRawMagneticZ();

			// rotation sensor
			static float GetRawRotationVectorX();
			static float GetRawRotationVectorY();
			static float GetRawRotationVectorZ();
			static float GetRawRotationVectorW();
        
            static float GetRawRotationVectorX2();
            static float GetRawRotationVectorY2();
            static float GetRawRotationVectorZ2();
            static float GetRawRotationVectorW2();

			// GPS
			static void StartGPSTracking();
			static void StopGPSTracking();
			static float GetRawGPSLatitude();
			static float GetRawGPSLongitude();
			static float GetRawGPSAltitude();

			// joystick
			static void CompleteRawJoystickDetection();
			static int GetRawJoystickExists( uint32_t index );
			static int GetRawJoystickConnected( uint32_t index );
			static char* GetRawJoystickName( uint32_t index );
			static float GetRawJoystickX( uint32_t index );
			static float GetRawJoystickY( uint32_t index );
			static float GetRawJoystickZ( uint32_t index );
			static float GetRawJoystickRX( uint32_t index );
			static float GetRawJoystickRY( uint32_t index );
			static float GetRawJoystickRZ( uint32_t index );
			static int GetRawJoystickSlider( uint32_t index, uint32_t slider );
			static int GetRawJoystickPOV( uint32_t index, uint32_t pov );
			static int GetRawJoystickButtonPressed( uint32_t index, uint32_t button );
			static int GetRawJoystickButtonState( uint32_t index, uint32_t button );
			static int GetRawJoystickButtonReleased( uint32_t index, uint32_t button );
			static void SetRawJoystickDeadZone( float threshold );

			// virtual joysticks
			static void AddVirtualJoystick( uint32_t index, float x, float y, float size );
			static void DeleteVirtualJoystick( uint32_t index );
			static int GetVirtualJoystickExists( uint32_t index );
			static float GetVirtualJoystickX( uint32_t index );
			static float GetVirtualJoystickY( uint32_t index );
			static void SetVirtualJoystickAlpha( uint32_t index, uint32_t alpha1, uint32_t alpha2 );
			static void SetVirtualJoystickPosition( uint32_t index, float x, float y );
			static void SetVirtualJoystickSize( uint32_t index, float size );
			static void SetVirtualJoystickActive( uint32_t index, int active );
			static void SetVirtualJoystickVisible( uint32_t index, int visible );
			static void SetVirtualJoystickImageInner( uint32_t index, uint32_t imageID );
			static void SetVirtualJoystickImageOuter( uint32_t index, uint32_t imageID );
			static void SetVirtualJoystickDeadZone( float threshold );
			
			// virtual buttons
			static void AddVirtualButton( uint32_t index, float x, float y, float size );
			static void DeleteVirtualButton( uint32_t index );
			static int GetVirtualButtonExists( uint32_t index );
			static int GetVirtualButtonPressed( uint32_t index );
			static int GetVirtualButtonReleased( uint32_t index );
			static int GetVirtualButtonState( uint32_t index );
			static void SetVirtualButtonColor( uint32_t index, uint32_t red, uint32_t green, uint32_t blue );
			static void SetVirtualButtonAlpha( uint32_t index, uint32_t alpha );
			static void SetVirtualButtonPosition( uint32_t index, float x, float y );
			static void SetVirtualButtonSize( uint32_t index, float size );
			static void SetVirtualButtonSize( uint32_t index, float sizeX, float sizeY );
			static void SetVirtualButtonActive( uint32_t index, int active );
			static void SetVirtualButtonVisible( uint32_t index, int visible );
			static void SetVirtualButtonImageUp( uint32_t index, uint32_t imageID );
			static void SetVirtualButtonImageDown( uint32_t index, uint32_t imageID );
			static void SetVirtualButtonText( uint32_t index, const char *str );

			// keyboard
			static int GetRawKeyPressed( uint32_t key );
			static int GetRawKeyState( uint32_t key );
			static int GetRawKeyReleased( uint32_t key );
			static int GetRawLastKey();
			static char* GetCharBuffer();
			static int GetCharBufferLength();
		
			// AGK emulated input commands
			static int GetPointerPressed();
			static int GetPointerState();
			static int GetPointerReleased();
			static float GetPointerX();
			static float GetPointerY();

			static float GetDirectionX();
			static float GetDirectionY();
			static float GetDirectionAngle();
			static float GetDirectionSpeed();

			static float GetJoystickX();
			static float GetJoystickY();
			static void SetJoystickScreenPosition( float x, float y, float size );
			static void SetJoystickDeadZone( float threshold );

			static int GetButtonPressed( uint32_t index );
			static int GetButtonState( uint32_t index );
			static int GetButtonReleased( uint32_t index );
			static void SetButtonScreenPosition( uint32_t index, float x, float y, float size );

			// text input
			static void StartTextInput( );
            static void StartTextInput( const char* initial );
			static void StopTextInput( );
			static int GetTextInputState();
			static int GetTextInputCompleted();
			static int GetTextInputCancelled();
			static char* GetTextInput();
			static int GetTextInput( uString &out );
			static uint32_t GetPrevChar();
			static uint32_t GetLastChar();
			static void SetCursorBlinkTime( float seconds );
			static void SetTextInputMaxChars( int max=0 );
			static void SetTextInputLabel( const char* str );

			// edit boxes
			static void CreateEditBox( uint32_t index );
			static uint32_t CreateEditBox();
			static int GetEditBoxExists( uint32_t index );
			static void DeleteEditBox( uint32_t index );
			static int GetEditBoxHasFocus( uint32_t index );
			static uint32_t GetCurrentEditBox();
			static void SetEditBoxPosition( uint32_t index, float x, float y );
			static void SetEditBoxSize( uint32_t index, float width, float height );
			static void SetEditBoxDepth( uint32_t index, int depth );
			static void SetEditBoxBorderSize( uint32_t index, float size );
			static void SetEditBoxBorderColor( uint32_t index, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			static void SetEditBoxBackgroundColor( uint32_t index, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			static void SetEditBoxText( uint32_t index, const char *str ); // 120212 - paul notice, how a command name which is contained in another similar command MUST be placed first!!
			static void SetEditBoxTextColor( uint32_t index, uint32_t red, uint32_t green, uint32_t blue );
			static void SetEditBoxCursorColor( uint32_t index, uint32_t red, uint32_t green, uint32_t blue );
			static void SetEditBoxFontImage( uint32_t index, uint32_t image );
            static void SetEditBoxExtendedFontImage( uint32_t index, uint32_t image );
			static void SetEditBoxFont( uint32_t index, uint32_t fontID );
			static void SetEditBoxTextSize( uint32_t index, float size );
			static void SetEditBoxCursorPosition( uint32_t index, int pos );
			static void SetEditBoxFocus( uint32_t index, int iActive );
			static void SetEditBoxActive( uint32_t index, int iActive );
			static void SetEditBoxVisible( uint32_t index, int iActive );
			static void SetEditBoxBorderImage( uint32_t index, uint32_t image );
			static void SetEditBoxBackgroundImage( uint32_t index, uint32_t image );
			static void SetEditBoxCursorBlinkTime( uint32_t index, float time );
			static void SetEditBoxCursorWidth( uint32_t index, float width );
			static void SetEditBoxMaxChars( uint32_t index, uint32_t max );
			static void SetEditBoxMaxLines( uint32_t index, uint32_t max );
			static void SetEditBoxMultiLine( uint32_t index, int mulitline );
			static void SetEditBoxScissor( uint32_t index, float x, float y, float x2, float y2 );
			static void SetEditBoxPasswordMode( uint32_t index, int mode );
            static void SetEditBoxUseAlternateInput( uint32_t index, int mode );
			static void SetEditBoxWrapMode( uint32_t index, int mode );
			static void SetEditBoxInputType( uint32_t index, int inputType );
			static void FixEditBoxToScreen( uint32_t index, int fix );
			static char* GetEditBoxText( uint32_t index );
			static float GetEditBoxX( uint32_t index );
			static float GetEditBoxY( uint32_t index );
			static float GetEditBoxWidth( uint32_t index );
			static float GetEditBoxHeight( uint32_t index );
			static int GetEditBoxChanged( uint32_t index );
			static int GetEditBoxActive( uint32_t index );
			static int GetEditBoxDepth(uint32_t index);
			static int GetEditBoxVisible( uint32_t index );
			static int GetEditBoxLines( uint32_t index );
			static int GetEditBoxCursorPosition( uint32_t index );

			// sound comands
			static void SetSoundDeviceMode( int mode );
			static void LoadSound( uint32_t iID, const char* sFilename );
			static uint32_t LoadSound( const char* sFilename );
			static void LoadSound( uint32_t iID, const uString &sFile );
			static uint32_t LoadSound( const uString &sFile );
			static void LoadSoundOGG( uint32_t iID, const char* sFilename );
			static uint32_t LoadSoundOGG( const char* sFilename );
			static uint32_t PlaySound( uint32_t iID );
			static uint32_t PlaySound( uint32_t iID, int iVol );
			static uint32_t PlaySound( uint32_t iID, int iVol, int iLoop );
			static uint32_t PlaySound( uint32_t iID, int iVol, int iLoop, int iPriority );
			static void StopSound( uint32_t iID );
			static void DeleteSound( uint32_t iID );
			static uint32_t GetSoundInstances( uint32_t iID );
			static uint32_t GetSoundsPlaying( uint32_t iID );
			static void SetSoundSystemVolume( int iVol );
			static uint32_t GetSoundExists( uint32_t iID );
			static float GetSoundMaxRate();
			static float GetSoundMinRate();
			static void SaveSound( uint32_t iID, const char* sImageFilename );

			static int GetSoundInstancePlaying( uint32_t iID );
			static int GetSoundInstanceVolume( uint32_t iID );
			static float GetSoundInstanceRate( uint32_t iID );
			static void SetSoundInstanceVolume( uint32_t iID, int vol );
			static void SetSoundInstanceRate( uint32_t iID, float rate );
			static void SetSoundInstanceBalance( uint32_t iID, float balance );
			static int GetSoundInstanceLoopCount( uint32_t iID );
			static void StopSoundInstance( uint32_t iID );

			// music comands
			static uint32_t LoadMusic( const char* sFile );
			static void LoadMusic( uint32_t iID, const char* sFile );
			static void SetMusicFileVolume( uint32_t ID, int vol );
			static void PlayMusic( );
			static void PlayMusic( uint32_t iID );
			static void PlayMusic( uint32_t iID, int bLoop );
			static void PlayMusic( uint32_t iID, int bLoop, uint32_t iStartID, uint32_t iEndID );
			static void PauseMusic( );
			static void ResumeMusic( );
			static void StopMusic( );
			static void DeleteMusic( uint32_t iID );
			static int GetMusicPlaying();
			static void SetMusicSystemVolume( int iVol );
			static uint32_t GetMusicExists( uint32_t iID );
			static float GetMusicDuration( uint32_t iID );
			static float GetMusicPosition();
			static void SeekMusic( float seconds, int mode );

			// OGG music
			static uint32_t LoadMusicOGG( const char* sFile );
			static void LoadMusicOGG( uint32_t musicID, const char* sFile );
			static int GetMusicExistsOGG( uint32_t musicID );
			static void SetMusicVolumeOGG( uint32_t musicID, int vol );
			static void PlayMusicOGG( uint32_t musicID );
			static void PlayMusicOGG( uint32_t musicID, int iLoop );
			static void PauseMusicOGG( uint32_t musicID );
			static void ResumeMusicOGG( uint32_t musicID );
			static void StopMusicOGG( uint32_t musicID );
			static void DeleteMusicOGG( uint32_t musicID );
			static int GetMusicPlayingOGG( uint32_t musicID );
			static int GetMusicLoopCountOGG( uint32_t musicID );
			static float GetMusicDurationOGG( uint32_t musicID );
			static float GetMusicPositionOGG( uint32_t musicID );
			static void SeekMusicOGG( uint32_t musicID, float seconds, int mode );
			static void SetMusicSystemVolumeOGG( int iVol );
			static void SetMusicLoopTimesOGG(uint32_t musicID, float startTime, float endTime);
			static void SetMusicLoopCountOGG(uint32_t musicID, int loop);

			// record sound
			static void RecordSound( const char* szFilename );
			static void StopSoundRecording();
			static int IsSoundRecording();

			static void VibrateDevice( float seconds );

			static void SetClipboardText( const char* szText );
			static char* GetClipboardText();

			// file system commands
			static char* SimplifyPath( const char *szPath );
			static char* JoinPaths( const char *szPath, const char *szPath2 );
			static int IsAbsolutePath( const char *szPath );
			static int CountWindowsDrives();
			static char* GetWindowsDrive( int index );

			static uint32_t OpenRawFolder( const char *szPath );
			static void CloseRawFolder( uint32_t ID );
			static int GetRawFolderNumFiles( uint32_t ID );
			static int GetRawFolderNumFolders( uint32_t ID );
			static char* GetRawFolderFileName( uint32_t ID, int index );
			static char* GetRawFolderFolderName( uint32_t ID, int index );

			static char* GetWritePath();
			static char* GetReadPath();
			static char* GetDocumentsPath();

			static char* ChooseRawFile( const char* ext );
			static char* ChooseRawFile( const char* ext, int returnFullPath );

			static int GetFileExists( const char *szFile );
			static void DeleteFile( const char *szFile );
			
			static uint32_t OpenToWrite( const char *szFile, int append );
			static void OpenToWrite( uint32_t ID, const char *szFile, int append );
			static uint32_t OpenToWrite( const char *szFile );
			static void OpenToWrite( uint32_t ID, const char *szFile );			
			
			static uint32_t OpenToRead( const char *szFile );
			static void OpenToRead( uint32_t ID, const char *szFile );

			static int FileIsOpen( uint32_t ID );
			static void CloseFile( uint32_t iFileID );
			static int FileEOF( uint32_t iFileID );
			static int GetFileSize( uint32_t iFileID );
			static int GetFilePos( uint32_t iFileID );
			static void SetFilePos( uint32_t iFileID, int pos );
			
			static void WriteByte( uint32_t iFileID, int b );
			static void WriteInteger( uint32_t iFileID, int i );
			static void WriteFloat( uint32_t iFileID, float f );
			static void WriteString( uint32_t iFileID, const char* str );
			static void WriteString2( uint32_t iFileID, const char* str );
			static void WriteLine( uint32_t iFileID, const char* str );

			static int ReadByte( uint32_t iFileID );
			static int ReadInteger( uint32_t iFileID );
			static float ReadFloat( uint32_t iFileID );
			static char* ReadString( uint32_t iFileID );
			static char* ReadString2( uint32_t iFileID );
			static char* ReadLine( uint32_t iFileID );

			// directory commands
			static int SetCurrentDir( const char* );
			static char* GetCurrentDir();
			static int SetFolder( const char* );
			static char* GetFolder();

			static int MakeFolder( const char* );
			static void DeleteFolder( const char* );
			
			static char* GetFirstFolder();
			static char* GetFirstFolder( int mode );
			static char* GetNextFolder();
			static int GetFolderCount();
            static int GetFolderCount( int mode );

			static char* GetFirstFile();
			static char* GetFirstFile( int mode );
			static char* GetNextFile();
			static int GetFileCount();
            static int GetFileCount( int mode );

			// broadcast commands
			static uint32_t CreateBroadcastListener( uint32_t port );
			static uint32_t CreateBroadcastListener( const char *szIP, uint32_t port );
			static void DeleteBroadcastListener( uint32_t iID );
			static uint32_t GetBroadcastMessage( uint32_t iID );

			// socket commands
			static uint32_t ConnectSocket( const char *szIP, int port, int timeout );
			static int ConnectSocket( uint32_t socketID, const char *szIP, int port, int timeout );
			static int GetSocketConnected( uint32_t socketID );
			static int GetSocketExists( uint32_t socketID );
			static void DeleteSocket( uint32_t socketID );
			static AGKSocket* GetSocketPtr( uint32_t socketID );

			static char* GetSocketRemoteIP( uint32_t socketID );
			static int FlushSocket( uint32_t scoketID );

			static int GetSocketBytesAvailable( uint32_t socketID );
			static int GetSocketByte( uint32_t socketID );
			static int GetSocketInteger( uint32_t socketID );
			static float GetSocketFloat( uint32_t socketID );
			static char* GetSocketString( uint32_t socketID );

			static int SendSocketByte( uint32_t socketID, int value );
			static int SendSocketInteger( uint32_t socketID, int value );
			static int SendSocketFloat( uint32_t socketID, float value );
			static int SendSocketString( uint32_t socketID, const char *value );

			// socket listener
			static uint32_t CreateSocketListener( const char *szIP, int port );
			static int CreateSocketListener( uint32_t listenerID, const char *szIP, int port );
			static uint32_t GetSocketListenerConnection( uint32_t listenerID );
			static void DeleteSocketListener( uint32_t listenerID );

			// network commands
			static uint32_t HostNetwork( const char *szNetworkName, const char *szMyName, int port );
			static uint32_t HostNetwork( const char *szNetworkName, const char *szMyName, int port, int portv6 );
			static void SetNetworkNoMoreClients( uint32_t iNetID );
			static void SetNetworkAllowClients( uint32_t iNetID );

			static uint32_t JoinNetwork( const char *szNetworkName, const char *szMyName );
			static uint32_t JoinNetwork( const char *szIP, uint32_t port, const char *szMyName );

			static int GetNetworkExists( uint32_t iNetID );
			static uint32_t IsNetworkActive( uint32_t iNetID );
			static char* GetDeviceIP();
			static char* GetDeviceIPv6();
            static int GetNetworkInterface();
			
			static void CloseNetwork( uint32_t iNetID );
			static void SetNetworkLatency( uint32_t iNetID, uint32_t latency );

			static uint32_t GetNetworkMyClientID( uint32_t iNetID );
			static uint32_t GetNetworkNumClients( uint32_t iNetID );
			static uint32_t GetNetworkFirstClient( uint32_t iNetID );
			static uint32_t GetNetworkNextClient( uint32_t iNetID );

			static void KickNetworkClient( uint32_t iNetID, uint32_t client );

			static uint32_t GetNetworkClientDisconnected( uint32_t iNetID, uint32_t client );
			static void DeleteNetworkClient( uint32_t iNetID, uint32_t client );
			static char* GetNetworkClientName( uint32_t iNetID, uint32_t client );
			static char* GetNetworkClientIP( uint32_t iNetID, uint32_t client );
			static float GetNetworkClientPing( uint32_t iNetID, uint32_t client );
			static uint32_t GetNetworkServerID( uint32_t iNetID );
			static char* GetNetworkServerIP( uint32_t iNetID );
			
			static void SetNetworkLocalInteger( uint32_t iNetID, const char *name, int i );
			static void SetNetworkLocalInteger( uint32_t iNetID, const char *name, int i, int mode );
			static void SetNetworkLocalFloat( uint32_t iNetID, const char *name, float f );
			static void SetNetworkLocalFloat( uint32_t iNetID, const char *name, float f, int mode );
			static int GetNetworkClientInteger( uint32_t iNetID, uint32_t client, const char *name );
			static float GetNetworkClientFloat( uint32_t iNetID, uint32_t client, const char *name );

			static uint32_t CreateNetworkMessage( );
			static uint32_t CopyNetworkMessage( uint32_t iFromMsgID );
			static void AddNetworkMessageByte( uint32_t iMsgID, uint32_t value );
			static void AddNetworkMessageInteger( uint32_t iMsgID, int value );
			static void AddNetworkMessageFloat( uint32_t iMsgID, float value );
			static void AddNetworkMessageString( uint32_t iMsgID, const char *value );
			static char* GetNetworkMessageFromIP( uint32_t iMsgID );
			static int GetNetworkMessageFromPort( uint32_t iMsgID );
			static uint32_t GetNetworkMessageFromClient( uint32_t iMsgID );
			static uint32_t GetNetworkMessageByte( uint32_t iMsgID );
			static int GetNetworkMessageInteger( uint32_t iMsgID );
			static float GetNetworkMessageFloat( uint32_t iMsgID );
			static char* GetNetworkMessageString( uint32_t iMsgID );
			static void DeleteNetworkMessage( uint32_t iMsgID );

			static void SendNetworkMessage( uint32_t iNetID, uint32_t toClient, uint32_t iMsgID );
			static uint32_t GetNetworkMessage( uint32_t iNetID );
			
			static void SetNetworkClientUserData( uint32_t iNetID, uint32_t client, uint32_t index, int value );
			static int GetNetworkClientUserData( uint32_t iNetID, uint32_t client, uint32_t index );

			// UDP
			static uint32_t CreateUDPListener( const char* ip, int port );
			static int CreateUDPListener( uint32_t listenerID, const char* ip, int port );
			static void SendUDPNetworkMessage( uint32_t listenerID, uint32_t messageID, const char* toIP, int toPort );
			static uint32_t GetUDPNetworkMessage( uint32_t listenerID );
			static void DeleteUDPListener( uint32_t listenerID );

			// HTTP commands
			static uint32_t CreateHTTPConnection();
			static void DeleteHTTPConnection( uint32_t iHTTP );

			static int GetInternetState();
			
			static uint32_t SetHTTPHost( uint32_t iHTTP, const char *szHost, int iSecure );
			static uint32_t SetHTTPHost( uint32_t iHTTP, const char *szHost, int iSecure, const char *szUser, const char *szPass );
			static void CloseHTTPConnection( uint32_t iHTTP );
			static void SetHTTPTimeout( uint32_t iHTTP, int milliseconds );
			static void SetHTTPVerifyCertificate( uint32_t iHTTP, int mode );

			static void AddHTTPHeader( uint32_t iHTTP, const char* headerName, const char* headerValue );
			static void RemoveHTTPHeader( uint32_t iHTTP, const char* headerName );
			
			static char* SendHTTPRequest( uint32_t iHTTP, const char *szServerFile ); //blocks
			static char* SendHTTPRequest( uint32_t iHTTP, const char *szServerFile, const char *szPostData ); //blocks
			static int SendHTTPRequestASync( uint32_t iHTTP, const char *szServerFile ); //does not block
			static int SendHTTPRequestASync( uint32_t iHTTP, const char *szServerFile, const char *szPostData ); //does not block
			static int SendHTTPFile( uint32_t iHTTP, const char *szServerFile, const char *szPostData, const char *szLocalFile ); //does not block
			static char* GetHTTPResponse( uint32_t iHTTP ); // 120112 - notice the shorter command goes first (for AGK compiler)
			static int GetHTTPResponseReady( uint32_t iHTTP );
			static const char* GetHTTPResponseC( uint32_t iHTTP );
			static int GetHTTPFile( uint32_t iHTTP, const char *szServerFile, const char *szLocalFile ); //does not block
			static int GetHTTPFile( uint32_t iHTTP, const char *szServerFile, const char *szLocalFile, const char *szPostData ); //does not block
			static int GetHTTPFileComplete( uint32_t iHTTP );
			static float GetHTTPFileProgress( uint32_t iHTTP );
			static int GetHTTPStatusCode( uint32_t iHTTP );
        
            static void OpenBrowser( const char* url );

			static uint32_t RunApp( const char *szFilename, const char *szParameters );
			static uint32_t GetAppRunning( uint32_t appID );
			static void TerminateApp( uint32_t appID );

			static void ViewFile( const char* szFilename );
			static void ShareText( const char* szText );
			static void ShareImage( const char* szFilename );
			static void ShareImageAndText( const char* szFilename, const char* szText );
			static void ShareFile( const char* szFilename );
		
			// string commands
			static char* Str( int value );
			static char* Str( float value );
			static char* Str( float value, uint32_t decimals );
			static char* Bin( int i );
			static char* Hex( int i );
			static char* StringToBase64( const char* input );
			static char* HexToBase64( const char* input );
			static int Val( const char* str );
			static int Val( const char* str, int base );
			static float ValFloat( const char* str );
			static char* Left ( const char* str, uint32_t count );
			static char* Right ( const char* str, uint32_t count );
			static char* Mid ( const char* str, uint32_t count, int length );
			static uint32_t Asc ( const char* str );
			static uint32_t Len ( const char* str );
			static uint32_t ByteLen ( const char* str );
			static char* Chr ( uint32_t iCharValue );
			static char* Lower ( const char* str );
			static char* Upper ( const char* str );
			static char* Spaces ( uint32_t count );
			static int FindStringCount( const char* str, const char* findStr );
			static int FindStringCount( const char* str, const char* findStr, int ignoreCase, int start );
			static int FindString( const char* str, const char* findStr );
			static int FindString( const char* str, const char* findStr, int ignoreCase, int start );
			static int FindStringReverse( const char* str, const char* findStr );
			static int FindStringReverse( const char* str, const char* findStr, int ignoreCase, int start );
			static int CompareString( const char* str, const char* str2 );
			static int CompareString( const char* str, const char* str2, int ignoreCase, int maxChars );
			static char* ReplaceString( const char* str, const char* find, const char* replace, int qty );
			static char* StripString( const char* str, const char* chars );
			static char* TrimString( const char* str, const char* chars );
			static char* TruncateString( const char* str, const char* character );
			static int CountStringTokens( const char* str, const char* delimiters );
			static char* GetStringToken( const char* str, const char* delimiters, int token );
			static int CountStringTokens2( const char* str, const char* delimiter );
			static char* GetStringToken2( const char* str, const char* delimiter, int token );

			// benchmarking
			static uint32_t GetManagedSpriteCount();
			static uint32_t GetManagedSpriteSortedCount();
			static uint32_t GetManagedSpriteDrawnCount();
			static uint32_t GetPixelsDrawn();
			static uint32_t GetManagedSpriteDrawCalls();

			static uint32_t GetParticleDrawnQuadCount();

			static float GetUpdateTime();
			static float GetPhysicsTime();
			static float GetDrawingSetupTime();
			static float GetDrawing3DSetupTime();
			static float GetDrawingTime();

			static uint32_t GetLoadedImages();
			static uint32_t GetUnassignedImages();
			static char* GetUnassignedImageFileName( uint32_t index );

			static float GetImageMemoryUsage();

			// date/time
			static int GetYearFromUnix64( int64_t unixtime, int* daysinyear=0 );
			static int GetMonthFromUnix64( int64_t unixtime );
			static int GetDayFromUnix64( int64_t unixtime );
			static int GetHoursFromUnix64( int64_t unixtime );
			static int GetMinutesFromUnix64( int64_t unixtime );
			static int GetSecondsFromUnix64( int64_t unixtime );
			static int64_t GetUnixFromDate64( int year, int month, int days, int hours, int minutes, int seconds );
			static int64_t GetUnixTime64();

			// tier 1 does not support 64 bit values, dates above 2038 will overflow
			static int GetYearFromUnix32( int unixtime, int* daysinyear=0 );
			static int GetYearFromUnix( int unixtime ); // 090112 - T1 not allowed - int* daysinyear=0 );
			static int GetMonthFromUnix( int unixtime );
			static int GetDaysFromUnix( int unixtime );
			static int GetHoursFromUnix( int unixtime );
			static int GetMinutesFromUnix( int unixtime );
			static int GetSecondsFromUnix( int unixtime );
			static int GetUnixFromDate( int year, int month, int days, int hours, int minutes, int seconds );
			static int GetUnixTime();

			static int GetLeapYear( int year );
			static char* GetCurrentDate();
			static char* GetCurrentTime();
			static int GetDayOfWeek();

			// adverts
			static void SetInneractiveDetails( const char* szCode );
            static void SetAdMobDetails( const char* szID );
			static void SetAdMobRewardAdDetails( const char* szID );
			static void SetChartboostDetails( const char* szKey1, const char* szKey2 );
			static void SetAmazonAdDetails( const char* szKey );

			static void LoadConsentStatusAdMob( const char* szPubID, const char* privacyPolicy );
			static int GetConsentStatusAdMob();
			static void RequestConsentAdMob();
			static void OverrideConsentAdMob( int consent );
			static void OverrideConsentChartboost( int consent );
        
            static void SetAmazonAdTesting( int mode );
			static void SetAdMobTesting( int mode );
			static void SetAdMobChildRating( int rating );

			static void ShowFullscreenAdvertAdMob();
			static void ShowFullscreenAdvertChartboost();
			static void ShowFullscreenAdvertAmazon();

			static int GetFullscreenAdvertLoadedAdMob();
			static int GetFullscreenAdvertLoadedChartboost();
			static int GetFullscreenAdvertLoadedAmazon();

			static void ShowRewardAdAdMob();
			static int GetRewardAdLoadedAdMob();
			static int GetRewardAdRewardedAdMob();
			static int GetRewardAdValueAdMob();
			static void ResetRewardAdMob();

			static void ShowRewardAdChartboost();
			static void CacheRewardAdChartboost();
			static int GetRewardAdLoadedChartboost();
			static int GetRewardAdRewardedChartboost();
			static void ResetRewardChartboost();

			static void CreateAdvert( int type, int horz, int vert, int test );
            static void CreateAdvertEx( int type, int horz, int vert, int test, float offsetx, float offsety );
			static void SetAdvertPosition( float x, float y, float width );
			static void SetAdvertLocation( int horz, int vert, float width );
            static void SetAdvertLocationEx( int horz, int vert, float offsetx, float offsety, float width );
            static void SetAdvertVisible( int iVisible );
            static void RequestAdvertRefresh ( void );
			static void DeleteAdvert( );

			// deprecated commands
			static void SetTransitionMode( int mode );
			static void CreateFullscreenAdvert(); 
			static void SetIntendedDeviceSize( int width, int height );
			static void Break();
			static void ClearDepthBuffer();
			static void SetSortTextures( int sort );
			static void SetSortDepth( int sort );
			static void SetSortTransparentDepth( int sort );
			static void SetSortCreated( int sort );
			static void EnableClearDepth( uint32_t clear );
			static void EnableClearColor( uint32_t clear );
			static char* GetDeviceName();
			static void SetRawWritePath( const char* str );
			static char* GetCL();
			static void SetShaderConstantArrayByName( uint32_t shaderID, const char* szName, uint32_t arrayIndex, float value1, float value2, float value3, float value4 );
			static uint32_t GetParticleDrawnPointCount();

			//
			// V108 - 
			//

			// log command (mainly for HTML5 but others can use it; output window)
			static void Log( const char *szMessage );
        
			// internal social functions
            static void SocialPluginsSetup ( void );
            static void SocialPluginsDestroy ( void );

			// youtube
			static void PlayYoutubeVideo( const char* developerKey, const char* videoID, float startTime );

			// video commands
			static int LoadVideo( const char *szFilename );
			static void DeleteVideo();
			static void SetVideoDimensions( float x, float y, float width, float height );
			static void PlayVideo();
			static void PlayVideoToImage( uint32_t imageID );
            static void PauseVideo(); 
			static void StopVideo();
			static int GetVideoPlaying();
			static float GetVideoPosition();
			static float GetVideoDuration();
			static void SetVideoVolume( float volume );
			static float GetVideoWidth();
			static float GetVideoHeight();
			static void VideoResumed();
			static void VideoUpdate();
			static void SetVideoPosition( float seconds );

			// screen recording
			static void StartScreenRecording( const char *szFilename, int microphone );
			static void StopScreenRecording();
			static int IsScreenRecording();
        
            // Smart watch
            static void ActivateSmartWatch( const char *szReserved );
            static int GetSmartWatchState();
            static void SendSmartWatchData( const char* szJson );
            static char* ReceiveSmartWatchData();

			// Text to speech
			static void TextToSpeechSetup();
			static int GetTextToSpeechReady();
			static void Speak( const char *text );
			static void Speak( const char *text, int delay );
			static void SetSpeechRate( float rate );
			static void SetSpeechLanguage( const char* lang );
			static void SetSpeechLanguageByID( const char* sID );
            static int GetSpeechNumVoices();
            static char* GetSpeechVoiceLanguage( int index );
            static char* GetSpeechVoiceName( int index );
			static char* GetSpeechVoiceID( int index );
			static int IsSpeaking();
			static void StopSpeaking();
        
			// RATING COMMANDS
			static void  RateApp                      ( const char* szID );
            static void  RateApp                      ( const char* szID, const char* title );
			static void  RateApp                      ( const char* szID, const char* title, const char* message );
			static void RequestAppReview(); // iOS only

			// IN APP PURCHASE COMMANDS
			static void  InAppPurchaseReset           (); // internal function
			static void  InAppPurchaseSetKeys         ( const char* szData1, const char* szData2 );
			static void  InAppPurchaseSetTitle        ( const char* szTitle );
			static void  InAppPurchaseAddProductID    ( const char* szID, int type );
			static void  InAppPurchaseSetup           ( void );
			static void  InAppPurchaseActivate        ( int iID );
			static void  InAppPurchaseActivateWithPlan( int iID, const char* planToken );
			static void  InAppPurchaseResetPurchase   ( const char* token );
			static void  InAppPurchaseRedeemOffer     ();
			static int   GetInAppPurchaseState        ( void );
			static int   GetInAppPurchaseAvailable    ( int iID );
			static int   GetInAppPurchaseAvailable2   ( int iID );
            static char* GetInAppPurchaseLocalPrice   ( int iID );
            static char* GetInAppPurchaseDescription  ( int iID );
			static int   GetInAppPurchaseIsRenewing   ( int iID );
			static void  InAppPurchaseRestore		  ();
			static char* GetInAppPurchaseSignature    ( int iID );
			static char* GetInAppPurchaseToken        ( int iID );
			static int   GetInAppPurchaseSubNumPlans  ( int iID );
			static int   GetInAppPurchaseSubPlanNumPeriods( int iID, int planIndex );
			static char* GetInAppPurchaseSubPlanPrice ( int iID, int planIndex, int periodIndex );
			static int   GetInAppPurchaseSubPlanDuration( int iID, int planIndex, int periodIndex );
			static char* GetInAppPurchaseSubPlanDurationUnit( int iID, int planIndex, int periodIndex );
			static int   GetInAppPurchaseSubPlanPaymentType( int iID, int planIndex, int periodIndex );
			static char* GetInAppPurchaseSubPlanTags  ( int iID, int planIndex );
			static char* GetInAppPurchaseSubPlanToken ( int iID, int planIndex );
			static char* GetAppReceipt();

			// FACEBOOK COMMANDS
			static void  FacebookSetup                ( const char* szID );
			static void  FacebookLogin                ( void );
			static void  FacebookLogout               ( void );
			static void  FacebookShowLikeButton       ( const char* szURL, int iX, int iY, int iWidth, int iHeight );
			static void  FacebookDestroyLikeButton    ( void );
			static void  FacebookPostOnMyWall         ( const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription );
			static void  FacebookPostOnFriendsWall    ( const char* szID, const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription );
			static void  FacebookInviteFriend         ( const char* szID, const char* szMessage );
			static void  FacebookGetFriends           ( void );
			static int   FacebookGetFriendsState      ( void );
			static int   FacebookGetFriendsCount      ( void );
			static char* FacebookGetFriendsName       ( int iIndex );
			static char* FacebookGetFriendsID         ( int iIndex );
			static void  FacebookDownloadFriendsPhoto ( int iIndex );
			static int   GetFacebookDownloadState     ( void );
			static char* GetFacebookDownloadFile      ( void );
			static int   GetFacebookLoggedIn          ( void );
			static char* FacebookGetUserID			  ( void );
			static char* FacebookGetUserName		  ( void );
			static char* FacebookGetAccessToken		  ( void );
			static void  FacebookActivateAppTracking  ( void );

			// TWITTER COMMANDS
			static void  TwitterSetup                 ( const char* szKey, const char* szSecret );
			static void  TwitterMessage               ( const char* szMessage );

			// NOTIFICATION COMMANDS
			static void  NotificationCreate           ( const char* szDateTime, const char* szMessage, const char* szData );
			static void  NotificationReset            ( void );
			static int   GetNotification              ( void );
			static char* GetNotificationData          ( void );

			// V108 Ultra notification (WinRT Metro-style)
			static int   GetNotificationType          ( void );
			static void  SetNotificationImage		  ( int iImageIndex );
			static void  SetNotificationText          ( const char* pText );

			// local notifications
			static void SetLocalNotification( int iID, int datetime, const char *szMessage, const char* szDeepLink );
			static void SetLocalNotification( int iID, int datetime, const char *szMessage );
			static void CancelLocalNotification( int iID );
			static int GetLocalNotificationExists( int iID );
			static int GetLocalNotificationTime( int iID );
			static char* GetLocalNotificationMessage( int iID );
			static void CheckLocalNotifications();

			// push notifications
			static void SetPushNotificationKeys( const char* data1, const char* reserved );
			static int PushNotificationSetup();
			static char* GetPushNotificationToken();

			// Android permissions
			static int CheckPermission( const char* szPermission );
			static void RequestPermission( const char* szPermission );

			// Shared variables
			static void SetSharedVariableAppGroup( const char* group );
			static void SaveSharedVariable( const char *varName, const char *varValue );
			static char* LoadSharedVariable( const char *varName, const char *defaultValue );
			static void DeleteSharedVariable( const char *varName );

			// Cloud data
			static void SetupCloudData( const char* reserved );
			static int GetCloudDataAllowed();
			static int GetCloudDataChanged();
			static char* GetCloudDataVariable( const char* varName, const char* defaultValue );
			static void SetCloudDataVariable( const char* varName, const char* varValue );
			static void DeleteCloudDataVariable( const char* varName );

			// Firebase
			static void FirebaseSetup();
			static void FirebaseLogEvent( const char *event_name );

			// zip functions
			static void CreateZip( uint32_t zipID, const char* filename );
			static void CreateZip(UINT zipID, const char* filename, int append);
			static uint32_t CreateZip( const char* filename );
			static void AddZipEntry( uint32_t zipID, const char* path, const char* zipPath );
			static void CloseZip( uint32_t zipID );
			static void ExtractZip( const char* zipfilename, const char* path);
			static void ExtractZip(const char* zipfilename, const char* path, const char* password);
			static void ExtractZipASync(const char* zipfilename, const char* path, const char* password);
			static float GetZipExtractProgress();
			static int GetZipExtractComplete();
			static void CancelZipExtract();

			// memblock functions (do not exist at this time)
			static uint32_t CreateMemblock( uint32_t size );
			static void CreateMemblock( uint32_t memID, uint32_t size );
			static int GetMemblockExists( uint32_t memID );
			static void DeleteMemblock( uint32_t memID );
			static unsigned char* GetMemblockPtr( uint32_t memID );
			static void CopyMemblock( uint32_t memSrcID, uint32_t memDstID, uint32_t srcOffset, uint32_t dstOffset, uint32_t size );
			static char* GetMemblockSHA1( uint32_t memID );
			static char* GetMemblockSHA256( uint32_t memID );
			
			static int GetMemblockSize( uint32_t memID );
			static int GetMemblockByte( uint32_t memID, uint32_t offset );
			static int GetMemblockByteSigned( uint32_t memID, uint32_t offset );
			static int GetMemblockShort( uint32_t memID, uint32_t offset );
			static int GetMemblockInt( uint32_t memID, uint32_t offset );
			static float GetMemblockFloat( uint32_t memID, uint32_t offset );
			static char* GetMemblockString( uint32_t memID, uint32_t offset, uint32_t length );

			static void SetMemblockByte( uint32_t memID, uint32_t offset, int value );
			static void SetMemblockByteSigned( uint32_t memID, uint32_t offset, int value );
			static void SetMemblockShort( uint32_t memID, uint32_t offset, int value );
			static void SetMemblockInt( uint32_t memID, uint32_t offset, int value );
			static void SetMemblockFloat( uint32_t memID, uint32_t offset, float value );
			static void SetMemblockString( uint32_t memID, uint32_t offset, const char* value );

			static void CreateMemblockFromImage( uint32_t memID, uint32_t imageID );
			static uint32_t CreateMemblockFromImage( uint32_t imageID );
			static void CreateImageFromMemblock( uint32_t imageID, uint32_t memID );
			static uint32_t CreateImageFromMemblock( uint32_t memID );
			static void CreateImageFromPNGMemblock( uint32_t imageID, uint32_t memID );
			static uint32_t CreateImageFromPNGMemblock( uint32_t memID );

			static void CreateMemblockFromSound( uint32_t memID, uint32_t soundID );
			static uint32_t CreateMemblockFromSound( uint32_t soundID );
			static void CreateSoundFromMemblock( uint32_t soundID, uint32_t memID );
			static uint32_t CreateSoundFromMemblock( uint32_t memID );
			static void CreateSoundFromOGGMemblock( uint32_t soundID, uint32_t memID );
			static uint32_t CreateSoundFromOGGMemblock( uint32_t memID );

			static void CreateMusicFromOGGMemblock( uint32_t musicID, uint32_t memID );
			static uint32_t CreateMusicFromOGGMemblock( uint32_t memID );

			static void CreateMemblockFromFile( uint32_t memID, const char* filename );
			static uint32_t CreateMemblockFromFile( const char* filename );
			static void CreateFileFromMemblock( const char* filename, uint32_t memID );

			static void CreateMemblockFromObjectMesh( uint32_t memID, uint32_t objID, uint32_t meshIndex );
			static uint32_t CreateMemblockFromObjectMesh( uint32_t objID, uint32_t meshIndex );
			static void CreateObjectFromMeshMemblock( uint32_t objID, uint32_t memID );
			static uint32_t CreateObjectFromMeshMemblock( uint32_t memID );
			static void SetObjectMeshFromMemblock( uint32_t objID, uint32_t meshIndex, uint32_t memID );
			static void AddObjectMeshFromMemblock( uint32_t objID, uint32_t memID );
			
			static void SetMeshMemblockVertexPosition( uint32_t memID, uint32_t vertexIndex, float x, float y, float z );
			static void SetMeshMemblockVertexNormal( uint32_t memID, uint32_t vertexIndex, float x, float y, float z );
			static void SetMeshMemblockVertexUV( uint32_t memID, uint32_t vertexIndex, float u, float v );
			static void SetMeshMemblockVertexColor( uint32_t memID, uint32_t vertexIndex, int red, int green, int blue, int alpha );
			static unsigned short InternalGetMemblockPosOffset( cMemblock *pMem );
			static unsigned short InternalGetMemblockNormOffset( cMemblock *pMem );
			static unsigned short InternalGetMemblockUVOffset( cMemblock *pMem );
			static unsigned short InternalGetMemblockColorOffset( cMemblock *pMem );
			static float GetMeshMemblockVertexX( uint32_t memID, uint32_t vertexIndex );
			static float GetMeshMemblockVertexY( uint32_t memID, uint32_t vertexIndex );
			static float GetMeshMemblockVertexZ( uint32_t memID, uint32_t vertexIndex );
			static float GetMeshMemblockVertexNormalX( uint32_t memID, uint32_t vertexIndex );
			static float GetMeshMemblockVertexNormalY( uint32_t memID, uint32_t vertexIndex );
			static float GetMeshMemblockVertexNormalZ( uint32_t memID, uint32_t vertexIndex );
			static float GetMeshMemblockVertexU( uint32_t memID, uint32_t vertexIndex );
			static float GetMeshMemblockVertexV( uint32_t memID, uint32_t vertexIndex );
			static int GetMeshMemblockVertexRed( uint32_t memID, uint32_t vertexIndex );
			static int GetMeshMemblockVertexGreen( uint32_t memID, uint32_t vertexIndex );
			static int GetMeshMemblockVertexBlue( uint32_t memID, uint32_t vertexIndex );
			static int GetMeshMemblockVertexAlpha( uint32_t memID, uint32_t vertexIndex );

			//
			// V108 ULTRA RAW COMMANDS
			//
			// Internal
			static void InitialiseSensorCommands	( void );
			static void FreeSensorCommands			( void );
			// Real Commands
			static int  GetNFCExists				( void );
			
			// V108 NFC
			static uint32_t GetRawNFCCount				( void );
			static uint32_t GetRawFirstNFCDevice		( void );
			static uint32_t GetRawNextNFCDevice			( void );
			static char* GetRawNFCName				( uint32_t iIndex );
			static int SendRawNFCData				( uint32_t iIndex, const char* pMessageText );
			static int GetRawNFCDataState			( uint32_t iIndex );
			static char* GetRawNFCData				( uint32_t iIndex );

			// GameCenter

			static int GetGameCenterExists();
			static void GameCenterSetup();
			static void GameCenterLogin();
			static void GameCenterLogout();
			static int GetGameCenterLoggedIn();
			static char* GetGameCenterPlayerID();
			static char* GetGameCenterPlayerDisplayName();
			static void GameCenterSubmitScore( int iScore, const char* szBoardID );
			static void GameCenterShowLeaderBoard ( const char* szBoardID ); 
			static void GameCenterSubmitAchievement ( const char* szAchievementID, int iPercentageComplete );
			static void GameCenterAchievementsShow ( );
			static void GameCenterAchievementsReset ( );

			// drawing commands
			static uint32_t MakeColor( uint32_t red, uint32_t green, uint32_t blue );
			static uint32_t MakeColor( uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			static uint32_t GetColorRed( uint32_t color );
			static uint32_t GetColorGreen( uint32_t color );
			static uint32_t GetColorBlue( uint32_t color );
			static uint32_t GetColorAlpha( uint32_t color );
			static void DrawLineInternal( float x, float y, float x2, float y2, uint32_t color1, uint32_t color2 );
			static void DrawLine( float x, float y, float x2, float y2, uint32_t red, uint32_t green, uint32_t blue );
			static void DrawLine( float x, float y, float x2, float y2, uint32_t color1, uint32_t color2 );
			static void DrawBox( float x, float y, float x2, float y2, uint32_t color1, uint32_t color2, uint32_t color3, uint32_t color4, int filled );
			static void DrawEllipse( float x, float y, float radiusx, float radiusy, uint32_t color1, uint32_t color2, int filled );

			//
			// V108 3D Commands
			//

			static void SetGlobal3DDepth( int depth );

			// 3d objects
			static void CreateObjectBox( uint32_t objID, float width, float height, float length );
			static uint32_t CreateObjectBox( float width, float height, float length );
			static void CreateObjectSphere( uint32_t objID, float diameter, int rows, int columns );
			static uint32_t CreateObjectSphere( float diameter, int rows, int columns );
			static uint32_t CreateObjectCapsule( float diameter, float height, int axis );
			static void CreateObjectCapsule( uint32_t objID, float diameter, float height, int axis );
			static void CreateObjectCone( uint32_t objID, float height, float diameter, int segments );
			static uint32_t CreateObjectCone( float height, float diameter, int segments );
			static void CreateObjectCylinder( uint32_t objID, float height, float diameter, int segments );
			static uint32_t CreateObjectCylinder( float height, float diameter, int segments );
			static void CreateObjectPlane( uint32_t objID, float width, float height );
			static uint32_t CreateObjectPlane( float width, float height );
			static void CreateObjectQuad( uint32_t objID );
			static uint32_t CreateObjectQuad();

			static void CreateObjectFromHeightMap( uint32_t objID, const char* szImageFile, float width, float height, float length, int smoothing, int split ); 
			static void CreateObjectFromRawHeightMap(uint32_t objID, const char* szImageFile, float width, float height, float length, int smoothing, int split, int rawWidth, int rawHeight );
			static uint32_t CreateObjectFromRawHeightMap( const char* szImageFile, float width, float height, float length, int smoothing, int split, int rawWidth, int rawHeight);
			static uint32_t CreateObjectFromHeightMap( const char* szImageFile, float width, float height, float length, int smoothing, int split );

			static uint32_t CreateObjectFromObjectMesh( uint32_t fromObjID, uint32_t meshIndex );
			static void CreateObjectFromObjectMesh( uint32_t objID, uint32_t fromObjID, uint32_t meshIndex );

			// for static objects
			static void LoadObject( uint32_t objID, const char *szFilename );
			static void LoadObject( uint32_t objID, const char *szFilename, float height );
			static uint32_t LoadObject( const char *szFilename );
			static uint32_t LoadObject( const char *szFilename, float height );

			// for animated objects
			static void LoadObjectWithChildren( uint32_t objID, const char *szFilename );
			static uint32_t LoadObjectWithChildren( const char *szFilename );

			static void SaveObject( uint32_t objID, const char *szFilename );

			static void CloneObject( uint32_t newobjID, uint32_t objID );
			static uint32_t CloneObject( uint32_t objID );
			static void InstanceObject( uint32_t newobjID, uint32_t objID );
			static uint32_t InstanceObject( uint32_t objID );

			static int GetObjectExists( uint32_t objID );
			static void DeleteObject( uint32_t objID );
			static void DeleteObjectWithChildren( uint32_t objID );
			static void DeleteObjectTree( uint32_t objID );
			static cObject3D* GetObjectPtr( uint32_t objID );
			static void DeleteAllObjects();

			static uint32_t GetObjectNumChildren( uint32_t objID );
			static uint32_t GetObjectChildID( uint32_t objID, int childIndex );

			static uint32_t GetObjectNumBones( uint32_t objID );
			static uint32_t GetObjectBoneByName( uint32_t objID, const char *name );

			// mesh
			static uint32_t GetObjectNumMeshes( uint32_t objID );
			static uint32_t GetObjectNumTextures(uint32_t objID);
			static char* GetObjectMeshName( uint32_t objID, uint32_t meshIndex );
			static char* GetObjectTextureName(uint32_t objID, uint32_t textureIndex);
			static void SetObjectMeshVisible(uint32_t objID, uint32_t meshIndex, int mode);
			static void SetObjectMeshCastShadow(uint32_t objID, uint32_t meshIndex, int mode);
			static void SetObjectMeshCollisionMode(uint32_t objID, int meshIndex, int mode);
			static void SetObjectMeshImage( uint32_t objID, uint32_t meshIndex, uint32_t imageID, uint32_t textureStage );
			static void SetObjectMeshLightMap( uint32_t objID, uint32_t meshIndex, uint32_t imageID );
			static void SetObjectMeshNormalMap( uint32_t objID, uint32_t meshIndex, uint32_t imageID );
			static void SetObjectMeshNormalMapScale( uint32_t objID, uint32_t meshIndex, float scaleU, float scaleV );
			static void SetObjectMeshShader( uint32_t objID, uint32_t meshIndex, uint32_t shaderID );
			static char* GetObjectMeshVSSource( uint32_t objID, uint32_t meshIndex );
			static char* GetObjectMeshPSSource( uint32_t objID, uint32_t meshIndex );
			static void SetObjectMeshUVOffset( uint32_t objID, uint32_t meshIndex, uint32_t textureStage, float offsetU, float offsetV );
			static void SetObjectMeshUVScale( uint32_t objID, uint32_t meshIndex, uint32_t textureStage, float scaleU, float scaleV );
			static float GetObjectMeshSizeMinX( uint32_t objID, uint32_t meshIndex );
			static float GetObjectMeshSizeMaxX( uint32_t objID, uint32_t meshIndex );
			static float GetObjectMeshSizeMinY( uint32_t objID, uint32_t meshIndex );
			static float GetObjectMeshSizeMaxY( uint32_t objID, uint32_t meshIndex );
			static float GetObjectMeshSizeMinZ( uint32_t objID, uint32_t meshIndex );
			static float GetObjectMeshSizeMaxZ( uint32_t objID, uint32_t meshIndex );

			static void FixObjectToObject( uint32_t objID, uint32_t toObjID );
			static void FixObjectToBone( uint32_t objID, uint32_t toObjID, uint32_t toBoneIndex );
			static void FixObjectToCamera( uint32_t objID, uint32_t camID );

			// animation
			static int GetObjectNumAnimations( uint32_t objID );
			static char* GetObjectAnimationName( uint32_t objID, int index );
			static void PlayObjectAnimation( uint32_t objID, const char *animName, float starttime, float endtime, int loop, float tweentime );
			static void SetObjectAnimationFrame(uint32_t objID, const char *animName, float time, float tweentime );
			static void StopObjectAnimation( uint32_t objID );
			static void ResetObjectAnimation( uint32_t objID );
			static void SetObjectAnimationSpeed(  uint32_t objID, float speed );
			static int GetObjectIsAnimating( uint32_t objID );
			static int GetObjectIsTweening( uint32_t objID );
			static float GetObjectAnimationTime( uint32_t objID );
			static float GetObjectAnimationDuration( uint32_t objID, const char *animName );

			// bone transform
			static void SetObjectBonePosition( uint32_t objID, uint32_t boneIndex, float x, float y, float z );
			static void SetObjectBoneRotation( uint32_t objID, uint32_t boneIndex, float angx, float angy, float angz );
			static void SetObjectBoneRotationQuat( uint32_t objID, uint32_t boneIndex, float w, float x, float y, float z );
			static void SetObjectBoneLookAt( uint32_t objID, uint32_t boneIndex, float x, float y, float z, float roll );
			static void SetObjectBoneCanAnimate( uint32_t objID, uint32_t boneIndex, int animate );

			static void RotateObjectBoneLocalX( uint32_t objID, uint32_t boneIndex, float amount );
			static void RotateObjectBoneLocalY( uint32_t objID, uint32_t boneIndex, float amount );
			static void RotateObjectBoneLocalZ( uint32_t objID, uint32_t boneIndex, float amount );

			static char* GetObjectBoneName( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneX( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneY( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneZ( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneAngleX( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneAngleY( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneAngleZ( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneQuatW( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneQuatX( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneQuatY( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneQuatZ( uint32_t objID, uint32_t boneIndex );

			static float GetObjectBoneWorldX( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldY( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldZ( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldAngleX( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldAngleY( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldAngleZ( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldQuatW( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldQuatX( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldQuatY( uint32_t objID, uint32_t boneIndex );
			static float GetObjectBoneWorldQuatZ( uint32_t objID, uint32_t boneIndex );

			// object transform
			static void SetObjectPosition( uint32_t objID, float x, float y, float z );
			static void SetObjectRotation( uint32_t objID, float angx, float angy, float angz );
			static void SetObjectRotationQuat( uint32_t objID, float w, float x, float y, float z );
			static void SetObjectScale( uint32_t objID, float x, float y, float z );
			static void SetObjectScalePermanent( uint32_t objID, float x, float y, float z );

			static void MoveObjectLocalX( uint32_t objID, float amount );
			static void MoveObjectLocalY( uint32_t objID, float amount );
			static void MoveObjectLocalZ( uint32_t objID, float amount );

			static void RotateObjectLocalX( uint32_t objID, float amount );
			static void RotateObjectLocalY( uint32_t objID, float amount );
			static void RotateObjectLocalZ( uint32_t objID, float amount );

			static void RotateObjectGlobalX( uint32_t objID, float amount );
			static void RotateObjectGlobalY( uint32_t objID, float amount );
			static void RotateObjectGlobalZ( uint32_t objID, float amount );

			static float GetObjectX( uint32_t objID );
			static float GetObjectY( uint32_t objID );
			static float GetObjectZ( uint32_t objID );
			static float GetObjectAngleX( uint32_t objID );
			static float GetObjectAngleY( uint32_t objID );
			static float GetObjectAngleZ( uint32_t objID );
			static float GetObjectQuatW( uint32_t objID );
			static float GetObjectQuatX( uint32_t objID );
			static float GetObjectQuatY( uint32_t objID );
			static float GetObjectQuatZ( uint32_t objID );

			static float GetObjectWorldX( uint32_t objID );
			static float GetObjectWorldY( uint32_t objID );
			static float GetObjectWorldZ( uint32_t objID );
			static float GetObjectWorldAngleX( uint32_t objID );
			static float GetObjectWorldAngleY( uint32_t objID );
			static float GetObjectWorldAngleZ( uint32_t objID );
			static float GetObjectWorldQuatW( uint32_t objID );
			static float GetObjectWorldQuatX( uint32_t objID );
			static float GetObjectWorldQuatY( uint32_t objID );
			static float GetObjectWorldQuatZ( uint32_t objID );

			static float GetObjectSizeMinX( uint32_t objID );
			static float GetObjectSizeMaxX( uint32_t objID );
			static float GetObjectSizeMinY( uint32_t objID );
			static float GetObjectSizeMaxY( uint32_t objID );
			static float GetObjectSizeMinZ( uint32_t objID );
			static float GetObjectSizeMaxZ( uint32_t objID );

			static void SetObjectLookAt( uint32_t objID, float x, float y, float z, float roll );
			static void FixObjectPivot( uint32_t objID );

			static float GetObjectHeightMapHeight( uint32_t objID, float x, float z );

			// object properties
			static void SetObjectImage( uint32_t objID, uint32_t imageID, uint32_t texStage );
			static void SetObjectLightMap( uint32_t objID, uint32_t imageID );
			static void SetObjectNormalMap( uint32_t objID, uint32_t imageID );
			static void SetObjectNormalMapScale( uint32_t objID, float scaleU, float scaleV );
			static void SetObjectShader( uint32_t objID, uint32_t shaderID );
			static void SetObjectColor( uint32_t objID, int red, int green, int blue, int alpha );
			static void SetObjectAlpha( uint32_t objID, int alpha );
			static void SetObjectColorEmissive( uint32_t objID, int red, int green, int blue );
			static void SetObjectLightMode( uint32_t objID, int mode );
			static void SetObjectScreenCulling( uint32_t objID, int mode );
			static void SetObjectUVOffset( uint32_t objID, uint32_t textureStage, float offsetU, float offsetV );
			static void SetObjectUVScale( uint32_t objID, uint32_t textureStage, float scaleU, float scaleV );
			static void SetObjectFogMode( uint32_t objID, int mode );
			static void SetObjectCastShadow( int objID, int mode );
			static void SetObjectReceiveShadow( int objID, int mode );

			static void SetObjectDepthReadMode( uint32_t objID, int mode );
			static void SetObjectDepthWrite( uint32_t objID, int mode );
			static void SetObjectDepthBias( uint32_t objID, float bias );
			static void SetObjectDepthRange( uint32_t objID, float zNear, float zFar );
			static void SetObjectTransparency( uint32_t objID, int mode );
			static void SetObjectBlendModes( uint32_t objID, int src, int dst );
			static void SetObjectAlphaMask( uint32_t objID, int mode );
			static void SetObjectCullMode( uint32_t objID, int mode );
			static void SetObjectVisible( uint32_t objID, int mode );

			static int GetObjectDepthReadMode( uint32_t objID );
			static int GetObjectDepthWrite( uint32_t objID );
			static float GetObjectDepthBias( uint32_t objID );
			static int GetObjectTransparency( uint32_t objID );
			static int GetObjectCullMode( uint32_t objID );
			static int GetObjectVisible( uint32_t objID );
			static int GetObjectInScreen( uint32_t objID );
			static int GetObjectCastShadowMode( int objID );
			static int GetObjectReceiveShadowMode( int objID );
			static int GetObjectColorRed( int objID );
			static int GetObjectColorGreen( int objID );
			static int GetObjectColorBlue( int objID );
			static int GetObjectAlpha( int objID );

			static char* GetObjectName( uint32_t objID );
			
			static void SetObjectShaderConstantByName( uint32_t objID, const char* szName, float value1, float value2, float value3, float value4 );
			static void SetObjectShaderConstantArrayByName( uint32_t objID, const char* szName, uint32_t arrayIndex, float value1, float value2, float value3, float value4 );
			static void SetObjectShaderConstantArrayFloatByName( uint32_t objID, const char* szName, uint32_t arrayIndex, float value );
			static void SetObjectShaderConstantArrayVec2ByName( uint32_t objID, const char* szName, uint32_t arrayIndex, float value1, float value2 );
			static void SetObjectShaderConstantArrayVec3ByName( uint32_t objID, const char* szName, uint32_t arrayIndex, float value1, float value2, float value3 );
			static void SetObjectShaderConstantArrayVec4ByName( uint32_t objID, const char* szName, uint32_t arrayIndex, float value1, float value2, float value3, float value4 );
			static void SetObjectShaderConstantDefault( uint32_t objID, const char* szName );

			static void DrawObject( uint32_t objID );

			static float GetScreenXFrom3D( float x, float y, float z );
			static float GetScreenYFrom3D( float x, float y, float z );

			static float Get3DVectorXFromScreen( float x, float y );
			static float Get3DVectorYFromScreen( float x, float y );
			static float Get3DVectorZFromScreen( float x, float y );

			// collision
			static void SetObjectCollisionMode( uint32_t objID, int mode );
			static bool InternalSphereCast( uint32_t objID, const AGKVector &p, const AGKVector &v, float radius );
			static int ObjectRayCast( uint32_t objID, float oldx, float oldy, float oldz, float newx, float newy, float newz );
			static int ObjectSphereCast( uint32_t objID, float oldx, float oldy, float oldz, float newx, float newy, float newz, float radius );
			static int ObjectSphereSlide( uint32_t objID, float oldx, float oldy, float oldz, float newx, float newy, float newz, float radius );
			static uint32_t GetObjectRayCastNumHits();
			static uint32_t GetObjectRayCastHitID( uint32_t index );
			static float GetObjectRayCastX( uint32_t index );
			static float GetObjectRayCastY( uint32_t index );
			static float GetObjectRayCastZ( uint32_t index );
			static float GetObjectRayCastSlideX( uint32_t index );
			static float GetObjectRayCastSlideY( uint32_t index );
			static float GetObjectRayCastSlideZ( uint32_t index );
			static float GetObjectRayCastNormalX( uint32_t index );
			static float GetObjectRayCastNormalY( uint32_t index );
			static float GetObjectRayCastNormalZ( uint32_t index );
			static float GetObjectRayCastBounceX( uint32_t index );
			static float GetObjectRayCastBounceY( uint32_t index );
			static float GetObjectRayCastBounceZ( uint32_t index );
			static float GetObjectRayCastDistance( uint32_t index );

			// fog
			static void SetFogMode( int mode );
			static void SetFogColor( int red, int green, int blue );
			static void SetFogSunColor( int red, int green, int blue );
			static void SetFogRange( float minDist, float maxDist );

			static int GetFogMode();
			static int GetFogColorsEqual();

			// sky box
			static void SetSkyBoxVisible( int active );
			static void SetSkyBoxSkyColor( int red, int green, int blue );
			static void SetSkyBoxHorizonColor( int red, int green, int blue );
			static void SetSkyBoxSunColor( int red, int green, int blue );
			static void SetSkyBoxHorizonSize( float size, float height );
			static void SetSkyBoxSunVisible( int visible );
			static void SetSkyBoxSunSize( float sun, float halo );

			// shaders
			static void SetShaderErrorMode( uint32_t mode );
			static void LoadShader( uint32_t shaderID, const char* szVertexFile, const char* szPixelFile );
			static uint32_t LoadShader( const char* szVertexFile, const char* szPixelFile );
			static void LoadShaderFromString( uint32_t shaderID, const char* szVertexSource, const char* szPixelSource );
			static uint32_t LoadShaderFromString( const char* szVertexSource, const char* szPixelSource );
			static void LoadFullScreenShader( uint32_t shaderID, const char* szPixelFile );
			static uint32_t LoadFullScreenShader( const char* szPixelFile );
			static void LoadSpriteShader( uint32_t shaderID, const char* szPixelFile );
			static uint32_t LoadSpriteShader( const char* szPixelFile );
			static void SetShaderConstantByName( uint32_t shaderID, const char* szName, float value1, float value2, float value3, float value4 );
			static void SetShaderConstantArrayFloatByName( uint32_t shaderID, const char* szName, uint32_t arrayIndex, float value );
			static void SetShaderConstantArrayVec2ByName( uint32_t shaderID, const char* szName, uint32_t arrayIndex, float value1, float value2 );
			static void SetShaderConstantArrayVec3ByName( uint32_t shaderID, const char* szName, uint32_t arrayIndex, float value1, float value2, float value3 );
			static void SetShaderConstantArrayVec4ByName( uint32_t shaderID, const char* szName, uint32_t arrayIndex, float value1, float value2, float value3, float value4 );
			static int GetShaderExists( uint32_t shaderID );
			static void DeleteShader( uint32_t shaderID );

			// cameras
			static void SetCameraPosition( uint32_t cameraID, float x, float y, float z );
			static void SetCameraRotationQuat( uint32_t cameraID, float w, float x, float y, float z );
			static void SetCameraRotation( uint32_t cameraID, float angx, float angy, float angz );

			static void MoveCameraLocalX( uint32_t cameraID, float amount );
			static void MoveCameraLocalY( uint32_t cameraID, float amount );
			static void MoveCameraLocalZ( uint32_t cameraID, float amount );

			static void RotateCameraLocalX( uint32_t cameraID, float amount );
			static void RotateCameraLocalY( uint32_t cameraID, float amount );
			static void RotateCameraLocalZ( uint32_t cameraID, float amount );

			static void RotateCameraGlobalX( uint32_t cameraID, float amount );
			static void RotateCameraGlobalY( uint32_t cameraID, float amount );
			static void RotateCameraGlobalZ( uint32_t cameraID, float amount );

			static float GetCameraX( uint32_t cameraID );
			static float GetCameraY( uint32_t cameraID );
			static float GetCameraZ( uint32_t cameraID );
			static float GetCameraAngleX( uint32_t cameraID );
			static float GetCameraAngleY( uint32_t cameraID );
			static float GetCameraAngleZ( uint32_t cameraID );
			static float GetCameraQuatW( uint32_t cameraID );
			static float GetCameraQuatX( uint32_t cameraID );
			static float GetCameraQuatY( uint32_t cameraID );
			static float GetCameraQuatZ( uint32_t cameraID );

			static float GetCameraWorldX( uint32_t cameraID );
			static float GetCameraWorldY( uint32_t cameraID );
			static float GetCameraWorldZ( uint32_t cameraID );
			static float GetCameraWorldAngleX( uint32_t cameraID );
			static float GetCameraWorldAngleY( uint32_t cameraID );
			static float GetCameraWorldAngleZ( uint32_t cameraID );
			static float GetCameraWorldQuatW( uint32_t cameraID );
			static float GetCameraWorldQuatX( uint32_t cameraID );
			static float GetCameraWorldQuatY( uint32_t cameraID );
			static float GetCameraWorldQuatZ( uint32_t cameraID );

			static void SetCameraLookAt( uint32_t cameraID, float x, float y, float z, float roll );
			static void SetCameraRange( uint32_t cameraID, float fNear, float fFar );
			static void SetCameraFOV( uint32_t cameraID, float fov );
			static void SetCameraAspect( uint32_t cameraID, float aspect );
			static void SetCameraOrthoWidth( uint32_t cameraID, float width );
			static void SetCameraBounds( uint32_t cameraID, float left, float right, float top, float bottom );
			static void SetCameraOffCenter( uint32_t cameraID, int mode );

			static float GetCameraFOV( uint32_t cameraID );

			static void FixCameraToObject( uint32_t cameraID, uint32_t objID );

			static void SetCurrentCamera( cCamera *pCamera );
			static cCamera* GetCurrentCamera() { return m_pCurrentCamera; }

			// lights
			static void CreatePointLight( uint32_t lightID, float x, float y, float z, float radius, int red, int green, int blue );
			static int GetPointLightExists( uint32_t lightID );
			static void DeletePointLight( uint32_t lightID );
			static void ClearPointLights();
			static void SetPointLightPosition( uint32_t lightID, float x, float y, float z );
			static void SetPointLightColor( uint32_t lightID, int red, int green, int blue );
			static void SetPointLightRadius( uint32_t lightID, float radius );
			static void SetPointLightMode( uint32_t lightID, int mode );

			// only allow one directional light
			//static void CreateLightDirectional( uint32_t lightID, float vx, float vy, float vz, uint32_t red, uint32_t green, uint32_t blue );
			//static int GetLightDirectionalExists( uint32_t lightID );
			//static void DeleteLightDirectional( uint32_t lightID );
			//static void ClearLightDirectionals();
			static void SetSunDirection( float vx, float vy, float vz );
			static void SetSunColor( int red, int green, int blue );
			static void SetSunActive( int active );

			static void SetAmbientColor( int red, int green, int blue );

			// Shadows
			static int GetShadowMappingSupported();
			static void SetShadowMappingMode( int mode );
			static int GetShadowMappingMode();
			static void SetShadowSmoothing( int mode );
			static int GetShadowSmoothing();
			static void SetShadowMapSize( int width, int height );
			static void SetShadowRange( float range );
			static void SetShadowBias( float bias );
			static void SetShadowLightStepSize( float step );
			static void SetShadowCascadeValues( float cascade1, float cascade2, float cascade3 );

			// 3D Particles

			static void Create3DParticles( uint32_t ID, float x, float y, float z );
			static uint32_t Create3DParticles( float x, float y, float z );
			static int Get3DParticlesExists( uint32_t ID );
			static void Delete3DParticles( uint32_t ID );
			static void Set3DParticlesPosition( uint32_t ID, float x, float y, float z );
			static void Set3DParticlesFrequency( uint32_t ID, float freq );
			static void Set3DParticlesStartZone( uint32_t ID, float x1, float y1, float z1, float x2, float y2, float z2 );
			static void Set3DParticlesDirection( uint32_t ID, float vx, float vy, float vz, float roll );
			static void Set3DParticlesVelocityRange( uint32_t ID, float v1, float v2 );
			static void Set3DParticlesDirectionRange( uint32_t ID, float angle, float angle2 );
			//static void Set3DParticlesFaceDirection( uint32_t ID, int mode );
			static void Set3DParticlesSize( uint32_t ID, float size );
			static void Set3DParticlesLife( uint32_t ID, float time );
			static void Set3DParticlesMax( uint32_t ID, int max );
			static void Reset3DParticleCount( uint32_t ID );
			static void Set3DParticlesImage( uint32_t ID, uint32_t imageID );
			static void Set3DParticlesColorInterpolation( uint32_t ID, int mode );
			static void Set3DParticlesVisible( uint32_t ID, int visible );
			static void Set3DParticlesActive( uint32_t ID, int active );
			static void Set3DParticlesTransparency( uint32_t ID, int mode );

			static float Get3DParticlesX( uint32_t ID );
			static float Get3DParticlesY( uint32_t ID );
			static float Get3DParticlesZ( uint32_t ID );
			static int Get3DParticlesVisible( uint32_t ID );
			static int Get3DParticlesActive( uint32_t ID );
			static float Get3DParticlesFrequency( uint32_t ID );
			static float Get3DParticlesDirectionX( uint32_t ID );
			static float Get3DParticlesDirectionY( uint32_t ID );
			static float Get3DParticlesDirectionZ( uint32_t ID );
			static float Get3DParticlesDirectionRange1( uint32_t ID );
			static float Get3DParticlesDirectionRange2( uint32_t ID );
			static float Get3DParticlesSize( uint32_t ID );
			static float Get3DParticlesLife( uint32_t ID );
			static int Get3DParticlesMaxReached( uint32_t ID );

			static void Add3DParticlesForce( uint32_t ID, float starttime, float endtime, float x, float y, float z );
			static void Clear3DParticlesForces( uint32_t ID );
			static void Add3DParticlesColorKeyFrame( uint32_t ID, float time, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			static void Clear3DParticlesColors( uint32_t ID );
			static void Add3DParticlesScaleKeyFrame( uint32_t ID, float time, float scale );
			static void Clear3DParticlesScales( uint32_t ID );

			static void Update3DParticles( uint32_t ID, float time );
			static void Offset3DParticles( uint32_t ID, float x, float y, float z );

			static void Draw3DParticles( uint32_t ID );

			//3D Physics Commands

			//3D Physics World Commands
			static void Create3DPhysicsWorld();
			static void Create3DPhysicsWorld( float scaleFactor );
			static void Set3DPhysicsGravity( float x, float y, float z );
			static void Set3DPhysicsGravity( uint32_t vectorID );
			static void Step3DPhysicsWorld();
			static void Reset3DPhysicsWorld();
			static void Delete3DPhysicsWorld();
			static void Debug3DPhysicsWorld();// Can not use Debugger at this time.Unable to draw wireframe.
			static int Get3DPhysicsTotalObjects();
			static int Get3DPhysicsActiveObjects();
			static int Get3DPhysicsTotalJoints();

			//CollisionShapes
			static void SetObjectShapeBox( uint32_t objID );
			static void SetObjectShapeBox( uint32_t objID, float sizeX, float sizeY, float sizeZ );
			static void SetObjectShapeBox( uint32_t objID, uint32_t vectorID );
			static void SetObjectShapeSphere( uint32_t objID );
			static void SetObjectShapeSphere( uint32_t objID, float diameter );
			static void SetObjectShapeCylinder( uint32_t objID, int axis );
			static void SetObjectShapeCylinder( uint32_t objID, int axis, float height, float diameter );
			static void SetObjectShapeCone( uint32_t objID, int axis );
			static void SetObjectShapeCone( uint32_t objID, int axis, float height, float diameter );
			static void SetObjectShapeCapsule( uint32_t objID, int axis );
			static void SetObjectShapeCapsule( uint32_t objID, int axis, float sizeX, float sizeY, float sizeZ );
			static void SetObjectShapeCapsule( uint32_t objID, int axis, uint32_t vectorID );
			static void SetObjectShapeConvexHull( uint32_t objID );
			static void SetObjectShapeStaticPolygon( uint32_t objID );
			static void SetObjectShapeCompound( uint32_t objID );

			//RigidBodies
			static void Create3DPhysicsDynamicBody( uint32_t objID );
			static void Create3DPhysicsStaticBody( uint32_t objID );
			static void Create3DPhysicsKinematicBody( uint32_t objID ); 
			static void Delete3DPhysicsBody( uint32_t objID );
			static int Create3DPhysicsStaticPlane ( float normlX, float normalY, float normalZ, float offsetPosition );
			static void Set3DPhysicsStaticPlanePosition( uint32_t planeID, float posX, float posY, float posZ );
			static void Set3DPhysicsStaticPlaneRotation( uint32_t planeID, float angX, float angY, float angZ );
			static void Delete3DPhysicsStaticPlane( uint32_t planeID );
			static void SetObject3DPhysicsGroupAndMask( uint32_t objID, int group, int mask );
			static int GetObject3DPhysicsGroup( uint32_t objID );
			static int GetObject3DPhysicsMask( uint32_t objID );
			static void SetObject3DPhysicsCanSleep( uint32_t objID, int canSleep );
			static void SetObject3DPhysicsMass( uint32_t objID, float mass );
			static float GetObject3DPhysicsMass( uint32_t objID );
			static void SetObject3DPhysicsFriction( uint32_t objID, float friction );
			static float GetObject3DPhysicsFriction( uint32_t objID );
			static void SetObject3DPhysicsRollingFriction( uint32_t objID, float friction );
			static float GetObject3DPhysicsRollingFriction( uint32_t objID );
			static void SetObject3DPhysicsAnisotropicFriction( uint32_t objID, int type );
			static void SetObject3DPhysicsMaxLinearVelocity( uint32_t objID, float maxLinearVelocity );
			static float GetObject3DPhysicsMaxLinearVelocity( uint32_t objID );
			static void SetObject3DPhysicsLinearVelocity( uint32_t objID, float dirX, float dirY, float dirZ, float initialSpeed );
			static void SetObject3DPhysicsLinearVelocity( uint32_t objID, uint32_t vectorID, float initialSpeed );
			static float GetObject3DPhysicsLinearVelocityX( uint32_t objID );
			static float GetObject3DPhysicsLinearVelocityY( uint32_t objID );
			static float GetObject3DPhysicsLinearVelocityZ( uint32_t objID );
			static void SetObject3DPhysicsAngularVelocity( uint32_t objID, float angX, float angY, float angZ, float initialSpeed );
			static void SetObject3DPhysicsAngularVelocity( uint32_t objID, uint32_t vectorID, float initialSpeed );
			static float GetObject3DPhysicsAngularVelocityX( uint32_t objID );
			static float GetObject3DPhysicsAngularVelocityY( uint32_t objID );
			static float GetObject3DPhysicsAngularVelocityZ( uint32_t objID );
			static void SetObject3DPhysicsDamping( uint32_t objID, float linearDamp, float angularDamp );
			static float GetObject3DPhysicsLinearDamp( uint32_t objID );
			static float GetObject3DPhysicsAngularDamp( uint32_t objID );
			static void SetObject3DPhysicsSleepingThreshold( uint32_t objID, float angular, float linear );
			static float GetObject3DPhysicsAngularSleepingThreshold( uint32_t objID );
			static float GetObject3DPhysicsLinearSleepingThreshold( uint32_t objID );
			static void SetObject3DPhysicsDeactivationTime( uint32_t objID, float time );
			static void SetObject3DPhysicsRestitution( uint32_t objID, float friction);
			static float GetObject3DPhysicsRestitution(  uint32_t objID );

			//ContactReports
			static int GetObject3DPhysicsFirstContact( uint32_t objID ); 
			static float GetObject3DPhysicsContactX(); 
			static float GetObject3DPhysicsContactY(); 
			static float GetObject3DPhysicsContactZ(); 
			static int GetObject3DPhysicsContactVector( int outPosVec3 );
			static int GetObject3DPhysicsContactObjectB();
			static int GetObject3DPhysicsNextContact();
			static int GetObjects3DPhysicsContactPositionVector( uint32_t objA, uint32_t objB, int outPosVec3 );

			//Joints/
			static int Create3DPhysicsPickJoint( uint32_t objID, int positionVec3 );
			static void Update3DPhysicsPickJoint( uint32_t jointID, int positionVec3 );
			static void Delete3DPhysicsPickJoint( uint32_t jointID );
			static int Create3DPhysicsHingeJoint( uint32_t objA, uint32_t objB, int positionVec3, int rotationVec3, int disableCollisions );
			static int Create3DPhysicsConeTwistJoint( uint32_t objA, uint32_t objB, int positionVec3, int rotationVec3, int disableCollisions );
			static int Create3DPhysicsSliderJoint( uint32_t objA, uint32_t objB, int positionVec3, int rotationVec3 );
			static int Create3DPhysicsFixedJoint( uint32_t objA, uint32_t objB, int positionVec3 );
			static int Create3DPhysics6DOFJoint( uint32_t objA, uint32_t objB, int positionVec3, float rotationVec3 );
			static void Set3DPhysicsJointSliderAngularLimits( uint32_t jointID, float lowerLimit, float upperLimit );
			static void Set3DPhysicsJointSliderLinearLimits( uint32_t jointID, float lowerLimit, float upperLimit );
			static void Set3DPhysicsJointConeTwistLimits( uint32_t jointID, float swingSpan1, float swingSpan2, float twistSpan );
			static void Set3DPhysicsJointHingeLimits( uint32_t jointID, float minAng, float maxAng );
			static void Set3DPhysicsJointBreakingThreshold( uint32_t jointID, float breakThreshold );
			static void Set3DPhysicsJointEnabled( uint32_t jointID, int isEnabled );
			static int Get3DPhysicsJointEnabled( uint32_t jointID );
			static int Get3DPhysicsJointPositionVector( uint32_t jointID );
			static int Get3DPhysicsJointRotationVector( uint32_t jointID );
			static void Delete3DPhysicsJoint( uint32_t jointID );

			//Joint Motors
			static void Set3DPhysicsHingeJointMotorIsEnabled( uint32_t jointID, int isEnabled );
		    static void Set3DPhysicsHingeJointMaxMotorImpulse( uint32_t jointID, float maxImpulse );
			static void Set3DPhysicsHingeJointMotorVelocity( uint32_t jointID, float targetVelocity );
			static void Set3DPhysicsTwistJointMotorIsEnabled( uint32_t jointID, int isEnabled );
			static void Set3DPhysicsTwistJointMaxMotorImpulse( uint32_t jointID, float maxImpulse );
			static void Set3DPhysicsTwistJointMotorRotationTarget( uint32_t jointID, int rotationVec3ID );
			static void Set3DPhysicsSliderJointPoweredLinearMotorIsEnabled( uint32_t jointID, int isEnabled );
			static void Set3DPhysicsSliderJointMaxLinearMotorForce( uint32_t jointID, float maxLinearForce );
			static void Set3DPhysicsSliderJointTargetLinearMotorVelocity( uint32_t jointID, float linearMotorVelocity );

			//CompoundCollisionShapes/
			static void AddObjectShapeBox( int objID, int positionVec3, int rotationVec3, int sizeVec3 );
			static void AddObjectShapeSphere( int objID, int positionVec3, float diameter );
			static void AddObjectShapeCapsule( int objID, int positionVec3, int rotationVec3, int sizeVec3, int axis );
			static void AddObjectShapeCone( int objID, int positionVec3, int rotationVec3, int sizeVec3, int axis);
			static void AddObjectShapeCylinder( int objID, int positionVec3, int rotationVec3, int sizeVec3, int axis);

			//SavingAndLoading/
			static int SaveObjectShape( uint32_t objID, const char* fileName );
			static int LoadObjectShape( uint32_t objID, const char* fileName );

			//Vector commands
			static int CreateVector3();
			static int CreateVector3( float x, float y, float z );
			static void SetVector3( uint32_t vectorID, float x, float y, float z );
			static void DeleteVector3( uint32_t vectorID );
			static float GetVector3X( uint32_t vectorID );
			static float GetVector3Y( uint32_t vectorID );
			static float GetVector3Z( uint32_t vectorID );
			static float GetVector3Distance( uint32_t vectorU, uint32_t vectorV );
			static float GetVector3Length( uint32_t vectorID );
			static float GetVector3Dot( uint32_t vectorU, uint32_t vectorV );
			static void GetVector3Cross( uint32_t resultVec, uint32_t vectorU, uint32_t vectorV );
			static void GetVector3Multiply( uint32_t resultVec, float multiplier );
			static void GetVector3Add( uint32_t resultVec, uint32_t addVec );

			//***************************************** MileStone3 **************************************************************
			//RayCasting/
			static int Create3DPhysicsRay();
			static void Delete3DPhysicsRay( uint32_t rayID );
			static int Ray3DPhysicsExist( uint32_t rayID );
			static void RayCast3DPhysics( uint32_t rayID, int fromVec3ID, int toVec3ID, int allOrClosest );
			static int RayCast3DPhysicsObject( uint32_t objID, uint32_t rayID, int fromVec3ID, int toVec3ID, int allOrClosest );
			static int Get3DPhysicsRayCastObjectHit( uint32_t rayID, float fractionIndex );
			static int Get3DPhysicsRayCastClosestObjectHit( uint32_t rayID );
			static float Get3DPhysicsRayCastFraction( uint32_t rayID );
			static int Get3DPhysicsRayCastContactPosition( uint32_t rayID, float fractionIndex, int outVec3ID );
			static int Get3DPhysicsRayCastClosestContactPosition( uint32_t rayID, int outVec3ID );
			static void Get3DPhysicsRayCastNormalVector( uint32_t rayID, int returnVec3ID );
			static int Get3DPhysicsRayCastNumHits( uint32_t rayID );
			static void SphereCast3DPhysics( uint32_t rayID, int fromVec3ID, int toVec3ID, float radius );
			static int SphereCast3DPhysicsObject( uint32_t objID, uint32_t rayID, int fromVec3ID, int toVec3ID, float radius );
			
			//CharacterController/
			static void Create3DPhysicsCharacterController( uint32_t objID, int axis, int objOffsetVec3, int objOrientationVec3, float crouchScale );
			static void Delete3DPhysicsCharacterController( uint32_t objID );
			static int Get3DPhysicsCharacterControllerExists( uint32_t objID );
			static void Set3DPhysicsCharacterControllerGravity( uint32_t objID, float gravity );
			static void Set3DPhysicsCharacterControllerFallSpeed( uint32_t objID, float fallSpeed );
			static void Set3DPhysicsCharacterControllerJumpSpeed( uint32_t objID, float jumpSpeed );
			static void Set3DPhysicsCharacterControllerMaxSlope( uint32_t objID, float maxSlopeDegress );
			static void Set3DPhysicsCharacterControllerStepHeight( uint32_t objID, float stepHeight );
			static float Get3DPhysicsCharacterControllerMaxSlope( uint32_t objID );
			static float Get3DPhysicsCharacterControllerGravity( uint32_t objID );
			static void Move3DPhysicsCharacterController( uint32_t objID, int direction, float velocity );
			static void Move3DPhysicsCharacterController( uint32_t objID, float x, float z, float velocity );
			static void Rotate3DPhysicsCharacterController( uint32_t objID, float angle );
			static void Jump3DPhysicsCharacterController( uint32_t objID );
			static void Crouch3DPhysicsCharacterController( uint32_t objID );
			static void Stand3DPhysicsCharacterController( uint32_t objID );
			static void Debug3DPhysicsCharacterController( uint32_t objID, int isDebug );
			static void Set3DPhysicsCharacterControllerPosition( uint32_t objID, float posX, float posY, float posZ );

			//Ragdoll/
			static void Create3DPhysicsRagDoll( uint32_t objID, float objTotalWeight );
			static int Add3DPhysicsRagDollBone( uint32_t startBoneID, uint32_t endBoneID, float diameter, int collisionGroup, int collisionMask );
			static void AssignTo3DPhysicsRagDollBoneObjectBone( uint32_t ragdollBoneID, uint32_t objBoneID );
			static void Add3DPhysicsRagDollHingeJoint( uint32_t boneAID, uint32_t boneBID, uint32_t objBoneID, int jointRotationVec3, float minLimit, float maxLimit );
			static void Add3DPhysicsRagDollTwistJoint( uint32_t boneAID, uint32_t boneBID, uint32_t objBoneID, int jointRotationVec3, int limitsVec3 );
			static void Finalize3DPhysicsRagDoll();
			static void Delete3DPhysicsRagdoll( uint32_t objID );
			static int Get3DPhysicsRagdollExist( uint32_t objID );
			static int Get3DPhysicsRagdollFromBoneObject( uint32_t objID );
			static int Is3dPhysicsRagdollStatic( uint32_t objID );
			static void Set3DphysicsRagdollStatic( uint32_t objID, int isStatic );
			static void Set3DPhysicsRagdollBonesVisible( uint32_t objID, int isVisible );
			static void Set3DPhysicsRagdollDamping( float linear, float angular );
			static void Set3DPhysicsRagdollSleepingThresholds( float linear, float angular );
			static void Set3DPhysicsRagdollDeactivationTime( float time );
			static void Set3DPhysicsRagdollDeactivation( uint32_t objID, int isDisabled );
		

			// AR
			static void ARSetup();
			static int ARGetStatus();
			static void ARUpdateInternal();
			static void ARPause();
			static void ARResume();
			static void ARDestroy();
			static void ARControlCamera();
			static void ARDrawBackground();
			static void ARSetPlaneDetectionMode( int mode );
			static void ARSetLightEstimationMode( int mode );

			static float ARGetLightEstimate();

			static int ARHitTest( float screenX, float screenY );
			static float ARGetHitTestX( int index );
			static float ARGetHitTestY( int index );
			static float ARGetHitTestZ( int index );
			static float ARGetHitTestNormalX( int index );
			static float ARGetHitTestNormalY( int index );
			static float ARGetHitTestNormalZ( int index );
			static int ARGetHitTestType( int index );
			static void ARHitTestFinish();

			static int ARGetPlanes( int reserved );
			static float ARGetPlaneX( int index );
			static float ARGetPlaneY( int index );
			static float ARGetPlaneZ( int index );
			static float ARGetPlaneAngleX( int index );
			static float ARGetPlaneAngleY( int index );
			static float ARGetPlaneAngleZ( int index );
			static float ARGetPlaneSizeX( int index );
			static float ARGetPlaneSizeZ( int index );
			static void ARGetPlanesFinish();
			
			static int ARCreateAnchorFromHitTest( int index );
			static int ARCreateAnchorFromPlane( int index );
			static void ARFixObjectToAnchor( int objID, int anchorID );
			static int ARGetAnchorStatus( int anchorID );
			static void ARDeleteAnchor( int anchorID );

			static int GetAppInstalled( const char *packageName );

			// SnapChat
			static void SetSnapChatStickerSettings( float x, float y, int width, int height, float angle );
			static void ShareSnapChatImage( const char* imageFile, const char* stickerFile, const char* caption, const char* url );

			// noise functions
			static void SetupNoise ( float frequency, float amplitude, float lacunarity, float persistence );
			static float GetNoiseX ( float x );
			static float GetNoiseXY ( float x, float y );
			static float GetNoiseXYZ ( float x, float y, float z );
			static float GetFractalX ( uint32_t octaves, float x );
			static float GetFractalXY ( uint32_t octaves, float x, float y );
			static float GetFractalXYZ ( uint32_t octaves, float x, float y, float z );

			// Extensions
			static int ExternalSDKSupported( const char* sdk );
			static void ExternalCommand( const char* sdk, const char* command, const char* str1, const char* str2 );
			static int ExternalCommandInt( const char* sdk, const char* command, const char* str1, const char* str2 );
			static float ExternalCommandFloat( const char* sdk, const char* command, const char* str1, const char* str2 );
			static char* ExternalCommandString( const char* sdk, const char* command, const char* str1, const char* str2 );
	};
}

#endif
