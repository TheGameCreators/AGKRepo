

#import "StoreObserver.h"
#import "StoreManager.h"

@implementation StoreObserver

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
	for (SKPaymentTransaction *transaction in transactions)
	{
		switch (transaction.transactionState)
		{
			case SKPaymentTransactionStatePurchasing:
			{
				// being processed by Apple
				NSLog ( @"SKPaymentTransactionStatePurchasing processed" );
			}
			break;
		
			case SKPaymentTransactionStatePurchased:
			{
				// everything was fine
				
				NSLog ( @"SKPaymentTransactionStatePurchased" );
			
			
				NSString *rawReceiptData = [[NSString alloc] initWithData:transaction.transactionReceipt encoding:NSUTF8StringEncoding];
                [[StoreManager sharedManager] provideContent: transaction.payment.productIdentifier signature:rawReceiptData token:transaction.transactionIdentifier ];
				[[SKPaymentQueue defaultQueue] finishTransaction: transaction];		
				[rawReceiptData release]; 
			}
			break;
				
			case SKPaymentTransactionStateFailed:
			{
				// transaction has failed
				
				NSLog ( @"SKPaymentTransactionStateFailed" );
                NSLog( @"%@", [transaction.error localizedDescription] );
                NSLog( @"%@", [transaction.error localizedFailureReason] );
				
				//int j = transaction.error.code;
				
				if ( transaction.error.code == SKErrorPaymentCancelled )
				{
					[[StoreManager sharedManager] cancelledTransaction: transaction];
				}
				
				//if (transaction.error.code != SKErrorPaymentCancelled)
				else
				{
					[[StoreManager sharedManager] failedTransaction: transaction];
					
				}
				
				[[SKPaymentQueue defaultQueue] finishTransaction: transaction];	
			}
			break;
			
			case SKPaymentTransactionStateRestored:
			{
				NSLog ( @"SKPaymentTransactionStateRestored" );
			
				// restored when something went wrong for example, user got a phone call when attempting to purchase
			
				NSString *rawReceiptData = [[NSString alloc] initWithData:transaction.originalTransaction.transactionReceipt encoding:NSUTF8StringEncoding];
				[[StoreManager sharedManager] provideContent: transaction.originalTransaction.payment.productIdentifier signature:rawReceiptData token:transaction.transactionIdentifier ];
				
				[[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                [rawReceiptData release];
			}
			break;
			
			default:
				break;
		}
	}
}

- (void) paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    for (SKPaymentTransaction *transaction in queue.transactions)
    {
        NSString *productID = transaction.payment.productIdentifier;
        
        NSString *rawReceiptData = [[NSString alloc] initWithData:transaction.transactionReceipt encoding:NSUTF8StringEncoding];
        [[StoreManager sharedManager] provideContent: productID signature:rawReceiptData token:transaction.transactionIdentifier];
    }
    
    NSLog(@"Restore Finished");
}

- ( void ) paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError: (NSError*) error
{
    NSLog(@"Restore Failed: %@", [error localizedDescription]);
}

- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray *)transactions
{
	
}

- (BOOL)paymentQueue:(SKPaymentQueue *)queue shouldAddStorePayment:(SKPayment *)payment forProduct:(SKProduct *)product
{
    return TRUE;
}

@end
