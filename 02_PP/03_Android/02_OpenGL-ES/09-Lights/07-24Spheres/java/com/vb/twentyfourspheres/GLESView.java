package com.vb.twentyfourspheres;

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
import java.nio.ShortBuffer;

import android.opengl.Matrix;


public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener
{
	// global\class variable
	private GestureDetector gestureDetector = null;

	// opengl related
	private int shaderProgramObject;

	private int[] vao_sphere = new int[1];
	private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];

	private int ProjectionMatrixUniform, ModelMatrixUniform, ViewMatrixUniform = 0;
	private int LightAmbientUniform, LightDiffuseUniform, LightSpecularUniform, LightPositionUniform = 0;
	private int MaterialAmbientUniform, MaterialDiffuseUniform, MaterialSpecularUniform, MaterialShinenessUniform = 0;
	
	private int keyPressUniform = 0;	
	private boolean bLightingEnable = false;	

	private float[] lightAmbient = new float[]
	{
		1.0f, 1.0f, 1.0f, 1.0f
	};

	private float[] lightDiffuse = new float[]
	{
		1.0f, 1.0f, 1.0f, 1.0f
	};

	private float[] lightSpecular = new float[]
	{
		1.0f, 1.0f, 1.0f,1.0f 
	};

	private float[] lightPosition = new float[4];	

	private float[] materialAmbient = new float[4];
	
	private float[] materialDiffuse = new float[4];

	private float[] materialSpecular = new float[4];

	private float materialShineness;

	//key press for x,y,z rotation
	private char keyPressed;	

	// light animation global variable
	private float lightAngle = 0.0f;
	private int count;

	private float[] perspectiveProjectionMatrix = new float[16];

	// sphere	
	private int numVertices = 0;
	private int numElements = 0;

	private float vWidth;
	private float vHeight;

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
		vWidth = width;
		vHeight = height;
	}

	@Override
	public void onDrawFrame(GL10 gl)
	{
		// code
		update();
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
		if (bLightingEnable == false)
		{
			bLightingEnable = true;
		}
		else
		{
			bLightingEnable = false;
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
		if(count <= 3){
			count++;
		}
		else{
			count = 1;
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
			"precision highp float;" +
			"in vec4 aPosition;" +
			"in vec3 aNormal;" +
			"uniform vec4 uLightPosition;" +
			"uniform mat4 uModelMatrix;" +
			"uniform mat4 uViewMatrix;" +
			"uniform mat4 uProjectionMatrix;" +
			"uniform mediump int uKeyPress;" +
			"out vec3 oTransformedNormals;" +
			"out vec3 oLightDirection;" +
			"out vec3 oViewerVector;" +
			"void main(void)" +
			"{" +
				"if (uKeyPress == 1)" +
				"{" +
					"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;" +
					"oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;" +
					"oLightDirection = vec3(uLightPosition - eyeCoordinates);" +
					"oViewerVector = (-eyeCoordinates.xyz);" +
				"}" +
				"else" +
				"{" +
					"oTransformedNormals = vec3(0.0f, 0.0f, 0.0f);" +
					"oLightDirection = vec3(0.0f, 0.0f, 0.0f);" +
					"oViewerVector = vec3(0.0f, 0.0f, 0.0f);" +
				"}" +
				"gl_Position = (uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition);" +
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

		// fragment shader
		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"in vec3 oTransformedNormals;" +
			"in vec3 oLightDirection;" +
			"in vec3 oViewerVector;" +
			"uniform vec3 uLightAmbient;" +
			"uniform vec3 uLightDiffuse;" +
			"uniform vec3 uLightSpecular;" +
			"uniform vec3 uMaterialAmbient;" +
			"uniform vec3 uMaterialDiffuse;" +
			"uniform vec3 uMaterialSpecular;" +
			"uniform float uMaterialShineness;" +
			"uniform mediump int uKeyPress;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
				"vec3 Phong_ADS_Light = vec3(1.0, 1.0, 1.0);" +
				"if (uKeyPress == 1)" +
				"{" +
					"vec3 normalizedTransformedNormals = normalize(oTransformedNormals);" +
					"vec3 normalizedLightDirection = normalize(oLightDirection);" +
					"vec3 normalizedViewerVector = normalize(oViewerVector);" +
					"vec3 ambientLight = uLightAmbient * uMaterialAmbient;" +
					"vec3 diffuseLight = uLightDiffuse * uMaterialDiffuse * max(dot(normalizedLightDirection, normalizedTransformedNormals),0.0);" +
					"vec3 reflectionVector = reflect(- normalizedLightDirection, normalizedTransformedNormals);" +
					"vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector, normalizedViewerVector), 0.0), uMaterialShineness);" +
					"Phong_ADS_Light = ambientLight + diffuseLight + specularLight;" +
					"FragColor = vec4(Phong_ADS_Light, 1.0);" +
				"}" +
				"else" +
				"{" +
					"FragColor = vec4(1.0, 1.0, 1.0, 1.0);" +
				"}" +				
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
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
		GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
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

		ModelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uModelMatrix");
		ViewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uViewMatrix");
		ProjectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
		LightAmbientUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightAmbient");
		LightDiffuseUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightDiffuse");
		LightSpecularUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightSpecular");
		LightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition");
		MaterialAmbientUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialAmbient");
		MaterialDiffuseUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialDiffuse");
		MaterialSpecularUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialSpecular");
		MaterialShinenessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialShineness");
		keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKeyPress");

		//sphere
		Sphere sphere = new Sphere();
        float sphere_vertices[] = new float[1146];
        float sphere_normals[] = new float[1146];
        float sphere_textures[] = new float[764];
        short sphere_elements[] = new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

		// vao
		GLES32.glGenVertexArrays(1, vao_sphere, 0);
		GLES32.glBindVertexArray(vao_sphere[0]);

		//position vbo
		GLES32.glGenBuffers(1, vbo_sphere_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_position[0]);

		ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(
			GLES32.GL_ARRAY_BUFFER,
			sphere_vertices.length * 4,
			verticesBuffer,
			GLES32.GL_STATIC_DRAW
		);
        
        GLES32.glVertexAttribPointer(
			VertexAttributesEnum.AMC_ATTRIBUTE_POSITION,
			3,
			GLES32.GL_FLOAT,
			false,
			0,
			0
		);
        
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        // normal vbo
        GLES32.glGenBuffers(1,vbo_sphere_normal, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(
			GLES32.GL_ARRAY_BUFFER,
			sphere_normals.length * 4,
			verticesBuffer,
			GLES32.GL_STATIC_DRAW
		);

        GLES32.glVertexAttribPointer(
			VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL,
			3,
			GLES32.GL_FLOAT,
			false,
			0,
			0
		);
        
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        // element vbo
        GLES32.glGenBuffers(1,vbo_sphere_element, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(
			GLES32.GL_ELEMENT_ARRAY_BUFFER,
			sphere_elements.length * 2,
			elementsBuffer,
			GLES32.GL_STATIC_DRAW
		);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);
	
		// depth enable setting
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// enable back face culling
		GLES32.glEnable(GLES32.GL_CULL_FACE);

		// initialize light position
		lightPosition[0] = 0.0f;
		lightPosition[1] = 0.0f;
		lightPosition[2] = 0.0f;
		lightPosition[3] = 1.0f;

		// clear color
		GLES32.glClearColor(0.75f, 0.75f, 0.75f, 1.0f);

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

		//transformation and geometry
	
		/*****************************1st column*********************************************** */
		//*******************1st sphere on 1st column * ******************************
		GLES32.glViewport(0, (int)(vHeight * (5.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0215, 0.1745, 0.0215, 0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.6 * 128);

		/*******************2st sphere on 1st column********************************************************/
		GLES32.glViewport(0, (int)(vHeight * (4.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.135, 0.2225, 0.1575, 0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1 * 128);
		
		/*******************3rd sphere on 1st column*******************************************************************/
		GLES32.glViewport(0, (int)(vHeight * (3.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.05375, 0.05, 0.06625, 0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3 * 128);
		
		/*******************4th sphere on 1st column********************************************************************/
		GLES32.glViewport(0, (int)(vHeight * (2.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.25, 0.20725, 0.20725, 1.0, 0.829, 0.829, 0.296648, 0.296648, 0.296648, 0.088 * 128);
		
		/*******************5th sphere on 1st column*******************************************************************/
		GLES32.glViewport(0, (int)(vHeight * (1.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.1745, 0.01175, 0.01175, 0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6 * 128);
		
		/*******************6th sphere on 1st column********************************************************************/
		GLES32.glViewport(0, (int)(vHeight * (0.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.1, 0.18725, 0.1745, 0.396, 0.74151, 0.69102, 0.297254, 0.30829, 0.306678, 0.1 * 128);
	

		/*****************************2nd column*********************************************** */
		/*******************1st sphere on 2nd column****************************************************************************/
		GLES32.glViewport((int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (5.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.329412, 0.223529, 0.027451, 0.780392, 0.568627, 0.113725, 0.992157, 0.941176, 0.807843, 0.21794872 * 128);

		/*******************2nd sphere on 2nd column*************************************************************************/
		GLES32.glViewport((int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (4.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.2125, 0.1275, 0.054, 0.714, 0.4284, 0.18144, 0.393548, 0.271906, 0.166721, 0.2 * 128);

		/*******************3rd sphere on 2nd column***************************************************************************/
		GLES32.glViewport((int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (3.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.25, 0.25, 0.25, 0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6 * 128);

		/*******************4th sphere on 2nd column************************************************************************/
		GLES32.glViewport((int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (2.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.19125, 0.0735, 0.0225, 0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.6 * 128);

		/*******************5th sphere on 2nd column*******************************************************************/
		GLES32.glViewport((int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.24725, 0.1995, 0.0745, 0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 0.4 * 128);

		/*******************6th sphere on 2nd column********************************************************************/
		GLES32.glViewport((int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (0.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.19225, 0.19225, 0.19225, 0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4 * 128);


		/*****************************3rd column*********************************************** */
		/*******************1st sphere on 3rd column****************************************************************************/
		GLES32.glViewport((int)(vWidth * (2.0f / 4.0f)), (int)(vHeight * (5.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0, 0.0, 0.0, 0.01, 0.01, 0.01, 0.50, 0.50, 0.50, 0.25 * 128);

		/*******************2nd sphere on 3rd column*************************************************************************/
		GLES32.glViewport((int)(vWidth * (2.0f / 4.0f)), (int)(vHeight * (4.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0, 0.1, 0.06, 0.0, 0.50980392, 0.50980392, 0.50196078, 0.50196078, 0.50196078, 0.25 * 128);

		/*******************3rd sphere on 3rd column***************************************************************************/
		GLES32.glViewport((int)(vWidth * (2.0f / 4.0f)), (int)(vHeight * (3.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0, 0.0, 0.0, 0.1, 0.35, 0.1, 0.45, 0.55, 0.45, 0.25 * 128);

		/*******************4th sphere on 3rd column************************************************************************/
		GLES32.glViewport((int)(vWidth * (2.0f / 4.0f)), (int)(vHeight * (2.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.7, 0.6, 0.6, 0.25 * 128);

		/*******************5th sphere on 3rd column*******************************************************************/
		GLES32.glViewport((int)(vWidth * (2.0f / 4.0f)), (int)(vHeight * (1.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0, 0.0, 0.0, 0.55, 0.55, 0.55, 0.70, 0.70, 0.70, 0.25 * 128);

		/*******************6th sphere on 3rd column********************************************************************/
		GLES32.glViewport((int)(vWidth * (2.0f / 4.0f)), (int)(vHeight * (0.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.60, 0.60, 0.50, 0.25 * 128);


		/*****************************4th column*********************************************** */
		/*******************1st sphere on 4th column****************************************************************************/
		GLES32.glViewport((int)(vWidth * (3.0f / 4.0f)), (int)(vHeight * (5.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.4, 0.4, 0.4, 0.078125 * 128);

		/*******************2nd sphere on 4th column*************************************************************************/
		GLES32.glViewport((int)(vWidth * (3.0f / 4.0f)), (int)(vHeight * (4.0f / 6.0f)), (int)(vWidth * (1.0f / 4.0f)), (int)(vHeight * (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0, 0.05, 0.05, 0.4, 0.5, 0.5, 0.04, 0.7, 0.7, 0.078125 * 128);

		/*******************3rd sphere on 4th column***************************************************************************/
		GLES32.glViewport((int)(vWidth* (3.0f / 4.0f)), (int)(vHeight* (3.0f / 6.0f)), (int)(vWidth* (1.0f / 4.0f)), (int)(vHeight* (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.0, 0.05, 0.0, 0.4, 0.5, 0.4, 0.04, 0.7, 0.04, 0.078125 * 128);

		/*******************4th sphere on 4th column************************************************************************/
		GLES32.glViewport((int)(vWidth* (3.0f / 4.0f)), (int)(vHeight* (2.0f / 6.0f)), (int)(vWidth* (1.0f / 4.0f)), (int)(vHeight* (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.05, 0.0, 0.0, 0.5, 0.4, 0.4, 0.7, 0.04, 0.04, 0.078125 * 128);

		/*******************5th sphere on 4th column*******************************************************************/
		GLES32.glViewport((int)(vWidth* (3.0f / 4.0f)), (int)(vHeight* (1.0f / 6.0f)), (int)(vWidth* (1.0f / 4.0f)), (int)(vHeight* (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.05, 0.05, 0.05, 0.5, 0.5, 0.5, 0.7, 0.7, 0.7, 0.078125 * 128);

		/*******************6th sphere on 4th column********************************************************************/
		GLES32.glViewport((int)(vWidth* (3.0f / 4.0f)), (int)(vHeight* (0.0f / 6.0f)), (int)(vWidth* (1.0f / 4.0f)), (int)(vHeight* (1.0f / 4.0f)));
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)vWidth / (float)vHeight, 0.1f, 100.0f);
		sphere(0.05, 0.05, 0.0, 0.5, 0.5, 0.4, 0.7, 0.7, 0.04, 0.078125 * 128);
		
		GLES32.glUseProgram(0);

		// Render
		requestRender();
	}

	private void sphere(double a1, double a2, double a3, double d1, double d2, double d3, double s1, double s2, double s3, double msh)
	{

		float[] modelMatrix = new float[16];
		float[] viewMatrix = new float[16];

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);

		Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -4.0f);
		
		GLES32.glUniformMatrix4fv(ViewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(ModelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(ProjectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);


		if (bLightingEnable == true)
		{
			GLES32.glUniform1i(keyPressUniform, 1);

			GLES32.glUniform3fv(LightAmbientUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(LightDiffuseUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(LightSpecularUniform, 1, lightSpecular, 0);
			GLES32.glUniform4fv(LightPositionUniform, 1, lightPosition, 0);

			// ambient material
			materialAmbient[0] = (float)a1;
			materialAmbient[1] = (float)a2;
			materialAmbient[2] = (float)a3;
			
			// diffuse material
			materialDiffuse[0] = (float)d1;
			materialDiffuse[1] = (float)d2;
			materialDiffuse[2] = (float)d3;
			
			// specular material
			materialSpecular[0] = (float)s1;
			materialSpecular[1] = (float)s2;
			materialSpecular[2] = (float)s3;
			

			// material shininess
			materialShineness = (float)msh;

			GLES32.glUniform3fv(MaterialAmbientUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(MaterialDiffuseUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(MaterialSpecularUniform, 1, materialSpecular, 0);
			GLES32.glUniform1f(MaterialShinenessUniform, materialShineness);
		}
		else 
		{
			GLES32.glUniform1i(keyPressUniform, 0);
		}
		
	
		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

		GLES32.glBindVertexArray(0);
	}

	void update()
	{		
		if (count == 1)
		{
			lightPosition[0] = 0.0f;
			lightPosition[1] = 100.0f * (float)Math.sin(lightAngle);
			lightPosition[2] = 100.0f * (float)Math.cos(lightAngle);
			lightPosition[3] = 1.0f;
		}
		if(count == 2)
		{
			lightPosition[0] = 100.0f * (float)Math.cos(lightAngle);
			lightPosition[1] = 0.0f;
			lightPosition[2] = 100.0f * (float)Math.sin(lightAngle);
			lightPosition[3] = 1.0f;	
		}
		if (count == 3)
		{
			lightPosition[0] = 100.0f * (float)Math.cos(lightAngle);
			lightPosition[1] = 100.0f * (float)Math.sin(lightAngle);
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}
	
		lightAngle += 0.05f;
		if (lightAngle > (2 * Math.PI))
		{
			lightAngle = 0.0f;
		}
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

		// destroy vao
        if(vao_sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        
        // destroy position vbo
        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo
        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }
	}	
}
