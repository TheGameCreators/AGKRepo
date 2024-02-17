// includes
#include "agk.h"
#include "template.h"

int done = 0;
void PlatformAppQuit()
{
    //exit(0);
    done = 1;
}

void Output ( char const* debugtext )
{
	// report this text to the internal debug reporting system
}


int main (int argc, char **argv)
{
    agk::SetCompanyName( COMPANY_NAME );

	// call app begin
	try
	{
		agk::SetErrorMode(1);
		agk::InitConsole();

		App.Begin();
	}
	catch( ... )
	{
		uString err = agk::GetLastError();
		err.Prepend( "Uncaught exception: \n\n" );
		done = 1;
	}
	while (!done)
	{
		// call app each frame
		try
		{
            App.Loop();
		}
		catch( ... )
		{
			uString err = agk::GetLastError();
			err.Prepend( "Uncaught exception: \n\n" );
			done = 1;
		}
	}
	
	// close app
	App.End();
    agk::CleanUp();
	
	// success    
    return 0;
}
