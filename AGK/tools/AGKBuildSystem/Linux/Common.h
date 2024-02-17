#ifndef _H_COMMON
#define _H_COMMON

void Error( const char* err );
void Message( const char* str );
void Message1( const char* str, int num );
void SetCurrentDirectoryWithCheck( const char* path );
int RunCmd( int index, const char* cmd, const char* params );
int GetFileContents( const char* filepath, char **data );
int CopyFile2( const char *src, const char *dst );
int CopyFolder( const char *src, const char *dst, int numIgnore=0, const char **szIgnoreExt=0 );
int DeleteFolder( const char *src );

#endif // _H_COMMON