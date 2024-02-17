//
//  SGRenderTimer.h
//  SGPlayer
//
//  Created by Single on 2019/6/28.
//  Copyright © 2019 single. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SGRenderTimer : NSObject

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

- (instancetype)initWithHandler:(dispatch_block_t)handler NS_DESIGNATED_INITIALIZER;

@property (nonatomic) NSTimeInterval timeInterval;
@property (nonatomic, getter=isPaused) BOOL paused;

- (void)start;
- (void)stop;

@end
