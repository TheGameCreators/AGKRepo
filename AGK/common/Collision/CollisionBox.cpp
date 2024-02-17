#include "CollisionBox.h"

#include <math.h>

using namespace AGK;

void CollisionBox::drawBounds(int freeObj)
{
	/*
    float cx = (bounds.maxbx()+bounds.minbx()) / 2.0f;
    float cy = (bounds.maxby()+bounds.minby()) / 2.0f;
    float cz = (bounds.maxbz()+bounds.minbz()) / 2.0f;
    
    float extentX = bounds.maxbx() - bounds.minbx();
    float extentY = bounds.maxby() - bounds.minby();
    float extentZ = bounds.maxbz() - bounds.minbz();
    
    MakeObjectBox(freeObj, extentX, extentY, extentZ);
    //PositionObject(freeObj, cx, cy, cz);
    OffsetLimb(freeObj, 0, cx, cy, cz);
	*/
}

void CollisionBox::makeCollisionObject(Face* pFaces)
{
    float newRadius;
	mainRadius = 0;

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
bool CollisionBox::intersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, CollisionResults* cRes)
{
    float dist1;
    AGKVector intersect;
    AGKVector normal;
    
    int side = bounds.intersectBox(p,vi,&dist1);
    
    if (dist1<0.0) return false;
    
    switch(side)
    {
        case 0: return false; break;
        case 1: normal.Set(-1.0,0,0); break;
        case 2: normal.Set(1.0,0.0,0.0); break;
        case 3: normal.Set(0.0,-1.0,0.0); break;
        case 4: normal.Set(0.0,1.0,0.0); break;
        case 5: normal.Set(0.0,0.0,-1.0); break;
        case 6: normal.Set(0.0,0.0,1.0); break;
        default : return false;
    }
    
    intersect.x = p->x + v->x*dist1;
    intersect.y = p->y + v->y*dist1;
    intersect.z = p->z + v->z*dist1;
            
    dist1=dist1*(v->Length());
        
    cRes->addPoint(&intersect,&normal,dist1,0);
        
    return true;       
}

//collision boxes are the only shape that needs accurate sphereRay - box checks
bool CollisionBox::sphereIntersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, float rRadius, AGKVector* scale, CollisionResults* cRes)
{
    float lminx,lminy,lminz,lmaxx,lmaxy,lmaxz;
    
    if (scale!=0)
    {
        lminx = bounds.minbx() * scale->x - rRadius;
        lminy = bounds.minby() * scale->y - rRadius;
        lminz = bounds.minbz() * scale->z - rRadius;
        lmaxx = bounds.maxbx() * scale->x + rRadius;
        lmaxy = bounds.maxby() * scale->y + rRadius;
        lmaxz = bounds.maxbz() * scale->z + rRadius;
    }
    else
    {
        lminx = bounds.minbx() - rRadius;
        lminy = bounds.minby() - rRadius;
        lminz = bounds.minbz() - rRadius;
        lmaxx = bounds.maxbx() + rRadius;
        lmaxy = bounds.maxby() + rRadius;
        lmaxz = bounds.maxbz() + rRadius;
    }
    
    float tminx,tmaxx,tminy,tmaxy;
    int side = 0;

    if (vi->x>=0) {
        tminx = (lminx - p->x) *vi->x;
        tmaxx = (lmaxx - p->x) *vi->x;
        side=1;
    }
    else {
        tmaxx = (lminx - p->x) *vi->x;
        tminx = (lmaxx - p->x) *vi->x;
        side=2;
    }
    
    if (vi->y>=0) {
        tminy = (lminy - p->y) *vi->y;
        tmaxy = (lmaxy - p->y) *vi->y;
        if (tminy>tminx) side=3;
    }
    else {
        tmaxy = (lminy - p->y) *vi->y;
        tminy = (lmaxy - p->y) *vi->y;
        if (tminy>tminx) side=4;
    }
               
    if ((tminx > tmaxy) || (tminy > tmaxx)) return false;
    
    if (tminy>tminx) tminx = tminy;
    if (tmaxy<tmaxx) tmaxx = tmaxy;
    
    if (vi->z>=0) {
        tminy = (lminz - p->z) *vi->z;
        tmaxy = (lmaxz - p->z) *vi->z;
        if (tminy>tminx) side=5;
    }
    else {
        tmaxy = (lminz - p->z) *vi->z;
        tminy = (lmaxz - p->z) *vi->z;
        if (tminy>tminx) side=6;
    }
    
    if ((tminx > tmaxy) || (tminy > tmaxx)) return false;
                                      
    if (tminy>tminx) tminx = tminy;
    if (tmaxy<tmaxx) tmaxx = tmaxy;
    
    if (tmaxx<0.0) return false;
    if (tminx>1.0) return false;
    if (side==0) return false;
    
    lminx += rRadius; lminy += rRadius; lminz += rRadius;
    lmaxx -= rRadius; lmaxy -= rRadius; lmaxz -= rRadius;
    
    AGKVector p1(p->x,p->y,p->z);
    AGKVector intersect;
    AGKVector n;
    float dist1, dist2;
    float sqrDist = v->x*v->x + v->y*v->y + v->z*v->z;
    
    if (side==1 || side==2)
    {
        if (side==1) { dist1 = lminx; p1.x = p->x + rRadius; }
        else { dist1 = lmaxx; p1.x = p->x - rRadius; }
        
        tminx = (dist1 - p1.x) / vn->x;
        if (tminx<-rRadius) return false;
        if (tminx>0 && (tminx*tminx)>sqrDist) return false;
        if (tminx<0) {
            p1.x = dist1; //p1.y = p->y; p1.z = p->z;
        }
        else {
            p1.x = p1.x + vn->x*tminx; p1.y = p1.y + vn->y*tminx; p1.z = p1.z + vn->z*tminx;
        }
            
        if (p1.y>=lminy && p1.y<=lmaxy && p1.z>=lminz && p1.z<=lmaxz) 
        {
            if (side==1) { n.Set(-1,0,0); p1.x = p1.x - rRadius; }
            else { n.Set(1,0,0); p1.x = p1.x + rRadius; }
            cRes->addPoint(&p1,&n,tminx,0);
            return true;
        }
        else
        {
            if (p1.y>lmaxy) p1.y = lmaxy; if (p1.y<lminy) p1.y = lminy;
            if (p1.z>lmaxz) p1.z = lmaxz; if (p1.z<lminz) p1.z = lminz;
        }
    }
    
    if (side==3 || side==4)
    {
        if (side==3) { dist1 = lminy; p1.y = p->y + rRadius; }
        else { dist1 = lmaxy; p1.y = p->y - rRadius; }
        
        tminx = (dist1 - p1.y) / vn->y;
        if (tminx<-rRadius) return false;
        if (tminx>0 && (tminx*tminx)>sqrDist) return false;
        if (tminx<0) {
            p1.y = dist1; //p1.x = p->x;  p1.z = p->z;
        }
        else {
            p1.x = p1.x + vn->x*tminx; p1.y = p1.y + vn->y*tminx; p1.z = p1.z + vn->z*tminx;
        }
            
        if (p1.x>=lminx && p1.x<=lmaxx && p1.z>=lminz && p1.z<=lmaxz) 
        {
            if (side==3) { n.Set(0,-1,0); p1.y = p1.y - rRadius; }
            else { n.Set(0,1,0); p1.y = p1.y + rRadius; }
            cRes->addPoint(&p1,&n,tminx,0);
            return true;
        }
        else
        {
            if (p1.x>lmaxx) p1.x = lmaxx; if (p1.x<lminx) p1.x = lminx;
            if (p1.z>lmaxz) p1.z = lmaxz; if (p1.z<lminz) p1.z = lminz;
        }
    }
    
    if (side==5 || side==6)
    {
        if (side==5) { dist1 = lminz; p1.z = p->z + rRadius; }
        else { dist1 = lmaxz; p1.z = p->z - rRadius; }
        
        tminx = (dist1 - p1.z) / vn->z;
        if (tminx<-rRadius) return false;
        if (tminx>0 && (tminx*tminx)>sqrDist) return false;
        if (tminx<0) {
            p1.z = dist1; //p1.x = p->x; p1.y = p->y; 
        }
        else {
            p1.x = p1.x + vn->x*tminx; p1.y = p1.y + vn->y*tminx; p1.z = p1.z + vn->z*tminx;
        }
            
        if (p1.y>=lminy && p1.y<=lmaxy && p1.x>=lminx && p1.x<=lmaxx)
        {
            if (side==5) { n.Set(0,0,-1); p1.z = p1.z - rRadius; }
            else { n.Set(0,0,1); p1.z = p1.z + rRadius; }
            cRes->addPoint(&p1,&n,tminx,0);
            return true;
        }
        else
        {
            if (p1.y>lmaxy) p1.y = lmaxy; if (p1.y<lminy) p1.y = lminy;
            if (p1.x>lmaxx) p1.x = lmaxx; if (p1.x<lminx) p1.x = lminx;
        }
    }
            
    dist1 = vn->x*(p->x-p1.x) + vn->y*(p->y-p1.y) + vn->z*(p->z-p1.z);
    if (dist1>0) return false;
    
    n.x = (p->x-p1.x); n.y = (p->y-p1.y); n.z = (p->z-p1.z);
    dist2 = (n.x*n.x + n.y*n.y + n.z*n.z) - rRadius*rRadius;
    dist2 = dist1*dist1 - dist2;
    
    if (dist2<0) return false;
    
	dist2 = -dist1 - (float) sqrt(dist2);
    
    if ((dist2>0) && ((dist2*dist2)>((v->x*v->x)+(v->y*v->y)+(v->z*v->z)))) return false;
    
    intersect.x = p->x + vn->x*dist2;
    intersect.y = p->y + vn->y*dist2;
    intersect.z = p->z + vn->z*dist2;
    
    n.Set(intersect.x - p1.x, intersect.y - p1.y, intersect.z - p1.z);
    n.Mult(1/rRadius);
    
    cRes->addPoint(&intersect,&n,dist2,0);
    return true;
}

//checks the box defined in local space against a list of triangles
//triangles are transformed to local space using  @transform
bool CollisionBox::collidesPoly(Face* obj2Faces, AGKMatrix4* transform)
{
    AGKVector p1, p2, p3, intersect;
    AGKVector normal, temp, temp2, temp3;
    float dist1, dist2, dist3, distv;
    float b1,b2,b3,b4,b5,b6;
    float d1,d2,d3,d4,d5;
    int count = 0;
    
    //start of poly object loop
    for(Face* thisFace=obj2Faces; thisFace!=0; thisFace = thisFace->nextFace)
    {
        //if (!thisFace->collisionon) continue;
        
        //translate the vertices
        p1 = *transform * thisFace->vert1;
        p2 = *transform * thisFace->vert2;
        p3 = *transform * thisFace->vert3;
        
        //check if the polygon is completely on one side of the box
        dist1 = bounds.minbx();
        if (p1.x<dist1 && p2.x<dist1 && p3.x<dist1) continue;
        dist1 = bounds.minby();
        if (p1.y<dist1 && p2.y<dist1 && p3.y<dist1) continue;
        dist1 = bounds.minbz();
        if (p1.z<dist1 && p2.z<dist1 && p3.z<dist1) continue;
            
        dist1 = bounds.maxbx();
        if (p1.x>dist1 && p2.x>dist1 && p3.x>dist1) continue;
        dist1 = bounds.maxby();
        if (p1.y>dist1 && p2.y>dist1 && p3.y>dist1) continue;
        dist1 = bounds.maxbz();
        if (p1.z>dist1 && p2.z>dist1 && p3.z>dist1) continue;
        
        //check which vertices of the triangle are inside the box
        count = 0;
        if (bounds.inBox(&p1)) count++;
        if (bounds.inBox(&p2)) count++;
        if (bounds.inBox(&p3)) count++;
        
        //3 inside = completely inside, collision
        //0 inside = might straddle box, need more checks
        //1 or 2   = must intersect box
        //if (count==3) continue;
        if (count>0) return true;
        
        //check the polygon edges for intersection with the box
        temp.Set(1/(p2.x-p1.x),1/(p2.y-p1.y),1/(p2.z-p1.z));
        if (bounds.intersectBox(&p1,&temp,&dist1) && dist1>=0) return true;
        
        temp.Set(1/(p3.x-p2.x),1/(p3.y-p2.y),1/(p3.z-p2.z));
        if (bounds.intersectBox(&p2,&temp,&dist1) && dist1>=0) return true;
        
        temp.Set(1/(p1.x-p3.x),1/(p1.y-p3.y),1/(p1.z-p3.z));
        if (bounds.intersectBox(&p3,&temp,&dist1) && dist1>=0) return true;
        
        //remaining case - triangle completely surrounds box, if so
        //box diagonals would intersect triangle
        
        //only check the box diagonals (4 of them)
        //get poly normal
		normal.Cross( p3 - p1, p2 - p1 );
        
        //check for 'point' triangles
        distv = normal.x*normal.x + normal.y*normal.y + normal.z*normal.z;
        if (distv<0.00001) continue;
        //calculate d
        distv = normal.x*p1.x + normal.y*p1.y + normal.z*p1.z;
        
        //work out the normal for each side of the poly, stored in temp1, temp2 and temp3
        temp.Cross( normal, p2 - p1 );
        dist1 = temp.x*p1.x + temp.y*p1.y + temp.z*p1.z;
        
		temp2.Cross( normal, p3 - p2 );
        dist2 = temp2.x*p2.x + temp2.y*p2.y + temp2.z*p2.z;
        
		temp3.Cross( normal, p1 - p3 );
        dist3 = temp3.x*p3.x + temp3.y*p3.y + temp3.z*p3.z;
        
        //do some precalculation
        b1 = bounds.minbx()*normal.x; b2 = bounds.minby()*normal.y; b3 = bounds.minbz()*normal.z;
        b4 = bounds.maxbx()*normal.x; b5 = bounds.maxby()*normal.y; b6 = bounds.maxbz()*normal.z;
        
        //diagonal 1
        d1 = b1 + b2 + b3 - distv;
        d2 = b4 + b5 + b6 - distv;
        if (d1*d2<0) {
            //find intersection point and check if inside triangle
            d3 = -d1/((b4-b1) + (b5-b2) + (b6-b3));
            intersect.x = bounds.minbx() + (bounds.maxbx()-bounds.minbx())*d3;
            intersect.y = bounds.minby() + (bounds.maxby()-bounds.minby())*d3;
            intersect.z = bounds.minbz() + (bounds.maxbz()-bounds.minbz())*d3;
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
            intersect.x = bounds.minbx() + (bounds.maxbx()-bounds.minbx())*d3;
            intersect.y = bounds.maxby() + (bounds.minby()-bounds.maxby())*d3;
            intersect.z = bounds.minbz() + (bounds.maxbz()-bounds.minbz())*d3;
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
            intersect.x = bounds.minbx() + (bounds.maxbx()-bounds.minbx())*d3;
            intersect.y = bounds.maxby() + (bounds.minby()-bounds.maxby())*d3;
            intersect.z = bounds.maxbz() + (bounds.minbz()-bounds.maxbz())*d3;
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
            intersect.x = bounds.minbx() + (bounds.maxbx()-bounds.minbx())*d3;
            intersect.y = bounds.minby() + (bounds.maxby()-bounds.minby())*d3;
            intersect.z = bounds.maxbz() + (bounds.minbz()-bounds.maxbz())*d3;
            d3 = temp.x*intersect.x + temp.y*intersect.y + temp.z*intersect.z - dist1;
            d4 = temp2.x*intersect.x + temp2.y*intersect.y + temp2.z*intersect.z - dist2;
            d5 = temp3.x*intersect.x + temp3.y*intersect.y + temp3.z*intersect.z - dist3;
            if (d3<-0.00001 && d4<-0.00001 && d5<-0.00001) return true;
        }
    }
    
    return false;
}

//@scale is the local box's scale amount - separated to preserve shape of remote box
//@scale2 is the scaling for @b
//check Box @b against the local box, transformed to local space using @transform
//check this Box against @b, transformed into @b space using @rTransform
int CollisionBox::collidesBox(Box* b, AGKVector* scale, AGKVector* scale2, AGKMatrix4* transform, AGKMatrix4* rTransform)
{
    int result = bounds.collidesBox(b,scale,transform);
    if (result<2) return result;
    else
    {
        result = b->collidesBox(&bounds,scale2,rTransform);
        if (result==1) return true; else return false;
    }
}

//@scale is local box's scale amount
//@scale2 is the sphere's max scale amount, take the largest so that the sphere is never 
//distorted but still surrounds the object
//sphere center transformed to local space using @transform
bool CollisionBox::collidesSphere(CollisionSphere* colSphere, AGKVector* scale, float scale2, AGKMatrix4* transform)
{
    AGKVector tCenter(colSphere->centerX,colSphere->centerY,colSphere->centerZ);
	tCenter.Mult( *transform );
    
    float largeRadius = colSphere->radius * scale2;
    
    float minx = bounds.minbx() * scale->x;
    float miny = bounds.minby() * scale->y;
    float minz = bounds.minbz() * scale->z;
    float maxx = bounds.maxbx() * scale->x;
    float maxy = bounds.maxby() * scale->y;
    float maxz = bounds.maxbz() * scale->z;
    
    float sqrDist = 0;
    
    //get the distance of sphere from the outside of the box
    if (tCenter.x > maxx) sqrDist = (tCenter.x - maxx)*(tCenter.x - maxx);
    else if (tCenter.x < minx) sqrDist = (minx - tCenter.x)*(minx - tCenter.x);
    
    if (tCenter.y > maxy) sqrDist += (tCenter.y - maxy)*(tCenter.y - maxy);
    else if (tCenter.y < miny) sqrDist += (miny - tCenter.y)*(miny - tCenter.y);
    
    if (tCenter.z > maxz) sqrDist += (tCenter.z - maxz)*(tCenter.z - maxz);
    else if (tCenter.z < minz) sqrDist += (minz - tCenter.z)*(minz - tCenter.z);
    
    if (sqrDist > (largeRadius*largeRadius)) return false;
    
    //request for sphere and box objects to be solid, instead of hollow
/*    
    //check if box is inside sphere
    float bCenterX = (minx+maxx)/2.0;
    float bCenterY = (miny+maxy)/2.0;
    float bCenterZ = (minz+maxz)/2.0;
    
    //find the box corner furthest from the sphere center
    if (tCenter.x<bCenterX) bCenterX = maxx; else bCenterX = minx;
    if (tCenter.y<bCenterY) bCenterY = maxy; else bCenterY = miny;
    if (tCenter.z<bCenterZ) bCenterZ = maxz; else bCenterZ = minz;
    
    //check this distance againt the radius
    if ((tCenter.x-bCenterX)*(tCenter.x-bCenterX) 
      + (tCenter.y-bCenterY)*(tCenter.y-bCenterY) 
      + (tCenter.z-bCenterZ)*(tCenter.z-bCenterZ) < (largeRadius*largeRadius)) return false;
    
    //check if sphere is inside box
    if (tCenter.x + largeRadius < maxx
     && tCenter.x - largeRadius > minx
     && tCenter.y + largeRadius < maxy
     && tCenter.y - largeRadius > miny
     && tCenter.z + largeRadius < maxz
     && tCenter.z - largeRadius > minz) return false;
*/    
    return true;
}

/*
bool CollisionBox::SaveObject( FILE *pFile )
{
	unsigned char iBoxVersion = 1;
	fwrite( &iBoxVersion, 1,1, pFile );

	fwrite( &mainRadius, 4,1, pFile );
	float value = bounds.minbx(); fwrite( &value, 4,1, pFile );
	value = bounds.minby(); fwrite( &value, 4,1, pFile );
	value = bounds.minbz(); fwrite( &value, 4,1, pFile );
	value = bounds.maxbx(); fwrite( &value, 4,1, pFile );
	value = bounds.maxby(); fwrite( &value, 4,1, pFile );
	value = bounds.maxbz(); fwrite( &value, 4,1, pFile );

	return true;
}

bool CollisionBox::LoadObject( FILE *pFile )
{
	unsigned char iBoxVersion = 0;
	fread( &iBoxVersion, 1,1, pFile );

	if ( iBoxVersion == 1 )
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
	}
	else
	{
		return false;
	}

	return true;
}
*/