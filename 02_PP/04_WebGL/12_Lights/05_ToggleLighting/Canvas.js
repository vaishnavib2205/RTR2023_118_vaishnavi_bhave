// main function
var canvas;
var gl = null; // renaming context as gl for our use
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;


// WebGL related Varaiables

const vertexAttributeEnum =
{
    AMC_ATTRIBUTE_POSITION: 0,
    AMC_ATTRIBUTE_TEXTURE: 1,
    AMC_ATTRIBUTE_NORMAL: 2
};

var shaderProgramObject_pv = null;
var shaderProgramObject_pf = null;

var perspectiveProjectionMatrix;

var bLightingEnabled = false;

var bLightingEnabled_pf = false;
var bLightingEnabled_pv = false;

var sphere = null;

var lightDiffuseUniform = 0; // for diffuse light
var materialDiffuseUniform = 0; // for diffuse material
var lightAmbientUniform = 0; // for ambient light
var materialAmbientUniform = 0; // for ambient material
var lightSpecularUniform = 0; // for specular light
var materialSpecularUniform = 0; // for specular material
var materialShinenessUniform = 0; // for shinneness material
var lightPositionUniform = 0;

var modelMatrixUniform_pv = 0;
var projectionMatrixUniform_pv = 0;
var viewMatrixUniform_pv = 0;

var modelMatrixUniform = 0;
var projectionMatrixUniform = 0;
var viewMatrixUniform = 0;


var lightDiffuseUniform_pv = 0; // for diffuse light
var materialDiffuseUniform_pv = 0; // for diffuse material
var lightAmbientUniform_pv = 0; // for ambient light
var materialAmbientUniform_pv = 0; // for ambient material
var lightSpecularUniform_pv = 0; // for specular light
var materialSpecularUniform_pv = 0; // for specular material
var materialShinenessUniform_pv = 0; // for shinneness material
var lightPositionUniform_pv = 0;


var keyPressUniform = 0;
var keyPressUniform_pv = 0;

var lightAmbient = [0.1, 0.1, 0.1];
var lightDiffuse = [1.0, 1.0, 1.0];
var lightSpecular = [1.0, 1.0, 1.0];
var lightPosition = [100.0, 100.0, 100.0, 1.0];

var materialAmbient = [0.0, 0.0, 0.0];
var materialDiffuse = [0.5, 0.2, 0.7];
var materialSpecular = [0.7, 0.7, 0.7];
var materialShineness = 128.0;



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

            if (bLightingEnabled_pf == false) {
                bLightingEnabled_pf = true;
            }
            else {
                bLightingEnabled_pf = false;
            }
            break;

        case 76:
        case 108:
            if (bLightingEnabled == false) {
                bLightingEnabled = true;
            }
            else {
                bLightingEnabled = false;
            }
            break;

        case 86:
        case 118:

            if (bLightingEnabled_pv == false) {
                bLightingEnabled_pv = true;
            }
            else {
                bLightingEnabled_pv = false;
            }
            break;

        case 27:

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

    // per vertex
    {
        // vertex shader

        var vertexShaderSourceCode_pv =
            "#version 300 es" +
            "\n" +
            "in vec4 aPosition;" +
            "in vec3 aNormal;" +
            "uniform mat4 uModelMatrix;" +
            "uniform mat4 uViewMatrix;" +
            "uniform mat4 uProjectionMatrix;" +
            "uniform vec3 uLightAmbient;" +
            "uniform vec3 uLightDiffuse;" +
            "uniform vec3 uLightSpecular;" +
            "uniform vec4 uLightposition;" +
            "uniform vec3 uMaterialAmbient;" +
            "uniform vec3 uMaterialDiffuse;" +
            "uniform vec3 uMaterialSpecular;" +
            "uniform float uMaterialShineness;" +
            "uniform mediump int uKeyPress;" +
            "out vec3 oFong_ADS_Light;" +
            "out vec3 oDiffuseLight;" +
            "in vec4 aColor;" +
            "out vec4 oColor;" +
            "void main(void)" +
            "{" +
            "if(uKeyPress == 1)" +
            "{" +
            "vec4 eyeCoordinates =  uViewMatrix * uModelMatrix * aPosition;" +
            "vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);" +
            "vec3 lightDirection = normalize(vec3(uLightposition - eyeCoordinates));" +
            "vec3 reflectionVector = reflect(-lightDirection , transformedNormals);" +
            "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" +
            "vec3 ambientLight = uLightAmbient * uMaterialAmbient;" +
            "vec3 diffuseLight = uLightDiffuse * uMaterialDiffuse * max(dot(lightDirection , transformedNormals) , 0.0f); " +
            "vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector , viewerVector) , 0.0f) , uMaterialShineness); " +
            "oFong_ADS_Light = ambientLight + diffuseLight + specularLight; " +
            "}" +
            "else" +
            "{" +
            "oFong_ADS_Light = vec3(0.0f , 0.0f , 0.0f);" +
            "}" +
            "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" +
            "}";

        var vertexShaderObject_pv = gl.createShader(gl.VERTEX_SHADER);
        gl.shaderSource(vertexShaderObject_pv, vertexShaderSourceCode_pv);
        gl.compileShader(vertexShaderObject_pv);

        if (gl.getShaderParameter(vertexShaderObject_pv, gl.COMPILE_STATUS) == false) {
            var error = gl.getShaderInfoLog(vertexShaderObject_pv);

            if (error.length > 0) {
                var Log = "vertex shader compilation error : " + error;
                alert(Log);
                uninitialize();
            }
        }
        else {
            console.log("vertex Shader Compileed Successfully !!! \n");
        }

        var fragmentShaderSourceCode_pv =
            "#version 300 es" +
            "\n" +
            "precision highp float;" +
            "in vec4 oColor;" +
            "in vec3 oFong_ADS_Light;" +
            "out vec4 FragColor;" +
            "uniform mediump int uKeyPress;" +
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
        gl.shaderSource(fragmentShaderObject_pv, fragmentShaderSourceCode_pv);
        gl.compileShader(fragmentShaderObject_pv);

        if (gl.getShaderParameter(fragmentShaderObject_pv, gl.COMPILE_STATUS) == false) {
            var error = gl.getShaderInfoLog(fragmentShaderObject_pv);
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
        shaderProgramObject_pv = gl.createProgram();
        gl.attachShader(shaderProgramObject_pv, vertexShaderObject_pv);
        gl.attachShader(shaderProgramObject_pv, fragmentShaderObject_pv);

        gl.bindAttribLocation(shaderProgramObject_pv, vertexAttributeEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
        gl.bindAttribLocation(shaderProgramObject_pv, vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");

        gl.linkProgram(shaderProgramObject_pv);

        if (gl.getProgramParameter(shaderProgramObject_pv, gl.LINK_STATUS) == false) {
            var error = gl.getProgramInfoLog(shaderProgramObject_pv);
            if (error.length > 0) {
                var Log = "shader Program Linking Error : " + error;
                alert(Log);
                uninitialize();
            }
        }
        else {
            console.log("ShaderProgeam Linked Successfully !!! \n");
        }

        // mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject , "uMVPMatrix");

        modelMatrixUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uModelMatrix");
        viewMatrixUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uViewMatrix");
        projectionMatrixUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uProjectionMatrix");
        lightAmbientUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uLightAmbient");
        lightDiffuseUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uLightDiffuse");
        lightSpecularUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uLightSpecular");
        lightPositionUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uLightposition");
        materialAmbientUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uMaterialAmbient");
        materialDiffuseUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uMaterialDiffuse");
        materialSpecularUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uMaterialSpecular");
        materialShinenessUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uMaterialShineness");
        keyPressUniform_pv = gl.getUniformLocation(shaderProgramObject_pv, "uKeyPress");

    }

    // per fragment
    {
        // vertex shader

        var vertexShaderSourceCode =
            "#version 300 es" +
            "\n" +
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
        shaderProgramObject_pf = gl.createProgram();
        gl.attachShader(shaderProgramObject_pf, vertexShaderObject);
        gl.attachShader(shaderProgramObject_pf, fragmentShaderObject);

        gl.bindAttribLocation(shaderProgramObject_pf, vertexAttributeEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
        gl.bindAttribLocation(shaderProgramObject_pf, vertexAttributeEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");

        gl.linkProgram(shaderProgramObject_pf);

        if (gl.getProgramParameter(shaderProgramObject_pf, gl.LINK_STATUS) == false) {
            var error = gl.getProgramInfoLog(shaderProgramObject_pf);
            if (error.length > 0) {
                var Log = "shader Program Linking Error : " + error;
                alert(Log);
                uninitialize();
            }
        }
        else {
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
        materialShinenessUniform = gl.getUniformLocation(shaderProgramObject_pf, "uMaterialShineness");
        keyPressUniform = gl.getUniformLocation(shaderProgramObject_pf, "uKeyPress");
    }



    // triangle attributes

    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);


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

    if (bLightingEnabled_pv == true) {
        gl.useProgram(shaderProgramObject_pv);
    }
    else if (bLightingEnabled_pf == true) {
        gl.useProgram(shaderProgramObject_pf);
    }
    else {
        gl.useProgram(shaderProgramObject_pv);
    }


    // transformation

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]); // source , target , values

    // mat4.multiply(modelViewProjectionMatrix , perspectiveProjectionMatrix , modelViewMatrix);

    mat4.multiply(modelMatrix, modelMatrix, viewMatrix);



    if (bLightingEnabled == true) {
        if (bLightingEnabled_pv == true) {
            gl.uniformMatrix4fv(modelMatrixUniform_pv, false, modelMatrix);
            gl.uniformMatrix4fv(viewMatrixUniform_pv, false, viewMatrix);
            gl.uniformMatrix4fv(projectionMatrixUniform_pv, false, perspectiveProjectionMatrix);

            gl.uniform1i(keyPressUniform_pv, 1);
            gl.uniform3fv(lightAmbientUniform_pv, lightAmbient);
            gl.uniform3fv(lightDiffuseUniform_pv, lightDiffuse);
            gl.uniform3fv(lightSpecularUniform_pv, lightSpecular);
            gl.uniform3fv(materialAmbientUniform_pv, materialAmbient);
            gl.uniform3fv(materialDiffuseUniform_pv, materialDiffuse);
            gl.uniform3fv(materialSpecularUniform_pv, materialSpecular);
            gl.uniform4fv(lightPositionUniform_pv, lightPosition);
            gl.uniform1f(materialShinenessUniform_pv, materialShineness);

        }

        else if (bLightingEnabled_pf == true) {
            gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
            gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
            gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

            gl.uniform1i(keyPressUniform, 1);
            gl.uniform3fv(lightAmbientUniform, lightAmbient);
            gl.uniform3fv(lightDiffuseUniform, lightDiffuse);
            gl.uniform3fv(lightSpecularUniform, lightSpecular);
            gl.uniform3fv(materialAmbientUniform, materialAmbient);
            gl.uniform3fv(materialDiffuseUniform, materialDiffuse);
            gl.uniform3fv(materialSpecularUniform, materialSpecular);
            gl.uniform4fv(lightPositionUniform, lightPosition);
            gl.uniform1f(materialShinenessUniform, materialShineness);
        }

    }
    else {
        gl.uniformMatrix4fv(modelMatrixUniform_pv, false, modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_pv, false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_pv, false, perspectiveProjectionMatrix);

        gl.uniform1i(keyPressUniform_pv, 0);
    }

    sphere.draw();

    gl.useProgram(null);

    // call update before double buffering
    update();

    // double buffering
    requestAnimationFrame(display, canvas);
}

function update() {
    //code


}

function uninitialize() {
    //code

    if (shaderProgramObject_pv) {
        gl.useProgram(shaderProgramObject_pv);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject_pv);
        if (shaderObjects && shaderObjects.length > 0) {
            for (let i = 0; i < shaderObjects.length; i++) {
                gl.detachShader(shaderProgramObject_pv, shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject_pv);
        shaderProgramObject_pv = null;
    }

    if (sphere) {
        sphere.deallocate();
        sphere = null;
    }

}