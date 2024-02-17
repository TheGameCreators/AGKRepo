/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <Box2D/Common/b2StackAllocator.h>
#include <Box2D/Common/b2Math.h>

b2StackAllocator::b2StackAllocator()
{
	m_index = 0;
	m_allocation = 0;
	m_maxAllocation = 0;
	m_entryCount = 0;
}

b2StackAllocator::~b2StackAllocator()
{
	b2Assert(m_index == 0);
	b2Assert(m_entryCount == 0);
}

void* b2StackAllocator::Allocate(int32 size)
{
	b2Assert(m_entryCount < b2_maxStackEntries);

	b2StackEntry* entry = m_entries + m_entryCount;
	entry->size = size;
	if (m_index + size > b2_stackSize)
	{
		entry->data = (char*)b2Alloc(size);
		entry->usedMalloc = true;
	}
	else
	{
		entry->data = m_data + m_index;
		entry->usedMalloc = false;
		m_index += size;
	}

	m_allocation += size;
	m_maxAllocation = b2Max(m_maxAllocation, m_allocation);
	++m_entryCount;

	return entry->data;
}

void b2StackAllocator::Free(void* p)
{
	b2Assert(m_entryCount > 0);
	b2StackEntry* entry = m_entries + m_entryCount - 1;
	b2Assert(p == entry->data);
	if (entry->usedMalloc)
	{
		b2Free(p);
	}
	else
	{
		m_index -= entry->size;
	}
	m_allocation -= entry->size;
	--m_entryCount;

	p = NULL;
}

int32 b2StackAllocator::GetMaxAllocation() const
{
	return m_maxAllocation;
}

// Temp Allocator

b2TempAllocator::b2TempAllocator() 
{ 
	m_pDataList = new b2TempEntry;
	m_pDataList->data = new char[ TEMP_ALLOC_SIZE ];
	m_pDataList->index = 0;
	m_pDataList->size = TEMP_ALLOC_SIZE;
	m_pDataList->pNext = 0;
}

b2TempAllocator::~b2TempAllocator() 
{
	while ( m_pDataList )
	{
		b2TempEntry *entry = m_pDataList;
		m_pDataList = m_pDataList->pNext;
		delete [] entry->data;
		delete entry;
	}
}

void* b2TempAllocator::Allocate(int32 size)
{
	m_lock.Acquire();
	if ( m_pDataList->index + size > m_pDataList->size )
	{
		// create new entry, this will now be used for all allocations
		int newSize = TEMP_ALLOC_SIZE;
		if ( size > newSize ) newSize = size;
		b2TempEntry *entry = new b2TempEntry;
		entry->data = new char[ newSize ];
		entry->index = 0;
		entry->size = newSize;
		entry->pNext = m_pDataList; 
		m_pDataList = entry;
	}
	
	char* data = m_pDataList->data + m_pDataList->index;
	m_pDataList->index += size;
	m_lock.Release();
	
	return data;
}

void b2TempAllocator::Reset() 
{ 
	// delete all but one, keep that to save memory allocations next time
	while ( m_pDataList->pNext )
	{
		b2TempEntry *entry = m_pDataList;
		m_pDataList = m_pDataList->pNext;
		delete [] entry->data;
		delete entry;
	}

	m_pDataList->index = 0;
}
