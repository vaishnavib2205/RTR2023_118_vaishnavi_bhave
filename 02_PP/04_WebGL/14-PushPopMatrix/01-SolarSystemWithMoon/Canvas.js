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


var mvpMatrixUniform ;
var perspectiveProjectionMatrix;

var sphere = null;


var stack = [32];

//stack top index
var top = 0 ;

function pushMatrix( element)
{
	
    if((top + 1) > 32)
    {
        uninitialized();
    }
    
	stack[top] = element;
	top++;
    
}

function popMatrix()
{

    if(top < 0)
    {
		// not valid stack 
		uninitialized();
    }
    
	top--;
	return stack[top];

}

var Day = 0;
var Year = 0;



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

        case 68:


			Day = (Day + 6) % 360;
			break;

		case 100: 

			Day = (Day - 6) % 360;
			break;

		case 89:

			Year = (Year + 1) % 360;
			break;

		case 121:

			Year = (Year - 1) % 360;

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
    "uniform mat4 uMVPMatrix;"+
    "void main(void)"+
    "{"+
    "gl_Position = uMVPMatrix * aPosition;"+
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
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "FragColor = vec4(1.0f , 1.0f , 1.0f ,1.0f);"+
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

    mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject , "uMVPMatrix");

    // triangle attributes

    sphere = new Mesh();
    makeSphere(sphere, 0.5, 30, 30);


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

    mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -10.0]); // source , target , values


    pushMatrix(modelViewMatrix);
        
    // sun
    mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);
    // gl.polygonMode(gl.FRONT_AND_BACK,gl.FILL);
    sphere.draw();

    mat4.rotate(modelViewMatrix , modelViewMatrix , parseFloat(Year) , [0.0, 1.0 , 0.0] );
    mat4.translate(modelViewMatrix , modelViewMatrix , [2.2, 0.0, 0.0]);
    mat4.rotate(modelViewMatrix , modelViewMatrix , parseFloat(Day) , [0.0, 1.0 , 0.0] );

    // earth
    mat4.rotate(modelViewMatrix , modelViewMatrix , 90.0 , [1.0, 0.0 , 0.0] );
    // modelMatrix = modelMatrix * vmath::rotate(90.0f , 1.0f , 0.0f , 0.0f);
    mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);

    sphere.draw();

    pushMatrix(modelViewMatrix);

    // moon
    mat4.translate(modelViewMatrix , modelViewMatrix , [1.0, 0.0, 0.0]);
    // modelMatrix = modelMatrix * vmath::translate(1.0f, 0.0f, 0.0f);
    mat4.rotate(modelViewMatrix , modelViewMatrix , parseFloat(Day) , [0.0, 1.0 , 0.0] );
    // modelMatrix = modelMatrix * vmath::rotate((GLfloat)Day, 0.0f, 1.0f, 0.0f);

    mat4.rotate(modelViewMatrix , modelViewMatrix , 90.0 , [1.0, 0.0 , 0.0] );
    mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);

    sphere.draw();

    modelViewMatrix = popMatrix();

    modelViewMatrix = popMatrix();




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