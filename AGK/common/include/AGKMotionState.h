
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

	// align to 16 bytes for btTransform
	void* operator new(size_t i) { return _mm_malloc(i,16); }
    void operator delete(void* p) { _mm_free(p); }

	int GetObjID();
	void SetObjID(int objID);
	btScalar GetMaxLinearVelocity();
	void SetMaxLinearVelocity(btScalar maxLinearVelocity);
	virtual void getWorldTransform(btTransform &worldTrans) const ;
	virtual void setWorldTransform(const btTransform &worldTrans);
	static void setWorldTransform(int objID, const btTransform &worldTrans, const btVector3& initialRotation );
	static void setWorldTransform(int objID, const btTransform &worldTrans);
protected:
	btTransform m_Pos1;
	btScalar m_maxLinearVelocity;
	int m_objID;
};

