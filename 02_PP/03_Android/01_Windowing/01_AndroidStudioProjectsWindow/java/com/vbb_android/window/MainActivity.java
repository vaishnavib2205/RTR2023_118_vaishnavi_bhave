package com.vbb_android.window;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import androidx.appcompat.widget.AppCompatTextView;

import android.graphics.Color;

import android.view.Gravity;

public class MainActivity extends AppCompatActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		//setting bg color to black
		getWindow().getDecorView().setBackgroundColor(Color.BLACK);

		AppCompatTextView myTextView = new AppCompatTextView(this);
		myTextView.setTextColor(Color.rgb(0, 255, 0));
		myTextView.setGravity(Gravity.CENTER);
		myTextView.setTextSize(15);
		myTextView.setText("Hello Vaishnavi. This is your first android window!");
		setContentView(myTextView);
	}
}