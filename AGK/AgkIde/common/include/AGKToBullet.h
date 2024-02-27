
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

namespace AGKToBullet
{
	class AGKVertexData
	{
		public:
		AGKVertexData()
		{
		}
		~AGKVertexData()
		{
			m_vertexBuffer.clear();
			m_indexBuffer.clear();
			m_normals.clear();
			m_uvData.clear();
		}
		btAlignedObjectArray<btVector3> m_vertexBuffer;
		btAlignedObjectArray<int> m_indexBuffer;
		btAlignedObjectArray<btVector3> m_normals;
		btAlignedObjectArray<btVector3> m_uvData;
	};//End of class
	bool AssertValidPhysicsWorld();
	bool AssertValidObject( UINT objID, const char* message );  
	bool AssertValidBody( btRigidBody* body, const char* message );  
	bool AssertValidJoint( btTypedConstraint* constraint, const char* message );
	bool AssertValidJointID(int jointID, const char* message);
	bool AssertValidPlaneID( int planeID, const char* message );
	bool AssertValidVectorID( int vectorID, const char* message );
	bool AssertValidRayID( int rayID, const char* message );
	bool AssertValidCharacterController( int ControllerID, const char* message );

	btVector3 GetObjectScale( UINT objID );
	btVector3 GetObjectSize( UINT objID );  
	btScalar GetObjectDiameter( UINT objID ); 
	AGKVertexData* GetVertexData( UINT objID, btScalar scaleFactor, bool bTransform );
	btTransform GetTransform( UINT objID, btScalar scaleFactor = 1.0 );
	btVector3 GetBtVector3( AGKVector vector );
	btVector3 GetBtVector3FromVecID( int agkVecID );
	btVector3 GetBtVector3ToRadians( int agkVecID );
}



















