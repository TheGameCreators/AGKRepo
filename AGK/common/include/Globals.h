#ifndef _H_AGK_GLOBALS_
#define _H_AGK_GLOBALS_

#ifndef _EXPORT_
#define _EXPORT_ 
#endif

#ifndef PI
#define PI 3.141592654f
#endif

#include <stdint.h>

// Box2D forward declarations
class b2Body;
class b2Shape;
class b2World;
class b2Joint;
class b2Fixture;
struct b2DistanceOutput;
class b2Contact;
struct b2ContactEdge;

struct AGKSortValue
{
	unsigned int iValue;
	void* ptr;
};

struct AGKSortValue64
{
	uint64_t iValue;
	void* ptr;
};

#endif