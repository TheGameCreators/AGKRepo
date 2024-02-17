#include "agk.h"

using namespace AGK;

AGKFrameBuffer* AGKFrameBuffer::g_pAllFrameBuffers = 0;
AGKFrameBuffer* AGKFrameBuffer::g_pCurrFrameBuffer = 0;

void AGKFrameBuffer::DeleteImage( cImage *pImage )
{
	if ( !pImage ) return;

	AGKFrameBuffer *pFBO = g_pAllFrameBuffers;
	AGKFrameBuffer *pLast = 0;
	while ( pFBO )
	{
		AGKFrameBuffer *pNext = pFBO->m_pNextFBO;

		if ( pFBO->m_pColor == pImage || pFBO->m_pDepth == pImage )
		{
			if ( pLast ) pLast->m_pNextFBO = pFBO->m_pNextFBO;
			else g_pAllFrameBuffers = pFBO->m_pNextFBO;

			delete pFBO;
		}
		else pLast = pFBO;

		pFBO = pNext;
	}
}

AGKFrameBuffer* AGKFrameBuffer::FindFrameBuffer( cImage *pColor, cImage* pDepth, bool forceDepth )
{
	AGKFrameBuffer *pFBO = g_pAllFrameBuffers;
	while ( pFBO )
	{
		if ( pFBO->m_pColor == pColor && pFBO->m_pDepth == pDepth ) 
		{
			if ( ((pFBO->m_bFlags & AGK_FBO_FORCE_DEPTH) != 0) == forceDepth ) return pFBO;
		}

		pFBO = pFBO->m_pNextFBO;
	}

	return 0;
}

void AGKFrameBuffer::ClearAll()
{
	BindFrameBuffer( 0 );

	while ( g_pAllFrameBuffers )
	{
		AGKFrameBuffer *pNext = g_pAllFrameBuffers->m_pNextFBO;

		delete g_pAllFrameBuffers;

		g_pAllFrameBuffers = pNext;
	}
}

void AGKFrameBuffer::ReloadAll()
{
	BindFrameBuffer( 0 );

	AGKFrameBuffer *pFBO = g_pAllFrameBuffers;
	while ( pFBO )
	{
		agk::GetRenderer()->DeleteFrameBuffer( &pFBO->m_pInternalResource );
		pFBO->m_pInternalResource = 0;
		pFBO = pFBO->m_pNextFBO;
	}

	pFBO = g_pAllFrameBuffers;
	while ( pFBO )
	{
		void *pInternalColor = 0;
		if ( pFBO->m_pColor ) pInternalColor = pFBO->m_pColor->m_pInternalResource;
	
		void *pInternalDepth = 0;
		if ( pFBO->m_pDepth ) pInternalDepth = pFBO->m_pDepth->m_pInternalResource;

		agk::GetRenderer()->CreateFrameBuffer( pInternalColor, pInternalDepth, pFBO->ShouldForceDepth() ? 1 : 0, &pFBO->m_pInternalResource );
		pFBO = pFBO->m_pNextFBO;
	}
}

void AGKFrameBuffer::BindFrameBuffer( AGKFrameBuffer *pFBO )
{
	if ( agk::GetRenderer()->IsFBOFlipped() )
	{
		if ( pFBO && !g_pCurrFrameBuffer )
		{
			agk::FlipOrthoMatrix();
			AGKShader::g_varInvert.SetFloat( agk::GetRenderer()->IsTopLeftOrigin() ? 1.0f : -1.0f );
			cCamera::UpdateAllInvert( 1 );
		}

		if ( !pFBO && g_pCurrFrameBuffer )
		{
			agk::FlipOrthoMatrix();
			AGKShader::g_varInvert.SetFloat( agk::GetRenderer()->IsTopLeftOrigin() ? -1.0f : 1.0f );
			cCamera::UpdateAllInvert( 0 );
		}
	}

	g_pCurrFrameBuffer = pFBO;
	agk::GetRenderer()->BindFrameBuffer( 0, pFBO ? pFBO->m_pInternalResource : 0 ); 
}

AGKFrameBuffer::AGKFrameBuffer( cImage *pColor, bool bCShared, cImage* pDepth, bool bDShared, bool forceDepth )
{
	if ( pColor && pDepth )
	{
		if ( pColor->GetTotalWidth() != pDepth->GetTotalWidth() || pColor->GetTotalHeight() != pDepth->GetTotalHeight() )
		{
			agk::Error( "Failed to create frame buffer, color and depth images must be the same size" );
			return;
		}
	}

	if ( !pColor && !pDepth )
	{
		agk::Error( "Failed to create frame buffer, either color or depth image must be specified" );
		return;
	}

	m_pColor = pColor;
	m_pDepth = pDepth;

	if ( pColor ) 
	{
		m_iWidth = pColor->GetTotalWidth();
		m_iHeight = pColor->GetTotalHeight();
	}
	else
	{
		m_iWidth = pDepth->GetTotalWidth();
		m_iHeight = pDepth->GetTotalHeight();
	}

	m_bFlags = 0;
	if ( m_pColor && bCShared ) m_bFlags |= AGK_FBO_SHARED_COLOR;
	if ( m_pDepth && bDShared ) m_bFlags |= AGK_FBO_SHARED_DEPTH;
	if ( forceDepth ) m_bFlags |= AGK_FBO_FORCE_DEPTH;

	void *pInternalColor = 0;
	if ( m_pColor ) pInternalColor = m_pColor->m_pInternalResource;
	
	void *pInternalDepth = 0;
	if ( m_pDepth ) pInternalDepth = m_pDepth->m_pInternalResource;

	agk::GetRenderer()->CreateFrameBuffer( pInternalColor, pInternalDepth, forceDepth ? 1 : 0, &m_pInternalResource );
	
	m_pNextFBO = g_pAllFrameBuffers;
	g_pAllFrameBuffers = this;
}

AGKFrameBuffer::~AGKFrameBuffer()
{
	if ( g_pCurrFrameBuffer == this ) BindFrameBuffer( 0 );

	AGKFrameBuffer *pFBO = g_pAllFrameBuffers;
	AGKFrameBuffer *pLast = 0;
	while ( pFBO )
	{
		if ( pFBO == this )
		{
			if ( pLast ) pLast->m_pNextFBO = pFBO->m_pNextFBO;
			else g_pAllFrameBuffers = pFBO->m_pNextFBO;

			break;
		}
		else pLast = pFBO;

		pFBO = pFBO->m_pNextFBO;
	}

	agk::GetRenderer()->DeleteFrameBuffer( m_pInternalResource );

	// only delete the images if they are not shared with anything
	if ( m_pColor && !(m_bFlags & AGK_FBO_SHARED_COLOR) ) delete m_pColor;
	if ( m_pDepth && !(m_bFlags & AGK_FBO_SHARED_DEPTH) ) delete m_pDepth;
}

void AGKFrameBuffer::SetShadow() 
{ 
	agk::GetRenderer()->SetFrameBufferIsShadow( m_pInternalResource ); 
}