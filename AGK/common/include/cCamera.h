#ifndef _H_CAMERA_
#define _H_CAMERA_

// Common includes
#include "Common.h"
#include "3DMath.h"
#include "cNode.h"
#include "AGKShader.h"

#define AGK_CAMERA_VIEW_NEEDS_UPDATE				0x0001
#define AGK_CAMERA_PROJ_NEEDS_UPDATE				0x0002
#define AGK_CAMERA_SHADER_VIEW_NEEDS_UPDATE			0x0004
#define AGK_CAMERA_SHADER_PROJ_NEEDS_UPDATE			0x0008
#define AGK_CAMERA_SHADER_VIEW_PROJ_NEEDS_UPDATE	0x0010

// plane update defines must be in consecutive bits
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_0				0x0020
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_1				0x0040
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_2				0x0080
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_3				0x0100
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_4				0x0200
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_5				0x0400
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_ALL			0x07E0	// all plane update flags OR'ed together
#define AGK_CAMERA_OFF_CENTER						0x0800

#define AGK_CAMERA_PROJ_INVERTED					0x1000
#define AGK_CAMERA_SHADER_POS_NEEDS_UPDATE			0x2000

// Namespace
namespace AGK
{
	class cCamera : public cNode
	{
		protected:
			// local transform now stored in cNode
			float m_fFOV = 70;
			float m_fFOV2Tan = 0.700207538f;
			float m_fNear = 1;
			float m_fFar = 1000;
			float m_fLeft = -40;
			float m_fRight = 40;
			float m_fTop = 30;
			float m_fBottom = -30;
			float m_fAspect = 1.33333333f;
			float m_fOrthoWidth = 40;
			uint32_t m_iFlags = AGK_CAMERA_SHADER_POS_NEEDS_UPDATE | AGK_CAMERA_VIEW_NEEDS_UPDATE | AGK_CAMERA_PROJ_NEEDS_UPDATE | AGK_CAMERA_PLANE_NEEDS_UPDATE_ALL;

			AGKMatrix4 m_matView;
			AGKMatrix4 m_matProj;

			AGKShaderConstantValue m_pShaderView;
			AGKShaderConstantValue m_pShaderProj;
			AGKShaderConstantValue m_pShaderViewProj;
			AGKShaderConstantValue m_pShaderPos;

			AGKVector m_vFrustumN[ 6 ];
			float m_fFrustumD[ 6 ];

			cCamera *m_pPrevCamera = 0;
			cCamera *m_pNextCamera = 0;
			static cCamera *g_pAllCameras;

			void SetInvertProj( int mode );

		public:
			static void UpdateAllAspectRatio( float aspect );
			static void UpdateAllInvert( int mode );

			cCamera();
			~cCamera();

			void NeedsUpdateCallback() { m_iFlags |= (AGK_CAMERA_VIEW_NEEDS_UPDATE | AGK_CAMERA_PLANE_NEEDS_UPDATE_ALL | AGK_CAMERA_SHADER_POS_NEEDS_UPDATE); }

			void SetPosition( float x, float y, float z ) { SetNodePosition(x, y, z); }
			void SetRotationQuat( const AGKQuaternion &rot ) { SetNodeRotation(rot.w, rot.x, rot.y, rot.z); }
			void SetRotationEuler( float angX, float angY, float angZ ) { AGKQuaternion q; q.MakeFromEulerYXZ(angX, angY, angZ); SetNodeRotation(q.w, q.x, q.y, q.z); }

			void SetAspectRatio( float aspect );
			void SetRange( float zNear, float zFar );
			void SetBounds( float fLeft, float fRight, float fTop, float fBottom ); // only used if SetOffCenter is set to 1
			void SetOffCenter( int mode );
			void SetFOV( float FOV );
			void SetOrthoWidth( float width ); // overwrites the results of SetBounds()

			float GetFOV() { return m_fFOV; }
			float GetNearRange() { return m_fNear; }
			float GetFarRange() { return m_fFar; }
			float GetAspectRatio() { return m_fAspect; }
			bool IsInverted() { return (m_iFlags & AGK_CAMERA_PROJ_INVERTED) != 0; }

			const AGKMatrix4* GetViewMatrix();
			const AGKMatrix4* GetProjMatrix();

			AGKShaderConstantValue* GetShaderView();
			AGKShaderConstantValue* GetShaderProj();
			AGKShaderConstantValue* GetShaderViewProj();
			AGKShaderConstantValue* GetShaderPos();

			void GetFrustumPlane( uint32_t index, AGKVector &n, float &d );
			void GetFrustumPoints( float dist, AGKVector *points );
	};
}

#endif