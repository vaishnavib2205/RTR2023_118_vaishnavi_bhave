package com.vb.window;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

import android.graphics.Color;


// package for full screen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;

import android.content.pm.ActivityInfo;

public class MainActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // fullscreen

        // get and hide action bar
        getSupportActionBar().hide();

        // do fullscreen

        WindowCompat.setDecorFitsSystemWindows(getWindow() , false);
        // get window insets controller object

        WindowInsetsControllerCompat windowInsetsControllerCompat = WindowCompat.getInsetsController(getWindow() , getWindow().getDecorView());

        // tell above object to hode system bars, means status bar , navigation bar and caption bar and IME

        windowInsetsControllerCompat.hide(WindowInsetsCompat.Type.systemBars() | WindowInsetsCompat.Type.ime());

        // do landscape
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        // setting background color to black
        getWindow().getDecorView().setBackgroundColor(Color.BLACK);

        MyTextView myTextView = new MyTextView(this);
        

        setContentView(myTextView);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
    }

    protected void onResume()
    {
        super.onResume();
    }

}