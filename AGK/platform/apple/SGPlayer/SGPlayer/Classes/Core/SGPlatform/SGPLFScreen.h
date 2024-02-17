//
//  SGPLFScreen.h
//  SGPlatform
//
//  Created by Single on 2017/2/24.
//  Copyright © 2017年 single. All rights reserved.
//

#import <SGPlayer/SGPLFObject.h>

#if SGPLATFORM_TARGET_OS_MAC

typedef NSScreen SGPLFScreen;

#elif SGPLATFORM_TARGET_OS_IPHONE_OR_TV

typedef UIScreen SGPLFScreen;

#endif

__BEGIN_DECLS

CGFloat SGPLFScreenGetScale(void);

__END_DECLS
