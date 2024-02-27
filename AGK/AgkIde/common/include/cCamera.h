#ifndef _H_CAMERA_
#define _H_CAMERA_

// Common includes
#include "Common.h"
#include "3DMath.h"
#include "cNode.h"

#define AGK_CAMERA_UPDATED					0x0001
#define AGK_CAMERA_PROJ_UPDATED				0x0002
#define AGK_CAMERA_PROJ_NEEDS_UPDATE		0x0004
#define AGK_CAMERA_USING_FBO				0x0008

#define AGK_CAMERA_PLANE_NEEDS_UPDATE_0		0x0010
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_1		0x0020
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_2		0x0040
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_3		0x0080
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_4		0x0100
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_5		0x0200
#define AGK_CAMERA_PLANE_NEEDS_UPDATE_ALL	0x03F0	// all plane update flags OR'ed together
#define AGK_CAMERA_OFF_CENTER				0x0400

// Namespace
namespace AGK
{
	class cCamera : public cNode
	{
		protected:
			// local transform now stored in cNode
			float m_fFOV;
			float m_fNear;
			float m_fFar;
			float m_fLeft;
			float m_fRight;
			float m_fTop;
			float m_fBottom;
			float m_fAspect;
			float m_fOrthoWidth;
			UINT m_iFlags;
			AGKMatrix4 m_matProj;

			AGKVector m_vFrustumN[ 6 ];
			float m_fFrustumD[ 6 ];

			cCamera *m_pPrevCamera;
			cCamera *m_pNextCamera;
			static cCamera *g_pAllCameras;

			void SetUsingFBO( int mode );

		public:
			static void ResetAllUpdated();
			static void ResetAllProjUpdated();
			static void UpdateAllAspectRatio( float aspect );
			static void UpdateAllUsingFBO( int mode );

			cCamera();
			~cCamera();

			void UpdatedCallback() { m_iFlags |= (AGK_CAMERA_UPDATED | AGK_CAMERA_PLANE_NEEDS_UPDATE_ALL); }

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
			
			void GetViewMatrix( AGKMatrix4 &matOut ) { matOut.MakeInvWorld( rotFinal(), posFinal(), AGKVector(1,1,1) ); }
			void GetViewNormalMatrix( AGKMatrix3 &matOut ) { matOut.MakeWorld( rotFinal(), -AGKVector(1,1,1) ); }
			const AGKMatrix4* GetProjMatrix();

			void GetFrustumPlane( UINT index, AGKVector &n, float &d );
			void GetFrustumPoints( float dist, AGKVector *points );

			bool HasUpdated() { return (m_iFlags & AGK_CAMERA_UPDATED) != 0 || AwaitingUpdate(); }
			bool HasProjUpdated() { return (m_iFlags & (AGK_CAMERA_PROJ_UPDATED | AGK_CAMERA_PROJ_NEEDS_UPDATE)) != 0; }
	};
}

#endif