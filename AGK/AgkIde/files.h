#ifndef _H_AGK_GUIFILES_
#define _H_AGK_GUIFILES_

#include "imgui/imgui.h"
#ifdef AGK_LINUX
#include <string>
#endif

#define FI_FOLDERISOPEN 1
class cFolderItem
{
public:
	struct sFolderFiles {
		sFolderFiles * m_dropptr; //Need to be the first entry for drag/drop.
		uString m_sName;
		uString m_sDropTarget;
		uString m_sPath;
		uString m_sFolder;
		UINT iFlags;
		int iPreview; //Preview image.
		int iBigPreview; //Preview image.
		int id;
		int iAnimationFrom = 0;
		bool bPreviewProcessed;
		sFolderFiles * m_pNext;
	};
	uString m_sFolder;
	uString m_sFolderFullPath;
	cFolderItem *m_pNext;
	cFolderItem *m_pSubFolder;
	sFolderFiles * m_pFirstFile;
	bool m_bFilesRead;
	bool visible;
	bool alwaysvisible;
	bool deletethisentry;
	bool bIsCustomFolder;
	char cfolder[256]; //PE: Only for faster sorting.
	time_t m_tFolderModify;
	float m_fLastTimeUpdate;
	UINT iFlags;
	int count;
	cFolderItem() { m_pNext = 0; iFlags = 0; m_bFilesRead = false; m_pFirstFile = NULL; m_pNext = NULL; m_pSubFolder = NULL; m_fLastTimeUpdate = 0; }
	~cFolderItem() { }
};

void CompileShader(const char* vertex, const char* fragment);
cFolderItem * getalldirs(cFolderItem *pFolder, cFolderItem *pStartFolder, char * path);
char * getFileCreationTime(char *filePath);
bool cp_copyfile(char *from, char *to);
bool cp_appendtofile(char *from, char *append);
int cp_realpath(char *path, char *resolvepath,bool folder);
long cp_getfilesize(char *filename);
int run_command(const char *szFilename, const char *szParameters, int *out_length, int *err_length, bool waittofinish);
int read_buffers(char *out_buffer, int out_length, char *err_buffer, int err_length);
int write_buffers(char *out_buffer, int out_length);
bool file_exists(const char * file);
char * read_eula(void);
char * g_file_get_contents(char *file, char *contents, int *length, char *unknown);
bool CopyDirTo(const char *source_folder, const char *target_folder);
uString GetGameGuruFolder(void);
uString GetAGKFolder(void);
void GetSteamFolder(void);
void CheckPlayBoxFolder(void);

#define MAXCOMMANDSIZE 100
#define MAXRETURNSIZE 30
#define MAXSYNTAXSIZE 330
#define MAXCATEGORYSIZE 100

struct KeywordHelp {
	char m_cCommand[MAXCOMMANDSIZE];
	char m_cLowerCommand[MAXCOMMANDSIZE];
	uString m_cCommandPath;
	uString m_cSyntax;
	uString m_cCategory;
	uString m_cReturn;
	uString m_HelpPage;
	KeywordHelp * m_pNext;
};

void processhelp(const char * page,bool readnewpage);
bool NoCaseLess(const std::string &a, const std::string &b);

#endif
