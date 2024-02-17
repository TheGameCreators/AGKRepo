// Image Header
#ifndef _H_AGKSHADER_
#define _H_AGKSHADER_

// Common includes
#include "Common.h"
#include "3DMath.h"
#include "uString.h"
#include "AGKArrays.h"
#include "cHashedList.h"

#ifdef GetUserName
	#undef GetUserName
#endif

#define AGK_SHADER_WORLD_CHANGED		0x00000001
#define AGK_SHADER_VIEW_CHANGED			0x00000002
#define AGK_SHADER_PROJ_CHANGED			0x00000004
#define AGK_SHADER_WORLDNORMAL_CHANGED	0x00000008
#define AGK_SHADER_ORTHO_CHANGED		0x00000010
#define AGK_SHADER_RELOAD_UNIFORMS		0x00000020
//#define AGK_SHADER_CAMERA_CHANGED		0x00000040
#define AGK_SHADER_PER_SPRITE_UNIFORM	0x00000080
#define AGK_SHADER_ATTRIBUTES_CHANGED	0x00000100
#define AGK_SHADER_IS_CUSTOM			0x00000200 // is a user custom shader or AGK generated shader
#define AGK_SHADER_USES_FOG				0x00000400
#define AGK_SHADER_USES_PS_LIGHTING		0x00000800 // shader needs, or is currently using, pixel lighting
#define AGK_SHADER_USES_VS_LIGHTING		0x00001000 // shader needs, or is currently using, vertex lighting
#define AGK_SHADER_IS_DEFAULT			0x00002000 // a core shader that should never be deleted
#define AGK_SHADER_SHADOW_PROJ_CHANGED	0x00004000
#define AGK_SHADER_RELOADING			0x00008000
#define AGK_SHADER_DELETING				0x00010000
#define AGK_SHADER_VALID				0x00020000

#define AGK_SHADER_MAX_ATTRIBUTES 16 

// vertex attributes
#define AGK_SHADER_ATTRIB_NAME_UNKNOWN		0
#define AGK_SHADER_ATTRIB_NAME_POS			1
#define AGK_SHADER_ATTRIB_NAME_NORM			2
#define AGK_SHADER_ATTRIB_NAME_UV0			3
#define AGK_SHADER_ATTRIB_NAME_COLOR		4
#define AGK_SHADER_ATTRIB_NAME_BONE_INDEX	5
#define AGK_SHADER_ATTRIB_NAME_BONE_WEIGHT	6
#define AGK_SHADER_ATTRIB_NAME_TANGENT		7
#define AGK_SHADER_ATTRIB_NAME_BINORMAL		8
#define AGK_SHADER_ATTRIB_NAME_UV1			9

// constant types
#define AGK_SHADER_CONSTANT_TYPE_UNKNOWN	0
#define AGK_SHADER_CONSTANT_TYPE_FLOAT		1	// must be 1
#define AGK_SHADER_CONSTANT_TYPE_VEC2		2	// must be 2
#define AGK_SHADER_CONSTANT_TYPE_VEC3		3	// must be 3
#define AGK_SHADER_CONSTANT_TYPE_VEC4		4	// must be 4
#define AGK_SHADER_CONSTANT_TYPE_MAT2		5
#define AGK_SHADER_CONSTANT_TYPE_MAT3		6
#define AGK_SHADER_CONSTANT_TYPE_MAT4		7
#define AGK_SHADER_CONSTANT_TYPE_STRUCT		8

#define AGK_SHADER_CONSTANT_NAME_TYPE_KNOWN		0
#define AGK_SHADER_CONSTANT_NAME_TYPE_USER		1
#define AGK_SHADER_CONSTANT_NAME_TYPE_BINDING	2

// constant known names
enum AGKShaderConstantName
{
	AGK_SHADER_CONSTANT_NAME_UNKNOWN = 0,
	
	AGK_SHADER_CONSTANT_NAME_OBJ_BONES1,		// agk_bonequats1
	AGK_SHADER_CONSTANT_NAME_OBJ_BONES2,		// agk_bonequats2
	AGK_SHADER_CONSTANT_NAME_MESH_DIFFUSE,		// agk_MeshDiffuse
	AGK_SHADER_CONSTANT_NAME_MESH_EMISSIVE,		// agk_MeshEmissive
	AGK_SHADER_CONSTANT_NAME_FOG_COLOR1,		// fogColor1
	AGK_SHADER_CONSTANT_NAME_FOG_COLOR2,		// fogColor2
	AGK_SHADER_CONSTANT_NAME_NORMAL_SCALE,		// agk_NormalScale
	AGK_SHADER_CONSTANT_NAME_CAMERA_POS,		// agk_CameraPos
	AGK_SHADER_CONSTANT_NAME_SPRITE_POS,		// agk_spritepos
	AGK_SHADER_CONSTANT_NAME_SPRITE_SIZE,		// agk_spritesize
	
	// matrices
	AGK_SHADER_CONSTANT_NAME_WORLD_MAT,			// agk_World
	AGK_SHADER_CONSTANT_NAME_WORLD_NORMAL,		// agk_WorldNormal
	AGK_SHADER_CONSTANT_NAME_VIEW,				// agk_View
	AGK_SHADER_CONSTANT_NAME_PROJ,				// agk_Proj
	AGK_SHADER_CONSTANT_NAME_VIEW_PROJ,			// agk_ViewProj

	AGK_SHADER_CONSTANT_NAME_WORLD_VIEW_PROJ,	// agk_WorldViewProj
	AGK_SHADER_CONSTANT_NAME_SHADOW_WORLD_PROJ,	// agk_ShadowWorldProj

	// lights
	AGK_SHADER_CONSTANT_NAME_VS_LIGHT_POS,		// agk_VSLightPos[]
	AGK_SHADER_CONSTANT_NAME_VS_LIGHT_COLOR,	// agk_VSLightColor[]
	AGK_SHADER_CONSTANT_NAME_PS_LIGHT_POS,		// agk_PSLightPos[]
	AGK_SHADER_CONSTANT_NAME_PS_LIGHT_COLOR,	// agk_PSLightColor[]
	
	// must be consecutive values
	AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_0,		// uvBounds0
	AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_1,		// uvBounds1
	AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_2,		// uvBounds2
	AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_3,		// uvBounds3
	AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_4,		// uvBounds4
	AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_5,		// uvBounds5
	AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_6,		// uvBounds6
	AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_7,		// uvBounds7

	// must be consecutive
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_0,		// textureBounds0
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_1,		// textureBounds1
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_2,		// textureBounds2
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_3,		// textureBounds3
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_4,		// textureBounds4
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_5,		// textureBounds5
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_6,		// textureBounds6
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_7,		// textureBounds7
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_8,		// textureBounds8
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_9,		// textureBounds9
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_10,		// textureBounds10
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_11,		// textureBounds11
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_12,		// textureBounds12
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_13,		// textureBounds13
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_14,		// textureBounds14
	AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_15,		// textureBounds15
		
	AGK_SHADER_CONSTANT_NAME_MAX_VALUE
};

// Namespace
namespace AGK
{
	class OpenGL2Renderer;
	class VulkanRenderer;
	class cCamera;
	class cMesh;

	struct AGKShaderAttrib
	{
		char *m_szName = 0; // position, normal, texcoord, color, etc
		uint8_t m_iKnownName = AGK_SHADER_ATTRIB_NAME_UNKNOWN;
		int8_t m_iLocation = -1;
		uint8_t m_iComponents = 4; // valid values: 1,2,3,4. Matrices are not supported

		AGKShaderAttrib() { }
		~AGKShaderAttrib() { if ( m_szName ) delete [] m_szName; }
	};

	struct AGKShaderUniform
	{
		char *m_szName = 0;
		int16_t m_iKnownName = AGK_SHADER_CONSTANT_NAME_UNKNOWN;
		short m_iLocation = -1;
		uint8_t m_iType = AGK_SHADER_CONSTANT_TYPE_UNKNOWN;

		AGKShaderUniform() { }
		~AGKShaderUniform() { if ( m_szName ) delete [] m_szName; }
	};
	
	class AGKShaderConstantValue
	{
		protected:
			static uint32_t g_iGlobalChangedVersion;

			union {
				char* m_szUniformName = 0; // if using a non-AGK known constant name
				uint32_t m_iKnownName; // if using an AGK known shader constant
				uint32_t m_iBinding; // if using set and binding location directly
			};
			float* m_pValues = 0;
			uint32_t m_iChangeVersion = 0; // increments on every value change
			uint16_t m_iElementSize = 0; // in floats (or other 4 byte type)
			uint8_t m_iNameType = 0; // name type: 0=m_iKnownName, 1=m_szUniformName, 2=m_iBinding
			uint8_t m_iDataType = AGK_SHADER_CONSTANT_TYPE_UNKNOWN; // [0-6]=data type, [7]=isArray
			
		public:
			uint32_t GetDataType() { return m_iDataType & 0x7F; } // data type = AGK_SHADER_CONSTANT_TYPE_*
			bool IsArray() { return (m_iDataType & 0x80) != 0; }
			uint32_t GetChangeVersion() { return m_iChangeVersion; }
			
			uint32_t GetNameType() { return m_iNameType; }
			const char* GetName() { return m_szUniformName; }
			uint32_t GetKnownName() { return m_iKnownName; }
			uint32_t GetBindingName() { return m_iBinding; }

			float* GetValues() { return m_pValues; }

			void SetDataType( uint32_t dataType ) { m_iDataType = dataType | (m_iDataType & 0x80); }
			void SetIsArray( uint32_t isArray ) { m_iDataType = (isArray << 7) | (m_iDataType & 0x7F); }
			
			AGKShaderConstantValue() { m_iChangeVersion = g_iGlobalChangedVersion; } // make sure new objects always look newer than deleted objects
			virtual ~AGKShaderConstantValue() { if ( m_pValues ) delete [] m_pValues; if ( m_iChangeVersion >= g_iGlobalChangedVersion ) g_iGlobalChangedVersion = m_iChangeVersion + 1; }

			// structSize is only used if type is AGK_UNIFORM_STRUCT, structSize is in floats (or other 4 byte type)
			void Initialize( uint32_t type, uint32_t structSize ) 
			{
				m_iDataType = type;
				switch( type )
				{
					case AGK_SHADER_CONSTANT_TYPE_FLOAT: m_iElementSize = 1; break;
					case AGK_SHADER_CONSTANT_TYPE_VEC2: m_iElementSize = 2; break;
					case AGK_SHADER_CONSTANT_TYPE_VEC3: m_iElementSize = 3; break;
					case AGK_SHADER_CONSTANT_TYPE_VEC4: m_iElementSize = 4; break;
					case AGK_SHADER_CONSTANT_TYPE_MAT2: m_iElementSize = 4; break;
					case AGK_SHADER_CONSTANT_TYPE_MAT3: m_iElementSize = 9; break;
					case AGK_SHADER_CONSTANT_TYPE_MAT4: m_iElementSize = 16; break;
					case AGK_SHADER_CONSTANT_TYPE_STRUCT: m_iElementSize = structSize; break;
					default: return;
				}
				m_pValues = new float[m_iElementSize]; 
				memset( m_pValues, 0, m_iElementSize * sizeof(float) );
			}

			AGKShaderConstantValue* Clone() const
			{
				AGKShaderConstantValue* newConstant = new AGKShaderConstantValue();
				newConstant->m_iChangeVersion = m_iChangeVersion;
				newConstant->m_iElementSize = m_iElementSize;
				newConstant->m_iDataType = m_iDataType;
				newConstant->m_iNameType = m_iNameType;
				newConstant->m_szUniformName = m_szUniformName; // also copies known name or binding
				
				if ( newConstant->m_iNameType == AGK_SHADER_CONSTANT_NAME_TYPE_USER && newConstant->m_szUniformName ) 
				{
					newConstant->m_szUniformName = new char[ strlen(m_szUniformName)+1 ];
					strcpy( newConstant->m_szUniformName, m_szUniformName );
				}

				if ( m_pValues )
				{
					newConstant->m_pValues = new float[ m_iElementSize ]; 
					memcpy( newConstant->m_pValues, m_pValues, m_iElementSize * sizeof(float) );
				}

				return newConstant;
			}

			void SetName( const char* name )
			{
				if ( m_iNameType == AGK_SHADER_CONSTANT_NAME_TYPE_USER && m_szUniformName ) delete [] m_szUniformName;
				m_iNameType = AGK_SHADER_CONSTANT_NAME_TYPE_USER;
				m_szUniformName = 0;
				if ( !name || !*name ) return;

				uint32_t length = (uint32_t)strlen( name );
				m_szUniformName = new char[ length+1 ];
				memcpy( (void*)m_szUniformName, name, length+1 );
			}

			void SetKnownName( uint32_t name )
			{
				if ( m_iNameType == AGK_SHADER_CONSTANT_NAME_TYPE_USER && m_szUniformName ) delete [] m_szUniformName;
				m_iNameType = AGK_SHADER_CONSTANT_NAME_TYPE_KNOWN;

				m_iKnownName = name;
			}

			void SetBindingName( uint32_t binding )
			{
				if ( m_iNameType == AGK_SHADER_CONSTANT_NAME_TYPE_USER && m_szUniformName ) delete [] m_szUniformName;
				m_iNameType = AGK_SHADER_CONSTANT_NAME_TYPE_BINDING;

				m_iBinding = binding;
			}

			void SetFloat( float f )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( AGK_SHADER_CONSTANT_TYPE_FLOAT, 0 );
				if ( m_pValues[0] == f ) return;
				m_pValues[0] = f;
				m_iChangeVersion++;
			}

			void SetVector2( float f1, float f2 )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( AGK_SHADER_CONSTANT_TYPE_VEC2, 0 );
				if ( m_pValues[0] == f1 && m_pValues[1] == f2 ) return;
				m_pValues[0] = f1;
				m_pValues[1] = f2;
				m_iChangeVersion++;
			}

			void SetVector3( float f1, float f2, float f3 )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( AGK_SHADER_CONSTANT_TYPE_VEC3, 0 );
				if ( m_pValues[0] == f1 && m_pValues[1] == f2 && m_pValues[2] == f3 ) return;
				m_pValues[0] = f1;
				m_pValues[1] = f2;
				m_pValues[2] = f3;
				m_iChangeVersion++;
			}

			void SetVector4( float f1, float f2, float f3, float f4 )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( AGK_SHADER_CONSTANT_TYPE_VEC4, 0 );
				if ( m_pValues[0] == f1 && m_pValues[1] == f2 && m_pValues[2] == f3 && m_pValues[3] == f4 ) return;
				m_pValues[0] = f1;
				m_pValues[1] = f2;
				m_pValues[2] = f3;
				m_pValues[3] = f4;
				m_iChangeVersion++;
			}

			void SetVector( const float *values, uint32_t numValues )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( numValues, 0 );
				bool changed = false;
				switch( numValues )
				{
					case 4:  if ( m_pValues[3] != values[3] ) { changed = true; m_pValues[3] = values[3]; } // fall through
					case 3:  if ( m_pValues[2] != values[2] ) { changed = true; m_pValues[2] = values[2]; } // fall through
					case 2:  if ( m_pValues[1] != values[1] ) { changed = true; m_pValues[1] = values[1]; } // fall through
					case 1: if ( m_pValues[0] != values[0] ) { changed = true; m_pValues[0] = values[0]; }
				}
				if ( changed ) m_iChangeVersion++;
			}
			
			void SetMatrix2( const float* values )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( AGK_SHADER_CONSTANT_TYPE_MAT2, 0 );

				if ( m_pValues[0] == values[0] && m_pValues[1] == values[1]
				  && m_pValues[2] == values[2] && m_pValues[3] == values[3] ) return;
				memcpy( m_pValues, values, 4*sizeof(float) );
				m_iChangeVersion++;
			}

			void SetMatrix3( const float* values )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( AGK_SHADER_CONSTANT_TYPE_MAT3, 0 );

				memcpy( m_pValues, values, 9*sizeof(float) );
				m_iChangeVersion++;
			}

			void SetMatrix4( const float* values )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( AGK_SHADER_CONSTANT_TYPE_MAT4, 0 );

				memcpy( m_pValues, values, 16*sizeof(float) );
				m_iChangeVersion++;
			}

			void SetStruct( const float* values, uint32_t floats )
			{
				if ( m_iDataType == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) Initialize( AGK_SHADER_CONSTANT_TYPE_STRUCT, floats );

				if ( m_iElementSize < floats ) floats = m_iElementSize;
				memcpy( m_pValues, values, floats*sizeof(float) );
				m_iChangeVersion++;
			}
	};

	class AGKShaderConstantArray : public AGKShaderConstantValue
	{
		public:
			unsigned short m_iNumElements = 0; // number of array elements
			unsigned short m_iNumElementsArraySize = 0; // max array size
			
			AGKShaderConstantArray() { SetIsArray(1); }
			~AGKShaderConstantArray() {}

		protected:
			void UpdateVectorArraySize( uint32_t numElements, bool preserve=false )
			{
				if ( preserve )
				{
					uint32_t newSize = numElements * m_iElementSize;
					float *newValues = new float[ newSize ];
					uint32_t oldSize = m_iNumElements*m_iElementSize;
					if ( newSize > oldSize )
					{
						memcpy( newValues, m_pValues, oldSize*sizeof(float) );
						memset( newValues+oldSize, 0, (newSize - oldSize)*sizeof(float) );
					}
					else
					{
						memcpy( newValues, m_pValues, newSize*sizeof(float) );
					}
					delete [] m_pValues;
					m_pValues = newValues;
					m_iNumElements = numElements;
					m_iNumElementsArraySize = numElements;
				}
				else
				{
					delete [] m_pValues;
					m_pValues = new float[ numElements * m_iElementSize ];
					m_iNumElements = numElements;
					m_iNumElementsArraySize = numElements;
					memset( m_pValues, 0, m_iNumElementsArraySize*m_iElementSize*sizeof(float) );
				}
			}

		public:
			void SetArraySize( uint32_t type, uint32_t numElements )
			{
				if ( m_pValues ) delete [] m_pValues;
				if ( numElements < 1 ) numElements = 1;
				m_iNumElementsArraySize = numElements;
				m_iNumElements = numElements;
				SetDataType( type );
				switch( type )
				{
					case AGK_SHADER_CONSTANT_TYPE_FLOAT: m_iElementSize = 1; break;
					case AGK_SHADER_CONSTANT_TYPE_VEC2: m_iElementSize = 2; break;
					case AGK_SHADER_CONSTANT_TYPE_VEC3: m_iElementSize = 3; break;
					case AGK_SHADER_CONSTANT_TYPE_VEC4: m_iElementSize = 4; break;
					case AGK_SHADER_CONSTANT_TYPE_MAT2: m_iElementSize = 4; break;
					case AGK_SHADER_CONSTANT_TYPE_MAT3: m_iElementSize = 9; break;
					case AGK_SHADER_CONSTANT_TYPE_MAT4: m_iElementSize = 16; break;
					default: return;
				}
				m_pValues = new float[m_iNumElementsArraySize*m_iElementSize]; 
				memset( m_pValues, 0, m_iNumElementsArraySize * m_iElementSize * sizeof(float) );
			}

			AGKShaderConstantArray* Clone() const
			{
				AGKShaderConstantArray* newConstant = new AGKShaderConstantArray();
				newConstant->m_iChangeVersion = m_iChangeVersion;
				newConstant->m_iElementSize = m_iElementSize;
				newConstant->m_iNumElements = m_iNumElements;
				newConstant->m_iNumElementsArraySize = m_iNumElementsArraySize;
				newConstant->m_iDataType = m_iDataType;
				newConstant->m_iNameType = m_iNameType;
				newConstant->m_szUniformName = m_szUniformName; // also copies known name or binding
				
				if ( newConstant->m_iNameType == AGK_SHADER_CONSTANT_NAME_TYPE_USER && newConstant->m_szUniformName ) 
				{
					newConstant->m_szUniformName = new char[ strlen(m_szUniformName)+1 ];
					strcpy( newConstant->m_szUniformName, m_szUniformName );
				}

				if ( m_pValues )
				{
					newConstant->m_pValues = new float[ m_iNumElements * m_iElementSize ]; 
					memcpy( newConstant->m_pValues, m_pValues, m_iNumElements * m_iElementSize * sizeof(float) );
				}

				return newConstant;
			}
			
			void SetArrayFloat( uint32_t index, float f )
			{
				if ( index > 65534 ) return;
				if ( GetDataType() == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) SetArraySize( AGK_SHADER_CONSTANT_TYPE_FLOAT, index+1 );
				else if ( index >= m_iNumElementsArraySize ) UpdateVectorArraySize( index+1, true );

				uint32_t offset = index * m_iElementSize;

				if ( m_pValues[offset] == f ) return;
				m_pValues[offset] = f;
				m_iChangeVersion++;
			}

			// set single element
			void SetArrayVector2( uint32_t index, float f1, float f2 )
			{
				if ( index > 65534 ) return;
				if ( GetDataType() == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) SetArraySize( AGK_SHADER_CONSTANT_TYPE_VEC2, index+1 );
				else if ( index >= m_iNumElementsArraySize ) UpdateVectorArraySize( index+1, true );

				uint32_t offset = index * m_iElementSize;
				if ( m_pValues[offset] == f1 && m_pValues[offset+1] == f2 ) return;
				m_pValues[offset] = f1;
				m_pValues[offset+1] = f2;
				m_iChangeVersion++;
			}

			void SetArrayVector3( uint32_t index, float f1, float f2, float f3 )
			{
				if ( index > 65534 ) return;
				if ( GetDataType() == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) SetArraySize( AGK_SHADER_CONSTANT_TYPE_VEC3, index+1 );
				else if ( index >= m_iNumElementsArraySize ) UpdateVectorArraySize( index+1, true );

				uint32_t offset = index * m_iElementSize;
				if ( m_pValues[offset] == f1 && m_pValues[offset+1] == f2 && m_pValues[offset+2] == f3 ) return;
				m_pValues[offset] = f1;
				m_pValues[offset+1] = f2;
				m_pValues[offset+2] = f3;
				m_iChangeVersion++;
			}

			void SetArrayVector4( uint32_t index, float f1, float f2, float f3, float f4 )
			{
				if ( index > 65534 ) return;
				if ( GetDataType() == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) SetArraySize( AGK_SHADER_CONSTANT_TYPE_VEC4, index+1 );
				else if ( index >= m_iNumElementsArraySize ) UpdateVectorArraySize( index+1, true );

				uint32_t offset = index * m_iElementSize;
				if ( m_pValues[offset] == f1 && m_pValues[offset+1] == f2 && m_pValues[offset+2] == f3 && m_pValues[offset+3] == f4 ) return;
				m_pValues[offset] = f1;
				m_pValues[offset+1] = f2;
				m_pValues[offset+2] = f3;
				m_pValues[offset+3] = f4;
				m_iChangeVersion++;
			}

			// set multiple elements
			void SetArrayVector( const float* values, uint32_t numElements, uint32_t type ) 
			{
				if ( GetDataType() == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) SetArraySize( type, numElements );
				else if ( numElements > m_iNumElementsArraySize ) UpdateVectorArraySize( numElements, false );

				memcpy( m_pValues, values, numElements*m_iElementSize*sizeof(float) );
				m_iNumElements = numElements;
				m_iChangeVersion++;
			}

			void SetArrayMatrix2( uint32_t index, const float* values )
			{
				if ( index > 65534 ) return;
				if ( GetDataType() == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) SetArraySize( AGK_SHADER_CONSTANT_TYPE_MAT2, index+1 );
				else if ( index >= m_iNumElementsArraySize ) UpdateVectorArraySize( index+1, true );

				uint32_t offset = index * m_iElementSize;
				memcpy( m_pValues + offset, values, 4*sizeof(float) );
				m_iChangeVersion++;
			}

			void SetArrayMatrix3( uint32_t index, const float* values )
			{
				if ( index > 65534 ) return;
				if ( GetDataType() == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) SetArraySize( AGK_SHADER_CONSTANT_TYPE_MAT3, index+1 );
				else if ( index >= m_iNumElementsArraySize ) UpdateVectorArraySize( index+1, true );

				uint32_t offset = index * m_iElementSize;
				memcpy( m_pValues + offset, values, 9*sizeof(float) );
				m_iChangeVersion++;
			}

			void SetArrayMatrix4( uint32_t index, const float* values )
			{
				if ( index > 65534 ) return;
				if ( GetDataType() == AGK_SHADER_CONSTANT_TYPE_UNKNOWN ) SetArraySize( AGK_SHADER_CONSTANT_TYPE_MAT4, index+1 );
				else if ( index >= m_iNumElementsArraySize ) UpdateVectorArraySize( index+1, true );

				uint32_t offset = index * m_iElementSize;
				memcpy( m_pValues + offset, values, 16*sizeof(float) );
				m_iChangeVersion++;
			}
	};

	// list is sorted by binding value
	class AGKShaderConstantsByBinding
	{
		public:
			AGKShaderConstantValue **m_pConstants = 0;
			uint32_t m_iNumItems = 0;
			uint32_t m_iArraySize = 0;

			AGKShaderConstantsByBinding() { m_pConstants = new AGKShaderConstantValue*[2]; m_iArraySize = 2; }
			~AGKShaderConstantsByBinding() { if ( m_pConstants ) delete [] m_pConstants; }

			void Clear( bool deleteItems=false )
			{
				if ( m_pConstants ) 
				{
					if ( deleteItems )
					{
						for( uint32_t i = 0; i < m_iNumItems; i++ ) delete m_pConstants[ i ];
					}
					delete [] m_pConstants;
				}
				m_pConstants = 0;
				m_iArraySize = 0;
				m_iNumItems = 0;
			}

			int Add( AGKShaderConstantValue* item )
			{
				if ( m_iArraySize < m_iNumItems+1 )
				{
					m_iArraySize = m_iArraySize + m_iArraySize/2;
					if ( m_iArraySize < 2 ) m_iArraySize = 2;
					AGKShaderConstantValue** newArray = new AGKShaderConstantValue*[ m_iArraySize ];
					if ( m_pConstants )
					{
						for( uint32_t i = 0; i < m_iNumItems; i++ ) newArray[ i ] = m_pConstants[ i ];
						delete [] m_pConstants;
					}
					m_pConstants = newArray;
				}

				int high = m_iNumItems-1;
				int low = 0;
				int mid;

				// binary search
				while( high >= low )
				{
					mid = (high+low)/2;
					if( m_pConstants[ mid ]->GetBindingName() > item->GetBindingName() ) high = mid-1;
					else if ( m_pConstants[ mid ]->GetBindingName() < item->GetBindingName() ) low = mid+1;
					else 
					{
						return -1; // not allowed dulplicates
					}
				}
				
				if ( low >= (int)m_iNumItems )
				{
					m_pConstants[ m_iNumItems++ ] = item;
					return m_iNumItems-1;
				}
				else
				{
					// insert new element at low index
					for( int i = m_iNumItems-1; i >= low; i-- )
					{
						m_pConstants[ i+1 ] = m_pConstants[ i ];
					}
					m_pConstants[ low ] = item;
					m_iNumItems++;
					return low;
				}
			}

			AGKShaderConstantValue* Find( uint32_t binding, int* index=0 )
			{
				if ( m_iNumItems == 0 ) return 0;

				int high = m_iNumItems-1;
				int low = 0;
				
				// binary search
				while( high >= low )
				{
					int mid = (high+low)/2;
					if( m_pConstants[ mid ]->GetBindingName() > binding ) high = mid-1;
					else if ( m_pConstants[ mid ]->GetBindingName() < binding ) low = mid+1;
					else 
					{
						if ( index ) *index = mid;
						return m_pConstants[ mid ];
					}
				}

				return 0;
			}

			void Remove( uint32_t binding )
			{
				int high = m_iNumItems-1;
				int low = 0;
				
				// binary search
				while( high >= low )
				{
					int mid = (high+low)/2;
					if( m_pConstants[ mid ]->GetBindingName() > binding ) high = mid-1;
					else if ( m_pConstants[ mid ]->GetBindingName() < binding ) low = mid+1;
					else 
					{
						AGKShaderConstantValue *pItem = m_pConstants[ mid ];
						for( uint32_t i = mid; i < m_iNumItems-1; i++ )
						{
							m_pConstants[ i ] = m_pConstants[ i+1 ];
						}
						m_iNumItems--;
						delete pItem;
						return;
					}
				}
			}
	};

	class AGKShaderConstantsByName
	{
		public:
			cHashedList<AGKShaderConstantValue*> m_cConstantsByName;
						
			AGKShaderConstantsByName( int hashTableSize ) : m_cConstantsByName(hashTableSize) {  }
			~AGKShaderConstantsByName() 
			{
				AGKShaderConstantValue *pVar = m_cConstantsByName.GetFirst();
				while( pVar )
				{
					delete pVar;
					pVar = m_cConstantsByName.GetNext();
				}
			}

			uint32_t GetCount() { return m_cConstantsByName.GetCount(); }

			void Remove( const char* szName )
			{
				AGKShaderConstantValue *pVar = m_cConstantsByName.RemoveItem( szName );
				if ( pVar ) delete pVar;
			}

			AGKShaderConstantValue* GetOrCreate( const char* szName )
			{
				AGKShaderConstantValue *pVar = m_cConstantsByName.GetItem( szName );
				if ( !pVar ) 
				{
					pVar = new AGKShaderConstantValue();
					pVar->SetName( szName );
					m_cConstantsByName.AddItem( pVar, szName );
				}
				return pVar;
			}
	};

	class AGKShader
	{
		friend class cObject3D;
		friend class OpenGL2Renderer;
		friend class VulkanRenderer;
        friend class OpenGLES2Renderer;

		public:
			uString m_sVSFilename;
			uString m_sPSFilename;

		protected:
			void* m_pInternalResources = 0; // for the renderer

			static uint64_t g_iLastCreated;
			uint64_t m_iCreated;

			AGKShader* m_pBaseShader = 0;
			uint32_t m_iShaderHash = 0;
			uint32_t m_iFlags = 0;
			AGKUnorderedArray<AGKShader*> m_cDerivedShaders;
			void AddDerived( AGKShader *pShader );
			void RemoveDerived( AGKShader *pShader );

			uString m_sVSSource;
			uString m_sPSSource;
			
			// base shader constants don't have an index so store them by name
			cHashedList<AGKShaderConstantValue*> m_baseConstants;

			// derived shader constants can be stored by index
			cHashedList<uint32_t> m_constantBindingByName;
			// constants set with SetConstantByName
			// could get rid of this and push the constants into the renderer with LOCAL_MEM set?
			AGKShaderConstantsByBinding m_localConstants; 

			uint32_t GetBindingForName( const char *name ) { return m_constantBindingByName.GetItem(name) - 1; }
			
			// vertex attributes
			AGKShaderAttrib* m_pAttribs = 0;
			int m_iNumAttribs = 0;

			uString m_sVSLog;
			uString m_sPSLog;
			uString m_sLinkLog;

			int m_iMeshUseCount = 0;

			AGKShader *m_pPrevShader = 0;
			AGKShader *m_pNextShader = 0;

			static AGKShader *g_pAllShaders;
			static AGKShader *g_pLastShader;
			
			static int g_iNumShadowBones;
			static int g_iNumShadowBonesAlpha;

		public:

			// global constants
			static AGKShaderConstantValue g_varTime;
			static AGKShaderConstantValue g_varSinTime;
			static AGKShaderConstantValue g_varInvert;
			static AGKShaderConstantValue g_varResolution;
			static AGKShaderConstantValue g_varOrtho;
			static AGKShaderConstantValue g_varShadowProj;
			static AGKShaderConstantValue g_varShadowProj2;
			static AGKShaderConstantValue g_varShadowProj3;
			static AGKShaderConstantValue g_varShadowProj4;
			static AGKShaderConstantValue g_varShadowParams;
			static AGKShaderConstantValue g_varShadowParams2;
			static AGKShaderConstantValue g_varFogRange;
			static AGKShaderConstantValue g_varFogColor1;
			static AGKShaderConstantValue g_varFogColor2;

			static AGKShaderConstantValue g_varDirLightDir;
			static AGKShaderConstantValue g_varDirLightColor;
			static AGKShaderConstantValue g_varLightAmbient;

			static AGKShaderConstantValue g_varDefaultUVBounds; // always offset=0 scale=1
			
			// default shaders
			static AGKShader *g_pShaderColor;
			static AGKShader *g_pShaderTexColor;
			static AGKShader *g_pShaderFont;
			static AGKShader *g_pShaderQuad;
			static AGKShader *g_pShaderShadow;
			static AGKShader *g_pShaderShadowAlpha;
			static AGKShader *g_pShaderShadowBone;
			static AGKShader *g_pShaderShadowBoneAlpha;
			static AGKShader *g_pShader3DParticlesTex;
			static AGKShader *g_pShader3DParticlesColor;

			static void CreateDefaultShaders();
			static void DeleteDefaultShaders();

			static AGKShader* GetShadowShader( int numBones, int alphamask );

			static void ReloadAll();

			static uint32_t GetMeshShaderHash( cMesh *pMesh );
			static uint32_t GetFinalShaderHash( int sunActive, int VSLights, int PSLights, int useShadows );
			static AGKShader* Make3DShader( cMesh *pMesh );
			static AGKShader* MakeFinalShader( AGKShader *pBaseShader, int sunActive, int VSLights, int PSLights, int useShadows, int normalMap );

			AGKShader();
			~AGKShader();

			void AddRef();
			void RemoveRef();

			uint64_t GetCreated() { return m_iCreated; }
			void* GetInternalResources() { return m_pInternalResources; }

			bool IsValid() const { return (m_iFlags & AGK_SHADER_VALID) != 0; }
			void SetValid( bool valid ) { if ( valid ) m_iFlags |= AGK_SHADER_VALID; else m_iFlags &= ~AGK_SHADER_VALID; }
			uint32_t GetHash() const { return m_iShaderHash; }
			AGKShader* GetBaseShader() const { return m_pBaseShader; }
			int IsCustom() const { return (m_iFlags & AGK_SHADER_IS_CUSTOM) ? 1 : 0; }
			int IsDefault() const { return (m_iFlags & AGK_SHADER_IS_DEFAULT) ? 1 : 0; }
			int IsGenerated() const { return (m_iFlags & (AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM)) ? 0 : 1; }
			bool NeedsAdditionalCode() const { return (m_iFlags & (AGK_SHADER_USES_VS_LIGHTING|AGK_SHADER_USES_PS_LIGHTING|AGK_SHADER_USES_FOG)) != 0; }
			bool NeedsFog() const { return (m_iFlags & AGK_SHADER_USES_FOG) != 0; }
			bool NeedsVSLighting() const { return (m_iFlags & AGK_SHADER_USES_VS_LIGHTING) != 0; }
			bool NeedsPSLighting() const { return (m_iFlags & AGK_SHADER_USES_PS_LIGHTING) != 0; }

			const char* GetVSLog() const { return m_sVSLog.GetStr(); }
			const char* GetPSLog() const { return m_sPSLog.GetStr(); }

			const char* GetVSSource() const { return m_sVSSource.GetStr(); }
			const char* GetPSSource() const { return m_sPSSource.GetStr(); }

			const char* GetVSFilename() const { return m_sVSFilename.GetStr(); }
			const char* GetPSFilename() const { return m_sPSFilename.GetStr(); }

			const char* GetBaseVSFilename() const { return m_pBaseShader ? m_pBaseShader->m_sVSFilename.GetStr() : m_sVSFilename.GetStr(); }
			const char* GetBasePSFilename() const { return m_pBaseShader ? m_pBaseShader->m_sPSFilename.GetStr() : m_sPSFilename.GetStr(); }

			uint32_t GetNumAttributes() { return m_iNumAttribs; }
			AGKShaderAttrib* GetAttribute( uint32_t index ) { return &(m_pAttribs[ index ]); }

			bool HasPerSpriteUniforms() const { return (m_iFlags & AGK_SHADER_PER_SPRITE_UNIFORM) != 0; }

			// sets the default values for all objects that use this shader. To set temporary values like the world matrix pass it to BindShaderConstants
			void SetConstantByName( const char* szName, float f1, float f2, float f3, float f4 );
			void SetConstantMatrixByName( const char* szName, int numValues, const float* values );

			void SetConstantArrayByName( const char* szName, uint32_t index, uint32_t numValues, float f1, float f2, float f3, float f4 );
			void SetConstantArrayMatrixByName( const char* szName, uint32_t index, int numValues, const float* values );
		};
}

#endif
