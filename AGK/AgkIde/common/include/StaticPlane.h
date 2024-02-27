
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

class StaticPlane : public BaseItem
{
public:
	StaticPlane( int planeID, btRigidBody* body );
	virtual ~StaticPlane();
	btRigidBody* GetPlane();

	
private:
	btRigidBody* m_StaticPlane;


};