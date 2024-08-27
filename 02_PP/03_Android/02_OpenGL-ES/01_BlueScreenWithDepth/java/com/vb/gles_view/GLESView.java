package com.vb.gles_view;

import android.content.Context;

//OpenGLES related pck
import android.opengl.GLSurfaceView;
import android.opengl.GLES32;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;


//event related pck
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener
{
	

	private GestureDetector gestureDetector = null;
	public GLESView(Context context)
	{
		super(context);

		//OpenglES related  -> equivalent to the pfd code in window
		setEGLContextClientVersion(3); 
		setRenderer(this);

		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		//event related

		//create and set gesture detector object
		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	//implementation of three methods of glsurface view interface

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		//code

		initialise(gl);

	}
		
	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		//code
		resize(width, height);
	}

	@Override
	public void onDrawFrame(GL10 gl)
	{
		//code
		display();
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

	}

	@Override 
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distnaceY)
	{
		uninitialise();

		System.exit(0);
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


	//implementation of private methods

	private void initialise(GL10 gl)
	{
		//code
		printOpenGLInfo(gl);

		//depth setting
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		//enable culling 
		GLES32.glEnable(GLES32.GL_CULL_FACE);


		GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	}

	private void printOpenGLInfo(GL10 gl)
	{
		//code
		System.out.println("VB: OpenGL_ES Renderer :"+ gl.glGetString(GL10.GL_RENDERER));
		System.out.println("VB: OpenGL_ES Version :"+ gl.glGetString(GL10.GL_VERSION));
		System.out.println("VB: OpenGL_ES Shading language Version :"+ gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
	}

	private void resize(int width, int height)
	{
		//code

		if(height <= 0)
		{
			height = 1;
		}

		GLES32.glViewport(0,0,width, height);
	}

	private void display()
	{
		//code
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		//Render
		requestRender();
	}

	private void uninitialise()
	{
		//code
	}

}