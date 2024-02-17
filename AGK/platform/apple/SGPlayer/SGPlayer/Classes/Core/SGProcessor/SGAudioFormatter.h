//
//  SGAudioFormatter.h
//  SGPlayer iOS
//
//  Created by Single on 2018/10/30.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGAudioDescriptor.h>
#import <SGPlayer/SGAudioFrame.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGAudioFormatter : NSObject

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy, nullable) SGAudioDescriptor *descriptor;

/**
 *
 */
- (nullable SGAudioFrame *)format:(SGAudioFrame *)frame;

/**
 *
 */
- (nullable SGAudioFrame *)finish;

/**
 *
 */
- (void)flush;

@end

NS_ASSUME_NONNULL_END
