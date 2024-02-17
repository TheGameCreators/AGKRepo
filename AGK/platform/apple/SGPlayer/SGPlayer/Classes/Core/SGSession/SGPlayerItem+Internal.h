//
//  SGPlayerItem+Internal.h
//  SGPlayer iOS
//
//  Created by Single on 2018/10/25.
//  Copyright © 2018 single. All rights reserved.
//

#import "SGPlayerItem.h"
#import "SGProcessorOptions.h"
#import "SGAudioDescriptor.h"
#import "SGDemuxerOptions.h"
#import "SGDecoderOptions.h"
#import "SGCapacity.h"
#import "SGFrame.h"

@protocol SGPlayerItemDelegate;

/**
 *
 */
typedef NS_ENUM(NSUInteger, SGPlayerItemState) {
    SGPlayerItemStateNone     = 0,
    SGPlayerItemStateOpening  = 1,
    SGPlayerItemStateOpened   = 2,
    SGPlayerItemStateReading  = 3,
    SGPlayerItemStateSeeking  = 4,
    SGPlayerItemStateFinished = 5,
    SGPlayerItemStateClosed   = 6,
    SGPlayerItemStateFailed   = 7,
};

@interface SGPlayerItem ()

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy) SGDemuxerOptions *demuxerOptions;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy) SGDecoderOptions *decoderOptions;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy) SGProcessorOptions *processorOptions;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, weak) id<SGPlayerItemDelegate> delegate;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) SGPlayerItemState state;

/**
 *
 */
- (BOOL)open;

/**
 *
 */
- (BOOL)start;

/**
 *
 */
- (BOOL)close;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly, getter=isSeekable) BOOL seekable;

/**
 *
 */
- (BOOL)seekToTime:(CMTime)time;

/**
 *
 */
- (BOOL)seekToTime:(CMTime)time result:(SGSeekResult)result;

/**
 *
 */
- (BOOL)seekToTime:(CMTime)time toleranceBefore:(CMTime)toleranceBefore toleranceAfter:(CMTime)toleranceAfter result:(SGSeekResult)result;

/**
 *
 */
- (SGCapacity)capacityWithType:(SGMediaType)type;

/**
 *
 */
- (BOOL)isAvailable:(SGMediaType)type;

/**
 *
 */
- (BOOL)isFinished:(SGMediaType)type;

/**
 *
 */
- (__kindof SGFrame *)copyAudioFrame:(SGTimeReader)timeReader;
- (__kindof SGFrame *)copyVideoFrame:(SGTimeReader)timeReader;

@end

@protocol SGPlayerItemDelegate <NSObject>

/**
 *
 */
- (void)playerItem:(SGPlayerItem *)playerItem didChangeState:(SGPlayerItemState)state;

/**
 *
 */
- (void)playerItem:(SGPlayerItem *)playerItem didChangeCapacity:(SGCapacity)capacity type:(SGMediaType)type;

@end
