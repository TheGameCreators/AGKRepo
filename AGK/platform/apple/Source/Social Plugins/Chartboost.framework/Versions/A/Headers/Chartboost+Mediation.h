/*
 * Chartboost+Mediation.h
 * Chartboost
 *
 * Copyright 2019 Chartboost. All rights reserved.
 */

#import "Chartboost.h"
#import "CHBBanner.h"
#import "CHBInterstitial.h"
#import "CHBRewarded.h"

NS_ASSUME_NONNULL_BEGIN

// APIs required to integrate Chartboost with a mediation platform.
// If you are the end user of this framework you typically won't need this. It is intended to support mediation adapters and in-house mediation solutions.

// MARK: - CHBMediation

/*!
@class CHBMediation
@brief A class that holds information about the mediation library used to create an ad.
*/
@interface CHBMediation : NSObject
/*! @brief An identifier for the mediation library. */
@property (nonatomic, readonly) CBMediation mediationType;
/*! @brief The mediation library version. */
@property (nonatomic, readonly) NSString *libraryVersion;
/*! @brief The mediation library adapter version. */
@property (nonatomic, readonly) NSString *adapterVersion;

/*!
@brief The initializer for the mediation object.
@param mediationType The mediation library used. Pass CBMediationOther if your mediation is not listed in the predefined values.
@param libraryVersion A string representing the library version. E.g. "5.1.0"
@param adapterVersion A string representing the Chartbooost adapter version for this library. E.g. "5.1.0.1"
*/
- (instancetype)initWithType:(CBMediation)mediationType libraryVersion:(NSString*)libraryVersion adapterVersion:(NSString*)adapterVersion;

- (instancetype)init NS_UNAVAILABLE;

@end

// MARK: - CHBBanner Extensions

@interface CHBBanner (MediationAPI)
/*!
 @brief Similar to -[CHBBanner initWithSize:location:delegate], but including a mediation parameter.
 @discussion Use this initializer only if you are writing an adapter to use Chartboost with a mediation solution.
*/
- (instancetype)initWithSize:(CHBBannerSize)size location:(CBLocation)location mediation:(nullable CHBMediation *)mediation delegate:(nullable id<CHBBannerDelegate>)delegate;

/*!
 @brief Caches an ad using an OpenRTB bid response.
 @param bidResponse A bid response containing information about the ad to cache in OpenRTB format.
 @discussion Use this method if you want to cache an ad already obtained from a bidding solution.
 Use the isCached property or implement didCacheAd:error: in your ad delegate to be notified of a cache request result.
*/
- (void)cacheBidResponse:(NSString *)bidResponse NS_SWIFT_NAME(cache(bidResponse:));

@end

// MARK: - CHBInterstitial Extensions

@interface CHBInterstitial (MediationAPI)
/*!
 @brief Similar to -[CHBInterstitial initWithLocation:delegate], but including a mediation parameter.
 @discussion Use this initializer only if you are writing an adapter to use Chartboost with a mediation solution.
*/
- (instancetype)initWithLocation:(CBLocation)location mediation:(nullable CHBMediation *)mediation delegate:(nullable id<CHBInterstitialDelegate>)delegate;

/*!
 @brief Caches an ad using an OpenRTB bid response.
 @param bidResponse A bid response containing information about the ad to cache in OpenRTB format.
 @discussion Use this method if you want to cache an ad already obtained from a bidding solution.
 Use the isCached property or implement didCacheAd:error: in your ad delegate to be notified of a cache request result.
*/
- (void)cacheBidResponse:(NSString *)bidResponse NS_SWIFT_NAME(cache(bidResponse:));

@end

// MARK: - CHBRewarded Extensions

@interface CHBRewarded (MediationAPI)
/*!
 @brief Similar to -[CHBRewarded initWithLocation:delegate], but including a mediation parameter.
 @discussion Use this initializer only if you are writing an adapter to use Chartboost with a mediation solution.
*/
- (instancetype)initWithLocation:(CBLocation)location mediation:(nullable CHBMediation *)mediation delegate:(nullable id<CHBRewardedDelegate>)delegate;

/*!
 @brief Caches an ad using an OpenRTB bid response.
 @param bidResponse A bid response containing information about the ad to cache in OpenRTB format.
 @discussion Use this method if you want to cache an ad already obtained from a bidding solution.
 Use the isCached property or implement didCacheAd:error: in your ad delegate to be notified of a cache request result.
*/
- (void)cacheBidResponse:(NSString *)bidResponse NS_SWIFT_NAME(cache(bidResponse:));

@end

NS_ASSUME_NONNULL_END
