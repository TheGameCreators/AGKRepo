
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

struct	RayResult 
{
	RayResult()
	{
	}
	const btCollisionObject* m_collisionObject;
	btVector3	m_hitNormalWorld;
	btVector3	m_hitPointWorld;
	btScalar m_hitFraction;
};

class RayResultSortPredicate
{
	public:
		bool operator() ( const RayResult* a, const RayResult* b ) const
		{
			 return a->m_hitFraction < b->m_hitFraction;
		}
};

class Ray : public BaseItem
{
public:
	Ray(int rayID);
	virtual ~Ray();
	//
	void ConvexCast(btScalar radius, btVector3& from, btVector3& to );
	void Cast( btVector3& from, btVector3& to, bool allContacts );
	void Cast( btVector3& from, btVector3& to, bool allContacts, int flags );
	int GetNumberOfContacts(); 
	int GetContactObjectID( int hitFractionIndex );
	int GetClosestContactObjectID();
	btVector3 GetContactPosition( int hitFractionIndex );
	btVector3 GetClosestContactPosition();
	float GetClosestHitFraction();
	btVector3 GetHitNormal();
private: 
	void ClearContacts();
	void AllContacts( int flags );
	void ClosestConvexContact(btSphereShape& sphereShape, int flags);

	void ClosestContact( int flags );
	void CopyRayResultCallback( btCollisionWorld::AllHitsRayResultCallback &allResults );
	void CopyRayResultCallback( btCollisionWorld::ClosestRayResultCallback &closestResult );
	void CopyRayConvexResultCallback( btCollisionWorld::ClosestConvexResultCallback &closestResult );
	//
	btAlignedObjectArray<RayResult*> m_rayResults;
	btVector3 m_from;
	btVector3 m_to;
	btScalar m_closestHitFraction;
	btVector3	m_hitNormalWorld;
	const btCollisionObject* m_closestCollisionObject;
	short int	m_collisionFilterGroup;
	short int	m_collisionFilterMask;
	unsigned int m_flags;
	bool m_hasHit;
};
