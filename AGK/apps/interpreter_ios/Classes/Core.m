// Common Includes
#import "Core.h"
#import "EAGLView.h"
#include "interpreter.h"
#include "MediaPlayer/MediaPlayer.h"
#import <UserNotifications/UNNotificationResponse.h>
#import <UserNotifications/UNNotification.h>
#import <UserNotifications/UNNotificationRequest.h>
#import <UserNotifications/UNNotificationContent.h>

// Platform specific
void Output ( char const* pdasds )
{
}

void PlatformAppQuit()
{
	exit(0);
}

using namespace AGK;
@implementation iphone_appAppDelegate
@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Tell the UIDevice to send notifications when the orientation changes
	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(orientationChanged:) 
												 name:@"UIDeviceOrientationDidChangeNotification" 
											   object:nil];
	
	//[ window addSubview: viewController.view ];
    [window setRootViewController:viewController];
	[ window makeKeyAndVisible ];
    
	// call app begin
	try
	{
		App.Begin();
	}
	catch(...)
	{
		uString err; agk::GetLastError(err);
		err.Prepend( "Error: " );
		agk::Message( err.GetStr() );
		PlatformAppQuit();
		return YES;
	}
	[viewController setActive];
    
    UILocalNotification *localNotify = launchOptions[UIApplicationLaunchOptionsLocalNotificationKey];
    if ( localNotify )
    {
        NSDictionary *infoDict = localNotify.userInfo;
        if ( infoDict )
        {
            NSDictionary *aps = [infoDict objectForKey:@"aps"];
            if ( aps )
            {
                NSString *deeplink = [aps objectForKey:@"deeplink"];
                if ( deeplink ) agk::HandleDeepLink( [deeplink UTF8String] );
            }
        }
    }

	NSDictionary *remoteNotify = launchOptions[UIApplicationLaunchOptionsRemoteNotificationKey];
    if ( remoteNotify )
    {
		NSString *deeplink = [remoteNotify objectForKey:@"deeplink"];
        if ( deeplink ) agk::HandleDeepLink( [deeplink UTF8String] );
	}
	 
    // Add to manage notification-related behaviors on iOS 10
    [UNUserNotificationCenter currentNotificationCenter].delegate = self;
    
    // success
    return YES;
}

//Called when a notification is delivered to a foreground app.
-(void)userNotificationCenter:(UNUserNotificationCenter *)center willPresentNotification:(UNNotification *)notification withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler
{
    //NSLog(@"User Info : %@",notification.request.content.userInfo);
    completionHandler(UNAuthorizationOptionSound | UNAuthorizationOptionAlert | UNAuthorizationOptionBadge);
}

//Called to let your app know which action was selected by the user for a given notification.
-(void)userNotificationCenter:(UNUserNotificationCenter *)center didReceiveNotificationResponse:(UNNotificationResponse *)response withCompletionHandler:(void(^)())completionHandler
{
    //NSLog(@"User Info : %@",response.notification.request.content.userInfo);
	
	NSString *deeplink = [response.notification.request.content.userInfo objectForKey:@"deeplink"];
    if ( deeplink )
	{
        agk::HandleDeepLink( [deeplink UTF8String] );
		completionHandler();
		return;
	}
	
	// get the whole string from the notification
    NSDictionary *aps = [response.notification.request.content.userInfo objectForKey:@"aps"];
    if ( aps )
    {
        NSString *deeplink = [aps objectForKey:@"deeplink"];
        if ( deeplink ) agk::HandleDeepLink( [deeplink UTF8String] );
    }
    
	completionHandler();
}

- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
    return UIInterfaceOrientationMaskAll;
}

- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings {
    if ( (notificationSettings.types & UIRemoteNotificationTypeAlert) != 0 )
    {
        [application registerForRemoteNotifications];
    }
}

- (void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken
{
    const unsigned char *dataBuffer = (const unsigned char *)[deviceToken bytes];
    
    NSUInteger          dataLength  = [deviceToken length];
    NSMutableString     *hexString  = [NSMutableString stringWithCapacity:(dataLength * 2)];
    
    for (int i = 0; i < dataLength; ++i)
        [hexString appendString:[NSString stringWithFormat:@"%02x", (unsigned int)dataBuffer[i]]];
    
    agk::PNToken( [hexString UTF8String] );
}

- (void)application:(UIApplication*)application didFailToRegisterForRemoteNotificationsWithError:(NSError*)error
{
    agk::PNToken( "Error" );
	NSLog(@"Failed to get token, error: %@", error);
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	[UIApplication sharedApplication].applicationIconBadgeNumber=0;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [viewController setAppActive:0];
    [viewController setInactive];
    agk::AppPausing();
#ifndef USE_METAL
    glFinish();
#endif
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    agk::AppResuming();
    agk::Resumed();
    [viewController setAppActive:1];
    [viewController setActive];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // end the app
    [viewController setInactive];
    App.End();
    agk::CleanUp();
}

- (void)dealloc
{
    [viewController release];
    [window release];
    [super dealloc];
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
	agk::HandleMusicEvents( NULL );
}

- (void)orientationChanged:(NSNotification *)notification
{
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	float angle = 0;
	
	int mode = 1;
	switch( orientation )
	{
		case UIDeviceOrientationPortrait: mode=1; break; 
		case UIDeviceOrientationPortraitUpsideDown: mode=2; angle = 3.1415927f; break; 
		case UIDeviceOrientationLandscapeLeft: mode=3; angle = 1.5707963f; break;
		case UIDeviceOrientationLandscapeRight: mode=4; angle = -1.5707963f; break; 
		default: return;
	}
	
	agk::OrientationChanged( mode );
}

@end


@implementation iphone_appAppDelegate ( Facebook )

- ( BOOL ) application: ( UIApplication* ) application handleOpenURL: ( NSURL* ) url 
{
    // called by Facebook when returning back to our application after signing in,
    // this version is called by OS 4.2 and previous
    
    // this also handles user URL schemes, but the command name is from before that
    return agk::FacebookHandleOpenURL(url) > 0;
}

- ( BOOL ) application: ( UIApplication* ) application openURL: ( NSURL* ) url sourceApplication: ( NSString* ) sourceApplication annotation: ( id ) annotation 
{
    // same as above but for OS 4.3 and later
    
    // this also handles user URL schemes, but the command name is from before that
    return agk::FacebookHandleOpenURL(url) > 0;
}

- ( BOOL ) application: ( UIApplication* ) application continueUserActivity:(NSUserActivity*) userActivity restorationHandler:(void (^)(NSArray<id<UIUserActivityRestoring>> *restorableObjects)) restorationHandler
{
    return agk::FacebookHandleOpenURL(userActivity.webpageURL) > 0;
}

- ( void ) application: ( UIApplication* ) application didReceiveLocalNotification: ( UILocalNotification* ) notification 
{
    // called when the app receives a local notification
    [[UIApplication sharedApplication] cancelLocalNotification:notification];
    
    // reset the icon badge to 0
	application.applicationIconBadgeNumber = 0; 
}



@end

// use this if you want to remove all uses of the IDFA
// @implementation ASIdentifierManager : NSObject @end

/*
// use this to get rid of the Ad Tracking consent form
@implementation UMPConsentForm : NSObject @end
@implementation UMPConsentInformation : NSObject @end
@implementation UMPRequestParameters : NSObject @end
*/

// use this if you want to remove the AdMob SDK (remove libGoogleAdMobAds.a and AdSupport.framework)
/*
 @implementation GADBannerView : UIView @end
 @implementation GADRequest : NSObject @end
 @implementation GADInterstitialAd : NSObject @end
 @implementation GADAdSize : NSObject @end
 @implementation GADExtras : NSObject @end
 @implementation GADMobileAds : NSObject @end
 GADAdSize const *kGADAdSizeBanner;
 GADAdSize const *kGADAdSizeLargeBanner;
 GADAdSize const *kGADAdSizeMediumRectangle;
 GADAdSize const *kGADAdSizeFullBanner;
 GADAdSize const *kGADAdSizeLeaderboard;
 GADAdSize const *kGADAdSizeSmartBannerPortrait;
 GADAdSize const *kGADAdSizeSmartBannerLandscape;
 GADAdSize const *kGADAdSizeFluid;
 @implementation GADRewardedAd : NSObject @end
 @implementation PACConsentForm : NSObject @end
 @implementation PACConsentInformation : NSObject @end
 */

// use this if you want to remove the Chartboost framework (also remove AdSupport.framework)
/*
 @implementation Chartboost : NSObject @end
 @implementation CHBGDPRDataUseConsent: NSObject @end
 @implementation CHBCCPADataUseConsent: NSObject @end
 extern "C" NSString* const CHBPrivacyStandardGDPR = 0;
 extern "C" NSString* const CHBPrivacyStandardCCPA = 0;
 NSString * CBLocationGameScreen;
 */

// use this if you want to remove the Firebase frameworks
/*
@implementation FIRApp : NSObject @end
@implementation FIRAnalytics : NSObject @end
*/

/*
// use this if you want to remove the SnapChat commands
@implementation SCSDKPhotoSnapContent : NSObject @end
@implementation SCSDKSnapPhoto : NSObject @end
*/

