// Sound Header
#ifndef _H_SOUND_
#define _H_SOUND_

// Common includes
#include "Common.h"
#include "uString.h"
#include "cFile.h"
#include "Thread.h"

#define MAX_SOUND_FILES 300
#define MAX_MUSIC_FILES 50
#define MAX_MUSIC_FILES_OGG 100

#define AGK_MUSICMGR_OGG_THREADED	0x01

#define AGK_MUSIC_OGG_LOOP			0x01
#define AGK_MUSIC_OGG_PLAYING		0x02
#define AGK_MUSIC_OGG_PAUSED		0x04
#define AGK_MUSIC_OGG_BUFFER_END	0x08
#define AGK_MUSIC_OGG_SHOULD_RESUME	0x10

#define AGK_OGG_BUFFER_SIZE 65536

struct oggMemData
{
	uint32_t pos = 0;
	uint32_t size = 0;
	uint8_t* pData = 0;
};

struct OggVorbis_File;

// Namespace
namespace AGK
{
	typedef struct tWaveFormat
	{
		unsigned short  wFormatTag;         /* format type */
		unsigned short  nChannels;          /* number of channels (i.e. mono, stereo...) */
		unsigned int    nSamplesPerSec;     /* sample rate */
		unsigned int    nAvgBytesPerSec;    /* for buffer estimation */
		unsigned short  nBlockAlign;        /* block size of data */
		unsigned short  wBitsPerSample;     /* number of bits per sample of mono data */
		unsigned short  cbSize;             /* the count in bytes of the size of extra data */
	} AGKWAVEFORMATEX;

	class cMusicMgr
	{
		protected:
			class cMusic
			{
				public:
					uint32_t m_iID;
					uString m_sFile;
					int m_iVolume;
			};

			cMusic* m_pMusicFiles[ MAX_MUSIC_FILES ];
			static cMusic *m_pCurrentlyPlaying;
			uint32_t m_iStartID;
			uint32_t m_iEndID;
			bool m_bLoop;
			int m_iMasterVolume;
			uint32_t m_iLastID;

			//cMusic* GetMusic( int iID );

			static void PlatformAddFile( cMusic *pMusic );
			bool PlatformPrepare( uint32_t iID );

		public:
			cMusicMgr();
			~cMusicMgr();

			void AddFile( uint32_t iID, const uString &szFile );
			uint32_t AddFile( const uString &szFile );
			void SetFileVolume( uint32_t iID, int volume ); 
			void Play( uint32_t iID=1, bool bLoop=true, uint32_t iStartID=1, uint32_t iEndID=MAX_MUSIC_FILES-1 );
			void Pause();
			void Resume();
			void Stop();
			void RemoveFile( uint32_t iID );
			void ClearAll();
			void HandleEvent();
			uint32_t CurrentlyPlaying();
			void SetMasterVolume( int vol );
			int GetMasterVolume() { return m_iMasterVolume; }
			uint32_t Exists( uint32_t iID );

			float GetDuration( uint32_t ID );
			void Seek( float seconds, int mode );
			float GetPosition();
	};

	class AGKMusicOGG
	{
		protected:
			class AGKMusicMgrOGG : public AGKThread
			{
				protected:
					unsigned int Run();

				public:
					AGKMusicMgrOGG();
					~AGKMusicMgrOGG();
			};

			int m_iVolume;
			AGKMusicOGG *m_pPrev;
			AGKMusicOGG *m_pNext;
			unsigned int m_iFlags;
			int m_iLoopCount;
			int m_iLoop;
						
			// ogg data
			cSpinLock m_filelock;
			OggVorbis_File *m_pOggFile;
			cFile m_cFile;
			uString m_sFile;
			oggMemData m_memData;
			
			// sound data
			AGKWAVEFORMATEX m_fmt;
			float m_fDuration;
			int m_iTotalSamples;
			float m_fCurrTime;
			double m_fLastTime;
			void *m_pSoundData; // used for platform specific data
			float m_fLoopStartTime;
			float m_fLoopEndTime;

			// global data
			static cSpinLock g_alllock; // if needed then it *must* be aquired before m_filelock
			static AGKMusicOGG *g_pAllMusic;
			static AGKMusicMgrOGG *g_pUpdateThread;
			static unsigned char *m_pDecodeBuffer;
			static unsigned int m_iBufferSize;
			static bool g_bDeletingAll;
			static int g_iMasterVolume;

			void PlatformInit();
			void PlatformCleanUp();
			int PlatformPlay();
			void PlatformSetVolume();
			void PlatformPause();
			void PlatformResume();
			void PlatformStop();
			void PlatformClearBuffers();
			void PlatformReset();
			int PlatformAddBuffer( int *reset );
			int PlatformGetNumBuffers();
			int PlatformGetMaxBuffers();

			static void UpdateAllInternal();
			
		public:
			bool m_bFinished;

			AGKMusicOGG();
			~AGKMusicOGG();

			void Reset();

			int Load( const uString &szFilename );
			int LoadMem( const uint8_t* data, uint32_t size );
			void SetVolume( int volume ); 
			void Play( int iLoop );
			void Pause();
			void Resume();
			void Stop();
			void Seek( float seconds, int mode );
			void SetLoopTimes( float starttime, float endtime );
			void SetLoopCount(int loop);

			float GetDuration() { return m_fDuration; }
			float GetPosition() { return m_fCurrTime; }
			int GetPlaying() { return (m_iFlags & AGK_MUSIC_OGG_PLAYING) ? 1 : 0; }
			int GetLoopCount() { return m_iLoopCount; }

			void Update();
			static void UpdateAll();
			
			static void AppPaused();
			static void AppResumed();
			static void DeleteAll();
			static void SetMasterVolume( int vol );
	};

	// to be define by the platform, requires device specific pointers for playing
	class cSoundInst;

	class cSoundMgr
	{
		public:

			class cSoundFile
			{
				public:
					uint32_t m_iID;
					uString m_sFile;
					unsigned char *m_pRawData;
					uint32_t m_uDataSize;
					AGKWAVEFORMATEX m_fmt;
					int m_iMax;
					uint32_t m_iInstances;

					cSoundFile();
					~cSoundFile();
			};

		protected:

			// between 0 and 100
			static int m_iGlobalVolume;

			static cSoundFile* m_pSoundFiles[ MAX_SOUND_FILES ];
			// cSoundInst is platform specific
			static cSoundInst *m_pSounds;
			// keep a list of used instances that have stopped so we don't have to reallocate the memory later
			static cSoundInst *m_pUsedSounds;

			static uint32_t m_iLastID;
			static float m_fMinPlaybackRate;
			static float m_fMaxPlaybackRate;
			static float m_fStepPlaybackRate;

			static void PlatformInit();
			static void PlatformUpdateVolume( );
			static void PlatformUpdate();
			static void PlatformCleanUp();
			static void PlatformAddFile( cSoundFile *pSound );

			static uint32_t PlatformCreateInstance( cSoundFile *pSound, int iVol=100, int iLoop=0, int iPriority=0 );
			static void PlatformStopInstances( uint32_t iID=0 );
			//static void PlatformPauseInstances( uint32_t iID=0 );
			//static void PlatformResumeInstances( uint32_t iID=0 );

			static void ProcessOGG( cSoundFile* pSound, OggVorbis_File* oggFile );

			cSoundMgr() {}

		public:

			static cSoundInst* GetCurrentSounds() { return m_pSounds; }

			static void Init();
			static void Destroy() { PlatformCleanUp(); }
            static void AppPaused();
            static void AppResumed();
        
			static void SetVolume( int vol );
			static void UpdateSounds();
			static int GetVolume() { return m_iGlobalVolume; }
			static float GetMinRate() { return m_fMinPlaybackRate; }
			static float GetMaxRate() { return m_fMaxPlaybackRate; }

			static uint32_t GetFreeID();

			static void AddFile( uint32_t iID, const uString &szFile, int iMaxInstances=-1 );
			static uint32_t AddFile( const uString &szFile, int iMaxInstances=-1 );
			static void AddFile( uint32_t iID, const unsigned char *pData, uint32_t size, int iMaxInstances=-1 );
			static uint32_t AddFile( const unsigned char *pData, uint32_t size, int iMaxInstances=-1 );

			static uint32_t AddOGGFile( const uString &szFile, int iMaxInstances=-1 );
			static void AddOGGFile( uint32_t iID, const uString &szFile, int iMaxInstances=-1 );

			static uint32_t AddOGGMem( const uint8_t* data, uint32_t size, int iMaxInstances=-1 );
			static void AddOGGMem( uint32_t iID, const uint8_t* data, uint32_t size, int iMaxInstances=-1 );

			static uint32_t PlayInstance( uint32_t iID, int iVol=100, int iLoop=0, int iPriority=0 );
			//static void PauseAll( uint32_t iID=0 );
			//static void ResumeAll( uint32_t iID=0 );
			static cSoundFile* GetFile( uint32_t iID );
			static void SaveFile( uint32_t iID, const uString &szFile );

			static void SetInstanceVolume( uint32_t instance, int vol );
			static void SetInstanceRate( uint32_t instance, float rate );
			static void SetInstanceBalance( uint32_t instance, float balance );
			static int GetInstanceVolume( uint32_t instance );
			static float GetInstanceRate( uint32_t instance );
			static int GetInstancePlaying( uint32_t instance );
			static int GetInstanceLoopCount( uint32_t instance );
			static void StopInstance( uint32_t instance );

			static void StopAll( uint32_t iID=0 );
			static void DeleteFile( uint32_t iID=0 );
			static uint32_t CountInstances( uint32_t iID );
			static uint32_t Exists( uint32_t iID );
	};
}

#endif
