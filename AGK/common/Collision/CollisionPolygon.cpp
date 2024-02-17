#include "CollisionPolygon.h"

#include <math.h>

using namespace AGK;

inline float max3(float x1, float x2, float x3)
{
    float result = x1;
    if (x2>result) result=x2;
    if (x3>result) result=x3;
    
    return result;
}

inline float min3(float x1, float x2, float x3)
{
    float result = x1;
    if (x2<result) result=x2;
    if (x3<result) result=x3;
    
    return result;
}

CollisionPolygon::~CollisionPolygon()
{
    Face* temp;
    
    while(faces!=0)
    {
        temp = faces;
        faces = faces->GetNextFace();
        delete temp;
    }                                     
}

void CollisionPolygon::makeCollisionObject(Face* pFaces)
{
	faces = pFaces;
	mainRadius = 0;
	float newRadius = 0;

	float maxx = -1000000000;
    float maxy = -1000000000;
    float maxz = -1000000000;
    float minx = 1000000000;
    float miny = 1000000000;
    float minz = 1000000000;

	Face *pFace = pFaces;

	while ( pFace )
    {
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

	mainRadius = (float) sqrt( mainRadius );

	bounds.set(minx,miny,minz,maxx,maxy,maxz);
    bounds.correctBox();
}

//check the ray starting at point @p along direction @v and store any intersection in @cRes
//@vn is @v normalized
//@vi is the inverse of @v        
bool CollisionPolygon::intersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, CollisionResults* cRes)
{
     float dist1;
     //quick reject
     if (bounds.intersectBox(p,vi,&dist1)==0) return false;
     
     bool result = false;
     
     //cycle through all faces
     for (Face* thisFace=faces; thisFace!=0; thisFace = thisFace->nextFace)
         result = result | thisFace->intersects(p,v,vn,cRes);   
         
     return result;
}

bool CollisionPolygon::sphereIntersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, float rRadius, AGKVector* scale, CollisionResults* cRes)
{
     bool result = false;
     
     //cycle through all faces
     for (Face* thisFace=faces; thisFace!=0; thisFace = thisFace->nextFace)
         result = result | thisFace->sphereIntersects(p,v,vn,rRadius,scale,cRes);   
         
     return result;
}

//Polygon-Polygon Collision
//check all faces in @obj2Faces against the local faces
//transform remote faces to local space using @transform
bool CollisionPolygon::collidesPoly(Face* obj2Faces, AGKMatrix4* transform)
{
    AGKVector p1, p2, p3, max, min;
    
    //start of remote face loop
    for(Face* thisFace=obj2Faces; thisFace!=0; thisFace=thisFace->nextFace)
    {
        //if (!thisFace->collisionon) continue;
        //translate the vertices
        p1 = *transform * thisFace->vert1;
        p2 = *transform * thisFace->vert2;
        p3 = *transform * thisFace->vert3;
        
        //build a bounding box around new face
        max.x = p1.x; max.y = p1.y; max.z = p1.z;
        min.x = p1.x; min.y = p1.y; min.z = p1.z;
        
        //X axis
        if (p2.x>p1.x) {
            if (p3.x>p2.x) max.x = p3.x;
            else {
                max.x = p2.x;
                if (p1.x>p3.x) min.x = p3.x;
            }
        }
        else {
            if (p3.x<p2.x) min.x = p3.x;
            else {
                min.x = p2.x;
                if (p1.x<p3.x) max.x = p3.x;
            }
        }
        
        //Y axis
        if (p2.y>p1.y) {
            if (p3.y>p2.y) max.y = p3.y;
            else {
                max.y = p2.y;
                if (p1.y>p3.y) min.y = p3.y;
            }
        }
        else {
            if (p3.y<p2.y) min.y = p3.y;
            else {
                min.y = p2.y;
                if (p1.y<p3.y) max.y = p3.y;
            }
        }
        
        //Z axis
        if (p2.z>p1.z) {
            if (p3.z>p2.z) max.z = p3.z;
            else {
                max.z = p2.z;
                if (p1.z>p3.z) min.z = p3.z;
            }
        }
        else {
            if (p3.z<p2.z) min.z = p3.z;
            else {
                min.z = p2.z;
                if (p1.z<p3.z) max.z = p3.z;
            }
        }
    
        //start of local face loop
        for(Face* thisFace2=faces; thisFace2!=0; thisFace2=thisFace2->nextFace)
        {
            //if (!thisFace2->collisionon) continue;
            if (thisFace2->collidesPoly(&p1,&p2,&p3)) return true;            
        }
    }
    
    return false;
}

/*
bool CollisionPolygon::SaveObject( FILE *pFile )
{
	unsigned char iPolyVersion = 1;
	fwrite( &iPolyVersion, 1,1, pFile );

	fwrite( &mainRadius, 4,1, pFile );

	float value = bounds.minbx(); fwrite( &value, 4,1, pFile );
	value = bounds.minby(); fwrite( &value, 4,1, pFile );
	value = bounds.minbz(); fwrite( &value, 4,1, pFile );
	value = bounds.maxbx(); fwrite( &value, 4,1, pFile );
	value = bounds.maxby(); fwrite( &value, 4,1, pFile );
	value = bounds.maxbz(); fwrite( &value, 4,1, pFile );

	int count = 0;
	Face *pFace = faces;
	while ( pFace )
	{
		count++;
		pFace = pFace->GetNextFace();
	}

	fwrite( &count, 4,1, pFile );
	
	pFace = faces;
	while ( pFace )
	{
		fwrite( &(pFace->vert1.x), 4,1, pFile );
		fwrite( &(pFace->vert1.y), 4,1, pFile );
		fwrite( &(pFace->vert1.z), 4,1, pFile );

		fwrite( &(pFace->vert2.x), 4,1, pFile );
		fwrite( &(pFace->vert2.y), 4,1, pFile );
		fwrite( &(pFace->vert2.z), 4,1, pFile );

		fwrite( &(pFace->vert3.x), 4,1, pFile );
		fwrite( &(pFace->vert3.y), 4,1, pFile );
		fwrite( &(pFace->vert3.z), 4,1, pFile );

		fwrite( &(pFace->normal.x), 4,1, pFile );
		fwrite( &(pFace->normal.y), 4,1, pFile );
		fwrite( &(pFace->normal.z), 4,1, pFile );

		fwrite( &(pFace->d), 4,1, pFile );

		unsigned char cColOn = pFace->collisionon ? 1 : 0;
		fwrite( &cColOn, 1,1, pFile );

		fwrite( &(pFace->faceId), sizeof(unsigned),1, pFile );

		pFace = pFace->GetNextFace();
	}

	return true;
}

bool CollisionPolygon::LoadObject( FILE *pFile )
{
	unsigned char iPolyVersion = 0;
	fread( &iPolyVersion, 1,1, pFile );

	if ( iPolyVersion == 1 )
	{
		fread( &mainRadius, 4,1, pFile );
		
		float minx, miny, minz, maxx, maxy, maxz;
		fread( &minx, 4,1, pFile );
		fread( &miny, 4,1, pFile );
		fread( &minz, 4,1, pFile );
		fread( &maxx, 4,1, pFile );
		fread( &maxy, 4,1, pFile );
		fread( &maxz, 4,1, pFile );
		bounds.set( minx, miny, minz, maxx, maxy, maxz );

		Face *pLast = 0, *pCurr = 0;
		int count = 0;
		fread( &count, 4,1, pFile );	//number of faces
		faces = 0;

		for ( int i = 0; i < count; i++ )
		{
			pCurr = new Face();
			pCurr->nextFace = 0;

			//tail add, to preverse linked list order
			if ( pLast ) pLast->nextFace = pCurr;
			else faces = pCurr;

			fread( &(pCurr->vert1.x), 4,1, pFile );
			fread( &(pCurr->vert1.y), 4,1, pFile );
			fread( &(pCurr->vert1.z), 4,1, pFile );

			fread( &(pCurr->vert2.x), 4,1, pFile );
			fread( &(pCurr->vert2.y), 4,1, pFile );
			fread( &(pCurr->vert2.z), 4,1, pFile );

			fread( &(pCurr->vert3.x), 4,1, pFile );
			fread( &(pCurr->vert3.y), 4,1, pFile );
			fread( &(pCurr->vert3.z), 4,1, pFile );

			fread( &(pCurr->normal.x), 4,1, pFile );
			fread( &(pCurr->normal.y), 4,1, pFile );
			fread( &(pCurr->normal.z), 4,1, pFile );

			fread( &(pCurr->d), 4,1, pFile );

			unsigned char cColOn = 0;
			fread( &cColOn, 1,1, pFile );
			pCurr->collisionon = cColOn > 0;

			fread( &pCurr->faceId, sizeof(unsigned),1, pFile );

			pLast = pCurr;
			pCurr = 0;
		}
	}

	return true;
}
*/