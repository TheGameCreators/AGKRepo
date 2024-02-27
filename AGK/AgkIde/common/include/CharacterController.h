
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once


class btKinematicClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
	btKinematicClosestNotMeRayResultCallback (btCollisionObject* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_me = me;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == m_me)
			return 1.0;

		return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
	}
protected:
	btCollisionObject* m_me;
};

class btKinematicClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	btKinematicClosestNotMeConvexResultCallback (btCollisionObject* me, const btVector3& up, btScalar minSlopeDot)
	: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	, m_me(me)
	, m_up(up)
	, m_minSlopeDot(minSlopeDot)
	{
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == m_me)
			return btScalar(1.0);

		if (!convexResult.m_hitCollisionObject->hasContactResponse())
			return btScalar(1.0);

		btVector3 hitNormalWorld;
		if (normalInWorldSpace)
		{
			hitNormalWorld = convexResult.m_hitNormalLocal;
		} else
		{
			///need to transform normal into worldspace
			hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis()*convexResult.m_hitNormalLocal;
		}

		btScalar dotUp = m_up.dot(hitNormalWorld);
		if (dotUp < m_minSlopeDot) {
			return btScalar(1.0);
		}

		return ClosestConvexResultCallback::addSingleResult (convexResult, normalInWorldSpace);
	}
protected:
	btCollisionObject* m_me;
	const btVector3 m_up;
	btScalar m_minSlopeDot;
};

class CharacterController : public btKinematicCharacterController, public BaseItem
{
public:

	CharacterController(int objectID, int debugObjectID, int debugObjectID_Duck, btPairCachingGhostObject* ghostObject,btConvexShape* convexShape,btConvexShape* convexShapeDuck, btScalar stepHeight, const btVector3& objectOffset, int upAxis = 1);
	virtual ~CharacterController();
	static void Create( int objectID, int axis, const btVector3& objectOffset, const btVector3& objOrientation, btScalar crouchScale );
	void setWalkDirection(const btVector3& walkDirection);
	void Update(btScalar elapsedTime);
	void SetWalkVelocity(btScalar walkVelocity);
	void playerStep(btCollisionWorld * collisionWorld, btScalar dt);
	btScalar GetInvMass();
	void SetStepHeight(btScalar stepheight);
	void Crouch();
	void Stand();
	bool CanStand();
	void SetCrouchConvexShape(btConvexShape * shape);
	void SetCrouchScale(btScalar crouchScale);
	void SetObjectInitialRotation(const btVector3& rotation);
	void SetRotation(btMatrix3x3& rotation);
	int GetDebugObjectID();
	void SetDebugMode( bool isDebug );
	bool GetDebugMode();
	void SetPosition( const btVector3& pos );

private:
	int m_debugObjectID;
	int m_debugObjectID_Crouch;
	btVector3 m_objectInitialRotation;
	btMatrix3x3 m_yAngleRotation;
	btScalar m_crouchScale;
	bool m_isCrouching;
	bool m_isStanding;
	bool m_isDebugMode;

	btConvexShape * m_standingConvexShape;
	btConvexShape * m_crouchConvexShape;
	bool m_canStand;
	btVector3 m_objectOffset;///AGK object offset vector
	btScalar m_mass;
	btScalar m_walkVelocity;
	void setRBForceImpulseBasedOnCollision();
};










