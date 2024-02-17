//
//  SGFrameReader.m
//  SGPlayer
//
//  Created by Single on 2019/11/12.
//  Copyright © 2019 single. All rights reserved.
//

#import "SGFrameReader.h"
#import "SGAsset+Internal.h"
#import "SGAudioDecoder.h"
#import "SGVideoDecoder.h"
#import "SGObjectQueue.h"
#import "SGDecodable.h"
#import "SGOptions.h"
#import "SGMacro.h"
#import "SGError.h"
#import "SGLock.h"

@interface SGFrameReader () <SGDemuxableDelegate>

{
    struct {
        BOOL noMorePacket;
    } _flags;
}

@property (NS_NONATOMIC_IOSONLY, strong, readonly) id<SGDemuxable> demuxer;
@property (NS_NONATOMIC_IOSONLY, strong, readonly) SGObjectQueue *frameQueue;
@property (NS_NONATOMIC_IOSONLY, strong, readonly) NSMutableDictionary<NSNumber *, id<SGDecodable>> *decoders;

@end

@implementation SGFrameReader

@synthesize selectedTracks = _selectedTracks;
@synthesize decoderOptions = _decoderOptions;

- (instancetype)initWithAsset:(SGAsset *)asset
{
    if (self = [super init]) {
        self->_demuxer = [asset newDemuxer];
        self->_demuxer.delegate = self;
        self->_demuxer.options = [SGOptions sharedOptions].demuxer.copy;
        self->_decoderOptions = [SGOptions sharedOptions].decoder.copy;
    }
    return self;
}

- (void)dealloc
{
    [self closeWithError:NULL];
}

#pragma mark - Mapping

SGGet0Map(CMTime, duration, self->_demuxer)
-(BOOL)seekableWithError:(NSError *__autoreleasing  _Nullable *)error
{
    return [self->_demuxer seekableWithError:error];
}
SGGet0Map(NSDictionary *, metadata, self->_demuxer)
SGGet0Map(SGDemuxerOptions *, options, self->_demuxer)
SGGet0Map(NSArray<SGTrack *> *, tracks, self->_demuxer)
SGGet00Map(SGDemuxerOptions *,demuxerOptions, options, self->_demuxer)
SGSet11Map(void, setDemuxerOptions, setOptions, SGDemuxerOptions *, self->_demuxer)

#pragma mark - Setter & Getter

- (NSArray<SGTrack *> *)selectedTracks
{
    return [self->_selectedTracks copy];
}

- (void)setDecoderOptions:(SGDecoderOptions *)decoderOptions
{
    self->_decoderOptions = decoderOptions;
    for (id<SGDecodable> obj in self->_decoders.allValues) {
        obj.options = decoderOptions;
    }
}

-(SGDecoderOptions *)decoderOptions
{
    return self->_decoderOptions;
}

#pragma mark - Control

- (BOOL)openWithError:(NSError *__autoreleasing  _Nullable *)error
{
    BOOL success = [self->_demuxer openWithError:error];
    if (success) {
        self->_selectedTracks = [self->_demuxer.tracks copy];
        self->_decoders = [[NSMutableDictionary alloc] init];
        self->_frameQueue = [[SGObjectQueue alloc] init];
        self->_frameQueue.shouldSortObjects = YES;
    }
    return success;
}

- (BOOL)closeWithError:(NSError *__autoreleasing  _Nullable *)error
{
    self->_decoders = nil;
    self->_frameQueue = nil;
    return [self->_demuxer closeWithError:error];
}

- (BOOL)seekToTime:(CMTime)time withError:(NSError *__autoreleasing  _Nullable * _Nullable)error
{
    return [self seekToTime:time toleranceBefore:kCMTimeInvalid toleranceAfter:kCMTimeInvalid withError:error];
}

- (BOOL)seekToTime:(CMTime)time toleranceBefore:(CMTime)toleranceBefor toleranceAfter:(CMTime)toleranceAfter withError:(NSError *__autoreleasing  _Nullable * _Nullable)error
{
    BOOL success = [self->_demuxer seekToTime:time toleranceBefore:toleranceBefor toleranceAfter:toleranceAfter withError:error];
    if (success) {
        for (id<SGDecodable> obj in self->_decoders.allValues) {
            [obj flush];
        }
        [self->_frameQueue flush];
        self->_flags.noMorePacket = NO;
    }
    return success;
}

- (BOOL)selectTracks:(NSArray<SGTrack *> *)tracks withError:(NSError *__autoreleasing  _Nullable * _Nullable)error
{
    self->_selectedTracks = [tracks copy];
    return YES;
}

- (BOOL)nextFrame:(__kindof SGFrame **)frame withError:(NSError *__autoreleasing  _Nullable * _Nullable)error
{
    NSError *err= nil;
    __kindof SGFrame *ret = nil;
    while (!ret && !err) {
        if ([self->_frameQueue getObjectAsync:&ret]) {
            continue;
        }
        if (self->_flags.noMorePacket) {
            err = SGCreateError(SGErrorCodeDemuxerEndOfFile, SGActionCodeNextFrame);
            continue;
        }
        SGPacket *packet = nil;
        [self->_demuxer nextPacket:&packet withError:NULL];
        NSArray<__kindof SGFrame *> *objs = nil;
        if (packet) {
            if (![self->_selectedTracks containsObject:packet.track]) {
                [packet unlock];
                continue;
            }
            id<SGDecodable> decoder = [self->_decoders objectForKey:@(packet.track.index)];
            if (!decoder) {
                if (packet.track.type == SGMediaTypeAudio) {
                    decoder = [[SGAudioDecoder alloc] init];
                }
                if (packet.track.type == SGMediaTypeVideo) {
                    decoder = [[SGVideoDecoder alloc] init];
                }
                if (decoder) {
                    decoder.options = self->_decoderOptions;
                    [self->_decoders setObject:decoder forKey:@(packet.track.index)];
                }
            }
            objs = [decoder decode:packet];
            [packet unlock];
        } else {
            NSMutableArray<__kindof SGFrame *> *mObjs = [NSMutableArray array];
            for (id<SGDecodable> decoder in self->_decoders.allValues) {
                [mObjs addObjectsFromArray:[decoder finish]];
            }
            objs = [mObjs copy];
            self->_flags.noMorePacket = YES;
        }
        for (id<SGData> obj in objs) {
            [self->_frameQueue putObjectSync:obj];
            [obj unlock];
        }
    }
    if (ret) {
        if (frame) {
            *frame = ret;
        } else {
            [ret unlock];
        }
    }
    if (err) {
        if (error) {
            *error = err;
        }
        return NO;
    }
    return YES;
}

#pragma mark - SGDemuxableDelegate

- (BOOL)demuxableShouldAbortBlockingFunctions:(id<SGDemuxable>)demuxable
{
    if ([self->_delegate respondsToSelector:@selector(demuxableShouldAbortBlockingFunctions:)]) {
        return [self->_delegate frameReaderShouldAbortBlockingFunctions:self];
    }
    return NO;
}

@end
