#include "agk.h"

using namespace AGK;

const char* JSONElement::TypeToString( int type )
{
	switch( type )
	{
		case 0: return "Undefined";
		case AGK_JSON_OBJECT: return "Object";
		case AGK_JSON_ARRAY: return "Array";
		case AGK_JSON_STRING: return "String";
		case AGK_JSON_NUMBER: return "Number";
		case AGK_JSON_BOOL: return "Bool";
		default: return "Unknown";
	}
}

JSONElement* JSONElement::LoadJSONFromFile( const char* filename )
{
	cFile oFile;
	if ( !oFile.OpenToRead( filename ) ) return 0;

	uint32_t size = oFile.GetSize();
	char* data = new char[ size+1 ];
	oFile.ReadData( data, size );
	oFile.Close();

	data[ size ] = 0;
	JSONElement* element = LoadJSONFromData( data );
	delete [] data;
	return element;
}

JSONElement* JSONElement::LoadJSONFromData( const char* data )
{
	uint32_t index = 0;

	while( 1 )
	{
		switch( data[ index ] )
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n': break;
			
			case '{': 
			{
				JSONObject *pObject = new JSONObject();
				index++;
				if ( pObject->ParseObject( data+index ) < 0 ) 
				{
					delete pObject;
					return 0;
				}
				return pObject;
			}
			
			case '[':
			{
				JSONArray *pArray = new JSONArray();
				index++;
				if ( pArray->ParseArray( data+index ) < 0 ) 
				{
					delete pArray;
					return 0;
				}
				return pArray;
			}
			
			default:
			{
				agk::Error( "Failed to parse JSON file, must begin with an object or array" );
				return 0;
			}
		}
		
		index++;
	}

	return 0;
}

int JSONObject::ParseObject( const char* data )
{
	int index = 0;

	JSONKeyPair *pPairList = 0;
	JSONKeyPair *pLastPair = 0;
	int count = 0;

	while ( 1 )
	{
		switch( data[ index ] )
		{
			case 0: 
			{
				while ( pPairList ) { pLastPair = pPairList; pPairList = pPairList->m_pTempNext; delete pLastPair; }
				agk::Error("Invalid JSON, unexpected end of object"); 
				return -1;
			}

			case ' ':
			case '\t':
			case '\r':
			case '\n': index++; break;
			
			case '}': return index+1;
			
			case '"':
			{
				JSONKeyPair *pNewPair = new JSONKeyPair();
				pNewPair->m_sName.SetStr("");
				index++;
				int length = pNewPair->ParsePair( data+index );
				if ( length < 0 ) 
				{
					while ( pPairList ) { pLastPair = pPairList; pPairList = pPairList->m_pTempNext; delete pLastPair; }
					delete pNewPair;
					return -1;
				}

				if ( pLastPair ) pLastPair->m_pTempNext = pNewPair;
				else pPairList = pNewPair;
				pLastPair = pNewPair;
				count++;

				index += length;
				if ( data[index] == '}' ) 
				{
					m_iNumPairs = count;
					m_pPairs = new JSONKeyPair*[ count ];
					for( int i = 0; i < count; i++ )
					{
						if ( pPairList )
						{
							m_pPairs[ i ] = pPairList;
							pPairList = pPairList->m_pTempNext;
							m_pPairs[ i ]->m_pTempNext = 0;
						}
						else m_pPairs[ i ] = 0;
					}
					return index+1;
				}

				break;
			}

			default:
			{
				agk::Error("Invalid JSON, unexpected character in object"); 
				return -1;
			}
		}
	}
}

int JSONArray::ParseArray( const char* data )
{
	int index = 0;

	JSONElement *pElementList = 0;
	JSONElement *pLastElement = 0;
	int count = 0;

	while ( 1 )
	{
		switch( data[ index ] )
		{
			case 0: 
			{
				while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
				agk::Error("Invalid JSON, unexpected end of array"); 
				return -1;
			}

			case ' ':
			case '\t':
			case '\r':
			case '\n': index++; break;

			case ']': 
			{
				m_iNumElements = count;
				m_pElements = new JSONElement*[ count ];
				for( int i = 0; i < count; i++ )
				{
					if ( pElementList )
					{
						m_pElements[ i ] = pElementList;
						pElementList = pElementList->m_pTempNext;
						m_pElements[ i ]->m_pTempNext = 0;
					}
					else m_pElements[ i ] = 0;
				}
				return index+1;
			}

			case '[':
			{
				JSONArray *pArray = new JSONArray();
				index++;
				int length = pArray->ParseArray( data+index );
				
				if ( length < 0 ) 
				{
					delete pArray;
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}

				index += length;
				
				if ( pLastElement ) pLastElement->m_pTempNext = pArray;
				else pElementList = pArray;
				pLastElement = pArray;
				count++;
				
				length = FindArrayEnd( data + index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				index += length;
				break;
			}

			case '{': 
			{
				JSONObject *pObject = new JSONObject();
				index++;
				int length = pObject->ParseObject( data+index );
				if ( length < 0 ) 
				{
					delete pObject;
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}

				index += length;
				
				if ( pLastElement ) pLastElement->m_pTempNext = pObject;
				else pElementList = pObject;
				pLastElement = pObject;
				count++;
				
				length = FindArrayEnd( data + index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				index += length;
				break;
			}

			case '"': 
			{
				index++;
				int length = JSONElement::ParseString( data+index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				JSONString *pString = new JSONString();
				if ( length > 0 ) pString->m_sValue.AppendN( data+index, length );
				pString->m_sValue.Unescape();
				
				index += (length+1);
								
				if ( pLastElement ) pLastElement->m_pTempNext = pString;
				else pElementList = pString;
				pLastElement = pString;
				count++;
				
				length = FindArrayEnd( data + index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				index += length;
				break;
			}

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			{
				int length = JSONElement::ParseNumber( data+index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				uString sNum;
				if ( length > 0 ) sNum.AppendN( data+index, length );
				JSONNumber *pNumber = new JSONNumber();
				if ( sNum.Find('.') < 0 && sNum.Find('e') < 0 && sNum.Find('E') < 0 ) 
				{
					pNumber->m_iIsInt = 1;
					pNumber->m_iValue = atoi( sNum.GetStr() );
				}
				pNumber->m_fValue = (float) atof( sNum.GetStr() );
				index += length;
				
				if ( pLastElement ) pLastElement->m_pTempNext = pNumber;
				else pElementList = pNumber;
				pLastElement = pNumber;
				count++;
				
				length = FindArrayEnd( data + index );
				if ( length < 0 ) 
				{
					while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
					return -1;
				}
				index += length;
				break;
			}

			case 'n':
			{
				if ( data[ index+1 ]  == 'u' 
				  && data[ index+2 ]  == 'l' 
				  && data[ index+3 ]  == 'l' )
				{
					JSONNull *pNull = new JSONNull();

					if ( pLastElement ) pLastElement->m_pTempNext = pNull;
					else pElementList = pNull;
					pLastElement = pNull;
					count++;

					int length = FindArrayEnd( data + index );
					if ( length < 0 ) 
					{
						while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
						return -1;
					}
					index += length;
					break;
				}
				else
				{
					agk::Error("Invalid JSON, unexpected character in array element");
					return -1;
				}
			}

			case 't':
			{
				if ( data[ index+1 ]  == 'r' 
				  && data[ index+2 ]  == 'u' 
				  && data[ index+3 ]  == 'e' )
				{
					JSONBool *pBool = new JSONBool();
					pBool->m_bValue = true;
					
					if ( pLastElement ) pLastElement->m_pTempNext = pBool;
					else pElementList = pBool;
					pLastElement = pBool;
					count++;
					
					int length = FindArrayEnd( data + index );
					if ( length < 0 ) 
					{
						while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
						return -1;
					}
					index += length;
					break;
				}
				else
				{
					agk::Error("Invalid JSON, unexpected character in array element");
					return -1;
				}
			}

			case 'f':
			{
				if ( data[ index+1 ]  == 'a' 
				  && data[ index+2 ]  == 'l' 
				  && data[ index+3 ]  == 's'
				  && data[ index+4 ]  == 'e' )
				{
					JSONBool *pBool = new JSONBool();
					pBool->m_bValue = false;
					
					if ( pLastElement ) pLastElement->m_pTempNext = pBool;
					else pElementList = pBool;
					pLastElement = pBool;
					count++;
					
					int length = FindArrayEnd( data + index );
					if ( length < 0 ) 
					{
						while ( pElementList ) { pLastElement = pElementList; pElementList = pElementList->m_pTempNext; delete pLastElement; }
						return -1;
					}
					index += length;
					break;
				}
				else
				{
					agk::Error("Invalid JSON, unexpected character in array element");
					return -1;
				}
			}

			default:
			{
				agk::Error("Invalid JSON, unexpected character in array element");
				return -1;
			}
		}
	}
}

int JSONArray::FindArrayEnd( const char* data )
{
	int index = 0;
	while ( data[index] && data[index] != ',' && data[index] != ']' ) index++;
	if ( !data[index] ) 
	{
		agk::Error("Invalid JSON, unexpected end of array element");
		return -1;
	}

	if ( data[index] == ',' ) index++;
	return index;
}

int JSONKeyPair::FindPairEnd( const char* data )
{
	int index = 0;
	while ( data[index] && data[index] != ',' && data[index] != '}' ) index++;
	if ( !data[index] ) 
	{
		agk::Error("Invalid JSON, unexpected end of object key pair");
		return -1;
	}

	if ( data[index] == ',' ) index++;
	return index;
}

int JSONKeyPair::ParsePair( const char *data )
{
	int length = JSONElement::ParseString( data );
	if ( length < 0 ) return -1;
	if ( length > 0 ) m_sName.AppendN( data, length );
	m_sName.Unescape();
		
	int index = length+1;

	while ( data[index] && data[index] != ':' ) index++;
	if ( !data[index] ) 
	{
		agk::Error("Invalid JSON, unexpected end of object key pair");
		return -1;
	}

	index++;
	
	while( 1 )
	{
		switch( data[ index ] )
		{
			case 0: agk::Error("Invalid JSON, unexpected end of object key pair"); return -1;

			case ' ':
			case '\t':
			case '\r':
			case '\n': index++; break;

			case '[':
			{
				JSONArray *pArray = new JSONArray();
				index++;
				int length = pArray->ParseArray( data+index );
				
				if ( length < 0 ) 
				{
					delete pArray;
					return -1;
				}

				index += length;
				m_pElement = pArray;
				
				length = FindPairEnd( data + index );
				if ( length < 0 ) return -1;
				return index+length;
			}

			case '{': 
			{
				JSONObject *pObject = new JSONObject();
				index++;
				int length = pObject->ParseObject( data+index );
				if ( length < 0 ) 
				{
					delete pObject;
					return -1;
				}

				index += length;
				m_pElement = pObject;
				
				length = FindPairEnd( data + index );
				if ( length < 0 ) return -1;
				return index+length;
			}

			case '"': 
			{
				index++;
				int length = JSONElement::ParseString( data+index );
				if ( length < 0 ) return -1;
				JSONString *pString = new JSONString();
				if ( length > 0 ) pString->m_sValue.AppendN( data+index, length );
				pString->m_sValue.Unescape();
				
				index += (length+1);
				m_pElement = pString;
				
				length = FindPairEnd( data + index );
				if ( length < 0 ) return -1;
				return index+length;
			}

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			{
				int length = JSONElement::ParseNumber( data+index );
				if ( length < 0 ) return -1;
				uString sNum;
				if ( length > 0 ) sNum.AppendN( data+index, length );
				JSONNumber *pNumber = new JSONNumber();
				if ( sNum.Find('.') < 0 && sNum.Find('e') < 0 && sNum.Find('E') < 0 ) 
				{
					pNumber->m_iIsInt = 1;
					pNumber->m_iValue = atoi( sNum.GetStr() );
				}
				pNumber->m_fValue = (float) atof( sNum.GetStr() );
				index += length;
				m_pElement = pNumber;
								
				length = FindPairEnd( data + index );
				if ( length < 0 ) return -1;
				return index+length;
			}

			case 'n':
			{
				if ( data[ index+1 ]  == 'u' 
				  && data[ index+2 ]  == 'l' 
				  && data[ index+3 ]  == 'l' )
				{
					JSONNull *pNull = new JSONNull();
					m_pElement = pNull;

					length = FindPairEnd( data + index );
					if ( length < 0 ) return -1;
					return index+length;
				}
				else
				{
					agk::Error("Invalid JSON, unexpected character in object key pair");
					return -1;
				}
			}

			case 't':
			{
				if ( data[ index+1 ]  == 'r' 
				  && data[ index+2 ]  == 'u' 
				  && data[ index+3 ]  == 'e' )
				{
					JSONBool *pBool = new JSONBool();
					pBool->m_bValue = true;
					m_pElement = pBool;

					length = FindPairEnd( data + index );
					if ( length < 0 ) return -1;
					return index+length;
				}
				else
				{
					agk::Error("Invalid JSON, unexpected character in object key pair");
					return -1;
				}
			}
			case 'f':
			{
				if ( data[ index+1 ]  == 'a' 
				  && data[ index+2 ]  == 'l' 
				  && data[ index+3 ]  == 's'
				  && data[ index+4 ]  == 'e' )
				{
					JSONBool *pBool = new JSONBool();
					pBool->m_bValue = false;
					m_pElement = pBool;

					length = FindPairEnd( data + index );
					if ( length < 0 ) return -1;
					return index+length;
				}
				else
				{
					agk::Error("Invalid JSON, unexpected character in object key pair");
					return -1;
				}
			}
			default:
			{
				agk::Error("Invalid JSON, unexpected character in object key pair");
				return -1;
			}
		}
	}
}

int JSONElement::ParseString( const char* data )
{
	int index = 0;

	while ( 1 )
	{
		switch( data[ index ] )
		{
			case 0: agk::Error("Invalid JSON, unexpected end of string"); return -1;
			case '"': return index;
			case '\\': 
			{
				// skip escaped character
				index++; 
				if ( data[ index ] == 0 ) 
				{
					agk::Error("Invalid JSON, unexpected end of string"); 
					return -1;
				}
				break;
			}
		}

		index++;
	}
}

int JSONElement::ParseNumber( const char* data )
{
	int index = 0;

	while ( 1 )
	{
		switch( data[ index ] )
		{
			case 0: agk::Error("Invalid JSON, unexpected end of string"); return -1;
			case ',': return index;
			case '}': return index;
			case ']': return index;
			case '\n': return index;
			case '\r': return index;
			case ' ':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			case '+':
			case '.':
			case 'e':
			case 'E': break;
			default: agk::Error("Invalid JSON, unexpected character in number"); return -1;
		}

		index++;
	}
}

JSONElement* JSONObject::GetElement( const char *szKey )
{
	if ( !m_pPairs ) return 0;

	for( uint32_t i = 0; i < m_iNumPairs; i++ )
	{
		if ( m_pPairs[ i ]->m_sName.CompareCaseTo( szKey ) == 0 ) return m_pPairs[ i ]->m_pElement;
	}

	return 0;
}

JSONElement* JSONArray::GetElement( int index )
{
	if ( !m_pElements ) return 0;
	if ( index < 0 || index >= (int) m_iNumElements ) return 0;
	
	return m_pElements[ index ];
}