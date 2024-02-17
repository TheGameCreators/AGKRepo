
#import <UIKit/UIKit.h>
#include "Classes/Core.h"

int main(int argc, char *argv[]) {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain( argc, argv, nil, NSStringFromClass([AGKAppDelegate class]) );
    [pool release];
    return retVal;
}
