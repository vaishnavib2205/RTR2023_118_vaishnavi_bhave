// JavaScript source code
// Main function

function main()
{
    //get canvas - get context from above canvas
    var canvas = document.getElementById("VBB");//html file madhla canvas aapla web cha server aaplayala manipulate karayla denar

    if (canvas == null) {
        console.log("Getting Canvas failed \n");
    } else
    {
        console.log("Getting Canvas succeeded\n"); //
    }

    var context = canvas.getContext("2d");
    if (context == null) {
        console.log("getContext failed");
    }
    else
    {
        console.log("getContext success");
    }

    //set canvas bg color to black
    context.fillStyle = "black"; //0x000000 -> black in hex

    //set rectanle color
    context.fillRect(0, 0, canvas.width, canvas.height);

    //declare the text 
    var text = "Hello World!!!";

    //set the text in middle of the canvas
    context.textAllign = "center"; //horizontal
    context.textBaseline = "middle"; //vertical

    context.font = "30px sans-serif";
    //set text color to white
    context.fillStyle = "lime";

    //paint the text with above color
    context.fillText(text, canvas.width / 2, canvas.height / 2);

    //register for keyboard events
    window.addEventListener("keydown", keyDown, false); //window in built

    //register for mouse events
    window.addEventListener("click", mouseDown, false);

    function keyDown(event)
    {
        alert("key is pressed");
    }

    function mouseDown()
    {
        alert("mouse is clicked");
    }

}