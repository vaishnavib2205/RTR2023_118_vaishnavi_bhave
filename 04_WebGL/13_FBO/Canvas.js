// main function
var canvas;
var gl = null; // renaming context as gl for our use
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;

var winWidth = 0;
var winHeight = 0;

// WebGL related Varaiables

const vertexAttributeEnum = 
{
    AMC_ATTRIBUTE_POSITION:0,
    AMC_ATTRIBUTE_COLOR:1,
    AMC_ATTRIBUTE_TEXCOORDS:2,
    AMC_ATTRIBUTE_NORMAL:3

};

var shaderProgramObject_cube = null;

var vao_cube = null;
var vbo_cube = null;
var angle_Rectangle_cube = 0.0;
var vbo_texture_cube = null;

var mvpMatrixUniform_cube ;
var perspectiveProjectionMatrix_cube;

var stone_texture ;
var textureSamplerUniform_cube;

var fbo;
var rbo;
var texture_FBO;
var vFboResult = false;

var FBO_WIDTH = 512;
var FBO_HEIGHT = 512;

// sphere related global variables

var shaderProgramObject_pv_sphere = null;
var shaderProgramObject_pf_sphere = null;

var perspectiveProjectionMatrix_sphere;

var bLightingEnabled = false;

var bLightingEnabled_pf_sphere = false;
var bLightingEnabled_pv_sphere = false;

var sphere = null;

var lightDiffuseUniform_sphere = new Array(3); // for diffuse light
var materialDiffuseUniform_sphere = 0; // for diffuse materia
var lightAmbientUniform_sphere = new Array(3); // for ambient light
var materialAmbientUniform_sphere = 0; // for ambient materia
var lightSpecularUniform_sphere = new Array(3); // for specular light
var materialSpecularUniform_sphere = 0; // for specular material
var materialShinenessUniform_sphere = 0; // for shinneness materia
var lightPositionUniform_sphere = new Array(3);

var modelMatrixUniform_pv_sphere = 0;
var projectionMatrixUniform_pv_sphere = 0;
var viewMatrixUniform_pv_sphere = 0;

var modelMatrixUniform_sphere = 0;
var projectionMatrixUniform_sphere = 0;
var viewMatrixUniform_sphere = 0;

var lightAngleZero_sphere = 0.0;
var lightAngleOne_sphere = 0.0;
var lightAngleTwo_sphere = 0.0;

var lightDiffuseUniform_pv_sphere = new Array(3); // for diffuse light
var materialDiffuseUniform_pv_sphere = 0; // for diffuse materia
var lightAmbientUniform_pv_sphere = new Array(3); // for ambient light
var materialAmbientUniform_pv_sphere = 0; // for ambient materia
var lightSpecularUniform_pv_sphere = new Array(3); // for specular light
var materialSpecularUniform_pv_sphere = 0; // for specular material
var materialShinenessUniform_pv_sphere = 0; // for shinneness materia
var lightPositionUniform_pv_sphere = new Array(3);

var keyPressUniform_sphere = 0;
var keyPressUniform_pv_sphere = 0;

var lightAmbient_one_sphere = [ 0.0 , 0.0 , 0.0  ];
var lightDiffuse_one_sphere = [ 1.0 , 0.0 , 0.0 ]; // white diffuse light
var lightSpecular_one_sphere = [ 1.0 , 0.0 , 0.0  ]; // white diffuse
var lightPosition_one_sphere = new Array(4);

var lightAmbient_two_sphere = [ 0.0 , 0.0 , 0.0  ];
var lightDiffuse_two_sphere = [ 0.0 , 1.0 , 0.0 ]; // white diffuse light
var lightSpecular_two_sphere = [ 0.0 , 1.0 , 0.0  ]; // white diffuse
var lightPosition_two_sphere = new Array(4);

var lightAmbient_three_sphere = [ 0.0 , 0.0 , 0.0  ];
var lightDiffuse_three_sphere = [ 0.0 , 0.0 , 1.0 ]; // white diffuse light
var lightSpecular_three_sphere = [ 0.0 , 0.0 , 1.0  ]; // white diffuse
var lightPosition_three_sphere = new Array(4);


var materialAmbient_sphere = [ 0.0 , 0.0 , 0.0 ];
var materialDiffuse_sphere = [ 0.5 , 0.2 , 0.7 ];
var materialSpecular_sphere = [ 0.7 , 0.7 , 0.7 ];
var materialShineness_sphere = 128.0;



var requestAnimationFrame =
window.requestAnimationFrame ||
window.webkitRequestAnimationFrame ||
window.mozRequestAnimationFrame ||
window.oRequestAnimationFrame ||
window.msReuestAnimationFrame;




function main()
{
    // get canvas
    canvas = document.getElementById("vbb");

    if(canvas == null)
    {
        console.log("getting canvas failed\n");
    }
    else
    {
        console.log("getting canvas success \n");
    }


    // set canvas width and height for future use

    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;


    // register for keyboard events
    window.addEventListener("keydown" , keyDown , false);

    // register for mouse events
    window.addEventListener("click" , mouseDown , false);

    // resize
    window.addEventListener("resize" , resize_cube , false);

    initialize_cube();

    resize_cube();

    display_cube();
            
}

function keyDown(event)
{
    // alert("Key is Pressed");
    // code

    switch (event.keyCode) 
    {
        case 81:
        case 113:
            uninitialize_sphere();
            window.close();

            break;
        case 70:
        case 102:   

            if (bLightingEnabled_pf_sphere == false)
            {
                bLightingEnabled_pf_sphere = true;
            }
            else
            {
                bLightingEnabled_pf_sphere = false;
            }
            break;

        case 76:
        case 108:
            if (bLightingEnabled == false)
            {
                bLightingEnabled = true;
            }
            else
            {
                bLightingEnabled = false;
            }
            break;

        case 86:
        case 118:

            if (bLightingEnabled_pv_sphere == false)
            {
                bLightingEnabled_pv_sphere = true;
            }
            else
            {
                bLightingEnabled_pv_sphere = false;
            }
            break;

        case 27:

            toggleFullscreen();

            break;
    
        default:
            break;

    }
    
}

function mouseDown()
{
    // alert("Mouse is clicked");
}

// function for fullscreen

function toggleFullscreen()
{
    var fullscreen_element = 
    document.fullscreenElement || 
    document.webkitFullscreenElement ||
    document.mozFullScreenElement ||
    document.msFullscreenElement ||
    null;

    // if not full screen

    if(fullscreen_element == null)
    {
        if(canvas.requestFullscreen) 
        {
            canvas.requestFullscreen();
        }
        else if(canvas.webkitRequestFullscreen)
        {
            canvas.webkitRequestFullscreen();
        }
        else if(canvas.mozRequestFullScreen)
        {
            canvas.mozRequestFullScreen();
        }
        else if(canvas.msRequestFullscreen)
        {
            canvas.msRequestFullscreen();
        }
        bFullScreen = true;
    }
    else // if alredy full screen
    {
        if(document.exitFullscreen)
        {
            document.exitFullscreen();                
        }
        else if(document.webkitExitFullscreen)
        {
            document.webkitExitFullscreen();                
        }
        else if(document.mosCancelFullScreen)
        {
            document.mosCancelFullScreen();
        }
        else if(document.msExitFullScreen)
        {
            document.msExitFullScreen();
        }
        bFullScreen = false;
    }
}

function initialize_cube()
{
    //code

    // get context from above canvas
    gl = canvas.getContext("webgl2");

    if(gl == null)
    {
        console.log("getting webgl2 context failed \n");
    }
    else
    {
        console.log("getting webgl2 context success \n");
    }

    // set webgl2 context's view width and view height
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    // vertex shader

    var vertexShaderSourceCode = 
    "#version 300 es"+
    "\n"+
    "in vec4 aPosition;"+
    "in vec2 aTexCoord;"+
    "out vec2 oTexCoord;"+
    "uniform mat4 uMVPMatrix;"+
    "void main(void)"+
    "{"+
    "gl_Position = uMVPMatrix * aPosition;"+
    "oTexCoord = aTexCoord;"+
    "}";

    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject , vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if(gl.getShaderParameter(vertexShaderObject , gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject);

        if(error.length > 0)
        {
            var Log = "vertex shader compilation error : " + error;
            alert(Log);
            uninitialize_cube();
        }
    }
    else
    {
        console.log("vertex Shader Compileed Successfully !!! \n");
    }

    var fragmentShaderSourceCode = 
    "#version 300 es"+
    "\n"+
    "precision highp float;"+
    "uniform highp sampler2D uTextureSampler;"+
    "in vec2 oTexCoord;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "FragColor = texture(uTextureSampler , oTexCoord);"+
    "}";

    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject , fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);

    if(gl.getShaderParameter(fragmentShaderObject , gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if(error.length > 0)
        {
            var Log = "Fragment SHader compilation error : " + error;
            alert(Log);
            uninitialize_cube();
        }
    }
    else
    {
        console.log("fragment SHader compilation Successfully !!! \n");
    }

    // shader Program
    shaderProgramObject_cube = gl.createProgram();
    gl.attachShader(shaderProgramObject_cube , vertexShaderObject);
    gl.attachShader(shaderProgramObject_cube , fragmentShaderObject);

    gl.bindAttribLocation(shaderProgramObject_cube , vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , "aPosition");  
    gl.bindAttribLocation(shaderProgramObject_cube , vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS , "aTexCoord"); 

    gl.linkProgram(shaderProgramObject_cube);

    if(gl.getProgramParameter(shaderProgramObject_cube , gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramInfoLog(shaderProgramObject_cube);
        if(error.length > 0)
        {
            var Log = "shader Program Linking Error : " + error;
            alert(Log);
            uninitialize_cube();
        }
    }
    else
    {
        console.log("ShaderProgeam Linked Successfully !!! \n");
    }

    mvpMatrixUniform_cube = gl.getUniformLocation(shaderProgramObject_cube , "uMVPMatrix");
    textureSamplerUniform_cube = gl.getUniformLocation(shaderProgramObject_cube , "uTextureSampler");


    // triangle attributes

    var cubePosition = new Float32Array([
    
			// top
            1.0, 1.0, -1.0,
            -1.0, 1.0, -1.0,
            -1.0, 1.0, 1.0,
            1.0, 1.0, 1.0,
    
                // bottom
                1.0, -1.0, -1.0,
            -1.0, -1.0, -1.0,
            -1.0, -1.0,  1.0,
                1.0, -1.0,  1.0,
    
                // front
                1.0, 1.0, 1.0,
                -1.0, 1.0, 1.0,
                -1.0, -1.0, 1.0,
                1.0, -1.0, 1.0,
    
                // back
                1.0, 1.0, -1.0,
            -1.0, 1.0, -1.0,
            -1.0, -1.0, -1.0,
                1.0, -1.0, -1.0,
    
                // right
                1.0, 1.0, -1.0,
                1.0, 1.0, 1.0,
                1.0, -1.0, 1.0,
                1.0, -1.0, -1.0,
    
                // left
            -1.0, 1.0, 1.0,
            -1.0, 1.0, -1.0,
            -1.0, -1.0, -1.0,
            -1.0, -1.0, 1.0
    ]);

    var cubeTexCoord = new Float32Array([
    
        // front
        1.0, 1.0, // top-right of front
        0.0, 1.0, // top-left of front
        0.0, 0.0, // bottom-left of front
        1.0, 0.0, // bottom-right of front

        // right
        1.0, 1.0, // top-right of right
        0.0, 1.0, // top-left of right
        0.0, 0.0, // bottom-left of right
        1.0, 0.0, // bottom-right of right

        // back
        1.0, 1.0, // top-right of back
        0.0, 1.0, // top-left of back
        0.0, 0.0, // bottom-left of back
        1.0, 0.0, // bottom-right of back

        // left
        1.0, 1.0, // top-right of left
        0.0, 1.0, // top-left of left
        0.0, 0.0, // bottom-left of left
        1.0, 0.0, // bottom-right of left

        // top
        1.0, 1.0, // top-right of top
        0.0, 1.0, // top-left of top
        0.0, 0.0, // bottom-left of top
        1.0, 0.0, // bottom-right of top

        // bottom
        1.0, 1.0, // top-right of bottom
        0.0, 1.0, // top-left of bottom
        0.0, 0.0, // bottom-left of bottom
        1.0, 0.0 // bottom-right of bottom

    ]);

    // vao

    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    // vbo

    vbo_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_cube);

    gl.bufferData(gl.ARRAY_BUFFER , cubePosition , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo_texture

    vbo_texture_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_texture_cube);

    gl.bufferData(gl.ARRAY_BUFFER , cubeTexCoord , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS , 2 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    gl.bindVertexArray(null);

    // depth initialise
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);


    // initialise projection matrix

    perspectiveProjectionMatrix_cube = mat4.create();

    //Set Clear Color
    gl.clearColor(0.85, 0.85, 0.85, 1.0);


    //FBO related code

    if (createFBO(FBO_WIDTH, FBO_HEIGHT) == true) 
    {
        vFboResult = initialize_sphere(FBO_WIDTH, FBO_HEIGHT);    
    }
    

}

function initialize_sphere()
{
    //code

    // per vertex
    {
            // vertex shader

        var vertexShaderSourceCode_pv = 
        "#version 300 es"+
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
        "}";

        var vertexShaderObject_pv = gl.createShader(gl.VERTEX_SHADER);
        gl.shaderSource(vertexShaderObject_pv , vertexShaderSourceCode_pv);
        gl.compileShader(vertexShaderObject_pv);

        if(gl.getShaderParameter(vertexShaderObject_pv , gl.COMPILE_STATUS) == false)
        {
            var error = gl.getShaderInfoLog(vertexShaderObject_pv);

            if(error.length > 0)
            {
                var Log = "vertex shader compilation error : " + error;
                alert(Log);
                uninitialize_sphere();
            }
        }
        else
        {
            console.log("vertex Shader Compileed Successfully !!! \n");
        }

        var fragmentShaderSourceCode_pv = 
        "#version 300 es"+
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
        "}";

        var fragmentShaderObject_pv = gl.createShader(gl.FRAGMENT_SHADER);
        gl.shaderSource(fragmentShaderObject_pv , fragmentShaderSourceCode_pv);
        gl.compileShader(fragmentShaderObject_pv);

        if(gl.getShaderParameter(fragmentShaderObject_pv , gl.COMPILE_STATUS) == false)
        {
            var error = gl.getShaderInfoLog(fragmentShaderObject_pv);
            if(error.length > 0)
            {
                var Log = "Fragment SHader compilation error : " + error;
                alert(Log);
                uninitialize_sphere();
            }
        }
        else
        {
            console.log("fragment SHader compilation Successfully !!! \n");
        }

        // shader Program
        shaderProgramObject_pv_sphere = gl.createProgram();
        gl.attachShader(shaderProgramObject_pv_sphere , vertexShaderObject_pv);
        gl.attachShader(shaderProgramObject_pv_sphere , fragmentShaderObject_pv);

        gl.bindAttribLocation(shaderProgramObject_pv_sphere , vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , "aPosition");  
        gl.bindAttribLocation(shaderProgramObject_pv_sphere , vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL , "aNormal");  
        
        gl.linkProgram(shaderProgramObject_pv_sphere);

        if(gl.getProgramParameter(shaderProgramObject_pv_sphere , gl.LINK_STATUS) == false)
        {
            var error = gl.getProgramInfoLog(shaderProgramObject_pv_sphere);
            if(error.length > 0)
            {
                var Log = "shader Program Linking Error : " + error;
                alert(Log);
                uninitialize_sphere();
            }
        }
        else
        {
            console.log("ShaderProgeam Linked Successfully !!! \n");
        }

        // mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject , "uMVPMatrix");

        modelMatrixUniform_pv_sphere = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uModelMatrix");
        viewMatrixUniform_pv_sphere = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uViewMatrix");
        projectionMatrixUniform_pv_sphere = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uProjectionMatrix");

        lightAmbientUniform_pv_sphere[0] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightAmbient[0]");
        lightDiffuseUniform_pv_sphere[0] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightDiffuse[0]");
        lightSpecularUniform_pv_sphere[0] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightSpecular[0]");
        lightPositionUniform_pv_sphere[0] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightposition[0]");

        lightAmbientUniform_pv_sphere[1] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightAmbient[1]");
        lightDiffuseUniform_pv_sphere[1] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightDiffuse[1]");
        lightSpecularUniform_pv_sphere[1] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightSpecular[1]");
        lightPositionUniform_pv_sphere[1] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightposition[1]");

        lightAmbientUniform_pv_sphere[2] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightAmbient[2]");
        lightDiffuseUniform_pv_sphere[2] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightDiffuse[2]");
        lightSpecularUniform_pv_sphere[2] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightSpecular[2]");
        lightPositionUniform_pv_sphere[2] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightposition[2]");

        materialAmbientUniform_pv_sphere = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uMaterialAmbient");
        materialDiffuseUniform_pv_sphere = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uMaterialDiffuse");
        materialSpecularUniform_pv_sphere = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uMaterialSpecular");
        materialShinenessUniform_pv_sphere = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uMaterialShineness");
        keyPressUniform_pv_sphere = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uKeyPress");

    }

    // per fragment
    {
         // vertex shader

        var vertexShaderSourceCode = 
        "#version 300 es"+
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
        "}";

        var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
        gl.shaderSource(vertexShaderObject , vertexShaderSourceCode);
        gl.compileShader(vertexShaderObject);

        if(gl.getShaderParameter(vertexShaderObject , gl.COMPILE_STATUS) == false)
        {
            var error = gl.getShaderInfoLog(vertexShaderObject);

            if(error.length > 0)
            {
                var Log = "vertex shader compilation error : " + error;
                alert(Log);
                uninitialize_sphere();
            }
        }
        else
        {
            console.log("vertex Shader Compileed Successfully !!! \n");
        }

        var fragmentShaderSourceCode = 
        "#version 300 es"+
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
        "}";

        var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
        gl.shaderSource(fragmentShaderObject , fragmentShaderSourceCode);
        gl.compileShader(fragmentShaderObject);

        if(gl.getShaderParameter(fragmentShaderObject , gl.COMPILE_STATUS) == false)
        {
            var error = gl.getShaderInfoLog(fragmentShaderObject);
            if(error.length > 0)
            {
                var Log = "Fragment SHader compilation error : " + error;
                alert(Log);
                uninitialize_sphere();
            }
        }
        else
        {
            console.log("fragment SHader compilation Successfully !!! \n");
        }

        // shader Program
        shaderProgramObject_pf_sphere = gl.createProgram();
        gl.attachShader(shaderProgramObject_pf_sphere , vertexShaderObject);
        gl.attachShader(shaderProgramObject_pf_sphere , fragmentShaderObject);

        gl.bindAttribLocation(shaderProgramObject_pf_sphere , vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , "aPosition");  
        gl.bindAttribLocation(shaderProgramObject_pf_sphere , vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL , "aNormal");  
        
        gl.linkProgram(shaderProgramObject_pf_sphere);

        if(gl.getProgramParameter(shaderProgramObject_pf_sphere , gl.LINK_STATUS) == false)
        {
            var error = gl.getProgramInfoLog(shaderProgramObject_pf_sphere);
            if(error.length > 0)
            {
                var Log = "shader Program Linking Error : " + error;
                alert(Log);
                uninitialize_sphere();
            }
        }
        else
        {
            console.log("ShaderProgeam Linked Successfully !!! \n");
        }

        // mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject , "uMVPMatrix");


        modelMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uModelMatrix");
        viewMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uViewMatrix");
        projectionMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uProjectionMatrix");

        lightAmbientUniform_sphere[0] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightAmbient[0]");
        lightDiffuseUniform_sphere[0] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightDiffuse[0]");
        lightSpecularUniform_sphere[0] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightSpecular[0]");
        lightPositionUniform_sphere[0] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightposition[0]");

        lightAmbientUniform_sphere[1] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightAmbient[1]");
        lightDiffuseUniform_sphere[1] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightDiffuse[1]");
        lightSpecularUniform_sphere[1] = gl.getUniformLocation(shaderProgramObject_pv_sphere, "uLightSpecular[1]");
        lightPositionUniform_sphere[1] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightposition[1]");

        lightAmbientUniform_sphere[2] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightAmbient[2]");
        lightDiffuseUniform_sphere[2] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightDiffuse[2]");
        lightSpecularUniform_sphere[2] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightSpecular[2]");
        lightPositionUniform_sphere[2] = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uLightposition[2]");

        materialAmbientUniform_sphere = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uMaterialAmbient");
        materialDiffuseUniform_sphere = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uMaterialDiffuse");
        materialSpecularUniform_sphere = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uMaterialSpecular");
        materialShinenessUniform_sphere = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uMaterialShineness");
        keyPressUniform_sphere = gl.getUniformLocation(shaderProgramObject_pf_sphere, "uKeyPress");
    }



    // triangle attributes

    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);


    // depth initialise
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // initialise projection matrix

    perspectiveProjectionMatrix_sphere = mat4.create();

    //Set Clear Color
    gl.clearColor(0.1, 0.1, 0.1, 1.0);

    return(true);

}

function createFBO(textureWidth, textureHeight)
{
	var maxRenderBufferSize;
	// check capacity of render buffer
    // // code
    maxRenderBufferSize = gl.getParameter(gl.MAX_RENDERBUFFER_SIZE);
    if (maxRenderBufferSize < textureWidth || maxRenderBufferSize < textureHeight) {
        console.log("maxRenderBufferSize is less than Tex width and height\n");
        return (false);
    }
    else {
        console.log("maxRenderBufferSize is greater than Tex Width and Height\n");
    }

	fbo = gl.createFramebuffer();
	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

	// now create render buffer to hold depth of custom fbo

    rbo = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER, rbo);

	// set storage of render buffer of texture size

	gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, textureWidth, textureHeight);

    // Create empty texture for target scene
    var texture = new ImageData(textureWidth, textureHeight);

	// create texture for FBO in which we are going to render the sphere

    texture_FBO = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, texture_FBO);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);

	// gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, textureWidth, textureHeight, 0, gl.RGB, gl.UNSIGNED_SHORT_5_6_5, null);
    
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, textureWidth, textureHeight, 0, gl.RGB, gl.UNSIGNED_SHORT_5_6_5, texture);
    gl.bindTexture(gl.TEXTURE_2D, null);

	// attach above texture to frame buffer at color attachment 0

    gl.framebufferTexture2D(gl.FRAMEBUFFER,
        gl.COLOR_ATTACHMENT0,
        gl.TEXTURE_2D,
        texture_FBO,
        0);

	// attach above depth related render buffer to FBO at depth attachment

	gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, rbo);

	// check the frame buffer status , wether success or not

    var result = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
    if (result != gl.FRAMEBUFFER_COMPLETE) {
        console.log("FrameBuffer is not complete\n");
        return (false);
    }

	// unbind framebuffer

	gl.bindFramebuffer(gl.FRAMEBUFFER, null);

	return true;

}


function resize_cube()
{
    if(bFullScreen == true)
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else
    {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    gl.viewport(0,0,canvas.width , canvas.height);

    // set perspection projection

    mat4.perspective(perspectiveProjectionMatrix_cube , 45.0 , parseFloat(canvas.width) / parseFloat(canvas.height) , 0.1 , 100.0);

}

function resize_sphere(width, height)
{
    //CODE

    //Set Perspective Projection
    mat4.perspective(perspectiveProjectionMatrix_sphere, 45.0, parseFloat(width) / parseFloat(height), 0.1, 100.0);
    
    gl.viewport(0, 0, width, height);

}

function display_cube()
{ 
    //Render FBO scene
    if (vFboResult == true) 
    {        
        display_sphere(FBO_WIDTH, FBO_HEIGHT);
        update_sphere();
    }

    //call resize cube again to compensate change by display_Sphere
    resize_cube();

    gl.clearColor(0.85, 0.85, 0.85, 1.0);

    // code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject_cube);

    // transformation

    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -4.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_Rectangle_cube , [1.0, 0.0 , 0.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_Rectangle_cube , [0.0, 1.0 , 0.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_Rectangle_cube , [0.0, 0.0 , 1.0]); // source , target , values

    mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix_cube , modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform_cube , false , modelViewProjectionMatrix);

    gl.activeTexture(gl.TEXTURE0);
    // gl.bindTexture(gl.TEXTURE_2D , stone_texture);

    gl.bindTexture(gl.TEXTURE_2D , texture_FBO);

    gl.uniform1i(textureSamplerUniform_cube , 0);

    gl.bindVertexArray(vao_cube);

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8,  4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12,  4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16,  4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20,  4);

    gl.bindVertexArray(null);

    gl.useProgram(null);

    // call update before double buffering
    update_cube();

    // double buffering
    requestAnimationFrame(display_cube  , canvas);
}

function update_cube()
{
    //code
    angle_Rectangle_cube -= 0.02;
	if (angle_Rectangle_cube <= 360.0)
	{
		angle_Rectangle_cube += 360.0;
	}

}

function uninitialize_cube()
{
    //code

    if(shaderProgramObject_cube)
    {
        gl.useProgram(shaderProgramObject_cube);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject_cube);
        if(shaderObjects && shaderObjects.length > 0)
        {
            for (let i = 0; i < shaderObjects.length; i++) 
            {
                gl.detachShader(shaderProgramObject_cube , shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;                
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject_cube);
        shaderProgramObject_cube = null;
    }

    uninitialize_sphere();

    if(vbo_cube)
    {
        gl.deleteBuffer(vbo_cube);
        vbo_cube = null;
    }
    if(vao_cube)
    {
        gl.deleteVertexArray(vao_cube);
        vao_cube = null;
    }
}


function display_sphere(textureWidth , textureHeight)
{ 
    //bind with fbo
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    
    resize_sphere(textureWidth, textureHeight);

    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    // code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    if(bLightingEnabled_pv_sphere == true)
    {
        gl.useProgram(shaderProgramObject_pv_sphere);
    }
    else if(bLightingEnabled_pf_sphere == true)
    {
        gl.useProgram(shaderProgramObject_pf_sphere);
    }
    else
    {
        gl.useProgram(shaderProgramObject_pv_sphere);
    }


    // transformation

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelMatrix , modelMatrix , [0.0, 0.0 , -6.0]); // source , target , values
  
    // mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);

    mat4.multiply(modelMatrix , modelMatrix , viewMatrix);



    if (bLightingEnabled == true)
    {
        if(bLightingEnabled_pv_sphere == true)
        {
            gl.uniformMatrix4fv(modelMatrixUniform_pv_sphere , false , modelMatrix);
            gl.uniformMatrix4fv(viewMatrixUniform_pv_sphere , false , viewMatrix);
            gl.uniformMatrix4fv(projectionMatrixUniform_pv_sphere , false , perspectiveProjectionMatrix_sphere);

            gl.uniform1i(keyPressUniform_pv_sphere , 1);

            gl.uniform3fv(lightAmbientUniform_pv_sphere[0], lightAmbient_one_sphere );
            gl.uniform3fv(lightDiffuseUniform_pv_sphere[0], lightDiffuse_one_sphere);
            gl.uniform3fv(lightSpecularUniform_pv_sphere[0], lightSpecular_one_sphere );
    
            gl.uniform3fv(lightAmbientUniform_pv_sphere[1], lightAmbient_two_sphere );
            gl.uniform3fv(lightDiffuseUniform_pv_sphere[1], lightDiffuse_two_sphere);
            gl.uniform3fv(lightSpecularUniform_pv_sphere[1], lightSpecular_two_sphere );

            gl.uniform3fv(lightAmbientUniform_pv_sphere[2], lightAmbient_three_sphere );
            gl.uniform3fv(lightDiffuseUniform_pv_sphere[2], lightDiffuse_three_sphere);
            gl.uniform3fv(lightSpecularUniform_pv_sphere[2], lightSpecular_three_sphere );
    
            gl.uniform3fv(materialAmbientUniform_pv_sphere, materialAmbient_sphere );
            gl.uniform3fv(materialDiffuseUniform_pv_sphere, materialDiffuse_sphere );
            gl.uniform3fv(materialSpecularUniform_pv_sphere, materialSpecular_sphere );
            gl.uniform1f(materialShinenessUniform_pv_sphere, materialShineness_sphere);
    
            gl.uniform4fv(lightPositionUniform_pv_sphere[0], lightPosition_one_sphere );
            gl.uniform4fv(lightPositionUniform_pv_sphere[1], lightPosition_two_sphere );   
            gl.uniform4fv(lightPositionUniform_pv_sphere[2], lightPosition_three_sphere );

        }

        else if(bLightingEnabled_pf_sphere == true)
        {
            gl.uniformMatrix4fv(modelMatrixUniform_sphere , false , modelMatrix);
            gl.uniformMatrix4fv(viewMatrixUniform_sphere , false , viewMatrix);
            gl.uniformMatrix4fv(projectionMatrixUniform_sphere , false , perspectiveProjectionMatrix_sphere);

            gl.uniform1i(keyPressUniform_sphere , 1);

            gl.uniform3fv(lightAmbientUniform_sphere[0], lightAmbient_one_sphere );
            gl.uniform3fv(lightDiffuseUniform_sphere[0], lightDiffuse_one_sphere);
            gl.uniform3fv(lightSpecularUniform_sphere[0], lightSpecular_one_sphere );
    
            gl.uniform3fv(lightAmbientUniform_sphere[1], lightAmbient_two_sphere );
            gl.uniform3fv(lightDiffuseUniform_sphere[1], lightDiffuse_two_sphere);
            gl.uniform3fv(lightSpecularUniform_sphere[1], lightSpecular_two_sphere );

            gl.uniform3fv(lightAmbientUniform_sphere[2], lightAmbient_three_sphere );
            gl.uniform3fv(lightDiffuseUniform_sphere[2], lightDiffuse_three_sphere);
            gl.uniform3fv(lightSpecularUniform_sphere[2], lightSpecular_three_sphere );
    
            gl.uniform3fv(materialAmbientUniform_sphere, materialAmbient_sphere );
            gl.uniform3fv(materialDiffuseUniform_sphere, materialDiffuse_sphere );
            gl.uniform3fv(materialSpecularUniform_sphere, materialSpecular_sphere );
            gl.uniform1f(materialShinenessUniform_sphere, materialShineness_sphere);
    
            gl.uniform4fv(lightPositionUniform_sphere[0], lightPosition_one_sphere );
            gl.uniform4fv(lightPositionUniform_sphere[1], lightPosition_two_sphere );   
            gl.uniform4fv(lightPositionUniform_sphere[2], lightPosition_three_sphere );  

        }

    }
    else
    {
        gl.uniformMatrix4fv(modelMatrixUniform_pv_sphere , false , modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_pv_sphere , false , viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_pv_sphere , false , perspectiveProjectionMatrix_sphere);

        gl.uniform1i(keyPressUniform_pv_sphere , 0);
    }

    sphere.draw();

    gl.useProgram(null);
 
    
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);

    
    // // double buffering
    // requestAnimationFrame(display_sphere  , canvas);


}

function update_sphere()
{
    //code
    if (bLightingEnabled == true )
    {
        lightPosition_one_sphere[0] = 0.0;
        lightPosition_one_sphere[1] = 5.0 * Math.sin(lightAngleZero_sphere);
        lightPosition_one_sphere[2] = 5.0 * Math.cos(lightAngleZero_sphere);
        lightPosition_one_sphere[3] = 1.0;


        lightPosition_two_sphere[0] = 5.0 * Math.cos(lightAngleOne_sphere);
        lightPosition_two_sphere[1] = 0.0;
        lightPosition_two_sphere[2] = 5.0 * Math.sin(lightAngleOne_sphere);
        lightPosition_two_sphere[3] = 1.0;


        lightPosition_three_sphere[0] = 5.0 * Math.cos(lightAngleTwo_sphere);
        lightPosition_three_sphere[1] = 5.0 * Math.sin(lightAngleTwo_sphere);
        lightPosition_three_sphere[2] = 0.0;
        lightPosition_three_sphere[3] = 1.0;

    }
        
        lightAngleZero_sphere = lightAngleZero_sphere + 0.05;
        if (lightAngleZero_sphere >= (360.0))
        {
            lightAngleZero_sphere = lightAngleZero_sphere - (360.0);
        }
    
        lightAngleOne_sphere = lightAngleOne_sphere + 0.05;
        if (lightAngleOne_sphere >= (360.0))
        {
            lightAngleOne_sphere = lightAngleOne_sphere - (360.0);
        }
    
        lightAngleTwo_sphere = lightAngleTwo_sphere + 0.05;
        if (lightAngleTwo_sphere >= (360.0))
        {
            lightAngleTwo_sphere = lightAngleTwo_sphere - (360.0);
        }

}

function uninitialize_sphere()
{
    //code

    if(shaderProgramObject_pv_sphere)
    {
        gl.useProgram(shaderProgramObject_pv_sphere);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject_pv_sphere);
        if(shaderObjects && shaderObjects.length > 0)
        {
            for (let i = 0; i < shaderObjects.length; i++) 
            {
                gl.detachShader(shaderProgramObject_pv_sphere , shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;                
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject_pv_sphere);
        shaderProgramObject_pv_sphere = null;
    }

    if (sphere)
		{
			sphere.deallocate();
			sphere = null;
		}

}