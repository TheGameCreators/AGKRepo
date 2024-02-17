#ifndef _H_AGK_THREADING_ANDROID
#define _H_AGK_THREADING_ANDROID

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

#if __ARM_ARCH__ == 7
	// spin locks are faster, but not recursive and must not be used on single core devices
	class cSpinLock 
	{
		protected:
			volatile int iLock;
			
		public:
			cSpinLock() { iLock = 0; }

			~cSpinLock() { }

			bool Acquire()
			{
				asm volatile ( "mov r1, #0x1 \n\t"
					 "1: ldrex r0, [%0] \n\t"
						"cmp r0, #0 \n\t"
						"strexeq r0, r1, [%0] \n\t"
						"cmpeq r0, #0 \n\t"
						"bne 1b \n\t"
						"dmb \n\t"
						: 
						: "r"(&iLock)
						: "r0", "r1", "cc", "memory" );
						
				return true;
			}

			void Release()
			{
				asm volatile ( "dmb" );
				iLock = 0;
			}
	};
#elif __ARM_ARCH__ == 8
	// spin locks are faster, but not recursive and must not be used on single core devices
	class cSpinLock 
	{
		protected:
			volatile int iLock;
			
		public:
			cSpinLock() { iLock = 0; }

			~cSpinLock() { }

			bool Acquire()
			{
				int count = 1000;
				while( 1 )
				{
					asm volatile ( 
							"mov w1, #0x1 \n"
						 "1: subs %w0, %w0, #0x1 \n"
							"cbz %w0, 2f \n"
							"ldaxr w0, [%1] \n"
							"cbnz w0, 1b \n"
							"stlxr w0, w1, [%1] \n"
							"cbnz w0, 1b \n"
							"dmb SY \n"
						 "2: nop \n"
							: "+r"(count)
							: "r"(&iLock) 
							: "w0", "w1", "cc", "memory" );

					if ( count == 0 ) usleep(10);
					else return true;

					count = 4;
				}
			}

			void Release()
			{
				asm volatile ( "dmb SY" );
				iLock = 0;
			}
	};
#else
	class cSpinLock 
	{
		protected:
			pthread_mutex_t mutex;
			
		public:
			cSpinLock() 
			{ 
				pthread_mutexattr_t attr;
				pthread_mutexattr_init( &attr );
				pthread_mutex_init( &mutex, &attr );
			}

			~cSpinLock()
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
#endif

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
				//pthread_cond_timeout_np( &condition, &mutex, 50 );
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
