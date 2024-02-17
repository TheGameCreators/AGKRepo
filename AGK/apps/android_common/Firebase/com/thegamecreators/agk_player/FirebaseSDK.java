package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.os.Bundle;

import com.google.firebase.FirebaseApp;
import com.google.firebase.analytics.FirebaseAnalytics;

public class FirebaseSDK
{
    static FirebaseAnalytics mFirebaseAnalytics = null;
    public static void FirebaseInit( Activity act )
    {
        FirebaseApp.initializeApp(act);
        mFirebaseAnalytics = FirebaseAnalytics.getInstance(act);
    }

    public static void FirebaseLogEvent( String event_name )
    {
        if ( mFirebaseAnalytics == null ) return;
        mFirebaseAnalytics.logEvent( event_name, null );
    }

    public static void FirebaseLogEventInt( String event_name, String param_name, int paramValue )
    {
        if ( mFirebaseAnalytics == null ) return;
        Bundle bundle = new Bundle();
        bundle.putInt( param_name, paramValue );
        mFirebaseAnalytics.logEvent( event_name, bundle );
    }
}