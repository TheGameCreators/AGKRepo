//
//  SGPacket+Internal.h
//  SGPlayer iOS
//
//  Created by Single on 2018/10/23.
//  Copyright © 2018 single. All rights reserved.
//

#import "SGPacket.h"
#import "SGFFmpeg.h"
#import "SGAudioDescriptor.h"
#import "SGVideoDescriptor.h"

@interface SGAudioDescriptor ()

- (instancetype)init NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithFrame:(AVFrame *)frame NS_DESIGNATED_INITIALIZER;

@end

@interface SGVideoDescriptor ()

- (instancetype)init NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithFrame:(AVFrame *)frame NS_DESIGNATED_INITIALIZER;

@end
