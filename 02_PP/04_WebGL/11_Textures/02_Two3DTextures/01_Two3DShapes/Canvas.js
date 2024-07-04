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
    AMC_ATTRIBUTE_TEXCOORDS: 1
};

var shaderProgramObject = null;

var vao_pyramid = null;
var vbo_pyramid = null;
var vao_cube = null;
var vbo_cube = null;


var angle_Cube = 0.0;
var angle_Pyramid = 0.0;

var vbo_cube_texture = null;
var vbo_pyramid_texture = null;

var mvpMatrixUniform ;
var perspectiveProjectionMatrix;

var stone_texture;

var kundali_texture ;
var textureSamplerUniform;

var requestAnimationFrame =
window.requestAnimationFrame ||
window.webkitRequestAnimationFrame ||
window.mozRequestAnimationFrame ||
window.oRequestAnimationFrame ||
window.msReuestAnimationFrame;


function main()
{
    // get canvas
    canvas = document.getElementById("vb");

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
    gl.bindAttribLocation(shaderProgramObject , vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS , "aTexCoord"); 
    
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

    mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject , "uMVPMatrix");
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject , "uTextureSampler");

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
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,

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

    // triangle attributes

    var pyramidPosition = new Float32Array([

        // front
        0.0, 1.0, 0.0,
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,

        // right
        0.0, 1.0, 0.0,
        1.0, -1.0, 1.0,
        1.0, -1.0, -1.0,

        // back
        0.0, 1.0, 0.0,
        1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,

        // left
        0.0, 1.0, 0.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, 1.0
    ]);

    var pyramidTexCoord = new Float32Array([

        // front
        0.5, 1.0, // front-top
        0.0, 0.0, // front-left
        1.0, 0.0, // front-right

        // right
        0.5, 1.0, // right-top
        1.0, 0.0, // right-left
        0.0, 0.0, // right-right

        // back
        0.5, 1.0, // back-top
        0.0, 0.0, // back-left
        1.0, 0.0, // back-right

        // left
        0.5, 1.0, // left-top
        1.0, 0.0, // left-left
        0.0, 0.0 // left-right
    ]);


    // vao
    { 
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    // vbo

    vbo_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_cube);

    gl.bufferData(gl.ARRAY_BUFFER, cubePosition, gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo_cube_texture

    vbo_cube_texture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_cube_texture);

    gl.bufferData(gl.ARRAY_BUFFER, cubeTexCoord, gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS , 2 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);
    gl.bindVertexArray(null);

    }


    {
        vao_pyramid = gl.createVertexArray();
        gl.bindVertexArray(vao_pyramid);

        // vbo

        vbo_pyramid = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid);

        gl.bufferData(gl.ARRAY_BUFFER, cubePosition, gl.STATIC_DRAW);
        gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        // vbo_pyramid_texture

        vbo_pyramid_texture = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_texture);

        gl.bufferData(gl.ARRAY_BUFFER, cubeTexCoord, gl.STATIC_DRAW);
        gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.bindVertexArray(null);

    }

    // depth initialise
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);


    //load image to create texture
    stone_texture = loadGLTexture("stone.png");
    kundali_texture = loadGLTexture("Vijay_Kundali.png");


    // set clear color
    gl.clearColor(0.0 , 0.0 , 0.0 , 1.0);

    // initialise projection matrix

    perspectiveProjectionMatrix = mat4.create();

}

function loadGLTexture(imageFileName)
{
    var texture = gl.createTexture();

    texture.image = new Image();

    console.log("before src load !!! \n");

    texture.image.src = imageFileName;

    console.log("after src load !!! \n");

    texture.image.onLoad = Function()
    {
        gl.bindTexture(gl.TEXTURE_2D , texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL , true);
        gl.texParameteri(gl.TEXTURE_2D , gl.TEXTURE_MAG_FILTER , gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D , gl.TEXTURE_MIN_FILTER , gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D , 0 , gl.RGBA , gl.RGBA , gl.UNSIGNED_BYTE , texture.image);
        gl.generateMipmap(gl.TEXTURE_2D);
        gl.bindTexture(gl.TEXTURE_2D , null);   

    }

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

    // Pyramid
    {
        var modelViewMatrix = mat4.create();
        var modelViewProjectionMatrix = mat4.create();

        mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -4.0]); // source , target , values
        mat4.rotate(modelViewMatrix, modelViewMatrix, angle_Pyramid, [0.0, 1.0, 0.0]); // source , target , values

        mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
        gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, stone_texture);
        gl.uniform1i(textureSamplerUniform, 0);

        gl.bindVertexArray(vao_pyramid);
        gl.drawArrays(gl.TRIANGLES, 0, 12);
        gl.bindVertexArray(null);
    }

    { 
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [1.0, 0.0, -4.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_Cube , [1.0 , 0.0 , 0.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_Cube , [0.0 , 1.0 , 0.0]); // source , target , values
    mat4.rotate(modelViewMatrix , modelViewMatrix , angle_Cube , [0.0 , 0.0 , 1.0]); // source , target , values

    mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D , kundali_texture);
    gl.uniform1i(textureSamplerUniform , 0);

        gl.bindVertexArray(vao_cube);

    gl.drawArrays(gl.TRIANGLES_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    gl.bindVertexArray(null);

    }
    gl.useProgram(null);

    // call update before double buffering
    update();

    // double buffering
    requestAnimationFrame(display  , canvas);
}

function update()
{
    //code
    angle_Pyramid += 0.02;
    if (angle_Pyramid >= 360.0) {
        angle_Pyramid -= 360.0;
    }

    angle_Cube += 0.02;
    if (angle_Cube >= 360.0) {
        angle_Cube -= 360.0;
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