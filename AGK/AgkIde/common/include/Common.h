// Common Header
#ifndef _H_COMMON_
#define _H_COMMON_

#include "PlatformDefines.h"

// Platform : Windows
#ifdef AGK_WINDOWS
	// disable deprecation warning
	#pragma warning(disable:4995)

	#define _AGK_ERROR_CHECK

	// Windows 7 for touch commands
	#ifndef _WIN32_WINNT //experiment (using Windows SDK 8.0 to fix sensor output)
	#define _WIN32_WINNT 0x0601 
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
	
	#ifndef _EXPORT_
		#define _EXPORT_ 
	#endif

	#define STDCALL __stdcall

	#ifndef INT64
		#define INT64 __int64
	#endif
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

	#ifndef _EXPORT_
		#define _EXPORT_ 
	#endif

	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

	#define MAX_PATH PATH_MAX

	#ifndef INT64
		#define INT64 __int64_t
	#endif

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif
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

	#ifndef _EXPORT_
		#define _EXPORT_ 
	#endif

	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL
	#define MAX_PATH 1024

	#ifndef INT64
		#define INT64 __int64_t
	#endif

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif

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

	#ifndef _EXPORT_
		#define _EXPORT_
	#endif

	#define MAX_PATH 512
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

	#define INT64 int64_t
    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif

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

#ifdef AGK_BLACKBERRY
	#define _AGK_ERROR_CHECK

	#include <EGL/egl.h>
	#include <screen/screen.h>
	#include <sys/platform.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>

	#include <ctype.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/keycodes.h>
	#include <assert.h>
	#include <bps/accelerometer.h>
	#include <bps/navigator.h>
	#include <bps/screen.h>
	#include <bps/bps.h>
	#include <bps/event.h>
	#include <bps/orientation.h>
	#include <bps/virtualkeyboard.h>
	#include <math.h>
	#include <time.h>
	#include <screen/screen.h>

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

	// Open AL
	#include <AL/al.h>
	#include <AL/alc.h>

	// Media Player
	#include <bps/mmrenderer.h>

	#ifndef _EXPORT_
		#define _EXPORT_
	#endif

	#define MAX_PATH 512
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

	#ifndef INT64
		#define INT64 int64_t
	#endif

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
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

	#ifndef _EXPORT_
		#define _EXPORT_
	#endif

	#define MAX_PATH 512
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define STDCALL

	#ifndef INT64
		#define INT64 int64_t
	#endif

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif
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

	#ifndef INT64
		#define INT64 int64_t
	#endif

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif
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

	#ifndef INT64
		#define INT64 int64_t
	#endif

    #ifndef UINT_PTR
        #define UINT_PTR uintptr_t
    #endif
#endif

extern "C" FILE* AGKfopen( const char *szPath, const char *mode );

#endif // _H_COMMON_
