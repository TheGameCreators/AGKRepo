package com.thegamecreators.agk_player;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import com.thegamecreators.agk_native.R;


public class TestActivity extends Activity
{
    @Override
    public void onCreate( Bundle bundle )
    {
        super.onCreate( bundle );
        setContentView( R.layout.test_activity );
    }

    public void switchToAGK( View view )
    {
        //Toast.makeText( this, "Button Tapped", Toast.LENGTH_SHORT );
        Intent intent = new Intent( this, AGKActivity.class );
        this.startActivity( intent );
    }
}
