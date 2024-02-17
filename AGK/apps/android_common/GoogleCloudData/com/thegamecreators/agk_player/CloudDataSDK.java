package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.accounts.Account;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;

import androidx.annotation.NonNull;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInStatusCodes;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.common.api.Scope;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.api.client.googleapis.extensions.android.gms.auth.GoogleAccountCredential;
import com.google.api.client.http.ByteArrayContent;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.jackson2.JacksonFactory;
import com.google.api.services.drive.Drive;
import com.google.api.services.drive.model.File;
import com.google.api.services.drive.model.FileList;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

public class CloudDataSDK
{
    public static int g_iCloudDataChanged = 0;
    public static int g_iCloudDataStatus = 0;
    public static int g_iCloudDataStatusInProgess = 0;
    public static GoogleSignInClient g_GoogleSignIn = null;
    public static GoogleSignInAccount g_GoogleAccount = null;
    public static Activity g_pCloudActivity = null;
    public static Date g_CloudLastChecked = null;
    public static Timer g_CloudRefreshTimer = null;
    public static TimerTask g_CloudRefreshTask = null;
    public static Drive g_DriveService = null;
    public static String g_sCloudAppDatafolder = null;

    public static void activityCallback( Activity act, int requestCode, int resultCode, Intent data )
    {
        Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);
        try {
            g_GoogleAccount = task.getResult(ApiException.class);
            FinishCloudDataSetup(task);
        } catch (ApiException e) {
            Log.e("Google Sign In", "Failed to sign in, error code: " + e.getStatusCode());
            if (e.getStatusCode() == GoogleSignInStatusCodes.SIGN_IN_CANCELLED)
                g_iCloudDataStatus = -1;
            else if (e.getStatusCode() != GoogleSignInStatusCodes.SIGN_IN_CURRENTLY_IN_PROGRESS)
                g_iCloudDataStatus = -2;
        }
    }

    public static void onStart( Activity act )
    {
        if ( g_CloudRefreshTask != null )
        {
            try {
                g_CloudRefreshTask = new TimerTask() {
                    @Override
                    public void run() {
                        UpdateCloudVariables();
                    }
                };
                g_CloudRefreshTimer = new Timer();
                g_CloudRefreshTimer.schedule(g_CloudRefreshTask, 0, 60000);
            }
            catch( IllegalStateException e ) { Log.e("Cloud Data","Failed to restart Cloud Data thread:"+e.toString()); }
        }
    }

    public static void onStop( Activity act )
    {
        if (  g_CloudRefreshTask != null )
        {
            try {
                g_CloudRefreshTask.cancel();
                g_CloudRefreshTimer.cancel();
                g_CloudRefreshTimer = null;
            }
            catch( IllegalStateException e ) { Log.e("Cloud Data","Failed to stop Cloud Data thread:"+e.toString()); }
        }
    }

    public static void SetupCloudData( final Activity act )
    {
        g_pCloudActivity = act;
        if ( g_CloudLastChecked == null ) g_CloudLastChecked = new Date(0);

        if ( g_GoogleSignIn == null )
        {
            GoogleSignInOptions signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                    .requestScopes(new Scope("https://www.googleapis.com/auth/drive.appdata"))
                    .requestEmail()
                    .build();
            g_GoogleSignIn = GoogleSignIn.getClient(act, signInOptions);
        }

        if ( g_GoogleAccount == null )
        {
            Task<GoogleSignInAccount> task = g_GoogleSignIn.silentSignIn();
            if (task.isSuccessful())
            {
                g_GoogleAccount = task.getResult();
                FinishCloudDataSetup( task );
            }
            else {
                task.addOnCompleteListener(new OnCompleteListener<GoogleSignInAccount>() {
                    @Override
                    public void onComplete(@NonNull Task<GoogleSignInAccount> task) {
                        try {
                            g_GoogleAccount = task.getResult(ApiException.class);
                            FinishCloudDataSetup( task );
                        }
                        catch (ApiException apiException)
                        {
                            if (apiException.getStatusCode() == GoogleSignInStatusCodes.SIGN_IN_REQUIRED) {
                                Log.i("Google Sign In", "Prompting user to sign in");
                                Intent signInIntent = g_GoogleSignIn.getSignInIntent();
                                act.startActivityForResult( signInIntent, 10003 );
                            }
                            else
                            {
                                Log.e("Google Sign In", "Failed to sign in user: " + apiException.toString());
                            }
                        }
                    }
                });
            }
        }
    }

    public static void UpdateCloudVariables()
    {
        //Log.i("Cloud Data", "Checking cloud files" );

        FileList result;
        try {
            result = g_DriveService.files().list()
                    .setSpaces("appDataFolder")
                    .setPageSize(1000)
                    .setFields("files(id, name, modifiedTime)")
                    .setQ("'"+g_sCloudAppDatafolder+"' in parents")
                    .execute();
        } catch (IOException e) {
            Log.e("Cloud Data", "Failed to get CloudVariables file list: " + e.toString());
            return;
        }

        //Log.i("Cloud Data", "Checking for mofified files, last checked: " + g_CloudLastChecked.toString() );

        List<File> files = result.getFiles();
        //Log.i("Cloud Data", "Got " + files.size() + " files" );

        // build a map of existing values to check for deleted items
        Map<String, String> map = new HashMap<String, String>();
        SharedPreferences sharedPref = g_pCloudActivity.getSharedPreferences("agkclouddatavariables", Context.MODE_PRIVATE);
        Iterator<String> iter = sharedPref.getAll().keySet().iterator();
        //Log.i("Cloud Data", "Current variables:");
        while (iter.hasNext()) {
            String name = iter.next();
            //Log.i("Cloud Data", "  - " + name);
            map.put(name, "0");
        }

        for (int i = 0; i < files.size(); i++) {
            com.google.api.services.drive.model.File file = files.get(i);
            final String filename = file.getName();
            if ( map.get(filename) != null && map.get(filename).equals("1") )
            {
                // duplicate
            }
            map.put(filename, "1"); // item still exists
            //Log.i("Cloud Data", "  - " + filename);

            SimpleDateFormat formatter = new SimpleDateFormat( "yyyy-MM-dd'T'HH:mm:ss.SSS" );
            Date fileModified;
            try { fileModified = formatter.parse( file.getModifiedTime().toString() ); }
            catch( ParseException e ) {
                Log.w( "Cloud Date", "Failed to parse file modified date: " + e.toString() );
                continue;
            }

            //Log.i("Cloud Data", "  File: " + filename + ", Modified: " + fileModified.toString() );

            int modified = 0;
            if (fileModified.after(g_CloudLastChecked)) {
                InputStream fileInputStream;
                try {
                    fileInputStream = g_DriveService.files().get(file.getId())
                            .executeMediaAsInputStream();
                    BufferedReader reader = new BufferedReader(new InputStreamReader(fileInputStream));
                    String value = reader.readLine();
                    SharedPreferences.Editor edit = sharedPref.edit();
                    edit.putString(filename, value);
                    edit.apply();
                    modified = 1;
                    //Log.i("Cloud Data", "  Updated file: " + filename);
                }
                catch( IOException e ) {
                    Log.w( "Cloud Data", "Failed to read file: " + file.getName() + ": " + e.toString() );
                    continue;
                }
            }

            if ( modified > 0 ) g_iCloudDataChanged = 1;
        }
        g_CloudLastChecked = Calendar.getInstance().getTime();

        // delete any missing values
        Iterator<String> iter2 = sharedPref.getAll().keySet().iterator();
        SharedPreferences.Editor edit = sharedPref.edit();
        while (iter2.hasNext()) {
            String name = iter2.next();
            String present = map.get(name);
            if (present != null && present.equals("0")) {
                //Log.i("Cloud Data", "Removed file: " + name);
                edit.remove(name);
            }
        }
        edit.apply();
    }

    public static void FinishCloudDataSetup( Task<GoogleSignInAccount> signInTask )
    {
        Log.i("Cloud Data", "Finishing cloud data setup");
        if ( g_iCloudDataStatusInProgess == 1 || g_iCloudDataStatus == 1 ) return;
        g_iCloudDataStatusInProgess = 1;

        Log.i("Cloud Data", "Finishing cloud data setup2");

        List<String> permissions = new ArrayList<String>();
        permissions.add("https://www.googleapis.com/auth/drive.appdata");
        GoogleAccountCredential accountCredential = GoogleAccountCredential.usingOAuth2( g_pCloudActivity, permissions );
        Account account = g_GoogleAccount.getAccount();
        if ( account == null )
        {
            Log.e( "Cloud Data", "Account is null" );
            g_iCloudDataStatusInProgess = 0;
            g_iCloudDataStatus = -1;
            return;
        }
        accountCredential.setSelectedAccount( account );

        NetHttpTransport HTTP_TRANSPORT = new com.google.api.client.http.javanet.NetHttpTransport();
        if ( HTTP_TRANSPORT == null )
        {
            g_iCloudDataStatusInProgess = 0;
            g_iCloudDataStatus = -2;
            Log.e( "Cloud Data", "Failed to get NetHttpTransport" );
            return;
        }

        g_DriveService = new Drive.Builder( HTTP_TRANSPORT, JacksonFactory.getDefaultInstance(), accountCredential )
                .setApplicationName( g_pCloudActivity.getPackageName() )
                .build();
        if ( g_DriveService == null )
        {
            Log.e( "Cloud Data", "Drive service is null" );
            g_iCloudDataStatusInProgess = 0;
            g_iCloudDataStatus = -2;
            return;
        }

        Thread enumFiles = new Thread() {
            @Override
            public void run() {
                FileList result;
                try {
                    // Print the names and IDs for up to 10 files.
                    result = g_DriveService.files().list()
                            .setSpaces("appDataFolder")
                            .setPageSize(10)
                            .setQ("name='CloudVariables'")
                            .setFields("nextPageToken, files(id, name)")
                            .execute();
                } catch (IOException e) {
                    Log.e("Cloud Data", "Failed to get file list: " + e.toString());
                    g_iCloudDataStatusInProgess = 0;
                    g_iCloudDataStatus = -2;
                    return;
                }

                List<com.google.api.services.drive.model.File> files = result.getFiles();
                boolean create = true;
                if ( files != null && !files.isEmpty() )
                {
                    if ( files.size() > 1 )
                    {
                        Log.e( "Cloud Data", "Found " + files.size() + " CloudVariables folders, only one must exist, please clear the app data in your Drive settings" );
                        g_iCloudDataStatus = -3;
                        g_iCloudDataStatusInProgess = 0;
                        return;
                    }

                    Log.i("Cloud Data", "Found CloudVariables folder");
                    g_sCloudAppDatafolder = files.get(0).getId();
                    create = false;
					/*
					for (int i = 1; i < files.size(); i++ ) {
						try{ g_DriveService.files().delete(files.get(i).getId()).execute(); }
						catch (IOException e) { Log.e( "Cloud Data", "Failed to delete duplicate CloudVariables folder: " + e.toString() ); }
					}
					*/
                }

                if ( create )
                {
                    com.google.api.services.drive.model.File fileMetadata = new com.google.api.services.drive.model.File();
                    fileMetadata.setName("CloudVariables");
                    fileMetadata.setMimeType("application/vnd.google-apps.folder");
                    fileMetadata.setParents(Collections.singletonList("appDataFolder"));

                    try {
                        com.google.api.services.drive.model.File file = g_DriveService.files().create(fileMetadata)
                                .setFields("id")
                                .execute();
                        Log.i("Cloud Data", "Created CloudVariables folder");
                        g_sCloudAppDatafolder = file.getId();
                    }
                    catch( IOException e )
                    {
                        Log.e("Cloud Data", "Failed to create CloudVariables folder: " + e.toString());
                        g_iCloudDataStatusInProgess = 0;
                        g_iCloudDataStatus = -2;
                        return;
                    }
                }

                if ( g_CloudRefreshTask == null ) {
                    try {
                        g_CloudRefreshTask = new TimerTask() {
                            @Override
                            public void run() {
                                UpdateCloudVariables();
                            }
                        };
                        g_CloudRefreshTimer = new Timer();
                        g_CloudRefreshTimer.schedule(g_CloudRefreshTask, 0, 60000);
                    } catch (IllegalStateException e) {
                        Log.e("Cloud Data", "Failed to start Cloud Data thread:" + e.toString());
                    }
                }
                g_iCloudDataStatusInProgess = 0;
                g_iCloudDataStatus = 1;
            }
        };
        enumFiles.start();
    }

    public static int GetCloudDataAllowed( Activity act )
    {
        return g_iCloudDataStatus;
    }

    public static int GetCloudDataChanged()
    {
        return g_iCloudDataChanged;
    }

    public static String GetCloudDataVariable( Activity act, String varName, String defaultValue )
    {
        if ( g_DriveService == null || g_iCloudDataStatus != 1 ) return defaultValue;
        g_iCloudDataChanged = 0;

        varName = varName.replace("/","_");
        varName = varName.replace("\\","_");
        varName = varName.replace("*","_");
        varName = varName.replace("?","_");
        varName = varName.replace("\"","_");
        varName = varName.replace("|","_");
        varName = varName.replace("<","_");
        varName = varName.replace(">","_");
        varName = varName.replace(":","_");

        SharedPreferences sharedPref = act.getSharedPreferences("agkclouddatavariables", Context.MODE_PRIVATE);
        return sharedPref.getString(varName, defaultValue);
    }

    public static void SetCloudDataVariable( Activity act, String varName, final String varValue )
    {
        if ( g_DriveService == null || g_iCloudDataStatus != 1 ) return;

        varName = varName.replace("/","_");
        varName = varName.replace("\\","_");
        varName = varName.replace("*","_");
        varName = varName.replace("?","_");
        varName = varName.replace("\"","_");
        varName = varName.replace("|","_");
        varName = varName.replace("<","_");
        varName = varName.replace(">","_");
        varName = varName.replace(":","_");
        final String finalVarname = varName;

        SharedPreferences sharedPref = act.getSharedPreferences("agkclouddatavariables", Context.MODE_PRIVATE);
        SharedPreferences.Editor edit = sharedPref.edit();
        edit.putString( varName, varValue );
        edit.apply();

        // write drive file
        if ( g_DriveService == null || g_iCloudDataStatus != 1 ) return;

        final String finalVarName = varName;
        final String finalVarValue = varValue;

        //Log.i( "Cloud Data", "Set cloud variable: " + finalVarName );

        Thread updateFile = new Thread() {
            @Override
            public void run() {
                FileList result;
                try {
                    result = g_DriveService.files().list()
                            .setSpaces("appDataFolder")
                            .setPageSize(10)
                            .setFields("files(id)")
                            .setQ("'"+g_sCloudAppDatafolder+"' in parents and name='"+finalVarName+"'")
                            .execute();
                } catch (IOException e) {
                    Log.e("Cloud Data", "Failed to get file ID for update: " + e.toString());
                    return;
                }

                List<com.google.api.services.drive.model.File> files = result.getFiles();
                if ( files == null || files.isEmpty() ) {
                    // create file
                    com.google.api.services.drive.model.File fileMetadata = new com.google.api.services.drive.model.File();
                    fileMetadata.setName(finalVarName);
                    fileMetadata.setParents( Collections.singletonList(g_sCloudAppDatafolder) );
                    ByteArrayContent fileContent = new ByteArrayContent("text/plain", finalVarValue.getBytes());
                    try {
                        com.google.api.services.drive.model.File file = g_DriveService.files().create(fileMetadata, fileContent)
                                .setFields("id")
                                .execute();
                    } catch (IOException e) {
                        Log.w("Cloud Data", "Failed to write new variable: " + finalVarName + ": " + e.toString());
                        return;
                    }

                    //Log.i( "Cloud Data", "Created cloud variable: " + finalVarName );
                }
                else
                {
                    com.google.api.services.drive.model.File file = files.get(0);
                    String fileID = file.getId();
                    com.google.api.services.drive.model.File fileMetadata = new com.google.api.services.drive.model.File();
                    ByteArrayContent fileContent = new ByteArrayContent("text/plain", finalVarValue.getBytes());
                    try { g_DriveService.files().update(fileID, fileMetadata, fileContent).execute(); }
                    catch( IOException e )
                    {
                        Log.w("Cloud Data", "Failed to update file: "+finalVarName+": " + e.toString());
                    }

                    //Log.i( "Cloud Data", "Updated cloud variable: " + finalVarName );

                    for (int i = 1; i < files.size(); i++) {
                        file = files.get(i);
                        fileID = file.getId();
                        try { g_DriveService.files().delete(fileID).execute(); }
                        catch( IOException e )
                        {
                            Log.w("Cloud Data", "Failed to delete duplicate file: "+finalVarName+": " + e.toString());
                        }
                    }
                }
            }
        };
        updateFile.start();
    }

    public static void DeleteCloudDataVariable( Activity act, String varName )
    {
        if ( g_DriveService == null || g_iCloudDataStatus != 1 ) return;

        varName = varName.replace("/","_");
        varName = varName.replace("\\","_");
        varName = varName.replace("*","_");
        varName = varName.replace("?","_");
        varName = varName.replace("\"","_");
        varName = varName.replace("|","_");
        varName = varName.replace("<","_");
        varName = varName.replace(">","_");
        varName = varName.replace(":","_");

        SharedPreferences sharedPref = act.getSharedPreferences( "agkclouddatavariables", Context.MODE_PRIVATE );
        SharedPreferences.Editor edit = sharedPref.edit();
        edit.remove(varName);
        edit.apply();

        // delete drive file
        if ( g_DriveService == null || g_iCloudDataStatus != 1 ) return;

        final String finalVarName = varName;

        Thread deleteFile = new Thread() {
            @Override
            public void run() {
                FileList result;
                try {
                    result = g_DriveService.files().list()
                            .setSpaces("appDataFolder")
                            .setPageSize(10)
                            .setFields("files(id)")
                            .setQ("'"+g_sCloudAppDatafolder+"' in parents and name='"+finalVarName+"'")
                            .execute();
                } catch (IOException e) {
                    Log.e("Cloud Data", "Failed to get file ID for deletion: " + e.toString());
                    return;
                }

                //Log.i( "Cloud Data", "Deleted cloud variable: " + finalVarName );

                List<com.google.api.services.drive.model.File> files = result.getFiles();
                for (int i = 0; i < files.size(); i++) {
                    com.google.api.services.drive.model.File file = files.get(i);
                    final String fileID = file.getId();
                    try { g_DriveService.files().delete(fileID).execute(); }
                    catch( IOException e )
                    {
                        Log.w("Cloud Data", "Failed to delete file: "+finalVarName+": " + e.toString());
                    }
                }
            }
        };
        deleteFile.start();
    }
}