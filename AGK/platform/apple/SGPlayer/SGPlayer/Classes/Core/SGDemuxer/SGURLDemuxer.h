//
//  SGURLDemuxer.h
//  SGPlayer iOS
//
//  Created by Single on 2018/8/13.
//  Copyright © 2018 single. All rights reserved.
//

#import <SGPlayer/SGDemuxable.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGURLDemuxer : NSObject <SGDemuxable>

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/*!
 @method initWithURL:
 @abstract
    Initializes an SGURLDemuxer with an NSURL.
 */
- (instancetype)initWithURL:(NSURL *)URL NS_DESIGNATED_INITIALIZER;

/*!
 @property URL
 @abstract
    Indicates the URL of the demuxer.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSURL *URL;

@end

NS_ASSUME_NONNULL_END
