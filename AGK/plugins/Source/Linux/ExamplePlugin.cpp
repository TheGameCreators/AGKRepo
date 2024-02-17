#include <stdio.h>
#include <string.h>

#include "../AGKLibraryCommands.h"

int i = 0;

DLL_EXPORT void SetI( int value )
{
    i = value;
}

DLL_EXPORT int GetI()
{
    return i + 10;
}

DLL_EXPORT float AddFloat(float a, float b)
{
    return a + b;
}

DLL_EXPORT char* ModifyString(const char* str)
{
	int length = (int) strlen(str) + 1;
	char* str2 = agk::CreateString( length );
	strcpy( str2, str );
	str2[0] = 'A';
    return str2;
}

DLL_EXPORT void DLLPrint(const char* str)
{
    agk::Print( str );
}

DLL_EXPORT void CreateRedSquare()
{
	agk::CreateSprite( 1, 0 );
	agk::SetSpriteSize( 1, 100, 100 );
	agk::SetSpritePosition( 1, 10, 100 );
	agk::SetSpriteColor( 1, 255,0,0,255 );
}

DLL_EXPORT void LotsOfParams( int a, int b, float c, float d, int e, const char* f, float g )
{
	char str[256];
	sprintf( str, "a: %d, b: %d, c: %f, d: %f, e: %d, f: %s, g: %f", a, b, c, d, e, f, g );
	agk::Print( str );
}
