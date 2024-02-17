//
//  SGMetalProjection.h
//  SGPlayer
//
//  Created by Single on 2019/6/28.
//  Copyright © 2019 single. All rights reserved.
//

#import <Metal/Metal.h>
#include <simd/matrix_types.h>

@interface SGMetalProjection : NSObject

- (instancetype)initWithDevice:(id<MTLDevice>)device;

@property (nonatomic) simd_float4x4 matrix;
@property (NS_NONATOMIC_IOSONLY, strong) id<MTLDevice> device;
@property (NS_NONATOMIC_IOSONLY, strong) id<MTLBuffer> matrixBuffer;

@end
