//
//  SGAudioMixerUnit.h
//  SGPlayer
//
//  Created by Single on 2018/11/29.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGAudioFrame.h>
#import <SGPlayer/SGCapacity.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGAudioMixerUnit : NSObject

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTimeRange timeRange;

/**
 *
 */
- (BOOL)putFrame:(SGAudioFrame *)frame;

/**
 *
 */
- (NSArray<SGAudioFrame *> *)framesToEndTime:(CMTime)endTime;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) SGCapacity capacity;

/**
 *
 */
- (void)flush;

@end

NS_ASSUME_NONNULL_END
