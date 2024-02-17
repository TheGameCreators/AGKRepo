package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.media.MediaRecorder;
import android.os.Build;
import android.util.Log;
import android.view.Surface;

import androidx.core.content.ContextCompat;

public class RecordScreenSDK
{
    static MediaRecorder mMediaRecorder = null;

    public static void onStart( Activity act )
    {
        if (mMediaRecorder != null)
        {
            if (Build.VERSION.SDK_INT >= 24)
            {
                try { mMediaRecorder.resume(); }
                catch( IllegalStateException e ) { Log.w("ScreenRecorder", "Tried to resume MediaRecorder from invalid state"); }
            }
        }
    }

    public static void onStop( Activity act )
    {
        if (mMediaRecorder != null)
        {
            if (Build.VERSION.SDK_INT >= 24) mMediaRecorder.pause();
            else StopScreenRecording();
        }
    }

    public static int StartScreenRecording( Activity act, String filename, int microphone )
    {
        if ( Build.VERSION.SDK_INT < 21 )
        {
            Log.w("Screen Recording", "Screen recording requires Android 5.0 or above");
            return 0;
        }

        if ( mMediaRecorder != null ) return 0;

        int width = act.getWindow().getDecorView().getWidth();
        int height = act.getWindow().getDecorView().getHeight();
        if (width > height) {
            if (width > 1280 || height > 720 )
            {
                float scaleW = 1280.0f / width;
                float scaleH = 720.0f / height;
                if ( scaleH < scaleW ) scaleW = scaleH;
                width = (int) (width * scaleW);
                height = (int) (height * scaleW);
            }
        } else {
            if (width > 720 || height > 1280 )
            {
                float scaleW = 720.0f / width;
                float scaleH = 1280.0f / height;
                if (scaleH < scaleW) scaleW = scaleH;
                width = (int) (width * scaleW);
                height = (int) (height * scaleW);
            }
        }
        int audioSource = 0;
        if (microphone == 1) {
            if (ContextCompat.checkSelfPermission(act, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED) {
                audioSource = MediaRecorder.AudioSource.MIC;
            } else {
                Log.w("Screen Recording", "The app does not have the RECORD_AUDIO permission, video will have no audio");
            }
        }

        mMediaRecorder = new MediaRecorder();
        if (audioSource > 0) mMediaRecorder.setAudioSource(audioSource);
        mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.SURFACE);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
        mMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
        if (audioSource > 0) {
            mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
            mMediaRecorder.setAudioEncodingBitRate(96000);
            mMediaRecorder.setAudioSamplingRate(44100);
        }
        mMediaRecorder.setVideoEncodingBitRate(2048 * 1000);
        mMediaRecorder.setVideoFrameRate(30);
        mMediaRecorder.setVideoSize(width, height);
        mMediaRecorder.setOutputFile(filename);
        try {
            mMediaRecorder.prepare();
        } catch (Exception e) {
            e.printStackTrace();
            mMediaRecorder.release();
            mMediaRecorder = null;
            Log.w("Screen Recording", "Failed to prepare media recorder");
            return 0;
        }

        mMediaRecorder.start();

        return 1; // success
    }

    public static void StopScreenRecording()
    {
        if ( Build.VERSION.SDK_INT >= 21 ) {
            if (mMediaRecorder != null) {
                try
                {
                    mMediaRecorder.stop();
                }
                catch( IllegalStateException e )
                {
                    Log.w("ScreenRecorder", "Tried to stop media recorder in an illegal state");
                }
                mMediaRecorder.release();
                mMediaRecorder = null;
            }
        }
    }

    public static int IsScreenRecording()
    {
        return mMediaRecorder == null ? 0 : 1;
    }

    public static Surface GetScreenRecordSurface()
    {
        if ( Build.VERSION.SDK_INT < 21 )  return null;
        if ( mMediaRecorder == null ) return null;

        return mMediaRecorder.getSurface();
    }
}