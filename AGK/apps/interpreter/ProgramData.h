#ifndef _H_PROGRAM_DATA
#define _H_PROGRAM_DATA

#include "agk.h"

// Assumption: Array types are base types + 10
#define AGK_VARIABLE_INTEGER		1
#define AGK_VARIABLE_FLOAT			2
#define AGK_VARIABLE_STRING			3
#define AGK_VARIABLE_TYPE			4
#define AGK_VARIABLE_ARRAY_0		10
#define AGK_VARIABLE_ARRAY_INT		11
#define AGK_VARIABLE_ARRAY_FLOAT	12
#define AGK_VARIABLE_ARRAY_STRING	13
#define AGK_VARIABLE_ARRAY_TYPE		14

#define AGK_DATA_TYPE_INT 1
#define AGK_DATA_TYPE_FLOAT 2
#define AGK_DATA_TYPE_STRING 3
#define AGK_DATA_TYPE_TYPE 4
#define AGK_DATA_TYPE_ARRAY 5

#define AGK_LOCAL_VAR_OFFSET 3

#define AGK_INSTRUCTION_BREAKPOINT		0x01
#define AGK_INSTRUCTION_CONTINUE		0x02

#define AGK_JSON_INDENT "    "

#include "AGKInstructions.h"

extern "C" void* win64_plugin_call( int rCount, void* rParams, int* rTypes, int sCount, void* sParams, int retType, void* func );

void GeneralError( const char *msg );

struct stInstruction
{
	AGKInstruction iInstruction;
	int iLineNum;
	union
	{
		int i;
		float f;
	};
	union
	{
		int i2;
		float f2;
	};
	int iIncludeFile;
	int iFlags;
};

struct stTypeDecVar
{
	uString varName;
	int *varLength; // arrays only
	unsigned int varTypeIndex; // the type struct ID if applicable
	unsigned short numDimensions; // arrays only
	unsigned short varOrder; // order the vars were defined by the user
	unsigned short varOffset; // byte offset into type data
	unsigned char varType; // 1=int, 2=float, 3=string, 4=type, 5=array
	unsigned char varSubType; // if varType is array then varSubType: 1=int, 2=float, 3=string, 4=type
	
	stTypeDecVar() { numDimensions = 0; varLength = 0; varTypeIndex = 0; varType = 0; varSubType = 0; varOffset = 0; }
};

struct stTypeDec
{
	UINT m_iNumVars;
	UINT m_iDataSize; // cannot be determined until the bytecode is run (32bit or 64bit affects this)
	int m_iFirstIndex; // index of the first variable defined by the user, used for sorting arrays, variables have been shuffled for implementation reasons
	stTypeDecVar *m_pVarTypes; 
};

struct stType
{
	UINT m_iVarSize; // cannot be determined until the bytecode is run (32bit or 64bit affects this)
	UINT m_iTypeDec; // references the type declaration structure
	char *m_pData; // for proper alignment the format of this must be pointers, then ints and floats.
	int m_iTemp; // temporary type created for functions returning types, must be deleted when popped.
	int reseverd;

	// struct must be a multiple of 8 bytes (with the pointer on an 8 byte boundary) if we want to support 64 bit interpreters

	stType() { m_iVarSize = 0; m_iTypeDec = 0; m_pData = 0; m_iTemp = 0; reseverd = 0; }
};

struct stArray
{
	UINT m_iLength;
	UINT m_iDataType; // combination of current type, type type, dimensions, and end type. current type is 1=int, 2=float, 3=string, 4=type, 5=array
	UINT m_iTotalLength; // might be bigger than the number of elements in m_iLength
	UINT m_iTemp; // temporary array created for functions returning arrays, must be deleted when popped.
	union
	{
		int *m_pI;
		float *m_pF;
		uString **m_pS; // must be array of pointers in case the array is resized which would call the string destructor
		stType **m_pT; // must be array of pointers in case the array is resized whilst a type is on the stack
		stArray **m_pA; // must be array of pointers in case the array is resized whilst an array is on the stack
	};
	
	// struct must be a multiple of 8 bytes (with the pointer on an 8 byte boundary) if we want to support 64 bit interpreters

	stArray() { m_iLength = 0; m_iDataType = 0; m_pI = 0; m_iTotalLength = 0; m_iTemp = 0; m_pI = 0; }

	int GetCurrType() { return m_iDataType & 0x07; }
	int GetDimensions() { return (m_iDataType >> 3) & 0x07; }
	int GetEndType() { return (m_iDataType >> 6) & 0x07; }
	int GetTypeType() { return m_iDataType >> 9; }
	void SetDataType( UINT currType, UINT typetype, UINT dimensions, UINT endtype ) { m_iDataType = (typetype << 9) | ((endtype & 0x07) << 6) | ((dimensions & 0x07) << 3) | (currType & 0x07); }

	int IndexOfInt (int offset, int value )
	{
		if ( m_iLength == 0 ) return -1;

		int varType = GetCurrType ( );
		if ( varType != AGK_DATA_TYPE_INT && varType != AGK_DATA_TYPE_TYPE )
		{
			return -1;
		}

		for ( unsigned int i = 0; i < m_iLength; i++ )
		{
			if ( varType == AGK_DATA_TYPE_TYPE )
			{
				if ( ( *( int* ) ( m_pT [ i ]->m_pData+offset ) ) == value )
					return i;
			}
			else
			{
				if ( m_pI [ i ] == value )
					return i;
			}
		}

		return -1;
	}

	int FindInt( int offset, int value, int closest=0 )
	{
		if ( m_iLength == 0 ) return closest ? 0 : -1;

		int varType = GetCurrType();
		if ( varType != AGK_DATA_TYPE_INT && varType != AGK_DATA_TYPE_TYPE ) 
		{
			return -1;
		}

		int high = m_iLength-1;
		int low = 0;
		int found = -1;
		int mid = 0;
		int result = 0;

		if ( varType == AGK_DATA_TYPE_TYPE )
		{
			while( high >= low )
			{
				mid = (high+low)/2;
				result = *((int*)(m_pT[mid]->m_pData+offset)) - value;
				if ( result > 0 ) high = mid-1;
				else if ( result < 0 ) low = mid+1;
				else 
				{
					found = mid;
					break;
				}
			}
		}
		else
		{
			while( high >= low )
			{
				mid = (high+low)/2;
				result = m_pI[mid] - value;
				if ( result > 0 ) high = mid-1;
				else if ( result < 0 ) low = mid+1;
				else 
				{
					found = mid;
					break;
				}
			}
		}

		if ( !closest ) return found;
		else
		{
			if ( found >= 0 ) return found;
			else
			{
				if ( result < 0 ) mid++;
				return mid;
			}
		}
	}

	int IndexOfFloat (int offset, float value )
	{
		if ( m_iLength == 0 ) return -1;

		int varType = GetCurrType ( );
		if ( varType != AGK_DATA_TYPE_FLOAT && varType != AGK_DATA_TYPE_TYPE )
		{
			return -1;
		}

		// mike - 060921 - the code beneath may be more efficient, however, it does not work correctly
		// dependent on how the data is ordered, therefore for the time being I've changed it
		for ( unsigned int i = 0; i < m_iLength; i++ )
		{
			if ( varType == AGK_DATA_TYPE_TYPE )
			{
				if ( ( *( float* ) ( m_pT [ i ]->m_pData+offset ) ) == value )
					return i;
			}
			else
			{
				if ( m_pF [ i ] == value )
					return i;
			}
		}

		return -1;
	}

	int FindFloat( int offset, float value, int closest=0 )
	{
		if ( m_iLength == 0 ) return closest ? 0 : -1;

		int varType = GetCurrType();
		if ( varType != AGK_DATA_TYPE_FLOAT && varType != AGK_DATA_TYPE_TYPE ) 
		{
			return -1;
		}

		int high = m_iLength-1;
		int low = 0;
		int found = -1;
		int mid = 0;
		float result = 0;

		if ( varType == AGK_DATA_TYPE_TYPE )
		{
			while( high >= low )
			{
				mid = (high+low)/2;
				result = *((float*)(m_pT[mid]->m_pData+offset)) - value;
				if( result > 0 ) high = mid-1;
				else if ( result < 0 ) low = mid+1;
				else 
				{
					found = mid;
					break;
				}
			}
		}
		else
		{
			while( high >= low )
			{
				mid = (high+low)/2;
				result = m_pF[mid] - value;
				if ( result > 0 ) high = mid-1;
				else if ( result < 0 ) low = mid+1;
				else 
				{
					found = mid;
					break;
				}
			}
		}

		if ( !closest ) return found;
		else
		{
			if ( found >= 0 ) return found;
			else
			{
				if ( result < 0 ) mid++;
				return mid;
			}
		}
	}

	int IndexOfString (int offset, const char* value )
	{
		if ( m_iLength == 0 ) return -1;

		int varType = GetCurrType ( );
		if ( varType != AGK_DATA_TYPE_STRING && varType != AGK_DATA_TYPE_TYPE )
		{
			return -1;
		}

		// mike - 060921 - the code beneath may be more efficient, however, it does not work correctly
		// dependent on how the data is ordered, therefore for the time being I've changed it
		for ( unsigned int i = 0; i < m_iLength; i++ )
		{
			if ( varType == AGK_DATA_TYPE_TYPE )
			{
				uString *pString = ( uString* ) ( m_pT [ i ]->m_pData+offset );

				if ( pString->CompareTo ( value ) == 0 )
					return i;
			}
			else
			{

				if ( m_pS [ i ]->CompareTo ( value ) == 0 )
					return i;
			}
		}

		return -1;
	}

	int FindString( int offset, const char* value, int closest=0 )
	{
		if ( m_iLength == 0 ) return closest ? 0 : -1;

		int varType = GetCurrType();
		if ( varType != AGK_DATA_TYPE_STRING && varType != AGK_DATA_TYPE_TYPE ) 
		{
			return -1;
		}

		int high = m_iLength-1;
		int low = 0;
		int found = -1;
		int mid = 0;
		int result = 0;

		if ( varType == AGK_DATA_TYPE_TYPE )
		{
			while( high >= low )
			{
				mid = (high+low)/2;
				uString *pString = (uString*)(m_pT[mid]->m_pData+offset);
				result = pString->CompareTo( value );
				if( result > 0 ) high = mid-1;
				else if ( result < 0 ) low = mid+1;
				else 
				{
					found = mid;
					break;
				}
			}
		}
		else
		{
			while( high >= low )
			{
				mid = (high+low)/2;
				result = m_pS[mid]->CompareTo( value );
				if ( result > 0 ) high = mid-1;
				else if ( result < 0 ) low = mid+1;
				else 
				{
					found = mid;
					break;
				}
			}
		}

		if ( !closest ) return found;
		else
		{
			if ( found >= 0 ) return found;
			else
			{
				if ( result < 0 ) mid++;
				return mid;
			}
		}
	
	}
};

struct sortTypeString
{
	uString *pString;
	stType *pType;
};

union unStackItem
{
	int i;
	float f;
	void* p;
};

class Breakpoint
{
	public:
		int m_iIncludeFile;
		int m_iLine;
		int m_iInstruction;
		Breakpoint *m_pNext;

		Breakpoint() { m_pNext = 0; m_iLine = 0; m_iIncludeFile = 0; m_iInstruction = 0; }
		~Breakpoint() {}
};

class AGKVariableWatch
{
	public:
		uString sExpression;
		AGKVariableWatch *m_pNext;

		AGKVariableWatch() { m_pNext = 0; }
		~AGKVariableWatch() {}
};

class AGKFunctionVar
{
	public:
		uString sName;
		int iVarID;
		unsigned short iTypeIndex;
		unsigned char iType;
		unsigned char iNumDimensions;

		AGKFunctionVar() { iVarID = 0; iTypeIndex = 0; iType = 0; iNumDimensions = 0; }
		~AGKFunctionVar() {}
};

class AGKFunction
{
	public:
		uString sName;
		int iInstructionPtr;
		int iNumVars;
		AGKFunctionVar *pVars;

		AGKFunction() { iInstructionPtr = 0; iNumVars = 0; pVars = 0; }
		~AGKFunction() { if ( pVars ) delete [] pVars; }
};

struct stPluginCommand
{
	void(*pFunc)(void);

	unsigned char iNumParams;
	unsigned char iParamsTypes[ 32 ]; // max 32 parameters
	unsigned char iReturnType;
};

struct stPlugin
{
	char* szName;
	int m_iNumCommands;
	stPluginCommand *m_pCommands;
	void *m_hModule;

	stPlugin() { szName = 0; m_iNumCommands = 0; m_pCommands = 0; m_hModule = 0; }
	~stPlugin() { if ( m_pCommands ) delete [] m_pCommands; if ( szName ) delete [] szName; }
};

class ProgramData
{
	protected:
		int m_iBytecodeVersion;
		int m_iBytecodeDebug;
		int m_iGosubCounter;
		int m_iFunctionLevel;
		int m_iDebugFrameDepth;
		int m_iLastBreakLine;
		int m_iLastBreakLevel;

		// bytecode header values
		uString m_sCompanyName;
		AGKRendererMode m_iRendererMode;
		uint32_t m_iFlags;

		Breakpoint *m_pBreakpoints;
		AGKVariableWatch *m_pWatchVaraibles;

		UINT m_pPrevInstructions[ 100 ];
		UINT m_iPrevInstructionPointer;

		UINT m_iProgramCounter;
		UINT m_iNumInstructions;
		stInstruction *m_pInstructions;

		UINT m_iNumPlugins;
		stPlugin *m_pPlugins;

		UINT m_iNumIncludeFiles;
		char **m_pIncludeFiles;

		UINT m_iNumStrings;
		char** m_pStringTable;

		UINT m_iNumTypeStructs;
		stTypeDec *m_pTypeStructs;

		UINT m_iNumLabels;
		UINT *m_pLabels;

		// global variables
		UINT m_iNumVariablesInt;
		UINT m_iNumVariablesFloat;
		UINT m_iNumVariablesString;
		UINT m_iNumVariablesType;
		UINT m_iNumVariablesArray;

		int *m_pVariablesInt;
		float *m_pVariablesFloat;
		uString *m_pVariablesString; 
		stType *m_pVariablesType; 
		stArray *m_pVariablesArray; 

		uString *m_pVariablesIntNames;
		uString *m_pVariablesFloatNames;
		uString *m_pVariablesStringNames; 
		uString *m_pVariablesTypeNames; 
		uString *m_pVariablesArrayNames;

		UINT m_iStackPtr;
		int m_iFramePtr;
		unStackItem m_pStack[ 32768 ];
		UINT m_iMaxStack; // debug value to see how big the stack gets

		// separate stack for strings as each string struct is 12 bytes in size (or 16 bytes on 64 bit platform)
		UINT m_iStrStackPtr;
		int m_iStrFramePtr;
		uString m_pStrStack[ 1024 ];

		// function details for debugging
		UINT m_iNumFunctions;
		AGKFunction *m_pFunctions;

		int m_iInstructionCounter;

		ProgramData *m_pSubProgram;
		uString m_sOldWritePath;

		static int CompareOrderVars( const void *a, const void *b );

		void AllocType( stType *pType, UINT typeStruct );
		void ZeroType( stType *pType );
		void FreeType( stType *pType );
		void CopyType( stType *pFromType, stType *pToType );

		void AllocArray( stArray *pArray, UINT type, UINT subType, UINT numDimensions );
		void SizeArray( stArray *pArray, UINT length );
		void ZeroArray( stArray *pArray );
		void FreeArray( stArray *pArray );
		void CopyArray( stArray *pFromArray, stArray *pToArray );
		void BlockResizeArray( stArray *pArray, UINT numDimensions, UINT dataType, UINT typeType );
		void BlockResizeArray( stArray *pArray, UINT numDimensions, int *pLengths, UINT dataType, UINT typeType );

		void ArrayToJSON( stArray *pArray, uString &output, int level=0 );
		void TypeToJSON( stType *pType, uString &output, int level=0 );

		void ArrayFromJSON( stArray *pArray, JSONElement *json );
		void TypeFromJSON( stType *pType, JSONElement *json );

		union converter
		{
			int i;
			float f;
			void* p;
		};

		int ParseDebugVariable( AGKFunction *pFunction, int stackPtr, int strStackPtr, const char *szVar, uString &sValue, int &consumed ); 
		int ParseDebugType( AGKFunction *pFunction, int stackPtr, int strStackPtr, stType *pType, const char *szField, uString &sValue, int &consumed );
		int ParseDebugArray( AGKFunction *pFunction, int stackPtr, int strStackPtr, stArray *pArray, const char *szDimensions, uString &sValue, int &consumed );

		int SetDebugVariable( AGKFunction *pFunction, int stackPtr, int strStackPtr, const char *szVar, uString &sNewValue, int &consumed ); 
		int SetDebugType( AGKFunction *pFunction, int stackPtr, int strStackPtr, stType *pType, const char *szField, uString &sNewValue, int &consumed );
		int SetDebugArray( AGKFunction *pFunction, int stackPtr, int strStackPtr, stArray *pArray, const char *szDimensions, uString &sNewValue, int &consumed );

	public:
		ProgramData();
		~ProgramData();

		int GetDebugMode() { return m_iBytecodeDebug; }

		const char* GetCurrentIncludeFile();
		int GetCurrentLineNum();
		const char* GetCompanyName() { return m_sCompanyName.GetStr(); }
		AGKRendererMode GetRendererMode() { return m_iRendererMode; }
		uint32_t GetFlags() { return m_iFlags; }

		static int CompareString( const void* a, const void* b );
		static int CompareTypeString( const void* a, const void* b );
		static int CompareStringCase( const void* a, const void* b );
		static int CompareTypeStringCase( const void* a, const void* b );
		
		void RuntimeError( const char *msg );
		
		int LoadBytecodeHeader( const char* filename );
		int LoadBytecode( const char* filename );
		void CleanUp();
		int ShouldEnd();

		void Break();
		int RunProgram();
		int RunProgramDebug( int stepMode );
		void PrintDebug();

		void AddBreakpoint( const char *szFile, int line );
		void RemoveBreakpoint( const char *szFile, int line );

		AGKVariableWatch* AddVariableWatch( const char *szVar );
		void RemoveVariableWatch( const char *szVar );
		void SetVariable( const char *szVar, const char *szValue );

		void PrintWatchVariables( AGKVariableWatch *pTargetVar=0 );
		void PrintCallStack();
		void SetCallStackFrame( int iFrame );
};

#endif