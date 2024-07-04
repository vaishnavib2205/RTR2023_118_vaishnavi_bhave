// main function
var canvas;
var gl = null; // renaming context as gl for our use
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;

// WebGL related Varaiables

const vertexAttributeEnum = 
{
    AMC_ATTRIBUTE_POSITION:0,
    AMC_ATTRIBUTE_NORMAL: 1
};

var shaderProgramObject = null;

var vao_cube = null;
var vbo_position_cube = null;
var vbo_normal_cube = null;
var angle_cube = 0.0;

var perspectiveProjectionMatrix;

//variables related to light
var ldUniform = null;
var kdUniform = null;
var lightPositionUniform = null;
var keypressedUniform = null;
var ModelViewMatrixUniform = null;
var projectionMatrixUniform = null;
var keyPressedUniform = null;

var bLightEnabled = false;
var nAnimationEnabled = false;

var lightAmbient = [0.5, 0.5, 0.5];
var lightDiffuse = [1.0, 1.0, 1.0];
var materialDiffuse = [0.5, 0.5, 0.5];
var lightPosition = [100.0, 100.0, 100.0, 1.0];


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
    window.addEventListener("resize" , resize , false);

    initialize();

    resize();

    display();
            
}

function keyDown(event)
{
    // alert("Key is Pressed");
    // code

    switch (event.keyCode) 
    {
        case 81:
        case 113:
            uninitialize();
            window.close();

            break;
        case 70:
        case 102:

            toggleFullscreen();

            break;

        case 76:
        case 108:
            if (bLightEnabled == false) {
                bLightEnabled = true;
            } else {
                bLightEnabled = false;
            }
    
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

function initialize()
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
    "in vec4 aPosition;" +
    "in vec3 aNormal;" +
    "uniform mat4 uModelViewMatrix;" +
    "uniform mat4 uProjectionMatrix;" +
    "uniform vec3 uld;" +
    "uniform vec3 ukd;" +
    "uniform mediump int ukeypressed;" +
    "uniform vec4 uLightPosition;" +
    "out vec3 oDiffuseLight;" +
    "in vec4 aColor;" +
    "out vec4 oColor;" +
    "void main(void)"+
    "{"+
    "if(ukeypressed == 1)" +
    "{" +
    "vec4 eyePosition = uModelViewMatrix * aPosition;" +
    "mat3 normalMatrix = mat3(transpose(inverse(uModelViewMatrix)));" +
    "vec3 n = normalize(normalMatrix * aNormal);" +
    "vec3 s = normalize(vec3(uLightPosition - eyePosition));" +
    "oDiffuseLight = uld * ukd * dot(s,n);" +
    "}" +
    "else" +
    "{" +
    "oDiffuseLight = vec3(1.0f, 1.0f, 1.0f);" +
    "}" +
    "gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;" +
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
            uninitialize();
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
    "out vec4 FragColor;" +
    "in vec3 oDiffuseLight;" +
    "uniform mediump int ukeypressed;" +
    "void main(void)"+
    "{"+
    "if(ukeypressed == 1)" +
    "{" +
    "FragColor = vec4(oDiffuseLight, 1.0);" +
    "}" +
    "else" +
    "{" +
    "FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);" +
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
            uninitialize();
        }
    }
    else
    {
        console.log("fragment SHader compilation Successfully !!! \n");
    }

    // shader Program
    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject , vertexShaderObject);
    gl.attachShader(shaderProgramObject , fragmentShaderObject);

    gl.bindAttribLocation(shaderProgramObject , vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , "aPosition");  
    gl.bindAttribLocation(shaderProgramObject , vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL , "aNormal");  
    
    gl.linkProgram(shaderProgramObject);

    if(gl.getProgramParameter(shaderProgramObject , gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if(error.length > 0)
        {
            var Log = "shader Program Linking Error : " + error;
            alert(Log);
            uninitialize();
        }
    }
    else
    {
        console.log("ShaderProgeam Linked Successfully !!! \n");
    }



    ModelViewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "uld");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "ukd");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "uLightPosition");
    keypressedUniform = gl.getUniformLocation(shaderProgramObject, "ukeypressed");
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

    var cubeNormals = new Float32Array([

        // front surface
        0.0, 0.0, 1.0, // top-right of front
        0.0, 0.0, 1.0, // top-left of front
        0.0, 0.0, 1.0, // bottom-left of front
        0.0, 0.0, 1.0, // bottom-right of front

        // right surface
        1.0, 0.0, 0.0, // top-right of right
        1.0, 0.0, 0.0, // top-left of right
        1.0, 0.0, 0.0, // bottom-left of right
        1.0, 0.0, 0.0, // bottom-right of right

        // back surface
        0.0, 0.0, - 1.0, // top-right of back
        0.0, 0.0, -1.0, // top-left of back
        0.0, 0.0, -1.0, // bottom-left of back
        0.0, 0.0, -1.0, // bottom-right of back

        // left surface
        -1.0, 0.0, 0.0, // top-right of left
        -1.0, 0.0, 0.0, // top-left of left
        -1.0, 0.0, 0.0, // bottom-left of left
        -1.0, 0.0, 0.0, // bottom-right of left

        // top surface
        0.0, 1.0, 0.0, // top-right of top
        0.0, 1.0, 0.0, // top-left of top
        0.0, 1.0, 0.0, // bottom-left of top
        0.0, 1.0, 0.0, // bottom-right of top

        // bottom surface
        0.0, -1.0, 0.0, // top-right of bottom
        0.0, -1.0, 0.0, // top-left of bottom
        0.0, -1.0, 0.0, // bottom-left of bottom
        0.0, -1.0, 0.0, // bottom-right of bottom
    ]);

    // vao

    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    // vbo

    vbo_position_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_cube);

    gl.bufferData(gl.ARRAY_BUFFER, cubePosition , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //for lights = normal
    vbo_normal_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,  vbo_normal_cube);

    gl.bufferData(gl.ARRAY_BUFFER, cubeNormals, gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    gl.bindVertexArray(null);

    // depth initialise
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // set clear color
    gl.clearColor(0.0 , 0.0 , 0.0 , 1.0);

    // initialise projection matrix

    perspectiveProjectionMatrix = mat4.create();

}

function resize()
{
    //code
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

    mat4.perspective(perspectiveProjectionMatrix , 45.0 , parseFloat(canvas.width) / parseFloat(canvas.height) , 0.1 , 100.0);

}

function display()
{ 
    // code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    // transformation

    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -4.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_cube , [1.0, 0.0 , 0.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_cube , [0.0, 1.0 , 0.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_cube , [0.0, 0.0 , 1.0]); // source , target , values

    gl.uniformMatrix4fv(ModelViewMatrixUniform, false, modelViewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    if (bLightEnabled == true) {
        gl.uniform1i(keypressedUniform, 1);
        gl.uniform3fv(ldUniform, lightDiffuse);
        gl.uniform3fv(kdUniform, materialDiffuse);
        gl.uniform4fv(lightPositionUniform, lightPosition);
    }
    else {
        gl.uniform1i(keypressedUniform, 0);
    }

    gl.bindVertexArray(vao_cube);

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    gl.bindVertexArray(null);

    gl.useProgram(null);

    // call update before double buffering
    update();

    // double buffering
    requestAnimationFrame(display  , canvas);
}

function update()
{
    //code
    angle_cube = angle_cube - 0.006;
    if (angle_cube <= 0.0) {
        angle_cube = angle_cube + 360.0;
    }

}

function uninitialize()
{
    //code

    if(shaderProgramObject)
    {
        gl.useProgram(shaderProgramObject);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject);
        if(shaderObjects && shaderObjects.length > 0)
        {
            for (let i = 0; i < shaderObjects.length; i++) 
            {
                gl.detachShader(shaderProgramObject , shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;                
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }

    if(vbo)
    {
        gl.deleteBuffer(vbo);
        vbo = null;
    }
    if(vao)
    {
        gl.deleteVertexArray(vao);
        vao = null;
    }
}