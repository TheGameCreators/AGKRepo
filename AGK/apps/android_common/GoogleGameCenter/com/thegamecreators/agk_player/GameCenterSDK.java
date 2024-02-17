package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import androidx.annotation.NonNull;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInStatusCodes;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.AnnotatedData;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.GamesClient;
import com.google.android.gms.games.LeaderboardsClient;
import com.google.android.gms.games.Player;
import com.google.android.gms.games.PlayersClient;
import com.google.android.gms.games.achievement.Achievement;
import com.google.android.gms.games.achievement.AchievementBuffer;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;

import java.util.Iterator;

public class GameCenterSDK
{
    static GoogleSignInClient g_GamesSignIn = null;
    static GoogleSignInAccount g_GamesAccount = null;
    static String g_GamesPlayerID = "";
    static String g_GamesPlayerName = "";
    static int m_GameCenterLoggedIn = 0;
    static AchievementBuffer m_AllAchievements = null;
    static int m_GoogleGameServicesCheck = -1;

    public static void activityCallback( Activity act, int requestCode, int resultCode, Intent data )
    {
        Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);
        try
        {
            g_GamesAccount = task.getResult(ApiException.class);
            GameCenterCompleteLogin( act );
        }
        catch (ApiException e)
        {
            Log.e("Games Sign In", "Failed to sign in, error code: " + e.getStatusCode());
            if (e.getStatusCode() == GoogleSignInStatusCodes.SIGN_IN_CANCELLED)
            {
                m_GameCenterLoggedIn = -1;
            }
            else if (e.getStatusCode() != GoogleSignInStatusCodes.SIGN_IN_CURRENTLY_IN_PROGRESS)
            {
                m_GameCenterLoggedIn = -1;
            }
            g_GamesAccount = null;
            g_GamesSignIn = null;
        }
    }

    public static void onStart( Activity act )
    {
        if ( g_GamesSignIn == null ) return;

        g_GamesAccount = null;
        m_GameCenterLoggedIn = 0;

        // silent only
        Task<GoogleSignInAccount> task = g_GamesSignIn.silentSignIn();
        if (task.isSuccessful())
        {
            g_GamesAccount = task.getResult();
            GameCenterCompleteLogin( act );
        }
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
        if ( g_pAct == null ) g_pAct = act;

        //if ( g_GamesSignIn != null ) GameCenterLogout();

        if ( g_GamesSignIn == null )
        {
            GoogleSignInOptions signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN).build();
            g_GamesSignIn = GoogleSignIn.getClient(act, signInOptions);
        }
    }

    public static void GameCenterLogin( final Activity act )
    {
        if ( g_GamesSignIn == null ) return;

        g_GamesAccount = null;
        m_GameCenterLoggedIn = 0;

        Task<GoogleSignInAccount> task = g_GamesSignIn.silentSignIn();
        if (task.isSuccessful())
        {
            g_GamesAccount = task.getResult();
            GameCenterCompleteLogin( act );
        }
        else {
            task.addOnCompleteListener(new OnCompleteListener<GoogleSignInAccount>() {
                @Override
                public void onComplete(@NonNull Task<GoogleSignInAccount> task) {
                    try {
                        g_GamesAccount = task.getResult(ApiException.class);
                        GameCenterCompleteLogin( act );
                    }
                    catch (ApiException apiException)
                    {
                        if (apiException.getStatusCode() == GoogleSignInStatusCodes.SIGN_IN_REQUIRED) {
                            Log.i("Games Sign In", "Prompting user to sign in");
                            if ( g_GamesSignIn == null )
                            {
                                Log.i("Games Sign In", "GamesSignIn is null");
                            }
                            else
                            {
                                Intent signInIntent = g_GamesSignIn.getSignInIntent();
                                act.startActivityForResult(signInIntent, 10004);
                            }
                        }
                        else
                        {
                            Log.e("Games Sign In", "Failed to sign in user: " + apiException.toString());
                            m_GameCenterLoggedIn = -1;
                            g_GamesAccount = null;
                            g_GamesSignIn = null;
                        }
                    }
                }
            });
        }
    }

    public static void GameCenterCompleteLogin( final Activity act )
    {
        GamesClient gamesClient = Games.getGamesClient( act, g_GamesAccount );
        gamesClient.setViewForPopups( act.getWindow().getDecorView() );

        PlayersClient playersClient = Games.getPlayersClient( act, g_GamesAccount );
        Task<Player> playerTask = playersClient.getCurrentPlayer().addOnCompleteListener(
                new OnCompleteListener<Player>() {
                    @Override
                    public void onComplete(@NonNull Task<Player> task) {
                        if ( task.isSuccessful() ) {
                            g_GamesPlayerName = task.getResult().getDisplayName();
                            g_GamesPlayerID = task.getResult().getPlayerId();
                        }

                        if ( g_GamesAccount == null )
                        {
                            Log.i("Games Sign In", "GamesAccount is null");
                            return;
                        }

                        AchievementsClient achievementsClient = Games.getAchievementsClient( act, g_GamesAccount );
                        achievementsClient.load(false).addOnCompleteListener(new OnCompleteListener<AnnotatedData<AchievementBuffer>>() {
                            @Override
                            public void onComplete(@NonNull Task<AnnotatedData<AchievementBuffer>> task) {
                                if ( task.isSuccessful() ) m_AllAchievements = task.getResult().get();

                                m_GameCenterLoggedIn = 1;
                            }
                        });
                    }
                }
        );
    }

    public static void GameCenterLogout()
    {
        if ( g_GamesSignIn != null ) g_GamesSignIn.signOut();
        m_GameCenterLoggedIn = 0;
        g_GamesAccount = null;
    }

    public static int GetGameCenterLoggedIn()
    {
        return m_GameCenterLoggedIn;
    }

    public static void GameCenterSubmitAchievement( String szAchievementID, int iPercentageComplete )
    {
        if ( g_GamesAccount == null ) return;
        if ( m_AllAchievements == null ) return;

        Achievement ach;

        for (Achievement m_allAchievement : m_AllAchievements)
        {
            ach = m_allAchievement;
            if (szAchievementID.equals(ach.getAchievementId())) {
                if (ach.getType() == Achievement.TYPE_INCREMENTAL) {
                    if (iPercentageComplete != 0) {
                        AchievementsClient achievementsClient = Games.getAchievementsClient(g_pAct, g_GamesAccount);
                        achievementsClient.setSteps(szAchievementID, iPercentageComplete);
                    }
                } else {
                    AchievementsClient achievementsClient = Games.getAchievementsClient(g_pAct, g_GamesAccount);
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
        if ( g_GamesAccount == null ) return;

        AchievementsClient client = Games.getAchievementsClient( act, g_GamesAccount );
        client.getAchievementsIntent().addOnSuccessListener(new OnSuccessListener<Intent>() {
            @Override
            public void onSuccess(Intent intent) {
                act.startActivityForResult( intent, 0 );
            }
        });
    }

    public static void GameCenterSubmitScore( String szBoardID, int iScore )
    {
        if ( g_GamesAccount == null ) return;

        LeaderboardsClient client = Games.getLeaderboardsClient( g_pAct, g_GamesAccount );
        client.submitScore( szBoardID, iScore );
    }

    public static void GameCenterShowLeaderBoard( Activity act, String szBoardID )
    {
        if ( g_GamesAccount == null ) return;

        LeaderboardsClient client = Games.getLeaderboardsClient( g_pAct, g_GamesAccount );
        client.getLeaderboardIntent( szBoardID ).addOnSuccessListener(new OnSuccessListener<Intent>() {
            @Override
            public void onSuccess(Intent intent) {
                g_pAct.startActivityForResult( intent, 0 );
            }
        });
    }
}