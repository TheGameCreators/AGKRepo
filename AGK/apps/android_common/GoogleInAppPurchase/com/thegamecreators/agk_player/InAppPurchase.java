package com.thegamecreators.agk_player;

import android.app.Activity;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.AcknowledgePurchaseResponseListener;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.ProductDetailsResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesResponseListener;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;

import java.util.ArrayList;
import java.util.List;

import com.google.common.collect.ImmutableList;
import com.thegamecreators.agk_player.AGKHelper;

public class InAppPurchase
{
    enum IAPProductState
    {
        NOT_PURCHASED,
        QUEUED,
        IN_PROGRESS,
        PENDING,
        PURCHASED
    }

    enum IAPSetupState
    {
        INITIAL_STATE,
        IN_PROGRESS,
        FINSIHED,
        FAILED
    }

    static class IAPProduct
    {
        String name = "";
        int type = 0; // 0=non-consumable, 1=consumable, 2=subscription
        int isRenewing = -1;
        IAPProductState state = IAPProductState.NOT_PURCHASED;
        String lastSignature = "";
        String lastToken = "";
        String planToken = "";
        ProductDetails details = null;
    }

    public static BillingClient billingClient = null;
    //public static int g_iNumProducts = 0;
    public static List<IAPProduct> g_pIAPProducts = new ArrayList<IAPProduct>();
    public static final Object iapLock = new Object();
    public static IAPSetupState g_iIAPStatus = IAPSetupState.INITIAL_STATE;
    private static int g_iapCallbackCount = 0;

    static ProductDetailsResponseListener billingProductListener = new ProductDetailsResponseListener() {
        @Override
        public void onProductDetailsResponse(BillingResult billingResult, List<ProductDetails> list)
        {
            if ( billingResult.getResponseCode() != BillingClient.BillingResponseCode.OK ) {
                Log.e("IAP","Failed to query products: " + billingResult.getResponseCode() +", " + billingResult.getDebugMessage() );
                g_iIAPStatus = IAPSetupState.FAILED;
                return;
            }

            int numProducts = (list != null) ? list.size() : -1;
            Log.i("IAP", "Query products was successful: " + numProducts );

            if ( list != null )
            {
                for (ProductDetails product : list) {
                    if (product == null) continue;

                    //Log.d("IAP", "product " + product.toString() );

                    Log.d("IAP", "product " + product.getName() + " ID: " + product.getProductId() + " Desc: " + product.getDescription());

                    String name = product.getProductId();
                    IAPProduct foundProduct = null;
                    for (IAPProduct localProduct : g_pIAPProducts) {
                        if (localProduct.name.equals(name)) {
                            foundProduct = localProduct;
                            break;
                        }
                    }
                    if (foundProduct == null) {
                        Log.e("IAP", "Unknown product: " + name);
                        continue;
                    }

                    synchronized (iapLock) {
                        foundProduct.details = product;
                    }
                }
            }

            synchronized(iapLock)
            {
                g_iapCallbackCount++;
                if ( g_iapCallbackCount >= 2 )
                {
                    g_iIAPStatus = IAPSetupState.FINSIHED;
                    iapRestore();
                }
            }
        }
    };

    static PurchasesUpdatedListener billingPurchaseListener = new PurchasesUpdatedListener() {
        @Override
        public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> list) {
            Log.d("IAP", "Purchases updated: " + ((list == null) ? 0 : list.size()) +", result: " + billingResult.getResponseCode() + ", " + billingResult.getDebugMessage());

            if ( BillingResultIsError( billingResult.getResponseCode() ) )
            {
                AGKHelper.ShowMessage( AGKHelper.g_pAct, "Failed to complete purchase, " + BillingResultMessage( billingResult.getResponseCode() ) );
            }

            // don't know if purchase list is for subscriptions or one-time purchases, so query them instead
            iapRestore();
        }
    };

    static PurchasesResponseListener billingInAppPurchaseResponseListener = new PurchasesResponseListener() {
        @Override
        public void onQueryPurchasesResponse( BillingResult billingResult, List<Purchase> list) {
            internalHandlePurchases( list, billingResult, 0 );
        }
    };

    static PurchasesResponseListener billingSubPurchaseResponseListener = new PurchasesResponseListener() {
        @Override
        public void onQueryPurchasesResponse( BillingResult billingResult, List<Purchase> list) {
            internalHandlePurchases( list, billingResult, 2 );
        }
    };

    private static void internalHandlePurchases(List<Purchase> list, BillingResult billingResult, int type )
    {
        int numPurchases = list != null ? list.size() : -1;
        Log.i( "IAP", "Query purchases successful: " + numPurchases );

        if ( list == null ) return;

        synchronized (iapLock)
        {
            if ( billingResult.getResponseCode() == BillingClient.BillingResponseCode.USER_CANCELED )
            {
                synchronized (iapLock) {
                    for (IAPProduct product : g_pIAPProducts) {
                        if (product.state == IAPProductState.IN_PROGRESS)
                            product.state = IAPProductState.NOT_PURCHASED;
                    }
                }
                return;
            }

            // list contains everything purchased for the given category (inapp, subscription), so
            // set everything in that category as unpurchased then process list
            for (IAPProduct product : g_pIAPProducts)
            {
                int prodType = product.type;
                if ( prodType == 1 ) prodType = 0;
                if ( prodType == type )
                {
                    product.state = IAPProductState.NOT_PURCHASED;
                    product.isRenewing = -1;
                }
            }

            for (Purchase purchase : list)
            {
                for( String purchaseProd : purchase.getProducts() )
                {
                    boolean found = false;
                    for (IAPProduct product : g_pIAPProducts) {
                        int prodType = product.type;
                        if (prodType == 1) prodType = 0;
                        if (prodType == type && product.name.equals(purchaseProd)) {
                            found = true;
                            Log.d("IAP", "Handling purchase for " + product.name + ", State: " + purchase.getPurchaseState());

                            if (purchase.getPurchaseState() == Purchase.PurchaseState.PENDING) {
                                synchronized (iapLock) {
                                    product.state = IAPProductState.PENDING;
                                }
                            } else if (purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED) {
                                synchronized (iapLock) {
                                    product.lastSignature = purchase.getSignature();
                                    product.lastToken = purchase.getPurchaseToken();
                                    product.state = IAPProductState.PURCHASED;
                                    if ( type == 2 )
                                    {
                                        product.isRenewing = purchase.isAutoRenewing() ? 1 : 0;
                                    }
                                }

                                // is it consumable? This should be phased out in favour of iapResetPurchase
                                if (product.type == 1) {
                                    ConsumeParams params = ConsumeParams.newBuilder().setPurchaseToken(purchase.getPurchaseToken()).build();
                                    billingClient.consumeAsync(params, new ConsumeResponseListener() {
                                        @Override
                                        public void onConsumeResponse(@NonNull BillingResult billingResult, @NonNull String s) {
                                            Log.d("IAP", "Consumption finished: " + billingResult.getResponseCode());
                                        }
                                    });
                                } else if (!purchase.isAcknowledged()) {
                                    AcknowledgePurchaseParams params = AcknowledgePurchaseParams.newBuilder().setPurchaseToken(purchase.getPurchaseToken()).build();
                                    billingClient.acknowledgePurchase(params, new AcknowledgePurchaseResponseListener() {
                                        @Override
                                        public void onAcknowledgePurchaseResponse(@NonNull BillingResult billingResult) {
                                            Log.d("IAP", "Acknowledge finished: " + billingResult.getResponseCode());
                                        }
                                    });
                                }
                            }

                            break;
                        }
                    }

                    if (!found)
                        Log.e("IAP", "Product not found for purchase " + purchaseProd + ", State: " + purchase.getPurchaseState());
                }
            }
        }
    }

    public static void onStart( Activity act )
    {
        refreshProducts();
        //iapRestore();
    }

    static boolean BillingResultIsError( int code )
    {
        switch( code ) {
            case BillingClient.BillingResponseCode.SERVICE_TIMEOUT:
            case BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED:
            case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED:
            case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE:
            case BillingClient.BillingResponseCode.BILLING_UNAVAILABLE:
            case BillingClient.BillingResponseCode.ITEM_UNAVAILABLE:
            case BillingClient.BillingResponseCode.DEVELOPER_ERROR:
                return true;
        }
        return false;
    }

    static String BillingResultMessage( int code )
    {
        switch( code )
        {
            case BillingClient.BillingResponseCode.SERVICE_TIMEOUT: return "Billing service timed out";
            case BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED: return "Feature not supported";
            case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED: return "Billing service disconnected";
            case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE: return "Billing service unavailable";
            case BillingClient.BillingResponseCode.BILLING_UNAVAILABLE: return "Billing unavailable";
            case BillingClient.BillingResponseCode.ITEM_UNAVAILABLE: return "Item unavailable";
            case BillingClient.BillingResponseCode.DEVELOPER_ERROR: return "Process error";
            case BillingClient.BillingResponseCode.ERROR: return "Unknown error";
            case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: return "Item already owned";
            case BillingClient.BillingResponseCode.ITEM_NOT_OWNED: return "Item not owned";
            case BillingClient.BillingResponseCode.OK: return "Ok";
            case BillingClient.BillingResponseCode.USER_CANCELED: return "User cancelled";
            default: return "Unknown";
        }
    }

    public static void iapSetKeyData( String publicKey, String developerID )
    {
        // do nothing
    }

    public static void iapReset()
    {
        g_iIAPStatus = IAPSetupState.INITIAL_STATE;
        g_pIAPProducts.clear();
    }

    public static void iapAddProduct( String name, int unused, int type )
    {
        if ( g_iIAPStatus != IAPSetupState.INITIAL_STATE )
        {
            if ( AGKHelper.g_pAct != null ) AGKHelper.ShowMessage( AGKHelper.g_pAct, "Cannot add IAP product after calling InAppPurchaseSetup()" );
            return;
        }

        name = name.toLowerCase();
        Log.i("IAP","Adding product: " + name + " to ID: " + g_pIAPProducts.size());
        IAPProduct newProduct = new IAPProduct();
        newProduct.name = name;
        newProduct.type = type;
        g_pIAPProducts.add( newProduct );
    }

    private static void refreshProducts()
    {
        ArrayList<QueryProductDetailsParams.Product> products = new ArrayList<QueryProductDetailsParams.Product>();
        ArrayList<QueryProductDetailsParams.Product> subscriptions = new ArrayList<QueryProductDetailsParams.Product>();

        for( IAPProduct product : g_pIAPProducts )
        {
            QueryProductDetailsParams.Product.Builder newProduct = QueryProductDetailsParams.Product.newBuilder();
            newProduct.setProductId( product.name );

            if ( product.type == 2 )
            {
                newProduct.setProductType( BillingClient.ProductType.SUBS );
                subscriptions.add( newProduct.build() );
            }
            else
            {
                newProduct.setProductType( BillingClient.ProductType.INAPP );
                products.add( newProduct.build() );
            }
        }

        synchronized (iapLock)
        {
            g_iapCallbackCount = 0;
            if ( products.size() == 0 ) g_iapCallbackCount++;
            if ( subscriptions.size() == 0 ) g_iapCallbackCount++;
        }

        if ( products.size() > 0 )
        {
            // query products
            QueryProductDetailsParams.Builder params = QueryProductDetailsParams.newBuilder();
            params.setProductList( products );
            billingClient.queryProductDetailsAsync( params.build(), billingProductListener );
        }

        if ( subscriptions.size() > 0 )
        {
            // query subscriptions
            QueryProductDetailsParams.Builder params = QueryProductDetailsParams.newBuilder();
            params.setProductList( subscriptions );
            billingClient.queryProductDetailsAsync( params.build(), billingProductListener );
        }
    }

    public static void iapSetup( Activity act )
    {
        switch( g_iIAPStatus )
        {
            case IN_PROGRESS: AGKHelper.ShowMessage( act, "Cannot set up IAP, setup is already in progress" ); return;
            case FINSIHED: AGKHelper.ShowMessage(act, "Failed to call InAppPurchaseSetup(), setup has already been completed"); return;
        }

        g_iIAPStatus = IAPSetupState.IN_PROGRESS;

        if ( billingClient == null ) {
            billingClient = BillingClient.newBuilder(act)
                    .setListener(billingPurchaseListener)
                    .enablePendingPurchases()
                    .build();
        }

        Log.i( "IAP", "Starting billing service" );

        billingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(BillingResult billingResult) {
                if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    Log.i( "IAP", "Billing service connected, querying products" );

                    refreshProducts();
                }
                else
                {
                    Log.w( "IAP", "Billing service connection error: " + billingResult.getResponseCode() + ", " + billingResult.getDebugMessage() );
                    g_iIAPStatus = IAPSetupState.FAILED;
                }
            }

            @Override
            public void onBillingServiceDisconnected() {
                Log.w( "IAP", "Billing service disconnected" );
            }
        });
    }

    private static void internalRefreshPurchases()
    {
        QueryPurchasesParams.Builder purchaseParams = QueryPurchasesParams.newBuilder();
        purchaseParams.setProductType( BillingClient.ProductType.INAPP );
        billingClient.queryPurchasesAsync( purchaseParams.build(), billingInAppPurchaseResponseListener );

        purchaseParams = QueryPurchasesParams.newBuilder();
        purchaseParams.setProductType( BillingClient.ProductType.SUBS );
        billingClient.queryPurchasesAsync( purchaseParams.build(), billingSubPurchaseResponseListener );
    }

    public static void iapRestore()
    {
        if ( billingClient == null ) return;
        if ( g_iIAPStatus != IAPSetupState.FINSIHED ) return;

        if ( billingClient.isReady() )
        {
            Log.i( "IAP", "Querying purchases" );
            internalRefreshPurchases();
        }
        else
        {
            billingClient.startConnection(new BillingClientStateListener() {
                @Override
                public void onBillingSetupFinished(BillingResult billingResult) {
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        Log.d( "IAP", "Billing service reconnected, querying purchases" );

                        internalRefreshPurchases();
                    }
                    else
                    {
                        Log.w( "IAP", "Billing service reconnection error: " + billingResult.getResponseCode() + ", " + billingResult.getDebugMessage() );
                    }
                }

                @Override
                public void onBillingServiceDisconnected() {
                    Log.w( "IAP", "Billing service disconnected" );
                }
            });
        }
    }

    private static void internalProcessPurchaseQueue()
    {
        synchronized (iapLock)
        {
            for (IAPProduct product : g_pIAPProducts)
            {
                // only process queued products
                if (product.state != IAPProductState.QUEUED) continue;
                product.state = IAPProductState.IN_PROGRESS;

                BillingFlowParams.ProductDetailsParams.Builder newProduct = BillingFlowParams.ProductDetailsParams.newBuilder();
                newProduct.setProductDetails(product.details);
                newProduct.setOfferToken(product.planToken);
                product.planToken = "";

                BillingFlowParams billingFlowParams = BillingFlowParams.newBuilder()
                        .setProductDetailsParamsList(ImmutableList.of(newProduct.build()))
                        .build();

                BillingResult result = billingClient.launchBillingFlow(AGKHelper.g_pAct, billingFlowParams);

                if (result.getResponseCode() != BillingClient.BillingResponseCode.OK) {
                    Log.e("IAP", "Failed to start purchase: " + result.getResponseCode() + ", " + result.getDebugMessage());
                    AGKHelper.ShowMessage(AGKHelper.g_pAct, "Failed to start purchase process, billing library returned the following error: " + result.getResponseCode() + ", " + result.getDebugMessage());
                }
            }
        }
    }

    public static void iapMakePurchase( Activity act, int ID )
    {
        iapMakePurchaseWithPlan( act, ID, "" );
    }

    public static void iapMakePurchaseWithPlan( Activity act, int ID, String planToken )
    {
        if ( ID < 0 || ID >= g_pIAPProducts.size() )
        {
            AGKHelper.ShowMessage(act,"Invalid item ID");
            return;
        }

        IAPProduct product = g_pIAPProducts.get( ID );

        // if not consumable
        if ( product.type != 1 )
        {
            if ( product.state == IAPProductState.QUEUED )
            {
                AGKHelper.ShowMessage(act,"A purchase for that product is already in progress");
                return;
            }
        }

        if ( g_iIAPStatus != IAPSetupState.FINSIHED )
        {
            switch( g_iIAPStatus )
            {
                case FAILED: AGKHelper.ShowMessage( act, "Cannot start purchase as IAP setup failed" ); break;
                case INITIAL_STATE: AGKHelper.ShowMessage( act, "Cannot start purchase as IAP has not been setup" ); break;
                case IN_PROGRESS: AGKHelper.ShowMessage(act, "Cannot start purchase until setup is finished, please try again in a minute"); break;
            }
            return;
        }

        synchronized (iapLock)
        {
            if ( product.details == null )
            {
                AGKHelper.ShowMessage( act, "Product not recognised by Google Billing Library" );
                return;
            }

            product.state = IAPProductState.QUEUED; // queued
            product.lastSignature = "";
            product.lastToken = "";
            product.planToken = planToken;
        }
        String planText = "";
        if ( !planToken.equals("") ) planToken = " with plan " + planToken.substring(0, 20) + "...";
        Log.i("IAP", "Starting purchase for " + product.name + planText );

        if ( billingClient.isReady() )
        {
            internalProcessPurchaseQueue();
        }
        else
        {
            billingClient.startConnection(new BillingClientStateListener() {
                @Override
                public void onBillingSetupFinished(BillingResult billingResult) {
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        Log.w( "IAP", "Billing service reconnected, starting queued purchases" );

                        internalProcessPurchaseQueue();
                    }
                    else
                    {
                        Log.w( "IAP", "Billing service reconnection error: " + billingResult.getResponseCode() + ", " + billingResult.getDebugMessage() );
                    }
                }

                @Override
                public void onBillingServiceDisconnected() {
                    Log.w( "IAP", "Billing service disconnected" );
                }
            });
        }
    }

    private static void internalResetPurchase( String token )
    {
        for (IAPProduct product : g_pIAPProducts)
        {
            if (product.lastToken.equals(token))
            {
                if ( product.type == 2 )
                {
                    AGKHelper.ShowMessage( AGKHelper.g_pAct, "Cannot reset a subscription, you can cancel it from your Google Play account" );
                    return;
                }

                ConsumeParams params = ConsumeParams.newBuilder().setPurchaseToken(token).build();
                billingClient.consumeAsync(params, new ConsumeResponseListener() {
                    @Override
                    public void onConsumeResponse(@NonNull BillingResult billingResult, @NonNull String s) {
                        Log.d("IAP", "Consumption finished: " + billingResult.getDebugMessage());
                    }
                });
                synchronized (iapLock)
                {
                    product.state = IAPProductState.NOT_PURCHASED;
                    product.lastToken = "";
                    product.lastSignature = "";
                    product.planToken = "";
                }
                return;
            }
        }
    }

    public static void iapResetPurchase( String token )
    {
        if ( billingClient == null ) return;
        if ( g_iIAPStatus != IAPSetupState.FINSIHED ) return;

        if ( billingClient.isReady() )
        {
            internalResetPurchase( token );
        }
        else
        {
            billingClient.startConnection(new BillingClientStateListener() {
                @Override
                public void onBillingSetupFinished(BillingResult billingResult) {
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        Log.w( "IAP", "Billing service reconnected, consuming purchase" );

                        internalResetPurchase( token );
                    }
                    else
                    {
                        Log.w( "IAP", "Billing service reconnection error: " + billingResult.getResponseCode() + ", " + billingResult.getDebugMessage() );
                    }
                }

                @Override
                public void onBillingServiceDisconnected() {
                    Log.w( "IAP", "Billing service disconnected" );
                }
            });
        }
    }

    public static int iapCheckPurchaseState()
    {
        return 1; // deprecated in favor of iapCheckPurchase
    }

    public static int iapCheckPurchase( int ID )
    {
        if ( ID < 0 || ID >= g_pIAPProducts.size() ) return 0;
        IAPProduct product = g_pIAPProducts.get( ID );
        if ( product.state == IAPProductState.PURCHASED ) return 1;
        else return 0;
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
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return "";
            if ( product.details.getProductType().equals( BillingClient.ProductType.INAPP ) ) {
                return product.details.getOneTimePurchaseOfferDetails().getFormattedPrice();
            }
            else
            {
                // this may not be the right price, but don't know which one to return for subscriptions
                // use iapGetPlanPrice instead
                return product.details.getSubscriptionOfferDetails().get(0).getPricingPhases().getPricingPhaseList().get(0).getFormattedPrice();
            }
        }
    }

    public static String iapGetDescription( int ID )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return "";
            return product.details.getDescription();
        }
    }

    public static String iapGetSignature( int ID )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            IAPProduct product = g_pIAPProducts.get( ID );
            return product.lastSignature;
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
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return 0;
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return 0;
            if ( !product.details.getProductType().equals( BillingClient.ProductType.SUBS ) ) return 0;

            return product.details.getSubscriptionOfferDetails().size();
        }
    }

    public static int iapGetPlanNumPeriods( int ID, int planIndex )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return 0;
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return 0;
            if ( !product.details.getProductType().equals( BillingClient.ProductType.SUBS ) ) return 0;

            int numPlans = product.details.getSubscriptionOfferDetails().size();
            if ( planIndex < 0 || planIndex >= numPlans ) return 0;

            return product.details.getSubscriptionOfferDetails().get( planIndex ).getPricingPhases().getPricingPhaseList().size();
        }
    }

    public static String iapGetPlanPrice( int ID, int planIndex, int periodIndex )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return "Error";
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return "";
            if ( !product.details.getProductType().equals( BillingClient.ProductType.SUBS ) ) return "Error";

            int numPlans = product.details.getSubscriptionOfferDetails().size();
            if ( planIndex < 0 || planIndex >= numPlans ) return "Error";

            List<ProductDetails.PricingPhase> planPeriods = product.details.getSubscriptionOfferDetails().get( planIndex ).getPricingPhases().getPricingPhaseList();
            if ( periodIndex < 0 || periodIndex >= planPeriods.size() ) return "Error";
            return planPeriods.get( periodIndex ).getFormattedPrice();
        }
    }

    public static int iapGetPlanDuration( int ID, int planIndex, int periodIndex )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return 0;
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return 0;
            if ( !product.details.getProductType().equals( BillingClient.ProductType.SUBS ) ) return 0;

            int numPlans = product.details.getSubscriptionOfferDetails().size();
            if ( planIndex < 0 || planIndex >= numPlans ) return 0;

            List<ProductDetails.PricingPhase> planPeriods = product.details.getSubscriptionOfferDetails().get( planIndex ).getPricingPhases().getPricingPhaseList();
            if ( periodIndex < 0 || periodIndex >= planPeriods.size() ) return 0;
            int count = planPeriods.get( periodIndex ).getBillingCycleCount();
            if ( count == 0 ) count = 1;

            return count;
        }
    }

    public static String iapGetPlanDurationUnit( int ID, int planIndex, int periodIndex )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return "";
            if ( !product.details.getProductType().equals( BillingClient.ProductType.SUBS ) ) return "";

            int numPlans = product.details.getSubscriptionOfferDetails().size();
            if ( planIndex < 0 || planIndex >= numPlans ) return "";

            List<ProductDetails.PricingPhase> planPeriods = product.details.getSubscriptionOfferDetails().get( planIndex ).getPricingPhases().getPricingPhaseList();
            if ( periodIndex < 0 || periodIndex >= planPeriods.size() ) return "";

            String unit = planPeriods.get( periodIndex ).getBillingPeriod();
            if ( unit.charAt(0) == 'P' ) unit = unit.substring( 1 );
            StringBuilder num = new StringBuilder( unit.length() );
            for( int i = 0; i < unit.length(); i++ )
            {
                char c = unit.charAt( i );
                if ( c < 48 || c > 57 ) break;
                num.append( c );
            }
            StringBuilder output = new StringBuilder();
            output.append( unit.charAt( unit.length()-1 ) );
            output.append( num );
            return output.toString();
        }
    }

    public static int iapGetPlanPaymentType( int ID, int planIndex, int periodIndex )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return 0;
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return 0;
            if ( !product.details.getProductType().equals( BillingClient.ProductType.SUBS ) ) return 0;

            int numPlans = product.details.getSubscriptionOfferDetails().size();
            if ( planIndex < 0 || planIndex >= numPlans ) return 0;

            List<ProductDetails.PricingPhase> planPeriods = product.details.getSubscriptionOfferDetails().get( planIndex ).getPricingPhases().getPricingPhaseList();
            if ( periodIndex < 0 || periodIndex >= planPeriods.size() ) return 0;
            ProductDetails.PricingPhase period = planPeriods.get( periodIndex );
            if ( period.getFormattedPrice().equalsIgnoreCase("Free") ) return 0;

            switch( period.getRecurrenceMode() )
            {
                case ProductDetails.RecurrenceMode.NON_RECURRING: return 1;
                case ProductDetails.RecurrenceMode.FINITE_RECURRING: return 2;
                case ProductDetails.RecurrenceMode.INFINITE_RECURRING: return 3;
            }

            return -1;
        }
    }

    public static String iapGetPlanTags( int ID, int planIndex )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return "";
            if ( !product.details.getProductType().equals( BillingClient.ProductType.SUBS ) ) return "";

            int numPlans = product.details.getSubscriptionOfferDetails().size();
            if ( planIndex < 0 || planIndex >= numPlans ) return "";

            ProductDetails.SubscriptionOfferDetails plan = product.details.getSubscriptionOfferDetails().get( planIndex );
            if ( plan.getOfferTags().size() == 0 ) return "";

            String tags = "";
            for( String tag : plan.getOfferTags() )
            {
                tags += ";" + tag;
            }

            return tags.substring( 1 );
        }
    }

    public static String iapGetPlanToken( int ID, int planIndex )
    {
        synchronized (iapLock)
        {
            if ( ID < 0 || ID >= g_pIAPProducts.size() ) return "";
            IAPProduct product = g_pIAPProducts.get( ID );
            if ( product.details == null ) return "";
            if ( !product.details.getProductType().equals( BillingClient.ProductType.SUBS ) ) return "";

            int numPlans = product.details.getSubscriptionOfferDetails().size();
            if ( planIndex < 0 || planIndex >= numPlans ) return "";

            ProductDetails.SubscriptionOfferDetails plan = product.details.getSubscriptionOfferDetails().get( planIndex );
            return plan.getOfferToken();
        }
    }
}