
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

int contactReportIndex = 0;
btAlignedObjectArray<ContactReport*> contactReports;

ContactReport::ContactReport( int objA, int objB, btManifoldPoint& pos)
{
	m_objA = objA;
	m_objB = objB;
	m_manifoldPoint = pos;
}

ContactReport::~ContactReport()
{
	
}

bool ContactReport::ContactExists( int objA, int objB )
{
	do
	{
		ContactReport* contact = GetContact();
		if (contact != NULL && contact->GetObjectA() == objA && contact->GetObjectB() == objB )
		{
			return true;
		}
	}
	while( GetNextContact() != NULL );
	return false;
}

int ContactReport::GetObjectA(){
	return m_objA;
}
int ContactReport::GetObjectB(){
	return m_objB;
}
const btVector3& ContactReport::GetContactPosition() const{
	return m_manifoldPoint.getPositionWorldOnB();
}

ContactReport* ContactReport::GetContact(){
	return contactReportIndex < contactReports.size() ? contactReports[contactReportIndex] : NULL;
}

ContactReport* ContactReport::GetNextContact(){
	return contactReportIndex < contactReports.size() - 1 ? contactReports[++contactReportIndex] : NULL;
}

