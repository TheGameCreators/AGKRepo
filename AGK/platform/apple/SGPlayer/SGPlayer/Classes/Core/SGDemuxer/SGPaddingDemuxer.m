//
//  SGPaddingDemuxer.m
//  SGPlayer
//
//  Created by Single on 2019/6/4.
//  Copyright © 2019 single. All rights reserved.
//

#import "SGPaddingDemuxer.h"
#import "SGPacket+Internal.h"
#import "SGError.h"

@interface SGPaddingDemuxer ()

@property (NS_NONATOMIC_IOSONLY, readonly) CMTime lasttime;

@end

@implementation SGPaddingDemuxer

@synthesize tracks = _tracks;
@synthesize options = _options;
@synthesize delegate = _delegate;
@synthesize metadata = _metadata;
@synthesize duration = _duration;
@synthesize finishedTracks = _finishedTracks;

- (instancetype)initWithDuration:(CMTime)duration
{
    if (self = [super init]) {
        self->_duration = duration;
        [self seekToTime:kCMTimeZero withError:NULL];
    }
    return self;
}

#pragma mark - Control

- (id<SGDemuxable>)sharedDemuxer
{
    return nil;
}

- (BOOL)openWithError:(NSError *__autoreleasing  _Nullable *)error
{
    return YES;
}

- (BOOL)closeWithError:(NSError *__autoreleasing  _Nullable *)error
{
    return YES;
}

- (BOOL)seekableWithError:(NSError *__autoreleasing  _Nullable *)error
{
    return YES;
}

- (BOOL)seekToTime:(CMTime)time withError:(NSError *__autoreleasing  _Nullable * _Nullable)error
{
    return [self seekToTime:time toleranceBefore:kCMTimeInvalid toleranceAfter:kCMTimeInvalid withError:error];
}

- (BOOL)seekToTime:(CMTime)time toleranceBefore:(CMTime)toleranceBefor toleranceAfter:(CMTime)toleranceAfter withError:(NSError *__autoreleasing  _Nullable * _Nullable)error
{
    if (!CMTIME_IS_NUMERIC(time)) {
        if (error) {
            *error = SGCreateError(SGErrorCodeInvlidTime, SGActionCodeFormatSeekFrame);
        }
        return NO;
    }
    time = CMTimeMaximum(time, kCMTimeZero);
    time = CMTimeMinimum(time, self->_duration);
    self->_lasttime = time;
    return YES;
}

- (BOOL)nextPacket:(SGPacket **)packet withError:(NSError *__autoreleasing  _Nullable * _Nullable)error
{
    if (CMTimeCompare(self->_lasttime, self->_duration) >= 0) {
        if (error) {
            *error = SGCreateError(SGErrorCodeDemuxerEndOfFile, SGActionCodeFormatReadFrame);
        }
        return NO;
    }
    CMTime timeStamp = self->_lasttime;
    CMTime duration = CMTimeSubtract(self->_duration, self->_lasttime);
    SGPacket *pkt = [SGPacket packet];
    pkt.flags |= SGDataFlagPadding;
    pkt.core->size = 1;
    pkt.core->pts = av_rescale(AV_TIME_BASE, timeStamp.value, timeStamp.timescale);
    pkt.core->dts = av_rescale(AV_TIME_BASE, timeStamp.value, timeStamp.timescale);
    pkt.core->duration = av_rescale(AV_TIME_BASE, duration.value, duration.timescale);
    SGCodecDescriptor *cd = [[SGCodecDescriptor alloc] init];
    cd.timebase = AV_TIME_BASE_Q;
    [pkt setCodecDescriptor:cd];
    [pkt fill];
    *packet = pkt;
    self->_lasttime = self->_duration;
    return YES;
}

@end
