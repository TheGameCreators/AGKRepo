
#include "stdafx.h"
#include "LMTexture.h"
#include "LMTexNode.h"
#include "ImageJoiner.h"
#include "jpeglib.h"

void WriteJPEG(const char * filename, int quality, unsigned char *bits, int width, int height)
{
	unsigned char *bits2 = new unsigned char[ width*height*3 ];
	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++ )
		{
			int index = y*width + x;
			bits2[ index*3 + 0 ] = bits[ index*4 + 0 ];
			bits2[ index*3 + 1 ] = bits[ index*4 + 1 ];
			bits2[ index*3 + 2 ] = bits[ index*4 + 2 ];
		}
	}

	/* This struct contains the JPEG compression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	* It is possible to have several such structures, representing multiple
	* compression/decompression processes, in existence at once.  We refer
	* to any one struct (and its associated working data) as a "JPEG object".
	*/
	struct jpeg_compress_struct cinfo;
	/* This struct represents a JPEG error handler.  It is declared separately
	* because applications often want to supply a specialized error handler
	* (see the second half of this file for an example).  But here we just
	* take the easy way out and use the standard error handler, which will
	* print a message on stderr and call exit() if compression fails.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct jpeg_error_mgr jerr;
	/* More stuff */
	FILE * outfile;		/* target file */
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */

	/* Step 1: allocate and initialize JPEG compression object */

	/* We have to set up the error handler first, in case the initialization
	* step fails.  (Unlikely, but it could happen if you are out of memory.)
	* This routine fills in the contents of struct jerr, and returns jerr's
	* address which we place into the link field in cinfo.
	*/
	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */

	/* Here we use the library-supplied code to send compressed data to a
	* stdio stream.  You can also write your own code to do something else.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to write binary files.
	*/
	if ((outfile = fopen(filename, "wb")) == NULL)
	{
		delete [] bits2;
		return;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	/* Step 3: set parameters for compression */

	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = width; 	/* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
	/* Now use the library's routine to set default compression parameters.
	* (You must set at least cinfo.in_color_space before calling this,
	* since the defaults depend on the source color space.)
	*/
	jpeg_set_defaults(&cinfo);
	/* Now you can set any non-default parameters you wish to.
	* Here we just illustrate the use of quality (quantization table) scaling:
	*/
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	/*
	cinfo.comp_info[0].h_samp_factor = 4;
	cinfo.comp_info[0].v_samp_factor = 2;
	cinfo.comp_info[1].h_samp_factor = 1;
	cinfo.comp_info[1].v_samp_factor = 1;
	cinfo.comp_info[2].h_samp_factor = 1;
	cinfo.comp_info[2].v_samp_factor = 1;
	*/

	/* Step 4: Start compressor */

	/* TRUE ensures that we will write a complete interchange-JPEG file.
	* Pass TRUE unless you are very sure of what you're doing.
	*/
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */

	/* Here we use the library's state variable cinfo.next_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	* To keep things simple, we pass one scanline per call; you can pass
	* more if you wish, though.
	*/
	row_stride = width * 3;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height)
	{
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could pass
		 * more than one scanline at a time if that's more convenient.
		 */
		row_pointer[0] = & bits2[ cinfo.next_scanline * row_stride ];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	/* Step 6: Finish compression */

	jpeg_finish_compress(&cinfo);
	/* After finish_compress, we can close the output file. */
	fclose(outfile);

	/* Step 7: release JPEG compression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_compress(&cinfo);

	delete [] bits2;

	/* And we're done! */
}


LMTexture::LMTexture( int sizeU, int sizeV, int count )
{
	iPos = count;
	iSizeU = sizeU;
	iSizeV = sizeV;
	bEmpty = true;

	ppTexPixel = new DWORD [ iSizeU*iSizeV ];

	pNextLMTex = 0;
	pNodeSpace = new LMTexNode( 0, 0, sizeU, sizeV );
	pNodeSpace->Finalise( );
	
	pPixels = 0;
	iPitch = 0;
	iWidth = 0;
	iHeight = 0;

	pTexture = 0;

	sprintf_s( pFilename, 255, "0" );

	//pLocalLightList = 0;
	//pLocalColTree = 0;
}

LMTexture::~LMTexture( )
{
	Reset();

	if ( ppTexPixel ) delete [] ppTexPixel;

	if ( pNodeSpace ) delete pNodeSpace;
	if ( pNextLMTex ) delete pNextLMTex;
}

bool LMTexture::IsEmpty( ) { return bEmpty; }

int LMTexture::GetSizeU( ) { return iSizeU; }
int LMTexture::GetSizeV( ) { return iSizeV; }
int LMTexture::GetIndex( ) { return iPos; }

LMTexture* LMTexture::AddImage( int width, int height, int &startX, int &startY )
{
	LMTexNode* pNode = pNodeSpace->AddImage( width, height );

	if ( !pNode )
	{
		//object did't fit in texture
		//if this texture was empty it'll never fit
		if ( bEmpty ) return NULL;
		
		if ( pNextLMTex )
		{
			return pNextLMTex->AddImage( width, height, startX, startY );
		}
		else
		{
			pNextLMTex = new LMTexture( iSizeU, iSizeV, iPos+1 );
			return pNextLMTex->AddImage( width, height, startX, startY );
		}
	}
	else
	{
		startX = pNode->GetStartU( );
		startY = pNode->GetStartV( );

		pNodeSpace->Finalise( );
		bEmpty = false;

		return this;
	}
}

/*
void LMTexture::SetLumel( int u, int v, float red, float green, float blue )
{
	if ( u < 0 || v < 0 || u >= iSizeU || v >= iSizeV ) return;

	ppTexPixel[u][v].SetCol( red, green ,blue );
}
*/

void LMTexture::SetLumel( int u, int v, DWORD color )
{
	if ( u < 0 || v < 0 || u >= iSizeU || v >= iSizeV ) return;

	int alpha = color >> 24;
	int red = (color >> 16) & 0xff;
	int green = (color >> 8) & 0xff;
	int blue = color & 0xff;

	ppTexPixel[v*iSizeU + u] = (alpha << 24) | (blue << 16) | (green <<8) | red;
}

void LMTexture::CopyToTexture( )
{
	/*
	if ( FAILED ( theApp.pDevice->CreateTexture ( iSizeU, iSizeV, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL ) ) )
	{
		MessageBox( NULL, "Error Creating Light Map Texture", "Error", 0 ); exit(1);
	}

	D3DLOCKED_RECT	d3dlr;
	D3DSURFACE_DESC desc;

	if ( FAILED ( pTexture->GetLevelDesc ( 0, &desc ) ) )
	{
		MessageBox( NULL, "Error Getting Texture Description", "Error", 0 ); exit(1);
	}

	if ( FAILED ( pTexture->LockRect ( 0, &d3dlr, 0, 0 ) ) )
	{
		MessageBox( NULL, "Error Locking Texture", "Error", 0 ); exit(1);
	}

	pPixels = ( DWORD* ) d3dlr.pBits;
	iPitch  = d3dlr.Pitch / 4;
	iWidth  = desc.Width;
	iHeight = desc.Height;

	DWORD dwIndex;
	DWORD index2;

	for ( int v = 0; v < iSizeV; v++ )
	{
		for ( int u = 0; u < iSizeU; u++ )
		{
			dwIndex = u + ( v * iPitch );
			index2 = v*iSizeU + u;
			*( pPixels + dwIndex ) = ppTexPixel[index2];
		}
	}

	pTexture->UnlockRect ( NULL );
	
	delete [] ppTexPixel;
	ppTexPixel = 0;

	if ( pNodeSpace ) delete pNodeSpace;
	pNodeSpace = 0;
	*/
}

void LMTexture::SaveTexture( char* pNewFilename, int format )
{
	strcpy_s( pFilename, 255, pNewFilename );

	//WriteJPEG( pFilename, 85, (unsigned char*)ppTexPixel, iSizeU, iSizeV );
	
	switch( format )
	{
		case 0: D3DXSaveTextureToFile( pFilename, D3DXIFF_PNG, pTexture, NULL ); break;
		case 1: D3DXSaveTextureToFile( pFilename, D3DXIFF_DDS, pTexture, NULL ); break;
		case 2: D3DXSaveTextureToFile( pFilename, D3DXIFF_BMP, pTexture, NULL ); break;
		case 3: D3DXSaveTextureToFile( pFilename, D3DXIFF_JPG, pTexture, NULL ); break;	
		default: D3DXSaveTextureToFile( pFilename, D3DXIFF_PNG, pTexture, NULL ); break;
	}
}

IDirect3DTexture9* LMTexture::GetDXTextureRef( )
{
	return pTexture;
}

char* LMTexture::GetFilename( )
{
	return pFilename;
}

void LMTexture::Reset()
{
	if ( pTexture ) pTexture->Release();
	pTexture = 0;

	if ( ppTexPixel ) delete [] ppTexPixel;
	ppTexPixel = 0;
}