#ifndef _H_AGK_ARRAYS
#define _H_AGK_ARRAYS

#include "Common.h"

template<class T> class AGKUnorderedArray
{
	public:
		AGKUnorderedArray( uint32_t size=2 ) { m_iArraySize = size; if ( m_iArraySize ) m_pItems = new T[ m_iArraySize ]; }
		~AGKUnorderedArray() { if ( m_pItems ) delete [] m_pItems; }

		void Clear() { m_iNumItems = 0; }

		void AddItem( T item )
		{
			if ( m_iArraySize < m_iNumItems+1 )
			{
				if ( m_iArraySize <= 1 ) m_iArraySize = 2;
				else m_iArraySize = m_iArraySize + m_iArraySize/2;
				T* newArray = new T[ m_iArraySize ];
				if ( m_pItems )
				{
					memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
					delete [] m_pItems;
				}
				m_pItems = newArray;
			}

			m_pItems[ m_iNumItems ] = item;
			m_iNumItems++;
		}

		void RemoveIndex( uint32_t index )
		{
			if ( m_iNumItems == 0 ) return;
			m_iNumItems--;
			if ( m_iNumItems > 0 ) m_pItems[ index ] = m_pItems[ m_iNumItems ];
		}

		uint32_t NumItems() { return m_iNumItems; }
		T& operator[](uint32_t i) { return m_pItems[ i ]; }
		T GetItem(uint32_t i) { return m_pItems[ i ]; }

	protected:
		T* m_pItems = 0;
		uint32_t m_iNumItems = 0;
		uint32_t m_iArraySize = 0;
};

// ordered does not mean sorted, items maintain the order they were added
template<class T> class AGKOrderedArray
{
	public:
		AGKOrderedArray( uint32_t size=2 ) { m_iArraySize = size; if ( m_iArraySize ) m_pItems = new T[ m_iArraySize ]; }
		~AGKOrderedArray() { if ( m_pItems ) delete [] m_pItems; }

		void Clear() { m_iNumItems = 0; }

		void AddItem( T item )
		{
			if ( m_iArraySize < m_iNumItems+1 )
			{
				if ( m_iArraySize <= 1 ) m_iArraySize = 2;
				else m_iArraySize = m_iArraySize + m_iArraySize/2;
				T* newArray = new T[ m_iArraySize ];
				if ( m_pItems )
				{
					memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
					delete [] m_pItems;
				}
				m_pItems = newArray;
			}

			m_pItems[ m_iNumItems ] = item;
			m_iNumItems++;
		}

		void RemoveIndex( uint32_t index )
		{
			if ( m_iNumItems == 0 ) return;
			m_iNumItems--;
			for( uint32_t i = index; i < m_iNumItems; i++ ) m_pItems[ i ] = m_pItems[ i+1 ];
		}

		uint32_t NumItems() { return m_iNumItems; }
		T& operator[](uint32_t i) { return m_pItems[ i ]; }
		T GetItem(uint32_t i) { return m_pItems[ i ]; }

	protected:
		T* m_pItems = 0;
		uint32_t m_iNumItems = 0;
		uint32_t m_iArraySize = 0;
};

// will keep track of the highest element, elements equal to 0 count as empty
// used mainly for image texture stages
template<class T> class AGKDynamicArray
{
	protected:
		T *m_pItems = 0;
		int32_t m_iHighestIndex = -1;
		uint32_t m_iArraySize = 0;
			
	public:						
		AGKDynamicArray( uint32_t size=0 ) 
		{ 
			m_iArraySize = size; 
			if ( m_iArraySize > 0 )
			{
				m_pItems = new T[ m_iArraySize ];
				memset( m_pItems, 0, sizeof(T) * m_iArraySize );
			}
		}

		~AGKDynamicArray() 
		{ 
			if ( m_pItems ) delete [] m_pItems; 
		}

		void Clone( const AGKDynamicArray* pOther )
		{
			m_iHighestIndex = pOther->m_iHighestIndex;
			m_iArraySize = pOther->m_iArraySize;

			if ( pOther->m_pItems ) 
			{
				m_pItems = new T[ m_iArraySize ];
				memcpy( m_pItems, pOther->m_pItems, sizeof(T) * m_iArraySize );
			}
		}

		int32_t GetHighestIndex() const { return m_iHighestIndex; }
			
		T GetItem( uint32_t index ) const
		{
			if ( index >= m_iArraySize ) return 0;
			return m_pItems[ index ];
		}
		
		void Clear() 
		{ 
			if ( m_pItems ) memset( m_pItems, 0, sizeof(T) * m_iArraySize );
			m_iHighestIndex = -1;
		}

		void Expand( uint32_t newSize )
		{
			if ( newSize <= m_iArraySize ) return;

			T *newArray = new T[ newSize ];
			if ( m_pItems )
			{
				memcpy( newArray, m_pItems, m_iArraySize * sizeof(T) );
				for( uint32_t i = m_iArraySize; i < newSize; i++ ) newArray[ i ] = 0;
				delete [] m_pItems;
			}
			else
			{
				memset( newArray, 0, sizeof(T) * newSize );
			}
			m_pItems = newArray;
			m_iArraySize = newSize;
		}
			
		// setting the item to 0 will remove it
		void SetItem( uint32_t index, T item ) 
		{
			if ( item != 0 )
			{
				// add
				if ( index >= m_iArraySize ) Expand( index+1 );
				
				m_pItems[ index ] = item;
				if ( (int)index > m_iHighestIndex ) m_iHighestIndex = (int)index;
			}
			else
			{
				if ( index >= m_iArraySize ) return;
					
				m_pItems[ index ] = 0;
					
				if ( m_iHighestIndex == index )
				{
					while( m_iHighestIndex >= 0 && m_pItems[ m_iHighestIndex ] ) m_iHighestIndex--;
				}
			}
		}
};

// items will be added and removed like a stack
template<class T> class AGKStackArray
{
	public:
		AGKStackArray( uint32_t size=2 ) { m_iMaxSize = size; if ( m_iMaxSize ) m_pItems = new T[ m_iMaxSize ]; }
		~AGKStackArray() { if ( m_pItems ) delete [] m_pItems; }

		void Resize( uint32_t size ) 
		{ 
			if ( size < m_iNumItems ) size = m_iNumItems;
			if ( size < 2 ) size = 2;
			if ( m_iMaxSize != size )
			{
				m_iMaxSize = size;
				T* newArray = new T[ m_iMaxSize ];
				if ( m_pItems )
				{
					memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
					delete [] m_pItems;
				}
				m_pItems = newArray;
			}
		}

		void Clear() { m_iNumItems = 0; }

		void PushItem( T item )
		{
			if ( m_iMaxSize < m_iNumItems+1 )
			{
				if ( m_iMaxSize <= 1 ) m_iMaxSize = 2;
				else m_iMaxSize = m_iMaxSize + m_iMaxSize/2;
				T* newArray = new T[ m_iMaxSize ];
				if ( m_pItems )
				{
					memcpy( newArray, m_pItems, sizeof(T)*m_iNumItems );
					delete [] m_pItems;
				}
				m_pItems = newArray;
			}

			m_pItems[ m_iNumItems ] = item;
			m_iNumItems++;
		}

		T PopItem()
		{
			if ( m_iNumItems == 0 ) return 0;
			m_iNumItems--;
			return m_pItems[ m_iNumItems ];
		}

		uint32_t NumItems() { return m_iNumItems; }
		T& operator[](uint32_t i) { return m_pItems[ i ]; }
		T GetItem(uint32_t i) { return m_pItems[ i ]; }
		
	protected:
		T* m_pItems = 0;
		uint32_t m_iNumItems = 0;
		uint32_t m_iMaxSize = 0;
};

#endif // _H_AGK_ARRAYS