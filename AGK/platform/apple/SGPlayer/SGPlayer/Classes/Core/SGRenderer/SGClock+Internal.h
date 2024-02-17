//
//  SGRenderer+Internal.h
//  SGPlayer iOS
//
//  Created by Single on 2018/11/1.
//  Copyright © 2018 single. All rights reserved.
//

#import "SGClock.h"
#import "SGDefines.h"

@protocol SGClockDelegate;

@interface SGClock ()

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, weak) id<SGClockDelegate> delegate;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) Float64 rate;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime currentTime;

/**
 *
 */
- (void)setAudioTime:(CMTime)time running:(BOOL)running;

/**
 *
 */
- (void)setVideoTime:(CMTime)time;

/**
 *
 */
- (BOOL)open;

/**
 *
 */
- (BOOL)close;

/**
 *
 */
- (BOOL)pause;

/**
 *
 */
- (BOOL)resume;

/**
 *
 */
- (BOOL)flush;

@end

@protocol SGClockDelegate <NSObject>

/**
 *
 */
- (void)clock:(SGClock *)clock didChcnageCurrentTime:(CMTime)currentTime;

@end
