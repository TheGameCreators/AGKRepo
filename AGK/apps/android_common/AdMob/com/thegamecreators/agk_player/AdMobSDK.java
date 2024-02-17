package com.thegamecreators.agk_player;

import static com.google.android.gms.ads.RequestConfiguration.MAX_AD_CONTENT_RATING_G;
import static com.google.android.gms.ads.RequestConfiguration.TAG_FOR_CHILD_DIRECTED_TREATMENT_TRUE;
import static com.google.android.gms.ads.RequestConfiguration.TAG_FOR_UNDER_AGE_OF_CONSENT_TRUE;

import android.app.Activity;
import android.content.Context;
import android.graphics.PixelFormat;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.OnUserEarnedRewardListener;
import com.google.android.gms.ads.RequestConfiguration;
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;
import com.google.android.gms.ads.rewarded.RewardItem;
import com.google.android.gms.ads.rewarded.RewardedAd;
import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

class RunnableAd implements Runnable
{
    public Activity act;
    public int action = 0;
    public int horz = 1;
    public int vert = 2;
    public int offsetX = 0;
    public int offsetY = 0;
    public String pubID = "";
    public String rewardpubID = "";

    public static int testMode = 0;
    public static AdView ad = null;
    public static int adType = 0;
    public static InterstitialAd interstitialAd = null;
    public static int interstitialLoading = 0;
    InterstitialAdLoadCallback callbackLoadI = null;
    FullScreenContentCallback callbackShowI = null;

    public static RewardedAd rewardAd = null;
    public static int rewardLoading = 0;
    RewardedAdLoadCallback callbackLoad = null;
    OnUserEarnedRewardListener callbackReward = null;
    FullScreenContentCallback callbackShow = null;

    public WindowManager.LayoutParams makeLayout()
    {
        WindowManager.LayoutParams ll_lp;

        //Just a sample layout parameters.
        ll_lp = new WindowManager.LayoutParams();
        ll_lp.format = PixelFormat.TRANSPARENT;
        ll_lp.height = -2;
        ll_lp.width = -2;
        ll_lp.gravity = 0;
        ll_lp.x = offsetX;
        ll_lp.y = offsetY;
        switch( horz )
        {
            case 0: ll_lp.gravity |= Gravity.LEFT; break;
            case 1: ll_lp.gravity |= Gravity.CENTER_HORIZONTAL; break;
            case 2: ll_lp.gravity |= Gravity.RIGHT; break;
            default: ll_lp.gravity |= Gravity.CENTER_HORIZONTAL; break;
        }
        switch( vert )
        {
            case 0: ll_lp.gravity |= Gravity.TOP; break;
            case 1: ll_lp.gravity |= Gravity.CENTER_VERTICAL; break;
            case 2: ll_lp.gravity |= Gravity.BOTTOM; break;
            default: ll_lp.gravity |= Gravity.BOTTOM; break;
        }
        ll_lp.type = WindowManager.LayoutParams.TYPE_APPLICATION_PANEL;
        ll_lp.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
        ll_lp.flags = ll_lp.flags | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;

        return ll_lp;
    }

    public static final String md5(final String s) {
        try {
            // Create MD5 Hash
            MessageDigest digest = java.security.MessageDigest.getInstance("MD5");
            digest.update(s.getBytes());
            byte messageDigest[] = digest.digest();

            // Create Hex String
            StringBuffer hexString = new StringBuffer();
            for (int i = 0; i < messageDigest.length; i++) {
                String h = Integer.toHexString(0xFF & messageDigest[i]);
                while (h.length() < 2)
                    h = "0" + h;
                hexString.append(h);
            }
            return hexString.toString();

        } catch (NoSuchAlgorithmException e) {
            Log.e("AdMob",e.toString());
        }
        return "";
    }

    private void LoadInterstitialAd()
    {
        if ( interstitialLoading == 1 ) return;
        interstitialLoading = 1;

        Log.i("AdMob", "Loading interstitial ad");

        interstitialAd = null;
        AdRequest request = new AdRequest.Builder().build();
        InterstitialAd.load( act, (testMode == 1) ? "ca-app-pub-3940256099942544/5224354917" : pubID, request, callbackLoadI );
    }

    private void CheckInterstitialAdInit()
    {
        if ( callbackShowI == null ) {
            callbackShowI = new FullScreenContentCallback() {
                @Override
                public void onAdFailedToShowFullScreenContent(AdError adError) {
                    super.onAdFailedToShowFullScreenContent(adError);
                    Log.e("AdMob", "Failed to show interstitial ad: " + adError.getMessage());
                    interstitialAd = null;
                }

                @Override
                public void onAdShowedFullScreenContent() {
                    super.onAdShowedFullScreenContent();
                    interstitialAd = null;
                }

                @Override
                public void onAdDismissedFullScreenContent() {
                    super.onAdDismissedFullScreenContent();
                    LoadInterstitialAd();
                }
            };
        }

        if ( callbackLoadI == null )
        {
            callbackLoadI = new InterstitialAdLoadCallback()
            {
                @Override
                public void onAdLoaded( InterstitialAd ad )
                {
                    Log.i( "AdMob", "Interstitial ad loaded" );
                    interstitialAd = ad;
                    interstitialLoading = 0;
                }

                @Override
                public void onAdFailedToLoad(LoadAdError adError)
                {
                    Log.e( "AdMob", "Failed to load interstitial ad: " + adError.toString() );
                    interstitialAd = null;
                    interstitialLoading = 0;
                }
            };
        }
    }

    private void LoadRewardAd()
    {
        if ( rewardLoading == 1 ) return;
        rewardLoading = 1;

        Log.i("AdMob", "Loading reward ad");

        rewardAd = null;
        AdRequest request = new AdRequest.Builder().build();
        RewardedAd.load( act, (testMode == 1) ? "ca-app-pub-3940256099942544/5224354917" : rewardpubID, request, callbackLoad );
    }

    private void CheckRewardAdInit()
    {
        if ( callbackShow == null ) {
            callbackShow = new FullScreenContentCallback() {
                @Override
                public void onAdFailedToShowFullScreenContent(AdError adError) {
                    super.onAdFailedToShowFullScreenContent(adError);
                    Log.e("AdMob", "Failed to show reward ad: " + adError.getMessage());
                    rewardAd = null;
                }

                @Override
                public void onAdShowedFullScreenContent() {
                    super.onAdShowedFullScreenContent();
                    rewardAd = null;
                }

                @Override
                public void onAdDismissedFullScreenContent() {
                    super.onAdDismissedFullScreenContent();
                    LoadRewardAd();
                }
            };
        }

        if ( callbackReward == null )
        {
            callbackReward = new OnUserEarnedRewardListener() {
                @Override
                public void onUserEarnedReward( RewardItem rewardItem ) {
                    Log.i("AdMob", "Reward ad rewarded: " + Integer.toString(rewardItem.getAmount()) );
                    AdMobSDK.m_iRewardAdRewarded = 1;
                    AdMobSDK.m_iRewardAdValue = rewardAd.getRewardItem().getAmount();
                }
            };
        }

        if ( callbackLoad == null )
        {
            callbackLoad = new RewardedAdLoadCallback()
            {
                @Override
                public void onAdLoaded( RewardedAd ad )
                {
                    Log.i( "AdMob", "Reward ad loaded" );
                    rewardAd = ad;
                    rewardLoading = 0;
                    AdMobSDK.m_iRewardAdValue = rewardAd.getRewardItem().getAmount();
                }

                @Override
                public void onAdFailedToLoad(LoadAdError adError)
                {
                    Log.e( "AdMob", "Failed to load reward ad: " + adError.toString() );
                    rewardAd = null;
                    rewardLoading = 0;
                }
            };
        }
    }

    public void run()
    {
        switch ( action )
        {
            case 1: // make ad
            {
                if ( ad == null )
                {
                    ad = new AdView(act);
                    ad.setAdUnitId(pubID);
                    switch (adType)
                    {
                        case 0: ad.setAdSize(AdSize.BANNER); break;
                        case 1: ad.setAdSize(AdSize.LARGE_BANNER); break;
                        case 2: ad.setAdSize(AdSize.MEDIUM_RECTANGLE); break;
                        case 3: ad.setAdSize(AdSize.FULL_BANNER); break;
                        case 4: ad.setAdSize(AdSize.LEADERBOARD); break;
                        case 5: ad.setAdSize(AdSize.SMART_BANNER); break;
                        case 6: ad.setAdSize(AdSize.FLUID); break;
                        default: ad.setAdSize(AdSize.BANNER);
                    }

                    WindowManager wm = (WindowManager) act.getSystemService(Context.WINDOW_SERVICE);
                    WindowManager.LayoutParams layout = makeLayout();
                    if ( adType == 5 ) layout.width = -1;
                    wm.addView(ad, layout);

                    AdRequest.Builder request = new AdRequest.Builder();

                    ad.loadAd( request.build() );
                }
                break;
            }

            case 2: // position ad
            {
                if ( ad == null ) return;

                WindowManager wm = (WindowManager) act.getSystemService(Context.WINDOW_SERVICE);
                WindowManager.LayoutParams layout = makeLayout();
                if ( adType == 5 ) layout.width = -1;
                wm.updateViewLayout(ad, layout);

                break;
            }

            case 3: // delete the ad
            {
                if ( ad == null ) return;

                WindowManager wm = (WindowManager) act.getSystemService(Context.WINDOW_SERVICE);
                wm.removeView(ad);

                ad = null;
                break;
            }

            case 4: // refresh the ad
            {
                if ( ad != null )
                {
                    AdRequest.Builder request = new AdRequest.Builder();
                    ad.loadAd(request.build());
                }
                break;
            }

            case 5: // hide ad
            {
                if ( ad != null ) ad.setVisibility(View.GONE);
                break;
            }

            case 6: // show ad
            {
                if ( ad != null ) ad.setVisibility(View.VISIBLE);
                break;
            }

            case 7: // pause ad
            {
                if ( ad != null ) ad.pause();
                break;
            }

            case 8: // resume ad
            {
                if ( ad != null ) ad.resume();
                break;
            }

            case 9: // fullscreen ad
            {
                Log.i("AdMob", "Show Interstitial");

                CheckInterstitialAdInit();

                if ( interstitialAd == null ) LoadInterstitialAd();
                else
                {
                    interstitialAd.setFullScreenContentCallback( callbackShowI );
                    interstitialAd.show( act );
                }

                break;
            }

            case 10: // cache fullscreen ad
            {
                Log.i("AdMob", "Cache Interstitial");
                CheckInterstitialAdInit();

                if ( interstitialAd == null ) LoadInterstitialAd();

                break;
            }

            case 11: // show reward ad
            {
                Log.i("AdMob", "Show reward ad");
                CheckRewardAdInit();

                if ( rewardAd == null ) LoadRewardAd();
                else
                {
                    rewardAd.setFullScreenContentCallback( callbackShow );
                    rewardAd.show( act, callbackReward );
                }

                break;
            }

            case 12: // cache reward ad
            {
                Log.i("AdMob", "Cache reward ad");
                CheckRewardAdInit();

                if ( rewardAd == null ) LoadRewardAd();

                break;
            }
        }
    }
}

public class AdMobSDK
{
    static int m_iRewardAdValue = 0;
    static int m_iRewardAdRewarded = 0;
    static int m_iAdMobInitialized = 0;

    public static void onStart( Activity act )
    {
        RunnableAd run = new RunnableAd();
        run.action = 8;
        run.act = act;
        act.runOnUiThread(run);
    }

    public static void onStop( Activity act )
    {
        RunnableAd run = new RunnableAd();
        run.action = 7;
        run.act = act;
        act.runOnUiThread(run);
    }

    public static void AdMobCheckInitialised( Activity act )
    {
        if ( m_iAdMobInitialized == 1 ) return;

        m_iAdMobInitialized = 1;
        if ( AdConsentSDK.m_iChildRating == 1 )
        {
            RequestConfiguration conf = MobileAds.getRequestConfiguration().toBuilder()
                    .setTagForChildDirectedTreatment( TAG_FOR_CHILD_DIRECTED_TREATMENT_TRUE )
                    .setTagForUnderAgeOfConsent( TAG_FOR_UNDER_AGE_OF_CONSENT_TRUE )
                    .setMaxAdContentRating( MAX_AD_CONTENT_RATING_G )
                    .build();
            MobileAds.setRequestConfiguration(conf);
        }
        MobileAds.initialize( act );
    }

    public static void SetAdMobTestMode( int mode )
    {
        RunnableAd.testMode = mode;
    }

    public static void CreateAd(Activity act, String publisherID, int horz, int vert, int offsetX, int offsetY, int type)
    {
        AdMobCheckInitialised( act );

        RunnableAd run = new RunnableAd();
        run.pubID = publisherID;
        run.horz = horz;
        run.vert = vert;
        run.offsetX = offsetX;
        run.offsetY = offsetY;
        run.action = 1;
        run.act = act;
        run.adType = type;
        act.runOnUiThread(run);
    }

    public static void CacheFullscreenAd(Activity act, String publisherID)
    {
        AdMobCheckInitialised( act );

        RunnableAd run = new RunnableAd();
        run.pubID = publisherID;
        run.action = 10;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static void CreateFullscreenAd(Activity act, String publisherID)
    {
        AdMobCheckInitialised( act );

        RunnableAd run = new RunnableAd();
        run.pubID = publisherID;
        run.action = 9;
        run.act = act;
        act.runOnUiThread(run);
    }

    public static void CacheRewardAd(Activity act, String publisherID)
    {
        AdMobCheckInitialised( act );

        RunnableAd run = new RunnableAd();
        run.rewardpubID = publisherID;
        run.action = 12;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static void ShowRewardAd(Activity act, String publisherID)
    {
        AdMobCheckInitialised( act );

        m_iRewardAdRewarded = 0;

        RunnableAd run = new RunnableAd();
        run.rewardpubID = publisherID;
        run.action = 11;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static int GetRewardAdRewarded()
    {
        return m_iRewardAdRewarded;
    }

    public static int GetRewardAdValue()
    {
        return m_iRewardAdValue;
    }

    public static void ResetRewardAd()
    {
        m_iRewardAdRewarded = 0;
    }

    public static int GetRewardAdLoadedAdMob()
    {
        return (RunnableAd.rewardAd != null) ? 1 : 0;
    }

    public static void PositionAd(Activity act, int horz, int vert, int offsetX, int offsetY)
    {
        RunnableAd run = new RunnableAd();
        run.horz = horz;
        run.vert = vert;
        run.offsetX = offsetX;
        run.offsetY = offsetY;
        run.action = 2;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static void DeleteAd(Activity act)
    {
        RunnableAd run = new RunnableAd();
        run.action = 3;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static void RefreshAd(Activity act)
    {
        RunnableAd run = new RunnableAd();
        run.action = 4;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static void SetAdVisible(Activity act, int visible)
    {
        RunnableAd run = new RunnableAd();
        run.action = visible>0 ? 6 : 5;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static int GetFullscreenLoadedAdMob()
    {
        return (RunnableAd.interstitialAd != null) ? 1 : 0;
    }

    public static void ClearCachedAds()
    {
        RunnableAd.interstitialAd = null;
        RunnableAd.rewardAd = null;
    }
}