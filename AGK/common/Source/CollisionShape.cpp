
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

CollisionShape::CollisionShape(int shapeID, btCollisionShape* shape) : BaseItem(shapeID)
{
	m_shape = shape;
}

CollisionShape::~CollisionShape()
{
	delete m_shape;
}

btCollisionShape* CollisionShape::GetShape()
{
	return m_shape;
}