package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.os.Build;
import android.speech.tts.TextToSpeech;
import android.speech.tts.UtteranceProgressListener;
import android.util.Log;

import java.util.HashMap;
import java.util.List;
import java.util.Locale;

class AGKSpeechListener extends UtteranceProgressListener implements TextToSpeech.OnInitListener
{
    @Override
    public void onInit(int status)
    {
        if ( status == TextToSpeech.SUCCESS )
        {
            if ( Build.VERSION.SDK_INT >= 21 )
            {
                try {
                    if (TextToSpeechSDK.g_pTextToSpeech.getAvailableLanguages() != null) {
                        TextToSpeechSDK.g_SpeechLanguages = TextToSpeechSDK.g_pTextToSpeech.getAvailableLanguages().toArray();
                    }
                }
                catch( Exception e ) { Log.w("TextToSpeech", "Failed to get available languages"); }
            }
            TextToSpeechSDK.g_iSpeechReady = 1;
        }
        else TextToSpeechSDK.g_iSpeechReady = -1;
    }

    @Override
    public void onStart(String utteranceId)
    {

    }

    @Override
    public void onDone(String utteranceId)
    {
        if ( utteranceId.equals(Integer.toString(TextToSpeechSDK.g_iSpeechIDLast)))
        {
            TextToSpeechSDK.g_iIsSpeaking = 0;
        }
    }

    @Override
    public void onError(String utteranceId)
    {
        Log.w( "TTS", "Failed to speak text" );
        if ( utteranceId.equals(Integer.toString(TextToSpeechSDK.g_iSpeechIDLast)))
        {
            TextToSpeechSDK.g_iIsSpeaking = 0;
        }
    }
}

public class TextToSpeechSDK
{
    public static TextToSpeech g_pTextToSpeech = null;
    public static AGKSpeechListener g_pSpeechListener = null;
    public static int g_iSpeechReady = 0;
    public static int g_iIsSpeaking = 0;
    public static int g_iSpeechIDLast = 0;
    static Object[] g_SpeechLanguages = null;

    public static void OnStop( Activity act )
    {
        StopSpeaking();
    }

    public static void TextToSpeechSetup( Activity act )
    {
        if ( g_pTextToSpeech != null ) return;

        g_pSpeechListener = new AGKSpeechListener();
        if ( LocalApp.g_sTextToSpeechEngine != null ) g_pTextToSpeech = new TextToSpeech( act, g_pSpeechListener, LocalApp.g_sTextToSpeechEngine );
        else g_pTextToSpeech = new TextToSpeech( act, g_pSpeechListener );
        g_pTextToSpeech.setOnUtteranceProgressListener(g_pSpeechListener);

        List<TextToSpeech.EngineInfo> engines = g_pTextToSpeech.getEngines();
        for ( TextToSpeech.EngineInfo engine: engines )
        {
            Log.i( "TextToSpeech", "Engine: " + engine.name );
        }

        Log.i( "TextToSpeech", "Default Engine: " + g_pTextToSpeech.getDefaultEngine() );
    }

    public static int GetTextToSpeechReady()
    {
        return g_iSpeechReady;
    }

    public static void Speak( Activity act, String text, int mode, int delay )
    {
        if ( g_pTextToSpeech == null ) return;
        if ( g_iSpeechReady == 0 )
        {
            Log.e("Speech","Failed to speak, speech engine not yet ready");
            return;
        }
        if ( g_iSpeechReady < 0 )
        {
            Log.e("Speech","Failed to speak, speech engine failed to initialise");
            return;
        }

        g_iIsSpeaking = 1;

        int queueMode = TextToSpeech.QUEUE_ADD;
        if ( mode > 0 ) queueMode = TextToSpeech.QUEUE_FLUSH;

        try
        {
            if ( delay > 0 )
            {
                g_iSpeechIDLast++;
                if ( g_iSpeechIDLast > 1000000000 ) g_iSpeechIDLast = 0;
                if ( Build.VERSION.SDK_INT >= 21 )
                {
                    g_pTextToSpeech.playSilentUtterance( delay, queueMode, Integer.toString(g_iSpeechIDLast) );
                }
                else
                {
                    HashMap<String,String> hashMap = new HashMap();
                    hashMap.put(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, Integer.toString(g_iSpeechIDLast) );
                    g_pTextToSpeech.playSilence( delay, queueMode, hashMap );
                }
            }
        }
        catch( IllegalArgumentException e ) {}

        g_iSpeechIDLast++;
        if ( g_iSpeechIDLast > 1000000000 ) g_iSpeechIDLast = 0;

        try
        {
            if ( Build.VERSION.SDK_INT >= 21 )
            {
                if ( g_pTextToSpeech.speak(text, queueMode, null, Integer.toString(g_iSpeechIDLast)) < 0) {
                    Log.e("TextToSpeech", "Failed to queue speech");
                }
            }
            else
            {
                HashMap<String,String> hashMap = new HashMap();
                hashMap.put(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, Integer.toString(g_iSpeechIDLast) );
                if (g_pTextToSpeech.speak(text, queueMode, hashMap) < 0) {
                    Log.e("TextToSpeech", "Failed to queue speech");
                }
            }
        }
        catch( IllegalArgumentException e ) {}
    }

    public static int GetSpeechNumVoices( Activity act )
    {
        if ( g_SpeechLanguages == null ) return 0;
        return g_SpeechLanguages.length;
    }

    public static String GetSpeechVoiceLanguage( Activity act, int index )
    {
        if ( g_SpeechLanguages == null ) return "";
        if ( index < 0 || index >= g_SpeechLanguages.length ) return "";

        Locale locale = (Locale)g_SpeechLanguages[ index ];
        return locale.toString();
    }

    public static String GetSpeechVoiceName( Activity act, int index )
    {
        if ( g_SpeechLanguages == null ) return "";
        if ( index < 0 || index >= g_SpeechLanguages.length ) return "";

        Locale locale = (Locale)g_SpeechLanguages[ index ];
        return locale.getDisplayName();
    }

    public static void SetSpeechLanguage( Activity act, String lang )
    {
        if ( g_pTextToSpeech == null ) return;
        String[] parts = lang.split("_");
        try {
            if (parts.length <= 1) g_pTextToSpeech.setLanguage(new Locale(lang));
            else g_pTextToSpeech.setLanguage(new Locale(parts[0], parts[1]));
        }
        catch( IllegalArgumentException e ) {}
    }

    public static void SetSpeechLanguageByID( Activity act, String sID )
    {
        if ( g_SpeechLanguages == null ) return;

        int index = 0;
        try {
            index = Integer.parseInt(sID);
        }
        catch( NumberFormatException e )
        {
            Log.e( "SetSpeechLanguageByID", "Invalid language ID: " + sID );
            return;
        }
        if ( index < 0 || index >= g_SpeechLanguages.length ) return;

        Locale locale = (Locale)g_SpeechLanguages[ index ];
        try { g_pTextToSpeech.setLanguage( locale ); }
        catch( IllegalArgumentException e ) {}
    }

    public static void SetSpeechRate( Activity act, float rate )
    {
        if ( g_pTextToSpeech == null ) return;
        try { g_pTextToSpeech.setSpeechRate( rate ); }
        catch( IllegalArgumentException e ) {}
    }

    public static int IsSpeaking()
    {
        if ( g_pTextToSpeech == null ) return 0;
        return g_iIsSpeaking;
    }

    public static void StopSpeaking()
    {
        if ( g_pTextToSpeech == null ) return;
        if ( g_iIsSpeaking == 0 ) return;
        try { g_pTextToSpeech.stop(); }
        catch( IllegalArgumentException e ) {}
        g_iIsSpeaking = 0;
    }
}