// Common Include
//#include "cImage.h"
//#include "Wrapper.h"
#include "agk.h"
#include "zlib.h"
#include "gif_lib.h"
#include "png.h"
#include "jpeglib.h"

#if !defined(AGK_BLACKBERRY) && !defined(AGK_HTML5)
	#ifdef AGK_IOS
		#include "QRCodeReader.h"
		#include "GreyscaleLuminanceSource.h"
		#include "GlobalHistogramBinarizer.h"
	#else
		#include "qrcode/QRCodeReader.h"
		#include "common/GreyscaleLuminanceSource.h"
		#include "common/GlobalHistogramBinarizer.h"
	#endif
	#include "BinaryBitmap.h"
	#include "Binarizer.h"
	#include "ReaderException.h"
	#include "QREncode.h"
#endif

extern unsigned int libArialSubImages[];
#ifndef AGK_HTML5
extern unsigned int libAvenirSubImages[];
#endif

#include "libImages.h"

namespace AGK
{
	struct my_error_mgr {
	  jpeg_error_mgr pub;	/* "public" fields */
	  jmp_buf setjmp_buffer;	/* for return to caller */
	};

	typedef struct my_error_mgr * my_error_ptr;

	static void my_error_exit (j_common_ptr cinfo)
	{
	  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	  my_error_ptr myerr = (my_error_ptr) cinfo->err;

	  /* Always display the message. */
	  /* We could postpone this until after returning, if we chose. */
	  //(*cinfo->err->output_message) (cinfo);

	  uString errstr;
	  errstr.Format( "JPEG Error: %d", cinfo->err->msg_code );
	  agk::Warning( errstr );

	  /* Return control to the setjmp point */
	  longjmp(myerr->setjmp_buffer, 1);
	}

	bool getJpegImageSize(const char *name, int &outWidth, int &outHeight)
	{
		jpeg_decompress_struct cinfo;
		my_error_mgr jerr;

		FILE * infile;
		//int row_stride;

		if ((infile = AGKfopen(name, "rb")) == NULL)
		{
			return false;
		}

		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;

		if (setjmp(jerr.setjmp_buffer))
		{
			// If we get here, the JPEG code has signaled an error.
			// We need to clean up the JPEG object, close the input file, and return.
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
			return false;
		}

		jpeg_create_decompress(&cinfo);

		jpeg_stdio_src(&cinfo, infile);

		jpeg_read_header(&cinfo, TRUE);
		outWidth = cinfo.output_width;
		outHeight = cinfo.output_height;
		
		jpeg_destroy_decompress(&cinfo);

		fclose(infile);
		return true;
	}

	bool loadJpegImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData)
	{
		jpeg_decompress_struct cinfo;
		my_error_mgr jerr;

		FILE * infile;
		JSAMPARRAY buffer;
		//int row_stride;

		/* In this example we want to open the input file before doing anything else,
		* so that the setjmp() error recovery below can assume the file is open.
		* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
		* requires it in order to read binary files.
		*/

		if ((infile = AGKfopen(name, "rb")) == NULL)
		{
			return false;
		}

		/* Step 1: allocate and initialize JPEG decompression object */

		/* We set up the normal JPEG error routines, then override error_exit. */
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;

		/* Establish the setjmp return context for my_error_exit to use. */
		if (setjmp(jerr.setjmp_buffer))
		{
			/* If we get here, the JPEG code has signaled an error.
			 * We need to clean up the JPEG object, close the input file, and return.
			 */
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
			return false;
		}

		/* Now we can initialize the JPEG decompression object. */
		jpeg_create_decompress(&cinfo);

		/* Step 2: specify data source (eg, a file) */

		jpeg_stdio_src(&cinfo, infile);

		/* Step 3: read file parameters with jpeg_read_header() */

		(void) jpeg_read_header(&cinfo, TRUE);
		/* We can ignore the return value from jpeg_read_header since
		*   (a) suspension is not possible with the stdio data source, and
		*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
		* See libjpeg.txt for more info.
		*/

		/* Step 4: set parameters for decompression */

		/* In this example, we don't need to change any of the defaults set by
		* jpeg_read_header(), so we do nothing here.
		*/

		/* Step 5: Start decompressor */

		(void) jpeg_start_decompress(&cinfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		/* We may need to do some setup of our own at this point before reading
		* the data.  After jpeg_start_decompress() we have the correct scaled
		* output image dimensions available, as well as the output colormap
		* if we asked for color quantization.
		* In this example, we need to make an output work buffer of the right size.
		*/
		/* JSAMPLEs per row in output buffer */

		outWidth = cinfo.output_width;
		outHeight = cinfo.output_height;
		outHasAlpha = false;
		*outData = new unsigned char[ outWidth * outHeight * cinfo.output_components ];
		buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, cinfo.output_width*cinfo.output_components, 1);

		/* Step 6: while (scan lines remain to be read) */
		/*           jpeg_read_scanlines(...); */

		/* Here we use the library's state variable cinfo.output_scanline as the
		* loop counter, so that we don't have to keep track ourselves.
		*/

		int line = 0;
		while (cinfo.output_scanline < cinfo.output_height)
		{
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);

			for ( int i = 0; i < outWidth*cinfo.output_components; i++ )
			{
				int index = line*outWidth*cinfo.output_components + i;
				(*outData)[ index ] = buffer[0][i];
			}

			line++;
		}

		// convert greyscale to RGB
		if ( cinfo.output_components == 1 )
		{
			unsigned char *newData = new unsigned char[ outWidth * outHeight * 3 ];
			for ( int i = 0; i < outWidth; i++ )
			{
				for ( int j = 0; j < outHeight; j++ )
				{
					int index = j*outWidth + i;
					newData[ index*3 ] = (*outData)[ index ];
					newData[ index*3 + 1 ] = (*outData)[ index ];
					newData[ index*3 + 2 ] = (*outData)[ index ];
				}
			}

			delete [] (*outData);
			*outData = newData;
		}

		/* Step 7: Finish decompression */

		(void) jpeg_finish_decompress(&cinfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		/* Step 8: Release JPEG decompression object */

		/* This is an important step since it will release a good deal of memory. */
		jpeg_destroy_decompress(&cinfo);

		/* After finish_decompress, we can close the input file.
		* Here we postpone it until after no more JPEG errors are possible,
		* so as to simplify the setjmp error logic above.  (Actually, I don't
		* think that jpeg_destroy can do an error exit, but why assume anything...)
		*/
		fclose(infile);

		/* That's it */
		return true;
	}

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
		if ((outfile = AGKfopen(filename, "wb")) == NULL)
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

	void no_warn(png_structp png, const char *msg) 
	{
		(void)png;
		(void)msg;
	}

	void no_errmsg(png_structp png, const char *msg) 
	{
		(void)msg;
		longjmp(png_jmpbuf(png), 1);        // 1 = libpng error
	}

	bool processPngImage( png_structp png_ptr, png_infop info_ptr, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData) 
	{
		outWidth = png_get_image_width( png_ptr, info_ptr );
		outHeight = png_get_image_height( png_ptr, info_ptr );
		switch( png_get_color_type( png_ptr, info_ptr) )
		{
			case PNG_COLOR_TYPE_GRAY:
			{
				outHasAlpha = false;
				unsigned int row_bytes = (unsigned int) png_get_rowbytes(png_ptr, info_ptr);
				*outData = (unsigned char*) malloc(row_bytes * 3 * outHeight);
				png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

				unsigned int out_row_bytes = row_bytes * 3;
				for (int i = 0; i < outHeight; i++) 
				{
					for ( int j = 0; j < outWidth; j++ )
					{
						(*outData)[ out_row_bytes*i + j*3 + 0 ] = row_pointers[i][j];
						(*outData)[ out_row_bytes*i + j*3 + 1 ] = row_pointers[i][j];
						(*outData)[ out_row_bytes*i + j*3 + 2 ] = row_pointers[i][j];
					}
				}
				break;
			}

			case PNG_COLOR_TYPE_GRAY_ALPHA:
			{
				outHasAlpha = true;
				unsigned int row_bytes = (unsigned int) png_get_rowbytes(png_ptr, info_ptr);
				*outData = (unsigned char*) malloc(row_bytes * 2 * outHeight);
				png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

				unsigned int out_row_bytes = row_bytes * 2;
				for (int i = 0; i < outHeight; i++) 
				{
					for ( int j = 0; j < outWidth; j++ )
					{
						(*outData)[ out_row_bytes*i + j*4 + 0 ] = row_pointers[i][j*2 + 0];
						(*outData)[ out_row_bytes*i + j*4 + 1 ] = row_pointers[i][j*2 + 0];
						(*outData)[ out_row_bytes*i + j*4 + 2 ] = row_pointers[i][j*2 + 0];
						(*outData)[ out_row_bytes*i + j*4 + 3 ] = row_pointers[i][j*2 + 1];
					}
				}
				break;
			}

			case PNG_COLOR_TYPE_RGB:
			{
				outHasAlpha = false;
				unsigned int row_bytes = (unsigned int) png_get_rowbytes(png_ptr, info_ptr);
				*outData = (unsigned char*) malloc(row_bytes * outHeight);
				png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

				for (int i = 0; i < outHeight; i++) {
					memcpy(*outData+(row_bytes * i), row_pointers[i], row_bytes);
				}
				break;
			}
		
			case PNG_COLOR_TYPE_RGBA:
			{
				outHasAlpha = true;
				unsigned int row_bytes = (unsigned int) png_get_rowbytes(png_ptr, info_ptr);
				*outData = (unsigned char*) malloc(row_bytes * outHeight);
				png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

				for (int i = 0; i < outHeight; i++) {
					memcpy(*outData+(row_bytes * i), row_pointers[i], row_bytes);
				}
				break;
			}

			default:
			{
        		uString err;
        		err.SetStr( "Color type " );
        		err.AppendUInt( png_get_color_type( png_ptr, info_ptr) );
        		err.Append( " not supported" );
        		agk::Warning( err );
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				return false;
			}
		}

		return true;
	}

	void libPNGReadData(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
	{
		png_voidp io_ptr = png_get_io_ptr(png_ptr);
		if(io_ptr == NULL) return;

		memcpy( outBytes, io_ptr, byteCountToRead );

		png_init_io( png_ptr, (png_FILE_p)((char*)io_ptr + byteCountToRead) );
	}

	bool loadPngImageFromMemory(unsigned char *inData, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData) 
	{
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned int sig_read = 0;
		
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, no_errmsg, no_warn);

		if (png_ptr == NULL) return false;

		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			png_destroy_read_struct(&png_ptr, 0, 0);
			return false;
		}

		png_set_read_fn( png_ptr, inData, libPNGReadData );

		if (setjmp(png_jmpbuf(png_ptr))) {
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			return false;
		}

		png_set_sig_bytes(png_ptr, sig_read);

		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, 0);
		if ( !processPngImage( png_ptr, info_ptr, outWidth, outHeight, outHasAlpha, outData ) ) return false;

		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return true;
	}

	bool getPngImageSize(const char *name, int &outWidth, int &outHeight) 
	{
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned int sig_read = 0;
		FILE *fp;

		if ((fp = AGKfopen(name, "rb")) == NULL)
		{
    		return false;
		}

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, no_errmsg, no_warn);

		if (png_ptr == NULL) {
			fclose(fp);
			return false;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			fclose(fp);
			png_destroy_read_struct(&png_ptr, 0, 0);
			return false;
		}

		png_set_read_fn( png_ptr, NULL, NULL );
		png_init_io( png_ptr, fp );

		if (setjmp(png_jmpbuf(png_ptr))) {
			/* Free all of the memory associated
			 * with the png_ptr and info_ptr */
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			fclose(fp);
			/* If we get here, we had a
			 * problem reading the file */
			return false;
		}

		png_set_sig_bytes(png_ptr, sig_read);

		png_read_info(png_ptr, info_ptr);
		outWidth = png_get_image_width( png_ptr, info_ptr );
		outHeight = png_get_image_height( png_ptr, info_ptr );
		
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);

		fclose(fp);
		return true;
	}

	bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData) 
	{
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned int sig_read = 0;
		FILE *fp;

		if ((fp = AGKfopen(name, "rb")) == NULL)
		{
    		return false;
		}

		/* Create and initialize the png_struct
		 * with the desired error handler
		 * functions.  If you want to use the
		 * default stderr and longjump method,
		 * you can supply NULL for the last
		 * three parameters.  We also supply the
		 * the compiler header file version, so
		 * that we know if the application
		 * was compiled with a compatible version
		 * of the library.  REQUIRED
		 */
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, no_errmsg, no_warn);

		if (png_ptr == NULL) {
			fclose(fp);
			return false;
		}

		/* Allocate/initialize the memory
		 * for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			fclose(fp);
			png_destroy_read_struct(&png_ptr, 0, 0);
			return false;
		}

		/* Set up the output control if
		 * you are using standard C streams */
		png_set_read_fn( png_ptr, NULL, NULL );
		png_init_io( png_ptr, fp );

		/* Set error handling if you are
		 * using the setjmp/longjmp method
		 * (this is the normal method of
		 * doing things with libpng).
		 * REQUIRED unless you  set up
		 * your own error handlers in
		 * the png_create_read_struct()
		 * earlier.
		 */
		if (setjmp(png_jmpbuf(png_ptr))) {
			/* Free all of the memory associated
			 * with the png_ptr and info_ptr */
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			fclose(fp);
			/* If we get here, we had a
			 * problem reading the file */
			return false;
		}

		/* If we have already
		 * read some of the signature */
		png_set_sig_bytes(png_ptr, sig_read);

		/*
		 * If you have enough memory to read
		 * in the entire image at once, and
		 * you need to specify only
		 * transforms that can be controlled
		 * with one of the PNG_TRANSFORM_*
		 * bits (this presently excludes
		 * dithering, filling, setting
		 * background, and doing gamma
		 * adjustment), then you can read the
		 * entire image (including pixels)
		 * into the info structure with this
		 * call
		 *
		 * PNG_TRANSFORM_STRIP_16 |
		 * PNG_TRANSFORM_PACKING  forces 8 bit
		 * PNG_TRANSFORM_EXPAND forces to
		 *  expand a palette into RGB
		 */
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, 0);

		if ( !processPngImage( png_ptr, info_ptr, outWidth, outHeight, outHasAlpha, outData ) ) return false;

		/* Clean up after the read,
		 * and free any memory allocated */
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);

		/* Close the file */
		fclose(fp);

		/* That's it */
		return true;
	}

	void write_png(const char *file_name, int width, int height, uint32_t* bits)
	{
	   FILE *fp;
	   png_structp png_ptr;
	   png_infop info_ptr;

	   fp = AGKfopen(file_name, "wb");
	   if (fp == NULL) return;

	   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	   if (png_ptr == NULL)
	   {
		  fclose(fp);
		  return;
	   }

	   // Allocate/initialize the image information data.  REQUIRED
	   info_ptr = png_create_info_struct(png_ptr);
	   if (info_ptr == NULL)
	   {
		  fclose(fp);
		  png_destroy_write_struct(&png_ptr,  NULL);
		  return;
	   }

	   if (setjmp(png_jmpbuf(png_ptr)))
	   {
		  fclose(fp);
		  png_destroy_write_struct(&png_ptr, &info_ptr);
		  return;
	   }

	   // Set up the output control if you are using standard C streams
	   png_init_io(png_ptr, fp);

	   png_set_IHDR(png_ptr,
					info_ptr,
					width,
					height,
					8,
					PNG_COLOR_TYPE_RGB_ALPHA,
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT,
					PNG_FILTER_TYPE_DEFAULT);

	   png_uint_32 bytes_per_row = width * 4;
	   png_byte **row_pointers = (png_byte**) png_malloc(png_ptr, height * sizeof(png_byte *));
	   for (int y = 0; y < height; ++y)
	   {
		   unsigned char *row = (unsigned char*) png_malloc(png_ptr, sizeof(unsigned char) * bytes_per_row);
		   row_pointers[y] = (png_byte *)row;
		   for (int x = 0; x < width; ++x)
		   {
			   int index = y*width + x;
			   *row++ = bits[ index ] & 0xff;
			   *row++ = (bits[ index ] >> 8) & 0xff;
			   *row++ = (bits[ index ] >> 16) & 0xff;
			   *row++ = (bits[ index ] >> 24) & 0xff;
		   }
	   }

	   // Actually write the image data.
	   png_init_io(png_ptr, fp);
	   png_set_rows(png_ptr, info_ptr, row_pointers);
	   png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	   //Cleanup.
	   for (int y = 0; y < height; y++) {
		   png_free(png_ptr, row_pointers[y]);
	   }
	   png_free(png_ptr, row_pointers);

	   // Finish writing.
	   png_destroy_write_struct(&png_ptr, &info_ptr);
	   fclose(fp);
	}

	int ReadGifFile( GifFileType *file, unsigned char *buffer, int length )
	{
		cFile *pFile = (cFile*) file->UserData;
		return pFile->ReadData( (char*)buffer, length );
	}

	class ImageCacheItem
	{
		public:
			ImageCacheItem *m_pNextItem;
			uString m_sLoadFilename;
			float m_fScaleX;
			float m_fScaleY;
			int m_iMaxTexSize;
			unsigned char* m_pData;
			int m_iWidth;
			int m_iHeight;
			uString m_sFilename;
			
			ImageCacheItem() { m_pNextItem = 0; m_pData = 0; }
			~ImageCacheItem() { if ( m_pData ) delete [] m_pData; }
	};

	class ImageCacher : public AGKThread
	{
		public:
			static ImageCacheItem * volatile g_pImages;
            static ImageCacheItem * volatile g_pLastImage;
			static int volatile g_iCaching;

			static cCondition pCondition;
			static cSpinLock pLock;

			ImageCacher() : AGKThread() {}
			~ImageCacher() {}

			uint32_t Run()
			{
				while( !m_bTerminate )
				{
#if defined(AGK_IOS) || defined(AGK_MACOS)
                  @autoreleasepool {
#endif
					// wait for something to do
					pCondition.Lock();
					while( !g_pImages && !m_bTerminate )
					{
						pCondition.Wait();
					}
					pCondition.Unlock();
					if ( m_bTerminate ) return 0;

					ImageCacheItem *pCurr = 0;

					// find some work
					pLock.Acquire();
					if ( g_pImages )
					{
						pCurr = g_pImages;
						g_pImages = g_pImages->m_pNextItem;
                        if ( g_pLastImage == pCurr ) g_pLastImage = 0;
						g_iCaching = 1;
					}
					pLock.Release();

					// whilst there is work to do
					while ( pCurr && !m_bTerminate )
					{
						// process work
						if ( pCurr->m_pData )
                        {
                            if ( pCurr->m_iMaxTexSize > 0 )
                            {
                                uLong size = pCurr->m_iWidth*pCurr->m_iHeight*4;
                                unsigned char* tempbuf = new unsigned char[ size ];
                                int err = uncompress( tempbuf, &size, pCurr->m_pData, pCurr->m_iMaxTexSize );
                                delete [] pCurr->m_pData;
                                pCurr->m_pData = tempbuf;
                            }
                            cImage::PlatformSaveDataToFile( pCurr->m_sFilename, pCurr->m_pData, pCurr->m_iWidth, pCurr->m_iHeight );
                        }
						else
                        {
                            cImage::CacheNewSize( pCurr->m_sLoadFilename, pCurr->m_fScaleX, pCurr->m_fScaleY, pCurr->m_iMaxTexSize );
                        }
                        
                        delete pCurr; // also deletes the m_pData pointer
                        pCurr = 0;
						
						// look for more work
						pLock.Acquire();
						g_iCaching = 0;
						if ( g_pImages )
						{
							pCurr = g_pImages;
							g_pImages = g_pImages->m_pNextItem;
                            if ( g_pLastImage == pCurr ) g_pLastImage = 0;
							g_iCaching = 1;
						}
						pLock.Release();
					}
#if defined(AGK_IOS) || defined(AGK_MACOS)
                  }
#endif
				}

				g_iCaching = 0;
				return 0;
			}
	};
}

using namespace AGK;

ImageCacheItem* volatile ImageCacher::g_pImages = 0;
ImageCacheItem* volatile ImageCacher::g_pLastImage = 0;
int volatile ImageCacher::g_iCaching = 0;
cCondition ImageCacher::pCondition;
cSpinLock ImageCacher::pLock;
ImageCacher* cImage::g_pCachingThread = 0;

cImage* cImage::g_pAllGifImages = 0;

int cImage::g_iAlphaColThreshold = 128;
cImage* cImage::g_pAllImages = 0;
int cImage::g_iSavePixels = 0; // no longer used
int cImage::g_iLosingContext = 0;


uint32_t cImage::g_iCreated = 0;

//
// Internal functions
//

void cImage::Reset()
{
	m_iCreated = g_iCreated;
	g_iCreated++;

	// reset members
	m_iID = 0;
	m_pInternalResource = 0;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iOrigWidth = 0;
	m_iOrigHeight = 0;
    m_iTextureOverridden = 0;

	m_fU1 = 0.0f;
	m_fV1 = 0.0f;
	m_fU2 = 1.0f;
	m_fV2 = 1.0f;

	m_shaderVarTexBounds.SetVector4( 1, 1, 0, 0 ); // scale first then offset
	
	m_pNextImage = 0;
	m_pParentImage = 0;
	m_pSubImages = 0;

	m_pBoundingPoints = 0;
	m_iNumPoints = 0;
	m_iMaxNumPoints = 0;

	m_pAlphaPixels = 0;
	m_fLastU1 = 0;
	m_fLastV1 = 0;
	m_fLastU2 = 0;
	m_fLastV2 = 0;

	m_bResized = false;
	
	m_fScaledAmount = 1.0f;
    m_iSpecialLoadMode = 0;
	m_bIsCubeMap = false;
	m_iNoMissingImage = 0;

	m_pCompressedPixelData = 0;
	m_iCompressedLength = 0;
	m_iCompressed = 0;

	m_pGifFrames = 0;
	m_iGifNumFrames = 0;
	m_iGifCurrFrame = 0;
	m_iGifFramesArraySize = 0;
	m_fGifTime = 0;
	m_pPrevGifImage = 0;
	m_pNextGifImage = 0;

	if ( g_pAllImages ) g_pAllImages->m_pPrevImage = this;
	m_pPrevImage = 0;
	m_pNextImage = g_pAllImages;
	g_pAllImages = this;
	//m_cAllImages.AddItem( this, m_iCreated );

	m_iFlags = 0;
	if ( agk::m_iDefaultMinFilter == 0 ) m_iFlags |= AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST;
	if ( agk::m_iDefaultMagFilter == 0 ) m_iFlags |= AGK_IMAGE_MAG_FILTER_NEAREST;

	m_bHasAlpha = false;

	m_iImageMode = 0;

	m_bDeleting = false;
	m_pChildren = 0;
	m_bDontLink = false;

	m_pLetterImages = 0;
	m_pLetterImagesExt = 0;

	m_pCurrentSprites = 0;
	m_pCurrentTextObjects = 0;
}

void cImage::SetCompressedPixelData( unsigned char *pixels, unsigned int size )
{
	if ( m_pCompressedPixelData ) delete [] m_pCompressedPixelData;
	m_pCompressedPixelData = 0;
	m_iCompressedLength = 0;
	m_iCompressed = 0;

	if ( !pixels ) return;
    
    uLong upperBound = compressBound(size);
	unsigned char *tempbuf = new unsigned char[ upperBound ];
	int err = compress2( tempbuf, &upperBound, pixels, size, 2 );
	if ( err == Z_OK )
	{
		m_pCompressedPixelData = new unsigned char[ upperBound ];
		memcpy( m_pCompressedPixelData, tempbuf, upperBound );
		delete [] tempbuf;
		m_iCompressedLength = (uint32_t) upperBound;
	}

	if ( !m_pCompressedPixelData ) return;

	m_iCompressed = 1;
}

void cImage::SetCachePixelData( unsigned char *pixels, int width, int height, const char* filename, int compress )
{
	// don't cache on HTML5 as there is no persistent storage
#ifdef AGK_HTML5
	delete [] pixels;
	return;
#endif

	if ( !g_pCachingThread )
	{
		g_pCachingThread = new ImageCacher();
		g_pCachingThread->Start();
	}

	if ( !g_pCachingThread->IsRunning() ) 
	{
		delete [] pixels;
		return;
	}
    
    int compressedSize = 0;
    if ( compress )
    {
        int size = width*height*4;
        uLong upperBound = compressBound(size);
        unsigned char *tempbuf = new unsigned char[ upperBound ];
        int err = compress2( tempbuf, &upperBound, pixels, size, 1 );
        if ( err == Z_OK )
        {
            delete [] pixels;
            pixels = tempbuf;
            tempbuf = new unsigned char[ upperBound ];
            memcpy( tempbuf, pixels, upperBound );
            delete [] pixels;
            pixels = tempbuf;
            compressedSize = upperBound;
        }
    }
    
	// check it isn't already in the queue
	ImageCacher::pLock.Acquire();
	ImageCacheItem *pItem = ImageCacher::g_pImages;
	bool bFound = false;
	while( pItem )
	{
		if ( pItem->m_sFilename.CompareTo( filename ) == 0 )
		{
			bFound = true;
			break;
		}
		pItem = pItem->m_pNextItem;
	}
	ImageCacher::pLock.Release();

	if ( bFound ) return;

	// tell thread to compress it
	ImageCacheItem *pNewItem = new ImageCacheItem();
	pNewItem->m_pData = pixels;
	pNewItem->m_iWidth = width;
	pNewItem->m_iHeight = height;
    pNewItem->m_iMaxTexSize = compressedSize;
	pNewItem->m_sFilename.SetStr( filename );

	ImageCacher::pLock.Acquire();
	pNewItem->m_pNextItem = 0;
    if ( ImageCacher::g_pLastImage ) ImageCacher::g_pLastImage->m_pNextItem = pNewItem;
    else ImageCacher::g_pImages = pNewItem;
    ImageCacher::g_pLastImage = pNewItem;
	ImageCacher::pLock.Release();

	ImageCacher::pCondition.Lock();
	ImageCacher::pCondition.Signal();
	ImageCacher::pCondition.Unlock();
}

//
// Public functions
//

void cImage::LoadFromData( int width, int height, unsigned int *bits, int format )
{
	m_iImageMode = 0;
	int numBytes = 4;
	if ( format == 2 ) 
	{
		m_iImageMode = 2;
		numBytes = 1;
	}

	m_iOrigWidth = width;
	m_iOrigHeight = height;
	m_bResized = false;
	m_bHasAlpha = false;

	m_iFlags = 0;
	if ( agk::m_iDefaultMinFilter == 0 ) m_iFlags |= AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST;
	if ( agk::m_iDefaultMagFilter == 0 ) m_iFlags |= AGK_IMAGE_MAG_FILTER_NEAREST;
	if ( agk::m_iDefaultWrapU == 1 ) m_iFlags |= AGK_IMAGE_WRAP_U_REPEAT;
	if ( agk::m_iDefaultWrapV == 1 ) m_iFlags |= AGK_IMAGE_WRAP_V_REPEAT;
	if ( agk::m_bGenerateMipmaps && !m_pGifFrames && format != 2 ) m_iFlags |= AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP;
	
	int texMaxSize = agk::GetRenderer()->GetMaxTextureSize();
	
	m_fScaledAmount = 1.0f;
	float deviceScale = agk::PlatformDeviceScale();
	if ( deviceScale == 0 ) deviceScale = 1;

	bool copied = 0;
	
	// reduce to reported max size
	if ( format == 0 && ((int)m_iOrigWidth > texMaxSize || (int)m_iOrigHeight > texMaxSize || deviceScale < 1) )
	{
		int newWidth, newHeight;

		if ( m_iOrigHeight > m_iOrigWidth )
		{
			float scaleFactor = texMaxSize / (float)m_iOrigHeight;
			if ( deviceScale < scaleFactor ) scaleFactor = deviceScale;
			newWidth = agk::Round(m_iOrigWidth * scaleFactor);
			newHeight = agk::Round(m_iOrigHeight * scaleFactor);
		}
		else
		{
			float scaleFactor = texMaxSize / (float)m_iOrigWidth;
			if ( deviceScale < scaleFactor ) scaleFactor = deviceScale;
			newWidth = agk::Round(m_iOrigWidth * scaleFactor);
			newHeight = agk::Round(m_iOrigHeight * scaleFactor);
		}

		uString str;
		str.Format( "Image %s is too large, resizing to %dx%d", m_szFile.GetStr(), newWidth, newHeight );
		agk::Warning( str );

		m_fScaledAmount = newWidth / (float)m_iOrigWidth;

		unsigned int *newData = new unsigned int[ newWidth * newHeight ];
		CommonResize( (unsigned char*)bits, m_iOrigWidth, m_iOrigHeight, (unsigned char*)newData, newWidth, newHeight );
		bits = newData;
		copied = 1;

		m_iOrigWidth = newWidth;
		m_iOrigHeight = newHeight;
	}

	uint32_t offsetX = 0;
	uint32_t offsetY = 0;

	if ( ((m_iFlags & AGK_IMAGE_USE_MIPMAP) && !agk::GetRenderer()->SupportsMipmapNPOT()) || !agk::GetRenderer()->SupportsNPOT() )
	{
		// make the image power of 2
		m_iWidth = 1;
		m_iHeight = 1;
		while ( m_iWidth < m_iOrigWidth ) m_iWidth = m_iWidth << 1;
		while ( m_iHeight < m_iOrigHeight ) m_iHeight = m_iHeight << 1;
		if ( m_iHeight != m_iOrigWidth || m_iHeight != m_iOrigHeight ) 
		{
			m_bResized = true;

			offsetX = (m_iWidth - m_iOrigWidth) / 2;
			offsetY = (m_iHeight - m_iOrigHeight) / 2;

			m_fU1 = offsetX / (float) m_iWidth;
			m_fV1 = offsetY / (float) m_iHeight;
			m_fU2 = m_fU1 + (m_iOrigWidth / (float) m_iWidth);
			m_fV2 = m_fV1 + (m_iOrigHeight / (float) m_iHeight);

			m_shaderVarTexBounds.SetVector4( m_fU2 - m_fU1, m_fV2 - m_fV1, m_fU1, m_fV1 );

			m_iFlags |= AGK_IMAGE_HAS_MODIFIED_UVS;
		}
	}
	else
	{
		m_iWidth = m_iOrigWidth;
		m_iHeight = m_iOrigHeight;
		m_iFlags &= ~AGK_IMAGE_HAS_MODIFIED_UVS;
	}
	
	if ( !m_bResized )
	{
		if ( format == 0 )
		{
			CreateAlphaPixels( m_iOrigHeight * m_iOrigWidth );
			for ( uint32_t y = 0; y < m_iOrigHeight; y++ )
			{
				uint32_t index = y*m_iOrigWidth;

				for ( uint32_t x = 0; x < m_iOrigWidth; x++ )
				{
					unsigned char alpha = bits[index] >> 24;
					SetAlphaPixel( index, alpha );
					index++;
				}
			}
		}
		else if ( format == 2 ) m_bHasAlpha = true;
	}
	else
	{
		uint32_t *bits2 = new uint32_t[ m_iWidth * m_iHeight ];

		CreateAlphaPixels( m_iOrigHeight * m_iOrigWidth );
		for ( uint32_t y = 0; y < m_iOrigHeight; y++ )
		{
			uint32_t index = y*m_iOrigWidth;
			uint32_t index2 = (y+offsetY)*m_iWidth + offsetX;

			for ( uint32_t x = 0; x < m_iOrigWidth; x++ )
			{
				unsigned char alpha = bits[index] >> 24;
				SetAlphaPixel( index, alpha );
				
				bits2[ index2 ] = bits[ index ];
				index++;
				index2++;
			}
		}

		if ( copied ) delete [] bits;
		bits = bits2;
		copied = 1;	

		// copy edge pixels across image border
		for ( unsigned int y = offsetY; y < m_iHeight-offsetY; y++ )
		{
			unsigned int index = y*m_iWidth;
			unsigned int index2 = y*m_iWidth + offsetX;
			unsigned int color = bits[ index2 ];

			for ( unsigned int x = 0; x < offsetX; x++ )
			{
				bits[ index++ ] = color;
			}
		}

		for ( unsigned int y = offsetY; y < m_iHeight-offsetY; y++ )
		{
			unsigned int index = y*m_iWidth + offsetX + m_iOrigWidth;
			unsigned int index2 = y*m_iWidth + m_iOrigWidth + offsetX - 1;
			unsigned int color = bits[ index2 ];

			for ( unsigned int x = offsetX+m_iOrigWidth; x < m_iWidth; x++ )
			{
				bits[ index++ ] = color;
			}
		}

		for ( unsigned int y = 0; y < offsetY; y++ )
		{
			unsigned int index = y*m_iWidth;
			unsigned int index2 = offsetY*m_iWidth;

			for ( unsigned int x = 0; x < m_iWidth; x++ )
			{
				bits[ index++ ] = bits[ index2++ ];
			}
		}

		for ( unsigned int y = m_iOrigHeight+offsetY; y < m_iHeight; y++ )
		{
			unsigned int index = y*m_iWidth;
			unsigned int index2 = (m_iOrigHeight+offsetY-1)*m_iWidth;

			for ( unsigned int x = 0; x < m_iWidth; x++ )
			{
				bits[ index++ ] = bits[ index2++ ];
			}
		}
	}
	
	//SetCompressedPixelData( (unsigned char*)bits, m_iWidth*m_iHeight*4 );
	SetCompressedPixelData( 0, 0 );
	
	AGKImgFormat agkFormat = AGK_COLOR_FORMAT_RGBA_8;
	if ( m_iImageMode == 2 ) agkFormat = AGK_COLOR_FORMAT_R_8;

	uint32_t mask = AGK_IMAGE_WRAP_U_REPEAT | AGK_IMAGE_WRAP_V_REPEAT | AGK_IMAGE_MAG_FILTER_NEAREST | AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST | AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP;
	uint32_t flags = m_iFlags & mask;
	agk::GetRenderer()->CreateImage2D( agkFormat, m_iWidth, m_iHeight, (unsigned char*)bits, flags, &m_pInternalResource );

	if ( copied ) delete [] bits;
}

void cImage::LoadSubRegionFromData( int x, int y, int width, int height, unsigned char *bits )
{
	if ( m_pInternalResource == 0 ) return;

	agk::GetRenderer()->UpdateSubImage2D( m_pInternalResource, 0, x, y, width, height, bits );
}

void cImage::ReloadFromData()
{
	//if ( !m_pCompressedPixelData ) return;  still want to create the image even if it doesn't have data
	if ( m_pParentImage ) return;

	uLong size = m_iWidth*m_iHeight*4;
	if ( m_iImageMode == 2 ) size = m_iWidth*m_iHeight;
		
	unsigned char* tempbuf = new unsigned char[ size ];
	if ( m_pCompressedPixelData ) 
	{
		int err = uncompress( tempbuf, &size, m_pCompressedPixelData, m_iCompressedLength );
		if ( err != Z_OK )
		{
			delete [] tempbuf;
			agk::Warning( "Failed to extract compressed image data" );
			return;
		}
	}
	else
	{
		// if no data exists for the image create it as a blank image
		for( unsigned int i = 0; i < size; i++ )
		{
			tempbuf[ i ] = 0;
		}
	}

	uint32_t mask = AGK_IMAGE_WRAP_U_REPEAT | AGK_IMAGE_WRAP_V_REPEAT | AGK_IMAGE_MAG_FILTER_NEAREST | AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST | AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP;
	uint32_t flags = m_iFlags & mask;

	if ( m_iImageMode == 1 ) agk::GetRenderer()->CreateDepthBuffer( AGK_DEPTH_FORMAT_32_FLOAT, m_iWidth, m_iHeight, flags, &m_pInternalResource );
	else
	{
		AGKImgFormat format = AGK_COLOR_FORMAT_RGBA_8;
		if ( m_iImageMode == 2 ) format = AGK_COLOR_FORMAT_R_8;

		agk::GetRenderer()->CreateImage2D( format, m_iWidth, m_iHeight, tempbuf, flags, &m_pInternalResource );
	}

	delete [] tempbuf;
}

//****if* cImage/Image
// FUNCTION
//   Create an blank image, not suitable for rendering
// SOURCE
cImage::cImage()
//****
{
	// call members reset
	Reset();
}

cImage::cImage( const char* szFile )
{
	// reset and load image
	Reset();
	Load( szFile );
}

//****if* cImage/Image
// FUNCTION
//   Create an image from a file stored in the device's media folder
// INPUTS
//   szFile -- A string pointing to the image file using a relative path
// SOURCE
cImage::cImage( const uString &szFile )
//****
{
	// reset and load image
	Reset();
	Load( szFile );
}

cImage::~cImage()
{
	m_bDeleting = true;

	// make sure the image list doesn't contain this image
	if ( m_iID > 0 ) agk::m_cImageList.RemoveItem( m_iID );

	AGKFrameBuffer::DeleteImage( this );

	// make sure no sprites are using this
	//cSprite::RemoveImage( this ); // is this necessary? Sprites using it should be listed below
	cText::GlobalImageDeleting( this );
	if ( m_pParentImage != NULL ) 
	{
		// notify parent that a child is being deleted
		if ( !m_bDontLink ) m_pParentImage->RemoveChild( this );
	}
	else
	{
		// delete all children
		cImageLink *pLink = 0;
		while ( m_pChildren )
		{
			pLink = m_pChildren;
			m_pChildren = m_pChildren->m_pNext;
			
			if ( pLink->m_pImage->m_iID > 0 ) agk::DeleteImage( pLink->m_pImage->m_iID );
			else delete pLink->m_pImage;

			delete pLink;
		}
	}

	// tell all sprites this image will no longer exist
	if ( m_pCurrentSprites )
	{
		cSprite *pLink = m_pCurrentSprites->GetFirst();
		while ( pLink )
		{
			pLink->ImageDeleting( this );
			pLink = m_pCurrentSprites->GetNext();
		}
		m_pCurrentSprites->ClearAll();
		delete m_pCurrentSprites;
		m_pCurrentSprites = 0;
	}
	
	// tell all text objects this image will no longer exist
	if ( m_pCurrentTextObjects )
	{
		cText *pLink2 = m_pCurrentTextObjects->GetFirst();
		while ( pLink2 )
		{
			pLink2->ImageDeleting( this );
			pLink2 = m_pCurrentTextObjects->GetNext();
		}
		m_pCurrentTextObjects->ClearAll();
		delete m_pCurrentTextObjects;
		m_pCurrentTextObjects = 0;
	}

	// delete gif frames
	if ( m_pGifFrames )
	{
		for( unsigned int i = 0; i < m_iGifNumFrames; i++ ) delete m_pGifFrames[ i ];
		delete [] m_pGifFrames;
		m_pGifFrames = 0;
	}

	// delete font letter images
	if ( m_pLetterImages )
	{
		for ( uint32_t i = 0; i < 96; i++ ) delete m_pLetterImages[ i ];
		delete [] m_pLetterImages;
		m_pLetterImages = 0;
	}

	if ( m_pLetterImagesExt )
	{
		for ( uint32_t i = 0; i < 128; i++ ) delete m_pLetterImagesExt[ i ];
		delete [] m_pLetterImagesExt;
		m_pLetterImagesExt = 0;
	}

	// delete any sub image listings
	while ( m_pSubImages ) 
	{
		cSubImage *pSubImage = m_pSubImages;
		m_pSubImages = m_pSubImages->pNextSubImage;
		delete pSubImage;
	}

	// sub images don't need deleting
	if ( m_pParentImage == 0 && m_pInternalResource ) agk::GetRenderer()->DeleteImage( m_pInternalResource );
	m_pInternalResource = 0;

	while ( m_pBoundingPoints ) 
	{
		Point2D *pPoint = m_pBoundingPoints;
		m_pBoundingPoints = m_pBoundingPoints->pNext;
		delete pPoint;
	}

	if ( m_pAlphaPixels ) delete [] m_pAlphaPixels;
	if ( m_pCompressedPixelData ) delete [] m_pCompressedPixelData;

	// remove from global list
	if ( m_pPrevImage ) m_pPrevImage->m_pNextImage = m_pNextImage;
	else g_pAllImages = m_pNextImage;
	if ( m_pNextImage ) m_pNextImage->m_pPrevImage = m_pPrevImage;

	// remove from GIF images list, if applicable
	if ( m_pPrevGifImage ) m_pPrevGifImage->m_pNextGifImage = m_pNextGifImage;
	else if ( g_pAllGifImages == this ) g_pAllGifImages = m_pNextGifImage;
	if ( m_pNextGifImage ) m_pNextGifImage->m_pPrevGifImage = m_pPrevGifImage;
}

void cImage::OverrideTexture( void* texID, int width, int height, AGKImgFormat format )
{
	if ( !m_iTextureOverridden && m_pInternalResource )
    {
        agk::GetRenderer()->DeleteImage( m_pInternalResource );
        m_pInternalResource = 0;
    }
    
    m_iTextureOverridden = 1;
    
    m_iWidth = width;
    m_iHeight = height;
    m_iOrigWidth = width;
    m_iOrigHeight = height;
    
    m_fU1 = 0;
    m_fV1 = 0;
    m_fU2 = 1;
    m_fV2 = 1;

	m_shaderVarTexBounds.SetVector4( m_fU2 - m_fU1, m_fV2 - m_fV1, m_fU1, m_fV1 );

	m_iFlags &= ~AGK_IMAGE_HAS_MODIFIED_UVS;

	agk::GetRenderer()->OverrideTexture( m_iWidth, m_iHeight, format, texID, &m_pInternalResource );
    
    if ( texID != 0 )
    {
        SetWrapU( 0 );
		SetWrapV( 0 );
		SetMinFilter( 1 );
		SetMagFilter( 1 );
    }
}

void cImage::AddChild( cImage *pChild )
{
	cImageLink* newLink = new cImageLink();
	newLink->m_pNext = m_pChildren;
	newLink->m_pImage = pChild;
	m_pChildren = newLink;
}

void cImage::RemoveChild( cImage *pChild )
{
	// if this child is being deleted because the parent is in the process of being deleted then do nothing.
	if ( m_bDeleting ) return;

	cImageLink *pLink = m_pChildren;
	cImageLink *pLast = 0;
	while ( pLink )
	{
		if ( pLink->m_pImage == pChild )
		{
			if ( pLast ) pLast->m_pNext = pLink->m_pNext;
			else m_pChildren = pLink->m_pNext;
			delete pLink;
			break;
		}

		pLast = pLink;
		pLink = pLink->m_pNext;
	}
}

void cImage::AddSprite( cSprite *pSprite )
{
	if ( !m_pCurrentSprites ) m_pCurrentSprites = new cHashedList<cSprite*>(256);

	// don't add a sprite more than once
	if ( m_pCurrentSprites->GetItem( pSprite->GetCreated() ) ) return;

	m_pCurrentSprites->AddItem( pSprite, pSprite->GetCreated() );
}

void cImage::RemoveSprite( cSprite *pSprite )
{
	// if this sprite is being removed because the image is in the process of being deleted then do nothing.
	if ( m_bDeleting ) return;
	if ( !m_pCurrentSprites ) return;

	m_pCurrentSprites->RemoveItem( pSprite->GetCreated() );
}

void cImage::AddText( cText *pText )
{
	if ( !m_pCurrentTextObjects ) m_pCurrentTextObjects = new cHashedList<cText*>(256);

	// don't add a text object more than once
	if ( m_pCurrentTextObjects->GetItem( pText->GetCreated() ) ) return;

	m_pCurrentTextObjects->AddItem( pText, pText->GetCreated() );
}

void cImage::RemoveText( cText *pText )
{
	if ( !m_pCurrentTextObjects ) return;

	// if this text is being removed because the image is in the process of being deleted then do nothing.
	if ( m_bDeleting ) return;

	m_pCurrentTextObjects->RemoveItem( pText->GetCreated() );
}

void cImage::DeleteAllImages()
{
	// delete
	cImage *pImage = g_pAllImages;
	while ( pImage )
	{
		if ( !pImage->HasParent() )
		{
			agk::GetRenderer()->DeleteImage( pImage->m_pInternalResource );
			pImage->m_pInternalResource = 0;
		}
		pImage = pImage->m_pNextImage;
	}

	agk::GetRenderer()->ClearImageBindings();
}
/*
void cImage::SaveAllImages()
{
	g_iLosingContext = 1;

	cImage *pImage = g_pAllImages;
	while ( pImage )
	{
		if ( !pImage->HasParent() && !pImage->m_pCompressedPixelData ) 
		{
			unsigned char *pData = 0;
			int size = agk::GetRenderer()->GetImageData( &pImage->m_pInternalResource, (uint8_t**) &pData );
			if ( size > 0 )
			{
				pImage->SetCompressedPixelData( pData, size );
				delete [] pData;
			}
		}
		
		pImage = pImage->m_pNextImage;
	}

	g_iLosingContext = 0;
}
*/

void cImage::UpdateGifImages()
{
	float currTime = agk::Timer();
	cImage *pGifImage = g_pAllGifImages;
	while( pGifImage )
	{
		pGifImage->UpdateGifImage();
		pGifImage = pGifImage->m_pNextGifImage;
	}
}

void cImage::UpdateGifImage()
{
	if ( m_iGifNumFrames <= 1 ) return;

	if ( m_fGifTime == 0 ) 
	{
		m_fGifTime = agk::Timer();
		return;
	}
	
	float currTime = agk::Timer();
	unsigned int nextFrame = m_iGifCurrFrame + 1;
	if ( nextFrame >= m_iGifNumFrames ) nextFrame = 0;
	if ( m_fGifTime + m_pGifFrames[m_iGifCurrFrame]->fDelay < currTime || m_fGifTime-0.1f > currTime ) // currTime may return to 0 at any time due to ResetTimer()
	{
		// extract new frame
		if ( m_pGifFrames[ nextFrame ]->pData )
		{
			unsigned long realSize = m_iOrigWidth * m_iOrigHeight * 4;
			unsigned char* pNewData = new unsigned char[ realSize ];
			uncompress( pNewData, &realSize, m_pGifFrames[nextFrame]->pData, m_pGifFrames[nextFrame]->iCompressedSize );

			LoadFromData( m_iOrigWidth, m_iOrigHeight, (unsigned int*)pNewData );
			delete [] pNewData;

			SetCompressedPixelData( 0, 0 );
		}

		m_iGifCurrFrame = nextFrame;
		m_fGifTime = currTime;
	}
}

void cImage::CreateColorImage( uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha )
{
	if ( red > 255 ) red = 255;
	if ( green > 255 ) green = 255;
	if ( blue > 255 ) blue = 255;
	if ( alpha > 255 ) alpha = 255;

	uint32_t data = (alpha << 24) | (blue << 16) | (green << 8) | red;
	LoadFromData( 1, 1, &data );
}

void cImage::CreateBlankImage( uint32_t width, uint32_t height, uint32_t format, uint32_t mipmap )
{
	if ( width < 1 ) width = 1;
	if ( height < 1 ) height = 1;

	if ( (int)width > agk::GetRenderer()->GetMaxTextureSize() || (int)height > agk::GetRenderer()->GetMaxTextureSize() ) 
	{
		agk::Error( "Failed to create image, width or height is greater than maximum size supported by the device" );
		return;
	}

	m_iImageMode = 0;
	if ( format == 1 ) m_iImageMode = 1;

	if ( m_iImageMode == 1 ) mipmap = 0;

	if ( mipmap && !agk::GetRenderer()->SupportsMipmapNPOT() )
	{
		if ( (width & (width-1)) != 0 || (height & (height-1)) != 0 )
		{
			mipmap = 0;
			agk::Warning( "Renderer does not support non-power of 2 mipmaps, mipmapping on this image will be turned off" );
		}
	}

	m_iOrigWidth = width;
	m_iOrigHeight = height;
	m_iWidth = width;
	m_iHeight = height;
	m_bResized = false;

	m_fU1 = 0;
	m_fV1 = 0;
	m_fU2 = 1;
	m_fV2 = 1;

	m_shaderVarTexBounds.SetVector4( m_fU2 - m_fU1, m_fV2 - m_fV1, m_fU1, m_fV1 );

	m_iFlags = 0;
	if ( agk::m_iDefaultMinFilter == 0 ) m_iFlags |= AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST;
	if ( agk::m_iDefaultMagFilter == 0 ) m_iFlags |= AGK_IMAGE_MAG_FILTER_NEAREST;
	if ( agk::m_iDefaultWrapU == 1 ) m_iFlags |= AGK_IMAGE_WRAP_U_REPEAT;
	if ( agk::m_iDefaultWrapV == 1 ) m_iFlags |= AGK_IMAGE_WRAP_V_REPEAT;
	if ( mipmap ) m_iFlags |= AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP;

	uint32_t mask = AGK_IMAGE_WRAP_U_REPEAT | AGK_IMAGE_WRAP_V_REPEAT | AGK_IMAGE_MAG_FILTER_NEAREST | AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST | AGK_IMAGE_GEN_MIPMAP | AGK_IMAGE_USE_MIPMAP;
	uint32_t flags = m_iFlags & mask;
	flags |= AGK_IMAGE_RENDERABLE;

	if ( m_iImageMode == 1 ) 
	{
		AGKImgFormat format;
		if ( agk::GetRenderer()->IsImageFormatSupported( AGK_DEPTH_FORMAT_32_FLOAT ) ) format = AGK_DEPTH_FORMAT_32_FLOAT;
		else if ( agk::GetRenderer()->IsImageFormatSupported( AGK_DEPTH_FORMAT_24_INT ) ) format = AGK_DEPTH_FORMAT_24_INT;
		else format = AGK_DEPTH_FORMAT_16_INT;
			
		agk::GetRenderer()->CreateDepthBuffer( format, width, height, flags, &m_pInternalResource );
	}
	else agk::GetRenderer()->CreateImage2D( AGK_COLOR_FORMAT_RGBA_8, width, height, 0, flags, &m_pInternalResource );
	
	SetCompressedPixelData( 0, 0 );
}

// return value will be width and height packed into the top 16bits and low 16bits respectively
unsigned int cImage::GetImageSizeFromFile( const char* filename )
{
	uString sPath( filename );

	if ( !agk::GetRealPath( sPath ) )
	{
		uString err; err.Format( "Could not find image: %s", filename );
		agk::Error( err );
		return 0;
	}

	int width;
	int height;
	
	bool result = false;
	const char *szExt = strrchr( filename, '.' );
	char *szExtL = agk::Lower( szExt );
	
	if ( strcmp( szExtL, ".png" ) == 0 ) result = getPngImageSize( sPath.GetStr(), width, height );
	else if ( strcmp( szExtL, ".jpg" ) == 0 || strcmp( szExtL, ".jpeg" ) == 0 ) result = getJpegImageSize( sPath.GetStr(), width, height );
	else if ( strcmp( szExtL, ".gif" ) == 0 )
	{
		GifFileType *pGif;
		//pGif = ::DGifOpenFileName( sPath.GetStr() );
		cFile sFile; sFile.OpenToRead( filename );
		pGif = ::DGifOpen( &sFile, ReadGifFile );
		if ( pGif )
		{
			width = pGif->SWidth;
			height = pGif->SHeight;
			DGifCloseFile( pGif );
			result = true;
		}
	}
	else
	{
		delete [] szExtL;
		agk::Error( "Unsupported image format, only PNG, JPG, and GIF are allowed" );
		return 0;
	}

	delete [] szExtL;

	if ( !result )
	{
		uString err; err.Format( "Failed to load image file: %s", sPath.GetStr() );
		agk::Error( err );
		return 0;
	}

	return (width << 16) | (height & 0xFFFF);
}

int cImage::GetBMPFromFile( cFile* pFile, unsigned char** imageData, unsigned int* width, unsigned int* height )
{
	unsigned char bmp_file_header[14];
	unsigned char bmp_info_start[4];
	unsigned char bmp_info_header[64];
	unsigned char bmp_pad[3];

	unsigned int w, h, bitsperpixel;

	memset(bmp_file_header, 0, sizeof(bmp_file_header));
	memset(bmp_info_header, 0, sizeof(bmp_info_header));
	memset(bmp_info_start, 0, sizeof(bmp_info_start));

	if ( pFile->ReadData( (char*) bmp_file_header, sizeof(bmp_file_header) ) <= 0 ) return 1;
	if ( pFile->ReadData( (char*) bmp_info_start, sizeof(bmp_info_start) ) <= 0 ) return 1;
	
	unsigned int header_size = (bmp_info_start[0] + (bmp_info_start[1] << 8) + (bmp_info_start[2] << 16) + (bmp_info_start[3] << 24));
	if (header_size != 12 && header_size != 40 && header_size != 64) return 1;

	if ( pFile->ReadData( (char*) bmp_info_header+4, header_size-4 ) <= 0 ) return 1;

	if ((bmp_file_header[0] != 'B') || (bmp_file_header[1] != 'M')) return 2;

	//PE: Added-Decode OS/2 1.x header 12 bytes headers.
	//PE: Missing-colormapped image.bmp_info_header[10] == 8
	if (header_size == 12) 
	{
		if ((bmp_info_header[10] != 24) && (bmp_info_header[10] != 32)) return 3;
		bitsperpixel = bmp_info_header[10];
		w = (bmp_info_header[4] + (bmp_info_header[5] << 8)); // +(bmp_info_header[6] << 16) + (bmp_info_header[7] << 24));
		h = (bmp_info_header[6] + (bmp_info_header[7] << 8)); // +(bmp_info_header[10] << 16) + (bmp_info_header[11] << 24));
	} 
	else 
	{
		if ((bmp_info_header[14] != 24) && (bmp_info_header[14] != 32)) return 3;
		bitsperpixel = bmp_info_header[14];
		w = (bmp_info_header[4] + (bmp_info_header[5] << 8) + (bmp_info_header[6] << 16) + (bmp_info_header[7] << 24));
		h = (bmp_info_header[8] + (bmp_info_header[9] << 8) + (bmp_info_header[10] << 16) + (bmp_info_header[11] << 24));
	}

	if ( w > 0 && h > 0 )
	{
		*imageData = new unsigned char[ w * h * 4 ];
		if ( !*imageData ) return 4;

		//PE: Added support for 24-bit to 32-bit conversion.
		int iDataSize = (bitsperpixel == 24) ? 3 : 4;
		
		for( unsigned int y = (h - 1); y != -1; y-- )
		{
			for( unsigned int x = 0; x < w; x++ )
			{
				unsigned int i = (x + y * w) * 4;

				if ( pFile->ReadData( (char*) (*imageData)+i, iDataSize ) <= 0 ) return 5;

				(*imageData)[i] ^= (*imageData)[i + 2] ^= (*imageData)[i] ^= (*imageData)[i + 2]; // BGR -> RGB
				(*imageData)[i + 3] = 255; // Alpha
			}

			unsigned int padding = ((4 - (w * 3) % 4) % 4);

			if ( pFile->ReadData( (char*) bmp_pad, padding ) <= 0 ) return 5;
		}
	}

	*width = w;
	*height = h;

	return 0;
}

bool cImage::Load( const uString &szFile, bool bBlackToAlpha )
{
	return Load( szFile.GetStr(), bBlackToAlpha );
}

//****if* cImage/Load
// FUNCTION
//   Loads an image from a file, can also be used to load an atlas texture that will be used by sub images.
//   If loading an atlas texture a subimages.txt file must exist detailing all the images contained on it.
// INPUTS
//   szFile -- The filename of the image to load
// SOURCE
bool cImage::Load( const char* szFile, bool bBlackToAlpha )
//****
{
	if ( strcmp(szFile, "/ascii.png") == 0 || strcmp(szFile, "ascii.png") == 0 ) return LoadPNGFromMemory( libImageAscii, 0, 0 );
	if ( strcmp(szFile, "/asciiExt.png") == 0 || strcmp(szFile, "asciiExt.png") == 0 ) return LoadPNGFromMemory( libImageAsciiExt, 0, 0 );

	if ( strcmp(szFile, "/JoystickOuter.png") == 0 || strcmp(szFile, "JoystickOuter.png") == 0 ) return LoadPNGFromMemory( libImageJoystickOuterPNG, 0, 0 );
	if ( strcmp(szFile, "/JoystickInner.png") == 0 || strcmp(szFile, "JoystickInner.png") == 0 ) return LoadPNGFromMemory( libImageJoystickInnerPNG, 0, 0 );
	if ( strcmp(szFile, "/Button.png") == 0 || strcmp(szFile, "Button.png") == 0 ) return LoadPNGFromMemory( libImageButtonPNG, 0, 0 );
	if ( strcmp(szFile, "/ButtonDown.png") == 0 || strcmp(szFile, "ButtonDown.png") == 0 ) return LoadPNGFromMemory( libImageButtonDownPNG, 0, 0 );
	if ( strcmp(szFile, "/Arial.png") == 0 || strcmp(szFile, "Arial.png") == 0 ) return LoadPNGFromMemory( libImageArialPNG, libArialSubImages, 95 );
	if ( strcmp(szFile, "/Missing.png") == 0 || strcmp(szFile, "Missing.png") == 0 ) return LoadPNGFromMemory( libImageMissingPNG, 0, 0 );
	
#ifndef AGK_HTML5
	if ( strcmp(szFile, "/Avenir.png") == 0 || strcmp(szFile, "Avenir.png") == 0 ) return LoadPNGFromMemory( libImageAvenirPNG, libAvenirSubImages, 95 );
	if ( strcmp(szFile, "/interpreter-backdrop.png") == 0 || strcmp(szFile, "interpreter-backdrop.png") == 0 ) return LoadPNGFromMemory( libImageInterpreterBackgroundPNG, 0, 0 );
	if ( strcmp(szFile, "/interpreter-power.png") == 0 || strcmp(szFile, "interpreter-power.png") == 0 ) return LoadPNGFromMemory( libImageInterpreterPowerPNG, 0, 0 );
	if ( strcmp(szFile, "/interpreter-logo.png") == 0 || strcmp(szFile, "interpreter-logo.png") == 0 ) return LoadPNGFromMemory( libImageInterpreterLogoPNG, 0, 0 );
	if ( strcmp(szFile, "/interpreter-spinner.png") == 0 || strcmp(szFile, "interpreter-spinner.png") == 0 ) return LoadPNGFromMemory( libImageInterpreterSpinnerPNG, 0, 0 );
	if ( strcmp(szFile, "/AGK-Help.png") == 0 || strcmp(szFile, "AGK-Help.png") == 0 ) return LoadPNGFromMemory( libImageAGKHelpPNG, 0, 0 );
	if ( strcmp(szFile, "/AGK-Help-Icon.png") == 0 || strcmp(szFile, "AGK-Help-Icon.png") == 0 ) return LoadPNGFromMemory( libImageAGKHelpIconPNG, 0, 0 );
#endif

	m_szFile.SetStr( szFile );
    m_szFile.Replace( '\\', '/' );
    
	m_pParentImage = 0;

	uString ext;
	int pos = m_szFile.RevFind( '.' );
	if ( pos >= 0 )
	{
		m_szFile.SubString( ext, pos+1 );
	}
	ext.Lower();

	uint32_t out_width, out_height;
	unsigned char* pData = 0;

	if ( ext.CompareTo( "gif" ) == 0 ) 
	{
		if ( !GetGifFromFile( szFile, &pData, &out_width, &out_height ) ) 
		{
			if ( pData ) delete [] pData;
			pData = 0;

			if ( m_iNoMissingImage )
			{
				m_iNoMissingImage = 0;
				return false;
			}

			m_iSpecialLoadMode = 0;
			return LoadPNGFromMemory( libImageMissingPNG, 0, 0 );
		}
	}
	else if ( ext.CompareTo("bmp") == 0 )
	{
		cFile sFile; 
		int err = 0;
		if ( sFile.OpenToRead( szFile ) )
		{
			err = GetBMPFromFile( &sFile, &pData, &out_width, &out_height );
			sFile.Close();
		}

		if ( err )
		{
			if ( pData ) delete[] pData;
			pData = 0;

#ifdef _AGK_ERROR_CHECK
			uString errStr;
			errStr.Format("Failed to load bmp %u", err);
			agk::Error(errStr);
#endif

			if (m_iNoMissingImage)
			{
				m_iNoMissingImage = 0;
				return false;
			}

			m_iSpecialLoadMode = 0;
			return LoadPNGFromMemory(libImageMissingPNG, 0, 0);
		}
	}
	else
	{
		// platform specific image load
		if ( !PlatformGetDataFromFile( szFile, &pData, &out_width, &out_height ) ) 
		{
			if ( pData ) delete [] pData;
			pData = 0;

			if ( m_iNoMissingImage )
			{
				m_iNoMissingImage = 0;
				return false;
			}

			m_iSpecialLoadMode = 0;
			return LoadPNGFromMemory( libImageMissingPNG, 0, 0 );
		}
	}

	if ( bBlackToAlpha )
	{
		for ( unsigned int y = 0; y < out_height; y++ )
		{
			unsigned int index = y*out_width;

			for ( unsigned int x = 0; x < out_width; x++ )
			{
				if ( pData[ index*4 ] == 0 
				  && pData[ index*4 + 1 ] == 0 
				  && pData[ index*4 + 2 ] == 0 ) 
				{
					pData[ index*4 + 3 ] = 0;
				}

				index++;
			}
		}
	}

	m_iSpecialLoadMode = 0;
	m_iNoMissingImage = 0;

	LoadFromData( out_width, out_height, (unsigned int*)pData );
	delete [] pData;

	// delete any existing sub images
	while ( m_pSubImages ) 
	{
		cSubImage *pSubImage = m_pSubImages;
		m_pSubImages = m_pSubImages->pNextSubImage;
		delete pSubImage;
	}

	// check for a texture atlas subimages file
	uString szPosFile( m_szFile, 100 );
	szPosFile.Trunc( '.' );
	szPosFile.Append( " subimages.txt" );

	if ( cFile::Exists( szPosFile ) )
	{
		cFile oFile;
		oFile.OpenToRead( szPosFile );

		//char szLine[ 1024 ];
		uString szLine;
		char szFilename[ 1024 ];
		char szTemp[ 30 ];
		float subX;
		float subY;
		float subWidth;
		float subHeight;
		uString sTemp;
		while ( !oFile.IsEOF() )
		{
			// extract values, one sub image per line
			//if ( !fgets( szLine, 1024, pFile ) ) continue;
			oFile.ReadLine( szLine );

			// filename
			const char *szRemaining = szLine.GetStr();
			const char *szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			uint32_t dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szFilename, szRemaining, dwLength );
			szFilename[ dwLength ] = '\0';

			// x
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subX = sTemp.ToFloat();

			// y
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subY = sTemp.ToFloat();

			// width
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subWidth = sTemp.ToFloat();

			// height
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( szFound ) continue; // don't want to find another ':' this time
			strcpy( szTemp, szRemaining );
			sTemp.SetStr( szTemp );
			sTemp.Trunc( '\r' );
			subHeight = sTemp.ToFloat();

			subX = subX * m_fScaledAmount;
			subY = subY * m_fScaledAmount;
			subWidth = subWidth * m_fScaledAmount;
			subHeight = subHeight * m_fScaledAmount;

			if ( subX < 0 ) subX = 0;
			if ( subY < 0 ) subY = 0;
			if ( subX > GetTotalWidth() ) subX = (float)GetTotalWidth();
			if ( subY > GetTotalHeight() ) subY = (float)GetTotalHeight();

			if ( subX + subWidth > GetTotalWidth() ) subWidth = GetTotalWidth() - subX;
			if ( subY + subHeight > GetTotalHeight() ) subHeight = GetTotalHeight() - subY;

			// add new sub image details
			cSubImage *pSubImage = new cSubImage();
			pSubImage->x = subX;
			pSubImage->y = subY;
			pSubImage->width = subWidth;
			pSubImage->height = subHeight;
			pSubImage->szFilename.SetStr( szFilename );
			pSubImage->pNextSubImage = m_pSubImages;
			m_pSubImages = pSubImage;
		}

		oFile.Close();
	}
	else
	{
		szPosFile.SetStr( m_szFile );
		szPosFile.Trunc( '.' );
		szPosFile.Append( ".atlas" );

		if ( cFile::Exists( szPosFile ) )
		{
			cFile oFile;
			oFile.OpenToRead( szPosFile );

			// ignore header information
			uString szLine;
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );

			while ( !oFile.IsEOF() && strncmp( szLine, "repeat: ", strlen("repeat: ") ) != 0 )
			{
				oFile.ReadLine( szLine );
			}

			if ( oFile.IsEOF() )
			{
				uString info;
				info.Format( "Unrecognised .atlas file format: %s", szPosFile.GetStr() );
				agk::Message( info.GetStr() );
				agk::Error( info.GetStr() );
				oFile.Close();
				return true;
			}

			char szFilename[ 1024 ];
			char szTemp[ 30 ];
			float subX;
			float subY;
			float subWidth;
			float subHeight;
			uString sTemp;
			while ( !oFile.IsEOF() )
			{
				// filename
				oFile.ReadLine( szLine );
				if ( szLine.CharAt(0) == '\n' || szLine.CharAt(0) == '\r' )
				{
					uString info;
					info.Format( "AGK does not support multiple textures inside a single .atlas file: %s", szPosFile.GetStr() );
					agk::Message( info.GetStr() );
					agk::Error( info.GetStr() );
					oFile.Close();
					return true;
				}
				szLine.Trim( " \t\n\r" );
				strcpy( szFilename, szLine );

				// rotate
				oFile.ReadLine( szLine );
				
				// XY
				oFile.ReadLine( szLine );

				// x
				const char *szRemaining = szLine.GetStr();
				const char *szFound = strchr( szRemaining, ':' );
				if ( !szFound ) continue;
				szRemaining = szFound + 1;
				szFound = strchr( szRemaining, ',' );
				if ( !szFound ) continue;
				uint32_t dwLength = (uint32_t) (szFound - szRemaining);
				strncpy( szTemp, szRemaining, dwLength );
				szTemp[ dwLength ] = '\0';
				sTemp.SetStr( szTemp );
				subX = sTemp.ToFloat();

				// y
				szRemaining = szFound + 1;
				strcpy( szTemp, szRemaining );
				sTemp.SetStr( szTemp );
				sTemp.Trim( " \n\r\t" );
				subY = sTemp.ToFloat();

				// Size
				oFile.ReadLine( szLine );

				// width
				szRemaining = szLine.GetStr();
				szFound = strchr( szRemaining, ':' );
				if ( !szFound ) continue;
				szRemaining = szFound + 1;
				szFound = strchr( szRemaining, ',' );
				if ( !szFound ) continue;
				dwLength = (uint32_t) (szFound - szRemaining);
				strncpy( szTemp, szRemaining, dwLength );
				szTemp[ dwLength ] = '\0';
				sTemp.SetStr( szTemp );
				subWidth = sTemp.ToFloat();

				// height
				szRemaining = szFound + 1;
				strcpy( szTemp, szRemaining );
				sTemp.SetStr( szTemp );
				sTemp.Trim( " \n\r\t" );
				subHeight = sTemp.ToFloat();

				// orig
				oFile.ReadLine( szLine );
				// offset
				oFile.ReadLine( szLine );
				// index
				oFile.ReadLine( szLine );

				subX = subX * m_fScaledAmount;
				subY = subY * m_fScaledAmount;
				subWidth = subWidth * m_fScaledAmount;
				subHeight = subHeight * m_fScaledAmount;

				if ( subX < 0 ) subX = 0;
				if ( subY < 0 ) subY = 0;
				if ( subX > GetTotalWidth() ) subX = (float)GetTotalWidth();
				if ( subY > GetTotalHeight() ) subY = (float)GetTotalHeight();

				if ( subX + subWidth > GetTotalWidth() ) subWidth = GetTotalWidth() - subX;
				if ( subY + subHeight > GetTotalHeight() ) subHeight = GetTotalHeight() - subY;

				// add new sub image details
				cSubImage *pSubImage = new cSubImage();
				pSubImage->x = subX;
				pSubImage->y = subY;
				pSubImage->width = subWidth;
				pSubImage->height = subHeight;
				pSubImage->szFilename.SetStr( szFilename );
				pSubImage->pNextSubImage = m_pSubImages;
				m_pSubImages = pSubImage;
			}

			oFile.Close();
		}
		else
		{
			unsigned int *subImages = 0;
			if ( m_szFile.CompareTo( "Arial.png" ) == 0 || m_szFile.CompareTo( "/Arial.png" ) == 0 ) subImages = libArialSubImages;
#ifndef AGK_HTML5
			else if ( m_szFile.CompareTo( "Avenir.png" ) == 0 || m_szFile.CompareTo( "/Avenir.png" ) == 0 ) subImages = libAvenirSubImages;
#endif

			if ( subImages != 0 )
			{
				uString sName( "", 5 );
				float subX;
				float subY;
				float subWidth;
				float subHeight;
				uint32_t index = 0;

				// load sub images from lib store
				for ( int i = 0; i < 95; i++ )
				{
					sName.ClearTemp();
					sName.AppendUInt(  subImages[ index++ ] );
					subX = (float)subImages[ index++ ];
					subY = (float)subImages[ index++ ];
					subWidth = (float)subImages[ index++ ];
					subHeight = (float)subImages[ index++ ];

					subX = subX * m_fScaledAmount;
					subY = subY * m_fScaledAmount;
					subWidth = subWidth * m_fScaledAmount;
					subHeight = subHeight * m_fScaledAmount;
					
					if ( subX < 0 ) subX = 0;
					if ( subY < 0 ) subY = 0;
					if ( subX > GetTotalWidth() ) subX = (float)GetTotalWidth();
					if ( subY > GetTotalHeight() ) subY = (float)GetTotalHeight();

					if ( subX + subWidth > GetTotalWidth() ) subWidth = GetTotalWidth() - subX;
					if ( subY + subHeight > GetTotalHeight() ) subHeight = GetTotalHeight() - subY;

					// add new sub image details
					cSubImage *pSubImage = new cSubImage();
					pSubImage->x = subX;
					pSubImage->y = subY;
					pSubImage->width = subWidth;
					pSubImage->height = subHeight;
					pSubImage->szFilename.SetStr( sName );
					pSubImage->pNextSubImage = m_pSubImages;
					m_pSubImages = pSubImage;
				}
			}
		}
	}

	return true;
}

bool cImage::LoadPNGFromMemory( unsigned int* pInData, unsigned int *pSubImageData, unsigned int iNumSubImages, bool bBlackToAlpha )
//****
{
	m_pParentImage = 0;
	m_iSpecialLoadMode = 0;

	int out_width, out_height;
	bool outHasAlpha;
	unsigned char* pData = 0;

	if ( !loadPngImageFromMemory( (unsigned char*)pInData, out_width, out_height, outHasAlpha, &pData ) )
	{
		if ( pData ) delete [] pData;
		pData = 0;

		if ( m_iNoMissingImage )
		{
			m_iNoMissingImage = 0;
			return false;
		}

		m_iSpecialLoadMode = 0;
		if ( pInData != libImageMissingPNG ) return LoadPNGFromMemory( libImageMissingPNG, 0, 0 );
		else return false;
	}
	else
	{
		if ( !outHasAlpha )
		{
			// add alpha channel
			unsigned char *tempData = (unsigned char*) new unsigned int[ out_width * out_height * 4 ];
			for ( int y = 0; y < out_height; y++ )
			{
				unsigned int index = y*out_width;

				for ( int x = 0; x < out_width; x++ )
				{
					tempData[ index*4 ] = pData[ index*3 ];
					tempData[ index*4 + 1 ] = pData[ index*3 + 1 ];
					tempData[ index*4 + 2 ] = pData[ index*3 + 2 ];
					tempData[ index*4 + 3 ] = 255;
					index++;
				}
			}

			delete [] pData;
			pData = tempData;
		}
	}

	if ( bBlackToAlpha )
	{
		for ( int y = 0; y < out_height; y++ )
		{
			int index = y*out_width;

			for ( int x = 0; x < out_width; x++ )
			{
				if ( pData[ index*4 ] == 0 
				  && pData[ index*4 + 1 ] == 0 
				  && pData[ index*4 + 2 ] == 0 ) 
				{
					pData[ index*4 + 3 ] = 0;
				}

				index++;
			}
		}
	}

	m_iNoMissingImage = 0;

	LoadFromData( out_width, out_height, (unsigned int*)pData );
	delete [] pData;

	// delete any existing sub images
	while ( m_pSubImages ) 
	{
		cSubImage *pSubImage = m_pSubImages;
		m_pSubImages = m_pSubImages->pNextSubImage;
		delete pSubImage;
	}

	if ( pSubImageData && iNumSubImages > 0 )
	{
		uString sName( "", 5 );
		float subX;
		float subY;
		float subWidth;
		float subHeight;
		uint32_t index = 0;

		// load sub images from lib store
		for ( unsigned int i = 0; i < iNumSubImages; i++ )
		{
			sName.ClearTemp();
			sName.AppendUInt(  pSubImageData[ index++ ] );
			subX = (float)pSubImageData[ index++ ];
			subY = (float)pSubImageData[ index++ ];
			subWidth = (float)pSubImageData[ index++ ];
			subHeight = (float)pSubImageData[ index++ ];

			subX = subX * m_fScaledAmount;
			subY = subY * m_fScaledAmount;
			subWidth = subWidth * m_fScaledAmount;
			subHeight = subHeight * m_fScaledAmount;
			
			if ( subX < 0 ) subX = 0;
			if ( subY < 0 ) subY = 0;
			if ( subX > GetTotalWidth() ) subX = (float)GetTotalWidth();
			if ( subY > GetTotalHeight() ) subY = (float)GetTotalHeight();

			if ( subX + subWidth > GetTotalWidth() ) subWidth = GetTotalWidth() - subX;
			if ( subY + subHeight > GetTotalHeight() ) subHeight = GetTotalHeight() - subY;

			// add new sub image details
			cSubImage *pSubImage = new cSubImage();
			pSubImage->x = subX;
			pSubImage->y = subY;
			pSubImage->width = subWidth;
			pSubImage->height = subHeight;
			pSubImage->szFilename.SetStr( sName );
			pSubImage->pNextSubImage = m_pSubImages;
			m_pSubImages = pSubImage;
		}
	}

	return true;
}

bool cImage::LoadSubImage( cImage *pParent, const uString &sSubFile, bool bSilent )
{
	return LoadSubImage( pParent, sSubFile.GetStr(), bSilent );
}

//****if* cImage/LoadSubImage
// FUNCTION
//   Loads a sub image from an atlas texture for use as a standalone image
// INPUTS
//   iParent -- The image ID that holds the atlas texture, loaded previously
//   szSubFile -- The filename of the sub image, as it was before being compressed to the atlas texture.
// SOURCE
bool cImage::LoadSubImage( cImage *pParent, const char *szSubFile, bool bSilent )
//****
{
	if ( !pParent ) return false;
	if ( pParent->HasParent() )
	{
#ifdef _AGK_ERROR_CHECK
		if ( !bSilent )
		{
			uString errStr("Error loading sub image - Cannot load sub images from other sub images ");
			agk::Error( errStr );
		}
#endif
	}
	
	cSubImage *pSubImage = pParent->m_pSubImages;
	while ( pSubImage )
	{
		if ( pSubImage->szFilename.CompareTo( szSubFile ) == 0 ) break;
		pSubImage = pSubImage->pNextSubImage;
	}

	if ( pSubImage == 0 )
	{
#ifdef _AGK_ERROR_CHECK
		if ( !bSilent )
		{
			uString errStr("Error loading sub image - Parent image does not contain sub image ");
			errStr.Append( szSubFile );
			agk::Error( errStr );
		}
#endif
		return false;
	}

	m_fU1 = pSubImage->x / (float) pParent->GetTotalWidth() + pParent->GetU1();
	m_fV1 = pSubImage->y / (float) pParent->GetTotalHeight() + pParent->GetV1();
	m_fU2 = (pSubImage->x+pSubImage->width) / (float) pParent->GetTotalWidth() + pParent->GetU1();
	m_fV2 = (pSubImage->y+pSubImage->height) / (float) pParent->GetTotalHeight() + pParent->GetV1();

	m_shaderVarTexBounds.SetVector4( m_fU2 - m_fU1, m_fV2 - m_fV1, m_fU1, m_fV1 );

	m_iFlags |= AGK_IMAGE_HAS_MODIFIED_UVS;

	m_pParentImage = pParent;
	m_iWidth = (uint32_t)pSubImage->width;
	m_iHeight = (uint32_t)pSubImage->height;
	m_iOrigWidth = m_iWidth;
	m_iOrigHeight = m_iHeight;
	m_szFile.SetStr( szSubFile );

	if ( !m_bDontLink ) pParent->AddChild( this );

	return true;
}

void cImage::ManualSubImage( cImage *pParent, int x, int y, int width, int height )
{
	m_fU1 = x / (float) pParent->GetTotalWidth() + pParent->GetU1();
	m_fV1 = y / (float) pParent->GetTotalHeight() + pParent->GetV1();
	m_fU2 = (x+width) / (float) pParent->GetTotalWidth() + pParent->GetU1();
	m_fV2 = (y+height) / (float) pParent->GetTotalHeight() + pParent->GetV1();

	m_shaderVarTexBounds.SetVector4( m_fU2 - m_fU1, m_fV2 - m_fV1, m_fU1, m_fV1 );

	m_iFlags |= AGK_IMAGE_HAS_MODIFIED_UVS;

	m_pParentImage = pParent;
	m_iWidth = (uint32_t)width;
	m_iHeight = (uint32_t)height;
	m_iOrigWidth = m_iWidth;
	m_iOrigHeight = m_iHeight;
}

void cImage::OverrideSubImage( cImage *pParent, int width, int height, float u1, float v1, float u2, float v2 )
{
	m_fU1 = u1;
	m_fV1 = v1;
	m_fU2 = u2;
	m_fV2 = v2;

	m_shaderVarTexBounds.SetVector4( m_fU2 - m_fU1, m_fV2 - m_fV1, m_fU1, m_fV1 );

	m_iFlags |= AGK_IMAGE_HAS_MODIFIED_UVS;

	m_pParentImage = pParent;
	m_iWidth = (uint32_t) width;
	m_iHeight = (uint32_t) height;
	m_iOrigWidth = m_iWidth;
	m_iOrigHeight = m_iHeight;
}

void cImage::SetSubImages( const char* szSubImageFile )
{
	// delete any existing sub images
	while ( m_pSubImages ) 
	{
		cSubImage *pSubImage = m_pSubImages;
		m_pSubImages = m_pSubImages->pNextSubImage;
		delete pSubImage;
	}

	cFile oFile;
	if ( !oFile.OpenToRead( szSubImageFile ) ) 
	{
		uString err; err.Format( "Failed to set sub images, could not open file \"%s\"", szSubImageFile );
		agk::Error( err );
		return;
	}
	
	uString szLine;
	char szFilename[ 1024 ];
	char szTemp[ 30 ];
	float subX;
	float subY;
	float subWidth;
	float subHeight;
	uString sTemp;
	while ( !oFile.IsEOF() )
	{
		// extract values, one sub image per line
		//if ( !fgets( szLine, 1024, pFile ) ) continue;
		oFile.ReadLine( szLine );

		// filename
		const char *szRemaining = szLine.GetStr();
		const char *szFound = strchr( szRemaining, ':' );
		if ( !szFound ) continue;
		uint32_t dwLength = (uint32_t) (szFound - szRemaining);
		strncpy( szFilename, szRemaining, dwLength );
		szFilename[ dwLength ] = '\0';

		// x
		szRemaining = szFound + 1;
		szFound = strchr( szRemaining, ':' );
		if ( !szFound ) continue;
		dwLength = (uint32_t) (szFound - szRemaining);
		strncpy( szTemp, szRemaining, dwLength );
		szTemp[ dwLength ] = '\0';
		sTemp.SetStr( szTemp );
		subX = sTemp.ToFloat();

		// y
		szRemaining = szFound + 1;
		szFound = strchr( szRemaining, ':' );
		if ( !szFound ) continue;
		dwLength = (uint32_t) (szFound - szRemaining);
		strncpy( szTemp, szRemaining, dwLength );
		szTemp[ dwLength ] = '\0';
		sTemp.SetStr( szTemp );
		subY = sTemp.ToFloat();

		// width
		szRemaining = szFound + 1;
		szFound = strchr( szRemaining, ':' );
		if ( !szFound ) continue;
		dwLength = (uint32_t) (szFound - szRemaining);
		strncpy( szTemp, szRemaining, dwLength );
		szTemp[ dwLength ] = '\0';
		sTemp.SetStr( szTemp );
		subWidth = sTemp.ToFloat();

		// height
		szRemaining = szFound + 1;
		szFound = strchr( szRemaining, ':' );
		if ( szFound ) continue; // don't want to find another ':' this time
		strcpy( szTemp, szRemaining );
		sTemp.SetStr( szTemp );
		sTemp.Trunc( '\r' );
		subHeight = sTemp.ToFloat();

		subX = subX * m_fScaledAmount;
		subY = subY * m_fScaledAmount;
		subWidth = subWidth * m_fScaledAmount;
		subHeight = subHeight * m_fScaledAmount;

		if ( subX < 0 ) subX = 0;
		if ( subY < 0 ) subY = 0;
		if ( subX > GetTotalWidth() ) subX = (float)GetTotalWidth();
		if ( subY > GetTotalHeight() ) subY = (float)GetTotalHeight();

		if ( subX + subWidth > GetTotalWidth() ) subWidth = GetTotalWidth() - subX;
		if ( subY + subHeight > GetTotalHeight() ) subHeight = GetTotalHeight() - subY;

		// add new sub image details
		cSubImage *pSubImage = new cSubImage();
		pSubImage->x = subX;
		pSubImage->y = subY;
		pSubImage->width = subWidth;
		pSubImage->height = subHeight;
		pSubImage->szFilename.SetStr( szFilename );
		pSubImage->pNextSubImage = m_pSubImages;
		m_pSubImages = pSubImage;
	}

	oFile.Close();
}

void cImage::CommonResize( unsigned char * pSrc, int width1, int height1, unsigned char * pDest, int width2, int height2 )
{
	unsigned char *pTemp = (unsigned char*) new unsigned int[ width2*height1 ];
	unsigned int *dtemp = (unsigned int*) pTemp;

	int pitch = width1*4;

	bool bUpsampleX = (width1 < width2);
	bool bUpsampleY = (height1 < height2);

	float fh = 1024*height1 / (float)height2;
	float fw = 1024*width1 / (float)width2;

	int* g_px1ab = new int[ width2*2 ];
	for( int x2 = 0; x2 < width2; x2++ )
	{
		// find the x-range of input pixels that will contribute:
		int x1a = (int)(x2*fw);
		int x1b = (int)((x2 + 1)*fw);
		if ( bUpsampleX ) x1b = x1a+1024; // map to same pixel -> we want to interpolate between two pixels
		if ( (1024*width1 - 1) < x1b ) x1b = 1024*width1 - 1;
		g_px1ab[ x2*2 + 0 ] = x1a;
		g_px1ab[ x2*2 + 1 ] = x1b;
	}

	// horizontal pass
	for( int y = 0; y < height1; y++ )
	{
		for( int x2 = 0; x2 < width2; x2++ )
		{
			// find the x-range of input pixels that will contribute:
			int x1a = g_px1ab[x2*2];	// (computed earlier)
			int x1b = g_px1ab[x2*2+1];	// (computed earlier)
			int x1c = x1a >> 10;
			int x1d = x1b >> 10;

			// ADD UP ALL INPUT PIXELS CONTRIBUTING TO THIS OUTPUT PIXEL:
			unsigned int r=0, g=0, b=0, a=0, tot=0, tota=0;
			unsigned char *dsrc2 = pSrc + y*pitch + (x1c << 2);
			for( int x = x1c; x <= x1d; x++ )
			{
				unsigned int weight_x = 1024;
				if ( x1c != x1d )
				{
					if (x == x1c) weight_x = 1024 - (x1a & 0x3FF);
					else if (x == x1d) weight_x = (x1b & 0x3FF);
				}
				unsigned int w = weight_x;

				if ( *(dsrc2+3) == 0 )
				{
					dsrc2 += 3;
					a += (*dsrc2++) * w;
					tota += w;
				}
				else
				{
					r += (*dsrc2++) * w;
					g += (*dsrc2++) * w;
					b += (*dsrc2++) * w;
					a += (*dsrc2++) * w;
					tot += w;
					tota += w;
				}
			}

			if ( tot > 0 )
			{
				r /= tot;
				g /= tot;
				b /= tot;
			}

			if ( tota > 0 ) a /= tota;

			// write results
			unsigned int c = r | (g << 8) | (b << 16) | (a << 24);
			*dtemp++ = c;
		}
	}

	delete [] g_px1ab;

	unsigned int *ddest = (unsigned int*) pDest;

	pitch = width2*4;

	int* g_py1ab = new int[ height2*2 ];
	for( int y2 = 0; y2 < height2; y2++ )
	{
		// find the y-range of input pixels that will contribute:
		int y1a = (int)(y2*fh);
		int y1b = (int)((y2 + 1)*fh);
		if ( bUpsampleY ) y1b = y1a + 1024; // map to same pixel -> we want to interpolate between two pixels
		if ( (1024*height1 - 1) < y1b ) y1b = 1024*height1 - 1;
		g_py1ab[ y2*2 + 0 ] = y1a;
		g_py1ab[ y2*2 + 1 ] = y1b;
	}

	// vetical pass
	for( int y2 = 0; y2 < height2; y2++ )
	{
		// find the y-range of input pixels that will contribute:
		int y1a = g_py1ab[ y2*2 ];
		int y1b = g_py1ab[ y2*2 + 1 ];
		int y1c = y1a >> 10;
		int y1d = y1b >> 10;

		for( int x2 = 0; x2 < width2; x2++ )
		{
			// ADD UP ALL INPUT PIXELS CONTRIBUTING TO THIS OUTPUT PIXEL:
			unsigned int r=0, g=0, b=0, a=0, tot=0, tota=0;
			for( int y = y1c; y <= y1d; y++ )
			{
				unsigned int weight_y = 1024;
				if (y1c != y1d)
				{
					if (y == y1c) weight_y = 1024 - (y1a & 0x3FF);
					else if (y == y1d) weight_y = (y1b & 0x3FF);
				}

				unsigned char *dsrc2 = pTemp + y*pitch + (x2 << 2);
				unsigned int w = weight_y;
				if ( *(dsrc2+3) == 0 )
				{
					dsrc2 += 3;
					a += (*dsrc2++) * w;
					tota += w;
				}
				else
				{
					r += (*dsrc2++) * w;
					g += (*dsrc2++) * w;
					b += (*dsrc2++) * w;
					a += (*dsrc2++) * w;
					tot += w;
					tota += w;
				}
			}

			if ( tot > 0 )
			{
				r /= tot;
				g /= tot;
				b /= tot;
			}

			if ( tota > 0 ) a /= tota;

			// write results
			unsigned int c = r | (g << 8) | (b << 16) | (a << 24);
			*ddest++ = c;
		}
	}

	delete [] g_py1ab;
	delete [] pTemp;
}

void cImage::CacheNewSize( const char* szFile, float scaleX, float scaleY, int maxTexSize )
{
	if ( scaleX <= 0 || scaleY <= 0 )
	{
		agk::Error("Failed to pre-cache image, scale values must be greater than 0");
		return;
	}

	// look for a cached version
	uString sFilename( szFile );
	sFilename.Trunc('.');
	uString sNewFilename;
	sNewFilename.Format( "%s_%.6f_%.6f.png", sFilename.GetStr(), scaleX, scaleY );
	if ( cFile::ExistsWrite( sNewFilename.GetStr() ) )
	{
		return;
	}

	// platform specific image load
	unsigned char *pData = 0;
	unsigned int width, height;
	if ( !PlatformGetDataFromFile( szFile, &pData, &width, &height ) ) 
	{
		if ( pData ) delete [] pData;
		return;
	}

	// scale the data
	int newWidth = (int) (width * scaleX);
	int newHeight = (int) (height * scaleY);

	if ( newWidth > maxTexSize || newHeight > maxTexSize )
	{
		if ( newWidth > newHeight )
		{
			float aspect = newHeight / (float)newWidth;
			newWidth = maxTexSize;
			newHeight = agk::Round(newWidth * aspect);
		}
		else
		{
			float aspect = newWidth / (float)newHeight;
			newHeight = maxTexSize;
			newWidth = agk::Round(newHeight * aspect);
		}
		
		uString err;
		err.Format( "Pre-cache final image size is larger than the device can support, it will be reduced to %dx%d", newWidth, newHeight );
		agk::Warning(err);
	}

	float realScaleX = newWidth / (float)width;
	float realScaleY = newHeight / (float)height;
	unsigned int *pFinalData = new unsigned int[ newWidth*newHeight ];

	CommonResize( pData, width, height, (unsigned char*)pFinalData, newWidth, newHeight );
	delete [] pData;

	// save for future loads
	PlatformSaveDataToFile( sNewFilename, (unsigned char*)pFinalData, newWidth, newHeight );
	delete [] pFinalData;

	cSubImage *pTempSubImages = 0;

	// check for a texture atlas subimages file
	uString szPosFile( szFile, 100 );
	szPosFile.Trunc( '.' );
	szPosFile.Append( " subimages.txt" );

	if ( cFile::Exists( szPosFile ) )
	{
		cFile oFile;
		oFile.OpenToRead( szPosFile );

		//char szLine[ 1024 ];
		uString szLine;
		char szFilename[ 1024 ];
		char szTemp[ 30 ];
		float subX;
		float subY;
		float subWidth;
		float subHeight;
		uString sTemp;
		while ( !oFile.IsEOF() )
		{
			// extract values, one sub image per line
			//if ( !fgets( szLine, 1024, pFile ) ) continue;
			oFile.ReadLine( szLine );

			// filename
			const char *szRemaining = szLine.GetStr();
			const char *szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			uint32_t dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szFilename, szRemaining, dwLength );
			szFilename[ dwLength ] = '\0';

			// x
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subX = sTemp.ToInt() * realScaleX;

			// y
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subY = sTemp.ToInt() * realScaleY;

			// width
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subWidth = sTemp.ToInt() * realScaleX;

			// height
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( szFound ) continue; // don't want to find another ':' this time
			strcpy( szTemp, szRemaining );
			sTemp.SetStr( szTemp );
			sTemp.Trunc( '\r' );
			subHeight = sTemp.ToInt() * realScaleY;

			if ( subX < 0 ) subX = 0;
			if ( subY < 0 ) subY = 0;
			if ( subX > newWidth ) subX = (float)newWidth;
			if ( subY > newHeight ) subY = (float)newHeight;

			if ( subX + subWidth > newWidth ) subWidth = newWidth - subX;
			if ( subY + subHeight > newHeight ) subHeight = newHeight - subY;

			// add new sub image details
			cSubImage *pSubImage = new cSubImage();
			pSubImage->x = subX;
			pSubImage->y = subY;
			pSubImage->width = subWidth;
			pSubImage->height = subHeight;
			pSubImage->szFilename.SetStr( szFilename );
			pSubImage->pNextSubImage = pTempSubImages;
			pTempSubImages = pSubImage;
		}

		oFile.Close();
	}
	else
	{
		szPosFile.SetStr( szFile );
		szPosFile.Trunc( '.' );
		szPosFile.Append( ".atlas" );

		if ( cFile::Exists( szPosFile ) )
		{
			cFile oFile;
			oFile.OpenToRead( szPosFile );

			// ignore header information
			uString szLine;
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );

			while ( !oFile.IsEOF() && strncmp( szLine, "repeat: ", strlen("repeat: ") ) != 0 )
			{
				oFile.ReadLine( szLine );
			}

			if ( oFile.IsEOF() )
			{
				uString info;
				info.Format( "Unrecognised .atlas file format: %s", szPosFile.GetStr() );
				agk::Message( info.GetStr() );
				agk::Error( info.GetStr() );
				oFile.Close();
				return;
			}

			char szFilename[ 1024 ];
			char szTemp[ 30 ];
			float subX;
			float subY;
			float subWidth;
			float subHeight;
			uString sTemp;
			while ( !oFile.IsEOF() )
			{
				// filename
				oFile.ReadLine( szLine );
				if ( szLine.CharAt(0) == '\n' || szLine.CharAt(0) == '\r' )
				{
					uString info;
					info.Format( "AGK does not support multiple textures inside a single .atlas file: %s", szPosFile.GetStr() );
					agk::Message( info.GetStr() );
					agk::Error( info.GetStr() );
					oFile.Close();
					return;
				}
				szLine.Trim( " \t\n\r" );
				strcpy( szFilename, szLine );

				// rotate
				oFile.ReadLine( szLine );
				
				// XY
				oFile.ReadLine( szLine );

				// x
				const char *szRemaining = szLine.GetStr();
				const char *szFound = strchr( szRemaining, ':' );
				if ( !szFound ) continue;
				szRemaining = szFound + 1;
				szFound = strchr( szRemaining, ',' );
				if ( !szFound ) continue;
				uint32_t dwLength = (uint32_t) (szFound - szRemaining);
				strncpy( szTemp, szRemaining, dwLength );
				szTemp[ dwLength ] = '\0';
				sTemp.SetStr( szTemp );
				subX = sTemp.ToInt() * realScaleX;

				// y
				szRemaining = szFound + 1;
				strcpy( szTemp, szRemaining );
				sTemp.SetStr( szTemp );
				sTemp.Trim( " \n\r\t" );
				subY = sTemp.ToInt() * realScaleY;

				// Size
				oFile.ReadLine( szLine );

				// width
				szRemaining = szLine.GetStr();
				szFound = strchr( szRemaining, ':' );
				if ( !szFound ) continue;
				szRemaining = szFound + 1;
				szFound = strchr( szRemaining, ',' );
				if ( !szFound ) continue;
				dwLength = (uint32_t) (szFound - szRemaining);
				strncpy( szTemp, szRemaining, dwLength );
				szTemp[ dwLength ] = '\0';
				sTemp.SetStr( szTemp );
				subWidth = sTemp.ToInt() * realScaleX;

				// height
				szRemaining = szFound + 1;
				strcpy( szTemp, szRemaining );
				sTemp.SetStr( szTemp );
				sTemp.Trim( " \n\r\t" );
				subHeight = sTemp.ToInt() * realScaleY;

				// orig
				oFile.ReadLine( szLine );
				// offset
				oFile.ReadLine( szLine );
				// index
				oFile.ReadLine( szLine );

				if ( subX < 0 ) subX = 0;
				if ( subY < 0 ) subY = 0;
				if ( subX > newWidth ) subX = (float)newWidth;
				if ( subY > newHeight ) subY = (float)newHeight;

				if ( subX + subWidth > newWidth ) subWidth = newWidth - subX;
				if ( subY + subHeight > newHeight ) subHeight = newHeight - subY;

				// add new sub image details
				cSubImage *pSubImage = new cSubImage();
				pSubImage->x = subX;
				pSubImage->y = subY;
				pSubImage->width = subWidth;
				pSubImage->height = subHeight;
				pSubImage->szFilename.SetStr( szFilename );
				pSubImage->pNextSubImage = pTempSubImages;
				pTempSubImages = pSubImage;
			}

			oFile.Close();
		}
	}

	// save new sub images to make loading faster next time
	if ( pTempSubImages )
	{
		uString szSubImagesFile( sNewFilename );
		szSubImagesFile.Trunc( '.' );
		szSubImagesFile.Append( " subimages.txt" );

		cFile oFile;
		oFile.OpenToWrite( szSubImagesFile );

		uString sLine;
		cSubImage *pSubImage = pTempSubImages;
		while ( pSubImage ) 
		{
			sLine.Format( "%s:%f:%f:%f:%f", pSubImage->szFilename.GetStr(), pSubImage->x, pSubImage->y, pSubImage->width, pSubImage->height );
			oFile.WriteLine( sLine );

			pSubImage = pSubImage->pNextSubImage;
		}

		oFile.Close();
	}

	// delete temporary sub images
	while ( pTempSubImages ) 
	{
		cSubImage *pSubImage = pTempSubImages;
		pTempSubImages = pTempSubImages->pNextSubImage;
		delete pSubImage;
	}
}

void cImage::PreCache( const char* szFile, float scaleX, float scaleY )
{
	// don't cache on HTML5 as there is no persistent storage
#ifdef AGK_HTML5
	return;
#endif

	if ( scaleX <= 0 || scaleY <= 0 )
	{
		agk::Error("Failed to pre-cache image, scale values must be greater than 0");
		return;
	}

	// look for a cached version
	uString sFilename( szFile );
	sFilename.Trunc('.');
	uString sNewFilename;
	sNewFilename.Format( "%s_%.6f_%.6f.png", sFilename.GetStr(), scaleX, scaleY );
	if ( cFile::ExistsWrite( sNewFilename.GetStr() ) )
	{
		return;
	}

	if ( !g_pCachingThread )
	{
		g_pCachingThread = new ImageCacher();
		g_pCachingThread->Start();
	}

	if ( !g_pCachingThread->IsRunning() ) 
	{
		return;
	}
    
    // convert to absolute path
    uString sFullPath( szFile );
    if ( szFile[0] != '/' && szFile[0] != '\\' )
    {
        char *pCurrDir = agk::GetCurrentDir();
        sFullPath.Prepend( pCurrDir );
        sFullPath.Prepend( "/" );
        delete [] pCurrDir;
    }

	// tell thread to compress it
	ImageCacheItem *pNewItem = new ImageCacheItem();
	pNewItem->m_sLoadFilename.SetStr( sFullPath );
	pNewItem->m_fScaleX = scaleX;
	pNewItem->m_fScaleY = scaleY;
	pNewItem->m_iMaxTexSize = agk::GetRenderer()->GetMaxTextureSize();
	pNewItem->m_pData = 0;

	ImageCacher::pLock.Acquire();
    pNewItem->m_pNextItem = 0;
    if ( ImageCacher::g_pLastImage ) ImageCacher::g_pLastImage->m_pNextItem = pNewItem;
    else ImageCacher::g_pImages = pNewItem;
    ImageCacher::g_pLastImage = pNewItem;
	ImageCacher::pLock.Release();

	ImageCacher::pCondition.Lock();
	ImageCacher::pCondition.Signal();
	ImageCacher::pCondition.Unlock();
}

int cImage::GetCachingFinished()
{
	ImageCacher::pLock.Acquire();
	int result = (ImageCacher::g_pImages == 0 && ImageCacher::g_iCaching == 0) ? 1 : 0;
	ImageCacher::pLock.Release();
	return result;
}

bool cImage::LoadResized( const char* szFile, float scaleX, float scaleY, int cache )
{
	if ( scaleX <=0 || scaleY <= 0 )
	{
		agk::Error("Failed to load resized image, scale values must be greater than 0");
		return false;
	}

	// if scale is very close to 1 then no need to scale
	if ( abs(scaleX-1) < 0.0005f && abs(scaleY-1) < 0.0005f ) 
	{
		return Load( szFile );
	}

	m_iSpecialLoadMode = 0;

	int iCPUScale = 0;
	if ( agk::IsSGX540GPU() ) iCPUScale = 1; // SGX 540 has an internal limit on rendered images, so we can't use the GPU for scaling lots of images

	//iCPUScale = 1;

	// look for a cached version
	uString sFilename( szFile );
	sFilename.Trunc('.');
	uString sNewFilename;
	sNewFilename.Format( "%s_%.6f_%.6f.png", sFilename.GetStr(), scaleX, scaleY );

	if ( iCPUScale )
	{
		// only do caching when using CPU scaling
		if ( cFile::ExistsWrite( sNewFilename.GetStr() ) )
		{
			// load cached version
			m_iNoMissingImage = 1;
			if ( Load( sNewFilename ) )
			{
				return true;
			}
		}
	}

	m_iNoMissingImage = 0;

	// not cached
	m_szFile.SetStr( szFile );
    m_szFile.Replace( '\\', '/' );

	m_pParentImage = 0;

	uString ext;
	int pos = m_szFile.RevFind( '.' );
	if ( pos >= 0 )
	{
		m_szFile.SubString( ext, pos+1 );
	}
	ext.Lower();

	float realScaleX, realScaleY;
	if ( iCPUScale )
	{
		// platform specific image load
		unsigned char *pData = 0;
		unsigned int width, height;
		if ( !PlatformGetDataFromFile( szFile, &pData, &width, &height ) ) 
		{
			m_iSpecialLoadMode = 0;
			if ( pData ) delete [] pData;
			pData = 0;
			cache = 0;
			return LoadPNGFromMemory( libImageMissingPNG, 0, 0 );
		}

		// scale the data
		int newWidth = (int) (width * scaleX);
		int newHeight = (int) (height * scaleY);

		if ( newWidth < 1 ) newWidth = 1;
		if ( newHeight < 1 ) newHeight = 1;

		int texMaxSize = agk::GetRenderer()->GetMaxTextureSize();

		if ( newWidth > texMaxSize || newHeight > texMaxSize )
		{
			if ( newWidth > newHeight )
			{
				float aspect = newHeight / (float)newWidth;
				newWidth = texMaxSize;
				newHeight = agk::Round(newWidth * aspect);
			}
			else
			{
				float aspect = newWidth / (float)newHeight;
				newHeight = texMaxSize;
				newWidth = agk::Round(newHeight * aspect);
			}
			
			uString err;
			err.Format( "LoadImageResized final image size is larger than the device can support, it will be reduced to %dx%d", newWidth, newHeight );
			agk::Warning(err);
		}

		realScaleX = newWidth / (float)width;
		realScaleY = newHeight / (float)height;
		unsigned int *pFinalData = new unsigned int[ newWidth*newHeight ];

		CommonResize( pData, width, height, (unsigned char*)pFinalData, newWidth, newHeight );
		delete [] pData;

		// load texture from scaled data
		LoadFromData( newWidth, newHeight, pFinalData );

		// save to make loading faster next time
		//PlatformSaveDataToFile( sNewFilename, (unsigned char*)pFinalData, newWidth, newHeight );
		if ( cache ) SetCachePixelData( (unsigned char*)pFinalData, newWidth, newHeight, sNewFilename, (cache==2)?1:0 ); // SetCachePixelData will delete pFinalData
		else delete [] pFinalData;
	}
	else
	{
		// no need to cache GPU scaled images due to much faster scaling method
		cache = 0;
	    
		cImage *OrigSizeImage = new cImage();
		if ( !OrigSizeImage->Load( szFile ) )
		{
			delete OrigSizeImage;
			return false;
		}
	    
		int newWidth = (int) (OrigSizeImage->GetWidth() * scaleX);
		int newHeight = (int) (OrigSizeImage->GetHeight() * scaleY);

		if ( newWidth < 1 ) newWidth = 1;
		if ( newHeight < 1 ) newHeight = 1;
	    
		int texMaxSize = agk::GetRenderer()->GetMaxTextureSize();
	    
		if ( newWidth > texMaxSize || newHeight > texMaxSize )
		{
			if ( newWidth > newHeight )
			{
				float aspect = newHeight / (float)newWidth;
				newWidth = texMaxSize;
				newHeight = agk::Round(newWidth * aspect);
			}
			else
			{
				float aspect = newWidth / (float)newHeight;
				newHeight = texMaxSize;
				newWidth = agk::Round(newHeight * aspect);
			}
	        
			uString err;
			err.Format( "LoadImageResized final image size is larger than the device can support, it will be reduced to %dx%d", newWidth, newHeight );
			agk::Warning(err);
		}

		CreateBlankImage( newWidth, newHeight, 0, 0 );
		m_bHasAlpha = OrigSizeImage->m_bHasAlpha;
		m_fScaledAmount = OrigSizeImage->m_fScaledAmount;
		m_iFlags = OrigSizeImage->m_iFlags;
		if ( !agk::GetRenderer()->SupportsNPOT() || ((m_iFlags & AGK_IMAGE_USE_MIPMAP) && !agk::GetRenderer()->SupportsMipmapNPOT()) )
		{
			m_iFlags &= ~(AGK_IMAGE_WRAP_U_REPEAT | AGK_IMAGE_WRAP_V_REPEAT);
		}
			    
		realScaleX = newWidth / (float)OrigSizeImage->GetWidth();
		realScaleY = newHeight / (float)OrigSizeImage->GetHeight();

		// set up frame buffer
		AGKFrameBuffer *pOldFBO = AGKFrameBuffer::g_pCurrFrameBuffer;
		AGKFrameBuffer *pFBO = new AGKFrameBuffer( this, true, 0, true, false );
		AGKFrameBuffer::BindFrameBuffer( pFBO );
		agk::GetRenderer()->SetViewport( 0, 0, newWidth, newHeight );
		agk::GetRenderer()->SetGlobalScissor( 0, 0, 0, 0 );
		agk::GetRenderer()->SetViewport( 0, 0, newWidth, newHeight ); // if we don't do this again then Android seems to revert to the backbuffer viewport sizes

		static cObject3D *pQuad = 0;
		if ( pQuad == 0 )
		{
			pQuad = new cObject3D();
			pQuad->CreateQuad();
		}

		// draw image to new image
		pQuad->SetImage(OrigSizeImage);
		pQuad->Draw();
	    
		pQuad->SetImage(0);
		delete OrigSizeImage;
	    
		// delete frame buffer
		AGKFrameBuffer::BindFrameBuffer( pOldFBO );
		if ( !agk::m_bScissorUser )
		{
			agk::GetRenderer()->SetGlobalScissor( agk::m_iScissorX, agk::m_iScissorY, agk::m_iScissorWidth, agk::m_iScissorHeight );
		}
		else
		{
			agk::SetScissor( agk::m_fScissorUserX, agk::m_fScissorUserY, agk::m_fScissorUserX2, agk::m_fScissorUserY2 );
		}
	    
		agk::GetRenderer()->SetViewport( 0,0, agk::m_iRenderWidth, agk::m_iRenderHeight );
		delete pFBO;    
	}

	// delete any existing sub images
	while ( m_pSubImages ) 
	{
		cSubImage *pSubImage = m_pSubImages;
		m_pSubImages = m_pSubImages->pNextSubImage;
		delete pSubImage;
	}

	// check for a texture atlas subimages file
	uString szPosFile( m_szFile, 100 );
	szPosFile.Trunc( '.' );
	szPosFile.Append( " subimages.txt" );

	if ( cFile::Exists( szPosFile ) )
	{
		cFile oFile;
		oFile.OpenToRead( szPosFile );

		//char szLine[ 1024 ];
		uString szLine;
		char szFilename[ 1024 ];
		char szTemp[ 30 ];
		float subX;
		float subY;
		float subWidth;
		float subHeight;
		uString sTemp;
		while ( !oFile.IsEOF() )
		{
			// extract values, one sub image per line
			//if ( !fgets( szLine, 1024, pFile ) ) continue;
			oFile.ReadLine( szLine );

			// filename
			const char *szRemaining = szLine.GetStr();
			const char *szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			uint32_t dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szFilename, szRemaining, dwLength );
			szFilename[ dwLength ] = '\0';

			// x
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subX = sTemp.ToInt() * realScaleX;

			// y
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subY = sTemp.ToInt() * realScaleY;

			// width
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( !szFound ) continue;
			dwLength = (uint32_t) (szFound - szRemaining);
			strncpy( szTemp, szRemaining, dwLength );
			szTemp[ dwLength ] = '\0';
			sTemp.SetStr( szTemp );
			subWidth = sTemp.ToInt() * realScaleX;

			// height
			szRemaining = szFound + 1;
			szFound = strchr( szRemaining, ':' );
			if ( szFound ) continue; // don't want to find another ':' this time
			strcpy( szTemp, szRemaining );
			sTemp.SetStr( szTemp );
			sTemp.Trunc( '\r' );
			subHeight = sTemp.ToInt() * realScaleY;

			subX = subX * m_fScaledAmount;
			subY = subY * m_fScaledAmount;
			subWidth = subWidth * m_fScaledAmount;
			subHeight = subHeight * m_fScaledAmount;

			if ( subX < 0 ) subX = 0;
			if ( subY < 0 ) subY = 0;
			if ( subX > GetTotalWidth() ) subX = (float)GetTotalWidth();
			if ( subY > GetTotalHeight() ) subY = (float)GetTotalHeight();

			if ( subX + subWidth > GetTotalWidth() ) subWidth = GetTotalWidth() - subX;
			if ( subY + subHeight > GetTotalHeight() ) subHeight = GetTotalHeight() - subY;

			// add new sub image details
			cSubImage *pSubImage = new cSubImage();
			pSubImage->x = subX;
			pSubImage->y = subY;
			pSubImage->width = subWidth;
			pSubImage->height = subHeight;
			pSubImage->szFilename.SetStr( szFilename );
			pSubImage->pNextSubImage = m_pSubImages;
			m_pSubImages = pSubImage;
		}

		oFile.Close();
	}
	else
	{
		szPosFile.SetStr( m_szFile );
		szPosFile.Trunc( '.' );
		szPosFile.Append( ".atlas" );

		if ( cFile::Exists( szPosFile ) )
		{
			cFile oFile;
			oFile.OpenToRead( szPosFile );

			// ignore header information
			uString szLine;
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );
			oFile.ReadLine( szLine );

			while ( !oFile.IsEOF() && strncmp( szLine, "repeat: ", strlen("repeat: ") ) != 0 )
			{
				oFile.ReadLine( szLine );
			}

			if ( oFile.IsEOF() )
			{
				uString info;
				info.Format( "Unrecognised .atlas file format: %s", szPosFile.GetStr() );
				agk::Message( info.GetStr() );
				agk::Error( info.GetStr() );
				oFile.Close();
				return true;
			}

			char szFilename[ 1024 ];
			char szTemp[ 30 ];
			float subX;
			float subY;
			float subWidth;
			float subHeight;
			uString sTemp;
			while ( !oFile.IsEOF() )
			{
				// filename
				oFile.ReadLine( szLine );
				if ( szLine.CharAt(0) == '\n' || szLine.CharAt(0) == '\r' )
				{
					uString info;
					info.Format( "AGK does not support multiple textures inside a single .atlas file: %s", szPosFile.GetStr() );
					agk::Message( info.GetStr() );
					agk::Error( info.GetStr() );
					oFile.Close();
					return true;
				}
				szLine.Trim( " \t\n\r" );
				strcpy( szFilename, szLine );

				// rotate
				oFile.ReadLine( szLine );
				
				// XY
				oFile.ReadLine( szLine );

				// x
				const char *szRemaining = szLine.GetStr();
				const char *szFound = strchr( szRemaining, ':' );
				if ( !szFound ) continue;
				szRemaining = szFound + 1;
				szFound = strchr( szRemaining, ',' );
				if ( !szFound ) continue;
				uint32_t dwLength = (uint32_t) (szFound - szRemaining);
				strncpy( szTemp, szRemaining, dwLength );
				szTemp[ dwLength ] = '\0';
				sTemp.SetStr( szTemp );
				subX = sTemp.ToInt() * realScaleX;

				// y
				szRemaining = szFound + 1;
				strcpy( szTemp, szRemaining );
				sTemp.SetStr( szTemp );
				sTemp.Trim( " \n\r\t" );
				subY = sTemp.ToInt() * realScaleY;

				// Size
				oFile.ReadLine( szLine );

				// width
				szRemaining = szLine.GetStr();
				szFound = strchr( szRemaining, ':' );
				if ( !szFound ) continue;
				szRemaining = szFound + 1;
				szFound = strchr( szRemaining, ',' );
				if ( !szFound ) continue;
				dwLength = (uint32_t) (szFound - szRemaining);
				strncpy( szTemp, szRemaining, dwLength );
				szTemp[ dwLength ] = '\0';
				sTemp.SetStr( szTemp );
				subWidth = sTemp.ToInt() * realScaleX;

				// height
				szRemaining = szFound + 1;
				strcpy( szTemp, szRemaining );
				sTemp.SetStr( szTemp );
				sTemp.Trim( " \n\r\t" );
				subHeight = sTemp.ToInt() * realScaleY;

				// orig
				oFile.ReadLine( szLine );
				// offset
				oFile.ReadLine( szLine );
				// index
				oFile.ReadLine( szLine );

				subX = subX * m_fScaledAmount;
				subY = subY * m_fScaledAmount;
				subWidth = subWidth * m_fScaledAmount;
				subHeight = subHeight * m_fScaledAmount;

				if ( subX < 0 ) subX = 0;
				if ( subY < 0 ) subY = 0;
				if ( subX > GetTotalWidth() ) subX = (float)GetTotalWidth();
				if ( subY > GetTotalHeight() ) subY = (float)GetTotalHeight();

				if ( subX + subWidth > GetTotalWidth() ) subWidth = GetTotalWidth() - subX;
				if ( subY + subHeight > GetTotalHeight() ) subHeight = GetTotalHeight() - subY;

				// add new sub image details
				cSubImage *pSubImage = new cSubImage();
				pSubImage->x = subX;
				pSubImage->y = subY;
				pSubImage->width = subWidth;
				pSubImage->height = subHeight;
				pSubImage->szFilename.SetStr( szFilename );
				pSubImage->pNextSubImage = m_pSubImages;
				m_pSubImages = pSubImage;
			}

			oFile.Close();
		}
	}
    
	// save new sub images to make loading faster next time
	if ( m_pSubImages && cache > 0 )
	{
		uString szSubImagesFile( sNewFilename );
		szSubImagesFile.Trunc( '.' );
		szSubImagesFile.Append( " subimages.txt" );

		cFile oFile;
		oFile.OpenToWrite( szSubImagesFile );

		uString sLine;
		cSubImage *pSubImage = m_pSubImages;
		while ( pSubImage ) 
		{
			sLine.Format( "%s:%f:%f:%f:%f", pSubImage->szFilename.GetStr(), pSubImage->x, pSubImage->y, pSubImage->width, pSubImage->height );
			oFile.WriteLine( sLine );

			pSubImage = pSubImage->pNextSubImage;
		}

		oFile.Close();
	}

	return true;
}

void cImage::Resize( int width, int height )
{
	if ( m_pParentImage ) return;

	unsigned int *pDest = new unsigned int[ width*height ];

	if ( m_iGifNumFrames > 0 )
	{
		unsigned long realSize = m_iOrigWidth * m_iOrigHeight * 4;
		unsigned char* pSrc = new unsigned char[ realSize ];

		uLong upperBound = compressBound( width * height * 4 );
		unsigned char *tempbuf = new unsigned char[ upperBound ];

		for( unsigned int i = 0; i < m_iGifNumFrames; i++ )
		{
			if ( m_pGifFrames[ i ]->pData )
			{
				uncompress( pSrc, &realSize, m_pGifFrames[i]->pData, m_pGifFrames[i]->iCompressedSize );
				delete [] m_pGifFrames[i]->pData;
				m_pGifFrames[i]->pData = 0;

				CommonResize( pSrc, m_iOrigWidth, m_iOrigHeight, (unsigned char*)pDest, width, height );

				uLong finalSize = upperBound;
				int err = compress2( tempbuf, &finalSize, (unsigned char*)pDest, width * height * 4, 1 );
				if ( err == Z_OK )
				{
					m_pGifFrames[ i ]->pData = new unsigned char[ finalSize ];
					memcpy(m_pGifFrames[ i ]->pData, tempbuf, finalSize );
					m_pGifFrames[ i ]->iCompressedSize = (uint32_t) finalSize;
				}
			}
		}

		delete [] tempbuf;
		delete [] pSrc;
	}

	unsigned char *pSrc = 0;
	int size = GetRawData( &pSrc );
	if ( size == 0 ) return;

	int origWidth = m_iOrigWidth;
	int origHeight = m_iOrigHeight;

	CommonResize( pSrc, m_iOrigWidth, m_iOrigHeight, (unsigned char*)pDest, width, height );
	delete [] pSrc;

	LoadFromData( width, height, pDest );
	delete [] pDest;

	if ( m_pSubImages )
	{
		float diffX = m_iOrigWidth / (float)origWidth;
		float diffY = m_iOrigHeight / (float)origHeight;

		cSubImage *pSubImage = m_pSubImages;
		while ( pSubImage ) 
		{
			pSubImage->x = pSubImage->x * diffX;
			pSubImage->y = pSubImage->y * diffY;
			pSubImage->width = pSubImage->width * diffX;
			pSubImage->height = pSubImage->height * diffY;

			pSubImage = pSubImage->pNextSubImage;
		}
	}
}

int cImage::GetRawData( unsigned char** pData )
{
	if ( HasParent() ) 
	{
		if ( !agk::GetRenderer()->GetImageData( &m_pParentImage->m_pInternalResource, (uint8_t**) pData ) ) return 0;
	}
	else
	{
		if ( !agk::GetRenderer()->GetImageData( &m_pInternalResource, (uint8_t**) pData ) ) return 0;
	}
	
	uint32_t iTrueWidthSrc = GetTotalWidth();
	uint32_t iTrueHeightSrc = GetTotalHeight();
	uint32_t iOrigWidthSrc = GetWidth();
	uint32_t iOrigHeightSrc = GetHeight();

	if ( iOrigWidthSrc == iTrueWidthSrc && iOrigHeightSrc == iTrueHeightSrc )
	{
		return iTrueWidthSrc*iTrueHeightSrc*4;
	}
	else
	{
		unsigned char* data2 = new unsigned char[ iOrigWidthSrc*iOrigHeightSrc*4 ];

		// get the offsets for the src image
		int startXSrc = agk::Floor( GetU1() * iTrueWidthSrc );
		int startYSrc = agk::Floor( GetV1() * iTrueHeightSrc );

		for ( uint32_t x = 0; x < iOrigWidthSrc; x++ )
		{
			for ( uint32_t y = 0; y < iOrigHeightSrc; y++ )
			{
				uint32_t index = y*iOrigWidthSrc + x;
				uint32_t index2 = (y+startYSrc)*iTrueWidthSrc + x + startXSrc;

				((uint32_t*)data2)[ index ] = ((uint32_t*)(*pData))[ index2 ];
			}
		}

		delete [] (*pData);
		*pData = data2;

		return iOrigWidthSrc*iOrigHeightSrc*4;
	}
}

void cImage::SaveSubImages()
{
	if ( HasParent() ) return;
	if ( !m_pSubImages ) return;

	unsigned char* pData = 0;
	if ( m_pParentImage ) agk::GetRenderer()->GetImageData( &m_pParentImage->m_pInternalResource, &pData );
	else agk::GetRenderer()->GetImageData( &m_pInternalResource, &pData );
	if ( pData == 0 ) return;
	
	uint32_t iTrueWidthSrc = GetTotalWidth();
	uint32_t iTrueHeightSrc = GetTotalHeight();

	cSubImage *pSubImage = m_pSubImages;
	while( pSubImage )
	{		
		// get the offsets for the src image
		int startXSrc = agk::Round( pSubImage->x );
		int startYSrc = agk::Round( pSubImage->y );

		uint32_t width = agk::Round( pSubImage->width );
		uint32_t height = agk::Round( pSubImage->height );

		unsigned char* data2 = new unsigned char[ width*height*4 ];

		for ( uint32_t x = 0; x < width; x++ )
		{
			for ( uint32_t y = 0; y < height; y++ )
			{
				uint32_t index = y*width + x;
				uint32_t index2 = (y+startYSrc)*iTrueWidthSrc + x + startXSrc;

				((uint32_t*)data2)[ index ] = ((uint32_t*)(pData))[ index2 ];
			}
		}

		PlatformSaveDataToFile( pSubImage->szFilename, (unsigned char*)data2, width, height );
		delete [] data2;

		pSubImage = pSubImage->pNextSubImage;
	}

	delete [] pData;
}

cImage** cImage::GetFontImages()
{
	if ( m_pLetterImages ) return m_pLetterImages;
	if ( !IsAtlas() ) 
	{
		m_pLetterImages = new cImage*[ 96 ];
		int width = GetWidth()/16;
		int height = GetHeight()/6;
		for( int j = 0; j < 6; j++ )
		{
			for ( int i = 0; i < 16; i++ )
			{
				float x = (float) i*width;
				float y = (float) j*height;
				int index = j*16 + i;

				m_pLetterImages[ index ] = new cImage();
				m_pLetterImages[ index ]->m_bDontLink = true;

				float u1 = x / (float) GetTotalWidth() + GetU1();
				float v1 = y / (float) GetTotalHeight() + GetV1();
				float u2 = (x+width) / (float) GetTotalWidth() + GetU1();
				float v2 = (y+height) / (float) GetTotalHeight() + GetV1();
				
				m_pLetterImages[ index ]->m_fU1 = u1;
				m_pLetterImages[ index ]->m_fV1 = v1;
				m_pLetterImages[ index ]->m_fU2 = u2;
				m_pLetterImages[ index ]->m_fV2 = v2;

				m_pLetterImages[ index ]->m_shaderVarTexBounds.SetVector4( u2 - u1, v2 - v1, u1, v1 );

				m_pLetterImages[ index ]->m_iFlags |= AGK_IMAGE_HAS_MODIFIED_UVS;

				m_pLetterImages[ index ]->m_pParentImage = this;
				m_pLetterImages[ index ]->m_iWidth = width;
				m_pLetterImages[ index ]->m_iHeight = height;
				m_pLetterImages[ index ]->m_iOrigWidth = width;
				m_pLetterImages[ index ]->m_iOrigHeight = height;
				m_pLetterImages[ index ]->m_szFile.SetStr( "Fixed Width Letter" );
			}
		}
	}
	else
	{
		m_pLetterImages = new cImage*[ 96 ];
		// load sub images
		for ( int i = 0; i < 96; i++ )
		{
			uString sLetter;
			sLetter.Format( "%d", i+32 );
			m_pLetterImages[ i ] = new cImage();
			m_pLetterImages[ i ]->m_bDontLink = true;
			bool result = m_pLetterImages[ i ]->LoadSubImage( this, sLetter, true );
			if ( !result )
			{
				delete m_pLetterImages[ i ];
				m_pLetterImages[ i ] = 0;
			}
		}
	}

	return m_pLetterImages;
}

cImage** cImage::GetExtendedFontImages()
{
	if ( m_pLetterImagesExt ) return m_pLetterImagesExt;
	if ( !IsAtlas() ) 
	{
		m_pLetterImagesExt = new cImage*[ 128 ];
		int width = GetWidth()/16;
		int height = GetHeight()/8;
		for( int j = 0; j < 8; j++ )
		{
			for ( int i = 0; i < 16; i++ )
			{
				float x = (float) i*width;
				float y = (float) j*height;
				int index = j*16 + i;

				m_pLetterImagesExt[ index ] = new cImage();
				m_pLetterImagesExt[ index ]->m_bDontLink = true;
				
				float u1 = x / (float) GetTotalWidth() + GetU1();
				float v1 = y / (float) GetTotalHeight() + GetV1();
				float u2 = (x+width) / (float) GetTotalWidth() + GetU1();
				float v2 = (y+height) / (float) GetTotalHeight() + GetV1();

				m_pLetterImagesExt[ index ]->m_fU1 = u1;
				m_pLetterImagesExt[ index ]->m_fV1 = v1;
				m_pLetterImagesExt[ index ]->m_fU2 = u2;
				m_pLetterImagesExt[ index ]->m_fV2 = v2;

				m_pLetterImagesExt[ index ]->m_shaderVarTexBounds.SetVector4( u2 - u1, v2 - v1, u1, v1 );

				m_pLetterImagesExt[ index ]->m_iFlags |= AGK_IMAGE_HAS_MODIFIED_UVS;

				m_pLetterImagesExt[ index ]->m_pParentImage = this;
				m_pLetterImagesExt[ index ]->m_iWidth = width;
				m_pLetterImagesExt[ index ]->m_iHeight = height;
				m_pLetterImagesExt[ index ]->m_iOrigWidth = width;
				m_pLetterImagesExt[ index ]->m_iOrigHeight = height;
				m_pLetterImagesExt[ index ]->m_szFile.SetStr( "Fixed Width LetterExt" );
			}
		}
	}
	else
	{
		m_pLetterImagesExt = new cImage*[ 128 ];
		// load sub images
		for ( int i = 0; i < 128; i++ )
		{
			uString sLetter;
			sLetter.Format( "%d", i+128 );
			m_pLetterImagesExt[ i ] = new cImage();
			m_pLetterImagesExt[ i ]->m_bDontLink = true;
			bool result = m_pLetterImagesExt[ i ]->LoadSubImage( this, sLetter, true );
			if ( !result )
			{
				delete m_pLetterImagesExt[ i ];
				m_pLetterImagesExt[ i ] = 0;
			}
		}
	}

	return m_pLetterImagesExt;
}

bool cImage::GetGifFromFile( const char* szFile, unsigned char **pData, unsigned int *out_width, unsigned int *out_height )
{
	/*
	// no longer needed now that OpenToRead is being used
	if ( !cFile::Exists( szFile ) ) return false;

	uString sPath( szFile );
	if ( !agk::GetRealPath( sPath ) )
	{
		agk::Warning( "Failed to find gif image" );
		return false;
	}
	*/

	GifFileType *pGif;
	//pGif = ::DGifOpenFileName( sPath.GetStr() );
	cFile sFile; sFile.OpenToRead( szFile );
	pGif = ::DGifOpen( &sFile, ReadGifFile );
	if ( !pGif )
	{
		agk::Warning( "Failed to load gif image" );
		return false;
	}

	int cxScreen =  pGif->SWidth;
	int cyScreen = pGif->SHeight;
	*out_width = cxScreen;
	*out_height = cyScreen;
//	uint32_t dwRowBytes = ((cxScreen * 3) + 3) & ~3; // pad to multiple of 4
//	DWORD dwScreen = dwRowBytes * cyScreen;

	uint32_t *bits = new uint32_t[ cxScreen * cyScreen ];
	if ( pGif->SColorMap )
	{
		GifColorType* pColor = pGif->SColorMap->Colors + pGif->SBackGroundColor;
		uint32_t color = (((((0xff << 8) | pColor->Red) << 8) | pColor->Green) << 8) | pColor->Blue;
		for ( int i = 0; i < cxScreen * cyScreen; i++ ) bits[i] = color;
	}
	else
	{
		for ( int i = 0; i < cxScreen * cyScreen; i++ ) bits[i] = 0;
	}
	bool bFound = false;

	const int InterlacedOffset[] = { 0, 4, 2, 1 }; 
	const int InterlacedJumps[] = { 8, 8, 4, 2 };  

	m_pGifFrames = new cGifFrame*[ 4 ];
	m_iGifFramesArraySize = 4;
	m_iGifNumFrames = 0;
	m_iGifCurrFrame = 0;

	if ( g_pAllGifImages ) g_pAllGifImages->m_pPrevGifImage = this;
	m_pPrevGifImage = 0;
	m_pNextGifImage = g_pAllGifImages;
	g_pAllGifImages = this;

	GifRecordType RecordType;
	GifByteType *pExtension;
	int delay = 10;     // Default to 100 msec
	int dispose = 0;
	int transparent = -1;
	do 
	{
		int i, ExtCode;

		if ( DGifGetRecordType(pGif, &RecordType) == GIF_ERROR ) 
		{
			break;
		}

		switch (RecordType)
		{
		  case IMAGE_DESC_RECORD_TYPE:
			if ( DGifGetImageDesc(pGif) != GIF_ERROR )
			{
				int x = pGif->Image.Left;
				int y = pGif->Image.Top;
				
				GifColorType* pColorTable;
				if ( pGif->Image.ColorMap == NULL )
				{
					pColorTable = pGif->SColorMap->Colors;
				}
				else
				{
					pColorTable = pGif->Image.ColorMap->Colors;
				}

				bFound = true;

				if ( dispose == 2 )
				{
					// fill with background color
					GifColorType* pColor = pColorTable + pGif->SBackGroundColor;
					uint32_t color = (((((0xff << 8) | pColor->Red) << 8) | pColor->Green) << 8) | pColor->Blue;
					for ( int i = 0; i < cxScreen * cyScreen; i++ ) bits[i] = color;
				}

				int Width = pGif->Image.Width;
				int Height = pGif->Image.Height;
				GifPixelType *pLine = new unsigned char[ Width ];
				if ( pGif->Image.Interlace )
				{
					// Need to perform 4 passes on the images:
					for (int pass = 0; pass < 4; pass++)
					{
						for (i = InterlacedOffset[pass]; i < Height; i += InterlacedJumps[pass])
						{
							if (DGifGetLine( pGif, pLine, Width ) == GIF_ERROR)
							{
								DGifCloseFile( pGif );
								return false;
							}

							uint32_t index = (y+i)*cxScreen + x;
							for ( int j = 0; j < Width; j++ )
							{
								unsigned char b = *(pLine + j);
								if (transparent < 0 || b != transparent)
								{
									// Translate to 24-bit RGB value if not transparent
									GifColorType* pColor = pColorTable + b;
									uint32_t red = pColor->Blue;
									uint32_t green = pColor->Green;
									uint32_t blue = pColor->Red;
									uint32_t alpha = 0xff;

									bits[ index ] = (((((alpha << 8) | red) << 8) | green) << 8) | blue;
								}
								
								index++;
							}
						}
					}
				}
				else
				{
					// Non-interlaced image
					for (i = 0; i < Height; i++)
					{
						if (DGifGetLine(pGif, pLine, Width) == GIF_ERROR)
						{
							DGifCloseFile( pGif );
							return false;
						}
						
						uint32_t index = (y+i)*cxScreen + x;
						for ( int j = 0; j < Width; j++ )
						{
							unsigned char b = *(pLine + j);
							if (transparent < 0 || b != transparent)
							{
								// Translate to 24-bit RGB value if not transparent
								GifColorType* pColor = pColorTable + b;
								uint32_t red = pColor->Blue;
								uint32_t green = pColor->Green;
								uint32_t blue = pColor->Red;
								uint32_t alpha = 0xff;

								
								bits[ index ] = (((((alpha << 8) | red) << 8) | green) << 8) | blue;
							}
							
							index++;
						}
					}
				}

				if ( m_iGifNumFrames == m_iGifFramesArraySize )
				{
					int newSize = m_iGifFramesArraySize * 3 / 2;
					cGifFrame ** newFrames = new cGifFrame*[ newSize ];
					for( unsigned int i = 0; i < m_iGifNumFrames; i++ ) newFrames[ i ] = m_pGifFrames[ i ];
					delete [] m_pGifFrames;
					m_pGifFrames = newFrames;
					m_iGifFramesArraySize = newSize;
				}

				cGifFrame *pNewFrame = new cGifFrame();

				uLong upperBound = compressBound( cxScreen * cyScreen * 4 );
				unsigned char *tempbuf = new unsigned char[ upperBound ];
				int err = compress2( tempbuf, &upperBound, (unsigned char*)bits, cxScreen * cyScreen * 4, 1 );
				if ( err == Z_OK )
				{
					pNewFrame->pData = new unsigned char[ upperBound ];
					memcpy(pNewFrame->pData, tempbuf, upperBound );
					delete [] tempbuf;
					pNewFrame->iCompressedSize = (uint32_t) upperBound;
				}

				pNewFrame->fDelay = delay / 100.0f;

				if ( m_iGifNumFrames == 0 ) 
				{
					unsigned int* bits2 = new unsigned int[ cxScreen * cyScreen ];
					for ( int i = 0; i < cxScreen * cyScreen; i++ ) bits2[i] = bits[i];
					*pData = (unsigned char*)bits2;
				}

				m_pGifFrames[ m_iGifNumFrames ] = pNewFrame;
				m_iGifNumFrames++;
			}
			break;
		case EXTENSION_RECORD_TYPE:
        {
			if (DGifGetExtension(pGif, &ExtCode, &pExtension) == GIF_ERROR)
			{
				DGifCloseFile( pGif );
				return false;
			}
            int bNetscapeExt = 0;
			switch (ExtCode)
			{
				case COMMENT_EXT_FUNC_CODE:
					break;
				case GRAPHICS_EXT_FUNC_CODE:
				{
					//ASSERT( pExtension[0] == 4 );
					// 
					int flag = pExtension[1];
					delay  = (pExtension[2] & 0xFF) | ((pExtension[3] & 0xFF) << 8);
					transparent = (flag & 1) ? pExtension[4] : -1;
					dispose = (flag >> 2) & 7;

					break;
				}
				case PLAINTEXT_EXT_FUNC_CODE:
					break;
				case APPLICATION_EXT_FUNC_CODE:
				{
	  				//ASSERT( pExtension );
	                //if (memcmp(pExtension, szNetscape20ext, szNetscape20ext[0]) == 0)
					{
	                    //bNetscapeExt = TRUE;
					}
					break;
				}
				default:
					//TRACE("pExtension record of unknown type");
					break;
			}
			
			do
			{
				if (DGifGetExtensionNext(pGif, &pExtension) == GIF_ERROR)
				{
					DGifCloseFile( pGif );
					return false;
				}
                // Process Netscape 2.0 extension (GIF looping)
                if (pExtension && bNetscapeExt)
                {
                    GifByteType bLength = pExtension[0];
                    int iSubCode = pExtension[1] & 0x07;
                    if (bLength == 3 && iSubCode == 1)
                    {
                        //uint32_t uLoopCount = MAKEWORD(pExtension[2], pExtension[3]);
//						uint32_t uLoopCount = ((pExtension[3] & 0xff) << 8) | (pExtension[2] & 0xff);
//                        m_uLoopCount = uLoopCount - 1;
                    }
                }
			}
			while (pExtension);
			break;
        }
		case TERMINATE_RECORD_TYPE:
			break;
		default:		     // Should be trapped by DGifGetRecordType
			break;
		}
	}
	while (RecordType != TERMINATE_RECORD_TYPE /*&& RecordType != IMAGE_DESC_RECORD_TYPE*/);

	delete [] bits;

	DGifCloseFile( pGif );

	if ( !bFound ) 
	{
		agk::Warning( "Failed to parse gif image" );
		return false;
	}

	return true;
}

void cImage::Save( const char* filename )
{
	unsigned char *pData = 0;
	GetRawData( &pData );
	if ( !pData )
	{
		agk::Warning( "Failed to get image data" );
		return;
	}

	PlatformSaveDataToFile( filename, pData, m_iOrigWidth, m_iOrigHeight );

	delete [] pData;
}

void cImage::PlatformSaveDataToFile( const char* szFile, unsigned char *pData, unsigned int width, unsigned int height )
{
	uString sPath( szFile );
	if ( strncmp(szFile, "raw:", 4) == 0 ) sPath.SetStr( szFile+4 );
	else agk::PlatformGetFullPathWrite(sPath);

	if ( !agk::PlatformCreateRawPath( sPath ) ) return;

	uString ext;
	int pos = sPath.RevFind( '.' );
	if ( pos >= 0 )
	{
		sPath.SubString( ext, pos+1 );
	}
	ext.Lower();

	if ( ext.CompareTo( "png" ) == 0 ) write_png( sPath.GetStr(), width, height, (unsigned int*)pData );
	else WriteJPEG( sPath.GetStr(), 90, pData, width, height );
}

void cImage::SetMask( cImage* pSrcImage, int dst, int src, int dstX, int dstY )
{
	if ( !pSrcImage ) return;
	if ( dst < 1 || dst > 4 ) return;
	if ( src < 1 || src > 4 ) return;
	int srcX = 0;
	int srcY = 0;
	if ( dstX < 0 ) { srcX = -dstX; dstX = 0; }
	if ( dstY < 0 ) { srcY = -dstY; dstY = 0; }
	
	// get src image size
	int iTrueWidthSrc = pSrcImage->GetTotalWidth();
	int iTrueHeightSrc = pSrcImage->GetTotalHeight();
	int iOrigWidthSrc = pSrcImage->GetWidth();
	int iOrigHeightSrc = pSrcImage->GetHeight();

	// get the offsets for the src image
	int startXSrc = agk::Floor( pSrcImage->GetU1() * iTrueWidthSrc );
	int startYSrc = agk::Floor( pSrcImage->GetV1() * iTrueHeightSrc );

	// get dest  image size
	int iTrueWidth = GetTotalWidth();
	int iTrueHeight = GetTotalHeight();
	int iOrigWidth = GetWidth();
	int iOrigHeight = GetHeight();

	// get the offsets for the dest image
	int startX = agk::Floor( GetU1() * iTrueWidth );
	int startY = agk::Floor( GetV1() * iTrueHeight );

	if ( srcX > iOrigWidthSrc || srcY > iOrigHeightSrc ) return;
	if ( dstX > iOrigWidth || dstY > iOrigHeight ) return;

	iOrigWidthSrc -= srcX; iOrigHeightSrc -= srcY;
	startXSrc += srcX; startYSrc += srcY;

	iOrigWidth -= dstX; iOrigHeight -= dstY;
	startX += dstX; startY += dstY;

	uint32_t* bitsSrc = 0;
	uint32_t* bits = 0;

	if ( pSrcImage->m_pParentImage ) agk::GetRenderer()->GetImageData( &pSrcImage->m_pParentImage->m_pInternalResource, (uint8_t**) &bitsSrc );
	else agk::GetRenderer()->GetImageData( &pSrcImage->m_pInternalResource, (uint8_t**) &bitsSrc );
	if ( !bitsSrc )
	{
		agk::Warning("Failed to get source image data");
		return;
	}

	if ( m_pParentImage ) agk::GetRenderer()->GetImageData( &m_pParentImage->m_pInternalResource, (uint8_t**) &bits );
	else agk::GetRenderer()->GetImageData( &m_pInternalResource, (uint8_t**) &bits );
	if ( !bits )
	{
		agk::Warning("Failed to get destination image data");
		return;
	}
	
	// images may not be the same size so use the smallest.
	int minWidth = iOrigWidth < iOrigWidthSrc ? iOrigWidth : iOrigWidthSrc;
	int minHeight = iOrigHeight < iOrigHeightSrc ? iOrigHeight : iOrigHeightSrc;

	// calculate shifts and masks for the pixel loop
	int srcshift = 0;
	switch( src )
	{
		case 2: srcshift = 8; break;
		case 3: srcshift = 16; break;
		case 4: srcshift = 24; break;
		default: srcshift = 0; break;
	}

	int dstshift = 0;
	int dstmask = 0xffffff00;
	switch( dst )
	{
		case 2: dstshift = 8;  dstmask = 0xffff00ff; break;
		case 3: dstshift = 16; dstmask = 0xff00ffff; break;
		case 4: dstshift = 24; dstmask = 0x00ffffff; break;
		default: dstshift = 0; dstmask = 0xffffff00; break;
	}

	uint32_t *bitsSub = new uint32_t[ minWidth*minHeight ];
	// copy pixels from src to dst
	for ( int y = 0; y < minHeight; y++ )
	{
		int index = (y+startY)*iTrueWidth + startX; // pixel on dest texture
		int index2 = (y+startYSrc)*iTrueWidthSrc + startXSrc; // pixel on src texture
		int indexSub = y*minWidth;

		for ( int x = 0; x < minWidth; x++ )
		{
			// filter out the channel we want
			uint32_t srcPixel = (bitsSrc[index2] >> srcshift) & 0xff;

			// mask out the current channel and OR in the src version.
			uint32_t pixel = bits[index] & dstmask;
			bitsSub[indexSub] = pixel | (srcPixel << dstshift);

			// set dest as well for compressed pixel data
			bits[index] = bitsSub[indexSub];

			index++;
			index2++;
			indexSub++;
		}
	}

	if ( m_pParentImage ) 
	{
		//m_pParentImage->SetCompressedPixelData( (unsigned char*)bits, iTrueWidth*iTrueHeight*4 );
		m_pParentImage->SetCompressedPixelData( 0, 0 );
		agk::GetRenderer()->UpdateSubImage2D( m_pParentImage->m_pInternalResource, 0, startX, startY, minWidth, minHeight, (unsigned char*)bitsSub );
	}
	else 
	{
		//SetCompressedPixelData( (unsigned char*)bits, iTrueWidth*iTrueHeight*4 );
		SetCompressedPixelData( 0, 0 );
		agk::GetRenderer()->UpdateSubImage2D( m_pInternalResource, 0, startX, startY, minWidth, minHeight, (unsigned char*)bitsSub );
	}

	delete [] bits;
	delete [] bitsSrc;
	delete [] bitsSub;
}

void cImage::CopyFrom( cImage *pFromImage, int srcX, int srcY, int width, int height )
{
	if ( !pFromImage ) return;
	if ( srcX < 0 || srcY < 0 ) return;
	
	// get src image size
	int iTrueWidthSrc = pFromImage->GetTotalWidth();
	int iTrueHeightSrc = pFromImage->GetTotalHeight();
	int iOrigWidthSrc = pFromImage->GetWidth();
	int iOrigHeightSrc = pFromImage->GetHeight();

	// get the offsets for the src image
	int startXSrc = agk::Floor( pFromImage->GetU1() * iTrueWidthSrc );
	int startYSrc = agk::Floor( pFromImage->GetV1() * iTrueHeightSrc );

	if ( srcX > iOrigWidthSrc || srcY > iOrigHeightSrc ) return;
	startXSrc += srcX;
	startYSrc += srcY;
	
	// get src pixels from OpenGL
	uint32_t* bitsSrc = 0;
	if ( pFromImage->m_pParentImage ) agk::GetRenderer()->GetImageData( &pFromImage->m_pParentImage->m_pInternalResource, (uint8_t**) &bitsSrc );
	else agk::GetRenderer()->GetImageData( &pFromImage->m_pInternalResource, (uint8_t**) &bitsSrc );
	if ( !bitsSrc )
	{
		agk::Warning("Failed to get source image data");
		return;
	}
	
	uint32_t *bits = new uint32_t[ width*height ];
	
	for ( int y = 0; y < height; y++ )
	{
		uint32_t index = y*width;
		uint32_t index2 = (y+startYSrc)*iTrueWidthSrc + startXSrc;

		for ( int x = 0; x < width; x++ )
		{
			bits[ index ] = bitsSrc[ index2 ];

			index++;
			index2++;
		}
	}

	delete [] bitsSrc;

	LoadFromData( width, height, bits );

	delete [] bits;
}

void cImage::SetTransparentColor( int red, int green, int blue )
{
	if ( red < 0 ) red = 0;
	if ( green < 0 ) green = 0;
	if ( blue < 0 ) blue = 0;

	if ( red > 255 ) red = 255;
	if ( green > 255 ) green = 255;
	if ( blue > 255 ) blue = 255;
		
	// get dest image size
	int iTrueWidth = GetTotalWidth();
	int iTrueHeight = GetTotalHeight();
	int iOrigWidth = GetWidth();
	int iOrigHeight = GetHeight();

	// get the offsets for the dest image
	int startX = agk::Floor( GetU1() * iTrueWidth );
	int startY = agk::Floor( GetV1() * iTrueHeight );

	// get dest pixels from OpenGL
	uint32_t* bits = 0;
	if ( m_pParentImage ) agk::GetRenderer()->GetImageData( &m_pParentImage->m_pInternalResource, (uint8_t**) &bits );
	else agk::GetRenderer()->GetImageData( &m_pInternalResource, (uint8_t**) &bits );
	if ( !bits )
	{
		agk::Warning("Failed to get image data");
		return;
	}
	
	uint32_t testColor = (((blue << 8) | green) << 8) | red;

	uint32_t *bitsSub = new uint32_t[ iOrigWidth*iOrigHeight ];
	// copy pixels from src to dst
	for ( int y = 0; y < iOrigHeight; y++ )
	{
		int index = (y+startY)*iTrueWidth + startX; // pixel on dest texture
		int indexSub = y*iOrigWidth;

		for ( int x = 0; x < iOrigWidth; x++ )
		{
			// test pixel against the transparent color
			uint32_t srcPixel = bits[index] & 0x00ffffff;
			if ( testColor != srcPixel )
			{
				// opaque
				srcPixel |= 0xff000000;
			}

			bitsSub[indexSub] = srcPixel;
			bits[ index ] = srcPixel;

			index++;
			indexSub++;
		}
	}

	if ( m_pParentImage ) 
	{
		//m_pParentImage->SetCompressedPixelData( (unsigned char*)bits, iTrueWidth*iTrueHeight*4 );
		m_pParentImage->SetCompressedPixelData( 0, 0 );
		agk::GetRenderer()->UpdateSubImage2D( m_pParentImage->m_pInternalResource, 0, startX, startY, m_iOrigWidth, m_iOrigHeight, (unsigned char*)bitsSub );
	}
	else 
	{
		//SetCompressedPixelData( (unsigned char*)bits, iTrueWidth*iTrueHeight*4 );
		SetCompressedPixelData( 0, 0 );
		agk::GetRenderer()->UpdateSubImage2D( m_pInternalResource, 0, startX, startY, m_iOrigWidth, m_iOrigHeight, (unsigned char*)bitsSub );
	}

	delete [] bits;
	delete [] bitsSub;
}

int cImage::DecodeQR( uString &out )
{
#if defined(AGK_BLACKBERRY) || defined(AGK_HTML5)
	return 0;
#else
	if ( !GetInternalResource() ) return 0;

	// get src image size
	uint32_t iOrigWidthSrc = GetWidth();
	uint32_t iOrigHeightSrc = GetHeight();

	// get src pixels from OpenGL
	uint32_t* bitsSrc = 0;
	GetRawData( (unsigned char**) &bitsSrc );
	if ( !bitsSrc ) return 0;

	unsigned char *bits = new unsigned char[ iOrigWidthSrc*iOrigHeightSrc ];
	
	for ( uint32_t y = 0; y < iOrigHeightSrc; y++ )
	{
		uint32_t index = y*iOrigWidthSrc;

		for ( uint32_t x = 0; x < iOrigWidthSrc; x++ )
		{
			int alpha = (bitsSrc[ index ] >> 24) & 0xff;
			int blue = (bitsSrc[ index ] >> 16) & 0xff;
			int green = (bitsSrc[ index ] >> 8) & 0xff;
			int red = bitsSrc[ index ] & 0xff;

			if ( alpha == 0 ) 
			{
				bitsSrc[ index ] = 0;
			}
			else
			{
				//int luminance = agk::Round(red*0.3f + green*0.59f + blue*0.11f);
				int luminance = agk::Round( (red + green + blue)/3.0f );
				if ( luminance > 255 ) luminance = 255;
				if ( luminance < 0 ) luminance = 0;

				bits[ index ] = (unsigned char)luminance;
			}

			index++;
		}
	}

	delete [] bitsSrc;

	using namespace zxing;

	try
	{
		GreyscaleLuminanceSource* greyscale = new GreyscaleLuminanceSource( bits, iOrigWidthSrc, iOrigHeightSrc, 0,0, iOrigWidthSrc, iOrigHeightSrc );
		Ref<GreyscaleLuminanceSource> refGrey( greyscale );
		
		GlobalHistogramBinarizer* binarizer = new GlobalHistogramBinarizer( refGrey );
		Ref<GlobalHistogramBinarizer> refHist( binarizer );
		
		BinaryBitmap* bitmap = new BinaryBitmap( refHist );
		Ref<BinaryBitmap> refBitmap( bitmap );

		DecodeHints hints( DecodeHints::BARCODEFORMAT_QR_CODE_HINT );

		qrcode::QRCodeReader qreader;
		Ref<Result> result = qreader.decode( refBitmap, hints );

		Ref<String> qstring = result->getText();

		out.SetStr( qstring->getText().c_str() );
	}
	catch(...)
	{
		
	}

	delete [] bits;

	return out.GetLength();
#endif
}

bool cImage::EncodeQR( const char* text, int errormode )
{
#if defined(AGK_BLACKBERRY) || defined(AGK_HTML5)
	return false;
#else
	if ( !text ) return false;
	if ( errormode < 0 ) errormode = 0;
	if ( errormode > 3 ) errormode = 3;

	unsigned char pQRCode[ 3917 ]; 
	int size = zxing::EncodeData( errormode, 0, text, 0, pQRCode );
	if ( size <= 0 ) 
	{
		agk::Warning( "Failed to encode QR Code, string may be too long" );
		return false;
	}

	// include a white border of 1 pixel on all sides, and make each QR pixel take up 10 pixels in the image.
	uint32_t border = 1;
	uint32_t width = (size+border*2)*10;
	uint32_t height = (size+border*2)*10;
	
	uint32_t *bits = new uint32_t[ width*height ];
	
	for ( uint32_t y = 0; y < height; y++ )
	{
		uint32_t index = y*width;

		for ( uint32_t x = 0; x < width; x++ )
		{
			if ( (int)x < border*10 || (int)x >= width-border*10 || (int)y < border*10 || (int)y > height-border*10 )
			{
				// white border
				bits[ index ] = 0xffffffff;
			}
			else
			{
				uint32_t tempindex = (y/10 - border)*(width/10 - border*2) + x/10 - border;
				uint32_t index2 = tempindex / 8;
				uint32_t index3 = tempindex % 8;
				
				int iByte = pQRCode[ index2 ];
				int iBit = iByte >> (7-index3);
				iBit = iBit & 1;

				if ( iBit ) bits[ index ] = 0xff000000;
				else bits[ index ] = 0xffffffff;
			}

			index++;
		}
	}

	LoadFromData( width, height, bits );
	delete [] bits;

	return true;
#endif
}

void cImage::CreateFromScreen( int x, int y, int width, int height )
{
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;
	if ( x + width > agk::GetDeviceWidth() ) width = agk::GetDeviceWidth() - x;
	if ( y + height > agk::GetDeviceHeight() ) height = agk::GetDeviceHeight() - y;

	unsigned int *pData = 0;
	agk::GetRenderer()->GetDataFromScreen( &pData, x, y, width, height );
	if ( !pData ) return;

	float backRed = agk::m_fClearRed;
	float backGreen = agk::m_fClearGreen;
	float backBlue = agk::m_fClearBlue;

	for ( int y = 0; y < height; y++ )
	{
		uint32_t index = y*width;

		for ( int x = 0; x < width; x++ )
		{
			// keep or clear alpha?
			//bits[ index ] = data[ index2 ];// | 0xff000000;

			// restore the color values that have been multipled by alpha during rendering
			float red = (pData[ index ] & 0xff) / 255.0f;
			float green = ((pData[ index ] >> 8) & 0xff) / 255.0f;
			float blue = ((pData[ index ] >> 16) & 0xff) / 255.0f;
			float alpha = ((pData[ index ] >> 24) & 0xff) / 255.0f;

			if ( alpha > 0 && alpha < 1 )
			{
				red -= (1-alpha)*backRed;
				green -= (1-alpha)*backGreen;
				blue -= (1-alpha)*backBlue;

				if ( red < 0 ) red = 0;
				if ( green < 0 ) green = 0;
				if ( blue < 0 ) blue = 0;

				red = red / alpha;
				green = green / alpha;
				blue = blue / alpha;

				if ( red > 1 ) red = 1;
				if ( green > 1 ) green = 1;
				if ( blue > 1 ) blue = 1;
			}

			int iRed = agk::Round(red * 255);
			int iGreen = agk::Round(green * 255);
			int iBlue = agk::Round(blue * 255);
			int iAlpha = (pData[ index ] >> 24) & 0xff;

			uint32_t temp = pData[ index ];
			pData[ index ] = (iAlpha << 24) | (iBlue << 16) | (iGreen << 8) | (iRed);

			index++;
		}
	}

	LoadFromData( width, height, pData );

	delete [] pData;
}

const Point2D* cImage::GetBoundingPoints( float u1, float v1, float u2, float v2 )
{
	if ( u1 > u2 )
	{
		float temp = u2;
		u2 = u1;
		u1 = temp;
	}

	if ( v1 > v2 )
	{
		float temp = v2;
		v2 = v1;
		v1 = temp;
	}

	if ( u1 < m_fU1 ) u1 = m_fU1;
	if ( v1 < m_fV1 ) v1 = m_fV1;
	if ( u2 < m_fU1 ) u2 = m_fU1;
	if ( v2 < m_fV1 ) v2 = m_fV1;

	if ( u1 > m_fU2 ) u1 = m_fU2;
	if ( v1 > m_fV2 ) v1 = m_fV2;
	if ( u2 > m_fU2 ) u2 = m_fU2;
	if ( v2 > m_fV2 ) v2 = m_fV2;

	// if points are already calculated for this image, return them
	// can only keep points that use the same UVs
	if ( m_pBoundingPoints
	  && m_fLastU1 == u1
	  && m_fLastV1 == v1 
	  && m_fLastU2 == u2 
	  && m_fLastV2 == v2 
	  && m_iMaxNumPoints == agk::m_iMaxPolygonPoints ) return m_pBoundingPoints;

	while ( m_pBoundingPoints ) 
	{
		Point2D *pPoint = m_pBoundingPoints;
		m_pBoundingPoints = m_pBoundingPoints->pNext;
		delete pPoint;
	}
	
	m_fLastU1 = u1;
	m_fLastV1 = v1;
	m_fLastU2 = u2;
	m_fLastV2 = v2;
	m_iMaxNumPoints = agk::m_iMaxPolygonPoints;
	
	int iSizeU = GetTotalWidth();
	int iSizeV = GetTotalHeight();
	//int iSizeU2 = GetTotalWidth2();
		
	int x = agk::Round(iSizeU * (u1-m_fU1));
	int y = agk::Round(iSizeV * (v1-m_fV1));
	int endx = agk::Round(iSizeU * (u2-m_fU1));
	int endy = agk::Round(iSizeV * (v2-m_fV1));
	int width = endx - x;
	int height = endy - y;

	CheckAndBuildAlphaPixels();

	unsigned char *data2 = new unsigned char[ width*height ];
	for ( int i = 0; i < height; i++ )
	{
		for ( int j = 0; j < width; j++ )
		{
			//int index = ((x + j) + (y + i)*iSizeU2);
			int index2 = j + i*width;
			data2[ index2 ] = GetAlphaPixel( x+j, y+i );
		}
	}

	//delete [] data;
	m_pBoundingPoints = Physics::ConvexHull2D( width, height, data2 );
	delete [] data2;
	
	return m_pBoundingPoints;
}

void cImage::CheckAndBuildAlphaPixels()
{
	if ( HasParent() ) 
	{
		m_pParentImage->CheckAndBuildAlphaPixels();
		return;
	}

	// only rebuild if it doesn't already exist
	if ( m_pAlphaPixels ) return;

	unsigned int *pData;
	int size = GetRawData( (unsigned char**)&pData );
	if ( size != GetWidth() * GetHeight() * 4 )
	{
		if ( pData ) delete [] pData;
		return;
	}

	CreateAlphaPixels( GetWidth()*GetHeight() );
	for ( int y = 0; y < GetHeight(); y++ )
	{
		int index = y*GetWidth();

		for ( int x = 0; x < GetWidth(); x++ )
		{
			unsigned char alpha = pData[index] >> 24;
			SetAlphaPixel( index, alpha );
			index++;
		}
	}

	delete [] pData;
}

void cImage::CreateAlphaPixels( int size )
{
	if ( m_pAlphaPixels ) delete [] m_pAlphaPixels;
	m_bHasAlpha = false;

	int bytes = (size / 8) + 1;
	m_pAlphaPixels = new unsigned char[ bytes ];
	memset( m_pAlphaPixels, 0, bytes );
}

void cImage::SetAlphaPixel( int index, int value )
{
	int index2 = index / 8;
	int index3 = index % 8;

	int mask = 1 << index3;
	int temp = m_pAlphaPixels[ index2 ];

	if ( value < 255 ) m_bHasAlpha = true;

	if ( value > g_iAlphaColThreshold )
	{
		m_pAlphaPixels[ index2 ] = temp | mask;
	}
	else
	{
		mask = ~mask;
		m_pAlphaPixels[ index2 ] = temp & mask;
	}
}

int cImage::GetAlphaPixel( int x, int y )
{
	if ( m_pParentImage ) 
	{
		x += agk::Round( (m_fU1 - m_pParentImage->m_fU1) * GetTotalWidth() );
		y += agk::Round( (m_fV1 - m_pParentImage->m_fV1) * GetTotalHeight() );
		return m_pParentImage->GetAlphaPixel( x, y );
	}
	if ( !m_pAlphaPixels ) return 255;
	
	// m_fScaledAmount shouldn't be used like this? Currently only iOS sets it anyway.
	//x = agk::Floor( x * m_fScaledAmount );
	//y = agk::Floor( y * m_fScaledAmount );
	//int width = agk::Floor( m_iOrigWidth * m_fScaledAmount );
	int width = m_iOrigWidth;
	int index = x + y*width;
		
	int index2 = index / 8;
	int index3 = index % 8;

	int mask = 1 << index3;
	int temp = m_pAlphaPixels[ index2 ] & mask;
	return temp;
}

int cImage::GetAlphaPixelUV( float u, float v )
{
	if ( m_pParentImage ) 
	{
		return m_pParentImage->GetAlphaPixelUV( u, v );
	}
	if ( !m_pAlphaPixels ) return 255;

	int x = agk::Round( u-m_fU1 );
	int y = agk::Round( v-m_fV1 );

	// m_fScaledAmount shouldn't be used like this? Currently only iOS sets it anyway.
	//x = agk::Floor( x * m_fScaledAmount );
	//y = agk::Floor( y * m_fScaledAmount );
	//int width = agk::Floor( m_iOrigWidth * m_fScaledAmount );
	int width = m_iOrigWidth;
	int index = x + y*width;
		
	int index2 = index / 8;
	int index3 = index % 8;

	int mask = 1 << index3;
	int temp = m_pAlphaPixels[ index2 ] & mask;
	return temp;
}

void cImage::SetMagFilter( uint32_t mode )
{
	if ( m_pParentImage )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot set mag filter on a texture derived from an atlas texture." );
#endif
		return;
	}

	if ( mode ) m_iFlags &= ~AGK_IMAGE_MAG_FILTER_NEAREST;
	else m_iFlags |= AGK_IMAGE_MAG_FILTER_NEAREST;
	agk::GetRenderer()->SetImageMagFilter( m_pInternalResource, mode );
}

void cImage::SetMinFilter( uint32_t mode )
{
	if ( m_pParentImage )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot set min filter on a texture derived from an atlas texture." );
#endif
		return;
	}

	if ( mode ) m_iFlags &= ~(AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST);
	else m_iFlags |= AGK_IMAGE_MAG_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST;
	agk::GetRenderer()->SetImageMinFilter( m_pInternalResource, mode );
}

void cImage::SetWrapU( uint32_t mode )
{
	if ( m_pParentImage )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot set UV repeat/clamp mode on a texture derived from an atlas texture." );
#endif
		return;
	}
		
	if ( mode ) m_iFlags |= AGK_IMAGE_WRAP_U_REPEAT;
	else m_iFlags &= ~AGK_IMAGE_WRAP_U_REPEAT;
	agk::GetRenderer()->SetImageWrapU( m_pInternalResource, mode );
}

void cImage::SetWrapV( uint32_t mode )
{
	if ( m_pParentImage )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Cannot set UV repeat/clamp mode on a texture derived from an atlas texture." );
#endif
		return;
	}

	if ( mode ) m_iFlags |= AGK_IMAGE_WRAP_V_REPEAT;
	else m_iFlags &= ~AGK_IMAGE_WRAP_V_REPEAT;
	agk::GetRenderer()->SetImageWrapV( m_pInternalResource, mode );
}
