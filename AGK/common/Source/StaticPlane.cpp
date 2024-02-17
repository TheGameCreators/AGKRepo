
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

StaticPlane::StaticPlane( int planeID, btRigidBody* body ) : BaseItem( planeID )
{
	m_StaticPlane = body;
}

StaticPlane::~StaticPlane()
{
	if(m_StaticPlane != NULL)
	{
		RigidBodies::Delete(m_StaticPlane);
	}
}

btRigidBody* StaticPlane::GetPlane()
{
	return m_StaticPlane;
}