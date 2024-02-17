#ifndef _H_AGK_PLATFORMDEFINES_
#define _H_AGK_PLATFORMDEFINES_

// for backwards compatibility set the old defines to enable the new defines
#if defined(IDE_VS2008) && !defined(AGK_WINDOWS)
	#define AGK_WINDOWS
#endif

#if defined(IDE_XCODE) && !defined(AGK_IOS)
	#define AGK_IOS
#endif

#if defined(IDE_MAC) && !defined(AGK_MACOS)
	#define AGK_MACOS
#endif

#if defined(IDE_ANDROID) && !defined(AGK_ANDROID)
	#define AGK_ANDROID
#endif

#if defined(IDE_BLACKBERRY) && !defined(AGK_BLACKBERRY)
	#define AGK_BLACKBERRY
#endif

#if defined(IDE_LINUX) && !defined(AGK_LINUX)
	#define AGK_LINUX
#endif

#if defined(IDE_PI) && !defined(AGK_RASPPI)
	#define AGK_RASPPI
#endif

#if defined(IDE_HTML5) && !defined(AGK_HTML5)
	#define AGK_HTML5
#endif

#endif // _H_AGK_PLATFORMDEFINES_