#ifndef _H_ZIP_FILE_
#define _H_ZIP_FILE_

#include "Common.h"

namespace AGK
{
	class ZipExtracter : public AGKThread
	{
		public:
			uString m_sFilename;
            uString m_sExtractPath;
			uString m_sPassword;
			volatile float m_fProgress;
			
			ZipExtracter() : AGKThread() {}
			~ZipExtracter() {}

			UINT Run();
	};

	extern ZipExtracter g_ZipExtracter;

	class _EXPORT_ ZipFile
	{
		protected:
			void* m_zf;
			
		public:
			
			ZipFile();
			~ZipFile();

			void Create( const char* filename );
			void CreateAppend(const char* filename, int append);
			bool AddEntry( const char* realPath, const char* zipPath );
			bool AddEntry( const char* realPath, const char* zipPath, int compressionlevel );
			void Close();

			static bool ExtractAll( const char* filename, const char* extractPath, const char* password, volatile float* progress=NULL, volatile bool* stop=NULL );

	};
}

#endif