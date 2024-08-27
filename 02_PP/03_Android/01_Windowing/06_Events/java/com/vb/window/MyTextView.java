package com.vb.window;

import android.content.Context;

import androidx.appcompat.widget.AppCompatTextView;
import android.graphics.Color;
import android.view.Gravity;

//event related pck
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

public class MyTextView extends AppCompatTextView implements OnDoubleTapListener, OnGestureListener
{
	private GestureDetector gestureDetector = null;
	public MyTextView(Context context)
	{
		super(context);

		setTextColor(Color.rgb(0,255,0));
        setTextSize(15);
        setGravity(Gravity.CENTER);
        setText("Hello World !!!");

		//create and set gesture detector object
		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	//implemetation of on touch event 
	@Override
    public boolean onTouchEvent(MotionEvent e)
    {
        // code
        if(!gestureDetector.onTouchEvent(e))
        {
            super.onTouchEvent(e);
        }
        return true;
	}

	//three methods of on double tap listener interfaces
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		setText("Double Tap");
		return true;
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return true;

	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		setText("Single Tap");
		return true;
	}

	//six methods of on gesture interfaces

	@Override
	public boolean onDown(MotionEvent e)
	{
		return true;
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return true;
	}


	@Override
	public void onLongPress(MotionEvent e)
	{
		setText("Long Press");

	}

	@Override 
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distnaceY)
	{
		setText("Scroll/Swipe");
		return true;
	}

	@Override
	public void onShowPress(MotionEvent e)
	{

	}

	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return true;
	}

}