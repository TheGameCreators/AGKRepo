
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

//TODO:SITDS not sure if this is the right place for this.
struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	btManifoldPoint m_cp;
	bool m_connected;
	btScalar m_margin;
	MyContactResultCallback() :m_connected(false),m_margin((btScalar)0.05)
	{
	}
	virtual   btScalar   addSingleResult(btManifoldPoint& cp,const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
	{
			
		if (cp.getDistance()<=m_margin)
		{
			m_connected = true;
			m_cp = cp;
		}
		return 1.f;
	}
};

class DynamicsWorld
{
public:
	DynamicsWorld( void );
	DynamicsWorld( btScalar scaleFactor ); 
	~DynamicsWorld( void );

	btDynamicsWorld*		m_dynamicsWorld;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher*	m_dispatcher;
	btBroadphaseInterface*	m_broadphase;
	btAxisSweep3* m_overlappingPairCache;
	btConstraintSolver*	m_solver;
		
	btScalar m_scaleFactor; //What the AGK unit equals; inches,feet,meters,centimeters,etc.
	btVector3 m_gravity;
	btScalar m_elapsedTime;
			
	btDynamicsWorld*	 GetBulletDynamicsWorld();
	btScalar GetDeltaTimeMilliseconds();
	void Initialize();
	void Delete();
	void Reset();
	static void ApplyDefaultCcd( btRigidBody* body );
	static void UpdateRagdolls();
	static void UpdateCharacterControllers();
	int GetNumberOfActiveObjects();
	int GetObjectID( btRigidBody* body );
	btRigidBody* GetRigidBodyFromObjectID( int objID );
	btScalar GetTimeMultiplier();

	static void PostTickCallback( btDynamicsWorld *world, btScalar timeStep );
	static bool CustomMaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1);
	#ifdef USE_BT_CLOCK
		btClock m_clock;
	#endif //USE_BT_CLOCK

private:
	btGhostPairCallback* m_ghostPairCallback;
};

//In case we need more than one physics world, possible use multiplayer.
extern btAlignedObjectArray<DynamicsWorld*> DynamicsWorldArray;
extern int currentDynamicsWorldID;
extern btDynamicsWorld* GetCurrentBulletDynamicsWorld();
extern DynamicsWorld* GetCurrentDynamicsWorld();
//
extern cHashedList<RigidBody> rigidBodyManager;
extern cHashedList<Joint> jointManager;
extern cHashedList<StaticPlane> staticPlaneManager;
extern cHashedList<Vector> vectorManager;
extern cHashedList<Ray> rayManager;
extern cHashedList<CharacterController> characterControllerManager;
extern cHashedList<RagDoll> ragDollManager;