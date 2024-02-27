//PE:
#define NOMINMAX // disable min max from include files, we have our own.

#include "agk.h"
#include "files.h"
#include "Ide.h"
#include "gui.h"
#ifndef AGK_WINDOWS
#include <sys/param.h>
#endif
#ifdef AGK_WINDOWS
#include <direct.h>
#endif

#ifdef AGK_MACOS
extern NSFileManager *g_pMacFileManager;
#endif

//#include <iostream>
//#include <string>
//#include <algorithm>
//#include <vector>

//Variables
bool trialexpired = false;
int trialaction = 0;

int iTotalFolders = 0; // we need unique id's for all folder and files.
int iTotalFiles = 1; // we need unique id's for all folder and files.

//Externals
extern char startupFolder[MAX_PATH];
extern char appFolder[MAX_PATH];
extern char cSteamFolder[MAX_PATH];
extern char *pDragDropText;
extern Preferences pref;
extern char defaultWriteFolder[MAX_PATH];

struct case_insensitive_less : public std::binary_function< char, char, bool >
{
	bool operator () (char x, char y) const
	{
		return toupper(static_cast< unsigned char >(x)) <
			toupper(static_cast< unsigned char >(y));
	}
};

bool NoCaseLess(const std::string &a, const std::string &b)
{
	return std::lexicographical_compare(a.begin(), a.end(),
		b.begin(), b.end(), case_insensitive_less());
}



//#########################################################################################
//#### Check Sandbox if we need to create the folder and setup a defulat project file. ####
//#########################################################################################

void CheckPlayBoxFolder(void)
{
	uString playboxfolder;

#ifdef AGK_WINDOWS
	_chdir(startupFolder);
#else
	chdir(startupFolder);
#endif

	playboxfolder = defaultWriteFolder;
	playboxfolder.Append("sandbox");

	uString dest_folder = defaultWriteFolder;
	dest_folder.Append("sandbox");

	struct stat sb;
	if (stat(dest_folder.GetStr(), &sb) != 0)
	{
		//Folder do not exist. create.
#ifdef AGK_WINDOWS
		_mkdir(dest_folder.GetStr());
#else
		mkdir(playboxfolder.GetStr(), S_IRWXU);
#endif
		dest_folder.Append("/media");
#ifdef AGK_WINDOWS
		_mkdir(dest_folder.GetStr());
#else
		mkdir(playboxfolder.GetStr(), S_IRWXU);
#endif
		playboxfolder = defaultWriteFolder;
		playboxfolder.Append("sandbox");
		playboxfolder.Append("/media/sandbox/sandbox.agk");
		cp_copyfile((char*) "media/newproject.agk", (char*)playboxfolder.GetStr());
	}


#ifdef TRIALVERSIONEXPIRES

	playboxfolder = defaultWriteFolder;
	playboxfolder.Append("sandbox/media/images");
	playboxfolder.ReplaceStr("\\", "/");

	if (pref.trialversion < TRIALVERSION || pref.trialversion == 1234567 ) {

		if (pref.trialversion == 1234567) {
			//First time preferences is created.
			//Dont do anything at this point.
		}
		else {
			//New Trial version, (Current user has used a trial before), so remove old trial block.
#ifdef AGK_WINDOWS
			_rmdir(playboxfolder.GetStr());
#else
			rmdir(playboxfolder.GetStr());
#endif
		}

		pref.trialversion = TRIALVERSION;
		pref.trialdate = (unsigned int) time(0);
	}


	if ( stat(playboxfolder.GetStr(), &sb) != 0 )
	{
		unsigned int curtime = (unsigned int) time(0);

		if ( curtime > (pref.trialdate+( 86400 * TRIALEXPIREDAYS )) ) {

			//Simple expire system, create a folder that expire the trial.
			trialexpired = true;
#ifdef AGK_WINDOWS
			_mkdir(playboxfolder.GetStr());
#else
			mkdir(playboxfolder.GetStr(), S_IRWXU);
#endif
		}
	}
	else {
		//expired.
		trialexpired = true;
	}
	//trialexpired = true; // JUST for testing
#endif

	return;
}


//#######################################################################
//#### Run command with including redirection of stdout,stdin,stderr ####
//#######################################################################

#ifdef AGK_WINDOWS
#include <process.h>

HANDLE hChildStdinRd, hChildStdinWr, hChildStdoutRd, hChildStdoutWr;
PROCESS_INFORMATION m_pi;

int run_command(const char *szFilename, const char *szParameters, int *out_length, int *err_length, bool waittofinish) {
	uString sPath(szFilename);

	uString sCmdLine;
	sCmdLine.Format("\"%s\" %s", sPath.GetStr(), szParameters ? szParameters : "");

	//Convert argv to cmd + par.

	SECURITY_ATTRIBUTES saAttr;
	STARTUPINFOA siStartInfo;

	// Set the bInheritHandle flag so pipe handles are inherited. 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Get the handle to the current STDOUT. 
	// Create a pipe for the child process's STDOUT. 

	if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
		return(false);

	// Ensure that the read handle to the child process's pipe for STDOUT is not inherited.
	SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);
	// Create a pipe for the child process's STDIN. 
	if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0))
		return(false);

	// Ensure that the write handle to the child process's pipe for STDIN is not inherited. 
	SetHandleInformation(hChildStdinWr, HANDLE_FLAG_INHERIT, 0);

	// Now create the child process. 
	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory(&m_pi, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
	siStartInfo.cb = sizeof(STARTUPINFOA);
	siStartInfo.hStdError = hChildStdoutWr;
	siStartInfo.hStdOutput = hChildStdoutWr;
	siStartInfo.hStdInput = hChildStdinRd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 
	char *str = new char[sCmdLine.GetLength() + 1];
	strcpy(str, sCmdLine.GetStr());
	//CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP
	BOOL result = CreateProcessA(NULL, str, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &siStartInfo, &m_pi);
	delete[] str;

	if (!result)  // receives PROCESS_INFORMATION 
		return(false);

	if (!CloseHandle(hChildStdoutWr))
		return(false);

	// Wait until child process exits.
	if (waittofinish)
		WaitForSingleObject(m_pi.hProcess, INFINITE);

	CloseHandle(m_pi.hProcess);
	CloseHandle(m_pi.hThread);
	if (waittofinish) {
		if (result == 0)
			return 1;
		else
			return 0;
	}
	else {
		return m_pi.dwProcessId;
	}
}

int read_buffers(char *out_buffer, int out_length, char *err_buffer, int err_length) {
	DWORD cl;

	DWORD bytesAvail = 0;
	BOOL isOK = PeekNamedPipe(hChildStdoutRd, NULL, 0, NULL, &bytesAvail, NULL);
	if (isOK && bytesAvail > 0)
	{
		if (bytesAvail > out_length)
			bytesAvail = out_length;
		ReadFile(hChildStdoutRd, out_buffer, bytesAvail, &cl, NULL);
	}
	else {
		cl = 0;
	}
	out_buffer[cl] = 0;
	return (int)cl;
}

int write_buffers(char *out_buffer, int out_length) {
	DWORD cl;
	bool result = WriteFile(hChildStdinWr, out_buffer, out_length, &cl, NULL);
	if (!result)
		return(0);
	return (int)cl;
}


int test_pipes(char *cmd)
{
	char tmp[1024];
	int ol, el;
	//	sprintf(tmp, "c:\\wintools\\wget.exe");
	sprintf(tmp, "d:\\github\\AGKTier2\\AgkIde\\media\\data\\android\\aapt2.exe");
	int appid = run_command(&tmp[0], "m", &ol, &el, false);
	//d:\github\AGKTier2\AgkIde\media\data\android\aapt2.exe

	char aaptcommand[1024];
	strcpy(aaptcommand, "compile\n-o\nresMerged\nresOrig\\values\\values.xml\n\n");
	Sleep(200);
	int ws = write_buffers(&aaptcommand[0], strlen(aaptcommand));

	float timestart = agk::Timer();
	int still_running;
	still_running = agk::GetAppRunning(appid);
	while (still_running == 1 && agk::Timer() - timestart < 10.0)
	{
		Sleep(200);
		still_running = agk::GetAppRunning(appid);
	}
	//test send to stdin.
	int rs = read_buffers(&tmp[0], 1024, NULL, 0);
	//	agk::Print("test");
	return true;
}


#endif
#ifndef AGK_WINDOWS
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef AGK_LINUX
#include <sys/types.h>
#include <sys/wait.h>
#endif

int out_pipe[2], err_pipe[2], in_pipe[2];

int run_command(const char *szFilename, const char *szParameters, int *out_length, int *err_length, bool waittofinish)
{

	uString sPath(szFilename);
	//GetRealPath(sPath);

	// count parameters
	int count = 2;
	const char *ptr = szParameters;
	while (*ptr)
	{
		if (*ptr == '"')
		{
			do { ptr++; } while (*ptr && (*ptr != '"' || *(ptr - 1) == '\\'));
			if (!*ptr)
			{
				//Error("Malformed parameter list, no matching double quote");
				return 0;
			}
		}

		if (*ptr == ' ' && (ptr == szParameters || *(ptr - 1) != '\\')) count++;
		ptr++;
	}

	char **vparams = new char*[count + 1];
	for (int i = 0; i <= count; i++) vparams[i] = 0;

	vparams[0] = new char[strlen(szFilename) + 1];
	strcpy(vparams[0], szFilename);

	// parse parameters into array
	count = 1;
	ptr = szParameters;
	const char *ptrBegin = szParameters;
	bool bTrimQuotes = false;
	while (*ptr)
	{
		if (*ptr == '"')
		{
			if (ptr == szParameters || *(ptr - 1) == ' ')
			{
				ptrBegin++;
				bTrimQuotes = true;
			}
			do { ptr++; } while (*ptr && (*ptr != '"' || *(ptr - 1) == '\\'));
			if (!*ptr)
			{
				for (int i = 0; i < count; i++)
					if (vparams[i]) delete[] vparams[i];
				delete[] vparams;
				//Error("Malformed parameter list, no matching double quote");
				return 0;
			}
		}

		if (*ptr == ' ' && (ptr == szParameters || *(ptr - 1) != '\\'))
		{
			int length = (int)(ptr - ptrBegin);
			if (ptr != ptrBegin && *(ptr - 1) == '"' && bTrimQuotes) length--;
			if (length <= 0)
			{
				for (int i = 0; i < count; i++)
					if (vparams[i]) delete[] vparams[i];
				delete[] vparams;
				//Error("Malformed parameter list, more than one space between parameters, or empty quotes");
				return 0;
			}

			vparams[count] = new char[length + 1];
			strncpy(vparams[count], ptrBegin, length);
			vparams[count][length] = 0;
            
            

			ptrBegin = ptr + 1;
			bTrimQuotes = false;
			count++;
		}
		ptr++;
	}

	// last param
	int length = (int)(ptr - ptrBegin);
	if (ptr != szParameters && *(ptr - 1) == '"' && bTrimQuotes) length--;
    
    
    
	if (length <= 0)
	{
		vparams[count] = 0;
	}
	else
	{
		vparams[count] = new char[length + 1];
		strncpy(vparams[count], ptrBegin, length);
		vparams[count][length] = 0;
	}

    for( int i = 0; i <= count; i++)
    {
    char* ptrStr = vparams[i];
    char* ptrStr2 = ptrStr;
    while( *ptrStr )
    {
    if ( *ptrStr != '\\' || (*(ptrStr+1) != ' ' && *(ptrStr+1) != '"') )
    {
    if ( *ptrStr != '"' )
    {
    *ptrStr2 = *ptrStr;
    ptrStr2++;
    }
    }
    ptrStr++;
    }
    *ptrStr2 = 0;
    }
    
	pid_t pid;
	int status = 0;
	struct stat out_stat, err_stat;

	if( pipe(out_pipe) < 0) {
		return(0);
	}
	if (pipe(err_pipe) < 0) {
		return(0);
	}
	if (pipe(in_pipe) < 0) {
		return(0);
	}


	fcntl(out_pipe[0], F_SETFL, O_NONBLOCK);
	fcntl(err_pipe[0], F_SETFL, O_NONBLOCK);

	if (!(pid = fork())) //spawn child 
	{
		// Child. Close the read end of the pipe
		close(out_pipe[0]);
		close(err_pipe[0]);
		close(in_pipe[1]);

		// redirect stdout and stderr to the write end of the pipe
		while( dup2(in_pipe[0], STDIN_FILENO) < 0 );

		dup2(out_pipe[1], STDOUT_FILENO);
		dup2(err_pipe[1], STDERR_FILENO);

		execvp(sPath.GetStr(), vparams);
		//status = execv(argv[0], argv); //child will terminate here
		_exit(127);
	}

	//Only parent gets here. Close write end of the pipe
	close(out_pipe[1]);
	close(err_pipe[1]);
	close(in_pipe[0]);

	//or wait for the child process to terminate
	if (waittofinish)
		waitpid(pid, &status, 0);

	fstat(out_pipe[0], &out_stat);
	fstat(err_pipe[0], &err_stat);

	*out_length = (int)out_stat.st_size;
	*err_length = (int)err_stat.st_size;

	for (int i = 0; i < count; i++)
		if (vparams[i]) delete[] vparams[i];
	delete[] vparams;

	if (waittofinish)
		return status;
	else
		return pid;
}

int run_command2(char *argv[], int *out_length, int *err_length, bool waittofinish) {
	pid_t pid;
	int status = 0;
	struct stat out_stat, err_stat;

	pipe(out_pipe); //create a pipe
	pipe(err_pipe);
	pipe(in_pipe);

	if (!(pid = fork())) //spawn child 
	{
		// Child. Close the read end of the pipe
		close(out_pipe[0]);
		close(err_pipe[0]);
		// redirect stdout and stderr to the write end of the pipe
		dup2(out_pipe[1], STDOUT_FILENO);
		dup2(err_pipe[1], STDERR_FILENO);
		dup2(in_pipe[0], STDIN_FILENO);
		status = execv(argv[0], argv); //child will terminate here
	}

	//Only parent gets here. Close write end of the pipe
	close(out_pipe[1]);
	close(err_pipe[1]);
	close(in_pipe[0]);

	//or wait for the child process to terminate
	if (waittofinish)
		waitpid(pid, &status, 0);

	fstat(out_pipe[0], &out_stat);
	fstat(err_pipe[0], &err_stat);

	*out_length = (int)out_stat.st_size;
	*err_length = (int)err_stat.st_size;
	if (waittofinish)
		return status;
	else
		return pid;
}

//FILE *stream = 0;
int write_buffers(char *out_buffer, int out_length)
{
//	if(!stream)
//		stream = fdopen(in_pipe[1], "w");
//	if (stream) {
//		fputs(out_buffer, stream);
//		fflush(stream);
//	}
   int w = write(in_pipe[1], out_buffer, out_length);
   if (w == -1) {
	   return(0);
   }
	return out_length;
}

int read_buffers(char *out_buffer, int out_length, char *err_buffer, int err_length) {
	if (out_buffer)
		out_buffer[read(out_pipe[0], out_buffer, out_length)] = 0;
	if(err_buffer)
		err_buffer[read(err_pipe[0], err_buffer, err_length)] = 0;
	return 0;
}
#endif

//#########################################
//#### PE: Sort for files and folders. ####
//#########################################

int cstring_cmp_folder(const void *a, const void *b)
{
	struct cFolderItem * ia = *(struct cFolderItem **) a;
	struct cFolderItem * ib = *(struct cFolderItem **) b;
#ifdef AGK_WINDOWS
	return _stricmp(ia->cfolder, ib->cfolder);
#else
	return strcasecmp(ia->cfolder, ib->cfolder);
#endif
}

//###########################
//#### Get the file size ####
//###########################

long cp_getfilesize( char *filename)
{
	struct stat stat_buf;
	int rc = stat(filename, &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

//###############################################
//#### Get real path , resolve relative path ####
//###############################################

int cp_realpath(char *path , char *resolvepath, bool folder)
{
#ifdef AGK_WINDOWS
	int retval = GetFullPathNameA(path, MAX_PATH, resolvepath, NULL);
	if (retval > 0) {
		return(retval);
	}
#else						
	char *res = realpath(path, resolvepath);
	if (res != NULL ) {
		return(true);
	}
#endif
	return(false);
}


bool cp_appendtofile(char *from, char *append)
{
	char buf[BUFSIZ];
	size_t size = 1;

	FILE* source = fopen(from, "a+");
	if (!source)
		source = AGKfopen(from, "a+");
	if (!source)
		return false;
	fwrite(append, 1, strlen(append), source);
	if (source) fclose(source);
	return true;
}

//##############################
//#### Copy file from - to. ####
//##############################

bool cp_copyfile(char *from , char *to)
{
	char buf[BUFSIZ];
	size_t size = 1;

	FILE* source = fopen(from, "rb");
	FILE* dest = fopen(to, "wb+");

	if (!source)
		source = AGKfopen(from, "rb");
	if (!dest)
		dest = AGKfopen(to, "wb+");


	if( source && dest ) {
		while (!feof(source) && size > 0 ) {
			size = fread(buf, 1, BUFSIZ, source);
			fwrite(buf, 1, size, dest);
		}
		fclose(source);
		fclose(dest);
#ifdef AGK_LINUX
		//chmod(to, 0755);
#endif
		return true;
	}
	if(source) fclose(source);
	if(dest) fclose(dest);
	return false;
}

//#include "Winerror.h"
#ifdef AGK_WINDOWS
	#include <shlobj.h>
#endif
#ifdef AGK_LINUX
//	#include <filesystem> // only c++17
#endif

bool CopyDirTo(const char *source_folder, const char *target_folder)
{
#ifdef AGK_WINDOWS

	WCHAR sf[MAX_PATH + 1];
	WCHAR tf[MAX_PATH + 1];
	uString tmp;

	tmp = source_folder;
	tmp.Append("\\*");
	tmp.ReplaceStr("/", "\\");
	mbstowcs(&sf[0], tmp.GetStr() , MAX_PATH);

	tmp = target_folder;
	tmp.ReplaceStr("/", "\\");
	mbstowcs(&tf[0], tmp.GetStr(), MAX_PATH);

	sf[lstrlenW(sf) + 1] = 0;
	tf[lstrlenW(tf) + 1] = 0;

	SHFILEOPSTRUCTW s = { 0 };
	s.wFunc = FO_COPY;
	s.pTo = tf;
	s.pFrom = sf;
	s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;
	int res = SHFileOperationW(&s);
	return res == 0;
#else
#ifdef AGK_MACOS
	uString sFromPath = source_folder;
	uString sToPath = target_folder;

	NSString* pFromMedia = [[NSString alloc] initWithUTF8String:sFromPath.GetStr()];
	NSString* pToMedia = [[NSString alloc] initWithUTF8String:sToPath.GetStr()];

	NSError *fileerror = nil;

	if (g_pMacFileManager == 0)
		g_pMacFileManager = [[NSFileManager alloc] init];

//	if (![g_pMacFileManager createDirectoryAtPath : pToMedia withIntermediateDirectories : NO attributes : nil error : &fileerror]) {
//		if(fileerror)
//			NSLog(@"error %@",fileerror);
//	}

	[g_pMacFileManager removeItemAtPath : pToMedia error : nil];
	if (![g_pMacFileManager copyItemAtPath : pFromMedia toPath : pToMedia error : &fileerror])
	{
		if(fileerror)
			NSLog(@"error %@",fileerror);
		[pFromMedia release];
		[pToMedia release];
		return(false);
	}

	[pFromMedia release];
	[pToMedia release];

	return(true);
#else
	//LINUX CopyDir missing.

	//std::filesystem::copy(source_folder, target_folder , std::filesystem::copy_options::recursive);
	//cp -a /path/from /path/to
	char tmp[MAX_PATH];
	sprintf(tmp, "cp -R \"%s/\"* \"%s\"", source_folder, target_folder);
	system(tmp);
	//Validate if target_folder is there.
	return(true);
#endif
#endif
}

char * g_file_get_contents(char *file, char *contents, int *length, char *unknown)
{
	size_t size = 1;
	char *spTmp;
	FILE* rfile = fopen(file, "rb");
	if (!rfile)
		rfile = AGKfopen(file, "rb");

	if (rfile) {
		fseek(rfile, 0L, SEEK_END);
		size = ftell(rfile);
		fseek(rfile, 0L, SEEK_SET);
		spTmp = (char *)malloc(size+1);
		size = fread(spTmp, 1, size, rfile);
		spTmp[size] = 0;
		*length = (int) size;
		fclose(rfile);
		return(spTmp);
	}
	return(NULL);
}

char * read_eula( void )
{
#ifdef AGK_WINDOWS
	_chdir(startupFolder);
#else
	chdir(startupFolder);
#endif
	size_t size = 1;
	char *spTmp;
	FILE* eulafile = fopen("media/eula.txt", "rb");
	if (!eulafile)
		eulafile = AGKfopen("media/eula.txt", "rb");

	if (eulafile) {
		fseek(eulafile, 0L, SEEK_END);
		size = ftell(eulafile);
		fseek(eulafile, 0L, SEEK_SET);
		spTmp = (char *)malloc(size+1);
		size = fread(spTmp, 1, size, eulafile);
		fclose(eulafile);
		for (int i = 0; i < size; i++) {
			if (spTmp[i] <= 3 || spTmp[i] >= 128)
				spTmp[i] = ' ';
		}
		spTmp[size] = 0;

		return(spTmp);
	}
	return(NULL);
}


//########################
//#### Do file exists ####
//########################

bool file_exists(const char * file)
{
	FILE *pFile;
	if( strncmp(file,"raw:",4) == 0 )
		pFile = fopen(file+4, "rb");
	else
		pFile = fopen(file, "rb");

	if (!pFile && strncmp(file, "raw:", 4) == 0 )
		pFile = AGKfopen(file + 4, "rb");
	else if(!pFile)
		pFile = AGKfopen(file, "rb");

	if (!pFile) return false;
	fclose(pFile);
	return true;
}

//#######################################
//#### Get file "modifyed" timestamp ####
//#######################################

char * getFileCreationTime(char *filePath)
{
	if (!filePath)
		return NULL;
#ifdef AGK_WINDOWS
	struct _stat attrib;
	if (_stat(filePath, &attrib) >= 0) {
		char * date = new char[31];
		if (date) {
			memset(date, 0, 31);
			strftime(date, 20, "%d-%m-%y %H:%M:%S", localtime(&(attrib.st_mtime))); //st_mtime,st_ctime
			return(date);
		}
	}
#else
	struct stat attrib;
	if( stat(filePath, &attrib) >= 0 ) {
		char * date = new char[31];
		if (date) {
			memset(date, 0, 31);
			strftime(date, 20, "%d-%m-%y", localtime(&(attrib.st_mtime)));
			return(date);
		}
	}
#endif
	return NULL;
}


//########################################################
//#### Compile shaders.
//#### Includes , functions used to compile a shader. ####
//########################################################

#ifdef AGK_WINDOWS
//#define GL_GLEXT_PROTOTYPES
//#include "../renderer/OpenGL2/OpenGL2.h"
#include <gl\gl.h>
#include "../../platform/windows/Source/glext.h"
#include "../../platform/windows/Source/wglext.h"
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

#endif

#ifdef AGK_MACOS
#include <OpenGL/gl.h>
#include "../../platform/mac/GLFW/glfw3.h"
#endif

#ifdef AGK_LINUX
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <GL/glx.h>

extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

#endif


uString m_sVSSource, m_sPSSource;

extern bool compiler_done;

void CompileShader(const char* vertex, const char* fragment)
{
	GLint logLength = 0, status = 0;
	GLuint vertShader = 0, fragShader = 0;
	char* fragment2;

	AGK_Compiler::ResetCompilerErrors();
	AGK_Compiler::SetCompilerMaxErrors( 30 );

	if (!glCreateShader)
		return;

	ClearAllEditorMarkers();
	ClearCompilerLog();

	if (fragment) {
		AddToCompilerLog(2, "Compiling fragment shader.\n");
		m_sPSSource.SetStr(fragment);
		m_sPSSource.ReplaceStr("mediump ", " ");
		m_sPSSource.ReplaceStr("highp ", " ");
		m_sPSSource.ReplaceStr("lowp ", " ");
		fragment2 = new char[m_sPSSource.GetLength() + 100];
		strcpy(fragment2, "");
		if (strstr(fragment, "#version") == 0) strcpy(fragment2, "#version 110\n"); // for OpenGL 2.0
		strcat(fragment2, m_sPSSource.GetStr());
	}
	if (vertex) {
		AddToCompilerLog(2, "Compiling vertex shader.\n");

		m_sVSSource.SetStr(vertex);

		// desktop OpenGL doesn't support precision modifiers
		m_sVSSource.ReplaceStr("mediump ", " ");
		m_sVSSource.ReplaceStr("highp ", " ");
		m_sVSSource.ReplaceStr("lowp ", " ");


		char * vertex2 = new char[m_sVSSource.GetLength() + 100];
		strcpy(vertex2, "");
		if (strstr(vertex, "#version") == 0) strcpy(vertex2, "#version 110\n"); // for OpenGL 2.0
		strcat(vertex2, m_sVSSource.GetStr());

		// vertex shader
		vertShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertShader, 1, (const GLchar**)&vertex2, NULL);
		glCompileShader(vertShader);

		delete[] vertex2;

		glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 1)
			{
				GLchar *log = (GLchar*)malloc(logLength);
				glGetShaderInfoLog(vertShader, logLength, &logLength, log);
				AddToCompilerLog(1, "Vertex shader error.\n%s\n",log);
				//m_sVSLog.SetStr(log);
				free(log);
			}

			if (vertShader > 0) glDeleteShader(vertShader);
			ImGui::SetWindowFocus("Message window");
			compiler_done = true;
			return;
		}

	}

	if (fragment) {

		// fragment shader
		fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragShader, 1, (const GLchar**)&fragment2, NULL);
		glCompileShader(fragShader);

		delete[] fragment2;

		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 1)
			{
				GLchar *log = (GLchar*)malloc(logLength);
				glGetShaderInfoLog(fragShader, logLength, &logLength, log);
				//m_sPSLog.SetStr(log);
				AddToCompilerLog(1, "Fragment shader error.\n%s\n", log);
				free(log);
			}

			if (vertShader > 0) glDeleteShader(vertShader);
			if (fragShader > 0) glDeleteShader(fragShader);
			ImGui::SetWindowFocus("Message window");
			compiler_done = true;
			return;
		}

	}

	if (fragment)
		AddToCompilerLog(2, "Fragment shader looked and compiled ok.\n");
	else
		AddToCompilerLog(2, "Vertex shader looked and compiled ok.\n");
	compiler_done = true;

	//Success compile ok.
	if( vertShader > 0 ) glDeleteShader(vertShader);
	if( fragShader > 0 ) glDeleteShader(fragShader);
	ImGui::SetWindowFocus("Message window");
	return;
}


//##########################################################
//#### Recursive get all files/folders inside a folder. ####
//##########################################################

int largest_path = 0;
#define MAXASSETSFOLDERSPERLEVEL 200
cFolderItem * getalldirs(cFolderItem *pFolder, cFolderItem *pParentFolder, char * path)
{
	char *oldpath,*oldfirstpath, *ctmp, *cftmp;
	if (iTotalFolders > 9000) return(0);
	if (path == NULL) return(0);

	oldfirstpath = agk::GetWritePath();
	if ( strlen(path) > 0 )
		agk::SetRawWritePath(path);

	cFolderItem *m_pfirstFolder = pFolder;
	cFolderItem *m_pDotDotDotFolder = pFolder;
	if (pParentFolder)
		m_pDotDotDotFolder = pParentFolder;

	int mc = 0;
	ctmp = agk::GetFirstFolder(1);

	//precache all strings.
	char *cached_str[999];
	cached_str[0] = 0;
	while (ctmp && strlen(ctmp) > 0 && mc < MAXASSETSFOLDERSPERLEVEL) {
		cached_str[mc] = ctmp;
		mc++;
		cached_str[mc] = 0;
		ctmp = agk::GetNextFolder();
	}

	mc = 0;
	ctmp = cached_str[mc];
	if (ctmp) {
		if ((strlen(ctmp) + strlen(path)) > largest_path)
			largest_path = (strlen(ctmp) + strlen(path));
	}

	while( ctmp && strlen(ctmp) > 0 && mc < MAXASSETSFOLDERSPERLEVEL -1) {

		cFolderItem *pNewItem;
		pNewItem = new cFolderItem();
		pNewItem->m_sFolder.SetStr(ctmp);
		strcpy(pNewItem->cfolder, pNewItem->m_sFolder.GetStr());
		pNewItem->m_sFolderFullPath.SetStr(agk::GetWritePath());
		pNewItem->m_sFolderFullPath.Append(ctmp);
		pNewItem->m_pSubFolder = NULL;
		pNewItem->visible = true;
		pNewItem->deletethisentry = false;
		pNewItem->count = ++iTotalFolders;
		pNewItem->m_pFirstFile = NULL;
		pNewItem->bIsCustomFolder = false;
		pFolder->m_pNext = pNewItem;


	//Get all files.
		if (pFolder && !pFolder->m_bFilesRead && pFolder->m_sFolder.CompareTo("...") != 0 && strlen(ctmp) > 0)
		{
			oldpath = agk::GetWritePath();
			agk::SetRawWritePath(pFolder->m_sFolderFullPath.GetStr());


			//Update last folder modify date time.
			struct stat sb;
			if (stat(pFolder->m_sFolderFullPath, &sb) == 0) {
				if (sb.st_mtime != pFolder->m_tFolderModify) {
					pFolder->m_tFolderModify = sb.st_mtime;
				}
			}

			//Check/Read files.
			pFolder->m_pFirstFile = NULL;
			int mcf = 0;
			cFolderItem::sFolderFiles * m_pFiles;

			std::vector<std::string> sorted_files;
			cftmp = agk::GetFirstFile(1);
			while (cftmp && strlen(cftmp) > 0 && mcf < 9999) {
				sorted_files.push_back(cftmp);
				delete[] cftmp;
				cftmp = NULL;
				cftmp = agk::GetNextFile();
				mcf++;
			}
			if ( !sorted_files.empty() ) {
				std::sort(sorted_files.begin(), sorted_files.end(), NoCaseLess);
//				std::reverse(sorted_files.begin(), sorted_files.end());

				std::vector<std::string>::iterator it = sorted_files.begin();
				if (it->size() > 0) {
					cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles();
					pNewItem->m_sName = "...";
					pNewItem->m_sPath = "";
					pNewItem->m_sFolder = "[na]";
					pNewItem->iFlags = 0;
					pNewItem->iPreview = 0;
					pNewItem->iBigPreview = 0;
					pNewItem->id = iTotalFiles++;
					pNewItem->bPreviewProcessed = false;
					pNewItem->m_pNext = NULL;
					pFolder->m_pFirstFile = pNewItem;
					m_pFiles = pNewItem;
				}
				//++it;
				for (; it != sorted_files.end(); ++it) {
					if (it->size() > 0) {
						cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles();
						pNewItem->m_sName.SetStr(it->c_str());
						pNewItem->m_sPath.SetStr(pFolder->m_sFolderFullPath.GetStr());
						pNewItem->m_sFolder = pFolder->m_sFolder;
						pNewItem->m_sFolder.ReplaceStr("-> ", "");
						pNewItem->m_sFolder.ReplaceStr("'", "");
						pNewItem->m_sFolder.ReplaceStr(" Project Media", "");
						pNewItem->iFlags = 0;
						pNewItem->iPreview = 0;
						pNewItem->iBigPreview = 0;
						pNewItem->id = iTotalFiles++;
						pNewItem->bPreviewProcessed = false;
						pNewItem->m_pNext = NULL;
						m_pFiles->m_pNext = pNewItem;
						m_pFiles = pNewItem;
					}
				}
				sorted_files.clear();
			}

/*
			cftmp = agk::GetFirstFile(1);

			if (cftmp && strlen(cftmp) > 0) {
				cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles();
				pNewItem->m_sName = "...";
				pNewItem->m_sPath = "";
				pNewItem->m_sFolder = "[na]";
				pNewItem->iFlags = 0;
				pNewItem->iPreview = 0;
				pNewItem->iBigPreview = 0;
				pNewItem->id = iTotalFiles++;
				pNewItem->bPreviewProcessed = false;
				
				pNewItem->m_pNext = NULL;
				pFolder->m_pFirstFile = pNewItem;

				m_pFiles = pNewItem;
			}

			while (cftmp && strlen(cftmp) > 0 && mcf < 9999) {
				cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles();
				pNewItem->m_sName.SetStr(cftmp);
				pNewItem->m_sPath.SetStr(pFolder->m_sFolderFullPath.GetStr());
				pNewItem->m_sFolder = pFolder->m_sFolder;
				pNewItem->m_sFolder.ReplaceStr("-> ", "");
				pNewItem->m_sFolder.ReplaceStr("'", "");
				pNewItem->m_sFolder.ReplaceStr(" Project Media", "");

				pNewItem->iFlags = 0;
				pNewItem->iPreview = 0;
				pNewItem->iBigPreview = 0;
				pNewItem->id = iTotalFiles++;
				pNewItem->bPreviewProcessed = false;
				pNewItem->m_pNext = NULL;

				m_pFiles->m_pNext = pNewItem;
				m_pFiles = pNewItem;

				delete[] cftmp;
				cftmp = agk::GetNextFile();
				mcf++;
			}
*/
			pFolder->m_bFilesRead = true;

			//Sort entryes.

			agk::SetRawWritePath(oldpath);

		}

		pFolder = pNewItem;
		
		delete[] cached_str[mc];
		mc++;
		ctmp = cached_str[mc];

		//Find curret folder location.
		//agk::GetFirstFolder(1);
		//for (int iloop = 0; iloop < mc; iloop++) {
		//	delete[] ctmp;
		//	ctmp = agk::GetNextFolder();
		//}
	}

	agk::SetRawWritePath(oldfirstpath);

	if (m_pfirstFolder == pFolder) return 0;

	if (mc > 1) {
		//#### SORT ####
		char ** cptr = new char *[mc + 1];

		cFolderItem *m_pSortFolder = m_pfirstFolder->m_pNext;
		for (int a = 0; a < mc; a++) {
			if (m_pSortFolder) {
				cptr[a] = (char *)m_pSortFolder;
				m_pSortFolder = m_pSortFolder->m_pNext;
			}
		}

		qsort(cptr, mc, sizeof(cptr[0]), cstring_cmp_folder);

		m_pSortFolder = m_pfirstFolder->m_pNext;
		m_pfirstFolder->m_pNext = (cFolderItem *)cptr[0];
		for (int a = 0; a < mc; a++) {
			m_pSortFolder = (cFolderItem *)cptr[a];
			if (m_pSortFolder) {
				if (a + 1 < mc) m_pSortFolder->m_pNext = (cFolderItem *)cptr[a + 1];
			}
		}
		delete [] cptr;
		if (m_pSortFolder) m_pSortFolder->m_pNext = NULL;
	}


	//	if (pStartFolder)
	//		m_pfirstFolder = pStartFolder;
	if (m_pfirstFolder->m_pNext)
		m_pfirstFolder = m_pfirstFolder->m_pNext;
	else
		m_pfirstFolder = NULL;

	while (m_pfirstFolder) {
		bool bignore = false;

		//Mac dont add files from .app folders.
		uString ext, macignore = m_pfirstFolder->m_sFolder;
		if (macignore.GetLength() >= 4) {
			macignore.SubString(ext, macignore.GetLength() - 4);
			if (ext.CompareCaseTo(".app") == 0)
				bignore = true;
		}

		cFolderItem *pNewItem = new cFolderItem();
		pNewItem->m_sFolder = "...";
		strcpy(pNewItem->cfolder, pNewItem->m_sFolder.GetStr());
		pNewItem->count = 0;
		pNewItem->m_pNext = NULL;

		if (pParentFolder)
			pNewItem->m_pSubFolder = m_pDotDotDotFolder;
		else
			pNewItem->m_pSubFolder = NULL;

//		if(m_pDotDotDotFolder)
//			pNewItem->m_pSubFolder = m_pDotDotDotFolder;
//		else
//			pNewItem->m_pSubFolder = NULL;

		pNewItem->m_pFirstFile = NULL;
		pNewItem->bIsCustomFolder = false;
		m_pfirstFolder->m_pSubFolder = pNewItem;
		char * fullpath = (char *)m_pfirstFolder->m_sFolderFullPath.GetStr();
		if(!bignore)
			getalldirs(pNewItem, m_pfirstFolder, fullpath);
		m_pfirstFolder = m_pfirstFolder->m_pNext;
	}

	return 0;
}

void GetSteamFolder( void )
{
	//Check startup folder. "C:\Program Files (x86)\Steam\steamapps\common\App Game Kit 2\"

	uString steamFolder = "";
	uString startFolder = startupFolder;
	int pos = startFolder.FindStr("\\steamapps\\common\\");
	if( pos < 0)
		pos = startFolder.FindStr("/steamapps/common/");
	if (pos > 0) {
		//We are launched from within the steam folder.
		startFolder.SubString(steamFolder, 0, pos + 18);
		steamFolder.Append("App Game Kit 2");
#ifdef AGK_WINDOWS
		steamFolder.Append("\\");
#else
		steamFolder.Append("/");
#endif
		strcpy(cSteamFolder, steamFolder.GetStr());
	}

#ifdef AGK_WINDOWS
	//Check startup folder.
	steamFolder = GetAGKFolder();
	steamFolder.ReplaceStr("/", "\\"); // Only "\" on windows.
	strcpy(cSteamFolder, steamFolder.GetStr());
#else
	//Mac need to get user name append what is needed.
	char* pEnvUser;
	//putenv("USER=pe"); //windows test.
	pEnvUser = getenv("USER");
	if (pEnvUser) {
		uString sf;
#ifdef AGK_MACOS
		sf = "/Users/";
		sf.Append(pEnvUser);
		sf.Append("/Library/Application Support/Steam/steamapps/common/App Game Kit 2/");
		//check if exist.
		//else ask and save in setup.ini
		strcpy(cSteamFolder, sf.GetStr());
#else
		sf = "/home/";
		sf.Append(pEnvUser);
		sf.Append("/.local/share/Steam/steamapps/common/App Game Kit 2/");
		//check if exist.
		//else ask and save in setup.ini
		strcpy(cSteamFolder, sf.GetStr());

#endif

	}
#endif

#ifdef AGK_LINUX
	printf("cSteamFolder: %s\n", cSteamFolder);
#endif
	return;
}


#ifdef AGK_WINDOWS
uString GetAGKFolder( void )
{
	uString AGKFolder, checkfolder;
	DWORD dwType = REG_SZ;
	HKEY hKey = 0;
	TCHAR lszValue[512];

	DWORD value_length = 512;
	LSTATUS ls;
	ls = RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", &hKey);
	if (RegQueryValueEx(hKey, L"InstallPath", NULL, &dwType, (LPBYTE)&lszValue, &value_length) == ERROR_SUCCESS) {
		lszValue[value_length] = 0;
		char c_szText[512];
		wcstombs(c_szText, lszValue, value_length);
		AGKFolder = c_szText;
		AGKFolder.ReplaceStr("\\", "/");
	}
	else {
		LSTATUS ls;
		ls = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);
		ls = RegQueryValueEx(hKey, L"InstallPath", NULL, &dwType, (LPBYTE)&lszValue, &value_length);
		if (ls == ERROR_SUCCESS) {
			lszValue[value_length] = 0;
			char c_szText[512];
			wcstombs(c_szText, lszValue, value_length);
			AGKFolder = c_szText;
			AGKFolder.ReplaceStr("\\", "/");
		}
	}
	checkfolder = AGKFolder;
	checkfolder.Append("/steamapps/appmanifest_325180.acf"); //Check current for AGK.
	FILE *fp = fopen(checkfolder, "r");
	if (!fp)
		fp = AGKfopen(checkfolder, "r");

	if (!fp == NULL) {
		fclose(fp);
		checkfolder = AGKFolder;
		checkfolder.Append("/steamapps/common/App Game Kit 2/");
		return(checkfolder);
	}
	else {

		checkfolder = AGKFolder;
		checkfolder.Append("/steamapps/libraryfolders.vdf"); //Check if steam app is moved to another drive.
		FILE *fp = fopen(checkfolder, "r");
		if (!fp)
			fp = AGKfopen(checkfolder, "r");

		if (!fp == NULL) {
			fclose(fp);
			//Read it:
			//"LibraryFolders"
			//{
			//	"TimeNextStatsReport"		"1542185290"
			//		"ContentStatsID"		"-7695251817932306232"
			//		"1"		"D:\\div\\SteamLibrary"
			//}
			FILE *fp;
			char str[512];

			fp = fopen(checkfolder, "r");
			if (!fp)
				fp = AGKfopen(checkfolder, "r");

			if (!fp == NULL) {
				while (fgets(str, MAXCHAR, fp) != NULL) {
					uString usCheck = str;
					usCheck.ReplaceStr("\\\\", "/");
					usCheck.ReplaceStr("\\t", " ");
					usCheck.ReplaceStr("\t", " ");
					usCheck.ReplaceStr("\n", "");
					usCheck.ReplaceStr("\\""", """");
					usCheck.ReplaceStr("\"", "!");
					if (usCheck.RevFindStr("!1!") > 0) {
						uString m_szParse, ext, rest, usTmp;
						int pos = usCheck.RevFind('!');
						if (pos >= 0) {
							usCheck.SubString(ext, pos);
							usCheck.SubString(rest, 0, pos);

							int pos = rest.RevFind('!');
							if (pos >= 0) {
								rest.SubString(AGKFolder, pos + 1);
								AGKFolder.Append("/steamapps/common/App Game Kit 2/");
								return(AGKFolder);
							}

						}
					}
				}
				fclose(fp);
			}

		}

	}
	//Nothing found, just set default.
	AGKFolder = "c:/div/SteamLibrary/steamapps/common/App Game Kit 2/";
	return(AGKFolder);
}

uString GetGameGuruFolder()
{
	uString GameGuruFolder, checkfolder;
	DWORD dwType = REG_SZ;
	HKEY hKey = 0;
	TCHAR lszValue[512];

	DWORD value_length = 512;
	LSTATUS ls;
	ls = RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", &hKey);
	if (RegQueryValueEx(hKey, L"InstallPath", NULL, &dwType, (LPBYTE)&lszValue, &value_length) == ERROR_SUCCESS) {
		lszValue[value_length] = 0;
		char c_szText[512];
		wcstombs(c_szText, lszValue, value_length);
		GameGuruFolder = c_szText;
		GameGuruFolder.ReplaceStr("\\", "/");
	}
	else {
		LSTATUS ls;
		ls = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);
		ls = RegQueryValueEx(hKey, L"InstallPath", NULL, &dwType, (LPBYTE)&lszValue, &value_length);
		if (ls == ERROR_SUCCESS) {
			lszValue[value_length] = 0;
			char c_szText[512];
			wcstombs(c_szText, lszValue, value_length);
			GameGuruFolder = c_szText;
			GameGuruFolder.ReplaceStr("\\", "/");
		}
	}
	checkfolder = GameGuruFolder;
	checkfolder.Append("/steamapps/appmanifest_266310.acf"); //Check current.
	FILE *fp = fopen(checkfolder, "r");
	if (!fp)
		fp = AGKfopen(checkfolder, "r");

	if (!fp == NULL) {
		fclose(fp);
		checkfolder = GameGuruFolder;
		checkfolder.Append("/steamapps/common/Game Guru/Files/entitybank/");
		return(checkfolder);
	}
	else {

		checkfolder = GameGuruFolder;
		checkfolder.Append("/steamapps/libraryfolders.vdf"); //Check current.
		FILE *fp = fopen(checkfolder, "r");
		if (!fp)
			fp = AGKfopen(checkfolder, "r");

		if (!fp == NULL) {
			fclose(fp);
			//Read it:
			//"LibraryFolders"
			//{
			//	"TimeNextStatsReport"		"1542185290"
			//		"ContentStatsID"		"-7695251817932306232"
			//		"1"		"D:\\div\\SteamLibrary"
			//}
			FILE *fp;
			char str[512];

			fp = fopen(checkfolder, "r");
			if (!fp)
				fp = AGKfopen(checkfolder, "r");

			if (!fp == NULL) {
				while (fgets(str, MAXCHAR, fp) != NULL) {
					uString usCheck = str;
					usCheck.ReplaceStr("\\\\", "/");
					usCheck.ReplaceStr("\\t", " ");
					usCheck.ReplaceStr("\t", " ");
					usCheck.ReplaceStr("\n", "");
					usCheck.ReplaceStr("\\""", """");
					usCheck.ReplaceStr("\"", "!");
					if (usCheck.RevFindStr("!1!") > 0) {
						uString m_szParse, ext, rest, usTmp;
						int pos = usCheck.RevFind('!');
						if (pos >= 0) {
							usCheck.SubString(ext, pos);
							usCheck.SubString(rest, 0, pos);

							int pos = rest.RevFind('!');
							if (pos >= 0) {
								rest.SubString(GameGuruFolder, pos + 1);
								GameGuruFolder.Append("/steamapps/common/Game Guru/Files/entitybank/");
								return(GameGuruFolder);
							}

						}
					}
				}
				fclose(fp);
			}

		}

	}
	//Nothing found, just set default.
	GameGuruFolder = "c:/div/SteamLibrary/steamapps/common/Game Guru/Files/entitybank/";
	return(GameGuruFolder);
}
#endif

static int button_count;
char cDropCode[4096];

void renderTheText(uString renderText, bool entercodemode, bool bSeperator)
{
	char ctmp[MAX_PATH];
	if (entercodemode) {
		uString ipText = renderText;
		ImVec2 cursoroldend;
		char *change;

		//renderText.Trim(" ");
		if (renderText[0] == ' ') {
			renderText.SubString(ipText, 1);
		}

		change = (char*)ipText.GetStr();
		if (change[ipText.GetLength() - 1] == '\n')
			change[ipText.GetLength() - 1] = 0;

		sprintf(ctmp, "##ecode%d", button_count++);
		int ti_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly;
		ImGui::PushItemWidth(-36);
		float count = ipText.Count('\n');
		if( count >= 1) {
			count++;
			count *= (ImGui::CalcTextSize("#").y+1.5);

			ImGui::InputTextMultiline(ctmp, (char *)ipText.GetStr(), ipText.GetLength() , ImVec2(-36, count), ti_flags);
			cursoroldend = ImGui::GetCursorPos();
			ImGui::SameLine();

//			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f,0.0f) );
//			ImGui::Button((char *)ipText.GetStr()); //, ImVec2(-16, count));
//			ImGui::PopStyleVar();

		}
		else {
			ImGui::InputText(ctmp, (char *)ipText.GetStr(), ipText.GetLength() + 1, ti_flags);
			cursoroldend = ImGui::GetCursorPos();
			ImGui::SameLine();
		}

		ImGui::PushID(button_count);
		if (ImGui::Button(ICON_MD_CONTENT_COPY)) {
			if (change && strlen(change) < 4096) {
				ImGui::SetClipboardText(change);
			}
		}
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			if (change && strlen(change) < 4096) {
				strcpy(cDropCode, change);
				char *ptr = &cDropCode[0];
				ImGui::SetDragDropPayload("DND_TEXT_DROP_TARGET", &ptr, sizeof(char*));
				ImGui::Text("Drop Code");
				pDragDropText = &cDropCode[0];
			}
			ImGui::EndDragDropSource();
		}
		ImGui::PopID();
		ImGui::SetCursorPos(cursoroldend);

		if (ImGui::BeginPopupContextItemAGK(ctmp)) //"project context menu"
		{
			if (ImGui::MenuItem("Copy to clipboard")) {
				ImGui::SetClipboardText(ipText.GetStr());
			}
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();
	}
	else {
		uString ipText = renderText;
		//renderText.Trim(" ");
		int pos = 0;
		int size = renderText.GetLength();
		if (renderText[pos] == '\n' && renderText[pos+1] == ' ') {
			pos++;
			while (renderText[pos] == ' ' && renderText[pos] != 0 && pos < size)
				pos++;
			if (pos > 0) {
				renderText.SubString(ipText, pos);
				renderText = "\n";
				renderText.Append(ipText);
				ipText = renderText;
			}
		}
		else {
			while (renderText[pos] == ' ' && renderText[pos] != 0 && pos < size)
				pos++;
			if (pos > 0) {
				if (pos >= renderText.GetLength())
					ipText = "";
				else
					renderText.SubString(ipText, pos);
			}
		}
		if(ipText.GetLength() > 0 )
			ImGui::TextWrapped("%s",ipText.GetStr());
	}
	if (bSeperator) {
		ImGui::Separator();
	}
	return;
}

#define MAXHELPFILESIZE 32768
static char cHelpPagePath[MAX_PATH];
static char cHelpFolder[MAX_PATH];
static char cHelpPage[MAXHELPFILESIZE + 2];
static char cHelpPage2[MAXHELPFILESIZE + 2];
bool bHelpSetup = false;
bool bSetScrollToTop = false;
bool bHTMLfirstRun = false;
int iFoundImages = 0;
int iFoundImagesThisRun = 0;
int iLoadedImages[99]; // max 99 images per page.
KeywordHelp *sKeywordHelp[256], *sKeyNext;
char cSearchHelp[MAX_PATH];

float fDeepButScrollY[10];

void processhelp(const char * page,bool readnewpage)
{
	if (!page) return;

	if( readnewpage || !bHelpSetup )
	{
		
		//Load new help page.

		strcpy(cHelpPagePath, page);
		strcpy(cSearchHelp, "");
#ifdef AGK_WINDOWS
		_chdir(startupFolder);
#else
		chdir(startupFolder);
#endif

		if (!bHelpSetup) {
			//Get full path to help files.
			strcpy(cHelpFolder, "");
			strcpy(cHelpPage, "");
			int retval = cp_realpath(cHelpPagePath, cHelpFolder, true);
			if (retval > 0) {
				uString HelpFolder;
				HelpFolder = cHelpFolder;
				HelpFolder.ReplaceStr("media/Help/home.html", "");
				HelpFolder.ReplaceStr("media\\Help\\home.html", "");
				strcpy(cHelpFolder, HelpFolder.GetStr());
			}

			//Setup keywords for search. using first char as index, for faster search.
			for (int a = 0; a < 256;a++)
				sKeywordHelp[a] = NULL;

			for (int a = 0; a < 10; a++)
				fDeepButScrollY[a] = 0.0;

			//Read Command,Return type,syntax.
			FILE* keyfile = fopen("media/main.agc.tags", "rb");
			if (!keyfile)
				keyfile = AGKfopen("media/main.agc.tags", "rb");

			if (keyfile) {
				char fpReadLine[500];
				while (!feof(keyfile)) {
					if (fgets(&fpReadLine[0], 500, keyfile)) {

						uString usString, usCommand, usReturn, usSyntax, usRest;
						usString = fpReadLine;
						int pos = usString.FindStr("|");
						if (pos > 0) {
							usString.SubString(usCommand, 0, pos);
							usString.SubString(usRest, pos + 1);

							int pos2 = usRest.FindStr("|");
							if (pos2 >= 0) {
								usRest.SubString(usReturn, 0, pos2);
								usRest.SubString(usString, pos2 + 1);
								int pos3 = usString.FindStr("|");
								if (pos3 >= 0) {
									usString.SubString(usSyntax, 0, pos3);
									usSyntax.ReplaceStr("|", "");
								}
							}
						}

						int index = tolower(char(fpReadLine[0]));

						if (sKeywordHelp[index] == NULL) {
							sKeywordHelp[index] = new KeywordHelp();
							sKeywordHelp[index]->m_pNext = NULL;
						}

						//First entry.
						sKeyNext = sKeywordHelp[index];
						if (sKeyNext) {
							while (sKeyNext->m_pNext != NULL)
								sKeyNext = sKeyNext->m_pNext;
						}

						//Last in entry.
						sKeyNext->m_pNext = new KeywordHelp();
						sKeyNext = sKeyNext->m_pNext;

						strcpy(sKeyNext->m_cCommand, usCommand.GetStr());
						usCommand.Lower();
						strcpy(sKeyNext->m_cLowerCommand, usCommand.GetStr());

						sKeyNext->m_cSyntax.SetStr(usSyntax.GetStr()); //do it this way if usSyntax is empty.
						sKeyNext->m_cReturn.SetStr(usReturn.GetStr());
						sKeyNext->m_pNext = NULL;

					}
				}
				fclose(keyfile);
			}

			//Get path to commands.
			//"Help/keywordlinks.txt"
			
			//Read command,path to help file.
			keyfile = fopen("media/Help/keywordlinks.txt", "rb");
			if (!keyfile)
				keyfile = AGKfopen("media/Help/keywordlinks.txt", "rb");

			if (keyfile) {
				char fpReadLine[501];
				while (!feof(keyfile)) {
					if (fgets(&fpReadLine[0], 500, keyfile)) {

						uString usString, usCommand, usRest;
						usString = fpReadLine;
						int pos = usString.FindStr("|");
						if (pos > 0) {
							usString.SubString(usCommand, 0, pos);
							usString.SubString(usRest, pos + 1);
							usCommand.Lower();
							//Locate command.

							int index = tolower(char(fpReadLine[0]));
							sKeyNext = sKeywordHelp[index];
							if (sKeyNext->m_pNext) {
								sKeyNext = sKeyNext->m_pNext;

								while (sKeyNext != NULL) {
									
									if (strcmp(usCommand.GetStr(), sKeyNext->m_cLowerCommand) == 0) {
//										sKeyNext->m_cCommandPath = cHelpFolder;
//										sKeyNext->m_cCommandPath.Append(usRest.GetStr());
										sKeyNext->m_cCommandPath = "media/Help/";
										usRest.ReplaceStr("\n", "");
										usRest.ReplaceStr("\r", "");
										sKeyNext->m_cCommandPath.Append(usRest);
									}
									sKeyNext = sKeyNext->m_pNext;
								}

							}

						}

					}
				}
				fclose(keyfile);
			}
			bHelpSetup = true;
		}

		//Load page.
		uString usFullHelpPath;
		usFullHelpPath = cHelpFolder;
		usFullHelpPath.Append(cHelpPagePath);
		
		bHTMLfirstRun = true;
		//Delete any old images.
		for (int i = 0; i < iFoundImages; i++) {
			if (agk::GetImageExists(iLoadedImages[i])) {
				agk::DeleteImage(iLoadedImages[i]);
				iLoadedImages[i] = 0;
			}
		}
		iFoundImages = 0;

		FILE* source = fopen(usFullHelpPath.GetStr(), "rb");
		if (!source)
			source = AGKfopen(usFullHelpPath.GetStr(), "rb");

		if (source) {
			int size = fread(cHelpPage, 1, MAXHELPFILESIZE, source);
			cHelpPage[size] = 0;
			fclose(source);
		}
		else {
			sprintf(cHelpPage, "Cant read %s.", usFullHelpPath.GetStr());
		}

		uString parsedPage;

		char * find = strstr(cHelpPage, "<div class=\"footer\"");
		if (find)
			find[0] = 0;

		find = strstr(cHelpPage, "<a href=");
		while (find) {
			find[0] = '[';
			char *find2 = strstr(find, ">");
			if (find2)
				find2[0] = ']';

			find = strstr(find + 1, "<a href=");
		}

		find = strstr(cHelpPage, "<body>");
		if (find) {
			find[0] = ' ';
			find[1] = ' ';
			find[2] = ' ';
			find[3] = ' ';
			find[4] = ' ';
			strcpy(cHelpPage2, find );
		}

		strcpy(cHelpPage,cHelpPage2);

		find = strstr(cHelpPage, "<div class=\"page\">");
		if (find) {
			strcpy(cHelpPage2, find + 18);
		}

		strcpy(cHelpPage, cHelpPage2);

		parsedPage = cHelpPage;

		parsedPage.ReplaceStr("[a href", "\\[a href");

		parsedPage.ReplaceStr("\n\r", "\n");
		parsedPage.ReplaceStr("\r\n", "\n");
		//parsedPage.ReplaceStr("\n", "");

		parsedPage.ReplaceStr("“", "\"");
		parsedPage.ReplaceStr("’", "'");
		parsedPage.ReplaceStr("‘", "'"); 
		parsedPage.ReplaceStr("”", "\"");

		parsedPage.ReplaceStr("</h1>", "\\[/h1]\n");
		parsedPage.ReplaceStr("</H1>", "\\[/h1]\n");
		parsedPage.ReplaceStr("</h2>", "\\[/h2]\n");
		parsedPage.ReplaceStr("</H2>", "\\[/h2]\n");
		parsedPage.ReplaceStr("</h3>", "\\[/h3]\n");
		parsedPage.ReplaceStr("</H3>", "\\[/h3]\n");
		
		parsedPage.ReplaceStr("<Br>", "<br>");
		parsedPage.ReplaceStr("<BR>", "<br>");
		parsedPage.ReplaceStr("<br/>", "<br>");
		parsedPage.ReplaceStr("<Br/>", "<br>");
		parsedPage.ReplaceStr("<BR/>", "<br>");
		 
		parsedPage.ReplaceStr("</ul>", ""); //\n
		parsedPage.ReplaceStr("<ul>", "<br>"); // ul not supported so just newline.

		parsedPage.ReplaceStr("\t", "    "); //tab

		parsedPage.ReplaceStr("<h1>", "\\[h1]");
		parsedPage.ReplaceStr("<h2>", "\\[h2]");
		parsedPage.ReplaceStr("<h3>", "\\[h3]");
		parsedPage.ReplaceStr("<p>", "\\[p]");
		parsedPage.ReplaceStr("</p>", "\\[/p]");
		parsedPage.ReplaceStr("</P>", "\\[/p]");

		//parsedPage.ReplaceStr("<li>", "[li]");
		//parsedPage.ReplaceStr("</li>", "[/li]\n");
		//parsedPage.ReplaceStr("<i>", "[i]");
		//parsedPage.ReplaceStr("</i>", "[/i]");

		//For now remove tags we dont parse yet!
		parsedPage.ReplaceStr("<li>", "\\[li]"); // bullet.
		parsedPage.ReplaceStr("</li>", "\\[/li]");
		parsedPage.ReplaceStr("<i>", "");
		parsedPage.ReplaceStr("</i>", "");


		char tmp[100];
		for (int a = 0; a < 50; a++) {
			//<div id="basicCodeBlock1"
			sprintf(tmp, "<div id=\"basicCodeBlock%d\"", a);
			parsedPage.ReplaceStr(tmp, "\\[code]<###");
			sprintf(tmp, "<div id =\"basicCodeBlock%d\" class=\"basiccode\">", a);
			parsedPage.ReplaceStr(tmp, "\\[code]");
			sprintf(tmp, "<div id = \"basicCodeBlock%d\" class=\"basiccode\">", a);
			parsedPage.ReplaceStr(tmp, "\\[code]");
			sprintf(tmp, "<div id=\"basicCodeBlock%d\" class=\"basiccode\">", a);
			parsedPage.ReplaceStr(tmp, "\\[code]");
			sprintf(tmp, "<div id=\"cppCodeBlock%d\" style=\"display:none\" class=\"cppcode\">", a);
			parsedPage.ReplaceStr(tmp, "\\[codecpp]");
			sprintf(tmp, "<div id= \"cppCodeBlock%d\" style=\"display:none\" class=\"cppcode\">", a);
			parsedPage.ReplaceStr(tmp, "\\[codecpp]");
			sprintf(tmp, "<div id = \"cppCodeBlock%d\" style=\"display:none\" class=\"cppcode\">", a);
			parsedPage.ReplaceStr(tmp, "\\[codecpp]");
		}
		//<div id="basicCodeBlock1" class="basiccode">
		//<div id="cppCodeBlock2" style="display:none" class="cppcode">
		parsedPage.ReplaceStr("</a>", "\\[/a]");

		//Looks wierd but we need to make sure we dont have spaces before a tag.
		parsedPage.ReplaceStr("          \\", " \\");
		parsedPage.ReplaceStr("         \\", " \\");
		parsedPage.ReplaceStr("        \\", " \\");
		parsedPage.ReplaceStr("       \\", " \\");
		parsedPage.ReplaceStr("      \\", " \\");
		parsedPage.ReplaceStr("     \\", " \\");
		parsedPage.ReplaceStr("    \\", " \\");
		parsedPage.ReplaceStr("   \\", " \\");
		parsedPage.ReplaceStr("  \\", " \\");
		parsedPage.ReplaceStr(" \\", "\\");
		parsedPage.ReplaceStr("       \n", "\n");
		parsedPage.ReplaceStr("      \n", "\n");
		parsedPage.ReplaceStr("     \n", "\n");
		parsedPage.ReplaceStr("    \n", "\n");
		parsedPage.ReplaceStr("   \n", "\n");
		parsedPage.ReplaceStr("  \n", "\n");
		parsedPage.ReplaceStr("  \n", "\n");
		parsedPage.ReplaceStr(" \n", "\n");
		parsedPage.ReplaceStr("        <", "<");
		parsedPage.ReplaceStr("       <", "<");
		parsedPage.ReplaceStr("      <", "<");
		parsedPage.ReplaceStr("     <", "<");
		parsedPage.ReplaceStr("    <", "<");
		parsedPage.ReplaceStr("   <", "<");
		parsedPage.ReplaceStr("  <", "<");
		parsedPage.ReplaceStr(" <", "<");

/*
		parsedPage.ReplaceStr("     ", " ");
		parsedPage.ReplaceStr("    ", " ");
		parsedPage.ReplaceStr("   ", " ");
		parsedPage.ReplaceStr("  ", " ");
		parsedPage.ReplaceStr("  ", " ");
		parsedPage.ReplaceStr("  ", " ");
		parsedPage.ReplaceStr("  ", " ");
		parsedPage.ReplaceStr("  ", " ");
		parsedPage.ReplaceStr("\n ", "\n");
*/
		parsedPage.ReplaceStr("&nbsp;", " ");
		
		strcpy(cHelpPage, parsedPage.GetStr());
		//Remove remaining html tags.

/*
		int size = strlen(cHelpPage);
		find = strstr(cHelpPage, "<");
		while (find && find - &cHelpPage[0] < size) {
			char *find2 = strstr(find+1, "</"); // '</' also content inside.
			if (find2) {
				char *find3 = strstr(find, ">"); // '</' also content inside.
				if (find3)
					strcpy(find, find3 + 1); // +1
			}
			else
				find++;
			find = strstr(find, "<");
		}
		//Run it again now only remaining <> tags.
		size = strlen(cHelpPage);
		find = strstr(cHelpPage, "<");
		while (find && find - &cHelpPage[0] < size) {
			char *find2 = strstr(find, ">");
			if (find2)
				strcpy(find, find2 + 1);
			else
				find++;
			find = strstr(find, "<");
		}
*/

//DEBUG html
//		FILE* dest = fopen("desthtml.txt", "wb+");
//		if (dest) {
//			fwrite(cHelpPage, 1, strlen(cHelpPage), dest);
//			fclose(dest);
//		}

		parsedPage = "";

		//PE: Get deep of current page.

		bSetScrollToTop = true;
		//bSearchActive = true;
		//ImGui::SetCursorPosition(ImVec2(0.0,0.0));
		//Parse page.

		//cHelpPage

		return;
	}

	//Break out if we are now visible.
	bool wtabvisible = ImGui::windowTabVisible();
	if (!wtabvisible)
		return;


	char *render;
	ImVec4* style_colors;
	render = &cHelpPage[0];
	int size = strlen(cHelpPage);
	uString renderText = "";
	char ctmp[MAX_PATH];
	char but_link[MAX_PATH];
	bool seperator = false;
	bool usenewline = false;
	bool userawdata = false;
	bool entercodemode = false;
	button_count = 0;
	style_colors = ImGui::GetStyle().Colors;
	size = strlen(cHelpPagePath);
	char *nav,*navend;
	nav = &cHelpPagePath[0];
	navend = nav;
	nav = strstr(cHelpPagePath, "/");
	bool skip_rest = false;

	iFoundImagesThisRun = 0;


	//####################
	//#### Navigation ####
	//####################

	static int iCurrentDeepBut = 0;
	int iScrollToDeepBut = 0;
	int iCountDeepBut = 0;
	//Render navigation.
	if (ImGui::Button("Home")) {
		iScrollToDeepBut = iCountDeepBut;
		processhelp((char*)"media/Help/home.html", true);
	}
	ImGui::SameLine();
	while (nav && nav- &cHelpPagePath[0] < size) {
		char link[MAX_PATH];
		int i = 0;
		navend = strstr(nav+1, "/");
		if (navend) {
			iCountDeepBut++;
			if(nav[0] == '/')
				nav++;
			while (nav < navend) {
				if (i < MAX_PATH) {
					link[i++] = nav[0];
					link[i] = 0;
					nav++;
				}
//				nav++;
			}
			//but link.
			if (strcmp(link, "Reference") == 0) {
				if (ImGui::Button("Commands")) {
					iScrollToDeepBut = iCountDeepBut-1;
					processhelp((char*)"media/Help/commands.html", true);
				}
				ImGui::SameLine();
			}
			else if (strcmp(link, "Help") == 0) {
				//Ignore.
			}
			//examples
			else {
				if( !skip_rest && strlen(link) > 1) {
					uString uslink, mlink;
					uslink = cHelpPagePath;
					uslink.SubString(mlink, 0, nav - &cHelpPagePath[0]);
					//mlink.Append(link);
					if (mlink.FindStr("Reference") > 0) {
						mlink.Append(".htm");
					}
					else {
						mlink.Append(".html");
					}
					//skip_rest
					if( strcmp(link, "examples") == 0 )
						skip_rest = true;
					if (ImGui::Button(link)) {
						iScrollToDeepBut = iCountDeepBut-1;
						processhelp((char*)mlink.GetStr(), true);
					}
					ImGui::SameLine();
				}
			}
			//agk::Print("");
		}
		else {
			if (nav[0] == '/')
				nav++;

			//Open link online.
/*
			if (ImGui::Button(ICON_MD_LINK)) {
				uString openonweb;
				openonweb = cHelpPagePath;
				openonweb.ReplaceStr("media/Help/", "https://www.appgamekit.com/documentation/");
				agk::OpenBrowser(openonweb.GetStr());
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Display page online.");
			
			ImGui::SameLine();
*/
			iCurrentDeepBut = iCountDeepBut;
//			ImGui::Text("%s(%d,%d)", nav, iCurrentDeepBut, (int) fDeepButScrollY[iCurrentDeepBut] );
			ImGui::Text("%s",nav);
			ImGui::SameLine();
			break;
		}
		nav++;
	}
	ImGui::Text("");

	//Search box commands.
	ImGui::PushItemWidth(-18);
	ImGui::InputText( ICON_MD_SEARCH "##ks", &cSearchHelp[0], MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue);
	if (strlen(cSearchHelp) > 0) {
		//Search.
		uString find;
		find = cSearchHelp;
		find.Lower();
		//Run into all index for all commands.
		for (int index = 0; index < 256; index++) {
			sKeyNext = sKeywordHelp[index];
			if (sKeyNext && sKeyNext->m_pNext) {
				sKeyNext = sKeyNext->m_pNext;
				while (sKeyNext) {
					if (strstr(sKeyNext->m_cLowerCommand, find.GetStr()) != NULL ) {
						if (sKeyNext->m_cCommandPath.GetLength() > 1) {
							uString syntax;
							uString syntaxtooltip;
							syntax = sKeyNext->m_cCommand;
							syntax.Append(sKeyNext->m_cSyntax);
							syntax.ReplaceStr("Integer ", "");
							syntax.ReplaceStr("String ", "");
							syntax.ReplaceStr("Float ", "");

							if (ImGui::Selectable(syntax.GetStr())) {
								//Goto help.
								processhelp((char*)sKeyNext->m_cCommandPath.GetStr(), true);
							}

							if (sKeyNext->m_cSyntax.GetLength() == 2) {
								syntaxtooltip = sKeyNext->m_cCommand;
								syntaxtooltip.Append(sKeyNext->m_cSyntax);
							}
							else {
								syntaxtooltip = sKeyNext->m_cCommand;
								syntaxtooltip.Append("\n");
								syntaxtooltip.Append(sKeyNext->m_cSyntax);
							}
							syntaxtooltip.Append("\n");
							syntaxtooltip.Append("Return: ");
							if (sKeyNext->m_cReturn.GetLength() == 0)
								syntaxtooltip.Append("none");
							else
								syntaxtooltip.Append(sKeyNext->m_cReturn);
							syntaxtooltip.Append("\n");

							if (ImGui::IsItemHovered()) {
								//ImGui::BeginTooltip();
								ImGui::SetTooltip("%s", syntaxtooltip.GetStr());
								//ImGui::TextUnformatted(syntaxtooltip.GetStr());
								//ImGui::EndTooltip();
							}
						}
					}
					sKeyNext = sKeyNext->m_pNext;
				}
			}
		}
	}
	ImGui::PopItemWidth();
	ImGui::Separator();

	//ImGui::EndColumns();
	ImGui::BeginChild("##navigation12", ImVec2(0, 0)); //, false, ImGuiWindowFlags_HorizontalScrollbar);
	

	if (bSetScrollToTop) {
		ImGui::SetScrollX(0);
		float fNextScrollY = 0.0;
		fNextScrollY = fDeepButScrollY[iScrollToDeepBut];

		ImGui::SetScrollY(fNextScrollY);
		bSetScrollToTop = false;
	}

	//Render html page.
	size = strlen(cHelpPage);
	ImVec4 oldTextColor = style_colors[ImGuiCol_Text];

	while (render[0] != 0 && (render - &cHelpPage[0]) < size)
	{
		if( !userawdata && render[0] == '\\' && render[1] == '[')
		{
			render++;
			if (strlen(renderText.GetStr()) > 0) {
				renderTheText(renderText, entercodemode, seperator);
				seperator = false;
				renderText = "";
			}

			//[a href="introduction.html"]Introduction[/a]
			if (render[1] == 'a') { //Reset everything.
				//Special button style.
				render += 2;
				while (render[0] != '"' && render[0] != ']' && render[0] != 0)
					render++;

				if (render[0] == '"') {
					render++;
					int a = 0;
					while (render[0] != '"' && render[0] != ']' && render[0] != 0) {
						but_link[a++] = render[0];
						but_link[a] = 0;
						if (a > MAX_PATH - 1)
							break;
						render++;
					}
					if (render[0] == '"' && render[1] == ']') {
						//OK.
						strcpy(ctmp, "");
						char limg[MAX_PATH];
						limg[0] = 0;

						render += 2;
//						while (render[0] != '\\' && render[1] != '[' && render[0] != '/' && render[0] != 0) {
						while (render[0] != '\\' && render[1] != '[' && render[0] != 0) {

							//Remove any < tags.

							//This is where we can parse and insert a image button.
							if (render[0] == '<') {

								//Image tag ?
								if (render[1] == 'i' && render[2] == 'm' && render[3] == 'g') {
									//Image tag.
									while (!(render[0] == 's' && render[1] == 'r' && render[2] == 'c' && render[3] == '=') && render[0] != 0)
										render++;
									if (render[0] == 's') {
										render += 4;
										if (render[0] == '\"')
											render++;
										int cc = 0;
										while (!(render[0] == '\"') && render[0] != 0 && render[0] != '>') {
											if (cc < MAX_PATH - 1) {
												limg[cc++] = render[0];
												limg[cc] = 0;
											}
											render++;
										}
									}
								}

								while (render[0] != '>' && render[0] != 0)
									render++;
								if (render[0] != 0)
									render++;
							}
							else {
								ctmp[0] = render[0];
								ctmp[1] = 0;
								renderText.Append(ctmp);
								render++;
							}

							//
						}

						//ImGui::SameLine();

						if (strlen(ctmp) > 0 || strlen(limg) > 0 ) { 
							uString finalLink, mlink;
							mlink = but_link;

							if (strlen(limg) > 0) {
								//Image link.

								if (bHTMLfirstRun && iFoundImages < 99) {

									//Relative to current page.
									uString finalLink = cHelpPagePath;
									uString mlink = "media/Help/";
									int pos = finalLink.RevFindStr("/");
									if (pos > 0) {
										finalLink.SubString(mlink, 0, pos + 1);
									}
									finalLink = mlink;

									finalLink.Append(limg);
									uString usFullHelpPath;
									usFullHelpPath = "raw:";
									usFullHelpPath.Append(cHelpFolder);
									usFullHelpPath.Append(finalLink);

									iLoadedImages[iFoundImages] = agk::LoadImage(usFullHelpPath.GetStr());
									iFoundImages++;
								}

								if (agk::GetImageExists(iLoadedImages[iFoundImagesThisRun]) && iFoundImagesThisRun < 99) {
									//Imgbtn.
									int myTextureID;
									int iBorderSize = ImGui::GetCurrentWindow()->ScrollbarSizes.x + 8;
									int TitleSize = 24;
									int iTextHeight = 20;
									float fPreviewWidth = agk::Round(ImGui::GetWindowSize().x) - iBorderSize;
									float fPreviewHeight = agk::Round(ImGui::GetWindowSize().y) - (iBorderSize + TitleSize + iTextHeight);
									cImage *pImage = agk::GetImagePtr(iLoadedImages[iFoundImagesThisRun]);
									int iImgW = agk::GetImageWidth(iLoadedImages[iFoundImagesThisRun]);
									int iImgH = agk::GetImageHeight(iLoadedImages[iFoundImagesThisRun]);
									float fRatio;
									if ((fPreviewWidth / iImgW) < (fPreviewHeight / iImgH))
										fRatio = fPreviewWidth / iImgW;
									else
										fRatio = fPreviewHeight / iImgH;
									if (fPreviewWidth > iImgW)
										fRatio = 1.0;

									if (pImage) {
										myTextureID = pImage->GetTextureID() + 100000; // 100000+ = No Color Array , 200000+ = No transparent
										
										if (strstr(limg, "prev.png")) {
											float fCenterX = fPreviewWidth * 0.5;
											ImGui::SetCursorPos(ImVec2(fCenterX - (iImgW*fRatio) - 8, ImGui::GetCursorPosY())); //TitleSize
										}
										if (strstr(limg, "next.png")) {
											float fCenterX = fPreviewWidth * 0.5;
											ImGui::SetCursorPos(ImVec2(fCenterX + 8, ImGui::GetCursorPosY())); //TitleSize
										}
										//float fCenterX = (fPreviewWidth - iImgW*fRatio) * 0.5;
										//ImGui::SetCursorPos(ImVec2(fCenterX + 2, ImGui::GetCursorPosY())); //TitleSize
										if (ImGui::ImageButton((void*)myTextureID, ImVec2(iImgW*fRatio, iImgH*fRatio), ImVec2(0, 0), ImVec2(1, 1), -5, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
											//Link.
											mlink = "";
											finalLink = cHelpPagePath;
											mlink = "media/Help/";
											int pos = finalLink.RevFindStr("/");
											if (pos > 0) {
												finalLink.SubString(mlink, 0, pos + 1);
											}
											finalLink = mlink;
											finalLink.Append(but_link);
											//Show new link page.
											processhelp((char*)finalLink.GetStr(), true);
											style_colors[ImGuiCol_Text] = oldTextColor;
											ImGui::SetWindowFontScale(1.0);
											ImGui::EndChild();
											//ImGui::EndColumns();
											return;
										}
										//ImGui::SameLine();
									}

								}
								iFoundImagesThisRun++;
								ImGui::SameLine();
								renderText = "";
								if (render[0] == '\\')
									render++;
								if (render[0] == '[' && render[1] == '/' && render[2] == ']')
									render += 3;
								if (render[0] == '[' && render[1] == '/' && render[3] == ']')
									render += 4;

							}
							else if (mlink.FindStr(":") < 0 && mlink.Find('#') >= 0) {
								ImGui::Text("%s",renderText.GetStr());
							}
							else {
								//Make it unique.
								sprintf(ctmp, "%s##%d", renderText.GetStr(), button_count++);
								if (ImGui::Button(ctmp)) {
									
									if (mlink.FindStr(":") > 0) {
										agk::OpenBrowser(but_link);
									}
									else if (mlink.FindStr(".pdf", 1) > 0) {

										finalLink = cHelpFolder;
										finalLink.ReplaceStr("\\", "/");
										finalLink.Append("media/Help/");
										finalLink.Append(but_link);

										agk::OpenBrowser(finalLink);
									}
									else {

										mlink = "";
										//cHelpPagePath
										//Relative to current page so.
										finalLink = cHelpPagePath;
										mlink = "media/Help/";
										int pos = finalLink.RevFindStr("/");
										if (pos > 0) {
											finalLink.SubString(mlink, 0, pos + 1);
										}
										finalLink = mlink;
										finalLink.Append(but_link);
										//Show new link page.
										//but_link
										processhelp((char*)finalLink.GetStr(), true);
									}
									style_colors[ImGuiCol_Text] = oldTextColor;
									ImGui::SetWindowFontScale(1.0);
									ImGui::EndChild();
									//ImGui::EndColumns();
									return;
								}
							}
							ImGui::SameLine();
							renderText = "";
							if (render[0] == '\\')
								render++;
							if (render[0] == '[' && render[1] == '/' && render[2] == ']')
								render += 3;
							if (render[0] == '[' && render[1] == '/' && render[3] == ']')
								render += 4;
						}

					}
				}
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == 'i' && render[2] == ']') { //Reset everything.
				render += 3;
				ImGui::SameLine();
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == '/' && render[2] == 'i' && render[3] == ']') { //Reset everything.
				render += 4;
				ImGui::SameLine();
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == 'p' && render[2] == ']') { //Reset everything.
				render += 3;
				if(!entercodemode)
					renderText.Append("\n");

				style_colors[ImGuiCol_Text] = oldTextColor;
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == 'l' && render[2] == 'i' && render[3] == ']') { //Reset everything.
				render += 4;
				ImGui::SameLine();
				//renderText.Append(" "); // before images.
				renderText.Append("\n"); // after images and not display " ".
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == '/' && render[2] == 'a' && render[3] == ']') { //Reset everything.
				render += 4;
				//ImGui::SameLine();
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == '/' && render[2] == 'l' && render[3] == 'i' && render[4] == ']') { //Reset everything.
				render += 5;
				renderText.Append("\n");
				//ImGui::SameLine();
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == '/' && render[2] == ']') { //Reset everything.
				render += 3;
				style_colors[ImGuiCol_Text] = oldTextColor;
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == '/' && render[3] == ']') { //Reset everything.
				render += 4;
				style_colors[ImGuiCol_Text] = oldTextColor;
				ImGui::SetWindowFontScale(1.0);
			}
			else if (render[1] == '/' && render[4] == ']') { //Reset everything.
				render += 5;
				style_colors[ImGuiCol_Text] = oldTextColor;
				ImGui::SetWindowFontScale(1.0);
			}
			//[code]
			else if (render[1] == 'c' && render[2] == 'o' && render[3] == 'd' && render[4] == 'e' && render[5] == ']') {
				render += 6;
				entercodemode = true;
			}
			//[codecpp]
			else if (render[1] == 'c' && render[2] == 'o' && render[3] == 'd' && render[4] == 'e' && render[5] == 'c' && render[6] == 'p' && render[7] == 'p' && render[8] == ']') {
				render += 9;
				//Ignore if not cpp.
				while (render[0] != '\n' && render[0] != 0 && !(render[0] == '\\' && render[1] == '[' && render[2] == '/'))
					render++;
			}
			else if (render[1] == 'h' && render[2] == '1' && render[3] == ']') {
				render += 4;
				//ImGui::SameLine();
				//renderText.Append("\n");
				ImGui::SetWindowFontScale(1.35);
				style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_PlotHistogram]; ////ImVec4(0.85, 0.0, 0.35, 1.0);
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8, 0.2, 0.2, 1.0));

			}
			else if (render[1] == 'h' && render[2] == '2' && render[3] == ']') {
				render += 4;
				ImGui::SetWindowFontScale(1.20);
				style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_TextDisabled];
				renderText.Append("\n");
				seperator = true;
			}
			else if (render[1] == 'h' && render[2] == '3' && render[3] == ']') {
				render += 4;
				ImGui::SetWindowFontScale(1.10);
				style_colors[ImGuiCol_Text] = style_colors[ImGuiCol_TextDisabled];
				renderText.Append("\n");
				seperator = true;
			}
			else {
				render++;
			}

		}
		else if (render[0] == '<' && !userawdata) {
			//Render any text already buffered.
			if (strlen(renderText) > 0) {
				
				if (entercodemode && strcmp(cHelpPagePath, "media/Help/guides/scancodes.htm")==0 ) {
					renderText.ReplaceStr("<b>", "");
					renderText.ReplaceStr("</b>", " ");
					renderText.ReplaceStr("<B>", "");
					renderText.ReplaceStr("</B>", " ");
					renderText.ReplaceStr("\\                220", "   \\                220");
					renderTheText(renderText, entercodemode, seperator);
				}
				else {
					renderTheText(renderText, entercodemode, seperator);
				}
				seperator = false;
				renderText = "";
			}

			//<img class="example-image" src="firstproject001.jpg" alt="image-1" />


			//Check for any tags with special attension.
			if (render[1] == 'i' && render[2] == 'm' && render[3] == 'g') {
				//Image tag.
				while (!(render[0] == 's' && render[1] == 'r' && render[2] == 'c' && render[3] == '=') && render[0] != 0)
					render++;
				if (render[0] == 's') {
					render += 4;
					if (render[0] == '\"')
						render++;
					char limg[MAX_PATH];
					limg[0] = 0;
					int cc = 0;
					while (!(render[0] == '\"') && render[0] != 0 && render[0] != '>') {
						if (cc < MAX_PATH - 1) {
							limg[cc++] = render[0];
							limg[cc] = 0;
						}
						render++;
					}
					//
					if (bHTMLfirstRun && iFoundImages < 99 ) {

						//Relative to current page.
						uString finalLink = cHelpPagePath;
						uString mlink = "media/Help/";
						int pos = finalLink.RevFindStr("/");
						if (pos > 0) {
							finalLink.SubString(mlink, 0, pos + 1);
						}
						finalLink = mlink;
						
						finalLink.Append(limg);
						uString usFullHelpPath;
						usFullHelpPath = "raw:";
						usFullHelpPath.Append(cHelpFolder);
						usFullHelpPath.Append(finalLink);

						iLoadedImages[iFoundImages] = agk::LoadImage(usFullHelpPath.GetStr() );
						iFoundImages++;
					}

					if (agk::GetImageExists(iLoadedImages[iFoundImagesThisRun]) && iFoundImagesThisRun < 99 ) {
						//Imgbtn.
						int myTextureID;
						int iBorderSize = ImGui::GetCurrentWindow()->ScrollbarSizes.x + 8;
						int TitleSize = 24;
						int iTextHeight = 20;
						float fPreviewWidth = agk::Round(ImGui::GetWindowSize().x) - iBorderSize;
						float fPreviewHeight = agk::Round(ImGui::GetWindowSize().y) - (iBorderSize + TitleSize + iTextHeight);

						cImage *pImage = agk::GetImagePtr(iLoadedImages[iFoundImagesThisRun]);
						int iImgW = agk::GetImageWidth(iLoadedImages[iFoundImagesThisRun]);
						int iImgH = agk::GetImageHeight(iLoadedImages[iFoundImagesThisRun]);
						float fRatio;
						if ((fPreviewWidth / iImgW) < (fPreviewHeight / iImgH))
							fRatio = fPreviewWidth / iImgW;
						else
							fRatio = fPreviewHeight / iImgH;
						if (fPreviewWidth > iImgW)
							fRatio = 1.0;

						if (pImage) {
							myTextureID = pImage->GetTextureID() + 200000; // 100000+ = No Color Array , 200000+ = No transparent
							float fCenterX = (fPreviewWidth - iImgW*fRatio) * 0.5;
							ImGui::SetCursorPos(ImVec2(fCenterX + 2, ImGui::GetCursorPosY() )); //TitleSize
							ImGui::ImageButton((void*)myTextureID, ImVec2(iImgW*fRatio, iImgH*fRatio), ImVec2(0, 0), ImVec2(1, 1), -5, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
						}

					}
					iFoundImagesThisRun++;
					//debug.
					//renderText.Append("\n[img=");
					//renderText.Append(limg);
					//renderText.Append("]\n");
					//agk::Print(limg);

				}

			}
			else if (render[1] == 'p' && render[2] == 'r' && render[3] == 'e') {
				usenewline = true;
				userawdata = true;
			}
			else if (render[1] == 'b' && render[2] == 'r' && render[3] == '>') {
				renderText.Append("\n");
			}
			else if (render[1] == 'b' && render[2] == 'r' && render[3] == '/') {
				renderText.Append("\n");
			}
			else if (render[1] == 'B' && render[2] == 'R' && render[3] == '>') {
				renderText.Append("\n");
			}
			else if (render[1] == 'B' && render[2] == 'r' && render[3] == '>') {
				renderText.Append("\n");
			}
			else if (render[1] == '/' && render[2] == 'p' && render[3] == 'r' && render[4] == 'e') {
				usenewline = false;
				userawdata = false;
			}
			else if (render[1] == '/' && render[2] == 'd' && render[3] == 'i' && render[4] == 'v') {
				entercodemode = false;
				usenewline = false;
				userawdata = false;
			}
			else if (render[1] == 'b' && render[2] == 'u' && render[3] == 't' ) {
				usenewline = false;
				//Remove all tag code.
				while ( !(render[0] == '<' && render[1] == '/' && (render[2] == 'b' || render[2] == 'B') && (render[3] == 'u' || render[3] == 'U')) && render[0] != 0)
					render++;
			}
			else if (render[1] == 's' && render[2] == 'c' && render[3] == 'r' && render[4] == 'i' && render[5] == 'p' && render[6] == 't') {
				//<script type="text/javascript">
				usenewline = false;
				//Remove all tag code.
				while (!(render[0] == '<' && render[1] == '/') && render[0] != 0)
					render++;
			}
			else if (render[1] == 'b' && render[2] == 'u' && render[3] == 't' && render[4] == 't' && render[5] == 'o' && render[6] == 'n') {
				//<script type="text/javascript">
				usenewline = false;
				//Remove all tag code.
				while (!(render[0] == '<' && render[1] == '/') && render[0] != 0)
					render++;
			}
			else if (render[1] == '!' && render[2] == '-' && render[3] == '-') {
				usenewline = false;
				//Remove until code. -->
				while (!(render[0] == '-' && render[1] == '-' && render[2] == '>') && render[0] != 0)
					render++;
				//if (render[0] != 0)
				//	render += 3;
			}
			//<!--
			else if (render[9] == 'c' && render[10] == 'p' && render[11] == 'p' && render[12] == 'C' && render[13] == 'o') {
				//<div id="cppCodeBlock2"
				usenewline = false;
				//Remove all tag code until a end.
				while (!(render[0] == '<' && render[1] == '/') && render[0] != 0)
					render++;
			}

			//Get to next >
			while (render[0] != '>' && render[0] != 0)
				render++;
			if(render[0] != 0)
				render++;
		}
		else if(render[0] == '<' && userawdata) {
			//In raw mode , only look for closing tags.
			if (render[1] == '/' && render[2] == 'd' && render[3] == 'i' && render[4] == 'v') {
				usenewline = false;
				userawdata = false;
				while (render[0] != '>' && render[0] != 0)
					render++;
				if (render[0] != 0)
					render++;
			}
			else if (render[1] == '/' && render[2] == 'p' && render[3] == 'r' && render[4] == 'e') {
				usenewline = false;
				userawdata = false;
				while (render[0] != '>' && render[0] != 0)
					render++;
				if (render[0] != 0)
					render++;
			}
			else if (render[1] == 'b' && render[2] == 'r' && render[3] == '>') {
				renderText.Append("\n");
				render += 4;
			}
			else if (render[1] == 'b' && render[2] == 'r' && render[3] == '/') {
				renderText.Append("\n");
				render += 4;
			}
			else if (render[1] == 'B' && render[2] == 'r' && render[3] == '/') {
				renderText.Append("\n");
				render += 4;
			}
			else if (render[1] == 'B' && render[2] == 'R' && render[3] == '>') {
				renderText.Append("\n");
				render += 4;
			}
			else {

				if (render[0] == '\n') {
					if (usenewline) {
						ctmp[0] = render[0];
						ctmp[1] = 0;
						renderText.Append(ctmp);
					}
				}
				else {
					ctmp[0] = render[0];
					ctmp[1] = 0;
					renderText.Append(ctmp);
				}
				render++;

			}

		}
		else {
			//if (render[0] == '\r')
			//	ctmp[0] = '\n';
			//else
			if(render[0] == '\n') {
				if (usenewline) {
					ctmp[0] = render[0];
					ctmp[1] = 0;
					renderText.Append(ctmp);
				}
			}
			else {
				ctmp[0] = render[0];
				ctmp[1] = 0;
				renderText.Append(ctmp);
			}
			render++;
		}
	}

	if (strlen(renderText) > 0) {
		renderTheText(renderText, entercodemode, seperator);
		seperator = false;
		renderText = "";
	}

	fDeepButScrollY[iCurrentDeepBut] = ImGui::GetScrollY();

	bHTMLfirstRun = false;
	style_colors[ImGuiCol_Text] = oldTextColor;
	ImGui::SetWindowFontScale(1.0);
	ImGui::EndChild();
	//ImGui::EndColumns();
	

	//ImGui::TextWrapped(cHelpPage);

	//"Help/home.html"

}

