//
//  SGSWResample.h
//  SGPlayer iOS
//
//  Created by Single on 2018/10/30.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGAudioDescriptor.h>

@interface SGSWResample : NSObject

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, nullable) SGAudioDescriptor *inputDescriptor;
@property (NS_NONATOMIC_IOSONLY, copy, nullable) SGAudioDescriptor *outputDescriptor;

/**
 *
 */
- (BOOL)open;

/**
 *
 */
- (int)write:(uint8_t *_Nullable*_Nullable)data nb_samples:(int)nb_samples;

/**
 *
 */
- (int)read:(uint8_t *_Nullable*_Nonnull)data nb_samples:(int)nb_samples;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) int delay;

@end
