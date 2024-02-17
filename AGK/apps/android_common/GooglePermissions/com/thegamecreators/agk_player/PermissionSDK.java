package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;

import androidx.core.content.ContextCompat;

public class PermissionSDK
{
    enum PermissionAsk
    {
        NOT_ASKED,
        ASKING,
        ASKED
    }

    static class AGKPermission
    {
        String nameAGK;
        String nameAndroid;
        PermissionAsk asked;

        AGKPermission( String agk, String android )
        {
            nameAGK = agk;
            nameAndroid = android;
            asked = PermissionAsk.NOT_ASKED;
        }
    }

    static AGKPermission[] g_Permissions = {
        new AGKPermission( "WriteExternal", Manifest.permission.WRITE_EXTERNAL_STORAGE ),
            new AGKPermission( "ReadExternal", Manifest.permission.READ_EXTERNAL_STORAGE ),
            new AGKPermission( "Location", Manifest.permission.ACCESS_FINE_LOCATION ),
            new AGKPermission( "Camera", Manifest.permission.CAMERA ),
            new AGKPermission( "RecordAudio", Manifest.permission.RECORD_AUDIO ),
    };

    public static void activityCallback( Activity act, int requestCode, int resultCode, Intent data )
    {
        int index = requestCode-5;
        if ( index < 0 ) return;
        if ( index >= g_Permissions.length ) return;

        g_Permissions[index].asked = PermissionAsk.ASKED;
    }

    public static int GetRealPermission( String permission )
    {
        for( int i = 0; i < g_Permissions.length; i++ )
        {
            if ( g_Permissions[i].nameAGK.equals(permission) ) return i;
        }

        return -1;
    }

    // -1=rejected, 0=not asked, 1=in progress, 2=granted
    public static int CheckPermission( Activity act, String permission )
    {
        int index = GetRealPermission(permission);
        if ( index < 0 )
        {
            AGKHelper.ShowMessage( act, "Could not find permission" );
            return 0;
        }

        if ( Build.VERSION.SDK_INT < 23 )
        {
            if ( ContextCompat.checkSelfPermission(act, g_Permissions[index].nameAndroid) == PackageManager.PERMISSION_GRANTED) return 2;
            else return -1;
        }
        else
        {
            if ( g_Permissions[index].asked == PermissionAsk.ASKING ) return 1;
            if (act.checkSelfPermission(g_Permissions[index].nameAndroid) == PackageManager.PERMISSION_GRANTED) return 2;
            else
            {
                if ( g_Permissions[index].asked == PermissionAsk.ASKED ) return -1;
                else return 0;
            }
        }
    }

    // 0=not granted, 1=granted
    public static int CheckRawPermission( Activity act, String permission )
    {
        if ( Build.VERSION.SDK_INT < 23 )
        {
            if ( ContextCompat.checkSelfPermission(act, permission) == PackageManager.PERMISSION_GRANTED) return 1;
            else return 0;
        }
        else
        {
            if (act.checkSelfPermission(permission) == PackageManager.PERMISSION_GRANTED) return 1;
            else return 0;
        }
    }

    public static void RequestPermission( Activity act, String permission )
    {
        if ( Build.VERSION.SDK_INT >= 23 )
        {
            int index = GetRealPermission(permission);
            if ( index < 0 )
            {
                AGKHelper.ShowMessage( act, "Could not find permission" );
                return;
            }

            // allow multiple calls just in case one gets stuck
            //if ( g_Permissions[index].asked == PermissionAsk.ASKING ) return;

            g_Permissions[index].asked = PermissionAsk.ASKING;
            act.requestPermissions( new String[]{g_Permissions[index].nameAndroid}, 5+index );
        }
    }
}