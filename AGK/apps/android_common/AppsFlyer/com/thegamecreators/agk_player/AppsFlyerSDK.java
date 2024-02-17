package com.thegamecreators.agk_player;

import android.app.Activity;
import android.util.Log;

import com.appsflyer.AppsFlyerLib;

import java.util.HashMap;
import java.util.Map;

public class AppsFlyerSDK
{
    public static void ExternalCommand(Activity act, String cmd, String str1, String str2 )
    {
        if( cmd.equals("init") )
        {
            //Log.i( "AGK", "AppsFlyer Init " + str1 );
            String devKey = str1;
            AppsFlyerLib.getInstance().init( devKey, null, act );
        }
        else if( cmd.equals("start") )
        {
            //Log.i( "AGK", "AppsFlyer Start " );
            AppsFlyerLib.getInstance().start( act );
        }
        else if ( cmd.equals("setdebug") )
        {
            boolean enabled = false;
            if ( str1.equals("true") ) enabled = true;
            //Log.i( "AGK", "AppsFlyer SetDebug " + Boolean.toString(enabled) );
            AppsFlyerLib.getInstance().setDebugLog( enabled );
        }
        else if ( cmd.equals("logevent") )
        {
            //Log.i( "AGK", "AppsFlyer LogEvent " + str1 + "" );
            String eventName = str1;
            Map<String,Object> values = new HashMap<String, Object>();
            String[] pairs = str2.split( ":" );
            if ( pairs.length < 1 ) return;
            for( String item : pairs )
            {
                String[] value = item.split( "=", 2 );
                if( value.length < 2 ) continue;
                values.put( value[0], value[1] );
            }
            AppsFlyerLib.getInstance().logEvent( act, str1, values );
        }
    }
}