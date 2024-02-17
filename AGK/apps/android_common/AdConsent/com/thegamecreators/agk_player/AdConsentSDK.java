package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.Log;

//import androidx.annotation.Nullable;

import com.google.android.ump.ConsentForm;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.FormError;
import com.google.android.ump.UserMessagingPlatform;

import java.lang.reflect.Method;

public class AdConsentSDK
{
    static int m_iChildRating = 0;
    static int m_iAdMobConsentStatus = -2; // -2=startup value triggers consent load, -1=loading, 0=unknown, 1=non-personalised, 2=personalised
    static ConsentInformation consentInformation;

    public static void SetAdMobChildRating( int rating )
    {
        m_iChildRating = rating;
    }

    public static void AdMobUpdateConsentStatus()
    {
        if ( consentInformation.getConsentStatus() == ConsentInformation.ConsentStatus.REQUIRED )
        {
            Log.i( "AdMob Consent", "Consent required" );
            m_iAdMobConsentStatus = 0;
            return;
        }

        if ( consentInformation.getConsentStatus() == ConsentInformation.ConsentStatus.NOT_REQUIRED )
        {
            Log.i( "AdMob Consent", "Consent not required" );
            m_iAdMobConsentStatus = 2;
            return;
        }

        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(g_pAct);
        String consentString = sharedPrefs.getString("IABTCF_PurposeConsents", "");
        int gdprApplies = sharedPrefs.getInt("IABTCF_gdprApplies", -1);

        Log.i( "AdMob Consent", "GDPR Applies: " + gdprApplies );
        Log.i( "AdMob Consent", "ConsentString: " + consentString );

        if ( gdprApplies == 0 )
        {
            m_iAdMobConsentStatus = 2;
        }
        else
        {
            if ( consentString.length() == 0 )
            {
                m_iAdMobConsentStatus = 0;
            }
            else if ( consentString.length() < 4 || consentString.charAt(0) != '1' || consentString.charAt(1) != '1' || consentString.charAt(2) != '1' || consentString.charAt(3) != '1' )
            {
                m_iAdMobConsentStatus = 1;
            }
            else
            {
                m_iAdMobConsentStatus = 2;
            }
        }

        // check for chartboost
        try {
            Class externalClass = Class.forName( "com.thegamecreators.agk_player.ChartboostSDK" );
            Method externalMethod = externalClass.getMethod( "OverrideChartboostConsent", Activity.class, Integer.TYPE );
            externalMethod.invoke( null, g_pAct, (m_iAdMobConsentStatus == 2) ? 1 : 0 );
        }
        catch (Exception e) {}

        // check for admob
        try {
            Class externalClass = Class.forName( "com.thegamecreators.agk_player.AdMobSDK" );
            Method externalMethod = externalClass.getMethod( "ClearCachedAds" );
            externalMethod.invoke( null );
        }
        catch (Exception e) {}
    }

    public static void LoadAdMobConsentStatus(Activity act, String publisherID, String privacyPolicy )
    {
        if ( m_iAdMobConsentStatus != -1 )
        {
            m_iAdMobConsentStatus = -1;

            ConsentRequestParameters consentParams = new ConsentRequestParameters
                    .Builder()
                    .setTagForUnderAgeOfConsent(m_iChildRating != 0)
                    .build();

            consentInformation = UserMessagingPlatform.getConsentInformation(act);
            consentInformation.requestConsentInfoUpdate( act, consentParams,
                    new ConsentInformation.OnConsentInfoUpdateSuccessListener() {
                        @Override
                        public void onConsentInfoUpdateSuccess() {
                            AdMobUpdateConsentStatus();
                        }
                    },
                    new ConsentInformation.OnConsentInfoUpdateFailureListener() {
                        @Override
                        public void onConsentInfoUpdateFailure(FormError formError) {
                            Log.w( "AdMob Consent", "Failed to request consent status: " + formError.getMessage());
                            m_iAdMobConsentStatus = 0;
                        }
                    }
            );

        }
    }

    public static int GetAdMobConsentStatus( Activity act )
    {
        return m_iAdMobConsentStatus;
    }

    public static void RequestAdMobConsent( Activity act )
    {
        if ( m_iAdMobConsentStatus == -1 ) return;
        m_iAdMobConsentStatus = -1;

        final Activity pAct = act;
        act.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                UserMessagingPlatform.loadConsentForm(act,
                        new UserMessagingPlatform.OnConsentFormLoadSuccessListener() {
                            @Override
                            public void onConsentFormLoadSuccess(ConsentForm consentForm) {
                                Log.i( "AdMob Consent", "Consent form loaded" );
                                consentForm.show(
                                        pAct,
                                        new ConsentForm.OnConsentFormDismissedListener() {
                                            @Override
                                            //public void onConsentFormDismissed(@Nullable FormError formError) {
                                            public void onConsentFormDismissed(FormError formError) {
                                                AdMobUpdateConsentStatus();
                                            }
                                        }
                                );
                            }
                        },
                        new UserMessagingPlatform.OnConsentFormLoadFailureListener() {
                            @Override
                            public void onConsentFormLoadFailure(FormError formError) {
                                Log.w( "AdMob Consent", "Failed to load consent form: " + formError.getMessage());
                                m_iAdMobConsentStatus = 0;
                            }
                        }
                );
            }
        });
    }

    public static void OverrideAdMobConsent( Activity act, int mode )
    {
        //m_iAdMobConsentStatus = 1;
        //if ( mode == 2 ) m_iAdMobConsentStatus = 2;
    }
}