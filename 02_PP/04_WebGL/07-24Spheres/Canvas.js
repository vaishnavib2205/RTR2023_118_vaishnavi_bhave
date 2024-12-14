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

var vao = null;
var vbo_position = null;
var vbo_normal = null;

// Uniform
var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var lightAmbientUniform;
var lightDiffuseUniform;
var lightSpecularUniform;
var lightPossitionUniform;

var materialAmbientUniform;
var materialDiffuseUniform;
var materialSpecularUniform;
var materialShininessUniform;

var keyPressUniform;

var bLightingEnable = false;

var lightAmbient =  [1.0, 1.0, 1.0];
var lightDiffuse =  [1.0, 1.0, 1.0];
var lightSpecular = [1.0, 1.0, 1.0];
var lightPossition = [0.0, 0.0, 0.0, 1.0];

var materialAmbient = [];
var materialDiffuse = [];
var materialSpecular = [];
var  materialShininess = 128.0;

var perspectiveProjectionMatrix;

var modelViewMatrix;

var sphere = null;

var angle = 0.0;

var check = null;
var vHeight;
var vWidth;

var requestAnimationFrame =
	window.requestAnimationFrame ||
	window.webkitRequestAnimationFrame ||
	window.mozRequestAnimationFrame ||
	window.oRequestAnimationFrame ||
	window.msRequestAnimationFrame;



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

            case 108:
                case 76:
                    if (bLightingEnable == false)
                        bLightingEnable = true;
                    else
                        bLightingEnable = false;
                    break;
                case 88:
                case 120:
                    check = 'x';
                    break;
        
                case 89:
                case 121:
                    check = 'y';
                    break;
        
                case 90:
                case 122:
                    check = 'z';
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
    "uniform vec4 uLightposition;" +
    "out vec3 otransformedNormals;" +
    "out vec3 olightDirection;" +
    "out vec3 oviewerVector;" +
    "in vec4 aColor;" +
    "out vec4 oColor;" +
    "void main(void)" +
    "{" +
    "if(uKeyPress == 1)" +
    "{" +
    "vec4 eyeCoordinates =  uViewMatrix * uModelMatrix * aPosition;" +
    "otransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;" +
    "olightDirection = vec3(uLightposition - eyeCoordinates);" +
    "oviewerVector = -eyeCoordinates.xyz;" +
    "}" +
    "else" +
    "{" +
    "otransformedNormals = vec3(0.0f , 0.0f , 0.0f);" +
    "olightDirection = vec3(0.0f , 0.0f , 0.0f);" +
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
    "in vec3 olightDirection;" +
    "in vec3 oviewerVector;" +
    "out vec4 FragColor;" +
    "uniform vec3 uLightAmbient;" +
    "uniform vec3 uLightDiffuse;" +
    "uniform vec3 uLightSpecular;" +
    "uniform vec3 uMaterialAmbient;" +
    "uniform vec3 uMaterialDiffuse;" +
    "uniform vec3 uMaterialSpecular;" +
    "uniform float uMaterialShineness;" +
    "void main(void)" +
    "{" +
    "vec3 Phong_ADS_Light;" +
    "if(uKeyPress == 1)" +
    "{" +
    "vec3 normalizedTranformedNormals = normalize(otransformedNormals);" +
    "vec3 normalizedLightDirection = normalize(olightDirection);" +
    "vec3 normalizedViewerVector= normalize(oviewerVector);" +
    "vec3 ambientLight = uLightAmbient * uMaterialAmbient;" +
    "vec3 diffuseLight = uLightDiffuse * uMaterialDiffuse * max(dot(normalizedLightDirection , normalizedTranformedNormals) , 0.0f); " +
    "vec3 reflectionVector = reflect(-normalizedLightDirection , normalizedTranformedNormals);" +
    "vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector , normalizedViewerVector) , 0.0f) , uMaterialShineness); " +
    "Phong_ADS_Light = ambientLight + diffuseLight + specularLight;" +
    "}" +
    "else" +
    "{" +
    "Phong_ADS_Light = vec3(1.0f , 1.0f , 1.0f );" +
    "}" +
    "FragColor = vec4(Phong_ADS_Light , 1.0f);" +
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
    shaderProgramObject_pf = gl.createProgram();
    gl.attachShader(shaderProgramObject_pf , vertexShaderObject);
    gl.attachShader(shaderProgramObject_pf , fragmentShaderObject);

    gl.bindAttribLocation(shaderProgramObject_pf , vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , "aPosition");  
    gl.bindAttribLocation(shaderProgramObject_pf , vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL , "aNormal");  
    
    gl.linkProgram(shaderProgramObject_pf);

    if(gl.getProgramParameter(shaderProgramObject_pf , gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramInfoLog(shaderProgramObject_pf);
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

    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject_pf, "uModelMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject_pf, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject_pf, "uProjectionMatrix");
    lightAmbientUniform = gl.getUniformLocation(shaderProgramObject_pf, "uLightAmbient");
    lightDiffuseUniform = gl.getUniformLocation(shaderProgramObject_pf, "uLightDiffuse");
    lightSpecularUniform = gl.getUniformLocation(shaderProgramObject_pf, "uLightSpecular");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject_pf, "uLightposition");
    materialAmbientUniform = gl.getUniformLocation(shaderProgramObject_pf, "uMaterialAmbient");
    materialDiffuseUniform = gl.getUniformLocation(shaderProgramObject_pf, "uMaterialDiffuse");
    materialSpecularUniform = gl.getUniformLocation(shaderProgramObject_pf, "uMaterialSpecular");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject_pf, "uMaterialShineness");
    keyPressUniform = gl.getUniformLocation(shaderProgramObject_pf, "uKeyPress");


	sphere = new Mesh();
	makeSphere(sphere, 1.0, 50, 50);


    // depth initialise
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // set clear color
	gl.clearColor(0.5, 0.5, 0.5, 1.0);

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

function spheres(a1, a2, a3, d1, d2, d3, s1, s2, s3, sh, x, y) {



	gl.useProgram(shaderProgramObject);

	// transformation
	
	if (bLightingEnable == true) {
		gl.uniform1i(keyPressUniform, 1);

		gl.uniform3fv(lightAmbientUniform, lightAmbient);
		gl.uniform3fv(lightDiffuseUniform, lightDiffuse);
		gl.uniform3fv(lightSpecularUniform, lightSpecular);
		gl.uniform4fv(lightPossitionUniform, lightPossition);
	
		materialAmbient = [a1, a2, a3];
		materialDiffuse = [d1, d2, d3];
		materialSpecular = [s1, s2, s3];

		materialShininess = sh;

		gl.uniform3fv(materialAmbientUniform, materialAmbient);
		gl.uniform3fv(materialDiffuseUniform, materialDiffuse);
		gl.uniform3fv(materialSpecularUniform, materialSpecular);

		gl.uniform1f(materialShininessUniform, materialShininess);
	}
	else {
		gl.uniform1i(keyPressUniform, 0);
	}

	modelMatrix = mat4.create();
	var viewMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [x, y, 0.0]);
	mat4.lookAt(viewMatrix, [0.0, 0.0,6.0], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0]);

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);


	sphere.draw();
    
	gl.useProgram(null);

}

function display() {
	// Code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);



	gl.viewport(0, vHeight * (5.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0215, 0.1745, 0.0215, 0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.6 * 128, 0.0, 0.0);

	gl.viewport(0, vHeight * (4.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.135, 0.2225, 0.1575, 0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1 * 128, 0.0, 0.0);

	gl.viewport(0, vHeight * (3.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.05375, 0.05, 0.06625, 0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3 * 128, 0.0, 0.0);

	gl.viewport(0, vHeight * (2.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	//spheres(0.05375, 0.05, 0.06625, 0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3 * 128, 0.0, 0.0);
	spheres(0.25, 0.20725, 0.20725, 1.0, 0.829, 0.829, 0.296648, 0.296648, 0.296648, 0.088 * 128, 0.0, 0.0);

	gl.viewport(0, vHeight * (1.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.1745, 0.01175, 0.01175, 0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6 * 128, 0.0, 0.0);

	gl.viewport(0, vHeight * (0.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.1, 0.18725, 0.1745, 0.396, 0.74151, 0.69102, 0.297254, 0.30829, 0.306678, 0.1 * 128, 0.0, 0.0);

	// 2nd column
	gl.viewport(vWidth * (1.0 / 4.0), vHeight * (5.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.329412, 0.223529, 0.027451, 0.780392, 0.568627, 0.113725, 0.992157, 0.941176, 0.807843, 0.21794872 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (1.0 / 4.0), vHeight * (4.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.2125, 0.1275, 0.054, 0.714, 0.4284, 0.18144, 0.393548, 0.271906, 0.166721, 0.2 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (1.0 / 4.0), vHeight * (3.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.25, 0.25, 0.25, 0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (1.0 / 4.0), vHeight * (2.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.19125, 0.0735, 0.0225, 0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.6 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (1.0 / 4.0), vHeight * (1.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.24725, 0.1995, 0.0745, 0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 0.4 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (1.0 / 4.0), vHeight * (0.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.19225, 0.19225, 0.19225, 0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4 * 128, 0.0, 0.0);

	// 3rd Column
	gl.viewport(vWidth * (2.0 / 4.0), vHeight * (5.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0, 0.0, 0.0, 0.01, 0.01, 0.01, 0.50, 0.50, 0.50, 0.25 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (2.0 / 4.0), vHeight * (4.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0, 0.1, 0.06, 0.0, 0.50980392, 0.50980392, 0.50196078, 0.50196078, 0.50196078, 0.25 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (2.0 / 4.0), vHeight * (3.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0, 0.0, 0.0, 0.1, 0.35, 0.1, 0.45, 0.55, 0.45, 0.25 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (2.0 / 4.0), vHeight * (2.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.7, 0.6, 0.6, 0.25 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (2.0 / 4.0), vHeight * (1.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0, 0.0, 0.0, 0.55, 0.55, 0.55, 0.70, 0.70, 0.70, 0.25 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (2.0 / 4.0), vHeight * (0.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.60, 0.60, 0.50, 0.25 * 128, 0.0, 0.0);

	// 4th column
	gl.viewport(vWidth * (3.0 / 4.0), vHeight * (5.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.4, 0.4, 0.4, 0.078125 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (3.0 / 4.0), vHeight * (4.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0, 0.05, 0.05, 0.4, 0.5, 0.5, 0.04, 0.7, 0.7, 0.078125 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (3.0 / 4.0), vHeight * (3.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.0, 0.05, 0.0, 0.4, 0.5, 0.4, 0.04, 0.7, 0.04, 0.078125 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (3.0 / 4.0), vHeight * (2.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.05, 0.0, 0.0, 0.5, 0.4, 0.4, 0.7, 0.04, 0.04, 0.078125 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (3.0 / 4.0), vHeight * (1.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.05, 0.05, 0.05, 0.5, 0.5, 0.5, 0.7, 0.7, 0.7, 0.078125 * 128, 0.0, 0.0);

	gl.viewport(vWidth * (3.0 / 4.0), vHeight * (0.0 / 6.0), vWidth * (1.0 / 4.0), vHeight * (1.0 / 4.0));
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(vWidth) / parseFloat(vHeight), 0.1, 100.0);
	spheres(0.05, 0.05, 0.0, 0.5, 0.5, 0.4, 0.7, 0.7, 0.04, 0.078125 * 128, 0.0, 0.0);

	
	
	update();
	// Double buffering
	requestAnimationFrame(display,canvas);
}


function regToRad(degree) {
	return (degree / 3.14 * 180.0);
}

function update()
{

	angle = angle + 0.0005;
	var x;
	var y;
	var z;

	if (check == 'x') {
		x = 0.0;
		y = 5.0 * Math.sin(regToRad(angle));
		z = 5.0 * Math.cos(regToRad(angle));
		lightPossition = [x, y, z, 1.0];
	}
	if (check == 'y') {
		x = 5.0 * Math.cos(regToRad(angle));
		y = 0.0;
		z = 5.0 * Math.sin(regToRad(angle));
		lightPossition = [x, y, z, 1.0];
	}
	if (check == 'z') {
		x = 5.0 * Math.cos(regToRad(angle));
		y = 5.0 * Math.sin(regToRad(angle));
		z = 0.0;
		lightPossition = [x, y, z, 1.0];
	}

}

function uninitialize()
{
    //code

    if(shaderProgramObject_pf)
    {
        gl.useProgram(shaderProgramObject_pf);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject_pf);
        if(shaderObjects && shaderObjects.length > 0)
        {
            for (let i = 0; i < shaderObjects.length; i++) 
            {
                gl.detachShader(shaderProgramObject_pf , shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;                
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject_pf);
        shaderProgramObject_pf = null;
    }

    if (sphere)
		{
			sphere.deallocate();
			sphere = null;
		}

}