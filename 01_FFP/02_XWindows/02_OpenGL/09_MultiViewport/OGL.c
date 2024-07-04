//standard header file
#include<stdio.h> //for printf
#include<stdlib.h> //for exit() function
#include<memory.h> //memset use karnar aahot tyasathi tikde zeromemory eekde memset

//X11 header files 
#include<X11/Xlib.h> // for all XWindow APIs
#include<X11/Xutil.h> // for xvisualinfo and related APIs
#include<X11/XKBlib.h>

//OpenGL header files
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

//macros
#define WINWIDTH 800
#define WINHEIGHT 600

//Global Variable Declarations
Display *display = NULL; //display he structure aahe -  xserver and client application madhla communication medium
Colormap colormap;
Window window; //structure la represent karnara
XVisualInfo *visualInfo = NULL;
Bool bActiveWindow = False;
Bool bFullscreen = False;
//OpenGL global var
GLXContext glxContext = NULL;


//entry point function
int main(void)
{
//local function declarations

void toggleFullscreen(void);
void initialize(void);
void resize(int, int);
void draw(void);
void update(void);
void uninitialize(void);

//local variable declarations
  int defaultScreen;
  XSetWindowAttributes windowattributes; //windowattributes analogous to wndclassEx
  int styleMask; // window chi style
  Atom windowManagerDelete;
  XEvent event;
  KeySym keySym;
  int screenWidth, screenHeight;
  char keys[26];
  int frameBufferAttributes[] =
                              {
                                GLX_DOUBLEBUFFER, True,
                                GLX_RGBA,
                                GLX_RED_SIZE, 8,
                                GLX_GREEN_SIZE, 8,
                                GLX_BLUE_SIZE, 8,
                                GLX_ALPHA_SIZE, 8,
                                None

                              };

  Bool bDone = False;                            
  
//code

//step 1: open the connection with XServer
  display = XOpenDisplay(NULL); //default displayserver yenar 
  if(display == NULL)
  {
    printf("XOpenDisplay() Failed./n");
    uninitialize();
    exit(1); //exit 0 and exit 1 he macro portable aahe 
  }
  
  //step 2: Get default screen from above display
  //graphic card jyala connect kelela asta to main monitor tyacha display windows madhe primary monitor mhnje eethli default screen
  defaultScreen = XDefaultScreen(display); //second API

  visualInfo = glXChooseVisual(display, defaultScreen, frameBufferAttributes);
 
  if(visualInfo == NULL)
  {
    printf("glxChooseVisual Failed./n");
    uninitialize();
    exit(1); //exit 0 and exit 1 he macro portable aahe 
  }
  
  //step 5: Set Window attributes/properties.
  memset((void*)&windowattributes, 0, sizeof(XSetWindowAttributes));
  windowattributes.border_pixel = 0; // 0 kinva null - default value will be given 
  windowattributes.background_pixel = XBlackPixel(display, visualInfo->screen); //hbrbackground analogy **  getstockobject - XBlackPixel
  windowattributes.background_pixmap=0;
  windowattributes.colormap = XCreateColormap(display,
                                              XRootWindow(display, visualInfo->screen),
                                              visualInfo->visual,
                                              AllocNone
                                              ); 
  //assign this color map to global color map
  colormap = windowattributes.colormap;


  
  // set style mask
  styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;   //ws_overlapped window analogy - 6 styles combo **CW - create window
  
  // Now finally create the window
  window = XCreateWindow(display,
                        XRootWindow(
                        display, 
                        visualInfo->screen),
                        0,
                        0,
                        WINWIDTH,
                        WINHEIGHT,
                        0,
                        visualInfo->depth,
                        InputOutput,
                        visualInfo->visual,
                        styleMask,
                        &windowattributes
                        ); //window madhla createwindow
                        
  if(!window)
  {
    printf("XOpenDisplay() Failed./n");
    uninitialize();
    exit(1); //exit 0 and exit 1 he macro portable aahe 
  }
  
  // Specify to which events this window should respond.
  XSelectInput(display, window, 
                ExposureMask|VisibilityChangeMask|StructureNotifyMask|KeyPressMask|ButtonPressMask|PointerMotionMask|FocusChangeMask);
                //wmpaint |                      | wmstyle            |wmchar        |lbuttondown etc. | wmmousemove
                
  // Specify Window Manager delete atom
  windowManagerDelete = XInternAtom(display, "WM_DELETE_WINDOW", True); //kayam tayar kar XInternAtom
  
  // Add/Set above atom as protocol for window manager
  XSetWMProtocols(display, window, &windowManagerDelete, 1);
  
  //Give caption to the window
  XStoreName(display, window, "Vaishnavi_chi_pahili_Assignment_XWindow");
  
  // Show/Map the window
  XMapWindow(display, window);
  
  //Center the window
  screenWidth = XWidthOfScreen
                            (XScreenOfDisplay(display, visualInfo->screen)); 
  screenHeight= XHeightOfScreen
                            (XScreenOfDisplay(display, visualInfo->screen)); 
                            //getsystemmetrics in windows
  XMoveWindow(display, 
              window, 
              (screenWidth - WINWIDTH) /2, 
              (screenHeight - WINHEIGHT) /2);


  //OpenGL Initialization
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
                            toggleFullscreen();
                            bFullscreen = True;
                        }
                        else
                        {
                            toggleFullscreen();
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


void toggleFullscreen(void)
{
  
//local variable declaration
Atom windowManagerStateNormal;
Atom windowManagerStateFullscreen;
XEvent event;

//code
windowManagerStateNormal = XInternAtom(display, "_NET_WM_STATE", False); //nasel trach tayar kar 

windowManagerStateFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

//Memset the event structure and fill it with above two atoms
memset((void*)&event, 0, sizeof(XEvent));

event.type = ClientMessage;
event.xclient.window = window;
event.xclient.message_type = windowManagerStateNormal;
event.xclient.format = 32; //eekdcha event struct mhnje tikcha message struct
event.xclient.data.l[0] = bFullscreen ? 0 : 1; //ternery operator
event.xclient.data.l[1] = windowManagerStateFullscreen;

//send the event
XSendEvent(display,
           XRootWindow(display, 
                       visualInfo->screen),
           False,
           SubstructureNotifyMask,
           &event
           );

}

void initialize(void)
{
  //local function declaration
  void resize(int, int);
  void uninitialize(void);
  //code
  //create OpenGL context 
  glxContext = glXCreateContext(display, visualInfo, NULL, True);
  if(glXMakeCurrent(display, window, glxContext) == False)
  {
    printf("glxCreateContext failed. /n");
    uninitialize();
    exit(1);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  //warmup resize
  resize(WINWIDTH, WINHEIGHT);


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
  //code
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(0.0f, 0.0f, -6.0f);

	glBegin(GL_TRIANGLES);
	{

		glVertex3f(0.0f, 1.0f, 0.0f);

		glVertex3f(-1.0f, -1.0f, 0.0f);

		glVertex3f(1.0f, -1.0f, 0.0f);

	}

  glXSwapBuffers(display, window);
}

void update(void)
{
  //code
}


void uninitialize(void)
{
  //local variable declarations
  GLXContext currentGLXContext = NULL;

  //codevisualInfo
  //free 
  if(visualInfo)
  {
    free(visualInfo);
    visualInfo = NULL;
  }

  //uncurrent the context 
  currentGLXContext = glXGetCurrentContext();
  if(currentGLXContext != NULL && currentGLXContext == glxContext)
  {
    glXMakeCurrent(display, 0, 0);
  }

  //
  if(glxContext)
  {
    glXDestroyContext(display, glxContext);
    glxContext = NULL;
  }

  if(window)
  {
    XDestroyWindow(display, window);
    
  }
  
  if(colormap)
  {
    XFreeColormap(display, colormap);
    
  }
  
  if(display)
  {
    XCloseDisplay(display);
    display = NULL;
    
  }
  
  
  
}


