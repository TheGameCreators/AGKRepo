#ifndef _H_PARSER2
#define _H_PARSER2

#include "ProgramData.h"

namespace AGK_Compiler
{
	class Parser2
	{
		protected:
			static const char* operatorName( int op );
			static int operatorPrecedence( int op );
			static int operatorLeftAssoc( int op );
			static int convertBinaryOperator( const char* szOp );
			static int convertUnaryOperator( const char* szOp );	
			static int operatorDataType( int op, int dataType );
			static AGKInstruction convertToInstruction( int op, int dataType );

			union converter
			{
				int i;
				float f;
			};

			int m_iReplacedConstants = 0;
			int m_iSkipInstructions = 0;

		public:

			Parser2();
			~Parser2();

			void SetSkipInstructions( int skip );

			int ParseDimArray( stFunction *pFunction, cProgramData *pProgram, stVariable *pVar, DynArrayObject<stInstruction> *pInstructions, int tokenID, int &consumed );
			int ParseArrayDefaultValue( int type, int length, stVariable *pVar, cProgramData *pProgram, DynArrayObject<stInstruction> *pInstructions, int tokenID, int &consumed );
			int ParseVariableDec( int allowDefaultValue, stVariable *pVar, cProgramData *pProgram, int tokenID, int &consumed );
			int ParseTypeVariableDec( stVariable *pVar, cProgramData *pProgram, int tokenID, int &consumed );
			int ParseParamVariableDec( stVariable *pVar, int &ref, cProgramData *pProgram, int tokenID, int &consumed );

			int ParseArrayStatement( stFunction *pFunction, cProgramData *pProgram, stVariable *pParentVar, int tokenID, int &consumed );
			int ParseTypeStatement( stFunction *pFunction, cProgramData *pProgram, stVariable *pParentVar, int tokenID, int &consumed );

			int ParseArrayInc( int inc, stFunction *pFunction, cProgramData *pProgram, stVariable *pParentVar, int tokenID, int &consumed );
			int ParseTypeInc( int inc, stFunction *pFunction, cProgramData *pProgram, stVariable *pParentVar, int tokenID, int &consumed );

			int ParseArrayExpression( stFunction *pFunction, cProgramData *pProgram, int &dataType, stVariable *pParentVar, DynArrayObject<stInstruction> *pInstructions, int tokenID, int &consumed, int allowUnassignedVars=0 );
			int ParseTypeExpression( stFunction *pFunction, cProgramData *pProgram, int &dataType, stVariable *pParentVar, DynArrayObject<stInstruction> *pInstructions, int tokenID, int &consumed, int allowUnassignedVars=0 );
			int ParseExpression( stFunction *pFunction, int allowFunctions, int prevPrec, int &dataType, DynArrayObject<stInstruction> *pInstructions, cProgramData *pProgram, int tokenID, int &consumed, int allowUnassignedVars=0 );

			int ParseFunctionStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed );
			int ParseIdentifierStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed );
			int ParseThenStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed );
			int ParseIncDecStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed );
			int ParseIfStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed );
			int ParseStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed );

			int ParsePluginFunctionStatement( stFunction *pFunction, cProgramData *pProgram, int tokenID, int &consumed, int pluginIndex );

			int FirstPass( cProgramData *pProgram, int replaceConstants=1 );
			int ParseFunctions( cProgramData *pProgram );
	};

}

#endif