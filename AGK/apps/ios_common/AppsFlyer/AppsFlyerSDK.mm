
#import "ExternalCommands.h"

#import <AppsFlyerLib/AppsFlyerLib.h>

class AppsFlyerSDK : public ExternalSDK
{
public:
    void ExternalCommand( const char* command, const char* str1, const char* str2 ) override
    {
        if ( strcmp( command, "init" ) == 0 )
        {
            [[AppsFlyerLib shared] setAppsFlyerDevKey:[NSString stringWithUTF8String:str1]];
            [[AppsFlyerLib shared] setAppleAppID:[NSString stringWithUTF8String:str2]];
        }
        else if ( strcmp( command, "start" ) == 0 )
        {
            [[AppsFlyerLib shared] start];
        }
        else if ( strcmp( command, "setdebug" ) == 0 )
        {
            bool enabled = false;
            if ( strcmp( str1, "true" ) == 0 ) enabled = true;
            [AppsFlyerLib shared].isDebug = enabled;
        }
        else if ( strcmp( command, "logevent" ) == 0 )
        {
            NSString* values = [NSString stringWithUTF8String:str2];
            NSArray<NSString*>* valuesArray = [values componentsSeparatedByString:@":"];
            if ( [valuesArray count] == 0 ) return;
            
            NSMutableDictionary<NSString*, id>* dict = [[NSMutableDictionary alloc] init];
            for( NSString* pair in valuesArray )
            {
                NSArray<NSString*>* items = [pair componentsSeparatedByString:@"="];
                if ( [items count] < 2 ) continue;
                [dict setObject:items[1] forKey:items[0]];
            }
            
            [[AppsFlyerLib shared] logEventWithEventName:[NSString stringWithUTF8String:str1] eventValues:dict  completionHandler:nil];
            [dict release];
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

AppsFlyerSDK cAppsFlyerSDK;
ExternalSDK* g_pAppsFlyerSDK = &cAppsFlyerSDK;
