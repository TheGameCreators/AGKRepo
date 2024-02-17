//
// GLSL Parser
//

// Includes
#include "GLSLParser.h"
#include <vector>
#include "AppErrors.h"
#include <stdio.h>
#include <string.h>

inline int local_strincmp( const char *s1, const char *s2, unsigned int n )
{
	if( n == 0 ) return 0;

#if (defined _MSC_VER)
	return ::_strnicmp( s1, s2, n );
#elif defined( __GNUC__ )
	return ::strncasecmp( s1, s2, n );
#else
	register char c1, c2;
	unsigned int p = 0;
	do 
	{
		if( p++ >= n ) return 0;
		c1 = tolower( *s1++ );
		c2 = tolower( *s2++ );
	} 
	while ( c1 && (c1 == c2) );

	return c1 - c2;
#endif
}

bool glsl450_Convert ( const char* pVert, const char* pFrag, char** ppVert450, char** ppFrag450, const char* szVSFilename, const char* szPSFilename )
{
	// Group up uniforms together (except textures)
	bool bGroupUniformsTogether = true;

	// Location Indexes are per shader, Uniform Indexes are shared by vert and frag shaders
	bool bFoundVersionToken = false;
	int iAttributeLocationIndex = 0;
	int iVaryingLocationIndex = 0;
	int iBindingUniformIndex = 0;
	int iUniformNameAlphaIndex = 0;
	int iHasFragOutput = 0;

	const unsigned int maxVaryings = 32;
	char* m_szVaryingLocations[ maxVaryings ] = {0};

	// Break down into lines
	for ( int iVertFragPass = 0; iVertFragPass < 2; iVertFragPass++ )
	{
		const char* pShaderTextDataPtr = NULL;
		if ( iVertFragPass == 0 ) 
		{
			// Vert Shader
			pShaderTextDataPtr = pVert;
		}
		else
		{
			// Frag Shader
			pShaderTextDataPtr = pFrag;
		}
		if ( pShaderTextDataPtr )
		{
			// Break into lines
			std::vector <char*> lines;
			lines.clear();
			const char* pPtr = pShaderTextDataPtr;
			const char* pPtrEnd = pPtr + strlen(pShaderTextDataPtr);
			const char* pPtrLast = pPtr;
			while ( pPtr <= pPtrEnd )
			{
				// look for end of line
				bool bSkipAdvanceIfEndOfLine = false;
				if ( *pPtr == 13 || *pPtr == 10 || pPtr == pPtrEnd )
				{
					// put these characters into a line
					unsigned int iLineLength = (unsigned int)(pPtr - pPtrLast);
					char* pNewLine = new char[iLineLength+1];
					memcpy ( pNewLine, pPtrLast, iLineLength );
					pNewLine[iLineLength] = 0;
					lines.push_back ( pNewLine );

					// if not end of data
					if ( pPtr != pPtrEnd )
					{
						// get to start of next line
						while ( *pPtr == 13 || *pPtr == 10 ) pPtr++;
						
						// mark start of this new line
						bSkipAdvanceIfEndOfLine = true;
						pPtrLast = pPtr;
					}
				}

				// next character
				if ( bSkipAdvanceIfEndOfLine == false ) pPtr++;
			}

			// First pass - simple line based half-tokenizer
			std::vector <glsl450tokenstruct> tokensequence;
			tokensequence.clear();

			// convert all lines to a sequence of tokens, each line is broken into:
			// (%stuff:stringdata% or %token%) over and over, then a %tokenendofline%
			int iLineCount = (int)lines.size();
			for ( int iLine = 0; iLine < iLineCount; iLine++ )
			{
				// start of line
				bool bStartOfLine = true;
				bool bWasAUniformLine = false;
				char* pThisLine = lines[iLine];
				char* pPtr = pThisLine;

				// go through line characters
				char* pPtrEnd = pThisLine + strlen(pThisLine);
				char* pPtrLast = pPtr;
				while ( pPtr <= pPtrEnd )
				{
					// check for tokens we are interested in
					bool bSkipAdvance = false;
					if ( bStartOfLine == true )
					{
						// for items at very start of line
						char* pTryToken = "";
						char* pTokenFound = "";
						glsl450tokentypes tokenFound = glsl450none;
						// capturing space as a start line token stops other tokens being the start line token
						//pTryToken = " ";						if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450startspace; pTokenFound = pTryToken; }
						pTryToken = "#version ";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_version; pTokenFound = pTryToken; bFoundVersionToken = true; }
						pTryToken = "attribute ";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_attribute; pTokenFound = pTryToken; }
						pTryToken = "varying ";					if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_varying; pTokenFound = pTryToken; }
						pTryToken = "uniform ";					if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_uniform; pTokenFound = pTryToken; }
						pTryToken = "uniform sampler";			if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_uniformsampler; pTokenFound = pTryToken; }
						pTryToken = "uniform highp sampler";	if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_uniformsampler; pTokenFound = pTryToken; }
						pTryToken = "uniform mediump sampler";	if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_uniformsampler; pTokenFound = pTryToken; }
						pTryToken = "uniform lowp sampler";		if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_uniformsampler; pTokenFound = pTryToken; }
						pTryToken = "void main";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_fragmain; pTokenFound = pTryToken; }
						if ( tokenFound != glsl450none )
						{
							// frag shader toggles attributes and varying
							if ( iVertFragPass == 1 )
							{
								if ( tokenFound == glsl450token_attribute ) 
									tokenFound = glsl450token_varying;
								else
									if ( tokenFound == glsl450token_varying ) 
										tokenFound = glsl450token_attribute;
							}

							// add token
							glsl450tokenstruct addToken;
							addToken.eTokenType = tokenFound;
							addToken.pTokenData = NULL;
							tokensequence.push_back ( addToken );
							pPtr += strlen(pTokenFound);
							bSkipAdvance = true;
							pPtrLast = pPtr;
							bStartOfLine = false;

							// if a uniform line, also need cap before end of line
							if ( tokenFound == glsl450token_uniform ) bWasAUniformLine = true;
						}
						else
						{
							// allow tabs in front of start line tokens
							if ( *pPtr == 9 )
							{
								glsl450tokenstruct addToken;
								addToken.eTokenType = glsl450tab;
								addToken.pTokenData = new char[2];
								addToken.pTokenData[0] = 9;
								addToken.pTokenData[1] = 0;
								tokensequence.push_back ( addToken );
								pPtr += 1;
								bSkipAdvance = true;
								pPtrLast = pPtr;
							}
							else
							{
								// encountered a character that is NOT a start line token
								if ( *pPtr > 32 )
								{
									// so no longer start of line
									bSkipAdvance = true;
									bStartOfLine = false;
								}
							}
						}
					}
					else
					{
						// for everything else
						char* pTryToken = "";
						char* pTokenFound = "";
						glsl450tokentypes tokenFound = glsl450none;
						pTryToken = "mat2 ";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL && *(pPtr-1) == 32 ) { tokenFound = glsl450token_mat2; pTokenFound = pTryToken; }
						pTryToken = "mat3 ";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL && *(pPtr-1) == 32 ) { tokenFound = glsl450token_mat3; pTokenFound = pTryToken; }
						pTryToken = "mat4 ";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL && *(pPtr-1) == 32 ) { tokenFound = glsl450token_mat4; pTokenFound = pTryToken; }
						pTryToken = "texture2d(";			if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_texture2D; pTokenFound = pTryToken; }
						pTryToken = "texture2d (";			if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_texture2D; pTokenFound = pTryToken; }
						pTryToken = "texture2dlod(";		if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_texture2Dlod; pTokenFound = pTryToken; }
						pTryToken = "texture2dlod (";		if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_texture2Dlod; pTokenFound = pTryToken; }
						pTryToken = "precision highp ";		if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_precisionhighp; pTokenFound = pTryToken; }
						pTryToken = "precision mediump ";	if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_precisionhighp; pTokenFound = pTryToken; }
						pTryToken = "precision lowp ";		if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_precisionhighp; pTokenFound = pTryToken; }
						pTryToken = "highp ";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_highp; pTokenFound = pTryToken; }
						pTryToken = "mediump ";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_mediump; pTokenFound = pTryToken; }
						pTryToken = "lowp ";				if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL ) { tokenFound = glsl450token_lowp; pTokenFound = pTryToken; }
						pTryToken = "gl_FragColor";			if ( local_strincmp( pPtr, pTryToken, strlen(pTryToken) ) == NULL && iVertFragPass == 1 ) { iHasFragOutput = 1; tokenFound = glsl450token_gl_FragColor; pTokenFound = pTryToken; }
						if ( tokenFound != glsl450none || pPtr == pPtrEnd )
						{
							// token to capture 'stuff' since last real token (if any)
							glsl450tokenstruct addToken;
							unsigned int dwStuffSize = (unsigned int)(pPtr - pPtrLast);
							if ( dwStuffSize > 0 )
							{
								addToken.eTokenType = glsl450stuff;
								addToken.pTokenData = new char[dwStuffSize+1];
								memcpy ( addToken.pTokenData, pPtrLast, dwStuffSize );
								addToken.pTokenData[dwStuffSize] = 0;
								tokensequence.push_back ( addToken );
							}

							// now the real token found above (if not here due to end of line)
							if ( pPtr != pPtrEnd )
							{
								addToken.eTokenType = tokenFound;
								if ( tokenFound == glsl450tab )
								{
									addToken.pTokenData = new char[2];
									addToken.pTokenData[0] = 9;
									addToken.pTokenData[1] = 0;
								}
								else
								{
									addToken.pTokenData = NULL;
								}
								tokensequence.push_back ( addToken );
								pPtr += strlen(pTokenFound);
								bSkipAdvance = true;
							}

							// and continue collecting stuff until next good token
							pPtrLast = pPtr;
						}
					}

					// next character
					if ( bSkipAdvance==false ) pPtr++;
				}

				// if this was a uniform line, need to cap it with end token
				if ( bWasAUniformLine == true )
				{
					glsl450tokenstruct addToken;
					addToken.eTokenType = glsl450token_uniformend;
					addToken.pTokenData = NULL;
					tokensequence.push_back ( addToken );
				}

				// at end of each line, mark with newline token
				glsl450tokenstruct addToken;
				addToken.eTokenType = glsl450newline;
				addToken.pTokenData = NULL;
				tokensequence.push_back ( addToken );
			}

			// add a VERSION token if the shader does not contain one
			if ( tokensequence.size() > 0 )
			{
				if ( bFoundVersionToken == false )
				{
					glsl450tokenstruct addToken;
					addToken.pTokenData = NULL;
					addToken.eTokenType = glsl450newline;
					tokensequence.insert ( tokensequence.begin(), 1, addToken );
					addToken.eTokenType = glsl450newline;
					tokensequence.insert ( tokensequence.begin(), 1, addToken );
					addToken.eTokenType = glsl450token_version;
					tokensequence.insert ( tokensequence.begin(), 1, addToken );
				}
			}

			// eat up uniforms and produce a uniform group if flagged
			if ( bGroupUniformsTogether == true )
			{
				std::vector <char*> uniformList;
				uniformList.clear();
				char pCollectGroupLine[1024];
				int iFirstUniformEndTokenIndex = -1;
				//int iLastUniformEndTokenIndex = -1;
				bool bEatContentsOfUniform = false;
				int iTokenCount = (int)tokensequence.size();
				for ( int iTokenIndex = 0; iTokenIndex < iTokenCount; iTokenIndex++ )
				{
					glsl450tokenstruct pThisToken = tokensequence[iTokenIndex];
					switch ( pThisToken.eTokenType )
					{
						case glsl450token_uniform:
							if ( bEatContentsOfUniform == false )
							{
								strcpy ( pCollectGroupLine, "" );
								bEatContentsOfUniform = true;
								tokensequence[iTokenIndex].eTokenType = glsl450none;
							}
							break;
						default:
							if ( bEatContentsOfUniform == true )
							{
								if ( pThisToken.pTokenData )
								{
									strcat ( pCollectGroupLine, pThisToken.pTokenData );
								}
								else
								{
									if ( pThisToken.eTokenType == glsl450token_mat2 ) strcat ( pCollectGroupLine, "mat2 " );
									if ( pThisToken.eTokenType == glsl450token_mat3 ) strcat ( pCollectGroupLine, "mat3 " );
									if ( pThisToken.eTokenType == glsl450token_mat4 ) strcat ( pCollectGroupLine, "mat4 " );
								}
								tokensequence[iTokenIndex].eTokenType = glsl450none;
								if ( tokensequence[iTokenIndex].pTokenData ) 
								{ 
									delete tokensequence[iTokenIndex].pTokenData; 
									tokensequence[iTokenIndex].pTokenData = NULL; 
								}
							}
							break;
						case glsl450token_uniformend:
							if ( bEatContentsOfUniform == true )
							{
								char* pGroupLine = new char[strlen(pCollectGroupLine)+1];
								strcpy ( pGroupLine, pCollectGroupLine );
								uniformList.push_back(pGroupLine);
								bEatContentsOfUniform = false;
								tokensequence[iTokenIndex].eTokenType = glsl450none;
								if ( iFirstUniformEndTokenIndex == -1 ) iFirstUniformEndTokenIndex = iTokenIndex;
								//iLastUniformEndTokenIndex = iTokenIndex;

								// also eat carriage return if next token
								if ( tokensequence[iTokenIndex+1].eTokenType == glsl450newline ) 
									tokensequence[iTokenIndex+1].eTokenType = glsl450none;
							}
							break;
					}
				}
				if ( iFirstUniformEndTokenIndex != -1 ) //iLastUniformEndTokenIndex != -1 )
				{
					// now create a uniform group from list of qualifying uniform items above
					unsigned int iSizeOfUniformGroupTest = 0;
					for ( unsigned int u = 0; u < (unsigned int)uniformList.size(); u++ ) iSizeOfUniformGroupTest += (unsigned int)strlen(uniformList[u])+1+1+1;
					char* pNewUniformGroupData = new char[iSizeOfUniformGroupTest+1];
					strcpy ( pNewUniformGroupData, "" );
					for ( unsigned int u = 0; u < (unsigned int)uniformList.size(); u++ ) 
					{
						strcat ( pNewUniformGroupData, "\t");
						strcat ( pNewUniformGroupData, uniformList[u]);
						strcat ( pNewUniformGroupData, "\n");
					}
					glsl450tokenstruct addNewToken;
					addNewToken.eTokenType = glsl450token_uniformgroup;
					addNewToken.pTokenData = pNewUniformGroupData;
					tokensequence[iFirstUniformEndTokenIndex] = addNewToken;
					//tokensequence[iLastUniformEndTokenIndex] = addNewToken;
				}
			}

			int maxVaryingLocation = iVaryingLocationIndex;

			// Second pass - convert to new shader data
			unsigned int dwSafeSizeForNewShader = (unsigned int)strlen(pShaderTextDataPtr)*5;
			char* pNewShaderDataPtr = NULL;
			if ( iVertFragPass == 0 ) 
			{
				*ppVert450 = new char[dwSafeSizeForNewShader];
				pNewShaderDataPtr = *ppVert450;
			}
			else
			{
				*ppFrag450 = new char[dwSafeSizeForNewShader];
				pNewShaderDataPtr = *ppFrag450;

				// reset location indexes as we can reuse indices in pixel shader
				iAttributeLocationIndex = 0;
				iVaryingLocationIndex = 0;
			}
			int iTokenCount = (int)tokensequence.size();
			if ( pNewShaderDataPtr )
			{
				// go through tokensequence and reconstruct shader as a GLSL 4.5 version
				int iLocationTypeForMAT3Advance = 0;
				memset ( pNewShaderDataPtr, 0, dwSafeSizeForNewShader );
				strcpy ( pNewShaderDataPtr, "" );
				for ( int iTokenIndex = 0; iTokenIndex < iTokenCount; iTokenIndex++ )
				{
					glsl450tokenstruct pThisToken = tokensequence[iTokenIndex];
					switch ( pThisToken.eTokenType )
					{
						case glsl450none:
						{
							break;
						}
						case glsl450tab:
						{
							if ( pThisToken.pTokenData )
							{
								strcat ( pNewShaderDataPtr, pThisToken.pTokenData );
							}
							break;
						}
						case glsl450stuff:
						{
							if ( pThisToken.pTokenData )
							{
								strcat ( pNewShaderDataPtr, pThisToken.pTokenData );
							}
							break;
						}
						case glsl450startspace:
						{
							strcat ( pNewShaderDataPtr, " " );
							break;
						}
						case glsl450token_version:
						{
							strcat ( pNewShaderDataPtr, "#version 450" );
							iTokenIndex++; // skip whatever version number the old shader had
							break;
						}
						case glsl450token_attribute:
						{
							int locationIndex;
							if ( iVertFragPass == 0 )
							{
								locationIndex = iAttributeLocationIndex;
							}
							else
							{
								locationIndex = -1;
								int nextToken = iTokenIndex+1;
								while( tokensequence[nextToken].eTokenType != glsl450stuff ) nextToken++;
								for( int i = 0; i < maxVaryingLocation; i++ )
								{
									if ( m_szVaryingLocations[ i ] && strncmp( tokensequence[nextToken].pTokenData, m_szVaryingLocations[ i ], strlen(m_szVaryingLocations[ i ]) ) == 0 )
									{
										locationIndex = i;
										break;
									}
								}
								if ( locationIndex < 0 )
								{
									AppError( "Pixel shader \"%s\" has varying \"%s\" that does not exist in the vertex shader", szPSFilename ? szPSFilename : "Unknown", tokensequence[nextToken].pTokenData );
									return false;
								}
							}
							char pWorkStr[1024];
							sprintf ( pWorkStr, "layout(location = %d) in ", locationIndex );
							strcat ( pNewShaderDataPtr, pWorkStr );
							iAttributeLocationIndex++;
							iLocationTypeForMAT3Advance = 1;
							break;
						}
						case glsl450token_varying:
						{
							if ( iVertFragPass == 0 )
							{
								if ( iVaryingLocationIndex >= maxVaryings )
								{
									AppError( "Vertex shader \"%s\" varyings exceed limit of 32 vec4", szVSFilename ? szVSFilename : "Unknown" );
									return false;
								}
								int nextToken = iTokenIndex+1;
								while( tokensequence[nextToken].eTokenType != glsl450stuff ) nextToken++;
								m_szVaryingLocations[ iVaryingLocationIndex ] = new char [ strlen(tokensequence[nextToken].pTokenData)+1 ];
								strcpy( m_szVaryingLocations[ iVaryingLocationIndex ], tokensequence[nextToken].pTokenData );
								char *ptr = m_szVaryingLocations[ iVaryingLocationIndex ];
								while( *ptr )
								{
									if ( *ptr == ';' )
									{
										*(ptr+1) = 0;
										break;
									}
									ptr++;
								}
							}
							char pWorkStr[1024];
							sprintf ( pWorkStr, "layout(location = %d) out ", iVaryingLocationIndex );
							strcat ( pNewShaderDataPtr, pWorkStr );
							iVaryingLocationIndex++;
							iLocationTypeForMAT3Advance = 2;
							break;
						}
						case glsl450token_uniform:
						case glsl450token_uniformsampler:
						{
							char pUniformName[3];
							int iLargeLetter = iUniformNameAlphaIndex / 26;
							int iSmallLetter = iUniformNameAlphaIndex - (iLargeLetter*26);
							pUniformName[0] = 65+iLargeLetter;
							pUniformName[1] = 65+iSmallLetter;
							pUniformName[2] = 0;
							char pWorkStr[1024];
							if ( pThisToken.eTokenType == glsl450token_uniform )
							{
								sprintf ( pWorkStr, "layout(binding = %d) uniform %s { ", iBindingUniformIndex, pUniformName );
							}
							else
							{
								sprintf ( pWorkStr, "layout(binding = %d) uniform sampler", iBindingUniformIndex );
							}
							strcat ( pNewShaderDataPtr, pWorkStr );
							iBindingUniformIndex++;
							iUniformNameAlphaIndex++;
							break;
						}
						case glsl450token_uniformend:
						{
							strcat ( pNewShaderDataPtr, " };" );
							break;
						}
						case glsl450token_uniformgroup:
						{
							char pUniformName[3];
							int iLargeLetter = iUniformNameAlphaIndex / 26;
							int iSmallLetter = iUniformNameAlphaIndex - (iLargeLetter*26);
							pUniformName[0] = 65+iLargeLetter;
							pUniformName[1] = 65+iSmallLetter;
							pUniformName[2] = 0;
							char pWorkStr[1024];
							sprintf ( pWorkStr, "layout(binding = %d) uniform %s\n", iBindingUniformIndex, pUniformName );
							strcat ( pNewShaderDataPtr, pWorkStr );
							strcat ( pNewShaderDataPtr, "{\n" );
							strcat ( pNewShaderDataPtr, pThisToken.pTokenData );
							strcat ( pNewShaderDataPtr, "};\n" );
							iBindingUniformIndex++;
							iUniformNameAlphaIndex++;
							break;
						}
						case glsl450token_fragmain:
						{
							// only add openglFragColor if the shader outputs a color value
							if ( iVertFragPass == 1 && iHasFragOutput ) 
							{
								char pWorkStr[1024];
								sprintf ( pWorkStr, "layout(location = %d) out vec4 openglFragColor;\r\n", iVaryingLocationIndex );
								strcat ( pNewShaderDataPtr, pWorkStr );
								iVaryingLocationIndex++;
								strcat ( pNewShaderDataPtr, "\r\n" );
							}
							strcat ( pNewShaderDataPtr, "void main" );
							break;
						}
						case glsl450token_gl_FragColor:
						{
							strcat ( pNewShaderDataPtr, "openglFragColor" );
							break;
						}
						case glsl450token_texture2D:
						{
							strcat ( pNewShaderDataPtr, "texture(" );
							break;
						}
						case glsl450token_texture2Dlod:
						{
							strcat ( pNewShaderDataPtr, "textureLod(" );
							break;
						}
						case glsl450token_mat2:
						{
							strcat ( pNewShaderDataPtr, "mat2 " );
							if ( iLocationTypeForMAT3Advance == 1 ) iAttributeLocationIndex += 1;
							if ( iLocationTypeForMAT3Advance == 2 ) iVaryingLocationIndex += 1;
							iLocationTypeForMAT3Advance = 0;
							break;
						}
						case glsl450token_mat3:
						{
							strcat ( pNewShaderDataPtr, "mat3 " );
							if ( iLocationTypeForMAT3Advance == 1 ) iAttributeLocationIndex += 2;
							if ( iLocationTypeForMAT3Advance == 2 ) iVaryingLocationIndex += 2;
							iLocationTypeForMAT3Advance = 0;
							break;
						}
						case glsl450token_mat4:
						{
							strcat ( pNewShaderDataPtr, "mat4 " );
							if ( iLocationTypeForMAT3Advance == 1 ) iAttributeLocationIndex += 3;
							if ( iLocationTypeForMAT3Advance == 2 ) iVaryingLocationIndex += 3;
							iLocationTypeForMAT3Advance = 0;
							break;
						}
						case glsl450token_precisionhighp:
						{
							strcat ( pNewShaderDataPtr, "precision highp " );
							break;
						}
						case glsl450token_highp:
						{
							//strcat ( pNewShaderDataPtr, "highp " ); // above quiets precision use
							break;
						}
						case glsl450token_mediump:
						{
							//strcat ( pNewShaderDataPtr, "mediump " ); // above quiets precision use
							break;
						}
						case glsl450token_lowp:
						{
							//strcat ( pNewShaderDataPtr, "lowp " ); // above quiets precision use
							break;
						}
						case glsl450newline:
						{
							strcat ( pNewShaderDataPtr, "\r\n" );
							break;
						}
					}
				}
			}

			// free resources in line array
			for ( int iTokenIndex = 0; iTokenIndex < iTokenCount; iTokenIndex++ ) if ( tokensequence[iTokenIndex].pTokenData ) delete tokensequence[iTokenIndex].pTokenData;
			for ( int iLine = 0; iLine < iLineCount; iLine++ ) if ( lines[iLine] ) delete lines[iLine];
			tokensequence.clear();
			lines.clear();
		}
	}

	for( unsigned int i = 0; i < maxVaryings; i++ ) if ( m_szVaryingLocations[i] ) delete [] m_szVaryingLocations[i];
	
	// success
	return true;
}
