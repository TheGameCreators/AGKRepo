#ifndef _H_AGK_OPENGLES2_
#define _H_AGK_OPENGLES2_

#include "PlatformDefines.h"
#include "../Renderer.h"

#define AGK_OPENGLES2_MAX_TEXTURE_STAGES 8

#define GL_SAMPLER_2D_EXTERNAL	0x8D66

#ifdef AGK_ANDROID
	#include <EGL/egl.h>
	#include <GLES2/gl2.h>
	#include "GLES2/gl2ext.h"

	#include <android/window.h>
	#include <android/native_activity.h>

	int GLESCreateRendererWindow( void *ptr, int deviceWidth, int deviceHeight );
	void GLESSetRendererPointers( void *ptr, int deviceWidth, int deviceHeight );

	void GLESRegenerateExternalTexture( unsigned int *tex );
	void GLESBindExternalTexture( unsigned int tex );
	void GLESDeleteExternalTexture( unsigned int *tex );

	void GLESSetRecordWindow( ANativeWindow* window );
#endif

#ifdef AGK_IOS
	#import <OpenGLES/EAGL.h>
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
	#import <OpenGLES/EAGLDrawable.h>

	#include "OpenGLES/ES3/gl.h"
    #include "OpenGLES/ES3/glext.h"

	int GLESCreateRendererWindow( void *ptr, int deviceWidth, int deviceHeight );
	void GLESSetRendererPointers( void *ptr, int deviceWidth, int deviceHeight );
	
    int GLESCreateVideoImageData();
    int GLESHasVideoImageData();
    void GLESCopyVideoImageToAGKImage( CVPixelBufferRef buffer, cImage* pImage );
    void GLESReleaseVideoImageData();

    int GLESCreateCameraImageData();
    int GLESHasCameraImageData();
    void GLESCopyCameraImageToAGKImage( CVImageBufferRef buffer, cImage* pImage );
    void GLESReleaseCameraImageData();

    int GLESCreateARImageData();
    int GLESHasARImageData();
    void GLESCopyARImageToAGKImage( CVPixelBufferRef buffer, cImage* pImageY, cImage* pImageUV );
    void GLESReleaseARImageData();
#endif

#ifdef AGK_RASPPI
	#include "bcm_host.h"
	#include "GLES2/gl2.h"
	#include "GLES2/gl2ext.h"
	#include "EGL/egl.h"
	#include "EGL/eglext.h"

/*
	void SetRendererPointers( void *ptr );
	int GetSurfaceWidth();
	int GetSurfaceHeight();
*/
#endif

#ifdef AGK_HTML5
	#include <GLES2/gl2.h>
	#define GLFW_INCLUDE_ES2
	#include <GLFW/glfw3.h>
#endif

void CheckRendererExtensions();


// internal image flags in the high 16bits
#define AGK_GLES2_IMAGE_IS_DEPTH		0x00010000

// internal vertex flags
#define AGK_GLES2_VERTICES_INDICES_INT	0x01

namespace AGK
{

	class OpenGLES2Image
	{
		public:
			OpenGLES2Image() {}
			~OpenGLES2Image() {}
				
			uint32_t m_iTextureID = 0;
			uint32_t m_iWidth = 0;
			uint32_t m_iHeight = 0;
			uint32_t m_iFlags = 0;
			uint32_t m_iFormat = 0;
			uint32_t m_iByteSize = 0;
	};

	class OpenGLES2Vertices
	{
		public:
			OpenGLES2Vertices() {}
			~OpenGLES2Vertices() {}

			uint32_t m_iVertexSize = 0; // single vertex size in bytes
			uint32_t m_iNumVertices = 0;
			uint32_t m_iNumIndices = 0;
			uint32_t m_iFlags = 0;
				
			uint32_t m_iVertexVBO = 0;
			uint32_t m_iIndexVBO = 0;
	};

	#define AGK_GLES2_FRAMEBUFFER_FORCE_DEPTH	0x00010000

	class OpenGLES2FrameBuffer
	{
		public:
			OpenGLES2FrameBuffer() {}
			~OpenGLES2FrameBuffer() {}

			OpenGLES2Image *m_pColor = 0;
			OpenGLES2Image *m_pDepth = 0;
			uint32_t m_iFlags = 0;

			uint32_t m_iRBO = 0;
			uint32_t m_iFBO = 0;
	};

	#define AGK_OPENGLES2_SHADER_LOCATION_IS_SET			0x01
	#define AGK_OPENGLES2_SHADER_LOCATION_USES_LOCAL_MEM	0x02
	#define AGK_OPENGLES2_SHADER_LOCATION_RELOAD			0x04
		
	class OpenGLES2ShaderLocation
	{
		public:
			OpenGLES2ShaderLocation() {}
			~OpenGLES2ShaderLocation() { if ( (m_iFlags & AGK_OPENGLES2_SHADER_LOCATION_USES_LOCAL_MEM) && m_pDefaultValue ) delete m_pDefaultValue; }

			void Set() { m_iFlags |= AGK_OPENGLES2_SHADER_LOCATION_IS_SET; }
			void UnSet() { m_iFlags &= ~AGK_OPENGLES2_SHADER_LOCATION_IS_SET; }
			bool IsSet() { return (m_iFlags & AGK_OPENGLES2_SHADER_LOCATION_IS_SET) != 0; }

			void Reload() { m_iFlags |= AGK_OPENGLES2_SHADER_LOCATION_RELOAD; }
			void Reloaded() { m_iFlags &= ~AGK_OPENGLES2_SHADER_LOCATION_RELOAD; }
			bool ShouldReload() { return (m_iFlags & AGK_OPENGLES2_SHADER_LOCATION_RELOAD) != 0; }

			AGKShaderConstantValue *m_pDefaultValue = 0;
			AGKShaderConstantValue *m_pCurrValue = 0;
			uint32_t m_iChangeVersion = 0;
			uint32_t m_iLocation = 0; // must be at least 32-bit as OpenGL provides no guarantees on value limits
			uint8_t m_iType = AGK_SHADER_CONSTANT_TYPE_UNKNOWN;
			uint8_t m_iFlags = 0;
	};

	class OpenGLES2Shader
	{
		public:
			OpenGLES2Shader() {}
			~OpenGLES2Shader() { if ( m_pLocations ) delete [] m_pLocations; }

			uint32_t m_iShaderID = 0;

			uint16_t m_iNumLocations = 0;
			OpenGLES2ShaderLocation *m_pLocations = 0;

			uint16_t m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_MAX_VALUE ]; // index into m_pLocations
			uint32_t m_iTextureLocations[ AGK_OPENGLES2_MAX_TEXTURE_STAGES ]; // must be at least 32-bit as OpenGL provides no guarantees on value limits
	};

	class OpenGLES2Renderer : public Renderer
	{
		protected:
			uint32_t m_iCurrTexture[ AGK_OPENGLES2_MAX_TEXTURE_STAGES ] = { 0 };
			uint32_t m_iCurrentBoundVBO = 0;
			uint32_t m_iVBOPtrSetup = 0;
			uint32_t m_iCurrentBoundIndexVBO = 0;

			OpenGLES2FrameBuffer *m_pCurrFrameBuffer = 0;
			uint32_t m_iCurrShader = 0;
			AGKShader *m_pCurrAGKShader = 0;
			const AGKVertexLayout *m_pCurrVertexLayout = 0;

			float m_fCurrentDepthBias = 0;
			float m_fCurrentDepthMin = 0;
			float m_fCurrentDepthMax = 1;
		
			int8_t m_iCurrActiveTexture = -1;
			int8_t m_iScissorEnabled = -1;
			int8_t m_iCurrentCullMode = -1;	
			int8_t m_iCurrentWindingOrder = -1;
			int8_t m_iCurrentDepthEnable = -1;
			int8_t m_iCurrentDepthFunc = -1;
			int8_t m_iCurrentDepthClampEnable = -1;
			int8_t m_iCurrentDepthWriteEnable = -1;
			int8_t m_iCurrentDepthBiasEnable = -1;
			int8_t m_iCurrentStencilEnable = -1;
			int8_t m_iCurrentBlendEnable = -1;
			int8_t m_iCurrentBlendColorSrc = -1;
			int8_t m_iCurrentBlendColorDst = -1;
			int8_t m_iCurrentMultiSampleCount = -1;

			uint16_t m_iGlobalScissorX = 0;
			uint16_t m_iGlobalScissorY = 0;
			uint16_t m_iGlobalScissorWidth = 0;
			uint16_t m_iGlobalScissorHeight = 0;

			uint16_t m_iCurrScissorX = 0xffff;
			uint16_t m_iCurrScissorY = 0xffff;
			uint16_t m_iCurrScissorWidth = 0xffff;
			uint16_t m_iCurrScissorHeight = 0xffff;
								
		public:
			OpenGLES2Renderer();
			~OpenGLES2Renderer();

			AGKRenderer GetIdentifier() { return AGK_RENDERER_OPENGL_ES2; }
			const char* GetName() { return "OpenGLES"; }

			int SupportsGLSL450() { return 0; }
			int SupportsSPIRV() { return 0; }

			int Init(); // things to do before window setup
			int SetupWindow( void* param1, void* param2, unsigned int width, unsigned int height );
            int WindowResized( void* param1, void* param2, unsigned int width, unsigned int height );
			int SetScreenRecordParams( void* param1, void* param2 );
			int Setup(); // things to do after window setup
			int Swap();
			void SetPresentMode( uint32_t mode ) {}
			int ForcePresent() { return APP_SUCCESS; }
			void UpdateSurfaceSize() {}
			void CleanUp();
			void ResetInternalState();
            void* GetDevicePtr() { return 0; }
			void LosingSurface();
			void LosingFocus();

			// core
			void SetVSync( int mode );
			void SetGlobalScissor( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) { m_iGlobalScissorX = x; m_iGlobalScissorY = y; m_iGlobalScissorWidth = width; m_iGlobalScissorHeight = height; }
			void SetViewport( uint16_t x, uint16_t y, uint16_t width, uint16_t height );
			void ClearRenderTarget( float red, float green, float blue, float alpha );
			void ClearDepthBuffer();
			void SetAntialiasMode( int mode );
			void GetDataFromScreen( uint32_t **pData, int x, int y, int width, int height );
			int GetRenderPassStarted() { return 0; }

			// images commands
			bool IsImageFormatSupported( AGKImgFormat format );
			int CreateImage2D( AGKImgFormat format, uint32_t width, uint32_t height, uint8_t *data, uint32_t flags, void **outResource );
			int UpdateSubImage2D( void *pResource, uint32_t mipmapLevel, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *data );
			int CreateDepthBuffer( AGKImgFormat format, uint32_t width, uint32_t height, uint32_t flags, void **outResource );
			int DeleteImage( void *inResource, int unbind=1 );
			int SetImageSampleSettings( void *pResource, uint32_t flags );
			void SetImageMagFilter( void *pResource, uint32_t mode );
			void SetImageMinFilter( void *pResource, uint32_t mode );
			void SetImageWrapU( void *pResource, uint32_t mode );
			void SetImageWrapV( void *pResource, uint32_t mode );
			int GetImageData( void **pResource, uint8_t **pData );
			uint32_t GetImageInternalID( void *pResource );
			void ClearImageBindings();
            int OverrideTexture( uint32_t width, uint32_t height, AGKImgFormat format, void* texture, void **outResource );

			// vertex commands
			int CreateVertices( uint32_t vertexSize, void **outResource );
			int UpdateVertices( void *pResource, float *pVertexData, uint32_t offset, uint32_t numVertices );
			int UpdateIndices( void *pResource, void *pIndexData, uint32_t offset, uint32_t numIndices ); // pIndexData must be unsigned int if original indices were unsigned int
			int SetVertices( void *pResource, float *pVertexData, uint32_t numVertices );
			int SetIndices( void *pResource, uint16_t *pIndexData, uint32_t numIndices );
			int SetIndicesInt( void *pResource, uint32_t *pIndexData, uint32_t numIndices );
			int DeleteVertices( void *pResource );

			// frame buffers
			int CreateFrameBuffer( void *pColorImage, void *pDepthImage, int iForceDepth, void **outResource );
			int DeleteFrameBuffer( void *pResource );
			void SetFrameBufferIsShadow( void *pResource ) {}
			void StartShadowFrameBuffer( void *pResource ) {}
			void StartShadowFrameBuffers( void *pResource1, void *pResource2, void *pResource3, void *pResource4 ) {}
			void CompleteShadowFrameBuffer( void *pResource ) {}
			void CompleteShadowFrameBuffers( void *pResource1, void *pResource2, void *pResource3, void *pResource4 ) {}
			int BindFrameBuffer( void *pCommandBuffer, void *pResource );

			const char* GetShaderDefaultFullScreenVS();
			const char* GetShaderDefaultSpriteVS();

			AGKShader* LoadDefaultShaderColor();
			AGKShader* LoadDefaultShaderTexColor();
			AGKShader* LoadDefaultShaderFont();
			AGKShader* LoadDefaultShaderShadow();
			AGKShader* LoadDefaultShaderShadowAlpha();
			AGKShader* LoadDefaultShaderShadowBone( uint32_t numBones );
			AGKShader* LoadDefaultShaderShadowBoneAlpha( uint32_t numBones );
			AGKShader* LoadDefaultShader3DParticlesTex();
			AGKShader* LoadDefaultShader3DParticlesColor();
			AGKShader* LoadDefaultShaderVideo();
			AGKShader* LoadDefaultShaderDeviceCamera();
			AGKShader* LoadDefaultShaderARAndroid();
			AGKShader* LoadDefaultShaderARiOS();
			AGKShader* LoadDefaultShaderQuad();
			AGKShader* LoadDefaultShaderSkyBoxSun();
			AGKShader* LoadDefaultShaderSkyBoxNoSun();

			AGKShader* Make3DShader( cMesh *pMesh );
			AGKShader* MakeFinalShader( AGKShader *pBaseShader, int sunActive, int VSLights, int PSLights, int useShadows, int normalMap );
			AGKShader* LoadShader( const char *szVertexSource, const char *szPixelSource, const char *szVertexFile, const char *szPixelFile );
			AGKShader* LoadShaderSPIRV( uint32_t *pVertexSource, uint32_t sizeVS, uint32_t *pPixelSource, uint32_t sizePS );
			void ReloadShader( AGKShader *pShader );
			void DeleteShader( AGKShader *pShader );

			void SetShaderConstant( AGKShader *pShader, uint32_t index, AGKShaderConstantValue *pConstant );
			int BindShaderConstants( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKUnorderedArray<AGKShaderConstantValue*> *pAdditionalConstants, 
									 AGKDynamicArray<cImage*> *pTextures, void **inoutConstants );
			int BindShaderConstantUpdates( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKDynamicArray<cImage*> *pTextures, void **inoutConstants );
			int DeleteShaderConstants( void *pConstants ) { return APP_SUCCESS; }

			// OpenGL does not use the pCommandBuffer or inoutPipeline parameters
			int BindPipeline( void *pCommandBuffer, AGKShader *pShader, const AGKRenderState *pRenderState, const AGKVertexLayout *pVertexLayout, void **inoutPipeline );
			int DeletePipeline( void *pPipeline ) { return APP_SUCCESS; }

			int DrawVertices( void *pCommandBuffer, void *pVertices, int count=-1 );

		protected:

			int BindTexture( void *pResource, uint32_t stage );		
			int UnBindTexture( void *pResource );

			// internal image functions
			void GenerateImageMipmaps( OpenGLES2Image *pImage );

			// internal shader functions
			void InternalCompileShader( AGKShader *pShader );
			void InternalEnumerateShaderResources( AGKShader *pShader );
	};
}

#endif // _H_AGK_OPENGLES2_
