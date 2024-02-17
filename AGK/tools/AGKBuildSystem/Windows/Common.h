#ifndef _H_COMMON
#define _H_COMMON

void Error( const char* err );
void Message( const char* str );
void Message1( const char* str, int num );

class FileRecord
{
	public:
		FileRecord();
		~FileRecord();

		void AddFile( const char *path );
		void Save( const char* filename );
		void Reset();
		int GetFileCount();
		char* GetFile( int index );

private:
		char **m_szFileNames;
		int m_iNumFiles;
		int m_iArrayLength;
};

void SetCurrentDirectoryWithCheck( const char* path );
int RunCmd( int index, const char* cmd, const char* params );
int GetFileContents( const char* filepath, char **data );
int CreatePath( const char *path );
int CopyFile2( const char *src, const char *dst );
int CopyFolder( const char *src, const char *dst, int numIgnore=0, const char **szIgnoreExt=0 );
int UpdateFolder( const char *src, const char *dst, int numIgnore=0, const char **szIgnoreExt=0 );
int RecordFiles( const char *base, const char* sub, FileRecord* fileStore, int numIgnore=0, const char **szIgnoreExt=0 );
int DeleteFolder( const char *src );
int UpdateWhatsNewFile();

#endif // _H_COMMON