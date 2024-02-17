//
//  SGExtractingDemuxer.h
//  SGPlayer
//
//  Created by Single on 2018/11/14.
//  Copyright © 2018 single. All rights reserved.
//

#import "SGDemuxable.h"

NS_ASSUME_NONNULL_BEGIN

@interface SGExtractingDemuxer : NSObject <SGDemuxable>

/**
 *
 */
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/**
 *
 */
- (instancetype)initWithDemuxable:(id<SGDemuxable>)demuxable index:(NSInteger)index timeRange:(CMTimeRange)timeRange scale:(CMTime)scale;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, strong, readonly) id<SGDemuxable> demuxable;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) NSInteger index;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTimeRange timeRange;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime scale;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) BOOL overgop;

@end

NS_ASSUME_NONNULL_END
