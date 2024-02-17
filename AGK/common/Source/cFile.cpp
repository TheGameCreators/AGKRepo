#include "agk.h"

namespace AGK
{

cFile::cFile()
{
	pFile = 0;
	pFilePtr = 0;
	mode = 0;
}

cFile::~cFile()
{
	Close();
}

bool cFile::IsOpen()
{
	return ( pFile || pFilePtr );
}

}
