package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import androidx.annotation.NonNull;

import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.AnnotatedData;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.LeaderboardsClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;
import com.google.android.gms.games.achievement.Achievement;
import com.google.android.gms.games.achievement.AchievementBuffer;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;

import java.util.Iterator;

// NOTE: Google Games V2 will prompt the user to sign in at app startup 
// even if you never call any GameCenter commands
public class GameCenterSDK
{
    static String g_GamesPlayerID = "";
    static String g_GamesPlayerName = "";
    static int m_GameCenterLoggedIn = 0;
    static int m_GameCenterSetup = 0;
    static AchievementBuffer m_AllAchievements = null;
    static int m_GoogleGameServicesCheck = -1;

    public static void activityCallback( Activity act, int requestCode, int resultCode, Intent data )
    {

    }

    public static void onStart( Activity act )
    {
        GameCenterCheckLogin();
    }

    public static int GetGameCenterExists( Activity act )
    {
        if ( m_GoogleGameServicesCheck < 0 )
        {
            int resultCode = GooglePlayServicesUtil.isGooglePlayServicesAvailable(act);
            if ( ConnectionResult.SUCCESS == resultCode ) m_GoogleGameServicesCheck = 1;
            else m_GoogleGameServicesCheck = -1;
        }

        return m_GoogleGameServicesCheck;
    }

    public static void GameCenterSetup( Activity act )
    {
        m_GameCenterSetup = 1;
        PlayGamesSdk.initialize( act );
        GameCenterCheckLogin();
    }

    public static void GameCenterCheckLogin()
    {
        if ( m_GameCenterSetup != 1 ) return;

        Log.i( "GameCenter", "Checking login" );

        GamesSignInClient gamesSignInClient = PlayGames.getGamesSignInClient( g_pAct );
        gamesSignInClient.isAuthenticated().addOnCompleteListener( isAuthenticatedTask ->
        {
            if ( isAuthenticatedTask.isSuccessful() && isAuthenticatedTask.getResult().isAuthenticated() )
            {
                Log.i( "GameCenter", "Login: User is logged in" );

                PlayGames.getPlayersClient( g_pAct ).getCurrentPlayer().addOnCompleteListener( mTask ->
                {
                    Log.i( "GameCenter", "Getting player ID" );
                    g_GamesPlayerID = mTask.getResult().getPlayerId();
                    g_GamesPlayerName = mTask.getResult().getDisplayName();
                    m_GameCenterLoggedIn = 1;
                });

                AchievementsClient achievementsClient = PlayGames.getAchievementsClient( g_pAct );
                achievementsClient.load(false).addOnCompleteListener(new OnCompleteListener<AnnotatedData<AchievementBuffer>>() {
                    @Override
                    public void onComplete(@NonNull Task<AnnotatedData<AchievementBuffer>> task)
                    {
                        Log.i( "GameCenter", "Getting achievements" );
                        if ( task.isSuccessful() ) m_AllAchievements = task.getResult().get();
                    }
                });
            }
            else
            {
                Log.i( "GameCenter", "Login: User is not logged in" );
                m_GameCenterLoggedIn = 0;
            }
        });
    }

    public static void GameCenterLogin( final Activity act )
    {
        if ( m_GameCenterLoggedIn == 1 ) return;

        Log.i( "GameCenter", "Requesting login" );
        GamesSignInClient gamesSignInClient = PlayGames.getGamesSignInClient( act );
        gamesSignInClient.signIn();
    }

    public static void GameCenterLogout()
    {
        // no longer possible
    }

    public static int GetGameCenterLoggedIn()
    {
        return m_GameCenterLoggedIn;
    }

    public static void GameCenterSubmitAchievement( String szAchievementID, int iPercentageComplete )
    {
        if ( m_AllAchievements == null ) return;

        Achievement ach;

        for (Achievement m_allAchievement : m_AllAchievements)
        {
            ach = m_allAchievement;
            if (szAchievementID.equals(ach.getAchievementId())) {
                if (ach.getType() == Achievement.TYPE_INCREMENTAL) {
                    if (iPercentageComplete != 0) {
                        AchievementsClient achievementsClient = PlayGames.getAchievementsClient(g_pAct);
                        achievementsClient.setSteps(szAchievementID, iPercentageComplete);
                    }
                } else {
                    AchievementsClient achievementsClient = PlayGames.getAchievementsClient(g_pAct);
                    achievementsClient.unlock(szAchievementID);
                }
                break;
            }
        }
    }

    public static String GetGameCenterPlayerID()
    {
        return g_GamesPlayerID;
    }

    public static String GetGameCenterPlayerDisplayName()
    {
        return g_GamesPlayerName;
    }

    public static void GameCenterAchievementsShow( final Activity act )
    {
        AchievementsClient client = PlayGames.getAchievementsClient( act );
        client.getAchievementsIntent().addOnSuccessListener(new OnSuccessListener<Intent>() {
            @Override
            public void onSuccess(Intent intent) {
                act.startActivityForResult( intent, 0 );
            }
        });
    }

    public static void GameCenterSubmitScore( String szBoardID, int iScore )
    {
        LeaderboardsClient client = PlayGames.getLeaderboardsClient( g_pAct );
        client.submitScore( szBoardID, iScore );
    }

    public static void GameCenterShowLeaderBoard( Activity act, String szBoardID )
    {
        LeaderboardsClient client = PlayGames.getLeaderboardsClient( g_pAct );
        client.getLeaderboardIntent( szBoardID ).addOnSuccessListener(new OnSuccessListener<Intent>() {
            @Override
            public void onSuccess(Intent intent) {
                g_pAct.startActivityForResult( intent, 0 );
            }
        });
    }
}