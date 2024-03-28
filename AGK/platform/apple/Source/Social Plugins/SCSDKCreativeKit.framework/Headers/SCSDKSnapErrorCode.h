//
//  SCSDKSnapErrorCode.h
//  SCSDKCreativeKit
//
//  Copyright © 2017 Snap, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#define SC_SDK_SNAP_ERROR_DOMAIN @"SCSDKCreativeKitErrorDomain"

typedef NS_ENUM(NSInteger, SCSDKCreativeKitErrorCode) {
    SCSDKCreativeKitErrorCodeUnknown,
    SCSDKCreativeKitErrorCodeInvalidArgument,
    SCSDKCreativeKitErrorCodeMainMediaDataReadFailure,
    SCSDKCreativeKitErrorCodeStickerDataReadFailure,
    SCSDKCreativeKitErrorCodeEncryptionMetadataReadFailure,
    SCSDKCreativeKitErrorCodeDeepLinkFailure,
    SCSDKCreativeKitErrorCodeIllegalState,
};
