//
//  SGMetalRenderPipeline.h
//  MetalTest
//
//  Created by Single on 2019/6/26.
//  Copyright © 2019 Single. All rights reserved.
//

#import <Metal/Metal.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGMetalRenderPipeline : NSObject

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithDevice:(id<MTLDevice>)device library:(id<MTLLibrary>)library NS_DESIGNATED_INITIALIZER;

@property (NS_NONATOMIC_IOSONLY, strong) id<MTLDevice> device;
@property (NS_NONATOMIC_IOSONLY, strong) id<MTLLibrary> library;
@property (NS_NONATOMIC_IOSONLY, strong, nullable) id<MTLRenderPipelineState> state;
@property (NS_NONATOMIC_IOSONLY, strong, nullable) MTLRenderPipelineDescriptor *descriptor;

@end

NS_ASSUME_NONNULL_END
