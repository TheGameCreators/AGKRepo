#include "PlatformDefines.h"

#ifdef AGK_WINDOWS
	#define HAVE_SYS_TYPES_H
	#define HAVE_BASETSD_H
	#define HAVE_VARARGS_H
	#define HAVE_FCNTL_H
	#define HAVE_IO_H
	#define WINDOWS32
#else
	#define HAVE_STDARG_H
	#define HAVE_SYS_TYPES_H
	#define HAVE_UNISTD_H
	#define HAVE_FCNTL_H
	#define HAVE_STDINT_H
	#define UINT32 uint32_t

	#if !defined(S_IREAD)
		#define S_IREAD  S_IRUSR
		#define S_IWRITE S_IRUSR
	#endif
#endif