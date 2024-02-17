#ifndef _H_OBJECT3D_
#define _H_OBJECT3D_

// Common includes
#include "Common.h"
#include "AGKShader.h"
#include "cNode.h"

#define AGK_OBJECT_CUSTOM_SHADER					0x00000001
#define AGK_OBJECT_USE_LIGHTS						0x00000002
#define AGK_OBJECT_DRAW_BACKFACES					0x00000004
#define AGK_OBJECT_Z_WRITE							0x00000008
#define AGK_OBJECT_IDENTITY_TRANSFORM				0x00000010
//#define AGK_OBJECT_UNIFORM_SCALE					0x00000020
#define AGK_OBJECT_VISIBLE							0x00000040
#define AGK_OBJECT_TRANSCHANGED						0x00000080
#define AGK_OBJECT_MANAGED							0x00000100
#define AGK_OBJECT_IS_QUAD							0x00000200
#define AGK_OBJECT_NO_FRUSTUM_CULLING				0x00000400
#define AGK_OBJECT_COLLISION_ON						0x00000800
#define AGK_OBJECT_ALPHA_MASK						0x00001000
#define AGK_OBJECT_USE_FOG							0x00002000
#define AGK_OBJECT_IS_HEIGHTMAP						0x00004000
#define AGK_OBJECT_CAST_SHADOWS						0x00008000
#define AGK_OBJECT_RECV_SHADOWS						0x00010000

#define AGK_OBJECT_SHADER_ARRAY_NEEDS_UPDATE		0x00020000
#define AGK_OBJECT_SHADER_WORLD_NEEDS_UPDATE		0x00040000
#define AGK_OBJECT_SHADER_DIFFUSE_NEEDS_UPDATE		0x00080000
#define AGK_OBJECT_SHADER_EMISSIVE_NEEDS_UPDATE		0x00100000
#define AGK_OBJECT_SHADER_WORLD_NORM_NEEDS_UPDATE	0x00200000
#define AGK_OBJECT_SHADER_SHADOW_ARRAY_NEEDS_UPDATE	0x00400000

#define AGK_OBJECT_MAX_BONES 200
#define AGK_OBJECT_MAX_TEXTURES 30
//#define AGK_OBJECT_MAX_BONES_STR "100"

// Bullet physics forward declarations
class btRigidBody;

// Namespace
namespace AGK
{
	class cImage;
	class Skeleton3D;
	class Animation3D;
	class CollisionObject;
	class CollisionResults;
	class cMesh;
	class cObjectMgr;

	class cObject3D : public cNode
	{
		protected:

			friend class agk;
			friend class cObjectMgr;
			friend class AGO;
			friend class cMesh;

			// global array of all objects
			cObject3D *m_pNextObject = 0;
			cObject3D *m_pPrevObject = 0;
			static cObject3D *g_pAllObjects;
			
			// Bone Animation
			Skeleton3D *m_pSkeleton = 0;
			Animation3D **m_pAnims = 0;
			uint32_t m_iNumAnims = 0;
			uint32_t m_iNumChildren = 0;
			uint32_t *m_iLoadedChildren = 0; // list of object IDs that were loaded from the same model file

			// 3D Physics
			btRigidBody* m_pRigidBody = 0;

			uString m_sName;
			uint32_t m_iID = 0; // unique ID for tier 1 only

			uint32_t m_iUsedTextures = 0;
			uString m_sTextures[AGK_OBJECT_MAX_TEXTURES];
			
			// meshes that hold raw vertices and VBO vertices
			uint32_t m_iNumMeshes = 0;
			cMesh **m_pMeshes = 0;
			
			// common shader values for this object
			AGKShaderConstantValue *m_pShaderWorldMatrix = 0;
			AGKShaderConstantValue *m_pShaderWorldNormMatrix = 0;
			AGKShaderConstantArray *m_pShaderBoneQuats1 = 0;
			AGKShaderConstantArray *m_pShaderBoneQuats2 = 0;
			AGKShaderConstantValue *m_pShaderFogColor1 = 0;
			AGKShaderConstantValue *m_pShaderFogColor2 = 0;
			AGKShaderConstantValue *m_pShaderDiffuse = 0;
			AGKShaderConstantValue *m_pShaderEmissive = 0;

			// shader values set by the user for this object
			AGKShaderConstantsByName m_cShaderConstantsByName;
			AGKUnorderedArray<AGKShaderConstantValue*> m_cShaderConstantArray;
			AGKUnorderedArray<AGKShaderConstantValue*> m_cShaderShadowConstantArray;

			// if object is instanced then it shares a collision object
			cObject3D *m_pSharedColObject = 0;
			
			// local transform now stored in cNode
			
			// attributes
			uint32_t m_iObjFlags = 0;
			AGKVector m_Color;
			float m_fAlpha = 1;
			AGKVector m_ColorEmissive;

			// collision
			CollisionObject *m_pColObject = 0;

			// for quick terrain height look up
			unsigned short *m_pHeightMap = 0;
			int m_iHeightMapPixelsX = 0;
			int m_iHeightMapPixelsZ = 0;
			float m_fHeightMapSizeX = 0;
			float m_fHeightMapSizeY = 0;
			float m_fHeightMapSizeZ = 0;

			AGKRenderState m_renderState;

			uint8_t m_iTransparency = 0;
			uint8_t m_iUserBlendFunc = (AGK_BLEND_ONE << 4) | AGK_BLEND_ZERO; // src in high 4 bits, dst in low 4 bits
			
			void CreateCollisionData();

		public:
			static void ReloadAll();

			cObject3D();
			cObject3D( cObject3D *pOther, int share );
			~cObject3D();

			void NeedsUpdateCallback() { m_iObjFlags |= AGK_OBJECT_SHADER_WORLD_NEEDS_UPDATE | AGK_OBJECT_SHADER_WORLD_NORM_NEEDS_UPDATE; }
			
			bool IsManaged() { return (m_iObjFlags & AGK_OBJECT_MANAGED) != 0; }
			uint32_t GetID() { return m_iID; }

			void DeleteMeshes();
			void DeleteChildren();
			void DeleteTree();
			void DeleteAnimations();

			void CreateBox( float width, float height, float length );
			void CreateSphere( float diameter, int rows, int columns );
			void CreateCapsule( float diameter, float height, int axis );
			void CreateCone( float height, float diameter, int segments );
			void CreateCylinder( float height, float diameter, int segments );
			void CreatePlane( float width, float height );
			void CreateQuad();
			void CreateFromHeightMap( cImage *pImage, float width, float height, float length, int smoothing, int split );
			void CreateFromHeightMap( const char *szHeightMap, float width, float height, float length, int smoothing, int split );
			void CreateFromRawHeightMap(const char *szHeightMap, float width, float height, float length, int smoothing, int split, int rawWidth, int rawHeight );
			void CreateFromHeightMapFromData( const unsigned short *szHeightMap, int imgWidth, int imgHeight, float width, float height, float length, int smoothing, int split );
			void CreateFromMeshes( int numMeshes, cMesh **pMeshes );
			void AddMesh( cMesh *pMesh, int updateCollision=1 );

			uint32_t GetNumMeshes() { return m_iNumMeshes; }
			cMesh* GetMesh( uint32_t index );

			void LoadObject( const char *szFilename, int withChildren=0, float height=0 );
			void LoadOBJ( const char *szFilename, float height=0 );
			void LoadAGOAscii( const char *szFilename, float height=0 );

			void SaveObject( const char *szFilename );

			void SetPosition( float x, float y, float z );
			void SetRotationQuat( const AGKQuaternion &rot ) { SetNodeRotation( rot.w, rot.x, rot.y, rot.z ); }
			void SetRotationEuler( float angX, float angY, float angZ ) { AGKQuaternion q; q.MakeFromEulerYXZ( angX, angY, angZ ); SetNodeRotation( q.w, q.x, q.y, q.z ); }
			void SetScale( float x, float y, float z ) { SetNodeScale( x,y,z ); }
			void SetScalePermanent( float x, float y, float z );
			void FixPivot();

			void SetShader( AGKShader *pShader );
			void SetImage( cImage *pImage, uint32_t stage=0 );
			void SetLightMap( cImage *pImage );
			void SetNormalMap( cImage *pImage );
			void SetNormalMapScale( float scaleU, float scaleV );
			void SetColor( int red, int green, int blue, int alpha );
			void SetRed( int iRed );
			void SetGreen( int iGreen );
			void SetBlue( int iBlue );
			void SetAlpha( int iAlpha );
			void SetColorEmissive( int red, int green, int blue );
			void SetLightMode( uint32_t mode );
			void SetScreenCulling( uint32_t mode );
			void SetUVOffset( uint32_t stage, float offsetU, float offsetV );
			void SetUVScale( uint32_t stage, float scaleU, float scaleV );

			void SetFogMode( uint32_t mode ) { if ( mode > 0 ) m_iObjFlags |= AGK_OBJECT_USE_FOG; else m_iObjFlags &= ~AGK_OBJECT_USE_FOG; }
			void SetShadowCastMode( uint32_t mode ) { if ( mode > 0 ) m_iObjFlags |= AGK_OBJECT_CAST_SHADOWS; else m_iObjFlags &= ~AGK_OBJECT_CAST_SHADOWS; }
			void SetShadowReceiveMode( uint32_t mode ) { if ( mode > 0 ) m_iObjFlags |= AGK_OBJECT_RECV_SHADOWS; else m_iObjFlags &= ~AGK_OBJECT_RECV_SHADOWS; }

			void SetDepthEnable( uint32_t mode ) { m_renderState.SetDepthEnable( mode ); }
			void SetDepthReadMode( uint32_t mode ) { m_renderState.SetDepthFunc( mode ); }
			void SetDepthWrite( uint32_t mode ) { m_renderState.SetDepthWriteEnable( mode ); }
			void SetDepthBias( float bias ) { m_renderState.SetDepthBias( bias ); }
			void SetDepthRange( float zNear, float zFar ) { m_renderState.SetDepthRange( zNear, zFar ); }
			void SetTransparency( uint32_t mode );
			void SetBlendModes( uint32_t src, uint32_t dst );
			void SetAlphaMask( uint32_t mode );
			void SetCullMode( uint32_t mode ) { m_renderState.SetCullMode( mode ); }
			void SetVisible( uint32_t mode );
			void SetOverrideScissor( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) { m_renderState.OverrideScissor( 1 ); m_renderState.SetScissor( x, y, width, height ); }

			void UpdateCollisionData();

			// get
			uint32_t GetDepthReadMode() const { return m_renderState.GetDepthFunc(); }
			uint32_t GetDepthWrite() const { return m_renderState.GetDepthWriteEnable(); }
			float GetDepthBias() const { return m_renderState.GetDepthBias(); }
			uint32_t GetTransparency() const { return m_iTransparency; }
			uint32_t GetBlendModeSource() const { return m_renderState.GetBlendColorSrc(); }
			uint32_t GetBlendModeDest() const { return m_renderState.GetBlendColorDst(); }
			uint32_t GetCullMode() const { return m_renderState.GetCullMode(); }
			uint32_t GetVisible() const { return (m_iObjFlags & AGK_OBJECT_VISIBLE) ? 1 : 0; }
			uint32_t GetInScreen();
			bool GetTransparencyChanged();
			bool HasAlphaMask() const { return (m_iObjFlags & AGK_OBJECT_ALPHA_MASK) ? 1 : 0; }
			bool CheckTransparencyChanged() const { return (m_iObjFlags & AGK_OBJECT_TRANSCHANGED) != 0; };
			int GetLightMode() const { return (m_iObjFlags & AGK_OBJECT_USE_LIGHTS) ? 1 : 0; }
			int GetFogMode() const { return (m_iObjFlags & AGK_OBJECT_USE_FOG) ? 1 : 0; }
			int DoesCastShadows() const { return (m_iObjFlags & AGK_OBJECT_CAST_SHADOWS) ? 1 : 0; }
			int DoesReceiveShadows() const { return (m_iObjFlags & AGK_OBJECT_RECV_SHADOWS) ? 1 : 0; }
			int DoesCullToScreen() const { return (m_iObjFlags & AGK_OBJECT_NO_FRUSTUM_CULLING) ? 0 : 1; }
			int GetColorRed() const { return agkfloattoint(m_Color.x*255); }
			int GetColorGreen() const { return agkfloattoint(m_Color.y*255); }
			int GetColorBlue() const { return agkfloattoint(m_Color.z*255); }
			int GetAlpha() const { return agkfloattoint(m_fAlpha*255); }
			const AGKRenderState* GetRenderState() const { return &m_renderState; }
			int IsQuad() const { return (m_iObjFlags & AGK_OBJECT_IS_QUAD) ? 1 : 0; }

			const char* GetName() { return m_sName.GetStr(); }
			const char* GetTexture(int index) { if (index >= 0 && index < AGK_OBJECT_MAX_TEXTURES) return m_sTextures[index].GetStr(); else return(""); }
			uint32_t GetNumTextures() { return m_iUsedTextures; }

			float GetMinX() const;
			float GetMinY() const;
			float GetMinZ() const;
			float GetMaxX() const;
			float GetMaxY() const;
			float GetMaxZ() const;

			Skeleton3D* GetSkeleton() { return m_pSkeleton; }

			void SetShaderConstantByName( const char *name, float v1, float v2, float v3, float v4 );
			void SetShaderConstantArrayByName( const char *name, uint32_t index, uint32_t numValues, float v1, float v2, float v3, float v4 );
			void SetShaderConstantDefault( const char *name );
			
			// collision
			void SetCollisionMode( int mode );

			bool RayCast( float oldx, float oldy, float oldz, float x, float y, float z, CollisionResults *res );
			bool RayCast( const AGKVector &p, const AGKVector &v, const AGKVector &vn, CollisionResults *res );

			bool SphereCast( float oldx, float oldy, float oldz, float x, float y, float z, float rRadius, CollisionResults* cRes);
			bool SphereCast( const AGKVector &p, const AGKVector &v, const AGKVector &vn, float rRadius, CollisionResults* cRes);

			bool SphereSlide( float oldx, float oldy, float oldz, float x, float y, float z, float rRadius, CollisionResults* cRes);
			bool SphereSlide( const AGKVector &p, const AGKVector &v, const AGKVector &vn, float rRadius, CollisionResults* cRes);

			//bool Intersects( cObject3D* obj2 );

			// height map look up
			float GetHeightMapHeight( float x, float z );

			AGKUnorderedArray<AGKShaderConstantValue*>* GetShaderConstants();
			AGKUnorderedArray<AGKShaderConstantValue*>* GetShaderShadowConstants();
			
			void Update( float time );
			void Draw(); // draw all meshes
			void DrawShadow(); // drawing to a shadow map
	};
}

#endif