package com.vb.lightsphere;

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


public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer,OnDoubleTapListener,OnGestureListener
{
    private GestureDetector gestureDetector;

    private int shaderProgramObject;
    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];

    private int mvpMatrixUniform;
    private int modelViewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;
    private int ldUniform = 0;  // for diffuse light
    private int kdUniform = 0;  // for diffuse material
    private int lightPositionUniform = 0;
    private int keyPressUniform = 0;
    private Boolean bLightingEnabled = false;
    private float angle_Triangle = 0.0f;
    private float[] perspectiveProjectionMatrix  = new float[16];


    final float[] lightAmbient = new float[]{0.5f , 0.5f , 0.5f , 1.0f};
    final float[] lightDiffuse = new float[]{1.0f , 1.0f , 1.0f , 1.0f};
    final float[] materialDiffuse = new float[]{0.5f , 0.5f , 0.5f , 1.0f};
    final float[] lightPosition = new float[]{0.0f , 0.0f , 200.0f , 1.0f};

    private int numVertices;
    private int numElements;




    public GLESView(Context context)
    {
        super(context);

        // OpenGLES releted 

        setEGLContextClientVersion(3); // EGL config kar version 3 la 
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        // create and set gesture detector object

        gestureDetector = new GestureDetector(context , this , null , false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    // Implementation of 3 methods of GLSurfaceView interface

    @Override
    public void onSurfaceCreated(GL10 gl , EGLConfig config)
    {
        // code
        initialize(gl);
    }    

    @Override
    public void onSurfaceChanged(GL10 gl , int width , int height)
    {
        // code
        resize(width , height);
    }

    @Override
    public void onDrawFrame(GL10 gl)
    {
        // code
        display();
    }

    // implementation of onTouch method event of view class

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

    // implement ondoubletap listener interface 3 methods. 

    @Override
    public boolean onDoubleTap(MotionEvent e)
    {
        if (bLightingEnabled == false)
        {
            bLightingEnabled = true;
        }
        else
        {
            bLightingEnabled = false;
        }

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

    // implementation of 6 methods of ongesturelistener

    @Override
    public boolean onDown(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onFling(MotionEvent e1 , MotionEvent e2 , float velocityX , float velocityY)
    {
        return true;
    }

    @Override
    public void onLongPress(MotionEvent e)
    {

    }

    @Override
    public boolean onScroll(MotionEvent e1 , MotionEvent e2 , float distanceX , float distanceY)
    {
        uninitialize();

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

    // implementation of our private methods

    private void initialize(GL10 gl)
    {
        // code

        // print OpenGLES information
        // printGLInfo(gl);

        // vertex shader

        final String vertexShaderSourceCode = String.format
        (
            "#version 320 es"+
            "\n"+
            "in vec4 aPosition;" +
            "in vec3 aNormal;" +
            "uniform mat4 uModelViewMatrix;" +
            "uniform mat4 uProjectionMatrix;" +
            "uniform vec3 uld;" +
            "uniform vec3 ukd;" +
            "uniform vec4 uLightposition;" +
            "uniform mediump int uKeyPress;" +
            "out vec3 oDiffuseLight;" +
            "in vec4 aColor;" +
            "out vec4 oColor;" +
            "void main(void)" +
            "{" +
            "if(uKeyPress == 1)" +
            "{" +
            "vec4 eyePosition = uModelViewMatrix * aPosition;" +
            "mat3 normalMatrix = mat3(transpose(inverse(uModelViewMatrix)));" +
            "vec3 n = normalize(normalMatrix * aNormal);" +
            "vec3 s = normalize(vec3(uLightposition - eyePosition));" +
            "oDiffuseLight = uld * ukd * dot(s,n); " +
            "}" +
            "else" +
            "{" +
            "oDiffuseLight = vec3(1.0f , 1.0f , 1.0f);" +
            "}" +
            "gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;" +
            "}"
        );

        int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
        GLES32.glCompileShader(vertexShaderObject);

        int[] shaderCompileStatus = new int[1];
        int[] infoLogLength = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(vertexShaderObject , GLES32.GL_COMPILE_STATUS , shaderCompileStatus , 0);  
        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("Vertex Shader Compilation Error Log :"+infoLog);
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
            "in vec4 oColor;" +
            "in vec3 oDiffuseLight;" +
            "out vec4 FragColor;" +
            "uniform mediump int uKeyPress;" +
            "void main(void)" +
            "{" +
            "if(uKeyPress == 1)" +
            "{" +
            "FragColor = vec4(oDiffuseLight , 1.0f);" +
            "}" +
            "else" +
            "{" +
            "FragColor = vec4(1.0f , 1.0f , 1.0f , 1.0f);" +
            "}" +
            "}"
        );

        int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
        GLES32.glCompileShader(fragmentShaderObject);

        shaderCompileStatus = new int[1];
        infoLogLength = new int[1];
        infoLog = null;

        GLES32.glGetShaderiv(fragmentShaderObject , GLES32.GL_COMPILE_STATUS , shaderCompileStatus , 0);
        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("Fragment Shader Compilation Error Log :"+infoLog);
                uninitialize();
                System.exit(0);
            }
        }
        

        // shader program

        shaderProgramObject = GLES32.glCreateProgram();

        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

        GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
        GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_NORMALS, "aNormal");

        GLES32.glLinkProgram(shaderProgramObject);

        int[] programLinkStatus = new int[1];
        infoLogLength = new int[1];
        infoLog = null;

        GLES32.glGetProgramiv(shaderProgramObject , GLES32.GL_LINK_STATUS , programLinkStatus , 0);
        if(programLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(shaderProgramObject);
                System.out.println("ShaderProgram Link Compilation Error Log :"+infoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // get shader uniform location


        modelViewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uModelViewMatrix");
        projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
        ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uld");
        kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "ukd");
        lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightposition");
        keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKeyPress");

        // Geometry / Shape

        Sphere sphere=new Sphere();
        float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // vao
        GLES32.glGenVertexArrays(1,vao_sphere,0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // position vbo
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
        
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION,
                                        3,
                                        GLES32.GL_FLOAT,
                                        false,0,0);
        
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // normal vbo
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
        
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_NORMALS,
                                        3,
                                        GLES32.GL_FLOAT,
                                        false,0,0);
        
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_NORMALS);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // element vbo
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
 
        Matrix.setIdentityM(perspectiveProjectionMatrix , 0);

        // depth setting

        GLES32.glClearDepthf(1.0f);
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        // enable back face culling 

        // GLES32.glEnable(GLES32.GL_CULL_FACE);

        // clear color

        GLES32.glClearColor(0.0f , 0.0f , 0.0f , 1.0f);
    }

    private void printGLInfo(GL10 gl)
    {
        // code
        System.out.println("VB: OpenGL-ES Renderer : " + gl.glGetString(GL10.GL_RENDERER));
        System.out.println("VB: OpenGL-ES Version : " + gl.glGetString(GL10.GL_VERSION));
        System.out.println("VB: OpenGL-ES Shading Language Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
        
    }

    private void resize(int width , int height)
    {
        // code
        if(height <= 0)
        {
            height = 1;
        }

        GLES32.glViewport(0,0,width , height);

        Matrix.perspectiveM(perspectiveProjectionMatrix , 0 , 45.0f, (float)width / (float)height, 0.1f, 100.f);

    }

    private void display()
    {
        // code
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        // // render

        GLES32.glUseProgram(shaderProgramObject);

        // transformation
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix , 0);
        Matrix.setIdentityM(modelViewProjectionMatrix , 0);

        Matrix.translateM(modelViewMatrix, 0 , 0.0f, 0.0f, -2.0f);

        // Matrix.multiplyMM(perspectiveProjectionMatrix,0, modelViewMatrix,0 );

        // GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix,0);

        GLES32.glUniformMatrix4fv(modelViewMatrixUniform, 1, false, modelViewMatrix , 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix , 0);

		if (bLightingEnabled == true)
		{
			GLES32.glUniform1i(keyPressUniform , 1);
			GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(lightPositionUniform, 1, lightPosition, 0);
		}
		else
		{
			GLES32.glUniform1i(keyPressUniform, 0);
		}

        // bind vao
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES32.glBindVertexArray(0);
    
        GLES32.glUseProgram(0);


        requestRender(); // equvivalent to swapbuffers

    }

    private void uninitialize()
    {
        // code
        if(shaderProgramObject > 0)
        {
            GLES32.glUseProgram(shaderProgramObject);

            int[] retVal = new int[1];

            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, retVal , 0);

            if(retVal[0] > 0)
            {
                int numattachShaders = retVal[0];
                int[] shaderObjectts = new int[numattachShaders];
    
                GLES32.glGetAttachedShaders(shaderProgramObject, numattachShaders, retVal ,0 , shaderObjectts , 0);
                for(int i = 0 ; i<numattachShaders; i++)
                {
                    GLES32.glDetachShader(shaderProgramObject, shaderObjectts[i]);
                    GLES32.glDeleteShader(shaderObjectts[i]);
                    shaderObjectts[i] = 0;
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