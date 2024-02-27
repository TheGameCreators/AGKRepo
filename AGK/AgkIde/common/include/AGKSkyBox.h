#ifndef _H_AGK_SKYBOX_
#define _H_AGK_SKYBOX_

#define AGK_SKYBOX_SUN_VISIBLE		0x0001
#define AGK_SKYBOX_ACTIVE			0x0002

// Namespace
namespace AGK
{
	class cImage;
	class cObject3D;
	class AGKShader;

	class AGKSkyBox
	{
		protected:
			static cImage *g_pGradientImage;
			cObject3D *m_pSkyObject;
			AGKShader *m_pNoSunShader;
			AGKShader *m_pSunShader;

			float m_fSkyColorR, m_fSkyColorG, m_fSkyColorB;
			float m_fHorizonColorR, m_fHorizonColorG, m_fHorizonColorB;
			float m_fSunColorR, m_fSunColorG, m_fSunColorB;
			float m_fSunSize, m_fHaloSize;
			float m_fHorizonSize, m_fHorizonHeight;
			UINT m_iFlags;

			static void BuildGradientImage();

		public:
			AGKSkyBox( int visible=1 );
			~AGKSkyBox();

			void Reset();

			void SetActive( int active );
			void SetSkyColor( float red, float green, float blue );
			void SetHorizonColor( float red, float green, float blue );
			void SetSunColor( float red, float green, float blue );
			void SetSunSize( float sun, float halo );
			void SetHorizonSize( float size, float height );
			void SetSunVisible( int visible );

			void Draw();
	};
}

#endif