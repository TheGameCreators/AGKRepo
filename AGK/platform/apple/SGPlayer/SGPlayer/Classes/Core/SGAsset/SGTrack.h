//
//  SGTrack.h
//  SGPlayer
//
//  Created by Single on 2018/1/17.
//  Copyright © 2018年 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGDefines.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGTrack : NSObject <NSCopying>

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/*!
 @property coreptr
 @abstract
    Indicates the pointer to the AVStream.
 */
@property (NS_NONATOMIC_IOSONLY, readonly, nullable) void *coreptr;

/*!
 @property type
 @abstract
    Indicates the track media type.
 */
@property (NS_NONATOMIC_IOSONLY, readonly) SGMediaType type;

/*!
 @property type
 @abstract
    Indicates the track index.
 */
@property (NS_NONATOMIC_IOSONLY, readonly) NSInteger index;

/*!
 @method trackWithTracks:type:
 @abstract
   Get track with media type.
*/
+ (nullable SGTrack *)trackWithTracks:(NSArray<SGTrack *> *)tracks type:(SGMediaType)type NS_SWIFT_NAME(track(with:type:));

/*!
 @method trackWithTracks:index:
 @abstract
   Get track with index.
*/
+ (nullable SGTrack *)trackWithTracks:(NSArray<SGTrack *> *)tracks index:(NSInteger)index NS_SWIFT_NAME(track(with:index:));

/*!
 @method tracksWithTracks:type:
 @abstract
   Get tracks with media types.
*/
+ (nullable NSArray<SGTrack *> *)tracksWithTracks:(NSArray<SGTrack *> *)tracks type:(SGMediaType)type;

@end

NS_ASSUME_NONNULL_END
