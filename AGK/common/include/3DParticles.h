#ifndef _H_AGK_3DPARTICLES_
#define _H_AGK_3DPARTICLES_

// Common includes
#include "Common.h"

// Namespace
namespace AGK
{
	class cImage;

	class AGK3DParticle
	{
		protected:
			uint32_t m_iColor;

		public:
			float m_fX = 0;
			float m_fY = 0;
			float m_fZ = 0;
			float m_fVX = 0;
			float m_fVY = 0;
			float m_fVZ = 0;
			//float m_fAngle;
			//float m_fAngleDelta;
			float m_fRed = 1.0f;
			float m_fGreen = 1.0f;
			float m_fBlue = 1.0f;
			float m_fAlpha = 1.0f;
			float m_fScale = 1;
			float m_fTime = 0;
			bool m_bAlive = false;
			uint16_t m_iColorIndex = 0;
			uint16_t m_iSizeIndex = 0;
			
			AGK3DParticle() {}
			~AGK3DParticle() {}

			void SetColor( float red, float green, float blue, float alpha )
			{
				m_fRed = red;
				m_fGreen = green;
				m_fBlue = blue;
				m_fAlpha = alpha;
			}
	};

	class AGK3DParticleForce
	{
		public:
			float m_fStartTime;
			float m_fEndTime;
			float m_fX;
			float m_fY;
			float m_fZ;
	};

	class AGK3DParticleColor
	{
		public:
			float m_fTime;
			float m_fRed;
			float m_fGreen;
			float m_fBlue;
			float m_fAlpha;
	};

	class AGK3DParticleSize
	{
		public:
			float m_fTime;
			float m_fScale;
	};

#define AGK_3DPARTICLE_VISIBLE		0x01
#define AGK_3DPARTICLE_ACTIVE		0x02
#define AGK_3DPARTICLE_FACEDIR		0x04
#define AGK_3DPARTICLE_INTERP_COLOR	0x08
#define AGK_3DPARTICLE_SOME_ALIVE	0x10

	class AGK3DParticleEmitter
	{
		protected:
			uint32_t m_iID = 0;
			float m_fX = 0;
			float m_fY = 0;
			float m_fZ = 0;
			float m_fVX = 0;
			float m_fVY = 10;
			float m_fVZ = 0;
			float m_fRoll = 0;
			float m_fAngle1 = 0;
			float m_fAngle2 = 0;
			float m_fVMin = 1;
			float m_fVMax = 1;
			float m_fSize = 0;
			float m_fLife = 3;
			float m_fFreq = 10;
			float m_fNumStart = 0;
			uint32_t m_iCurrParticle = 0;
			uint32_t m_iNumParticles = 12;
			uint32_t m_iFlags = AGK_3DPARTICLE_ACTIVE | AGK_3DPARTICLE_VISIBLE | AGK_3DPARTICLE_INTERP_COLOR;
			float m_fAMin = 0;
			float m_fAMax = 0;
			int m_iTransparencyMode = 0; // must be 0 so that the call to SetTransparency(1) does something

			int m_iMaxParticles = -1;
			int m_iEmittedParticles = 0;

			float m_fStartX1 = 0;
			float m_fStartY1 = 0;
			float m_fStartZ1 = 0;
			float m_fStartX2 = 0;
			float m_fStartY2 = 0;
			float m_fStartZ2 = 0;

			uint32_t m_iImageID = 0;
			cImage *m_pImage = 0;
			AGK3DParticle **m_pParticles = 0;

			AGK3DParticleForce *m_pForces = 0;
			uint32_t m_iNumForces = 0;
			uint32_t m_iMaxNumForces = 0;
			AGK3DParticleColor *m_pColors = 0;
			uint32_t m_iNumColors = 0;
			uint32_t m_iMaxNumColors = 0;
			AGK3DParticleSize *m_pSizes = 0;
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
			
			void UpdateNumParticles();
									
		public:
		
			static int m_iParticlesDrawn;

			static int UpdateVertexLayout( AGKVertexLayout *pLayout, AGKShader *pShader );
			static int GetParticleVertexFloats() { return 3 + 4 + 2; } // position + color + uv
			static int GetParticleVertexPosOffset() { return 0; }
			static int GetParticleVertexColorOffset() { return 3; }
			static int GetParticleVertexUVOffset() { return 7; }
			
			AGK3DParticleEmitter();
			~AGK3DParticleEmitter();

			uint32_t GetID() { return m_iID; }
			void SetID( uint32_t ID ) { m_iID = ID; }
			
			void SetPosition( float x, float y, float z );
			void SetFrequency( float freq );
			void SetStartZone( float x1, float y1, float z1, float x2, float y2, float z2 );
			void SetDirection( float vx, float vy, float vz, float roll );
			void SetVelocityRange( float v1, float v2 );
			void SetDirectionRange( float angle1, float angle2 );
			void SetSize( float size );
			void SetLife( float time );
			void SetImage( cImage *pImage );
			void SetColorInterpolation( int mode );
			void SetMaxParticles( int max );
			void ResetParticleCount();
			void SetVisible( int visible );
			void SetActive( int active );
			//void SetRotationRate( float a1, float a2 );
			//void SetRotationRateRad( float a1, float a2 );
			//void SetFaceDirection( int flag );
			void SetTransparency( int mode );

			float GetX() { return m_fX; }
			float GetY() { return m_fY; }
			float GetZ() { return m_fZ; }
			float GetFrequency() { return m_fFreq; }
			float GetDirectionX() { return m_fVX; }
			float GetDirectionY() { return m_fVY; }
			float GetDirectionZ() { return m_fVZ; }
			float GetDirectionRange1() { return m_fAngle1; }
			float GetDirectionRange2() { return m_fAngle2; }
			float GetSize() { return m_fSize; }
			float GetLife() { return m_fLife; }
			int GetMaxParticlesReached();
			int GetVisible() { return (m_iFlags & AGK_3DPARTICLE_VISIBLE) ? 1 : 0; }
			int GetActive() { return (m_iFlags & AGK_3DPARTICLE_ACTIVE) ? 1 : 0; }
			int GetSomeAlive() { return (m_iFlags & AGK_3DPARTICLE_SOME_ALIVE) ? 1 : 0; }
			int GetInterpolateColor() { return (m_iFlags & AGK_3DPARTICLE_INTERP_COLOR) ? 1 : 0; }
			int GetTransparency() { return m_iTransparencyMode; }

			void Offset( float x, float y, float z );

			void AddForce( float starttime, float endtime, float vx, float vy, float vz );
			void ClearForces();
			void AddColorKeyFrame( float time, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			void AddColorKeyFrameF( float time, float red, float green, float blue, float alpha );
			void ClearColors();
			void AddScaleKeyFrame( float time, float scale );
			void ClearScales();

			void Update( float time );
			void DrawAll();
	};
}

#endif