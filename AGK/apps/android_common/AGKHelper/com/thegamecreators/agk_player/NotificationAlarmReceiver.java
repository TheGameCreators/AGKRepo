package com.thegamecreators.agk_player;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import androidx.core.app.NotificationCompat;

import android.net.Uri;
import android.util.Log;

public class NotificationAlarmReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i("Notification Alarm","Received");
        if ( AGKHelper.isVisible == 1 ) return;

        String message = intent.getStringExtra("message");
        String title = intent.getStringExtra("title");
        int id = intent.getIntExtra("id", 0);
        String deeplink = intent.getStringExtra("deeplink");

        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

        Intent intent2 = new Intent(context, AGKActivity.class);
        if ( deeplink != null && !deeplink.equals("") )
        {
            intent2.setData( Uri.parse(deeplink) );
        }

        int flags = 0;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) flags |= PendingIntent.FLAG_IMMUTABLE;
        PendingIntent pIntent = PendingIntent.getActivity(context, 0, intent2, flags);

        Bitmap largeIcon = null;
        try
        {
            largeIcon = BitmapFactory.decodeResource(context.getResources(), LocalApp.GetAppMainIcon());
        }
        catch (Exception ex)
        {
            Log.i("AGK", "Failed to create large icon from app icon");
            ex.printStackTrace();
        }

        Notification notif = new NotificationCompat.Builder(context)
                .setContentTitle(title)
                .setContentText(message)
                .setSmallIcon( LocalApp.GetAppNotifyIcon() )
                .setContentIntent(pIntent)
                .setAutoCancel(true)
                .setLargeIcon(largeIcon)
                .setDefaults(Notification.DEFAULT_SOUND | Notification.DEFAULT_LIGHTS)
                .setChannelId("notify")
                .build();

        notificationManager.notify(id, notif);
    }
}