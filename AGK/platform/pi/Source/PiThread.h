#ifndef _H_AGK_THREADING_PI
#define _H_AGK_THREADING_PI

#include "Common.h"

namespace AGK
{
	class cLock 
	{
		protected:
		pthread_mutex_t mutex;
			
		public:
			cLock()
			{
				pthread_mutexattr_t attr;
				pthread_mutexattr_init( &attr );
				pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
				pthread_mutex_init( &mutex, &attr );
			}

			~cLock()
			{
				pthread_mutex_destroy( &mutex );
			}

			bool Acquire()
			{
				pthread_mutex_lock( &mutex );
				return true;
			}

			void Release()
			{
				pthread_mutex_unlock( &mutex );
			}
	};
	
	class cSpinLock
	{
		protected:
			pthread_spinlock_t spinlock;

		public:
			cSpinLock()
			{
				pthread_spin_init( &spinlock, PTHREAD_PROCESS_PRIVATE );
			}

			~cSpinLock()
			{
				pthread_spin_destroy( &spinlock );
			}

			bool Acquire()
			{
				pthread_spin_lock( &spinlock );
				return true;
			}

			void Release()
			{
				pthread_spin_unlock( &spinlock );
			}
	};

	class cCondition
	{
		protected:
			pthread_cond_t condition;
			pthread_mutex_t mutex;
			bool m_bLocked;

		public:
			cCondition()
			{
				pthread_cond_init( &condition, NULL );
				pthread_mutexattr_t attr;
				pthread_mutexattr_init( &attr );
				pthread_mutex_init( &mutex, &attr );
				m_bLocked = false;
			}

			~cCondition()
			{
				pthread_mutex_destroy( &mutex );
				pthread_cond_destroy( &condition );
			}

			void Lock()
			{
				pthread_mutex_lock( &mutex );
				m_bLocked = true;
			}

			void Unlock()
			{
				m_bLocked = false;
				pthread_mutex_unlock( &mutex );
			}

			void Wait()
			{
				pthread_cond_wait( &condition, &mutex );
			}

			void Signal()
			{
				pthread_cond_signal( &condition );
			}

			void Broadcast()
			{
				pthread_cond_broadcast( &condition );
			}
	};
}

#endif
