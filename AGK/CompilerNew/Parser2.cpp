#include "Parser2.h"
#include "AGKCompiler2.h"
#include "../apps/interpreter/AGKInstructions.h"

#define AGK_OPERATOR_ADD	1
#define AGK_OPERATOR_SUB	2
#define AGK_OPERATOR_MUL	3
#define AGK_OPERATOR_DIV	4
//#define AGK_OPERATOR_MOD	6
#define AGK_OPERATOR_POW	7
#define AGK_OPERATOR_AND	8
#define AGK_OPERATOR_OR		9
#define AGK_OPERATOR_XOR	10
#define AGK_OPERATOR_BOOL_AND	11
#define AGK_OPERATOR_BOOL_OR	12
#define AGK_OPERATOR_BOOL_NOT	13
#define AGK_OPERATOR_CMP_EQUAL		14
#define AGK_OPERATOR_CMP_NEQUAL		15
#define AGK_OPERATOR_CMP_GREATER	16
#define AGK_OPERATOR_CMP_EGREATER	17
#define AGK_OPERATOR_CMP_LESS		18
#define AGK_OPERATOR_CMP_ELESS		19
#define AGK_OPERATOR_LSHIFT		20
#define AGK_OPERATOR_RSHIFT		21
#define AGK_OPERATOR_NEG		22
#define AGK_OPERATOR_NOT		23

using namespace AGK_Compiler;

extern int error_count;

const char* Parser2::operatorName( int op )
{
	switch( op )
	{
		case AGK_OPERATOR_NEG: return "Negation";
		case AGK_OPERATOR_NOT: return "Bitwise NOT";
		case AGK_OPERATOR_POW: return "Power";
		case AGK_OPERATOR_DIV: return "Division";
//		case AGK_OPERATOR_MOD: return "Remainder";
		case AGK_OPERATOR_MUL: return "Multiplication";
		case AGK_OPERATOR_ADD: return "Addition";
		case AGK_OPERATOR_SUB: return "Subtraction";
		case AGK_OPERATOR_LSHIFT: return "Left Shift";
		case AGK_OPERATOR_RSHIFT: return "Right Shift";
		case AGK_OPERATOR_AND: return "Bitwise AND";
		case AGK_OPERATOR_XOR: return "Bitwise XOR";
		case AGK_OPERATOR_OR: return "Bitwise OR";
		case AGK_OPERATOR_CMP_EQUAL: return "Equals";
		case AGK_OPERATOR_CMP_NEQUAL: return "Not Equals";
		case AGK_OPERATOR_CMP_GREATER: return "Greater Than";
		case AGK_OPERATOR_CMP_EGREATER: return "Greater Or Equal";
		case AGK_OPERATOR_CMP_LESS: return "Less Than";
		case AGK_OPERATOR_CMP_ELESS: return "Less Or Equal";
		case AGK_OPERATOR_BOOL_NOT: return "Boolean NOT";
		case AGK_OPERATOR_BOOL_AND: return "Boolean AND";
		case AGK_OPERATOR_BOOL_OR: return "Boolean OR";
		default: Error( "Unrecognised operator", 0,0 ); return "";
	}
}

int Parser2::operatorPrecedence( int op )
{
	switch( op )
	{
		case AGK_OPERATOR_NEG: // unary minus
		case AGK_OPERATOR_NOT: return 110; // bitwise not
		case AGK_OPERATOR_POW: return 100;
		case AGK_OPERATOR_DIV: 
//		case AGK_OPERATOR_MOD: 
		case AGK_OPERATOR_MUL: return 90;
		case AGK_OPERATOR_ADD: 
		case AGK_OPERATOR_SUB: return 80; 
		case AGK_OPERATOR_LSHIFT: 
		case AGK_OPERATOR_RSHIFT: return 70;
		case AGK_OPERATOR_AND: return 62;
		case AGK_OPERATOR_XOR: return 61;
		case AGK_OPERATOR_OR: return 60; 
		case AGK_OPERATOR_CMP_EQUAL:
		case AGK_OPERATOR_CMP_NEQUAL:
		case AGK_OPERATOR_CMP_GREATER:
		case AGK_OPERATOR_CMP_EGREATER:
		case AGK_OPERATOR_CMP_LESS:
		case AGK_OPERATOR_CMP_ELESS: return 50;
		case AGK_OPERATOR_BOOL_NOT: return 42;
		case AGK_OPERATOR_BOOL_AND: return 41;
		case AGK_OPERATOR_BOOL_OR: return 40;
		default: Error( "Unrecognised operator", 0,0 ); return 0;
	}

	return 0;
}

int Parser2::operatorLeftAssoc( int op )
{
	if ( op == AGK_OPERATOR_NEG
	  || op == AGK_OPERATOR_NOT
	  || op == AGK_OPERATOR_BOOL_NOT ) return 0;
	else return 1;
}

int Parser2::convertUnaryOperator( const char* szOp )
{
	if ( strcmp( szOp, "!" ) == 0 ) return AGK_OPERATOR_NOT;
	else if ( strcmp( szOp, "-" ) == 0 ) return AGK_OPERATOR_NEG;
	else if ( strcmp( szOp, "not" ) == 0 ) return AGK_OPERATOR_BOOL_NOT;
	else Error( "Unrecognised unary operator", 0, szOp );

	return 0;
}

int Parser2::convertBinaryOperator( const char* szOp )
{
	if ( strcmp( szOp, "+" ) == 0 ) return AGK_OPERATOR_ADD;
	else if ( strcmp( szOp, "-" ) == 0 ) return AGK_OPERATOR_SUB;
	else if ( strcmp( szOp, "*" ) == 0 ) return AGK_OPERATOR_MUL;
	else if ( strcmp( szOp, "/" ) == 0 ) return AGK_OPERATOR_DIV;
//	else if ( strcmp( szOp, "mod" ) == 0 ) return AGK_OPERATOR_MOD;
	else if ( strcmp( szOp, "^" ) == 0 ) return AGK_OPERATOR_POW;
	else if ( strcmp( szOp, "&&" ) == 0 ) return AGK_OPERATOR_AND;
	else if ( strcmp( szOp, "||" ) == 0 ) return AGK_OPERATOR_OR;
	else if ( strcmp( szOp, "~~" ) == 0 ) return AGK_OPERATOR_XOR;
	else if ( strcmp( szOp, "and" ) == 0 ) return AGK_OPERATOR_BOOL_AND;
	else if ( strcmp( szOp, "or" ) == 0 ) return AGK_OPERATOR_BOOL_OR;
	else if ( strcmp( szOp, "<<" ) == 0 ) return AGK_OPERATOR_LSHIFT;
	else if ( strcmp( szOp, ">>" ) == 0 ) return AGK_OPERATOR_RSHIFT;
	else if ( strcmp( szOp, "=>" ) == 0 ) return AGK_OPERATOR_CMP_EGREATER;
	else if ( strcmp( szOp, "=<" ) == 0 ) return AGK_OPERATOR_CMP_ELESS;
	else if ( strcmp( szOp, "=" ) == 0 ) return AGK_OPERATOR_CMP_EQUAL;
	else if ( strcmp( szOp, "<>" ) == 0 ) return AGK_OPERATOR_CMP_NEQUAL;
	else if ( strcmp( szOp, ">=" ) == 0 ) return AGK_OPERATOR_CMP_EGREATER;
	else if ( strcmp( szOp, "<=" ) == 0 ) return AGK_OPERATOR_CMP_ELESS;
	else if ( strcmp( szOp, ">" ) == 0 ) return AGK_OPERATOR_CMP_GREATER;
	else if ( strcmp( szOp, "<" ) == 0 ) return AGK_OPERATOR_CMP_LESS;
	else Error( "Unrecognised binary operator", 0, szOp );

	return 0;
}

int Parser2::operatorDataType( int op, int dataType )
{
	switch( op )
	{
		case AGK_OPERATOR_NOT: 
		case AGK_OPERATOR_LSHIFT: 
		case AGK_OPERATOR_RSHIFT: 
		case AGK_OPERATOR_AND: 
		case AGK_OPERATOR_XOR: 
		case AGK_OPERATOR_OR: 
		case AGK_OPERATOR_CMP_EQUAL:
		case AGK_OPERATOR_CMP_NEQUAL:
		case AGK_OPERATOR_CMP_GREATER:
		case AGK_OPERATOR_CMP_EGREATER:
		case AGK_OPERATOR_CMP_LESS:
		case AGK_OPERATOR_CMP_ELESS: 
		case AGK_OPERATOR_BOOL_NOT: 
		case AGK_OPERATOR_BOOL_AND: 
		case AGK_OPERATOR_BOOL_OR: return AGK_VARIABLE_INTEGER;
		default: return dataType;
	}
}

AGKInstruction Parser2::convertToInstruction( int op, int dataType )
{
	if ( dataType == AGK_VARIABLE_INTEGER )
	{
		switch( op )
		{
			case AGK_OPERATOR_ADD: return AGKI_ADD_INT;
			case AGK_OPERATOR_SUB: return AGKI_SUB_INT;
			case AGK_OPERATOR_MUL: return AGKI_MUL_INT;
			case AGK_OPERATOR_DIV: return AGKI_DIV_INT;
//			case AGK_OPERATOR_MOD: return AGKI_MOD_INT;
			case AGK_OPERATOR_POW: return AGKI_POW_INT;
			case AGK_OPERATOR_AND: return AGKI_AND;
			case AGK_OPERATOR_OR: return AGKI_OR;
			case AGK_OPERATOR_XOR: return AGKI_XOR;
			case AGK_OPERATOR_BOOL_AND: return AGKI_BOOL_AND;
			case AGK_OPERATOR_BOOL_OR: return AGKI_BOOL_OR;
			case AGK_OPERATOR_BOOL_NOT: return AGKI_BOOL_NOT;
			case AGK_OPERATOR_CMP_EQUAL: return AGKI_CMP_EQUAL_INT;
			case AGK_OPERATOR_CMP_NEQUAL: return AGKI_CMP_NOT_EQUAL_INT;
			case AGK_OPERATOR_CMP_GREATER: return AGKI_CMP_GREATER_INT;
			case AGK_OPERATOR_CMP_EGREATER: return AGKI_CMP_GREATER_EQUAL_INT;
			case AGK_OPERATOR_CMP_LESS: return AGKI_CMP_LESS_INT;
			case AGK_OPERATOR_CMP_ELESS: return AGKI_CMP_LESS_EQUAL_INT;
			case AGK_OPERATOR_LSHIFT: return AGKI_LSHIFT;
			case AGK_OPERATOR_RSHIFT: return AGKI_RSHIFT;
			case AGK_OPERATOR_NEG: return AGKI_NEG_INT;
			case AGK_OPERATOR_NOT: return AGKI_NOT;
			default: return AGKI_NOP;
		}
	}
	else if ( dataType == AGK_VARIABLE_FLOAT )
	{
		switch( op )
		{
			case AGK_OPERATOR_ADD: return AGKI_ADD_FLOAT;
			case AGK_OPERATOR_SUB: return AGKI_SUB_FLOAT;
			case AGK_OPERATOR_MUL: return AGKI_MUL_FLOAT;
			case AGK_OPERATOR_DIV: return AGKI_DIV_FLOAT;
//			case AGK_OPERATOR_MOD: return AGKI_MOD_FLOAT;
			case AGK_OPERATOR_POW: return AGKI_POW_FLOAT;
			case AGK_OPERATOR_AND: return AGKI_AND;
			case AGK_OPERATOR_OR: return AGKI_OR;
			case AGK_OPERATOR_XOR: return AGKI_XOR;
			case AGK_OPERATOR_CMP_EQUAL: return AGKI_CMP_EQUAL_FLOAT;
			case AGK_OPERATOR_CMP_NEQUAL: return AGKI_CMP_NOT_EQUAL_FLOAT;
			case AGK_OPERATOR_CMP_GREATER: return AGKI_CMP_GREATER_FLOAT;
			case AGK_OPERATOR_CMP_EGREATER: return AGKI_CMP_GREATER_EQUAL_FLOAT;
			case AGK_OPERATOR_CMP_LESS: return AGKI_CMP_LESS_FLOAT;
			case AGK_OPERATOR_CMP_ELESS: return AGKI_CMP_LESS_EQUAL_FLOAT;
			case AGK_OPERATOR_LSHIFT: return AGKI_LSHIFT;
			case AGK_OPERATOR_RSHIFT: return AGKI_RSHIFT;
			case AGK_OPERATOR_NEG: return AGKI_NEG_FLOAT;
			default: return AGKI_NOP;
		}
	}
	else if ( dataType == AGK_VARIABLE_STRING )
	{
		switch( op )
		{
			case AGK_OPERATOR_ADD: return AGKI_STRING_CONCAT;
			case AGK_OPERATOR_CMP_EQUAL: return AGKI_CMP_EQUAL_STRING;
			case AGK_OPERATOR_CMP_NEQUAL: return AGKI_CMP_NOT_EQUAL_STRING;
			case AGK_OPERATOR_CMP_GREATER: return AGKI_CMP_GREATER_STRING;
			case AGK_OPERATOR_CMP_EGREATER: return AGKI_CMP_GREATER_EQUAL_STRING;
			case AGK_OPERATOR_CMP_LESS: return AGKI_CMP_LESS_STRING;
			case AGK_OPERATOR_CMP_ELESS: return AGKI_CMP_LESS_EQUAL_STRING;
			default: return AGKI_NOP;
		}
	}

	return AGKI_NOP;
}

Parser2::Parser2()
{

}

Parser2::~Parser2()
{
	
}

void Parser2::SetSkipInstructions( int skip )
{
	m_iSkipInstructions = skip;
}

int Parser2::ParseDimArray( stFunction *pFunction, cProgramData *pProgram, stVariable *pVar, DynArrayObject<stInstruction> *pInstructions, int tokenID, int &consumed )
{
	int origTokenID = tokenID;

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		pProgram->TokenError( "Dim must be followed by an array name identifier", tokenID );
		return 0;
	}

	// duplicate the array pointer in case we need to assign default values
	if ( pInstructions )
	{
		stInstruction newInstruction;
		newInstruction.Construct( AGKI_DUP, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
		pInstructions->Add( &newInstruction );
	}

	int existingVar = 0;
	int nameToken = tokenID;

	if ( pVar )
	{
		pVar->iTokenID = tokenID;
		pVar->sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
		pVar->sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
	
		if ( pProgram->m_iOptionExplicit )
		{
			/*
			if ( pProgram->FindFunction( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is already being used as a function name", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			*/

			/*
			if ( pProgram->FindType( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is already being used as a type name", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			*/
		}

		int index = pProgram->FindVariable( pVar->sName );
		if ( index >= 0 )
		{
			// already defined
			// "dim" allows redefinition, makes parsing more difficult

			if ( pProgram->m_pVariables.m_pData[ index ]->iNumDimensions == 0 )
			{
				int varToken = pProgram->m_pVariables.m_pData[ index ]->iTokenID;
				int varLine = pProgram->GetToken( varToken )->iLine;
				const char* varInclude = "Unknown File";
				int includeID = pProgram->GetToken( varToken )->iIncludeFile;
				if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) varInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sNameOrig.GetStr();

				int currLine = pProgram->GetToken( tokenID )->iLine;
				const char* currInclude = "Unknown File";
				includeID = pProgram->GetToken( tokenID )->iIncludeFile;
				if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) currInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sName.GetStr();

				uString err;
				err.Format( "Array \"%s\" has already been defined as a variable at line %d in %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), varLine, varInclude );
				Error( err.GetStr(), currLine, currInclude );
				return 0;
			}

			existingVar = 1;
			pVar = pProgram->m_pVariables.m_pData[ index ];
		}
	}
	
	tokenID++;
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_SQR_BRACKET )
	{
		pProgram->TokenError( "Array name must be followed by an open square bracket", tokenID );
		return 0;
	}

	int defaultSuitable = 1;
	int temp[ 6 ];
	int numDimensions = 0;

	if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_CLOSE_SQR_BRACKET )
	{
		tokenID++;
		numDimensions = 1;
		temp[ 0 ] = -1;

		if ( pInstructions )
		{
			stInstruction newInstruction;
			newInstruction.Construct( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, -1, 0 );
			pInstructions->Add( &newInstruction );
		}
	}
	else
	{
		// start dimensions
		do
		{
			if ( numDimensions >= 6 )
			{
				pProgram->TokenError( "Too many array dimensions, maximum of 6 allowed", tokenID );
				return 0;
			}

			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_INTEGER 
			  && (pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_COMMA || pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_CLOSE_SQR_BRACKET) )
			{
				if ( pInstructions )
				{
					stInstruction newInstruction;
					newInstruction.Construct( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID+1 )->iIncludeFile, pProgram->GetToken( tokenID )->iValueInt, 0 );
					pInstructions->Add( &newInstruction );
				}

				temp[ numDimensions ] = pProgram->GetToken( tokenID )->iValueInt;
				tokenID++;
			}
			else
			{
				defaultSuitable = 0;
				int subConsumed = 0;
				int dataType = 0;
				int result = ParseExpression( pFunction, 0, 0, dataType, pInstructions, pProgram, tokenID, subConsumed );
				if ( result <= 0 ) return result;
				if ( dataType != AGK_VARIABLE_INTEGER )
				{
					uString error;
					error.Format( "Cannot use \"%s\" as an array index, must be an integer", getDataTypeName(dataType) );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
				tokenID += subConsumed;
			}
			numDimensions++;
		} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );
	}

	// end dimensions
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
	{
		uString error;
		error.Format( "Unexected token \"%s\", Array dimensions must be separated by commas and enclosed in square brackets", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	if ( pVar )
	{
		if ( existingVar )
		{
			if ( pVar->iNumDimensions != numDimensions )
			{
				const char* szInclude = "Unknown File";
				int includeID = pProgram->GetToken( pVar->iTokenID )->iIncludeFile;
				if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) szInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sNameOrig.GetStr();

				int lineNum = pProgram->GetToken( pVar->iTokenID )->iLine;
				uString msg; msg.Format( "Array has already been defined with a different number of dimensions on line %d in %s, number of dimensions must match", lineNum, szInclude );
				pProgram->TokenError( msg, tokenID );
				return 0;
			}
		}
		else pVar->iNumDimensions = numDimensions;

		// Dim arrays don't have a predefined size, resized at runtime
		/*
		if ( defaultSuitable && !pVar->pDefaultLengths )
		{
			pVar->pDefaultLengths = new int[ numDimensions ];
			for ( int i = 0; i < numDimensions; i++ ) pVar->pDefaultLengths[ i ] = temp [ i ];
		}
		*/
	}

	int iType = 0;
	int iTypeIndex = 0;
	
	tokenID++;
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_AS )
	{
		// Dim arrays do not require an AS
		char lastChar = pProgram->GetToken( nameToken )->sValueRaw.CharAt( pProgram->GetToken( nameToken )->sValueRaw.GetNumChars()-1 );
		if ( lastChar == '#' ) iType = AGK_VARIABLE_ARRAY_FLOAT;
		else if ( lastChar == '$' ) iType = AGK_VARIABLE_ARRAY_STRING;
		else iType = AGK_VARIABLE_ARRAY_INT;
	}
	else
	{
		tokenID++;
		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_DEC_VAR_TYPE )
		{
			if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "integer" ) == 0 ) iType = AGK_VARIABLE_ARRAY_INT;
			else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "float" ) == 0 ) iType = AGK_VARIABLE_ARRAY_FLOAT;
			else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "string" ) == 0 ) iType = AGK_VARIABLE_ARRAY_STRING;
			else
			{
				uString error;
				error.Format( "Unrecognised variable type \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}
		else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_IDENTIFIER )
		{
			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE;
			pProgram->GetToken( tokenID )->pExtraData = 0;

			// type
			iType = AGK_VARIABLE_ARRAY_TYPE;
			iTypeIndex = pProgram->FindType( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( iTypeIndex < 0 )
			{
				uString error;
				error.Format( "\"%s\" has not been defined as a type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0; 
			}
			if ( pVar ) 
			{
				if ( existingVar )
				{
					if ( pVar->iTypeIndex != iTypeIndex )
					{
						const char* szInclude = "Unknown File";
						int includeID = pProgram->GetToken( pVar->iTokenID )->iIncludeFile;
						if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) szInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sNameOrig.GetStr();

						int lineNum = pProgram->GetToken( pVar->iTokenID )->iLine;
						uString msg; msg.Format( "Array has already been defined with a different type on line %d in %s, types must match", lineNum, szInclude );
						pProgram->TokenError( msg, tokenID );
						return 0;
					}
				}
				else pVar->iTypeIndex = iTypeIndex;
			}
		}
		else
		{
			uString error;
			error.Format( "Unexpected token \"%s\", Array data type must be Integer, Float, String, or a user defined type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
		tokenID++;
	}

	if ( pVar ) 
	{
		if ( existingVar )
		{
			if ( pVar->iType != iType )
			{
				const char* szInclude = "Unknown File";
				int includeID = pProgram->GetToken( pVar->iTokenID )->iIncludeFile;
				if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) szInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sNameOrig.GetStr();

				int lineNum = pProgram->GetToken( pVar->iTokenID )->iLine;
				uString msg; msg.Format( "Array has already been defined with a different type on line %d in %s, types must match", lineNum, szInclude );
				pProgram->TokenError( msg, tokenID );
				return 0;
			}
		}
		else pVar->iType = iType;
	}

	if ( pInstructions )
	{
		int dataType = (iType-AGK_VARIABLE_ARRAY_0) | (iTypeIndex << 8);
		stInstruction newInstruction;
		newInstruction.Construct( AGKI_ARRAY_BLOCK_RESIZE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, numDimensions, dataType );
		pInstructions->Add( &newInstruction );
	}

	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
	{
		// remove the duplicated array pointer since we didn't need it
		if ( pInstructions )
		{
			stInstruction newInstruction;
			newInstruction.Construct( AGKI_POP_BLANK, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 1, 0 );
			pInstructions->Add( &newInstruction );
		}

		// valid ending
		consumed += (tokenID - origTokenID);
		return 1;
	}

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS )
	{
		uString error;
		error.Format( "Unexpected token %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	if ( iType == AGK_VARIABLE_ARRAY_TYPE )
	{
		pProgram->TokenError( "Array of types cannot have a default value", tokenID );
		return 0;
	}

	if ( numDimensions > 1 )
	{
		pProgram->TokenError( "Arrays with more than 1 dimension cannot have default values", tokenID );
		return 0;
	}

	if ( !defaultSuitable )
	{
		pProgram->TokenError( "Arrays with undefined sizes cannot have default values", tokenID );
		return 0;
	}

	// must have a default value
	tokenID++;
	consumed += tokenID - origTokenID;
	int result = ParseArrayDefaultValue( iType, temp[ 0 ], 0, pProgram, pInstructions, tokenID, consumed );
	if ( result <= 0 ) return result;
	return 1;
}

int Parser2::ParseArrayDefaultValue( int type, int length, stVariable *pVar, cProgramData *pProgram, DynArrayObject<stInstruction> *pInstructions, int tokenID, int &consumed )
{
	int origTokenID = tokenID;

	if ( pVar && length < 0 )
	{
		pProgram->TokenError( "Cannot assign an array a default value when declared with a zero length", tokenID );
		return 0;
	}

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_SQR_BRACKET )
	{
		pProgram->TokenError( "Array values must begin with a square bracket", tokenID );
		return 0;
	}

	if ( pVar ) 
	{
		if ( type == AGK_VARIABLE_ARRAY_INT ) 
		{
			pVar->pDefaultI = new int[ length+1 ];
			for( int i = 0; i < length+1; i++ ) pVar->pDefaultI[ i ] = 0;
		}
		else if ( type == AGK_VARIABLE_ARRAY_FLOAT ) 
		{
			pVar->pDefaultF = new float[ length+1 ];
			for( int i = 0; i < length+1; i++ ) pVar->pDefaultF[ i ] = 0;
		}
		else if ( type == AGK_VARIABLE_ARRAY_STRING )
		{
			pVar->pDefaultI = new int[ length+1 ];
			for( int i = 0; i < length+1; i++ ) pVar->pDefaultI[ i ] = -1;
		}
	}

	int count = 0;
	int values = 0; 
	do
	{
		tokenID++;
		while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL ) tokenID++;

		if ( type == AGK_VARIABLE_ARRAY_INT )
		{
			int negate = 1;
			if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "-" ) == 0 ) 
			{
				negate = -1;
				tokenID++;
			}

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_INTEGER )
			{
				pProgram->TokenError( "Array values must match the array type (integer), and cannot include variables or expressions", tokenID );
				return 0;
			}
			if ( pVar && count <= length ) pVar->pDefaultI[ count ] = pProgram->GetToken( tokenID )->iValueInt*negate;
			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pProgram->GetToken( tokenID )->iValueInt*negate, 0 );
				pInstructions->Add( &newInstruction );
			}
		}
		else if ( type == AGK_VARIABLE_ARRAY_FLOAT )
		{
			int negate = 1;
			if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "-" ) == 0 ) 
			{
				negate = -1;
				tokenID++;
			}

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_INTEGER && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_FLOAT )
			{
				pProgram->TokenError( "Array values must match the array type (float), and cannot include variables or expressions", tokenID );
				return 0;
			}
			if ( pVar && count <= length ) 
			{
				if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_INTEGER ) pVar->pDefaultF[ count ] = (float) pProgram->GetToken( tokenID )->iValueInt*negate;
				else pVar->pDefaultF[ count ] = pProgram->GetToken( tokenID )->fValueFloat*negate;
			}
			if ( pInstructions )
			{
				converter c;
				if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_INTEGER ) c.f = (float) pProgram->GetToken( tokenID )->iValueInt*negate;
				else c.f = pProgram->GetToken( tokenID )->fValueFloat*negate;
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, c.i, 0 );
				pInstructions->Add( &newInstruction );
			}
		}
		else if ( type == AGK_VARIABLE_ARRAY_STRING )
		{
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_STRING )
			{
				pProgram->TokenError( "Array values must match the array type (string), and cannot include variables or expressions", tokenID );
				return 0;
			}
			if ( pVar && count <= length ) pVar->pDefaultI[ count ] = pProgram->GetToken( tokenID )->iStringIndex;
			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_PUSH_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pProgram->GetToken( tokenID )->iStringIndex, 0 );
				pInstructions->Add( &newInstruction );
			}
		}
		else if ( type == AGK_VARIABLE_ARRAY_TYPE )
		{
			pProgram->TokenError( "Arrays of types cannot have default values", tokenID );
			return 0;
		}
		else
		{
			pProgram->TokenError( "Unrecognised array data type", tokenID );
			return 0;
		}

		count++;
		tokenID++;
	} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

	if ( pVar && count > length+1 )
	{
		pProgram->TokenError( "Number of default values exceeds array size", tokenID );
		return 0;
	}

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
	{
		pProgram->TokenError( "Array values must be followed by a square bracket or comma", tokenID );
		return 0;
	}

	if ( pInstructions )
	{
		stInstruction newInstruction;
		if ( type == AGK_VARIABLE_ARRAY_INT ) newInstruction.Construct( AGKI_ARRAY_ASSIGN_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, count, 1 );
		else if ( type == AGK_VARIABLE_ARRAY_FLOAT ) newInstruction.Construct( AGKI_ARRAY_ASSIGN_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, count, 1 );
		else if ( type == AGK_VARIABLE_ARRAY_STRING ) newInstruction.Construct( AGKI_ARRAY_ASSIGN_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, count, 1 );
		else 
		{
			Error( "Shouldn't get here",0,0 );
			return 0;
		}
		pInstructions->Add( &newInstruction );
	}

	// let calling function handle the EOL
	/*
	tokenID++;
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
	{
		uString error;
		error.Format( "Unexpected token: %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}
	*/

	consumed += (tokenID - origTokenID) + 1;
	return 1;
}

int Parser2::ParseVariableDec( int allowDefaultValue, stVariable *pVar, cProgramData *pProgram, int tokenID, int &consumed )
{
	int origTokenID = tokenID;

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		pProgram->TokenError( "Variable declaration must start with a variable name", tokenID );
		return 0;
	}

	if ( pVar )
	{
		pVar->iTokenID = tokenID;
		pVar->sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
		pVar->sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );

		if ( pProgram->m_iOptionExplicit )
		{
			if ( pProgram->FindFunction( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is already being used as a function name", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			if ( pProgram->FindCommandName( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is a reserved word", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			/*
			if ( pProgram->FindType( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is already being used as a type name", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			*/
		}

		if ( pVar->iScope != AGK_VARIABLE_FUNC_LOCAL )
		{
			int index = pProgram->FindVariable( pVar->sName );
			if ( index >= 0 )
			{
				// already defined
				int varToken = pProgram->m_pVariables.m_pData[ index ]->iTokenID;
				int varLine = pProgram->GetToken( varToken )->iLine;
				const char* varInclude = "Unknown File";
				int includeID = pProgram->GetToken( varToken )->iIncludeFile;
				if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) varInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sNameOrig.GetStr();

				int currLine = pProgram->GetToken( tokenID )->iLine;
				const char* currInclude = "Unknown File";
				includeID = pProgram->GetToken( tokenID )->iIncludeFile;
				if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) currInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sNameOrig.GetStr();

				uString err;
				err.Format( "Variable \"%s\" has already been defined at line %d in %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), varLine, varInclude );
				Error( err.GetStr(), currLine, currInclude );
				return 0;
			}
		}
	}

	tokenID++;
	int iType = 0;
	int numDimensions = 0;
	int temp[ 6 ];
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_AS )
	{
		char lastChar = pProgram->GetToken( origTokenID )->sValueRaw.CharAt( pProgram->GetToken( origTokenID )->sValueRaw.GetNumChars()-1 );
		if ( lastChar == '#' ) iType = AGK_VARIABLE_FLOAT;
		else if ( lastChar == '$' ) iType = AGK_VARIABLE_STRING;
		else iType = AGK_VARIABLE_INTEGER;

		if ( pVar ) pVar->iType = iType;

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL || pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
		{
			consumed += (tokenID - origTokenID);
			return 1;
		}
	}
	else
	{
		tokenID++;
		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_DEC_VAR_TYPE )
		{
			if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "integer" ) == 0 ) iType = AGK_VARIABLE_INTEGER;
			else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "float" ) == 0 ) iType = AGK_VARIABLE_FLOAT;
			else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "string" ) == 0 ) iType = AGK_VARIABLE_STRING;
			else
			{
				uString error;
				error.Format( "Unrecognised variable type \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}
		else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_IDENTIFIER )
		{
			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE;
			pProgram->GetToken( tokenID )->pExtraData = 0;

			// type
			iType = AGK_VARIABLE_TYPE;
			int index = pProgram->FindType( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( index < 0 )
			{
				uString error;
				error.Format( "\"%s\" has not been defined as a type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0; 
			}
			if ( pVar ) pVar->iTypeIndex = index;
		}
		else
		{
			pProgram->TokenError( "Variable data type must be Integer, Float, String, or a user defined type", tokenID );
			return 0;
		}

		int length = 0;
		
		tokenID++;
		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
		{
			// actually an array
			if ( iType == AGK_VARIABLE_INTEGER ) iType = AGK_VARIABLE_ARRAY_INT;
			else if ( iType == AGK_VARIABLE_FLOAT ) iType = AGK_VARIABLE_ARRAY_FLOAT;
			else if ( iType == AGK_VARIABLE_STRING ) iType = AGK_VARIABLE_ARRAY_STRING;
			else if ( iType == AGK_VARIABLE_TYPE ) iType = AGK_VARIABLE_ARRAY_TYPE;

			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CLOSE_SQR_BRACKET )
			{
				numDimensions = 1;
				temp[ 0 ] = -1;
			}
			else
			{
				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_INTEGER )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", Array dimension must be an integer literal or a constant", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				numDimensions = 1;
				length = pProgram->GetToken( tokenID )->iValueInt;
				temp[ 0 ] = length;

				tokenID++;
				while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
				{
					// multiple dimensions
					numDimensions++;
					if ( numDimensions > 6 )
					{
						pProgram->TokenError( "Too many array dimensions, maximum of 6 allowed", tokenID );
						return 0;
					}

					tokenID++;
					if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_INTEGER )
					{
						pProgram->TokenError( "Array dimension must be an integer literal or a constant", tokenID );
						return 0;
					}
					temp[ numDimensions-1 ] = pProgram->GetToken( tokenID )->iValueInt;
					tokenID++;
				}
			}

			// end dimensions
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array dimensions must be integers separated by commas and enclosed in square brackets", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			tokenID++;
		}

		if ( pVar )
		{
			pVar->iType = iType;
			pVar->iNumDimensions = numDimensions;
			if ( numDimensions > 0 )
			{
				pVar->pDefaultLengths = new int[ numDimensions ];
				for ( int i = 0; i < numDimensions; i++ ) pVar->pDefaultLengths[ i ] = temp[ i ];
			}
		}

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL || pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
		{
			// valid ending
			consumed += (tokenID - origTokenID);
			return 1;
		}
	}

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS )
	{
		uString error;
		error.Format( "Unexpected token %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	if ( !allowDefaultValue )
	{
		pProgram->TokenError( "Cannot give a variable a default value here", tokenID );
		return 0;
	}

	if ( iType == AGK_VARIABLE_TYPE || iType == AGK_VARIABLE_ARRAY_TYPE )
	{
		pProgram->TokenError( "Variables defined as types cannot have a default value", tokenID );
		return 0;
	}

	if ( numDimensions > 1 ) 
	{
		pProgram->TokenError( "Arrays with more than 1 dimension cannot have default values", tokenID );
		return 0;
	}

	// parse default value, for array or variable
	if ( numDimensions == 1 )
	{
		// single dimension array
		tokenID++;
		consumed += tokenID - origTokenID;
		int result = ParseArrayDefaultValue( iType, temp[0], pVar, pProgram, 0, tokenID, consumed );
		if ( result <= 0 ) return result;
		return 1;
	}
	else
	{
		// normal variable
		tokenID++;
		int negate = 1;
		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_UNARY_OP )
		{
			if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "-" ) == 0 ) negate = -1;
			else
			{
				pProgram->TokenError( "Variable default value must be a literal or constant", tokenID );
				return 0;
			}

			tokenID++;
		}

		if ( iType == AGK_VARIABLE_INTEGER )
		{
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_INTEGER )
			{
				pProgram->TokenError( "Variable default value must be an integer literal or constant", tokenID );
				return 0;
			}

			if ( pVar )
			{
				pVar->pDefaultI = new int[ 1 ];
				pVar->pDefaultI[ 0 ] = pProgram->GetToken( tokenID )->iValueInt * negate;
			}
		}
		else if ( iType == AGK_VARIABLE_FLOAT )
		{
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_INTEGER && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_FLOAT )
			{
				pProgram->TokenError( "Variable default value must be a float literal or constant", tokenID );
				return 0;
			}

			if ( pVar )
			{
				pVar->pDefaultF = new float[ 1 ];
				if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_INTEGER )
					pVar->pDefaultF[ 0 ] = (float)(pProgram->GetToken( tokenID )->iValueInt * negate);
				else
					pVar->pDefaultF[ 0 ] = pProgram->GetToken( tokenID )->fValueFloat * negate;
			}
		}
		else if ( iType == AGK_VARIABLE_STRING )
		{
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_STRING )
			{
				pProgram->TokenError( "Variable default value must be a string literal or constant", tokenID );
				return 0;
			}

			if ( negate < 0 )
			{
				pProgram->TokenError( "Cannot negate a string", tokenID );
				return 0;
			}

			if ( pVar )
			{
				pVar->pDefaultI = new int[ 1 ];
				pVar->pDefaultI[ 0 ] = pProgram->GetToken( tokenID )->iStringIndex;
			}
		}
		else
		{
			pProgram->TokenError( "Unrecognised variable type", tokenID );
			return 0;
		}
	}

	tokenID++;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
	{
		// multiple variables on one line with default values
		pProgram->TokenError( "Cannot define multiple variables on one line when assigning default values to them", tokenID );
		return 0;
	}

	consumed += (tokenID - origTokenID);
	return 1;
}

int Parser2::ParseTypeVariableDec(  stVariable *pVar, cProgramData *pProgram, int tokenID, int &consumed )
{
	int origTokenID = tokenID;

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		pProgram->TokenError( "Variable declaration must start with a variable name", tokenID );
		return 0;
	}

	if ( pVar )
	{
		pVar->iTokenID = tokenID;
		pVar->sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
		pVar->sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );

		if ( pProgram->m_iOptionExplicit )
		{
			/*
			if ( pProgram->FindType( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is already being used as a type name", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			*/
		
			/*
			if ( pProgram->FindCommandName( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is a reserved word", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			*/
		}

		// simple declarations can have redefinitions of global vars (e.g. types and function parameters)
	}

	tokenID++;
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_AS )
	{
		// simple declaration doesn't need an AS
		if ( pVar )
		{
			char lastChar = pVar->sName.CharAt( pVar->sName.GetNumChars()-1 );
			if ( lastChar == '#' ) pVar->iType = AGK_VARIABLE_FLOAT;
			else if ( lastChar == '$' ) pVar->iType = AGK_VARIABLE_STRING;
			else pVar->iType = AGK_VARIABLE_INTEGER;
		}

		consumed += (tokenID - origTokenID);
		return 1;
	}

	tokenID++;
	int iType = 0;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_DEC_VAR_TYPE )
	{
		if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "integer" ) == 0 ) iType = AGK_VARIABLE_INTEGER;
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "float" ) == 0 ) iType = AGK_VARIABLE_FLOAT;
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "string" ) == 0 ) iType = AGK_VARIABLE_STRING;
		else
		{
			uString error;
			error.Format( "Unrecognised variable type \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}
	else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_IDENTIFIER )
	{
		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE;
		pProgram->GetToken( tokenID )->pExtraData = 0;

		// type
		iType = AGK_VARIABLE_TYPE;
		int index = pProgram->FindType( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
		if ( index < 0 )
		{
			uString error;
			error.Format( "\"%s\" has not been defined as a type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0; 
		}
		if ( pVar ) pVar->iTypeIndex = index;
	}
	else
	{
		uString error;
		error.Format( "Unexpected token \"%s\", variable data type must be Integer, Float, String, or a user defined type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	int numDimensions = 0;
	int length = 0;
	int temp[ 6 ];

	tokenID++;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
	{
		// actually an array
		if ( iType == AGK_VARIABLE_INTEGER ) iType = AGK_VARIABLE_ARRAY_INT;
		else if ( iType == AGK_VARIABLE_FLOAT ) iType = AGK_VARIABLE_ARRAY_FLOAT;
		else if ( iType == AGK_VARIABLE_STRING ) iType = AGK_VARIABLE_ARRAY_STRING;
		else if ( iType == AGK_VARIABLE_TYPE ) iType = AGK_VARIABLE_ARRAY_TYPE;

		tokenID++;
		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CLOSE_SQR_BRACKET )
		{
			numDimensions = 1;
			temp[ 0 ] =-1;
		}
		else
		{
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_INTEGER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", Array dimension must be an integer literal or a constant", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			numDimensions = 1;
			length = pProgram->GetToken( tokenID )->iValueInt;
			temp[ 0 ] = length;

			tokenID++;
			while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
			{
				// multiple dimensions
				numDimensions++;
				if ( numDimensions > 6 )
				{
					pProgram->TokenError( "Too many array dimensions, maximum of 6 allowed", tokenID );
					return 0;
				}

				tokenID++;
				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_INTEGER )
				{
					pProgram->TokenError( "Array dimension must be an integer literal or a constant", tokenID );
					return 0;
				}
				temp[ numDimensions-1 ] = pProgram->GetToken( tokenID )->iValueInt;
				tokenID++;
			}
		}

		// end dimensions
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", array dimensions must be integers separated by commas and enclosed in square brackets", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
		tokenID++;
	}

	if ( pVar )
	{
		pVar->iType = iType;
		pVar->iNumDimensions = numDimensions;
		if ( numDimensions > 0 )
		{
			pVar->pDefaultLengths = new int[ numDimensions ];
			for ( int i = 0; i < numDimensions; i++ ) pVar->pDefaultLengths[ i ] = temp[ i ];
		}
	}

	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EQUALS )
	{
		pProgram->TokenError( "Cannot give a variable a default value here", tokenID );
		return 0;
	}
	
	consumed += (tokenID - origTokenID);
	return 1;
}

int Parser2::ParseParamVariableDec( stVariable *pVar, int &ref, cProgramData *pProgram, int tokenID, int &consumed )
{
	int origTokenID = tokenID;

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		pProgram->TokenError( "Function parameter must start with a variable name", tokenID );
		return 0;
	}

	if ( pVar )
	{
		pVar->iTokenID = tokenID;
		pVar->sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
		pVar->sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );

		if ( pProgram->m_iOptionExplicit )
		{
			if ( pProgram->FindFunction( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is already being used as a function name", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			if ( pProgram->FindCommandName( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is a reserved word", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			/*
			if ( pProgram->FindType( pVar->sName.GetStr() ) >= 0 )
			{
				uString error;
				error.Format( "\"%s\" is already being used as a type name", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			*/
		}

		// parameter declarations can have redefinitions of global vars
	}

	ref = 0;
	tokenID++;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_DEC_REFERENCE )
	{
		ref = 1;
		tokenID++;
	}

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_AS )
	{
		// parameter declaration doesn't need an AS
		if ( pVar )
		{
			char lastChar = pVar->sName.CharAt( pVar->sName.GetNumChars()-1 );
			if ( lastChar == '#' ) pVar->iType = AGK_VARIABLE_FLOAT;
			else if ( lastChar == '$' ) pVar->iType = AGK_VARIABLE_STRING;
			else pVar->iType = AGK_VARIABLE_INTEGER;
		}

		consumed += (tokenID - origTokenID);
		return 1;
	}

	tokenID++;
	int iType = 0;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_DEC_VAR_TYPE )
	{
		if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "integer" ) == 0 ) iType = AGK_VARIABLE_INTEGER;
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "float" ) == 0 ) iType = AGK_VARIABLE_FLOAT;
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "string" ) == 0 ) iType = AGK_VARIABLE_STRING;
		else
		{
			uString error;
			error.Format( "Unrecognised parameter type \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}
	else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_IDENTIFIER )
	{
		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE;
		pProgram->GetToken( tokenID )->pExtraData = 0;

		// type
		iType = AGK_VARIABLE_TYPE;
		int index = pProgram->FindType( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
		if ( index < 0 )
		{
			uString error;
			error.Format( "\"%s\" has not been defined as a type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0; 
		}
		if ( pVar ) pVar->iTypeIndex = index;
	}
	else
	{
		uString error;
		error.Format( "Unexpected token \"%s\", parameter data type must be Integer, Float, String, or a user defined type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	int numDimensions = 0;
	int length = 0;

	tokenID++;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
	{
		tokenID++;
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", array parameters don't use size values, use integer[] or integer[][] for arrays with two dimensions", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		// actually an array
		if ( iType == AGK_VARIABLE_INTEGER ) iType = AGK_VARIABLE_ARRAY_INT;
		else if ( iType == AGK_VARIABLE_FLOAT ) iType = AGK_VARIABLE_ARRAY_FLOAT;
		else if ( iType == AGK_VARIABLE_STRING ) iType = AGK_VARIABLE_ARRAY_STRING;
		else if ( iType == AGK_VARIABLE_TYPE ) iType = AGK_VARIABLE_ARRAY_TYPE;

		numDimensions = 1;
		
		tokenID++;
		while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
		{
			// multiple dimensions
			numDimensions++;
			if ( numDimensions > 6 )
			{
				pProgram->TokenError( "Too many array dimensions, maximum of 6 allowed", tokenID );
				return 0;
			}

			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array parameters don't use size values. For example use integer[] or integer[][] for arrays with two dimensions", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			tokenID++;
		}
	}

	if ( ref == 1 && numDimensions == 0 && iType != AGK_VARIABLE_TYPE )
	{
		pProgram->TokenError( "Cannot pass basic data types by reference, only arrays and types support the ref keyword", tokenID );
		return 0;
	}

	if ( pVar ) 
	{
		pVar->iType = iType;
		pVar->iNumDimensions = numDimensions;
	}

	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EQUALS )
	{
		pProgram->TokenError( "Cannot assign a default value here", tokenID );
		return 0;
	}
	
	consumed += (tokenID - origTokenID);
	return 1;
}

// array must be pushed on the stack before calling
int Parser2::ParseArrayExpression( stFunction *pFunction, cProgramData *pProgram, int &dataType, stVariable *pParentVar, DynArrayObject<stInstruction> *pInstructions, int tokenID, int &consumed, int allowUnassignedVars )
{
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		// shouldn't get here
		uString error;
		error.Format( "Unexpected token \"%s\", array must start with an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	tokenID++;
	consumed++; // consume identifier

	int numDimensions = 0;
	int isArray = 1;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
	{
		consumed++; // consume bracket
		
		do
		{
			tokenID++;
			int subConsumed = 0;
			int dataType = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, pInstructions, pProgram, tokenID, subConsumed, allowUnassignedVars );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				uString error;
				error.Format( "Cannot use \"%s\" as an array index, must be an integer", getDataTypeName(dataType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			numDimensions++;

			AGKInstruction instruction = AGKI_NOP;
			if ( numDimensions < pParentVar->iNumDimensions ) instruction = AGKI_ARRAY_DEREF_ARRAY;
			else
			{
				switch ( pParentVar->iType )
				{
					case AGK_VARIABLE_ARRAY_INT: instruction = AGKI_ARRAY_DEREF_INT; break;
					case AGK_VARIABLE_ARRAY_FLOAT: instruction = AGKI_ARRAY_DEREF_FLOAT; break;
					case AGK_VARIABLE_ARRAY_STRING: instruction = AGKI_ARRAY_DEREF_STRING; break;
					case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_ARRAY_DEREF_TYPE; break;
					default: Error( "Unrecognised array type", 0,0 ); return 0;
				}
			}

			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				pInstructions->Add( &newInstruction ); 
			}

			tokenID += subConsumed;
			consumed += subConsumed + 1; // also consume the comma
		} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
		{
			pProgram->TokenError( "Array access must end with a square close bracket", tokenID );
			return 0;
		}

		if ( pParentVar->iNumDimensions == numDimensions ) isArray = 0;
		else if ( numDimensions > pParentVar->iNumDimensions  )
		{
			uString err; err.Format( "Array access to \"%s\" has too many dimensions", pParentVar->sNameOrig.GetStr() );
			pProgram->TokenError( err, tokenID );
			return 0;
		}

		tokenID++;
	}

	if ( isArray )
	{
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DOT )
		{
			int remainingDimensions = pParentVar->iNumDimensions - numDimensions;
			int typeIndex = 0;
			if ( pParentVar->iType == AGK_VARIABLE_ARRAY_TYPE ) typeIndex = pParentVar->iTypeIndex;
			dataType = pParentVar->iType | (remainingDimensions << 5) | (typeIndex << 8);
			return 1;
		}

		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_ARRAY_EXP_DOT;
		pProgram->GetToken( tokenID )->pExtraData = 0;

		// dot must be followed by .length or .find
		tokenID++;
		consumed++; // consume dot
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", array must be followed by a complete set of dimensions or one of .length, .find, .tojson", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_ARRAY_FUNC_EXP;
		pProgram->GetToken( tokenID )->pExtraData = 0;

		if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "length" ) == 0 )
		{
			consumed++; // consume "length"
			dataType = AGK_VARIABLE_INTEGER;

			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_ARRAY_LENGTH, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				pInstructions->Add( &newInstruction ); 
			}
			return 1;			
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "find" ) == 0 || pProgram->GetToken( tokenID )->sValueRaw.CompareTo ( "indexof" ) == 0 )
		{
			int iIndexOf = 0;

			if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo ( "indexof" ) == 0 )
				iIndexOf = 1;
			
			// search the array with array.find(item)
			int remainingDimensions = pParentVar->iNumDimensions - numDimensions;
			if ( remainingDimensions > 1 )
			{
				pProgram->TokenError( "Cannot search arrays of arrays, use more dimensions to search a sub array", tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume "find"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array find must be in the format .find(item)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			int type = pParentVar->iType - AGK_VARIABLE_ARRAY_0;
			if ( type == AGK_VARIABLE_TYPE )
			{
				if ( !pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->valid ) return 0;

				stVariable findVar;
				findVar.sName.SetStr( pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->sFirstVar );
				int index = pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->m_pSubVars.Find( &findVar );
				if ( index < 0 ) 
				{
					pProgram->TokenError( "Parser error, could not find first type variable", tokenID );
					return 0;
				}
				
				type = pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->m_pSubVars.m_pData[ index ]->iType;
				if ( type != AGK_VARIABLE_INTEGER && type != AGK_VARIABLE_FLOAT && type != AGK_VARIABLE_STRING )
				{
					pProgram->TokenError( "Can only search arrays of types if the first type variable is an Integer, Float, or String", tokenID );
					return 0;
				}
			}

			int subDataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, subDataType, pInstructions, pProgram, tokenID, subConsumed, allowUnassignedVars );
			if ( result <= 0 ) return result;
			if ( pParentVar->iType == AGK_VARIABLE_ARRAY_TYPE && type != (subDataType & 0x1f) )
			{
				uString error;
				error.Format( "To search an array of types the search expression must match the first variable of the type (%s in this case)", getDataTypeName(type) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			if ( (subDataType & 0x1f) != type )
			{
				if ( subDataType == AGK_VARIABLE_FLOAT && type == AGK_VARIABLE_INTEGER )
				{
					if ( pInstructions )
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pInstructions->Add( &newInstruction );
					}
				}
				else if ( subDataType == AGK_VARIABLE_INTEGER && type == AGK_VARIABLE_FLOAT )
				{
					if ( pInstructions )
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pInstructions->Add( &newInstruction );
					}
				}
				else
				{
					uString error;
					error.Format( "Tried to search for %s with %s", getDataTypeName(subDataType), getDataTypeName(type) );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
			}
			tokenID += subConsumed;
			consumed += subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;

				if ( iIndexOf == 1 )
					error.Format( "Unexpected token \"%s\", array find must be in the format .IndexOf(item)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				else
					error.Format ( "Unexpected token \"%s\", array find must be in the format .find(item)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr ( ) );

				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			if ( pInstructions )
			{
				stInstruction newInstruction;
				AGKInstruction instruction = AGKI_ARRAY_FIND;
				if ( type == AGK_VARIABLE_STRING ) instruction = AGKI_ARRAY_FIND_STRING; 

				if ( iIndexOf == 1 )
				{
					instruction = AGKI_ARRAY_INDEXOF;
					if ( type == AGK_VARIABLE_STRING ) instruction = AGKI_ARRAY_INDEXOF_STRING;
				}

				newInstruction.Construct( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				pInstructions->Add( &newInstruction );
			}

			dataType = AGK_VARIABLE_INTEGER;

			consumed++; // consume close bracket
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "tojson" ) == 0 )
		{
			// convert the array to json string
			tokenID++;
			consumed++; // consume "tojson"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array to json must be in the format .tojson()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array to json must be in the format .tojson()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_ARRAY_TO_JSON, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				pInstructions->Add( &newInstruction );
			}

			dataType = AGK_VARIABLE_STRING;

			consumed++; // consume close bracket
			return 1;
		}
		else
		{
			uString error;
			error.Format( "Unexpected token \"%s\", array must be followed by a complete set of dimensions or one of .length, .find, .indexof, .tojson", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}
	else
	{
		if ( pParentVar->iType != AGK_VARIABLE_ARRAY_TYPE ) 
		{
			// normal variable
			dataType = pParentVar->iType - AGK_VARIABLE_ARRAY_0;
			return 1;
		}
		else
		{
			// type
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DOT )
			{
				dataType = (pParentVar->iType - AGK_VARIABLE_ARRAY_0) | (pParentVar->iTypeIndex << 8);
				return 1;
			}

			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_EXP_DOT;
			pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ];

			tokenID++;
			consumed++; // consume dot
			return ParseTypeExpression( pFunction, pProgram, dataType, pParentVar, pInstructions, tokenID, consumed, allowUnassignedVars );
		}
	}

	return 1;
}

int Parser2::ParseTypeExpression( stFunction *pFunction, cProgramData *pProgram, int &dataType, stVariable *pParentVar, DynArrayObject<stInstruction> *pInstructions, int tokenID, int &consumed, int allowUnassignedVars )
{
	int typeIndex = pParentVar->iTypeIndex;
	if ( typeIndex < 0 )
	{
		uString error;
		error.Format( "\"%s\" is not a valid type", pParentVar->sNameOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		uString error;
		error.Format( "Unexpected token \"%s\", type field must be an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	// if this type generated an error during parsing ignore this expression and don't generate another error
	if ( !pProgram->m_pTypes.m_pData[ typeIndex ]->valid ) return 0;

	if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_BRACKET )
	{
		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_FUNC_EXP;
		pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ typeIndex ];

		// type function
		if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "tojson" ) == 0 )
		{
			// convert the array to json string
			tokenID++;
			consumed++; // consume "tojson"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET ) // redundant, but here in case refactoring removes outer check
			{
				uString error;
				error.Format( "Unexpected token \"%s\", type to json must be in the format .tojson()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", type to json must be in the format .tojson()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_TYPE_TO_JSON, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				pInstructions->Add( &newInstruction );
			}

			dataType = AGK_VARIABLE_STRING;

			consumed++; // consume close bracket
			return 1;
		}
		else
		{
			uString error;
			error.Format( "Type function \"%s\" not recognised, builtin function is .tojson()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}
	else
	{
		// type variable
		stVariable subVar;
		subVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw );
		subVar.sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig );
		int subVarIndex = pProgram->m_pTypes.m_pData[ typeIndex ]->m_pSubVars.Find( &subVar );
		if ( subVarIndex < 0 )
		{
			uString error;
			error.Format( "\"%s\" is not a valid field for type \"%s\"", subVar.sNameOrig.GetStr(), pProgram->m_pTypes.m_pData[ typeIndex ]->sNameOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_SUB_VARIABLE;
		pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ typeIndex ];

		stVariable *pSubVar = pProgram->m_pTypes.m_pData[ typeIndex ]->m_pSubVars.m_pData[ subVarIndex ];
		int varID = pSubVar->iVarID;
	
		AGKInstruction instruction = AGKI_NOP;
		switch ( pSubVar->iType )
		{
			case AGK_VARIABLE_INTEGER: instruction = AGKI_TYPE_DEREF_INT; break;
			case AGK_VARIABLE_FLOAT: instruction = AGKI_TYPE_DEREF_FLOAT; break;
			case AGK_VARIABLE_STRING: instruction = AGKI_TYPE_DEREF_STRING; break;
			case AGK_VARIABLE_TYPE: instruction = AGKI_TYPE_DEREF_TYPE; break;
			case AGK_VARIABLE_ARRAY_INT: 
			case AGK_VARIABLE_ARRAY_FLOAT: 
			case AGK_VARIABLE_ARRAY_STRING: 
			case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_TYPE_DEREF_ARRAY; break;
			default: Error( "Unrecognised variable type", 0,0 ); return 0;
		}
	
		if ( pInstructions )
		{
			stInstruction newInstruction;
			newInstruction.Construct( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, typeIndex );
			pInstructions->Add( &newInstruction ); 
		}

		if ( pSubVar->iNumDimensions > 0 ) 
		{
			// array
			return ParseArrayExpression( pFunction, pProgram, dataType, pSubVar, pInstructions, tokenID, consumed, allowUnassignedVars );
		}
		else if ( pSubVar->iType == AGK_VARIABLE_TYPE )
		{
			// type
			tokenID++;
			consumed++; // consume identifier

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_DOT )
			{
				pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_EXP_DOT;
				pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ pSubVar->iTypeIndex ];

				tokenID++;
				consumed++; // consume dot
				return ParseTypeExpression( pFunction, pProgram, dataType, pSubVar, pInstructions, tokenID, consumed, allowUnassignedVars );
			}
			else
			{
				dataType = AGK_VARIABLE_TYPE | (pSubVar->iTypeIndex << 8);
				return 1;
			}
		}
		else
		{
			// variable
			tokenID++;
			consumed++; // consume identifier
			dataType = pSubVar->iType;
			return 1;
		}
	}
}

// array must be pushed on the stack before calling
int Parser2::ParseArrayStatement( stFunction *pFunction, cProgramData *pProgram, stVariable *pParentVar, int tokenID, int &consumed )
{
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		// shouldn't get here
		uString error;
		error.Format( "Unexpected token \"%s\", array must start with an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	tokenID++;
	consumed++; // consume identifier

	int isArray = 1;
	int numDimensions = 0;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
	{
		consumed++; // consume bracket
		do
		{
			// for statements we don't want to dereference the final array index, so delay the instruction adding to the next loop
			if ( numDimensions > 0 && numDimensions < pParentVar->iNumDimensions )
			{
				pProgram->AddInstruction( AGKI_ARRAY_DEREF_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}

			tokenID++;
			int subConsumed = 0;
			int dataType = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				uString error;
				error.Format( "Cannot use \"%s\" as an array index, must be an integer", getDataTypeName(dataType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			numDimensions++;

			tokenID += subConsumed;
			consumed += subConsumed + 1; // also consume the comma
		} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
		{
			pProgram->TokenError( "Array access must end with a square close bracket", tokenID );
			return 0;
		}

		if ( pParentVar->iNumDimensions == numDimensions ) isArray = 0;
		else if ( numDimensions > pParentVar->iNumDimensions  )
		{
			uString err; err.Format( "Array access to \"%s\" has too many dimensions", pParentVar->sNameOrig.GetStr() );
			pProgram->TokenError( err, tokenID );
			return 0;
		}

		tokenID++;
	}

	if ( isArray )
	{
		if ( numDimensions > 0 ) pProgram->AddInstruction( AGKI_ARRAY_DEREF_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DOT )
		{
			// assign one array to another
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array variable must be followed by an equals or a dot", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume equals

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
			{
				// array literal assignment
				int remainingDimensions = pParentVar->iNumDimensions - numDimensions;
				if ( remainingDimensions > 1 )
				{
					pProgram->TokenError( "You can only assign array values to the last dimension of the array", tokenID );
					return 0;
				}
				if ( remainingDimensions < 1 )
				{
					pProgram->TokenError( "Cannot assign array values to an array element", tokenID );
					return 0;
				}

				int result = ParseArrayDefaultValue( pParentVar->iType, 0, 0, pProgram, &(pProgram->m_pInstructions), tokenID, consumed ); 
				return result;
			}
			else
			{
				int dataType = 0;
				int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, consumed );
				if ( result <= 0 ) return result;
				if ( (dataType & 0x1f) != pParentVar->iType )
				{
					uString error;
					error.Format( "Tried to assign %s to %s", getDataTypeName(dataType), getDataTypeName(pParentVar->iType) );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
				if ( pParentVar->iType == AGK_VARIABLE_ARRAY_TYPE && (dataType >> 8) != pParentVar->iTypeIndex )
				{
					pProgram->TokenError( "Cannot assign two different user defined types to each other", tokenID );
					return 0;
				}
				int remainingDimensions = pParentVar->iNumDimensions - numDimensions;
				if ( remainingDimensions != ((dataType >> 5) & 7) )
				{
					pProgram->TokenError( "Cannot assign arrays with a different number of dimensions to each other", tokenID );
					return 0;
				}
				
				pProgram->AddInstruction( AGKI_STORE_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				return 1;
			}
		}

		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_ARRAY_DOT;
		pProgram->GetToken( tokenID )->pExtraData = 0;

		tokenID++;
		consumed++; // consume dot

		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", array must be followed by a complete set of dimensions or one of .length, .swap, .reverse, .insert, .remove, .sort, .insertsorted, .load, .save, .fromjson, .tojson", 
						  pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_ARRAY_FUNC;
		pProgram->GetToken( tokenID )->pExtraData = 0;

		// .length
		if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "length" ) == 0 )
		{
			// resizing with array.length = <expr>
			tokenID++;
			consumed++; // consume "length"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array length must be followed by an equals and an expression here", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume equals

			int dataType = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, consumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				pProgram->TokenError( "New array length must be an integer data type", tokenID );
				return 0;
			}

			// if we are resizing an array of arrays of arrays the final data type is an array (ID 5)
			if ( numDimensions+2 < pParentVar->iNumDimensions )
			{
				pProgram->AddInstruction( AGKI_ARRAY_RESIZE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 5, 0 );
			}
			else
			{
				int type = pParentVar->iType - AGK_VARIABLE_ARRAY_0;
				if ( type == AGK_VARIABLE_TYPE ) type |= (pParentVar->iTypeIndex << 8);
				pProgram->AddInstruction( AGKI_ARRAY_RESIZE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, type, 0 );
			}
			
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "swap" ) == 0 )
		{
			// swaping two array elements with arrya.swap(index1,index2)

			tokenID++;
			consumed++; // consume "swap"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array swap must be in the format .swap(index1,index2)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			int dataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				pProgram->TokenError( "Array swap index must be an integer data type", tokenID );
				return 0;
			}
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_COMMA )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array swap must be in the format .swap(index1,index2)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume comma

			dataType = 0;
			subConsumed = 0;
			result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				pProgram->TokenError( "Array swap index must be an integer data type", tokenID );
				return 0;
			}
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array swap must be in the format .swap(index1,index2)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume close bracket

			pProgram->AddInstruction( AGKI_ARRAY_SWAP, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "insert" ) == 0 )
		{
			// inserting an element with array.insert(element,[index])

			tokenID++;
			consumed++; // consume "insert"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array insert must be in the format .insert(item) or .insert(item,index)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			int dataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( pParentVar->iType == AGK_VARIABLE_ARRAY_TYPE && (dataType >> 8) != pParentVar->iTypeIndex )
			{
				pProgram->TokenError( "Cannot insert a user defined type that is different from the array type", tokenID );
				return 0;
			}
			int remainingDimensions = pParentVar->iNumDimensions - numDimensions;
			if ( remainingDimensions-1 != ((dataType >> 5) & 7) )
			{
				pProgram->TokenError( "Cannot insert arrays with a different number of dimensions into each other", tokenID );
				return 0;
			}
			int type = pParentVar->iType;
			if ( remainingDimensions == 1 ) type -= AGK_VARIABLE_ARRAY_0;
			if ( (dataType & 0x1f) != type )
			{
				uString error;
				error.Format( "Tried to insert %s into %s", getDataTypeName(dataType), getDataTypeName(type) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_COMMA )
			{
				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", array insert must be in the format .insert(item) or .insert(item,index)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				// valid ending
				AGKInstruction instruction = AGKI_NOP;
				if ( remainingDimensions > 1 ) instruction = AGKI_ARRAY_INSERT_TAIL_ARRAY;
				else
				{
					switch( pParentVar->iType )
					{
						case AGK_VARIABLE_ARRAY_INT: instruction = AGKI_ARRAY_INSERT_TAIL_INT; break;
						case AGK_VARIABLE_ARRAY_FLOAT: instruction = AGKI_ARRAY_INSERT_TAIL_FLOAT; break;
						case AGK_VARIABLE_ARRAY_STRING: instruction = AGKI_ARRAY_INSERT_TAIL_STRING; break;
						case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_ARRAY_INSERT_TAIL_TYPE; break;
						default: Error( "Unrecognised variable type",0,0 );
					}
				}
				pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

				consumed++; // consume close bracket
				return 1;
			}

			tokenID++;
			consumed++; // consume comma

			dataType = 0;
			subConsumed = 0;
			result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				pProgram->TokenError( "Insert index must be an integer data type", tokenID );
				return 0;
			}
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array insert must be in the format .insert(item) or .insert(item,index)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume close bracket

			AGKInstruction instruction = AGKI_NOP;
			if ( remainingDimensions > 1 ) instruction = AGKI_ARRAY_INSERT_ARRAY;
			else
			{
				switch( pParentVar->iType )
				{
					case AGK_VARIABLE_ARRAY_INT: instruction = AGKI_ARRAY_INSERT_INT; break;
					case AGK_VARIABLE_ARRAY_FLOAT: instruction = AGKI_ARRAY_INSERT_FLOAT; break;
					case AGK_VARIABLE_ARRAY_STRING: instruction = AGKI_ARRAY_INSERT_STRING; break;
					case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_ARRAY_INSERT_TYPE; break;
					default: Error( "Unrecognised variable type",0,0 );
				}
			}
			pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "remove" ) == 0 )
		{
			// removing an element with array.remove(index)

			tokenID++;
			consumed++; // consume "remove"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array remove must be in the format .remove() or .remove(index)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CLOSE_BRACKET )
			{
				// valid ending
				consumed++; // consume close bracket
				pProgram->AddInstruction( AGKI_ARRAY_REMOVE_TAIL, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				return 1;
			}

			int dataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				pProgram->TokenError( "Array remove index must be an integer data type", tokenID );
				return 0;
			}
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array remove must be in the format .remove() or .remove(index)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // conusme close bracket

			pProgram->AddInstruction( AGKI_ARRAY_REMOVE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "sort" ) == 0 )
		{
			// sort an array with array.sort()
			int remainingDimensions = pParentVar->iNumDimensions - numDimensions;
			if ( remainingDimensions > 1 )
			{
				pProgram->TokenError( "Sort cannot be called on arrays of arrays, use more dimensions to sort a sub array", tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume "sort"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array sort must be in the format .sort()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			int noCase = 0;
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_INTEGER )
			{
				if ( pProgram->GetToken( tokenID )->iValueInt == 1 ) noCase = 1;
				else if ( pProgram->GetToken( tokenID )->iValueInt != 0 )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", array sort mode must be equal to 0 or 1", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				tokenID++;
				consumed++; // consume integer
			}

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array sort must be in the format .sort()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // conusme close bracket

			int type = pParentVar->iType - AGK_VARIABLE_ARRAY_0;
			if ( type == AGK_VARIABLE_TYPE )
			{
				if ( !pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->valid ) return 0;

				stVariable findVar;
				findVar.sName.SetStr( pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->sFirstVar );
				int index = pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->m_pSubVars.Find( &findVar );
				if ( index < 0 ) 
				{
					pProgram->TokenError( "Parser error, could not find first type variable", tokenID );
					return 0;
				}
				
				type = pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->m_pSubVars.m_pData[ index ]->iType;
				if ( type != AGK_VARIABLE_INTEGER && type != AGK_VARIABLE_FLOAT && type != AGK_VARIABLE_STRING )
				{
					pProgram->TokenError( "Can only sort arrays of types if the first type variable is an Integer, Float, or String", tokenID );
					return 0;
				}
			}

			pProgram->AddInstruction( AGKI_ARRAY_SORT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, noCase, 0 );
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "reverse" ) == 0 )
		{
			// reverse an array with array.reverse()
			
			tokenID++;
			consumed++; // consume "reverse"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array reverse must be in the format .reverse()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array reverse must be in the format .reverse()", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // conusme close bracket

			pProgram->AddInstruction( AGKI_ARRAY_REVERSE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "insertsorted" ) == 0 )
		{
			// inserting an element with array.insertsorted(element)
			int remainingDimensions = pParentVar->iNumDimensions - numDimensions;
			if ( remainingDimensions > 1 )
			{
				pProgram->TokenError( "Cannot insert sorted items into arrays of arrays, use more dimensions to insert into a sub array", tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume "insertsorted"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array insertsorted must be in the format .insertsorted(item)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			int type = pParentVar->iType - AGK_VARIABLE_ARRAY_0;
			if ( type == AGK_VARIABLE_TYPE )
			{
				if ( !pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->valid ) return 0;

				stVariable findVar;
				findVar.sName.SetStr( pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->sFirstVar );
				int index = pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->m_pSubVars.Find( &findVar );
				if ( index < 0 ) 
				{
					pProgram->TokenError( "Parser error, could not find first type variable", tokenID );
					return 0;
				}
				
				type = pProgram->m_pTypes.m_pData[ pParentVar->iTypeIndex ]->m_pSubVars.m_pData[ index ]->iType;
				if ( type != AGK_VARIABLE_INTEGER && type != AGK_VARIABLE_FLOAT && type != AGK_VARIABLE_STRING )
				{
					pProgram->TokenError( "Can only insert types if the first type variable is an Integer, Float, or String", tokenID );
					return 0;
				}
			}

			int dataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			type = pParentVar->iType;
			if ( remainingDimensions == 1 ) type -= AGK_VARIABLE_ARRAY_0;
			if ( (dataType & 0x1f) != type )
			{
				if ( dataType == AGK_VARIABLE_INTEGER && type == AGK_VARIABLE_FLOAT )
				{
					pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}
				else if ( dataType == AGK_VARIABLE_FLOAT && type == AGK_VARIABLE_INTEGER )
				{
					pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}
				else
				{
					uString error;
					error.Format( "Tried to insert %s into %s", getDataTypeName(dataType), getDataTypeName(type) );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
			}
			if ( pParentVar->iType == AGK_VARIABLE_ARRAY_TYPE && (dataType >> 8) != pParentVar->iTypeIndex )
			{
				pProgram->TokenError( "Cannot insert a user defined type that is different from the array type", tokenID );
				return 0;
			}
			
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array insertsorted must be in the format .insertsorted(item)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			AGKInstruction instruction = AGKI_ARRAY_INSERT_SORTED;
			if ( pParentVar->iType == AGK_VARIABLE_ARRAY_STRING ) instruction = AGKI_ARRAY_INSERT_SORTED_STRING;
			pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			consumed++; // consume close bracket
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "load" ) == 0 || pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "save" ) == 0 )
		{
			// saving/loading to/from file with .save(filename) or .load(filename)
			int isLoading = (pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "load" ) == 0) ? 1 : 0;

			tokenID++;
			consumed++; // consume "load"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array save/load must be in the format .save(filename) or .load(filename)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			int dataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_STRING )
			{
				uString error;
				error.Format( "Filename must be a string, found %s", getDataTypeName(dataType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
						
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array save/load must be in the format .save(filename) or .load(filename)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			AGKInstruction instruction = AGKI_ARRAY_LOAD_FROM_FILE;
			if ( !isLoading ) instruction = AGKI_ARRAY_SAVE_TO_FILE;
			pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			consumed++; // consume close bracket
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "fromjson" ) == 0 )
		{
			// loading from json with .fromjson(string)
			tokenID++;
			consumed++; // consume "load"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array from json must be in the format .fromjson(string)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			int dataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_STRING )
			{
				uString error;
				error.Format( "JSON must be a string, found %s", getDataTypeName(dataType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
						
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array from json must be in the format .fromjson(string)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			AGKInstruction instruction = AGKI_ARRAY_FROM_JSON;
			pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			consumed++; // consume close bracket
			return 1;
		}
		else
		{
			uString error;
			error.Format( "Unexpected token \"%s\", array must be followed by a complete set of dimensions or one of .length, .swap, .reverse, .insert, .remove, .sort, .insertsorted", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}
	else if ( pParentVar->iType == AGK_VARIABLE_ARRAY_TYPE )
	{
		pProgram->AddInstruction( AGKI_ARRAY_DEREF_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
		return ParseTypeStatement( pFunction, pProgram, pParentVar, tokenID, consumed );
	}
	else
	{
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS ) 
		{
			uString error;
			error.Format( "Unexpected token \"%s\", array must be followed by an equals and an expression here", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		tokenID++;
		consumed++; // consume equals

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
		{
			// array literal assignment
			pProgram->TokenError( "Cannot assign array values to an array element", tokenID );
			return 0;
		}

		int varType = pParentVar->iType;
		AGKInstruction instruction = AGKI_NOP;
		switch( pParentVar->iType )
		{
			case AGK_VARIABLE_ARRAY_FLOAT:	varType = AGK_VARIABLE_FLOAT;	instruction = AGKI_STORE_ARRAY_FLOAT; break;
			case AGK_VARIABLE_ARRAY_INT:	varType = AGK_VARIABLE_INTEGER; instruction = AGKI_STORE_ARRAY_INT; break;
			case AGK_VARIABLE_ARRAY_STRING: varType = AGK_VARIABLE_STRING;	instruction = AGKI_STORE_ARRAY_STRING; break;
			default: Error("Unrecognised array data type",0,0); return 0;
		}
		int dataType = 0;
		int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, consumed );
		if ( result <= 0 ) return result;
		if ( (dataType & 0x1f) != varType )
		{
			if ( dataType == AGK_VARIABLE_INTEGER && varType == AGK_VARIABLE_FLOAT )
			{
				pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else if ( dataType == AGK_VARIABLE_FLOAT && varType == AGK_VARIABLE_INTEGER )
			{
				pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else
			{
				uString error;
				error.Format( "Cannot assign %s to %s", getDataTypeName(dataType), getDataTypeName(varType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}

		pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
		return result;
	}

	return 1;
}

// type must be pushed to stack before calling
int Parser2::ParseTypeStatement( stFunction *pFunction, cProgramData *pProgram, stVariable *pParentVar, int tokenID, int &consumed )
{
	int typeIndex = pParentVar->iTypeIndex;
	if ( typeIndex < 0 )
	{
		uString error;
		error.Format( "\"%s\" is not a valid type", pParentVar->sNameOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DOT )
	{
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", type variable must be followed by an equals or a dot", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		tokenID++;
		consumed++; // consume equals

		int dataType = 0;
		int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, consumed );
		if ( result <= 0 ) return result;
		if ( (dataType & 0x1f) != AGK_VARIABLE_TYPE )
		{
			uString error;
			error.Format( "Tried to assign %s to Type", getDataTypeName( dataType ) );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
		if ( (dataType >> 8) != typeIndex )
		{
			pProgram->TokenError( "Cannot assign two different user defined types to each other", tokenID );
			return 0;
		}
		
		pProgram->AddInstruction( AGKI_STORE_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
		return 1;
	}

	// if this type generated an error during parsing ignore this statement and don't generate another error
	if ( !pProgram->m_pTypes.m_pData[ typeIndex ]->valid ) return 0;

	pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_DOT;
	pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ typeIndex ];

	tokenID++;
	consumed++; // consume dot

	if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_BRACKET )
	{
		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_FUNC;
		pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ typeIndex ];

		// type function
		if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "fromjson" ) == 0 )
		{
			tokenID++;
			consumed++; // consume "load"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET ) // redundant, but here in case refactoring removes outer check
			{
				uString error;
				error.Format( "Unexpected token \"%s\", type from json must be in the format .fromjson(string)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume bracket

			int dataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_STRING )
			{
				uString error;
				error.Format( "JSON must be a string, found %s", getDataTypeName(dataType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
						
			tokenID += subConsumed;
			consumed+= subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", type from json must be in the format .fromjson(string)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			AGKInstruction instruction = AGKI_TYPE_FROM_JSON;
			pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			consumed++; // consume close bracket
			return 1;
		}
		else
		{
			uString error;
			error.Format( "Type function \"%s\" not recognised, built in function is .fromjson(string)", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}
	else
	{
		// type variable
		stVariable subVar;
		subVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw );
		subVar.sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig );
		int subVarIndex = pProgram->m_pTypes.m_pData[ typeIndex ]->m_pSubVars.Find( &subVar );
		if ( subVarIndex < 0 )
		{
			uString error;
			error.Format( "\"%s\" is not a valid field for type \"%s\"", subVar.sNameOrig.GetStr(), pProgram->m_pTypes.m_pData[ typeIndex ]->sNameOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_SUB_VARIABLE;
		pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ typeIndex ];

		stVariable *pSubVar = pProgram->m_pTypes.m_pData[ typeIndex ]->m_pSubVars.m_pData[ subVarIndex ];
		int varID = pSubVar->iVarID;
		if ( pSubVar->iNumDimensions > 0 ) 
		{
			pProgram->AddInstruction( AGKI_TYPE_DEREF_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, typeIndex );
			return ParseArrayStatement( pFunction, pProgram, pSubVar, tokenID, consumed );
		}
		else if ( pSubVar->iType == AGK_VARIABLE_TYPE )
		{
			tokenID++;
			consumed++; // consume identifier
			pProgram->AddInstruction( AGKI_TYPE_DEREF_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, typeIndex );
			return ParseTypeStatement( pFunction, pProgram, pSubVar, tokenID, consumed );
		}
		else
		{
			tokenID++;
			consumed++; // consume identifier

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", type field must be followed by an equals and an expression here", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			AGKInstruction instruction = AGKI_NOP;
			switch ( pSubVar->iType )
			{
				case AGK_VARIABLE_INTEGER:	instruction = AGKI_STORE_TYPE_INT; break;
				case AGK_VARIABLE_FLOAT:	instruction = AGKI_STORE_TYPE_FLOAT; break;
				case AGK_VARIABLE_STRING:	instruction = AGKI_STORE_TYPE_STRING; break;
				default:
				{
					pProgram->TokenError( "Unexpected variable type", tokenID );
					return 0;
				}
			}

			tokenID++;
			consumed++; // consume equals

			int dataType = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, consumed );
			if ( result <= 0 ) return result;
			if ( (dataType & 0x1f) != pSubVar->iType )
			{
				if ( dataType == AGK_VARIABLE_INTEGER && pSubVar->iType == AGK_VARIABLE_FLOAT )
				{
					pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}
				else if ( dataType == AGK_VARIABLE_FLOAT && pSubVar->iType == AGK_VARIABLE_INTEGER )
				{
					pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}
				else
				{
					uString error;
					error.Format( "Cannot assign %s to %s", getDataTypeName(dataType), getDataTypeName(pSubVar->iType) );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
			}
		
			pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, typeIndex );
		}
	}

	return 1;
}

// array must be pushed on the stack before calling
int Parser2::ParseArrayInc( int inc, stFunction *pFunction, cProgramData *pProgram, stVariable *pParentVar, int tokenID, int &consumed )
{
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		// shouldn't get here
		uString error;
		error.Format( "Unexpected token \"%s\", array must start with an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	tokenID++;
	consumed++; // consume identifier

	int isArray = 1;
	int numDimensions = 0;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
	{
		consumed++; // consume bracket
		do
		{
			// for statements we don't want to dereference the final array index, so delay the instruction adding to the next loop
			if ( numDimensions > 0 && numDimensions < pParentVar->iNumDimensions )
			{
				pProgram->AddInstruction( AGKI_ARRAY_DEREF_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}

			tokenID++;
			int subConsumed = 0;
			int dataType = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				uString error;
				error.Format( "Cannot use \"%s\" as an array index, must be an integer", getDataTypeName(dataType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			numDimensions++;

			tokenID += subConsumed;
			consumed += subConsumed + 1; // also consume the comma
		} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
		{
			pProgram->TokenError( "Array access must end with a square close bracket", tokenID );
			return 0;
		}

		if ( pParentVar->iNumDimensions == numDimensions ) isArray = 0;
		else if ( numDimensions > pParentVar->iNumDimensions  )
		{
			uString err; err.Format( "Array access to \"%s\" has too many dimensions", pParentVar->sNameOrig.GetStr() );
			pProgram->TokenError( err, tokenID );
			return 0;
		}

		tokenID++;
	}

	if ( isArray )
	{
		uString err; err.Format( "Array access to \"%s\" must have a full set of dimensions", pParentVar->sNameOrig.GetStr() );
		pProgram->TokenError( err, tokenID );
		return 0;
	}
	
	if ( pParentVar->iType == AGK_VARIABLE_ARRAY_TYPE )
	{
		pProgram->AddInstruction( AGKI_ARRAY_DEREF_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
		return ParseTypeInc( inc, pFunction, pProgram, pParentVar, tokenID, consumed );
	}
	else
	{
		if ( pParentVar->iType == AGK_VARIABLE_ARRAY_STRING )
		{
			pProgram->TokenError( "Cannot increment or decrement arrays of strings", tokenID );
			return 0;
		}

		int dataType = 0;
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_COMMA ) 
		{
			converter c; c.f = 1;
			if ( pParentVar->iType == AGK_VARIABLE_ARRAY_FLOAT ) pProgram->AddInstruction( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, c.i, 0 );
			if ( pParentVar->iType == AGK_VARIABLE_ARRAY_INT ) pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 1, 0 );
			dataType = pParentVar->iType - AGK_VARIABLE_ARRAY_0;
		}
		else
		{
			tokenID++;
			consumed++; // consume comma

			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, consumed );
			if ( result <= 0 ) return result;
		}

		int varType = pParentVar->iType - AGK_VARIABLE_ARRAY_0;
		if ( (dataType & 0x1f) != varType )
		{
			if ( dataType == AGK_VARIABLE_INTEGER && varType == AGK_VARIABLE_FLOAT )
			{
				pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else if ( dataType == AGK_VARIABLE_FLOAT && varType == AGK_VARIABLE_INTEGER )
			{
				pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else
			{
				uString error;
				error.Format( "Cannot assign %s to %s", getDataTypeName(dataType), getDataTypeName(varType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}

		AGKInstruction instruction = AGKI_NOP;
		if ( pParentVar->iType == AGK_VARIABLE_ARRAY_FLOAT )
		{
			instruction = inc ? AGKI_INC_ARRAY_FLOAT : AGKI_DEC_ARRAY_FLOAT;
		} 
		else if ( pParentVar->iType == AGK_VARIABLE_ARRAY_INT )
		{
			instruction = inc ? AGKI_INC_ARRAY_INT : AGKI_DEC_ARRAY_INT;
		}
		else
		{
			pProgram->TokenError( "Unrecognised array data type", tokenID );
			return 0;
		}

		pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
		return 1;
	}

	return 1;
}

// type must be pushed to stack before calling
int Parser2::ParseTypeInc( int inc, stFunction *pFunction, cProgramData *pProgram, stVariable *pParentVar, int tokenID, int &consumed )
{
	int typeIndex = pParentVar->iTypeIndex;
	if ( typeIndex < 0 )
	{
		uString error;
		error.Format( "\"%s\" is not a valid type", pParentVar->sNameOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DOT )
	{
		uString error;
		error.Format( "Unexpected token \"%s\", type variable must be followed by a dot", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	// if this type generated an error during parsing ignore this expression and don't generate another error
	if ( !pProgram->m_pTypes.m_pData[ typeIndex ]->valid ) return 0;

	pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_DOT;
	pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ typeIndex ];

	tokenID++;
	consumed++; // consume dot

	stVariable subVar;
	subVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw );
	subVar.sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig );
	int subVarIndex = pProgram->m_pTypes.m_pData[ typeIndex ]->m_pSubVars.Find( &subVar );
	if ( subVarIndex < 0 )
	{
		uString error;
		error.Format( "\"%s\" is not a valid field for type \"%s\"", subVar.sNameOrig.GetStr(), pProgram->m_pTypes.m_pData[ typeIndex ]->sNameOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_SUB_VARIABLE;
	pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ typeIndex ];

	stVariable *pSubVar = pProgram->m_pTypes.m_pData[ typeIndex ]->m_pSubVars.m_pData[ subVarIndex ];
	int varID = pSubVar->iVarID;
	if ( pSubVar->iNumDimensions > 0 ) 
	{
		pProgram->AddInstruction( AGKI_TYPE_DEREF_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, typeIndex );
		return ParseArrayInc( inc, pFunction, pProgram, pSubVar, tokenID, consumed );
	}
	else if ( pSubVar->iType == AGK_VARIABLE_TYPE )
	{
		tokenID++;
		consumed++; // consume identifier
		pProgram->AddInstruction( AGKI_TYPE_DEREF_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, typeIndex );
		return ParseTypeInc( inc, pFunction, pProgram, pSubVar, tokenID, consumed );
	}
	else
	{
		tokenID++;
		consumed++; // consume identifier

		if ( pSubVar->iType == AGK_VARIABLE_STRING )
		{
			pProgram->TokenError( "Cannot increment or decrement strings", tokenID );
			return 0;
		}

		int dataType = 0;
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_COMMA )
		{
			converter c; c.f = 1;
			if ( pSubVar->iType == AGK_VARIABLE_FLOAT ) pProgram->AddInstruction( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, c.i, 0 );
			if ( pSubVar->iType == AGK_VARIABLE_INTEGER ) pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 1, 0 );
			dataType = pSubVar->iType;
		}
		else
		{
			tokenID++;
			consumed++; // consume comma

			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, consumed );
			if ( result <= 0 ) return result;
		}
		
		if ( (dataType & 0x1f) != pSubVar->iType )
		{
			if ( dataType == AGK_VARIABLE_INTEGER && pSubVar->iType == AGK_VARIABLE_FLOAT )
			{
				pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else if ( dataType == AGK_VARIABLE_FLOAT && pSubVar->iType == AGK_VARIABLE_INTEGER )
			{
				pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else
			{
				uString error;
				error.Format( "Cannot assign %s to %s", getDataTypeName(dataType), getDataTypeName(pSubVar->iType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}

		AGKInstruction instruction = AGKI_NOP;
		if ( pSubVar->iType == AGK_VARIABLE_INTEGER )
		{
			instruction = inc ? AGKI_INC_TYPE_INT : AGKI_DEC_TYPE_INT;
		}
		else if ( pSubVar->iType == AGK_VARIABLE_FLOAT )
		{
			instruction = inc ? AGKI_INC_TYPE_FLOAT : AGKI_DEC_TYPE_FLOAT;
		}
		else
		{
			pProgram->TokenError( "Unexpected variable type", tokenID );
			return 0;
		}
		
		pProgram->AddInstruction( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, typeIndex );
	}

	return 1;
}

int Parser2::ParseExpression( stFunction *pFunction, int allowFunctions, int prevPrec, int &dataType, DynArrayObject<stInstruction> *pInstructions, cProgramData *pProgram, int tokenID, int &consumed, int allowUnassignedVars )
{
	switch( pProgram->GetToken( tokenID )->iType )
	{
		case AGK_TOKEN_IDENTIFIER:
		{
			int identToken = tokenID;
			int pluginIndex = -1;

			if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_BRACKET )
			{
				// function call
				if ( !allowFunctions )
				{
					pProgram->TokenError( "Cannot use a function call here", identToken );
					return 0;
				}

				consumed++; // consume identifier
				tokenID++;

				unsigned int numParams = 0;

				int dataTypes[ 32 ];
				DynArrayObject<stInstruction> parameterInstructions[ 32 ];
				
				if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_CLOSE_BRACKET )
				{
					consumed += 2; // consume open and close brackets
					tokenID += 2;
				}
				else
				{
					consumed++; // consume open bracket
					do
					{
						if ( numParams >= 32 )
						{
							pProgram->TokenError( "Too many parmaeters, a maximum of 32 are supported", tokenID );
							return 0;
						}
						tokenID++;
						int subConsumed = 0;
						int result = ParseExpression( pFunction, allowFunctions, 0, dataTypes[numParams], pInstructions ? parameterInstructions+numParams : 0, pProgram, tokenID, subConsumed, allowUnassignedVars );
						if ( result <= 0 ) return result;
						numParams++;
						tokenID += subConsumed;
						consumed += subConsumed + 1; // also consume the comma
					} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

					if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
					{
						pProgram->TokenError( "Function call must end with a close bracket", tokenID );
						return 0;
					}
					tokenID++;
				}

#ifndef AGK_STATIC_LIB
				// hard coded commands for interpreter compiler
				if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "compile" ) == 0 )
				{
					if ( numParams != 1 || dataTypes[0] != AGK_VARIABLE_STRING )
					{
						pProgram->TokenError( "\"Compile\" function only accepts 1 parameter which must be of type String", identToken );
						return 0;
					}

					if ( pInstructions )
					{
						for ( int j = 0; j < parameterInstructions[ 0 ].m_iCount; j++ )
						{
							pInstructions->Add( parameterInstructions[ 0 ].m_pData[ j ] );
						}

						// add command instruction
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_COMPILE, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
						pInstructions->Add( &newInstruction );
					}

					dataType = AGK_VARIABLE_INTEGER;
					break;
				}

				if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "getcompileerrors" ) == 0 )
				{
					if ( numParams != 0 )
					{
						pProgram->TokenError( "\"GetCompileErrors\" function does not take any parameters", identToken );
						return 0;
					}

					if ( pInstructions )
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_GET_COMPILE_ERRORS, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
						pInstructions->Add( &newInstruction );
					}

					dataType = AGK_VARIABLE_STRING;
					break;
				}

				if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "runcode" ) == 0 )
				{
					pProgram->TokenError( "\"RunCode\" cannot be called here, it does not return a value", identToken );
					return 0;
				}
#endif
				
				// find function or command
				int funcIndex = pProgram->FindFunction( pProgram->GetToken( identToken )->sValueRaw.GetStr() );
				if ( funcIndex >= 0 )
				{
					// user defined function
					stFunction *pFunction = pProgram->m_pFunctions.m_pData[ funcIndex ];
					if ( !pFunction->valid ) return 0;

					pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_USER_FUNCTION;
					pProgram->GetToken( identToken )->pExtraData = pFunction;

					// check parameter count
					if ( pFunction->iNumParams != numParams )
					{
						uString error;
						error.Format( "Number of parameters does not match function declaration for \"%s\", expected %d parameters", pProgram->GetToken( identToken )->sValueRawOrig.GetStr(), pFunction->iNumParams );
						pProgram->TokenError( error.GetStr(), identToken );
						return 0;
					}

					if ( pFunction->iReturnType == 0 )
					{
						uString error;
						error.Format( "Function \"%s\" must return a value to be used here", pFunction->sNameOrig.GetStr() );
						pProgram->TokenError( error.GetStr(), identToken );
						return 0;
					}

					// add space on the stack for the return value
					int returnType = pFunction->iReturnType & 0x1f;
					int returnTypeType = pFunction->iReturnType >> 8;
					switch( returnType )
					{
						case AGK_VARIABLE_INTEGER:
						case AGK_VARIABLE_FLOAT:
						{
							stInstruction newInstruction;
							newInstruction.Construct( AGKI_PUSH_BLANK, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 1, 0 );
							pInstructions->Add( &newInstruction );
							break;
						}
						case AGK_VARIABLE_STRING:
						{
							stInstruction newInstruction;
							newInstruction.Construct( AGKI_PUSH_BLANK, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 1 );
							pInstructions->Add( &newInstruction );
							break;
						}
						case AGK_VARIABLE_TYPE: 
						{
							stInstruction newInstruction;
							newInstruction.Construct( AGKI_PUSH_TEMP_TYPE, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, returnTypeType, 0 );
							pInstructions->Add( &newInstruction );
							break;
						}
						case AGK_VARIABLE_ARRAY_INT: 
						case AGK_VARIABLE_ARRAY_FLOAT:
						case AGK_VARIABLE_ARRAY_STRING:
						case AGK_VARIABLE_ARRAY_TYPE:
						{
							stInstruction newInstruction;
							newInstruction.Construct( AGKI_PUSH_TEMP_ARRAY, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
							pInstructions->Add( &newInstruction );
							break;
						}
						default: Error( "Unrecognised variable type", 0, 0 );
					}

					// check parameter types
					for ( unsigned int i = 0; i < numParams; i++ )
					{
						int type1 = pProgram->m_pFunctions.m_pData[ funcIndex ]->iParamsTypes[ i ] & 0x1f;
						int type2 = dataTypes[ i ] & 0x1f;
						if ( type1 != type2 )
						{
							if ( dataTypes[ i ] == AGK_VARIABLE_INTEGER && pProgram->m_pFunctions.m_pData[ funcIndex ]->iParamsTypes[ i ] == AGK_VARIABLE_FLOAT )
							{
								if ( pInstructions )
								{
									if ( parameterInstructions[ i ].m_iCount == 1 && parameterInstructions[ i ].m_pData[0]->iInstruction == AGKI_PUSH_INT )
									{
										// optimization, convert push_int in to push_float
										parameterInstructions[ i ].m_pData[0]->f = (float) parameterInstructions[ i ].m_pData[0]->i;
										parameterInstructions[ i ].m_pData[0]->iInstruction = AGKI_PUSH_FLOAT;
									}
									else
									{
										stInstruction newInstruction;
										newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
										parameterInstructions[ i ].Add( &newInstruction );
									}
								}
								dataTypes[ i ] = AGK_VARIABLE_FLOAT;
							}
							else if ( dataTypes[ i ] == AGK_VARIABLE_FLOAT && pProgram->m_pFunctions.m_pData[ funcIndex ]->iParamsTypes[ i ] == AGK_VARIABLE_INTEGER )
							{
								if ( pInstructions )
								{
									stInstruction newInstruction;
									newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
									parameterInstructions[ i ].Add( &newInstruction );
								}
								dataTypes[ i ] = AGK_VARIABLE_INTEGER;
							}
							else
							{
								uString error;
								error.Format( "Cannot convert type \"%s\" to \"%s\"", getDataTypeName(dataTypes[i]), getDataTypeName(pProgram->m_pFunctions.m_pData[ funcIndex ]->iParamsTypes[i]) );
								pProgram->TokenError( error.GetStr(), identToken );
								return 0;
							}
						}

						if ( type1 == AGK_VARIABLE_TYPE || type1 == AGK_VARIABLE_ARRAY_TYPE )
						{
							// check types
							type1 = pProgram->m_pFunctions.m_pData[ funcIndex ]->iParamsTypes[ i ] >> 8;
							type2 = dataTypes[ i ] >> 8;
							if ( type1 != type2 )
							{
								uString error;
								error.Format( "Cannot convert type \"%s\" to \"%s\"", pProgram->m_pTypes.m_pData[ type2 ]->sNameOrig.GetStr(), pProgram->m_pTypes.m_pData[ type1 ]->sNameOrig.GetStr() );
								pProgram->TokenError( error.GetStr(), identToken );
								return 0;
							}
						}
						else
						{
							// check array dimensions
							type1 = (pProgram->m_pFunctions.m_pData[ funcIndex ]->iParamsTypes[ i ] >> 5) & 7;
							type2 = (dataTypes[ i ] >> 5) & 7;
							if ( type1 != type2 )
							{
								uString error;
								error.Format( "Cannot convert an array with %d dimensions to one with %d dimensions", type2, type1 );
								pProgram->TokenError( error.GetStr(), identToken );
								return 0;
							}
						}

						if ( pInstructions )
						{
							for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
							{
								pInstructions->Add( parameterInstructions[ i ].m_pData[ j ] );
							}
						}
					}

					// add the function call instruction
					if ( pInstructions )
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_FUNCTION_CALL, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, funcIndex, 0 );
						pInstructions->Add( &newInstruction );

						// pop parameters off the stack
						for ( int i = ((int)numParams)-1; i >= 0; i-- )
						{
							AGKInstruction instruction = AGKI_NOP;
							int type = dataTypes[ i ] & 0x1f;
							switch( type )
							{
								case AGK_VARIABLE_INTEGER: instruction = AGKI_POP_VAR; break;
								case AGK_VARIABLE_FLOAT: instruction = AGKI_POP_VAR; break;
								case AGK_VARIABLE_STRING: instruction = AGKI_POP_STRING; break;
								case AGK_VARIABLE_TYPE: instruction = AGKI_POP_TYPE; break;
								case AGK_VARIABLE_ARRAY_INT: 
								case AGK_VARIABLE_ARRAY_FLOAT: 
								case AGK_VARIABLE_ARRAY_STRING: 
								case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_POP_ARRAY; break;
								default: Error( "Unrecognised variable type", 0, 0 );
							}

							stInstruction newInstruction;
							newInstruction.Construct( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
							pInstructions->Add( &newInstruction );
						}
					}

					dataType = pProgram->m_pFunctions.m_pData[ funcIndex ]->iReturnType;
				}
				else
				{
					int commandID = pProgram->FindCommandName( pProgram->GetToken( identToken )->sValueRaw.GetStr() );
					if ( commandID < 0 )
					{
						uString error;
						error.Format( "\"%s\" is not a recognised function or command", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
						pProgram->TokenError( error.GetStr(), identToken );
						return 0;
					}

					pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_AGK_COMMAND;
					pProgram->GetToken( identToken )->pExtraData = 0;

					if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "arsetup" ) == 0 ) pProgram->m_iFlags |= AGK_PROGRAM_FLAG_USES_AR;
					else if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "playvideotoimage" ) == 0 ) pProgram->m_iFlags |= AGK_PROGRAM_FLAG_USES_EXTERN_TEX;
					else if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "setdevicecameratoimage" ) == 0 ) pProgram->m_iFlags |= AGK_PROGRAM_FLAG_USES_EXTERN_TEX;

					stCommand findCommand;
					findCommand.iNumParams = numParams;
					strcpy( findCommand.szCommandName, pProgram->GetToken( identToken )->sValueRaw.GetStr() );
					for ( uint32_t i = 0; i < numParams; i++ ) findCommand.iParamsTypes[i] = (dataTypes[i] & 0x1f);
					funcIndex = pProgram->FindCommand( &findCommand );
					if ( funcIndex >= 0 )
					{
						// native AGK command
						if ( pProgram->m_pCommands[ funcIndex ].iReturnType == 0 )
						{
							uString error;
							error.Format( "Cannot call \"%s\" here, it does not return a value", findCommand.szCommandName );
							pProgram->TokenError( error.GetStr(), identToken );
							return 0;
						}

						if ( pInstructions )
						{
							for ( unsigned int i = 0; i < numParams; i++ )
							{
								for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
								{
									pInstructions->Add( parameterInstructions[ i ].m_pData[ j ] );
								}
							}

							// add command instruction
							stInstruction newInstruction;
							newInstruction.Construct( pProgram->m_pCommands[ funcIndex ].instructionID, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
							pInstructions->Add( &newInstruction );
						}

						dataType = pProgram->m_pCommands[ funcIndex ].iReturnType;
					}
					else 
					{
						funcIndex = pProgram->FindCommandCompatible( &findCommand );
						if ( funcIndex >= 0 )
						{
							// AGK command with casting
							if ( pProgram->m_pCommands[ funcIndex ].iReturnType == 0 )
							{
								uString error;
								error.Format( "Cannot call \"%s\" here, it does not return a value", findCommand.szCommandName );
								pProgram->TokenError( error.GetStr(), identToken );
								return 0;
							}

							// check parameter types
							for ( unsigned int i = 0; i < numParams; i++ )
							{
								if ( pProgram->m_pCommands[ funcIndex ].iParamsTypes[ i ] != dataTypes[ i ] )
								{
									if ( dataTypes[ i ] == AGK_VARIABLE_INTEGER && pProgram->m_pCommands[ funcIndex ].iParamsTypes[ i ] == AGK_VARIABLE_FLOAT )
									{
										if ( pInstructions )
										{
											if ( parameterInstructions[ i ].m_iCount == 1 && parameterInstructions[ i ].m_pData[0]->iInstruction == AGKI_PUSH_INT )
											{
												// optimization, convert push_int in to push_float
												parameterInstructions[ i ].m_pData[0]->f = (float) parameterInstructions[ i ].m_pData[0]->i;
												parameterInstructions[ i ].m_pData[0]->iInstruction = AGKI_PUSH_FLOAT;
											}
											else
											{
												stInstruction newInstruction;
												newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
												parameterInstructions[ i ].Add( &newInstruction );
											}
										}
										dataTypes[ i ] = AGK_VARIABLE_FLOAT;
									}
									else if ( dataTypes[ i ] == AGK_VARIABLE_FLOAT && pProgram->m_pCommands[ funcIndex ].iParamsTypes[ i ] == AGK_VARIABLE_INTEGER )
									{
										if ( pInstructions )
										{
											stInstruction newInstruction;
											newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
											parameterInstructions[ i ].Add( &newInstruction );
										}
										dataTypes[ i ] = AGK_VARIABLE_INTEGER;
									}
									else
									{
										uString error;
										error.Format( "Cannot convert type \"%s\" to \"%s\"", getDataTypeName(dataTypes[i]), getDataTypeName(pProgram->m_pCommands[ funcIndex ].iParamsTypes[i]) );
										pProgram->TokenError( error.GetStr(), identToken );
										return 0;
									}
								}

								if ( pInstructions )
								{
									for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
									{
										pInstructions->Add( parameterInstructions[ i ].m_pData[ j ] );
									}
								}
							}

							// add command instruction
							if ( pInstructions )
							{
								stInstruction newInstruction;
								newInstruction.Construct( pProgram->m_pCommands[ funcIndex ].instructionID, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
								pInstructions->Add( &newInstruction );
							}

							dataType = pProgram->m_pCommands[ funcIndex ].iReturnType;
						}
						else
						{
							if ( findCommand.iNumParams > 0 )
							{
								uString error;
								error.Format( "\"%s\" does not accept the parameters (", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
								for ( int i = 0; i < findCommand.iNumParams; i++ )
								{
									error.Append( getDataTypeName(findCommand.iParamsTypes[i]) );
									error.Append( ", " );
								}
								error.Trunc2(2);
								error.Append( ")" );
								pProgram->TokenError( error.GetStr(), identToken );
								return 0;
							}
							else
							{
								uString error;
								error.Format( "\"%s\" expects %d parameters", pProgram->GetToken( identToken )->sValueRawOrig.GetStr(), pProgram->m_pCommands[ commandID ].iNumParams );
								pProgram->TokenError( error.GetStr(), identToken );
								return 0;
							}
						}
					}
				}
			}
			else if ( (pluginIndex = pProgram->FindPlugin(pProgram->GetToken( tokenID )->sValueRaw.GetStr())) >= 0 )
			{
				// plugin command used as an expression
				if ( pProgram->GetToken( tokenID+1 )->iType != AGK_TOKEN_DOT || pProgram->GetToken( tokenID+2 )->iType != AGK_TOKEN_IDENTIFIER )
				{
					pProgram->TokenError( "Plugin reference must be followed by a dot and then the function call", tokenID );
					return 0;
				}

				pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_PLUGIN;
				pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pPlugins.m_pData[ pluginIndex ];

				pProgram->GetToken( tokenID+1 )->iTypeExtra = AGK_TOKEN_EXTRA_PLUGIN_DOT;
				pProgram->GetToken( tokenID+1 )->pExtraData = pProgram->m_pPlugins.m_pData[ pluginIndex ];

				// consume pluginRef and dot
				tokenID += 2;
				consumed += 2;

				if ( !allowFunctions )
				{
					pProgram->TokenError( "Cannot use a function call here", tokenID );
					return 0;
				}

				pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_PLUGIN_FUNC;
				pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pPlugins.m_pData[ pluginIndex ];

				identToken = tokenID;
				consumed++; // consume identifier
				tokenID++;

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
				{
					pProgram->TokenError( "Plugin function call must include open and close brackets", tokenID );
					return 0;
				}

				unsigned int numParams = 0;

				int dataTypes[ 32 ];
				DynArrayObject<stInstruction> parameterInstructions[ 32 ];
				
				if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_CLOSE_BRACKET )
				{
					consumed += 2; // consume open and close brackets
					tokenID += 2;
				}
				else
				{
					consumed++; // consume open bracket
					do
					{
						if ( numParams >= 32 )
						{
							pProgram->TokenError( "Too many parmaeters, a maximum of 32 are supported", tokenID );
							return 0;
						}
						tokenID++;
						int subConsumed = 0;
						int result = ParseExpression( pFunction, allowFunctions, 0, dataTypes[numParams], pInstructions ? parameterInstructions+numParams : 0, pProgram, tokenID, subConsumed, allowUnassignedVars );
						if ( result <= 0 ) return result;
						numParams++;
						tokenID += subConsumed;
						consumed += subConsumed + 1; // also consume the comma
					} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

					if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
					{
						pProgram->TokenError( "Function call must end with a close bracket", tokenID );
						return 0;
					}
					tokenID++;
				}

				int commandID = pProgram->FindPluginCommandName( pluginIndex, pProgram->GetToken( identToken )->sValueRaw.GetStr() );
				if ( commandID < 0 )
				{
					uString error;
					error.Format( "\"%s\" is not a recognised command for that plugin", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				stPluginCommand findCommand;
				findCommand.iNumParams = numParams;
				findCommand.sTier1Name.SetStr( pProgram->GetToken( identToken )->sValueRaw );
				for ( uint32_t i = 0; i < numParams; i++ ) findCommand.iParamsTypes[i] = (dataTypes[i] & 0x1f);
				int funcIndex = pProgram->FindPluginCommand( pluginIndex, &findCommand );
				if ( funcIndex >= 0 )
				{
					// native plugin command
					stPluginCommand *pPluginCommand = pProgram->m_pPlugins.m_pData[ pluginIndex ]->m_pCommands + funcIndex;
					pPluginCommand->iFlags |= AGK_PLUGIN_COMMAND_USED;

					if ( pPluginCommand->iReturnType == 0 )
					{
						uString error;
						error.Format( "Cannot call \"%s\" here, it does not return a value", pPluginCommand->sTier1Name.GetStr() );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}

					if ( pInstructions )
					{
						for ( unsigned int i = 0; i < numParams; i++ )
						{
							for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
							{
								pInstructions->Add( parameterInstructions[ i ].m_pData[ j ] );
							}
						}

						// add command instruction
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_PLUGIN_COMMAND, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pluginIndex, funcIndex );
						pInstructions->Add( &newInstruction );
					}

					dataType = pPluginCommand->iReturnType;
				}
				else 
				{
					funcIndex = pProgram->FindPluginCommandCompatible( pluginIndex, &findCommand );
					if ( funcIndex >= 0 )
					{
						// plugin command with casting
						stPluginCommand *pPluginCommand = pProgram->m_pPlugins.m_pData[ pluginIndex ]->m_pCommands + funcIndex;
						pPluginCommand->iFlags |= AGK_PLUGIN_COMMAND_USED;

						if ( pPluginCommand->iReturnType == 0 )
						{
							uString error;
							error.Format( "Cannot call \"%s\" here, it does not return a value", pPluginCommand->sTier1Name.GetStr() );
							pProgram->TokenError( error.GetStr(), tokenID );
							return 0;
						}

						// check parameter types
						for ( unsigned int i = 0; i < numParams; i++ )
						{
							if ( pPluginCommand->iParamsTypes[ i ] != dataTypes[ i ] )
							{
								if ( dataTypes[ i ] == AGK_VARIABLE_INTEGER && pPluginCommand->iParamsTypes[ i ] == AGK_VARIABLE_FLOAT )
								{
									if ( pInstructions )
									{
										if ( parameterInstructions[ i ].m_iCount == 1 && parameterInstructions[ i ].m_pData[0]->iInstruction == AGKI_PUSH_INT )
										{
											// optimization, convert push_int in to push_float
											parameterInstructions[ i ].m_pData[0]->f = (float) parameterInstructions[ i ].m_pData[0]->i;
											parameterInstructions[ i ].m_pData[0]->iInstruction = AGKI_PUSH_FLOAT;
										}
										else
										{
											stInstruction newInstruction;
											newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
											parameterInstructions[ i ].Add( &newInstruction );
										}
									}
									dataTypes[ i ] = AGK_VARIABLE_FLOAT;
								}
								else if ( dataTypes[ i ] == AGK_VARIABLE_FLOAT && pPluginCommand->iParamsTypes[ i ] == AGK_VARIABLE_INTEGER )
								{
									if ( pInstructions )
									{
										stInstruction newInstruction;
										newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
										parameterInstructions[ i ].Add( &newInstruction );
									}
									dataTypes[ i ] = AGK_VARIABLE_INTEGER;
								}
								else
								{
									uString error;
									error.Format( "Cannot convert type \"%s\" to \"%s\"", getDataTypeName(dataTypes[i]), getDataTypeName(pPluginCommand->iParamsTypes[i]) );
									pProgram->TokenError( error.GetStr(), tokenID );
									return 0;
								}
							}

							if ( pInstructions )
							{
								for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
								{
									pInstructions->Add( parameterInstructions[ i ].m_pData[ j ] );
								}
							}
						}

						// add command instruction
						if ( pInstructions )
						{
							stInstruction newInstruction;
							newInstruction.Construct( AGKI_PLUGIN_COMMAND, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pluginIndex, funcIndex );
							pInstructions->Add( &newInstruction );
						}

						dataType = pPluginCommand->iReturnType;
					}
					else
					{
						if ( findCommand.iNumParams > 0 )
						{
							uString error;
							error.Format( "\"%s\" does not accept the parameters (", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
							for ( int i = 0; i < findCommand.iNumParams; i++ )
							{
								error.Append( getDataTypeName(findCommand.iParamsTypes[i]) );
								error.Append( ", " );
							}
							error.Trunc2(2);
							error.Append( ")" );
							pProgram->TokenError( error.GetStr(), tokenID );
							return 0;
						}
						else
						{
							uString error;
							error.Format( "\"%s\" expects %d parameters", pProgram->GetToken( identToken )->sValueRawOrig.GetStr(), pProgram->m_pPlugins.m_pData[ pluginIndex ]->m_pCommands[ funcIndex ].iNumParams );
							pProgram->TokenError( error.GetStr(), tokenID );
							return 0;
						}
					}
				}
			}
			else
			{
				// confirm identifier is a variable
				stVariable newVar;
				stVariable *pVar = 0;
				stVariable findVar;
				findVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );

				// check function local first, if applicable
				if ( pFunction )
				{
					int index = pFunction->pFuncVars.Find( &findVar );
					if ( index >= 0 ) 
					{
						pVar = pFunction->pFuncVars.m_pData[ index ];
						pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
						pProgram->GetToken( tokenID )->pExtraData = pFunction;
					}
				}

				if ( !pVar )
				{
					int index = pProgram->m_pVariables.Find( &findVar );
					if ( index >= 0 )
					{
						// functions can only access global variables
						if ( pFunction && pProgram->m_pVariables.m_pData[ index ]->iScope != AGK_VARIABLE_GLOBAL )
						{
							if ( !allowUnassignedVars )
							{
								uString error;
								error.Format( "\"%s\" is defined as a local variable so cannot be accessed here, and has not been assigned a value in this function", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
								pProgram->TokenError( error.GetStr(), tokenID );
								return 0;
							}
								
							index = -1;
						}
					}
					
					if ( index >= 0 )
					{
						pVar = pProgram->m_pVariables.m_pData[ index ];
						pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
						pProgram->GetToken( tokenID )->pExtraData = 0;
					}
					else
					{
						// if we didn't replace constants then don't treat them as variables
						if (!m_iReplacedConstants)
						{
							int constIndex = pProgram->FindConstant( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
							if ( constIndex >= 0 && pProgram->m_pConstants.m_pData[ constIndex ]->valid ) return 0;
						}

						// add undeclared variable, if allowed
						if ( pProgram->m_iOptionExplicit )
						{
							uString error;
							error.Format( "\"%s\" has not been defined", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
							pProgram->TokenError( error.GetStr(), tokenID );
							return 0;
						}

						if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
						{
							uString error;
							error.Format( "\"%s\" has not been defined as an array", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
							pProgram->TokenError( error.GetStr(), tokenID );
							return 0;
						}

						if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_DOT )
						{
							uString error;
							error.Format( "\"%s\" has not been defined as a type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
							pProgram->TokenError( error.GetStr(), tokenID );
							return 0;
						}
						
						// using an unitialised variable in the middle of an expression is likely to be a mistake, 
						// but might be a gosub that will be called after variable is defined
						if ( allowFunctions || pInstructions )
						{
							if ( pFunction && !allowUnassignedVars )
							{
								uString error;
								error.Format( "\"%s\" is undefined, or it is defined as a local variable in the outer scope", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
								pProgram->TokenError( error.GetStr(), tokenID );
								return 0;
							}

							// add the variable and check later if it was ever initialised
							stVariable newVar;
							newVar.iScope = pFunction ? AGK_VARIABLE_FUNC_LOCAL : AGK_VARIABLE_LOCAL;
							newVar.iTokenID = identToken;
							newVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw );
							newVar.sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig );

							newVar.iType = AGK_VARIABLE_INTEGER;
							if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '#' ) newVar.iType = AGK_VARIABLE_FLOAT;
							else if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '$' ) newVar.iType = AGK_VARIABLE_STRING;

							if ( pFunction ) 
							{
								index = pFunction->AddVariable( &newVar );
								pVar = pFunction->pFuncVars.m_pData[ index ];
								pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
								pProgram->GetToken( tokenID )->pExtraData = pFunction;
							}
							else 
							{
								index = pProgram->AddVariable( newVar );
								pVar = pProgram->m_pVariables.m_pData[ index ];
								pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
								pProgram->GetToken( tokenID )->pExtraData = 0;
							}
						}
						else
						{
							// exception for EndFunction parsing since we don't have all the information yet
							newVar.iScope = pFunction ? AGK_VARIABLE_FUNC_LOCAL : AGK_VARIABLE_LOCAL;
							newVar.iTokenID = tokenID;
							newVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw );
							newVar.sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig );
							newVar.iNumDimensions = 0;
							newVar.iVarID = 0;

							newVar.iType = AGK_VARIABLE_INTEGER;
							if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '#' ) newVar.iType = AGK_VARIABLE_FLOAT;
							else if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '$' ) newVar.iType = AGK_VARIABLE_STRING;

							pVar = &newVar;
						}
					}
				}

				if ( pVar->iNumDimensions > 0 )
				{
					// variable is an array
					if ( pInstructions )
					{
						stInstruction newInstruction;
						newInstruction.Construct( (pVar->iScope == AGK_VARIABLE_FUNC_LOCAL) ? AGKI_PUSH_LOCAL_ARRAY : AGKI_PUSH_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->iVarID, 0 );
						pInstructions->Add( &newInstruction );
					}

					int subConsumed = 0;
					int result = ParseArrayExpression( pFunction, pProgram, dataType, pVar, pInstructions, tokenID, subConsumed, allowUnassignedVars );
					if ( result <= 0 ) return result;
					tokenID += subConsumed;
					consumed += subConsumed;
				}
				else if ( pVar->iType == AGK_VARIABLE_TYPE )
				{
					// variable is a type
					if ( pInstructions )
					{
						stInstruction newInstruction;
						newInstruction.Construct( (pVar->iScope == AGK_VARIABLE_FUNC_LOCAL) ? AGKI_PUSH_LOCAL_TYPE : AGKI_PUSH_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->iVarID, 0 );
						pInstructions->Add( &newInstruction );
					}

					tokenID++;
					consumed++; // consume identifier

					if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_DOT )
					{
						pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_EXP_DOT;
						pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pTypes.m_pData[ pVar->iTypeIndex ];

						tokenID++;
						consumed++; // consume dot

						int subConsumed = 0;
						int result = ParseTypeExpression( pFunction, pProgram, dataType, pVar, pInstructions, tokenID, subConsumed, allowUnassignedVars );
						if ( result <= 0 ) return result;
						tokenID += subConsumed;
						consumed += subConsumed;
					}
					else
					{
						dataType = AGK_VARIABLE_TYPE | (pVar->iTypeIndex << 8);
					}
				}
				else
				{
					// normal variable
					if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
					{
						uString error;
						error.Format( "\"%s\" has been defined as a%s %s, not an array", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), getDataTypeName(pVar->iType)[0] == 'I' ? "n" : "", getDataTypeName(pVar->iType) );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}

					if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_DOT )
					{
						uString error;
						error.Format( "\"%s\" has been defined as a%s %s, not a type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), getDataTypeName(pVar->iType)[0] == 'I' ? "n" : "", getDataTypeName(pVar->iType) );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}

					tokenID++;
					consumed++; // consume identifier

					dataType = pVar->iType;

					AGKInstruction instruction = AGKI_NOP;
					if ( pVar->iScope == AGK_VARIABLE_FUNC_LOCAL )
					{
						switch( pVar->iType )
						{
							case AGK_VARIABLE_INTEGER: instruction = AGKI_PUSH_LOCAL_INT; break;
							case AGK_VARIABLE_FLOAT: instruction = AGKI_PUSH_LOCAL_FLOAT; break;
							case AGK_VARIABLE_STRING: instruction = AGKI_PUSH_LOCAL_STRING; break;
							default: Error( "Unrecognised variable type, shouldn't get here!", 0,0 );
						}
					}
					else
					{
						switch( pVar->iType )
						{
							case AGK_VARIABLE_INTEGER: instruction = AGKI_PUSH_VAR_INT; break;
							case AGK_VARIABLE_FLOAT: instruction = AGKI_PUSH_VAR_FLOAT; break;
							case AGK_VARIABLE_STRING: instruction = AGKI_PUSH_VAR_STRING; break;
							default: Error( "Unrecognised variable type, shouldn't get here!", 0,0 );
						}
					}

					if ( pInstructions )
					{
						stInstruction newInstruction;
						newInstruction.Construct( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pVar->iVarID, 0 );
						pInstructions->Add( &newInstruction );
					}
				}
			}

			break;
		}
		case AGK_TOKEN_UNARY_OP:
		{
			int op = convertUnaryOperator( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			int opPrec = operatorPrecedence( op );

			tokenID++;
			consumed++; // consume unary op

			int instructionCount = 0;
			if ( pInstructions ) instructionCount = pInstructions->m_iCount;

			int subConsumed = 0;
			int result = ParseExpression( pFunction, allowFunctions, opPrec, dataType, pInstructions, pProgram, tokenID, subConsumed, allowUnassignedVars );

			if ( pInstructions ) 
			{
				bool bContinue = true;
				if ( op == AGK_OPERATOR_NEG && pInstructions->m_iCount - instructionCount == 1 )
				{
					if ( pInstructions->m_pData[ pInstructions->m_iCount-1 ]->iInstruction == AGKI_PUSH_INT )
					{
						// optimization, replace push_int neg_int with push_int
						pInstructions->m_pData[ pInstructions->m_iCount-1 ]->i = -(pInstructions->m_pData[ pInstructions->m_iCount-1 ]->i);
						bContinue = false;
					}
					else if ( pInstructions->m_pData[ pInstructions->m_iCount-1 ]->iInstruction == AGKI_PUSH_FLOAT )
					{
						// optimization, replace push_float neg_float with push_float
						pInstructions->m_pData[ pInstructions->m_iCount-1 ]->f = -(pInstructions->m_pData[ pInstructions->m_iCount-1 ]->f);
						bContinue = false;
					}
				}

				if ( bContinue )
				{
					AGKInstruction instruction = convertToInstruction( op, dataType );
					if ( instruction == AGKI_NOP )
					{
						uString error;
						error.Format( "Cannot perform operation \"%s\" on data type \"%s\"", operatorName(op), getDataTypeName(dataType) );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}

					dataType = operatorDataType( op, dataType );

					stInstruction newInstruction;
					newInstruction.Construct( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					pInstructions->Add( &newInstruction );
				}
			}

			tokenID += subConsumed;
			consumed += subConsumed;

			break;
		}
		case AGK_TOKEN_OPEN_BRACKET:
		{
			consumed++; // consume bracket
			tokenID++;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, allowFunctions, 0, dataType, pInstructions, pProgram, tokenID, subConsumed, allowUnassignedVars );
			if ( result <= 0 ) return result;

			tokenID += subConsumed;
			consumed += subConsumed;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", expected close bracket", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume close bracket
			tokenID++;
			
			break;
		}
		case AGK_TOKEN_INTEGER:
		{
			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pProgram->GetToken( tokenID )->iValueInt, 0 );
				pInstructions->Add( &newInstruction );
			}

			consumed++; // consume integer
			tokenID++;
			dataType = AGK_VARIABLE_INTEGER;
			break;
		}
		case AGK_TOKEN_FLOAT:
		{
			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				newInstruction.f = pProgram->GetToken( tokenID )->fValueFloat;
				pInstructions->Add( &newInstruction );
			}

			consumed++; // consume float
			tokenID++;
			dataType = AGK_VARIABLE_FLOAT;
			break;
		}
		case AGK_TOKEN_STRING:
		{
			if ( pInstructions )
			{
				stInstruction newInstruction;
				newInstruction.Construct( AGKI_PUSH_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pProgram->GetToken( tokenID )->iStringIndex, 0 );
				pInstructions->Add( &newInstruction );
			}

			consumed++; // consume string
			tokenID++;
			dataType = AGK_VARIABLE_STRING;
			break;
		}
		default:
		{
			uString error;
			error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}

	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
	{
		// valid ending
		//consumed += 1; // don't consume eol, let caller do it
		return 1;
	}

	DynArrayObject<stInstruction> pSubInstructions;
	while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_BINARY_OP || pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMPARISON || pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EQUALS )
	{
		pSubInstructions.Clear();

		int op = convertBinaryOperator( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
		int opPrec = operatorPrecedence( op );
		if ( (opPrec <= prevPrec && operatorLeftAssoc( op ))
		  || opPrec < prevPrec )
		{
			return 1;
		}
		
		tokenID++;
		consumed++; // consume binary op

		int subType = 0;
		int subConsumed = 0;
		int result = ParseExpression( pFunction, allowFunctions, opPrec, subType, (pInstructions ? &pSubInstructions : 0), pProgram, tokenID, subConsumed, allowUnassignedVars );
		if ( result <= 0 ) return result;

		if ( (subType & 0x1f) != (dataType & 0x1f) )
		{
			if ( dataType == AGK_VARIABLE_INTEGER && subType == AGK_VARIABLE_FLOAT )
			{
				if ( pInstructions )
				{
					if ( pInstructions->m_iCount == 1 && pInstructions->m_pData[0]->iInstruction == AGKI_PUSH_INT )
					{
						// optimization, convert push_int in to push_float
						pInstructions->m_pData[0]->f = (float) pInstructions->m_pData[0]->i;
						pInstructions->m_pData[0]->iInstruction = AGKI_PUSH_FLOAT;
					}
					else
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pInstructions->Add( &newInstruction );
					}
				}
				dataType = AGK_VARIABLE_FLOAT;
			}
			else if ( dataType == AGK_VARIABLE_FLOAT && subType == AGK_VARIABLE_INTEGER )
			{
				if ( pInstructions )
				{
					if ( pSubInstructions.m_iCount == 1 && pSubInstructions.m_pData[0]->iInstruction == AGKI_PUSH_INT )
					{
						// optimization, convert push_int in to push_float
						pSubInstructions.m_pData[0]->f = (float) pSubInstructions.m_pData[0]->i;
						pSubInstructions.m_pData[0]->iInstruction = AGKI_PUSH_FLOAT;
					}
					else
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pSubInstructions.Add( &newInstruction );
					}
				}
				subType = AGK_VARIABLE_FLOAT;
			}
			else
			{
				uString error;
				error.Format( "Incompatible types \"%s\" and \"%s\"", getDataTypeName(dataType&0x1f), getDataTypeName(subType&0x1f) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}

		if ( pInstructions )
		{
			for ( int i = 0; i < pSubInstructions.m_iCount; i++ )
			{
				pInstructions->Add( pSubInstructions.m_pData[ i ] );
			}
		}

		AGKInstruction instruction = convertToInstruction( op, dataType & 0x1f );
		if ( instruction == AGKI_NOP )
		{
			uString error;
			error.Format( "Cannot perform operation \"%s\" on data type \"%s\"", operatorName(op), getDataTypeName(dataType&0x1f) );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		dataType = operatorDataType( op, dataType );

		if ( pInstructions )
		{
			stInstruction newInstruction;
			newInstruction.Construct( instruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			pInstructions->Add( &newInstruction );
		}
		
		tokenID += subConsumed;
		consumed += subConsumed;
	}

	return 1;
}

int Parser2::ParseFunctionStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed )
{
	int identToken = tokenID;
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		uString error;
		error.Format( "Unexpected token \"%s\", function call must start with an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	consumed++; // consume identifier
	tokenID++;

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
	{
		uString error;
		error.Format( "Unexpected token \"%s\", function call must have open and close brackets", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	consumed++; // consume open bracket
	tokenID++;

	// count parameters and parameter types
	int numParams = 0;
	int dataTypes[ 32 ];
	DynArrayObject<stInstruction> parameterInstructions[ 32 ];

	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CLOSE_BRACKET )
	{
		consumed++; // consume close bracket
		tokenID++;
	}
	else
	{
		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", first function parameter must be on the same line as the function call", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", function parameter cannot be empty", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		do
		{
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA ) 
			{
				tokenID++;
				consumed++;
			}
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL ) 
			{
				tokenID++;
				consumed++;
			}

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", function parameters must not have more than one new line between them", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			if ( numParams >= 32 )
			{
				pProgram->TokenError( "Too many parmaeters, a maximum of 32 are supported", tokenID );
				return 0;
			}
			
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataTypes[numParams], parameterInstructions+numParams, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			numParams++;
			tokenID += subConsumed;
			consumed += subConsumed;
		} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", close bracket must be on the same line as the last parameter", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", function call must end with a close bracket", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		consumed++;
		tokenID++; 
	}

#ifndef AGK_STATIC_LIB
	// hard coded functions for compiler
	if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "compile" ) == 0 )
	{
		if ( numParams != 1 || dataTypes[0] != AGK_VARIABLE_STRING )
		{
			pProgram->TokenError( "\"Compile\" function only accepts 1 parameter which must be of type String", identToken );
			return 0;
		}

		if ( !pProgram->IsSkippingInstructions() )
		{
			for ( int j = 0; j < parameterInstructions[ 0 ].m_iCount; j++ )
			{
				pProgram->m_pInstructions.Add( parameterInstructions[ 0 ].m_pData[ j ] );
			}
		}
		
		pProgram->AddInstruction( AGKI_COMPILE, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
		
		// dump the return type for statements
		pProgram->AddInstruction( AGKI_POP_VAR, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
		return 1;
	}

	if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "getcompileerrors" ) == 0 )
	{
		if ( numParams != 0 )
		{
			pProgram->TokenError( "\"GetCompileErrors\" function does not accept any parameters", identToken );
			return 0;
		}

		pProgram->AddInstruction( AGKI_GET_COMPILE_ERRORS, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
		
		// dump the return type for statements
		pProgram->AddInstruction( AGKI_POP_STRING, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
		return 1;
	}

	if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "runcode" ) == 0 )
	{
		if ( numParams != 1 || dataTypes[0] != AGK_VARIABLE_STRING )
		{
			pProgram->TokenError( "\"RunCode\" function only accepts 1 parameter which must be of type String", identToken );
			return 0;
		}

		if ( !pProgram->IsSkippingInstructions() )
		{
			for ( int j = 0; j < parameterInstructions[ 0 ].m_iCount; j++ )
			{
				pProgram->m_pInstructions.Add( parameterInstructions[ 0 ].m_pData[ j ] );
			}
		}
		
		pProgram->AddInstruction( AGKI_RUN_CODE, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
		return 1;
	}
#endif

	// find function or command
	int funcIndex = pProgram->FindFunction( pProgram->GetToken( identToken )->sValueRaw.GetStr() );
	if ( funcIndex >= 0 )
	{
		// user defined function
		stFunction* pIdentFunction = pProgram->m_pFunctions.m_pData[ funcIndex ];
		if ( !pIdentFunction->valid ) return 0;

		pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_USER_FUNCTION;
		pProgram->GetToken( identToken )->pExtraData = pIdentFunction;

		// check parameter count
		if ( pIdentFunction->iNumParams != numParams )
		{
			uString error;
			error.Format( "Number of parameters does not match function declaration, expected %d parameters", pIdentFunction->iNumParams );
			pProgram->TokenError( error.GetStr(), identToken );
			return 0;
		}

		if ( pIdentFunction->iReturnType > 0 )
		{
			// add space on the stack for the return value
			int returnType = pIdentFunction->iReturnType & 0x1f;
			int returnTypeType = pIdentFunction->iReturnType >> 8;
			switch( returnType )
			{
				case AGK_VARIABLE_INTEGER:
				case AGK_VARIABLE_FLOAT:
				{
					pProgram->AddInstruction( AGKI_PUSH_BLANK, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 1, 0 );
					break;
				}
				case AGK_VARIABLE_STRING:
				{
					pProgram->AddInstruction( AGKI_PUSH_BLANK, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 1 );
					break;
				}
				case AGK_VARIABLE_TYPE: 
				{
					pProgram->AddInstruction( AGKI_PUSH_TEMP_TYPE, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, returnTypeType, 0 );
					break;
				}
				case AGK_VARIABLE_ARRAY_INT: 
				case AGK_VARIABLE_ARRAY_FLOAT:
				case AGK_VARIABLE_ARRAY_STRING:
				case AGK_VARIABLE_ARRAY_TYPE:
				{
					pProgram->AddInstruction( AGKI_PUSH_TEMP_ARRAY, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
					break;
				}
				default: Error( "Unrecognised variable type", 0, 0 );
			}
		}

		// check parameter types
		for ( int i = 0; i < numParams; i++ )
		{
			int type1 = pIdentFunction->iParamsTypes[ i ] & 0x1f;
			int type2 = dataTypes[ i ] & 0x1f;
			if ( type1 != type2 )
			{
				if ( dataTypes[ i ] == AGK_VARIABLE_INTEGER && pIdentFunction->iParamsTypes[ i ] == AGK_VARIABLE_FLOAT )
				{
					if ( parameterInstructions[ i ].m_iCount == 1 && parameterInstructions[ i ].m_pData[0]->iInstruction == AGKI_PUSH_INT )
					{
						// optimization, convert push_int in to push_float
						parameterInstructions[ i ].m_pData[0]->f = (float) parameterInstructions[ i ].m_pData[0]->i;
						parameterInstructions[ i ].m_pData[0]->iInstruction = AGKI_PUSH_FLOAT;
					}
					else
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
						parameterInstructions[ i ].Add( &newInstruction );
					}
					dataTypes[ i ] = AGK_VARIABLE_FLOAT;
				}
				else if ( dataTypes[ i ] == AGK_VARIABLE_FLOAT && pIdentFunction->iParamsTypes[ i ] == AGK_VARIABLE_INTEGER )
				{
					stInstruction newInstruction;
					newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
					parameterInstructions[ i ].Add( &newInstruction );
					dataTypes[ i ] = AGK_VARIABLE_INTEGER;
				}
				else
				{
					uString error;
					error.Format( "Cannot convert type \"%s\" to \"%s\"", getDataTypeName(dataTypes[i]), getDataTypeName(pIdentFunction->iParamsTypes[i]) );
					pProgram->TokenError( error.GetStr(), identToken );
					return 0;
				}
			}
			
			if ( type1 == AGK_VARIABLE_TYPE || type1 == AGK_VARIABLE_ARRAY_TYPE )
			{
				// check type index
				type1 = pIdentFunction->iParamsTypes[ i ] >> 8;
				type2 = dataTypes[ i ] >> 8;
				if ( type1 != type2 )
				{
					uString error;
					error.Format( "Cannot convert type \"%s\" to \"%s\"", pProgram->m_pTypes.m_pData[ type2 ]->sNameOrig.GetStr(), pProgram->m_pTypes.m_pData[ type1 ]->sNameOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), identToken );
					return 0;
				}
			}
			else
			{
				// check array dimensions
				type1 = (pIdentFunction->iParamsTypes[ i ] >> 5) & 7;
				type2 = (dataTypes[ i ] >> 5) & 7;
				if ( type1 != type2 )
				{
					uString error;
					error.Format( "Cannot convert an array with %d dimensions to one with %d dimensions", type2, type1 );
					pProgram->TokenError( error.GetStr(), identToken );
					return 0;
				}
			}

			if ( !pProgram->IsSkippingInstructions() )
			{
				for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
				{
					pProgram->m_pInstructions.Add( parameterInstructions[ i ].m_pData[ j ] );
				}
			}
		}

		// add the function call instruction
		pProgram->AddInstruction( AGKI_FUNCTION_CALL, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, funcIndex, 0 );

		// pop parameters off the stack
		for ( int i = numParams-1; i >= 0; i-- )
		{
			AGKInstruction instruction = AGKI_NOP;
			int type = dataTypes[ i ] & 0x1f;
			switch( type )
			{
				case AGK_VARIABLE_INTEGER: instruction = AGKI_POP_VAR; break;
				case AGK_VARIABLE_FLOAT: instruction = AGKI_POP_VAR; break;
				case AGK_VARIABLE_STRING: instruction = AGKI_POP_STRING; break;
				case AGK_VARIABLE_TYPE: instruction = AGKI_POP_TYPE; break;
				case AGK_VARIABLE_ARRAY_INT: 
				case AGK_VARIABLE_ARRAY_FLOAT: 
				case AGK_VARIABLE_ARRAY_STRING: 
				case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_POP_ARRAY; break;
				default: Error( "Unrecognised variable type", 0, 0 );
			}

			pProgram->AddInstruction( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
		}

		// dump the return type for statements
		if ( pIdentFunction->iReturnType > 0 )
		{
			AGKInstruction instruction = AGKI_NOP;
			int type = pIdentFunction->iReturnType & 0x1f;
			switch( type )
			{
				case AGK_VARIABLE_INTEGER: instruction = AGKI_POP_VAR; break;
				case AGK_VARIABLE_FLOAT: instruction = AGKI_POP_VAR; break;
				case AGK_VARIABLE_STRING: instruction = AGKI_POP_STRING; break;
				case AGK_VARIABLE_TYPE: instruction = AGKI_POP_TYPE; break;
				case AGK_VARIABLE_ARRAY_INT: 
				case AGK_VARIABLE_ARRAY_FLOAT: 
				case AGK_VARIABLE_ARRAY_STRING: 
				case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_POP_ARRAY; break;
				default: Error( "Unrecognised variable type", 0, 0 );
			}

			pProgram->AddInstruction( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
		}
	}
	else
	{
		// not a user defined function 
		int commandID = pProgram->FindCommandName( pProgram->GetToken( identToken )->sValueRaw.GetStr() );
		if ( commandID < 0 )
		{
			uString error;
			error.Format( "\"%s\" is not a recognised function or command", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), identToken );
			return 0;
		}

		pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_AGK_COMMAND;
		pProgram->GetToken( identToken )->pExtraData = 0;

		if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "arsetup" ) == 0 ) pProgram->m_iFlags |= AGK_PROGRAM_FLAG_USES_AR;
		else if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "playvideotoimage" ) == 0 ) pProgram->m_iFlags |= AGK_PROGRAM_FLAG_USES_EXTERN_TEX;
		else if ( pProgram->GetToken( identToken )->sValueRaw.CompareTo( "setdevicecameratoimage" ) == 0 ) pProgram->m_iFlags |= AGK_PROGRAM_FLAG_USES_EXTERN_TEX;

		stCommand findCommand;
		findCommand.iNumParams = numParams;
		findCommand.iReturnType = 0;
		strcpy( findCommand.szCommandName, pProgram->GetToken( identToken )->sValueRaw.GetStr() );
		for ( int i = 0; i < numParams; i++ ) findCommand.iParamsTypes[i] = dataTypes[i] & 0x1f;
		funcIndex = pProgram->FindCommand( &findCommand );
		if ( funcIndex >= 0 )
		{
			// native AGK command
			if ( !pProgram->IsSkippingInstructions() )
			{
				for ( int i = 0; i < numParams; i++ )
				{
					for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
					{
						pProgram->m_pInstructions.Add( parameterInstructions[ i ].m_pData[ j ] );
					}
				}
			}

			// add command instruction, special cases for Sync and Swap
			if ( strcmp( findCommand.szCommandName, "sync" ) == 0 )
			{
				pProgram->AddInstruction( AGKI_REAL_SYNC, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
			}
			else if ( strcmp( findCommand.szCommandName, "swap" ) == 0 )
			{
				pProgram->AddInstruction( AGKI_REAL_SWAP, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
			}
			else
			{
				pProgram->AddInstruction( pProgram->m_pCommands[ funcIndex ].instructionID, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
			}

			// dump the return type for statements
			if ( pProgram->m_pCommands[ funcIndex ].iReturnType > 0 )
			{
				AGKInstruction instruction = AGKI_NOP;
				int type = pProgram->m_pCommands[ funcIndex ].iReturnType & 0x1f;
				switch( type )
				{
					case AGK_VARIABLE_INTEGER: instruction = AGKI_POP_VAR; break;
					case AGK_VARIABLE_FLOAT: instruction = AGKI_POP_VAR; break;
					case AGK_VARIABLE_STRING: instruction = AGKI_POP_STRING; break;
					case AGK_VARIABLE_TYPE: instruction = AGKI_POP_TYPE; break;
					case AGK_VARIABLE_ARRAY_INT: 
					case AGK_VARIABLE_ARRAY_FLOAT: 
					case AGK_VARIABLE_ARRAY_STRING: 
					case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_POP_ARRAY; break;
					default: Error( "Unrecognised variable type", 0, 0 );
				}

				pProgram->AddInstruction( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
			}
		}
		else 
		{
			funcIndex = pProgram->FindCommandCompatible( &findCommand );
			if ( funcIndex >= 0 )
			{
				// AGK command with casting
				// check parameter types
				for ( int i = 0; i < numParams; i++ )
				{
					if ( pProgram->m_pCommands[ funcIndex ].iParamsTypes[ i ] != dataTypes[ i ] )
					{
						if ( dataTypes[ i ] == AGK_VARIABLE_INTEGER && pProgram->m_pCommands[ funcIndex ].iParamsTypes[ i ] == AGK_VARIABLE_FLOAT )
						{
							if ( parameterInstructions[ i ].m_iCount == 1 && parameterInstructions[ i ].m_pData[0]->iInstruction == AGKI_PUSH_INT )
							{
								// optimization, convert push_int in to push_float
								parameterInstructions[ i ].m_pData[0]->f = (float) parameterInstructions[ i ].m_pData[0]->i;
								parameterInstructions[ i ].m_pData[0]->iInstruction = AGKI_PUSH_FLOAT;
							}
							else
							{
								stInstruction newInstruction;
								newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
								parameterInstructions[ i ].Add( &newInstruction );
							}
							dataTypes[ i ] = AGK_VARIABLE_FLOAT;
						}
						else if ( dataTypes[ i ] == AGK_VARIABLE_FLOAT && pProgram->m_pCommands[ funcIndex ].iParamsTypes[ i ] == AGK_VARIABLE_INTEGER )
						{
							stInstruction newInstruction;
							newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
							parameterInstructions[ i ].Add( &newInstruction );
							dataTypes[ i ] = AGK_VARIABLE_INTEGER;
						}
						else
						{
							uString error;
							error.Format( "Cannot convert type \"%s\" to \"%s\"", getDataTypeName(dataTypes[i]), getDataTypeName(pProgram->m_pCommands[ funcIndex ].iParamsTypes[i]) );
							pProgram->TokenError( error.GetStr(), identToken );
							return 0;
						}
					}

					if ( !pProgram->IsSkippingInstructions() )
					{
						for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
						{
							pProgram->m_pInstructions.Add( parameterInstructions[ i ].m_pData[ j ] );
						}
					}
				}

				// add command instruction
				pProgram->AddInstruction( pProgram->m_pCommands[ funcIndex ].instructionID, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );

				// dump the return type for statements
				if ( pProgram->m_pCommands[ funcIndex ].iReturnType > 0 )
				{
					AGKInstruction instruction = AGKI_NOP;
					int type = pProgram->m_pCommands[ funcIndex ].iReturnType & 0x1f;
					switch( type )
					{
						case AGK_VARIABLE_INTEGER: instruction = AGKI_POP_VAR; break;
						case AGK_VARIABLE_FLOAT: instruction = AGKI_POP_VAR; break;
						case AGK_VARIABLE_STRING: instruction = AGKI_POP_STRING; break;
						case AGK_VARIABLE_TYPE: instruction = AGKI_POP_TYPE; break;
						case AGK_VARIABLE_ARRAY_INT: 
						case AGK_VARIABLE_ARRAY_FLOAT: 
						case AGK_VARIABLE_ARRAY_STRING: 
						case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_POP_ARRAY; break;
						default: Error( "Unrecognised variable type", 0, 0 );
					}

					pProgram->AddInstruction( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
				}
			}
			else
			{
				if ( findCommand.iNumParams > 0 )
				{
					uString error;
					error.Format( "\"%s\" does not accept the parameters (", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
					for ( int i = 0; i < findCommand.iNumParams; i++ )
					{
						error.Append( getDataTypeName(findCommand.iParamsTypes[i]) );
						error.Append( ", " );
					}
					error.Trunc2(2);
					error.Append( ")" );
					pProgram->TokenError( error.GetStr(), identToken );
					return 0;
				}
				else
				{
					uString error;
					error.Format( "\"%s\" expects %d parameters", pProgram->GetToken( identToken )->sValueRawOrig.GetStr(), pProgram->m_pCommands[ commandID ].iNumParams );
					pProgram->TokenError( error.GetStr(), identToken );
					return 0;
				}
			}
		}
	}

	return 1;
}

int Parser2::ParsePluginFunctionStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed, int pluginIndex )
{
	int identToken = tokenID;
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		uString error;
		error.Format( "Unexpected token \"%s\", plugin function call must start with an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_PLUGIN_FUNC;
	pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pPlugins.m_pData[ pluginIndex ];

	consumed++; // consume identifier
	tokenID++;

	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
	{
		uString error;
		error.Format( "Unexpected token \"%s\", plugin function call must have open and close brackets", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	consumed++; // consume open bracket
	tokenID++;

	// count parameters and parameter types
	int numParams = 0;
	int dataTypes[ 32 ];
	DynArrayObject<stInstruction> parameterInstructions[ 32 ];

	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CLOSE_BRACKET )
	{
		consumed++; // consume close bracket
		tokenID++;
	}
	else
	{
		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", first function parameter must be on the same line as the function call", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", function parameter cannot be empty", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		do
		{
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA ) 
			{
				tokenID++;
				consumed++;
			}
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL ) 
			{
				tokenID++;
				consumed++;
			}

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", function parameters must not have more than one new line between them", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			if ( numParams >= 32 )
			{
				pProgram->TokenError( "Too many parmaeters, a maximum of 32 are supported", tokenID );
				return 0;
			}
			
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataTypes[numParams], parameterInstructions+numParams, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			numParams++;
			tokenID += subConsumed;
			consumed += subConsumed;
		} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", close bracket must be on the same line as the last parameter", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", function call must end with a close bracket", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		consumed++;
		tokenID++; 
	}

	// not a user defined function 
	int commandID = pProgram->FindPluginCommandName( pluginIndex, pProgram->GetToken( identToken )->sValueRaw.GetStr() );
	if ( commandID < 0 )
	{
		uString error;
		error.Format( "\"%s\" is not a recognised command for that plugin", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	stPluginCommand findCommand;
	findCommand.iNumParams = numParams;
	findCommand.iReturnType = 0;
	findCommand.sTier1Name.SetStr( pProgram->GetToken( identToken )->sValueRaw );
	for ( int i = 0; i < numParams; i++ ) findCommand.iParamsTypes[i] = dataTypes[i] & 0x1f;
	int funcIndex = pProgram->FindPluginCommand( pluginIndex, &findCommand );
	if ( funcIndex >= 0 )
	{
		// native plugin command
		if ( !pProgram->IsSkippingInstructions() )
		{
			for ( int i = 0; i < numParams; i++ )
			{
				for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
				{
					pProgram->m_pInstructions.Add( parameterInstructions[ i ].m_pData[ j ] );
				}
			}
		}

		stPluginCommand *pPluginCommand = pProgram->m_pPlugins.m_pData[ pluginIndex ]->m_pCommands + funcIndex;
		pPluginCommand->iFlags |= AGK_PLUGIN_COMMAND_USED;

		// add plugin command instruction
		pProgram->AddInstruction( AGKI_PLUGIN_COMMAND, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pluginIndex, funcIndex );
		
		// dump the return type for statements
		if ( pPluginCommand->iReturnType > 0 )
		{
			AGKInstruction instruction = AGKI_NOP;
			int type = pPluginCommand->iReturnType & 0x1f;
			switch( type )
			{
				case AGK_VARIABLE_INTEGER: instruction = AGKI_POP_VAR; break;
				case AGK_VARIABLE_FLOAT: instruction = AGKI_POP_VAR; break;
				case AGK_VARIABLE_STRING: instruction = AGKI_POP_STRING; break;
				case AGK_VARIABLE_TYPE: instruction = AGKI_POP_TYPE; break;
				case AGK_VARIABLE_ARRAY_INT: 
				case AGK_VARIABLE_ARRAY_FLOAT: 
				case AGK_VARIABLE_ARRAY_STRING: 
				case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_POP_ARRAY; break;
				default: Error( "Unrecognised variable type", 0, 0 );
			}

			pProgram->AddInstruction( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
		}
	}
	else 
	{
		funcIndex = pProgram->FindPluginCommandCompatible( pluginIndex, &findCommand );
		if ( funcIndex >= 0 )
		{
			stPluginCommand *pPluginCommand = pProgram->m_pPlugins.m_pData[ pluginIndex ]->m_pCommands + funcIndex;
			pPluginCommand->iFlags |= AGK_PLUGIN_COMMAND_USED;

			// plugin command with casting
			// check parameter types
			for ( int i = 0; i < numParams; i++ )
			{
				if ( pPluginCommand->iParamsTypes[ i ] != dataTypes[ i ] )
				{
					if ( dataTypes[ i ] == AGK_VARIABLE_INTEGER && pPluginCommand->iParamsTypes[ i ] == AGK_VARIABLE_FLOAT )
					{
						if ( parameterInstructions[ i ].m_iCount == 1 && parameterInstructions[ i ].m_pData[0]->iInstruction == AGKI_PUSH_INT )
						{
							// optimization, convert push_int in to push_float
							parameterInstructions[ i ].m_pData[0]->f = (float) parameterInstructions[ i ].m_pData[0]->i;
							parameterInstructions[ i ].m_pData[0]->iInstruction = AGKI_PUSH_FLOAT;
						}
						else
						{
							stInstruction newInstruction;
							newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
							parameterInstructions[ i ].Add( &newInstruction );
						}
						dataTypes[ i ] = AGK_VARIABLE_FLOAT;
					}
					else if ( dataTypes[ i ] == AGK_VARIABLE_FLOAT && pPluginCommand->iParamsTypes[ i ] == AGK_VARIABLE_INTEGER )
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
						parameterInstructions[ i ].Add( &newInstruction );
						dataTypes[ i ] = AGK_VARIABLE_INTEGER;
					}
					else
					{
						uString error;
						error.Format( "Cannot convert type \"%s\" to \"%s\"", getDataTypeName(dataTypes[i]), getDataTypeName(pPluginCommand->iParamsTypes[i]) );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}
				}

				if ( !pProgram->IsSkippingInstructions() )
				{
					for ( int j = 0; j < parameterInstructions[ i ].m_iCount; j++ )
					{
						pProgram->m_pInstructions.Add( parameterInstructions[ i ].m_pData[ j ] );
					}
				}
			}

			// add command instruction
			pProgram->AddInstruction( AGKI_PLUGIN_COMMAND, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pluginIndex, funcIndex );

			// dump the return type for statements
			if ( pPluginCommand->iReturnType > 0 )
			{
				AGKInstruction instruction = AGKI_NOP;
				int type = pPluginCommand->iReturnType & 0x1f;
				switch( type )
				{
					case AGK_VARIABLE_INTEGER: instruction = AGKI_POP_VAR; break;
					case AGK_VARIABLE_FLOAT: instruction = AGKI_POP_VAR; break;
					case AGK_VARIABLE_STRING: instruction = AGKI_POP_STRING; break;
					case AGK_VARIABLE_TYPE: instruction = AGKI_POP_TYPE; break;
					case AGK_VARIABLE_ARRAY_INT: 
					case AGK_VARIABLE_ARRAY_FLOAT: 
					case AGK_VARIABLE_ARRAY_STRING: 
					case AGK_VARIABLE_ARRAY_TYPE: instruction = AGKI_POP_ARRAY; break;
					default: Error( "Unrecognised variable type", 0, 0 );
				}

				pProgram->AddInstruction( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, 0, 0 );
			}
		}
		else
		{
			if ( findCommand.iNumParams > 0 )
			{
				uString error;
				error.Format( "\"%s\" does not accept the parameters (", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
				for ( int i = 0; i < findCommand.iNumParams; i++ )
				{
					error.Append( getDataTypeName(findCommand.iParamsTypes[i]) );
					error.Append( ", " );
				}
				error.Trunc2(2);
				error.Append( ")" );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			else
			{
				uString error;
				error.Format( "\"%s\" expects %d parameters", pProgram->GetToken( identToken )->sValueRawOrig.GetStr(), pProgram->m_pPlugins.m_pData[ pluginIndex ]->m_pCommands[ funcIndex ].iNumParams );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}
	}

	return 1;
}

int Parser2::ParseIdentifierStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed )
{
	// check if identifier is a function or command
	if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_BRACKET )
	{
		return ParseFunctionStatement( pFunction, pProgram, tokenID, consumed );
	}

	// check for plugin command
	int pluginIndex = pProgram->FindPlugin( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
	if ( pluginIndex >= 0 )
	{
		pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_PLUGIN;
		pProgram->GetToken( tokenID )->pExtraData = pProgram->m_pPlugins.m_pData[ pluginIndex ];

		if ( pProgram->GetToken( tokenID+1 )->iType != AGK_TOKEN_DOT )
		{
			pProgram->TokenError( "Plugin reference must be followed by a dot and then the command to call", tokenID );
			return 0;
		}

		pProgram->GetToken( tokenID+1 )->iTypeExtra = AGK_TOKEN_EXTRA_PLUGIN_DOT;
		pProgram->GetToken( tokenID+1 )->pExtraData = pProgram->m_pPlugins.m_pData[ pluginIndex ];

		// consume pluginref and dot
		consumed += 2;
		tokenID += 2;
		return ParsePluginFunctionStatement( pFunction, pProgram, tokenID, consumed, pluginIndex );
	}

	int identToken = tokenID;
	
	// confirm identifier is a variable
	stVariable *pVar = 0;
	stVariable findVar;
	findVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );

	// check function local first, if applicable
	if ( pFunction )
	{
		int index = pFunction->pFuncVars.Find( &findVar );
		if ( index >= 0 ) 
		{
			pVar = pFunction->pFuncVars.m_pData[ index ];

			pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
			pProgram->GetToken( identToken )->pExtraData = pFunction;
		}
	}

	if ( !pVar )
	{
		int index = pProgram->m_pVariables.Find( &findVar );
		if ( index >= 0 )
		{
			if ( pFunction )
			{
				// functions can only access global variables
				if ( pProgram->m_pVariables.m_pData[ index ]->iScope != AGK_VARIABLE_GLOBAL ) 
				{
					/*
					uString error;
					error.Format( "\"%s\" has the same name as a local variable outside this scope, it will be redefined for this function", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
					pProgram->TokenWarning( error.GetStr(), identToken );
					//return 0;
					*/
					index = -1;
				}
			}
		}
		
		if ( index >= 0 )
		{
			pVar = pProgram->m_pVariables.m_pData[ index ];

			pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
			pProgram->GetToken( identToken )->pExtraData = 0;
		}
		else
		{
			if ( pProgram->m_iOptionExplicit 
			  || pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_SQR_BRACKET
			  || pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_DOT )
			{
				uString error;
				error.Format( "\"%s\" has not been defined", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), identToken );
				return 0;
			}
			
			// if not using option explicit we must add the variable
			stVariable newVar;
			newVar.iScope = pFunction ? AGK_VARIABLE_FUNC_LOCAL : AGK_VARIABLE_LOCAL;
			newVar.iTokenID = identToken;
			newVar.sName.SetStr( pProgram->GetToken( identToken )->sValueRaw );
			newVar.sNameOrig.SetStr( pProgram->GetToken( identToken )->sValueRawOrig );

			newVar.iType = AGK_VARIABLE_INTEGER;
			if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '#' ) newVar.iType = AGK_VARIABLE_FLOAT;
			else if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '$' ) newVar.iType = AGK_VARIABLE_STRING;

			if ( pFunction ) 
			{
				index = pFunction->AddVariable( &newVar );
				pVar = pFunction->pFuncVars.m_pData[ index ];

				pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
				pProgram->GetToken( identToken )->pExtraData = pFunction;
			}
			else 
			{
				index = pProgram->AddVariable( newVar );
				pVar = pProgram->m_pVariables.m_pData[ index ];

				pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
				pProgram->GetToken( identToken )->pExtraData = 0;
			}
		}
	}
	
	if ( pVar->iNumDimensions > 0 )
	{
		// variable is an array
		pProgram->AddInstruction( (pVar->iScope == AGK_VARIABLE_FUNC_LOCAL) ? AGKI_PUSH_LOCAL_ARRAY : AGKI_PUSH_ARRAY, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pVar->iVarID, 0 );
		
		int subConsumed = 0;
		int result = ParseArrayStatement( pFunction, pProgram, pVar, tokenID, subConsumed );
		if ( result <= 0 ) return result;
		tokenID += subConsumed;
		consumed += subConsumed;

		return 1;
	}
	else if ( pVar->iType == AGK_VARIABLE_TYPE )
	{
		// variable is a type
		pProgram->AddInstruction( (pVar->iScope == AGK_VARIABLE_FUNC_LOCAL) ? AGKI_PUSH_LOCAL_TYPE : AGKI_PUSH_TYPE, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pVar->iVarID, 0 );

		tokenID++;
		consumed++; // consume identifier

		int subConsumed = 0;
		int result = ParseTypeStatement( pFunction, pProgram, pVar, tokenID, subConsumed );
		if ( result <= 0 ) return result;
		tokenID += subConsumed;
		consumed += subConsumed;

		return 1;
	}
	else
	{
		if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_SQR_BRACKET )
		{
			uString error;
			error.Format( "\"%s\" has been defined as a%s %s, not an array", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), getDataTypeName(pVar->iType)[0] == 'I' ? "n" : "", getDataTypeName(pVar->iType) );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_DOT )
		{
			uString error;
			error.Format( "\"%s\" has been defined as a%s %s, not a type", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), getDataTypeName(pVar->iType)[0] == 'I' ? "n" : "", getDataTypeName(pVar->iType) );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		tokenID++;
		consumed++; // consumed identifier

		// just a normal variable
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", variable must be followed by an assignment", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}

		pVar->iInitialised = 1;
		
		tokenID++;
		consumed++; // consumed equals

		int subConsumed = 0;
		int dataType = 0;
		int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
		if ( result <= 0 ) return result;
		tokenID += subConsumed;
		consumed += subConsumed;

		// no types here, handled earlier
		if ( dataType != pVar->iType )
		{
			if ( dataType == AGK_VARIABLE_INTEGER && pVar->iType == AGK_VARIABLE_FLOAT )
			{
				// possible optimization, convert push_int to push_float
				pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else if ( dataType == AGK_VARIABLE_FLOAT && pVar->iType == AGK_VARIABLE_INTEGER )
			{
				pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else
			{
				uString error;
				error.Format( "Cannot assign %s to %s", getDataTypeName(dataType), getDataTypeName(pVar->iType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}

		AGKInstruction instruction = AGKI_NOP;
		if ( pVar->iScope == AGK_VARIABLE_FUNC_LOCAL )
		{
			switch( pVar->iType )
			{
				case AGK_VARIABLE_INTEGER: instruction = AGKI_STORE_LOCAL_INT; break;
				case AGK_VARIABLE_FLOAT: instruction = AGKI_STORE_LOCAL_FLOAT; break;
				case AGK_VARIABLE_STRING: instruction = AGKI_STORE_LOCAL_STRING; break;
				default: Error( "Unrecognised variable type, shouldn't get here!", 0,0 ); return 0;
			}
		}
		else
		{
			switch( pVar->iType )
			{
				case AGK_VARIABLE_INTEGER: instruction = AGKI_STORE_INT; break;
				case AGK_VARIABLE_FLOAT: instruction = AGKI_STORE_FLOAT; break;
				case AGK_VARIABLE_STRING: instruction = AGKI_STORE_STRING; break;
				default: Error( "Unrecognised variable type, shouldn't get here!", 0,0 ); return 0;
			}
		}
		pProgram->AddInstruction( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pVar->iVarID, 0 );

		return 1;
	}
}

int Parser2::ParseIncDecStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed )
{
	int inc = 0;
	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_INC ) inc = 1;

	tokenID++;
	consumed++; // consume "inc"/"dec"
	if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
	{
		uString error;
		error.Format( "Unexpected token \"%s\", %s must be followed by a variable", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), inc ? "INC" : "DEC" );
		pProgram->TokenError( error.GetStr(), tokenID );
		return 0;
	}

	int identToken = tokenID;

	stVariable *pVar = 0;
	stVariable findVar;
	findVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );

	// check function local first, if applicable
	if ( pFunction )
	{
		int index = pFunction->pFuncVars.Find( &findVar );
		if ( index >= 0 ) pVar = pFunction->pFuncVars.m_pData[ index ];

		pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
		pProgram->GetToken( identToken )->pExtraData = pFunction;
	}

	if ( !pVar )
	{
		int index = pProgram->m_pVariables.Find( &findVar );
		if ( index >= 0 )
		{
			if ( pFunction )
			{
				// functions can only access global variables
				if ( pProgram->m_pVariables.m_pData[ index ]->iScope != AGK_VARIABLE_GLOBAL ) 
				{
					/*
					uString error;
					error.Format( "\"%s\" has been defined as a local variable so cannot be accessed from functions", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), identToken );
					return 0;
					*/
					index = -1;
				}
			}
		}
		
		if ( index >= 0 )
		{
			pVar = pProgram->m_pVariables.m_pData[ index ];

			pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
			pProgram->GetToken( identToken )->pExtraData = 0;
		}
		else
		{
			if ( pProgram->m_iOptionExplicit 
			  || pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_SQR_BRACKET
			  || pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_DOT )
			{
				uString error;
				error.Format( "\"%s\" has not been defined", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), identToken );
				return 0;
			}
			
			//uString error;
			//error.Format( "\"%s\" is being used without being defined or initialised", pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );
			//pProgram->TokenError( error.GetStr(), tokenID );
			//return 0;

			stVariable newVar;
			newVar.iScope = pFunction ? AGK_VARIABLE_FUNC_LOCAL : AGK_VARIABLE_LOCAL;
			newVar.iTokenID = identToken;
			newVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw );
			newVar.sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig );

			newVar.iType = AGK_VARIABLE_INTEGER;
			if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '#' ) newVar.iType = AGK_VARIABLE_FLOAT;
			else if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '$' ) newVar.iType = AGK_VARIABLE_STRING;

			if ( pFunction ) 
			{
				index = pFunction->AddVariable( &newVar );
				pVar = pFunction->pFuncVars.m_pData[ index ];

				pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
				pProgram->GetToken( identToken )->pExtraData = pFunction;
			}
			else 
			{
				index = pProgram->AddVariable( newVar );
				pVar = pProgram->m_pVariables.m_pData[ index ];

				pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
				pProgram->GetToken( identToken )->pExtraData = 0;
			}
		}
	}
	
	if ( pVar->iNumDimensions > 0 )
	{
		// variable is an array
		pProgram->AddInstruction( (pVar->iScope == AGK_VARIABLE_FUNC_LOCAL) ? AGKI_PUSH_LOCAL_ARRAY : AGKI_PUSH_ARRAY, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pVar->iVarID, 0 );
		
		return ParseArrayInc( inc, pFunction, pProgram, pVar, tokenID, consumed );
		return 1;
	}
	else if ( pVar->iType == AGK_VARIABLE_TYPE )
	{
		tokenID++;
		consumed++; // consume identifier

		// variable is a type
		pProgram->AddInstruction( (pVar->iScope == AGK_VARIABLE_FUNC_LOCAL) ? AGKI_PUSH_LOCAL_TYPE : AGKI_PUSH_TYPE, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pVar->iVarID, 0 );

		int subConsumed = 0;
		return ParseTypeInc( inc, pFunction, pProgram, pVar, tokenID, consumed );
	}
	else
	{
		tokenID++;
		consumed++; // consumed identifier

		if ( pVar->iType == AGK_VARIABLE_STRING )
		{
			pProgram->TokenError( "Cannot increment or decrement strings", tokenID );
			return 0;
		}

		// just a normal variable
		int dataType = 0;
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_COMMA )
		{
			converter c; c.f = 1;
			if ( pVar->iType == AGK_VARIABLE_FLOAT ) pProgram->AddInstruction( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, c.i, 0 );
			if ( pVar->iType == AGK_VARIABLE_INTEGER ) pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 1, 0 );
			dataType = pVar->iType;
		}
		else
		{
			tokenID++;
			consumed++; // consume comma

			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			tokenID += subConsumed;
			consumed += subConsumed;
		}

		// no types here, handled earlier
		if ( dataType != pVar->iType )
		{
			if ( dataType == AGK_VARIABLE_INTEGER && pVar->iType == AGK_VARIABLE_FLOAT )
			{
				// possible optimization, convert push_int to push_float
				pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else if ( dataType == AGK_VARIABLE_FLOAT && pVar->iType == AGK_VARIABLE_INTEGER )
			{
				pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else
			{
				uString error;
				error.Format( "Cannot assign %s to %s", getDataTypeName(dataType), getDataTypeName(pVar->iType) );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}

		AGKInstruction instruction = AGKI_NOP;
		if ( pVar->iScope == AGK_VARIABLE_FUNC_LOCAL )
		{
			switch( pVar->iType )
			{
			case AGK_VARIABLE_INTEGER: instruction = inc ? AGKI_INC_LOCAL_INT : AGKI_DEC_LOCAL_INT; break;
			case AGK_VARIABLE_FLOAT: instruction = inc ? AGKI_INC_LOCAL_FLOAT : AGKI_DEC_LOCAL_FLOAT; break;
				default: Error( "Unrecognised variable type, shouldn't get here!", 0,0 ); return 0;
			}
		}
		else
		{
			switch( pVar->iType )
			{
			case AGK_VARIABLE_INTEGER: instruction = inc ? AGKI_INC_INT : AGKI_DEC_INT; break;
			case AGK_VARIABLE_FLOAT: instruction = inc ? AGKI_INC_FLOAT : AGKI_DEC_FLOAT; break;
				default: Error( "Unrecognised variable type, shouldn't get here!", 0,0 ); return 0;
			}
		}

		pProgram->AddInstruction( instruction, pProgram->GetToken( identToken )->iLine, pProgram->GetToken( identToken )->iIncludeFile, pVar->iVarID, 0 );
		return 1;
	}
}

int Parser2::ParseIfStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed )
{
	int ifToken = tokenID-1;

	int subConsumed = 0;
	int dataType = 0;
	int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
	if ( result <= 0 ) 
	{
		//return result;
		while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_THEN && tokenID < (int) pProgram->m_iNumTokens ) 
		{
			tokenID++;
			consumed++;
		}
	}
	else
	{
		tokenID += subConsumed;
		consumed += subConsumed;

		if ( dataType != AGK_VARIABLE_INTEGER )
		{
			pProgram->TokenError( "IF conditional must be an integer data type", tokenID );
			//return 0;
		}
	}

	// if false jump to ELSE block
	pProgram->AddInstruction( AGKI_JUMP_NOT_COND, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
	int jumpElsePtr = pProgram->GetInstructionPosition();

	if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
	{
		tokenID++;
		consumed++; // consume EOL

		// block IF statement
		while ( tokenID < (int)pProgram->m_iNumTokens
			   && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_ELSE
			   && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_ELSEIF
			   && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_ENDIF )
		{
			subConsumed = 0;
			int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				continue;
			}
			tokenID += subConsumed;
			consumed += subConsumed;
		} 

		if ( tokenID >= (int)pProgram->m_iNumTokens )
		{
			pProgram->TokenError( "IF statement must have a matching THEN or ENDIF statement", ifToken );
			return 0;
		}

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_ENDIF )
		{
			// jump ELSE to here
			int endPtr = pProgram->GetInstructionPosition() + 1;
			pProgram->EditInstruction( jumpElsePtr, AGKI_JUMP_NOT_COND, endPtr, 0 );

			tokenID++;
			consumed++; // consume "endif"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", ENDIF must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
			}
			consumed++; // consume EOL
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_ELSE )
		{
			int elseToken = tokenID;

			// jump THEN to end
			pProgram->AddInstruction( AGKI_JUMP, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			int jumpEndPtr = pProgram->GetInstructionPosition();

			// jump ELSE to here
			pProgram->EditInstruction( jumpElsePtr, AGKI_JUMP_NOT_COND, (jumpEndPtr+1), 0 );

			tokenID++;
			consumed++; // consume "else"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", ELSE must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
			}

			tokenID++;
			consumed++; // consume EOL

			// block ELSE statement
			while ( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_ENDIF )
			{
				subConsumed = 0;
				int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
					{
						tokenID++;
						consumed++;
					}
				}
				else
				{
					tokenID += subConsumed;
					consumed += subConsumed;
				}
			} 

			if ( tokenID >= (int) pProgram->m_iNumTokens || pProgram->GetToken( tokenID )->iType != AGK_TOKEN_ENDIF )
			{
				pProgram->TokenError( "ELSE does not have a matching ENDIF", elseToken );
				return 0;
			}

			// edit jump THEN to end
			int endPtr = pProgram->GetInstructionPosition() + 1;
			pProgram->EditInstruction( jumpEndPtr, AGKI_JUMP, endPtr, 0 );

			tokenID++;
			consumed++; // consume "endif"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", ENDIF must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
			}
			consumed++; // consume EOL
			return 1;
		}
		else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_ELSEIF )
		{
			tokenID++;
			consumed++; // consumed "elseif"

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
			{
				pProgram->TokenError( "ELSEIF must be followed by a condition", tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
			}

			// jump THEN to end
			pProgram->AddInstruction( AGKI_JUMP, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			int jumpEndPtr = pProgram->GetInstructionPosition();

			// jump ELSE to here
			pProgram->EditInstruction( jumpElsePtr, AGKI_JUMP_NOT_COND, (jumpEndPtr+1), 0 );

			subConsumed = 0;
			int result = ParseIfStatement( pFunction, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			tokenID += subConsumed;
			consumed += subConsumed;

			// edit jump THEN to end
			int endPtr = pProgram->GetInstructionPosition() + 1;
			pProgram->EditInstruction( jumpEndPtr, AGKI_JUMP, endPtr, 0 );
			return 1;
		}
		else
		{
			pProgram->TokenError( "IF statement does not have a matching ENDIF", ifToken );
			return 0;
		}
	}
	else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_THEN )
	{
		// single line IF statement
		tokenID++;
		consumed++; // consume "then"

		subConsumed = 0;
		result = ParseThenStatement( pFunction, pProgram, tokenID, subConsumed );
		if ( result <= 0 ) 
		{
			//return result;
			while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
			{
				tokenID++;
				consumed++;
			}
			consumed++; // consume EOL
			return 1; // pretend it succeeded to continue finding errors
		}
		tokenID += subConsumed;
		consumed += subConsumed;

		if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
		{
			// end of if statement
			int ifEnd = pProgram->GetInstructionPosition() + 1;
			pProgram->EditInstruction( jumpElsePtr, AGKI_JUMP_NOT_COND, ifEnd, 0 );
			consumed++; // consume EOL
			return 1;
		} 
		else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_ELSE )
		{	
			// single line ELSE statement
			// end the THEN clause with a jump over the ELSE part
			pProgram->AddInstruction( AGKI_JUMP, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			int jumpEndPtr = pProgram->GetInstructionPosition();

			pProgram->EditInstruction( jumpElsePtr, AGKI_JUMP_NOT_COND, (jumpEndPtr+1), 0 );
			tokenID++;
			consumed++; // consume "else"

			subConsumed = 0;
			result = ParseThenStatement( pFunction, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				consumed++; // consume EOL
				return 1; // pretend it succeeded to continue finding errors
			}
			tokenID += subConsumed;
			consumed += subConsumed;

			// edit THEN jump to point to the end
			int endPtr = pProgram->GetInstructionPosition() + 1;
			pProgram->EditInstruction( jumpEndPtr, AGKI_JUMP, endPtr, 0 );

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", ELSE statement must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
			}
			consumed++; // consume EOL
			return 1;
		}
		else
		{
			uString error;
			error.Format( "Unexpected token \"%s\", THEN statement must be followed by ELSE or a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			//return 0;
			while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
			{
				tokenID++;
				consumed++;
			}
			consumed++; // consume EOL
			return 1; // pretend it succeeded to continue finding errors
		}
	}
	else
	{
		uString error;
		error.Format( "Unexpected token \"%s\", IF conditional must be followed by THEN or a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
		pProgram->TokenError( error.GetStr(), tokenID );
		//return 0;
		while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
		{
			tokenID++;
			consumed++;
		}
		consumed++; // consume EOL
		return 1; // pretend it succeeded to continue finding errors
	}
}

int Parser2::ParseThenStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed )
{
	switch( pProgram->GetToken( tokenID )->iType )
	{
		case AGK_TOKEN_FUNCTION_EXIT:
		{
			if ( !pFunction )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", cannot exit a function here", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			// parse end function return value
			if ( pFunction->iReturnType > 0 )
			{
				if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "ExitFunction must return a value because EndFunction returns a value", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				int returnType = pFunction->iReturnType & 0x1f;
				int returnDimensions = (pFunction->iReturnType >> 5) & 7;
				int numVarParams = ((int)pFunction->iNumParams) - pFunction->iNumParamStrings;
				if ( returnType == AGK_VARIABLE_TYPE )
				{
					pProgram->AddInstruction( AGKI_PUSH_LOCAL_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 );
				}
				else if ( returnDimensions > 0 )
				{
					pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 );
				}

				tokenID++;
				consumed++; // consume "ExitFunction"
				
				int dataType = 0;
				int subConsumed = 0;
				int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
				if ( result <= 0 ) return result;
				tokenID += subConsumed;
				consumed += subConsumed;
			
				// check return type matches function return type, or can be cast to it
				int type1 = pFunction->iReturnType & 0x1f;
				int type2 = dataType & 0x1f;
				if ( type1 != type2 )
				{
					if ( type2 == AGK_VARIABLE_INTEGER && type1 == AGK_VARIABLE_FLOAT )
					{
						pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						dataType = AGK_VARIABLE_FLOAT;
					}
					else if ( type2 == AGK_VARIABLE_FLOAT && type1 == AGK_VARIABLE_INTEGER )
					{
						pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						dataType = AGK_VARIABLE_INTEGER;
					}
					else
					{
						uString error;
						error.Format( "Cannot convert type %s to %s, data type must match the value returned by EndFunction", getDataTypeName(type2), getDataTypeName(type1) );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}
				}
				
				if ( type1 == AGK_VARIABLE_TYPE || type1 == AGK_VARIABLE_ARRAY_TYPE )
				{
					// check type index
					type1 = pFunction->iReturnType >> 8;
					type2 = dataType >> 8;
					if ( type1 != type2 )
					{
						uString error;
						error.Format( "Cannot convert type \"%s\" to \"%s\", data type must match the value returned by EndFunction", pProgram->m_pTypes.m_pData[ type2 ]->sNameOrig.GetStr(), pProgram->m_pTypes.m_pData[ type1 ]->sNameOrig.GetStr() );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}
				}
				else
				{
					// check array dimensions
					type1 = (pFunction->iReturnType >> 5) & 7;
					type2 = (dataType >> 5) & 7;
					if ( type1 != type2 )
					{
						uString error;
						error.Format( "Cannot convert an array with %d dimensions to one with %d dimensions, data type must match the value returned by EndFunction", type2, type1 );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}
				}

				// store return type on stack in previously determined location
				if ( returnDimensions > 0 )
				{
					pProgram->AddInstruction( AGKI_STORE_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}
				else
				{
					switch( returnType )
					{
						case AGK_VARIABLE_INTEGER: pProgram->AddInstruction( AGKI_STORE_LOCAL_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 ); break;
						case AGK_VARIABLE_FLOAT: pProgram->AddInstruction( AGKI_STORE_LOCAL_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 ); break;
						case AGK_VARIABLE_STRING: pProgram->AddInstruction( AGKI_STORE_LOCAL_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-pFunction->iNumParamStrings), 0 ); break;
						case AGK_VARIABLE_TYPE: pProgram->AddInstruction( AGKI_STORE_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 ); break;
						default: Error("Unrecognised variable type", 0, 0);
					}
				}
			}
			else
			{
				// no return value
				tokenID++;
				consumed++; // consume "ExitFunction"

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_ELSE )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", this function does not return a value", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
			}

			// put a place holder to signal the end of the function, this will be replaced with a jump later
			pProgram->AddInstruction( AGKI_UNMATCHED_FUNC_EXIT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			return 1;
		}
		case AGK_TOKEN_EOL: // empty line
		{
			pProgram->TokenError( "THEN and ELSE must be followed by a statement on the same line", tokenID );
			return 0;
		}
		case AGK_TOKEN_IDENTIFIER: // could be function call, variable assignment, or variable declaration
		{
			int subConsumed = 0;
			int result = ParseIdentifierStatement( pFunction, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			tokenID += subConsumed;
			consumed += subConsumed;

			return 1;
		}
		
		case AGK_TOKEN_INC:
		case AGK_TOKEN_DEC:
		{
			return ParseIncDecStatement( pFunction, pProgram, tokenID, consumed );
		}

		case AGK_TOKEN_LOOP_CONT:
		{
			// place holder, loops will replace this with a jump
			pProgram->AddInstruction( AGKI_UNMATCHED_CONTINUE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			tokenID++;
			consumed++; // consume "continue"
			return 1;	
		}

		case AGK_TOKEN_LOOP_EXIT:
		{
			// place holder, loops will replace this with a jump
			pProgram->AddInstruction( AGKI_UNMATCHED_EXIT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			tokenID++;
			consumed++; // consume "exit"
			return 1;	
		}

		// gosub
		case AGK_TOKEN_GOSUB:
		{
			tokenID++;
			consumed++; // consume "gosub"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", gosub must be followed by an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			int index = pProgram->FindLabel( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( index < 0 )
			{
				uString error;
				error.Format( "\"%s\" is not a valid label", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			pProgram->AddInstruction( AGKI_GOSUB, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, index, 0 );

			consumed++; // consume identifier
			return 1;
		}
		case AGK_TOKEN_RETURN:
		{
			tokenID++;
			consumed++; // consume "return"
			
			pProgram->AddInstruction( AGKI_GOSUB_RETURN, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			return 1;
		}

		case AGK_TOKEN_GOTO:
		{
			tokenID++;
			consumed++; // consume "goto"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", goto must be followed by an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			int index = pProgram->FindLabel( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( index < 0 )
			{
				uString error;
				error.Format( "\"%s\" is not a valid label", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			pProgram->AddInstruction( AGKI_JUMP_LABEL, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, index, 0 );

			consumed++; // consume identifier
			return 1;
		}

		case AGK_TOKEN_END:
		{
			pProgram->AddInstruction( AGKI_END, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			tokenID++;
			consumed++; // consume "end"
			return 1;
		}

		default:
		{
			uString error;
			error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}

	return 1;
}

int Parser2::ParseStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed )
{
	switch( pProgram->GetToken( tokenID )->iType )
	{
		case AGK_TOKEN_FOLDSTART:
		{
			tokenID++;
			consumed++; // consume "FoldStart"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", FoldStart must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_FOLDEND:
		{
			tokenID++;
			consumed++; // consume "FoldEnd"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", FoldEnd must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_DEC_FUNCTION:
		{
			// skip functions, they are handled separately
			do
			{
				tokenID++;
				consumed++;
			} while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_FUNCTION_END );

			do 
			{
				tokenID++;
				consumed++;
			} while ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL );
			break;
		}
		case AGK_TOKEN_FUNCTION_EXIT:
		{
			if ( !pFunction )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", cannot exit a function here", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			// parse end function return value
			if ( pFunction->iReturnType > 0 )
			{
				if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "ExitFunction must return a value because EndFunction returns a value", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				int returnType = pFunction->iReturnType & 0x1f;
				int returnDimensions = (pFunction->iReturnType >> 5) & 7;
				int numVarParams = ((int)pFunction->iNumParams) - pFunction->iNumParamStrings;
				if ( returnType == AGK_VARIABLE_TYPE )
				{
					pProgram->AddInstruction( AGKI_PUSH_LOCAL_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 );
				}
				else if ( returnDimensions > 0 )
				{
					pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 );
				}

				tokenID++;
				consumed++; // consume "ExitFunction"

				int dataType = 0;
				int subConsumed = 0;
				int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
				if ( result <= 0 ) return result;
				tokenID += subConsumed;
				consumed += subConsumed;

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				consumed++; // consume EOL
			
				// check return type matches function return type, or can be cast to it
				int type1 = pFunction->iReturnType & 0x1f;
				int type2 = dataType & 0x1f;
				if ( type1 != type2 )
				{
					if ( type2 == AGK_VARIABLE_INTEGER && type1 == AGK_VARIABLE_FLOAT )
					{
						pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						dataType = AGK_VARIABLE_FLOAT;
					}
					else if ( type2 == AGK_VARIABLE_FLOAT && type1 == AGK_VARIABLE_INTEGER )
					{
						pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						dataType = AGK_VARIABLE_INTEGER;
					}
					else
					{
						uString error;
						error.Format( "Cannot convert type %s to %s, data type must match the value returned by EndFunction", getDataTypeName(type2), getDataTypeName(type1) );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}
				}
				
				if ( type1 == AGK_VARIABLE_TYPE || type1 == AGK_VARIABLE_ARRAY_TYPE )
				{
					// check type index
					type1 = pFunction->iReturnType >> 8;
					type2 = dataType >> 8;
					if ( type1 != type2 )
					{
						uString error;
						error.Format( "Cannot convert type \"%s\" to \"%s\", data type must match the value returned by EndFunction", pProgram->m_pTypes.m_pData[ type2 ]->sNameOrig.GetStr(), pProgram->m_pTypes.m_pData[ type1 ]->sNameOrig.GetStr() );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}
				}
				else
				{
					// check array dimensions
					type1 = (pFunction->iReturnType >> 5) & 7;
					type2 = (dataType >> 5) & 7;
					if ( type1 != type2 )
					{
						uString error;
						error.Format( "Cannot convert an array with %d dimensions to one with %d dimensions, data type must match the value returned by EndFunction", type2, type1 );
						pProgram->TokenError( error.GetStr(), tokenID );
						return 0;
					}
				}

				// store return type on stack in previously determined location
				if ( returnDimensions > 0 )
				{
					pProgram->AddInstruction( AGKI_STORE_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}
				else
				{
					switch( returnType )
					{
						case AGK_VARIABLE_INTEGER: pProgram->AddInstruction( AGKI_STORE_LOCAL_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 ); break;
						case AGK_VARIABLE_FLOAT: pProgram->AddInstruction( AGKI_STORE_LOCAL_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 ); break;
						case AGK_VARIABLE_STRING: pProgram->AddInstruction( AGKI_STORE_LOCAL_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-pFunction->iNumParamStrings), 0 ); break;
						case AGK_VARIABLE_TYPE: pProgram->AddInstruction( AGKI_STORE_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 ); break;
						default: Error("Unrecognised variable type", 0, 0);
					}
				}
			}
			else
			{
				// no return value
				tokenID++;
				consumed++; // consume "ExitFunction"

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", this function does not return a value", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
				consumed++; // consume EOL
			}

			// put a place holder to signal the end of the function, this will be replaced with a jump later
			pProgram->AddInstruction( AGKI_UNMATCHED_FUNC_EXIT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			return 1;
		}
		case AGK_TOKEN_DEC_SCOPE: // global or local keyword
		{
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_DEC_ARRAY )
			{
				// dim
				tokenID++;
				consumed += 2;

				int index = pProgram->FindVariable( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
				if ( index < 0 )
				{
					uString error;
					error.Format( "Array \"%s\" has not been defined", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				// push array for block resize
				int varID = pProgram->m_pVariables.m_pData[ index ]->iVarID;
				pProgram->AddInstruction( AGKI_PUSH_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, 0 );
				
				int subConsumed = 0;
				int result = ParseDimArray( pFunction, pProgram, 0, &(pProgram->m_pInstructions), tokenID, subConsumed );
				if ( result <= 0 ) return result;
				tokenID += subConsumed;
				consumed += subConsumed;

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
				consumed++;
				return 1;
			}
			else if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_IDENTIFIER )
			{
				stVariable *pVarDec = 0;
				stVariable findVar;
				findVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
				findVar.sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );

				if ( pFunction )
				{
					int index = pFunction->pFuncVars.Find( &findVar );
					if ( index >= 0 ) 
					{
						pVarDec = pFunction->pFuncVars.m_pData[ index ];

						pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
						pProgram->GetToken( tokenID )->pExtraData = pFunction;
					}
				}

				if ( !pVarDec )
				{
					int index = pProgram->m_pVariables.Find( &findVar );
					if ( index >= 0 )
					{
						pVarDec = pProgram->m_pVariables.m_pData[ index ];

						pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
						pProgram->GetToken( tokenID )->pExtraData = 0;
					}
				}

				// variable
				consumed += 1;
				tokenID--;
				do
				{
					tokenID++;
					int subConsumed = 0;
					int result = ParseVariableDec( 1, 0, pProgram, tokenID, subConsumed );
					if ( result <= 0 ) return result;
					tokenID += subConsumed;
					consumed += subConsumed+1; // also consume the comma
				} while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );
				
				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
				// EOL already consumed

				// if variable has a default value reset the value here
				if ( pVarDec )
				{
					switch( pVarDec->iType )
					{
						case AGK_VARIABLE_FLOAT:
						{
							if ( pVarDec->pDefaultF )
							{
								converter c; c.f = pVarDec->pDefaultF[0];
								pProgram->AddInstruction( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, c.i, 0 );
								if ( pVarDec->iScope == AGK_VARIABLE_FUNC_LOCAL )
									pProgram->AddInstruction( AGKI_STORE_LOCAL_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
								else
									pProgram->AddInstruction( AGKI_STORE_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
							}
							break;
						}
						case AGK_VARIABLE_INTEGER:
						{
							if ( pVarDec->pDefaultI )
							{
								pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->pDefaultI[0], 0 );
								if ( pVarDec->iScope == AGK_VARIABLE_FUNC_LOCAL )
									pProgram->AddInstruction( AGKI_STORE_LOCAL_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
								else
									pProgram->AddInstruction( AGKI_STORE_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
							}
							break;
						}
						case AGK_VARIABLE_STRING:
						{
							if ( pVarDec->pDefaultI )
							{
								pProgram->AddInstruction( AGKI_PUSH_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->pDefaultI[0], 0 );
								if ( pVarDec->iScope == AGK_VARIABLE_FUNC_LOCAL )
									pProgram->AddInstruction( AGKI_STORE_LOCAL_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
								else
									pProgram->AddInstruction( AGKI_STORE_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
							}
							break;
						}
					}
				}

				return 1;
			}
			else
			{
				uString error;
				error.Format( "Unexpected token \"%s\", %s must be followed by an array or variable declaration", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), pProgram->GetToken( tokenID-1 )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
		}
		case AGK_TOKEN_DEC_ARRAY: // dim
		{
			tokenID++;
			consumed++;

			int index = pProgram->FindVariable( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( index < 0 )
			{
				uString error;
				error.Format( "Array \"%s\" has not been defined", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			// push array for block resize
			int varID = pProgram->m_pVariables.m_pData[ index ]->iVarID;
			pProgram->AddInstruction( AGKI_PUSH_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, 0 );
			
			int subConsumed = 0;
			int result = ParseDimArray( pFunction, pProgram, 0, &(pProgram->m_pInstructions), tokenID, subConsumed );
			if ( result <= 0 ) return result;
			tokenID += subConsumed;
			consumed += subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++;
			return 1;
		}
		case AGK_TOKEN_UNDEC_ARRAY: // undim
		{
			tokenID++;
			consumed++; // consume "undim"

			int index = pProgram->FindVariable( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( index < 0 )
			{
				uString error;
				error.Format( "Array \"%s\" has not been defined", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			// push array for deleting
			int varID = pProgram->m_pVariables.m_pData[ index ]->iVarID;
			pProgram->AddInstruction( AGKI_PUSH_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, 0 );

			tokenID++;
			consumed++; // consume identifer
			
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_SQR_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array must be followed by an open and close square bracket", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume open bracket

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_SQR_BRACKET )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array must be followed by empty square brackets", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume close bracket

			pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, -1, 0 );
			pProgram->AddInstruction( AGKI_ARRAY_RESIZE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++;
			return 1;
		}
		case AGK_TOKEN_EOL: // empty line
		{
			consumed += 1;
			return 1;
		}
		case AGK_TOKEN_DEC_TYPE: // type, already parsed in the first pass
		{
			do 
			{
				consumed++;
				tokenID++;
			} while ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_TYPE_END );

			consumed++;
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", EndType must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++;
			return 1;
		}
		case AGK_TOKEN_IDENTIFIER: // could be function call, variable assignment, or variable declaration
		{
			int identToken = tokenID;

			// check if this is a function parameter on a new line, if so skip it
			if ( tokenID > 1 && pProgram->GetToken( tokenID-2 )->iType == AGK_TOKEN_COMMA )
			{
				while ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
				{
					tokenID++;
					consumed++;
				}

				// consume EOL
				tokenID++;
				consumed++;
				return 1;
			}
			
			// check for declaration without a scope
			if ( pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_DEC_AS )
			{
				tokenID--;
				do
				{
					tokenID++;
					int subConsumed = 0;
					int result = ParseVariableDec( 1, 0, pProgram, tokenID, subConsumed );
					if ( result <= 0 ) return result;
					tokenID += subConsumed;
					consumed += subConsumed+1; // also consume the comma
				} while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );
				
				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}
				// EOL already consumed

				stVariable *pVarDec = 0;
				stVariable findVar;
				findVar.sName.SetStr( pProgram->GetToken( identToken )->sValueRaw.GetStr() );
				findVar.sNameOrig.SetStr( pProgram->GetToken( identToken )->sValueRawOrig.GetStr() );

				if ( pFunction )
				{
					int index = pFunction->pFuncVars.Find( &findVar );
					if ( index >= 0 ) 
					{
						pVarDec = pFunction->pFuncVars.m_pData[ index ];

						pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
						pProgram->GetToken( identToken )->pExtraData = pFunction;
					}
				}

				if ( !pVarDec )
				{
					int index = pProgram->m_pVariables.Find( &findVar );
					if ( index >= 0 )
					{
						pVarDec = pProgram->m_pVariables.m_pData[ index ];

						pProgram->GetToken( identToken )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
						pProgram->GetToken( identToken )->pExtraData = 0;
					}
				}

				// if variable has a default value reset the value here
				if ( pVarDec )
				{
					switch( pVarDec->iType )
					{
						case AGK_VARIABLE_FLOAT:
						{
							if ( pVarDec->pDefaultF )
							{
								converter c; c.f = pVarDec->pDefaultF[0];
								pProgram->AddInstruction( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, c.i, 0 );
								if ( pVarDec->iScope == AGK_VARIABLE_FUNC_LOCAL )
									pProgram->AddInstruction( AGKI_STORE_LOCAL_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
								else
									pProgram->AddInstruction( AGKI_STORE_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
							}
							break;
						}
						case AGK_VARIABLE_INTEGER:
						{
							if ( pVarDec->pDefaultI )
							{
								pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->pDefaultI[0], 0 );
								if ( pVarDec->iScope == AGK_VARIABLE_FUNC_LOCAL )
									pProgram->AddInstruction( AGKI_STORE_LOCAL_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
								else
									pProgram->AddInstruction( AGKI_STORE_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
							}
							break;
						}
						case AGK_VARIABLE_STRING:
						{
							if ( pVarDec->pDefaultI )
							{
								pProgram->AddInstruction( AGKI_PUSH_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->pDefaultI[0], 0 );
								if ( pVarDec->iScope == AGK_VARIABLE_FUNC_LOCAL )
									pProgram->AddInstruction( AGKI_STORE_LOCAL_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
								else
									pProgram->AddInstruction( AGKI_STORE_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVarDec->iVarID, 0 );
							}
							break;
						}
					}
				}

				return 1;
			}

			int subConsumed = 0;
			int result = ParseIdentifierStatement( pFunction, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			tokenID += subConsumed;
			consumed += subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consumed EOL
			return 1;
		}
		case AGK_TOKEN_CONSTANT:
		{
			consumed++; // consume #constant
			tokenID++;

			// constant name
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #constant must be followed by a name", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_CONSTANT_NAME;
			pProgram->GetToken( tokenID )->pExtraData = 0;

			tokenID++;
			consumed++; // consume identifier

			while (pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL)
			{
				tokenID++;
				consumed++;
			}

			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_RENDERER:
		{
			consumed++; // consume "#renderer"
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_STRING )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #renderer must be followed by a string", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume string
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", renderer string must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_INCLUDE:
		{
			consumed++; // consume "#include"
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_STRING )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #include must be followed by a string", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_INCLUDE_PATH;
			pProgram->GetToken( tokenID )->pExtraData = 0;

			consumed++; // consume string
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", include file must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_INSERT:
		{
			consumed++; // consume "#insert"
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_STRING )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #insert must be followed by a string", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_INCLUDE_PATH;
			pProgram->GetToken( tokenID )->pExtraData = 0;

			consumed++; // consume string
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", insert file must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_IMPORT_PLUGIN:
		{
			consumed++; // consume "#import_plugin"
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #import_plugin must be followed by an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume identifier
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
			{
				consumed++;
				return 0;
			}

			// if not EOL then must be AS Identifier
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_AS )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #import_plugin identifier must be followed by a new line or AS", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume AS
			tokenID++;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #import_plugin as must be followed by an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume identifier
			tokenID++;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #import_plugin must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_IMPORT_MODULE:
		{
			consumed++; // consume "#import_module"
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #import_module must be followed by an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume identifier
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
			{
				consumed++;
				return 0;
			}

			// if not EOL then must be AS Identifier
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_AS )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #import_module identifier must be followed by a new line or AS", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume AS
			tokenID++;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #import_module as must be followed by an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume identifier
			tokenID++;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #import_module must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_LABEL:
		{
			int index = pProgram->FindLabel( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( index < 0 )
			{
				uString error;
				error.Format( "Label \"%s\" was not correctly parsed, reason unknown", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_LABEL_NAME;
			pProgram->GetToken( tokenID )->pExtraData = 0;

			pProgram->m_pLabels.m_pData[ index ]->iInstructionPtr = pProgram->GetInstructionPosition() + 1;

			tokenID++;
			consumed++; // consume label
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", label must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_COMPANY_NAME:
		{
			consumed++; // consume "#company_name"
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_STRING )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", #company_name must be followed by a string", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume string
			tokenID++;
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", company_name string must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume EOL
			return 1;
		}

		case AGK_TOKEN_INC:
		case AGK_TOKEN_DEC:
		{
			int subConsumed = 0;
			int result = ParseIncDecStatement( pFunction, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) return result;
			tokenID += subConsumed;
			consumed += subConsumed;

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consumed EOL
			return 1;
		}

		// do loop
		case AGK_TOKEN_DO:
		{
			int doToken = tokenID;
			tokenID++;
			consumed++; // consume "do"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", DO must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			tokenID++;
			consumed++; // consume new line

			int loopStart = pProgram->GetInstructionPosition()+1;

			while( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_LOOP )
			{
				int subConsumed = 0;
				int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
					{
						tokenID++;
						consumed++;
					}
				}
				else
				{
					tokenID += subConsumed;
					consumed+= subConsumed;
				}
			}

			if ( tokenID >= (int)pProgram->m_iNumTokens || pProgram->GetToken( tokenID )->iType != AGK_TOKEN_LOOP )
			{
				pProgram->TokenError( "DO does not have a matching LOOP", doToken );
				return 0;
			}

			if ( !pProgram->IsSkippingInstructions() )
			{
				pProgram->AddInstruction( AGKI_JUMP, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, loopStart, 0 );
				int loopEnd = pProgram->GetInstructionPosition() + 1;

				for ( int i = loopStart; i < loopEnd; i++ )
				{
					if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_CONTINUE )
					{
						// loop continue
						pProgram->EditInstruction( i, AGKI_JUMP, loopStart, 0 );
					}
					else if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_EXIT )
					{
						// loop exit
						pProgram->EditInstruction( i, AGKI_JUMP, loopEnd, 0 );
					}
				}
			}

			tokenID++;
			consumed++; // consume "loop"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", LOOP must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
				{
					tokenID++;
					consumed++;
				}
			}
			consumed++; // consume new line
			return 1;
		}

		// while endwhile
		case AGK_TOKEN_WHILE:
		{
			int whileToken = tokenID;
			tokenID++;
			consumed++; // consume "while"
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
			{
				pProgram->TokenError( "WHILE must be followed by an expression", tokenID );
				return 0;
			}

			int loopStart = pProgram->GetInstructionPosition()+1;

			int subConsumed = 0;
			int dataType = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
				{
					tokenID++;
					consumed++;
				}
			}
			else
			{
				tokenID += subConsumed;
				consumed += subConsumed;

				if ( dataType != AGK_VARIABLE_INTEGER )
				{
					pProgram->TokenError( "WHILE expression must be an integer type", tokenID );
					return 0;
				}
			}

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				pProgram->TokenError( "WHILE expression must be followed by a new line", tokenID );
				return 0;
			}

			// don't know where to jump yet, modify it later
			pProgram->AddInstruction( AGKI_JUMP_NOT_COND, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			int jumpEndInstruction = pProgram->GetInstructionPosition();

			while( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_ENDWHILE )
			{
				int subConsumed = 0;
				int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
					{
						tokenID++;
						consumed++;
					}
				}
				else
				{
					tokenID += subConsumed;
					consumed+= subConsumed;
				}
			}

			if ( tokenID >= (int)pProgram->m_iNumTokens || pProgram->GetToken( tokenID )->iType != AGK_TOKEN_ENDWHILE )
			{
				pProgram->TokenError( "WHILE does not have a matching ENDWHILE", whileToken );
				return 0;
			}

			if ( !pProgram->IsSkippingInstructions() )
			{
				pProgram->AddInstruction( AGKI_JUMP, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, loopStart, 0 );
				int loopEnd = pProgram->GetInstructionPosition() + 1;

				pProgram->EditInstruction( jumpEndInstruction, AGKI_JUMP_NOT_COND, loopEnd, 0 );

				for ( int i = loopStart; i < loopEnd; i++ )
				{
					if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_CONTINUE )
					{
						// loop continue
						pProgram->EditInstruction( i, AGKI_JUMP, loopStart, 0 );
					}
					else if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_EXIT )
					{
						// loop exit
						pProgram->EditInstruction( i, AGKI_JUMP, loopEnd, 0 );
					}
				}
			}

			tokenID++;
			consumed++; // consume "endwhile"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", ENDWHILE must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
				{
					tokenID++;
					consumed++;
				}
			}
			consumed++; // consume new line
			return 1;
		}

		// repeat until
		case AGK_TOKEN_REPEAT:
		{
			int repeatToken = tokenID;
			tokenID++;
			consumed++; // consume "repeat"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				pProgram->TokenError( "REPEAT must be followed by a new line", tokenID );
				return 0;
			}

			int loopStart = pProgram->GetInstructionPosition()+1;

			while( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_UNTIL )
			{
				int subConsumed = 0;
				int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
					{
						tokenID++;
						consumed++;
					}
				}
				else
				{
					tokenID += subConsumed;
					consumed+= subConsumed;
				}
			}

			if ( tokenID >= (int)pProgram->m_iNumTokens || pProgram->GetToken( tokenID )->iType != AGK_TOKEN_UNTIL )
			{
				pProgram->TokenError( "REPEAT does not have a matching UNTIL", repeatToken );
				return 0;
			}

			tokenID++;
			consumed++; // consume "until"

			int subConsumed = 0;
			int dataType = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
				{
					tokenID++;
					consumed++;
				}
				consumed++;
				return 1;
			}
			
			tokenID += subConsumed;
			consumed += subConsumed;
			
			if ( dataType != AGK_VARIABLE_INTEGER )
			{
				pProgram->TokenError( "UNTIL expression must be an integer type", tokenID );
				//return 0;
				while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
				{
					tokenID++;
					consumed++;
				}
			}

			if ( !pProgram->IsSkippingInstructions() )
			{
				pProgram->AddInstruction( AGKI_JUMP_NOT_COND, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, loopStart, 0 );
				int loopEnd = pProgram->GetInstructionPosition() + 1;

				for ( int i = loopStart; i < loopEnd; i++ )
				{
					if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_CONTINUE )
					{
						// loop continue
						pProgram->EditInstruction( i, AGKI_JUMP, loopStart, 0 );
					}
					else if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_EXIT )
					{
						// loop exit
						pProgram->EditInstruction( i, AGKI_JUMP, loopEnd, 0 );
					}
				}
			}

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", UNTIL expression must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) 
				{
					tokenID++;
					consumed++;
				}
			}
			consumed++; // consume new line
			return 1;
		}

		// for next
		case AGK_TOKEN_FOR:
		{
			// declare variables first as we may GOTO over their declarations
			int jumpEndPtr = -1;
			int cmpInstruction = -1;
			AGKInstruction origInstruction = AGKI_NOP;
			AGKInstruction alternateInstruction = AGKI_NOP;
			int loopStart = -1;
			int editInstruction = -1;
			int setInstruction = -1;
			DynArrayObject<stInstruction> pStepInstructions;
			stInstruction newInstruction;
			DynArrayObject<stInstruction> pToInstructions;
			int subConsumed = 0;
			int dataType = 0;
			int result;
			stVariable *pVar = 0;
			stVariable findVar;
			AGKInstruction instructionStore = AGKI_NOP;
			AGKInstruction instructionLoad = AGKI_NOP;
			AGKInstruction instructionInc = AGKI_NOP;

			int forToken = tokenID;
			tokenID++;
			consumed++; // consume "for"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				pProgram->TokenError( "FOR must be followed by a variable", tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				goto skip_for_declaration;
			}
			
			findVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );

			// check function local first, if applicable
			if ( pFunction )
			{
				int index = pFunction->pFuncVars.Find( &findVar );
				if ( index >= 0 ) 
				{
					pVar = pFunction->pFuncVars.m_pData[ index ];

					pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
					pProgram->GetToken( tokenID )->pExtraData = pFunction;
				}
			}

			if ( !pVar )
			{
				int index = pProgram->m_pVariables.Find( &findVar );
				if ( index >= 0 )
				{
					if ( pFunction )
					{
						// functions can only access global variables
						if ( pProgram->m_pVariables.m_pData[ index ]->iScope != AGK_VARIABLE_GLOBAL ) index = -1;
					}
				}
				
				if ( index >= 0 )
				{
					pVar = pProgram->m_pVariables.m_pData[ index ];

					pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
					pProgram->GetToken( tokenID )->pExtraData = 0;
				}
				else
				{
					if ( pProgram->m_iOptionExplicit 
					  || pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_OPEN_SQR_BRACKET
					  || pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_DOT )
					{
						uString error;
						error.Format( "\"%s\" has not been defined", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
						pProgram->TokenError( error.GetStr(), tokenID );
						//return 0;
						while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
						{
							tokenID++;
							consumed++;
						}
						goto skip_for_declaration;
					}
					
					// if not using option explicit we must add the variable
					stVariable newVar;
					newVar.iScope = pFunction ? AGK_VARIABLE_FUNC_LOCAL : AGK_VARIABLE_LOCAL;
					newVar.iTokenID = tokenID;
					newVar.sName.SetStr( pProgram->GetToken( tokenID )->sValueRaw );
					newVar.sNameOrig.SetStr( pProgram->GetToken( tokenID )->sValueRawOrig );

					newVar.iType = AGK_VARIABLE_INTEGER;
					if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '#' ) newVar.iType = AGK_VARIABLE_FLOAT;
					else if ( newVar.sName.CharAt( newVar.sName.GetNumChars()-1 ) == '$' ) newVar.iType = AGK_VARIABLE_STRING;

					if ( pFunction ) 
					{
						index = pFunction->AddVariable( &newVar );
						pVar = pFunction->pFuncVars.m_pData[ index ];

						pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_FUNCTION_LOCAL_VARIABLE;
						pProgram->GetToken( tokenID )->pExtraData = pFunction;
					}
					else 
					{
						index = pProgram->AddVariable( newVar );
						pVar = pProgram->m_pVariables.m_pData[ index ];

						pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_VARIABLE;
						pProgram->GetToken( tokenID )->pExtraData = 0;
					}
				}
			}
			
			if ( pVar->iNumDimensions > 0 )
			{
				pProgram->TokenError( "Cannot use arrays as a FOR loop variable", tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				goto skip_for_declaration;
			}
			else if ( pVar->iType == AGK_VARIABLE_TYPE )
			{
				pProgram->TokenError( "Cannot use types as a FOR loop variable", tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				goto skip_for_declaration;
			}
			else if ( pVar->iType == AGK_VARIABLE_STRING )
			{
				pProgram->TokenError( "Cannot use strings as a FOR loop variable", tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				goto skip_for_declaration;
			}
					
			tokenID++;
			consumed++; // consume identifier

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EQUALS )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", FOR variable must be followed by an equals", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				goto skip_for_declaration;
			}

			pVar->iInitialised = 1;

			tokenID++;
			consumed++; // consume equals

			// first (start) expression
			subConsumed = 0;
			dataType = 0;
			result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				goto skip_for_declaration;
			}
			tokenID += subConsumed;
			consumed += subConsumed;

			if ( dataType != pVar->iType )
			{
				if ( dataType == AGK_VARIABLE_FLOAT && pVar->iType == AGK_VARIABLE_INTEGER )
				{
					pProgram->AddInstruction( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}
				else if ( dataType == AGK_VARIABLE_INTEGER && pVar->iType == AGK_VARIABLE_FLOAT )
				{
					pProgram->AddInstruction( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}
				else 
				{
					uString error;
					error.Format( "Cannot assign %s to %s", getDataTypeName(dataType), getDataTypeName(pVar->iType) );
					pProgram->TokenError( error.GetStr(), tokenID );
					//return 0;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
					{
						tokenID++;
						consumed++;
					}
					goto skip_for_declaration;
				}
			}

			if ( pVar->iScope == AGK_VARIABLE_FUNC_LOCAL )
			{
				switch( pVar->iType )
				{
					case AGK_VARIABLE_INTEGER: instructionStore = AGKI_STORE_LOCAL_INT; instructionLoad = AGKI_PUSH_LOCAL_INT; instructionInc = AGKI_INC_LOCAL_INT; break;
					case AGK_VARIABLE_FLOAT: instructionStore = AGKI_STORE_LOCAL_FLOAT; instructionLoad = AGKI_PUSH_LOCAL_FLOAT; instructionInc = AGKI_INC_LOCAL_FLOAT; break;
					default: Error( "Unrecognised variable type, shouldn't get here!", 0,0 ); return 0;
				}
			}
			else
			{
				switch( pVar->iType )
				{
					case AGK_VARIABLE_INTEGER: instructionStore = AGKI_STORE_INT; instructionLoad = AGKI_PUSH_VAR_INT; instructionInc = AGKI_INC_INT; break;
					case AGK_VARIABLE_FLOAT: instructionStore = AGKI_STORE_FLOAT; instructionLoad = AGKI_PUSH_VAR_FLOAT; instructionInc = AGKI_INC_FLOAT; break;
					default: Error( "Unrecognised variable type, shouldn't get here!", 0,0 ); return 0;
				}
			}
			pProgram->AddInstruction( instructionStore, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->iVarID, 0 );

			// check for TO
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_TO )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", FOR expression must be followed by TO", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				goto skip_for_declaration;
			}

			tokenID++;
			consumed++; // consume "to"

			// check second (end) expression
			//pProgram->AddInstruction( instructionLoad, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->iVarID, 0 );
			
			newInstruction.Construct( instructionLoad, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->iVarID, 0 );
			pToInstructions.Add( &newInstruction );
			subConsumed = 0;
			dataType = 0;
			result = ParseExpression( pFunction, 1, 0, dataType, &pToInstructions, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
				goto skip_for_declaration;
			}
			tokenID += subConsumed;
			consumed += subConsumed;

			if ( dataType != pVar->iType )
			{
				if ( dataType == AGK_VARIABLE_FLOAT && pVar->iType == AGK_VARIABLE_INTEGER )
				{
					newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					pToInstructions.Add( &newInstruction );
				}
				else if ( dataType == AGK_VARIABLE_INTEGER && pVar->iType == AGK_VARIABLE_FLOAT )
				{
					// possible optimization, convert push_int to push_float
					newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					pToInstructions.Add( &newInstruction );
				}
				else 
				{
					uString error;
					error.Format( "Cannot compare %s and %s", getDataTypeName(dataType), getDataTypeName(pVar->iType) );
					pProgram->TokenError( error.GetStr(), tokenID );
					//return 0;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
					{
						tokenID++;
						consumed++;
					}
					goto skip_for_declaration;
				}
			}

			// check step expression, save for later
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_STEP )
			{
				stInstruction newInstruction;
				if ( pVar->iType == AGK_VARIABLE_INTEGER )
				{
					newInstruction.Construct( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 1, 0 );
				}
				else if ( pVar->iType == AGK_VARIABLE_FLOAT )
				{
					converter c; c.f = 1;
					newInstruction.Construct( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, c.i, 0 );
				}
				pStepInstructions.Add( &newInstruction );
			}
			else
			{
				tokenID++;
				consumed++; // consume "step"

				subConsumed = 0;
				dataType = 0;
				result = ParseExpression( pFunction, 1, 0, dataType, &pStepInstructions, pProgram, tokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
					{
						tokenID++;
						consumed++;
					}
					goto skip_for_declaration;
				}
				tokenID += subConsumed;
				consumed += subConsumed;

				if ( dataType != pVar->iType )
				{
					if ( dataType == AGK_VARIABLE_FLOAT && pVar->iType == AGK_VARIABLE_INTEGER )
					{
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_CAST_FLOAT_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pStepInstructions.Add( &newInstruction );
					}
					else if ( dataType == AGK_VARIABLE_INTEGER && pVar->iType == AGK_VARIABLE_FLOAT )
					{
						// possible optimization, convert push_int to push_float
						stInstruction newInstruction;
						newInstruction.Construct( AGKI_CAST_INT_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pStepInstructions.Add( &newInstruction );
					}
					else 
					{
						uString error;
						error.Format( "Invalid data type for STEP, cannot assign %s to %s", getDataTypeName(dataType), getDataTypeName(pVar->iType) );
						pProgram->TokenError( error.GetStr(), tokenID );
						//return 0;
						while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
						{
							tokenID++;
							consumed++;
						}
						goto skip_for_declaration;
					}
				}
			}

			if ( !pProgram->IsSkippingInstructions() )
			{
				if ( pStepInstructions.m_iCount > 1 || pStepInstructions.m_pData[ 0 ]->iInstruction != AGKI_PUSH_INT || pStepInstructions.m_pData[ 0 ]->i < 0 )
				{
					// check step direction and do some code modification
					setInstruction = pProgram->AddInstruction( AGKI_SET_INSTRUCTION, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

					for ( int i = 0; i < pStepInstructions.m_iCount; i++ )
					{
						pProgram->m_pInstructions.Add( pStepInstructions.m_pData[ i ] );
					}

					if ( pVar->iType == AGK_VARIABLE_INTEGER )
					{
						pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pProgram->AddInstruction( AGKI_CMP_LESS_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					}
					else
					{
						pProgram->AddInstruction( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pProgram->AddInstruction( AGKI_CMP_LESS_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					}

					// if step direction is negative then modify the code at runtime
					editInstruction = pProgram->AddInstruction( AGKI_EDIT_COND, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				}

				// loop iterates from here
				loopStart = pProgram->GetInstructionPosition()+1;

				for ( int i = 0; i < pToInstructions.m_iCount; i++ )
				{
					pProgram->m_pInstructions.Add( pToInstructions.m_pData[ i ] );
				}
			}

			if ( pVar->iType == AGK_VARIABLE_INTEGER )
			{
				origInstruction = AGKI_CMP_GREATER_INT;
				alternateInstruction = AGKI_CMP_LESS_INT;
				cmpInstruction = pProgram->AddInstruction( AGKI_CMP_GREATER_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else if ( pVar->iType == AGK_VARIABLE_FLOAT )
			{
				origInstruction = AGKI_CMP_GREATER_FLOAT;
				alternateInstruction = AGKI_CMP_LESS_FLOAT;
				cmpInstruction = pProgram->AddInstruction( AGKI_CMP_GREATER_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else
			{
				pProgram->TokenError( "Unrecognised variable type", tokenID );
				//return 0;
			}

			if ( cmpInstruction >= 0 )
			{
				if ( setInstruction >= 0 ) pProgram->EditInstruction( setInstruction, AGKI_SET_INSTRUCTION, cmpInstruction, origInstruction );
				if ( editInstruction >= 0 ) pProgram->EditInstruction( editInstruction, AGKI_EDIT_COND, cmpInstruction, alternateInstruction );
			}

			// jump to end of the loop if variable exceeds second expression
			pProgram->AddInstruction( AGKI_JUMP_COND, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			jumpEndPtr = pProgram->GetInstructionPosition();

skip_for_declaration:

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				pProgram->TokenError( "FOR expression must be followed by a new line", tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
			}

			while( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_NEXT )
			{
				int subConsumed = 0;
				int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
					{
						tokenID++;
						consumed++;
					}
				}
				else
				{
					tokenID += subConsumed;
					consumed+= subConsumed;
				}
			}

			if ( tokenID >= (int)pProgram->m_iNumTokens || pProgram->GetToken( tokenID )->iType != AGK_TOKEN_NEXT )
			{
				pProgram->TokenError( "FOR does not have a matching NEXT", forToken );
				return 0;
			}

			// jump here to step the var and continue the loop
			int loopContinue = pProgram->GetInstructionPosition() + 1;
			if ( !pProgram->IsSkippingInstructions() )
			{
				for ( int i = 0; i < pStepInstructions.m_iCount; i++ )
				{
					pProgram->m_pInstructions.Add( pStepInstructions.m_pData[ i ] );
				}

				// step the variable
				if ( pVar ) pProgram->AddInstruction( instructionInc, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (pVar->iVarID), 0 );

				pProgram->AddInstruction( AGKI_JUMP, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, loopStart, 0 );
				int loopEnd = pProgram->GetInstructionPosition() + 1;

				if ( jumpEndPtr >= 0 ) pProgram->EditInstruction( jumpEndPtr, AGKI_JUMP_COND, loopEnd, 0 );

				// check for continues and exits
				if ( loopStart >= 0 )
				{
					for ( int i = loopStart; i < loopEnd; i++ )
					{
						if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_CONTINUE )
						{
							// loop continue
							pProgram->EditInstruction( i, AGKI_JUMP, loopContinue, 0 );
						}
						else if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_EXIT )
						{
							// loop exit
							pProgram->EditInstruction( i, AGKI_JUMP, loopEnd, 0 );
						}
					}
				}
			}

			tokenID++;
			consumed++; // consume "next"

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_IDENTIFIER )
			{
				if ( pVar && pVar->sName.CompareTo( pProgram->GetToken( tokenID )->sValueRaw ) != 0 )
				{
					uString error;
					error.Format( "NEXT varaible \"%s\" does not match FOR variable \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), pVar->sNameOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					//return 0;
				}

				tokenID++;
				consumed++; // consume identifier
			}

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", NEXT must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens ) 
				{
					tokenID++;
					consumed++;
				}
			}
			consumed++; // consume new line
			return 1;
		}

		case AGK_TOKEN_LOOP_CONT:
		{
			// place holder, loops will replace this with a jump
			pProgram->AddInstruction( AGKI_UNMATCHED_CONTINUE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			tokenID++;
			consumed++; // consume "continue"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume new line
			return 1;	
		}

		case AGK_TOKEN_LOOP_EXIT:
		{
			// place holder, loops will replace this with a jump
			pProgram->AddInstruction( AGKI_UNMATCHED_EXIT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			tokenID++;
			consumed++; // consume "exit"

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			consumed++; // consume new line
			return 1;	
		}

		case AGK_TOKEN_IF:
		{
			tokenID++;
			consumed++; // consume "if"

			return ParseIfStatement( pFunction, pProgram, tokenID, consumed );
		}
		
		
		case AGK_TOKEN_SELECT:
		{
			int selectToken = tokenID;
			tokenID++;
			consumed++; // consume "select"

			int subConsumed = 0;
			int dataTypeSelect = 0;
			int result = ParseExpression( pFunction, 1, 0, dataTypeSelect, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
				{
					tokenID++;
					consumed++;
				}
			}
			else
			{
				tokenID += subConsumed;
				consumed += subConsumed;

				if ( dataTypeSelect != AGK_VARIABLE_INTEGER && dataTypeSelect != AGK_VARIABLE_FLOAT && dataTypeSelect != AGK_VARIABLE_STRING )
				{
					pProgram->TokenError( "SELECT expression must be an Integer, Float, or String data type", tokenID );
					//return 0;
				}
			}

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", SELECT must be followed by a single expression", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
				{
					tokenID++;
					consumed++;
				}
			}

			tokenID++;
			consumed++; // consume "EOL"

			while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
			{
				tokenID++;
				consumed++; // consume EOL
			}

			int startPtr = pProgram->GetInstructionPosition();

			while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CASE )
			{
				int caseToken = tokenID;

				tokenID++;
				consumed++; // consume "case"

				subConsumed = 0;
				int dataType = 0;
				int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
					{
						tokenID++;
						consumed++;
					}
				}
				else
				{
					tokenID += subConsumed;
					consumed += subConsumed;

					if ( dataTypeSelect > 0 && dataType != dataTypeSelect )
					{
						uString error;
						error.Format( "CASE expression (%s) must be the same data type as SELECT expression (%s)", getDataTypeName(dataType), getDataTypeName(dataTypeSelect) );
						pProgram->TokenError( error.GetStr(), tokenID );
						//return 0;
					}
				}

				int caseCount = 1; // number of expressions in this case statement
				while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
				{
					caseCount++;
					tokenID++;
					consumed++; // consume comma

					subConsumed = 0;
					dataType = 0;
					int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed );
					if ( result <= 0 ) 
					{
						//return result;
						while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
						{
							tokenID++;
							consumed++;
						}
					}
					else
					{
						tokenID += subConsumed;
						consumed += subConsumed;

						if ( dataTypeSelect > 0 && dataType != dataTypeSelect )
						{
							uString error;
							error.Format( "CASE expression (%s) must be the same data type as SELECT expression (%s)", getDataTypeName(dataType), getDataTypeName(dataTypeSelect) );
							pProgram->TokenError( error.GetStr(), tokenID );
							//return 0;
						}
					}
				}

				pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, caseCount, 0 );

				// jump to next case statement if conditions don't match
				int caseJumpPtr = -1;
				if ( dataType == AGK_VARIABLE_INTEGER ) caseJumpPtr = pProgram->AddInstruction( AGKI_JUMP_CASE_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				if ( dataType == AGK_VARIABLE_FLOAT ) caseJumpPtr = pProgram->AddInstruction( AGKI_JUMP_CASE_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				if ( dataType == AGK_VARIABLE_STRING ) caseJumpPtr = pProgram->AddInstruction( AGKI_JUMP_CASE_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", CASE expression must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					//return 0;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
					{
						tokenID++;
						consumed++;
					}
				}

				tokenID++;
				consumed++; // consume EOL

				while( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CASE_END )
				{
					if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CASE || pProgram->GetToken( tokenID )->iType == AGK_TOKEN_SELECT_END )
					{
						pProgram->TokenError( "CASE statement does not have a matching ENDCASE", caseToken );
						//return 0;
						break;
					}

					int subConsumed = 0;
					int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
					if ( result <= 0 ) 
					{
						//return result;
						while( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
						{
							tokenID++;
							consumed++;
						}
					}
					else
					{
						tokenID += subConsumed;
						consumed+= subConsumed;
					}
				}

				if ( tokenID >= (int)pProgram->m_iNumTokens )
				{
					pProgram->TokenError( "CASE statement does not have a matching ENDCASE", caseToken );
					return 0;
				}

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CASE_END )
				{
					if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CASE || pProgram->GetToken( tokenID )->iType == AGK_TOKEN_SELECT_END )
					{
						continue;
					}
					pProgram->TokenError( "CASE statement does not have a matching ENDCASE", caseToken );
					return 0;
				}

				tokenID++;
				consumed++; // consume ENDCASE

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", ENDCASE must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					//return 0;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
					{
						tokenID++;
						consumed++;
					}
				}

				tokenID++;
				consumed++; // consume EOL

				while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
				{
					tokenID++;
					consumed++; // consume EOL
				}

				// jump to the end of SELECT
				// don't know where to jump yet so use a placeholder
				pProgram->AddInstruction( AGKI_UNKNOWN, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 3, 0 );

				// modify previous jump to point here, AGKI_UNKNOWN won't change the instruction type
				int nextCasePtr = pProgram->GetInstructionPosition() + 1;
				if ( caseJumpPtr >= 0 ) pProgram->EditInstruction( caseJumpPtr, AGKI_UNKNOWN, nextCasePtr, 0 );
			}

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CASE_DEFAULT )
			{
				int caseToken = tokenID;

				tokenID++;
				consumed++; // consume CASEDEFAULT

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", CASE DEFAULT must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					//return 0;
					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
					{
						tokenID++;
						consumed++;
					}
				}

				tokenID++;
				consumed++; // consume EOL

				while( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CASE_END )
				{
					if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CASE || pProgram->GetToken( tokenID )->iType == AGK_TOKEN_SELECT_END )
					{
						pProgram->TokenError( "CASE DEFAULT statement does not have a matching ENDCASE", caseToken );
						break;
						//return 0;
					}

					int subConsumed = 0;
					int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
					if ( result <= 0 ) 
					{
						//return result;
						while( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
						{
							tokenID++;
							consumed++;
						}
					}
					else
					{
						tokenID += subConsumed;
						consumed+= subConsumed;
					}
				}

				if ( tokenID >= (int)pProgram->m_iNumTokens )
				{
					pProgram->TokenError( "CASE DEFAULT statement does not have a matching ENDCASE", caseToken );
					return 0;
				}

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CASE_END )
				{
					if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CASE && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_SELECT_END )
					{
						pProgram->TokenError( "CASE DEFAULT statement does not have a matching ENDCASE", caseToken );
						return 0;
					}
				}
				else
				{
					tokenID++;
					consumed++; // consume ENDCASE
				
					if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
					{
						uString error;
						error.Format( "Unexpected token \"%s\", ENDCASE must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
						pProgram->TokenError( error.GetStr(), tokenID );
						//return 0;
						while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
						{
							tokenID++;
							consumed++;
						}
					}

					tokenID++;
					consumed++; // consume EOL

					while ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
					{
						tokenID++;
						consumed++; // consume EOL
					}
				}
			}

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CASE )
			{
				pProgram->TokenError( "CASE DEFAULT must be the last case in a SELECT statement", tokenID );
				//return 0;
				while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_SELECT_END )
				{
					tokenID++;
					consumed++;
				}
			}

			if ( tokenID >= (int) pProgram->m_iNumTokens )
			{
				pProgram->TokenError( "SELECT does not have a matching ENDSELECT", selectToken );
				return 0;
			}

			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_SELECT_END )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", expected ENDSELECT", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				//return 0;
				while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL && tokenID < (int) pProgram->m_iNumTokens )
				{
					tokenID++;
					consumed++;
				}
			}
			else
			{
				tokenID++;
				consumed++; // consume ENDSELECT

				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
				{
					uString error;
					error.Format( "Unexpected token \"%s\", ENDSELECT must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					return 0;
				}

				tokenID++;
				consumed++; // consume EOL
			}

			if ( !pProgram->IsSkippingInstructions() )
			{
				int endPtr = pProgram->GetInstructionPosition();

				// we need to pop the select expression off the stack
				AGKInstruction popInstruction = AGKI_NOP;
				if ( dataTypeSelect == AGK_VARIABLE_INTEGER || dataTypeSelect == AGK_VARIABLE_FLOAT )
				{
					popInstruction = AGKI_POP_VAR;
				}
				else if ( dataTypeSelect == AGK_VARIABLE_STRING )
				{
					popInstruction = AGKI_POP_STRING;
				}

				// skip over loop and function hacks
				pProgram->AddInstruction( AGKI_UNKNOWN, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 3, 0 );

				// if the select contains an unmatched Continue, Exit, or ExitFunction then it will jump over this cleanup section
				// therefore it must be trapped to point here and a new Continue, Exit, or ExitFunction placed here instead

				for ( int i = startPtr; i <= endPtr; i++ )
				{
					if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_CONTINUE )
					{
						// replace Continue with jump to here, pop select expression, then place the new Continue
						pProgram->EditInstruction( i, AGKI_JUMP, pProgram->GetInstructionPosition()+1, 0 );
						pProgram->AddInstruction( popInstruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pProgram->AddInstruction( AGKI_UNMATCHED_CONTINUE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					}
					if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_EXIT )
					{
						// replace Exit with jump to here, pop select expression, then place the new Exit
						pProgram->EditInstruction( i, AGKI_JUMP, pProgram->GetInstructionPosition()+1, 0 );
						pProgram->AddInstruction( popInstruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pProgram->AddInstruction( AGKI_UNMATCHED_EXIT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					}
					if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_FUNC_EXIT )
					{
						// replace ExitFunction with jump to here, pop select expression, then place the new ExitFunction
						pProgram->EditInstruction( i, AGKI_JUMP, pProgram->GetInstructionPosition()+1, 0 );
						pProgram->AddInstruction( popInstruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
						pProgram->AddInstruction( AGKI_UNMATCHED_FUNC_EXIT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					}
				}

				// modify all the undefined jumps to point here
				endPtr = pProgram->GetInstructionPosition() + 1;
				for ( int i = startPtr; i < endPtr; i++ )
				{
					if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNKNOWN && pProgram->m_pInstructions.m_pData[ i ]->i == 3 )
					{
						pProgram->EditInstruction( i, AGKI_JUMP, endPtr, 0 );
					}
				}

				// pop select expression
				pProgram->AddInstruction( popInstruction, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			
			return 1;
		}

		// gosub
		case AGK_TOKEN_GOSUB:
		{
			tokenID++;
			consumed++; // consume "gosub"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", gosub must be followed by an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			int index = pProgram->FindLabel( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( index < 0 )
			{
				uString error;
				error.Format( "\"%s\" is not a valid label", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_LABEL_NAME;
			pProgram->GetToken( tokenID )->pExtraData = 0;

			pProgram->AddInstruction( AGKI_GOSUB, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, index, 0 );

			tokenID++;
			consumed++; // consume identifier
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", gosub must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume EOL
			return 1;
		}
		case AGK_TOKEN_RETURN:
		{
			tokenID++;
			consumed++; // consume "return"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", RETURN must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume EOL

			pProgram->AddInstruction( AGKI_GOSUB_RETURN, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			return 1;
		}

		case AGK_TOKEN_GOTO:
		{
			tokenID++;
			consumed++; // consume "goto"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", goto must be followed by an identifier", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			pProgram->GetToken( tokenID )->iTypeExtra = AGK_TOKEN_EXTRA_LABEL_NAME;
			pProgram->GetToken( tokenID )->pExtraData = 0;

			int index = pProgram->FindLabel( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
			if ( index < 0 )
			{
				uString error;
				error.Format( "\"%s\" is not a valid label", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}

			if ( pProgram->m_pLabels.m_pData[ index ]->pInFunction != pFunction )
			{
				pProgram->TokenError( "Cannot use GOTO to jump into or out of functions", tokenID );
				return 0;
			}

			pProgram->AddInstruction( AGKI_JUMP_LABEL, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, index, 0 );

			tokenID++;
			consumed++; // consume identifier
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", goto must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume EOL
			return 1;
		}

		case AGK_TOKEN_END:
		{
			pProgram->AddInstruction( AGKI_END, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			tokenID++;
			consumed++; // consume "end"
			if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", END must be followed by a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), tokenID );
				return 0;
			}
			consumed++; // consume EOL
			return 1;
		}

		default:
		{
			uString error;
			error.Format( "Unexpected token \"%s\"", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			return 0;
		}
	}

	return 1;
}

int Parser2::FirstPass( cProgramData *pProgram, int replaceConstants )
{
	int has_error = 0;
	m_iReplacedConstants = replaceConstants;

	// parse plugins
	for ( uint32_t t = 0; t < pProgram->m_iNumTokens; t++ )
	{
		if ( pProgram->GetToken( t )->iType != AGK_TOKEN_IMPORT_PLUGIN ) continue;
		if ( pProgram->GetToken( t+1 )->iType != AGK_TOKEN_IDENTIFIER ) continue;

		int identToken = t+1;

		uString sName, sRefName;
		sName.SetStr( pProgram->GetToken( t+1 )->sValueRaw );

		if ( pProgram->GetToken( t+2 )->iType != AGK_TOKEN_DEC_AS || pProgram->GetToken( t+3 )->iType != AGK_TOKEN_IDENTIFIER ) 
		{
			sRefName.SetStr( sName );
		}
		else
		{
			sRefName.SetStr( pProgram->GetToken( t+3 )->sValueRaw );
			identToken = t+3;
		}

		uString error;
		if ( !pProgram->AddPlugin( sName, sRefName, identToken, &error ) )
		{
			pProgram->TokenError( error.GetStr(), t );
			return 0; // fatal
		}
	}

	// parse constants
	for ( int c = 0; c < pProgram->m_pConstants.m_iCount; c++ )
	{
		uint32_t iTokenID = pProgram->m_pConstants.m_pData[ c ]->tokenID;
		
		iTokenID++;
		// constant name
		if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_IDENTIFIER )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", #constant must be followed by a name", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			pProgram->m_pConstants.m_pData[ c ]->valid = 0;
			has_error = 1;
			//return 0;
			continue;
		}

		if ( pProgram->FindPlugin( pProgram->GetToken( iTokenID )->sValueRaw.GetStr() ) >= 0 )
		{
			uString error;
			error.Format( "\"%s\" is already being used as a plugin name", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			pProgram->m_pConstants.m_pData[ c ]->valid = 0;
			has_error = 1;
			//return 0;
			continue;
		}

		if ( pProgram->FindFunction( pProgram->GetToken( iTokenID )->sValueRaw.GetStr() ) >= 0 )
		{
			uString error;
			error.Format( "\"%s\" is already being used as a function name", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			pProgram->m_pConstants.m_pData[ c ]->valid = 0;
			has_error = 1;
			//return 0;
			continue;
		}

		if ( pProgram->FindCommandName( pProgram->GetToken( iTokenID )->sValueRaw.GetStr() ) >= 0 )
		{
			uString error;
			error.Format( "\"%s\" is a reserved word", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			pProgram->m_pConstants.m_pData[ c ]->valid = 0;
			has_error = 1;
			//return 0;
			continue;
		}

		if ( pProgram->FindType( pProgram->GetToken( iTokenID )->sValueRaw.GetStr() ) >= 0 )
		{
			uString error;
			error.Format( "\"%s\" is already being used as a type name", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			pProgram->m_pConstants.m_pData[ c ]->valid = 0;
			has_error = 1;
			//return 0;
			continue;
		}

		if ( pProgram->FindLabel( pProgram->GetToken( iTokenID )->sValueRaw.GetStr() ) >= 0 )
		{
			uString error;
			error.Format( "\"%s\" is already being used as a label name", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			pProgram->m_pConstants.m_pData[ c ]->valid = 0;
			has_error = 1;
			//return 0;
			continue;
		}

		int identToken = iTokenID;

		// skip over equals if it exists
		iTokenID++;
		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_EQUALS )
		{
			iTokenID++;
		}

		pProgram->m_pConstants.m_pData[ c ]->tokenID = iTokenID;

		pProgram->m_pConstants.m_pData[ c ]->length = 0;
		while ( iTokenID < pProgram->m_iNumTokens && pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
		{
			if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_CONSTANT )
			{
				pProgram->TokenError( "#constant cannot contain another constant declaration", iTokenID );
				pProgram->m_pConstants.m_pData[ c ]->valid = 0;
				has_error = 1;
				//return 0;
			}

			if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_IDENTIFIER )
			{
				if ( pProgram->GetToken( iTokenID )->sValueRaw.CompareTo( pProgram->GetToken( identToken )->sValueRaw.GetStr() ) == 0 )
				{
					pProgram->TokenError( "#constant value cannot contain itself", iTokenID );
					pProgram->m_pConstants.m_pData[ c ]->valid = 0;
					has_error = 1;
					//return 0;
				}
			}

			iTokenID++;
			pProgram->m_pConstants.m_pData[ c ]->length++;
		}

		if ( !pProgram->m_pConstants.m_pData[ c ]->valid ) continue;

		if ( pProgram->m_pConstants.m_pData[ c ]->length == 0 )
		{
			pProgram->TokenError( "#constant must have a value", iTokenID );
			pProgram->m_pConstants.m_pData[ c ]->valid = 0;
			has_error = 1;
			//return 0;
			continue;
		}

		/*
		// constant value
		if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_INTEGER
		  && pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_FLOAT 
		  && pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_STRING )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", constant value must be an integer, float, or string", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			return 0;
		}

		iTokenID++;
		*/

		if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", constant value must be followed by a new line", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			pProgram->m_pConstants.m_pData[ c ]->valid = 0;
			has_error = 1;
			//return 0;
			continue;
		}
	}

	// replace constant identifiers
	if ( m_iReplacedConstants && pProgram->m_pConstants.m_iCount > 0 )
	{
		int extra = pProgram->m_pConstants.m_iCount * 100;
		if ( extra < 10000 ) extra = 10000;
		int newSize = pProgram->m_iNumTokens + extra;
		stToken **pNewTokens = new stToken*[ newSize ];
		uint32_t newBytes = sizeof(stToken*) * newSize;
		memset( pNewTokens, 0, newBytes );

		int newPtr = 0;
		for ( uint32_t t = 0; t < pProgram->m_iNumTokens; t++ )
		{
			if ( pProgram->GetToken( t )->iType == AGK_TOKEN_CONSTANT )
			{
				// skip constant declarations
				while ( pProgram->GetToken( t )->iType != AGK_TOKEN_EOL && t < pProgram->m_iNumTokens )
				{
					pNewTokens[ newPtr++ ] = pProgram->GetToken( t++ );
				}

				// copy end of line
				if (t < pProgram->m_iNumTokens)
				{
					pNewTokens[ newPtr++ ] = pProgram->GetToken( t );
				}
			}
			else
			{
				bool replaced = false;
				if ( pProgram->GetToken( t )->iType == AGK_TOKEN_IDENTIFIER )
				{
					int index = pProgram->FindConstant( pProgram->GetToken( t )->sValueRaw.GetStr() );
					if ( index >= 0 && pProgram->m_pConstants.m_pData[ index ]->valid )
					{
						// identifier is a constant, replace it with the value
						for ( int i = 0; i < pProgram->m_pConstants.m_pData[ index ]->length; i++ )
						{
							int constantTokenID = pProgram->m_pConstants.m_pData[ index ]->tokenID + i;
							pNewTokens[ newPtr ] = pProgram->NewToken();
							*(pNewTokens[ newPtr ]) = *(pProgram->m_pTokens[ constantTokenID ]);
							pNewTokens[ newPtr ]->iLine = pProgram->GetToken( t )->iLine;
							pNewTokens[ newPtr ]->iCharStart = pProgram->GetToken( t )->iCharStart;
							pNewTokens[ newPtr ]->iCharEnd = pProgram->GetToken( t )->iCharEnd;
							pNewTokens[ newPtr ]->iIncludeFile = pProgram->GetToken( t )->iIncludeFile;
							newPtr++;
							replaced = true;

							if ( newPtr+5000 >= newSize )
							{
								uint32_t newNewSize = newSize + 10000;
								stToken **pNewArray = new stToken*[ newNewSize ];
								uint32_t oldBytes = sizeof(stToken*) * newSize;
								uint32_t newBytes = sizeof(stToken*) * newNewSize;
								memcpy( pNewArray, pNewTokens, oldBytes );
								memset( pNewArray + newSize, 0, newBytes-oldBytes );

								delete [] pNewTokens;
								pNewTokens = pNewArray;
								newSize = newNewSize;
							}
						}
					}
				}
				
				if ( !replaced )
				{
					pNewTokens[ newPtr++ ] = pProgram->m_pTokens[ t ];

					if ( newPtr+5000 >= newSize )
					{
						uint32_t newNewSize = newSize + 10000;
						stToken **pNewArray = new stToken*[ newNewSize ];
						uint32_t oldBytes = sizeof(stToken*) * newSize;
						uint32_t newBytes = sizeof(stToken*) * newNewSize;
						memcpy( pNewArray, pNewTokens, oldBytes );
						memset( pNewArray + newSize, 0, newBytes-oldBytes );

						delete [] pNewTokens;
						pNewTokens = pNewArray;
						newSize = newNewSize;
					}
				}
			}
		}

		delete [] pProgram->m_pTokens;
		pProgram->m_pTokens = pNewTokens;
		pProgram->m_iNumTokens = newPtr;
		pProgram->m_iSizeTokens = newSize;

		// repair types, functions, and labels after the constant replacements
		AGKToken prevToken = AGK_TOKEN_UNKNOWN;
		for ( uint32_t t = 0; t < pProgram->m_iNumTokens; t++ )
		{
			AGKToken token = pProgram->GetToken( t )->iType;

			// functions
			if ( prevToken == AGK_TOKEN_DEC_FUNCTION && token == AGK_TOKEN_IDENTIFIER )
			{
				int index = pProgram->FindFunction( pProgram->GetToken( t )->sValueRaw.GetStr() );
				if ( index >= 0 )
				{
					pProgram->m_pFunctions.m_pData[ index ]->iTokenID = t-1;
				}
			}

			// types
			if ( prevToken == AGK_TOKEN_DEC_TYPE && token == AGK_TOKEN_IDENTIFIER )
			{
				int index = pProgram->FindType( pProgram->GetToken( t )->sValueRaw.GetStr() );
				if ( index >= 0 )
				{
					pProgram->m_pTypes.m_pData[ index ]->iTokenID = t-1;
					pProgram->GetToken( t )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE;
					pProgram->GetToken( t )->pExtraData = 0;
				}
			}

			// labels
			if ( token == AGK_TOKEN_LABEL )
			{
				int index = pProgram->FindLabel( pProgram->GetToken( t )->sValueRaw.GetStr() );
				if ( index >= 0 )
				{
					pProgram->m_pLabels.m_pData[ index ]->iTokenID = t;
				}
			}

			// constants
			if ( token == AGK_TOKEN_CONSTANT )
			{
				// constant text is just "#constant", constant identifier is next token
				int index = pProgram->FindConstant( pProgram->GetToken( t+1 )->sValueRaw.GetStr() );
				if ( index >= 0 )
				{
					int tokenID = t+2;
					if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EQUALS )
					{
						tokenID++;
					}
					pProgram->m_pConstants.m_pData[ index ]->tokenID = tokenID;
				}
			}

			prevToken = token;
		}
	}

	// parse type variables
	for ( int t = 0; t < pProgram->m_pTypes.m_iCount; t++ )
	{
		uint32_t iTokenID = pProgram->m_pTypes.m_pData[ t ]->iTokenID;
		pProgram->m_pTypes.m_pData[ t ]->m_pSubVars.SetSorted( 1, CompareVariable );

		iTokenID++;
		// type name
		if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_IDENTIFIER )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", Type must be followed by a type name", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			has_error = 1;
			//return 0;
			pProgram->m_pTypes.m_pData[ t ]->valid = 0;
			continue;
		}

		if ( pProgram->FindPlugin( pProgram->GetToken( iTokenID )->sValueRaw ) >= 0 )
		{
			uString error;
			error.Format( "Type name \"%s\" is already being used by a plugin", pProgram->m_pTypes.m_pData[ t ]->sNameOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			has_error = 1;
			//return 0;
			pProgram->m_pTypes.m_pData[ t ]->valid = 0;
			continue;
		}

		iTokenID++;
		if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", Type name must be followed by a new line", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			has_error = 1;
			//return 0;
			pProgram->m_pTypes.m_pData[ t ]->valid = 0;
			continue;
		}

		while ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_EOL )
		{
			iTokenID++;
		}

		int numVars = 0;
				
		while ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_DEC_TYPE_END ) 
		{
			do
			{
				pProgram->GetToken( iTokenID )->iTypeExtra = AGK_TOKEN_EXTRA_TYPE_SUB_VARIABLE;
				pProgram->GetToken( iTokenID )->pExtraData = pProgram->m_pTypes.m_pData[ t ];

				stVariable newVar;
				newVar.iScope = 0;
				int subConsumed = 0;
				int result = ParseTypeVariableDec( &newVar, pProgram, iTokenID, subConsumed );
				if ( result <= 0 ) 
				{
					has_error = 1;
					pProgram->m_pTypes.m_pData[ t ]->valid = 0;
					//return result;
					while ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
					{
						iTokenID++;
					}
					continue;
				}
				iTokenID += subConsumed;

				if ( pProgram->m_pTypes.m_pData[ t ]->sFirstVar.GetLength() == 0 )
				{
					pProgram->m_pTypes.m_pData[ t ]->sFirstVar.SetStr( newVar.sName );
				}

				if ( newVar.iType == AGK_VARIABLE_TYPE || newVar.iType == AGK_VARIABLE_ARRAY_TYPE )
				{
					if ( newVar.iTypeIndex == t )
					{
						uString error;
						error.Format( "Type \"%s\" cannot contain itself as a varaible", pProgram->m_pTypes.m_pData[ newVar.iTypeIndex ]->sNameOrig.GetStr() );
						pProgram->TokenError( error.GetStr(), iTokenID );
						has_error = 1;
						//return 0;
						pProgram->m_pTypes.m_pData[ t ]->valid = 0;
						continue;
					}
				}
				
				newVar.iTypeOrder = pProgram->m_pTypes.m_pData[ t ]->m_pSubVars.m_iCount;
				result = pProgram->m_pTypes.m_pData[ t ]->m_pSubVars.Add( &newVar, false );
				if ( result < 0 ) 
				{
					uString error;
					error.Format( "Variable \"%s\" has already been defined in this type", newVar.sNameOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), iTokenID );
					has_error = 1;
					//return 0;
					pProgram->m_pTypes.m_pData[ t ]->valid = 0;
				}
				
				numVars++;
			} while ( pProgram->GetToken( iTokenID++ )->iType == AGK_TOKEN_COMMA );
			
			// consumed a comma that didn't exist
			iTokenID--; 

			if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\"", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), iTokenID );
				pProgram->m_pTypes.m_pData[ t ]->valid = 0;
				has_error = 1;
				//return 0;

				while ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
				{
					iTokenID++;
				}
			}

			// can have multiple new lines between vars
			while ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_EOL )
			{
				iTokenID++;
			}

		} 

		pProgram->m_pTypes.m_pData[ t ]->iInitialised = 1;

		iTokenID++;

		if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", EndType must be followed by a new line", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), iTokenID );
			has_error = 1;
			//return 0;
			pProgram->m_pTypes.m_pData[ t ]->valid = 0;

			while ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
			{
				iTokenID++;
			}
		}

		if ( pProgram->m_pTypes.m_pData[ t ]->valid == 0 ) continue;

		// check there is at least one variable
		if ( numVars == 0 )
		{
			pProgram->TokenError( "Type must contain at least one variable", iTokenID );
			has_error = 1;
			//return 0;
			pProgram->m_pTypes.m_pData[ t ]->valid = 0;
			continue;
		}

		// re-order type variables by pointer first, to allow types to be byte aligned in 64 bit interpreters
		pProgram->m_pTypes.m_pData[ t ]->m_pSubVars.SetSorted( 1, CompareVariableType );

		for ( int i = 0; i < pProgram->m_pTypes.m_pData[ t ]->m_pSubVars.m_iCount; i++ )
		{
			pProgram->m_pTypes.m_pData[ t ]->m_pSubVars.m_pData[ i ]->iVarID = i;
		}

		// put them back in name order for searching
		pProgram->m_pTypes.m_pData[ t ]->m_pSubVars.SetSorted( 1, CompareVariable );
	}

	// look for variables
	uint32_t iTokenID = 0;
	int iFunctionBlock = 0;
	int iTypeBlock = 0;
	int iFunctionDef = 0;
	while( iTokenID < pProgram->m_iNumTokens )
	{	
		// functions
		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_FUNCTION )
		{
			iFunctionBlock = 1;
			iFunctionDef = 1;
			iTokenID++;
			continue;
		}

		if ( iFunctionBlock && pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_CLOSE_BRACKET )
		{
			iFunctionDef = 0;
			iTokenID++;
			continue;
		}

		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_FUNCTION_END )
		{
			iFunctionBlock = 0;
			iFunctionDef = 0;
			iTokenID++;
			continue;
		}

		// types
		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_TYPE )
		{
			iTypeBlock = 1;
			iTokenID++;
			continue;
		}

		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_TYPE_END )
		{
			iTypeBlock = 0;
			iTokenID++;
			continue;
		}

		if ( iFunctionDef ) 
		{
			iTokenID++;
			continue;
		}

		if ( iTypeBlock )
		{
			iTokenID++;
			continue;
		}

		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_SCOPE && pProgram->GetToken( iTokenID+1 )->iType == AGK_TOKEN_IDENTIFIER )
		{
			int scope = AGK_VARIABLE_GLOBAL;
			if ( pProgram->GetToken( iTokenID )->sValueRaw.CompareTo( "local" ) == 0 )
			{
				if ( iFunctionBlock )
				{
					while( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL ) iTokenID++;
					iTokenID++;
					continue;
				}
				scope = AGK_VARIABLE_LOCAL;
			}
			else if ( pProgram->GetToken( iTokenID )->sValueRaw.CompareTo( "global" ) == 0 )
			{
				if ( iFunctionBlock )
				{
					//pProgram->TokenWarning( "Declaring global variables inside functions is discouraged", iTokenID );
				}
			}

			iTokenID++;
			
			stVariable newVar;
			newVar.iScope = scope;
			newVar.iInitialised = 1;
			newVar.iTokenID = iTokenID;

			int subConsumed = 0;
			int result = ParseVariableDec( 1, &newVar, pProgram, iTokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				iTokenID++;
				has_error = 1;
				continue;
			}
			pProgram->AddVariable( newVar );
			iTokenID += subConsumed;

			while ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_COMMA )
			{
				iTokenID++;
				int subConsumed = 0;
				int result = ParseVariableDec( 1, &newVar, pProgram, iTokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					iTokenID++;
					has_error = 1;
					continue;
				}
				pProgram->AddVariable( newVar );
				iTokenID += subConsumed;
			}

			if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", variable declaration must be followed by a new line", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), iTokenID );
				//return 0;
				iTokenID++;
				has_error = 1;
				continue;
			}
			iTokenID++;
			continue;
		}

		int isImport = 0;
		if ( iTokenID > 0 && pProgram->GetToken( iTokenID-1 )->iType == AGK_TOKEN_IMPORT_PLUGIN ) isImport = 1;
		if ( iTokenID > 0 && pProgram->GetToken( iTokenID-1 )->iType == AGK_TOKEN_IMPORT_MODULE ) isImport = 1;
		if ( !isImport && pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_IDENTIFIER && pProgram->GetToken( iTokenID+1 )->iType == AGK_TOKEN_DEC_AS )
		{
			int scope = AGK_VARIABLE_LOCAL;
			if ( iFunctionBlock )
			{
				if ( pProgram->GetToken( iTokenID-1 )->iType != AGK_TOKEN_EOL )
				{
					while( pProgram->GetToken( iTokenID-1 )->iType != AGK_TOKEN_EOL ) iTokenID++;
				}
				else 
				{
					iTokenID++;
				}
				continue;
			}

			stVariable newVar;
			newVar.iScope = scope;
			newVar.iInitialised = 1;
			newVar.iTokenID = iTokenID;

			int subConsumed = 0;
			int result = ParseVariableDec( 1, &newVar, pProgram, iTokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				iTokenID++;
				has_error = 1;
				continue;
			}
			pProgram->AddVariable( newVar );
			iTokenID += subConsumed;

			while ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_COMMA )
			{
				iTokenID++;
				int subConsumed = 0;
				int result = ParseVariableDec( 1, &newVar, pProgram, iTokenID, subConsumed );
				if ( result <= 0 ) 
				{
					//return result;
					iTokenID++;
					has_error = 1;
					continue;
				}
				pProgram->AddVariable( newVar );
				iTokenID += subConsumed;
			}

			if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", variable declaration must be followed by a new line", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), iTokenID );
				//return 0;
				iTokenID++;
				has_error = 1;
				continue;
			}
			iTokenID++;
			continue;
		}

		iTokenID++;
	}

	// look for arrays, cannot be combined with variables as array declarations may use variables in their dimensions
	iTokenID = 0;
	iFunctionBlock = 0;
	iTypeBlock = 0;
	iFunctionDef = 0;
	while( iTokenID < pProgram->m_iNumTokens )
	{	
		// functions
		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_FUNCTION )
		{
			iFunctionBlock = 1;
			iFunctionDef = 1;
			iTokenID++;
			continue;
		}

		if ( iFunctionBlock && pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_CLOSE_BRACKET )
		{
			iFunctionDef = 0;
			iTokenID++;
			continue;
		}

		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_FUNCTION_END )
		{
			iFunctionBlock = 0;
			iFunctionDef = 0;
			iTokenID++;
			continue;
		}

		// types
		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_TYPE )
		{
			iTypeBlock = 1;
			iTokenID++;
			continue;
		}

		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_TYPE_END )
		{
			iTypeBlock = 0;
			iTokenID++;
			continue;
		}

		if ( iFunctionDef ) 
		{
			iTokenID++;
			continue;
		}

		if ( iTypeBlock )
		{
			iTokenID++;
			continue;
		}

		if ( pProgram->GetToken( iTokenID )->iType == AGK_TOKEN_DEC_ARRAY )
		{
			stVariable newVar;
			newVar.iScope = AGK_VARIABLE_GLOBAL;
			newVar.iInitialised = 1;
			newVar.iTokenID = iTokenID+1;
			
			int subConsumed = 0;
			iTokenID++;
			int result = ParseDimArray( 0, pProgram, &newVar, 0, iTokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return result;
				iTokenID++;
				has_error = 1;
				continue;
			}
			if ( newVar.iType > 0 ) pProgram->AddVariable( newVar );
			iTokenID += subConsumed;

			if ( pProgram->GetToken( iTokenID )->iType != AGK_TOKEN_EOL )
			{
				uString error;
				error.Format( "Unexpected token \"%s\", array declaration must be followed by a new line", pProgram->GetToken( iTokenID )->sValueRawOrig.GetStr() );
				pProgram->TokenError( error.GetStr(), iTokenID );
				//return 0;
				iTokenID++;
				has_error = 1;
				continue;
			}
			iTokenID++;
			continue;
		}

		iTokenID++;
	}

	if ( has_error ) return 0;
	else return 1;
}

int Parser2::ParseFunctions( cProgramData *pProgram )
{
	int has_error = 0;

	for ( int f = 0; f < pProgram->m_pFunctions.m_iCount; f++ )
	{
		stFunction *pFunction = pProgram->m_pFunctions.m_pData[ f ];
		pFunction->iNumParams = 0;
		pFunction->iReturnType = 0;

		int functionToken = pFunction->iTokenID;
		int identToken = pFunction->iTokenID + 1;

		// parse function parameters
		int tokenID = pFunction->iTokenID + 2;
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_OPEN_BRACKET )
		{
			uString error;
			error.Format( "Unexpected token \"%s\", function parameters must start with an open bracket", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
			pProgram->TokenError( error.GetStr(), tokenID );
			//return 0;
			has_error = 1;
			pFunction->valid = 0;

			while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL || pProgram->GetToken( tokenID-1 )->iType == AGK_TOKEN_COMMA )
			{
				tokenID++;
			}
		}
		else
		{
			tokenID++;

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_CLOSE_BRACKET )
			{
				tokenID++;
			}
			else
			{
				if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_IDENTIFIER )
				{
					uString error;
					if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
						error.Format( "Unexpected token \"%s\", first function parameter must be on the same line as the function declaration", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					else
						error.Format( "Unexpected token \"%s\", function must have a parmeter list or a close bracket", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					//return 0;
					has_error = 1;
					pFunction->valid = 0;

					while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL || pProgram->GetToken( tokenID-1 )->iType == AGK_TOKEN_COMMA )
					{
						tokenID++;
					}
				}
				else
				{
					int numParams = 0;
					int numStrings = 0;
					stVariable paramVars[ 32 ];
					int ref[ 32 ];
					do
					{
						if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA ) tokenID++;
						if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL ) tokenID++;

						if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_EOL )
						{
							pProgram->TokenError( "Function parameters must not have more than one new line between them", tokenID );
							//return 0;
							pFunction->valid = 0;
							has_error = 1;
							while( tokenID < (int)pProgram->m_iNumTokens-1 && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET ) tokenID++;
							break;
						}

						if ( numParams >= 32 )
						{
							pProgram->TokenError( "Too many parmaeters, a maximum of 32 are supported", tokenID );
							//return 0;
							pFunction->valid = 0;
							has_error = 1;
							while( tokenID < (int)pProgram->m_iNumTokens-1 && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET ) tokenID++;
							break;
						}
						int subConsumed = 0;
						paramVars[numParams].iScope = AGK_VARIABLE_FUNC_LOCAL;
						paramVars[numParams].iTokenID = tokenID;
						int result = ParseParamVariableDec( paramVars+numParams, ref[numParams], pProgram, tokenID, subConsumed );
						if ( result <= 0 ) 
						{
							//return result;
							pFunction->valid = 0;
							has_error = 1;
							while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET ) tokenID++;
							break;
						}
						if ( paramVars[numParams].iType == AGK_VARIABLE_STRING ) numStrings++;
						numParams++;
						tokenID += subConsumed;
					} while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA );

					if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_CLOSE_BRACKET )
					{
						uString error;
						error.Format( "Function parameters must end with a close bracket" );
						pProgram->TokenError( error.GetStr(), identToken );
						//return 0;
						has_error = 1;
						pFunction->valid = 0;
						while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) tokenID++;
						tokenID++;
					}
					else
					{
						tokenID++;
						if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
						{
							uString error;
							error.Format( "Unexpected token \"%s\", function declaration must end with a new line", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
							pProgram->TokenError( error.GetStr(), identToken );
							//return 0;
							has_error = 1;
							pFunction->valid = 0;
							while( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) tokenID++;
						}
						tokenID++;
					}

					pFunction->iNumParams = numParams;
					pFunction->iNumParamStrings = numStrings;

					// local offset needs separate string version
					int numVars = numParams - numStrings;
					int offsetVar = -numVars;
					int offsetString = -numStrings;
					for ( int i = 0; i < numParams; i++ )
					{
						paramVars[ i ].iInitialised = 1;
						pFunction->iParamsTypes[ i ] = paramVars[ i ].iType;
						pFunction->iParamNames[ i ] = paramVars[ i ].sName;
						if ( paramVars[ i ].iType == AGK_VARIABLE_TYPE || paramVars[ i ].iType == AGK_VARIABLE_ARRAY_TYPE ) pFunction->iParamsTypes[ i ] |= (paramVars[ i ].iTypeIndex << 8);
						if ( paramVars[ i ].iNumDimensions > 0 ) pFunction->iParamsTypes[ i ] |= ((paramVars[ i ].iNumDimensions & 7) << 5);

						pFunction->iParamRef[ i ] = ref[ i ];
						
						// set var offset into parameter data (-) or local data (+) depending on passing by reference or value respectively
						int offset = 0; // 0 is a special case that will calculate the local offset in AddVariable
						if ( ref[ i ] == 0 )
						{
							// pass by value, need to check for arrays and types
							if ( paramVars[ i ].iNumDimensions > 0 || paramVars[ i ].iType == AGK_VARIABLE_TYPE )
							{
								// point to local, copy from parameter
								paramVars[ i ].iCopyFrom = offsetVar++;
							}
							else
							{
								// point to parameter
								if ( paramVars[ i ].iType == AGK_VARIABLE_STRING ) offset = offsetString++;
								else offset = offsetVar++;
							}
						}
						else
						{
							// pass by reference, only arrays and types should be here
							if ( paramVars[ i ].iNumDimensions == 0 && paramVars[ i ].iType != AGK_VARIABLE_TYPE )
							{
								uString error;
								error.Format( "Parameter \"%s\" cannot be pased by reference, only types and arrays can be passed by reference", paramVars[ i ].sNameOrig.GetStr() );
								pProgram->TokenError( error.GetStr(), tokenID );
								//return 0;
								has_error = 1;
								pFunction->valid = 0;
							}
							else
							{
								// point to parameter, guaranteed not to be a string
								offset = offsetVar++;					
							}
						}

						if ( pFunction->AddVariable( paramVars+i, offset ) < 0 )
						{
							uString error;
							error.Format( "Parameter \"%s\" has already been defined", paramVars[ i ].sNameOrig.GetStr() );
							pProgram->TokenError( error.GetStr(), tokenID );
							//return 0;
							has_error = 1;
							pFunction->valid = 0;
						}
					}
				}
			}
		}

		// check for local variables and labels
		while ( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_FUNCTION_END )
		{
			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_IDENTIFIER && pProgram->GetToken( tokenID+1 )->iType == AGK_TOKEN_DEC_AS )
			{
				if ( tokenID == 0 || pProgram->GetToken( tokenID-1 )->iType != AGK_TOKEN_COMMA )
				{
					if ( tokenID > 0 && pProgram->GetToken( tokenID-1 )->iType == AGK_TOKEN_DEC_SCOPE )
					{
						if ( pProgram->GetToken( tokenID-1 )->sValueRaw.CompareTo( "global" ) == 0 )
						{
							while( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) tokenID++;
							tokenID++;
							continue;
						}
					}
					
					stVariable newVar;
					newVar.iScope = AGK_VARIABLE_FUNC_LOCAL;
					newVar.iInitialised = 1;
					newVar.iTokenID = tokenID;

					int subConsumed = 0;
					int result = ParseVariableDec( 1, &newVar, pProgram, tokenID, subConsumed );
					if ( result <= 0 ) 
					{
						//return result;
						has_error = 1;
						pFunction->valid = 0;
						tokenID++;
						continue;
					}
					if ( pFunction->AddVariable( &newVar ) < 0 )
					{
						uString error;
						error.Format( "Variable \"%s\" has already been defined in this function", newVar.sNameOrig.GetStr() );
						pProgram->TokenError( error.GetStr(), tokenID );
						//return 0;
						has_error = 1;
						pFunction->valid = 0;
					}
					tokenID += subConsumed;

					while( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_COMMA )
					{
						tokenID++;
						int subConsumed = 0;
						int result = ParseVariableDec( 1, &newVar, pProgram, tokenID, subConsumed );
						if ( result <= 0 ) 
						{
							//return result;
							has_error = 1;
							pFunction->valid = 0;
							tokenID++;
							continue;
						}
						if ( pFunction->AddVariable( &newVar ) < 0 )
						{
							uString error;
							error.Format( "Variable \"%s\" has already been defined in this function", newVar.sNameOrig.GetStr() );
							pProgram->TokenError( error.GetStr(), tokenID );
							//return 0;
							has_error = 1;
							pFunction->valid = 0;
						}
						tokenID += subConsumed;
					}
					continue;
				}
			}

			if ( pProgram->GetToken( tokenID )->iType == AGK_TOKEN_LABEL )
			{
				int index = pProgram->FindLabel( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
				if ( index < 0 )
				{
					uString error;
					error.Format( "Label \"%s\" was not correctly parsed, reason unknown", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
					pProgram->TokenError( error.GetStr(), tokenID );
					//return 0;
					has_error = 1;
					pFunction->valid = 0;
					tokenID++;
					continue;
				}
				
				pProgram->m_pLabels.m_pData[ index ]->pInFunction = pFunction;

				tokenID++;
				continue;
			}

			tokenID++;
		}

		if ( tokenID >= (int)pProgram->m_iNumTokens || pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_FUNCTION_END )
		{
			pProgram->TokenError( "EndFunction was not correctly parsed, reason unknown", tokenID );
			//return 0;
			has_error = 1;
			pFunction->valid = 0;
			continue;
		}

		// parse return type
		tokenID++;
		if ( pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
		{
			int subConsumed = 0;
			int dataType = 0;
			int result = ParseExpression( pFunction, 0, 0, dataType, 0, pProgram, tokenID, subConsumed, 1 );
			if ( result <= 0 ) 
			{
				//return result;
				has_error = 1;
				pFunction->valid = 0;
				continue;
			}
			pFunction->iReturnType = dataType;
		}
	}

	// do not merge these loops, all function parameters and return types must be processed before the function code can be parsed

	// parse function code
	for ( int f = 0; f < pProgram->m_pFunctions.m_iCount; f++ )
	{
		stFunction *pFunction = pProgram->m_pFunctions.m_pData[ f ];
		int tokenID = pFunction->iTokenID + 2;
		// skip function parameters
		while ( tokenID < (int)pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL )
		{
			tokenID++;
		}

		int localVarInstructionPtr = -1;

		if ( !m_iSkipInstructions )
		{
			// make sure the function isn't entered without a call
			pProgram->AddInstruction( AGKI_ILLEGAL_INSTRUCTION, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

			// function entry point
			pFunction->iInstructionPtr = pProgram->GetInstructionPosition() + 1; 
		
			// place holder for local variable space, function code may contain undefined variables without option explicit
			localVarInstructionPtr = pProgram->AddInstruction( AGKI_PUSH_BLANK, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
				
			// allocate local types and arrays, these are guaranteed to be defined already
			for ( int p = 0; p < pFunction->pFuncVars.m_iCount; p++ )
			{
				stVariable *pVar = pFunction->pFuncVars.m_pData[ p ];
				if ( pVar->iVarID > 0 )
				{
					if ( pVar->iType == AGK_VARIABLE_TYPE ) 
					{
						int varID = pVar->iVarID;
						int typeIndex = pVar->iTypeIndex;
						pProgram->AddInstruction( AGKI_LOCAL_VAR_DEC_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, typeIndex );
					}
					else if ( pVar->iNumDimensions > 0 ) 
					{
						int varID = pVar->iVarID;
						int typeIndex = pVar->iTypeIndex;
						int type = pVar->iType - AGK_VARIABLE_ARRAY_0;
						if ( type == AGK_VARIABLE_TYPE ) type |= (typeIndex << 8);
						int numDimensions = pVar->iNumDimensions;

						// allocate empty array, no size
						pProgram->AddInstruction( AGKI_LOCAL_VAR_DEC_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, 0 );

						if ( pVar->iCopyFrom == 0 )
						{
							// push all the dimension sizes on to the stack to allocate everything in one call
							pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, 0 );

							for ( int i = 0; i < numDimensions; i++ )
							{
								pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (pVar->pDefaultLengths[i]), 0 );
							}

							// allocate the array sizes with the given data type
							pProgram->AddInstruction( AGKI_ARRAY_BLOCK_RESIZE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, numDimensions, type );
						
							if( pVar->iNumDimensions == 1 && pVar->pDefaultI && pVar->pDefaultLengths && pVar->pDefaultLengths[0] >= 0 )
							{
								if ( type == AGK_VARIABLE_INTEGER ) 
								{
									pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->iVarID, 0 );
									for ( int i = 0; i <= pVar->pDefaultLengths[0]; i++ )
									{
										pProgram->AddInstruction( AGKI_PUSH_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->pDefaultI[ i ], 0 );
									}
									pProgram->AddInstruction( AGKI_ARRAY_ASSIGN_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->pDefaultLengths[0]+1, 1 );
								}
								else if ( type == AGK_VARIABLE_FLOAT ) 
								{
									pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->iVarID, 0 );
									for ( int i = 0; i <= pVar->pDefaultLengths[0]; i++ )
									{
										converter c; c.f = pVar->pDefaultF[ i ];
										pProgram->AddInstruction( AGKI_PUSH_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, c.i, 1 );
									}
									pProgram->AddInstruction( AGKI_ARRAY_ASSIGN_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->pDefaultLengths[0]+1, 1 );
								}
								else if ( type == AGK_VARIABLE_STRING ) 
								{
									pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->iVarID, 0 );
									for ( int i = 0; i <= pVar->pDefaultLengths[0]; i++ )
									{
										pProgram->AddInstruction( AGKI_PUSH_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->pDefaultI[ i ], 0 );
									}
									pProgram->AddInstruction( AGKI_ARRAY_ASSIGN_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, pVar->pDefaultLengths[0]+1, 1 );
								}
							}
						}
					}
				}

				// check if local variable copies a parameter
				if ( pVar->iCopyFrom < 0 )
				{
					if ( pVar->iNumDimensions > 0 )
					{
						pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (pVar->iVarID), 0 );
						pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (pVar->iCopyFrom), 0 );
						pProgram->AddInstruction( AGKI_STORE_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					}
					else
					{
						pProgram->AddInstruction( AGKI_PUSH_LOCAL_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (pVar->iVarID), 0 );
						pProgram->AddInstruction( AGKI_PUSH_LOCAL_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (pVar->iCopyFrom), 0 );
						pProgram->AddInstruction( AGKI_STORE_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
					}
				}
			}
		}

		tokenID++;

		while ( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_DEC_FUNCTION_END )
		{
			int subConsumed = 0;
			int result = ParseStatement( pFunction, pProgram, tokenID, subConsumed );
			if ( result <= 0 ) 
			{
				//return 0;
				has_error = 1;
				pFunction->valid = 0;
				while ( tokenID < (int) pProgram->m_iNumTokens && pProgram->GetToken( tokenID )->iType != AGK_TOKEN_EOL ) tokenID++;
				continue;
			}
			else tokenID += subConsumed;
		}

		if ( tokenID >= (int)pProgram->m_iNumTokens ) return 0;

		if ( pProgram->IsSkippingInstructions() )
		{
			continue;
		}

		// retroactively modify local variable space on stack
		if ( pFunction->iNumLocalVars > 0 || pFunction->iNumLocalStrings > 0 )
		{
			pProgram->EditInstruction( localVarInstructionPtr, AGKI_PUSH_BLANK, (pFunction->iNumLocalVars), (pFunction->iNumLocalStrings) );
		}
		else
		{
			pProgram->EditInstruction( localVarInstructionPtr, AGKI_NOP, 0, 0 );
		}

		// parse end function return value
		if ( pFunction->iReturnType > 0 )
		{
			int returnType = pFunction->iReturnType & 0x1f;
			int returnDimensions = (pFunction->iReturnType >> 5) & 7;
			int numVarParams = ((int)pFunction->iNumParams) - pFunction->iNumParamStrings;
			if ( returnType == AGK_VARIABLE_TYPE )
			{
				pProgram->AddInstruction( AGKI_PUSH_LOCAL_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 );
			}
			else if ( returnDimensions > 0 )
			{
				pProgram->AddInstruction( AGKI_PUSH_LOCAL_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 );
			}
			tokenID++;
			int dataType = 0;
			int subConsumed = 0;
			int result = ParseExpression( pFunction, 1, 0, dataType, &(pProgram->m_pInstructions), pProgram, tokenID, subConsumed, 0 );
			if ( result <= 0 ) 
			{
				//return result;
				has_error = 1;
				pFunction->valid = 0;
				continue;
			}
			
			if ( dataType != pFunction->iReturnType )
			{
				pProgram->TokenError( "EndFunction type could not be determined, try declaring the variable explicitly with a type", tokenID );
			}

			// store return type on stack in previously determined location
			if ( returnDimensions > 0 )
			{
				pProgram->AddInstruction( AGKI_STORE_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
			}
			else
			{
				switch( returnType )
				{
					case AGK_VARIABLE_INTEGER: pProgram->AddInstruction( AGKI_STORE_LOCAL_INT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 ); break;
					case AGK_VARIABLE_FLOAT: pProgram->AddInstruction( AGKI_STORE_LOCAL_FLOAT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-numVarParams), 0 ); break;
					case AGK_VARIABLE_STRING: pProgram->AddInstruction( AGKI_STORE_LOCAL_STRING, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (-1-pFunction->iNumParamStrings), 0 ); break;
					case AGK_VARIABLE_TYPE: pProgram->AddInstruction( AGKI_STORE_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 ); break;
					default: Error("Unrecognised variable type", 0, 0);
				}
			}
		}

		// look for exitfunction calls and replace them with the jump instruction
		int functionEndPtr = pProgram->GetInstructionPosition()+1;
		for ( int i = pFunction->iInstructionPtr; i < functionEndPtr; i++ )
		{
			if ( pProgram->m_pInstructions.m_pData[ i ]->iInstruction == AGKI_UNMATCHED_FUNC_EXIT )
			{
				pProgram->EditInstruction( i, AGKI_JUMP, functionEndPtr, 0 );
			}
		}
		
		// clean up local variables
		for ( int p = 0; p < pFunction->pFuncVars.m_iCount; p++ )
		{
			if ( pFunction->pFuncVars.m_pData[ p ]->iVarID > 0 )
			{
				if ( pFunction->pFuncVars.m_pData[ p ]->iType == AGK_VARIABLE_TYPE ) 
				{
					int varID = pFunction->pFuncVars.m_pData[ p ]->iVarID;
					pProgram->AddInstruction( AGKI_LOCAL_VAR_DELETE_TYPE, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, 0 );
				}
				else if ( pFunction->pFuncVars.m_pData[ p ]->iNumDimensions > 0 ) 
				{
					int varID = pFunction->pFuncVars.m_pData[ p ]->iVarID;
					pProgram->AddInstruction( AGKI_LOCAL_VAR_DELETE_ARRAY, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, varID, 0 );
				}
			}
		}

		if ( pFunction->iNumLocalVars > 0 || pFunction->iNumLocalStrings > 0 )
		{
			pProgram->AddInstruction( AGKI_POP_BLANK, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, (pFunction->iNumLocalVars), (pFunction->iNumLocalStrings) );
		}

		// return control, caller cleans up parameters
		pProgram->AddInstruction( AGKI_FUNCTION_EXIT, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );

		// guard instruction to make sure the function returns
		pProgram->AddInstruction( AGKI_ILLEGAL_INSTRUCTION, pProgram->GetToken( tokenID )->iLine, pProgram->GetToken( tokenID )->iIncludeFile, 0, 0 );
	}

	if ( has_error ) return 0;
	else return 1;
}
