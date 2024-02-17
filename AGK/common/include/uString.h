// String Header
#ifndef _H_AGKUSTRING_
#define _H_AGKUSTRING_

#ifndef AGK_COMPILER
	#include "Common.h"
#endif

namespace AGK
{
	class uString
	{
		protected:
			// a 64bit interpreter depends on this struct being a multiple of 8 bytes in size for use in AGK types
			char* m_pData;
			unsigned int m_iNumChars; // number of UTF-8 characters in the string
			unsigned int m_iLength; // length of the current string in bytes
			unsigned int m_iTotalLength; // length of the array holding the string, might have room for more data
			unsigned short m_iCachedByteOffset; // cache the most recent char<->byte offset check in case the next one is nearby
			unsigned short m_iCachedCharOffset; // unsigned shorts to maintain 8 byte multiple
			
			static int UnicodeCaseFoldedChar( unsigned int input, unsigned int *output ); // for case insensitive comparison, output must be at least 3 ints in size
			static int UnicodeUpperCaseChar( int input, int *output ); // converts to upper case, output must be at least 3 ints in size
			static int UnicodeLowerCaseChar( int input ); // converts to lower case, currently guaranteed to be a single character
		
		public:
			
			static int IsUTF8( const char* str, int *numChars=0, int maxBytes=0x7FFFFFFF, int maxChars=0x7FFFFFFF );
			static int DecodeUTF8Char( const char* str, int *numBytes ); // output numBytes optional
			static int CountUTF8Chars( const char* str, int *length ); // output length optional
			static char* ConvertToUTF8( const char* str, int *length, int *chars, int max=0x7FFFFFFF ); // output length and chars optional
			static char* ConvertUTF16ToUTF8( const char* str, int *length, int *chars, int max=0x7FFFFFFF ); // output length and chars optional
			static unsigned int GetByteOffsetStatic( const char *str, unsigned int charOffset );
			//static int GetUTF8FromChar( unsigned int c, char* szOutput ); // szOutput must be at least 4 chars in size
			static int GetUTF8FromUnicode( unsigned int c, char* szOutput ); // szOutput must be at least 4 chars in size
			static int GetUnicodeFromChar( unsigned int c ); // convert pre C++11 char into its Unicode value
			static uint8_t DecodeHexChar( unsigned char c );
						
			uString();
			uString( const uString &szString );
			uString( const char* szString, unsigned int initialLength=0 ); //can define a longer array than the data given
			~uString();

			unsigned int GetByteOffset( unsigned int charOffset );
			unsigned int GetByteOffsetConst( unsigned int charOffset ) const;

			void Reset();
			void Init();

			void CheckSize( unsigned int size, bool preserve );

			void SetStr( const char* szString ); // accepts UTF-8 or Windows-1252
			void SetStrUTF8( const char* szString ); // faster if we can assume the string is valid UTF8
			void SetStr( const uString &szString );
			void SetStrN( const char* szString, unsigned int length ); // accepts UTF-8 or Windows-1252
			inline const char* GetStr() const { if ( !m_pData || m_iLength == 0 ) return ""; else return m_pData; }
			inline unsigned int GetLength() const { return m_iLength; }
			inline unsigned int GetNumChars() const { return m_iNumChars; }
			void ClearTemp(); // call if you want to empty the string but will likely be reusing its capacity soon

			uString& Prepend( const char* szString );
			uString& Prepend( const uString &szString );

			//uString& AppendChar( unsigned int c ); // append a C++ char literal encoded as UTF-8
			uString& AppendUnicode( unsigned int c ); // append a Unicode character in the range 1 to 0x10FFFF
			uString& AppendAscii( int c ); // append an Ascii character in the range 1 to 127
			uString& AppendByte( char b ); // append a single byte to the string, must be followed up by the rest of the UTF-8 bytes
			uString& AppendInt( int num ); 
			uString& AppendUInt( unsigned int num );
			uString& AppendFloat( float num );
			uString& Append( const char* szString ); // Append a string encoded in UTF-8 or Windows-1252
			uString& AppendUTF8( const char* szString ); // Append a string encoded in UTF-8
			uString& AppendAscii( const char* szString ); // Append a pure ASCII string
			uString& Append( const uString &sString );
			uString& AppendN( const char* szString, unsigned int max ); // adds a null terminator to the internal string when done, accepts UTF-8 and Windows-1252

			//uString& InsertCharAt( unsigned int index, unsigned int c ); // c must be a C++ char literal encoded in UTF-8
			uString& InsertUnicodeAt( unsigned int index, unsigned int c ); // c must be a unicode character between 1 and 0x10FFFF
			uString& DeleteCharAt( unsigned int index );

			uString& Format( const char* szString, ... ); //same format as sprintf

			uString& Strip( const char* tokens ); //removes all characters in "tokens" from the string
            uString& Strip( unsigned int token ); //removes all instances of "token" from the string
            uString& StripUTF8(); // remove all multi-byte UTF8 characters
			uString& Trim( const char* tokens ); //removes all characters in "tokens" from the ends of the string
			uString& Replace( unsigned int find, unsigned int replace ); //replaces all instances of "find" with "replace"
			uString& ReplaceStr( const char* find, const char* replace, int qty=-1 ); //replaces all instances of "find" with "replace"
			uString& Upper( ); //converts to upper case
			uString& Lower( ); //converts to lower case
			uString& Unescape(); //converts backslash escaped characters to their true values

			int ByteAt( unsigned int index );
			int CharAt( unsigned int index );
			int CharAtConst( unsigned int index ) const;
			int Find( int cFind ) const; //finds the first instance of the character and returns its index
			int RevFind( int cFind ) const; //finds the last instance of the character and returns its index
			void Trunc( int cFind ); //removes all characters after the last occurance of cFind, including that character
			void Trunc2( int count ); //removes the last count characters from the string
			void Keep( int count ); //keeps the first specified number of characters and truncates the rest
			int FindStrCount( const char* str, int ignorecase=0, int start=0 );
			int FindStr( const char* str, int ignorecase=0, int start=0 );
			int RevFindStr( const char* str, int ignorecase=0, int start=-1 );
			int Count( int find ); // counts the occurances of the given character

			int ToInt() const;
			float ToFloat() const;

			// "delimit" is a list of characters that split words
			int CountTokens( const char* delimit );
			char* GetToken( const char* delimit, int index );
			int GetToken( const char* delimit, int index, uString &out );

			// "delimit" is a single character that splits sections
			int CountTokens2( int delimit );
			int GetToken2( int delimit, int index, uString &out );
			int SplitTokens2( int delimit, uString* &out );
			
			int CompareTo( const uString &pTo ) const;
			int CompareTo( const char* szTo ) const;
        
            int CompareToN( const uString &pTo, int max ) const;
            int CompareToN( const char* szTo, int max ) const;
        
            int CompareCaseTo( const uString &pTo ) const;
            int CompareCaseTo( const char* szTo ) const;
        
            int CompareCaseToN( const uString &pTo, int max ) const;
            int CompareCaseToN( const char* szTo, int max ) const;

			int GetMatchLength( const char* szOther ) const;
			int GetMatchCaseLength( const char* szOther ) const; // case insensitive

			int SimplifyPath(); // returns 0 if invalid, converts any backslashes to forward slashes and resolves any ../

			inline uString& operator =( const char* str ) { SetStr(str); return *this; }
			inline uString& operator =( const uString& str ) { SetStr(str); return *this; }
			inline uString& operator +=( const char* str ) { return Append(str); }
			inline uString& operator +=( unsigned int u ) { return AppendUInt(u); }
			inline uString& operator +=( int i ) { return AppendInt(i); }
			inline uString& operator +=( float f ) { return AppendFloat(f); }
			inline uString& operator +=( const uString& str ) { return Append(str); }
			inline bool operator ==( const uString& str ) const { return CompareTo(str) == 0; }
			inline bool operator !=( const uString& str ) const { return CompareTo(str) != 0; }
			inline operator const char *() const { if ( !m_pData || m_iLength == 0 ) return ""; else return m_pData; }
		
			void SubString( uString &output, int start, int length=-1 );

			void XOR( unsigned char x );
	};
}

#endif
