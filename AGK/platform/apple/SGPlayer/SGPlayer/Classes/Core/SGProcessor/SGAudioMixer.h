//
//  SGAudioMixer.h
//  SGPlayer
//
//  Created by Single on 2018/11/29.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGAudioDescriptor.h>
#import <SGPlayer/SGAudioFrame.h>
#import <SGPlayer/SGCapacity.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGAudioMixer : NSObject

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/**
 *
 */
- (instancetype)initWithTracks:(NSArray<SGTrack *> *)tracks weights:(NSArray<NSNumber *> *)weights;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSArray<SGTrack *> *tracks;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy) NSArray<NSNumber *> *weights;

/**
 *
 */
- (nullable SGAudioFrame *)putFrame:(SGAudioFrame *)frame;

/**
 *
 */
- (nullable SGAudioFrame *)finish;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) SGCapacity capacity;

/**
 *
 */
- (void)flush;

@end

NS_ASSUME_NONNULL_END
