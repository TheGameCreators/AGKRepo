#ifndef _H_AGK_SKELETON3D
#define _H_AGK_SKELETON3D

#define AGK_BONE3D_ANIMATE				0x0001
#define AGK_BONE3D_ROOT					0x0002

#define AGK_SKELETON3D_PLAYING          0x0001
#define AGK_SKELETON3D_LOOPING          0x0002

#define AGK_ANIMATION3D_USER_LOADED		0x0001

struct aiNode;
struct aiAnimation;

#include <vector>

namespace AGK
{
	// rotation keyframes
	class Anim3DKeyFrameRotation
	{
		public:
			float m_fTime;
			AGKQuaternion m_qRotation;

			Anim3DKeyFrameRotation() { m_fTime = 0; }
			~Anim3DKeyFrameRotation() {}

			void Interpolate( Anim3DKeyFrameRotation* pNext, float t, AGKQuaternion &r )
			{
				if ( !pNext ) 
				{
					r = m_qRotation;
					return;
				}

				//t = t * t * (3.0f - 2.0f * t); // smooth interpolation
				r.Lerp( m_qRotation, pNext->m_qRotation, t );			
			}

			void Stepped( AGKQuaternion &r )
			{
				r = m_qRotation;
			}
	};

	// position keyframes
	class Anim3DKeyFramePosition
	{
		public:
			float m_fTime;
			AGKVector m_position;

			Anim3DKeyFramePosition() { m_fTime = 0; }
			~Anim3DKeyFramePosition() {}

			void Interpolate( Anim3DKeyFramePosition* pNext, float t, AGKVector &next )
			{
				if ( !pNext ) 
				{
					next = m_position;
					return;
				}

				//t = t * t * (3.0f - 2.0f * t); // smooth interpolation
				next.Interpolate( m_position, pNext->m_position, t );
			}

			void Stepped( AGKVector &next )
			{
				next = m_position;
			}
	};

	// scale keyframes
	class Anim3DKeyFrameScale
	{
		public:
			float m_fTime;
			AGKVector m_scale;

			Anim3DKeyFrameScale() { m_fTime = 0; }
			~Anim3DKeyFrameScale() {}

			void Interpolate( Anim3DKeyFrameScale* pNext, float t, AGKVector &scale )
			{
				if ( !pNext ) 
				{
					scale = m_scale;
					return;
				}

				//t = t * t * (3.0f - 2.0f * t); // smooth interpolation
				scale.Interpolate( m_scale, pNext->m_scale, t );
			}

			void Stepped( AGKVector &scale )
			{
				scale = m_scale;
			}
	};

	class Anim3DBone
	{
		public:
			uString m_sBoneName;
			
			UINT m_iNumRotations;
			Anim3DKeyFrameRotation *m_pRotations;

			UINT m_iNumPositions;
			Anim3DKeyFramePosition *m_pPositions;

			UINT m_iNumScales;
			Anim3DKeyFrameScale *m_pScales;

			Anim3DBone();
			void Copy( const Anim3DBone *pOther );
			~Anim3DBone();
	};

	class Bone3D : public cNode
	{
		public:
			UINT m_iIndex;
			uString m_sName;
			AGKVector m_origPosition;
			AGKQuaternion m_origRotation;
			AGKVector m_origScale;
			AGKVector m_offsetPosition;
			AGKQuaternion m_offsetRotation;
			UINT m_bFlags;
			Bone3D *m_pParent;
			AGKQuaternion m_tempRot; // store the inverse bind plus bone transform for child bones to compare their ortho direction

			AGKVector m_tweenPosition;
			AGKQuaternion m_tweenRotation;
			AGKVector m_tweenScale;

			Anim3DBone *m_pAnim;
			UINT m_iCurrFramePosition;
			UINT m_iCurrFrameRotation;
			UINT m_iCurrFrameScale;

			Box m_BoundingBox;
			
			Bone3D();
			Bone3D( const Bone3D *pOther );
			~Bone3D();

			void Tween( float currtime, float s );
			void Interpolate( float currtime );

			void SetCanAnimate( int animate );
			void ResetToOrig();

			void DeleteAttachedObjects();
	};

	class Animation3D
	{
		protected:
			struct sSearchItem
			{
				const char* m_sName;
				UINT m_iIndex;
			};

			sSearchItem *m_pBoneSearchList;
			static int CompareSearchItem( const void* a, const void* b )
			{
				sSearchItem* s1 = (sSearchItem*)a;
				sSearchItem* s2 = (sSearchItem*)b;

				return strcmp( s1->m_sName, s2->m_sName );
			}

			UINT m_iID;
			UINT m_iFlags;
			int m_iReferences;

			uString m_sName;
			float m_fTime;
			
			UINT m_iNumBones;
			Anim3DBone *m_pBoneAnims;

			~Animation3D(); // stop external objects deleting this, it must delete itself with RemoveRef()

		public:			
			Animation3D();
			Animation3D( const Animation3D *pOther );
			
			void AddRef() { m_iReferences++; }
			void RemoveRef();

			void LoadFromScene( aiAnimation *pAnim );
			void ScaleFrames( float x, float y, float z );

			Anim3DBone* GetAnimForBone( const char* name );
			float GetDuration() { return m_fTime; }
			const char* GetName() { return m_sName.GetStr(); }
			const uString& GetNameString() { return m_sName; }
	};

	class Skeleton3D
	{
			friend class agk;
			friend class cObject3D;

		protected:

			struct sSearchItem
			{
				const char* m_sName;
				UINT m_iIndex;
			};

			sSearchItem *m_pBoneSearchList;
			static int CompareSearchItem( const void* a, const void* b )
			{
				sSearchItem* s1 = (sSearchItem*)a;
				sSearchItem* s2 = (sSearchItem*)b;

				return strcmp( s1->m_sName, s2->m_sName );
			}

			UINT m_iNumBones;
			Bone3D **m_pBones;

			UINT m_iID;
			UINT m_bFlags;
			float m_fCurrTime;
			float m_fTweenTime;
			float m_fTotalTweenTime;
			float m_fSpeed;
			Animation3D *m_pCurrAnim;
			int m_iLoopCount;
			int m_iLoopTotal;
			float m_fLoopStartTime;
			float m_fLoopEndTime;

		public:
			Skeleton3D();
			Skeleton3D( const Skeleton3D *pOther );
			~Skeleton3D();

			void LoadFromScene( aiNode *pRootNode );
			Bone3D* GetBone( const char* name );
			Bone3D* GetBone( UINT index );
			int GetBoneIndex( const char* name );
			Bone3D ** GetBones() { return m_pBones; };
			int GetBoneCount() { return m_iNumBones; };

			void SetRoot( cNode *pNode );
			void SetBones( std::vector<Bone3D*> &boneList );
			void ScaleBones( float x, float y, float z );
			void FixBonePivots( const AGKVector &pos, const AGKQuaternion &rot );
			void ResetBoundingBoxes();
			void DiscoverBounds( cMesh *pMesh );

			void PlayAnimation( Animation3D *pAnim, float starttime, float endtime, int loop, float tweentime );
			void SetAnimationFrame( Animation3D *pAnim, float time, float tweentime );
			void StopAnimation();
			void ResetAnimation();
			void SetAnimationSpeed( float speed );
			int GetIsAnimating();
			int GetIsTweening();
			float GetAnimationTime();

			void Update( float time );
			void Draw();
			//void DrawBones();
			//void DrawBoneNames();
	};

	
}

#endif