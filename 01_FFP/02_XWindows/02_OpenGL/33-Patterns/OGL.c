// Standard header file

#include<stdio.h>  // for printf
#include<stdlib.h> // for exit()
#include<memory.h> // for memset

// X11 header files

#include<X11/Xlib.h> // for all XWindow API
#include<X11/Xutil.h> // for XVisualInfo and related APIs
#include<X11/XKBlib.h>

// OpenGL header files

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

// macros

#define WINWIDTH 800
#define WINHEIGHT 600

// global variable declaration

Bool bFullscreen = False;
Bool bActiveWindow = False;
Display *display = NULL; // connect Xserver with client App
Colormap colormap;
Window window; //
XVisualInfo *visualInfo;

// declare OpenGl global variables

GLXContext glxContext = NULL;

FILE *gpFile = NULL;



// main

int main(void)
{
    // local function declaration
    void uninitialize(void);
    void toggleFullScreen(void);
    int initialize(void);
    void resize(int , int);
    void draw(void);
    void update(void);


    // local variable declaraion
    int defaultScreen;
    XSetWindowAttributes windowAttribute; // wndclassEx in windows window
    int styleMask;
    Atom windowManagerDelete;
    XEvent event;
    KeySym keySym;
    int screenWidth , screenHeight;
    char keys[26];
    int frameBufferAttributes[] =
    {
        GLX_DOUBLEBUFFER , True ,
        GLX_RGBA ,
        GLX_RED_SIZE  ,     8 ,
        GLX_GREEN_SIZE,     8 ,
        GLX_BLUE_SIZE ,     8 ,
        GLX_ALPHA_SIZE,     8 ,
        None
    };
    Bool bDone = False;


    // code

    gpFile = fopen("Log.txt" , "w");
    if (gpFile == NULL)
    {
        printf("fopen() , failed to create new file /n");
        exit(1);
    }




    // open the connection with XSerer

    display = XOpenDisplay(NULL);
    if(display == NULL)
    {
        printf("XOpenDisplay failed \n");
        uninitialize();
        exit(1);
    }
    // get default screen from above display

    defaultScreen = XDefaultScreen(display);

    // get visual info from above three

    visualInfo = glXChooseVisual(display , defaultScreen ,  frameBufferAttributes);
    if(visualInfo == NULL)
    {
        printf("glXChooseVisual failed \n");
        uninitialize();
        exit(1);
    }

    // set window attribute/properties
    // total 15 members

    memset((void*)&windowAttribute , 0 , sizeof(XSetWindowAttributes));
    windowAttribute.border_pixel = 0; // hya window la border asnar ahe 0 , mala default dyaycha ahe color
    windowAttribute.background_pixel = XBlackPixel(display , visualInfo->screen);
    windowAttribute.background_pixmap = 0; // pixmap mhanje image dena
    windowAttribute.colormap = XCreateColormap(
        display ,
        XRootWindow(display , visualInfo->screen),
        visualInfo->visual,
        AllocNone
         );

    // assign this colormap to global colormap
    colormap = windowAttribute.colormap;

    // set the style mask of window
    styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;     // ws_overlapped in windows is style mask here
    // CW  = create window

    // now finally create window

    // createWindow in Windows
    window = XCreateWindow(display,
        XRootWindow(display , visualInfo->screen),
        0,
        0,
        WINWIDTH,
        WINHEIGHT,
        0,
        visualInfo->depth,
        InputOutput,
        visualInfo->visual,
        styleMask,
        &windowAttribute
    );

    if(!window)
    {
        printf("XCreateWindow failed \n");
        uninitialize();
        exit(1);
    }

    // specify to which event this window should respond

    XSelectInput(
        display ,
        window ,
        ExposureMask | VisibilityChangeMask | StructureNotifyMask | KeyPressMask | ButtonPressMask | PointerMotionMask | FocusChangeMask
    );

    // specify window manager delete atem

    windowManagerDelete = XInternAtom(display , "WM_DELETE_WINDOW" , True);

    // set above atom as protocol for window manager

    XSetWMProtocols(display , window , &windowManagerDelete , 1);

    // give caption/ / name to the window

    XStoreName(display , window , "Vaishnavi Bhave");

    // show / map the window

    XMapWindow(display , window);


    // center the window

    screenWidth = XWidthOfScreen(
        XScreenOfDisplay(display , visualInfo->screen ));

    screenHeight = XHeightOfScreen(
        XScreenOfDisplay(display , visualInfo->screen));

    XMoveWindow(
        display ,
         window ,
         (screenWidth - WINWIDTH) / 2,
         (screenHeight - WINHEIGHT) / 2
          );

    // OpenGL initialization

    initialize();

    // game loop

    while (bDone == False)
    {
        while(XPending(display))
        {
            // do memset in XWindow.c code os first window
            XNextEvent(display , &event);
            switch (event.type)
            {
            case MapNotify: // show window in Windows window

                break;

            case FocusIn :
                bActiveWindow = True;

                break;

            case FocusOut:
                bActiveWindow = False;

                break;

            case ConfigureNotify:
                resize(event.xconfigure.width , event.xconfigure.height);
                break;

            case ButtonPress:
                switch (event.xbutton.button)
                {
                case 1:

                    break;

                case 2:

                    break;

                case 3:

                    break;

                default:
                    break;
                }
                break;

            case KeyPress :
                keySym = XkbKeycodeToKeysym(display , event.xkey.keycode , 0 , 0);
                switch(keySym)
                {
                    case XK_Escape :
                        bDone = True;
                        break;

                    default :
                        break;
                }
                XLookupString(&event.xkey , keys , sizeof(keys) , NULL , NULL);
                switch(keys[0])
                {
                    case 'F':
                    case 'f':

                        if(bFullscreen == False)
                        {
                            toggleFullScreen();
                            bFullscreen = True;
                        }
                        else
                        {
                            toggleFullScreen();
                            bFullscreen = False;
                        }
                        break;

                    default :
                        break;
                }

                break;


            case 33:
                bDone = True;
                break;

            default:
                break;
            }
        }
        // rendering

        if(bActiveWindow == True)
        {
            draw();

            update();
        }
    }

    uninitialize();

    return 0;

}


void toggleFullScreen(void)
{
    // local variable declaration
    Atom windowManagerStateNormal;
    Atom windowManagerStateFullscreen;
    XEvent event;

    // code

    windowManagerStateNormal = XInternAtom(
        display ,
        "_NET_WM_STATE" ,
        False
        );

    windowManagerStateFullscreen = XInternAtom(
        display,
        "_NET_WM_STATE_FULLSCREEN",
        False
    );

    // memset the event structure and fill it with our above 2 atoms

    memset((void*)&event , 0 , sizeof(XEvent));

    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = windowManagerStateNormal;
    event.xclient.format = 32;
    event.xclient.data.l[0] = bFullscreen ? 0 : 1;
    event.xclient.data.l[1] = windowManagerStateFullscreen;

    // send the event

    XSendEvent(
        display ,
        XRootWindow(display , visualInfo->screen) ,
        False ,// can child window become fullscreen , don't propogate to child windows
        SubstructureNotifyMask, // sub message
        &event // urlele tu bhar mala 5 ch kamache ahet
        );

}

int initialize(void)
{
    // local function declaration

    void uninitialize(void);
    void resize(int , int);

    // code
    glxContext = glXCreateContext(display , visualInfo , NULL , True);
    if(glxContext == False)
    {
        printf("glXCreateContext failed \n");
        return 1;
    }
    if(glXMakeCurrent(display , window , glxContext) == False)
    {
        printf("glXCreateContext failed \n");
        return 2;
    }
    // create OpenGl context

    glClearColor(0.0f , 0.0f , 0.0f , 0.0f);

    // warmup resize

    resize(WINWIDTH , WINHEIGHT);

    // return 0;

    // make this context as current context

}

void resize(int width, int height)
{
	// code

	if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

}



void draw(void)
{
    // Code
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-1.5f, 0.0f, -5.0f);

    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (float i = -1.0f; i <= 0.0f; i += 0.25)
    {
        for (float j = 1.0f; j >= 0.0f; j -= 0.25)
        {
            glVertex3f(i, j, 0.0f);
        }
    }
    glEnd();

    // patter 2
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -5.0f);

    glPointSize(4.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    for (float i = -1.0f; i <= -0.25f; i += 0.25)
    {
        glVertex3f(i, 1.0f, 0.0f);
        glVertex3f(i, 0.0f, 0.0f);
    }

    for (float i = 1.0f; i >= 0.25f; i -= 0.25)
    {
        glVertex3f(0.0f, i, 0.0f);
        glVertex3f(-1.0f, i, 0.0f);
    }

    float x = -0.25f, y = 0.25f;
    int mark = 1;
    int stop = 0;
    while (x >= -1.0f && y <= 1.0f)
    {
        if (mark == 1)
        {
            glVertex3f(x, 0.0f, 0.0f);
            glVertex3f(0.0f, y, 0.0f);
        }
        else {
            glVertex3f(-1.0f, y, 0.0f);
            glVertex3f(x, 1.0f, 0.0f);
        }
        x -= 0.25, y += 0.25;

        if ((x <= -1.01f && y >= 1.01f) && stop == 0)
        {
            stop = 1;
            mark = 0;
            x = -0.25f;
            y = 0.25f;
        }
    }
    glEnd();

    // pattern 3
    glLoadIdentity();

    glTranslatef(1.5f, 0.0f, -5.0f);

    glPointSize(4.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);

    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    for (float i = -1.0f; i <= -0.25f; i += 0.25)
    {
        glVertex3f(i, 1.0f, 0.0f);
        glVertex3f(i, 0.0f, 0.0f);
    }

    for (float i = 1.0f; i >= 0.25f; i -= 0.25)
    {
        glVertex3f(0.0f, i, 0.0f);
        glVertex3f(-1.0f, i, 0.0f);
    }

    glEnd();

    // patter 4
    glLoadIdentity();

    glTranslatef(-1.5f, -1.5f, -5.0f);
    glPointSize(4.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);

    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    for (float i = -1.0f; i <= -0.25f; i += 0.25)
    {
        glVertex3f(i, 1.0f, 0.0f);
        glVertex3f(i, 0.0f, 0.0f);
    }

    for (float i = 1.0f; i >= 0.25f; i -= 0.25)
    {
        glVertex3f(0.0f, i, 0.0f);
        glVertex3f(-1.0f, i, 0.0f);
    }

    x = -0.25f, y = 0.25f;
    mark = 1;
    stop = 0;
    while (x >= -1.0f && y <= 1.0f)
    {
        if (mark == 1)
        {
            glVertex3f(x, 0.0f, 0.0f);
            glVertex3f(0.0f, y, 0.0f);
        }
        else {
            glVertex3f(-1.0f, y, 0.0f);
            glVertex3f(x, 1.0f, 0.0f);
        }
        x -= 0.25, y += 0.25;

        if ((x <= -1.01f && y >= 1.01f) && stop == 0)
        {
            stop = 1;
            mark = 0;
            x = -0.25f;
            y = 0.25f;
        }
    }
    glEnd();

    // patter 5
    glLoadIdentity();

    glTranslatef(0.0f, -1.5f, -5.0f);
    glPointSize(4.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);

    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-0.75f, 0.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-0.25f, 0.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.25f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.75f, 0.0f);

    glEnd();

    // patter 6
    glLoadIdentity();

    glTranslatef(1.5f, -1.5f, -5.0f);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.65f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.65f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.3f, 1.0f, 0.0f);
    glVertex3f(-0.65f, 1.0f, 0.0f);
    glVertex3f(-0.65f, 0.0f, 0.0f);
    glVertex3f(-0.3f, 0.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.3f, 1.0f, 0.0f);
    glVertex3f(0.05f, 1.0f, 0.0f);
    glVertex3f(0.05f, 0.0f, 0.0f);
    glVertex3f(-0.3f, 0.0f, 0.0f);

    glEnd();

    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f);

    glVertex3f(0.05f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);

    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.05f, 0.0f, 0.0f);

    glVertex3f(0.05f, 0.0f, 0.0f);
    glVertex3f(0.05f, 1.0f, 0.0f);

    for (float i = -1.0f; i <= -0.25f; i += 0.35)
    {
        glVertex3f(i, 1.0f, 0.0f);
        glVertex3f(i, 0.0f, 0.0f);
    }

    for (float i = 1.0f; i >= 0.25f; i -= 0.35)
    {
        glVertex3f(0.05f, i, 0.0f);
        glVertex3f(-1.0f, i, 0.0f);
    }
    glEnd();

    glXSwapBuffers(display , window);
}

void update(void)
{
    // code

}

void uninitialize(void)
{
    // code

    GLXContext currentGLXContext = NULL;

    if(visualInfo)
    {
        free(visualInfo);
        visualInfo = NULL;
    }

    //uncurrent the context

    currentGLXContext = glXGetCurrentContext();
    if(currentGLXContext != NULL && currentGLXContext == glxContext)
    {
        glXMakeCurrent(display , 0 , 0);
    }

    if(glxContext)
    {
        glXDestroyContext(display , glxContext);
        glxContext = NULL;
    }

    if(window)
    {
        XDestroyWindow(display , window);
    }

    if(colormap)
    {
        XFreeColormap(display , colormap);
    }

    if(display)
    {
        XCloseDisplay(display);
    }

    if(gpFile)
    {
        fclose(gpFile);
        gpFile = NULL;
    }

}


