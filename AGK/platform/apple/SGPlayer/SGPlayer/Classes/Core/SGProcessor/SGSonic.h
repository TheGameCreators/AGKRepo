//
//  SGSonic.h
//  SGPlayer
//
//  Created by Single on 2018/12/20.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGAudioDescriptor.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGSonic : NSObject

/**
 *
 */
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/**
 *
 */
- (instancetype)initWithDescriptor:(SGAudioDescriptor *)descriptor;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) SGAudioDescriptor *descriptor;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) float speed;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) float pitch;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) float rate;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) float volume;

/**
 *
 */
- (BOOL)open;

/**
 *
 */
- (int)flush;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) int samplesInput;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) int samplesAvailable;

/**
 *
 */
- (int)write:(uint8_t *_Nullable*_Nullable)data nb_samples:(int)nb_samples;

/**
 *
 */
- (int)read:(uint8_t *_Nullable*_Nonnull)data nb_samples:(int)nb_samples;

@end

NS_ASSUME_NONNULL_END
