//
//  SGRenderer+Internal.h
//  SGPlayer iOS
//
//  Created by Single on 2018/11/1.
//  Copyright © 2018 single. All rights reserved.
//

#import "SGRenderable.h"
#import "SGAudioDescriptor.h"
#import "SGAudioRenderer.h"
#import "SGVideoRenderer.h"
#import "SGClock+Internal.h"

@class SGAudioFormatter;

@interface SGAudioRenderer () <SGRenderable>

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/**
 *
 */
- (instancetype)initWithClock:(SGClock *)clock NS_DESIGNATED_INITIALIZER;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) Float64 rate;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) SGAudioDescriptor *descriptor;

@end

@interface SGVideoRenderer () <SGRenderable>

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/**
 *
 */
- (instancetype)initWithClock:(SGClock *)clock NS_DESIGNATED_INITIALIZER;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) Float64 rate;

@end
