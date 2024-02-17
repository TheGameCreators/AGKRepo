#ifndef _H_AGK_RENDERER
#define _H_AGK_RENDERER

#include "Common.h"
#include "cImage.h"
#include "AGKShader.h"

// values format bits [0-3]=blend src, [4-7]=blend dst, [8]=blend enable, [9]=depth enable, [10]=depth write enable, [11]=depth clamp enable, 
//		    		  [12-14]=depth func, [15]=has depth bias, [16]=has depth range, [17]=winding order, [18-19]=cull mode, [20]=scissor override
//					  [21]=has multisample
#define AGK_RENDERSTATE_MASK_BLEND_SRC			0x0F
#define AGK_RENDERSTATE_SHIFT_BLEND_SRC			0
#define AGK_RENDERSTATE_MASK_BLEND_DST			0x0F
#define AGK_RENDERSTATE_SHIFT_BLEND_DST			4
#define AGK_RENDERSTATE_SHIFT_BLEND_ENABLE		8
#define AGK_RENDERSTATE_SHIFT_DEPTH_ENABLE		9
#define AGK_RENDERSTATE_SHIFT_DEPTH_WRITE_ENABLE 10
#define AGK_RENDERSTATE_SHIFT_DEPTH_CLAMP_ENABLE 11
#define AGK_RENDERSTATE_MASK_DEPTH_FUNC			0x07
#define AGK_RENDERSTATE_SHIFT_DEPTH_FUNC		12
#define AGK_RENDERSTATE_SHIFT_HAS_DEPTH_BIAS	15
#define AGK_RENDERSTATE_SHIFT_HAS_DEPTH_RANGE	16
#define AGK_RENDERSTATE_SHIFT_WINDING			17
#define AGK_RENDERSTATE_MASK_CULL				0x03
#define AGK_RENDERSTATE_SHIFT_CULL				18
#define AGK_RENDERSTATE_SHIFT_HAS_SCISSOR		20
#define AGK_RENDERSTATE_SHIFT_HAS_MULTISAMPLE	21

#define AGK_RENDERSTATE_MASK_ALL_BLEND			0x1FF
#define AGK_RENDERSTATE_MASK_ALL_DEPTH			(0xFF << 9)

#define AGK_CULL_NONE	0	// will draw both
#define AGK_CULL_BACK	1	// will draw front faces
#define AGK_CULL_FRONT	2   // will draw back faces
// if this goes higher than 3 then change render state hash

// must be the values 0 and 1
#define AGK_WINDING_CLOCKWISE			0	
#define AGK_WINDING_COUNTER_CLOCKWISE	1

#define AGK_DEPTH_FUNC_NEVER	0
#define AGK_DEPTH_FUNC_LESS		1
#define AGK_DEPTH_FUNC_EQUAL	2
#define AGK_DEPTH_FUNC_LEQUAL	3
#define AGK_DEPTH_FUNC_GREATER	4
#define AGK_DEPTH_FUNC_NOTEQUAL	5
#define AGK_DEPTH_FUNC_GEQUAL	6
#define AGK_DEPTH_FUNC_ALWAYS	7
// if this goes higher than 7 then change render state hash

#define AGK_BLEND_ZERO					0
#define AGK_BLEND_ONE					1
#define AGK_BLEND_SRC_ALPHA				2
#define AGK_BLEND_ONE_MINUS_SRC_ALPHA	3
#define AGK_BLEND_DST_ALPHA				4
#define AGK_BLEND_ONE_MINUS_DST_ALPHA	5
#define AGK_BLEND_SRC_COLOR				6
#define AGK_BLEND_ONE_MINUS_SRC_COLOR	7
#define AGK_BLEND_DST_COLOR				8
#define AGK_BLEND_ONE_MINUS_DST_COLOR	9
#define AGK_BLEND_ALPHA_SATURATE		10
// if this goes higher than 15 then change render state hash

namespace AGK
{ 
	static const unsigned int SpvMagicNumber = 0x07230203;

	class AGKShader;
	class AGKVertexLayout;

	class AGKRenderState 
	{
		public:
			AGKRenderState() 
			{
				m_iValues = (1 << AGK_RENDERSTATE_SHIFT_DEPTH_ENABLE)
						  | (1 << AGK_RENDERSTATE_SHIFT_DEPTH_WRITE_ENABLE)
						  | (AGK_DEPTH_FUNC_LESS << AGK_RENDERSTATE_SHIFT_DEPTH_FUNC)
						  | (AGK_WINDING_COUNTER_CLOCKWISE << AGK_RENDERSTATE_SHIFT_WINDING)
						  | (AGK_CULL_BACK << AGK_RENDERSTATE_SHIFT_CULL);
			}
			~AGKRenderState() {}

			int Compare( AGKRenderState *pOther )
			{
				uint32_t hash = GetHash();
				uint32_t otherHash = pOther->GetHash();

				if ( hash > otherHash ) return 1;
				else if ( hash < otherHash ) return -1;

				if ( GetDepthEnable() )
				{
					if ( m_fDepthBias > pOther->m_fDepthBias ) return 1;
					else if ( m_fDepthBias < pOther->m_fDepthBias ) return -1;

					if ( m_fDepthMax > pOther->m_fDepthMax ) return 1;
					else if ( m_fDepthMax < pOther->m_fDepthMax ) return -1;

					if ( m_fDepthMin > pOther->m_fDepthMin ) return 1;
					else if ( m_fDepthMin < pOther->m_fDepthMin ) return -1;
				}

				if ( DoesOverrideScissor() )
				{
					if ( m_iScissorHash > pOther->m_iScissorHash ) return 1;
					else if ( m_iScissorHash < pOther->m_iScissorHash ) return -1;
				}

				if ( m_iMultiSampleCount > pOther->m_iMultiSampleCount ) return 1;
				else if ( m_iMultiSampleCount < pOther->m_iMultiSampleCount ) return -1;

				return 0;
			}
			
		// scissor
		protected:
			union
			{
				struct
				{
					uint16_t m_iScissorX;
					uint16_t m_iScissorY;
					uint16_t m_iScissorWidth;
					uint16_t m_iScissorHeight;
				};
				uint64_t m_iScissorHash = 0;
			};

			// m_iValues bits [0-3]=blend src, [4-7]=blend dst, [8]=blend enable, [9]=depth enable, [10]=depth write enable, [11]=depth clamp enable, 
			//		    	  [12-14]=depth func, [15]=has depth bias, [16]=has depth range, [17]=winding order, [18-19]=cull mode, [20]=scissor override
			//				  [21]=has multisample
			uint32_t m_iValues; // only includes small types
		public:
			void SetScissor( int x, int y, int width, int height ) 
			{
				if ( x < 0 ) x = 0;
				if ( y < 0 ) y = 0;
				if ( width > 65535 ) width = 65535;
				if ( height > 65535 ) height = 65535;
				m_iScissorX = x; 
				m_iScissorY = y; 
				m_iScissorWidth = width; 
				m_iScissorHeight = height; 
			}
			void OverrideScissor( int mode )
			{
				if ( mode ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_HAS_SCISSOR);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_HAS_SCISSOR);
			}
			bool DoesOverrideScissor() const { return ((m_iValues >> AGK_RENDERSTATE_SHIFT_HAS_SCISSOR) & 1) != 0; }
			uint16_t GetScissorX() const { return m_iScissorX; }
			uint16_t GetScissorY() const { return m_iScissorY; }
			uint16_t GetScissorWidth() const { return m_iScissorWidth; }
			uint16_t GetScissorHeight() const { return m_iScissorHeight; }
			uint64_t GetScissorHash() const { return m_iScissorHash; }

			uint32_t GetHash() 
			{ 
				uint32_t iHash = m_iValues;
				if ( !GetDepthEnable() ) iHash &= ~AGK_RENDERSTATE_MASK_ALL_DEPTH;
				if ( !GetBlendEnable() ) iHash &= ~AGK_RENDERSTATE_MASK_ALL_BLEND;
				return iHash;
			}

		// cull mode
			void SetCullMode( int cullMode ) 
			{
				m_iValues &= ~(AGK_RENDERSTATE_MASK_CULL << AGK_RENDERSTATE_SHIFT_CULL);
				m_iValues |= (cullMode << AGK_RENDERSTATE_SHIFT_CULL);
			}
			int GetCullMode() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_CULL) & AGK_RENDERSTATE_MASK_CULL; }
		
		// winding order
			void SetWindingOrder( int windingOrder ) 
			{
				if ( windingOrder == AGK_WINDING_COUNTER_CLOCKWISE ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_WINDING);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_WINDING);
			}
			int GetWindingOrder() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_WINDING) & 1; }
			void InvertWindingOrder() { m_iValues ^= (1 << AGK_RENDERSTATE_SHIFT_WINDING); }

		// depth
			void SetDepthEnable( int enable ) 
			{
				if ( enable ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_DEPTH_ENABLE);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_DEPTH_ENABLE);
			}
			int GetDepthEnable() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_DEPTH_ENABLE) & 1; }

			void SetDepthWriteEnable( int enable ) 
			{
				if ( enable ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_DEPTH_WRITE_ENABLE);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_DEPTH_WRITE_ENABLE);
			}
			int GetDepthWriteEnable() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_DEPTH_WRITE_ENABLE) & 1; }

			void SetDepthFunc( int func ) 
			{
				m_iValues &= ~(AGK_RENDERSTATE_MASK_DEPTH_FUNC << AGK_RENDERSTATE_SHIFT_DEPTH_FUNC);
				m_iValues |= (func << AGK_RENDERSTATE_SHIFT_DEPTH_FUNC);
			}
			int GetDepthFunc() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_DEPTH_FUNC) & AGK_RENDERSTATE_MASK_DEPTH_FUNC; }

			void SetDepthClampEnable( int enable ) 
			{
				if ( enable ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_DEPTH_CLAMP_ENABLE);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_DEPTH_CLAMP_ENABLE);
			}
			int GetDepthClampEnable() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_DEPTH_CLAMP_ENABLE) & 1; }

		protected:
			float m_fDepthBias = 0;
		public:
			void SetDepthBias( float bias ) 
			{
				m_fDepthBias = bias;
				if ( bias != 0 ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_HAS_DEPTH_BIAS);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_HAS_DEPTH_BIAS);
			}
			int HasDepthBias() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_HAS_DEPTH_BIAS) & 1; }
			float GetDepthBias() const { return m_fDepthBias; }

		protected:
			float m_fDepthMin = 0;
			float m_fDepthMax = 1;
		public:
			void SetDepthRange( float min, float max ) 
			{
				if ( min < 0 ) min = 0;
				if ( min > 1 ) min = 1;
				if ( max < 0 ) max = 0;
				if ( max > 1 ) max = 1;

				m_fDepthMin = min;
				m_fDepthMax = max;
				if ( min != 0 || max != 1 ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_HAS_DEPTH_RANGE);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_HAS_DEPTH_RANGE);
			}
			int HasDepthRange() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_HAS_DEPTH_RANGE) & 1; }
			float GetDepthRangeMin() const { return m_fDepthMin; }
			float GetDepthRangeMax() const { return m_fDepthMax; }

			/*
			int8_t m_iStencilEnable;
			int8_t m_iStencilFrontPass;
			int8_t m_iStencilFrontFail;
			int8_t m_iStencilFrontDepthFail;
			int8_t m_iStencilFrontCompareOp;
			int8_t m_iStencilFrontCompareMask;
			int8_t m_iStencilFrontWriteMask;
			int8_t m_iStencilFrontReference;
			int8_t m_iStencilBackPass;
			int8_t m_iStencilBackFail;
			int8_t m_iStencilBackDepthFail;
			int8_t m_iStencilBackCompareOp;
			int8_t m_iStencilBackCompareMask;
			int8_t m_iStencilBackWriteMask;
			int8_t m_iStencilBackReference;
			*/

		// blending
			void SetBlendEnable( int enable ) 
			{
				if ( enable ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_BLEND_ENABLE);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_BLEND_ENABLE);
			}
			int GetBlendEnable() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_BLEND_ENABLE) & 1; }

			void SetBlendFunc( int src, int dst ) 
			{
				uint32_t mask = (AGK_RENDERSTATE_MASK_BLEND_SRC << AGK_RENDERSTATE_SHIFT_BLEND_SRC) | (AGK_RENDERSTATE_MASK_BLEND_DST << AGK_RENDERSTATE_SHIFT_BLEND_DST);
				m_iValues &= ~mask;
				m_iValues |= (src << AGK_RENDERSTATE_SHIFT_BLEND_SRC) | (dst << AGK_RENDERSTATE_SHIFT_BLEND_DST);
			}
			int GetBlendColorSrc() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_BLEND_SRC) & AGK_RENDERSTATE_MASK_BLEND_SRC; }
			int GetBlendColorDst() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_BLEND_DST) & AGK_RENDERSTATE_MASK_BLEND_DST; }
		
			/*
			int8_t m_iBlendAlphaSrc = AGK_BLEND_ONE_MINUS_DST_ALPHA;
			int8_t m_iBlendAlphaDst = AGK_BLEND_ONE;
			int8_t m_iBlendColorFunc = 0;
			int8_t m_iBlendAlphaFunc = 0;
			int8_t m_iBlendColorMask = 0;
			*/

		// multisampling
		protected:
			int8_t m_iMultiSampleCount = 1;
		public:
			void SetMultiSampleCount( int8_t samples ) 
			{
				m_iMultiSampleCount = samples;
				if ( samples > 1 ) m_iValues |= (1 << AGK_RENDERSTATE_SHIFT_HAS_MULTISAMPLE);
				else m_iValues &= ~(1 << AGK_RENDERSTATE_SHIFT_HAS_MULTISAMPLE);
			}
			int HasMultisampling() const { return (m_iValues >> AGK_RENDERSTATE_SHIFT_HAS_MULTISAMPLE) & 1; }
			int8_t GetMultiSampleCount() const { return m_iMultiSampleCount; }
	};

	#define AGK_MAX_TEXTURE_STAGES 16

	#define AGK_PRESENT_MODE_DELAYED	0
	#define AGK_PRESENT_MODE_IMMEDIATE	1
	//#define AGK_PRESENT_MODE_MANUAL	2 // no longer used

	enum AGKRenderer
	{
		AGK_RENDERER_OPENGL2 = 0,
		AGK_RENDERER_OPENGL_ES2,
		AGK_RENDERER_VULKAN,
	};

	enum AGKRendererMode
	{
		AGK_RENDERER_MODE_PREFER_BEST = 0, // will try the best renderer but fall back to a compatible renderer, e.g. Vulkan then OpenGL
		AGK_RENDERER_MODE_ONLY_ADVANCED, // will only succeed if an advanced renderer such as Vulkan or Metal is supported on the current device
		AGK_RENDERER_MODE_ONLY_LOWEST, // will choose the renderer with the widest support on the current platform, e.g. OpenGL, even if more advanced renderers are available
		AGK_RENDERER_MODE_NONE // will not use a renderer, no graphics commands can be used
	};

	class Renderer
	{
		protected:
			int m_iSupportsDepthTexture = 0;
			int m_iSupportsDepth24 = 0;
			int m_iSupportsDepthNonLinear = 0;
			int m_iSupportsIntIndices = 0;
			int m_iSupportsNPOT = 0;
			int m_iSupportsMipmapNPOT = 0;
			int m_iSupportsDepthClamp = 0;
			int m_iSupportsPSHighP = 0;

			int m_iFBOFlipped = 0;
			int m_iTopLeftOrigin = 0;
			int m_iClipSpaceZeroOne = 0; // 0 to 1, or -1 to 1
			int m_iInvertedDepth = 0;
		
			int m_iMaxTextureSize = 0; // 2D image
			int m_iMaxVSUniforms = 0;
			int m_iMaxPSUniforms = 0;
			int m_iMaxVaryings = 0;

			int m_iSurfaceWidth = 0;
			int m_iSurfaceHeight = 0;

			uint32_t m_iVerticesProcessed = 0;
			uint32_t m_iPolygonsDrawn = 0;

			uint32_t m_iShaderErrorMode = 0; // 0=report compile errors and unsupported shaders, 1=report compile errors only, 2=report nothing
		
		public:
			Renderer() {}
			virtual ~Renderer() {}	

			virtual AGKRenderer GetIdentifier() = 0;
			virtual const char* GetName() = 0;

			// capabilities
			int SupportsDepthTexture() { return m_iSupportsDepthTexture; }
			int SupportsDepth24() { return m_iSupportsDepth24; }
			int SupportsDepthNonLinear() { return m_iSupportsDepthNonLinear; }
			int SupportsIntIndices() { return m_iSupportsIntIndices; }
			int SupportsNPOT() { return m_iSupportsNPOT; }
			int SupportsMipmapNPOT() { return m_iSupportsMipmapNPOT; }
			int SupportsDepthClamp() { return m_iSupportsDepthClamp; }
			int SupportsPSHighP() { return m_iSupportsPSHighP; }

			virtual int SupportsGLSL450() = 0;
			virtual int SupportsSPIRV() = 0;
		
			int IsFBOFlipped() { return m_iFBOFlipped; }
			int IsTopLeftOrigin() { return m_iTopLeftOrigin; }
			int IsClipSpaceZeroOne() { return m_iClipSpaceZeroOne; }
			int IsInvertedDepth() { return m_iInvertedDepth; }
		
			int GetMaxTextureSize() { return m_iMaxTextureSize; }
			int GetMaxVSUniforms() { return m_iMaxVSUniforms; }
			int GetMaxPSUniforms() { return m_iMaxPSUniforms; }
			int GetMaxVaryings() { return m_iMaxVaryings; }

			int GetSurfaceWidth() { return m_iSurfaceWidth; }
			int GetSurfaceHeight() { return m_iSurfaceHeight; }

			// setup
			virtual int Init() = 0;
			virtual int SetupWindow( void* param1, void* param2, unsigned int width, unsigned int height ) = 0;
			virtual int WindowResized( void* param1, void* param2, unsigned int width, unsigned int height ) = 0;
			virtual int SetScreenRecordParams( void* param1, void* param2 ) = 0;
			virtual int Setup() = 0;
			virtual int Swap() = 0;
			virtual void SetPresentMode( uint32_t mode ) = 0; // set delayed (0) or immediate (1) present
			virtual int ForcePresent() = 0; // if the renderer does delayed presenting then this forces it to present now
			virtual void UpdateSurfaceSize() = 0;
			virtual void CleanUp() = 0;
			virtual void ResetInternalState() = 0;
			virtual void* GetDevicePtr() = 0;
			virtual void LosingSurface() = 0;
			virtual void LosingFocus() = 0;
				
			// core
			virtual void SetVSync( int mode ) = 0;
			virtual void SetGlobalScissor( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) = 0;
			virtual void SetViewport( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) = 0;
			virtual void ClearRenderTarget( float red, float green, float blue, float alpha ) = 0;
			virtual void ClearDepthBuffer() = 0;
			virtual void SetAntialiasMode( int mode ) = 0;
			virtual void GetDataFromScreen( uint32_t **pData, int x, int y, int width, int height ) = 0;
			virtual int GetRenderPassStarted() = 0;
			void ResetDrawStats() { m_iVerticesProcessed = 0; m_iPolygonsDrawn = 0; }
			uint32_t GetPolygonsDrawn() { return m_iPolygonsDrawn; }
			uint32_t GetVerticesProcessed() { return m_iVerticesProcessed; }
			
			// image commands
			virtual bool IsImageFormatSupported( AGKImgFormat format ) = 0;
			virtual int CreateImage2D( AGKImgFormat format, uint32_t width, uint32_t height, uint8_t *data, uint32_t flags, void **outResource ) = 0;
			virtual int UpdateSubImage2D( void *pResource, uint32_t mipmapLevel, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *data ) = 0;
			virtual int CreateDepthBuffer( AGKImgFormat format, uint32_t width, uint32_t height, uint32_t flags, void **outResource ) = 0;
			virtual int DeleteImage( void *inResource, int unbind=1 ) = 0;
			virtual int SetImageSampleSettings( void *pResource, uint32_t flags ) = 0;
			virtual void SetImageMagFilter( void *pResource, uint32_t mode ) = 0;
			virtual void SetImageMinFilter( void *pResource, uint32_t mode ) = 0;
			virtual void SetImageWrapU( void *pResource, uint32_t mode ) = 0;
			virtual void SetImageWrapV( void *pResource, uint32_t mode ) = 0;
			virtual int GetImageData( void **pResource, uint8_t **pData ) = 0;
			virtual uint32_t GetImageInternalID( void *pResource ) = 0;
			virtual void ClearImageBindings() = 0;
			virtual int OverrideTexture( uint32_t width, uint32_t height, AGKImgFormat format, void* texture, void **outResource ) = 0;

			// vertex commands
			virtual int CreateVertices( uint32_t vertexSize, void **outResource ) = 0;
			virtual int UpdateVertices( void *pResource, float *pVertexData, uint32_t offset, uint32_t numVertices ) = 0;
			virtual int UpdateIndices( void *pResource, void *pIndexData, uint32_t offset, uint32_t numIndices ) = 0; // pIndexData must be unsigned short if original indices were unsigned short
			virtual int SetVertices( void *pResource, float *pVertexData, uint32_t numVertices ) = 0;
			virtual int SetIndices( void *pResource, uint16_t *pIndexData, uint32_t numIndices ) = 0;
			virtual int SetIndicesInt( void *pResource, uint32_t *pIndexData, uint32_t numIndices ) = 0;
			virtual int DeleteVertices( void *pResource ) = 0;
		
			// frame buffer commands
			virtual int CreateFrameBuffer( void *pColorImage, void *pDepthImage, int iForceDepth, void **outResource ) = 0;
			virtual int DeleteFrameBuffer( void *pResource ) = 0;
			virtual void SetFrameBufferIsShadow( void *pResource ) = 0;
			virtual void StartShadowFrameBuffer( void *pResource ) = 0;
			virtual void StartShadowFrameBuffers( void *pResource1, void *pResource2, void *pResource3, void *pResource4 ) = 0;
			virtual void CompleteShadowFrameBuffer( void *pResource ) = 0;
			virtual void CompleteShadowFrameBuffers( void *pResource1, void *pResource2, void *pResource3, void *pResource4 ) = 0;
			virtual int BindFrameBuffer( void *pCommandBuffer, void *pResource ) = 0;

			// shader commands
			void SetShaderErrorMode( uint32_t mode ) { m_iShaderErrorMode = mode; }
			virtual const char* GetShaderDefaultFullScreenVS() = 0;
			virtual const char* GetShaderDefaultSpriteVS() = 0;

			virtual AGKShader* LoadDefaultShaderColor() = 0;
			virtual AGKShader* LoadDefaultShaderTexColor() = 0;
			virtual AGKShader* LoadDefaultShaderFont() = 0;
			virtual AGKShader* LoadDefaultShaderShadow() = 0;
			virtual AGKShader* LoadDefaultShaderShadowAlpha() = 0;
			virtual AGKShader* LoadDefaultShaderShadowBone( uint32_t numBones ) = 0;
			virtual AGKShader* LoadDefaultShaderShadowBoneAlpha( uint32_t numBones ) = 0;
			virtual AGKShader* LoadDefaultShader3DParticlesTex() = 0;
			virtual AGKShader* LoadDefaultShader3DParticlesColor() = 0;
			virtual AGKShader* LoadDefaultShaderVideo() = 0;
			virtual AGKShader* LoadDefaultShaderDeviceCamera() = 0;
			virtual AGKShader* LoadDefaultShaderARAndroid() = 0;
			virtual AGKShader* LoadDefaultShaderARiOS() = 0;
			virtual AGKShader* LoadDefaultShaderQuad() = 0;
			virtual AGKShader* LoadDefaultShaderSkyBoxSun() = 0;
			virtual AGKShader* LoadDefaultShaderSkyBoxNoSun() = 0;
				
			// shader commands
			virtual AGKShader* Make3DShader( cMesh *pMesh ) = 0;
			virtual AGKShader* MakeFinalShader( AGKShader *pBaseShader, int sunActive, int VSLights, int PSLights, int useShadows, int normalMap ) = 0;
			virtual AGKShader* LoadShader( const char *szVertexSource, const char *szPixelSource, const char *szVertexFile, const char *szPixelFile ) = 0;
			virtual AGKShader* LoadShaderSPIRV( uint32_t *pVertexSource, uint32_t sizeVS, uint32_t *pPixelSource, uint32_t sizePS ) = 0;
			virtual void ReloadShader( AGKShader *pShader ) = 0;
			virtual void DeleteShader( AGKShader *pShader ) = 0; // only deletes internal resources, not the pointer itself

			// shader constants
			virtual void SetShaderConstant( AGKShader *pShader, uint32_t index, AGKShaderConstantValue *pConstant ) = 0;
			virtual int BindShaderConstants( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKUnorderedArray<AGKShaderConstantValue*> *pAdditionalConstants, 
											 AGKDynamicArray<cImage*> *pTextures, void **inoutConstants ) = 0;
			virtual int BindShaderConstantUpdates( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKDynamicArray<cImage*> *pTextures, void **inoutConstants ) = 0;
			virtual int DeleteShaderConstants( void *pConstants ) = 0;

			// pipeline commands
			//virtual int CreatePipeline( AGKShader *pShader, const AGKRenderState *pRenderState, const AGKVertexLayout *pVertexLayout, void **inoutPipeline ) = 0;
			virtual int BindPipeline( void *pCommandBuffer, AGKShader *pShader, const AGKRenderState *pRenderState, const AGKVertexLayout *pVertexLayout, void **inoutPipeline ) = 0;
			virtual int DeletePipeline( void *pPipeline ) = 0;
		
			// drawing commands
			virtual int DrawVertices( void *pCommandBuffer, void *pVertices, int count=-1 ) = 0;
	};
}

#endif // _H_AGK_RENDERER
