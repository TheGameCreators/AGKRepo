#include "CollisionSphere.h"
#include <math.h>

using namespace AGK;

void CollisionSphere::drawBounds(int freeObj)
{    
	/*
    MakeObjectSphere(freeObj, radius*2.0f);
    //PositionObject(freeObj, centerX, centerY, centerZ);
    OffsetLimb(freeObj, 0, centerX, centerY, centerZ);
	*/
}

void CollisionSphere::makeCollisionObject(Face *pFaces)
{
	float maxx = -1000000000;
    float maxy = -1000000000;
    float maxz = -1000000000;
    float minx = 1000000000;
    float miny = 1000000000;
    float minz = 1000000000;

	float newRadius = 0;

	Face *pFace = pFaces;

	while ( pFace )
    {
		if ( pFace->vert1.x > maxx )	maxx = pFace->vert1.x;
		if ( pFace->vert1.x < minx )	minx = pFace->vert1.x;
		if ( pFace->vert1.y > maxy )	maxy = pFace->vert1.y;
		if ( pFace->vert1.y < miny )	miny = pFace->vert1.y;
		if ( pFace->vert1.z > maxz )	maxz = pFace->vert1.z;
		if ( pFace->vert1.z < minz )	minz = pFace->vert1.z;

		if ( pFace->vert2.x > maxx )	maxx = pFace->vert2.x;
		if ( pFace->vert2.x < minx )	minx = pFace->vert2.x;
		if ( pFace->vert2.y > maxy )	maxy = pFace->vert2.y;
		if ( pFace->vert2.y < miny )	miny = pFace->vert2.y;
		if ( pFace->vert2.z > maxz )	maxz = pFace->vert2.z;
		if ( pFace->vert2.z < minz )	minz = pFace->vert2.z;

		if ( pFace->vert3.x > maxx )	maxx = pFace->vert3.x;
		if ( pFace->vert3.x < minx )	minx = pFace->vert3.x;
		if ( pFace->vert3.y > maxy )	maxy = pFace->vert3.y;
		if ( pFace->vert3.y < miny )	miny = pFace->vert3.y;
		if ( pFace->vert3.z > maxz )	maxz = pFace->vert3.z;
		if ( pFace->vert3.z < minz )	minz = pFace->vert3.z;

		pFace = pFace->GetNextFace();
    }

	centerX = (maxx + minx)/2.0f;
    centerY = (maxy + miny)/2.0f;
    centerZ = (maxz + minz)/2.0f;

	radius = 0;
    mainRadius = 0;
	pFace = pFaces;

	while ( pFace )
    {
		//main radius
		newRadius = ( pFace->vert1.x * pFace->vert1.x 
                    + pFace->vert1.y * pFace->vert1.y
                    + pFace->vert1.z * pFace->vert1.z );
        if (newRadius > mainRadius) mainRadius = newRadius;

		newRadius = ( pFace->vert2.x * pFace->vert2.x 
                    + pFace->vert2.y * pFace->vert2.y
                    + pFace->vert2.z * pFace->vert2.z );
        if (newRadius > mainRadius) mainRadius = newRadius;

		newRadius = ( pFace->vert3.x * pFace->vert3.x 
                    + pFace->vert3.y * pFace->vert3.y
                    + pFace->vert3.z * pFace->vert3.z );
        if (newRadius > mainRadius) mainRadius = newRadius;

		//shifted radius
		newRadius = ( (pFace->vert1.x-centerX) * (pFace->vert1.x-centerX)
                    + (pFace->vert1.y-centerY) * (pFace->vert1.y-centerY)
                    + (pFace->vert1.z-centerZ) * (pFace->vert1.z-centerZ) );
        if (newRadius > radius) radius = newRadius;

		newRadius = ( (pFace->vert2.x-centerX) * (pFace->vert2.x-centerX)
                    + (pFace->vert2.y-centerY) * (pFace->vert2.y-centerY)
                    + (pFace->vert2.z-centerZ) * (pFace->vert2.z-centerZ) );
        if (newRadius > radius) radius = newRadius;

		newRadius = ( (pFace->vert3.x-centerX) * (pFace->vert3.x-centerX)
                    + (pFace->vert3.y-centerY) * (pFace->vert3.y-centerY)
                    + (pFace->vert3.z-centerZ) * (pFace->vert3.z-centerZ) );
        if (newRadius > radius) radius = newRadius;

		pFace = pFace->GetNextFace();
    }

	mainRadius = (float) sqrt( mainRadius );
	radius = (float) sqrt( radius );
}

//check the ray starting at point @p along direction @v and store any intersection in @cRes
//@vn is @v normalized
//@vi is the inverse of @v      
bool CollisionSphere::intersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, CollisionResults* cRes)
{
    AGKVector p1(p->x - centerX, p->y - centerY, p->z - centerZ);
    AGKVector intersect;
    AGKVector normal;
    float dist1,dist2;
    
    //if ray is moving away from sphere center, no instection (assuming backface culling)
    dist2=(p1.x*vn->x) + (p1.y*vn->y) + (p1.z*vn->z);
    if (dist2>0) return false;

    dist1=((p1.x*p1.x) + (p1.y*p1.y) + (p1.z*p1.z)) - radius*radius;
    dist1=dist2*dist2 - dist1;

    if (dist1<0) return false;

	dist1 = -dist2 - (float) sqrt(dist1);
    
    if (dist1<0) return false;
    if (dist1>0 && (dist1*dist1)>((v->x*v->x)+(v->y*v->y)+(v->z*v->z))) return false;

    intersect.Set(p1.x + vn->x*dist1, p1.y + vn->y*dist1, p1.z + vn->z*dist1);
    normal.Set(intersect.x/radius, intersect.y/radius, intersect.z/radius);
    intersect.x += centerX;
	intersect.y += centerY;
	intersect.z += centerZ;
          
    cRes->addPoint(&intersect,&normal,dist1,0);   
     
    return true;
}

bool CollisionSphere::sphereIntersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, float rRadius, AGKVector* scale, CollisionResults* cRes)
{
    float oldRadius = radius;
    AGKVector intersect;
    AGKVector normal;
    float dist1,dist2;
    bool result = true;
    
    if (scale!=0)
    {    
        float oldCenterX = centerX;
        float oldCenterY = centerY;
        float oldCenterZ = centerZ;
        
        float largestScale = scale->x;
        if (scale->y > largestScale) largestScale = scale->y;
        if (scale->z > largestScale) largestScale = scale->z;
        
        radius = radius*largestScale + rRadius;
        centerX = centerX*scale->x;
        centerY = centerY*scale->y;
        centerZ = centerZ*scale->z;
        
        AGKVector p1(p->x - centerX, p->y - centerY, p->z - centerZ);      
        
        dist2=(p1.x*vn->x) + (p1.y*vn->y) + (p1.z*vn->z);
        if (dist2>0) result = false;
    
        dist1=((p1.x*p1.x) + (p1.y*p1.y) + (p1.z*p1.z)) - radius*radius;
        dist1=dist2*dist2 - dist1;
    
        if (dist1<0) result = false;
    
		dist1 = -dist2 - (float) sqrt(dist1);
        
        if (dist1>0 && (dist1*dist1)>((v->x*v->x)+(v->y*v->y)+(v->z*v->z))) result = false;
    
        intersect.Set(p1.x + vn->x*dist1, p1.y + vn->y*dist1, p1.z + vn->z*dist1);
        normal.Set(intersect.x/radius, intersect.y/radius, intersect.z/radius);        
        intersect.x += centerX;
		intersect.y += centerY;
		intersect.z += centerZ;
                     
        if (result) cRes->addPoint(&intersect,&normal,dist1,0); 
        
        radius = oldRadius;
        centerX = oldCenterX; centerY = oldCenterY; centerZ = oldCenterZ;
        
        return result;
    }
    else
    {
        radius = radius + rRadius;
        
        AGKVector p1(p->x - centerX, p->y - centerY, p->z - centerZ);
        
        dist2=(p1.x*vn->x) + (p1.y*vn->y) + (p1.z*vn->z);
        if (dist2>0) result = false;
    
        dist1=((p1.x*p1.x) + (p1.y*p1.y) + (p1.z*p1.z)) - radius*radius;
        dist1=dist2*dist2 - dist1;
    
        if (dist1<0) result = false;
    
		dist1 = -dist2 - (float) sqrt(dist1);
        
        if (dist1>0 && (dist1*dist1)>((v->x*v->x)+(v->y*v->y)+(v->z*v->z))) result = false;
        
        intersect.Set(p1.x + vn->x*dist1, p1.y + vn->y*dist1, p1.z + vn->z*dist1);
        normal.Set(intersect.x/radius, intersect.y/radius, intersect.z/radius);
        intersect.x += centerX;
		intersect.y += centerY;
		intersect.z += centerZ;
              
        if (result) cRes->addPoint(&intersect,&normal,dist1,0);   
        
        radius = oldRadius;
        
        return result;
    }        
}

//@scale1 is the greatest scale on the three axis for the remote sphere
//@scale2 is the greatest scale on the three axis for the local sphere
//@transform is used to transform from @colSphere to local space
//perform a distance check between the two maximum extent spheres compared with their radii
bool CollisionSphere::collidesSphere(CollisionSphere* colSphere, float scale1, float scale2, AGKMatrix4* transform)
{
    float largeRadius1 = colSphere->radius*scale1;
    float largeRadius2 = radius*scale2;
    
    AGKVector tCenter(colSphere->centerX,colSphere->centerY,colSphere->centerZ);
	tCenter.Mult( *transform );
    
    float sqrDist = ((tCenter.x - centerX)*(tCenter.x - centerX)
                    + (tCenter.y - centerY)*(tCenter.y - centerY)
                    + (tCenter.z - centerZ)*(tCenter.z - centerZ));
    
    if (sqrDist > (largeRadius1 + largeRadius2)*(largeRadius1 + largeRadius2)) return false;
    
    return true;
}

//translates the faces in @obj2Faces using @transform to local space
//and checks against the local sphere
bool CollisionSphere::collidesPoly(Face* obj2Faces, AGKVector* scale, float lScale, AGKMatrix4* transform)
{
    float sqrRadius = (radius*lScale); sqrRadius = sqrRadius*sqrRadius;
    float sCenterX = centerX*scale->x;
    float sCenterY = centerY*scale->y;
    float sCenterZ = centerZ*scale->z;
    int count = 0;
    AGKVector p1, p2, p3, intersect;
    AGKVector normal, temp;
    float dist1, dist2, distv;
    
    //start of poly object loop
    for(Face* thisFace=obj2Faces; thisFace!=0; thisFace = thisFace->nextFace)
    {           
        //if (!thisFace->collisionon) continue;
        
        count = 0;
        
        //transform the vertices
        p1 = *transform * thisFace->vert1;
        p2 = *transform * thisFace->vert2;
        p3 = *transform * thisFace->vert3; 
        
        p1.x = p1.x - sCenterX; p1.y = p1.y - sCenterY; p1.z = p1.z - sCenterZ;
        p2.x = p2.x - sCenterX; p2.y = p2.y - sCenterY; p2.z = p2.z - sCenterZ;
        p3.x = p3.x - sCenterX; p3.y = p3.y - sCenterY; p3.z = p3.z - sCenterZ;
        
        //count number of vertices inside the sphere
        if (p1.x*p1.x + p1.y*p1.y + p1.z*p1.z<sqrRadius) count++;
        if (p2.x*p2.x + p2.y*p2.y + p2.z*p2.z<sqrRadius) count++;
        if (p3.x*p3.x + p3.y*p3.y + p3.z*p3.z<sqrRadius) count++;
        
        //if all inside (3), collision, outside (0) requires more checks
        //anything else signals definite intersection
        //if (count==3) continue;
        if (count>0) return true;
        
        //check edges against the sphere
        temp = p2 - p1;
        dist2 = temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;
        if (dist2>0.00001) dist1 = (-p1.x*temp.x - p1.y*temp.y - p1.z*temp.z)/dist2;
        else dist1 = 0;
        if (dist1>0 && dist1<1)
        { 
            temp.x = p1.x + temp.x*dist1;
            temp.y = p1.y + temp.y*dist1;
            temp.z = p1.z + temp.z*dist1;
            if (temp.x*temp.x + temp.y*temp.y + temp.z*temp.z<sqrRadius) return true;
        }
        
        //edge 2
        temp = p3 - p2;
        dist2 = temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;
        if (dist2>0.00001) dist1 = (-p2.x*temp.x - p2.y*temp.y - p2.z*temp.z)/dist2;
        else dist1 = 0;
        if (dist1>0 && dist1<1)
        {
            temp.x = p2.x + temp.x*dist1;
            temp.y = p2.y + temp.y*dist1;
            temp.z = p2.z + temp.z*dist1;
            if (temp.x*temp.x + temp.y*temp.y + temp.z*temp.z<sqrRadius) return true;
        }
        
        //edge 3
        temp = p1 - p3;
        dist2 = temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;
        if (dist2>0.00001) dist1 = (-p3.x*temp.x - p3.y*temp.y - p3.z*temp.z)/dist2;
        else dist1 = 0;
        if (dist1>0 && dist1<1)
        {
            temp.x = p3.x + temp.x*dist1;
            temp.y = p3.y + temp.y*dist1;
            temp.z = p3.z + temp.z*dist1;
            if (temp.x*temp.x + temp.y*temp.y + temp.z*temp.z<sqrRadius) return true;
        }
        
        //remaining case - surface of triangle brushes the sphere, does it touch:
        
        //work out polygon normal
		temp = p2 - p1;
        normal.Cross( temp, p3 - p1 );
        
        //work out plane's distance from origin (sphere is centered on origin)
        dist1 = normal.x*p1.x + normal.y*p1.y + normal.z*p1.z;
        dist2 = normal.x*normal.x + normal.y*normal.y + normal.z*normal.z;
        if (dist2<0.00001) continue;
        dist2 = (dist1*dist1)/dist2;
        if (dist2>sqrRadius) continue;
        
        //find the point on the plane closest to the sphere
        if (dist1>-0.00001 || dist1<0.00001) 
        {
            intersect.Set(0,0,0);
        }
        else
        {
            dist2 = dist2/dist1;    
            intersect.Set(normal.x*dist2, normal.y*dist2, normal.z*dist2);
        }
        
        //is this point inside the triangle
        temp.Cross( normal, p2 - p1 );
        
        distv = temp.x*p1.x + temp.y*p1.y + temp.z*p1.z;
        
        distv = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - distv;
        if (distv<-0.00001) continue;
        
        //Second side
        temp.Cross( normal, p3 - p2 );
        
        distv = temp.x*p2.x + temp.y*p2.y + temp.z*p2.z;
        
        distv = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - distv;
        if (distv<-0.00001) continue;
        
        //Third side
        temp.Cross( normal, p1 - p3 );
        
        distv = temp.x*p3.x + temp.y*p3.y + temp.z*p3.z;
        
        distv = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - distv;
        if (distv<-0.00001) continue;   
        
        return true;           
    }
    
    return false;
}

/*
bool CollisionSphere::SaveObject( FILE *pFile )
{
	unsigned char iSphereVersion = 1;
	fwrite( &iSphereVersion, 1,1, pFile );

	fwrite( &mainRadius, 4,1, pFile );
	fwrite( &radius, 4,1, pFile );
	fwrite( &centerX, 4,1, pFile );
	fwrite( &centerY, 4,1, pFile );
	fwrite( &centerZ, 4,1, pFile );

	return true;
}

bool CollisionSphere::LoadObject( FILE *pFile )
{
	unsigned char iSphereVersion = 0;
	fread( &iSphereVersion, 1,1, pFile );

	if ( iSphereVersion == 1 )
	{
		fread( &mainRadius, 4,1, pFile );
		fread( &radius, 4,1, pFile );
		fread( &centerX, 4,1, pFile );
		fread( &centerY, 4,1, pFile );
		fread( &centerZ, 4,1, pFile );
	}

	return true;
}
*/