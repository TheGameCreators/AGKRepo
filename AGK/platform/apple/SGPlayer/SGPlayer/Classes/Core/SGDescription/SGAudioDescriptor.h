//
//  SGAudioDescriptor.h
//  SGPlayer
//
//  Created by Single on 2018/11/28.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGAudioDescriptor : NSObject <NSCopying>

/*!
 @property format
 @abstract
    Indicates the audio format.
 
 @discussion
    The value corresponds to AVSampleFormat.
 */
@property (nonatomic) int format;

/*!
 @property sampleRate
 @abstract
    Indicates the sample rate.
 */
@property (nonatomic) int sampleRate;

/*!
 @property numberOfChannels
 @abstract
    Indicates the channel count.
 */
@property (nonatomic) int numberOfChannels;

/*!
 @property channelLayout
 @abstract
    Indicates the channel layout.
 */
@property (nonatomic) uint64_t channelLayout;

/*!
 @property isPlanar
 @abstract
    Get whether the sample format is planar.
 */
@property (readonly, getter=isPlanar) BOOL planar;

/*!
 @@property bytesPerSample
 @abstract
    Get the bytes per sample.
 */
@property (readonly) int bytesPerSample;

/*!
 @@property numberOfPlanes
 @abstract
    Get the number of planes.
 */
@property (readonly) int numberOfPlanes;

/*!
 @method linesize:
 @abstract
    Get the linesize of the number of samples.
 */
- (int)linesize:(int)numberOfSamples;

/*!
 @method isEqualToDescriptor:
 @abstract
    Check if the descriptor is equal to another.
 */
- (BOOL)isEqualToDescriptor:(nullable SGAudioDescriptor *)descriptor;

@end

NS_ASSUME_NONNULL_END
