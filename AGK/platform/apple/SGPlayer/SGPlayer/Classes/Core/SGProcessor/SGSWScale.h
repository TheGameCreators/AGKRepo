//
//  SGSWScale.h
//  SGPlayer iOS
//
//  Created by Single on 2018/8/28.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGVideoDescriptor.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGSWScale : NSObject

/**
 *
 */
+ (BOOL)isSupportedInputFormat:(int)format;

/**
 *
 */
+ (BOOL)isSupportedOutputFormat:(int)format;

/**
 *
 */
@property (nonatomic, copy) SGVideoDescriptor *inputDescriptor;

/**
 *
 */
@property (nonatomic, copy) SGVideoDescriptor *outputDescriptor;

/**
 *
 */
@property (nonatomic) int flags;          // SWS_FAST_BILINEAR

/**
 *
 */
- (BOOL)open;

/**
 *
 */
- (int)convert:(const uint8_t *_Nullable const [_Nonnull])inputData inputLinesize:(const int[_Nonnull])inputLinesize outputData:(uint8_t *_Nullable const [_Nonnull])outputData outputLinesize:(const int[_Nonnull])outputLinesize;

@end

NS_ASSUME_NONNULL_END
