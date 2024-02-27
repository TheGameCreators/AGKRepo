#ifndef _H_AGK_3DPARTICLES_
#define _H_AGK_3DPARTICLES_

// Common includes
#include "Common.h"
#include "cImage.h"
#include "Globals.h"

// Namespace
namespace AGK
{
	class AGK3DParticle
	{
		protected:
			UINT m_iColor;

		public:
			float m_fX;
			float m_fY;
			float m_fZ;
			float m_fVX;
			float m_fVY;
			float m_fVZ;
			//float m_fAngle;
			//float m_fAngleDelta;
			float m_fScale;
			float m_fTime;
			bool m_bAlive;
			
			AGK3DParticle();
			~AGK3DParticle() {}

			UINT GetColor() { return m_iColor; }

			UINT GetAlpha() { return m_iColor >> 24; }
			UINT GetBlue() { return (m_iColor >> 16) & 0xff; }
			UINT GetGreen() { return (m_iColor >> 8) & 0xff; }
			UINT GetRed() { return m_iColor & 0xff; }

			void SetColor( UINT red, UINT green, UINT blue, UINT alpha )
			{
				if ( red > 255 ) red = 255;
				if ( green > 255 ) green = 255;
				if ( blue > 255 ) blue = 255;
				if ( alpha > 255 ) alpha = 255;

				m_iColor = (((((alpha << 8) | blue) << 8) | green) << 8) | red;
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
			AGK3DParticleForce *m_pNext;
	};

	class AGK3DParticleColor
	{
		public:
			float m_fTime;
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			unsigned char alpha;
			AGK3DParticleColor *m_pNext;
	};

	class AGK3DParticleSize
	{
		public:
			float m_fTime;
			float m_fScale;
			AGK3DParticleSize *m_pNext;
	};

#define AGK_3DPARTICLE_VISIBLE		0x01
#define AGK_3DPARTICLE_ACTIVE		0x02
#define AGK_3DPARTICLE_FACEDIR		0x04
#define AGK_3DPARTICLE_INTERCOLOR	0x08

	class AGK3DParticleEmitter
	{
		protected:
			UINT m_iID;
			float m_fX;
			float m_fY;
			float m_fZ;
			float m_fVX;
			float m_fVY;
			float m_fVZ;
			float m_fRoll;
			float m_fAngle1;
			float m_fAngle2;
			float m_fVMin;
			float m_fVMax;
			float m_fSize;
			float m_fLife;
			float m_fFreq;
			float m_fNumStart;
			UINT m_iCurrParticle;
			UINT m_iNumParticles;
			UINT m_bFlags;
			float m_fAMin;
			float m_fAMax;

			int m_iMaxParticles;
			int m_iEmittedParticles;
			bool m_bSomeAlive;

			float m_fStartX1;
			float m_fStartY1;
			float m_fStartZ1;
			float m_fStartX2;
			float m_fStartY2;
			float m_fStartZ2;

			UINT m_iImageID;
			cImage *m_pImage;
			AGK3DParticle **m_pParticles;
			AGK3DParticleForce *m_pForces;
			AGK3DParticleColor *m_pColors;
			AGK3DParticleSize *m_pSizes;

			UINT m_iNumVertices;
			float *m_pVertexArray;
			float *m_pUVArray;
			UINT *m_pColorArray;
			unsigned short *m_pIndices;
			int m_iTransparencyMode;
			
			void UpdateNumParticles();

			void PlatformDrawParticles( UINT count, unsigned short *pIndices, float *pVertices, float *pUV, unsigned char *pColors );
						
		public:
		
			static int m_iParticlesDrawn;
			
			AGK3DParticleEmitter();
			~AGK3DParticleEmitter();

			UINT GetID() { return m_iID; }
			void SetID( UINT ID ) { m_iID = ID; }
			
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
			int GetVisible() { return (m_bFlags & AGK_3DPARTICLE_VISIBLE) ? 1 : 0; }
			int GetActive() { return (m_bFlags & AGK_3DPARTICLE_ACTIVE) ? 1 : 0; }
			int GetTransparency() { return m_iTransparencyMode; }

			void Offset( float x, float y, float z );

			void AddForce( float starttime, float endtime, float vx, float vy, float vz );
			void ClearForces();
			void AddColorKeyFrame( float time, UINT red, UINT green, UINT blue, UINT alpha );
			void ClearColors();
			void AddScaleKeyFrame( float time, float scale );
			void ClearScales();

			void Update( float time );
			void DrawAll();
	};
}

#endif