// Image Header
#ifndef _H_AGKLIGHTS_
#define _H_AGKLIGHTS_

// Common includes
#include "Common.h"
#include "3DMath.h"

// Namespace
namespace AGK
{
	class AGKPointLight
	{
		public:
			AGKVector m_position;
			AGKVector m_color;
			float m_fRadius;
			int m_iType; // 0=vertex, 1=pixel

			AGKPointLight()
			{
				m_fRadius = 1;
				m_color.Set( 1,1,1 );
				m_iType = 0;
			}

			void SetColor( int red, int green, int blue ) { m_color.Set( red/255.0f, green/255.0f, blue/255.0f ); }
			void SetPosition( float x, float y, float z ) { m_position.Set(x,y,z); }
			void SetRadius( float radius ) { m_fRadius = radius; }
			void SetType( int mode ) { m_iType = mode; }
	};

	class AGKDirectionalLight
	{
		public:
			AGKVector m_direction;
			AGKVector m_color;
			int m_active;

			AGKDirectionalLight()
			{
				Reset();
			}

			void Reset()
			{
				m_color.Set( 1,1,1 );
				m_direction.Set( -0.3714f, -0.7428f, 0.5571f ); // normalized
				m_active = 1;
			}

			void SetColor( int red, int green, int blue ) { m_color.Set( red/255.0f, green/255.0f, blue/255.0f ); }
			void SetDirection( float x, float y, float z ) { m_direction.Set(x,y,z); m_direction.Normalize(); }
	};
}

#endif