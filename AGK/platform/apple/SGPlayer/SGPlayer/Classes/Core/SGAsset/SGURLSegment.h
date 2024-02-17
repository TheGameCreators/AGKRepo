//
//  SGURLSegment.h
//  SGPlayer
//
//  Created by Single on 2018/11/14.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CMTime.h>
#import <CoreMedia/CMTimeRange.h>
#import <SGPlayer/SGSegment.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGURLSegment : SGSegment

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/*!
 @method initWithURL:index:timeRange:scale:
 @abstract
    Initializes an SGURLSegment.
 */
- (instancetype)initWithURL:(NSURL *)URL index:(NSInteger)index timeRange:(CMTimeRange)timeRange scale:(CMTime)scale NS_DESIGNATED_INITIALIZER;

/*!
 @property URL
 @abstract
    Indicates the URL of the segment.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSURL *URL;

/*!
 @property type
 @abstract
    Indicates the index.
 */
@property (NS_NONATOMIC_IOSONLY, readonly) NSInteger index;

/*!
 @property timeRange
 @abstract
    Indicates the timeRange of the segment.
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTimeRange timeRange;

/*!
 @property scale
 @abstract
    Indicates the scale of the segment.
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime scale;

@end

NS_ASSUME_NONNULL_END
