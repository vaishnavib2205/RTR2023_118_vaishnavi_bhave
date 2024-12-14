package com.vb.fbo;

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

	// *******cube related global variable****
	// opengl related
	private int shaderProgramObject_Cube;
	
	private int[] vao_Cube = new int[1];

	private int[] vbo_position_Cube = new int[1];
	private int[] vbo_texcoord_Cube = new int[1];

	private float cube_angle = 0.0f;

	private int mvpMatrixUniform_Cube;
	private int textureSamplerUniform_Cube;
	private float[] perspectiveProjectionMatrix_Cube = new float[16];

	//******* Sphere related global variable********
	// Shader Program object
	private int shaderProgramObject_PV_Sphere;
	private int shaderProgramObject_PF_Sphere;

	private int[] vao_Sphere = new int[1];
	private int[] vbo_position_Sphere = new int[1];
    private int[] vbo_normal_Sphere = new int[1];
	private int[] vbo_texcoord_Sphere = new int[1];
    private int[] vbo_element_Sphere = new int[1];

	private int ProjectionMatrixUniform_PV_Sphere, ModelMatrixUniform_PV_Sphere, ViewMatrixUniform_PV_Sphere = 0;
	private int MaterialAmbientUniform_PV_Sphere, MaterialDiffuseUniform_PV_Sphere, MaterialSpecularUniform_PV_Sphere, MaterialShinenessUniform_PV_Sphere = 0;
	private int keyPressUniform_PV_Sphere = 0;	
	private int[] LightAmbientUniform_PV_Sphere = new int[3];
	private int[] LightDiffuseUniform_PV_Sphere = new int[3];
	private int[] LightSpecularUniform_PV_Sphere = new int[3];
	private int[] LightPositionUniform_PV_Sphere = new int[3];

	private int ProjectionMatrixUniform_PF_Sphere, ModelMatrixUniform_PF_Sphere, ViewMatrixUniform_PF_Sphere = 0;
	private int MaterialAmbientUniform_PF_Sphere, MaterialDiffuseUniform_PF_Sphere, MaterialSpecularUniform_PF_Sphere, MaterialShinenessUniform_PF_Sphere = 0;
	private int keyPressUniform_PF_Sphere = 0;	
	private int[] LightAmbientUniform_PF_Sphere = new int[3];
	private int[] LightDiffuseUniform_PF_Sphere= new int[3];
	private int[] LightSpecularUniform_PF_Sphere = new int[3];
	private int[] LightPositionUniform_PF_Sphere = new int[3];

	private class Light
	{
		float[] ambient = new float[3];
		float[] diffuse = new float[3];
		float[] specular = new float[3];
		float[] position = new float[4];
	};

	private Light[] light = new Light[3];

	private boolean bLightingEnable = false;

	// sphere	
	private int numVertices = 0;
	private int numElements = 0;

	// light animation global variable
	private float lightAngleZero = 0.0f;
	private float lightAngleOne = 0.0f;
	private float lightAngleTwo = 0.0f;

	private float[] materialAmbient = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
	private float[] materialDiffuse = new float[]{0.5f, 0.2f, 0.7f, 1.0f};
	private float[] materialSpecular = new float[]{0.7f, 0.7f, 0.7f, 1.0f};	

	private float[] perspectiveProjectionMatrix_Sphere = new float[16];

	//******** FBO related global variable*******
	private int[] FBO = new int[1];//frame buffer object
	private int[] RBO = new int[1];//render buffer object
	private int[] textureFBO = new int[1];;
	private boolean bFboResult = false;
	
	private int winWidth;
	private int winHeight;

	private int WIN_WIDTH = 800;
	private int WIN_HEIGHT = 600;

	private int FBO_WIDTH = 512;
	private int FBO_HEIGHT = 512;

	// for shader selection
	private int  choosenShader = 1;

	private float materialShineness = 128.0f;
	
	private final Context context;

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
		initialize_Cube(gl);
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		// code
		resize_Cube(width, height);
	}

	@Override
	public void onDrawFrame(GL10 gl)
	{
		// code
		update_Cube();

		display_Cube();
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
		if (choosenShader == 1)
		{
			choosenShader = 2;
		}
		else
		{
			choosenShader = 1;
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
		uninitialize_Cube();

		uninitialize_Sphere();

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
	private void initialize_Cube(GL10 gl)
	{
		// code
		//print opengl es info
		printGLInfo(gl);

		// vertex shader
		final String vertexShaderSourceCode = String.format
		(	"#version 320 es"+
			"\n"+
			"in vec4 aPosition;"+
			"in vec2 aTexCoord;" +
			"out vec2 oTexCoord;" +
			"uniform mat4 uMVPMatrix;"+
			"void main(void)"+
			"{"+
				"gl_Position = uMVPMatrix * aPosition;"+
				"oTexCoord = aTexCoord;" +
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
				uninitialize_Cube();
				System.exit(0);
			}
		}

		// fragment shader
		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"uniform highp sampler2D uTextureSampler;"+
			"in vec2 oTexCoord;" +
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
				"FragColor = texture(uTextureSampler, oTexCoord);"+
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
				uninitialize_Cube();
				System.exit(0);
			}
		}

		// shader program
		shaderProgramObject_Cube = GLES32.glCreateProgram();
		GLES32.glAttachShader(shaderProgramObject_Cube, vertexShaderObject);
		GLES32.glAttachShader(shaderProgramObject_Cube, fragmentShaderObject);
		GLES32.glBindAttribLocation(shaderProgramObject_Cube, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
		GLES32.glBindAttribLocation(shaderProgramObject_Cube, VertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");
		GLES32.glLinkProgram(shaderProgramObject_Cube);

		int[] programLinkStatus = new int[1];
		infoLogLength[0] = 0;
		infoLog = null;

		GLES32.glGetProgramiv(shaderProgramObject_Cube, GLES32.GL_LINK_STATUS, programLinkStatus, 0);
		if (programLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject_Cube,GLES32. GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetProgramInfoLog(shaderProgramObject_Cube);
				System.out.println("shader program link error log: " + infoLog);
				uninitialize_Cube();
				System.exit(0);
			}
		}

		mvpMatrixUniform_Cube = GLES32.glGetUniformLocation(shaderProgramObject_Cube, "uMVPMatrix");
		textureSamplerUniform_Cube = GLES32.glGetUniformLocation(shaderProgramObject_Cube, "uTextureSampler");

		// Geometry/ shapes
		final float[] cube_position = new float[]
		{
			// top
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
		
			// bottom
			1.0f, -1.0f, -1.0f,
		   -1.0f, -1.0f, -1.0f,
		   -1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
		
			// front
			1.0f, 1.0f, 1.0f,
		   -1.0f, 1.0f, 1.0f,
		   -1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
		
			// back
			1.0f, 1.0f, -1.0f,
		   -1.0f, 1.0f, -1.0f,
		   -1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
		
			// right
			1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
		
			// left
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f
		};	
		
		final float[] cube_texcoord = new float[]
		{
			// front
			1.0f, 1.0f, // top-right of front
			0.0f, 1.0f, // top-left of front
			0.0f, 0.0f, // bottom-left of front
			1.0f, 0.0f, // bottom-right of front

			// right
			1.0f, 1.0f, // top-right of right
			0.0f, 1.0f, // top-left of right
			0.0f, 0.0f, // bottom-left of right
			1.0f, 0.0f, // bottom-right of right

			// back
			1.0f, 1.0f, // top-right of back
			0.0f, 1.0f, // top-left of back
			0.0f, 0.0f, // bottom-left of back
			1.0f, 0.0f, // bottom-right of back

			// left
			1.0f, 1.0f, // top-right of left
			0.0f, 1.0f, // top-left of left
			0.0f, 0.0f, // bottom-left of left
			1.0f, 0.0f, // bottom-right of left

			// top
			1.0f, 1.0f, // top-right of top
			0.0f, 1.0f, // top-left of top
			0.0f, 0.0f, // bottom-left of top
			1.0f, 0.0f, // bottom-right of top

			// bottom
			1.0f, 1.0f, // top-right of bottom
			0.0f, 1.0f, // top-left of bottom
			0.0f, 0.0f, // bottom-left of bottom
			1.0f, 0.0f // bottom-right of bottom
		};

		//---------------Square----------------------------
		// vao
		GLES32.glGenVertexArrays(1, vao_Cube, 0);
		GLES32.glBindVertexArray(vao_Cube[0]);
		GLES32.glGenBuffers(1, vbo_position_Cube, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_Cube[0]);
	
		//prepare triagle vertices for glbuffer data()
		ByteBuffer byteBuffer= ByteBuffer.allocateDirect(cube_position.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(cube_position);
		positionBuffer.position(0);
	
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cube_position.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		//TexCoord
		GLES32.glGenBuffers(1, vbo_texcoord_Cube, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord_Cube[0]);
		ByteBuffer texcoordBuffer = ByteBuffer.allocateDirect(cube_texcoord.length * 4);
		texcoordBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer texBuffer = texcoordBuffer.asFloatBuffer();
		texBuffer.put(cube_texcoord);
		texBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cube_texcoord.length * 4, texBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
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
		GLES32.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		Matrix.setIdentityM(perspectiveProjectionMatrix_Cube, 0);
		
		// warmup resize for cube
		resize_Cube(WIN_WIDTH, WIN_HEIGHT);

		// FBO related code
		if (createFBO(FBO_WIDTH, FBO_HEIGHT) == true)
		{
			bFboResult = initialize_Sphere(FBO_WIDTH, FBO_HEIGHT);
		}
		//return(0);
	}

	private boolean initialize_Sphere(int textureWidth, int textureHeight)
	{
		// code
				
		/***************************************************per vertex****************************************************************************/
		/*//////*//////////////////////////////////////////////////////////////////////// */
		// vertex shader
		final String vertexShaderSourceCode_PV = String.format
		(	"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"in vec4 aPosition;" +
			"in vec3 aNormal;" +
			"uniform vec3 uLightAmbient[3];" +
			"uniform vec3 uLightDiffuse[3];" +
			"uniform vec3 uLightSpecular[3];" +
			"uniform vec4 uLightPosition[3];" +
			"uniform vec3 uMaterialAmbient;" +
			"uniform vec3 uMaterialDiffuse;" +
			"uniform vec3 uMaterialSpecular;" +
			"uniform float uMaterialShineness;" +
			"uniform mat4 uModelMatrix;" +
			"uniform mat4 uViewMatrix;" +
			"uniform mat4 uProjectionMatrix;" +
			"uniform mediump int uKeyPress;" +
			"out vec3 oPhong_ADS_Light;" +
			"void main(void)" +
			"{" +
				"oPhong_ADS_Light = vec3 (0.0, 0.0, 0.0);" +
			"if (uKeyPress == 1)" +
			"{" +
				"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;" +
				"vec3 tranformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);" +
				"vec3 lightDirection[3];" +
				"vec3 reflectionVector[3];" +
				"vec3 ambientLight[3];" +
				"vec3 diffuseLight[3];" +
				"vec3 specularLight[3];" +
				"for (int i = 0; i < 3; i++)" +
				"{" +
					"lightDirection[i] = normalize(vec3(uLightPosition[i] - eyeCoordinates));" +
					"reflectionVector[i] = reflect(-lightDirection[i],tranformedNormals);" +
					"vec3 viewerVector = normalize(-eyeCoordinates.xyz);" +
					"ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;" +
					"diffuseLight[i] = uLightDiffuse[i] * uMaterialDiffuse * max(dot(lightDirection[i], tranformedNormals),0.0);" +
					"specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i], viewerVector), 0.0), uMaterialShineness);" +
					"oPhong_ADS_Light = oPhong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" +
				"}" +
			"}" +
			"else" +
			"{" +
				"oPhong_ADS_Light = vec3(0.0f, 0.0f, 0.0f);" +
			"}" +
			"gl_Position = (uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition);" +
			"}"
		);

		int vertexShaderObject_PV = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		GLES32.glShaderSource(vertexShaderObject_PV, vertexShaderSourceCode_PV);
		GLES32.glCompileShader(vertexShaderObject_PV);

		int[] shaderCompileStatus = new int[1];
		int[] infoLogLength = new int[1];
		String infoLog = null;

		GLES32.glGetShaderiv(vertexShaderObject_PV, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		if (shaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject_PV,GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_PV);
				System.out.println("Vertex shader per vertex compilation error log: " + infoLog);
				uninitialize_Sphere();
				System.exit(0);
			}
		}

		// fragment shader
		final String fragmentShaderSourceCode_PV = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"in vec3 oPhong_ADS_Light;" +
			"uniform mediump int uKeyPress;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
				"if (uKeyPress == 1)" +
				"{" +
					"FragColor = vec4(oPhong_ADS_Light, 1.0);" +
				"}" +
				"else" +
				"{" +
					"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" +
				"}" +
			"}"
		);

		int fragmentShaderObject_PV = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		GLES32.glShaderSource(fragmentShaderObject_PV, fragmentShaderSourceCode_PV);
		GLES32.glCompileShader(fragmentShaderObject_PV);

		shaderCompileStatus[0] = 0;
		infoLogLength[0] = 0;
		infoLog = null;

		GLES32.glGetShaderiv(fragmentShaderObject_PV, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		if (shaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject_PV,GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_PV);
				System.out.println("Fragment shader per vertex compilation error log: " + infoLog);
				uninitialize_Sphere();
				System.exit(0);
			}
		}

		// shader program
		shaderProgramObject_PV_Sphere = GLES32.glCreateProgram();
		GLES32.glAttachShader(shaderProgramObject_PV_Sphere, vertexShaderObject_PV);
		GLES32.glAttachShader(shaderProgramObject_PV_Sphere, fragmentShaderObject_PV);
		GLES32.glBindAttribLocation(shaderProgramObject_PV_Sphere, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
		GLES32.glBindAttribLocation(shaderProgramObject_PV_Sphere, VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
		GLES32.glLinkProgram(shaderProgramObject_PV_Sphere);

		int[] programLinkStatus = new int[1];
		infoLogLength[0] = 0;
		infoLog = null;

		GLES32.glGetProgramiv(shaderProgramObject_PV_Sphere, GLES32.GL_LINK_STATUS, programLinkStatus, 0);
		if (programLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject_PV_Sphere,GLES32. GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetProgramInfoLog(shaderProgramObject_PV_Sphere);
				System.out.println("shader program per vertex Sphere link error log: " + infoLog);
				uninitialize_Sphere();
				System.exit(0);
			}
		}

		ModelMatrixUniform_PV_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uModelMatrix");
		ViewMatrixUniform_PV_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uViewMatrix");
		ProjectionMatrixUniform_PV_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uProjectionMatrix");
		
		LightAmbientUniform_PV_Sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightAmbient[0]");
		LightDiffuseUniform_PV_Sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightDiffuse[0]");
		LightSpecularUniform_PV_Sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightSpecular[0]");
		LightPositionUniform_PV_Sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightPosition[0]");
		
		LightAmbientUniform_PV_Sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightAmbient[1]");
		LightDiffuseUniform_PV_Sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightDiffuse[1]");
		LightSpecularUniform_PV_Sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightSpecular[1]");
		LightPositionUniform_PV_Sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightPosition[1]");

		LightAmbientUniform_PV_Sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightAmbient[2]");
		LightDiffuseUniform_PV_Sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightDiffuse[2]");
		LightSpecularUniform_PV_Sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightSpecular[2]");
		LightPositionUniform_PV_Sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uLightPosition[2]");

		MaterialAmbientUniform_PV_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uMaterialAmbient");
		MaterialDiffuseUniform_PV_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uMaterialDiffuse");
		MaterialSpecularUniform_PV_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uMaterialSpecular");
		MaterialShinenessUniform_PV_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uMaterialShineness");
		keyPressUniform_PV_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PV_Sphere, "uKeyPress");
		
		/*////////////////////////////////////////////////////////////////////////////////////// ////////////////////////////////////*/
		/**********************************************************PER FRAGMENT *******************************************************/
		/*////////////////////////////////////////////////////////////////////////////////////// ////////////////////////////////////*/
		// vertex shader
		final String vertexShaderSourceCode_PF = String.format
		(	"#version 320 es"+
			"\n"+
			"precision highp float;" +
			"in vec4 aPosition;" +
			"in vec3 aNormal;" +
			"uniform vec4 uLightPosition[3];" +
			"uniform mat4 uModelMatrix;" +
			"uniform mat4 uViewMatrix;" +
			"uniform mat4 uProjectionMatrix;" +
			"uniform mediump int uKeyPress;" +
			"out vec3 oTransformedNormals;" +
			"out vec3 oLightDirection[3];" +
			"out vec3 oViewerVector;" +
			"void main(void)" +
			"{" +
				"if (uKeyPress == 1)" +
				"{" +
					"vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;" +
					"oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;" +
					"oViewerVector = (-eyeCoordinates.xyz);" +
					"for (int i = 0; i < 3; i++)" +
					"{" +
						"oLightDirection[i] = vec3(uLightPosition[i] - eyeCoordinates);" +
					"}" +
				"}" +
				"else" +
				"{" +
					"oTransformedNormals = vec3(0.0f, 0.0f, 0.0f);" +
					"oLightDirection[0] = vec3(0.0f, 0.0f, 0.0f);" +
					"oLightDirection[1] = vec3(0.0f, 0.0f, 0.0f);" +
					"oLightDirection[2] = vec3(0.0f, 0.0f, 0.0f);" +
					"oViewerVector = vec3(0.0f, 0.0f, 0.0f);" +
				"}" +
				"gl_Position = (uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition);" +
			"}"
		);

		int vertexShaderObject_PF = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		GLES32.glShaderSource(vertexShaderObject_PF, vertexShaderSourceCode_PF);
		GLES32.glCompileShader(vertexShaderObject_PF);

		shaderCompileStatus = new int[1];
		infoLogLength = new int[1];
		infoLog = null;

		GLES32.glGetShaderiv(vertexShaderObject_PF, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		if (shaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject_PF,GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_PF);
				System.out.println("Vertex shader PF Sphere compilation error log: " + infoLog);
				uninitialize_Sphere();
				System.exit(0);
			}
		}

		// fragment shader
		final String fragmentShaderSourceCode_PF = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"in vec3 oTransformedNormals;" +
			"in vec3 oLightDirection[3];" +
			"in vec3 oViewerVector;" +
			"uniform vec3 uLightAmbient[3];" +
			"uniform vec3 uLightDiffuse[3];" +
			"uniform vec3 uLightSpecular[3];" +
			"uniform vec3 uMaterialAmbient;" +
			"uniform vec3 uMaterialDiffuse;" +
			"uniform vec3 uMaterialSpecular;" +
			"uniform float uMaterialShineness;" +
			"uniform mediump int uKeyPress;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
				"vec3 Phong_ADS_Light = vec3 (0.0, 0.0, 0.0);" +
				"if (uKeyPress == 1)" +
				"{" +
					"for (int i = 0; i < 3; i++)" +
					"{" +
						"vec3 normalizedLightDirection[3];" +
						"vec3 reflectionVector[3];" +
						"vec3 ambientLight[3];" +
						"vec3 diffuseLight[3];" +
						"vec3 specularLight[3];" +
						"vec3 normalizedTransformedNormals = normalize(oTransformedNormals);" +
						"normalizedLightDirection[i] = normalize(oLightDirection[i]);" +
						"vec3 normalizedViewerVector = normalize(oViewerVector);" +
						"ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;" +
						"diffuseLight[i] = uLightDiffuse[i] * uMaterialDiffuse * max(dot(normalizedLightDirection[i], normalizedTransformedNormals),0.0);" +
						"reflectionVector[i] = reflect(- normalizedLightDirection[i], normalizedTransformedNormals);" +
						"specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i], normalizedViewerVector), 0.0), uMaterialShineness);" +
						"Phong_ADS_Light = Phong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" +
					"}" +
				"}" +
				"else" +
				"{" +
					"Phong_ADS_Light = vec3(1.0, 1.0,1.0);" +
				"}" +
				"FragColor = vec4(Phong_ADS_Light, 1.0);" +
			"}"
		);

		int fragmentShaderObject_PF = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		GLES32.glShaderSource(fragmentShaderObject_PF, fragmentShaderSourceCode_PF);
		GLES32.glCompileShader(fragmentShaderObject_PF);

		shaderCompileStatus[0] = 0;
		infoLogLength[0] = 0;
		infoLog = null;

		GLES32.glGetShaderiv(fragmentShaderObject_PF, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
		if (shaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject_PF,GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_PF);
				System.out.println("Fragment shader PF Spherecompilation error log: " + infoLog);
				uninitialize_Sphere();
				System.exit(0);
			}
		}

		// shader program
		shaderProgramObject_PF_Sphere = GLES32.glCreateProgram();
		GLES32.glAttachShader(shaderProgramObject_PF_Sphere, vertexShaderObject_PF);
		GLES32.glAttachShader(shaderProgramObject_PF_Sphere, fragmentShaderObject_PF);
		GLES32.glBindAttribLocation(shaderProgramObject_PF_Sphere, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
		GLES32.glBindAttribLocation(shaderProgramObject_PF_Sphere, VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
		GLES32.glLinkProgram(shaderProgramObject_PF_Sphere);

		programLinkStatus = new int[1];
		infoLogLength[0] = 0;
		infoLog = null;

		GLES32.glGetProgramiv(shaderProgramObject_PF_Sphere, GLES32.GL_LINK_STATUS, programLinkStatus, 0);
		if (programLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject_PF_Sphere,GLES32. GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				infoLog = GLES32.glGetProgramInfoLog(shaderProgramObject_PF_Sphere);
				System.out.println("shader program PF Sphere link error log: " + infoLog);
				uninitialize_Sphere();
				System.exit(0);
			}
		}

		ModelMatrixUniform_PF_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uModelMatrix");
		ViewMatrixUniform_PF_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uViewMatrix");
		ProjectionMatrixUniform_PF_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uProjectionMatrix");
		
		LightAmbientUniform_PF_Sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightAmbient[0]");
		LightDiffuseUniform_PF_Sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightDiffuse[0]");
		LightSpecularUniform_PF_Sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightSpecular[0]");
		LightPositionUniform_PF_Sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightPosition[0]");
		
		LightAmbientUniform_PF_Sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightAmbient[1]");
		LightDiffuseUniform_PF_Sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightDiffuse[1]");
		LightSpecularUniform_PF_Sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightSpecular[1]");
		LightPositionUniform_PF_Sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightPosition[1]");

		LightAmbientUniform_PF_Sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightAmbient[2]");
		LightDiffuseUniform_PF_Sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightDiffuse[2]");
		LightSpecularUniform_PF_Sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightSpecular[2]");
		LightPositionUniform_PF_Sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uLightPosition[2]");

		MaterialAmbientUniform_PF_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uMaterialAmbient");
		MaterialDiffuseUniform_PF_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uMaterialDiffuse");
		MaterialSpecularUniform_PF_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uMaterialSpecular");
		MaterialShinenessUniform_PF_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uMaterialShineness");
		keyPressUniform_PF_Sphere = GLES32.glGetUniformLocation(shaderProgramObject_PF_Sphere, "uKeyPress");
		
		/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////// */
		
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
		GLES32.glGenVertexArrays(1, vao_Sphere, 0);
		GLES32.glBindVertexArray(vao_Sphere[0]);

		//position vbo
		GLES32.glGenBuffers(1, vbo_position_Sphere, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_Sphere[0]);

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
        GLES32.glGenBuffers(1,vbo_normal_Sphere, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal_Sphere[0]);
        
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
        GLES32.glGenBuffers(1,vbo_element_Sphere, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_Sphere[0]);
        
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

		// clear color
		GLES32.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		// light array initialiasation
		light[0] = new Light();
		light[0].ambient[0] = 0.0f;
		light[0].ambient[1] = 0.0f;
		light[0].ambient[2] = 0.0f;
		
		light[0].diffuse[0] = 1.0f;
		light[0].diffuse[1] = 0.0f;
		light[0].diffuse[2] = 0.0f;

		light[0].specular[0] = 1.0f;
		light[0].specular[1] = 0.0f;
		light[0].specular[2] = 0.0f;
		//light[0].position[4];
		
		light[1] = new Light();
		light[1].ambient[0] = 0.0f;
		light[1].ambient[1] = 0.0f;
		light[1].ambient[2] = 0.0f;
		
		light[1].diffuse[0] = 0.0f;
		light[1].diffuse[1] = 1.0f;
		light[1].diffuse[2] = 0.0f;

		light[1].specular[0] = 0.0f;
		light[1].specular[1] = 1.0f;
		light[1].specular[2] = 0.0f;
		//light[1].position[4];

		light[2] = new Light();
		light[2].ambient[0] = 0.0f;
		light[2].ambient[1] = 0.0f;
		light[2].ambient[2] = 0.0f;
		
		light[2].diffuse[0] = 0.0f;
		light[2].diffuse[1] = 0.0f;
		light[2].diffuse[2] = 1.0f;

		light[2].specular[0] = 0.0f;
		light[2].specular[1] = 0.0f;
		light[2].specular[2] = 1.0f;
		//light[2].position[4];

		Matrix.setIdentityM(perspectiveProjectionMatrix_Sphere, 0);
		
		// warmup resize
		resize_Sphere(WIN_WIDTH, WIN_HEIGHT);

		return(true);
	}

	private void printGLInfo(GL10 gl)
	{
		// code
		System.out.println("OpenGL-ES Renderer : " + gl.glGetString(GL10.GL_RENDERER));
		System.out.println("OpenGL-ES Version : " + gl.glGetString(GL10.GL_VERSION));
		System.out.println("OpenGL-ES Shading Langauge Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
	}

	private boolean createFBO(int textureWidth, int textureHeight)
	{
		// variable
		int[] maxRenderBufferSize= new int[1];

		//check capacity of render buffer
		GLES32.glGetIntegerv(GLES32.GL_MAX_RENDERBUFFER_SIZE, maxRenderBufferSize, 0);

		if (maxRenderBufferSize[0] < textureWidth || maxRenderBufferSize[0] < textureHeight)
		{
			System.out.println("texture size overflow \n");				
			return(false);
		}
		
		// Create custom frame Buffer
		GLES32.glGenFramebuffers(1, FBO, 0);
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, FBO[0]);

		// create texture for FBO in which we are going to render the sphere(second scene)
		GLES32.glGenTextures(1, textureFBO, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, textureFBO[0]);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_CLAMP_TO_EDGE);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_CLAMP_TO_EDGE);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexImage2D(GLES32.GL_TEXTURE_2D, 0, GLES32.GL_RGB, textureWidth, textureHeight, 0, GLES32.GL_RGB, GLES32.GL_UNSIGNED_SHORT_5_6_5, null);
		
		// attached above texture to frame buffer at defalut color attachement 0
		GLES32.glFramebufferTexture2D(GLES32.GL_FRAMEBUFFER, GLES32.GL_COLOR_ATTACHMENT0, GLES32.GL_TEXTURE_2D, textureFBO[0], 0);

		// now create render buffer to hold depth of custom FBO
		GLES32.glGenRenderbuffers(1, RBO, 0);
		GLES32.glBindRenderbuffer(GLES32.GL_RENDERBUFFER, RBO[0]);

		// set the storage of above render buffer of texture size for depth
		GLES32.glRenderbufferStorage(GLES32.GL_RENDERBUFFER, GLES32.GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

		// attach above depth related renderbuffer to FBO at depth attachment
		GLES32.glFramebufferRenderbuffer(GLES32.GL_FRAMEBUFFER, GLES32.GL_DEPTH_ATTACHMENT, GLES32.GL_RENDERBUFFER, RBO[0]);

		// check the frame buffer status whether successfull or not
		if (GLES32.glCheckFramebufferStatus(GLES32.GL_FRAMEBUFFER) != GLES32.GL_FRAMEBUFFER_COMPLETE)
		{
			System.out.println("frame buffer status is not complete\n ");
				
			return(false);
		}

		// unbind with the frame buffer
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);

		return(true);
	}

	private void resize_Cube(int width, int height)
	{
		// code
		if (height <= 0)
		{
			height = 1;
		}
		// initialise width and height
		winWidth = width;
		winHeight = height;

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix_Cube, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void resize_Sphere(int width, int height)
	{
		// code
		if (height <= 0)
		{
			height = 1;
		}
		
		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix_Sphere, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void display_Cube()
	{
		// code
		// Render FBO scene
		if (bFboResult == true)
		{
			display_Sphere(FBO_WIDTH, FBO_HEIGHT);
			update_Sphere();
		}

		// call resize_Cube() again to compensate the change done by the display_Sphere()
		resize_Cube(winWidth, winHeight);
	
		// reset the background color to white to compensate the change done by display_Sphere()
		GLES32.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject_Cube);

		// transformation		
		//-----------------Cube-----------------------------------
		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];
		float[] translationMatrix = new float[16];
		float[] rotationMatrix = new float[16];
		float[] rotationMatrixX = new float[16];
		float[] rotationMatrixY = new float[16];
		float[] rotationMatrixZ = new float[16];
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);
		Matrix.setIdentityM(rotationMatrixX, 0);
		Matrix.setIdentityM(rotationMatrixY, 0);
		Matrix.setIdentityM(rotationMatrixZ, 0);
		Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.setRotateM(rotationMatrixX, 0, cube_angle, 1.0f, 0.0f, 0.0f);
		Matrix.setRotateM(rotationMatrixY, 0, cube_angle, 0.0f, 1.0f, 0.0f);
		Matrix.setRotateM(rotationMatrixZ, 0, cube_angle, 0.0f, 0.0f, 1.0f);
		Matrix.multiplyMM(rotationMatrix, 0, rotationMatrixX, 0, rotationMatrixY, 0);
		Matrix.multiplyMM(rotationMatrix, 0, rotationMatrix, 0, rotationMatrixZ, 0);
		
		Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix_Cube, 0, modelViewMatrix, 0);
		GLES32.glUniformMatrix4fv(mvpMatrixUniform_Cube, 1, false, modelViewProjectionMatrix, 0);

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, textureFBO[0]); // bind texture
		GLES32.glUniform1i(textureSamplerUniform_Cube, 0);

		GLES32.glBindVertexArray(vao_Cube[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

		GLES32.glUseProgram(0);

		// Render
		requestRender();
	}

	private void display_Sphere(int textureWidth, int textureHeight)
	{
		// code
		// bind with FBO
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, FBO[0]);

		// call resize sphere
		resize_Sphere(textureWidth, textureHeight);

		// set the clear color to black to compensate the change done by display_Cube
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// transformation
		float[] modelMatrix = new float[16];
		float[] viewMatrix = new float[16];
		float[] translationMatrix = new float[16];
		Matrix.setIdentityM(modelMatrix,0);
		Matrix.setIdentityM(translationMatrix,0);
		Matrix.setIdentityM(viewMatrix,0);

		Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -3.0f);
		modelMatrix = translationMatrix;

		if (choosenShader == 1)
		{	
			GLES32.glUseProgram(shaderProgramObject_PF_Sphere);
			GLES32.glUniformMatrix4fv(ModelMatrixUniform_PF_Sphere, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(ViewMatrixUniform_PF_Sphere, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(ProjectionMatrixUniform_PF_Sphere, 1, false, perspectiveProjectionMatrix_Sphere, 0);
		}
		else if (choosenShader == 2)
		{
			GLES32.glUseProgram(shaderProgramObject_PV_Sphere);
			GLES32.glUniformMatrix4fv(ModelMatrixUniform_PV_Sphere, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(ViewMatrixUniform_PV_Sphere, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(ProjectionMatrixUniform_PV_Sphere, 1, false, perspectiveProjectionMatrix_Sphere, 0);
		}
		if (choosenShader != 2 && choosenShader != 1)
		{
			GLES32.glUseProgram(shaderProgramObject_PV_Sphere);
			GLES32.glUniformMatrix4fv(ModelMatrixUniform_PV_Sphere, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(ViewMatrixUniform_PV_Sphere, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(ProjectionMatrixUniform_PV_Sphere, 1, false, perspectiveProjectionMatrix_Sphere, 0);
		}


		if (bLightingEnable == true)
		{		
			if (choosenShader == 1)		
			{
				GLES32.glUniform1i(keyPressUniform_PF_Sphere, 1);
				GLES32.glUniform3fv(LightAmbientUniform_PF_Sphere[0], 1, light[0].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PF_Sphere[0], 1, light[0].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PF_Sphere[0], 1, light[0].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PF_Sphere[0], 1, light[0].position, 0);

				GLES32.glUniform3fv(LightAmbientUniform_PF_Sphere[1], 1, light[1].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PF_Sphere[1], 1, light[1].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PF_Sphere[1], 1, light[1].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PF_Sphere[1], 1, light[1].position, 0);

				GLES32.glUniform3fv(LightAmbientUniform_PF_Sphere[2], 1, light[2].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PF_Sphere[2], 1, light[2].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PF_Sphere[2], 1, light[2].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PF_Sphere[2], 1, light[2].position, 0);

				GLES32.glUniform3fv(MaterialAmbientUniform_PF_Sphere, 1, materialAmbient, 0);
				GLES32.glUniform3fv(MaterialDiffuseUniform_PF_Sphere, 1, materialDiffuse, 0);
				GLES32.glUniform3fv(MaterialSpecularUniform_PF_Sphere, 1, materialSpecular, 0);
				GLES32.glUniform1f(MaterialShinenessUniform_PF_Sphere, materialShineness);
			}
			if (choosenShader == 2)
			{
				GLES32.glUniform1i(keyPressUniform_PV_Sphere, 1);
				GLES32.glUniform3fv(LightAmbientUniform_PV_Sphere[0], 1, light[0].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PV_Sphere[0], 1, light[0].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PV_Sphere[0], 1, light[0].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PV_Sphere[0], 1, light[0].position, 0);

				GLES32.glUniform3fv(LightAmbientUniform_PV_Sphere[1], 1, light[1].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PV_Sphere[1], 1, light[1].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PV_Sphere[1], 1, light[1].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PV_Sphere[1], 1, light[1].position, 0);

				GLES32.glUniform3fv(LightAmbientUniform_PV_Sphere[2], 1, light[2].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PV_Sphere[2], 1, light[2].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PV_Sphere[2], 1, light[2].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PV_Sphere[2], 1, light[2].position, 0);

				GLES32.glUniform3fv(MaterialAmbientUniform_PV_Sphere, 1, materialAmbient, 0);
				GLES32.glUniform3fv(MaterialDiffuseUniform_PV_Sphere, 1, materialDiffuse, 0);
				GLES32.glUniform3fv(MaterialSpecularUniform_PV_Sphere, 1, materialSpecular, 0);
				GLES32.glUniform1f(MaterialShinenessUniform_PV_Sphere, materialShineness);
			}
			if (choosenShader != 2 && choosenShader != 1)
			{
				GLES32.glUniform1i(keyPressUniform_PV_Sphere, 1);
				GLES32.glUniform3fv(LightAmbientUniform_PV_Sphere[0], 1, light[0].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PV_Sphere[0], 1, light[0].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PV_Sphere[0], 1, light[0].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PV_Sphere[0], 1, light[0].position, 0);

				GLES32.glUniform3fv(LightAmbientUniform_PV_Sphere[1], 1, light[1].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PV_Sphere[1], 1, light[1].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PV_Sphere[1], 1, light[1].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PV_Sphere[1], 1, light[1].position, 0);

				GLES32.glUniform3fv(LightAmbientUniform_PV_Sphere[2], 1, light[2].ambient, 0);
				GLES32.glUniform3fv(LightDiffuseUniform_PV_Sphere[2], 1, light[2].diffuse, 0);
				GLES32.glUniform3fv(LightSpecularUniform_PV_Sphere[2], 1, light[2].specular, 0);
				GLES32.glUniform4fv(LightPositionUniform_PV_Sphere[2], 1, light[2].position, 0);

				GLES32.glUniform3fv(MaterialAmbientUniform_PV_Sphere, 1, materialAmbient, 0);
				GLES32.glUniform3fv(MaterialDiffuseUniform_PV_Sphere, 1, materialDiffuse, 0);
				GLES32.glUniform3fv(MaterialSpecularUniform_PV_Sphere, 1, materialSpecular, 0);
				GLES32.glUniform1f(MaterialShinenessUniform_PV_Sphere, materialShineness);
			}						
		}
		else
		{
			GLES32.glUniform1i(keyPressUniform_PV_Sphere, 0);
			GLES32.glUniform1i(keyPressUniform_PF_Sphere, 0);
		}

		// bind vao
        GLES32.glBindVertexArray(vao_Sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_Sphere[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		// unbind with the framebuffer
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);
	}

	private void update_Cube()
	{
		// Rectangle Rotate
		cube_angle = cube_angle - 1.0f;
		if (cube_angle <= 360.0f)
		{
			cube_angle = cube_angle + 360.0f;
		}
	}

	private void update_Sphere()
	{
		// Code	
		if (bLightingEnable == true)
		{
			// Animating light 0
			light[0].position[0] = 0.0f;// x rotation by rule
			light[0].position[1] = 5.0f * (float)Math.sin(lightAngleZero); // precision
			light[0].position[2] = 5.0f * (float)Math.cos(lightAngleZero); // light animate value
			light[0].position[3] = 1.0f; // positional light

			lightAngleZero = lightAngleZero + 0.05f;
			if (lightAngleZero > 360.0f)
			{
				lightAngleZero = lightAngleZero - 360.0f;
			}

			// Animating light 1
			light[1].position[0] = 5.0f * (float)Math.cos(lightAngleOne); // light animate value
			light[1].position[1] = 0.0f; // y rotation by rule
			light[1].position[2] = 5.0f * (float)Math.sin(lightAngleOne); // precision
			light[1].position[3] = 1.0f; // positional light 

			lightAngleOne = lightAngleOne + 0.05f;
			if (lightAngleOne > 360.0f)
			{
				lightAngleOne = lightAngleOne - 360.0f;
			}

			// Animating light 2
			light[2].position[0] = 5.0f * (float)Math.cos(lightAngleTwo);
			light[2].position[1] =5.0f * (float)Math.sin(lightAngleTwo); // light animate value
			light[2].position[2] = 0.0f; // z rotation by rule
			light[2].position[3] = 1.0f; // positional light 

			lightAngleTwo = lightAngleTwo + 0.05f;
			if (lightAngleTwo > 360.0f)
			{
				lightAngleTwo = lightAngleTwo - 360.0f;
			}		
		}
	}

	private void uninitialize_Cube()
	{
		// code
		if (shaderProgramObject_Cube > 0)
		{
			GLES32.glUseProgram(shaderProgramObject_Cube);
			int[] ratVal = new int[1];
			GLES32.glGetProgramiv(shaderProgramObject_Cube,GLES32.GL_ATTACHED_SHADERS, ratVal, 0);

			if (ratVal[0] > 0)
			{
				int numAttachedShader = ratVal[0];
				int[] shaderObjects = new int[numAttachedShader];

				GLES32.glGetAttachedShaders(shaderProgramObject_Cube,numAttachedShader, ratVal, 0, shaderObjects, 0);

				for (int i = 0; i < numAttachedShader; i++)
				{
					GLES32.glDetachShader(shaderProgramObject_Cube, shaderObjects[i]);
					GLES32.glDeleteShader(shaderObjects[i]);
					shaderObjects[i] = 0;
				}
			}			
			GLES32.glUseProgram(0);
			GLES32.glDeleteProgram(shaderProgramObject_Cube);
			shaderProgramObject_Cube = 0;
		}

		//---------------Square-------------------------
		if (vbo_texcoord_Cube[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_texcoord_Cube, 0);
			vbo_texcoord_Cube[0] = 0;
		}

		if (textureFBO[0] > 0)
		{
			GLES32.glDeleteTextures(1, textureFBO, 0);
			textureFBO[0] = 0;
		}
		if (vbo_position_Cube[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_Cube, 0);
			vbo_position_Cube[0] = 0;
		}

		if (vao_Cube[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_Cube, 0);
			vao_Cube[0] = 0;
		}	
		
		// Delete RBO
		if (RBO[0] > 0)
		{
			GLES32.glDeleteRenderbuffers(1, RBO, 0);
			RBO[0] = 0;
		}

		// Delete FBO
		if (FBO[0] > 0)
		{
			GLES32.glDeleteFramebuffers(1, FBO, 0);
			FBO[0] = 0;
		}
	}	

	private void uninitialize_Sphere()
	{
		// code
		//pv
		if (shaderProgramObject_PV_Sphere > 0)
		{
			GLES32.glUseProgram(shaderProgramObject_PV_Sphere);
			int[] ratVal = new int[1];
			GLES32.glGetProgramiv(shaderProgramObject_PV_Sphere,GLES32.GL_ATTACHED_SHADERS, ratVal, 0);

			if (ratVal[0] > 0)
			{
				int numAttachedShader = ratVal[0];
				int[] shaderObjects = new int[numAttachedShader];

				GLES32.glGetAttachedShaders(shaderProgramObject_PV_Sphere,numAttachedShader, ratVal, 0, shaderObjects, 0);

				for (int i = 0; i < numAttachedShader; i++)
				{
					GLES32.glDetachShader(shaderProgramObject_PV_Sphere, shaderObjects[i]);
					GLES32.glDeleteShader(shaderObjects[i]);
					shaderObjects[i] = 0;
				}
			}			
			GLES32.glUseProgram(0);
			GLES32.glDeleteProgram(shaderProgramObject_PV_Sphere);
			shaderProgramObject_PV_Sphere = 0;
		}

		//pF
		if (shaderProgramObject_PF_Sphere > 0)
		{
			GLES32.glUseProgram(shaderProgramObject_PF_Sphere);
			int[] ratVal = new int[1];
			GLES32.glGetProgramiv(shaderProgramObject_PF_Sphere,GLES32.GL_ATTACHED_SHADERS, ratVal, 0);

			if (ratVal[0] > 0)
			{
				int numAttachedShader = ratVal[0];
				int[] shaderObjects = new int[numAttachedShader];

				GLES32.glGetAttachedShaders(shaderProgramObject_PF_Sphere,numAttachedShader, ratVal, 0, shaderObjects, 0);

				for (int i = 0; i < numAttachedShader; i++)
				{
					GLES32.glDetachShader(shaderProgramObject_PF_Sphere, shaderObjects[i]);
					GLES32.glDeleteShader(shaderObjects[i]);
					shaderObjects[i] = 0;
				}
			}			
			GLES32.glUseProgram(0);
			GLES32.glDeleteProgram(shaderProgramObject_PF_Sphere);
			shaderProgramObject_PF_Sphere = 0;
		}

		// destroy vao
        if(vao_Sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_Sphere, 0);
            vao_Sphere[0]=0;
        }
        
        // destroy position vbo
        if(vbo_position_Sphere[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_position_Sphere, 0);
            vbo_position_Sphere[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_normal_Sphere[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_normal_Sphere, 0);
            vbo_normal_Sphere[0]=0;
        }
        
        // destroy element vbo
        if(vbo_element_Sphere[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_element_Sphere, 0);
            vbo_element_Sphere[0]=0;
        }
	}
}
