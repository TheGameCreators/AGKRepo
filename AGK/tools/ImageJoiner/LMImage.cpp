
#include "stdafx.h"
#include "LMImage.h"
#include "LMTexNode.h"
#include "LMImage.h"
#include "LMTexture.h"
#include "ImageJoiner.h"

LMImage::LMImage( const char *szFilename, UINT padding )
{
	pNextImage = 0;
	
	iStartX = 0;
	iStartY = 0;
	pDest = 0;

	pTexture = 0;
	iWidth = 0;
	iHeight = 0;
	iPadding = 0;

	strcpy_s( pFilename, 256, szFilename );

	HRESULT hr = D3DXCreateTextureFromFileEx( theApp.pDevice, pFilename, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &pTexture );
	if ( FAILED(hr) ) 
	{
		pTexture = 0;
		return;
	}

	D3DSURFACE_DESC pDesc;
	pTexture->GetLevelDesc( 0, &pDesc );

	iWidth = pDesc.Width;
	iHeight = pDesc.Height;
	iPadding = padding;
}

LMImage::~LMImage( )
{
	if ( pTexture ) pTexture->Release();
	pTexture = 0;
}

int LMImage::GetWidth( ) { return iWidth; }
int LMImage::GetHeight( ) { return iHeight; }
int LMImage::GetPaddedWidth( ) { return iWidth + iPadding*2; }
int LMImage::GetPaddedHeight( ) { return iHeight + iPadding*2; }
int LMImage::GetArea( ) { return iHeight*iWidth; }

int LMImage::GetStartX( ) { return iStartX + iPadding; }
int LMImage::GetStartY( ) { return iStartY + iPadding; }
int LMImage::GetPaddedStartX( ) { return iStartX; }
int LMImage::GetPaddedStartY( ) { return iStartY; }

void LMImage::CopyToTexture( )
{
	if ( !pTexture ) return;
	if ( !pDest ) return;

	D3DLOCKED_RECT	d3dlr;
	D3DSURFACE_DESC desc;

	if ( FAILED ( pTexture->GetLevelDesc ( 0, &desc ) ) )
	{
		char str[256];
		sprintf_s( str, 256, "Error Getting Texture Description: %s", pFilename );
		MessageBox( NULL, str, "Error", 0 ); 
		return;
	}

	if ( FAILED ( pTexture->LockRect ( 0, &d3dlr, 0, 0 ) ) )
	{
		char str[256];
		sprintf_s( str, 256, "Error Locking Texture: %s", pFilename );
		MessageBox( NULL, str, "Error", 0 ); 
		return;
	}

	pPixels = ( DWORD* ) d3dlr.pBits;
	iPitch  = d3dlr.Pitch / 4;
	iWidth  = desc.Width;
	iHeight = desc.Height;

	DWORD dwIndex;

	int iRealStartX = iStartX + iPadding;
	int iRealStartY = iStartY + iPadding;

	// padding the left side
	for ( int x = iStartX; x < (int) iStartX+iPadding; x++ )
	{
		for ( int y = 0; y < (int) iHeight; y++ )
		{
			dwIndex = 0 + ( y * iPitch );
			pDest->SetLumel( x, y+iRealStartY, *( pPixels + dwIndex ) );
		}
	}

	// padding the right side
	for ( int x = iStartX+iWidth+iPadding; x < (int) iStartX+iWidth+iPadding*2; x++ )
	{
		for ( int y = 0; y < (int) iHeight; y++ )
		{
			dwIndex = iWidth-1 + ( y * iPitch );
			pDest->SetLumel( x, y+iRealStartY, *( pPixels + dwIndex ) );
		}
	}

	int iPaddingInt = iPadding;

	// padding the top
	for ( int x = -iPaddingInt; x < (int) iWidth+iPaddingInt; x++ )
	{
		for ( int y = iStartY; y < (int) iStartY+iPadding; y++ )
		{
			int indexX = x;
			if ( indexX < 0 ) indexX = 0;
			if ( indexX > iWidth-1 ) indexX = iWidth-1;
			dwIndex = indexX;

			pDest->SetLumel( x+iRealStartX, y, *( pPixels + dwIndex ) );
		}
	}

	// padding the bottom
	for ( int x = -iPaddingInt; x < (int) iWidth+iPaddingInt; x++ )
	{
		for ( int y = iStartY+iHeight+iPadding; y < (int) iStartY+iHeight+iPadding*2; y++ )
		{
			int indexX = x;
			if ( indexX < 0 ) indexX = 0;
			if ( indexX > iWidth-1 ) indexX = iWidth-1;
			dwIndex = indexX + ( (iHeight-1) * iPitch );

			pDest->SetLumel( x+iRealStartX, y, *( pPixels + dwIndex ) );
		}
	}

	// fill in the center
	for ( int v = 0; v < (int) iHeight; v++ )
	{
		for ( int u = 0; u < (int) iWidth; u++ )
		{
			dwIndex = u + ( v * iPitch );

			pDest->SetLumel( u+iRealStartX, v+iRealStartY, *( pPixels + dwIndex ) );
		}
	}

	pTexture->UnlockRect ( NULL );
}

void LMImage::SetTexture( int startX, int startY, LMTexture *pTex )
{
	iStartX = startX;
	iStartY = startY;
	pDest = pTex;
}

const char* LMImage::GetFilename( )
{
	return pFilename;
}
