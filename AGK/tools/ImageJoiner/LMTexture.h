#ifndef LMTEXTURE_H
#define LMTEXTURE_H

#include "d3dx9.h"

class LMTexNode;
class Lumel;

//#include "Thread.h"

class LMTexture //: public Thread
{

private:
	
	int iSizeU, iSizeV;
	//Lumel **ppTexLumel;			//the pixels of this texture
	DWORD *ppTexPixel;
	bool bEmpty;

	LMTexNode* pNodeSpace;

	char pFilename[256];
	IDirect3DTexture9 *pTexture;

	DWORD *pPixels;
	int iPitch;
	UINT iWidth;
	UINT iHeight;

	int iPos;

public:

	LMTexture *pNextLMTex;		//the next light map in the list, should get progressively less filled.
	
	LMTexture( int sizeU, int sizeV, int count );
	~LMTexture( );

	bool IsEmpty( );

	int GetSizeU( );
	int GetSizeV( );
	int GetIndex( );

	LMTexture* AddImage( int width, int height, int &startX, int &startY );
	
	//void SetLumel( int u, int v, float red, float green, float blue );
	void SetLumel( int u, int v, DWORD color );

	void CopyToTexture( );
	void SaveTexture( char* pFilename, int format=0 );
	IDirect3DTexture9* GetDXTextureRef( );
	char* GetFilename( );

	void Reset();
};

#endif