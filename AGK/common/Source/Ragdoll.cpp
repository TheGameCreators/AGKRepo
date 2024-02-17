
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

RagDoll* currentRagDoll = NULL;

RagDoll::RagDoll( int ragdollID ) : BaseItem( ragdollID )
{
	m_bNeedsUpdateForCulling = false;
	m_linearDamping = (btScalar) 0.08;
	m_angularDamping = (btScalar) 0.95;
	m_linearSleepingThresholds = (btScalar) 1.8;
	m_angularSleepingThresholds = (btScalar) 2.8;
	m_deactivationTime = (btScalar) 0.8;
	m_modelTotalVolume = (btScalar) 0.0;
}

RagDoll::~RagDoll()
{
	for ( int i = 0; i < m_joints.size(); i++ )
	{
		delete jointManager.RemoveItem( m_joints[i] );
	}
	for ( int i = 0; i < m_ragDollBoneArray.size(); i++ )
	{
		delete m_ragDollBoneArray[i];
	}
	m_ragDollBoneArray.clear();
}

void RagDoll::ResetObjectParametersForAnimation()
{
	cObject3D* pObject = agk::GetObjectPtr( m_id );
	for (int i = 0; i < pObject->GetSkeleton()->GetBoneCount(); i++ )
	{
		// added new function to get bone by index for better perfromance
		pObject->GetSkeleton()->GetBone( i )->RemoveWorldOverride();
	}
}

void RagDoll::SetTotalWeight(btScalar totalWeight)
{
	m_modelTotalWeight = totalWeight;
}

bool RagDoll::IsStatic()
{
	return m_bIsStatic;
}

void RagDoll::SetStatic( bool bStatic )
{
	m_bIsStatic = bStatic;
	for ( int i = 0; i < m_ragDollBoneArray.size(); i++ )
	{
		if(bStatic)
		{
			RigidBodies::SetMass( m_ragDollBoneArray[i]->GetRigidBody(), 0.0 );
		}
		else
		{
			btScalar mass = m_modelTotalDensity * m_ragDollBoneArray[i]->m_boneVolume;
			RigidBodies::SetMass( m_ragDollBoneArray[i]->GetRigidBody(), mass );
		}
	}
}

btRigidBody* RagDoll::localCreateRigidBody (btScalar mass, const btTransform& startTransform, btCollisionShape* shape, int objID, int collisionGroup, int collisionMask)
{
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);
	AGKMotionState* myMotionState = new AGKMotionState(startTransform, objID);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addRigidBody(body, collisionGroup, collisionMask);
	DynamicsWorld::ApplyDefaultCcd(body); 
	return body;
}

int RagDoll::AddBone(int objID, int limbID1, int limbID2, btScalar diameter, int collisionGroup, int collisionMask)
{
	m_ragDollBoneArray.push_back( new RagDollBone( objID, limbID1, limbID2, diameter, collisionGroup, collisionMask ) );

	return m_ragDollBoneArray.size()-1;
}

void RagDoll::AddHingeJoint(int boneIndex1, int boneIndex2, int limbID, const btVector3& jointRotation,
																					btScalar minRotation, btScalar maxRotation)
{
	btScalar scaleFactor =  GetCurrentDynamicsWorld()->m_scaleFactor;
	btHingeConstraint* hingeC;
	btTransform localATrans, localBTrans, jointTrans, limbRotationTrans;
	btMatrix3x3 jointMatrix, limbMatrix;
	int objectID = m_id;
	cObject3D* pObject = agk::GetObjectPtr( objectID );

	btVector3 jointPosition = AGKToBullet::GetBtVector3( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, limbID ) )->posFinal() );
	jointPosition= jointPosition/scaleFactor;

	//When we use setEulerZYX here it needs to be YXZ
	jointMatrix.setEulerZYX(btRadians(jointRotation.getY()), btRadians(jointRotation.getX()), btRadians(jointRotation.getZ()));
	jointTrans.setIdentity();
	jointTrans.setBasis(jointMatrix);

	//Adjustment based on the joints rotations at a frame of animation 
	//Get current limb rotation from animation and convert to radians
	//Create Rotation only Transform for Models limb, No position
	//We use setEulerYPR the order is ZYX passed in.
	limbMatrix.setEulerYPR( btRadians( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, limbID ) )->rotFinal().GetEulerZ() ) ,
												btRadians( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, limbID ) )->rotFinal().GetEulerY() ),
														btRadians( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, limbID ) )->rotFinal().GetEulerX() ) );
	limbRotationTrans.setIdentity();
	limbRotationTrans.setBasis(limbMatrix);

	//Rotate the joints transform with the limbs rotation
	jointTrans = limbRotationTrans * jointTrans ;

	jointTrans.setOrigin(jointPosition);
	
	//Creates local transforms for each side of joint using our adjusted joint transform
	Joints::CreateLocalTransforms(m_ragDollBoneArray[boneIndex1]->GetRigidBody(),
																m_ragDollBoneArray[boneIndex2]->GetRigidBody(),
																	jointTrans, localATrans, localBTrans);
	btScalar minLimit = btRadians(minRotation);
	btScalar maxLimit = btRadians(maxRotation);

	//Get the Angle between the two bone capsules for a hinge joint.
	btVector3 bone1Norm, bone2Norm;
	bone1Norm = m_ragDollBoneArray[boneIndex1]->GetNormilizedVector();
	bone2Norm = m_ragDollBoneArray[boneIndex2]->GetNormilizedVector();
	btScalar ang = bone1Norm.angle(bone2Norm);

	//Subtract this angle from the limits to adjust for the frame of animation.
	minLimit = minLimit - abs(ang);
	maxLimit = maxLimit - abs(ang);

	hingeC = Joints::CreateHingeConstraint(m_ragDollBoneArray[boneIndex1]->GetRigidBody(), 
																				m_ragDollBoneArray[boneIndex2]->GetRigidBody(),
																					localATrans, localBTrans, true);
	hingeC->setLimit( minLimit, maxLimit );
	uint32_t jointID = jointManager.GetFreeID();
	jointManager.AddItem( new Joint( jointID, hingeC ), jointID );
	m_joints.push_back(jointID);
	hingeC = NULL;
}

void RagDoll::AddTwistConstraint(int boneIndex1, int boneIndex2, int limbID, const btVector3& jointRotation, const btVector3& jointlimits)
{
	btScalar scaleFactor =  GetCurrentDynamicsWorld()->m_scaleFactor;
	btConeTwistConstraint* coneC; 
	btTransform localATrans, localBTrans, jointTrans, limbRotationTrans;
	btMatrix3x3  jointMatrix, limbMatrix;
	int objectID = m_id;
	cObject3D* pObject = agk::GetObjectPtr( objectID );

	//Create Transform for Joint, When we use setEulerYPR here it needs to be YXZ
	btVector3 jointPosition = AGKToBullet::GetBtVector3( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, limbID ) )->posFinal() );
	jointPosition= jointPosition/scaleFactor;

	//When we use setEulerZYX here it needs to be YXZ
	jointMatrix.setEulerZYX(btRadians(jointRotation.getY()), btRadians(jointRotation.getX()), btRadians(jointRotation.getZ()));
	jointTrans.setIdentity();
	jointTrans.setBasis(jointMatrix);

	//Adjustment based on the joints rotations at a frame of animation 
	//Get current limb world rotation from animation and convert to radians
	limbMatrix.setEulerYPR( btRadians( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, limbID ) )->rotFinal().GetEulerZ() ) ,
											btRadians( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, limbID ) )->rotFinal().GetEulerY() ),
													btRadians( pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, limbID ) )->rotFinal().GetEulerX() ) );
	limbRotationTrans.setIdentity();
	limbRotationTrans.setBasis(limbMatrix);

	//Rotate the joints transform with the limbs rotation
	jointTrans = limbRotationTrans * jointTrans ;

	jointTrans.setOrigin(jointPosition);

	//Creates local transforms for each side of joint using our adjusted joint transform
	Joints::CreateLocalTransforms( m_ragDollBoneArray[boneIndex1]->GetRigidBody(),
																m_ragDollBoneArray[boneIndex2]->GetRigidBody(),
																	jointTrans, localATrans, localBTrans);

	btScalar swingSpan1 = btRadians(jointlimits.getZ());
	btScalar swingSpan2 = btRadians(jointlimits.getY());
	btScalar twistSpan = btRadians(jointlimits.getX());

	coneC =  Joints::CreateTwistConstraint(m_ragDollBoneArray[boneIndex1]->GetRigidBody(), 
																				m_ragDollBoneArray[boneIndex2]->GetRigidBody(),
																					localATrans, localBTrans, true);
	coneC->setLimit( swingSpan1, swingSpan2, twistSpan );
	uint32_t jointID = jointManager.GetFreeID();
	jointManager.AddItem( new Joint( jointID, coneC ), jointID );
	m_joints.push_back(jointID);
	coneC = NULL;
}

int RagDoll::GetBoneObjID(int boneIndex) 
{
	return (boneIndex >= 0 && boneIndex < m_ragDollBoneArray.size()) ? m_ragDollBoneArray[boneIndex]->GetRagDollBoneID() : -1; 
} 

bool RagDoll::IsBoneObject(int objectID)
{
	for(int i = 0; i < m_ragDollBoneArray.size(); i++)
	{
		if(m_ragDollBoneArray[i]->GetObjectID()==objectID)
		{
			return true;
		}
	}
	return false;
}

void RagDoll::Finalize()
{
	int objectID = m_id;
	cObject3D* pObject = agk::GetObjectPtr( objectID );

	for (int i = 0; i < m_ragDollBoneArray.size(); i++)
	{
		m_modelTotalVolume += m_ragDollBoneArray[i]->m_boneVolume;
	}
	//Calculate density of ragdoll
	m_modelTotalDensity = m_modelTotalWeight / m_modelTotalVolume;

	//Set the mass of all the Rag doll bones to the calculated mass.
	for (int i = 0; i < m_ragDollBoneArray.size(); i++)
	{
		btScalar mass = m_modelTotalDensity * m_ragDollBoneArray[i]->m_boneVolume;
		RigidBodies::SetMass( m_ragDollBoneArray[i]->GetRigidBody(), mass );
		m_ragDollBoneArray[i]->GetRigidBody()->setFriction(0.5);
		m_ragDollBoneArray[i]->GetRigidBody()->setDamping(m_linearDamping, m_angularDamping);
		m_ragDollBoneArray[i]->GetRigidBody()->setDeactivationTime(m_deactivationTime);
		m_ragDollBoneArray[i]->GetRigidBody()->setSleepingThresholds(m_linearSleepingThresholds, m_angularSleepingThresholds);
	}
}

bool RagDoll::IsSleeping()
{
	bool bIsSleeping = true;
	for (int i = 0; i < m_ragDollBoneArray.size(); i++)
	{
		bIsSleeping = bIsSleeping && !m_ragDollBoneArray[i]->GetRigidBody()->isActive();
	}
	return bIsSleeping;
}

void RagDoll::Update() 
{
	m_bNeedsUpdateForCulling = true;
	int objectID = m_id;
	cObject3D* pObject = agk::GetObjectPtr( objectID );
	btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;

	//Create Transform for AGK Model
	btTransform ModelRotationTrans, objBoneInitalWorldRotationTrans;
	
	//Create a transform with the models roatation only.
	ModelRotationTrans.setIdentity();
	btMatrix3x3 ModelRotation;
	ModelRotation.setEulerYPR( btRadians( agk::GetObjectAngleZ( objectID ) ),
															btRadians( agk::GetObjectAngleY( objectID ) ),
																btRadians( agk::GetObjectAngleX(objectID ) ) );
	ModelRotationTrans.setBasis( ModelRotation );

	//Loop through all ragdoll bones and subtract all initial rotations of the capsules from there transforms
	for ( int i = 0; i < m_ragDollBoneArray.size(); i++ )
	{
		btTransform ragdollBoneWorldTransform,ragdollBoneRotatedWorldTransform, finalObjBoneWorldTransform;
		ragdollBoneWorldTransform.setIdentity();
		ragdollBoneRotatedWorldTransform.setIdentity();
		finalObjBoneWorldTransform.setIdentity();
		btVector3 finalWorldPosition;

		//Get the ragdoll bones World Transform
		ragdollBoneWorldTransform = btTransform( m_ragDollBoneArray[i]->GetRigidBody()->getWorldTransform() );

		//Subtract the initial rotation of the rag doll bone
		ragdollBoneRotatedWorldTransform.setBasis( ragdollBoneWorldTransform.getBasis() * m_ragDollBoneArray[i]->initialRotation.inverse() );
		
		for ( int j = 0; j < m_ragDollBoneArray[i]->m_limbOffsets.size(); j++ )
		{
			//Get the final World position vector adjusted by the offset
			//How far the objects bones are offset from the ragdoll capsule center.
			finalWorldPosition = ragdollBoneWorldTransform * m_ragDollBoneArray[i]->m_limbOffsets[j];

			//Updated vector is new final world position for the objects bone transform
			finalObjBoneWorldTransform.setOrigin( finalWorldPosition * scaleFactor );

			//set the rotation with the ragdolls bones rotated transform 
			finalObjBoneWorldTransform.setBasis( ragdollBoneRotatedWorldTransform.getBasis() );

			//Create a transform for the object's bones initial rotation, to be filled in the loop below.
			objBoneInitalWorldRotationTrans.setIdentity();

			//Handel the Models bones rotations at a frame of animation.
			objBoneInitalWorldRotationTrans.setBasis( m_ragDollBoneArray[i]->m_limbInitalRotation[j] );

			objBoneInitalWorldRotationTrans *= ModelRotationTrans.inverse();
			finalObjBoneWorldTransform *= objBoneInitalWorldRotationTrans;
			finalObjBoneWorldTransform *= ModelRotationTrans;

			//Finally we update the AGK Object
			//This moves the joints but not the models bounding box that is the object
			pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, m_ragDollBoneArray[i]->m_limbIDs[j] ) )
				->OverrideWorldPosition( finalObjBoneWorldTransform.getOrigin().getX(), finalObjBoneWorldTransform.getOrigin().getY(), 
					finalObjBoneWorldTransform.getOrigin().getZ() );

			pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, m_ragDollBoneArray[i]->m_limbIDs[j] ) )
				->OverrideWorldRotation( finalObjBoneWorldTransform.getRotation().getW(), finalObjBoneWorldTransform.getRotation().getX(),
					finalObjBoneWorldTransform.getRotation().getY(), finalObjBoneWorldTransform.getRotation().getZ() );
		}
	}
}

void RagDoll::AssignLimbIDToBone(int boneIndex, int agkBoneID)
{
	int objectID = m_id;
	cObject3D* pObject = agk::GetObjectPtr( objectID );
	m_ragDollBoneArray[ boneIndex ]->AddLimbID( agkBoneID );

	btMatrix3x3 boneMatrix;
	boneMatrix.setRotation( btQuaternion(pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, agkBoneID ) )->rotFinal().x, 
		pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, agkBoneID ) )->rotFinal().y,
		pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, agkBoneID ) )->rotFinal().z,
		pObject->GetSkeleton()->GetBone( agk::GetObjectBoneName( objectID, agkBoneID ) )->rotFinal().w));

	//Store the initial world rotation of the AGK objects Bones.
	m_ragDollBoneArray[ boneIndex ]->m_limbInitalRotation.push_back( boneMatrix );
}

void RagDoll::HideBones()
{
	for(int i = 0; i < m_ragDollBoneArray.size(); i++)
	{
		agk::SetObjectVisible(m_ragDollBoneArray[i]->GetRagDollBoneID(), 0);
	}
}

void RagDoll::DisableDeactivation(int isDisabled)
{
	int state = isDisabled ? DISABLE_DEACTIVATION: WANTS_DEACTIVATION;
	for(int i = 0; i < m_ragDollBoneArray.size(); i++)
	{
		RigidBodies::SetActivationSate(m_ragDollBoneArray[i]->GetRigidBody(),state);
	}
}

void RagDoll::ShowBones()
{
	for(int i = 0; i < m_ragDollBoneArray.size(); i++)
	{
		agk::SetObjectVisible( m_ragDollBoneArray[i]->GetRagDollBoneID(),1 );
	}
}

void RagDoll::SetDamping(btScalar linear, btScalar angular)
{
	m_linearDamping = linear;
	m_angularDamping = angular;
}

void RagDoll::SetSleepingThresholds(btScalar linear, btScalar angular)
{
	m_linearSleepingThresholds = linear;
	m_angularSleepingThresholds = angular;
}

void  RagDoll::SetDeactivationTime(btScalar time)
{
	m_deactivationTime = time;
}

bool RagDoll::AssertRagdollExist( int ragdollID, const char* message, bool bExist /*= true*/ ) 
{
	if ( bExist && ragDollManager.GetItem( ragdollID ) == NULL )
	{
		agk::Error( message );
		return false;
	}
	else if ( !bExist && ragDollManager.GetItem( ragdollID ) != NULL )
	{
		agk::Error( message );
		return false;
	}
	return true;
}

int RagDoll::GetIDFromBoneObject(int objID)
{
	for(uint32_t i = 0; i < ragDollManager.GetCount(); i++)
	{
		RagDoll* ragdoll = ragDollManager.GetItem( objID );
		if(ragdoll->IsBoneObject(objID))
		{
			return ragdoll->GetID();
		}
	}
	return -1;
}








