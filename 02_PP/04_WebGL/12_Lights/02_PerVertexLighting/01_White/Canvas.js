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
    AMC_ATTRIBUTE_NORMAL: 1,
    AMC_ATTRIBUTE_TEXTURE : 2
};

var shaderProgramObject = null;

//sphere init
var sphere = null;

//variables related to light
var lightDiffuseUniform = null;
var lightAmbientUniform = null;
var lightSpecularUniform = null;
var lightPositionUniform = null;


var keypressedUniform = null;

var ModelMatrixUniform = null;
var ViewMatrixUniform = null;
var projectionMatrixUniform = null;
var keyPressedUniform = null;

var bLightingEnabled = false;
var nAnimationEnabled = false;

var lightAmbient = [0.5, 0.5, 0.5];
var lightDiffuse = [1.0, 1.0, 1.0];
var lightSpecular = [1.0, 1.0, 1.0];
var lightPosition = [100.0, 100.0, 100.0, 1.0];

var materialAmbient = [ 0.0, 0.0, 0.0];
var materialDiffuse = [1.0, 1.0, 1.0];
var materialSpecular = [1.0, 1.0, 1.0];
var materialShininess = 50.0;

var perspectiveProjectionMatrix;

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
            if (bLightingEnabled == false) {
                bLightingEnabled = true;
            } else {
                bLightingEnabled = false;
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
    "uniform mat4 uModelMatrix;" +
    "uniform mat4 uViewMatrix;" +
    "uniform mat4 uProjectionMatrix;" +
    "uniform vec3 ulightAmbient;" +
    "uniform vec3 ulightDiffuse;" +
    "uniform vec3 ulightSpecular;" +
    "uniform vec4 uLightPosition;" +
    "uniform vec3 umaterialAmbient;" +
    "uniform vec3 umaterialDiffuse;" +
    "uniform vec3 umaterialSpecular;" +
    "uniform float umaterialShininess;" +
    "uniform mediump int ukeypressed;" +
    "out vec3 ophong_ads_light;" +
    "in vec4 aColor;" +
    "out vec4 oColor;" +
    "void main(void)" +
    "{" +
    "if(ukeypressed == 1)" +
    "{" +
    "vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;" +
    "vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);" +
    "vec3 lightDirection = normalize(vec3(uLightPosition - eyeCoordinates));" +
    "vec3 reflectionVector = reflect(-lightDirection, transformedNormals);" +
    "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" +
    "vec3 ambientLight = ulightAmbient * umaterialAmbient;" +
    "vec3 diffuseLight = ulightDiffuse * umaterialDiffuse * max(dot(lightDirection, transformedNormals), 0.0f);" +
    "vec3 specularLight = ulightSpecular * umaterialSpecular * (pow(max(dot(reflectionVector, viewerVector), 0.0f), umaterialShininess));" +
    "ophong_ads_light = ambientLight + diffuseLight + specularLight;" +
    "}" +
    "else" +
    "{" +
    "ophong_ads_light = vec3(0.0f , 0.0f , 0.0f);" +
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
    "in vec4 oColor;" +
    "in vec3 ophong_ads_light;" +
    "out vec4 FragColor;" +
    "uniform mediump int ukeypressed;" +
    "void main(void)" +
    "{" +
    "if(ukeypressed == 1)" +
    "{" +
    "FragColor = vec4(ophong_ads_light, 1.0);" +
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

    ModelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelMatrix");
    ViewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");
    lightAmbientUniform = gl.getUniformLocation(shaderProgramObject, "ulightAmbient");
    lightDiffuseUniform = gl.getUniformLocation(shaderProgramObject, "ulightDiffuse");
    lightSpecularUniform = gl.getUniformLocation(shaderProgramObject, "ulightSpecular");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "uLightPosition");
    materialAmbientUniform = gl.getUniformLocation(shaderProgramObject, "umaterialAmbient");
    materialDiffuseUniform = gl.getUniformLocation(shaderProgramObject, "umaterialDiffuse");
    materialSpecularUniform = gl.getUniformLocation(shaderProgramObject, "umaterialSpecular");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "umaterialShininess");
    keypressedUniform = gl.getUniformLocation(shaderProgramObject, "ukeypressed");
 

    // create sphere
    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);

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

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var ModelViewProjectionMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]); // source , target , values

    // mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);

    mat4.multiply(modelMatrix, modelMatrix, viewMatrix);

    gl.uniformMatrix4fv(ModelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(ViewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    if (bLightingEnabled == true) {


        gl.uniform1i(keypressedUniform, 1);
        gl.uniform3fv(lightAmbientUniform, lightAmbient);
        gl.uniform3fv(lightDiffuseUniform, lightDiffuse);
        gl.uniform3fv(lightSpecularUniform, lightSpecular);
        gl.uniform3fv(materialAmbientUniform, materialAmbient);
        gl.uniform3fv(materialDiffuseUniform, materialDiffuse);
        gl.uniform3fv(materialSpecularUniform, materialSpecular);
        gl.uniform4fv(lightPositionUniform, lightPosition);
        gl.uniform1f(materialShininessUniform, materialShininess);
    }
    else {
        gl.uniform1i(keypressedUniform, 0);
    }

    sphere.draw();
    gl.useProgram(null);

    // call update before double buffering
    update();

    // double buffering
    requestAnimationFrame(display  , canvas);
}

function update()
{
    //code
    
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

    if (sphere) {
        sphere.deallocate();
        sphere = null;
    }

}