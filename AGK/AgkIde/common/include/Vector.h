
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 
#pragma once


class Vector : public BaseItem
{
public:
		Vector(int vectorID, AGKVector* agkVector );
		virtual ~Vector();

	    AGKVector* GetAGKVector();

private:
	AGKVector* m_agkVector;
};