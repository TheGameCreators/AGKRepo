
#import "ExternalCommands.h"
#import "AppGameKit_Player-Swift.h"

class NativeAppSDK : public ExternalSDK
{
public:
    void ExternalCommand( const char* command, const char* str1, const char* str2 ) override
    {
        if ( strcmp( command, "switchapp" ) == 0 )
        {
            NativeViewController* viewController = [NativeViewController getSharedViewController];
            NSLog( @"Presenting native view controller" );
            //[[UIApplication sharedApplication].keyWindow.rootViewController showViewController:(UIViewController*)viewController sender:nil];
            [viewController dismissViewControllerAnimated:YES completion:nil];
        }
        else if ( strcmp( command, "retruntoprev" ) == 0 )
        {
            
        }
    }
    
    int ExternalCommandInt( const char* command, const char* str1, const char* str2 ) override
    {
        return 1;
    }
    
    float ExternalCommandFloat( const char* command, const char* str1, const char* str2 ) override
    {
        return 4.0567f;
    }
    
    char* ExternalCommandString( const char* command, const char* str1, const char* str2 ) override
    {
        char* str = new char[10];
        //*str = 0;
        strcpy( str, "hello!" );
        return str;
    }
};

NativeAppSDK cNativeAppSDK;
ExternalSDK* g_pNativeAppSDK = &cNativeAppSDK;
