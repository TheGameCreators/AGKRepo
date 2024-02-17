#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "Common.h"

void Error( const char* err )
{
	printf( "%s", err );
	printf( "\n" );
	exit(1);
}

void Message( const char* str )
{
	printf( "%s", str );
	printf( "\n" );
}

void Message1( const char* str, int num )
{
	printf( str, num );
	printf( "\n" );
}

void SetCurrentDirectoryWithCheck( const char* path )
{
	if ( chdir( path ) != 0 ) 
	{
		char str[1000];
		sprintf( str, "Failed to set path to: %s", path );
		Error( str );
	}
}

int RunCmd( int index, const char* cmd, const char* params )
{
	printf( "Running %d: %s %s\n", index, cmd, params );
	
	// count parameters
    int count = 2;
    const char *ptr = params;
    while( *ptr )
    {
        if ( *ptr == '"' )
        {
            do { ptr++; } while( *ptr && (*ptr != '"' || *(ptr-1) == '\\') );
            if ( !*ptr ) Error( "Malformed parameter list, no matching double quote" );
        }
        
        if ( *ptr == ' ' && (ptr == params || *(ptr-1) != '\\') ) count++;
        ptr++;
    }
    
    char **vparams = new char*[ count+1 ];
    vparams[ count ] = 0;
    
    vparams[ 0 ] = new char[ strlen(cmd)+1 ];
    strcpy( vparams[0], cmd );
    
    // parse parameters into array
    count = 1;
    ptr = params;
    const char *ptrBegin = params;
    bool bTrimQuotes = false;
    while( *ptr )
    {
        if ( *ptr == '"' )
        {
            if ( ptr == params || *(ptr-1) == ' ' )
            {
                ptrBegin++;
                bTrimQuotes = true;
            }
            do { ptr++; } while( *ptr && (*ptr != '"' || *(ptr-1) == '\\') );
            if ( !*ptr ) Error( "Malformed parameter list, no matching double quote" );
        }
        
        if ( *ptr == ' ' && (ptr == params || *(ptr-1) != '\\') )
        {
            int length = (int)(ptr - ptrBegin);
            if ( ptr != ptrBegin && *(ptr-1) == '"' && bTrimQuotes ) length--;
            if ( length <= 0 ) Error( "Malformed parameter list, more than one space between parameters, or empty quotes" );
            
            vparams[ count ] = new char[ length+1 ];
            strncpy( vparams[ count ], ptrBegin, length );
            vparams[ count ][ length ] = 0;
            
            ptrBegin = ptr+1;
            bTrimQuotes = false;
            count++;
        }
        ptr++;
    }
    
    // last param
    int length = (int)(ptr - ptrBegin);
    if ( ptr != params && *(ptr-1) == '"' && bTrimQuotes ) length--;
    if ( length <= 0 )
    {
        vparams[ count ] = 0;
    }
    else
    {
        vparams[ count ] = new char[ length+1 ];
        strncpy( vparams[ count ], ptrBegin, length );
        vparams[ count ][ length ] = 0;
    }
    
	pid_t pid = fork();
	if ( pid < 0 )
	{
		Error( "Failed to run command" );
		return 1;
	}
	else if ( pid == 0 )
	{
		execvp( cmd, vparams ); // start child process
		_exit(127); // failed to start child process
		return 1;
	}
	else
	{
        for ( int i = 0; i < count; i++ )
            if ( vparams[i] ) delete [] vparams[i];
        delete [] vparams;
        
		int returnStatus = 0;
		waitpid( pid, &returnStatus, 0 );
		return returnStatus;
	}
}

int GetFileContents( const char* filepath, char **data )
{
	FILE *fp = fopen( filepath, "rb" );
	if ( !fp ) return 0;
	fseek( fp, 0, SEEK_END );
	size_t size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	*data = new char[ size+1 ];
	fread( *data, 1, size, fp );
	fclose( fp );

	(*data)[size] = 0;
	return (int)size;
}

int CopyFile2( const char *src, const char *dst )
{
	char buf[ 4096 ];
	int bytes = 0;
	FILE *fpSrc = fopen( src, "rb" );
	if ( !fpSrc ) 
	{
		char err[1000]; sprintf( err, "Failed to open file for reading: %s", src );
		Error( err );
		return 0;
	}

	FILE *fpDst = fopen( dst, "wb" );
	if ( !fpDst ) 
	{
		char err[1000]; sprintf( err, "Failed to open file for writing: %s", dst );
		Error( err );
		return 0;
	}

	do
	{
		bytes = fread( buf, 1, 4096, fpSrc );
		if ( bytes > 0 ) fwrite( buf, 1, bytes, fpDst );
	} while( bytes > 0 );

	fclose( fpSrc );
	fclose( fpDst );

	return 1;
}

// must be absolute paths
int CopyFolder( const char *src, const char *dst, int numIgnore, const char **szIgnoreExt )
{
	if ( *src != '/' ) Error( "Failed to copy, source path must be absolute" );
	if ( *dst != '/' ) Error( "Failed to copy, destination path must be absolute" );

	char currDir[ 1024 ];
	getcwd( currDir, 1024 );
	SetCurrentDirectoryWithCheck( src );
	
	mkdir( dst, 0777 );
	DIR *d = opendir( src );
	dirent *dent = 0;
	
	while( (dent = readdir( d )) )
	{
		if ( strcmp ( dent->d_name, "." ) != 0 && strcmp ( dent->d_name, ".." ) != 0 )
		{
			if ( dent->d_type == DT_DIR )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "/" );
				strcat( newSrc, dent->d_name );

				char newDst[ 1024 ];
				strcpy( newDst, dst );
				strcat( newDst, "/" );
				strcat( newDst, dent->d_name );
				
				mkdir( newDst, 0777 );
				if ( !CopyFolder( newSrc, newDst, numIgnore, szIgnoreExt ) ) return 0;
			}
			else
			{
				// file
				bool bSkip = false;
				char *szExt = strrchr( dent->d_name, '.' );
				for ( int i = 0; i < numIgnore; i++ )
				{
					if ( strcmp(szIgnoreExt[i], szExt) == 0 )
					{
						bSkip = true;
						break;
					}
				}

				if ( bSkip ) continue;

				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "/" );
				strcat( newSrc, dent->d_name );

				char newDst[ 1024 ];
				strcpy( newDst, dst );
				strcat( newDst, "/" );
				strcat( newDst, dent->d_name );

				if ( !CopyFile2( newSrc, newDst ) ) return 0;
			}
		}
	}

	closedir( d );
	
	SetCurrentDirectoryWithCheck( currDir );
	return 1;
}

int DeleteFolder( const char *src )
{
	char currDir[ 1024 ];
	getcwd( currDir, 1024 );
	if ( chdir( src ) != 0 ) return 1;
	
	DIR *d = opendir( src );
	dirent *dent = 0;
	
	while( (dent = readdir( d )) )
	{
		if ( strcmp ( dent->d_name, "." ) != 0 && strcmp ( dent->d_name, ".." ) != 0 )
		{
			if ( dent->d_type == DT_DIR )
			{
				// directory
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "/" );
				strcat( newSrc, dent->d_name );

				if ( !DeleteFolder( newSrc ) ) return 0;
				if ( rmdir( newSrc ) != 0 )
				{
					char err[1024]; sprintf( err, "Failed to remove directory: %s", newSrc );
					Error( err );
					return 0;
				}
			}
			else
			{
				// file
				char newSrc[ 1024 ];
				strcpy( newSrc, src );
				strcat( newSrc, "/" );
				strcat( newSrc, dent->d_name );

				if ( unlink( newSrc ) != 0 ) 
				{
					char err[1024]; sprintf( err, "Failed to delete file: %s", newSrc );
					Error( err );
					return 0;
				}
			}
		}
	}

	closedir( d );
	
	SetCurrentDirectoryWithCheck( currDir );
	
	return 1;
}
