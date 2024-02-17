// Text Header
#ifndef _H_AGK_HLIST_
#define _H_AGK_HLIST_

// Common includes
#include "Common.h"

// Namespace
namespace AGK
{
	// An array type structure to hold a list of pointers for constant time access, but using linked lists 
	// when the number of items exceeds iSize for unlimited IDs without pre-assigning a large array.
	// Creates an array of size iSize (default 1024), with each element being a linked list of items.
	// For example ID 1024 and ID 2048 would be stored in the same array index, but linked one after the other.
	template<class T> class cHashedList
	{
		protected:

			class cHashedItem
			{
				public:
					union
					{
						const void *m_ptrIndex;
						char *m_strIndex;
						uint32_t m_intIndex;
					};
					cHashedItem *m_pNextItem;
					T m_pItem;
					unsigned int m_iItemStrLength;

					cHashedItem() { m_iItemStrLength = 0; m_ptrIndex = 0; m_pItem = 0; m_pNextItem = 0; }
					~cHashedItem() { if ( m_iItemStrLength ) delete [] m_strIndex; }
			};

			cHashedItem **m_pHashedItems = 0;
			cHashedItem *m_pIter = 0;
			uint32_t m_iIterIndex = 0;
			uint32_t m_iListSize = 0;
			uint32_t m_iLastID = 100000;
			uint32_t m_iItemCount = 0;
			uint32_t m_iPower = 0;

			uint32_t HashIndexInt( uint32_t iIndex ) const;
			uint32_t HashIndexStr( const char *szIndex, unsigned int *pLength=0 ) const;
			uint32_t HashIndexPtr( const void *ptr ) const;

		public:
			cHashedList( uint32_t iSize=1024 );
			~cHashedList();
			void Resize( uint32_t iSize );

			void Clone( cHashedList<T>* pOther );

			void AddItem( T pItem, uint32_t iID );
			T GetItem( uint32_t iID ) const;
			T GetFirst();
			T GetNext();
			T RemoveItem( uint32_t iID );
			uint32_t GetFreeID( uint32_t max=0x7fffffff ) const;
			void ClearAll( );
			uint32_t GetCount() const { return m_iItemCount; }

			// string versions
			void AddItem( T pItem, const char* szID );
			T GetItem( const char* szID ) const;
			T RemoveItem( const char* szID );

			// pointer versions
			void AddItem( T pItem, const void *ptr );
			T GetItem( const void *ptr ) const;
			T RemoveItem( const void *ptr );
	};

	template<class T> cHashedList<T>::cHashedList( uint32_t iSize )
	{
		if ( iSize < 2 ) iSize = 2;
		if ( iSize > 0x7fffffff ) iSize = 0x7fffffff;

		// find the next highest power of 2 
		uint32_t iNewSize = 1;
		m_iPower = 0;
		while ( iNewSize < iSize ) 
		{
			iNewSize = iNewSize << 1;
			m_iPower++;
		}
		m_iListSize = iNewSize;

		m_pHashedItems = new cHashedItem*[ m_iListSize ];
		for( uint32_t i = 0; i < m_iListSize; i++ )
		{
			m_pHashedItems[ i ] = 0;
		}
	}

	template<class T>cHashedList<T>::~cHashedList()
	{
		ClearAll( );
		delete [] m_pHashedItems;
	}

	template<class T> void cHashedList<T>::Resize( uint32_t iSize )
	{
		if ( m_pHashedItems ) 
		{
			ClearAll();
			delete [] m_pHashedItems;
		}

		if ( iSize < 2 ) iSize = 2;
		if ( iSize > 0x7fffffff ) iSize = 0x7fffffff;

		// find the next highest power of 2 
		uint32_t iNewSize = 1;
		m_iPower = 0;
		while ( iNewSize < iSize ) 
		{
			iNewSize = iNewSize << 1;
			m_iPower++;
		}
		m_iListSize = iNewSize;

		m_pHashedItems = new cHashedItem*[ m_iListSize ];
		for( uint32_t i = 0; i < m_iListSize; i++ )
		{
			m_pHashedItems[ i ] = 0;
		}

		m_pIter = 0;
		m_iIterIndex = 0;

		m_iLastID = 100000;
		m_iItemCount = 0;
	}

	template<class T> void cHashedList<T>::Clone( cHashedList<T>* pOther )
	{
		m_pIter = pOther->m_pIter;
		m_iIterIndex = pOther->m_iIterIndex;
		m_iLastID = pOther->m_iLastID;
		m_iItemCount = pOther->m_iItemCount;
		m_iPower = pOther->m_iPower;
		m_iListSize = pOther->m_iListSize;
		if ( pOther->m_pHashedItems ) m_pHashedItems = new cHashedItem*[ m_iListSize ];
	
		for ( uint32_t i = 0; i < pOther->m_iListSize; i++ )
		{
			m_pHashedItems[ i ] = 0;

			cHashedItem *pCurrItem = pOther->m_pHashedItems[ i ];
			while ( pCurrItem )
			{
				cHashedItem *pNewItem = new cHashedItem();
				*pNewItem = *pCurrItem;
				if ( pNewItem->m_iItemStrLength > 0 ) 
				{
					pNewItem->m_strIndex = new char[ pNewItem->m_iItemStrLength+1 ];
					memcpy( pNewItem->m_strIndex, pCurrItem->m_strIndex, pNewItem->m_iItemStrLength+1 );
				}
				pNewItem->m_pNextItem = m_pHashedItems[ i ];
				m_pHashedItems[ i ] = pNewItem;			

				pCurrItem = pCurrItem->m_pNextItem;
			}
		}
	}

	template<class T>uint32_t cHashedList<T>::HashIndexInt( uint32_t iIndex ) const
	{
		iIndex = (iIndex >> 16) ^ iIndex;
		iIndex = (iIndex >> m_iPower) ^ iIndex;
		return iIndex & (m_iListSize-1); // m_iListSize must be a power of 2
	}

	template<class T>uint32_t cHashedList<T>::HashIndexStr( const char *szIndex, unsigned int *pLength ) const
	{
		uint32_t iIndex = 2166136261;
		while( *szIndex )
		{
			iIndex ^= *szIndex;
			iIndex *= 16777619;
			szIndex++;
			if ( pLength ) (*pLength)++;
		}

		iIndex = (iIndex >> 16) ^ iIndex;
		iIndex = (iIndex >> m_iPower) ^ iIndex;
		return iIndex & (m_iListSize-1); // m_iListSize must be a power of 2
	}

	template<class T>uint32_t cHashedList<T>::HashIndexPtr( const void *ptr ) const
	{
		unsigned long long ptr64 = (unsigned long long)ptr;
		unsigned int ptr32 = (unsigned int)ptr64;
		ptr32 = (ptr64 >> 32) ^ ptr32;
		ptr32 = (ptr32 >> 16) ^ ptr32;
		ptr32 = (ptr32 >> m_iPower) ^ ptr32;
		return ptr32 & (m_iListSize-1); // m_iListSize must be a power of 2
	}

	template<class T> void cHashedList<T>::AddItem( T pItem, uint32_t iID )
	{
		//if ( GetItem( iID ) ) return;
		uint32_t index = HashIndexInt( iID );

		cHashedItem *pNewItem = new cHashedItem();
		pNewItem->m_iItemStrLength = 0;
		pNewItem->m_intIndex = iID;
		pNewItem->m_pItem = pItem;

		pNewItem->m_pNextItem = m_pHashedItems[ index ];
		m_pHashedItems[ index ] = pNewItem;

		if ( iID > m_iLastID ) m_iLastID = iID;
		if ( m_iLastID > 0x7fffffff ) m_iLastID = 100000;

		m_iItemCount++;
	}

	template<class T> T cHashedList<T>::GetItem( uint32_t iID ) const
	{
		if ( m_iItemCount == 0 ) return 0;

		uint32_t index = HashIndexInt( iID );
		cHashedItem *pItem = m_pHashedItems[ index ];
		while( pItem )
		{
			if ( pItem->m_intIndex == iID ) return pItem->m_pItem;
			pItem = pItem->m_pNextItem;
		}

		return 0;
	}

	template<class T> T cHashedList<T>::GetFirst()
	{
		if ( m_iItemCount == 0 ) return 0;

		T pReturnItem = 0;
		m_pIter = 0;
		m_iIterIndex = 0;
		for ( uint32_t i = 0; i < m_iListSize; i++ )
		{
			cHashedItem *pCurrItem = m_pHashedItems[ i ];
			while ( pCurrItem )
			{
				if ( !pReturnItem ) pReturnItem = pCurrItem->m_pItem;
				else 
				{
					// find next ready for GetNext()
					m_pIter = pCurrItem;
					m_iIterIndex = i;
					return pReturnItem;
				}

				pCurrItem = pCurrItem->m_pNextItem;
			}
		}

		return pReturnItem;
	}

	template<class T> T cHashedList<T>::GetNext()
	{
		if ( !m_pIter ) return 0;
	
		T pReturnItem = m_pIter->m_pItem;
		if ( m_pIter->m_pNextItem ) 
		{
			m_pIter = m_pIter->m_pNextItem;
			return pReturnItem;
		}

		// find next item for next GetNext()
		for ( uint32_t i = m_iIterIndex+1; i < m_iListSize; i++ )
		{
			if ( m_pHashedItems[ i ] )
			{
				m_pIter = m_pHashedItems[ i ];
				m_iIterIndex = i;
				return pReturnItem;
			}		
		}

		m_pIter = 0;
		m_iIterIndex = 0;
		return pReturnItem;
	}

	template<class T> T cHashedList<T>::RemoveItem( uint32_t iID )
	{
		uint32_t index = HashIndexInt( iID );
		cHashedItem *pItem = m_pHashedItems[ index ];
		cHashedItem *pLast = 0;
		T pItemContent = 0;
	
		while( pItem )
		{
			if ( pItem->m_intIndex == iID )
			{
				// if this item is the current iterator then re-calculate the next step
				if ( pItem == m_pIter ) GetNext();
			
				// remove the item from the list
				if ( pLast ) pLast->m_pNextItem = pItem->m_pNextItem;
				else m_pHashedItems[ index ] = pItem->m_pNextItem;

				pItemContent = pItem->m_pItem;
				if ( m_iItemCount > 0 ) m_iItemCount--;

				// delete item, but not the contents, pass it back to be deleted.
				delete pItem;
				return pItemContent;
			}

			// next item
			pLast = pItem;
			pItem = pItem->m_pNextItem;
		}

		return 0;
	}

	template<class T> uint32_t cHashedList<T>::GetFreeID( uint32_t max ) const
	{
		uint32_t iID = m_iLastID;
		uint32_t start = iID;

		iID++;
		if ( iID > max ) 
		{
			iID = 1;
			start = max;
		}

		while ( GetItem( iID ) && iID != start )
		{
			iID++;
			if ( iID > max ) iID = 1;
		}

		if ( GetItem( iID ) ) iID = 0;
		return iID;
	}

	template<class T> void cHashedList<T>::ClearAll( )
	{
		if ( m_iItemCount > 0 )
		{
			for ( uint32_t i = 0; i < m_iListSize; i++ )
			{
				while( m_pHashedItems[ i ] )
				{
					cHashedItem *pItem = m_pHashedItems[ i ];
					m_pHashedItems[ i ] = m_pHashedItems[ i ]->m_pNextItem;

					// don't delete item contents, only the item itself
					delete pItem;
				}
			}
		}

		m_iLastID = 100000;
		m_iItemCount = 0;
		m_pIter = 0;
		m_iIterIndex = 0;
	}

	// String versions

	template<class T> void cHashedList<T>::AddItem( T pItem, const char *szID )
	{
		if ( !szID ) return;
		//if ( GetItem( szID ) ) return;
		unsigned int iLength = 0;
		uint32_t index = HashIndexStr( szID, &iLength );

		cHashedItem *pNewItem = new cHashedItem();
		pNewItem->m_iItemStrLength = iLength;
		pNewItem->m_strIndex = new char[ iLength+1 ];
		memcpy( pNewItem->m_strIndex, szID, iLength+1 );
		pNewItem->m_pItem = pItem;

		pNewItem->m_pNextItem = m_pHashedItems[ index ];
		m_pHashedItems[ index ] = pNewItem;

		m_iItemCount++;
	}

	template<class T> T cHashedList<T>::GetItem( const char *szID ) const
	{
		if ( m_iItemCount == 0 ) return 0;

		if ( !szID ) return 0;
		unsigned int iLength = 0;
		uint32_t index = HashIndexStr( szID, &iLength );
		cHashedItem *pItem = m_pHashedItems[ index ];
		while( pItem )
		{
			if ( pItem->m_iItemStrLength == iLength && pItem->m_strIndex && strcmp( szID, pItem->m_strIndex ) == 0 ) return pItem->m_pItem;
			pItem = pItem->m_pNextItem;
		}

		return 0;
	}

	template<class T> T cHashedList<T>::RemoveItem( const char *szID )
	{
		unsigned int iLength = 0;
		uint32_t index = HashIndexStr( szID, &iLength );
		cHashedItem *pItem = m_pHashedItems[ index ];
		cHashedItem *pLast = 0;
		T pItemContent = 0;
	
		while( pItem )
		{
			// check if this item matches target
			if ( pItem->m_iItemStrLength == iLength && pItem->m_strIndex && strcmp( szID, pItem->m_strIndex ) == 0 )
			{
				// if this item is the current iterator then re-calculate the next step
				if ( pItem == m_pIter ) GetNext();
			
				// remove the item from the list
				if ( pLast ) pLast->m_pNextItem = pItem->m_pNextItem;
				else m_pHashedItems[ index ] = pItem->m_pNextItem;

				pItemContent = pItem->m_pItem;
				m_iItemCount--;

				// delete item, but not the contents, pass it back to be deleted.
				delete pItem;
				return pItemContent;
			}

			// next item
			pLast = pItem;
			pItem = pItem->m_pNextItem;
		}

		return 0;
	}

	// Pointer versions

	template<class T> void cHashedList<T>::AddItem( T pItem, const void *ptr )
	{
		//if ( GetItem( ptr ) ) return;
		uint32_t index = HashIndexPtr( ptr );

		cHashedItem *pNewItem = new cHashedItem();
		pNewItem->m_iItemStrLength = 0;
		pNewItem->m_ptrIndex = ptr;
		pNewItem->m_pItem = pItem;

		pNewItem->m_pNextItem = m_pHashedItems[ index ];
		m_pHashedItems[ index ] = pNewItem;

		m_iItemCount++;
	}

	template<class T> T cHashedList<T>::GetItem( const void *ptr ) const
	{
		if ( m_iItemCount == 0 ) return 0;

		uint32_t index = HashIndexPtr( ptr );
		cHashedItem *pItem = m_pHashedItems[ index ];
		while( pItem )
		{
			if ( pItem->m_ptrIndex == ptr ) return pItem->m_pItem;
			pItem = pItem->m_pNextItem;
		}

		return 0;
	}

	template<class T> T cHashedList<T>::RemoveItem( const void *ptr )
	{
		uint32_t index = HashIndexPtr( ptr );
		cHashedItem *pItem = m_pHashedItems[ index ];
		cHashedItem *pLast = 0;
		T pItemContent = 0;
	
		while( pItem )
		{
			// check if this item matches target
			if ( pItem->m_ptrIndex == ptr )
			{
				// if this item is the current iterator then re-calculate the next step in advance
				if ( pItem == m_pIter ) GetNext();
			
				// remove the item from the list
				if ( pLast ) pLast->m_pNextItem = pItem->m_pNextItem;
				else m_pHashedItems[ index ] = pItem->m_pNextItem;

				pItemContent = pItem->m_pItem;
				m_iItemCount--;

				// delete item, but not the contents as it is undefined, pass it back to be deleted.
				delete pItem;
				return pItemContent;
			}

			// next item
			pLast = pItem;
			pItem = pItem->m_pNextItem;
		}

		return 0;
	}
}

#endif
