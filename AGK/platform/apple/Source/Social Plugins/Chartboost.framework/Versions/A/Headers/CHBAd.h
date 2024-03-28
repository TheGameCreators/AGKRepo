/*
 * CHBAd.h
 * Chartboost
 *
 * Copyright 2019 Chartboost. All rights reserved.
 */

#import <UIKit/UIKit.h>
#import "ChartboostDelegate.h"

@protocol CHBAdDelegate;

NS_ASSUME_NONNULL_BEGIN

/*!
 @protocol CHBAd
 @brief The protocol to which all Chartboost ads conform to.
 @discussion Provides an interface for basic functionalities common to all ads.
 */
@protocol CHBAd <NSObject>

/*!
 @brief The delegate instance to receive ad callbacks.
 */
@property (nonatomic, weak, nullable) id<CHBAdDelegate> delegate;

/*!
 @brief Chartboost location for the ad.
 @discussion Used to obtain ads with increased performance.
 */
@property (nonatomic, readonly) CBLocation location;

/*!
 @brief Determines if a cached ad exists.
 @return YES if there is a cached ad, and NO if not.
 */
@property (nonatomic, readonly) BOOL isCached;

/*!
 @brief Caches an ad.
 @discussion This method will first check if there is a cached ad and, if found, will do nothing.
 If no cached ad exists the method will attempt to fetch it from the Chartboost server.
 Implement didCacheAd:error: in your ad delegate to be notified of a cache request result.
 */
- (void)cache;

/*!
 @brief Shows an ad.
 @param viewController The view controller to present the ad on.
 @discussion This method will first check if there is a cached ad, if found it will present it.
 It is highly recommended that a non-nil view controller is passed, as it is required for enhanced ad presentation and some features like opening links in an in-app web browser.
 */
- (void)showFromViewController:(nullable UIViewController *)viewController;

@end

NS_ASSUME_NONNULL_END
