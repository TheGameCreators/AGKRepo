//
//  ExtensionDelegate.m
//  Watch Extension
//
//  Created by Paul Johnston on 17/10/2017.
//
//

#import "ExtensionDelegate.h"

extern InterfaceController *g_pInterface;
ExtensionDelegate *g_pExtensionDelegate = 0;

@interface ExtensionDelegate ()
@property (nonatomic, weak) IBOutlet InterfaceController* controller;
@end

@implementation ExtensionDelegate

- (void)applicationDidFinishLaunching {
    // Perform any final initialization of your application.
    g_pExtensionDelegate = self;
    
    sessionState = 0;
    WCSession* session = [WCSession defaultSession];
    session.delegate = self;
    [session activateSession];
}

- (void)applicationDidBecomeActive {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillResignActive {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, etc.
}

- (void)session:(WCSession *)session activationDidCompleteWithState:(WCSessionActivationState)activationState
          error:(NSError *)error
{
    if ( activationState == WCSessionActivationStateActivated ) sessionState = 1;
    else sessionState = 0;
}

- (void)session:(WCSession *)session didReceiveUserInfo:(NSDictionary<NSString *,id> *)userInfo
{
    if ( g_pInterface )
    {
        [g_pInterface setQuestion:userInfo[@"question"]];
    }
}

- (void) sendMessage:(NSDictionary*)message
{
    [[WCSession defaultSession] transferUserInfo:message];
}

- (void)handleBackgroundTasks:(NSSet<WKRefreshBackgroundTask *> *)backgroundTasks {
    // Sent when the system needs to launch the application in the background to process tasks. Tasks arrive in a set, so loop through and process each one.
    for (WKRefreshBackgroundTask * task in backgroundTasks) {
        // Check the Class of each task to decide how to process it
        if ([task isKindOfClass:[WKApplicationRefreshBackgroundTask class]]) {
            // Be sure to complete the background task once you’re done.
            WKApplicationRefreshBackgroundTask *backgroundTask = (WKApplicationRefreshBackgroundTask*)task;
            [backgroundTask setTaskCompleted];
        } else if ([task isKindOfClass:[WKSnapshotRefreshBackgroundTask class]]) {
            // Snapshot tasks have a unique completion call, make sure to set your expiration date
            WKSnapshotRefreshBackgroundTask *snapshotTask = (WKSnapshotRefreshBackgroundTask*)task;
            [snapshotTask setTaskCompletedWithDefaultStateRestored:YES estimatedSnapshotExpiration:[NSDate distantFuture] userInfo:nil];
        } else if ([task isKindOfClass:[WKWatchConnectivityRefreshBackgroundTask class]]) {
            // Be sure to complete the background task once you’re done.
            WKWatchConnectivityRefreshBackgroundTask *backgroundTask = (WKWatchConnectivityRefreshBackgroundTask*)task;
            [backgroundTask setTaskCompleted];
        } else if ([task isKindOfClass:[WKURLSessionRefreshBackgroundTask class]]) {
            // Be sure to complete the background task once you’re done.
            WKURLSessionRefreshBackgroundTask *backgroundTask = (WKURLSessionRefreshBackgroundTask*)task;
            [backgroundTask setTaskCompleted];
        } else {
            // make sure to complete unhandled task types
            [task setTaskCompleted];
        }
    }
}

@end
