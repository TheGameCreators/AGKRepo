//
//  SGError.m
//  SGPlayer
//
//  Created by Single on 2018/1/16.
//  Copyright © 2018年 single. All rights reserved.
//

#import "SGError.h"
#import "SGFFmpeg.h"

NSErrorDomain const SGErrorDomain = @"SGPlayer-Error-SGErrorCode";
NSString * const SGErrorUserInfoKeyOperation = @"SGErrorUserInfoKeyOperation";
NSString * const SGErrorUserInfoKeyFFMpegErrorNumber = @"SGErrorUserInfoKeyFFMpegErrorNumber";


NSError * SGGetFFError(int result, SGActionCode operation)
{
    if (result >= 0) {
        return nil;
    }
    NSMutableDictionary *errDict = [[NSMutableDictionary alloc] init];
    errDict[SGErrorUserInfoKeyOperation] = @(operation);
    errDict[SGErrorUserInfoKeyFFMpegErrorNumber] = @(result);
    char *data = malloc(256);
    av_strerror(result, data, 256);
    NSString *descriptionStr = [[NSString alloc] initWithBytesNoCopy:data length:strnlen(data, 256) encoding:NSUTF8StringEncoding freeWhenDone:YES];
    errDict[NSLocalizedDescriptionKey] = descriptionStr;
    if (result == AVERROR_EXIT) {
        result = SGErrorCodeImmediateExitRequested;
    } else if (result == AVERROR_EOF) {
        result = SGErrorCodeDemuxerEndOfFile;
    }
    return [NSError errorWithDomain:SGErrorDomain code:result userInfo:errDict];
}

NSError * SGCreateError(NSInteger code, SGActionCode operation)
{
    return [NSError errorWithDomain:SGErrorDomain code:(NSInteger)code userInfo:@{SGErrorUserInfoKeyOperation : @(operation)}];
}
