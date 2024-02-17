package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;

public class PermissionSDK
{
    public static void activityCallback( Activity act, int requestCode, int resultCode, Intent data )
    {

    }

    // -1=rejected, 0=not asked, 1=in progress, 2=granted
    public static int CheckPermission(Activity act, String permission )
    {
        return 2;
    }

    public static int CheckRawPermission(Activity act, String permission )
    {
        return 1;
    }

    public static void RequestPermission( Activity act, String permission )
    {

    }
}