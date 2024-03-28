/*
 * CHBBanner.h
 * Chartboost
 *
 * Copyright 2019 Chartboost. All rights reserved.
 */

#import "CHBAd.h"
#import "CHBAdDelegate.h"

NS_ASSUME_NONNULL_BEGIN

/*!
 @typedef CHBBannerSize
 @brief Defines standard sizes for CHBBanner objects.
 @discussion Standard sizes used to describe default banner bounds.
 */
typedef CGSize const CHBBannerSize;
/*! "Banner" - Standard banner size on phones. */
FOUNDATION_EXPORT CHBBannerSize const CHBBannerSizeStandard;
/*! "Medium Rect" - Medium banner size on phones. */
FOUNDATION_EXPORT CHBBannerSize const CHBBannerSizeMedium;
/*! "Tablet" - Leaderboard banner size on tablets. */
FOUNDATION_EXPORT CHBBannerSize const CHBBannerSizeLeaderboard;

/*!
 @protocol CHBBannerDelegate
 @brief Banner delegate protocol that inherits from CHBAdDelegate.
 @discussion Provides methods to receive notifications related to a banner's actions and to control its behavior.
 */
@protocol CHBBannerDelegate <CHBAdDelegate>
@end

/*!
 @class CHBBanner
 
 @brief CHBBanner is a UIView subclass able to show banner ads. Once obtained via the provided initializer, the
 developer is responsible of adding it to the app's view hierarchy and laying it out properly.
 
  @discussion A banner ad has a CHBBannerSize which corresponds to the view's intrinsicContentSize.
 The developer should not give the banner view object a frame size different than its CHBBannerSize property.
 If you are using AutoLayout just give the banner an X and Y position constraint and it will be automatically sized.
 
 By default a banner will automatically update its content on its own. This means you only need to call showFromViewController: once and the banner will get new ads and show them, gracefully handling errors if they occur.
 
 You can create and present as many banners as you want at the same time.
 
 A typical implementation would look like this:
 @code
 - (void)createAndShowBanner {
    CHBBanner *banner = [[CHBBanner alloc] initWithSize:CHBBannerSizeStandard location:CBLocationDefault delegate:self];
    banner.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:banner];
    [NSLayoutConstraint activateConstraints:@[[banner.bottomAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.bottomAnchor],
                                              [banner.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor]]];
    [banner showFromViewController:self];
 }
 @endcode
 
 For more information on integrating and using the Chartboost SDK please visit our help site documentation at https://help.chartboost.com
 */
@interface CHBBanner : UIView <CHBAd>

/*!
 @brief Chartboost location for the ad.
 @discussion Used to obtain ads with increased performance.
 */
@property (nonatomic, readonly) CBLocation location;

/*!
 @brief Size for the banner ad.
 @discussion You should provide one of the predefined values.
 The `intrinsicContentSize` of the banner view will depend on this value.
 */
@property (nonatomic, readonly) CHBBannerSize size;

/*!
 @brief Set to enable or disable the automatic refresh of a banner's content.
 @warning You should use this property to pause the refresh whenever the banner is no longer visible to the user.
 General cases like full-screen ad presentation, opening of the link for an ad or going to the background are handled by the banner itself so you don't need to worry about them.
 @discussion YES by default.
 The auto refresh cycle won't start until the the method showFromViewController: is called for the first time.
 Since that moment you can let the banner update its content on its own.
*/
@property (nonatomic) BOOL automaticallyRefreshesContent;

/*!
 @brief The delegate instance to receive banner callbacks.
 */
@property (nonatomic, weak, nullable) id<CHBBannerDelegate> delegate;

/*!
 @brief Determines if a cached ad exists.
 @return YES if there is a cached ad, and NO if not.
 @discussion A return value of YES here indicates that a call to showFromViewController: will present without making additional Chartboost server requests to fetch data.
 */
@property (nonatomic, readonly) BOOL isCached;

/*!
 @brief The initializer for the banner object. Default UIView initializers should not be used!
 @param size Size for the banner. See the size property documentation.
 @param location Location for the banner. See the location property documentation.
 @param delegate Delegate for the banner. See the delegate property documentation.
*/
- (instancetype)initWithSize:(CHBBannerSize)size location:(CBLocation)location delegate:(nullable id<CHBBannerDelegate>)delegate;

/*!
 @brief Please use initWithSize:location:delegate: instead.
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
 If no cached ad exists the method will attempt to fetch it from the Chartboost server first.
 If automaticallyRefreshesContent is YES (the default), calling this method causes the banner to start automatically updating its content.
 It is highly recommended that a non-nil view controller is passed, as it is required for enhanced ad presentation and some features like opening links in an in-app web browser.
 Implement didShowAd:error: in your ad delegate to be notified of a show request result, and didCacheAd:error: to be notified of the result of a cache request triggered by showing a non-cached banner.
 */
- (void)showFromViewController:(nullable UIViewController *)viewController;

@end

NS_ASSUME_NONNULL_END
