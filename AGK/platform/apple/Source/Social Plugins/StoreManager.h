
#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>
#import "StoreObserver.h"

@interface StoreManager : NSObject < SKProductsRequestDelegate >
{
	NSMutableArray*		purchasableObjects;
	StoreObserver*		storeObserver;
}

@property ( nonatomic, retain ) NSMutableArray* purchasableObjects;
@property ( nonatomic, retain ) StoreObserver*  storeObserver;

// public functions
- ( void ) setup;
- ( void ) setTitle: ( const char* ) ID;
- ( void ) reset;
- ( void ) addProductID: ( const char* ) ID;
- ( void ) purchaseUnlockableContent:    ( int ) ID;
- ( BOOL ) isUnlockableContentAvailable: ( int ) ID;
- ( int ) getContentState: ( int ) ID;
- ( int  ) getState;
- ( char* ) getToken: (int) ID;
- ( void ) resetPurchase: (const char*) token;

// private functions
- ( void ) requestProductData;
//- ( void ) buyFeature: ( NSString* ) featureId;
- ( void ) failedTransaction: ( SKPaymentTransaction* ) transaction;
- ( void ) cancelledTransaction: ( SKPaymentTransaction* ) transaction;
- ( void ) provideContent: ( NSString* ) productIdentifier signature:(NSString*) signature token:(NSString*) token;
+ ( StoreManager* ) sharedManager;
- ( void ) loadPurchases;
- ( void ) updatePurchases;
- ( void ) restore;
- ( void ) finishedRestore: (int) success;
- ( char*) formatPrice:(NSDecimalNumber*)num locale:(NSLocale*)locale;
- ( char*) formatSubUnit:(void*)unit;
- ( char*) getLocalPrice: (int) iID;
- ( char*) getDescription: (int) iID;
- ( char*) getSignature: (int) iID;
- (SKProduct*) getProductFromID: (int) iID;
- ( int )  getNumPlans: (int) iID;
- ( int )  getPlanNumPeriods: (int) iID plan:(int)plan;
- ( char*) getPlanPrice: (int) iID plan:(int)plan period:(int)period;
- ( int )  getPlanPaymentType: (int)iID plan:(int)plan period:(int)period;
- ( char*) getPlanDurationUnit: (int) iID plan:(int)plan period:(int)period;
- ( int )  getPlanDuration: (int) iID plan:(int)plan period:(int)period;
- ( char*) getPlanTags: (int) iID plan:(int)plan;
- ( char*) getPlanToken: (int) iID plan:(int)plan;

@end
