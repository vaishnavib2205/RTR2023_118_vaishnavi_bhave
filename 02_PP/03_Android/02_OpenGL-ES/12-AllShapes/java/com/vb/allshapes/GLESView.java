package com.vb.allshapes;

import android.content.Context;

// OpenGLES releted packages

import android.opengl.GLSurfaceView;
import android.opengl.GLES32;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

// pacakge related to event

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

// java i/o packages
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.Matrix;



public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer,OnDoubleTapListener,OnGestureListener
{
	private GestureDetector gestureDetector;
	
	float RADIUS = 1.0f;
	float x = 0.0f;
	float y = 0.0f;
	float Radian = 0.0f;

	int i = 0;
	float circle_position[] = new float[1080];
	float circle_color[] = new float[1080];
	
	private int shaderProgramObject;
	private int[] vao_square = new int[1];
	private int[] vbo_square_position = new int[1];
	private int[] vbo_square_color = new int[1];

	private int[] vao_triangle = new int[1];
	private int[] vbo_triangle_position = new int[1];
	private int[] vbo_triangle_color = new int[1];

	private int[] vao_circle = new int[1];
	private int[] vbo_circle_position = new int[1];
	private int[] vbo_circle_color = new int[1];

	private int[] vao_blueV = new int[1];
	private int[] vbo_blueV_position = new int[1];
	private int[] vbo_blueV_color = new int[1];

	private int[] vao_blueH = new int[1];
	private int[] vbo_blueH_position = new int[1];
	private int[] vbo_blueH_color = new int[1];

	private int[] vao_red = new int[1];
	private int[] vbo_red_position = new int[1];
	private int[] vbo_red_color = new int[1];

	private int[] vao_green = new int[1];
	private int[] vbo_green_position = new int[1];
	private int[] vbo_green_color = new int[1];

	private int mvpMatrixUniform;
	private float[] perspectiveProjectionMatrix = new float[16];

	public GLESView(Context context)
	{
		super(context);
			
		// OpenGLES Related
		setEGLContextClientVersion(3);
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		gestureDetector = new GestureDetector(context,this,null,false);
		gestureDetector.setOnDoubleTapListener(this);		
	}
	// Implementation of 3 methods of GLSurfaceView
	@Override
	public void onSurfaceCreated(GL10 gl,EGLConfig config){
		// Code
		initialize(gl);
	}
	
	@Override
	public void onSurfaceChanged(GL10 gl,int width,int height){
		// Code
		resize(width,height);
	}
	
	@Override
	public void onDrawFrame(GL10 gl){
		// Code
		display();
	}

	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		if(!gestureDetector.onTouchEvent(e))
		{
			super.onTouchEvent(e);
		}
		return(true);
	}

	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		return(true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return(true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		return(true);

	}

	@Override
	public boolean onDown(MotionEvent e)
	{
		return(true);
	}
	
	@Override
	public boolean onFling(MotionEvent e1,MotionEvent e2,float velocityX,float velocityY)
	{
		return(true);
	}

	@Override
	public void onLongPress(MotionEvent e)
	{
	}

	@Override
	public boolean onScroll(MotionEvent e1,MotionEvent e2,float distanceX,float distanceY)
	{
		uninitialize();

		System.exit(0);
		return(true);
	}

	@Override
	public void onShowPress(MotionEvent e)
	{
	
	}
	
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return(true);
		
	}
	

	// Implementation of private method
	private void initialize(GL10 gl)
	{
		// variable declaration


		// Code
		// Print info
		printGLInfo(gl);

		// Vertex Shader
		final String vertexShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"in vec4 aPosition;"+
			"in vec4 aColor;"+
			"out vec4 oColor;"+
			"uniform mat4 uMVPMatrix;"+
			"void main(void)"+
			"{"+
			"gl_Position = uMVPMatrix * aPosition;"+
			"oColor = aColor;"+
			"}"
		);

		int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		GLES32.glShaderSource(vertexShaderObject,vertexShaderSourceCode);
		GLES32.glCompileShader(vertexShaderObject);

		int[] status = new int[1];
		int[] infoLogLength = new int[1];
		String szInfoLog = null;

		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, status, 0);
		if(status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if(infoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("Vertex Shader compilation error log :" + szInfoLog);
			
				uninitialize();
				System.exit(0);
			}

		}


		// Fragment Shader
		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"out vec4 fragColor;"+
			"in vec4 oColor;"+
			"void main(void)"+
			"{"+
			"fragColor = oColor;"+
			"}"
		);

		int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		GLES32.glShaderSource(fragmentShaderObject,fragmentShaderSourceCode);
		GLES32.glCompileShader(fragmentShaderObject);

		status[0] = 0;
		infoLogLength[0] = 0;
		szInfoLog = null;

		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, status, 0);
		if(status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if(infoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("Fragment Shader compilation error log :" + szInfoLog);
			
				uninitialize();
				System.exit(0);
			}

		}

		// Shader Program
		shaderProgramObject = GLES32.glCreateProgram();
	
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
	
		GLES32.glBindAttribLocation(shaderProgramObject, vertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, vertexAttributesEnum.AMC_ATTRIBUTE_COLOR, "aColor");
	
		GLES32.glLinkProgram(shaderProgramObject);

		int[] linkStatus = new int[1];
		infoLogLength[0] = 0;
		szInfoLog = null;

		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, linkStatus, 0);
		if(linkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if(infoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("Shader program object linking error log :" + szInfoLog);
			
				uninitialize();
				System.exit(0);
			}

		}

		mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

		final float[] rectangle_position = {
		1.0f,1.0f,0.0f,
		-1.0f,1.0f,0.0f,

		-1.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		
		1.0f,-1.0f,0.0f,
		1.0f,1.0f,0.0f
		};

		final float[] rectangle_color = {
			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f,

			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f,
			
			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f,
			
			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f
		};

		final float[] triangle_position = {
		
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		
		1.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f
		
		};

		final float[] triangle_color = {
			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f,

			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f,
			
			1.0f,1.0f,0.0f,
			1.0f,1.0f,0.0f
		};
		
	
	for (int fAngle = 0; fAngle < 360; fAngle++)
	{
		Radian = (float)fAngle * ((float)Math.PI / 180.0f);
	
		x = RADIUS * (float)Math.cos(Radian);
		y = RADIUS * (float)Math.sin(Radian);
	
		circle_position[i] = x;
		circle_position[i + 1] = y;
		circle_position[i + 2] = 0.0f;
	

		i = i + 3;
		
	}

		
	for(int j = 0;j < 1080;j = j + 3)
	{
		circle_color[j] = 1.0f;
		circle_color[j + 1] = 1.0f;
		circle_color[j + 2] = 0.0f;
	}

	final float[] blueV_position = {
			0.0f,10.0f,0.0f,
			0.0f,-10.0f,0.0f
		};
		
		final float[] blueH_position = {
			10.0f,0.0f,0.0f,
			-10.0f,0.0f,0.0f
		};
		final float[] blue_color = {
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f
		};
		
		final float[] red_position = {
			0.0f,10.0f,0.0f,
			0.0f,-10.0f,0.0f
		};
		final float[] red_color = {
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f
		};
		
		final float[] green_position = {
			10.0f,0.0f,0.0f,
			-10.0f,0.0f,0.0f
		};
		final float[] green_color = {
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f
		};

		initializeShape(vao_square, vbo_square_position, vbo_square_color, rectangle_position, rectangle_color);
		initializeShape(vao_triangle, vbo_triangle_position, vbo_triangle_color, triangle_position, triangle_color);
		initializeShape(vao_circle, vbo_circle_position, vbo_circle_color, circle_position, circle_color);

		initializeShape(vao_blueV, vbo_blueV_position, vbo_blueV_color, blueV_position, blue_color);
		initializeShape(vao_blueH, vbo_blueH_position, vbo_blueH_color, blueH_position, blue_color);
		initializeShape(vao_red, vbo_red_position, vbo_red_color, red_position, red_color);
		initializeShape(vao_green, vbo_green_position, vbo_green_color, green_position, green_color);
		
		Matrix.setIdentityM(perspectiveProjectionMatrix,0);

		// Depth enable
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// Clear color
		GLES32.glClearColor(0.0f,0.0f,0.0f,1.0f);
	}
	
	private void initializeShape(int vao[],int vbo_position[], int vbo_color[], final float position[], final float color[])
	{
		
		// Vao position
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);

		// vbo_position 
		GLES32.glGenBuffers(1, vbo_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(position.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(position);
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,	 position.length * 4 , positionBuffer, GLES32.GL_STATIC_DRAW);
	
		GLES32.glVertexAttribPointer(vertexAttributesEnum.AMC_ATTRIBUTE_POSITION,
			3,
			GLES32.GL_FLOAT,
			false,
			0,
			0);
		GLES32.glEnableVertexAttribArray(vertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		// vbo_color
		GLES32.glGenBuffers(1, vbo_color, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		
		ByteBuffer cByteBuffer = ByteBuffer.allocateDirect(color.length * 4);
		cByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer colorBuffer = cByteBuffer.asFloatBuffer();
		colorBuffer.put(color);
		colorBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,	 color.length * 4 , colorBuffer, GLES32.GL_STATIC_DRAW);
	
		GLES32.glVertexAttribPointer(vertexAttributesEnum.AMC_ATTRIBUTE_COLOR,
			3,
			GLES32.GL_FLOAT,
			false,
			0,
			0);
		GLES32.glEnableVertexAttribArray(vertexAttributesEnum.AMC_ATTRIBUTE_COLOR);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


		// Unbind vertex array
		GLES32.glBindVertexArray(0);
	}
	private void printGLInfo(GL10 gl)
	{
		// Code
		System.out.println("openGL-ES renderer : " + gl.glGetString(GL10.GL_RENDERER)); // Java ch printf
		System.out.println("openGL-ES Version : " + gl.glGetString(GL10.GL_VERSION));
		System.out.println("openGL-ES Shading Language Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));

	}
	private void resize(int width,int height)
	{
		// Code
		if(height <= 0)
			height = 1;

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.0f, (float)width / (float)height, 0.1f, 100.0f); 
	}
	private void display(){
		// Code
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		// Render
		// Transformation
		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];

		for (float i = 0.1f; i < 4.0f; i += 0.1)
		{
			Matrix.setIdentityM(modelViewMatrix,0);
			Matrix.setIdentityM(modelViewProjectionMatrix,0);

			Matrix.translateM(modelViewMatrix, 0, i, 0.0f, -6.0f);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

			GLES32.glBindVertexArray(vao_blueV[0]);
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
			GLES32.glBindVertexArray(0);
		}
		for (float i = -0.1f; i > -4.0f; i -= 0.1)
		{
			Matrix.setIdentityM(modelViewMatrix,0);
			Matrix.setIdentityM(modelViewProjectionMatrix,0);

			Matrix.translateM(modelViewMatrix, 0, i, 0.0f, -6.0f);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

			GLES32.glBindVertexArray(vao_blueV[0]);
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
			GLES32.glBindVertexArray(0);
		}

		for (float i = -0.1f; i > -4.0f; i -= 0.1)
		{
			Matrix.setIdentityM(modelViewMatrix,0);
			Matrix.setIdentityM(modelViewProjectionMatrix,0);

			Matrix.translateM(modelViewMatrix, 0, 0.0f, i, -6.0f);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

			GLES32.glBindVertexArray(vao_blueH[0]);
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
			GLES32.glBindVertexArray(0);
		}
		for (float i = 0.1f; i < 4.0f; i += 0.1)
		{
			Matrix.setIdentityM(modelViewMatrix,0);
			Matrix.setIdentityM(modelViewProjectionMatrix,0);

			Matrix.translateM(modelViewMatrix, 0, 0.0f, i, -6.0f);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

			GLES32.glBindVertexArray(vao_blueH[0]);
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
			GLES32.glBindVertexArray(0);
		}

			Matrix.setIdentityM(modelViewMatrix,0);
			Matrix.setIdentityM(modelViewProjectionMatrix,0);

			Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

			GLES32.glBindVertexArray(vao_red[0]);
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
			GLES32.glBindVertexArray(0);
			Matrix.setIdentityM(modelViewMatrix,0);
			Matrix.setIdentityM(modelViewProjectionMatrix,0);

			Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

			GLES32.glBindVertexArray(vao_green[0]);
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
			GLES32.glBindVertexArray(0);
		// ****************************************************************************************************
		// ****************************************************************************************************
		// ****************************************************************************************************

		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);

		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

		GLES32.glBindVertexArray(vao_square[0]);
		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 8);
		GLES32.glBindVertexArray(0);

		// ****************************************************************************************************
		// ****************************************************************************************************
		// ****************************************************************************************************

		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);

		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

		GLES32.glBindVertexArray(vao_triangle[0]);
		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 6);
		GLES32.glBindVertexArray(0);


		// ****************************************************************************************************
		// ****************************************************************************************************
		// ****************************************************************************************************

		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		
		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
		
		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);
		
		GLES32.glBindVertexArray(vao_circle[0]);
		
		
		for (int i = 0; i <= 358; i+=2) {
			GLES32.glDrawArrays(GLES32.GL_LINE_STRIP, i, 4);
		}
		GLES32.glBindVertexArray(0);

		// ****************************************************************************************************
		// ****************************************************************************************************
		// ****************************************************************************************************
		
		GLES32.glUseProgram(0);	

		requestRender(); // Android ch swapbuffer
	}
	
	private void uninitialize(){
				
		// Code

		if(shaderProgramObject > 0)
		{
			GLES32.glUseProgram(shaderProgramObject);
			int[] retVal = new int[1];
			
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, retVal, 0);
			
			if(retVal[0] > 0)
			{
				int numShaders = retVal[0];
				int[] shaderObjects = new int[numShaders];
				GLES32.glGetAttachedShaders(shaderProgramObject, numShaders, retVal, 0, shaderObjects, 0);	
			
				for(int i = 0; i<numShaders;i++){
					GLES32.glDetachShader(shaderProgramObject, shaderObjects[i]);
					GLES32.glDeleteShader(shaderObjects[i]);
					shaderObjects[i] = 0;
				}
			}
			GLES32.glUseProgram(0);
			GLES32.glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
		}
		if(vbo_triangle_position[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_triangle_position, 0);
			vbo_triangle_position[0] = 0;
		}
		if(vbo_triangle_color[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_triangle_color, 0);
			vbo_triangle_color[0] = 0;
		}

		if(vao_triangle[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_triangle, 0);
			vao_triangle[0] = 0;
		}
		if(vbo_square_position[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_square_position, 0);
			vbo_square_position[0] = 0;
		}
		if(vbo_square_color[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_square_color, 0);
			vbo_square_color[0] = 0;
		}

		if(vao_square[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_square, 0);
			vao_square[0] = 0;
		}
	}

}

