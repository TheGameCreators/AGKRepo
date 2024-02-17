#ifndef _H_AGK_AGO_
#define _H_AGK_AGO_

// Common includes
#include "Common.h"

namespace AGK
{
	class AGKVertexAttrib;
	class cMesh;
	class cObject3D;

	class AGO
	{
		protected:
			// cannot create an instance of this class
			AGO() {}
			~AGO() {}

		public:
			static int ParseFindChunk( const char *szFilename, char* data, int index, char* chunkName, uint32_t maxLength );
			static int ParseChunkUnknown( const char *szFilename, char* data, int index );
			static int ParseChunkFloat( const char *szFilename, char* data, int index, float* f );
			static int ParseChunkInt( const char *szFilename, char* data, int index, int* i );
			static int ParseChunkString( const char *szFilename, char* data, int index, uString &out );
			static int ParseChunkFloatArray( const char *szFilename, const char *szChunkName, char* data, int index, float* f, uint32_t elements );
			static int ParseChunkIntArray( const char *szFilename, const char *szChunkName, char* data, int index, int* i, uint32_t elements );
			static int ParseChunkObject( const char *szFilename, char* data, int index, cObject3D *pObject );
			static int ParseChunkMesh( const char *szFilename, char* data, int index, cMesh *pMesh );
			static int ParseChunkVertexAttrib( const char *szFilename, char* data, int index, int numVertices, AGKVertexAttrib *pAttrib, bool &bSuccess );
	};
}

#endif