//
//  SGPLFImage.h
//  SGPlatform
//
//  Created by Single on 2017/2/24.
//  Copyright © 2017年 single. All rights reserved.
//

#import <SGPlayer/SGPLFObject.h>
#import <CoreVideo/CoreVideo.h>
#import <CoreImage/CoreImage.h>

#if SGPLATFORM_TARGET_OS_MAC

typedef NSImage SGPLFImage;

#elif SGPLATFORM_TARGET_OS_IPHONE_OR_TV

typedef UIImage SGPLFImage;

#endif

NS_ASSUME_NONNULL_BEGIN

__BEGIN_DECLS

SGPLFImage * SGPLFImageWithCGImage(CGImageRef image);

// CVPixelBufferRef
SGPLFImage *_Nullable SGPLFImageWithCVPixelBuffer(CVPixelBufferRef pixelBuffer);
CIImage *_Nullable SGPLFImageCIImageWithCVPixelBuffer(CVPixelBufferRef pixelBuffer);
CGImageRef _Nullable SGPLFImageCGImageWithCVPixelBuffer(CVPixelBufferRef pixelBuffer) CF_RETURNS_RETAINED;

// RGB data buffer
SGPLFImage *_Nullable SGPLFImageWithRGBData(uint8_t *rgb_data, int linesize, int width, int height);
CGImageRef _Nullable SGPLFImageCGImageWithRGBData(uint8_t *rgb_data, int linesize, int width, int height) CF_RETURNS_RETAINED;

__END_DECLS

NS_ASSUME_NONNULL_END
