package com.vb.gles_perspective;

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

//java I/O
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import android.opengl.Matrix;


public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener
{
	

	private GestureDetector gestureDetector = null;

	private int shaderProgramObject;
	private int[] vao = new int[1];
	private int[] vbo = new int[1];
	private int mvpMatrixUniform = 0;
	private float[] perspectiveProjectionMatrix = new float[16];



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

		//Vertex Shader

		final String vertexShaderSourceCode = String.format(
		"#version 320 es" +
		"\n" +
		"in vec4 aPosition;" +
		"uniform mat4 uMVPMatrix;" +
		"void main(void)" +
		"{" +
		"gl_Position = uMVPMatrix * aPosition;" +
		"}"
		);

		
		int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

		GLES32.glCompileShader(vertexShaderObject);

		int[] shaderCompileStatus = new int[1];
		int[] infoLogLength = new int[1];
		String infoLog = null;


		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		
		if (shaderCompileStatus[0] == GLES32.GL_FALSE) // if error
		{
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("VBB: VertexShader Compilation Error Log :" +infoLog);

				uninitialise();
				System.exit(0);

			}
		}

		//fragment Shader 

		final String fragmentShaderSourceCode = String.format(
		"#version 320 es" +
		"\n" +
		"precision highp float;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" +
		"}"
		);

		int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		GLES32.glShaderSource(fragmentShaderObject, vertexShaderSourceCode);

		GLES32.glCompileShader(fragmentShaderObject);

		shaderCompileStatus[0] = 0;
		infoLogLength[0] = 0;
		infoLog = null;


		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		
		if (shaderCompileStatus[0] == GLES32.GL_FALSE) // if error
		{
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("VBB: FragmentShader Compilation Error Log :" +infoLog);

				uninitialise();
				System.exit(0);

			}
		}


		//shaderprogram

		shaderProgramObject = GLES32.glCreateProgram();

		// attach both shader to this program
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

		//bind Attribute location with the shader program object

		GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributeEnum.AMC_ATTRIBUTE_POSITION, "aPosition");

		//link the shader program
		GLES32.glLinkProgram(shaderProgramObject);

		//do above shader compilation related steps for linking error 

		int[] programLinkStatus = new int[1];
		infoLogLength[0] = 0;
		infoLog = null;

		
		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, programLinkStatus, 0);
		
		if (programLinkStatus[0] == GLES32.GL_FALSE) // if error
		{
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("VBB: programLinkStatus Error Log :" +infoLog);

				uninitialise();
				System.exit(0);

			}
		}

		mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

		//geometry
		final float[] triangle_position = new float[]
		{
			0.0f, 1.0f, 0.0f,    // glVertex3f(0.0f, 1.0f, 0.0f)
			-1.0f, -1.0f, 0.0f,    // glVertex3f(-1.0f, -1.0f, 0.0f)
			1.0f, -1.0f, 0.0f    //glVertex3f(1.0f, -1.0f, 0.0f)
		};

		GLES32.glGenVertexArrays(1, vao, 0);

		//bind with vao
		GLES32.glBindVertexArray(vao[0]);

		// VBO for position
		//create vertex buffer array for position
		GLES32.glGenBuffers(1, vbo, 0);

		//bind with vbo of position
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);


		//prep tri vertices array for glbufferdata


		ByteBuffer bytebuffer = ByteBuffer.allocateDirect(triangle_position.length * 4);

		bytebuffer.order(ByteOrder.nativeOrder());
		FloatBuffer positionBuffer = bytebuffer.asFloatBuffer();

		positionBuffer.put(triangle_position);

		positionBuffer.position(0);


		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangle_position.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glVertexAttribPointer(VertexAttributeEnum.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);

		GLES32.glEnableVertexAttribArray(VertexAttributeEnum.AMC_ATTRIBUTE_POSITION);

		//unbind with vbo of positin
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		//UNBIND WITH VAO
		GLES32.glBindVertexArray(0);


		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);



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
		System.out.println("VBB: OpenGL_ES Renderer :"+ gl.glGetString(GL10.GL_RENDERER));
		System.out.println("VBB: OpenGL_ES Version :"+ gl.glGetString(GL10.GL_VERSION));
		System.out.println("VBB: OpenGL_ES Shading language Version :"+ gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
	}

	private void resize(int width, int height)
	{
		//code

		if(height <= 0)
		{
			height = 1;
		}

		GLES32.glViewport(0,0,width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void display()
	{
		//code
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		//transformations
		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];

		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);

		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -3.0f);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);


		GLES32.glBindVertexArray(vao[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 3);

		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);
	
		//Render
		requestRender();
	}

	private void uninitialise()
	{
		//code

		if(shaderProgramObject > 0)
		{
			GLES32.glUseProgram(shaderProgramObject);
			int[] retVal = new int[1];
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, retVal, 0);

			if(retVal[0] > 0)
			{
				int numAttachedShaders = retVal[0];
				int[] shaderObjects = new int[numAttachedShaders];
				GLES32.glGetAttachedShaders(shaderProgramObject, numAttachedShaders, retVal, 0, shaderObjects, 0);
				for (int i = 0; i < numAttachedShaders; i++)
					{
						GLES32.glDetachShader(shaderProgramObject, shaderObjects[i]);
						GLES32.glDeleteShader(shaderObjects[i]);
						shaderObjects[i] = 0;
					}

			}
		
			GLES32.glUseProgram(0);
			GLES32.glDeleteProgram(shaderProgramObject);

			shaderProgramObject = 0;
		}	

		if(vbo[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo, 0);
			vbo[0] = 0;
		}

		if(vao[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao, 0);
			vao[0] = 0;
		}
	}
}