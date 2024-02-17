#ifndef _H_DYN_ARRAY
#define _H_DYN_ARRAY

namespace AGK_Compiler
{
	template<class T> class DynArray
	{
		protected: 
			

		public:
			T *m_pData;
			int m_iSize;
			int m_iCount;
			int m_iSorted;
			int (*CompareItems)(const void*, const void*);

			DynArray( int size=0 )
			{
				CompareItems = 0;
				m_pData = 0;
				m_iSize = 0;
				m_iCount = 0;
				m_iSorted = 0;
				if ( size > 0 )
				{
					int newSize = 1;
					while ( newSize < size ) newSize <<= 1;
					m_pData = new T[ newSize ];
					m_iSize = newSize;
				}
			}

			~DynArray()
			{
				if ( m_pData ) 
				{
					delete [] m_pData;
				}
			}

			void Clear()
			{
				m_iCount = 0;
			}

			void SetSorted( bool flag, int (*Compare)(const void*, const void*) )
			{
				CompareItems = Compare;
				if ( Compare == 0 ) flag = false;
				m_iSorted = flag ? 1 : 0;

				if ( m_iSorted && m_iCount > 0 )
				{
					// sort existing array
					qsort( m_pData, m_iCount, sizeof(T), Compare );
				}
			}

			int Add( T &item, bool duplicates=true )
			{
				if ( m_iCount+1 > m_iSize )
				{
					m_iSize *= 2;
					if ( m_iSize == 0 ) m_iSize = 32;
					T* newArray = new T[m_iSize];
					for ( int i = 0; i < m_iCount; i++ )
					{
						newArray[ i ] = m_pData[ i ];
					}
					delete [] m_pData;
					m_pData = newArray;
				}

				if ( m_iSorted == 0 )
				{
					m_pData[ m_iCount++ ] = item;
					return m_iCount-1;
				}
				else
				{
					int high = m_iCount-1;
					int low = 0;
					
					// binary search
					int result = 0;
					int mid = 0;
					while( high >= low )
					{
						mid = (high+low)/2;
						result = CompareItems( &(m_pData[ mid ]), &item );
						if( result > 0 ) high = mid-1;
						else if ( result < 0 ) low = mid+1;
						else 
						{
							if ( duplicates ) break;
							else return -1; // not allowed dulplicates
						}
					}

					if ( result < 0 ) mid++;
					if ( mid >= m_iCount )
					{
						m_pData[ m_iCount++ ] = item;
						return m_iCount-1;
					}
					else
					{
						// insert new element at mid index
						for( int i = m_iCount-1; i >= mid; i-- )
						{
							m_pData[ i+1 ] = m_pData[ i ];
						}
						m_pData[ mid ] = item;
						return mid;
					}
				}
			}

			int Find( T &item )
			{
				if ( !m_iSorted ) return -1;

				int high = m_iCount-1;
				int low = 0;
				
				// binary search
				int result = 0;
				while( high >= low )
				{
					int mid = (high+low)/2;
					result = CompareItems( &(m_pData[ mid ]), &item );
					if( result > 0 ) high = mid-1;
					else if ( result < 0 ) low = mid+1;
					else return mid;
				}

				return -1;
			}

			void Remove( int index )
			{
				if ( index < 0 || index >= m_iCount ) return;

				for( int i = index; i < m_iCount-1; i++ )
				{
					m_pData[ i ] = m_pData[ i+1 ];
				}

				m_iCount--;
			}

			T GetItem( int index )
			{
				if ( index < 0 || index >= m_iCount ) return 0;
				return m_pData[ index ];
			}
	};


	template<class T> class DynArrayObject
	{
		public:
			T **m_pData;
			int m_iSize;
			int m_iCount;
			int m_iSorted;
			int (*CompareItems)(const void*, const void*);

			DynArrayObject( int size=0 )
			{
				CompareItems = 0;
				m_pData = 0;
				m_iSize = 0;
				m_iCount = 0;
				m_iSorted = 0;
				if ( size > 0 )
				{
					int newSize = 1;
					while ( newSize < size ) newSize <<= 1;
					m_pData = new T*[ newSize ];
					m_iSize = newSize;
				}
			}

			~DynArrayObject()
			{
				if ( m_pData ) 
				{
					for( int i = 0; i < m_iCount; i++ )
					{
						delete m_pData[ i ];
					}
					
					delete [] m_pData;
				}
			}

			void Clear()
			{
				if ( m_pData ) 
				{
					for( int i = 0; i < m_iCount; i++ )
					{
						delete m_pData[ i ];
					}
					
					m_pData = 0;
				}
				m_iSize = 0;
				m_iCount = 0;
			}

			void SetSorted( bool flag, int (*Compare)(const void*, const void*) )
			{
				CompareItems = Compare;
				if ( Compare == 0 ) flag = false;
				m_iSorted = flag ? 1 : 0;

				if ( m_iSorted && m_iCount > 0 )
				{
					// sort existing array
					qsort( m_pData, m_iCount, sizeof(T*), Compare );
				}
			}

			int Add( T *item, bool duplicates=true, bool copy=true )
			{
				if ( m_iCount+1 > m_iSize )
				{
					m_iSize *= 2;
					if ( m_iSize == 0 ) m_iSize = 4;
					T** newArray = new T*[m_iSize];
					for ( int i = 0; i < m_iCount; i++ )
					{
						newArray[ i ] = m_pData[ i ];
					}
					if ( m_pData ) delete [] m_pData;
					m_pData = newArray;
				}

				T *storeItem = item;
				if ( copy )
				{
					storeItem = new T();
					*storeItem = *item;
				}

				if ( m_iSorted == 0 )
				{
					m_pData[ m_iCount++ ] = storeItem;
					return m_iCount-1;
				}
				else
				{
					int high = m_iCount-1;
					int low = 0;
					
					// binary search
					int result = 0;
					int mid = 0;
					while( high >= low )
					{
						mid = (high+low)/2;
						result = CompareItems( &(m_pData[ mid ]), &storeItem );
						if( result > 0 ) high = mid-1;
						else if ( result < 0 ) low = mid+1;
						else 
						{
							if ( duplicates ) break;
							else return -1; // not allowed dulplicates
						}
					}

					if ( result < 0 ) mid++;
					if ( mid >= m_iCount )
					{
						m_pData[ m_iCount++ ] = storeItem;
						return m_iCount-1;
					}
					else
					{
						// insert new element at mid index
						for( int i = m_iCount-1; i >= mid; i-- )
						{
							m_pData[ i+1 ] = m_pData[ i ];
						}
						m_pData[ mid ] = storeItem;
						m_iCount++;
						return mid;
					}
				}
			}

			int Find( T *item )
			{
				if ( !m_iSorted ) return -1;

				int high = m_iCount-1;
				int low = 0;
				
				// binary search
				int result = 0;
				while( high >= low )
				{
					int mid = (high+low)/2;
					result = CompareItems( &(m_pData[ mid ]), &item );
					if( result > 0 ) high = mid-1;
					else if ( result < 0 ) low = mid+1;
					else return mid;
				}

				return -1;
			}

			void Remove( int index )
			{
				if ( index < 0 || index >= m_iCount ) return;

				delete m_pData[ index ];

				for( int i = index; i < m_iCount-1; i++ )
				{
					m_pData[ i ] = m_pData[ i+1 ];
				}

				m_iCount--;
			}

			DynArrayObject<T>& operator=( DynArrayObject<T> &other )
			{
				if ( other.m_iCount > m_iSize )
				{
					m_iSize *= 2;
					if ( m_iSize == 0 ) m_iSize = 32;
					T** newArray = new T*[m_iSize];
					for ( int i = 0; i < m_iCount; i++ )
					{
						newArray[ i ] = m_pData[ i ];
					}
					delete [] m_pData;
					m_pData = newArray;
				}

				m_iCount = other.m_iCount;
				m_iSorted = other.m_iSorted;
				CompareItems = other.CompareItems;

				for ( int i = 0; i < other.m_iCount; i++ )
				{
					*(m_pData[ i ]) = *(other.m_pData[ i ]);
				}
				
				return *this;
			}

			T* GetItem( int index )
			{
				if ( index < 0 || index >= m_iCount ) return 0;
				return m_pData[ index ];
			}
	};
}

#endif