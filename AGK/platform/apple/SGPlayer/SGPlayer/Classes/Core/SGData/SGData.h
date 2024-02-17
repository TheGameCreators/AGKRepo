//
//  SGDemuxable.h
//  SGPlayer iOS
//
//  Created by Single on 2018/9/25.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

NS_ASSUME_NONNULL_BEGIN

/**
 *
 */
typedef NS_OPTIONS(NSUInteger, SGDataFlags) {
    SGDataFlagPadding = 1 << 0,
};

@protocol SGData <NSObject>

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY) SGDataFlags flags;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, copy) NSString *reuseName;

/**
 *
 */
- (void)lock;

/**
 *
 */
- (void)unlock;

/**
 *
 */
- (void)clear;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime duration;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime timeStamp;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) int size;

@end

NS_ASSUME_NONNULL_END
