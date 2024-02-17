//
//  SGMotionSensor.m
//  SGPlayer iOS
//
//  Created by Single on 2018/8/23.
//  Copyright © 2018 single. All rights reserved.
//

#import "SGMotionSensor.h"
#import <UIKit/UIKit.h>
#import <CoreMotion/CoreMotion.h>
#import "SGVector.h"

@interface SGMotionSensor ()

@property (nonatomic) CGFloat defalutRotateY;
@property (nonatomic) simd_float4x4 deviceToDisplay;
@property (nonatomic) simd_float4x4 worldToInertialReferenceFrame;
@property (nonatomic, strong) CMMotionManager * manager;
@property (nonatomic) UIInterfaceOrientation orientation;

@end

@implementation SGMotionSensor

- (instancetype)init
{
    if (self = [super init]) {
        self.deviceToDisplay = matrix_identity_float4x4;
        self.manager = [[CMMotionManager alloc] init];
        self.worldToInertialReferenceFrame = [self getRotateEulerMatrixX:-90 Y:0 Z:90];
        switch ([UIApplication sharedApplication].statusBarOrientation) {
            case UIInterfaceOrientationPortrait:
            case UIInterfaceOrientationUnknown:
            case UIInterfaceOrientationPortraitUpsideDown:
                self.defalutRotateY = 0;
                break;
            case UIInterfaceOrientationLandscapeRight:
                self.defalutRotateY = -90;
                break;
            case UIInterfaceOrientationLandscapeLeft:
                self.defalutRotateY = 90;
                break;
        }
    }
    return self;
}

- (void)setOrientation:(UIInterfaceOrientation)orientation
{
    if (_orientation != orientation) {
        _orientation = orientation;
        switch (orientation) {
            case UIInterfaceOrientationPortrait:
            case UIInterfaceOrientationUnknown:
                self.deviceToDisplay = matrix_identity_float4x4;
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                self.deviceToDisplay = [self getRotateEulerMatrixX:0 Y:0 Z:180];
                break;
            case UIInterfaceOrientationLandscapeRight:
                self.deviceToDisplay = [self getRotateEulerMatrixX:0 Y:0 Z:-90];
                break;
            case UIInterfaceOrientationLandscapeLeft:
                self.deviceToDisplay = [self getRotateEulerMatrixX:0 Y:0 Z:90];
                break;
        }
    }
}

- (BOOL)isReady
{
    if (self.manager.isDeviceMotionAvailable) {
        return self.manager.deviceMotion && self.manager.isDeviceMotionActive;
    }
    return YES;
}

- (void)start
{
    if (!self.ready && !self.manager.isDeviceMotionActive) {
        self.manager.deviceMotionUpdateInterval = 0.01;
        [self.manager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXArbitraryZVertical];
    }
}

- (void)stop
{
    [self.manager stopDeviceMotionUpdates];
    // reset the manager.
    self.manager = [[CMMotionManager alloc] init];
}

- (simd_float4x4)matrix
{
    if (!self.manager.isDeviceMotionActive ||
        !self.manager.isDeviceMotionAvailable) {
        return matrix_identity_float4x4;
    }
    CMDeviceMotion * motion = self.manager.deviceMotion;
    if (!motion) {
        return matrix_identity_float4x4;
    }
    self.orientation = [UIApplication sharedApplication].statusBarOrientation;
    CMRotationMatrix rotationMatrix = motion.attitude.rotationMatrix;
    simd_float4x4 inertialReferenceFrameToDevice = simd_transpose([self simdMatrixFromRotationMatrix:rotationMatrix]);
    simd_float4x4 worldToDevice = matrix_multiply(inertialReferenceFrameToDevice, self.worldToInertialReferenceFrame);
    simd_float4x4 worldToDisplay = matrix_multiply(self.deviceToDisplay, worldToDevice);
    simd_float4x4 ret = SGMatrix4x4RotateY(worldToDisplay, SGDegreesToRadians(self.defalutRotateY));
    return ret;
}

- (simd_float4x4)simdMatrixFromRotationMatrix:(CMRotationMatrix)rotationMatrix
{
    simd_float4x4 glRotationMatrix;
    
    glRotationMatrix.columns[0][0] = rotationMatrix.m11;
    glRotationMatrix.columns[0][1] = rotationMatrix.m12;
    glRotationMatrix.columns[0][2] = rotationMatrix.m13;
    glRotationMatrix.columns[0][3] = 0.0f;
    
    glRotationMatrix.columns[1][0] = rotationMatrix.m21;
    glRotationMatrix.columns[1][1] = rotationMatrix.m22;
    glRotationMatrix.columns[1][2] = rotationMatrix.m23;
    glRotationMatrix.columns[1][3] = 0.0f;
    
    glRotationMatrix.columns[2][0] = rotationMatrix.m31;
    glRotationMatrix.columns[2][1] = rotationMatrix.m32;
    glRotationMatrix.columns[2][2] = rotationMatrix.m33;
    glRotationMatrix.columns[2][3] = 0.0f;
    
    glRotationMatrix.columns[3][0] = 0.0f;
    glRotationMatrix.columns[3][1] = 0.0f;
    glRotationMatrix.columns[3][2] = 0.0f;
    glRotationMatrix.columns[3][3] = 1.0f;
    
    return glRotationMatrix;
}

- (simd_float4x4)getRotateEulerMatrixX:(CGFloat)x Y:(CGFloat)y Z:(CGFloat)z
{
    x *= (float)(M_PI / 180.0f);
    y *= (float)(M_PI / 180.0f);
    z *= (float)(M_PI / 180.0f);
    float cx = (float) cos(x);
    float sx = (float) sin(x);
    float cy = (float) cos(y);
    float sy = (float) sin(y);
    float cz = (float) cos(z);
    float sz = (float) sin(z);
    float cxsy = cx * sy;
    float sxsy = sx * sy;
    simd_float4x4 matrix;
    matrix.columns[0].x = cy * cz;
    matrix.columns[0].y = -cy * sz;
    matrix.columns[0].z = sy;
    matrix.columns[0].w = 0.0f;
    matrix.columns[1].x = cxsy * cz + cx * sz;
    matrix.columns[1].y = -cxsy * sz + cx * cz;
    matrix.columns[1].z = -sx * cy;
    matrix.columns[1].w = 0.0f;
    matrix.columns[2].x = -sxsy * cz + sx * sz;
    matrix.columns[2].y = sxsy * sz + sx * cz;
    matrix.columns[2].z = cx * cy;
    matrix.columns[2].w = 0.0f;
    matrix.columns[3].x = 0.0f;
    matrix.columns[3].y = 0.0f;
    matrix.columns[3].z = 0.0f;
    matrix.columns[3].w = 1.0f;
    return matrix;
}

@end

@implementation SGMotionSensor (deprecated)

- (BOOL)ready
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSLog(@"The function -[SGMotionSensor ready] was called inside your code. make sure you are calling -[SGMotionSensor isReady] instead. This will only be displayed once.");
    });
    return self.isReady;
}

@end
