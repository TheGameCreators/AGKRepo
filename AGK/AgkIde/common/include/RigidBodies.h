
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

namespace RigidBodies
{
	btRigidBody* CreateRigidBodyStatic( btCollisionShape* collShape );
	btRigidBody* CreateRigidBody( btCollisionShape* collShape, int objID, btScalar mass, bool bUseCCD = true );
	void Delete( btRigidBody* body );

	btScalar GetMass( btRigidBody* body );
	btVector3 GetLinearVelocity( btRigidBody* body );


	void SetMass( btRigidBody* body, btScalar mass );
	void SetActivationSate( btRigidBody* body, int eActivationState );
	void SetFriction( btRigidBody* body, btScalar friction );
	void SetRollingFriction( btRigidBody* body, btScalar friction );
	void SetDamping( btRigidBody* body, btScalar linearDamping, btScalar angularDamping );
	void SetAnisotropicFriction( btRigidBody* body, int type );
	void SetLinearVelocity( btRigidBody* body, const btVector3& linVelocity );
	void SetAngularVelocity( btRigidBody* body, const btVector3& angVelocity );

	bool GetContacts( btRigidBody* body );
}

