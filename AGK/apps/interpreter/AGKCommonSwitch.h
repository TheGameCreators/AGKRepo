case AGKI_NOP: // do nothing
	{
		break;
	}

	// push literal
	case AGKI_PUSH_INT:
	{
		m_pStack[ m_iStackPtr++ ].i = m_pInstructions[ instruction ].i;
		break;
	}
	case AGKI_PUSH_FLOAT:
	{
		m_pStack[ m_iStackPtr++ ].f = m_pInstructions[ instruction ].f;
		break;
	}
	case AGKI_PUSH_STRING:
	{
		UINT index = m_pInstructions[ instruction ].i;
		if ( (int)index == -1 ) m_pStrStack[ m_iStrStackPtr++ ].SetStr("");
		else
		{
			CHECKRANGE( index, 0, m_iNumStrings, "String index out of bounds" )
			m_pStrStack[ m_iStrStackPtr++ ].SetStr( m_pStringTable[ index ] );
		}
		break;
	}

	// push variable
	case AGKI_PUSH_VAR_INT:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesInt, "Integer variable index out of bounds" )

		m_pStack[ m_iStackPtr++ ].i = m_pVariablesInt[ index ];
		break;
	}
	case AGKI_PUSH_VAR_FLOAT:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesFloat, "Float variable index out of bounds" )

		m_pStack[ m_iStackPtr++ ].f = m_pVariablesFloat[ index ];
		break;
	}
	case AGKI_PUSH_VAR_STRING:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesString, "String variable index out of bounds" )

		m_pStrStack[ m_iStrStackPtr++ ].SetStr( m_pVariablesString[ index ].GetStr() );
		//m_pStack[ m_iStackPtr++ ].p = m_pVariablesString + index;
		break;
	}
	case AGKI_PUSH_ARRAY:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesArray, "Array variable index out of bounds" )

		m_pStack[ m_iStackPtr++ ].p = m_pVariablesArray + index;
		break;
	}
	case AGKI_PUSH_TYPE:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesType, "Type variable index out of bounds" )

		m_pStack[ m_iStackPtr++ ].p = m_pVariablesType + index;
		break;
	}

	// push local variable from frame pointer to the top of the stack
	case AGKI_PUSH_LOCAL_INT:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iStackPtr++ ].i = m_pStack[ m_iFramePtr+index ].i;
		break;
	}
	case AGKI_PUSH_LOCAL_FLOAT:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iStackPtr++ ].f = m_pStack[ m_iFramePtr+index ].f;
		break;
	}
	case AGKI_PUSH_LOCAL_STRING:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStrStack[ m_iStrStackPtr++ ].SetStr( m_pStrStack[ m_iStrFramePtr+index ].GetStr() );
		break;
	}
	case AGKI_PUSH_LOCAL_ARRAY:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iStackPtr++ ].p = m_pStack[ m_iFramePtr+index ].p;
		break;
	}
	case AGKI_PUSH_LOCAL_TYPE:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iStackPtr++ ].p = m_pStack[ m_iFramePtr+index ].p;
		break;
	}

	// temp variables
	case AGKI_PUSH_TEMP_TYPE:
	{
		// assign memory for a temp type variable
		int typeStruct = m_pInstructions[ instruction ].i;
		
		stType *pType = new stType();
		AllocType( pType, typeStruct );
		pType->m_iTemp = m_iFunctionLevel+1;

		m_pStack[ m_iStackPtr++ ].p = pType;
		break;
	}
	case AGKI_PUSH_TEMP_ARRAY:
	{
		// assign memory for a temp array variable
		stArray *pArray = new stArray();
		pArray->m_iTemp = m_iFunctionLevel+1;
		m_pStack[ m_iStackPtr++ ].p = pArray;
		break;
	}

	case AGKI_POP_VAR:
	{
		m_iStackPtr--;
		break;
	}
	case AGKI_POP_STRING:
	{
		m_iStrStackPtr--;
		break;
	}
	case AGKI_POP_TYPE:
	{
		stType *pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		if ( pType->m_iTemp > 0 && pType->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeType( pType );
			delete pType;
			m_pStack[ m_iStackPtr ].p = 0; // clear the stack item in case it is referenced by the debugger
		}
		break;
	}
	case AGKI_POP_ARRAY:
	{
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		if ( pArray->m_iTemp > 0 && pArray->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeArray( pArray );
			delete pArray;
			m_pStack[ m_iStackPtr ].p = 0; // clear the stack item in case it is referenced by the debugger
		}
		break;
	}

	// array dereferencing
	case AGKI_ARRAY_DEREF_INT:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ m_iStackPtr-1 ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		m_pStack[ m_iStackPtr-1 ].i = pArray->m_pI[ index ];
		break;
	}
	case AGKI_ARRAY_DEREF_FLOAT:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ m_iStackPtr-1 ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		m_pStack[ m_iStackPtr-1 ].f = pArray->m_pF[ index ];
		break;
	}
	case AGKI_ARRAY_DEREF_STRING:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		m_pStrStack[ m_iStrStackPtr++ ].SetStr( pArray->m_pS[ index ]->GetStr() );
		//m_pStack[ m_iStackPtr-1 ].p = pArray->m_pS + index;
		break;
	}
	case AGKI_ARRAY_DEREF_TYPE:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ m_iStackPtr-1 ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		m_pStack[ m_iStackPtr-1 ].p = pArray->m_pT[ index ];
		break;
	}
	case AGKI_ARRAY_DEREF_ARRAY:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ m_iStackPtr-1 ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		m_pStack[ m_iStackPtr-1 ].p = pArray->m_pA[ index ];
		break;
	}

	// type dereferencing
	case AGKI_TYPE_DEREF_INT:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		stType* pType = (stType*) (m_pStack[ m_iStackPtr-1 ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		m_pStack[ m_iStackPtr-1 ].i = *((int*)(pType->m_pData + index));
		break;
	}
	case AGKI_TYPE_DEREF_FLOAT:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		stType* pType = (stType*) (m_pStack[ m_iStackPtr-1 ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		m_pStack[ m_iStackPtr-1 ].f = *((float*)(pType->m_pData + index));
		break;
	}
	case AGKI_TYPE_DEREF_STRING:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )
		
		m_pStrStack[ m_iStrStackPtr++ ].SetStr( ((uString*)(pType->m_pData + index))->GetStr() );
		break;
	}
	case AGKI_TYPE_DEREF_TYPE:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		stType* pType = (stType*) (m_pStack[ m_iStackPtr-1 ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		m_pStack[ m_iStackPtr-1 ].p = (pType->m_pData + index);
		break;
	}
	case AGKI_TYPE_DEREF_ARRAY:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		stType* pType = (stType*) (m_pStack[ m_iStackPtr-1 ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		m_pStack[ m_iStackPtr-1 ].p = (pType->m_pData + index);
		break;
	}

	// gosub
	case AGKI_GOSUB:
	{
		m_pStack[ m_iStackPtr++ ].i = instruction+1; // push return address

		// find label by index
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumLabels, "Label index out of bounds, probably a parser bug" );

		m_iProgramCounter = m_pLabels[ index ];
		m_iGosubCounter++;
		break;
	}

	case AGKI_GOSUB_RETURN:
	{
		if ( m_iGosubCounter <= 0 ) 
		{
			RuntimeError( "Encountered a RETURN statement when not in a GOSUB" );
			return 1;
		}
		m_iGosubCounter--;

		m_iProgramCounter = m_pStack[ --m_iStackPtr ].i; // return address
		break;
	}

	// user function
	case AGKI_FUNCTION_CALL:
	{
		// function parameters should already have been pushed

		m_pStack[ m_iStackPtr++ ].i = instruction+1; // push return address
		m_pStack[ m_iStackPtr++ ].i = m_iFramePtr; // push current frame pointer
		m_pStack[ m_iStackPtr++ ].i = m_iStrFramePtr; // push string frame pointer
		m_iFramePtr = m_iStackPtr-3; // frame pointer points to return address
		m_iStrFramePtr = m_iStrStackPtr;
		
		m_iProgramCounter = m_pInstructions[ instruction ].i; // jump to function code

		m_iFunctionLevel++;
		// function will now set up local variables
		break;
	}

	case AGKI_FUNCTION_EXIT:
	{
		// local variables should already have been cleaned up
		m_iFunctionLevel--;

		m_iStrFramePtr = m_pStack[ --m_iStackPtr ].i; // old string frame pointer
		m_iFramePtr = m_pStack[ --m_iStackPtr ].i; // old stack frame pointer
		m_iProgramCounter = m_pStack[ --m_iStackPtr ].i; // return address

		// caller will now clean up function parameters from stack
		break;
	}

	// create local variables
	case AGKI_PUSH_BLANK:
	{
		int numLocals = m_pInstructions[ instruction ].i; // not including strings
		int numStrings = m_pInstructions[ instruction ].i2; // strings
		
		// zero variables
		for ( int i = 0; i < numLocals; i++ ) m_pStack[ m_iStackPtr+i ].i = 0;
		for ( int i = 0; i < numStrings; i++ ) m_pStrStack[ m_iStrStackPtr+i ].ClearTemp();

		m_iStackPtr += numLocals;
		m_iStrStackPtr += numStrings;

		break;
	}
	case AGKI_LOCAL_VAR_DEC_TYPE:
	{
		// assign memory for a local type variable
		int index = m_pInstructions[ instruction ].i;
		int typeStruct = m_pInstructions[ instruction ].i2;
		
		stType *pType = new stType();
		AllocType( pType, typeStruct );

		m_pStack[ m_iFramePtr + index ].p = pType;
		break;
	}
	case AGKI_LOCAL_VAR_DEC_ARRAY:
	{
		// assign memory for a local array variable
		int index = m_pInstructions[ instruction ].i;

		stArray *pArray = new stArray();
		// can't allocate array until we know its data type

		m_pStack[ m_iFramePtr + index ].p = pArray;
		break;
	}
	
	// delete local variables
	case AGKI_POP_BLANK:
	{
		// arrays and types must have had their data cleared with another command
		int numLocals = m_pInstructions[ instruction ].i; // not including strings
		int numStrings = m_pInstructions[ instruction ].i2;
		m_iStackPtr -= numLocals;
		m_iStrStackPtr -= numStrings;
		break;
	}
	case AGKI_LOCAL_VAR_DELETE_TYPE:
	{
		int index = m_pInstructions[ instruction ].i;
		stType *pType = (stType*) (m_pStack[ m_iFramePtr + index ].p);

		FreeType( pType );
		delete pType;
		break;
	}
	case AGKI_LOCAL_VAR_DELETE_ARRAY:
	{
		int index = m_pInstructions[ instruction ].i;
		stArray *pArray = (stArray*) (m_pStack[ m_iFramePtr + index ].p);

		FreeArray( pArray );
		delete pArray;
		break;
	}

	// array block resizing for multi-dimensional arrays, allocates all sub arrays (of arbitrary depth) as a single data type
	case AGKI_ARRAY_BLOCK_RESIZE:
	{
		// dimension lengths are on the stack
		int numDimensions = m_pInstructions[ instruction ].i;
		UINT type = m_pInstructions[ instruction ].i2; // both data type and structID (if needed) packed in i2

		stArray *pArray = (stArray*) (m_pStack[ m_iStackPtr-(numDimensions+1) ].p);
		BlockResizeArray( pArray, numDimensions, type & 0xff, type >> 8 );

		// pop lengths and array pointer off the stack
		m_iStackPtr -= (numDimensions+1);
		break;
	}

	// resize single array, can't change data type
	case AGKI_ARRAY_RESIZE:
	{
		int type = m_pInstructions[ instruction ].i;
		int length = m_pStack[ --m_iStackPtr ].i;
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		length++; // convert between BASIC and C++ indexes
		if ( length < 0 )
		{
			RuntimeError( "Array size cannot be less than -1" );
			return 1;
		}
		
		SizeArray( pArray, length );
		pArray->m_iLength = pArray->m_iTotalLength; // arrays resized like this always report full if used as lists
		break;
	}

	// assign array some default values from the stack
	case AGKI_ARRAY_ASSIGN_INT:
	{
		// values are on the stack
		int numValues = m_pInstructions[ instruction ].i;
		int shrink = m_pInstructions[ instruction ].i2;

		stArray *pArray = (stArray*) (m_pStack[ m_iStackPtr-(numValues+1) ].p);
		if ( (int)(pArray->m_iTotalLength) < numValues ) SizeArray( pArray, numValues );
		if ( shrink ) pArray->m_iLength = numValues;
		else if ( (int)(pArray->m_iLength) < numValues ) pArray->m_iLength = numValues;

		for ( int i = 0; i < numValues; i++ )
		{
			int index = numValues - i;
			pArray->m_pI[ i ] = m_pStack[ m_iStackPtr-index ].i;
		}

		// pop values and array pointer off the stack
		m_iStackPtr -= (numValues+1);
		break;
	}
	case AGKI_ARRAY_ASSIGN_FLOAT:
	{
		// values are on the stack
		int numValues = m_pInstructions[ instruction ].i;
		int shrink = m_pInstructions[ instruction ].i2;

		stArray *pArray = (stArray*) (m_pStack[ m_iStackPtr-(numValues+1) ].p);
		if ( (int)(pArray->m_iTotalLength) < numValues ) SizeArray( pArray, numValues );
		if ( shrink ) pArray->m_iLength = numValues;
		else if ( (int)(pArray->m_iLength) < numValues ) pArray->m_iLength = numValues;

		for ( int i = 0; i < numValues; i++ )
		{
			int index = numValues - i;
			pArray->m_pF[ i ] = m_pStack[ m_iStackPtr-index ].f;
		}

		// pop values and array pointer off the stack
		m_iStackPtr -= (numValues+1);
		break;
	}
	case AGKI_ARRAY_ASSIGN_STRING:
	{
		// values are on the stack
		int numValues = m_pInstructions[ instruction ].i;
		int shrink = m_pInstructions[ instruction ].i2;

		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		if ( (int)(pArray->m_iTotalLength) < numValues ) SizeArray( pArray, numValues );
		if ( shrink ) pArray->m_iLength = numValues;
		else if ( (int)(pArray->m_iLength) < numValues ) pArray->m_iLength = numValues;

		for ( int i = 0; i < numValues; i++ )
		{
			int index = numValues - i;
			pArray->m_pS[ i ]->SetStr( m_pStrStack[ m_iStrStackPtr-index ].GetStr() );
		}

		// pop values off the stack
		m_iStrStackPtr -= numValues;
		break;
	}

	case AGKI_ARRAY_LENGTH:
	{
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		int length = pArray->m_iLength;
		length--; // convert between BASIC and C++ indexes
		m_pStack[ m_iStackPtr++ ].i = length;
		break;
	}

	// array insert at tail
	case AGKI_ARRAY_INSERT_TAIL_INT:
	{
		int value = m_pStack[ --m_iStackPtr ].i;
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_INT )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}
		
		pArray->m_pI[ pArray->m_iLength++ ] = value;
		break;
	}
	case AGKI_ARRAY_INSERT_TAIL_FLOAT:
	{
		float value = m_pStack[ --m_iStackPtr ].f;
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_FLOAT )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}
		
		pArray->m_pF[ pArray->m_iLength++ ] = value;
		break;
	}
	case AGKI_ARRAY_INSERT_TAIL_STRING:
	{
		uString *value = m_pStrStack + (--m_iStrStackPtr);
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_STRING )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}
		
		pArray->m_pS[ pArray->m_iLength++ ]->SetStr( value->GetStr() );
		break;
	}
	case AGKI_ARRAY_INSERT_TAIL_TYPE:
	{
		stType *value = (stType*) (m_pStack[ --m_iStackPtr ].p);
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_TYPE )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}
		
		CopyType( value, pArray->m_pT[ pArray->m_iLength++ ] );
		if ( value->m_iTemp > 0 && value->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeType( value );
			delete value;
			m_pStack[ m_iStackPtr+1 ].p = 0; // clear the stack item in case it is referenced by the debugger
		}
		break;
	}
	case AGKI_ARRAY_INSERT_TAIL_ARRAY:
	{
		stArray *value = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_ARRAY )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}
		
		CopyArray( value, pArray->m_pA[ pArray->m_iLength++ ] );
		if ( value->m_iTemp > 0 && value->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeArray( value );
			delete value;
			m_pStack[ m_iStackPtr+1 ].p = 0; // clear the stack item in case it is referenced by the debugger
		}
		break;
	}

	// array insert at index
	case AGKI_ARRAY_INSERT_INT:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		int value = m_pStack[ --m_iStackPtr ].i;
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_INT )
		CHECKARRAYRANGE( index, pArray->m_iLength )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}

		for ( int i = (int)(pArray->m_iLength)-1; i >= (int)index; i-- ) pArray->m_pI[ i+1 ] = pArray->m_pI[ i ];
		pArray->m_pI[ index ] = value;
		pArray->m_iLength++;
		break;
	}
	case AGKI_ARRAY_INSERT_FLOAT:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		float value = m_pStack[ --m_iStackPtr ].f;
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_FLOAT )
		CHECKARRAYRANGE( index, pArray->m_iLength )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}

		for ( int i = (int)(pArray->m_iLength)-1; i >= (int)index; i-- ) pArray->m_pF[ i+1 ] = pArray->m_pF[ i ];
		pArray->m_pF[ index ] = value;
		pArray->m_iLength++;
		break;
	}
	case AGKI_ARRAY_INSERT_STRING:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		uString *value = m_pStrStack + (--m_iStrStackPtr);
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_STRING )
		CHECKARRAYRANGE( index, pArray->m_iLength )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}

		uString *temp = pArray->m_pS[ pArray->m_iLength ];
		temp->SetStr( value->GetStr() );

		for ( int i = (int)(pArray->m_iLength)-1; i >= (int)index; i-- ) pArray->m_pS[ i+1 ] = pArray->m_pS[ i ];
		pArray->m_pS[ index ] = temp;
		pArray->m_iLength++;
		break;
	}
	case AGKI_ARRAY_INSERT_TYPE: 
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stType *value = (stType*) (m_pStack[ --m_iStackPtr ].p);
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_TYPE )
		CHECKARRAYRANGE( index, pArray->m_iLength )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}

		stType *temp = pArray->m_pT[ pArray->m_iLength ];
		CopyType( value, temp );
		if ( value->m_iTemp > 0 && value->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeType( value );
			delete value;
			m_pStack[ m_iStackPtr+1 ].p = 0; // clear the stack item in case it is referenced by the debugger
		}

		for ( int i = (int)(pArray->m_iLength)-1; i >= (int)index; i-- ) pArray->m_pT[ i+1 ] = pArray->m_pT[ i ];
		pArray->m_pT[ index ] = temp;
		pArray->m_iLength++;
		break;
	}
	case AGKI_ARRAY_INSERT_ARRAY: 
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray *value = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYTYPE( pArray, AGK_DATA_TYPE_ARRAY )
		CHECKARRAYRANGE( index, pArray->m_iLength )

		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}

		stArray *temp = pArray->m_pA[ pArray->m_iLength ];
		CopyArray( value, temp );
		if ( value->m_iTemp > 0 && value->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeArray( value );
			delete value;
			m_pStack[ m_iStackPtr+1 ].p = 0; // clear the stack item in case it is referenced by the debugger
		}

		for ( int i = (int)(pArray->m_iLength)-1; i >= (int)index; i-- ) pArray->m_pA[ i+1 ] = pArray->m_pA[ i ];
		pArray->m_pA[ index ] = temp;
		pArray->m_iLength++;
		break;
	}

	// array remove from tail
	case AGKI_ARRAY_REMOVE_TAIL:
	{
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		if ( pArray->m_iLength == 0 )
		{
			RuntimeError( "Cannot remove an element from an empty array" );
			return 1;
		}

		switch( pArray->GetCurrType() )
		{
			case AGK_DATA_TYPE_INT:
			{
				pArray->m_pI[ pArray->m_iLength-1 ] = 0;
				pArray->m_iLength--;
				break;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				pArray->m_pF[ pArray->m_iLength-1 ] = 0;
				pArray->m_iLength--;
				break;
			}
			case AGK_DATA_TYPE_STRING:
			{
				pArray->m_pS[ pArray->m_iLength-1 ]->ClearTemp();
				pArray->m_iLength--;
				break;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				ZeroType( pArray->m_pT[ pArray->m_iLength-1 ] );
				pArray->m_iLength--;
				break;
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				ZeroArray( pArray->m_pA[ pArray->m_iLength-1 ] );
				pArray->m_iLength--;
				break;
			}
		}
		
		if ( pArray->m_iTotalLength > pArray->m_iLength*2 )
		{
			UINT newLength = pArray->m_iTotalLength*2 / 3;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}
		break;
	}
	case AGKI_ARRAY_REMOVE:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		switch( pArray->GetCurrType() )
		{
			case AGK_DATA_TYPE_INT:
			{
				for ( UINT i = index; i < pArray->m_iLength-1; i++ ) pArray->m_pI[ i ] = pArray->m_pI[ i+1 ];
				pArray->m_pI[ pArray->m_iLength-1 ] = 0;
				pArray->m_iLength--;
				break;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				for ( UINT i = index; i < pArray->m_iLength-1; i++ ) pArray->m_pF[ i ] = pArray->m_pF[ i+1 ];
				pArray->m_pF[ pArray->m_iLength-1 ] = 0;
				pArray->m_iLength--;
				break;
			}
			case AGK_DATA_TYPE_STRING:
			{
				uString *temp = pArray->m_pS[ index ];
				temp->ClearTemp();
				for ( UINT i = index; i < pArray->m_iLength-1; i++ ) pArray->m_pS[ i ] = pArray->m_pS[ i+1 ];
				pArray->m_pS[ pArray->m_iLength-1 ] = temp;
				pArray->m_iLength--;
				break;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				stType *temp = pArray->m_pT[ index ];
				ZeroType( temp );
				for ( UINT i = index; i < pArray->m_iLength-1; i++ ) pArray->m_pT[ i ] = pArray->m_pT[ i+1 ];
				pArray->m_pT[ pArray->m_iLength-1 ] = temp;
				pArray->m_iLength--;
				break;
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				stArray *temp = pArray->m_pA[ index ];
				ZeroArray( temp );
				for ( UINT i = index; i < pArray->m_iLength-1; i++ ) pArray->m_pA[ i ] = pArray->m_pA[ i+1 ];
				pArray->m_pA[ pArray->m_iLength-1 ] = temp;
				pArray->m_iLength--;
				break;
			}
		}

		if ( pArray->m_iTotalLength > pArray->m_iLength*2 )
		{
			UINT newLength = pArray->m_iTotalLength*2 / 3;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}
						
		break;
	}

	// swap two elements in an array
	case AGKI_ARRAY_SWAP:
	{
		UINT index2 = m_pStack[ --m_iStackPtr ].i;
		UINT index1 = m_pStack[ --m_iStackPtr ].i;
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index1, pArray->m_iLength )
		CHECKARRAYRANGE( index2, pArray->m_iLength )
		
		switch( pArray->GetCurrType() )
		{
			case AGK_DATA_TYPE_INT:
			{
				int temp = pArray->m_pI[ index1 ];
				pArray->m_pI[ index1 ] = pArray->m_pI[ index2 ];
				pArray->m_pI[ index2 ] = temp;
				break;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				float temp = pArray->m_pF[ index1 ];
				pArray->m_pF[ index1 ] = pArray->m_pF[ index2 ];
				pArray->m_pF[ index2 ] = temp;
				break;
			}
			case AGK_DATA_TYPE_STRING:
			{
				uString *temp = pArray->m_pS[ index1 ];
				pArray->m_pS[ index1 ] = pArray->m_pS[ index2 ];
				pArray->m_pS[ index2 ] = temp;
				break;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				stType *temp = pArray->m_pT[ index1 ];
				pArray->m_pT[ index1 ] = pArray->m_pT[ index2 ];
				pArray->m_pT[ index2 ] = temp;
				break;
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				stArray *temp = pArray->m_pA[ index1 ];
				pArray->m_pA[ index1 ] = pArray->m_pA[ index2 ];
				pArray->m_pA[ index2 ] = temp;
				break;
			}
		}
		break;
	}

	// reverse the array
	case AGKI_ARRAY_REVERSE:
	{
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		
		switch( pArray->GetCurrType() )
		{
			case AGK_DATA_TYPE_INT:
			{
				int temp;
				int maxindex = pArray->m_iLength-1;
				for ( int i = 0; i < (int)pArray->m_iLength/2; i++ )
				{
					temp = pArray->m_pI[ i ];
					pArray->m_pI[ i ] = pArray->m_pI[ maxindex-i ];
					pArray->m_pI[ maxindex-i ] = temp;
				}
				break;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				float temp;
				int maxindex = pArray->m_iLength-1;
				for ( int i = 0; i < (int)pArray->m_iLength/2; i++ )
				{
					temp = pArray->m_pF[ i ];
					pArray->m_pF[ i ] = pArray->m_pF[ maxindex-i ];
					pArray->m_pF[ maxindex-i ] = temp;
				}
				break;
			}
			case AGK_DATA_TYPE_STRING:
			{
				uString *temp;
				int maxindex = pArray->m_iLength-1;
				for ( int i = 0; i < (int)pArray->m_iLength/2; i++ )
				{
					temp = pArray->m_pS[ i ];
					pArray->m_pS[ i ] = pArray->m_pS[ maxindex-i ];
					pArray->m_pS[ maxindex-i ] = temp;
				}
				break;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				stType *temp;
				int maxindex = pArray->m_iLength-1;
				for ( int i = 0; i < (int)pArray->m_iLength/2; i++ )
				{
					temp = pArray->m_pT[ i ];
					pArray->m_pT[ i ] = pArray->m_pT[ maxindex-i ];
					pArray->m_pT[ maxindex-i ] = temp;
				}
				break;
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				stArray *temp;
				int maxindex = pArray->m_iLength-1;
				for ( int i = 0; i < (int)pArray->m_iLength/2; i++ )
				{
					temp = pArray->m_pA[ i ];
					pArray->m_pA[ i ] = pArray->m_pA[ maxindex-i ];
					pArray->m_pA[ maxindex-i ] = temp;
				}
				break;
			}
		}
		break;
	}

	// array sorting
	case AGKI_ARRAY_SORT:
	{
		UINT noCase = m_pInstructions[ instruction ].i;

		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		if ( pArray->m_iLength == 0 ) 
		{
			//RuntimeError("Cannot sort an empty array");
			//return 1;
			// let them sort an empty array, do nothing
			break;
		}

		AGKSortValue *pValues = new AGKSortValue[ pArray->m_iLength ];
		
		switch( pArray->GetCurrType() )
		{
			case AGK_DATA_TYPE_INT:
			{
				for ( UINT i = 0; i < pArray->m_iLength; i++ )
				{
					converter c; c.i = pArray->m_pI[ i ];
					pValues[ i ].iValue = agk::SortIntToUINT( pArray->m_pI[ i ] );
					pValues[ i ].ptr = c.p;
				}
				agk::SortArray( pValues, pArray->m_iLength );
				for ( UINT i = 0; i < pArray->m_iLength; i++ )
				{
					converter c; c.p = pValues[ i ].ptr;
					pArray->m_pI[ i ] = c.i;
				}
				break;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				for ( UINT i = 0; i < pArray->m_iLength; i++ )
				{
					converter c; c.f = pArray->m_pF[ i ];
					pValues[ i ].iValue = agk::SortFloatToUINT( pArray->m_pF[ i ] );
					pValues[ i ].ptr = c.p;
				}
				agk::SortArray( pValues, pArray->m_iLength );
				for ( UINT i = 0; i < pArray->m_iLength; i++ )
				{
					converter c; c.p = pValues[ i ].ptr;
					pArray->m_pF[ i ] = c.f;
				}
				break;
			}
			case AGK_DATA_TYPE_STRING:
			{
				if ( noCase ) qsort( pArray->m_pS, pArray->m_iLength, sizeof(uString*), &ProgramData::CompareStringCase );
				else qsort( pArray->m_pS, pArray->m_iLength, sizeof(uString*), &ProgramData::CompareString );
				break;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				stTypeDec *pTypeStruct = &(m_pTypeStructs[ pArray->GetTypeType() ]);
				int offset = pTypeStruct->m_pVarTypes[ pTypeStruct->m_iFirstIndex ].varOffset;
				int varType = pTypeStruct->m_pVarTypes[ pTypeStruct->m_iFirstIndex ].varType;
				switch( varType )
				{
					case AGK_DATA_TYPE_INT:
					{
						for ( UINT i = 0; i < pArray->m_iLength; i++ )
						{
							pValues[ i ].iValue = agk::SortIntToUINT( *((int*)(pArray->m_pT[ i ]->m_pData + offset)) );
							pValues[ i ].ptr = pArray->m_pT[ i ];
						}
						agk::SortArray( pValues, pArray->m_iLength );
						for ( UINT i = 0; i < pArray->m_iLength; i++ )
						{
							pArray->m_pT[ i ] = (stType*) pValues[ i ].ptr;
						}
						break;
					}
					case AGK_DATA_TYPE_FLOAT:
					{
						for ( UINT i = 0; i < pArray->m_iLength; i++ )
						{
							pValues[ i ].iValue = agk::SortFloatToUINT( *((float*)(pArray->m_pT[ i ]->m_pData + offset)) );
							pValues[ i ].ptr = pArray->m_pT[ i ];
						}
						agk::SortArray( pValues, pArray->m_iLength );
						for ( UINT i = 0; i < pArray->m_iLength; i++ )
						{
							pArray->m_pT[ i ] = (stType*) pValues[ i ].ptr;
						}
						break;
					}
					case AGK_DATA_TYPE_STRING:
					{
						sortTypeString *pValues2 = new sortTypeString[ pArray->m_iLength ];
						for ( UINT i = 0; i < pArray->m_iLength; i++ )
						{
							pValues2[ i ].pString = (uString*)(pArray->m_pT[ i ]->m_pData + offset);
							pValues2[ i ].pType = pArray->m_pT[ i ];
						}
						
						if ( noCase ) qsort( pValues2, pArray->m_iLength, sizeof(sortTypeString), &ProgramData::CompareTypeStringCase );
						else qsort( pValues2, pArray->m_iLength, sizeof(sortTypeString), &ProgramData::CompareTypeString );
						
						for ( UINT i = 0; i < pArray->m_iLength; i++ )
						{
							pArray->m_pT[ i ] = pValues2[ i ].pType;
						}
						delete [] pValues2;
						break;
					}
					case AGK_DATA_TYPE_TYPE:
					{
						RuntimeError("Cannot sort array of types as the type does not have an Integer, Float, or String as its first member");
						return 1;
					}
					case AGK_DATA_TYPE_ARRAY:
					{
						RuntimeError( "Cannot sort arrays of arrays" );
						return 1;
					}
				}
				break;
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				RuntimeError( "Cannot sort arrays of arrays" );
				return 1;
			}
		}
		delete [] pValues;
		break;
	}

	// insert an item assuming the array is sorted
	case AGKI_ARRAY_INSERT_SORTED:
	{
		stArray *pArray = (stArray*) (m_pStack[ m_iStackPtr-2 ].p);
		
		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}

		switch( pArray->GetCurrType() )
		{
			case AGK_DATA_TYPE_INT:
			{
				int value = m_pStack[ --m_iStackPtr ].i;
				int index = pArray->FindInt( 0, value, 1 );
				if ( index >= (int)pArray->m_iLength ) pArray->m_pI[ pArray->m_iLength++ ] = value;
				else
				{
					for( int i = ((int)pArray->m_iLength)-1; i >= index; i-- )
					{
						pArray->m_pI[ i+1 ] = pArray->m_pI[ i ];
					}
					pArray->m_pI[ index ] = value;
					pArray->m_iLength++;
				}
				break;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				float value = m_pStack[ --m_iStackPtr ].f;
				int index = pArray->FindFloat( 0, value, 1 );
				if ( index >= (int)pArray->m_iLength ) pArray->m_pF[ pArray->m_iLength++ ] = value;
				else
				{
					for( int i = ((int)pArray->m_iLength)-1; i >= index; i-- )
					{
						pArray->m_pF[ i+1 ] = pArray->m_pF[ i ];
					}
					pArray->m_pF[ index ] = value;
					pArray->m_iLength++;
				}
				break;
			}
			case AGK_DATA_TYPE_STRING:
			{
				RuntimeError( "Invalid instruction for string insert" );
				return 1;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				stType *pType = (stType*) m_pStack[ --m_iStackPtr ].p;
				
				int firstIndex = m_pTypeStructs[ pType->m_iTypeDec ].m_iFirstIndex;
				int offset = m_pTypeStructs[ pType->m_iTypeDec ].m_pVarTypes[ firstIndex ].varOffset;
				int varType = m_pTypeStructs[ pType->m_iTypeDec ].m_pVarTypes[ firstIndex ].varType;
				int index = -1;
				switch( varType )
				{
					case AGK_DATA_TYPE_INT:
					{
						int value = *((int*)(pType->m_pData+offset));
						index = pArray->FindInt( offset, value, 1 );
						break;
					}
					case AGK_DATA_TYPE_FLOAT:
					{
						float value = *((float*)(pType->m_pData+offset));
						index = pArray->FindFloat( offset, value, 1 );
						break;
					}
					case AGK_DATA_TYPE_STRING:
					{
						uString *value = (uString*)(pType->m_pData+offset);
						index = pArray->FindString( offset, value->GetStr(), 1 );
						break;
					}
					case AGK_DATA_TYPE_TYPE:
					{
						RuntimeError( "Cannot insert into array of types as the first variable of the type is another type" );
						return 1;
					}
					case AGK_DATA_TYPE_ARRAY:
					{
						RuntimeError( "Cannot insert into array of types as the first variable of the type is an array" );
						return 1;
					}
				}

				if ( index >= 0 )
				{
					if ( index >= (int)pArray->m_iLength ) 
					{
						CopyType( pType, pArray->m_pT[ pArray->m_iLength++ ] );
					}
					else
					{
						stType *temp = pArray->m_pT[ pArray->m_iLength ];
						CopyType( pType, temp );

						for( int i = ((int)pArray->m_iLength)-1; i >= index; i-- )
						{
							pArray->m_pT[ i+1 ] = pArray->m_pT[ i ];
						}
						pArray->m_pT[ index ] = temp;
						pArray->m_iLength++;
					}
				}
				
				if ( pType->m_iTemp > 0 && pType->m_iTemp == m_iFunctionLevel+1 )
				{
					FreeType( pType );
					delete pType;
					m_pStack[ m_iStackPtr ].p = 0; // clear the stack item in case it is referenced by the debugger
				}
				
				break;
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				RuntimeError( "Arrays of arrays cannot be sorted, use more dimensions to insert into a sub array" );
				return 1;
			}
		}

		m_iStackPtr--;
		break;
	}

	case AGKI_ARRAY_INSERT_SORTED_STRING:
	{
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		uString *pString = &(m_pStrStack[ --m_iStrStackPtr ]);
		
		if ( pArray->m_iLength+1 > pArray->m_iTotalLength )
		{
			UINT newLength = pArray->m_iLength + pArray->m_iLength/2;
			if ( newLength < 10 ) newLength = 10;
			SizeArray( pArray, newLength );
		}

		switch( pArray->GetCurrType() )
		{
			case AGK_DATA_TYPE_INT:
			{
				RuntimeError( "Invalid instruction for inserting into integer array" );
				return 1;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				RuntimeError( "Invalid instruction for inserting into float array" );
				return 1;
			}
			case AGK_DATA_TYPE_STRING:
			{
				int index = pArray->FindString( 0, pString->GetStr(), 1 );
				if ( index >= (int)pArray->m_iLength ) pArray->m_pS[ pArray->m_iLength++ ]->SetStr( pString->GetStr() );
				else
				{
					uString *temp = pArray->m_pS[ pArray->m_iLength ];
					temp->SetStr( pString->GetStr() );
					for( int i = ((int)pArray->m_iLength)-1; i >= index; i-- )
					{
						pArray->m_pS[ i+1 ] = pArray->m_pS[ i ];
					}
					pArray->m_pS[ index ] = temp;
					pArray->m_iLength++;
				}
				break;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				RuntimeError( "Invalid instruction for inserting into type array" );
				return 1;						
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				RuntimeError( "Arrays of arrays cannot be sorted, use more dimensions to insert into a sub array" );
				return 1;
			}
		}
			
		break;
	}

	case AGKI_ARRAY_FIND:
	{
		stArray *pArray = (stArray*) (m_pStack[ m_iStackPtr-2 ].p);
		
		if ( pArray->m_iLength == 0 ) 
		{
			m_pStack[ m_iStackPtr-2 ].i = -1;
			m_iStackPtr--;
		}
		else
		{				
			switch( pArray->GetCurrType() )
			{
				case AGK_DATA_TYPE_INT:
				{
					int value = m_pStack[ --m_iStackPtr ].i;
					m_pStack[ m_iStackPtr-1 ].i = pArray->FindInt( 0, value );
					break;
				}
				case AGK_DATA_TYPE_FLOAT:
				{
					float value = m_pStack[ --m_iStackPtr ].f;
					m_pStack[ m_iStackPtr-1 ].i = pArray->FindFloat( 0, value );
					break;
				}
				case AGK_DATA_TYPE_STRING:
				{
					RuntimeError( "Invalid instruction for searching string array" );
					return 1;
				}
				case AGK_DATA_TYPE_TYPE:
				{
					stTypeDec *pTypeStruct = &(m_pTypeStructs[ pArray->GetTypeType() ]);
					int offset = pTypeStruct->m_pVarTypes[ pTypeStruct->m_iFirstIndex ].varOffset;
					int varType = pTypeStruct->m_pVarTypes[ pTypeStruct->m_iFirstIndex ].varType;
					switch( varType )
					{
						case AGK_DATA_TYPE_INT:
						{
							int value = m_pStack[ --m_iStackPtr ].i;
							m_pStack[ m_iStackPtr-1 ].i = pArray->FindInt( offset, value );
							break;
						}
						case AGK_DATA_TYPE_FLOAT:
						{
							float value = m_pStack[ --m_iStackPtr ].f;
							m_pStack[ m_iStackPtr-1 ].i = pArray->FindFloat( offset, value );
							break;
						}
						case AGK_DATA_TYPE_STRING:
						{
							RuntimeError( "Invalid instruction for searching type array string" );
							return 1;
						}
						case AGK_DATA_TYPE_TYPE:
						{
							RuntimeError( "Cannot search array of types as the first variable of the type is another type" );
							return 1;
						}
						case AGK_DATA_TYPE_ARRAY:
						{
							RuntimeError( "Cannot search array of types as the first variable of the type is an array" );
							return 1;
						}
					}
					break;
				}
				case AGK_DATA_TYPE_ARRAY:
				{
					RuntimeError( "Cannot search array of arrays, use more dimensions to search a sub array" );
					return 1;
				}
			}					
		}
		break;
	}

	case AGKI_ARRAY_INDEXOF:
	{
		stArray *pArray = ( stArray* ) ( m_pStack [ m_iStackPtr - 2 ].p );

		if ( pArray->m_iLength == 0 )
		{
			m_pStack [ m_iStackPtr - 2 ].i = -1;
			m_iStackPtr--;
		}
		else
		{
			switch ( pArray->GetCurrType ( ) )
			{
			case AGK_DATA_TYPE_INT:
			{
				int value = m_pStack [ --m_iStackPtr ].i;
				m_pStack [ m_iStackPtr - 1 ].i = pArray->IndexOfInt ( 0, value );
				break;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				float value = m_pStack [ --m_iStackPtr ].f;
				m_pStack [ m_iStackPtr - 1 ].i = pArray->IndexOfFloat ( 0, value );
				break;
			}
			case AGK_DATA_TYPE_STRING:
			{
				RuntimeError ( "Invalid instruction for searching string array" );
				return 1;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				stTypeDec *pTypeStruct = &( m_pTypeStructs [ pArray->GetTypeType ( ) ] );
				int offset = pTypeStruct->m_pVarTypes [ pTypeStruct->m_iFirstIndex ].varOffset;
				int varType = pTypeStruct->m_pVarTypes [ pTypeStruct->m_iFirstIndex ].varType;
				switch ( varType )
				{
				case AGK_DATA_TYPE_INT:
				{
					int value = m_pStack [ --m_iStackPtr ].i;
					m_pStack [ m_iStackPtr - 1 ].i = pArray->IndexOfInt ( offset, value );
					break;
				}
				case AGK_DATA_TYPE_FLOAT:
				{
					float value = m_pStack [ --m_iStackPtr ].f;
					m_pStack [ m_iStackPtr - 1 ].i = pArray->IndexOfFloat ( offset, value );
					break;
				}
				case AGK_DATA_TYPE_STRING:
				{
					RuntimeError ( "Invalid instruction for searching type array string" );
					return 1;
				}
				case AGK_DATA_TYPE_TYPE:
				{
					RuntimeError ( "Cannot search array of types as the first variable of the type is another type" );
					return 1;
				}
				case AGK_DATA_TYPE_ARRAY:
				{
					RuntimeError ( "Cannot search array of types as the first variable of the type is an array" );
					return 1;
				}
				}
				break;
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				RuntimeError ( "Cannot search array of arrays, use more dimensions to search a sub array" );
				return 1;
			}
			}
		}
		break;
	}

	case AGKI_ARRAY_FIND_STRING:
	{
		stArray *pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		
		if ( pArray->m_iLength == 0 ) 
		{
			m_pStack[ m_iStackPtr++ ].i = -1;
			m_iStrStackPtr--;
		}
		else
		{
			uString *pFind = &(m_pStrStack[ --m_iStrStackPtr ]);
								
			switch( pArray->GetCurrType() )
			{
				case AGK_DATA_TYPE_INT:
				{
					RuntimeError( "Invalid instruction for searching integer array" );
					return 1;
				}
				case AGK_DATA_TYPE_FLOAT:
				{
					RuntimeError( "Invalid instruction for searching float array" );
					return 1;
				}
				case AGK_DATA_TYPE_STRING:
				{
					m_pStack[ m_iStackPtr++ ].i = pArray->FindString( 0, pFind->GetStr() );
					break;
				}
				case AGK_DATA_TYPE_TYPE:
				{
					stTypeDec *pTypeStruct = &(m_pTypeStructs[ pArray->GetTypeType() ]);
					int offset = pTypeStruct->m_pVarTypes[ pTypeStruct->m_iFirstIndex ].varOffset;
					int varType = pTypeStruct->m_pVarTypes[ pTypeStruct->m_iFirstIndex ].varType;
					switch( varType )
					{
						case AGK_DATA_TYPE_INT:
						{
							RuntimeError( "Invalid instruction for searching type array integer" );
							return 1;
						}
						case AGK_DATA_TYPE_FLOAT:
						{
							RuntimeError( "Invalid instruction for searching type array float" );
							return 1;
						}
						case AGK_DATA_TYPE_STRING:
						{
							m_pStack[ m_iStackPtr++ ].i = pArray->FindString( offset, pFind->GetStr() );
							break;
						}
						case AGK_DATA_TYPE_TYPE:
						{
							RuntimeError( "Cannot search array of types as the first variable of the type is another type" );
							return 1;
						}
						case AGK_DATA_TYPE_ARRAY:
						{
							RuntimeError( "Cannot search array of types as the first variable of the type is an array" );
							return 1;
						}
					}
					break;
				}
				case AGK_DATA_TYPE_ARRAY:
				{
					RuntimeError( "Cannot search array of arrays, use more dimensions to search a sub array" );
					return 1;
				}
			}					
		}
		break;
	}

	case AGKI_ARRAY_INDEXOF_STRING:
	{
		stArray *pArray = ( stArray* ) ( m_pStack [ --m_iStackPtr ].p );

		if ( pArray->m_iLength == 0 )
		{
			m_pStack [ m_iStackPtr++ ].i = -1;
			m_iStrStackPtr--;
		}
		else
		{
			uString *pFind = &( m_pStrStack [ --m_iStrStackPtr ] );

			switch ( pArray->GetCurrType ( ) )
			{
			case AGK_DATA_TYPE_INT:
			{
				RuntimeError ( "Invalid instruction for searching integer array" );
				return 1;
			}
			case AGK_DATA_TYPE_FLOAT:
			{
				RuntimeError ( "Invalid instruction for searching float array" );
				return 1;
			}
			case AGK_DATA_TYPE_STRING:
			{
				m_pStack [ m_iStackPtr++ ].i = pArray->IndexOfString ( 0, pFind->GetStr ( ) );
				break;
			}
			case AGK_DATA_TYPE_TYPE:
			{
				stTypeDec *pTypeStruct = &( m_pTypeStructs [ pArray->GetTypeType ( ) ] );
				int offset = pTypeStruct->m_pVarTypes [ pTypeStruct->m_iFirstIndex ].varOffset;
				int varType = pTypeStruct->m_pVarTypes [ pTypeStruct->m_iFirstIndex ].varType;
				switch ( varType )
				{
				case AGK_DATA_TYPE_INT:
				{
					RuntimeError ( "Invalid instruction for searching type array integer" );
					return 1;
				}
				case AGK_DATA_TYPE_FLOAT:
				{
					RuntimeError ( "Invalid instruction for searching type array float" );
					return 1;
				}
				case AGK_DATA_TYPE_STRING:
				{
					m_pStack [ m_iStackPtr++ ].i = pArray->IndexOfString (offset, pFind->GetStr ( ) );
					break;
				}
				case AGK_DATA_TYPE_TYPE:
				{
					RuntimeError ( "Cannot search array of types as the first variable of the type is another type" );
					return 1;
				}
				case AGK_DATA_TYPE_ARRAY:
				{
					RuntimeError ( "Cannot search array of types as the first variable of the type is an array" );
					return 1;
				}
				}
				break;
			}
			case AGK_DATA_TYPE_ARRAY:
			{
				RuntimeError ( "Cannot search array of arrays, use more dimensions to search a sub array" );
				return 1;
			}
			}
		}
		break;
	}

	// copy arrays and types for passing by value
	case AGKI_ARRAY_COPY:
	{
		stArray *pFromArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		stArray *pToArray = (stArray*) (m_pStack[ m_iStackPtr-1 ].p); // don't pop
		CopyArray( pFromArray, pToArray );
		if ( pFromArray->m_iTemp > 0 && pFromArray->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeArray( pFromArray );
			delete pFromArray;
			m_pStack[ m_iStackPtr ].p = 0; // clear the stack item in case it is referenced by the debugger
		}
		break;
	}
	case AGKI_TYPE_COPY:
	{
		stType *pFromType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		stType *pToType = (stType*) (m_pStack[ m_iStackPtr-1 ].p); // don't pop
		CopyType( pFromType, pToType );
		if ( pFromType->m_iTemp > 0 && pFromType->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeType( pFromType );
			delete pFromType;
			m_pStack[ m_iStackPtr ].p = 0; // clear the stack item in case it is referenced by the debugger
		}
		break;
	}

	// cast value
	case AGKI_CAST_FLOAT_INT:
	{
		float a = m_pStack[ m_iStackPtr-1 ].f;
		m_pStack[ m_iStackPtr-1 ].i = agk::Round( a );
		break;
	}
	case AGKI_CAST_INT_FLOAT:
	{
		int a = m_pStack[ m_iStackPtr-1 ].i;
		m_pStack[ m_iStackPtr-1 ].f = (float) a;
		break;
	}

	// inc
	case AGKI_INC_INT:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesInt, "Integer variable index out of bounds" )
		m_pVariablesInt[ index ] += m_pStack[ --m_iStackPtr ].i;
		break;
	}
	case AGKI_INC_FLOAT:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesFloat, "Float variable index out of bounds" )
		m_pVariablesFloat[ index ] += m_pStack[ --m_iStackPtr ].f;
		break;
	}
	case AGKI_INC_LOCAL_INT:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iFramePtr+index ].i += m_pStack[ --m_iStackPtr ].i;
		break;
	}
	case AGKI_INC_LOCAL_FLOAT:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iFramePtr+index ].f += m_pStack[ --m_iStackPtr ].f;
		break;
	}
	case AGKI_INC_ARRAY_INT:
	{
		int value = m_pStack[ --m_iStackPtr ].i;
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		pArray->m_pI[ index ] += value;
		break;
	}
	case AGKI_INC_ARRAY_FLOAT:
	{
		float value = m_pStack[ --m_iStackPtr ].f;
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		pArray->m_pF[ index ] += value;
		break;
	}
	case AGKI_INC_TYPE_INT:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		int value = m_pStack[ --m_iStackPtr ].i;
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		*((int*)(pType->m_pData + index)) += value;
		break;
	}
	case AGKI_INC_TYPE_FLOAT:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		float value = m_pStack[ --m_iStackPtr ].f;
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		*((float*)(pType->m_pData + index)) += value;
		break;
	}

	// dec
	case AGKI_DEC_INT:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesInt, "Integer variable index out of bounds" )
		m_pVariablesInt[ index ] -= m_pStack[ --m_iStackPtr ].i;
		break;
	}
	case AGKI_DEC_FLOAT:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesFloat, "Float variable index out of bounds" )
		m_pVariablesFloat[ index ] -= m_pStack[ --m_iStackPtr ].f;
		break;
	}
	case AGKI_DEC_LOCAL_INT:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iFramePtr+index ].i -= m_pStack[ --m_iStackPtr ].i;
		break;
	}
	case AGKI_DEC_LOCAL_FLOAT:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iFramePtr+index ].f -= m_pStack[ --m_iStackPtr ].f;
		break;
	}
	case AGKI_DEC_ARRAY_INT:
	{
		int value = m_pStack[ --m_iStackPtr ].i;
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		pArray->m_pI[ index ] -= value;
		break;
	}
	case AGKI_DEC_ARRAY_FLOAT:
	{
		float value = m_pStack[ --m_iStackPtr ].f;
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		pArray->m_pF[ index ] -= value;
		break;
	}
	case AGKI_DEC_TYPE_INT:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		int value = m_pStack[ --m_iStackPtr ].i;
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		*((int*)(pType->m_pData + index)) -= value;
		break;
	}
	case AGKI_DEC_TYPE_FLOAT:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		float value = m_pStack[ --m_iStackPtr ].f;
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		*((float*)(pType->m_pData + index)) -= value;
		break;
	}

	// integer operations
	case AGKI_ADD_INT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = a + b;
		break;
	}
	case AGKI_SUB_INT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = b - a;
		break;
	}
	case AGKI_MUL_INT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = a * b;
		break;
	}
	case AGKI_DIV_INT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		if ( a == 0 ) 
		{
			RuntimeError( "Integer division by 0" );
			return 1;
		}
		m_pStack[ m_iStackPtr++ ].i = b / a;
		break;
	}
	case AGKI_MOD_INT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		if ( a == 0 ) 
		{
			RuntimeError( "Division by 0" );
			return 1;
		}
		m_pStack[ m_iStackPtr++ ].i = b % a;
		break;
	}
	case AGKI_POW_INT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = agk::Round(agk::Pow( (float) b, (float) a ));
		break;
	}
	case AGKI_NEG_INT:
	{
		m_pStack[ m_iStackPtr-1 ].i = -(m_pStack[ m_iStackPtr-1 ].i);
		break;
	}

	// bitwise arithmetic
	case AGKI_LSHIFT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = b << a;
		break;
	}
	case AGKI_RSHIFT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		UINT b = (UINT) m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = b >> a;
		break;
	}
	case AGKI_AND:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = b & a;
		break;
	}
	case AGKI_OR:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = b | a;
		break;
	}
	case AGKI_XOR:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = b ^ a;
		break;
	}
	case AGKI_NOT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = ~a;
		break;
	}

	// boolean arithmetic
	case AGKI_BOOL_AND:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = (a && b) ? 1 : 0;
		break;
	}
	case AGKI_BOOL_OR:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		int b = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = (a || b) ? 1 : 0;
		break;
	}
	case AGKI_BOOL_NOT:
	{
		int a = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = a ? 0 : 1;
		break;
	}

	// float operations
	case AGKI_ADD_FLOAT:
	{
		float a = m_pStack[ --m_iStackPtr ].f;
		float b = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].f = a + b;
		break;
	}
	case AGKI_SUB_FLOAT:
	{
		float a = m_pStack[ --m_iStackPtr ].f;
		float b = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].f = b - a;
		break;
	}
	case AGKI_MUL_FLOAT:
	{
		float a = m_pStack[ --m_iStackPtr ].f;
		float b = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].f = a * b;
		break;
	}
	case AGKI_DIV_FLOAT:
	{
		float a = m_pStack[ --m_iStackPtr ].f;
		float b = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].f = b / a;
		break;
	}
	case AGKI_MOD_FLOAT:
	{
		float a = m_pStack[ --m_iStackPtr ].f;
		float b = m_pStack[ --m_iStackPtr ].f;
		if ( a == 0 ) 
		{
			RuntimeError( "Integer division by 0" );
			return 1;
		}
		m_pStack[ m_iStackPtr++ ].f = agk::FMod( b, a );
		break;
	}
	case AGKI_POW_FLOAT:
	{
		float a = m_pStack[ --m_iStackPtr ].f;
		float b = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].f = agk::Pow( b, a );
		break;
	}
	case AGKI_NEG_FLOAT:
	{
		m_pStack[ m_iStackPtr-1 ].f = -(m_pStack[ m_iStackPtr-1 ].f);
		break;
	}

	// string functions
	case AGKI_STRING_CONCAT:
	{
		uString *pSrc = &(m_pStrStack[ --m_iStrStackPtr ]);
		uString *pDest = &(m_pStrStack[ m_iStrStackPtr-1 ]); // don't pop
		pDest->Append( pSrc->GetStr() );
		break;
	}
	
	// global variable assignment
	case AGKI_STORE_INT:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesInt, "Integer variable index out of bounds" )

		m_pVariablesInt[ index ] = m_pStack[ --m_iStackPtr ].i;
		break;
	}
	case AGKI_STORE_FLOAT:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesFloat, "Float variable index out of bounds" )

		m_pVariablesFloat[ index ] = m_pStack[ --m_iStackPtr ].f;
		break;
	}
	case AGKI_STORE_STRING:
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumVariablesString, "String variable index out of bounds" )

		m_pVariablesString[ index ].SetStr( m_pStrStack[ --m_iStrStackPtr ].GetStr() );
		break;
	}
	case AGKI_STORE_TYPE:
	{
		stType *pTypeValue = (stType*) (m_pStack[ --m_iStackPtr ].p);
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		
		CopyType( pTypeValue, pType );
		if ( pTypeValue->m_iTemp > 0 && pTypeValue->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeType( pTypeValue );
			delete pTypeValue;
			m_pStack[ m_iStackPtr+1 ].p = 0; // clear the stack item in case it is referenced by the debugger
		}
		break;
	}
	case AGKI_STORE_ARRAY:
	{
		stArray *pArrayValue = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		
		CopyArray( pArrayValue, pArray );
		if ( pArrayValue->m_iTemp > 0 && pArrayValue->m_iTemp == m_iFunctionLevel+1 )
		{
			FreeArray( pArrayValue );
			delete pArrayValue;
			m_pStack[ m_iStackPtr+1 ].p = 0; // clear the stack item in case it is referenced by the debugger
		}
		break;
	}

	// local variable assignment
	case AGKI_STORE_LOCAL_INT:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iFramePtr+index ].i = m_pStack[ --m_iStackPtr ].i;
		break;
	}
	case AGKI_STORE_LOCAL_FLOAT:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStack[ m_iFramePtr+index ].f = m_pStack[ --m_iStackPtr ].f;
		break;
	}
	case AGKI_STORE_LOCAL_STRING:
	{
		int index = m_pInstructions[ instruction ].i;
		m_pStrStack[ m_iStrFramePtr+index ].SetStr( m_pStrStack[ --m_iStrStackPtr ].GetStr() );
		break;
	}

	// array assignment, array pointer must be on the stack
	case AGKI_STORE_ARRAY_INT:
	{
		int value = m_pStack[ --m_iStackPtr ].i;
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		pArray->m_pI[ index ] = value;
		break;
	}
	case AGKI_STORE_ARRAY_FLOAT:
	{
		float value = m_pStack[ --m_iStackPtr ].f;
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		pArray->m_pF[ index ] = value;
		break;
	}
	case AGKI_STORE_ARRAY_STRING:
	{
		UINT index = m_pStack[ --m_iStackPtr ].i;
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		CHECKARRAYRANGE( index, pArray->m_iLength )

		pArray->m_pS[ index ]->SetStr( m_pStrStack[ --m_iStrStackPtr ].GetStr() );
		break;
	}

	// type assignment
	case AGKI_STORE_TYPE_INT:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		int value = m_pStack[ --m_iStackPtr ].i;
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		*((int*)(pType->m_pData + index)) = value;
		break;
	}
	case AGKI_STORE_TYPE_FLOAT:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		float value = m_pStack[ --m_iStackPtr ].f;
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		*((float*)(pType->m_pData + index)) = value;
		break;
	}
	case AGKI_STORE_TYPE_STRING:
	{
		UINT index = m_pInstructions[ instruction ].i; // byte offset into type data
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		CHECKRANGE( index, 0, pType->m_iVarSize-4, "Type offset out of bounds" )

		((uString*)(pType->m_pData + index))->SetStr( m_pStrStack[ --m_iStrStackPtr ].GetStr() );
		break;
	}

	// load/save arrays
	case AGKI_ARRAY_LOAD_FROM_FILE:
	{
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		JSONElement *json = JSONElement::LoadJSONFromFile( m_pStrStack[ --m_iStrStackPtr ] );
		if ( json ) ArrayFromJSON( pArray, json );
		else FreeArray( pArray );
		delete json;
		break;
	}
	case AGKI_ARRAY_SAVE_TO_FILE:
	{
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		m_iStrStackPtr--;
		AGK::cFile fileOut;
		if ( fileOut.OpenToWrite( m_pStrStack[ m_iStrStackPtr ] ) )
		{
			uString sJson;
			ArrayToJSON( pArray, sJson );
			fileOut.WriteData( sJson, sJson.GetLength() );
		}
		break;
	}
	case AGKI_ARRAY_FROM_JSON:
	{
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		JSONElement *json = JSONElement::LoadJSONFromData( m_pStrStack[ --m_iStrStackPtr ] );
		if ( json ) ArrayFromJSON( pArray, json );
		else FreeArray( pArray );
		delete json;
		break;
	}
	case AGKI_ARRAY_TO_JSON:
	{
		stArray* pArray = (stArray*) (m_pStack[ --m_iStackPtr ].p);
		m_pStrStack[ m_iStrStackPtr ].ClearTemp();
		ArrayToJSON( pArray, m_pStrStack[ m_iStrStackPtr ] );
		m_iStrStackPtr++;
		break;
	}

	case AGKI_TYPE_FROM_JSON:
	{
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		JSONElement *json = JSONElement::LoadJSONFromData( m_pStrStack[ --m_iStrStackPtr ] );
		if ( json ) TypeFromJSON( pType, json );
		else ZeroType( pType );
		delete json;
		break;
	}
	case AGKI_TYPE_TO_JSON:
	{
		stType* pType = (stType*) (m_pStack[ --m_iStackPtr ].p);
		m_pStrStack[ m_iStrStackPtr ].ClearTemp();
		TypeToJSON( pType, m_pStrStack[ m_iStrStackPtr ] );
		m_iStrStackPtr++;
		break;
	}

	case AGKI_DUP:
	{
		m_pStack[ m_iStackPtr ] = m_pStack[ m_iStackPtr-1 ];
		++m_iStackPtr;
		break;
	}

	// comparison int
	case AGKI_CMP_LESS_INT:
	{
		int b = m_pStack[ --m_iStackPtr ].i;
		int a = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = (a < b);
		break;
	}
	case AGKI_CMP_LESS_EQUAL_INT:
	{
		int b = m_pStack[ --m_iStackPtr ].i;
		int a = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = (a <= b);
		break;
	}
	case AGKI_CMP_GREATER_INT:
	{
		int b = m_pStack[ --m_iStackPtr ].i;
		int a = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = (a > b);
		break;
	}
	case AGKI_CMP_GREATER_EQUAL_INT:
	{
		int b = m_pStack[ --m_iStackPtr ].i;
		int a = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = (a >= b);
		break;
	}
	case AGKI_CMP_EQUAL_INT:
	{
		int b = m_pStack[ --m_iStackPtr ].i;
		int a = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = (a == b);
		break;
	}
	case AGKI_CMP_NOT_EQUAL_INT:
	{
		int b = m_pStack[ --m_iStackPtr ].i;
		int a = m_pStack[ --m_iStackPtr ].i;
		m_pStack[ m_iStackPtr++ ].i = (a != b);
		break;
	}

	// comparison float
	case AGKI_CMP_LESS_FLOAT:
	{
		float b = m_pStack[ --m_iStackPtr ].f;
		float a = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].i = (a < b);
		break;
	}
	case AGKI_CMP_LESS_EQUAL_FLOAT:
	{
		float b = m_pStack[ --m_iStackPtr ].f;
		float a = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].i = (a <= b);
		break;
	}
	case AGKI_CMP_GREATER_FLOAT:
	{
		float b = m_pStack[ --m_iStackPtr ].f;
		float a = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].i = (a > b);
		break;
	}
	case AGKI_CMP_GREATER_EQUAL_FLOAT:
	{
		float b = m_pStack[ --m_iStackPtr ].f;
		float a = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].i = (a >= b);
		break;
	}
	case AGKI_CMP_EQUAL_FLOAT:
	{
		float b = m_pStack[ --m_iStackPtr ].f;
		float a = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].i = (a == b);
		break;
	}
	case AGKI_CMP_NOT_EQUAL_FLOAT:
	{
		float b = m_pStack[ --m_iStackPtr ].f;
		float a = m_pStack[ --m_iStackPtr ].f;
		m_pStack[ m_iStackPtr++ ].i = (a != b);
		break;
	}

	// string comparison
	case AGKI_CMP_LESS_STRING:
	{
		uString *b = &(m_pStrStack[ --m_iStrStackPtr ]);
		uString *a = &(m_pStrStack[ --m_iStrStackPtr ]);
		m_pStack[ m_iStackPtr++ ].i = (a->CompareTo(b->GetStr()) < 0) ? 1 : 0;
		break;
	}
	case AGKI_CMP_LESS_EQUAL_STRING:
	{
		uString *b = &(m_pStrStack[ --m_iStrStackPtr ]);
		uString *a = &(m_pStrStack[ --m_iStrStackPtr ]);
		m_pStack[ m_iStackPtr++ ].i = (a->CompareTo(b->GetStr()) <= 0) ? 1 : 0;
		break;
	}
	case AGKI_CMP_GREATER_STRING:
	{
		uString *b = &(m_pStrStack[ --m_iStrStackPtr ]);
		uString *a = &(m_pStrStack[ --m_iStrStackPtr ]);
		m_pStack[ m_iStackPtr++ ].i = (a->CompareTo(b->GetStr()) > 0) ? 1 : 0;
		break;
	}
	case AGKI_CMP_GREATER_EQUAL_STRING:
	{
		uString *b = &(m_pStrStack[ --m_iStrStackPtr ]);
		uString *a = &(m_pStrStack[ --m_iStrStackPtr ]);
		m_pStack[ m_iStackPtr++ ].i = (a->CompareTo(b->GetStr()) >= 0) ? 1 : 0;
		break;
	}
	case AGKI_CMP_EQUAL_STRING:
	{
		uString *b = &(m_pStrStack[ --m_iStrStackPtr ]);
		uString *a = &(m_pStrStack[ --m_iStrStackPtr ]);
		m_pStack[ m_iStackPtr++ ].i = (a->CompareTo(b->GetStr()) == 0) ? 1 : 0;
		break;
	}
	case AGKI_CMP_NOT_EQUAL_STRING:
	{
		uString *b = &(m_pStrStack[ --m_iStrStackPtr ]);
		uString *a = &(m_pStrStack[ --m_iStrStackPtr ]);
		m_pStack[ m_iStackPtr++ ].i = (a->CompareTo(b->GetStr()) != 0) ? 1 : 0;
		break;
	}

	// jump
	case AGKI_JUMP: // unconditional
	{
		int index = m_pInstructions[ instruction ].i;
		m_iProgramCounter = index;
		break;
	}
	case AGKI_JUMP_COND:
	{
		if ( m_pStack[ --m_iStackPtr ].i != 0 )
		{
			m_iProgramCounter = m_pInstructions[ instruction ].i;
		}
		break;
	}
	case AGKI_JUMP_NOT_COND:
	{
		if ( m_pStack[ --m_iStackPtr ].i == 0 )
		{
			m_iProgramCounter = m_pInstructions[ instruction ].i;
		}
		break;
	}
	case AGKI_JUMP_LABEL: // jump to label
	{
		UINT index = m_pInstructions[ instruction ].i;
		CHECKRANGE( index, 0, m_iNumLabels, "Label index out of bounds, probably a parser bug" );

		m_iProgramCounter = m_pLabels[ index ];
		break;
	}
	case AGKI_JUMP_CASE_INT: // compare and jump over case block
	{
		// can have multiple case values, compare them all and if any match don't jump
		bool jump = true;
		int count = m_pStack[ --m_iStackPtr ].i;
		int a = m_pStack[ m_iStackPtr-count-1 ].i; // don't pop
		for ( int i = 0; i < count; i++ )
		{
			int b = m_pStack[ --m_iStackPtr ].i;
			if (a == b) jump = false;
		}

		if ( jump )
		{
			m_iProgramCounter = m_pInstructions[ instruction ].i;
		}

		break;
	}
	case AGKI_JUMP_CASE_FLOAT: // compare and jump over case block
	{
		// can have multiple case values, compare them all and if any match don't jump
		bool jump = true;
		int count = m_pStack[ --m_iStackPtr ].i;
		float a = m_pStack[ m_iStackPtr-count-1 ].f; // don't pop
		for ( int i = 0; i < count; i++ )
		{
			float b = m_pStack[ --m_iStackPtr ].f;
			if (a == b) jump = false;
		}

		if ( jump )
		{
			m_iProgramCounter = m_pInstructions[ instruction ].i;
		}

		break;
	}
	case AGKI_JUMP_CASE_STRING: // compare and jump over case block
	{
		// can have multiple case values, compare them all and if any match don't jump
		bool jump = true;
		int count = m_pStack[ --m_iStackPtr ].i;
		uString* a = m_pStrStack + (m_iStrStackPtr-count-1); // don't pop
		for ( int i = 0; i < count; i++ )
		{
			if ( m_pStrStack[ --m_iStrStackPtr ].CompareTo( a->GetStr() ) == 0 ) jump = false;
		}

		if ( jump )
		{
			m_iProgramCounter = m_pInstructions[ instruction ].i;
		}
		break;
	}

	// code modification
	case AGKI_EDIT_COND:
	{
		// if the condition is met replace the given instruction index
		if( m_pStack[ --m_iStackPtr ].i != 0 )
		{
			int index = m_pInstructions[ instruction ].i;
			int newInstruction = m_pInstructions[ instruction ].i2;

			m_pInstructions[ index ].iInstruction = (AGKInstruction) newInstruction;
		}
		break;
	}
	case AGKI_SET_INSTRUCTION:
	{
		// replace the given instruction index
		int index = m_pInstructions[ instruction ].i;
		int newInstruction = m_pInstructions[ instruction ].i2;

		m_pInstructions[ index ].iInstruction = (AGKInstruction) newInstruction;
		break;
	}

	case AGKI_REAL_SYNC:
	{
		// if we get here assume the user knows to sync regularly from now on, so instruction counter should be set high
		m_iInstructionCounter = 10000000;
		agk::Sync();
		return 0;
	}

	case AGKI_REAL_SWAP:
	{
		// if we get here assume the user knows to sync regularly from now on, so instruction counter should be set high
		m_iInstructionCounter = 10000000;
		agk::Swap();
		return 0;
	}

	case AGKI_PLUGIN_COMMAND:
	{
		int pluginIndex = m_pInstructions[ instruction ].i;
		int commandIndex = m_pInstructions[ instruction ].i2;

		stPluginCommand *pCommand = m_pPlugins[ pluginIndex ].m_pCommands + commandIndex;

		int numParams = pCommand->iNumParams;
		int returnType = pCommand->iReturnType;
		void(*pFunc)(void) = pCommand->pFunc;

		union parameter
		{
			int i;
			float f;
			const char* s;
		};

#ifdef AGK_WINDOWS
	#ifdef _WIN64
		int pRegisterParamTypes[ 4 ];
		parameter pRegisterParams[ 4 ];
		parameter *pStackParams = new parameter[ pCommand->iNumParams ];

		// reverse parameters
		for( int i = pCommand->iNumParams-1; i >= 0; i-- )
		{
			if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_INTEGER )
			{
				pStackParams[ i ].i = m_pStack[ --m_iStackPtr ].i;
			} 
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_FLOAT )
			{
				pStackParams[ i ].f = m_pStack[ --m_iStackPtr ].f;
			}
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_STRING )
			{
				pStackParams[ i ].s = m_pStrStack[ --m_iStrStackPtr ].GetStr();
			}
		}

		// assign parameters to float registers, non-float registers, and stack variables
		int stackCount = 0;
		int registerCount = 0;
		for( int i = 0; i < pCommand->iNumParams; i++ )
		{
			if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_INTEGER )
			{
				if ( registerCount < 4 )
				{
					pRegisterParams[ registerCount ].i = pStackParams[ i ].i;
					pRegisterParamTypes[ registerCount ] = 0;
					registerCount++;
				}
				else
				{
					pStackParams[ stackCount ].i = pStackParams[ i ].i;
					stackCount++;
				}
			} 
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_FLOAT )
			{
				if ( registerCount < 4 )
				{
					pRegisterParams[ registerCount ].f = pStackParams[ i ].f;
					pRegisterParamTypes[ registerCount ] = 1;
					registerCount++;
				}
				else
				{
					pStackParams[ stackCount ].f = pStackParams[ i ].f;
					stackCount++;
				}
			}
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_STRING )
			{
				if ( registerCount < 4 )
				{
					pRegisterParams[ registerCount ].s = pStackParams[ i ].s;
					pRegisterParamTypes[ registerCount ] = 0;
					registerCount++;
				}
				else
				{
					pStackParams[ stackCount ].s = pStackParams[ i ].s;
					stackCount++;
				}
			}
		}

		parameter result;
		result.s = (const char*) win64_plugin_call( registerCount, pRegisterParams, pRegisterParamTypes, stackCount, pStackParams, returnType, pFunc );
			
		switch( pCommand->iReturnType )
		{
			case AGK_VARIABLE_INTEGER: m_pStack[ m_iStackPtr++ ].i = result.i; break;
			case AGK_VARIABLE_FLOAT: m_pStack[ m_iStackPtr++ ].f = result.f; break;
			case AGK_VARIABLE_STRING: 
			{
				m_pStrStack[ m_iStrStackPtr++ ].SetStr( result.s ); 
				delete [] result.s;
				break;
			}
		}

		delete [] pStackParams;
	#else	
		int returnInt = 0;
		float returnFloat = 0;
		char* returnStr = 0;
		
		parameter *pParams = new parameter[ pCommand->iNumParams ];

		for( int i = pCommand->iNumParams-1; i >= 0; i-- )
		{
			if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_INTEGER )
			{
				pParams[ i ].i = m_pStack[ --m_iStackPtr ].i;
			} 
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_FLOAT )
			{
				pParams[ i ].f = m_pStack[ --m_iStackPtr ].f;
			}
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_STRING )
			{
				pParams[ i ].s = m_pStrStack[ --m_iStrStackPtr ].GetStr();
			}
		}

		__asm
		{
			mov eax, numParams
			mov ebx, esp
nextloop:
			// dec loop counter
			sub eax, 1
			cmp eax, 0
			jl endLoop

			// push parameter onto stack
			mov ecx, pParams
			push [ecx+eax*4]
			jmp nextloop
endloop:

			// call function
			call pFunc
			
			mov ecx, returnType
			cmp ecx, AGK_VARIABLE_INTEGER
			jne floatCmp
			// return int
			mov returnInt, eax
			jmp end
floatCmp:
			cmp ecx, AGK_VARIABLE_FLOAT
			jne stringCmp
			// return float
			fstp returnFloat
			jmp end
stringCmp:
			cmp ecx, AGK_VARIABLE_STRING
			jne end
			// return string
			mov returnStr, eax
end:
			// remove variables from stack
			mov esp,ebx
		}

		switch( pCommand->iReturnType )
		{
			case AGK_VARIABLE_INTEGER: m_pStack[ m_iStackPtr++ ].i = returnInt; break;
			case AGK_VARIABLE_FLOAT: m_pStack[ m_iStackPtr++ ].f = returnFloat; break;
			case AGK_VARIABLE_STRING: 
			{
				m_pStrStack[ m_iStrStackPtr++ ].SetStr( returnStr ); 
				delete [] returnStr;
				break;
			}
		}

		delete [] pParams;
	#endif
#endif

#if defined(AGK_LINUX) || defined(AGK_MACOS)
	#ifndef __x86_64__
		RuntimeError( "Plugin commands are not supported on 32bit Linux" );
	#else
		float *pFloatParams = new float[ pCommand->iNumParams ];
		void **pNonFloatParams = new void*[ pCommand->iNumParams ];
		parameter *pStackParams = new parameter[ pCommand->iNumParams ];
		
		// reverse parameters
		for( int i = pCommand->iNumParams-1; i >= 0; i-- )
		{
			if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_INTEGER )
			{
				pStackParams[ i ].i = m_pStack[ --m_iStackPtr ].i;
			} 
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_FLOAT )
			{
				pStackParams[ i ].f = m_pStack[ --m_iStackPtr ].f;
			}
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_STRING )
			{
				pStackParams[ i ].s = m_pStrStack[ --m_iStrStackPtr ].GetStr();
			}
		}

		// assign parameters to float registers, non-float registers, and stack variables
		int floatCount = 0;
		int nonFloatCount = 0;
		int stackCount = 0;
		for( int i = 0; i < pCommand->iNumParams; i++ )
		{
			if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_INTEGER )
			{
				if ( nonFloatCount < 6 )
				{
					pNonFloatParams[ nonFloatCount ] = (void*) pStackParams[ i ].s;
					nonFloatCount++;
				}
				else
				{
					pStackParams[ stackCount ].i = pStackParams[ i ].i;
					stackCount++;
				}
			} 
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_FLOAT )
			{
				if ( floatCount < 8 )
				{
					pFloatParams[ floatCount ] = pStackParams[ i ].f;
					floatCount++;
				}
				else
				{
					pStackParams[ stackCount ].f = pStackParams[ i ].f;
					stackCount++;
				}
			}
			else if ( pCommand->iParamsTypes[i] == AGK_VARIABLE_STRING )
			{
				if ( nonFloatCount < 6 )
				{
					pNonFloatParams[ nonFloatCount ] = (void*) pStackParams[ i ].s;
					nonFloatCount++;
				}
				else
				{
					pStackParams[ stackCount ].s = pStackParams[ i ].s;
					stackCount++;
				}
			}
		}
		
		parameter result; result.s = 0;

		asm( "movq %[func], %%r11 \n\t" // -O2 puts [func] in rsp which is destroyed 
										// when parameters are pushed to the stack
									
			 // float param 1
			 "movl %[numF], %%eax \n\t"
			 "cmp $0, %%eax \n\t"
			 "je 1f \n\t"
			 "movq %[params], %%rbx \n\t"
			 "movss (%%rbx), %%xmm0 \n\t"
			 
			 // float param 2
			 "sub $1, %%eax \n\t"
			 "cmp $0, %%eax \n\t"
			 "je 1f \n\t"
	 		 "movss 4(%%rbx), %%xmm1 \n\t"
	 		 
	 		 // float param 3
	 		 "sub $1, %%eax \n\t"
			 "cmp $0, %%eax \n\t"
			 "je 1f \n\t"
	 		 "movss 8(%%rbx), %%xmm2 \n\t"
	 		 
	 		 // float param 4
	 		 "sub $1, %%eax \n\t"
			 "cmp $0, %%eax \n\t"
			 "je 1f \n\t"
	 		 "movss 12(%%rbx), %%xmm3 \n\t"
	 		 
	 		 // float param 5
	 		 "sub $1, %%eax \n\t"
			 "cmp $0, %%eax \n\t"
			 "je 1f \n\t"
	 		 "movss 16(%%rbx), %%xmm4 \n\t"
	 		 
	 		 // float param 6
	 		 "sub $1, %%eax \n\t"
			 "cmp $0, %%eax \n\t"
			 "je 1f \n\t"
	 		 "movss 20(%%rbx), %%xmm5 \n\t"
	 		 
	 		 // float param 7
	 		 "sub $1, %%eax \n\t"
			 "cmp $0, %%eax \n\t"
			 "je 1f \n\t"
	 		 "movss 24(%%rbx), %%xmm6 \n\t"
	 		 
	 		 // float param 8
	 		 "sub $1, %%eax \n\t"
			 "cmp $0, %%eax \n\t"
			 "je 1f \n\t"
	 		 "movss 28(%%rbx), %%xmm7 \n\t"
	 		 
	 		 // int param 1
	 		 "1:\t movl %[numI], %%eax \n\t"
	 		 "cmp $0, %%eax \n\t"
			 "je 2f \n\t"
			 "movq %[params2], %%rbx \n\t"
			 "mov (%%rbx), %%rdi \n\t"
			 
			 // int param 2
	 		 "sub $1, %%eax \n\t"
	 		 "cmp $0, %%eax \n\t"
			 "je 2f \n\t"
			 "mov 8(%%rbx), %%rsi \n\t"
			 
			 // int param 3
	 		 "sub $1, %%eax \n\t"
	 		 "cmp $0, %%eax \n\t"
			 "je 2f \n\t"
			 "mov 16(%%rbx), %%rdx \n\t"
			 
			 // int param 4
	 		 "sub $1, %%eax \n\t"
	 		 "cmp $0, %%eax \n\t"
			 "je 2f \n\t"
			 "mov 24(%%rbx), %%rcx \n\t"
			 
			 // int param 5
	 		 "sub $1, %%eax \n\t"
	 		 "cmp $0, %%eax \n\t"
			 "je 2f \n\t"
			 "mov 32(%%rbx), %%r8 \n\t"
			 
			 // int param 6
	 		 "sub $1, %%eax \n\t"
	 		 "cmp $0, %%eax \n\t"
			 "je 2f \n\t"
			 "mov 40(%%rbx), %%r9 \n\t"
			 
			 // stack params
			 "2:\t movq %%rsp, %%rbx \n\t"
			 "movslq %[numS], %%rax \n\t"
			 "movq %[params3], %%r10 \n\t"
			 
			 // stack param loop
			 "3:\t sub $1, %%rax \n\t"
			 "cmp $0, %%rax \n\t"
			 "jl 4f \n\t" // end loop
			 
			 "pushq (%%r10,%%rax,8) \n\t"
			 "jmp 3b \n\t"
			 
			 // call function
			 "4:\t call *%%r11 \n\t"
			 
			 // clean stack
			 "movq %%rbx, %%rsp \n\t"
	 		 
	 		 // handle return type
			 "movl %[rt], %%ecx \n\t"
			 "cmp $2, %%ecx \n\t"
			 "je 5f \n\t"
			 "movq %%rax, %[result] \n\t"
			 "jmp 6f \n\t"
			 
			 "5:\t movd %%xmm0, %[result] \n\t" 
			 "6:\t \n\t"
			: [result] "=rm" (result) 
			: [func] "rm" (pFunc), [rt] "rm" (returnType),
			  [params] "rm" (pFloatParams), [numF] "rm" (floatCount),
			  [params2] "rm" (pNonFloatParams), [numI] "rm" (nonFloatCount),
			  [params3] "rm" (pStackParams), [numS] "rm" (stackCount)
			: "memory", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", 
				"rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "rbx", "r10", "r11" );
				
		switch( pCommand->iReturnType )
		{
			case AGK_VARIABLE_INTEGER: m_pStack[ m_iStackPtr++ ].i = result.i; break;
			case AGK_VARIABLE_FLOAT: m_pStack[ m_iStackPtr++ ].f = result.f; break;
			case AGK_VARIABLE_STRING: 
			{
				m_pStrStack[ m_iStrStackPtr++ ].SetStr( result.s ); 
				delete [] result.s;
				break;
			}
		}

		delete [] pStackParams;
		delete [] pNonFloatParams;
		delete [] pFloatParams;
	#endif
#endif

#ifdef AGK_ANDROID
		RuntimeError( "Plugin commands are not yet supported on this platform" );
#endif

#ifdef AGK_IOS
		RuntimeError( "Plugin commands are not yet supported on this platform" );
#endif

		break;
	}

	// special cases
	case AGKI_UNMATCHED_CONTINUE:
	{
		uString err;
		err.Format( "Reached \"continue\" without being in a loop" );
		RuntimeError( err );
		return 1;
	}

	case AGKI_UNMATCHED_EXIT:
	{
		uString err;
		err.Format( "Reached \"exit\" without being in a loop" );
		RuntimeError( err );
		return 1;
	}

	case AGKI_UNMATCHED_FUNC_EXIT:
	{
		uString err;
		err.Format( "Reached \"exitfunction\" without being in a function" );
		RuntimeError( err );
		return 1;
	}

	case AGKI_ILLEGAL_INSTRUCTION:
	{
		uString err;
		err.Format( "Reached illegal instruction" );
		RuntimeError( err );
		return 1;
	}

	case AGKI_END: 
	{
		//App.AppFinished();
		return 3;
	}
