//
//  SGMetalProjection.m
//  SGPlayer
//
//  Created by Single on 2019/6/28.
//  Copyright © 2019 single. All rights reserved.
//

#import "SGMetalProjection.h"
#import "SGMetalTypes.h"

@implementation SGMetalProjection

- (instancetype)initWithDevice:(id<MTLDevice>)device
{
    if (self = [super init]) {
        self.device = device;
        self.matrixBuffer = [device newBufferWithLength:sizeof(SGMetalMatrix) options:MTLResourceStorageModeShared];
    }
    return self;
}

- (void)setMatrix:(simd_float4x4)matrix
{
    self->_matrix = matrix;
    ((SGMetalMatrix *)self.matrixBuffer.contents)->mvp = matrix;
}

@end
