package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import com.chartboost.sdk.CBLocation;
import com.chartboost.sdk.Chartboost;
import com.chartboost.sdk.ChartboostDelegate;
import com.chartboost.sdk.Model.CBError;
import com.chartboost.sdk.Privacy.model.CCPA;
import com.chartboost.sdk.Privacy.model.COPPA;
import com.chartboost.sdk.Privacy.model.DataUseConsent;
import com.chartboost.sdk.Privacy.model.GDPR;

class RunnableChartboost implements Runnable
{
    public Activity act;
    public int action = 0;
    public static int caching = 0;
    public static int cached = 0;
    public static int rewardCached = 0;
    public static int rewardCaching = 0;
    public static String AppID;
    public static String AppSig;

    private ChartboostDelegate chartBoostDelegate = new ChartboostDelegate() {
        @Override
        public boolean shouldDisplayInterstitial(String location) {
            return true;
        }

        @Override
        public boolean shouldRequestInterstitial(String location) {
            return true;
        }

        @Override
        public void didCacheInterstitial(String location) {
            Log.i("Chartboost", "Interstitial cached");
            caching = 0;
            cached = 1;
        }

        @Override
        public void didFailToLoadInterstitial(String location, CBError.CBImpressionError error) {
            caching = 0;
            cached = 0;
            Log.e("Chartboost", "Interstitial failed to load: " + error.toString());
        }

        @Override
        public void didDismissInterstitial(String location) {
            Log.i("Chartboost", "Interstitial dismissed");
            if ( cached == 0 && caching == 0 )
            {
                Chartboost.cacheInterstitial(CBLocation.LOCATION_DEFAULT);
                caching = 1;
            }
        }

        @Override
        public void didCloseInterstitial(String location) {
            Log.i("Chartboost", "Interstitial closed");
            if ( cached == 0 && caching == 0 )
            {
                Chartboost.cacheInterstitial(CBLocation.LOCATION_DEFAULT);
                caching = 1;
            }
        }

        @Override
        public void didDisplayInterstitial(String location) {
            Log.i("Chartboost", "Interstitial displayed");
        }

        @Override
        public boolean shouldRequestMoreApps(String location) {
            return true;
        }

        @Override
        public boolean shouldDisplayMoreApps(String location) {
            return true;
        }

        // reward videos

        @Override
        public void didCacheRewardedVideo(String location)
        {
            Log.i("Chartboost", "Reward video cached");
            rewardCached = 1;
            rewardCaching = 0;
        }

        @Override
        public void didClickRewardedVideo(String location)
        {
            Log.i("Chartboost", "Reward video clicked");
        }

        @Override
        public void didCloseRewardedVideo(String location)
        {
            Log.i("Chartboost", "Reward video closed");
            if ( rewardCached == 0 && rewardCaching == 0 )
            {
                Chartboost.cacheRewardedVideo(CBLocation.LOCATION_DEFAULT);
                rewardCaching = 1;
            }
        }

        @Override
        public void didCompleteRewardedVideo(String location, int reward)
        {
            Log.i("Chartboost", "Reward video rewarded");
            ChartboostSDK.m_iRewardAdRewardedChartboost = 1;
        }

        @Override
        public void didDismissRewardedVideo(String location)
        {
            Log.i("Chartboost", "Reward video dismissed");
            if ( rewardCached == 0 && rewardCaching == 0 )
            {
                Chartboost.cacheRewardedVideo(CBLocation.LOCATION_DEFAULT);
                rewardCaching = 1;
            }
        }

        @Override
        public void didDisplayRewardedVideo(String location)
        {
            Log.i("Chartboost", "Reward video displayed");
        }

        @Override
        public void didFailToLoadRewardedVideo(String location, CBError.CBImpressionError error)
        {
            Log.i("Chartboost", "Reward video failed to load: " + error.toString());
            rewardCached = 0;
            rewardCaching = 0;
        }

        @Override
        public boolean shouldDisplayRewardedVideo(String location) { return true; }
    };

    public static void UpdateConsent( Activity act )
    {
        Chartboost.clearDataUseConsent( act, GDPR.GDPR_STANDARD );
        Chartboost.clearDataUseConsent( act, CCPA.CCPA_STANDARD );
        DataUseConsent dataUseConsent = new GDPR( (ChartboostSDK.m_iConsentStatus == 1) ? GDPR.GDPR_CONSENT.BEHAVIORAL : GDPR.GDPR_CONSENT.NON_BEHAVIORAL );
        Chartboost.addDataUseConsent(act, dataUseConsent);
        dataUseConsent = new CCPA( (ChartboostSDK.m_iConsentStatus == 1) ? CCPA.CCPA_CONSENT.OPT_IN_SALE : CCPA.CCPA_CONSENT.OPT_OUT_SALE );
        Chartboost.addDataUseConsent(act, dataUseConsent);

        if ( AdConsentSDK.m_iChildRating == 1 )
        {
            dataUseConsent = new COPPA( true );
            Chartboost.addDataUseConsent( act, dataUseConsent );
        }

        cached = 0;
        rewardCached = 0;
    }

    @Override
    public void run() {
        switch ( action )
        {
            case 1: // initialize
            {
                Log.i("Chartboost", "Initialize Chartboost SDK");

                UpdateConsent( act );

                cached = 0;
                caching = 1;
                Chartboost.startWithAppId((Context)this.act, AppID, AppSig);
                Chartboost.setShouldRequestInterstitialsInFirstSession(true);
                Chartboost.setDelegate(this.chartBoostDelegate);
                Chartboost.cacheInterstitial(CBLocation.LOCATION_DEFAULT);
                break;
            }
            case 3: // show ad
            {
                Log.i("Chartboost", "Display Chartboost Ad");

                if ( cached == 0 )
                {
                    if (caching == 0)
                    {
                        caching = 1;
                        Chartboost.cacheInterstitial(CBLocation.LOCATION_DEFAULT);
                    }
                }
                else
                {
                    Log.i("Chartboost", "Showing Chartboost Ad");
                    cached = 0;
                    Chartboost.showInterstitial( CBLocation.LOCATION_DEFAULT );
                }

                break;
            }
            case 4: // cache More Apps
            {
                //Chartboost.sharedChartboost().cacheMoreApps();
                break;
            }
            case 5: // show More Apps
            {
                //Chartboost.sharedChartboost().showMoreApps();
                break;
            }
            case 6: // cache reward ad
            {
                Log.i("Chartboost", "Cache Chartboost Reward Ad");

                if ( rewardCached == 0 )
                {
                    if (rewardCaching == 0)
                    {
                        rewardCaching = 1;
                        Chartboost.cacheRewardedVideo(CBLocation.LOCATION_DEFAULT);
                    }
                }
                break;
            }
            case 7: // show reward ad
            {
                Log.i("Chartboost", "Display Chartboost Reward Ad");

                if ( rewardCached == 0 )
                {
                    if (rewardCaching == 0)
                    {
                        rewardCaching = 1;
                        Chartboost.cacheRewardedVideo(CBLocation.LOCATION_DEFAULT);
                    }
                }
                else
                {
                    Log.i("Chartboost", "Showing Chartboost Reward Ad");
                    rewardCached = 0;
                    Chartboost.showRewardedVideo(CBLocation.LOCATION_DEFAULT);
                }
                break;
            }
            default:
            {
                Log.i("CBTEST", "undefinedChartboostAction");
                break;
            }
        }
    }
}

public class ChartboostSDK
{
    static int m_iRewardAdRewardedChartboost = 0;
    static int m_iConsentStatus = 0;

    public static void OverrideChartboostConsent( Activity act, int mode )
    {
        m_iConsentStatus = 0;
        if ( mode == 2 ) m_iConsentStatus = 1;

        RunnableChartboost.UpdateConsent( g_pAct );
    }

    public static void SetChartboostDetails( Activity act, String publisherID, String publisherID2 )
    {
        RunnableChartboost.AppID = publisherID;
        RunnableChartboost.AppSig = publisherID2;

        RunnableChartboost run = new RunnableChartboost();
        run.action = 1;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static void CreateFullscreenAdChartboost(Activity act, int type)
    {
        RunnableChartboost run = new RunnableChartboost();
        run.action = 3;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static void CacheRewardAdChartboost(Activity act)
    {
        RunnableChartboost run = new RunnableChartboost();
        run.action = 6;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static void ShowRewardAdChartboost(Activity act)
    {
        m_iRewardAdRewardedChartboost = 0;

        RunnableChartboost run = new RunnableChartboost();
        run.action = 7;
        run.act = act;
        act.runOnUiThread( run );
    }

    public static int GetRewardAdRewardedChartboost()
    {
        return m_iRewardAdRewardedChartboost;
    }

    public static void ResetRewardAdChartboost()
    {
        m_iRewardAdRewardedChartboost = 0;
    }

    public static int GetRewardAdLoadedChartboost()
    {
        return RunnableChartboost.rewardCached;
    }

    public static int GetFullscreenLoadedChartboost()
    {
        return RunnableChartboost.cached;
    }
}