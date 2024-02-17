
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

void Joints::CreateLocalTransforms(btRigidBody *bodyA, btRigidBody *bodyB, btTransform& jointTrans, btTransform& localATrans, btTransform& localBTrans )
{
	//Get world Transform for rigid bodies 
	btTransform bodyAWorldTrans, bodyBWorldTrans; 
	bodyAWorldTrans = bodyA->getWorldTransform();
	bodyBWorldTrans = bodyB->getWorldTransform();

	localATrans.setIdentity();
	localBTrans.setIdentity();

	localATrans = bodyAWorldTrans.inverse() * jointTrans;
	localBTrans = bodyBWorldTrans.inverse() * jointTrans;
}

int Joints::GetContactPosition( btRigidBody *bodyA, btRigidBody *bodyB, btTransform &outJointTrans )
{
	MyContactResultCallback result;

	btCollisionObject collObjectA, collObjectB;

	collObjectA.setWorldTransform( bodyA->getWorldTransform() );
	collObjectA.setCollisionShape( bodyA->getCollisionShape() );

	collObjectB.setWorldTransform( bodyB->getWorldTransform());
	collObjectB.setCollisionShape( bodyB->getCollisionShape());

	GetCurrentDynamicsWorld()->m_dynamicsWorld->contactPairTest(&collObjectA, &collObjectB, result );

	if( result.m_connected )
	{
		btVector3 contactPosWorld = result.m_cp.getPositionWorldOnA();
		outJointTrans.setIdentity();
		outJointTrans.setOrigin( contactPosWorld );
		return 1;
	}
	return 0;
}

btConeTwistConstraint* Joints::CreateTwistConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB, bool disableCollisionsBetweenLinkedBodies)
{
	btConeTwistConstraint *twistC = new btConeTwistConstraint(*a_body, *b_body, localA, localB);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addConstraint(twistC, disableCollisionsBetweenLinkedBodies);
	return twistC;
}

btHingeConstraint* Joints::CreateHingeConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB, bool disableCollisionsBetweenLinkedBodies)
{		
	btHingeConstraint *hingeC = new btHingeConstraint(*a_body, *b_body, localA, localB);			
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addConstraint(hingeC, disableCollisionsBetweenLinkedBodies);
	return hingeC;
}

btPoint2PointConstraint* Joints::CreatePoint2PointConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB)
{		
	btPoint2PointConstraint *pointC = new btPoint2PointConstraint(*a_body, *b_body, localA.getOrigin(), localB.getOrigin());
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addConstraint(pointC, true);
	return pointC;
}

btPoint2PointConstraint* Joints::CreatePoint2PointConstraint(btRigidBody *a_body,btRigidBody *b_body, btVector3 &pivotPosInA, btVector3 &pivotPosInB)
{		
	btPoint2PointConstraint *pointC = new btPoint2PointConstraint(*a_body, *b_body, pivotPosInA, pivotPosInB);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addConstraint(pointC, true);	
	return pointC;
}

btSliderConstraint* Joints::CreateSliderConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB)
{
	btSliderConstraint *sliderC = new btSliderConstraint(*a_body, *b_body, localA, localB, true);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addConstraint(sliderC, true);	
	return sliderC;
}

btFixedConstraint* Joints::CreateFixedConstraint(btRigidBody *a_body,btRigidBody *b_body,btTransform &localA, btTransform &localB )
{	
	btFixedConstraint* fixed = new btFixedConstraint(*a_body,*b_body,localA,localB);
	//FHB: Default is 10 ,pass a -1. Demo was set to 30. Need to investigate this so no slow down is caused.
	fixed->setOverrideNumSolverIterations(-1);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addConstraint(fixed,true);
	return fixed;
}

btGeneric6DofConstraint* Joints::Create6DofConstraint(btRigidBody *a_body,btRigidBody *b_body,btTransform &localA, btTransform &localB )
{
	btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*a_body,*b_body,localA,localB,true);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addConstraint(dof6,true);
	return dof6;
}

btGeneric6DofConstraint* Joints::Create6DofConstraint(btRigidBody *b_body, btTransform &localb, bool useLinearReferenceFrameB)
{
	btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*b_body,localb, useLinearReferenceFrameB);
	GetCurrentDynamicsWorld()->m_dynamicsWorld->addConstraint(dof6,true);
	return dof6;
}

int Joints::SetBreakingThreshold( btTypedConstraint* constraint,btScalar breakThreshold )
{
	if( !constraint->getRigidBodyA().isStaticOrKinematicObject() && !constraint->getRigidBodyB().isStaticOrKinematicObject()
		&& constraint->getRigidBodyA().getInvMass() != 0 && constraint->getRigidBodyB().getInvMass() != 0)
	{
		btScalar totalMass = (btScalar) (1.0/constraint->getRigidBodyA().getInvMass() + 1.0/constraint->getRigidBodyB().getInvMass());
		constraint->setBreakingImpulseThreshold( breakThreshold * totalMass );
		return 0;
	}
	return -1;
}

btGeneric6DofConstraint* Joints::CreatePickConstraint( btRigidBody* body, btVector3& pickJointPosition )
{
	btRigidBody* pickedBody;//for deactivation state
		if ( !body->isStaticOrKinematicObject() )
		{
			pickedBody = body;
			pickedBody->setActivationState(DISABLE_DEACTIVATION);
			btVector3 localJointPosition = pickedBody->getCenterOfMassTransform().inverse() * pickJointPosition;
			btTransform pickedJointTrans;
			pickedJointTrans.setIdentity();
			pickedJointTrans.setOrigin(localJointPosition);

			btGeneric6DofConstraint* dof6 = Joints::Create6DofConstraint(pickedBody, pickedJointTrans,false);
			if(dof6 != NULL)
			{
				//Set Joints parameters so it can be used as a pick joint
				dof6->setLinearLowerLimit(btVector3(0,0,0));
				dof6->setLinearUpperLimit(btVector3(0,0,0));
				dof6->setAngularLowerLimit(btVector3(0,0,0));
				dof6->setAngularUpperLimit(btVector3(0,0,0));

				dof6->setParam(BT_CONSTRAINT_STOP_CFM,(btScalar)0.8,0);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM,(btScalar)0.8,1);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM,(btScalar)0.8,2);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM,(btScalar)0.8,3);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM,(btScalar)0.8,4);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM,(btScalar)0.8,5);

				dof6->setParam(BT_CONSTRAINT_STOP_ERP,(btScalar)0.1,0);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP,(btScalar)0.1,1);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP,(btScalar)0.1,2);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP,(btScalar)0.1,3);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP,(btScalar)0.1,4);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP,(btScalar)0.1,5);

				return dof6;
			}
		}
		return NULL;
}