#include "Box.h"  

using namespace AGK;

Box::Box(AGKVector* min, AGKVector* max)
{
	minx = min->x; miny = min->y; minz = min->z;
	maxx = max->x; maxy = max->y; maxz = max->z;
	correctBox();
}

Box::Box(float x,float y,float z,float x2,float y2,float z2)
{
	minx = x; miny = y; minz = z;
	maxx = x2; maxy = y2; maxz = z2;
	correctBox();
}

void Box::resetBox()
{
	minx = 1000000000; miny = 1000000000; minz = 1000000000;
	maxx = -1000000000; maxy = -1000000000; maxz = -1000000000;
}

void Box::set(AGKVector* min, AGKVector* max)
{
	minx = min->x; miny = min->y; minz = min->z;
	maxx = max->x; maxy = max->y; maxz = max->z;
}            

void Box::set(float x,float y,float z,float x2,float y2,float z2)
{
	minx = x; miny = y; minz = z;
	maxx = x2; maxy = y2; maxz = z2;
}

void Box::copy( Box *pOther )
{
	minx = pOther->minx;
	miny = pOther->miny;
	minz = pOther->minz;
	maxx = pOther->maxx;
	maxy = pOther->maxy;
	maxz = pOther->maxz;
}

void Box::correctBox() 
{ 
	minx=minx-0.0001f; miny=miny-0.0001f; minz=minz-0.0001f; 
	maxx=maxx+0.0001f; maxy=maxy+0.0001f; maxz=maxz+0.0001f;
}

bool Box::inBox(AGKVector* p) { 
	if (p->x<minx || p->x>maxx || p->y<miny || p->y>maxy || p->z<minz || p->z>maxz) return false;         
	return true;
}
        
int Box::intersectBox(AGKVector* p, AGKVector* vi, float* result)
{        
    float tminx,tmaxx,tminy,tmaxy;
    int side=0;           

    if (vi->x>=0) {
        tminx = (minx - p->x) *vi->x;
        tmaxx = (maxx - p->x) *vi->x;
        side=1;
    }
    else {
        tmaxx = (minx - p->x) *vi->x;
        tminx = (maxx - p->x) *vi->x;
        side=2;
    }
    
    if (vi->y>=0) {
        tminy = (miny - p->y) *vi->y;
        tmaxy = (maxy - p->y) *vi->y;
        if (tminy>tminx) side=3;
    }
    else {
        tmaxy = (miny - p->y) *vi->y;
        tminy = (maxy - p->y) *vi->y;
        if (tminy>tminx) side=4;
    }
               
    if ((tminx > tmaxy) || (tminy > tmaxx)) return 0;
    
    if (tminy>tminx) tminx = tminy;
    if (tmaxy<tmaxx) tmaxx = tmaxy;
    
    if (vi->z>=0) {
        tminy = (minz - p->z) *vi->z;
        tmaxy = (maxz - p->z) *vi->z;
        if (tminy>tminx) side=5;
    }
    else {
        tmaxy = (minz - p->z) *vi->z;
        tminy = (maxz - p->z) *vi->z;
        if (tminy>tminx) side=6;
    }
    
    if ((tminx > tmaxy) || (tminy > tmaxx)) return 0;
                                      
    if (tminy>tminx) tminx = tminy;
    if (tmaxy<tmaxx) tmaxx = tmaxy;
    
    if (tmaxx<0.0) return 0;
    if (tminx>1.0) return 0;
    
    *result=tminx;
    return side;
}

//a quick check to try and detect when a sphere ray is too far from a box to bother checking
//returns true if sphere ray may intersect box, with a lower bound distance in @dist
bool Box::quickSphereIntersectBox(AGKVector* p, AGKVector* vi, float rRadius, AGKVector* scale, float* dist)
{
    //pad out the box with the sphere size then check a normal ray
    
    float lminx,lminy,lminz,lmaxx,lmaxy,lmaxz;
    
    if (scale!=0)
    {
        lminx = minx * scale->x - rRadius;
        lminy = miny * scale->y - rRadius;
        lminz = minz * scale->z - rRadius;
        lmaxx = maxx * scale->x + rRadius;
        lmaxy = maxy * scale->y + rRadius;
        lmaxz = maxz * scale->z + rRadius;
    }
    else
    {
        lminx = minx - rRadius;
        lminy = miny - rRadius;
        lminz = minz - rRadius;
        lmaxx = maxx + rRadius;
        lmaxy = maxy + rRadius;
        lmaxz = maxz + rRadius;
    }
    
    float tminx,tmaxx,tminy,tmaxy;

    if (vi->x>=0) {
        tminx = (lminx - p->x) *vi->x;
        tmaxx = (lmaxx - p->x) *vi->x;
    }
    else {
        tmaxx = (lminx - p->x) *vi->x;
        tminx = (lmaxx - p->x) *vi->x;
    }
    
    if (vi->y>=0) {
        tminy = (lminy - p->y) *vi->y;
        tmaxy = (lmaxy - p->y) *vi->y;
    }
    else {
        tmaxy = (lminy - p->y) *vi->y;
        tminy = (lmaxy - p->y) *vi->y;
    }
               
    if ((tminx > tmaxy) || (tminy > tmaxx)) return false;
    
    if (tminy>tminx) tminx = tminy;
    if (tmaxy<tmaxx) tmaxx = tmaxy;
    
    if (vi->z>=0) {
        tminy = (lminz - p->z) *vi->z;
        tmaxy = (lmaxz - p->z) *vi->z;
    }
    else {
        tmaxy = (lminz - p->z) *vi->z;
        tminy = (lmaxz - p->z) *vi->z;
    }
    
    if ((tminx > tmaxy) || (tminy > tmaxx)) return false;
                                      
    if (tminy>tminx) tminx = tminy;
    if (tmaxy<tmaxx) tmaxx = tmaxy;
    
    if (tmaxx<0.0) return false;
    if (tminx>1.0) return false;
    
    *dist = tminx;
    return true;
}

int Box::collidesBox(Box* b, AGKVector* scale, AGKMatrix4* transform)
{
    float lminx = minx * scale->x;
    float lminy = miny * scale->y;
    float lminz = minz * scale->z;
    float lmaxx = maxx * scale->x;
    float lmaxy = maxy * scale->y;
    float lmaxz = maxz * scale->z;
    
    //first calculate the 3 axis points of @b from its bounds, and also its min point
    AGKVector axis0(b->maxbx(),b->minby(),b->minbz());
    AGKVector axis1(b->minbx(),b->maxby(),b->minbz());
    AGKVector axis2(b->minbx(),b->minby(),b->maxbz());
    AGKVector minp(b->minbx(),b->minby(),b->minbz());
    
    //translate these into local space
	axis0.Mult( *transform );
    axis1.Mult( *transform );
    axis2.Mult( *transform );
    minp.Mult( *transform );
    
    axis0.Set(axis0.x - minp.x, axis0.y - minp.y, axis0.z - minp.z);
    axis1.Set(axis1.x - minp.x, axis1.y - minp.y, axis1.z - minp.z);
    axis2.Set(axis2.x - minp.x, axis2.y - minp.y, axis2.z - minp.z);
 
    AGKVector rmin(minp.x,minp.y,minp.z);
    AGKVector rmax(minp.x,minp.y,minp.z);
    
    //find the extent of the new box (min and max points) in local space rmin, rmax
    if (axis0.x>0) rmax.x += axis0.x; else rmin.x += axis0.x;    
    if (axis1.x>0) rmax.x += axis1.x; else rmin.x += axis1.x;
    if (axis2.x>0) rmax.x += axis2.x; else rmin.x += axis2.x;
    
    //Second axis
    if (axis0.y>0) rmax.y += axis0.y; else rmin.y += axis0.y;
    if (axis1.y>0) rmax.y += axis1.y; else rmin.y += axis1.y;
    if (axis2.y>0) rmax.y += axis2.y; else rmin.y += axis2.y;
        
    //Third axis
    if (axis0.z>0) rmax.z += axis0.z; else rmin.z += axis0.z;
    if (axis1.z>0) rmax.z += axis1.z; else rmin.z += axis1.z;
    if (axis2.z>0) rmax.z += axis2.z; else rmin.z += axis2.z;
    
    //if this box is completely inside the local box, collision
    if (rmax.x<lmaxx && rmax.y<lmaxy && rmax.z<lmaxz &&
        rmin.x>lminx && rmin.y>lminy && rmin.z>lminz) return 1;
    
    //if this box is completely on one side of the local box, no intersection
    if (rmax.x<lminx) return 0;
    if (rmax.y<lminy) return 0;
    if (rmax.z<lminz) return 0;
    
    if (rmin.x>lmaxx) return 0;
    if (rmin.y>lmaxy) return 0;
    if (rmin.z>lmaxz) return 0;
    
    //get the vector between the centers of the two boxes
    float cx = (minp.x+(minp.x+axis0.x+axis1.x+axis2.x))/2.0f;
    float cy = (minp.y+(minp.y+axis0.y+axis1.y+axis2.y))/2.0f;
    float cz = (minp.z+(minp.z+axis0.z+axis1.z+axis2.z))/2.0f;
    
    cx = cx - (lminx+lmaxx)/2.0f;
    cy = cy - (lminy+lmaxy)/2.0f;
    cz = cz - (lminz+lmaxz)/2.0f;
    
    //find the closest corner in box 2 to box 1, based on the center vector
    rmax.x = minp.x;
    rmax.y = minp.y;
    rmax.z = minp.z;
    
    //if an axis if pointed away from the center vector, move t[0] along it
    //hence moving closer to the other box
    if (axis0.x*cx + axis0.y*cy + axis0.z*cz <0) {
        rmax.x += axis0.x;
        rmax.y += axis0.y;
        rmax.z += axis0.z;
        //invert this axis to keep it pointed away from min point
        axis0.x = -axis0.x;
        axis0.y = -axis0.y;
        axis0.z = -axis0.z;
    }
    
    if (axis1.x*cx + axis1.y*cy + axis1.z*cz <0) {
        rmax.x += axis1.x;
        rmax.y += axis1.y;
        rmax.z += axis1.z;
        axis1.x = -axis1.x;
        axis1.y = -axis1.y;
        axis1.z = -axis1.z;
    }
    
    if (axis2.x*cx + axis2.y*cy + axis2.z*cz <0) {
        rmax.x += axis2.x;
        rmax.y += axis2.y;
        rmax.z += axis2.z;
        axis2.x = -axis2.x;
        axis2.y = -axis2.y;
        axis2.z = -axis2.z;
    }
    
    //rmax should now hold the closest corner in the remote box to the local box
    
    //if this point lies inside the local box then intersection occurs
    if (rmax.x<lmaxx && rmax.x>lminx 
     && rmax.y<lmaxy && rmax.y>lminy
     && rmax.z<lmaxz && rmax.z>lminz) return 1; 
    
    //build a temp box using the scaled co-ords
    Box tempBox(lminx,lminy,lminz,lmaxx,lmaxy,lmaxz);
    
    //test the 3 edges connected to this corner for intersection with the local box
    //AGKVector p(tx[0],ty[0],tz[0]);
    AGKVector v(1/axis0.x,1/axis0.y,1/axis0.z);
    float result;
    
    if (tempBox.intersectBox(&rmax,&v,&result)) return 1;
    v.Set(1/axis1.x,1/axis1.y,1/axis1.z);
    if (tempBox.intersectBox(&rmax,&v,&result)) return 1;
    v.Set(1/axis2.x,1/axis2.y,1/axis2.z);
    if (tempBox.intersectBox(&rmax,&v,&result)) return 1;
    
    //possible collision, further checks required (check boxes in opposite order)
    return 2;
}

//returns @b is
//        0 - outside
//        1 - inside
//        2 - intersect/don't know
//the local box
int Box::roughBox(Box* b, AGKVector* scale, AGKMatrix4* transform)
{
    float lminx = minx * scale->x;
    float lminy = miny * scale->y;
    float lminz = minz * scale->z;
    float lmaxx = maxx * scale->x;
    float lmaxy = maxy * scale->y;
    float lmaxz = maxz * scale->z;
    
    //first calculate the 3 axis points of @b from its bounds, and also its min point
    AGKVector axis0(b->maxbx(),b->minby(),b->minbz());
    AGKVector axis1(b->minbx(),b->maxby(),b->minbz());
    AGKVector axis2(b->minbx(),b->minby(),b->maxbz());
    AGKVector minp(b->minbx(),b->minby(),b->minbz());
    
    //translate these into local space
	axis0.Mult( *transform );
    axis1.Mult( *transform );
    axis2.Mult( *transform );
    minp.Mult( *transform );
    
    axis0.Set(axis0.x - minp.x, axis0.y - minp.y, axis0.z - minp.z);
    axis1.Set(axis1.x - minp.x, axis1.y - minp.y, axis1.z - minp.z);
    axis2.Set(axis2.x - minp.x, axis2.y - minp.y, axis2.z - minp.z);
 
    AGKVector rmin(minp.x,minp.y,minp.z);
    AGKVector rmax(minp.x,minp.y,minp.z);
    
    //find the extent of the new box (min and max points) in local space rmin, rmax
    if (axis0.x>0) rmax.x += axis0.x; else rmin.x += axis0.x;    
    if (axis1.x>0) rmax.x += axis1.x; else rmin.x += axis1.x;
    if (axis2.x>0) rmax.x += axis2.x; else rmin.x += axis2.x;
    
    //Second axis
    if (axis0.y>0) rmax.y += axis0.y; else rmin.y += axis0.y;
    if (axis1.y>0) rmax.y += axis1.y; else rmin.y += axis1.y;
    if (axis2.y>0) rmax.y += axis2.y; else rmin.y += axis2.y;
        
    //Third axis
    if (axis0.z>0) rmax.z += axis0.z; else rmin.z += axis0.z;
    if (axis1.z>0) rmax.z += axis1.z; else rmin.z += axis1.z;
    if (axis2.z>0) rmax.z += axis2.z; else rmin.z += axis2.z;
    
    //if this box is completely inside the local box, return result
    if (rmax.x<lmaxx && rmax.y<lmaxy && rmax.z<lmaxz &&
        rmin.x>lminx && rmin.y>lminy && rmin.z>lminz) return 1;
    
    //if this box is completely on one side of the local box, no intersection
    if (rmax.x<lminx) return 0;
    if (rmax.y<lminy) return 0;
    if (rmax.z<lminz) return 0;
    
    if (rmin.x>lmaxx) return 0;
    if (rmin.y>lmaxy) return 0;
    if (rmin.z>lmaxz) return 0;
    
    //give up to save calculations, assume they intersect
    return 2;
}

bool Box::collidesFace(AGKVector* p1, AGKVector* p2, AGKVector* p3)
{
    //check if the polygon is completely on one side of the box
    if (p1->x<minx && p2->x<minx && p3->x<minx) return false;
    if (p1->y<miny && p2->y<miny && p3->y<miny) return false;
    if (p1->z<minz && p2->z<minz && p3->z<minz) return false;
    
    if (p1->x>maxx && p2->x>maxx && p3->x>maxx) return false;
    if (p1->y>maxy && p2->y>maxy && p3->y>maxy) return false;
    if (p1->z>maxz && p2->z>maxz && p3->z>maxz) return false;
    
    //check which vertices of the triangle are inside the box
    int count = 0;
    if (inBox(p1)) count++;
    if (inBox(p2)) count++;
    if (inBox(p3)) count++;
    
    //3 inside = completely inside, collision
    //0 inside = might straddle box, need more checks
    //1 or 2   = must intersect box
    //if (count==3) return false;
    if (count>0) return true;
    
    float dist1;
    //check the polygon edges for intersection with the box
    AGKVector temp(1/(p2->x-p1->x),1/(p2->y-p1->y),1/(p2->z-p1->z));
    if (intersectBox(p1,&temp,&dist1) && dist1>=0) return true;
    
    temp.Set(1/(p3->x-p2->x),1/(p3->y-p2->y),1/(p3->z-p2->z));
    if (intersectBox(p2,&temp,&dist1) && dist1>=0) return true;
    
    temp.Set(1/(p1->x-p3->x),1/(p1->y-p3->y),1/(p1->z-p3->z));
    if (intersectBox(p3,&temp,&dist1) && dist1>=0) return true;
    
    //remaining case - triangle completely surrounds box, if so
    //box diagonals would intersect triangle
    
    //only check the box diagonals (4 of them)
    //get poly normal
    temp = *p2 - *p1;
    AGKVector normal = *p3 - *p1;
	normal.Cross( *p3 - *p1, temp );
    
    //check for 'point' triangles
    float distv = normal.x*normal.x + normal.y*normal.y + normal.z*normal.z;
    if (distv<0.00001) return false;
    //calculate d
    distv = normal.x*p1->x + normal.y*p1->y + normal.z*p1->z;
    
    //work out the normal for each side of the poly, stored in temp, temp2 and temp3
    temp.Cross( normal, *p2 - *p1 );
    dist1 = temp.x*p1->x + temp.y*p1->y + temp.z*p1->z;
    
    AGKVector temp2;
    temp2.Cross( normal, *p3 - *p2 );
    float dist2 = temp2.x*p2->x + temp2.y*p2->y + temp2.z*p2->z;
    
    AGKVector temp3;
	temp3.Cross( normal, *p1 - *p3 );
    float dist3 = temp3.x*p3->x + temp3.y*p3->y + temp3.z*p3->z;
    
    //do some precalculation
    float b1 = minx*normal.x; float b4 = maxx*normal.x;
    float b2 = miny*normal.y; float b5 = maxy*normal.y;
    float b3 = minz*normal.z; float b6 = maxz*normal.z;
    
    float d1,d2,d3,d4,d5;
    AGKVector intersect;
    
    //diagonal 1
    d1 = b1 + b2 + b3 - distv;
    d2 = b4 + b5 + b6 - distv;
    if (d1*d2<0) {
        //find intersection point and check if inside triangle
        d3 = -d1/((b4-b1) + (b5-b2) + (b6-b3));
        intersect.x = minx + (maxx-minx)*d3;
        intersect.y = miny + (maxy-miny)*d3;
        intersect.z = minz + (maxz-minz)*d3;
        d3 = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - dist1;
        d4 = temp2.x*intersect.x + temp2.y*intersect.y + temp2.z*intersect.z - dist2;
        d5 = temp3.x*intersect.x + temp3.y*intersect.y + temp3.z*intersect.z - dist3;
        if (d3<-0.00001 && d4<-0.00001 && d5<-0.00001) return true;
    }
    
    //diagonal 2 - re-use previous calculation
    d1 = d1 - b2 + b5;
    d2 = d2 - b5 + b2;
    if (d1*d2<0) {
        d3 = -d1/((b4-b1) + (b2-b5) + (b6-b3));
        intersect.x = minx + (maxx-minx)*d3;
        intersect.y = maxy + (miny-maxy)*d3;
        intersect.z = minz + (maxz-minz)*d3;
        d3 = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - dist1;
        d4 = temp2.x*intersect.x + temp2.y*intersect.y + temp2.z*intersect.z - dist2;
        d5 = temp3.x*intersect.x + temp3.y*intersect.y + temp3.z*intersect.z - dist3;
        if (d3<-0.00001 && d4<-0.00001 && d5<-0.00001) return true;
    }
    
    //diagonal 3
    d1 = d1 - b3 + b6;
    d2 = d2 - b6 + b3;
    if (d1*d2<0) {
        d3 = -d1/((b4-b1) + (b2-b5) + (b3-b6));
        intersect.x = minx + (maxx-minx)*d3;
        intersect.y = maxy + (miny-maxy)*d3;
        intersect.z = maxz + (minz-maxz)*d3;
        d3 = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - dist1;
        d4 = temp2.x*intersect.x + temp2.y*intersect.y + temp2.z*intersect.z - dist2;
        d5 = temp3.x*intersect.x + temp3.y*intersect.y + temp3.z*intersect.z - dist3;
        if (d3<-0.00001 && d4<-0.00001 && d5<-0.00001) return true;
    }
    
    //diagonal 4
    d1 = d1 - b5 + b2;
    d2 = d2 - b2 + b5;
    if (d1*d2<0) {
        d3 = -d1/((b4-b1) + (b5-b2) + (b3-b6));
        intersect.x = minx + (maxx-minx)*d3;
        intersect.y = miny + (maxy-miny)*d3;
        intersect.z = maxz + (minz-maxz)*d3;
        d3 = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - dist1;
        d4 = temp2.x*intersect.x + temp2.y*intersect.y + temp2.z*intersect.z - dist2;
        d5 = temp3.x*intersect.x + temp3.y*intersect.y + temp3.z*intersect.z - dist3;
        if (d3<-0.00001 && d4<-0.00001 && d5<-0.00001) return true;
    }
    
    return false;
}

int Box::inFrustumPlane( const AGKVector* n, float d )
{
	float closestX = (n->x < 0) ? minx : maxx;
	float closestY = (n->y < 0) ? miny : maxy;
	float closestZ = (n->z < 0) ? minz : maxz;

	float dotp = n->x*closestX + n->y*closestY + n->z*closestZ + d;

	return dotp > 0 ? 1 : 0;
}

int Box::inFrustum( const AGKMatrix4* transform, float zMin, float zMax )
{
	AGKVector4 pos[8];
	pos[0].Set( minx, miny, minz, 1 );
	pos[1].Set( minx, miny, maxz, 1 );
	pos[2].Set( minx, maxy, minz, 1 );
	pos[3].Set( minx, maxy, maxz, 1 );
	pos[4].Set( maxx, miny, minz, 1 );
	pos[5].Set( maxx, miny, maxz, 1 );
	pos[6].Set( maxx, maxy, minz, 1 );
	pos[7].Set( maxx, maxy, maxz, 1 );

	for( int i = 0; i < 8; i++ )
	{
		pos[i].Mult( *transform );
		pos[i].vec.x /= pos[i].w;
		pos[i].vec.y /= pos[i].w;
		pos[i].vec.z /= pos[i].w;
	}

	int resultX=0, resultY=0, resultZ=0;
	for( int i = 0; i < 8; i++ )
	{
		if ( pos[i].vec.x > 1 ) resultX++;
		else if ( pos[i].vec.x < -1 ) resultX--;
		if ( pos[i].vec.y > 1 ) resultY++;
		else if ( pos[i].vec.y < -1 ) resultY--;
		if ( pos[i].vec.z > zMax ) resultZ++;
		else if ( pos[i].vec.z < zMin ) resultZ--;
	}

	resultX = abs(resultX);
	resultY = abs(resultY);
	resultZ = abs(resultZ);

	if ( resultX < 8 && resultY < 8 && resultZ < 8 ) return 1;
	else return 0;
}
