package com.vb.texturedcube;

import android.content.Context;

// OpenGLES releted packages

import android.opengl.GLSurfaceView;
import android.opengl.GLES32;
import javax.microedition.khronos.opengles.GL10;

import com.pb.texturedcube.VertexAttributesEnum;

import javax.microedition.khronos.egl.EGLConfig;

// pacakge related to event

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

// package for textures

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

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
	
	private int[] vao_Cube = new int[1];

	private int[] vbo_pcnt = new int[1];

	private float angle_Cube = 0.0f;

	private int ProjectionMatrixUniform, ModelMatrixUniform, ViewMatrixUniform;
	private int LightAmbientUniform, LightDiffuseUniform, LightSpecularUniform, LightPositionUniform;
	private int MaterialAmbientUniform, MaterialDiffuseUniform, MaterialSpecularUniform, MaterialShinenessUniform;
	
	// Key press
	private int keyPressUniform = 0; 
	private boolean bLightingEnable = false;

	//texture related
	private int[] texture_marble= new int[1];
	private int textureSamplerUniform;

	private final Context context;

	// light 1 properties global variable
	private float[] lightAmbient = new float[]{ 0.1f, 0.1f, 0.1f, 1.0f };
	private float[] lightDiffuse = new float[]{ 1.0f, 1.0f, 1.0f, 1.0f }; 
	private float[] lightSpecular = new float[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	private float[] lightPosition = new float[]{ 0.0f, 0.0f, 0.0f, 1.0f };

	// Material properties global variable
	private float[] materialAmbient = new float[]{ 0.0f, 0.0f, 0.0f, 1.0f };
	private float[] materialDiffuse = new float[]{ 1.0f, 1.0f, 1.0f, 1.0f }; 
	private float[] materialSpecular = new float[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	private float materialShineness = 128.0f;

	private float[] perspectiveProjectionMatrix = new float[16];

	//constructor GLESView
	public GLESView(Context _context)
	{
		super(_context);

		context = _context;

		// OpenGl-ES related
		setEGLContextClientVersion(3);
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		
		//event related
		// create and set gestureDetector object
		gestureDetector = new GestureDetector(_context, this, null, false);
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
			"in vec4 aPosition;" +
			"in vec3 aNormal;" +
			"in vec4 aColor;" +
			"in vec2 aTexCoord;" +
			"uniform vec4 uLightPosition;" +
			"uniform mat4 uModelMatrix;" +
			"uniform mat4 uViewMatrix;" +
			"uniform mat4 uProjectionMatrix;" +
			"uniform mediump int uKeyPress;" +
			"out vec3 oTransformedNormals;" +
			"out vec3 oLightDirection;" +
			"out vec3 oViewerVector;" +
			"out vec4 oColor;" +
			"out vec2 oTexCoord;" +
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
				"oColor = aColor;" +
				"oTexCoord = aTexCoord;" +
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
			"in vec3 oTransformedNormals;"+
			"in vec3 oLightDirection;"+
			"in vec3 oViewerVector;"+
			"in vec4 oColor;"+
			"in vec2 oTexCoord;"+
			"uniform vec3 uLightAmbient;"+
			"uniform vec3 uLightDiffuse;"+
			"uniform vec3 uLightSpecular;"+
			"uniform vec3 uMaterialAmbient;"+
			"uniform vec3 uMaterialDiffuse;"+
			"uniform vec3 uMaterialSpecular;"+
			"uniform float uMaterialShineness;"+
			"uniform mediump int uKeyPress;"+
			"uniform highp sampler2D uTextureSampler;"+
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
				"vec3 Phong_ADS_Light;"+
				"if (uKeyPress == 1)"+
				"{"+
					"vec3 normalizedTransformedNormals = normalize(oTransformedNormals);"+
					"vec3 normalizedLightDirection = normalize(oLightDirection);"+
					"vec3 normalizedViewerVector = normalize(oViewerVector);"+
					"vec3 ambientLight = uLightAmbient * uMaterialAmbient;"+
					"vec3 diffuseLight = uLightDiffuse * uMaterialDiffuse * max(dot(normalizedLightDirection, normalizedTransformedNormals),0.0f);"+
					"vec3 reflectionVector = reflect(- normalizedLightDirection, normalizedTransformedNormals);"+
					"vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector, normalizedViewerVector), 0.0f), uMaterialShineness);"+
					"Phong_ADS_Light = ambientLight + diffuseLight + specularLight;"+
				"}"+
				"else"+
				"{"+
					"Phong_ADS_Light = vec3(1.0f, 1.0f, 1.0f);"+
				"}"+
				"vec3 tex = vec3(texture(uTextureSampler, oTexCoord));"+
				"FragColor = vec4(tex *  vec3(oColor) * Phong_ADS_Light, 1.0f);"+
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
		GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");
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

		textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uTextureSampler");
		
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

		// Geometry/ shapes
		final float[] cube_PCNT = new float[]
		{
			// front
			// position				// color			 // normals				// texcoords
			1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
			-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
			-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
			1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
								
			// right			 
			// position				// color			 // normals				// texcoords
			1.0f,  1.0f, -1.0f,	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
			1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
			1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
								
			// back				 
			// position				// color			 // normals				// texcoords
			1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
			-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
			1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
								
			// left				 
			// position				// color			 // normals				// texcoords
			-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
			-1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
			-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
								
			// top				 
			// position				// color			 // normals				// texcoords
			1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
			-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
			-1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
			1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
								
			// bottom			 
			// position				// color			 // normals				// texcoords
			1.0f, -1.0f,  1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
			-1.0f, -1.0f,  1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
			1.0f, -1.0f, -1.0f,	1.0f, 0.5f, 0.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f
		};	
		
		
		//---------------cube----------------------------
		// vao
		GLES32.glGenVertexArrays(1, vao_Cube, 0);
		GLES32.glBindVertexArray(vao_Cube[0]);
		GLES32.glGenBuffers(1, vbo_pcnt, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_pcnt[0]);
	
		//prepare triagle vertices for glbuffer data()
		ByteBuffer byteBuffer= ByteBuffer.allocateDirect(cube_PCNT.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer positionBuffer1 = byteBuffer.asFloatBuffer();
		positionBuffer1.put(cube_PCNT);
		positionBuffer1.position(0);
	
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, (24 * 11 * 4), positionBuffer1, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, (11 * 4), (0 * 4));
		GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
		
		//Color
		GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, (11 * 4) , (3 * 4));
		GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_COLOR);
		
		//Normal
		GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, (11 * 4) , (6 * 4));
		GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL);
		
		//Texcoord
		GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, (11 * 4) , (9 * 4));
		GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		// depth enable setting
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// Disable back face culling
		GLES32.glDisable(GLES32.GL_CULL_FACE);

		// clear color
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
		
		//load texture
		texture_marble[0] = loadGLTexture(R.raw.marble);
	}

	private int loadGLTexture(int ImageResoureID)
	{
		// Create Bitmap factory option object
		BitmapFactory.Options options = new BitmapFactory.Options();

		// don't scale the image
		options.inScaled = false;

		// create the bitmap image from image resource
		Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), ImageResoureID, options);

		// create texture
		int [] texture  = new int[1];
		GLES32.glGenTextures(1, texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR); //magnification filter
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR); // minification filter
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
		GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);// unbind 
		return texture[0];
	}

	private void printGLInfo(GL10 gl)
	{
		// code
		System.out.println(" OpenGL-ES Renderer : " + gl.glGetString(GL10.GL_RENDERER));
		System.out.println(" OpenGL-ES Version : " + gl.glGetString(GL10.GL_VERSION));
		System.out.println(" OpenGL-ES Shading Langauge Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
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
		//-----------------Cube-----------------------------------
		float[] modelViewMatrix = new float[16];		
		float[] translationMatrix = new float[16];
		float[] rotationMatrix = new float[16];
		float[] rotationMatrixX = new float[16];
		float[] rotationMatrixY = new float[16];
		float[] rotationMatrixZ = new float[16];
		float[] scaleMatrix = new float[16];
		float[] ModelMatrix = new float[16];

		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);
		Matrix.setIdentityM(rotationMatrixX, 0);
		Matrix.setIdentityM(rotationMatrixY, 0);
		Matrix.setIdentityM(rotationMatrixZ, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
		Matrix.setIdentityM(ModelMatrix, 0);

		Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.scaleM(scaleMatrix, 0, 0.75f, 0.75f, 0.75f);
		Matrix.setRotateM(rotationMatrixX, 0, angle_Cube, 1.0f, 0.0f, 0.0f);
		Matrix.setRotateM(rotationMatrixY, 0, angle_Cube, 0.0f, 1.0f, 0.0f);
		Matrix.setRotateM(rotationMatrixZ, 0, angle_Cube, 0.0f, 0.0f, 1.0f);

		Matrix.multiplyMM(rotationMatrix, 0, rotationMatrixX, 0, rotationMatrixY, 0);
		Matrix.multiplyMM(rotationMatrix, 0, rotationMatrix, 0, rotationMatrixZ, 0);
		
		Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, scaleMatrix, 0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);
		
		GLES32.glUniformMatrix4fv(ModelMatrixUniform, 1, false, ModelMatrix, 0);
		GLES32.glUniformMatrix4fv(ViewMatrixUniform, 1, false, modelViewMatrix, 0);
		GLES32.glUniformMatrix4fv(ProjectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);

		if (bLightingEnable == true)
		{
			GLES32.glUniform1i(keyPressUniform, 1);
			GLES32.glUniform3fv(LightAmbientUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(LightDiffuseUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(LightSpecularUniform, 1, lightSpecular, 0);
			GLES32.glUniform4fv(LightPositionUniform, 1, lightPosition, 0);

			GLES32.glUniform3fv(MaterialAmbientUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(MaterialDiffuseUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(MaterialSpecularUniform, 1, materialSpecular, 0);
			GLES32.glUniform1f(MaterialShinenessUniform, materialShineness);			
		}
		else
		{
			GLES32.glUniform1i(keyPressUniform, 0);
		}

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_marble[0]); // bind texture
		GLES32.glUniform1i(textureSamplerUniform, 0);

		GLES32.glBindVertexArray(vao_Cube[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		// Render
		requestRender();
	}

	private void update()
	{
		// Rectangle Rotate
		angle_Cube = angle_Cube - 1.0f;
		if (angle_Cube <= 360.0f)
		{
			angle_Cube = angle_Cube + 360.0f;
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

		//---------------Square-------------------------
		if (vbo_pcnt[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_pcnt, 0);
			vbo_pcnt[0] = 0;
		}

		if (texture_marble[0] > 0)
		{
			GLES32.glDeleteTextures(1, texture_marble, 0);
			texture_marble[0] = 0;
		}
		
		if (vao_Cube[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_Cube, 0);
			vao_Cube[0] = 0;
		}		
	}	
}
