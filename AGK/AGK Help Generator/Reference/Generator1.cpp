// Generator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cFile.h"
#include "cFileList.h"

#include <iostream>
#include <string>
using namespace std;

#include "cHashedList.h"
using namespace AGK;

// SOURCE DATA

cHashedList<char> g_CombinedCategories(2048);
cHashedList<char> g_Categories(128);

int iTemplateSize = 0;
char* pTemplate = NULL;

int iTemplateSize_Index = 0;
char* pTemplate_Index = NULL;

struct sData
{
	char szLine [ 64 ] [ 256 ];
	int iLines;
};

sData g_Data [ 4000 ];
int g_iDataCount = 0;

// PARSED DATA

struct sParameter
{
	char szName [ 256 ];
	char szDescription [ 1024 ];
};

struct sListing
{
	char	   szListing [ 256 ];
	sParameter parameters [ 15 ];
	int        iCount;
};

struct sCommand
{
	bool bMarked;

	char szCategory [ 256 ];
	char szSubCategory [ 256 ];
	char szName     [ 256 ];

	char szDescription [ 8192 ];

	sListing listing [ 5 ];
	int iCount;
};

sCommand g_Commands [ 4000 ];
int g_iCommand = 0;

sCommand g_SortedCommands [ 4000 ];
int g_iSortedCommand = 0;


char g_szKeywordLinks [ 4000 ] [ 256 ];
int g_iKeywordCount = 0;

void ParseFile ( char* szFile )
{
	int iToken = 0;
	cFile file ( szFile, "\t\n" );

	int begin = g_iDataCount;

	while ( iToken < file.GetTokenCount ( ) )
	{
		sData* p = &g_Data [ g_iDataCount ];

		char* szToken = file.GetToken ( iToken++ );
		
		if ( strstr ( szToken, "****f*" ) )
		{
			int beginning = p->iLines;
			strcpy ( p->szLine [ p->iLines++ ], szToken );

			while ( 1 )
			{
				szToken = file.GetToken ( iToken++ );

				if ( strstr ( szToken, "****f*" ) )
				{
					char err[ 1024 ]; sprintf( err, "Missing //**** for command beginning \"%s\"", p->szLine[beginning] );
					MessageBox( NULL, err, "Error", 0 );
					exit(1);
				}

				if ( strstr ( szToken, "****" ) )
				{
					g_iDataCount++;
					break;
				}

				strcpy ( p->szLine [ p->iLines++ ], szToken );
			}
		}
	}

	// strip initial comment part
	for ( int i = begin; i < g_iDataCount; i++ )
	{
		sData* p = &g_Data [ i ];

		for ( int j = 0; j < p->iLines; j++ )
		{
			if ( strlen ( p->szLine [ j ] ) > 2 )
			{
				if ( p->szLine [ j ] [ 0 ] == '/' && p->szLine [ j ] [ 1 ] == '/' )
				{
					//p->szLine [ j ] [ 0 ] = ' ';
					//p->szLine [ j ] [ 1 ] = ' ';

					char szLine [ 256 ] = "";

					memcpy ( &szLine, &p->szLine [ j ] [ 2 ], strlen ( p->szLine [ j ] ) - 2 );

					strcpy ( p->szLine [ j ], szLine );
				}
			}
		}
	}
}

void ParseSourceFile ( void )
{
	// extract each line from the source file

	memset ( &g_Data, 0, sizeof ( g_Data ) );

	ParseFile ( "..\\..\\..\\common\\Source\\wrapper.cpp" );
	ParseFile ( "..\\..\\..\\platform\\windows\\Source\\WindowsCore.cpp" );
	ParseFile ( "..\\..\\..\\renderer\\OpenGL2\\OpenGL2.cpp" );
	
}

void GetCategory ( sData* s, sCommand* c )
{
	char* token = strtok ( s->szLine [ 0 ], " " );
	token = strtok ( 0, "/" );
	strcpy ( c->szCategory, token );

	token = strtok ( 0, "/" );
	strcpy ( c->szSubCategory, token );

	token = strtok ( 0, "/" );
	strcpy ( c->szName, token );
}

void GetName ( sData* s, sCommand* c )
{
	strcpy ( c->szName, s->szLine [ 1 ] );
}

void GetDescription ( sData* s, sCommand* c )
{
	char szDescription [ 8192 ] = "";

	bool bGo = false;

	int iLength = 0;

	for ( int i = 0; i < s->iLines; i++ )
	{
		for ( int j = 0; j < (int)strlen ( s->szLine [ i ] ) - 1; j++ )
		{
			if ( s->szLine [ i ] [ j ] == '/' && s->szLine [ i ] [ j + 1 ] == '/' )
			{
				s->szLine [ i ] [ j ] = ' ';
				s->szLine [ i ] [ j + 1 ] = ' ';
			}
		}
	}

	for ( int i = 0; i < s->iLines; i++ )
	{
		iLength += strlen ( s->szLine [ i ] );


		if ( strstr ( s->szLine [ i ], "FUNCTION" ) )
		{
			bGo = true;
			continue;
		}

		if ( bGo )
		{
			if ( strstr ( s->szLine [ i ], "INPUTS" ) )
				break;

			if ( strstr ( s->szLine [ i ], "SOURCE" ) )
				break;

			strcat ( szDescription, s->szLine [ i ] );
			strcat ( szDescription, " " );
		}
		
	}
	
	// strip any weird whitespace
	char* token = strtok ( szDescription, " " );

	while ( token )
	{
		strcat ( c->szDescription, token );
		strcat ( c->szDescription, " " );

		token = strtok ( 0, " " );
	}
}

void GetInputs ( sData* s, sCommand* c )
{
	char szParameters [ 1024 ] = "";

	int iStart = 0;

	for ( int i = 0; i < s->iLines; i++ )
	{
		if ( strstr ( s->szLine [ i ], "INPUTS" ) )
		{
			iStart = i + 1;
			break;
		}
	}

	if ( iStart == 0 )
		return;

	for ( int i = iStart; i < s->iLines; i++ )
	{
		if ( strstr ( s->szLine [ i ], "SOURCE" ) )
			break;

		strcpy ( szParameters, s->szLine [ i ] );
	
		char* token = strstr ( szParameters, " --" );
		if ( token )
		{
			char *szEnd = token;
			while ( *szEnd == ' ' ) szEnd--;
			szEnd++; *szEnd = 0;

			char *szStart = szParameters;
			while ( *szStart == ' ' ) szStart++;

			strcat ( c->listing [ 0 ].parameters [ c->listing [ 0 ].iCount ].szName, szStart );
			*szEnd = ' ';
			szEnd = token + 3;
			while ( *szEnd == ' ' ) szEnd++;

			strcat ( c->listing [ 0 ].parameters [ c->listing [ 0 ].iCount++ ].szDescription, szEnd );
		}
	}
}

void GetSource ( sData* s, sCommand* c )
{
	// last line is source
	strcpy ( c->listing->szListing, s->szLine [ s->iLines - 1 ] );
}

void FormatCommands ( void )
{
	// parse each command from the data
	for ( int i = 0; i < g_iDataCount; i++ )
	{
		sData* p = &g_Data [ i ];


		sCommand* c = &g_Commands [ i ];

		GetCategory    ( p, c );
		GetDescription ( p, c );
		GetInputs      ( p, c );
		GetSource      ( p, c );

		// change categories to more appropriate ones
		if ( strcmp ( c->szCategory, "AGKInput" ) == 0 )
			strcpy ( c->szCategory, "Input" );

		if ( strcmp ( c->szCategory, "Network" ) == 0 )
			strcpy ( c->szCategory, "Multiplayer" );

		if ( strcmp ( c->szCategory, "Joint" ) == 0 )
			strcpy ( c->szCategory, "2DPhysics" );

		if ( strcmp ( c->szCategory, "TextEntry" ) == 0 )
			strcpy ( c->szCategory, "Text" );

		if ( strcmp ( c->szCategory, "Touch" ) == 0 )
			strcpy ( c->szCategory, "Input" );

		if ( strcmp ( c->szCategory, "RayCast" ) == 0 )
			strcpy ( c->szCategory, "2DPhysics" );

		if ( strcmp ( c->szCategory, "String" ) == 0 )
			strcpy ( c->szCategory, "Core" );

		if ( strcmp ( c->szCategory, "Broadcast" ) == 0 )
			strcpy ( c->szCategory, "Multiplayer" );

		char *szCombined = new char[ strlen(c->szCategory)+strlen(c->szSubCategory)+2 ];
		strcpy( szCombined, c->szCategory );
		strcat( szCombined, "/" );
		strcat( szCombined, c->szSubCategory );

		// store combined category if unique
		if ( g_CombinedCategories.GetItem( szCombined ) ) delete [] szCombined;
		else g_CombinedCategories.AddItem( szCombined, szCombined );

		// do the same for the main category
		if ( g_Categories.GetItem( c->szCategory ) == 0 )
		{
			char *szMainCategory = new char[ strlen(c->szCategory)+1 ];
			strcpy( szMainCategory, c->szCategory );
			g_Categories.AddItem( szMainCategory, szMainCategory );
		}
	}
}

sCommand temp [ 250 ];
int iTempCount = 0;

int CompareNames ( const void* a, const void* b ) 
{ 
    sCommand* commandA = ( sCommand* ) a;
    sCommand* commandB = ( sCommand* ) b;

	return strcmp ( commandA->szName, commandB->szName );
} 

void CreateListingForSection ( char* szCategory, char* szSubCategory )
{
	iTempCount = 0;	

	memset ( &temp, 0, sizeof ( temp ) );

	for ( int i = 0; i < g_iDataCount; i++ )
	{
		sCommand* p = &g_Commands [ i ];

		if ( strcmp ( p->szCategory, szCategory ) == 0 )
		{
			if ( strcmp ( p->szSubCategory, szSubCategory ) == 0 )
			{
				memcpy ( &temp [ iTempCount ], p, sizeof ( sCommand ) );
				iTempCount++;
			}
		}
	}

	qsort ( temp, iTempCount, sizeof ( sCommand ), CompareNames );

	for ( int i = 0; i < iTempCount; i++ )
		memcpy ( &g_SortedCommands [ g_iSortedCommand++ ], &temp [ i ], sizeof ( sCommand ) );
}

const char* szLine1 = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n<html>\n<head>\n<title>";
const char* szLine2 = " - AGK Help</title>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n<script type=\"text/javascript\" src=\"";
const char* szLine3 = "main.js\"></script>\n<script type=\"text/javascript\">var commandName = \"";
const char* szLine3b = "\";</script>\n<link href=\"";
const char* szLine4 = "main.css\" rel=\"styleSheet\" type=\"text/css\" /><script type=\"text/javascript\">window.addEventListener(\"message\", receiveMessage, false);function receiveMessage(event){if( event.origin !== 'http://www.appgamekit.com' && event.origin !== 'https://www.appgamekit.com' && event.origin !== 'http://appgamekit.com' && event.origin !== 'https://appgamekit.com' && event.origin !== 'http://agk.thegamecreators.com' && event.origin !== 'https://agk.thegamecreators.com' ) return;document.getElementById('comments').style.height = (parseInt(event.data))+'px'; document.getElementById('comments').scrolling='no';}</script>\n";
const char* szLine4a = "<script type=\"text/javascript\" src=\"https://www.appgamekit.com/assets/js/docs-jquery.min.js\"></script>";
const char* szLine4b = "<script type=\"text/javascript\" src=\"https://www.appgamekit.com/assets/js/docs-translate.js\"></script>";
const char* szLine4c = "</head>\n<body>\n";
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
const char* szLine17 = "home.html\">Home</a> &gt; <a href=\"";
const char* szLine18 = "commands.html\">Commands</a> &gt; ";
const char* szLine19 = "</div><div class=\"page\">";

// footer
const char* szLine20 = "<br><br>";
const char* szLine20b = "<div style=\"height:2px;width:100%;border-bottom:1px solid #b0b0b0;margin:0px auto 10px auto;padding:0px;\"> </div><iframe id=\"comments\" style=\"width:700px;margin:0px auto;border:none\" src=\"https://www.appgamekit.com/help/comments?command=";
const char* szLine20c = "\"></iframe>";
const char* szLine20d = "</div><div class=\"footer\" style=\"background: #000 url('";
const char* szLine21 = "gfx/footer-back.png') top center no-repeat;\"><div><p><a href=\"https://www.thegamecreators.com\" target=\"_blank\" style=\"float:none\"><img style=\"float:right;margin-top:2px\" src=\"";
const char* szLine22 = "gfx/tgc-logo.png\" width=\"188\" height=\"27\" alt=\"\" border=\"0\"></a><br><strong>App Game Kit</strong> &nbsp;&copy; The Game Creators Ltd. All Rights Reserved.&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:agkreports@thegamecreators.com\" style=\"float:none\"><font color=\"FFFFFF\">Report an issue</font></a></p></p></div></div>";

void WriteHTMLHeader( FILE* fp, const char* szTitle, const char* szBreadCrumb, const char* szPathToHome )
{
	fwrite ( szLine1, strlen(szLine1), 1, fp );
	fwrite ( szTitle, strlen(szTitle), 1, fp );

	fwrite ( szLine2, strlen(szLine2), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine3, strlen(szLine3), 1, fp );
	fwrite ( szTitle, strlen(szTitle), 1, fp );
	
	fwrite ( szLine3b, strlen(szLine3b), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine4, strlen(szLine4), 1, fp );
	fwrite ( szLine4a, strlen(szLine4a), 1, fp );
	fwrite ( szLine4b, strlen(szLine4b), 1, fp );
	fwrite ( szLine4c, strlen(szLine4c), 1, fp );

	fwrite ( szLine5, strlen(szLine5), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine6, strlen(szLine6), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine7, strlen(szLine7), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine8, strlen(szLine8), 1, fp );
	fwrite ( szLine9, strlen(szLine9), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine10, strlen(szLine10), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine11, strlen(szLine11), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine12, strlen(szLine12), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine13, strlen(szLine13), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine14, strlen(szLine14), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine15, strlen(szLine15), 1, fp );
	fwrite ( szLine16, strlen(szLine16), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine17, strlen(szLine17), 1, fp );
	fwrite ( szPathToHome, strlen(szPathToHome), 1, fp );

	fwrite ( szLine18, strlen(szLine18), 1, fp );
	if ( szBreadCrumb ) fwrite ( szBreadCrumb, strlen(szBreadCrumb), 1, fp );
	fwrite ( szTitle, strlen(szTitle), 1, fp );
	fwrite ( szLine19, strlen(szLine19), 1, fp );
}

void WriteListing ( const char* szCategory )
{
	
	char szFile [ 8192 ] = "";

	sprintf ( szFile, "%s.htm", szCategory );

	FILE* fp = fopen ( szFile, "wb" );
	
	WriteHTMLHeader( fp, szCategory, 0, "../" );

	// make big listings
	sprintf ( szFile, "<h1>%s</h1>", szCategory );
	fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

	if ( strcmp ( "Input-Raw", szCategory ) == 0 )
	{
		sprintf ( szFile, "<p>The commands listed in this section are for dealing with specific hardware requirements - they are not cross platform. You must check for existance on the current device before using them.</p>" );
		fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
	}

	if ( strcmp ( "Input", szCategory ) == 0 )
	{
		sprintf ( szFile, "<p>The commands listed in this section are guaranteed to function in some way on all platforms, those not supported natively are emulated.</p>" );
		fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
	}

	if ( strcmp ( "Music", szCategory ) == 0 )
	{
		sprintf ( szFile, "<p>The old music commands have been deprecated in favour of the new OGG music commands (except on HTML5). The new commands require files compressed with OGG Vorbis, but will work on all platforms, support multiple music files playing at the same time, and allow seamless looping.</p><p>On HTML5 the old music commands have better performance than the newer commands.</p>" );
		fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
	}

	char szOld [ 256 ] = "";

	for ( int i = 0; i < g_iSortedCommand; i++ )
	{
	
		sCommand* p = &g_SortedCommands [ i ];

		if ( p->bMarked )
			continue;

		if ( strcmp ( p->szName, "GetSpritePtr" ) == 0 )
			continue;

		if ( strcmp ( p->szCategory, szCategory ) == 0 )
		{
			if ( strcmp ( szOld, p->szSubCategory ) != 0 )
			{
				if ( strlen( szOld ) > 0 ) fwrite ( "</ul>", strlen( "</ul>" ), 1, fp );

				sprintf ( szFile, "<h2>%s</h2>", p->szSubCategory );
				fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

				/*
				if ( strcmp ( "Platform Specific", p->szSubCategory ) == 0 )
				{
					sprintf ( szFile, "<p>The commands listed in this section are for dealing with specific hardware requirements - they are not cross platform.</p>" );
					fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
				}
				*/

				strcpy ( szOld, p->szSubCategory );

				fwrite ( "<ul>", strlen( "<ul>" ), 1, fp );
			}

			sprintf ( szFile, "<li><a href=\"%s/%s.htm\">%s</a></li>", p->szCategory, p->szName, p->szName );
			fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
		}
	}

	sprintf ( szFile, "</ul>" );
	fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

	// footer
	fwrite ( szLine20, strlen(szLine20), 1, fp );
	fwrite ( szLine20d, strlen(szLine20d), 1, fp );
	fwrite ( "../", strlen("../"), 1, fp );

	fwrite ( szLine21, strlen(szLine21), 1, fp );
	fwrite ( "../", strlen("../"), 1, fp );

	fwrite ( szLine22, strlen(szLine22), 1, fp );

	fwrite ( "</body>", strlen("</body>"), 1, fp );
	fwrite ( "</html>", strlen("</html>"), 1, fp );
		
	fclose ( fp );
}

void WriteCommandList ( const char* szCategory )
{
	
	char szFile [ 8192 ] = "";

	sprintf ( szFile, "%s Commands.txt", szCategory );

	FILE* fp = fopen ( szFile, "wb" );
	
	char szOld [ 256 ] = "";

	for ( int i = 0; i < g_iSortedCommand; i++ )
	{
	
		sCommand* p = &g_SortedCommands [ i ];

		if ( p->bMarked )
			continue;

		if ( strcmp ( p->szName, "GetSpritePtr" ) == 0 )
			continue;

		if ( strcmp ( szCategory, "Music" ) == 0 && strstr( p->szName, "OGG" ) == 0 )
			continue;

		if ( strcmp ( p->szCategory, szCategory ) == 0 )
		{
			if ( strcmp ( szOld, p->szSubCategory ) != 0 )
			{
				fputs( "\n", fp );
				fputs( p->szSubCategory, fp );
				fputs( "\n\n", fp );

				strcpy ( szOld, p->szSubCategory );
			}


			for ( int j = 0; j < p->iCount; j++ )
			{
				char szOriginal [ 256 ] = "";
				char szTemp     [ 256 ] = "";
				char szTier1    [ 256 ] = "";

				strcpy ( szOriginal, p->listing [ j ].szListing );

				char* token = strtok ( szOriginal, " " );

				while ( token )
				{
					if ( 
							strcmp ( token, "void" ) != 0 && 
							strcmp ( token, "UINT" ) != 0 && 
							strcmp ( token, "float" ) != 0 &&
							strcmp ( token, "const" ) != 0 &&
							strcmp ( token, "char" ) != 0 &&
							strcmp ( token, "char*" ) != 0 &&
							strcmp ( token, "int" ) != 0 &&
							strcmp ( token, "unsigned" ) != 0 &&
							strcmp ( token, "DWORD" ) != 0 &&
							strcmp ( token, "cImage*" ) != 0 &&
							strcmp ( token, "uint32_t" ) != 0 
					)
					{
						if ( token [ 0 ] == '*' )
							token [ 0 ] = ' ';

						strcat ( szTemp, token );
						strcat ( szTemp, " " );
					}

					token = strtok ( 0, " " );
				}

				strcpy( szTier1, szTemp+5 );

				// need to add basic block or cpp block around each listing
				strcpy ( szOriginal, p->listing [ j ].szListing );
				token = strtok ( szOriginal, " " );
				char szReturn [ 256 ] = "";

				if ( strcmp ( token, "float" ) == 0 ) strcpy ( szReturn, "float" );
				if ( strcmp ( token, "UINT" ) == 0 ) strcpy ( szReturn, "integer" );
				if ( strcmp ( token, "char" ) == 0 ) strcpy ( szReturn, "string" );
				if ( strcmp ( token, "char*" ) == 0 ) strcpy ( szReturn, "string" );
				if ( strcmp ( token, "int" ) == 0 ) strcpy ( szReturn, "integer" );
				if ( strcmp ( token, "unsigned" ) == 0 ) strcpy ( szReturn, "integer" );
				if ( strcmp ( token, "uint32_t" ) == 0 ) strcpy ( szReturn, "integer" );

				fputs ( szReturn, fp );
				if ( *szReturn ) fputs ( " ", fp );
				fputs ( szTier1, fp );
				fputs ( "\n", fp );
			}
		}
	}
		
	fclose ( fp );
}

#include <time.h>

int _tmain ( int argc, _TCHAR* argv [ ] )
{
	
	ParseSourceFile ( );
	FormatCommands  ( );

	// new dynamic method of creating listings
	char szMainCat[ 128 ];
	char szSubCat[ 128 ];
	const char *szCombined = g_CombinedCategories.GetFirst();
	while( szCombined )
	{
		const char* pos = strchr( szCombined, '/' );
		if ( pos )
		{
			strcpy( szSubCat, pos+1 );
			strncpy( szMainCat, szCombined, (UINT)(pos-szCombined) );
			szMainCat[ (UINT)(pos-szCombined) ] = 0;

			CreateListingForSection( szMainCat, szSubCat );
		}
		
		delete [] szCombined;
		szCombined = g_CombinedCategories.GetNext();
	}

	g_CombinedCategories.ClearAll();
		
	for ( int i = 0; i < g_iDataCount; i++ )
		g_SortedCommands [ i ].iCount = 1;

	

	// remove dupliactes
	for ( int j = 0; j < g_iDataCount; j++ )
	{
		sCommand* a = &g_SortedCommands [ j ];

		if ( a->bMarked ) continue;
			
		for ( int i = 0; i < g_iDataCount; i++ )
		{
			if ( j == i ) continue;

			sCommand* b = &g_SortedCommands [ i ];

			//if ( b->bMarked ) continue;
			
			if ( strcmp ( b->szName, a->szName ) == 0 && strcmp(b->szSubCategory, a->szSubCategory) == 0 && strcmp(b->szCategory, a->szCategory) == 0 )
			{
				b->bMarked = true;
				memcpy ( &a->listing [ a->iCount++ ], &b->listing [ 0 ], sizeof ( sListing ) );
			}
		}
	}

	// new dynamic method for creating main listings
	const char *szMainCat2 = g_Categories.GetFirst();
	while( szMainCat2 )
	{
		WriteListing( szMainCat2 );
		//WriteCommandList( szMainCat2 );
		
		delete [] szMainCat2;
		szMainCat2 = g_Categories.GetNext();
	}

	g_Categories.ClearAll();

	/*
	WriteListing ( "Benchmarking" );
	WriteListing ( "Core" );
	WriteListing ( "Error" );
	WriteListing ( "File" );
	WriteListing ( "Image" );
	WriteListing ( "Input" );
	WriteListing ( "Multiplayer" );
	WriteListing ( "Music" );
	WriteListing ( "Particles" );
	WriteListing ( "2DPhysics" );
	WriteListing ( "Sound" );
	WriteListing ( "Sprite" );
	WriteListing ( "Text" );
	WriteListing ( "HTTP" );
	WriteListing ( "Time" );
	WriteListing ( "Advert" );
	WriteListing ( "Memblock" );
	WriteListing ( "Extras" );
	WriteListing ( "3D" );
	WriteListing ( "Video" );
	*/

	FILE* keywordFile1 = fopen("commands1.txt","wb");
	FILE* keywordFile2 = fopen("commands2.txt","wb");
	FILE* keywordFile3 = fopen("commands3.txt","wb");
	FILE* keywordFile5 = fopen("commands4.txt","wb");
	FILE* keywordFile6 = fopen("commandsIDE.txt","wb");
	FILE* keywordFile4 = fopen("commandsSQL.txt","wb");
	int keywordFile6Count = 0;
	char strLower[256];

	for ( int i = 0; i < g_iDataCount; i++ )
	{
		// get pointer to command
		sCommand* p = &g_SortedCommands [ i ];



		char szFile [ 8192 ] = "";

		if ( strlen ( p->szCategory ) == 0 )
			continue;

		if ( strcmp ( p->szName, "GetSpritePtr" ) == 0 )
			continue;

		if ( p->bMarked )
			continue;

		// make a directory for the command
		if ( !SetCurrentDirectory ( p->szCategory ) )
		{
			CreateDirectory ( p->szCategory, NULL );
			SetCurrentDirectory ( p->szCategory );
		}

		sprintf ( szFile, "%s.htm", p->szName );

		FILE* fp = fopen ( szFile, "wb" );

		sprintf ( szFile, "<a href=\"../%s.htm\">%s</a> &gt; ", p->szCategory, p->szCategory );
		WriteHTMLHeader( fp, p->szName, szFile, "../../" );

		sprintf ( szFile, "<h1>%s</h1>", p->szName );
		fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

		sprintf ( szFile, "<h2>Description</h2>"  );
		fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

		sprintf ( szFile, "<p>%s</p>", p->szDescription );
		fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

		sprintf ( szFile, "<h2>Definition</h2>"  );
		fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

		// TIER2 this is ok - void agk::AddParticlesColorKeyFrame( UINT ID, float time, UINT red, UINT green, UINT blue, UINT alpha )
		// TIER1 needs to be - AddParticlesColorKeyFrame( ID, time, red, green, blue, alpha )

		fwrite( "\"", 1, 1, keywordFile1 );
		fwrite( p->szName, strlen(p->szName), 1, keywordFile1 );
		fwrite( "\"", 1, 1, keywordFile1 );
		if ( i < g_iDataCount-1 ) fwrite( ",", 1, 1, keywordFile1 );

		char *ptr = p->szName;
		char *ptr2 = strLower;
		while( *ptr )
		{
			*ptr2 = tolower( *ptr );
			ptr++;
			ptr2++;
		} 
		*ptr2 = 0;
		fwrite( "\"", 1, 1, keywordFile6 );
		fwrite( strLower, strlen(strLower), 1, keywordFile6 );
		fwrite( "\"", 1, 1, keywordFile6 );
		if ( i < g_iDataCount-1 ) 
		{
			fwrite( ",", 1, 1, keywordFile6 );
			keywordFile6Count += strlen(strLower);
			if ( keywordFile6Count > 150 ) 
			{
				fwrite( "\n", 1, 1, keywordFile6 );
				keywordFile6Count = 0;
			}
		}

		fwrite( "\"", 1, 1, keywordFile2 );
		fwrite( p->szCategory, strlen(p->szCategory), 1, keywordFile2 );
		fwrite( "\"", 1, 1, keywordFile2 );
		if ( i < g_iDataCount-1 ) fwrite( ",", 1, 1, keywordFile2 );

		fwrite( "\"", 1, 1, keywordFile3 );

		fwrite( "|", 1, 1, keywordFile5 );
		fwrite( p->szName, strlen(p->szName), 1, keywordFile5 );

		fwrite( "(\"", 2, 1, keywordFile4 );
		fwrite( p->szName, strlen(p->szName), 1, keywordFile4 );
		fwrite( "\",\"", 3, 1, keywordFile4 );
		fwrite( p->szCategory, strlen(p->szCategory), 1, keywordFile4 );
		fwrite( "\")", 2, 1, keywordFile4 );
		if ( i < g_iDataCount-1 ) fwrite( ",\n", 2, 1, keywordFile4 );

		int iBlock = 1;

		

		for ( int j = 0; j < p->iCount; j++ )
		{
			

			char szOriginal [ 256 ] = "";
			char szTemp     [ 256 ] = "";
			char szTier1    [ 256 ] = "";

			strcpy ( szOriginal, p->listing [ j ].szListing );

			char* token = strtok ( szOriginal, " " );

			while ( token )
			{
				if ( 
						strcmp ( token, "void" ) != 0 && 
						strcmp ( token, "UINT" ) != 0 && 
						strcmp ( token, "float" ) != 0 &&
						strcmp ( token, "const" ) != 0 &&
						strcmp ( token, "char" ) != 0 &&
						strcmp ( token, "char*" ) != 0 &&
						strcmp ( token, "int" ) != 0 &&
						strcmp ( token, "unsigned" ) != 0 &&
						strcmp ( token, "DWORD" ) != 0 &&
						strcmp ( token, "cImage*" ) != 0 &&
						strcmp ( token, "uint32_t" ) != 0 
				)
				{
					if ( token [ 0 ] == '*' )
						token [ 0 ] = ' ';

					strcat ( szTemp, token );
					strcat ( szTemp, " " );
				}

				token = strtok ( 0, " " );
			}

			


			memcpy ( szTier1, &szTemp [ 5 ], sizeof ( char ) * strlen ( szTemp ) - 5 );

			// need to add basic block or cpp block around each listing

			sprintf ( szFile, "<div id=\"basicCodeBlock%i\" class=\"basiccode\">", iBlock++ );
			fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

			//if ( strcmp ( p->szName, "ScreenFPS" ) == 0 )
			{
				strcpy ( szOriginal, p->listing [ j ].szListing );
				char* token = strtok ( szOriginal, " " );
				char szReturn [ 256 ] = "";

				if ( strcmp ( token, "float" ) == 0 ) strcpy ( szReturn, "float" );
				if ( strcmp ( token, "UINT" ) == 0 ) strcpy ( szReturn, "integer" );
				if ( strcmp ( token, "char" ) == 0 ) strcpy ( szReturn, "string" );
				if ( strcmp ( token, "char*" ) == 0 ) strcpy ( szReturn, "string" );
				if ( strcmp ( token, "int" ) == 0 ) strcpy ( szReturn, "integer" );
				if ( strcmp ( token, "unsigned" ) == 0 ) strcpy ( szReturn, "integer" );
				if ( strcmp ( token, "uint32_t" ) == 0 ) strcpy ( szReturn, "integer" );


				sprintf ( szFile, "<p>%s %s</p></div>", szReturn, szTier1 );

				// need to store this bit - it's the tier 1 part we require

				char szLine [ 512 ] = "";
				sprintf ( szLine, "%s|Reference/%s/%s.htm\n", p->szName, p->szCategory, p->szName );
				
				strcpy ( g_szKeywordLinks [ g_iKeywordCount ], szLine );
				g_iKeywordCount++;

				int d = 0;

				strcpy( szLine, strchr(szTier1,'(') );
				if ( j == 0 ) fwrite( szLine, strlen(szLine), 1, keywordFile3 );
			}

			//sprintf ( szFile, "<p>%s</p></div>", szTier1 );
			fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

			sprintf ( szFile, "<div id=\"cppCodeBlock%i\" style=\"display:none\" class=\"cppcode\">", iBlock++ );
			fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

			sprintf ( szFile, "<p>%s</p></div>", p->listing [ j ].szListing );
			fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
		}

		fwrite( "\"", 1, 1, keywordFile3 );
		if ( i < g_iDataCount-1 ) fwrite( ",", 1, 1, keywordFile3 );

		//sprintf ( szFile, "<p>%s</p>", p->szListing );
		//fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

		char szParams [ 50 ] [ 256 ];
		int params = 0;
		memset ( &szParams, 0, sizeof ( szParams ) );


		int paramCount = 0;

		for ( int z = 0; z < p->iCount; z++ )
			paramCount += p->listing [ z ].iCount;

		//if ( p->listing [ 0 ].iCount != 0 )
		if ( paramCount != 0 )
		{
			sprintf ( szFile, "<h2>Parameters</h2>"  );
			fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

			

			sprintf ( szFile, "<ul>" );
			fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

			// add all parameters into a list, if it's been written already ignore it

			for ( int z = 0; z < p->iCount; z++ )
			{
				for ( int j = 0; j < p->listing [ z ].iCount; j++ )
				{
					bool bAdd = true;

					// is it in the list
					for ( int d = 0; d < paramCount; d++ )
					{
						if ( strcmp ( &szParams [ d ] [ 0 ], p->listing [ z ].parameters [ j ].szName ) == 0 )
						{
							bAdd = false;
							break;
						}
					}

					if ( bAdd )
					{
						strcpy ( &szParams [ paramCount++ ] [ 0 ], p->listing [ z ].parameters [ j ].szName );

						sprintf ( szFile, "<li><i>%s</i> - %s</li>", p->listing [ z ].parameters [ j ].szName, p->listing [ z ].parameters [ j ].szDescription );
						fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
					}
				}
			}
			
			sprintf ( szFile, "</ul>" );
			fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
		}

		// footer
		fwrite ( szLine20, strlen(szLine20), 1, fp );
		fwrite ( szLine20b, strlen(szLine20b), 1, fp );
		fwrite ( p->szName, strlen(p->szName), 1, fp );

		fwrite ( szLine20c, strlen(szLine20c), 1, fp );
		fwrite ( szLine20d, strlen(szLine20d), 1, fp );
		fwrite ( "../../", strlen("../../"), 1, fp );

		fwrite ( szLine21, strlen(szLine21), 1, fp );
		fwrite ( "../../", strlen("../../"), 1, fp );

		fwrite ( szLine22, strlen(szLine22), 1, fp );

		fwrite ( "</body>", strlen("</body>"), 1, fp );
		fwrite ( "</html>", strlen("</html>"), 1, fp );
			
		fclose ( fp );

		SetCurrentDirectory ( "..\\" );
	}

	fclose(keywordFile1);
	fclose(keywordFile2);
	fclose(keywordFile3);
	fclose(keywordFile5);
	fclose(keywordFile6);
	fclose(keywordFile4);


	// now make one new file

	qsort ( g_SortedCommands, g_iDataCount, sizeof ( sCommand ), CompareNames );

	FILE* fp = fopen ( "Index.htm", "wt" );

	WriteHTMLHeader( fp, "Index", 0, "../" );

	char szFile [ 256 ] = "";

	sprintf ( szFile, "<h1>Index</h1>" );
	fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );

	for ( int i = 0; i < g_iDataCount; i++ )
	{
		sCommand* p = &g_SortedCommands [ i ];

		if ( p->bMarked )
			continue;

		if ( strlen ( p->szCategory ) == 0 )
			continue;

		sprintf ( szFile, "<li><a href=\"%s/%s.htm\">%s</a></li>", p->szCategory, p->szName, p->szName );
		fwrite ( szFile, sizeof ( char ) * strlen ( szFile ), 1, fp );
	}

	// footer
	fwrite ( szLine20, strlen(szLine20), 1, fp );
	fwrite ( szLine20d, strlen(szLine20d), 1, fp );
	fwrite ( "../", strlen("../"), 1, fp );

	fwrite ( szLine21, strlen(szLine21), 1, fp );
	fwrite ( "../", strlen("../"), 1, fp );

	fwrite ( szLine22, strlen(szLine22), 1, fp );

	fwrite ( "</body>", strlen("</body>"), 1, fp );
	fwrite ( "</html>", strlen("</html>"), 1, fp );

	fclose ( fp );

	// run through all the commands, write a big text file:

	
	//SetCurrentDirectory ( "..\\" );
	fp = fopen ( "keywordlinks.txt", "wt" );


	for ( int i = 0; i < g_iKeywordCount; i++ )
	{

		char szLine [ 256 ] = "";


		sprintf ( szLine, "%s", g_szKeywordLinks [ i ] );
		fwrite ( szLine, strlen ( szLine ), 1, fp );
	}

	fclose ( fp );
	//SetCurrentDirectory ( "Reference" );
	
	//////////////
	
	return 0;
}
