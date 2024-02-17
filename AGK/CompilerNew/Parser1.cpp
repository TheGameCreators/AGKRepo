#include "AGKCompiler2.h"
#include "ProgramData.h"
#include "Parser1.h"
#include "cFile.h"

using namespace AGK_Compiler;

const char* GetTokenString( AGKToken token )
{
	switch( token )
	{
		case AGK_TOKEN_UNKNOWN: return "Unknown";

		case AGK_TOKEN_INTEGER: return "Integer";
		case AGK_TOKEN_FLOAT: return "Float";
		case AGK_TOKEN_STRING: return "String";
		case AGK_TOKEN_IDENTIFIER: return "Identifier";

		case AGK_TOKEN_DEC_SCOPE: return "Scope";
		case AGK_TOKEN_DEC_AS: return "As";
		case AGK_TOKEN_DEC_VAR_TYPE: return "VarType";
		case AGK_TOKEN_DEC_REFERENCE: return "Reference";
		case AGK_TOKEN_DEC_FUNCTION: return "FunctionDec";
		case AGK_TOKEN_DEC_FUNCTION_END: return "EndFunctionDec";
		case AGK_TOKEN_DEC_ARRAY: return "ArrayDec";
		case AGK_TOKEN_DEC_TYPE: return "TypeDec";
		case AGK_TOKEN_DEC_TYPE_END: return "EndTypeDec";

		case AGK_TOKEN_UNDEC_ARRAY: return "ArrayUnDec";
		case AGK_TOKEN_REDEC_ARRAY: return "ArrayReDec";

		case AGK_TOKEN_LABEL: return "Label";
		case AGK_TOKEN_INCLUDE: return "Include";
		case AGK_TOKEN_IMPORT_PLUGIN: return "ImportPlugin";
		case AGK_TOKEN_IMPORT_MODULE: return "ImportModule";
		case AGK_TOKEN_INSERT: return "Insert";
		case AGK_TOKEN_CONSTANT: return "Constant";
		case AGK_TOKEN_COMPANY_NAME: return "CompanyName";
		case AGK_TOKEN_COMP_DIRECTIVE: return "Directive";
		case AGK_TOKEN_EXPORT: return "Export";
		case AGK_TOKEN_RENDERER: return "RendererMode";

		case AGK_TOKEN_EQUALS: return "Equals";
		case AGK_TOKEN_COMPARISON: return "Comparison";
		case AGK_TOKEN_UNARY_OP: return "UnaryOp";
		case AGK_TOKEN_BINARY_OP: return "BinaryOp";
		//case AGK_TOKEN_BOOL_OP: return "BoolOp";
		//case AGK_TOKEN_NOT: return "Not";

		case AGK_TOKEN_REMSTART: return "Remstart";
		case AGK_TOKEN_REMEND: return "Remend";
		case AGK_TOKEN_COMMENT: return "Comment";

		case AGK_TOKEN_DOT: return "Dot";
		case AGK_TOKEN_COMMA: return "Comma";
		case AGK_TOKEN_EOL: return "EOL";
		case AGK_TOKEN_OPEN_BRACKET: return "OpenBracket";
		case AGK_TOKEN_CLOSE_BRACKET: return "CloseBracket";
		case AGK_TOKEN_OPEN_SQR_BRACKET: return "OpenSqrBracket";
		case AGK_TOKEN_CLOSE_SQR_BRACKET: return "CloseSqrBracket";

		case AGK_TOKEN_IF: return "If";
		case AGK_TOKEN_THEN: return "Then";
		case AGK_TOKEN_ELSE: return "Else";
		case AGK_TOKEN_ELSEIF: return "ElseIf";
		case AGK_TOKEN_ENDIF: return "Endif";

		case AGK_TOKEN_DO: return "Do";
		case AGK_TOKEN_LOOP: return "Loop";
		case AGK_TOKEN_WHILE: return "While";
		case AGK_TOKEN_ENDWHILE: return "EndWhile";
		case AGK_TOKEN_FOR: return "For";
		case AGK_TOKEN_TO: return "To";
		case AGK_TOKEN_STEP: return "Step";
		case AGK_TOKEN_NEXT: return "Next";
		case AGK_TOKEN_REPEAT: return "Repeat";
		case AGK_TOKEN_UNTIL: return "Until";

		case AGK_TOKEN_LOOP_EXIT: return "LoopExit";
		case AGK_TOKEN_LOOP_CONT: return "LoopCont";
		case AGK_TOKEN_FUNCTION_EXIT: return "FunctionExit";
		case AGK_TOKEN_END: return "End";
		case AGK_TOKEN_GOTO: return "Goto";
		case AGK_TOKEN_GOSUB: return "Gosub";
		case AGK_TOKEN_RETURN: return "Return";

		case AGK_TOKEN_SELECT: return "Select";
		case AGK_TOKEN_CASE: return "Case";
		case AGK_TOKEN_CASE_DEFAULT: return "CaseDefault";
		case AGK_TOKEN_SELECT_END: return "SelectEnd";
		case AGK_TOKEN_CASE_END: return "CaseEnd";

		case AGK_TOKEN_INC: return "Inc";
		case AGK_TOKEN_DEC: return "Dec";

		case AGK_TOKEN_FOLDSTART: return "FoldStart";
		case AGK_TOKEN_FOLDEND: return "FoldEnd";

		default: return "Invalid";
	}
}

int Parser1::g_iSorted = 0;
int Parser1::g_iNumKeywords = 0;
stKeyword Parser1::g_pKeywords[] = { { "rem", AGK_TOKEN_COMMENT },
					      { "remstart", AGK_TOKEN_REMSTART },
						  { "remend", AGK_TOKEN_REMEND },
						  { "foldstart", AGK_TOKEN_FOLDSTART },
						  { "foldend", AGK_TOKEN_FOLDEND },
						  { "dim", AGK_TOKEN_DEC_ARRAY },
						  { "undim", AGK_TOKEN_UNDEC_ARRAY },
						  { "redim", AGK_TOKEN_REDEC_ARRAY },
						  { "global", AGK_TOKEN_DEC_SCOPE },
						  { "local", AGK_TOKEN_DEC_SCOPE },
						  { "end", AGK_TOKEN_END },
						  { "goto", AGK_TOKEN_GOTO },
						  { "gosub", AGK_TOKEN_GOSUB },
						  { "return", AGK_TOKEN_RETURN },
						  { "function", AGK_TOKEN_DEC_FUNCTION },
						  { "exitfunction", AGK_TOKEN_FUNCTION_EXIT },
						  { "endfunction", AGK_TOKEN_DEC_FUNCTION_END },
						  { "do", AGK_TOKEN_DO },
						  { "loop", AGK_TOKEN_LOOP },
						  { "repeat", AGK_TOKEN_REPEAT },
						  { "until", AGK_TOKEN_UNTIL },
						  { "while", AGK_TOKEN_WHILE },
						  { "endwhile", AGK_TOKEN_ENDWHILE },
						  { "for", AGK_TOKEN_FOR },
						  { "to", AGK_TOKEN_TO },
						  { "step", AGK_TOKEN_STEP },
						  { "next", AGK_TOKEN_NEXT },
						  { "exit", AGK_TOKEN_LOOP_EXIT },
						  { "continue", AGK_TOKEN_LOOP_CONT },
						  { "if", AGK_TOKEN_IF },
						  { "then", AGK_TOKEN_THEN },
						  { "else", AGK_TOKEN_ELSE },
						  { "elseif", AGK_TOKEN_ELSEIF },
						  { "endif", AGK_TOKEN_ENDIF },
						  { "select", AGK_TOKEN_SELECT },
						  { "case", AGK_TOKEN_CASE },
						  { "endcase", AGK_TOKEN_CASE_END },
						  { "endselect", AGK_TOKEN_SELECT_END },
						  { "inc", AGK_TOKEN_INC },
						  { "dec", AGK_TOKEN_DEC },
						  { "as", AGK_TOKEN_DEC_AS },
						  { "integer", AGK_TOKEN_DEC_VAR_TYPE },
						  { "float", AGK_TOKEN_DEC_VAR_TYPE },
						  { "string", AGK_TOKEN_DEC_VAR_TYPE },
						  { "and", AGK_TOKEN_BINARY_OP },
						  { "or", AGK_TOKEN_BINARY_OP },
						  { "not", AGK_TOKEN_UNARY_OP },
						  { "type", AGK_TOKEN_DEC_TYPE },
						  { "endtype", AGK_TOKEN_DEC_TYPE_END },
						  { "ref", AGK_TOKEN_DEC_REFERENCE } };
//						  { "mod", AGK_TOKEN_BINARY_OP } };

int CompareKeyword( const void* a, const void* b )
{
	stKeyword *k1 = (stKeyword*) a;
	stKeyword *k2 = (stKeyword*) b;

	return strcmp( k1->szKeyword, k2->szKeyword );
}

bool IsIdentifierChar( char c )
{
	if ( c >= '0' && c <= '9' ) return true;
	if ( c >= 'A' && c <= 'Z' ) return true;
	if ( c >= 'a' && c <= 'z' ) return true;
	if ( c == '_' ) return true;

	return false;
}

bool IsIdentifierCharFirst( char c )
{
	if ( c >= 'A' && c <= 'Z' ) return true;
	if ( c >= 'a' && c <= 'z' ) return true;
	if ( c == '_' ) return true;

	return false;
}

bool IsIdentifierCharLast( char c )
{
	if ( c >= '0' && c <= '9' ) return true;
	if ( c >= 'A' && c <= 'Z' ) return true;
	if ( c >= 'a' && c <= 'z' ) return true;
	if ( c == '_' ) return true;
	if ( c == '#' || c == '$' ) return true;

	return false;
}

bool IsNumber( char c )
{
	if ( c >= '0' && c <= '9' ) return true;
	return false;
}

bool IsSeparator( char c )
{
	if ( c == ' ' ) return true;
	if ( c == '+' ) return true;
	if ( c == '-' ) return true;
	if ( c == '*' ) return true;
	if ( c == '/' ) return true;
	if ( c == '^' ) return true;
	if ( c == '\t' ) return true;
	if ( c == '\r' ) return true;
	if ( c == '\n' ) return true;
	if ( c == '(' ) return true;
	if ( c == ')' ) return true;
	if ( c == '[' ) return true;
	if ( c == ']' ) return true;
	if ( c == ',' ) return true;
	if ( c == '.' ) return true;
	if ( c == ':' ) return true;
	if ( c == '<' ) return true;
	if ( c == '>' ) return true;
	if ( c == '=' ) return true;
	if ( c == '&' ) return true;
	if ( c == '|' ) return true;
	if ( c == '~' ) return true;
	if ( c == '`' ) return true;
	if ( c == 0 ) return true;

	return false;
}

bool IsEOL( char c )
{
	if ( c == '\r' ) return true;
	if ( c == '\n' ) return true;
	if ( c == ':' ) return true;
	if ( c == 0 ) return true;

	return false;
}

bool IsWhiteSpace( char c )
{
	if ( c == '\r' ) return true;
	if ( c == '\n' ) return true;
	if ( c == '\t' ) return true;
	if ( c == ' ' ) return true;
	if ( c == 0 ) return true;

	return false;
}

void Parser1::Parser1Init()
{
	if ( g_iSorted ) return;
	g_iSorted = 1;

	g_iNumKeywords = sizeof(g_pKeywords)/sizeof(stKeyword);
	qsort( g_pKeywords, g_iNumKeywords, sizeof(stKeyword), CompareKeyword );
}

Parser1::Parser1()
{
	Reset();
}

Parser1::~Parser1()
{
	
}

void Parser1::Reset()
{
	iCommentBlock = 0;
	iFunctionBlock = 0;
	iSelectBlock = 0;
	iTypeBlock = 0;

	iFunctionLine = 0;
	iTypeLine = 0;
}

void Parser1::SetIncludeMode( IncludeMode mode )
{
	includeMode = mode;
}

AGKToken Parser1::ParseKeyword( const char* str )
{
	int high = g_iNumKeywords-1;
	int low = 0;
	
	// binary search
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = strcmp( g_pKeywords[ mid ].szKeyword, str );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else return g_pKeywords[ mid ].iToken;
	}
	
	return AGK_TOKEN_UNKNOWN;
}

AGKToken Parser1::ParseTokenCommentBlock( AGKToken prevToken, const char* str, int &consumed, uString &error )
{
	if ( !str ) 
	{
		error.SetStr( "ParseToken encountered a null string" );
		return AGK_TOKEN_UNKNOWN;
	}

	if ( !*str ) 
	{
		error.SetStr( "ParseToken encountered an empty string" );
		return AGK_TOKEN_UNKNOWN;
	}

	consumed = 0;

	while( *str )
	{
		if ( *str == '*' && *(str+1) == '/' ) 
		{
			consumed += 2;
			return AGK_TOKEN_REMEND;
		} 
		else if ( *str == 'r' && strncmp( str, "remend", 6 ) == 0 )
		{
			if ( !IsWhiteSpace( *(str+6) ) )
			{
				error.SetStr("Remend must be follwed by white space");
				return AGK_TOKEN_UNKNOWN;
			}
			else
			{
				consumed += 6;
				return AGK_TOKEN_REMEND;
			}
		}

		consumed++;
		str++;
	}

	return AGK_TOKEN_COMMENT;
}

AGKToken Parser1::ParseToken( AGKToken prevprevToken, AGKToken prevToken, const char* str, int &consumed, uString &error )
{
	const char* origStr = str;

	if ( !str ) 
	{
		error.SetStr( "ParseToken encountered a null string" );
		return AGK_TOKEN_UNKNOWN;
	}

	if ( !*str ) 
	{
		error.SetStr( "ParseToken encountered an empty string" );
		return AGK_TOKEN_UNKNOWN;
	}

	consumed = 1;

	switch( *str )
	{
		// operators
		case '+': return AGK_TOKEN_BINARY_OP;
		case '-': 
		{	
			if ( prevprevToken == AGK_TOKEN_CONSTANT ) 
			{
				if ( !IsNumber(*(str+1)) ) return AGK_TOKEN_UNARY_OP;
			}
			else if ( prevToken == AGK_TOKEN_IDENTIFIER
				   || prevToken == AGK_TOKEN_INTEGER
				   || prevToken == AGK_TOKEN_FLOAT
				   || prevToken == AGK_TOKEN_CLOSE_BRACKET 
				   || prevToken == AGK_TOKEN_CLOSE_SQR_BRACKET ) return AGK_TOKEN_BINARY_OP;
			else 
			{
				if ( !IsNumber(*(str+1)) ) return AGK_TOKEN_UNARY_OP;
			}
			break;
		}
		case '!': return AGK_TOKEN_UNARY_OP;
		case '^': return AGK_TOKEN_BINARY_OP;
		case '&': 
		{
			if ( *(str+1) == '&' ) 
			{
				consumed++;
				return AGK_TOKEN_BINARY_OP;
			}
		}
		case '|': 
		{
			if ( *(str+1) == '|' ) 
			{
				consumed++;
				return AGK_TOKEN_BINARY_OP;
			}
		}
		case '~': 
		{
			if ( *(str+1) == '~' ) 
			{
				consumed++;
				return AGK_TOKEN_BINARY_OP;
			}
		}

		// new lines
		case '\r': if ( *(str+1) == '\n' ) consumed++; // no break
		case '\n': return AGK_TOKEN_EOL;
		case ':': return AGK_TOKEN_EOL;

		// directives
		case '#': 
		{
			if ( strncmp( str+1, "include", 7 ) == 0 && !IsIdentifierChar(*(str+8)) )
			{
				consumed += 7;
				return AGK_TOKEN_INCLUDE;
			}
			else if ( strncmp( str+1, "insert", 6 ) == 0 && !IsIdentifierChar(*(str+7)) )
			{
				consumed += 6;
				return AGK_TOKEN_INSERT;
			}
			else if ( strncmp( str+1, "constant", 8 ) == 0 && !IsIdentifierChar(*(str+9)) )
			{
				consumed += 8;
				return AGK_TOKEN_CONSTANT;
			}
			else if ( strncmp( str+1, "option_explicit", 15 ) == 0 && !IsIdentifierChar(*(str+16)) )
			{
				consumed += 15;
				return AGK_TOKEN_COMP_DIRECTIVE;
			}
			else if ( strncmp( str+1, "company_name", 12 ) == 0 && !IsIdentifierChar(*(str+13)) )
			{
				consumed += 12;
				return AGK_TOKEN_COMPANY_NAME;
			}
			else if ( strncmp( str+1, "import_plugin", 13 ) == 0 && !IsIdentifierChar(*(str+14)) )
			{
				consumed += 13;
				return AGK_TOKEN_IMPORT_PLUGIN;
			}
			else if ( strncmp( str+1, "import_module", 13 ) == 0 && !IsIdentifierChar(*(str+14)) )
			{
				consumed += 13;
				return AGK_TOKEN_IMPORT_MODULE;
			}
			else if ( strncmp( str+1, "export", 6 ) == 0 && !IsIdentifierChar(*(str+7)) )
			{
				consumed += 6;
				return AGK_TOKEN_EXPORT;
			}
			else if ( strncmp( str+1, "renderer", 8 ) == 0 && !IsIdentifierChar(*(str+9)) )
			{
				consumed += 8;
				return AGK_TOKEN_RENDERER;
			}
			else 
			{
				error.SetStr( "Unrecognised compiler directive, only compiler directives may begin with a hash" );
				return AGK_TOKEN_UNKNOWN;
			}
		}

		// comparison and assignment, also left and right shift are caught here
		case '=': 
		{
			if ( *(str+1) == '<' )
			{
				consumed++;
				return AGK_TOKEN_COMPARISON;
			}
			else if ( *(str+1) == '>' )
			{
				consumed++;
				return AGK_TOKEN_COMPARISON;
			}
			else return AGK_TOKEN_EQUALS;
		}
		case '<':
		{
			if ( *(str+1) == '<' )
			{
				consumed++;
				return AGK_TOKEN_BINARY_OP;
			}
			else
			{
				if ( *(str+1) == '=' || *(str+1) == '>' ) consumed++;
				return AGK_TOKEN_COMPARISON;
			}
		}
		case '>':  
		{
			if ( *(str+1) == '>' )
			{
				consumed++;
				return AGK_TOKEN_BINARY_OP;
			}
			else
			{
				if ( *(str+1) == '=' ) consumed++;
				return AGK_TOKEN_COMPARISON;
			}
		}

		// comments
		case '/':
		{
			if ( *(str+1) == '/' ) 
			{
				consumed++;
				return AGK_TOKEN_COMMENT;
			}
			else if ( *(str+1) == '*' ) 
			{
				consumed++;
				return AGK_TOKEN_REMSTART;
			}
			else return AGK_TOKEN_BINARY_OP;
		}
		case '`': return AGK_TOKEN_COMMENT;
		case '*':
		{
			if ( *(str+1) == '/' ) 
			{
				consumed++;
				error.SetStr("End of comment block found without matching start");
				return AGK_TOKEN_UNKNOWN;
			}
			else return AGK_TOKEN_BINARY_OP;
		}
		
		// misc
		case '.': 
		{
			if ( !IsNumber( *(str+1) ) ) return AGK_TOKEN_DOT;
			else break;
		}
		case ',': return AGK_TOKEN_COMMA;
		case '(': return AGK_TOKEN_OPEN_BRACKET;
		case ')': return AGK_TOKEN_CLOSE_BRACKET;
		case '[': return AGK_TOKEN_OPEN_SQR_BRACKET;
		case ']': return AGK_TOKEN_CLOSE_SQR_BRACKET;

		// "case default" is the only two word keyword 
		case 'c': 
		{
			if ( *(str+1) == 'a' && *(str+2) == 's' && *(str+3) == 'e' && !IsIdentifierChar(*(str+4)) ) 
			{
				if ( *(str+5) == 'd' && *(str+6) == 'e' 
				  && *(str+7) == 'f' && *(str+8) == 'a' 
				  && *(str+9) == 'u' && *(str+10) == 'l' 
				  && *(str+11) == 't' && !IsIdentifierChar(*(str+12)) ) 
				{
					consumed += 11;
					return AGK_TOKEN_CASE_DEFAULT;
				}
				else
				{
					consumed += 3;
					return AGK_TOKEN_CASE;
				}
			}
			else break;
		}

		// string
		case '"':
		{
			do
			{
				str++;
				consumed++;
			}
			while( *str && *str != '"' );

			if ( !*str ) 
			{
				error.SetStr( "Invalid string, must all be on one line and end with a double quote" );
				return AGK_TOKEN_UNKNOWN;
			}
			else return AGK_TOKEN_STRING;
		}

		// escaped string
		case '\'':
		{
			do
			{
				str++;
				consumed++;
				while ( *str == '\\' )
				{
					if ( *(str+1) == '\\' || *(str+1) == '\'' || *(str+1) == 'n' || *(str+1) == 'r' || *(str+1) == 't' ) 
					{
						str += 2;
						consumed += 2;
					}
					else
					{
						error.Format( "Invalid escape character %c%c in string, use forward slashes for path names, or escape backslashes with a double backslash \\\\ ", *str, *(str+1) );
						return AGK_TOKEN_UNKNOWN;
					}
				}
			}
			while( *str && *str != '\'' );

			if ( !*str ) 
			{
				error.SetStr( "Invalid escaped string, must all be on one line and end with a single quote" );
				return AGK_TOKEN_UNKNOWN;
			}
			else return AGK_TOKEN_STRING;
		}
		
		// binary number
		case '%': 
		{
			if ( *(str+1) != '0' && *(str+1) != '1' )
			{
				error.SetStr( "Invalid binary number, must be all zeros and ones. If you are attempting remainder division use the mod command" );
				return AGK_TOKEN_UNKNOWN;
			}

			str++;

			while( *str == '0' || *str == '1' )
			{
				consumed++;
				str++;
			}

			if ( !IsSeparator(*str) )
			{
				error.SetStr( "Invalid binary number, must be all zeros and ones. If you are attempting remainder division use the mod command" );
				return AGK_TOKEN_UNKNOWN;
			}

			return AGK_TOKEN_INTEGER;
		}

		case '0':
		{
			if ( *(str+1) == 'x' )
			{
				str += 2;
				consumed++; // consume 'x'

				while( (*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'f') )
				{
					consumed++;
					str++;
				}

				if ( consumed == 2 || !IsSeparator(*str) )
				{
					error.SetStr( "Invalid hexidecimal number, must start 0x and only use the characters 0-9 and A-F" );
					return AGK_TOKEN_UNKNOWN;
				}

				return AGK_TOKEN_INTEGER;
			}
			break;
		}

		case 'r':
		{
			if ( strncmp( str, "remend", 6 ) == 0 && !IsIdentifierChar(*(str+6)) )
			{
				error.SetStr("End of comment block found without mathcing start");
				return AGK_TOKEN_UNKNOWN;
			}
			else break;
		}
	}

	consumed--;

	// try and parse an integer or float
	if ( IsNumber( *str ) || *str == '.' || *str == '-' )
	{
		if ( *str == '-' ) 
		{
			str++;
			consumed++;
		}

		while ( *str && IsNumber( *str ) )
		{
			str++;
			consumed++;
		}

		if ( *str != '.' )
		{
			if ( IsSeparator( *str ) ) return AGK_TOKEN_INTEGER;
			else 
			{
				char szTemp[ 50 ];
				int length = (int)(str-origStr);
				length++;
				if ( length < 50 ) 
				{
					strncpy( szTemp, origStr, length );
					szTemp[ length ] = 0;
					error.Format( "Invalid integer \"%s\"", szTemp );
				}
				else error.SetStr( "Invalid integer" );
				return AGK_TOKEN_UNKNOWN;
			}
		}
		else
		{
			str++;
			consumed++;
			if ( !IsNumber( *str ) ) 
			{
				char szTemp[ 50 ];
				int length = (int)(str-origStr);
				length++;
				if ( length < 50 ) 
				{
					strncpy( szTemp, origStr, length );
					szTemp[ length ] = 0;
					error.Format( "Invalid floating point number \"%s\"", szTemp );
				}
				else error.SetStr( "Invalid floating point number" );
				return AGK_TOKEN_UNKNOWN;
			}

			do
			{
				str++;
				consumed++;
			} while ( *str && IsNumber( *str ) );

			if ( *str != 'e' && *str != 'E' )
			{
				if ( IsSeparator( *str ) ) return AGK_TOKEN_FLOAT;
				else 
				{
					char szTemp[ 50 ];
					int length = (int)(str-origStr);
					length++;
					if ( length < 50 ) 
					{
						strncpy( szTemp, origStr, length );
						szTemp[ length ] = 0;
						error.Format( "Invalid floating point number \"%s\"", szTemp );
					}
					else error.SetStr( "Invalid floating point number" );
					return AGK_TOKEN_UNKNOWN;
				}
			}
			else
			{
				str++;
				consumed++;
				if ( *str == '+' || *str == '-' ) { str++; consumed++; }
				if ( !IsNumber( *str ) )
				{
					char szTemp[ 50 ];
					int length = (int)(str-origStr);
					length++;
					if ( length < 50 ) 
					{
						strncpy( szTemp, origStr, length );
						szTemp[ length ] = 0;
						error.Format( "Invalid floating point number \"%s\", E must be followed by a positive or negative number", szTemp );
					}
					else error.SetStr( "Invalid floating point number, E must be followed by a positive or negative number" );
					return AGK_TOKEN_UNKNOWN;
				}
				
				do
				{
					str++;
					consumed++;
				} while ( *str && IsNumber( *str ) );

				if ( IsSeparator( *str ) ) return AGK_TOKEN_FLOAT;
				else 
				{
					char szTemp[ 50 ];
					int length = (int)(str-origStr);
					length++;
					if ( length < 50 ) 
					{
						strncpy( szTemp, origStr, length );
						szTemp[ length ] = 0;
						error.Format( "Invalid floating point number \"%s\"", szTemp );
					}
					else error.SetStr( "Invalid floating point number" );
					return AGK_TOKEN_UNKNOWN;
				}
			}
		}

		// all paths to here should have returned
	}

	const char *orig = str;
	uString keyword;

	// try to parse identifiers, labels, and keywords
	if ( !IsIdentifierCharFirst( *str ) )
	{
		error.Format( "Invalid character '%c'", *str );
		return AGK_TOKEN_UNKNOWN;
	}
	else
	{
		do
		{
			consumed++; 
			str++;
		}
		while( *str && IsIdentifierChar( *str ) );

		if ( IsIdentifierCharLast( *str ) ) { consumed++; str++; }

		// check for keywords
		keyword.AppendN( orig, consumed );
		AGKToken keywordtoken = ParseKeyword( keyword.GetStr() );
		if ( keywordtoken != AGK_TOKEN_UNKNOWN )
		{
			if ( !IsSeparator( *(str) ) ) 
			{
				error.Format( "Invalid character '%c' after keyword", *str );
				return AGK_TOKEN_UNKNOWN;
			}
			else return keywordtoken;
		}

		// must be an identifier or label
		if ( prevToken == AGK_TOKEN_UNKNOWN || prevToken == AGK_TOKEN_EOL )
		{
			if ( *str == ':' )
			{
				if ( IsSeparator( *(str+1) ) )
				{
					consumed++;
					return AGK_TOKEN_LABEL;
				}
				else 
				{
					error.Format( "Invalid character '%c' after label", *str );
					return AGK_TOKEN_UNKNOWN;
				}
			}
		}

		// must be an identifier
		if ( !IsSeparator( *(str) ) ) 
		{
			error.Format( "Invalid character '%c' after identifier", *str );
			return AGK_TOKEN_UNKNOWN;
		}

		return AGK_TOKEN_IDENTIFIER;
	}

	uString temp( str );
	uString temp2;
	int index = temp.Find( ' ' );
	if ( index > 0 ) temp.SubString( temp2, 0, index );
	else temp2.SetStr( temp );
	error.Format( "Unrecognised token \"%s\"", temp2.GetStr() );
	return AGK_TOKEN_UNKNOWN;
}

int Parser1::ParseLine( int includeFileID, const uString &sLine, const uString &sLineRaw, int iLineNum, uString &error, cProgramData *pProgram )
{
	const char *str = sLine.GetStr();
	uint32_t offset = 0;
	// skip white space
	while ( str[offset] == ' ' || str[offset] == '\t' ) offset++;

	int consumed = 0;
	AGKToken prevprevToken = AGK_TOKEN_UNKNOWN;
	AGKToken prevToken = AGK_TOKEN_UNKNOWN;

	if ( str[offset] == 0 ) return 1;

	char szBuffer[128];
	char szBuffer2[128];

	do
	{
		AGKToken token = AGK_TOKEN_UNKNOWN;
		if ( iCommentBlock ) token = ParseTokenCommentBlock( prevToken, str+offset, consumed, error );
		else token = ParseToken( prevprevToken, prevToken, str+offset, consumed, error );
		
		if ( token == AGK_TOKEN_UNKNOWN )
		{
			// error
			return 0;
		}
		else
		{
			if ( token == AGK_TOKEN_COMMENT ) 
			{
				pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", iLineNum, includeFileID, offset, offset);
				return 1;
			}
			if ( token == AGK_TOKEN_REMSTART ) iCommentBlock = 1;
			if ( token == AGK_TOKEN_DEC_FUNCTION )
			{
				if ( iFunctionBlock == 0 ) iFunctionBlock = 1;
				else
				{
					error.SetStr( "Cannot declare a function inside a function" );
					return 0;		
				}
				if ( iTypeBlock > 0 ) 
				{
					error.SetStr( "Cannot declare a function inside a type" );
					return 0;	
				}
			}
			if ( token == AGK_TOKEN_GOTO )
			{
				// allow it, but check later if the goto tries to leave the function
				/*
				if ( iFunctionBlock > 0 ) 
				{
					error.SetStr( "Cannot use Goto inside a function" );
					return 0;
				}
				*/

				if ( iSelectBlock > 0 ) 
				{
					error.SetStr( "Cannot use Goto inside a Select statement" );
					return 0;
				}
			}
			if ( token == AGK_TOKEN_LABEL )
			{
				/*
				if ( iFunctionBlock > 0 ) 
				{
					error.SetStr( "Cannot define a label inside a function" );
					return 0;
				}
				*/

				if ( iSelectBlock > 0 ) 
				{
					error.SetStr( "Cannot declare a label inside a Select statement" );
					return 0;
				}
			}
			if ( token == AGK_TOKEN_DEC_TYPE )
			{
				if ( iTypeBlock == 0 ) iTypeBlock = 1;
				else
				{
					error.SetStr( "Cannot declare a type inside another type" );
					return 0;		
				}
				if ( iFunctionBlock > 0 ) 
				{
					error.SetStr( "Cannot declare a type inside a function" );
					return 0;	
				}
			}
			if ( token == AGK_TOKEN_SELECT )
			{
				iSelectBlock++;
			}
				
			int tokenID = 0;
			int ignore = 0;
			if ( token == AGK_TOKEN_COMP_DIRECTIVE && consumed < 120 )
			{
				ignore = 1;
				strncpy( szBuffer, str+offset, consumed );
				szBuffer[ consumed ] = 0;
				if ( strcmp( szBuffer, "#option_explicit" ) == 0 )
				{
					pProgram->m_iOptionExplicit = 1;
				}
			}

			if ( token == AGK_TOKEN_EXPORT ) 
			{
				// treat as a comment
				pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", iLineNum, includeFileID, offset, offset);
				return 1;
			}

			// add token to token list
			if ( pProgram && iCommentBlock == 0 && ignore == 0 )
			{
				char* szRaw = szBuffer;
				char* szRaw2 = szBuffer2;
				if ( consumed > 127 ) 
				{
					szRaw = new char[ consumed+1 ];
					szRaw2 = new char[ consumed+1 ];
				}

				const char* str2 = sLineRaw.GetStr() + offset;

				if ( token == AGK_TOKEN_STRING )
				{
					strncpy( szRaw, str2, consumed-1 );
					szRaw[ consumed-1 ] = 0;

					szRaw2[ 0 ] = 0; // don't need another copy of the string, case is not modified
				}
				else if ( token == AGK_TOKEN_IDENTIFIER && (prevToken == AGK_TOKEN_IMPORT_PLUGIN 
															|| prevToken == AGK_TOKEN_IMPORT_MODULE) )
				{
					strncpy( szRaw, str2, consumed );
					szRaw[ consumed ] = 0;

					strncpy( szRaw2, str2, consumed );
					szRaw2[ consumed ] = 0;
				}
				else
				{
					// lower case
					strncpy( szRaw, str+offset, consumed );
					szRaw[ consumed ] = 0;

					// original case
					strncpy( szRaw2, str2, consumed );
					szRaw2[ consumed ] = 0;
				}

				tokenID = pProgram->AddToken( token, szRaw, szRaw2, iLineNum, includeFileID, offset, offset+consumed );

				if ( szRaw2 != szBuffer2 ) delete [] szRaw2;
				if ( szRaw != szBuffer ) delete [] szRaw;

				// check for labels
				if ( token == AGK_TOKEN_LABEL )
				{
					if ( includeFileID & 0x02000000 )
					{
						error.SetStr( "Inserted files cannot contain any labels or gosubs, use include files instead" );
						return 0;
					}

					int index = pProgram->AddLabel( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), tokenID );
					if ( index < 0 ) 
					{
						// do find after attempting to add as this case should be a rare event, and doing the find before the add would do two lookups
						int prevIndex = pProgram->FindLabel( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
						int tokenID2 = pProgram->m_pLabels.m_pData[ prevIndex ]->iTokenID;
						const char* szInclude = "Unknown File";
						int includeID = pProgram->GetToken( tokenID2 )->iIncludeFile;
						if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) szInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sName.GetStr();
						int lineNum = pProgram->GetToken( tokenID2 )->iLine;

						error.Format( "Label \"%s\" has already been defined on line %d in %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), lineNum, szInclude );
						return 0;
					}
				}

				// check for render mode
				if ( prevToken == AGK_TOKEN_RENDERER && token == AGK_TOKEN_STRING )
				{
					if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "Prefer Best" ) == 0 ) pProgram->m_iRendererMode = 0; // AGK_RENDERER_MODE_PREFER_BEST
					else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "Advanced" ) == 0 ) pProgram->m_iRendererMode = 1; // AGK_RENDERER_MODE_ADVANCED
					else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "Basic" ) == 0 ) pProgram->m_iRendererMode = 2; // AGK_RENDERER_MODE_LOWEST
					else if ( pProgram->GetToken( tokenID )->sValueRaw.CompareTo( "None" ) == 0 ) pProgram->m_iRendererMode = 3; // AGK_RENDERER_MODE_NONE
					else
					{
						error.SetStr( "Unrecognised renderer, only \"Prefer Best\", \"Advanced\", \"Basic\", and \"None\" are supported" );
						return 0;
					}
				}

				// check for include files
				if ( prevToken == AGK_TOKEN_INCLUDE && token == AGK_TOKEN_STRING )
				{
					if ( includeMode == INCLUDE_FORBIDDEN )
					{
						error.SetStr( "Include files are not supported in this version of the compiler" );
						return 0;
					}
					else if ( includeMode != INCLUDE_IGNORE )
					{
						if ( includeFileID & 0x02000000 )
						{
							error.SetStr( "Inserted files cannot include other files" );
							return 0;
						}

						int index = pProgram->AddIncludeFile( pProgram->GetToken( tokenID )->sValueRaw.GetStr(), 1, &error );
						// included files are appended to the end of the program
					}
				}

				// check for inserted files
				if ( prevToken == AGK_TOKEN_INSERT && token == AGK_TOKEN_STRING )
				{
					if ( includeMode == INCLUDE_FORBIDDEN )
					{
						error.SetStr( "#insert is not supported in this version of the compiler" );
						return 0;
					}
					else if ( includeMode != INCLUDE_IGNORE )
					{
						if ( includeFileID & 0x02000000 )
						{
							error.SetStr( "Inserted files cannot insert other files" );
							return 0;
						}

						int index = pProgram->AddIncludeFile( pProgram->GetToken( tokenID )->sValueRaw.GetStr(), 2, &error );
						if ( (index & 0xFFFFFF) == (includeFileID & 0xFFFFFF) ) 
						{
							error.SetStr( "Inserted file cannot insert itself" );
							return 0;
						}

						index |= 0x02000000; // second bit marks it as inserted instead of included
					
						// add tokens from inserted file here
						Parser1 subParser;
						int result = subParser.ParseFile( pProgram->GetToken( tokenID )->sValueRaw.GetStr(), index, pProgram );
						if ( result == 0 ) 
						{
							error.SetStr( "Error in inserted file" );
							return 0;
						}
					}
				}

				// check for functions
				if ( prevToken == AGK_TOKEN_DEC_FUNCTION )
				{
					if( token == AGK_TOKEN_IDENTIFIER )
					{
						if ( pProgram->FindCommandName( pProgram->GetToken( tokenID )->sValueRaw.GetStr() ) >= 0 )
						{
							error.Format( "Function \"%s\" is already a built in AppGameKit function", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
							return 0;
						}

						iFunctionLine = iLineNum;
						int index = pProgram->AddFunction( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), tokenID-1 );
						if ( index < 0 ) 
						{
							// do find after attempting to add as this case should be a rare event, and doing the find before the add would do two lookups
							int prevIndex = pProgram->FindFunction( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
							int tokenID2 = pProgram->m_pFunctions.m_pData[ prevIndex ]->iTokenID;
							const char* szInclude = "Unknown File";
							int includeID = pProgram->GetToken( tokenID2 )->iIncludeFile;
							if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) szInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sName.GetStr();
							int lineNum = pProgram->GetToken( tokenID2 )->iLine;

							error.Format( "Function \"%s\" has already been defined on line %d in %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), lineNum, szInclude );

							return 0;
						}
					}
					else
					{
						error.SetStr( "Function declaration must have a name" );
						return 0;
					}
				}

				// check for types
				if ( prevToken == AGK_TOKEN_DEC_TYPE )
				{
					if ( token == AGK_TOKEN_IDENTIFIER )
					{
						if ( pProgram->FindCommandName( pProgram->GetToken( tokenID )->sValueRaw.GetStr() ) >= 0 )
						{
							error.Format( "\"%s\" is a reserved word", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr() );
							return 0;
						}

						iTypeLine = iLineNum;
						int index = pProgram->AddType( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), tokenID-1 );
						if ( index < 0 ) 
						{
							// do find after attempting to add as this case should be a rare event, and doing the find before the add would do two lookups
							int prevIndex = pProgram->FindType( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
							int tokenID2 = pProgram->m_pTypes.m_pData[ prevIndex ]->iTokenID;
							const char* szInclude = "Unknown File";
							int includeID = pProgram->GetToken( tokenID2 )->iIncludeFile;
							if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) szInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sName.GetStr();
							int lineNum = pProgram->GetToken( tokenID2 )->iLine;

							error.Format( "Type \"%s\" has already been defined on line %d in %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), lineNum, szInclude );
							return 0;
						}
					}
					else
					{
						error.SetStr( "Type declaration must have a name" );
						return 0;
					}
				}

				// check for constants
				if ( prevToken == AGK_TOKEN_CONSTANT && token == AGK_TOKEN_IDENTIFIER )
				{
					int index = pProgram->AddConstant( pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), tokenID-1 );
					if ( index < 0 ) 
					{
						// do find after attempting to add as this case should be a rare event, and doing the find before the add would do two lookups
						int prevIndex = pProgram->FindConstant( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
						int tokenID2 = pProgram->m_pConstants.m_pData[ prevIndex ]->tokenID;
						const char* szInclude = "Unknown File";
						int includeID = pProgram->GetToken( tokenID2 )->iIncludeFile;
						if ( includeID < pProgram->m_pIncludeFiles.m_iCount ) szInclude = pProgram->m_pIncludeFiles.m_pData[ includeID ]->sName.GetStr();
						int lineNum = pProgram->GetToken( tokenID2 )->iLine;

						error.Format( "Constant \"%s\" has already been defined on line %d in %s", pProgram->GetToken( tokenID )->sValueRawOrig.GetStr(), lineNum, szInclude );
						return 0;
					}
				}

				// check for company name
				if ( prevToken == AGK_TOKEN_COMPANY_NAME && token == AGK_TOKEN_STRING )
				{
					pProgram->m_sCompanyName.SetStr( pProgram->GetToken( tokenID )->sValueRaw.GetStr() );
				}
			}

			if ( token == AGK_TOKEN_DEC_FUNCTION_END ) 
			{
				iFunctionLine = 0;
				iFunctionBlock = 0;
			}

			if ( token == AGK_TOKEN_DEC_TYPE_END ) 
			{
				iTypeLine = 0;
				iTypeBlock = 0;
			}

			if ( token == AGK_TOKEN_SELECT_END )
			{
				iSelectBlock--;
			}

			if ( token == AGK_TOKEN_REMEND ) iCommentBlock = 0;

			offset += consumed;
			//char outputStr[1024];
			//sprintf(outputStr,"Line: %d, Token: %s\n", iLineNum, GetTokenString(token) );
			//OutputDebugString(outputStr);
		}

		prevprevToken = prevToken;
		prevToken = token;

		while ( str[offset] == ' ' || str[offset] == '\t' ) offset++;
	} 
	while ( str[offset] );

	return 1;
}

int Parser1::ParseFile( const char* filename, int includeFileID, cProgramData *pProgram )
{
	if ( !filename ) 
	{
		Error( "Invalid file name for parsing", 0, 0 );
		return 0;
	}

	uString sFilename = filename;
	sFilename.Replace( '\\', '/' );

	cFile inputFile;
	inputFile.OpenToRead( sFilename );
	int iLineCount = 0;

	uString sError;
	uString sLine;
	uString sLineRaw;
	while( !inputFile.IsEOF() )
	{
		int length = inputFile.ReadLineFast( sLineRaw );
		
		//char outputStr[1024];
		//sprintf(outputStr,"File: %s, Line: %s", filename, sLine.GetStr());
		//OutputDebugString(outputStr);

		
		sLine.SetStr( sLineRaw );
		sLine.Lower();

		if ( !ParseLine( includeFileID, sLine, sLineRaw, ++iLineCount, sError, pProgram ) ) 
		{
			Error( sError.GetStr(), iLineCount, sFilename );
			//return 0; // find all errors
		}
		
	}

	if ( iCommentBlock > 0 )
	{
		Error( "Comment block is not closed", iLineCount, sFilename );
		return 0;
	}

	if ( iFunctionBlock > 0 )
	{
		Error( "Function definition has no EndFunction", iFunctionLine, sFilename );
		return 0;
	}

	if ( iTypeBlock > 0 )
	{
		Error( "Type definition has no EndType", iTypeLine, sFilename );
		return 0;
	}

	// add some line endings for token parsing that looks ahead so they don't go off the end
	pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", iLineCount, includeFileID, 0, 0 );
	pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", iLineCount+1, includeFileID, 0, 0 );
	pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", iLineCount+2, includeFileID, 0, 0 );

	pProgram->iLineCountTotal += iLineCount;
	return iLineCount;
}

int Parser1::ParseSource( const char* source, cProgramData *pProgram )
{
	if ( !source ) 
	{
		Error( "Invalid file name for parsing", 0, 0 );
		return 0;
	}
	
	int iLineCount = 0;
	uString sError;
	uString sLine;
	uString sLineRaw;
	while( *source )
	{
		const char* start = source;
		while( *source && *source != '\n' ) source++;
		int length = (unsigned int)(source - start) + 1;
		sLineRaw.SetStrN( start, length );
		if ( *source ) source++;
				
		sLine.SetStr( sLineRaw );
		sLine.Lower();

		if ( !ParseLine( 0, sLine, sLineRaw, ++iLineCount, sError, pProgram ) ) 
		{
			Error( sError.GetStr(), iLineCount, "main.agc" );
			// don't return, find all errors
		}
	}

	if ( iCommentBlock > 0 )
	{
		Error( "Comment block is not closed", iLineCount, "main.agc" );
		return 0;
	}

	if ( iFunctionBlock > 0 )
	{
		Error( "Function definition has no EndFunction", iFunctionLine, "main.agc" );
		return 0;
	}

	if ( iTypeBlock > 0 )
	{
		Error( "Type definition has no EndType", iTypeLine, "main.agc" );
		return 0;
	}

	// add some line endings for token parsing that looks ahead so they don't go off the end
	pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", iLineCount, 0, 0, 0 );
	pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", iLineCount+1, 0, 0, 0 );
	pProgram->AddToken( AGK_TOKEN_EOL, "\n", "\\n", iLineCount+2, 0, 0, 0 );

	pProgram->iLineCountTotal += iLineCount;
	return iLineCount;
}

int Parser1::HasUnclosedBlocks()
{
	if ( iCommentBlock > 0 || iFunctionBlock > 0 || iTypeBlock > 0 ) return 1;
	else return 0;
}