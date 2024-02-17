#define AGK_DATA_TYPE_VOID 0
#define AGK_DATA_TYPE_INT 1
#define AGK_DATA_TYPE_FLOAT 2
#define AGK_DATA_TYPE_STRING 3
#define AGK_DATA_TYPE_POINTER 4
#define AGK_DATA_TYPE_ARRAY_INT 11
#define AGK_DATA_TYPE_ARRAY_FLOAT 12
#define AGK_DATA_TYPE_ARRAY_STRING 13

void Error( const char *str );

void strstrip( char *str, const char *tokens = " \t" ); //removes any specified tokens from the string
void strtrimquotes( char *str ); //remove single and double quotes from the ends of a string
void strtrim( char *str, const char *tokens = " \t\n\r" ); //remove specified tokens from the ends of a string
void strupper( char *str ); //convert a string to upper case
void strlower( char *str ); //convert a string to lower case
char nextrealchar( const char *str, const char *tokens = " \t\n\r" ); //returns the next non space character
bool inarray( const char **szArray, int iArrayCount, const char *szStr ); //returns true if szStr is in szArray

int csvExplode( const char *str, char** out, int maxCount ); //explode a string of csv values into an array of strings
int csvCount( const char *str ); //count the number of csv columns

int ConvertType( const char *szStr );
const char* ConvertTypeName( const char *szStr );
const char* TypeString1( int type );
const char* TypeString2( int type );
const char* TypeStringC( int type );