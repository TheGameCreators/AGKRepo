package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.content.ActivityNotFoundException;
import android.util.Log;
import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;

public class RequestReviewSDK
{
    static ReviewManager reviewManager = null;
    public static void RequestReview()
    {
        if ( reviewManager == null ) reviewManager = ReviewManagerFactory.create( g_pAct );

        com.google.android.gms.tasks.Task<ReviewInfo> request = reviewManager.requestReviewFlow();
        request.addOnCompleteListener(task -> {
            if (task.isSuccessful()) {
                Log.i("RateApp", "Starting review process");
                ReviewInfo reviewInfo = task.getResult();
                try
                {
                    reviewManager.launchReviewFlow( g_pAct, reviewInfo );
                }
                catch( ActivityNotFoundException e )
                {
                    Log.w("RateApp", "Failed to start review process due to activity not found");
                }
            }
            else
            {
                Log.w("RateApp", "Unable to start review process");
            }
        });
    }
}