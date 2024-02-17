
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 
#pragma once

class BaseItem
{
public:
	BaseItem(int id);
	virtual ~BaseItem();
	int GetID() const;
	bool operator < (const BaseItem&) const; 
	bool operator > (const BaseItem&) const; 
	bool operator <= (const BaseItem&) const;
	bool operator >= (const BaseItem&) const; 
	bool operator == (const BaseItem&) const; 
	bool operator != (const BaseItem&) const; 
	static bool sortfnc ( const BaseItem* a, const BaseItem* b ) 
	{
			return *a < *b; 
	}
protected: 
	int m_id;
};


