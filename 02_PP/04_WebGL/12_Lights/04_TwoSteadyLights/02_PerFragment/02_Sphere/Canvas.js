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
    AMC_ATTRIBUTE_TEXTURE:1,
    AMC_ATTRIBUTE_NORMAL:2
};

var shaderProgramObject = null;

var sphere = null;


var modelMatrixUniform = 0;
var projectionMatrixUniform = 0;
var viewMatrixUniform = 0;
var lightDiffuseUniform = new Array(2); // for diffuse light
var materialDiffuseUniform = 0; // for diffuse materia
var lightAmbientUniform = new Array(2); // for ambient light
var materialAmbientUniform = 0; // for ambient materia
var lightSpecularUniform = new Array(2); // for specular light
var materialSpecularUniform = 0; // for specular material
var materialShinenessUniform = 0; // for shinneness materia
var lightPositionUniform = new Array(2);
var keyPressUniform = 0;

var bLightingEnabled = false;

var lightAmbient_one = [ 0.0 , 0.0 , 0.0  ];
var lightDiffuse_one = [ 1.0 , 0.0 , 0.0 ]; // white diffuse light
var lightSpecular_one = [ 1.0 , 0.0 , 0.0  ]; // white diffuse
var lightPosition_one = [ -200.0 , 0.0 , 0.0 , 1.0 ];

var lightAmbient_two = [ 0.0 , 0.0 , 0.0  ];
var lightDiffuse_two = [ 0.0 , 0.0 , 1.0 ]; // white diffuse light
var lightSpecular_two = [ 0.0 , 0.0 , 1.0  ]; // white diffuse
var lightPosition_two = [ 200.0 , 0.0 , 0.0 , 1.0 ];

var materialAmbient = [ 0.0 , 0.0 , 0.0 ];
var materialDiffuse = [ 1.0 , 1.0 , 1.0 ];
var materialSpecular = [ 1.0 , 1.0 , 1.0 ];
var materialShineness = 128.0;

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
            if (bLightingEnabled == false)
            {
                bLightingEnabled = true;
            }
            else
            {
                bLightingEnabled = false;
            }
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
    "uniform mediump int uKeyPress;" +
    "in vec4 oColor;" +
    "in vec3 otransformedNormals;" +
    "in vec3 oviewerVector;" +
    "in vec4 oeyeCoordinates;" +
    "out vec4 FragColor;" +
    "uniform vec3 uLightAmbient[2];" +
    "uniform vec3 uLightDiffuse[2];" +
    "uniform vec3 uLightSpecular[2];" +
    "uniform vec4 uLightposition[2];" +
    "uniform vec3 uMaterialAmbient;" +
    "uniform vec3 uMaterialDiffuse;" +
    "uniform vec3 uMaterialSpecular;" +
    "uniform float uMaterialShineness;" +
    "void main(void)" +
    "{" +
    "if(uKeyPress == 1)" +
    "{" +
    "vec3 oFong_ADS_Light;" +
    "vec3 ambientLight[2];" +
    "vec3 diffuseLight[2];" +
    "vec3 specularLight[2]; " +
    "vec3 lightDirection[2];" +
    "vec3 reflectionVector[2];" +
    "vec3 normalizedTranformedNormals = normalize(otransformedNormals);" +
    "vec3 normalizedViewerVector= normalize(oviewerVector);" +
    "for(int i = 0 ; i <2;i++)" +
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

    // mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject , "uMVPMatrix");

    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");

    lightAmbientUniform[0] = gl.getUniformLocation(shaderProgramObject, "uLightAmbient[0]");
    lightDiffuseUniform[0] = gl.getUniformLocation(shaderProgramObject, "uLightDiffuse[0]");
    lightSpecularUniform[0] = gl.getUniformLocation(shaderProgramObject, "uLightSpecular[0]");
    lightPositionUniform[0] = gl.getUniformLocation(shaderProgramObject, "uLightposition[0]");

    lightAmbientUniform[1] = gl.getUniformLocation(shaderProgramObject, "uLightAmbient[1]");
    lightDiffuseUniform[1] = gl.getUniformLocation(shaderProgramObject, "uLightDiffuse[1]");
    lightSpecularUniform[1] = gl.getUniformLocation(shaderProgramObject, "uLightSpecular[1]");
    lightPositionUniform[1] = gl.getUniformLocation(shaderProgramObject, "uLightposition[1]");

    materialAmbientUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialAmbient");
    materialDiffuseUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialDiffuse");
    materialSpecularUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialSpecular");
    materialShinenessUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialShineness");
    keyPressUniform = gl.getUniformLocation(shaderProgramObject, "uKeyPress");


    sphere = new Mesh();
    makeSphere(sphere, 2.0, 50, 50);

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
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelMatrix , modelMatrix , [0.0, 0.0 , -6.0]); // source , target , values

    mat4.multiply(modelMatrix , modelMatrix , viewMatrix);

    gl.uniformMatrix4fv(modelMatrixUniform , false , modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform , false , viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform , false , perspectiveProjectionMatrix);


    if (bLightingEnabled == true)
    {
        gl.uniform1i(keyPressUniform, 1);
        gl.uniform3fv(lightAmbientUniform[0], lightAmbient_one );
        gl.uniform3fv(lightDiffuseUniform[0], lightDiffuse_one);
        gl.uniform3fv(lightSpecularUniform[0], lightSpecular_one );

        gl.uniform3fv(lightAmbientUniform[1], lightAmbient_two );
        gl.uniform3fv(lightDiffuseUniform[1], lightDiffuse_two);
        gl.uniform3fv(lightSpecularUniform[1], lightSpecular_two );

        gl.uniform3fv(materialAmbientUniform, materialAmbient );
        gl.uniform3fv(materialDiffuseUniform, materialDiffuse );
        gl.uniform3fv(materialSpecularUniform, materialSpecular );
        gl.uniform1f(materialShinenessUniform, materialShineness);

        gl.uniform4fv(lightPositionUniform[0], lightPosition_one );
        gl.uniform4fv(lightPositionUniform[1], lightPosition_two );   
    }
    else
    {
        gl.uniform1i(keyPressUniform, 0);
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

    if (sphere)
		{
			sphere.deallocate();
			sphere = null;
		}
}