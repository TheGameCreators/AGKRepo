
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

RagDollBone::RagDollBone( int dbproObjectID, int dbproStartLimbID, int dbproEndLimbID, btScalar diameter, int collisionGroup, int collisionMask )
{
	m_objectID = dbproObjectID;
	m_startLimbID = dbproStartLimbID;
	m_endLimbID = dbproEndLimbID;
	m_diameter = diameter;
	m_collisionGroup = collisionGroup;
	m_collisionMask = collisionMask;
	CreateBone();
}

RagDollBone::~RagDollBone(void)
{
	delete rigidBodyManager.RemoveItem( m_ragDollBoneID );
	agk::DeleteObject(m_ragDollBoneID);
}

void RagDollBone::CreateBone()
{
	cObject3D* pObject = agk::GetObjectPtr( m_objectID );
	btScalar scaleFactor =  GetCurrentDynamicsWorld()->m_scaleFactor;
	btVector3 ObjectPosition = AGKToBullet::GetBtVector3( pObject->posFinal() );
	btVector3 jointVec1 = AGKToBullet::GetBtVector3( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( m_objectID, m_startLimbID ) )->posFinal() );
	btVector3 jointVec2 = AGKToBullet::GetBtVector3( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( m_objectID, m_endLimbID ) )->posFinal() );

	//get distance between models joints
	btVector3 resultVec;
	resultVec = jointVec1 - jointVec2;

	//Get a vector which represents a line from jointVec1 to jointVec2
	//Then store this vector to calculate a min/max angle for the joint in add joint.
	btVector3 boneVec;
	boneVec = resultVec;
	m_boneNormVec = boneVec.normalize();

	btScalar height = resultVec.length();
	//make capsule height from distance between joints
	//Capsule is made along the Z axis with zero rotations
	m_ragDollBoneID = agk::CreateObjectCapsule( m_diameter, height, Z_AXIS );
	//Store the Total volume for mass calculations
	m_boneVolume = m_diameter * m_diameter * height;
	//Divide distance to get center to position capsule
	resultVec = resultVec / 2;
	btVector3 positionVec;
	positionVec = jointVec1 - resultVec;
	agk::SetObjectPosition( m_ragDollBoneID, positionVec.getX(), positionVec.getY(), positionVec.getZ() );

	//Rotate capsule to point at second joint, now capsule has rotations.
	agk::SetObjectLookAt( m_ragDollBoneID, jointVec2.getX(), jointVec2.getY(), jointVec2.getZ(), 0.0 );

	m_collisionShape = new btCapsuleShapeZ( btScalar( m_diameter/scaleFactor/2 ), btScalar( ( height - m_diameter ) /scaleFactor ) );

	//Create a World Transform for the bone
	btTransform boneTrans;
	boneTrans.setIdentity();
	boneTrans.setOrigin( positionVec / scaleFactor );

	//Using a matrix it takes it ZYX
	btMatrix3x3 boneRotation;
	boneRotation.setEulerYPR( btScalar( btRadians( agk::GetObjectAngleZ( m_ragDollBoneID ) ) ),
												btScalar( btRadians( agk::GetObjectAngleY( m_ragDollBoneID ) ) ),
													btScalar( btRadians( agk::GetObjectAngleX( m_ragDollBoneID ) ) ) );
	boneTrans.setBasis( boneRotation );

	m_rigidBody = localCreateRigidBody( btScalar( 1.0 ), boneTrans, m_collisionShape, m_ragDollBoneID, m_collisionGroup, m_collisionMask );
	rigidBodyManager.AddItem( new RigidBody( m_rigidBody, m_ragDollBoneID) , m_ragDollBoneID );

	initialRotation = m_rigidBody->getWorldTransform().getBasis();

	agk::SetObjectVisible(m_ragDollBoneID, 0 );
}

btRigidBody* RagDollBone::GetRigidBody()
{
	return m_rigidBody;
}

int RagDollBone::GetStartLimbID()
{
	return m_startLimbID;
}
int RagDollBone::GetEndLimbID()
{
	return m_endLimbID;
}
int RagDollBone::GetObjectID()
{
	return m_objectID;
}
int RagDollBone::GetRagDollBoneID()
{
	return m_ragDollBoneID;
}

btVector3  RagDollBone::GetNormilizedVector()
{
	return m_boneNormVec;
}

btRigidBody* RagDollBone::localCreateRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int objID, int collisionGroup, int collisionMask)
{
	bool isDynamic = ( mass != 0.f );
	btVector3 localInertia( 0,0,0 );
	if ( isDynamic )
	{
		shape->calculateLocalInertia( mass, localInertia );
	}
	AGKMotionState* myMotionState = new AGKMotionState( startTransform, objID );
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia );
	btRigidBody* body = new btRigidBody( rbInfo );
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addRigidBody( body, collisionGroup, collisionMask );
	return body;
}

void RagDollBone::AddLimbID(int LimbID)
{
	cObject3D* pObject = agk::GetObjectPtr( m_objectID );
	btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;
	m_limbIDs.push_back(LimbID);
	//Get limb world Position
	btVector3 limbWorldPosition = AGKToBullet::GetBtVector3( pObject->GetSkeleton()->
												GetBone( agk::GetObjectBoneName( GetObjectID(), LimbID ) )->posFinal() );
	limbWorldPosition /= scaleFactor;

	btVector3 limbOffset = limbWorldPosition - GetRigidBody()->getWorldTransform().getOrigin();

	//Store the Vector rotated
	m_limbOffsets.push_back(limbOffset * GetRigidBody()->getWorldTransform().getBasis());

	btVector3 objectTranslation = btVector3( agk::GetObjectX( GetObjectID() ),
																		agk::GetObjectY( GetObjectID() ),
																			agk::GetObjectZ( GetObjectID() ) );
	objectTranslation /= scaleFactor;
	m_centerOfObjectOffsets.push_back(limbWorldPosition - objectTranslation);
}

