
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

Ray::Ray(int rayID )  : BaseItem( rayID )
{
	m_hasHit = false;
}

Ray::~Ray()
{
}

int Ray::GetNumberOfContacts()
{
	return m_rayResults.size();
}

int Ray::GetContactObjectID(int hitFractionIndex)
{
	int objectID = -1;//Error Value
	if(hitFractionIndex >-1 && hitFractionIndex < m_rayResults.size())
	{
		try
		{
			btRigidBody* rigidBody = (btRigidBody*)btRigidBody::upcast(m_rayResults[hitFractionIndex]->m_collisionObject);
			objectID = DynamicsWorldArray[0]->GetObjectID(rigidBody); 
		}
		catch(int)
		{

		}
	}
	return objectID;
}

int Ray::GetClosestContactObjectID()
{
	int objectID = -1;
	if(m_rayResults.size()>0)
	{
		try
		{
			btRigidBody* body = (btRigidBody*)btRigidBody::upcast(m_rayResults[0]->m_collisionObject);
			objectID = GetCurrentDynamicsWorld()->GetObjectID(body);
		}
		catch(int)
		{

		}
	}
	return objectID; 
}

btVector3 Ray::GetContactPosition(int hitFractionIndex)
{
	btVector3 contactPos = btVector3(-1.0,-1.0,-1.0);//TODO: SITDS Error Value
	if(hitFractionIndex >-1 && hitFractionIndex < m_rayResults.size())
	{
		contactPos = m_from.lerp(m_to,m_rayResults[hitFractionIndex]->m_hitFraction);
	}
	return contactPos;
}

btVector3 Ray::GetClosestContactPosition()
{
	if(m_rayResults.size()>0)
	{
		return m_from.lerp(m_to,m_rayResults[0]->m_hitFraction);
	}
	return btVector3(-1.0,-1.0,-1.0);//TODO: SITDS Return Error Value
}

void Ray::Cast( btVector3& from, btVector3& to, bool allContacts )
{
	m_from = from;
	m_to = to;
	unsigned int flag = 0;
	if ( allContacts )
	{
		flag |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
		flag |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
		AllContacts(flag);
	}
	else
	{
		flag |= btTriangleRaycastCallback::kF_FilterBackfaces;
		ClosestContact(flag);
	}
}

void Ray::ConvexCast(btScalar radius, btVector3& from, btVector3& to )
{
	m_from = from;
	m_to = to;
	btSphereShape sphereShape(radius);
	unsigned int flag = 0;
	flag |= btTriangleRaycastCallback::kF_FilterBackfaces;
	ClosestConvexContact(sphereShape, flag);
}

void Ray::Cast(btVector3& from, btVector3& to, bool allContacts, int flags)
{
	m_from = from;
	m_to = to;
	if(allContacts)
	{
		AllContacts(flags);
	}
	else
	{
		ClosestContact(flags);
	}
}

void Ray::ClearContacts()
{
	m_hasHit = false;
	for(int i = 0; i < m_rayResults.size(); i++)
	{
		delete m_rayResults[i];
	}
	m_rayResults.clear();
}

void Ray::CopyRayResultCallback(btCollisionWorld::AllHitsRayResultCallback &allResults)
{
	m_collisionFilterGroup = allResults.m_collisionFilterGroup;
	m_collisionFilterMask = allResults.m_collisionFilterMask;
	m_flags = allResults.m_flags;
	//
	m_closestCollisionObject = allResults.m_collisionObject;
	m_closestHitFraction = allResults.m_closestHitFraction;
	//
	for(int i = 0; i < allResults.m_hitFractions.size(); i++)
	{
		RayResult* ray = new RayResult();
		ray->m_hitFraction = allResults.m_hitFractions[i];
		ray->m_collisionObject = allResults.m_collisionObjects[i];
		ray->m_hitNormalWorld = allResults.m_hitNormalWorld[i];
		ray->m_hitPointWorld = allResults.m_hitPointWorld[i];
		m_rayResults.push_back(ray);
	}
	m_rayResults.quickSort(RayResultSortPredicate());
}

void Ray::CopyRayResultCallback(btCollisionWorld::ClosestRayResultCallback &closestResult)
{
	m_collisionFilterGroup = closestResult.m_collisionFilterGroup;
	m_collisionFilterMask = closestResult.m_collisionFilterMask;
	m_flags = closestResult.m_flags;
	m_hasHit = closestResult.hasHit();
	m_closestCollisionObject = closestResult.m_collisionObject;
	m_closestHitFraction = closestResult.m_closestHitFraction;

	RayResult* ray = new RayResult();
	ray->m_hitFraction = closestResult.m_closestHitFraction;
	ray->m_collisionObject = closestResult.m_collisionObject;
	ray->m_hitNormalWorld = closestResult.m_hitNormalWorld;
	ray->m_hitPointWorld = closestResult.m_hitPointWorld;
	m_rayResults.push_back(ray);
}

void Ray::CopyRayConvexResultCallback( btCollisionWorld::ClosestConvexResultCallback &closestResult )
{
	m_collisionFilterGroup = closestResult.m_collisionFilterGroup;
	m_collisionFilterMask = closestResult.m_collisionFilterMask;
	m_hasHit = closestResult.hasHit();
	m_closestCollisionObject = closestResult.m_hitCollisionObject;
	m_closestHitFraction = closestResult.m_closestHitFraction;

	RayResult* ray = new RayResult();
	ray->m_hitFraction = closestResult.m_closestHitFraction;
	ray->m_collisionObject = closestResult.m_hitCollisionObject;
	ray->m_hitNormalWorld = closestResult.m_hitNormalWorld;
	ray->m_hitPointWorld = closestResult.m_hitPointWorld;
	m_rayResults.push_back(ray);
}

void Ray::AllContacts(int flags)
{
	btVector3 hitPosition;
	btCollisionWorld::AllHitsRayResultCallback allResults(m_from,m_to);
	allResults.m_flags = flags;
	GetCurrentBulletDynamicsWorld()->rayTest(m_from, m_to, allResults);
	ClearContacts();
	if(allResults.m_hitFractions.size()>0)
	{
		CopyRayResultCallback(allResults);
	}
}

void Ray::ClosestContact(int flags)
{
	btVector3 hitPosition;
	btCollisionWorld::ClosestRayResultCallback closestResult(m_from, m_to);
	closestResult.m_flags = flags;
	GetCurrentBulletDynamicsWorld()->rayTest(m_from, m_to,closestResult);
	ClearContacts();
	if(closestResult.hasHit())
	{
		CopyRayResultCallback(closestResult);
	}
}

void Ray::ClosestConvexContact(btSphereShape& sphereShape, int flags)
{
	btVector3 hitPosition;
	btCollisionWorld::ClosestConvexResultCallback closestResult( m_from, m_to);
	btQuaternion qFrom(0.0, 0.0, 0.0, 1.0);
	btQuaternion qTo(0.0, 0.0, 0.0, 1.0);
	btTransform from(qFrom, m_from);
	btTransform to(qTo, m_to);
	//closestResult.m_flags = flags;
	GetCurrentBulletDynamicsWorld()->convexSweepTest(&sphereShape, from, to, closestResult );
	ClearContacts();
	if( closestResult.hasHit() )
	{
		CopyRayConvexResultCallback( closestResult );
	}
}

float Ray::GetClosestHitFraction()
{
	return m_closestHitFraction;
}

btVector3 Ray::GetHitNormal()
{
	return m_hitNormalWorld;
}
