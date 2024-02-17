package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Build;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;

public class CameraSDK
{
    public static int deviceCameraID = -1;
    public static Camera deviceCamera = null;
    public static SurfaceTexture pTexture = null;
    public static Surface pSurface = null;
    public static int iLastTex = 0;
    public static int cameraWidth = 0;
    public static int cameraHeight = 0;

    static public void Stop()
    {
        Log.i("Camera","Stop Camera");

        if ( deviceCamera != null )
        {
            deviceCamera.stopPreview();
            deviceCamera.release();
            deviceCamera = null;
        }

        if ( pTexture != null ) pTexture = null;
        if ( pSurface != null ) pSurface = null;
        iLastTex = 0;
    }

    static public void Start(int tex, int cameraID)
    {
        Log.i("Camera","Start Camera");

        if ( Build.VERSION.SDK_INT >= 14 )
        {
            if ( pTexture != null && tex != iLastTex )
            {
                Stop();
            }
            iLastTex = tex;

            if ( deviceCamera == null )
            {
                if ( cameraID >= Camera.getNumberOfCameras() ) cameraID = 0;

                pTexture = new SurfaceTexture(tex);
                deviceCamera = Camera.open(cameraID);

                Camera.Parameters params = deviceCamera.getParameters();
                Camera.Size previewSize = params.getPreferredPreviewSizeForVideo();
                if ( previewSize == null ) previewSize = params.getPreviewSize();
                cameraWidth = previewSize.width;
                cameraHeight = previewSize.height;

                if (params.getSupportedFocusModes().contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
                {
                    params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
                    deviceCamera.setParameters(params);
                }

                try {
                    deviceCamera.setPreviewTexture(pTexture);
                    deviceCamera.startPreview();
                }
                catch( IOException e ){
                    Log.e( "Camera", "Failed to set camera texture" + e.toString() );
                }
            }
        }
        else
        {
            Log.e("Camera", "Playing device camera to image is not supported on this device");
            AGKHelper.ShowMessage(AGKHelper.g_pAct,"Playing device camera to image is not supported on this device" );
        }
    }

    static public void Start( Surface surface, int cameraID )
    {
        Log.i("Camera","Start Camera");

        if ( Build.VERSION.SDK_INT >= 14 )
        {
            if ( deviceCamera == null )
            {
                pSurface = surface;
                if ( cameraID >= Camera.getNumberOfCameras() ) cameraID = 0;

                deviceCamera = Camera.open(cameraID);

                Camera.Parameters params = deviceCamera.getParameters();
                Camera.Size previewSize = params.getPreferredPreviewSizeForVideo();
                if ( previewSize == null ) previewSize = params.getPreviewSize();
                cameraWidth = previewSize.width;
                cameraHeight = previewSize.height;

                if (params.getSupportedFocusModes().contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
                {
                    params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
                    deviceCamera.setParameters(params);
                }

                //try {
                //deviceCamera.setPreviewSurface();
                deviceCamera.startPreview();
				/*
				}
				catch( IOException e ){
					Log.e( "Camera", "Failed to set camera texture" + e.toString() );
				}
				*/
            }
        }
        else
        {
            Log.e("Camera", "Playing device camera to image is not supported on this device");
            AGKHelper.ShowMessage(AGKHelper.g_pAct,"Playing device camera to image is not supported on this device" );
        }
    }

    public static void OnStop( Activity act )
    {
        Stop();
    }

    public static void SetDeviceCameraToImage( Activity act, int tex, int cameraID )
    {
        if ( tex <= 0)
        {
            Stop();
            deviceCameraID = -1;
        }
        else
        {
            Start(tex, cameraID);
            deviceCameraID = cameraID;
        }
    }

    public static void SetDeviceCameraToSurface( Activity act, Surface surface, int cameraID )
    {
        if ( surface == null )
        {
            Stop();
            deviceCameraID = -1;
        }
        else
        {
            Start(surface, cameraID);
            deviceCameraID = cameraID;
        }
    }

    public static int GetCameraWidth()
    {
        return cameraWidth;
    }

    public static int GetCameraHeight()
    {
        return cameraHeight;
    }

    public static void UpdateCamera()
    {
        if ( Build.VERSION.SDK_INT < 14 ) return;

        if ( pTexture != null ) pTexture.updateTexImage();
    }

    public static int GetNumCameras()
    {
        return Camera.getNumberOfCameras();
    }

    public static int GetCameraType( int cameraID )
    {
        if ( cameraID < 0 || cameraID >= Camera.getNumberOfCameras() ) return 0;

        android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
        android.hardware.Camera.getCameraInfo(cameraID, info);

        if ( info.facing == Camera.CameraInfo.CAMERA_FACING_BACK ) return 1;
        else if ( info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT ) return 2;
        else return 0;
    }
}