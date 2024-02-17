//
//  SGDecoderOptions.h
//  SGPlayer
//
//  Created by Single on 2019/6/14.
//  Copyright © 2019 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGAudioDescriptor.h>
#import <SGPlayer/SGTime.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGDecoderOptions : NSObject <NSCopying>

/*!
 @property options
 @abstract
    The options for avformat_open_input.
    Default is nil.
 */
@property (NS_NONATOMIC_IOSONLY, copy, nullable) NSDictionary<NSString*,id> *options;

/*!
 @property threadsAuto
 @abstract
    The option for avformat_open_input.
    Default is YES.
 */
@property (NS_NONATOMIC_IOSONLY) BOOL threadsAuto;

/*!
 @property refcountedFrames
 @abstract
    The option for avformat_open_input.
    Default is YES.
 */
@property (NS_NONATOMIC_IOSONLY) BOOL refcountedFrames;

/*!
 @property hardwareDecodeH264
 @abstract
    Indicates whether hardware decoding is enabled for H264.
    Default is YES.
 */
@property (NS_NONATOMIC_IOSONLY) BOOL hardwareDecodeH264;

/*!
 @property hardwareDecodeH265
 @abstract
    Indicates whether hardware decoding is enabled for H265.
    Default is YES.
 */
@property (NS_NONATOMIC_IOSONLY) BOOL hardwareDecodeH265;

/*!
 @property preferredPixelFormat
 @abstract
    Indicates the default hardware decoding output format.
    Default is kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange.
 */
@property (NS_NONATOMIC_IOSONLY) OSType preferredPixelFormat;

/*!
 @property supportedPixelFormats
 @abstract
    Indicates the supported pixel formats.
 */
@property (NS_NONATOMIC_IOSONLY, copy) NSArray<NSNumber *> *supportedPixelFormats;

/*!
 @property supportedAudioDescriptors
 @abstract
    Indicates the supported audio descriptors.
 */
@property (NS_NONATOMIC_IOSONLY, copy) NSArray<SGAudioDescriptor *> *supportedAudioDescriptors;

/*!
 @property resetFrameRate
 @abstract
    Indicates whether video decoder needs reset frame rate.
    Default is NO.
 */
@property (NS_NONATOMIC_IOSONLY) BOOL resetFrameRate;

/*!
 @property preferredFrameRate
 @abstract
    Indicates the preferred video track frame rate.
    Default is (1, 25).
 */
@property (NS_NONATOMIC_IOSONLY) CMTime preferredFrameRate;

@end

NS_ASSUME_NONNULL_END
