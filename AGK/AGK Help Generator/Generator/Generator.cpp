// Generator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cFile.h"
#include "cFileList.h"

// <i>Hello</i>
// <b>Bold</b>
// <big>big text</big>
// <blockquote>Hello</blockquote> 


#define MAX_SIZE	4096
#define DB_PRO		0
#define DARK_GDK	1

struct sTemplate
{
	std::vector < char* > contentList;
};

/*
struct sParameter
{
	char szDBPro       [ MAX_SIZE ];
	char szGDK         [ MAX_SIZE ];
	char szDescription [ MAX_SIZE ];
};
*/


	/*
	_header
	_sheader
	_text
	_list
	_tier1
	_tier2
	*/

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
};

struct sType
{
	eType type;
	char  szText [ MAX_SIZE ];

	//char szCodeListing [ 512 ] [ MAX_SIZE ];
	//int iCodeCount;
};

sType				contents [ 512 ];
	int			iContentCount;

struct sData
{
	//char						szHeader			[ MAX_SIZE ];

	//char		szContents [ 512 ] [ MAX_SIZE ];
	

	


	/*
	char						szNameDBPro			[ MAX_SIZE ];
	char						szNameGDK			[ MAX_SIZE ];

	char						szDescription		[ MAX_SIZE ];

	int							iSyntaxListing;

	char						szSyntaxDBPro		[ MAX_SIZE ];
	char						szSyntaxGDK			[ MAX_SIZE ];

	char						szReturnDBPro		[ MAX_SIZE ];
	char						szReturnGDK			[ MAX_SIZE ];
	char						szReturnDescription [ MAX_SIZE ];
	
	std::vector < sParameter >	parameterList;
	*/
};

void ParseDataFile    ( char* szFile, sData* pData );
void LoadTemplateFile ( char* szFile, sTemplate* pTemplate );
void OutputFile       ( char* szFile, sTemplate* pTemplate, sData* pData, int iMode );

int _tmain ( int argc, _TCHAR* argv [ ] )
{
	/*
	<h1>Getting started with multiplayer</h1>
<h2>Description</h2>
	<p>AGK provides a set of commands that can be used to quickly and easily get communications up and running for a multiplayer enabled game. The commands provided allow you to host or join a network, send messages around the network, retrieve messages and find out information such as the amount and names of players in a game.</p>
<div id="basicCodeBlock0"  class="cppcode">
<pre style=width: 500px; border: 1px;>
CreateSprite ( 1, 1 )SetSpritePosition ( 1, 10, 20 )SetSpriteSize ( 1, 10, -1 )
</div>

<div id="cppCodeBlock1" style="display:none" class="basiccode">
<pre style=width: 500px; border: 1px;>
agk::CreateSprite ( 1, 1 );agk::SetSpritePosition ( 1, 10, 20 );agk::SetSpriteSize ( 1, 10, -1 );
</div>

<h2>Overview</h2>
<p>This example will handle the following:</p>
<ul>    <li>set up a screen which has two sprites</li></ul>
*/

	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

	FILE* fp = fopen ( "E:\\Projects\\AGK Help Generator\\Generator\\base.htm", "rt" );
	int iFileSize = 0;
	char* pBuffer = 0;

	fseek ( fp, 0, SEEK_END );
	iFileSize = ftell ( fp );
	fseek ( fp, 0, SEEK_SET );

	pBuffer = new char [ iFileSize ];

	memset ( pBuffer, 0, iFileSize );

	fread ( pBuffer, 1, iFileSize, fp );
	fclose ( fp );

	fp = fopen ( "test.html", "wt" );

	fwrite ( pBuffer, iFileSize, 1, fp );

	char* pEnd = "<div></body></html>";

	fwrite ( pEnd, sizeof ( char ) * strlen ( pEnd ), 1, fp );

	fclose ( fp );

	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

	cFileList fileList;

	fileList.Search ( "*.txt" );

	//DWORD dwCount = fileList.FindMatchingFiles ( "txt" );

	for ( int i = 0; i < fileList.GetCount ( ); i++ )
	{
		sData		dataFile;
		sTemplate	templateFile;
		char		szOutput [ 256 ]	= "";
		char*		szFile				= fileList.GetName ( i );
		int			iLength				= strlen ( szFile );

		strcpy ( szOutput, szFile );

		szOutput [ iLength - 1 ] = 'm';
		szOutput [ iLength - 2 ] = 't';
		szOutput [ iLength - 3 ] = 'h';

		memset ( &dataFile,     0, sizeof ( sData ) );
		memset ( &templateFile, 0, sizeof ( sTemplate ) );

		//LoadTemplateFile ( "template.htm", &templateFile );
		ParseDataFile    ( szFile, &dataFile );
		OutputFile       ( szOutput, &templateFile, &dataFile, DARK_GDK );
	}
	return 0;
}

void ParseDataFile ( char* szFile, sData* pData )
{
	//cFile file ( szFile, "=\n" );
	cFile file ( szFile, "\n" );
	int   iToken = 0;

	//pData->iSyntaxListing = 1;
	//pData->iContentCount = 0;
	iContentCount = 0;

	/*
	enum eType
	{
		eHeader,
		eSubHeader,
		eText,
		eList,
		eTier1,
		eTier2,
	};

	struct sType
	{
		eType type;
		char szText [ MAX_SIZE ];
	};
*/
	
	while ( iToken < file.GetTokenCount ( ) )
	{
		char* szToken = file.GetToken ( iToken );

		bool bParseTier1 = false;
		bool bParseTier2 = false;
		bool bSkip       = true;

		if ( strstr ( szToken, "_header"  ) ) contents [ iContentCount ].type = eHeader;
		if ( strstr ( szToken, "_sheader" ) ) contents [ iContentCount ].type = eSubHeader;
		if ( strstr ( szToken, "_text"    ) ) contents [ iContentCount ].type = eText;

		if ( strstr ( szToken, "_image"   ) ) contents [ iContentCount ].type = eImage;

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
				strcpy ( contents [ iContentCount++ ].szText, file.GetToken ( ++iToken ) );
		}

		if ( bSkip )
			iToken++;
	}
}

void LoadTemplateFile ( char* szFile, sTemplate* pTemplate )
{
	cFile file ( szFile, "\n" );
	int   iToken = 0;
	
	while ( iToken < file.GetTokenCount ( ) )
	{
		char* szToken = file.GetToken ( iToken );

		char* szLine = new char [ 4096 ];

		strcpy ( szLine, szToken );

		pTemplate->contentList.push_back ( szLine );

		iToken++;
	}
}

/*
enum eType
{
	eHeader,
	eSubHeader,
	eText,
	eList,
	eCode,
	eBeginTier1,
	eEndTier1,
	eBeginTier2,
	eEndTier2,
};

struct sType
{
	eType type;
	char  szText [ MAX_SIZE ];
};
*/

char szScript [ MAX_SIZE ] = "<script>var maxCodeBlocks = 20;var cppVisible = 0;var basicVisible = 1;function ToggleCPP(){for ( i=0; i<maxCodeBlocks; i++ ){var name = 'cppCodeBlock' + i;var code = document.getElementById( name );if ( !code ) continue;if ( cppVisible ) code.style.display = 'none';else code.style.display = '';}cppVisible = 1 - cppVisible;for ( i=0; i<maxCodeBlocks; i++ ){var name = 'basicCodeBlock' + i;var code = document.getElementById( name );if ( !code ) continue;if ( basicVisible ) code.style.display = 'none';else code.style.display = '';}basicVisible = 1 - basicVisible;}function ToggleBASIC(){for ( i=0; i<maxCodeBlocks; i++ ){var name = 'basicCodeBlock' + i;var code = document.getElementById( name );if ( !code ) continue;if ( basicVisible ) code.style.display = 'none';else code.style.display = '';}basicVisible = 1 - basicVisible;}</script>";
char szToggle [ MAX_SIZE ] = "<p><a href=\"#\" onclick=\"ToggleCPP()\">Change Language</a> <br/></p>";

char szHead [ MAX_SIZE ] = "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><style type=\"text/css\">body {font-family: Arial, sans-serif; width: 800px; margin: 50px auto; font-size: 14px;}h1 { font-size: 16px; text-align: center;}h2 { font-size:15px;  margin-bottom: 5px;} p {margin-top: 5px;}.basiccode { width: 100%;  border: 1px solid #000000; background-color: #CCFFFF; font-family: Arial, sans-serif; line-height: 1.2; font-size: 14px; padding: 5px;margin-bottom: 0px;}.cppcode {width: 100%; border: 1px solid #000000; background-color: #FFCCCC; font-family: Arial, sans-serif; line-height: 1.2; font-size: 14px; padding: 5px;margin-top: 0px;}</style></head>";


void OutputFile ( char* szFile, sTemplate* pTemplate, sData* pData, int iMode )
{
	FILE* fp = fopen ( szFile, "wt" );

	char szText [ MAX_SIZE ] = "";

	strcpy ( szText, "<html>" );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

	fwrite ( szHead, strlen ( szHead ) * sizeof ( char ), 1, fp );

	strcpy ( szText, "<body>" );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

	fwrite ( szScript, strlen ( szScript ) * sizeof ( char ), 1, fp );
	
	fwrite ( szToggle, strlen ( szToggle ) * sizeof ( char ), 1, fp );

	

	int iCodeBlock = 0;

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

			/*
			case eList:
			{
				strcpy ( szText, "<ul>" );

				char szTemp [ MAX_SIZE ] = "";
				char szTokens [ 16 ] [ MAX_SIZE ];
				int iTokenCount = 0;

				strcpy ( szTemp, contents [ i ].szText );

				// get the token
				char* token = strtok ( szTemp, ",\n" );

				// loop round getting tokens
				while ( token )
				{
					strcpy ( szTokens [ iTokenCount++ ], token );

					// get the next token
					token = strtok ( 0, ",\n" );
				}

				for ( int i = 0; i < iTokenCount; i++ )
				{
					strcat ( szText, "<li>" );
					strcat ( szText, szTokens [ i ] );
					strcat ( szText, "</li>" );
				}

				strcat ( szText, "</ul>" );

				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;
			*/

			case eBeginTier1:
			{
				sprintf ( szText, "<div id=\"basicCodeBlock%i\" class=\"basiccode\">", iCodeBlock++ );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				//sprintf ( szText, "<p>" );
				//fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				sprintf ( szText, "<pre style=\"width: 500px; border: 1px;\">" );
				
				

				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eBeginTier2:
			{
				sprintf ( szText, "<div id=\"cppCodeBlock%i\" style=\"display:none\" class=\"cppcode\">", iCodeBlock++ );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );

				//sprintf ( szText, "<p>" );
				//fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
				
				//sprintf ( szText, "<pre style=\"width: 500px; border: 1px\">" );
				sprintf ( szText, "<pre style=\"width: 500px; border: 1px;\">" );
				
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eCode:
			{
				//sprintf ( szText, "<i>%s</i><br>", contents [ i ].szText );
				sprintf ( szText, "%s\n", contents [ i ].szText );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;

			case eEndTier1:
			case eEndTier2:
			{
				//sprintf ( szText, "</p></div>" );
				//sprintf ( szText, "</pre></div>" );
				sprintf ( szText, "</div>" );
				fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
			}
			break;
		}
	}

	strcpy ( szText, "</html></body>" );
	fwrite ( szText, strlen ( szText ) * sizeof ( char ), 1, fp );
		
	fclose ( fp );

	/*
	{
		char szNewFile [ 2048 ] = "";
		char szData    [ 2048 ] = "";
		int  iLen = strlen ( szFile );

		strcpy ( szNewFile, szFile );

		szNewFile [ iLen - 1 ] = 't';
		szNewFile [ iLen - 2 ] = 'a';
		szNewFile [ iLen - 3 ] = 'd';

		// now build a html file
		FILE* fp = fopen ( szNewFile, "wt" );

		sprintf ( szData, "[COMMAND]\n" );
		fwrite ( szData, strlen ( szData ) * sizeof ( char ), 1, fp );

		sprintf ( szData, "DESC=%s\n", pData->szDescription );
		fwrite ( szData, strlen ( szData ) * sizeof ( char ), 1, fp );

		for ( int i = 0; i < pData->parameterList.size ( ); i++ )
		{
			sprintf ( szData, "PC%i=%s\n", i + 1, pData->parameterList [ i ].szDescription );
			fwrite ( szData, strlen ( szData ) * sizeof ( char ), 1, fp );
		}
		
		sprintf ( szData, "EXAMPLE=\n" );
		fwrite ( szData, strlen ( szData ) * sizeof ( char ), 1, fp );

		if ( strlen ( pData->szReturnDBPro ) > 1 )
		{
			sprintf ( szData, "RETURN=%s\n", pData->szReturnDescription );
			fwrite ( szData, strlen ( szData ) * sizeof ( char ), 1, fp );
		}
		else
		{
			sprintf ( szData, "RETURN=\n" );
			fwrite ( szData, strlen ( szData ) * sizeof ( char ), 1, fp );
		}
		
		for ( int i = 0; i < pData->parameterList.size ( ); i++ )
		{
			sprintf ( szData, "PA%i=%s\n", i + 1, pData->parameterList [ i ].szDBPro );
			fwrite ( szData, strlen ( szData ) * sizeof ( char ), 1, fp );
		}

		fclose ( fp );

		return;
	}

	if ( strlen ( pData->szHeader ) > 1 )
	{
		// now build a html file
		FILE* fp = fopen ( szFile, "wt" );

		

		char szHeader [ MAX_SIZE ] = "";

		sprintf ( szHeader, "<P ALIGN=CENTER STYLE=\"margin-bottom: 0cm\"><FONT FACE=\"Arial, sans-serif\"><FONT SIZE=4><B>%s</B></FONT></FONT></P><BR>", pData->szHeader );

		fwrite ( szHeader, strlen ( szHeader ) * sizeof ( char ), 1, fp );

		sprintf ( szHeader, "<P ALIGN=LEFT STYLE=\"margin-bottom: 0cm\"><FONT FACE=\"Arial, sans-serif\">%s</FONT></P><BR>", pData->szDescription );

		fwrite ( szHeader, strlen ( szHeader ) * sizeof ( char ), 1, fp );
		
		fclose ( fp );

		return;
	}

	for ( int i = 0; i < pTemplate->contentList.size ( ); i++ )
	{
		char* szLine = pTemplate->contentList [ i ];

		// page title
		if ( strcmp ( szLine, "&&TAG0#" ) == 0 )
		{
			if ( iMode == 0 ) strcpy ( szLine, pData->szNameDBPro );
			if ( iMode == 1 ) strcpy ( szLine, pData->szNameGDK   );
		}

		// function name
		if ( strcmp ( szLine, "&&TAG1#" ) == 0 )
		{
			strcpy ( szLine, "<font color=\"mediumblue\">" );

			if ( iMode == 0 ) strcat ( szLine, pData->szNameDBPro );
			if ( iMode == 1 ) strcat ( szLine, pData->szNameGDK   );

			strcat ( szLine, "</font>" );
		}

		// description of function
		if ( strcmp ( szLine, "&&TAG2#" ) == 0 )
		{
			strcpy ( szLine, pData->szDescription );
		}

		// syntax
		if ( strcmp ( szLine, "&&TAG3#" ) == 0 )
		{
			if ( iMode == 0 )
			{
				bool bReturn = false;

				if ( strlen ( pData->szReturnDBPro ) > 1 )
					bReturn = true;

				strcpy ( szLine, "<font color=\"seagreen\">" );
				strcat ( szLine, pData->szReturnDBPro );
				strcat ( szLine, "</font>" );

				strcat ( szLine, "<font color=\"mediumblue\">" );
				
				//strcat ( szLine, pData->szSyntaxDBPro );
				strcat ( szLine, pData->szNameDBPro );

				strcat ( szLine, "</font>" );

				if ( bReturn )
					strcat ( szLine, " ( " );

				for ( int j = 0; j < pData->parameterList.size ( ); j++ )
				{
					strcat ( szLine, "<i>" );
					strcat ( szLine, "<font color=\"seagreen\">" );
					strcat ( szLine, pData->parameterList [ j ].szDBPro );
					strcat ( szLine, "</font>" );

					if ( j < pData->parameterList.size ( ) - 1 )
						strcat ( szLine, "," );

					strcat ( szLine, "</i>" );
				}

				if ( bReturn )
					strcat ( szLine, " ) " );
			}

			if ( iMode == 1 )
			{

				int iSyntaxListing = pData->iSyntaxListing;

				if ( iSyntaxListing > 1 )
				{


					
					{
						for ( int iSyntax = 0; iSyntax < iSyntaxListing; iSyntax++ )
						{
							if ( iSyntax == 0 )
								strcpy ( szLine, "<font color=\"blueviolet\">" );
							else
								strcat ( szLine, "<font color=\"blueviolet\">" );

							strcat ( szLine, "<i>" );
							strcat ( szLine, pData->szReturnGDK );
							strcat ( szLine, "</i>" );
							strcat ( szLine, "</font>" );
							strcat ( szLine, "<font color=\"mediumblue\">" );
							strcat ( szLine, pData->szNameGDK );

							strcat ( szLine, "</font>" );
							strcat ( szLine, " ( " );

							if ( pData->parameterList.size ( ) > 0 ) 
							{
								for ( int j = 0; j < iSyntax + 1; j++ )
								{
									strcat ( szLine, "<i>" );
									strcat ( szLine, "<font color=\"seagreen\">" );
									strcat ( szLine, pData->parameterList [ j ].szGDK );
									strcat ( szLine, "</font>" );

									if ( j < pData->parameterList.size ( ) - 1 )
										strcat ( szLine, "," );

									strcat ( szLine, "</i>" );
								}
							}
						
							strcat ( szLine, " ); " );

							if ( iSyntax < iSyntaxListing - 1 )
								strcat ( szLine, "<br>" );
						}
					}
				}
				else
				{
					strcpy ( szLine, "<font color=\"blueviolet\">" );
					strcat ( szLine, "<i>" );
					strcat ( szLine, pData->szReturnGDK );
					strcat ( szLine, "</i>" );
					strcat ( szLine, "</font>" );
					strcat ( szLine, "<font color=\"mediumblue\">" );
					//strcat ( szLine, pData->szSyntaxGDK );
					strcat ( szLine, pData->szNameGDK );

					strcat ( szLine, "</font>" );

					strcat ( szLine, " ( " );

					for ( int j = 0; j < pData->parameterList.size ( ); j++ )
					{
						strcat ( szLine, "<i>" );
						strcat ( szLine, "<font color=\"seagreen\">" );
						strcat ( szLine, pData->parameterList [ j ].szGDK );
						strcat ( szLine, "</font>" );

						if ( j < pData->parameterList.size ( ) - 1 )
							strcat ( szLine, "," );

						strcat ( szLine, "</i>" );
					}

					if ( pData->parameterList.size ( ) == 0 )
					{
						strcat ( szLine, "<i>" );
						strcat ( szLine, "<font color=\"seagreen\">" );
						strcat ( szLine, "void" );
						strcat ( szLine, "</font>" );
						strcat ( szLine, "</i>" );
					}

					strcat ( szLine, " ); " );
				}
			}
		}

		// parameter listings
		if ( strcmp ( szLine, "&&TAG4#" ) == 0 )
		{
			//if ( iMode == 1 )
			{
				for ( int j = 0; j < pData->parameterList.size ( ); j++ )
				{
					if ( j == 0 )
						strcpy ( szLine, "<i>" );
					else
						strcat ( szLine, "<i>" );

					strcat ( szLine, "<font color=\"seagreen\">" );

					if ( iMode == 0 ) strcat ( szLine, pData->parameterList [ j ].szDBPro );
					if ( iMode == 1 ) strcat ( szLine, pData->parameterList [ j ].szGDK );
					strcat ( szLine, "</font>" );
					strcat ( szLine, "</i>" );
					strcat ( szLine, "<br>" );
					strcat ( szLine, "<ul><li>" );
					strcat ( szLine, pData->parameterList [ j ].szDescription );
					strcat ( szLine, "</li></ul>" );
				}
			}

			if ( pData->parameterList.size ( ) == 0 )
			{
				strcpy ( szLine, "<i>" );
				strcat ( szLine, "n/a" );
				strcat ( szLine, "</i>" );
				strcat ( szLine, "<br><br>" );
			}
		}

		// return description
		if ( strcmp ( szLine, "&&TAG5#" ) == 0 )
		{
			strcpy ( szLine, pData->szReturnDescription  );
		}
	}

	// now build a html file
	FILE* fp = fopen ( szFile, "wt" );

	for ( int i = 0; i < pTemplate->contentList.size ( ); i++ )
	{
		fwrite ( pTemplate->contentList [ i ], strlen ( pTemplate->contentList [ i ] ) * sizeof ( char ), 1, fp );
	}

	fclose ( fp );
	*/
}

