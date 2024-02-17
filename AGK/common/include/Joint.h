
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once

class Joint : public BaseItem
{
public:
	Joint(int jointID, btTypedConstraint* constraint);
	virtual ~Joint();

	btTypedConstraint* GetConstraint();
	btTransform* GetFrameOffsetA();  
	btTransform* GetFrameOffsetB();   

private:
	btTypedConstraint* m_constraint;
};















