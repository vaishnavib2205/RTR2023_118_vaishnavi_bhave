// JavaScript source code
// Main function
var canvas;
var context = null;
function main() {
    //get canvas - get context from above canvas
   canvas = document.getElementById("VBB");//html file madhla canvas aapla web cha server aaplayala manipulate karayla denar

    if (canvas == null) {
        console.log("Getting Canvas failed \n");
    } else {
        console.log("Getting Canvas succeeded\n"); //
    }

    context = canvas.getContext("2d");
    if (context == null) {
        console.log("getContext failed");
    }
    else {
        console.log("getContext success");
    }

    //set canvas bg color to black
    context.fillStyle = "black"; //0x000000 -> black in hex

    //set rectanle color
    context.fillRect(0, 0, canvas.width, canvas.height);

    //declare the text
    // draw text
    // drawText("Hello World !!!");

    //register for keyboard events
    window.addEventListener("keydown", keyDown, false); //window in built

    //register for mouse events
    window.addEventListener("click", mouseDown, false);
}
function keyDown(event) {
//code
    switch (event.keyCode) 
    {
        case 70:
        case 102:
            
            toggleFullscreen();
            drawText("Hello World !!!");

            break;

        default:
            break;
    }
}

function mouseDown() {
/*alert("mouse is clicked");*/
}

//
function drawText(text) {
    // code

    // declare the text

    // set the text in middle of the canvas
    context.textAlign = "center";
    context.textBaseline = "middle";

    // set font

    context.font = "48px sans-serif";

    // set text color to white

    context.fillStyle = "lime"; // 0xffffff

    // paint the text with above color

    context.fillText(text, canvas.width / 2, canvas.height / 2);

}

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
    }
}
