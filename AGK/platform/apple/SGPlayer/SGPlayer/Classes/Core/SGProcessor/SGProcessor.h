//
//  SGProcessor.h
//  SGPlayer
//
//  Created by Single on 2019/8/13.
//  Copyright © 2019 single. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SGPlayer/SGTrackSelection.h>
#import <SGPlayer/SGCapacity.h>
#import <SGPlayer/SGFrame.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SGProcessor <NSObject>

/**
 *
 */
- (void)setSelection:(SGTrackSelection *)selection action:(SGTrackSelectionAction)action;

/**
 *
 */
- (nullable __kindof SGFrame *)putFrame:(__kindof SGFrame *)frame;

/**
 *
 */
- (nullable __kindof SGFrame *)finish;

/**
 *
 */
@property (readonly) SGCapacity capacity;

/**
 *
 */
- (void)flush;

/**
 *
 */
- (void)close;

@end

NS_ASSUME_NONNULL_END
