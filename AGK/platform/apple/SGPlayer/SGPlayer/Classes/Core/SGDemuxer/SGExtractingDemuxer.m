//
//  SGExtractingDemuxer.m
//  SGPlayer
//
//  Created by Single on 2018/11/14.
//  Copyright © 2018 single. All rights reserved.
//

#import "SGExtractingDemuxer.h"
#import "SGPacket+Internal.h"
#import "SGObjectQueue.h"
#import "SGError.h"
#import "SGMacro.h"

@interface SGExtractingDemuxer ()

{
    struct {
        BOOL finished;
        BOOL inputting;
        BOOL outputting;
    } _flags;
}

@property (NS_NONATOMIC_IOSONLY, strong, readonly) SGTrack *track;
@property (NS_NONATOMIC_IOSONLY, strong, readonly) SGTimeLayout *scaleLayout;
@property (NS_NONATOMIC_IOSONLY, strong, readonly) SGTimeLayout *offsetLayout;
@property (NS_NONATOMIC_IOSONLY, strong, readonly) SGObjectQueue *packetQueue;

- (BOOL)nextPacketInternalOvergop:(SGPacket **)packet withError:(NSError**)outError;
- (BOOL)nextPacketInternal:(SGPacket **)packet withError:(NSError**)outError;

@end

@implementation SGExtractingDemuxer

@synthesize tracks = _tracks;
@synthesize duration = _duration;
@synthesize finishedTracks = _finishedTracks;

- (instancetype)initWithDemuxable:(id<SGDemuxable>)demuxable index:(NSInteger)index timeRange:(CMTimeRange)timeRange scale:(CMTime)scale
{
    if (self = [super init]) {
        self->_overgop = YES;
        self->_scale = scale;
        self->_index = index;
        self->_demuxable = demuxable;
        self->_timeRange = SGCMTimeRangeFitting(timeRange);
        self->_packetQueue = [[SGObjectQueue alloc] init];
    }
    return self;
}

#pragma mark - Mapping

SGGet0Map(id<SGDemuxableDelegate>, delegate, self->_demuxable)
SGSet1Map(void, setDelegate, id<SGDemuxableDelegate>, self->_demuxable)
SGGet0Map(SGDemuxerOptions *, options, self->_demuxable)
SGSet1Map(void, setOptions, SGDemuxerOptions *, self->_demuxable)
SGGet0Map(NSDictionary *, metadata, self->_demuxable)

#pragma mark - Control

- (BOOL)closeWithError:(NSError *__autoreleasing  _Nullable *)error
{
    return [self->_demuxable closeWithError:error];
}

- (BOOL)seekableWithError:(NSError *__autoreleasing  _Nullable *)error
{
    return [self->_demuxable seekableWithError:error];
}

- (id<SGDemuxable>)sharedDemuxer
{
    return [self->_demuxable sharedDemuxer];
}

- (BOOL)openWithError:(NSError *__autoreleasing  _Nullable *)error
{
    BOOL success = [self->_demuxable openWithError:error];
    if (!success) {
        return NO;
    }
    for (SGTrack *obj in self->_demuxable.tracks) {
        if (self->_index == obj.index) {
            self->_track = obj;
            self->_tracks = @[obj];
            break;
        }
    }
    CMTime start = self->_timeRange.start;
    if (!CMTIME_IS_NUMERIC(start)) {
        start = kCMTimeZero;
    }
    CMTime duration = self->_timeRange.duration;
    if (!CMTIME_IS_NUMERIC(duration)) {
        duration = CMTimeSubtract(self->_demuxable.duration, start);
    }
    self->_timeRange = CMTimeRangeMake(start, duration);
    self->_duration = SGCMTimeMultiply(duration, self->_scale);
    self->_scaleLayout = [[SGTimeLayout alloc] initWithScale:self->_scale];
    self->_offsetLayout = [[SGTimeLayout alloc] initWithOffset:CMTimeMultiply(start, -1)];
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
    time = [self->_scaleLayout reconvertTimeStamp:time];
    time = [self->_offsetLayout reconvertTimeStamp:time];
    BOOL success = [self->_demuxable seekToTime:time toleranceBefore:toleranceBefor toleranceAfter:toleranceAfter withError:error];
    if (!success) {
        return NO;
    }
    [self->_packetQueue flush];
    self->_flags.finished = NO;
    self->_flags.inputting = NO;
    self->_flags.outputting = NO;
    self->_finishedTracks = nil;
    return YES;
}

- (BOOL)nextPacket:(SGPacket **)packet withError:(NSError *__autoreleasing  _Nullable * _Nullable)error
{
    if (self->_overgop) {
        return [self nextPacketInternalOvergop:packet withError:error];
    }
    return [self nextPacketInternal:packet withError:error];
}

- (BOOL)nextPacketInternal:(SGPacket **)packet withError:(NSError**)outError
{
    NSError *error = nil;
    while (YES) {
        SGPacket *pkt = nil;
        BOOL success = [self->_demuxable nextPacket:&pkt withError:&error];
        if (!success) {
            break;
        }
        if (self->_index != pkt.track.index) {
            [pkt unlock];
            continue;
        }
        if (CMTimeCompare(pkt.timeStamp, self->_timeRange.start) < 0) {
            [pkt unlock];
            continue;
        }
        if (CMTimeCompare(pkt.timeStamp, CMTimeRangeGetEnd(self->_timeRange)) >= 0) {
            [pkt unlock];
            error = SGCreateError(SGErrorCodeDemuxerEndOfFile, SGActionCodeURLDemuxerFunnelNext);
            break;
        }
        [pkt.codecDescriptor appendTimeLayout:self->_offsetLayout];
        [pkt.codecDescriptor appendTimeLayout:self->_scaleLayout];
        [pkt.codecDescriptor appendTimeRange:self->_timeRange];
        [pkt fill];
        *packet = pkt;
        break;
    }
    if (error.code == SGErrorCodeDemuxerEndOfFile) {
        self->_finishedTracks = self->_tracks.copy;
    }
    if (error) {
        if (outError) {
            *outError = error;
        }
        return NO;
    }
    return YES;
}

- (BOOL)nextPacketInternalOvergop:(SGPacket **)packet withError:(NSError**)outError
{
    NSError *error = nil;
    while (YES) {
        SGPacket *pkt = nil;
        if (self->_flags.outputting) {
            [self->_packetQueue getObjectAsync:&pkt];
            if (pkt) {
                [pkt.codecDescriptor appendTimeLayout:self->_offsetLayout];
                [pkt.codecDescriptor appendTimeLayout:self->_scaleLayout];
                [pkt.codecDescriptor appendTimeRange:self->_timeRange];
                [pkt fill];
                *packet = pkt;
                break;
            }
        }
        if (self->_flags.finished) {
            error = SGCreateError(SGErrorCodeDemuxerEndOfFile, SGActionCodeURLDemuxerFunnelNext);
            break;
        }
        BOOL success = [self->_demuxable nextPacket:&pkt withError:&error];
        if (!success) {
            if (error.code == SGErrorCodeImmediateExitRequested) {
                break;
            }
            self->_flags.finished = YES;
            continue;
        }
        if (self->_index != pkt.track.index) {
            [pkt unlock];
            continue;
        }
        if (CMTimeCompare(pkt.timeStamp, self->_timeRange.start) < 0) {
            if (pkt.core->flags & AV_PKT_FLAG_KEY) {
                [self->_packetQueue flush];
                self->_flags.inputting = YES;
            }
            if (self->_flags.inputting) {
                [self->_packetQueue putObjectSync:pkt];
            }
            [pkt unlock];
            continue;
        }
        if (CMTimeCompare(pkt.timeStamp, CMTimeRangeGetEnd(self->_timeRange)) >= 0) {
            if (pkt.core->flags & AV_PKT_FLAG_KEY) {
                self->_flags.finished = YES;
            } else {
                [self->_packetQueue putObjectSync:pkt];
            }
            [pkt unlock];
            continue;
        }
        if (!self->_flags.outputting && pkt.core->flags & AV_PKT_FLAG_KEY) {
            [self->_packetQueue flush];
        }
        self->_flags.outputting = YES;
        [self->_packetQueue putObjectSync:pkt];
        [pkt unlock];
        continue;
    }
    if (error.code == SGErrorCodeDemuxerEndOfFile) {
        self->_finishedTracks = self->_tracks.copy;
    }
    if (error) {
        if (outError) {
            *outError = error;
        }
        return NO;
    }
    return YES;
}

@end
