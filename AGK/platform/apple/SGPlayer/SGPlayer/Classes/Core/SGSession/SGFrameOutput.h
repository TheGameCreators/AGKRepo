//
//  SGFrameOutput.h
//  SGPlayer iOS
//
//  Created by Single on 2018/10/22.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGDemuxerOptions.h>
#import <SGPlayer/SGDecoderOptions.h>
#import <SGPlayer/SGCapacity.h>
#import <SGPlayer/SGAsset.h>
#import <SGPlayer/SGFrame.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SGFrameOutputDelegate;

/**
 *
 */
typedef NS_ENUM(NSUInteger, SGFrameOutputState) {
    SGFrameOutputStateNone     = 0,
    SGFrameOutputStateOpening  = 1,
    SGFrameOutputStateOpened   = 2,
    SGFrameOutputStateReading  = 3,
    SGFrameOutputStateSeeking  = 4,
    SGFrameOutputStateFinished = 5,
    SGFrameOutputStateClosed   = 6,
    SGFrameOutputStateFailed   = 7,
};

@interface SGFrameOutput : NSObject

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/**
 *
 */
- (instancetype)initWithAsset:(SGAsset *)asset NS_DESIGNATED_INITIALIZER;

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
@property (NS_NONATOMIC_IOSONLY, weak) id<SGFrameOutputDelegate> delegate;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) SGFrameOutputState state;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSError *error;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGTrack *> *tracks;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGTrack *> *selectedTracks;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSDictionary *metadata;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime duration;

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
- (BOOL)pause:(SGMediaType)type;

/**
 *
 */
- (BOOL)resume:(SGMediaType)type;

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
- (BOOL)seekToTime:(CMTime)time result:(nullable SGSeekResult)result;

/**
 *
 */
- (BOOL)seekToTime:(CMTime)time toleranceBefore:(CMTime)toleranceBefor toleranceAfter:(CMTime)toleranceAfter result:(nullable SGSeekResult)result;

/**
 *
 */
- (BOOL)selectTracks:(NSArray<SGTrack *> *)tracks;

/**
 *
 */
- (SGCapacity)capacityWithType:(SGMediaType)type;

@end

@protocol SGFrameOutputDelegate <NSObject>

/**
 *
 */
- (void)frameOutput:(SGFrameOutput *)frameOutput didChangeState:(SGFrameOutputState)state;

/**
 *
 */
- (void)frameOutput:(SGFrameOutput *)frameOutput didChangeCapacity:(SGCapacity)capacity type:(SGMediaType)type;

/**
 *
 */
- (void)frameOutput:(SGFrameOutput *)frameOutput didOutputFrames:(NSArray<__kindof SGFrame *> *)frames needsDrop:(BOOL(^)(void))needsDrop;

@end

NS_ASSUME_NONNULL_END
