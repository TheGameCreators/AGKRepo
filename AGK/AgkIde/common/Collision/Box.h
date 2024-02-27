#ifndef _H_AGKBOX
#define _H_AGKBOX

//#define L 1000000000
//#define S -1000000000

#include "3DMath.h"

namespace AGK
{
	class Box
	{

		public:
	    
			//AGKVector lastNormal;
	        
			Box(AGKVector* min, AGKVector* max);
			Box(float x=1000000000,float y=1000000000,float z=1000000000,float x2=-1000000000,float y2=-1000000000,float z2=-1000000000);
			~Box() {}
	        
			void resetBox();        
			void set(AGKVector* min, AGKVector* max);
			void set(float x,float y,float z,float x2,float y2,float z2);
			void copy( Box *pOther );
	        
			void correctBox();        
	        
			bool inBox(AGKVector* p);        
			int intersectBox(AGKVector* p, AGKVector* vi, float* result);

			bool quickSphereIntersectBox(AGKVector* p, AGKVector* vi, float rRadius, AGKVector* scale, float* dist);
	        
			int roughBox(Box* b, AGKVector* scale, AGKMatrix4* transform);
			int collidesBox(Box* b, AGKVector* scale, AGKMatrix4* transform);

			bool collidesFace(AGKVector* p1, AGKVector* p2, AGKVector* p3);

			int inFrustumPlane( const AGKVector* n, float d ); // 0=outside, 1=inside, normal should point inwards
			int inFrustum( const AGKMatrix4* transform );
	                
			float maxbx() const { return maxx; }
			float maxby() const { return maxy; }
			float maxbz() const { return maxz; }
			float minbx() const { return minx; }
			float minby() const { return miny; }
			float minbz() const { return minz; }
	        
			void maxbx(float x) { maxx = x; }
			void maxby(float y) { maxy = y; }
			void maxbz(float z) { maxz = z; }
			void minbx(float x) { minx = x; }
			void minby(float y) { miny = y; }
			void minbz(float z) { minz = z; }
	    
		protected:
	    
			float maxx,maxy,maxz,minx,miny,minz;
	};
}

#endif
