//
//  SGFrameReader.h
//  SGPlayer
//
//  Created by Single on 2019/11/12.
//  Copyright © 2019 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGDemuxerOptions.h>
#import <SGPlayer/SGDecoderOptions.h>
#import <SGPlayer/SGAsset.h>
#import <SGPlayer/SGFrame.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SGFrameReaderDelegate;

@interface SGFrameReader : NSObject

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
@property (NS_NONATOMIC_IOSONLY, weak) id<SGFrameReaderDelegate> delegate;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGTrack *> *tracks;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly, nullable) NSArray<SGTrack *> *selectedTracks;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly, nullable) NSDictionary *metadata;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime duration;

/**
 *
 */
- (BOOL)openWithError:(NSError**)error;

/**
 *
 */
- (BOOL)closeWithError:(NSError**)error;

/**
 *
 */
- (BOOL)seekableWithError:(NSError**)error;

/**
 *
 */
- (BOOL)seekToTime:(CMTime)time withError:(NSError**)error;

/**
 *
 */
- (BOOL)seekToTime:(CMTime)time toleranceBefore:(CMTime)toleranceBefore toleranceAfter:(CMTime)toleranceAfter withError:(NSError**)error;

/**
 *
 */
- (BOOL)selectTracks:(NSArray<SGTrack *> *)tracks withError:(NSError**)error;

/**
 *
 */
- (BOOL)nextFrame:(__kindof SGFrame *_Nullable*_Nullable)frame withError:(NSError**)error;

@end

@protocol SGFrameReaderDelegate <NSObject>

/**
 *
 */
- (BOOL)frameReaderShouldAbortBlockingFunctions:(SGFrameReader *)frameReader;

@end

NS_ASSUME_NONNULL_END
