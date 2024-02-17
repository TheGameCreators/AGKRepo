#ifndef _H_AGK_MESH_
#define _H_AGK_MESH_

// Common includes
#include "Common.h"
#include "../Collision/Box.h"
#include "AGKLights.h" 
#include "cObject3D.h"
#include "Skeleton3D.h"

#define AGK_MESH_HAS_BONES			0x0002
#define AGK_MESH_HAS_LIGHTMAP		0x0004
#define AGK_MESH_HAS_NORMALMAP		0x0008
#define AGK_MESH_VISIBLE			0x0010
#define AGK_MESH_COLLISION			0x0020
#define AGK_MESH_CAST_SHADOW		0x0040
#define AGK_MESH_UV_SCALES_UPDATED	0x0080

#define AGK_PRIMITIVE_TRIANGLES			0
#define AGK_PRIMITIVE_TRIANGLE_STRIP	1
#define AGK_PRIMITIVE_TRIANGLE_FAN		2
#define AGK_PRIMITIVE_LINE_LOOP			3
#define AGK_PRIMITIVE_LINES				4
#define AGK_PRIMITIVE_POINTS			5

#define AGK_MAX_VERTEX_LIGHTS	8 // if greater than 16 then modify the nums array to include additional characters
#define AGK_MAX_PIXEL_LIGHTS	4 // if greater than 16 then modify the nums array to include additional characters
#define AGK_MAX_MESH_LIGHTS		8 // greater of the two above values

struct aiMesh;

namespace AGK
{
	class Face;
	class Bone3D;
	class AGKShader;
	class AGKShaderConstantValue;

	struct cVertex
	{
		float x,y,z;
	};

	class AGKVertexLayout
	{
		public:
			AGKVertexLayout() {}
			~AGKVertexLayout() { if ( m_pOffsets ) delete [] m_pOffsets; }

			void Clone( const AGKVertexLayout *pOther )
			{
				m_iNumOffsets = pOther->m_iNumOffsets;
				m_iVertexSize = pOther->m_iVertexSize;
				m_iPrimitiveType = pOther->m_iPrimitiveType;
				if ( pOther->m_pOffsets ) 
				{
					m_pOffsets = new uint16_t[ m_iNumOffsets ];
					memcpy( m_pOffsets, pOther->m_pOffsets, sizeof(uint16_t) * m_iNumOffsets );
				}
			}

			void SetNumOffsets( uint32_t num ) 
			{ 
				if ( num == m_iNumOffsets ) return;
				if ( m_pOffsets ) delete [] m_pOffsets;
				m_iNumOffsets = num; 
				m_pOffsets = new uint16_t[ m_iNumOffsets ]; 
			}

			int Compare( AGKVertexLayout *pOther )
			{
				if ( m_iHash > pOther->m_iHash ) return 1;
				else if ( m_iHash < pOther->m_iHash ) return -1;
				else
				{
					for( uint32_t i = 0; i < m_iNumOffsets; i++ )
					{
						if ( m_pOffsets[ i ] > pOther->m_pOffsets[ i ] ) return 1;
						else if ( m_pOffsets[ i ] < pOther->m_pOffsets[ i ] ) return -1;
					}
					return 0;
				}
			}

			uint16_t *m_pOffsets = 0;
			union
			{
				struct
				{
					uint16_t m_iVertexSize; // size of an entire vertex in bytes
					uint8_t m_iNumOffsets;
					uint8_t m_iPrimitiveType;
				};
				uint32_t m_iHash = 0;
			};
	};

	class AGKVertexAttrib
	{
	public:
		void* m_pData = 0;
		char* m_szName = 0; // position, normal, texcoord, color, etc
		int16_t m_iOffset = -1; // offset of this element in the combined vertex buffer, in bytes, -1 if it is not in the combined buffer
		uint8_t m_iType = 0; // 0=float, 1=ubyte
		uint8_t m_iComponents = 4; // valid values: 1,2,3,4
		uint8_t m_iNormalize = 0; // when attributes are converted to float should they be normalised to the range 0.0-1.0
		
		AGKVertexAttrib() {}
		~AGKVertexAttrib()
		{
			if ( m_szName ) delete [] m_szName;
			ClearData();
		}

		void Copy( AGKVertexAttrib* other, uint32_t numVertices )
		{
			m_iType = other->m_iType;
			m_iComponents = other->m_iComponents;
			m_iOffset = other->m_iOffset;
			m_iNormalize = other->m_iNormalize;
			if ( other->m_szName )
			{
				m_szName = new char[ strlen(other->m_szName) + 1 ];
				strcpy( m_szName, other->m_szName );
			}
			m_pData = 0;
			if ( other->m_pData )
			{
				if ( m_iType == 0 )
				{
					m_pData = new float[ numVertices*m_iComponents ];
					memcpy( m_pData, other->m_pData, numVertices*m_iComponents*sizeof(float) );
				}
				else if ( m_iType == 1 )
				{
					m_pData = new unsigned char[ numVertices*4 ];
					memcpy( m_pData, other->m_pData, numVertices*m_iComponents );
				}
			}
		}
		
		void ClearData()
		{
			if ( m_pData )
			{
				if ( m_iType == 0 ) delete [] (float*)m_pData;
				else if ( m_iType == 1 ) delete [] (unsigned char*)m_pData;
			}
			m_pData = 0;
		}
	};

	class cMesh
	{
		protected:

			friend class AGO;
			friend class cObject3D;

			struct lightItem 
			{
				float dist;
				int index;
			};

			struct UVOffsetScale
			{
				float offsetU;
				float offsetV;
				float scaleU;
				float scaleV;
			};

			//bounding box
			Box m_BoundingBox;
			float m_fRadius;

			uint32_t m_iFlags = AGK_MESH_VISIBLE | AGK_MESH_COLLISION | AGK_MESH_CAST_SHADOW;

			uString m_sName;

			AGKDynamicArray<cImage*> m_shaderImages;
			cObject3D *m_pObject = 0; // determines OpenGL states depth buffer, blend modes, and transforms
			AGKShader *m_pShader = 0; // shader currently being used to draw the mesh, may be regenerated as the number of lights changes
			AGKShader *m_pOrigShader = 0; // shader originally assigned to the mesh before being modified for lighting
			AGKShader *m_pShadowShader = 0;

			// shader constants specific to this mesh
			AGKShaderConstantValue *m_pShaderVarNormalScale = 0;
			AGKShaderConstantArray *m_pShaderVarVSLightPos = 0;
			AGKShaderConstantArray *m_pShaderVarVSLightColor = 0;
			AGKShaderConstantArray *m_pShaderVarPSLightPos = 0;
			AGKShaderConstantArray *m_pShaderVarPSLightColor = 0;
			AGKUnorderedArray<AGKShaderConstantValue*> m_shaderConstants;

			// local lights
			uint32_t m_iNumVSLights = 0;
			uint32_t m_iNumPSLights = 0;
			AGKPointLight* m_pVSLights[ AGK_MAX_VERTEX_LIGHTS ];
			AGKPointLight* m_pPSLights[ AGK_MAX_PIXEL_LIGHTS ];

			cMesh *m_pSharedVertices = 0; // if non-zero then read the vertex data from m_pSharedVertices mesh instead

			// drawing array resources, may have multiple VBOs if mesh contains more than 65535 vertices
			//unsigned char m_iNumInternalVertices; // moved below
			void** m_pInternalVertices = 0;
			void* m_pInternalPipeline = 0;
			void* m_pInternalConstants = 0;
			void* m_pInternalShadowPipeline = 0;
			void* m_pInternalShadowConstants = 0;

			cHashedList<uint32_t> m_cOtherAttributeNames; // only for attributes that don't have a known name

			// vertex attributes containing the raw vertex data (not for drawing), this is used to create the draw arrays and the collision data.
			AGKVertexAttrib *m_pVertexAttribs = 0;
			//unsigned char m_iNumAttribs; // moved below
			//uint32_t m_iNumRawVertices; // moved below

			AGKVertexLayout m_vertexLayout; // only filled out once we have a shader
			AGKVertexLayout m_shadowVertexLayout;
			
			// raw indices, if any. Not for drawing, this is used to create the drawing arrays and collision data.
			unsigned int* m_pRawIndices = 0; // could be unsigned short or unsigned int
			uint32_t m_iNumRawIndices = 0;

			float m_fScaledBy = 1; // how much the model was scaled during loading
			AGKDynamicArray<UVOffsetScale*> m_uvOffsetScales;
			AGKDynamicArray<AGKShaderConstantValue*> m_shaderVarUVOffsetScales;

			float m_fNormalScaleU = 1;
			float m_fNormalScaleV = 1;

			// better packing
			uint32_t m_iNumRawVertices = 0;
			uint16_t m_iNumInternalVertices = 0;
			uint8_t m_iNumAttribs = 0;
			int8_t m_iPosAttrib = -1;
			int8_t m_iNormAttrib = -1;
			int8_t m_iUVAttrib = -1;
			int8_t m_iUV1Attrib = -1;
			int8_t m_iTangentAttrib = -1;
			int8_t m_iBiNormAttrib = -1;
			int8_t m_iColorAttrib = -1;
			int8_t m_iBoneIndicesAttrib = -1;
			int8_t m_iBoneWeightsAttrib = -1;
			uint8_t m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLES;

			void SetInternalVerticesCount( uint32_t num );
			void CreateInternalVertices( float *pVertices, uint32_t numVertices, uint32_t vertexStride, uint32_t *pIndices, uint32_t numIndices );
			void UpdateVertexLayout( AGKVertexLayout *pLayout, AGKShader *pShader );
			void UpdateUVOffsetScaleShaderVars();
			void SetAttribNameIndex( const char *szName, int8_t index );

		public:
			static AGKRenderState g_shadowRenderState;

			static int CompareLightItem( const void* a, const void* b );

			cMesh( cObject3D *pParent );
			cMesh( cObject3D *pParent, cMesh *pCopyFrom, int share );
			~cMesh();

			void ClearAttribs();
			void ClearRawVertexData(); // if the vertex data will never be modified we can throw the original arrays away.
			void DeleteInternalVertices();
			void ProcessVertexData();

			bool HasBones() const { return (m_iFlags & AGK_MESH_HAS_BONES) != 0; }
			bool HasValidBones() const;
			int GetNumBones() const { return m_pObject->GetSkeleton() ? m_pObject->GetSkeleton()->GetBoneCount() : 0; }
			bool HasNormals() const;
			bool HasTangents() const;
			bool HasBiNormals() const;
			bool HasVertColors() const;
			bool HasUVs() const;
			bool HasUV1s() const;
			bool HasUVStage( int stage ) const;
			int HasTexture0() const { return m_shaderImages.GetItem(0) ? 1 : 0; }
			int HasTexture1() const { return m_shaderImages.GetItem(1) ? 1 : 0; }
			int HasTextureStage(int stage) const  {return m_shaderImages.GetItem( stage ) ? 1 : 0; }
			int HasLightMap() const { return (m_shaderImages.GetItem(1) && (m_iFlags & AGK_MESH_HAS_LIGHTMAP)) ? 1 : 0; }
			int HasNormalMap() const { return (m_shaderImages.GetItem(2) && (m_iFlags & AGK_MESH_HAS_NORMALMAP)) ? 1 : 0; }
			int GetVisible() const { return (m_iFlags & AGK_MESH_VISIBLE) ? 1 : 0; }
			int GetCollision() const { return (m_iFlags & AGK_MESH_COLLISION) ? 1 : 0; }
			int DoesCastShadows() const { return (m_iFlags & AGK_MESH_CAST_SHADOW) ? 1 : 0; }
			int WantsLighting() const { return m_pObject->GetLightMode() ? 1 : 0; }
			int WantsFog() const { return m_pObject->GetFogMode() ? 1 : 0; }
			int WantsShadows() const;
			const uString* GetName() const { return &m_sName; }
			int InFrustumPlane( const AGKVector *n, float d ) { return m_BoundingBox.inFrustumPlane(n,d); }
			uint32_t GetInScreen();
			uint32_t GetInShadowFrustum();
			cImage* GetImage( uint32_t stage ) { return m_shaderImages.GetItem( stage ); }
			
			void CreateBox( float width, float height, float length );
			void CreateSphere( float diameter, int rows, int columns );
			void CreateCone( float height, float diameter, int segments );
			void CreateCylinder( float height, float diameter, int segments );
			void CreatePlane( float width, float height );
			void CreateQuad();
			void CreateCapsule( float diameter, int rows, int columns, float height, int axis );
			void CreateFromHeightMap( unsigned short *pValues, int totalSegsX, int totalSegsZ, int startX, int endX, int startZ, int endZ, float width, float height, float length );

			void CreateMesh( aiMesh *pMesh, float height, int scaleMode=0 ); // scaleMode: 0=scale mesh to specified height, 1=use height as a scale factor, i.e. height=2.0 means double the size
			void CreateFromObj( int lines, uString *sLines, float height, const char* szFilename );

			void GetVerticesFromMemblock( uint32_t &memSize, unsigned char **pMemData );
			void SetVerticesFromMemblock( uint32_t memSize, unsigned char *pMemData );

			void GetBoundingBoxForBone( int boneIndex, Bone3D *pBone, Box* pBounds );
			Face* GetFaceList( Face **pLast );
			float GetLoadScale() const { return m_fScaledBy; }

			void TranslateMesh( float x, float y, float z, int update=1 );
			void RotateMesh( float w, float x, float y, float z, int update=1 );
			void ScaleMesh( float x, float y, float z, int update=1 ); // permanent change

			void SetImage( cImage *pImage, uint32_t stage=0 );
			void SetUVOffset( uint32_t stage, float offsetU, float offsetV );
			void SetUVScale( uint32_t stage, float scaleU, float scaleV );
			void SetLightMap( cImage *pImage );
			void SetNormalMap( cImage *pImage );
			void SetNormalMapScale( float scaleU, float scaleV );
			void SetShader( AGKShader *pShader );
			void SetVisible(uint32_t mode) { if (mode > 0) m_iFlags |= AGK_MESH_VISIBLE; else m_iFlags &= ~AGK_MESH_VISIBLE; }
			void SetCollision(uint32_t mode) { if (mode > 0) m_iFlags |= AGK_MESH_COLLISION; else m_iFlags &= ~AGK_MESH_COLLISION; }
			void SetShadowCastMode(uint32_t mode) { if (mode > 0) m_iFlags |= AGK_MESH_CAST_SHADOW; else m_iFlags &= ~AGK_MESH_CAST_SHADOW; }

			float GetUVOffsetU( uint32_t stage ) const { UVOffsetScale *res = m_uvOffsetScales.GetItem( stage ); return res ? res->offsetU : 0; }
			float GetUVOffsetV( uint32_t stage ) const { UVOffsetScale *res = m_uvOffsetScales.GetItem( stage ); return res ? res->offsetV : 0; }
			float GetUVScaleU( uint32_t stage ) const { UVOffsetScale *res = m_uvOffsetScales.GetItem( stage ); return res ? res->scaleU : 1; }
			float GetUVScaleV( uint32_t stage ) const { UVOffsetScale *res = m_uvOffsetScales.GetItem( stage ); return res ? res->scaleV : 1; }

			float GetMinX() const { return m_BoundingBox.minbx(); }
			float GetMinY() const { return m_BoundingBox.minby(); }
			float GetMinZ() const { return m_BoundingBox.minbz(); }
			float GetMaxX() const { return m_BoundingBox.maxbx(); }
			float GetMaxY() const { return m_BoundingBox.maxby(); }
			float GetMaxZ() const { return m_BoundingBox.maxbz(); }
			
			AGKShader* GetShader() const { return m_pOrigShader; }
			cObject3D* GetObject() const { return m_pObject; }
			
			void CheckShader();
			void CheckLights();

			void Update();
			void Draw(); // sets up lights, but assumes higher level items like shader, textures, blend/depth/etc states have already been set up
			void DrawShadow();
	};
}

#endif