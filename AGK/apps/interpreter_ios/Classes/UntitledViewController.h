//
//  UntitledViewController.h
//  Untitled
//
//  Created by Paul Johnston on 02/12/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#include "EAGLView.h"

#ifdef USE_METAL
    #import <QuartzCore/CAMetalLayer.h>
#else
    #import <OpenGLES/EAGL.h>

    #import <OpenGLES/ES1/gl.h>
    #import <OpenGLES/ES1/glext.h>
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#endif

@interface UntitledViewController : UIViewController
{
    int appActive;
    
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

- (void)setActive;
- (void)setInactive;

- (void)setAppActive: (int)mode;

@end
