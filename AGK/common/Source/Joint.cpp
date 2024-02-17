
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

Joint::Joint(int jointID, btTypedConstraint* constraint) : BaseItem( jointID )
{
	m_constraint = constraint;
}

Joint::~Joint()
{
	if(m_constraint != NULL)
	{
		 GetCurrentDynamicsWorld()->m_dynamicsWorld->removeConstraint( m_constraint );
		delete m_constraint;	
	}
}

btTypedConstraint* Joint::GetConstraint()
{
	return m_constraint;
}

//TODO:SITDS handel all types of joints
btTransform* Joint::GetFrameOffsetA()
{
	btTypedConstraintType type = m_constraint->getConstraintType();
	if(type == D6_CONSTRAINT_TYPE)
	{
		btGeneric6DofConstraint* pickJoint = static_cast<btGeneric6DofConstraint*>(m_constraint);
		return &pickJoint->getFrameOffsetA();
	}
	return NULL;
}

//TODO:SITDS handel all types of joints
btTransform* Joint::GetFrameOffsetB()  
{
	btTypedConstraintType type = m_constraint->getConstraintType();
	if(type == D6_CONSTRAINT_TYPE)
	{
		btGeneric6DofConstraint* pickJoint = static_cast<btGeneric6DofConstraint*>(m_constraint);
		return &pickJoint->getFrameOffsetB();
	}
	return NULL;
} 











