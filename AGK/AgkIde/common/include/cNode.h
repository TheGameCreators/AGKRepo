#ifndef _H_AGKNODE_
#define _H_AGKNODE_

#include "Common.h"
#include <vector>

#define AGK_NODE_UPDATED			0x0001
#define AGK_NODE_NEEDS_UPDATE		0x0002
#define AGK_NODE_OVERRIDE_POSITION	0x0004
#define AGK_NODE_OVERRIDE_ROTATION	0x0008
#define AGK_NODE_OVERRIDE_SCALE		0x0010

namespace AGK
{
	class cNode
	{
		private:

			AGKVector m_position;
			AGKQuaternion m_rotation;
			AGKVector m_scale;

			AGKVector m_positionFinal;
			AGKQuaternion m_rotationFinal;
			AGKVector m_scaleFinal;

			UINT m_iNodeFlags;

			void NeedsUpdate();
			void ChildrenNeedUpdate();

		protected:

			std::vector<cNode*> m_vChildren;
			cNode *m_pParentNode;
			
		public:
			enum eNodeType
			{
				eNode,
				eObject,
				eCamera,
				eLight,
				eBone
			};

			cNode();
			virtual ~cNode();
			virtual void UpdatedCallback() {}
			bool AwaitingUpdate() const { return (m_iNodeFlags & AGK_NODE_NEEDS_UPDATE) != 0; }

			// hierarchy
			void AddChild( cNode *pChild );
			void RemoveChild( cNode *pChild );
			void RemoveFromParent();
			void RemoveFromDeletingParent();
			void TransformChildrenByParent();

			// local transform
			float GetX() const { return m_position.x; }
			float GetY() const { return m_position.y; }
			float GetZ() const { return m_position.z; }

			float GetEulerX() const { return m_rotation.GetEulerX(); }
			float GetEulerY() const { return m_rotation.GetEulerY(); }
			float GetEulerZ() const { return m_rotation.GetEulerZ(); }

			float GetScaleX() const { return m_scale.x; }
			float GetScaleY() const { return m_scale.y; }
			float GetScaleZ() const { return m_scale.z; }

			// world transform
			float GetWorldX() { return posFinal().x; }
			float GetWorldY() { return posFinal().y; }
			float GetWorldZ() { return posFinal().z; }

			float GetWorldEulerX() { return rotFinal().GetEulerX(); }
			float GetWorldEulerY() { return rotFinal().GetEulerY(); }
			float GetWorldEulerZ() { return rotFinal().GetEulerZ(); }

			float GetWorldScaleX() { return scaleFinal().x; }
			float GetWorldScaleY() { return scaleFinal().y; }
			float GetWorldScaleZ() { return scaleFinal().z; }
			float GetLargestWorldScale();

			// set
			void SetNodePosition( float x, float y, float z ) { m_position.Set(x,y,z); NeedsUpdate(); }
			void SetNodeRotation( float w, float x, float y, float z ) { m_rotation.Set(w,x,y,z); NeedsUpdate(); }
			void SetNodeScale( float x, float y, float z ) { m_scale.Set(x,y,z); NeedsUpdate(); }

			void MoveLocalX( float amount );
			void MoveLocalY( float amount );
			void MoveLocalZ( float amount );

			void RotateLocalX( float amount );
			void RotateLocalY( float amount );
			void RotateLocalZ( float amount );

			void RotateGlobalX( float amount );
			void RotateGlobalY( float amount );
			void RotateGlobalZ( float amount );

			void LookAt( float x, float y, float z, float roll );
			
			// override hierarchy
			void OverrideWorldPosition( float x, float y, float z );
			void OverrideWorldRotation( float w, float x, float y, float z );
			void OverrideWorldScale( float x, float y, float z );
			void RemoveWorldOverride(); // removes any world override and adjusts the relative position and rotation to match the current world position and rotation

			const AGKVector& pos() const { return m_position; }
			const AGKQuaternion& rot() const { return m_rotation; }
			const AGKVector& scale() const { return m_scale; }

			const AGKVector& posFinal() { UpdateNode(); return m_positionFinal; }
			const AGKQuaternion& rotFinal() { UpdateNode(); return m_rotationFinal; }
			const AGKVector& scaleFinal() { UpdateNode(); return m_scaleFinal; }

			virtual eNodeType GetNodeType() { return eNode; }
			
			void UpdateNode();
	};
}

#endif