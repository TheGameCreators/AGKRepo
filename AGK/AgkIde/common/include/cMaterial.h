#ifndef _H_AGK_MATERIAL_
#define _H_AGK_MATERIAL_

// Common includes
#include "Common.h"
#include "cHashedList.h"

#define AGK_MAX_TEXTURES 8

namespace AGK
{
	// forward declarations
	class cMesh;

	class cMaterial
	{
		protected:
			cImage *m_pImages[ AGK_MAX_TEXTURES ];
			cHashedList<cMesh> m_pMeshes;

			cMaterial *m_pNextMaterial;
			cMaterial *m_pPrevMaterial;
			static cMaterial *g_pAllMaterials;

		public:
			cMaterial();
			~cMaterial();

			static cMaterial* FindMaterial( cImage **pDesiredImages ); // must be an array of length AGK_MAX_TEXTURES

			void AddMesh( cMesh *pMesh );
			void RemoveMesh( cMesh *pMesh );
	};

}

#endif