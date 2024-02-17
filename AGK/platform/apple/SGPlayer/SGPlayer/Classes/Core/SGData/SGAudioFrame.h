//
//  SGAudioFrame.h
//  SGPlayer
//
//  Created by Single on 2018/1/19.
//  Copyright © 2018年 single. All rights reserved.
//

#import <SGPlayer/SGFrame.h>
#import <SGPlayer/SGAudioDescriptor.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGAudioFrame : SGFrame

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, strong, readonly) SGAudioDescriptor *descriptor;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) int numberOfSamples;

/**
 *
 */
@property (readonly) int *linesize NS_RETURNS_INNER_POINTER;

/**
 *
 */
@property (readonly) uint8_t *_Nullable*_Nonnull data NS_RETURNS_INNER_POINTER;

@end

NS_ASSUME_NONNULL_END
