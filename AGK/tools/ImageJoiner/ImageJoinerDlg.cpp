// ImageJoinerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageJoiner.h"
#include "ImageJoinerDlg.h"
#include "LMImage.h"
#include "LMTexture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void strtolower( char* str1 )
{
	if ( !str1 ) return;
	
	while ( *str1 )
	{
		int ch = *str1;
		ch = tolower( ch );
		*str1 = ch;

		str1++;
	}
}


// CImageJoinerDlg dialog


CImageJoinerDlg::CImageJoinerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageJoinerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageJoinerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editImageFolder);
	DDX_Control(pDX, IDC_EDIT2, m_editWidth);
	DDX_Control(pDX, IDC_EDIT3, m_editHeight);
	DDX_Control(pDX, IDC_EDIT4, m_editPadding);
}

BEGIN_MESSAGE_MAP(CImageJoinerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CImageJoinerDlg::OnBrowse)
	ON_BN_CLICKED(IDOK, &CImageJoinerDlg::OnPack)
END_MESSAGE_MAP()


// CImageJoinerDlg message handlers

BOOL CImageJoinerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetupD3D();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImageJoinerDlg::SetupD3D( )
{
	theApp.pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if ( !theApp.pD3D ) ASSERT(FALSE);

	CRect rect;
	GetClientRect( &rect );

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = rect.Width();
	d3dpp.BackBufferHeight           = rect.Height();
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = this->GetSafeHwnd();
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	HRESULT hr = theApp.pD3D->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		D3DDEVTYPE_HAL,     // device type
		this->GetSafeHwnd(),// window associated with device
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
	    &d3dpp,             // present parameters
	    &theApp.pDevice);            // return created device

	if ( FAILED( hr ) ) ASSERT(FALSE);
}

void CImageJoinerDlg::CleanD3D( )
{
	if ( theApp.pDevice ) theApp.pDevice->Release();
	if ( theApp.pD3D ) theApp.pD3D->Release( );

	theApp.pDevice = 0;
	theApp.pD3D = 0;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImageJoinerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CImageJoinerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CImageJoinerDlg::OnBrowse()
{
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.hwndOwner = this->m_hWnd;
	bi.lpszTitle = "Choose an input folder and press 'OK'.";
	bi.ulFlags = BIF_VALIDATE;

	LPITEMIDLIST pIDL;
	char buffer[_MAX_PATH];
	bool bValid = true;

	do
	{
		bValid = true;

		pIDL = ::SHBrowseForFolder(&bi);
		if ( pIDL == NULL ) return;    //cancel

		if(::SHGetPathFromIDList(pIDL, buffer) == 0) { AfxMessageBox( "Invalid item selected" ); bValid = false; }
	}
	while ( !bValid );

	CString sFolderPath = buffer;
	
	m_editImageFolder.SetWindowText( sFolderPath );
}

void CImageJoinerDlg::OnPack()
{
	CString sWidth;
	m_editWidth.GetWindowText( sWidth );
	int iFinalWidth = atoi(sWidth);

	CString sHeight;
	m_editHeight.GetWindowText( sHeight );
	int iFinalHeight = atoi(sHeight);

	CString sPadding;
	m_editPadding.GetWindowText( sPadding );
	int iFinalPadding = atoi(sPadding);

	CString sPath;
	m_editImageFolder.GetWindowText( sPath );

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	if ( !SetCurrentDirectory( sPath ) ) 
	{
		MessageBox( "Invalid Folder" );
		return;
	}

	if ( !SetCurrentDirectory( "result" ) ) CreateDirectory( "result", NULL );
	SetCurrentDirectory( sPath );

	CString sNewPath = sPath + "\\result\\";
		
	hFind = FindFirstFile("*.*", &FindFileData);

	LMImage *pImages = NULL;
	int iImageCount = 0;

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if ( _tcscmp( FindFileData.cFileName, _T(".") ) && _tcscmp( FindFileData.cFileName, _T("..") ) )
			{
				if ( !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
					const char *szExt = strrchr( FindFileData.cFileName, '.' );
					
					char szExt2[256];
					strcpy_s( szExt2, 256, szExt );
					strtolower( szExt2 );

					if ( strcmp( szExt2, ".jpg" ) != 0 
					  && strcmp( szExt2, ".jpeg" ) != 0 
					  && strcmp( szExt2, ".bmp" ) != 0 
					  && strcmp( szExt2, ".png" ) != 0 ) continue;

					//load images
					LMImage *pImage = new LMImage( FindFileData.cFileName, iFinalPadding );
					if ( !pImage->IsLoaded() ) 
					{
						char str[256];
						sprintf_s( str, 256, "Failed to load image: %s", FindFileData.cFileName );
						MessageBox( str );
						delete pImage;
					}
					else
					{
						pImage->pNextImage = pImages;
						pImages = pImage;
						iImageCount++;
					}
				}
			}
		} while (FindNextFile(hFind, &FindFileData) != 0);

		FindClose(hFind);
	}

	if ( !pImages )
	{
		MessageBox( "No images to pack" );
		return;
	}

	//order images by size
	LMImage **ppImageArray = new LMImage* [iImageCount];
	int count = 0;
	LMImage *pImage = pImages;
	while ( pImage )
	{
		ppImageArray[count] = pImage;
		count++;
		pImage = pImage->pNextImage;
	}

	//bubble sort
	for ( int i = 0; i < iImageCount-1; i++ )
	{
		for ( int j = i+1; j < iImageCount; j++ )
		{
			if ( ppImageArray[i]->GetArea() < ppImageArray[j]->GetArea() )
			{
				//swap
				LMImage *temp = ppImageArray[j];
				ppImageArray[j] = ppImageArray[i];
				ppImageArray[i] = temp;
			}
		}
	}

	LMTexture *pTextures = new LMTexture( iFinalWidth, iFinalHeight, 0 );

	//map images to texture space
	for ( int i = 0; i < iImageCount; i++ )
	{
		if ( ppImageArray[i]->IsLoaded() )
		{
			int startx, starty;
			LMTexture *pTex = pTextures->AddImage( ppImageArray[i]->GetPaddedWidth(), ppImageArray[i]->GetPaddedHeight(), startx, starty );
			if ( !pTex )
			{
				char str[256];
				sprintf_s( str, 256, "image too large for final texture: %s", ppImageArray[i]->GetFilename() );
				MessageBox( str );
			}
			else ppImageArray[i]->SetTexture( startx, starty, pTex );
		}
	}

	//create position file
	/*
	char str[256];
	sprintf_s( str, 256, "%simage_positions.txt", sNewPath );
	FILE *pFile = fopen( str, "w" );
	if ( !pFile ) 
	{
		MessageBox( "Failed to open image position file for writing" );
	}
	*/

	//copy images onto textures
	for ( int i = 0; i < iImageCount; i++ )
	{
		if ( ppImageArray[i]->IsLoaded() && ppImageArray[i]->GetDest() ) 
		{
			ppImageArray[i]->CopyToTexture();
/*	
			if ( pFile ) 
			{
				char str[256];
				sprintf_s( str, 256, "%s,%d,%d,%d,%d,Texture %d.png\n", ppImageArray[i]->GetFilename(), ppImageArray[i]->GetStartX(), ppImageArray[i]->GetStartY(), ppImageArray[i]->GetWidth(), ppImageArray[i]->GetHeight(), ppImageArray[i]->GetDest()->GetIndex() );
				fputs( str, pFile );
			}
*/
		}
	}

//	if ( pFile ) fclose( pFile );

	char linksfilename[256];
	sprintf_s( linksfilename, 256, "%slinks.txt", sNewPath );
	FILE *pLinksFile = fopen( linksfilename, "wb" );

	//save textures
	count = 0;
	LMTexture *pTex = pTextures;
	while ( pTex )
	{
		char filename[256];
		sprintf_s( filename, 256, "Texture %d.png", count );
		char fullpath[256];
		sprintf_s( fullpath, 256, "%s%s", sNewPath, filename );
		pTex->CopyToTexture();
		pTex->SaveTexture( fullpath, 0 );
		pTex->Reset();

		char str[256];
		sprintf_s( str, 256, "%sTexture %d subimages.txt", sNewPath, count );
		FILE *pFile = fopen( str, "w" );
		if ( !pFile ) 
		{
			MessageBox( "Failed to open image position file for writing" );
		}

		//write position details for this texture
		for ( int i = 0; i < iImageCount; i++ )
		{
			if ( ppImageArray[i]->IsLoaded() && ppImageArray[i]->GetDest() == pTex )
			{
				if ( pFile ) 
				{
					sprintf_s( str, 256, "%s:%d:%d:%d:%d\n", ppImageArray[i]->GetFilename(), ppImageArray[i]->GetStartX(), ppImageArray[i]->GetStartY(), ppImageArray[i]->GetWidth(), ppImageArray[i]->GetHeight() );
					fputs( str, pFile );
				}

				if ( pLinksFile )
				{
					sprintf_s( str, 256, "%s\n%s\n", ppImageArray[i]->GetFilename(), filename );
					fputs( str, pLinksFile );
				}
			}
		}

		if ( pFile ) fclose( pFile );

		count++;
		pTex = pTex->pNextLMTex;
	}

	if ( pLinksFile ) fclose( pLinksFile );
	pLinksFile = 0;

	MessageBox( "Done" );
}
