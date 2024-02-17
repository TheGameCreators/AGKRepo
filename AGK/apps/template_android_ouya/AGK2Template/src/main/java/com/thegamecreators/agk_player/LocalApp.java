package com.thegamecreators.agk_player;

import android.app.Activity;

import com.mycompany.mytemplate.R;

public class LocalApp
{
    public static String g_sTextToSpeechEngine = null;

    public static String GetAppName( Activity act )
    {
        return act.getString( R.string.app_name );
    }

    public static int GetAppNotifyIcon()
    {
        return R.drawable.icon;
    }

    public static int GetAppMainIcon()
    {
        return R.drawable.icon;
    }
}