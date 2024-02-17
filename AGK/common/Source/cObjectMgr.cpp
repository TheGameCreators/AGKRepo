#include "agk.h"
//#include "Wrapper.h"

using namespace AGK;

cObjectMgr::cObjectMgr()
{
	m_pOpaqueObjects = 0;
	m_pLastOpaque = 0;
	m_pAlphaObjects = 0;
	m_iNumAlphaObjects = 0;
	m_pAlphaObjectsArray = 0;

	iCurrentCount = 0;
	m_iLastSorted = 0; 
	m_iLastDrawn = 0; 
	m_iLastTotal = 0;
	m_iLastDrawCalls = 0;

	m_bSortDepth = true;

	m_pSkyBox = 0;
}

cObjectMgr::~cObjectMgr()
{
	ClearAll();
}

void cObjectMgr::ClearAll()
{
	cObjectContainer *pCont;
	
	while ( m_pOpaqueObjects )
	{
		pCont = m_pOpaqueObjects;
		m_pOpaqueObjects = m_pOpaqueObjects->m_pNext;
		delete pCont;
	}
	m_pOpaqueObjects = NULL;
	m_pLastOpaque = NULL;

	while ( m_pAlphaObjects )
	{
		pCont = m_pAlphaObjects;
		m_pAlphaObjects = m_pAlphaObjects->m_pNext;
		delete pCont;
	}
	m_pAlphaObjects = NULL;

	iCurrentCount = 0;
	m_bSortDepth = true;
}

void cObjectMgr::SetSortDepth( bool sort )
{
	m_bSortDepth = sort;
}

void cObjectMgr::AddObject( cObject3D* object )
{
	if ( !object ) return;

	cObjectContainer *pNewMember = new cObjectContainer();
	pNewMember->SetObject( object );
	object->m_iObjFlags |= AGK_OBJECT_MANAGED;

	if ( !AddContainer( pNewMember ) ) delete pNewMember;
}

bool cObjectMgr::AddContainer( cObjectContainer* pNewMember )
{
	if ( !pNewMember ) return false;
	if ( pNewMember->GetType() == 0 ) return false;

	pNewMember->m_pNext = 0;
	
	int iMode = pNewMember->GetDrawMode();
	if ( iMode == 0 )
	{
		// opaque, add to the end of the list to draw in order they were added
		pNewMember->m_pNext = 0;;
		if ( m_pLastOpaque ) m_pLastOpaque->m_pNext = pNewMember;
		else m_pOpaqueObjects = pNewMember;
		m_pLastOpaque = pNewMember;
	}
	else
	{
		// alpha transparency, should be drawn by depth back to front
		pNewMember->m_pNext = m_pAlphaObjects;
		m_pAlphaObjects = pNewMember;
	}

	return true;
}

void cObjectMgr::RemoveObject( cObject3D* object )
{
	if ( !object ) return;
	object->m_iObjFlags &= ~AGK_OBJECT_MANAGED;

	// need to check all arrays
	// opaque objects
	cObjectContainer *pMember = m_pOpaqueObjects;
	cObjectContainer *pLast = 0;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 && pMember->GetObject() == object )
		{
			//found, remove it
			cObjectContainer *pNext = pMember->m_pNext;

			if ( m_pLastOpaque == pMember ) m_pLastOpaque = pLast;
			
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pOpaqueObjects = pNext;

			delete pMember;
			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}

	if ( m_pAlphaObjectsArray )
	{
		for ( int i = 0; i < m_iNumAlphaObjects; i++ )
		{
			cObjectContainer *pContainer = (cObjectContainer*) m_pAlphaObjectsArray[ i ].ptr;
			if ( pContainer && pContainer->GetType() == 1 && pContainer->GetObject() == object ) 
			{
				m_pAlphaObjectsArray[ i ].iValue = 0xFFFFFFFF;
				m_pAlphaObjectsArray[ i ].ptr = 0;
				break;
			}
		}
	}

	// alpha objects
	pLast = 0;
	pMember = m_pAlphaObjects;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 && pMember->GetObject() == object )
		{
			//found, remove it
			cObjectContainer *pNext = pMember->m_pNext;
			
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pAlphaObjects = pNext;

			delete pMember;
			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}
}

void cObjectMgr::UpdateAll( float time )
{
	cObjectContainer *pMember = m_pOpaqueObjects;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 ) pMember->GetObject()->Update( time );
		pMember = pMember->m_pNext;
	}

	pMember = m_pAlphaObjects;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 ) pMember->GetObject()->Update( time );
		pMember = pMember->m_pNext;
	}
}

void cObjectMgr::ResortAll()
{
	m_iLastTotal = 0;

	// check for any changed objects that need reordering
	// check back opaque objects
	cObjectContainer *pMember = m_pOpaqueObjects;
	cObjectContainer *pLast = 0;
	cObjectContainer *pChanged = 0;
	cObjectContainer *pLastChanged = 0;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 ) m_iLastTotal++;
		
		if ( pMember->GetTransparencyChanged() )
		{
			// object has changed, remove it from this list
			cObjectContainer *pNext = pMember->m_pNext;
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pOpaqueObjects = pNext;

			if ( m_pLastOpaque == pMember ) m_pLastOpaque = pLast;

			// add it to the changed list, deal with it later
			pMember->m_pNext = 0;
			if ( pLastChanged ) pLastChanged->m_pNext = pMember;
			else pChanged = pMember;
			pLastChanged = pMember;

			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}

	// check alpha objects
	pMember = m_pAlphaObjects;
	pLast = 0;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 ) m_iLastTotal++;
		
		if ( pMember->GetTransparencyChanged() )
		{
			// object has changed, remove it from this list
			cObjectContainer *pNext = pMember->m_pNext;
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pAlphaObjects = pNext;

			// add it to the changed list, deal with it later
			pMember->m_pNext = 0;
			if ( pLastChanged ) pLastChanged->m_pNext = pMember;
			else pChanged = pMember;
			pLastChanged = pMember;

			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}

	m_iLastSorted = 0;
	
	// re-add changed objects
	pMember = pChanged;
	cObjectContainer *pNext = 0;
	while ( pMember )
	{
		pNext = pMember->m_pNext;
		if ( !AddContainer( pMember ) ) delete pMember;
		else m_iLastSorted++;
		pMember = pNext;
	}

	// count transparent objects
	int alphaCount = 0;
	pMember = m_pAlphaObjects;
	while ( pMember )
	{
		alphaCount++;
		pMember = pMember->m_pNext;
	}

	// resize transparent array if needed
	if ( alphaCount > m_iNumAlphaObjects )
	{
		if ( m_pAlphaObjectsArray ) delete [] m_pAlphaObjectsArray;
		m_pAlphaObjectsArray = 0;
		if ( alphaCount > 0 ) m_pAlphaObjectsArray = new AGKSortValue[ alphaCount ];
	}

	m_iNumAlphaObjects = alphaCount;

	// rebuild transparent array
	cCamera *pCurrCamera = agk::GetCurrentCamera();
	alphaCount = 0;
	pMember = m_pAlphaObjects;
	cObject3D* tmpobj;
	while ( pMember )
	{
		m_pAlphaObjectsArray[ alphaCount ].ptr = pMember;
		m_pAlphaObjectsArray[ alphaCount ].iValue = 0;

		if ( pCurrCamera ) {
			tmpobj = ((cObject3D*) pMember->GetObject()); 
			float dist = -tmpobj->posFinal().GetSqrDist( pCurrCamera->posFinal() ); // negative so it sorts in reverse order
			m_pAlphaObjectsArray[ alphaCount ].iValue = agk::SortFloatToUINT( dist );
		}

		alphaCount++;
		
		pMember = pMember->m_pNext;
	}

	// sort transparent objects
	if ( pCurrCamera ) agk::SortArray( m_pAlphaObjectsArray, m_iNumAlphaObjects );
}

void cObjectMgr::DrawAll()
{
	ResortAll();

	m_iLastDrawn = 0;
	m_iLastDrawCalls = 0;
	
	if ( m_pOpaqueObjects ) DrawList( m_pOpaqueObjects, 0 );
	// skybox drawn last using the depth buffer to cull as much as possible, but before transparent objects
	if ( m_pSkyBox ) m_pSkyBox->Draw();

	//if ( m_pAlphaObjects ) DrawList( m_pAlphaObjects, 1 );
	if ( m_pAlphaObjectsArray )
	{
		for ( int i = 0 ; i < m_iNumAlphaObjects; i++ )
		{
			cObjectContainer *pContainer = (cObjectContainer*) m_pAlphaObjectsArray[ i ].ptr;
			if ( pContainer && pContainer->GetType() == 1 )
			{
				m_iLastDrawn++;
				pContainer->GetObject()->Draw();
			}
		}
	}
}

void cObjectMgr::DrawList( cObjectContainer *pList, int mode )
{
	if ( !pList ) return;

	cObjectContainer *pObject = pList;
	while ( pObject )
	{
		if ( pObject->GetType() == 1 )
		{
			m_iLastDrawn++;
			pObject->GetObject()->Draw();
		}
		pObject = pObject->m_pNext;
	}
}

void cObjectMgr::DrawShadowMap()
{
	if ( m_pOpaqueObjects ) DrawShadowList( m_pOpaqueObjects, 0 );
	if ( m_pAlphaObjects ) DrawShadowList( m_pAlphaObjects, 1 );
}

void cObjectMgr::DrawShadowList( cObjectContainer *pList, int mode )
{
	if ( !pList ) return;

	cObjectContainer *pObject = pList;
	while ( pObject )
	{
		if ( pObject->GetType() == 1 )
		{
			pObject->GetObject()->DrawShadow();
		}
		pObject = pObject->m_pNext;
	}
}
