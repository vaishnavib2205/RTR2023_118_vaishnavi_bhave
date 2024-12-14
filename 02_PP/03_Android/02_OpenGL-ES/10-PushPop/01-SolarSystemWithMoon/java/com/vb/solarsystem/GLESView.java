package com.vb.solarsystem;

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
	
	private int shaderProgramObject;
	
	// Sphere
	private int[] vao_sun = new int[1];
    private int[] vbo_sun_position = new int[1];
    private int[] vbo_sun_normal = new int[1];
    private int[] vbo_sun_texCoord = new int[1];
    private int[] vbo_sun_element = new int[1];
	private int[] sun_texture = new int[1];

	
	private int[] vao_earth = new int[1];
    private int[] vbo_earth_position = new int[1];
    private int[] vbo_earth_normal = new int[1];
    private int[] vbo_earth_texCoord = new int[1];
    private int[] vbo_earth_element = new int[1];
	private int[] earth_texture = new int[1];

	
	private int[] vao_moon = new int[1];
    private int[] vbo_moon_position = new int[1];
    private int[] vbo_moon_normal = new int[1];
    private int[] vbo_moon_texCoord = new int[1];
    private int[] vbo_moon_element = new int[1];
	private int[] moon_texture = new int[1];

	int numVertices;
    int numElements;

	private int textureSamplerUniform;
	private int mvpMatrixUniform;
	private float[] perspectiveProjectionMatrix = new float[16];
	
	private final Context context;
	
	Sphere sphere=new Sphere();
    float sphere_vertices[]=new float[1146];
    float sphere_normals[]=new float[1146];
    float sphere_textures[]=new float[764];
    short sphere_elements[]=new short[2280];

	float[][] stack = new float[32][16];
	int top = -1;

	int day;
	int year;
	int moonRev;
	int moonRot;

	boolean right = false;
	boolean left = true;
	boolean moon = false;

	public GLESView(Context _context)
	{
		super(_context);
			
		context = _context;
		
		// OpenGLES Related
		setEGLContextClientVersion(3);
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		gestureDetector = new GestureDetector(_context,this,null,false);
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
		if(right == false)
		{
			right = true;
			left = false;
		}
		else
		{
			right = false;
			left = true;
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
		if(moon == true)
		{
			moon = false;
		}
		else
		{
			moon = true;
		}
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
		if(moon == false)
		{
			if(left == true)
			{
				day = (day + 6) % 360;
				year = (year + 6) % 360;
			}
			else
			{
				day = (day - 6) % 360;			
				year = (year - 6) % 360;			
			}
		}
		else
		{
			if(left == true)
			{
				moonRev = (moonRev + 6) % 360;
				moonRot = (moonRot + 6) % 360;
			}
			else
			{
				moonRev = (moonRev - 6) % 360;
				moonRot = (moonRot - 6) % 360;
			}
		}
		return(true);	
	}
	

	// Implementation of private method
	private void initialize(GL10 gl)
	{
		
		
		// Code
		// Print info
		printGLInfo(gl);

		// Vertex Shader
		final String vertexShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"in vec4 aPosition;"+
			"in vec2 aTexCoord;"+
			"out vec2 oTexCoord;"+
			"uniform mat4 uMVPMatrix;"+
			"void main(void)"+
			"{"+
			"gl_Position = uMVPMatrix * aPosition;"+
			"oTexCoord = aTexCoord;"+
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
			"uniform highp sampler2D uTextureSampler;"+
			"in vec2 oTexCoord;"+
			"out vec4 fragColor;"+
			"void main(void)"+
			"{"+
			"fragColor = texture(uTextureSampler, oTexCoord);"+
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
		GLES32.glBindAttribLocation(shaderProgramObject, vertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");
	
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
		textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uTextureSampler");
	
	
		sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();
		
		// *************************************************************************************************************************************************************************************
		// ********************************************************  SPHERE  ************************************************************************************************************
		// *************************************************************************************************************************************************************************************
		initializeSphere(vao_sun,vbo_sun_position, vbo_sun_normal, vbo_sun_element, sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		initializeSphere(vao_earth,vbo_earth_position, vbo_earth_normal, vbo_earth_element, sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		initializeSphere(vao_moon,vbo_moon_position, vbo_moon_normal, vbo_moon_element, sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		
		Matrix.setIdentityM(perspectiveProjectionMatrix,0);

		// load texture
		GLES32.glEnable(GLES32.GL_TEXTURE_2D);
		sun_texture[0]=loadGLTexture(R.raw.sun);
		earth_texture[0]=loadGLTexture(R.raw.earth);
		moon_texture[0]=loadGLTexture(R.raw.moon);


		// Depth enable
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);


		// Clear color
		GLES32.glClearColor(0.0f,0.0f,0.0f,1.0f);
	}
	private int loadGLTexture(int imageResId)
	{
		// create bitmap factory options object
		BitmapFactory.Options options = new BitmapFactory.Options();
		
		// dont scale the image
		options.inScaled=false;
		
		// create bitmap image from image resource
		Bitmap bitmap=BitmapFactory.decodeResource(context.getResources(), imageResId,options);

		// create texture
		int[] texture=new int[1];
		GLES32.glGenTextures(1,texture,0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,texture[0]);
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT,
			1);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,GLES32.GL_TEXTURE_MAG_FILTER,
			GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,GLES32.GL_TEXTURE_MIN_FILTER,
			GLES32.GL_LINEAR_MIPMAP_LINEAR);
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D,0,bitmap,
			0);
		GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,0);

		return texture[0];
	}

	private void initializeSphere(int[] vao_sphere,int[] vbo_sphere_position,int[] vbo_sphere_normal,int[] vbo_sphere_element,float sphere_vertices[], float sphere_normals[], float sphere_textures[], short sphere_elements[])
	{

		// Sphere vao
        GLES32.glGenVertexArrays(1,vao_sphere,0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // Position vbo
        GLES32.glGenBuffers(1,vbo_sphere_position,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(vertexAttributesEnum.AMC_ATTRIBUTE_POSITION,
                                        3,
                                        GLES32.GL_FLOAT,
                                        false,0,0);
        
        GLES32.glEnableVertexAttribArray(vertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		// texcoord vbo
		GLES32.glGenBuffers(1, vbo_sun_texCoord, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sun_texCoord[0]);
		
		ByteBuffer tByteBufferS = ByteBuffer.allocateDirect(sphere_textures.length * 4);
		tByteBufferS.order(ByteOrder.nativeOrder());
		FloatBuffer textureBuffer = tByteBufferS.asFloatBuffer();
		textureBuffer.put(sphere_textures);
		textureBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,	 sphere_textures.length * 4 , textureBuffer, GLES32.GL_STATIC_DRAW);
	
		GLES32.glVertexAttribPointer(vertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD,
			2,
			GLES32.GL_FLOAT,
			false,
			0,
			0);
		GLES32.glEnableVertexAttribArray(vertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

     
        // Normal vbo
        GLES32.glGenBuffers(1,vbo_sphere_normal,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(vertexAttributesEnum.AMC_ATTRIBUTE_NORMALS,
                                        3,
                                        GLES32.GL_FLOAT,
                                        false,0,0);
        
        GLES32.glEnableVertexAttribArray(vertexAttributesEnum.AMC_ATTRIBUTE_NORMALS);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // Element vbo
        GLES32.glGenBuffers(1,vbo_sphere_element,0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);


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

	private void push(float[] matrix)
	{
		top++;
		
		for(int i = 0;i < 16;i++)
		{
			stack[top][i] = matrix[i];
		}

	}
	private float[] pop()
	{
		float[] temp = new float[16];

		for(int i = 0;i < 16;i++)
		{
			temp[i] = stack[top][i];
		}
		top--;
		return(temp);
	}
	private void display(){
		
		// Code
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		// Render
		// Transformation

		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];

		// *************************************************************************************************************************************************************************************
		// ********************************************************  SUN  ************************************************************************************************************
		// *************************************************************************************************************************************************************************************
	
		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);

		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -3.0f);
		push(modelViewMatrix);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0); 
		
			// For texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, sun_texture[0]);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		GLES32.glBindVertexArray(vao_sun[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sun_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        GLES32.glBindVertexArray(0); 
		modelViewMatrix = pop();
		
		// *************************************************************************************************************************************************************************************
		// ********************************************************  Earth  ************************************************************************************************************
		// *************************************************************************************************************************************************************************************
	
		Matrix.rotateM(modelViewMatrix, 0, (float)year, 0.0f, 1.0f, 0.0f);
		push(modelViewMatrix);
		Matrix.translateM(modelViewMatrix, 0, 1.5f, 0.0f, 0.0f);
		Matrix.rotateM(modelViewMatrix, 0, (float)day, 0.0f, 1.0f, 0.0f);
		Matrix.scaleM(modelViewMatrix, 0, 0.3f,0.31f,0.3f);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0); 
		
		// For texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, earth_texture[0]);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		GLES32.glBindVertexArray(vao_earth[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_earth_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        GLES32.glBindVertexArray(0); 

		// *************************************************************************************************************************************************************************************
		// ********************************************************  MOON  ************************************************************************************************************
		// *************************************************************************************************************************************************************************************
	
		Matrix.rotateM(modelViewMatrix, 0, (float)moonRev, 0.0f, 1.0f, 0.0f);
		push(modelViewMatrix);
		Matrix.translateM(modelViewMatrix, 0, 1.5f, 0.0f, 0.0f);
		Matrix.rotateM(modelViewMatrix, 0, (float)moonRot, 0.0f, 1.0f, 0.0f);
		Matrix.scaleM(modelViewMatrix, 0, 0.3f,0.3f,0.3f);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0); 
		
		// For texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, moon_texture[0]);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		GLES32.glBindVertexArray(vao_earth[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_earth_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        GLES32.glBindVertexArray(0); 
		modelViewMatrix = pop();
		modelViewMatrix = pop();
		
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

		if (earth_texture[0] > 0)
		{
			GLES32.glDeleteTextures(1, earth_texture, 0);
			earth_texture[0] = 0;
		}
        if(vao_earth[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_earth, 0);
            vao_earth[0]=0;
        }
        
        if(vbo_earth_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_earth_position, 0);
            vbo_earth_position[0]=0;
        }
        
        if(vbo_earth_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_earth_normal, 0);
            vbo_earth_normal[0]=0;
        }
        
        if(vbo_earth_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_earth_element, 0);
           vbo_earth_element[0]=0;
        }

		if (sun_texture[0] > 0)
		{
			GLES32.glDeleteTextures(1, sun_texture, 0);
			sun_texture[0] = 0;
		}
        if(vao_sun[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sun, 0);
            vao_sun[0]=0;
        }
        
        if(vbo_sun_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sun_position, 0);
            vbo_sun_position[0]=0;
        }
        
        if(vbo_sun_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sun_normal, 0);
            vbo_sun_normal[0]=0;
        }
        
        if(vbo_sun_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sun_element, 0);
            vbo_sun_element[0]=0;
        }

	
	}

}
