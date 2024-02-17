//
//  SGPlayer.h
//  SGPlayer
//
//  Created by Single on 03/01/2017.
//  Copyright © 2017 single. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import <SGPlayer/SGPlayerHeader.h>
#import "SGPlayerHeader.h"

NS_ASSUME_NONNULL_BEGIN

#pragma mark - SGPlayer

@interface SGPlayer : NSObject

/*!
 @property options
 @abstract
    The options for an SGPlayer.
 
 @discussion
    When a player is Created, a copy of the shared options is made.
    After modifying the options, call -replaceWithXXX: to apply the new options.
 */
@property (NS_NONATOMIC_IOSONLY, strong) SGOptions *options;

/*!
 @property error
 @abstract
    Get player error information.
 */
@property (NS_NONATOMIC_IOSONLY, readonly, strong, nullable) NSError *error;

/*!
 @property timeInfo
 @abstract
    Get player time information.
 */
@property (NS_NONATOMIC_IOSONLY, readonly) SGTimeInfo timeInfo;

/*!
 @property stateInfo
 @abstract
    Get player state information.
 */
@property (NS_NONATOMIC_IOSONLY, readonly) SGStateInfo stateInfo;

/*!
 @method stateInfo:timeInfo:error:
 @abstract
    Get player state and time information.
 @result
    Returns \c YES if the information was set to the params.
 
 @discussion
    Use this method to get synchronized state and time information.
 */
- (BOOL)stateInfo:(nullable SGStateInfo *)stateInfo timeInfo:(nullable SGTimeInfo *)timeInfo error:(NSError **)error NS_SWIFT_NOTHROW;

@end

#pragma mark - Item

@interface SGPlayer ()

/*!
 @property currentItem
 @abstract
    Indicates the current item of the Player.
 @result
    Returns \c nil if the player is idle.
 */
@property (nonatomic, strong, readonly, nullable) SGPlayerItem *currentItem;

/*!
 @property readyHandler
 @abstract
    The handler will be called when the player gets ready.
 
 @discussion
    This handler will be executed in an unknown thread. Do not perform complex opertaionts in this method.
    For example, do some basic configuration of the current item.
 */
@property (nonatomic, copy) SGHandler readyHandler;

/*!
 @method replaceWithURL:
 @abstract
    Equivalent to:
        SGAsset *asset = [[SGURLAsset alloc] initWithURL:URL];
        [self replaceWithAsset:asset];
 */
- (BOOL)replaceWithURL:(nullable NSURL *)URL;

/*!
 @method replaceWithAsset:
 @abstract
    Equivalent to:
        SGPlayerItem *item = [[SGPlayerItem alloc] initWithAsset:asset];
        [self replaceWithPlayerItem:item];
 */
- (BOOL)replaceWithAsset:(nullable SGAsset *)asset;

/*!
 @method replaceWithPlayerItem:
 @abstract
    Replaces the player's current item with the specified player item.
 
 @discussion
    After calling this method, the player will be reset and automatically start loading the new item.
 */
- (BOOL)replaceWithPlayerItem:(nullable SGPlayerItem *)item;

@end

#pragma mark - Playback

@interface SGPlayer ()

/*!
 @property rate
 @abstract
    Indicates the desired rate of playback.
 
 @discussion
    This is different from AVPlayer's rate. It only indicates the desired rate of playback and does not trigger playback action.
 */
@property (nonatomic) Float64 rate;

/*!
 @property wantsToPlay
 @abstract
    Indicates whether the player is currently paused or played.
 
 @discussion
    After calling -play, the value will change to YES.
    After calling -pause, the value will change to NO.
 */
@property (nonatomic, readonly) BOOL wantsToPlay;

#if SGPLATFORM_TARGET_OS_IPHONE_OR_TV
/*!
 @property pausesWhenInterrupted
 @abstract
    Indicates whether to automatically pause when audio session is interrupted.
    Default is YES.
 */
@property (nonatomic) BOOL pausesWhenInterrupted;

/*!
 @property pausesWhenEnteredBackground
 @abstract
    Indicates whether to automatically pause when application enters the background.
    Default is NO.
 */
@property (nonatomic) BOOL pausesWhenEnteredBackground;

/*!
 @property pausesWhenEnteredBackgroundIfNoAudioTrack
 @abstract
    Indicates whether to automatically pause when application enters the background if there's no audio track.
    Default is YES.
 */
@property (nonatomic) BOOL pausesWhenEnteredBackgroundIfNoAudioTrack;
#endif

/*!
 @method play
 @abstract
    Signals the desire to begin playback at the current rate.
 */
- (BOOL)play;

/*!
 @method pause
 @abstract
    Signals the desire to pause playback.
 */
- (BOOL)pause;

/*!
 @property seekable
 @abstract
    Indicates whether it is possible to seek.
 */
@property (NS_NONATOMIC_IOSONLY, readonly, getter=isSeekable) BOOL seekable;

/*!
 @method seekToTime:
 @abstract
    Equivalent to:
        [self seekToTime:time result:nil];
 */
- (BOOL)seekToTime:(CMTime)time;

/*!
 @method seekToTime:result:
 @abstract
    Equivalent to:
        [self seekToTime:time toleranceBefore:kCMTimeInvalid toleranceAfter:kCMTimeInvalid result:result];
 */
- (BOOL)seekToTime:(CMTime)time result:(nullable SGSeekResult)result;

/*!
 @method seekToTime:toleranceBefore:toleranceAfter:result:
 @abstract
    Moves the playback cursor.

 @discussion
    Use this method to seek to a specified time for the current player item and to be notified when the seek operation is complete. The result handler for any prior seek request that is still in process will be invoked immediately with the error parameter. If the new request completes without being interrupted by another seek request or by any other operation the specified result handler will be invoked without error.
 */
- (BOOL)seekToTime:(CMTime)time toleranceBefore:(CMTime)toleranceBefore toleranceAfter:(CMTime)toleranceAfter result:(nullable SGSeekResult)result;

@end

#pragma mark - Renderer

@interface SGPlayer ()

/*!
 @property audioRenderer
 @abstract
    The renderer for audio frame.
 
 @discussion
    The life cycle is consistent with the player. The settings for the renderer will always take effect.
 */
@property (NS_NONATOMIC_IOSONLY, strong, readonly, nullable) SGAudioRenderer *audioRenderer;

/*!
 @property videoRenderer
 @abstract
    The renderer for video frame.
 
 @discussion
    The life cycle is consistent with the player. The settings for the renderer will always take effect.
 */
@property (NS_NONATOMIC_IOSONLY, strong, readonly, nullable) SGVideoRenderer *videoRenderer;

@end

#pragma mark - Notification

__BEGIN_DECLS

/*!
 @constant SGPlayerDidChangeInfoNotification
 @abstract
    A notification that fires whenever information changes.
 
 @discussion
    This notification will be triggered by a change in playback state or playback time.
 */
SGPLAYER_EXTERN NSNotificationName const SGPlayerDidChangeInfoNotification;

/*!
 @constant SGPlayerTimeInfoUserInfoKey
 @abstract
    The user info key to get the time info.
 */
SGPLAYER_EXTERN NSString * const SGPlayerTimeInfoUserInfoKey;

/*!
 @constant SGPlayerStateInfoUserInfoKey
 @abstract
    The user info key to get the state info.
 */
SGPLAYER_EXTERN NSString * const SGPlayerStateInfoUserInfoKey;

/*!
 @constant SGPlayerInfoActionUserInfoKey
 @abstract
    The user info key to get the actions that triggers the notification.
 */
SGPLAYER_EXTERN NSString * const SGPlayerInfoActionUserInfoKey;

__END_DECLS

@interface SGPlayer ()

/*!
 @method timeInfoFromUserInfo:
 @abstract
    To get the time info from user info.
 */
+ (SGTimeInfo)timeInfoFromUserInfo:(NSDictionary *)userInfo;

/*!
 @method timeInfoFromUserInfo:
 @abstract
    To get the state info from user info.
 */
+ (SGStateInfo)stateInfoFromUserInfo:(NSDictionary *)userInfo;

/*!
 @method timeInfoFromUserInfo:
 @abstract
    To get the info action from user info.
 */
+ (SGInfoAction)infoActionFromUserInfo:(NSDictionary *)userInfo;

/*!
 @property actionMask
 @abstract
    Block unwanted actions.
 */
@property (NS_NONATOMIC_IOSONLY) SGInfoAction actionMask;

/*!
 @property minimumTimeInfoInterval
 @abstract
    Indicates the minimum interval at which the time info change triggers the notification.
    Default is 1.0 (1 second).
 */
@property (NS_NONATOMIC_IOSONLY) NSTimeInterval minimumTimeInfoInterval;

/*!
 @property notificationQueue
 @abstract
    Indicates in which queue the notification will be executed.
    Default is main queue.
 */
@property (NS_NONATOMIC_IOSONLY, strong) NSOperationQueue *notificationQueue;

@end

NS_ASSUME_NONNULL_END
