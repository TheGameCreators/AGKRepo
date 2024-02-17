
#ifdef AGK_COMPILER
	#include "../../CompilerNew/CompilerCommon.h"
#endif

#include "../include/uString.h"

#include "../include/UnicodeMappings.h"

using namespace AGK;

unsigned int uString::GetByteOffsetStatic( const char *str, unsigned int charOffset )
{
	// assumes valid UTF-8 for performance reasons

	if ( charOffset == 0 ) return 0;
	
	// start from the beginning
	unsigned char *ustr = (unsigned char*) str;
	unsigned int currOffset = 0;
	while ( *ustr && currOffset < charOffset )
	{
		unsigned char u = (*ustr) >> 4;
		switch ( u )
		{
			case 0xF : ustr += 4; break;
			case 0xE : ustr += 3; break;
			case 0xD : ustr += 2; break;
			case 0xC : ustr += 2; break;
			default: ustr += 1;
		}
		
		currOffset++;
	}

	unsigned int byteOffset = (unsigned int) (ustr - (unsigned char*)str);
	return byteOffset;
}

unsigned int uString::GetByteOffsetConst( unsigned int charOffset ) const
{
	// assumes valid UTF-8 for performance reasons

	if ( m_iNumChars == 0 ) return 0;
	if ( m_iNumChars == m_iLength ) return charOffset;
	if ( charOffset == 0 ) return 0;
	if ( m_iCachedCharOffset == charOffset ) return m_iCachedByteOffset;
	if ( charOffset >= m_iNumChars ) charOffset = m_iNumChars-1;

	unsigned char *ustr;
	unsigned int currOffset;
	unsigned int diff = abs((int)m_iCachedCharOffset - (int)charOffset);
	if ( diff < charOffset || diff < (m_iNumChars-charOffset) )
	{
		// cached offset is closer than starting from the beginning or end
		if ( charOffset < m_iCachedCharOffset )
		{
			// go backwards
			ustr = (unsigned char*) (m_pData + m_iCachedByteOffset);

			currOffset = m_iCachedCharOffset;
			while( currOffset > charOffset )
			{
				do { ustr--; } while( (*ustr & 0xC0) == 0x80 );

				currOffset--;
			}
		}
		else
		{
			// go forwards
			ustr = (unsigned char*) (m_pData + m_iCachedByteOffset);

			currOffset = m_iCachedCharOffset;
			while ( currOffset < charOffset )
			{
				unsigned char u = (*ustr) >> 4;
				switch ( u )
				{
					case 0xF : ustr += 4; break;
					case 0xE : ustr += 3; break;
					case 0xD : ustr += 2; break;
					case 0xC : ustr += 2; break;
					default: ustr += 1;
				}

				currOffset++;
			}
		}
	}
	else
	{
		if ( charOffset < m_iNumChars-charOffset )
		{
			// start from the beginning
			ustr = (unsigned char*) m_pData;

			currOffset = 0;
			while ( currOffset < charOffset )
			{
				unsigned char u = (*ustr) >> 4;
				switch ( u )
				{
					case 0xF : ustr += 4; break;
					case 0xE : ustr += 3; break;
					case 0xD : ustr += 2; break;
					case 0xC : ustr += 2; break;
					default: ustr += 1;
				}
				
				currOffset++;
			}
		}
		else
		{
			// start from the end
			ustr = (unsigned char*) (m_pData + m_iLength - 1);
			while( (*ustr & 0xC0) == 0x80 ) ustr--; // find the beginning of the final character

			currOffset = m_iNumChars-1;
			while( currOffset > charOffset )
			{
				do { ustr--; } while( (*ustr & 0xC0) == 0x80 );

				currOffset--;
			}
		}
	}

	unsigned int byteOffset = (unsigned int) (ustr - (unsigned char*)m_pData);
	return byteOffset;
}

unsigned int uString::GetByteOffset( unsigned int charOffset )
{
	// assumes valid UTF-8 for performance reasons

	if ( m_iNumChars == 0 ) return 0;
	if ( charOffset >= m_iNumChars ) charOffset = m_iNumChars-1;
	unsigned int byteOffset = GetByteOffsetConst( charOffset );

	// cache the result, unless it is out of range of an unsigned short, or close to the beginning or end
	if ( charOffset < 65535 && charOffset < 65535 && charOffset > 5 && charOffset < m_iNumChars-5 )
	{
		m_iCachedCharOffset = charOffset;
		m_iCachedByteOffset = byteOffset;
	}

	return byteOffset;
}

int uString::IsUTF8( const char* str, int *numChars, int maxBytes, int maxChars )
{
	// does not check for invalid characters, only that the string looks like it is UTF-8
	// this is to decide whether we should try converting this string to UTF-8

	int charCount = 0;
	if ( numChars ) *numChars = 0;
	const unsigned char* ustr = (const unsigned char*) str;
	while( *ustr )
	{
		int currBytes = (int)((const char*)ustr - str);
		if ( currBytes == maxBytes ) return 1;
		else if ( currBytes > maxBytes ) return 0; // byte limit spans a character

		if ( charCount >= maxChars ) return 1;

		charCount++;
		if ( numChars ) *numChars = charCount;

		if ( *ustr < 128 ) 
		{
			// one byte
			ustr++;
			continue;
		}

		if ( (*ustr & 0xE0) == 0xC0 ) 
		{
			// two bytes
			if ( (ustr[1] & 0xC0) != 0x80 ) return 0; // second byte must start 10xxxxxx
			ustr += 2;
			continue;
		}

		if ( (*ustr & 0xF0) == 0xE0 ) 
		{
			// three bytes
			if ( (ustr[1] & 0xC0) != 0x80 ) return 0; // second byte must start 10xxxxxx
			if ( (ustr[2] & 0xC0) != 0x80 ) return 0; // third byte must start 10xxxxxx
			ustr += 3;
			continue;
		}

		if ( (*ustr & 0xF8) == 0xF0 ) 
		{
			// four bytes
			if ( (ustr[1] & 0xC0) != 0x80 ) return 0; // second byte must start 10xxxxxx
			if ( (ustr[2] & 0xC0) != 0x80 ) return 0; // third byte must start 10xxxxxx
			if ( (ustr[3] & 0xC0) != 0x80 ) return 0; // fourth byte must start 10xxxxxx
			ustr += 4;
			continue;
		}

		return 0;
	}

	return 1;
}

int uString::DecodeUTF8Char( const char* str, int *numBytes )
{
	if (numBytes) *numBytes = 1;

	const unsigned char* ustr = (const unsigned char*) str;
	if ( *ustr < 128 ) 
	{
		// one byte
		return *ustr;
	}
	else
	{
		int result;

		if ( *ustr < 194 ) return -1; // not valid as the first byte

		if ( *ustr < 224 )
		{
			// two bytes
			result = (*ustr & 0x1F);
			result <<= 6;

			ustr++;
			if ( (*ustr & 0xC0) != 0x80 ) return -1; // second byte must start 10xxxxxx
			result |= (*ustr & 0x3F);

			if (numBytes) (*numBytes)++;
			
			return result;
		}
		
		if ( *ustr < 240 )
		{
			// three bytes
			result = (*ustr & 0x0F);
			result <<= 6;

			ustr++;
			if ( (*ustr & 0xC0) != 0x80 ) return -1; // second byte must start 10xxxxxx
			result |= (*ustr & 0x3F);
			result <<= 6;

			if (numBytes) (*numBytes)++;

			ustr++;
			if ( (*ustr & 0xC0) != 0x80 ) return -1; // third byte must start 10xxxxxx
			result |= (*ustr & 0x3F);

			if (numBytes) (*numBytes)++;

			if ( result < 0x800 ) return -1; // overlong encoding
			if ( result >= 0xD800 && result <= 0xDFFF ) return -1; // reserved for UTF-16, not valid characters
			
			return result;
		}
		
		if ( *ustr < 245 )
		{
			// four bytes
			result = (*ustr & 0x07);
			result <<= 6;

			ustr++;
			if ( (*ustr & 0xC0) != 0x80 ) return -1; // second byte must start 10xxxxxx
			result |= (*ustr & 0x3F);
			result <<= 6;

			if (numBytes) (*numBytes)++;

			ustr++;
			if ( (*ustr & 0xC0) != 0x80 ) return -1; // third byte must start 10xxxxxx
			result |= (*ustr & 0x3F);
			result <<= 6;

			if (numBytes) (*numBytes)++;

			ustr++;
			if ( (*ustr & 0xC0) != 0x80 ) return -1; // fourth byte must start 10xxxxxx
			result |= (*ustr & 0x3F);

			if (numBytes) (*numBytes)++;

			if ( result < 0x10000 ) return -1; // overlong encoding
			if ( result > 0x10FFFF ) return -1; // outside valid character range
			
			return result;
		}
		
		// invalid
		return -1;
	}
}

int uString::CountUTF8Chars( const char* s, int *length )
{
	int count = 0;
	int i = 0;
	int iBefore = 0;

	while (s[i] > 0) {
ascii:
		i++;
	}

	count += i - iBefore;

	while (s[i]) {
		if (s[i] > 0) {
			iBefore = i;
			goto ascii;
		} else {
			switch (0xF0 & s[i]) {
				case 0xE0: i += 3; break;
				case 0xF0: i += 4; break;
				default: i += 2; break;
			}
		}

		count++;
	}

	if ( length ) *length = i;
	return count;
}

// return value must be deleted when no longer needed
char* uString::ConvertToUTF8( const char* str, int *length, int *chars, int max )
{
	int requiredBytes = 0;
	const unsigned char* ptr = (const unsigned char*) str;
	while( *ptr && (const char*)ptr-str < max )
	{
		if ( *ptr < 128 ) requiredBytes++;
		else if ( *ptr < 160 ) requiredBytes += 3; // over estimate translated chars
		else requiredBytes += 2;
		
		ptr++;
	}

	if ( chars ) *chars = (int) ((const char*)ptr - str);
	
	char *utf8str = new char[ requiredBytes + 1 ];
	unsigned char *ptr2 = (unsigned char*) utf8str;
	ptr = (const unsigned char*) str;
	while( *ptr && (const char*)ptr-str < max )
	{
		if ( *ptr < 128 ) 
		{
			// single byte
			*ptr2 = *ptr;
			ptr2++;
		}
		else
		{
			// two bytes
			if ( *ptr >= 0xA0 )
			{
				// direct copy
				ptr2[0] = 0xC0 | (*ptr >> 6);
				ptr2[1] = 0x80 | (*ptr & 0x3F);
				ptr2 += 2;
			}
			else
			{
				// translate
				switch( *ptr )
				{
					case 0x80: ptr2[0] = 0xE2; ptr2[1] = 0x82; ptr2[2] = 0xAC; ptr2 += 3; break;
					case 0x82: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0x9A; ptr2 += 3; break;
					case 0x83: ptr2[0] = 0xC6; ptr2[1] = 0x92; ptr2 += 2; break;
					case 0x84: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0x9E; ptr2 += 3; break;
					case 0x85: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0xA6; ptr2 += 3; break;
					case 0x86: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0xA0; ptr2 += 3; break;
					case 0x87: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0xA1; ptr2 += 3; break;
					case 0x88: ptr2[0] = 0xCB; ptr2[1] = 0x86; ptr2 += 2; break;
					case 0x89: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0xB0; ptr2 += 3; break;
					case 0x8A: ptr2[0] = 0xC5; ptr2[1] = 0xA0; ptr2 += 2; break;
					case 0x8B: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0xB9; ptr2 += 3; break;
					case 0x8C: ptr2[0] = 0xC5; ptr2[1] = 0x92; ptr2 += 2; break;
					case 0x8E: ptr2[0] = 0xC5; ptr2[1] = 0xBD; ptr2 += 2; break;

					case 0x91: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0x98; ptr2 += 3; break;
					case 0x92: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0x99; ptr2 += 3; break;
					case 0x93: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0x9C; ptr2 += 3; break;
					case 0x94: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0x9D; ptr2 += 3; break;
					case 0x95: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0xA2; ptr2 += 3; break;
					case 0x96: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0x93; ptr2 += 3; break;
					case 0x97: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0x94; ptr2 += 3; break;
					case 0x98: ptr2[0] = 0xCB; ptr2[1] = 0x9C; ptr2 += 2; break;
					case 0x99: ptr2[0] = 0xE2; ptr2[1] = 0x84; ptr2[2] = 0xA2; ptr2 += 3; break;
					case 0x9A: ptr2[0] = 0xC5; ptr2[1] = 0xA1; ptr2 += 2; break;
					case 0x9B: ptr2[0] = 0xE2; ptr2[1] = 0x80; ptr2[2] = 0xBA; ptr2 += 3; break;
					case 0x9C: ptr2[0] = 0xC5; ptr2[1] = 0x93; ptr2 += 2; break;
					case 0x9E: ptr2[0] = 0xC5; ptr2[1] = 0xBE; ptr2 += 2; break;
					case 0x9F: ptr2[0] = 0xC5; ptr2[1] = 0xB8; ptr2 += 2; break;
					default: *ptr2 = 0x3f; ptr2++; // question mark for invalid characters
				}
			}
		}

		ptr++;
	}

	*ptr2 = 0;

	if ( length ) *length = (int) ((char*)ptr2 - utf8str);

	return utf8str;
}

// return value must be deleted when no longer needed
char* uString::ConvertUTF16ToUTF8( const char* str, int *length, int *chars, int max )
{
	const unsigned short* utf16 = (const unsigned short*) str;
	const unsigned short* ptr = utf16;

	int numChars = 0;
	int requiredBytes = 0;
	while( *ptr && (const char*)ptr-str < max )
	{
		if ( *ptr < 128 ) requiredBytes++;
		else if ( *ptr < 2048 ) requiredBytes += 2;
		else 
		{
			if ( *ptr < 0xD800 || *ptr >= 0xE000 ) requiredBytes += 3;
			else 
			{
				if ( *ptr < 0xDC00 && *(ptr+1) >= 0xDC00 && *(ptr+1) < 0xE000 ) 
				{
					// valid UTF-16 surrogate pair
					requiredBytes += 4;
					ptr++;
				}
				else 
				{
					// invalid UTF-16 surrogate pair, will replace with question mark
					requiredBytes += 1;
				}
			}
		}
		
		numChars++;
		ptr++;
	}

	if ( chars ) *chars = numChars;
	
	char *utf8str = new char[ requiredBytes + 1 ];
	unsigned char *ptr2 = (unsigned char*) utf8str;
	ptr = utf16;
	while( *ptr && (const char*)ptr-str < max )
	{
		if ( *ptr < 128 ) 
		{
			// single byte
			*ptr2 = (unsigned char) *ptr;
			ptr2++;
		}
		else if ( *ptr < 2048 ) 
		{
			// two bytes
			ptr2[0] = 0xC0 | (*ptr >> 6);
			ptr2[1] = 0x80 | (*ptr & 0x3F);
			ptr2 += 2;
		}
		if ( *ptr < 0xD800 || *ptr >= 0xE000 ) 
		{
			// three bytes
			ptr2[0] = 0xE0 | (*ptr >> 12);
			ptr2[1] = 0x80 | ((*ptr >> 6) & 0x3F);
			ptr2[2] = 0x80 | (*ptr & 0x3F);
			ptr2 += 3;
		}
		else
		{
			if ( *ptr < 0xDC00 && *(ptr+1) >= 0xDC00 && *(ptr+1) < 0xE000 ) 
			{
				// valid UTF-16 surrogate pair
				unsigned int unichar = *(ptr+1) & 0x3FF;
				unichar <<= 10;
				unichar |= *ptr & 0x3FF;
				ptr++;

				ptr2[0] = 0xF0 | (unichar >> 18);
				ptr2[1] = 0x80 | ((unichar >> 12) & 0x3F);
				ptr2[2] = 0x80 | ((unichar >> 6) & 0x3F);
				ptr2[3] = 0x80 | (unichar & 0x3F);
				ptr2 += 4;
			}
			else 
			{
				// invalid UTF-16 surrogate pair, replace with question mark
				*ptr2 = 0x3f; ptr2++; 
			}
		}

		ptr++;
	}

	*ptr2 = 0;

	if ( length ) *length = (int) ((char*)ptr2 - utf8str);

	return utf8str;
}

/*
// returns the number of bytes used for the UTF8 char, or 0 if invalid. c is the result of literal character encoding i.e. 'A' with UTF8 encoding
int uString::GetUTF8FromChar( unsigned int c, char* szOutput )
{
	char *cptr = (char*)&c;
	int charLength;

	if ( c < 0xFF ) 
	{
		charLength = 1;
		if ( cptr[0] < 0 ) return 0; // invalid UTF8 
		szOutput[0] = cptr[0];
	}
	else if ( c < 0xFFFF ) 
	{
		charLength = 2;
		if ( (cptr[1] & 0xE0) != 0xC0 ) return 0; // invalid UTF8
		if ( (cptr[0] & 0xC0) != 0x80 ) return 0; // invalid UTF8
		szOutput[0] = cptr[1];
		szOutput[1] = cptr[0];
	}
	else if ( c < 0xFFFFFF ) 
	{
		charLength = 3;
		if ( (cptr[2] & 0xF0) != 0xE0 ) return 0; // invalid UTF8
		if ( (cptr[1] & 0xC0) != 0x80 ) return 0; // invalid UTF8
		if ( (cptr[0] & 0xC0) != 0x80 ) return 0; // invalid UTF8
		szOutput[0] = cptr[2];
		szOutput[1] = cptr[1];
		szOutput[2] = cptr[0];
	}
	else 
	{
		charLength = 4;
		if ( (cptr[3] & 0xF8) != 0xF0 ) return 0; // invalid UTF8
		if ( (cptr[2] & 0xC0) != 0x80 ) return 0; // invalid UTF8
		if ( (cptr[1] & 0xC0) != 0x80 ) return 0; // invalid UTF8
		if ( (cptr[0] & 0xC0) != 0x80 ) return 0; // invalid UTF8
		szOutput[0] = cptr[3];
		szOutput[1] = cptr[2];
		szOutput[2] = cptr[1];
		szOutput[3] = cptr[0];
	}

	return charLength;
}
*/

int uString::GetUTF8FromUnicode( unsigned int c, char* szOutput )
{
	if ( c > 0x10FFFF ) return 0;

	if ( c < 128 )
	{
		szOutput[0] = c;
		return 1;
	}
	else if ( c < 2048 )
	{
		szOutput[0] = 0xC0 | (c >> 6);
		szOutput[1] = 0x80 | (c & 0x3F);
		return 2;
	}
	else if ( c < 65536 )
	{
		szOutput[0] = 0xE0 | (c >> 12);
		szOutput[1] = 0x80 | ((c >> 6) & 0x3F);
		szOutput[2] = 0x80 | (c & 0x3F);
		return 3;
	}
	else
	{
		szOutput[0] = 0xF0 | (c >> 18);
		szOutput[1] = 0x80 | ((c >> 12) & 0x3F);
		szOutput[2] = 0x80 | ((c >> 6) & 0x3F);
		szOutput[3] = 0x80 | (c & 0x3F);
		return 4;
	}
}

int uString::GetUnicodeFromChar( unsigned int c )
{
	char *cptr = (char*)&c;
	int result = 0xFFFD;

	if ( c < 0xFF ) 
	{
		if ( cptr[0] < 0 ) return 0xFFFD; // invalid UTF8 
		return c;
	}
	else if ( c < 0xFFFF ) 
	{
		if ( (cptr[1] & 0xE0) != 0xC0 ) return 0xFFFD; // invalid UTF8
		if ( (cptr[0] & 0xC0) != 0x80 ) return 0xFFFD; // invalid UTF8
		
		result = (cptr[1] & 0x1F);
		result <<= 6;
		result |= (cptr[0] & 0x3F);
	}
	else if ( c < 0xFFFFFF ) 
	{
		if ( (cptr[2] & 0xF0) != 0xE0 ) return 0xFFFD; // invalid UTF8
		if ( (cptr[1] & 0xC0) != 0x80 ) return 0xFFFD; // invalid UTF8
		if ( (cptr[0] & 0xC0) != 0x80 ) return 0xFFFD; // invalid UTF8
		
		result = (cptr[2] & 0x0F);
		result <<= 6;
		result |= (cptr[1] & 0x3F);
		result <<= 6;
		result |= (cptr[0] & 0x3F);

		if ( result < 0x800 ) return 0xFFFD; // overlong encoding
		if ( result >= 0xD800 && result <= 0xDFFF ) return 0xFFFD; // reserved for UTF16
	}
	else 
	{
		if ( (cptr[3] & 0xF8) != 0xF0 ) return 0xFFFD; // invalid UTF8
		if ( (cptr[2] & 0xC0) != 0x80 ) return 0xFFFD; // invalid UTF8
		if ( (cptr[1] & 0xC0) != 0x80 ) return 0xFFFD; // invalid UTF8
		if ( (cptr[0] & 0xC0) != 0x80 ) return 0xFFFD; // invalid UTF8
		
		result = (cptr[3] & 0x07);
		result <<= 6;
		result |= (cptr[2] & 0x3F);
		result <<= 6;
		result |= (cptr[1] & 0x3F);
		result <<= 6;
		result |= (cptr[0] & 0x3F);

		if ( result < 0x10000 ) return 0xFFFD; // overlong encoding
		if ( result > 0x10FFFF ) return 0xFFFD; // outside valid character range
	}

	return result;
}

int uString::UnicodeCaseFoldedChar( unsigned int input, unsigned int *output )
{
	// ascii characters don't need to be looked up
	if ( input < 128 )
	{
		if ( input >= 65 && input <= 90 ) output[0] = input + 32;
		else output[0] = input;
		return 1;
	}
	else
	{
		// find closest value in g_iCaseFoldingStartValues
		unsigned int high = (sizeof(g_iCaseFoldingStartValues) / sizeof(int)) - 1;
		unsigned int low = 0;

		// next most likely case is a latin extended character, so only do this loop if necessary
		if ( input > (unsigned int)g_iCaseFoldingEndValues[ 0 ] ) 
		{
			while( high > low )
			{
				unsigned int mid = (high+low+1)/2;
				if( (unsigned int)g_iCaseFoldingStartValues[ mid ] > input ) high = mid-1;
				else if ( (unsigned int)g_iCaseFoldingStartValues[ mid ] < input ) low = mid;
				else { low = mid; break; }
			}

			// check it is in the range of valid values
			if ( input > (unsigned int)g_iCaseFoldingEndValues[ low ] ) 
			{
				output[ 0 ] = input;
				return 1;
			}
		}

		unsigned int index = input - (unsigned int)g_iCaseFoldingStartValues[ low ];
		unsigned int replacement = ((unsigned int**)g_iCaseFoldingReplacements)[ low ][ index ];
		if ( replacement == 0 ) 
		{
			output[ 0 ] = input;
			return 1;
		}

		unsigned int type = replacement >> 30;
		switch( type )
		{
			case 0: // 1 character
			{
				output[ 0 ] = (replacement & 0x3FFFFFFF);
				return 1;
				break;
			}
			case 1: // 2 characters
			{
				output[ 0 ] = (replacement & 0x7FFF);
				output[ 1 ] = ((replacement >> 15) & 0x7FFF);
				return 2;
				break;
			}
			case 2: // 3 characters
			{
				output[ 0 ] = (replacement & 0x3FF);
				output[ 1 ] = ((replacement >> 10) & 0x3FF);
				output[ 2 ] = ((replacement >> 20) & 0x3FF);
				return 3;
				break;
			}
			case 3: // more than 3, or out of range
			{
				// currently unsupported, and not yet needed
				break;
			}
		}

		output[ 0 ] = input;
		return 1;
	}
}

int uString::UnicodeUpperCaseChar( int input, int *output )
{
	// ascii characters can take a shortcut
	if ( input < 128 )
	{
		if ( input >= 97 && input <= 122 ) output[0] = input - 32;
		else output[0] = input;
		return 1;
	}
	else
	{
		// find closest value in g_iUpperCaseStartValues
		unsigned int high = (sizeof(g_iUpperCaseStartValues) / sizeof(int)) - 1;
		unsigned int low = 0;

		// next most likely case is a latin extended character, so only do this loop if necessary
		if ( input > g_iUpperCaseEndValues[ 0 ] ) 
		{
			while( high > low )
			{
				unsigned int mid = (high+low+1)/2;
				if( g_iUpperCaseStartValues[ mid ] > input ) high = mid-1;
				else if ( g_iUpperCaseStartValues[ mid ] < input ) low = mid;
				else { low = mid; break; }
			}

			// check it is in the range of valid values
			if ( input > g_iUpperCaseEndValues[ low ] ) 
			{
				output[ 0 ] = input;
				return 1;
			}
		}

		int index = input - g_iUpperCaseStartValues[ low ];
		unsigned int replacement = ((unsigned int**)g_iUpperCaseReplacements)[ low ][ index ];
		if ( replacement == 0 ) 
		{
			output[ 0 ] = input;
			return 1;
		}

		unsigned int type = replacement >> 30;
		switch( type )
		{
			case 0: // 1 character
			{
				output[ 0 ] = (replacement & 0x3FFFFFFF);
				return 1;
				break;
			}
			case 1: // 2 characters
			{
				output[ 0 ] = (replacement & 0x7FFF);
				output[ 1 ] = ((replacement >> 15) & 0x7FFF);
				return 2;
				break;
			}
			case 2: // 3 characters
			{
				output[ 0 ] = (replacement & 0x3FF);
				output[ 1 ] = ((replacement >> 10) & 0x3FF);
				output[ 2 ] = ((replacement >> 20) & 0x3FF);
				return 3;
				break;
			}
			case 3: // more than 3, or out of range
			{
				// currently unsupported, and not yet needed
				break;
			}
		}

		output[ 0 ] = input;
		return 1;
	}
}

int uString::UnicodeLowerCaseChar( int input )
{
	// ascii characters can take a shortcut
	if ( input < 128 )
	{
		if ( input >= 65 && input <= 90 ) return input + 32;
		else return input;
	}
	else
	{
		// find closest value in g_iLowerCaseStartValues
		unsigned int high = (sizeof(g_iLowerCaseStartValues) / sizeof(int)) - 1;
		unsigned int low = 0;

		// next most likely case is a latin extended character, so only do this loop if necessary
		if ( input > g_iLowerCaseEndValues[ 0 ] ) 
		{
			while( high > low )
			{
				unsigned int mid = (high+low+1)/2;
				if( g_iLowerCaseStartValues[ mid ] > input ) high = mid-1;
				else if ( g_iLowerCaseStartValues[ mid ] < input ) low = mid;
				else { low = mid; break; }
			}

			// check it is in the range of valid values
			if ( input > g_iLowerCaseEndValues[ low ] ) 
			{
				return input;
			}
		}

		int index = input - g_iLowerCaseStartValues[ low ];
		if ( g_iLowerCaseReplacements[ low ][ index ] == 0 ) return input;
		else return g_iLowerCaseReplacements[ low ][ index ];
	}
}

uint8_t uString::DecodeHexChar( unsigned char c )
{
	if ( c < 48 ) return 0xFF;
	if ( c < 58 ) return c - 48;
	if ( c < 65 ) return 0xFF;
	if ( c < 71 ) return c - 55;
	if ( c < 97 ) return 0xFF;
	if ( c < 103 ) return c - 87;
	return 0xFF;
}

uString::uString()
{
	m_pData = 0;
	m_iNumChars = 0;
	m_iLength = 0;
	m_iTotalLength = 0;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;
}

//****if* uString/String
// FUNCTION
//   Creates a string object with an intital value and an initial data length. If the intitial length is greater than the string length extra space will be reserved, 
//   useful if data will be appened to this string in future.
// INPUTS
//   szString -- The initial value for the string
//   initialLength -- The length of the array to hold the string, defaults to the initial string length
// SOURCE
uString::uString( const char* szString, unsigned int initialLength )
//****
{
	// clear the variables and use the set function
	m_pData = 0;
	m_iNumChars = 0;
	m_iLength = 0;
	m_iTotalLength = 0;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;

	// pre allocate the data array to a given size, plus null terminator
	if ( initialLength > 0 && IsUTF8(szString, (int*)&m_iNumChars) )
	{
		unsigned int length = (unsigned int)strlen( szString );
		if ( initialLength < length ) SetStr( szString );
		else
		{
			m_pData = new char[ initialLength + 1 ];
			m_iTotalLength = initialLength+1;
			memcpy( m_pData, szString, length+1 );
			m_iLength = length;
		}
	}
	else
	{
		SetStr( szString );
	}
}

uString::uString( const uString &sString )
{
	// clear the variables and use the set function
	m_pData = 0;
	m_iNumChars = 0;
	m_iLength = 0;
	m_iTotalLength = 0;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;

	SetStr( sString );
}

uString::~uString()
{
	Reset();
}

void uString::Reset()
{
	if ( m_pData )
	{
		delete [] m_pData;
		m_pData = 0;
	}
	
	m_iNumChars = 0;
	m_iLength = 0;
	m_iTotalLength = 0;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;
}

void uString::Init()
{
	m_pData = 0;
	m_iNumChars = 0;
	m_iLength = 0;
	m_iTotalLength = 0;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;
}

void uString::CheckSize( unsigned int size, bool preserve )
{
	unsigned int newLength = m_iTotalLength;
	if ( size < 3 ) size = 3;

	// check if we have enough allocated memory for this string
	if ( m_iTotalLength <= size )
	{
		// not enough room, reallocate
		newLength = size + size/2;
	}
	else if ( m_iTotalLength > size*3 )
	{
		// wasting space, reallocate
		newLength = m_iTotalLength / 2; // reduce gradually in case small storage is temporary
	}

	if ( m_iTotalLength != newLength )
	{
		if ( preserve )
		{
			char *pNewData = new char[ newLength ];
			if ( m_pData )
			{
				memcpy( pNewData, m_pData, m_iLength+1 );
				delete [] m_pData;
			}
			else pNewData[ 0 ] = 0;
			m_pData = pNewData;
		}
		else
		{
			if ( m_pData ) delete [] m_pData;
			m_pData = new char[ newLength ];
			m_pData[ 0 ] = 0;
		}
		m_iTotalLength = newLength;
	}	
}

//****if* uString/SetStr
// FUNCTION
//   Sets the string to the specified value, deletes any existing string held in this object.
// INPUTS
//   szString -- The data to hold in this string, must be null terminated
// SOURCE
void uString::SetStr( const char *szString )
//****
{
	if ( m_pData == szString ) return;

	if ( !szString ) 
	{
		Reset();
		return;
	}

	// limit of 100MB on strings, arbitrary limit
	int length = (int) strlen( szString );
	if ( length <= 0 || length > 100000000 ) 
	{
		//Reset();
		CheckSize( 1, false );
		if ( m_iTotalLength > 0 ) m_pData[ 0 ] = 0;
		m_iLength = 0;
		m_iNumChars = 0;
		return;
	}

	const char* szUTF8String = szString;
	if ( !IsUTF8(szString, (int*)&m_iNumChars) ) szUTF8String = ConvertToUTF8( szString, &length, (int*)&m_iNumChars );

	CheckSize( length, false );

	memcpy( m_pData, szUTF8String, length+1 );
	m_iLength = length;
	m_iCachedCharOffset = 0;
	m_iCachedByteOffset = 0;

	if ( szUTF8String != szString ) delete [] szUTF8String;
}

void uString::SetStrUTF8( const char *szString )
{
	if ( m_pData == szString ) return;

	if ( !szString ) 
	{
		Reset();
		return;
	}

	// limit of 100MB on strings, arbitrary limit
	int length;
	int numChars = CountUTF8Chars( szString, &length );
	if ( length <= 0 || length > 100000000 ) 
	{
		//Reset();
		CheckSize( 1, false );
		if ( m_iTotalLength > 0 ) m_pData[ 0 ] = 0;
		m_iLength = 0;
		m_iNumChars = 0;
		return;
	}

	CheckSize( length, false );

	memcpy( m_pData, szString, length+1 );
	m_iLength = length;
	m_iNumChars = numChars;
	m_iCachedCharOffset = 0;
	m_iCachedByteOffset = 0;
}

void uString::SetStr( const uString &sString )
{
	if ( m_pData == sString.m_pData ) return;

	if ( !sString.m_pData ) 
	{
		Reset();
		return;
	}

	CheckSize( sString.m_iLength, false );
	memcpy( m_pData, sString.m_pData, sString.m_iLength+1 );
	m_iLength = sString.m_iLength;
	m_iNumChars = sString.m_iNumChars;
	m_iCachedCharOffset = sString.m_iCachedCharOffset;
	m_iCachedByteOffset = sString.m_iCachedByteOffset;
}

// length is in bytes, not characters
void uString::SetStrN( const char* szString, unsigned int length )
{
	if ( m_pData == szString ) return;

	if ( !szString || !*szString || length == 0 ) 
	{
		Reset();
		return;
	}

	// limit of 100MB on strings, arbitrary limit
	//unsigned int oldLength = (unsigned int)strlen( szString );
	//if ( length > oldLength ) length = oldLength;
	if ( length <= 0 || length > 100000000 ) 
	{
		//Reset();
		CheckSize( 1, false );
		if ( m_iTotalLength > 0 ) m_pData[ 0 ] = 0;
		m_iLength = 0;
		return;
	}

	const char* szUTF8String = szString;
	int numChars = 0;
	int otherLength = length;
	if ( !IsUTF8(szString, &numChars, length) ) szUTF8String = ConvertToUTF8( szString, &otherLength, &numChars, length );

	CheckSize( otherLength, false );

	memcpy( m_pData, szUTF8String, otherLength );
	m_pData[ otherLength ] = 0;
	m_iLength = otherLength;
	m_iNumChars = numChars;
	m_iCachedCharOffset = 0;
	m_iCachedByteOffset = 0;

	if ( szUTF8String != szString ) delete [] szUTF8String;
}

void uString::ClearTemp()
{
	if ( m_iTotalLength > 0 ) m_pData[ 0 ] = 0;
	m_iLength = 0;
	m_iNumChars = 0;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;
}

//****if* uString/CompareTo
// FUNCTION
//   Compares a string to another string, returns 0 if they are equal, less than 0 if this string is less than the given string, greater than 0 if this string is greater than the given string.
// INPUTS
//   szTo -- The string to compare with, must be null terminated
// SOURCE
int uString::CompareTo( const char *szTo ) const
//****
{
	if ( !m_pData && !szTo ) return 0;
	if ( !m_pData ) return -1;
	if ( !szTo ) return 1;

	// if this string is all ascii then it doesn't matter what the other string is since it will fail to match on the first non-ascii char
	// or if they are both UTF8 then strcmp will work, m_pData is guaranteed to be UTF8
	if ( m_iNumChars == m_iLength || IsUTF8( szTo ) ) return strcmp( m_pData, szTo );

	// This string is UTF8 but the other one isn't
	char *szUTF8 = ConvertToUTF8( szTo, 0, 0 );
	int result = strcmp( m_pData, szUTF8 );
	delete [] szUTF8;
	return result;
}

int uString::CompareTo( const uString &pTo ) const
{
	if ( !m_pData && !pTo.m_pData ) return 0;
	if ( !m_pData ) return -1;
	if ( !pTo.m_pData ) return 1;

	// both are guaranteed to be UTF8
	return strcmp( m_pData, pTo.m_pData );
}

// max is in characters, not bytes
int uString::CompareToN( const char *szTo, int max ) const
{
	if ( !m_pData && !szTo ) return 0;
	if ( !m_pData ) return -1;
	if ( !szTo ) return 1;

	// if this string is all ascii then it doesn't matter what the other string is since it will fail to match on the first non-ascii char
	if ( m_iNumChars == m_iLength ) return strncmp( m_pData, szTo, max );

	if ( !IsUTF8( szTo ) ) 
	{
		// we can use the fact that we have to convert the other string to work out how many bytes there are
		int length = 0;
		char* szUTF8Str = ConvertToUTF8( szTo, &length, 0, max );
		int result = strncmp( m_pData, szUTF8Str, length );
		delete [] szUTF8Str;
		return result;
	}

	if ( max == (int)m_iNumChars ) return strncmp( m_pData, szTo, m_iLength );
	if ( max > (int)m_iNumChars ) return strncmp( m_pData, szTo, m_iLength+1 );

	// both UTF8 but we don't know how many bytes to check for the number of characters
	int byteOffset = GetByteOffsetConst( max ); // byte offset to the end of the max-1 character
	return strncmp( m_pData, szTo, byteOffset );
}

// max is in characters, not bytes
int uString::CompareToN( const uString &pTo, int max ) const
{
	if ( !m_pData && !pTo.m_pData ) return 0;
	if ( !m_pData ) return -1;
	if ( !pTo.m_pData ) return 1;

	// if this string is all ascii then it doesn't matter what the other string is since it will fail to match on the first non-ascii char
	if ( m_iNumChars == m_iLength ) return strncmp( m_pData, pTo.m_pData, max );

	if ( max == (int)m_iNumChars ) return strncmp( m_pData, pTo.m_pData, m_iLength );
	if ( max > (int)m_iNumChars ) return strncmp( m_pData, pTo.m_pData, m_iLength+1 );

	if ( max == (int)pTo.m_iNumChars ) return strncmp( m_pData, pTo.m_pData, pTo.m_iLength );
	if ( max > (int)pTo.m_iNumChars ) return strncmp( m_pData, pTo.m_pData, pTo.m_iLength+1 );

	// both UTF8 but we don't know how many bytes to check for the number of characters
	int byteOffset; // byte offset to the end of the max-1 character
	if ( abs(m_iCachedCharOffset-max) < abs(pTo.m_iCachedCharOffset-max) ) byteOffset = GetByteOffsetConst( max ); 
	else byteOffset = pTo.GetByteOffsetConst( max );

	return strncmp( m_pData, pTo.m_pData, byteOffset );
}

// case insensitive compare
int uString::CompareCaseTo( const char *szTo ) const
{
	if ( !m_pData && !szTo ) return 0;
	if ( !m_pData ) return -1;
	if ( !szTo ) return 1;

	if ( !*m_pData && !*szTo ) return 0;
	if ( !*m_pData ) return -1;
	if ( !*szTo ) return 1;
	
	// convert other string to UTF8
	const char *szUTF8 = szTo;
	if ( !IsUTF8( szTo ) ) szUTF8 = ConvertToUTF8( szTo, 0, 0 );
	
	// setup some variables
	unsigned int output1[ 3 ] = {0,0,0};
	unsigned int output2[ 3 ] = {0,0,0};
	int outputPtr1=0, outputPtr2=0;
	int outputSize1=0, outputSize2=0;
	int bytes1=0, bytes2=0;
	const char* ptr1 = m_pData;
	const char* ptr2 = szUTF8;
	int result = 0;

	// get the first character from each stream and prime the output buffers
	int c1 = DecodeUTF8Char( ptr1, &bytes1 ); ptr1 += bytes1;
	int c2 = DecodeUTF8Char( ptr2, &bytes2 ); ptr2 += bytes2;
	
	if ( c1 < 0 ) { result = -1; goto cleanup; } // invalid character
	if ( c2 < 0 ) { result = 1; goto cleanup; } // invalid character
	
	outputSize1 = UnicodeCaseFoldedChar( (unsigned int)c1, output1 );
	outputSize2 = UnicodeCaseFoldedChar( (unsigned int)c2, output2 );
    
	while( outputSize1 != 0 && outputSize2 != 0 )
    {
		c1 = output1[ outputPtr1 ];
		c2 = output2[ outputPtr2 ];

		if ( c1 < c2 ) { result = -1; goto cleanup; }
        else if ( c1 > c2 ) { result = 1; goto cleanup; }

		outputPtr1++;
		outputPtr2++;

		if ( outputPtr1 >= outputSize1 )
		{
			if ( !*ptr1 ) outputSize1 = 0;
			else
			{
				c1 = DecodeUTF8Char( ptr1, &bytes1 ); ptr1 += bytes1;
				if ( c1 < 0 ) { result = -1; goto cleanup; } // invalid character
				outputSize1 = UnicodeCaseFoldedChar( (unsigned int)c1, output1 );
				outputPtr1 = 0;
			}
		}
		
		if ( outputPtr2 >= outputSize2 )
		{
			if ( !*ptr2 ) outputSize2 = 0;
			else
			{
				c2 = DecodeUTF8Char( ptr2, &bytes2 ); ptr2 += bytes2;
				if ( c2 < 0 ) { result = 1; goto cleanup; } // invalid character
				outputSize2 = UnicodeCaseFoldedChar( (unsigned int)c2, output2 );
				outputPtr2 = 0;
			}
		}
    }
    
    if ( outputSize1 == 0 && outputSize2 == 0 ) result = 0;
	else if ( outputSize1 == 0 ) result = -1;
	else if ( outputSize2 == 0 ) result = 1;

cleanup:
	if ( szUTF8 != szTo ) delete [] szUTF8;
	return result;
}

int uString::CompareCaseTo( const uString &pTo ) const
{
	return CompareCaseTo( pTo.m_pData );
}

// case insensitive compare
int uString::CompareCaseToN( const char *szTo, int max ) const
{
	if ( max <= 0 ) return 0;
	if ( !m_pData && !szTo ) return 0;
	if ( !m_pData ) return -1;
	if ( !szTo ) return 1;
	
	// convert other string to UTF8
	const char *szUTF8 = szTo;
	if ( !IsUTF8( szTo, 0, 0x7FFFFFFF, max ) ) szUTF8 = ConvertToUTF8( szTo, 0, 0, max );
	
	// setup some variables
	unsigned int output1[ 3 ] = {0,0,0};
	unsigned int output2[ 3 ] = {0,0,0};
	int outputPtr1=0, outputPtr2=0;
	int outputSize1=0, outputSize2=0;
	int bytes1=0, bytes2=0;
	const char* ptr1 = m_pData;
	const char* ptr2 = szUTF8;
	int result = 0;
	int count1=0, count2=0;

	// get the first character from each stream and prime the output buffers
	int c1 = DecodeUTF8Char( ptr1, &bytes1 ); ptr1 += bytes1;
	int c2 = DecodeUTF8Char( ptr2, &bytes2 ); ptr2 += bytes2;
	
	if ( c1 < 0 ) { result = -1; goto cleanup; } // invalid character
	if ( c2 < 0 ) { result = 1; goto cleanup; } // invalid character
	
	outputSize1 = UnicodeCaseFoldedChar( (unsigned int)c1, output1 );
	outputSize2 = UnicodeCaseFoldedChar( (unsigned int)c2, output2 );
    
	while( outputSize1 != 0 && outputSize2 != 0 )
    {
		c1 = output1[ outputPtr1 ];
		c2 = output2[ outputPtr2 ];

		if ( c1 < c2 ) { result = -1; goto cleanup; }
        else if ( c1 > c2 ) { result = 1; goto cleanup; }

		outputPtr1++;
		outputPtr2++;

		if ( outputPtr1 >= outputSize1 )
		{
			count1++;
			if ( count1 >= max ) { result = 0; goto cleanup; }

			if ( !*ptr1 ) outputSize1 = 0;
			else
			{
				c1 = DecodeUTF8Char( ptr1, &bytes1 ); ptr1 += bytes1;
				if ( c1 < 0 ) { result = -1; goto cleanup; } // invalid character
				outputSize1 = UnicodeCaseFoldedChar( (unsigned int)c1, output1 );
				outputPtr1 = 0;
			}
		}
		
		if ( outputPtr2 >= outputSize2 )
		{
			count2++;
			if ( count2 >= max ) { result = 0; goto cleanup; }

			if ( !*ptr2 ) outputSize2 = 0;
			else
			{
				c2 = DecodeUTF8Char( ptr2, &bytes2 ); ptr2 += bytes2;
				if ( c2 < 0 ) { result = 1; goto cleanup; } // invalid character
				outputSize2 = UnicodeCaseFoldedChar( (unsigned int)c2, output2 );
				outputPtr2 = 0;
			}
		}
    }
    
    if ( outputSize1 == 0 && outputSize2 == 0 ) result = 0;
	else if ( outputSize1 == 0 ) result = -1;
	else if ( outputSize2 == 0 ) result = 1;

cleanup:
	if ( szUTF8 != szTo ) delete [] szUTF8;
	return result;
}

int uString::CompareCaseToN( const uString &pTo, int max ) const
{
	return CompareCaseToN( pTo.m_pData, max );
}

int uString::GetMatchLength( const char* szOther ) const
{
	if ( !m_pData && !szOther ) return 0;
	
	// convert other string to UTF8
	const char *szUTF8 = szOther;
	if ( !IsUTF8( szOther, 0 ) ) szUTF8 = ConvertToUTF8( szOther, 0, 0 );
	
	int count = 0;

	const char* ptr1 = m_pData;
	const char* ptr2 = szUTF8;

	while( *ptr1 && *ptr2 && *ptr1 == *ptr2 )
	{
		unsigned char c = (unsigned char) (*ptr1);
		if ( (c & 0xC0) != 0x80 ) count++;

		ptr1++;
		ptr2++;
	}

	if ( *ptr1 )
	{
		unsigned char c = (unsigned char) (*ptr1);
		if ( (c & 0xC0) == 0x80 ) count--;
	}

	if ( szUTF8 != szOther ) delete [] szUTF8;
	return count;
}

int uString::GetMatchCaseLength( const char* szOther ) const
{
	if ( !m_pData && !szOther ) return 0;
	
	// convert other string to UTF8
	const char *szUTF8 = szOther;
	if ( !IsUTF8( szOther, 0 ) ) szUTF8 = ConvertToUTF8( szOther, 0, 0 );
	
	// setup some variables
	unsigned int output1[ 3 ] = {0,0,0};
	unsigned int output2[ 3 ] = {0,0,0};
	int outputPtr1=0, outputPtr2=0;
	int outputSize1=0, outputSize2=0;
	int bytes1=0, bytes2=0;
	const char* ptr1 = m_pData;
	const char* ptr2 = szUTF8;
	int result = 0;
	int count1=0, count2=0;

	// get the first character from each stream and prime the output buffers
	int c1 = DecodeUTF8Char( ptr1, &bytes1 ); ptr1 += bytes1;
	int c2 = DecodeUTF8Char( ptr2, &bytes2 ); ptr2 += bytes2;
	
	if ( c1 < 0 || c2 < 0 ) goto cleanup; // invalid character
	
	outputSize1 = UnicodeCaseFoldedChar( (unsigned int)c1, output1 );
	outputSize2 = UnicodeCaseFoldedChar( (unsigned int)c2, output2 );
    
	while( outputSize1 != 0 && outputSize2 != 0 )
    {
		c1 = output1[ outputPtr1 ];
		c2 = output2[ outputPtr2 ];

		if ( c1 != c2 ) { result = count1; goto cleanup; }

		outputPtr1++;
		outputPtr2++;

		if ( outputPtr1 >= outputSize1 )
		{
			count1++;
			
			if ( !*ptr1 ) outputSize1 = 0;
			else
			{
				c1 = DecodeUTF8Char( ptr1, &bytes1 ); ptr1 += bytes1;
				if ( c1 < 0 ) { result = count1; goto cleanup; } // invalid character
				outputSize1 = UnicodeCaseFoldedChar( (unsigned int)c1, output1 );
				outputPtr1 = 0;
			}
		}
		
		if ( outputPtr2 >= outputSize2 )
		{
			count2++;

			if ( !*ptr2 ) outputSize2 = 0;
			else
			{
				c2 = DecodeUTF8Char( ptr2, &bytes2 ); ptr2 += bytes2;
				if ( c2 < 0 ) { result = count1; goto cleanup; } // invalid character
				outputSize2 = UnicodeCaseFoldedChar( (unsigned int)c2, output2 );
				outputPtr2 = 0;
			}
		}
    }

	result = count1;

cleanup:
	if ( szUTF8 != szOther ) delete [] szUTF8;
	return result;
}

int uString::SimplifyPath()
{
	Replace( '\\', '/' );

	int index = 0;
	while( (index = FindStr( "/../" )) >= 0 )
	{
		int index2 = RevFindStr( "/", 0, index-1 );
		if ( index < 0 ) return 0;
		uString sNewPath, sNewPath2;
		SubString( sNewPath, 0, index2 );
		SubString( sNewPath2, index+3 );
		SetStr( sNewPath );
		Append( sNewPath2 );
	}

	return 1;
}

//****if* uString/Prepend
// FUNCTION
//   Prefix a string to the beginning of this string
// INPUTS
//   szString -- The string to prepend, must be null terminated
// SOURCE
uString& uString::Prepend( const char *szString )
//****
{
	if ( !szString ) return *this;
	if ( szString == m_pData ) return *this;

	int oldLength = m_iLength;
	int length=0, numChars=0;
	const char* szUTF8 = szString;
	if ( !IsUTF8( szString, &numChars ) ) szUTF8 = ConvertToUTF8( szString, &length, &numChars );
	else length = (int) strlen( szString );

	unsigned int newLength = GetLength() + length;
	if ( newLength <= 0 || newLength > 100000000 ) 
	{
		if ( szString != szUTF8 ) delete [] szUTF8;
		return *this;
	}
	m_iLength = newLength;
	m_iNumChars = m_iNumChars + numChars;

	if ( m_iTotalLength > newLength ) newLength = m_iTotalLength;
	newLength++;
	char *szNewString = new char[ newLength ];

	memcpy( szNewString, szUTF8, length+1 );
	if ( szString != szUTF8 ) delete [] szUTF8;
	if ( m_pData ) 
	{
		memcpy( szNewString+length, m_pData, oldLength+1 );
		delete [] m_pData;
	}

	m_pData = szNewString;
	m_iTotalLength = newLength;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;
	return *this;
}

uString& uString::Prepend( const uString &sString )
{
	if ( !sString.m_pData ) return *this;
	
	int oldLength = m_iLength;
	unsigned int newLength = GetLength() + sString.GetLength();
	if ( newLength <= 0 || newLength > 100000000 ) return *this;
	m_iLength = newLength;
	m_iNumChars = m_iNumChars + sString.m_iNumChars;
	
	if ( m_iTotalLength > newLength ) newLength = m_iTotalLength;
	newLength++;
	char *szNewString = new char[ newLength ];

	memcpy( szNewString, sString.m_pData, sString.m_iLength+1 );
	if ( m_pData ) 
	{
		memcpy( szNewString+sString.m_iLength, m_pData, oldLength+1 );
		delete [] m_pData;
	}

	m_pData = szNewString;
	m_iTotalLength = newLength;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;
	return *this;
}

//****if* uString/Append
// FUNCTION
//   Append a string to the end of this string
// INPUTS
//   szString -- The string to append, must be null terminated
// SOURCE
uString& uString::Append( const char *szString )
//****
{
	if ( m_pData == szString ) return *this;
	if ( !szString ) return *this;

	int length=0, numChars=0;
	const char* szUTF8 = szString;
	if ( !IsUTF8( szString, &numChars ) ) szUTF8 = ConvertToUTF8( szString, &length, &numChars );
	else length = (int) strlen( szString );

	unsigned int newLength = GetLength() + length;
	if ( newLength <= 0 || newLength > 100000000 ) 
	{
		if ( szString != szUTF8 ) delete [] szUTF8;
		return *this;
	}
	
	// check if we have enough allocated memory for this string
	CheckSize( newLength, true );

	memcpy( m_pData+m_iLength, szUTF8, length+1 );
	
	m_iLength = newLength;
	m_iNumChars = m_iNumChars + numChars;

	if ( szString != szUTF8 ) delete [] szUTF8;
	return *this;
}

uString& uString::AppendUTF8( const char *szString )
{
	if ( !*szString ) return *this;

	int length = 0;
	int numChars = CountUTF8Chars( szString, &length );

	unsigned int newLength = GetLength() + length;
	if ( newLength <= 0 || newLength > 100000000 ) return *this;
	
	CheckSize( newLength, true );

	memcpy( m_pData+m_iLength, szString, length+1 );

	m_iLength = newLength;
	m_iNumChars = m_iNumChars + numChars;

	return *this;
}

uString& uString::AppendAscii( const char *szString )
{
	if ( !*szString ) return *this;

	int length = (int) strlen( szString );
	int numChars = length;

	unsigned int newLength = GetLength() + length;
	if ( newLength <= 0 || newLength > 100000000 ) return *this;
	
	CheckSize( newLength, true );

	memcpy( m_pData+m_iLength, szString, length+1 );

	m_iLength = newLength;
	m_iNumChars = m_iNumChars + numChars;

	return *this;
}

uString& uString::Append( const uString &sString )
{
	if ( m_pData == sString.m_pData ) return *this;
	if ( !sString.m_pData ) return *this;

	unsigned int newLength = GetLength() + sString.GetLength();
	if ( newLength <= 0 || newLength > 100000000 ) return *this;
	
	CheckSize( newLength, true );

	memcpy( m_pData+m_iLength, sString.m_pData, sString.m_iLength+1 );

	m_iLength = newLength;
	m_iNumChars = m_iNumChars + sString.m_iNumChars;

	return *this;
}

//****if* uString/AppendN
// FUNCTION
//   Append a given number of bytes from a string to the end of this string
// INPUTS
//   szString -- The string to append, must be null terminated
//   max -- The number of bytes to copy
// SOURCE
uString& uString::AppendN( const char *szString, unsigned int max )
//****
{
	if ( m_pData == szString ) return *this; 
	if ( max == 0 ) return *this;
	if ( !szString ) return *this;

	int length=max, numChars=0;
	const char* szUTF8 = szString;
	if ( !IsUTF8( szString, &numChars, (int)max ) ) szUTF8 = ConvertToUTF8( szString, &length, &numChars, (int)max );

	unsigned int newLength = GetLength() + length;
	if ( newLength <= 0 || newLength > 100000000 ) return *this;

	// check if we have enough allocated memory for this string
	CheckSize( newLength, true );

	strncat( m_pData+m_iLength, szUTF8, length );
	m_pData[ newLength ] = 0; //null terminator
	
	m_iLength = newLength;
	m_iNumChars = m_iNumChars + numChars;

	if ( szString != szUTF8 ) delete [] szUTF8;
	return *this;
}

//****if* uString/AppendUnicode
// FUNCTION
//   Append a single character to the end of this string, must be non-zero.
// INPUTS
//   c -- The character to append, in Unicode
// SOURCE
uString& uString::AppendUnicode( unsigned int c )
//****
{
	if ( c == 0 ) return *this;
	
	char szUTF8[ 4 ];
	int charLength = GetUTF8FromUnicode( c, szUTF8 );
	if ( charLength <= 0 ) return *this;
	
	unsigned int newLength = GetLength() + charLength;
	if ( newLength <= 0 || newLength > 100000000 ) return *this;

	// check if we have enough allocated memory for this string
	CheckSize( newLength, true );

	int i = charLength;
	while( i > 0 )
	{
		m_pData[ newLength - i ] = szUTF8[charLength-i];
		i--;
	}
	
	m_pData[ newLength ] = 0;
	
	m_iLength = newLength;
	m_iNumChars++;
	return *this;
}

//****if* uString/AppendAscii
// FUNCTION
//   Append a single character to the end of this string, must be non-zero.
// INPUTS
//   c -- The character to append, must be less than 128
// SOURCE
uString& uString::AppendAscii( int c )
//****
{
	if ( c <= 0 || c >= 128 ) return *this;
	
	unsigned int newLength = GetLength() + 1;
	if ( newLength <= 0 || newLength > 100000000 ) return *this;

	// check if we have enough allocated memory for this string
	CheckSize( newLength, true );

	m_pData[ newLength - 1 ] = (char)c;
	m_pData[ newLength ] = 0;
	
	m_iLength = newLength;
	m_iNumChars++;
	return *this;
}

//****if* uString/AppendByte
// FUNCTION
//   Append a single byte to the end of this string, must be non-zero.
// INPUTS
//   b -- The byte to append, assumes it is part of a valid UTF-8 string, and that you know what you are doing
// SOURCE
uString& uString::AppendByte( char b )
//****
{
	if ( b <= 0 ) return *this;
	
	unsigned int newLength = GetLength() + 1;
	if ( newLength <= 0 || newLength > 100000000 ) return *this;

	// check if we have enough allocated memory for this string
	CheckSize( newLength, true );

	m_pData[ newLength - 1 ] = b;
	m_pData[ newLength ] = 0;
	
	m_iLength = newLength;
	m_iNumChars++;
	return *this;
}


//****if* uString/AppendInt
// FUNCTION
//   Append an integer to the end of this string
// INPUTS
//   num -- The integer to append
// SOURCE
uString& uString::AppendInt( int num )
//****
{
	char szNum[20];
	sprintf( szNum, "%d", num );
	return AppendUTF8( szNum );
}

//****if* uString/Append
// FUNCTION
//   Append an float to the end of this string
// INPUTS
//   num -- The float to append
// SOURCE
uString& uString::AppendFloat( float num )
//****
{
	char szNum[20];
	sprintf( szNum, "%f", num );
	return AppendUTF8( szNum );
}

//****if* uString/Append
// FUNCTION
//   Append an unsigned integer to the end of this string
// INPUTS
//   num -- The integer to append
// SOURCE
uString& uString::AppendUInt( unsigned int num )
//****
{
	char szNum[20];
	sprintf( szNum, "%u", num );
	return AppendUTF8( szNum );
}

/*
// use unicode literals instead of char literals
uString& uString::InsertCharAt( unsigned int index, unsigned int c )
{
	if ( c == 0 ) return *this;
	if ( index >= m_iNumChars ) return AppendChar( c );

	char szUTF8[ 4 ];
	int charLength = GetUTF8FromChar( c, szUTF8 );
	if ( charLength <= 0 ) return *this;

	int unicode = DecodeUTF8Char( szUTF8, 0 );

	return InsertUnicodeAt( index, unicode );
}
*/

uString& uString::InsertUnicodeAt( unsigned int index, unsigned int c )
//****
{
	if ( c <= 0 ) return *this;
	if ( index >= m_iNumChars ) return AppendUnicode( c );

	char szUTF8[ 4 ];
	int charLength = GetUTF8FromUnicode( c, szUTF8 );
	if ( charLength <= 0 ) return *this;

	unsigned int newLength = GetLength() + charLength;
	if ( newLength <= 0 || newLength > 100000000 ) return *this;

	CheckSize( newLength, true );

	int charIndex = m_iNumChars+1;
	unsigned int i;
	for ( i = newLength; charIndex > (int)index; i-- )
	{
		char curr = m_pData[ i-charLength ];
		m_pData[ i ] = curr;
		if ( (curr & 0xC0) != 0x80 ) charIndex--;
	}

	i -= (charLength-1);

	m_iCachedByteOffset = i;
	m_iCachedCharOffset = index;

	for ( int j = 0; j < charLength; j++ )
	{
		m_pData[ i+j ] = szUTF8[ j ];
	}

	m_iLength = newLength;
	m_iNumChars++;
	return *this;
}

//****if* uString/DeleteCharAt
// FUNCTION
//   Deletes a character at the specified point in the string. 
// INPUTS
//   index -- The index of the character to delete.
// SOURCE
uString& uString::DeleteCharAt( unsigned int index )
//****
{
	if ( index >= m_iNumChars ) return *this;

	unsigned int byteOffset = GetByteOffset( index );
	unsigned char c = ((unsigned char*)m_pData)[byteOffset] >> 4;
	unsigned int charBytes;
	switch( c )
	{
		case 0xF: charBytes = 4; break;
		case 0xE: charBytes = 3; break;
		case 0xD: charBytes = 2; break;
		case 0xC: charBytes = 2; break;
		default: charBytes = 1;
	}
	
	for ( unsigned int i = byteOffset; i <= m_iLength-charBytes; i++ )
	{
		m_pData[ i ] = m_pData[ i+charBytes ];
	}

	m_iLength -= charBytes;
	m_iNumChars--;
	m_iCachedByteOffset = byteOffset; // have to reset this as there is no guaranatee that GetByteOffset will have cached it, might still point further down the string
	m_iCachedCharOffset = index;
	return *this;
}

//****if* uString/Format
// FUNCTION
//   Construct a string out of a format string with a list of variables to include.
// INPUTS
//   szString -- The format string, using %d for integers, %f for floats, %s for strings. Uses standard sprintf notation
//   ... -- any number of variables referenced in the format string
// SOURCE
uString& uString::Format( const char* szString, ... )
//****
{
	if ( !szString ) return *this;

	const char *szUTF8 = szString;
	if ( !IsUTF8(szString) ) szUTF8 = ConvertToUTF8( szString, 0, 0 );
    
	va_list argList;

	// get the size needed for the final string
	va_start(argList, szString);
	int newLength = vsnprintf( 0, 0, szUTF8, argList);
    va_end(argList);

	if ( newLength < 0 )
	{
		// don't know how much to allocate
		Reset();
		if ( szUTF8 != szString ) delete [] szUTF8;
		return *this;
	}

	// allocate new size if needed
	CheckSize( newLength, false );
	va_start(argList, szString);
	newLength = vsnprintf( m_pData, m_iTotalLength, szUTF8, argList);
	va_end(argList);

	if ( szUTF8 != szString ) delete [] szUTF8;

	if ( newLength < 0 )
	{
		// got the size but couldn't generate the final string
		Reset();
		return *this;
	}
	else 
	{
		// success
		m_iLength = newLength;
		m_iNumChars = CountUTF8Chars( m_pData, 0 );
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
	}

	return *this;
}

uString& uString::Strip( const char* tokens )
{
	if ( m_iLength == 0 ) return *this;
	char *str = m_pData;
	char *str2 = m_pData;
		
	if ( m_iLength == m_iNumChars )
	{
		// current string is all ascii, doesn't matter if tokens contains unicode chars
		do
		{
			while ( *str && strchr(tokens,*str) ) str++;
			if ( !*str ) break;
			*str2 = *str;
			str++;
			str2++;
		} while ( *str );
		
		*str2 = 0;
		m_iLength = (int)(str2 - m_pData);
		m_iNumChars = m_iLength;
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
	}
	else
	{
		// current string has unicode characters
		const char *szUTF8Tokens = tokens;
		int numTokenChars, tokenLength;
		if ( !IsUTF8(tokens, &numTokenChars) ) szUTF8Tokens = ConvertToUTF8( tokens, &tokenLength, &numTokenChars );

		// convert tokens string into an array of unicode characters
		int *tokenChars = new int[ numTokenChars+1 ];
		tokenChars[ numTokenChars ] = 0;
		int offset = 0;
		int numBytes = 0;
		for ( int i = 0; i < numTokenChars; i++ )
		{
			tokenChars[ i ] = DecodeUTF8Char( szUTF8Tokens+offset, &numBytes );
			offset += numBytes;
		}

		int numChars = 0;
		// repeat until the end of the string
		do
		{
			// skip over characters that match token characters
			numBytes = 0;
			int found = 0;
			do
			{
				str += numBytes;
				int c = DecodeUTF8Char( str, &numBytes );
				found = 0;
				for ( int i = 0; i < numTokenChars; i++ )
				{
					if ( tokenChars[ i ] == c ) { found = 1; break; }
				}
			} while ( found );

			if ( !*str ) break;

			// copy non-matching character
			*str2 = *str;
			str++; str2++;
			numChars++;

			// copy any continuation bytes
			while( (*str & 0xC0) == 0x80 )
			{
				*str2 = *str;
				str++; str2++;
			}
		} while (*str );

		*str2 = 0;
		m_iLength = (int)(str2 - m_pData);
		m_iNumChars = numChars;
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;

		delete [] tokenChars;
		if ( tokens != szUTF8Tokens ) delete [] szUTF8Tokens;
	}

	// now we have removed some characters, check for wasted space
	CheckSize( m_iLength, true );

	return *this;
}

uString& uString::Strip( unsigned int token )
{
	if ( m_iLength == 0 || token == 0 ) return *this;
	char *str = m_pData;
	char *str2 = m_pData;

	if ( m_iLength == m_iNumChars )
	{
		// current string is all ascii
		if ( token > 127 ) return *this; // any unicode characters will not be found

		do 
		{
			while ( *str == token ) str++;
			if ( !*str ) break;
			*str2 = *str;
			str++;
			str2++;
		} while ( *str );
		
		*str2 = 0;
		m_iLength = (int)(str2 - m_pData);
		m_iNumChars = m_iLength;
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
	}
	else
	{
		// current string has unicode characters
		/*
		char szUTF8[ 4 ];
		int charLength = GetUTF8FromChar( token, szUTF8 );
		if ( charLength <= 0 ) return *this;

		int iUTF8Token = DecodeUTF8Char( szUTF8, 0 );
		if ( iUTF8Token <= 0 ) return *this;
		*/
		int iUTF8Token = (int)token;

		int numChars = 0;
		// repeat until the end of the string
		do
		{
			// skip over characters that match token characters
			int numBytes = 0;
			int c;
			do
			{
				str += numBytes;
				c = DecodeUTF8Char( str, &numBytes );
			} while ( iUTF8Token == c );

			if ( !*str ) break;

			// copy non-matching character
			*str2 = *str;
			str++; str2++;
			numChars++;

			// copy any continuation bytes
			while( (*str & 0xC0) == 0x80 )
			{
				*str2 = *str;
				str++; str2++;
			}
		} while (*str );

		*str2 = 0;
		m_iLength = (int)(str2 - m_pData);
		m_iNumChars = numChars;
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
	}

	// now we have removed some characters, check for wasted space
	CheckSize( m_iLength, true );

	return *this;
}

uString& uString::StripUTF8()
{
	if ( m_iLength == 0 ) return *this;
	unsigned char *str = (unsigned char*)m_pData;
	unsigned char *str2 = (unsigned char*)m_pData;
	int count = 0;
    
	while ( *str )
    {
        if ( *str < 128 )
        {
            *str2 = *str;
            str2++;
            count++;
        }
        str++;
    }
    
    *str2 = 0;
    m_iLength = count;
	m_iNumChars = count;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;

	// now we have removed some characters, check for wasted space
	CheckSize( m_iLength, true );

    return *this;
}

uString& uString::Trim( const char* tokens )
{
	if ( m_iLength == 0 ) return *this;
	char *str = m_pData;
	char *str2 = m_pData;
		
	if ( m_iLength == m_iNumChars )
	{
		// current string is all ascii, doesn't matter if tokens contains unicode chars
		while ( *str && strchr(tokens,*str) ) str++;
		
		while ( *str ) 
		{
			*str2 = *str;
			str++; str2++;
		}
		
		if ( str2 != m_pData )
		{
			// remove characters from the end
			str2--;
			while( strchr(tokens,*str2) ) str2--;
			str2++;
		}

		*str2 = 0;
		
		m_iLength = (int)(str2 - m_pData);
		m_iNumChars = m_iLength;
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
	}
	else
	{
		// current string has unicode characters
		const char *szUTF8Tokens = tokens;
		int numTokenChars, tokenLength;
		if ( !IsUTF8(tokens, &numTokenChars) ) szUTF8Tokens = ConvertToUTF8( tokens, &tokenLength, &numTokenChars );

		// convert tokens string into an array of unicode characters
		int *tokenChars = new int[ numTokenChars+1 ];
		tokenChars[ numTokenChars ] = 0;
		int offset = 0;
		int numBytes = 0;
		for ( int i = 0; i < numTokenChars; i++ )
		{
			tokenChars[ i ] = DecodeUTF8Char( szUTF8Tokens+offset, &numBytes );
			offset += numBytes;
		}

		numBytes = 0;
		int numChars = m_iNumChars+1;
		// skip over characters that match token characters
		offset = 0;
		int found;
		do
		{
			numChars--;
			str += numBytes;
			int c = DecodeUTF8Char( str, &numBytes );
			found = 0;
			for ( int i = 0; i < numTokenChars; i++ )
			{
				if ( tokenChars[ i ] == c ) { found = 1; break; }
			}
		} while ( found );

		// copy non-matching character
		while ( *str ) 
		{
			*str2 = *str;
			str++; str2++;
		}

		if ( str2 != m_pData )
		{
			// remove characters from the end
			int found;
			do
			{
				str2--;
				while( (*str2 & 0xC0) == 0x80 ) str2--; // find the start of the character
				int c = DecodeUTF8Char( str2, &numBytes );
				found = 0;
				for ( int i = 0; i < numTokenChars; i++ )
				{
					if ( tokenChars[ i ] == c ) { found = 1; numChars--; break; }
				}
			} while ( found );

			str2 += numBytes;
		}

		*str2 = 0;
		m_iLength = (int)(str2 - m_pData);
		m_iNumChars = numChars;
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;

		delete [] tokenChars;
		if ( tokens != szUTF8Tokens ) delete [] szUTF8Tokens;
	}

	return *this;
}

uString& uString::Replace( unsigned int find, unsigned int replace )
{
	if ( m_iLength == 0 ) return *this;
	if ( find == 0 ) return *this;

	if ( m_iLength == m_iNumChars )
	{
		if ( find >= 128 ) return *this; // if find is non-ascii it will never be found

		if ( replace < 128 ) // can only take this shortcut if replace is also ascii
		{
			// string is all ascii	
			if ( find >= 128 ) return *this; // if find is non-ascii it will never be found

			char *str = m_pData;
			while ( *str )
			{
				if ( *str == find ) *str = replace;
				str++;
			}

			if ( replace == 0 ) m_iLength = (unsigned int)strlen( m_pData );
			m_iNumChars = m_iLength;
			return *this;
		}
	}

	
	// convert find into unicode value
	/*
	char szUTF8Find[ 4 ];
	int findLength = GetUTF8FromChar( find, szUTF8Find );
	if ( findLength <= 0 ) return *this;

	int iUTF8Find = DecodeUTF8Char( szUTF8Find, 0 );
	if ( iUTF8Find <= 0 ) return *this;
	*/
	int iUTF8Find = (int)find;
	int findLength = 1;
	if ( iUTF8Find > 0xFFFF ) findLength = 4;
	else if ( iUTF8Find > 0x7FF ) findLength = 3;
	else if ( iUTF8Find > 0x7F ) findLength = 2;

	// convert replace into unicode value
	char szUTF8Replace[ 4 ];
	int replaceLength = GetUTF8FromUnicode( replace, szUTF8Replace );
	if ( replaceLength <= 0 ) return *this;

	if ( findLength >= replaceLength )
	{
		// string length will be the same or smaller
		int numBytes = 0;
		char *str = m_pData;
		char *str2 = m_pData;
		while ( *str )
		{
			int c = DecodeUTF8Char( str, &numBytes );
			if ( c == iUTF8Find ) 
			{
				for ( int i = 0; i < replaceLength; i++ ) 
				{
					*str2 = szUTF8Replace[ i ];
					str2++;
				}
				str += numBytes;
			}
			else 
			{
				if ( findLength == replaceLength ) 
				{
					str2 += numBytes;
					str += numBytes;
				}
				else
				{
					for ( int i = 0; i < numBytes; i++ ) 
					{
						*str2 = *str;
						str2++;
						str++;
					}
				}
			}
		}

		*str2 = 0;

		// if replace is a null terminator then recalculate everything
		if ( replace == 0 ) 
		{
			m_iNumChars = CountUTF8Chars( m_pData, (int*)&m_iLength );
			m_iCachedByteOffset = 0;
			m_iCachedCharOffset = 0;
		}
		else if ( findLength > replaceLength )
		{
			// number of characters doesn't change
			m_iLength = (int)(str2 - m_pData);
			m_iCachedByteOffset = 0;
			m_iCachedCharOffset = 0;
		}
	}
	else 
	{
		// string will increase in length
		int newTotalLength = m_iLength + m_iLength/2;
		char *szNew = new char[ newTotalLength ];
		char *str2 = szNew;
		char *str = m_pData;
		int numBytes = 0;
		
		while ( *str )
		{
			int c = DecodeUTF8Char( str, &numBytes );
			if ( c == iUTF8Find ) 
			{
				for ( int i = 0; i < replaceLength; i++ ) 
				{
					*str2 = szUTF8Replace[ i ];
					str2++;
				}
				str += numBytes;
			}
			else 
			{
				for ( int i = 0; i < numBytes; i++ ) 
				{
					*str2 = *str;
					str2++;
					str++;
				}
			}

			int currLength = (int)(str2-szNew);
			if ( newTotalLength < currLength + 5 ) // 4 potential new bytes plus null terminator
			{
				int newLength = newTotalLength + newTotalLength/2;
				char *szNew2 = new char[ newLength ];
				memcpy( szNew2, szNew, currLength );
				newTotalLength = newLength;
				delete [] szNew;
				szNew = szNew2;
				str2 = szNew + currLength;
			}
		}

		*str2 = 0;

		// replace can't be equal to 0 here, so only one case to account for
		m_iLength = (int)(str2 - szNew);
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;

		delete [] m_pData;
		m_pData = szNew;
		m_iTotalLength = newTotalLength;
	}

	return *this;
}

uString& uString::ReplaceStr( const char* find, const char *replace, int qty )
{
	if ( qty < 0 ) qty = 0x7FFFFFFF;
	if ( m_iLength == 0 ) return *this;
	if ( find == 0 || *find == 0 ) return *this;
	if ( replace == 0 ) return *this;

	// convert find to UTF8
	const unsigned char *szUTF8Find = (const unsigned char*)find;
	int numCharsFind, lengthFind;
	if ( !IsUTF8( find ) ) szUTF8Find = (const unsigned char*)ConvertToUTF8( find, &lengthFind, &numCharsFind );
	else numCharsFind = CountUTF8Chars( find, &lengthFind );

	if ( lengthFind > (int)m_iLength ) 
	{
		if ( szUTF8Find != (const unsigned char*)find ) delete [] szUTF8Find;
		return *this;
	}

	if ( lengthFind == 0 ) return *this;

	int iShiftArray[ 256 ];
	for ( int i = 0; i < 256; i++ ) 
	{
		iShiftArray[ i ] = lengthFind;
	}
	for ( int i = 0; i < lengthFind-1; i++ )
	{
		iShiftArray[ szUTF8Find[i] ] = lengthFind-1 - i;
	}
	
	// convert replace to UTF8
	const unsigned char *szUTF8Replace = (const unsigned char*)replace;
	int numCharsReplace, lengthReplace;
	if ( !IsUTF8( replace ) ) szUTF8Replace = (const unsigned char*)ConvertToUTF8( replace, &lengthReplace, &numCharsReplace );
	else numCharsReplace = CountUTF8Chars( replace, &lengthReplace );

	int replaceCount = 0;
	if ( lengthFind >= lengthReplace )
	{
		// string length will be the same or smaller
		unsigned char *str = (unsigned char*)m_pData;
		unsigned char *str2 = (unsigned char*)m_pData;
		while ( *str && replaceCount < qty )
		{
			// compare from the end of the string to the beginning
			int index = lengthFind-1;
			while( index >= 0 && str[index] == szUTF8Find[index] ) index--;
			if ( index == -1 ) 
			{
				// matched
				for ( int i = 0; i < lengthReplace; i++ ) 
				{
					*str2 = szUTF8Replace[ i ];
					str2++;
				}
				str += lengthFind;
				replaceCount++;
			}
			else 
			{
				// didn't match
				int shift = iShiftArray[ str[lengthFind-1] ];
				int shift2 = iShiftArray[ str[index] ] - (lengthFind-1-index);
				if ( shift2 > shift ) shift = shift2;

				if ( (char*)str+shift+lengthFind > m_pData+m_iLength ) shift = m_iLength - (int)((char*)str-m_pData);

				if (shift <= 0) shift = 1;

				if ( lengthFind == lengthReplace ) 
				{
					str2 += shift;
					str += shift;
				}
				else
				{
					for ( int i = 0; i < shift; i++ ) 
					{
						*str2 = *str;
						str2++;
						str++;
					}
				}
			}
		}

		if ( lengthFind > lengthReplace )
		{
			while( *str )
			{
				*str2 = *str;
				str2++;
				str++;
			}

			*str2 = 0;
			m_iLength = (int)((char*)str2 - m_pData);
		}
				
		if ( numCharsFind != numCharsReplace )
		{
			int diff = numCharsReplace - numCharsFind;
			m_iNumChars += diff*replaceCount;
		}
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
	}
	else 
	{
		// string will increase in length
		int newTotalLength = m_iLength + m_iLength/2;
		if ( newTotalLength <= (int)m_iLength+lengthReplace ) newTotalLength = m_iLength + lengthReplace + 1;
		char *szNew = new char[ newTotalLength ];
		unsigned char *str2 = (unsigned char*)szNew;
		unsigned char *str = (unsigned char*)m_pData;

		while ( *str && replaceCount < qty )
		{
			// compare from the end of the string to the beginning
			int index = lengthFind-1;
			while( index >= 0 && str[index] == szUTF8Find[index] ) index--;
			if ( index == -1 ) 
			{
				// matched
				for ( int i = 0; i < lengthReplace; i++ ) 
				{
					*str2 = szUTF8Replace[ i ];
					str2++;
				}
				str += lengthFind;
				replaceCount++;
			}
			else 
			{
				// didn't match
				int shift = iShiftArray[ str[lengthFind-1] ];
				int shift2 = iShiftArray[ str[index] ] - (lengthFind-1-index);
				if ( shift2 > shift ) shift = shift2;

				if ( (char*)str+shift+lengthFind > m_pData+m_iLength ) shift = m_iLength - (int)((char*)str-m_pData);

				for ( int i = 0; i < shift; i++ ) 
				{
					*str2 = *str;
					str2++;
					str++;
				}
			}

			int currLength = (int)((char*)str2-szNew);
			if ( newTotalLength < currLength + lengthReplace )
			{
				int newLength = newTotalLength + newTotalLength/2;
				if ( newLength <= newTotalLength+lengthReplace ) newLength = newTotalLength + lengthReplace + 1;
				char *szNew2 = new char[ newLength ];
				memcpy( szNew2, szNew, currLength );
				newTotalLength = newLength;
				delete [] szNew;
				szNew = szNew2;
				str2 = (unsigned char*)(szNew + currLength);
			}
		}

		if ( *str )
		{
			int currLength = (int)((char*)str2-szNew);
			int remaining = m_iLength - (int)((char*)str-m_pData) + 1;
			if ( newTotalLength < currLength + remaining )
			{
				int newLength = currLength + remaining;
				char *szNew2 = new char[ newLength ];
				memcpy( szNew2, szNew, currLength );
				newTotalLength = newLength;
				delete [] szNew;
				szNew = szNew2;
				str2 = (unsigned char*)(szNew + currLength);
			}

			while( *str )
			{
				*str2 = *str;
				str2++;
				str++;
			}
		}

		*str2 = 0;

		if ( numCharsFind != numCharsReplace )
		{
			int diff = numCharsReplace - numCharsFind;
			m_iNumChars += diff*replaceCount;
		}
		m_iLength = (int)((char*)str2 - szNew);
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
				
		delete [] m_pData;
		m_pData = szNew;
		m_iTotalLength = newTotalLength;
	}

	if ( szUTF8Find != (const unsigned char*)find ) delete [] szUTF8Find;
	if ( szUTF8Replace != (const unsigned char*)replace ) delete [] szUTF8Replace;

	return *this;
}

uString& uString::Upper( )
{
	if ( m_iLength == 0 ) return *this;

	if ( m_iNumChars == m_iLength )
	{
		// all ascii
		for ( unsigned int i = 0; i < m_iLength; i++ )
		{
			if ( m_pData[ i ] >= 97 && m_pData[ i ] <= 122 ) m_pData[ i ] = m_pData[ i ] - 32;
		}
	}
	else
	{
		char *str = m_pData;

		int newTotalLength = m_iLength + m_iLength/4;
		if ( newTotalLength < (int)m_iLength+7 ) newTotalLength = m_iLength + 7;
		char *szOutput = new char[ newTotalLength ]; // make some room for extra characters
		char *str2 = szOutput;
		int iUpperChars[ 4 ];
		
		int numBytes;
		int numChars = 0;
		while( *str )
		{
			// check we have enough room for a new replacement, max possible is 7 bytes (3 chars of 2 bytes, plus null terminator)
			int currLength = (int)(str2 - szOutput);
			if ( currLength+7 > newTotalLength )
			{
				int newSize = newTotalLength + newTotalLength/4;
				if ( newSize < newTotalLength+7 ) newSize = newTotalLength + 7;
				char *szOutput2 = new char[ newSize ];
				memcpy( szOutput2, szOutput, newTotalLength );
				delete [] szOutput;
				szOutput = szOutput2;
				newTotalLength = newSize;
				str2 = szOutput + currLength;
			}

			int c = DecodeUTF8Char( str, &numBytes );
			str += numBytes;
			if ( c < 0 ) continue;
			
			int numReplacements = UnicodeUpperCaseChar( c, iUpperChars );
			for ( int i = 0; i < numReplacements; i++ )
			{
				numBytes = GetUTF8FromUnicode( iUpperChars[i], str2 );
				str2 += numBytes;
			}

			numChars += numReplacements;
		}

		*str2 = 0;

		m_iNumChars = numChars;
		m_iLength = (int)(str2-szOutput);
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;

		delete [] m_pData;
		m_pData = szOutput;
		m_iTotalLength = newTotalLength;
	}

	return *this;
}

uString& uString::Lower( )
{
	if ( m_iNumChars == m_iLength )
	{
		// all ascii
		for ( unsigned int i = 0; i < m_iLength; i++ )
		{
			if ( m_pData[ i ] >= 65 && m_pData[ i ] <= 90 ) m_pData[ i ] = m_pData[ i ] + 32;
		}
	}
	else
	{
		char *str = m_pData;

		int newTotalLength = m_iLength + m_iLength/4;
		if ( newTotalLength < (int)m_iLength+5 ) newTotalLength = m_iLength + 5;
		char *szOutput = new char[ newTotalLength ]; // make some room for extra characters
		char *str2 = szOutput;
		
		int numBytes;
		int numChars = 0;
		while( *str )
		{
			// check we have enough room for a new replacement, max possible is 5 bytes (1 char of 4 bytes, plus null terminator)
			int currLength = (int)(str2 - szOutput);
			if ( currLength+5 > newTotalLength )
			{
				int newSize = newTotalLength + newTotalLength/4;
				if ( newSize < newTotalLength+5 ) newSize = newTotalLength + 5;
				char *szOutput2 = new char[ newSize ];
				memcpy( szOutput2, szOutput, newTotalLength );
				delete [] szOutput;
				szOutput = szOutput2;
				newTotalLength = newSize;
				str2 = szOutput + currLength;
			}

			int c = DecodeUTF8Char( str, &numBytes );
			str += numBytes;
			if ( c < 0 ) continue;
			
			int replacement = UnicodeLowerCaseChar( c );
			numBytes = GetUTF8FromUnicode( replacement, str2 );
			str2 += numBytes;
			numChars++;
		}

		*str2 = 0;

		m_iNumChars = numChars;
		m_iLength = (int)(str2-szOutput);
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;

		delete [] m_pData;
		m_pData = szOutput;
		m_iTotalLength = newTotalLength;
	}

	return *this;
}

uString& uString::Unescape()
{
	if ( m_iLength == 0 ) return *this;
	// string is guaranteed to be smaller than it was before, so we can do it in place
	char *str = m_pData;
	char *str2 = m_pData;
	
	int numChars = 0;
	do
	{
		if ( *str == '\\' )
		{
			str++;
			switch( *str )
			{
				case 'n': *str2 = '\n'; break;
				case 'r': *str2 = '\r'; break;
				case '"': *str2 = '"'; break;
				case 'b': *str2 = '\b'; break;
				case 'f': *str2 = '\f'; break;
				case 't': *str2 = '\t'; break;
				case '/': *str2 = '/'; break;
				case 'u': 
				{
					str++;
					if ( str[0] == 0 || str[1] == 0 || str[2] == 0 || str[3] == 0  ) break;
					uint32_t c1 = DecodeHexChar( (uint8_t)str[0] );
					uint32_t c2 = DecodeHexChar( (uint8_t)str[1] );
					uint32_t c3 = DecodeHexChar( (uint8_t)str[2] );
					uint32_t c4 = DecodeHexChar( (uint8_t)str[3] );
					str += 3; // extra 1 will be added later
					if ( c1 != 0xFF && c2 != 0xFF && c3 != 0xFF && c4 != 0xFF ) 
					{
						uint32_t unicode = (c1 << 12) | (c2 << 8) | (c3 << 4) | c4;
						int count = GetUTF8FromUnicode( unicode, str2 ) - 1; // extra 1 will be added later
						if ( count > 0 ) str2 += count;
					}
				} break;
				case '\\': *str2 = '\\'; break;
				default: *str2 = *str;
			}
			if ( *str == 0 ) break;
		}
		else 
		{
			*str2 = *str;
		}
		str++;
		str2++;
		numChars++;

		// copy any continuation bytes
		while( (*str & 0xC0) == 0x80 )
		{
			*str2 = *str;
			str++; str2++;
		}
	} while ( *str );
		
	*str2 = 0;
	m_iLength = (int)(str2 - m_pData);
	m_iNumChars = numChars;
	m_iCachedByteOffset = 0;
	m_iCachedCharOffset = 0;

	// now we have removed some characters, check for wasted space
	CheckSize( m_iLength, true );

	return *this;
}

int uString::ByteAt( unsigned int index )
//****
{
	if ( index >= m_iLength ) return 0;
	if ( !m_pData ) return 0;
	return m_pData[ index ];
}

//****if* uString/CharAt
// FUNCTION
//   Return a character at a specific index in the string
// INPUTS
//   index -- The character to return, the first character is index 0.
// SOURCE
int uString::CharAt( unsigned int index )
//****
{
	if ( index >= m_iLength ) return 0;
	if ( !m_pData ) return 0;
	if ( m_iNumChars == m_iLength ) return ((unsigned char*)m_pData)[ index ];
	else
	{
		int byteOffset = GetByteOffset( index );
		int result = DecodeUTF8Char( m_pData+byteOffset, 0 );
		if ( result < 0 ) result = 0xFFFD; // missing character symbol
		return result;
	}
}

int uString::CharAtConst( unsigned int index ) const
{
	if ( index >= m_iLength ) return 0;
	if ( !m_pData ) return 0;
	if ( m_iNumChars == m_iLength ) return ((unsigned char*)m_pData)[ index ];
	else
	{
		int byteOffset = GetByteOffsetConst( index );
		int result = DecodeUTF8Char( m_pData+byteOffset, 0 );
		if ( result < 0 ) result = 0xFFFD; // missing character symbol
		return result;
	}
}

//****if* uString/Find
// FUNCTION
//   Find a character within this string, returns the index of the character starting from 0, or -1 if not found.
// INPUTS
//   cFind -- The character to find
// SOURCE
int uString::Find( int cFind ) const
//****
{
	if ( m_iNumChars == m_iLength )
	{
		const char *szFound = strchr( GetStr(), cFind );
		if ( !szFound ) return -1;

		int index = (int)(szFound - GetStr());
		return index;
	}
	else
	{
		// convert find into unicode value
		/*
		char szUTF8Find[ 4 ];
		int findLength = GetUTF8FromChar( cFind, szUTF8Find );
		if ( findLength <= 0 ) return -1;

		int iUTF8Find = DecodeUTF8Char( szUTF8Find, 0 );
		if ( iUTF8Find <= 0 ) return -1;
		*/
		int iUTF8Find = cFind;

		int numBytes = 0;
		int index = 0;
		char *str = m_pData;
		while ( *str )
		{
			int c = DecodeUTF8Char( str, &numBytes );
			if ( c == iUTF8Find ) 
			{
				return index;
			}
			str += numBytes;
			index++;
		}

		return -1;
	}
}

//****if* uString/RevFind
// FUNCTION
//   Find a character within this string starting from the end of the string and searching backward, returns the index of the character starting from 0, or -1 if not found.
// INPUTS
//   cFind -- The character to find
// SOURCE
int uString::RevFind( int cFind ) const
//****
{
	if ( m_iLength == 0 || !m_pData ) return -1;

	if ( m_iNumChars == m_iLength )
	{
		if ( cFind >= 128 ) return -1;
		char *str = m_pData + (m_iLength-1);
		int pos = m_iLength-1;
		while ( str != m_pData )
		{
			if ( *str == cFind ) return pos;

			pos--;
			str--;
		}

		if ( *str == cFind ) return 0;
		return -1;
	}
	else
	{
		// convert find into unicode value
		/*
		char szUTF8Find[ 4 ];
		int findLength = GetUTF8FromChar( cFind, szUTF8Find );
		if ( findLength <= 0 ) return -1;

		int iUTF8Find = DecodeUTF8Char( szUTF8Find, 0 );
		if ( iUTF8Find <= 0 ) return -1;
		*/
		int iUTF8Find = cFind;

		int index = m_iNumChars - 1;
		char *str = m_pData + m_iLength - 1;
		while( (*str & 0xC0) == 0x80 ) str--;

		while ( str >= m_pData )
		{
			int c = DecodeUTF8Char( str, 0 );
			if ( c == iUTF8Find ) 
			{
				return index;
			}
			index--;
			str--;
			while( (*str & 0xC0) == 0x80 && str > m_pData ) str--;
		}

		return -1;
	}
}

//****if* uString/Trunc
// FUNCTION
//   Searches a string for the given character from the end of the string to the beginning and, if found, cuts off everything from that character onwards, inclusive.
// INPUTS
//   cFind -- The character to find
// SOURCE
void uString::Trunc( int cFind )
//****
{
	if ( m_iLength == 0 || !m_pData ) return;

	/*
	char szUTF8Find[ 4 ];
	int findLength = GetUTF8FromChar( cFind, szUTF8Find );
	if ( findLength <= 0 ) return;

	int iUTF8Find = DecodeUTF8Char( szUTF8Find, 0 );
	if ( iUTF8Find <= 0 ) return;
	*/
	int iUTF8Find = cFind;

	int index = m_iNumChars - 1;
	char *str = m_pData + m_iLength - 1;
	while( (*str & 0xC0) == 0x80 ) str--;

	while ( str >= m_pData )
	{
		int c = DecodeUTF8Char( str, 0 );
		if ( c == iUTF8Find ) 
		{
			*str = 0;
			m_iLength = (int)(str - m_pData);
			m_iNumChars = index;
			m_iCachedByteOffset = 0;
			m_iCachedCharOffset = 0;
			return;
		}
		index--;
		str--;
		while( (*str & 0xC0) == 0x80 && str > m_pData ) str--;
	}
}

//****if* uString/Trunc2
// FUNCTION
//   Removes the last count characters from the string
// INPUTS
//   count -- The number of characters to remove
// SOURCE
void uString::Trunc2( int count )
//****
{
	if ( count <= 0 || m_iLength <= 0 || !m_pData ) return;

	if ( count >= (int) m_iNumChars )
	{
		*m_pData = 0;
		m_iLength = 0;
	}
	else
	{
		if ( m_iNumChars == m_iLength )
		{
			int remaining = m_iLength - count;
			*(m_pData+remaining) = 0;
			m_iLength = remaining;
			m_iNumChars = m_iLength;
			m_iCachedByteOffset = 0;
			m_iCachedCharOffset = 0;
		}
		else
		{
			int stop = m_iNumChars - count;
			int index = m_iNumChars - 1;
			char *str = m_pData + m_iLength - 1;
			while( (*str & 0xC0) == 0x80 ) str--;

			while ( index > stop && str > m_pData )
			{
				index--;
				str--;
				while( (*str & 0xC0) == 0x80 && str > m_pData ) str--;
			}

			*str = 0;
			m_iLength = (int)(str-m_pData);
			m_iNumChars = index;
			m_iCachedByteOffset = 0;
			m_iCachedCharOffset = 0;
		}
	}
}

void uString::Keep( int count )
//****
{
	if ( m_iLength <= 0 || !m_pData ) return;
	if ( count <= 0 ) return Reset(); // delete everything
	if ( count >= (int)m_iNumChars ) return; // delete nothing

	if ( m_iNumChars == m_iLength )
	{
		*(m_pData+count) = 0;
		m_iLength = count;
		m_iNumChars = m_iLength;
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
	}
	else
	{
		int index = 0;
		char *str = m_pData;
		while ( index < count && *str )
		{
			index++;
			str++;
			while( (*str & 0xC0) == 0x80 ) str++;
		}

		*str = 0;
		m_iLength = (uint32_t)(str - m_pData);
		m_iNumChars = index;
		m_iCachedByteOffset = 0;
		m_iCachedCharOffset = 0;
	}
}

// start is in characters
int uString::FindStrCount( const char* find, int ignorecase, int start )
{
	if ( !m_pData || m_iLength == 0 ) return 0;
	if ( !find || !*find ) return 0;
	if ( start < 0 ) start = 0;
	if ( start >= (int)m_iNumChars ) return 0;

	// convert find to UTF8
	const char *szUTF8Find = find;
	int numCharsFind, lengthFind;
	if ( !IsUTF8( find ) ) szUTF8Find = ConvertToUTF8( find, &lengthFind, &numCharsFind );
	else numCharsFind = CountUTF8Chars( find, &lengthFind );

	int byteOffset = GetByteOffset( start );
	
	char* szData = m_pData + byteOffset;
	int lengthData = m_iLength - byteOffset;
	int numCharsData = m_iNumChars - start;
	if ( ignorecase )
	{
		// case fold local string
		int newTotalLength = (m_iLength-byteOffset) + (m_iLength-byteOffset)/4;
		szData = new char[ newTotalLength ];
		
		const char *str = m_pData + byteOffset;
		char *str2 = szData;
		int numBytes = 0;
		numCharsData = 0;
		while( *str )
		{
			int c = DecodeUTF8Char( str, &numBytes );
			str += numBytes;
			if ( c < 0 ) continue;

			unsigned int iReplacements[ 3 ];
			int numReplacements = UnicodeCaseFoldedChar( c, iReplacements );

			// check we have enough room in the new string for the replacements
			int currLength = (int)(str2 - szData);
			if ( currLength+7 > newTotalLength )
			{
				int newSize = newTotalLength + newTotalLength/4;
				if ( newSize < newTotalLength+7 ) newSize = newTotalLength + 7;
				char *szOutput2 = new char[ newSize ];
				memcpy( szOutput2, szData, newTotalLength );
				delete [] szData;
				szData = szOutput2;
				newTotalLength = newSize;
				str2 = szData + currLength;
			}

			for ( int i = 0; i < numReplacements; i++ )
			{
				numBytes = GetUTF8FromUnicode( iReplacements[i], str2 );
				str2 += numBytes;
				numCharsData++;
			}
		}

		*str2 = 0;
		lengthData = (int)(str2-szData);

		// case fold string to find
		newTotalLength = lengthFind + lengthFind/4;
		char *szOutput = new char[ newTotalLength ];

		str = szUTF8Find;
		str2 = szOutput;
		numBytes = 0;
		numCharsFind = 0;
		while( *str )
		{
			int c = DecodeUTF8Char( str, &numBytes );
			str += numBytes;
			if ( c < 0 ) continue;

			unsigned int iReplacements[ 3 ];
			int numReplacements = UnicodeCaseFoldedChar( c, iReplacements );

			// check we have enough room in the new string for the replacements
			int currLength = (int)(str2 - szOutput);
			if ( currLength+7 > newTotalLength )
			{
				int newSize = newTotalLength + newTotalLength/4;
				if ( newSize < newTotalLength+7 ) newSize = newTotalLength + 7;
				char *szOutput2 = new char[ newSize ];
				memcpy( szOutput2, szOutput, newTotalLength );
				delete [] szOutput;
				szOutput = szOutput2;
				newTotalLength = newSize;
				str2 = szOutput + currLength;
			}

			for ( int i = 0; i < numReplacements; i++ )
			{
				numBytes = GetUTF8FromUnicode( iReplacements[i], str2 );
				str2 += numBytes;
				numCharsFind++;
			}
		}

		*str2 = 0;
		lengthFind = (int)(str2-szOutput);

		if ( szUTF8Find != find ) delete [] szUTF8Find;
		szUTF8Find = szOutput;
	}
	
	// start the comparison
	if ( lengthFind > (int)lengthData ) 
	{
		if ( szUTF8Find != find ) delete [] szUTF8Find;
		if ( szData != m_pData+byteOffset ) delete [] szData;
		return 0;
	}

	// build shift array so we know later how much of the string we can skip
	int iShiftArray[ 256 ];
	for ( int i = 0; i < 256; i++ ) 
	{
		iShiftArray[ i ] = lengthFind;
	}
	for ( int i = 0; i < lengthFind-1; i++ )
	{
		iShiftArray[ ((const unsigned char*)szUTF8Find)[i] ] = lengthFind-1 - i;
	}
	
	int count = 0;
	unsigned char *str = (unsigned char*)szData;
	while ( *str )
	{
		// compare from the end of the string to the beginning
		int index = lengthFind - 1;
		while( index >= 0 && str[index] == ((unsigned char*)szUTF8Find)[index] ) index--;

		// prepare the next comparison point by skipping as many characters as possible
		int shift = iShiftArray[ str[lengthFind-1] ];
		if ( index == -1 )
		{
			// matched
			count++;
		}
		else
		{
			// didn't match, use non-matching character to try and improve skip amount
			int shift2 = iShiftArray[ str[index] ] - (lengthFind-1-index);
			if ( shift2 > shift ) shift = shift2;
		}

		str += shift;
		
		// check the skip amount hasn't gone past the end of the string
		if ( (char*)str+lengthFind > szData+lengthData ) break;
	}
	
	if ( szUTF8Find != find ) delete [] szUTF8Find;
	if ( szData != m_pData+byteOffset ) delete [] szData;

	return count;
}

//****if* uString/FindStr
// FUNCTION
//   Searches for the given string from the beginning of the string to the end returns the index of the first character of the first occurance found.
//   returns -1 if not found.
// INPUTS
//   str -- The string to find
// SOURCE
int uString::FindStr( const char* find, int ignorecase, int start )
//****
{
	if ( !m_pData || m_iLength == 0 ) return -1;
	if ( !find || !*find ) return -1;
	if ( start < 0 ) start = 0;
	if ( start >= (int)m_iNumChars ) return -1;

	int byteOffset = GetByteOffset( start );
	
	if ( ignorecase )
	{
		uString sFind( find );

		// have to check every character as case folding means that the string may not be the same size as the original
		while( byteOffset < (int)m_iLength && sFind.CompareCaseToN( m_pData+byteOffset, sFind.GetNumChars() ) != 0 )
		{
			byteOffset++;
			start++;
			while( (m_pData[byteOffset] & 0xC0) == 0x80 ) byteOffset++;
		}

		if ( byteOffset < (int)m_iLength ) return start;
		else return -1;
	}
	else
	{	
		// can use shortcuts to avoid checking every character
		char* szData = m_pData + byteOffset;
		int lengthData = m_iLength - byteOffset;

		// convert find to UTF8
		const char *szUTF8Find = find;
		int numCharsFind, lengthFind;
		if ( !IsUTF8( find ) ) szUTF8Find = ConvertToUTF8( find, &lengthFind, &numCharsFind );
		else numCharsFind = CountUTF8Chars( find, &lengthFind );

		// start the comparison
		if ( lengthFind > (int)lengthData ) 
		{
			if ( szUTF8Find != find ) delete [] szUTF8Find;
			return 0;
		}

		// build shift array so we know later how much of the string we can skip
		int iShiftArray[ 256 ];
		for ( int i = 0; i < 256; i++ ) 
		{
			iShiftArray[ i ] = lengthFind;
		}
		for ( int i = 0; i < lengthFind-1; i++ )
		{
			iShiftArray[ ((const unsigned char*)szUTF8Find)[i] ] = lengthFind-1 - i;
		}
		
		int pos = start;
		unsigned char *str = (unsigned char*)szData;
		while ( *str )
		{
			// compare from the end of the string to the beginning
			int index = lengthFind - 1;
			while( index >= 0 && str[index] == ((unsigned char*)szUTF8Find)[index] ) index--;

			// prepare the next comparison point by skipping as many characters as possible
			int shift = iShiftArray[ str[lengthFind-1] ];
			if ( index == -1 ) 
			{
				// matched
				if ( szUTF8Find != find ) delete [] szUTF8Find;
				return pos;
			}
			
			// didn't match, use non-matching character to try and improve skip amount
			int shift2 = iShiftArray[ str[index] ] - (lengthFind-1-index);
			if ( shift2 > shift ) shift = shift2;
			
			for ( int i = 0; i < shift; i++ )
			{
				str++;
				if ( (*str & 0xC0) != 0x80 ) pos++;
			}
			
			// check the skip amount hasn't gone past the end of the string
			if ( (char*)str+lengthFind > szData+lengthData ) break;
		}
		
		if ( szUTF8Find != find ) delete [] szUTF8Find;

		return -1;
	}
}

int uString::RevFindStr( const char* find, int ignorecase, int start )
{
	if ( !m_pData || m_iLength == 0 ) return -1;
	if ( start < 0 ) start = m_iNumChars - 1;
	if ( start >= (int)m_iNumChars ) start = m_iNumChars - 1;
	if ( !find || !*find ) return -1;

	// convert find to UTF8
	uString sFind( find );

	int byteOffset = GetByteOffset( start );
	
	if ( ignorecase )
	{
		while( byteOffset >= 0 && sFind.CompareCaseToN( m_pData+byteOffset, sFind.GetNumChars() ) != 0 )
		{
			byteOffset--;
			start--;
			while( byteOffset >= 0 && (m_pData[byteOffset] & 0xC0) == 0x80 ) byteOffset--;
		}

		if ( byteOffset < 0 ) return -1;
		else return start;
	}
	else
	{
		while( byteOffset >= 0 && strncmp( m_pData+byteOffset, sFind.m_pData, sFind.GetLength() ) != 0 )
		{
			byteOffset--;
			start--;
			while( byteOffset >= 0 && (m_pData[byteOffset] & 0xC0) == 0x80 ) byteOffset--;
		}

		if ( byteOffset < 0 ) return -1;
		else return start;
	}

	return start;
}

int uString::Count( int find )
{
	if ( !m_pData || m_iLength == 0 ) return 0;

	int iUTF8Find = find;

	int count = 0;
	char* ptr = m_pData;
	int numBytes;
	while ( *ptr )
	{
		int c = DecodeUTF8Char( ptr, &numBytes );
		if ( c == iUTF8Find ) count++;
		ptr += numBytes;
	}

	return count;
}

//****if* uString/SubString
// FUNCTION
//   copies a sub section of the string into the given string object
// INPUTS
//   output -- The location to put the string segment
//   start -- the index to start the copy, includes this character, index starts at 0.
//   length -- number of characters to copy
// SOURCE
void uString::SubString( uString &output, int start, int length )
//****
{
	if ( output.m_pData == m_pData ) return;
	if ( m_iLength == 0 ) return;
	if ( start < 0 ) start = 0;
	if ( start >= (int) m_iNumChars ) return;
	if ( length < 0 ) length = m_iNumChars - start;
	if ( length <= 0 ) return;
	
	int byteOffset = GetByteOffset( start );
	int byteLength;
	if ( start+length < (int)m_iNumChars ) byteLength = GetByteOffsetConst( start+length ) - byteOffset;
	else byteLength = m_iLength - byteOffset;
	
	output.SetStrN( m_pData+byteOffset, byteLength );
}

int uString::CountTokens( const char* delimit )
{
	if ( !m_pData || m_iLength == 0 ) return 0;
	if ( !delimit || !*delimit ) return 1;

	const char *szUTF8Delimit = delimit;
	int numDelimitChars, delimitLength;
	if ( !IsUTF8(delimit, &numDelimitChars) ) szUTF8Delimit = ConvertToUTF8( delimit, &delimitLength, &numDelimitChars );

	// convert delimit string into an array of unicode characters
	int *delimitChars = new int[ numDelimitChars+1 ];
	delimitChars[ numDelimitChars ] = 0;
	int offset = 0;
	int numBytes = 0;
	for ( int i = 0; i < numDelimitChars; i++ )
	{
		delimitChars[ i ] = DecodeUTF8Char( szUTF8Delimit+offset, &numBytes );
		offset += numBytes;
	}

	if ( szUTF8Delimit != delimit ) delete [] szUTF8Delimit;

	int count = 0;
	char* pos = m_pData;
	while ( *pos )
	{
		// find start of token
		numBytes = 0;
		int found = 0;
		do
		{
			pos += numBytes;
			int c = DecodeUTF8Char( pos, &numBytes );
			found = 0;
			for ( int i = 0; i < numDelimitChars; i++ )
			{
				if ( delimitChars[ i ] == c ) { found = 1; break; }
			}
		} while ( found );

		if ( !*pos ) 
		{
			delete [] delimitChars;
			return count;
		}
		count++;

		//find end of token
		do
		{
			pos += numBytes;
			int c = DecodeUTF8Char( pos, &numBytes );
			found = 0;
			for ( int i = 0; i < numDelimitChars; i++ )
			{
				if ( delimitChars[ i ] == c ) { found = 1; break; }
			}
		} while ( *pos && !found );
	}

	delete [] delimitChars;
	return count;
}

char* uString::GetToken( const char* delimit, int index )
{
	uString out;
	GetToken( delimit, index, out );

	char *str = new char[ out.GetLength()+1 ];
	memcpy( str, out.GetStr(), out.m_iLength+1 );
	return str;
}

int uString::GetToken( const char* delimit, int index, uString &out )
{
	if ( index < 1 ) index = 1;

	if ( !m_pData || m_iLength == 0 ) return 0;
	if ( !delimit || !*delimit )
	{
		// return entire string
		out.SetStr( GetStr() );
		return out.GetLength();
	}

	const char *szUTF8Delimit = delimit;
	int numDelimitChars, delimitLength;
	if ( !IsUTF8(delimit, &numDelimitChars) ) szUTF8Delimit = ConvertToUTF8( delimit, &delimitLength, &numDelimitChars );

	// convert delimit string into an array of unicode characters
	int *delimitChars = new int[ numDelimitChars+1 ];
	delimitChars[ numDelimitChars ] = 0;
	int offset = 0;
	int numBytes = 0;
	for ( int i = 0; i < numDelimitChars; i++ )
	{
		delimitChars[ i ] = DecodeUTF8Char( szUTF8Delimit+offset, &numBytes );
		offset += numBytes;
	}

	if ( szUTF8Delimit != delimit ) delete [] szUTF8Delimit;

	int count = 0;
	char *begin = 0;
	char* pos = m_pData;
	while ( *pos )
	{
		// find start of token
		numBytes = 0;
		int found = 0;
		do
		{
			pos += numBytes;
			int c = DecodeUTF8Char( pos, &numBytes );
			found = 0;
			for ( int i = 0; i < numDelimitChars; i++ )
			{
				if ( delimitChars[ i ] == c ) { found = 1; break; }
			}
		} while ( found );

		if ( !*pos ) 
		{
			delete [] delimitChars;
			return 0;
		}
		begin = pos;
		count++;

		//find end of token
		do
		{
			pos += numBytes;
			int c = DecodeUTF8Char( pos, &numBytes );
			found = 0;
			for ( int i = 0; i < numDelimitChars; i++ )
			{
				if ( delimitChars[ i ] == c ) { found = 1; break; }
			}
		} while ( *pos && !found );

		if ( index == count )
		{
			unsigned int newLength = (unsigned int) (pos - begin);
			out.SetStrN( begin, newLength );
			delete [] delimitChars;
			return out.GetLength();
		}
	}

	delete [] delimitChars;
	return 0;
}

int uString::CountTokens2( int delimit )
{
	if ( !m_pData || m_iLength == 0 ) return 0;
	if ( delimit <= 0 ) return 1;

	int count = 1;
	char* pos = m_pData;
	int numBytes;
	while ( *pos )
	{
		int c = DecodeUTF8Char( pos, &numBytes );
		if ( c == delimit ) count++;
		pos += numBytes;
	}

	return count;
}

int uString::GetToken2( int delimit, int index, uString &out )
{
	if ( index < 1 ) index = 1;

	if ( !m_pData || m_iLength == 0 ) return 0;
	if ( delimit <= 0 )
	{
		// return entire string
		out.SetStr( *this );
		return out.GetLength();
	}

	int count = 0;
	char *begin = m_pData;
	char* pos = m_pData;
	int numBytes;
	while ( *pos )
	{
		int c = DecodeUTF8Char( pos, &numBytes );
		
		if ( c != delimit ) 
		{
			pos += numBytes;
			continue;
		}

		count++;
		if ( index == count )
		{
			unsigned int newLength = (unsigned int) (pos - begin);
			out.SetStrN( begin, newLength );
			return out.GetLength();
		}

		pos += numBytes;
		begin = pos;
	}

	count++;
	if ( index == count )
	{
		unsigned int newLength = (unsigned int) (pos - begin);
		out.SetStrN( begin, newLength );
		return out.GetLength();
	}

	out.SetStr( "" );
	return 0;
}

int uString::SplitTokens2( int delimit, uString* &out )
{
	if ( !m_pData || m_iLength == 0 ) return 0;
	if ( delimit <= 0 )
	{
		// return entire string
		out = new uString[ 1 ];
		out[ 0 ].SetStr( GetStr() );
		return 1;
	}

	int count = CountTokens2( delimit );
	if ( count == 1 )
	{
		// return entire string
		out = new uString[ 1 ];
		out[ 0 ].SetStr( *this );
		return 1;
	}

	out = new uString[ count ];
	int index = 0;
	char* begin = m_pData;
	char* pos = m_pData;
	int numBytes;
	while ( *pos )
	{
		int c = DecodeUTF8Char( pos, &numBytes );
		
		if ( c == delimit )
		{
			unsigned int newLength = (unsigned int) (pos - begin);
			if ( newLength > 0 ) out[ index ].SetStrN( begin, newLength );
			index++;
			begin = pos + numBytes;
		}

		pos += numBytes;
	}

	unsigned int newLength = (unsigned int) (pos - begin);
	if ( newLength > 0 ) out[ index ].SetStrN( begin, newLength );

	return count;
}

void uString::XOR( unsigned char x )
{
	unsigned char* pos = (unsigned char*) m_pData;
	while ( *pos )
	{
		unsigned char orig = *pos;
		if ( (*pos & 0x80) == 0 ) *pos = (*pos ^ x) & 0x7f;
		else if ( (*pos & 0xC0) == 0x80 ) *pos = ((*pos ^ x) & 0x3f) | 0x80;
		else if ( (*pos & 0xE0) == 0xC0 ) *pos = ((*pos ^ x) & 0x1f) | 0xC0;
		else if ( (*pos & 0xF0) == 0xE0 ) *pos = ((*pos ^ x) & 0x0f) | 0xE0;
		else if ( (*pos & 0xF8) == 0xF0 ) *pos = ((*pos ^ x) & 0x07) | 0xF0;

		if ( *pos == 0 ) *pos = orig;
		pos++;
	}
}
