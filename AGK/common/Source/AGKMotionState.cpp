
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

AGKMotionState::AGKMotionState(const btTransform &initialpos, int objID)
{
	m_objID = objID;
	m_Pos1 = initialpos;
	m_maxLinearVelocity = 30.0;
	//TODO: need to add max angular velocity.
}

AGKMotionState::~AGKMotionState(void)
{
}

int AGKMotionState::GetObjID()
{
	return m_objID;
}

void AGKMotionState::SetObjID(int objID)
{
	m_objID = objID;
}

void AGKMotionState::getWorldTransform(btTransform &worldTrans) const 
{
	//SITDS: added for kinematic bodies
	btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;
	btTransform currentTrans;
	currentTrans = AGKToBullet::GetTransform( m_objID, scaleFactor );
	worldTrans = currentTrans;
}

void AGKMotionState::setWorldTransform(const btTransform &worldTrans) 
{	
	btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;
    if(agk::GetObjectExists(m_objID) == false)
        return; 

	btQuaternion tempQ;
	tempQ = worldTrans.getRotation();
	btScalar wAngle =  tempQ.getW();
	btScalar xAngle =  tempQ.getX();
	btScalar yAngle =  tempQ.getY();
	btScalar zAngle =  tempQ.getZ();

    agk::SetObjectRotationQuat(m_objID, wAngle, xAngle, yAngle, zAngle);
    btVector3 pos = worldTrans.getOrigin();
    agk::SetObjectPosition(m_objID,pos.x()*scaleFactor, pos.y()*scaleFactor, pos.z()*scaleFactor);
 }

//Called by The DBPro Character Controller
void AGKMotionState::setWorldTransform(int objID, const btTransform &worldTrans, const btVector3& initialRotation ) 
{	
	btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;
    if(agk::GetObjectExists(objID) == false)
        return; 
	btQuaternion tempQ;
	tempQ = worldTrans.getRotation();

	btQuaternion initRotMat;
	initRotMat.setW( initialRotation.w() );
	initRotMat.setX( initialRotation.getX() );
	initRotMat.setY( initialRotation.getY() );
	initRotMat.setZ( initialRotation.getZ() );

	tempQ *= initRotMat;

	btScalar wAngle =  tempQ.getW();
	btScalar xAngle =  tempQ.getX();
	btScalar yAngle =  tempQ.getY();
	btScalar zAngle =  tempQ.getZ();

	agk::SetObjectRotationQuat( objID, wAngle, xAngle, yAngle, zAngle );
    btVector3 pos = worldTrans.getOrigin();
    agk::SetObjectPosition( objID, pos.x() * scaleFactor, pos.y() * scaleFactor, pos.z() * scaleFactor );
 }

void AGKMotionState::setWorldTransform(int objID, const btTransform &worldTrans) 
{
	AGKMotionState::setWorldTransform(objID, worldTrans, btVector3( 0.0,0.0,0.0 ) );
}

btScalar AGKMotionState::GetMaxLinearVelocity()
{
	return m_maxLinearVelocity;
}

void AGKMotionState::SetMaxLinearVelocity(btScalar maxLinearVelocity)
{
	m_maxLinearVelocity = maxLinearVelocity;
}
