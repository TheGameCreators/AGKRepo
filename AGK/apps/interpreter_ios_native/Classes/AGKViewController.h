
#import <UIKit/UIKit.h>

#include "AGKView.h"

#ifdef USE_METAL
    #import <QuartzCore/CAMetalLayer.h>
#else
    #import <OpenGLES/EAGL.h>

    #import <OpenGLES/ES1/gl.h>
    #import <OpenGLES/ES1/glext.h>
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#endif

@interface AGKViewController : UIViewController
{
	BOOL active;
    BOOL displayLinkSupported;
    NSInteger frameInterval;
    /*
	 Use of the CADisplayLink class is the preferred method for controlling your animation timing.
	 CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
	 The NSTimer object is used only as fallback when running on a pre-3.1 device where CADisplayLink isn't available.
	 */
    CADisplayLink *displayLink;
    NSTimer *syncTimer;
}

//@property (nonatomic, strong) IBOutlet UIView* view;

+ (BOOL)isOpen;
+ (AGKViewController*)getSharedViewController;

- (void)setActive;
- (void)setInactive;

@end
