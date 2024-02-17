package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.media.MediaMetadataRetriever;
import android.media.MediaPlayer;
import android.os.Build;
import android.util.Log;
import android.view.Gravity;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;

class AGKSurfaceView extends SurfaceView implements SurfaceHolder.Callback, MediaPlayer.OnCompletionListener
{
    public volatile MediaPlayer player = null;
    public Activity act;
    public SurfaceHolder pHolder = null;
    public SurfaceTexture pTexture = null;
    public Surface pSurface = null;
    public int iLastTex = 0;
    public int m_x = -10;
    public int m_y = 0;
    public int m_width = 1;
    public int m_height = 1;
    public volatile String m_filename = "";
    public int m_filetype = 0;
    public int m_isStream = 0;

    public int prepared = 0;
    public int isPlaying = 0;
    public int pausePos = -1;
    public int paused = 0;
    public int completed = 0;

    public int isDisplayed = 0;
    public volatile int videoWidth = 0;
    public volatile int videoHeight = 0;
    public volatile int videoDuration = 0;
    public volatile int viewAdded = 0;
    public volatile int shouldRemoveView = 0;

    public float U1, U2, V1, V2;

    public static WindowManager.LayoutParams makeLayout(int x, int y, int width, int height)
    {
        WindowManager.LayoutParams ll_lp;

        //Just a sample layout parameters.
        ll_lp = new WindowManager.LayoutParams();
        ll_lp.format = PixelFormat.OPAQUE;
        ll_lp.height = height;
        ll_lp.width = width;
        ll_lp.gravity = Gravity.LEFT | Gravity.TOP;
        ll_lp.x = x;
        ll_lp.y = y;
        ll_lp.token = null;
        //ll_lp.gravity |= Gravity.CENTER_HORIZONTAL;
        //ll_lp.gravity |= Gravity.CENTER_VERTICAL;
        ll_lp.type = WindowManager.LayoutParams.TYPE_APPLICATION;
        ll_lp.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
        ll_lp.flags = ll_lp.flags | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
        ll_lp.flags = ll_lp.flags | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE;

        return ll_lp;
    }

    public AGKSurfaceView(Activity context)
    {
        super(context);

        getHolder().addCallback(this);
        getHolder().setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        act = context;
    }

    public void LoadVideo( String filename, int type )
    {
        if ( RunnableVideo.video != null ) RunnableVideo.video.videoDuration = 0;

        Log.i("Video", "Load Video");

        m_filename = filename;
        m_filetype = type;
        U1 = 0;
        V1 = 0;
        U2 = 1;
        V2 = 1;
        m_isStream = 0;

        if ( player != null )
        {
            synchronized( PlayVideoSDK.videoLock )
            {
                player.reset();
                player.release();
                player = null;
            }

            StopVideo();
        }

        int m_duration = -1;
        int m_videoWidth = -1;
        int m_videoHeight = -1;

        MediaMetadataRetriever metaRetriever = new MediaMetadataRetriever();
        MediaPlayer tempPlayer = new MediaPlayer();

        Log.d("Video", "File: "+filename+" Type:"+type );

        try
        {
            switch( type )
            {
                case 0:
                {
                    // asset folder
                    AssetFileDescriptor afd = act.getAssets().openFd(filename);
                    metaRetriever.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(), afd.getLength());
                    tempPlayer.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(), afd.getLength());
                    afd.close();
                    break;
                }
                case 1:
                {
                    // data folder
                    metaRetriever.setDataSource(filename);
                    tempPlayer.setDataSource(filename);
                    break;
                }
                case 2:
                {
                    // expansion file
					/*
					int index = filename.indexOf(':');
					if ( index < 0 )
					{
						Log.e("Load Video","Invalid file name for expansion file");
						m_filename = "";
						return;
					}
					String subfilename = filename.substring(index+1);
					if ( AGKHelper.g_pExpansionFile == null ) {
						AGKHelper.g_pExpansionFile = APKExpansionSupport.getAPKExpansionZipFile(act, AGKHelper.g_iExpansionVersion, AGKHelper.g_iExpansionVersion);
					}
					if (AGKHelper.g_pExpansionFile == null)
					{
						Log.e("Video","Failed to load expansion file");
						m_filename = "";
						return;
					}
					AssetFileDescriptor afd = AGKHelper.g_pExpansionFile.getAssetFileDescriptor(subfilename);
					if ( afd == null )
					{
						Log.e("Video","Failed to find video file in expansion file");
						m_filename = "";
						return;
					}
					metaRetriever.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(), afd.getLength());
					tempPlayer.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(), afd.getLength());
					afd.close();
					*/
                    break;
                }
                case 3:
                {
                    m_isStream = 1;
                    break;
                }
                default:
                {
                    Log.e("Video","Unrecognised file type");
                    m_filename = "";
                    return;
                }
            }

            if ( m_isStream == 1 )
            {
                m_duration = 1000;
                m_videoWidth = 1024;
                m_videoHeight = 768;
            }
            else
            {
                int tempprepared = 0;

                String duration = metaRetriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);
                if (duration != null) {
                    m_duration = Integer.valueOf(duration);
                } else {
                    Log.w("Video", "Duration is null");
                    try {
                        tempPlayer.prepare();
                        tempprepared = 1;
                        m_duration = tempPlayer.getDuration();
                    } catch (Exception e) {
                        Log.e("Video", "Temp player couldn't prepare");
                    }
                }

                Bitmap bmp = metaRetriever.getFrameAtTime();
                if (bmp != null) {
                    m_videoWidth = bmp.getWidth();
                    m_videoHeight = bmp.getHeight();
                } else {
                    Log.w("Video", "Bitmap is null");
                    try {
                        if (tempprepared == 0) tempPlayer.prepare();
                        tempprepared = 1;
                        m_videoWidth = tempPlayer.getVideoWidth();
                        m_videoHeight = tempPlayer.getVideoHeight();
                    } catch (Exception e) {
                        Log.e("Video", "Temp player couldn't prepare 2");
                    }
                }

                Log.d("Video", "Duration: " + m_duration);
                Log.d("Video", "Width: " + Integer.toString(m_videoWidth) + " Height: " + Integer.toString(m_videoHeight));
                if (m_videoWidth == 0) m_videoWidth = -1;
                if (m_videoHeight == 0) m_videoHeight = -1;

                tempPlayer.reset();
                tempPlayer.release();
                tempPlayer = null;
            }
        }
        catch(Exception e)
        {
            Log.e("Exception", e.toString() );
            StackTraceElement[] elements = e.getStackTrace();
            for ( int i = 0; i < elements.length; i++ )
                Log.e("Exception", elements[i].toString() );
            m_filename = "Error";
        }

        videoDuration = m_duration;
        videoWidth = m_videoWidth;
        videoHeight = m_videoHeight;
        pausePos = -1;
        completed = 0;

        RunnableVideo.videoWidth = videoWidth;
        RunnableVideo.videoHeight = videoHeight;
        RunnableVideo.videoDuration = videoDuration;
    }

    public void DeleteVideo()
    {
        Log.i("Video","Delete Video");

        m_filename = "";
        m_filetype = 0;
        paused = 0;

        if ( viewAdded == 1 )
        {
            shouldRemoveView = 0;
            WindowManager wm = (WindowManager) act.getSystemService(Context.WINDOW_SERVICE);
            wm.removeView(this);
            isDisplayed = 0;
        }
        else shouldRemoveView = 1;

        synchronized( PlayVideoSDK.videoLock )
        {
            if ( pTexture != null ) pTexture = null;
            if ( pSurface != null ) pSurface = null;
            iLastTex = 0;
            if ( player != null )
            {
                player.reset();
                player.release();
                player = null;
            }
        }
    }

    public void PlayVideo()
    {
        Log.i("Video","Play Video");
        if ( m_filename.equals("") || m_filename.equals("Error") ) return;

        if ( player != null )
        {
            if ( pausePos >= 0 ) player.seekTo(pausePos);
            pausePos = -1;
            completed = 0;
            player.start();
        }

        paused = 0;

        if ( pTexture != null || pSurface != null )
        {
            Log.e("Video", "Cannot play to screen whilst the video is playing to a texture");
            AGKHelper.ShowMessage(act,"Cannot play video to screen whilst the video is playing to a texture");
        }
        else
        {
            shouldRemoveView = 0;
            if (isDisplayed == 0) {
                WindowManager wm = (WindowManager) act.getSystemService(Context.WINDOW_SERVICE);
                WindowManager.LayoutParams layout = makeLayout(m_x, m_y, m_width, m_height);
                wm.addView(this, layout);
                isDisplayed = 1;
            }
        }
    }

    public void PlayVideoToTexture(int tex)
    {
        Log.i("Video","Play Video To Image");
        if ( m_filename.equals("") || m_filename.equals("Error") ) return;

        if ( player != null )
        {
            if ( pausePos >= 0 ) player.seekTo(pausePos);
            pausePos = -1;
            completed = 0;
            player.start();
        }

        paused = 0;

        if ( isDisplayed == 1 )
        {
            Log.e("Video","Cannot play to texture whilst the video is playing to the screen");
            AGKHelper.ShowMessage(act,"Cannot play video to texture whilst the video is playing to the screen");
        }
        else
        {
            if ( Build.VERSION.SDK_INT >= 14 )
            {
                if ( pTexture != null && tex != iLastTex )
                {
                    OnContextLost();
                }
                iLastTex = tex;

                if ( pTexture == null )
                {
                    pTexture = new SurfaceTexture(tex);
                    setupPlayer(1);
                }
            }
            else
            {
                Log.e("Video", "Playing video to texture is not supported on this device");
                AGKHelper.ShowMessage(act,"Playing video to texture is not supported on this device" );
            }
        }
    }

    public void PlayVideoToSurface(Surface surface)
    {
        Log.i("Video","Play Video To Surface");
        if ( m_filename.equals("") || m_filename.equals("Error") ) return;

        if ( player != null )
        {
            if ( pausePos >= 0 ) player.seekTo(pausePos);
            pausePos = -1;
            completed = 0;
            player.start();
        }

        paused = 0;

        if ( isDisplayed == 1 )
        {
            Log.e("Video","Cannot play to texture whilst the video is playing to the screen");
            AGKHelper.ShowMessage(act,"Cannot play video to texture whilst the video is playing to the screen");
        }
        else
        {
            if ( Build.VERSION.SDK_INT >= 14 )
            {
                setupPlayer(1);
            }
            else
            {
                Log.e("Video", "Playing video to texture is not supported on this device");
                AGKHelper.ShowMessage(act,"Playing video to texture is not supported on this device" );
            }
        }
    }

    public void SetPosition( float seconds )
    {
        Log.i("Video","Set Position");
        if ( player != null )
        {
            player.seekTo((int) (seconds * 1000));
        }
        else
        {
            pausePos = (int)(seconds*1000);
        }
    }

    public void PauseVideo()
    {
        Log.i("Video","Pause Video");
        if ( m_filename.equals("") || m_filename.equals("Error") ) return;

        paused = 1;
        if ( player == null ) return;
        if ( !player.isPlaying() ) return;
        player.pause();
    }

    public void StopVideo()
    {
        Log.i("Video","Stop Video");
        paused = 0;

        if ( viewAdded == 1 )
        {
            shouldRemoveView = 0;
            WindowManager wm = (WindowManager) act.getSystemService(Context.WINDOW_SERVICE);
            wm.removeView(this);
            isDisplayed = 0;
            viewAdded = 0;
        }
        else shouldRemoveView = 1;

        synchronized( PlayVideoSDK.videoLock )
        {
            if ( pTexture != null ) pTexture = null;
            iLastTex = 0;
            if ( pSurface != null ) pSurface = null;
            if ( player != null )
            {
                player.reset();
                player.release();
                player = null;
            }
        }
    }

    public void UpdateVideo()
    {
        synchronized( PlayVideoSDK.videoLock ) {
            if (Build.VERSION.SDK_INT < 14) return;

            if ( player != null ) RunnableVideo.videoPosition = player.getCurrentPosition();

            if (pTexture != null) {
                try {
                    pTexture.updateTexImage();
                    float matrix[] = new float[16];
                    pTexture.getTransformMatrix(matrix);
                    U1 = matrix[12];
                    V1 = matrix[5] + matrix[13];
                    U2 = matrix[0] + matrix[12];
                    V2 = matrix[13];
                } catch (RuntimeException e) {
                    Log.e("Video", "Failed to update video texture: " + e.toString());
                }
            }
        }
    }

    public void SetDimensions( int x, int y, int width, int height )
    {
        Log.i("Video","Set Dimensions X:"+x+" Y:"+y+" Width:"+width+" Height:"+height);

        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;

        if ( viewAdded == 1 && pHolder != null )
        {
            WindowManager wm = (WindowManager) act.getSystemService(Context.WINDOW_SERVICE);
            WindowManager.LayoutParams layout = makeLayout(m_x,m_y,m_width,m_height);
            wm.updateViewLayout(this, layout);
        }
    }

    private void setupPlayer( int mode )
    {
        MediaPlayer newplayer = new MediaPlayer();
        newplayer.setOnCompletionListener(this);
        try
        {
            Log.i("Video2", "File: "+m_filename+" Type:"+m_filetype );

            switch( m_filetype )
            {
                case 0:
                {
                    // asset folder
                    AssetFileDescriptor afd = act.getAssets().openFd(m_filename);
                    newplayer.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(), afd.getLength());
                    afd.close();
                    break;
                }
                case 1:
                {
                    // data folder
                    newplayer.setDataSource(m_filename);
                    break;
                }
                case 2:
                {
                    // expansion file
					/*
					int index = m_filename.indexOf(':');
					if ( index < 0 )
					{
						Log.e("Load Video","Invalid file name for expansion file");
						return;
					}
					String subfilename = m_filename.substring(index+1);
					if ( AGKHelper.g_pExpansionFile == null ) {
						AGKHelper.g_pExpansionFile = APKExpansionSupport.getAPKExpansionZipFile(act, AGKHelper.g_iExpansionVersion, AGKHelper.g_iExpansionVersion);
					}
					if ( AGKHelper.g_pExpansionFile == null )
					{
						Log.e("Video","Failed to load expansion file");
						return;
					}
					AssetFileDescriptor afd = AGKHelper.g_pExpansionFile.getAssetFileDescriptor(subfilename);
					if ( afd == null )
					{
						Log.e("Video","Failed to find video file in expansion file");
						m_filename = "";
						return;
					}
					newplayer.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(), afd.getLength());
					afd.close();
					*/
                    break;
                }
                case 3:
                {
                    // streaming
                    newplayer.setDataSource(m_filename);
                    break;
                }
                default:
                {
                    Log.e("Video","Unrecognised file type");
                    return;
                }
            }

            if ( mode == 0 ) newplayer.setDisplay(pHolder);
            else
            {
                if ( Build.VERSION.SDK_INT >= 14 )
                {
                    if ( pTexture != null ) newplayer.setSurface(new Surface(pTexture));
                    else if ( pSurface != null ) newplayer.setSurface( pSurface );
                }
                else
                {
                    Log.e("Video", "Playing video to texture is not supported on this device");
                    AGKHelper.ShowMessage(act,"Playing video to texture is not supported on this device");
                }
            }

            try
            {
                newplayer.prepare();
            }
            catch( Exception e )
            {
                Log.e("Prepare Exception",e.toString());
            }
            if ( pausePos >= 0 ) newplayer.seekTo(pausePos);
            pausePos = -1;
            newplayer.start();
            if ( paused == 1 ) newplayer.pause();

            player = newplayer;

            float log1=(float)(Math.log(100-PlayVideoSDK.g_fVideoVolume)/Math.log(100));
            player.setVolume( 1-log1, 1-log1 );
        }
        catch(Exception e)
        {
            Log.e("Exception", e.toString() );
            StackTraceElement[] elements = e.getStackTrace();
            for ( int i = 0; i < elements.length; i++ )
                Log.e("Exception", elements[i].toString() );
            m_filename = "Error";
        }
    }

    public void surfaceCreated(SurfaceHolder holder)
    {
        Log.i("Video","surface created");
        pHolder = holder;
        if ( player == null )
        {
            setupPlayer(0);
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder)
    {
        Log.i("Video","surface destroyed");
        pHolder = null;

        if ( player != null )
        {
            if ( completed == 0 ) pausePos = player.getCurrentPosition();
            else pausePos = -1;
            synchronized( PlayVideoSDK.videoLock )
            {
                player.reset();
                player.release();
                player = null;
            }
        }
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        Log.i("Video", "Surface changed");
    }

    @Override
    public void onAttachedToWindow()
    {
        viewAdded = 1;
        super.onAttachedToWindow();

        if ( shouldRemoveView == 1 )
        {
            shouldRemoveView = 0;
            WindowManager wm = (WindowManager) act.getSystemService(Context.WINDOW_SERVICE);
            wm.removeView(this);
            isDisplayed = 0;
            viewAdded = 0;
        }
    }

    @Override
    public void onDetachedFromWindow()
    {
        shouldRemoveView = 0;
        super.onDetachedFromWindow();
    }

    public void onCompletion(MediaPlayer mp)
    {
        Log.i("Video","Completed");
        PlayVideoSDK.hasStartedVideo = 0;
        completed = 1;
        StopVideo();
    }

    public void OnContextLost()
    {
        synchronized( PlayVideoSDK.videoLock ) {
            if (pTexture != null) {
                pTexture = null;
            }
            iLastTex = 0;

            if ( pSurface != null ) pSurface = null;

            if (player != null) {
                if (completed == 0) pausePos = player.getCurrentPosition();
                else pausePos = -1;
                player.reset();
                player.release();
                player = null;
            }
        }
    }
}

class RunnableVideo implements Runnable
{
    public Activity act;
    public static volatile AGKSurfaceView video = null;
    public int action = 0;

    public static int videoWidth = 0;
    public static int videoHeight = 0;
    public static int videoDuration = 0;
    public static int videoPosition = 0;

    public String filename = "";
    public int fileType = 0;
    public int m_x,m_y,m_width,m_height;
    public int tex = 0;
    public Surface surface = null;
    public float pos = 0;

    public void run()
    {
        if ( video == null ) video = new AGKSurfaceView(act);

        switch( action )
        {
            case 1: // Load video
            {
                video.LoadVideo(filename, fileType);
                break;
            }
            case 2: // set dimensions
            {
                video.SetDimensions(m_x, m_y, m_width, m_height);
                break;
            }
            case 3: // play
            {
                video.PlayVideo();
                break;
            }
            case 4: // stop
            {
                video.StopVideo();
                break;
            }
            case 5: // pause
            {
                video.PauseVideo();
                break;
            }
            case 6: // delete
            {
                //video.StopVideo();
                video.DeleteVideo();
                video = null;
                break;
            }
            case 7: // play to texture (OpenGL ES)
            {
                video.PlayVideoToTexture(tex);
                break;
            }
            case 8: // context lost
            {
                video.OnContextLost();
                break;
            }
            case 9: // set position
            {
                video.SetPosition(pos);
                break;
            }
            case 10: // play video to surface (Vulkan)
            {
                video.PlayVideoToSurface(surface);
                break;
            }
        }
    }
}

public class PlayVideoSDK
{
    public static void OnStop( Activity act )
    {
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.action = 8; // OnContextLost
        act.runOnUiThread(video);
    }

    public static int hasStartedVideo = 0;
    public static int videoLoaded = 0;
    public static final Object videoLock = new Object();
    public static float g_fVideoVolume = 100;

    public static void LoadVideo( Activity act, String filename, int type )
    {
        RunnableVideo.videoDuration = 0;
        RunnableVideo.videoPosition = 0;
        RunnableVideo.videoWidth = 0;
        RunnableVideo.videoHeight = 0;

        Log.i("Video","Load Video");
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.action = 1;
        video.filename = filename;
        video.fileType = type;
        act.runOnUiThread(video);
        videoLoaded = 1;
    }

    public static void SetVideoDimensions( Activity act, int x, int y, int width, int height )
    {
        Log.i("Video","Set Dimensions");
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.action = 2;
        video.m_x = x;
        video.m_y = y;
        video.m_width = width;
        video.m_height = height;
        act.runOnUiThread(video);
    }

    public static void PlayVideo( Activity act )
    {
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.action = 3;
        act.runOnUiThread(video);

        hasStartedVideo = 1;
    }

    public static void PlayVideoToTexture( Activity act, int tex )
    {
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.tex = tex;
        video.action = 7;
        act.runOnUiThread(video);

        hasStartedVideo = 1;
    }

    public static void PlayVideoToSurface( Activity act, Surface surface )
    {
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.surface = surface;
        video.action = 10;
        act.runOnUiThread(video);

        hasStartedVideo = 1;
    }

    public static void PauseVideo( Activity act )
    {
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.action = 5;
        act.runOnUiThread(video);

        hasStartedVideo = 0;
    }

    public static void StopVideo( Activity act )
    {
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.action = 4;
        act.runOnUiThread(video);

        hasStartedVideo = 0;
    }

    public static void DeleteVideo( Activity act )
    {
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.action = 6;
        act.runOnUiThread(video);

        hasStartedVideo = 0;
        videoLoaded = 0;
    }

    public static void UpdateVideo()
    {
        // call this on the current thread as it is an OpenGL call
        if ( RunnableVideo.video == null ) return;
        RunnableVideo.video.UpdateVideo();
    }

    public static int GetVideoPlaying( Activity act )
    {
        return hasStartedVideo;
    }

    public static float GetVideoValue( Activity act, int value )
    {
        if ( videoLoaded == 0 ) return -1;

        switch(value)
        {
            case 1: return RunnableVideo.videoPosition/1000.0f; // video position
            case 2: return RunnableVideo.videoDuration/1000.0f; // video duration
            case 3: return RunnableVideo.videoWidth; // video width
            case 4: return RunnableVideo.videoHeight; // video height
        }

        return 0;
    }

    public static float GetVideoTextureValue( Activity act, int value )
    {
        try
        {
            if ( RunnableVideo.video == null ) return 0;
            if ( RunnableVideo.video.m_filename.equals("Error") ) return 0;
            if ( RunnableVideo.video.m_filename.equals("") ) return 0;

            switch(value) {
                case 1:
                    return RunnableVideo.video.U1;
                case 2:
                    return RunnableVideo.video.V1;
                case 3:
                    return RunnableVideo.video.U2;
                case 4:
                    return RunnableVideo.video.V2;
            }
        }
        catch( Exception e )
        {
            Log.e( "GetVideoTextureValue", "Caught Exception: " + e.toString() );
            return 0;
        }

        return 0;
    }

    public static void SetVideoVolume( float volume )
    {
        g_fVideoVolume = volume;
        if ( g_fVideoVolume > 99 ) g_fVideoVolume = 99;
        if ( g_fVideoVolume < 0 ) g_fVideoVolume = 0;

        synchronized( videoLock )
        {
            if (RunnableVideo.video == null || RunnableVideo.video.player == null) return;

            float log1 = (float) (Math.log(100 - g_fVideoVolume) / Math.log(100));
            RunnableVideo.video.player.setVolume(1 - log1, 1 - log1);
        }
    }

    public static void SetVideoPosition( Activity act, float position )
    {
        RunnableVideo video = new RunnableVideo();
        video.act = act;
        video.pos = position;
        video.action = 9;
        act.runOnUiThread(video);
    }
}