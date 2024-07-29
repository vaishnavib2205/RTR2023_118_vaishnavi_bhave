package com.vb.threemovinglights;

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

    private int shaderProgramObject_pf;
    private int shaderProgramObject_pv;

    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];

    private int modelMatrixUniform = 0;
    private int projectionMatrixUniform = 0;
    private int viewMatrixUniform = 0;
    
    private int modelMatrixUniform_pf = 0;
    private int projectionMatrixUniform_pf = 0;
    private int viewMatrixUniform_pf = 0;
    
    private int[] lightDiffuseUniform = new int[3]; // for diffuse light
    private int materialDiffuseUniform = 0; // for diffuse material
    private int[] lightAmbientUniform = new int[3]; // for ambient light
    private int materialAmbientUniform = 0; // for ambient material
    private int[] lightSpecularUniform = new int[3]; // for specular light
    private int materialSpecularUniform = 0; // for specular material
    private int materialShinenessUniform = 0; // for shinneness material
    private int[] lightPositionUniform = new int[3];
    private int keyPressUniform = 0;
    
    private int[] lightDiffuseUniform_pf = new int[3]; // for diffuse light
    private int materialDiffuseUniform_pf = 0; // for diffuse material
    private int[] lightAmbientUniform_pf = new int[3]; // for ambient light
    private int materialAmbientUniform_pf = 0; // for ambient material
    private int[] lightSpecularUniform_pf = new int[3]; // for specular light
    private int materialSpecularUniform_pf = 0; // for specular material
    private int materialShinenessUniform_pf = 0; // for shinneness material
    private int[] lightPositionUniform_pf = new int[3];


    private Boolean bLightingEnabled = false;

    private Boolean bLightingEnabled_pf = false;
    private Boolean bLightingEnabled_pv = false;

    private float angle_Triangle = 0.0f;
    private float[] perspectiveProjectionMatrix  = new float[16];

    private class Light
    {
        float[] ambient = new float[4];
        float[] diffuse = new float[4];
        float[] specular = new float[4];
        float[] position = new float[4];
    };

    private Light[] light = new Light[3];

    private float lightAngleZero = 0.0f;
    private float lightAngleOne = 0.0f;
    private float lightAngleTwo = 0.0f;


    final float[] materialAmbient = new float[]{0.0f , 0.0f , 0.0f , 1.0f};
    final float[] materialDiffuse = new float[]{0.5f , 0.2f , 0.7f , 1.0f};
    final float[] materialSpecular = new float[]{0.7f , 0.7f , 0.7f , 1.0f};
    final float materialShineness = 128.0f;

    private int singleTap = 0;


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
        singleTap++;
        if(singleTap > 2)
        {
            singleTap = 0;
        }

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
        //Per vertex
        {
            // vertex shader

            final String vertexShaderSourceCode_pv = String.format
            (
                "#version 320 es"+
                "\n"+
                "uniform mediump int uKeyPress;" +
                "in vec4 aPosition;" +
                "in vec3 aNormal;" +
                "uniform mat4 uModelMatrix;" +
                "uniform mat4 uViewMatrix;" +
                "uniform mat4 uProjectionMatrix;" +
                "uniform vec3 uLightAmbient[3];" +
                "uniform vec3 uLightDiffuse[3];" +
                "uniform vec3 uLightSpecular[3];" +
                "uniform vec4 uLightposition[3];" +
                "uniform vec3 uMaterialAmbient;" +
                "uniform vec3 uMaterialDiffuse;" +
                "uniform vec3 uMaterialSpecular;" +
                "uniform float uMaterialShineness;" +
                "out vec3 oFong_ADS_Light;" +
                "in vec4 aColor;" +
                "out vec4 oColor;" +
                "void main(void)" +
                "{" +
                "oFong_ADS_Light = vec3(0.0f , 0.0f , 0.0f);" +
                "if(uKeyPress == 1)" +
                "{" +
                "vec4 eyeCoordinates =  uViewMatrix * uModelMatrix * aPosition;" +
                "vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);" +
                "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" +
                "vec3 ambientLight[3];" +
                "vec3 diffuseLight[3];" +
                "vec3 specularLight[3]; " +
                "vec3 lightDirection[3];" +
                "vec3 reflectionVector[3];" +
                "for(int i = 0 ; i <3;i++)" +
                "{" +
                "ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;" +
                "lightDirection[i] = normalize(vec3(uLightposition[i] - eyeCoordinates));" +
                "diffuseLight[i] = uLightDiffuse[i] * uMaterialDiffuse * max(dot(lightDirection[i] , transformedNormals) , 0.0f); " +
                "reflectionVector[i] = reflect(-lightDirection[i] , transformedNormals);" +
                "specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i] , viewerVector) , 0.0f) , uMaterialShineness); " +
                "oFong_ADS_Light = oFong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" +
                "}" +
                "}" +
                "else" +
                "{" +
                "oFong_ADS_Light = vec3(0.0f , 0.0f , 0.0f);" +
                "}" +
                "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" +
                "}"
            );

            int vertexShaderObject_pv = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
            GLES32.glShaderSource(vertexShaderObject_pv, vertexShaderSourceCode_pv);
            GLES32.glCompileShader(vertexShaderObject_pv);

            int[] shaderCompileStatus = new int[1];
            int[] infoLogLength = new int[1];
            String infoLog = null;

            GLES32.glGetShaderiv(vertexShaderObject_pv , GLES32.GL_COMPILE_STATUS , shaderCompileStatus , 0);  
            if(shaderCompileStatus[0] == GLES32.GL_FALSE)
            {
                GLES32.glGetShaderiv(vertexShaderObject_pv, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
                if(infoLogLength[0] > 0)
                {
                    infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_pv);
                    System.out.println("Vertex Shader Compilation Error Log :"+infoLog);
                    uninitialize();
                    System.exit(0);
                }
            }
            

            // fragment shader

            final String fragmentShaderSourceCode_pv = String.format
            (
                "#version 320 es"+
                "\n"+
                "precision highp float;"+
                "uniform mediump int uKeyPress;" +
                "in vec4 oColor;" +
                "in vec3 oFong_ADS_Light;" +
                "out vec4 FragColor;" +
                "void main(void)" +
                "{" +
                "if(uKeyPress == 1)" +
                "{" +
                "FragColor = vec4(oFong_ADS_Light , 1.0f);" +
                "}" +
                "else" +
                "{" +
                "FragColor = vec4(1.0f , 1.0f , 1.0f , 1.0f);" +
                "}" +
                "}"
            );

            int fragmentShaderObject_pv = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
            GLES32.glShaderSource(fragmentShaderObject_pv, fragmentShaderSourceCode_pv);
            GLES32.glCompileShader(fragmentShaderObject_pv);

            shaderCompileStatus = new int[1];
            infoLogLength = new int[1];
            infoLog = null;

            GLES32.glGetShaderiv(fragmentShaderObject_pv , GLES32.GL_COMPILE_STATUS , shaderCompileStatus , 0);
            if(shaderCompileStatus[0] == GLES32.GL_FALSE)
            {
                GLES32.glGetShaderiv(fragmentShaderObject_pv, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
                if(infoLogLength[0] > 0)
                {
                    infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_pv);
                    System.out.println("Fragment Shader Compilation Error Log :"+infoLog);
                    uninitialize();
                    System.exit(0);
                }
            }
            

            // shader program

            shaderProgramObject_pv = GLES32.glCreateProgram();

            GLES32.glAttachShader(shaderProgramObject_pv, vertexShaderObject_pv);
            GLES32.glAttachShader(shaderProgramObject_pv, fragmentShaderObject_pv);

            GLES32.glBindAttribLocation(shaderProgramObject_pv, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
            GLES32.glBindAttribLocation(shaderProgramObject_pv, VertexAttributesEnum.AMC_ATTRIBUTE_NORMALS, "aNormal");

            GLES32.glLinkProgram(shaderProgramObject_pv);

            int[] programLinkStatus = new int[1];
            infoLogLength = new int[1];
            infoLog = null;

            GLES32.glGetProgramiv(shaderProgramObject_pv , GLES32.GL_LINK_STATUS , programLinkStatus , 0);
            if(programLinkStatus[0] == GLES32.GL_FALSE)
            {
                GLES32.glGetProgramiv(shaderProgramObject_pv, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
                if(infoLogLength[0] > 0)
                {
                    infoLog = GLES32.glGetShaderInfoLog(shaderProgramObject_pv);
                    System.out.println("ShaderProgram Link Compilation Error Log :"+infoLog);
                    uninitialize();
                    System.exit(0);
                }
            }

            // get shader uniform location


            modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uModelMatrix");
            viewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uViewMatrix");
            projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uProjectionMatrix");
    
            lightAmbientUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightAmbient[0]");
            lightDiffuseUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightDiffuse[0]");
            lightSpecularUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular[0]");
            lightPositionUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightposition[0]");
    
            lightAmbientUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightAmbient[1]");
            lightDiffuseUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightDiffuse[1]");
            lightSpecularUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular[1]");
            lightPositionUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightposition[1]");
    
            lightAmbientUniform[2] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightAmbient[2]");
            lightDiffuseUniform[2] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightDiffuse[2]");
            lightSpecularUniform[2] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular[2]");
            lightPositionUniform[2] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightposition[2]");
    
            materialAmbientUniform = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uMaterialAmbient");
            materialDiffuseUniform = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uMaterialDiffuse");
            materialSpecularUniform = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uMaterialSpecular");
            materialShinenessUniform = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uMaterialShineness");
            keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uKeyPress");

        }

        // per vertex lighting
        {
            // vertex shader

            final String vertexShaderSourceCode_pf = String.format
            (
                "#version 320 es"+
                "\n"+
                "uniform mediump int uKeyPress;" +
                "in vec4 aPosition;" +
                "in vec3 aNormal;" +
                "uniform mat4 uModelMatrix;" +
                "uniform mat4 uViewMatrix;" +
                "uniform mat4 uProjectionMatrix;" +
                "out vec3 otransformedNormals;" +
                "out vec3 oviewerVector;" +
                "out vec4 oeyeCoordinates;" +
                "in vec4 aColor;" +
                "out vec4 oColor;" +
                "void main(void)" +
                "{" +
                "if(uKeyPress == 1)" +
                "{" +
                "oeyeCoordinates =  uViewMatrix * uModelMatrix * aPosition;" +
                "otransformedNormals = (mat3(uViewMatrix * uModelMatrix) * aNormal);" +
                "oviewerVector = (-oeyeCoordinates.xyz);" +
                "}" +
                "else" +
                "{" +
                "otransformedNormals = vec3(0.0f , 0.0f , 0.0f);" +
                "oviewerVector = vec3(0.0f , 0.0f , 0.0f);" +
                "}" +
                "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" +
                "}"
            );

            int vertexShaderObject_pf = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
            GLES32.glShaderSource(vertexShaderObject_pf, vertexShaderSourceCode_pf);
            GLES32.glCompileShader(vertexShaderObject_pf);

            int[] shaderCompileStatus = new int[1];
            int[] infoLogLength = new int[1];
            String infoLog = null;

            GLES32.glGetShaderiv(vertexShaderObject_pf , GLES32.GL_COMPILE_STATUS , shaderCompileStatus , 0);  
            if(shaderCompileStatus[0] == GLES32.GL_FALSE)
            {
                GLES32.glGetShaderiv(vertexShaderObject_pf, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
                if(infoLogLength[0] > 0)
                {
                    infoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_pf);
                    System.out.println("Vertex Shader Compilation Error Log :"+infoLog);
                    uninitialize();
                    System.exit(0);
                }
            }
            

            // fragment shader

            final String fragmentShaderSourceCode_pv = String.format
            (
                "#version 320 es"+
                "\n"+
                "precision highp float;"+
                "uniform mediump int uKeyPress;" +
                "in vec4 oColor;" +
                "in vec3 otransformedNormals;" +
                "in vec3 oviewerVector;" +
                "in vec4 oeyeCoordinates;" +
                "out vec4 FragColor;" +
                "uniform vec3 uLightAmbient[3];" +
                "uniform vec3 uLightDiffuse[3];" +
                "uniform vec3 uLightSpecular[3];" +
                "uniform vec4 uLightposition[3];" +
                "uniform vec3 uMaterialAmbient;" +
                "uniform vec3 uMaterialDiffuse;" +
                "uniform vec3 uMaterialSpecular;" +
                "uniform float uMaterialShineness;" +
                "void main(void)" +
                "{" +
                "if(uKeyPress == 1)" +
                "{" +
                "vec3 oFong_ADS_Light;" +
                "vec3 ambientLight[3];" +
                "vec3 diffuseLight[3];" +
                "vec3 specularLight[3]; " +
                "vec3 lightDirection[3];" +
                "vec3 reflectionVector[3];" +
                "vec3 normalizedTranformedNormals = normalize(otransformedNormals);" +
                "vec3 normalizedViewerVector= normalize(oviewerVector);" +
                "for(int i = 0 ; i <3;i++)" +
                "{" +
                "ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;" +
                "lightDirection[i] = normalize(vec3(uLightposition[i] - oeyeCoordinates));" +
                "diffuseLight[i] = uLightDiffuse[i] * uMaterialDiffuse * max(dot(lightDirection[i] , normalizedTranformedNormals) , 0.0f); " +
                "reflectionVector[i] = reflect(-lightDirection[i] , normalizedTranformedNormals);" +
                "specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i] , normalizedViewerVector) , 0.0f) , uMaterialShineness); " +
                "oFong_ADS_Light = oFong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" +
                "}" +
                "FragColor = vec4(oFong_ADS_Light , 1.0f);" +
                "}" +
                "else" +
                "{" +
                "FragColor = vec4(1.0f , 1.0f , 1.0f , 1.0f);" +
                "}" +
                "}"
            );

            int fragmentShaderObject_pf = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
            GLES32.glShaderSource(fragmentShaderObject_pf, fragmentShaderSourceCode_pv);
            GLES32.glCompileShader(fragmentShaderObject_pf);

            shaderCompileStatus = new int[1];
            infoLogLength = new int[1];
            infoLog = null;

            GLES32.glGetShaderiv(fragmentShaderObject_pf , GLES32.GL_COMPILE_STATUS , shaderCompileStatus , 0);
            if(shaderCompileStatus[0] == GLES32.GL_FALSE)
            {
                GLES32.glGetShaderiv(fragmentShaderObject_pf, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
                if(infoLogLength[0] > 0)
                {
                    infoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_pf);
                    System.out.println("Fragment Shader Compilation Error Log :"+infoLog);
                    uninitialize();
                    System.exit(0);
                }
            }
            

            // shader program

            shaderProgramObject_pf = GLES32.glCreateProgram();

            GLES32.glAttachShader(shaderProgramObject_pf, vertexShaderObject_pf);
            GLES32.glAttachShader(shaderProgramObject_pf, fragmentShaderObject_pf);

            GLES32.glBindAttribLocation(shaderProgramObject_pf, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
            GLES32.glBindAttribLocation(shaderProgramObject_pf, VertexAttributesEnum.AMC_ATTRIBUTE_NORMALS, "aNormal");

            GLES32.glLinkProgram(shaderProgramObject_pf);

            int[] programLinkStatus = new int[1];
            infoLogLength = new int[1];
            infoLog = null;

            GLES32.glGetProgramiv(shaderProgramObject_pf , GLES32.GL_LINK_STATUS , programLinkStatus , 0);
            if(programLinkStatus[0] == GLES32.GL_FALSE)
            {
                GLES32.glGetProgramiv(shaderProgramObject_pf, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
                if(infoLogLength[0] > 0)
                {
                    infoLog = GLES32.glGetShaderInfoLog(shaderProgramObject_pf);
                    System.out.println("ShaderProgram Link Compilation Error Log :"+infoLog);
                    uninitialize();
                    System.exit(0);
                }
            }

            // get shader uniform location


            modelMatrixUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uModelMatrix");
            viewMatrixUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uViewMatrix");
            projectionMatrixUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uProjectionMatrix");
    
            lightAmbientUniform_pf[0] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightAmbient[0]");
            lightDiffuseUniform_pf[0] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightDiffuse[0]");
            lightSpecularUniform_pf[0] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightSpecular[0]");
            lightPositionUniform_pf[0] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightposition[0]");
    
            lightAmbientUniform_pf[1] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightAmbient[1]");
            lightDiffuseUniform_pf[1] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightDiffuse[1]");
            lightSpecularUniform_pf[1] = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular[1]");
            lightPositionUniform_pf[1] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightposition[1]");
    
            lightAmbientUniform_pf[2] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightAmbient[2]");
            lightDiffuseUniform_pf[2] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightDiffuse[2]");
            lightSpecularUniform_pf[2] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightSpecular[2]");
            lightPositionUniform_pf[2] = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightposition[2]");
    
            materialAmbientUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uMaterialAmbient");
            materialDiffuseUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uMaterialDiffuse");
            materialSpecularUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uMaterialSpecular");
            materialShinenessUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uMaterialShineness");
            keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uKeyPress");

        }

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

        light[0] = new Light();

        light[0].ambient[0] = 0.0f;
        light[0].ambient[1] = 0.0f;
        light[0].ambient[2] = 0.0f;
        light[0].ambient[3] = 1.0f;

        light[0].diffuse[0] = 1.0f;
        light[0].diffuse[1] = 0.0f;
        light[0].diffuse[2] = 0.0f;
        light[0].diffuse[3] = 1.0f;

        light[0].specular[0] = 1.0f;
        light[0].specular[1] = 0.0f;
        light[0].specular[2] = 0.0f;
        light[0].specular[3] = 1.0f;


        light[1] = new Light();

        light[1].ambient[0] = 0.0f;
        light[1].ambient[1] = 0.0f;
        light[1].ambient[2] = 0.0f;
        light[1].ambient[3] = 1.0f;

        light[1].diffuse[0] = 0.0f;
        light[1].diffuse[1] = 1.0f;
        light[1].diffuse[2] = 0.0f;
        light[1].diffuse[3] = 1.0f;

        light[1].specular[0] = 0.0f;
        light[1].specular[1] = 1.0f;
        light[1].specular[2] = 0.0f;
        light[1].specular[2] = 1.0f;


        light[2] = new Light();

        light[2].ambient[0] = 0.0f;
        light[2].ambient[1] = 0.0f;
        light[2].ambient[2] = 0.0f;
        light[2].ambient[3] = 1.0f;

        light[2].diffuse[0] = 0.0f;
        light[2].diffuse[1] = 0.0f;
        light[2].diffuse[2] = 1.0f;
        light[2].diffuse[3] = 1.0f;

        light[2].specular[0] = 0.0f;
        light[2].specular[1] = 0.0f;
        light[2].specular[2] = 1.0f;
        light[2].specular[2] = 1.0f;



    }

    private void printGLInfo(GL10 gl)
    {
        // code
        System.out.println("PB: OpenGL-ES Renderer : " + gl.glGetString(GL10.GL_RENDERER));
        System.out.println("PB: OpenGL-ES Version : " + gl.glGetString(GL10.GL_VERSION));
        System.out.println("PB: OpenGL-ES Shading Language Version : " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
        
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



        if (singleTap == 1)
        {
            GLES32.glUseProgram(shaderProgramObject_pv);
        }
        else if (singleTap == 2)
        {
            GLES32.glUseProgram(shaderProgramObject_pf);
        }
        else
        {
            GLES32.glUseProgram(shaderProgramObject_pv);
        }

        // transformation
        float[] modelMatrix = new float[16];
        float[] ViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelMatrix , 0);
        Matrix.setIdentityM(ViewMatrix , 0);
        Matrix.setIdentityM(modelViewProjectionMatrix , 0);

        Matrix.translateM(modelMatrix, 0 , 0.0f, 0.0f, -2.0f);

        // Matrix.multiplyMM(perspectiveProjectionMatrix, 0 ,modelMatrix,0, ViewMatrix,0 );




        if (bLightingEnabled == true)
		{
			if(singleTap == 1)
			{
                GLES32.glUniformMatrix4fv(modelMatrixUniform , 1, false, modelMatrix ,0);
                GLES32.glUniformMatrix4fv(viewMatrixUniform , 1, false, ViewMatrix ,0);
                GLES32.glUniformMatrix4fv(projectionMatrixUniform , 1, false, perspectiveProjectionMatrix ,0);

				GLES32.glUniform1i(keyPressUniform, 1);
                GLES32.glUniform3fv(lightAmbientUniform[0], 1, light[0].ambient , 0);
                GLES32.glUniform3fv(lightDiffuseUniform[0], 1, light[0].diffuse , 0);
                GLES32.glUniform3fv(lightSpecularUniform[0], 1, light[0].specular , 0);

				GLES32.glUniform3fv(lightAmbientUniform[1], 1, light[1].ambient , 0);
				GLES32.glUniform3fv(lightDiffuseUniform[1], 1, light[1].diffuse , 0);
				GLES32.glUniform3fv(lightSpecularUniform[1], 1, light[1].specular , 0);

				GLES32.glUniform3fv(lightAmbientUniform[2], 1, light[2].ambient , 0);
				GLES32.glUniform3fv(lightDiffuseUniform[2], 1, light[2].diffuse , 0);
				GLES32.glUniform3fv(lightSpecularUniform[2], 1, light[2].specular , 0);

                GLES32.glUniform3fv(materialAmbientUniform, 1, materialAmbient , 0);
                GLES32.glUniform3fv(materialDiffuseUniform, 1, materialDiffuse , 0);
                GLES32.glUniform3fv(materialSpecularUniform, 1, materialSpecular , 0);
                GLES32.glUniform1f(materialShinenessUniform, materialShineness);

                GLES32.glUniform4fv(lightPositionUniform[0], 1, light[0].position , 0);
                GLES32.glUniform4fv(lightPositionUniform[1], 1, light[1].position , 0);
				GLES32.glUniform4fv(lightPositionUniform[2], 1, light[2].position , 0);

			}
			if (singleTap == 2)
			{
                GLES32.glUniformMatrix4fv(modelMatrixUniform_pf , 1, false, modelMatrix ,0);
                GLES32.glUniformMatrix4fv(viewMatrixUniform_pf , 1, false, ViewMatrix ,0);
                GLES32.glUniformMatrix4fv(projectionMatrixUniform_pf , 1, false, perspectiveProjectionMatrix ,0);

				GLES32.glUniform1i(keyPressUniform, 1);
                GLES32.glUniform3fv(lightAmbientUniform_pf[0], 1, light[0].ambient , 0);
                GLES32.glUniform3fv(lightDiffuseUniform_pf[0], 1, light[0].diffuse , 0);
                GLES32.glUniform3fv(lightSpecularUniform_pf[0], 1, light[0].specular , 0);

				GLES32.glUniform3fv(lightAmbientUniform_pf[1], 1, light[1].ambient , 0);
				GLES32.glUniform3fv(lightDiffuseUniform_pf[1], 1, light[1].diffuse , 0);
				GLES32.glUniform3fv(lightSpecularUniform_pf[1], 1, light[1].specular , 0);

				GLES32.glUniform3fv(lightAmbientUniform_pf[2], 1, light[2].ambient , 0);
				GLES32.glUniform3fv(lightDiffuseUniform_pf[2], 1, light[2].diffuse , 0);
				GLES32.glUniform3fv(lightSpecularUniform_pf[2], 1, light[2].specular , 0);

                GLES32.glUniform3fv(materialAmbientUniform_pf, 1, materialAmbient , 0);
                GLES32.glUniform3fv(materialDiffuseUniform_pf, 1, materialDiffuse , 0);
                GLES32.glUniform3fv(materialSpecularUniform_pf, 1, materialSpecular , 0);
                GLES32.glUniform1f(materialShinenessUniform_pf, materialShineness);

                GLES32.glUniform4fv(lightPositionUniform_pf[0], 1, light[0].position , 0);
                GLES32.glUniform4fv(lightPositionUniform_pf[1], 1, light[1].position , 0);
				GLES32.glUniform4fv(lightPositionUniform_pf[2], 1, light[2].position , 0);

			}
		}
		else
		{
            GLES32.glUniformMatrix4fv(modelMatrixUniform , 1, false, modelMatrix ,0);
            GLES32.glUniformMatrix4fv(viewMatrixUniform , 1, false, ViewMatrix ,0);
            GLES32.glUniformMatrix4fv(projectionMatrixUniform , 1, false, perspectiveProjectionMatrix ,0);

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

        if (bLightingEnabled == true )
        {
            light[0].position[0] = 0.0f;
            light[0].position[1] = 5.0f * (float)Math.sin(lightAngleZero);
            light[0].position[2] = 5.0f * (float)Math.cos(lightAngleZero);
            light[0].position[3] = 1.0f;
    
    
            light[1].position[0] = 5.0f * (float)Math.cos(lightAngleOne);
            light[1].position[1] = 0.0f;
            light[1].position[2] = 5.0f * (float)Math.sin(lightAngleOne);
            light[1].position[3] = 1.0f;
    
    
            light[2].position[0] = 5.0f * (float)Math.cos(lightAngleTwo);
            light[2].position[1] = 5.0f * (float)Math.sin(lightAngleTwo);
            light[2].position[2] = 0.0f;
            light[2].position[3] = 1.0f;
        }
        
        lightAngleZero = lightAngleZero + 0.05f;
        if (lightAngleZero >= (360.0f))
        {
            lightAngleZero = lightAngleZero - (360.0f);
        }
    
        lightAngleOne = lightAngleOne + 0.05f;
        if (lightAngleOne >= (360.0f))
        {
            lightAngleOne = lightAngleOne - (360.0f);
        }
    
        lightAngleTwo = lightAngleTwo + 0.05f;
        if (lightAngleTwo >= (360.0f))
        {
            lightAngleTwo = lightAngleTwo - (360.0f);
        }

        requestRender(); // equvivalent to swapbuffers

    }

    private void uninitialize()
    {
        // code
        if(shaderProgramObject_pf > 0)
        {
            GLES32.glUseProgram(shaderProgramObject_pf);

            int[] retVal = new int[1];

            GLES32.glGetProgramiv(shaderProgramObject_pf, GLES32.GL_ATTACHED_SHADERS, retVal , 0);

            if(retVal[0] > 0)
            {
                int numattachShaders = retVal[0];
                int[] shaderObjectts = new int[numattachShaders];
    
                GLES32.glGetAttachedShaders(shaderProgramObject_pf, numattachShaders, retVal ,0 , shaderObjectts , 0);
                for(int i = 0 ; i<numattachShaders; i++)
                {
                    GLES32.glDetachShader(shaderProgramObject_pf, shaderObjectts[i]);
                    GLES32.glDeleteShader(shaderObjectts[i]);
                    shaderObjectts[i] = 0;
                }
            }

            GLES32.glUseProgram(0);
            GLES32.glDeleteProgram(shaderProgramObject_pf);
            shaderProgramObject_pf = 0;
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