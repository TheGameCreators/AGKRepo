//
//  SGObjectPool.h
//  SGPlayer
//
//  Created by Single on 2018/1/22.
//  Copyright © 2018年 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SGData.h"

NS_ASSUME_NONNULL_BEGIN

@interface SGObjectPool : NSObject

@property (class, strong, readonly) SGObjectPool *sharedPool;

/**
 *
 */
- (__kindof id<SGData>)objectWithClass:(Class)class reuseName:(NSString *)reuseName;

/**
 *
 */
- (void)comeback:(id<SGData>)object;

/**
 *
 */
- (void)flush;

@end

NS_ASSUME_NONNULL_END
