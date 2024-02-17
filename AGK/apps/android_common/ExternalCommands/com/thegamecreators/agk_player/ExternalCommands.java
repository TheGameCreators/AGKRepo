package com.thegamecreators.agk_player;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import java.lang.reflect.Method;

public class ExternalCommands
{
    static String[][] externalSDKs =
    {
        { "appsflyer", "com.thegamecreators.agk_player.AppsFlyerSDK" },
        { "inapppurchase", "com.thegamecreators.agk_player.InAppPurchase" },
        { "snapchat", "com.thegamecreators.agk_player.SnapChatSDK" },
        { "admob", "com.thegamecreators.agk_player.AdMobSDK" },
        { "chartboost", "com.thegamecreators.agk_player.ChartboostSDK" },
        { "adconsent", "com.thegamecreators.agk_player.AdConsentSDK" },
        { "firebase", "com.thegamecreators.agk_player.FirebaseSDK" },
        { "playvideo", "com.thegamecreators.agk_player.PlayVideoSDK" },
        { "location", "com.thegamecreators.agk_player.LocationSDK" },
        { "pushnotify", "com.thegamecreators.agk_player.PushNotificationsSDK" },
        { "installreferrer", "com.thegamecreators.agk_player.InstallReferrerSDK" },
        { "camera", "com.thegamecreators.agk_player.CameraSDK" },
        { "texttospeech", "com.thegamecreators.agk_player.TextToSpeechSDK" },
        { "recordscreen", "com.thegamecreators.agk_player.RecordScreenSDK" },
        { "textentry", "com.thegamecreators.agk_player.TextEntrySDK" },
        { "gamecenter", "com.thegamecreators.agk_player.GameCenterSDK" },
        { "clouddata", "com.thegamecreators.agk_player.CloudDataSDK" },
        { "youtube", "com.thegamecreators.agk_player.YoutubeSDK" },
        { "permissions", "com.thegamecreators.agk_player.PermissionSDK" },
        { "immersive", "com.thegamecreators.agk_player.ImmersiveModeSDK" },
        { "requestreview", "com.thegamecreators.agk_player.RequestReviewSDK" },
        { "idfatracking", "com.thegamecreators.agk_player.TrackingConsentSDK" }, // only used on iOS, but defined here to prevent warning
        { "nativeapp", "com.thegamecreators.agk_player.NativeAppsSDK" },
        { "custom", "com.thegamecreators.agk_player.CustomSDK" },
    };

    // ************************************
    // Do not modify anything below here
    // ************************************

    private static String GetClassName( String sdk )
    {
        for( String[] externalSDK : externalSDKs )
        {
            if ( sdk.equals( externalSDK[0] ) ) return externalSDK[ 1 ];
        }

        Log.e( "AGK", "Class not found for SDK: " + sdk );
        AGKHelper.ShowMessage( AGKHelper.g_pAct, "Class not found for SDK: " + sdk );
        return null;
    }

    public static void onStop( Activity act )
    {
        for( String[] externalSDK : externalSDKs )
        {
            Class externalClass;
            try {
                externalClass = Class.forName( externalSDK[1] );
            }
            catch (ClassNotFoundException e)
            {
                continue;
            }

            Method externalMethod;
            try {
                externalMethod = externalClass.getMethod( "onStop", Activity.class );
                externalMethod.invoke( null, act );
            }
            catch (Exception e)
            {
                continue;
            }
        }
    }

    public static void onStart( Activity act )
    {
        for( String[] externalSDK : externalSDKs )
        {
            Class externalClass;
            try {
                externalClass = Class.forName( externalSDK[1] );
            }
            catch (ClassNotFoundException e)
            {
                continue;
            }

            Method externalMethod;
            try {
                externalMethod = externalClass.getMethod( "onStart", Activity.class );
                externalMethod.invoke( null, act );
            }
            catch (Exception e)
            {
                continue;
            }
        }
    }

    public static void activityCallback( String sdk, Activity act, int requestCode, int resultCode, Intent data )
    {
        String className = GetClassName( sdk );
        if ( className == null ) return;

        Class externalClass;
        try {
            externalClass = Class.forName( className );
        }
        catch (ClassNotFoundException e)
        {
            return;
        }

        Method externalMethod;
        try {
            externalMethod = externalClass.getMethod( "activityCallback", Activity.class, Integer.TYPE, Integer.TYPE, Intent.class );
            externalMethod.invoke( null, act, requestCode, resultCode, data );
        }
        catch (Exception e)
        {
            return;
        }
    }

    public static int IsExternalSDKSupported( String sdk )
    {
        String className = GetClassName( sdk );
        if ( className == null ) return 0;

        Class externalClass;
        try {
            externalClass = Class.forName( className );
        }
        catch (ClassNotFoundException e)
        {
            return 0;
        }

        return 1;
    }

    public static void ExternalCommand(Activity act, String sdk, String cmd, String str1, String str2 )
    {
        String className = GetClassName( sdk );
        if ( className == null ) return;

        //Log.i( "AGK", "Running external command " + cmd + " for: " + sdk );

        Class externalClass;
        try {
            externalClass = Class.forName( className );
        }
        catch (ClassNotFoundException e)
        {
            Log.d( "External Command", "SDK: " + sdk + " is not implemented" );
            return;
        }

        Method externalMethod;
        try {
            externalMethod = externalClass.getMethod( "ExternalCommand", Activity.class, String.class, String.class, String.class );
            externalMethod.invoke( null, act, cmd, str1, str2 );
        }
        catch (NoSuchMethodException e)
        {
            Log.w( "External Command", "ExternalCommand method is not implemented in SDK: " + sdk );
            return;
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return;
        }
    }

    public static int ExternalCommandInt(Activity act, String sdk, String cmd, String str1, String str2 )
    {
        String className = GetClassName( sdk );
        if ( className == null ) return 0;

        Class externalClass;
        try {
            externalClass = Class.forName( className );
        }
        catch (ClassNotFoundException e)
        {
            Log.d( "External Command", "SDK: " + sdk + " is not implemented" );
            return 0;
        }

        Method externalMethod;
        try {
            externalMethod = externalClass.getMethod( "ExternalCommandInt", Activity.class, String.class, String.class, String.class );
            Integer result = (Integer) externalMethod.invoke( null, act, cmd, str1, str2 );
            return result.intValue();
        }
        catch (NoSuchMethodException e)
        {
            Log.w( "External Command", "ExternalCommandInt method is not implemented in SDK: " + sdk );
            return 0;
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return 0;
        }
    }

    public static float ExternalCommandFloat(Activity act, String sdk, String cmd, String str1, String str2 )
    {
        String className = GetClassName( sdk );
        if ( className == null ) return 0;

        Class externalClass;
        try {
            externalClass = Class.forName( className );
        }
        catch (ClassNotFoundException e)
        {
            Log.d( "External Command", "SDK: " + sdk + " is not implemented" );
            return 0;
        }

        Method externalMethod;
        try {
            externalMethod = externalClass.getMethod( "ExternalCommandFloat", Activity.class, String.class, String.class, String.class );
            Float result = (Float) externalMethod.invoke( null, act, cmd, str1, str2 );
            return result.floatValue();
        }
        catch (NoSuchMethodException e)
        {
            Log.w( "External Command", "ExternalCommandFloat method is not implemented in SDK: " + sdk );
            return 0;
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return 0;
        }
    }

    public static String ExternalCommandString(Activity act, String sdk, String cmd, String str1, String str2 )
    {
        String className = GetClassName( sdk );
        if ( className == null ) return "";

        Class externalClass;
        try {
            externalClass = Class.forName( className );
        }
        catch (ClassNotFoundException e)
        {
            Log.d( "External Command", "SDK: " + sdk + " is not implemented" );
            return "";
        }

        Method externalMethod;
        try {
            externalMethod = externalClass.getMethod( "ExternalCommandString", Activity.class, String.class, String.class, String.class );
            String result = (String) externalMethod.invoke( null, act, cmd, str1, str2 );
            return result;
        }
        catch (NoSuchMethodException e)
        {
            Log.w( "External Command", "ExternalCommandString method is not implemented in SDK: " + sdk );
            return "";
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return "";
        }
    }
}