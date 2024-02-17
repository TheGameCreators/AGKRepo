#ifndef _H_AGK_THREADING_WIN
#define _H_AGK_THREADING_WIN

#include "Common.h"

namespace AGK
{
	class cLock 
	{
		protected:
			CRITICAL_SECTION m_cs;

		public:
			cLock()	{ InitializeCriticalSectionAndSpinCount( &m_cs, 4000 );	}
			~cLock() { DeleteCriticalSection( &m_cs ); }

			bool Acquire()
			{
				EnterCriticalSection( &m_cs );
				return true;
			}

			void Release() { LeaveCriticalSection( &m_cs );	}
	};

	class cSpinLock 
	{
		protected:
			CRITICAL_SECTION m_cs;

		public:
			cSpinLock()	{ InitializeCriticalSectionAndSpinCount( &m_cs, 4000 );	}

			~cSpinLock() { DeleteCriticalSection( &m_cs ); }

			bool Acquire()
			{
				EnterCriticalSection( &m_cs );
				return true;
			}

			void Release() { LeaveCriticalSection( &m_cs );	}
	};

	class cCondition 
	{
		protected:
			HANDLE condition[2];
			HANDLE hold;
			HANDLE release;
			CRITICAL_SECTION m_cs;
			volatile int waiting;
			
		public:
			cCondition()
			{
				condition[0] = CreateEvent( NULL, FALSE, FALSE, NULL );
				condition[1] = CreateEvent( NULL, TRUE, FALSE, NULL );
				hold = CreateEvent( NULL, TRUE, TRUE, NULL );
				release = CreateEvent( NULL, TRUE, TRUE, NULL );
				InitializeCriticalSection( &m_cs );
				waiting = 0;
			}

			~cCondition()
			{
				DeleteCriticalSection( &m_cs );
				CloseHandle( release );
				CloseHandle( hold );
				CloseHandle( condition[1] );
				CloseHandle( condition[0] );
			}

			void Lock() { EnterCriticalSection( &m_cs ); }
			void Unlock() { LeaveCriticalSection( &m_cs ); }

			// this would be so much simpler with condition variables
			void Wait()
			{
				waiting++;
				LeaveCriticalSection( &m_cs );
				
				// wait for single or broadcast conditions
				WaitForMultipleObjects( 2, condition, FALSE, INFINITE );

				EnterCriticalSection( &m_cs );
				waiting--;
				if ( waiting == 0 ) SetEvent( release ); // notify the broadcast function that all threads are awake, doesn't matter if it is waiting or not
				LeaveCriticalSection( &m_cs );

				// if this thread was woken by a broadcast then they will all be held here until they all wake up.
				WaitForSingleObject( hold, INFINITE );

				EnterCriticalSection( &m_cs );
			}

			void Signal()
			{
				// this will release a single thread
				SetEvent( condition[0] );
			}

			void Broadcast()
			{
				if ( waiting == 0 ) return;

				// this will release all threads and wait for them all to be awake before returning
				ResetEvent( release );
				ResetEvent( hold );
				SetEvent( condition[1] ); // this will release all threads
				LeaveCriticalSection( &m_cs );

				WaitForSingleObject( release, INFINITE ); // wait for them all to wake up
				
				EnterCriticalSection( &m_cs );
				ResetEvent( condition[1] );
				SetEvent( hold ); // release the threads to continue
			}
	};

	/*
	// not supported in Windows XP
	class cCondition 
	{
		protected:
			CONDITION_VARIABLE condition;
			CRITICAL_SECTION m_cs;
			
		public:
			cCondition()
			{
				InitializeConditionVariable( &condition );
				InitializeCriticalSection( &m_cs );
			}

			~cCondition() {	DeleteCriticalSection( &m_cs ); }

			void Lock()	{ EnterCriticalSection( &m_cs ); }
			void Unlock() { LeaveCriticalSection( &m_cs ); }

			void Wait() { SleepConditionVariableCS( &condition, &m_cs, INFINITE ); }
			void Signal() { WakeConditionVariable( &condition ); }
			void Broadcast() { WakeAllConditionVariable( &condition ); }
	};
	*/

}

#endif