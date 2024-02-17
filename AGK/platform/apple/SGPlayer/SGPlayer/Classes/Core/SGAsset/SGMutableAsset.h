//
//  SGMutableAsset.h
//  SGPlayer
//
//  Created by Single on 2018/11/14.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGAsset.h>
#import <SGPlayer/SGDefines.h>
#import <SGPlayer/SGMutableTrack.h>

@interface SGMutableAsset : SGAsset

- (instancetype)init NS_DESIGNATED_INITIALIZER;

/*!
 @property tracks
 @abstract
    Provides array of mutable asset tracks.
*/
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGMutableTrack *> *tracks;

/*!
 @method addTrack:
 @abstract
    Add a track to the asset.
 
 @discussion
    Returns a initialized mutable track of the given type.
 */
- (SGMutableTrack *)addTrack:(SGMediaType)type;

@end
