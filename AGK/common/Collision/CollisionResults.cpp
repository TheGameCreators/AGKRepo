#include "CollisionResults.h"

#include <cmath>

using namespace AGK;

CollisionResults::CollisionResults()
{
    CollisionResults(50);
}

CollisionResults::CollisionResults(int max)
{
    if (max<1) max = 1;
    if (max>500) max = 500;
    
    recordAll = false;
	scaleOn = false;
	changed = false;
    
    maxCols=max;
    finalIntersectX = new float[max];
    finalIntersectY = new float[max];
    finalIntersectZ = new float[max];
    finalNormalX = new float[max];
    finalNormalY = new float[max];
    finalNormalZ = new float[max];
    finalBounceX = new float[max];
    finalBounceY = new float[max];
    finalBounceZ = new float[max];
    finalSlideX = new float[max];
    finalSlideY = new float[max];
    finalSlideZ = new float[max];
    finalDistance = new float[max];
    finalObjHit = new int[max];
    finalGroupHit = new int[max];
    finalFaceHit = new int[max];

	finalDistance[0] = 1000000000;
    
    numOfCol=0;
    firstCol = 0;
}

CollisionResults::~CollisionResults()
{
    delete [] finalIntersectX;
    delete [] finalIntersectY;
    delete [] finalIntersectZ;
    delete [] finalNormalX;
    delete [] finalNormalY;
    delete [] finalNormalZ;
    delete [] finalBounceX;
    delete [] finalBounceY;
    delete [] finalBounceZ;
    delete [] finalSlideX;
    delete [] finalSlideY;
    delete [] finalSlideZ;
    delete [] finalDistance;
    delete [] finalObjHit; 
    delete [] finalGroupHit;
    delete [] finalFaceHit;   
}

void CollisionResults::reset(float dist) 
{
	finalDistance[0] = dist;
	recordAll = false;
	scaleOn = false;
	changed = false;
	numOfCol = 0;   
	firstCol = 0;
}

void CollisionResults::setScaleOn(float sx, float sy, float sz, AGKVector* p) 
{ 
	scaleOn = true; 
	scalex = sx; scaley = sy; scalez = sz;
	origx = p->x*sx; origy = p->y*sy; origz = p->z*sz;
}

bool CollisionResults::isValidDistance(float dist)
{
	if (scaleOn || recordAll) return true;
    
	return dist<finalDistance[0];
}

bool CollisionResults::isValidDistanceSqr(float dist)
{
	if (scaleOn || recordAll) return true;
    
	return dist<finalDistance[0]*finalDistance[0];
}

void CollisionResults::storePoints(int num, AGKVector* intersect, AGKVector* normal, AGKVector* slide)
{
	intersect->Set(finalIntersectX[0],finalIntersectY[0],finalIntersectZ[0]);
	normal->Set(finalNormalX[0],finalNormalY[0],finalNormalZ[0]);
	slide->Set(finalSlideX[0],finalSlideY[0],finalSlideZ[0]);
}

void CollisionResults::setMaxCollisions(int max)
{
    if (max<4) max=4;
    if (max>500) max=500;

    recordAll = false;
 
    maxCols=max;   
    delete [] finalIntersectX;
    delete [] finalIntersectY;
    delete [] finalIntersectZ;
    delete [] finalNormalX;
    delete [] finalNormalY;
    delete [] finalNormalZ;
    delete [] finalBounceX;
    delete [] finalBounceY;
    delete [] finalBounceZ;
    delete [] finalSlideX;
    delete [] finalSlideY;
    delete [] finalSlideZ;
    delete [] finalDistance;
    delete [] finalObjHit; 
    delete [] finalGroupHit;
    delete [] finalFaceHit;   
    
    finalIntersectX = new float[maxCols];
    finalIntersectY = new float[maxCols];
    finalIntersectZ = new float[maxCols];
    finalNormalX = new float[maxCols];
    finalNormalY = new float[maxCols];
    finalNormalZ = new float[maxCols];
    finalBounceX = new float[maxCols];
    finalBounceY = new float[maxCols];
    finalBounceZ = new float[maxCols];
    finalSlideX = new float[maxCols];
    finalSlideY = new float[maxCols];
    finalSlideZ = new float[maxCols];
    finalDistance = new float[maxCols];
    finalObjHit = new int[maxCols];
    finalGroupHit = new int[maxCols];
    finalFaceHit = new int[maxCols];

	finalDistance[0] = 1000000000;
    
    numOfCol = 0;
    firstCol = 0;
}

//normalize any stored normals (only one in typical case) and shift the intersect point
//slightly off the polygon to account for inaccuracies
void CollisionResults::completeResults( const AGKVector* p, const AGKVector* v1, bool bShift)
{
    if (recordAll)
    {
        for (int i=0; i<numOfCol; i++)
        {
			float dist1 = sqrt(finalNormalX[i]*finalNormalX[i] + finalNormalY[i]*finalNormalY[i] + finalNormalZ[i]*finalNormalZ[i]);
            
            finalNormalX[i] = finalNormalX[i]/dist1; 
            finalNormalY[i] = finalNormalY[i]/dist1; 
            finalNormalZ[i] = finalNormalZ[i]/dist1; 

			finalIntersectX[i] += finalNormalX[i]*(abs(finalIntersectX[i])*0.00001f);
			finalIntersectY[i] += finalNormalY[i]*(abs(finalIntersectY[i])*0.00001f);
			finalIntersectZ[i] += finalNormalZ[i]*(abs(finalIntersectZ[i])*0.00001f);
            
            finalIntersectX[i] += finalNormalX[i]*0.0001f;
            finalIntersectY[i] += finalNormalY[i]*0.0001f;
            finalIntersectZ[i] += finalNormalZ[i]*0.0001f;
            
            dist1 = (v1->x*finalNormalX[i] + v1->y*finalNormalY[i] + v1->z*finalNormalZ[i])*2.0f;
            
            finalBounceX[i] =(v1->x - finalNormalX[i]*dist1);
            finalBounceY[i] =(v1->y - finalNormalY[i]*dist1);
            finalBounceZ[i] =(v1->z - finalNormalZ[i]*dist1);
            
            dist1 = (finalIntersectX[i]*finalNormalX[i] + finalIntersectY[i]*finalNormalY[i] + finalIntersectZ[i]*finalNormalZ[i]);
            dist1 = p->x*finalNormalX[i] + p->y*finalNormalY[i] + p->z*finalNormalZ[i] - dist1;
            dist1 -= 0.0001f;
            
            if (dist1<0)
            {
                finalSlideX[i] = p->x - finalNormalX[i]*dist1;
                finalSlideY[i] = p->y - finalNormalY[i]*dist1;
                finalSlideZ[i] = p->z - finalNormalZ[i]*dist1;
            }
            else
            {
                finalSlideX[i] = p->x; finalSlideY[i] = p->y; finalSlideZ[i] = p->z;
            }
        }
        
        sort();
    }
    else
    {
        numOfCol=1;
        
		float dist1 = sqrt(finalNormalX[0]*finalNormalX[0] + finalNormalY[0]*finalNormalY[0] + finalNormalZ[0]*finalNormalZ[0]);
        
        finalNormalX[0] = finalNormalX[0]/dist1; 
        finalNormalY[0] = finalNormalY[0]/dist1; 
        finalNormalZ[0] = finalNormalZ[0]/dist1; 

		float px = p->x;
		float py = p->y;
		float pz = p->z;
        
		if ( bShift )
		{
			//push point off collision plane by small amount (scaled by float size as real small amounts won't affect large floats
			finalIntersectX[0] += finalNormalX[0]*(abs(finalIntersectX[0])*0.00001f);
			finalIntersectY[0] += finalNormalY[0]*(abs(finalIntersectY[0])*0.00001f);
			finalIntersectZ[0] += finalNormalZ[0]*(abs(finalIntersectZ[0])*0.00001f);

			//push by a small amount anyway incase finalIntersect is close to 0, and above didn't move it anywhere
			finalIntersectX[0] += finalNormalX[0]*0.0001f;
			finalIntersectY[0] += finalNormalY[0]*0.0001f;
			finalIntersectZ[0] += finalNormalZ[0]*0.0001f;

			px = p->x + finalNormalX[0]*(abs(finalIntersectX[0])*0.00001f);
			py = p->y + finalNormalY[0]*(abs(finalIntersectY[0])*0.00001f);
			pz = p->z + finalNormalZ[0]*(abs(finalIntersectZ[0])*0.00001f);

			px = p->x + finalNormalX[0]*0.0001f;
			py = p->y + finalNormalY[0]*0.0001f;
			pz = p->z + finalNormalZ[0]*0.0001f;
		}
        
        dist1 = (v1->x*finalNormalX[0] + v1->y*finalNormalY[0] + v1->z*finalNormalZ[0])*2.0f;
        
        finalBounceX[0] = (v1->x - finalNormalX[0]*dist1);
        finalBounceY[0] = (v1->y - finalNormalY[0]*dist1);
        finalBounceZ[0] = (v1->z - finalNormalZ[0]*dist1);
        
        dist1 = (finalIntersectX[0]*finalNormalX[0] + finalIntersectY[0]*finalNormalY[0] + finalIntersectZ[0]*finalNormalZ[0]);
        dist1 = dist1 - (px*finalNormalX[0] + py*finalNormalY[0] + pz*finalNormalZ[0]);
        //dist1 -= 0.0001f;
        
        if (dist1>0)
        {
            finalSlideX[0] = px + finalNormalX[0]*dist1;
            finalSlideY[0] = py + finalNormalY[0]*dist1;
            finalSlideZ[0] = pz + finalNormalZ[0]*dist1;
        }
        else
        {
            finalSlideX[0] = p->x; finalSlideY[0] = p->y; finalSlideZ[0] = p->z;
        }
    }
}

void CollisionResults::rotatePoints( const AGKQuaternion* rot, int obj, int group, float x, float y, float z)
{
    if (!changed) return;
    changed = false;
    
    if (recordAll)
    {
        for (int i=firstCol; i<numOfCol; i++)
        {
            AGKVector a(finalIntersectX[i],finalIntersectY[i],finalIntersectZ[i]);
			a.Mult( *rot );
            finalIntersectX[i] = a.x + x; 
            finalIntersectY[i] = a.y + y; 
            finalIntersectZ[i] = a.z + z;
            
            a.Set(finalNormalX[i],finalNormalY[i],finalNormalZ[i]);
			a.Mult( *rot );
            finalNormalX[i] = a.x; finalNormalY[i] = a.y; finalNormalZ[i] = a.z;  
            
            finalObjHit[i] = obj;
            finalGroupHit[i] = group;
        }
        
        firstCol = numOfCol;
    }
    else
    {
        AGKVector a(finalIntersectX[0],finalIntersectY[0],finalIntersectZ[0]);
		a.Mult( *rot );
        finalIntersectX[0] = a.x + x; 
        finalIntersectY[0] = a.y + y; 
        finalIntersectZ[0] = a.z + z;
        
		a.Set(finalNormalX[0],finalNormalY[0],finalNormalZ[0]);
		a.Mult( *rot );
        finalNormalX[0] = a.x; finalNormalY[0] = a.y; finalNormalZ[0] = a.z;  
        
        finalObjHit[0] = obj;
        finalGroupHit[0] = group;
    }
}

//normal is assumed not normalized, intersect point will be adjusted 'off' the polygon later
void CollisionResults::addPoint(AGKVector* intersect, AGKVector* normal, float distance, int id)
{
    float intersectx = intersect->x;
    float intersecty = intersect->y;
    float intersectz = intersect->z;
    float normalx = normal->x;
    float normaly = normal->y;
    float normalz = normal->z;
    
    if (recordAll)
    {
        if (numOfCol>=maxCols) return;
        
        if (scaleOn)
        {
            intersectx *= scalex; intersecty *= scaley; intersectz *= scalez;
			distance = sqrt((intersectx-origx)*(intersectx-origx) + (intersecty-origy)*(intersecty-origy) + (intersectz-origz)*(intersectz-origz));
            
            normalx /= scalex; normaly /= scaley; normalz /= scalez;
        }
        
        finalIntersectX[numOfCol] = intersectx;
        finalIntersectY[numOfCol] = intersecty;
        finalIntersectZ[numOfCol] = intersectz;
        finalNormalX[numOfCol] = normalx;
        finalNormalY[numOfCol] = normaly;
        finalNormalZ[numOfCol] = normalz;
        finalDistance[numOfCol] = distance;
        finalFaceHit[numOfCol] = id;
        
        if (!changed) firstCol = numOfCol;
        changed = true;
        numOfCol++;
    }
    else
    {
        if (scaleOn)
        {
            intersectx *= scalex; intersecty *= scaley; intersectz *= scalez;
            float dist = (intersectx-origx)*(intersectx-origx) + (intersecty-origy)*(intersecty-origy) + (intersectz-origz)*(intersectz-origz);            
            
            if (dist<(finalDistance[0]*finalDistance[0])) 
            {
				dist = sqrt(dist);
                normalx /= scalex; normaly /= scaley; normalz /= scalez;
                finalIntersectX[0] = intersectx;
                finalIntersectY[0] = intersecty;
                finalIntersectZ[0] = intersectz;
                finalNormalX[0] = normalx;
                finalNormalY[0] = normaly;
                finalNormalZ[0] = normalz;
                finalDistance[0] = dist;
                finalFaceHit[0] = id;
                changed = true;
            }
        }
        else
        {
            if (distance<finalDistance[0])
            {
                finalIntersectX[0] = intersectx;
                finalIntersectY[0] = intersecty;
                finalIntersectZ[0] = intersectz;
                finalNormalX[0] = normalx;
                finalNormalY[0] = normaly;
                finalNormalZ[0] = normalz;
                finalDistance[0] = distance;
                finalFaceHit[0] = id;
                changed = true;
            }
        }    
    }
}

void CollisionResults::copyElements(int i,int j)
{
    finalIntersectX[j]=finalIntersectX[i];
    finalIntersectY[j]=finalIntersectY[i];
    finalIntersectZ[j]=finalIntersectZ[i];
    finalNormalX[j]=finalNormalX[i];
    finalNormalY[j]=finalNormalY[i];
    finalNormalZ[j]=finalNormalZ[i];
    finalBounceX[j]=finalBounceX[i];
    finalBounceY[j]=finalBounceY[i];
    finalBounceZ[j]=finalBounceZ[i];
    finalSlideX[j]=finalSlideX[i];
    finalSlideY[j]=finalSlideY[i];
    finalSlideZ[j]=finalSlideZ[i];
    finalDistance[j]=finalDistance[i];
    finalObjHit[j]=finalObjHit[i];
    finalGroupHit[j]=finalGroupHit[i];
    finalFaceHit[j]=finalFaceHit[i];
}

void CollisionResults::swapElements(int i,int j)
{
    float temp1=finalIntersectX[i];
    float temp2=finalIntersectY[i];
    float temp3=finalIntersectZ[i];
    float temp4=finalNormalX[i];
    float temp5=finalNormalY[i];
    float temp6=finalNormalZ[i];
    float temp7=finalBounceX[i];
    float temp8=finalBounceY[i];
    float temp9=finalBounceZ[i];
    float temp10=finalSlideX[i];
    float temp11=finalSlideY[i];
    float temp12=finalSlideZ[i];
    float temp13=finalDistance[i];
    int temp14=finalObjHit[i];
    int temp15=finalGroupHit[i];
    int temp16=finalFaceHit[i];
    
    finalIntersectX[i]=finalIntersectX[j];
    finalIntersectY[i]=finalIntersectY[j];
    finalIntersectZ[i]=finalIntersectZ[j];
    finalNormalX[i]=finalNormalX[j];
    finalNormalY[i]=finalNormalY[j];
    finalNormalZ[i]=finalNormalZ[j];
    finalBounceX[i]=finalBounceX[j];
    finalBounceY[i]=finalBounceY[j];
    finalBounceZ[i]=finalBounceZ[j];
    finalSlideX[i]=finalSlideX[j];
    finalSlideY[i]=finalSlideY[j];
    finalSlideZ[i]=finalSlideZ[j];
    finalDistance[i]=finalDistance[j];
    finalObjHit[i]=finalObjHit[j];
    finalGroupHit[i]=finalGroupHit[j];
    finalFaceHit[i]=finalFaceHit[j];
    
    finalIntersectX[j]=temp1;
    finalIntersectY[j]=temp2;
    finalIntersectZ[j]=temp3;
    finalNormalX[j]=temp4;
    finalNormalY[j]=temp5;
    finalNormalZ[j]=temp6;
    finalBounceX[j]=temp7;
    finalBounceY[j]=temp8;
    finalBounceZ[j]=temp9;
    finalSlideX[j]=temp10;
    finalSlideY[j]=temp11;
    finalSlideZ[j]=temp12;
    finalDistance[j]=temp13;
    finalObjHit[j]=temp14;
    finalGroupHit[j]=temp15;
    finalFaceHit[j]=temp16;
}

void CollisionResults::sort()
{
    float min;
    int index;
    
	// simple bubble sort
    for(int i=0; i<numOfCol-1; i++)
    {
        index = i;
        min = finalDistance[i];
        
        for(int j=i+1; j<numOfCol; j++)
        {
            if (finalDistance[j]<min) { min = finalDistance[j]; index = j; }
        }
        
        if (index!=i) swapElements(i,index);
    }
}
