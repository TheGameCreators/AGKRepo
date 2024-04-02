

// import all of the files we need    
#import "StoreManager.h"
#import "SocialPlugins.h"
#import <vector>
#include <algorithm>
#import "AGK.h"

#import <GoogleMobileAds/GoogleMobileAds.h>

//#import <Chartboost/Chartboost.h>
//#import "Chartboost.framework/Versions/A/Headers/Chartboost.h"

#import <AdSupport/ASIdentifierManager.h>
#include <CommonCrypto/CommonDigest.h>

#import <UserMessagingPlatform/UMPConsentForm.h>

// a few defines
#define DEG 0.0174532925
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * 3.14)

namespace AGK
{
    extern UIViewController *g_pViewController;
}

// our globals, maybe these should go in a namespace
struct sFriend
{
    char szName [ MAX_PATH ];
    char szID   [ MAX_PATH ];
};

std::vector < sFriend* >    g_FriendsList;
int                         g_iFriendsState                             = 0;
int                         g_iFacebookHTTP                             = 0;
int                         g_iFacebookDownloadState                    = 0;
SocialPlugins*              g_pSocialPlugins                            = nil;
NSString*                   g_nsFacebookID                              = nil;
int                         g_iNotification                             = 0;
char                        g_szFacebookFileDestination [ MAX_PATH ]    = "";
char                        g_szNotificationData        [ MAX_PATH ]    = "";
int                         g_iFacebookLoginState                       = 0;
bool bError = false;

uString m_sFBUserID;
uString m_sFBName;

int g_iAdMobInitialized = 0;
int g_iAdMobTesting = 0;
int g_iAdMobConsentStatus = -2; //-2=initial value, -1=loading, 0=unknown, 1=non-personalised, 2=personalised
int g_iChartboostConsentStatus = -2; //-2=initial value, -1=loading, 0=unknown, 1=non-personalised, 2=personalised
int g_iAppTargetsChildren = 0;

@interface InterstitialListener : NSObject <GADFullScreenContentDelegate>
{
@public
    int loading;
    uString sKey;
    GADInterstitialAd* pInterstitial;
}

- (void)show;
- (void)load;
- (void)reset;

@end

@implementation InterstitialListener
-(id)init
{
    NSLog( @"Init Interstitial" );
    self = [super init];
    loading = 0;
    pInterstitial = nil;
    return self;
}

- (void) reset
{
    if ( pInterstitial ) [pInterstitial release];
    pInterstitial = nil;
}

- (void)show
{
    if ( !pInterstitial )
    {
        [self load];
    }
    else
    {
        NSLog( @"Showing Interstitial" );
        
        UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
        [ viewController setInactive ];
        
        [pInterstitial presentFromRootViewController:viewController];
    }
}

- (void)load
{
    if ( loading != 0 ) return;
    if ( pInterstitial ) return;
    loading = 1;
    
    NSLog( @"Loading Interstitial" );
    
    GADRequest *request = [GADRequest request];
    
    NSString* adID = [NSString stringWithUTF8String: sKey.GetStr()];
    if ( g_iAdMobTesting ) adID = @"ca-app-pub-3940256099942544/4411468910";
    
    [GADInterstitialAd loadWithAdUnitID:adID request:request
                        completionHandler:^(GADInterstitialAd *ad, NSError *error)
        {
            loading = 0;
            if (error) {
                NSLog(@"Failed to load interstitial ad with error: %@", [error localizedDescription]);
                return;
            }
            NSLog( @"Loaded Interstitial" );
            pInterstitial = ad;
            pInterstitial.fullScreenContentDelegate = self;
            [ pInterstitial retain ];
        }
    ];
}

- (void)ad:(nonnull id<GADFullScreenPresentingAd>)ad
didFailToPresentFullScreenContentWithError:(nonnull NSError *)error
{
    NSLog(@"AdMob interstitial failed to show: %@", [error localizedDescription]);
    [self reset];
}

- (void)adDidPresentFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad
{
    NSLog( @"Presented Interstitial" );
}

- (void)adDidDismissFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad
{
    UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    [ viewController setActive ];
    
    NSLog( @"Dismissed Interstitial" );
    [self reset];
    [self load];
}
@end

InterstitialListener *g_pInterstitialListener = nil;

// reward ads
@interface AdMobRewardListener : NSObject <GADFullScreenContentDelegate>
{
@public
    int loading;
    int rewarded;
    int rewardValue;
    uString sKey;
    GADRewardedAd* pRewardAd;
}

- (void)show;
- (void)load;
- (void)reset;

@end

@implementation AdMobRewardListener
-(id)init
{
    self = [super init];
    loading = 0;
    rewarded = 0;
    rewardValue = 0;
    pRewardAd = 0;
    return self;
}

-(void) reset
{
    if ( pRewardAd ) [pRewardAd release];
    pRewardAd = nil;
}

- (void)show
{
    if ( pRewardAd == 0 )
    {
        [self load];
    }
    else
    {
        UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
        [ viewController setInactive ];
        
        [ pRewardAd presentFromRootViewController:viewController
            userDidEarnRewardHandler:^
            {
                GADAdReward *reward = pRewardAd.adReward;
                rewardValue = [reward.amount intValue];
                rewarded = 1;
            }
        ];

    }
}

- (void)load
{
    if ( loading != 0 ) return;
    if ( pRewardAd ) return;
    loading = 1;
    
    GADRequest *request = [GADRequest request];
    
    NSString* adID = [NSString stringWithUTF8String: sKey.GetStr()];
    if ( g_iAdMobTesting ) adID = @"ca-app-pub-3940256099942544/1712485313";
    
    [GADRewardedAd loadWithAdUnitID:adID request:request
                        completionHandler:^(GADRewardedAd *ad, NSError *error)
        {
            loading = 0;
            if (error) {
                NSLog(@"Failed to load reward ad with error: %@", [error localizedDescription]);
                return;
            }
            pRewardAd = ad;
            pRewardAd.fullScreenContentDelegate = self;
            [pRewardAd retain];
        }
    ];
}

- (void)ad:(nonnull id<GADFullScreenPresentingAd>)ad
didFailToPresentFullScreenContentWithError:(nonnull NSError *)error
{
    NSLog(@"AdMob interstitial failed to show: %@", [error localizedDescription]);
    [self reset];
}

- (void)adDidPresentFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad
{

}

- (void)adDidDismissFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad
{
    UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    [ viewController setActive ];
    
    [self reset];
    [self load];
}
@end

AdMobRewardListener* g_pAdMobRewardListener = nil;

// chartboost listener
#ifdef CHARTBOOST
#ifndef LITEVERSION
@interface ChartboostListener : NSObject <ChartboostDelegate>
{
@public
    int loading;
    int loaded;
    int loadingReward;
    int loadedReward;
    int rewarded;
}

- (void) reset;
- (void) show;
- (void) load;
- (void) showReward;
- (void) loadReward;

@end

@implementation ChartboostListener
-(id)init
{
    self = [super init];
    loading = 0;
    loaded = 0;
    loadingReward = 0;
    loadedReward = 0;
    rewarded = 0;
    return self;
}

- (void) reset
{
    loading = 0;
    loaded = 0;
    loadingReward = 0;
    loadedReward = 0;
    rewarded = 0;
}

- (void)show
{
    if ( loaded == 0 )
    {
        if ( loading == 0 )
        {
            [self load];
        }
    }
    else
    {
        loaded = 0;
        loading = 0;
        
        UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
        [ viewController setInactive ];
        
        [Chartboost showInterstitial:CBLocationGameScreen];
    }
}

- (void)load
{
    if ( loaded == 0 && loading == 0 )
    {
        loading = 1;
        [Chartboost cacheInterstitial:CBLocationGameScreen];
    }
}

- (void)didCacheInterstitial:(CBLocation)location
{
    loading = 0;
    loaded = 1;
}

- (void)didFailToLoadInterstitial:(CBLocation)location withError:(CBLoadError)error
{
    loading = 0;
    loaded = 0;
    NSLog(@"Failed to load ad, error %d", (unsigned int) error);
}

- (void)didDismissInterstitial:(CBLocation)location
{
    UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    [ viewController setActive ];
    
    [self load];
}

- (void)didCloseInterstitial:(CBLocation)location
{
    UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    [ viewController setActive ];
}

// reward videos
- (void)showReward
{
    rewarded = 0;
    
    if ( loadedReward == 0 )
    {
        if ( loadingReward == 0 )
        {
            [self loadReward];
        }
    }
    else
    {
        loadedReward = 0;
        loadingReward = 0;
        
        UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
        [ viewController setInactive ];
        
        [Chartboost showRewardedVideo:CBLocationGameScreen];
    }
}

- (void)loadReward
{
    if ( loadedReward == 0 && loadingReward == 0 )
    {
        loadingReward = 1;
        [Chartboost cacheRewardedVideo:CBLocationGameScreen];
    }
}

- (void)didCacheRewardedVideo:(CBLocation)location
{
    loadingReward = 0;
    loadedReward = 1;
}

- (void)didFailToLoadRewardedVideo:(CBLocation)location withError:(CBLoadError)error
{
    loadingReward = 0;
    loadedReward = 0;
    NSLog(@"Failed to load reward ad, error %d", (unsigned int) error);
}

- (void)didDismissRewardedVideo:(CBLocation)location
{
    UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    [ viewController setActive ];
    
    [self loadReward];
}

- (void)didCloseRewardedVideo:(CBLocation)location
{
    UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    [ viewController setActive ];
}

-(void)didCompleteRewardedVideo:(CBLocation)location withReward:(int)reward
{
    rewarded = 1;
}
@end

ChartboostListener *g_pChartboostListener = nil;
#endif
#endif

@implementation SocialPlugins

#ifndef LITEVERSION

#ifdef USE_FACEBOOK_SDK
void sessionStateChanged ( FBSession* session, FBSessionState state, NSError* error )
{
    //[ FBSession openActiveSessionWithAllowLoginUI: YES ];
    
    switch ( state )
    {
        case FBSessionStateOpen:
        {
            
            
            
            // get info about the user logged in
            [ [ [ FBRequest alloc ] initWithSession:FBSession.activeSession graphPath: @"me" ] startWithCompletionHandler: ^( FBRequestConnection* connection, NSDictionary* result, NSError* error )
             {
                 if ( !error )
                 {
                     
                     if ( [ result isKindOfClass: [ NSDictionary class ] ] )
                     {
                         //NSLog ( @"Birthday: %@", [ result objectForKey: @"birthday" ] );
                         //NSLog ( @"Name:     %@", [ result objectForKey: @"name"     ] );
                         //NSLog ( @"ID:       %@", [ result objectForKey: @"id"       ] );
                         
                         NSString* nsID   = [ result objectForKey: @"id"   ];
                         NSString* nsName = [ result objectForKey: @"name"   ];
                         
                         m_sFBUserID.SetStr( [ nsID UTF8String ] );
                         m_sFBName.SetStr( [ nsName UTF8String ] );
                         
                         // copy details
                         char szID [ 256 ];
                         strcpy ( szID, [ nsID UTF8String ] );
                         
                         NSData *sanitizedData = [nsName dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
                         // Corrected back-conversion from NSData to NSString
                         NSString *sanitizedText = [[[NSString alloc] initWithData:sanitizedData encoding:NSASCIIStringEncoding] autorelease];
                         
                         char szName [ 256 ];
                         strcpy ( szName, [ sanitizedText UTF8String ] );
                         
                         // write this out to a file using AGK commands
                         int iFile = agk::OpenToWrite ( "facebook_id.txt", 0 );
                         
                         agk::WriteString ( iFile, szID );
                         agk::WriteString ( iFile, szName );
                         
                         agk::CloseFile ( iFile );
                         
                         g_iFacebookLoginState = 1;
                     }
                     bError = false;
                 }
                 else
                 {
                     bError = true;
                     g_iFacebookLoginState = -1;
                     [FBSession.activeSession closeAndClearTokenInformation ];
                 }
             } ];
        }
        break;
            
        case FBSessionStateClosed:
        {
            if ( !bError ) g_iFacebookLoginState = 0;
            
            [FBSession.activeSession closeAndClearTokenInformation ];
        }
        break;
            
        case FBSessionStateClosedLoginFailed:
        {
            bError = true;
            g_iFacebookLoginState = -1;
            
            [FBSession.activeSession closeAndClearTokenInformation ];
        }
        break;
            
        default:
        {
            bError = true;
            g_iFacebookLoginState = -1;
        }
        break;
    }
    
}

- ( void ) facebookSetup: ( NSString* ) nsID;
{
    [FBSDKSettings setAutoInitEnabled: YES ];
    [FBSDKApplicationDelegate initializeSDK:nil];
    
    g_iFacebookHTTP = agk::CreateHTTPConnection ( );
    agk::SetHTTPHost ( g_iFacebookHTTP, "graph.facebook.com", 0 );
    
    [FBSession setDefaultAppID:nsID];
}
#endif
#endif

- ( void ) dealloc
{
    [ super dealloc ];
}

@end

#pragma mark -
#pragma mark Internal AGK Commands

namespace AGK {
    extern uString g_sLastURLSchemeText;
}

int agk::FacebookHandleOpenURL( void* url )
{
    NSString *text = [((NSURL*)url) absoluteString];
    AGK::g_sLastURLSchemeText.SetStr( [text UTF8String] );
#if !defined(LITEVERSION) && defined(USE_FACEBOOK_SDK)
    if ( AGK::g_sLastURLSchemeText.CompareCaseToN("fb", 2) == 0 )
    {
        if ( FBSession.activeSession ) return [ FBSession.activeSession handleOpenURL: (NSURL*)url ] ? 1 : 0;
    }
#endif
    return 1;
}

void agk::HandleDeepLink( const char* link )
{
    AGK::g_sLastURLSchemeText.SetStr( link );
}

void agk::PlatformSocialPluginsSetup ( void )
{
    // this must be called to initialise the interface
    
    g_pSocialPlugins = [ SocialPlugins alloc ];
    
    g_pSocialPlugins->bannerView_ = nil;
}

void agk::PlatformSocialPluginsDestroy ( void )
{
    // take everything down
    
    if ( !g_pSocialPlugins )
        return;
    
    //if ( g_pSocialPlugins->likeButton )
    {
        //[ g_pSocialPlugins->likeButton removeFromSuperview ];
        //[ g_pSocialPlugins->likeButton release ];
    }
    
    if ( g_pSocialPlugins->bannerView_ )
        [ g_pSocialPlugins->bannerView_ release ];
    
#ifndef LITEVERSION
    //if ( g_pSocialPlugins->twitter )
    //    [ g_pSocialPlugins->twitter release ];
#endif
    
    [ g_pSocialPlugins release ];
    g_pSocialPlugins = nil;
    
    for ( int i = 0; i < g_FriendsList.size ( ); i++ )
    {
        delete g_FriendsList [ i ];
    }
    
    g_FriendsList.clear ( );
}

#pragma mark -
#pragma mark Rating Commands

void agk::PlatformRateApp( const char* szID, const char* title, const char* message )
{
    if ( !szID || strlen(szID) == 0 ) return;

    // convert the string into an unsigned integer
    NSUInteger ID = [ [ NSString stringWithUTF8String: szID ] intValue ];
    
    // store the ID in the rating interface
    [ iRate sharedInstance ].appStoreID = ID;
    if ( title ) [ iRate sharedInstance ].messageTitle = [NSString stringWithCString:title encoding:NSWindowsCP1252StringEncoding];
	if ( message ) [ iRate sharedInstance ].message = [NSString stringWithCString:message encoding:NSWindowsCP1252StringEncoding];

    // now prompt for a rating
    [ [ iRate sharedInstance ] promptForRating ];
}

#pragma mark -
#pragma mark In App Purchase Commands

void agk::PlatformInAppPurchaseReset()
{
    [ [StoreManager sharedManager] reset ];
}

void agk::PlatformInAppPurchaseSetKeys ( const char* szData1, const char* szData2 )
{
    // do nothing on iOS
}

void agk::PlatformInAppPurchaseSetTitle ( const char* szTitle )
{
    
    // set the title
    [ [StoreManager sharedManager] setTitle: szTitle ];
}

void agk::PlatformInAppPurchaseAddProductID ( const char* szID, int type )
{    
    // add the product ID
    [ [StoreManager sharedManager] addProductID: szID ];
}

void agk::PlatformInAppPurchaseSetup ( void )
{    
    // final set up, must be called after setting title and adding product IDs
    [ [StoreManager sharedManager] setup ];
}

int agk::PlatformGetInAppPurchaseAvailable ( int iID )
{
    
    // find out if content is available
    if ( [ [StoreManager sharedManager] isUnlockableContentAvailable: iID ] == YES )
        return 1;
    
    // not available yet
    return 0;
}

int agk::PlatformGetInAppPurchaseAvailable2 ( int iID )
{
    return [ [StoreManager sharedManager] getContentState: iID ];
}

void agk::PlatformInAppPurchaseActivate ( int iID )
{    
    [ [ StoreManager sharedManager ] purchaseUnlockableContent: iID ];
}

void agk::PlatformInAppPurchaseActivateWithPlan( int iID, const char* planToken )
{    
    // todo
    [ [ StoreManager sharedManager ] purchaseUnlockableContent: iID ];
}

int agk::PlatformGetInAppPurchaseState ( void )
{
    return [ [StoreManager sharedManager] getState ];
}

char* agk::PlatformGetInAppPurchaseLocalPrice ( int iID )
{
    return [ [ StoreManager sharedManager ] getLocalPrice: iID ];
}

char* agk::PlatformGetInAppPurchaseDescription ( int iID )
{
    if ( iID < 0 )
    {
        char* str = new char[1];
        *str = 0;
        return str;
    }
    
    return [ [ StoreManager sharedManager ] getDescription: iID ];
}

bool agk::PlatformHasInAppPurchase ( void )
{
    return true;
}

void agk::PlatformInAppPurchaseRestore()
{
	[ [ StoreManager sharedManager ] restore ];
}

char* agk::PlatformGetInAppPurchaseSignature( int iID )
{
	return [[StoreManager sharedManager] getSignature: iID];
}

void agk::PlatformInAppPurchaseResetPurchase( const char* token )
{
    [[StoreManager sharedManager] resetPurchase: token];
}

void agk::PlatformInAppPurchaseRedeemOffer()
{
    if (@available(iOS 14.0, *))
    {
        [[SKPaymentQueue defaultQueue] presentCodeRedemptionSheet];
    }
}

char* agk::PlatformGetInAppPurchaseToken( int iID )
{
    return [[StoreManager sharedManager] getToken: iID];
}

int agk::PlatformGetInAppPurchaseSubNumPlans( int iID )
{
    return [[StoreManager sharedManager] getNumPlans:iID];
}

int agk::PlatformGetInAppPurchaseSubPlanNumPeriods( int iID, int planIndex )
{
    return [[StoreManager sharedManager] getPlanNumPeriods: iID plan:planIndex];
}

char* agk::PlatformGetInAppPurchaseSubPlanPrice( int iID, int planIndex, int periodIndex )
{
    return [[StoreManager sharedManager] getPlanPrice:iID plan:planIndex period:periodIndex];
}

int agk::PlatformGetInAppPurchaseSubPlanDuration( int iID, int planIndex, int periodIndex )
{
    return [[StoreManager sharedManager] getPlanDuration:iID plan:planIndex period:periodIndex];
}

char* agk::PlatformGetInAppPurchaseSubPlanDurationUnit( int iID, int planIndex, int periodIndex )
{
    return [[StoreManager sharedManager] getPlanDurationUnit:iID plan:planIndex period:periodIndex];
}

int agk::PlatformGetInAppPurchaseSubPlanPaymentType( int iID, int planIndex, int periodIndex )
{
    return [[StoreManager sharedManager] getPlanPaymentType:iID plan:planIndex period:periodIndex];
}

char* agk::PlatformGetInAppPurchaseSubPlanTags( int iID, int planIndex )
{
    return [[StoreManager sharedManager] getPlanTags:iID plan:planIndex];
}

char* agk::PlatformGetInAppPurchaseSubPlanToken( int iID, int planIndex )
{
    return [[StoreManager sharedManager] getPlanToken:iID plan:planIndex];
}

#pragma mark -
#pragma mark AdMob Commands

void UpdateChartboostConsent()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
    [Chartboost clearDataUseConsentForPrivacyStandard:CHBPrivacyStandardGDPR];
    [Chartboost clearDataUseConsentForPrivacyStandard:CHBPrivacyStandardCCPA];
    if ( g_iChartboostConsentStatus == 2 )
    {
        [Chartboost addDataUseConsent:[CHBGDPRDataUseConsent gdprConsent:CHBGDPRConsentBehavioral] ];
        [Chartboost addDataUseConsent:[CHBCCPADataUseConsent ccpaConsent:CHBCCPAConsentOptInSale] ];
    }
    else
    {
        [Chartboost addDataUseConsent:[CHBGDPRDataUseConsent gdprConsent:CHBGDPRConsentBehavioral] ];
        [Chartboost addDataUseConsent:[CHBCCPADataUseConsent ccpaConsent:CHBCCPAConsentOptInSale] ];
    }
    
    if ( g_pChartboostListener )
    {
        g_pChartboostListener->loaded = 0;
        g_pChartboostListener->loadedReward = 0;
    }
#endif
#endif
}

void UpdateConsentStatus()
{
    if (UMPConsentInformation.sharedInstance.consentStatus == UMPConsentStatusNotRequired)
    {
        g_iAdMobConsentStatus = 2;
        return;
    }
    
    if (UMPConsentInformation.sharedInstance.consentStatus == UMPConsentStatusRequired)
    {
        g_iAdMobConsentStatus = 0;
        return;
    }
    
    NSInteger gdpr_applies = [[NSUserDefaults standardUserDefaults] integerForKey:@"IABTCF_gdprApplies"];
    if ( gdpr_applies == 0 )
    {
        g_iAdMobConsentStatus = 2;
        return;
    }
    
    NSString* purposes = [[NSUserDefaults standardUserDefaults] stringForKey:@"IABTCF_PurposeConsents"];
    if ( [purposes length] == 0 )
    {
        g_iAdMobConsentStatus = 0;
    }
    else if ( [purposes length] < 4 || [purposes characterAtIndex:0] == '0' || [purposes characterAtIndex:1] == '0' || [purposes characterAtIndex:2] == '0' || [purposes characterAtIndex:3] == '0' )
    {
        g_iAdMobConsentStatus = 1;
    }
    else
    {
        g_iAdMobConsentStatus = 2;
    }
    
    NSLog( @"Updated Consent" );
    
    g_iChartboostConsentStatus = g_iAdMobConsentStatus;
    UpdateChartboostConsent();
    
    if ( g_pInterstitialListener ) [g_pInterstitialListener reset];
    if ( g_pAdMobRewardListener ) [g_pAdMobRewardListener reset];
}

void agk::LoadConsentStatusAdMob( const char* szPubID, const char* privacyPolicy )
//****
{
    if ( g_iAdMobConsentStatus == -1 ) return;
    g_iAdMobConsentStatus = -1;

    UMPRequestParameters *parameters = [[UMPRequestParameters alloc] init];
    parameters.tagForUnderAgeOfConsent = g_iAppTargetsChildren ? YES : NO;

    // Request an update to the consent information.
    [UMPConsentInformation.sharedInstance requestConsentInfoUpdateWithParameters:parameters
        completionHandler:^(NSError *_Nullable error)
        {
            if (error) {
                agk::Warning( [[error localizedDescription] UTF8String] );
                g_iAdMobConsentStatus = 0;
            } else {
                UpdateConsentStatus();
            }
        }
    ];
}

int agk::GetConsentStatusAdMob()
//****
{
    return g_iAdMobConsentStatus;
}

void agk::RequestConsentAdMob()
//****
{
    if ( g_iAdMobConsentStatus == -1 ) return;
    g_iAdMobConsentStatus = -1;
    
    UMPFormStatus formStatus = UMPConsentInformation.sharedInstance.formStatus;
    if (formStatus == UMPFormStatusAvailable)
    {
        [UMPConsentForm loadWithCompletionHandler:^(UMPConsentForm *form, NSError *loadError)
            {
                if (loadError) {
                    agk::Warning( [[loadError localizedDescription] UTF8String] );
                    g_iAdMobConsentStatus = 0;
                } else {
                    [g_pViewController setInactive];
                    [form presentFromViewController: g_pViewController
                        completionHandler:^(NSError *_Nullable dismissError)
                        {
                            UpdateConsentStatus();
                            [g_pViewController setActive];
                        }
                    ];
                }
            }
        ];
    }
	else
	{
		g_iAdMobConsentStatus = 2;
	}
}

void agk::OverrideConsentAdMob( int consent )
{
    // can't override AdMob consent
}

void agk::OverrideConsentChartboost( int consent )
//****
{
#ifndef LITEVERSION
	g_iChartboostConsentStatus = 1;
    if ( consent == 2 ) g_iChartboostConsentStatus = 2;
    
    UpdateChartboostConsent();
#endif
}

void CheckAdMobInitialised()
{
    if ( !g_iAdMobInitialized )
    {
        g_iAdMobInitialized = 1;
        [[GADMobileAds sharedInstance] startWithCompletionHandler:nil];
    }
}

void agk::PlatformAdMobSetupRelative ( const char* szID, int iHorz, int iVert, float fOffsetX, float fOffsetY, int type )
{
    //****    
    
    // back out if the plugin interface is not available
    if ( !g_pSocialPlugins )
        return;
    
    CheckAdMobInitialised();
    
    // get our view controller
    UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    
    float viewWidth = viewController.view.frame.size.width;
    float viewHeight = viewController.view.frame.size.height;
    
    if ( agk::GetDeviceWidth() > agk::GetDeviceHeight() )
    {
        if ( viewWidth < viewHeight )
        {
            float temp = viewWidth;
            viewWidth = viewHeight;
            viewHeight = temp;
        }
    }
    else
    {
        if ( viewWidth > viewHeight )
        {
            float temp = viewWidth;
            viewWidth = viewHeight;
            viewHeight = temp;
        }
    }
    
    GADAdSize adSize = kGADAdSizeBanner;
    switch( type )
    {
		case 0: adSize = kGADAdSizeBanner; break;
		case 1: adSize = kGADAdSizeLargeBanner; break;
		case 2: adSize = kGADAdSizeMediumRectangle; break;
		case 3: adSize = kGADAdSizeFullBanner; break;
		case 4: adSize = kGADAdSizeLeaderboard; break;
		case 5: 
		{
			if ( agk::GetDeviceWidth() > agk::GetDeviceHeight() ) adSize = kGADAdSizeSmartBannerLandscape;
			else adSize = kGADAdSizeSmartBannerPortrait;
            break;
		}
		case 6: adSize = kGADAdSizeFluid; break;
		default: adSize = kGADAdSizeBanner; break;
    }
    
    float realWidth = adSize.size.width;
    float realHeight = adSize.size.height;
    if ( realWidth == 0 ) realWidth = 320;
    if ( realHeight == 0 ) realHeight = 50;

    if ( type == 5 )
    {
        realWidth = viewWidth;
        if ( agk::GetDeviceWidth() > agk::GetDeviceHeight() ) realHeight = 32;
        else realHeight = 50;
    }
	
    int iWidth = realWidth * agk::GetDeviceWidth() / viewWidth;
    int iHeight = realHeight * agk::GetDeviceHeight() / viewHeight;
    
    int iX = agk::ScreenToDeviceX(fOffsetX) * agk::GetDeviceWidth() / viewWidth;
    int iY = agk::ScreenToDeviceY(fOffsetY) * agk::GetDeviceHeight() / viewHeight;
    
    if ( iHorz == 1 ) iX = (agk::GetDeviceWidth() - iWidth) / 2;
    else if ( iHorz == 2 ) iX = agk::GetDeviceWidth() - iWidth - iX;
    
    if ( iVert == 1 ) iY = (agk::GetDeviceHeight() - iHeight) / 2;
    else if ( iVert == 2 ) iY = agk::GetDeviceHeight() - iHeight - iY;
    
    iX *= viewWidth / agk::GetDeviceWidth();
    iY *= viewHeight / agk::GetDeviceHeight();
    
    iWidth *= viewWidth / agk::GetDeviceWidth();
    iHeight *= viewHeight / agk::GetDeviceHeight();
    
    CGPoint pos = CGPointMake( iX, iY );
    
    // create the ad
    g_pSocialPlugins->bannerView_ = [ [ GADBannerView alloc ] initWithAdSize:adSize origin:pos ];
    g_pSocialPlugins->bannerView_.adUnitID = [ NSString stringWithUTF8String: szID ];
    g_pSocialPlugins->bannerView_.rootViewController = viewController;
    
    // rotate the ad if needed
    //CGAffineTransform cgCTM = CGAffineTransformMakeRotation ( DEGREES_TO_RADIANS ( iAngle ) );
    //g_pSocialPlugins->bannerView_.transform = cgCTM;
    
    // add it into the view
    [ [ viewController view ] addSubview: g_pSocialPlugins->bannerView_ ];
    
    GADRequest* p = [ GADRequest request ];
    
    if ( g_iAdMobConsentStatus != 2 )
    {
        GADExtras *extras = [[GADExtras alloc] init];
        extras.additionalParameters = @{@"npa": @"1"};
        [p registerAdNetworkExtras:extras];
    }
    
    //NSLog( @"View: %f, %f", g_pSocialPlugins->bannerView_.frame.size.width, g_pSocialPlugins->bannerView_.frame.size.height );
    
    // finally request an ad
    [ g_pSocialPlugins->bannerView_ loadRequest: p ];
}

void  agk::PlatformAdMobFullscreen ()
{
    CheckAdMobInitialised();
    
    if ( !g_pInterstitialListener )
    {
        g_pInterstitialListener = [[InterstitialListener alloc] init];
        g_pInterstitialListener->sKey.SetStr( m_sAdMobCode );
    }
    
    [g_pInterstitialListener show];
}

void  agk::PlatformAdMobCacheFullscreen ()
{
    CheckAdMobInitialised();
    
    if ( !g_pInterstitialListener )
    {
        g_pInterstitialListener = [[InterstitialListener alloc] init];
        g_pInterstitialListener->sKey.SetStr( m_sAdMobCode );
    }
    
    [g_pInterstitialListener load];
}

void agk::PlatformAdMobPosition( int horz, int vert, float offsetx, float offsety )
{
    // return if the plugin interface is not available
    if ( !g_pSocialPlugins )
        return;
    
    // make sure we have an advert
    if ( !g_pSocialPlugins->bannerView_ )
        return;
    
    UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    
    float viewWidth = viewController.view.frame.size.width;
    float viewHeight = viewController.view.frame.size.height;
    
    if ( agk::GetDeviceWidth() > agk::GetDeviceHeight() )
    {
        if ( viewWidth < viewHeight )
        {
            float temp = viewWidth;
            viewWidth = viewHeight;
            viewHeight = temp;
        }
    }
    else
    {
        if ( viewWidth > viewHeight )
        {
            float temp = viewWidth;
            viewWidth = viewHeight;
            viewHeight = temp;
        }
    }
    
    int iWidth = 320 * agk::GetDeviceWidth() / viewWidth;
    int iHeight = 50 * agk::GetDeviceHeight() / viewHeight;
    //if ( [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad )
    //{
    //    iWidth = agk::GetDeviceWidth();
    //    iHeight = 90 * agk::GetDeviceHeight() / viewHeight;
    //}
    
    int iX = agk::ScreenToDeviceX(offsetx) * agk::GetDeviceWidth() / viewWidth;
    int iY = agk::ScreenToDeviceY(offsety) * agk::GetDeviceHeight() / viewHeight;
    
    if ( horz == 1 ) iX = (agk::GetDeviceWidth() - iWidth) / 2;
    else if ( horz == 2 ) iX = agk::GetDeviceWidth() - iWidth - iX;
    
    if ( vert == 1 ) iY = (agk::GetDeviceHeight() - iHeight) / 2;
    else if ( vert == 2 ) iY = agk::GetDeviceHeight() - iHeight - iY;
    
    iX *= viewWidth / agk::GetDeviceWidth();
    iY *= viewHeight / agk::GetDeviceHeight();
    
    iWidth *= viewWidth / agk::GetDeviceWidth();
    iHeight *= viewHeight / agk::GetDeviceHeight();
    
    [ g_pSocialPlugins->bannerView_ setFrame:CGRectMake(iX, iY, iWidth, iHeight) ];
}

//****f* Extras/Adverts/SetAdMobVisible
// FUNCTION
//   Set the visibility of any AdMob advert.
// INPUTS
//   visible -- 1 will display the advert and 0 will hide it.
// SOURCE
void agk::PlatformSetAdMobVisible ( int iVisible )
{
//****    
    
    // return if the plugin interface is not available
    if ( !g_pSocialPlugins )
        return;
    
    // make sure we have an advert
    if ( !g_pSocialPlugins->bannerView_ )
        return;
    
    // show or hide it
    if ( iVisible == 1 )
    {
        // get our view controller
        UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
        
        // add it into the view
        [ [ viewController view ] addSubview: g_pSocialPlugins->bannerView_ ];
    }
    else
    {
        // remove it from the view
        [ g_pSocialPlugins->bannerView_ removeFromSuperview ];
    }
}

//****f* Extras/Adverts/AdMobRequestNewAd
// FUNCTION
//   Call this command to request a new advert. Usually adverts will be provided automatically.
//   You may only want to do this when switching to new screens within your application
// SOURCE
void agk::PlatformAdMobRequestNewAd ( void )
{
//****
    
    // make sure our plugin is okay
    if ( !g_pSocialPlugins )
        return;
    
    // back out if there is no advert
    if ( !g_pSocialPlugins->bannerView_ )
        return;
    
    // request a new ad - only call this if you want ads served faster
    [ g_pSocialPlugins->bannerView_ loadRequest: nil ];
}

//****f* Extras/Adverts/AdMobDestroy
// FUNCTION
//   Completely remove AdMob and any displayed adverts.
// SOURCE
void agk::PlatformAdMobDestroy ( void )
{
//****    
    
    // check the plugin is okay
    if ( !g_pSocialPlugins )
        return;
    
    // make sure we have an advert
    if ( !g_pSocialPlugins->bannerView_ )
        return;

    // take it out
    [ g_pSocialPlugins->bannerView_ removeFromSuperview ];
    [ g_pSocialPlugins->bannerView_ release ];
    g_pSocialPlugins->bannerView_ = nil;
}

bool agk::PlatformHasAdMob ( void )
{
    return true;
}

int  agk::PlatformAdMobGetFullscreenLoaded ()
{
	if ( !g_pInterstitialListener ) return 0;
	
	return g_pInterstitialListener->pInterstitial ? 1 : 0;
}

void agk::PlatformAdMobRewardAd()
{
    CheckAdMobInitialised();
    
    if ( !g_pAdMobRewardListener )
    {
        g_pAdMobRewardListener = [[AdMobRewardListener alloc] init];
        g_pAdMobRewardListener->sKey.SetStr( m_sAdMobRewardAdCode );
    }
    
    g_pAdMobRewardListener->rewarded = 0;
    [g_pAdMobRewardListener show];
}

void agk::PlatformAdMobCacheRewardAd()
{
    CheckAdMobInitialised();
    
    if ( !g_pAdMobRewardListener )
    {
        g_pAdMobRewardListener = [[AdMobRewardListener alloc] init];
        g_pAdMobRewardListener->sKey.SetStr( m_sAdMobRewardAdCode );
    }
    
    [g_pAdMobRewardListener load];
}

int agk::PlatformAdMobGetRewardAdLoaded()
{
    if ( !g_pAdMobRewardListener ) return 0;
    return g_pAdMobRewardListener->pRewardAd ? 1 : 0;
}

int agk::PlatformAdMobGetRewardAdRewarded()
{
    if ( !g_pAdMobRewardListener ) return 0;
	return g_pAdMobRewardListener->rewarded;
}

int agk::PlatformAdMobGetRewardAdValue()
{
    if ( !g_pAdMobRewardListener ) return 0;
	return g_pAdMobRewardListener->rewardValue;
	return 0;
}

void agk::PlatformAdMobResetRewardAd()
{
    if ( g_pAdMobRewardListener ) g_pAdMobRewardListener->rewarded = 0;
}

void agk::PlatformAdMobSetTesting (int testing)
{
    g_iAdMobTesting = testing;
}

void agk::PlatformAdMobSetChildRating(int rating)
{
    g_iAppTargetsChildren = rating;
}

// CHARTBOOST

void  agk::PlatformChartboostSetup ()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
	if ( !g_pChartboostListener ) g_pChartboostListener = [[ChartboostListener alloc] init];
    
    UpdateChartboostConsent();
    
    [Chartboost startWithAppId:[ NSString stringWithUTF8String:m_sChartboostCode1.GetStr() ]
                  appSignature:[ NSString stringWithUTF8String:m_sChartboostCode2.GetStr() ]
                  delegate:g_pChartboostListener];
        
    [g_pChartboostListener reset];
    [g_pChartboostListener load];
#else
    agk::Message("Chartboost commands are not available");
#endif
#else
    agk::Message("Chartboost commands are not available");
#endif
}

void  agk::PlatformChartboostFullscreen ()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
    if ( !g_pChartboostListener ) return;
    
    [g_pChartboostListener show];
#endif
#endif
}

int  agk::PlatformChartboostGetFullscreenLoaded ()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
	if ( !g_pChartboostListener ) return 0;
	
	return g_pChartboostListener->loaded;
#else
    return 0;
#endif
#endif
}

void agk::PlatformChartboostRewardAd()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
    if ( !g_pChartboostListener ) return;
    
    [g_pChartboostListener showReward];
#endif
#endif
}

void agk::PlatformChartboostCacheRewardAd()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
    if ( !g_pChartboostListener ) return;
    
    [g_pChartboostListener loadReward];
#endif
#endif
}

int agk::PlatformChartboostGetRewardAdLoaded()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
    if ( !g_pChartboostListener ) return 0;
    
    return g_pChartboostListener->loadedReward;
#else
    return 0;
#endif
#endif
}

int agk::PlatformChartboostGetRewardAdRewarded()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
    if ( !g_pChartboostListener ) return 0;
    
    return g_pChartboostListener->rewarded;
#else
    return 0;
#endif
#endif
}

void agk::PlatformChartboostResetRewardAd()
{
#ifdef CHARTBOOST
#ifndef LITEVERSION
    if ( !g_pChartboostListener ) return;
    g_pChartboostListener->rewarded = 0;
#endif
#endif
}


#pragma mark -
#pragma mark Twitter Commands

//****f* Extras/Twitter/TwitterSetup
// FUNCTION
//   Set up Twitter access with your application key and secret.
// INPUTS
//   key    -- you need to supply your Twitter app ouath key.
//   secret -- you need to supply your Twitter app ouath secret.
// SOURCE
void agk::PlatformTwitterSetup ( const char* szKey, const char* szSecret )
//****
{

#ifndef LITEVERSION
    /*
    // make sure our plugin is okay
    if ( !g_pSocialPlugins )
        return;
   
    // set up the twitter engine if needed
    if ( !g_pSocialPlugins->twitter )
    {  
        g_pSocialPlugins->twitter                = [ [ SA_OAuthTwitterEngine alloc ] initOAuthWithDelegate: g_pSocialPlugins ];
        g_pSocialPlugins->twitter.consumerKey    = [ NSString stringWithUTF8String: szKey    ];  
        g_pSocialPlugins->twitter.consumerSecret = [ NSString stringWithUTF8String: szSecret ];  
    }
    
    // if the user is not logged in then sort it out
    if ( ![ g_pSocialPlugins->twitter isAuthorized ] )
    {
        // get the controller
        UIViewController* controller = [ SA_OAuthTwitterController controllerToEnterCredentialsWithTwitterEngine: g_pSocialPlugins->twitter delegate: g_pSocialPlugins ];
        
        // back out if there's a problem
        if ( !controller )
            return;
        
        // get the view controller and add our controller to it and have it displayed so we can login
        UIViewController* viewController = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
        [ viewController presentModalViewController: controller animated: YES ];
    }
    else
    {
       // agk::Message ( "LOGGED IN TO TWITTER" );
    }
     */
    agk::Message("Twitter commands are not currently available, if you want to request they are updated email paul@thegamecreators.com so we can judge the level of demand");
#else
    agk::Message("Twitter commands are not available in the lite build of AGK, please use the full version");
#endif
}

//****f* Extras/Twitter/TwitterMessage
// FUNCTION
//   Send the specified message using Twitter. If the user is not logged into Twitter
//   then this command will take care of the login process.
// INPUTS
//   message -- the message to post to Twitter
// SOURCE
void agk::PlatformTwitterMessage ( const char* szMessage )
{
//****

#ifndef LITEVERSION
    /*
    // check the plugin is okay
    if ( !g_pSocialPlugins )
        return;

    // make sure twitter is available
    if ( !g_pSocialPlugins->twitter )
        return;

    // send a message
    if ( [ g_pSocialPlugins->twitter isAuthorized ] )
        [ g_pSocialPlugins->twitter sendUpdate: [ NSString stringWithUTF8String: szMessage ] ];
    //else
      //  agk::Message ( "NOT LOGGED IN TO TWITTER" );
     */
#endif
}

bool agk::PlatformHasTwitter ( void )
{
#ifndef LITEVERSION
    return true;
#else
    return false;
#endif
}

#pragma mark -
#pragma mark Facebook Commands

//****f* Extras/Facebook/FacebookSetup
// FUNCTION
//   Set up facebook.
// INPUTS
//   ID -- your Facebook app ID.
// SOURCE
void agk::PlatformFacebookSetup ( const char* szID )
{
//****
#ifdef USE_FACEBOOK_SDK
#ifndef LITEVERSION
    // check plugin is okay
    if ( !g_pSocialPlugins )
        return;

    // set things up
    [ g_pSocialPlugins facebookSetup: [ NSString stringWithUTF8String: szID ] ];
#else
    agk::Message("Facebook is not available in the lite build of AGK, please use the full version");
#endif
#endif
}

//****f* Extras/Facebook/GetFacebookLoggedIn
// FUNCTION
//   Returns 1 if the user is logged in to Facebook and 0 if not.
// SOURCE
int agk::PlatformGetFacebookLoggedIn ( void )
{
//****
    
    // check the plugin
    if ( !g_pSocialPlugins )
        return 0;

    // is the user logged in
    //if ( [ [ g_pSocialPlugins facebook ] isSessionValid ] == YES )
        //return 1;
    
    // not logged in
    return g_iFacebookLoginState;
}

//****f* Extras/Facebook/FacebookLogin
// FUNCTION
//   Call this to start the process of logging in to Facebook. If the user
//   is already logged in nothing will happen when calling this command.
// SOURCE
void agk::PlatformFacebookLogin ( void )
{
//****
#ifdef USE_FACEBOOK_SDK
#ifndef LITEVERSION
    if ( !FBSession.activeSession.isOpen )
    {
        if ( g_iFacebookLoginState == 2 && FBSession.activeSession )
        {
            [FBSession.activeSession close ];
        }
        
        g_iFacebookLoginState = 0;
        
        
        [ FBSession openActiveSessionWithReadPermissions:@[@"public_profile",@"user_friends"] allowLoginUI:YES completionHandler: ^( FBSession* session, FBSessionState status, NSError* error )
        {
            sessionStateChanged ( session, status, error );
        } ];
    }
#endif
#endif
}

//****f* Extras/Facebook/FacebookLogout
// FUNCTION
//   Use this command to logout of Facebook.
// SOURCE
void agk::PlatformFacebookLogout ( void )
{
//****
    #ifdef USE_FACEBOOK_SDK
#ifndef LITEVERSION
    // check the plugin
    if ( !g_pSocialPlugins )
        return;
    
    if ( FBSession.activeSession.isOpen )
        [ FBSession.activeSession closeAndClearTokenInformation ];
    
    g_iFacebookLoginState = 0;

    
    // check facebook
    //if ( !g_pSocialPlugins->facebook )
        //return;

    // finally logout
    //[ [ g_pSocialPlugins facebook ] logout ];
#endif
#endif
}

//****f* Extras/Facebook/FacebookPostOnMyWall
// FUNCTION
//   Post a message on your wall. This will display a dialog where the user can enter
//   a specific message.
// INPUTS
//   link        -- link to a URL.
//   picture     -- link to a picture.
//   name        -- name to be displayed.
//   caption     -- caption.
//   description -- description of post.
// SOURCE
void agk::PlatformFacebookPostOnMyWall ( const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription )
{
//****
#ifdef USE_FACEBOOK_SDK
#ifndef LITEVERSION
    // check the plugin
    if ( !g_pSocialPlugins )
        return;
    
    
    
    
    // set up the data we need
    NSMutableDictionary* params = [
                                    NSMutableDictionary dictionaryWithObjectsAndKeys:
                                    [ NSString stringWithUTF8String: m_sFBUserID.GetStr()], @"to",
                                    [ NSString stringWithUTF8String: szLink        ], @"link",
                                    [ NSString stringWithUTF8String: szPicture     ], @"picture",
                                    [ NSString stringWithUTF8String: szName        ], @"name",
                                    [ NSString stringWithUTF8String: szCaption     ], @"caption",
                                    [ NSString stringWithUTF8String: szDescription ], @"description",
                                    nil
                                  ];
    
    // fire it over to the wall or feed
    //[ g_pSocialPlugins.facebook dialog: @"feed" andParams: params andDelegate: nil ];
    [FBWebDialogs presentFeedDialogModallyWithSession:nil parameters:params handler:nil ];
#endif
#endif
}

//****f* Extras/Facebook/FacebookPostOnFriendsWall
// FUNCTION
//   Post a message on the wall of a friend. This will display a dialog where the user can enter
//   a specific message.
// INPUTS
//   ID          -- ID of the friend.
//   link        -- link to a URL.
//   picture     -- link to a picture.
//   name        -- name to be displayed.
//   caption     -- caption.
//   description -- description of post.
// SOURCE
void agk::PlatformFacebookPostOnFriendsWall ( const char* szID, const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription )
{
//****
#ifdef USE_FACEBOOK_SDK
#ifndef LITEVERSION    
    // check the plugin
    if ( !g_pSocialPlugins )
        return;
    
    

    
    
    NSMutableDictionary *params = [
                                        NSMutableDictionary dictionaryWithObjectsAndKeys:
                                        [ NSString stringWithUTF8String: szID          ], @"to",
                                        [ NSString stringWithUTF8String: szName        ], @"name",
                                        [ NSString stringWithUTF8String: szCaption     ], @"caption",
                                        [ NSString stringWithUTF8String: szDescription ], @"description",
                                        [ NSString stringWithUTF8String: szLink        ], @"link",
                                        [ NSString stringWithUTF8String: szPicture     ], @"picture",
                                        nil
                                   ];
    

    // and now post it
    //[ g_pSocialPlugins.facebook dialog: @"feed" andParams: params andDelegate: nil ];
    [FBWebDialogs presentFeedDialogModallyWithSession:nil parameters:params handler:nil ];
#endif
#endif

}

//****f* Extras/Facebook/FacebookInviteFriend
// FUNCTION
//   Send a message to your friend inviting them to play your game.
// INPUTS
//   ID      -- ID of the friend.
//   message -- the message.
// SOURCE
void agk::PlatformFacebookInviteFriend ( const char* szID, const char* szMessage )
{
//****
    
    // check the plugin
    if ( !g_pSocialPlugins )
        return;
    
    // check facebook
    /*
    if ( !g_pSocialPlugins->facebook )
        return;
    
    // set up our data
    NSMutableDictionary* params = [ NSMutableDictionary dictionaryWithObjectsAndKeys: [ NSString stringWithUTF8String: szMessage ], @"message", [ NSString stringWithUTF8String: szID ], @"to", nil ];
    
    // send the invitation
    [ [ g_pSocialPlugins facebook ] dialog: @"apprequests" andParams: params andDelegate: g_pSocialPlugins ];
    */
}

//****f* Extras/Facebook/FacebookShowLikeButton
// FUNCTION
//   Show a like button for Facebook.
// INPUTS
//   url    -- URL that you want to like.
//   x      -- x position of like button.
//   y      -- y position of like button.
//   width  -- width of like button.
//   height -- height of like button.
// SOURCE
void agk::PlatformFacebookShowLikeButton ( const char* szURL, int iX, int iY, int iWidth, int iHeight )
{
//****
 
    /*
    // check the plugin is okay
    if ( !g_pSocialPlugins )
        return;
    
    // check facebook
    if ( !g_pSocialPlugins->facebook )
        return;
    
    // show the facebook like button, the user needs to specify url, x and y positions
    // and the dimensions of the box to be displayed
    
    // get the view controller and set up the frame
    UIViewController*   viewController  = [ [ [ UIApplication sharedApplication ] delegate ] viewController ];
    CGRect              frame           = CGRectMake ( iX, iY, iWidth, iHeight );
    
    // remove if it already exists
    if ( g_pSocialPlugins->likeButton )
        agk::FacebookDestroyLikeButton ( );

    // create our like button
    g_pSocialPlugins->likeButton = [ [ FBLikeButton alloc ] initWithFrame: frame andUrl: [ NSString stringWithUTF8String: szURL ] ];
    
    // add it into the view
    [ [ viewController view ] addSubview: g_pSocialPlugins->likeButton ];
    */
}

//****f* Extras/Facebook/FacebookDestroyLikeButton
// FUNCTION
//   Destroy the like button.
// SOURCE
void agk::PlatformFacebookDestroyLikeButton ( void )
{
//****

    /*
    // check plugins are okay
    if ( !g_pSocialPlugins )
        return;
    
    // check like button exists
    if ( !g_pSocialPlugins->likeButton )
        return;
    
    // remove it
    [ g_pSocialPlugins->likeButton removeFromSuperview ];
    [ g_pSocialPlugins->likeButton release ];
    */
}

bool CheckStrings ( sFriend* i, sFriend* j )
{
    if ( strcmp ( i->szName, j->szName ) < 0 )
        return true;
       
    return false;
}

//****f* Extras/Facebook/FacebookGetFriends
// FUNCTION
//   Get a list of friends for the user logged in.
// SOURCE
void agk::PlatformFacebookGetFriends ( void )
{
//****
#ifdef USE_FACEBOOK_SDK
#ifndef LITEVERSION    
    // set friends state to 0
    g_iFriendsState = 0;
    
    if ( FBSession.activeSession.isOpen )
    {
		g_FriendsList.clear ( );
    
        [ [ [ FBRequest alloc ] initWithSession:FBSession.activeSession graphPath: @"me/friends" ] startWithCompletionHandler: ^( FBRequestConnection* connection, NSDictionary* result, NSError* error )
        {
            //NSString* alertText;
            
            if ( !error )
            {
                //NSLog ( @"DICTIONARY" );

                NSDictionary* dictionary = result;
                NSArray*      unsortedFriends = [ dictionary objectForKey: @"data" ];
                
                // run through our friends
                for ( NSDictionary* rawFriend in unsortedFriends )
                {
                    // extract the name and ID
                    NSString* nsName = [ rawFriend objectForKey: @"name" ];
                    NSString* nsID   = [ rawFriend objectForKey: @"id"   ];
                    
                    
                    // Defining what characters to accept
                    NSMutableCharacterSet *acceptedCharacters = [[NSMutableCharacterSet alloc] init];
                    [acceptedCharacters formUnionWithCharacterSet:[NSCharacterSet letterCharacterSet]];
                    [acceptedCharacters formUnionWithCharacterSet:[NSCharacterSet decimalDigitCharacterSet]];
                    [acceptedCharacters addCharactersInString:@" _-.!"];
                    
                    // Turn accented letters into normal letters (optional)
                    NSData *sanitizedData = [nsName dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
                    // Corrected back-conversion from NSData to NSString
                    NSString *sanitizedText = [[[NSString alloc] initWithData:sanitizedData encoding:NSASCIIStringEncoding] autorelease];
                    
                    // Removing unaccepted characters
                    NSString* output = [[sanitizedText componentsSeparatedByCharactersInSet:[acceptedCharacters invertedSet]] componentsJoinedByString:@""];
                    
                    // make a new friend
                    sFriend* pFriend = new sFriend;

                    // copy details
                    //strcpy ( pFriend->szName, [ nsName UTF8String ] );
                    strcpy ( pFriend->szName, [ output UTF8String ] );
                    strcpy ( pFriend->szID,   [ nsID   UTF8String ] );
                    
                    // stick it in the list
                    g_FriendsList.push_back ( pFriend );
                    
                    [acceptedCharacters release];
                }
                
                std::sort ( g_FriendsList.begin ( ), g_FriendsList.end ( ), CheckStrings );

                // let the user know we have download friend information
                g_iFriendsState = 1;
            }
            else
            {
                // record failure for whatever reason
                g_iFriendsState = 2;
                
                //alertText = [NSString stringWithFormat:@"error: domain = %@, code = %d", error.domain, error.code];
                //[[[UIAlertView alloc] initWithTitle:@"Result" message:alertText delegate:nil cancelButtonTitle:@"Thanks!" otherButtonTitles:nil]show];
            }
        } ];
    }
    else g_iFriendsState = 2;

    
    /*
    // check plugins are okay
    if ( !g_pSocialPlugins )
        return;
    
    // check facebook is okay
    if ( !g_pSocialPlugins->facebook )
        return;
    
    // set friends state to 0
    g_iFriendsState = 0;
    
    // obtain the friends
    [ [ g_pSocialPlugins facebook ] requestWithGraphPath: @"me/friends" andDelegate: g_pSocialPlugins ];
    */
#endif
#endif
}

//****f* Extras/Facebook/FacebookGetFriendsState
// FUNCTION
//   Returns 1 when the list of friends has been downloaded, otherwise 0.
// SOURCE
int agk::PlatformFacebookGetFriendsState ( void )
{
//****
    
    // friend state
    return g_iFriendsState;
}

//****f* Extras/Facebook/FacebookGetFriendsCount
// FUNCTION
//   Returns number of friends.
// SOURCE
int agk::PlatformFacebookGetFriendsCount ( void )
{
//****
    
    // how many friends the user has
    return ((int)g_FriendsList.size ( )) - 1;
}

//****f* Extras/Facebook/FacebookGetFriendsName
// FUNCTION
//   Returns name for the specified index.
// INPUTS
//   index -- index for the player starting at 0.
// SOURCE
char* agk::PlatformFacebookGetFriendsName ( int iIndex )
{
//****
    char* pszReturn = NULL;

#if !defined(LITEVERSION) && defined(USE_FACEBOOK_SDK)
        
    // return data
    if ( iIndex < g_FriendsList.size ( ) )
    {
        pszReturn = new char [ strlen ( g_FriendsList [ iIndex ]->szName ) + 1 ];
        strcpy ( pszReturn, g_FriendsList [ iIndex ]->szName );
        strcat ( pszReturn, "\0" );
    }
    else
#endif
    {
        pszReturn = new char [ 1 ];
        strcpy ( pszReturn, "\0" );
    }

    return pszReturn;
}

//****f* Extras/Facebook/FacebookGetFriendsID
// FUNCTION
//   Returns ID for the specified index.
// INPUTS
//   index -- index for the player starting at 0.
// SOURCE
char* agk::PlatformFacebookGetFriendsID ( int iIndex )
{
//****
    
    char* pszReturn = NULL;
    
#if !defined(LITEVERSION) && defined(USE_FACEBOOK_SDK)
    // return data
    if ( iIndex < g_FriendsList.size ( ) )
    {
        pszReturn = new char [ strlen ( g_FriendsList [ iIndex ]->szID ) + 1 ];
        strcpy ( pszReturn, g_FriendsList [ iIndex ]->szID );
        strcat ( pszReturn, "\0" );
    }
    else
#endif
    {
        pszReturn = new char [ 1 ];
        strcpy ( pszReturn, "\0" );
    }
    
    return pszReturn;
}

//****f* Extras/Facebook/FacebookDownloadFriendsPhoto
// FUNCTION
//   Download the specified friends photo.
// INPUTS
//   index -- index for the player starting at 0.
// SOURCE
void agk::PlatformFacebookDownloadFriendsPhoto ( int iIndex )
{
//****
#ifdef USE_FACEBOOK_SDK
#ifndef LITEVERSION
    // ensure index is valid
    if ( iIndex >= g_FriendsList.size ( ) )
        return;
    
    // source file
    char szFileSource [ MAX_PATH ] = "";

    // create connection
    //g_iFacebookHTTP = agk::CreateHTTPConnection ( );

    // set up source and destination
    sprintf ( szFileSource,                "v2.6/%s/picture?type=normal", g_FriendsList [ iIndex ]->szID );
    sprintf ( g_szFacebookFileDestination, "FB%sProfile.jpg", g_FriendsList [ iIndex ]->szID );
    
    // start downloading
    //agk::SetHTTPHost ( g_iFacebookHTTP, "graph.facebook.com", 0 );
    agk::GetHTTPFile ( g_iFacebookHTTP, szFileSource, g_szFacebookFileDestination, "" );
    
    // update download state
    g_iFacebookDownloadState = 1;
#endif
#endif
}

//****f* Extras/Facebook/GetFacebookDownloadState
// FUNCTION
//   Returns 0 when nothing is happening, 1 when downloading is taking place
//   and 2 when the download has finished.
// SOURCE
int agk::PlatformGetFacebookDownloadState ( void )
{
//****
#if !defined(LITEVERSION) && defined(USE_FACEBOOK_SDK)
    // if a download has been started then check file progress
    if ( g_iFacebookDownloadState == 1 )
    {
        // when we reach 100% we're done
        if ( agk::GetHTTPFileProgress ( g_iFacebookHTTP ) >= 100 )
        {
            // close the connection
            //agk::CloseHTTPConnection  ( g_iFacebookHTTP );
            //agk::DeleteHTTPConnection ( g_iFacebookHTTP );
            
            // update the state
            g_iFacebookDownloadState = 2;
        }
    }
    
    // return the state
    return g_iFacebookDownloadState;
#else
    return 0;
#endif
}

//****f* Extras/Facebook/GetFacebookDownloadFile
// FUNCTION
//   Returns the filename of the last downloaded file.
// SOURCE
char* agk::PlatformGetFacebookDownloadFile ( void )
{
//****
    
    char* pszReturn = NULL;

#if !defined(LITEVERSION) && defined(USE_FACEBOOK_SDK)
    // return filename if we have one
    if ( g_iFacebookDownloadState == 2 )
    {
        pszReturn = new char [ strlen ( g_szFacebookFileDestination ) + 1 ];
        strcpy ( pszReturn, g_szFacebookFileDestination );
        strcat ( pszReturn, "\0" );
    }
    else
#endif
    {
        pszReturn = new char [ 1 ];
        strcpy ( pszReturn, "\0" );
    }
    
    return pszReturn;
}

bool agk::PlatformHasFacebook ( void )
{
#if !defined(LITEVERSION) && defined(USE_FACEBOOK_SDK)
    return true;
#else
    return false;
#endif
}

char* agk::PlatformFacebookGetUserID			  ( void )
{
	char *str = new char[ m_sFBUserID.GetLength() + 1 ];
	strcpy( str, m_sFBUserID.GetStr() );
	return str;
}

char* agk::PlatformFacebookGetUserName		  ( void )
{
	char *str = new char[ m_sFBName.GetLength() + 1 ];
	strcpy( str, m_sFBName.GetStr() );
	return str;
}

char* agk::PlatformFacebookGetAccessToken		  ( void )
{
	char *str;
#if !defined(LITEVERSION) && defined(USE_FACEBOOK_SDK)
	if ( FBSession.activeSession )
	{
		NSString *sAccessToken = [[[FBSession activeSession] accessTokenData] accessToken];
		const char *szAccessToken = [ sAccessToken UTF8String ];
		str = new char[ strlen(szAccessToken) + 1 ];
		strcpy( str, szAccessToken );
	}
	else
#else
	{
		str = new char[ 1 ];
        *str = 0;
	}
#endif

	return str;
}

#pragma mark -
#pragma mark Notification Commands

extern int agk_PushNotificationsSetup;
extern int agk_LocalNotificationsSetup;

// local notifications

void agk::PlatformCreateLocalNotification( int iID, int datetime, const char *szMessage, const char *szDeepLink )
{
    static int first = 1;
    if ( first )
    {
        first = 0;
        if ([[UIApplication sharedApplication] respondsToSelector:@selector(registerUserNotificationSettings:)])
        {
            agk_LocalNotificationsSetup = 1;
            UIUserNotificationType notifTypes = UIUserNotificationTypeAlert | UIUserNotificationTypeSound;
            if ( agk_PushNotificationsSetup ) notifTypes |= (UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert);
            
            UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:notifTypes categories:nil];
            
            [[UIApplication sharedApplication] registerUserNotificationSettings:settings];
        }
    }
    
    PlatformCancelLocalNotification( iID );
    
    NSDate* dateFromUnix = [ NSDate dateWithTimeIntervalSince1970: datetime ];

    // create the notification
    UILocalNotification* localNotification		 = [ [ UILocalNotification alloc ] init ];
    localNotification.fireDate                   =  dateFromUnix;
	localNotification.alertBody                  = [ NSString stringWithUTF8String: szMessage ];
	localNotification.soundName                  = UILocalNotificationDefaultSoundName;
	localNotification.applicationIconBadgeNumber = 0;

	// set up data in KeyID
	NSMutableDictionary* infoDict = [NSMutableDictionary dictionaryWithCapacity:2];
	[ infoDict setObject:[NSNumber numberWithInt:iID] forKey:@"KeyID" ];
    
	if ( szDeepLink && *szDeepLink )
	{
		NSDictionary* deeplinkDict = [ NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:szDeepLink] forKey:@"deeplink" ];
		[infoDict setObject:deeplinkDict forKey:@"aps"];
	}

	localNotification.userInfo = infoDict;
	
    // schedule the notification
	[ [ UIApplication sharedApplication ] scheduleLocalNotification: localNotification ];
	[ localNotification release ];
}

void agk::PlatformCancelLocalNotification( int iID )
{
	for (UILocalNotification *lNotification in [[UIApplication sharedApplication] scheduledLocalNotifications]) 
    {
        NSNumber *num = [lNotification.userInfo valueForKey:@"KeyID"];
        if ( [num intValue] == iID )
        {
            [[UIApplication sharedApplication] cancelLocalNotification:lNotification];
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


















