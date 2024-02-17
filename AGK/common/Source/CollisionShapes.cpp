
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"
#define CONVEX_MARGIN 0.00

namespace CollisionShapes
{
	btCollisionShape* Box( btVector3 &size )
	{
		return new btBoxShape( size / 2.0 );
	}

	btCollisionShape* Sphere( btScalar diameter )
	{
		return new btSphereShape( btScalar( diameter/2.0 ) );
	}

	btCollisionShape* Cylinder( btVector3 &size, int axis )
	{
		btCollisionShape* cylinder;
		if( axis == X_AXIS )
		{
			cylinder = new btCylinderShapeX( size / 2.0 );
		}
		else if( axis == Y_AXIS )
		{
			cylinder = new btCylinderShape( size / 2.0 );
		}
		else
		{
			cylinder = new btCylinderShapeZ( size / 2.0 );
		}
		return cylinder;
	}

	btCollisionShape* Cone( btVector3 &size, int axis )
	{
		btCollisionShape* cone;
		if( axis == X_AXIS )
		{
			cone = new btConeShapeX( size.getY() / (btScalar)2.0,  size.getX() );
		}
		else if( axis == Y_AXIS )
		{
			cone = new btConeShape( size.getX() / (btScalar)2.0, size.getY() );
		}
		else
		{
			cone = new btConeShapeZ( size.getY() / (btScalar)2.0, size.getZ() );
		}
		return cone;
	}

	btCollisionShape* Capsule(btVector3 &size, int axis)
	{
		btCollisionShape* capsule;
		if( axis == X_AXIS )
		{
			capsule = new btCapsuleShapeX( size.getY() / (btScalar)2.0, size.getX() - size.getY() );
		}
		else if( axis == Y_AXIS )
		{
			capsule = new btCapsuleShape( size.getX() / (btScalar)2.0, size.getY() - size.getX() );
		}
		else
		{
			capsule = new btCapsuleShapeZ( size.getY() / (btScalar)2.0, size.getZ() - size.getY() );
		}
		return capsule;
	}

	btConvexHullShape* ConvexHull( btAlignedObjectArray<btVector3> &vertexBuffer, bool vertexReduction )
	{
		btConvexHullShape* collShape = new btConvexHullShape( &( vertexBuffer[0].getX() ), vertexBuffer.size() );
		collShape->setMargin( CONVEX_MARGIN );

		if( vertexReduction )
		{
			//Convex hull simplifcation method to keep under 100 vertices.
			//Must be done for all Convex hulls
			//http://bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
			btShapeHull* hull = new btShapeHull( collShape );
			btScalar margin = collShape->getMargin();
			hull->buildHull( margin );
			btConvexHullShape* simplifiedConvexShape = new btConvexHullShape( &( hull->getVertexPointer()[0].getX() ), hull->numVertices() );

			delete collShape;
			delete hull;
			simplifiedConvexShape->setMargin( CONVEX_MARGIN );
			return simplifiedConvexShape;
		}
		return collShape;
	}

	btConvexHullShape* ConvexHull( uint32_t objectID, bool vertexReduction )
	{
		AGKToBullet::AGKVertexData* vertexData = AGKToBullet::GetVertexData( objectID, GetCurrentDynamicsWorld()->m_scaleFactor, false );
		btConvexHullShape *collShape = CollisionShapes::ConvexHull( vertexData->m_vertexBuffer,vertexReduction ); 
		delete vertexData;
		return collShape;
	}

	btBvhTriangleMeshShape* TriangleMesh( uint32_t objectID, bool bBuildBvh )
	{
		btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;
		AGKToBullet::AGKVertexData* vertexData = AGKToBullet::GetVertexData( objectID, GetCurrentDynamicsWorld()->m_scaleFactor, false );
		btTriangleMesh *mTriMesh = new btTriangleMesh();
		for(int i = 0; i < vertexData->m_indexBuffer.size(); i+=3 )
		{
			btVector3 v0 = vertexData->m_vertexBuffer[vertexData->m_indexBuffer[i]];
			btVector3 v1 = vertexData->m_vertexBuffer[vertexData->m_indexBuffer[i+1]];
			btVector3 v2 = vertexData->m_vertexBuffer[vertexData->m_indexBuffer[i+2]];
			mTriMesh->addTriangle( v0,v1,v2 );
		}
		return new btBvhTriangleMeshShape( mTriMesh, true);
	}

	btCompoundShape* AddChildShapeToParent( btCompoundShape* parent, btCollisionShape* child )
	{
		//FHB:Seems we only need a Idenity transform here??
		btTransform localTrans;
		localTrans.setIdentity();
		parent->addChildShape(localTrans,child);
		return parent;
	}



void DeleteShape( btCollisionShape* collShape )
{
	if ( collShape ){
		if ( collShape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE )
		{
			const btBvhTriangleMeshShape* bvhShape = static_cast<const btBvhTriangleMeshShape*>( collShape );
			if ( bvhShape != NULL ){
				delete bvhShape->getTriangleInfoMap();
				delete bvhShape;
			}
			return;
		}
		delete collShape;
	}
}




}












