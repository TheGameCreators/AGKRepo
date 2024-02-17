
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

Vector::Vector(int vectorID,  AGKVector* agkVector) : BaseItem( vectorID )
{
	m_agkVector = agkVector;
}

Vector::~Vector()
{
	delete m_agkVector;
}

AGKVector* Vector::GetAGKVector()
{
	return m_agkVector;
}