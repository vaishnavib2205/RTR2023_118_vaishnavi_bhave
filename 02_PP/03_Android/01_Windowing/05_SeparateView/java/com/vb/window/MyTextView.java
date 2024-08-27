package com.vb.window;

import android.content.Context;

import androidx.appcompat.widget.AppCompatTextView;
import android.graphics.Color;
import android.view.Gravity;

public class MyTextView extends AppCompatTextView 
{
	public MyTextView(Context context)
	{
		super(context);

		setTextColor(Color.rgb(0,255,0));
        setTextSize(30);
        setGravity(Gravity.CENTER);
        setText("Hello World !!!");
	}
}