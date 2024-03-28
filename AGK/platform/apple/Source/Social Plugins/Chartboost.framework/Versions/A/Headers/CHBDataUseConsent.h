//
//  CHBDataUseConsent.h
//  Chartboost
//
//  Copyright 2020 Chartboost. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/*!
@typedef CHBPrivacyStandard
@brief Constant that identifies a privacy standard to comply to.
*/
typedef NSString * CHBPrivacyStandard NS_TYPED_EXTENSIBLE_ENUM;
/*! @brief GDPR privacy standard identifier */
FOUNDATION_EXPORT CHBPrivacyStandard const CHBPrivacyStandardGDPR;
/*! @brief CCPA privacy standard identifier */
FOUNDATION_EXPORT CHBPrivacyStandard const CHBPrivacyStandardCCPA;

/*!
@class CHBDataUseConsent
@brief Abstract class. Subclasses define a data use consent option for a privacy standard.
Not intended to be used directly, always use a subclass to pass to +[Chartboost addDataUseConsent:] or to cast the result of +[Chartboost dataUseConsentForPrivacyStandard:]
*/
@interface CHBDataUseConsent: NSObject
/*! @brief The identifier for the privacy standard this consent applies to. */
@property (nonatomic, readonly) CHBPrivacyStandard privacyStandard;
/*! @brief Use the subclasses initializers to obtain a valid consent instance. */
- (instancetype)init NS_UNAVAILABLE;

@end

// MARK: - GDPR

/*!
@typedef NS_ENUM (NSUInteger, CHBGDPRConsent)
@brief Consent options for GDPR compliance.
*/
typedef NS_ENUM(NSUInteger, CHBGDPRConsent) {
    /*! User does not consent to behavioral targeting in compliance with GDPR. */
    CHBGDPRConsentNonBehavioral,
    /*! User consents to behavioral targeting in compliance with GDPR. */
    CHBGDPRConsentBehavioral
} NS_SWIFT_NAME(CHBGDPRDataUseConsent.Consent);

/*!
@class CHBGDPRDataUseConsent
@brief CHBDataUseConsent subclass for compliance with GDPR.
*/
NS_SWIFT_NAME(CHBDataUseConsent.GDPR)
@interface CHBGDPRDataUseConsent: CHBDataUseConsent
/*! @brief The GDPR consent option. */
@property (nonatomic, readonly) CHBGDPRConsent consent;
/*!
 @brief Returns a GDPR consent object.
 @param consent The desired GDPR consent option.
 */
+ (CHBGDPRDataUseConsent *)gdprConsent:(CHBGDPRConsent)consent NS_SWIFT_NAME(init(_:));

@end

// MARK: - CCPA

/*!
@typedef NS_ENUM (NSUInteger, CHBCCPAConsent)
@brief Consent options for CCPA compliance.
*/
typedef NS_ENUM(NSUInteger, CHBCCPAConsent) {
    /*! User does not consent to the sale of his or her personal information in compliance with CCPA. */
    CHBCCPAConsentOptOutSale,
    /*! User consents to the sale of his or her personal information in compliance with CCPA. */
    CHBCCPAConsentOptInSale
} NS_SWIFT_NAME(CHBCCPADataUseConsent.Consent);

/*!
@class CHBCCPADataUseConsent
@brief CHBDataUseConsent subclass for compliance with CCPA.
*/
NS_SWIFT_NAME(CHBDataUseConsent.CCPA)
@interface CHBCCPADataUseConsent: CHBDataUseConsent
/*! @brief The CCPA consent option. */
@property (nonatomic, readonly) CHBCCPAConsent consent;
/*!
@brief Returns a CCPA consent object.
@param consent The desired CCPA consent option.
*/
+ (CHBCCPADataUseConsent *)ccpaConsent:(CHBCCPAConsent)consent NS_SWIFT_NAME(init(_:));
@end

// MARK: - Custom

/*!
@class CHBCustomDataUseConsent
@brief CHBDataUseConsent subclass for compliance with a custom privacy standard.
*/
NS_SWIFT_NAME(CHBDataUseConsent.Custom)
@interface CHBCustomDataUseConsent: CHBDataUseConsent
/*! @brief The custom consent value. */
@property (nonatomic, readonly) NSString *consent;
/*!
 @brief Returns a custom consent object.
 @discussion Normally you would use other CHBDataUseConsent subclasses instead, which provide predefined options for current privacy standards.
 If you decide to use this make sure you pass valid values, as defined in https://answers.chartboost.com/en-us/child_article/ios-privacy-methods
 @param privacyStandard The desired privacy standard identifier.
 @param consent The desired consent value.
*/
+ (CHBCustomDataUseConsent *)customConsentWithPrivacyStandard:(CHBPrivacyStandard)privacyStandard consent:(NSString *)consent NS_SWIFT_NAME(init(privacyStandard:consent:));

@end

NS_ASSUME_NONNULL_END
