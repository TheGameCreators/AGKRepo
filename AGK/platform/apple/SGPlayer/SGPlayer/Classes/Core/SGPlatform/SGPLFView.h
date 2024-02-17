//
//  SGPLFView.h
//  SGPlatform
//
//  Created by Single on 2017/2/24.
//  Copyright © 2017年 single. All rights reserved.
//

#import <SGPlayer/SGPLFObject.h>
#import <SGPlayer/SGPLFImage.h>
#import <SGPlayer/SGPLFColor.h>

#if SGPLATFORM_TARGET_OS_MAC

typedef NSView SGPLFView;

#elif SGPLATFORM_TARGET_OS_IPHONE_OR_TV

typedef UIView SGPLFView;

#endif

NS_ASSUME_NONNULL_BEGIN

__BEGIN_DECLS

void SGPLFViewSetBackgroundColor(SGPLFView *view, SGPLFColor *color);
void SGPLFViewInsertSubview(SGPLFView *superView, SGPLFView *subView, NSInteger index);

SGPLFImage *_Nullable SGPLFViewGetCurrentSnapshot(SGPLFView *view);

__END_DECLS

NS_ASSUME_NONNULL_END
