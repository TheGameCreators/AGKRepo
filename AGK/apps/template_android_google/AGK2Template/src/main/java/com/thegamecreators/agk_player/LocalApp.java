package com.thegamecreators.agk_player;

import android.app.Activity;

import com.mycompany.mytemplate.R;

public class LocalApp
{
    public static String g_sTextToSpeechEngine = "com.google.android.tts";

    public static String GetAppName( Activity act )
    {
        return act.getString( R.string.app_name );
    }

    public static int GetAppNotifyIcon()
    {
        return R.drawable.icon_white;
    }

    public static int GetAppMainIcon()
    {
        return R.mipmap.ic_launcher;
    }
}