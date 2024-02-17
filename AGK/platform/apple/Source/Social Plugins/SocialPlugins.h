#import "Rate.h"

#import <GoogleMobileAds/GADBannerView.h>

//#define USE_FACEBOOK_SDK

#ifndef LITEVERSION
  #ifdef USE_FACEBOOK_SDK
    #import <FBSDKCoreKit/FBSDKCoreKit.h>
    #import <FBSDKLoginKit/FBSDKLoginKit.h>
  #endif
#endif

@interface SocialPlugins : NSObject
{
    @public
        GADBannerView*          bannerView_;
}

@end
