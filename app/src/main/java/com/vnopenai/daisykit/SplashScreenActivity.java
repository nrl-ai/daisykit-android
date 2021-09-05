package com.vnopenai.daisykit;

import android.app.Activity;
import android.os.Bundle;
import android.content.Intent;
import android.os.Handler;

import androidx.appcompat.app.AppCompatActivity;

public class SplashScreenActivity extends AppCompatActivity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
       super.onCreate(savedInstanceState);
        new Handler().postDelayed(new Runnable() {
           @Override
           public void run() {
            //This method will be executed once the timer is over
            // Start your app main activity
           Intent i = new Intent(SplashScreenActivity.this, MainActivity.class);
           startActivity(i);
            // close this activity
           finish();
           }
        }, 1000);
    }
}