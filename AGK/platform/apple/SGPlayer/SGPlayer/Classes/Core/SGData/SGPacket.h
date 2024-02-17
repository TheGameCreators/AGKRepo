//
//  SGPacket.h
//  SGPlayer
//
//  Created by Single on 2018/1/22.
//  Copyright © 2018年 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGTrack.h>
#import <SGPlayer/SGData.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGPacket : NSObject <SGData>

/**
 * \c AVPacket pointer
 */
@property (nonatomic, readonly) void *coreptr;

/**
 *
 */
@property (nonatomic, strong, readonly, nullable) SGTrack *track;

/**
 *
 */
@property (nonatomic, strong, readonly) NSDictionary *metadata;

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
