package com.thegamecreators.agk_player;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

public class NativeAppsSDK
{
    public static void ExternalCommand(Activity act, String cmd, String str1, String str2 )
    {
        if( cmd.equals("switchapp") )
        {
            Intent intent = new Intent( act, TestActivity.class );
            act.startActivity( intent );
        }
        else if( cmd.equals("returntoprev") )
        {

        }
    }
}