
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

bool AGKToBullet::AssertValidPhysicsWorld()
{
	if ( DynamicsWorldArray.size() == 0 )
	{
		agk::Error( "The physics world does not exist "  );
		return false;
	}
	return true;
}

 bool AGKToBullet::AssertValidObject( uint32_t objID, const char* message ) 
{
	if(agk::GetObjectExists( objID ) == false)
	{
		agk::Error( message );
		return false;
	}
	return true;
}

bool AGKToBullet::AssertValidBody( btRigidBody* body, const char* message )
{
	if ( body == NULL )
	{
		agk::Error( message );
		return false;
	}
	return true;
}

bool AGKToBullet::AssertValidJoint( btTypedConstraint* constraint, const char* message )
{
	if ( constraint == NULL )
	{
		agk::Error( message );
		return false;
	}
	return true;
}

bool AGKToBullet::AssertValidJointID(int jointID, const char* message)
{
	if(jointManager.GetItem(jointID)==NULL)
	{
		agk::Error(message);
		return false;
	}
	return true;
}

bool AGKToBullet::AssertValidPlaneID( int planeID, const char* message )
{
	if ( staticPlaneManager.GetItem( planeID ) == NULL )
	{
		agk::Error( message );
		return false;
	}
	else if ( ( ( StaticPlane* )staticPlaneManager.GetItem( planeID ) )->GetPlane() == NULL )
	{
		agk::Error( message );
		return false;
	}
	return true;
}

bool AGKToBullet::AssertValidVectorID( int vectorID, const char* message )
{
	if ( vectorManager.GetItem( vectorID ) == NULL )
	{
		agk::Error( message );
		return false;
	}
	else if ( ( ( Vector* )vectorManager.GetItem( vectorID ) )->GetAGKVector() == NULL )
	{
		agk::Error( message );
		return false;
	}
	return true;
}

bool AGKToBullet::AssertValidRayID( int rayID, const char* message )
{
	if ( rayManager.GetItem( rayID ) == NULL )
	{
		agk::Error( message );
		return false;
	}
	else if ( rayManager.GetItem( rayID ) == NULL )
	{
		agk::Error( message );
		return false;
	}
	return true;
}

bool AGKToBullet::AssertValidCharacterController( int ControllerID, const char* message )
{
	if ( characterControllerManager.GetItem( ControllerID ) == NULL )
	{
		agk::Error( message );
		return false;
	}
	else if ( characterControllerManager.GetItem( ControllerID ) == NULL )
	{
		agk::Error( message );
		return false;
	}
	return true;
}

btVector3 AGKToBullet::GetObjectSize( uint32_t objID )
{
	cObject3D* pObject = agk::GetObjectPtr(objID);
	btVector3 objectScale = AGKToBullet::GetObjectScale( objID );
	
	Face *pAllFaces = 0;
	Face *pLast = 0;
	for( uint32_t i = 0; i < pObject->GetNumMeshes(); i++ )
	{
		if (!pObject->GetMesh(i)->GetCollision()) {
			continue; //PE: Disable mesh collision.
		}

		Face *pMeshFaces = pObject->GetMesh(i)->GetFaceList( &pLast );
		if ( pMeshFaces && pLast ) 
		{
			// append mesh faces to cumulative list
			pLast->nextFace = pAllFaces;
			pAllFaces = pMeshFaces;
		}
	}

	if ( pAllFaces == 0 ) return btVector3( 1,1,1 );

	Face *pFace = pAllFaces;
	
	btVector3 minVector = AGKToBullet::GetBtVector3(pFace->vert1)  * objectScale;
	btVector3 maxVector =  AGKToBullet::GetBtVector3(pFace->vert1)  * objectScale;

	int vertexIndex = 0;
	while ( pFace )
    {
		btVector3 vertex1 = AGKToBullet::GetBtVector3(pFace->vert1)  * objectScale;
		btVector3 vertex2 = AGKToBullet::GetBtVector3(pFace->vert2)  * objectScale;
		btVector3 vertex3 = AGKToBullet::GetBtVector3(pFace->vert3)  * objectScale;

		minVector.setMin(vertex1);
		minVector.setMin(vertex2);
		minVector.setMin(vertex3);

		maxVector.setMax(vertex1);
		maxVector.setMax(vertex2);
		maxVector.setMax(vertex3);

		pFace = pFace->GetNextFace();
    }

	// mike - 011221 - this list never got deleted and ended up causing a bad memory leak
	// have added this to loop through all elements and wipe them out
	pFace = pAllFaces;

	while ( pFace )
	{
		Face* p = pFace;
		pFace = pFace->GetNextFace ( );
		delete p;
	}

	return maxVector - minVector;
}

btScalar AGKToBullet::GetObjectDiameter( uint32_t objID )
{
	btVector3 sizeVec = GetObjectSize( objID );
	btScalar diameter = sizeVec.getX();
	if(diameter < sizeVec.getY())
		diameter = sizeVec.getY();
	else if (diameter < sizeVec.getZ())
		diameter = sizeVec.getZ();
	return diameter;
}

//Note: Transform does not include  object scaling.
btTransform AGKToBullet::GetTransform(uint32_t objID, btScalar scaleFactor/* = 1.0*/)
{
	btTransform objTransform;
	objTransform.setIdentity();
	objTransform.setOrigin(btVector3(agk::GetObjectX(objID)/scaleFactor, agk::GetObjectY(objID)/scaleFactor, agk::GetObjectZ(objID)/scaleFactor));

	//Using a matrix it takes it ZYX
	btMatrix3x3 btMatrix;
	btMatrix.setEulerYPR(btScalar(btRadians(agk::GetObjectAngleZ(objID))),
										btScalar(btRadians(agk::GetObjectAngleY(objID))),
										btScalar(btRadians(agk::GetObjectAngleX(objID))));
	objTransform.setBasis(btMatrix);
	return objTransform;
}

btVector3 AGKToBullet::GetObjectScale(uint32_t objID)
{
	cObject3D* pObject = agk::GetObjectPtr(objID);
	return btVector3( pObject->GetScaleX(), pObject->GetScaleY(), pObject->GetScaleZ() );
}

AGKToBullet::AGKVertexData* AGKToBullet::GetVertexData( uint32_t objID, btScalar scaleFactor, bool bTransform )
{
	AGKToBullet::AGKVertexData* vertexData = new AGKToBullet::AGKVertexData();
	//We get a pointer to the AGK Object
	cObject3D* pObject = agk::GetObjectPtr(objID);
	int prevIndexCount = 0;
	btTransform objectTrans = AGKToBullet::GetTransform(objID);
	btVector3 objectScale = AGKToBullet::GetObjectScale(objID);
	
	Face *pAllFaces = 0;
	Face *pLast = 0;
	for( uint32_t i = 0; i < pObject->GetNumMeshes(); i++ )
	{
		if (!pObject->GetMesh(i)->GetCollision()) {
			continue; //PE: Disable mesh collision.
		}

		Face *pMeshFaces = pObject->GetMesh(i)->GetFaceList( &pLast );
		if ( pMeshFaces && pLast ) 
		{
			// append mesh faces to cumulative list
			pLast->nextFace = pAllFaces;
			pAllFaces = pMeshFaces;
		}
	}

	Face *pFace = pAllFaces;
	int vertexIndex = 0;
	while ( pFace )
    {
		btVector3 vertex1 = AGKToBullet::GetBtVector3(pFace->vert1)  * objectScale  / scaleFactor;
		btVector3 vertex2 = AGKToBullet::GetBtVector3(pFace->vert2)  * objectScale  / scaleFactor;
		btVector3 vertex3 = AGKToBullet::GetBtVector3(pFace->vert3)  * objectScale  / scaleFactor;

		if(bTransform)
		{
			vertex1 = objectTrans * vertex1;
			vertex2 = objectTrans * vertex2;
			vertex3 = objectTrans * vertex3;
		}

		//TODO: SITDS Need to Get Vetex Normals
		//May be needed for future use in CSG and Voronoi Fracture
		btVector3 normal(pFace->normal.x, pFace->normal.y, pFace->normal.z);

		//TODO: SITDS Need to Get Vertex UV Coordinates
		//May be needed for future use in CSG and Voronoi Fracture
		btVector3 uv(0,0,0);

		vertexData->m_vertexBuffer.push_back(vertex1);
		vertexData->m_normals.push_back(normal);
		vertexData->m_uvData.push_back(uv);
		//
		vertexData->m_vertexBuffer.push_back(vertex2);
		vertexData->m_normals.push_back(normal);
		vertexData->m_uvData.push_back(uv);
		//
		vertexData->m_vertexBuffer.push_back(vertex3);
		vertexData->m_normals.push_back(normal);
		vertexData->m_uvData.push_back(uv);
		//
		vertexData->m_indexBuffer.push_back(vertexIndex++);
		vertexData->m_indexBuffer.push_back(vertexIndex++);
		vertexData->m_indexBuffer.push_back(vertexIndex++);
		//
		pFace = pFace->GetNextFace();
    }
	return vertexData;
}

btVector3 AGKToBullet::GetBtVector3( AGKVector vector ) 
{ 
	return btVector3( vector.x, vector.y, vector.z );
}

btVector3 AGKToBullet::GetBtVector3FromVecID( int agkVecID ) 
{ 
	AGKVector* vector = vectorManager.GetItem( agkVecID )->GetAGKVector();
	return btVector3( vector->x, vector->y, vector->z );
}

btVector3 AGKToBullet::GetBtVector3ToRadians( int agkVecID ) 
{ 
	AGKVector* vector = vectorManager.GetItem( agkVecID )->GetAGKVector();
	return btVector3( btRadians( vector->x ), btRadians( vector->y ), btRadians( vector->z ) );
}



















