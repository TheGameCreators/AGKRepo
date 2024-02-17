package com.thegamecreators.agk_player;

import static com.thegamecreators.agk_player.AGKHelper.g_pAct;

import android.app.Activity;
import android.content.Context;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;


class MyTextActionWatcher implements TextView.OnEditorActionListener, TextWatcher
{
    //OnEditorActionListener

    @Override
    public boolean onEditorAction(TextView arg0, int arg1, KeyEvent arg2) {
        //String out = String.format("Action: %d, Event: %s", arg1, arg2 == null ? "" : arg2.toString());
        //Log.e("TextView",out);
        TextEntrySDK.mTextFinished = true;
        return true;
    }

    // TextWatcher

    @Override
    public void afterTextChanged( Editable s )
    {
        //Log.e("EditText", AGKHelper.mTextInput.getText().toString());
    }

    @Override
    public void beforeTextChanged(CharSequence arg0, int arg1, int arg2, int arg3) {}

    @Override
    public void onTextChanged(CharSequence arg0, int arg1, int arg2, int arg3) {}
}

class RunnableKeyboard implements Runnable
{
    public Activity act;
    public int action = 0;
    public String text = "";
    public int multiline = 0;
    public int inputType = 0; //0=normal, 1=numbers
    public int cursorpos = 0;

    public void run() {
        switch( action )
        {
            case 1: // start text input
            {
                TextEntrySDK.mTextInput = new EditText(act);
                TextEntrySDK.mTextInput.setSingleLine(multiline == 0);
                if ( inputType==1 ) TextEntrySDK.mTextInput.setInputType( InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL | InputType.TYPE_NUMBER_FLAG_SIGNED );
                else TextEntrySDK.mTextInput.setInputType( InputType.TYPE_CLASS_TEXT );

                FrameLayout.LayoutParams mEditTextLayoutParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT);
                mEditTextLayoutParams.gravity = Gravity.TOP;
                mEditTextLayoutParams.setMargins(0, 0, 0, 0);
                TextEntrySDK.mTextInput.setLayoutParams(mEditTextLayoutParams);
                TextEntrySDK.mTextInput.setVisibility(View.VISIBLE);
                TextEntrySDK.mTextInput.addTextChangedListener(TextEntrySDK.textActionWatcher);
                TextEntrySDK.mTextInput.setOnEditorActionListener(TextEntrySDK.textActionWatcher);

                act.addContentView(TextEntrySDK.mTextInput, mEditTextLayoutParams);

                TextEntrySDK.mTextInput.bringToFront();
                if ( cursorpos >= 0 ) TextEntrySDK.mTextInput.setSelection(cursorpos);
                TextEntrySDK.mTextInput.requestFocus();

                TextEntrySDK.mTextFinished = false;

                InputMethodManager m = (InputMethodManager) act.getSystemService(Context.INPUT_METHOD_SERVICE);
                m.showSoftInput(TextEntrySDK.mTextInput, 0);
                break;
            }
            case 2: // stop text input
            {
                InputMethodManager lInputMethodManager = (InputMethodManager)act.getSystemService(Context.INPUT_METHOD_SERVICE);
                lInputMethodManager.hideSoftInputFromWindow(act.getWindow().getDecorView().getWindowToken(), 0);

                if ( TextEntrySDK.mTextInput != null ) {
                    ((ViewGroup) (TextEntrySDK.mTextInput.getParent())).removeView(TextEntrySDK.mTextInput);
                    TextEntrySDK.mTextInput = null;
                }
                TextEntrySDK.mTextHiding = false;
                break;
            }
            case 3: // position text cursor
            {
                if ( TextEntrySDK.mTextInput != null )
                {
                    TextEntrySDK.mTextInput.setText(text);
                    if ( cursorpos >= 0 ) TextEntrySDK.mTextInput.setSelection(cursorpos);
                }
                break;
            }
            case 4: // show keyboard for existing text input
            {
                if ( TextEntrySDK.mTextInput != null )
                {
                    TextEntrySDK.mTextInput.setSingleLine(multiline == 0);
                    if ( inputType==1 ) TextEntrySDK.mTextInput.setInputType( InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL | InputType.TYPE_NUMBER_FLAG_SIGNED );
                    else TextEntrySDK.mTextInput.setInputType( InputType.TYPE_CLASS_TEXT );
                    TextEntrySDK.mTextFinished = false;
                    if ( cursorpos >= 0 ) TextEntrySDK.mTextInput.setSelection(cursorpos);
                    TextEntrySDK.mTextInput.requestFocus();
                    InputMethodManager lInputMethodManager = (InputMethodManager)act.getSystemService(Context.INPUT_METHOD_SERVICE);
                    lInputMethodManager.showSoftInput(TextEntrySDK.mTextInput, 0);
                }
                break;
            }
            case 5: // position text cursor only
            {
                if ( TextEntrySDK.mTextInput != null )
                {
                    if ( cursorpos >= 0 )
                    {
                        try {
                            TextEntrySDK.mTextInput.setSelection(cursorpos);
                        }
                        catch( IndexOutOfBoundsException e )
                        {
                            Log.w("Keyboard", "SetCursor index out of bounds: " + cursorpos);
                        }
                    }
                }
                break;
            }
        }
    }
}

public class TextEntrySDK
{
    // Edit box input
    static EditText mTextInput = null;
    static boolean mTextFinished = false;
    static boolean mTextHiding = false;
    static MyTextActionWatcher textActionWatcher = new MyTextActionWatcher();

    public static int GetInputFinished( Activity act )
    {
        return mTextFinished ? 1 : 0;
    }

    public static int GetInputCursor( Activity act )
    {
        if ( mTextInput == null ) return 0;
        return mTextInput.getSelectionStart();
    }

    public static String GetInputText( Activity act )
    {
        if ( mTextInput == null ) return "";
        try
        {
            return mTextInput.getText().toString();
        }
        catch( Exception e )
        {
            return "";
        }
    }

    public static void SetInputText( Activity act, String text, int cursorpos )
    {
        //if ( mTextInput == null ) return;

        RunnableKeyboard run = new RunnableKeyboard();
        run.act = act;
        run.action = 3;
        run.text = text;
        run.cursorpos = cursorpos;
        act.runOnUiThread( run );
    }

    public static void SetInputTextCursor( Activity act, int cursorpos )
    {
        //if ( mTextInput == null ) return;

        RunnableKeyboard run = new RunnableKeyboard();
        run.act = act;
        run.action = 5;
        run.cursorpos = cursorpos;
        act.runOnUiThread( run );
    }

    public static void ShowKeyboard( Activity act, int multiline, int inputType )
    {
        //InputMethodManager lInputMethodManager = (InputMethodManager)act.getSystemService(Context.INPUT_METHOD_SERVICE);
        //lInputMethodManager.showSoftInput( act.getWindow().getDecorView(), 0 );

        mTextFinished = false;

        if ( mTextInput != null && !mTextHiding )
        {
            RunnableKeyboard run = new RunnableKeyboard();
            run.act = act;
            run.action = 4;
            run.multiline = multiline;
            run.inputType = inputType;
            run.cursorpos = -1;
            act.runOnUiThread( run );
            return;
        }

        mTextHiding = false;

        RunnableKeyboard run = new RunnableKeyboard();
        run.act = act;
        run.action = 1;
        run.multiline = multiline;
        run.inputType = inputType;
        run.cursorpos = -1;
        act.runOnUiThread( run );
    }

    public static void HideKeyboard( Activity act )
    {
        mTextFinished = true;
        mTextHiding = true;

        if ( mTextInput == null )
        {
            InputMethodManager lInputMethodManager = (InputMethodManager)act.getSystemService(Context.INPUT_METHOD_SERVICE);
            lInputMethodManager.hideSoftInputFromWindow( act.getWindow().getDecorView().getWindowToken(), 0 );
            return;
        }

        RunnableKeyboard run = new RunnableKeyboard();
        run.act = act;
        run.action = 2;
        act.runOnUiThread( run );

        ExternalCommands.ExternalCommand( act,"immersive", "refresh", null, null );
    }
}