#ifndef _H_ZXING_GLOBALS
#define _H_ZXING_GLOBALS

namespace zxing
{
	inline bool custom_isnan( float f )
	{
		volatile float vf = f;
		return vf != f ? true : false;
	}
}

#endif