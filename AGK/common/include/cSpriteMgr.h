#ifndef _H_SPRITEMGR_
#define _H_SPRITEMGR_

// Common includes
#include "Common.h"
#include "Globals.h"
#include "cSprite.h"
#include "cText.h"
#include "Particles.h"
#include "cTextEntry.h"
#include "AGKShader.h"
#include "Skeleton2D.h"

// Namespace
namespace AGK
{
	//class cSprite;
	//class cParticleEmitter;
	//class cText;
	class DebugDraw;
	class agk;

	extern DebugDraw g_DebugDraw;

	class _EXPORT_ cSpriteContainer
	{
		protected:
			uint32_t m_iType;
			void *m_pItem;
					
		public:
			cSpriteContainer *m_pPrev;
			cSpriteContainer *m_pNext;

			cSpriteContainer() { m_pNext = 0; m_iType = 0; m_pItem = 0; m_pPrev = 0; }
			~cSpriteContainer() {}
		
			void SetSprite( cSprite *pSprite )
			{
				m_iType = 1;
				m_pItem = pSprite;
			}

			void SetParticles( cParticleEmitter *pParticles )
			{
				m_iType = 2;
				m_pItem = pParticles;
			}

			void SetText( cText *pText )
			{
				m_iType = 3;
				m_pItem = pText;
			}

			void SetEditBox( cEditBox *pEditBox )
			{
				m_iType = 4;
				m_pItem = pEditBox;
			}

			void SetSkeleton2D( Skeleton2D *pSkeleton )
			{
				m_iType = 5;
				m_pItem = pSkeleton;
			}

			int GetType() { return m_iType; }

			/*
			float GetDepth()
			{
				switch( m_iType )
				{
					case 1: return ((cSprite*)m_pItem)->GetZ(); break;
					case 2: return ((cParticleEmitter*)m_pItem)->GetZ(); break;
					case 3: return ((cText*)m_pItem)->GetZ(); break;
					case 4: return ((cEditBox*)m_pItem)->GetZ(); break;
					case 5: return ((Skeleton2D*)m_pItem)->GetZ(); break;
					default: return 0;
				}
			}
			*/

			int GetDepthInt()
			{
				switch( m_iType )
				{
					case 1: return ((cSprite*)m_pItem)->GetDepth(); break;
					case 2: return ((cParticleEmitter*)m_pItem)->GetDepth(); break;
					case 3: return ((cText*)m_pItem)->GetDepth(); break;
					case 4: return ((cEditBox*)m_pItem)->GetDepth(); break;
					case 5: return ((Skeleton2D*)m_pItem)->GetDepth(); break;
					default: return 0;
				}
			}

			uint32_t GetCreated()
			{
				switch( m_iType )
				{
					case 1: return ((cSprite*)m_pItem)->GetCreated(); break;
					case 2: return 0xffffffff; break;
					case 3: return 0xffffffff; break;
					case 4: return 0xffffffff; break;
					case 5: return 0xffffffff; break;
					default: return 0;
				}
			}

			const AGKRenderState* GetRenderState()
			{
				switch( m_iType )
				{
					case 1: return ((cSprite*)m_pItem)->GetRenderState(); break;
					case 2: return 0; break;
					case 3: return 0; break;
					case 4: return 0; break;
					case 5: return 0; break;
					default: return 0;
				}
			}

			cImage* GetTexture()
			{
				switch( m_iType )
				{
					case 1: 
					{
						if ( !((cSprite*)m_pItem)->GetImagePtr() ) return 0;
						else return ((cSprite*)m_pItem)->GetImagePtr()->GetRootImage();
						break;
					}

					case 2: return 0; break;
					case 3: return 0; break;
					case 4: return 0; break;
					case 5: return 0; break;
					default: return 0;
				}
			}

			AGKShader* GetShader()
			{
				switch( m_iType )
				{
					case 1: 
					{
						return ((cSprite*)m_pItem)->GetShader();
						break;
					}

					case 2: return AGKShader::g_pShaderTexColor; break;
					case 3: return AGKShader::g_pShaderTexColor; break;
					case 4: return 0; break;
					case 5: return 0; break;
					default: return 0;
				}
			}

			cSprite* GetSprite() { return (cSprite*)m_pItem; }
			cParticleEmitter* GetParticles() { return (cParticleEmitter*)m_pItem; }
			cText* GetText() { return (cText*)m_pItem; }
			cEditBox* GetEditBox() { return (cEditBox*)m_pItem; }
			Skeleton2D* GetSkeleton2D() { return (Skeleton2D*)m_pItem; }
	};
	
	class _EXPORT_ cSpriteMgrEx
	{
		protected:

			void *m_pInternalVertices = 0;
			void *m_pInternalPipeline = 0;
			void *m_pInternalConstants = 0;

			float *pVertexData = 0;
			unsigned short *pIndices = 0;
		
			int iCurrentCount = 0;
			int iCurrentCountAll = 0;
			int iCurrentResizeCount = 0;
			int iCurrentResizeAllCount = 0;
			
			cSpriteContainer *m_pSprites = 0;
			cSpriteContainer *m_pLastSprite = 0;
			AGKSortValue *m_pDrawList = 0;
			
			uint32_t m_iLastTotal = 0;
			uint32_t m_iLastDrawn = 0;
			uint32_t m_iLastDrawCalls = 0;
			uint32_t m_iLastSorted = 0;
						
			bool AddContainer( cSpriteContainer *pNewMember );
						
		public:

			cSpriteMgrEx();
			~cSpriteMgrEx();

			void ClearSprites();
			void ClearAll();
		
			uint32_t GetTotalCount() { return m_iLastTotal; }
			uint32_t GetDrawnCount() { return m_iLastDrawn; }
			uint32_t GetDrawCalls() { return m_iLastDrawCalls; }
			uint32_t GetSortedCount() { return m_iLastSorted; }
		
			void AddSprite( cSprite* sprite );
			void AddText( cText* text );
			void AddParticles( cParticleEmitter* particles );
			void AddEditBox( cEditBox* editbox );
			void AddSkeleton2D( Skeleton2D* skeleton );

			void RemoveSprite( cSprite* sprite );
			void RemoveText( cText* text );
			void RemoveParticles( cParticleEmitter* particles );
			void RemoveEditBox( cEditBox* editbox );
			void RemoveSkeleton2D( Skeleton2D* skeleton );

			virtual void UpdateAll( float time );
			//virtual void DrawAll();
			void DrawSplit( int depth, int side );
			void DrawDebug();
	};
}

#endif
