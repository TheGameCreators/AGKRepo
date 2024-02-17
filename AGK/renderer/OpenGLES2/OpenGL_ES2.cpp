#include "agk.h"
#include "zlib.h"

#include "OpenGL_ES2.h"

#ifndef GL_DEPTH_COMPONENT24
	#define GL_DEPTH_COMPONENT24 0x81A6
#endif

bool IsExtensionSupported(const char *extension)
{
	const size_t extlen = strlen(extension);
	const char *supported = NULL;
 
	supported = (char*)glGetString(GL_EXTENSIONS);
 
	// If That Failed Must Be No Extensions Supported
	if (supported == NULL) return false;
 
	// Begin Examination At Start Of String, Increment By 1 On False Match
	for (const char* p = supported; ; p++)
	{
		// Advance p Up To The Next Possible Match
		p = strstr(p, extension);
		if (p == NULL) return false;                       // No Match
 
		// Make Sure That Match Is At The Start Of The String Or That
		// The Previous Char Is A Space, Or Else We Could Accidentally
		// Match "wglFunkywglExtension" With "wglExtension"
 
		// Also, Make Sure That The Following Character Is Space Or NULL
		// Or Else "wglExtensionTwo" Might Match "wglExtension"
		if ((p==supported || p[-1]==' ') && (p[extlen]=='\0' || p[extlen]==' ')) return true; // Match
	}
}

namespace AGK
{
	int iOpenGL3 = 0;
}

#ifdef AGK_ANDROID
	EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
	EGLConfig eglConfig = 0;

	ANativeWindow* recordWindow = 0;
	EGLSurface recordSurface = EGL_NO_SURFACE;
	int recordWidth = 0;
	int recordHeight = 0;
	cObject3D* recordQuad = 0;
	cImage* recordImage = 0;
	
	int GLESCreateRendererWindow( void *ptr, int deviceWidth, int deviceHeight )
	{
		ANativeWindow* window = (ANativeWindow*) ptr;

		int EGL_DEPTH_ENCODING_NV = 0x30E2;
		int EGL_DEPTH_ENCODING_NONLINEAR_NV = 0x30E3;

		EGLint format;
		EGLint numConfigs;

		display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		eglInitialize(display, 0, 0);

		const EGLint attribs[] = {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_BLUE_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_RED_SIZE, 8,
				EGL_ALPHA_SIZE, 8,
				EGL_BUFFER_SIZE, 32,
				EGL_DEPTH_SIZE, 16,
				EGL_STENCIL_SIZE, 0,
				EGL_CONFIG_CAVEAT, EGL_NONE,
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
				EGL_DEPTH_ENCODING_NV, EGL_DEPTH_ENCODING_NONLINEAR_NV,
				EGL_NONE
		};
	
		/* Here, the application chooses the configuration it desires. In this
		 * sample, we have a very simplified selection process, where we pick
		 * the first EGLConfig that matches our criteria */
		EGLConfig allConfigs[20];
		eglChooseConfig(display, attribs, allConfigs, 20, &numConfigs);
		eglConfig = allConfigs[0];

		if ( numConfigs == 0 )
		{
    		agk::Warning( "Linear depth not supported, adjusting requested parameters" );

			const EGLint attribs[] = {
					EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
					EGL_BLUE_SIZE, 8,
					EGL_GREEN_SIZE, 8,
					EGL_RED_SIZE, 8,
					EGL_ALPHA_SIZE, 8,
					EGL_BUFFER_SIZE, 32,
					EGL_DEPTH_SIZE, 24,
					EGL_STENCIL_SIZE, 0,
					EGL_CONFIG_CAVEAT, EGL_NONE,
					EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
					EGL_NONE
			};
	
			eglChooseConfig(display, attribs, allConfigs, 20, &numConfigs);
			eglConfig = allConfigs[0];

			if ( numConfigs == 0 )
			{
				agk::Warning( "24 bit depth not supported, adjusting requested parameters" );

				const EGLint attribs[] = {
						EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
						EGL_BLUE_SIZE, 8,
						EGL_GREEN_SIZE, 8,
						EGL_RED_SIZE, 8,
						EGL_ALPHA_SIZE, 8,
						EGL_BUFFER_SIZE, 32,
						EGL_DEPTH_SIZE, 16,
						EGL_STENCIL_SIZE, 0,
						EGL_CONFIG_CAVEAT, EGL_NONE,
						EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
						EGL_NONE
				};
	
				eglChooseConfig(display, attribs, allConfigs, 20, &numConfigs);
				eglConfig = allConfigs[0];

				if ( numConfigs == 0 )
				{
					agk::Error( "Failed to find suitable render format" );
					return 0;
				}
			}
		}
		
		/*
		int i = 0;
		for ( i = 0; i < numConfigs; i++ )
		{
    		if ( i > 19 ) continue;
    		EGLint red;
    		EGLint green;
    		EGLint blue;
    		EGLint alpha;
    		EGLint depth;
    		EGLint stencil;
    		EGLint window;
    		EGLint render;
    		eglGetConfigAttrib( display, allConfigs[i], EGL_RED_SIZE, &red );
    		eglGetConfigAttrib( display, allConfigs[i], EGL_GREEN_SIZE, &green );
    		eglGetConfigAttrib( display, allConfigs[i], EGL_BLUE_SIZE, &blue );
    		eglGetConfigAttrib( display, allConfigs[i], EGL_ALPHA_SIZE, &alpha );
    		eglGetConfigAttrib( display, allConfigs[i], EGL_DEPTH_SIZE, &depth );
    		eglGetConfigAttrib( display, allConfigs[i], EGL_STENCIL_SIZE, &stencil );
    		eglGetConfigAttrib( display, allConfigs[i], EGL_SURFACE_TYPE, &window );
    		eglGetConfigAttrib( display, allConfigs[i], EGL_NATIVE_VISUAL_ID, &format );
    		eglGetConfigAttrib( display, allConfigs[i], EGL_RENDERABLE_TYPE, &render );

    		LOGI( "R: %d, G: %d, B: %d, A: %d, D: %d, W: %d, F: %d, S: %d, R: %d", red, green, blue, alpha, depth, window, format, stencil, render );
		}
		*/

		int formatIndex = 0;
				
		uString err; err.Format( "Window Width: %d Height: %d", ANativeWindow_getWidth(window), ANativeWindow_getHeight(window) );
		agk::Warning( err );

		eglGetConfigAttrib(display, eglConfig, EGL_NATIVE_VISUAL_ID, &format);
		int result = ANativeWindow_setBuffersGeometry(window, 0, 0, 0);
		//LOGI( "Result: %d", result );

		surface = eglCreateWindowSurface(display, eglConfig, window, NULL);
		while ( surface == EGL_NO_SURFACE )
		{
			uString err; err.Format( "Failed to create EGL surface: %d, trying different format", eglGetError() );
			agk::Warning( err );

			formatIndex++;
			if ( formatIndex >= numConfigs || formatIndex > 19 )
			{
				agk::Error( "Failed to find compatible format" );
				return 0;
			}

			eglConfig = allConfigs[ formatIndex ];
			surface = eglCreateWindowSurface(display, eglConfig, window, NULL);
		}

		const EGLint contextAttribList[] = {EGL_CONTEXT_CLIENT_VERSION, 2,EGL_NONE};
		context = eglCreateContext(display, eglConfig, NULL, contextAttribList);
		if ( context == EGL_NO_CONTEXT )
		{
			uString err; err.Format( "Failed to create EGL context: %d", eglGetError() );
			agk::Error( err );
			return 0;
		}

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
    		int error = eglGetError();
			uString err; err.Format( "Unable to eglMakeCurrent: %d", eglGetError() );
			agk::Error( err );
			return 0;
		}

		return 1;
	}

	void GLESSetRendererPointers( void *ptr, int deviceWidth, int deviceHeight )
	{
		ANativeWindow* window = (ANativeWindow*) ptr;
		
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if ( surface ) eglDestroySurface(display, surface);

		int result = ANativeWindow_setBuffersGeometry(window, 0, 0, 0);
		//LOGI( "Result: %d", result );

		surface = eglCreateWindowSurface(display, eglConfig, window, NULL);
		if ( surface == EGL_NO_SURFACE )
		{
			uString err; err.Format( "Failed to create EGL surface: %d", eglGetError() );
			agk::Error( err );
			return;
		}

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
		{
			uString err; err.Format( "Unable to eglMakeCurrent: %d", eglGetError() );
			agk::Error( err );
			return;
		}
	}

	// external textures are used on Android for VideoToTexture and CameraToTexture, they use GL_TEXTURE_EXTERNAL_OES instead of GL_TEXTURE_2D
	void GLESRegenerateExternalTexture( unsigned int *tex )
	{
		if ( *tex ) glDeleteTextures( 1, tex );
		glGenTextures( 1, tex );
		glBindTexture( GL_TEXTURE_EXTERNAL_OES, *tex ); // not shared with TEXTURE_2D so no need to clear binding
		glTexParameteri( GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}

	void GLESBindExternalTexture( unsigned int tex )
	{
		glBindTexture( GL_TEXTURE_EXTERNAL_OES, tex );
	}

	void GLESDeleteExternalTexture( unsigned int *tex )
	{
		if ( *tex ) glDeleteTextures( 1, tex );
		*tex = 0;
	}
#endif

#ifdef AGK_IOS
	namespace AGK
    {
        EAGLContext *context = 0;
		GLuint defaultFramebuffer = 0;
		GLuint colorRenderbuffer = 0;
		GLuint depthRenderbuffer = 0;
		UINT m_iManualResolution = 0;
    }

	// for video to OpenGLES texture
    __CVOpenGLESTextureCache *videoCache = 0;
    CVOpenGLESTextureRef videoTextureCV[2] = {0,0};
    unsigned int videoCurrTexture = 0;

    // for camera to OpenGLES texture
    __CVOpenGLESTextureCache *cameraCache = 0;
    CVOpenGLESTextureRef cameraTextureCV[2] = {0,0};
    unsigned int cameraCurrTexture = 0;

    // for AR to OpenGLES texture
    __CVOpenGLESTextureCache *ARCache = 0;
    CVOpenGLESTextureRef ARTextureCV[2] = {0,0};
    CVOpenGLESTextureRef ARTextureCV2[2] = {0,0};
    unsigned int ARCurrTexture = 0;

	int GLESCreateRendererWindow( void *ptr, int deviceWidth, int deviceHeight )
	{
		UIView* pView = (UIView*)ptr;
		CAEAGLLayer *eaglLayer = (CAEAGLLayer*)pView.layer;

        iOpenGL3 = 1;
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if ( !context )
        {
            iOpenGL3 = 0;
            agk::Warning( "OpenGL ES 3.0 not supported, trying OpenGL ES 2.0" );
            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
            if ( !context )
            {
                agk::Message( "Failed to create OpenGL ES 2.0 context" );
                agk::Sleep(2000);
                NSLog(@"Failed to create OpenGL ES 2.0 context");
                return 0;
            }
        }
        
        if ( ![EAGLContext setCurrentContext:context] )
        {
            agk::Message( "Failed to make OpenGL context current" );
            agk::Sleep(2000);
            NSLog(@"Failed to make OpenGL context current");
            return 0;
        }

		if ( !defaultFramebuffer ) glGenFramebuffers(1, &defaultFramebuffer);
		if ( !colorRenderbuffer ) glGenRenderbuffers(1, &colorRenderbuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
			    
		// iPad may try to scale down our render buffer to 320x480 
		// override this to native resolution
		CGRect oldRect = [ eaglLayer bounds ];
        CGRect rect;
			    
		rect.origin.x = 0; rect.origin.y = 0;
		rect.size.width = deviceWidth;
		rect.size.height = deviceHeight;
		[ eaglLayer setBounds:rect ];
		
        //NSLog(@"Old Rect Width: %f, Height: %f", oldRect.size.width, oldRect.size.height);
        //NSLog(@"New Rect Width: %f, Height: %f", rect.size.width, rect.size.height);
        //NSLog(@"Screen Width: %d, Screen Height: %d", deviceWidth, deviceHeight);
	    
		[context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
		
		// reset layer size so it thinks we accepted the original resolution
		[ eaglLayer setBounds:oldRect ];

		GLint renderWidth;
		GLint renderHeight;
        
        int depth24 = 0;
        if ( iOpenGL3 || IsExtensionSupported( "GL_OES_depth24" ) ) depth24 = 1;
		
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &renderWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &renderHeight);
		
		if ( !depthRenderbuffer ) glGenRenderbuffers(1, &depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, depth24 ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16, renderWidth, renderHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
		
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        
        return 1;
	}

	void GLESSetRendererPointers( void *ptr, int deviceWidth, int deviceHeight )
	{
		UIView* pView = (UIView*)ptr;
        CAEAGLLayer *eaglLayer = (CAEAGLLayer*)pView.layer;

		CGRect oldRect = [ eaglLayer bounds ];
			    
		CGRect rect;
		rect.origin.x = 0; rect.origin.y = 0;
		rect.size.width = deviceWidth;
		rect.size.height = deviceHeight;
		[ eaglLayer setBounds:rect ];
				        
		[context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
		
		[ eaglLayer setBounds:oldRect ];
	    
		GLint renderWidth;
		GLint renderHeight;
		
		//work out if we need any borders to prevent image stretching
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &renderWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &renderHeight);
        
        int depth24 = 0;
        if ( iOpenGL3 || IsExtensionSupported( "GL_OES_depth24" ) ) depth24 = 1;
		
		if ( !depthRenderbuffer ) glGenRenderbuffers(1, &depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, depth24 ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16, renderWidth, renderHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
		
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
	}

    int GLESCreateVideoImageData()
    {
        if ( !videoCache )
        {
            CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, context, NULL, &videoCache);
            if (err != noErr) {
                agk::Warning( "Failed to play video to texture, failed to create texture cache" );
                return 0;
            }
        }
        
        return 1;
    }

    int GLESHasVideoImageData()
    {
        return videoCache ? 1 : 0;
    }

    void GLESCopyVideoImageToAGKImage( CVPixelBufferRef buffer, cImage* pImage )
    {
        videoCurrTexture = 1 - videoCurrTexture;
        if ( videoTextureCV[videoCurrTexture] )
        {
            CFRelease(videoTextureCV[videoCurrTexture]);
            videoTextureCV[videoCurrTexture] = 0;
        }
        
        CVOpenGLESTextureCacheFlush(videoCache, 0);
        
        CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                     videoCache,
                                                     buffer,
                                                     NULL,
                                                     GL_TEXTURE_2D,
                                                     GL_RGBA,
                                                     (int)CVPixelBufferGetWidth(buffer),
                                                     (int)CVPixelBufferGetHeight(buffer),
                                                     GL_BGRA,
                                                     GL_UNSIGNED_BYTE,
                                                     0,
                                                     &(videoTextureCV[videoCurrTexture]));
        
        void* texID = 0;
        *((uint32_t*)&texID) = CVOpenGLESTextureGetName(videoTextureCV[videoCurrTexture]);
        pImage->OverrideTexture( texID, (int)CVPixelBufferGetWidth(buffer), (int)CVPixelBufferGetHeight(buffer), AGK_COLOR_FORMAT_RGBA_8);
    }

    void GLESReleaseVideoImageData()
    {
        if ( videoTextureCV[0] )
        {
            CFRelease(videoTextureCV[0]);
            videoTextureCV[0] = 0;
        }
        if ( videoTextureCV[1] )
        {
            CFRelease(videoTextureCV[1]);
            videoTextureCV[1] = 0;
        }
        
        if ( videoCache )
        {
            CVOpenGLESTextureCacheFlush(videoCache, 0);
            CFRelease(videoCache);
            videoCache = 0;
        }
    }

    int GLESCreateCameraImageData()
    {
        if ( !cameraCache )
        {
            CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, context, NULL, &cameraCache);
            if (err != noErr) {
                agk::Warning( "Failed to set device camera to image, failed to create texture cache" );
                return 0;
            }
        }
        
        return 1;
    }

    int GLESHasCameraImageData()
    {
        return cameraCache ? 1 : 0;
    }

    void GLESCopyCameraImageToAGKImage( CVImageBufferRef buffer, cImage* pImage )
    {
        cameraCurrTexture = 1 - cameraCurrTexture;
        if ( cameraTextureCV[cameraCurrTexture] )
        {
            CFRelease(cameraTextureCV[cameraCurrTexture]);
            cameraTextureCV[cameraCurrTexture] = 0;
        }
        
        CVOpenGLESTextureCacheFlush(cameraCache, 0);
        
        CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                     cameraCache,
                                                     buffer,
                                                     NULL,
                                                     GL_TEXTURE_2D,
                                                     GL_RGBA,
                                                     (int)CVPixelBufferGetWidth(buffer),
                                                     (int)CVPixelBufferGetHeight(buffer),
                                                     GL_BGRA,
                                                     GL_UNSIGNED_BYTE,
                                                     0,
                                                     &(cameraTextureCV[cameraCurrTexture]));
        
        void* texID = 0;
        *((uint32_t*)&texID) = CVOpenGLESTextureGetName((cameraTextureCV[cameraCurrTexture]));
        pImage->OverrideTexture(texID, (int)CVPixelBufferGetWidth(buffer), (int)CVPixelBufferGetHeight(buffer), AGK_COLOR_FORMAT_RGBA_8);
    }

    void GLESReleaseCameraImageData()
    {
        if ( cameraTextureCV[0] )
        {
            CFRelease(cameraTextureCV[0]);
            cameraTextureCV[0] = 0;
        }
        if ( cameraTextureCV[1] )
        {
            CFRelease(cameraTextureCV[1]);
            cameraTextureCV[1] = 0;
        }
        
        if ( cameraCache )
        {
            CVOpenGLESTextureCacheFlush(cameraCache, 0);
            CFRelease(cameraCache);
            cameraCache = 0;
        }
    }

    int GLESCreateARImageData()
    {
        if ( !ARCache )
        {
            CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, context, NULL, &ARCache);
            if (err != noErr) {
                agk::Warning( "Failed to create AR texture cache" );
                return 0;
            }
        }
        
        return 1;
    }

    int GLESHasARImageData()
    {
        return ARCache ? 1 : 0;
    }

    void GLESCopyARImageToAGKImage( CVPixelBufferRef buffer, cImage* pImageY, cImage* pImageUV )
    {
        ARCurrTexture = 1 - ARCurrTexture;
        if ( ARTextureCV[ARCurrTexture] )
        {
            CFRelease(ARTextureCV[ARCurrTexture]);
            ARTextureCV[ARCurrTexture] = 0;
        }
        
        if ( ARTextureCV2[ARCurrTexture] )
        {
            CFRelease(ARTextureCV2[ARCurrTexture]);
            ARTextureCV2[ARCurrTexture] = 0;
        }
        
        CVOpenGLESTextureCacheFlush(ARCache, 0);
        
        int width = (int)CVPixelBufferGetWidthOfPlane(buffer,0);
        int height = (int)CVPixelBufferGetHeightOfPlane(buffer,0);
        
        // Lumninace texture
        CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                     ARCache,
                                                     buffer,
                                                     NULL,
                                                     GL_TEXTURE_2D,
                                                     GL_LUMINANCE,
                                                     width,
                                                     height,
                                                     GL_LUMINANCE,
                                                     GL_UNSIGNED_BYTE,
                                                     0,
                                                     &(ARTextureCV[ARCurrTexture]));
        
        void* texID = 0;
        *((uint32_t*)&texID) = CVOpenGLESTextureGetName( ARTextureCV[ARCurrTexture] );
        pImageY->OverrideTexture( texID, width, height, AGK_COLOR_FORMAT_R_8 );
        
        width = (int)CVPixelBufferGetWidthOfPlane(buffer,1);
        height = (int)CVPixelBufferGetHeightOfPlane(buffer,1);
        
        CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                     ARCache,
                                                     buffer,
                                                     NULL,
                                                     GL_TEXTURE_2D,
                                                     GL_LUMINANCE_ALPHA,
                                                     width,
                                                     height,
                                                     GL_LUMINANCE_ALPHA,
                                                     GL_UNSIGNED_BYTE,
                                                     1,
                                                     &(ARTextureCV2[ARCurrTexture]));
        
        texID = 0;
        *((uint32_t*)&texID) = CVOpenGLESTextureGetName( ARTextureCV2[ARCurrTexture] );
        pImageUV->OverrideTexture( texID, width, height, AGK_COLOR_FORMAT_RG_8 );
    }

    void GLESReleaseARImageData()
    {
        if ( ARTextureCV[0] ) CFRelease(ARTextureCV[0]);
        if ( ARTextureCV[1] ) CFRelease(ARTextureCV[1]);
        if ( ARTextureCV2[0] ) CFRelease(ARTextureCV2[0]);
        if ( ARTextureCV2[1] ) CFRelease(ARTextureCV2[1]);
        ARTextureCV[0] = 0;
        ARTextureCV[1] = 0;
        ARTextureCV2[0] = 0;
        ARTextureCV2[1] = 0;
        
        if ( ARCache )
        {
            CVOpenGLESTextureCacheFlush(ARCache, 0);
            CFRelease(ARCache);
            ARCache = 0;
        }
    }
#endif

#ifdef AGK_RASPPI
	void *g_pWindow = 0;
	void *g_pSurface = 0;
	void *g_pContext = 0;
	
	struct egldata
	{
		void *display;
		void *surface;
		void *context;
		void *reserved1;
		void *reserved2;
	};

	void SetRendererPointers( void *ptr )
	{
		egldata *ePtr = (egldata*)ptr;
		g_pWindow = ePtr->display;
		g_pSurface = ePtr->surface;
		g_pContext = ePtr->context;
	}

	int GetSurfaceWidth()
	{
		int width;
		eglQuerySurface(g_pWindow, g_pSurface, EGL_WIDTH, &width);
		return width;
	}

	int GetSurfaceHeight()
	{
		int height;
		eglQuerySurface(g_pWindow, g_pSurface, EGL_HEIGHT, &height);
		return height;
	}
#endif

#ifdef AGK_HTML5
	namespace AGK
	{
		extern GLFWwindow *g_pWindow;
	}

	int GLESCreateRendererWindow( void *ptr, int deviceWidth, int deviceHeight ) { return 1; }
	void GLESSetRendererPointers( void *ptr, int deviceWidth, int deviceHeight ) {}
#endif




OpenGLES2Renderer::OpenGLES2Renderer()
{

}

OpenGLES2Renderer::~OpenGLES2Renderer()
{

}

void OpenGLES2Renderer::ResetInternalState()
{
	for( int i = 0; i < AGK_OPENGLES2_MAX_TEXTURE_STAGES; i++ ) m_iCurrTexture[ i ] = 0;
	m_iCurrentBoundVBO = 0;
	m_iCurrentBoundIndexVBO = 0;

	m_pCurrFrameBuffer = 0;
	m_iCurrShader = 0;
	m_pCurrAGKShader = 0;
	m_pCurrVertexLayout = 0;

	m_fCurrentDepthBias = -1;
	m_fCurrentDepthMin = -1;
	m_fCurrentDepthMax = -1;
	
	m_iCurrActiveTexture = -1;
	m_iScissorEnabled = -1;
	m_iCurrentCullMode = -1;	
	m_iCurrentWindingOrder = -1;
	m_iCurrentDepthEnable = -1;
	m_iCurrentDepthFunc = -1;
	m_iCurrentDepthClampEnable = -1;
	m_iCurrentDepthWriteEnable = -1;
	m_iCurrentDepthBiasEnable = -1;
	m_iCurrentStencilEnable = -1;
	m_iCurrentBlendEnable = -1;
	m_iCurrentBlendColorSrc = -1;
	m_iCurrentBlendColorDst = -1;
	m_iCurrentMultiSampleCount = -1;
}

int OpenGLES2Renderer::Init()
{
    m_iFBOFlipped = 1;
    m_iTopLeftOrigin = 0;
    m_iSupportsNPOT = 1; // OpenGLES 2 guarantees support for non-power of two textures
    m_iSupportsMipmapNPOT = 0;
    m_iClipSpaceZeroOne = 0; // OpenGLES is -1 to 1
    m_iInvertedDepth = 0;
    m_iSupportsDepthClamp = 0;

	return APP_SUCCESS;
}

int OpenGLES2Renderer::SetupWindow( void* param1, void* param2, unsigned int width, unsigned int height )
{
	if ( !GLESCreateRendererWindow( param1, width, height ) ) return APP_ERROR_GENERAL;
	    
#if defined(AGK_IOS)
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_iSurfaceWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_iSurfaceHeight);
#elif defined(AGK_ANDROID)
	eglQuerySurface(display, surface, EGL_WIDTH, &m_iSurfaceWidth);
	eglQuerySurface(display, surface, EGL_HEIGHT, &m_iSurfaceHeight);
#elif defined(AGK_HTML5)
	glfwGetFramebufferSize( g_pWindow, &m_iSurfaceWidth, &m_iSurfaceHeight );
#else
	#error Unknown OPENGLES platform
#endif

	AppInfo( "Surface Size: %dx%d", m_iSurfaceWidth, m_iSurfaceHeight );
    
    return APP_SUCCESS;
}

int OpenGLES2Renderer::WindowResized( void* param1, void* param2, unsigned int width, unsigned int height )
{
    GLESSetRendererPointers( param1, width, height );
        
#if defined(AGK_IOS)
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_iSurfaceWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_iSurfaceHeight);
#elif defined(AGK_ANDROID)
	eglQuerySurface(display, surface, EGL_WIDTH, &m_iSurfaceWidth);
	eglQuerySurface(display, surface, EGL_HEIGHT, &m_iSurfaceHeight);
#elif defined(AGK_HTML5)
	glfwGetFramebufferSize( g_pWindow, &m_iSurfaceWidth, &m_iSurfaceHeight );
#else
	#error Unknown OPENGLES platform
#endif
    
    return APP_SUCCESS;
}

int OpenGLES2Renderer::SetScreenRecordParams( void* param1, void* param2 )
{
#if defined(AGK_ANDROID)
	recordWindow = (ANativeWindow*) param1;

	if ( recordSurface ) eglDestroySurface( display, recordSurface );
	recordSurface = 0;

	if ( recordImage ) delete recordImage;
	recordImage = 0;

	if ( recordQuad ) delete recordQuad;
	recordQuad = 0;
		
	if ( recordWindow )
	{		
		recordSurface = eglCreateWindowSurface(display, eglConfig, recordWindow, NULL);
		if ( recordSurface == EGL_NO_SURFACE )
		{
			uString err; err.Format( "Failed to create record surface: %d", eglGetError() );
			agk::Error( err );
			return APP_ERROR_GENERAL;
		}
		
		eglQuerySurface(display, recordSurface, EGL_WIDTH, &recordWidth);
		eglQuerySurface(display, recordSurface, EGL_HEIGHT, &recordHeight);
			
		recordImage = new cImage();
		recordImage->CreateBlankImage( m_iSurfaceWidth, m_iSurfaceHeight, 0, 0 );
		recordImage->SetWrapU( 0 );
		recordImage->SetWrapV( 0 );
		recordImage->SetMinFilter( 1 );
		recordImage->SetMagFilter( 1 );

		recordQuad = new cObject3D();
		recordQuad->CreateQuad();
		recordQuad->SetImage( recordImage );
	}

	return APP_SUCCESS;
#else
	return APP_ERROR_GENERAL;
#endif
}

void OpenGLES2Renderer::LosingSurface() 
{
#if defined(AGK_IOS)
    // do nothing
#elif defined(AGK_ANDROID)
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	if ( surface ) eglDestroySurface(display, surface);
	surface = EGL_NO_SURFACE;
#elif defined(AGK_HTML5)
	// do nothing
#else
	#error Unknown OPENGLES platform
#endif
}

void OpenGLES2Renderer::LosingFocus() 
{
	glFinish(); // this fixes a crash on some Android Adreno GPUs, can't hurt to do it on everything
}

int OpenGLES2Renderer::Setup()
{
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_iMaxTextureSize);
    
    const char* extensions = (const char*) glGetString( GL_EXTENSIONS );
    if ( IsExtensionSupported( "GL_OES_depth_texture" ) ) m_iSupportsDepthTexture = 1;
    if ( IsExtensionSupported( "GL_OES_depth24" ) ) m_iSupportsDepth24 = 1;
    if ( IsExtensionSupported( "GL_NV_depth_nonlinear" ) ) m_iSupportsDepthNonLinear = 1;
    // something is wrong on iOS with OpenGL ES 2.0 and the UINT index extension, objects become corrupted
#ifndef AGK_IOS
    if ( IsExtensionSupported( "GL_OES_element_index_uint" ) ) m_iSupportsIntIndices = 1;
#endif
    
	if ( iOpenGL3 )
    {
        m_iSupportsDepthTexture = 1;
        m_iSupportsDepth24 = 1;
        m_iSupportsIntIndices = 1;
    }

	int range[2];
	int precision;
	glGetShaderPrecisionFormat( GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, range, &precision );
	if ( range[0] > 0 ) m_iSupportsPSHighP = 1;
	else m_iSupportsPSHighP = 0;

	glGetIntegerv( GL_MAX_VERTEX_UNIFORM_VECTORS, &m_iMaxVSUniforms );
	glGetIntegerv( GL_MAX_FRAGMENT_UNIFORM_VECTORS, &m_iMaxPSUniforms );
	glGetIntegerv( GL_MAX_VARYING_VECTORS, &m_iMaxVaryings ); m_iMaxVaryings *= 4;

	// not using memory alignment
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	glClearColor( 0, 0, 0, 0 );
	glClearDepthf( m_iInvertedDepth ? 0.0f : 1.0f );

	agk::ClearScreen();
    agk::Swap();
	agk::ClearScreen();

	return APP_SUCCESS;
}

int OpenGLES2Renderer::Swap()
{
	GLenum err = glGetError();
	if ( err != GL_NO_ERROR )
	{
		if ( err == GL_OUT_OF_MEMORY )
		{
			agk::Error( "Ran out of GPU memory, try using smaller or fewer images" );
			throw 1;
		}
	}
	
#if defined(AGK_IOS)
	[context presentRenderbuffer:GL_RENDERBUFFER];
#elif defined(AGK_ANDROID)
	if ( display != 0 && surface != 0 ) 
	{
		if ( recordSurface )
		{
			// copy back buffer to texture
			BindTexture( recordImage->GetInternalResource(), 0 );
			glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_iSurfaceWidth, m_iSurfaceHeight, 0 );
			eglSwapBuffers(display, surface);

			// draw texture to recording surface
			eglMakeCurrent(display, recordSurface, recordSurface, context);
			SetViewport( 0, 0, recordWidth, recordHeight );
			AGKShader::g_varInvert.SetFloat( -1.0 );
			recordQuad->Draw();
			eglSwapBuffers(display, recordSurface);
			
			// switch back to main surface
			eglMakeCurrent(display, surface, surface, context);
			SetViewport( 0, 0, m_iSurfaceWidth, m_iSurfaceHeight );
			AGKShader::g_varInvert.SetFloat( 1.0 );
		}
		else
		{
			eglSwapBuffers(display, surface);
		}		
	}
#elif defined(AGK_HTML5)
    glfwSwapBuffers(g_pWindow);
#elif defined(AGK_RASPPI)
	eglSwapBuffers(g_pWindow,g_pSurface);
#else
    #error unrecognised platform
#endif
    
	return APP_SUCCESS;
}

void OpenGLES2Renderer::CleanUp()
{
#ifdef AGK_ANDROID
	if (display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) eglDestroyContext( display, context );
        if (surface != EGL_NO_SURFACE) eglDestroySurface( display, surface);
        eglTerminate(display);
    }
    
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
#endif
}

void OpenGLES2Renderer::SetVSync( int mode )
{
#if defined(AGK_IOS)
	// do nothing
#elif defined(AGK_ANDROID)
	eglSwapInterval(display, mode);
#elif defined(AGK_HTML5)
    // never called
#elif defined(AGK_RASPPI)
    #error todo
#else
    #error unrecognised platform
#endif
}

void OpenGLES2Renderer::SetViewport( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) 
{ 
	glViewport( x, y, width, height );
}

void OpenGLES2Renderer::ClearRenderTarget( float red, float green, float blue, float alpha )
{
	// depth write must be enabled to clear depth
	if ( m_iCurrentDepthWriteEnable != 1 ) glDepthMask( GL_TRUE );
	m_iCurrentDepthWriteEnable = 1;

	if ( m_iScissorEnabled != 0 ) glDisable( GL_SCISSOR_TEST );
	m_iScissorEnabled = 0;

	glClearColor( red, green, blue, alpha );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

void OpenGLES2Renderer::ClearDepthBuffer()
{
	// depth write must be enabled to clear depth
	if ( m_iCurrentDepthWriteEnable != 1 ) glDepthMask( GL_TRUE );
	m_iCurrentDepthWriteEnable = 1;

	if ( m_iScissorEnabled != 0 ) glDisable( GL_SCISSOR_TEST );
	m_iScissorEnabled = 0;

	glClear( GL_DEPTH_BUFFER_BIT );
}

void OpenGLES2Renderer::SetAntialiasMode( int mode )
{
	// do nothing on mobile
	/*
	if ( mode == 1 ) glEnable( GL_MULTISAMPLE );
	else glDisable( GL_MULTISAMPLE );
	*/
}

void OpenGLES2Renderer::GetDataFromScreen( uint32_t **pData, int x, int y, int width, int height )
{
	*pData = new uint32_t[ width*height ];
	glReadPixels( x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pData );

	// flip the image as OpenGL renders upside down
	uint32_t *pRow = new uint32_t[ width ];
	for ( int y = 0; y < height/2; y++ )
	{
		uint32_t index = y*width;
		uint32_t index2 = (height-y-1)*width;

		memcpy( pRow, (*pData) + index, width*sizeof(uint32_t) );
		memcpy( (*pData) + index, (*pData) + index2, width*sizeof(uint32_t) );
		memcpy( (*pData) + index2, pRow, width*sizeof(uint32_t) );
	}

	delete [] pRow;
}

bool OpenGLES2Renderer::IsImageFormatSupported( AGKImgFormat format )
{
	switch( format )
	{
		case AGK_COLOR_FORMAT_R_8:		return true;
		case AGK_COLOR_FORMAT_RG_8:		return false;
		case AGK_COLOR_FORMAT_RGBA_8:	return true;
		case AGK_COLOR_FORMAT_R_F16:	return false;
		case AGK_COLOR_FORMAT_RG_F16:	return false;
		case AGK_COLOR_FORMAT_RGBA_F16: return false;
		case AGK_COLOR_FORMAT_R_F32:	return false;
		case AGK_COLOR_FORMAT_RG_F32:	return false;
		case AGK_COLOR_FORMAT_RGBA_F32: return false;
										
		case AGK_DEPTH_FORMAT_16_INT:	return true;
		case AGK_DEPTH_FORMAT_24_INT:	return true;
		case AGK_DEPTH_FORMAT_32_FLOAT:	return false;
		default: 
		{
			AppError( g_AppErrorDescs[APP_ERROR_IMAGE_FORMAT_NOT_RECOGNISED].szDesc, format );
			return false;
		}
	}
}

int OpenGLES2Renderer::BindTexture( void *pResource, uint32_t stage )
{
	if ( stage >= AGK_OPENGLES2_MAX_TEXTURE_STAGES ) return APP_ERROR_UNKNOWN;

	if ( pResource == 0 )
	{
		if ( m_iCurrTexture[stage] != 0 )
		{
			if ( stage != m_iCurrActiveTexture )
			{
				glActiveTexture( GL_TEXTURE0 + stage );
				m_iCurrActiveTexture = stage;
			}

			glBindTexture( GL_TEXTURE_2D, 0 );
			m_iCurrTexture[stage] = 0;
		}
	}
	else
	{
		OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;

		// check if we need to change texture
		if ( pImage->m_iTextureID == m_iCurrTexture[stage] ) return APP_SUCCESS;

		if ( stage != m_iCurrActiveTexture )
		{
			glActiveTexture( GL_TEXTURE0 + stage );
			m_iCurrActiveTexture = stage;
		}
		
		glBindTexture( GL_TEXTURE_2D, pImage->m_iTextureID );
		m_iCurrTexture[stage] = pImage->m_iTextureID;
	}

	return APP_SUCCESS;
}

int OpenGLES2Renderer::UnBindTexture( void *pResource )
{
	if ( !pResource ) return APP_SUCCESS;
	OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;

	for ( int stage = 0; stage < AGK_OPENGLES2_MAX_TEXTURE_STAGES; stage++ )
	{
		if ( pImage->m_iTextureID == m_iCurrTexture[stage] ) 
		{
			glActiveTexture( GL_TEXTURE0 + stage );
			glBindTexture( GL_TEXTURE_2D, 0 );
			m_iCurrTexture[stage] = 0;
			m_iCurrActiveTexture = stage;
		}
	}
	
	return APP_SUCCESS;
}

int OpenGLES2Renderer::CreateImage2D( AGKImgFormat format, uint32_t width, uint32_t height, uint8_t *data, uint32_t flags, void **outResource )
{
	uint32_t glformat = GL_RGBA;
	uint32_t glbytesize = GL_UNSIGNED_BYTE;

	if ( format == AGK_COLOR_FORMAT_R_8 ) glformat = GL_ALPHA;
    else if ( format == AGK_COLOR_FORMAT_RG_8 ) glformat = GL_LUMINANCE_ALPHA;

	OpenGLES2Image *pResource = (OpenGLES2Image*) *outResource;
	if ( !pResource )
	{
		pResource = new OpenGLES2Image();
		glGenTextures( 1, &pResource->m_iTextureID );
		*outResource = pResource;
	}
	pResource->m_iWidth = width;
	pResource->m_iHeight = height;
	pResource->m_iFormat = glformat;
	pResource->m_iByteSize = glbytesize;
	pResource->m_iFlags = 0;

	if ( flags & AGK_IMAGE_GEN_MIPMAP ) pResource->m_iFlags |= AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP;
	else if ( flags & AGK_IMAGE_USE_MIPMAP ) pResource->m_iFlags |= AGK_IMAGE_USE_MIPMAP;

	BindTexture( pResource, 0 );
	
	glTexImage2D(GL_TEXTURE_2D, 0, glformat, width, height, 0, glformat, glbytesize, data );

	if ( (flags & AGK_IMAGE_GEN_MIPMAP) == AGK_IMAGE_GEN_MIPMAP ) glGenerateMipmap(GL_TEXTURE_2D);
		
	if ( flags & AGK_IMAGE_WRAP_U_REPEAT ) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	else glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	if ( flags & AGK_IMAGE_WRAP_U_REPEAT ) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	else glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if ( flags & AGK_IMAGE_MAG_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if ( pResource->m_iFlags & AGK_IMAGE_USE_MIPMAP )
	{
		if ( flags & AGK_IMAGE_MIN_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		if ( flags & AGK_IMAGE_MIN_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	
	return APP_SUCCESS;
}

int OpenGLES2Renderer::UpdateSubImage2D( void *pResource, uint32_t mipmapLevel, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *data )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;

	OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;

	BindTexture( pResource, 0 );

	if ( !(pImage->m_iFlags & AGK_IMAGE_USE_MIPMAP) && mipmapLevel > 0 ) ReportAndReturnErrorID( APP_ERROR_VK_INVALID_IMAGE_MIP_LEVEL, mipmapLevel, 1 );

	glTexSubImage2D(GL_TEXTURE_2D, mipmapLevel, x, y, width, height, pImage->m_iFormat, pImage->m_iByteSize, data );

	if ( pImage->m_iFlags & AGK_IMAGE_GEN_MIPMAP ) glGenerateMipmap(GL_TEXTURE_2D);
	
	return APP_SUCCESS;
}

int OpenGLES2Renderer::CreateDepthBuffer( AGKImgFormat format, uint32_t width, uint32_t height, uint32_t flags, void **outResource )
{
	OpenGLES2Image *pResource = (OpenGLES2Image*) *outResource;
	if ( !pResource )
	{
		pResource = new OpenGLES2Image();
		glGenTextures( 1, &pResource->m_iTextureID );
		*outResource = pResource;
	}
	pResource->m_iFlags = flags | AGK_GLES2_IMAGE_IS_DEPTH;
	pResource->m_iFlags &= ~(AGK_IMAGE_WRAP_U_REPEAT | AGK_IMAGE_WRAP_V_REPEAT | AGK_IMAGE_USE_MIPMAP | AGK_IMAGE_GEN_MIPMAP);

	BindTexture( pResource, 0 );
		
	uint32_t glformat = GL_DEPTH_COMPONENT;
	uint32_t glformat2 = GL_DEPTH_COMPONENT;
	uint32_t glsize = GL_UNSIGNED_INT;
	if ( !m_iSupportsDepth24 )
    {
        glsize = GL_UNSIGNED_SHORT;
        if ( iOpenGL3 ) glformat = GL_DEPTH_COMPONENT16;
    }
    else
    {
        if ( iOpenGL3 ) glformat = GL_DEPTH_COMPONENT24;
    }
	
	glTexImage2D( GL_TEXTURE_2D, 0, glformat, width, height, 0, glformat2, glsize, 0 );

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if ( pResource->m_iFlags & AGK_IMAGE_MAG_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if ( pResource->m_iFlags & AGK_IMAGE_MIN_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return APP_SUCCESS;
}

int OpenGLES2Renderer::DeleteImage( void *inResource, int unbind )
{
	if ( !inResource ) return APP_SUCCESS;

	if ( unbind ) UnBindTexture( inResource );

	OpenGLES2Image *pImage = (OpenGLES2Image*) inResource;
	if ( pImage->m_iTextureID != 0 ) 
	{
		glDeleteTextures( 1, &pImage->m_iTextureID );
	}
	delete pImage;

	return APP_SUCCESS;
}

int OpenGLES2Renderer::OverrideTexture( uint32_t width, uint32_t height, AGKImgFormat format, void* texture, void **outResource )
{
    uint32_t glformat = GL_RGBA;
    uint32_t glbytesize = GL_UNSIGNED_BYTE;

    if ( format == AGK_COLOR_FORMAT_R_8 ) glformat = GL_ALPHA;
    else if ( format == AGK_COLOR_FORMAT_RG_8 ) glformat = GL_LUMINANCE_ALPHA;
    
    OpenGLES2Image *pResource = (OpenGLES2Image*) *outResource;
    if ( !pResource )
    {
        pResource = new OpenGLES2Image();
        *outResource = pResource;
        
        pResource->m_iWidth = width;
        pResource->m_iHeight = height;
        pResource->m_iFormat = glformat;
        pResource->m_iByteSize = glbytesize;
        pResource->m_iFlags = 0;
    }
    
    pResource->m_iTextureID = *((uint32_t*)&texture);

    return APP_SUCCESS;
}

void OpenGLES2Renderer::ClearImageBindings()
{
	for ( int stage = 0; stage < AGK_OPENGLES2_MAX_TEXTURE_STAGES; stage++ )
	{
		glActiveTexture( GL_TEXTURE0 + stage );
		glBindTexture( GL_TEXTURE_2D, 0 );
		m_iCurrTexture[stage] = 0;
	}

	glActiveTexture( GL_TEXTURE0 );
	m_iCurrActiveTexture = -1;
}

int OpenGLES2Renderer::SetImageSampleSettings( void *pResource, uint32_t flags )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;

	BindTexture( pResource, 0 );

	if ( flags & AGK_IMAGE_GEN_MIPMAP ) flags |= AGK_IMAGE_USE_MIPMAP;

	pImage->m_iFlags &= ~(AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP);
	if ( flags & AGK_IMAGE_USE_MIPMAP ) pImage->m_iFlags |= AGK_IMAGE_USE_MIPMAP;
	if ( flags & AGK_IMAGE_GEN_MIPMAP )
	{
		pImage->m_iFlags |= AGK_IMAGE_GEN_MIPMAP;
		glGenerateMipmap( GL_TEXTURE_2D );
	}
		
	if ( flags & AGK_IMAGE_WRAP_U_REPEAT ) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	else glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	if ( flags & AGK_IMAGE_WRAP_U_REPEAT ) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	else glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if ( flags & AGK_IMAGE_MAG_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if ( pImage->m_iFlags & AGK_IMAGE_USE_MIPMAP )
	{
		if ( flags & AGK_IMAGE_MIN_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		if ( flags & AGK_IMAGE_MIN_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	return APP_SUCCESS;
}

void OpenGLES2Renderer::SetImageMagFilter( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;
	BindTexture( pResource, 0 );

	if ( mode == 0 ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenGLES2Renderer::SetImageMinFilter( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;
	BindTexture( pResource, 0 );

	if ( mode == 0 ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if ( pImage->m_iFlags & AGK_IMAGE_USE_MIPMAP )
	{
		if ( mode == 0 ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		if ( mode == 0 ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void OpenGLES2Renderer::SetImageWrapU( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;
	BindTexture( pResource, 0 );

	if ( mode != 0 )
	{
		OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;

		int width = pImage->m_iWidth;
		if ( (width & (width-1)) != 0 ) // check power of 2
		{
			agk::Error( "Cannot set image wrap mode to repeat on an image that is not a power of two in size when using OpenGLES2" );
			return;
		}
	}

	if ( mode == 0 ) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
}

void OpenGLES2Renderer::SetImageWrapV( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;
	BindTexture( pResource, 0 );

	if ( mode != 0 )
	{
		OpenGLES2Image *pImage = (OpenGLES2Image*) pResource;

		int height = pImage->m_iHeight;
		if ( (height & (height-1)) != 0 ) // check power of 2
		{
			agk::Error( "Cannot set image wrap mode to repeat on an image that is not a power of two in size when using OpenGLES2" );
			return;
		}
	}
	
	if ( mode == 0 ) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void OpenGLES2Renderer::GenerateImageMipmaps( OpenGLES2Image *pImage )
{
	if ( !(pImage->m_iFlags & AGK_IMAGE_GEN_MIPMAP) ) return;

	BindTexture( pImage, 0 );
	glGenerateMipmap(GL_TEXTURE_2D);
}

int OpenGLES2Renderer::GetImageData( void **pResource, uint8_t **pData )
{
	if ( !pData ) return 0;
	if ( !pResource ) return 0;
	
	OpenGLES2Image *pImage = (OpenGLES2Image*) *pResource;
	if ( !pImage ) return 0;

	if ( pImage->m_iFormat == GL_ALPHA )
	{
		// can't get GL_ALPHA or depth textures back from the GPU on OpenGLES 2.0
		return 0;
	}
	
	uint32_t iTexID = pImage->m_iTextureID;
	if ( iTexID == 0 ) return 0;
	
	BindTexture( pImage, 0 );
	
	*pData = new unsigned char[ pImage->m_iWidth * pImage->m_iHeight * 4 ];

	BindFrameBuffer( 0, 0 );
	
	GLuint framebuf;
	glGenFramebuffers(1, &framebuf);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuf);
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iTexID, 0 );
	glReadPixels( 0, 0, pImage->m_iWidth, pImage->m_iHeight, GL_RGBA, GL_UNSIGNED_BYTE, *pData );
	
	// can't call BindFrameBuffer( 0, 0 ) here as AGK will think has already been done
#ifdef AGK_IOS
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer );
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
#else
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
#endif

	glDeleteFramebuffers( 1, &framebuf );

	// should only be true on Android
	if ( agk::IsSGX540GPU() )
	{
		// SGX540 doesn't like glFramebufferTexture2D it will fail after a certain number of calls
		// only way around it is to delete the texture that was used and recreate it
		uint32_t flags = pImage->m_iFlags;
		DeleteImage( *pResource );
		*pResource = 0;
		if ( !cImage::IsLosingContext() )
		{
			CreateImage2D( AGK_COLOR_FORMAT_RGBA_8, pImage->m_iWidth, pImage->m_iHeight, *pData, flags, pResource );
		}
	}

	return pImage->m_iWidth * pImage->m_iHeight * 4;
}

uint32_t OpenGLES2Renderer::GetImageInternalID( void *pResource )
{
	if ( !pResource ) return 0;
	
	OpenGLES2Image *pImage = (OpenGLES2Image*)pResource;
	return pImage->m_iTextureID;
}

// vertex commands
int OpenGLES2Renderer::CreateVertices( uint32_t vertexSize, void **outResource )
{
	OpenGLES2Vertices *outVertices = (OpenGLES2Vertices*) *outResource;
	if ( outVertices )
	{
		if ( vertexSize == outVertices->m_iVertexSize ) return APP_SUCCESS;
		
		DeleteVertices( outVertices );
	}

	outVertices = new OpenGLES2Vertices();
	*outResource = outVertices;

	outVertices->m_iVertexSize = vertexSize;
	outVertices->m_iNumIndices = 0;
	outVertices->m_iNumVertices = 0;
		
	return APP_SUCCESS;
}

int OpenGLES2Renderer::UpdateVertices( void *pResource, float *pVertexData, uint32_t offset, uint32_t numVertices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pVertexData ) return APP_ERROR_UNKNOWN;

	OpenGLES2Vertices *pVertices = (OpenGLES2Vertices*) pResource;
	if ( offset + numVertices > pVertices->m_iNumVertices ) ReportAndReturnErrorID( APP_ERROR_VK_VERTEX_UPDATE_OUT_OF_BOUNDS, offset, numVertices, pVertices->m_iNumVertices );

	if ( m_iCurrentBoundVBO != pVertices->m_iVertexVBO )
	{
		glBindBuffer( GL_ARRAY_BUFFER, pVertices->m_iVertexVBO );
		m_iCurrentBoundVBO = pVertices->m_iVertexVBO;
		m_iVBOPtrSetup = 0;
	}
	glBufferSubData( GL_ARRAY_BUFFER, offset*pVertices->m_iVertexSize, numVertices*pVertices->m_iVertexSize, pVertexData );

	return APP_SUCCESS;
}

// pIndexData must be unsigned int if original indices were unsigned int, and unsigned short if the original indices were unsigned short
int OpenGLES2Renderer::UpdateIndices( void *pResource, void *pIndexData, uint32_t offset, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	OpenGLES2Vertices *pVertices = (OpenGLES2Vertices*) pResource;
	if ( offset + numIndices > pVertices->m_iNumIndices ) ReportAndReturnErrorID( APP_ERROR_VK_INDEX_UPDATE_OUT_OF_BOUNDS, offset, numIndices, pVertices->m_iNumIndices );

	uint32_t indexSize = sizeof(unsigned short);
	if ( pVertices->m_iFlags & AGK_GLES2_VERTICES_INDICES_INT ) indexSize = sizeof(unsigned int);

	if ( m_iCurrentBoundIndexVBO != pVertices->m_iIndexVBO )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pVertices->m_iIndexVBO );
		m_iCurrentBoundIndexVBO = pVertices->m_iIndexVBO;
	}
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, offset*indexSize, numIndices*indexSize, pIndexData );

	return APP_SUCCESS;
}

int OpenGLES2Renderer::SetVertices( void *pResource, float *pVertexData, uint32_t numVertices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pVertexData ) return APP_ERROR_UNKNOWN;

	OpenGLES2Vertices *pVertices = (OpenGLES2Vertices*) pResource;

	if ( pVertices->m_iVertexVBO == 0 ) glGenBuffers( 1, &pVertices->m_iVertexVBO );
	pVertices->m_iNumVertices = numVertices;
	if ( m_iCurrentBoundVBO != pVertices->m_iVertexVBO )
	{
		glBindBuffer( GL_ARRAY_BUFFER, pVertices->m_iVertexVBO );
		m_iCurrentBoundVBO = pVertices->m_iVertexVBO;
		m_iVBOPtrSetup = 0;
	}
	//glBufferData( GL_ARRAY_BUFFER, numVertices*pVertices->m_iVertexSize, NULL, GL_STATIC_DRAW );
	glBufferData( GL_ARRAY_BUFFER, numVertices*pVertices->m_iVertexSize, pVertexData, GL_STATIC_DRAW );

	return APP_SUCCESS;
}

int OpenGLES2Renderer::SetIndices( void *pResource, uint16_t *pIndexData, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	OpenGLES2Vertices *pVertices = (OpenGLES2Vertices*) pResource;
	if ( pVertices->m_iIndexVBO == 0 ) glGenBuffers( 1, &pVertices->m_iIndexVBO );
	pVertices->m_iNumIndices = numIndices;

	pVertices->m_iFlags &= ~AGK_GLES2_VERTICES_INDICES_INT;
	
	if ( m_iCurrentBoundIndexVBO != pVertices->m_iIndexVBO )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pVertices->m_iIndexVBO );
		m_iCurrentBoundIndexVBO = pVertices->m_iIndexVBO;
	}
	//glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint16_t), NULL, GL_STATIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint16_t), pIndexData, GL_STATIC_DRAW );

	return APP_SUCCESS;
}

int OpenGLES2Renderer::SetIndicesInt( void *pResource, uint32_t *pIndexData, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	OpenGLES2Vertices *pVertices = (OpenGLES2Vertices*) pResource;
	if ( pVertices->m_iIndexVBO == 0 ) glGenBuffers( 1, &pVertices->m_iIndexVBO );
	pVertices->m_iNumIndices = numIndices;

	pVertices->m_iFlags |= AGK_GLES2_VERTICES_INDICES_INT;
		
	if ( m_iCurrentBoundIndexVBO != pVertices->m_iIndexVBO )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pVertices->m_iIndexVBO );
		m_iCurrentBoundIndexVBO = pVertices->m_iIndexVBO;
	}
	//glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint32_t), NULL, GL_STATIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint32_t), pIndexData, GL_STATIC_DRAW );

	return APP_SUCCESS;
}

int OpenGLES2Renderer::DeleteVertices( void *pResource )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	OpenGLES2Vertices *pVertices = (OpenGLES2Vertices*) pResource;

	if ( m_iCurrentBoundVBO == pVertices->m_iVertexVBO )
	{
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		m_iCurrentBoundVBO = 0;
		m_iVBOPtrSetup = 0;
	}

	glDeleteBuffers( 1, &pVertices->m_iVertexVBO );
	if ( pVertices->m_iIndexVBO != 0 ) 
	{
		if ( m_iCurrentBoundIndexVBO == pVertices->m_iIndexVBO )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			m_iCurrentBoundIndexVBO = 0;
		}
		glDeleteBuffers( 1, &pVertices->m_iIndexVBO );
	}

	delete pVertices;
	return APP_SUCCESS;
}

// pCommandBuffer is not used in OpenGL
int OpenGLES2Renderer::DrawVertices( void *pCommandBuffer, void *pResVertices, int count )
{
	if ( !pResVertices ) return APP_ERROR_UNKNOWN;
	OpenGLES2Vertices *pVertices = (OpenGLES2Vertices*) pResVertices;

	if ( !m_pCurrAGKShader ) ReportAndReturnErrorID( APP_ERROR_GL2_NO_SHADER_BOUND );
	if ( !m_pCurrVertexLayout ) ReportAndReturnErrorID( APP_ERROR_GL2_NO_VERTEX_LAYOUT );

	if ( pVertices->m_iVertexVBO != m_iCurrentBoundVBO )
	{
		glBindBuffer( GL_ARRAY_BUFFER, pVertices->m_iVertexVBO );

		m_iCurrentBoundVBO = pVertices->m_iVertexVBO;
		m_iVBOPtrSetup = 0;
	}

	if ( m_iVBOPtrSetup == 0 )
	{
		// vertex layout
		for( uint32_t i = 0; i < m_pCurrAGKShader->GetNumAttributes(); i++ )
		{
			AGKShaderAttrib *pAttrib = m_pCurrAGKShader->GetAttribute( i );
			glVertexAttribPointer( pAttrib->m_iLocation, pAttrib->m_iComponents, GL_FLOAT, GL_FALSE, m_pCurrVertexLayout->m_iVertexSize, (void*) m_pCurrVertexLayout->m_pOffsets[i] ); 
			glEnableVertexAttribArray( pAttrib->m_iLocation );
		}
		m_iVBOPtrSetup = 1;
	}

	GLenum glprimitive = GL_TRIANGLES;
	switch( m_pCurrVertexLayout->m_iPrimitiveType )
	{
		case AGK_PRIMITIVE_TRIANGLES: glprimitive = GL_TRIANGLES; break;
		case AGK_PRIMITIVE_TRIANGLE_STRIP: glprimitive = GL_TRIANGLE_STRIP; break;
		case AGK_PRIMITIVE_TRIANGLE_FAN: glprimitive = GL_TRIANGLE_FAN; break;
		case AGK_PRIMITIVE_LINE_LOOP: glprimitive = GL_LINE_LOOP; break;
		case AGK_PRIMITIVE_LINES: glprimitive = GL_LINES; break;
		case AGK_PRIMITIVE_POINTS: glprimitive = GL_POINTS; break;
	}

	// index draw or vertex draw
	if ( pVertices->m_iNumIndices > 0 )
	{
		if ( count < 0 ) count = pVertices->m_iNumIndices;
		if ( pVertices->m_iIndexVBO != m_iCurrentBoundIndexVBO )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pVertices->m_iIndexVBO );
			m_iCurrentBoundIndexVBO = pVertices->m_iIndexVBO;
		}
		glDrawElements( glprimitive, count, (pVertices->m_iFlags & AGK_GLES2_VERTICES_INDICES_INT) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0 );

		m_iVerticesProcessed += pVertices->m_iNumVertices;
		if ( m_pCurrVertexLayout->m_iPrimitiveType == AGK_PRIMITIVE_TRIANGLES ) m_iPolygonsDrawn += count / 3;
		else if ( m_pCurrVertexLayout->m_iPrimitiveType == AGK_PRIMITIVE_TRIANGLE_STRIP ) m_iPolygonsDrawn += count - 2;
	}
	else
	{
		if ( m_iCurrentBoundIndexVBO != 0 )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			m_iCurrentBoundIndexVBO = 0;
		}

		if ( count < 0 ) count = pVertices->m_iNumVertices;
		glDrawArrays( glprimitive, 0, count );

		m_iVerticesProcessed += count;
		if ( m_pCurrVertexLayout->m_iPrimitiveType == AGK_PRIMITIVE_TRIANGLES ) m_iPolygonsDrawn += count / 3;
		else if ( m_pCurrVertexLayout->m_iPrimitiveType == AGK_PRIMITIVE_TRIANGLE_STRIP ) m_iPolygonsDrawn += count - 2;
	}

	return APP_SUCCESS;
}


// frame buffers
int OpenGLES2Renderer::CreateFrameBuffer( void *pColorImage, void *pDepthImage, int iForceDepth, void **outFBO )
{
	if ( !outFBO ) return APP_ERROR_UNKNOWN;
	if ( *outFBO ) DeleteFrameBuffer( *outFBO );

	OpenGLES2FrameBuffer *pFBO = new OpenGLES2FrameBuffer;
	*outFBO = pFBO;

	pFBO->m_pColor = (OpenGLES2Image*) pColorImage;
	pFBO->m_pDepth = (OpenGLES2Image*) pDepthImage;
	pFBO->m_iFlags = iForceDepth ? AGK_GLES2_FRAMEBUFFER_FORCE_DEPTH : 0 ;
	
	uint32_t iCurr = 0;
	if ( m_pCurrFrameBuffer ) iCurr = m_pCurrFrameBuffer->m_iFBO;

	glGenFramebuffers( 1, &pFBO->m_iFBO );
	glBindFramebuffer( GL_FRAMEBUFFER, pFBO->m_iFBO );
		
	if ( pFBO->m_pColor )
	{
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pFBO->m_pColor->m_iTextureID, 0 );
	}

	if ( pFBO->m_pDepth )
	{
		if ( m_iSupportsDepthTexture )
		{
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pFBO->m_pDepth->m_iTextureID, 0 );
		}
		else
		{
			glGenRenderbuffers( 1, &pFBO->m_iRBO );
			glBindRenderbuffer( GL_RENDERBUFFER, pFBO->m_iRBO );
			glRenderbufferStorage( GL_RENDERBUFFER, m_iSupportsDepth24 ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16, pFBO->m_pColor->m_iWidth, pFBO->m_pColor->m_iHeight );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pFBO->m_iRBO );
		}
	}
	else
	{
		if ( pFBO->m_iFlags & AGK_GLES2_FRAMEBUFFER_FORCE_DEPTH )
		{
			glGenRenderbuffers( 1, &pFBO->m_iRBO );
			glBindRenderbuffer( GL_RENDERBUFFER, pFBO->m_iRBO );
			glRenderbufferStorage( GL_RENDERBUFFER, m_iSupportsDepth24 ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16, pFBO->m_pColor->m_iWidth, pFBO->m_pColor->m_iHeight );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pFBO->m_iRBO );
		}
	}

	GLenum result = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( result != GL_FRAMEBUFFER_COMPLETE )
	{
		switch( result )
		{
			case GL_FRAMEBUFFER_UNSUPPORTED: agk::Error( "GL_FRAMEBUFFER_UNSUPPORTED" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: agk::Error( "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: agk::Error( "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" ); break;
			default: 
			{
				uString err = "Framebuffer error: ";
				err += result;
				agk::Error( err );
			}
		}
		ReportAndReturnErrorID( APP_ERROR_GL2_CREATE_FRAMEBUFFER_FAILED, (int)result );
	}

#ifdef AGK_IOS
	if ( iCurr == 0 )
	{
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer );
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
	}
	else
#endif
	{
		glBindFramebuffer( GL_FRAMEBUFFER, iCurr );
	}

	return APP_SUCCESS;
}

int OpenGLES2Renderer::DeleteFrameBuffer( void *pResource )
{
	if ( m_pCurrFrameBuffer == pResource ) 
	{
		#ifdef AGK_IOS
			glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer );
			glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		#else
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		#endif
		m_pCurrFrameBuffer = 0;
	}

	OpenGLES2FrameBuffer *pFBO = (OpenGLES2FrameBuffer*) pResource;

	if ( pFBO->m_iFBO ) glDeleteFramebuffers( 1, &pFBO->m_iFBO );
	pFBO->m_iFBO = 0;

	if ( pFBO->m_iRBO ) glDeleteRenderbuffers( 1, &pFBO->m_iRBO );
	pFBO->m_iRBO = 0;

	delete pFBO;
	return APP_SUCCESS;
}

// pCommandBuffer is not used in OpenGL
int OpenGLES2Renderer::BindFrameBuffer( void *pCommandBuffer, void *pResource )
{
	if ( m_pCurrFrameBuffer == pResource ) return APP_SUCCESS;
	OpenGLES2FrameBuffer *pFBO = (OpenGLES2FrameBuffer*) pResource;
	
	if ( m_pCurrFrameBuffer && m_pCurrFrameBuffer->m_pColor )
	{
		GenerateImageMipmaps( m_pCurrFrameBuffer->m_pColor );
	}

	if ( pFBO == 0 || pFBO->m_iFBO == 0 ) 
	{
#ifdef AGK_IOS
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer );
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
#else
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
#endif
	}
	else 
	{
		glBindFramebuffer( GL_FRAMEBUFFER, pFBO->m_iFBO );
	}

	m_pCurrFrameBuffer = pFBO;
	return APP_SUCCESS;
}

// Shaders

const char* OpenGLES2Renderer::GetShaderDefaultFullScreenVS()
{
	return "\
	attribute highp vec3 position;\n\
	varying highp vec2 uvVarying;\n\
	uniform mediump float agk_invert;\n\
	\
	void main() {\n\
		gl_Position = vec4(position.xy*vec2(1,agk_invert),0.5,1.0);\n\
		uvVarying = (position.xy*vec2(0.5,-0.5) + 0.5);\n\
	}";
}

const char* OpenGLES2Renderer::GetShaderDefaultSpriteVS()
{
	return "\
	attribute highp vec3 position;\
	attribute mediump vec4 color;\
	attribute highp vec2 uv;\
	varying highp vec2 uvVarying;\
	varying mediump vec4 colorVarying;\
	uniform highp mat4 agk_Ortho;\
	void main() { \
		gl_Position = agk_Ortho * vec4(position,1.0);\
		uvVarying = uv;\
		colorVarying = color;\
	}";
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Color Shader" );
	pShader->m_sPSFilename.SetStr( "Default Color Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	attribute mediump vec4 color;\n\
	varying mediump vec4 colorVarying;\n\
	uniform highp mat4 agk_Ortho;\n\
	void main() {\n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	varying mediump vec4 colorVarying;\n\
	void main() { gl_FragColor = colorVarying; }" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderTexColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default TexColor Shader" );
	pShader->m_sPSFilename.SetStr( "Default TexColor Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	attribute mediump vec4 color;\n\
	attribute highp vec2 uv;\n\
	\
	varying highp vec2 uvVarying;\n\
	varying mediump vec4 colorVarying;\n\
	\
	uniform highp mat4 agk_Ortho;\n\
	\
	void main() {\n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	uniform sampler2D texture0;\n\
	varying highp vec2 uvVarying;\n\
	varying mediump vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = texture2D(texture0, uvVarying) * colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderFont()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Font Shader" );
	pShader->m_sPSFilename.SetStr( "Default Font Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	attribute mediump vec4 color;\n\
	attribute highp vec2 uv;\n\
	\
	varying highp vec2 uvVarying;\n\
	varying mediump vec4 colorVarying;\n\
	\
	uniform highp mat4 agk_Ortho;\n\
	\
	void main() { \n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	uniform sampler2D texture0;\
	varying highp vec2 uvVarying;\
	varying mediump vec4 colorVarying;\
	void main()\
	{ \
		gl_FragColor = colorVarying;\
		gl_FragColor.a = gl_FragColor.a * texture2D(texture0, uvVarying).a;\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderShadow()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	\
	uniform highp mat4 agk_ShadowWorldProj;\n\
	\
	void main() { \n\
		gl_Position = agk_ShadowWorldProj * vec4(position,1.0);\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	void main() {}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderShadowAlpha()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Alpha Mask Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Alpha Mask Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	attribute highp vec2 uv;\n\
	\
	varying highp vec2 uvVarying;\n\
	\
	uniform highp vec4 uvBounds0;\n\
	uniform highp vec4 textureBounds0;\n\
	\
	uniform highp mat4 agk_ShadowWorldProj;\n\
	\
	void main() { \n\
		gl_Position = agk_ShadowWorldProj * vec4(position,1.0);\n\
		uvVarying = uv * uvBounds0.xy + uvBounds0.zw;\n\
		uvVarying = uvVarying * textureBounds0.xy + textureBounds0.zw;\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	uniform sampler2D texture0;\n\
	varying highp vec2 uvVarying;\n\
	void main() {\n\
	    mediump float alpha = texture2D(texture0, uvVarying).a;\n\
	    if ( alpha < 0.5 ) discard;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderShadowBone( uint32_t numBones )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Bone Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Bone Shader" );

	char szBoneCount[ 12 ];
	sprintf( szBoneCount, "%d", numBones );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	attribute highp vec4 boneweights;\n\
	attribute mediump vec4 boneindices;\n\
	\
	uniform highp mat4 agk_ShadowProj;\n\
	uniform highp vec4 agk_bonequats1[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	uniform highp vec4 agk_bonequats2[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	\
	vec3 transformDQ( highp vec3 p, highp vec4 q1, highp vec4 q2 ) {\n\
	    p += 2.0 * cross( q1.xyz, cross(q1.xyz, p) + q1.w*p );\n\
	    p += 2.0 * (q1.w*q2.xyz - q2.w*q1.xyz + cross(q1.xyz,q2.xyz));\n\
	    return p;\n\
	}\n\
	\
	void main() { \n\
		highp vec4 q1 = agk_bonequats1[ int(boneindices.x) ] * boneweights.x;\n\
		q1 += agk_bonequats1[ int(boneindices.y) ] * boneweights.y;\n\
		q1 += agk_bonequats1[ int(boneindices.z) ] * boneweights.z;\n\
		q1 += agk_bonequats1[ int(boneindices.w) ] * boneweights.w;\n\
		\
		highp vec4 q2 = agk_bonequats2[ int(boneindices.x) ] * boneweights.x;\n\
		q2 += agk_bonequats2[ int(boneindices.y) ] * boneweights.y;\n\
		q2 += agk_bonequats2[ int(boneindices.z) ] * boneweights.z;\n\
		q2 += agk_bonequats2[ int(boneindices.w) ] * boneweights.w;\n\
		\
		highp float len = 1.0/length(q1);\n\
		q1 *= len;\n\
		q2 = (q2 - q1*dot(q1,q2)) * len;\n\
		\
		gl_Position = agk_ShadowProj * vec4( transformDQ(position,q1,q2), 1.0 );\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	void main() {}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderShadowBoneAlpha( uint32_t numBones )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Bone Alpha Mask Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Bone Alpha Mask Shader" );

	char szBoneCount[ 12 ];
	sprintf( szBoneCount, "%d", numBones );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	attribute highp vec4 boneweights;\n\
	attribute mediump vec4 boneindices;\n\
	attribute highp vec2 uv;\n\
	\
	varying highp vec2 uvVarying;\n\
	\
	uniform highp vec4 uvBounds0;\n\
	uniform highp vec4 textureBounds0;\n\
	\
	uniform highp mat4 agk_ShadowProj;\n\
	uniform highp vec4 agk_bonequats1[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	uniform highp vec4 agk_bonequats2[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	\
	vec3 transformDQ( highp vec3 p, highp vec4 q1, highp vec4 q2 ) {\n\
	    p += 2.0 * cross( q1.xyz, cross(q1.xyz, p) + q1.w*p );\n\
	    p += 2.0 * (q1.w*q2.xyz - q2.w*q1.xyz + cross(q1.xyz,q2.xyz));\n\
	    return p;\n\
	}\n\
	\
	void main() { \n\
		highp vec4 q1 = agk_bonequats1[ int(boneindices.x) ] * boneweights.x;\n\
		q1 += agk_bonequats1[ int(boneindices.y) ] * boneweights.y;\n\
		q1 += agk_bonequats1[ int(boneindices.z) ] * boneweights.z;\n\
		q1 += agk_bonequats1[ int(boneindices.w) ] * boneweights.w;\n\
		\
		highp vec4 q2 = agk_bonequats2[ int(boneindices.x) ] * boneweights.x;\n\
		q2 += agk_bonequats2[ int(boneindices.y) ] * boneweights.y;\n\
		q2 += agk_bonequats2[ int(boneindices.z) ] * boneweights.z;\n\
		q2 += agk_bonequats2[ int(boneindices.w) ] * boneweights.w;\n\
		\
		highp float len = 1.0/length(q1);\n\
		q1 *= len;\n\
		q2 = (q2 - q1*dot(q1,q2)) * len;\n\
		\
		gl_Position = agk_ShadowProj * vec4( transformDQ(position,q1,q2), 1.0 );\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	uniform sampler2D texture0;\n\
	varying highp vec2 uvVarying;\n\
	void main() {\n\
	    mediump float alpha = texture2D(texture0, uvVarying).a;\n\
	    if ( alpha < 0.5 ) discard;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShader3DParticlesTex()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default 3D Particles Tex Shader" );
	pShader->m_sPSFilename.SetStr( "Default 3D Particles Tex Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	attribute mediump vec4 color;\n\
	attribute highp vec2 uv;\n\
	\
	varying highp vec2 uvVarying;\n\
	varying mediump vec4 colorVarying;\n\
	\
	uniform highp mat4 agk_ViewProj;\n\
	\
	void main() {\n\
		gl_Position = agk_ViewProj * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	uniform sampler2D texture0;\n\
	varying highp vec2 uvVarying;\n\
	varying mediump vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = texture2D(texture0, uvVarying) * colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShader3DParticlesColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default 3D Particles Color Shader" );
	pShader->m_sPSFilename.SetStr( "Default 3D Particles Color Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	attribute mediump vec4 color;\n\
	\
	varying mediump vec4 colorVarying;\n\
	\
	uniform highp mat4 agk_ViewProj;\n\
	\
	void main() {\n\
		gl_Position = agk_ViewProj * vec4(position,1.0);\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	varying mediump vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderVideo()
{
	AGKShader *pShader = new AGKShader();

    pShader->m_sVSFilename.SetStr( "Video Fullscreen" );
    pShader->m_sPSFilename.SetStr( "Video Texture Android" );

    pShader->m_sVSSource.SetStr( "\
    attribute highp vec3 position;\n\
    varying highp vec2 uvVarying;\n\
    uniform highp vec4 uvBounds;\n\
    uniform mediump float agk_invert;\n\
    void main() {\n\
        gl_Position = vec4(position.xy*vec2(1.0,agk_invert),0.5,1.0);\n\
        uvVarying = (position.xy*vec2(0.5,-0.5) + 0.5) * uvBounds.xy + uvBounds.zw;\n\
    }" );

    pShader->m_sPSSource.SetStr( "\
    #extension GL_OES_EGL_image_external : require\n\
    #ifdef GL_ES\n\
        precision highp float;\n\
    #endif\n\
    uniform samplerExternalOES videoTexture;\n\
    varying highp vec2 uvVarying;\n\
    void main() {\n\
        gl_FragColor = texture2D( videoTexture, uvVarying );\n\
    }" );

    pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

    InternalCompileShader( pShader );
    InternalEnumerateShaderResources( pShader );

    return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderDeviceCamera()
{
	AGKShader *pShader = new AGKShader();

    pShader->m_sVSFilename.SetStr( "Camera Fullscreen" );
    pShader->m_sPSFilename.SetStr( "Camera Texture Android" );

    pShader->m_sVSSource.SetStr( "\
    attribute highp vec3 position;\n\
    varying highp vec2 uvVarying;\n\
    uniform highp vec4 orientation;\n\
    uniform mediump float agk_invert;\n\
    void main() {\n\
        gl_Position = vec4(position.xy*vec2(1.0,agk_invert),0.5,1.0);\n\
        highp vec2 uv = position.xy*vec2(0.5,-0.5);\n\
        uvVarying.x = uv.x*orientation.x + uv.y*orientation.y + 0.5;\n\
        uvVarying.y = uv.x*orientation.z + uv.y*orientation.w + 0.5;\n\
    }" );

    pShader->m_sPSSource.SetStr( "\
    #extension GL_OES_EGL_image_external : require\n\
    #ifdef GL_ES\n\
        precision highp float;\n\
    #endif\n\
    uniform samplerExternalOES videoTexture;\n\
    varying highp vec2 uvVarying;\n\
    void main() {\n\
        gl_FragColor = texture2D( videoTexture, uvVarying );\n\
    }" );

    pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

    InternalCompileShader( pShader );
    InternalEnumerateShaderResources( pShader );
    
    pShader->SetConstantByName( "orientation", 1,0,0,1 );

    return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderARAndroid()
{
	AGKShader *pShader = new AGKShader();

    pShader->m_sVSFilename.SetStr( "AR Fullscreen" );
    pShader->m_sPSFilename.SetStr( "AR Texture Android" );

    pShader->m_sVSSource.SetStr( "\
    attribute highp vec3 position;\n\
    varying highp vec2 uvVarying;\n\
    uniform highp vec4 orientation;\n\
    uniform mediump float agk_invert;\n\
    void main() {\n\
        gl_Position = vec4(position.xy*vec2(1.0,agk_invert),0.5,1.0);\n\
        highp vec2 uv = position.xy*vec2(0.5,-0.5);\n\
        uvVarying.x = uv.x*orientation.x + uv.y*orientation.y + 0.5;\n\
        uvVarying.y = uv.x*orientation.z + uv.y*orientation.w + 0.5;\n\
    }" );

    pShader->m_sPSSource.SetStr( "\
    #extension GL_OES_EGL_image_external : require\n\
    #ifdef GL_ES\n\
        precision highp float;\n\
    #endif\n\
    uniform samplerExternalOES videoTexture;\n\
    varying highp vec2 uvVarying;\n\
    void main() {\n\
        gl_FragColor = texture2D( videoTexture, uvVarying );\n\
    }" );

    pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

    InternalCompileShader( pShader );
    InternalEnumerateShaderResources( pShader );
    
    pShader->SetConstantByName( "orientation", 1,0,0,1 );

    return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderARiOS()
{
	AGKShader *pShader = new AGKShader();

    pShader->m_sVSFilename.SetStr( "AR Fullscreen" );
    pShader->m_sPSFilename.SetStr( "AR Texture iOS" );

    pShader->m_sVSSource.SetStr( "\
    attribute highp vec3 position;\n\
    varying highp vec2 uvVarying;\n\
    uniform highp vec4 orientation;\n\
    uniform mediump float agk_invert;\n\
    void main() {\n\
        gl_Position = vec4(position.xy*vec2(1.0,agk_invert),0.5,1.0);\n\
        highp vec2 uv = position.xy*vec2(0.5,-0.5);\n\
        uvVarying.x = uv.x*orientation.x + uv.y*orientation.y + 0.5;\n\
        uvVarying.y = uv.x*orientation.z + uv.y*orientation.w + 0.5;\n\
    }" );

    pShader->m_sPSSource.SetStr( "\
    uniform sampler2D texture0;\n\
    uniform sampler2D texture1;\n\
    varying highp vec2 uvVarying;\n\
    void main() {\n\
        highp vec3 yuv = vec3( texture2D(texture0, uvVarying).r, texture2D(texture1, uvVarying).ra );\n\
        lowp float red = yuv.r + yuv.b*1.402 - 0.701;\n\
        lowp float green = yuv.r - yuv.g*0.3441 - yuv.b*0.7141 + 0.5291;\n\
        lowp float blue = yuv.r + yuv.g*1.772 - 0.886;\n\
        gl_FragColor = vec4( red, green, blue, 1.0 );\n\
    }" );

    pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

    InternalCompileShader( pShader );
    InternalEnumerateShaderResources( pShader );
    
    pShader->SetConstantByName( "orientation", 1,0,0,1 );

    return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderQuad()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Quad Shader" );
	pShader->m_sPSFilename.SetStr( "Default Quad Shader" );

	pShader->m_sVSSource.SetStr( GetShaderDefaultFullScreenVS() );

	pShader->m_sPSSource.SetStr( "\
	uniform sampler2D texture0;\n\
	varying highp vec2 uvVarying;\n\
	\
	void main() {\n\
		gl_FragColor = texture2D(texture0, uvVarying);\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderSkyBoxSun()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default SkyBox Sun Shader" );
	pShader->m_sPSFilename.SetStr( "Default SkyBox Sun Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	\
	varying highp vec2 uvVarying;\n\
	varying highp vec2 uvVarying2;\n\
	varying highp vec2 horizonVarying;\n\
	\
	uniform highp mat4 agk_World;\n\
	uniform highp mat4 agk_ViewProj;\n\
	\
	uniform highp vec2 sunSize;\n\
	uniform highp float horizonHeight;\n\
	uniform highp float objectScale; // 1.0 / objectScaleY \n\
	\
	void main() {\n\
	    highp vec4 pos = agk_World * vec4(position,1.0);\n\
	    gl_Position = agk_ViewProj * pos;\n\
	    gl_Position.z = gl_Position.w; // force it to the back \n\
		\
	    horizonVarying.x = (pos.y-horizonHeight)*objectScale;\n\
	    horizonVarying.y = step(position.y,0.0);\n\
		\
	    uvVarying = position.xz*sunSize.x + 0.5;\n\
	    uvVarying2 = position.xz*sunSize.y + 0.5;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	uniform sampler2D texture0;\n\
	\
	uniform mediump vec3 skyColor;\n\
	uniform mediump vec3 horizonColor;\n\
	uniform mediump vec3 sunColor;\n\
	uniform highp float horizonSize;\n\
	\
	varying highp vec2 uvVarying;\n\
	varying highp vec2 uvVarying2;\n\
	varying highp vec2 horizonVarying;\n\
	\
	void main() {\n\
	    highp float horizon = 1.0 - min( horizonSize*horizonVarying.x, 1.0 );\n\
	    horizon *= horizon;\n\
	    mediump vec3 color = mix( skyColor, horizonColor, horizon );\n\
		\
	    mediump vec3 sunColor2 = sunColor*1.5 - color;\n\
	    sunColor2 *= horizonVarying.y;\n\
		\
	    highp float sunPoint = texture2D(texture0,uvVarying).r;\n\
	    color += sunColor2 * sunPoint*sunPoint;\n\
		\
	    sunPoint = texture2D(texture0,uvVarying2).r;\n\
	    color += 0.2 * sunColor2 * sunPoint;\n\
		\
	    gl_FragColor = vec4(color,1.0);\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadDefaultShaderSkyBoxNoSun()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default SkyBox Shader" );
	pShader->m_sPSFilename.SetStr( "Default SkyBox Shader" );

	pShader->m_sVSSource.SetStr( "\
	attribute highp vec3 position;\n\
	\
	varying highp vec2 horizonVarying;\n\
	\
	uniform highp mat4 agk_World;\n\
	uniform highp mat4 agk_ViewProj;\n\
	\
	uniform highp float horizonHeight;\n\
	uniform highp float objectScale; // 1.0 / objectScaleY \n\
	\
	void main() {\n\
	    highp vec4 pos = agk_World * vec4(position,1.0);\n\
	    gl_Position = agk_ViewProj * pos;\n\
	    gl_Position.z = gl_Position.w; // force it to the back \n\
		\
	    horizonVarying.x = (pos.y-horizonHeight)*objectScale;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	uniform sampler2D texture0;\n\
	\
	uniform mediump vec3 skyColor;\n\
	uniform mediump vec3 horizonColor;\n\
	uniform highp float horizonSize;\n\
	\
	varying highp vec2 horizonVarying;\n\
	\
	void main() {\n\
	    highp float horizon = 1.0 - min( horizonSize*horizonVarying.x, 1.0 );\n\
		horizon *= horizon;\n\
		mediump vec3 color = mix( skyColor, horizonColor, horizon );\n\
		gl_FragColor = vec4(color,1.0);\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGLES2Renderer::Make3DShader( cMesh *pMesh )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Generated 3D Shader" );
	pShader->m_sPSFilename.SetStr( "Generated 3D Shader" );

	// Vertex shader
	pShader->m_sVSSource.ClearTemp();
    pShader->m_sVSSource.AppendAscii( "precision highp float;\n" );
	pShader->m_sVSSource.AppendAscii( "attribute vec3 position;\n" );
	if ( pMesh->HasNormals() && pMesh->WantsLighting() ) 
	{
		pShader->m_iFlags |= AGK_SHADER_USES_VS_LIGHTING;

		pShader->m_sVSSource.AppendAscii( "attribute vec3 normal;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 posVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 normalVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 lightVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "vec3 GetVSLighting( mediump vec3 normal, highp vec3 pos );\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
		if ( !pMesh->HasValidBones() )
		{
			pShader->m_sVSSource.AppendAscii( "uniform mat3 agk_WorldNormal;\n" );
			pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_World;\n" );
		}
		pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_ViewProj;\n" );

		if ( pMesh->HasNormalMap() )
		{
			if ( pMesh->HasTangents() && pMesh->HasBiNormals() )
			{
				pShader->m_sVSSource.AppendAscii( "attribute vec3 tangent;\n" );
				pShader->m_sVSSource.AppendAscii( "attribute vec3 binormal;\n" );
			}
			pShader->m_sVSSource.AppendAscii( "varying vec3 tangentVarying;\n" );
			pShader->m_sVSSource.AppendAscii( "varying vec3 binormalVarying;\n" );
		}
	}
	else
	{
		pShader->m_sVSSource.AppendAscii( "\n");
		if ( pMesh->WantsFog() && agk::GetFogMode() ) pShader->m_sVSSource.AppendAscii( "varying vec3 posVarying;\n" );
		if ( pMesh->HasValidBones() )
		{
			pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_ViewProj;\n" );
		}
		else
		{
			if ( pMesh->WantsFog() && agk::GetFogMode() )
			{
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_World;\n" );
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_ViewProj;\n" );
			}
			else
			{
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_WorldViewProj;\n" );
			}
		}
	}

	if ( pMesh->HasUVs() ) 
	{
		pShader->m_sVSSource.AppendAscii( "attribute vec2 uv;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec2 uvVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec4 uvBounds0;\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
	}
	
	if ( pMesh->HasUV1s() )
	{
		pShader->m_sVSSource.AppendAscii( "attribute vec2 uv1;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec2 uv1Varying;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec4 uvBounds1;\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
	}

	if ( pMesh->HasVertColors() ) 
	{
		pShader->m_sVSSource.AppendAscii( "attribute vec4 color;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec4 colorVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
	}

	if ( pMesh->HasValidBones() )
	{
		char szBoneCount[ 10 ];
		sprintf( szBoneCount, "%d", pMesh->GetNumBones() );

		pShader->m_sVSSource.AppendAscii( "attribute vec4 boneweights;\n" );
		pShader->m_sVSSource.AppendAscii( "attribute vec4 boneindices;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec4 agk_bonequats1["); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii( "];\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec4 agk_bonequats2["); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii( "];\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
		pShader->m_sVSSource.AppendAscii( "vec3 transformDQ( vec3 p, vec4 q1, vec4 q2 )\n" );
		pShader->m_sVSSource.AppendAscii( "{\n" );
		pShader->m_sVSSource.AppendAscii( "    p += 2.0 * cross( q1.xyz, cross(q1.xyz, p) + q1.w*p );\n" );
		pShader->m_sVSSource.AppendAscii( "    p += 2.0 * (q1.w*q2.xyz - q2.w*q1.xyz + cross(q1.xyz,q2.xyz));\n" );
		pShader->m_sVSSource.AppendAscii( "    return p;\n" );
		pShader->m_sVSSource.AppendAscii( "}\n" );
		pShader->m_sVSSource.AppendAscii( "\n");
	}

	pShader->m_sVSSource.AppendAscii( "void main()\n" );
	pShader->m_sVSSource.AppendAscii( "{ \n" );
	if ( pMesh->HasUVs() ) 
	{
		pShader->m_sVSSource.AppendAscii( "    uvVarying = uv * uvBounds0.xy + uvBounds0.zw;\n" );
	}
	if ( pMesh->HasUV1s() ) 
	{
		pShader->m_sVSSource.AppendAscii( "    uv1Varying = uv1 * uvBounds1.xy + uvBounds1.zw;\n" );
	}
	if ( pMesh->HasVertColors() ) 
	{
		pShader->m_sVSSource.AppendAscii( "    colorVarying = color;\n" );
	}

	if ( pMesh->HasNormals() && pMesh->WantsLighting() && pMesh->HasNormalMap() )
	{
		if ( !pMesh->HasTangents() || !pMesh->HasBiNormals() )
		{
			pShader->m_sVSSource.AppendAscii( "    vec3 tangent;\n" );
			pShader->m_sVSSource.AppendAscii( "    if ( abs(normal.y) > 0.999 ) tangent = vec3( abs(normal.y),0.0,0.0 );\n" );
			pShader->m_sVSSource.AppendAscii( "    else tangent = normalize( vec3(-normal.z, 0.0, normal.x) );\n" );
			pShader->m_sVSSource.AppendAscii( "    vec3 binormal = normalize( vec3(normal.y*tangent.z, normal.z*tangent.x-normal.x*tangent.z, -normal.y*tangent.x) );\n" );
		}
	}

	if ( pMesh->HasValidBones() )
	{
		pShader->m_sVSSource.AppendAscii( "    vec4 q1 = agk_bonequats1[ int(boneindices.x) ] * boneweights.x;\n" );
		pShader->m_sVSSource.AppendAscii( "    q1 += agk_bonequats1[ int(boneindices.y) ] * boneweights.y;\n" );
		pShader->m_sVSSource.AppendAscii( "    q1 += agk_bonequats1[ int(boneindices.z) ] * boneweights.z;\n" );
		pShader->m_sVSSource.AppendAscii( "    q1 += agk_bonequats1[ int(boneindices.w) ] * boneweights.w;\n" );
		
		pShader->m_sVSSource.AppendAscii( "    vec4 q2 = agk_bonequats2[ int(boneindices.x) ] * boneweights.x;\n" );
		pShader->m_sVSSource.AppendAscii( "    q2 += agk_bonequats2[ int(boneindices.y) ] * boneweights.y;\n" );
		pShader->m_sVSSource.AppendAscii( "    q2 += agk_bonequats2[ int(boneindices.z) ] * boneweights.z;\n" );
		pShader->m_sVSSource.AppendAscii( "    q2 += agk_bonequats2[ int(boneindices.w) ] * boneweights.w;\n" );
		
		pShader->m_sVSSource.AppendAscii( "    float len = 1.0/length(q1);\n" );
		pShader->m_sVSSource.AppendAscii( "    q1 *= len;\n" );
		//pShader->m_sVSSource.AppendAscii( "    q2 *= len;\n" ); // quicker but less accurate?
		pShader->m_sVSSource.AppendAscii( "    q2 = (q2 - q1*dot(q1,q2)) * len;\n" );
		
		pShader->m_sVSSource.AppendAscii( "    vec4 pos = vec4( transformDQ(position,q1,q2), 1.0 );\n" );
		pShader->m_sVSSource.AppendAscii( "    gl_Position = agk_ViewProj * pos;\n");
		if ( pMesh->HasNormals() && pMesh->WantsLighting() )
		{
			pShader->m_sVSSource.AppendAscii( "\n");
			pShader->m_sVSSource.AppendAscii( "    normalVarying = normal + 2.0*cross( q1.xyz, cross(q1.xyz,normal) + q1.w*normal );\n");
			pShader->m_sVSSource.AppendAscii( "    posVarying = pos.xyz;\n");
			pShader->m_sVSSource.AppendAscii( "    lightVarying = GetVSLighting( normalVarying, posVarying );\n");
			if ( pMesh->HasNormalMap() )
			{
				pShader->m_sVSSource.AppendAscii( "    tangentVarying = tangent + 2.0*cross( q1.xyz, cross(q1.xyz,tangent) + q1.w*tangent );\n");
				pShader->m_sVSSource.AppendAscii( "    binormalVarying = binormal + 2.0*cross( q1.xyz, cross(q1.xyz,binormal) + q1.w*binormal );\n");
			}
		}
		else if ( pMesh->WantsFog() && agk::GetFogMode() )
		{
			pShader->m_sVSSource.AppendAscii( "    posVarying = pos.xyz;\n");
		}
	}
	else
	{
		if ( pMesh->HasNormals() && pMesh->WantsLighting() )
		{
			pShader->m_sVSSource.AppendAscii( "    vec4 pos = agk_World * vec4(position,1.0);\n");
			pShader->m_sVSSource.AppendAscii( "    gl_Position = agk_ViewProj * pos;\n");
			pShader->m_sVSSource.AppendAscii( "    vec3 norm = normalize(agk_WorldNormal * normal);\n");
			pShader->m_sVSSource.AppendAscii( "    posVarying = pos.xyz;\n");
			pShader->m_sVSSource.AppendAscii( "    normalVarying = norm;\n");
			pShader->m_sVSSource.AppendAscii( "    lightVarying = GetVSLighting( norm, posVarying );\n");
			if ( pMesh->HasNormalMap() )
			{
				pShader->m_sVSSource.AppendAscii( "    tangentVarying = normalize(agk_WorldNormal * tangent);\n");
				pShader->m_sVSSource.AppendAscii( "    binormalVarying = normalize(agk_WorldNormal * binormal);\n");
			}
		}
		else if ( pMesh->WantsFog() && agk::GetFogMode() )
		{
			pShader->m_sVSSource.AppendAscii( "    vec4 pos = agk_World * vec4(position,1.0);\n");
			pShader->m_sVSSource.AppendAscii( "    gl_Position = agk_ViewProj * pos;\n");
			pShader->m_sVSSource.AppendAscii( "    posVarying = pos.xyz;\n");
		}
		else
		{
			pShader->m_sVSSource.AppendAscii( "    gl_Position = agk_WorldViewProj * vec4(position,1.0);\n");
		}
	}
	
	pShader->m_sVSSource.AppendAscii( "}\n" );

	// Pixel shader
	pShader->m_sPSSource.ClearTemp();
    pShader->m_sPSSource.AppendAscii( "precision highp float;\n" );
	if ( pMesh->HasUVs() )
	{
		pShader->m_sPSSource.AppendAscii( "varying vec2 uvVarying;\n" );
		if ( pMesh->HasTexture0() )
		{
			pShader->m_sPSSource.AppendAscii( "uniform sampler2D texture0;\n" );
			if ( pMesh->GetImage(0)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "uniform vec4 textureBounds0;\n" );
		}
	}

	if ( (pMesh->HasTexture1() || pMesh->HasNormalMap()) && pMesh->HasUV1s() )
	{
		pShader->m_sPSSource.AppendAscii( "varying vec2 uv1Varying;\n" );
	}

	if ( pMesh->HasTexture1() )
	{
		pShader->m_sPSSource.AppendAscii( "uniform sampler2D texture1;\n" );
		if ( pMesh->GetImage(1)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "uniform vec4 textureBounds1;\n" );
	}

	if ( pMesh->HasVertColors() )
	{
		pShader->m_sPSSource.AppendAscii( "varying vec4 colorVarying;\n" );
	}

	if ( pMesh->HasNormals() && pMesh->WantsLighting() )
	{
		pShader->m_iFlags |= AGK_SHADER_USES_PS_LIGHTING;

		pShader->m_sPSSource.AppendAscii( "varying vec3 normalVarying;\n" );
		pShader->m_sPSSource.AppendAscii( "varying vec3 lightVarying;\n" );
		pShader->m_sPSSource.AppendAscii( "varying vec3 posVarying;\n" );
		pShader->m_sPSSource.AppendAscii( "vec3 GetPSLighting( mediump vec3 normal, highp vec3 pos );\n" );

		if ( pMesh->HasNormalMap() )
		{
			pShader->m_sPSSource.AppendAscii( "uniform sampler2D texture2;\n" );
			if ( pMesh->GetImage(2)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "uniform vec4 textureBounds2;\n" );
			pShader->m_sPSSource.AppendAscii( "varying vec3 tangentVarying;\n" );
			pShader->m_sPSSource.AppendAscii( "varying vec3 binormalVarying;\n" );
			pShader->m_sPSSource.AppendAscii( "uniform vec2 agk_NormalScale;\n" );
		}
	}
	
	if ( pMesh->WantsFog() && agk::GetFogMode() )
	{
		pShader->m_iFlags |= AGK_SHADER_USES_FOG;

		if ( pShader->NeedsPSLighting() == 0 ) pShader->m_sPSSource.AppendAscii( "varying vec3 posVarying;\n" );
		pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( mediump vec3 color, highp vec3 pointPos );\n" );
	}

	pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_MeshDiffuse;\n" );
	pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_MeshEmissive;\n" );

	pShader->m_sPSSource.AppendAscii( "void main()\n" );
	pShader->m_sPSSource.AppendAscii( "{ \n" );
	if ( pMesh->HasVertColors() )
	{
		pShader->m_sPSSource.AppendAscii( "    vec4 blendTex = colorVarying;\n" );
	}
	else
	{
		pShader->m_sPSSource.AppendAscii( "    vec4 blendTex = vec4(1.0,1.0,1.0,1.0);\n" );
	}
	
	int hasLight = 0;
	if ( pMesh->HasNormals() && pMesh->WantsLighting() )
	{
		pShader->m_sPSSource.AppendAscii( "    vec3 norm = normalize(normalVarying);\n" );

		if ( pMesh->HasNormalMap() )
		{
			// if there is a separate UV channel, and it isn't being used by texture 1, then use it for normal mapping, otherwise use the base UV
			if ( pMesh->HasUV1s() && !pMesh->HasTexture1() )
			{
				if ( !pMesh->GetImage(2)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec2 texture2UV = uv1Varying;\n" );
				else pShader->m_sPSSource.AppendAscii( "    vec2 texture2UV = uv1Varying*textureBounds2.xy + textureBounds2.zw;\n" );
			}
			else
			{
				if ( !pMesh->GetImage(2)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec2 texture2UV = uvVarying;\n" );
				else pShader->m_sPSSource.AppendAscii( "    vec2 texture2UV = uvVarying*textureBounds2.xy + textureBounds2.zw;\n" );
			}

			pShader->m_sPSSource.AppendAscii( "    vec3 normalmap = texture2D(texture2, texture2UV*agk_NormalScale).xyz;\n" );
			pShader->m_sPSSource.AppendAscii( "    normalmap = normalmap * 2.0 - 1.0;\n" );

			pShader->m_sPSSource.AppendAscii( "    vec3 tangent = normalize(tangentVarying);\n" );
			pShader->m_sPSSource.AppendAscii( "    vec3 binormal = normalize(binormalVarying);\n" );
			pShader->m_sPSSource.AppendAscii( "    mat3 TBN = mat3( tangent, binormal, norm );\n" );
			pShader->m_sPSSource.AppendAscii( "    norm = TBN * normalmap;\n" );
		}

		pShader->m_sPSSource.AppendAscii( "    vec3 light = lightVarying + GetPSLighting( norm, posVarying ); \n" );
		hasLight = 1;
	}
	else 
	{
		pShader->m_sPSSource.AppendAscii( "    vec3 light = vec3(1.0,1.0,1.0);\n" );
	}
	
	if ( pMesh->HasTexture1() )
	{
		if ( pMesh->HasUV1s() ) 
		{
			if ( !pMesh->GetImage(1)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = texture2D(texture1, uv1Varying);\n" );
			else pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = texture2D(texture1, uv1Varying*textureBounds1.xy + textureBounds1.zw);\n" );
		}
		else if ( pMesh->HasUVs() ) 
		{
			if ( !pMesh->GetImage(1)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = texture2D(texture1, uvVarying);\n" );
			else pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = texture2D(texture1, uvVarying*textureBounds1.xy + textureBounds1.zw);\n" );
		}
		else pShader->m_sPSSource.AppendAscii( "    vec4 tex1 = vec4(1.0,1.0,1.0,1.0);\n" );

		// is it a light map, or a multiplied blending texture
		if ( pMesh->HasLightMap() )
		{
			if ( hasLight ) pShader->m_sPSSource.AppendAscii( "    light += tex1.rgb;\n" );
			else pShader->m_sPSSource.AppendAscii( "    light = tex1.rgb;\n" );
		}
		else
		{
			pShader->m_sPSSource.AppendAscii( "    blendTex *= tex1 * 2.0;\n" );
		}
	}

	//pShader->m_sPSSource.AppendAscii( "   light = clamp(light,0.0,1.0); \n" );
	
	if ( pMesh->HasUVs() && pMesh->HasTexture0() ) 
	{
		if ( !pMesh->GetImage(0)->HasModifiedUVs() ) pShader->m_sPSSource.AppendAscii( "    vec4 texColor = texture2D(texture0, uvVarying);\n" );
		else pShader->m_sPSSource.AppendAscii( "    vec4 texColor = texture2D(texture0, uvVarying*textureBounds0.xy + textureBounds0.zw);\n" );

		pShader->m_sPSSource.AppendAscii( "    gl_FragColor = texColor * blendTex * vec4(light,1.0) * agk_MeshDiffuse + agk_MeshEmissive;\n" );
	}
	else 
	{
		pShader->m_sPSSource.AppendAscii( "    gl_FragColor = blendTex * vec4(light,1.0) * agk_MeshDiffuse + agk_MeshEmissive;\n" );
	}

	if ( pMesh->GetObject() && pMesh->GetObject()->HasAlphaMask() )
	{
		pShader->m_sPSSource.AppendAscii( "    if ( gl_FragColor.a < 0.5 ) discard;\n" );
	}

	if ( pMesh->WantsFog() && agk::GetFogMode() )
	{
		pShader->m_sPSSource.AppendAscii( "    gl_FragColor.rgb = ApplyFog( gl_FragColor.rgb, posVarying );\n" );
	}
	
	pShader->m_sPSSource.AppendAscii( "}\n" );

	pShader->m_iFlags &= ~AGK_SHADER_IS_CUSTOM;
	if ( pShader->NeedsAdditionalCode() ) 
	{
		// if this shader uses lighting or fog it will fail compilation until it has the code generated for it
		pShader->SetValid( true );
	}
	else 
	{
		// no additional code required, good to compile
		InternalCompileShader( pShader );
		if ( !pShader->IsValid() ) 
		{
			delete pShader;
			return 0;
		}
		
		InternalEnumerateShaderResources( pShader );
		if ( !pShader->IsValid() ) 
		{
			delete pShader;
			return 0;
		}
	}

	return pShader;
}

AGKShader* OpenGLES2Renderer::MakeFinalShader( AGKShader *pBaseShader, int sunActive, int VSLights, int PSLights, int useShadows, int normalMap )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_pBaseShader = pBaseShader;

	pShader->m_sVSFilename.SetStr( "Internal Generated 3D Shader" );
	pShader->m_sPSFilename.SetStr( "Internal Generated 3D Shader" );
	
	pShader->m_sVSSource.SetStr( pBaseShader->m_sVSSource );
	pShader->m_sPSSource.SetStr( pBaseShader->m_sPSSource );
	
	pShader->m_sVSSource.AppendAscii( '\n' );

	if ( pBaseShader->NeedsVSLighting() )
	{
		pShader->m_sVSSource.AppendAscii( "uniform vec3 agk_LightAmbient;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec3 agk_DLightDir;\n" );
		pShader->m_sVSSource.AppendAscii( "uniform vec3 agk_DLightColor;\n" );

		if ( VSLights > 0 )
		{
			pShader->m_sVSSource.AppendAscii( "uniform vec4 agk_VSLightPos[" ); pShader->m_sVSSource.AppendInt(VSLights); pShader->m_sVSSource.AppendAscii( "];\n" );
			pShader->m_sVSSource.AppendAscii( "uniform vec4 agk_VSLightColor[" ); pShader->m_sVSSource.AppendInt(VSLights); pShader->m_sVSSource.AppendAscii( "];\n" );
		}
		
		if ( useShadows )
		{
			pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_ShadowProj;\n" );
			pShader->m_sVSSource.AppendAscii( "varying vec4 shadowVarying;\n" );
			pShader->m_sVSSource.AppendAscii( "varying float depth;\n" );
			if ( agk::GetShadowMappingMode() == 3 )
			{
				// cascade shadow mapping
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_Shadow2Proj;\n" );
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_Shadow3Proj;\n" );
				pShader->m_sVSSource.AppendAscii( "uniform mat4 agk_Shadow4Proj;\n" );

				pShader->m_sVSSource.AppendAscii( "varying vec3 shadow2Varying;\n" );
				pShader->m_sVSSource.AppendAscii( "varying vec3 shadow3Varying;\n" );
				pShader->m_sVSSource.AppendAscii( "varying vec3 shadow4Varying;\n" );
			}
		}

		// Vertex shader
		pShader->m_sVSSource.AppendAscii( "vec3 GetVSLighting( mediump vec3 normal, highp vec3 pos )\n" );
		pShader->m_sVSSource.AppendAscii( "{\n" );

		if ( useShadows ) 
		{
			pShader->m_sVSSource.AppendAscii( "    shadowVarying = agk_ShadowProj * vec4(pos,1.0);\n");
			if ( agk::GetShadowMappingMode() != 2 ) // not LiSPM
			{
				pShader->m_sVSSource.AppendAscii( "    shadowVarying.xy = shadowVarying.xy*0.5 + 0.5;\n");
				pShader->m_sVSSource.AppendAscii( "    shadowVarying.z = shadowVarying.z*0.5 + 0.5;\n");
			}
			if ( agk::GetShadowMappingMode() == 3 ) // cascade
			{
				pShader->m_sVSSource.AppendAscii( "    shadow2Varying = (agk_Shadow2Proj * vec4(pos,1.0)).xyz;\n");
				pShader->m_sVSSource.AppendAscii( "    shadow3Varying = (agk_Shadow3Proj * vec4(pos,1.0)).xyz;\n");
				pShader->m_sVSSource.AppendAscii( "    shadow4Varying = (agk_Shadow4Proj * vec4(pos,1.0)).xyz;\n");

				pShader->m_sVSSource.AppendAscii( "    shadow2Varying.xy = shadow2Varying.xy*0.5 + 0.5;\n");
				pShader->m_sVSSource.AppendAscii( "    shadow2Varying.z = shadow2Varying.z*0.5 + 0.5;\n");

				pShader->m_sVSSource.AppendAscii( "    shadow3Varying.xy = shadow3Varying.xy*0.5 + 0.5;\n");
				pShader->m_sVSSource.AppendAscii( "    shadow3Varying.z = shadow3Varying.z*0.5 + 0.5;\n");

				pShader->m_sVSSource.AppendAscii( "    shadow4Varying.xy = shadow4Varying.xy*0.5 + 0.5;\n");
				pShader->m_sVSSource.AppendAscii( "    shadow4Varying.z = shadow4Varying.z*0.5 + 0.5;\n");
			}
			pShader->m_sVSSource.AppendAscii( "    depth = gl_Position.w;\n");
		}

		// start with ambient and directional light
		pShader->m_sVSSource.AppendAscii( "    vec3 light = agk_LightAmbient;\n" );
		// if shadows or normal maps are active then the directional light has to be calculated in the pixel shader
		if ( useShadows == 0 && normalMap == 0 ) pShader->m_sVSSource.AppendAscii( "    light += max(dot(-agk_DLightDir, normal),0.0) * agk_DLightColor;\n" );
		
		for ( int i = 0; i < VSLights; ++i )
		{
			if ( i == 0 ) pShader->m_sVSSource.AppendAscii( "    vec3 dir2;\n float atten;\n float lightRange;\n float intensity;\n" );
			pShader->m_sVSSource.AppendAscii( "    dir2 = (agk_VSLightPos[" ); pShader->m_sVSSource.AppendInt(i); pShader->m_sVSSource.AppendAscii( "].xyz - pos);\n" );
			pShader->m_sVSSource.AppendAscii( "    lightRange = agk_VSLightPos[" ); pShader->m_sVSSource.AppendInt(i); pShader->m_sVSSource.AppendAscii( "].w;\n" );
			pShader->m_sVSSource.AppendAscii( "    atten = max(0.0, 1.0 - dot(dir2,dir2)/lightRange);\n" ); // squared exponetial fall off
			pShader->m_sVSSource.AppendAscii( "    atten *= atten; atten *= atten;\n" ); // squared exponetial fall off
			//pShader->m_sVSSource.AppendAscii( "    atten = clamp(atten,0.0,1.0);\n" );
			pShader->m_sVSSource.AppendAscii( "    intensity = max(0.0,dot(normalize(dir2),normal));\n" );
			pShader->m_sVSSource.AppendAscii( "    light += agk_VSLightColor[" ); pShader->m_sVSSource.AppendInt(i); pShader->m_sVSSource.AppendAscii( "].xyz * intensity * atten; \n" );
		}

		pShader->m_sVSSource.AppendAscii( "    return light;\n" );
		pShader->m_sVSSource.AppendAscii( "}\n" );
	}

	// Pixel shader
	pShader->m_sPSSource.AppendAscii( '\n' );

	// fog and shadows use the directional light
	pShader->m_sPSSource.AppendAscii( "uniform vec3 agk_DLightDir;\n" );
	pShader->m_sPSSource.AppendAscii( "uniform vec3 agk_DLightColor;\n" );

	if ( pBaseShader->NeedsPSLighting() )
	{
		if ( useShadows ) 
		{
			pShader->m_sPSSource.AppendAscii( "uniform sampler2D shadowMap;\n" );
			pShader->m_sPSSource.AppendAscii( "varying vec4 shadowVarying;\n" );
			pShader->m_sPSSource.AppendAscii( "varying float depth;\n" );
			pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_ShadowParams;\n" );
			
			if ( agk::GetShadowMappingMode() == 3 ) // cascade shadows
			{
				pShader->m_sPSSource.AppendAscii( "uniform sampler2D shadowMap2;\n" );
				pShader->m_sPSSource.AppendAscii( "uniform sampler2D shadowMap3;\n" );
				pShader->m_sPSSource.AppendAscii( "uniform sampler2D shadowMap4;\n" );

				pShader->m_sPSSource.AppendAscii( "varying vec3 shadow2Varying;\n" );
				pShader->m_sPSSource.AppendAscii( "varying vec3 shadow3Varying;\n" );
				pShader->m_sPSSource.AppendAscii( "varying vec3 shadow4Varying;\n" );

				pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_ShadowParams2;\n" );
			}
		}

		if ( PSLights > 0 )
		{
			pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_PSLightPos[" ); pShader->m_sPSSource.AppendInt(PSLights); pShader->m_sPSSource.AppendAscii( "];\n" );
			pShader->m_sPSSource.AppendAscii( "uniform vec4 agk_PSLightColor[" ); pShader->m_sPSSource.AppendInt(PSLights); pShader->m_sPSSource.AppendAscii( "];\n" );
		}
				
		pShader->m_sPSSource.AppendAscii( "vec3 GetPSLighting( mediump vec3 normal, highp vec3 pos )\n" );
		pShader->m_sPSSource.AppendAscii( "{\n" );
		pShader->m_sPSSource.AppendAscii( "    float scale = 0.01;\n" );
		pShader->m_sPSSource.AppendAscii( "    float scale2 = 0.0001;\n" );
		pShader->m_sPSSource.AppendAscii( "    vec3 light = vec3(0.0,0.0,0.0);\n" );

		if ( useShadows ) 
		{
			// calculate directional light and its shadow
			pShader->m_sPSSource.AppendAscii( "    float dotp = dot(-agk_DLightDir, normal);\n" );
			pShader->m_sPSSource.AppendAscii( "    if ( dotp > 0.0 ) {\n" );
			pShader->m_sPSSource.AppendAscii( "    vec3 dirLight = dotp * agk_DLightColor;\n" );
			//pShader->m_sPSSource.AppendAscii( "    float bias = min(agk_ShadowParams.y*tan(acos(dotp)),0.01);\n" );
			pShader->m_sPSSource.AppendAscii( "    dotp = 1.0-dotp;\n" );
			pShader->m_sPSSource.AppendAscii( "    dotp = dotp*dotp; dotp = dotp*dotp;\n" );
			pShader->m_sPSSource.AppendAscii( "    float bias = agk_ShadowParams.y*10.0*dotp + agk_ShadowParams.y;\n" );

			if ( agk::GetShadowSmoothing() == 1 ) // fixed multisample
			{
				pShader->m_sPSSource.AppendAscii( "    int samples = 4; float sampleFactor = 0.25;\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 pd[4];\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[0] = vec2(0.5785557, 0.3818373);\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[1] = vec2(-0.5812719, -0.451035);\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[2] = vec2(-0.4778019, 0.4936384);\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[3] = vec2(0.5724188, -0.4114415);\n" );
			}
			else if ( agk::GetShadowSmoothing() == 2 ) // random multisample
			{
				pShader->m_sPSSource.AppendAscii( "    int samples = 4; float sampleFactor = 0.25;\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 pd[4];\n" );
				pShader->m_sPSSource.AppendAscii( "    float base = dot(floor(pos*1000.0),vec3(0.0129898,0.078233,0.045164));\n" );
				pShader->m_sPSSource.AppendAscii( "    for( int i = 0; i < 4; i++ ) {\n" );
				pShader->m_sPSSource.AppendAscii( "        float random = sin(float(i)*94.673 + base);\n" );
				//pShader->m_sPSSource.AppendAscii( "        float random = dot(vec4(gl_FragCoord.xyy,i), vec4(12.9898,78.233,45.164,94.673));\n" );
				pShader->m_sPSSource.AppendAscii( "        float randomX = fract(random * 43758.5453);\n" );
				pShader->m_sPSSource.AppendAscii( "        float randomY = fract(random * 55204.6122);\n" );
				pShader->m_sPSSource.AppendAscii( "        pd[i].x = randomX*2.0 - 1.0;\n" );
				pShader->m_sPSSource.AppendAscii( "        pd[i].y = randomY*2.0 - 1.0;\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n" );
			}
			else if ( agk::GetShadowSmoothing() == 3 ) // 16 fixed sample
			{
				pShader->m_sPSSource.AppendAscii( "    int samples = 16; float sampleFactor = 0.0625;\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 pd[16];\n" );
				pShader->m_sPSSource.AppendAscii( "    pd[0] = vec2( -0.94201624, -0.39906216 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[1] = vec2( 0.94558609, -0.76890725 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[2] = vec2( -0.094184101, -0.92938870 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[3] = vec2( 0.34495938, 0.29387760 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[4] = vec2( -0.91588581, 0.45771432 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[5] = vec2( -0.81544232, -0.87912464 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[6] = vec2( -0.38277543, 0.27676845 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[7] = vec2( 0.97484398, 0.75648379 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[8] = vec2( 0.44323325, -0.97511554 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[9] = vec2( 0.53742981, -0.47373420 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[10] = vec2( -0.26496911, -0.41893023 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[11] = vec2( 0.79197514, 0.19090188 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[12] = vec2( -0.24188840, 0.99706507 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[13] = vec2( -0.81409955, 0.91437590 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[14] = vec2( 0.19984126, 0.78641367 );\n" ); 
				pShader->m_sPSSource.AppendAscii( "    pd[15] = vec2( 0.14383161, -0.14100790 );\n" ); 
			}
			else if ( agk::GetShadowSmoothing() == 4 ) // 16 random samples
			{
				pShader->m_sPSSource.AppendAscii( "    int samples = 16; float sampleFactor = 0.0625;\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 pd[16];\n" );
				pShader->m_sPSSource.AppendAscii( "    float base = dot(floor(pos*1000.0),vec3(0.0129898,0.078233,0.045164));\n" );
				pShader->m_sPSSource.AppendAscii( "    for( int i = 0; i < samples; i++ ) {\n" );
				pShader->m_sPSSource.AppendAscii( "        float random = sin(float(i)*94.673 + base);\n" );
				pShader->m_sPSSource.AppendAscii( "        float randomX = fract(random * 43758.5453);\n" );
				pShader->m_sPSSource.AppendAscii( "        float randomY = fract(random * 55204.6122);\n" );
				pShader->m_sPSSource.AppendAscii( "        pd[i].x = randomX*4.0 - 2.0;\n" );
				pShader->m_sPSSource.AppendAscii( "        pd[i].y = randomY*4.0 - 2.0;\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n" );
			}

			if ( agk::GetShadowMappingMode() == 1 || agk::GetShadowMappingMode() == 2 ) // Uniform and LiPSM
			{
				pShader->m_sPSSource.AppendAscii( "    vec3 shadowUV = shadowVarying.xyz;\n");
				if ( agk::GetShadowMappingMode() == 2 ) // LiPSM
				{
					pShader->m_sPSSource.AppendAscii( "    shadowUV.xyz = shadowUV / shadowVarying.w;\n");
					pShader->m_sPSSource.AppendAscii( "    shadowUV.xy = shadowUV.xy*0.5 + 0.5;\n");
					pShader->m_sPSSource.AppendAscii( "    shadowUV.z = shadowUV.z*0.5 + 0.5;\n");
				}

				pShader->m_sPSSource.AppendAscii( "    float pixelDepth = shadowUV.z - bias;\n" );
				if ( !agk::GetRenderer()->SupportsPSHighP() ) pShader->m_sPSSource.AppendAscii( "    pixelDepth -= bias;\n" );

				if ( agk::GetShadowSmoothing() == 0 )
				{
					pShader->m_sPSSource.AppendAscii( "    float shadow = step(0.0,pixelDepth - texture2D( shadowMap, shadowUV.xy ).r);\n" );
				}
				else // multisampled
				{
					pShader->m_sPSSource.AppendAscii( "    float shadow = 0.0;\n" );
					pShader->m_sPSSource.AppendAscii( "    for( int i = 0; i < samples; i++ ) {\n" );
					pShader->m_sPSSource.AppendAscii( "        shadow += step(0.0,pixelDepth - texture2D( shadowMap, vec2(shadowUV.xy + pd[i]*agk_ShadowParams.zw) ).r);\n" );
					pShader->m_sPSSource.AppendAscii( "    }\n" );
					pShader->m_sPSSource.AppendAscii( "    shadow = shadow*sampleFactor;\n" );
				}

				pShader->m_sPSSource.AppendAscii( "    shadow *= step(0.0, agk_ShadowParams.x - depth);\n" );
			}
			else if ( agk::GetShadowMappingMode() == 3 ) // Cascade
			{
				pShader->m_sPSSource.AppendAscii( "    float shadow = 0.0;\n" );
				pShader->m_sPSSource.AppendAscii( "    if( depth < agk_ShadowParams2.w ) {\n");
				pShader->m_sPSSource.AppendAscii( "        float pixelDepth = shadow4Varying.z - bias;\n" );
				if ( agk::GetShadowSmoothing() > 0 ) 
				{
					pShader->m_sPSSource.AppendAscii( "        float dist = abs(texture2D( shadowMap4, shadow4Varying.xy ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        dist = min( dist*300.0, 1.0 );\n" );
					pShader->m_sPSSource.AppendAscii( "        for( int i = 0; i < 4; i++ ) shadow += step(0.0,pixelDepth - texture2D( shadowMap4, shadow4Varying.xy+pd[i]*agk_ShadowParams.zw*vec2(dist,dist) ).r);\n" );
					pShader->m_sPSSource.AppendAscii( "        shadow = shadow*0.25;\n" );
				}
				else pShader->m_sPSSource.AppendAscii( "        shadow = step(0.0,pixelDepth - texture2D( shadowMap4, shadow4Varying.xy ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n");
				pShader->m_sPSSource.AppendAscii( "    else if( depth < agk_ShadowParams2.z ) {\n");
				pShader->m_sPSSource.AppendAscii( "        float pixelDepth = shadow3Varying.z - 0.5*bias;\n" );
				if ( agk::GetShadowSmoothing() > 0 ) 
				{
					pShader->m_sPSSource.AppendAscii( "        float dist = abs(texture2D( shadowMap3, shadow3Varying.xy ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        dist = min( dist*300.0, 1.0 );\n" );
					pShader->m_sPSSource.AppendAscii( "        for( int i = 0; i < 4; i++ ) shadow += step(0.0,pixelDepth - texture2D( shadowMap3, shadow3Varying.xy+pd[i]*agk_ShadowParams.zw*vec2(dist,dist) ).r);\n" );
					pShader->m_sPSSource.AppendAscii( "        shadow = shadow*0.25;\n" );
				}
				else pShader->m_sPSSource.AppendAscii( "        shadow = step(0.0,pixelDepth - texture2D( shadowMap3, shadow3Varying.xy ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n");
				pShader->m_sPSSource.AppendAscii( "    else if( depth < agk_ShadowParams2.y ) {\n");
				pShader->m_sPSSource.AppendAscii( "        float pixelDepth = shadow2Varying.z - 0.25*bias;\n" );
				if ( agk::GetShadowSmoothing() > 0 ) 
				{
					pShader->m_sPSSource.AppendAscii( "        float dist = abs(texture2D( shadowMap2, shadow2Varying.xy ).r - pixelDepth);\n" );
					pShader->m_sPSSource.AppendAscii( "        dist = min( dist*300.0, 1.0 );\n" );
					pShader->m_sPSSource.AppendAscii( "        for( int i = 0; i < 4; i++ ) shadow += step(0.0,pixelDepth - texture2D( shadowMap2, shadow2Varying.xy+pd[i]*agk_ShadowParams.zw*vec2(dist,dist) ).r);\n" );
					pShader->m_sPSSource.AppendAscii( "        shadow = shadow*0.25;\n" );
				}
				else pShader->m_sPSSource.AppendAscii( "        shadow = step(0.0,pixelDepth - texture2D( shadowMap2, shadow2Varying.xy ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n");
				pShader->m_sPSSource.AppendAscii( "    else if( depth < agk_ShadowParams2.x ) {\n");
				pShader->m_sPSSource.AppendAscii( "        float pixelDepth = shadowVarying.z - bias;\n" );
				pShader->m_sPSSource.AppendAscii( "        shadow = step(0.0,pixelDepth - texture2D( shadowMap, shadowVarying.xy ).r);\n" );
				//if ( agk::GetShadowSmoothing() == 1 ) pShader->m_sPSSource.AppendAscii( "        for( int i = 0; i < 5; i++ ) shadow += step(0.0,pixelDepth - texture2D( shadowMap, shadowVarying.xy+pd[i]*agk_ShadowParams.zw ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    }\n");
				//pShader->m_sPSSource.AppendAscii( "    light += texture2D( shadowMap2, shadow2Varying.xy ).r;\n" );
			}

			/*
			// linear depth - multisampled
			if ( agk::GetShadowSmoothing() == 1 )
			{
				pShader->m_sPSSource.AppendAscii( "    vec2 uv = shadowUV.xy;\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow += step(0.0,pixelDepth - texture2D( shadowMap, vec2(uv.x-agk_ShadowParams.z*0.23550406,uv.y-agk_ShadowParams.w*0.09976554) ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow += step(0.0,pixelDepth - texture2D( shadowMap, vec2(uv.x+agk_ShadowParams.z*0.23639652,uv.y-agk_ShadowParams.w*0.19222681) ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow += step(0.0,pixelDepth - texture2D( shadowMap, vec2(uv.x-agk_ShadowParams.z*0.02354603,uv.y-agk_ShadowParams.w*0.23234718) ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow += step(0.0,pixelDepth - texture2D( shadowMap, vec2(uv.x+agk_ShadowParams.z*0.08623985,uv.y+agk_ShadowParams.w*0.07346941) ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow = shadow*0.2;\n" );
			}
			*/

			/*
			// nearest depth - bilinear filtering
			if ( agk::GetShadowSmoothing() == 2 )
			{
				pShader->m_sPSSource.AppendAscii( "    vec2 uv = shadowUV.xy;\n" );
				pShader->m_sPSSource.AppendAscii( "    float shadow2 = step(0.0,pixelDepth - texture2D( shadowMap, vec2(uv.x+agk_ShadowParams.z,uv.y) ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    float shadow3 = step(0.0,pixelDepth - texture2D( shadowMap, vec2(uv.x,uv.y+agk_ShadowParams.w) ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    float shadow4 = step(0.0,pixelDepth - texture2D( shadowMap, vec2(uv.x+agk_ShadowParams.z,uv.y+agk_ShadowParams.w) ).r);\n" );
				pShader->m_sPSSource.AppendAscii( "    vec2 f = fract(shadowUV.xy/agk_ShadowParams.zw);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow = mix(shadow, shadow2, f.x);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow3 = mix(shadow3, shadow4, f.x);\n" );
				pShader->m_sPSSource.AppendAscii( "    shadow = mix(shadow, shadow3, f.y);\n" );
			}
			*/
									
			
			pShader->m_sPSSource.AppendAscii( "    light += dirLight*(1.0-shadow);\n" );
			//pShader->m_sPSSource.AppendAscii( "    float color = 1.0-shadowUV.z;\n" );
			//pShader->m_sPSSource.AppendAscii( "    light += texture2D( shadowMap, shadowVarying.xy ).r;\n" );
			pShader->m_sPSSource.AppendAscii( "    }\n" );
		}
		else if ( normalMap )
		{
			pShader->m_sPSSource.AppendAscii( "    light += max(dot(-agk_DLightDir, normal),0.0) * agk_DLightColor;\n\n" );
		}
		
		for ( int i = 0; i < PSLights; ++i )
		{
			if ( i == 0 ) pShader->m_sPSSource.AppendAscii( "    vec3 dir;\n float atten;\n float lightRange;\n float intensity;\n" );
			pShader->m_sPSSource.AppendAscii( "    dir = (agk_PSLightPos[" ); pShader->m_sPSSource.AppendInt(i); pShader->m_sPSSource.AppendAscii( "].xyz - pos) * scale;\n" );
			pShader->m_sPSSource.AppendAscii( "    lightRange = agk_PSLightPos[" ); pShader->m_sPSSource.AppendInt(i); pShader->m_sPSSource.AppendAscii( "].w * scale2;\n" );
			pShader->m_sPSSource.AppendAscii( "    atten = max(0.0, 1.0 - dot(dir,dir)/lightRange);\n" ); // squared exponetial fall off
			pShader->m_sPSSource.AppendAscii( "    atten *= atten; atten *= atten;\n" ); // squared exponetial fall off

			pShader->m_sPSSource.AppendAscii( "    intensity = max(0.0,dot(normalize(dir),normal));\n" );

			pShader->m_sPSSource.AppendAscii( "    light += agk_PSLightColor[" ); pShader->m_sPSSource.AppendInt(i); pShader->m_sPSSource.AppendAscii( "].xyz * intensity * atten; \n" );
		}

		pShader->m_sPSSource.AppendAscii( "    return light;\n" );
		pShader->m_sPSSource.AppendAscii( "}\n" );
	}

	if ( pBaseShader->NeedsFog() )
	{
		if ( agk::GetFogMode() == 0 )
		{
			pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( mediump vec3 color, highp vec3 pointPos )\n" );
			pShader->m_sPSSource.AppendAscii( "{\n" );
			pShader->m_sPSSource.AppendAscii( "    return color;\n" );
			pShader->m_sPSSource.AppendAscii( "}\n" );
		}
		else
		{
			int fogColorsEqual = agk::GetFogColorsEqual();

			pShader->m_sPSSource.AppendAscii( '\n' );
			if ( pShader->m_sPSSource.FindStr("vec3 agk_CameraPos") < 0 ) pShader->m_sPSSource.AppendAscii( "uniform vec3 agk_CameraPos;\n" );
			pShader->m_sPSSource.AppendAscii( "uniform vec2 fogRange;\n" ); // = vec2( minDist, -4.0 / (maxDist-minDist) );
			pShader->m_sPSSource.AppendAscii( "uniform vec3 fogColor1;\n" ); // = vec3(0.63,0.73,0.82);
			if ( !fogColorsEqual ) pShader->m_sPSSource.AppendAscii( "uniform vec3 fogColor2;\n" ); // = vec3(1.0,0.9,0.7);
			pShader->m_sPSSource.AppendAscii( '\n' );
			pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( mediump vec3 color, highp vec3 pointPos )\n" );
			pShader->m_sPSSource.AppendAscii( "{\n" );
			pShader->m_sPSSource.AppendAscii( "    vec3 viewDir = agk_CameraPos - pointPos;\n" );
			pShader->m_sPSSource.AppendAscii( "    float invDist = inversesqrt(dot(viewDir,viewDir));\n" );
			if ( !fogColorsEqual ) pShader->m_sPSSource.AppendAscii( "    float sunPoint = dot(viewDir*invDist,agk_DLightDir)*0.499 + 0.5;\n" ); // iPad 3 (and maybe others) having floating point issues if exactly 0.5 is used
			pShader->m_sPSSource.AppendAscii( "    invDist = max( 0.0, 1.0/invDist - fogRange.x );\n" );
			pShader->m_sPSSource.AppendAscii( "    invDist = exp( invDist*fogRange.y );\n" ); // variable reuse
			if ( !fogColorsEqual ) pShader->m_sPSSource.AppendAscii( "    vec3 fogColor = mix(fogColor1, fogColor2, pow(sunPoint,24.0));\n" );
			else pShader->m_sPSSource.AppendAscii( "    vec3 fogColor = fogColor1;\n" );
			pShader->m_sPSSource.AppendAscii( "    color = mix( fogColor, color, invDist );\n" );
			pShader->m_sPSSource.AppendAscii( "    return color;\n" );
			pShader->m_sPSSource.AppendAscii( "}\n" );
		}
	}

	if ( pBaseShader->m_iFlags & AGK_SHADER_IS_CUSTOM ) pShader->m_iFlags |= AGK_SHADER_IS_CUSTOM;
	else pShader->m_iFlags &= ~AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	if ( !pShader->IsValid() )
	{
		delete pShader;
		return 0;
	}
	
	InternalEnumerateShaderResources( pShader );
	if ( !pShader->IsValid() ) 
	{
		delete pShader;
		return 0;
	}
	
	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadShader( const char *szVertexSource, const char *szPixelSource, const char *szVertexFile, const char *szPixelFile )
{
	if ( ((uint32_t*)szVertexSource)[0] == SpvMagicNumber || ((uint32_t*)szPixelSource)[0] == SpvMagicNumber )
	{
		AppError( "LoadShader() does not support SPIRV shaders, use LoadShaderSPIRV() instead" );
		return 0;
	}

	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( szVertexFile );
	pShader->m_sPSFilename.SetStr( szPixelFile );

	pShader->m_sVSSource.SetStr( szVertexSource );
	pShader->m_sPSSource.SetStr( szPixelSource );

	if ( pShader->m_sVSSource.FindStr( "vec3 GetVSLighting( mediump vec3 normal, highp vec3 pos );" ) >= 0 )
		pShader->m_iFlags |= AGK_SHADER_USES_VS_LIGHTING;

	if ( pShader->m_sPSSource.FindStr( "vec3 GetPSLighting( mediump vec3 normal, highp vec3 pos );" ) >= 0 )
		pShader->m_iFlags |= AGK_SHADER_USES_PS_LIGHTING;

	if ( pShader->m_sPSSource.FindStr( "vec3 ApplyFog( mediump vec3 color, highp vec3 pointPos );" ) >= 0 )
		pShader->m_iFlags |= AGK_SHADER_USES_FOG;

	if ( m_iSupportsPSHighP )
	{
#ifdef AGK_ANDROID
		// Nexus 5 reports high precision and fails if we try to use mediump (uses lowp instead) so we must use highp
		// This may cause performance problems, but we must use highp because at least 2 devices have the mediump bug!
		pShader->m_sPSSource.ReplaceStr( "mediump ", "highp " );

		// also change any shared uniforms in the vertex shader as this may cause the linker to fail
		pShader->m_sVSSource.ReplaceStr( "mediump float agk_", "highp float agk_" );
		pShader->m_sVSSource.ReplaceStr( "mediump vec2 agk_", "highp vec2 agk_" );
		pShader->m_sVSSource.ReplaceStr( "mediump vec3 agk_", "highp vec3 agk_" );
		pShader->m_sVSSource.ReplaceStr( "mediump vec4 agk_", "highp vec4 agk_" );
#endif
	}
	else
	{
		static int warned = 0;
		if ( !warned )
		{
			warned = 1;
			agk::Warning( "Device does not support high precision pixel shader values" );
		}
		pShader->m_sPSSource.ReplaceStr( "highp ", "mediump " );

		// also change any shared uniforms in the vertex shader as this may cause the linker to fail
		pShader->m_sVSSource.ReplaceStr( "highp float agk_", "mediump float agk_" );
		pShader->m_sVSSource.ReplaceStr( "highp vec2 agk_", "mediump vec2 agk_" );
		pShader->m_sVSSource.ReplaceStr( "highp vec3 agk_", "mediump vec3 agk_" );
		pShader->m_sVSSource.ReplaceStr( "highp vec4 agk_", "mediump vec4 agk_" );
	}	

	pShader->m_sVSSource.ReplaceStr( "#version 110", "" );
    pShader->m_sPSSource.ReplaceStr( "#version 110", "" );

    // add precision if not already present
    if ( pShader->m_sVSSource.FindStr("precision highp") <= 0 && pShader->m_sVSSource.FindStr("precision mediump") <= 0 )
    {
        pShader->m_sVSSource.Prepend( "precision highp float;\n" );
    }

    if ( pShader->m_sPSSource.FindStr("precision highp") <= 0 && pShader->m_sPSSource.FindStr("precision mediump") <= 0 )
    {
#ifdef AGK_ANDROID
        if ( m_iSupportsPSHighP ) pShader->m_sPSSource.Prepend( "precision highp float;\n" );
        else pShader->m_sPSSource.Prepend( "precision mediump float;\n" );
#else
        pShader->m_sPSSource.Prepend( "precision mediump float;\n" );
#endif
    }

	pShader->m_iFlags |= AGK_SHADER_IS_CUSTOM;
	
	// if the shader needs lighting then it can't be compiled until it has the lighting section generated
	if ( !pShader->NeedsAdditionalCode() ) 
	{
		InternalCompileShader( pShader );
		if ( !pShader->IsValid() ) 
		{
			delete pShader;
			return 0;
		}

		InternalEnumerateShaderResources( pShader );
		if ( !pShader->IsValid() ) 
		{
			delete pShader;
			return 0;
		}
	}
	else pShader->SetValid( true ); // assume it's vaild for now, will find out later when we add the missing code
	
	return pShader;
}

AGKShader* OpenGLES2Renderer::LoadShaderSPIRV( uint32_t *pVertexSource, uint32_t sizeVS, uint32_t *pPixelSource, uint32_t sizePS )
{
	if ( m_iShaderErrorMode == 0 ) agk::Error( "Failed to load shader, SPIRV is not supported in OpenGLES2" );
	return 0;
}

void OpenGLES2Renderer::ReloadShader( AGKShader *pShader )
{
	if( pShader->NeedsAdditionalCode() ) return;
	if ( !pShader->m_pInternalResources ) return;
	OpenGLES2Shader *pOpenGLES2Shader = (OpenGLES2Shader*) pShader->m_pInternalResources;

	if ( m_iCurrShader == pOpenGLES2Shader->m_iShaderID )
	{
		glUseProgram( 0 );
		m_iCurrShader = 0;
		m_pCurrAGKShader = 0;
		m_iVBOPtrSetup = 0;
	}

	if ( pOpenGLES2Shader->m_iShaderID ) glDeleteProgram( pOpenGLES2Shader->m_iShaderID );
	pOpenGLES2Shader->m_iShaderID = 0;

	InternalCompileShader( pShader );

	pShader->m_iFlags |= AGK_SHADER_RELOAD_UNIFORMS;
}

void OpenGLES2Renderer::DeleteShader( AGKShader *pShader )
{
	if ( !pShader->m_pInternalResources ) return;
	OpenGLES2Shader *pOpenGLES2Shader = (OpenGLES2Shader*) pShader->m_pInternalResources;

	if ( m_iCurrShader == pOpenGLES2Shader->m_iShaderID )
	{
		glUseProgram( 0 );
		m_iCurrShader = 0;
		m_pCurrAGKShader = 0;
		m_iVBOPtrSetup = 0;
	}

	if ( pOpenGLES2Shader->m_iShaderID ) glDeleteProgram( pOpenGLES2Shader->m_iShaderID );
	pOpenGLES2Shader->m_iShaderID = 0;

	delete pOpenGLES2Shader;
	pShader->m_pInternalResources = 0;
}

void OpenGLES2Renderer::InternalCompileShader( AGKShader *pShader )
{
	OpenGLES2Shader *pOpenGLES2Shader = new OpenGLES2Shader();
	pShader->m_pInternalResources = pOpenGLES2Shader;
	pShader->SetValid( true );

	pOpenGLES2Shader->m_iShaderID = glCreateProgram();

	// compile vertex shaderl
	const char *szVertexSource = pShader->m_sVSSource.GetStr();
	uint32_t vertShader = glCreateShader ( GL_VERTEX_SHADER );
	glShaderSource( vertShader, 1, &szVertexSource, NULL );
	glCompileShader( vertShader );

	int status = 0;
	int logLength = 0;
	glGetShaderiv ( vertShader, GL_COMPILE_STATUS, &status );
	if ( status == 0 )
	{
		glGetShaderiv ( vertShader, GL_INFO_LOG_LENGTH, &logLength );
		if ( logLength > 1 )
		{
			char *log = new char[ logLength+1 ];
			glGetShaderInfoLog( vertShader, logLength, &logLength, log );
			pShader->m_sVSLog.SetStr( log );
			delete [] log;
		}

		glDeleteShader( vertShader );
		if ( m_iShaderErrorMode < 2 )
		{
			uString info;
			info.Format( "Vertex shader \"%s\" (based on \"%s\") failed to compile: %s", pShader->GetVSFilename(), pShader->GetBaseVSFilename(), pShader->m_sVSLog.GetStr() );
			agk::Error( info );
			agk::Message( info );
		}
		pShader->SetValid( false );
		return;
	}

	// compile fragment shader
	const char *szFragSource = pShader->m_sPSSource.GetStr();
	uint32_t fragShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource ( fragShader, 1, &szFragSource, NULL );
	glCompileShader ( fragShader );
	
	glGetShaderiv ( fragShader, GL_COMPILE_STATUS, &status );
	if ( status == 0 )
	{
		glGetShaderiv ( fragShader, GL_INFO_LOG_LENGTH, &logLength );
		if ( logLength > 1 )
		{
			char *log = new char[ logLength+1 ];
			glGetShaderInfoLog( fragShader, logLength, &logLength, log );
			pShader->m_sPSLog.SetStr( log );
			delete [] log;
		}

		glDeleteShader( vertShader );
		glDeleteShader( fragShader );
		if ( m_iShaderErrorMode < 2 )
		{
			uString info;
			info.Format( "Pixel shader \"%s\" (based on \"%s\") failed to compile: %s", pShader->GetPSFilename(), pShader->GetBasePSFilename(), pShader->m_sPSLog.GetStr() );
			agk::Error( info );
			agk::Message( info );
		}
		pShader->SetValid( false );
		return;
	}

	glAttachShader( pOpenGLES2Shader->m_iShaderID, vertShader );
	glAttachShader( pOpenGLES2Shader->m_iShaderID, fragShader );

	// link program
	glLinkProgram( pOpenGLES2Shader->m_iShaderID );

	glGetProgramiv( pOpenGLES2Shader->m_iShaderID, GL_LINK_STATUS, &status );
	if ( status == 0 )
	{
		glDeleteShader( vertShader );
		glDeleteShader( fragShader );

		glGetProgramiv ( pOpenGLES2Shader->m_iShaderID, GL_INFO_LOG_LENGTH, &logLength);
		if ( logLength > 0 )
		{
			char *log = new char[ logLength+1 ];
			glGetProgramInfoLog( pOpenGLES2Shader->m_iShaderID, logLength, &logLength, log );
			pShader->m_sLinkLog.SetStr( log );
			delete [] log;
		}

		if ( m_iShaderErrorMode < 2 )
		{
			uString info;
			info.Format( "Vertex shader \"%s\" (based on \"%s\") and pixel shader \"%s\" (based on \"%s\") failed to link: %s", pShader->GetVSFilename(), pShader->GetBaseVSFilename(), pShader->GetPSFilename(), pShader->GetBasePSFilename(), pShader->m_sLinkLog.GetStr() );
			agk::Error( info );
			agk::Message( info );
		}
		pShader->SetValid( false );
		return;
	}

	glDeleteShader( vertShader );
	glDeleteShader( fragShader );

	return;
}

void OpenGLES2Renderer::InternalEnumerateShaderResources( AGKShader *pShader )
{
	if ( !pShader->m_pInternalResources ) return;
	OpenGLES2Shader *pOpenGLES2Shader = (OpenGLES2Shader*) pShader->m_pInternalResources;
	
	// bind it so we can set the texture locations (if any)
	if ( m_iCurrShader != pOpenGLES2Shader->m_iShaderID )
	{
		glUseProgram( pOpenGLES2Shader->m_iShaderID );
		m_iCurrShader = pOpenGLES2Shader->m_iShaderID;
		m_iVBOPtrSetup = 0;
	}

	// attributes
	if ( pShader->m_pAttribs ) delete [] pShader->m_pAttribs;
	pShader->m_pAttribs = 0;
	pShader->m_iNumAttribs = 0;

	glGetProgramiv( pOpenGLES2Shader->m_iShaderID, GL_ACTIVE_ATTRIBUTES, &pShader->m_iNumAttribs );
	if ( pShader->m_iNumAttribs > 0 )
	{
		pShader->m_pAttribs = new AGKShaderAttrib[ pShader->m_iNumAttribs ];

		int maxLength = 0;
		glGetProgramiv( pOpenGLES2Shader->m_iShaderID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength );
		char* szAttribName = new char[ maxLength+1 ];

		int length = 0;
		int size = 0;
		uint32_t type = 0;
		for ( int i = 0; i < pShader->m_iNumAttribs; i++ )
		{
			AGKShaderAttrib *pAttrib = &(pShader->m_pAttribs[ i ]);
			glGetActiveAttrib( pOpenGLES2Shader->m_iShaderID, i, maxLength, &length, &size, &type, szAttribName );
			pAttrib->m_iLocation = glGetAttribLocation( pOpenGLES2Shader->m_iShaderID, szAttribName );
			switch( type )
			{
				case GL_FLOAT: pAttrib->m_iComponents = 1; break;
				case GL_FLOAT_VEC2: pAttrib->m_iComponents = 2; break;
				case GL_FLOAT_VEC3: pAttrib->m_iComponents = 3; break;
				case GL_FLOAT_VEC4: pAttrib->m_iComponents = 4; break;
				default:
				{
					if ( m_iShaderErrorMode < 1 )
					{
						uString info;
						info.Format( "Shader attribute \"%s\" type is not supported. Only float, vec2, vec3, and vec4 are allowed", szAttribName );
						agk::Error( info );
						agk::Message( info );
					}
					pShader->SetValid( false );
					return;
				}
			}

			if ( strcmp(szAttribName, "position") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_POS;
			else if ( strcmp(szAttribName, "normal") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_NORM;
			else if ( strcmp(szAttribName, "tangent") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_TANGENT;
			else if ( strcmp(szAttribName, "binormal") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_BINORMAL;
			else if ( strcmp(szAttribName, "color") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_COLOR;
			else if ( strcmp(szAttribName, "uv") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_UV0;
			else if ( strcmp(szAttribName, "uv1") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_UV1;
			else if ( strcmp(szAttribName, "boneweights") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_BONE_WEIGHT;
			else if ( strcmp(szAttribName, "boneindices") == 0 ) pAttrib->m_iKnownName = AGK_SHADER_ATTRIB_NAME_BONE_INDEX;
			else 
			{
				pAttrib->m_szName = new char[ length+1 ];
				memcpy( (void*)pAttrib->m_szName, szAttribName, length+1 );
			}
		}

		delete [] szAttribName;
	}

	// uniforms
	pShader->m_constantBindingByName.ClearAll();
	pShader->m_localConstants.Clear( true );

	for( int i = 0; i < AGK_SHADER_CONSTANT_NAME_MAX_VALUE; i++ ) pOpenGLES2Shader->m_indexByKnownName[ i ] = (uint16_t) -1;
	if ( pOpenGLES2Shader->m_pLocations ) delete [] pOpenGLES2Shader->m_pLocations;
	pOpenGLES2Shader->m_iNumLocations = 0;

	for( int i = 0; i < AGK_OPENGLES2_MAX_TEXTURE_STAGES; i++ )
	{
		pOpenGLES2Shader->m_iTextureLocations[ i ] = (uint16_t) -1;
	}
	
	int numUniforms;
	glGetProgramiv( pOpenGLES2Shader->m_iShaderID, GL_ACTIVE_UNIFORMS, &numUniforms );
	if ( numUniforms > 65000 )
	{
		if ( m_iShaderErrorMode < 2 ) agk::Error( "Shader has too many uniform values" );
		pShader->SetValid( false );
		return;
	}

	if ( numUniforms > 0 )
	{
		pShader->m_constantBindingByName.Resize( numUniforms + numUniforms/2 );
		pOpenGLES2Shader->m_pLocations = new OpenGLES2ShaderLocation[ numUniforms ];

		int maxLength = 0;
		glGetProgramiv( pOpenGLES2Shader->m_iShaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength );
		char* szUniformName = new char[ maxLength+1 ];

		uint16_t locationIndex = 0;
		int length = 0;
		int size = 0;
		uint32_t type = 0;
		for ( int i = 0; i < numUniforms; i++ )
		{
			glGetActiveUniform( pOpenGLES2Shader->m_iShaderID, i, maxLength, &length, &size, &type, szUniformName );
			uint32_t location = (uint32_t) glGetUniformLocation( pOpenGLES2Shader->m_iShaderID, szUniformName );
			pOpenGLES2Shader->m_pLocations[ locationIndex ].m_iLocation = location;
			switch( type )
			{
				case GL_FLOAT: pOpenGLES2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_FLOAT; break;
				case GL_FLOAT_VEC2: pOpenGLES2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_VEC2; break;
				case GL_FLOAT_VEC3: pOpenGLES2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_VEC3; break;
				case GL_FLOAT_VEC4: pOpenGLES2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_VEC4; break;
				case GL_FLOAT_MAT2: pOpenGLES2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_MAT2; break;
				case GL_FLOAT_MAT3: pOpenGLES2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_MAT3; break;
				case GL_FLOAT_MAT4: pOpenGLES2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_MAT4; break;
				case GL_SAMPLER_2D: break;
				case GL_SAMPLER_2D_EXTERNAL: break;
				default: 
				{
					if ( m_iShaderErrorMode < 2 ) 
					{
						uString err; err.Format( "Unsupported shader variable type, only float, vec2, vec3, vec4, mat2, mat3, mat4, and sampler2D are supported: %d", type );
						agk::Error( err );
					}
				}
			}
			
			// remove array brackets
			char *str = szUniformName;
			while( *str )
			{
				if ( *str == '[' ) { *str = 0; break; }
				str++;
			}

			uint8_t iTextureIsSet[ AGK_OPENGLES2_MAX_TEXTURE_STAGES ] = {0};

			if ( strcmp(szUniformName, "fogColor1") == 0 )
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varFogColor1;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_FOG_COLOR1 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "fogColor2") == 0 ) 			
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varFogColor2;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_FOG_COLOR2 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "fogRange") == 0 )				pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varFogRange;
			else if ( strcmp(szUniformName, "agk_invert") == 0 ) 			pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varInvert;
			else if ( strcmp(szUniformName, "agk_time") == 0 ) 				pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varTime;
			else if ( strcmp(szUniformName, "agk_sintime") == 0 ) 			pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varSinTime;
			else if ( strcmp(szUniformName, "agk_resolution") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varResolution;
			else if ( strcmp(szUniformName, "agk_ShadowParams") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowParams;
			else if ( strcmp(szUniformName, "agk_ShadowParams2") == 0 ) 	pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowParams2;
			else if ( strcmp(szUniformName, "agk_Ortho") == 0 ) 			pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varOrtho;
			else if ( strcmp(szUniformName, "agk_ShadowProj") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowProj;
			else if ( strcmp(szUniformName, "agk_Shadow2Proj") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowProj2;
			else if ( strcmp(szUniformName, "agk_Shadow3Proj") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowProj3;
			else if ( strcmp(szUniformName, "agk_Shadow4Proj") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowProj4;
			else if ( strcmp(szUniformName, "agk_DLightDir") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varDirLightDir;
			else if ( strcmp(szUniformName, "agk_DLightColor") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varDirLightColor;
			else if ( strcmp(szUniformName, "agk_LightAmbient") == 0 ) 		pOpenGLES2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varLightAmbient;

			else if ( strcmp(szUniformName, "agk_bonequats1") == 0 )		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_OBJ_BONES1 ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_bonequats2") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_OBJ_BONES2 ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_MeshDiffuse") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_MESH_DIFFUSE ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_MeshEmissive") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_MESH_EMISSIVE ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_NormalScale") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_NORMAL_SCALE ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_CameraPos") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_CAMERA_POS ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_spritepos") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_POS ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_spritesize") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_SIZE ] = locationIndex++;
																							
			else if ( strcmp(szUniformName, "agk_World") == 0 ) 			pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_MAT ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_WorldNormal") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_NORMAL ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_View") == 0 ) 				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_Proj") == 0 ) 				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PROJ ] = locationIndex++;
																										
			else if ( strcmp(szUniformName, "agk_ViewProj") == 0 ) 			pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW_PROJ ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_WorldViewProj") == 0 ) 	pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_VIEW_PROJ ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_ShadowWorldProj") == 0 ) 	pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SHADOW_WORLD_PROJ ] = locationIndex++;
																							
			else if ( strcmp(szUniformName, "agk_VSLightPos") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VS_LIGHT_POS ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_VSLightColor") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VS_LIGHT_COLOR ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_PSLightPos") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PS_LIGHT_POS ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_PSLightColor") == 0 ) 		pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PS_LIGHT_COLOR ] = locationIndex++;
																										
			else if ( strcmp(szUniformName, "uvBounds0") == 0 ) 			
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_0 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "uvBounds1") == 0 )
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_1 ] = locationIndex++;
				
			}
			else if ( strcmp(szUniformName, "uvBounds2") == 0 ) 
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_2 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "uvBounds3") == 0 ) 
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_3 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "uvBounds4") == 0 )
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_4 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "uvBounds5") == 0 )
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_5 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "uvBounds6") == 0 )
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_6 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "uvBounds7") == 0 )
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_7 ] = locationIndex++;
			}
																							
			else if ( strcmp(szUniformName, "textureBounds0") == 0 )
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_0 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds1") == 0 ) 
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_1 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds2") == 0 ) 
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_2 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds3") == 0 ) 
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_3 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds4") == 0 )
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_4 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds5") == 0 ) 
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_5 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds6") == 0 ) 
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_6 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds7") == 0 ) 
			{
				pOpenGLES2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_7 ] = locationIndex++;
			}
																											
			else if ( strcmp(szUniformName, "texture0") == 0 ) { iTextureIsSet[ 0 ] = 1; pOpenGLES2Shader->m_iTextureLocations[ 0 ] = location; glUniform1i( location, 0 ); }
			else if ( strcmp(szUniformName, "texture1") == 0 ) { iTextureIsSet[ 1 ] = 1; pOpenGLES2Shader->m_iTextureLocations[ 1 ] = location; glUniform1i( location, 1 ); }
			else if ( strcmp(szUniformName, "texture2") == 0 ) { iTextureIsSet[ 2 ] = 1; pOpenGLES2Shader->m_iTextureLocations[ 2 ] = location; glUniform1i( location, 2 ); }
			else if ( strcmp(szUniformName, "texture3") == 0 ) { iTextureIsSet[ 3 ] = 1; pOpenGLES2Shader->m_iTextureLocations[ 3 ] = location; glUniform1i( location, 3 ); }
			else if ( strcmp(szUniformName, "texture4") == 0 ) { iTextureIsSet[ 4 ] = 1; pOpenGLES2Shader->m_iTextureLocations[ 4 ] = location; glUniform1i( location, 4 ); }
			else if ( strcmp(szUniformName, "texture5") == 0 ) { iTextureIsSet[ 5 ] = 1; pOpenGLES2Shader->m_iTextureLocations[ 5 ] = location; glUniform1i( location, 5 ); }
			else if ( strcmp(szUniformName, "texture6") == 0 ) { iTextureIsSet[ 6 ] = 1; pOpenGLES2Shader->m_iTextureLocations[ 6 ] = location; glUniform1i( location, 6 ); }
			else if ( strcmp(szUniformName, "texture7") == 0 ) { iTextureIsSet[ 7 ] = 1; pOpenGLES2Shader->m_iTextureLocations[ 7 ] = location; glUniform1i( location, 7 ); }

			else if ( strcmp(szUniformName, "cubeMap0") == 0 ) { iTextureIsSet[ 0 ] = 2; pOpenGLES2Shader->m_iTextureLocations[ 0 ] = location; glUniform1i( location, 0 ); }
			else if ( strcmp(szUniformName, "cubeMap1") == 0 ) { iTextureIsSet[ 1 ] = 2; pOpenGLES2Shader->m_iTextureLocations[ 1 ] = location; glUniform1i( location, 1 ); }
			else if ( strcmp(szUniformName, "cubeMap2") == 0 ) { iTextureIsSet[ 2 ] = 2; pOpenGLES2Shader->m_iTextureLocations[ 2 ] = location; glUniform1i( location, 2 ); }
			else if ( strcmp(szUniformName, "cubeMap3") == 0 ) { iTextureIsSet[ 3 ] = 2; pOpenGLES2Shader->m_iTextureLocations[ 3 ] = location; glUniform1i( location, 3 ); }
			else if ( strcmp(szUniformName, "cubeMap4") == 0 ) { iTextureIsSet[ 4 ] = 2; pOpenGLES2Shader->m_iTextureLocations[ 4 ] = location; glUniform1i( location, 4 ); }
			else if ( strcmp(szUniformName, "cubeMap5") == 0 ) { iTextureIsSet[ 5 ] = 2; pOpenGLES2Shader->m_iTextureLocations[ 5 ] = location; glUniform1i( location, 5 ); }
			else if ( strcmp(szUniformName, "cubeMap6") == 0 ) { iTextureIsSet[ 6 ] = 2; pOpenGLES2Shader->m_iTextureLocations[ 6 ] = location; glUniform1i( location, 6 ); }
			else if ( strcmp(szUniformName, "cubeMap7") == 0 ) { iTextureIsSet[ 7 ] = 2; pOpenGLES2Shader->m_iTextureLocations[ 7 ] = location; glUniform1i( location, 7 ); }
			
			else if ( strcmp(szUniformName, "shadowMap") == 0 ) 
			{ 
				if ( iTextureIsSet[ 7 ] && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 7, it is overriden by shadow mapping" );
				pOpenGLES2Shader->m_iTextureLocations[ 7 ] = location;
				glUniform1i( location, 7 );
			}
			else if ( strcmp(szUniformName, "shadowMap2") == 0 ) 
			{ 
				if ( iTextureIsSet[ 6 ] && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 6, it is overriden by cascade shadow mapping" );
				pOpenGLES2Shader->m_iTextureLocations[ 6 ] = location;
				glUniform1i( location, 6 );
			}
			else if ( strcmp(szUniformName, "shadowMap3") == 0 ) 
			{ 
				if ( iTextureIsSet[ 5 ] && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 5, it is overriden by cascade shadow mapping" );
				pOpenGLES2Shader->m_iTextureLocations[ 5 ] = location;
				glUniform1i( location, 5 );
			}
			else if ( strcmp(szUniformName, "shadowMap4") == 0 ) 
			{ 
				if ( iTextureIsSet[ 4 ] && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 4, it is overriden by cascade shadow mapping" );
				pOpenGLES2Shader->m_iTextureLocations[ 4 ] = location;
				glUniform1i( location, 4 );
			}

			else
			{
				pShader->m_constantBindingByName.AddItem( locationIndex+1, szUniformName ); // +1 to avoid adding zero which is a null element
				locationIndex++;
			}
		}

		delete [] szUniformName;

		if ( pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_POS ] != (uint16_t) -1
		  || pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_SIZE ] != (uint16_t) -1 )
		{
			pShader->m_iFlags |= AGK_SHADER_PER_SPRITE_UNIFORM;
		}

		pOpenGLES2Shader->m_iNumLocations = locationIndex;
	}
}

void OpenGLES2Renderer::SetShaderConstant( AGKShader *pShader, uint32_t index, AGKShaderConstantValue *pConstant )
{
	if ( !pShader ) return;
	OpenGLES2Shader *pOpenGLES2Shader = (OpenGLES2Shader*) (pShader->m_pInternalResources);
	if ( !pOpenGLES2Shader ) return;
	if ( index >= pOpenGLES2Shader->m_iNumLocations ) return;

	if ( (pOpenGLES2Shader->m_pLocations[ index ].m_iFlags & AGK_OPENGLES2_SHADER_LOCATION_USES_LOCAL_MEM) && pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue )
	{
		delete pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue;
	}
	pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue = pConstant;
}

int OpenGLES2Renderer::BindShaderConstants( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKUnorderedArray<AGKShaderConstantValue*> *pAdditionalConstants, 
										  AGKDynamicArray<cImage*> *pTextures, void **inoutConstants )
{
	if ( !pShader || !pShader->m_pInternalResources ) return APP_ERROR_GENERAL;
	OpenGLES2Shader *pOpenGLES2Shader = (OpenGLES2Shader*) pShader->m_pInternalResources;
	if ( m_iCurrShader != pOpenGLES2Shader->m_iShaderID )
	{
		glUseProgram( pOpenGLES2Shader->m_iShaderID );
		m_iCurrShader = pOpenGLES2Shader->m_iShaderID;
		m_iVBOPtrSetup = 0;
	}

	// reset location flags
	for( uint32_t i = 0; i < pOpenGLES2Shader->m_iNumLocations; i++ )
	{
		pOpenGLES2Shader->m_pLocations[ i ].UnSet();
		if ( pShader->m_iFlags & AGK_SHADER_RELOAD_UNIFORMS ) pOpenGLES2Shader->m_pLocations[ i ].Reload();
	}

	// bind images and check for image texture bound constants
	if ( pTextures )
	{
		for( int i = 0; i <= pTextures->GetHighestIndex(); i++ )
		{
			cImage *pImage = pTextures->GetItem( i );
			if ( pImage && pOpenGLES2Shader->m_iTextureLocations[ i ] != (uint16_t) -1 )
			{
				BindTexture( pImage->GetInternalResource(), i ); // null image is accepted
				
				// check for texture bounds
				uint16_t index = pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_0 + i ];
				if ( index != (uint16_t) -1 && pImage )
				{
					pOpenGLES2Shader->m_pLocations[ index ].Set();
					if ( pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue != pImage->GetShaderVarTexBounds() )
					{
						pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue = pImage->GetShaderVarTexBounds();
						pOpenGLES2Shader->m_pLocations[ index ].Reload();
					}
				}
			}
		}

		for( int i = pTextures->GetHighestIndex()+1; i < AGK_OPENGLES2_MAX_TEXTURE_STAGES; i++ )
		{
			if ( pOpenGLES2Shader->m_iTextureLocations[ i ] != (uint16_t) -1 ) BindTexture( 0, i );
		}
	}
	else
	{
		// no textures
		for( uint32_t i = 0; i < AGK_OPENGLES2_MAX_TEXTURE_STAGES; i++ )
		{
			if ( pOpenGLES2Shader->m_iTextureLocations[ i ] != (uint16_t) -1 ) BindTexture( 0, i );
		}
	}

	// first set of constants
	AGKShaderConstantValue *pWorldConstant = 0;
	if ( pConstants )
	{
		for( uint32_t i = 0; i < pConstants->NumItems(); i++ )
		{
			AGKShaderConstantValue *pConstant = pConstants->GetItem( i );
			uint16_t index = (uint16_t) -1;
			switch( pConstant->GetNameType() )
			{
				case AGK_SHADER_CONSTANT_NAME_TYPE_KNOWN:
				{
					index = pOpenGLES2Shader->m_indexByKnownName[ pConstant->GetKnownName() ];
					if ( pConstant->GetKnownName() == AGK_SHADER_CONSTANT_NAME_WORLD_MAT && !pWorldConstant ) pWorldConstant = pConstant;
					break;
				}
				case AGK_SHADER_CONSTANT_NAME_TYPE_USER:
				{
					index = (uint16_t) pShader->GetBindingForName( pConstant->GetName() );
					break;
				}
				case AGK_SHADER_CONSTANT_NAME_TYPE_BINDING:
				{
					index = pConstant->GetBindingName();
					break;
				}
			}

			if ( index == (uint16_t) -1 ) continue;

			if ( pOpenGLES2Shader->m_pLocations[ index ].IsSet() ) continue;
			pOpenGLES2Shader->m_pLocations[ index ].Set();
			if ( pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue != pConstant )
			{
				pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue = pConstant;
				pOpenGLES2Shader->m_pLocations[ index ].Reload();
			}
			// check type?
		}
	}

	// second set of constants
	if ( pAdditionalConstants )
	{
		for( uint32_t i = 0; i < pAdditionalConstants->NumItems(); i++ )
		{
			AGKShaderConstantValue *pConstant = pAdditionalConstants->GetItem( i );
			uint16_t index = (uint16_t) -1;
			switch( pConstant->GetNameType() )
			{
				case AGK_SHADER_CONSTANT_NAME_TYPE_KNOWN:
				{
					index = pOpenGLES2Shader->m_indexByKnownName[ pConstant->GetKnownName() ];
					if ( pConstant->GetKnownName() == AGK_SHADER_CONSTANT_NAME_WORLD_MAT && !pWorldConstant ) pWorldConstant = pConstant;
					break;
				}
				case AGK_SHADER_CONSTANT_NAME_TYPE_USER:
				{
					index = (uint16_t) pShader->GetBindingForName( pConstant->GetName() );
					break;
				}
				case AGK_SHADER_CONSTANT_NAME_TYPE_BINDING:
				{
					index = pConstant->GetBindingName();
					break;
				}
			}

			if ( index == (uint16_t) -1 ) continue;

			if ( pOpenGLES2Shader->m_pLocations[ index ].IsSet() ) continue;
			pOpenGLES2Shader->m_pLocations[ index ].Set();
			if ( pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue != pConstant )
			{
				pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue = pConstant;
				pOpenGLES2Shader->m_pLocations[ index ].Reload();
			}
			// check type?
		}
	}

	// camera constants
	cCamera *pCamera = agk::GetCurrentCamera();

	// camera view matrix
	uint16_t index = pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW ];
	if ( index != (uint16_t) -1 && !pOpenGLES2Shader->m_pLocations[ index ].IsSet() )
	{
		pOpenGLES2Shader->m_pLocations[ index ].Set();
		AGKShaderConstantValue *pConstantView = pCamera->GetShaderView();
		if ( pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue != pConstantView )
		{
			pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue = pConstantView;
			pOpenGLES2Shader->m_pLocations[ index ].Reload();
		}
	}

	// camera proj matrix
	index = pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PROJ ];
	if ( index != (uint16_t) -1 && !pOpenGLES2Shader->m_pLocations[ index ].IsSet() )
	{
		pOpenGLES2Shader->m_pLocations[ index ].Set();
		AGKShaderConstantValue *pConstantProj = pCamera->GetShaderProj();
		if ( pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue != pConstantProj )
		{
			pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue = pConstantProj;
			pOpenGLES2Shader->m_pLocations[ index ].Reload();
		}
	}

	// camera viewProj matrix
	index = pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW_PROJ ];
	if ( index != (uint16_t) -1 && !pOpenGLES2Shader->m_pLocations[ index ].IsSet() )
	{
		pOpenGLES2Shader->m_pLocations[ index ].Set();
		AGKShaderConstantValue *pConstantViewProj = pCamera->GetShaderViewProj();
		if ( pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue != pConstantViewProj )
		{
			pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue = pConstantViewProj;
			pOpenGLES2Shader->m_pLocations[ index ].Reload();
		}
	}

	// camera pos
	index = pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_CAMERA_POS ];
	if ( index != (uint16_t) -1 && !pOpenGLES2Shader->m_pLocations[ index ].IsSet() )
	{
		pOpenGLES2Shader->m_pLocations[ index ].Set();
		AGKShaderConstantValue *pConstantCamPos = pCamera->GetShaderPos();
		if ( pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue != pConstantCamPos )
		{
			pOpenGLES2Shader->m_pLocations[ index ].m_pCurrValue = pConstantCamPos;
			pOpenGLES2Shader->m_pLocations[ index ].Reload();
		}
	}
	
	// calculate WVP matrix
	index = pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_VIEW_PROJ ];
	if ( index != (uint16_t) -1 && !pOpenGLES2Shader->m_pLocations[ index ].IsSet() && pWorldConstant )
	{
		// don't set it as it is a default value
		
		if ( !pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue )
		{
			pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue = new AGKShaderConstantValue();
			pOpenGLES2Shader->m_pLocations[ index ].m_iFlags |= AGK_OPENGLES2_SHADER_LOCATION_USES_LOCAL_MEM;
		}

		AGKShaderConstantValue *pConstantViewProj = pCamera->GetShaderViewProj();
		AGKMatrix4 matWVP, matViewProj;
		matWVP.Set( pWorldConstant->GetValues() );
		matViewProj.Set( pConstantViewProj->GetValues() );
		matWVP.Mult( matViewProj );

		pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue->SetMatrix4( matWVP.GetFloatPtr() );
	}

	// calculate WorldShadow matrix
	index = pOpenGLES2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SHADOW_WORLD_PROJ ];
	if ( index != (uint16_t) -1 && !pOpenGLES2Shader->m_pLocations[ index ].IsSet() && pWorldConstant )
	{
		// don't set it as it is a default value
		
		if ( !pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue )
		{
			pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue = new AGKShaderConstantValue();
			pOpenGLES2Shader->m_pLocations[ index ].m_iFlags |= AGK_OPENGLES2_SHADER_LOCATION_USES_LOCAL_MEM;
		}

		AGKMatrix4 matWS, matShadowProj;
		matWS.Set( pWorldConstant->GetValues() );
		matShadowProj.Set( AGKShader::g_varShadowProj.GetValues() );
		matWS.Mult( matShadowProj );

		pOpenGLES2Shader->m_pLocations[ index ].m_pDefaultValue->SetMatrix4( matWS.GetFloatPtr() );
	}
	
	// set locations
	for( uint32_t i = 0; i < pOpenGLES2Shader->m_iNumLocations; i++ )
	{
		if ( !pOpenGLES2Shader->m_pLocations[ i ].IsSet() )
		{
			if ( pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue != pOpenGLES2Shader->m_pLocations[ i ].m_pDefaultValue )
			{
				pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue = pOpenGLES2Shader->m_pLocations[ i ].m_pDefaultValue;
				pOpenGLES2Shader->m_pLocations[ i ].Reload();
			}
		}

		if ( !pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue ) continue;

		if ( pOpenGLES2Shader->m_pLocations[ i ].ShouldReload()
		  || pOpenGLES2Shader->m_pLocations[ i ].m_iChangeVersion != pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetChangeVersion() )
		{
			pOpenGLES2Shader->m_pLocations[ i ].m_iChangeVersion = pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetChangeVersion();
			pOpenGLES2Shader->m_pLocations[ i ].Reloaded();

			uint32_t elements = 1;
			if ( pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->IsArray() )
			{
				AGKShaderConstantArray *pConstantArray = (AGKShaderConstantArray*) pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue;
				elements = pConstantArray->m_iNumElements;
			}
			switch( pOpenGLES2Shader->m_pLocations[ i ].m_iType )
			{
				case AGK_SHADER_CONSTANT_TYPE_FLOAT: 
				{
					glUniform1fv( pOpenGLES2Shader->m_pLocations[ i ].m_iLocation, elements, pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_VEC2: 
				{
					glUniform2fv( pOpenGLES2Shader->m_pLocations[ i ].m_iLocation, elements, pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_VEC3: 
				{
					glUniform3fv( pOpenGLES2Shader->m_pLocations[ i ].m_iLocation, elements, pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_VEC4: 
				{
					glUniform4fv( pOpenGLES2Shader->m_pLocations[ i ].m_iLocation, elements, pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_MAT2: 
				{
					glUniformMatrix2fv( pOpenGLES2Shader->m_pLocations[ i ].m_iLocation, elements, GL_FALSE, pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_MAT3: 
				{
					glUniformMatrix3fv( pOpenGLES2Shader->m_pLocations[ i ].m_iLocation, elements, GL_FALSE, pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_MAT4: 
				{
					glUniformMatrix4fv( pOpenGLES2Shader->m_pLocations[ i ].m_iLocation, elements, GL_FALSE, pOpenGLES2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
			}
		}
	}
	
	if ( pShader->m_iFlags & AGK_SHADER_RELOAD_UNIFORMS )
	{
		// set texture, cubemap, or shadow map locations
		for( int i = 0; i < AGK_OPENGLES2_MAX_TEXTURE_STAGES; i++ )
		{
			if ( pOpenGLES2Shader->m_iTextureLocations[ i ] != (uint16_t) -1 ) glUniform1i( pOpenGLES2Shader->m_iTextureLocations[ i ], i );
		}

		pShader->m_iFlags &= ~AGK_SHADER_RELOAD_UNIFORMS;
	}

	return APP_SUCCESS;
}

int OpenGLES2Renderer::BindShaderConstantUpdates( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKDynamicArray<cImage*> *pTextures, void **inoutConstants )
{
	agk::Error( "BindShaderConstantUpdates currently not used" );
	return APP_ERROR_UNKNOWN;
}

int OpenGLES2Renderer::BindPipeline( void *pCommandBuffer, AGKShader *pAGKShader, const AGKRenderState *pRenderState, const AGKVertexLayout *pVertexLayout, void **inoutPipeline )
{
	OpenGLES2Shader *pShader = (OpenGLES2Shader*) pAGKShader->m_pInternalResources;
	if ( m_iCurrShader != pShader->m_iShaderID )
	{
		glUseProgram( pShader->m_iShaderID );
		m_iCurrShader = pShader->m_iShaderID;
		m_iVBOPtrSetup = 0;
	}

	m_pCurrAGKShader = pAGKShader;
	m_pCurrVertexLayout = pVertexLayout; // can't set vertex layout until VBO is bound
	
	// render state

	// scissor
	uint16_t iScissorX, iScissorY, iScissorWidth, iScissorHeight;
	if ( pRenderState->DoesOverrideScissor() )
	{
		iScissorX = pRenderState->GetScissorX();
		iScissorY = pRenderState->GetScissorY();
		iScissorWidth = pRenderState->GetScissorWidth();
		iScissorHeight = pRenderState->GetScissorHeight();
	}
	else
	{
		iScissorX = m_iGlobalScissorX;
		iScissorY = m_iGlobalScissorY;
		iScissorWidth = m_iGlobalScissorWidth;
		iScissorHeight = m_iGlobalScissorHeight;
	}

	if ( iScissorX == 0 && iScissorY == 0 && iScissorWidth == 0 && iScissorHeight == 0 )
	{
		if ( m_iScissorEnabled != 0 ) 
		{
			glDisable( GL_SCISSOR_TEST );
			m_iScissorEnabled = 0;
		}
	}
	else
	{
		if ( m_iScissorEnabled != 1 ) 
		{
			glEnable( GL_SCISSOR_TEST );
			m_iScissorEnabled = 1;
		}
		if ( iScissorX != m_iCurrScissorX || iScissorY != m_iCurrScissorY || iScissorWidth != m_iCurrScissorWidth || iScissorHeight != m_iCurrScissorHeight )
		{
			glScissor( iScissorX, iScissorY, iScissorWidth, iScissorHeight );
			m_iCurrScissorX = iScissorX;
			m_iCurrScissorY = iScissorY;
			m_iCurrScissorWidth = iScissorWidth;
			m_iCurrScissorHeight = iScissorHeight;
		}
	}

	// cull mode
	if ( pRenderState->GetCullMode() != m_iCurrentCullMode )
	{
		switch( pRenderState->GetCullMode() )
		{
			case AGK_CULL_NONE: // draw both front and back
			{
				glDisable( GL_CULL_FACE ); 
				break; 
			}
			case AGK_CULL_BACK: // draw front faces
			{
				if ( m_iCurrentCullMode <= AGK_CULL_NONE ) glEnable( GL_CULL_FACE );
				glCullFace( GL_BACK );
				break;
			}
			case AGK_CULL_FRONT: // draw back faces
			{
				if ( m_iCurrentCullMode <= AGK_CULL_NONE ) glEnable( GL_CULL_FACE );
				glCullFace( GL_FRONT );
				break;
			}
		}
		m_iCurrentCullMode = pRenderState->GetCullMode();
	}

	// winding order
	int iWindingOrder = pRenderState->GetWindingOrder();
	if ( m_pCurrFrameBuffer ) iWindingOrder = 1 - iWindingOrder; // FBOs are drawn upside down in OpenGL

	if ( iWindingOrder != m_iCurrentWindingOrder )
	{
		if ( iWindingOrder == AGK_WINDING_CLOCKWISE ) glFrontFace( GL_CW );
		else glFrontFace( GL_CCW );
		m_iCurrentWindingOrder = iWindingOrder;
	}

	// depth test
	if ( pRenderState->GetDepthEnable() != m_iCurrentDepthEnable )
	{
		if ( pRenderState->GetDepthEnable() > 0 ) glEnable( GL_DEPTH_TEST );
		else glDisable( GL_DEPTH_TEST );
		m_iCurrentDepthEnable = pRenderState->GetDepthEnable();
	}

	if ( m_iCurrentDepthEnable == 1 )
	{
		// depth write
		if ( pRenderState->GetDepthWriteEnable() != m_iCurrentDepthWriteEnable )
		{
			if ( pRenderState->GetDepthWriteEnable() > 0 ) glDepthMask( GL_TRUE );
			else glDepthMask( GL_FALSE );
			m_iCurrentDepthWriteEnable = pRenderState->GetDepthWriteEnable();
		}

		// depth function
		int8_t depthFunc = pRenderState->GetDepthFunc();
		if ( IsInvertedDepth() )
		{
			switch( depthFunc )
			{
				case AGK_DEPTH_FUNC_LESS: depthFunc = AGK_DEPTH_FUNC_GREATER; break;
				case AGK_DEPTH_FUNC_LEQUAL: depthFunc = AGK_DEPTH_FUNC_GEQUAL; break;
				case AGK_DEPTH_FUNC_GREATER: depthFunc = AGK_DEPTH_FUNC_LESS; break;
				case AGK_DEPTH_FUNC_GEQUAL: depthFunc = AGK_DEPTH_FUNC_LEQUAL; break;
			}
		}
		if ( depthFunc != m_iCurrentDepthFunc )
		{
			switch ( depthFunc )
			{
				case AGK_DEPTH_FUNC_NEVER:		glDepthFunc( GL_NEVER ); break;
				case AGK_DEPTH_FUNC_LESS:		glDepthFunc( GL_LESS ); break;
				case AGK_DEPTH_FUNC_EQUAL:		glDepthFunc( GL_EQUAL ); break;
				case AGK_DEPTH_FUNC_LEQUAL:		glDepthFunc( GL_LEQUAL ); break;
				case AGK_DEPTH_FUNC_GREATER:	glDepthFunc( GL_GREATER ); break;
				case AGK_DEPTH_FUNC_NOTEQUAL:	glDepthFunc( GL_NOTEQUAL ); break;
				case AGK_DEPTH_FUNC_GEQUAL:		glDepthFunc( GL_GEQUAL ); break;
				case AGK_DEPTH_FUNC_ALWAYS:		glDepthFunc( GL_ALWAYS ); break;
			}
			m_iCurrentDepthFunc = depthFunc;
		}

#ifdef GL_DEPTH_CLAMP
		// depth clamp
		if ( pRenderState->GetDepthClampEnable() != m_iCurrentDepthClampEnable )
		{
			if ( pRenderState->GetDepthClampEnable() > 0 ) glEnable( GL_DEPTH_CLAMP );
			else glDisable( GL_DEPTH_CLAMP );
			m_iCurrentDepthClampEnable = pRenderState->GetDepthClampEnable();
		}
#endif

		// depth bias
		if ( pRenderState->GetDepthBias() != m_fCurrentDepthBias ) 
		{
			if ( pRenderState->GetDepthBias() == 0 ) glDisable( GL_POLYGON_OFFSET_FILL );
			else 
			{
				if ( m_fCurrentDepthBias == 0 ) glEnable( GL_POLYGON_OFFSET_FILL );
				glPolygonOffset( 1, pRenderState->GetDepthBias() );
			}
			m_fCurrentDepthBias = pRenderState->GetDepthBias();
		}

		// depth range
		float depthMin = pRenderState->GetDepthRangeMin();
		float depthMax = pRenderState->GetDepthRangeMax();
		if ( IsInvertedDepth() )
		{
			float temp = depthMin;
			depthMin = 1.0f - depthMax;
			depthMax = 1.0f - temp;
		}
		if ( depthMax != m_fCurrentDepthMax || depthMin != m_fCurrentDepthMin )
		{
			glDepthRangef( depthMin, depthMax );
			m_fCurrentDepthMin = depthMin;
			m_fCurrentDepthMax = depthMax;
		}
	}

	// stencil not currently supported
	if ( m_iCurrentStencilEnable != 0 )
	{
		glDisable( GL_STENCIL_TEST );
		m_iCurrentStencilEnable = 0;
	}

	// blending
	if ( pRenderState->GetBlendEnable() != m_iCurrentBlendEnable )
	{
		if ( pRenderState->GetBlendEnable() > 0 ) glEnable( GL_BLEND );
		else glDisable( GL_BLEND );
		m_iCurrentBlendEnable = pRenderState->GetBlendEnable();
	}

	if ( m_iCurrentBlendEnable == 1 )
	{
		if ( pRenderState->GetBlendColorSrc() != m_iCurrentBlendColorSrc || pRenderState->GetBlendColorDst() != m_iCurrentBlendColorDst )
		{
			int colorSrc = GL_ONE;
			switch ( pRenderState->GetBlendColorSrc() )
			{
				case AGK_BLEND_ZERO:				colorSrc = GL_ZERO; break;
				case AGK_BLEND_ONE:					colorSrc = GL_ONE; break;
				case AGK_BLEND_SRC_ALPHA:			colorSrc = GL_SRC_ALPHA; break;
				case AGK_BLEND_ONE_MINUS_SRC_ALPHA: colorSrc = GL_ONE_MINUS_SRC_ALPHA; break;
				case AGK_BLEND_DST_ALPHA:			colorSrc = GL_DST_ALPHA; break;
				case AGK_BLEND_ONE_MINUS_DST_ALPHA: colorSrc = GL_ONE_MINUS_DST_ALPHA; break;
				//case AGK_BLEND_SRC_COLOR:			colorSrc = GL_SRC_COLOR; break; // illegal
				//case AGK_BLEND_ONE_MINUS_SRC_COLOR: colorSrc = GL_ONE_MINUS_SRC_COLOR; break; // illegal
				case AGK_BLEND_DST_COLOR:			colorSrc = GL_DST_COLOR; break;
				case AGK_BLEND_ONE_MINUS_DST_COLOR: colorSrc = GL_ONE_MINUS_DST_COLOR; break;
				case AGK_BLEND_ALPHA_SATURATE:		colorSrc = GL_SRC_ALPHA_SATURATE; break;
			}

			int colorDst = GL_ZERO;
			switch ( pRenderState->GetBlendColorDst() )
			{
				case AGK_BLEND_ZERO:				colorDst = GL_ZERO; break;
				case AGK_BLEND_ONE:					colorDst = GL_ONE; break;
				case AGK_BLEND_SRC_ALPHA:			colorDst = GL_SRC_ALPHA; break;
				case AGK_BLEND_ONE_MINUS_SRC_ALPHA: colorDst = GL_ONE_MINUS_SRC_ALPHA; break;
				case AGK_BLEND_DST_ALPHA:			colorDst = GL_DST_ALPHA; break;
				case AGK_BLEND_ONE_MINUS_DST_ALPHA: colorDst = GL_ONE_MINUS_DST_ALPHA; break;
				case AGK_BLEND_SRC_COLOR:			colorDst = GL_SRC_COLOR; break;
				case AGK_BLEND_ONE_MINUS_SRC_COLOR: colorDst = GL_ONE_MINUS_SRC_COLOR; break;
				//case AGK_BLEND_DST_COLOR:			colorDst = GL_DST_COLOR; break; // illegal
				//case AGK_BLEND_ONE_MINUS_DST_COLOR: colorDst = GL_ONE_MINUS_DST_COLOR; break; // illegal
				//case AGK_BLEND_ALPHA_SATURATE:	colorDst = GL_SRC_ALPHA_SATURATE; break; // illegal
			}

			glBlendFuncSeparate( colorSrc, colorDst, GL_ONE_MINUS_DST_ALPHA, GL_ONE );
	
			m_iCurrentBlendColorSrc = pRenderState->GetBlendColorSrc();
			m_iCurrentBlendColorDst = pRenderState->GetBlendColorDst();
		}
	}

	/*
	if ( pRenderState->GetMultiSampleCount() != m_iCurrentMultiSampleCount )
	{
		if ( pRenderState->GetMultiSampleCount() <= 1 ) glDisable( GL_MULTISAMPLE );
		else glEnable( GL_MULTISAMPLE );

		m_iCurrentMultiSampleCount = pRenderState->GetMultiSampleCount();
	}
	*/

	return APP_SUCCESS;
}
