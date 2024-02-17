#include "Face.h"

using namespace AGK;

bool Face::MakeFace( unsigned id, AGKVector *p1, AGKVector *p2, AGKVector *p3 )
{
	vert1 = *p1;
	vert2 = *p2;
	vert3 = *p3;

	AGKVector v1 = *p3 - *p1;
	AGKVector v2 = *p2 - *p1;

	//if ( v1.SqrLength() < SMALL_FLOAT*SMALL_FLOAT || v2.SqrLength() < SMALL_FLOAT*SMALL_FLOAT ) return false;
	if ( v1.SqrLength() < SMALL_FLOAT || v2.SqrLength() < SMALL_FLOAT ) return false;

	//v1.Normalize();
	//v2.Normalize();
	normal.Cross( v2, v1 );
	float length = normal.Length();

	//zero area polygons will have an undefined normal, which will cause problems later
	//so if any exist remove them
	if (length<SMALL_FLOAT) return false;
    
	normal.Mult(1.0f/length);

	v1 = normal;
	v2.Set(p2->x,p2->y,p2->z);
    
	d = -1.0f * v1.Dot(v2);

//	collisionon = true;
	faceId = id;

	return true;
}

//checks the ray from @p in direction @v for collision with the polygon
//@vn is @v normalized
bool Face::intersects(AGKVector* p, AGKVector* v, AGKVector* vn, CollisionResults* cRes)
{
//    if (!collisionon) return false;
    
    float dist1,dist2;
    AGKVector intersect;
    
	//calculate the distance of each end of the ray from the polygon plane (negative is in behind, positive is in front, with the normal pointing away from the front face)
    dist1 = normal.x*(p->x+v->x) + normal.y*(p->y+v->y) + normal.z*(p->z+v->z) + d; //end point
    dist2 = normal.x*p->x + normal.y*p->y + normal.z*p->z + d; //start point
    
	if (dist2 < -SMALL_FLOAT || dist1 > SMALL_FLOAT || dist1 >= dist2) return false; //both ends must be on opposite sides of the plane, and be pointing towards the front face
    
	//ray intersects plane

	//calculate the point along the ray that it intersects the plane
    dist1 = -(normal.x*vn->x) - (normal.y*vn->y) - (normal.z*vn->z);
    if (dist1 < -SMALL_FLOAT || dist1 > SMALL_FLOAT) dist1 = dist2/dist1;
    else dist1 = 0;
    
	//check if there is already an intersection point further up the ray
    if (!cRes->isValidDistance(dist1)) return false;
                
    intersect.x = p->x + vn->x*dist1;
    intersect.y = p->y + vn->y*dist1;
    intersect.z = p->z + vn->z*dist1;
    
	//check the intersection point is within the triangle bounds
    if (!pointInPoly(&intersect)) return false;  
    
    cRes->addPoint(&intersect,&normal,dist1,faceId);
    
    return true;
}

bool Face::sphereIntersects(AGKVector* p, AGKVector* v, AGKVector* vn, float rRadius, AGKVector* scale, CollisionResults* cRes)
{
//    if (!collisionon) return false;
    
    float dist1,dist2;
    AGKVector intersect;
//    bool edge = false;
    AGKVector n;
    
    if (scale!=0) 
    { 
        n.Set( normal.x/scale->x, normal.y/scale->y, normal.z/scale->z );
        n.Normalize();
        
        float d2 = -n.x*vert1.x*scale->x - n.y*vert1.y*scale->y - n.z*vert1.z*scale->z;
        
        AGKVector p1(p->x - n.x*rRadius, p->y - n.y*rRadius, p->z - n.z*rRadius);
        
        dist1 = n.x*(p1.x+v->x) + n.y*(p1.y+v->y) + n.z*(p1.z+v->z) + d2;
        dist2 = n.x*p1.x + n.y*p1.y + n.z*p1.z + d2;
        
        if (dist2<dist1 || dist2<-(rRadius*2) || dist1>0.00001) return false;
        
        if (dist2<0)
        {
            //case: sphere starts imbedded in the polygon
            //response is to push sphere perpendicularly out of the polygon
            //alternative of pushing out in direction of incoming ray produced 
            //undesirable results.
            intersect.x = p1.x - dist2*normal.x;            
            intersect.y = p1.y - dist2*normal.y;
            intersect.z = p1.z - dist2*normal.z;
            dist1 = dist2;
        }
        else
        {
            //case: sphere hits sometime after time 0
            dist1=-(n.x*vn->x) - (n.y*vn->y) - (n.z*vn->z);
            if (dist1<-0.00001 || dist1>0.00001) dist1=dist2/dist1;
            else dist1=0;
                    
            //if (!cRes->isValidDistance(dist1)) return false;
                        
            intersect.x = p1.x + vn->x*dist1;
            intersect.y = p1.y + vn->y*dist1;
            intersect.z = p1.z + vn->z*dist1;
        }
        
        intersect.x /= scale->x; intersect.y /= scale->y; intersect.z /= scale->z;
    }
    else
    {    
        AGKVector p1(p->x - normal.x*rRadius, p->y - normal.y*rRadius, p->z - normal.z*rRadius);
        
        dist1 = normal.x*(p1.x+v->x) + normal.y*(p1.y+v->y) + normal.z*(p1.z+v->z) + d;
        dist2 = normal.x*p1.x + normal.y*p1.y + normal.z*p1.z + d;
                   
        if (dist2<dist1 || dist2<-(rRadius*2) || dist1>0.00001) return false;
        
        if (dist2<0)
        {
            intersect.x = p1.x - dist2*normal.x;            
            intersect.y = p1.y - dist2*normal.y;
            intersect.z = p1.z - dist2*normal.z;
            dist1 = dist2;
        }
        else
        {
            dist1=-(normal.x*vn->x) - (normal.y*vn->y) - (normal.z*vn->z);
            if (dist1<-0.00001 || dist1>0.00001) dist1=dist2/dist1;
            else dist1=0;
                    
            //if (!cRes->isValidDistance(dist1)) return false;
                        
            intersect.x = p1.x + vn->x*dist1;
            intersect.y = p1.y + vn->y*dist1;
            intersect.z = p1.z + vn->z*dist1;
        }
    }
    
    //if (!(bounds.inBox(&intersect))) return false;
    
    if (pointInPoly(&intersect))
    {
        if (scale!=0) 
        { 
            intersect.x *= scale->x; intersect.y *= scale->y; intersect.z *= scale->z;
            intersect.x += n.x*rRadius; intersect.y += n.y*rRadius; intersect.z += n.z*rRadius;
            cRes->addPoint(&intersect,&n,dist1,faceId);
        }
        else
        {
            intersect.x += normal.x*rRadius; intersect.y += normal.y*rRadius; intersect.z += normal.z*rRadius;
            cRes->addPoint(&intersect,&normal,dist1,faceId);
        }     
    }
    else
    {
        //case: sphere ray intersects polygon plane but not the polygon directly
        //need to check if it glances the polygon
        
        //find the closest point on the polygon to @intersect
        
        //edge 1
        AGKVector eVec = vert2 - vert1;
        AGKVector closest1 = eVec.ClosestPoint( vert1, intersect );
        
		dist1 = closest1.GetSqrDist( intersect );
        
        //edge 2
        eVec = vert3 - vert2;
        AGKVector closest2 = eVec.ClosestPoint( vert2, intersect );
        
		dist2 = closest2.GetSqrDist( intersect );
        //is this point closer than the last
        if (dist2<dist1)
        {
            dist1 = dist2;
            closest1.x = closest2.x; closest1.y = closest2.y; closest1.z = closest2.z;
        }
        
        //edge 3
        eVec = vert1 - vert3;
        closest2 = eVec.ClosestPoint( vert3, intersect );
        
		dist2 = closest2.GetSqrDist( intersect );
        //is this point closer than the last
        if (dist2<dist1) {
            closest1.x = closest2.x; closest1.y = closest2.y; closest1.z = closest2.z;
        }
               
        //closest point now in @closest1
        if (scale!=0) { closest1.x *= scale->x; closest1.y *= scale->y; closest1.z *= scale->z; }
        
        //back cast the ray from @closest1 in the direction of -@v and check against the sphere at @p
        dist1 = vn->x*(p->x-closest1.x) + vn->y*(p->y-closest1.y) + vn->z*(p->z-closest1.z);
        if (dist1>0) return false;
        
		eVec = *p - closest1;
        //eVec.x = (p->x-closest1.x); eVec.y = (p->y-closest1.y); eVec.z = (p->z-closest1.z);
        dist2 = (eVec.x*eVec.x + eVec.y*eVec.y + eVec.z*eVec.z) - rRadius*rRadius;
        dist2 = dist1*dist1 - dist2;
        
        if (dist2<=0) return false;
        
		dist2 = -dist1 - (float) sqrt(dist2);
        
        if ((dist2>0) && ((dist2*dist2)>((v->x*v->x)+(v->y*v->y)+(v->z*v->z)))) return false;
        
        intersect.x = p->x + vn->x*dist2;
        intersect.y = p->y + vn->y*dist2;
        intersect.z = p->z + vn->z*dist2;
        
        AGKVector n = intersect - closest1;
        n.Mult(1.0f/rRadius);
        
        cRes->addPoint(&intersect,&n,dist2,faceId);
    }
    
    return true;
}

//check if the point @p, on the polygon plane, is inside the polygon bounds  
bool Face::pointInPoly(AGKVector* p)
{
    //method: make each edge into a plane facing outwards, if the point is 'below'
    //each plane then point lies inside the polygon
    
    //First side
    AGKVector v = vert2 - vert1;
    AGKVector v3;
	v3.Cross( normal, v );
    //v3 is the new plane's normal, points towards the triangle
    
    float ld = v3.x*vert1.x + v3.y*vert1.y + v3.z*vert1.z;
    
    ld = v3.x*p->x + v3.y*p->y + v3.z*p->z - ld;
    if (ld<-0.00001) return false; //negative=below, positive=above
    
    //Second side
    v = vert3 - vert2;
	v3.Cross( normal, v );
        
    ld = v3.x*vert2.x + v3.y*vert2.y + v3.z*vert2.z;
    
    ld = v3.x*p->x + v3.y*p->y + v3.z*p->z - ld;
    if (ld<-0.00001) return false;
    
    //Third side
    v = vert1 - vert3;
    v3.Cross( normal, v );
    
    ld = v3.x*vert3.x + v3.y*vert3.y + v3.z*vert3.z;
    
    ld = v3.x*p->x + v3.y*p->y + v3.z*p->z - ld;
    if (ld<-0.00001) return false;

    return true;
}

//checks the polygon defined by @p1, @p2, @p3 for intersection with the local polygon
bool Face::collidesPoly(AGKVector* p1, AGKVector* p2, AGKVector* p3)
{
//    if (!collisionon) return false;
    
    //check the sidedness of each vertex against this triangle's plane
    float dist1 = normal.x*p1->x + normal.y*p1->y + normal.z*p1->z + d;
    float dist2 = normal.x*p2->x + normal.y*p2->y + normal.z*p2->z + d;
    float dist3 = normal.x*p3->x + normal.y*p3->y + normal.z*p3->z + d;
    float distv;
    AGKVector* ps = new AGKVector[2];
    int index = 0;
    AGKVector temp;
    AGKVector intersect;
            
    //find any two vertices that are on different sides    
    if (dist1*dist2<0)
    {
        //calculate distance to intersection point
        temp = *p2 - *p1;
        distv=-(normal.x*temp.x) - (normal.y*temp.y) - (normal.z*temp.z);
        if (distv<-0.00001 || distv>0.00001) distv=dist1/distv;
        else distv=0;
        
        //calc the intersection point on the poly for this edge
        intersect.x = p1->x + temp.x*distv;
        intersect.y = p1->y + temp.y*distv;
        intersect.z = p1->z + temp.z*distv;
        
        //store the point for later
        *(ps) = intersect;
        index++;
    }
    
    if (dist2*dist3<0)
    {
		temp = *p3 - *p2;
        distv=-(normal.x*temp.x) - (normal.y*temp.y) - (normal.z*temp.z);
        if (distv<-0.00001 || distv>0.00001) distv=dist2/distv;
        else distv=0;
        
        //calc the intersection point on the poly for this edge
        intersect.x = p2->x + temp.x*distv;
        intersect.y = p2->y + temp.y*distv;
        intersect.z = p2->z + temp.z*distv;
        
        *(ps+index) = intersect;
        index++;
    }
    
    if (dist1*dist3<0)
    {
		temp = *p1 - *p3;
        distv=-(normal.x*temp.x) - (normal.y*temp.y) - (normal.z*temp.z);
        if (distv<-0.00001 || distv>0.00001) distv=dist3/distv;
        else distv=0;
        
        //calc the intersection point on the poly for this edge
        intersect.x = p3->x + temp.x*distv;
        intersect.y = p3->y + temp.y*distv;
        intersect.z = p3->z + temp.z*distv;
        
        *(ps+index) = intersect;
    }
    
    //index is either 0 (all on same side) or 2 (one point on opposite side)
    if (index>0) {
        //if 2 then check the line joining these two intersection points with the triangle
        bool result = lineInPoly(ps,ps+1);
        delete [] ps;
        return result;
    }
    
    delete [] ps;
    return false;
}

bool Face::lineInPoly(AGKVector* p1, AGKVector* p2)
{
    //flatten the triangle onto it's biggest area (longest normal)
    //if normal is negative then vertex winding order is reversed in 2D perspective
    
    //update 08/01/06 - suggestion - if winding order is correctly accounted for then 
    //                               6 potential calls can be reduced to 3
    if (normal.x>0.55)
    {
        return lineInPoly2D( vert1.y,vert1.z, vert2.y,vert2.z, vert3.y,vert3.z, p1->y,p1->z, p2->y-p1->y, p2->z-p1->z );
    }
    else if (normal.x<-0.55)
    {
        return lineInPoly2D( vert1.y,vert1.z, vert3.y,vert3.z, vert2.y,vert2.z, p1->y,p1->z, p2->y-p1->y, p2->z-p1->z );
    }
    else if (normal.y>0.55)
    {                         
        return lineInPoly2D( vert1.z,vert1.x, vert2.z,vert2.x, vert3.z,vert3.x, p1->z,p1->x, p2->z-p1->z, p2->x-p1->x );                  
    }
    else if (normal.y<-0.55)
    { 
        return lineInPoly2D( vert1.z,vert1.x, vert3.z,vert3.x, vert2.z,vert2.x, p1->z,p1->x, p2->z-p1->z, p2->x-p1->x );
    }
    else if (normal.z>0)
    {
        return lineInPoly2D( vert1.x,vert1.y, vert2.x,vert2.y, vert3.x,vert3.y, p1->x,p1->y, p2->x-p1->x, p2->y-p1->y );
    }
    else
    {
        return lineInPoly2D( vert1.x,vert1.y, vert3.x,vert3.y, vert2.x,vert2.y, p1->x,p1->y, p2->x-p1->x, p2->y-p1->y );
    }    
}

//line defined by @p1 in direction @diffP
//polygon defined by @v1, @v2, @v3
//will return true if the line cuts the polygon
bool Face::lineInPoly2D(float v1x,float v1y,float v2x,float v2y,float v3x,float v3y,
                        float p1x,float p1y,float diffPx,float diffPy)
{
    float diffY = v2y - v1y;
    float diffX = v2x - v1x;
    
    //method: cut the line until it's start and end points are inside the bounds
    //store these in t1, t2
    
    //assumes anti-clockwise winding order

    //update 08/01/06 - modifier for detecting cw/ccw winding order
    float rel = diffX*(v3y-v1y) - diffY*(v3x-v1x);
    float ccw = 1;
    if (rel<0) ccw = -1;
    
    float t1=0;
    float t2=1;
    float tempt;
    
    rel = diffPy*diffX - diffPx*diffY;
    
    //heading inside*ccw = +ve
    if (rel>0.00001 || rel<-0.00001) { 
        tempt = (diffY*(p1x-v1x) - diffX*(p1y-v1y))/rel;
        if (rel*ccw>0) { if (tempt>t1) t1 = tempt; }
        else { if (tempt<t2) t2 = tempt; }
    }
    else {
        if ( ((p1x-v1x)*diffY + (v1y-p1y)*diffX)*ccw > -0.00001 ) return false;                        
    }
    
    diffY = v3y - v2y;
    diffX = v3x - v2x;    
    rel = diffPy*diffX - diffPx*diffY;
    
    if (rel>0.00001 || rel<-0.00001) {
        tempt = (diffY*(p1x-v2x) - diffX*(p1y-v2y))/rel;
        if (rel*ccw>0) { if (tempt>t1) t1 = tempt; }
        else { if (tempt<t2) t2 = tempt; }
    }
    else {
        if ( ((p1x-v2x)*diffY + (v2y-p1y)*diffX)*ccw > -0.00001 ) return false;                        
    }
    
    diffY = v1y - v3y;
    diffX = v1x - v3x;    
    rel = diffPy*diffX - diffPx*diffY;
    
    if (rel>0.00001 || rel<-0.00001) {
        tempt = (diffY*(p1x-v3x) - diffX*(p1y-v3y))/rel;
        if (rel*ccw>0) { if (tempt>t1) t1 = tempt; }
        else { if (tempt<t2) t2 = tempt; }
    }
    else {
        if ( ((p1x-v3x)*diffY + (v3y-p1y)*diffX)*ccw > -0.00001 ) return false;                        
    }
    
    //if line leaves the polygon before it enters it, must be outside
    if (t2>t1) return true;
    
    return false;
}

bool Face::collidesSphere(AGKVector* center, float sRadius, AGKVector* scale)
{
//    if (!collisionon) return false;
    
    AGKVector p1(vert1.x*scale->x - center->x, vert1.y*scale->y - center->y, vert1.z*scale->z - center->z);
    AGKVector p2(vert2.x*scale->x - center->x, vert2.y*scale->y - center->y, vert2.z*scale->z - center->z);
    AGKVector p3(vert3.x*scale->x - center->x, vert3.y*scale->y - center->y, vert3.z*scale->z - center->z);
    
    float sqrRadius = sRadius*sRadius;
    int count = 0;
    
    //count number of vertices inside the sphere
    if (p1.x*p1.x + p1.y*p1.y + p1.z*p1.z<sqrRadius) count++;
    if (p2.x*p2.x + p2.y*p2.y + p2.z*p2.z<sqrRadius) count++;
    if (p3.x*p3.x + p3.y*p3.y + p3.z*p3.z<sqrRadius) count++;
    
    //if all inside (3), collision, outside (0) requires more checks
    //anything else signals definite intersection
    //if (count==3) return false;
    if (count>0) return true;
    
    float dist1, dist2;
    
    //check edges against the sphere
    AGKVector temp = p2 - p1;
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
        
    AGKVector n(normal.x/scale->x, normal.y/scale->y, normal.z/scale->z);
    
//    temp.set(&p1,&p2);  
//    AGKVector n(&p1,&p3);          
//    n = n.crossProduct(&temp);
    
    //work out plane's distance from origin (sphere is centered on origin)
    dist1 = n.x*p1.x + n.y*p1.y + n.z*p1.z;
    dist2 = n.x*n.x + n.y*n.y + n.z*n.z;
    if (dist2<0.00001) return false;
    dist2 = (dist1*dist1)/dist2;
    if (dist2>sqrRadius) return false;             
    
    AGKVector intersect;
    
    //find the point on the plane closest to the sphere
    if (dist1>-0.00001 || dist1<0.00001) 
    {
        intersect.Set(0,0,0);
    }
    else
    {
        dist2 = dist2/dist1;    
        intersect.Set(n.x*dist2, n.y*dist2, n.z*dist2);
    }
    
    //is this point inside the triangle
    AGKVector temp2 = p2 - p1;
	temp.Cross( n, temp2 );
        
    dist1 = temp.x*p1.x + temp.y*p1.y + temp.z*p1.z;
    
    dist1 = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - dist1;
    if (dist1<-0.00001) return false;
    
    //Second side
    temp2 = p3 - p2;
    temp.Cross( n, temp2 );
    
    dist1 = temp.x*p2.x + temp.y*p2.y + temp.z*p2.z;
    
    dist1 = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - dist1;
    if (dist1<-0.00001) return false;
    
    //Third side
    temp2 = p1 - p3;
    temp.Cross( n, temp2 );
    
    dist1 = temp.x*p3.x + temp.y*p3.y + temp.z*p3.z;
    
    dist1 = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - dist1;
    if (dist1<-0.00001) return false; 
  
    return true;
    //return false;
}