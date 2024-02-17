//
//  SGURLSource.h
//  SGPlayer
//
//  Created by Single on 2018/1/16.
//  Copyright © 2018年 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGDemuxerOptions.h>
#import <SGPlayer/SGPacket.h>
#import <SGPlayer/SGAsset.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SGPacketOutputDelegate;

/**
 *
 */
typedef NS_ENUM(NSUInteger, SGPacketOutputState) {
    SGPacketOutputStateNone     = 0,
    SGPacketOutputStateOpening  = 1,
    SGPacketOutputStateOpened   = 2,
    SGPacketOutputStateReading  = 3,
    SGPacketOutputStatePaused   = 4,
    SGPacketOutputStateSeeking  = 5,
    SGPacketOutputStateFinished = 6,
    SGPacketOutputStateClosed   = 7,
    SGPacketOutputStateFailed   = 8,
};

@interface SGPacketOutput : NSObject

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/**
 *
 */
- (instancetype)initWithAsset:(SGAsset *)asset NS_DESIGNATED_INITIALIZER;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy) SGDemuxerOptions *options;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, weak) id<SGPacketOutputDelegate> delegate;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) SGPacketOutputState state;

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
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGTrack *> *finishedTracks;

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

@end

@protocol SGPacketOutputDelegate <NSObject>

/**
 *
 */
- (void)packetOutput:(SGPacketOutput *)packetOutput didChangeState:(SGPacketOutputState)state;

/**
 *
 */
- (void)packetOutput:(SGPacketOutput *)packetOutput didOutputPacket:(SGPacket *)packet;

@end

NS_ASSUME_NONNULL_END
