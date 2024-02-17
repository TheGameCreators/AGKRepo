
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#pragma once
class ContactReport {
	public:
		ContactReport(int objA, int objB, btManifoldPoint& pos);
		virtual ~ContactReport();

		// align to 16 bytes for btManifoldPoint
		void* operator new(size_t i) { return _mm_malloc(i,16); }
		void operator delete(void* p) { _mm_free(p); }

		int GetObjectA();
		int GetObjectB();
		const btVector3& GetContactPosition() const;
		static ContactReport* GetContact();
		static  ContactReport* GetNextContact();
		static bool ContactExists( int objA, int objB );

	private:
		btManifoldPoint m_manifoldPoint;
		int m_objA;
		int m_objB;
};

extern int contactReportIndex;
extern btAlignedObjectArray<ContactReport*> contactReports;
