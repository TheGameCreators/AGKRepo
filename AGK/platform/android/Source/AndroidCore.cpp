#include "agk.h"
#include "OpenGLES2/OpenGL_ES2.h"

#ifndef DISABLE_VULKAN
    #include "Vulkan/AGKVulkan.h"
#endif

#include <pthread.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
//#include <linux/fcntl.h>
//#include <linux/unistd.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/native_activity.h>
#include <android/native_window_jni.h>
#include <android/window.h>
#include <android/configuration.h>

//#include <media/NdkImageReader.h> // only supported in API 26+

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "png.h"
#include "pnginfo.h"
#include "pngstruct.h"

#include "jpeglib.h"
#include "jpegint.h"

#include "zlib.h"

#include "curl/curl.h"
#include "jansson.h"

#include <net/if.h>

#define AGK_USE_AR

#ifdef AGK_USE_AR
	#include "arcore_c_api.h"
	#include <dlfcn.h>
#endif

extern unsigned char libImageAscii[];
extern unsigned char libImageAsciiExt[];

int agk::m_iKeyboardMode = 2; // virtual keyboard

extern "C" {
FILE* AGKfopen( const char *szPath, const char* mode )
{
	FILE* pFile = fopen( szPath, mode );
	return pFile;
}
}

namespace AGK
{
	ANativeActivity* g_pActivity = 0;
	ANativeWindow* g_pNativeWindow = 0;
	//AImageReader* g_pVideoImageReader = 0;

	#define AGK_PROGRAM_FLAG_USES_AR			0x01
	#define AGK_PROGRAM_FLAG_USES_EXTERN_TEX	0x02

	struct initdata 
	{
		struct ANativeActivity* activity;
		ANativeWindow* window;
	};
	    
	jclass GetAGKHelper(JNIEnv* lJNIEnv)
	{
		// get NativeActivity object (clazz)
		jobject lNativeActivity = g_pActivity->clazz;
		
		// get java class for NativeActivity
		jclass classActivity = lJNIEnv->FindClass("android/app/NativeActivity");
		if ( !classActivity ) agk::Warning("Failed to get class NativeActivity");
	
		// we need classLoader to find our OWN Java Class Code
		jmethodID getClassLoader = lJNIEnv->GetMethodID(classActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
		if ( !getClassLoader ) agk::Warning("Failed to get getClassLoader");
		jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
		if ( !cls ) agk::Warning("Failed to get cls");
		jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
		if ( !classLoader ) agk::Warning("Failed to get classLoader");
		jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
		if ( !findClass ) agk::Warning("Failed to get findClass");
	
		// use classLoader to find our java class code
		jstring strClassName = lJNIEnv->NewStringUTF("com/thegamecreators/agk_player/AGKHelper");
		jclass MyJavaClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
		if ( !MyJavaClass ) agk::Warning("Failed to get AGKHelper");
		lJNIEnv->DeleteLocalRef( strClassName );
		return MyJavaClass;
	}

	jclass GetAGKClass( JNIEnv* lJNIEnv, const char* classPath )
	{
		// get NativeActivity object (clazz)
		jobject lNativeActivity = g_pActivity->clazz;
		
		// get java class for NativeActivity
		jclass classActivity = lJNIEnv->FindClass("android/app/NativeActivity");
		if ( !classActivity ) agk::Warning("Failed to get class NativeActivity");
	
		// we need classLoader to find our OWN Java Class Code
		jmethodID getClassLoader = lJNIEnv->GetMethodID(classActivity,"getClassLoader", "()Ljava/lang/ClassLoader;");
		if ( !getClassLoader ) agk::Warning("Failed to get getClassLoader");
		jobject cls = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
		if ( !cls ) agk::Warning("Failed to get cls");
		jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
		if ( !classLoader ) agk::Warning("Failed to get classLoader");
		jmethodID findClass = lJNIEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
		if ( !findClass ) agk::Warning("Failed to get findClass");
	
		// use classLoader to find our java class code
		jstring strClassName = lJNIEnv->NewStringUTF( classPath );
		jclass MyJavaClass = (jclass)lJNIEnv->CallObjectMethod(cls, findClass, strClassName);
		lJNIEnv->DeleteLocalRef( strClassName );
		if ( lJNIEnv->ExceptionCheck() )
		{
			lJNIEnv->ExceptionClear();
			return 0;
		}
		else
		{
			return MyJavaClass;
		}
	}

	int uFixTime = 0;
	float fFixTime2 = 0;

	// camera to image variables
	uint32_t m_iCameraTextureRaw = 0;
	cObject3D *m_pCameraTextureQuad = 0;
	AGKShader *m_pCameraTextureShader = 0;
	AGKFrameBuffer *m_pCameraTextureFBO = 0;
	int iCameraWidth = 0;
	int iCameraHeight = 0;

	// music
	SLObjectItf pMusicPlayer = NULL;
	SLPlayItf pMusicPlayerPlay;
	SLSeekItf pMusicPlayerSeek;
	
	// an audio player that plays nothing to force the android volume control buttons to media mode
	SLObjectItf pTempPlayer = NULL;
	bool g_bTempFinished = false;
	unsigned char *pSilence = 0;
	
	bool g_bMusicFinished = false;
	
	// music finished callback
	void MusicEventCallback( SLPlayItf caller, void *pContext, SLuint32 event )
	{
		if ( event != SL_PLAYEVENT_HEADATEND ) return;
		if ( !pContext ) return;

		g_bMusicFinished = true;
		//((cMusicMgr*)pContext)->HandleEvent();
	}
	
	// sounds
	SLObjectItf engineObject = NULL;
	SLEngineItf engineEngine;
	SLObjectItf outputMixObject = NULL;
	
	class cSoundInst
	{
		public:
			uint32_t m_iID;
			uint32_t m_uLastUsed;
			int m_iParent;
			int m_iVolume;
			int m_iLoop;
			float m_fRate;
			float m_fBalance;
			int m_iLoopCount;
			bool m_bFinished;
			bool m_bCleanedUp;
			float m_fFinishedTimer;
			cSoundInst *m_pPrevInst;
			cSoundInst *m_pNextInst;
		
			SLObjectItf bqPlayerObject;
			SLPlayItf bqPlayerPlay;
			SLPlaybackRateItf rateIf;
			SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
		
			cSoundInst() 
			{ 
				m_iID = 0;
				m_uLastUsed = 0;
				m_iParent = 0;
				m_iVolume = 100;
				m_pPrevInst = 0;
				m_pNextInst = 0;
				m_iLoop = 0;
				m_fRate = 1.0f;
				m_fBalance = 0;
				m_iLoopCount = 0;
				m_bFinished = false;
				m_fFinishedTimer = 0;
				m_bCleanedUp = false;

				bqPlayerObject = 0;
				bqPlayerPlay = 0;
				bqPlayerBufferQueue = 0;
				rateIf = 0;
			}
			
			~cSoundInst()
			{
				if ( bqPlayerObject ) (*bqPlayerObject)->Destroy( bqPlayerObject );
			}
		
			void Reset()
			{
				m_iParent = 0;
				m_iLoop = 0;
				m_fRate = 1.0f;
				m_fBalance = 0;
				m_iLoopCount = 0;

				if ( bqPlayerPlay ) (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
				
				if ( bqPlayerBufferQueue ) (*bqPlayerBufferQueue)->Clear( bqPlayerBufferQueue );
				if ( bqPlayerObject ) (*bqPlayerObject)->Destroy(bqPlayerObject);
				bqPlayerObject = 0;
				bqPlayerPlay = 0;
				bqPlayerBufferQueue = 0;
				rateIf = 0;
				m_bFinished = false;
				m_fFinishedTimer = 0;
				m_bCleanedUp = false;
			}
	};

	cHashedList<cSoundInst*> m_cSoundInstances(256);
	
	void SoundEventCallback( SLPlayItf caller, void *pContext, SLuint32 event )
	{		
		if ( event != SL_PLAYEVENT_HEADATEND ) return;
		if ( !pContext ) return;
		
		((cSoundInst*)pContext)->m_bFinished = true;
		((cSoundInst*)pContext)->m_fFinishedTimer = 1.0f;
		((cSoundInst*)pContext)->m_bCleanedUp = false;
	}
	
	void TempSoundEventCallback( SLPlayItf caller, void *pContext, SLuint32 event )
	{	
		if ( event != SL_PLAYEVENT_HEADATEND ) return;
		g_bTempFinished = true;
	}
	
	char szAppFolderName[ MAX_PATH ] = "";
	char szRootDir[ MAX_PATH ] = "";
	char szWriteDir[ MAX_PATH ] = "";
	char szOriginalWriteDir[ MAX_PATH ] = "";
	
	/*
	NSBundle *g_pBundle = NULL;
	NSFileManager *g_pFileManager = NULL;
	*/
	
	// text input variables
	int g_iTextCursorPos = 0;
	cSprite *g_pTextInputCursor = 0;
	cSprite *g_pTextInputArea = 0;
	cSprite *g_pTextInputBack = 0;
	cText *g_pTextInputText = 0;
	float g_fCursorBlinkTime = 0;
	int g_iShowCursor = 1;
	float g_fTextStartX = 0;
	float g_fTextStartY = 0;
	bool g_bPasswordMode = false;
	float g_fChangeTimer = 0;
	
	cSprite *pTextBackground = 0;
	bool g_bEditBoxHack = false;
	
	// advert variables
	bool g_bAdvertActive = false;
	int g_iAdvertHorz = 0;
	int g_iAdvertVert = 0;
	float g_fAdvertOffsetX = 0;
	float g_fAdvertOffsetY = 0;

	// facebook variables
	uString m_sFBAppID;
	uString m_sAccessToken;
	cHTTPConnection *m_pFacebookConnection = 0;
	int m_iFacebookGettingFriends = 0;
	int m_iFBGettingPicture = 0;
	uString m_sFBLocalFile;
	uString m_sFBUserID;
	uString m_sFBName;
	int m_iFBUserIDStarted = 0;

	struct FacebookUser
	{
		uString userID;
		uString name;
	};

	FacebookUser *m_pFBFriends = 0;
	int m_iFBFriendCount = 0;

	// AR
	int g_iARStatus = 0;
	int g_iARPlaneFindingMode = 1;
	int g_iARLightEstimationMode = 1;
#ifdef AGK_USE_AR
	void *g_pARCoreLibHandle = 0;
	ArSession* g_pARSession = 0;
	ArFrame* g_pARFrame = 0;
	uint32_t g_iARTextureRaw = 0;
	cObject3D *g_pARTextureQuad = 0;
	AGKShader *g_pARTextureShader = 0;
	int g_iARWidth = 0;
	int g_iARHeight = 0;
	
	int g_iARHitResultsSize = 0;
	ArHitResult **g_pARHitResults = 0;
	ArPose **g_pARHitPoses = 0;

	int g_iARPlanesSize = 0;
	ArPlane **g_pARPlanes = 0;
	ArPose **g_pARPlanePoses = 0;
#endif
}

using namespace AGK;

void cFileEntry::TraverseDirectory( const char* dir )
{
	DIR *pDir = opendir( dir );
	if ( pDir )
	{
		dirent* item = readdir( pDir );
		while( item )
		{
			//uString sDir;
			//sDir.Format( "%s - %d", item->d_name, item->d_type );
			//agk::Warning( sDir );

			if ( item->d_type == DT_DIR )
			{
				// directory
				if ( strcmp( item->d_name, "." ) != 0 && strcmp( item->d_name, ".." ) != 0 )
				{
					char str[ 1024 ];
					strcpy( str, dir );
					strcat( str, item->d_name );
					AddNewFile( str );

					strcat( str, "/" );
					TraverseDirectory( str );
				}
			}
			else if ( item->d_type == DT_REG )
			{
				// file
				char str[ 1024 ];
				strcpy( str, dir );
				strcat( str, item->d_name );
				AddNewFile( str );
			}

			item = readdir( pDir );
		}

		closedir( pDir );
	}
}

void cFileEntry::InitFileList()
{
	//TraverseDirectory( szRootDir ); //can't get asset folder directories so root is case sensitive only
	//TraverseDirectory( szWriteDir ); // make broadcast and exported apps the same, everything is case sensitive
}

void agk::SetWindowPosition( int x, int y )
{
	// do nothing on android
}

void agk::SetWindowSize( int width, int height, int fullscreen )
{
	SetWindowSize( width, height, fullscreen, 0 );
}

void agk::SetWindowSize( int width, int height, int fullscreen, int allowOverSized )
{
	// do nothing on android
}

void agk::SetWindowAllowResize( int mode )
{
	// do nothing on android
}

void agk::MaximizeWindow()
{
	
}

void agk::MinimizeApp()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID minimizeApp = lJNIEnv->GetStaticMethodID( AGKHelper, "MinimizeApp","(Landroid/app/Activity;)V" );

	lJNIEnv->CallStaticVoidMethod( AGKHelper, minimizeApp, lNativeActivity );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::RestoreApp()
//****
{
	// do nothing
}

void agk::PinApp( int enable )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "PinApp","(Landroid/app/Activity;I)V" );
	if ( !method ) return;

	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity, enable );

	vm->DetachCurrentThread();
}

int agk::IsPinAppAvailable()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "IsPinAppAvailable","(Landroid/app/Activity;)I" );
	if ( !method ) return 0;

	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, method, lNativeActivity );

	vm->DetachCurrentThread();

	return result;
}

void agk::SetImmersiveMode( int mode )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ImmersiveModeSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID setImmersive = lJNIEnv->GetStaticMethodID( sdkClass, "SetImmersiveMode","(Landroid/app/Activity;I)V" );

	lJNIEnv->CallStaticVoidMethod( sdkClass, setImmersive, lNativeActivity, mode );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::SetScreenResolution( int width, int height )
//****
{
	// removed due to lack of support in Vulkan, use SetRenderToImage instead
}

int agk::IsDarkTheme()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "IsDarkTheme", "(Landroid/app/Activity;)I" );
	if ( !method ) return 0;

	// call our java class method
	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, method, lNativeActivity );
	
	vm->DetachCurrentThread();

	return result;
}

char* agk::GetURLSchemeText()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetLastURIText", "()Ljava/lang/String;" );

	// call our java class method
	jstring text = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, method );

	jboolean bCopy;
	const char* sText = lJNIEnv->GetStringUTFChars( text, &bCopy );

	char *str = new char[ strlen(sText)+1 ];
	strcpy( str, sText );

	lJNIEnv->ReleaseStringUTFChars( text, sText );
	lJNIEnv->DeleteLocalRef( text );

	vm->DetachCurrentThread();

	return str;
}

void agk::ClearURLSchemeText()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "ClearLastURIText", "()V" );

	lJNIEnv->CallStaticVoidMethod( AGKHelper, method );

	vm->DetachCurrentThread();
}

char* agk::GetDeviceBaseName( )
//****
{
	char *str = new char[ 8 ];
	strcpy( str, "android" );
	return str;
}

char* agk::GetDeviceType( )
//****
{
	uString sModel;

	JNIEnv* lJNIEnv = g_pActivity->env;

	jint lFlags = 0;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);
	
	jobject lNativeActivity = g_pActivity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);
	jboolean isCopy;

	// Retrieves Build class.
	jclass ClassContext = lJNIEnv->FindClass( "android/os/Build" );

	jfieldID FieldMODEL = lJNIEnv->GetStaticFieldID( ClassContext, "MODEL", "Ljava/lang/String;" );
	jstring MODEL = (jstring) lJNIEnv->GetStaticObjectField( ClassContext, FieldMODEL );
	const char *szModel = lJNIEnv->GetStringUTFChars( MODEL, &isCopy );
	sModel.SetStr( szModel );
	lJNIEnv->ReleaseStringUTFChars( MODEL, szModel );

	vm->DetachCurrentThread();

	sModel.Lower();
	char *str = new char[ sModel.GetLength()+1 ];
	strcpy( str, sModel.GetStr() );
	return str;
}

int agk::GetDeviceNetworkType()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);
	jmethodID networkAvailable = lJNIEnv->GetStaticMethodID( AGKHelper, "GetNetworkType","(Landroid/app/Activity;)I" );
	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, networkAvailable, lNativeActivity );

	vm->DetachCurrentThread();
	return result;
}

int agk::GetStorageRemaining( const char *path )
{
	uString sPath;
	if ( strncmp( path, "raw:", 4 ) == 0 )
	{
		sPath.SetStrUTF8( path+4 );
	}
	else
	{
		sPath.SetStr( path );
		PlatformGetFullPathWrite( sPath );
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetStorageRemaining","(Landroid/app/Activity;Ljava/lang/String;)I" );

	jstring strPath = lJNIEnv->NewStringUTF( sPath.GetStr() );
	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, method, lNativeActivity, strPath );
	lJNIEnv->DeleteLocalRef( strPath );

	vm->DetachCurrentThread();
	return result;
}

int agk::GetStorageTotal( const char *path )
{
	uString sPath;
	if ( strncmp( path, "raw:", 4 ) == 0 )
	{
		sPath.SetStrUTF8( path+4 );
	}
	else
	{
		sPath.SetStr( path );
		PlatformGetFullPathWrite( sPath );
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetStorageTotal","(Landroid/app/Activity;Ljava/lang/String;)I" );

	jstring strPath = lJNIEnv->NewStringUTF( sPath.GetStr() );
	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, method, lNativeActivity, strPath );
	lJNIEnv->DeleteLocalRef( strPath );

	vm->DetachCurrentThread();
	return result;
}

void agk::GetAppName( uString &outString )
//****
{
	outString.SetStr( m_sAppName );
}

char* agk::GetAppName( )
//****
{
	char *str = new char[ m_sAppName.GetLength()+1 ];
	strcpy( str, m_sAppName.GetStr() );
	return str;
}

char* agk::GetDeviceLanguage( )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID getlanguage = lJNIEnv->GetStaticMethodID( AGKHelper, "GetLanguage", "()Ljava/lang/String;" );

	// call our java class method
	jstring token = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, getlanguage );

	jboolean bCopy;
	const char* sToken = lJNIEnv->GetStringUTFChars( token, &bCopy );

	char *str = new char[ strlen(sToken)+1 ];
	strcpy( str, sToken );

	lJNIEnv->ReleaseStringUTFChars( token, sToken );
	lJNIEnv->DeleteLocalRef( token );

	vm->DetachCurrentThread();

	return str;
}

void agk::SetSleepMode( int mode )
//****
{
	if ( !g_pActivity ) return;

	if ( mode == 0 )
	{
		ANativeActivity_setWindowFlags( g_pActivity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0 );
	}
	else
	{
		ANativeActivity_setWindowFlags( g_pActivity, 0, AWINDOW_FLAG_KEEP_SCREEN_ON );
	}
}

void agk::SetExpansionFileKey( const char* key )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID setExpansionKey = lJNIEnv->GetStaticMethodID( AGKHelper, "setExpansionKey","(Ljava/lang/String;)V" );

	jstring strKey = lJNIEnv->NewStringUTF( key ? key : "" );
	lJNIEnv->CallStaticVoidMethod( AGKHelper, setExpansionKey, strKey );
	lJNIEnv->DeleteLocalRef( strKey );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::SetExpansionFileVersion(int version)
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID expansionstate = lJNIEnv->GetStaticMethodID( AGKHelper, "SetExpansionVersion", "(I)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( AGKHelper, expansionstate, version );

	vm->DetachCurrentThread();

	return;
}

int agk::GetExpansionFileState()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID expansionstate = lJNIEnv->GetStaticMethodID( AGKHelper, "GetExpansionState", "(Landroid/app/Activity;)I" );

	// call our java class method
	int state = lJNIEnv->CallStaticIntMethod( AGKHelper, expansionstate, lNativeActivity );

	vm->DetachCurrentThread();

	return state;
}

int agk::GetExpansionFileError()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetExpansionError", "(Landroid/app/Activity;)I" );

	// call our java class method
	int error = lJNIEnv->CallStaticIntMethod( AGKHelper, method, lNativeActivity );

	vm->DetachCurrentThread();

	return error;
}

void agk::DownloadExpansionFile()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID expansionstate = lJNIEnv->GetStaticMethodID( AGKHelper, "DownloadExpansion", "(Landroid/app/Activity;)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( AGKHelper, expansionstate, lNativeActivity );

	vm->DetachCurrentThread();
}

float agk::GetExpansionFileProgress()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID expansionstate = lJNIEnv->GetStaticMethodID( AGKHelper, "GetExpansionProgress", "(Landroid/app/Activity;)F" );

	// call our java class method
	float progress = lJNIEnv->CallStaticFloatMethod( AGKHelper, expansionstate, lNativeActivity );

	vm->DetachCurrentThread();

	return progress;
}

bool agk::ExtractExpansionFile( const char* localFile, const char* expansionFile )
{
	// extract file from expansion file then load normally
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
	jclass AGKHelper = GetAGKHelper(lJNIEnv);
	jmethodID Extract = lJNIEnv->GetStaticMethodID( AGKHelper, "ExtractExpansionFileImage", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)I" );

	jstring text = lJNIEnv->NewStringUTF( expansionFile );
	jstring path = lJNIEnv->NewStringUTF( localFile );
	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, Extract, lNativeActivity, text, path );
	lJNIEnv->DeleteLocalRef( path );
	lJNIEnv->DeleteLocalRef( text );

	vm->DetachCurrentThread();

	if ( result == 0 )
	{
		uString err; err.Format( "Failed to extract file from expansion file: %s", expansionFile );
		agk::Error( err );
		return false;
	}

	return true;
}

void agk::SetWindowTitle( const char *szTitle )
{

}

bool agk::GetDeviceCanRotate()
{
	return true;
}

void agk::PlatformSetOrientationAllowed( int portrait, int portrait2, int landscape, int landscape2 )
{
	int orien = 10; // fullSensor
	if ( portrait && portrait2 && landscape && landscape2 ) orien = 10; // fullSensor
	else
	{
		if ( portrait && portrait2 ) orien = 7; // sensorPortrait
		else
		{
			if ( landscape && landscape2 ) orien = 6; // sensorLandscape
			else
			{
				if ( portrait ) orien = 1; // portrait
				else if ( portrait2 ) orien = 9; // reversePortrait
				else if ( landscape ) orien = 0; // landscape
				else if ( landscape2 ) orien = 8; // reverseLandscape
				else orien = 10; // fullSensor
			}
		}
	}
	
	jint lFlags = 0;
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	if ( !g_pActivity ) agk::Warning("Failed to get activity pointer");
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	jclass classActivity = lJNIEnv->FindClass("android/app/NativeActivity");
	if ( !classActivity ) agk::Warning("Failed to get class NativeActivity");

	jmethodID getClassLoader = lJNIEnv->GetMethodID(classActivity, "getClassLoader", "()Ljava/lang/ClassLoader;");
	if( !getClassLoader ) agk::Warning("Exception occurred while getting getClassLoader methodId");

	jobject objClassLoader = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	if ( !objClassLoader ) agk::Warning("Exception occurred while getting class loader instance");

	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	if ( !classLoader ) agk::Warning("Exception occurred while finding ClassLoader class definition");

	jmethodID methodFindClass = lJNIEnv->GetMethodID( classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if( !methodFindClass ) agk::Warning("Exception occurred while getting loadClass method id");

	// Get AGK Helper class
	jstring classname = lJNIEnv->NewStringUTF("com.thegamecreators.agk_player.AGKHelper");
	jclass classHelper = (jclass)lJNIEnv->CallObjectMethod(objClassLoader, methodFindClass, classname );
	if( !classHelper ) agk::Warning("Failed to get AGKHelper class");
	lJNIEnv->DeleteLocalRef( classname );

	jmethodID methodShowMessage = lJNIEnv->GetStaticMethodID( classHelper, "SetOrientation","(Landroid/app/Activity;I)V");
	if ( !methodShowMessage ) agk::Warning("Failed to get method SetOrientation");

	lJNIEnv->CallStaticVoidMethod( classHelper, methodShowMessage, lNativeActivity, orien );
	

	vm->DetachCurrentThread();
}

bool agk::PlatformGetDeviceID( uString &out )
{
	jint lFlags = 0;
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);
	
	if ( !g_pActivity ) agk::Warning("Failed to get activity pointer");
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	jclass classActivity = lJNIEnv->FindClass("android/app/NativeActivity");
	if ( !classActivity ) agk::Warning("Failed to get class NativeActivity");

	jmethodID getClassLoader = lJNIEnv->GetMethodID(classActivity, "getClassLoader", "()Ljava/lang/ClassLoader;");
	if( !getClassLoader ) agk::Warning("Exception occurred while getting getClassLoader methodId");

	jobject objClassLoader = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	if ( !objClassLoader ) agk::Warning("Exception occurred while getting class loader instance");

	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	if ( !classLoader ) agk::Warning("Exception occurred while finding ClassLoader class definition");

	jmethodID methodFindClass = lJNIEnv->GetMethodID( classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if( !methodFindClass ) agk::Warning("Exception occurred while getting loadClass method id");

	// Get AGK Helper class
	jstring classname = lJNIEnv->NewStringUTF("com.thegamecreators.agk_player.AGKHelper");
	jclass classHelper = (jclass)lJNIEnv->CallObjectMethod(objClassLoader, methodFindClass, classname );
	if( !classHelper ) agk::Warning("Failed to get AGKHelper class");
	lJNIEnv->DeleteLocalRef( classname );

	jmethodID getDeivceID = lJNIEnv->GetStaticMethodID( classHelper, "GetDeviceID","(Landroid/app/Activity;)Ljava/lang/String;");
	if ( !getDeivceID ) agk::Warning("Failed to get method GetDeviceID");

	jstring devicestring = (jstring) lJNIEnv->CallStaticObjectMethod( classHelper, getDeivceID, lNativeActivity );
	
	jboolean bCopy;
	const char* sDeviceID = lJNIEnv->GetStringUTFChars( devicestring, &bCopy );
	
	out.SetStr( "" );
	if ( !sDeviceID || strlen(sDeviceID) < 1 ) return false;	
	
	unsigned int result[ 5 ];
	SHA1 sha;
	sha.Input( sDeviceID, strlen(sDeviceID) );
	sha.Result( &(result[0]) );
	out.Format( "%08X%08X%08X%08X%08X", result[0], result[1], result[2], result[3], result[4] );
	
	lJNIEnv->ReleaseStringUTFChars( devicestring, sDeviceID );
	lJNIEnv->DeleteLocalRef( devicestring );
	
	vm->DetachCurrentThread();

	return true; 
}

float agk::PlatformDeviceScale()
{
	return 1.0f;
}

int agk::PlatformGetNumProcessors()
{
	//return 1;
	FILE* fp;
    int res, i = -1, j = -1;

    /* open file */
    fp = fopen("/sys/devices/system/cpu/present", "r");
    if (fp == 0)
    {
        return 1; /* failure */
    }

    /* read and interpret line */
    res = fscanf(fp, "%d-%d", &i, &j);

    /* close file */
    fclose(fp);

    /* interpret result */
    if (res == 1 && i == 0) /* single-core? */
    {
        return 1;
    }
    if (res == 2 && i == 0) /* 2+ cores */
    {
        return j+1;
    }

    return 1; /* failure */
}

struct AGKData
{
	int width;
	int height;
};

void agk::PlatformInitFilePaths()
{
	strcpy( szRootDir, "" );

	// get app name
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID getappname = lJNIEnv->GetStaticMethodID( AGKHelper, "GetAppName", "(Landroid/app/Activity;)Ljava/lang/String;" );

	// call our java class method
	jstring name = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, getappname, lNativeActivity );

	jboolean bCopy;
	const char* sName = lJNIEnv->GetStringUTFChars( name, &bCopy );

	strcpy( szAppFolderName, sName );
	m_sAppName.SetStr( sName );

	lJNIEnv->ReleaseStringUTFChars( name, sName );
	lJNIEnv->DeleteLocalRef( name );

	vm->DetachCurrentThread();
	
	// get app name and size for write folder
	//strcpy( szAppFolderName, "Unknown" );
	//m_sAppName.SetStr( "Unknown" );
	
	const char *internal = g_pActivity->internalDataPath;
	if ( internal )
	{
		strcpy( szWriteDir, internal );
		strcat( szWriteDir, "/" );
	}
	else
	{
		// if no internal directory found, use sd card
		chdir( "/sdcard/" );
		if ( chdir( "AGK" ) < 0 )
		{
			mkdir( "AGK", 0777 );
			chdir( "AGK" );
		}
		
		if ( chdir( szAppFolderName ) < 0 )
		{
			mkdir( szAppFolderName, 0777 );
			chdir( szAppFolderName );
		}
	
		strcpy( szWriteDir, "/sdcard/AGK/" );
		strcat( szWriteDir, szAppFolderName );
		strcat( szWriteDir, "/" );
	}

	// make sure this value is set here in case restore is called without changing path
	strcpy( szOriginalWriteDir, szWriteDir );

	chdir( szWriteDir );
	
	//ParseCurrentDirectory();
	m_bUpdateFileLists = true;
}

void agk::PlatformUpdateWritePath()
{
	if ( m_sAppName.GetLength() == 0 ) 
	{
		PlatformRestoreWritePath();
		return;
	}
	
	const char *internal = g_pActivity->internalDataPath;
	if ( internal )
	{
		strcpy( szWriteDir, internal );
		strcat( szWriteDir, "/" );
		if ( m_sAppName.GetLength() > 0 )
		{
			uString sModule( m_sAppName ); // make documents path simpler
			sModule.Replace( '\\', '_' );
			sModule.Replace( '/', '_' );
			sModule.Strip( "\\/.: \t*" );
			strcat( szWriteDir, sModule );
			strcat( szWriteDir, "/" );

			int fd = open( internal, O_RDONLY | O_CLOEXEC );
			int newFd = openat( fd, sModule, O_RDONLY | O_CLOEXEC );
			if ( newFd >= 0 )
			{
				close( newFd );
			}
			else
			{
				mkdirat( fd, sModule, 0777 );
			}
			close( fd );
		}
	}
	else
	{
		strcpy( szWriteDir, "/sdcard/AGK/" );
		if ( m_sAppName.GetLength() > 0 )
		{
			uString sModule( m_sAppName ); // make documents path simpler
			sModule.Replace( '\\', '_' );
			sModule.Replace( '/', '_' );
			sModule.Strip( "\\/.: \t*" );
			strcat( szWriteDir, sModule );
			strcat( szWriteDir, "/" );

			int fd = open( szWriteDir, O_RDONLY | O_CLOEXEC );
			int newFd = openat( fd, sModule, O_RDONLY | O_CLOEXEC );
			if ( newFd >= 0 )
			{
				close( newFd );
			}
			else
			{
				mkdirat( fd, sModule, 0777 );
			}
			close( fd );
		}
	}
	
	//ParseCurrentDirectory();
	m_bUpdateFileLists = true;
}

void agk::PlatformRestoreWritePath()
{
	if ( strlen ( szOriginalWriteDir ) > 0 ) strcpy( szWriteDir, szOriginalWriteDir );
	
	//ParseCurrentDirectory();
	m_bUpdateFileLists = true;
}

void agk::OverrideDirectories( const char *szWrite, int useRead )
{
	if ( szWrite && *szWrite )
	{
		strcpy( szWriteDir, szWrite );
		cFileEntry::ClearAll();
		m_bUpdateFileLists = true;
	}
	else
	{
		char *szTemp = GetAppName();
		SetAppName( szTemp );
		delete [] szTemp;
	}
}

void agk::InitJoysticks()
{
	
}

void agk::PlatformSetDevicePtr( void* ptr )
{
	initdata* data = (initdata*) ptr;
	g_pActivity = data->activity;
	g_pNativeWindow = data->window;
}

void agk::PlatformInitGraphicsCommon()
{
	// only seems to affect Android
	const char* szGL = GetRendererName();
	if ( strncmp(szGL, "PowerVR SGX 5", strlen("PowerVR SGX 5")) == 0 ) // catch all SGX 5xx GPUs
	{
		agk::Warning( "Detected PowerVR SGX 5xx, using driver workaround" );
		m_iSGX540GPU = 1;
	}

	// text input setup
	float DevToVirX = 1.0f;
	float DevToVirY = 1.0f;
	if ( agk::m_fTargetViewportWidth > 0 ) DevToVirX = (agk::GetDeviceDPI() / 150.0f) * agk::GetVirtualWidth() / agk::m_fTargetViewportWidth;
	if ( agk::m_fTargetViewportHeight > 0 )  DevToVirY = (agk::GetDeviceDPI() / 150.0f) * agk::GetVirtualHeight() / agk::m_fTargetViewportHeight;
	
	float width = 250 * DevToVirX;
	float height = 22 * DevToVirY;
	
	if ( width > agk::GetVirtualWidth() ) width = (float) agk::GetVirtualWidth();

	g_fTextStartX = (agk::GetVirtualWidth() - width)/2.0f + 3*DevToVirX;
	g_fTextStartY = agk::GetVirtualHeight()/3.0f + 2*DevToVirY;
	
	g_pTextInputCursor = new cSprite();
	g_pTextInputCursor->SetSize( 2 * DevToVirX, 18 * DevToVirY );
	g_pTextInputCursor->SetColor( 102, 213, 255, 255 );
	g_pTextInputCursor->SetPosition( g_fTextStartX, g_fTextStartY );
	//g_pTextInputCursor->SetOffset( 0,0 );
	g_pTextInputCursor->FixToScreen(1);
	
	g_pTextInputArea = new cSprite();
	g_pTextInputArea->SetSize( width, height );
	g_pTextInputArea->SetColor( 255,255,255,255 );
	g_pTextInputArea->SetPosition( (agk::GetVirtualWidth() - width) / 2.0f, agk::GetVirtualHeight()/3.0f );
	//g_pTextInputArea->SetOffset( 0,0 );
	g_pTextInputArea->FixToScreen(1);
	
	width += 8 * DevToVirX;
	height += 8 * DevToVirY;
	
	g_pTextInputBack = new cSprite();
	g_pTextInputBack->SetSize( width, height );
	g_pTextInputBack->SetColor( 190,190,190,255 );
	g_pTextInputBack->SetPosition( (agk::GetVirtualWidth() - width) / 2.0f, agk::GetVirtualHeight()/3.0f - 4*DevToVirY );
	//g_pTextInputBack->SetOffset( 0,0 );
	g_pTextInputBack->FixToScreen(1);
	
	g_pTextInputText = new cText(30);
	g_pTextInputText->SetPosition( g_fTextStartX, g_fTextStartY );
	g_pTextInputText->SetColor( 0,0,0 );
	g_pTextInputText->SetSpacing( 0 );
	g_pTextInputText->FixToScreen(1);
	
	//cImage *pArial = new cImage( "/Arial.png" );
	//g_pTextInputText->SetFontImage( pArial );
	g_pTextInputText->SetFont( 0 );
	
	pTextBackground = new cSprite();
	pTextBackground->SetColor( 0,0,0, 128 );
	pTextBackground->SetPosition( -m_iDisplayExtraX, -m_iDisplayExtraY );
	pTextBackground->SetSize( m_iDisplayWidth+m_iDisplayExtraX*2, m_iDisplayHeight+m_iDisplayExtraY*2 );
	pTextBackground->FixToScreen(1);
}

void agk::PlatformInitNonGraphicsCommon()
{
	// stop SIGPIPE causing a crash when a network socket disconnects
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGPIPE, &sa, 0) == -1) {
		perror(0);
		agk::Warning( "Failed to disable SIGPIPE" );
	}

	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	uFixTime = now.tv_sec;
	fFixTime2 = now.tv_nsec * 1e-9;
	SetRandomSeed( uFixTime + (now.tv_nsec % 1000) );

	// curl setup
	curl_global_init( CURL_GLOBAL_SSL );

	// sensors
	ASensorManager* sensorManager = ASensorManager_getInstance();
	if ( sensorManager )
	{
		const ASensor* accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
		const ASensor* gyroSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
		const ASensor* proximitySensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_PROXIMITY);
		const ASensor* lightSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_LIGHT);
		const ASensor* magneticSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
		const ASensor* rotVectorSensor = ASensorManager_getDefaultSensor(sensorManager, 11);

		m_iAccelerometerExists = accelerometerSensor ? 1 : 0;
		m_iGyroSensorExists = gyroSensor ? 1 : 0;
		m_iProximitySensorExists = proximitySensor ? 1 : 0;
		m_iLightSensorExists = lightSensor ? 1 : 0;
		m_iMagneticSensorExists = magneticSensor ? 1 : 0;
		m_iRotationSensorExists = rotVectorSensor ? 1 : 0;
	}
}

void agk::PlatformInitExternal( void* ptr, int width, int height, AGKRenderer renderer )
{
	agk::Error( "External mode is not supported on Android" );
}

int agk::PlatformNeedsNewRenderer( AGKRendererMode rendererMode, uint32_t flags )
{
	if ( !g_pRenderer ) return 1;

	// Vulkan does not yet support AR
	if ( flags & AGK_PROGRAM_FLAG_USES_AR ) rendererMode = AGK_RENDERER_MODE_ONLY_LOWEST;
	else if ( flags & AGK_PROGRAM_FLAG_USES_EXTERN_TEX )
	{
		// maybe check existing extensions
		return 1;
	}

	switch( rendererMode )
	{
		case AGK_RENDERER_MODE_ONLY_LOWEST: 
		{
			if ( g_pRenderer->GetIdentifier() != AGK_RENDERER_OPENGL_ES2 ) return 1;
			else return 0;
			break;
		}
#ifndef DISABLE_VULKAN
		case AGK_RENDERER_MODE_ONLY_ADVANCED: 
		{
			if ( g_pRenderer->GetIdentifier() != AGK_RENDERER_VULKAN ) return 1;
			else return 0;
			break;
		}
#endif
		case AGK_RENDERER_MODE_PREFER_BEST:
		{
#ifndef DISABLE_VULKAN
			if ( g_pRenderer->GetIdentifier() != AGK_RENDERER_VULKAN ) return 1;
			else return 0;
#else 
			return 0;
#endif
			break;
		}
		default:
        {
            agk::Error("Unknown render mode parameter");
            return 1;
        }
	}

	return 1;
}

void agk::PlatformInitGraphics( void* ptr, AGKRendererMode rendererMode, uint32_t flags )
{
	if ( ptr )
	{
		initdata* data = (initdata*) ptr;
		g_pActivity = data->activity;
		g_pNativeWindow = data->window;
	}

	// Vulkan does not yet support AR
	if ( flags & AGK_PROGRAM_FLAG_USES_AR ) rendererMode = AGK_RENDERER_MODE_ONLY_LOWEST;
	else if ( flags & AGK_PROGRAM_FLAG_USES_EXTERN_TEX )
	{
		rendererMode = AGK_RENDERER_MODE_ONLY_LOWEST;
		// this requires NDK API 26+, we need NDK API 16 for backwards compatibility
		/*
		// extensions required for video to texture or camera to texture
		AddVulkanDeviceExtensions( "VK_ANDROID_external_memory_android_hardware_buffer" );
		AddVulkanDeviceExtensions( "VK_KHR_sampler_ycbcr_conversion" );
		AddVulkanDeviceExtensions( "VK_KHR_external_memory" );
		AddVulkanDeviceExtensions( "VK_EXT_queue_family_foreign" );
		*/
	}

	// prefer OpenGLES over Vulkan, make sure using #renderer "Advanced" on Android will still drop down to OpenGLES
	if ( rendererMode == AGK_RENDERER_MODE_PREFER_BEST ) rendererMode = AGK_RENDERER_MODE_ONLY_LOWEST;
	if ( rendererMode == AGK_RENDERER_MODE_ONLY_ADVANCED ) rendererMode = AGK_RENDERER_MODE_PREFER_BEST;
	
	g_pRenderer = 0;
	switch( rendererMode )
	{
		case AGK_RENDERER_MODE_ONLY_LOWEST: 
		{
			g_pRenderer = new OpenGLES2Renderer(); 
			if ( g_pRenderer->Init() != APP_SUCCESS ) 
			{
				delete g_pRenderer;
				g_pRenderer = 0;
				agk::Error( "Failed to initialise OpenGL ES2" );
				return;
			}
			break;
		}
#ifndef DISABLE_VULKAN
		case AGK_RENDERER_MODE_ONLY_ADVANCED: 
		{
			VulkanRenderer *pVulkanRenderer = new VulkanRenderer();
			g_pRenderer = pVulkanRenderer;
			pVulkanRenderer->AddDeviceExtensions( &m_pVulkanDeviceExtensions );
			pVulkanRenderer->AddInstanceExtensions( &m_pVulkanInstanceExtensions );
			if ( g_pRenderer->Init() != APP_SUCCESS ) 
			{
				delete g_pRenderer;
				g_pRenderer = 0;
				agk::Error( "Failed to initialise Vulkan" );
				return;
			}
			break;
		}
#endif
		case AGK_RENDERER_MODE_PREFER_BEST:
		{
#ifndef DISABLE_VULKAN
			int oldMode = m_iErrorMode;
			SetErrorMode(1);
			VulkanRenderer *pVulkanRenderer = new VulkanRenderer();
			g_pRenderer = pVulkanRenderer;
			pVulkanRenderer->AddDeviceExtensions( &m_pVulkanDeviceExtensions );
			pVulkanRenderer->AddInstanceExtensions( &m_pVulkanInstanceExtensions );
			if ( g_pRenderer->Init() == APP_SUCCESS ) 
			{
				AppInfo( "Using Vulkan" );
				SetErrorMode(oldMode);
			}
			else
			{
				delete g_pRenderer;
#endif
				SetErrorMode(oldMode);
				g_pRenderer = new OpenGLES2Renderer();
				if ( g_pRenderer->Init() == APP_SUCCESS ) 
				{
					AppInfo( "Using OpenGL ES2" );
				}
				else
				{
					delete g_pRenderer;
					g_pRenderer = 0;
					AppError( "No supported renderer found" );
					return;
				}
#ifndef DISABLE_VULKAN
			}
#endif
			break;
		}
		default:
        {
            agk::Error("Unknown render mode parameter");
            return;
        }
	}

	m_iRealDeviceWidth = ANativeWindow_getWidth( g_pNativeWindow );
	m_iRealDeviceHeight = ANativeWindow_getHeight( g_pNativeWindow );

	static int informed = 0;
	while( m_iRealDeviceWidth <= 1 || m_iRealDeviceHeight <= 1 )
	{
		if ( !informed )
		{
			agk::Warning( "Width or height invalid, waiting for updated size" );
			informed = 1;
		}

		agk::Sleep( 10 );
		m_iRealDeviceWidth = ANativeWindow_getWidth( g_pNativeWindow );
		m_iRealDeviceHeight = ANativeWindow_getHeight( g_pNativeWindow );
	}

	if ( g_pRenderer->SetupWindow( g_pNativeWindow, 0, m_iRealDeviceWidth, m_iRealDeviceHeight ) != APP_SUCCESS )
	{
		agk::Error( "Failed to setup renderer window" );
		delete g_pRenderer;
		g_pRenderer = 0;
		return;
	}

	m_iRenderWidth = g_pRenderer->GetSurfaceWidth();
	m_iRenderHeight = g_pRenderer->GetSurfaceHeight();

	cCamera::UpdateAllAspectRatio( m_iRenderWidth/(float)m_iRenderHeight );

	if ( g_pRenderer->Setup() != APP_SUCCESS )
	{
		agk::Error( "Failed to setup renderer" );
		delete g_pRenderer;
		g_pRenderer = 0;
		return;
	}

	/*
	// print out extensions for debug purposes
	const char* extensions = (const char*) glGetString( GL_EXTENSIONS );
	agk::Warning( extensions );
	if ( strlen(extensions) > 1024 ) agk::Warning( extensions+1020 );
	agk::Warning( "" );
	*/
}

void agk::PlatformInitConsole()
{
	
}

void agk::PlatformDeleteRenderer()
{
	if ( g_pTextInputCursor ) delete g_pTextInputCursor;
	g_pTextInputCursor = 0;
	if ( g_pTextInputArea ) delete g_pTextInputArea;
	g_pTextInputArea = 0;
	if ( g_pTextInputBack ) delete g_pTextInputBack;
	g_pTextInputBack = 0;
	if ( g_pTextInputText ) delete g_pTextInputText;
	g_pTextInputText = 0;
	if ( pTextBackground ) delete pTextBackground;
	pTextBackground = 0;

	if ( g_pRenderer ) 
	{
		g_pRenderer->CleanUp();
		delete g_pRenderer;
	}
}

void agk::UpdatePtr( void *ptr )
{
	initdata *data = (initdata*) ptr;
	g_pActivity = data->activity;
	g_pNativeWindow = data->window;
}

void agk::UpdatePtr2( void *ptr )
{
	
}

int agk::GetInternalDataI( int index )
{
	switch( index )
	{	
		case 1: // does surface need updating
		case 2: // new surface width
		case 3: // new surface height
		{
			// no longer supported
			return 0; 
		}
		case 4: // activate accelerometer
		{
			return (m_bSensorFlags & AGK_SENSOR_ACTIVATE_ACCEL) ? 1 : 0; 
		}
		case 5: // activate gyro
		{
			return (m_bSensorFlags & AGK_SENSOR_ACTIVATE_GYRO) ? 1 : 0; 
		}
		case 6: // activate prox
		{
			return (m_bSensorFlags & AGK_SENSOR_ACTIVATE_PROX) ? 1 : 0; 
		}
		case 7: // activate light sensor
		{
			return (m_bSensorFlags & AGK_SENSOR_ACTIVATE_LIGHT) ? 1 : 0; 
		}
		case 8: // activate magnometer
		{
			return (m_bSensorFlags & AGK_SENSOR_ACTIVATE_MAG) ? 1 : 0; 
		}
		case 9: // activate rotation vector
		{
			return (m_bSensorFlags & AGK_SENSOR_ACTIVATE_ROT) ? 1 : 0; 
		}
		default: return 0;
	}
}

void agk::WindowMoved()
{
	
}

void agk::SetVSync( int mode )
{
	if ( !g_pRenderer ) return;
	if ( mode < 0 ) mode = 0;

	g_pRenderer->SetVSync( mode );

	if ( mode > 0 ) m_iSyncMode = 2;
	else m_iSyncMode = 0;
}

void agk::Sleep( uint32_t milliseconds )
//****
{
	// convert from milliseconds to microseconds
	usleep( milliseconds*1000 );
}

void agk::PlatformCleanUp()
{
	agk::StopSound(0); // stop all
	AGKMusicOGG::DeleteAll();
	cSoundMgr::Destroy();

    curl_global_cleanup();
}

int agk::GetMaxDeviceWidth()
//****
{
	return m_iRealDeviceWidth;
}

int agk::GetMaxDeviceHeight()
//****
{
	return m_iRealDeviceHeight;
}

int agk::GetDeviceDPI()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID GetDPI = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDeviceDPI","(Landroid/app/Activity;)I" );
	int dpi = lJNIEnv->CallStaticIntMethod( AGKHelper, GetDPI, lNativeActivity );

	vm->DetachCurrentThread();

	return dpi;
}

int agk::GetDisplayNumCutouts()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	int count = 0;
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplayNumCutouts","(Landroid/app/Activity;)I" );
	if ( method ) count = lJNIEnv->CallStaticIntMethod( AGKHelper, method, lNativeActivity );
	
	vm->DetachCurrentThread();

	return count;
}

float agk::GetDisplayCutoutTop( int index )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	float value = 0;
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplayCutoutBound","(Landroid/app/Activity;II)F" );
	if ( method ) value = lJNIEnv->CallStaticFloatMethod( AGKHelper, method, lNativeActivity, index, 1 );

	vm->DetachCurrentThread();

	return agk::DeviceToScreenY( value );
}

float agk::GetDisplayCutoutBottom( int index )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	float value = 0;
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplayCutoutBound","(Landroid/app/Activity;II)F" );
	if ( method ) value = lJNIEnv->CallStaticFloatMethod( AGKHelper, method, lNativeActivity, index, 2 );

	vm->DetachCurrentThread();

	return agk::DeviceToScreenY( value );
}

float agk::GetDisplayCutoutLeft( int index )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	float value = 0;
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplayCutoutBound","(Landroid/app/Activity;II)F" );
	if ( method ) value = lJNIEnv->CallStaticFloatMethod( AGKHelper, method, lNativeActivity, index, 3 );

	vm->DetachCurrentThread();

	return agk::DeviceToScreenX( value );
}

float agk::GetDisplayCutoutRight( int index )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	float value = 0;
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplayCutoutBound","(Landroid/app/Activity;II)F" );
	if ( method ) value = lJNIEnv->CallStaticFloatMethod( AGKHelper, method, lNativeActivity, index, 4 );

	vm->DetachCurrentThread();

	return agk::DeviceToScreenX( value );
}

float agk::GetScreenBoundsSafeTop()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	float result = GetScreenBoundsTop();
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplaySafeBound","(Landroid/app/Activity;I)F" );
	if ( method ) 
	{
		float value = lJNIEnv->CallStaticFloatMethod( AGKHelper, method, lNativeActivity, 1 );
		if ( value > -9999 ) result = agk::DeviceToScreenY( value );
	}
	
	vm->DetachCurrentThread();

	return result;
}

float agk::GetScreenBoundsSafeBottom()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	float result = GetScreenBoundsBottom();
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplaySafeBound","(Landroid/app/Activity;I)F" );
	if ( method ) 
	{
		float value = lJNIEnv->CallStaticFloatMethod( AGKHelper, method, lNativeActivity, 2 );
		if ( value > -9999 ) result = agk::DeviceToScreenY( m_iRealDeviceHeight - value );
	}
	
	vm->DetachCurrentThread();

	return result;
}

float agk::GetScreenBoundsSafeLeft()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	float result = GetScreenBoundsLeft();
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplaySafeBound","(Landroid/app/Activity;I)F" );
	if ( method ) 
	{
		float value = lJNIEnv->CallStaticFloatMethod( AGKHelper, method, lNativeActivity, 3 );
		if ( value > -9999 ) result = agk::DeviceToScreenX( value );
	}
	
	vm->DetachCurrentThread();

	return result;
}

float agk::GetScreenBoundsSafeRight()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	float result = GetScreenBoundsRight();
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplaySafeBound","(Landroid/app/Activity;I)F" );
	if ( method ) 
	{
		float value = lJNIEnv->CallStaticFloatMethod( AGKHelper, method, lNativeActivity, 4 );
		if ( value > -9999 ) result = agk::DeviceToScreenX( m_iRealDeviceWidth - value );
	}
	
	vm->DetachCurrentThread();

	return result;
}

char* agk::GetAppPackageName()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID GetPackage = lJNIEnv->GetStaticMethodID( AGKHelper, "GetPackageName", "(Landroid/app/Activity;)Ljava/lang/String;" );
	
	jstring text = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, GetPackage, lNativeActivity );
	
	jboolean bCopy;
	const char* sText = lJNIEnv->GetStringUTFChars( text, &bCopy );

	char *str = new char[strlen(sText)+1];
	strcpy( str, sText );
	
	lJNIEnv->ReleaseStringUTFChars( text, sText );
	lJNIEnv->DeleteLocalRef( text );

	vm->DetachCurrentThread();

	return str;
}

int agk::GetDevicePlatform()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID GetPlatform = lJNIEnv->GetStaticMethodID( AGKHelper, "GetPlatform", "()I" );
	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, GetPlatform );
	
	vm->DetachCurrentThread();

	return result;
}

void agk::PlatformUpdateDeviceSize()
{
	if ( g_pNativeWindow == 0 ) return;

	/*
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID GetDisplayWidth = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplayWidth","(Landroid/app/Activity;)I" );
	m_iRealDeviceWidth = lJNIEnv->CallStaticIntMethod( AGKHelper, GetDisplayWidth, lNativeActivity );

	jmethodID GetDisplayHeight = lJNIEnv->GetStaticMethodID( AGKHelper, "GetDisplayHeight","(Landroid/app/Activity;)I" );
	m_iRealDeviceHeight = lJNIEnv->CallStaticIntMethod( AGKHelper, GetDisplayHeight, lNativeActivity );
	vm->DetachCurrentThread();
	*/

	// C++ method of getting window size, may give wrong values on some Huawei devices?
	m_iRealDeviceWidth = ANativeWindow_getWidth( g_pNativeWindow );
	m_iRealDeviceHeight = ANativeWindow_getHeight( g_pNativeWindow );

	g_pRenderer->WindowResized( g_pNativeWindow, 0, m_iRealDeviceWidth, m_iRealDeviceHeight );

	m_iRenderWidth = g_pRenderer->GetSurfaceWidth();
	m_iRenderHeight = g_pRenderer->GetSurfaceHeight();

	// On Android changing orientation seems to turn on VSync so turn it off if we don't want it
	if ( m_iSyncMode != 2 ) SetVSync( 0 );

	//uString str;
	//str.Format( "Width: %d, Height: %d", m_iRenderWidth, m_iRenderHeight );
	//agk::Warning( str );

	// banner ads need repositioning
	if ( g_bAdvertActive )
	{
		PlatformAdMobPosition( g_iAdvertHorz, g_iAdvertVert, g_fAdvertOffsetX, g_fAdvertOffsetY );
	}

	// video
	agk::SetVideoDimensions( m_fVideoX, m_fVideoY, m_fVideoWidth, m_fVideoHeight );

	agk::ClearScreen();
}

void agk::PlatformUpdateTime ( void )
{
	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	int time = now.tv_sec - uFixTime;
	m_fTimeCurr = time + (now.tv_nsec * 1e-9) - fFixTime2;
	m_iTimeMilliseconds = time*1000 + (now.tv_nsec / 1000000) - agk::Round(fFixTime2*1000);
}

void agk::PlatformResetTime ( void )
{
	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	uFixTime = now.tv_sec;
	fFixTime2 = now.tv_nsec * 1e-9;
	
	m_fTimeCurr = 0;
	m_iTimeMilliseconds = 0;
}

double agk::PlatformGetRawTime ( void )
{
	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	double dtime = now.tv_sec;
	dtime = dtime + (now.tv_nsec * 1e-9);
	return dtime;
}

void agk::CompositionChanged()
{

}

void agk::PlatformSync()
{
	
}

void agk::PlatformCompleteInputInit()
{
	
}

void showKeyboard( bool show, int multiline, int inputType )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	if ( show )
	{
		jmethodID ShowKeyboard = lJNIEnv->GetStaticMethodID( sdkClass, "ShowKeyboard","(Landroid/app/Activity;II)V" );

		lJNIEnv->CallStaticVoidMethod( sdkClass, ShowKeyboard, lNativeActivity, multiline, inputType );
	}
	else
	{
		jmethodID HideKeyboard = lJNIEnv->GetStaticMethodID( sdkClass, "HideKeyboard","(Landroid/app/Activity;)V" );
		
		lJNIEnv->CallStaticVoidMethod( sdkClass, HideKeyboard, lNativeActivity );
	}

	vm->DetachCurrentThread();
}

void agk::KeyboardMode( int mode )
{
	if ( m_iKeyboardMode != mode && (m_bInputStarted || cEditBox::GetCurrentFocus()) )
	{
		if ( mode == 2 ) 
		{
			if ( m_bInputStarted )
			{
				showKeyboard( true, 0, 0 );
				agk::PlatformChangeTextInput( m_sCurrInput );
			}
			else if ( cEditBox::GetCurrentFocus() ) 
			{
				showKeyboard( true, cEditBox::GetCurrentFocus()->GetMultiLine() ? 1 : 0, cEditBox::GetCurrentFocus()->GetInputType() );
				uString currText;
				cEditBox::GetCurrentFocus()->GetText( currText );
				agk::PlatformChangeTextInput( currText );
			}		
		}
		else showKeyboard( false, 0, 0 );
	}
	m_iKeyboardMode = mode;
}

int agk::PlatformInputPointerPressed(float x, float y)
{
	//showKeyboard(true,0,0);

	if ( g_bEditBoxHack )
	{
		// check if pressed on an edit box
		if ( m_cEditBoxList.GetCount() > 0 )
		{
			cEditBox *pFound = 0;
			cEditBox *pEditBox = m_cEditBoxList.GetFirst();
			while ( pEditBox )
			{
				float fWorldX = x;
				float fWorldY = y;

				if ( !pEditBox->GetFixed() )
				{	
					fWorldX = agk::ScreenToWorldX( x );
					fWorldY = agk::ScreenToWorldY( y );
				}

				if ( pEditBox->GetHitTest( fWorldX, fWorldY ) && pEditBox->GetVisible() && pEditBox->GetActive() && !pEditBox->HasFocus() )
				{
					pEditBox->SetFocus( true );
					pEditBox->SetCursorLocation( fWorldX, fWorldY );
					pFound = pEditBox;
				}
				
				pEditBox = m_cEditBoxList.GetNext();
			}

			if ( pFound )
			{
				pFound->GetText( m_sCurrInput );
				g_pTextInputText->SetString( m_sCurrInput );

				pEditBox = m_cEditBoxList.GetFirst();
				while ( pEditBox )
				{
					if ( pEditBox != pFound ) pEditBox->SetFocus( false );
					pEditBox = m_cEditBoxList.GetNext();
				}
			}
			else
			{
				showKeyboard(false,0,0);
				m_bInputStarted = false;
				m_bInputCancelled = true;
				int editbox = agk::GetCurrentEditBox();
				if ( editbox > 0 ) agk::SetEditBoxFocus( editbox, 0 );
				return 0;
			}
		}
	}
	else
	{
		showKeyboard(true,0,0);
		agk::PlatformChangeTextInput( m_sCurrInput );
	}
	/*
	else
	{
		showKeyboard(false,0,0);
		m_bInputStarted = false;
		m_bInputCancelled = true;
		int editbox = agk::GetCurrentEditBox();
		if ( editbox > 0 ) agk::SetEditBoxFocus( editbox, 0 );
	}
	*/

	return 1;
}

void agk::PlatformStartTextInput( const char *sInitial )
{
	if ( m_bInputStarted ) return;

	showKeyboard( true,0,0 );

	// doesn't work
	//ANativeActivity_showSoftInput( g_pActivity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED );
	
	m_sCurrInput.SetStr( sInitial );
	g_pTextInputText->SetString( sInitial );
	g_iTextCursorPos = g_pTextInputText->GetLength();
	
	g_bEditBoxHack = false;
	g_bPasswordMode = false;

	if ( m_iKeyboardMode == 2 )
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		jmethodID SetText = lJNIEnv->GetStaticMethodID( sdkClass, "SetInputText", "(Landroid/app/Activity;Ljava/lang/String;I)V" );

		jstring text = lJNIEnv->NewStringUTF(sInitial);
		lJNIEnv->CallStaticVoidMethod( sdkClass, SetText, lNativeActivity, text, m_sCurrInput.GetNumChars() );
		lJNIEnv->DeleteLocalRef( text );

		vm->DetachCurrentThread();

		g_fChangeTimer = 0.25;
	}
}

void agk::PlatformStopTextInput()
{
	if ( !m_bInputStarted ) return;

	showKeyboard( false,0,0 );
}

void agk::PlatformChangeTextInput( const char* str )
{
	m_sCurrInput.SetStr( str );

	if ( m_iKeyboardMode == 2 )
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		jmethodID SetText = lJNIEnv->GetStaticMethodID( sdkClass, "SetInputText", "(Landroid/app/Activity;Ljava/lang/String;I)V" );

		jstring text = lJNIEnv->NewStringUTF(str);
		lJNIEnv->CallStaticVoidMethod( sdkClass, SetText, lNativeActivity, text, m_sCurrInput.GetNumChars() );
		lJNIEnv->DeleteLocalRef( text );

		vm->DetachCurrentThread();

		g_fChangeTimer = 0.25;
	}
}

void agk::PlatformUpdateTextInput()
{
	if ( !m_bInputStarted ) return;

	if ( g_fChangeTimer > 0 )
	{
		g_fChangeTimer -= agk::GetFrameTime();
		return;
	}

	// if virtual keyboard then grab text from Android edit box instead
	if ( m_iKeyboardMode == 2 )
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		jmethodID GetText = lJNIEnv->GetStaticMethodID( sdkClass, "GetInputText", "(Landroid/app/Activity;)Ljava/lang/String;" );
		jmethodID GetFinished = lJNIEnv->GetStaticMethodID( sdkClass, "GetInputFinished", "(Landroid/app/Activity;)I" );
		jmethodID GetCursor = lJNIEnv->GetStaticMethodID( sdkClass, "GetInputCursor", "(Landroid/app/Activity;)I" );

		jstring text = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, GetText, lNativeActivity );
		int finished = lJNIEnv->CallStaticIntMethod( sdkClass, GetFinished, lNativeActivity );
		g_iTextCursorPos = lJNIEnv->CallStaticIntMethod( sdkClass, GetCursor, lNativeActivity );

		jboolean bCopy;
		const char* sText = lJNIEnv->GetStringUTFChars( text, &bCopy );

		m_sCurrInput.SetStr( sText );
		
		lJNIEnv->ReleaseStringUTFChars( text, sText );
		lJNIEnv->DeleteLocalRef( text );
		
		vm->DetachCurrentThread();
		
		//g_iShowCursor = 0;

		if ( finished || (m_iPrevKeyDown[ AGK_KEY_ESCAPE ] && !m_iKeyDown[ AGK_KEY_ESCAPE ]) )
		{
			m_bInputCancelled = false;
			m_bInputStarted = false;
			showKeyboard( false,0,0 );
			int editbox = agk::GetCurrentEditBox();
			if ( editbox > 0 ) agk::SetEditBoxFocus( editbox, 0 );
		}
		
		// Normal Android handles everything in the EditText above, but Amazon sends some events to us instead, so process them below
		//return;
	}

	bool bChanged = false;
	
	if ( m_iLastChar != m_iCurrChar )
	{
		if ( m_iInputMaxChars > 0 && m_sCurrInput.GetNumChars() >= m_iInputMaxChars )
		{
			g_iShowCursor = 1;
			g_fCursorBlinkTime = m_fCursorBlinkDelay;
		}
		else
		{
			if ( g_iTextCursorPos < (int) m_sCurrInput.GetNumChars() )
			{
				m_sCurrInput.InsertUnicodeAt( g_iTextCursorPos, m_iCurrChar );
			}
			else
			{
				m_sCurrInput.AppendUnicode( m_iCurrChar );
			}
			
			bChanged = true;
			g_iTextCursorPos++;
			if ( g_iTextCursorPos > (int) m_sCurrInput.GetNumChars() ) g_iTextCursorPos = m_sCurrInput.GetNumChars();
		}
	}
	
	if ( agk::GetPointerPressed() && m_bInputStarted )
	{
		showKeyboard(false,0,0);
		m_bInputStarted = false;
		m_bInputCancelled = true;
		int editbox = agk::GetCurrentEditBox();
		if ( editbox > 0 ) agk::SetEditBoxFocus( editbox, 0 );
	}
	
	// enter released
	if ( m_iPrevKeyDown[ AGK_KEY_ENTER ] && !m_iKeyDown[ AGK_KEY_ENTER ] )
	{
		m_bInputStarted = false;
		m_bInputCancelled = false;
		showKeyboard( false,0,0 );
		int editbox = agk::GetCurrentEditBox();
		if ( editbox > 0 ) agk::SetEditBoxFocus( editbox, 0 );
	}
		
	// escape released
	if ( m_iPrevKeyDown[ AGK_KEY_ESCAPE ] && !m_iKeyDown[ AGK_KEY_ESCAPE ] )
	{
		m_bInputCancelled = false;
		m_bInputStarted = false;
		showKeyboard( false,0,0 );
		int editbox = agk::GetCurrentEditBox();
		if ( editbox > 0 ) agk::SetEditBoxFocus( editbox, 0 );
	}
	
	// backspace held
	static float fBackspaceTimer = 0;
	bool bBackspaceRepeat = false;
	if ( !m_iKeyDown[ AGK_KEY_BACK ] ) fBackspaceTimer = 0;
	
	if ( fBackspaceTimer > 0 )
	{
		fBackspaceTimer -= agk::GetFrameTime();
		if ( fBackspaceTimer <= 0 ) 
		{
			fBackspaceTimer = 0.05f;
			bBackspaceRepeat = true;
		}
	}
	
	if ( (m_iPrevKeyDown[ AGK_KEY_BACK ] && !m_iKeyDown[ AGK_KEY_BACK ]) || bBackspaceRepeat )
	{
		if ( g_iTextCursorPos > 0 )
		{
			m_sCurrInput.DeleteCharAt( g_iTextCursorPos-1 );
			g_iTextCursorPos--;
			bChanged = true;
		}
		g_iShowCursor = 1;
		g_fCursorBlinkTime = m_fCursorBlinkDelay;
		
		if ( fBackspaceTimer <= 0 ) fBackspaceTimer = 0.5f;
	}
	
	// delete held
	static float fDeleteTimer = 0;
	bool bDeleteRepeat = false;
	if ( !m_iKeyDown[ AGK_KEY_DELETE ] ) fDeleteTimer = 0;
	
	if ( fDeleteTimer > 0 )
	{
		fDeleteTimer -= agk::GetFrameTime();
		if ( fDeleteTimer <= 0 ) 
		{
			fDeleteTimer = 0.05f;
			bDeleteRepeat = true;
		}
	}
	
	// delete released
	if ( (m_iPrevKeyDown[ AGK_KEY_DELETE ] && !m_iKeyDown[ AGK_KEY_DELETE ]) || bDeleteRepeat )
	{
		if ( g_iTextCursorPos < (int)m_sCurrInput.GetNumChars() )
		{
			m_sCurrInput.DeleteCharAt( g_iTextCursorPos );
			bChanged = true;
		}
		
		if ( fDeleteTimer <= 0 ) fDeleteTimer = 0.5f;
	}
	
	// left repeat
	static float fLeftTimer = 0;
	bool bLeftRepeat = false;
	if ( !m_iKeyDown[ AGK_KEY_LEFT ] ) fLeftTimer = 0;
	
	if ( fLeftTimer > 0 )
	{
		fLeftTimer -= agk::GetFrameTime();
		if ( fLeftTimer <= 0 ) 
		{
			fLeftTimer = 0.05f;
			bLeftRepeat = true;
		}
	}
	
	// left arrow released
	if ( (m_iPrevKeyDown[ AGK_KEY_LEFT ] && !m_iKeyDown[ AGK_KEY_LEFT ]) || bLeftRepeat )
	{
		g_iTextCursorPos--;
		if ( g_iTextCursorPos < 0 ) g_iTextCursorPos = 0;
		g_iShowCursor = 1;
		g_fCursorBlinkTime = m_fCursorBlinkDelay;
		
		if ( fLeftTimer <= 0 ) fLeftTimer = 0.5f;
	}
	
	// right repeat
	static float fRightTimer = 0;
	bool bRightRepeat = false;
	if ( !m_iKeyDown[ AGK_KEY_RIGHT ] ) fRightTimer = 0;
	
	if ( fRightTimer > 0 )
	{
		fRightTimer -= agk::GetFrameTime();
		if ( fRightTimer <= 0 ) 
		{
			fRightTimer = 0.05f;
			bRightRepeat = true;
		}
	}
	
	// right arrow released
	if ( (m_iPrevKeyDown[ AGK_KEY_RIGHT ] && !m_iKeyDown[ AGK_KEY_RIGHT ]) || bRightRepeat )
	{
		g_iTextCursorPos++;
		if ( g_iTextCursorPos > (int) m_sCurrInput.GetNumChars() ) g_iTextCursorPos = m_sCurrInput.GetNumChars();
		g_iShowCursor = 1;
		g_fCursorBlinkTime = m_fCursorBlinkDelay;
		
		if ( fRightTimer <= 0 ) fRightTimer = 0.5f;
	}
	
	// end released
	if ( m_iPrevKeyDown[ AGK_KEY_END ] && !m_iKeyDown[ AGK_KEY_END ] )
	{
		g_iTextCursorPos = m_sCurrInput.GetNumChars();
		g_iShowCursor = 1;
		g_fCursorBlinkTime = m_fCursorBlinkDelay;
	}
	
	// home released
	if ( m_iPrevKeyDown[ AGK_KEY_HOME ] && !m_iKeyDown[ AGK_KEY_HOME ] )
	{
		g_iTextCursorPos = 0;
		g_iShowCursor = 1;
		g_fCursorBlinkTime = m_fCursorBlinkDelay;
	}

	if ( bChanged && m_iKeyboardMode == 2 )
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		jmethodID SetText = lJNIEnv->GetStaticMethodID( sdkClass, "SetInputText", "(Landroid/app/Activity;Ljava/lang/String;I)V" );

		jstring text = lJNIEnv->NewStringUTF(m_sCurrInput.GetStr());
		lJNIEnv->CallStaticVoidMethod( sdkClass, SetText, lNativeActivity, text, g_iTextCursorPos );
		lJNIEnv->DeleteLocalRef( text );

		vm->DetachCurrentThread();
	}
}

void agk::PlatformDrawTextInput()
{
	if ( !m_bInputStarted ) return;
	if ( g_bEditBoxHack ) return;

	pTextBackground->SetPosition( -m_iDisplayExtraX, -m_iDisplayExtraY );
	pTextBackground->SetSize( m_iDisplayWidth+m_iDisplayExtraX*2, m_iDisplayHeight+m_iDisplayExtraY*2 );
	pTextBackground->Draw();
	
	float virtualWidth = m_iDisplayWidth;
	float virtualHeight = m_iDisplayHeight;
	
	float DevToVirX = (agk::GetDeviceDPI() / 150.0f) * virtualWidth / agk::m_fTargetViewportWidth;
	float DevToVirY = (agk::GetDeviceDPI() / 150.0f) * virtualHeight / agk::m_fTargetViewportHeight;
	
	float width = 300 * DevToVirX;
	float height = 30 * DevToVirY;
	
	if ( width > virtualWidth ) width = virtualWidth;
	
	g_fTextStartX = (virtualWidth - width)/2.0f + 3*DevToVirX;
	g_fTextStartY = virtualHeight/3.0f + 2*DevToVirY;
	
	g_pTextInputCursor->SetSize( 2 * DevToVirX, 26 * DevToVirY );
	g_pTextInputCursor->SetColor( 102, 213, 255, 255 );
	
	g_pTextInputArea->SetSize( width, height );
	g_pTextInputArea->SetColor( 255,255,255,255 );
	g_pTextInputArea->SetPosition( (virtualWidth - width) / 2.0f, virtualHeight/3.0f );
	
	width += 8 * DevToVirX;
	height += 8 * DevToVirY;
	
	g_pTextInputBack->SetSize( width, height );
	g_pTextInputBack->SetColor( 190,190,190,255 );
	g_pTextInputBack->SetPosition( (virtualWidth - width) / 2.0f, virtualHeight/3.0f - 4*DevToVirY );
	
	g_pTextInputText->SetPosition( g_fTextStartX, g_fTextStartY );
	g_pTextInputText->SetColor( 0,0,0 );
	g_pTextInputText->SetSpacing( 0 );
	g_pTextInputText->SetSize( 28 * DevToVirY );
	
	g_pTextInputBack->Draw();
	g_pTextInputArea->Draw();
	
	float fX = g_pTextInputArea->GetX();
	float fY = g_pTextInputArea->GetY();
	float fWidth = g_pTextInputArea->GetWidth();
	float fHeight = g_pTextInputArea->GetHeight();
	float fX2 = g_pTextInputArea->GetX() + g_pTextInputArea->GetWidth();
	float fY2 = g_pTextInputArea->GetY() + g_pTextInputArea->GetHeight();

	int iScissorX = 0;
	int iScissorY = 0;
	int iScissorWidth = 0;
	int iScissorHeight = 0;

	uint32_t flipped = 0;
	if ( !agk::GetRenderer()->IsTopLeftOrigin() ) flipped = 1 - flipped;
	if ( agk::GetRenderer()->IsFBOFlipped() && AGKFrameBuffer::g_pCurrFrameBuffer ) flipped = 1 - flipped;

	if ( flipped )
	{
		iScissorX = agk::ScreenToViewportX( fX );
		iScissorY = agk::ScreenToViewportY( fY2 );
		iScissorWidth = agk::ScreenToViewportX( fX2 ) - iScissorX;
		iScissorHeight = agk::ScreenToViewportY( fY ) - iScissorY;
	}
	else
	{
		iScissorX = agk::ScreenToViewportX( fX );
		iScissorY = agk::ScreenToViewportY( fY );
		iScissorWidth = agk::ScreenToViewportX( fX2 ) - iScissorX;
		iScissorHeight = agk::ScreenToViewportY( fY2 ) - iScissorY;
	}

	g_pTextInputText->SetManualScissor( iScissorX, iScissorY, iScissorWidth, iScissorHeight );
	g_pTextInputCursor->SetManualScissor( iScissorX, iScissorY, iScissorWidth, iScissorHeight );

	if ( g_bPasswordMode )
	{
		int length = m_sCurrInput.GetNumChars();
		uString pass("", length);
		for ( int i = 0; i < length; i++ ) pass.AppendAscii( '*' );
		g_pTextInputText->SetString( pass );
	}
	else g_pTextInputText->SetString( m_sCurrInput );
	g_pTextInputText->ChangedAspect();

	float posX = g_pTextInputText->GetX();
	float posY = g_pTextInputText->GetY();
	if ( g_iTextCursorPos >= 0 )
	{
		if ( g_iTextCursorPos >= (int)g_pTextInputText->GetLength() ) 
		{
			posX += g_pTextInputText->GetCharX( g_pTextInputText->GetLength()-1 ) + g_pTextInputText->GetCharWidth( g_pTextInputText->GetLength()-1 );
			posY += g_pTextInputText->GetCharY( g_pTextInputText->GetLength()-1 );
		}
		else 
		{
			posX += g_pTextInputText->GetCharX( g_iTextCursorPos );
			posY += g_pTextInputText->GetCharY( g_iTextCursorPos );
		}
	}

	float padX = 2*DevToVirY;
	if ( posX + padX > fX + fWidth )
	{
		float newX = g_pTextInputText->GetX() - ((posX + padX) - (fX + fWidth));
		g_pTextInputText->SetPosition( newX, g_pTextInputText->GetY() );
	}
	else if ( posX < fX + padX )
	{
		float newX = g_pTextInputText->GetX() + ((fX + padX) - posX);
		if ( newX > fX + 1 ) newX = fX + 1;
		g_pTextInputText->SetPosition( newX, g_pTextInputText->GetY() );
	}

	g_pTextInputText->Draw();

	g_fCursorBlinkTime -= agk::GetFrameTime();
	if ( g_fCursorBlinkTime <= 0 )
	{
		g_iShowCursor = 1 - g_iShowCursor;
		g_fCursorBlinkTime = m_fCursorBlinkDelay;
	}

	if ( g_iShowCursor ) 
	{
		float posX = g_pTextInputText->GetX();
		float posY = g_pTextInputText->GetY();
		if ( g_iTextCursorPos >= 0 )
		{
			if ( g_iTextCursorPos >= (int)g_pTextInputText->GetLength() ) 
			{
				posX += g_pTextInputText->GetCharX( g_pTextInputText->GetLength()-1 ) + g_pTextInputText->GetCharWidth( g_pTextInputText->GetLength()-1 );
				posY += g_pTextInputText->GetCharY( g_pTextInputText->GetLength()-1 );
			}
			else 
			{
				posX += g_pTextInputText->GetCharX( g_iTextCursorPos );
				posY += g_pTextInputText->GetCharY( g_iTextCursorPos );
			}
		}

		g_pTextInputCursor->SetPosition( posX, posY );
		g_pTextInputCursor->Draw();
	}
}

void agk::AppStart( void* activity )
{
	g_pActivity = (ANativeActivity*) activity;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	// AGKHelper onStart
	jclass AGKHelper = GetAGKHelper( lJNIEnv );
	if ( !AGKHelper ) 
	{
		agk::Message( "AGKHelper class not found, app was not compiled correctly" );
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "onStart", "(Landroid/app/Activity;)V" );
	
	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity );

	 // External commands onStart	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ExternalCommands" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	method = lJNIEnv->GetStaticMethodID( sdkClass, "onStart", "(Landroid/app/Activity;)V" );
	
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity );
	
	vm->DetachCurrentThread();
}

void agk::AppStop()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	// AGKHelper onStop
	jclass AGKHelper = GetAGKHelper( lJNIEnv );
	if ( !AGKHelper ) 
	{
		agk::Message( "AGKHelper class not found, app was not compiled correctly" );
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "onStop", "(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity );
	
	// External commands onStop
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ExternalCommands" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	method = lJNIEnv->GetStaticMethodID( sdkClass, "onStop", "(Landroid/app/Activity;)V" );	
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity );
	
	vm->DetachCurrentThread();
}

void agk::PlatformResumed()
{
	// do not do any OpenGL dependent code here, put it in PlatformResumedOpenGL
}

void agk::PlatformResumedOpenGL()
{
	// check for captured image
	if ( m_bIsCapturing )
	{
		if ( m_iUncollectedCaptureImage ) agk::DeleteImage( m_iUncollectedCaptureImage );
		
		uString sPath("/capturedimage.jpg");
		agk::PlatformGetFullPathWrite(sPath);

		FILE *pFile = fopen( sPath.GetStr(), "rb" );
		if ( !pFile )
		{
			m_iUncollectedCaptureImage = 0;
		}
		else
		{
			fclose(pFile);
			m_iUncollectedCaptureImage = agk::LoadImage( "/capturedimage.jpg" );
		}
		
		m_bIsCapturing = false;	
	}
	
	if ( m_bIsChoosing )
	{
		if ( m_pUncollectedChosenImage ) delete m_pUncollectedChosenImage;
		m_pUncollectedChosenImage = 0;
				
		uString sPath("/chosenimage.jpg");
		agk::PlatformGetFullPathWrite(sPath);

		FILE *pFile = fopen( sPath.GetStr(), "rb" );
		if ( pFile )
		{
			fclose(pFile);
			m_pUncollectedChosenImage = new cImage();
			m_pUncollectedChosenImage->Load( "/chosenimage.jpg" );
		}

		// finished choosing image
		m_bIsChoosing = false;
	}

	agk::SetVideoDimensions( m_fVideoX, m_fVideoY, m_fVideoWidth, m_fVideoHeight );
}

void agk::PlatformDeviceVolume()
{
	// android doesn't require app control of the device volume
}

uint32_t agk::PlatformLittleEndian( uint32_t u )
{
	// translate from local endian to little endian
	return u;
}

int agk::PlatformLittleEndian( int i )
{
	return i;
}

uint32_t agk::PlatformLocalEndian( uint32_t u )
{
	// translate from little endian to local endian
	return u;
}

int agk::PlatformLocalEndian( int i )
{
	return i;
}

void RemovePath( uString &sPath )
{
	int index = sPath.RevFind( '/' );
	if ( index >= 0 )
	{
		uString out;
		sPath.SubString( out, index+1 );
		sPath.SetStr( out );
	}
}

// Image

struct my_error_mgr {
  jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

static void my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  //(*cinfo->err->output_message) (cinfo);

  uString errstr;
  errstr.Format( "JPEG Error: %d", cinfo->err->msg_code );
  agk::Warning( errstr );

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

bool loadJpegAsset(AAsset* asset, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData)
{
	jpeg_decompress_struct cinfo;
	my_error_mgr jerr;

	FILE * infile;
	JSAMPARRAY buffer;
	int row_stride;

	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	int size = AAsset_getLength( asset );
	unsigned char *fileBuf = new unsigned char[ size ];

	AAsset_read( asset, fileBuf, size );

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		return false;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_mem_src(&cinfo, fileBuf, size );

	/* Step 3: read file parameters with jpeg_read_header() */

	(void) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	*   (a) suspension is not possible with the stdio data source, and
	*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	* See libjpeg.txt for more info.
	*/

	/* Step 4: set parameters for decompression */

	/* In this example, we don't need to change any of the defaults set by
	* jpeg_read_header(), so we do nothing here.
	*/

	/* Step 5: Start decompressor */

	(void) jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	* In this example, we need to make an output work buffer of the right size.
	*/
	/* JSAMPLEs per row in output buffer */

	outWidth = cinfo.output_width;
	outHeight = cinfo.output_height;
	outHasAlpha = false;
	*outData = new unsigned char[ outWidth * outHeight * cinfo.output_components ];
	buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, cinfo.output_width*cinfo.output_components, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/

	int line = 0;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		for ( int i = 0; i < outWidth*cinfo.output_components; i++ )
		{
			int index = line*outWidth*cinfo.output_components + i;
			(*outData)[ index ] = buffer[0][i];
		}

		line++;
	}

	// convert greyscale to RGB
	if ( cinfo.output_components == 1 )
	{
		unsigned char *newData = new unsigned char[ outWidth * outHeight * 3 ];
		for ( int i = 0; i < outWidth; i++ )
		{
			for ( int j = 0; j < outHeight; j++ )
			{
				int index = j*outWidth + i;
				newData[ index*3 ] = (*outData)[ index ];
				newData[ index*3 + 1 ] = (*outData)[ index ];
				newData[ index*3 + 2 ] = (*outData)[ index ];
			}
		}

		delete [] (*outData);
		*outData = newData;
	}

	/* Step 7: Finish decompression */

	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	* Here we postpone it until after no more JPEG errors are possible,
	* so as to simplify the setjmp error logic above.  (Actually, I don't
	* think that jpeg_destroy can do an error exit, but why assume anything...)
	*/
	delete [] fileBuf;

    /* That's it */
    return true;
}

void png_read_asset(png_structp png_ptr, png_bytep data, png_size_t length)
{
	AAsset* asset = (AAsset*) png_get_io_ptr(png_ptr);
	int result = AAsset_read( asset, data, length );
	if ( result < 0 )
	{
		png_error(png_ptr, "Asset Read Error");
	}
	else
	{
		int total = result;
		while ( result > 0 && total < length )
		{
			result = AAsset_read( asset, data+total, length-total );
			if ( result > 0 ) total += result;
			if ( result < 0 )
			{
				png_error(png_ptr, "Asset Read Error");
			}
		}
	}
}

void png_write_asset(png_structp png_ptr, png_bytep data, png_size_t length)
{
	return;
}

bool loadPngAsset( AAsset* asset, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData) {
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type;

    if (asset == 0)
    {
    	return false;
    }

    /* Create and initialize the png_struct
     * with the desired error handler
     * functions.  If you want to use the
     * default stderr and longjump method,
     * you can supply NULL for the last
     * three parameters.  We also supply the
     * the compiler header file version, so
     * that we know if the application
     * was compiled with a compatible version
     * of the library.  REQUIRED
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL) {
    	return false;
    }

    /* Allocate/initialize the memory
     * for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, 0, 0);
        return false;
    }

    png_set_read_fn( png_ptr, asset, png_read_asset );

    /* Set error handling if you are
     * using the setjmp/longjmp method
     * (this is the normal method of
     * doing things with libpng).
     * REQUIRED unless you  set up
     * your own error handlers in
     * the png_create_read_struct()
     * earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated
         * with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        /* If we get here, we had a
         * problem reading the file */
        agk::Warning( "Failed to read PNG file" );
        return false;
    }

    /* If we have already
     * read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);

    /*
     * If you have enough memory to read
     * in the entire image at once, and
     * you need to specify only
     * transforms that can be controlled
     * with one of the PNG_TRANSFORM_*
     * bits (this presently excludes
     * dithering, filling, setting
     * background, and doing gamma
     * adjustment), then you can read the
     * entire image (including pixels)
     * into the info structure with this
     * call
     *
     * PNG_TRANSFORM_STRIP_16 |
     * PNG_TRANSFORM_PACKING  forces 8 bit
     * PNG_TRANSFORM_EXPAND forces to
     *  expand a palette into RGB
     */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, 0);

    outWidth = info_ptr->width;
    outHeight = info_ptr->height;
		
	switch( info_ptr->color_type )
	{
		case PNG_COLOR_TYPE_GRAY:
		{
			outHasAlpha = false;
			unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
			*outData = (unsigned char*) malloc(row_bytes * 3 * outHeight);
			png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

			unsigned int out_row_bytes = row_bytes * 3;
			for (int i = 0; i < outHeight; i++) 
			{
				for ( int j = 0; j < outWidth; j++ )
				{
					(*outData)[ out_row_bytes*i + j*3 + 0 ] = row_pointers[i][j];
					(*outData)[ out_row_bytes*i + j*3 + 1 ] = row_pointers[i][j];
					(*outData)[ out_row_bytes*i + j*3 + 2 ] = row_pointers[i][j];
				}
			}
			break;
		}

		case PNG_COLOR_TYPE_GRAY_ALPHA:
		{
			outHasAlpha = true;
			unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
			*outData = (unsigned char*) malloc(row_bytes * 2 * outHeight);
			png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

			unsigned int out_row_bytes = row_bytes * 2;
			for (int i = 0; i < outHeight; i++) 
			{
				for ( int j = 0; j < outWidth; j++ )
				{
					(*outData)[ out_row_bytes*i + j*4 + 0 ] = row_pointers[i][j*2 + 0];
					(*outData)[ out_row_bytes*i + j*4 + 1 ] = row_pointers[i][j*2 + 0];
					(*outData)[ out_row_bytes*i + j*4 + 2 ] = row_pointers[i][j*2 + 0];
					(*outData)[ out_row_bytes*i + j*4 + 3 ] = row_pointers[i][j*2 + 1];
				}
			}
			break;
		}

		case PNG_COLOR_TYPE_RGB:
		{
			outHasAlpha = false;
			unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
			*outData = (unsigned char*) malloc(row_bytes * outHeight);
			png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

			for (int i = 0; i < outHeight; i++) {
				memcpy(*outData+(row_bytes * i), row_pointers[i], row_bytes);
			}
			break;
		}
	
		case PNG_COLOR_TYPE_RGBA:
		{
			outHasAlpha = true;
            unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
			*outData = (unsigned char*) malloc(row_bytes * outHeight);
			png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

			for (int i = 0; i < outHeight; i++) {
				memcpy(*outData+(row_bytes * i), row_pointers[i], row_bytes);
			}

            break;
		}

		default:
		{
        	uString err;
        	err.SetStr( "Color type " );
        	err.AppendInt( info_ptr->color_type );
        	err.Append( " not supported" );
        	agk::Warning( err );
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return false;
		}
    }

    /* Clean up after the read,
     * and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    /* That's it */
    return true;
}

bool cImage::ChooseFromSystem()
{
	// do nothing, not possible on android, use ShowChooseImageScreen() instead.
	return false;
}

void agk::PlatformShowChooseScreen()
{
	uString sPath("/chosenimage.jpg");
	agk::PlatformGetFullPathWrite(sPath);
	remove( sPath );

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "StartChooseImage", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring sText = lJNIEnv->NewStringUTF( sPath.GetStr() );
	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity, sText );
	lJNIEnv->DeleteLocalRef( sText );

	vm->DetachCurrentThread();
}

bool agk::PlatformShowCaptureScreen()
{
	uString sPath("/capturedimage.jpg");
	agk::PlatformGetFullPathWrite(sPath);
	remove( sPath );

	if ( CheckPermission( "Camera" ) != 2 )
	{
		agk::Error( "Cannot use the camera without the camera permission, use RequestPermission(\"Camera\") first" );
		return false;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "CaptureImage", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring sText = lJNIEnv->NewStringUTF( sPath.GetStr() );
	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity, sText );
	lJNIEnv->DeleteLocalRef( sText );

	vm->DetachCurrentThread();

	return true;
}

void agk::PlatformHideCaptureScreen()
{
	// do nothing, not possible on Android
}

bool cImage::CaptureFromCamera()
{
	// do nothing, use ShowImageCaptureScreen() instead
	return false;
}

int agk::GetNumDeviceCameras()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CameraSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID Camera = lJNIEnv->GetStaticMethodID( sdkClass, "GetNumCameras","()I" );
	int num = lJNIEnv->CallStaticIntMethod( sdkClass, Camera );
	vm->DetachCurrentThread();

	return num;
}

int agk::SetDeviceCameraToImage( uint32_t cameraID, uint32_t imageID )
{
	cImage *pImage = m_cImageList.GetItem( imageID );
	if ( pImage )
	{
		if ( pImage != m_pCameraTexture )
		{
			agk::Error( "Failed to set device camera to image, image already exists" );
			return 0;
		}
		else
		{
			agk::Warning( "Device camera is already streaming to that image" );
			return 1;
		}
	}

	if ( imageID == 0 )
	{
		m_iDeviceCameraID = -1;

		if ( m_pCameraTexture )
		{
			// delete everything
			JNIEnv* lJNIEnv = g_pActivity->env;
			JavaVM* vm = g_pActivity->vm;
			vm->AttachCurrentThread(&lJNIEnv, NULL);

			jobject lNativeActivity = g_pActivity->clazz;
			if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
			
			jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CameraSDK" );
			if ( !sdkClass ) 
			{
				vm->DetachCurrentThread();
				return 0;
			}

			jmethodID Camera = lJNIEnv->GetStaticMethodID( sdkClass, "SetDeviceCameraToImage","(Landroid/app/Activity;II)V" );
			lJNIEnv->CallStaticVoidMethod( sdkClass, Camera, lNativeActivity, 0, 0 );
			vm->DetachCurrentThread();

			if ( m_pCameraTextureFBO ) delete m_pCameraTextureFBO; m_pCameraTextureFBO = 0;
			if ( m_pCameraTextureQuad ) delete m_pCameraTextureQuad; m_pCameraTextureQuad = 0;
			if ( m_pCameraTextureShader ) delete m_pCameraTextureShader; m_pCameraTextureShader = 0;
			GLESDeleteExternalTexture( &m_iCameraTextureRaw ); m_iCameraTextureRaw = 0;
			
			if ( m_pCameraTexture ) 
			{
				m_cImageList.RemoveItem( m_pCameraTexture->m_iID );
				delete m_pCameraTexture; 
				m_pCameraTexture = 0;
			}
		}

		return 1;
	}

	if ( CheckPermission("Camera") != 2 )
	{
		agk::Error("Failed to set device camera to image, app does not have permission to access the camera, use RequestPermission first");
		return 0;
	}

	m_iDeviceCameraID = cameraID;
	
	if ( m_pCameraTexture )
	{
		agk::Error( "Cannot change image ID whilst the camera is streaming, use an ID of 0 first to clear the existing image, then use the new ID" );
		return 0;
	}

	if ( g_pRenderer->GetIdentifier() == AGK_RENDERER_OPENGL_ES2 )
	{
		// create destination image
		GLESRegenerateExternalTexture( &m_iCameraTextureRaw );
	
		// send the texture to Android
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CameraSDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return 0;
		}

		jmethodID Camera = lJNIEnv->GetStaticMethodID( sdkClass, "SetDeviceCameraToImage","(Landroid/app/Activity;II)V" );
		lJNIEnv->CallStaticVoidMethod( sdkClass, Camera, lNativeActivity, m_iCameraTextureRaw, cameraID );

		Camera = lJNIEnv->GetStaticMethodID( sdkClass, "GetCameraWidth","()I" );
		iCameraWidth = lJNIEnv->CallStaticIntMethod( sdkClass, Camera );

		Camera = lJNIEnv->GetStaticMethodID( sdkClass, "GetCameraHeight","()I" );
		iCameraHeight = lJNIEnv->CallStaticIntMethod( sdkClass, Camera );

		Camera = lJNIEnv->GetStaticMethodID( sdkClass, "GetCameraType","(I)I" );
		m_iDeviceCameraType = lJNIEnv->CallStaticIntMethod( sdkClass, Camera, m_iDeviceCameraID );

		vm->DetachCurrentThread();
	}/*
	else if ( g_pRenderer->GetIdentifier() == AGK_RENDERER_VULKAN )
	{
		if ( !g_pVideoImageReader )
		{
			int width = 1024;
			int height = 1024;
			AImageReader_newWithUsage( width, height, AIMAGE_FORMAT_PRIVATE, AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE, 3, &g_pVideoImageReader );
		}
	}*/
	else
	{
		agk::Error( "SetDeviceCameraToImage is only supported with OpenGLES, please change the InitGraphics call to use AGK_RENDERER_MODE_ONLY_LOWEST" );
		return 0;
	}

	// create support items
	m_pCameraTexture = new cImage();
	m_pCameraTexture->CreateBlankImage( iCameraWidth, iCameraHeight, 0, 0 );
	m_pCameraTexture->m_iID = imageID;
	m_cImageList.AddItem( m_pCameraTexture, imageID );

	// create quad and shader
	m_pCameraTextureShader = g_pRenderer->LoadDefaultShaderDeviceCamera();
	
	m_pCameraTextureQuad = new cObject3D();
	m_pCameraTextureQuad->CreateQuad();
	m_pCameraTextureQuad->SetCullMode(1);
    m_pCameraTextureQuad->SetDepthReadMode(7);
	m_pCameraTextureQuad->SetShader( m_pCameraTextureShader );

	m_pCameraTextureFBO = new AGKFrameBuffer( m_pCameraTexture, true, 0, true, false );

	return 1;
}

void agk::DeviceCameraUpdate()
{
	if ( !m_iCameraTextureRaw ) return;
	if ( !m_pCameraTexture ) return;
	if ( !m_pCameraTextureQuad ) return;
	if ( !m_pCameraTextureFBO ) return;

	// update camera texture
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);
	jobject lNativeActivity = g_pActivity->clazz;

	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CameraSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Camera = lJNIEnv->GetStaticMethodID( sdkClass, "UpdateCamera","()V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Camera );

	jclass AGKHelper = GetAGKHelper( lJNIEnv );

	jmethodID methodGetOrien = lJNIEnv->GetStaticMethodID( AGKHelper, "GetOrientation","(Landroid/app/Activity;)I");
	int orien = lJNIEnv->CallStaticIntMethod( AGKHelper, methodGetOrien, lNativeActivity );

	vm->DetachCurrentThread();

	// check for orientation changes
	static int isTablet = -1;
	if ( isTablet < 0 )
	{
		if ( agk::GetDeviceWidth() > agk::GetDeviceHeight() )
		{
			if (orien == 0 || orien == 2) isTablet = 1;
			else isTablet = 0;
		}
		else 
		{
			if (orien == 1 || orien == 3) isTablet = 1;
			else isTablet = 0;
		}
	}

	if ( isTablet )
	{
		switch( orien )
		{
			case 0: orien = 3; break;
			case 1: orien = 2; break;
			case 2: orien = 4; break;
			case 3: orien = 1; break;
			default: break;
		}
	}
	else
	{
		switch( orien )
		{
			case 0: orien = 1; break;
			case 1: orien = 3; break;
			case 2: orien = 2; break;
			case 3: orien = 4; break;
			default: break;
		}
	}

	static int lastOrien = 0;
	if ( orien != lastOrien ) agk::OrientationChanged( orien );
	lastOrien = orien;

	// AGK may not have changed the orientation depending on the restrictions
	orien = GetOrientation();
	
	if ( orien <= 2 )
	{
		if ( iCameraWidth > iCameraHeight )
		{
			int temp = iCameraWidth;
			iCameraWidth = iCameraHeight;
			iCameraHeight = temp;
			m_pCameraTexture->CreateBlankImage( iCameraWidth, iCameraHeight, 0, 0 );
		}
	}
	else
	{
		if ( iCameraWidth < iCameraHeight )
		{
			int temp = iCameraWidth;
			iCameraWidth = iCameraHeight;
			iCameraHeight = temp;
			m_pCameraTexture->CreateBlankImage( iCameraWidth, iCameraHeight, 0, 0 );
		}
	}

	// front facing cameras are mirrored
	int flip = 1;
	if ( m_iDeviceCameraType == 2 ) flip = -1;

	switch( orien )
	{
		case 1: m_pCameraTextureShader->SetConstantByName( "orientation", 0, flip, -1, 0 ); break;
		case 2: m_pCameraTextureShader->SetConstantByName( "orientation", 0, -flip, 1, 0 ); break;
		case 3: m_pCameraTextureShader->SetConstantByName( "orientation", flip, 0, 0, 1 ); break;
		case 4: m_pCameraTextureShader->SetConstantByName( "orientation", -flip, 0, 0, -1 ); break;
	}
	
	// copy it to a normal image
	AGKFrameBuffer *pOldFBO = AGKFrameBuffer::g_pCurrFrameBuffer;
	AGKFrameBuffer::BindFrameBuffer( m_pCameraTextureFBO );
	g_pRenderer->SetViewport( 0, 0, m_pCameraTexture->GetWidth(), m_pCameraTexture->GetHeight() );
	g_pRenderer->ClearRenderTarget( 0,0,0,0 );

	g_pRenderer->SetViewport( 0, 0, m_pCameraTexture->GetWidth(), m_pCameraTexture->GetHeight() ); // if we don't do this again then Android seems to revert to the backbuffer viewport sizes

	// draw image to new image
	GLESBindExternalTexture( m_iCameraTextureRaw );
	m_pCameraTextureQuad->Draw();
    
    // restore old frame buffer
	AGKFrameBuffer::BindFrameBuffer( pOldFBO );
    if( pOldFBO ) g_pRenderer->SetViewport( 0, 0, pOldFBO->GetWidth(), pOldFBO->GetHeight() );
	else g_pRenderer->SetViewport( 0, 0, agk::m_iRenderWidth, agk::m_iRenderHeight );
}

void agk::DeviceCameraResumed()
{
	if ( m_iDeviceCameraID < 0 ) return;
	if ( !m_iCameraTextureRaw ) return;

	// send the texture to Android
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CameraSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Camera = lJNIEnv->GetStaticMethodID( sdkClass, "SetDeviceCameraToImage","(Landroid/app/Activity;II)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Camera, lNativeActivity, m_iCameraTextureRaw, m_iDeviceCameraID );

	vm->DetachCurrentThread();
}

int agk::GetDeviceCameraType( uint32_t cameraID )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CameraSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID Camera = lJNIEnv->GetStaticMethodID( sdkClass, "GetCameraType","(I)I" );
	int type = lJNIEnv->CallStaticIntMethod( sdkClass, Camera, cameraID );
	vm->DetachCurrentThread();

	return type;
}

void cImage::Print( float size )
{
	uint32_t* bits = 0;
	GetRawData( (unsigned char**) &bits );
	if ( !bits )
	{
		agk::Warning("Failed to get image data");
		return;
	}

	uString filePath = "/PrintedImage.png";
	agk::PlatformGetFullPathWrite( filePath );

	// write the image data
	write_png( filePath, m_iOrigWidth, m_iOrigHeight, bits );
	delete [] bits;

	agk::ShareImage( "/PrintedImage.png" );
}

bool cImage::PlatformGetDataFromFile( const char* szFile, unsigned char **pData, unsigned int *out_width, unsigned int *out_height )
{
	bool bIsAsset = false;
	
	uString sPath( szFile );

	if ( strncmp(szFile, "expansion:", strlen("expansion:")) == 0 )
	{
		sPath.SetStr( szFile + strlen("expansion") );
		sPath.Replace( ':', '/' );

		// always load from expansion file in case it has changed
		//if ( cFile::ExistsWrite( sPath ) ) agk::PlatformGetFullPathWrite( sPath );
		//else
		{
			agk::PlatformGetFullPathWrite( sPath );
			if ( !agk::ExtractExpansionFile( sPath, szFile ) ) return false;
		}
	}
	else
	{
		if ( cFile::ExistsRaw( szFile ) ) 
		{
			uString sRealPath; sRealPath.SetStrUTF8( sPath.GetStr()+4 );
			sPath.SetStr( sRealPath );
		}
		else if ( cFile::ExistsWrite( szFile ) ) agk::PlatformGetFullPathWrite( sPath );
		else if ( cFile::ExistsRead( szFile ) ) 
		{
			agk::PlatformGetFullPathRead( sPath );
			bIsAsset = true;
		}
		else
		{
			uString err; err.Format( "Could not find image: %s", szFile );
			agk::Error( err );
			return false;
		}
	}

	int width;
	int height;
	bool hasAlpha;
	
	bool result = false;
	const char *szExt = strrchr( szFile, '.' );
	char *szExtL = agk::Lower( szExt );

	bool bIsPNG = false;
	bool bIsJPG = false;

	if ( strcmp( szExtL, ".png" ) == 0 ) bIsPNG = true;
	else bIsJPG = true;

	delete [] szExtL;

	if ( bIsAsset )
	{
		AAssetManager* assetManager = g_pActivity->assetManager;
		AAsset* asset = AAssetManager_open(assetManager, sPath.GetStr(), AASSET_MODE_UNKNOWN);
		if (asset)
		{
			if ( bIsPNG ) result = loadPngAsset( asset, width, height, hasAlpha, pData );
			else if ( bIsJPG ) result = loadJpegAsset( asset, width, height, hasAlpha, pData );

			AAsset_close(asset);
		}
	}
	else
	{
		if ( bIsPNG ) result = loadPngImage( sPath.GetStr(), width, height, hasAlpha, pData );
		else if ( bIsJPG ) result = loadJpegImage( sPath.GetStr(), width, height, hasAlpha, pData );
	}

	if ( !result )
	{	
		uString str( "Failed to load image ", 100 ); str.Append( sPath ); str.Append( " (Note that Android paths are case sensitive)" );
		agk::Error( str );
		return false;
	}
	
	if ( out_width ) *out_width = width;
	if ( out_height ) *out_height = height;
	
	if ( !hasAlpha )
	{
		// add alpha channel
		unsigned char *tempData = new unsigned char[ width * height * 4 ];
		for ( int y = 0; y < height; y++ )
		{
			unsigned int index = y*width;

			for ( int x = 0; x < width; x++ )
			{
				tempData[ index*4 ] = (*pData)[ index*3 ];
				tempData[ index*4 + 1 ] = (*pData)[ index*3 + 1 ];
				tempData[ index*4 + 2 ] = (*pData)[ index*3 + 2 ];
				tempData[ index*4 + 3 ] = 255;
				index++;
			}
		}

		delete [] (*pData);
		*pData = tempData;
	}

	return true;
}

void agk::VibrateDevice( float seconds )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "Vibrate","(Landroid/app/Activity;F)V" );
	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity, seconds );

	vm->DetachCurrentThread();
}

void agk::SetClipboardText( const char* szText )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "SetClipboardText","(Landroid/app/Activity;Ljava/lang/String;)V" );
	jstring sText = lJNIEnv->NewStringUTF( szText );
	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity, sText );
	lJNIEnv->DeleteLocalRef( sText );

	vm->DetachCurrentThread();
}

char* agk::GetClipboardText()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetClipboardText","(Landroid/app/Activity;)Ljava/lang/String;" );
	jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, method, lNativeActivity );
	jboolean bCopy;
	const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

	char *retstr = new char[ strlen(str2) + 1 ];
	strcpy( retstr, str2 );
		
	lJNIEnv->ReleaseStringUTFChars( str, str2 );
	lJNIEnv->DeleteLocalRef( str );

	vm->DetachCurrentThread();

	return retstr;
}

// Music

void cMusicMgr::PlatformAddFile( cMusic *pMusic )
{
	/*
	uString sFilename = pMusic->m_sFile;
	sFilename.Lower();
	if ( sFilename.FindStr(".mp3") && !cFile::Exists( pMusic->m_sFile ) )
	{
		pMusic->m_sFile.Trunc( '.' );
		pMusic->m_sFile.Append( ".m4a" );
	}
	*/
}

bool cMusicMgr::PlatformPrepare( uint32_t iID )
{
	cMusic *pMusic = m_pMusicFiles[ iID ];
	
	m_pCurrentlyPlaying = 0;
	
	if ( pMusicPlayer )
	{
		if ( pMusicPlayer )
		{
			(*pMusicPlayer)->Destroy(pMusicPlayer);
			pMusicPlayer = 0;
		}
	}
	
	off_t start = 0;
	off_t length;
	int fd = open( pMusic->m_sFile.GetStr(), O_RDONLY, 0777 );
	if ( fd < 0 )
	{
		AAssetManager* assetManager = g_pActivity->assetManager;
		AAsset* asset = AAssetManager_open(assetManager, pMusic->m_sFile.GetStr(), AASSET_MODE_UNKNOWN);
		if ( !asset ) return false;
		
		fd = AAsset_openFileDescriptor(asset, &start, &length);
		AAsset_close(asset);
		if ( fd < 0 )
		{
			uString err( "Failed to load music file ", 50 );
			err.Append( pMusic->m_sFile );
			agk::Error( err );
			return false;
		}
	}
	else
	{
		struct stat buf;
		fstat(fd, &buf);
		length = buf.st_size;
	}
	
	m_pCurrentlyPlaying = pMusic;
	
	// configure audio source
	SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
	SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
	SLDataSource audioSrc = {&loc_fd, &format_mime};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	SLDataSink audioSnk = {&loc_outmix, NULL};

	// create audio player
	const SLInterfaceID ids[2] = {SL_IID_VOLUME, SL_IID_SEEK};
	const SLboolean req[2] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE};
	SLresult result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pMusicPlayer, &audioSrc, &audioSnk, 2, ids, req);
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to create music player" );
		return false;
	}
	
	// realize the player
	result = (*pMusicPlayer)->Realize(pMusicPlayer, SL_BOOLEAN_FALSE);
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to initialise music player" );
		return false;
	}
	
	// get the play interface
	result = (*pMusicPlayer)->GetInterface(pMusicPlayer, SL_IID_PLAY, &pMusicPlayerPlay);
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to get music player interface" );
		return false;
	}

	// get the seek interface
	result = (*pMusicPlayer)->GetInterface(pMusicPlayer, SL_IID_SEEK, &pMusicPlayerSeek);
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to get music seeking interface" );
		return false;
	}
	
	result = (*pMusicPlayerPlay)->RegisterCallback(pMusicPlayerPlay, &MusicEventCallback, (void*)this );
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to set music callback" );
		return false;
	}
	
	result = (*pMusicPlayerPlay)->SetCallbackEventsMask( pMusicPlayerPlay, SL_PLAYEVENT_HEADATEND );
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to set music callback flags" );
		return false;
	}

	return true;
}

void cMusicMgr::Play( uint32_t iID, bool bLoop, uint32_t iStartID, uint32_t iEndID )
{
	if ( iStartID < 1 ) iStartID = 1;
	if ( iStartID >= MAX_MUSIC_FILES ) iStartID = MAX_MUSIC_FILES - 1;
	if ( iEndID < iStartID ) iEndID = iStartID;
	if ( iEndID >= MAX_MUSIC_FILES ) iEndID = MAX_MUSIC_FILES - 1;
	
	m_iStartID = iStartID;
	m_iEndID = iEndID;
	m_bLoop = bLoop;
	
	if ( iID < 1 || iID >= MAX_MUSIC_FILES ) 
	{
		uString str( "Could not play music file ", 100 ); 
		str.AppendUInt( iID ).Append( " - ID must be between 1 and " ).AppendInt( MAX_MUSIC_FILES-1 );
		agk::Error( str );
		return;
	}
	
	if ( !m_pMusicFiles[ iID ] )
	{
		uString str( "Could not play music file ", 100 );
		str.AppendUInt( iID ).Append(" - ID does not exist" );
		agk::Error( str );
		return;
	}
	
	if ( !PlatformPrepare( iID ) ) return;
	
	cMusic *pMusic = m_pMusicFiles[ iID ];
	
	float volume = m_iMasterVolume / 100.0f;
	volume *= pMusic->m_iVolume / 100.0f;

	// convert to centibels
	if ( volume > 0.00001f ) volume = 100.0f * 20.0f * log10(volume);
	else volume = -10000;

	if ( pMusicPlayer )
	{
		SLVolumeItf volumeIf;
		(*pMusicPlayer)->GetInterface(pMusicPlayer, SL_IID_VOLUME, &volumeIf);
		(*volumeIf)->SetVolumeLevel(volumeIf, volume);
	}
	
	SLresult result = (*pMusicPlayerPlay)->SetPlayState( pMusicPlayerPlay, SL_PLAYSTATE_PLAYING );
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to start playing music" );
		return;
	}
}

void cMusicMgr::Pause()
{
	if ( !pMusicPlayer ) return;
	(*pMusicPlayerPlay)->SetPlayState( pMusicPlayerPlay, SL_PLAYSTATE_PAUSED );
}

void cMusicMgr::Resume()
{
	if ( !pMusicPlayer ) return;
	(*pMusicPlayerPlay)->SetPlayState( pMusicPlayerPlay, SL_PLAYSTATE_PLAYING );
}

void cMusicMgr::Stop()
{
	if ( !pMusicPlayer ) return;
	(*pMusicPlayer)->Destroy( pMusicPlayer );
	pMusicPlayer = 0;
	m_pCurrentlyPlaying = 0;
}

void cMusicMgr::SetMasterVolume( int vol )
{
	if ( vol > 100 ) vol = 100;
	if ( vol < 0 ) vol = 0;
	m_iMasterVolume = vol;
	
	float volume = 1.0f;
	if ( m_pCurrentlyPlaying ) volume = m_pCurrentlyPlaying->m_iVolume / 100.0f;
	volume *= m_iMasterVolume / 100.0f;
	
	// convert to centibels
	if ( volume > 0.00001f ) volume = 100.0f * 20.0f * log10(volume);
	else volume = -10000;

	if ( pMusicPlayer )
	{
		SLVolumeItf volumeIf;
		(*pMusicPlayer)->GetInterface(pMusicPlayer, SL_IID_VOLUME, &volumeIf);
		(*volumeIf)->SetVolumeLevel(volumeIf, volume);
	}
}

float cMusicMgr::GetDuration( uint32_t ID )
{
	if ( m_pCurrentlyPlaying )
	{
		if ( m_pCurrentlyPlaying->m_iID != ID )
		{
#ifdef _AGK_ERROR_CHECK
			agk::Error( "Cannot get music file duration whilst another file is playing" );
#endif
			return 0;
		}
	}
	else
	{
		if ( !PlatformPrepare( ID ) ) return 0;
	}

	SLmillisecond dur = 0;
	(*pMusicPlayerPlay)->GetDuration( pMusicPlayerPlay, &dur );
	float seconds = dur / 1000.0f;
	return seconds;
}

void cMusicMgr::Seek( float seconds, int mode )
{
	if ( !m_pCurrentlyPlaying )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot seek as no music is playing" );
#endif
		return;
	}

	SLmillisecond pos = (SLmillisecond) (seconds*1000);
	if ( mode == 0 )
	{
		(*pMusicPlayerSeek)->SetPosition( pMusicPlayerSeek, pos, SL_SEEKMODE_ACCURATE );
	}
	else
	{
		SLmillisecond currpos = 0;
		(*pMusicPlayerPlay)->GetPosition( pMusicPlayerPlay, &currpos );
		currpos += pos;
		(*pMusicPlayerSeek)->SetPosition( pMusicPlayerSeek, currpos, SL_SEEKMODE_ACCURATE );
	}
}

float cMusicMgr::GetPosition()
{
	if ( !m_pCurrentlyPlaying ) return 0;
	
	SLmillisecond pos = 0;
	(*pMusicPlayerPlay)->GetPosition( pMusicPlayerPlay, &pos );
	float seconds = pos / 1000.0f;
	return seconds;
}

void cMusicMgr::HandleEvent()
{
	//agk::Warning( "Music Finished" );
	if ( m_pCurrentlyPlaying == 0 ) return;
	if ( !pMusicPlayer ) return;
		
	//if ( [musicPlayer rate] == 0 )
	{
		uint32_t iCurrID = m_pCurrentlyPlaying->m_iID;
		uint32_t iNextID = 0;
		
		if ( iCurrID >= m_iEndID )
		{
			if ( m_bLoop ) 
			{
				for ( uint32_t i = m_iStartID; i <= m_iEndID; i++ )
				{
					if ( m_pMusicFiles[ i ] ) 
					{
						iNextID = i;
						break;
					}
				}
			}
		}
		else
		{
			for ( uint32_t i = iCurrID+1; i <= m_iEndID; i++ )
			{
				if ( m_pMusicFiles[ i ] ) 
				{
					iNextID = i;
					break;
				}
			}
			
			if ( iNextID == 0 && m_bLoop ) 
			{
				for ( uint32_t i = m_iStartID; i <= iCurrID; i++ )
				{
					if ( m_pMusicFiles[ i ] ) 
					{
						iNextID = i;
						break;
					}
				}
			}
		}
		
		if ( iNextID > 0 ) Play( iNextID, m_bLoop, m_iStartID, m_iEndID );
		else m_pCurrentlyPlaying = 0;
	}
}

// OGG Music

struct AGKOGGSoundData
{
	SLObjectItf slPlayerObject;
	SLPlayItf slPlayerPlay;
	SLAndroidSimpleBufferQueueItf slPlayerBufferQueue;

	unsigned char *m_pBuffers[ 2 ];
	unsigned int m_iResetFlag[ 2 ];
	unsigned int m_iCurrBuffer;
};

/*
void AGKOGGMusicEventCallback( SLPlayItf caller, void *pContext, SLuint32 event )
{		
	if ( event != SL_PLAYEVENT_HEADATEND ) return;
	if ( !pContext ) return;
	
	((AGKMusicOGG*)pContext)->m_bFinished = true;
}
*/

void AGKMusicOGG::PlatformInit()
{
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;
	
	if ( !m_pSoundData ) 
	{
		m_pSoundData = (void*) new AGKOGGSoundData();
		pTemp = (AGKOGGSoundData*)m_pSoundData;

		pTemp->m_iCurrBuffer = 0;
		pTemp->m_iResetFlag[ 0 ] = 0;
		pTemp->m_iResetFlag[ 1 ] = 0;
		pTemp->m_pBuffers[ 0 ] = new unsigned char[ AGK_OGG_BUFFER_SIZE ];
		pTemp->m_pBuffers[ 1 ] = new unsigned char[ AGK_OGG_BUFFER_SIZE ];
	}

	SLuint32 speakers = SL_SPEAKER_FRONT_CENTER;
	if ( m_fmt.nChannels > 1 ) speakers = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;

	// configure audio source
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
	SLDataFormat_PCM format_pcm = { SL_DATAFORMAT_PCM,
									m_fmt.nChannels,
									m_fmt.nSamplesPerSec * 1000, // value should be in millihertz
									m_fmt.wBitsPerSample,
									m_fmt.wBitsPerSample,
									speakers,
									SL_BYTEORDER_LITTLEENDIAN };
	SLDataSource audioSrc = {&loc_bufq, &format_pcm};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	SLDataSink audioSnk = {&loc_outmix, NULL};

	// create audio player
	const SLInterfaceID ids[2] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE , SL_IID_VOLUME};
	const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
	SLresult result = (*engineEngine)->CreateAudioPlayer(engineEngine, &(pTemp->slPlayerObject), &audioSrc, &audioSnk, 2, ids, req);
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to create audio player" );
		if ( pTemp->m_pBuffers[ 0 ] ) delete [] pTemp->m_pBuffers[ 0 ];
		if ( pTemp->m_pBuffers[ 1 ] ) delete [] pTemp->m_pBuffers[ 1 ];
		delete pTemp;
		m_pSoundData = 0;
		return;
	}

	// realize the player
	result = (*pTemp->slPlayerObject)->Realize(pTemp->slPlayerObject, SL_BOOLEAN_FALSE);
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to initialise audio player" );
		if ( pTemp->m_pBuffers[ 0 ] ) delete [] pTemp->m_pBuffers[ 0 ];
		if ( pTemp->m_pBuffers[ 1 ] ) delete [] pTemp->m_pBuffers[ 1 ];
		delete pTemp;
		m_pSoundData = 0;
		return;
	}

	// get the play interface
	result = (*pTemp->slPlayerObject)->GetInterface(pTemp->slPlayerObject, SL_IID_PLAY, &(pTemp->slPlayerPlay));
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to get audio player interface" );
		if ( pTemp->m_pBuffers[ 0 ] ) delete [] pTemp->m_pBuffers[ 0 ];
		if ( pTemp->m_pBuffers[ 1 ] ) delete [] pTemp->m_pBuffers[ 1 ];
		delete pTemp;
		m_pSoundData = 0;
		return;
	}

	/*
	result = (*pTemp->slPlayerPlay)->RegisterCallback(pTemp->slPlayerPlay, &SoundEventCallback, (void*)pSoundInst );
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to set sound callback" );
		return;
	}

	result = (*pTemp->slPlayerPlay)->SetCallbackEventsMask( pTemp->slPlayerPlay, SL_PLAYEVENT_HEADATEND );
	if ( result != SL_RESULT_SUCCESS )
	{
		agk::Warning( "Failed to set sound callback flags" );
		return;
	}
	*/

	// get the buffer queue interface
	result = (*pTemp->slPlayerObject)->GetInterface(pTemp->slPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE , &(pTemp->slPlayerBufferQueue));
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to get audio player buffer" );
		if ( pTemp->m_pBuffers[ 0 ] ) delete [] pTemp->m_pBuffers[ 0 ];
		if ( pTemp->m_pBuffers[ 1 ] ) delete [] pTemp->m_pBuffers[ 1 ];
		delete pTemp;
		m_pSoundData = 0;
		return;
	}

	float fVol = m_iVolume / 100.0f;
	
	// convert to centibels
	if ( fVol > 0.00001f ) fVol = 100.0f * 20.0f * log10(fVol);
	else fVol = -10000;

	SLVolumeItf volumeIf;
	(*pTemp->slPlayerObject)->GetInterface(pTemp->slPlayerObject, SL_IID_VOLUME, &volumeIf);
	(*volumeIf)->SetVolumeLevel(volumeIf, fVol);
}

void AGKMusicOGG::PlatformCleanUp()
{
	if ( m_pSoundData ) 
	{
		AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;

		if ( pTemp->slPlayerPlay ) (*pTemp->slPlayerPlay)->SetPlayState(pTemp->slPlayerPlay, SL_PLAYSTATE_STOPPED);
		if ( pTemp->slPlayerBufferQueue ) (*pTemp->slPlayerBufferQueue)->Clear( pTemp->slPlayerBufferQueue );
		if ( pTemp->slPlayerObject ) (*pTemp->slPlayerObject)->Destroy(pTemp->slPlayerObject);
		pTemp->slPlayerObject = 0;
		pTemp->slPlayerPlay = 0;
		pTemp->slPlayerBufferQueue = 0;

		if ( pTemp->m_pBuffers[ 0 ] ) delete [] pTemp->m_pBuffers[ 0 ];
		if ( pTemp->m_pBuffers[ 1 ] ) delete [] pTemp->m_pBuffers[ 1 ];

		delete pTemp;
		m_pSoundData = 0;
	}
}

int AGKMusicOGG::PlatformPlay()
{
	if ( !m_pSoundData ) return 0;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;

	SLresult result = (*pTemp->slPlayerPlay)->SetPlayState(pTemp->slPlayerPlay, SL_PLAYSTATE_PLAYING);
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to play audio" );
		return 0;
	}

	return 1;
}

void AGKMusicOGG::PlatformSetVolume()
{
	if ( !m_pSoundData ) return;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;

	float fVol = (m_iVolume * g_iMasterVolume) / 10000.0f;
	if ( fVol > 0.00001f ) fVol = 100.0f * 20.0f * log10(fVol);
	else fVol = -10000;

	SLVolumeItf volumeIf;
	(*pTemp->slPlayerObject)->GetInterface(pTemp->slPlayerObject, SL_IID_VOLUME, &volumeIf);
	(*volumeIf)->SetVolumeLevel(volumeIf, fVol);
}

void AGKMusicOGG::PlatformPause()
{
	if ( !m_pSoundData ) return;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;

	SLresult result = (*pTemp->slPlayerPlay)->SetPlayState(pTemp->slPlayerPlay, SL_PLAYSTATE_PAUSED);
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to pause audio" );
		return;
	}
}

void AGKMusicOGG::PlatformResume()
{
	if ( !m_pSoundData ) return;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;

	SLresult result = (*pTemp->slPlayerPlay)->SetPlayState(pTemp->slPlayerPlay, SL_PLAYSTATE_PLAYING);
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to resume audio" );
		return;
	}
}

void AGKMusicOGG::PlatformStop()
{
	if ( !m_pSoundData ) return;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;

	SLresult result = (*pTemp->slPlayerPlay)->SetPlayState(pTemp->slPlayerPlay, SL_PLAYSTATE_STOPPED);
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to stop audio" );
		return;
	}

	(*pTemp->slPlayerBufferQueue)->Clear( pTemp->slPlayerBufferQueue );

	pTemp->m_iResetFlag[ 0 ] = 0;
	pTemp->m_iResetFlag[ 1 ] = 0;
}

void AGKMusicOGG::PlatformClearBuffers()
{
	if ( !m_pSoundData ) return;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;
		
	SLresult result = (*pTemp->slPlayerPlay)->SetPlayState(pTemp->slPlayerPlay, SL_PLAYSTATE_STOPPED);
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to stop audio" );
		return;
	}

	(*pTemp->slPlayerBufferQueue)->Clear( pTemp->slPlayerBufferQueue );

	pTemp->m_iResetFlag[ 0 ] = 0;
	pTemp->m_iResetFlag[ 1 ] = 0;
}

void AGKMusicOGG::PlatformReset()
{
	if ( !m_pSoundData ) return;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;

	if ( pTemp->slPlayerPlay ) (*pTemp->slPlayerPlay)->SetPlayState(pTemp->slPlayerPlay, SL_PLAYSTATE_STOPPED);
	if ( pTemp->slPlayerBufferQueue ) (*pTemp->slPlayerBufferQueue)->Clear( pTemp->slPlayerBufferQueue );
	if ( pTemp->slPlayerObject ) (*pTemp->slPlayerObject)->Destroy(pTemp->slPlayerObject);
	pTemp->slPlayerObject = 0;
	pTemp->slPlayerPlay = 0;
	pTemp->slPlayerBufferQueue = 0;

	pTemp->m_iResetFlag[ 0 ] = 0;
	pTemp->m_iResetFlag[ 1 ] = 0;
}

int AGKMusicOGG::PlatformGetNumBuffers()
{
	if ( !m_pSoundData ) return 0;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;
		
	SLAndroidSimpleBufferQueueState state;
	(*(pTemp->slPlayerBufferQueue))->GetState( pTemp->slPlayerBufferQueue, &state );

	return state.count;
}

int AGKMusicOGG::PlatformGetMaxBuffers() { return 2; }

int AGKMusicOGG::PlatformAddBuffer( int *reset )
{
	if ( !m_pSoundData ) return 0;
	AGKOGGSoundData *pTemp = (AGKOGGSoundData*)m_pSoundData;
		
	int currBuf = pTemp->m_iCurrBuffer;
	if ( reset ) *reset = pTemp->m_iResetFlag[ currBuf ];
	memcpy( pTemp->m_pBuffers[currBuf], m_pDecodeBuffer, m_iBufferSize );
	pTemp->m_iResetFlag[ currBuf ] = 0;

	if ( m_iFlags & AGK_MUSIC_OGG_BUFFER_END ) pTemp->m_iResetFlag[ currBuf ] = 1;

	pTemp->m_iCurrBuffer++;
	if ( pTemp->m_iCurrBuffer >= PlatformGetMaxBuffers() ) pTemp->m_iCurrBuffer = 0;

	if ( !pTemp->slPlayerBufferQueue ) return 0;
	SLresult result = (*pTemp->slPlayerBufferQueue)->Enqueue(pTemp->slPlayerBufferQueue, pTemp->m_pBuffers[currBuf], m_iBufferSize);
	if ( result != SL_RESULT_SUCCESS)
	{
		agk::Warning( "Failed to add audio buffer" );
		return 0;
	}

	return 1;
}

// Sound

void cSoundMgr::PlatformInit()
{	
	SLresult result;

	// create engine
	if ( engineObject == 0 )
	{
		result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
		if ( result != SL_RESULT_SUCCESS)
		{
			agk::Warning( "Failed to create audio engine" );
			return;
		}

		// realize the engine
		result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
		if ( result != SL_RESULT_SUCCESS)
		{
			agk::Warning( "Failed to initialise audio engine" );
			return;
		}

		// get the engine interface, which is needed in order to create other objects
		result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
		if ( result != SL_RESULT_SUCCESS)
		{
			agk::Warning( "Failed to get audio engine interface" );
			return;
		}
	}

	if ( outputMixObject == 0 )
	{
		// create output mix, with environmental reverb specified as a non-required interface
		result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
		if ( result != SL_RESULT_SUCCESS)
		{
			agk::Warning( "Failed to create output mix" );
			return;
		}

		// realize the output mix
		result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
		if ( result != SL_RESULT_SUCCESS)
		{
			agk::Warning( "Failed to initialise output mix" );
			return;
		}
	}
	
	// create a temp player
	if ( pTempPlayer == 0 )
	{	
		// configure audio source
		SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
		SLDataFormat_PCM format_pcm = { SL_DATAFORMAT_PCM,
										1,
										16000 * 1000, // value should be in millihertz
										16,
										16,
										SL_SPEAKER_FRONT_LEFT,
										SL_BYTEORDER_LITTLEENDIAN };
		SLDataSource audioSrc = {&loc_bufq, &format_pcm};
	
		// configure audio sink
		SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
		SLDataSink audioSnk = {&loc_outmix, NULL};
	
		// create audio player
		const SLInterfaceID ids[3] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAYBACKRATE};
		const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
		result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pTempPlayer, &audioSrc, &audioSnk, 3, ids, req);
		if ( result != SL_RESULT_SUCCESS)
		{
			agk::Warning( "Failed to create temp audio player" );
			return;
		}
	
		// realize the player
		result = (*pTempPlayer)->Realize(pTempPlayer, SL_BOOLEAN_FALSE);
		if ( result != SL_RESULT_SUCCESS)
		{
			agk::Warning( "Failed to initialise temp audio player" );
			return;
		}
	
		// no longer play the temp player, was only used to force the volume buttons to set the media volume instead 
		// of notification volume, but Android now has an option to do this in the volume control
		// we still need it to work out the max/min payback rates though
		
		SLPlaybackRateItf rateIf;
		SLpermille min, max, step;
		SLuint32 caps;
		(*pTempPlayer)->GetInterface(pTempPlayer, SL_IID_PLAYBACKRATE, &rateIf);
		(*rateIf)->GetRateRange(rateIf, 0, &min, &max, &step, &caps);

		//uString info;
		//info.Format( "Min: %u, Max: %u, Step: %u, Caps: %u", min, max, step, caps );
		//agk::Warning( info );

		m_fMinPlaybackRate = min/1000.0f;
		m_fMaxPlaybackRate = max/1000.0f;
		m_fStepPlaybackRate = step/1000.0f;
		
		(*pTempPlayer)->Destroy( pTempPlayer );
		pTempPlayer = 0;
	}
}

void cSoundMgr::AppPaused()
{
    
}

void cSoundMgr::AppResumed()
{
    
}

void cSoundMgr::PlatformAddFile( cSoundFile *pSound )
{
	if ( pSound->m_uDataSize < 1000000 )
	{
		unsigned int msLength = (pSound->m_uDataSize * 1000) / pSound->m_fmt.nAvgBytesPerSec;

		// sounds shorter than 400ms may not play properly on some devices (popping, distortion, etc)
		if ( msLength < 400 )
		{
			// pad to 400ms
			unsigned int newLength = (pSound->m_fmt.nAvgBytesPerSec * 400) / 1000;
			unsigned char *newBuf = new unsigned char[ newLength ];
			memcpy( newBuf, pSound->m_pRawData, pSound->m_uDataSize );
			memset( newBuf+pSound->m_uDataSize, 0, newLength-pSound->m_uDataSize );

			delete [] pSound->m_pRawData;
			pSound->m_pRawData = newBuf;
			pSound->m_uDataSize = newLength;
		}
	}
}

void cSoundMgr::PlatformUpdate()
{
	// check music
	if ( g_bMusicFinished )
	{
		g_bMusicFinished = false;
		agk::m_cMusicMgr.HandleEvent();
	}
	
	cSoundInst *pSound = m_pSounds;
	cSoundInst *pNext = 0;
	while ( pSound )
	{
		pNext = pSound->m_pNextInst;
		
		if ( pSound->bqPlayerObject && pSound->m_bFinished )
		{
			if ( pSound->m_bFinished /*&& pSound->m_fFinishedTimer <= 0*/ )
			{
				if ( pSound->m_iLoop == 1 || pSound->m_iLoopCount+1 < pSound->m_iLoop )
				{
					if ( pSound->m_iParent == 0 || !m_pSoundFiles[ pSound->m_iParent ] ) pSound->m_iLoop = 0;
					else
					{
						pSound->m_iLoopCount++;
						SLresult result = (*pSound->bqPlayerBufferQueue)->Enqueue(pSound->bqPlayerBufferQueue, m_pSoundFiles[ pSound->m_iParent ]->m_pRawData, m_pSoundFiles[ pSound->m_iParent ]->m_uDataSize);
						if ( result != SL_RESULT_SUCCESS ) pSound->m_iLoop = 0;
						else 
						{
							pSound->m_bFinished = false;
						}
					}
				}

				if ( pSound->m_bFinished )
				{
					if ( m_pSoundFiles[ pSound->m_iParent ] ) m_pSoundFiles[ pSound->m_iParent ]->m_iInstances--;

					// sound is finished, cleanup and move to used list
					pSound->m_iLoopCount++;
					pSound->Reset();
					pSound->m_uLastUsed = agk::GetSeconds();
					if ( pSound->m_pPrevInst ) pSound->m_pPrevInst->m_pNextInst = pSound->m_pNextInst;
					else m_pSounds = pSound->m_pNextInst;

					if ( pSound->m_pNextInst ) pSound->m_pNextInst->m_pPrevInst = pSound->m_pPrevInst;

					m_cSoundInstances.RemoveItem( pSound->m_iID );

					// add to head of use list
					pSound->m_pPrevInst = 0;
					pSound->m_pNextInst = m_pUsedSounds;
					m_pUsedSounds = pSound;

					if ( pSound->m_pNextInst ) pSound->m_pNextInst->m_pPrevInst = pSound;
				}
			}
		}
		
		pSound = pNext;
	}
	
	int iSeconds = agk::GetSeconds();
	
	// check for used sounds that haven't been used in a while and delete them
	pSound = m_pUsedSounds;
	while ( pSound )
	{
		pNext = pSound->m_pNextInst;
		
		// delete after 10 seconds on the used list
		if ( iSeconds - pSound->m_uLastUsed > 10 )
		{
			if ( pSound->m_pPrevInst ) pSound->m_pPrevInst->m_pNextInst = pSound->m_pNextInst;
			else m_pUsedSounds = pSound->m_pNextInst;
			
			if ( pSound->m_pNextInst ) pSound->m_pNextInst->m_pPrevInst = pSound->m_pPrevInst;
			delete pSound;
		}
		
		pSound = pNext;
	}
}

void cSoundMgr::PlatformUpdateVolume()
{
	cSoundInst *pSound = m_pSounds;
	while ( pSound )
	{
		float fVol = m_iGlobalVolume / 100.0f;
		fVol *= ( pSound->m_iVolume / 100.0f );
		
		// convert to centibels
		if ( fVol > 0.00001f ) fVol = 100.0f * 20.0f * log10(fVol);
		else fVol = -10000;

		if ( pSound->bqPlayerObject )
		{
			SLVolumeItf volumeIf;
			(*pSound->bqPlayerObject)->GetInterface(pSound->bqPlayerObject, SL_IID_VOLUME, &volumeIf);
			(*volumeIf)->SetVolumeLevel(volumeIf, fVol);
		}

		pSound = pSound->m_pNextInst;
	}

	//SLmillibel maxVolume;
	//(*volumeIf_)->GetMaxVolumeLevel(volumeIf, &maxVolume);
}

void cSoundMgr::PlatformCleanUp()
{
	m_cSoundInstances.ClearAll();

	cSoundInst *pSound;
	while ( m_pSounds )
	{
		pSound = m_pSounds;
		m_pSounds = m_pSounds->m_pNextInst;
		delete pSound;
	}
	
	while ( m_pUsedSounds )
	{
		pSound = m_pUsedSounds;
		m_pUsedSounds = m_pUsedSounds->m_pNextInst;
		delete pSound;
	}

	if ( outputMixObject ) (*outputMixObject)->Destroy( outputMixObject );
	outputMixObject = 0;
	 
	if ( engineObject ) (*engineObject)->Destroy( engineObject );
	engineObject = 0;
	engineEngine = 0;
}

uint32_t cSoundMgr::PlatformCreateInstance( cSoundMgr::cSoundFile *pSound, int iVol, int iLoop, int iPriority )
{
	if ( !pSound ) return 0;
	if ( iVol < 0 ) iVol = 0;
	if ( iVol > 100 ) iVol = 100;
	
	cSoundInst *pSoundInst = 0;
	// find any used instances
	if ( m_pUsedSounds )
	{
		pSoundInst = m_pUsedSounds;
		m_pUsedSounds = m_pUsedSounds->m_pNextInst;
		if ( m_pUsedSounds ) m_pUsedSounds->m_pPrevInst = 0;
	}

	if ( iLoop < 0 ) iLoop = 0;
	
	// if no used sounds awaiting re-use, create a new one
	if ( !pSoundInst ) pSoundInst = new cSoundInst();
	pSoundInst->m_iID = m_cSoundInstances.GetFreeID();
	pSoundInst->m_iParent = pSound->m_iID;
	pSoundInst->m_iVolume = iVol;
	pSoundInst->m_iLoop = iLoop;
	pSoundInst->m_iLoopCount = 0;
	
	SLresult result;
	
	if ( pSoundInst->bqPlayerObject == 0 )
	{
		SLuint32 speakers = SL_SPEAKER_FRONT_CENTER;
		if ( pSound->m_fmt.nChannels > 1 ) speakers = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
	
		// configure audio source
		SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
		SLDataFormat_PCM format_pcm = { SL_DATAFORMAT_PCM,
										pSound->m_fmt.nChannels,
										pSound->m_fmt.nSamplesPerSec * 1000, // value should be in millihertz
										pSound->m_fmt.wBitsPerSample,
										pSound->m_fmt.wBitsPerSample,
										speakers,
										SL_BYTEORDER_LITTLEENDIAN };
		SLDataSource audioSrc = {&loc_bufq, &format_pcm};
	
		// configure audio sink
		SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
		SLDataSink audioSnk = {&loc_outmix, NULL};
	
		// create audio player
		const SLInterfaceID ids[3] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE , SL_IID_VOLUME, SL_IID_PLAYBACKRATE};
		const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
		result = (*engineEngine)->CreateAudioPlayer(engineEngine, &(pSoundInst->bqPlayerObject), &audioSrc, &audioSnk, 3, ids, req);
		if ( result != SL_RESULT_SUCCESS)
		{
			delete pSoundInst;
			agk::Warning( "Failed to create audio player" );
			return 0;
		}
	
		// realize the player
		result = (*pSoundInst->bqPlayerObject)->Realize(pSoundInst->bqPlayerObject, SL_BOOLEAN_FALSE);
		if ( result != SL_RESULT_SUCCESS)
		{
			delete pSoundInst;
			agk::Warning( "Failed to initialise audio player" );
			return 0;
		}
	
		// get the play interface
		result = (*pSoundInst->bqPlayerObject)->GetInterface(pSoundInst->bqPlayerObject, SL_IID_PLAY, &(pSoundInst->bqPlayerPlay));
		if ( result != SL_RESULT_SUCCESS)
		{
			delete pSoundInst;
			agk::Warning( "Failed to get audio player interface" );
			return 0;
		}

		// get playback rate interface
		result = (*pSoundInst->bqPlayerObject)->GetInterface(pSoundInst->bqPlayerObject, SL_IID_PLAYBACKRATE, &(pSoundInst->rateIf));
		if ( result != SL_RESULT_SUCCESS)
		{
			delete pSoundInst;
			agk::Warning( "Failed to get audio playback rate interface" );
			return 0;
		}
		
		result = (*pSoundInst->bqPlayerPlay)->RegisterCallback(pSoundInst->bqPlayerPlay, &SoundEventCallback, (void*)pSoundInst );
		if ( result != SL_RESULT_SUCCESS )
		{
			delete pSoundInst;
			agk::Warning( "Failed to set sound callback" );
			return 0;
		}

		result = (*pSoundInst->bqPlayerPlay)->SetCallbackEventsMask( pSoundInst->bqPlayerPlay, SL_PLAYEVENT_HEADATEND );
		if ( result != SL_RESULT_SUCCESS )
		{
			delete pSoundInst;
			agk::Warning( "Failed to set sound callback flags" );
			return 0;
		}
	
		// get the buffer queue interface
		result = (*pSoundInst->bqPlayerObject)->GetInterface(pSoundInst->bqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE , &(pSoundInst->bqPlayerBufferQueue));
		if ( result != SL_RESULT_SUCCESS)
		{
			delete pSoundInst;
			agk::Warning( "Failed to get audio player buffer" );
			return 0;
		}
	}

	// add it to the running list
	pSoundInst->m_pPrevInst = 0;
	pSoundInst->m_pNextInst = m_pSounds;
	m_pSounds = pSoundInst;
	if ( pSoundInst->m_pNextInst ) pSoundInst->m_pNextInst->m_pPrevInst = pSoundInst;

	m_cSoundInstances.AddItem( pSoundInst, pSoundInst->m_iID );

	pSoundInst->m_fFinishedTimer = 0;
	result = (*pSoundInst->bqPlayerBufferQueue)->Enqueue( pSoundInst->bqPlayerBufferQueue, pSound->m_pRawData, pSound->m_uDataSize );
	if ( result != SL_RESULT_SUCCESS)
	{
		pSoundInst->m_bFinished = true;
		agk::Warning( "Failed to queue audio buffer" );
		//return;
	}

	float fVol = m_iGlobalVolume / 100.0f;
	fVol *= ( pSoundInst->m_iVolume / 100.0f );

	// convert to centibels
	if ( fVol > 0.00001f ) fVol = 100.0f * 20.0f * log10(fVol);
	else fVol = -10000;

	SLVolumeItf volumeIf;
	(*pSoundInst->bqPlayerObject)->GetInterface(pSoundInst->bqPlayerObject, SL_IID_VOLUME, &volumeIf);
	(*volumeIf)->SetVolumeLevel(volumeIf, fVol);

	
	
	// set the player's state to playing
	result = (*pSoundInst->bqPlayerPlay)->SetPlayState(pSoundInst->bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	if ( result != SL_RESULT_SUCCESS)
	{
		pSoundInst->m_bFinished = true;
		agk::Warning( "Failed to play audio" );
		return 0;
	}
	
	pSound->m_iInstances++;

	return pSoundInst->m_iID;
}

void cSoundMgr::PlatformStopInstances( uint32_t iID )
{
	cSoundInst *pNext = 0;
	cSoundInst *pSound = m_pSounds;
	while ( pSound )
	{
		pNext = pSound->m_pNextInst;
		// do this even if the sound is finished
		//if ( !pSound->m_bFinished )
		{
			if ( iID == 0 || pSound->m_iParent == iID )
			{
				if ( m_pSoundFiles[ pSound->m_iParent ] ) m_pSoundFiles[ pSound->m_iParent ]->m_iInstances = 0;

				// stop sound, remove from playing list
				pSound->Reset();
				pSound->m_uLastUsed = agk::GetSeconds();
				if ( pSound->m_pPrevInst ) pSound->m_pPrevInst->m_pNextInst = pSound->m_pNextInst;
				else m_pSounds = pSound->m_pNextInst;

				m_cSoundInstances.RemoveItem( pSound->m_iID );
				
				if ( pSound->m_pNextInst ) pSound->m_pNextInst->m_pPrevInst = pSound->m_pPrevInst;
				
				// add to head of use list
				pSound->m_pPrevInst = 0;
				pSound->m_pNextInst = m_pUsedSounds;
				m_pUsedSounds = pSound;
				
				if ( pSound->m_pNextInst ) pSound->m_pNextInst->m_pPrevInst = pSound;
			}
		}
		
		pSound = pNext;
	}
}

void cSoundMgr::SetInstanceVolume( uint32_t instance, int vol )
{
	cSoundInst *pSound = m_cSoundInstances.GetItem( instance );
	if ( !pSound ) return;

	if ( vol < 0 ) vol = 0;
	if ( vol > 100 ) vol = 100;

	pSound->m_iVolume = vol;

	float fVol = m_iGlobalVolume / 100.0f;
	fVol *= ( pSound->m_iVolume / 100.0f );

	// convert to centibels
	if ( fVol > 0.00001f ) fVol = 100.0f * 20.0f * log10(fVol);
	else fVol = -10000;

	SLVolumeItf volumeIf;
	(*pSound->bqPlayerObject)->GetInterface(pSound->bqPlayerObject, SL_IID_VOLUME, &volumeIf);
	(*volumeIf)->SetVolumeLevel(volumeIf, fVol);
}

void cSoundMgr::SetInstanceRate( uint32_t instance, float rate )
{
	cSoundInst *pSound = m_cSoundInstances.GetItem( instance );
	if ( !pSound ) return;

	if ( rate < m_fMinPlaybackRate ) rate = m_fMinPlaybackRate;
	if ( rate > m_fMaxPlaybackRate ) rate = m_fMaxPlaybackRate;

	pSound->m_fRate = rate;

	(*pSound->rateIf)->SetRate( pSound->rateIf, pSound->m_fRate*1000 );
}

void cSoundMgr::SetInstanceBalance( uint32_t instance, float balance )
{
	cSoundInst *pSound = m_cSoundInstances.GetItem( instance );
	if ( !pSound ) return;

	if ( balance < -1 ) balance = -1;
	if ( balance > 1 ) balance = 1;

	pSound->m_fBalance = balance;

	SLVolumeItf volumeIf;
	(*pSound->bqPlayerObject)->GetInterface(pSound->bqPlayerObject, SL_IID_VOLUME, &volumeIf);
	(*volumeIf)->SetStereoPosition(volumeIf, pSound->m_fBalance*1000);
}

int cSoundMgr::GetInstanceVolume( uint32_t instance )
{
	cSoundInst *pSound = m_cSoundInstances.GetItem( instance );
	if ( !pSound ) return 0;
	return pSound->m_iVolume;
}

float cSoundMgr::GetInstanceRate( uint32_t instance )
{
	cSoundInst *pSound = m_cSoundInstances.GetItem( instance );
	if ( !pSound ) return 0;
	return pSound->m_fRate;
}

int cSoundMgr::GetInstancePlaying( uint32_t instance )
{
	cSoundInst *pSound = m_cSoundInstances.GetItem( instance );
	if ( !pSound ) return 0;
	else return 1;
}

int cSoundMgr::GetInstanceLoopCount( uint32_t instance )
{
	cSoundInst *pSound = m_cSoundInstances.GetItem( instance );
	if ( !pSound ) return 0;
	return pSound->m_iLoopCount;
}

void cSoundMgr::StopInstance( uint32_t instance )
{
	cSoundInst *pSound = m_cSoundInstances.GetItem( instance );
	if ( !pSound ) return;

	// stop sound, remove from playing list
	pSound->Reset();
	pSound->m_uLastUsed = agk::GetSeconds();
	if ( pSound->m_pPrevInst ) pSound->m_pPrevInst->m_pNextInst = pSound->m_pNextInst;
	else m_pSounds = pSound->m_pNextInst;

	m_cSoundInstances.RemoveItem( pSound->m_iID );
	
	if ( pSound->m_pNextInst ) pSound->m_pNextInst->m_pPrevInst = pSound->m_pPrevInst;
	
	// add to head of use list
	pSound->m_pPrevInst = 0;
	pSound->m_pNextInst = m_pUsedSounds;
	m_pUsedSounds = pSound;
	
	if ( pSound->m_pNextInst ) pSound->m_pNextInst->m_pPrevInst = pSound;
}

// youtube videos

void agk::PlayYoutubeVideo( const char* developerKey, const char* videoID, float startTime )
//****
{
	if ( !developerKey || !*developerKey ) return;
	if ( !videoID || !*videoID ) return;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/YoutubeSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "PlayYoutubeVideo","(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;I)V" );

	jstring sDeveloperKey = lJNIEnv->NewStringUTF(developerKey);
	jstring sVideoID = lJNIEnv->NewStringUTF(videoID);
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, sDeveloperKey, sVideoID, (int)(startTime*1000) );
	lJNIEnv->DeleteLocalRef( sVideoID );
	lJNIEnv->DeleteLocalRef( sDeveloperKey );

	vm->DetachCurrentThread();
}

// video commands 

int agk::LoadVideo( const char *szFilename )
//****
{
	int type = -1;

	uString sPath( szFilename );
	if ( strncmp( szFilename, "http://", 7 ) == 0 || strncmp( szFilename, "https://", 8 ) == 0 ) type = 3;
	else if ( strncmp( szFilename, "expansion:", 10 ) == 0 ) type = 2;
	else
	{
		type = 1;
		if ( cFile::ExistsRaw( szFilename ) ) sPath.SetStr( szFilename+4 );
		else if ( cFile::ExistsWrite( szFilename ) ) agk::PlatformGetFullPathWrite(sPath);
		else if ( cFile::ExistsRead( szFilename ) ) 
		{
			agk::PlatformGetFullPathRead(sPath);
			type = 0;
		}
		else type = -1;
	}

	// file not found?
	if ( type < 0 ) 
	{
		uString err;
		err.Format( "Failed to load video file, file not found: %s", szFilename );
		agk::Error( err );
		return 0;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID LoadVideo = lJNIEnv->GetStaticMethodID( sdkClass, "LoadVideo","(Landroid/app/Activity;Ljava/lang/String;I)V" );
	jstring videoPath = lJNIEnv->NewStringUTF(sPath.GetStr());
	lJNIEnv->CallStaticVoidMethod( sdkClass, LoadVideo, lNativeActivity, videoPath, type );
	lJNIEnv->DeleteLocalRef( videoPath );
	vm->DetachCurrentThread();

	return 1;
}

void agk::HandleVideoEvents()
{
	// do nothing on android
}

void agk::ChangeVideoPointer( void *ptr )
{
	
}

void agk::DeleteVideo()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "DeleteVideo","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Video, lNativeActivity );
	vm->DetachCurrentThread();

	if ( m_pVideoTextureFBO ) delete m_pVideoTextureFBO; m_pVideoTextureFBO = 0;
	if ( m_pVideoTextureQuad ) delete m_pVideoTextureQuad; m_pVideoTextureQuad = 0;
	if ( m_pVideoTextureShader ) delete m_pVideoTextureShader; m_pVideoTextureShader = 0;
	if ( m_iVideoTextureRaw ) 
	{ 
		GLESDeleteExternalTexture( &m_iVideoTextureRaw ); 
		m_iVideoTextureRaw = 0;
	}/*
	else if ( g_pVideoImageReader )
	{
		AImageReader_delete( g_pVideoImageReader );
		g_pVideoImageReader = 0;
	}*/
	else
	{
		// clear touch events in case the video captured some, they won't get released 
		cTouch::ClearAll();
	}

	if ( m_pVideoTexture ) 
	{
		m_cImageList.RemoveItem( m_pVideoTexture->m_iID );
		delete m_pVideoTexture; 
		m_pVideoTexture = 0;
	}
}

void agk::SetVideoDimensions( float x, float y, float width, float height )
//****
{
	m_fVideoX = x;
	m_fVideoY = y;
	m_fVideoWidth = width;
	m_fVideoHeight = height;

	int iX = agk::ScreenToDeviceX( x );
	int iY = agk::ScreenToDeviceY( y );
	int iWidth = agk::ScreenToDeviceX( x+width ) - iX;
	int iHeight = agk::ScreenToDeviceY( y+height ) - iY;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "SetVideoDimensions","(Landroid/app/Activity;IIII)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Video, lNativeActivity, iX, iY, iWidth, iHeight );
	vm->DetachCurrentThread();
}

void agk::VideoUpdate()
{
	if ( !m_iVideoTextureRaw /*&& !g_pVideoImageReader*/ ) return;
	if ( !m_pVideoTexture ) return;
	if ( !m_pVideoTextureQuad ) return;
	if ( !m_pVideoTextureFBO ) return;

	if ( m_iVideoTextureRaw )
	{
		// update video texture
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);
		jobject lNativeActivity = g_pActivity->clazz;

		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "UpdateVideo","()V" );
		lJNIEnv->CallStaticVoidMethod( sdkClass, Video );

		Video = lJNIEnv->GetStaticMethodID( sdkClass, "GetVideoTextureValue","(Landroid/app/Activity;I)F" );
		float u1 = lJNIEnv->CallStaticFloatMethod( sdkClass, Video, lNativeActivity, 1 );
		float v1 = lJNIEnv->CallStaticFloatMethod( sdkClass, Video, lNativeActivity, 2 );
		float u2 = lJNIEnv->CallStaticFloatMethod( sdkClass, Video, lNativeActivity, 3 );
		float v2 = lJNIEnv->CallStaticFloatMethod( sdkClass, Video, lNativeActivity, 4 );

		vm->DetachCurrentThread();

		m_pVideoTextureShader->SetConstantByName( "uvBounds", u2-u1, v2-v1, u1, v1 );

		// copy it to a normal image
		AGKFrameBuffer *pOldFBO = AGKFrameBuffer::g_pCurrFrameBuffer;
		AGKFrameBuffer::BindFrameBuffer( m_pVideoTextureFBO );
		g_pRenderer->SetViewport( 0, 0, m_pVideoTexture->GetWidth(), m_pVideoTexture->GetHeight() );
		g_pRenderer->ClearRenderTarget( 0,0,0,0 );

		g_pRenderer->SetViewport( 0, 0, m_pVideoTexture->GetWidth(), m_pVideoTexture->GetHeight() ); // if we don't do this again then Android seems to revert to the backbuffer viewport sizes

		// draw image to new image
		GLESBindExternalTexture( m_iVideoTextureRaw );
		m_pVideoTextureQuad->Draw();
    
		// restore old frame buffer
		AGKFrameBuffer::BindFrameBuffer( pOldFBO );
		if( pOldFBO ) g_pRenderer->SetViewport( 0, 0, pOldFBO->GetWidth(), pOldFBO->GetHeight() );
		else g_pRenderer->SetViewport( 0, 0, agk::m_iRenderWidth, agk::m_iRenderHeight );
	}
	/*
	if ( g_pVideoImageReader )
	{
		AImage *latestImage = 0;
		media_status_t status = AImageReader_acquireLatestImage( g_pVideoImageReader, &latestImage );
		if ( status != AMEDIA_OK ) return;

		AHardwareBuffer *latestBuffer = 0;
		status = AImage_getHardwareBuffer( latestImage, &latestBuffer );
		if ( status != AMEDIA_OK )
		{
			AImage_delete( latestImage );
			return;
		}

		

		AImage_delete( latestImage );
	}*/
}

void agk::PlayVideoToImage( uint32_t imageID )
//****
{
	int count = 0;
	while ( GetVideoWidth() == 0 && count < 130 )
	{
		agk::Sleep( 30 );
		count++;
	}

	if ( GetVideoWidth() <= 0 || GetVideoHeight() <= 0 )
	{
		agk::Error( "Failed to play video to image, could not get video width or height" );
		return;
	}

	cImage *pImage = m_cImageList.GetItem( imageID );
	if ( pImage && pImage != m_pVideoTexture )
	{
		agk::Error( "Failed to play video to image, image already exists" );
		return;
	}

	// create video image if necessary
	if ( m_pVideoTexture ) 
	{
		if ( m_pVideoTexture->m_iID != imageID ) agk::Warning( "Cannot change video image ID during playback, call DeleteVideo first then PlayVideoToImage with the new ID" );
	}
	else
	{
		m_pVideoTexture = new cImage();
		m_pVideoTexture->CreateBlankImage( GetVideoWidth(), GetVideoHeight(), 0, 0 );
		m_pVideoTexture->m_iID = imageID;
		m_cImageList.AddItem( m_pVideoTexture, imageID );
	} 

	// create quad and shader if necessary
	if ( !m_pVideoTextureShader )
	{
		m_pVideoTextureShader = g_pRenderer->LoadDefaultShaderVideo();
		m_pVideoTextureShader->SetConstantByName( "uvBounds", 1,1, 0,0 );
	}

	if ( !m_pVideoTextureQuad ) 
	{
		m_pVideoTextureQuad = new cObject3D();
		m_pVideoTextureQuad->CreateQuad();
		m_pVideoTextureQuad->SetCullMode(1);
        m_pVideoTextureQuad->SetDepthReadMode(7);
		m_pVideoTextureQuad->SetShader( m_pVideoTextureShader );
	}

	if ( !m_pVideoTextureFBO )
	{
		m_pVideoTextureFBO = new AGKFrameBuffer( m_pVideoTexture, true, 0, true, false );
	}

	if ( g_pRenderer->GetIdentifier() == AGK_RENDERER_OPENGL_ES2 )
	{
		if ( !m_iVideoTextureRaw )
		{
			GLESRegenerateExternalTexture( &m_iVideoTextureRaw );
		}

		// send the texture to Android
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "PlayVideoToTexture","(Landroid/app/Activity;I)V" );
		lJNIEnv->CallStaticVoidMethod( sdkClass, Video, lNativeActivity, m_iVideoTextureRaw );
		vm->DetachCurrentThread();
	}/*
	else if ( g_pRenderer->GetIdentifier() == AGK_RENDERER_VULKAN )
	{
		if ( !g_pVideoImageReader )
		{
			int width = 1024;
			int height = 1024;
			AImageReader_newWithUsage( width, height, AIMAGE_FORMAT_PRIVATE, AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE, 3, &g_pVideoImageReader );
		}
	}*/
	else
	{
		agk::Error( "PlayVideoToImage is only supported with OpenGLES, please change the InitGraphics call to use AGK_RENDERER_MODE_ONLY_LOWEST" );
		return;
	}

	m_iVideoPlayMode = 2;
}

void agk::PlayVideo()
//****
{
	m_iVideoPlayMode = 1;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "PlayVideo","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Video, lNativeActivity );
	vm->DetachCurrentThread();
}

void agk::PauseVideo()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "PauseVideo","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Video, lNativeActivity );
	vm->DetachCurrentThread();
}

void agk::StopVideo()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "StopVideo","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Video, lNativeActivity );
	vm->DetachCurrentThread();

	m_iVideoPlayMode = 0;
}

int agk::GetVideoPlaying()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "GetVideoPlaying","(Landroid/app/Activity;)I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, Video, lNativeActivity );
	vm->DetachCurrentThread();

	return result;
}

float agk::GetVideoPosition()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "GetVideoValue","(Landroid/app/Activity;I)F" );
	float result = lJNIEnv->CallStaticFloatMethod( sdkClass, Video, lNativeActivity, 1 );
	vm->DetachCurrentThread();

	return result;
}

float agk::GetVideoDuration()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "GetVideoValue","(Landroid/app/Activity;I)F" );
	float result = lJNIEnv->CallStaticFloatMethod( sdkClass, Video, lNativeActivity, 2 );
	vm->DetachCurrentThread();

	return result;
}

void agk::SetVideoVolume( float volume )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "SetVideoVolume","(F)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Video, volume );
	vm->DetachCurrentThread();
}

float agk::GetVideoWidth()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "GetVideoValue","(Landroid/app/Activity;I)F" );
	float result = lJNIEnv->CallStaticFloatMethod( sdkClass, Video, lNativeActivity, 3 );
	vm->DetachCurrentThread();

	return result;
}

float agk::GetVideoHeight()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "GetVideoValue","(Landroid/app/Activity;I)F" );
	float result = lJNIEnv->CallStaticFloatMethod( sdkClass, Video, lNativeActivity, 4 );
	vm->DetachCurrentThread();

	return result;
}

void agk::SetVideoPosition( float seconds )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PlayVideoSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID Video = lJNIEnv->GetStaticMethodID( sdkClass, "SetVideoPosition","(Landroid/app/Activity;F)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, Video, lNativeActivity, seconds );
	vm->DetachCurrentThread();
}

// Screen recording

ANativeWindow* g_pRecordWindow = 0;

void agk::StartScreenRecording( const char *szFilename, int microphone )
//****
{
	uString sPath;
	if ( strncmp(szFilename, "raw:", 4) == 0 ) sPath.SetStr( szFilename+4 );
	else
	{
		sPath.SetStr( szFilename );
		agk::PlatformGetFullPathWrite(sPath);
	}

	if ( !agk::PlatformCreateRawPath( sPath ) ) 
	{
		uString err = "Failed to create path for file ";
		err += sPath.GetStr();
		agk::Error( err );
		return;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/RecordScreenSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// setup screen recorder
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "StartScreenRecording","(Landroid/app/Activity;Ljava/lang/String;I)I" );

	jstring sfilename = lJNIEnv->NewStringUTF(sPath.GetStr());
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method, lNativeActivity, sfilename, microphone );
	lJNIEnv->DeleteLocalRef( sfilename );

	if ( result != 1 ) return;

	// get recording surface
	method = lJNIEnv->GetStaticMethodID( sdkClass, "GetScreenRecordSurface","()Landroid/view/Surface;" );
	jobject javaSurface = lJNIEnv->CallStaticObjectMethod( sdkClass, method );
	g_pRecordWindow = ANativeWindow_fromSurface( lJNIEnv, javaSurface );

	vm->DetachCurrentThread();

	g_pRenderer->SetScreenRecordParams( g_pRecordWindow, 0 );
}

void agk::StopScreenRecording()
//****
{
	// switch rendering to main surface
	if ( g_pRecordWindow != 0 )
	{
		g_pRenderer->SetScreenRecordParams( 0, 0 );

		ANativeWindow_release( g_pRecordWindow );
		g_pRecordWindow = 0;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/RecordScreenSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// stop recording
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "StopScreenRecording","()V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method );
		
	vm->DetachCurrentThread();
}

int agk::IsScreenRecording()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/RecordScreenSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "IsScreenRecording","()I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method );
	
	vm->DetachCurrentThread();

	return result;
}

// Smart Watch

void agk::ActivateSmartWatch( const char *szReserved )
//****
{
    
}

int agk::GetSmartWatchState()
//****
{
    return -1;
}

void agk::SendSmartWatchData( const char* szJson )
//****
{
    
}

char* agk::ReceiveSmartWatchData()
//****
{
    char* str = new char[1]; *str = 0;
    return str;
}

// Text to speech

void agk::TextToSpeechSetup()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "TextToSpeechSetup","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity );
	
	vm->DetachCurrentThread();
}

int agk::GetTextToSpeechReady()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "GetTextToSpeechReady","()I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method );
	
	vm->DetachCurrentThread();

	return result;
}

void agk::Speak( const char *text )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "Speak","(Landroid/app/Activity;Ljava/lang/String;II)V" );

	jstring sText = lJNIEnv->NewStringUTF(text);
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, sText, 0, 0 );
	lJNIEnv->DeleteLocalRef( sText );

	vm->DetachCurrentThread();
}

void agk::Speak( const char *text, int delay )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "Speak","(Landroid/app/Activity;Ljava/lang/String;II)V" );

	jstring sText = lJNIEnv->NewStringUTF(text);
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, sText, 0, delay );
	lJNIEnv->DeleteLocalRef( sText );

	vm->DetachCurrentThread();
}

void agk::SetSpeechRate( float rate )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "SetSpeechRate","(Landroid/app/Activity;F)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, rate );

	vm->DetachCurrentThread();
}

int agk::GetSpeechNumVoices()
//****
{
    JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "GetSpeechNumVoices","(Landroid/app/Activity;)I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method, lNativeActivity );

	vm->DetachCurrentThread();
	return result;
}

char* agk::GetSpeechVoiceLanguage( int index )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "GetSpeechVoiceLanguage","(Landroid/app/Activity;I)Ljava/lang/String;" );
	
	jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, lNativeActivity, index );
	jboolean bCopy;
	const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

	char *retstr = new char[ strlen(str2) + 1 ];
	strcpy( retstr, str2 );
		
	lJNIEnv->ReleaseStringUTFChars( str, str2 );
	lJNIEnv->DeleteLocalRef( str );

	vm->DetachCurrentThread();
	return retstr;
}

char* agk::GetSpeechVoiceName( int index )
//****
{
    JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "GetSpeechVoiceName","(Landroid/app/Activity;I)Ljava/lang/String;" );
	
	jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, lNativeActivity, index );
	jboolean bCopy;
	const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

	char *retstr = new char[ strlen(str2) + 1 ];
	strcpy( retstr, str2 );
		
	lJNIEnv->ReleaseStringUTFChars( str, str2 );
	lJNIEnv->DeleteLocalRef( str );

	vm->DetachCurrentThread();
	return retstr;
}

char* agk::GetSpeechVoiceID( int index )
//****
{
    char *str = new char[20];
	sprintf( str, "%d", index );
    return str;
}

void agk::SetSpeechLanguage( const char* lang )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "SetSpeechLanguage","(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring sLang = lJNIEnv->NewStringUTF(lang);
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, sLang );
	lJNIEnv->DeleteLocalRef( sLang );

	vm->DetachCurrentThread();
}

void agk::SetSpeechLanguageByID( const char* szID )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "SetSpeechLanguageByID","(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring sID = lJNIEnv->NewStringUTF(szID);
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, sID );
	lJNIEnv->DeleteLocalRef( sID );

	vm->DetachCurrentThread();
}

int agk::IsSpeaking()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "IsSpeaking","()I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method );
	
	vm->DetachCurrentThread();

	return result;
}

void agk::StopSpeaking()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextToSpeechSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "StopSpeaking","()V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method );
	
	vm->DetachCurrentThread();
}

// String

int uString::ToInt() const
{
	if ( !m_pData || !*m_pData ) return 0;
	return atoi(m_pData);
}

float uString::ToFloat() const
{
	if ( !m_pData || !*m_pData ) return 0;
	return (float)atof(m_pData);
}

void agk::PlatformReportError( const uString &sMsg )
{
//	printf("%s\n", sMsg.GetStr());
	//__android_log_print( ANDROID_LOG_WARN, "native-activity", sMsg.GetStr() );
	__android_log_print( ANDROID_LOG_ERROR, "native-activity", "%s", sMsg.GetStr() );
}

void agk::PlatformMessage( const char* msg )
{
	jint lFlags = 0;
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	if ( !g_pActivity ) agk::Warning("Failed to get activity pointer");
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	jclass classActivity = lJNIEnv->FindClass("android/app/NativeActivity");
	if ( !classActivity ) agk::Warning("Failed to get class NativeActivity");
	
	jmethodID getClassLoader = lJNIEnv->GetMethodID(classActivity, "getClassLoader", "()Ljava/lang/ClassLoader;");
	if( !getClassLoader ) agk::Warning("Exception occurred while getting getClassLoader methodId");

	jobject objClassLoader = lJNIEnv->CallObjectMethod(lNativeActivity, getClassLoader);
	if ( !objClassLoader ) agk::Warning("Exception occurred while getting class loader instance");

	jclass classLoader = lJNIEnv->FindClass("java/lang/ClassLoader");
	if ( !classLoader ) agk::Warning("Exception occurred while finding ClassLoader class definition");
		
	jmethodID methodFindClass = lJNIEnv->GetMethodID( classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if( !methodFindClass ) agk::Warning("Exception occurred while getting loadClass method id");
	
	// Get AGK Helper class
	jstring classname = lJNIEnv->NewStringUTF("com.thegamecreators.agk_player.AGKHelper");
	jclass classHelper = (jclass)lJNIEnv->CallObjectMethod(objClassLoader, methodFindClass, classname);
	if( !classHelper ) agk::Warning("Failed to get AGKHelper class");
	lJNIEnv->DeleteLocalRef( classname );
	
	jmethodID methodShowMessage = lJNIEnv->GetStaticMethodID( classHelper, "ShowMessage","(Landroid/app/Activity;Ljava/lang/String;)V");
	if ( !methodShowMessage ) agk::Warning("Failed to get method ShowMessage");
	
	jstring javamsg = lJNIEnv->NewStringUTF(msg);
	lJNIEnv->CallStaticVoidMethod( classHelper, methodShowMessage, lNativeActivity, javamsg );
	lJNIEnv->DeleteLocalRef( javamsg );
	
	vm->DetachCurrentThread();
}

// Thread functions

void AGKThread::PlatformInit( )
{
	pThread = new pthread_t; *((pthread_t*)pThread) = 0;
	m_pStop = new pthread_cond_t;
	
	pthread_cond_init( (pthread_cond_t*)m_pStop, NULL );
}

/*
uint32_t STDCALL StartNewThread( void* param )
{
	uint32_t result = ((AGKThread*)param)->EntryPoint( param );
	return result;
}
*/

void AGKThread::PlatformStart( )
{
	if ( *((pthread_t*)pThread) ) pthread_detach( *((pthread_t*)pThread) );
	*((pthread_t*)pThread) = 0;
	int result = pthread_create( (pthread_t*)pThread, NULL, (void*(*)(void*))EntryPoint, this );
	if ( result != 0 ) agk::Warning( "Failed to start pthread" );
}

void AGKThread::PlatformStop( )
{
	pthread_cond_signal( (pthread_cond_t*)m_pStop );
}

void AGKThread::PlatformTerminate( )
{
	if ( m_bRunning ) 
	{
		agk::Warning("Forcing a thread to terminate, this may cause a crash...");
		//pthread_cancel(*pThread);
	}
}

void AGKThread::PlatformCleanUp( )
{
	if ( m_pStop ) 
	{
		pthread_cond_destroy( (pthread_cond_t*)m_pStop );
		delete ((pthread_cond_t*)m_pStop);
	}
	m_pStop = 0;
	
	if ( pThread )
	{
		if ( *((pthread_t*)pThread) ) pthread_detach( *((pthread_t*)pThread) );
		delete (pthread_t*)pThread;
		pThread = 0;
	}
}

void AGKThread::PlatformJoin( )
{
	if ( !pThread || !*((pthread_t*)pThread) ) return; 
	pthread_join( *((pthread_t*)pThread), NULL );
	*((pthread_t*)pThread) = 0;
}

void AGKThread::PlatformSleepSafe( uint32_t milliseconds )
{
	if ( m_bTerminate ) return;
	pthread_mutex_t mutex;
	pthread_mutex_init( &mutex, NULL );
	pthread_mutex_lock( &mutex );
	timespec waittime;
	clock_gettime(CLOCK_REALTIME, &waittime);
	waittime.tv_sec += milliseconds / 1000;
	waittime.tv_nsec += (milliseconds%1000) * 1000000;
	pthread_cond_timedwait( (pthread_cond_t*)m_pStop, &mutex, &waittime );
	pthread_mutex_unlock( &mutex );
	pthread_mutex_destroy( &mutex );
}

// filesystem commands

char* agk::GetWritePath()
{
	// allow write directory to be determined
	char *szStr = new char[ strlen(szWriteDir)+1 ];
	strcpy( szStr, szWriteDir );
	return szStr;
	//return 0;
}

char* agk::GetReadPath()
{
	char *szStr = new char[ strlen(szRootDir)+1 ];
	strcpy( szStr, szRootDir );
	return szStr;
}

char* agk::GetDocumentsPath()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID getExternalDir = lJNIEnv->GetStaticMethodID( AGKHelper, "GetExternalDir","()Ljava/lang/String;" );

	jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, getExternalDir );
	jboolean bCopy;
	const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

	char *retstr = new char[ strlen(str2) + 1 ];
	strcpy( retstr, str2 );
		
	lJNIEnv->ReleaseStringUTFChars( str, str2 );
	lJNIEnv->DeleteLocalRef( str );

	vm->DetachCurrentThread();

	return retstr;
}

bool agk::PlatformChooseFile( uString &out, const char *ext, int fullPath )
{
	// do nothing on android
	return false;
}

void agk::PlatformGetFullPathWrite( uString &inout )
{
	inout.Replace( '\\', '/' );
	if ( inout.FindStr( "../" ) >= 0 ) 
	{
		uString sub;
		inout.SubString( sub, inout.RevFind( '/' )+1 );
		inout.SetStr( sub );
	}

	if ( inout.CharAt( 0 ) == '/' ) 
	{
		uString sub;
		inout.SubString( sub, 1 );
		inout.SetStr( sub );
		inout.Prepend( szWriteDir );
	}
	else 
	{
		inout.Prepend( m_sCurrentDir );
		inout.Prepend( szWriteDir );
	}
	
	cFileEntry::ConvertToReal( inout );
}

void agk::PlatformGetFullPathRead( uString &inout, int mode )
{
	inout.Replace( '\\', '/' );
	if ( inout.FindStr( "../" ) >= 0 ) 
	{
		uString sub;
		inout.SubString( sub, inout.RevFind( '/' )+1 );
		inout.SetStr( sub );
	}

	if ( inout.CharAt( 0 ) == '/' ) 
	{
		uString sub;
		inout.SubString( sub, 1 );
		inout.SetStr( sub );
		inout.Prepend( szRootDir );
	}
	else 
	{
		inout.Prepend( m_sCurrentDir );
		inout.Prepend( szRootDir );
	}
	
	cFileEntry::ConvertToReal( inout );
}

int agk::PlatformCreateRawPath( const char* path )
{
	if ( !path || !*path ) return 0;
	if ( path[0] != '/' ) 
	{
		uString err; err.Format( "Invalid path \"%s\", must be absolute", path );
		agk::Error( err );
		return 0;
	}

	uString sPath( path );
	sPath.Replace( '\\', '/' );
	sPath.Trunc( '/' );
	if ( sPath.GetLength() == 0 ) sPath.SetStr( "/" );

	int fd = open( sPath.GetStr(), O_RDONLY | O_CLOEXEC );
	if ( fd >= 0 ) 
	{
		close( fd );
		return 1; // already exists
	}

	int found = 0;
	do
	{
		sPath.Trunc( '/' );
		if ( sPath.GetLength() == 0 ) sPath.SetStr( "/" );
		fd = open( sPath.GetStr(), O_RDONLY | O_CLOEXEC );
		if ( fd >= 0 ) found = 1;
	} while( sPath.GetLength() > 1 && !found );
	
	if ( !found )
	{
		uString err; err.Format( "Failed to create path \"%s\", the app may not have permissions to create folders in the part that exists", path );
		agk::Error( err );
		return 0;
	}

	uString sPath2( path );
	sPath2.Replace( '\\', '/' );
	const char *szRemaining = sPath2.GetStr() + sPath.GetLength() + 1;
	const char *szSlash;
	char szFolder[ MAX_PATH ];
	while ( (szSlash = strchr( szRemaining, '/' )) )
	{
		uint32_t length = (uint32_t)(szSlash-szRemaining);
		if ( length == 0 )
		{
			uString err; err.Format( "Invalid path \"%s\", folder names must have at least one character", path );
			agk::Error( err );
			return 0;
		}
		
		strncpy( szFolder, szRemaining, length );
		szFolder[ length ] = '\0';

		int newFd = openat( fd, szFolder, O_RDONLY | O_CLOEXEC );
		if ( newFd < 0 )
		{
			mkdirat( fd, szFolder, 0777 );
			newFd = openat( fd, szFolder, O_RDONLY | O_CLOEXEC );
			if ( newFd < 0 )
			{
				uString err; err.Format( "Failed to create folder \"%s\" in path \"%s\", the app may not have permission to create it", szFolder, path );
				agk::Error( err );
				return 0;
			}
		}

		close( fd );
		fd = newFd;

		szRemaining = szSlash+1;
	}

	close( fd );

	return 1;
}

int cFolder::OpenFolder( const char* szPath )
{
	if ( m_pFiles ) delete [] m_pFiles;
	m_pFiles = 0;
	m_iNumFiles = 0;

	if ( m_pFolders ) delete [] m_pFolders;
	m_pFolders = 0;
	m_iNumFolders = 0;

	if ( strncmp( szPath, "raw:", 4 ) == 0 )
	{
		szPath += 4;
	}

	if ( !agk::IsAbsolutePath( szPath ) )
	{
		uString err; err.Format( "Failed to open folder \"%s\", it must be an absolute path", szPath );
		agk::Error( err );
		return 0;
	}

	DIR *pDir = opendir( szPath );
	if ( !pDir )
	{
		uString err; err.Format( "Failed to open folder \"%s\", it may not exist or this app doesn't have permission to accesss it", szPath );
		agk::Error( err );
		return 0;
	}

	dirent* item = readdir( pDir );
	while( item )
	{
		if ( item->d_type == DT_DIR )
		{
			if ( strcmp( item->d_name, "." ) != 0 && strcmp( item->d_name, ".." ) != 0 )
			{
				m_iNumFolders++;
			}
		}
		else if ( item->d_type == DT_REG )
		{
			m_iNumFiles++;
		}

		item = readdir( pDir );
	}
	closedir( pDir );

	m_pFiles = new uString[ m_iNumFiles ];
	m_pFolders = new uString[ m_iNumFolders ];

	int fileCount = 0;
	int folderCount = 0;

	pDir = opendir( szPath );
	item = readdir( pDir );
	while( item )
	{
		if ( item->d_type == DT_DIR )
		{
			// directory
			if ( strcmp( item->d_name, "." ) != 0 && strcmp( item->d_name, ".." ) != 0 )
			{
				m_pFolders[ folderCount++ ].SetStr( item->d_name );
			}
		}
		else if ( item->d_type == DT_REG )
		{
			// file
			m_pFiles[ fileCount++ ].SetStr( item->d_name );
		}

		item = readdir( pDir );
	}
	closedir( pDir );

	return 1;
}

#define ANDROID_EOF_FLAG 0x40000000

bool AGK::cFile::ExistsWrite( const char *szFilename )
{
	if ( !szFilename || !*szFilename ) return false;
	if ( strncmp(szFilename, "raw:", 4) == 0 ) return false;

	if ( strchr( szFilename, ':' ) ) return false;
	if ( strstr(szFilename, "..\\") || strstr(szFilename, "../") ) return false;
	
	uint32_t length = strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;
	
	uString sPath( szFilename );
	agk::PlatformGetFullPathWrite( sPath );
	
	//struct stat buf;
	//if ( stat( sPath.GetStr(), &buf ) != 0 ) return false;
	
	FILE *pFile = AGKfopen( sPath.GetStr(), "rb" );
	if ( !pFile ) return false;
	fclose( pFile );

	return true;
}

bool AGK::cFile::ExistsRead( const char *szFilename, int *mode )
{
	if ( !szFilename || !*szFilename ) return false;
	if ( strchr( szFilename, ':' ) ) return false;
	if ( strncmp(szFilename, "raw:", 4) == 0 ) return false;

	if ( strstr(szFilename, "..\\") || strstr(szFilename, "../") ) return false;
	
	uint32_t length = strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;
	
	uString sPath( szFilename );
	if ( !g_pActivity ) return false;

	agk::PlatformGetFullPathRead( sPath );
	
	AAssetManager* assetManager = g_pActivity->assetManager;
	if ( assetManager )
	{
		AAsset* asset = AAssetManager_open(assetManager, sPath.GetStr(), AASSET_MODE_UNKNOWN);
		if ( asset )
		{
			AAsset_close( asset );
			return true;
		}
	}
		
	return false;
}

bool cFile::ExistsRaw( const char *szFilename )
{
	if ( !szFilename || !*szFilename ) return false;
	uint32_t length = strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;

	if ( strncmp(szFilename, "raw:", 4) != 0 ) return false;
	if ( !agk::IsAbsolutePath( szFilename ) ) return false;
	
	// absolute path to anywhere allowed
	//struct stat buf;
	//if ( stat( szFilename+4, &buf ) != 0 ) return false;

	FILE *pFile = AGKfopen( szFilename+4, "rb" );
	if ( !pFile ) return false;
	fclose( pFile );

	return true;
}

bool AGK::cFile::Exists( const char *szFilename )
{
	if ( !szFilename || !*szFilename ) return false;

	if ( strncmp(szFilename, "expansion:", strlen("expansion:")) == 0 )
	{
		uString sPath;
		sPath.SetStr( szFilename + strlen("expansion") );
		sPath.Replace( ':', '/' );
		if ( ExistsWrite( sPath ) ) return true;

		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass AGKHelper = GetAGKHelper(lJNIEnv);

		jmethodID GetExists = lJNIEnv->GetStaticMethodID( AGKHelper, "GetExpansionFileExists", "(Landroid/app/Activity;Ljava/lang/String;)I" );

		jstring text = lJNIEnv->NewStringUTF(szFilename);
		int result = lJNIEnv->CallStaticIntMethod( AGKHelper, GetExists, lNativeActivity, text );
		lJNIEnv->DeleteLocalRef( text );

		vm->DetachCurrentThread();

		return result ? true : false;
	}
	else
	{
		if ( !ExistsRaw( szFilename ) )
		{
			if ( !ExistsWrite( szFilename ) )
			{
				if ( !ExistsRead( szFilename ) ) return false;
			}
		}

		return true;
	}
}

bool AGK::cFile::GetModified( const char *szFilename, int &time )
{
	int64_t time64 = 0;
	bool result = GetModified64( szFilename, time64 );
	time = (int) time64;
	return result;
}

bool AGK::cFile::GetModified64( const char *szFilename, int64_t &time )
{
	time = 0;
	if ( !szFilename || !*szFilename ) return false;
	
	uint32_t length = strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return false;
	
	uString sPath( szFilename );
	if ( !agk::GetRealPath( sPath ) ) return false;
	
	struct stat fileInfo;
	int result = stat( sPath, &fileInfo );
	if ( result != 0 )
	{
		return false;
	}
	
	time = (int64_t) fileInfo.st_mtime;
	return true;
}

void AGK::cFile::SetModified( const char *szFilename, int time )
{
	if ( !szFilename || !*szFilename ) return;
	
	uint32_t length = strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' ) return;
	
	uString sPath( szFilename );
	if ( cFile::ExistsRaw( szFilename ) ) sPath.SetStr( szFilename+4 );
	else if ( cFile::ExistsWrite( szFilename ) ) agk::PlatformGetFullPathWrite(sPath);
	else return;
	
	struct utimbuf fileInfo;
	fileInfo.actime = time;
	fileInfo.modtime = time;
	utime( sPath.GetStr(), &fileInfo );
}

uint32_t AGK::cFile::GetFileSize( const char *szFilename )
{
	cFile pFile;
	if ( !pFile.OpenToRead( szFilename ) ) return 0;
	uint32_t size = pFile.GetSize();
	pFile.Close();
	
	return size;
}

void AGK::cFile::DeleteFile( const char *szFilename )
{
	if ( !szFilename || !*szFilename ) return;
	
	uint32_t length = strlen(szFilename);
	if ( szFilename[length-1] == '/' || szFilename[length-1] == '\\' )
	{
		agk::Error( "Invalid path for DeleteFile file, must not end in a forward or backward slash" );
		return;
	}
	
	uString sPath( szFilename );
	if ( cFile::ExistsRaw( szFilename ) ) sPath.SetStr( szFilename+4 );
	else if ( cFile::ExistsWrite( szFilename ) ) agk::PlatformGetFullPathWrite(sPath);
	else return;
	
	remove( sPath.GetStr() );
	agk::m_bUpdateFileLists = true;
}

bool AGK::cFile::OpenToWrite( const char *szFilename, bool append )
{
	if ( !szFilename || !*szFilename ) return false;
	if ( pFile ) Close();
	mode = 1;
	
	sFullPath.SetStr( "" );
	uString sPath( szFilename );
	if ( strncmp(szFilename, "raw:", 4) == 0 ) 
	{
		sPath.SetStr( szFilename+4 );
		sFullPath.SetStr( sPath ); // record raw paths for media refresh on close
	}
	else agk::PlatformGetFullPathWrite(sPath);

	if ( !agk::PlatformCreateRawPath( sPath ) ) 
	{
		uString err = "Failed to create path for file ";
		err += sPath.GetStr();
		agk::Error( err );
		return false;
	}
	
	pFilePtr = 0;
	if ( append ) pFile = AGKfopen( sPath.GetStr(), "ab" );
	else pFile = AGKfopen( sPath.GetStr(), "wb" );
	
	if ( !pFile )
	{
		uString err = "Failed to open file for writing ";
		err += sPath.GetStr();
		agk::Error( err );
		return false;
	}
	
	// refresh any stored directory details for the new file
	//cFileEntry::AddNewFile( sPath.GetStr() ); // Android is now case sensitive in all cases
	agk::m_bUpdateFileLists = true;
	
	return true;
}

bool AGK::cFile::OpenToRead( const char *szFilename )
{
	if ( !szFilename || !*szFilename ) return false;
	if ( pFile ) Close();
	mode = 0;

	uString sPath( szFilename );
	if ( strncmp(szFilename, "expansion:", strlen("expansion:")) == 0 )
	{
		sPath.SetStr( szFilename + strlen("expansion") );
		sPath.Replace( ':', '/' );
		
		// always load from expansion file in case it has changed
		//if ( cFile::ExistsWrite( sPath ) ) agk::PlatformGetFullPathWrite( sPath );
		//else
		{
			agk::PlatformGetFullPathWrite( sPath );
			if ( !agk::ExtractExpansionFile( sPath, szFilename ) ) return false;
		}
	}
	else
	{
		// attempt read from my documents folder first, then exe directory, otherwise fail
		if ( cFile::ExistsRaw( szFilename ) ) sPath.SetStr( szFilename+4 );
		else if ( cFile::ExistsWrite( szFilename ) ) agk::PlatformGetFullPathWrite(sPath);
		else
		{
			if ( g_pActivity )
			{
				// try asset folder
				agk::PlatformGetFullPathRead(sPath);

				AAssetManager* assetManager = g_pActivity->assetManager;
				AAsset* asset = AAssetManager_open(assetManager, sPath.GetStr(), AASSET_MODE_UNKNOWN);
				if (asset)
				{
					pFile = (FILE*) asset;
					pFilePtr = (void*)1;
					return true;
				}
				else
				{
					uString err = "Failed to open file for reading ";
					err += szFilename;
					agk::Error( err );
					return false;
				}
			}
		}
	}
	
	pFilePtr = 0;
	pFile = AGKfopen( sPath, "rb" );
	if ( !pFile )
	{
		uString err = "Failed to open file for reading ";
		err += sPath.GetStr();
		agk::Error( err );
		return false;
	}

	sFullPath.SetStr( "" );
	
	return true;
}

void AGK::cFile::Close()
{
	if ( pFilePtr )
	{
		if ( pFile ) AAsset_close( (AAsset*) pFile );
	}
	else
	{
		if ( pFile ) fclose( pFile );
		if ( mode == 1 && sFullPath.GetLength() > 0 )
		{
			JNIEnv* lJNIEnv = g_pActivity->env;
			JavaVM* vm = g_pActivity->vm;
			vm->AttachCurrentThread(&lJNIEnv, NULL);
			jobject lNativeActivity = g_pActivity->clazz;
			jclass AGKHelper = GetAGKHelper(lJNIEnv);

			// refresh the file listings
			jmethodID refreshmedia = lJNIEnv->GetStaticMethodID( AGKHelper, "RefreshMediaPath", "(Landroid/app/Activity;Ljava/lang/String;)V" );
			jstring jfilePath = lJNIEnv->NewStringUTF(sFullPath.GetStr());
			lJNIEnv->CallStaticVoidMethod( AGKHelper, refreshmedia, lNativeActivity, jfilePath );
			lJNIEnv->DeleteLocalRef( jfilePath );

			vm->DetachCurrentThread();
		}
	}
	pFile = 0;
	sFullPath.SetStr( "" );
}

uint32_t AGK::cFile::GetPos()
{
	if ( !pFile ) return 0;
	if ( pFilePtr )
	{
		return AAsset_getLength( (AAsset*)pFile ) - AAsset_getRemainingLength( (AAsset*)pFile );
	}
	else
	{
		return ftell( pFile );
	}
}

void AGK::cFile::Seek( uint32_t pos )
{
	if ( !pFile ) return;
	if ( pFilePtr )
	{
		pFilePtr = (void*)1; // clear EOF 
		AAsset_seek( (AAsset*)pFile, pos, SEEK_SET );
	}
	else
	{
		fseek( pFile, pos, SEEK_SET );
	}
}

void AGK::cFile::Flush()
{
	if ( !pFile ) return;
	if ( pFilePtr ) return;
	fflush( pFile );
}

uint32_t AGK::cFile::GetSize()
{
	if ( !pFile ) return 0;
	if ( pFilePtr )
	{
		// clear EOF
		pFilePtr = (void*)1;
		return AAsset_getLength( (AAsset*)pFile );
	}
	else
	{
		fpos_t pos;
		fgetpos( pFile, &pos );
		fseek( pFile, 0, SEEK_END );
		uint32_t size = ftell( pFile );
		fsetpos( pFile, &pos );
		return size;
	}
}

void AGK::cFile::Rewind()
{
	if ( !pFile ) return;
	if ( pFilePtr ) pFilePtr = (void*)1; //clear EOF
	rewind( pFile );
}

bool AGK::cFile::IsEOF()
{
	if ( !pFile ) return true;
	if ( pFilePtr )
	{
		//return AAsset_getRemainingLength( (AAsset*)pFile ) == 0;
		return (pFilePtr == (void*)2);
	}
	else
	{
		return feof( pFile ) != 0;
	}
}

void cFile::WriteByte( unsigned char b )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot write to file opened for reading" );
#endif
		return;
	}

	fwrite( &b, 1, 1, pFile );
}

void AGK::cFile::WriteInteger( int i )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		agk::Error( "Cannot write to file opened for reading" );
		return;
	}
	
	//convert everything to little endian for cross platform compatibility
	i = agk::PlatformLittleEndian( i );
	fwrite( &i, 4, 1, pFile );
}

void AGK::cFile::WriteFloat( float f )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		agk::Error( "Cannot write to file opened for reading" );
		return;
	}
	fwrite( &f, 4, 1, pFile );
}

void AGK::cFile::WriteString( const char *str )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		agk::Error( "Cannot write to file opened for reading" );
		return;
	}
	uint32_t length = strlen( str );
	fwrite( str, 1, length+1, pFile );
}

void cFile::WriteString2( const char *str )
{
	if ( !str ) return;
	if ( !pFile ) return;
	if ( mode != 1 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot write to file opened for reading" );
#endif
		return;
	}
	uint32_t length = strlen( str );
	uint32_t l = agk::PlatformLittleEndian( length );
	fwrite( &l, 4, 1, pFile );
	fwrite( str, 1, length, pFile );
}

void AGK::cFile::WriteData( const char *str, uint32_t bytes )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
		agk::Error( "Cannot write to file opened for reading" );
		return;
	}
	
	fwrite( str, 1, bytes, pFile );
}

void AGK::cFile::WriteLine( const char *str )
{
	if ( !pFile ) return;
	if ( mode != 1 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot write to file opened for reading" );
#endif
		return;
	}
	uint32_t length = strlen( str );
	fwrite( str, 1, length, pFile );

	// strings terminate with CR (13,10) - so it resembles a regular text file when file viewed
	char pCR[2];
	pCR[0]=13;
	pCR[1]=10;
	fwrite( &pCR[0], 1, 1, pFile );
	fwrite( &pCR[1], 1, 1, pFile );
}

unsigned char cFile::ReadByte( )
{
	if ( !pFile ) return 0;
	if ( mode != 0 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot read from file opened for writing" );
#endif
		return 0;
	}
	unsigned char b = 0;
	if ( pFilePtr )
	{
		AAsset_read( (AAsset*)pFile, &b, 1 );
		// set EOF
		if ( AAsset_getRemainingLength( (AAsset*)pFile ) == 0 ) pFilePtr = (void*)2;
	}
	else
	{
		fread( &b, 1, 1, pFile );
	}
	
	return b;
}

int AGK::cFile::ReadInteger( )
{
	if ( !pFile ) return 0;
	if ( mode != 0 )
	{
		agk::Error( "Cannot read from file opened for writing" );
		return 0;
	}
	int i = 0;
	if ( pFilePtr )
	{
		AAsset_read( (AAsset*)pFile, &i, 4 );
		// set EOF
		if ( AAsset_getRemainingLength( (AAsset*)pFile ) == 0 ) pFilePtr = (void*)2;
	}
	else
	{
		fread( &i, 4, 1, pFile );
	}
	// convert back to local endian, everything in the file is little endian.
	return i = agk::PlatformLocalEndian( i );
}

float AGK::cFile::ReadFloat( )
{
	if ( !pFile ) return 0;
	if ( mode != 0 )
	{
		agk::Error( "Cannot read from file opened for writing" );
		return 0;
	}
	float f;
	if ( pFilePtr )
	{
		AAsset_read( (AAsset*)pFile, &f, 4 );
		// set EOF
		if ( AAsset_getRemainingLength( (AAsset*)pFile ) == 0 ) pFilePtr = (void*)2;
	}
	else
	{
		fread( &f, 4, 1, pFile );
	}
	return f;
}

int AGK::cFile::ReadString( uString &str )
{
	if ( !pFile ) return 0;
	if ( mode != 0 )
	{
		agk::Error( "Cannot read from file opened for writing" );
		return 0;
	}
	
	char *buffer = 0;
	int bufLen = 0;
	int pos = 0;
	int diff = 0;
	
	// read until a LF (10) line terminator is found, or eof.
	bool bNullFound = false;
	do
	{
		pos = bufLen;
		if ( bufLen == 0 )
		{
			bufLen = 256;
			buffer = new char[257];
			diff = 256;
		}
		else
		{
			int newLen = bufLen*3 / 2;
			char *newBuf = new char[ newLen+1 ];
			memcpy( newBuf, buffer, bufLen );
			delete [] buffer;
			diff = newLen - bufLen;
			buffer = newBuf;
			bufLen = newLen;
		}

		long lPos = GetPos();;
		uint32_t written = ReadData( buffer+pos, diff );
		buffer[pos+written] = 0;
		bool bFound = false;
		for ( uint32_t i = 0; i < written; i++ )
		{
			if ( buffer[ pos+i ] == 0 ) 
			{
				Seek( lPos+i+1 );
				bFound = true;
				break;
			}
		}
		if ( bFound ) break;
	} while( !IsEOF() );

	str.SetStr( buffer );
	delete [] buffer;
	return str.GetLength();
}

int cFile::ReadString2( uString &str )
{
	if ( !pFile ) 
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot read from file, file not open" );
#endif
		return -1;
	}

	if ( mode != 0 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot read from file opened for writing" );
#endif
		return -1;
	}

	str.ClearTemp();

	uint32_t length = ReadInteger();
	
	char *buffer = new char[ length+1 ];
	ReadData( buffer, length );
	buffer[ length ] = 0;
	str.SetStr( buffer );

	delete [] buffer;
		
	return str.GetLength();
}

int AGK::cFile::ReadLine( uString &str )
{
	if ( !pFile ) 
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot read from file, file not open" );
#endif
		return -1;
	}

	if ( mode != 0 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot read from file opened for writing" );
#endif
		return -1;
	}

	char *buffer = 0;
	int bufLen = 0;
	int pos = 0;
	int diff = 0;
	
	// read until a LF (10) line terminator is found, or eof.
	bool bNullFound = false;
	do
	{
		pos = bufLen;
		if ( bufLen == 0 )
		{
			bufLen = 256;
			buffer = new char[257];
			diff = 256;
		}
		else
		{
			int newLen = bufLen*3 / 2;
			char *newBuf = new char[ newLen+1 ];
			memcpy( newBuf, buffer, bufLen );
			delete [] buffer;
			diff = newLen - bufLen;
			buffer = newBuf;
			bufLen = newLen;
		}

		long lPos = GetPos();
		uint32_t written = ReadData( buffer+pos, diff );
		buffer[pos+written] = 0;
		bool bFound = false;
		for ( uint32_t i = 0; i < written; i++ )
		{
			if ( buffer[ pos+i ] == '\n' ) 
			{
				buffer[ pos+i ] = 0;
				Seek( lPos+i+1 );
				bFound = true;
				break;
			}
		}
		if ( bFound ) break;
	} while( !IsEOF() );

	str.SetStr( buffer );
	delete [] buffer;
	str.Trim( "\r\n" );
	return str.GetLength();
}
/*
int AGK::cFile::ReadLineFast( uString &str )
{
	if ( pFilePtr ) return ReadLine( str );

	if ( !pFile ) 
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot read from file, file not open" );
#endif
		return -1;
	}

	if ( mode != 0 )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot read from file opened for writing" );
#endif
		return -1;
	}

	str.ClearTemp();

	char tempstr[ 1024 ];
	if ( !fgets( tempstr, 1024, pFile ) ) return 0;
	str.SetStr( tempstr );

	while ( !IsEOF() && str.ByteAt( str.GetLength()-1 ) != '\n' )
	{
		if ( !fgets( tempstr, 1024, pFile ) ) break;
		str.Append( tempstr );
	}

	str.Trim( "\n\r" );
	return str.GetLength();
}
*/
int AGK::cFile::ReadData( char *str, uint32_t length )
{
	if ( !pFile ) return 0;
	if ( mode != 0 )
	{
		agk::Error( "Cannot read from file opened for writing" );
		return 0;
	}
	
	if ( pFilePtr )
	{
		int result = AAsset_read( (AAsset*)pFile, str, length );
		// set EOF
		if ( AAsset_getRemainingLength( (AAsset*)pFile ) == 0 ) pFilePtr = (void*)2;
		return result;
	}
	else
	{
		return fread( str, 1, length, pFile );
	}
}

void agk::ParseCurrentDirectory()
{
	m_bUpdateFileLists = false;

	// clear any old directories
	while ( m_pCurrentDirectories )
	{
		cDirectoryItem *pItem = m_pCurrentDirectories;
		m_pCurrentDirectories = m_pCurrentDirectories->m_pNext;
		delete pItem;
	}
	
	m_pCurrentDirectoryIter = 0;
	
	// clear any old files
	while ( m_pCurrentFiles )
	{
		cDirectoryItem *pItem = m_pCurrentFiles;
		m_pCurrentFiles = m_pCurrentFiles->m_pNext;
		delete pItem;
	}
	
	m_pCurrentFileIter = 0;
	
	// check root directory for files and directories (non-recursive)
	uString sPath( szRootDir );
	sPath.Append( m_sCurrentDir );
	if ( sPath.Find( '/' ) >= 0 ) sPath.Trunc( '/' );
	cFileEntry::ConvertToReal( sPath );
	
	// can only get files from the asset folder, not directories!
	if ( g_pActivity )
	{
		AAssetDir *dir = AAssetManager_openDir( g_pActivity->assetManager, sPath.GetStr() );
		if ( dir )
		{
			const char *szFile = AAssetDir_getNextFileName( dir );
			while ( szFile )
			{
				// file
				cDirectoryItem *pNewItem = new cDirectoryItem();
				pNewItem->iFlags |= AGK_FILE_TYPE_READ;
				pNewItem->m_sItem.SetStr( szFile );
				pNewItem->m_pNext = m_pCurrentFiles;
				m_pCurrentFiles = pNewItem;
				szFile = AAssetDir_getNextFileName( dir );
			}
			AAssetDir_close( dir );
		}
	}

	// check write directory for files and directories (non-recursive)
	sPath.SetStr( szWriteDir );
	sPath.Append( m_sCurrentDir );
	cFileEntry::ConvertToReal( sPath );
	
	DIR *pDir = opendir( sPath.GetStr() );
	if ( pDir )
	{
		dirent* item = readdir( pDir );
		while( item )
		{
			//uString sDir;
			//sDir.Format( "%s - %d", item->d_name, item->d_type );
			//agk::Warning( sDir );

			if ( item->d_type == DT_DIR )
			{
				// directory
				if ( strcmp( item->d_name, "." ) != 0 && strcmp( item->d_name, ".." ) != 0 )
				{
					bool bExists = false;
					cDirectoryItem *pItem = m_pCurrentDirectories;
					while ( pItem )
					{
						if ( pItem->m_sItem.CompareTo( item->d_name ) == 0 )
						{
							pItem->iFlags |= AGK_FILE_TYPE_WRITE;
							bExists = true;
							break;
						}
						pItem = pItem->m_pNext;
					}

					if ( !bExists )
					{
						cDirectoryItem *pNewItem = new cDirectoryItem();
						pNewItem->iFlags |= AGK_FILE_TYPE_WRITE;
						pNewItem->m_sItem.SetStr( item->d_name );
						pNewItem->m_pNext = m_pCurrentDirectories;
						m_pCurrentDirectories = pNewItem;
					}
				}
			}
			else if ( item->d_type == DT_REG )
			{
				// file
				bool bExists = false;
				cDirectoryItem *pItem = m_pCurrentFiles;
				while ( pItem )
				{
					if ( pItem->m_sItem.CompareTo( item->d_name ) == 0 )
					{
						pItem->iFlags |= AGK_FILE_TYPE_WRITE;
						bExists = true;
						break;
					}
					pItem = pItem->m_pNext;
				}

				if ( !bExists )
				{
					cDirectoryItem *pNewItem = new cDirectoryItem();
					pNewItem->iFlags |= AGK_FILE_TYPE_WRITE;
					pNewItem->m_sItem.SetStr( item->d_name );
					pNewItem->m_pNext = m_pCurrentFiles;
					m_pCurrentFiles = pNewItem;
				}
			}

			item = readdir( pDir );
		}

		closedir( pDir );
	}
}

int agk::SetCurrentDir( const char* szPath )
{
	if ( !szPath || strlen( szPath ) == 0 )
	{
		m_sCurrentDir.SetStr( "" );
		//ParseCurrentDirectory();
		m_bUpdateFileLists = true;
		return 1;
	}
	
	if ( strcmp( szPath, ".." ) == 0 ) 
	{
		int pos = m_sCurrentDir.Find( '/' );
		if ( pos >= 0 && pos < m_sCurrentDir.GetLength()-1 )
		{
			m_sCurrentDir.Trunc( '/' );
			m_sCurrentDir.Trunc( '/' );
			m_sCurrentDir.AppendAscii( '/' );
		}
		else
		{
			m_sCurrentDir.SetStr( "" );
		}
		//ParseCurrentDirectory();
		m_bUpdateFileLists = true;
		return 1;
	}
	
	if ( strstr(szPath, "..") )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Invalid path for SetCurrentDir, must not traverse backwards up the directory tree using ../" );
#endif
		return 0;
	}
	
	uString sPath( szPath );
	
	if ( szPath[0] == '\\' || szPath[0] == '/' )
	{
		m_sCurrentDir.SetStr( "" );
		sPath.SetStr( szPath+1 );
	}
	
	sPath.Replace( '\\', '/' );
		
	// add a final slash
	uint32_t length = strlen( szPath );
	if ( szPath[ length-1 ] != '/' && szPath[ length-1 ] != '\\' ) sPath.AppendAscii( '/' );

	m_sCurrentDir.Append( sPath );
	m_sCurrentDir.Replace( '\\', '/' );
	
	//ParseCurrentDirectory();
	m_bUpdateFileLists = true;
	
	return 1;
}

int agk::MakeFolder( const char* szName )
{
	if ( !szName || strlen( szName ) == 0 )
	{
		return 0;
	}

	if ( strncmp(szName, "raw:", 4) == 0 )
	{
		uString sPath( szName+4 );
		sPath.AppendAscii( '/' );
		PlatformCreateRawPath( sPath );
	}
	else
	{
		// no special characters
		if ( strchr( szName, ':' ) || strchr( szName, '/' ) || strchr( szName, '\\' ) || strstr( szName, ".." )  )
		{
			agk::Error( "Invalid folder name for MakeFolder, it must not contain the special characters / : \\ .." );
			return 0;
		}

		uString sPath( szName );
		PlatformGetFullPathWrite( sPath );
		sPath.AppendAscii( '/' );
		PlatformCreateRawPath( sPath );

		m_bUpdateFileLists = true;
	}
	
	return true;
}

void agk::DeleteFolder( const char* szName )
{
	if ( !szName || strlen( szName ) == 0 )
	{
		return;
	}

	if ( strncmp(szName, "raw:", 4) == 0 )
	{
		uString sPath( szName+4 );
		sPath.Replace( '\\', '/' );
		
		rmdir( sPath.GetStr() );
	}
	else
	{
		// no special characters
		if ( strchr( szName, ':' ) || strchr( szName, '/' ) || strchr( szName, '\\' ) || strstr( szName, ".." )  )
		{
			agk::Error( "Invalid folder name for DeleteFolder, it must not contain the special characters / : \\ .." );
			return;
		}

		uString sPath( szName );
		PlatformGetFullPathWrite( sPath );

		rmdir( sPath.GetStr() );
			
		m_bUpdateFileLists = true;
	}
}

// input commands
int agk::GetMultiTouchExists()
{
	return 1;
}

int agk::GetMouseExists()
{
	for ( int i = 0 ; i < 4; i++ )
	{
		if ( m_pMouse[ i ] ) return 1;
	}
	return 0;
}

int agk::GetKeyboardExists()
{
	return m_iKeyboardMode;
}

int agk::GetCameraExists()
{
	return 1; // todo	
}

int agk::GetGPSSensorExists()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/LocationSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID GetGPS = lJNIEnv->GetStaticMethodID( sdkClass, "GetGPSExists","(Landroid/app/Activity;)I" );
	m_iGPSSensorExists = lJNIEnv->CallStaticIntMethod( sdkClass, GetGPS, lNativeActivity );

	vm->DetachCurrentThread();

	return m_iGPSSensorExists;
}

//****f* Input/Support/SetRawMouseVisible
// FUNCTION
//   Attempts to hide or show any mouse pointer used on this platform. Not guaranteed to work
//   on all platforms.
// INPUTS
//   visible -- 1 to make it visible, 0 to hide it.
// SOURCE
void agk::SetRawMouseVisible( int visible )
//****
{
	// do nothing on android
}

void agk::SetRawMousePosition( float x, float y )
//****
{
	// do nothing on android
}

void cJoystick::DetectJoysticks()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID updateinput = lJNIEnv->GetStaticMethodID( AGKHelper, "UpdateInputDevices","()Ljava/lang/String;" );

	// call our java class method
	jstring result = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, updateinput );

	jboolean bCopy;
	const char* szString = lJNIEnv->GetStringUTFChars( result, &bCopy );

	if ( !szString || strlen(szString) == 0 )
	{
		// no devices, or they were disconnected
		for ( int i = 0; i < AGK_NUM_JOYSTICKS; i++ )
		{
			//if ( agk::m_pJoystick[ i ] ) delete agk::m_pJoystick[ i ];
			//agk::m_pJoystick[ i ] = 0;
			if ( agk::m_pJoystick[ i ] ) agk::m_pJoystick[ i ]->m_iConnected = 0;
		}
	}
	else
	{
		// extract device IDs from the string
		int deviceCount = 0;
		const char* szRemaining = szString;
		while ( *szRemaining )
		{
			if ( *szRemaining == ':' ) deviceCount++;
			szRemaining++;
		}

		int* ids = new int[ deviceCount+1 ];
		for ( int i = 0; i < deviceCount+1; i++ ) ids[ i ] = 0;
		char szTemp[32];

		int count = 0;
		szRemaining = szString;
		const char *szToken;
		while( (szToken = strchr( szRemaining, ':' )) )
		{
			uint32_t length = (uint32_t)(szToken-szRemaining);
			if ( length > 31 ) 
			{
				szRemaining = szToken+1;
				continue;
			}

			if ( length == 0 ) continue;

			strncpy( szTemp, szRemaining, length );
			szTemp[ length ] = 0;
			int value = atoi( szTemp );
			ids[ count++ ] = value;

			szRemaining = szToken+1;
		}

		if ( strlen(szRemaining) > 0 )
		{
			strcpy( szTemp, szRemaining );
			int value = atoi( szTemp );
			ids[ count++ ] = value;
		}

		// check for disconnected devices
		for ( int i = 0; i < AGK_NUM_JOYSTICKS; i++ )
		{
			if ( !agk::m_pJoystick[ i ] || agk::m_pJoystick[ i ]->m_iConnected == 0 ) continue;

			int id = (int) ((long long) agk::m_pJoystick[ i ]->m_pDevice);
			if ( id < 1 )
			{
				agk::m_pJoystick[ i ]->m_iConnected = 0;
				//delete agk::m_pJoystick[ i ]; 
				//agk::m_pJoystick[ i ] = 0;
				continue;
			}

			bool bFound = false;
			for ( int j = 0; j < count; j++ )
			{
				if ( ids[j] == id ) 
				{ 
					ids[j] = 0; // don't need to worry about this ID any more
					bFound = true; 
					// don't break as we should clear out any duplicate IDs
				}
			}
			if ( !bFound ) 
			{ 
				//delete agk::m_pJoystick[ i ]; 
				//agk::m_pJoystick[ i ] = 0; 
				agk::m_pJoystick[ i ]->m_iConnected = 0;
			}
		}

		// assign new devices, all non-zero ids are now new devices
		for ( int i = 0; i < count; i++ )
		{
			int id = ids[i];
			if ( id < 1 ) continue;

			// look for a free joystick slot
			bool bFound = false;
			for ( int j = 0; j < AGK_NUM_JOYSTICKS; j++ )
			{
				if ( !agk::m_pJoystick[ j ] )
				{
					agk::m_pJoystick[ j ] = new cJoystick( (void*)id );
					agk::m_pJoystick[ j ]->m_iConnected = 1;
					bFound = true;
					break;
				}
				else if ( agk::m_pJoystick[ j ]->m_iConnected == 0 )
				{
					agk::m_pJoystick[ j ]->m_pDevice = (void*)id;
					agk::m_pJoystick[ j ]->m_iConnected = 1;
					bFound = true;
					break;
				}
			}

			if ( !bFound ) break; // no more joystick slots
		}
	}
	
	lJNIEnv->ReleaseStringUTFChars( result, szString );
	lJNIEnv->DeleteLocalRef( result );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void cJoystick::PlatformUpdate()
{

}

// date/time commands

int64_t agk::GetUnixTime64()
{
	time_t unixtime = time(NULL);
	return unixtime;
}

int agk::GetUnixTime()
{
	time_t unixtime = time(NULL);
	if ( unixtime > 2147483646 ) unixtime = 2147483647;
	if ( unixtime < -2147483647 ) unixtime = -2147483647;
	return (int) unixtime;
}

//****f* Time/General/GetDayOfWeek
// FUNCTION
//   Returns an integer representing the current day of the week in the range 0-6 with 0 as Sunday, 1 as Monday, etc.
// SOURCE
int agk::GetDayOfWeek()
//****
{
	time_t a = time ( NULL );
	tm*    b = localtime ( &a );
	int    c = b->tm_wday;
	return c;
}

//****f* Time/General/GetCurrentDate
// FUNCTION
//   Returns a string representing the current date in the format YYYY-MM-DD, for example 2012-01-09.
// SOURCE
char* agk::GetCurrentDate()
//****
{
	time_t a = time ( NULL );
	tm*    b = localtime ( &a );
	
	int year = b->tm_year + 1900;
	int month = b->tm_mon+1;
	int days = b->tm_mday;

	uString date;
	date.Format( "%04d-%02d-%02d", year, month, days );
	
	char* str = new char[ date.GetLength()+1 ];
	strcpy( str, date.GetStr() );
	return str;
}

//****f* Time/General/GetCurrentTime
// FUNCTION
//   Returns a string representing the current time in the format HH-MM-SS, for example 15:23:04.
// SOURCE
char* agk::GetCurrentTime()
//****
{
	time_t a = time ( NULL );
	tm*    b = localtime ( &a );
	
	int hours = b->tm_hour;
	int minutes = b->tm_min;
	int seconds = b->tm_sec;

	uString time;
	time.Format( "%02d:%02d:%02d", hours, minutes, seconds );
	
	char* str = new char[ time.GetLength()+1 ];
	strcpy( str, time.GetStr() );
	return str;
}

// advert commands

int agk::PlatformGetAdPortal()
{
	//return 659; // Windows Phone 7
	//return 642; // iPhone
	//return 551; // Ovi
	return 559; // Android
	//return 635; // Blackberry
	//return 641; // SonyEricsson 
	//return 661; // Bada
	//return 738; // Palm
	//return 818; // Samsung
	//return 947; // iPad, doesn't seem to produce ads
}

///

void cEditBox::PlatformStartText()
{
	float topY = m_fY+m_fHeight;
	if ( !GetFixed() ) topY = agk::WorldToScreenY( topY );
	if ( !IsAlternateInput() || topY < agk::GetVirtualHeight()/2.1f || agk::m_iKeyboardMode != 2 ) 
	{
		g_bEditBoxHack = true;
		
		if ( agk::m_iKeyboardMode == 2 )
		{
			showKeyboard( true, GetMultiLine() ? 1 : 0, m_iInputType );

			JNIEnv* lJNIEnv = g_pActivity->env;
			JavaVM* vm = g_pActivity->vm;
			vm->AttachCurrentThread(&lJNIEnv, NULL);

			jobject lNativeActivity = g_pActivity->clazz;
			if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
			
			jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
			if ( !sdkClass ) 
			{
				vm->DetachCurrentThread();
				return;
			}

			jmethodID SetText = lJNIEnv->GetStaticMethodID( sdkClass, "SetInputText", "(Landroid/app/Activity;Ljava/lang/String;I)V" );

			jstring text = lJNIEnv->NewStringUTF(m_sCurrInput.GetStr());
			lJNIEnv->CallStaticVoidMethod( sdkClass, SetText, lNativeActivity, text, m_sCurrInput.GetNumChars() );
			lJNIEnv->DeleteLocalRef( text );

			vm->DetachCurrentThread();

			g_fChangeTimer = 0.25;
		}
	}
	else
	{
		agk::StartTextInput( m_sCurrInput );
		g_bPasswordMode = IsPassword();
	}
}

void cEditBox::PlatformEndText()
{
	if ( g_bEditBoxHack ) 
	{
		showKeyboard( false,0,0 );
	}
	else
	{
		agk::StopTextInput();
	}
}

void cEditBox::PlatformUpdateExternal()
{
	
}

bool cEditBox::PlatformUpdateText()
{
	m_iOldLength = m_sCurrInput.GetNumChars();

	if ( g_bEditBoxHack ) 
	{
		if ( agk::m_iKeyboardMode != 2 ) return false;

		if ( g_fChangeTimer > 0 )
		{
			g_fChangeTimer -= agk::GetFrameTime();
			return false;
		}

		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return false;
		}

		jmethodID GetText = lJNIEnv->GetStaticMethodID( sdkClass, "GetInputText", "(Landroid/app/Activity;)Ljava/lang/String;" );
		jstring text = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, GetText, lNativeActivity );
		
		jboolean bCopy;
		const char* sText = lJNIEnv->GetStringUTFChars( text, &bCopy );

		m_sCurrInput.SetStr( sText );
		
		lJNIEnv->ReleaseStringUTFChars( text, sText );
		lJNIEnv->DeleteLocalRef( text );

		jmethodID GetFinished = lJNIEnv->GetStaticMethodID( sdkClass, "GetInputFinished", "(Landroid/app/Activity;)I" );
		int finished = lJNIEnv->CallStaticIntMethod( sdkClass, GetFinished, lNativeActivity );
		
		if ( m_iOldCursorPos < 0 || m_iOldCursorPos == m_iCursorPos )
		{
			jmethodID GetCursor = lJNIEnv->GetStaticMethodID( sdkClass, "GetInputCursor", "(Landroid/app/Activity;)I" );
			m_iCursorPos = lJNIEnv->CallStaticIntMethod( sdkClass, GetCursor, lNativeActivity );
			if ( m_iOldCursorPos != m_iCursorPos )
			{
				m_iShowCursor = 1;
				m_fCursorBlinkTime = m_fCursorBlinkDelay;
			}
		}
		else
		{
			jmethodID SetCursor = lJNIEnv->GetStaticMethodID( sdkClass, "SetInputTextCursor", "(Landroid/app/Activity;I)V" );
			lJNIEnv->CallStaticVoidMethod( sdkClass, SetCursor, lNativeActivity, m_iCursorPos );
		}

		vm->DetachCurrentThread();

		// check the returned string
		bool bChanged = false;
		m_pInputText->SetString( m_sCurrInput );
		while ( (m_iMaxLines > 0 && m_pInputText->GetLines() > m_iMaxLines) || (m_iMaxChars > 0 && m_sCurrInput.GetNumChars() > m_iMaxChars) )
		{
			m_sCurrInput.Trunc2( 1 );
			m_pInputText->SetString( m_sCurrInput );
			bChanged = true;
		}
	    
		if ( !SupportsExtAscii() && (m_iFlags & AGK_EDITBOX_USING_NEW_FONT) == 0 )
		{
			int length = m_sCurrInput.GetNumChars();
			m_sCurrInput.StripUTF8();
			if ( length != m_sCurrInput.GetNumChars() )
			{
				if ( !GetSupportWarned() )
				{
					m_iFlags |= AGK_EDITBOX_SUPPORT_WARNED;
					agk::Message("This edit box does not support extended characters");
				}
				m_pInputText->SetString( m_sCurrInput );
				bChanged = true;
			}
		}

		if ( m_iCursorPos > (int) m_sCurrInput.GetNumChars() ) m_iCursorPos = m_sCurrInput.GetNumChars();

		if ( finished == 1 )
		{
			SetFocus( false );
			return true;
		}
	    
		if ( bChanged )
		{
			JNIEnv* lJNIEnv = g_pActivity->env;
			JavaVM* vm = g_pActivity->vm;
			vm->AttachCurrentThread(&lJNIEnv, NULL);

			jobject lNativeActivity = g_pActivity->clazz;
			if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
			
			jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
			if ( !sdkClass ) 
			{
				vm->DetachCurrentThread();
				return false;
			}

			jmethodID SetText = lJNIEnv->GetStaticMethodID( sdkClass, "SetInputText", "(Landroid/app/Activity;Ljava/lang/String;I)V" );

			jstring text = lJNIEnv->NewStringUTF(m_sCurrInput.GetStr());
			lJNIEnv->CallStaticVoidMethod( sdkClass, SetText, lNativeActivity, text, m_iCursorPos );
			lJNIEnv->DeleteLocalRef( text );

			vm->DetachCurrentThread();
		}

		if ( m_iOldLength != m_sCurrInput.GetNumChars() )
		{
			m_iShowCursor = 1;
			m_fCursorBlinkTime = m_fCursorBlinkDelay;
		}
		m_iOldLength = m_sCurrInput.GetNumChars();
		m_iOldCursorPos = m_iCursorPos;
		return false;
	}
	else
	{
		agk::GetTextInput( m_sCurrInput );
		    
		bool bChanged = false;
    
		m_pInputText->SetString( m_sCurrInput );
		while ( (m_iMaxLines > 0 && m_pInputText->GetLines() > m_iMaxLines) || (m_iMaxChars > 0 && m_sCurrInput.GetNumChars() > m_iMaxChars) )
		{
			m_sCurrInput.Trunc2( 1 );
			m_pInputText->SetString( m_sCurrInput );
			bChanged = true;
		}
	    
		if ( !SupportsExtAscii() && (m_iFlags & AGK_EDITBOX_USING_NEW_FONT) == 0 )
		{
			int length = m_sCurrInput.GetNumChars();
			m_sCurrInput.StripUTF8();
			if ( length != m_sCurrInput.GetNumChars() )
			{
				if ( !GetSupportWarned() )
				{
					m_iFlags |= AGK_EDITBOX_SUPPORT_WARNED;
					agk::Message("This edit box does not support extended characters");
				}
				m_pInputText->SetString( m_sCurrInput );
				bChanged = true;
			}
		}
	    
		if ( bChanged )
		{
			agk::PlatformChangeTextInput( m_sCurrInput );
		}
	    
		m_iCursorPos = m_sCurrInput.GetNumChars();
		
		return true;
	}
}

void cEditBox::PlatformUpdateTextEnd()
{
	if ( g_bEditBoxHack ) 
	{
		if ( agk::m_iKeyboardMode != 2 ) return;
		if ( m_iOldLength == m_sCurrInput.GetNumChars() ) return;

		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/TextEntrySDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		jmethodID SetText = lJNIEnv->GetStaticMethodID( sdkClass, "SetInputText", "(Landroid/app/Activity;Ljava/lang/String;I)V" );

		jstring text = lJNIEnv->NewStringUTF(m_sCurrInput.GetStr());
		lJNIEnv->CallStaticVoidMethod( sdkClass, SetText, lNativeActivity, text, m_iCursorPos );
		lJNIEnv->DeleteLocalRef( text );

		vm->DetachCurrentThread();

		m_iOldLength = m_sCurrInput.GetNumChars();
		g_fChangeTimer = 0.25;
	}
}

//****f* HTTP/General/OpenBrowser
// FUNCTION
//   Opens the default browser of the current platform with points it to the page given.
// SOURCE
void agk::OpenBrowser( const char *url )
//****
{
	jint lFlags = 0;
	JNIEnv* lJNIEnv = g_pActivity->env;

	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jclass classIntent = 0;
	jclass classURI = 0;
	jmethodID methodURIParse = 0;
	jmethodID methodStartActivity = 0;
	jmethodID methodIntentConstruct = 0;
	jfieldID fieldActionView = 0;
	jstring stringActionView = 0;
	jobject objURI = 0;
	jobject objIntent = 0;
	if ( !g_pActivity ) agk::Warning("Failed to get activity pointer");
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	//jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	jclass classActivity = lJNIEnv->FindClass("android/app/NativeActivity");
	classIntent = lJNIEnv->FindClass("android/content/Intent");
	classURI = lJNIEnv->FindClass("android/net/Uri");

	if ( !classActivity ) agk::Warning("Failed to get class NativeActivity");
	if ( !classIntent ) agk::Warning("Failed to get class Intent");
	if ( !classURI ) agk::Warning("Failed to get class URI");

	// get ACTION_VIEW field
	fieldActionView = lJNIEnv->GetStaticFieldID( classIntent, "ACTION_VIEW", "Ljava/lang/String;" );
	if ( !fieldActionView ) agk::Warning("Failed to get field ACTION_VIEW");
	
	stringActionView = (jstring) lJNIEnv->GetStaticObjectField( classIntent, fieldActionView );
	if ( !stringActionView ) agk::Warning("Failed to get string ACTION_VIEW");

	// get Uri.parse()
	methodURIParse = lJNIEnv->GetStaticMethodID( classURI, "parse","(Ljava/lang/String;)Landroid/net/Uri;");
	if ( !methodURIParse ) agk::Warning("Failed to get method parse");

	// get startActivity()
	methodStartActivity = lJNIEnv->GetMethodID( classActivity, "startActivity","(Landroid/content/Intent;)V");
	if ( !methodStartActivity ) agk::Warning("Failed to get method startActivity");

	// get Intent Constructor
	methodIntentConstruct = lJNIEnv->GetMethodID( classIntent, "<init>","(Ljava/lang/String;Landroid/net/Uri;)V");
	if ( !methodIntentConstruct ) agk::Warning("Failed to get method construct");

	uString sURL( url );
	if ( sURL.FindStr( ":" ) < 0 ) sURL.Prepend( "http://" );

	jstring urlstring = lJNIEnv->NewStringUTF( sURL.GetStr() );
	objURI = lJNIEnv->CallStaticObjectMethod( classURI, methodURIParse, urlstring );
	if ( !objURI ) agk::Warning("Failed to parse URI");
	lJNIEnv->DeleteLocalRef( urlstring );

	objIntent = lJNIEnv->NewObject( classIntent, methodIntentConstruct, stringActionView, objURI );
	if ( !objIntent ) agk::Warning("Failed to construct Intent");
	lJNIEnv->CallVoidMethod( lNativeActivity, methodStartActivity, objIntent );
	agk::Warning("Opened Browser");

	lJNIEnv->DeleteLocalRef( objURI );
	lJNIEnv->DeleteLocalRef( objIntent );

	vm->DetachCurrentThread();
}

uint32_t agk::RunApp( const char *szFilename, const char *szParameters )
{
	return 0;
}

uint32_t agk::GetAppRunning( uint32_t appID )
{
	return 0;
}

void agk::TerminateApp( uint32_t appID )
{

}

void agk::ViewFile( const char* szFilename )
{
	if ( strncmp( szFilename, "raw:", 4 ) == 0 )
	{
		if ( CheckPermission( "WriteExternal" ) != 2 )
		{
			agk::Error( "You must request the WriteExternal permission before you can access raw file locations" );
			return;
		}
	}

	uString sPath( szFilename );
	if ( !GetRealPath( sPath ) )
	{
		agk::Error( "Could not find file at the specified path" );
		return;
	}

	if ( cFile::ExistsRead( szFilename ) && !cFile::ExistsWrite( szFilename ) ) 
	{
		// move file to write folder to avoid APK assets folder
		cFile cSrcFile;
		cSrcFile.OpenToRead( szFilename );

		cFile cDstFile;
		cDstFile.OpenToWrite( szFilename );

		char buf[ 4096 ];
		do
		{
			int written = cSrcFile.ReadData( buf, 4096 );
			cDstFile.WriteData( buf, written );
		} while( !cSrcFile.IsEOF() );

		cDstFile.Close();
		cSrcFile.Close();

		sPath.SetStr( szFilename );
		agk::PlatformGetFullPathWrite( sPath );
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID viewFile = lJNIEnv->GetStaticMethodID( AGKHelper, "ViewFile", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring filename = lJNIEnv->NewStringUTF(sPath.GetStr());
	lJNIEnv->CallStaticVoidMethod( AGKHelper, viewFile, lNativeActivity, filename );
	lJNIEnv->DeleteLocalRef( filename );

	vm->DetachCurrentThread();
}

void agk::ShareText( const char* szText )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID shareText = lJNIEnv->GetStaticMethodID( AGKHelper, "ShareText", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring text = lJNIEnv->NewStringUTF(szText);
	lJNIEnv->CallStaticVoidMethod( AGKHelper, shareText, lNativeActivity, text );
	lJNIEnv->DeleteLocalRef( text );

	vm->DetachCurrentThread();
}

void agk::ShareImage( const char* szFilename )
{
	uString sPath( szFilename );
	if ( !GetRealPath( sPath ) )
	{
		agk::Error( "Could not find file at the specified path" );
		return;
	}

	if ( cFile::ExistsRead( szFilename ) && !cFile::ExistsWrite( szFilename ) ) 
	{
		// move file to write folder to avoid APK assets folder
		cFile cSrcFile;
		cSrcFile.OpenToRead( szFilename );

		cFile cDstFile;
		cDstFile.OpenToWrite( szFilename );

		char buf[ 4096 ];
		do
		{
			int written = cSrcFile.ReadData( buf, 4096 );
			cDstFile.WriteData( buf, written );
		} while( !cSrcFile.IsEOF() );

		cDstFile.Close();
		cSrcFile.Close();

		sPath.SetStr( szFilename );
		agk::PlatformGetFullPathWrite( sPath );
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID shareImage = lJNIEnv->GetStaticMethodID( AGKHelper, "ShareImage", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring filename = lJNIEnv->NewStringUTF(sPath.GetStr());
	lJNIEnv->CallStaticVoidMethod( AGKHelper, shareImage, lNativeActivity, filename );
	lJNIEnv->DeleteLocalRef( filename );

	vm->DetachCurrentThread();
}

void agk::ShareImageAndText( const char* szFilename, const char* szText )
{
	uString sPath( szFilename );
	if ( !GetRealPath( sPath ) )
	{
		agk::Error( "Could not find file at the specified path" );
		return;
	}

	if ( cFile::ExistsRead( szFilename ) && !cFile::ExistsWrite( szFilename ) ) 
	{
		// move file to write folder to avoid APK assets folder
		cFile cSrcFile;
		cSrcFile.OpenToRead( szFilename );

		cFile cDstFile;
		cDstFile.OpenToWrite( szFilename );

		char buf[ 4096 ];
		do
		{
			int written = cSrcFile.ReadData( buf, 4096 );
			cDstFile.WriteData( buf, written );
		} while( !cSrcFile.IsEOF() );

		cDstFile.Close();
		cSrcFile.Close();

		sPath.SetStr( szFilename );
		agk::PlatformGetFullPathWrite( sPath );
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "ShareImageAndText", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V" );

	jstring filename = lJNIEnv->NewStringUTF(sPath.GetStr());
	jstring text = lJNIEnv->NewStringUTF(szText);
	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity, filename, text );
	lJNIEnv->DeleteLocalRef( text );
	lJNIEnv->DeleteLocalRef( filename );
	
	vm->DetachCurrentThread();
}

void agk::ShareFile( const char* szFilename )
//****
{
	uString sPath( szFilename );
	if ( !GetRealPath( sPath ) )
	{
		agk::Error( "Could not find file at the specified path" );
		return;
	}

	if ( cFile::ExistsRead( szFilename ) && !cFile::ExistsWrite( szFilename ) ) 
	{
		// move file to write folder to avoid APK assets folder
		cFile cSrcFile;
		cSrcFile.OpenToRead( szFilename );

		cFile cDstFile;
		cDstFile.OpenToWrite( szFilename );

		char buf[ 4096 ];
		do
		{
			int written = cSrcFile.ReadData( buf, 4096 );
			cDstFile.WriteData( buf, written );
		} while( !cSrcFile.IsEOF() );

		cDstFile.Close();
		cSrcFile.Close();

		sPath.SetStr( szFilename );
		agk::PlatformGetFullPathWrite( sPath );
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	jmethodID shareFile = lJNIEnv->GetStaticMethodID( AGKHelper, "ShareFile", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring filename = lJNIEnv->NewStringUTF(sPath.GetStr());
	lJNIEnv->CallStaticVoidMethod( AGKHelper, shareFile, lNativeActivity, filename );
	lJNIEnv->DeleteLocalRef( filename );

	vm->DetachCurrentThread();
}

void agk::FacebookActivateAppTracking()
//****
{
	/*
	if ( m_sFBAppID.GetLength() == 0 )
	{
		agk::Error("FacebookSetup must be called before FacebookActivateAppTracking");
		return;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "FacebookActivateAppTracking","(Landroid/app/Activity;)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( AGKHelper, method, lNativeActivity );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
	*/
}

int agk::GetInternetState()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID networkAvailable = lJNIEnv->GetStaticMethodID( AGKHelper, "isNetworkAvailable","(Landroid/app/Activity;)I" );

	// call our java class method
	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, networkAvailable, lNativeActivity );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return result;
}

void agk::SetPushNotificationKeys( const char* data1, const char* reserved )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PushNotificationsSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID setPNKey = lJNIEnv->GetStaticMethodID( sdkClass, "setPushNotificationKeys","(Ljava/lang/String;Ljava/lang/String;)V" );

	jstring strKey1 = lJNIEnv->NewStringUTF( data1 ? data1 : "" );
	jstring strKey2 = lJNIEnv->NewStringUTF( reserved ? reserved : "" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, setPNKey, strKey1, strKey2 );
	lJNIEnv->DeleteLocalRef( strKey1 );
	lJNIEnv->DeleteLocalRef( strKey2 );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

int agk::PushNotificationSetup()
//****
{
	m_sPNToken.SetStr( "1" );

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PushNotificationsSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// get the method from our java class
	jmethodID registerPN = lJNIEnv->GetStaticMethodID( sdkClass, "registerPushNotification","(Landroid/app/Activity;)I" );

	// call our java class method
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, registerPN, lNativeActivity );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return result;
}


char* agk::GetPushNotificationToken()
//****
{
	if ( m_sPNToken.CompareTo("1") == 0 )
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		// get NativeActivity object (clazz)
		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PushNotificationsSDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			char *str = new char[ 1 ];
			*str = 0;
			return str;
		}

		// get the method from our java class
		jmethodID getPN = lJNIEnv->GetStaticMethodID( sdkClass, "getPNRegID","()Ljava/lang/String;" );

		// call our java class method
		jstring result = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, getPN, lNativeActivity );

		jboolean bCopy;
		const char* sPNRegId = lJNIEnv->GetStringUTFChars( result, &bCopy );
		
		if ( !sPNRegId || strlen(sPNRegId) < 1 ) 
		{
			lJNIEnv->ReleaseStringUTFChars( result, sPNRegId );
			lJNIEnv->DeleteLocalRef( result );
			vm->DetachCurrentThread();

			char *str = new char[ 1 ];
			*str = 0;
			return str;
		}
		 
		m_sPNToken.SetStr( sPNRegId );
		
		lJNIEnv->ReleaseStringUTFChars( result, sPNRegId );
		lJNIEnv->DeleteLocalRef( result );

		// detatch thread from Java VM before we leave
		vm->DetachCurrentThread();
	}

	char *str = new char[ m_sPNToken.GetLength()+1 ];
	strcpy( str, m_sPNToken.GetStr() );
	return str;
}

// ********************
// Social commands
// ********************

// internally called by AGK
void agk::PlatformSocialPluginsSetup( void )
{
	// do nothing
}

void agk::PlatformSocialPluginsDestroy( void )
{
	// do nothing
}

// actually links from RequestAppReview()
void agk::PlatformRateApp( const char* szID, const char* title, const char* message )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/RequestReviewSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "RequestReview","()V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, method );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

// IN APP PURCHASE COMMANDS

void agk::PlatformInAppPurchaseReset()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapReset","()V" );
	if ( method ) lJNIEnv->CallStaticVoidMethod( sdkClass, method );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformInAppPurchaseRestore()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapRestore","()V" );
	if ( method ) lJNIEnv->CallStaticVoidMethod( sdkClass, method );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

bool agk::PlatformHasInAppPurchase()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );

	vm->DetachCurrentThread();

	return sdkClass != 0;
}

void agk::PlatformInAppPurchaseSetKeys( const char* szData1, const char* szData2 )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapSetKeyData","(Ljava/lang/String;Ljava/lang/String;)V" );

	if ( method )
	{
		// call our java class method
		jstring strPublicKey = lJNIEnv->NewStringUTF( szData1 ? szData1 : "" );
		jstring strDeveloperID = lJNIEnv->NewStringUTF( szData2 ? szData2 : "" );
		lJNIEnv->CallStaticVoidMethod( sdkClass, method, strPublicKey, strDeveloperID );
		lJNIEnv->DeleteLocalRef( strPublicKey );
		lJNIEnv->DeleteLocalRef( strDeveloperID );
	}

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformInAppPurchaseSetTitle( const char* szTitle )
{
	// do nothing
}

void agk::PlatformInAppPurchaseAddProductID( const char* szID, int type )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapAddProduct","(Ljava/lang/String;II)V" );

	if ( method )
	{
		// call our java class method
		jstring strProduct = lJNIEnv->NewStringUTF( szID );
		lJNIEnv->CallStaticVoidMethod( sdkClass, method, strProduct, 0, type );
		lJNIEnv->DeleteLocalRef( strProduct );
	}

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformInAppPurchaseSetup()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapSetup","(Landroid/app/Activity;)V" );
	if ( method ) lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity );

	vm->DetachCurrentThread();
}

void agk::PlatformInAppPurchaseActivate( int iID )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapMakePurchase","(Landroid/app/Activity;I)V" );
	if ( method ) lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, iID );
	
	vm->DetachCurrentThread();
}

void agk::PlatformInAppPurchaseActivateWithPlan( int iID, const char* planToken )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapMakePurchaseWithPlan","(Landroid/app/Activity;ILjava/lang/String;)V" );

	if ( method )
	{
		jstring strToken = lJNIEnv->NewStringUTF( planToken );
		lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, iID, strToken );
		lJNIEnv->DeleteLocalRef( strToken );
	}

	vm->DetachCurrentThread();
}

void agk::PlatformInAppPurchaseResetPurchase( const char* szToken )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapResetPurchase","(Ljava/lang/String;)V" );

	if ( method )
	{
		jstring strToken = lJNIEnv->NewStringUTF( szToken );
		lJNIEnv->CallStaticVoidMethod( sdkClass, method, strToken );
		lJNIEnv->DeleteLocalRef( strToken );
	}

	vm->DetachCurrentThread();
}

int agk::PlatformGetInAppPurchaseState( void )
{
	return 1; // deprecated
}

int agk::PlatformGetInAppPurchaseAvailable( int iID )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapCheckPurchase","(I)I" );

	int result = 0;
	if ( method ) result = lJNIEnv->CallStaticIntMethod( sdkClass, method, iID );

	vm->DetachCurrentThread();

	return result;
}

int agk::PlatformGetInAppPurchaseAvailable2( int iID )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapCheckPurchase2","(I)I" );

	int result = 0;
	if ( method ) result = lJNIEnv->CallStaticIntMethod( sdkClass, method, iID );

	vm->DetachCurrentThread();

	return result;
}

char* agk::PlatformGetInAppPurchaseLocalPrice( int iID )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetPrice","(I)Ljava/lang/String;" );

	char* retstr;
	if ( !method )
	{
		retstr = new char[1];
		*retstr = 0;
	}
	else
	{
		jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, iID );
		jboolean bCopy;
		const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

		retstr = new char[ strlen(str2) + 1 ];
		strcpy( retstr, str2 );
			
		lJNIEnv->ReleaseStringUTFChars( str, str2 );
		lJNIEnv->DeleteLocalRef( str );
	}

	vm->DetachCurrentThread();

	return retstr;
}

char* agk::PlatformGetInAppPurchaseDescription( int iID )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetDescription","(I)Ljava/lang/String;" );

	char* retstr;
	if ( !method )
	{
		retstr = new char[1];
		*retstr = 0;
	}
	else
	{
		jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, iID );
		jboolean bCopy;
		const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

		retstr = new char[ strlen(str2) + 1 ];
		strcpy( retstr, str2 );
		
		lJNIEnv->ReleaseStringUTFChars( str, str2 );
		lJNIEnv->DeleteLocalRef( str );
	}

	vm->DetachCurrentThread();

	return retstr;
}

int agk::PlatformGetInAppPurchaseIsRenewing( int iID )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetIsRenewing","(I)I" );

	int result = -1;
	if ( method ) result = lJNIEnv->CallStaticIntMethod( sdkClass, method, iID );

	vm->DetachCurrentThread();

	return result;
}

char* agk::PlatformGetInAppPurchaseSignature(int iID)
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID(sdkClass, "iapGetSignature", "(I)Ljava/lang/String;");

	char* retstr;
	if ( !method )
	{
		retstr = new char[1];
		*retstr = 0;
	}
	else
	{
		jstring str = (jstring)lJNIEnv->CallStaticObjectMethod(sdkClass, method, iID);
		jboolean bCopy;
		const char* str2 = lJNIEnv->GetStringUTFChars(str, &bCopy);

		retstr = new char[strlen(str2) + 1];
		strcpy(retstr, str2);

		lJNIEnv->ReleaseStringUTFChars(str, str2);
		lJNIEnv->DeleteLocalRef(str);
	}

	vm->DetachCurrentThread();

	return retstr;
}

char* agk::PlatformGetInAppPurchaseToken(int iID)
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID(sdkClass, "iapGetToken", "(I)Ljava/lang/String;");

	char *retstr;
	if ( method )
	{
		jstring str = (jstring)lJNIEnv->CallStaticObjectMethod(sdkClass, method, iID);
		jboolean bCopy;
		const char* str2 = lJNIEnv->GetStringUTFChars(str, &bCopy);

		retstr = new char[strlen(str2) + 1];
		strcpy(retstr, str2);

		lJNIEnv->ReleaseStringUTFChars(str, str2);
		lJNIEnv->DeleteLocalRef(str);
	}
	else
	{
		retstr = new char[1]; 
		*retstr = 0;
	}

	vm->DetachCurrentThread();

	return retstr;
}

int agk::PlatformGetInAppPurchaseSubNumPlans( int iID )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetNumPlans","(I)I" );

	int result = 0;
	if ( method ) result = lJNIEnv->CallStaticIntMethod( sdkClass, method, iID );

	vm->DetachCurrentThread();

	return result;
}

int agk::PlatformGetInAppPurchaseSubPlanNumPeriods( int iID, int planIndex )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetPlanNumPeriods","(II)I" );

	int result = 0;
	if ( method ) result = lJNIEnv->CallStaticIntMethod( sdkClass, method, iID, planIndex );

	vm->DetachCurrentThread();

	return result;
}

char* agk::PlatformGetInAppPurchaseSubPlanPrice( int iID, int planIndex, int periodIndex )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetPlanPrice","(III)Ljava/lang/String;" );

	char* retstr;
	if ( !method )
	{
		retstr = new char[1];
		*retstr = 0;
	}
	else
	{
		jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, iID, planIndex, periodIndex );
		jboolean bCopy;
		const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

		retstr = new char[ strlen(str2) + 1 ];
		strcpy( retstr, str2 );
			
		lJNIEnv->ReleaseStringUTFChars( str, str2 );
		lJNIEnv->DeleteLocalRef( str );
	}

	vm->DetachCurrentThread();

	return retstr;
}

int agk::PlatformGetInAppPurchaseSubPlanDuration( int iID, int planIndex, int periodIndex )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetPlanDuration","(III)I" );

	int result = 0;
	if ( method ) result = lJNIEnv->CallStaticIntMethod( sdkClass, method, iID, planIndex, periodIndex );

	vm->DetachCurrentThread();

	return result;
}

char* agk::PlatformGetInAppPurchaseSubPlanDurationUnit( int iID, int planIndex, int periodIndex )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetPlanDurationUnit","(III)Ljava/lang/String;" );

	char* retstr;
	if ( !method )
	{
		retstr = new char[1];
		*retstr = 0;
	}
	else
	{
		jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, iID, planIndex, periodIndex );
		jboolean bCopy;
		const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

		retstr = new char[ strlen(str2) + 1 ];
		strcpy( retstr, str2 );
			
		lJNIEnv->ReleaseStringUTFChars( str, str2 );
		lJNIEnv->DeleteLocalRef( str );
	}

	vm->DetachCurrentThread();

	return retstr;
}

int agk::PlatformGetInAppPurchaseSubPlanPaymentType( int iID, int planIndex, int periodIndex )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetPlanPaymentType","(III)I" );

	int result = 0;
	if ( method ) result = lJNIEnv->CallStaticIntMethod( sdkClass, method, iID, planIndex, periodIndex );

	vm->DetachCurrentThread();

	return result;
}

char* agk::PlatformGetInAppPurchaseSubPlanTags( int iID, int planIndex )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetPlanTags","(II)Ljava/lang/String;" );

	char* retstr;
	if ( !method )
	{
		retstr = new char[1];
		*retstr = 0;
	}
	else
	{
		jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, iID, planIndex );
		jboolean bCopy;
		const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

		retstr = new char[ strlen(str2) + 1 ];
		strcpy( retstr, str2 );
			
		lJNIEnv->ReleaseStringUTFChars( str, str2 );
		lJNIEnv->DeleteLocalRef( str );
	}

	vm->DetachCurrentThread();

	return retstr;
}

char* agk::PlatformGetInAppPurchaseSubPlanToken( int iID, int planIndex )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/InAppPurchase" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "iapGetPlanToken","(II)Ljava/lang/String;" );

	char* retstr;
	if ( !method )
	{
		retstr = new char[1];
		*retstr = 0;
	}
	else
	{
		jstring str = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, iID, planIndex );
		jboolean bCopy;
		const char* str2 = lJNIEnv->GetStringUTFChars( str, &bCopy );

		retstr = new char[ strlen(str2) + 1 ];
		strcpy( retstr, str2 );
			
		lJNIEnv->ReleaseStringUTFChars( str, str2 );
		lJNIEnv->DeleteLocalRef( str );
	}

	vm->DetachCurrentThread();

	return retstr;
}

// ADMOB COMMANDS
void agk::LoadConsentStatusAdMob( const char* szPubID, const char* privacyPolicy )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdConsentSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "LoadAdMobConsentStatus","(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V" );

	jstring strID = lJNIEnv->NewStringUTF( szPubID );
	jstring strPrivacy = lJNIEnv->NewStringUTF( privacyPolicy );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, strID, strPrivacy );
	lJNIEnv->DeleteLocalRef( strPrivacy );
	lJNIEnv->DeleteLocalRef( strID );
	
	vm->DetachCurrentThread();
}

int agk::GetConsentStatusAdMob()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdConsentSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "GetAdMobConsentStatus","(Landroid/app/Activity;)I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method, lNativeActivity );
	
	vm->DetachCurrentThread();
	return result;
}

void agk::RequestConsentAdMob()
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdConsentSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "RequestAdMobConsent","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity );
	
	vm->DetachCurrentThread();
}

void agk::OverrideConsentAdMob( int consent )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdConsentSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "OverrideAdMobConsent","(Landroid/app/Activity;I)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, consent );
	
	vm->DetachCurrentThread();
}

void agk::OverrideConsentChartboost( int consent )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "OverrideChartboostConsent","(Landroid/app/Activity;I)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, consent );
	
	vm->DetachCurrentThread();
}

// szID: publisher ID provided by AdMob
// horz: 0=left, 1=center, 2=right
// vert: 0=top, 1=center, 2=right
// offsetX: AGK distance to offset the horz position from the left or right edge. Ignored when centered
// offsetY: AGK distance to offset the vert position from the top or bottom edge. Ignored when centered
void agk::PlatformAdMobSetupRelative( const char* szID, int horz, int vert, float offsetX, float offsetY, int type )
{
	g_fAdvertOffsetX = offsetX;
	g_fAdvertOffsetY = offsetY;
	g_iAdvertHorz = horz;
	g_iAdvertVert = vert;
	
	int iOffsetX = agk::ScreenToDeviceX( offsetX );
	int iOffsetY = agk::ScreenToDeviceY( offsetY );
	
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID createAd = lJNIEnv->GetStaticMethodID( sdkClass, "CreateAd","(Landroid/app/Activity;Ljava/lang/String;IIIII)V" );

	// call our java class method
	jstring strID = lJNIEnv->NewStringUTF(szID);
	lJNIEnv->CallStaticVoidMethod( sdkClass, createAd, lNativeActivity, strID, horz, vert, iOffsetX, iOffsetY, type );
	lJNIEnv->DeleteLocalRef( strID );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
	
	g_bAdvertActive = true;
}

void agk::PlatformAdMobFullscreen()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID createAd = lJNIEnv->GetStaticMethodID( sdkClass, "CreateFullscreenAd","(Landroid/app/Activity;Ljava/lang/String;)V" );

	// call our java class method
	jstring strID = lJNIEnv->NewStringUTF(m_sAdMobCode.GetStr());
	lJNIEnv->CallStaticVoidMethod( sdkClass, createAd, lNativeActivity, strID );
	lJNIEnv->DeleteLocalRef( strID );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformAdMobCacheFullscreen()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID createAd = lJNIEnv->GetStaticMethodID( sdkClass, "CacheFullscreenAd","(Landroid/app/Activity;Ljava/lang/String;)V" );

	// call our java class method
	jstring strID = lJNIEnv->NewStringUTF(m_sAdMobCode.GetStr());
	lJNIEnv->CallStaticVoidMethod( sdkClass, createAd, lNativeActivity, strID );
	lJNIEnv->DeleteLocalRef( strID );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void  agk::PlatformSetAdMobVisible ( int iVisible )
{
	if ( g_bAdvertActive )
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		// get NativeActivity object (clazz)
		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		// get the method from our java class
		jmethodID refreshAd = lJNIEnv->GetStaticMethodID( sdkClass, "SetAdVisible","(Landroid/app/Activity;I)V" );

		// call our java class method
		lJNIEnv->CallStaticVoidMethod( sdkClass, refreshAd, lNativeActivity, iVisible );

		// detatch thread from Java VM before we leave
		vm->DetachCurrentThread();
	}
}

void  agk::PlatformAdMobPosition ( int horz, int vert, float offsetX, float offsetY )
{
	if ( g_bAdvertActive )
	{
		g_fAdvertOffsetX = offsetX;
		g_fAdvertOffsetY = offsetY;
		g_iAdvertHorz = horz;
		g_iAdvertVert = vert;
		
		int iOffsetX = agk::ScreenToDeviceX( offsetX );
		int iOffsetY = agk::ScreenToDeviceY( offsetY );
		
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		jmethodID PositionAd = lJNIEnv->GetStaticMethodID( sdkClass, "PositionAd","(Landroid/app/Activity;IIII)V" );
		lJNIEnv->CallStaticVoidMethod( sdkClass, PositionAd, lNativeActivity, g_iAdvertHorz, g_iAdvertVert, iOffsetX, iOffsetY );
		vm->DetachCurrentThread();
	}
}

void agk::PlatformAdMobRequestNewAd( void )
{
	if ( g_bAdvertActive )
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		// get NativeActivity object (clazz)
		jobject lNativeActivity = g_pActivity->clazz;
		if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
		
		jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
		if ( !sdkClass ) 
		{
			vm->DetachCurrentThread();
			return;
		}

		// get the method from our java class
		jmethodID refreshAd = lJNIEnv->GetStaticMethodID( sdkClass, "RefreshAd","(Landroid/app/Activity;)V" );

		// call our java class method
		lJNIEnv->CallStaticVoidMethod( sdkClass, refreshAd, lNativeActivity );

		// detatch thread from Java VM before we leave
		vm->DetachCurrentThread();
	}
}

void agk::PlatformAdMobDestroy( void )
{
	if ( !g_bAdvertActive ) return;

	g_bAdvertActive = false;
	
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID deleteAd = lJNIEnv->GetStaticMethodID( sdkClass, "DeleteAd","(Landroid/app/Activity;)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, deleteAd, lNativeActivity );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

bool agk::PlatformHasAdMob( void )
{	
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	
	vm->DetachCurrentThread();
	
	return sdkClass != 0;
}

int agk::PlatformAdMobGetFullscreenLoaded()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// get the method from our java class
	jmethodID createAd = lJNIEnv->GetStaticMethodID( sdkClass, "GetFullscreenLoadedAdMob","()I" );

	// call our java class method
	int loaded = lJNIEnv->CallStaticIntMethod( sdkClass, createAd );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return loaded;
}

void agk::PlatformAdMobRewardAd()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "ShowRewardAd","(Landroid/app/Activity;Ljava/lang/String;)V" );

	// call our java class method
	jstring strID = lJNIEnv->NewStringUTF(m_sAdMobRewardAdCode.GetStr());
	lJNIEnv->CallStaticVoidMethod( sdkClass, adFunc, lNativeActivity, strID );
	lJNIEnv->DeleteLocalRef( strID );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformAdMobCacheRewardAd()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "CacheRewardAd","(Landroid/app/Activity;Ljava/lang/String;)V" );

	// call our java class method
	jstring strID = lJNIEnv->NewStringUTF(m_sAdMobRewardAdCode.GetStr());
	lJNIEnv->CallStaticVoidMethod( sdkClass, adFunc, lNativeActivity, strID );
	lJNIEnv->DeleteLocalRef( strID );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

int agk::PlatformAdMobGetRewardAdLoaded()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "GetRewardAdLoadedAdMob","()I" );

	// call our java class method
	int loaded = lJNIEnv->CallStaticIntMethod( sdkClass, adFunc );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return loaded;
}

int agk::PlatformAdMobGetRewardAdRewarded()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "GetRewardAdRewarded","()I" );

	// call our java class method
	int rewarded = lJNIEnv->CallStaticIntMethod( sdkClass, adFunc );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return rewarded;
}

int agk::PlatformAdMobGetRewardAdValue()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "GetRewardAdValue","()I" );
	if ( !adFunc ) return 0;

	// call our java class method
	int value = lJNIEnv->CallStaticIntMethod( sdkClass, adFunc );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return value;
}

void agk::PlatformAdMobResetRewardAd()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "ResetRewardAd","()V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, adFunc );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformAdMobSetTesting (int testing)
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdMobSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "SetAdMobTestMode","(I)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, adFunc, testing );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformAdMobSetChildRating( int rating )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/AdConsentSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "SetAdMobChildRating","(I)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, adFunc, rating );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

// CHARTBOOST

void agk::PlatformChartboostSetup()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	// get the method from our java class
	jmethodID createAd = lJNIEnv->GetStaticMethodID( sdkClass, "SetChartboostDetails","(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V" );

	// call our java class method
	jstring strID = lJNIEnv->NewStringUTF(m_sChartboostCode1.GetStr());
	jstring strID2 = lJNIEnv->NewStringUTF(m_sChartboostCode2.GetStr());
	lJNIEnv->CallStaticVoidMethod( sdkClass, createAd, lNativeActivity, strID, strID2 );
	lJNIEnv->DeleteLocalRef( strID );
	lJNIEnv->DeleteLocalRef( strID2 );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformChartboostFullscreen()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}	

	// get the method from our java class
	jmethodID createAd = lJNIEnv->GetStaticMethodID( sdkClass, "CreateFullscreenAdChartboost","(Landroid/app/Activity;I)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, createAd, lNativeActivity, 0 );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

int agk::PlatformChartboostGetFullscreenLoaded()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}	

	// get the method from our java class
	jmethodID createAd = lJNIEnv->GetStaticMethodID( sdkClass, "GetFullscreenLoadedChartboost","()I" );

	// call our java class method
	int loaded = lJNIEnv->CallStaticIntMethod( sdkClass, createAd );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return loaded;
}

void agk::PlatformChartboostRewardAd()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "ShowRewardAdChartboost","(Landroid/app/Activity;)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, adFunc, lNativeActivity );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformChartboostCacheRewardAd()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "CacheRewardAdChartboost","(Landroid/app/Activity;)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, adFunc, lNativeActivity );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

int agk::PlatformChartboostGetRewardAdLoaded()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "GetRewardAdLoadedChartboost","()I" );

	// call our java class method
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, adFunc );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return result;
}

int agk::PlatformChartboostGetRewardAdRewarded()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "GetRewardAdRewardedChartboost","()I" );

	// call our java class method
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, adFunc );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();

	return result;
}

void agk::PlatformChartboostResetRewardAd()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ChartboostSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID adFunc = lJNIEnv->GetStaticMethodID( sdkClass, "ResetRewardAdChartboost","()V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( sdkClass, adFunc );
	
	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

// FACEBOOK COMMANDS
void  agk::PlatformFacebookSetup                ( const char* szID )
{
	
}

void  agk::PlatformFacebookLogin                ( void )
{

}

void  agk::PlatformFacebookLogout               ( void )
{

}

void  agk::PlatformFacebookShowLikeButton       ( const char* szURL, int iX, int iY, int iWidth, int iHeight )
{
}

void  agk::PlatformFacebookDestroyLikeButton    ( void )
{
}

void  agk::PlatformFacebookPostOnMyWall         ( const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription )
{

}

void  agk::PlatformFacebookPostOnFriendsWall    ( const char* szID, const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription )
{

}

void  agk::PlatformFacebookInviteFriend         ( const char* szID, const char* szMessage )
{
}

void  agk::PlatformFacebookGetFriends           ( void )
{

}

int   agk::PlatformFacebookGetFriendsState      ( void )
{
	return 1;
}

int   agk::PlatformFacebookGetFriendsCount      ( void )
{
	return m_iFBFriendCount-1;
}

char* agk::PlatformFacebookGetFriendsName       ( int iIndex )
{
	char *str = new char[ 1 ];
	*str = 0;
	return str;
}

char* agk::PlatformFacebookGetFriendsID         ( int iIndex )
{
	char *str = new char[ 1 ];
	*str = 0;
	return str;
}

void  agk::PlatformFacebookDownloadFriendsPhoto ( int iIndex )
{
}

int   agk::PlatformGetFacebookDownloadState     ( void )
{
	return m_iFBGettingPicture;
}

char* agk::PlatformGetFacebookDownloadFile      ( void )
{
	char* str = new char[1]; *str = 0;
	return str;
}

int   agk::PlatformGetFacebookLoggedIn          ( void )
{
	return 0;
}

bool  agk::PlatformHasFacebook             ( void )
{
	return false;
}

char* agk::PlatformFacebookGetUserID			  ( void )
{
	char* str = new char[1]; *str = 0;
	return str;
}

char* agk::PlatformFacebookGetUserName		  ( void )
{
	char* str = new char[1]; *str = 0;
	return str;
}

char* agk::PlatformFacebookGetAccessToken		  ( void )
{
	char* str = new char[1]; *str = 0;
	return str;
}

// TWITTER COMMANDS
void  agk::PlatformTwitterSetup                 ( const char* szKey, const char* szSecret )
{
}

void  agk::PlatformTwitterMessage               ( const char* szMessage )
{
}

bool  agk::PlatformHasTwitter             ( void )
{
	return false;
}

// local notifications

void agk::PlatformCreateLocalNotification( int iID, int datetime, const char *szMessage, const char *szDeepLink )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID setNotif = lJNIEnv->GetStaticMethodID( AGKHelper, "SetNotification","(Landroid/app/Activity;IILjava/lang/String;Ljava/lang/String;)V" );

	// call our java class method
	jstring strMsg = lJNIEnv->NewStringUTF(szMessage);
	jstring strDeepLink = lJNIEnv->NewStringUTF(szDeepLink);
	lJNIEnv->CallStaticVoidMethod( AGKHelper, setNotif, lNativeActivity, iID, datetime, strMsg, strDeepLink );
	lJNIEnv->DeleteLocalRef( strDeepLink );
	lJNIEnv->DeleteLocalRef( strMsg );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

void agk::PlatformCancelLocalNotification( int iID )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID cancelNotif = lJNIEnv->GetStaticMethodID( AGKHelper, "CancelNotification","(Landroid/app/Activity;I)V" );

	// call our java class method
	lJNIEnv->CallStaticVoidMethod( AGKHelper, cancelNotif, lNativeActivity, iID );

	// detatch thread from Java VM before we leave
	vm->DetachCurrentThread();
}

// V108 ULTRABOOK COMMANDS

int agk::GetNotificationType()
{
	return 0;
}

void agk::SetNotificationImage(int iImageIndex)
{
}

void agk::SetNotificationText(const char* pText)
{
}

int agk::GetNFCExists()
{
	return 0;
}

uint32_t agk::GetRawNFCCount()
{
	return 0;
}

uint32_t agk::GetRawFirstNFCDevice()
{
	return 0;
}

uint32_t agk::GetRawNextNFCDevice()
{
	return 0;
}

char* agk::GetRawNFCName(uint32_t iIndex)
{
	return 0;
}

int agk::SendRawNFCData(uint32_t iIndex, const char* pMessageText)
{
	return 0;
}

int agk::GetRawNFCDataState(uint32_t iIndex)
{
	return 0;
}

char* agk::GetRawNFCData(uint32_t iIndex)
{
	return 0;
}

void agk::StartGPSTracking()
{
	if ( CheckPermission("Location") != 2 )
	{
		agk::Error( "The app does not have permission to get the GPS location, use RequestPermission first" );
		return;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/LocationSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID StartGPS = lJNIEnv->GetStaticMethodID( sdkClass, "StartGPSTracking","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, StartGPS, lNativeActivity );

	jmethodID GetGPS = lJNIEnv->GetStaticMethodID( sdkClass, "GetGPSLatitude","()F" );
	m_fGPSLat = lJNIEnv->CallStaticFloatMethod( sdkClass, GetGPS );

	jmethodID GetGPS2 = lJNIEnv->GetStaticMethodID( sdkClass, "GetGPSLongitude","()F" );
	m_fGPSLong = lJNIEnv->CallStaticFloatMethod( sdkClass, GetGPS2 );

	vm->DetachCurrentThread();
}

void agk::StopGPSTracking()
{
	if ( CheckPermission("Location") != 2 )
	{
		return;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/LocationSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID StopGPS = lJNIEnv->GetStaticMethodID( sdkClass, "StopGPSTracking","()V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, StopGPS );

	vm->DetachCurrentThread();
}

float agk::GetRawGPSLatitude()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/LocationSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID GetGPS = lJNIEnv->GetStaticMethodID( sdkClass, "GetGPSLatitude","()F" );
	m_fGPSLat = lJNIEnv->CallStaticFloatMethod( sdkClass, GetGPS );

	vm->DetachCurrentThread();

	return m_fGPSLat;
}

float agk::GetRawGPSLongitude()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/LocationSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID GetGPS2 = lJNIEnv->GetStaticMethodID( sdkClass, "GetGPSLongitude","()F" );
	m_fGPSLong = lJNIEnv->CallStaticFloatMethod( sdkClass, GetGPS2 );

	vm->DetachCurrentThread();

	return m_fGPSLong;
}

float agk::GetRawGPSAltitude()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/LocationSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID GetGPS3 = lJNIEnv->GetStaticMethodID( sdkClass, "GetGPSAltitude","()F" );
	m_fGPSAltitude = lJNIEnv->CallStaticFloatMethod( sdkClass, GetGPS3 );

	vm->DetachCurrentThread();

	return m_fGPSAltitude;
}

// GameCenter

int agk::GetGameCenterExists()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID GameCenterExists = lJNIEnv->GetStaticMethodID( sdkClass, "GetGameCenterExists","(Landroid/app/Activity;)I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, GameCenterExists, lNativeActivity );

	vm->DetachCurrentThread();

	return result;
}

void agk::GameCenterSetup()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID GameCenterSetup = lJNIEnv->GetStaticMethodID( sdkClass, "GameCenterSetup","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, GameCenterSetup, lNativeActivity );

	vm->DetachCurrentThread();
}

void agk::GameCenterLogin()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID GameCenterLogin = lJNIEnv->GetStaticMethodID( sdkClass, "GameCenterLogin","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, GameCenterLogin, lNativeActivity );

	vm->DetachCurrentThread();
}

void agk::GameCenterLogout()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "GameCenterLogout","()V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity );

	vm->DetachCurrentThread();
}

int agk::GetGameCenterLoggedIn()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID GameCenterLoggedIn = lJNIEnv->GetStaticMethodID( sdkClass, "GetGameCenterLoggedIn","()I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, GameCenterLoggedIn );

	vm->DetachCurrentThread();

	return result;
}

char* agk::GetGameCenterPlayerID()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1]; *str = 0;
		return str;
	}

	jmethodID GameCenterPlayerID = lJNIEnv->GetStaticMethodID( sdkClass, "GetGameCenterPlayerID","()Ljava/lang/String;" );
	jstring playerid = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, GameCenterPlayerID );

	jboolean bCopy;
	const char* str = lJNIEnv->GetStringUTFChars( playerid, &bCopy );

	char *szPlayerID = new char[ strlen(str)+1 ];
	strcpy( szPlayerID, str );

	lJNIEnv->ReleaseStringUTFChars( playerid, str );
	lJNIEnv->DeleteLocalRef( playerid );

	vm->DetachCurrentThread();

	return szPlayerID;
}

char* agk::GetGameCenterPlayerDisplayName()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1]; *str = 0;
		return str;
	}

	jmethodID GameCenterPlayerName = lJNIEnv->GetStaticMethodID( sdkClass, "GetGameCenterPlayerDisplayName","()Ljava/lang/String;" );
	jstring playername = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, GameCenterPlayerName );

	jboolean bCopy;
	const char* str = lJNIEnv->GetStringUTFChars( playername, &bCopy );

	char *szPlayerName = new char[ strlen(str)+1 ];
	strcpy( szPlayerName, str );

	lJNIEnv->ReleaseStringUTFChars( playername, str );
	lJNIEnv->DeleteLocalRef( playername );

	vm->DetachCurrentThread();

	return szPlayerName;
}

void agk::GameCenterSubmitScore( int iScore, const char* szBoardID )
{
	if ( !szBoardID || !*szBoardID )
	{
		agk::Error( "Leaderboard ID cannot be an empty string" );
		return;
	}

	char *szPackage = agk::GetAppPackageName();
	if ( strcmp(szPackage, "com.thegamecreators.agk_player2") == 0 )
	{
		if ( strcmp(szBoardID, "CgkI5Zjo8fsbEAIQBg") != 0 )
		{
			agk::Warning( "Using GameCenterSubmitScore when broadcasting must use BoardID CgkI5Zjo8fsbEAIQBg for testing, export your app with your Google Play Games ID to use your own leaderboards" );
			return;
		}
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID GameCenterSubmitScore = lJNIEnv->GetStaticMethodID( sdkClass, "GameCenterSubmitScore","(Ljava/lang/String;I)V" );

	jstring strBoard = lJNIEnv->NewStringUTF( szBoardID ? szBoardID : "" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, GameCenterSubmitScore, strBoard, iScore );
	lJNIEnv->DeleteLocalRef( strBoard );

	vm->DetachCurrentThread();
}

void agk::GameCenterShowLeaderBoard ( const char* szBoardID )
{
	if ( !szBoardID || !*szBoardID )
	{
		agk::Error( "Leaderboard ID cannot be an empty string" );
		return;
	}

	char *szPackage = agk::GetAppPackageName();
	if ( strcmp(szPackage, "com.thegamecreators.agk_player2") == 0 )
	{
		if ( strcmp(szBoardID, "CgkI5Zjo8fsbEAIQBg") != 0 )
		{
			agk::Warning( "Using GameCenterShowLeaderBoard when broadcasting must use BoardID CgkI5Zjo8fsbEAIQBg for testing, export your app with your Google Play Games ID to use your own leaderboards" );
			return;
		}
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID GameCenterShowLeaderBoard = lJNIEnv->GetStaticMethodID( sdkClass, "GameCenterShowLeaderBoard","(Landroid/app/Activity;Ljava/lang/String;)V" );

	jstring strBoard = lJNIEnv->NewStringUTF( szBoardID ? szBoardID : "" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, GameCenterShowLeaderBoard, lNativeActivity, strBoard );
	lJNIEnv->DeleteLocalRef( strBoard );

	vm->DetachCurrentThread();
}

void agk::GameCenterSubmitAchievement ( const char* szAchievementID, int iPercentageComplete )
{
	if ( !szAchievementID || !*szAchievementID )
	{
		agk::Error( "Achievement ID cannot be an empty string" );
		return;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID GameCenterSubmitAchievement = lJNIEnv->GetStaticMethodID( sdkClass, "GameCenterSubmitAchievement","(Ljava/lang/String;I)V" );

	jstring strAchievement = lJNIEnv->NewStringUTF( szAchievementID ? szAchievementID : "" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, GameCenterSubmitAchievement, strAchievement, iPercentageComplete );
	lJNIEnv->DeleteLocalRef( strAchievement );

	vm->DetachCurrentThread();
}

void agk::GameCenterAchievementsShow ( void )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/GameCenterSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID GameCenterAchievementsShow = lJNIEnv->GetStaticMethodID( sdkClass, "GameCenterAchievementsShow","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, GameCenterAchievementsShow, lNativeActivity );

	vm->DetachCurrentThread();
}

void agk::GameCenterAchievementsReset ( void )
{
	
}

int agk::PlatformGetIP( uString &sIP )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID getip = lJNIEnv->GetStaticMethodID( AGKHelper, "GetIP", "(Landroid/app/Activity;)Ljava/lang/String;" );

	// call our java class method
	jstring ip = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, getip, lNativeActivity );

	jboolean bCopy;
	const char* szIP = lJNIEnv->GetStringUTFChars( ip, &bCopy );

	sIP.SetStr( szIP );

	lJNIEnv->ReleaseStringUTFChars( ip, szIP );
	lJNIEnv->DeleteLocalRef( ip );

	vm->DetachCurrentThread();

	if ( sIP.GetLength() == 0 ) return 0;
	return 1;
}

int agk::PlatformGetIPv6( uString &sIP, int *iInterface )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID getip = lJNIEnv->GetStaticMethodID( AGKHelper, "GetIPv6", "(Landroid/app/Activity;)Ljava/lang/String;" );

	// call our java class method
	jstring ip = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, getip, lNativeActivity );

	jboolean bCopy;
	const char* szIP = lJNIEnv->GetStringUTFChars( ip, &bCopy );

	sIP.SetStr( szIP );

	lJNIEnv->ReleaseStringUTFChars( ip, szIP );
	lJNIEnv->DeleteLocalRef( ip );

	vm->DetachCurrentThread();

	if ( sIP.GetLength() == 0 ) return 0;

	int pos = sIP.Find( '%' );
	if ( pos > 0 )
	{
		int index = if_nametoindex( sIP.GetStr() + pos + 1 );
		sIP.Trunc( '%' );
		
		if ( iInterface ) *iInterface = index;
	}

	return 1;
}

int agk::CheckPermission( const char* szPermission )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PermissionSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "CheckPermission", "(Landroid/app/Activity;Ljava/lang/String;)I" );

	// call our java class method
	jstring str = lJNIEnv->NewStringUTF( szPermission );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method, lNativeActivity, str );
	lJNIEnv->DeleteLocalRef( str );

	vm->DetachCurrentThread();

	return result;
}

void agk::RequestPermission( const char* szPermission )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");

	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/PermissionSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "RequestPermission", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	// call our java class method
	jstring str = lJNIEnv->NewStringUTF( szPermission );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, str );
	lJNIEnv->DeleteLocalRef( str );

	vm->DetachCurrentThread();
}

// Cloud data

void agk::SetupCloudData( const char* reserved )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CloudDataSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID networkAvailable = lJNIEnv->GetStaticMethodID( sdkClass, "SetupCloudData","(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, networkAvailable, lNativeActivity );

	vm->DetachCurrentThread();
}

int agk::GetCloudDataAllowed()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CloudDataSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID networkAvailable = lJNIEnv->GetStaticMethodID( sdkClass, "GetCloudDataAllowed","(Landroid/app/Activity;)I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, networkAvailable, lNativeActivity );

	vm->DetachCurrentThread();
	return result;
}

int agk::GetCloudDataChanged()
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CloudDataSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID networkAvailable = lJNIEnv->GetStaticMethodID( sdkClass, "GetCloudDataChanged","()I" );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, networkAvailable, lNativeActivity );

	vm->DetachCurrentThread();
	return result;
}

char* agk::GetCloudDataVariable( const char* varName, const char* defaultValue )
{
	if ( !varName || !defaultValue )
	{
		char *str = new char[1];
		*str = 0;
		return str;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CloudDataSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char *str = new char[strlen(defaultValue)+1];
		strcpy( str, defaultValue );
		return str;
	}

	// get the method from our java class
	jmethodID loadVar = lJNIEnv->GetStaticMethodID( sdkClass, "GetCloudDataVariable", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;" );

	// call our java class method
	jstring strName = lJNIEnv->NewStringUTF( varName );
	jstring strDefaultValue = lJNIEnv->NewStringUTF( defaultValue );
	jstring value = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, loadVar, lNativeActivity, strName, strDefaultValue );
	lJNIEnv->DeleteLocalRef( strDefaultValue );
	lJNIEnv->DeleteLocalRef( strName );

	jboolean bCopy;
	const char* szValue = lJNIEnv->GetStringUTFChars( value, &bCopy );

	char *str = new char[ strlen(szValue)+1 ];
	strcpy( str, szValue );

	lJNIEnv->ReleaseStringUTFChars( value, szValue );
	lJNIEnv->DeleteLocalRef( value );

	vm->DetachCurrentThread();
	
	return str;
}

void agk::SetCloudDataVariable( const char* varName, const char* varValue )
{
	if ( !varName || !varValue ) return;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CloudDataSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID saveVar = lJNIEnv->GetStaticMethodID( sdkClass, "SetCloudDataVariable", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V" );

	// call our java class method
	jstring strName = lJNIEnv->NewStringUTF( varName );
	jstring strValue = lJNIEnv->NewStringUTF( varValue );
	lJNIEnv->CallStaticVoidMethod( sdkClass, saveVar, lNativeActivity, strName, strValue );
	lJNIEnv->DeleteLocalRef( strValue );
	lJNIEnv->DeleteLocalRef( strName );

	vm->DetachCurrentThread();
}

void agk::DeleteCloudDataVariable( const char* varName )
{
	if ( !varName ) return;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/CloudDataSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// get the method from our java class
	jmethodID deleteVar = lJNIEnv->GetStaticMethodID( sdkClass, "DeleteCloudDataVariable", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	// call our java class method
	jstring strName = lJNIEnv->NewStringUTF( varName );
	lJNIEnv->CallStaticVoidMethod( sdkClass, deleteVar, lNativeActivity, strName );
	lJNIEnv->DeleteLocalRef( strName );

	vm->DetachCurrentThread();
}

// Shared variables

void agk::SetSharedVariableAppGroup( const char* group )
{
	
}

void agk::SaveSharedVariable( const char *varName, const char *varValue )
{
	if ( !varName || !varValue ) return;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID saveVar = lJNIEnv->GetStaticMethodID( AGKHelper, "SaveSharedVariable", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V" );

	// call our java class method
	jstring strName = lJNIEnv->NewStringUTF( varName );
	jstring strValue = lJNIEnv->NewStringUTF( varValue );
	lJNIEnv->CallStaticVoidMethod( AGKHelper, saveVar, lNativeActivity, strName, strValue );
	lJNIEnv->DeleteLocalRef( strValue );
	lJNIEnv->DeleteLocalRef( strName );

	vm->DetachCurrentThread();
}

char* agk::LoadSharedVariable( const char *varName, const char *defaultValue )
{
	if ( !varName || !defaultValue )
	{
		char *str = new char[1];
		*str = 0;
		return str;
	}

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID loadVar = lJNIEnv->GetStaticMethodID( AGKHelper, "LoadSharedVariable", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;" );

	// call our java class method
	jstring strName = lJNIEnv->NewStringUTF( varName );
	jstring strDefaultValue = lJNIEnv->NewStringUTF( defaultValue );
	jstring value = (jstring) lJNIEnv->CallStaticObjectMethod( AGKHelper, loadVar, lNativeActivity, strName, strDefaultValue );
	lJNIEnv->DeleteLocalRef( strDefaultValue );
	lJNIEnv->DeleteLocalRef( strName );

	jboolean bCopy;
	const char* szValue = lJNIEnv->GetStringUTFChars( value, &bCopy );

	char *str = new char[ strlen(szValue)+1 ];
	strcpy( str, szValue );

	lJNIEnv->ReleaseStringUTFChars( value, szValue );
	lJNIEnv->DeleteLocalRef( value );

	vm->DetachCurrentThread();
	
	return str;
}

void agk::DeleteSharedVariable( const char *varName )
{
	if ( !varName ) return;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID deleteVar = lJNIEnv->GetStaticMethodID( AGKHelper, "DeleteSharedVariable", "(Landroid/app/Activity;Ljava/lang/String;)V" );

	// call our java class method
	jstring strName = lJNIEnv->NewStringUTF( varName );
	lJNIEnv->CallStaticVoidMethod( AGKHelper, deleteVar, lNativeActivity, strName );
	lJNIEnv->DeleteLocalRef( strName );

	vm->DetachCurrentThread();
}

void agk::FirebaseSetup() 
{
	static int configured = 0;
	if ( configured ) return;
	configured = 1;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/FirebaseSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}
	
	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "FirebaseInit", "(Landroid/app/Activity;)V" );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity );
	
	vm->DetachCurrentThread();
}

void agk::FirebaseLogEvent( const char *event_name )
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/FirebaseSDK" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "FirebaseLogEvent", "(Ljava/lang/String;)V" );

	// call our java class method
	jstring strName = lJNIEnv->NewStringUTF( event_name );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, strName );
	lJNIEnv->DeleteLocalRef( strName );

	vm->DetachCurrentThread();
}

int AGKFont::PlatformGetSystemFontPath( const uString &sFontName, uString &sOut )
{
	uString sFontNameShort = sFontName;
	sFontNameShort.Trunc( '.' );
	sFontNameShort.AppendUTF8( ".ttf" );

	sFontNameShort.Prepend( "/system/fonts/" );

	FILE *pFile = AGKfopen( sFontNameShort, "rb" );
	if ( pFile ) 
	{
		fclose(pFile);
		sOut.SetStr( sFontNameShort );
		return 1;
	}

	return 0;
}

// AR commands
#ifdef AGK_USE_AR
	class AGKAndroidARAnchor : public cNode
	{
		public:
			ArAnchor *m_pARAnchor;

			AGKAndroidARAnchor() { m_pARAnchor = 0; }
			~AGKAndroidARAnchor() { if ( m_pARAnchor ) ArAnchor_release(m_pARAnchor); }

			void Update() 
			{
				if ( !m_pARAnchor ) return;

				ArTrackingState tracking_state = AR_TRACKING_STATE_STOPPED;
				ArAnchor_getTrackingState( g_pARSession, m_pARAnchor, &tracking_state );
				if (tracking_state != AR_TRACKING_STATE_TRACKING) return;

				ArPose *pPose = 0;
				ArPose_create( g_pARSession, 0, &pPose );
				ArAnchor_getPose( g_pARSession, m_pARAnchor, pPose );
				float poseRaw[7];
				ArPose_getPoseRaw( g_pARSession, pPose, poseRaw );
				ArPose_destroy( pPose );

				SetNodePosition( poseRaw[4], poseRaw[5], -poseRaw[6] );
				SetNodeRotation( -poseRaw[3], poseRaw[0], poseRaw[1], -poseRaw[2] );
			}
	};

	cHashedList<AGKAndroidARAnchor*> g_pARAnchorList(256);

	extern "C" 
	{
		ArStatus (*fpArCoreApk_requestInstallCustom)( void* env, void* activity, int user_requested_install, ArInstallBehavior install_behavior, ArInstallUserMessageType message_type, ArInstallStatus *out_install_status ) = 0;

		//ArStatus (*fpArSession_checkSupported)( const ArSession* session, const ArConfig* config ) = 0;
		ArStatus (*fpArSession_configure)( ArSession* session, const ArConfig* config ) = 0;
		ArStatus (*fpArSession_create)( void *env, void *application_context, ArSession **out_session_pointer) = 0;
		void (*fpArSession_setDisplayGeometry)( ArSession *session, int rotation, int width, int height) = 0;
		void (*fpArSession_setCameraTextureName)( ArSession *session, unsigned int texture_id ) = 0;
		ArStatus (*fpArSession_update)( ArSession *session, ArFrame *out_frame ) = 0;
		ArStatus (*fpArSession_pause)( ArSession *session ) = 0;
		ArStatus (*fpArSession_resume)( ArSession *session ) = 0;
		void (*fpArSession_destroy)( ArSession *session ) = 0;
		void (*fpArSession_getAllTrackables)( const ArSession *session, ArTrackableType filter_type, ArTrackableList *out_trackable_list ) = 0;
		ArStatus (*fpArSession_acquireNewAnchor)( ArSession *session, const ArPose *pose, ArAnchor **out_anchor ) = 0;
	
		void (*fpArConfig_create)( const ArSession* session, ArConfig** config ) = 0;
		void (*fpArConfig_destroy)( ArConfig *config ) = 0;
	
		void (*fpArFrame_create)( const ArSession *session, ArFrame **out_frame) = 0;
		void (*fpArFrame_destroy)( ArFrame *frame ) = 0;
		void (*fpArFrame_acquireCamera)( const ArSession *session, const ArFrame *frame, ArCamera **out_camera ) = 0;
		void (*fpArFrame_hitTest)( const ArSession *session, const ArFrame *frame, float pixel_x, float pixel_y, ArHitResultList *hit_result_list ) = 0;
		void (*fpArFrame_getLightEstimate)( const ArSession *session, const ArFrame *frame, ArLightEstimate *out_light_estimate ) = 0;
	
		void (*fpArPose_create)( const ArSession *session, const float *pose_raw, ArPose **out_pose ) = 0;
		void (*fpArPose_getPoseRaw)( const ArSession *session, const ArPose *pose, float *out_pose_raw ) = 0;
		void (*fpArPose_destroy)( ArPose *pose ) = 0;

		void (*fpArCamera_getTrackingState)( const ArSession *session, const ArCamera *camera, ArTrackingState *out_tracking_state) = 0;
		void (*fpArCamera_getDisplayOrientedPose)( const ArSession *session, const ArCamera *camera, ArPose *out_pose ) = 0;
		void (*fpArCamera_getProjectionMatrix)( const ArSession *session, const ArCamera *camera, float near, float far, float *dest_col_major_4x4 ) = 0;
		void (*fpArCamera_release)( ArCamera *camera ) = 0;

		void (*fpArHitResult_create)( const ArSession *session, ArHitResult **out_hit_result ) = 0;
		void (*fpArHitResult_destroy)( ArHitResult *hit_result ) = 0;
		void (*fpArHitResult_getHitPose)( const ArSession *session, const ArHitResult *hit_result, ArPose *out_pose ) = 0;
		void (*fpArHitResult_acquireTrackable)( const ArSession *session, const ArHitResult *hit_result, ArTrackable **out_trackable ) = 0;
		ArStatus (*fpArHitResult_acquireNewAnchor)( ArSession *session, ArHitResult *hit_result, ArAnchor **out_anchor ) = 0;

		void (*fpArHitResultList_create)( const ArSession *session, ArHitResultList **out_hit_result_list ) = 0;
		void (*fpArHitResultList_getItem)( const ArSession *session, const ArHitResultList *hit_result_list, int32_t index, ArHitResult *out_hit_result ) = 0;
		void (*fpArHitResultList_destroy)( ArHitResultList *hit_result_list ) = 0;
		void (*fpArHitResultList_getSize)( const ArSession *session, const ArHitResultList *hit_result_list, int32_t *out_size ) = 0;

		void (*fpArTrackable_getType)( const ArSession *session, const ArTrackable *trackable, ArTrackableType *out_trackable_type ) = 0;
		void (*fpArTrackable_release)( ArTrackable *trackable ) = 0;

		void (*fpArTrackableList_create)( const ArSession *session, ArTrackableList **out_trackable_list ) = 0;
		void (*fpArTrackableList_destroy)( ArTrackableList *trackable_list ) = 0;
		void (*fpArTrackableList_getSize)( const ArSession *session, const ArTrackableList *trackable_list, int32_t *out_size ) = 0;
		void (*fpArTrackableList_acquireItem)( const ArSession *session, const ArTrackableList *trackable_list, int32_t index, ArTrackable **out_trackable ) = 0;
		void (*fpArTrackable_getTrackingState)( const ArSession *session, const ArTrackable *trackable, ArTrackingState *out_tracking_state ) = 0;

		void (*fpArPlane_isPoseInExtents)( const ArSession *session, const ArPlane *plane, const ArPose *pose, int32_t *out_pose_in_extents ) = 0;
		void (*fpArPlane_isPoseInPolygon)( const ArSession *session, const ArPlane *plane, const ArPose *pose, int32_t *out_pose_in_extents ) = 0;
		void (*fpArPlane_getCenterPose)( const ArSession *session, const ArPlane *plane, ArPose *out_pose ) = 0;
		void (*fpArPlane_getExtentX)( const ArSession *session, const ArPlane *plane, float *out_extent_x ) = 0;
		void (*fpArPlane_getExtentZ)( const ArSession *session, const ArPlane *plane, float *out_extent_z ) = 0;
		void (*fpArPlane_acquireSubsumedBy)( const ArSession *session, const ArPlane *plane, ArPlane **out_subsumed_by ) = 0;

		void (*fpArPoint_getOrientationMode)( const ArSession *session, const ArPoint *point, ArPointOrientationMode *out_orientation_mode ) = 0;

		void (*fpArConfig_setUpdateMode)( const ArSession *session, ArConfig *config, ArUpdateMode update_mode ) = 0;
		void (*fpArConfig_setPlaneFindingMode)( const ArSession *session, ArConfig *config, ArPlaneFindingMode plane_finding_mode ) = 0;
		void (*fpArConfig_setLightEstimationMode)( const ArSession *session, ArConfig *config, ArLightEstimationMode light_estimation_mode ) = 0;
		
		void (*fpArLightEstimate_create)(const ArSession *session, ArLightEstimate **out_light_estimate) = 0;
		void (*fpArLightEstimate_getState)(const ArSession *session, const ArLightEstimate *light_estimate, ArLightEstimateState *out_light_estimate_state) = 0;
		void (*fpArLightEstimate_getPixelIntensity)(const ArSession *session, const ArLightEstimate *light_estimate, float *out_pixel_intensity) = 0;
		void (*fpArLightEstimate_destroy)(ArLightEstimate *light_estimate) = 0;
		
		void (*fpArAnchor_getPose)(const ArSession *session, const ArAnchor *anchor, ArPose *out_pose) = 0;
		void (*fpArAnchor_getTrackingState)(const ArSession *session, const ArAnchor *anchor, ArTrackingState *out_tracking_state) = 0;
		void (*fpArAnchor_release)(ArAnchor *anchor) = 0;		

		
		// real functions
		ArStatus ArCoreApk_requestInstallCustom( void* env, void* activity, int user_requested_install, ArInstallBehavior install_behavior, ArInstallUserMessageType message_type, ArInstallStatus* out_install_status ) { 
			return (ArStatus)fpArCoreApk_requestInstallCustom( env, activity, user_requested_install, install_behavior, message_type, out_install_status ); 
		}

		//ArStatus ArSession_checkSupported( const ArSession* session, const ArConfig* config ) { return (ArStatus)fpArSession_checkSupported( session, config ); }
		ArStatus ArSession_configure( ArSession* session, const ArConfig* config ) { return (ArStatus)fpArSession_configure( session, config ); }
		ArStatus ArSession_create( void *env, void *application_context, ArSession **out_session_pointer) { return (ArStatus)fpArSession_create( env, application_context, out_session_pointer); }
		void ArSession_setDisplayGeometry( ArSession *session, int rotation, int width, int height) { fpArSession_setDisplayGeometry( session, rotation, width, height); }
		void ArSession_setCameraTextureName( ArSession *session, unsigned int texture_id ) { fpArSession_setCameraTextureName( session, texture_id ); }
		ArStatus ArSession_update( ArSession *session, ArFrame *out_frame ) { return (ArStatus)fpArSession_update( session, out_frame ); }
		ArStatus ArSession_pause( ArSession *session ) { return (ArStatus)fpArSession_pause( session ); }
		ArStatus ArSession_resume( ArSession *session ) { return (ArStatus)fpArSession_resume( session ); }
		void ArSession_destroy( ArSession *session ) { fpArSession_destroy( session ); }
		void ArSession_getAllTrackables( const ArSession *session, ArTrackableType filter_type, ArTrackableList *out_trackable_list ) { fpArSession_getAllTrackables( session, filter_type, out_trackable_list ); }
		ArStatus ArSession_acquireNewAnchor( ArSession *session, const ArPose *pose, ArAnchor **out_anchor ) { return fpArSession_acquireNewAnchor( session, pose, out_anchor ); }
	
		void ArConfig_create( const ArSession* session, ArConfig** config ) { fpArConfig_create( session, config ); }
		void ArConfig_destroy( ArConfig *config ) { fpArConfig_destroy( config ); }
	
		void ArFrame_create( const ArSession *session, ArFrame **out_frame) { fpArFrame_create( session, out_frame); }
		void ArFrame_destroy( ArFrame *frame ) { fpArFrame_destroy( frame ); }
		void ArFrame_acquireCamera( const ArSession *session, const ArFrame *frame, ArCamera **out_camera ) { fpArFrame_acquireCamera( session, frame, out_camera ); }
		void ArFrame_hitTest( const ArSession *session, const ArFrame *frame, float pixel_x, float pixel_y, ArHitResultList *hit_result_list ) { fpArFrame_hitTest( session, frame, pixel_x, pixel_y, hit_result_list ); }
		void ArFrame_getLightEstimate( const ArSession *session, const ArFrame *frame, ArLightEstimate *out_light_estimate ) { fpArFrame_getLightEstimate( session, frame, out_light_estimate ); }

		void ArPose_create( const ArSession *session, const float *pose_raw, ArPose **out_pose ) { fpArPose_create( session, pose_raw, out_pose ); }
		void ArPose_getPoseRaw( const ArSession *session, const ArPose *pose, float *out_pose_raw ) { fpArPose_getPoseRaw( session, pose, out_pose_raw ); }
		void ArPose_destroy( ArPose *pose ) { fpArPose_destroy( pose ); }

		void ArCamera_getTrackingState( const ArSession *session, const ArCamera *camera, ArTrackingState *out_tracking_state) { fpArCamera_getTrackingState( session, camera, out_tracking_state); }
		void ArCamera_getDisplayOrientedPose( const ArSession *session, const ArCamera *camera, ArPose *out_pose ) { fpArCamera_getDisplayOrientedPose( session, camera, out_pose ); }
		void ArCamera_getProjectionMatrix( const ArSession *session, const ArCamera *camera, float near, float far, float *dest_col_major_4x4 ) { fpArCamera_getProjectionMatrix( session, camera, near, far, dest_col_major_4x4 ); }
		void ArCamera_release( ArCamera *camera ) { fpArCamera_release( camera ); }

		void ArHitResult_create( const ArSession *session, ArHitResult **out_hit_result ) { fpArHitResult_create( session, out_hit_result ); }
		void ArHitResult_destroy( ArHitResult *hit_result ) { fpArHitResult_destroy( hit_result ); }
		void ArHitResult_getHitPose( const ArSession *session, const ArHitResult *hit_result, ArPose *out_pose ) { fpArHitResult_getHitPose( session, hit_result, out_pose ); }
		void ArHitResult_acquireTrackable( const ArSession *session, const ArHitResult *hit_result, ArTrackable **out_trackable ) { fpArHitResult_acquireTrackable( session, hit_result, out_trackable ); }
		ArStatus ArHitResult_acquireNewAnchor( ArSession *session, ArHitResult *hit_result, ArAnchor **out_anchor ) { return fpArHitResult_acquireNewAnchor( session, hit_result, out_anchor ); }

		void ArHitResultList_create( const ArSession *session, ArHitResultList **out_hit_result_list ) { fpArHitResultList_create( session, out_hit_result_list ); }
		void ArHitResultList_getItem( const ArSession *session, const ArHitResultList *hit_result_list, int32_t index, ArHitResult *out_hit_result ) { fpArHitResultList_getItem( session, hit_result_list, index, out_hit_result ); }
		void ArHitResultList_destroy( ArHitResultList *hit_result_list ) { fpArHitResultList_destroy( hit_result_list ); }
		void ArHitResultList_getSize( const ArSession *session, const ArHitResultList *hit_result_list, int32_t *out_size ) { fpArHitResultList_getSize(session, hit_result_list, out_size); }
		 
		void ArTrackable_getType( const ArSession *session, const ArTrackable *trackable, ArTrackableType *out_trackable_type ) { fpArTrackable_getType( session, trackable, out_trackable_type ); }
		void ArTrackable_release( ArTrackable *trackable ) { fpArTrackable_release( trackable ); }
		 
		void ArTrackableList_create( const ArSession *session, ArTrackableList **out_trackable_list ) { fpArTrackableList_create( session, out_trackable_list ); }
		void ArTrackableList_destroy( ArTrackableList *trackable_list ) { ArTrackableList_destroy( trackable_list ); }
		void ArTrackableList_getSize( const ArSession *session, const ArTrackableList *trackable_list, int32_t *out_size ) { fpArTrackableList_getSize( session, trackable_list, out_size ); }
		void ArTrackableList_acquireItem( const ArSession *session, const ArTrackableList *trackable_list, int32_t index, ArTrackable **out_trackable ) { fpArTrackableList_acquireItem( session, trackable_list, index, out_trackable ); }
		void ArTrackable_getTrackingState( const ArSession *session, const ArTrackable *trackable, ArTrackingState *out_tracking_state ) { fpArTrackable_getTrackingState( session, trackable, out_tracking_state ); }
		 
		void ArPlane_isPoseInExtents( const ArSession *session, const ArPlane *plane, const ArPose *pose, int32_t *out_pose_in_extents ) { fpArPlane_isPoseInExtents( session, plane, pose, out_pose_in_extents ); }
		void ArPlane_isPoseInPolygon( const ArSession *session, const ArPlane *plane, const ArPose *pose, int32_t *out_pose_in_extents ) { fpArPlane_isPoseInPolygon( session, plane, pose, out_pose_in_extents ); }
		void ArPlane_getCenterPose( const ArSession *session, const ArPlane *plane, ArPose *out_pose ) { fpArPlane_getCenterPose( session, plane, out_pose ); }
		void ArPlane_getExtentX( const ArSession *session, const ArPlane *plane, float *out_extent_x ) { fpArPlane_getExtentX( session, plane, out_extent_x ); }
		void ArPlane_getExtentZ( const ArSession *session, const ArPlane *plane, float *out_extent_z ) { fpArPlane_getExtentZ( session, plane, out_extent_z ); }
		void ArPlane_acquireSubsumedBy( const ArSession *session, const ArPlane *plane, ArPlane **out_subsumed_by ) { fpArPlane_acquireSubsumedBy( session, plane, out_subsumed_by ); }
		 
		void ArPoint_getOrientationMode( const ArSession *session, const ArPoint *point, ArPointOrientationMode *out_orientation_mode ) { fpArPoint_getOrientationMode( session, point, out_orientation_mode ); }

		void ArConfig_setUpdateMode( const ArSession *session, ArConfig *config, ArUpdateMode update_mode ) { fpArConfig_setUpdateMode( session, config, update_mode ); }
		void ArConfig_setPlaneFindingMode( const ArSession *session, ArConfig *config, ArPlaneFindingMode plane_finding_mode ) { fpArConfig_setPlaneFindingMode( session, config, plane_finding_mode ); }
		void ArConfig_setLightEstimationMode( const ArSession *session, ArConfig *config, ArLightEstimationMode light_estimation_mode ) { fpArConfig_setLightEstimationMode( session, config, light_estimation_mode ); }
		
		void ArLightEstimate_create(const ArSession *session, ArLightEstimate **out_light_estimate) { fpArLightEstimate_create(session, out_light_estimate); }
		void ArLightEstimate_getState(const ArSession *session, const ArLightEstimate *light_estimate, ArLightEstimateState *out_light_estimate_state) { fpArLightEstimate_getState(session, light_estimate, out_light_estimate_state); }
		void ArLightEstimate_getPixelIntensity(const ArSession *session, const ArLightEstimate *light_estimate, float *out_pixel_intensity) { fpArLightEstimate_getPixelIntensity(session, light_estimate, out_pixel_intensity); }
		void ArLightEstimate_destroy(ArLightEstimate *light_estimate) { fpArLightEstimate_destroy(light_estimate); }
		
		void ArAnchor_getPose(const ArSession *session, const ArAnchor *anchor, ArPose *out_pose) { fpArAnchor_getPose(session, anchor, out_pose); }
		void ArAnchor_getTrackingState(const ArSession *session, const ArAnchor *anchor, ArTrackingState *out_tracking_state) { fpArAnchor_getTrackingState(session, anchor, out_tracking_state); }
		void ArAnchor_release(ArAnchor *anchor) { fpArAnchor_release(anchor); }
	}
#endif

void agk::ARSetup()
//****
{
	int request_install = (g_iARStatus != 1) ? 1 : 0; // only request install if we aren't returning from an install request
	g_iARStatus = -1;

	if ( g_pRenderer->GetIdentifier() != AGK_RENDERER_OPENGL_ES2 )
	{
		agk::Error( "AR is currently only supported by OpenGL ES, please change the InitGraphics call to use AGK_RENDERER_MODE_ONLY_LOWEST" );
		return;
	}

#ifdef AGK_USE_AR
	if ( !g_pARCoreLibHandle )
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);
		jclass AGKHelper = GetAGKHelper(lJNIEnv);
		jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetAPIVersion","()I");
		int APIVersion = lJNIEnv->CallStaticIntMethod( AGKHelper, method );
		vm->DetachCurrentThread();

		if ( APIVersion < 24 )
		{
			agk::Warning( "ARCore not supported on this API version" );
			return;
		}

		g_pARCoreLibHandle = (void*) dlopen( "libarcore_sdk.so", RTLD_NOW );
		if ( !g_pARCoreLibHandle ) 
		{
			agk::Warning( "Failed to load ARCore lib" );
			return;
		}

		agk::Warning( "Successfully loaded ARCore lib" );

		fpArCoreApk_requestInstallCustom = (ArStatus(*)(void*, void*, int, ArInstallBehavior, ArInstallUserMessageType, ArInstallStatus*)) dlsym( g_pARCoreLibHandle, "ArCoreApk_requestInstallCustom" );

		//fpArSession_checkSupported = (ArStatus(*)(const ArSession*, const ArConfig*)) dlsym( g_pARCoreLibHandle, "ArSession_checkSupported" );
		fpArSession_configure = (ArStatus(*)(ArSession*, const ArConfig* )) dlsym( g_pARCoreLibHandle, "ArSession_configure" );
		fpArSession_create = (ArStatus(*)(void*, void*, ArSession**)) dlsym( g_pARCoreLibHandle, "ArSession_create" );
		fpArSession_setDisplayGeometry = (void(*)(ArSession*, int, int, int)) dlsym( g_pARCoreLibHandle, "ArSession_setDisplayGeometry" );
		fpArSession_setCameraTextureName = (void(*)(ArSession*, unsigned int)) dlsym( g_pARCoreLibHandle, "ArSession_setCameraTextureName" );
		fpArSession_update = (ArStatus(*)(ArSession*, ArFrame*)) dlsym( g_pARCoreLibHandle, "ArSession_update" );
		fpArSession_pause = (ArStatus(*)(ArSession*)) dlsym( g_pARCoreLibHandle, "ArSession_pause" );
		fpArSession_resume = (ArStatus(*)(ArSession*)) dlsym( g_pARCoreLibHandle, "ArSession_resume" );
		fpArSession_destroy = (void(*)(ArSession*)) dlsym( g_pARCoreLibHandle, "ArSession_destroy" );
		fpArSession_getAllTrackables = (void(*)(const ArSession*, ArTrackableType, ArTrackableList*)) dlsym( g_pARCoreLibHandle, "ArSession_getAllTrackables" );
		fpArSession_acquireNewAnchor = (ArStatus(*)(ArSession*, const ArPose*, ArAnchor**)) dlsym( g_pARCoreLibHandle, "ArSession_acquireNewAnchor" );
	
		fpArConfig_create = (void(*)(const ArSession*, ArConfig**)) dlsym( g_pARCoreLibHandle, "ArConfig_create" );
		fpArConfig_destroy = (void(*)(ArConfig*)) dlsym( g_pARCoreLibHandle, "ArConfig_destroy" );
	
		fpArFrame_create = (void(*)(const ArSession*, ArFrame**)) dlsym( g_pARCoreLibHandle, "ArFrame_create" );
		fpArFrame_destroy = (void(*)(ArFrame*)) dlsym( g_pARCoreLibHandle, "ArFrame_destroy" );
		fpArFrame_acquireCamera = (void(*)(const ArSession*, const ArFrame*, ArCamera**)) dlsym( g_pARCoreLibHandle, "ArFrame_acquireCamera" );
		fpArFrame_hitTest = (void(*)(const ArSession*, const ArFrame*, float, float, ArHitResultList*)) dlsym( g_pARCoreLibHandle, "ArFrame_hitTest" );
		fpArFrame_getLightEstimate = (void(*)(const ArSession*, const ArFrame*, ArLightEstimate*)) dlsym( g_pARCoreLibHandle, "ArFrame_getLightEstimate" );
	
		fpArPose_create = (void(*)(const ArSession*, const float*, ArPose**)) dlsym( g_pARCoreLibHandle, "ArPose_create" );
		fpArPose_getPoseRaw = (void(*)(const ArSession*, const ArPose*, float*)) dlsym( g_pARCoreLibHandle, "ArPose_getPoseRaw" );
		fpArPose_destroy = (void(*)(ArPose*)) dlsym( g_pARCoreLibHandle, "ArPose_destroy" );

		fpArCamera_getTrackingState = (void(*)(const ArSession*, const ArCamera*, ArTrackingState*)) dlsym( g_pARCoreLibHandle, "ArCamera_getTrackingState" );
		fpArCamera_getDisplayOrientedPose = (void(*)(const ArSession*, const ArCamera*, ArPose*)) dlsym( g_pARCoreLibHandle, "ArCamera_getDisplayOrientedPose" );
		fpArCamera_getProjectionMatrix = (void(*)(const ArSession*, const ArCamera*, float, float, float*)) dlsym( g_pARCoreLibHandle, "ArCamera_getProjectionMatrix" );
		fpArCamera_release = (void(*)(ArCamera*)) dlsym( g_pARCoreLibHandle, "ArCamera_release" );

		fpArHitResult_create = (void(*)(const ArSession*, ArHitResult**)) dlsym( g_pARCoreLibHandle, "ArHitResult_create" );
		fpArHitResult_destroy = (void(*)(ArHitResult*)) dlsym( g_pARCoreLibHandle, "ArHitResult_destroy" );
		fpArHitResult_getHitPose = (void(*)(const ArSession*, const ArHitResult*, ArPose*)) dlsym( g_pARCoreLibHandle, "ArHitResult_getHitPose" );
		fpArHitResult_acquireTrackable = (void(*)(const ArSession*, const ArHitResult*, ArTrackable**)) dlsym( g_pARCoreLibHandle, "ArHitResult_acquireTrackable" );
		fpArHitResult_acquireNewAnchor = (ArStatus(*)( ArSession*, ArHitResult*, ArAnchor**)) dlsym( g_pARCoreLibHandle, "ArHitResult_acquireNewAnchor" );

		fpArHitResultList_create = (void(*)(const ArSession*, ArHitResultList**)) dlsym( g_pARCoreLibHandle, "ArHitResultList_create" );
		fpArHitResultList_getItem = (void(*)(const ArSession*, const ArHitResultList*, int32_t, ArHitResult*)) dlsym( g_pARCoreLibHandle, "ArHitResultList_getItem" );
		fpArHitResultList_destroy = (void(*)(ArHitResultList*)) dlsym( g_pARCoreLibHandle, "ArHitResultList_destroy" );
		fpArHitResultList_getSize = (void(*)(const ArSession*, const ArHitResultList*, int32_t*)) dlsym( g_pARCoreLibHandle, "ArHitResultList_getSize" );

		fpArTrackable_getType = (void(*)(const ArSession*, const ArTrackable*, ArTrackableType*)) dlsym( g_pARCoreLibHandle, "ArTrackable_getType" );
		fpArTrackable_release = (void(*)(ArTrackable*)) dlsym( g_pARCoreLibHandle, "ArTrackable_release" );

		fpArTrackableList_create = (void(*)(const ArSession*, ArTrackableList**)) dlsym( g_pARCoreLibHandle, "ArTrackableList_create" );
		fpArTrackableList_destroy = (void(*)(ArTrackableList*)) dlsym( g_pARCoreLibHandle, "ArTrackableList_destroy" );
		fpArTrackableList_getSize = (void(*)(const ArSession*, const ArTrackableList*, int32_t*)) dlsym( g_pARCoreLibHandle, "ArTrackableList_getSize" );
		fpArTrackableList_acquireItem = (void(*)(const ArSession*, const ArTrackableList*, int32_t, ArTrackable**)) dlsym( g_pARCoreLibHandle, "ArTrackableList_acquireItem" );
		fpArTrackable_getTrackingState = (void(*)(const ArSession*, const ArTrackable*, ArTrackingState*)) dlsym( g_pARCoreLibHandle, "ArTrackable_getTrackingState" );

		fpArPlane_isPoseInExtents = (void(*)(const ArSession*, const ArPlane*, const ArPose*, int32_t*)) dlsym( g_pARCoreLibHandle, "ArPlane_isPoseInExtents" );
		fpArPlane_isPoseInPolygon = (void(*)(const ArSession*, const ArPlane*, const ArPose*, int32_t*)) dlsym( g_pARCoreLibHandle, "ArPlane_isPoseInPolygon" );
		fpArPlane_getCenterPose = (void(*)(const ArSession*, const ArPlane*, ArPose*)) dlsym( g_pARCoreLibHandle, "ArPlane_getCenterPose" );
		fpArPlane_getExtentX = (void(*)(const ArSession*, const ArPlane*, float*)) dlsym( g_pARCoreLibHandle, "ArPlane_getExtentX" );
		fpArPlane_getExtentZ = (void(*)(const ArSession*, const ArPlane*, float*)) dlsym( g_pARCoreLibHandle, "ArPlane_getExtentZ" );
		fpArPlane_acquireSubsumedBy = (void(*)(const ArSession*, const ArPlane*, ArPlane**)) dlsym( g_pARCoreLibHandle, "ArPlane_acquireSubsumedBy" );

		fpArPoint_getOrientationMode = (void(*)(const ArSession*, const ArPoint*, ArPointOrientationMode*)) dlsym( g_pARCoreLibHandle, "ArPoint_getOrientationMode" );

		fpArConfig_setUpdateMode = (void(*)(const ArSession*, ArConfig*, ArUpdateMode)) dlsym( g_pARCoreLibHandle, "ArConfig_setUpdateMode" );
		fpArConfig_setPlaneFindingMode = (void(*)(const ArSession*, ArConfig*, ArPlaneFindingMode)) dlsym( g_pARCoreLibHandle, "ArConfig_setPlaneFindingMode" );
		fpArConfig_setLightEstimationMode = (void(*)(const ArSession*, ArConfig*, ArLightEstimationMode)) dlsym( g_pARCoreLibHandle, "ArConfig_setLightEstimationMode" );
		
		fpArLightEstimate_create = (void(*)(const ArSession*, ArLightEstimate**)) dlsym( g_pARCoreLibHandle, "ArLightEstimate_create" );
		fpArLightEstimate_getState = (void(*)(const ArSession*, const ArLightEstimate*, ArLightEstimateState*)) dlsym( g_pARCoreLibHandle, "ArLightEstimate_getState" );
		fpArLightEstimate_getPixelIntensity = (void(*)(const ArSession*, const ArLightEstimate*, float*)) dlsym( g_pARCoreLibHandle, "ArLightEstimate_getPixelIntensity" );
		fpArLightEstimate_destroy = (void(*)(ArLightEstimate*)) dlsym( g_pARCoreLibHandle, "ArLightEstimate_destroy" );
		
		fpArAnchor_getPose = (void(*)(const ArSession*, const ArAnchor*, ArPose*)) dlsym( g_pARCoreLibHandle, "ArAnchor_getPose" );
		fpArAnchor_getTrackingState = (void(*)(const ArSession*, const ArAnchor*, ArTrackingState*)) dlsym( g_pARCoreLibHandle, "ArAnchor_getTrackingState" );
		fpArAnchor_release = (void(*)(ArAnchor*)) dlsym( g_pARCoreLibHandle, "ArAnchor_release" );
	}

	if ( !g_pARSession ) 
	{
		JNIEnv* lJNIEnv = g_pActivity->env;
		JavaVM* vm = g_pActivity->vm;
		vm->AttachCurrentThread(&lJNIEnv, NULL);

		jobject lNativeActivity = g_pActivity->clazz;
		jclass AGKHelper = GetAGKHelper(lJNIEnv);

		jmethodID methodGetOrien = lJNIEnv->GetStaticMethodID( AGKHelper, "GetOrientation","(Landroid/app/Activity;)I");
		int orien = lJNIEnv->CallStaticIntMethod( AGKHelper, methodGetOrien, lNativeActivity );

		ArInstallStatus install_status;
		agk::Warning( "Requesting ARCore Install" );
		ArStatus result = ArCoreApk_requestInstallCustom( lJNIEnv, g_pActivity->clazz, request_install, AR_INSTALL_BEHAVIOR_OPTIONAL, AR_INSTALL_USER_MESSAGE_TYPE_FEATURE, &install_status );
		if ( result != AR_SUCCESS )
		{
			vm->DetachCurrentThread();
			uString info; info.Format( "Failed to request ARCore installation, error: %d", result );
			agk::Warning( info );
			if ( result == AR_UNAVAILABLE_USER_DECLINED_INSTALLATION ) g_iARStatus = -2;
			return;
		}

		if ( install_status == AR_INSTALL_STATUS_INSTALL_REQUESTED )
		{
			// user will be prompted to install
			vm->DetachCurrentThread();
			agk::Warning( "Prompting AR Core installation" );
			g_iARStatus = 1;
			return;
		}

		agk::Warning( "Creating ARCore session" );
		result = ArSession_create( lJNIEnv, g_pActivity->clazz, &g_pARSession );
		if ( result != AR_SUCCESS ) 
		{
			vm->DetachCurrentThread();
			uString info; info.Format( "Failed to create ARCore session, error: %d", result );
			agk::Warning( info );
			return;
		}

		vm->DetachCurrentThread();

		if ( !g_pARSession ) 
		{
			agk::Warning( "Failed to get ARCore session" );
			return;
		}

		agk::Warning( "Creating ARCore config" );
		ArConfig* ar_config = 0;
		ArConfig_create( g_pARSession, &ar_config );
		if ( !ar_config )
		{
			agk::Warning( "Failed to create ARCore config" );
			ArSession_destroy( g_pARSession );
			g_pARSession = 0;
			return;
		}

		ArConfig_setUpdateMode( g_pARSession, ar_config, AR_UPDATE_MODE_BLOCKING );

		/*
		// deprecated, now always returns true
		agk::Warning( "Checking ARCore config supported" );
		result = ArSession_checkSupported( g_pARSession, ar_config );
		if ( result != AR_SUCCESS )
		{
			agk::Warning( "ARCore configuration not supported" );
			ArConfig_destroy( ar_config );
			ArSession_destroy( g_pARSession );
			g_pARSession = 0;
			return;
		}
		*/

		agk::Warning( "Configuring ARCore session" );
		result = ArSession_configure( g_pARSession, ar_config );
		if ( result != AR_SUCCESS )
		{
			agk::Warning( "Failed to configure ARCore session" );
			ArConfig_destroy( ar_config );
			ArSession_destroy( g_pARSession );
			g_pARSession = 0;
			return;
		}

		ArConfig_destroy( ar_config );

		agk::Warning( "Creating ARCore frame" );
		ArFrame_create( g_pARSession, &g_pARFrame );
		if ( !g_pARFrame )
		{
			agk::Warning( "Failed to create ARCore frame" );
			ArSession_destroy( g_pARSession );
			g_pARSession = 0;
			return;
		}

		g_iARWidth = m_iRealDeviceWidth;
		g_iARHeight = m_iRealDeviceHeight;
		ArSession_setDisplayGeometry( g_pARSession, orien, g_iARWidth, g_iARHeight );

		result = ArSession_resume(g_pARSession);
		if ( result != AR_SUCCESS )
		{
			switch( result )
			{
				case AR_ERROR_CAMERA_PERMISSION_NOT_GRANTED: agk::Warning( "Failed to resume ARCore session, camera permission not granted" );
				case AR_ERROR_CAMERA_NOT_AVAILABLE: agk::Warning( "Failed to resume ARCore session, camera not available" );
				default: agk::Warning( "Failed to resume ARCore session" );
			}
			return;
		}
	}

	g_iARStatus = 2;

	if ( !g_iARTextureRaw )
	{
		GLESRegenerateExternalTexture( &g_iARTextureRaw );
	}

	// create quad and shader
	if ( !g_pARTextureShader )
	{
		g_pARTextureShader = g_pRenderer->LoadDefaultShaderARAndroid();
	}
	
	if ( !g_pARTextureQuad )
	{
		g_pARTextureQuad = new cObject3D();
		g_pARTextureQuad->CreateQuad();
		g_pARTextureQuad->SetCullMode(1);
		g_pARTextureQuad->SetDepthReadMode(7);
		g_pARTextureQuad->SetDepthWrite(0);
		g_pARTextureQuad->SetShader( g_pARTextureShader );
	}
#endif
}

int agk::ARGetStatus()
//****
{
	return g_iARStatus;
}

// not a Tier 1 command
void agk::ARUpdateInternal()
{
#ifdef AGK_USE_AR
	if ( !g_pARSession ) return;
	if ( !g_pARFrame ) return;
	if ( !g_iARTextureRaw ) return;

	static int lastOrien = -1;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);
	jobject lNativeActivity = g_pActivity->clazz;
	jclass AGKHelper = GetAGKHelper(lJNIEnv);
	jmethodID methodGetOrien = lJNIEnv->GetStaticMethodID( AGKHelper, "GetOrientation","(Landroid/app/Activity;)I");
	int orien = lJNIEnv->CallStaticIntMethod( AGKHelper, methodGetOrien, lNativeActivity );

	if ( m_iRealDeviceWidth != g_iARWidth || m_iRealDeviceHeight != g_iARHeight || orien != lastOrien )
	{
		g_iARWidth = m_iRealDeviceWidth;
		g_iARHeight = m_iRealDeviceHeight;
		ArSession_setDisplayGeometry( g_pARSession, orien, g_iARWidth, g_iARHeight );
	}
	lastOrien = orien;

	ArSession_setCameraTextureName( g_pARSession, g_iARTextureRaw );

	if ( ArSession_update(g_pARSession, g_pARFrame) != AR_SUCCESS) 
	{
		vm->DetachCurrentThread();
		agk::Warning( "Failed to update AR Session" );
		return;
	}

	vm->DetachCurrentThread();

	AGKAndroidARAnchor *pAnchor = g_pARAnchorList.GetFirst();
	while ( pAnchor )
	{
		pAnchor->Update();
		pAnchor = g_pARAnchorList.GetNext();
	}
#endif
}

void agk::ARPause()
{
#ifdef AGK_USE_AR
	if ( !g_pARSession ) return;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	ArSession_pause( g_pARSession );

	vm->DetachCurrentThread();
#endif
}

void agk::ARResume()
{
#ifdef AGK_USE_AR
	if ( g_iARStatus == 1 ) 
	{
		ARSetup();
		return;
	}

	if ( !g_pARSession ) return;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	ArSession_resume( g_pARSession );

	vm->DetachCurrentThread();
#endif
}

void agk::ARDestroy()
{
#ifdef AGK_USE_AR
	int clearScreen = 0;
	if ( g_pARSession )
	{
		clearScreen = 1;

		uint32_t spr = agk::CreateSprite( 0 );
		agk::SetSpriteColor( spr, 0,0,0,255 );
		agk::SetSpriteSize( spr, agk::GetVirtualWidth()*0.7f, agk::GetVirtualHeight()*0.2f );
		agk::SetSpritePositionByOffset( spr, agk::GetVirtualWidth()/2.0, agk::GetVirtualHeight()/2.0 );
		agk::DrawSprite( spr );
		agk::DeleteSprite( spr );

		uint32_t txt = agk::CreateText( "Deactivating AR..." );
		agk::SetTextSize( txt, agk::GetVirtualHeight()*0.1f );
		agk::SetTextAlignment( txt, 1 );
		agk::SetTextPosition( txt, agk::GetVirtualWidth()/2.0, agk::GetVirtualHeight()/2.0-agk::GetVirtualHeight()*0.05f );
		agk::DrawText( txt );
		agk::DeleteText( txt );

		agk::Swap();
	}

	ARHitTestFinish();
	ARGetPlanesFinish();

	AGKAndroidARAnchor *pAnchor = g_pARAnchorList.GetFirst();
	while ( pAnchor )
	{
		delete pAnchor;		
		pAnchor = g_pARAnchorList.GetNext();
	}
	g_pARAnchorList.ClearAll();

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	if ( g_pARSession ) ArSession_destroy( g_pARSession );
    if ( g_pARFrame ) ArFrame_destroy( g_pARFrame );
	g_pARSession = 0;
	g_pARFrame = 0;

	vm->DetachCurrentThread();

	if ( g_pARTextureQuad )
	{
		delete g_pARTextureQuad;
		g_pARTextureQuad = 0;
	}

	if ( g_pARTextureShader )
	{
		delete g_pARTextureShader;
		g_pARTextureShader = 0;
	}

	if ( g_iARTextureRaw ) GLESDeleteExternalTexture( &g_iARTextureRaw );

	if ( clearScreen )
	{
		agk::ClearScreen();
		agk::Swap();
	}
#endif

	g_iARStatus = 0;
	g_iARPlaneFindingMode = 1;
	g_iARLightEstimationMode = 1;

	agk::ClearScreen();
	agk::Swap();
}

void agk::ARControlCamera()
{
#ifdef AGK_USE_AR
	if ( !g_pARSession ) return;
	if ( !g_pARFrame ) return;

	cCamera *pAGKCamera = agk::GetCurrentCamera();

	ArCamera* pARCamera;
	ArFrame_acquireCamera( g_pARSession, g_pARFrame, &pARCamera );

	ArTrackingState camera_tracking_state;
	ArCamera_getTrackingState( g_pARSession, pARCamera, &camera_tracking_state);
	if ( camera_tracking_state != AR_TRACKING_STATE_TRACKING ) return;

	float fNear = pAGKCamera->GetNearRange();
	float fFar = pAGKCamera->GetFarRange();

	float camera_data[7];
	float proj_mat[16];
	ArPose* pPose = 0;
    ArPose_create( g_pARSession, 0, &pPose );
	ArCamera_getDisplayOrientedPose( g_pARSession, pARCamera, pPose );
	ArPose_getPoseRaw( g_pARSession, pPose, camera_data );
	ArCamera_getProjectionMatrix( g_pARSession, pARCamera, fNear, fFar, proj_mat );

	// centered proj matrix
	//float fov = agk::ATan(1.0f / proj_mat[0]) * 2.0f;
	//float aspect = proj_mat[5] / proj_mat[0];
	//pAGKCamera->SetFOV( fov );
	//pAGKCamera->SetAspectRatio( aspect );
	//pAGKCamera->SetOffCenter(0);

	// off center proj matrix
	float diffX = (proj_mat[8]-1) / (proj_mat[8]+1); // l*diffX = r
	float tempX = (2*fNear) / proj_mat[0]; // r-l = tempX
	float left = tempX / (diffX-1);
	float right = left*diffX;

	float diffY = (proj_mat[9]-1) / (proj_mat[9]+1); // b*diffY = t
	float tempY = (2*fNear) / proj_mat[5]; // t-b = tempY
	float bottom = tempY / (diffY-1);
	float top = bottom*diffY;
	pAGKCamera->SetBounds( left, right, top, bottom );
	pAGKCamera->SetOffCenter(1);

	pAGKCamera->SetPosition( camera_data[4], camera_data[5], -camera_data[6] );
	pAGKCamera->SetRotationQuat( AGKQuaternion(-camera_data[3], camera_data[0], camera_data[1], -camera_data[2]) );

	ArPose_destroy( pPose );
	ArCamera_release( pARCamera );	
#endif
}

void agk::ARDrawBackground()
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARSession ) return;
	if ( !g_pARFrame ) return;
	if ( !g_iARTextureRaw ) return;
	if ( !g_pARTextureQuad ) return;
	if ( !g_pARTextureShader ) return;

	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);
	jobject lNativeActivity = g_pActivity->clazz;
	jclass AGKHelper = GetAGKHelper(lJNIEnv);
	jmethodID methodGetOrien = lJNIEnv->GetStaticMethodID( AGKHelper, "GetOrientation","(Landroid/app/Activity;)I");
	int orien = lJNIEnv->CallStaticIntMethod( AGKHelper, methodGetOrien, lNativeActivity );
	vm->DetachCurrentThread();

	// check for orientation changes
	static int isTablet = -1;
	if ( isTablet < 0 )
	{
		if ( agk::GetDeviceWidth() > agk::GetDeviceHeight() )
		{
			if (orien == 0 || orien == 2) isTablet = 1;
			else isTablet = 0;
		}
		else 
		{
			if (orien == 1 || orien == 3) isTablet = 1;
			else isTablet = 0;
		}
	}

	if ( isTablet )
	{
		switch( orien )
		{
			case 0: orien = 3; break;
			case 1: orien = 2; break;
			case 2: orien = 4; break;
			case 3: orien = 1; break;
			default: break;
		}
	}
	else
	{
		switch( orien )
		{
			case 0: orien = 1; break;
			case 1: orien = 3; break;
			case 2: orien = 2; break;
			case 3: orien = 4; break;
			default: break;
		}
	}

	switch( orien )
	{
		case 1: g_pARTextureShader->SetConstantByName( "orientation", 0, 1, -1, 0 ); break;
		case 2: g_pARTextureShader->SetConstantByName( "orientation", 0, -1, 1, 0 ); break;
		case 3: g_pARTextureShader->SetConstantByName( "orientation", 1, 0, 0, 1 ); break;
		case 4: g_pARTextureShader->SetConstantByName( "orientation", -1, 0, 0, -1 ); break;
	}

	GLESBindExternalTexture( g_iARTextureRaw );
	g_pARTextureQuad->Draw();
#endif
}

void agk::ARSetPlaneDetectionMode( int mode )
//****
{
#ifdef AGK_USE_AR
	if ( mode == 1 ) g_iARPlaneFindingMode = 1;
	else g_iARPlaneFindingMode = 0;

	if ( !g_pARSession ) return;

	ArConfig* ar_config = 0;
	ArConfig_create( g_pARSession, &ar_config );
	if ( !ar_config ) return;

	ArPlaneFindingMode planeMode = (g_iARPlaneFindingMode == 1) ? AR_PLANE_FINDING_MODE_HORIZONTAL : AR_PLANE_FINDING_MODE_DISABLED;
	ArLightEstimationMode lightMode = (g_iARLightEstimationMode == 1) ? AR_LIGHT_ESTIMATION_MODE_AMBIENT_INTENSITY : AR_LIGHT_ESTIMATION_MODE_DISABLED;

	ArConfig_setPlaneFindingMode( g_pARSession, ar_config, planeMode );
	ArConfig_setLightEstimationMode( g_pARSession, ar_config, lightMode );
	ArConfig_setUpdateMode( g_pARSession, ar_config, AR_UPDATE_MODE_BLOCKING );
	
	/*
	// deprecated, now always returns true
	ArStatus result = ArSession_checkSupported( g_pARSession, ar_config );
	if ( result != AR_SUCCESS )
	{
		ArConfig_destroy( ar_config );
		agk::Warning( "ARCore configuration not supported" );
		return;
	}
	*/

	agk::Warning( "Configuring ARCore session" );
	ArStatus result = ArSession_configure( g_pARSession, ar_config );
	if ( result != AR_SUCCESS )
	{
		ArConfig_destroy( ar_config );
		agk::Warning( "Failed to configure ARCore session" );
		return;
	}

	ArConfig_destroy( ar_config );
#endif
}

void agk::ARSetLightEstimationMode( int mode )
//****
{
#ifdef AGK_USE_AR
	if ( mode == 1 ) g_iARLightEstimationMode = 1;
	else g_iARLightEstimationMode = 0;

	if ( !g_pARSession ) return;

	ArConfig* ar_config = 0;
	ArConfig_create( g_pARSession, &ar_config );
	if ( !ar_config ) return;

	ArPlaneFindingMode planeMode = (g_iARPlaneFindingMode == 1) ? AR_PLANE_FINDING_MODE_HORIZONTAL : AR_PLANE_FINDING_MODE_DISABLED;
	ArLightEstimationMode lightMode = (g_iARLightEstimationMode == 1) ? AR_LIGHT_ESTIMATION_MODE_AMBIENT_INTENSITY : AR_LIGHT_ESTIMATION_MODE_DISABLED;

	ArConfig_setPlaneFindingMode( g_pARSession, ar_config, planeMode );
	ArConfig_setLightEstimationMode( g_pARSession, ar_config, lightMode );
	ArConfig_setUpdateMode( g_pARSession, ar_config, AR_UPDATE_MODE_BLOCKING );
	
	/*
	// deprecated
	ArStatus result = ArSession_checkSupported( g_pARSession, ar_config );
	if ( result != AR_SUCCESS )
	{
		ArConfig_destroy( ar_config );
		agk::Warning( "ARCore configuration not supported" );
		return;
	}
	*/

	agk::Warning( "Configuring ARCore session" );
	ArStatus result = ArSession_configure( g_pARSession, ar_config );
	if ( result != AR_SUCCESS )
	{
		ArConfig_destroy( ar_config );
		agk::Warning( "Failed to configure ARCore session" );
		return;
	}

	ArConfig_destroy( ar_config );
#endif
}

float agk::ARGetLightEstimate()
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARSession ) return 0.5f;

	ArLightEstimate* ar_light_estimate;
	ArLightEstimateState ar_light_estimate_state;
	ArLightEstimate_create( g_pARSession, &ar_light_estimate );

	ArFrame_getLightEstimate( g_pARSession, g_pARFrame, ar_light_estimate);
	ArLightEstimate_getState( g_pARSession, ar_light_estimate, &ar_light_estimate_state);

	// Set light intensity to default. Intensity value ranges from 0.0f to 1.0f.
	float light_intensity = 0.5f;
	if ( ar_light_estimate_state == AR_LIGHT_ESTIMATE_STATE_VALID ) 
	{
		ArLightEstimate_getPixelIntensity( g_pARSession, ar_light_estimate, &light_intensity );
	}

	ArLightEstimate_destroy( ar_light_estimate );

	return light_intensity;
#else
	return 0.5f;
#endif
}

int agk::ARHitTest( float screenX, float screenY )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARSession ) return 0;
	if ( !g_pARFrame ) return 0;
	ARHitTestFinish();

	ArHitResultList *pHitList = 0;
    ArHitResultList_create( g_pARSession, &pHitList );

	float fX = ScreenToDeviceX( screenX );
	float fY = ScreenToDeviceY( screenY );
    
    ArFrame_hitTest( g_pARSession, g_pARFrame, fX, fY, pHitList );

	int iListSize = 0;
    ArHitResultList_getSize( g_pARSession, pHitList, &iListSize );
	g_pARHitResults = new ArHitResult*[ iListSize ];
	g_pARHitPoses = new ArPose*[ iListSize ];
	int count = 0;
	for( int i = 0; i < iListSize; i++ )
	{
		ArHitResult* pHitResult = 0;
		ArHitResult_create( g_pARSession, &pHitResult );
		ArHitResultList_getItem( g_pARSession, pHitList, i, pHitResult );

		ArTrackable* pTrackable = 0;
		ArHitResult_acquireTrackable( g_pARSession, pHitResult, &pTrackable );
		ArTrackableType iType = AR_TRACKABLE_NOT_VALID;
		ArTrackable_getType( g_pARSession, pTrackable, &iType );

		// Creates an anchor if a plane or an oriented point was hit.
		if ( AR_TRACKABLE_PLANE == iType ) 
		{
			ArPose* pPose = 0;
			ArPose_create( g_pARSession, 0, &pPose );
			ArHitResult_getHitPose( g_pARSession, pHitResult, pPose );
			int in_polygon = 0;
			ArPlane* pPlane = ArAsPlane( pTrackable );
			ArPlane_isPoseInPolygon( g_pARSession, pPlane, pPose, &in_polygon );
			ArTrackable_release( pTrackable );
			if ( !in_polygon ) 
			{
				ArPose_destroy( pPose );
				ArHitResult_destroy( pHitResult );
			}
			else
			{
				g_pARHitPoses[ count ] = pPose;
				g_pARHitResults[ count ] = pHitResult;
				count++;
			}
		}
		else if ( AR_TRACKABLE_POINT == iType )
		{
			ArPoint* pPoint = ArAsPoint( pTrackable );
			ArPointOrientationMode mode;
			ArPoint_getOrientationMode( g_pARSession, pPoint, &mode );
			ArTrackable_release( pTrackable );
			if ( AR_POINT_ORIENTATION_ESTIMATED_SURFACE_NORMAL == mode ) 
			{
				ArPose* pPose = 0;
				ArPose_create( g_pARSession, 0, &pPose );
				ArHitResult_getHitPose( g_pARSession, pHitResult, pPose );
				g_pARHitPoses[ count ] = pPose;
				g_pARHitResults[ count ] = pHitResult;
				count++;
			}
			else
			{
				ArHitResult_destroy( pHitResult );
			}
		}
	}

	ArHitResultList_destroy( pHitList );

	g_iARHitResultsSize = count;
	return g_iARHitResultsSize;
#else
	return 0;
#endif
}

float agk::ARGetHitTestX( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARHitPoses ) return 0;
	if ( index < 1 || index > g_iARHitResultsSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARHitPoses[ index ], fPoseRaw );

	return fPoseRaw[ 4 ];
#else
	return 0;
#endif
}

float agk::ARGetHitTestY( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARHitPoses ) return 0;
	if ( index < 1 || index > g_iARHitResultsSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARHitPoses[ index ], fPoseRaw );

	return fPoseRaw[ 5 ];
#else
	return 0;
#endif
}

float agk::ARGetHitTestZ( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARHitPoses ) return 0;
	if ( index < 1 || index > g_iARHitResultsSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARHitPoses[ index ], fPoseRaw );

	return -fPoseRaw[ 6 ]; // convert pose Z to left hand coordinate
#else
	return 0;
#endif
}

float agk::ARGetHitTestNormalX( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARHitPoses ) return 0;
	if ( index < 1 || index > g_iARHitResultsSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARHitPoses[ index ], fPoseRaw );

	AGKVector normal( 0, 1, 0 );
	AGKQuaternion quat( -fPoseRaw[3], fPoseRaw[0], fPoseRaw[1], -fPoseRaw[2] );
	normal.Mult( quat );
	return normal.x;
#else
	return 0;
#endif
}

float agk::ARGetHitTestNormalY( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARHitPoses ) return 0;
	if ( index < 1 || index > g_iARHitResultsSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARHitPoses[ index ], fPoseRaw );

	AGKVector normal( 0, 1, 0 );
	AGKQuaternion quat( -fPoseRaw[3], fPoseRaw[0], fPoseRaw[1], -fPoseRaw[2] );
	normal.Mult( quat );
	return normal.y;
#else
	return 0;
#endif
}

float agk::ARGetHitTestNormalZ( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARHitPoses ) return 0;
	if ( index < 1 || index > g_iARHitResultsSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARHitPoses[ index ], fPoseRaw );

	AGKVector normal( 0, 1, 0 );
	AGKQuaternion quat( -fPoseRaw[3], fPoseRaw[0], fPoseRaw[1], -fPoseRaw[2] );
	normal.Mult( quat );
	return normal.z;
#else
	return 0;
#endif
}

int agk::ARGetHitTestType( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARHitResults ) return 0;
	if ( index < 1 || index > g_iARHitResultsSize ) return 0;
	index--;

	ArTrackable* pTrackable = 0;
	ArHitResult_acquireTrackable( g_pARSession, g_pARHitResults[index], &pTrackable );
	ArTrackableType iType = AR_TRACKABLE_NOT_VALID;
	ArTrackable_getType( g_pARSession, pTrackable, &iType );
	ArTrackable_release( pTrackable );

	if ( AR_TRACKABLE_PLANE == iType ) return 1;
	else if ( AR_TRACKABLE_POINT == iType ) return 0;
	else return -1;
#else
	return -1;
#endif
}

void agk::ARHitTestFinish()
//****
{
#ifdef AGK_USE_AR
	if ( g_pARHitResults ) 
	{
		for( int i = 0; i < g_iARHitResultsSize; i++ ) ArHitResult_destroy( g_pARHitResults[i] );
		delete [] g_pARHitResults;
	}
	g_pARHitResults = 0;

	if ( g_pARHitPoses ) 
	{
		for( int i = 0; i < g_iARHitResultsSize; i++ ) ArPose_destroy( g_pARHitPoses[i] );
		delete [] g_pARHitPoses;
	}
	g_pARHitPoses = 0;

	g_iARHitResultsSize = 0;
#endif
}

int agk::ARGetPlanes( int reserved )
//****
{
#ifdef AGK_USE_AR
	ARGetPlanesFinish();
	if ( !g_pARSession ) return 0;
	
	ArTrackableList *pTrackableList = 0;
	ArTrackableList_create( g_pARSession, &pTrackableList );
	ArSession_getAllTrackables( g_pARSession, AR_TRACKABLE_PLANE, pTrackableList );

	int listSize = 0;
	ArTrackableList_getSize( g_pARSession, pTrackableList, &listSize );
	g_pARPlanes = new ArPlane*[ listSize ];
	g_pARPlanePoses = new ArPose*[ listSize ];
	int count = 0;
	for( int i = 0; i < listSize; i++ )
	{
		ArTrackable *pTrackable = 0;
		ArTrackableList_acquireItem( g_pARSession, pTrackableList, i, &pTrackable );

		ArTrackingState iState = AR_TRACKING_STATE_STOPPED;
		ArTrackable_getTrackingState( g_pARSession, pTrackable, &iState );
		if ( iState != AR_TRACKING_STATE_TRACKING ) 
		{
			ArTrackable_release( pTrackable );
			continue;
		}

		ArPlane *pParent = 0;
		ArPlane_acquireSubsumedBy( g_pARSession, ArAsPlane( pTrackable ), &pParent );
		if ( pParent != 0 )
		{
			ArTrackable_release( ArAsTrackable(pParent) );
			ArTrackable_release( pTrackable );
			continue;
		}

		g_pARPlanes[ count ] = ArAsPlane( pTrackable );
		
		ArPose *pPose = 0;
		ArPose_create( g_pARSession, 0, &pPose );
		ArPlane_getCenterPose( g_pARSession, g_pARPlanes[count], pPose );
		g_pARPlanePoses[ count ] = pPose;

		count++;
	}

	ArTrackableList_destroy( pTrackableList );

	g_iARPlanesSize = count;
	return g_iARPlanesSize;
#else
	return 0;
#endif
}

float agk::ARGetPlaneX( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanePoses ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARPlanePoses[ index ], fPoseRaw );

	return fPoseRaw[ 4 ];
#else
	return 0;
#endif
}

float agk::ARGetPlaneY( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanePoses ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARPlanePoses[ index ], fPoseRaw );

	return fPoseRaw[ 5 ];
#else
	return 0;
#endif
}

float agk::ARGetPlaneZ( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanePoses ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARPlanePoses[ index ], fPoseRaw );

	return -fPoseRaw[ 6 ]; // convert to left hand coordinate
#else
	return 0;
#endif
}

float agk::ARGetPlaneAngleX( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanePoses ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARPlanePoses[ index ], fPoseRaw );

	AGKQuaternion quat( -fPoseRaw[3], fPoseRaw[0], fPoseRaw[1], -fPoseRaw[2] );
	return quat.GetEulerX();
#else
	return 0;
#endif
}

float agk::ARGetPlaneAngleY( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanePoses ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARPlanePoses[ index ], fPoseRaw );

	AGKQuaternion quat( -fPoseRaw[3], fPoseRaw[0], fPoseRaw[1], -fPoseRaw[2] );
	return quat.GetEulerY();
#else
	return 0;
#endif
}

float agk::ARGetPlaneAngleZ( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanePoses ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;
	
	float fPoseRaw[ 7 ];
	ArPose_getPoseRaw( g_pARSession, g_pARPlanePoses[ index ], fPoseRaw );

	AGKQuaternion quat( -fPoseRaw[3], fPoseRaw[0], fPoseRaw[1], -fPoseRaw[2] );
	return quat.GetEulerZ();
#else
	return 0;
#endif
}

float agk::ARGetPlaneSizeX( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanes ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;
	
	float sizeX = 0;
	ArPlane_getExtentX( g_pARSession, g_pARPlanes[ index ], &sizeX );

	return sizeX;
#else
	return 0;
#endif
}

float agk::ARGetPlaneSizeZ( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanes ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;
	
	float sizeZ = 0;
	ArPlane_getExtentZ( g_pARSession, g_pARPlanes[ index ], &sizeZ );

	return sizeZ;
#else
	return 0;
#endif
}

void agk::ARGetPlanesFinish()
//****
{
#ifdef AGK_USE_AR
	if ( g_pARPlanes )
	{
		for( int i = 0; i < g_iARPlanesSize; i++ ) ArTrackable_release( ArAsTrackable(g_pARPlanes[i]) );
		delete [] g_pARPlanes;
	}
	g_pARPlanes = 0;

	if ( g_pARPlanePoses )
	{
		for( int i = 0; i < g_iARPlanesSize; i++ ) ArPose_destroy( g_pARPlanePoses[i] );
		delete [] g_pARPlanePoses;
	}
	g_pARPlanePoses = 0;

	g_iARPlanesSize = 0;
#endif
}

int agk::ARCreateAnchorFromHitTest( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARHitResults ) return 0;
	if ( index < 1 || index > g_iARHitResultsSize ) return 0;
	index--;
	
	ArAnchor* anchor = 0;
    if ( ArHitResult_acquireNewAnchor(g_pARSession, g_pARHitResults[index], &anchor) != AR_SUCCESS )
	{
		agk::Warning( "Failed to get hit test anchor" );
		return 0;
    }

    ArTrackingState tracking_state = AR_TRACKING_STATE_STOPPED;
    ArAnchor_getTrackingState( g_pARSession, anchor, &tracking_state );
    if (tracking_state != AR_TRACKING_STATE_TRACKING) 
	{
		agk::Warning( "Failed to get hit test anchor, anchor is not currently tracked" );
		ArAnchor_release(anchor);
		return 0;
    }

	uint32_t anchorID = g_pARAnchorList.GetFreeID();
	if ( !anchorID )
	{
		agk::Warning( "Failed to get hit test anchor, no free IDs found" );
		return 0;
	}
	AGKAndroidARAnchor *pNewAnchor = new AGKAndroidARAnchor();
	pNewAnchor->m_pARAnchor = anchor;
	pNewAnchor->Update();

	g_pARAnchorList.AddItem( pNewAnchor, anchorID );
	return anchorID;
#else
	return 0;
#endif
}

int agk::ARCreateAnchorFromPlane( int index )
//****
{
#ifdef AGK_USE_AR
	if ( !g_pARPlanePoses ) return 0;
	if ( index < 1 || index > g_iARPlanesSize ) return 0;
	index--;

	ArAnchor* anchor = 0;
    if ( ArSession_acquireNewAnchor(g_pARSession, g_pARPlanePoses[ index ], &anchor) != AR_SUCCESS )
	{
		agk::Warning( "Failed to create plane anchor" );
		return 0;
    }

    ArTrackingState tracking_state = AR_TRACKING_STATE_STOPPED;
    ArAnchor_getTrackingState( g_pARSession, anchor, &tracking_state );
    if (tracking_state != AR_TRACKING_STATE_TRACKING) 
	{
		agk::Warning( "Failed to create plane anchor, anchor is not currently tracked" );
		ArAnchor_release(anchor);
		return 0;
    }

	uint32_t anchorID = g_pARAnchorList.GetFreeID();
	if ( !anchorID )
	{
		agk::Warning( "Failed to create plane anchor, no free IDs found" );
		return 0;
	}
	AGKAndroidARAnchor *pNewAnchor = new AGKAndroidARAnchor();
	pNewAnchor->m_pARAnchor = anchor;

	float poseRaw[7];
	ArPose_getPoseRaw( g_pARSession, g_pARPlanePoses[ index ], poseRaw );
	
	pNewAnchor->SetNodePosition( poseRaw[4], poseRaw[5], -poseRaw[6] );
	pNewAnchor->SetNodeRotation( -poseRaw[3], poseRaw[0], poseRaw[1], -poseRaw[2] );

	g_pARAnchorList.AddItem( pNewAnchor, anchorID );
	return anchorID;
#else
	return 0;
#endif
}

void agk::ARFixObjectToAnchor( int objID, int anchorID )
//****
{
#ifdef AGK_USE_AR
	cObject3D *pObject = m_cObject3DList.GetItem( objID );
	if ( !pObject )
	{
		uString errStr; errStr.Format( "Failed to fix object %d - object does not exist", objID );
		Error( errStr );
		return;
	}

	if ( anchorID == 0 )
	{
		pObject->RemoveFromParent();
	}
	else
	{
		AGKAndroidARAnchor *pToAnchor = g_pARAnchorList.GetItem( anchorID );
		if ( !pToAnchor )
		{
			uString errStr; errStr.Format( "Failed to fix object to anchor %d - anchor does not exist", anchorID );
			Error( errStr );
			return;
		}

		pToAnchor->AddChild( pObject );
	}
#endif
}

int agk::ARGetAnchorStatus( int anchorID )
//****
{
#ifdef AGK_USE_AR
	AGKAndroidARAnchor *pAnchor = g_pARAnchorList.GetItem( anchorID );
	if ( !pAnchor )
	{
		uString errStr; errStr.Format( "Failed to get anchor status, anchor %d does not exist", anchorID );
		Error( errStr );
		return 0;
	}

	ArTrackingState tracking_state = AR_TRACKING_STATE_STOPPED;
    ArAnchor_getTrackingState( g_pARSession, pAnchor->m_pARAnchor, &tracking_state );

	switch( tracking_state )
	{
		case AR_TRACKING_STATE_STOPPED: return 0;
		case AR_TRACKING_STATE_PAUSED: return 1;
		case AR_TRACKING_STATE_TRACKING: return 2;
		default: return -1;
	}
#else
	return -1;
#endif
}

void agk::ARDeleteAnchor( int anchorID )
//****
{
#ifdef AGK_USE_AR
	AGKAndroidARAnchor *pAnchor = g_pARAnchorList.RemoveItem( anchorID );
	if ( pAnchor ) delete pAnchor;
#endif
}

int agk::GetAppInstalled( const char *packageName )
//****
{
#ifdef AGK_USE_AR
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass AGKHelper = GetAGKHelper(lJNIEnv);

	// get the method from our java class
	jmethodID method = lJNIEnv->GetStaticMethodID( AGKHelper, "GetPackageInstalled", "(Landroid/app/Activity;Ljava/lang/String;)I" );

	// call our java class method
	jstring strPackageName = lJNIEnv->NewStringUTF( packageName );
	int result = lJNIEnv->CallStaticIntMethod( AGKHelper, method, lNativeActivity, strPackageName );
	lJNIEnv->DeleteLocalRef( strPackageName );

	vm->DetachCurrentThread();

	return result;
#else
	return 0;
#endif
}

// SnapChat

void agk::SetSnapChatStickerSettings( float x, float y, int width, int height, float angle )
//****
{
	char str1[32];
	char str2[32];
	sprintf( str1, "%.6f", x );
	sprintf( str2, "%.6f", y );
	ExternalCommand( "snapchat", "setposition", str1, str2 );

	sprintf( str1, "%d", width );
	sprintf( str2, "%d", height );
	ExternalCommand( "snapchat", "setsize", str1, str2 );

	sprintf( str1, "%.6f", angle );
	ExternalCommand( "snapchat", "setangle", str1, "" );
}

void agk::ShareSnapChatImage( const char* imageFile, const char* stickerFile, const char* caption, const char* url )
//****
{
	if ( !imageFile || !*imageFile ) return;
	if ( !stickerFile ) stickerFile = "";
	if ( !caption ) caption = "";
	if ( !url ) url = "";

	uString sPath( imageFile );
    if ( !GetRealPath( sPath ) )
    {
        uString err; err.Format( "Could not find image at path: %s", imageFile );
        agk::Error( err );
        return;
    }

	if ( cFile::ExistsRead( imageFile ) && !cFile::ExistsWrite( imageFile ) ) 
	{
		// move file to write folder to avoid APK assets folder
		cFile cSrcFile;
		cSrcFile.OpenToRead( imageFile );

		cFile cDstFile;
		cDstFile.OpenToWrite( imageFile );

		char buf[ 4096 ];
		do
		{
			int written = cSrcFile.ReadData( buf, 4096 );
			cDstFile.WriteData( buf, written );
		} while( !cSrcFile.IsEOF() );

		cDstFile.Close();
		cSrcFile.Close();

		sPath.SetStr( imageFile );
		agk::PlatformGetFullPathWrite( sPath );
	}

	uString sPathSticker( stickerFile );
	if ( stickerFile && *stickerFile )
	{
		if ( !GetRealPath( sPathSticker ) )
		{
			uString err; err.Format( "Could not find sticker image at path: %s", stickerFile );
			agk::Error( err );
			return;
		}

		if ( cFile::ExistsRead( stickerFile ) && !cFile::ExistsWrite( stickerFile ) ) 
		{
			// move file to write folder to avoid APK assets folder
			cFile cSrcFile;
			cSrcFile.OpenToRead( stickerFile );

			cFile cDstFile;
			cDstFile.OpenToWrite( stickerFile );

			char buf[ 4096 ];
			do
			{
				int written = cSrcFile.ReadData( buf, 4096 );
				cDstFile.WriteData( buf, written );
			} while( !cSrcFile.IsEOF() );

			cDstFile.Close();
			cSrcFile.Close();

			sPathSticker.SetStr( stickerFile );
			agk::PlatformGetFullPathWrite( sPathSticker );
		}
	}

	ExternalCommand( "snapchat", "captionandurl", caption, url );
	ExternalCommand( "snapchat", "share", sPath.GetStr(), sPathSticker.GetStr() );
}

// Extensions
int agk::ExternalSDKSupported( const char* sdk )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	// get NativeActivity object (clazz)
	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ExternalCommands" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "IsExternalSDKSupported", "(Ljava/lang/String;)I" );
	if ( !method ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// call our java class method
	jstring jstrSDK = lJNIEnv->NewStringUTF( sdk );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method, jstrSDK );
	lJNIEnv->DeleteLocalRef( jstrSDK );

	vm->DetachCurrentThread();

	return result;
}

void agk::ExternalCommand( const char* sdk, const char* command, const char* str1, const char* str2 )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ExternalCommands" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "ExternalCommand", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );
	if ( !method ) 
	{
		vm->DetachCurrentThread();
		return;
	}

	// call our java class method
	jstring jstrSDK = lJNIEnv->NewStringUTF( sdk );
	jstring jstrCmd = lJNIEnv->NewStringUTF( command );
	jstring jstr1 = lJNIEnv->NewStringUTF( str1 );
	jstring jstr2 = lJNIEnv->NewStringUTF( str2 );
	lJNIEnv->CallStaticVoidMethod( sdkClass, method, lNativeActivity, jstrSDK, jstrCmd, jstr1, jstr2 );
	lJNIEnv->DeleteLocalRef( jstr2 );
	lJNIEnv->DeleteLocalRef( jstr1 );
	lJNIEnv->DeleteLocalRef( jstrCmd );
	lJNIEnv->DeleteLocalRef( jstrSDK );
	
	vm->DetachCurrentThread();
}

int agk::ExternalCommandInt( const char* sdk, const char* command, const char* str1, const char* str2 )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ExternalCommands" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "ExternalCommandInt", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I" );
	if ( !method ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// call our java class method
	jstring jstrSDK = lJNIEnv->NewStringUTF( sdk );
	jstring jstrCmd = lJNIEnv->NewStringUTF( command );
	jstring jstr1 = lJNIEnv->NewStringUTF( str1 );
	jstring jstr2 = lJNIEnv->NewStringUTF( str2 );
	int result = lJNIEnv->CallStaticIntMethod( sdkClass, method, lNativeActivity, jstrSDK, jstrCmd, jstr1, jstr2 );
	lJNIEnv->DeleteLocalRef( jstr2 );
	lJNIEnv->DeleteLocalRef( jstr1 );
	lJNIEnv->DeleteLocalRef( jstrCmd );
	lJNIEnv->DeleteLocalRef( jstrSDK );
	
	vm->DetachCurrentThread();

	return result;
}

float agk::ExternalCommandFloat( const char* sdk, const char* command, const char* str1, const char* str2 )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ExternalCommands" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "ExternalCommandFloat", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)F" );
	if ( !method ) 
	{
		vm->DetachCurrentThread();
		return 0;
	}

	// call our java class method
	jstring jstrSDK = lJNIEnv->NewStringUTF( sdk );
	jstring jstrCmd = lJNIEnv->NewStringUTF( command );
	jstring jstr1 = lJNIEnv->NewStringUTF( str1 );
	jstring jstr2 = lJNIEnv->NewStringUTF( str2 );
	float result = lJNIEnv->CallStaticFloatMethod( sdkClass, method, lNativeActivity, jstrSDK, jstrCmd, jstr1, jstr2 );
	lJNIEnv->DeleteLocalRef( jstr2 );
	lJNIEnv->DeleteLocalRef( jstr1 );
	lJNIEnv->DeleteLocalRef( jstrCmd );
	lJNIEnv->DeleteLocalRef( jstrSDK );
	
	vm->DetachCurrentThread();

	return result;
}

char* agk::ExternalCommandString( const char* sdk, const char* command, const char* str1, const char* str2 )
//****
{
	JNIEnv* lJNIEnv = g_pActivity->env;
	JavaVM* vm = g_pActivity->vm;
	vm->AttachCurrentThread(&lJNIEnv, NULL);

	jobject lNativeActivity = g_pActivity->clazz;
	if ( !lNativeActivity ) agk::Warning("Failed to get native activity pointer");
	
	jclass sdkClass = GetAGKClass( lJNIEnv, "com/thegamecreators/agk_player/ExternalCommands" );
	if ( !sdkClass ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	jmethodID method = lJNIEnv->GetStaticMethodID( sdkClass, "ExternalCommandString", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;" );
	if ( !method ) 
	{
		vm->DetachCurrentThread();
		char* str = new char[1];
		*str = 0;
		return str;
	}

	// call our java class method
	jstring jstrSDK = lJNIEnv->NewStringUTF( sdk );
	jstring jstrCmd = lJNIEnv->NewStringUTF( command );
	jstring jstr1 = lJNIEnv->NewStringUTF( str1 );
	jstring jstr2 = lJNIEnv->NewStringUTF( str2 );
	jstring result = (jstring) lJNIEnv->CallStaticObjectMethod( sdkClass, method, lNativeActivity, jstrSDK, jstrCmd, jstr1, jstr2 );
	lJNIEnv->DeleteLocalRef( jstr2 );
	lJNIEnv->DeleteLocalRef( jstr1 );
	lJNIEnv->DeleteLocalRef( jstrCmd );
	lJNIEnv->DeleteLocalRef( jstrSDK );

	jboolean bCopy;
	const char* szResult = lJNIEnv->GetStringUTFChars( result, &bCopy );

	char *str = new char[ strlen(szResult)+1 ];
	strcpy( str, szResult );

	lJNIEnv->ReleaseStringUTFChars( result, szResult );
	lJNIEnv->DeleteLocalRef( result );
	
	vm->DetachCurrentThread();

	return str;
}