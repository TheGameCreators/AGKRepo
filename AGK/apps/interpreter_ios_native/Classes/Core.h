

#import <UIKit/UIKit.h>
#include "AGKViewController.h"
#import <UserNotifications/UNUserNotificationCenter.h>
#import <AVFoundation/AVAudioPlayer.h>

@class AGKView;

@interface AGKAppDelegate : NSObject <UIApplicationDelegate, AVAudioPlayerDelegate, UNUserNotificationCenterDelegate> {
    UIWindow *window;
}

@property (nonatomic, strong) UIWindow *window;

@end

@interface GADBannerView : UIView @end
@interface GADRequest : NSObject @end
@interface GADInterstitial : NSObject @end
@interface Chartboost : NSObject @end
