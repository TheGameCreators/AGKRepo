//
//  SGMetalYUVRenderPipeline.h
//  MetalTest
//
//  Created by Single on 2019/6/26.
//  Copyright © 2019 Single. All rights reserved.
//

#import "SGMetalRenderPipeline.h"

NS_ASSUME_NONNULL_BEGIN

@interface SGMetalYUVRenderPipeline : SGMetalRenderPipeline

- (instancetype)initWithDevice:(id<MTLDevice>)device library:(id<MTLLibrary>)library NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END
