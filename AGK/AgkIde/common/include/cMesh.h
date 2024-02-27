#ifndef _H_AGK_MESH_
#define _H_AGK_MESH_

// Common includes
#include "Common.h"
#include "AGKShader.h"
#include "cMaterial.h"
#include "../Collision/Box.h"
#include "AGKLights.h" 

#define AGK_MESH_UINT_INDICES		0x0001
#define AGK_MESH_HAS_BONES			0x0002
#define AGK_MESH_HAS_LIGHTMAP		0x0004
#define AGK_MESH_HAS_NORMALMAP		0x0008
#define AGK_MESH_VISIBLE			0x0010
#define AGK_MESH_COLLISION			0x0020

struct aiMesh;

namespace AGK
{
	class Face;
	class Bone3D;

	struct cVertex
	{
		float x,y,z;
	};

	class cVertexAttrib
	{
	public:
		uString m_sName; // position, normal, texcoord, color, etc
		unsigned char m_iType; // 0=float, 1=ubyte
		char m_iShaderLoc; // max 16 on most cards
		unsigned char m_iComponents; // valid values: 1,2,3,4
		char m_iOffset; // offset of this element in the combined vertex buffer, in bytes, -1 if it is not in the combined buffer
		bool m_bNormalize; // when attributes are converted to float should they be normalised to the range 0.0-1.0
		void* m_pData;

		cVertexAttrib() 
		{ 
			m_iType = 0;
			m_iShaderLoc = -1;
			m_iComponents = 4;
			m_iOffset = -1;
			m_bNormalize = false;
			m_pData = 0;
		}

		cVertexAttrib( cVertexAttrib* other, UINT numVertices )
		{
			m_iType = other->m_iType;
			m_iShaderLoc = other->m_iShaderLoc;
			m_iComponents = other->m_iComponents;
			m_iOffset = other->m_iOffset;
			m_bNormalize = other->m_bNormalize;
			m_sName.SetStr( other->m_sName );
			m_pData = 0;
			if ( other->m_pData )
			{
				if ( m_iType == 0 )
				{
					m_pData = new float[ numVertices*m_iComponents ];
					for ( UINT i = 0; i < numVertices*m_iComponents; i++ ) ((float*)m_pData)[ i ] = ((float*)other->m_pData)[ i ];
				}
				else if ( m_iType == 1 )
				{
					m_pData = new unsigned char[ numVertices*4 ];
					for ( UINT i = 0; i < numVertices*m_iComponents; i++ ) ((unsigned char*)m_pData)[ i ] = ((unsigned char*)other->m_pData)[ i ];
				}
			}
		}

		~cVertexAttrib()
		{
			ClearData();
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

	// if a shader requires more attributes than the object provides, use these to fill the blanks with static data
	class cDummyAttrib
	{
	public:
		char m_iShaderLoc;
		unsigned char *m_pData;
		uString m_sName;
		cDummyAttrib *m_pNextAttrib;

		cDummyAttrib()
		{
			m_iShaderLoc = -1;
			m_pData = 0;
			m_pNextAttrib = 0;
		}

		~cDummyAttrib()
		{
			if ( m_pData ) delete [] m_pData;
		}
	};

	
	class cMesh
	{
		protected:

			friend class AGO;
			friend class cObject3D;
			friend class cMaterial;

			//bounding box
			Box m_BoundingBox;
			float m_fRadius;

			uString m_sName;

			cImage *m_pImages[ AGK_MAX_TEXTURES ]; // also stored in m_pMaterial but keep a copy so we can find materials that match
			cMaterial *m_pMaterial; // material that this mesh is assigned to, materials are mainly used for grouping similar meshes together
			cObject3D *m_pObject; // determines OpenGL states depth buffer, blend modes, and transforms
			AGKShader *m_pShader; // shader currently being used to draw the mesh, may be regenerated as the number of lights changes
			AGKShader *m_pOrigShader; // shader originally assigned to the mesh before being modified for lighting

			// local lights
			int m_iNumVSLights;
			int m_iNumPSLights;
			AGKPointLight* m_pVSLights[ AGK_MAX_VERTEX_LIGHTS ];
			AGKPointLight* m_pPSLights[ AGK_MAX_PIXEL_LIGHTS ];

			cMesh *m_pSharedVertices; // if non-zero then read the vertex data from m_pSharedVertices mesh instead

			// drawing arrays, may have multiple VBOs if mesh contains more than 65535 vertices
			//unsigned char m_iNumArrays; // moved below
			//unsigned char m_iVertexStride; // moved below
			UINT *m_iNumVertices;
			float **m_ppVBOVertexData;
			UINT *m_iNumIndices;
			unsigned short **m_ppIndices; // could be unsigned shorts or unsigned ints
			UINT *m_iVBOVertices;
			UINT *m_iVBOIndices;
			UINT m_iFlags;

			// vertex attributes containing the raw vertex data (not for drawing), this is used to create the draw arrays and the collision data.
			//unsigned char m_iPosAttrib; // index for the vertex positions (if any) for quick access.  // moved below
			cVertexAttrib **m_pVertexAttribs;
			//unsigned char m_iNumAttribs; // moved below
			cDummyAttrib *m_pDummyAttribs;
			UINT m_iNumAttribVertices;

			// raw indices, if any. Not for drawing, this is used to create the drawing arrays and collision data.
			unsigned int* m_pRawIndices; // could be unsigned short or unsigned int
			UINT m_iNumRawIndices;

			float m_fScaledBy; // how much the model was scaled during loading
			float m_fUVOffsetU[ AGK_MAX_TEXTURES ];
			float m_fUVOffsetV[ AGK_MAX_TEXTURES ];
			float m_fUVScaleU[ AGK_MAX_TEXTURES ];
			float m_fUVScaleV[ AGK_MAX_TEXTURES ];

			float m_fNormalScaleU;
			float m_fNormalScaleV;

			// better packing
			unsigned char m_iNumArrays;
			unsigned char m_iVertexStride;
			unsigned char m_iNumAttribs;
			char m_iPosAttrib;
			char m_iNormAttrib;
			char m_iUVAttrib;
			char m_iUV1Attrib;
			char m_iTangentAttrib;
			char m_iBiNormAttrib;
			char m_iColorAttrib;
			char m_iPrimitiveType;

			void PlatformGenBuffers();
			void CreateVBOLists( float *pVertices, UINT numVertices, UINT vertexStride, UINT *pIndices, UINT numIndices, int keepBuffers=0 );
			void ProcessVertexData( int keepBuffers=0 );

			void PlatformDraw( int shadow, int alphamask=0 );

		public:
			cMesh( cObject3D *pParent );
			cMesh( cObject3D *pParent, cMesh *pCopyFrom, int share );
			~cMesh();

			void ClearAttribs();
			void ClearRawVertexData(); // if the vertex data will never be modified we can throw the original arrays away.
			void CreateDummyAttributesForShader( AGKShader *pShader );
			void DeleteGLData();
			void ReloadGLData();
			void DeleteLocalVBOData();

			bool HasBones() const { return (m_iFlags & AGK_MESH_HAS_BONES) != 0; }
			bool HasValidBones() const;
			int GetNumBones() const;
			bool HasNormals() const;
			bool HasTangents() const;
			bool HasBiNormals() const;
			bool HasVertColors() const;
			bool HasUVs() const;
			bool HasUV1s() const;
			bool HasUVStage( int stage ) const;
			int HasTexture0() const { return m_pImages[0] ? 1 : 0; }
			int HasTexture1() const { return m_pImages[1] ? 1 : 0; }
			int HasTextureStage(int stage) const;
			int HasLightMap() const { return (m_pImages[1] && (m_iFlags & AGK_MESH_HAS_LIGHTMAP)) ? 1 : 0; }
			int HasNormalMap() const { return (m_pImages[2] && (m_iFlags & AGK_MESH_HAS_NORMALMAP)) ? 1 : 0; }
			int GetVisible() const { return (m_iFlags & AGK_MESH_VISIBLE) ? 1 : 0; }
			int GetCollision() const { return (m_iFlags & AGK_MESH_COLLISION) ? 1 : 0; }
			int WantsLighting() const;
			int WantsFog() const;
			int WantsShadows() const;
			const uString* GetName() const { return &m_sName; }
			int InFrustumPlane( const AGKVector *n, float d ) { return m_BoundingBox.inFrustumPlane(n,d); }
			UINT GetInScreen();
			UINT GetInShadowFrustum();
			cImage* GetImage( UINT stage );
			
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

			void GetVerticesFromMemblock( UINT &memSize, unsigned char **pMemData );
			void SetVerticesFromMemblock( UINT memSize, unsigned char *pMemData );

			void GetBoundingBoxForBone( int boneIndex, Bone3D *pBone, Box* pBounds );
			Face* GetFaceList( Face **pLast );
			float GetLoadScale() const { return m_fScaledBy; }

			void TranslateMesh( float x, float y, float z, int update=1 );
			void RotateMesh( float w, float x, float y, float z, int update=1 );
			void ScaleMesh( float x, float y, float z, int update=1 ); // permanent change

			void SetImage( cImage *pImage, UINT stage=0 );
			void SetUVOffset( UINT stage, float offsetU, float offsetV );
			void SetUVScale( UINT stage, float scaleU, float scaleV );
			void SetLightMap( cImage *pImage );
			void SetNormalMap( cImage *pImage );
			void SetNormalMapScale( float scaleU, float scaleV );
			void SetShader( AGKShader *pShader );
			void SetLights( int numVSLights, AGKPointLight **pVSLights, int numPSLights, AGKPointLight **pPSLights );
			void SetVisible(UINT mode);
			void SetCollision(UINT mode);

			float GetUVOffsetU( UINT stage ) const { return stage >= AGK_MAX_TEXTURES ? 0 : m_fUVOffsetU[ stage ]; }
			float GetUVOffsetV( UINT stage ) const { return stage >= AGK_MAX_TEXTURES ? 0 : m_fUVOffsetV[ stage ]; }
			float GetUVScaleU( UINT stage ) const { return stage >= AGK_MAX_TEXTURES ? 1 : m_fUVScaleU[ stage ]; }
			float GetUVScaleV( UINT stage ) const { return stage >= AGK_MAX_TEXTURES ? 1 : m_fUVScaleV[ stage ]; }

			float GetMinX() const { return m_BoundingBox.minbx(); }
			float GetMinY() const { return m_BoundingBox.minby(); }
			float GetMinZ() const { return m_BoundingBox.minbz(); }
			float GetMaxX() const { return m_BoundingBox.maxbx(); }
			float GetMaxY() const { return m_BoundingBox.maxby(); }
			float GetMaxZ() const { return m_BoundingBox.maxbz(); }
			
			AGKShader* GetShader() const { return m_pOrigShader; }
			cObject3D* GetObject() const { return m_pObject; }
			void CheckShader();

			void Update();
			void Draw(); // sets up lights, but assumes higher level items like shader, textures, blend/depth/etc states have already been set up
			void DrawShadow();
	};
}

#endif