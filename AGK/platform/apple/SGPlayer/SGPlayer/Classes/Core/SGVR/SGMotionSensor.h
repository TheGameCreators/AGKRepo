//
//  SGMotionSensor.h
//  SGPlayer iOS
//
//  Created by Single on 2018/8/23.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <simd/matrix_types.h>

@interface SGMotionSensor : NSObject

@property (nonatomic, readonly, getter=isReady) BOOL ready;
@property (nonatomic, readonly) simd_float4x4 matrix;

- (void)start;
- (void)stop;

@end
