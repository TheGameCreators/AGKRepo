/*
 * CHBInterstitial.h
 * Chartboost
 *
 * Copyright 2019 Chartboost. All rights reserved.
 */

#import "CHBAd.h"
#import "CHBAdDelegate.h"

NS_ASSUME_NONNULL_BEGIN

/*!
 @protocol CHBInterstitialDelegate
 @brief Interstitial delegate protocol that inherits from CHBAdDelegate.
 @discussion Provides methods to receive notifications related to an interstitial's actions and to control its behavior.
 In a typical integration you would implement willShowAd:, didShowAd: and didDismissAd:, pausing and resuming ongoing processes (e.g: gameplay, video) there.
 */
@protocol CHBInterstitialDelegate <CHBDismissableAdDelegate>
@end

/*!
 @class CHBInterstitial
 
 @brief CHBInterstitial is a full-screen ad.
 
 @discussion To show an interstitial it first needs to be cached. Trying to show an uncached intersitital will always fail, thus it is recommended to always check if the ad is cached first.
 
 You can create and cache as many interstitial ads as you want, but only one can be presented at the same time.
 
 A basic implementation would look like this:
 @code
 - (void)createAndCacheInterstitial {
    self.interstitial = [[CHBInterstitial alloc] initWithLocation:CBLocationDefault delegate:self];
    [self.interstitial cache];
 }
 
 - (void)showInterstitial {
    if (self.interstitial.isCached) {
        [self.interstitial showFromViewController:self];
    }
 }
 
 // Delegate methods
 
 - (void)didCacheAd:(CHBCacheEvent *)event error:(nullable CHBCacheError *)error {
    if (error) {
        // Handle error, possibly scheduling a retry
    }
 }
 
 - (void)willShowAd:(CHBShowEvent *)event {
    // Pause ongoing processes
 }
 
 - (void)didShowAd:(CHBShowEvent *)event error:(CHBShowError *)error {
    if (error) {
        // Resume paused processes
    }
 }
 
 - (void)didDismissAd:(CHBDismissEvent *)event {
    // Resume paused processes
 }
 @endcode
 
 If your application uses a View controller-based status bar appearance (usually the default), an ad shown with a valid view controller will hide the status bar. Otherwise it is your responsibility to hide it or not.
 
 For more information on integrating and using the Chartboost SDK please visit our help site documentation at https://help.chartboost.com
 */
@interface CHBInterstitial : NSObject <CHBAd>

/*!
 @brief Chartboost location for the ad.
 @discussion Used to obtain ads with increased performance.
 */
@property (nonatomic, readonly) CBLocation location;

/*!
 @brief The delegate instance to receive interstitial callbacks.
 */
@property (nonatomic, weak, nullable) id<CHBInterstitialDelegate> delegate;

/*!
 @brief Determines if a cached ad exists.
 @return YES if there is a cached ad, and NO if not.
 @discussion A return value of YES here indicates that it is safe to call the showFromViewController: method.
 Calling this method when this value is NO will cause the show request to fail with a CHBShowErrorCodeNoCachedAd error.
 */
@property (nonatomic, readonly) BOOL isCached;

/*!
 @brief The initializer for the interstitial object.
 @param location Location for the interstitial. See the location property documentation.
 @param delegate Delegate for the interstitial. See the delegate property documentation.
 */
- (instancetype)initWithLocation:(CBLocation)location delegate:(nullable id<CHBInterstitialDelegate>)delegate;

/*!
 @brief Please use initWithLocation:delegate: instead.
*/
- (instancetype)init NS_UNAVAILABLE;

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
 If no cached ad exists the request will fail with a CHBShowErrorCodeNoCachedAd error.
 It is highly recommended that a non-nil view controller is passed, as it is required for enhanced ad presentation and some features like opening links in an in-app web browser.
 Implement didShowAd:error: in your ad delegate to be notified of a show request result.
 */
- (void)showFromViewController:(nullable UIViewController *)viewController;

@end

NS_ASSUME_NONNULL_END
