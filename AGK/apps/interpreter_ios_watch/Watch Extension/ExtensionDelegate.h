//
//  ExtensionDelegate.h
//  Watch Extension
//
//  Created by Paul Johnston on 17/10/2017.
//
//

#import <WatchKit/WatchKit.h>
#import <WatchConnectivity/WatchConnectivity.h>
#include "InterfaceController.h"

@interface ExtensionDelegate : NSObject <WKExtensionDelegate, WCSessionDelegate>
{
    int sessionState;
}
- (void) sendMessage:(NSDictionary*)message;
@end
