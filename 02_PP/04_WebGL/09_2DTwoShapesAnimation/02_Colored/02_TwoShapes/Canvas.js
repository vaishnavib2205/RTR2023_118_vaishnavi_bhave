// main function
var canvas;
var gl = null; // renaming context as gl for our use
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;

// WebGL related Varaiables

const vertexAttributeEnum =
{
    AMC_ATTRIBUTE_POSITION: 0
};

var shaderProgramObject = null;

var vao = null;
var vbo = null;
var angle_Triangle = 0.0;

var vao_rectangle = null;
var vbo_rectangle = null;
var angle_Rectangle = 0.0;


var mvpMatrixUniform;
var perspectiveProjectionMatrix;


var requestAnimationFrame =
    window.requestAnimationFrame ||
    window.webkitRequestAnimationFrame ||
    window.mozRequestAnimationFrame ||
    window.oRequestAnimationFrame ||
    window.msReuestAnimationFrame;


function main() {
    // get canvas
    canvas = document.getElementById("vbb");

    if (canvas == null) {
        console.log("getting canvas failed\n");
    }
    else {
        console.log("getting canvas success \n");
    }


    // set canvas width and height for future use

    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;


    // register for keyboard events
    window.addEventListener("keydown", keyDown, false);

    // register for mouse events
    window.addEventListener("click", mouseDown, false);

    // resize
    window.addEventListener("resize", resize, false);

    initialize();

    resize();

    display();

}

function keyDown(event) {
    // alert("Key is Pressed");
    // code

    switch (event.keyCode) {
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

function mouseDown() {
    // alert("Mouse is clicked");
}

// function for fullscreen

function toggleFullscreen() {
    var fullscreen_element =
        document.fullscreenElement ||
        document.webkitFullscreenElement ||
        document.mozFullScreenElement ||
        document.msFullscreenElement ||
        null;

    // if not full screen

    if (fullscreen_element == null) {
        if (canvas.requestFullscreen) {
            canvas.requestFullscreen();
        }
        else if (canvas.webkitRequestFullscreen) {
            canvas.webkitRequestFullscreen();
        }
        else if (canvas.mozRequestFullScreen) {
            canvas.mozRequestFullScreen();
        }
        else if (canvas.msRequestFullscreen) {
            canvas.msRequestFullscreen();
        }
        bFullScreen = true;
    }
    else // if alredy full screen
    {
        if (document.exitFullscreen) {
            document.exitFullscreen();
        }
        else if (document.webkitExitFullscreen) {
            document.webkitExitFullscreen();
        }
        else if (document.mosCancelFullScreen) {
            document.mosCancelFullScreen();
        }
        else if (document.msExitFullScreen) {
            document.msExitFullScreen();
        }
        bFullScreen = false;
    }
}

function initialize() {
    //code

    // get context from above canvas
    gl = canvas.getContext("webgl2");

    if (gl == null) {
        console.log("getting webgl2 context failed \n");
    }
    else {
        console.log("getting webgl2 context success \n");
    }

    // set webgl2 context's view width and view height
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    // vertex shader

    var vertexShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "in vec4 aPosition;" +
        "uniform mat4 uMVPMatrix;" +
        "void main(void)" +
        "{" +
        "gl_Position = uMVPMatrix * aPosition;" +
        "}";

    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObject);

        if (error.length > 0) {
            var Log = "vertex shader compilation error : " + error;
            alert(Log);
            uninitialize();
        }
    }
    else {
        console.log("vertex Shader Compileed Successfully !!! \n");
    }

    var fragmentShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "FragColor = vec4(1.0f , 1.0f , 1.0f ,1.0f);" +
        "}";

    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);

    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0) {
            var Log = "Fragment SHader compilation error : " + error;
            alert(Log);
            uninitialize();
        }
    }
    else {
        console.log("fragment SHader compilation Successfully !!! \n");
    }

    // shader Program
    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    gl.bindAttribLocation(shaderProgramObject, vertexAttributeEnum.AMC_ATTRIBUTE_POSITION, "aPosition");

    gl.linkProgram(shaderProgramObject);

    if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            var Log = "shader Program Linking Error : " + error;
            alert(Log);
            uninitialize();
        }
    }
    else {
        console.log("ShaderProgeam Linked Successfully !!! \n");
    }

    mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uMVPMatrix");

    // triangle attributes

    var trianglePosition = new Float32Array([

        0.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0
    ]);

    var rectanglePosition = new Float32Array([

        1.0, 1.0, 0.0,		//glVertex3f(1.0f, 1.0f, 0.0f);
        -1.0, 1.0, 0.0,		//glVertex3f(-1.0f, 1.0f, 0.0f);
        -1.0, -1.0, 0.0,	//glVertex3f(-1.0f, -1.0f, 0.0f);
        1.0, -1.0, 0.0		//glVertex3f(1.0f, -1.0f, 0.0f);
    ]);

    // vao triangle

    {
        vao = gl.createVertexArray();
        gl.bindVertexArray(vao);

        // vbo

        vbo = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo);

        gl.bufferData(gl.ARRAY_BUFFER, trianglePosition, gl.STATIC_DRAW);
        gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        gl.bindVertexArray(null);
    }

    // vao rectangle

    {
        vao_rectangle = gl.createVertexArray();
        gl.bindVertexArray(vao_rectangle);

        // vbo

        vbo_rectangle = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_rectangle);

        gl.bufferData(gl.ARRAY_BUFFER, rectanglePosition, gl.STATIC_DRAW);
        gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        gl.bindVertexArray(null);

    }




    // depth initialise
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    // initialise projection matrix

    perspectiveProjectionMatrix = mat4.create();

}

function resize() {
    //code
    if (bFullScreen == true) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    gl.viewport(0, 0, canvas.width, canvas.height);

    // set perspection projection

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);

}

function display() {
    // code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    // transformation

    // triangle
    {
        var modelViewMatrix = mat4.create();
        var modelViewProjectionMatrix = mat4.create();

        mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -4.0]); // source , target , values
        mat4.rotate(modelViewMatrix, modelViewMatrix, angle_Triangle, [0.0, 1.0, 0.0]); // source , target , values

        mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
        gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

        gl.bindVertexArray(vao);
        gl.drawArrays(gl.TRIANGLES, 0, 3);
        gl.bindVertexArray(null);
    }


    // rectangle
    {
        modelViewMatrix = mat4.create();
        modelViewProjectionMatrix = mat4.create();

        mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -4.0]); // source , target , values
        mat4.rotate(modelViewMatrix, modelViewMatrix, angle_Rectangle, [1.0, 0.0, 0.0]); // source , target , values


        mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
        gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

        gl.bindVertexArray(vao_rectangle);
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
        gl.bindVertexArray(null);
    }



    gl.useProgram(null);

    // call update before double buffering
    update();

    // double buffering
    requestAnimationFrame(display, canvas);
}

function update() {
    //code

    angle_Triangle += 0.02;
    if (angle_Triangle >= 360.0) {
        angle_Triangle -= 360.0;
    }

    angle_Rectangle += 0.02;
    if (angle_Rectangle >= 360.0) {
        angle_Rectangle -= 360.0;
    }

}

function uninitialize() {
    //code

    if (shaderProgramObject) {
        gl.useProgram(shaderProgramObject);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject);
        if (shaderObjects && shaderObjects.length > 0) {
            for (let i = 0; i < shaderObjects.length; i++) {
                gl.detachShader(shaderProgramObject, shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }

    if (vbo) {
        gl.deleteBuffer(vbo);
        vbo = null;
    }
    if (vao) {
        gl.deleteVertexArray(vao);
        vao = null;
    }
}