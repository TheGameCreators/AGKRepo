//
//  SGPlayerItem.h
//  SGPlayer
//
//  Created by Single on 2018/1/16.
//  Copyright © 2018年 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGTrackSelection.h>
#import <SGPlayer/SGAsset.h>
#import <SGPlayer/SGTrack.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGPlayerItem : NSObject

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/*!
 @method initWithAsset:
 @abstract
    Initializes an SGPlayerItem with asset.
 */
- (instancetype)initWithAsset:(SGAsset *)asset NS_DESIGNATED_INITIALIZER;

/*!
 @property error
 @abstract
    If the loading item failed, this describes the error that caused the failure.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly, nullable) NSError *error;

/*!
 @property tracks
 @abstract
    Provides array of SGPlayerItem tracks.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGTrack *> *tracks;

/*!
 @property duration
 @abstract
    Indicates the metadata of the item.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSDictionary *metadata;

/*!
 @property duration
 @abstract
    Indicates the duration of the item.
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime duration;

/*!
 @property duration
 @abstract
    Indicates the audioSelection of the item.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly, nullable) SGTrackSelection *audioSelection;

/*!
 @method setAudioSelection:action:
 @abstract
    Select specific audio tracks.
 */
- (void)setAudioSelection:(nullable SGTrackSelection *)audioSelection action:(SGTrackSelectionAction)action;

/*!
 @property duration
 @abstract
    Indicates the videoSelection of the item.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly, nullable) SGTrackSelection *videoSelection;

/*!
 @method setVideoSelection:action:
 @abstract
    Select specific video tracks.
 */
- (void)setVideoSelection:(nullable SGTrackSelection *)videoSelection action:(SGTrackSelectionAction)action;

@end

NS_ASSUME_NONNULL_END
