#include "agk.h"

int AGO::ParseFindChunk( const char *szFilename, char* data, int index, char* chunkName, uint32_t maxLength )
{
	if ( !chunkName || maxLength == 0 ) return -1;

	uint32_t iLength = 0;
	chunkName[ 0 ] = 0;
	while( data[ index ] )
	{
		if ( data[ index ] == '#' )
		{
			// skip rest of line
			while ( data[ index ] && data[ index ] != '\n' )
			{
				index++;
			}
		}
		else
		{
			if ( strchr(" \t\n\r", data[ index ]) == 0 )
			{
				if ( data[ index ] == '{' )
				{
					chunkName[ iLength ] = 0;
					return index+1;
				}
				else if ( data[ index ] == '}' )
				{
					if ( iLength == 0 )
					{
						// end of previous chunk
						chunkName[ 0 ] = '}';
						chunkName[ 1 ] = 0;
						return index+1;
					}
					else
					{
						chunkName[ iLength ] = 0;
						uString info;
						info.Format( "Failed to load object \"%s\", file is incorrectly formatted, chunk name beginning \"%s\" contains an unexpected close bracket }", szFilename, chunkName );
						agk::Error( info );
						return -1;
					}
				}
				else
				{
					// read chunk name 1 char at a time
					chunkName[ iLength++ ] = data[ index ];
					if ( iLength >= maxLength-1 )
					{
						chunkName[ maxLength-1 ] = 0;
						uString info;
						info.Format( "Failed to load object \"%s\", file is incorrectly formatted, chunk name beginning \"%s\" is too long (max %d)", szFilename, chunkName, maxLength-1 );
						agk::Error( info );
						return -1;
					}
				}
			}
		}

		index++;
	}

	if ( iLength > 0 )
	{
		uString info;
		info.Format( "Failed to load object \"%s\", unexpected end of file, perhaps a missing bracket?", szFilename, chunkName, maxLength-1 );
		agk::Error( info );
		return -1;
	}

	return index;
}

int AGO::ParseChunkUnknown( const char *szFilename, char* data, int index )
{
	// skip chunk
	int scopeCount = 1;
	int bQuoteMode = 0;
	while ( scopeCount > 0 && data[ ++index ] )
	{
		if ( data[ index ] == '"' ) bQuoteMode = 1-bQuoteMode;
		if ( !bQuoteMode )
		{
			if ( data[ index ] == '{' ) scopeCount++;
			else if ( data[ index ] == '}' ) scopeCount--;
		}
	}

	if ( data[ index ] == 0 ) 
	{
		uString info;
		info.Format( "Failed to load object \"%s\", unexpected end of file reached, possibly missing a closing bracket }", szFilename );
		agk::Error( info );
		return -1;
	}

	return index+1;
}



int AGO::ParseChunkFloat( const char *szFilename, char* data, int index, float* f )
{
	uString sFloat( "", 20 );
	while ( data[ index ] && data[ index ] != '}' )
	{
		if ( data[ index ] == '#' )
		{
			// skip rest of line
			while ( data[ index ] && data[ index ] != '\n' )
			{
				index++;
			}
		}

		if ( strchr(" \t\n\r", data[ index ]) == 0 ) sFloat.AppendAscii( data[ index ] );
		index++;
	}

	if ( data[ index ] == 0 ) 
	{
		uString info;
		info.Format( "Failed to load object \"%s\", unexpected end of file reached, possibly missing a closing bracket }", szFilename );
		agk::Error( info );
		return -1;
	}

	*f = sFloat.ToFloat();

	return index+1;
}

int AGO::ParseChunkInt( const char *szFilename, char* data, int index, int* i )
{
	uString sInt( "", 20 );
	while ( data[ index ] && data[ index ] != '}' )
	{
		if ( data[ index ] == '#' )
		{
			// skip rest of line
			while ( data[ index ] && data[ index ] != '\n' )
			{
				index++;
			}
		}

		if ( strchr(" \t\n\r", data[ index ]) == 0 ) sInt.AppendAscii( data[ index ] );
		index++;
	}

	if ( data[ index ] == 0 ) 
	{
		uString info;
		info.Format( "Failed to load object \"%s\", unexpected end of file reached, possibly missing a closing bracket }", szFilename );
		agk::Error( info );
		return -1;
	}

	*i = sInt.ToInt();

	return index+1;
}

int AGO::ParseChunkString( const char *szFilename, char* data, int index, uString &out )
{
	out.ClearTemp();
	uString sTemp;
	bool bStarted = false;
	bool bFinished = false;
	while ( data[ index ] && data[ index ] != '}' )
	{
		if ( data[ index ] == '#' )
		{
			// skip rest of line
			while ( data[ index ] && data[ index ] != '\n' )
			{
				index++;
			}
		}

		if ( !bFinished )
		{
			if ( data[ index ] == '"' )
			{
				if ( bStarted ) bFinished = true;
				else bStarted = true;
			}
			else if ( strchr(" \t\n\r", data[ index ]) == 0 ) 
			{
				if ( bStarted ) sTemp.AppendByte( data[ index ] ); // string may be UTF-8 or Windows-1252
			}
		}

		index++;
	}

	if ( data[ index ] == 0 ) 
	{
		uString info;
		info.Format( "Failed to load object \"%s\", unexpected end of file reached, possibly missing a closing bracket }", szFilename );
		agk::Error( info );
		return -1;
	}

	// convert Windows-1252 to UTF-8 if necessary
	out.SetStr( sTemp.GetStr() );

	return index+1;
}

int AGO::ParseChunkFloatArray( const char *szFilename, const char *szChunkName, char* data, int index, float* f, uint32_t elements )
{
	uint32_t index2 = 0;
	uString sFloat( "", 20 );
	while ( data[ index ] && data[ index ] != '}' )
	{
		if ( data[ index ] == '#' )
		{
			// skip rest of line
			while ( data[ index ] && data[ index ] != '\n' )
			{
				index++;
			}
		}

		if ( data[ index ] == ',' )
		{
			if ( index2 >= elements )
			{
				uString info;
				info.Format( "Failed to load object \"%s\", file format error, too many float values in float array \"%s\"", szFilename, szChunkName );
				agk::Error( info );
				return -1;
			}

			f[ index2++ ] = sFloat.ToFloat();
			sFloat.ClearTemp();
		}
		else if ( strchr(" \t\n\r", data[ index ]) == 0 ) sFloat.AppendAscii( data[ index ] );
		index++;
	}

	if ( data[ index ] == 0 ) 
	{
		uString info;
		info.Format( "Failed to load object \"%s\", unexpected end of file reached, possibly missing a closing bracket }", szFilename );
		agk::Error( info );
		return -1;
	}

	f[ index2++ ] = sFloat.ToFloat();

	if ( index2 < elements )
	{
		uString info;
		info.Format( "Failed to load object \"%s\", file format error, not enough float values in float array \"%s\"", szFilename, szChunkName );
		agk::Error( info );
		return -1;
	}

	return index+1;
}

int AGO::ParseChunkIntArray( const char *szFilename, const char *szChunkName, char* data, int index, int* i, uint32_t elements )
{
	uint32_t index2 = 0;
	uString sInt( "", 20 );
	while ( data[ index ] && data[ index ] != '}' )
	{
		if ( data[ index ] == '#' )
		{
			// skip rest of line
			while ( data[ index ] && data[ index ] != '\n' )
			{
				index++;
			}
		}

		if ( data[ index ] == ',' )
		{
			if ( index2 >= elements )
			{
				uString info;
				info.Format( "Failed to load object \"%s\", file format error, too many integer values in integer array \"%s\"", szFilename, szChunkName );
				agk::Error( info );
				return -1;
			}

			i[ index2++ ] = sInt.ToInt();
			sInt.ClearTemp();
		}
		else if ( strchr(" \t\n\r", data[ index ]) == 0 ) sInt.AppendAscii( data[ index ] );
		index++;
	}

	if ( data[ index ] == 0 ) 
	{
		uString info;
		info.Format( "Failed to load object \"%s\", unexpected end of file reached, possibly missing a closing bracket }", szFilename );
		agk::Error( info );
		return -1;
	}

	i[ index2++ ] = sInt.ToInt();

	if ( index2 < elements )
	{
		uString info;
		info.Format( "Failed to load object \"%s\", file format error, not enough integer values in integer array \"%s\"", szFilename, szChunkName );
		agk::Error( info );
		return -1;
	}

	return index+1;
}

int AGO::ParseChunkObject( const char *szFilename, char* data, int index, cObject3D *pObject )
{
	char chunkName[ 31 ];
	float f[4];

	while(1)
	{
		index = ParseFindChunk( szFilename, data, index, chunkName, 31 );
		if ( index < 0 ) return -1;
		else
		{
			if ( chunkName[ 0 ] == '}' ) return index;
			else
			{
				// new chunk
				if ( strcmp( chunkName, "Position" ) == 0 )
				{
					index = ParseChunkFloatArray( szFilename, "Position", data, index, &(f[0]), 3 );
					if ( index < 0 ) return -1;
					pObject->SetNodePosition( f[0], f[1], f[2] );
				}
				else if ( strcmp( chunkName, "VertexAttrib" ) == 0 )
				{
					agk::Error("AGO format has changed to require a Mesh tag around vertex data");
					index = ParseChunkUnknown( szFilename, data, index );
					if ( index < 0 ) return -1;
				}
				else if ( strcmp( chunkName, "RotationQuat" ) == 0 )
				{
					index = ParseChunkFloatArray( szFilename, "RotationQuat", data, index, &(f[0]), 4 );
					if ( index < 0 ) return -1;
					pObject->SetNodeRotation( f[0], f[1], f[2], f[3] );
				}
				else if ( strcmp( chunkName, "Mesh" ) == 0 )
				{
					// parse mesh
					bool bSuccess = false;
					cMesh *pNewMesh = new cMesh(pObject);
					index = ParseChunkMesh( szFilename, data, index, pNewMesh );					
					if ( index < 0 ) return -1;

					pObject->m_iNumMeshes++;
					cMesh **pNewMeshes = new cMesh*[ pObject->m_iNumMeshes ];
					if ( pObject->m_pMeshes )
					{
						for ( uint32_t i = 0 ; i < pObject->m_iNumMeshes-1; i++ ) pNewMeshes[ i ] = pObject->m_pMeshes[ i ];
						delete [] pObject->m_pMeshes;
					}
					pObject->m_pMeshes = pNewMeshes;
					pObject->m_pMeshes[ pObject->m_iNumMeshes-1 ] = pNewMesh;
				}
				else
				{
					index = ParseChunkUnknown( szFilename, data, index );
					if ( index < 0 ) return -1;
				}
			}
		}
	}
}

int AGO::ParseChunkMesh( const char *szFilename, char* data, int index, cMesh *pMesh )
{
	char chunkName[ 31 ];

	while(1)
	{
		index = ParseFindChunk( szFilename, data, index, chunkName, 31 );
		if ( index < 0 ) return -1;
		else
		{
			if ( chunkName[ 0 ] == '}' ) break;
			else
			{
				// new chunk
				if ( strcmp( chunkName, "VertexCount" ) == 0 )
				{
					int i = 0;
					index = ParseChunkInt( szFilename, data, index, &i );
					if ( index < 0 ) return -1;
					pMesh->m_iNumRawVertices = i;
				}
				else if ( strcmp( chunkName, "IndexCount" ) == 0 )
				{
					int i = 0;
					index = ParseChunkInt( szFilename, data, index, &i );
					if ( index < 0 ) return -1;
					pMesh->m_iNumRawIndices = i;
				}
				/*
				else if ( strcmp( chunkName, "AttribCount" ) == 0 )
				{
					int i = 0;
					index = ParseChunkInt( szFilename, data, index, &i );
					if ( index < 0 ) return -1;
					pMesh->m_iNumAttribs = i;
					pMesh->m_pVertexAttribs = new cVertexAttrib*[ i ];
				}
				*/
				else if ( strcmp( chunkName, "VertexAttrib" ) == 0 )
				{
					// parse vertex attrib
					bool bSuccess = false;
					AGKVertexAttrib *pNewAttrib = new AGKVertexAttrib();
					index = ParseChunkVertexAttrib( szFilename, data, index, pMesh->m_iNumRawVertices, pNewAttrib, bSuccess );					
					if ( index < 0 ) return -1;
					if ( bSuccess )
					{
						pMesh->SetAttribNameIndex( pNewAttrib->m_szName, pMesh->m_iNumAttribs );

						AGKVertexAttrib *pNewAttribs = new AGKVertexAttrib[ pMesh->m_iNumAttribs+1 ];
						if ( pMesh->m_pVertexAttribs )
						{
							memcpy( pNewAttribs, pMesh->m_pVertexAttribs, pMesh->m_iNumAttribs );
							for ( int i = 0 ; i < pMesh->m_iNumAttribs; i++ ) 
							{
								// zero out the pointers so they don't get deleted
								pMesh->m_pVertexAttribs[ i ].m_pData = 0;
								pMesh->m_pVertexAttribs[ i ].m_szName = 0;
							}
							delete [] pMesh->m_pVertexAttribs;
						}
						pMesh->m_pVertexAttribs = pNewAttribs;
						pMesh->m_pVertexAttribs[ pMesh->m_iNumAttribs ] = *pNewAttrib;
						pMesh->m_iNumAttribs++;

						pNewAttrib->m_pData = 0;
						pNewAttrib->m_szName = 0;
						delete pNewAttrib;						
					}
				}
				else if ( strcmp( chunkName, "Indices" ) == 0 )
				{
					pMesh->m_pRawIndices = new uint32_t[ pMesh->m_iNumRawIndices ];
					index = ParseChunkIntArray( szFilename, "Indices", data, index, (int*)pMesh->m_pRawIndices, pMesh->m_iNumRawIndices );
					if ( index < 0 ) return -1;
				}
				else
				{
					index = ParseChunkUnknown( szFilename, data, index );
					if ( index < 0 ) return -1;
				}
			}
		}
	}

	pMesh->ProcessVertexData();

	return index;
}

int AGO::ParseChunkVertexAttrib( const char *szFilename, char* data, int index, int numVertices, AGKVertexAttrib *pAttrib, bool &bSuccess )
{
	bSuccess = true;
	char chunkName[ 31 ];

	while(1)
	{
		index = ParseFindChunk( szFilename, data, index, chunkName, 31 );
		if ( index < 0 ) return -1;
		else
		{
			if ( chunkName[ 0 ] == '}' ) return index;
			else
			{
				if ( !bSuccess )
				{
					index = ParseChunkUnknown( szFilename, data, index );
					if ( index < 0 ) return -1;
				}
				else
				{
					// new chunk
					if ( strcmp( chunkName, "Name" ) == 0 )
					{
						uString sName;
						index = ParseChunkString( szFilename, data, index, sName );
						pAttrib->m_szName = new char[ sName.GetLength() + 1 ];
						strcpy( pAttrib->m_szName, sName.GetStr() );
						if ( index < 0 ) return -1;
					}
					else if ( strcmp( chunkName, "Type" ) == 0 )
					{
						uString sType( "", 20 );
						index = ParseChunkString( szFilename, data, index, sType );
						if ( index < 0 ) return -1;
						sType.Lower();
						if ( sType.CompareTo( "float" ) == 0 ) pAttrib->m_iType = 0;
						else if ( sType.CompareTo( "ubyte" ) == 0 ) pAttrib->m_iType = 1;
						else 
						{
							// can't support this vertex attrib in this version of the loader
							bSuccess = false;
						}
					}
					else if ( strcmp( chunkName, "Components" ) == 0 )
					{
						int i = 0;
						index = ParseChunkInt( szFilename, data, index, &i );
						if ( index < 0 ) return -1;
						if ( i < 1 || i > 4 ) 
						{
							// can't support this vertex attrib in this version of the loader
							bSuccess = false;
						}
						else pAttrib->m_iComponents = i;
					}
					else if ( strcmp( chunkName, "Normalize" ) == 0 )
					{
						uString sType( "", 20 );
						index = ParseChunkString( szFilename, data, index, sType );
						if ( index < 0 ) return -1;
						sType.Lower();
						if ( sType.CompareTo( "yes" ) == 0 ) pAttrib->m_iNormalize = 1;
						else pAttrib->m_iNormalize = 0;
					}
					else if ( strcmp( chunkName, "Data" ) == 0 )
					{
						if ( pAttrib->m_iType == 0 )
						{
							float *pData = new float[ numVertices*pAttrib->m_iComponents ];
							pAttrib->m_pData = (void*) pData;
							index = ParseChunkFloatArray( szFilename, "VertexAttrib:Data", data, index, pData, numVertices*pAttrib->m_iComponents );
						}
						else if ( pAttrib->m_iType == 1 )
						{
							unsigned char *pData = new unsigned char[ numVertices*pAttrib->m_iComponents ];
							int *pDataI = new int[ numVertices*pAttrib->m_iComponents ];
							pAttrib->m_pData = (void*) pData;
							index = ParseChunkIntArray( szFilename, "VertexAttrib:Data", data, index, pDataI, numVertices*pAttrib->m_iComponents );
							for ( int i = 0; i < numVertices*pAttrib->m_iComponents; i++ )
							{
								pData[ i ] = pDataI[ i ];
							}
							delete [] pDataI;
						}
						else 
						{
							// can't support this vertex attrib in this version of the loader
							bSuccess = false;
						}
						if ( index < 0 ) return -1;
					}
					else
					{
						index = ParseChunkUnknown( szFilename, data, index );
						if ( index < 0 ) return -1;
					}
				}
			}
		}
	}
}