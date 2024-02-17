#import "ExternalCommands.h"

// include any necessary API files

class MySDKClassName : public ExternalSDK
{
public:
    // define any intenal variables
    
    void ExternalCommand( const char* command, const char* str1, const char* str2 ) override
    {
        if ( strcmp( command, "myfirstcommandname" ) == 0 )
        {
            // do something
        }
        else if ( strcmp( command, "mysecondcommandname" ) == 0 )
        {
            // do something
        }
    }
    
    int ExternalCommandInt( const char* command, const char* str1, const char* str2 ) override
    {
        if ( strcmp( command, "myfirstintcommandname" ) == 0 )
        {
            // return something
            return 1;
        }
        else if ( strcmp( command, "mysecondintcommandname" ) == 0 )
        {
            // return something
            return 2;
        }
        
        return 0;
    }
    
    // can also override ExternalCommandFloat and ExternalCommandString, or not
};

MySDKClassName cMySDKClass;
ExternalSDK* g_pMySDKClass = &cMySDKClass; // also add this to ExternalCommands.mm
