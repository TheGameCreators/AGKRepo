package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.os.RemoteException;

import com.android.installreferrer.api.InstallReferrerClient;
import com.android.installreferrer.api.InstallReferrerStateListener;
import com.android.installreferrer.api.ReferrerDetails;

public class InstallReferrerSDK
{
    enum REFERRAL_STATE
    {
        INITIAL_STATE,
        IN_PROGRESS,
        FINISHED,
        FAILED,
    }

    private static Object lock = new Object();
    public static String g_sReferral = "";
    public static long g_sReferralInstallTime = 0;
    public static long g_sReferralLinkClickTime = 0;
    public static String g_sReferralVersion = "";
    public static REFERRAL_STATE g_iReferralState = REFERRAL_STATE.INITIAL_STATE;

    public static void StartGetReferrer( Activity act )
    {
        g_iReferralState = REFERRAL_STATE.IN_PROGRESS;

        InstallReferrerClient referrerClient;
        referrerClient = InstallReferrerClient.newBuilder(act.getApplicationContext()).build();

        referrerClient.startConnection(new InstallReferrerStateListener()
        {
            @Override
            public void onInstallReferrerSetupFinished(int responseCode)
            {
                switch (responseCode)
                {
                    case InstallReferrerClient.InstallReferrerResponse.OK:
                    {
                        // Connection established.
                        ReferrerDetails response = null;

                        try
                        {
                            response = referrerClient.getInstallReferrer();

                            synchronized (lock)
                            {
                                g_sReferral = response.getInstallReferrer();
                                g_sReferralInstallTime = response.getInstallBeginTimestampSeconds();
                                g_sReferralLinkClickTime = response.getReferrerClickTimestampSeconds();
                                g_sReferralVersion = response.getInstallVersion();
                                g_iReferralState = REFERRAL_STATE.FINISHED;
                            }

                            //long referrerClickTime = response.getReferrerClickTimestampSeconds();
                            //long appInstallTime = response.getInstallBeginTimestampSeconds();
                            //boolean instantExperienceLaunched = response.getGooglePlayInstantParam();
                        }
                        catch (RemoteException e)
                        {
                            e.printStackTrace();
                        }
                    }
                    break;

                    case InstallReferrerClient.InstallReferrerResponse.FEATURE_NOT_SUPPORTED:
                    {
                        // API not available on the current Play Store app.
                        g_iReferralState = REFERRAL_STATE.FAILED;
                    }
                    break;

                    case InstallReferrerClient.InstallReferrerResponse.SERVICE_UNAVAILABLE:
                    {
                        // Connection couldn't be established.
                        g_iReferralState = REFERRAL_STATE.FAILED;
                    }
                    break;
                }

                referrerClient.endConnection();
            }

            @Override
            public void onInstallReferrerServiceDisconnected()
            {
                // Try to restart the connection on the next request to
                // Google Play by calling the startConnection() method.
                g_iReferralState = REFERRAL_STATE.FAILED;
            }
        });
    }

    public static void ExternalCommand(Activity act, String cmd, String str1, String str2 )
    {
        if( cmd.equals("start") )
        {
            StartGetReferrer( act );
        }
    }

    public static int ExternalCommandInt(Activity act, String cmd, String str1, String str2 )
    {
        if( cmd.equals("status") )
        {
            switch( g_iReferralState )
            {
                case INITIAL_STATE: return 0;
                case IN_PROGRESS: return 1;
                case FINISHED: return 2;
                case FAILED: return -1;
            }

            return -1;
        }
        else if( cmd.equals("get") )
        {
            synchronized (lock)
            {
                switch( str1 )
                {
                    case "installtime": return (int) g_sReferralInstallTime;
                    case "linkclicktime": return (int) g_sReferralLinkClickTime;
                }
            }
        }

        return 0;
    }

    public static String ExternalCommandString(Activity act, String cmd, String str1, String str2 )
    {
        if( cmd.equals("get") )
        {
            synchronized (lock)
            {
                switch( str1 )
                {
                    case "version": return g_sReferralVersion;
                    default: return g_sReferral;
                }
            }
        }

        return "";
    }
}