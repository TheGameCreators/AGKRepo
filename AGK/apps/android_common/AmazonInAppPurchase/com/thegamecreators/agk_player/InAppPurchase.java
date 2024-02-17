package com.thegamecreators.agk_player;

import android.app.Activity;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.amazon.device.iap.PurchasingListener;
import com.amazon.device.iap.PurchasingService;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.amazon.device.iap.model.FulfillmentResult;
import com.amazon.device.iap.model.Product;
import com.amazon.device.iap.model.ProductDataResponse;
import com.amazon.device.iap.model.PurchaseResponse;
import com.amazon.device.iap.model.PurchaseUpdatesResponse;
import com.amazon.device.iap.model.Receipt;
import com.amazon.device.iap.model.UserDataResponse;
import com.thegamecreators.agk_player.AGKHelper;
import static com.thegamecreators.agk_player.AGKHelper.ShowMessage;
import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

public class InAppPurchase implements PurchasingListener
{
    public static final int MAX_PRODUCTS = 100;

    enum IAPSetupState
    {
        INITIAL_STATE,
        IN_PROGRESS,
        FINSIHED,
        FAILED
    }

    enum IAPProductState
    {
        NOT_PURCHASED,
        QUEUED,
        IN_PROGRESS,
        PENDING,
        PURCHASED
    }

    static class IAPProduct
    {
        String name = "";
        int type = 0; // 0=non-consumable, 1=consumable, 2=subscription
        IAPProductState state = IAPProductState.NOT_PURCHASED;
        String lastSignature = "";
        String lastToken = "";
        String planToken = "";
        Product details = null;
    }

    public static final Object iapLock = new Object();
    public static IAPSetupState g_iIAPStatus = IAPSetupState.INITIAL_STATE;
    public static List<IAPProduct> g_pIAPProducts = new ArrayList<IAPProduct>();

    private static InAppPurchase purchasingListener = null;

    @Override
    public void onUserDataResponse(final UserDataResponse response) {
        Log.d("IAB", "onGetUserDataResponse: requestId (" + response.getRequestId()
                + ") userIdRequestStatus: "
                + response.getRequestStatus()
                + ")");
    }

    @Override
    public void onProductDataResponse(final ProductDataResponse response) {
        final ProductDataResponse.RequestStatus status = response.getRequestStatus();
        //Log.d(TAG, "onProductDataResponse: RequestStatus (" + status + ")");

        synchronized (iapLock)
        {
            switch (status) {
                case SUCCESSFUL:
                    //Log.d(TAG, "onProductDataResponse: successful.  The item data map in this response includes the valid SKUs");
                    final Map<String, Product> products = response.getProductData();
                    //Log.i( TAG, products.toString() );
                    if ( !response.getUnavailableSkus().isEmpty() ) Log.w( "IAB", "Invalid IAP SKUs: " + response.getUnavailableSkus().toString() );
                    //for (Map.Entry<String,Product> entry : products.entrySet())
                    for ( IAPProduct localProduct : g_pIAPProducts )
                    {
                        Product item = products.get( localProduct.name );
                        if ( item != null )
                        {
                            Log.i( "IAB", "Got details for " + localProduct.name );

                            synchronized (iapLock)
                            {
                                localProduct.details = item;
                            }
                        }
                    }

                    g_iIAPStatus = IAPSetupState.FINSIHED;

                    // get previous purchases
                    PurchasingService.getPurchaseUpdates(true);
                    break;

                case FAILED:
                case NOT_SUPPORTED:
                    Log.d("IAB", "onProductDataResponse: failed, should retry request");
                    g_iIAPStatus = IAPSetupState.FAILED;
                    break;
            }
        }
    }

    @Override
    public void onPurchaseUpdatesResponse(final PurchaseUpdatesResponse response) {

        //Log.d("IAB", "onPurchaseUpdatesResponse: " + response.getReceipts().toString());

        synchronized (iapLock)
        {
            final PurchaseUpdatesResponse.RequestStatus status = response.getRequestStatus();
            switch (status) {
                case SUCCESSFUL:
                    for (final Receipt receipt : response.getReceipts())
                    {
                        if ( !receipt.isCanceled() )
                        {
                            for ( IAPProduct localProduct : g_pIAPProducts )
                            {
                                if ( receipt.getSku().equals( localProduct.name ) )
                                {
                                    Log.i("IAB", "Update: Restored " + localProduct.name);
                                    localProduct.state = IAPProductState.PURCHASED;
                                    PurchasingService.notifyFulfillment(receipt.getReceiptId(), FulfillmentResult.FULFILLED);
                                    break;
                                }
                            }
                        }
                    }
                    if (response.hasMore()) {
                        PurchasingService.getPurchaseUpdates(false);
                    }
                    break;
                case FAILED:
                case NOT_SUPPORTED:
                    Log.d("IAB", "onProductDataResponse: failed, should retry request");
                    break;
            }
        }
    }

    @Override
    public void onPurchaseResponse(final PurchaseResponse response) {
        final String requestId = response.getRequestId().toString();
        final String userId = response.getUserData().getUserId();
        final PurchaseResponse.RequestStatus status = response.getRequestStatus();
        Receipt receipt = response.getReceipt();

        //Log.d("IAB", "onPurchaseResponse: " + receipt.toString());

        synchronized (iapLock)
        {
            switch (status) {
                case SUCCESSFUL:
                {
                    Log.d("IAB", "onPurchaseResponse: receipt json:" + receipt.toJSON());
                    switch (receipt.getProductType())
                    {
                        case CONSUMABLE:
                        case ENTITLED: // purchases
                        {
                            for ( IAPProduct localProduct : g_pIAPProducts )
                            {
                                if ( receipt.getSku().equals( localProduct.name ) )
                                {
                                    if (receipt.isCanceled())
                                    {
                                        Log.i("IAB", "Cancelled " + localProduct.name);
                                        localProduct.state = IAPProductState.NOT_PURCHASED;
                                    }
                                    else
                                    {
                                        Log.i("IAB", "Bought " + localProduct.name);
                                        localProduct.state = IAPProductState.PURCHASED;
                                        PurchasingService.notifyFulfillment(receipt.getReceiptId(), FulfillmentResult.FULFILLED);
                                        break;
                                    }
                                }
                            }
                        } break;

                        case SUBSCRIPTION:
                            // check subscription sample app to know how to handle subscription
                            // purchases
                            break;
                    }
                } break;

                case ALREADY_PURCHASED:
                {
                    Log.d("IAB", "onPurchaseResponse: already purchased: " + receipt.toString());
                    for ( IAPProduct localProduct : g_pIAPProducts )
                    {
                        if ( receipt.getSku().equals( localProduct.name ) )
                        {
                            Log.i("IAB", "Bought " + localProduct.name);
                            localProduct.state = IAPProductState.PURCHASED;
                            PurchasingService.notifyFulfillment(receipt.getReceiptId(), FulfillmentResult.FULFILLED);
                            break;
                        }
                    }
                } break;

                case INVALID_SKU:
                {
                    Log.d("IAB", "onPurchaseResponse: Invalid SKU");
                } break;

                case FAILED:
                {
                    Log.d("IAB", "onPurchaseResponse: failed");
                    if ( receipt != null )
                    {
                        for ( IAPProduct localProduct : g_pIAPProducts )
                        {
                            if ( receipt.getSku().equals( localProduct.name ) )
                            {
                                Log.i("IAB", "Failed " + localProduct.name);
                                localProduct.state = IAPProductState.NOT_PURCHASED;
                                break;
                            }
                        }
                    }
                } break;

                case NOT_SUPPORTED:
                {
                    Log.d("IAB", "onPurchaseResponse: Not Supported");
                } break;
            }
        }
    }

    public static void onStart( Activity act )
    {
        if ( purchasingListener == null )
        {
            purchasingListener = new InAppPurchase();
            PurchasingService.registerListener(act, purchasingListener);
        }

        if ( g_iIAPStatus == IAPSetupState.FINSIHED )
        {
            PurchasingService.getPurchaseUpdates(false);
        }
    }

    public static void iapSetKeyData( String publicKey, String developerID )
    {

    }

    public static void iapAddProduct( String name, int unused, int type )
    {
        if ( g_iIAPStatus != IAPSetupState.INITIAL_STATE )
        {
            ShowMessage(g_pAct, "Cannot add IAP product after calling InAppPurchaseSetup()" );
            return;
        }

        name = name.toLowerCase();
        Log.i("IAB","Adding product: " + name + " to ID: " + g_pIAPProducts.size());
        IAPProduct newProduct = new IAPProduct();
        newProduct.name = name;
        newProduct.lastToken = name;
        newProduct.type = type;
        g_pIAPProducts.add( newProduct );
    }

    public static void iapSetup( Activity act )
    {
        if ( g_iIAPStatus != IAPSetupState.INITIAL_STATE )
        {
            switch( g_iIAPStatus )
            {
                case IN_PROGRESS: ShowMessage( act, "Cannot set up IAP, setup is already in progress" ); break;
                case FINSIHED: ShowMessage( act, "Failed to call InAppPurchaseSetup(), setup has already been completed" ); break;
            }

            return;
        }

        g_iIAPStatus = IAPSetupState.IN_PROGRESS;

        // purchase listener is created in OnCreate

        final Set<String> productSkus = new HashSet<String>();
        for ( IAPProduct product : g_pIAPProducts )
        {
            productSkus.add( product.name );
        }
        PurchasingService.getProductData(productSkus);
    }

    public static void iapMakePurchase( Activity act, int ID )
    {
        iapMakePurchaseWithPlan( act, ID, "" );
    }

    public static void iapMakePurchaseWithPlan( Activity act, int ID, String planToken )
    {
        if ( ID < 0 || ID >= g_pIAPProducts.size() )
        {
            ShowMessage(act,"Invalid item ID");
            return;
        }

        synchronized (iapLock)
        {
            IAPProduct product = g_pIAPProducts.get( ID );

            if ( product.type == 0 && product.state == IAPProductState.PURCHASED )
            {
                ShowMessage(act,"You have already purchased that item");
                return; // non-consumable item already purchased
            }

            if ( g_iIAPStatus != IAPSetupState.FINSIHED )
            {
                switch( g_iIAPStatus )
                {
                    case FAILED: ShowMessage( act, "Cannot start purchase as IAP setup failed" ); break;
                    case INITIAL_STATE: ShowMessage( act, "Cannot start purchase as IAP has not been setup" ); break;
                    case IN_PROGRESS: ShowMessage(act, "Cannot start purchase until setup is finished, please try again in a minute"); break;
                }
                return;
            }

            product.state = IAPProductState.IN_PROGRESS;
            product.lastSignature = "";
            Log.i("IAB MakePurchase", "Buying " + product.name);

            PurchasingService.purchase( product.name );
        }
    }

    public static int iapCheckPurchaseState()
    {
        // deprecated in favour of iapCheckPurchase()
        return 0;
    }

    public static int iapCheckPurchase( int ID )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return 0;
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.state == IAPProductState.PURCHASED ) return 1;
            else return 0;
        }
    }

    public static int iapCheckPurchase2( int ID )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return 0;
            IAPProduct product = g_pIAPProducts.get( ID );
            switch( product.state )
            {
                case NOT_PURCHASED:  return 0;
                case QUEUED:         return 1;
                case IN_PROGRESS:    return 2;
                case PENDING:        return 3;
                case PURCHASED:      return 4;
                default:             return -1;
            }
        }
    }

    public static String iapGetPrice( int ID )
    {
        synchronized (iapLock)
        {
            if (ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            Product product = g_pIAPProducts.get( ID ).details;
            if ( product == null ) return "";
            return product.getPrice();
        }
    }

    public static String iapGetDescription( int ID )
    {
        synchronized (iapLock)
        {
            if (ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            Product product = g_pIAPProducts.get( ID ).details;
            if ( product == null ) return "";
            return product.getDescription();
        }
    }

    public static String iapGetSignature( int ID )
    {
        synchronized (iapLock)
        {
            if (ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            return g_pIAPProducts.get( ID ).lastSignature;
        }
    }

    public static void iapReset()
    {
        g_iIAPStatus = IAPSetupState.INITIAL_STATE;
        g_pIAPProducts.clear();
    }


    public static void iapRestore()
    {
        if ( g_iIAPStatus != IAPSetupState.FINSIHED ) return;

        PurchasingService.getPurchaseUpdates( false );
    }

    public static void iapResetPurchase( String token )
    {
        synchronized (iapLock)
        {
            if (token == "" ) return;
            for ( IAPProduct localProduct : g_pIAPProducts )
            {
                if ( token.equals( localProduct.lastToken ) )
                {
                    Log.i("IAB", "Reset " + localProduct.name);
                    localProduct.state = IAPProductState.NOT_PURCHASED;
                    break;
                }
            }
        }
    }

    public static String iapGetToken( int ID )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            IAPProduct product = g_pIAPProducts.get( ID );
            return product.lastToken;
        }
    }

    public static int iapGetNumPlans( int ID )
    {
        return 0;
    }

    public static int iapGetPlanNumPeriods( int ID, int planIndex )
    {
        return 0;
    }

    public static String iapGetPlanPrice( int ID, int planIndex, int periodIndex )
    {
        return "";
    }

    public static int iapGetPlanDuration( int ID, int planIndex, int periodIndex )
    {
        return 0;
    }

    public static String iapGetPlanDurationUnit( int ID, int planIndex, int periodIndex )
    {
        return "";
    }

    public static int iapGetPlanPaymentType( int ID, int planIndex, int periodIndex )
    {
        return -1;
    }

    public static String iapGetPlanTags( int ID, int planIndex )
    {
        return "";
    }

    public static String iapGetPlanToken( int ID, int planIndex )
    {
        return "";
    }
}