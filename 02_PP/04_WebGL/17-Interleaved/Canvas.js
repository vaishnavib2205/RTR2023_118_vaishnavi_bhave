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
    AMC_ATTRIBUTE_COLOR:1,
    AMC_ATTRIBUTE_NORMAL:2,
    AMC_ATTRIBUTE_TEXTURE:3
};

var shaderProgramObject = null;

var vao = null;
var vbo = null;
var angle_Rectangle = 0.0;

var bLightingEnabled = false;

var lightDiffuseUniform = 0; // for diffuse light
var materialDiffuseUniform = 0; // for diffuse material
var lightAmbientUniform = 0; // for ambient light
var materialAmbientUniform = 0; // for ambient material
var lightSpecularUniform = 0; // for specular light
var materialSpecularUniform = 0; // for specular material
var materialShinenessUniform = 0; // for shinneness material
var lightPositionUniform = 0;
var keyPressUniform = 0;

var lightAmbient = [ 0.1 , 0.1 , 0.1 ];
var lightDiffuse = [ 1.0 , 1.0 , 1.0 ];
var lightSpecular = [ 1.0 , 1.0 , 1.0 ];
var lightPosition = [ 100.0 , 100.0 , 100.0 , 1.0];

var materialAmbient = [ 0.0 , 0.0 , 0.0 ];
var materialDiffuse = [ 1.0 , 1.0 , 1.0 ];
var materialSpecular = [ 1.0 , 1.0 , 1.0 ];
var materialShineness = 50.0;

var textureSamplerUniform;
var stone_texture ;

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
    "uniform vec4 uLightposition;" +
    "out vec3 otransformedNormals;" +
    "out vec3 olightDirection;" +
    "out vec3 oviewerVector;" +
    "in vec4 aColor;" +
    "out vec4 oColor;" +
    "in vec2 aTexCoord;"+
    "out vec2 oTexCoord;"+
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
    "oColor = aColor;" +
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
    "uniform mediump int uKeyPress;" +
    "in vec2 oTexCoord;"+
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
    "vec3 tex = vec3(texture(uTextureSampler , oTexCoord));" +
    "FragColor = vec4(tex * vec3(oColor) * Phong_ADS_Light , 1.0);" +
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
    gl.bindAttribLocation(shaderProgramObject , vertexAttributeEnum.AMC_ATTRIBUTE_COLOR , "aColor");  
    gl.bindAttribLocation(shaderProgramObject , vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL , "aNormal");  
    gl.bindAttribLocation(shaderProgramObject , vertexAttributeEnum.AMC_ATTRIBUTE_TEXTURE , "aTexCoord");  
    
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

    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");
    lightAmbientUniform = gl.getUniformLocation(shaderProgramObject, "uLightAmbient");
    lightDiffuseUniform = gl.getUniformLocation(shaderProgramObject, "uLightDiffuse");
    lightSpecularUniform = gl.getUniformLocation(shaderProgramObject, "uLightSpecular");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "uLightposition");
    materialAmbientUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialAmbient");
    materialDiffuseUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialDiffuse");
    materialSpecularUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialSpecular");
    materialShinenessUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialShineness");
    keyPressUniform = gl.getUniformLocation(shaderProgramObject, "uKeyPress");
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject , "uTextureSampler");

    // triangle attributes

    var cube_PCNT = new Float32Array([
    
		// front
		// position				// color			 // normals				// texcoords
		1.0,  1.0,  1.0,	1.0, 1.0, 0.0,	 0.0,  0.0,  1.0,	1.0, 1.0,
		-1.0,  1.0,  1.0,	1.0, 1.0, 0.0,	 0.0,  0.0,  1.0,	0.0, 1.0,
		-1.0, -1.0,  1.0,	1.0, 1.0, 0.0,	 0.0,  0.0,  1.0,	0.0, 0.0,
		1.0, -1.0,  1.0,	1.0, 1.0, 0.0,	 0.0,  0.0,  1.0,	1.0, 0.0,
							
		// right			 
		// position				// color			 // normals				// texcoords
		1.0,  1.0, -1.0,	0.0, 0.0, 1.0,	 1.0,  0.0,  0.0,	1.0, 1.0,
		1.0,  1.0,  1.0,	0.0, 0.0, 1.0,	 1.0,  0.0,  0.0,	0.0, 1.0,
		1.0, -1.0,  1.0,	0.0, 0.0, 1.0,	 1.0,  0.0,  0.0,	0.0, 0.0,
		1.0, -1.0, -1.0,	0.0, 0.0, 1.0,	 1.0,  0.0,  0.0,	1.0, 0.0,
							
		// back				 
		// position				// color			 // normals				// texcoords
		1.0,  1.0, -1.0,	1.0, 1.0, 0.0,	 0.0,  0.0, -1.0,	1.0, 1.0,
		-1.0,  1.0, -1.0,	1.0, 1.0, 0.0,	 0.0,  0.0, -1.0,	0.0, 1.0,
		-1.0, -1.0, -1.0,	1.0, 1.0, 0.0,	 0.0,  0.0, -1.0,	0.0, 0.0,
		1.0, -1.0, -1.0,	1.0, 1.0, 0.0,	 0.0,  0.0, -1.0,	1.0, 0.0,
							
		// left				 
		// position				// color			 // normals				// texcoords
		-1.0,  1.0,  1.0,	1.0, 0.0, 1.0,	-1.0,  0.0,  0.0,	1.0, 1.0,
		-1.0,  1.0, -1.0,	1.0, 0.0, 1.0,	-1.0,  0.0,  0.0,	0.0, 1.0,
		-1.0, -1.0, -1.0,	1.0, 0.0, 1.0,	-1.0,  0.0,  0.0,	0.0, 0.0,
		-1.0, -1.0,  1.0,	1.0, 0.0, 1.0,	-1.0,  0.0,  0.0,	1.0, 0.0,
							
		// top				 
		// position				// color			 // normals				// texcoords
		1.0,  1.0, -1.0,	0.0, 1.0, 0.0,	 0.0,  1.0,  0.0,	1.0, 1.0,
		-1.0,  1.0, -1.0,	0.0, 1.0, 0.0,	 0.0,  1.0,  0.0,	0.0, 1.0,
		-1.0,  1.0,  1.0,	0.0, 1.0, 0.0,	 0.0,  1.0,  0.0,	0.0, 0.0,
		1.0,  1.0,  1.0,	0.0, 1.0, 0.0,	 0.0,  1.0,  0.0,	1.0, 0.0,
							
		// bottom			 
		// position				// color			 // normals				// texcoords
		1.0, -1.0,  1.0,	1.0, 0.5, 0.0,	 0.0, -1.0,  0.0,	1.0, 1.0,
		-1.0, -1.0,  1.0,	1.0, 0.5, 0.0,	 0.0, -1.0,  0.0,	0.0, 1.0,
		-1.0, -1.0, -1.0,	1.0, 0.5, 0.0,	 0.0, -1.0,  0.0,	0.0, 0.0,
		1.0, -1.0, -1.0,	1.0, 0.5, 0.0,	 0.0, -1.0,  0.0,	1.0, 0.0

    ]);

    // vao

    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    // vbo

    vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo);

    gl.bufferData(gl.ARRAY_BUFFER , cube_PCNT , gl.STATIC_DRAW);

    //postion
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 11 * 4 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);

    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR , 3 , gl.FLOAT , false , 11 * 4 , 3 * 4 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR);

    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL , 3 , gl.FLOAT , false , 11 * 4 , 6 * 4 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL);

    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_TEXTURE , 2 , gl.FLOAT , false , 11 * 4 , 9 * 4 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_TEXTURE);

    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    gl.bindVertexArray(null);

    loadGLTexture();

    // depth initialise
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // set clear color
    gl.clearColor(0.0 , 0.0 , 0.0 , 1.0);

    // initialise projection matrix

    perspectiveProjectionMatrix = mat4.create();

}

function loadGLTexture()
{
    stone_texture = gl.createTexture();

    stone_texture.image = new Image();

    console.log("before src load !!! \n");

    stone_texture.image.src = "marble.png";

    console.log("after src load !!! \n");

    stone_texture.image.onLoad = Function()
    {
        gl.bindTexture(gl.TEXTURE_2D , stone_texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL , true);
        gl.texParameteri(gl.TEXTURE_2D , gl.TEXTURE_MAG_FILTER , gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D , gl.TEXTURE_MIN_FILTER , gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D , 0 , gl.RGBA , gl.RGBA , gl.UNSIGNED_BYTE , stone_texture.image);
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

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelMatrix , modelMatrix , [0.0, 0.0 , -4.0]); // source , target , values
    mat4.rotate(modelMatrix , modelMatrix , angle_Rectangle , [1.0, 0.0 , 0.0]); // source , target , values
    mat4.rotate(modelMatrix , modelMatrix , angle_Rectangle , [0.0, 1.0 , 0.0]); // source , target , values
    mat4.rotate(modelMatrix , modelMatrix , angle_Rectangle , [0.0, 0.0 , 1.0]); // source , target , values

    mat4.multiply(modelMatrix , modelMatrix , viewMatrix);

    gl.uniformMatrix4fv(modelMatrixUniform , false , modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform , false , viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform , false , perspectiveProjectionMatrix);

    if (bLightingEnabled == true)
    {
        gl.uniform1i(keyPressUniform , 1);
        gl.uniform3fv(lightAmbientUniform,  lightAmbient );
        gl.uniform3fv(lightDiffuseUniform,  lightDiffuse );
        gl.uniform3fv(lightSpecularUniform,  lightSpecular );
        gl.uniform3fv(materialAmbientUniform,  materialAmbient );
        gl.uniform3fv(materialDiffuseUniform,  materialDiffuse );
        gl.uniform3fv(materialSpecularUniform,  materialSpecular );
        gl.uniform4fv(lightPositionUniform,  lightPosition );
        gl.uniform1f(materialShinenessUniform, materialShineness);

    }
    else
    {
        gl.uniform1i(keyPressUniform , 0);
    }
    
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D , stone_texture);
    gl.uniform1i(textureSamplerUniform , 0);

    gl.bindVertexArray(vao);

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4,4);
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
    angle_Rectangle -= 0.02;
	if (angle_Rectangle <= 360.0)
	{
		angle_Rectangle += 360.0;
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