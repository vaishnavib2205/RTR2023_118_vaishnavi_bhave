//standard header file
#include<stdio.h> //for printf
#include<stdlib.h> //for exit() function
#include<memory.h> //memset use karnar aahot tyasathi tikde zeromemory eekde memset

#define USE_MATH_DEFINES 1
#include<math.h>

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

GLfloat cAngle = 0.0f;

GLfloat identityMatrix[16];
GLfloat translationMatrix[16];
GLfloat scaleMatrix[16];
GLfloat rotationMatrix_X[16];
GLfloat rotationMatrix_Y[16];
GLfloat rotationMatrix_Z[16];

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

  //enabling depth

  glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST);


  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  //warmup resize

  // initialization of mitrix
	{ 
		// Identity matrix
		// column 1
		identityMatrix[0] = 1.0f;
		identityMatrix[1] = 0.0f;
		identityMatrix[2] = 0.0f;
		identityMatrix[3] = 0.0f;
		// column 2
		identityMatrix[4] = 0.0f;
		identityMatrix[5] = 1.0f;
		identityMatrix[6] = 0.0f;
		identityMatrix[7] = 0.0f;
		// column 3
		identityMatrix[8] = 0.0f;
		identityMatrix[9] = 0.0f;
		identityMatrix[10] = 1.0f;
		identityMatrix[11] = 0.0f;
		// column 4
		identityMatrix[12] = 0.0f;
		identityMatrix[13] = 0.0f;
		identityMatrix[14] = 0.0f;
		identityMatrix[15] = 1.0f;
	}
	{
		// translation matrix
		//column 1
		translationMatrix[0] = 1.0f;
		translationMatrix[1] = 0.0f;
		translationMatrix[2] = 0.0f;
		translationMatrix[3] = 0.0f;
		//column 2
		translationMatrix[4] = 0.0f;
		translationMatrix[5] = 1.0f;
		translationMatrix[6] = 0.0f;
		translationMatrix[7] = 0.0f;
		// column 3
		translationMatrix[8] = 0.0f;
		translationMatrix[9] = 0.0f;
		translationMatrix[10] = 1.0f;
		translationMatrix[11] = 0.0f;
		//column 4
		translationMatrix[12] = 0.0f;
		translationMatrix[13] = 0.0f;
		translationMatrix[14] = -6.0f;
		translationMatrix[15] = 1.0f;

	}
	{
		// scale matrix
		// column 1
		scaleMatrix[0] = 0.75f;
		scaleMatrix[1] = 0.0f;
		scaleMatrix[2] = 0.0f;
		scaleMatrix[3] = 0.0f;
		//column2
		scaleMatrix[4] = 0.0f;
		scaleMatrix[5] = 0.75f;
		scaleMatrix[6] = 0.0f;
		scaleMatrix[7] = 0.0f;
		// column 3
		scaleMatrix[8] = 0.0f;
		scaleMatrix[9] = 0.0f;
		scaleMatrix[10] = 0.75f;
		scaleMatrix[11] = 0.0f;
		// column 4
		scaleMatrix[12] = 0.0f;
		scaleMatrix[13] = 0.0f;
		scaleMatrix[14] = 0.0f;
		scaleMatrix[15] = 1.0f;
		
	}

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

	// glLoadIdentity();
	glLoadMatrixf(identityMatrix);

	// glTranslatef(0.0f, 0.0f, -6.0f);
	glMultMatrixf(translationMatrix);

	// glScalef(0.75f, 0.75f, 0.75f);
	glMultMatrixf(scaleMatrix);

	//glRotatef(cAngle, 1.0f, 0.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 1.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 0.0f, 1.0f);

	cAngle = cAngle * (M_PI / 180.0f);

	{
		// rotation through X
		// column 1
		rotationMatrix_X[0] = 1.0f;
		rotationMatrix_X[1] = 0.0f;
		rotationMatrix_X[2] = 0.0f;
		rotationMatrix_X[3] = 0.0f;
		//column 2
		rotationMatrix_X[4] = 0.0f;
		rotationMatrix_X[5] = cos(cAngle);
		rotationMatrix_X[6] = sin(cAngle);
		rotationMatrix_X[7] = 0.0f;
		// column 3
		rotationMatrix_X[8] = 0.0f;
		rotationMatrix_X[9] = -sin(cAngle);
		rotationMatrix_X[10] = cos(cAngle);
		rotationMatrix_X[11] = 0.0f;
		// column 4
		rotationMatrix_X[12] = 0.0f;
		rotationMatrix_X[13] = 0.0f;
		rotationMatrix_X[14] = 0.0f;
		rotationMatrix_X[15] = 1.0f;
	}
	{
		// rotation through Y
		// column 1
		rotationMatrix_Y[0] = cos(cAngle);
		rotationMatrix_Y[1] = 0.0f;
		rotationMatrix_Y[2] = -sin(cAngle);
		rotationMatrix_Y[3] = 0.0f;
		// column 2
		rotationMatrix_Y[4] = 0.0f;
		rotationMatrix_Y[5] = 1.0f;
		rotationMatrix_Y[6] = 0.0f;
		rotationMatrix_Y[7] = 0.0f;
		// column 3
		rotationMatrix_Y[8] =sin(cAngle);
		rotationMatrix_Y[9] = 0.0f;
		rotationMatrix_Y[10] = cos(cAngle);
		rotationMatrix_Y[11] = 0.0f;
		// column 4
		rotationMatrix_Y[12] = 0.0f;
		rotationMatrix_Y[13] = 0.0f;
		rotationMatrix_Y[14] = 0.0f;
		rotationMatrix_Y[15] = 1.0f;

	}
	{
		// rotation through Z
		//column 1
		rotationMatrix_Z[0] = cos(cAngle);
		rotationMatrix_Z[1] = sin(cAngle);
		rotationMatrix_Z[2] = 0.0f;
		rotationMatrix_Z[3] = 0.0f;
		// column 3
		rotationMatrix_Z[4] = -sin(cAngle);
		rotationMatrix_Z[5] = cos(cAngle);
		rotationMatrix_Z[6] = 0.0f;
		rotationMatrix_Z[7] = 0.0f;
		// column 3
		rotationMatrix_Z[8] = 0.0f;
		rotationMatrix_Z[9] = 0.0f;
		rotationMatrix_Z[10] = 1.0f;
		rotationMatrix_Z[11] = 0.0f;
		// column 4
		rotationMatrix_Z[12] = 0.0f;
		rotationMatrix_Z[13] = 0.0f;
		rotationMatrix_Z[14] = 0.0f;
		rotationMatrix_Z[15] = 1.0f;

	}

	glMultMatrixf(rotationMatrix_X);
	glMultMatrixf(rotationMatrix_Y);
	glMultMatrixf(rotationMatrix_Z);
	

	glBegin(GL_QUADS);
	// front face
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
	}
	// right face
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
	}
	// Back face
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
	}
	// Left face
	{
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
	}
	// Top face
	{
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
	}
	// Bottom face
	{
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);

	}

	glEnd();

  glXSwapBuffers(display, window);
}

void update(void)
{
  // Code
	cAngle = cAngle - 0.0005f;
	if (cAngle <= 0.0f)
	{
		cAngle = cAngle + 360.0f;
	}

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


