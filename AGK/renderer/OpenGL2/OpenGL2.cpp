#include "agk.h"
#include "OpenGL2.h"
#include "zlib.h"

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

#if defined(AGK_WINDOWS) || defined(AGK_LINUX)
	// OpenGL extensions
	PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebuffer = 0;
	PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffers = 0;
	PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2D = 0;
	PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbuffer = 0;
	PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatus = 0;
	PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffers = 0;
	PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbuffer = 0;
	PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffers = 0;
	PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffers = 0;
	PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorage = 0;

	PFNGLPOINTPARAMETERFARBPROC glPointParameterfARB = 0;
	PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = 0;

	// shaders
	PFNGLCREATESHADERPROC glCreateShader = 0;
	PFNGLSHADERSOURCEPROC glShaderSource = 0;
	PFNGLCOMPILESHADERPROC glCompileShader = 0;
	PFNGLGETSHADERIVPROC glGetShaderiv = 0;
	PFNGLDELETESHADERPROC glDeleteShader = 0;
	PFNGLATTACHSHADERPROC glAttachShader = 0;
	PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = 0;
	PFNGLLINKPROGRAMPROC glLinkProgram = 0;
	PFNGLGETPROGRAMIVPROC glGetProgramiv = 0;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = 0;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = 0;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = 0;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = 0;
	PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv = 0;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = 0;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = 0;
	PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = 0;
	PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = 0;
	PFNGLCREATEPROGRAMPROC glCreateProgram = 0;
	PFNGLDELETEPROGRAMPROC glDeleteProgram = 0;
	PFNGLUSEPROGRAMPROC glUseProgram = 0;

	// shader uniforms
	PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = 0;
	PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = 0;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = 0;
	PFNGLUNIFORM1IPROC glUniform1i = 0;
	PFNGLUNIFORM1FPROC glUniform1f = 0;
	PFNGLUNIFORM2FPROC glUniform2f = 0;
	PFNGLUNIFORM3FPROC glUniform3f = 0;
	PFNGLUNIFORM4FPROC glUniform4f = 0;
	PFNGLUNIFORM1FVPROC glUniform1fv = 0;
	PFNGLUNIFORM2FVPROC glUniform2fv = 0;
	PFNGLUNIFORM3FVPROC glUniform3fv = 0;
	PFNGLUNIFORM4FVPROC glUniform4fv = 0;
	PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = 0;

	// texture
#ifndef AGK_LINUX
	PFNGLACTIVETEXTUREPROC glActiveTexture = 0;
#endif
	PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmap = 0;

	// vertex buffer objects
	PFNGLGENBUFFERSPROC glGenBuffers = 0;
	PFNGLBINDBUFFERPROC glBindBuffer = 0;
	PFNGLBUFFERDATAPROC glBufferData = 0;
	PFNGLBUFFERSUBDATAPROC glBufferSubData = 0;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers = 0;

	PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = 0;
#endif

#ifdef AGK_WINDOWS
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	PFNWGLCreateContextARB wglCreateContextAttribsARB = 0;

	bool LoadRendererExtensions()
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		// framebuffer/renderbuffer objects
		glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress( "glBindFramebufferEXT" );
		if( !glBindFramebuffer ) agk::Message( "Failed to get OpenGL extension glBindFramebufferEXT" );

		glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress( "glGenFramebuffersEXT" );
		if( !glGenFramebuffers ) agk::Message( "Failed to get OpenGL extension glGenFramebuffersEXT" );

		glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress( "glFramebufferTexture2DEXT" );
		if( !glFramebufferTexture2D ) agk::Message( "Failed to get OpenGL extension glFramebufferTexture2DEXT" );

		glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress( "glFramebufferRenderbufferEXT" );
		if( !glFramebufferRenderbuffer ) agk::Message( "Failed to get OpenGL extension glFramebufferRenderbufferEXT" );

		glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress( "glCheckFramebufferStatusEXT" );
		if( !glCheckFramebufferStatus ) agk::Message( "Failed to get OpenGL extension glCheckFramebufferStatusEXT" );

		glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress( "glDeleteFramebuffersEXT" );
		if( !glDeleteFramebuffers ) agk::Message( "Failed to get OpenGL extension glDeleteFramebuffersEXT" );

		glBindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress( "glBindRenderbufferEXT" );
		if( !glBindRenderbuffer ) agk::Message( "Failed to get OpenGL extension glBindRenderbufferEXT" );

		glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress( "glDeleteRenderbuffersEXT" );
		if( !glDeleteRenderbuffers ) agk::Message( "Failed to get OpenGL extension glDeleteRenderbuffersEXT" );

		glGenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress( "glGenRenderbuffersEXT" );
		if( !glGenRenderbuffers ) agk::Message( "Failed to get OpenGL extension glGenRenderbuffersEXT" );

		glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress( "glRenderbufferStorageEXT" );
		if( !glRenderbufferStorage ) agk::Message( "Failed to get OpenGL extension glRenderbufferStorageEXT" );

		// mipmaps (OpenGL 3.0+ only), if missing don't error just fall back to GL_GENERATE_MIPMAP
		glGenerateMipmap = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress( "glGenerateMipmap" );
		if ( !glGenerateMipmap ) glGenerateMipmap = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress( "glGenerateMipmapEXT" );
		//if ( !glGenerateMipmap ) agk::Message( "Failed to load OpenGL extension glGenerateMipmap" );

		// shader functions
		glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress( "glCreateShader" );
		if ( !glCreateShader ) { agk::Message( "Failed to load OpenGL extension glCreateShader" ); return false; }

		glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress( "glShaderSourceARB" );
		if ( !glShaderSource ) { agk::Message( "Failed to load OpenGL extension glShaderSource" ); return false; }

		glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress( "glCompileShaderARB" );
		if ( !glCompileShader ) { agk::Message( "Failed to load OpenGL extension glCompileShader" ); return false; }

		glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress( "glGetShaderiv" );
		if ( !glGetShaderiv ) { agk::Message( "Failed to load OpenGL extension glGetShaderiv" ); return false; }

		glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress( "glDeleteShader" );
		if ( !glDeleteShader ) { agk::Message( "Failed to load OpenGL extension glDeleteShader" ); return false; }

		glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress( "glAttachShader" );
		if ( !glAttachShader ) { agk::Message( "Failed to load OpenGL extension glAttachShader" ); return false; }

		glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress( "glBindAttribLocation" );
		if ( !glBindAttribLocation ) { agk::Message( "Failed to load OpenGL extension glBindAttribLocation" ); return false; }

		glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress( "glLinkProgramARB" );
		if ( !glLinkProgram ) { agk::Message( "Failed to load OpenGL extension glLinkProgram" ); return false; }

		glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress( "glGetProgramiv" );
		if ( !glGetProgramiv ) { agk::Message( "Failed to load OpenGL extension glGetProgramiv" ); return false; }

		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress( "glGetUniformLocation" );
		if ( !glGetUniformLocation ) { agk::Message( "Failed to load OpenGL extension glGetUniformLocation" ); return false; }

		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress( "glDisableVertexAttribArray" );
		if ( !glDisableVertexAttribArray ) { agk::Message( "Failed to load OpenGL extension glDisableVertexAttribArray" ); return false; }

		glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress( "glEnableVertexAttribArray" );
		if ( !glEnableVertexAttribArray ) { agk::Message( "Failed to load OpenGL extension glEnableVertexAttribArray" ); return false; }

		glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress( "glVertexAttribPointer" );
		if ( !glVertexAttribPointer ) { agk::Message( "Failed to load OpenGL extension glVertexAttribPointer" ); return false; }

		glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress( "glVertexAttrib4fv" );
		if ( !glVertexAttrib4fv ) { agk::Message( "Failed to load OpenGL extension glVertexAttrib4fv" ); return false; }

		glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress( "glCreateProgram" );
		if ( !glCreateProgram ) { agk::Message( "Failed to load OpenGL extension glCreateProgram" ); return false; }

		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress( "glDeleteProgram" );
		if ( !glDeleteProgram ) { agk::Message( "Failed to load OpenGL extension glDeleteProgram" ); return false; }

		glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress( "glUseProgram" );
		if ( !glUseProgram ) { agk::Message( "Failed to load OpenGL extension glUseProgram" ); return false; }

		glUniformMatrix2fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress( "glUniformMatrix2fv" );
		if ( !glUniformMatrix2fv ) { agk::Message( "Failed to load OpenGL extension glUniformMatrix2fv" ); return false; }

		glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress( "glUniformMatrix3fv" );
		if ( !glUniformMatrix3fv ) { agk::Message( "Failed to load OpenGL extension glUniformMatrix3fv" ); return false; }

		glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress( "glUniformMatrix4fv" );
		if ( !glUniformMatrix4fv ) { agk::Message( "Failed to load OpenGL extension glUniformMatrix4fv" ); return false; }

		glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress( "glUniform1i" );
		if ( !glUniform1i ) { agk::Message( "Failed to load OpenGL extension glUniform1i" ); return false; }

		glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress( "glUniform1f" );
		if ( !glUniform1f ) { agk::Message( "Failed to load OpenGL extension glUniform1f" ); return false; }

		glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress( "glUniform2f" );
		if ( !glUniform2f ) { agk::Message( "Failed to load OpenGL extension glUniform2f" ); return false; }

		glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress( "glUniform3f" );
		if ( !glUniform3f ) { agk::Message( "Failed to load OpenGL extension glUniform3f" ); return false; }

		glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress( "glUniform4f" );
		if ( !glUniform4f ) { agk::Message( "Failed to load OpenGL extension glUniform4f" ); return false; }

		glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress( "glUniform1fv" );
		if ( !glUniform1fv ) { agk::Message( "Failed to load OpenGL extension glUniform1fv" ); return false; }

		glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress( "glUniform2fv" );
		if ( !glUniform2fv ) { agk::Message( "Failed to load OpenGL extension glUniform2fv" ); return false; }

		glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress( "glUniform3fv" );
		if ( !glUniform3fv ) { agk::Message( "Failed to load OpenGL extension glUniform3fv" ); return false; }

		glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress( "glUniform4fv" );
		if ( !glUniform4fv ) { agk::Message( "Failed to load OpenGL extension glUniform4fv" ); return false; }

		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress( "glGetShaderInfoLog" );
		if ( !glGetShaderInfoLog ) { agk::Message( "Failed to load OpenGL extension glGetShaderInfoLog" ); return false; }

		glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress( "glGetProgramInfoLog" );
		if ( !glGetProgramInfoLog ) { agk::Message( "Failed to load OpenGL extension glGetProgramInfoLog" ); return false; }

		glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress( "glGetActiveUniform" );
		if ( !glGetActiveUniform ) { agk::Message( "Failed to load OpenGL extension glGetActiveUniform" ); return false; }

		glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) wglGetProcAddress( "glGetActiveAttrib" );
		if ( !glGetActiveAttrib ) { agk::Message( "Failed to load OpenGL extension glGetActiveAttrib" ); return false; }

		glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) wglGetProcAddress( "glGetAttribLocation" );
		if ( !glGetAttribLocation ) { agk::Message( "Failed to load OpenGL extension glGetAttribLocation" ); return false; }

		glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress( "glGenBuffers" );
		if ( !glGenBuffers ) { agk::Message( "Failed to load OpenGL extension glGenBuffers" ); return false; }

		glBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress( "glBindBuffer" );
		if ( !glBindBuffer ) { agk::Message( "Failed to load OpenGL extension glBindBuffer" ); return false; }

		glBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress( "glBufferData" );
		if ( !glBufferData ) { agk::Message( "Failed to load OpenGL extension glBufferData" ); return false; }

		glBufferSubData = (PFNGLBUFFERSUBDATAPROC) wglGetProcAddress( "glBufferSubData" );
		if ( !glBufferSubData ) { agk::Message( "Failed to load OpenGL extension glBufferSubData" ); return false; }

		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress( "glDeleteBuffers" );
		if ( !glDeleteBuffers ) { agk::Message( "Failed to load OpenGL extension glDeleteBuffers" ); return false; }

		glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress( "glActiveTexture" );
		if ( !glActiveTexture ) { agk::Message( "Failed to load OpenGL extension glActiveTexture" ); return false; }

		glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) wglGetProcAddress( "glBlendFuncSeparate" );
		if ( !glBlendFuncSeparate ) 
		{ 
			glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) wglGetProcAddress( "glBlendFuncSeparateExt" );
			if ( !glBlendFuncSeparate ) 
			{
				agk::Message( "Failed to load OpenGL extension glBlendFuncSeparate" ); 
				return false; 
			}
		}
				
		return true;
	}

	HWND g_hWndOpenGL = 0;
	HDC g_hDCOpenGL = 0;
	HGLRC g_hOpenGLRC = 0;

	int SetupRendererWindow( void* ptr )
	{
		g_hWndOpenGL = (HWND) ptr;
		g_hDCOpenGL = GetDC( g_hWndOpenGL );
		if ( g_hDCOpenGL == 0 )
		{
			agk::Error( uString("Failed to get DC") );
			return 0;
		}

		BOOL bResult = 0;
		int multisample = 1;
		int iFormat = 0;
		PIXELFORMATDESCRIPTOR pfd;
		
		if ( multisample )
		{
			// create a dummy window to get WGL extensions, this will be destroyed before we return
			WNDCLASSEX wcex;
			memset( &wcex, 0, sizeof(WNDCLASSEX) );
			// hardcoded resource IDs for icons
			wcex.cbSize			= sizeof(WNDCLASSEX);
			wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wcex.lpfnWndProc	= DefWindowProc;
			wcex.hInstance		= (HINSTANCE) GetModuleHandle(NULL);
			wcex.hIcon			= LoadIcon((HINSTANCE) GetModuleHandle(NULL), MAKEINTRESOURCE(104)); 
			wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
			wcex.lpszClassName	= "AGKOPENGLWINDOW2";
			RegisterClassEx(&wcex);

			HWND tempHWnd = CreateWindow("AGKOPENGLWINDOW2", "", WS_OVERLAPPEDWINDOW & (~WS_SIZEBOX), 0, 0, 1, 1, NULL, NULL, (HINSTANCE) GetModuleHandle(NULL), NULL);
			if ( tempHWnd == 0 )
			{
				agk::Error( uString("Failed to create temp window") );
				return 0;
			}

			HDC tempHDC = GetDC( tempHWnd );
			if ( tempHDC == 0 )
			{
				agk::Error( uString("Failed to get temp DC") );
				return 0;
			}
			
			// create temporary OpenGL context just to load extensions
			ZeroMemory( &pfd, sizeof( pfd ) );
			pfd.nSize = sizeof( pfd );
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cAlphaBits = 8;
			pfd.cDepthBits = 24;
			pfd.iLayerType = PFD_MAIN_PLANE;
			iFormat = ChoosePixelFormat( tempHDC, &pfd );
			if ( iFormat == 0 )
			{
				agk::Error( uString("Temp choose Format Failed") );
				return 0;
			}

			bResult = SetPixelFormat( tempHDC, iFormat, &pfd );
			if ( !bResult ) 
			{
				char str[256];
				sprintf_s( str, 256, "Temp set Pixel Format Failed: %d", GetLastError() );
				agk::Error( uString( str ) );
				return 0;
			}

			HGLRC tempHGLRC = wglCreateContext( tempHDC );
			bResult = wglMakeCurrent( tempHDC, tempHGLRC );
			if ( !bResult ) 
			{
				agk::Error( uString("Temp wglMakeCurrent Failed") );
				return 0;
			}

			// get what we came for and delete the temp window
			PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress( "wglChoosePixelFormatARB" );
			DestroyWindow( tempHWnd );

			if ( !wglChoosePixelFormatARB ) 
			{
				multisample = 0;
			}
			else
			{
				// start normal window setup
				uint32_t numFormats;
				float fAttributes[] = {0,0};
			 
				int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_COLOR_BITS_ARB, 24,
					WGL_ALPHA_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 0,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_SAMPLE_BUFFERS_EXT, GL_TRUE,
					WGL_SAMPLES_EXT, 4,
					0,0 };
			 
				// first we check to see if we can get a pixel format for 4 samples
				int valid = wglChoosePixelFormatARB( g_hDCOpenGL, iAttributes, fAttributes, 1, &iFormat, &numFormats );
			  
				if ( !valid || numFormats < 1 )
				{
					// try without multisampling
					agk::Warning( uString("Multisample Choose Format Failed") );
					multisample = 0;
				}
			}
		}		
		
		if ( !multisample )
		{
			// old window setup code that doesn't use multisampling
			ZeroMemory( &pfd, sizeof( pfd ) );
			pfd.nSize = sizeof( pfd );
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cAlphaBits = 8;
			pfd.cDepthBits = 24;
			pfd.iLayerType = PFD_MAIN_PLANE;
			iFormat = ChoosePixelFormat( g_hDCOpenGL, &pfd );
			if ( iFormat == 0 )
			{
				agk::Error( uString("Choose Format Failed") );
				return 0;
			}
		}

		bResult = SetPixelFormat( g_hDCOpenGL, iFormat, &pfd );
		if ( !bResult ) 
		{
	#ifdef _AGK_ERROR_CHECK
			char str[256];
			sprintf_s( str, 256, "Set Pixel Format Failed: %d", GetLastError() );
			//MessageBox( NULL, str, "Error", 0 );
			agk::Error( uString( str ) );
	#endif
			return 0;
		}

		g_hOpenGLRC = wglCreateContext( g_hDCOpenGL );
		bResult = wglMakeCurrent( g_hDCOpenGL, g_hOpenGLRC );
		if ( !bResult ) 
		{
	#ifdef _AGK_ERROR_CHECK
			//MessageBox( NULL, "Make Current Failed", "Error", 0 );
			agk::Error( uString("Make Current Failed") );
	#endif
			return 0;
		}

		return 1;
	}
#endif

#ifdef AGK_MACOS

    namespace AGK
    {
        extern GLFWwindow *g_pWindow;
    }

	bool LoadRendererExtensions() { return true; }
	
	int SetupRendererWindow( void*)
	{
		return 1;
	}
#endif

#ifdef AGK_LINUX
	namespace AGK
	{
		extern GLFWwindow *g_pWindow;
	}

	bool LoadRendererExtensions()
	{
		// framebuffer/renderbuffer objects
		glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC)glXGetProcAddress( (GLubyte*)"glBindFramebufferEXT" );
		if( !glBindFramebuffer ) agk::Message( "Failed to get OpenGL extension glBindFramebufferEXT" );

		glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)glXGetProcAddress( (GLubyte*) "glGenFramebuffersEXT" );
		if( !glGenFramebuffers ) agk::Message( "Failed to get OpenGL extension glGenFramebuffersEXT" );

		glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)glXGetProcAddress( (GLubyte*) "glFramebufferTexture2DEXT" );
		if( !glFramebufferTexture2D ) agk::Message( "Failed to get OpenGL extension glFramebufferTexture2DEXT" );

		glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)glXGetProcAddress( (GLubyte*) "glFramebufferRenderbufferEXT" );
		if( !glFramebufferRenderbuffer ) agk::Message( "Failed to get OpenGL extension glFramebufferRenderbufferEXT" );

		glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)glXGetProcAddress( (GLubyte*) "glCheckFramebufferStatusEXT" );
		if( !glCheckFramebufferStatus ) agk::Message( "Failed to get OpenGL extension glCheckFramebufferStatusEXT" );

		glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSEXTPROC)glXGetProcAddress( (GLubyte*) "glDeleteFramebuffersEXT" );
		if( !glDeleteFramebuffers ) agk::Message( "Failed to get OpenGL extension glDeleteFramebuffersEXT" );

		glBindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC)glXGetProcAddress( (GLubyte*) "glBindRenderbufferEXT" );
		if( !glBindRenderbuffer ) agk::Message( "Failed to get OpenGL extension glBindRenderbufferEXT" );

		glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSEXTPROC)glXGetProcAddress( (GLubyte*) "glDeleteRenderbuffersEXT" );
		if( !glDeleteRenderbuffers ) agk::Message( "Failed to get OpenGL extension glDeleteRenderbuffersEXT" );

		glGenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC)glXGetProcAddress( (GLubyte*) "glGenRenderbuffersEXT" );
		if( !glGenRenderbuffers ) agk::Message( "Failed to get OpenGL extension glGenRenderbuffersEXT" );

		glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEEXTPROC)glXGetProcAddress( (GLubyte*) "glRenderbufferStorageEXT" );
		if( !glRenderbufferStorage ) agk::Message( "Failed to get OpenGL extension glRenderbufferStorageEXT" );

		// mipmaps (OpenGL 3.0+ only), if missing don't error just fall back to GL_GENERATE_MIPMAP
		glGenerateMipmap = (PFNGLGENERATEMIPMAPEXTPROC)glXGetProcAddress( (GLubyte*) "glGenerateMipmap" );
		if ( !glGenerateMipmap ) glGenerateMipmap = (PFNGLGENERATEMIPMAPEXTPROC)glXGetProcAddress( (GLubyte*) "glGenerateMipmapEXT" );
		//if ( !glGenerateMipmap ) agk::Message( "Failed to load OpenGL extension glGenerateMipmap" );

		// shader functions
		glCreateShader = (PFNGLCREATESHADERPROC)glXGetProcAddress( (GLubyte*) "glCreateShader" );
		if ( !glCreateShader ) { agk::Message( "Failed to load OpenGL extension glCreateShader" ); return false; }

		glShaderSource = (PFNGLSHADERSOURCEPROC)glXGetProcAddress( (GLubyte*) "glShaderSourceARB" );
		if ( !glShaderSource ) { agk::Message( "Failed to load OpenGL extension glShaderSource" ); return false; }

		glCompileShader = (PFNGLCOMPILESHADERPROC)glXGetProcAddress( (GLubyte*) "glCompileShaderARB" );
		if ( !glCompileShader ) { agk::Message( "Failed to load OpenGL extension glCompileShader" ); return false; }

		glGetShaderiv = (PFNGLGETSHADERIVPROC)glXGetProcAddress( (GLubyte*) "glGetShaderiv" );
		if ( !glGetShaderiv ) { agk::Message( "Failed to load OpenGL extension glGetShaderiv" ); return false; }

		glDeleteShader = (PFNGLDELETESHADERPROC)glXGetProcAddress( (GLubyte*) "glDeleteShader" );
		if ( !glDeleteShader ) { agk::Message( "Failed to load OpenGL extension glDeleteShader" ); return false; }

		glAttachShader = (PFNGLATTACHSHADERPROC)glXGetProcAddress( (GLubyte*) "glAttachShader" );
		if ( !glAttachShader ) { agk::Message( "Failed to load OpenGL extension glAttachShader" ); return false; }

		glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)glXGetProcAddress( (GLubyte*) "glBindAttribLocation" );
		if ( !glBindAttribLocation ) { agk::Message( "Failed to load OpenGL extension glBindAttribLocation" ); return false; }

		glLinkProgram = (PFNGLLINKPROGRAMPROC)glXGetProcAddress( (GLubyte*) "glLinkProgramARB" );
		if ( !glLinkProgram ) { agk::Message( "Failed to load OpenGL extension glLinkProgram" ); return false; }

		glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glXGetProcAddress( (GLubyte*) "glGetProgramiv" );
		if ( !glGetProgramiv ) { agk::Message( "Failed to load OpenGL extension glGetProgramiv" ); return false; }

		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glXGetProcAddress( (GLubyte*) "glGetUniformLocation" );
		if ( !glGetUniformLocation ) { agk::Message( "Failed to load OpenGL extension glGetUniformLocation" ); return false; }

		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress( (GLubyte*) "glDisableVertexAttribArray" );
		if ( !glDisableVertexAttribArray ) { agk::Message( "Failed to load OpenGL extension glDisableVertexAttribArray" ); return false; }

		glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress( (GLubyte*) "glEnableVertexAttribArray" );
		if ( !glEnableVertexAttribArray ) { agk::Message( "Failed to load OpenGL extension glEnableVertexAttribArray" ); return false; }

		glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress( (GLubyte*) "glVertexAttribPointer" );
		if ( !glVertexAttribPointer ) { agk::Message( "Failed to load OpenGL extension glVertexAttribPointer" ); return false; }

		glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)glXGetProcAddress( (GLubyte*) "glVertexAttrib4fv" );
		if ( !glVertexAttrib4fv ) { agk::Message( "Failed to load OpenGL extension glVertexAttrib4fv" ); return false; }

		glCreateProgram = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress( (GLubyte*) "glCreateProgram" );
		if ( !glCreateProgram ) { agk::Message( "Failed to load OpenGL extension glCreateProgram" ); return false; }

		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glXGetProcAddress( (GLubyte*) "glDeleteProgram" );
		if ( !glDeleteProgram ) { agk::Message( "Failed to load OpenGL extension glDeleteProgram" ); return false; }

		glUseProgram = (PFNGLUSEPROGRAMPROC)glXGetProcAddress( (GLubyte*) "glUseProgram" );
		if ( !glUseProgram ) { agk::Message( "Failed to load OpenGL extension glUseProgram" ); return false; }

		glUniformMatrix2fv = (PFNGLUNIFORMMATRIX3FVPROC)glXGetProcAddress( (GLubyte*) "glUniformMatrix2fv" );
		if ( !glUniformMatrix2fv ) { agk::Message( "Failed to load OpenGL extension glUniformMatrix2fv" ); return false; }

		glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)glXGetProcAddress( (GLubyte*) "glUniformMatrix3fv" );
		if ( !glUniformMatrix3fv ) { agk::Message( "Failed to load OpenGL extension glUniformMatrix3fv" ); return false; }

		glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glXGetProcAddress( (GLubyte*) "glUniformMatrix4fv" );
		if ( !glUniformMatrix4fv ) { agk::Message( "Failed to load OpenGL extension glUniformMatrix4fv" ); return false; }

		glUniform1i = (PFNGLUNIFORM1IPROC)glXGetProcAddress( (GLubyte*) "glUniform1i" );
		if ( !glUniform1i ) { agk::Message( "Failed to load OpenGL extension glUniform1i" ); return false; }

		glUniform1f = (PFNGLUNIFORM1FPROC)glXGetProcAddress( (GLubyte*) "glUniform1f" );
		if ( !glUniform1f ) { agk::Message( "Failed to load OpenGL extension glUniform1f" ); return false; }

		glUniform2f = (PFNGLUNIFORM2FPROC)glXGetProcAddress( (GLubyte*) "glUniform2f" );
		if ( !glUniform2f ) { agk::Message( "Failed to load OpenGL extension glUniform2f" ); return false; }

		glUniform3f = (PFNGLUNIFORM3FPROC)glXGetProcAddress( (GLubyte*) "glUniform3f" );
		if ( !glUniform3f ) { agk::Message( "Failed to load OpenGL extension glUniform3f" ); return false; }

		glUniform4f = (PFNGLUNIFORM4FPROC)glXGetProcAddress( (GLubyte*) "glUniform4f" );
		if ( !glUniform4f ) { agk::Message( "Failed to load OpenGL extension glUniform4f" ); return false; }

		glUniform1fv = (PFNGLUNIFORM1FVPROC)glXGetProcAddress( (GLubyte*) "glUniform1fv" );
		if ( !glUniform1fv ) { agk::Message( "Failed to load OpenGL extension glUniform1fv" ); return false; }

		glUniform2fv = (PFNGLUNIFORM2FVPROC)glXGetProcAddress( (GLubyte*) "glUniform2fv" );
		if ( !glUniform2fv ) { agk::Message( "Failed to load OpenGL extension glUniform2fv" ); return false; }

		glUniform3fv = (PFNGLUNIFORM3FVPROC)glXGetProcAddress( (GLubyte*) "glUniform3fv" );
		if ( !glUniform3fv ) { agk::Message( "Failed to load OpenGL extension glUniform3fv" ); return false; }

		glUniform4fv = (PFNGLUNIFORM4FVPROC)glXGetProcAddress( (GLubyte*) "glUniform4fv" );
		if ( !glUniform4fv ) { agk::Message( "Failed to load OpenGL extension glUniform4fv" ); return false; }

		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glXGetProcAddress( (GLubyte*) "glGetShaderInfoLog" );
		if ( !glGetShaderInfoLog ) { agk::Message( "Failed to load OpenGL extension glGetShaderInfoLog" ); return false; }

		glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glXGetProcAddress( (GLubyte*) "glGetProgramInfoLog" );
		if ( !glGetProgramInfoLog ) { agk::Message( "Failed to load OpenGL extension glGetProgramInfoLog" ); return false; }

		glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)glXGetProcAddress( (GLubyte*) "glGetActiveUniform" );
		if ( !glGetActiveUniform ) { agk::Message( "Failed to load OpenGL extension glGetActiveUniform" ); return false; }

		glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) glXGetProcAddress( (GLubyte*) "glGetActiveAttrib" );
		if ( !glGetActiveAttrib ) { agk::Message( "Failed to load OpenGL extension glGetActiveAttrib" ); return false; }

		glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) glXGetProcAddress( (GLubyte*) "glGetAttribLocation" );
		if ( !glGetAttribLocation ) { agk::Message( "Failed to load OpenGL extension glGetAttribLocation" ); return false; }

		glGenBuffers = (PFNGLGENBUFFERSPROC) glXGetProcAddress( (GLubyte*) "glGenBuffers" );
		if ( !glGenBuffers ) { agk::Message( "Failed to load OpenGL extension glGenBuffers" ); return false; }

		glBindBuffer = (PFNGLBINDBUFFERPROC) glXGetProcAddress( (GLubyte*) "glBindBuffer" );
		if ( !glBindBuffer ) { agk::Message( "Failed to load OpenGL extension glBindBuffer" ); return false; }

		glBufferData = (PFNGLBUFFERDATAPROC) glXGetProcAddress( (GLubyte*) "glBufferData" );
		if ( !glBufferData ) { agk::Message( "Failed to load OpenGL extension glBufferData" ); return false; }

		glBufferSubData = (PFNGLBUFFERSUBDATAPROC) glXGetProcAddress( (GLubyte*) "glBufferSubData" );
		if ( !glBufferSubData ) { agk::Message( "Failed to load OpenGL extension glBufferSubData" ); return false; }

		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) glXGetProcAddress( (GLubyte*) "glDeleteBuffers" );
		if ( !glDeleteBuffers ) { agk::Message( "Failed to load OpenGL extension glDeleteBuffers" ); return false; }

		//glActiveTexture = (PFNGLACTIVETEXTUREPROC) glXGetProcAddress( (GLubyte*) "glActiveTexture" );
		//if ( !glActiveTexture ) { agk::Message( "Failed to load OpenGL extension glActiveTexture" ); return false; }

		glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) glXGetProcAddress( (GLubyte*) "glBlendFuncSeparate" );
		if ( !glBlendFuncSeparate ) 
		{ 
			glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) glXGetProcAddress( (GLubyte*) "glBlendFuncSeparateExt" );
			if ( !glBlendFuncSeparate ) 
			{
				agk::Message( "Failed to load OpenGL extension glBlendFuncSeparate" ); 
				return false; 
			}
		}
		
		return true;
	}

	int SetupRendererWindow( void*)
	{
		return 1;
	}
#endif






OpenGL2Renderer::OpenGL2Renderer()
{

}

OpenGL2Renderer::~OpenGL2Renderer()
{

}

void OpenGL2Renderer::ResetInternalState()
{
	for( int i = 0; i < AGK_OPENGL2_MAX_TEXTURE_STAGES; i++ ) m_iCurrTexture[ i ] = 0;
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

int OpenGL2Renderer::Init()
{
	m_iFBOFlipped = 1;
	m_iTopLeftOrigin = 0;
	m_iSupportsDepthTexture = 1; // OpenGL 2 guarantees support for depth textures
	m_iSupportsDepth24 = 1;
	m_iSupportsDepthNonLinear = 0; 
	m_iSupportsIntIndices = 1; // OpenGL 2 guarantees support for int indices
	m_iSupportsNPOT = 1; // OpenGL 2 guarantees support for non-power of two textures
	m_iSupportsMipmapNPOT = 0;
	m_iClipSpaceZeroOne = 0; // OpenGL is -1 to 1
	m_iInvertedDepth = 0;
	m_iSupportsDepthClamp = 0;
	m_iSupportsPSHighP = 1;

	return APP_SUCCESS;
}

int OpenGL2Renderer::SetupWindow( void* param1, void* param2, unsigned int width, unsigned int height )
{
#if defined(AGK_MACOS) || defined(AGK_LINUX)
	glfwGetFramebufferSize( g_pWindow, &m_iSurfaceWidth, &m_iSurfaceHeight );
#else
	m_iSurfaceWidth = width;
	m_iSurfaceHeight = height;
#endif

	if ( !SetupRendererWindow( param2 ) ) return APP_ERROR_GENERAL;
	else return APP_SUCCESS;
}

int OpenGL2Renderer::WindowResized( void* param1, void* param2, unsigned int width, unsigned int height )
{
	// OpenGL should resize the backbuffer automatically
#if defined(AGK_MACOS) || defined(AGK_LINUX)
	glfwGetFramebufferSize( g_pWindow, &m_iSurfaceWidth, &m_iSurfaceHeight );
#else
	m_iSurfaceWidth = width;
	m_iSurfaceHeight = height;
#endif
	return APP_SUCCESS;
}

int OpenGL2Renderer::SetScreenRecordParams( void* param1, void* param2 )
{
	return APP_ERROR_GENERAL;
}

int OpenGL2Renderer::Setup()
{
	if ( !LoadRendererExtensions() )
	{
		agk::Message( "Your graphics card does not support one or more OpenGL features required by this program. Please check if there are any driver updates available for your card." );
		exit(0);
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_iMaxTextureSize);

#ifndef GL_MAX_VARYING_COMPONENTS
    #define GL_MAX_VARYING_COMPONENTS GL_MAX_VARYING_FLOATS
#endif

	glGetIntegerv( GL_MAX_VERTEX_UNIFORM_COMPONENTS, &m_iMaxVSUniforms ); m_iMaxVSUniforms /= 4;
	glGetIntegerv( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &m_iMaxPSUniforms ); m_iMaxPSUniforms /= 4;
	glGetIntegerv( GL_MAX_VARYING_COMPONENTS, &m_iMaxVaryings );

	// not using memory alignment
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	glClearColor( 0, 0, 0, 0 );
	glClearDepth( m_iInvertedDepth ? 0.0f : 1.0f );

	agk::ClearScreen();
    agk::Swap();
	agk::ClearScreen();

	return APP_SUCCESS;
}

int OpenGL2Renderer::Swap()
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
	
#if defined(AGK_WINDOWS)
	SwapBuffers( g_hDCOpenGL );
	//wglSwapLayerBuffers( g_hDC, WGL_SWAP_MAIN_PLANE );
#elif defined(AGK_MACOS)
	glfwSwapBuffers(g_pWindow);
#elif defined(AGK_LINUX)
	glfwSwapBuffers(g_pWindow);
#endif

	return APP_SUCCESS;
}

void OpenGL2Renderer::CleanUp()
{
#if defined(AGK_WINDOWS)
	if ( g_hDCOpenGL && g_hOpenGLRC )
	{
		wglMakeCurrent(g_hDCOpenGL,NULL);
		wglDeleteContext(g_hOpenGLRC);
	}
	g_hOpenGLRC = 0;

	if ( g_hWndOpenGL && g_hDCOpenGL ) ReleaseDC( g_hWndOpenGL, g_hDCOpenGL );
	g_hDCOpenGL = 0;
#elif defined(AGK_MACOS)
	
#elif defined(AGK_LINUX)
	
#endif
}

void OpenGL2Renderer::SetVSync( int mode )
{
#if defined(AGK_WINDOWS)
	if( wglSwapIntervalEXT ) wglSwapIntervalEXT(mode);
#elif defined(AGK_MACOS)
	glfwSwapInterval( mode );
#elif defined(AGK_LINUX)
	glfwSwapInterval( mode );
#endif
}

void OpenGL2Renderer::SetViewport( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) 
{ 
	glViewport( x, y, width, height );
}

void OpenGL2Renderer::ClearRenderTarget( float red, float green, float blue, float alpha )
{
	// depth write must be enabled to clear depth
	if ( m_iCurrentDepthWriteEnable != 1 ) glDepthMask( GL_TRUE );
	m_iCurrentDepthWriteEnable = 1;

	if ( m_iScissorEnabled != 0 ) glDisable( GL_SCISSOR_TEST );
	m_iScissorEnabled = 0;

	glClearColor( red, green, blue, alpha );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

void OpenGL2Renderer::ClearDepthBuffer()
{
	// depth write must be enabled to clear depth
	if ( m_iCurrentDepthWriteEnable != 1 ) glDepthMask( GL_TRUE );
	m_iCurrentDepthWriteEnable = 1;

	if ( m_iScissorEnabled != 0 ) glDisable( GL_SCISSOR_TEST );
	m_iScissorEnabled = 0;

	glClear( GL_DEPTH_BUFFER_BIT );
}

void OpenGL2Renderer::SetAntialiasMode( int mode )
{
	if ( mode == 1 ) glEnable( GL_MULTISAMPLE );
	else glDisable( GL_MULTISAMPLE );
}

void OpenGL2Renderer::GetDataFromScreen( uint32_t **pData, int x, int y, int width, int height )
{
	*pData = new uint32_t[ width*height ];
	glReadBuffer( GL_BACK );
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

bool OpenGL2Renderer::IsImageFormatSupported( AGKImgFormat format )
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

int OpenGL2Renderer::BindTexture( void *pResource, uint32_t stage )
{
	if ( stage >= AGK_OPENGL2_MAX_TEXTURE_STAGES ) return APP_ERROR_UNKNOWN;

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
		OpenGL2Image *pImage = (OpenGL2Image*) pResource;

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

int OpenGL2Renderer::UnBindTexture( void *pResource )
{
	if ( !pResource ) return APP_SUCCESS;
	OpenGL2Image *pImage = (OpenGL2Image*) pResource;

	for ( int stage = 0; stage < AGK_OPENGL2_MAX_TEXTURE_STAGES; stage++ )
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

int OpenGL2Renderer::CreateImage2D( AGKImgFormat format, uint32_t width, uint32_t height, uint8_t *data, uint32_t flags, void **outResource )
{
	uint32_t glformat = GL_RGBA8;
	uint32_t glformat2 = GL_RGBA;
	uint32_t glbytesize = GL_UNSIGNED_BYTE;

	if ( format == AGK_COLOR_FORMAT_R_8 )
	{
		glformat = GL_ALPHA;
		glformat2 = GL_ALPHA;
	}

	OpenGL2Image *pResource = (OpenGL2Image*) *outResource;
	if ( !pResource )
	{
		pResource = new OpenGL2Image();
		glGenTextures( 1, &pResource->m_iTextureID );
		*outResource = pResource;
	}
	pResource->m_iWidth = width;
	pResource->m_iHeight = height;
	pResource->m_iFormat = glformat2;
	pResource->m_iByteSize = glbytesize;
	pResource->m_iFlags = 0;

	if ( flags & AGK_IMAGE_GEN_MIPMAP ) pResource->m_iFlags |= AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP;
	else if ( flags & AGK_IMAGE_USE_MIPMAP ) pResource->m_iFlags |= AGK_IMAGE_USE_MIPMAP;

	BindTexture( pResource, 0 );
	
	if ( !glGenerateMipmap )
	{
		if ( (flags & AGK_IMAGE_GEN_MIPMAP) == AGK_IMAGE_GEN_MIPMAP ) glTexParameterf( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
		else glTexParameterf( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, glformat, width, height, 0, glformat2, glbytesize, data );

	if ( glGenerateMipmap )
	{
		if ( (flags & AGK_IMAGE_GEN_MIPMAP) == AGK_IMAGE_GEN_MIPMAP ) glGenerateMipmap(GL_TEXTURE_2D);
	}
	
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

int OpenGL2Renderer::UpdateSubImage2D( void *pResource, uint32_t mipmapLevel, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t *data )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;

	OpenGL2Image *pImage = (OpenGL2Image*) pResource;

	BindTexture( pResource, 0 );

	if ( !(pImage->m_iFlags & AGK_IMAGE_USE_MIPMAP) && mipmapLevel > 0 ) ReportAndReturnErrorID( APP_ERROR_VK_INVALID_IMAGE_MIP_LEVEL, mipmapLevel, 1 );

	if( !glGenerateMipmap )
	{
		if ( pImage->m_iFlags & AGK_IMAGE_GEN_MIPMAP ) glTexParameterf( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
		else glTexParameterf( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
	}

	glTexSubImage2D(GL_TEXTURE_2D, mipmapLevel, x, y, width, height, pImage->m_iFormat, pImage->m_iByteSize, data );

	if ( glGenerateMipmap )
	{
		if ( pImage->m_iFlags & AGK_IMAGE_GEN_MIPMAP ) glGenerateMipmap(GL_TEXTURE_2D);
	}

	return APP_SUCCESS;
}

int OpenGL2Renderer::CreateDepthBuffer( AGKImgFormat format, uint32_t width, uint32_t height, uint32_t flags, void **outResource )
{
	OpenGL2Image *pResource = (OpenGL2Image*) *outResource;
	if ( !pResource )
	{
		pResource = new OpenGL2Image();
		glGenTextures( 1, &pResource->m_iTextureID );
		*outResource = pResource;
	}
	pResource->m_iFlags = flags | AGK_GL2_IMAGE_IS_DEPTH;
	pResource->m_iFlags &= ~(AGK_IMAGE_WRAP_U_REPEAT | AGK_IMAGE_WRAP_V_REPEAT | AGK_IMAGE_USE_MIPMAP | AGK_IMAGE_GEN_MIPMAP);

	BindTexture( pResource, 0 );
		
	uint32_t glformat = GL_DEPTH_COMPONENT24;
	uint32_t glformat2 = GL_DEPTH_COMPONENT;
	uint32_t glsize = GL_UNSIGNED_INT;
	
	glTexImage2D( GL_TEXTURE_2D, 0, glformat, width, height, 0, glformat2, glsize, 0 );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if ( pResource->m_iFlags & AGK_IMAGE_MAG_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if ( pResource->m_iFlags & AGK_IMAGE_MIN_FILTER_NEAREST ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return APP_SUCCESS;
}

int OpenGL2Renderer::DeleteImage( void *inResource, int unbind )
{
	if ( !inResource ) return APP_SUCCESS;

	if ( unbind ) UnBindTexture( inResource );

	OpenGL2Image *pImage = (OpenGL2Image*) inResource;
	if ( pImage->m_iTextureID != 0 ) 
	{
		glDeleteTextures( 1, &pImage->m_iTextureID );
	}
	delete pImage;

	return APP_SUCCESS;
}

void OpenGL2Renderer::ClearImageBindings()
{
	for ( int stage = 0; stage < AGK_OPENGL2_MAX_TEXTURE_STAGES; stage++ )
	{
		glActiveTexture( GL_TEXTURE0 + stage );
		glBindTexture( GL_TEXTURE_2D, 0 );
		m_iCurrTexture[stage] = 0;
	}

	glActiveTexture( GL_TEXTURE0 );
	m_iCurrActiveTexture = -1;
}

int OpenGL2Renderer::OverrideTexture( uint32_t width, uint32_t height, AGKImgFormat format, void* texture, void **outResource )
{
	uint32_t glformat = GL_RGBA;
	uint32_t glbytesize = GL_UNSIGNED_BYTE;

	if ( format == AGK_COLOR_FORMAT_R_8 ) glformat = GL_ALPHA;
    else if ( format == AGK_COLOR_FORMAT_RG_8 ) glformat = GL_LUMINANCE_ALPHA;

	OpenGL2Image *pResource = (OpenGL2Image*) *outResource;
	if ( !pResource )
	{
		pResource = new OpenGL2Image();
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

int OpenGL2Renderer::SetImageSampleSettings( void *pResource, uint32_t flags )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	OpenGL2Image *pImage = (OpenGL2Image*) pResource;

	BindTexture( pResource, 0 );

	if ( flags & AGK_IMAGE_GEN_MIPMAP ) flags |= AGK_IMAGE_USE_MIPMAP;

	pImage->m_iFlags &= ~(AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP);
	if ( flags & AGK_IMAGE_USE_MIPMAP ) pImage->m_iFlags |= AGK_IMAGE_USE_MIPMAP;
	if ( flags & AGK_IMAGE_GEN_MIPMAP )
	{
		pImage->m_iFlags |= AGK_IMAGE_GEN_MIPMAP;
		if ( !glGenerateMipmap ) glTexParameterf( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
		else glGenerateMipmap( GL_TEXTURE_2D );
	}
	else 
	{
		if ( !glGenerateMipmap ) glTexParameterf( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
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

void OpenGL2Renderer::SetImageMagFilter( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	OpenGL2Image *pImage = (OpenGL2Image*) pResource;
	BindTexture( pResource, 0 );

	if ( mode == 0 ) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenGL2Renderer::SetImageMinFilter( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	OpenGL2Image *pImage = (OpenGL2Image*) pResource;
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

void OpenGL2Renderer::SetImageWrapU( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	OpenGL2Image *pImage = (OpenGL2Image*) pResource;
	BindTexture( pResource, 0 );

	if ( mode != 0 )
	{
		OpenGL2Image *pImage = (OpenGL2Image*) pResource;

		int width = pImage->m_iWidth;
		if ( (width & (width-1)) != 0 ) // check power of 2
		{
			agk::Error( "Cannot set image wrap mode to repeat on an image that is not a power of two in size when using OpenGL2" );
			return;
		}
	}

	if ( mode == 0 ) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
}

void OpenGL2Renderer::SetImageWrapV( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	OpenGL2Image *pImage = (OpenGL2Image*) pResource;
	BindTexture( pResource, 0 );

	if ( mode != 0 )
	{
		OpenGL2Image *pImage = (OpenGL2Image*) pResource;

		int height = pImage->m_iHeight;
		if ( (height & (height-1)) != 0 ) // check power of 2
		{
			agk::Error( "Cannot set image wrap mode to repeat on an image that is not a power of two in size when using OpenGL2" );
			return;
		}
	}
	
	if ( mode == 0 ) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void OpenGL2Renderer::GenerateImageMipmaps( OpenGL2Image *pImage )
{
	if ( !glGenerateMipmap ) return;
	if ( !(pImage->m_iFlags & AGK_IMAGE_GEN_MIPMAP) ) return;

	BindTexture( pImage, 0 );
	glGenerateMipmap(GL_TEXTURE_2D);
}

int OpenGL2Renderer::GetImageData( void **pResource, uint8_t **pData )
{
	if ( !pData ) return 0;
	if ( !pResource ) return 0;
	
	OpenGL2Image *pImage = (OpenGL2Image*) *pResource;
	if ( !pImage ) return 0;
	
	int iTexID = pImage->m_iTextureID;
	if ( iTexID == 0 ) return 0;
	
	BindTexture( pImage, 0 );
	if ( pImage->m_iFormat == GL_ALPHA )
	{
		*pData = new unsigned char[ pImage->m_iWidth * pImage->m_iHeight ];
		glGetTexImage( GL_TEXTURE_2D, 0, GL_ALPHA, GL_UNSIGNED_BYTE, *pData );
		return pImage->m_iWidth * pImage->m_iHeight;
	}
	else
	{
		*pData = new unsigned char[ pImage->m_iWidth * pImage->m_iHeight * 4 ];
		glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, *pData );
		return pImage->m_iWidth * pImage->m_iHeight * 4;
	}
}

uint32_t OpenGL2Renderer::GetImageInternalID( void *pResource )
{
	if ( !pResource ) return 0;
	
	OpenGL2Image *pImage = (OpenGL2Image*)pResource;
	return pImage->m_iTextureID;
}

// vertex commands
int OpenGL2Renderer::CreateVertices( uint32_t vertexSize, void **outResource )
{
	OpenGL2Vertices *outVertices = (OpenGL2Vertices*) *outResource;
	if ( outVertices )
	{
		if ( vertexSize == outVertices->m_iVertexSize ) return APP_SUCCESS;
		
		DeleteVertices( outVertices );
	}

	outVertices = new OpenGL2Vertices();
	*outResource = outVertices;

	outVertices->m_iVertexSize = vertexSize;
	outVertices->m_iNumIndices = 0;
	outVertices->m_iNumVertices = 0;
		
	return APP_SUCCESS;
}

int OpenGL2Renderer::UpdateVertices( void *pResource, float *pVertexData, uint32_t offset, uint32_t numVertices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pVertexData ) return APP_ERROR_UNKNOWN;

	OpenGL2Vertices *pVertices = (OpenGL2Vertices*) pResource;
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
int OpenGL2Renderer::UpdateIndices( void *pResource, void *pIndexData, uint32_t offset, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	OpenGL2Vertices *pVertices = (OpenGL2Vertices*) pResource;
	if ( offset + numIndices > pVertices->m_iNumIndices ) ReportAndReturnErrorID( APP_ERROR_VK_INDEX_UPDATE_OUT_OF_BOUNDS, offset, numIndices, pVertices->m_iNumIndices );

	uint32_t indexSize = sizeof(unsigned short);
	if ( pVertices->m_iFlags & AGK_GL2_VERTICES_INDICES_INT ) indexSize = sizeof(unsigned int);

	if ( m_iCurrentBoundIndexVBO != pVertices->m_iIndexVBO )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pVertices->m_iIndexVBO );
		m_iCurrentBoundIndexVBO = pVertices->m_iIndexVBO;
	}
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, offset*indexSize, numIndices*indexSize, pIndexData );

	return APP_SUCCESS;
}

int OpenGL2Renderer::SetVertices( void *pResource, float *pVertexData, uint32_t numVertices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pVertexData ) return APP_ERROR_UNKNOWN;

	OpenGL2Vertices *pVertices = (OpenGL2Vertices*) pResource;

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

int OpenGL2Renderer::SetIndices( void *pResource, uint16_t *pIndexData, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	OpenGL2Vertices *pVertices = (OpenGL2Vertices*) pResource;
	if ( pVertices->m_iIndexVBO == 0 ) glGenBuffers( 1, &pVertices->m_iIndexVBO );
	pVertices->m_iNumIndices = numIndices;

	pVertices->m_iFlags &= ~AGK_GL2_VERTICES_INDICES_INT;
	
	if ( m_iCurrentBoundIndexVBO != pVertices->m_iIndexVBO )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pVertices->m_iIndexVBO );
		m_iCurrentBoundIndexVBO = pVertices->m_iIndexVBO;
	}
	//glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint16_t), NULL, GL_STATIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint16_t), pIndexData, GL_STATIC_DRAW );

	return APP_SUCCESS;
}

int OpenGL2Renderer::SetIndicesInt( void *pResource, uint32_t *pIndexData, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	OpenGL2Vertices *pVertices = (OpenGL2Vertices*) pResource;
	if ( pVertices->m_iIndexVBO == 0 ) glGenBuffers( 1, &pVertices->m_iIndexVBO );
	pVertices->m_iNumIndices = numIndices;

	pVertices->m_iFlags |= AGK_GL2_VERTICES_INDICES_INT;
		
	if ( m_iCurrentBoundIndexVBO != pVertices->m_iIndexVBO )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pVertices->m_iIndexVBO );
		m_iCurrentBoundIndexVBO = pVertices->m_iIndexVBO;
	}
	//glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint32_t), NULL, GL_STATIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint32_t), pIndexData, GL_STATIC_DRAW );

	return APP_SUCCESS;
}

int OpenGL2Renderer::DeleteVertices( void *pResource )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	OpenGL2Vertices *pVertices = (OpenGL2Vertices*) pResource;

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
int OpenGL2Renderer::DrawVertices( void *pCommandBuffer, void *pResVertices, int count )
{
	if ( !pResVertices ) return APP_ERROR_UNKNOWN;
	OpenGL2Vertices *pVertices = (OpenGL2Vertices*) pResVertices;

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
		glDrawElements( glprimitive, count, (pVertices->m_iFlags & AGK_GL2_VERTICES_INDICES_INT) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0 );

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
int OpenGL2Renderer::CreateFrameBuffer( void *pColorImage, void *pDepthImage, int iForceDepth, void **outFBO )
{
	if ( !outFBO ) return APP_ERROR_UNKNOWN;
	if ( *outFBO ) DeleteFrameBuffer( *outFBO );

	OpenGL2FrameBuffer *pFBO = new OpenGL2FrameBuffer;
	*outFBO = pFBO;

	pFBO->m_pColor = (OpenGL2Image*) pColorImage;
	pFBO->m_pDepth = (OpenGL2Image*) pDepthImage;
	pFBO->m_iFlags = iForceDepth ? AGK_GL2_FRAMEBUFFER_FORCE_DEPTH : 0 ;
	
	uint32_t iCurr = 0;
	if ( m_pCurrFrameBuffer ) iCurr = m_pCurrFrameBuffer->m_iFBO;

	glGenFramebuffers( 1, &pFBO->m_iFBO );
	glBindFramebuffer( GL_FRAMEBUFFER, pFBO->m_iFBO );
		
	if ( pFBO->m_pColor )
	{
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pFBO->m_pColor->m_iTextureID, 0 );
	}
	else
    {
        glDrawBuffer( GL_NONE );
        glReadBuffer( GL_NONE );
    }

	if ( pFBO->m_pDepth )
	{
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pFBO->m_pDepth->m_iTextureID, 0 );
	}
	else
	{
		if ( pFBO->m_iFlags & AGK_GL2_FRAMEBUFFER_FORCE_DEPTH )
		{
			glGenRenderbuffers( 1, &pFBO->m_iRBO );
			glBindRenderbuffer( GL_RENDERBUFFER, pFBO->m_iRBO );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, pFBO->m_pColor->m_iWidth, pFBO->m_pColor->m_iHeight );
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
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: agk::Error( "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: agk::Error( "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" ); break;
			default: 
			{
				uString err = "Framebuffer error: ";
				err += result;
				agk::Error( err );
			}
		}
		ReportAndReturnErrorID( APP_ERROR_GL2_CREATE_FRAMEBUFFER_FAILED, (int)result );
	}

	glBindFramebuffer( GL_FRAMEBUFFER, iCurr );

	return APP_SUCCESS;
}

int OpenGL2Renderer::DeleteFrameBuffer( void *pResource )
{
	if ( m_pCurrFrameBuffer == pResource ) 
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		m_pCurrFrameBuffer = 0;
	}

	OpenGL2FrameBuffer *pFBO = (OpenGL2FrameBuffer*) pResource;

	if ( pFBO->m_iFBO ) glDeleteFramebuffers( 1, &pFBO->m_iFBO );
	pFBO->m_iFBO = 0;

	if ( pFBO->m_iRBO ) glDeleteRenderbuffers( 1, &pFBO->m_iRBO );
	pFBO->m_iRBO = 0;

	delete pFBO;
	return APP_SUCCESS;
}

// pCommandBuffer is not used in OpenGL
int OpenGL2Renderer::BindFrameBuffer( void *pCommandBuffer, void *pResource )
{
	if ( m_pCurrFrameBuffer == pResource ) return APP_SUCCESS;
	OpenGL2FrameBuffer *pFBO = (OpenGL2FrameBuffer*) pResource;
	
	if ( m_pCurrFrameBuffer && m_pCurrFrameBuffer->m_pColor )
	{
		GenerateImageMipmaps( m_pCurrFrameBuffer->m_pColor );
	}

	if ( pFBO == 0 ) glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	else glBindFramebuffer( GL_FRAMEBUFFER, pFBO->m_iFBO );

	m_pCurrFrameBuffer = pFBO;
	return APP_SUCCESS;
}

// Shaders

const char* OpenGL2Renderer::GetShaderDefaultFullScreenVS()
{
	return "\
	attribute vec3 position;\n\
	varying vec2 uvVarying;\n\
	uniform float agk_invert;\n\
	\
	void main() {\n\
		gl_Position = vec4(position.xy*vec2(1,agk_invert),0.5,1.0);\n\
		uvVarying = (position.xy*vec2(0.5,-0.5) + 0.5);\n\
	}";
}

const char* OpenGL2Renderer::GetShaderDefaultSpriteVS()
{
	return "\
	attribute vec3 position;\
	attribute vec4 color;\
	attribute vec2 uv;\
	varying vec2 uvVarying;\
	varying vec4 colorVarying;\
	uniform mat4 agk_Ortho;\
	void main() { \
		gl_Position = agk_Ortho * vec4(position,1.0);\
		uvVarying = uv;\
		colorVarying = color;\
	}";
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Color Shader" );
	pShader->m_sPSFilename.SetStr( "Default Color Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	varying vec4 colorVarying;\n\
	uniform mat4 agk_Ortho;\n\
	void main() {\n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	varying vec4 colorVarying;\n\
	void main() { gl_FragColor = colorVarying; }" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderTexColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default TexColor Shader" );
	pShader->m_sPSFilename.SetStr( "Default TexColor Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	uniform mat4 agk_Ortho;\n\
	\
	void main() {\n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = texture2D(texture0, uvVarying) * colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderFont()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Font Shader" );
	pShader->m_sPSFilename.SetStr( "Default Font Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	uniform mat4 agk_Ortho;\n\
	\
	void main() { \n\
		gl_Position = agk_Ortho * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\
	varying vec2 uvVarying;\
	varying vec4 colorVarying;\
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

AGKShader* OpenGL2Renderer::LoadDefaultShaderShadow()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	\
	uniform mat4 agk_ShadowWorldProj;\n\
	\
	void main() { \n\
		gl_Position = agk_ShadowWorldProj * vec4(position,1.0);\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	void main() {}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderShadowAlpha()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Alpha Mask Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Alpha Mask Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	\
	uniform vec4 uvBounds0;\n\
	uniform vec4 textureBounds0;\n\
	\
	uniform mat4 agk_ShadowWorldProj;\n\
	\
	void main() { \n\
		gl_Position = agk_ShadowWorldProj * vec4(position,1.0);\n\
		uvVarying = uv * uvBounds0.xy + uvBounds0.zw;\n\
		uvVarying = uvVarying * textureBounds0.xy + textureBounds0.zw;\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	void main() {\n\
	    float alpha = texture2D(texture0, uvVarying).a;\n\
	    if ( alpha < 0.5 ) discard;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderShadowBone( uint32_t numBones )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Bone Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Bone Shader" );

	char szBoneCount[ 12 ];
	sprintf( szBoneCount, "%d", numBones );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 boneweights;\n\
	attribute vec4 boneindices;\n\
	\
	uniform mat4 agk_ShadowProj;\n\
	uniform vec4 agk_bonequats1[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	uniform vec4 agk_bonequats2[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	\
	vec3 transformDQ( vec3 p, vec4 q1, vec4 q2 ) {\n\
	    p += 2.0 * cross( q1.xyz, cross(q1.xyz, p) + q1.w*p );\n\
	    p += 2.0 * (q1.w*q2.xyz - q2.w*q1.xyz + cross(q1.xyz,q2.xyz));\n\
	    return p;\n\
	}\n\
	\
	void main() { \n\
		vec4 q1 = agk_bonequats1[ int(boneindices.x) ] * boneweights.x;\n\
		q1 += agk_bonequats1[ int(boneindices.y) ] * boneweights.y;\n\
		q1 += agk_bonequats1[ int(boneindices.z) ] * boneweights.z;\n\
		q1 += agk_bonequats1[ int(boneindices.w) ] * boneweights.w;\n\
		\
		vec4 q2 = agk_bonequats2[ int(boneindices.x) ] * boneweights.x;\n\
		q2 += agk_bonequats2[ int(boneindices.y) ] * boneweights.y;\n\
		q2 += agk_bonequats2[ int(boneindices.z) ] * boneweights.z;\n\
		q2 += agk_bonequats2[ int(boneindices.w) ] * boneweights.w;\n\
		\
		float len = 1.0/length(q1);\n\
		q1 *= len;\n\
		q2 = (q2 - q1*dot(q1,q2)) * len;\n\
		\
		gl_Position = agk_ShadowProj * vec4( transformDQ(position,q1,q2), 1.0 );\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	void main() {}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderShadowBoneAlpha( uint32_t numBones )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Shadow Bone Alpha Mask Shader" );
	pShader->m_sPSFilename.SetStr( "Shadow Bone Alpha Mask Shader" );

	char szBoneCount[ 12 ];
	sprintf( szBoneCount, "%d", numBones );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 boneweights;\n\
	attribute vec4 boneindices;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	\
	uniform vec4 uvBounds0;\n\
	uniform vec4 textureBounds0;\n\
	\
	uniform mat4 agk_ShadowProj;\n\
	uniform vec4 agk_bonequats1[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	uniform vec4 agk_bonequats2[" ); pShader->m_sVSSource.AppendAscii(szBoneCount); pShader->m_sVSSource.AppendAscii("];\n\
	\
	vec3 transformDQ( vec3 p, vec4 q1, vec4 q2 ) {\n\
	    p += 2.0 * cross( q1.xyz, cross(q1.xyz, p) + q1.w*p );\n\
	    p += 2.0 * (q1.w*q2.xyz - q2.w*q1.xyz + cross(q1.xyz,q2.xyz));\n\
	    return p;\n\
	}\n\
	\
	void main() { \n\
		vec4 q1 = agk_bonequats1[ int(boneindices.x) ] * boneweights.x;\n\
		q1 += agk_bonequats1[ int(boneindices.y) ] * boneweights.y;\n\
		q1 += agk_bonequats1[ int(boneindices.z) ] * boneweights.z;\n\
		q1 += agk_bonequats1[ int(boneindices.w) ] * boneweights.w;\n\
		\
		vec4 q2 = agk_bonequats2[ int(boneindices.x) ] * boneweights.x;\n\
		q2 += agk_bonequats2[ int(boneindices.y) ] * boneweights.y;\n\
		q2 += agk_bonequats2[ int(boneindices.z) ] * boneweights.z;\n\
		q2 += agk_bonequats2[ int(boneindices.w) ] * boneweights.w;\n\
		\
		float len = 1.0/length(q1);\n\
		q1 *= len;\n\
		q2 = (q2 - q1*dot(q1,q2)) * len;\n\
		\
		gl_Position = agk_ShadowProj * vec4( transformDQ(position,q1,q2), 1.0 );\n\
	}" );
	
	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	void main() {\n\
	    float alpha = texture2D(texture0, uvVarying).a;\n\
	    if ( alpha < 0.5 ) discard;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT | AGK_SHADER_IS_CUSTOM;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShader3DParticlesTex()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default 3D Particles Tex Shader" );
	pShader->m_sPSFilename.SetStr( "Default 3D Particles Tex Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	attribute vec2 uv;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	uniform mat4 agk_ViewProj;\n\
	\
	void main() {\n\
		gl_Position = agk_ViewProj * vec4(position,1.0);\n\
		uvVarying = uv;\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	varying vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = texture2D(texture0, uvVarying) * colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShader3DParticlesColor()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default 3D Particles Color Shader" );
	pShader->m_sPSFilename.SetStr( "Default 3D Particles Color Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	attribute vec4 color;\n\
	\
	varying vec4 colorVarying;\n\
	\
	uniform mat4 agk_ViewProj;\n\
	\
	void main() {\n\
		gl_Position = agk_ViewProj * vec4(position,1.0);\n\
		colorVarying = color;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	varying vec4 colorVarying;\n\
	\
	void main() { \n\
		gl_FragColor = colorVarying;\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderVideo()
{
	// Video shader is not support, or required, in OpenGL 2
	return 0;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderDeviceCamera()
{
	// Device camera shader is not support, or required, in OpenGL 2
	return 0;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderARAndroid()
{
	// AR Android shader is not support, or required, in OpenGL 2
	return 0;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderARiOS()
{
	// AR iOS shader is not support, or required, in OpenGL 2
	return 0;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderQuad()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default Quad Shader" );
	pShader->m_sPSFilename.SetStr( "Default Quad Shader" );

	pShader->m_sVSSource.SetStr( GetShaderDefaultFullScreenVS() );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	varying vec2 uvVarying;\n\
	\
	void main() {\n\
		gl_FragColor = texture2D(texture0, uvVarying);\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::LoadDefaultShaderSkyBoxSun()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default SkyBox Sun Shader" );
	pShader->m_sPSFilename.SetStr( "Default SkyBox Sun Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec2 uvVarying2;\n\
	varying vec2 horizonVarying;\n\
	\
	uniform mat4 agk_World;\n\
	uniform mat4 agk_ViewProj;\n\
	\
	uniform vec2 sunSize;\n\
	uniform float horizonHeight;\n\
	uniform float objectScale; // 1.0 / objectScaleY \n\
	\
	void main() {\n\
	    vec4 pos = agk_World * vec4(position,1.0);\n\
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
	#version 110\n\
	uniform sampler2D texture0;\n\
	\
	uniform vec3 skyColor;\n\
	uniform vec3 horizonColor;\n\
	uniform vec3 sunColor;\n\
	uniform float horizonSize;\n\
	\
	varying vec2 uvVarying;\n\
	varying vec2 uvVarying2;\n\
	varying vec2 horizonVarying;\n\
	\
	void main() {\n\
	    float horizon = 1.0 - min( horizonSize*horizonVarying.x, 1.0 );\n\
	    horizon *= horizon;\n\
	    vec3 color = mix( skyColor, horizonColor, horizon );\n\
		\
	    vec3 sunColor2 = sunColor*1.5 - color;\n\
	    sunColor2 *= horizonVarying.y;\n\
		\
	    float sunPoint = texture2D(texture0,uvVarying).r;\n\
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

AGKShader* OpenGL2Renderer::LoadDefaultShaderSkyBoxNoSun()
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Default SkyBox Shader" );
	pShader->m_sPSFilename.SetStr( "Default SkyBox Shader" );

	pShader->m_sVSSource.SetStr( "\
	#version 110\n\
	attribute vec3 position;\n\
	\
	varying vec2 horizonVarying;\n\
	\
	uniform mat4 agk_World;\n\
	uniform mat4 agk_ViewProj;\n\
	\
	uniform float horizonHeight;\n\
	uniform float objectScale; // 1.0 / objectScaleY \n\
	\
	void main() {\n\
	    vec4 pos = agk_World * vec4(position,1.0);\n\
	    gl_Position = agk_ViewProj * pos;\n\
	    gl_Position.z = gl_Position.w; // force it to the back \n\
		\
	    horizonVarying.x = (pos.y-horizonHeight)*objectScale;\n\
	}" );

	pShader->m_sPSSource.SetStr( "\
	#version 110\n\
	uniform sampler2D texture0;\n\
	\
	uniform vec3 skyColor;\n\
	uniform vec3 horizonColor;\n\
	uniform float horizonSize;\n\
	\
	varying vec2 horizonVarying;\n\
	\
	void main() {\n\
	    float horizon = 1.0 - min( horizonSize*horizonVarying.x, 1.0 );\n\
		horizon *= horizon;\n\
		vec3 color = mix( skyColor, horizonColor, horizon );\n\
		gl_FragColor = vec4(color,1.0);\n\
	}" );

	pShader->m_iFlags |= AGK_SHADER_IS_DEFAULT;

	InternalCompileShader( pShader );
	InternalEnumerateShaderResources( pShader );

	return pShader;
}

AGKShader* OpenGL2Renderer::Make3DShader( cMesh *pMesh )
{
	AGKShader *pShader = new AGKShader();

	pShader->m_sVSFilename.SetStr( "Generated 3D Shader" );
	pShader->m_sPSFilename.SetStr( "Generated 3D Shader" );

	// Vertex shader
	pShader->m_sVSSource.ClearTemp();
	pShader->m_sVSSource.AppendAscii( "attribute vec3 position;\n" );
	if ( pMesh->HasNormals() && pMesh->WantsLighting() ) 
	{
		pShader->m_iFlags |= AGK_SHADER_USES_VS_LIGHTING;

		pShader->m_sVSSource.AppendAscii( "attribute vec3 normal;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 posVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 normalVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "varying vec3 lightVarying;\n" );
		pShader->m_sVSSource.AppendAscii( "vec3 GetVSLighting( vec3 normal, vec3 pos );\n" );
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
			pShader->m_sVSSource.AppendAscii( "    if ( abs(normal.y) > 0.999 ) tangent = vec3( normal.y,0.0,0.0 );\n" );
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
		pShader->m_sPSSource.AppendAscii( "vec3 GetPSLighting( vec3 normal, vec3 pos );\n" );

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
		pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( vec3 color, vec3 pointPos );\n" );
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

AGKShader* OpenGL2Renderer::MakeFinalShader( AGKShader *pBaseShader, int sunActive, int VSLights, int PSLights, int useShadows, int normalMap )
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
		pShader->m_sVSSource.AppendAscii( "vec3 GetVSLighting( vec3 normal, vec3 pos )\n" );
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
				
		pShader->m_sPSSource.AppendAscii( "vec3 GetPSLighting( vec3 normal, vec3 pos )\n" );
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
			pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( vec3 color, vec3 pointPos )\n" );
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
			pShader->m_sPSSource.AppendAscii( "vec3 ApplyFog( vec3 color, vec3 pointPos )\n" );
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

AGKShader* OpenGL2Renderer::LoadShader( const char *szVertexSource, const char *szPixelSource, const char *szVertexFile, const char *szPixelFile )
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

	if ( pShader->m_sVSSource.FindStr( "#version " ) < 0 ) pShader->m_sVSSource.Prepend( "#version 110\n" );
	if ( pShader->m_sPSSource.FindStr( "#version " ) < 0 ) pShader->m_sPSSource.Prepend( "#version 110\n" );

	pShader->m_sVSSource.ReplaceStr( "mediump ", "" );
	pShader->m_sVSSource.ReplaceStr( "highp ", "" );
	pShader->m_sVSSource.ReplaceStr( "lowp ", "" );
	pShader->m_sVSSource.ReplaceStr( "precision float;", "" );

	pShader->m_sPSSource.ReplaceStr( "mediump ", "" );
	pShader->m_sPSSource.ReplaceStr( "highp ", "" );
	pShader->m_sPSSource.ReplaceStr( "lowp ", "" );
	pShader->m_sPSSource.ReplaceStr( "precision float;", "" );

	pShader->m_iFlags |= AGK_SHADER_IS_CUSTOM;

	if ( pShader->m_sVSSource.FindStr( "vec3 GetVSLighting( vec3 normal, vec3 pos );" ) >= 0 ) 
		pShader->m_iFlags |= AGK_SHADER_USES_VS_LIGHTING;

	if ( pShader->m_sPSSource.FindStr( "vec3 GetPSLighting( vec3 normal, vec3 pos );" ) >= 0 ) 
		pShader->m_iFlags |= AGK_SHADER_USES_PS_LIGHTING;

	if ( pShader->m_sPSSource.FindStr( "vec3 ApplyFog( vec3 color, vec3 pointPos );" ) >= 0 ) 
		pShader->m_iFlags |= AGK_SHADER_USES_FOG;

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

AGKShader* OpenGL2Renderer::LoadShaderSPIRV( uint32_t *pVertexSource, uint32_t sizeVS, uint32_t *pPixelSource, uint32_t sizePS )
{
	if ( m_iShaderErrorMode == 0 ) agk::Error( "Failed to load shader, SPIRV is not supported in OpenGL2" );
	return 0;
}

void OpenGL2Renderer::ReloadShader( AGKShader *pShader )
{
	if( pShader->NeedsAdditionalCode() ) return;
	if ( !pShader->m_pInternalResources ) return;
	OpenGL2Shader *pOpenGL2Shader = (OpenGL2Shader*) pShader->m_pInternalResources;

	if ( m_iCurrShader == pOpenGL2Shader->m_iShaderID )
	{
		glUseProgram( 0 );
		m_iCurrShader = 0;
		m_pCurrAGKShader = 0;
		m_iVBOPtrSetup = 0;
	}

	if ( pOpenGL2Shader->m_iShaderID ) glDeleteProgram( pOpenGL2Shader->m_iShaderID );
	pOpenGL2Shader->m_iShaderID = 0;

	InternalCompileShader( pShader );

	pShader->m_iFlags |= AGK_SHADER_RELOAD_UNIFORMS;
}

void OpenGL2Renderer::DeleteShader( AGKShader *pShader )
{
	if ( !pShader->m_pInternalResources ) return;
	OpenGL2Shader *pOpenGL2Shader = (OpenGL2Shader*) pShader->m_pInternalResources;

	if ( m_iCurrShader == pOpenGL2Shader->m_iShaderID )
	{
		glUseProgram( 0 );
		m_iCurrShader = 0;
		m_pCurrAGKShader = 0;
		m_iVBOPtrSetup = 0;
	}

	if ( pOpenGL2Shader->m_iShaderID ) glDeleteProgram( pOpenGL2Shader->m_iShaderID );
	pOpenGL2Shader->m_iShaderID = 0;

	delete pOpenGL2Shader;
	pShader->m_pInternalResources = 0;
}

void OpenGL2Renderer::InternalCompileShader( AGKShader *pShader )
{
	OpenGL2Shader *pOpenGL2Shader = new OpenGL2Shader();
	pShader->m_pInternalResources = pOpenGL2Shader;
	pShader->SetValid( true );

	pOpenGL2Shader->m_iShaderID = glCreateProgram();

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

	glAttachShader( pOpenGL2Shader->m_iShaderID, vertShader );
	glAttachShader( pOpenGL2Shader->m_iShaderID, fragShader );

	// link program
	glLinkProgram( pOpenGL2Shader->m_iShaderID );

	glGetProgramiv( pOpenGL2Shader->m_iShaderID, GL_LINK_STATUS, &status );
	if ( status == 0 )
	{
		glDeleteShader( vertShader );
		glDeleteShader( fragShader );

		glGetProgramiv ( pOpenGL2Shader->m_iShaderID, GL_INFO_LOG_LENGTH, &logLength);
		if ( logLength > 0 )
		{
			char *log = new char[ logLength+1 ];
			glGetProgramInfoLog( pOpenGL2Shader->m_iShaderID, logLength, &logLength, log );
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

void OpenGL2Renderer::InternalEnumerateShaderResources( AGKShader *pShader )
{
	if ( !pShader->m_pInternalResources ) return;
	OpenGL2Shader *pOpenGL2Shader = (OpenGL2Shader*) pShader->m_pInternalResources;
	
	// bind it so we can set the texture locations (if any)
	if ( m_iCurrShader != pOpenGL2Shader->m_iShaderID )
	{
		glUseProgram( pOpenGL2Shader->m_iShaderID );
		m_iCurrShader = pOpenGL2Shader->m_iShaderID;
		m_iVBOPtrSetup = 0;
	}

	// attributes
	if ( pShader->m_pAttribs ) delete [] pShader->m_pAttribs;
	pShader->m_pAttribs = 0;
	pShader->m_iNumAttribs = 0;

	glGetProgramiv( pOpenGL2Shader->m_iShaderID, GL_ACTIVE_ATTRIBUTES, &pShader->m_iNumAttribs );
	if ( pShader->m_iNumAttribs > 0 )
	{
		pShader->m_pAttribs = new AGKShaderAttrib[ pShader->m_iNumAttribs ];

		int maxLength = 0;
		glGetProgramiv( pOpenGL2Shader->m_iShaderID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength );
		char* szAttribName = new char[ maxLength+1 ];

		int length = 0;
		int size = 0;
		uint32_t type = 0;
		for ( int i = 0; i < pShader->m_iNumAttribs; i++ )
		{
			AGKShaderAttrib *pAttrib = &(pShader->m_pAttribs[ i ]);
			glGetActiveAttrib( pOpenGL2Shader->m_iShaderID, i, maxLength, &length, &size, &type, szAttribName );
			pAttrib->m_iLocation = glGetAttribLocation( pOpenGL2Shader->m_iShaderID, szAttribName );
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

	for( int i = 0; i < AGK_SHADER_CONSTANT_NAME_MAX_VALUE; i++ ) pOpenGL2Shader->m_indexByKnownName[ i ] = (uint16_t) -1;
	if ( pOpenGL2Shader->m_pLocations ) delete [] pOpenGL2Shader->m_pLocations;
	pOpenGL2Shader->m_iNumLocations = 0;

	for( int i = 0; i < AGK_OPENGL2_MAX_TEXTURE_STAGES; i++ )
	{
		pOpenGL2Shader->m_iTextureLocations[ i ] = (uint16_t) -1;
	}
	
	int numUniforms;
	glGetProgramiv( pOpenGL2Shader->m_iShaderID, GL_ACTIVE_UNIFORMS, &numUniforms );
	if ( numUniforms > 65000 )
	{
		if ( m_iShaderErrorMode < 2 ) agk::Error( "Shader has too many uniform values" );
		pShader->SetValid( false );
		return;
	}

	if ( numUniforms > 0 )
	{
		pShader->m_constantBindingByName.Resize( numUniforms + numUniforms/2 );
		pOpenGL2Shader->m_pLocations = new OpenGL2ShaderLocation[ numUniforms ];

		int maxLength = 0;
		glGetProgramiv( pOpenGL2Shader->m_iShaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength );
		char* szUniformName = new char[ maxLength+1 ];

		uint16_t locationIndex = 0;
		int length = 0;
		int size = 0;
		uint32_t type = 0;
		for ( int i = 0; i < numUniforms; i++ )
		{
			glGetActiveUniform( pOpenGL2Shader->m_iShaderID, i, maxLength, &length, &size, &type, szUniformName );
			uint32_t location = (uint32_t) glGetUniformLocation( pOpenGL2Shader->m_iShaderID, szUniformName );
			pOpenGL2Shader->m_pLocations[ locationIndex ].m_iLocation = location;
			switch( type )
			{
				case GL_FLOAT: pOpenGL2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_FLOAT; break;
				case GL_FLOAT_VEC2: pOpenGL2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_VEC2; break;
				case GL_FLOAT_VEC3: pOpenGL2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_VEC3; break;
				case GL_FLOAT_VEC4: pOpenGL2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_VEC4; break;
				case GL_FLOAT_MAT2: pOpenGL2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_MAT2; break;
				case GL_FLOAT_MAT3: pOpenGL2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_MAT3; break;
				case GL_FLOAT_MAT4: pOpenGL2Shader->m_pLocations[ locationIndex ].m_iType = AGK_SHADER_CONSTANT_TYPE_MAT4; break;
				case GL_SAMPLER_2D: break;
				default: if ( m_iShaderErrorMode < 2 ) agk::Error( "Unsupported shader variable type, only float, vec2, vec3, vec4, mat2, mat3, mat4, and sampler2D are supported" );
			}
			
			// remove array brackets
			char *str = szUniformName;
			while( *str )
			{
				if ( *str == '[' ) { *str = 0; break; }
				str++;
			}

			uint8_t iTextureIsSet[ AGK_OPENGL2_MAX_TEXTURE_STAGES ] = {0};

			if ( strcmp(szUniformName, "fogColor1") == 0 )
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varFogColor1;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_FOG_COLOR1 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "fogColor2") == 0 ) 			
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varFogColor2;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_FOG_COLOR2 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "fogRange") == 0 )				pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varFogRange;
			else if ( strcmp(szUniformName, "agk_invert") == 0 ) 			pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varInvert;
			else if ( strcmp(szUniformName, "agk_time") == 0 ) 				pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varTime;
			else if ( strcmp(szUniformName, "agk_sintime") == 0 ) 			pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varSinTime;
			else if ( strcmp(szUniformName, "agk_resolution") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varResolution;
			else if ( strcmp(szUniformName, "agk_ShadowParams") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowParams;
			else if ( strcmp(szUniformName, "agk_ShadowParams2") == 0 ) 	pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowParams2;
			else if ( strcmp(szUniformName, "agk_Ortho") == 0 ) 			pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varOrtho;
			else if ( strcmp(szUniformName, "agk_ShadowProj") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowProj;
			else if ( strcmp(szUniformName, "agk_Shadow2Proj") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowProj2;
			else if ( strcmp(szUniformName, "agk_Shadow3Proj") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowProj3;
			else if ( strcmp(szUniformName, "agk_Shadow4Proj") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varShadowProj4;
			else if ( strcmp(szUniformName, "agk_DLightDir") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varDirLightDir;
			else if ( strcmp(szUniformName, "agk_DLightColor") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varDirLightColor;
			else if ( strcmp(szUniformName, "agk_LightAmbient") == 0 ) 		pOpenGL2Shader->m_pLocations[ locationIndex++ ].m_pDefaultValue = &AGKShader::g_varLightAmbient;

			else if ( strcmp(szUniformName, "agk_bonequats1") == 0 )		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_OBJ_BONES1 ] = locationIndex++;		
			else if ( strcmp(szUniformName, "agk_bonequats2") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_OBJ_BONES2 ] = locationIndex++;		
			else if ( strcmp(szUniformName, "agk_MeshDiffuse") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_MESH_DIFFUSE ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_MeshEmissive") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_MESH_EMISSIVE ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_NormalScale") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_NORMAL_SCALE ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_CameraPos") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_CAMERA_POS ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_spritepos") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_POS ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_spritesize") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_SIZE ] = locationIndex++;
																							
			else if ( strcmp(szUniformName, "agk_World") == 0 ) 			pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_MAT ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_WorldNormal") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_NORMAL ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_View") == 0 ) 				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_Proj") == 0 ) 				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PROJ ] = locationIndex++;
																										
			else if ( strcmp(szUniformName, "agk_ViewProj") == 0 ) 			pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW_PROJ ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_WorldViewProj") == 0 ) 	pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_VIEW_PROJ ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_ShadowWorldProj") == 0 ) 	pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SHADOW_WORLD_PROJ ] = locationIndex++;
																							
			else if ( strcmp(szUniformName, "agk_VSLightPos") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VS_LIGHT_POS ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_VSLightColor") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VS_LIGHT_COLOR ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_PSLightPos") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PS_LIGHT_POS ] = locationIndex++;
			else if ( strcmp(szUniformName, "agk_PSLightColor") == 0 ) 		pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PS_LIGHT_COLOR ] = locationIndex++;
																										
			else if ( strcmp(szUniformName, "uvBounds0") == 0 ) 			
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_0 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "uvBounds1") == 0 )
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_1 ] = locationIndex++;
				
			}
			else if ( strcmp(szUniformName, "uvBounds2") == 0 ) 
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_2 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "uvBounds3") == 0 ) 
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_3 ] = locationIndex++;				
			}
			else if ( strcmp(szUniformName, "uvBounds4") == 0 )
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_4 ] = locationIndex++;				
			}
			else if ( strcmp(szUniformName, "uvBounds5") == 0 )
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_5 ] = locationIndex++;				
			}
			else if ( strcmp(szUniformName, "uvBounds6") == 0 )
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_6 ] = locationIndex++;				
			}
			else if ( strcmp(szUniformName, "uvBounds7") == 0 )
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_UV_BOUNDS_7 ] = locationIndex++;				
			}
																							
			else if ( strcmp(szUniformName, "textureBounds0") == 0 )
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_0 ] = locationIndex++;				
			}
			else if ( strcmp(szUniformName, "textureBounds1") == 0 ) 
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_1 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds2") == 0 ) 
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_2 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds3") == 0 ) 
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_3 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds4") == 0 )
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_4 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds5") == 0 ) 
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_5 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds6") == 0 ) 
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_6 ] = locationIndex++;
			}
			else if ( strcmp(szUniformName, "textureBounds7") == 0 ) 
			{
				pOpenGL2Shader->m_pLocations[ locationIndex ].m_pDefaultValue = &AGKShader::g_varDefaultUVBounds;
				pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_7 ] = locationIndex++;	
			}
																											
			else if ( strcmp(szUniformName, "texture0") == 0 ) { iTextureIsSet[ 0 ] = 1; pOpenGL2Shader->m_iTextureLocations[ 0 ] = location; glUniform1i( location, 0 ); }
			else if ( strcmp(szUniformName, "texture1") == 0 ) { iTextureIsSet[ 1 ] = 1; pOpenGL2Shader->m_iTextureLocations[ 1 ] = location; glUniform1i( location, 1 ); }
			else if ( strcmp(szUniformName, "texture2") == 0 ) { iTextureIsSet[ 2 ] = 1; pOpenGL2Shader->m_iTextureLocations[ 2 ] = location; glUniform1i( location, 2 ); }
			else if ( strcmp(szUniformName, "texture3") == 0 ) { iTextureIsSet[ 3 ] = 1; pOpenGL2Shader->m_iTextureLocations[ 3 ] = location; glUniform1i( location, 3 ); }
			else if ( strcmp(szUniformName, "texture4") == 0 ) { iTextureIsSet[ 4 ] = 1; pOpenGL2Shader->m_iTextureLocations[ 4 ] = location; glUniform1i( location, 4 ); }
			else if ( strcmp(szUniformName, "texture5") == 0 ) { iTextureIsSet[ 5 ] = 1; pOpenGL2Shader->m_iTextureLocations[ 5 ] = location; glUniform1i( location, 5 ); }
			else if ( strcmp(szUniformName, "texture6") == 0 ) { iTextureIsSet[ 6 ] = 1; pOpenGL2Shader->m_iTextureLocations[ 6 ] = location; glUniform1i( location, 6 ); }
			else if ( strcmp(szUniformName, "texture7") == 0 ) { iTextureIsSet[ 7 ] = 1; pOpenGL2Shader->m_iTextureLocations[ 7 ] = location; glUniform1i( location, 7 ); }

			else if ( strcmp(szUniformName, "cubeMap0") == 0 ) { iTextureIsSet[ 0 ] = 2; pOpenGL2Shader->m_iTextureLocations[ 0 ] = location; glUniform1i( location, 0 ); }
			else if ( strcmp(szUniformName, "cubeMap1") == 0 ) { iTextureIsSet[ 1 ] = 2; pOpenGL2Shader->m_iTextureLocations[ 1 ] = location; glUniform1i( location, 1 ); }
			else if ( strcmp(szUniformName, "cubeMap2") == 0 ) { iTextureIsSet[ 2 ] = 2; pOpenGL2Shader->m_iTextureLocations[ 2 ] = location; glUniform1i( location, 2 ); }
			else if ( strcmp(szUniformName, "cubeMap3") == 0 ) { iTextureIsSet[ 3 ] = 2; pOpenGL2Shader->m_iTextureLocations[ 3 ] = location; glUniform1i( location, 3 ); }
			else if ( strcmp(szUniformName, "cubeMap4") == 0 ) { iTextureIsSet[ 4 ] = 2; pOpenGL2Shader->m_iTextureLocations[ 4 ] = location; glUniform1i( location, 4 ); }
			else if ( strcmp(szUniformName, "cubeMap5") == 0 ) { iTextureIsSet[ 5 ] = 2; pOpenGL2Shader->m_iTextureLocations[ 5 ] = location; glUniform1i( location, 5 ); }
			else if ( strcmp(szUniformName, "cubeMap6") == 0 ) { iTextureIsSet[ 6 ] = 2; pOpenGL2Shader->m_iTextureLocations[ 6 ] = location; glUniform1i( location, 6 ); }
			else if ( strcmp(szUniformName, "cubeMap7") == 0 ) { iTextureIsSet[ 7 ] = 2; pOpenGL2Shader->m_iTextureLocations[ 7 ] = location; glUniform1i( location, 7 ); }
			
			else if ( strcmp(szUniformName, "shadowMap") == 0 ) 
			{ 
				if ( iTextureIsSet[ 7 ] && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 7, it is overriden by shadow mapping" );
				pOpenGL2Shader->m_iTextureLocations[ 7 ] = location; 
				glUniform1i( location, 7 );
			}
			else if ( strcmp(szUniformName, "shadowMap2") == 0 ) 
			{ 
				if ( iTextureIsSet[ 6 ] && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 6, it is overriden by cascade shadow mapping" );
				pOpenGL2Shader->m_iTextureLocations[ 6 ] = location; 
				glUniform1i( location, 6 );
			}
			else if ( strcmp(szUniformName, "shadowMap3") == 0 ) 
			{ 
				if ( iTextureIsSet[ 5 ] && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 5, it is overriden by cascade shadow mapping" );
				pOpenGL2Shader->m_iTextureLocations[ 5 ] = location; 
				glUniform1i( location, 5 );
			}
			else if ( strcmp(szUniformName, "shadowMap4") == 0 ) 
			{ 
				if ( iTextureIsSet[ 4 ] && m_iShaderErrorMode < 2 ) agk::Error( "Cannot use texture stage 4, it is overriden by cascade shadow mapping" );
				pOpenGL2Shader->m_iTextureLocations[ 4 ] = location; 
				glUniform1i( location, 4 );
			}

			else
			{
				pShader->m_constantBindingByName.AddItem( locationIndex+1, szUniformName ); // +1 to avoid adding zero which is a null element
				locationIndex++;
			}
		}

		delete [] szUniformName;

		if ( pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_POS ] != (uint16_t) -1
		  || pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SPRITE_SIZE ] != (uint16_t) -1 )
		{
			pShader->m_iFlags |= AGK_SHADER_PER_SPRITE_UNIFORM;
		}

		pOpenGL2Shader->m_iNumLocations = locationIndex;
	}
}

void OpenGL2Renderer::SetShaderConstant( AGKShader *pShader, uint32_t index, AGKShaderConstantValue *pConstant )
{
	if ( !pShader ) return;
	OpenGL2Shader *pOpenGL2Shader = (OpenGL2Shader*) (pShader->m_pInternalResources);
	if ( !pOpenGL2Shader ) return;
	if ( index >= pOpenGL2Shader->m_iNumLocations ) return;

	if ( (pOpenGL2Shader->m_pLocations[ index ].m_iFlags & AGK_OPENGL2_SHADER_LOCATION_USES_LOCAL_MEM) && pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue )
	{
		delete pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue;
	}
	pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue = pConstant;
}

int OpenGL2Renderer::BindShaderConstants( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKUnorderedArray<AGKShaderConstantValue*> *pAdditionalConstants, 
										  AGKDynamicArray<cImage*> *pTextures, void **inoutConstants )
{
	if ( !pShader || !pShader->m_pInternalResources ) return APP_ERROR_GENERAL;
	OpenGL2Shader *pOpenGL2Shader = (OpenGL2Shader*) pShader->m_pInternalResources;
	if ( m_iCurrShader != pOpenGL2Shader->m_iShaderID )
	{
		glUseProgram( pOpenGL2Shader->m_iShaderID );
		m_iCurrShader = pOpenGL2Shader->m_iShaderID;
		m_iVBOPtrSetup = 0;
	}

	// reset location flags
	for( uint32_t i = 0; i < pOpenGL2Shader->m_iNumLocations; i++ )
	{
		pOpenGL2Shader->m_pLocations[ i ].UnSet();
		if ( pShader->m_iFlags & AGK_SHADER_RELOAD_UNIFORMS ) pOpenGL2Shader->m_pLocations[ i ].Reload();
	}

	// bind images and check for image texture bound constants
	if ( pTextures )
	{
		for( int i = 0; i <= pTextures->GetHighestIndex(); i++ )
		{
			cImage *pImage = pTextures->GetItem( i );
			if ( pImage && pOpenGL2Shader->m_iTextureLocations[ i ] != (uint16_t) -1 )
			{
				BindTexture( pImage->GetInternalResource(), i ); // null image is accepted
				
				// check for texture bounds
				uint16_t index = pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_TEX_BOUNDS_0 + i ];
				if ( index != (uint16_t) -1 && pImage )
				{
					pOpenGL2Shader->m_pLocations[ index ].Set();
					if ( pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue != pImage->GetShaderVarTexBounds() ) 
					{
						pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue = pImage->GetShaderVarTexBounds();
						pOpenGL2Shader->m_pLocations[ index ].Reload();
					}
				}
			}
		}

		for( int i = pTextures->GetHighestIndex()+1; i < AGK_OPENGL2_MAX_TEXTURE_STAGES; i++ )
		{
			if ( pOpenGL2Shader->m_iTextureLocations[ i ] != (uint16_t) -1 ) BindTexture( 0, i );
		}
	}
	else
	{
		// no textures
		for( uint32_t i = 0; i < AGK_OPENGL2_MAX_TEXTURE_STAGES; i++ )
		{
			if ( pOpenGL2Shader->m_iTextureLocations[ i ] != (uint16_t) -1 ) BindTexture( 0, i );
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
					index = pOpenGL2Shader->m_indexByKnownName[ pConstant->GetKnownName() ];
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

			if ( pOpenGL2Shader->m_pLocations[ index ].IsSet() ) continue;
			pOpenGL2Shader->m_pLocations[ index ].Set();
			if ( pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue != pConstant ) 
			{
				pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue = pConstant;
				pOpenGL2Shader->m_pLocations[ index ].Reload();
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
					index = pOpenGL2Shader->m_indexByKnownName[ pConstant->GetKnownName() ];
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

			if ( pOpenGL2Shader->m_pLocations[ index ].IsSet() ) continue;
			pOpenGL2Shader->m_pLocations[ index ].Set();
			if ( pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue != pConstant ) 
			{
				pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue = pConstant;
				pOpenGL2Shader->m_pLocations[ index ].Reload();
			}
			// check type?
		}
	}

	// camera constants
	cCamera *pCamera = agk::GetCurrentCamera();

	// camera view matrix
	uint16_t index = pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW ];
	if ( index != (uint16_t) -1 && !pOpenGL2Shader->m_pLocations[ index ].IsSet() ) 
	{
		pOpenGL2Shader->m_pLocations[ index ].Set();
		AGKShaderConstantValue *pConstantView = pCamera->GetShaderView();
		if ( pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue != pConstantView ) 
		{
			pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue = pConstantView;
			pOpenGL2Shader->m_pLocations[ index ].Reload();
		}
	}

	// camera proj matrix
	index = pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_PROJ ];
	if ( index != (uint16_t) -1 && !pOpenGL2Shader->m_pLocations[ index ].IsSet() ) 
	{
		pOpenGL2Shader->m_pLocations[ index ].Set();
		AGKShaderConstantValue *pConstantProj = pCamera->GetShaderProj();
		if ( pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue != pConstantProj ) 
		{
			pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue = pConstantProj;
			pOpenGL2Shader->m_pLocations[ index ].Reload();
		}
	}

	// camera viewProj matrix
	index = pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_VIEW_PROJ ];
	if ( index != (uint16_t) -1 && !pOpenGL2Shader->m_pLocations[ index ].IsSet() ) 
	{
		pOpenGL2Shader->m_pLocations[ index ].Set();
		AGKShaderConstantValue *pConstantViewProj = pCamera->GetShaderViewProj();
		if ( pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue != pConstantViewProj ) 
		{
			pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue = pConstantViewProj;
			pOpenGL2Shader->m_pLocations[ index ].Reload();
		}
	}

	// camera pos
	index = pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_CAMERA_POS ];
	if ( index != (uint16_t) -1 && !pOpenGL2Shader->m_pLocations[ index ].IsSet() ) 
	{
		pOpenGL2Shader->m_pLocations[ index ].Set();
		AGKShaderConstantValue *pConstantCamPos = pCamera->GetShaderPos();
		if ( pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue != pConstantCamPos ) 
		{
			pOpenGL2Shader->m_pLocations[ index ].m_pCurrValue = pConstantCamPos;
			pOpenGL2Shader->m_pLocations[ index ].Reload();
		}
	}
	
	// calculate WVP matrix
	index = pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_WORLD_VIEW_PROJ ];
	if ( index != (uint16_t) -1 && !pOpenGL2Shader->m_pLocations[ index ].IsSet() && pWorldConstant ) 
	{
		// don't set it as it is a default value
		
		if ( !pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue )
		{
			pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue = new AGKShaderConstantValue();
			pOpenGL2Shader->m_pLocations[ index ].m_iFlags |= AGK_OPENGL2_SHADER_LOCATION_USES_LOCAL_MEM;
		}

		AGKShaderConstantValue *pConstantViewProj = pCamera->GetShaderViewProj();
		AGKMatrix4 matWVP, matViewProj;
		matWVP.Set( pWorldConstant->GetValues() );
		matViewProj.Set( pConstantViewProj->GetValues() );
		matWVP.Mult( matViewProj );

		pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue->SetMatrix4( matWVP.GetFloatPtr() );
	}

	// calculate WorldShadow matrix
	index = pOpenGL2Shader->m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_SHADOW_WORLD_PROJ ];
	if ( index != (uint16_t) -1 && !pOpenGL2Shader->m_pLocations[ index ].IsSet() && pWorldConstant ) 
	{
		// don't set it as it is a default value
		
		if ( !pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue )
		{
			pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue = new AGKShaderConstantValue();
			pOpenGL2Shader->m_pLocations[ index ].m_iFlags |= AGK_OPENGL2_SHADER_LOCATION_USES_LOCAL_MEM;
		}

		AGKMatrix4 matWS, matShadowProj;
		matWS.Set( pWorldConstant->GetValues() );
		matShadowProj.Set( AGKShader::g_varShadowProj.GetValues() );
		matWS.Mult( matShadowProj );

		pOpenGL2Shader->m_pLocations[ index ].m_pDefaultValue->SetMatrix4( matWS.GetFloatPtr() );
	}
	
	// set locations
	for( uint32_t i = 0; i < pOpenGL2Shader->m_iNumLocations; i++ )
	{
		if ( !pOpenGL2Shader->m_pLocations[ i ].IsSet() )
		{
			if ( pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue != pOpenGL2Shader->m_pLocations[ i ].m_pDefaultValue )
			{
				pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue = pOpenGL2Shader->m_pLocations[ i ].m_pDefaultValue;
				pOpenGL2Shader->m_pLocations[ i ].Reload();
			}
		}

		if ( !pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue ) continue;

		if ( pOpenGL2Shader->m_pLocations[ i ].ShouldReload()
		  || pOpenGL2Shader->m_pLocations[ i ].m_iChangeVersion != pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetChangeVersion() )
		{
			pOpenGL2Shader->m_pLocations[ i ].m_iChangeVersion = pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetChangeVersion();
			pOpenGL2Shader->m_pLocations[ i ].Reloaded();

			uint32_t elements = 1;
			if ( pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->IsArray() )
			{
				AGKShaderConstantArray *pConstantArray = (AGKShaderConstantArray*) pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue;
				elements = pConstantArray->m_iNumElements;
			}
			switch( pOpenGL2Shader->m_pLocations[ i ].m_iType )
			{
				case AGK_SHADER_CONSTANT_TYPE_FLOAT: 
				{
					glUniform1fv( pOpenGL2Shader->m_pLocations[ i ].m_iLocation, elements, pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_VEC2: 
				{
					glUniform2fv( pOpenGL2Shader->m_pLocations[ i ].m_iLocation, elements, pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_VEC3: 
				{
					glUniform3fv( pOpenGL2Shader->m_pLocations[ i ].m_iLocation, elements, pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_VEC4: 
				{
					glUniform4fv( pOpenGL2Shader->m_pLocations[ i ].m_iLocation, elements, pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_MAT2: 
				{
					glUniformMatrix2fv( pOpenGL2Shader->m_pLocations[ i ].m_iLocation, elements, GL_FALSE, pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_MAT3: 
				{
					glUniformMatrix3fv( pOpenGL2Shader->m_pLocations[ i ].m_iLocation, elements, GL_FALSE, pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
				case AGK_SHADER_CONSTANT_TYPE_MAT4: 
				{
					glUniformMatrix4fv( pOpenGL2Shader->m_pLocations[ i ].m_iLocation, elements, GL_FALSE, pOpenGL2Shader->m_pLocations[ i ].m_pCurrValue->GetValues() );
					break;
				}
			}
		}
	}
	
	if ( pShader->m_iFlags & AGK_SHADER_RELOAD_UNIFORMS )
	{
		// set texture, cubemap, or shadow map locations
		for( int i = 0; i < AGK_OPENGL2_MAX_TEXTURE_STAGES; i++ )
		{
			if ( pOpenGL2Shader->m_iTextureLocations[ i ] != (uint16_t) -1 ) glUniform1i( pOpenGL2Shader->m_iTextureLocations[ i ], i );
		}

		pShader->m_iFlags &= ~AGK_SHADER_RELOAD_UNIFORMS;
	}

	return APP_SUCCESS;
}

int OpenGL2Renderer::BindShaderConstantUpdates( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKDynamicArray<cImage*> *pTextures, void **inoutConstants )
{
	agk::Error( "BindShaderConstantUpdates currently not used" );
	return APP_ERROR_UNKNOWN;
}

int OpenGL2Renderer::BindPipeline( void *pCommandBuffer, AGKShader *pAGKShader, const AGKRenderState *pRenderState, const AGKVertexLayout *pVertexLayout, void **inoutPipeline )
{
	OpenGL2Shader *pShader = (OpenGL2Shader*) pAGKShader->m_pInternalResources;
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
			glDepthRange( depthMin, depthMax );
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
