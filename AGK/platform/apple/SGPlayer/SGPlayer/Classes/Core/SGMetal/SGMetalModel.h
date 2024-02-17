//
//  SGMetalModel.h
//  MetalTest
//
//  Created by Single on 2019/6/24.
//  Copyright © 2019 Single. All rights reserved.
//

#import <Metal/Metal.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGMetalModel : NSObject

- (instancetype)initWithDevice:(id<MTLDevice>)device;

@property (NS_NONATOMIC_IOSONLY) NSUInteger indexCount;
@property (NS_NONATOMIC_IOSONLY) MTLIndexType indexType;
@property (NS_NONATOMIC_IOSONLY) MTLPrimitiveType primitiveType;
@property (NS_NONATOMIC_IOSONLY, strong) id<MTLDevice> device;
@property (NS_NONATOMIC_IOSONLY, strong, nullable) id<MTLBuffer> indexBuffer;
@property (NS_NONATOMIC_IOSONLY, strong, nullable) id<MTLBuffer> vertexBuffer;

@end

NS_ASSUME_NONNULL_END
