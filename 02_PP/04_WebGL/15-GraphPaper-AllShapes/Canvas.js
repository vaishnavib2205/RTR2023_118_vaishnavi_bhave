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
    AMC_ATTRIBUTE_COLOR:1
};

var shaderProgramObject = null;

var vao_triangle = 0;
var vbo_position_triangle = 0;

var vao_square = 0;
var vbo_position_square = 0;

var vao_circle = 0;
var vbo_position_circle = 0;
var vbo_color = 0;

var vao_blueLines = 0;
var vbo_position_blueLines = 0;

var vao_HorizontalBlueLines = 0;
var vbo_position_HorizontalBlueLines= 0;
var vbo_blueColor = 0;

var vao_greenLines = 0;
var vbo_position_greenLine = 0;
var vbo_color_greenLine = 0;

var vao_redLine = 0;
var vbo_position_redLine = 0;
var vbo_redColor = 0;

var Graph = false;
var Triangle = false;
var Square = false;
var CirclePressed = false;

var x = 0.0;
var y = 0.0;
var Radian;

var i = 0;
var circle_position = new Float32Array(1081);


var mvpMatrixUniform ;
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

        case 71:
        case 103:
            if(Graph == false)
            {
                Graph = true;
            }
            else
            {
                Graph = false;
            }
            
            break;

        case 84:
        case 116:
            if(Triangle == false)
            {
                Triangle = true;
            }
            else
            {
                Triangle = false;
            }

            break;

        case 83:
        case 115:
            if(Square == false)
            {
                Square = true;
            }
            else
            {
                Square = false;
            }

            break;

        case 67:
        case 99:
            if(CirclePressed == false)
            {
                CirclePressed = true;
            }
            else
            {
                CirclePressed = false;
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
    "in vec4 aPosition;" +
    "uniform mat4 uMVPMatrix;" +
    "in vec4 aColor;" +
    "out vec4 oColor;" +
    "void main(void)" +
    "{" +
    "gl_Position = uMVPMatrix * aPosition;" +
    "oColor = aColor;" +
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
    "out vec4 FragColor;" +
    "void main(void)" +
    "{" +
    "FragColor = oColor;" +
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

    // triangle attributes

    var trianglePosition = new Float32Array([
    
        0.0 , 1.0 , 0.0,
        -1.0 , -1.0 , 0.0,
        1.0 , -1.0 , 0.0
    ]);

    var triangleColor = new Float32Array([
    
        1.0 , 0.0 , 0.0 , //		glColor3f(1.0f, 0.0f, 0.0f);
        0.0 , 1.0 , 0.0 , //		glColor3f(0.0f, 1.0f, 0.0f);
        0.0 , 0.0 , 1.0   //		glColor3f(0.0f, 0.0f, 1.0f);
    ]);

    var triangle_Position = new Float32Array([
		0.0,1.0,0.0,
		-1.0,-1.0,0.0,
		
		-1.0,-1.0,0.0,
		1.0,-1.0,0.0,
		
		1.0,-1.0,0.0,
		0.0,1.0,0.0
    ]);

    var square_Position = new Float32Array([
		1.0,1.0,0.0,
		-1.0,1.0,0.0,
		
		-1.0,1.0,0.0,
		-1.0,-1.0,0.0,

		-1.0,-1.0,0.0,
		1.0,-1.0,0.0,
		
		1.0,-1.0,0.0,
		1.0,1.0,0.0
    ]);
	

	for (var fAngle = 0.0; fAngle < 361.0; fAngle++)
	{
		Radian = fAngle * Math.PI / 180.0;

		x = 1.0 * Math.cos(Radian);
		y = 1.0 * Math.sin(Radian);

		if (x != 0.0)
			circle_position[i] = x;

		circle_position[i + 1] = y;
		circle_position[i + 2] = 0.0;
		console.log("%f , %f , %f, \n", circle_position[i], circle_position[i + 1], circle_position[i + 2]);
		i += 3;
	}

    var blueVertical_Position = new Float32Array([
		0.0,10.0,0.0,
		0.0,-10.0,0.0
    ]);

    var blueHorizontal_Position = new Float32Array([
		10.0,0.0,0.0,
		-10.0,0.0,0.0
    ]);

    var blue_Color = new Float32Array([
		0.0,0.0,1.0,
		0.0,0.0,1.0
	]);

    var red_position = new Float32Array([
		10.0,0.0,0.0,
		-10.0,0.0,0.0
	]);

    var red_Color = new Float32Array([
		1.0,0.0,0.0,
		1.0,0.0,0.0
	]);

    var green_Position = new Float32Array([
		0.0,10.0,0.0,
		0.0,-10.0,0.0
	]);

    var green_Color = new Float32Array([
		0.0,1.0,0.0,
		0.0,1.0,0.0
	]);
    
    // vao

    vao_triangle = gl.createVertexArray();
    gl.bindVertexArray(vao_triangle);

    // vbo

    vbo_position_triangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_position_triangle);

    gl.bufferData(gl.ARRAY_BUFFER , triangle_Position , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo color
    gl.vertexAttrib3f(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR, 1.0, 1.0, 0.0);

    gl.bindVertexArray(null);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// SQUARE VAO for position
    vao_square = gl.createVertexArray();
    gl.bindVertexArray(vao_square);

    // vbo

    vbo_position_square = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_position_square);

    gl.bufferData(gl.ARRAY_BUFFER , square_Position , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo color
    gl.vertexAttrib3f(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR, 1.0, 1.0, 0.0);

    gl.bindVertexArray(null);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CIRCLE VAO for position
    vao_circle = gl.createVertexArray();
    gl.bindVertexArray(vao_circle);

    vbo_position_circle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_position_circle);

    gl.bufferData(gl.ARRAY_BUFFER , circle_position , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo color
    gl.vertexAttrib3f(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR, 1.0, 1.0, 0.0);

    gl.bindVertexArray(null);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //Blue
	// verticle VAO for position
    vao_blueLines = gl.createVertexArray();
    gl.bindVertexArray(vao_blueLines);

    vbo_position_blueLines = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_position_blueLines);

    gl.bufferData(gl.ARRAY_BUFFER , blueVertical_Position , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo color
    vbo_blueColor = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_blueColor);

    gl.bufferData(gl.ARRAY_BUFFER , blue_Color , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    gl.bindVertexArray(null);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// horizontal VAO for position
    vao_HorizontalBlueLines = gl.createVertexArray();
    gl.bindVertexArray(vao_HorizontalBlueLines);

    vbo_position_HorizontalBlueLines = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_position_HorizontalBlueLines);

    gl.bufferData(gl.ARRAY_BUFFER , blueHorizontal_Position , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo color
    vbo_blueColor = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_blueColor);

    gl.bufferData(gl.ARRAY_BUFFER , blue_Color , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    gl.bindVertexArray(null);

////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Red
	// verticle VAO for position

    vao_redLine = gl.createVertexArray();
    gl.bindVertexArray(vao_redLine);

    vbo_position_redLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_position_redLine);

    gl.bufferData(gl.ARRAY_BUFFER , red_position , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo color
    vbo_redColor = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_redColor);

    gl.bufferData(gl.ARRAY_BUFFER , red_Color , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    gl.bindVertexArray(null);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Green
	// verticle VAO for position

    vao_greenLines = gl.createVertexArray();
    gl.bindVertexArray(vao_greenLines);

    vbo_position_greenLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_position_greenLine);

    gl.bufferData(gl.ARRAY_BUFFER , green_Position , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    // vbo color
    vbo_color_greenLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER , vbo_color_greenLine);

    gl.bufferData(gl.ARRAY_BUFFER , green_Color , gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR , 3 , gl.FLOAT , false , 0 , 0 );
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER , null);

    gl.bindVertexArray(null);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    // depth initialise
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // set clear color
    gl.clearColor(0.0 , 0.0 , 0.2 , 1.0);

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

    // mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -3.0]); // source , target , values

    // mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
    // gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);

	if(Graph == true)
        {
            for (var i = 0.1; i < 5.0; i += 0.1)
            {
                modelViewMatrix = mat4.create();
                modelViewProjectionMatrix = mat4.create();
            
                mat4.translate(modelViewMatrix , modelViewMatrix , [i, 0.0 , -6.0]); // source , target , values
            
                mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
                gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);
    
                gl.bindVertexArray(vao_blueLines);
                gl.drawArrays(gl.LINES, 0, 2);
                gl.bindVertexArray(null);

            }
            for (var i = -0.1; i > -5.0; i -= 0.1)
            {
                modelViewMatrix = mat4.create();
                modelViewProjectionMatrix = mat4.create();
            
                mat4.translate(modelViewMatrix , modelViewMatrix , [i, 0.0 , -6.0]); // source , target , values
            
                mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
                gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);
    
                gl.bindVertexArray(vao_blueLines);
                gl.drawArrays(gl.LINES, 0, 2);
                gl.bindVertexArray(null);
            }
            for (var i = -0.1; i > -5.0; i -= 0.1)
            {
                modelViewMatrix = mat4.create();
                modelViewProjectionMatrix = mat4.create();
            
                mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, i , -6.0]); // source , target , values
            
                mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
                gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);
    
                gl.bindVertexArray(vao_HorizontalBlueLines);
                gl.drawArrays(gl.LINES, 0, 2);
                gl.bindVertexArray(null);

            }
            for (var i = 0.1; i < 5.0; i += 0.1)
            {
                modelViewMatrix = mat4.create();
                modelViewProjectionMatrix = mat4.create();
            
                mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, i , -6.0]); // source , target , values
            
                mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
                gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);
    
                gl.bindVertexArray(vao_HorizontalBlueLines);
                gl.drawArrays(gl.LINES, 0, 2);
                gl.bindVertexArray(null);
            }
    
            modelViewMatrix = mat4.create();
            modelViewProjectionMatrix = mat4.create();
        
            mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -6.0]); // source , target , values
        
            mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
            gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);
    
            gl.bindVertexArray(vao_redLine);
            gl.drawArrays(gl.LINES, 0, 2);
            gl.bindVertexArray(null);
    
    
            modelViewMatrix = mat4.create();
            modelViewProjectionMatrix = mat4.create();
        
            mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -6.0]); // source , target , values
        
            mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
            gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);
    
            gl.bindVertexArray(vao_greenLines);
            gl.drawArrays(gl.LINES, 0, 2);
            gl.bindVertexArray(null);
    
        }
        if(Triangle == true)
        {
            modelViewMatrix = mat4.create();
            modelViewProjectionMatrix = mat4.create();
        
            mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -6.0]); // source , target , values
        
            mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
            gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);

            gl.bindVertexArray(vao_triangle);
            gl.drawArrays(gl.LINES, 0, 6);
            gl.bindVertexArray(null);
        }
        if(Square == true)
        {
            modelViewMatrix = mat4.create();
            modelViewProjectionMatrix = mat4.create();
        
            mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -6.0]); // source , target , values
        
            mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
            gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);

            gl.bindVertexArray(vao_square);
            gl.drawArrays( gl.LINES, 0, 8);
            gl.bindVertexArray(null);
        }
        if(CirclePressed == true)
        {
            modelViewMatrix = mat4.create();
            modelViewProjectionMatrix = mat4.create();
        
            mat4.translate(modelViewMatrix , modelViewMatrix , [0.0, 0.0 , -6.0]); // source , target , values
        
            mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);
            gl.uniformMatrix4fv(mvpMatrixUniform , false , modelViewProjectionMatrix);

            gl.bindVertexArray(vao_circle);
    
            for (var i = 0; i < 119; i++) {
    
                gl.drawArrays(gl.LINES, i, 242);
    
            }
    
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