#ifndef _H_NETWORK_COMMON
#define _H_NETWORK_COMMON

	#include "PlatformDefines.h"

	#ifdef AGK_WINDOWS
		#include "../../platform/windows/Source/WindowsNetwork.h"
	#endif

	#ifdef AGK_IOS
		#include "../../platform/apple/Source/AppleNetwork.h"
	#endif

	#ifdef AGK_MACOS
		#include "../../platform/mac/Source/MacNetwork.h"
	#endif

	#ifdef AGK_ANDROID
		#include "../../platform/android/Source/AndroidNetwork.h"
	#endif

	#ifdef AGK_BLACKBERRY
		#include "../../platform/blackberry/Source/BlackberryNetwork.h"
	#endif

	#ifdef AGK_LINUX
		#include "../../platform/linux/Source/LinuxNetwork.h"
	#endif

	#ifdef AGK_RASPPI
		#include "../../platform/pi/Source/PiNetwork.h"
	#endif
	
	#ifdef AGK_HTML5
		#include "../../platform/html5/Source/HTML5Network.h"
	#endif

#endif
