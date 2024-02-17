// Common Header
#ifndef _H_COMMON_
#define _H_COMMON_

#include "PlatformDefines.h"

#if defined(__APPLE__) && !defined(__LP64__)
    #define DISABLE_VULKAN
#endif

// Platform : Windows
#ifdef AGK_WINDOWS
	// disable deprecation warning
	#pragma warning(disable:4995)

	#define _AGK_ERROR_CHECK

	// Windows 7 for touch commands
	#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0601  // must use 0x0601 for Windows 7 support
	#endif

	#include <WinSock2.h>
	#include <windows.h>
	#include <stdio.h>
	#include <math.h>
	#include <cassert>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/utime.h>
	#include <wininet.h>
	#include <time.h>
	#include <stdint.h>
	#include <xmmintrin.h>

	#ifndef GL_CLAMP_TO_EDGE
		#define GL_CLAMP_TO_EDGE  0x812F
	#endif

	#ifdef LoadImage
		#undef LoadImage
	#endif

	#ifdef SendMessage
		#undef SendMessage
	#endif

	#ifdef GetMessage
		#undef GetMessage
	#endif

	#ifdef DeleteFont
		#undef DeleteFont	
	#endif

	#ifdef CompareString
		#undef CompareString
	#endif

	#ifdef DeleteFile
		#undef DeleteFile
	#endif

	#ifdef GetCurrentTime
		#undef GetCurrentTime
	#endif

	#ifdef DrawText
		#undef DrawText
	#endif

	#ifdef PlaySound
		#undef PlaySound
	#endif
	
	#ifndef _EXPORT_
		#define _EXPORT_ 
	#endif

	#define STDCALL __stdcall

	// perform a fast conversion from float to int
	static int agkfloattoint( float flt )
	{	
		return _mm_cvt_ss2si(_mm_set_ss(flt)); 
	}
#endif

// Platform : Apple iPhone / iPad
#ifdef AGK_IOS

	#define LPSTR char*

	#define _AGK_ERROR_CHECK

	#import <Availability.h>
	#import <Foundation/Foundation.h>
	#import <UIKit/UIKit.h>
	#import <QuartzCore/QuartzCore.h>
	#import <AudioToolbox/AudioToolbox.h>
	#import <AVFoundation/AVFoundation.h>
	#import <OpenAL/al.h>
	#import <OpenAL/alc.h>

	#include "string.h"
	#include <math.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>	
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
	#include <utime.h>
    #include <ifaddrs.h>
    #include <arpa/inet.h>
	#include <stdint.h>

	#ifndef _EXPORT_
		#define _EXPORT_ 
	#endif

	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

	#define MAX_PATH PATH_MAX

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif

	static int agkfloattoint( float flt )
	{	
		return (int) lrintf( flt );
	}
#endif

// Platform : Mac
#ifdef AGK_MACOS

	#define _AGK_ERROR_CHECK
	
	#import <Cocoa/Cocoa.h>

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <string.h>
	#include <math.h>
	#include <sys/time.h>
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
	#include <mach/mach_time.h>
	#include <mach-o/dyld.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>	
	#include <sys/param.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
	#include <utime.h>
    #include <ifaddrs.h>
	#include <stdint.h>

	#ifndef _EXPORT_
		#define _EXPORT_ 
	#endif

	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL
	#define MAX_PATH 1024

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif

	static int agkfloattoint( float flt )
	{	
		return (int)lrintf( flt ); 
	}
#endif

#ifdef AGK_ANDROID
	#define _AGK_ERROR_CHECK

	#include <stdio.h>
	#include <string.h>
	#include <jni.h>
	#include <stdlib.h>
	#include <math.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>	
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
	#include <utime.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <pthread.h>
	#include <stdint.h>

	#ifndef _EXPORT_
		#define _EXPORT_
	#endif

	#define MAX_PATH 512
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif

	static int agkfloattoint( float flt )
	{	
		return lrintf( flt );
	}

	#ifndef __ARM_ARCH__
	#  if defined __ARM_ARCH_8__   || defined __ARM_ARCH_8A__ || \
		  defined __ARM_ARCH_8R__  || defined __ARM_ARCH_8M__ || defined __ARM_ARCH_ISA_A64
	#    define __ARM_ARCH__ 8
	#
	#  elif defined __ARM_ARCH_7__   || defined __ARM_ARCH_7A__ || \
		  defined __ARM_ARCH_7R__  || defined __ARM_ARCH_7M__
	#    define __ARM_ARCH__ 7
	#
	#  elif defined __ARM_ARCH_6__   || defined __ARM_ARCH_6J__ || \
		  defined __ARM_ARCH_6K__  || defined __ARM_ARCH_6Z__ || \
		  defined __ARM_ARCH_6KZ__ || defined __ARM_ARCH_6T2__
	#    define __ARM_ARCH__ 6
	#
	#  elif defined __ARM_ARCH_5__ || defined __ARM_ARCH_5T__ || \
			defined __ARM_ARCH_5TE__ || defined __ARM_ARCH_5TEJ__
	#    define __ARM_ARCH__ 5
	#  else 
	#    define __ARM_ARCH__ 4
	#  endif
	#endif
#endif

#ifdef AGK_LINUX
	#define _AGK_ERROR_CHECK

	#include <ctype.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include <math.h>
	#include <time.h>

	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
	#include <utime.h>
	#include <fcntl.h>
	#include <pthread.h>
	#include <stdarg.h>
	#include <stdint.h>
	#include <mm_malloc.h>

	#ifndef _EXPORT_
		#define _EXPORT_
	#endif

	#define MAX_PATH 512
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif

	static int agkfloattoint( float flt )
	{	
		return lrintf( flt ); 
	}
#endif

#ifdef AGK_RASPPI
	#define _AGK_ERROR_CHECK

	#include <ctype.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include <math.h>
	#include <time.h>

	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
	#include <utime.h>
	#include <fcntl.h>
	#include <pthread.h>
	#include <stdarg.h>
	#include <stdint.h>

	#ifndef _EXPORT_
		#define _EXPORT_
	#endif

	#define MAX_PATH 512
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif

	static int agkfloattoint( float flt )
	{	
		return lrintf( flt );
	}
#endif

#ifdef AGK_HTML5
	#define _AGK_ERROR_CHECK

	#include <ctype.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include <math.h>
	#include <time.h>

	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
	#include <utime.h>
	#include <fcntl.h>
	#include <stdarg.h>
	#include <stdint.h>

	#ifndef _EXPORT_
		#define _EXPORT_
	#endif

	#define MAX_PATH 512
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif

	static int agkfloattoint( float flt )
	{	
		return lrintf( flt );
	}
#endif

#ifndef UINT
	#define UINT uint32_t
#endif

#if ((defined(AGK_IOS) || defined(AGK_MACOS)) && !defined(__i386__) && !defined(__x86_64__)) \
 || defined(AGK_ANDROID) || defined(AGK_HTML5)
static void* _mm_malloc(size_t i, size_t alignment)
{
    void *mem;
    posix_memalign( &mem, alignment, i );
    return mem;
}

static void _mm_free(void* p)
{
    free(p);
}
#endif

extern "C" FILE* AGKfopen( const char *szPath, const char *mode );

#endif // _H_COMMON_
