
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

enum Axis
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS
};

namespace CollisionShapes
{
	btCollisionShape* Box( btVector3 &size );
	btCollisionShape* Sphere( btScalar diameter );
	btCollisionShape* Cylinder( btVector3 &size, int axis );
	btCollisionShape* Cone( btVector3 &size, int axis );
	btCollisionShape* Capsule( btVector3 &size, int axis );
	btConvexHullShape* ConvexHull( UINT objectID, bool vertexReduction );
	btConvexHullShape* ConvexHull( btAlignedObjectArray<btVector3> &vertexBuffer, bool vertexReduction );
	btBvhTriangleMeshShape* TriangleMesh( UINT objectID, bool bBuildBvh );
	btCompoundShape* AddChildShapeToParent( btCompoundShape* parent, btCollisionShape* child );
	void DeleteShape( btCollisionShape* collShape );
}








