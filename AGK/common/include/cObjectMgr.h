#ifndef _H_OBJECTMGR_
#define _H_OBJECTMGR_

// Common includes
#include "Common.h"
#include "cObject3D.h"

// Namespace
namespace AGK
{
	class AGKSkyBox;

	class _EXPORT_ cObjectContainer
	{
		protected:
			uint32_t m_iType;
			void *m_pItem;
					
		public:
			cObjectContainer *m_pNext;

			cObjectContainer() { m_pNext = 0; m_iType = 0; m_pItem = 0; }
			~cObjectContainer() {}
		
			void SetObject( cObject3D *pObject )
			{
				m_iType = 1;
				m_pItem = pObject;
			}

			int GetType() { return m_iType; }

			void GetPosition( float &x, float &y, float &z )
			{
				switch( m_iType )
				{
					case 1: 
					{
						x = ((cObject3D*)m_pItem)->GetX(); 
						y = ((cObject3D*)m_pItem)->GetY(); 
						z = ((cObject3D*)m_pItem)->GetZ(); 
						break;
					}
					default: return;
				}
			}

			int GetDrawMode()
			{
				switch( m_iType )
				{
					case 1: return ((cObject3D*)m_pItem)->GetTransparency(); break;
					default: return 0;
				}
			}
			
			bool GetTransparencyChanged()
			{
				switch( m_iType )
				{
					case 1: return ((cObject3D*)m_pItem)->GetTransparencyChanged(); break;
					default: return false;
				}
			}

			cObject3D* GetObject() { return (cObject3D*)m_pItem; }
	};

	class _EXPORT_ cObjectMgr
	{
		protected:

			int iCurrentCount;
			bool m_bSortDepth;
			
			cObjectContainer *m_pOpaqueObjects;
			cObjectContainer *m_pLastOpaque;
			cObjectContainer *m_pAlphaObjects;
			AGKSortValue *m_pAlphaObjectsArray;
			int m_iNumAlphaObjects;

			uint32_t m_iLastTotal;
			uint32_t m_iLastSorted;
			uint32_t m_iLastDrawn;
			uint32_t m_iLastDrawCalls;
			
			AGKSkyBox *m_pSkyBox;

			bool AddContainer( cObjectContainer *pNewMember );
			void DrawList( cObjectContainer *pList, int mode );
			void DrawShadowList( cObjectContainer *pList, int mode );

		public:

			cObjectMgr();
			~cObjectMgr();
			
			void ClearAll();

			void SetSkyBox( AGKSkyBox *pSkyBox ) { m_pSkyBox = pSkyBox; }
		
			uint32_t GetTotalCount() { return m_iLastTotal; }
			uint32_t GetSortedCount() { return m_iLastSorted; }
			uint32_t GetDrawnCount() { return m_iLastDrawn; }
			uint32_t GetDrawCalls() { return m_iLastDrawCalls; }
		
			void SetSortDepth( bool sort );
			
			void AddObject( cObject3D* object );
			void RemoveObject( cObject3D* object );
			
			void ResortAll();
			virtual void UpdateAll( float time );
			virtual void DrawAll();

			virtual void DrawShadowMap();
	};
}

#endif
