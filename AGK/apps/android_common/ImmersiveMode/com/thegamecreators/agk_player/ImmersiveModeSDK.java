package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.os.Build;
import android.view.View;

public class ImmersiveModeSDK
{
    static int immersiveMode = 0; // 0 = show nav bar, 1 = hide nav bar
    static int g_iImmersiveListenerSet = 0;

    public static void onStart( Activity act )
    {
        if ( immersiveMode > 0 )
        {
            SetImmersiveMode(act, immersiveMode);
        }
    }

    public static void SetImmersiveMode( Activity act, int mode )
    {
        immersiveMode = mode;

        if (Build.VERSION.SDK_INT < 19) return;

        Runnable NavBar = new Runnable() {
            @Override
            public void run()
            {
                if (Build.VERSION.SDK_INT < 19) return;

                if ( g_iImmersiveListenerSet == 0 )
                {
                    g_iImmersiveListenerSet = 1;
                    g_pAct.getWindow().getDecorView().setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
                        @Override
                        public void onSystemUiVisibilityChange( int visibility )
                        {
                            if ( g_pAct == null ) return;
                            android.content.res.Configuration config = g_pAct.getResources().getConfiguration();
                            g_pAct.onConfigurationChanged(config);
                        }
                    });
                }

                int options = 0;
                if ( immersiveMode == 1 ) options = View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;

                View decorView = g_pAct.getWindow().getDecorView();
                decorView.setSystemUiVisibility(options);
            }
        };

        act.runOnUiThread(NavBar);
    }

    public static void ExternalCommand(Activity act, String cmd, String str1, String str2 )
    {
        if ( cmd.equals("refresh") )
        {
            if ( immersiveMode > 0 ) SetImmersiveMode( act, immersiveMode );
        }
    }
}