
#import "StoreManager.h"
//#import "GameAppDelegate.h"

#import <UIKit/UIKit.h>

//extern int g_iPasses;

@implementation StoreManager

@synthesize purchasableObjects;
@synthesize storeObserver;


#define MAX_PRODUCTS 100

static int				productCount = 0;
static char*			productID [ MAX_PRODUCTS ] = {0};
static int				productState [ MAX_PRODUCTS ];
static char*			productSig [ MAX_PRODUCTS ] = {0};
static char*            productToken [ MAX_PRODUCTS ] = {0};
static char				title [ 128 ];

- ( void ) reset
{
    productCount = 0;

	for ( int i = 0; i < MAX_PRODUCTS; i++ )
    {
        productState [ i ] = 0;
        if ( productID[ i ] ) delete productID[ i ];
		productID[ i ] = 0;
        if ( productSig[ i ] ) delete [] productSig[ i ];
		productSig[i] = 0;
        if ( productToken[ i ] ) delete [] productToken[ i ];
        productToken[i] = 0;
    }
    
    if ( storeObserver ) [ storeObserver release ];
    storeObserver = nil;
   
    [purchasableObjects removeAllObjects];
}

- ( void ) addProductID: ( const char* ) ID
{
    if ( productCount >= MAX_PRODUCTS )
    {
        NSLog(@"Max number of In App Purchase products reached");
        return;
    }
    
    if ( strlen(ID) > 1024 )
    {
        NSLog(@"IAP product ID exceeds maximum length of 1024 characters");
        return;
    }
    
    if ( productID[ productCount ] ) delete [] productID[ productCount ];
    productID[ productCount ] = new char[ strlen(ID) + 1 ];
	strcpy( productID[productCount], ID );
    productCount++;
}

- ( int  ) getState
{
	return 1; // deprecated
}

- ( void ) setTitle: ( const char* ) ID
{
	strcpy ( title, ID );
}

- ( void ) dealloc
{
	[ storeObserver release ];
	[ super dealloc ];
}

- ( BOOL ) isUnlockableContentAvailable: ( int ) ID
{
    // deprecated in favor of getContentState
	return (productState[ ID ] == 4);
}

- ( int ) getContentState: ( int ) ID
{
	// use this function to determine whether content is available
	return productState[ ID ];
}

- ( void ) setup
{
	@synchronized ( self )
	{
        for ( int i = 0; i < MAX_PRODUCTS; i++ )
        {
            productState [ i ] = 0;
            productSig[ i ] = 0;
            productToken[ i ] = 0;
        }
        
        purchasableObjects = [ [ NSMutableArray alloc ] init ];
        [ self requestProductData ];
        
        [ self loadPurchases ];
        storeObserver =  [ [ StoreObserver alloc ] init ];
        [ [ SKPaymentQueue defaultQueue ] addTransactionObserver: storeObserver ];
    }
}

+ ( StoreManager* ) sharedManager
{
	// return the shared store manager
    static StoreManager* defaultManager = nil;
    @synchronized (self) {
        if ( defaultManager == nil ) defaultManager = [[self alloc] init];
    }
    return defaultManager;
}


- ( void ) requestProductData
{
    if ( productCount == 0 ) return;
    
	NSString *str = 0;
    NSSet *set = [NSSet set];
    for ( int i = 0; i < productCount; i++ )
    {
        if ( productID[i] == 0 ) continue;
        str = [ NSString stringWithUTF8String:productID [ i ] ];
        set = [ set setByAddingObject:str ];
    }
    
    // request product details from the AppStore
	SKProductsRequest* request=  [ [ SKProductsRequest alloc ] initWithProductIdentifiers: set ];
    request.delegate = self;
	[ request start ];
}

- ( void ) productsRequest: ( SKProductsRequest* ) request didReceiveResponse: ( SKProductsResponse* ) response
{
	// this callback function will provide us with information from the AppStore

    @synchronized (self)
    {
        [ purchasableObjects addObjectsFromArray:response.products ];
    }
	
	[ request autorelease ];
}

- (SKProduct*) getProductFromID: (int) iID
{
    if ( iID < 0 || iID > productCount || productID[iID] == 0 ) return nil;
    
    for ( int i = 0; i < [purchasableObjects count]; i++ )
    {
        SKProduct* product = [ purchasableObjects objectAtIndex: i ];
        const char *szID = [product.productIdentifier UTF8String];
        
        if ( strcmp( szID, productID[iID] ) == 0 )
        {
            return product;
        }
    }
    
    return nil;
}

- ( char*) formatPrice:(NSDecimalNumber*)num locale:(NSLocale*)locale
{
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    [numberFormatter setLocale:locale];
    numberFormatter.positiveSuffix = @"";
    numberFormatter.negativeSuffix = @"";

    NSString *currency_symbol = [numberFormatter currencySymbol];
    NSString *currency_code = [numberFormatter currencyCode];
    
    [numberFormatter setCurrencySymbol:@""];
    
    NSString *price = [numberFormatter stringFromNumber:num];
    
    char szPrice[ 32 ];

    if ( [currency_symbol length] > 0 )
    {
        strcpy( szPrice, [currency_symbol UTF8String] );
        strcat( szPrice, [price UTF8String] );
    }
    else
    {
        strcpy( szPrice, [price UTF8String] );
        strcat( szPrice, " " );
        strcat( szPrice, [currency_code UTF8String]);
    }
    
    [ numberFormatter release ];
    
    char *str = new char[ strlen(szPrice) + 1 ];
    strcpy( str, szPrice );
    return str;
}

- (char*) getLocalPrice: (int) iID
{
    @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
    
        if ( product == nil )
        {
            char* str = new char[1];
            *str = 0;
            return str;
        }
        
        return [self formatPrice:product.price locale:product.priceLocale];
    }
}

- ( int )  getNumPlans: (int) iID
{
   @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil ) return 0;
    
        if (@available(iOS 12.2, *))
        {
            if ( product.discounts == nil || product.discounts.count == 0 )
            {
                return 1;
            }
            else
            {
                // requires a server to sign discounts
                //return (int) product.discounts.count + 1;
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }
}

- ( int ) getPlanNumPeriods: (int) iID plan:(int)plan
{
    @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil || plan < 0 ) return 0;
        
        if ( plan == 0 )
        {
            if ( @available(iOS 11.2, *) )
            {
                if ( product.introductoryPrice != nil ) return 2;
                else return 1;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            if (@available(iOS 12.2, *))
            {
                plan--;
                
                if ( product.discounts == nil || plan >= [product.discounts count] )
                {
                    return 0;
                }
            
                return 2; // iOS only supports the offer followed by the full price
            }
            else
            {
                return 0;
            }
        }
    }
}

- ( char*) getPlanPrice: (int) iID plan:(int)plan period:(int)period
{
    @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil || plan < 0 || period < 0 )
        {
            char* str = new char[1];
            *str = 0;
            return str;
        }
        
        if ( plan == 0 )
        {
            if ( @available(iOS 11.2, *) )
            {
                if ( product.introductoryPrice != nil && period == 0 )
                {
                    return [self formatPrice:product.introductoryPrice.price locale:product.introductoryPrice.priceLocale];
                }
                else
                {
                    return [self formatPrice:product.price locale:product.priceLocale];
                }
            }
            else
            {
                return [self formatPrice:product.price locale:product.priceLocale];
            }
        }
        else
        {
            if (@available(iOS 12.2, *))
            {
                plan--;
                
                if ( product.discounts == nil || plan >= [product.discounts count] )
                {
                    return [self formatPrice:product.price locale:product.priceLocale];
                }
                
                SKProductDiscount* discount = product.discounts[plan];
            
                if ( period == 0 ) return [self formatPrice:product.price locale:product.priceLocale];
                else return [self formatPrice:discount.price locale:discount.priceLocale];
            }
            else
            {
                return [self formatPrice:product.price locale:product.priceLocale];
            }
        }
    }
}

#define AGK_PAYMENT_TYPE_UNKNOWN -1
#define AGK_PAYMENT_TYPE_FREE 0
#define AGK_PAYMENT_TYPE_ONE_OFF 1
#define AGK_PAYMENT_TYPE_RECURRING_LIMITED 2
#define AGK_PAYMENT_TYPE_RECURRING_UNTIL_CANCELLED 3

- ( int )  getPlanPaymentType: (int)iID plan:(int)plan period:(int)period
{
    @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil || plan < 0 || period < 0 )
        {
            return AGK_PAYMENT_TYPE_UNKNOWN;
        }
        
        if ( plan == 0 )
        {
            if ( @available(iOS 11.2, *) )
            {
                if ( product.introductoryPrice != nil && period == 0 )
                {
                    switch( product.introductoryPrice.paymentMode )
                    {
                        case SKProductDiscountPaymentModeFreeTrial: return AGK_PAYMENT_TYPE_FREE;
                        case SKProductDiscountPaymentModePayUpFront: return AGK_PAYMENT_TYPE_ONE_OFF;
                        case SKProductDiscountPaymentModePayAsYouGo: return AGK_PAYMENT_TYPE_RECURRING_LIMITED;
                    }
                    return AGK_PAYMENT_TYPE_UNKNOWN;
                }
                else
                {
                    return AGK_PAYMENT_TYPE_RECURRING_UNTIL_CANCELLED;
                }
            }
            else
            {
                return AGK_PAYMENT_TYPE_RECURRING_UNTIL_CANCELLED;
            }
        }
        else
        {
            if (@available(iOS 12.2, *))
            {
                plan--;
                
                if ( product.discounts == nil || plan >= [product.discounts count] )
                {
                    return AGK_PAYMENT_TYPE_UNKNOWN;
                }
                
                SKProductDiscount* discount = product.discounts[plan];
            
                if ( period > 0 )
                {
                    return AGK_PAYMENT_TYPE_RECURRING_UNTIL_CANCELLED;
                }
                else
                {
                    switch( discount.paymentMode )
                    {
                        case SKProductDiscountPaymentModeFreeTrial: return AGK_PAYMENT_TYPE_FREE;
                        case SKProductDiscountPaymentModePayUpFront: return AGK_PAYMENT_TYPE_ONE_OFF;
                        case SKProductDiscountPaymentModePayAsYouGo: return AGK_PAYMENT_TYPE_RECURRING_LIMITED;
                    }
                    return AGK_PAYMENT_TYPE_UNKNOWN;
                }
            }
            else
            {
                return AGK_PAYMENT_TYPE_RECURRING_UNTIL_CANCELLED;
            }
        }
    }
}

- ( char*) formatSubUnit:(void*)unit
{
    char* str = new char[ 4 ];
    str[ 0 ] = 0;
    str[ 1 ] = 0;
    str[ 2 ] = 0;
    str[ 3 ] = 0;
    
    if (@available(iOS 11.2, *))
    {
        SKProductSubscriptionPeriod* realUnit = (SKProductSubscriptionPeriod*) unit;
    
        switch( realUnit.unit )
        {
            case SKProductPeriodUnitDay: str[0] = 'D'; break;
            case SKProductPeriodUnitWeek: str[0] = 'W'; break;
            case SKProductPeriodUnitMonth: str[0] = 'M'; break;
            case SKProductPeriodUnitYear: str[0] = 'Y'; break;
        }
        
        uint32_t num = (uint32_t) realUnit.numberOfUnits;
        if ( num > 99 ) num = 99;
        sprintf( str+1, "%d", num );
    }
    
    return str;
}

- ( char*) getPlanDurationUnit: (int) iID plan:(int)plan period:(int)period
{
    @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil || plan < 0 || period < 0 )
        {
            char* str = new char[1];
            *str = 0;
            return str;
        }
        
        if ( plan == 0 )
        {
            if ( @available(iOS 11.2, *) )
            {
                if ( product.introductoryPrice != nil && period == 0 )
                {
                    return [self formatSubUnit:product.introductoryPrice.subscriptionPeriod];
                }
                else
                {
                    return [self formatSubUnit:product.subscriptionPeriod];
                }
            }
            else
            {
                char* str = new char[3];
                str[0] = 'M';
                str[1] = '1';
                str[2] = 0;
                return str;
            }
        }
        else
        {
            if (@available(iOS 12.2, *))
            {
                plan--;
                
                if ( product.discounts == nil || plan >= [product.discounts count] )
                {
                    return [self formatSubUnit:product.subscriptionPeriod];
                }
                
                SKProductDiscount* discount = product.discounts[plan];
            
                if ( period == 0 ) return [self formatSubUnit:discount.subscriptionPeriod];
                else return [self formatSubUnit:product.subscriptionPeriod];
            }
            else
            {
                char* str = new char[3];
                str[0] = 'M';
                str[1] = '1';
                str[2] = 0;
                return str;
            }
        }
    }
}

- ( int )  getPlanDuration: (int) iID plan:(int)plan period:(int)period
{
    @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil || plan < 0 || period < 0 )
        {
            return 0;
        }
        
        if ( plan == 0 )
        {
            if ( @available(iOS 11.2, *) )
            {
                if ( product.introductoryPrice != nil && period == 0 )
                {
                    return (int) product.introductoryPrice.numberOfPeriods;
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                return 1;
            }
        }
        else
        {
            if (@available(iOS 12.2, *))
            {
                plan--;
                
                if ( product.discounts == nil || plan >= [product.discounts count] )
                {
                    return 0;
                }
                
                SKProductDiscount* discount = product.discounts[plan];
            
                if ( period > 0 )
                {
                    return 1;
                }
                else
                {
                    return (int) discount.numberOfPeriods;
                }
            }
            else
            {
                return 1;
            }
        }
    }
}

- ( char*) getPlanTags: (int) iID plan:(int)plan
{
    // iOS doesn't have tags
    char* str = new char[1];
    *str = 0;
    return str;
}

- ( char*) getPlanToken: (int) iID plan:(int)plan
{
    @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil || plan <= 0 ) // plan 0 doesn't have a token
        {
            char* str = new char[1];
            *str = 0;
            return str;
        }
        
        if (@available(iOS 12.2, *))
        {
            plan--;
            
            if ( product.discounts == nil || plan >= [product.discounts count] )
            {
                char* str = new char[1];
                *str = 0;
                return str;
            }
            
            SKProductDiscount* discount = product.discounts[plan];
        
            const char* name = [discount.identifier UTF8String];
            char* str = new char[ strlen(name) + 1 ];
            strcpy( str, name );
            return str;
        }
        else
        {
            char* str = new char[1];
            *str = 0;
            return str;
        }
    }
}

- (char*) getDescription: (int) iID
{
    @synchronized ( self )
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil || product.localizedDescription == nil )
        {
            char *str = new char[1];
            *str = 0;
            return str;
        }
        else
        {
            NSString* desc = product.localizedDescription;
            const char* szDesc = [desc UTF8String];
            char* str = new char[ strlen(szDesc) + 1 ];
            strcpy( str, szDesc );
            return str;
        }
    }
}

- (char*) getSignature: (int) iID
{
	if ( iID < 0 || iID >= productCount || productSig[iID] == 0 )
    {
        char *str = new char[1];
        *str = 0;
        return str;
    }
    else
    {
        @synchronized ( self )
        {
            char *str = new char[ strlen(productSig[iID]) + 1 ];
            strcpy( str, productSig[iID] );
            return str;
        }
    }
}

- ( void ) purchaseUnlockableContent: ( int ) iID
{
	// call this function when you want to purchase some new content
    
    @synchronized (self)
    {
        SKProduct* product = [self getProductFromID:iID];
        if ( product == nil ) return;
        
        productState[ iID ] = 2; // in progress

        if ( [ SKPaymentQueue canMakePayments ] )
        {
            SKMutablePayment* payment = [ SKMutablePayment paymentWithProduct:product ];
            [ [ SKPaymentQueue defaultQueue ] addPayment: payment ];
        }
        else
        {
            NSString* pString = [ [ NSString alloc ] initWithUTF8String: productID [ iID ] ];
            productState[ iID ] = 0;
                
            UIAlertView *alert = [ [ UIAlertView alloc ] initWithTitle:pString message:@"You are not authorized to purchase from the App Store"
                                                           delegate:self cancelButtonTitle:@"OK" otherButtonTitles: nil ];
            [ alert show ];
            [ alert release ];
            [pString release];
        }
    }
}


- ( void ) cancelledTransaction: ( SKPaymentTransaction* ) transaction
{
	for ( int i = 0; i < productCount; i++ )
	{
        if ( productID[i] == 0 ) continue;
		NSString* pString = [ [ NSString alloc ] initWithUTF8String: productID [ i ] ];
		
		if ( [ transaction.payment.productIdentifier isEqualToString: pString ] )
		{
			productState [ i ] = 0; // not purchased
		}
	}
}

- ( void ) failedTransaction: ( SKPaymentTransaction* ) transaction
{
	for ( int i = 0; i < productCount; i++ )
	{
        if ( productID[i] == 0 ) continue;
		NSString* pString = [ [ NSString alloc ] initWithUTF8String: productID [ i ] ];
		
		if ( [ transaction.payment.productIdentifier isEqualToString: pString ] )
		{
			productState [ i ] = 0; // not purchased
		}
	}
	
	NSString* messageToBeShown = [ NSString stringWithFormat:@"Please try again later." ];
	UIAlertView* alert = [ [ UIAlertView alloc ] initWithTitle:@"Unable to complete your purchase" message:messageToBeShown
												   delegate:self cancelButtonTitle:@"OK" otherButtonTitles: nil ];
	[ alert show ];
	[ alert release ];
}

- ( void ) restore
{
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- ( void ) finishedRestore: (int) success
{
    
}

- ( void ) provideContent: ( NSString* ) productIdentifier signature:(NSString*) signature token:(NSString*) token
{
	for ( int i = 0; i < productCount; i++ )
	{
        if ( productID[i] == 0 ) continue;
		NSString* pString = [ [ NSString alloc ] initWithUTF8String: productID [ i ] ];
		
		if ( [ productIdentifier isEqualToString: pString ] )
		{
			productState [ i ] = 4; // purchased

			if ( signature )
			{
				@synchronized ( self )
				{
					const char* szSig = [signature UTF8String];
					if ( productSig[ i ] ) delete [] productSig[ i ];
					productSig[ i ] = new char[ strlen(szSig) + 1 ];
					strcpy( productSig[i], szSig );
                    
                    const char* szToken = [token UTF8String];
                    if ( productToken[ i ] ) delete [] productToken[ i ];
                    productToken[ i ] = new char[ strlen(szToken) + 1 ];
                    strcpy( productToken[i], szToken );
				}
			}
		}
        
        [pString release];
	}

	[ self updatePurchases ];
}

- ( void ) loadPurchases
{
	NSUserDefaults* userDefaults = [ NSUserDefaults standardUserDefaults ];
	
	for ( int i = 0; i < productCount; i++ )
	{
        if ( productID[i] == 0 ) continue;
		NSString* pString = [ [ NSString alloc ] initWithUTF8String: productID [ i ] ];
	
		productState [ i ] = [ userDefaults boolForKey:pString ] ? 4 : 0;
        [pString release];
	}
}

- ( void ) updatePurchases
{
	NSUserDefaults* userDefaults = [ NSUserDefaults standardUserDefaults ];
	
	for ( int i = 0; i < productCount; i++ )
	{
        if ( productID[i] == 0 ) continue;
		NSString* pString = [ [ NSString alloc ] initWithUTF8String: productID [ i ] ];
	
		[ userDefaults setBool: (productState [ i ] == 4) forKey: pString ];
        [pString release];
	}
}

- ( char* ) getToken: (int) iID
{
    if ( iID < 0 || iID >= productCount || !productToken[iID] )
    {
        char *str = new char[1];
        *str = 0;
        return str;
    }
    else
    {
        @synchronized ( self )
        {
            char *str = new char[ strlen(productToken[iID]) + 1 ];
            strcpy( str, productToken[iID] );
            return str;
        }
    }
}

- ( void ) resetPurchase: (const char*) token
{
    for ( int i = 0; i < productCount; i++ )
    {
        if ( productID[i] == 0 ) continue;
        
        if ( productToken[i] && strcmp(productToken[i], token) == 0 )
        {
            NSUserDefaults* userDefaults = [ NSUserDefaults standardUserDefaults ];
            NSString* pString = [ [ NSString alloc ] initWithUTF8String: productID[i] ];
            [ userDefaults removeObjectForKey:pString ];
            
            delete [] productToken[i];
            productToken[ i ] = 0;
            productState [ i ] = 0;
        }
    }
}
@end
