// Generator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cFile.h"
#include "cFileList.h"

//#define MAX_SIZE	1024
#define MAX_SIZE	17000
#define DB_PRO		0
#define DARK_GDK	1

struct sTemplate
{
	std::vector < char* > contentList;
};

enum eType
{
	eHeader,
	eSubHeader,
	eText,
	eList,
	eCode,
	eListBegin,
	eListEnd,
	eBeginTier1,
	eEndTier1,
	eBeginTier2,
	eEndTier2,
	eImage,
	eImageClick,
	eNavigation,
	eNavigationBegin,
	eNavigationEnd,
};

struct sType
{
	eType type;
	char  szText [ MAX_SIZE ];
};

sType	contents [ 4096 ];
int		iContentCount;
char	szFileHeader[ 4096 ];
char	szBreadCrumb[ 4096 ];

char g_szHome [ 256 ] = "";

void ParseDataFile    ( char* szFile );
void OutputFile       ( char* szFile );

void GenerateFiles ( char* szDirectory, char* szHome );

int _tmain ( int argc, _TCHAR* argv [ ] )
{
	SetCurrentDirectory ( "..\\AGK Txt" );

	SetCurrentDirectory ( "examples" );

	strcpy( szBreadCrumb, "<a href=\"../../examples.html\">Examples</a>" );

	//GenerateFiles ( "Files", "..\\..\\home.html" );
	GenerateFiles ( "Files", "../../" );
	GenerateFiles ( "Games", "" );
	GenerateFiles ( "General", "" );
	GenerateFiles ( "Input", "" );
	GenerateFiles ( "Multiplayer", "" );
	GenerateFiles ( "Particles", "" );
	GenerateFiles ( "Physics", "" );
	GenerateFiles ( "Sound", "" );
	GenerateFiles ( "Sprites", "" );
	GenerateFiles ( "Text", "" );
	GenerateFiles ( "Timers", "" );
	GenerateFiles ( "Benchmarking", "" );
	//GenerateFiles ( "User", "" );

	SetCurrentDirectory ( "..\\" );

	strcpy( szBreadCrumb, "<a href=\"../guides.html\">Guides</a>" );
	GenerateFiles ( "Guides", "../" );

	strcpy( szBreadCrumb, "<a href=\"../principles.html\">Principles</a>" );
	GenerateFiles ( "Principles", "" );

	strcpy( szBreadCrumb, "<a href=\"../utilities.html\">Utilities</a>" );
	GenerateFiles ( "Utilities", "" );

	strcpy( szBreadCrumb, "<a href=\"../language.html\">Language</a>" );
	GenerateFiles ( "Language", "" );

	//strcpy( szBreadCrumb, "<a href=\"../tier_1.html\">Tier 1</a>" );
	//GenerateFiles ( "Tier1", "" );

	return 0;
}

void GenerateFiles ( char* szDirectory, char* szHome )
{
	if ( !SetCurrentDirectory ( szDirectory ) )
	{
		char szErr[1024];
		sprintf( szErr, "Missing folder: %s", szDirectory );
		MessageBox( NULL, szErr, "Help Generator Error", 0 );
		exit(1);
	}

	if ( strlen ( szHome ) > 1 ) strcpy ( g_szHome, szHome );

	cFileList fileList;
	fileList.Search ( "*.txt" );

	//DWORD dwCount = fileList.FindMatchingFiles ( "txt" );

	for ( int i = 0; i < fileList.GetCount ( ); i++ )
	{
		char		szOutput [ 256 ]	= "";
		char*		szFile				= fileList.GetName ( i );
		int			iLength				= strlen ( szFile );

		strcpy ( szOutput, szFile );

		szOutput [ iLength - 1 ] = 'm';
		szOutput [ iLength - 2 ] = 't';
		szOutput [ iLength - 3 ] = 'h';

		if ( strcmp( szFile, "keywordlinks.txt" ) == 0 || strcmp( szFile, "corekeywordlinks.txt" ) == 0 )
		{
			continue;
		}

		ParseDataFile    ( szFile );
		OutputFile       ( szOutput );
	}

	SetCurrentDirectory ( "..\\" );

}

void ParseDataFile ( char* szFile )
{
	cFile file ( szFile, "\n" );
	int   iToken = 0;

	int match = strcmp( szFile, "tier2_state_machine.txt" );

	iContentCount = 0;

	bool bHeaderFound = false;
	strcpy( szFileHeader, "" );

	while ( iToken < file.GetTokenCount ( ) )
	{
		char* szToken = file.GetToken ( iToken );

		bool bParseTier1 = false;
		bool bParseTier2 = false;
		bool bSkip       = true;

		if ( *szToken == (char)0xEF ) szToken++;
		if ( *szToken == (char)0xBB ) szToken++;
		if ( *szToken == (char)0xBF ) szToken++;

		if ( *szToken != '_' ) 
		{
			char errStr[1024];
			sprintf( errStr, "Invalid token \"%s\" in file \"%s\". Make sure all tokens before this are formatted correctly", szToken, szFile );
			MessageBox( NULL, errStr, "Error", 0 );
			exit(0);
		}

		if ( strstr ( szToken, "_header"  ) ) contents [ iContentCount ].type = eHeader;
		if ( strstr ( szToken, "_sheader" ) ) contents [ iContentCount ].type = eSubHeader;
		if ( strstr ( szToken, "_text"    ) ) contents [ iContentCount ].type = eText;

		if ( strstr ( szToken, "_image"   ) ) contents [ iContentCount ].type = eImage;
		if ( strstr ( szToken, "_imageclick"   ) ) contents [ iContentCount ].type = eImageClick;

		if ( strstr ( szToken, "_navigation" ) ) contents [ iContentCount ].type = eNavigation;

		

		if ( strstr ( szToken, "_listbegin" ) )
		{
			contents [ iContentCount++ ].type = eListBegin;
			iToken++;

			while ( 1 )
			{
				char* szToken = file.GetToken ( iToken );

				if ( strstr ( szToken, "_listend" ) )
				{
					contents [ iContentCount++ ].type = eListEnd;
					iToken++;
					bSkip = false;
					break;
				}
				else
				{
					contents [ iContentCount ].type = eList;
					strcpy ( contents [ iContentCount++ ].szText, file.GetToken ( iToken++ ) );
				}
			}
		}
		
		if ( strstr ( szToken, "_tier1" ) )
			bParseTier1 = true;

		if ( strstr ( szToken, "_tier2" ) )
			bParseTier2 = true;

		if ( bParseTier1 || bParseTier2 )
		{
			if ( bParseTier1 ) contents [ iContentCount++ ].type = eBeginTier1;
			if ( bParseTier2 ) contents [ iContentCount++ ].type = eBeginTier2;
			
			iToken++;

			while ( 1 )
			{
				char* szToken = file.GetToken ( iToken );

				if ( strstr ( szToken, "_tier1" ) || strstr ( szToken, "_tier2" ) )
				{
					if ( bParseTier1 ) contents [ iContentCount++ ].type = eEndTier1;
					if ( bParseTier2 ) contents [ iContentCount++ ].type = eEndTier2;

					bParseTier1 = false;
					bParseTier2 = false;
					
					break;
				}
				else
				{
					contents [ iContentCount ].type = eCode;
					strcpy ( contents [ iContentCount++ ].szText, file.GetToken ( iToken++ ) );
				}
			}
		}
		else
		{
			if ( bSkip )
			{
				iToken++;
				if ( !bHeaderFound )
				{
					if ( strstr ( szToken, "_header"  ) )
					{
						bHeaderFound = true;
						strcpy( szFileHeader, file.GetToken ( iToken ) );
					}
				}
				strcpy ( contents [ iContentCount++ ].szText, file.GetToken ( iToken ) );
			}
		}

		if ( bSkip )
			iToken++;
	}
}

// header
const char* szLine1 = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n<html>\n<head>\n<title>";
const char* szLine2 = " - AGK Help</title>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n<script type=\"text/javascript\" src=\"";
const char* szLine3 = "main.js\"></script>\n<link href=\"";
const char* szLine4 = "main.css\" rel=\"styleSheet\" type=\"text/css\" />\n<link href=\"";

const char* szLine4b = "prettify.css\" rel=\"styleSheet\" type=\"text/css\" />\n<script type=\"text/javascript\" src=\"";
const char* szLine4c = "prettify.js\"></script>\n<script type=\"text/javascript\" src=\"";
const char* szLine4d = "lang-agk.js\"></script>\n";
//const char* szLine4d = "lang-agk.js\"></script>\n</head>\n<body>\n";

const char* szLine4a1 = "<link rel=\"stylesheet\" href=\"../lightbox/css/lightbox.min.css\">\n";
const char* szLine4a2 = "<script src=\"../lightbox/js/lightbox-plus-jquery.min.js\"></script>\n<script src=\"../clipboard/clipboard.min.js\"></script>\n";
const char* szLine4a3 = "<script type=\"text/javascript\" src=\"https://www.appgamekit.com/assets/js/docs-jquery.min.js\"></script>\n";
const char* szLine4a4 = "<script type=\"text/javascript\" src=\"https://www.appgamekit.com/assets/js/docs-translate.js\"></script>\n";
const char* szLine4a5 = "</head>\n<body>\n";

const char* szLine4a6 = "<script>var clipboard = new Clipboard('.btn');\nclipboard.on('success', function(e)\n{\nconsole.log(e);\n});\nclipboard.on('error', function(e)\n{\nconsole.log(e);\n});\n</script>\n";

const char* szLine5 = "<div class=\"header\" style=\"background: #000 url('";
const char* szLine6 = "gfx/header-back.png') top center no-repeat;\"><div><div class=\"navigation\"><a href=\"";
const char* szLine7 = "home.html\"><img src=\"";
const char* szLine8 = "gfx/b-home.png\" width=\"76\" height=\"44\" alt=\"\"></a>";
const char* szLine9 = "<a href=\"javascript:history.go(-1)\"><img src=\"";
const char* szLine10 = "gfx/b-back.png\" width=\"71\" height=\"44\" alt=\"\"></a><a href=\"javascript:window.print()\"><img src=\"";
const char* szLine11 = "gfx/b-print.png\" width=\"75\" height=\"44\" alt=\"\"></a><a style=\"cursor:pointer;\" onclick=\"ToggleCPP()\"><img src=\"";
const char* szLine12 = "gfx/lang-c.png\" width=\"253\" height=\"44\" alt=\"\"  id=\"basicCodeBlock0\" border=\"0\"></a><a style=\"cursor:pointer;\" onclick=\"ToggleCPP()\"><img src=\"";
const char* szLine13 = "gfx/lang-basic.png\" width=\"253\" height=\"44\" alt=\"\"  id=\"cppCodeBlock0\" style=\"display:none\" border=\"0\"></a></div><a href=\"https://www.appgamekit.com\" target=\"_blank\"><img class=\"logo\" src=\"";
const char* szLine14 = "gfx/agk-logo.png\" height=\"44\" style=\"margin:15px 0\" alt=\"App Game Kit\" border=\"0\"></a></div><img src=\"";
const char* szLine15 = "gfx/drop-shadow.png\" width=\"1158\" height=\"40\" alt=\"\"></div>";

const char* szLine16 = "<div class=\"breadcrumbs\">\n<a href=\"";
const char* szLine17 = "home.html\">Home</a> &gt; ";
const char* szLine18 = " &gt; ";
const char* szLine19 = "</div><div class=\"page\">";

// footer
const char* szLine20 = "<br><br></div><div class=\"footer\" style=\"background: #000 url('";
const char* szLine21 = "gfx/footer-back.png') top center no-repeat;\"><div><p><a href=\"https://www.thegamecreators.com\" target=\"_blank\" style=\"float:none\"><img style=\"float:right;margin-top:2px\" src=\"";
const char* szLine22 = "gfx/tgc-logo.png\" width=\"188\" height=\"27\" alt=\"\" border=\"0\"></a><br><strong>App Game Kit</strong> &nbsp;&copy; The Game Creators Ltd. All Rights Reserved.&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:agkreports@thegamecreators.com\" style=\"float:none\"><font color=\"FFFFFF\">Report an issue</font></a></p></p></div></div>";

void OutputFile ( char* szFile )
{
	FILE* fp = fopen ( szFile, "wb" );

	char szText [ MAX_SIZE ] = "";

	int stop = strncmp( szFile, "corekeyword", strlen("corekeyword") );

	// header
	fwrite ( szLine1, strlen(szLine1), 1, fp );
	fwrite ( szFileHeader, strlen(szFileHeader), 1, fp );

	fwrite ( szLine2, strlen(szLine2), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine3, strlen(szLine3), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine4, strlen(szLine4), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine4b, strlen(szLine4b), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine4c, strlen(szLine4c), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine4d, strlen(szLine4d), 1, fp );

	fwrite ( szLine4a1, strlen(szLine4a1), 1, fp );
	fwrite ( szLine4a2, strlen(szLine4a2), 1, fp );
	fwrite ( szLine4a3, strlen(szLine4a3), 1, fp );
	fwrite ( szLine4a4, strlen(szLine4a4), 1, fp );
	fwrite ( szLine4a5, strlen(szLine4a5), 1, fp );
	fwrite ( szLine4a6, strlen(szLine4a6), 1, fp );

	fwrite ( szLine5, strlen(szLine5), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	

	fwrite ( szLine6, strlen(szLine6), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine7, strlen(szLine7), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine8, strlen(szLine8), 1, fp );
	fwrite ( szLine9, strlen(szLine9), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine10, strlen(szLine10), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine11, strlen(szLine11), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine12, strlen(szLine12), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine13, strlen(szLine13), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine14, strlen(szLine14), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine15, strlen(szLine15), 1, fp );
	fwrite ( szLine16, strlen(szLine16), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine17, strlen(szLine17), 1, fp );
	fwrite ( szBreadCrumb, strlen(szBreadCrumb), 1, fp );

	fwrite ( szLine18, strlen(szLine18), 1, fp );
	fwrite ( szFileHeader, strlen(szFileHeader), 1, fp );
	fwrite ( szLine19, strlen(szLine19), 1, fp );

	unsigned int iBeginTier1 = 0;
	unsigned int iEndTier1   = 0;
	
	// page content
	int iCodeBlock = 1;
	for ( int i = 0; i < iContentCount; i++ )
	{
		switch ( contents [ i ].type )
		{
			case eHeader:
			{
				sprintf ( szText, "<h1>%s</h1>", contents [ i ].szText );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eSubHeader:
			{
				sprintf ( szText, "<h2>%s</h2>", contents [ i ].szText );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eImage:
			{
				sprintf ( szText, "<div align=\"center\"><img src=\"%s\"/></div>", contents [ i ].szText );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eImageClick:
			{
				char szImageLarge  [ 256 ] = "";
				char szImageNormal [ 256 ] = "";

				sprintf ( szImageNormal, "%s", contents [ i ].szText );
				memcpy ( &szImageLarge [ 0 ], &contents [ i ].szText [ 0 ], ( strlen ( contents [ i ].szText ) - 4 ) * sizeof ( char ) );
				strcat ( szImageLarge, "_large.jpg" );
				sprintf ( szText, "<div align=\"center\"><a class=\"example-image-link\" href=\"%s\" data-lightbox=\"example-1\"><img class=\"example-image\" src=\"%s\" alt=\"image-1\" /></a></div>", szImageLarge, szImageNormal );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eText:
			{
				sprintf ( szText, "<p>%s</p>", contents [ i ].szText );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eListBegin:
			{
				strcpy ( szText, "<ul>" );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eNavigation:
			{
				std::vector < char* > lines;

				char szLine [ 256 ] = "";
				strcpy ( szLine, contents [ i ].szText );

				char delims [ ] = " ";
				char* result = NULL;
				result = strtok ( &szLine [ 0 ], delims );
				
				while ( result != NULL )
				{
					lines.push_back ( result );
					
					result = strtok ( NULL, delims );
				}

				if ( lines.size ( ) == 1 )
					sprintf ( szText, "<div align=\"center\"><a href=\"%s\"><img src=\"../gfx/next.png\"  alt=\"\"></a></div>", lines [ 0 ] );
				else
					sprintf ( szText, "<div align=\"center\"><a href=\"%s\"><img src=\"../gfx/prev.png\"  alt=\"\"></a> &nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;<a href=\"%s\"><img src=\"../gfx/next.png\"  alt=\"\"></a></div>", lines [ 0 ], lines [ 1 ] );
				

				//sprintf ( szText, "<li>%s</li>", contents [ i ].szText );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eList:
			{
				sprintf ( szText, "<li>%s</li>", contents [ i ].szText );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eListEnd:
			{
				strcpy ( szText, "</ul>" );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eBeginTier1:
			{
				

				sprintf ( szText, "<div id=\"basicCodeBlock%i\" style=\"border: 1px solid #ccd; background: #f3f3fc; margin: 10px 0;\">", iCodeBlock++ );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				strcpy ( szText, "<pre class=\"prettyprint lang-agk\" style=\"margin:14px; font-size: 14px; white-space:pre-wrap;\">" );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				iBeginTier1 = i + 1;	

				//sprintf ( szText, "<pre style=\"width: 500px; border: 1px;\">" );

				//sprintf ( szText, "<pre style=\"white-space: pre-wrap; /* css-3 */ white-space: -moz-pre-wrap !important; white-space: -pre-wrap; white-space: -o-pre-wrap; word-wrap: break-word;width: 99%;\">" );
				
				//fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eBeginTier2:
			{
				sprintf ( szText, "<div id=\"cppCodeBlock%i\" style=\"display:none; border: 1px solid #ccd; background: #f3f3fc; margin: 10px 0;\">", iCodeBlock++ );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				strcpy ( szText, "<pre style=\"margin:14px; font-size: 14px; white-space:pre-wrap;\" class=\"prettyprint lang-cpp\">" );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				//sprintf ( szText, "<pre style=\"width: 500px; border: 1px;\">" );
				//sprintf ( szText, "<pre style=\"white-space: pre-wrap; /* css-3 */ white-space: -moz-pre-wrap !important; white-space: -pre-wrap; white-space: -o-pre-wrap; word-wrap: break-word;width: 99%;\">" );
				
				//fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eCode:
			{
				
				sprintf ( szText, "%s\n", contents [ i ].szText );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eEndTier1:
			{
				strcpy ( szText, "</pre></div>" );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				iEndTier1 = i;

				char szCopy [ 8096 ] = "";

				if ( strcmp ( szFile, "variables.htm" ) == 0 )
					int k = 0;
				
				unsigned int m = 0;
				
				for ( unsigned int k = iBeginTier1; k < i; k++ )
				{
					

					if ( strstr ( contents [ k ].szText, "<br>" ) )
					{
						strcat ( szCopy, "\n" );
						m++;
						continue;
					}

					for ( unsigned int j = 0; j < strlen ( contents [ k ].szText ); j++ )
					{
						if ( contents [ k ].szText [ j ] == '"' )
						{
							strcat ( szCopy, "&quot;" );
							m += 6;
						}
						else
						{
							memcpy ( &szCopy [ m ], &contents [ k ].szText [ j ], sizeof ( char ) );
							m++;
						}
					}

					strcat ( szCopy, "\n" );

					m++;
				}

				//sprintf ( szCopy, "%s", "SetWindowTitle( \"First project\" )" );;

				sprintf ( szText, "<div align=\"right\"><button class=\"btn\" data-clipboard-text=\"%s\">Copy Code</button></div>", szCopy );

				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				break;
			}

			case eEndTier2:
			{
				strcpy ( szText, "</pre></div>" );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				break;
			}
		}
	}

	// footer
	fwrite ( szLine20, strlen(szLine20), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine21, strlen(szLine21), 1, fp );
	fwrite ( g_szHome, strlen(g_szHome), 1, fp );

	fwrite ( szLine22, strlen(szLine22), 1, fp );

	fwrite( "<script type=\"text/javascript\">prettyPrint(true);</script>", strlen("<script type=\"text/javascript\">prettyPrint(true);</script>"), 1, fp );

	fwrite ( "</body>", strlen("</body>"), 1, fp );
	fwrite ( "</html>", strlen("</html>"), 1, fp );
		
	fclose ( fp );
}

