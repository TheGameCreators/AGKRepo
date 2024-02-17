
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

struct Joints
{
	public:
		static void CreateLocalTransforms(btRigidBody *bodyA, btRigidBody *bodyB,  btTransform& jointTrans, btTransform& localATrans, btTransform& localBTrans);
		static int GetContactPosition( btRigidBody *bodyA, btRigidBody *bodyB,btTransform &outJointTrans );
		static btConeTwistConstraint* CreateTwistConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB, bool disableCollisionsBetweenLinkedBodies);
		static btHingeConstraint* CreateHingeConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB, bool disableCollisionsBetweenLinkedBodies);
		static btPoint2PointConstraint* CreatePoint2PointConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB);
		static btPoint2PointConstraint* CreatePoint2PointConstraint(btRigidBody *a_body,btRigidBody *b_body, btVector3 &pivotPosInA, btVector3 &pivotPosInB);
		static btSliderConstraint* CreateSliderConstraint(btRigidBody *a_body,btRigidBody *b_body, btTransform &localA, btTransform &localB);
		static btFixedConstraint* CreateFixedConstraint(btRigidBody *a_body,btRigidBody *b_body,btTransform &localA, btTransform &localB );
		static btGeneric6DofConstraint* Create6DofConstraint(btRigidBody *a_body,btRigidBody *b_body,btTransform &localA, btTransform &localB );
		static btGeneric6DofConstraint* Create6DofConstraint(btRigidBody *b_body, btTransform &localb, bool useLinearReferenceFrameB); 
		static int SetBreakingThreshold( btTypedConstraint* constraint,btScalar breakThreshold ); 
		static btGeneric6DofConstraint* CreatePickConstraint( btRigidBody* body, btVector3& pickJointPosition );
}; 