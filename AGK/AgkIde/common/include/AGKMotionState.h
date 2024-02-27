
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

class AGKMotionState : public btMotionState
{
public:
	AGKMotionState(const btTransform &initialpos, int objID);
	~AGKMotionState(void);
	int GetObjID();
	void SetObjID(int objID);
	btScalar GetMaxLinearVelocity();
	void SetMaxLinearVelocity(btScalar maxLinearVelocity);
	virtual void getWorldTransform(btTransform &worldTrans) const ;
	virtual void setWorldTransform(const btTransform &worldTrans);
	static void setWorldTransform(int objID, const btTransform &worldTrans, const btVector3& initialRotation );
	static void setWorldTransform(int objID, const btTransform &worldTrans);
protected:
	int m_objID;
	btScalar m_maxLinearVelocity;
	btTransform m_Pos1;
};

