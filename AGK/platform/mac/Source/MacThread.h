#ifndef _H_AGK_THREADING_MAC
#define _H_AGK_THREADING_MAC

#include "Common.h"

namespace AGK
{
	class cLock 
	{
		protected:
		NSRecursiveLock *MacLock;
			
		public:
			cLock()
			{
				MacLock = [[NSRecursiveLock alloc] init];
			}

			~cLock()
			{
				[ MacLock release ];
			}

			bool Acquire()
			{
				[ MacLock lock ];
				return true;
			}

			void Release()
			{
				[ MacLock unlock ];
			}
	};

	class cSpinLock 
	{
		protected:
		NSLock *MacLock;
			
		public:
			cSpinLock()
			{
				MacLock = [[NSLock alloc] init];
			}

			~cSpinLock()
			{
				[ MacLock release ];
			}

			bool Acquire()
			{
				[ MacLock lock ];
				return true;
			}

			void Release()
			{
				[ MacLock unlock ];
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
