
#import <QuartzCore/QuartzCore.h>

#import "AGKView.h"
#include "agk.h"

using namespace AGK;

@implementation AGKView


// You must implement this method
+ (Class)layerClass
{
    NSLog( @"View layer class" );
    
#ifdef USE_METAL
    return [CAMetalLayer class];
#else
    return [CAEAGLLayer class];
#endif
}

- (id)init
{
    NSLog( @"View init" );
    self = [super init];
    return self;
}

@end

@implementation AGKView ( input )

-(BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer { return FALSE; }

- ( void ) touchesBegan: ( NSSet* ) touches withEvent: ( UIEvent* ) event
{
	// this function will be activated whenever a touch even begins
	
	for ( UITouch *pTouch in touches )
	{
		CGPoint location = [ pTouch locationInView: self ];
		//NSLog( @"Touch Pressed ID: %p, X: %f, Y: %f", pTouch, location.x, location.y );
        // don't mind about losing 64bit precision as long as the ID is fairly unique
        unsigned long touchID = (unsigned long) pTouch;
		agk::TouchPressed( (UINT) touchID, location.x, location.y );
	}	
}

- ( void ) touchesMoved: ( NSSet* ) touches withEvent: ( UIEvent* ) event
{
	for ( UITouch *pTouch in touches )
	{
		CGPoint location = [ pTouch locationInView: self ];
		//NSLog( @"Touch Moved ID: %p, X: %f, Y: %f", pTouch, location.x, location.y );
        // don't mind about losing 64bit precision as long as the ID is fairly unique
        unsigned long touchID = (unsigned long) pTouch;
		agk::TouchMoved( (UINT) touchID, location.x, location.y );
	}
}

- ( void ) touchesEnded: ( NSSet* ) touches withEvent: ( UIEvent* )event
{
	for ( UITouch *pTouch in touches )
	{
		CGPoint location = [ pTouch locationInView: self ];
		//NSLog( @"Touch Released ID: %p, X: %f, Y: %f", pTouch, location.x, location.y );
        // don't mind about losing 64bit precision as long as the ID is fairly unique
        unsigned long touchID = (unsigned long) pTouch;
		agk::TouchReleased( (UINT) touchID, location.x, location.y );
	}
}

- ( void ) touchesCancelled: ( NSSet* ) touches withEvent: ( UIEvent* )event
{
	for ( UITouch *pTouch in touches )
	{
		CGPoint location = [ pTouch locationInView: self ];
		//NSLog( @"Touch Cancelled ID: %p, X: %f, Y: %f", pTouch, location.x, location.y );
        // don't mind about losing 64bit precision as long as the ID is fairly unique
        unsigned long touchID = (unsigned long) pTouch;
		agk::TouchReleased( (UINT) touchID, location.x, location.y );
	}
}

@end
