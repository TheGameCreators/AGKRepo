#ifndef _H_PARTICLES_
#define _H_PARTICLES_

// Common includes
#include "Common.h"

#define AGK_PARTICLES_INTERP_COLOR		0x0001
#define AGK_PARTICLES_FACE_DIR			0x0002
#define AGK_PARTICLES_VISIBLE			0x0004
#define AGK_PARTICLES_ACTIVE			0x0008
#define AGK_PARTICLES_SOME_ALIVE		0x0010
#define AGK_PARTICLES_FIXED_TO_SCREEN	0x0020
#define AGK_PARTICLES_MANAGED_DRAW		0x0040
#define AGK_PARTICLES_INT_INDICES		0x0080

// Namespace
namespace AGK
{
	class cSpriteMgrEx;

	class cParticle
	{
		public:
			float m_fX = 0;
			float m_fY = 0;
			float m_fVX = 0;
			float m_fVY = 0;
			float m_fAngle = 0;
			float m_fAngleDelta = 0;
			float m_fScale = 1;
			float m_fTime = 0;
			float m_fRed = 1.0f;
			float m_fGreen = 1.0f;
			float m_fBlue = 1.0f;
			float m_fAlpha = 1.0f;
			bool m_bAlive = false;
			uint16_t m_iColorIndex = 0;
			uint16_t m_iSizeIndex = 0;
			
			cParticle() {}
			~cParticle() {}

			void SetColor( float red, float green, float blue, float alpha )
			{
				m_fRed = red;
				m_fGreen = green;
				m_fBlue = blue;
				m_fAlpha = alpha;
			}
	};

	class cParticleForce
	{
		public:
			float m_fStartTime;
			float m_fEndTime;
			float m_fX;
			float m_fY;
	};

	class cParticleColor
	{
		public:
			float m_fTime;
			float m_fRed;
			float m_fGreen;
			float m_fBlue;
			float m_fAlpha;
	};

	class cParticleSize
	{
		public:
			float m_fTime;
			float m_fScale;
	};

	class cParticleEmitter
	{
		protected:
			uint32_t m_iID = 0;
			float m_fX = 0;
			float m_fY = 0;
			float m_fVX = 0;
			float m_fVY = 10;
			float m_fAngle = 10;
			float m_fVMin = 1;
			float m_fVMax = 1;
			float m_fSize = 0;
			float m_fDepth = 0.001f;
			int m_iDepth = 10;
			float m_fLife = 3;
			float m_fFreq = 10;
			float m_fNumStart = 0;
			uint32_t m_iCurrParticle = 0;
			uint32_t m_iNumParticles = 12;
			float m_fAMin = 0;
			float m_fAMax = 0;
			int m_iTransparencyMode = 0; // must be 0 so that the call to SetTransparency(1) does something
			uint32_t m_iFlags = AGK_PARTICLES_INTERP_COLOR | AGK_PARTICLES_ACTIVE | AGK_PARTICLES_VISIBLE;

			int m_iMaxParticles = -1;
			int m_iEmittedParticles = 0;

			float m_fStartX1 = 0;
			float m_fStartY1 = 0;
			float m_fStartX2 = 0;
			float m_fStartY2 = 0;

			uint32_t m_iImageID = 0;
			cImage *m_pImage = 0;
			cParticle **m_pParticles = 0;

			cParticleForce *m_pForces = 0;
			uint32_t m_iNumForces = 0;
			uint32_t m_iMaxNumForces = 0;
			cParticleColor *m_pColors = 0;
			uint32_t m_iNumColors = 0;
			uint32_t m_iMaxNumColors = 0;
			cParticleSize *m_pSizes = 0;
			uint32_t m_iNumSizes = 0;
			uint32_t m_iMaxNumSizes = 0;

			void* m_pInternalVertices = 0;
			void* m_pInternalPipeline = 0;
			void* m_pInternalConstants = 0;
			AGKRenderState m_renderState;
			AGKVertexLayout m_vertexLayout;
			AGKShader *m_pShader = 0;

			uint32_t m_iNumVertices = 0;
			float *m_pVertexData = 0;
			void *m_pIndices = 0; // could be unsigned short or unsigned int
						
			cSpriteMgrEx* m_pSpriteManager = 0;

			void UpdateNumParticles();

		public:
		
			static int m_iQuadParticlesDrawn;

			static int UpdateVertexLayout( AGKVertexLayout *pLayout, AGKShader *pShader );
			static int GetParticleVertexFloats() { return 3 + 4 + 2; } // position + color + uv
			static int GetParticleVertexPosOffset() { return 0; }
			static int GetParticleVertexColorOffset() { return 3; }
			static int GetParticleVertexUVOffset() { return 7; }
						
			cParticleEmitter();
			~cParticleEmitter();

			uint32_t GetID() { return m_iID; }
			void SetID( uint32_t ID ) { m_iID = ID; }
			void SetSpriteManager( cSpriteMgrEx *pMgr );
			
			void SetPosition( float x, float y );
			void SetDepth( int depth );
			void SetFrequency( float freq );
			void SetStartZone( float x1, float y1, float x2, float y2 );
			void SetDirection( float vx, float vy );
			void SetVelocityRange( float v1, float v2 );
			void SetAngle( float angle );
			void SetAngleRad( float angle );
			void SetSize( float size );
			void SetLife( float time );
			void SetImage( cImage *pImage );
			void SetColorInterpolation( int mode ) { if (mode) m_iFlags |= AGK_PARTICLES_INTERP_COLOR; else m_iFlags &= ~AGK_PARTICLES_INTERP_COLOR; }
			void SetMaxParticles( int max );
			void ResetParticleCount();
			void SetVisible( int visible ) { if (visible) m_iFlags |= AGK_PARTICLES_VISIBLE; else m_iFlags &= ~AGK_PARTICLES_VISIBLE; }
			void SetActive( int active ) { if (active) m_iFlags |= AGK_PARTICLES_ACTIVE; else m_iFlags &= ~AGK_PARTICLES_ACTIVE; }
			void SetRotationRate( float a1, float a2 );
			void SetRotationRateRad( float a1, float a2 );
			void SetFaceDirection( int flag ) { if (flag) m_iFlags |= AGK_PARTICLES_FACE_DIR; else m_iFlags &= ~AGK_PARTICLES_FACE_DIR; }
			void SetTransparency( int mode );

			float GetX() { return m_fX; }
			float GetY() { return m_fY; }
			float GetZ() { return m_fDepth; }
			int GetDepth();
			float GetFrequency() { return m_fFreq; }
			float GetDirectionX() { return m_fVX; }
			float GetDirectionY() { return m_fVY; }
			float GetAngle() { return m_fAngle * 180 / PI; }
			float GetAngleRad() { return m_fAngle; }
			float GetSize() { return m_fSize; }
			float GetLife() { return m_fLife; }
			int GetMaxParticlesReached();
			bool GetVisible() { return (m_iFlags & AGK_PARTICLES_VISIBLE) != 0; }
			bool GetActive() { return (m_iFlags & AGK_PARTICLES_ACTIVE) != 0; }
			bool GetFaceDirection() { return (m_iFlags & AGK_PARTICLES_FACE_DIR) != 0; }
			bool GetInterpolateColor() { return (m_iFlags & AGK_PARTICLES_INTERP_COLOR) != 0; }
			bool IsFixedToScreen() { return (m_iFlags & AGK_PARTICLES_FIXED_TO_SCREEN) != 0; }
			bool GetSomeAlive() { return (m_iFlags & AGK_PARTICLES_SOME_ALIVE) != 0; }
			int GetTransparency() { return m_iTransparencyMode; }

			void Offset( float x, float y );

			void AddForce( float starttime, float endtime, float vx, float vy );
			void ClearForces();
			void AddColorKeyFrame( float time, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			void AddColorKeyFrameF( float time, float red, float green, float blue, float alpha );
			void ClearColors();
			void AddScaleKeyFrame( float time, float scale );
			void ClearScales();

			void FixToScreen( bool fix ) { if (fix) m_iFlags |= AGK_PARTICLES_FIXED_TO_SCREEN; else m_iFlags &= ~AGK_PARTICLES_FIXED_TO_SCREEN; }
			
			bool IsManaged() { return (m_iFlags & AGK_PARTICLES_MANAGED_DRAW) != 0; }
			void Update( float time );
			void DrawAll();
	};
}

#endif