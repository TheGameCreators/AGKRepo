package com.thegamecreators.agk_player;

import static android.app.Activity.RESULT_OK;
import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ResolveInfo;
import android.widget.Toast;

import com.google.android.youtube.player.YouTubeInitializationResult;
import com.google.android.youtube.player.YouTubeStandalonePlayer;

import java.util.List;

public class YoutubeSDK
{
    public static void activityCallback( Activity act, int requestCode, int resultCode, Intent data )
    {
        if ( resultCode != RESULT_OK )
        {
            YouTubeInitializationResult errorReason = YouTubeStandalonePlayer.getReturnedInitializationResult(data);
            if (errorReason.isUserRecoverableError()) {
                errorReason.getErrorDialog(act, 0).show();
            } else {
                String errorMessage = errorReason.toString();
                Toast.makeText(act, errorMessage, Toast.LENGTH_LONG).show();
            }
        }
    }

    public static void PlayYoutubeVideo(Activity act, String key, String video, int time )
    {
        Intent intent = YouTubeStandalonePlayer.createVideoIntent(act, key, video, time, true, true);

        List<ResolveInfo> resolveInfo = act.getPackageManager().queryIntentActivities(intent, 0);
        if (resolveInfo != null && !resolveInfo.isEmpty())
        {
            act.startActivityForResult(intent, 9003);
        }
        else
        {
            // Could not resolve the intent - must need to install or update the YouTube API service.
            g_pAct.runOnUiThread( new Runnable() {
                @Override
                public void run() {
                    YouTubeInitializationResult.SERVICE_MISSING.getErrorDialog(act, 1).show();
                }
            });
        }
    }
}