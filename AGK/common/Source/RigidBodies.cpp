
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

btRigidBody* RigidBodies::CreateRigidBodyStatic( btCollisionShape* collShape )
{
	btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3( 0.0 ,0.0, 0.0) );

	btRigidBody::btRigidBodyConstructionInfo rbInfo( 0.0, NULL, collShape );
	rbInfo.m_startWorldTransform = startTransform;
	return new btRigidBody( rbInfo );
}

btRigidBody* RigidBodies::CreateRigidBody(btCollisionShape* collShape, int objID, btScalar mass, bool bUseCCD /*=true*/)
{
	btScalar scaleFactor = GetCurrentDynamicsWorld()->m_scaleFactor;
	btVector3 localInertia( 0, 0, 0 );
	//if ( !collShape->isConcave() && collShape->getShapeType() != STATIC_PLANE_PROXYTYPE )
	//TODO: SITDS: I think we can replace with these 2 lines.
	bool isDynamic = (mass != 0.0);
	if (isDynamic)
	{
		collShape->calculateLocalInertia(mass,localInertia);
	}

	btTransform primativeTransform;
	primativeTransform.setIdentity();
	primativeTransform.setOrigin(btVector3(agk::GetObjectX(objID)/scaleFactor, agk::GetObjectY(objID)/scaleFactor, agk::GetObjectZ(objID)/scaleFactor));

	btMatrix3x3 btMatrix;
	//When we use setEulerYPR here for rigidbodies, it needs to be ZYX
	btMatrix.setEulerYPR( btScalar( btRadians( agk::GetObjectAngleZ( objID ) ) ),
										btScalar( btRadians( agk::GetObjectAngleY( objID ) ) ),
											btScalar( btRadians( agk::GetObjectAngleX( objID ) ) ) );
	primativeTransform.setBasis( btMatrix );

	//Using AGKMotionState, it provides interpolation capabilities, and only synchronizes "ACTIVE" objects
	AGKMotionState* myMotionState = new AGKMotionState( primativeTransform, objID );
	btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, myMotionState, collShape, localInertia );
	//SITDS: Will Need this for ragdoll
	//rbInfo.m_additionalDamping = true;
	btRigidBody* body = new btRigidBody( rbInfo );

	if ( bUseCCD ) 
	{ 
		DynamicsWorld::ApplyDefaultCcd( body ); 
	}
	return body;
}

void RigidBodies::Delete( btRigidBody* body )
{
	AGKMotionState* agkMotionState = (AGKMotionState*)body->getMotionState();
	delete agkMotionState; 
	CollisionShapes::DeleteShape( body->getCollisionShape() );
	GetCurrentDynamicsWorld()->m_dynamicsWorld->removeRigidBody( body );

	delete body;
}

btScalar  RigidBodies::GetMass(btRigidBody* body)
{
	return ((btScalar)1.0) / body->getInvMass();
}

btVector3 RigidBodies::GetLinearVelocity( btRigidBody* body )
{
	return body->getLinearVelocity();
}

void  RigidBodies::SetMass(btRigidBody* body, btScalar mass)
{
	btVector3 localInertia(0.0,0.0,0.0);
	body->getCollisionShape()->calculateLocalInertia(mass,localInertia);
	body->setMassProps(mass, localInertia);
}

void  RigidBodies::SetActivationSate(btRigidBody* body, int eActivationState)
{
	body->forceActivationState(eActivationState);
}

void  RigidBodies::SetFriction(btRigidBody* body, btScalar friction)
{
	body->setFriction(friction);
}

void  RigidBodies::SetRollingFriction(btRigidBody* body, btScalar friction)
{
	body->setRollingFriction(friction);
}

void  RigidBodies::SetDamping( btRigidBody* body, btScalar linearDamping, btScalar angularDamping  )
{
	body->setDamping( linearDamping, angularDamping );
}

void RigidBodies::SetAnisotropicFriction( btRigidBody* body, int type )
{
	//CF_ANISOTROPIC_FRICTION_DISABLED=0,
	//CF_ANISOTROPIC_FRICTION = 1,
	//CF_ANISOTROPIC_ROLLING_FRICTION = 2
	btCollisionShape* shape = body->getCollisionShape();
	body->setAnisotropicFriction( shape->getAnisotropicRollingFrictionDirection(), type );
}

void RigidBodies::SetLinearVelocity( btRigidBody* body, const btVector3& linVelocity )
{
	body->setLinearVelocity(linVelocity);
}

void RigidBodies::SetAngularVelocity( btRigidBody* body, const btVector3& angVelocity )
{
	body->setAngularVelocity( angVelocity );
}

//TODO:SITDS Need to create a check list of contacts.
 bool RigidBodies::GetContacts(btRigidBody* body)
{
	contactReportIndex = 0;
	contactReports.clear();

	int numManifolds = GetCurrentDynamicsWorld()->m_dynamicsWorld->getDispatcher()->getNumManifolds();
    for ( int i = 0; i < numManifolds; i++ )
    {
        btPersistentManifold* contactManifold =  GetCurrentDynamicsWorld()->m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal( i ); 
        btCollisionObject* obA = ( btCollisionObject* )( contactManifold->getBody0() );
        btCollisionObject* obB = ( btCollisionObject* )( contactManifold->getBody1() );

		if(body == btRigidBody::upcast( obA )  || body == btRigidBody::upcast( obB ))
		{
			for (int j = 0; j < contactManifold->getNumContacts(); j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if ( pt.getLifeTime() == 1 )
				{
					if(body == btRigidBody::upcast( obA ))
					{
						contactReports.push_back(new ContactReport(GetCurrentDynamicsWorld()->GetObjectID(btRigidBody::upcast( obA ) ), 
							GetCurrentDynamicsWorld()->GetObjectID(btRigidBody::upcast( obB ) ), pt));
					}
					else
					{
						contactReports.push_back(new ContactReport(GetCurrentDynamicsWorld()->GetObjectID(btRigidBody::upcast( obB ) ), 
							GetCurrentDynamicsWorld()->GetObjectID(btRigidBody::upcast( obA ) ), pt));
					}
				}
			}
		}
    }

	return contactReports.size() > 0 ? true : false;
}


