#include "TreeNode.h"

using namespace AGK;

TreeNode::~TreeNode()
{
    if (right==0)
    {
        Face* temp;
        
        while(left!=0)
        {
            temp = (Face*)left;
            left = (TreeNode*)(temp->nextFace);
            delete (Face*)temp;
        }
    }
    else
    {
        delete left;
        delete right;
    }
}

void TreeNode::drawBounds(int freeObj, int tempObj, int mesh, int* limb)
{
    /*
    float difx = bounds.maxbx()-bounds.minbx();
    float dify = bounds.maxby()-bounds.minby();
    float difz = bounds.maxbz()-bounds.minbz();

    float posx = (bounds.maxbx()+bounds.minbx())/2.0f;
    float posy = (bounds.maxby()+bounds.minby())/2.0f;
    float posz = (bounds.maxbz()+bounds.minbz())/2.0f;
     */
    
	/*
    MakeObjectBox(tempObj,difx,dify,difz);
    MakeMeshFromObject(mesh,tempObj);
    AddLimb(freeObj,*limb,mesh);
    OffsetLimb(freeObj,*limb,posx,posy,posz);
    DeleteMesh(mesh);
    DeleteDBObject(tempObj);
    
    (*limb)++;
    
    if (right!=0)
    {
        left->drawBounds(freeObj,tempObj,mesh,limb);
        right->drawBounds(freeObj,tempObj,mesh,limb);
    }
	*/
}
     
void TreeNode::buildTree(Face* faces,int facesPerNode)
{
    left=0;
    right=0;
    unsigned fnum = 0;

	if ( !faces )
	{
		bounds.set(0,0,0,0,0,0);
		return;
	}

    //sprintf_s(str,255,"Creating Node... %d",fnum);
    //MessageBox(NULL, str, "Tree", MB_OK);
    
    //build a bounding box for this node
    float minx = faces->vert1.x;
    float miny = faces->vert1.y;
    float minz = faces->vert1.z;
    float maxx = faces->vert1.x;
    float maxy = faces->vert1.y;
    float maxz = faces->vert1.z;
    
	for(Face* thisFace=faces; thisFace!=0; thisFace = thisFace->GetNextFace()) 
    {
        if (thisFace->vert1.x>maxx) maxx = thisFace->vert1.x;
        if (thisFace->vert1.x<minx) minx = thisFace->vert1.x;
        if (thisFace->vert1.y>maxy) maxy = thisFace->vert1.y;
        if (thisFace->vert1.y<miny) miny = thisFace->vert1.y;
        if (thisFace->vert1.z>maxz) maxz = thisFace->vert1.z;
        if (thisFace->vert1.z<minz) minz = thisFace->vert1.z;
        
        if (thisFace->vert2.x>maxx) maxx = thisFace->vert2.x;
        if (thisFace->vert2.x<minx) minx = thisFace->vert2.x;
        if (thisFace->vert2.y>maxy) maxy = thisFace->vert2.y;
        if (thisFace->vert2.y<miny) miny = thisFace->vert2.y;
        if (thisFace->vert2.z>maxz) maxz = thisFace->vert2.z;
        if (thisFace->vert2.z<minz) minz = thisFace->vert2.z;
        
        if (thisFace->vert3.x>maxx) maxx = thisFace->vert3.x;
        if (thisFace->vert3.x<minx) minx = thisFace->vert3.x;
        if (thisFace->vert3.y>maxy) maxy = thisFace->vert3.y;
        if (thisFace->vert3.y<miny) miny = thisFace->vert3.y;
        if (thisFace->vert3.z>maxz) maxz = thisFace->vert3.z;
        if (thisFace->vert3.z<minz) minz = thisFace->vert3.z;

		fnum++;
    }
    
    bounds.set(minx,miny,minz,maxx,maxy,maxz);
    bounds.correctBox();
    
    //sprintf_s(str,255,"Building Tree... %5.3f,%5.3f,%5.3f,%5.3f,%5.3f,%5.3f",maxx,maxy,maxz,minx,miny,minz);
    //MessageBox(NULL, str, "Intersect Object", MB_OK);
    
    //if this is a leaf node then place the linked list into this node's pointer
    if ((int)fnum<=facesPerNode)
    {
        left = (TreeNode*)faces;
        return;
    }
    
    //otherwise divide the linked list into two new lists
    float difx = maxx-minx;
    float dify = maxy-miny;
    float difz = maxz-minz;
    
    //x=0, y=1, z=2
    //axis is biggest extent, axis1 is second, axis2 is smallest
    int axis = 0;
    int axis1 = 0;
    int axis2 = 0;
    
    if (difx>dify)
    {
        if (difx<difz) { axis = 2; axis1 = 0; axis2 = 1; }
        else
        {
            axis = 0;
            if (dify>difz) { axis1 = 1; axis2 = 2; }
            else { axis1 = 2; axis2 = 1; }
        } 
    }
    else
    {
        if (dify<difz) { axis = 2; axis1 = 1; axis2 = 0; }
        else
        {
            axis = 1;
            if (difx>difz) { axis1 = 0; axis2 = 2; }
            else { axis1 = 2; axis2 = 0; }
        }
    }
    //axes stored for later when handling unequal splits
    
    float limit = 0;
    Face* leftFaces = 0; //new linked list 1
    Face* rightFaces = 0; //new linked list 2
    Face* lastFace = 0; //points to the last element in the new leftFaces list, helps reconstruct the whole list
    Face* temp = 0;
    unsigned fnum1=0, fnum2=0;
    
    //limit for deciding which side of the split a triangle belongs                                                                            
    switch(axis)
    {
        case 0 : limit=difx/2.0f + minx;break;
        case 1 : limit=dify/2.0f + miny;break;
        case 2 : limit=difz/2.0f + minz;break;
    }
    
    float avg;
    float total = 0;
            
    while (faces)
    {
        //get the triangles average distance along this axis and compare with the limit
        switch(axis)
        {
            case 0 : avg = (faces->vert1.x + faces->vert2.x + faces->vert3.x)/3.0f;break;
            case 1 : avg = (faces->vert1.y + faces->vert2.y + faces->vert3.y)/3.0f;break;
            case 2 : avg = (faces->vert1.z + faces->vert2.z + faces->vert3.z)/3.0f;break;
        }
            
        total += avg;
                
        if (avg>=limit)
        {
            if (fnum1==0) lastFace = faces;
            
            //pull the first item from the @faces list and attach to the front of @leftFaces
			temp = faces->GetNextFace();
            faces->nextFace = leftFaces;
            leftFaces = faces;
            faces = temp;
            
            fnum1++;
        }
        else
        {
            temp = (Face*)faces->GetNextFace();
            faces->nextFace = rightFaces;
            rightFaces = faces;
            faces = temp;
            
            fnum2++;
        }
    }
    
    //sprintf_s(str,255,"Attempt 1: %d, %d, %d, %d",fnum,fnum1,fnum2,axis);
    //MessageBox(NULL, str, "Debug", MB_OK);
    
    //check for an uneven split - more than 90% of the triangles on one side
    //if so then try changing axis
    if (abs((int) (fnum1-fnum2))>fnum*0.8)
    {
        fnum1=0;
        fnum2=0;
        limit=0;
        
        //check the second largest axis
        switch(axis1)
        {
            case 0 : limit=difx/2.0f + minx;break;
            case 1 : limit=dify/2.0f + miny;break;
            case 2 : limit=difz/2.0f + minz;break;
        }
        
        //reconstruct the list, if none in @leftFaces, already done in @rightFaces
        //otherwise use @lastFace to append the two together
        if (leftFaces)
        {
            faces = leftFaces;
            lastFace->nextFace = rightFaces;
        }
        else faces = rightFaces;
        
        leftFaces = 0;
        rightFaces = 0;
        
        while (faces)
        {
            switch(axis1)
            {
                case 0 : avg = (faces->vert1.x + faces->vert2.x + faces->vert3.x)/3.0f;break;
                case 1 : avg = (faces->vert1.y + faces->vert2.y + faces->vert3.y)/3.0f;break;
                case 2 : avg = (faces->vert1.z + faces->vert2.z + faces->vert3.z)/3.0f;break;
            }
                
            if (avg>=limit)
            {
                if (fnum1==0) lastFace = faces;
                
                temp = faces->GetNextFace();
                faces->nextFace = leftFaces;
                leftFaces = faces;
                faces = temp;
                
                fnum1++;
            }
            else
            {
                temp = faces->GetNextFace();
                faces->nextFace = rightFaces;
                rightFaces = faces;
                faces = temp;
                
                fnum2++;
            }
        }
        
        //sprintf_s(str,255,"Attempt 2: %d, %d, %d, %d",fnum,fnum1,fnum2,axis1);
        //MessageBox(NULL, str, "Debug", MB_OK);
    }
    
    //if we still don't get a decent split try the final axis
    if (abs((int) (fnum1-fnum2))>fnum*0.8)
    {
        fnum1=0;
        fnum2=0;
        
        //check the third largest axis
        switch(axis2)
        {
            case 0 : limit=difx/2.0f + minx;break;
            case 1 : limit=dify/2.0f + miny;break;
            case 2 : limit=difz/2.0f + minz;break;
        }
        
        if (leftFaces)
        {
            faces = leftFaces;
            lastFace->nextFace = rightFaces;
        }
        else faces = rightFaces;
        
        leftFaces = 0;
        rightFaces = 0;
        
        while (faces)
        {
            switch(axis2)
            {
                case 0 : avg = (faces->vert1.x + faces->vert2.x + faces->vert3.x)/3.0f;break;
                case 1 : avg = (faces->vert1.y + faces->vert2.y + faces->vert3.y)/3.0f;break;
                case 2 : avg = (faces->vert1.z + faces->vert2.z + faces->vert3.z)/3.0f;break;
            }
                
            if (avg>=limit)
            {
                if (fnum1==0) lastFace = faces;
                
                temp = faces->GetNextFace();
                faces->nextFace = leftFaces;
                leftFaces = faces;
                faces = temp;
                
                fnum1++;
            }
            else
            {
                temp = faces->GetNextFace();
                faces->nextFace = rightFaces;
                rightFaces = faces;
                faces = temp;
                
                fnum2++;
            }
        }
        
        //sprintf_s(str,255,"Attempt 3: %d, %d, %d, %d",fnum,fnum1,fnum2,axis2);
        //MessageBox(NULL, str, "Debug", MB_OK);
    }
    
    //if we get another uneven split then split based on the average distance
    //along the original axis
    if (abs((int) (fnum1-fnum2))>fnum*0.8)
    {
        fnum1=0;
        fnum2=0;
        limit = total/fnum;
        
        if (leftFaces)
        {
            faces = leftFaces;
            lastFace->nextFace = rightFaces;
        }
        else faces = rightFaces;
        
        leftFaces = 0;
        rightFaces = 0;
        
        while (faces)
        {
            switch(axis)
            {
                case 0 : avg = (faces->vert1.x + faces->vert2.x + faces->vert3.x)/3.0f;break;
                case 1 : avg = (faces->vert1.y + faces->vert2.y + faces->vert3.y)/3.0f;break;
                case 2 : avg = (faces->vert1.z + faces->vert2.z + faces->vert3.z)/3.0f;break;
            }
                
            if (avg>=limit)
            {
                if (fnum1==0) lastFace = faces;
                
                temp = faces->GetNextFace();
                faces->nextFace = leftFaces;
                leftFaces = faces;
                faces = temp;
                
                fnum1++;
            }
            else
            {
                temp = faces->GetNextFace();
                faces->nextFace = rightFaces;
                rightFaces = faces;
                faces = temp;
                
                fnum2++;
            }
        }
        
        //sprintf_s(str,255,"Attempt 4: %d, %d, %d, %d",fnum,fnum1,fnum2,axis);
        //MessageBox(NULL, str, "Debug", MB_OK);
    }
                
    //If this still doesn't produce a decent split, split them randomly
    if (abs((int) (fnum1-fnum2))>fnum*0.9)
    {
        fnum1=0;
        fnum2=0;
        
        if (leftFaces)
        {
            faces = leftFaces;
            lastFace->nextFace = rightFaces;
        }
        else faces = rightFaces;
        
        leftFaces = 0;
        rightFaces = 0;
        
        int flag = 1;
        
        while (faces)
        {
            if (flag)
            {
                temp = faces->GetNextFace();
                faces->nextFace = leftFaces;
                leftFaces = faces;
                faces = temp;
                
                fnum1++;
                flag = 0;
            }
            else
            {
                temp = faces->GetNextFace();
                faces->nextFace = rightFaces;
                rightFaces = faces;
                faces = temp;
                
                fnum2++;
                flag = 1;
            }
        }
        
        //sprintf_s(str,255,"Random: %d, %d, %d",fnum,fnum1,fnum2);
        //MessageBox(NULL, str, "Debug", MB_OK);
    }
    
    left = new TreeNode();
    right = new TreeNode();
    
//    sprintf_s(str,255,"Done");
//    MessageBox(NULL, str, "Collision Setup Info", MB_OK); 
                
    left->buildTree(leftFaces,facesPerNode);
    right->buildTree(rightFaces,facesPerNode);
}

bool TreeNode::intersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, CollisionResults* cRes)
{
    float dist = 0;
    
    if (bounds.intersectBox(p,vi,&dist)==0) return false;
    
    //attempt a quick reject if a collision point has already been found that would always be closer
	if (dist>0 && !cRes->isValidDistanceSqr(dist*dist*v->SqrLength())) return false;
                        
    bool answer=false;
    
    //if not a leaf node then simply pass ray details to the two child nodes
    if (right!=0)
    {
        answer = left->intersects(p,v,vn,vi,cRes);
        answer = answer | (right->intersects(p,v,vn,vi,cRes));
    }
    else
    {
        //else cycle through all the faces in it
        for(Face* thisFace=(Face*)left; thisFace!=0; thisFace=thisFace->GetNextFace())
        {
            answer = answer | (thisFace->intersects(p,v,vn,cRes));
        }
    }
                        
    return answer; 
}

bool TreeNode::sphereIntersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, float rRadius, AGKVector* scale, CollisionResults* cRes)
{
    float dist = 0;
    
    if (!bounds.quickSphereIntersectBox(p,vi,rRadius,scale,&dist)) return false;
    
    //if (!cRes->isValidDistance(dist*v->length)) return false;
                        
    bool answer=false;
                        
    if (right!=0)
    {
        answer = left->sphereIntersects(p,v,vn,vi,rRadius,scale,cRes);
        answer = answer | (right->sphereIntersects(p,v,vn,vi,rRadius,scale,cRes));
    }
    else
    {                
        for(Face* thisFace=(Face*)left; thisFace!=0; thisFace=thisFace->GetNextFace())
        {
            answer = answer | (thisFace->sphereIntersects(p,v,vn,rRadius,scale,cRes));
        }
    }
                        
    return answer; 
}
    
bool TreeNode::collidesPoly(AGKVector* p1, AGKVector* p2, AGKVector* p3, AGKVector* max, AGKVector* min)
{
    //check if triangle is comlpetely on one side of the node
    if (max->x < bounds.minbx()) return false;
    if (max->y < bounds.minby()) return false;
    if (max->z < bounds.minbz()) return false;
    
    if (min->x > bounds.maxbx()) return false;
    if (min->y > bounds.maxby()) return false;
    if (min->z > bounds.maxbz()) return false;
    
    if (right!=0)
    {
        if (left->collidesPoly(p1,p2,p3,max,min)) return true;
        return right->collidesPoly(p1,p2,p3,max,min);
    }
    else
    {        
        for(Face* thisFace2=(Face*)left; thisFace2!=0; thisFace2=thisFace2->GetNextFace())
        {
            if (thisFace2->collidesPoly(p1,p2,p3)) return true;
        }
    }
    
    return false;
}

bool TreeNode::collidesSphere(AGKVector* center, float sRadius, AGKVector* scale)
{
    {
        float minx = bounds.minbx() * scale->x;
        float miny = bounds.minby() * scale->y;
        float minz = bounds.minbz() * scale->z;
        float maxx = bounds.maxbx() * scale->x;
        float maxy = bounds.maxby() * scale->y;
        float maxz = bounds.maxbz() * scale->z;
        
        float sqrDist = 0;
        
        //get the distance of sphere from the outside of the box
        if (center->x > maxx) sqrDist = (center->x - maxx)*(center->x - maxx);
        else if (center->x < minx) sqrDist = (minx - center->x)*(minx - center->x);
        
        if (center->y > maxy) sqrDist += (center->y - maxy)*(center->y - maxy);
        else if (center->y < miny) sqrDist += (miny - center->y)*(miny - center->y);
        
        if (center->z > maxz) sqrDist += (center->z - maxz)*(center->z - maxz);
        else if (center->z < minz) sqrDist += (minz - center->z)*(minz - center->z);
        
        if (sqrDist > (sRadius*sRadius)) return false;
        
        //check if box is completely inside sphere
        float bCenterX = (minx+maxx)/2.0f;
        float bCenterY = (miny+maxy)/2.0f;
        float bCenterZ = (minz+maxz)/2.0f;
        
        //find the box corner furthest from the sphere center
        if (center->x<bCenterX) bCenterX = maxx; else bCenterX = minx;
        if (center->y<bCenterY) bCenterY = maxy; else bCenterY = miny;
        if (center->z<bCenterZ) bCenterZ = maxz; else bCenterZ = minz;
        
        //check this distance againt the radius
        if ((center->x-bCenterX)*(center->x-bCenterX) 
          + (center->y-bCenterY)*(center->y-bCenterY) 
          + (center->z-bCenterZ)*(center->z-bCenterZ) < (sRadius*sRadius)) return true;
    }
    
    if (right!=0)
    {
        if (left->collidesSphere(center,sRadius,scale)) return true;
        return right->collidesSphere(center,sRadius,scale);
    }
    else
    {        
        for(Face* thisFace=(Face*)left; thisFace!=0; thisFace=thisFace->GetNextFace())
        {
            if (thisFace->collidesSphere(center,sRadius,scale)) return true;
        }
    }
    
    return false;
}

bool TreeNode::collidesBox(Box* b, AGKVector* scale, AGKVector* scale2, AGKMatrix4* transform, AGKMatrix4* rTransform)
{
    {
        //result - 0 = bounds is outside, 1 = inside, 2 = don't know
        int result = b->roughBox(&bounds,scale2,rTransform);
        if (result<2) return (result != 0);
        result = bounds.roughBox(b,scale,transform);
        //if result = 1 (inside) box may still not collide with triangles inside the node
        if (result==0) return false;
    }
    
    if (right!=0)
    {
        if (left->collidesBox(b,scale,scale2,transform,rTransform)) return true;
        return right->collidesBox(b,scale,scale2,transform,rTransform);
    }
    else
    {   
        AGKVector p1,p2,p3;
        Box scaledBox(b->minbx()*scale2->x,
                      b->minby()*scale2->y,
                      b->minbz()*scale2->z,
                      b->maxbx()*scale2->x,
                      b->maxby()*scale2->y,
                      b->maxbz()*scale2->z);
                      
        for(Face* thisFace=(Face*)left; thisFace!=0; thisFace=thisFace->GetNextFace())
        {
            p1 = *rTransform * thisFace->vert1;
            p2 = *rTransform * thisFace->vert2;
            p3 = *rTransform * thisFace->vert3;
            if (scaledBox.collidesFace(&p1,&p2,&p3)) return true;
        }
    }
    
    return false;
}

bool TreeNode::collidesTree(TreeNode* node, AGKVector* scale, AGKVector* scale2, AGKMatrix4* transform, AGKMatrix4* rTransform)
{
    {
        Box* b = node->getBounds();
        //result - 0 = bounds is outside, 1 = inside, 2 = don't know
        int result = b->roughBox(&bounds,scale2,rTransform);
        if (result==0) return false;
        
        //if this node is completely inside @node, switch trees and traverse the other one
        if (result==1) return node->collidesTree(this,scale2,scale,rTransform,transform);

        result = bounds.roughBox(b,scale,transform);
        if (result==0) return false;
    }
    
    if (right!=0)
    {
        if (left->collidesTree(node,scale,scale2,transform,rTransform)) return true;
        return right->collidesTree(node,scale,scale2,transform,rTransform);
    }
    else
    {   
        AGKVector p1,p2,p3,min,max;
        float iScaleX = 1.0f/scale2->x;
        float iScaleY = 1.0f/scale2->y;
        float iScaleZ = 1.0f/scale2->z;
                      
        for(Face* thisFace=(Face*)left; thisFace!=0; thisFace=thisFace->GetNextFace())
        {
            p1 = *rTransform * thisFace->vert1;
            p2 = *rTransform * thisFace->vert2;
            p3 = *rTransform * thisFace->vert3;
            
            p1.Scale(iScaleX, iScaleY, iScaleZ);
            p2.Scale(iScaleX, iScaleY, iScaleZ);
            p3.Scale(iScaleX, iScaleY, iScaleZ);
            
            max.x = p1.x; max.y = p1.y; max.z = p1.z;
            min.x = p1.x; min.y = p1.y; min.z = p1.z;
            
            if (p2.x>max.x) max.x = p2.x; else min.x = p2.x;
            if (p3.x>max.x) max.x = p3.x; else if (p3.x<min.x) min.x = p3.x;
            
            if (p2.y>max.y) max.y = p2.y; else min.y = p2.y;
            if (p3.y>max.y) max.y = p3.y; else if (p3.y<min.y) min.y = p3.y;
            
            if (p2.z>max.z) max.z = p2.z; else min.z = p2.z;
            if (p3.z>max.z) max.z = p3.z; else if (p3.z<min.z) min.z = p3.z;
            
            if (node->collidesPoly(&p1,&p2,&p3,&max,&min)) return true;
        }
    }

    return false;
}

/*
bool TreeNode::SaveNode( FILE *pFile )
{
	//version 1
	float value = bounds.minbx(); fwrite( &value, 4,1, pFile );
	value = bounds.minby(); fwrite( &value, 4,1, pFile );
	value = bounds.minbz(); fwrite( &value, 4,1, pFile );
	value = bounds.maxbx(); fwrite( &value, 4,1, pFile );
	value = bounds.maxby(); fwrite( &value, 4,1, pFile );
	value = bounds.maxbz(); fwrite( &value, 4,1, pFile );

	int iType = right ? 0 : 1;	//0 = node, 1 = leaf
	fwrite( &iType, 1,1, pFile );

	if ( right )
	{
		left->SaveNode( pFile );
		right->SaveNode( pFile );
	}
	else
	{
		int count = 0;
		Face *pFace = (Face*) left;
		while ( pFace )
		{
			count++;
			pFace = pFace->GetNextFace();
		}
		
		fwrite( &count, 4,1, pFile );
		
		pFace = (Face*) left;
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
	}

	return true;
}

bool TreeNode::LoadNode( FILE *pFile, int iVersion )
{
	if ( iVersion == 1 )
	{
		float minx, miny, minz, maxx, maxy, maxz;
		fread( &minx, 4,1, pFile );
		fread( &miny, 4,1, pFile );
		fread( &minz, 4,1, pFile );
		fread( &maxx, 4,1, pFile );
		fread( &maxy, 4,1, pFile );
		fread( &maxz, 4,1, pFile );
		bounds.set( minx, miny, minz, maxx, maxy, maxz );

		int iType = 0;
		fread( &iType, 1,1, pFile );
		if ( iType == 0 )
		{
			//node
			left = new TreeNode();
			right = new TreeNode();
			left->LoadNode( pFile, iVersion );
			right->LoadNode( pFile, iVersion );
		}
		else
		{
			//faces
			Face *pLast = 0, *pCurr = 0;
			int count = 0;
			fread( &count, 4,1, pFile );	//number of faces
			left = 0;
			right = 0;

			for ( int i = 0; i < count; i++ )
			{
				pCurr = new Face();
				pCurr->nextFace = 0;

				//tail add, to preverse linked list order
				if ( pLast ) pLast->nextFace = pCurr;
				else left = (TreeNode*)pCurr;

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
	}
	
	return true;
}
*/
