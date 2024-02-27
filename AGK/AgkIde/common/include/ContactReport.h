
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once
class ContactReport {
	public:
		ContactReport(int objA, int objB, btManifoldPoint& pos);
		virtual ~ContactReport();

		int GetObjectA();
		int GetObjectB();
		const btVector3& GetContactPosition() const;
		static ContactReport* GetContact();
		static  ContactReport* GetNextContact();
		static bool ContactExists( int objA, int objB );

	private:
		int m_objA;
		int m_objB;
		btManifoldPoint m_manifoldPoint;
};

extern int contactReportIndex;
extern btAlignedObjectArray<ContactReport*> contactReports;