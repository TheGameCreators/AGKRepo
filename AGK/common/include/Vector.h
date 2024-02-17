
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 
#pragma once

#include "3DMath.h"

class Vector : public BaseItem
{
public:
		Vector(int vectorID, AGK::AGKVector* agkVector );
		virtual ~Vector();

	    AGK::AGKVector* GetAGKVector();

private:
	AGK::AGKVector* m_agkVector;
};