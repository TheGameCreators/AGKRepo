#ifndef _H_JOYSTICK_
#define _H_JOYSTICK_

#include "Common.h"

#define AGK_NUM_JOYSTICKS 8
#define AGK_MAX_JOYSTICK_BUTTONS 64

namespace AGK
{
	class cSprite;
	class cMouse;
	class cTouch;

	class _EXPORT_ cVirtualButton
	{
		protected:
			friend class agk;

			cText *m_pText;

			bool m_bPrevDown;
			bool m_bDown;

			static cImage *g_pButtonImage;
			static cImage *g_pButtonDownImage;

			cSprite *m_pButtonSprite;
			cSprite *m_pButtonDownSprite;

			bool m_bActive;
			bool m_bVisible;

			cMouse *m_pCapturedMouse;
			cTouch *m_pCapturedTouch;

			void Zero();

		public:
			cVirtualButton( );
			cVirtualButton( float x, float y, float size );
			~cVirtualButton();

			void SetPosition( float x, float y );
			void SetSize( float size );
			void SetSize( float sizeX, float sizeY );
			void SetColor( uint32_t red, uint32_t green, uint32_t blue );
			void SetAlpha( uint32_t alpha );
			void SetActive( bool active );
			void SetVisible( bool visible );
			void SetUpImage( cImage *pImage );
			void SetDownImage( cImage *pImage );
			void SetText( const char *str );

			void CaptureMouse( cMouse *pMouse ) { m_pCapturedMouse = pMouse; }
			void CaptureTouch( cTouch *pTouch ) { m_pCapturedTouch = pTouch; }

			bool GetState() { return m_bDown; }
			bool GetPressed() { return !m_bPrevDown && m_bDown; }
			bool GetReleased() { return m_bPrevDown && !m_bDown; }
			bool GetHitTest( float x, float y );
			
			void Update();
			void Draw();
	};

	class _EXPORT_ cVirtualJoystick
	{
		protected:
			friend class agk;

			float m_fX;
			float m_fY;

			static cImage *g_pOuterImage;
			static cImage *g_pInnerImage;

			cSprite *m_pOuterSprite;
			cSprite *m_pInnerSprite;

			static float g_fDeadZone;

			bool m_bActive;
			bool m_bVisible;

			cMouse *m_pCapturedMouse;
			cTouch *m_pCapturedTouch;

			void Zero();

		public:
			cVirtualJoystick( );
			cVirtualJoystick( float x, float y, float size );
			~cVirtualJoystick();

			static void SetDeadZone( float zone ) { g_fDeadZone = zone; }

			void SetPosition( float x, float y );
			void SetSize( float size );
			void SetAlpha( uint32_t alpha1, uint32_t alpha2 );
			void SetActive( bool active );
			void SetVisible( bool visible );
			void SetInnerImage( cImage *pImage );
			void SetOuterImage( cImage *pImage );

			void CaptureMouse( cMouse *pMouse ) { m_pCapturedMouse = pMouse; }
			void CaptureTouch( cTouch *pTouch ) { m_pCapturedTouch = pTouch; }

			float GetX();
			float GetY();
			bool GetHitTest( float x, float y );
			
			void Update();
			void Draw();
	};

	class _EXPORT_ cJoystick
	{
		protected:
			friend class agk;

			void *m_pDevice;
			int m_iDeviceType;

			int m_iConnected;
			uString m_sName;

			float m_fX;
			float m_fY;
			float m_fZ;

			float m_fRX;
			float m_fRY;
			float m_fRZ;

			int m_iSlider[ 2 ];
			int m_iPOV[ 4 ];

			uint32_t m_iNumButtons;
			char m_iPrevButtons[ AGK_MAX_JOYSTICK_BUTTONS ];
			char m_iButtons[ AGK_MAX_JOYSTICK_BUTTONS ];
			char m_iResetButtons[ AGK_MAX_JOYSTICK_BUTTONS ];

			void PlatformUpdate();
			//void PlatformDelete();

			static float g_fDeadZone;

			void Zero();

		public:
			//cJoystick( float x, float y, float size );
			cJoystick( void *pDevice );
			cJoystick( void *pDevice, int devicetype );
			~cJoystick() { /*PlatformDelete();*/ }

			static void SetDeadZone( float zone ) { g_fDeadZone = zone; }
			static void DetectJoysticks();

			int GetConnected() { return m_iConnected; }
			const char* GetName() { return m_sName.GetStr(); }
			void SetName( const char* name ) { m_sName.SetStr( name ); }

			float GetX();
			float GetY();
			float GetZ();

			float GetRX();
			float GetRY();
			float GetRZ();

			int GetSlider( uint32_t slider );
			int GetPOV( uint32_t pov );

			uint32_t GetNumButtons() { return m_iNumButtons; }
			bool GetButtonPressed( uint32_t button )
			{
				if ( button >= AGK_MAX_JOYSTICK_BUTTONS ) return 0;
				if ( !m_iPrevButtons[ button ] && m_iButtons[ button ] ) return true;
				return false;
			}

			bool GetButtonReleased( uint32_t button )
			{
				if ( button >= AGK_MAX_JOYSTICK_BUTTONS ) return 0;
				if ( m_iPrevButtons[ button ] && !m_iButtons[ button ] ) return true;
				return false;
			}

			bool GetButtonState( uint32_t button )
			{
				if ( button >= AGK_MAX_JOYSTICK_BUTTONS ) return 0;
				return m_iButtons[ button ] != 0;
			}

			void Update();
	};
}

#endif
