package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.app.Dialog;
import android.content.IntentSender;
import android.location.Location;
import android.os.Bundle;
import android.util.Log;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationListener;

class AGKLocationListener implements GoogleApiClient.ConnectionCallbacks,
        GoogleApiClient.OnConnectionFailedListener,
        LocationListener
{
    Activity act;

    public void onConnected(Bundle dataBundle) {
        Log.i("GPS","Connected");
        Location mCurrentLocation;
        try {
            mCurrentLocation = LocationServices.FusedLocationApi.getLastLocation(LocationSDK.m_GPSClient);
            if ( mCurrentLocation != null )
            {
                LocationSDK.m_fGPSLatitude = (float) mCurrentLocation.getLatitude();
                LocationSDK.m_fGPSLongitude = (float) mCurrentLocation.getLongitude();
            }

            LocationRequest mLocationRequest = LocationRequest.create();
            mLocationRequest.setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);
            mLocationRequest.setInterval(5000);
            mLocationRequest.setFastestInterval(1000);

            LocationServices.FusedLocationApi.requestLocationUpdates(LocationSDK.m_GPSClient, mLocationRequest, this);
        }
        catch( SecurityException e ) {
            Log.e( "GPS", "User has not granted location permission" );
        }
    }

    public void onDisconnected() {
        Log.i("GPS","Disconnected");
    }

    public void onConnectionFailed(ConnectionResult connectionResult) {
        /*
         * Google Play services can resolve some errors it detects.
         * If the error has a resolution, try sending an Intent to
         * start a Google Play services activity that can resolve
         * error.
         */
        if (connectionResult.hasResolution()) {
            try {
                // Start an Activity that tries to resolve the error
                connectionResult.startResolutionForResult(act,9000);
                /*
                 * Thrown if Google Play services canceled the original
                 * PendingIntent
                 */
            } catch (IntentSender.SendIntentException e) {
                // Log the error
                e.printStackTrace();
            }
        } else {
            /*
             * If no resolution is available, display a dialog to the
             * user with the error.
             */
            if ( connectionResult.getErrorCode() == ConnectionResult.SERVICE_MISSING
                    || connectionResult.getErrorCode() == ConnectionResult.SERVICE_VERSION_UPDATE_REQUIRED
                    || connectionResult.getErrorCode() == ConnectionResult.SERVICE_DISABLED )
            {
                Dialog resolution = GooglePlayServicesUtil.getErrorDialog(connectionResult.getErrorCode(), act, 9000 );
                resolution.show();
            }
            else
            {
                AGKHelper.ShowMessage(act,connectionResult.toString());
            }
        }
    }

    public void onLocationChanged(Location location)
    {
        LocationSDK.m_fGPSLatitude = (float) location.getLatitude();
        LocationSDK.m_fGPSLongitude = (float) location.getLongitude();
        LocationSDK.m_fGPSAltitude = (float) location.getAltitude();
        Log.i("GPS", "Updated");
    }

    @Override
    public void onConnectionSuspended(int arg0) {}
}

public class LocationSDK
{
    static int m_GoogleLocationServicesCheck = -1;
    static AGKLocationListener m_GPSListener = null;
    static float m_fGPSLatitude;
    static float m_fGPSLongitude;
    static float m_fGPSAltitude;
    static GoogleApiClient m_GPSClient = null;
    static boolean m_GPSRequested = false;

    public static void onStart( Activity act )
    {
        if ( m_GPSRequested && m_GPSClient != null && !m_GPSClient.isConnected() && !m_GPSClient.isConnecting() )
        {
            m_GPSClient.connect();
        }
    }

    public static void onStop( Activity act )
    {
        if (m_GPSClient != null) {
            if (m_GPSClient.isConnected()) {
                LocationServices.FusedLocationApi.removeLocationUpdates(m_GPSClient, m_GPSListener);
            }
            m_GPSClient.disconnect();
        }
    }

    public static int GetGPSExists( Activity act )
    {
        if ( m_GoogleLocationServicesCheck < 0 )
        {
            int resultCode = GooglePlayServicesUtil.isGooglePlayServicesAvailable(act);
            if (ConnectionResult.SUCCESS == resultCode) m_GoogleLocationServicesCheck = 1;
            else m_GoogleLocationServicesCheck = 0;
        }

        return m_GoogleLocationServicesCheck;
    }

    public static void StartGPSTracking( Activity act )
    {
        if ( m_GPSListener == null )
        {
            m_GPSListener = new AGKLocationListener();
            m_GPSListener.act = act;
        }

        if ( m_GPSClient == null )
        {
            m_GPSClient = new GoogleApiClient.Builder(act)
                    .addApi(LocationServices.API)
                    .addConnectionCallbacks(m_GPSListener)
                    .addOnConnectionFailedListener(m_GPSListener)
                    .build();
        }

        m_GPSRequested = true;
        if ( !m_GPSClient.isConnected() && !m_GPSClient.isConnecting() ) m_GPSClient.connect();
    }

    public static void StopGPSTracking()
    {
        if ( m_GPSClient != null )
        {
            if ( m_GPSClient.isConnected() )
            {
                LocationServices.FusedLocationApi.removeLocationUpdates(m_GPSClient, m_GPSListener);
            }
            m_GPSClient.disconnect();
        }
        m_GPSRequested = false;
    }

    public static float GetGPSLatitude()
    {
        return m_fGPSLatitude;
    }

    public static float GetGPSLongitude()
    {
        return m_fGPSLongitude;
    }

    public static float GetGPSAltitude()
    {
        return m_fGPSAltitude;
    }
}