// JavaScript source code
// Main function
var canvas;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

var requestAnimationFrame =
    window.requestAnimationFrame || //google chrome
    window.webkitRequestAnimationFrame || //webkit
    window.mozRequestAnimationFrame || //mozilla firefox
    window.oRequestAnimationFrame || // o for opera 
    window.msRequestAnimationFrame; // microsoft ms
function main() {
    //get canvas - get context from above canvas
   canvas = document.getElementById("VBB");//html file madhla canvas aapla web cha server aaplayala manipulate karayla denar

    if (canvas == null) {
        console.log("Getting Canvas failed \n");
    } else {
        console.log("Getting Canvas succeeded\n"); //
    }

    //set canvas width and height for future use
    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    window.addEventListener("resize", resize, false);

    initialise();

    resize();

    display();

    //register for keyboard events
    window.addEventListener("keydown", keyDown, false); //window in built

    //register for mouse events
    window.addEventListener("click", mouseDown, false);
}
function keyDown(event) {
//code
    switch (event.keyCode) 
    {
        case 81:
        case 113:
            uninitialise();
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
/*alert("mouse is clicked");*/
}

//


//for fullscreen
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
    bFullscreen = true;
    }
    else // if alredy full screen
    {
        if (document.exitFullscreen) {
            document.exitFullscreen();
        }
        else if (document.webkitExitFullscreen) {
            document.webkitExitFullscreen();
        }
        else if (document.mozCancelFullScreen) {
            document.mozCancelFullScreen();
        }
        else if (document.msExitFullScreen) {
            document.msExitFullScreen();
        }
    bFullscreen = false;
    }
}

function initialise() {
    //code
    gl = canvas.getContext("webgl2"); 
    if (gl == null) {
        console.log("getting webgl2 context failed");
    }
    else {
        console.log("getting webgl2 context success");
    }

    //set webgl2 context's view width and view width properties
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    //set clear color
    gl.clearColor(0.0, 0.0, 1.0, 1.0);  


}

function resize() {
    //code

    if (bFullscreen == true) {

        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;

    }
    else {

        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    //set viewport
    gl.viewport(0, 0, canvas.width, canvas.height);
}

function display() {
    //code

    gl.clear(gl.COLOR_BUFFER_BIT);

    //double buffering 
    requestAnimationFrame(display, canvas);
}

function uninitialise() {
    //code
}