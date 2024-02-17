//
//  SGURLAsset.h
//  SGPlayer iOS
//
//  Created by Single on 2018/9/19.
//  Copyright © 2018 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGAsset.h>

NS_ASSUME_NONNULL_BEGIN

@interface SGURLAsset : SGAsset

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

/*!
 @method initWithURL:
 @abstract
    Initializes an SGURLAsset with the given URL.
 */
- (instancetype)initWithURL:(NSURL *)URL NS_DESIGNATED_INITIALIZER;

/*!
 @property URL
 @abstract
    Indicates the URL of the asset.
 */
@property (NS_NONATOMIC_IOSONLY, copy, readonly) NSURL *URL;

@end

NS_ASSUME_NONNULL_END
