//
//  SGMutableTrack.h
//  SGPlayer
//
//  Created by Single on 2018/11/29.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGSegment.h>
#import <SGPlayer/SGTrack.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGMutableTrack : SGTrack

- (instancetype)initWithType:(SGMediaType)type index:(NSInteger)index NS_DESIGNATED_INITIALIZER;

/*!
 @property subTracks
 @abstract
    Indicates the sub tracks.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGTrack *> *subTracks;

/*!
 @property segments
 @abstract
    Provides array of SGMutableTrack segments.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGSegment *> *segments;

/*!
 @method appendSegment:
 @abstract
    Append a segment to the track.
 */
- (BOOL)appendSegment:(SGSegment *)segment;

@end

NS_ASSUME_NONNULL_END
