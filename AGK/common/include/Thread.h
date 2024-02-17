#ifndef AGKTHREAD_H
#define AGKTHREAD_H

#include "Common.h"

namespace AGK
{
	class AGKThread
	{
		private:

			void* pThread;
			uint32_t iThreadID;
			volatile bool m_bRunning;

		public:
			inline static uint32_t STDCALL EntryPoint( void *pParams )
			{
				if ( !pParams ) return 0;
				AGKThread* pThis = (AGKThread*) pParams;
				uint32_t result = 0;
				if ( !pThis->m_bTerminate ) 
				{
					result = pThis->Run();
				}
				pThis->Finally();
				return result;
			}

			// when a thread stops for whatever reason this function should be called
			void Finally()
			{
				m_bRunning = false;
			}

		protected:

			// variables to tell the thread to stop, it must watch these and stop appropriately
			volatile bool m_bTerminate;
			void* m_pStop; 

			// this must be overridden by the inheriting class, contains all code that the thread will run.
			virtual uint32_t Run( ) = 0;

		private:
			void PlatformInit();
			void PlatformStart();
			void PlatformStop();
			void PlatformSleepSafe( uint32_t milliseconds );
			void PlatformJoin();
			void PlatformTerminate();
			void PlatformCleanUp();
			
		public:

			AGKThread( )
			{
				pThread = 0;
				iThreadID = 0;
				m_bRunning = false;
				m_bTerminate = false;
				m_pStop = 0;
				PlatformInit(); 
			}

			virtual ~AGKThread( )
			{
				// tell it to stop
				Stop();

				// give some time?
				Join();
				//Sleep( 100 );

				// force it to stop
				PlatformTerminate();
				PlatformCleanUp();
			}

			bool IsRunning() { return m_bRunning; }

			// starts the thread and calls Run() (if not already running)
			virtual void Start( )
			{
				if ( m_bRunning ) return;
				m_bTerminate = false;
				m_bRunning = true;
				PlatformStart();
			}

			// tells the thread to stop and returns immediately, the thread is not guaranteed to stop
			virtual void Stop()
			{
				m_bTerminate = true;
				PlatformStop();
			}

			// sleep for a specified time but will wake early if the thread is told to stop, check m_bTerminate after this command
			void SleepSafe( uint32_t milliseconds )
			{
				PlatformSleepSafe( milliseconds );
			}

			// waits for the thread to stop then returns, can wait forever. If thread is already stopped or 
			// not yet started returns immediately
			void Join( )
			{
				if ( !IsRunning() ) return;
				PlatformJoin();
			}

			/*
			// forces the thread to stop immediately, if it held any locks they will not be released and
			// may cause a crash
			void Terminate()
			{
				PlatformTerminate();
				Finally();
			}
			*/
	};
}

#ifdef AGK_WINDOWS
	#include "../../platform/windows/Source/WindowsThread.h"
#endif

#ifdef AGK_IOS
	#include "../../platform/apple/Source/AppleThread.h"
#endif

#ifdef AGK_MACOS
	#include "../../platform/mac/Source/MacThread.h"
#endif

#ifdef AGK_ANDROID
	#include "../../platform/android/Source/AndroidThread.h"
#endif

#ifdef AGK_LINUX
	#include "../../platform/linux/Source/LinuxThread.h"
#endif

#ifdef AGK_RASPPI
	#include "../../platform/pi/Source/PiThread.h"
#endif
	
#ifdef AGK_HTML5
	#include "../../platform/html5/Source/HTML5Thread.h"
#endif

#endif
