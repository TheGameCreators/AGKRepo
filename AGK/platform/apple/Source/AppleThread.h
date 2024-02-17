#ifndef _H_AGK_THREADING_IOS
#define _H_AGK_THREADING_IOS

#include "Common.h"

namespace AGK
{
	class cLock 
	{
		protected:
		NSRecursiveLock *iOSLock;
			
		public:
			cLock()
			{
				iOSLock = [[NSRecursiveLock alloc] init];
			}

			~cLock()
			{
				[ iOSLock release ];
			}

			bool Acquire()
			{
				[ iOSLock lock ];
				return true;
			}

			void Release()
			{
				[ iOSLock unlock ];
			}
	};

	class cSpinLock 
	{
		protected:
		NSLock *iOSLock;
			
		public:
			cSpinLock()
			{
				iOSLock = [[NSLock alloc] init];
			}

			~cSpinLock()
			{
				[ iOSLock release ];
			}

			bool Acquire()
			{
				[ iOSLock lock ];
				return true;
			}

			void Release()
			{
				[ iOSLock unlock ];
			}
	};

	class cCondition 
	{
		protected:
            NSCondition *m_condition;
			bool m_bLocked;
			
		public:
			cCondition()
			{
                m_condition = [[NSCondition alloc] init];
                m_bLocked = false;
			}

			~cCondition()
			{
                [m_condition release];
			}

			void Lock()
			{
                [m_condition lock];
				m_bLocked = true;
			}

			void Unlock()
			{
				m_bLocked = false;
                [m_condition unlock];
            }

			void Wait()
			{
                [m_condition wait];
			}

			void Signal()
			{
				[m_condition signal];
            }

			void Broadcast()
			{
				[m_condition broadcast];
            }
	};
}

#endif
