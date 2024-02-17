
#import "LikeButton.h"

/////////////////


//FBCustomLoginDialog.m
@implementation FBCustomLoginDialog
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType {
	NSURL* url = request.URL;
	if ([[url absoluteString] rangeOfString:@"login"].location == NSNotFound) {
		[self dialogDidSucceed:url];
		return NO;
	} 
	else if (url != nil) {
		[_spinner startAnimating];
		[_spinner setHidden:NO];
		return YES;
	}
	return NO;
}
@end



static FBDialog *loginDialog_;

@implementation FBLikeButton

@synthesize textColor = textColor_, buttonColor = buttonColor_, linkColor = linkColor_;

- (id)initWithFrame:(CGRect)frame andUrl:(NSString *)likePage andStyle:(FBLikeButtonStyle)style andColor:(FBLikeButtonColor)color {
	if ((self = [super initWithFrame:frame])) {
		NSString *styleQuery=(style==FBLikeButtonStyleButtonCount? @"button_count" : (style==FBLikeButtonStyleBoxCount? @"box_count" : @"standard"));
		NSString *colorQuery=(color==FBLikeButtonColorDark? @"dark" : @"light");
		NSString *url =[NSString stringWithFormat:@"http://www.facebook.com/plugins/like.php?layout=%@&show_faces=true&width=%d&height=%d&action=like&colorscheme=%@&href=%@",
                        styleQuery, (int) frame.size.width, (int) frame.size.height, colorQuery, likePage];
        
		//Creamos una webview muy alta para evitar el scroll interno por la foto del usuario y otras cosas
		webView_ = [[UIWebView alloc] initWithFrame:CGRectMake(0, 0, frame.size.width, 300)];
		[self addSubview:webView_];
		[webView_ loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:url]]];
		webView_.opaque = NO;
		webView_.backgroundColor = [UIColor clearColor];
		webView_.delegate = (id) [[UIApplication sharedApplication] delegate ];
		webView_.autoresizingMask = UIViewAutoresizingFlexibleWidth;
		//[[webView_ scrollView] setBounces:NO];
		self.backgroundColor=[UIColor clearColor];
		self.clipsToBounds=YES;
        
		[[NSNotificationCenter defaultCenter] addObserver:webView_ selector:@selector(reload) name:FB_LIKE_BUTTON_LOGIN_NOTIFICATION object:nil];
	}
	return self;
}

- (id)initWithFrame:(CGRect)frame andUrl:(NSString *)likePage{
	return [self initWithFrame:frame andUrl:likePage andStyle:FBLikeButtonStyleStandard andColor:FBLikeButtonColorLight];
}

- (void)dealloc {
    
	[[NSNotificationCenter defaultCenter] removeObserver:webView_ name:FB_LIKE_BUTTON_LOGIN_NOTIFICATION object:nil];
    
	[webView_ stopLoading];
	webView_.delegate=nil;
	[webView_ removeFromSuperview];
	[webView_ release]; webView_=nil;
    
	self.linkColor=nil;
	self.textColor=nil;
	self.buttonColor=nil;
    
	//[super dealloc];
}


- (void) configureTextColors{
	NSString *textColor=[textColor_ hexStringFromColor];
	NSString *buttonColor=[buttonColor_ hexStringFromColor];
	NSString *linkColor=[linkColor_ hexStringFromColor];
    
	NSString *javascriptLinks = [NSString stringWithFormat:@"{"
                                 "var textlinks=document.getElementsByTagName('a');"
                                 "for(l in textlinks) { textlinks[l].style.color='#%@';}"
                                 "}", linkColor];
    
	NSString *javascriptSpans = [NSString stringWithFormat:@"{"
                                 "var spans=document.getElementsByTagName('span');"
                                 "for(s in spans) { if (spans[s].className!='liketext') { spans[s].style.color='#%@'; } else {spans[s].style.color='#%@';}}"
                                 "}", textColor, (buttonColor==nil? textColor : buttonColor)];
    
	//Lanzamos el javascript inmediatamente
	if (linkColor)
		[webView_ stringByEvaluatingJavaScriptFromString:javascriptLinks];
	if (textColor)
		[webView_ stringByEvaluatingJavaScriptFromString:javascriptSpans];
    
	//Programamos la ejecucion para cuando termine
	if (linkColor)
		[webView_ stringByEvaluatingJavaScriptFromString:[NSString stringWithFormat:@"setTimeout(function () %@, 3000)", javascriptLinks]];
	if (textColor)
		[webView_ stringByEvaluatingJavaScriptFromString:[NSString stringWithFormat:@"setTimeout(function () %@, 3000)", javascriptSpans]];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark UIWebViewDelegate

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType {
    
	if (loginDialog_!=nil)
		return NO;
    
	// if user has to log in, open a new (modal) window
	if ([[[request URL] absoluteString] rangeOfString:@"login.php"].location!=NSNotFound){
		loginDialog_= [[[FBCustomLoginDialog alloc] init] autorelease];
		[loginDialog_ loadURL:[[request URL] absoluteString] get:nil];
		loginDialog_.delegate = (id) [[UIApplication sharedApplication] delegate ];
		[loginDialog_ show];
		[loginDialog_.delegate retain]; //Retenemos el boton que ha abierto el login para que pueda recibir la confirmacion correctamente
		return NO;
	}
	if (([[[request URL] absoluteString] rangeOfString:@"/connect/"].location!=NSNotFound) || ([[[request URL] absoluteString] rangeOfString:@"like.php"].location!=NSNotFound)){
		return YES;
	}
    
	NSLog(@"URL de Facebook no contemplada: %@", [[request URL] absoluteString]);
    
	return NO;
}

- (void)webViewDidFinishLoad:(UIWebView *)webView{
	[self configureTextColors];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
#pragma mark Facebook Connect

- (void)dialogDidSucceed:(FBDialog*)dialog {
	[loginDialog_.delegate release];
	loginDialog_.delegate=nil;
	loginDialog_=nil;
    
	//Lanzamos la notificacion para que se actualicen los botones
	[[NSNotificationCenter defaultCenter] postNotificationName:FB_LIKE_BUTTON_LOGIN_NOTIFICATION object:nil];
}

/**
 * Called when the dialog succeeds and is about to be dismissed.
 */
- (void)dialogDidComplete:(FBDialog *)dialog{
	[self dialogDidSucceed:dialog];
}

/**
 * Called when the dialog succeeds with a returning url.
 */
- (void)dialogCompleteWithUrl:(NSURL *)url{
	[self dialogDidSucceed:loginDialog_];
}

/**
 * Called when the dialog get canceled by the user.
 */
- (void)dialogDidNotCompleteWithUrl:(NSURL *)url{
	[self dialogDidSucceed:loginDialog_];
}

/**
 * Called when the dialog is cancelled and is about to be dismissed.
 */
- (void)dialogDidNotComplete:(FBDialog *)dialog{
	[self dialogDidSucceed:loginDialog_];
}

/**
 * Called when dialog failed to load due to an error.
 */
- (void)dialog:(FBDialog*)dialog didFailWithError:(NSError *)error{
	[self dialogDidSucceed:loginDialog_];
}

@end

///////////////////
