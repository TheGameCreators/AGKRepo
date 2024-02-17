#include "CollisionTree.h"
#include <math.h>

using namespace AGK;

void CollisionTree::drawBounds(int freeObj)
{
	/*
    //make dummy root object, nodes will be limbs
    MakeObjectBox(freeObj,0,0,0);
    
    bool found = false;
    int tempObj = 65000;
    while (tempObj>1 && !found)
    {
        if (ObjectExist(tempObj)==0) found = true;
        tempObj--;
    }
    
    if (!found) return;
    
    found = false;
    int mesh = 65000;
    while (mesh>1 && !found)
    {
        if (MeshExist(mesh)==0) found = true;
        mesh--;
    }
    
    if (!found) return;
    
    int limb = 1;
    
    tree->drawBounds(freeObj,tempObj+1,mesh+1,&limb);
	*/
}
        
void CollisionTree::makeCollisionObject(Face *pFaces)
{
	float newRadius;
    mainRadius = 0;

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

		pFace = pFace->GetNextFace();
    }

	mainRadius = (float) sqrt( mainRadius );

	//sprintf_s( str, 256, "%f", mainRadius );
	//MessageBox( NULL, str, "Info", 0 );

	if (facesPerNode<2) facesPerNode = 2;
    
    tree = new TreeNode(); 
    tree->buildTree((Face*)pFaces,facesPerNode);
}

bool CollisionTree::intersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, CollisionResults* cRes)
{              
    return (tree->intersects(p,v,vn,vi,cRes));
}

bool CollisionTree::sphereIntersects(AGKVector* p, AGKVector* v, AGKVector* vn, AGKVector* vi, float rRadius, AGKVector* scale, CollisionResults* cRes)
{
    return (tree->sphereIntersects(p,v,vn,vi,rRadius,scale,cRes));
}

bool CollisionTree::collidesPoly(Face* obj2Faces, AGKMatrix4* transform)
{
    AGKVector p1, p2, p3, max, min;
    
    for(Face* thisFace=obj2Faces; thisFace!=0; thisFace=thisFace->nextFace)
    {
        //if (!thisFace->collisionon) continue;
        
        p1 = *transform * thisFace->vert1;
        p2 = *transform * thisFace->vert2;
        p3 = *transform * thisFace->vert3;
        
        max.x = p1.x; max.y = p1.y; max.z = p1.z;
        min.x = p1.x; min.y = p1.y; min.z = p1.z;
        
        if (p2.x>max.x) max.x = p2.x; else min.x = p2.x;
        if (p3.x>max.x) max.x = p3.x; else if (p3.x<min.x) min.x = p3.x;
        
        if (p2.y>max.y) max.y = p2.y; else min.y = p2.y;
        if (p3.y>max.y) max.y = p3.y; else if (p3.y<min.y) min.y = p3.y;
        
        if (p2.z>max.z) max.z = p2.z; else min.z = p2.z;
        if (p3.z>max.z) max.z = p3.z; else if (p3.z<min.z) min.z = p3.z;
        
        if (tree->collidesPoly(&p1,&p2,&p3,&max,&min)) return true;
    }
    
    return false;
}

bool CollisionTree::collidesSphere(CollisionSphere* colSphere, AGKVector* scale, float scale2, AGKMatrix4* transform)
{
    AGKVector tCenter(colSphere->centerX, colSphere->centerY, colSphere->centerZ);
	tCenter.Mult( *transform );
    float sRadius = colSphere->radius*scale2;
    
    return tree->collidesSphere(&tCenter, sRadius, scale);
}

bool CollisionTree::collidesBox(Box* b, AGKVector* scale, AGKVector* scale2, AGKMatrix4* transform, AGKMatrix4* rTransform)
{
    return tree->collidesBox(b, scale, scale2, transform, rTransform);
}

bool CollisionTree::collidesTree(TreeNode* colTree, AGKVector* scale, AGKVector* scale2, AGKMatrix4* transform, AGKMatrix4* rTransform)
{
    return tree->collidesTree(colTree, scale, scale2, transform, rTransform);
}

/*
bool CollisionTree::SaveObject( FILE *pFile )
{
	unsigned char iTreeVersion = 1;
	fwrite( &iTreeVersion, 1,1, pFile );

	fwrite( &mainRadius, 4,1, pFile );
	fwrite( &facesPerNode, 4,1, pFile );

	//recursively save the nodes, includes the faces
	if ( tree ) return tree->SaveNode( pFile );

	return true;
}

bool CollisionTree::LoadObject( FILE *pFile )
{
	unsigned char iTreeVersion = 0;
	fread( &iTreeVersion, 1,1, pFile );

	if ( iTreeVersion == 1 )
	{
		fread( &mainRadius, 4,1, pFile );
		fread( &facesPerNode, 4,1, pFile );
		
		tree = new TreeNode();
		tree->LoadNode( pFile, iTreeVersion );
	}

	return true;
}
*/
