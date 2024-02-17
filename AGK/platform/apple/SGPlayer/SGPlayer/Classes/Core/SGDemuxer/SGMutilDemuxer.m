//
//  SGMutilDemuxer.m
//  SGPlayer
//
//  Created by Single on 2018/11/16.
//  Copyright © 2018 single. All rights reserved.
//

#import "SGMutilDemuxer.h"
#import "SGError.h"

@interface SGMutilDemuxer ()

@property (NS_NONATOMIC_IOSONLY, strong, readonly) NSArray<id<SGDemuxable>> *demuxers;
@property (NS_NONATOMIC_IOSONLY, strong, readonly) NSMutableArray<SGTrack *> *finishedTracksInternal;
@property (NS_NONATOMIC_IOSONLY, strong, readonly) NSMutableArray<id<SGDemuxable>> *finishedDemuxers;
@property (NS_NONATOMIC_IOSONLY, strong, readonly) NSMutableDictionary<NSString *, NSValue *> *timeStamps;

@end

@implementation SGMutilDemuxer

@synthesize tracks = _tracks;
@synthesize duration = _duration;
@synthesize metadata = _metadata;

- (instancetype)initWithDemuxables:(NSArray<id<SGDemuxable>> *)demuxables
{
    if (self = [super init]) {
        self->_demuxers = [demuxables copy];
        self->_finishedDemuxers = [NSMutableArray array];
        self->_finishedTracksInternal = [NSMutableArray array];
    }
    return self;
}

#pragma mark - Setter & Getter

- (void)setDelegate:(id<SGDemuxableDelegate>)delegate
{
    for (id<SGDemuxable> obj in self->_demuxers) {
        obj.delegate = delegate;
    }
}

- (id<SGDemuxableDelegate>)delegate
{
    return self->_demuxers.firstObject.delegate;
}

- (void)setOptions:(SGDemuxerOptions *)options
{
    for (id<SGDemuxable> obj in self->_demuxers) {
        obj.options = options;
    }
}

- (SGDemuxerOptions *)options
{
    return self->_demuxers.firstObject.options;
}

- (NSArray<SGTrack *> *)finishedTracks
{
    return self->_finishedTracksInternal.copy;
}

#pragma mark - Control

- (id<SGDemuxable>)sharedDemuxer
{
    return nil;
}

- (BOOL)openWithError:(NSError *__autoreleasing  _Nullable *)error
{
    for (id<SGDemuxable> obj in self->_demuxers) {
        BOOL success = [obj openWithError:error];
        if (!success) {
            return NO;
        }
    }
    CMTime duration = kCMTimeZero;
    NSMutableArray<SGTrack *> *tracks = [NSMutableArray array];
    for (id<SGDemuxable> obj in self->_demuxers) {
        NSAssert(CMTIME_IS_VALID(obj.duration), @"Invalid Duration.");
        duration = CMTimeMaximum(duration, obj.duration);
        [tracks addObjectsFromArray:obj.tracks];
    }
    self->_duration = duration;
    self->_tracks = [tracks copy];
    NSMutableArray<NSNumber *> *indexes = [NSMutableArray array];
    for (SGTrack *obj in self->_tracks) {
        NSAssert(![indexes containsObject:@(obj.index)], @"Invalid Track Indexes");
        [indexes addObject:@(obj.index)];
    }
    self->_timeStamps = [NSMutableDictionary dictionary];
    return YES;
}

- (BOOL)closeWithError:(NSError *__autoreleasing  _Nullable *)error
{
    for (id<SGDemuxable> obj in self->_demuxers) {
        BOOL success = [obj closeWithError:error];
        if (!success) {
            return NO;
        }
    }
    return YES;
}

- (BOOL)seekableWithError:(NSError *__autoreleasing  _Nullable *)error
{
    for (id<SGDemuxable> obj in self->_demuxers) {
        BOOL success = [obj seekableWithError:error];
        if (!success) {
            return NO;
        }
    }
    return YES;
}

- (BOOL)seekToTime:(CMTime)time withError:(NSError *__autoreleasing  _Nullable *)error
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
    for (id<SGDemuxable> obj in self->_demuxers) {
        BOOL success = [obj seekToTime:time toleranceBefore:toleranceBefor toleranceAfter:toleranceAfter withError:error];
        if (!success) {
            return NO;
        }
    }
    [self->_timeStamps removeAllObjects];
    [self->_finishedDemuxers removeAllObjects];
    [self->_finishedTracksInternal removeAllObjects];
    return YES;
}

- (BOOL)nextPacket:(SGPacket **)packet withError:(NSError *__autoreleasing  _Nullable * _Nullable)outError
{
    NSError *error = nil;
    while (YES) {
        id<SGDemuxable> demuxable = nil;
        CMTime minimumTime = kCMTimePositiveInfinity;
        for (id<SGDemuxable> obj in self->_demuxers) {
            if ([self->_finishedDemuxers containsObject:obj]) {
                continue;
            }
            NSString *key = [NSString stringWithFormat:@"%p", obj];
            NSValue *value = [self->_timeStamps objectForKey:key];
            if (!value) {
                demuxable = obj;
                break;
            }
            CMTime time = kCMTimePositiveInfinity;
            [value getValue:&time];
            if (CMTimeCompare(time, minimumTime) < 0) {
                minimumTime = time;
                demuxable = obj;
            }
        }
        if (!demuxable) {
            if (outError) {
                *outError = SGCreateError(SGErrorCodeDemuxerEndOfFile, SGActionCodeMutilDemuxerNext);
            }
            return NO;
        }
        BOOL success = [demuxable nextPacket:packet withError:&error];
        if (!success) {
            if (error.code == SGErrorCodeImmediateExitRequested) {
                break;
            }
            [self->_finishedDemuxers addObject:demuxable];
            [self->_finishedTracksInternal addObjectsFromArray:demuxable.tracks];
            continue;
        }
        CMTime decodeTimeStamp = (*packet).decodeTimeStamp;
        NSString *key = [NSString stringWithFormat:@"%p", demuxable];
        NSValue *value = [NSValue value:&decodeTimeStamp withObjCType:@encode(CMTime)];
        [self->_timeStamps setObject:value forKey:key];
        break;
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
