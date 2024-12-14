package com.vb.tesselationshader;

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

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener
{
	// global\class variable
	private GestureDetector gestureDetector = null;

	// opengl related
	private int shaderProgramObject;
	private int[] vao = new int[1];
	private int[] vbo_position = new int[1];

	private int numberOfSegmentsUniform;
	private int[] numberOfStripsUniform = new int[1];
	private int[] lineColorUniform = new int[1];
	
	private int uiNumberOfLineSegments;

	private int mvpMatrixUniform;
	private float[] perspectiveProjectionMatrix = new float[16];

	private float[] lineColor = new float[]
	{
		1.0f, 1.0f, 0.0f, 1.0f
	};
	//constructor GLESView
	public GLESView(Context context)
	{
		super(context);

		// OpenGl-ES related
		setEGLContextClientVersion(3);
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		
		//event related
		// create and set gestureDetector object
		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	// implimentation of 3 methods of GLSurfaceView.Renderer interface
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		// code
		initialize(gl);
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		// code
		resize(width, height);
	}

	@Override
	public void onDrawFrame(GL10 gl)
	{
		// code
		display();
	}

	// implementations of OnTouch event of view Class	
	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		if (!gestureDetector.onTouchEvent(e))
		{
			super.onTouchEvent(e);
		}
		return(true);
	}

	// implementations of 3 methods of OnDoubleTapLisner interface
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		uiNumberOfLineSegments --;
		if (uiNumberOfLineSegments <= 1)
		{
			uiNumberOfLineSegments = 1;
		}
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
		uiNumberOfLineSegments ++;
		if (uiNumberOfLineSegments >= 30)
		{
			uiNumberOfLineSegments = 30;
		}
		return(true);
	}

	// implementations of 6 methods of OnGestureLisner interface
	@Override
	public boolean onDown(MotionEvent e)
	{
		return(true);
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return(true);
	}

	@Override
	public void onLongPress(MotionEvent e)
	{
		
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		// code
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

	// implimentation of private methods
	private void initialize(GL10 gl)
	{
		// code
		//print opengl es info
		printGLInfo(gl);

		// vertex shader
		final String vertexShaderSourceCode = String.format
		(	"#version 320 es"+
			"\n"+
			"in vec2 aPosition;"+
			"void main(void)"+
			"{"+
				"gl_Position = vec4(aPosition, 0.0, 1.0);"+
			"}"
		);

		int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
		GLES32.glCompileShader(vertexShaderObject);

		int[] shaderCompileStatus = new int[1];
		int[] infoLogLength = new int[1];
		String infoLog = null;

		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		if (shaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject,GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("Vertex shader compilation error log: " + infoLog);
				uninitialize();
				System.exit(0);
			}
		}

		// Tesselation Control Shader
		final String tesselationControlShaderSourceCode = String.format
		(	"#version 320 es"+
			"\n"+
			"layout(vertices=4) out;" +
			"uniform int uNumberOfSegments;" +
			"uniform int uNumberOfStrips;" +
			"void main(void)" +
			"{" +
				"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" +
				"gl_TessLevelOuter[0] = float(uNumberOfStrips);" +
				"gl_TessLevelOuter[1] = float(uNumberOfSegments);" +
			"}"
		);

		int tesselationControlShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);
		GLES32.glShaderSource(tesselationControlShaderObject, tesselationControlShaderSourceCode);
		GLES32.glCompileShader(tesselationControlShaderObject);

		shaderCompileStatus = new int[1];
		infoLogLength = new int[1];
		infoLog = null;

		GLES32.glGetShaderiv(tesselationControlShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		if (shaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(tesselationControlShaderObject,GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(tesselationControlShaderObject);
				System.out.println("Tesselation Control Shader compilation error log: " + infoLog);
				uninitialize();
				System.exit(0);
			}
		}

		// Tesselation Evaluation Shader
		final String tesselationEvaluationShaderSourceCode = String.format
		(	"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"precision highp int;"+
			"layout(isolines) in;" +
			"uniform mat4 uMVPMatrix;" +
			"void main(void)" +
			"{" +
				"vec3 P0 = gl_in[0].gl_Position.xyz;" +
				"vec3 P1 = gl_in[1].gl_Position.xyz;" +
				"vec3 P2 = gl_in[2].gl_Position.xyz;" +
				"vec3 P3 = gl_in[3].gl_Position.xyz;" +
				"vec3 P = P0 * (1.0 - gl_TessCoord.x) * (1.0 - gl_TessCoord.x) * (1.0 - gl_TessCoord.x) + P1 * 3.0 * gl_TessCoord.x * (1.0 - gl_TessCoord.x) * (1.0 - gl_TessCoord.x) + P2 * 3.0 * gl_TessCoord.x * gl_TessCoord.x * (1.0 - gl_TessCoord.x) + P3 *  gl_TessCoord.x * gl_TessCoord.x * gl_TessCoord.x;" +
				"gl_Position = uMVPMatrix * vec4(P, 1.0f);" +
			"}"
		);

		int tesselationEvaluationShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);
		GLES32.glShaderSource(tesselationEvaluationShaderObject, tesselationEvaluationShaderSourceCode);
		GLES32.glCompileShader(tesselationEvaluationShaderObject);

		shaderCompileStatus = new int[1];
		infoLogLength = new int[1];
		infoLog = null;

		GLES32.glGetShaderiv(tesselationEvaluationShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		if (shaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(tesselationEvaluationShaderObject,GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(tesselationEvaluationShaderObject);
				System.out.println("Tesselation Evaluation Shader compilation error log: " + infoLog);
				uninitialize();
				System.exit(0);
			}
		}

		// fragment shader
		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"uniform vec4 uLineColor;" +
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
				"FragColor = uLineColor;"+
			"}"
		);

		int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
		GLES32.glCompileShader(fragmentShaderObject);

		shaderCompileStatus[0] = 0;
		infoLogLength[0] = 0;
		infoLog = null;

		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		if (shaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject,GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("Fragment shader compilation error log: " + infoLog);
				uninitialize();
				System.exit(0);
			}
		}

		// shader program
		shaderProgramObject = GLES32.glCreateProgram();
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);		
		GLES32.glAttachShader(shaderProgramObject, tesselationControlShaderObject);
		GLES32.glAttachShader(shaderProgramObject, tesselationEvaluationShaderObject);
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

		GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
		GLES32.glLinkProgram(shaderProgramObject);

		int[] programLinkStatus = new int[1];
		infoLogLength[0] = 0;
		infoLog = null;

		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, programLinkStatus, 0);
		if (programLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject,GLES32. GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("shader program link error log: " + infoLog);
				uninitialize();
				System.exit(0);
			}
		}

		mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
		numberOfSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uNumberOfSegments");
		numberOfStripsUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uNumberOfStrips");
		lineColorUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLineColor");

		// Geometry/ shapes
		final float[] point_position = new float[]
		{
			-1.0f, -1.0f,//3rd quad
			-0.5f, 1.0f,// 2nd quad
			0.5f, -1.0f,// 4th quad
			1.0f, 1.0f// 1st quad
		};

		// vao
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);
		GLES32.glGenBuffers(1, vbo_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		//prepare triagle vertices for glbuffer data()
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(point_position.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(point_position);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, point_position.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, 2, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);		

		// depth enable setting
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// enable back face culling
		GLES32.glEnable(GLES32.GL_CULL_FACE);

		// clear color
		GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

		// For one Line
		uiNumberOfLineSegments = 1; // for one line

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);	
	}

	private void printGLInfo(GL10 gl)
	{
		// code
		System.out.println("OpenGL-ES Renderer : " + gl.glGetString(GL10.GL_RENDERER));
		System.out.println("OpenGL-ES Version : " + gl.glGetString(GL10.GL_VERSION));
		System.out.println("OpenGL-ES Shading Langauge Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
	}

	private void resize(int width, int height)
	{
		// code
		if (height <= 0)
		{
			height = 1;
		}
		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void display()
	{
		// code
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		// transformation
		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];

		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		
		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
		
		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);
		GLES32.glUniform1i(numberOfSegmentsUniform, uiNumberOfLineSegments);
		GLES32.glUniform1i(numberOfStripsUniform[0], 1);
		GLES32.glUniform4fv(lineColorUniform[0], 1, lineColor, 0);

		GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES, 4);

		GLES32.glBindVertexArray(vao[0]);

		// draw the geometry
		GLES32.glDrawArrays(GLES32.GL_PATCHES, 0, 4);

		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		// Render
		requestRender();
	}

	private void uninitialize()
	{
		// code
		if (shaderProgramObject > 0)
		{
			GLES32.glUseProgram(shaderProgramObject);
			int[] ratVal = new int[1];
			GLES32.glGetProgramiv(shaderProgramObject,GLES32.GL_ATTACHED_SHADERS, ratVal, 0);

			if (ratVal[0] > 0)
			{
				int numAttachedShader = ratVal[0];
				int[] shaderObjects = new int[numAttachedShader];

				GLES32.glGetAttachedShaders(shaderProgramObject,numAttachedShader, ratVal, 0, shaderObjects, 0);

				for (int i = 0; i < numAttachedShader; i++)
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

		if (vbo_position[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position, 0);
			vbo_position[0] = 0;
		}

		if (vao[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao, 0);
			vao[0] = 0;
		}
	}	
}
