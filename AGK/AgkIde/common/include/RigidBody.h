
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

//Class for RigidBody Manager
class RigidBody : public BaseItem
{
public:
	RigidBody(btRigidBody* rigidBody, int bodyID);
	virtual ~RigidBody();
	btRigidBody* GetRigidBody();
	int GetCollissionGroup();
	void SetCollissionGroup(int collissionGroup);
	int GetCollissionMask();
	void SetCollissionMask(int collissionMask);
private:
	btRigidBody* m_rigidBody;
	int m_collissionGroup;
	int m_collissionMask;
};










