
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

RigidBody::RigidBody( btRigidBody* rigidBody, int bodyID ) : BaseItem( bodyID )
{
	m_rigidBody = rigidBody;
}

RigidBody::~RigidBody()
{
	RigidBodies::Delete( m_rigidBody );
}

btRigidBody* RigidBody::GetRigidBody()
{
	return m_rigidBody;
}

int RigidBody::GetCollissionGroup()
{
	return m_collissionGroup;
}

void RigidBody::SetCollissionGroup(int collissionGroup)
{
	m_collissionGroup = collissionGroup;
}

int RigidBody::GetCollissionMask()
{
	return m_collissionMask;
}

void RigidBody::SetCollissionMask(int collissionMask)
{
	m_collissionMask = collissionMask;
}










