
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

using namespace AGK;

int currentDynamicsWorldID = 0;
btAlignedObjectArray<DynamicsWorld*> DynamicsWorldArray;

cHashedList<RigidBody*> rigidBodyManager( 1024 );
cHashedList<Joint*> jointManager( 1024 );
cHashedList<StaticPlane*> staticPlaneManager( 1024 );
cHashedList<Vector*> vectorManager( 1024 );
cHashedList<Ray*> rayManager( 1024 );
cHashedList<CharacterController*> characterControllerManager( 1024 );
cHashedList<RagDoll*> ragDollManager( 1024 );



btDynamicsWorld* GetCurrentBulletDynamicsWorld()
{
	return GetCurrentDynamicsWorld()->GetBulletDynamicsWorld();
}

DynamicsWorld* GetCurrentDynamicsWorld()
{
	return DynamicsWorldArray[currentDynamicsWorldID];
}

DynamicsWorld::DynamicsWorld(void)
{
	m_scaleFactor = (btScalar)40.0;
	m_gravity = btVector3( (btScalar)0.0, (btScalar)-10.0, (btScalar)0.0 );
	m_elapsedTime = (btScalar) (1.0 / 60.0);
}

DynamicsWorld::DynamicsWorld( btScalar scaleFactor )
{
	m_scaleFactor = scaleFactor;
	m_gravity = btVector3( (btScalar)0.0, (btScalar)-10.0, (btScalar)0.0);
	m_elapsedTime = (btScalar) (1.0 / 60.0);
}

DynamicsWorld::~DynamicsWorld(void)
{
	Delete(); 
}

btDynamicsWorld*	DynamicsWorld::GetBulletDynamicsWorld()
{
	return m_dynamicsWorld;
}

btScalar DynamicsWorld::GetDeltaTimeMilliseconds()
{
	#ifdef USE_BT_CLOCK
			m_elapsedTime = (btScalar)m_clock.getTimeMilliseconds();
			m_clock.reset();
			return m_elapsedTime;
	#else
			return btScalar(16666.);
	#endif
}

void	DynamicsWorld::Initialize()
{
	//glDebugDraw = new GLDebugDrawer();

	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new	btCollisionDispatcher( m_collisionConfiguration );

	int  maxProxies = 1048;
	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);
	m_broadphase = new bt32BitAxisSweep3(worldAabbMin, worldAabbMax,maxProxies);

	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	m_solver = sol;

	m_dynamicsWorld = new btDiscreteDynamicsWorld( m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration );
	m_dynamicsWorld->getSolverInfo().m_splitImpulse = true;
	m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = (btScalar)0.00001;
	
	//SITDS:These lines make Kinematic Controller objects collide with static
	m_ghostPairCallback = new btGhostPairCallback();
    m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback( m_ghostPairCallback );

	m_dynamicsWorld->setGravity( m_gravity );

	//Call Backs
	//NEEDED TO FIX INTERNAL EDGES PROBLEMS WITH btBvhTriangleMeshShapes Bug---
	//http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=6662&p=24566#p24566
    gContactAddedCallback = CustomMaterialCombinerCallback;

	m_dynamicsWorld->setInternalTickCallback( &DynamicsWorld::PostTickCallback );

	//m_dynamicsWorld->setDebugDrawer( glDebugDraw );

	//glDebugDraw->setDebugMode( btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb |
												//btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawConstraintLimits	);
}

void DynamicsWorld::PostTickCallback(btDynamicsWorld *world, btScalar timeStep) 
{
	for ( int i = 0; i < world->getCollisionObjectArray().size(); i++ )
	{
		btRigidBody* body = btRigidBody::upcast( world->getCollisionObjectArray()[i] );
		if ( body!=NULL && body->isActive() )
		{
			btVector3 velocity = body->getLinearVelocity();
			btScalar speed = velocity.length();
			btScalar maxLinearVelocity = ( ( AGKMotionState* )body->getMotionState() )->GetMaxLinearVelocity();
			if ( speed > maxLinearVelocity )
			{
				velocity *= maxLinearVelocity/speed;
				body->setLinearVelocity( velocity );
			}
			//TODO:SITDS Add Limiting angular velocity.
		}
	}
}

void	DynamicsWorld::Delete() 
{
	CharacterController* pController = characterControllerManager.GetFirst();
	while ( pController )
	{
		delete pController;		
		pController = characterControllerManager.GetNext();
	}
	characterControllerManager.ClearAll();
	
	Joint* pJoint = jointManager.GetFirst();
	while ( pJoint )
	{
		delete pJoint;		
		pJoint = jointManager.GetNext();
	}
	jointManager.ClearAll();
	
	RagDoll* pRagdoll = ragDollManager.GetFirst();
	while ( pRagdoll )
	{
		delete pRagdoll;		
		pRagdoll = ragDollManager.GetNext();
	}
	ragDollManager.ClearAll();
	
	Ray* pRay = rayManager.GetFirst();
	while ( pRay )
	{
		delete pRay;		
		pRay = rayManager.GetNext();
	}
	rayManager.ClearAll();
	
	StaticPlane* pPlane = staticPlaneManager.GetFirst();
	while ( pPlane )
	{
		delete pPlane;		
		pPlane = staticPlaneManager.GetNext();
	}
	staticPlaneManager.ClearAll();
	
	RigidBody* pBody = rigidBodyManager.GetFirst();
	while ( pBody )
	{
		delete pBody;		
		pBody = rigidBodyManager.GetNext();
	}
	rigidBodyManager.ClearAll();

	//SITDS:Must be deleted in reverse order of creation. 
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_broadphase;
	delete m_ghostPairCallback;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}

void	DynamicsWorld::Reset()
{
	Delete();
	Initialize();
}

void DynamicsWorld::ApplyDefaultCcd( btRigidBody* body )
{
	if( body->getInvMass() > 0.0 && !body->isStaticOrKinematicObject() )
	{
		btVector3 bbDimensions;
		btScalar diameter;
		bbDimensions = ( ( btBoxShape* )body->getCollisionShape() )->getImplicitShapeDimensions();
	    diameter = bbDimensions.getX();
		diameter = bbDimensions.getY() > 0.0 && bbDimensions.getY() < diameter ? bbDimensions.getY() : diameter;
		diameter = bbDimensions.getZ() > 0.0 && bbDimensions.getZ() < diameter ? bbDimensions.getZ() : diameter;
		body->setCcdMotionThreshold( (btScalar) (diameter / 2.0) );
		body->setCcdSweptSphereRadius( diameter );
	}
}

int DynamicsWorld::GetNumberOfActiveObjects()
{
	int activeObjects = 0;
	for ( int i = m_dynamicsWorld->getNumCollisionObjects() -1; i >= 0 ; i-- )
	{
		btRigidBody* body = btRigidBody::upcast( m_dynamicsWorld->getCollisionObjectArray()[i] ); 
		if ( body && body->isActive() ) 
		{
			activeObjects++;
		}
	}
	return activeObjects;
}

//TODO: SITDS Possibly Move to RigidBodyManager
int DynamicsWorld::GetObjectID( btRigidBody* body ) 
{
	//TODO:SITDS Return error if the RigidBody is NULL this should not happen!
	if ( body )
	{
		try
		{
			AGKMotionState* agkMotionState = ( AGKMotionState* )body->getMotionState();
			if( agkMotionState )
			{
				return agkMotionState->GetObjID();
			}
		}
		catch ( int /*error*/ )
		{
			//TODO:SITDS Return Error Message to AGK, 
			//Either the MotionState was NULL or was not a AGKMotionState, 
			//this circumstance should never occur though.
		}
	}
	return -1;
}

btRigidBody* DynamicsWorld::GetRigidBodyFromObjectID( int objID )
{
	for ( int i = m_dynamicsWorld->getNumCollisionObjects()-1; i >= 0 ; i-- )
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast( obj );
		if ( body && body->getMotionState() )
		{
			try
			{
				AGKMotionState* agkMotionState = (AGKMotionState*)body->getMotionState();
				if ( agkMotionState!=NULL && agkMotionState->GetObjID() == objID )
				{
					return body;
				}
			}
			catch ( int /*error*/ )
			{
				//TODO:SITDS Return Error Message to AGK, 
				//Either the MotionState was NULL or was not a AGKMotionState, 
				//this circumstance should never occur though.
			}
		}
	}
	return NULL;
}

void DynamicsWorld::UpdateRagdolls()
{
	RagDoll *pRagDoll = ragDollManager.GetFirst();
	while ( pRagDoll )
	{
		if(pRagDoll->IsSleeping()==false)
		{
			pRagDoll->Update();
		}
		pRagDoll = ragDollManager.GetNext();
	}
}

void DynamicsWorld::UpdateCharacterControllers()
{
	CharacterController *pController = characterControllerManager.GetFirst();
	while ( pController )
	{
		pController->Update(GetCurrentDynamicsWorld()->m_elapsedTime);
		pController = characterControllerManager.GetNext();
	}
}

//CallBack for BVH Triangle meshes
//--- STUFF NEEDED TO FIX INTERNAL EDGES PROBLEMS WITH btBvhTriangleMeshShapes ---
//http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=6662&p=24566#p24566
bool  DynamicsWorld::CustomMaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,
	int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
{
    btAdjustInternalEdgeContacts(cp,colObj1Wrap,colObj0Wrap, partId1,index1);
    btAdjustInternalEdgeContacts(cp,colObj1Wrap,colObj0Wrap, partId1,index1, BT_TRIANGLE_CONVEX_BACKFACE_MODE);
    btAdjustInternalEdgeContacts(cp,colObj1Wrap,colObj0Wrap, partId1,index1, BT_TRIANGLE_CONVEX_DOUBLE_SIDED+BT_TRIANGLE_CONCAVE_DOUBLE_SIDED);
    return false;
}
