#ifndef LMIMAGE_H
#define LMIMAGE_H

#include "d3dx9.h"

class LMTexNode;
class LMTexture;

class LMImage //: public Thread
{

private:
	
	LMTexNode* pNodeSpace;

	char pFilename[256];
	IDirect3DTexture9 *pTexture;
	
	DWORD *pPixels;
	int iPitch;
	UINT iWidth;
	UINT iHeight;
	UINT iPadding;

	int iStartX;
	int iStartY;
	LMTexture *pDest;

public:

	LMImage *pNextImage;
	
	LMImage( const char *szFilename, UINT padding );
	~LMImage( );

	bool IsLoaded() { return pTexture != NULL; }

	int GetWidth( );
	int GetHeight( );
	int GetPaddedWidth( );
	int GetPaddedHeight( );
	int GetArea();

	int GetStartX( );
	int GetStartY( );
	int GetPaddedStartX( );
	int GetPaddedStartY( );

	void SetTexture( int startX, int startY, LMTexture *pTex );

	void CopyToTexture( );
	const char* GetFilename( );
	LMTexture* GetDest( ) { return pDest; }
};

#endif