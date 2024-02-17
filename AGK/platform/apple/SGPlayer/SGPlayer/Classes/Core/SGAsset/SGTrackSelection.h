//
//  SGTrackSelection.h
//  SGPlayer
//
//  Created by Single on 2019/8/13.
//  Copyright © 2019 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGTrack.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_OPTIONS(NSUInteger, SGTrackSelectionAction) {
    SGTrackSelectionActionTracks  = 1 << 0,
    SGTrackSelectionActionWeights = 1 << 1,
};

@interface SGTrackSelection : NSObject <NSCopying>

/*!
 @property tracks
 @abstract
    Provides array of \c SGTrackSelection tracks.
 */
@property (NS_NONATOMIC_IOSONLY, copy, nullable) NSArray<SGTrack *> *tracks;

/*!
 @property weights
 @abstract
    Provides array of \c SGTrackSelection weights.
 */
@property (NS_NONATOMIC_IOSONLY, copy, nullable) NSArray<NSNumber *> *weights;

@end

NS_ASSUME_NONNULL_END
