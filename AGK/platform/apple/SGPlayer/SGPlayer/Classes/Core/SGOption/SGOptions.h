//
//  SGOptions.h
//  SGPlayer iOS
//
//  Created by Single on 2018/10/29.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGProcessorOptions.h>
#import <SGPlayer/SGDecoderOptions.h>
#import <SGPlayer/SGDemuxerOptions.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGOptions : NSObject <NSCopying>

/*!
 @property sharedOptions
 @abstract
    Globally shared configuration options.
 */
@property (class, strong, readonly) SGOptions *sharedOptions;

/*!
 @property demuxer
 @abstract
    The options for demuxer.
 */
@property (NS_NONATOMIC_IOSONLY, strong) SGDemuxerOptions *demuxer;

/*!
 @property decoder
 @abstract
    The options for decoder.
 */
@property (NS_NONATOMIC_IOSONLY, strong) SGDecoderOptions *decoder;

/*!
 @property processor
 @abstract
    The options for processor.
 */
@property (NS_NONATOMIC_IOSONLY, strong) SGProcessorOptions *processor;

@end

NS_ASSUME_NONNULL_END
