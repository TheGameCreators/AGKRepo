//
//  InterfaceController.m
//  Watch Extension
//
//  Created by Paul Johnston on 17/10/2017.
//
//

#import "InterfaceController.h"
#import "ExtensionDelegate.h"

InterfaceController *g_pInterface = 0;
extern ExtensionDelegate *g_pExtensionDelegate;

@interface InterfaceController ()
@property (nonatomic, weak) IBOutlet WKInterfaceLabel* questionlabel;
@property (nonatomic, weak) IBOutlet WKInterfaceButton* answer1button;
- (IBAction)anserPressed;
@end


@implementation InterfaceController

- (void)awakeWithContext:(id)context {
    [super awakeWithContext:context];
    g_pInterface = self;
    [_questionlabel setText:@"What's the national speed limit on motorways for cars and motorcycles?"];
}

- (void)willActivate {
    // This method is called when watch view controller is about to be visible to user
    [super willActivate];
}

- (void)didDeactivate {
    // This method is called when watch view controller is no longer visible
    [super didDeactivate];
}

-(void)setQuestion: (NSString*)text
{
    [_questionlabel setText:text];
}

- (IBAction)anserPressed {
    if ( !g_pExtensionDelegate ) return;
    [g_pExtensionDelegate sendMessage:@{@"answer":@"Pressed"}];
}
@end



