// Standard header file

#include<stdio.h>  // for printf
#include<stdlib.h> // for exit()
#include<memory.h> // for memset

// X11 header files

#include<X11/Xlib.h> // for all XWindow API
#include<X11/Xutil.h> // for XVisualInfo and related APIs
#include<X11/XKBlib.h>

// OpenGL header files

#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>

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

typedef GLXContext(*glXCreateContextAttribsARBProc)(Display * , GLXFBConfig , GLXContext , Bool , const int *);

glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig glxFBConfig;

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

    // PP related 

    GLXFBConfig *glxFBConfigs = NULL;
    GLXFBConfig bestglxFBConfig;
    XVisualInfo *tempXvisualInfo = NULL;
    int numFBConfigs;

    int bestFramebufferConfig = -1 , bestNumberOfSamples = -1 ;
    int worstFramebufferConfig = -1, worstNumberOfSamples= 999 ; 
    int sampleBuffers , samples;
    int i;

 

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
        // for PP
        GLX_X_RENDERABLE,  True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        // as before
        GLX_DOUBLEBUFFER , True ,
        GLX_RED_SIZE  ,     8 ,
        GLX_GREEN_SIZE,     8 ,
        GLX_BLUE_SIZE ,     8 ,
        GLX_ALPHA_SIZE,     8 ,
        GLX_DEPTH_SIZE,     24,
        GLX_STENCIL_SIZE,   8,
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
    else
    {
        fprintf(gpFile , "logfile created \n");
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



    glxFBConfigs = glXChooseFBConfig(display , XDefaultScreen(display) , frameBufferAttributes , &numFBConfigs);
    if(glxFBConfigs == NULL)
    {
        fprintf(gpFile , "matching glxFBcongigs files cannot be found \n");
        uninitialize();
        exit(1);
    }
    fprintf(gpFile , "%d , matching FBconig found" , numFBConfigs);

    // find best matching FB config from above array

    for(i = 0 ; i < numFBConfigs ; i++)
    {
        tempXvisualInfo = glXGetVisualFromFBConfig(display , glxFBConfigs[i]);
        if(tempXvisualInfo != NULL)
        {
            // get sample buffers
            glXGetFBConfigAttrib(display , glxFBConfigs[i] , GLX_SAMPLE_BUFFERS , &sampleBuffers);
            // get samples
            glXGetFBConfigAttrib(display , glxFBConfigs[i] , GLX_SAMPLES , &sampleBuffers);

            if(bestFramebufferConfig < 0 || sampleBuffers && samples > bestNumberOfSamples)
            {
                bestFramebufferConfig = i;
                bestNumberOfSamples = samples;
            }

            if(worstFramebufferConfig < 0 || !sampleBuffers || samples < worstNumberOfSamples)
            {
                worstNumberOfSamples = samples;
            }

            XFree(tempXvisualInfo);    
        }
    }

    // accordingly get best glxfbconfig

    bestglxFBConfig = glxFBConfigs[bestFramebufferConfig];
    // assign this found bext glxfbconfig to global glxfbconfid

    glxFBConfig = bestglxFBConfig;

    // free the mem given to array

    XFree(glxFBConfigs);

    visualInfo = glXGetVisualFromFBConfig(display , bestglxFBConfig);

    fprintf(gpFile , "The Chosen visual id is = 0x%lu\n",visualInfo->visualid);



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

    int attribs_new[] = 
    {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    int attribs_old[] = 
    {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        None
    };

    // code

    // get the address of function in function pointer

    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((GLubyte *)"glXCreateContextAttribsARB");
    if(glXCreateContextAttribsARB == NULL)
    {
        fprintf(gpFile , "Cannot get required function address \n");
        uninitialize();
        exit(1);
    }

    // create PP compatibloe GLX context 

    glxContext = glXCreateContextAttribsARB(display , glxFBConfig , 0 , True , attribs_new );
    if(!glxContext)
    {
        fprintf(gpFile , "core profile based GLXContext cannot be obtained. \n Falling back to old context \n");
        
        // getting old context
        glxContext = glXCreateContextAttribsARB(display , glxFBConfig , 0 , True , attribs_old);
        if(!glxContext)
        {
            fprintf(gpFile , "old glXcontext cannot be found \n");
            uninitialize();
            exit(1);
        }
        else
        {
            fprintf(gpFile , "old glXcontext found \n");

        }
    }
    else
    {
        fprintf(gpFile , "core profile based GLXContext Obtained successfully \n");
    }

    // is the context support direct rendering

    if(!glXIsDirect(display , glxContext))
    {
        fprintf(gpFile , "not supporting direct rendering \n");
    }
    else
    {
        fprintf(gpFile , "supporting direct rendering \n");
    }

    if(glXMakeCurrent(display , window , glxContext) == False)
    {
        printf("glXCreateContext failed \n");
        return 2;
    }

    // enabling depth 

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST);


    // create OpenGl context

    glClearColor(0.0f , 0.0f , 1.0f , 1.0f);

    // warmup resize

    resize(WINWIDTH , WINHEIGHT);

    // return 0;

    // make this context as current context

}

void resize(int width, int height)
{
    // code
    if(height == 0)
    {
        height = 1;
    }
    glViewport(0,0,(GLsizei)width , (GLsizei)height);
}


void draw(void)
{
    // code

    glClear(GL_COLOR_BUFFER_BIT);

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


