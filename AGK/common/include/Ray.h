
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
	
	// align to 16 bytes for btVector3
	void* operator new(size_t i) { return _mm_malloc(i,16); }
	void operator delete(void* p) { _mm_free(p); }

	btVector3	m_hitNormalWorld;
	btVector3	m_hitPointWorld;
	const btCollisionObject* m_collisionObject;
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

	// align to 16 bytes for btVector3
	void* operator new(size_t i) { return _mm_malloc(i,16); }
    void operator delete(void* p) { _mm_free(p); }
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
	btVector3 m_from;
	btVector3 m_to;
	btVector3	m_hitNormalWorld;
	btAlignedObjectArray<RayResult*> m_rayResults;
	btScalar m_closestHitFraction;
	const btCollisionObject* m_closestCollisionObject;
	short int	m_collisionFilterGroup;
	short int	m_collisionFilterMask;
	unsigned int m_flags;
	bool m_hasHit;
};
