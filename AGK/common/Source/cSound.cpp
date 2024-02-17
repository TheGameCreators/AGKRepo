//#include "cSound.h"
//#include "Wrapper.h"
#include "agk.h"

#include "vorbis/vorbisfile.h"

using namespace AGK;

// 
// OGG file access callbacks
//

size_t AGKSoundFileRead(void *pData, size_t size, size_t count, void *pUser)
{
	AGK::cFile *pFile = (AGK::cFile*) pUser;
	if ( !pFile ) return 0;

	return pFile->ReadData( (char*)pData, (unsigned int) (size*count) );
}

int AGKSoundFileSeek(void *pUser, ogg_int64_t offset, int whence)
{
	AGK::cFile *pFile = (AGK::cFile*) pUser;
	if ( !pFile ) return 1;

	if ( whence == SEEK_SET ) pFile->Seek( (uint32_t)offset );
	else if ( whence == SEEK_CUR ) pFile->Seek( (uint32_t)offset + pFile->GetPos() );
	else if ( whence == SEEK_END ) pFile->Seek( pFile->GetSize() + (uint32_t)offset );

	return 0;
}

int AGKSoundFileClose(void *pUser)
{
	AGK::cFile *pFile = (AGK::cFile*) pUser;
	if ( !pFile ) return 1;

	pFile->Close();
	return 0;
}

long AGKSoundFileTell(void *pUser)
{
	AGK::cFile *pFile = (AGK::cFile*) pUser;
	if ( !pFile ) return 0;

	return (long)pFile->GetPos();
}

// OGG memory access callbacks

size_t AGKSoundMemRead(void *pData, size_t size, size_t count, void *pUser)
{
	oggMemData *pMem = (oggMemData*) pUser;
	if ( !pMem ) return 0;

	uint32_t totalSize = (uint32_t) (size * count);
	if ( pMem->pos + totalSize > pMem->size ) totalSize = pMem->size - pMem->pos;

	memcpy( pData, pMem->pData + pMem->pos, totalSize );
	pMem->pos += totalSize;
	return totalSize;
}

int AGKSoundMemSeek(void *pUser, ogg_int64_t offset, int whence)
{
	oggMemData *pMem = (oggMemData*) pUser;
	if ( !pMem ) return 1;

	if ( whence == SEEK_SET ) pMem->pos = (uint32_t) offset;
	else if ( whence == SEEK_CUR ) pMem->pos += (uint32_t) offset;
	else if ( whence == SEEK_END ) pMem->pos = pMem->size + (uint32_t)offset;

	return 0;
}

int AGKSoundMemClose(void *pUser)
{
	return 0;
}

long AGKSoundMemTell(void *pUser)
{
	oggMemData *pMem = (oggMemData*) pUser;
	if ( !pMem ) return 0;

	return (long)pMem->pos;
}

//
// Music
//

cMusicMgr::cMusic* cMusicMgr::m_pCurrentlyPlaying = 0;

cMusicMgr::cMusicMgr()
{
	for ( uint32_t i = 0; i < MAX_MUSIC_FILES; i++ )
	{
		m_pMusicFiles[ i ] = 0;
	}
	m_iStartID = 0;
	m_iEndID = 0;
	m_bLoop = true;
	m_iMasterVolume = 100;
}

cMusicMgr::~cMusicMgr()
{
	ClearAll();
}


uint32_t cMusicMgr::AddFile( const uString &szFilename )
{
	uint32_t iID = m_iLastID;
	if ( iID >= MAX_MUSIC_FILES ) iID = 1;
	else iID++;

	while ( m_pMusicFiles[ iID ] && iID != m_iLastID )
	{
		iID++;
	}

	if ( iID == m_iLastID )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add msuic file ", 100 ); 
		str.Append( szFilename ).Append( " - No free ID found" );
		agk::Error( str );
#endif
		return 0;
	}

	cMusic *pMusic = new cMusic();
	m_pMusicFiles[ iID ] = pMusic;

	pMusic->m_sFile.SetStr( szFilename );
	if ( !agk::GetRealPath( pMusic->m_sFile ) )
	{
		uString err; err.Format( "Failed to load music file \"%s\" - file not found", szFilename.GetStr() );
		agk::Error( err );
	}

	pMusic->m_iID = iID;
	pMusic->m_iVolume = 100;

	PlatformAddFile( pMusic );
	return iID;
}

void cMusicMgr::AddFile( uint32_t iID, const uString &szFilename )
{
	//uString szFile( szFilename );

	if ( iID < 1 || iID >= MAX_MUSIC_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add music file ", 100 ); 
		str.Append( szFilename ).Append( " - ID must be between 1 and " ).AppendInt( MAX_MUSIC_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	if ( m_pMusicFiles[ iID ] )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add music file ", 100 );
		str.Append( szFilename ).Append(" - ID ").AppendUInt( iID ).Append( " already taken" );
		agk::Error( str );
#endif
		return;
	}

	cMusic *pMusic = new cMusic();
	m_pMusicFiles[ iID ] = pMusic;

    pMusic->m_sFile.SetStr( szFilename );
    if ( !agk::GetRealPath( pMusic->m_sFile ) )
    {
        uString err; err.Format( "Failed to load music file \"%s\" - file not found", szFilename.GetStr() );
        agk::Error( err );
    }
    
	pMusic->m_iID = iID;
	pMusic->m_iVolume = 100;

	PlatformAddFile( pMusic );
}


uint32_t cMusicMgr::Exists( uint32_t iID )
{
	if ( iID < 1 || iID >= MAX_MUSIC_FILES ) return 0;
	if ( !m_pMusicFiles[ iID ] ) return 0;
	return 1;
}

void cMusicMgr::SetFileVolume( uint32_t iID, int volume )
{
	if ( iID < 1 || iID >= MAX_MUSIC_FILES ) return;
	if ( !m_pMusicFiles[ iID ] ) return;

	if ( volume < 0 ) volume = 0;
	if ( volume > 100 ) volume = 100;
	m_pMusicFiles[ iID ]->m_iVolume = volume;

	if ( m_pMusicFiles[ iID ] == m_pCurrentlyPlaying )
	{
		SetMasterVolume( m_iMasterVolume );
	}
}


uint32_t cMusicMgr::CurrentlyPlaying()
{
	if ( !m_pCurrentlyPlaying ) return 0;

	for ( int i = 0; i < MAX_MUSIC_FILES; i++ )
	{
		if ( m_pMusicFiles[ i ] == m_pCurrentlyPlaying )
		{
			return m_pMusicFiles[ i ]->m_iID;
		}
	}

	return 0;
}

void cMusicMgr::RemoveFile( uint32_t iID )
{
	if ( iID < 1 || iID >= MAX_MUSIC_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not remove music file ", 100 ); 
		str.AppendUInt( iID ).Append( " - ID must be between 1 and " ).AppendInt( MAX_MUSIC_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	if ( !m_pMusicFiles[ iID ] ) return;

	if ( m_pMusicFiles[ iID ] == m_pCurrentlyPlaying )
	{
		Stop();
	}

	delete m_pMusicFiles[ iID ];
	m_pMusicFiles[ iID ] = 0;
}

void cMusicMgr::ClearAll()
{
	for ( uint32_t i = 0; i < MAX_MUSIC_FILES; i++ )
	{
		if ( !m_pMusicFiles[ i ] ) continue;
		if ( m_pMusicFiles[ i ] == m_pCurrentlyPlaying )
		{
			//Stop();
		}

		delete m_pMusicFiles[ i ];
		m_pMusicFiles[ i ] = 0;
	}
}

//
// OGG Music Mgr
//

AGKMusicOGG::AGKMusicMgrOGG::AGKMusicMgrOGG()
{
	if ( agk::GetNumProcessors() > 1 ) 
	{
		Start();
	}
}

AGKMusicOGG::AGKMusicMgrOGG::~AGKMusicMgrOGG()
{
	Stop();
}

unsigned int AGKMusicOGG::AGKMusicMgrOGG::Run()
{
	while( !m_bTerminate )
	{
		AGKMusicOGG::UpdateAllInternal();
		SleepSafe( 33 );
	}

	return 0;
}

// 
// OGG Music
// 

AGKMusicOGG *AGKMusicOGG::g_pAllMusic = 0;
AGKMusicOGG::AGKMusicMgrOGG *AGKMusicOGG::g_pUpdateThread = 0;
cSpinLock AGKMusicOGG::g_alllock;
unsigned char *AGKMusicOGG::m_pDecodeBuffer = 0;
unsigned int AGKMusicOGG::m_iBufferSize = 0;
bool AGKMusicOGG::g_bDeletingAll = false;
int AGKMusicOGG::g_iMasterVolume = 100;

AGKMusicOGG::AGKMusicOGG()
{
	m_iVolume = 100;
	m_iFlags = 0;
	m_fDuration = 0;
	m_iTotalSamples = 0;
	m_pOggFile = 0;
	m_pSoundData = 0;
	m_iLoop = 0;
	m_iLoopCount = 0;
	m_fCurrTime = 0;
	m_bFinished = false;
	m_fLoopStartTime = -1;
	m_fLoopEndTime = -1;

	cAutoSLock autolock( &g_alllock ); // automatically unlocks

	if ( !m_pDecodeBuffer ) m_pDecodeBuffer = new unsigned char[ AGK_OGG_BUFFER_SIZE ];
	if ( !g_pUpdateThread ) g_pUpdateThread = new AGKMusicMgrOGG();
	
	// add to list of all music files
	if ( g_pAllMusic ) g_pAllMusic->m_pPrev = this;
	m_pNext = g_pAllMusic;
	m_pPrev = 0;
	g_pAllMusic = this;
}

AGKMusicOGG::~AGKMusicOGG()
{
	if ( !g_bDeletingAll )
	{
		cAutoSLock autolock( &g_alllock ); // automatically unlocks

		// remove from list of all music files
		if ( m_pPrev ) m_pPrev->m_pNext = m_pNext;
		else g_pAllMusic = m_pNext;
		if ( m_pNext ) m_pNext->m_pPrev = m_pPrev;
	
		/* no need to keep deleting and creating the thread, and may be causing a freeze on Android. Could be caused if the thread is deleted whilst it is still waiting on g_alllock
		if ( !g_pAllMusic && g_pUpdateThread ) 
		{
			delete g_pUpdateThread;
			g_pUpdateThread = 0;
		}
		*/
	}

	Stop();
	
	if ( m_pOggFile )
	{
		ov_clear( m_pOggFile );
		delete m_pOggFile;
		m_pOggFile = 0;
	}

	if ( m_memData.pData ) delete [] m_memData.pData;
	m_memData.pData = 0;
	m_memData.size = 0;

	PlatformCleanUp();
}

void AGKMusicOGG::Reset()
{
	Stop();

	cAutoSLock autolock( &m_filelock );

	if ( m_memData.pData ) delete [] m_memData.pData;
	m_memData.pData = 0;
	m_memData.size = 0;

	m_iVolume = 100;
	m_iFlags = 0;
	m_fDuration = 0;
	m_iTotalSamples = 0;
	m_iLoop = 0;
	m_iLoopCount = 0;
	m_fCurrTime = 0;
	m_bFinished = false;
	m_fLoopStartTime = -1;
	m_fLoopEndTime = -1;

	if ( m_pOggFile )
	{
		ov_clear( m_pOggFile );
		delete m_pOggFile;
		m_pOggFile = 0;
	}

	PlatformReset();
}

int AGKMusicOGG::Load( const uString &szFilename )
{
	if ( m_pOggFile ) Reset();

	cAutoSLock autolock( &m_filelock );

	uString szFile( szFilename );
	szFile.Replace( '\\', '/' );
	m_sFile.SetStr( szFile );

	ov_callbacks callbacks = {
		AGKSoundFileRead,
		AGKSoundFileSeek,
		AGKSoundFileClose,
		AGKSoundFileTell
	};

	if ( !m_cFile.OpenToRead( szFile ) )
	{
		uString str;
		str.Format( "Failed to open OGG music file: %s", szFile.GetStr() );
		agk::Error( str );
		return 0;
	}

	m_pOggFile = new OggVorbis_File();
	if ( ov_open_callbacks( &m_cFile, m_pOggFile, NULL, 0, callbacks ) != 0 )
	{
		m_cFile.Close();
		delete m_pOggFile;
		m_pOggFile = 0;

		uString str;
		str.Format( "Failed to read OGG file: %s", szFile.GetStr() );
		agk::Error( str );
		return 0;
	}

	vorbis_info *pInfo = ov_info( m_pOggFile, -1 );

	// wave header
	m_fmt.wFormatTag = 1;
	m_fmt.nChannels = pInfo->channels;
	m_fmt.nSamplesPerSec = pInfo->rate;
	m_fmt.wBitsPerSample = 16;
	m_fmt.nBlockAlign = m_fmt.nChannels * (m_fmt.wBitsPerSample / 8);
	m_fmt.nAvgBytesPerSec = m_fmt.nSamplesPerSec * m_fmt.nBlockAlign;
	m_fmt.cbSize = 0;

	m_iTotalSamples = (int) ov_pcm_total( m_pOggFile, -1 );
	m_fDuration = m_iTotalSamples / (float)m_fmt.nSamplesPerSec;
	m_fCurrTime = 0;

	if ( m_fDuration < 0.5 )
	{
		m_cFile.Close();
		delete m_pOggFile;
		m_pOggFile = 0;

		agk::Error( "Failed to load music file, duration must be greater than 0.5 seconds, use LoadSoundOGG instead" );
		return 0;
	}

	PlatformInit();

	return 1;
}

int AGKMusicOGG::LoadMem( const uint8_t* data, uint32_t size )
{
	if ( m_pOggFile ) Reset();

	cAutoSLock autolock( &m_filelock );
	
	m_sFile.SetStr( "" );

	ov_callbacks callbacks = {
		AGKSoundMemRead,
		AGKSoundMemSeek,
		AGKSoundMemClose,
		AGKSoundMemTell
	};

	m_memData.pData = new uint8_t[ size ];
	memcpy( m_memData.pData, data, size );
	m_memData.pos = 0;
	m_memData.size = size;

	m_pOggFile = new OggVorbis_File();
	if ( ov_open_callbacks( &m_memData, m_pOggFile, NULL, 0, callbacks ) != 0 )
	{
		delete [] m_memData.pData;
		m_memData.pData = 0;
		m_memData.size = 0;
		delete m_pOggFile;
		m_pOggFile = 0;

		agk::Error( "Failed to read OGG data" );
		return 0;
	}

	vorbis_info *pInfo = ov_info( m_pOggFile, -1 );

	// wave header
	m_fmt.wFormatTag = 1;
	m_fmt.nChannels = pInfo->channels;
	m_fmt.nSamplesPerSec = pInfo->rate;
	m_fmt.wBitsPerSample = 16;
	m_fmt.nBlockAlign = m_fmt.nChannels * (m_fmt.wBitsPerSample / 8);
	m_fmt.nAvgBytesPerSec = m_fmt.nSamplesPerSec * m_fmt.nBlockAlign;
	m_fmt.cbSize = 0;

	m_iTotalSamples = (int) ov_pcm_total( m_pOggFile, -1 );
	m_fDuration = m_iTotalSamples / (float)m_fmt.nSamplesPerSec;
	m_fCurrTime = 0;

	if ( m_fDuration < 0.5 )
	{
		delete [] m_memData.pData;
		m_memData.pData = 0;
		m_memData.size = 0;
		delete m_pOggFile;
		m_pOggFile = 0;

		agk::Error( "Failed to load music file, duration must be greater than 0.5 seconds, use LoadSoundOGG instead" );
		return 0;
	}

	PlatformInit();

	return 1;
}

void AGKMusicOGG::SetVolume( int volume )
{
	cAutoSLock autolock( &m_filelock );

	if ( volume > 100 ) volume = 100;
	if ( volume < 0 ) volume = 0;
	m_iVolume = volume;

	PlatformSetVolume();
}

void AGKMusicOGG::Play( int iLoop )
{
	if ( m_iFlags & AGK_MUSIC_OGG_PLAYING ) Stop();

	cAutoSLock autolock( &m_filelock );

	m_iFlags &= ~AGK_MUSIC_OGG_PLAYING;
	m_iFlags &= ~AGK_MUSIC_OGG_PAUSED;
	m_iFlags &= ~AGK_MUSIC_OGG_BUFFER_END;

	m_iLoop = iLoop;
	m_iLoopCount = 0;

	int ptr = 0;
	int bytes = 0;
	int remaining = AGK_OGG_BUFFER_SIZE; 
	// can reduce main thread work if the decode thread is running
	if ( g_pUpdateThread && g_pUpdateThread->IsRunning() ) remaining = AGK_OGG_BUFFER_SIZE / 2;
	do
	{
		int bitstream;
		bytes = ov_read( m_pOggFile, (char*)(m_pDecodeBuffer+ptr), remaining, 0, 2, 1, &bitstream );
		ptr += bytes;
		remaining -= bytes;
	} while( remaining > 0 && bytes > 0 );

	if ( bytes == 0 ) m_iFlags |= AGK_MUSIC_OGG_BUFFER_END;
	m_iBufferSize = ptr;

	if ( !PlatformAddBuffer(0) ) 
	{
		Stop();
		return;
	}
	
	if ( PlatformPlay() ) m_iFlags |= AGK_MUSIC_OGG_PLAYING;

	m_bFinished = false;
	m_fLastTime = agk::PlatformGetRawTime();
}

void AGKMusicOGG::Pause()
{
	cAutoSLock autolock( &m_filelock );

	if ( (m_iFlags & AGK_MUSIC_OGG_PLAYING) == 0 ) return;

	m_iFlags |= AGK_MUSIC_OGG_PAUSED;
	PlatformPause();
}

void AGKMusicOGG::Resume()
{
	cAutoSLock autolock( &m_filelock );

	if ( (m_iFlags & AGK_MUSIC_OGG_PLAYING) == 0 ) return;

	m_iFlags &= ~AGK_MUSIC_OGG_PAUSED;
	PlatformResume();
}

void AGKMusicOGG::Stop()
{
	cAutoSLock autolock( &m_filelock );

	if ( (m_iFlags & AGK_MUSIC_OGG_PLAYING) == 0 ) return;

	m_iFlags &= ~AGK_MUSIC_OGG_PAUSED;
	m_iFlags &= ~AGK_MUSIC_OGG_PLAYING;
	m_iFlags &= ~AGK_MUSIC_OGG_BUFFER_END;
	PlatformStop();

	ov_time_seek_lap( m_pOggFile, 0 );
	m_fCurrTime = 0;
	m_bFinished = false;
}

void AGKMusicOGG::Seek( float seconds, int mode )
{
	cAutoSLock autolock( &m_filelock );

	if ( !m_pOggFile ) return;

	float pos = seconds;
	if ( mode == 1 ) pos += m_fCurrTime;
	if ( pos == m_fCurrTime ) return;

	ov_time_seek_lap( m_pOggFile, pos );

	m_fCurrTime = (float) ov_time_tell( m_pOggFile );

	if ( m_iFlags & AGK_MUSIC_OGG_PLAYING )
	{
		m_iFlags &= ~AGK_MUSIC_OGG_BUFFER_END;

		int ptr = 0;
		int bytes = 0;
		int remaining = AGK_OGG_BUFFER_SIZE / 2; // use smaller buffer for main thread decode
		do
		{
			int bitstream;
			bytes = ov_read( m_pOggFile, (char*)(m_pDecodeBuffer+ptr), remaining, 0, 2, 1, &bitstream );
			ptr += bytes;
			remaining -= bytes;
		} while( remaining > 0 && bytes > 0 );

		if ( bytes == 0 ) m_iFlags |= AGK_MUSIC_OGG_BUFFER_END;
		m_iBufferSize = ptr;

		PlatformClearBuffers();

		if ( !PlatformAddBuffer(0) )
		{
			m_iFlags &= ~AGK_MUSIC_OGG_PLAYING;
			return;
		}

		if ( PlatformPlay() ) m_iFlags |= AGK_MUSIC_OGG_PLAYING;
		else m_iFlags &= ~AGK_MUSIC_OGG_PLAYING;

		m_fLastTime = agk::PlatformGetRawTime();
	}
}

void AGKMusicOGG::SetLoopTimes( float starttime, float endtime )
{
	cAutoSLock autolock(&m_filelock);

	if (starttime < 0) starttime = -1;
	if (endtime < 0) endtime = -1;
	if (starttime > endtime && endtime >= 0)
	{
		agk::Error("Failed to set OGG loop times, end time must be greater than start time, or equal to -1 to play to the end");
		return;
	}

	m_fLoopStartTime = starttime;
	m_fLoopEndTime = endtime;
}

void AGKMusicOGG::SetLoopCount(int loop)
{
	cAutoSLock autolock(&m_filelock);

	m_iLoop = loop;
	m_iLoopCount = 0;
}

void AGKMusicOGG::UpdateAllInternal()
{
	if ( !g_pAllMusic ) return;

	cAutoSLock autolock( &g_alllock ); // automatically unlocks

	AGKMusicOGG *pMusic = g_pAllMusic;
	while( pMusic )
	{
		pMusic->Update();
		pMusic = pMusic->m_pNext;
	}
}

void AGKMusicOGG::UpdateAll()
{
	// if the update thread is running then it will handle the updates, but Android requires sounds be stopped by the main thread
	if ( g_pUpdateThread && g_pUpdateThread->IsRunning() ) 
	{
		cAutoSLock autolock( &g_alllock );

		AGKMusicOGG *pMusic = g_pAllMusic;
		while( pMusic )
		{
			if ( pMusic->m_bFinished ) pMusic->Stop();
			pMusic = pMusic->m_pNext;
		}
	}
	else
	{
		// not threaded
		UpdateAllInternal();
		
		AGKMusicOGG *pMusic = g_pAllMusic;
		while( pMusic )
		{
			if ( pMusic->m_bFinished ) pMusic->Stop();
			pMusic = pMusic->m_pNext;
		}
	}
}

void AGKMusicOGG::AppPaused()
{
	cAutoSLock autolock( &g_alllock );

	AGKMusicOGG *pMusic = g_pAllMusic;
	while( pMusic )
	{
		if ( (pMusic->m_iFlags & AGK_MUSIC_OGG_PLAYING) && !(pMusic->m_iFlags & AGK_MUSIC_OGG_PAUSED) ) 
		{
			pMusic->Pause();
			pMusic->m_iFlags |= AGK_MUSIC_OGG_SHOULD_RESUME;
		}
		else pMusic->m_iFlags &= ~AGK_MUSIC_OGG_SHOULD_RESUME;

		pMusic = pMusic->m_pNext;
	}
}

void AGKMusicOGG::AppResumed()
{
	cAutoSLock autolock( &g_alllock );

	AGKMusicOGG *pMusic = g_pAllMusic;
	while( pMusic )
	{
		if ( pMusic->m_iFlags & AGK_MUSIC_OGG_SHOULD_RESUME ) 
		{
			pMusic->Resume();
			pMusic->m_iFlags &= ~AGK_MUSIC_OGG_SHOULD_RESUME;
		}

		pMusic = pMusic->m_pNext;
	}
}

void AGKMusicOGG::DeleteAll()
{
	{
		cAutoSLock autolock( &g_alllock );
		g_bDeletingAll = true;
	
		while( g_pAllMusic )
		{
			AGKMusicOGG *pMusic = g_pAllMusic;
			g_pAllMusic = g_pAllMusic->m_pNext;
			delete pMusic;
		}
		
		g_bDeletingAll = false;
	}
	
	/* 
	if ( g_pUpdateThread ) 
	{
		delete g_pUpdateThread;
		g_pUpdateThread = 0;
	}
	*/
}

void AGKMusicOGG::SetMasterVolume( int vol )
{
	if ( vol < 0 ) vol = 0;
	if ( vol > 100 ) vol = 100;
	g_iMasterVolume = vol;

	{
		cAutoSLock autolock( &g_alllock );
	
		AGKMusicOGG *pMusic = g_pAllMusic;
		while( pMusic )
		{
			{
				cAutoSLock autolock( &pMusic->m_filelock );
				pMusic->PlatformSetVolume();
			}
			pMusic = pMusic->m_pNext;
		}
		
		g_bDeletingAll = false;
	}
}

void AGKMusicOGG::Update()
{
	cAutoSLock autolock( &m_filelock );

	if ( (m_iFlags & AGK_MUSIC_OGG_PLAYING) == 0 ) return;
	if ( m_bFinished ) return;

	double currtime = agk::PlatformGetRawTime();
	if ( (m_iFlags & AGK_MUSIC_OGG_PAUSED) == 0 ) m_fCurrTime += (float)(currtime - m_fLastTime);
	m_fLastTime = currtime;
	
	int shouldLoop = 0;
	if ( m_iLoop == 1 ) shouldLoop = 1;
	else if ( m_iLoopCount+1 < m_iLoop ) shouldLoop = 1;

	int maxBuffers = PlatformGetMaxBuffers();

	int buffersRemaining = PlatformGetNumBuffers();
	if ( buffersRemaining >= maxBuffers ) return;
	else if ( buffersRemaining == 0 )
	{
		if ( shouldLoop == 0 && (m_iFlags & AGK_MUSIC_OGG_BUFFER_END) )
		{
			m_iLoopCount++;
			m_bFinished = true;
			//Stop(); // Android doesn't like this, possibly because start and stop were called from different threads?
			return;
		}
	}

	int addBuffers = maxBuffers - buffersRemaining;

	while ( addBuffers > 0 )
	{
		if ( (m_iFlags & AGK_MUSIC_OGG_BUFFER_END) ) 
		{
			int shouldLoop = 0;
			if ( m_iLoop == 1 ) shouldLoop = 1;
			else if ( m_iLoopCount+1 < m_iLoop ) shouldLoop = 1;

			if ( !shouldLoop ) return; // wait for the buffers to run out
			else
			{
				ov_time_seek_lap( m_pOggFile, (m_fLoopStartTime >= 0) ? m_fLoopStartTime : 0 );
				m_iFlags &= ~AGK_MUSIC_OGG_BUFFER_END;
			}
		}

		int ptr = 0;
		int bytes = 0;
		int remaining = AGK_OGG_BUFFER_SIZE;

		// check decode time will not go over the loop end time
		int ending = 0;
		if (m_fLoopEndTime > 0)
		{
			float currTime = (float) ov_time_tell(m_pOggFile);

			int stride = 2;
			if (m_fmt.nChannels == 2) stride = 4;
			float diff = m_fLoopEndTime - currTime;
			if (remaining / (float) m_fmt.nSamplesPerSec / stride > diff )
			{
				remaining = (int) (diff * m_fmt.nSamplesPerSec * stride);
				remaining /= stride; // round to nearest stride value
				remaining *= stride;
				ending = 1;
			}
		}

		do
		{
			int bitstream;
			bytes = ov_read( m_pOggFile, (char*)(m_pDecodeBuffer+ptr), remaining, 0, 2, 1, &bitstream );
			ptr += bytes;
			remaining -= bytes;
		} while( remaining > 0 && bytes > 0 );

		int samples = (int) ov_pcm_tell( m_pOggFile );
		if ( samples >= m_iTotalSamples && bytes > 0 )
		{
			// reached end of stream, but haven't triggered ov_read to return 0
			int dump, bitstream;
			bytes = ov_read( m_pOggFile, (char*)&dump, 4, 0, 2, 1, &bitstream );
			if ( bytes > 0 )
			{
				agk::Warning( "Something went wrong trying to trigger end of OGG file" );
			}
		}

		if ( bytes == 0 || ending == 1 )
		{
			m_iFlags |= AGK_MUSIC_OGG_BUFFER_END;
		}
		
		m_iBufferSize = ptr;
		
		addBuffers--;

		int reset = 0;
		if ( !PlatformAddBuffer( &reset ) )
		{
			m_iFlags &= ~AGK_MUSIC_OGG_PLAYING;
			return;
		}

		if ( reset )
		{
			m_iLoopCount++;
			m_fCurrTime = (m_fLoopStartTime >= 0) ? m_fLoopStartTime : 0;
		}
	}
}


//
// Sound Effects
//

int cSoundMgr::m_iGlobalVolume = 100;
cSoundMgr::cSoundFile* cSoundMgr::m_pSoundFiles[ MAX_SOUND_FILES ] = { 0 };
cSoundInst* cSoundMgr::m_pSounds = 0;
cSoundInst* cSoundMgr::m_pUsedSounds = 0;

uint32_t cSoundMgr::m_iLastID = 0;
float cSoundMgr::m_fMinPlaybackRate = 1;
float cSoundMgr::m_fMaxPlaybackRate = 1;
float cSoundMgr::m_fStepPlaybackRate = 0;

cSoundMgr::cSoundFile::cSoundFile()
{
	m_iID = 0;
	m_pRawData = 0;
	m_uDataSize = 0;
	memset( &m_fmt, 0, sizeof(AGKWAVEFORMATEX) );
	m_iMax = 0;
	m_iInstances = 0;
}

cSoundMgr::cSoundFile::~cSoundFile()
{
	if ( m_pRawData ) delete [] m_pRawData;
}

void cSoundMgr::Init()
{
	for ( int i = 0; i < MAX_SOUND_FILES; i++ ) m_pSoundFiles[ i ] = 0;
	m_pSounds = 0;
	m_pUsedSounds = 0;

	PlatformInit();
}

void cSoundMgr::UpdateSounds()
{
	PlatformUpdate();
}

void cSoundMgr::SetVolume( int vol )
{
	if ( vol < 0 ) vol = 0;
	if ( vol > 100 ) vol = 100;
	m_iGlobalVolume = vol;

	PlatformUpdateVolume();
}


uint32_t cSoundMgr::GetFreeID()
{
	uint32_t iID = m_iLastID + 1;
	if ( iID >= MAX_SOUND_FILES ) iID = 1;
	
	while ( m_pSoundFiles[ iID ] && iID != m_iLastID )
	{
		iID++;
		if ( iID >= MAX_SOUND_FILES ) 
		{
			if ( m_iLastID == 0 ) iID = 0;
			else iID = 1;
		}
	}

	if ( iID == m_iLastID ) return 0;
	else return iID;
}


uint32_t cSoundMgr::AddFile( const uString &szFilename, int iMaxInstances )
{
	uint32_t iID = m_iLastID + 1;
	if ( iID >= MAX_SOUND_FILES ) iID = 1;
	
	while ( m_pSoundFiles[ iID ] && iID != m_iLastID )
	{
		iID++;
		if ( iID >= MAX_SOUND_FILES ) 
		{
			if ( m_iLastID == 0 ) iID = 0;
			else iID = 1;
		}
	}

	if ( iID == m_iLastID )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 ); 
		str.Append( szFilename ).Append( " - No free ID found" );
		agk::Error( str );
#endif
		return 0;
	}

	m_iLastID = iID;
	AddFile( iID, szFilename, iMaxInstances );

	return iID;
}

void cSoundMgr::AddFile( uint32_t iID, const uString &szFilename, int iMaxInstances )
{
	uString szFile( szFilename );
	szFile.Replace( '\\', '/' );

	if ( iID < 1 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 ); 
		str.Append( szFile ).Append( " - ID must be between 1 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	if ( m_pSoundFiles[ iID ] )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 );
		str.Append( szFile ).Append(" - ID ").AppendUInt( iID ).Append( " already taken" );
		agk::Error( str );
#endif
		return;
	}

	cFile oFile;
	if ( !oFile.OpenToRead( szFile ) )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 );
		str.Append( szFile ); str.Append(" - Could not load file" );
		agk::Error( str );
#endif
		return;
	}

	// chunk ID
	uint32_t dwData = 0;
	oFile.ReadData( (char*)&dwData, 4 );
	if ( dwData != 0x46464952 ) // RIFF backwards
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 );
		str.Append( szFile ); str.Append(" - Not a RIFF file" );
		agk::Error( str );
#endif
		oFile.Close();
		return;
	}

	// chunk size, not needed
	oFile.ReadData( (char*)&dwData, 4 );

	// file format
	oFile.ReadData( (char*)&dwData, 4 );
	if ( dwData != 0x45564157 ) // WAVE backwards
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 );
		str.Append( szFile ).Append(" - Not a WAVE file" );
		agk::Error( str );
#endif
		oFile.Close();
		return;
	}

	// find the fmt sub chunk
	uint32_t dwSize = 0;
	oFile.ReadData( (char*)&dwData, 4 );
	oFile.ReadData( (char*)&dwSize, 4 );

	while ( !oFile.IsEOF() && dwData != 0x20746D66 ) // "fmt " backwards
	{
		// not the chunk we were looking for
		uint32_t pos = oFile.GetPos() + dwSize;
		oFile.Seek( pos );
		
		oFile.ReadData( (char*)&dwData, 4 );
		oFile.ReadData( (char*)&dwSize, 4 );	
	} 

	if ( dwData != 0x20746D66 ) // fmt backwards
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 );
		str.Append( szFile ).Append(" - Unrecognised sub chunk data format, expected 'fmt '" );
		agk::Error( str );
#endif
		oFile.Close();
		return;
	}

	if ( dwSize < 16 )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 );
		str.Append( szFile ).Append(" - sub chunk size too small" );
		agk::Error( str );
#endif
		oFile.Close();
		return;
	}

	cSoundMgr::cSoundFile *pSound = new cSoundMgr::cSoundFile();
	m_pSoundFiles[ iID ] = pSound;
	pSound->m_iID = iID;
	pSound->m_iMax = iMaxInstances;
	pSound->m_sFile.SetStr( szFile );

	// wave header
	oFile.ReadData( (char*)&(pSound->m_fmt.wFormatTag), 2 );
	oFile.ReadData( (char*)&(pSound->m_fmt.nChannels), 2 );
	oFile.ReadData( (char*)&(pSound->m_fmt.nSamplesPerSec), 4 );
	oFile.ReadData( (char*)&(pSound->m_fmt.nAvgBytesPerSec), 4 );
	oFile.ReadData( (char*)&(pSound->m_fmt.nBlockAlign), 2 );
	oFile.ReadData( (char*)&(pSound->m_fmt.wBitsPerSample), 2 );
	pSound->m_fmt.cbSize = 0;

	// skip the extra header info
	uint32_t pos = oFile.GetPos() + (dwSize-16);
	oFile.Seek( pos );

	// find data header
	oFile.ReadData( (char*)&dwData, 4 );
	oFile.ReadData( (char*)&dwSize, 4 );
	while ( !oFile.IsEOF() && dwData != 0x61746164 ) // data backwards
	{
		uint32_t pos = oFile.GetPos() + dwSize;
		oFile.Seek( pos );
		
		oFile.ReadData( (char*)&dwData, 4 );
		oFile.ReadData( (char*)&dwSize, 4 );
	}

	if ( dwData != 0x61746164 ) // data backwards
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 );
		str.Append( szFile ).Append(" - Could not find 'data' header" );
		agk::Error( str );
#endif
		oFile.Close();

		delete m_pSoundFiles[ iID ];
		m_pSoundFiles[ iID ] = 0;
		return;
	}

	pSound->m_uDataSize = dwSize;
	pSound->m_pRawData = new unsigned char[ dwSize ];

	// raw sound data
	oFile.ReadData( (char*)pSound->m_pRawData, dwSize );
	oFile.Close();

	PlatformAddFile( pSound );
}


uint32_t cSoundMgr::AddFile( const unsigned char *pData, uint32_t size, int iMaxInstances )
{
	uint32_t iID = m_iLastID + 1;
	if ( iID >= MAX_SOUND_FILES ) iID = 1;
	
	while ( m_pSoundFiles[ iID ] && iID != m_iLastID )
	{
		iID++;
		if ( iID >= MAX_SOUND_FILES ) 
		{
			if ( m_iLastID == 0 ) iID = 0;
			else iID = 1;
		}
	}

	if ( iID == m_iLastID )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file from memblock - No free ID found", 100 ); 
		agk::Error( str );
#endif
		return 0;
	}

	m_iLastID = iID;
	AddFile( iID, pData, iMaxInstances );

	return iID;
}

void cSoundMgr::AddFile( uint32_t iID, const unsigned char *pData, uint32_t size, int iMaxInstances )
{
	if ( iID < 1 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file from memblock", 100 ); 
		str.Append( " - ID must be between 1 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	if ( m_pSoundFiles[ iID ] )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file ", 100 );
		str.Append(" - ID ").AppendUInt( iID ).Append( " already taken" );
		agk::Error( str );
#endif
		return;
	}

	cSoundMgr::cSoundFile *pSound = new cSoundMgr::cSoundFile();
	m_pSoundFiles[ iID ] = pSound;
	pSound->m_iID = iID;
	pSound->m_iMax = iMaxInstances;
	pSound->m_sFile.SetStr( "Memblock" );

	// wave header
	pSound->m_fmt.wFormatTag = 1;
	pSound->m_fmt.nChannels = ((short*)pData)[0];
	pSound->m_fmt.nSamplesPerSec = ((int*)pData)[1];
	pSound->m_fmt.wBitsPerSample = ((short*)pData)[1];
	pSound->m_fmt.nBlockAlign = pSound->m_fmt.nChannels*(pSound->m_fmt.wBitsPerSample/8);
	pSound->m_fmt.nAvgBytesPerSec = pSound->m_fmt.nSamplesPerSec*pSound->m_fmt.nBlockAlign;
	pSound->m_fmt.cbSize = 0;

	pSound->m_uDataSize = ((int*)pData)[2] * pSound->m_fmt.nBlockAlign;
	pSound->m_pRawData = new unsigned char[ pSound->m_uDataSize ];

	if ( pSound->m_uDataSize+12 > size )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add sound file from memblock", 100 );
		str.Append(" - not enough data" );
		agk::Error( str );
#endif

		delete m_pSoundFiles[ iID ];
		m_pSoundFiles[ iID ] = 0;
		return;		
	}

	// raw sound data
	memcpy( pSound->m_pRawData, pData+12, pSound->m_uDataSize );

	PlatformAddFile( pSound );
}

void cSoundMgr::ProcessOGG( cSoundFile* pSound, OggVorbis_File* oggFile )
{
	vorbis_info* pInfo = ov_info( oggFile, -1 );

	// wave header
	pSound->m_fmt.wFormatTag = 1;
	pSound->m_fmt.nChannels = pInfo->channels;
	pSound->m_fmt.nSamplesPerSec = pInfo->rate;
	pSound->m_fmt.wBitsPerSample = 16;
	pSound->m_fmt.nBlockAlign = pSound->m_fmt.nChannels*(pSound->m_fmt.wBitsPerSample/8);
	pSound->m_fmt.nAvgBytesPerSec = pSound->m_fmt.nSamplesPerSec*pSound->m_fmt.nBlockAlign;
	pSound->m_fmt.cbSize = 0;

	// raw sound data
	int samples = (int) ov_pcm_total( oggFile, -1 );
	pSound->m_uDataSize = samples * (pSound->m_fmt.wBitsPerSample/8) * pSound->m_fmt.nChannels;
	pSound->m_pRawData = new unsigned char[ pSound->m_uDataSize ];
	
	int ptr = 0;
	int bytes = 0;
	int remaining = pSound->m_uDataSize;
	do
	{
		int bitstream;
		bytes = ov_read( oggFile, (char*)(pSound->m_pRawData+ptr), remaining, 0, 2, 1, &bitstream );
		ptr += bytes;
		remaining -= bytes;
	} while( remaining > 0 && bytes > 0 );

	ov_clear( oggFile );
}

uint32_t cSoundMgr::AddOGGFile( const uString &szFilename, int iMaxInstances )
{
	uint32_t iID = m_iLastID + 1;
	if ( iID >= MAX_SOUND_FILES ) iID = 1;
	
	while ( m_pSoundFiles[ iID ] && iID != m_iLastID )
	{
		iID++;
		if ( iID >= MAX_SOUND_FILES ) 
		{
			if ( m_iLastID == 0 ) iID = 0;
			else iID = 1;
		}
	}

	if ( iID == m_iLastID )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add OGG sound file ", 100 ); 
		str.Append( szFilename ).Append( " - No free ID found" );
		agk::Error( str );
#endif
		return 0;
	}

	m_iLastID = iID;
	AddOGGFile( iID, szFilename, iMaxInstances );

	return iID;
}

void cSoundMgr::AddOGGFile( uint32_t iID, const uString &szFilename, int iMaxInstances )
{
	uString szFile( szFilename );
	szFile.Replace( '\\', '/' );

	if ( iID < 1 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add OGG sound file ", 100 ); 
		str.Append( szFile ).Append( " - ID must be between 1 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	if ( m_pSoundFiles[ iID ] )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add OGG sound file ", 100 );
		str.Append( szFile ).Append(" - ID ").AppendUInt( iID ).Append( " already taken" );
		agk::Error( str );
#endif
		return;
	}

	ov_callbacks callbacks = {
		AGKSoundFileRead,
		AGKSoundFileSeek,
		AGKSoundFileClose,
		AGKSoundFileTell
	};

	cFile oFile;
	if ( !oFile.OpenToRead( szFile ) )
	{
		uString str;
		str.Format( "Failed to open OGG file: %s", szFile.GetStr() );
		agk::Error( str );
		return;
	}

	OggVorbis_File oggFile;
	if ( ov_open_callbacks( &oFile, &oggFile, NULL, 0, callbacks ) != 0 )
	{
		oFile.Close();
		uString str;
		str.Format( "Failed to read OGG file: %s", szFile.GetStr() );
		agk::Error( str );
		return;
	}

	// cosntruct AGK sound file
	cSoundMgr::cSoundFile *pSound = new cSoundMgr::cSoundFile();
	m_pSoundFiles[ iID ] = pSound;
	pSound->m_iID = iID;
	pSound->m_iMax = iMaxInstances;
	pSound->m_sFile.SetStr( szFile );

	ProcessOGG( pSound, &oggFile );

	PlatformAddFile( pSound );
}

uint32_t cSoundMgr::AddOGGMem( const uint8_t* data, uint32_t size, int iMaxInstances )
{
	uint32_t iID = m_iLastID + 1;
	if ( iID >= MAX_SOUND_FILES ) iID = 1;
	
	while ( m_pSoundFiles[ iID ] && iID != m_iLastID )
	{
		iID++;
		if ( iID >= MAX_SOUND_FILES ) 
		{
			if ( m_iLastID == 0 ) iID = 0;
			else iID = 1;
		}
	}

	if ( iID == m_iLastID )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Could not add OGG sound - No free ID found" );
#endif
		return 0;
	}

	m_iLastID = iID;
	AddOGGMem( iID, data, size, iMaxInstances );

	return iID;
}

void cSoundMgr::AddOGGMem( uint32_t iID, const uint8_t* data, uint32_t size, int iMaxInstances )
{
	if ( iID < 1 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add OGG sound - ID must be between 1 and ", 100 ); 
		str.AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	if ( m_pSoundFiles[ iID ] )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not add OGG sound - ID ", 100 );
		str.AppendUInt( iID ).Append( " already taken" );
		agk::Error( str );
#endif
		return;
	}

	ov_callbacks callbacks = {
		AGKSoundMemRead,
		AGKSoundMemSeek,
		AGKSoundMemClose,
		AGKSoundMemTell
	};

	oggMemData oggData;
	oggData.pData = (uint8_t*) data;
	oggData.pos = 0;
	oggData.size = size;

	OggVorbis_File oggFile;
	if ( ov_open_callbacks( &oggData, &oggFile, NULL, 0, callbacks ) != 0 )
	{
		agk::Error( "Failed to read OGG data" );
		return;
	}

	// cosntruct AGK sound file
	cSoundMgr::cSoundFile *pSound = new cSoundMgr::cSoundFile();
	m_pSoundFiles[ iID ] = pSound;
	pSound->m_iID = iID;
	pSound->m_iMax = iMaxInstances;
	pSound->m_sFile.SetStr( "memory" );
	
	ProcessOGG( pSound, &oggFile );

	PlatformAddFile( pSound );
}

void cSoundMgr::SaveFile( uint32_t iID, const uString &szFilename )
{
	uString szFile( szFilename );
	szFile.Replace( '\\', '/' );

	if ( iID < 1 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not save sound file ", 100 ); 
		str.Append( szFile ).Append( " - ID must be between 1 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	if ( !m_pSoundFiles[ iID ] )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not save sound file ", 100 );
		str.Append( szFile ).Append(" - ID ").AppendUInt( iID ).Append( " does not exist" );
		agk::Error( str );
#endif
		return;
	}

	cFile oFile;
	if ( !oFile.OpenToWrite( szFile ) )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not save sound file ", 100 );
		str.Append( szFile ); str.Append(" - Could not open file for writing" );
		agk::Error( str );
#endif
		return;
	}

	cSoundFile *pSound = m_pSoundFiles[ iID ];

	uint32_t dwData = 0x46464952; // RIFF backwards
	oFile.WriteData( (char*)&dwData, 4 );

	// chunk size, header size + data size
	dwData = 24 + pSound->m_uDataSize + 8;
	oFile.WriteData( (char*)&dwData, 4 );

	dwData = 0x45564157; // WAVE backwards
	oFile.WriteData( (char*)&dwData, 4 );

	// write the header
	dwData = 0x20746D66; // fmt backwards
	oFile.WriteData( (char*)&dwData, 4 );
	dwData = 16;
	oFile.WriteData( (char*)&dwData, 4 );

	oFile.WriteData( (char*)&(pSound->m_fmt.wFormatTag), 2 );
	oFile.WriteData( (char*)&(pSound->m_fmt.nChannels), 2 );
	oFile.WriteData( (char*)&(pSound->m_fmt.nSamplesPerSec), 4 );
	oFile.WriteData( (char*)&(pSound->m_fmt.nAvgBytesPerSec), 4 );
	oFile.WriteData( (char*)&(pSound->m_fmt.nBlockAlign), 2 );
	oFile.WriteData( (char*)&(pSound->m_fmt.wBitsPerSample), 2 );

	// write the sound data
	dwData = 0x61746164; // data backwards
	oFile.WriteData( (char*)&dwData, 4 );
	oFile.WriteData( (char*)&(pSound->m_uDataSize), 4 );
	oFile.WriteData( (char*)pSound->m_pRawData, pSound->m_uDataSize );

	oFile.Close();
}

cSoundMgr::cSoundFile* cSoundMgr::GetFile( uint32_t iID )
{
	if ( iID < 1 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not get sound file ", 100 ); 
		str.Append( " - ID must be between 1 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return 0;
	}

	return m_pSoundFiles[ iID ];
}


uint32_t cSoundMgr::PlayInstance( uint32_t iID, int iVol, int iLoop, int iPriority )
{
	if ( iID < 1 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not play sound file ", 100 ); 
		str.AppendInt( iID ).Append( " - ID must be between 1 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return 0;
	}

	if ( m_pSoundFiles[ iID ] == 0 )
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not play sound file - ID ", 100 );
		str.AppendUInt( iID ).Append( " does not exist" );
		agk::Error( str );
#endif
		return 0;
	}

	return PlatformCreateInstance( m_pSoundFiles[ iID ], iVol, iLoop, iPriority );
}

/*
void cSoundMgr::PauseAll( uint32_t iID )
{
	if ( iID < 0 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not pause sounds ", 100 ); 
		str.AppendInt( iID ).Append( " - ID must be between 0 and " ).Append( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	PlatformPauseInstances( iID );
}

void cSoundMgr::ResumeAll( uint32_t iID )
{
	if ( iID < 0 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not resume sounds ", 100 ); 
		str.AppendInt( iID ).Append( " - ID must be between 0 and " ).Append( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	PlatformResumeInstances( iID );
}
 */

void cSoundMgr::StopAll( uint32_t iID )
{
	if ( iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not stop sound file ", 100 ); 
		str.AppendInt( iID ).Append( " - ID must be between 0 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	PlatformStopInstances( iID );
}

void cSoundMgr::DeleteFile( uint32_t iID )
{
	if ( iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not delete sound file ", 100 ); 
		str.AppendInt( iID ).Append( " - ID must be between 1 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return;
	}

	PlatformStopInstances( iID );
	if ( iID == 0 )
	{
		for ( int i = 0; i < MAX_SOUND_FILES; i++ ) 
		{
			if ( m_pSoundFiles[ i ] ) delete m_pSoundFiles[ i ];
			m_pSoundFiles[ i ] = 0;
		}
	}
	else
	{
		if ( m_pSoundFiles[ iID ] ) delete m_pSoundFiles[ iID ];
		m_pSoundFiles[ iID ] = 0;
	}
}


uint32_t cSoundMgr::CountInstances( uint32_t iID )
{
	if ( iID < 1 || iID >= MAX_SOUND_FILES ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString str( "Could not count sound instances ", 100 ); 
		str.AppendInt( iID ).Append( " - ID must be between 1 and " ).AppendInt( MAX_SOUND_FILES-1 );
		agk::Error( str );
#endif
		return 0;
	}

	if ( m_pSoundFiles[ iID ] ) return m_pSoundFiles[ iID ]->m_iInstances;
	else return 0;
}


uint32_t cSoundMgr::Exists( uint32_t iID )
{
	if ( iID < 1 || iID >= MAX_SOUND_FILES ) return 0;
	if ( !m_pSoundFiles[ iID ] ) return 0;
	return 1;
}
