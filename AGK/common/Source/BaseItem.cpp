
//Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 

#include "agk.h"

BaseItem::BaseItem(int id)
{
	m_id = id;
}

BaseItem::~BaseItem()
{
}

int BaseItem::GetID() const
{
	return m_id;
}

bool BaseItem::operator< (const BaseItem&c) const{ return GetID() < c.GetID(); }
bool BaseItem::operator> (const BaseItem& c) const{ return GetID() > c.GetID(); }
bool BaseItem::operator<= (const BaseItem& c) const{ return GetID() <= c.GetID(); }
bool BaseItem::operator>= (const BaseItem& c) const{ return GetID() >= c.GetID(); }
bool BaseItem::operator== (const BaseItem& c) const{ return GetID() == c.GetID(); }
bool BaseItem::operator!= (const BaseItem& c) const{ return GetID() != c.GetID(); }