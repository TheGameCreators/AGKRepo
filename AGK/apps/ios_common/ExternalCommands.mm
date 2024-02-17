#import "ExternalCommands.h"

#include <stdio.h>
#include <string.h>

#import <React/RCTRootView.h>
#import <React/RCTBundleURLProvider.h>

#import <UIKit/UIKit.h>

#import "NativeViewController.h"

// All SDKs should be defined here, even if they are not used
ExternalSDK* g_pAppsFlyerSDK __attribute__((weak)) = 0;
ExternalSDK* g_pIDFATrackingSDK __attribute__((weak)) = 0;

ExternalSDK* GetExternalSDK( const char* sdk )
{
    // add all SDKs here, even unused ones
    if ( strcmp( sdk, "appsflyer" ) == 0 ) return g_pAppsFlyerSDK;
    else if ( strcmp( sdk, "idfatracking" ) == 0 ) return g_pIDFATrackingSDK;

    return 0;
}


// Do not modify anything below here

extern "C" int ExternExternalSDKSupported( const char* sdk )
{
    ExternalSDK* pSDK = GetExternalSDK( sdk );
    return pSDK ? 1 : 0;
}

extern "C" void ExternExternalCommand( const char* sdk, const char* command, const char* str1, const char* str2 )
{
    ExternalSDK* pSDK = NULL;
    if ( strcmp( sdk, "nativeapp" ) == 0 )
    {
        // do not need an SDK ptr
    }
    else
    {
        pSDK = GetExternalSDK( sdk );
        if ( !pSDK ) return;
    }
    
    if ( strcmp( command, "switchapp" ) == 0 )
    {
        NSLog( @"AGKRN: Get URL to React Native Bundle" );
        NSURL *jsCodeLocation;
        //#if DEBUG
        //    jsCodeLocation = [[RCTBundleURLProvider sharedSettings] jsBundleURLForBundleRoot:@"index"];
        //#else
            jsCodeLocation = [[NSBundle mainBundle] URLForResource:@"main" withExtension:@"jsbundle"];
        //#endif

        NSLog( @"AGKRN: Create RCTRootView" );
        RCTRootView *rootView =
          [[RCTRootView alloc] initWithBundleURL: jsCodeLocation
                                      moduleName: @"DrivingLessons"
                               initialProperties: @{}
                                   launchOptions: nil];
        
        UIViewController* rootViewController = [[NativeViewController alloc] init];
        rootViewController.view = rootView;
        [rootViewController dismissViewControllerAnimated:YES completion:nil];
        [[UIApplication sharedApplication].keyWindow.rootViewController showViewController:(UIViewController*)rootViewController sender:nil];
    }
    else if ( strcmp( command, "returntoprev" ) == 0 )
    {
        // go back to AGK side :)
        // find return code in NativeVIewController
        //UIStoryboard* storyboard = [UIStoryboard storyboardWithName:@"MainStoryboard" bundle:nil];
        //UIViewController* viewController = [storyboard instantiateViewControllerWithIdentifier:@"AGKViewController"];
        //[viewController dismissViewControllerAnimated:YES completion:nil];
        //[[UIApplication sharedApplication].keyWindow.rootViewController showViewController:(UIViewController*)viewController sender:nil];
    }
    else
    {
        pSDK->ExternalCommand( command, str1, str2 );
    }
}

extern "C" int ExternExternalCommandInt( const char* sdk, const char* command, const char* str1, const char* str2 )
{
    ExternalSDK* pSDK = GetExternalSDK( sdk );
    if ( !pSDK ) return 0;
    
    return pSDK->ExternalCommandInt( command, str1, str2 );
}

extern "C" float ExternExternalCommandFloat( const char* sdk, const char* command, const char* str1, const char* str2 )
{
    ExternalSDK* pSDK = GetExternalSDK( sdk );
    if ( !pSDK ) return 0;
    
    return pSDK->ExternalCommandFloat( command, str1, str2 );
}

extern "C" char* ExternExternalCommandString( const char* sdk, const char* command, const char* str1, const char* str2 )
{
    ExternalSDK* pSDK = GetExternalSDK( sdk );
    if ( !pSDK )
    {
        char* str = new char[ 1 ];
        *str = 0;
        return str;
    }
    
    return pSDK->ExternalCommandString( command, str1, str2 );
}

