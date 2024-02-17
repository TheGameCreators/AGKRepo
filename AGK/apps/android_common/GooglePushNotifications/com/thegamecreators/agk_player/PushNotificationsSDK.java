package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.util.Log;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.messaging.FirebaseMessaging;

public class PushNotificationsSDK
{
    public static String GCM_PNRegID = "";
    public static String FCM_Sender_ID = "";

    public static void setPushNotificationKeys( String key1, String key2 )
    {
        switch( key1 )
        {
            case "SenderID": FCM_Sender_ID = key2; break;
        }
    }

    public static int registerPushNotification( Activity nativeactivityptr )
    {
        FirebaseMessaging.getInstance().setAutoInitEnabled( true );

        FirebaseMessaging.getInstance().getToken()
                .addOnCompleteListener(new OnCompleteListener<String>() {
                    @Override
                    public void onComplete(Task<String> task) {
                        if (!task.isSuccessful()) {
                            Log.w("Firebase", "Fetching FCM registration token failed", task.getException());
                            GCM_PNRegID = "Error";
                            return;
                        }

                        // Get new FCM registration token
                        GCM_PNRegID = task.getResult();
                        Log.i( "Firebase", "PN Token: " + GCM_PNRegID );
                    }
                });

        return 1;
    }

    public static String getPNRegID()
    {
        if ( GCM_PNRegID == null ) return "Error";
        else return GCM_PNRegID;
    }
}