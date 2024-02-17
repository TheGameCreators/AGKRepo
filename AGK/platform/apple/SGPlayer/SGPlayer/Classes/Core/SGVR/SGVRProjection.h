//
//  SGVRProjection.h
//  SGPlayer iOS
//
//  Created by Single on 2018/8/23.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <simd/matrix_types.h>
#import "SGVRViewport.h"

@interface SGVRProjection : NSObject

@property (NS_NONATOMIC_IOSONLY, strong) SGVRViewport * viewport;

@property (NS_NONATOMIC_IOSONLY, readonly, getter=isReady) BOOL ready;
- (BOOL)matrixWithAspect:(Float64)aspect matrix1:(simd_float4x4 *)matrix1;
- (BOOL)matrixWithAspect:(Float64)aspect matrix1:(simd_float4x4 *)matrix1 matrix2:(simd_float4x4 *)matrix2;

@end

