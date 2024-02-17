//
//  SGFrame.h
//  SGPlayer
//
//  Created by Single on 2018/1/18.
//  Copyright © 2018年 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CMTime.h>
#import <SGPlayer/SGTrack.h>
#import <SGPlayer/SGData.h>

NS_ASSUME_NONNULL_BEGIN

static int const SGFramePlaneCount = 8;

@interface SGFrame : NSObject <SGData>

/**
 * \c AVFrame pointer
 */
@property (NS_NONATOMIC_IOSONLY, readonly) void *coreptr;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, strong, readonly, nullable) SGTrack *track;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, strong, readonly) NSDictionary *metadata;

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
@property (NS_NONATOMIC_IOSONLY, readonly) CMTime decodeTimeStamp;

/**
 *
 */
@property (NS_NONATOMIC_IOSONLY, readonly) int size;

@end

NS_ASSUME_NONNULL_END
