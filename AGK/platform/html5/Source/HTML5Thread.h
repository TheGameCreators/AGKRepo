#ifndef _H_AGK_THREADING_LINUX
#define _H_AGK_THREADING_LINUX

#include "Common.h"

namespace AGK
{
	class cLock 
	{
		protected:
			
		public:
			cLock()
			{
				
			}

			~cLock()
			{
				
			}

			bool Acquire()
			{
				return true;
			}

			void Release()
			{
				
			}
	};
	
	class cSpinLock
	{
		protected:

		public:
			cSpinLock()
			{
				
			}

			~cSpinLock()
			{
				
			}

			bool Acquire()
			{
				return true;
			}

			void Release()
			{
				
			}
	};

	class cCondition
	{
		protected:
			bool m_bLocked;

		public:
			cCondition()
			{
				m_bLocked = false;
			}

			~cCondition()
			{
				
			}

			void Lock()
			{
				m_bLocked = true;
			}

			void Unlock()
			{
				m_bLocked = false;
			}

			void Wait()
			{
				
			}

			void Signal()
			{
				
			}

			void Broadcast()
			{
				
			}
	};
}

#endif
