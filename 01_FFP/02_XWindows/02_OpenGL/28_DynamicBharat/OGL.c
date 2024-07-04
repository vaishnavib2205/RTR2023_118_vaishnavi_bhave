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
GLXContext glxContext = NULL;

// FighterPlane Global variables

GLfloat x1_plane = -5.5f;
GLfloat y1_plane = 0.1f;

GLfloat x2_plane = -5.5f;
GLfloat y2_plane = 3.1f;

GLfloat x3_plane = -5.0f;
GLfloat y3_plane = -2.6f;

GLfloat rotatePlane = 90.0f;
//**************************************************************************************************************************
//***********************************///// BHARAT Global Variables /////****************************************************
//**************************************************************************************************************************

GLfloat x_letterB = -5.0f;
GLfloat y_letterB = 0.0f;

GLfloat x_letterH = -5.0f;
GLfloat y_letterH = 2.0f;

GLfloat x_letterA = -4.0f;
GLfloat y_letterA = -1.5f;

GLfloat x_letterR = 5.3f;
GLfloat y_letterR = 3.0f;

GLfloat x_letterA2 = 5.1f;
GLfloat y_letterA2 = 0.0f;

GLfloat x_letterT = 5.7f;
GLfloat y_letterT = 0.17f;



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


  //Initialize quadric
	
	//quadric = gluNewQuadric();


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
  	// Code
	void BHARAT(void);
	void drawfighterplane(void);

  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

	BHARAT();

	//********************************************************************************************************************************
	//**********************************************/// FIGHTERPLANE ///****************************************************************
	//********************************************************************************************************************************
	glLoadIdentity();

	glTranslatef(-0.7f, -0.75f, -3.0f);

	glPushMatrix();
	glTranslatef(x1_plane, y1_plane, -3.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	drawfighterplane();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(x2_plane, y2_plane, -3.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	glRotatef(-rotatePlane, 0.0f, 0.0f, 1.0f);
	drawfighterplane();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(x3_plane, y3_plane, -3.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	glRotatef(rotatePlane, 0.0f, 0.0f, 1.0f);
	drawfighterplane();
	glPopMatrix();

  glXSwapBuffers(display, window);
}

void update(void)
{
//code
if (x_letterB <= -3.0)
	{
		x_letterB += 0.0004f;

	}
	else if (x_letterB >= -3.0f && x_letterH <= -2.0f)
	{
		x_letterH += 0.0004f;
		y_letterH -= 0.000294f;

	}
	else if (x_letterH >= -2.0f && x_letterA <= -0.75f)
	{
		x_letterA += 0.00034f;
		y_letterA += 0.00031f;
	}
	else if (x_letterA >= -0.75f && x_letterR >= 0.7f)
	{
		x_letterR -= 0.00055f;
		y_letterR -= 0.00018f;
	}
	else if (x_letterR <= 0.7f && x_letterA2 >= 1.7f)
	{
		x_letterA2 -= 0.00045f;
		y_letterA2 += 0.0002f;
	}
	else if (x_letterA2 <= 1.7f && x_letterT >= 3.5f)
	{
		x_letterT -= 0.0004f;
	}

	//************************** // fighterPlane Transformation // ***************************
	//****************************************************************************************

	else if (x_letterT <= 3.5f)
	{
		rotatePlane -= 0.004f;
		if (rotatePlane <= 0.0f)
		{
			rotatePlane = 0.0f;

		}
		//x1_plane += 0.00009;
		//x2_plane += 0.00009;
		//x3_plane += 0.000073;
		//y2_plane -= 0.000099;
		//y3_plane += 0.000044;
		//
		//if (y2_plane <= 0.0f) {
		//	y2_plane = 0.0f;
		//	x2_plane += 0.00001f;
		// 	
		//}
		//
		//if (y3_plane >= 0.0f) {
		//	y3_plane = 0.0f;
		//	y1_plane = 0.0f;
		//	x3_plane += 0.00001f;
		//	x1_plane += 0.00002f;
		x1_plane += 0.00009f;
		x2_plane += 0.00009f;
		x3_plane += 0.00008f;
		y2_plane -= 0.0001f;
		y3_plane += 0.000099f;
	}
	if (y2_plane <= 0.1f)
	{
		y2_plane = 0.1f;
		x2_plane = x1_plane;
	}
	if (y3_plane >= 0.1f)
	{
		y3_plane = 0.1f;
		x3_plane = x1_plane;
	}

	if (x1_plane >= 3.0f && rotatePlane <= 0.0f)
	{
		y2_plane += 0.00005f;
		y3_plane -= 0.00005f;
		rotatePlane -= 1.0f;
	}

}

void drawfighterplane(void) {

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);

	// body 
	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.3f, 0.4f, 0.0f);
		glVertex3f(0.3f, 0.2f, 0.0f);
		glVertex3f(0.1f, 0.2f, 0.0f);
		glVertex3f(0.1f, 0.4f, 0.0f);

	}

	// back side quad of body

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.1f, 0.38f, 0.0f);
		glVertex3f(0.1f, 0.22f, 0.0f);
		glVertex3f(-0.1f, 0.22f, 0.0f);
		glVertex3f(-0.1f, 0.38f, 0.0f);

	}

	//front trapezium part
	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.3f, 0.4f, 0.0f);
		glVertex3f(0.3f, 0.2f, 0.0f);
		glVertex3f(0.45f, 0.25f, 0.0f);
		glVertex3f(0.45f, 0.35f, 0.0f);

	}

	// front lid
	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.45f, 0.35f, 0.0f);
		glVertex3f(0.45f, 0.25f, 0.0f);
		glVertex3f(0.65f, 0.30f, 0.0f);
		glVertex3f(0.65f, 0.30f, 0.0f);

	}

	// plane both sided trapezium shaped sides

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.29f, 0.41, 0.0f);
		glVertex3f(0.10f, 0.41, 0.0f);
		glVertex3f(0.10f, 0.6, 0.0f);
		glVertex3f(0.15f, 0.6, 0.0f);
	}

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.29f, 0.19f, 0.0f);
		glVertex3f(0.10f, 0.19f, 0.0f);
		glVertex3f(0.10f, 0.01, 0.0f);
		glVertex3f(0.15f, 0.01, 0.0f);
	}

	//1 short fang back

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.05f, 0.385f, 0.0f);
		glVertex3f(-0.12f, 0.385f, 0.0f);
		glVertex3f(-0.12f, 0.5f, 0.0f);
		glVertex3f(-0.05f, 0.5f, 0.0f);

	}

	//2

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.05f, 0.215f, 0.0f);
		glVertex3f(-0.12f, 0.215f, 0.0f);
		glVertex3f(-0.12f, 0.1f, 0.0f);
		glVertex3f(-0.05f, 0.1f, 0.0f);

	}

	// black quad for smoke of plane

	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.35f, 0.0f);
		glVertex3f(-0.1f, 0.35f, 0.0f);
		glVertex3f(-0.1f, 0.25f, 0.0f);
		glVertex3f(-0.05f, 0.25f, 0.0f);

	}

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(-0.05f, 0.34f, 0.0f);
		glVertex3f(-0.08f, 0.34f, 0.0f);
		glVertex3f(-0.08f, 0.26f, 0.0f);
		glVertex3f(-0.05f, 0.26f, 0.0f);

	}

	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.08f, 0.335f, 0.0f);
		glVertex3f(-0.11f, 0.335f, 0.0f);
		glVertex3f(-0.11f, 0.265f, 0.0f);
		glVertex3f(-0.08f, 0.265f, 0.0f);

	}

	// flag tricolor smoke

	{
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(-0.11f, 0.333f, 0.0f);
		//glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(-0.31f, 0.333f, 0.0f);
		//glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(-0.31f, 0.311f, 0.0f);
		//glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(-0.11f, 0.311f, 0.0f);

	}

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.31f, 0.311f, 0.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.11f, 0.311f, 0.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.11f, 0.289f, 0.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.31f, 0.289f, 0.0f);

	}

	{
		glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(-0.11f, 0.289f, 0.0f);
		//glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(-0.31f, 0.289f, 0.0f);
		//glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(-0.31f, 0.267f, 0.0f);
		//glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(-0.11f, 0.267f, 0.0f);

	}

	glEnd();

	//glDisable(GL_BLEND);



	// ***************** IAF letters****************


	glTranslatef(0.2f, 0.3f, 0.0f);
	glScalef(0.4f, 0.4f, 0.4f);

	glBegin(GL_QUADS);

	{
		// letter I

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, 0.1f, 0.0f);
		glVertex3f(-0.37f, 0.1f, 0.0f);
		glVertex3f(-0.37f, 0.075f, 0.0f);
		glVertex3f(-0.3f, 0.075f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.32f, 0.1f, 0.0f);
		glVertex3f(-0.35f, 0.1f, 0.0f);
		glVertex3f(-0.35f, -0.1f, 0.0f);
		glVertex3f(-0.32f, -0.1f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, -0.1f, 0.0f);
		glVertex3f(-0.37f, -0.1f, 0.0f);
		glVertex3f(-0.37f, -0.075f, 0.0f);
		glVertex3f(-0.3f, -0.075f, 0.0f);

		// letter A

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.2f, 0.1f, 0.0f);
		glVertex3f(-0.23f, 0.1f, 0.0f);
		glVertex3f(-0.28f, -0.1f, 0.0f);
		glVertex3f(-0.25f, -0.1f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.17f, 0.0f, 0.0f);
		glVertex3f(-0.25f, 0.0f, 0.0f);
		glVertex3f(-0.26f, -0.03f, 0.0f);
		glVertex3f(-0.17f, -0.03f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.2f, 0.1f, 0.0f);
		glVertex3f(-0.23f, 0.1f, 0.0f);
		glVertex3f(-0.17f, -0.1f, 0.0f);
		glVertex3f(-0.14f, -0.1f, 0.0f);

		// letter F

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.09f, 0.1f, 0.0f);
		glVertex3f(-0.12f, 0.1f, 0.0f);
		glVertex3f(-0.12f, -0.1f, 0.0f);
		glVertex3f(-0.09f, -0.1f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.02f, 0.1f, 0.0f);
		glVertex3f(-0.09f, 0.1f, 0.0f);
		glVertex3f(-0.09f, 0.075f, 0.0f);
		glVertex3f(-0.02f, 0.075f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.02f, 0.0f, 0.0f);
		glVertex3f(-0.09f, 0.0f, 0.0f);
		glVertex3f(-0.09f, 0.025f, 0.0f);
		glVertex3f(-0.02f, 0.025f, 0.0f);
	}
	glEnd();

}

void BHARAT(void) 
{
	void drawLetterB(void);
	void drawLetterH(void);
	void drawLetterA(void);
	void drawLetterR(void);
	void drawLetterA2(void);
	void drawLetterT(void);

	glTranslatef(-0.3f, -1.0f, -6.0f);

	glPushMatrix();
	glTranslatef(-3.0f, 0.0f, 0.0f);
	drawLetterB();
	glPopMatrix();

	//H
	glPushMatrix();
	glTranslatef(-2.0f, -0.1999f, 0.0f);
	drawLetterH();
	glPopMatrix();

	//A
	glPushMatrix();
	glTranslatef(-0.75f, 1.47f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawLetterA();
	glPopMatrix();

	//R
	glPushMatrix();
	glTranslatef(0.3f, 1.5f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 1.05f, 1.0f);
	drawLetterR();
	glPopMatrix();

	//A2
	glPushMatrix();
	glTranslatef(1.3f, 1.54f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 1.05f, 1.0f);
	drawLetterA2();
	glPopMatrix();

	//T
	glPushMatrix();
	glTranslatef(2.8f, 0.15f, 0.0f);
	glScalef(1.1f, 1.4f, 1.2f);
	drawLetterT();
	glPopMatrix();

}

void drawLetterB(void) 
{
	//glTranslatef(0.5f, -2.0f, -6.0f);
	glBegin(GL_QUADS);
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f); // orange 1.0f, 0.403f, 0.11f
		glVertex3f(0.3f, 0.7f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.3f, 1.3f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.5f, 1.3f, 0.0f);

	}

	{
		glColor3f(0.015, 0.415, 0.219); //green
		glVertex3f(0.5f, 0.3f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.3f, 0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.3f, 0.8f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.8f, 0.0f);

	}


	//{
	//	glColor3f(1.0f, 1.0f, 1.0f);
	//	glVertex3f(0.5f, 0.1f, 0.0f);
	//	glVertex3f(0.3f, 0.1f, 0.0f);
	//	glVertex3f(0.3f, 1.3f, 0.0f);
	//	glVertex3f(0.5f, 1.3f, 0.0f);
	//
	//}

	// - upper horizontal line

	{
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(1.0f, 1.5f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(1.0f, 1.3f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.3f, 1.3f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.3f, 1.5f, 0.0f);

	}

	// middle vertical line 1

	{
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(1.0f, 0.3f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.8f, 0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.8f, 0.8f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 0.6f, 0.0f);

	}

	// middle vertical line 2

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.8f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f); // orange 1.0f, 0.403f, 0.11f
		glVertex3f(1.0f, 0.9f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(1.0f, 1.3f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.8f, 1.3f, 0.0f);

	}


// horizontal lower line


	// middle horizontal line 
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.8f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.9f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.8f, 0.9f, 0.0f);

	}
	{
		glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(1.0f, 0.1f, 0.0f);
		glVertex3f(0.3f, 0.1f, 0.0f);
		glVertex3f(0.3f, 0.3f, 0.0f);
		glVertex3f(1.0f, 0.3f, 0.0f);

	}


	glEnd();
}

void drawLetterH(void) {

	//position p = 5.5 for x
	// 3.7 for y

	glBegin(GL_QUADS);

	// vertical line 1 left
	// upper 
	{
		glColor3f(0.015, 0.415, 0.219); // green 0.015, 0.415, 0.219
		glVertex3f(0.5f, 0.3f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.3f, 0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.3f, 1.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 1.0f, 0.0f);

	}
	//lower
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 1.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.3f, 1.0f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f); //orange
		glVertex3f(0.3f, 1.7f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.5f, 1.7f, 0.0f);

	}


	// verticle line right 2
	//upper
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.8f, 1.0f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.8f, 1.7f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(1.0f, 1.7f, 0.0f);

	}

	//lower
	{
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(1.0f, 0.3f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.8f, 0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.8f, 1.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

	}
	//horizontal middle line

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.8f, 0.9f, 0.0f);
		glVertex3f(0.5f, 0.9f, 0.0f);
		glVertex3f(0.5f, 1.2f, 0.0f);
		glVertex3f(0.8f, 1.2f, 0.0f);

	}



	glEnd();

}

	
void drawLetterA(void) {
	// 4.2 for  x 
	// 3.4 for y
	glBegin(GL_QUADS);
	// vertical line 1 left 
	{
		glColor3f(1.0f, 0.403f, 0.11f); //orange
		glVertex3f(0.2f, 0.0f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.7f, 0.0f);
	}

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.7f, 0.0f);
		glColor3f(0.015, 0.415, 0.219); //green
		glVertex3f(0.0f, 1.35f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.2f, 1.35f, 0.0f);
	}

	// vertical side line 

	{
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.7f, 0.2f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.5f, 0.2f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.6f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
	}

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.6f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.5f, 1.35f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.7f, 1.35f, 0.0f);
	}

	// horizontal upper line 

	{
		glColor3f(1.0f, 0.403f, 0.11f); //orange
		glVertex3f(0.2f, 0.0f, 0.0f);
		glVertex3f(0.7f, 0.0f, 0.0f);
		glVertex3f(0.7f, 0.2f, 0.0f);
		glVertex3f(0.2f, 0.2f, 0.0f);
	}

	// horizontal lower line

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.8f, 0.0f);
		glVertex3f(0.2f, 0.8f, 0.0f);
	}


	glEnd();
}


void drawLetterR(void) {

	// 2.8 for x coordinates 
	// 3.4 for y coordinates

	glBegin(GL_QUADS);
	// vertical line 1 left 
	{
		glColor3f(1.0f, 0.403f, 0.11f); //orange
		glVertex3f(0.2f, 0.0f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.7f, 0.0f);
	}

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.7f, 0.0f);
		glColor3f(0.015, 0.415, 0.219); //green
		glVertex3f(0.0f, 1.3f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.2f, 1.3f, 0.0f);
	}

	// vertical side line 

	{
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.7f, 0.2f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.5f, 0.2f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.6f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
	}

	// horizontal upper line 

	{
		glColor3f(1.0f, 0.403f, 0.11f); //orange
		glVertex3f(0.2f, 0.0f, 0.0f);
		glVertex3f(0.7f, 0.0f, 0.0f);
		glVertex3f(0.7f, 0.2f, 0.0f);
		glVertex3f(0.2f, 0.2f, 0.0f);
	}

	// horizontal lower line

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.8f, 0.0f);
		glVertex3f(0.2f, 0.8f, 0.0f);
	}

	//slant line

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.45f, 0.8f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.8f, 0.0f); 
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.55f, 1.3f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.8f, 1.3f, 0.0f);
		
	}


	glEnd();
}


void drawLetterA2(void) {


	glBegin(GL_QUADS);
	// vertical line 1 left 
	{
		glColor3f(1.0f, 0.403f, 0.11f); //orange
		glVertex3f(0.2f, 0.0f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.7f, 0.0f);
	}

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.7f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.7f, 0.0f);
		glColor3f(0.015, 0.415, 0.219); //green
		glVertex3f(0.0f, 1.35f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.2f, 1.35f, 0.0f);
	}

	// vertical side line 

	{
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.7f, 0.2f, 0.0f);
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(0.5f, 0.2f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.6f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
	}

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.6f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.5f, 1.35f, 0.0f);
		glColor3f(0.015, 0.415, 0.219);
		glVertex3f(0.7f, 1.35f, 0.0f);
	}

	// horizontal upper line 

	{
		glColor3f(1.0f, 0.403f, 0.11f); //orange
		glVertex3f(0.2f, 0.0f, 0.0f);
		glVertex3f(0.7f, 0.0f, 0.0f);
		glVertex3f(0.7f, 0.2f, 0.0f);
		glVertex3f(0.2f, 0.2f, 0.0f);
	}

	// horizontal lower line

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.2f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.8f, 0.0f);
		glVertex3f(0.2f, 0.8f, 0.0f);
	}


	glEnd();
}
	
	
void drawLetterT(void) 
{
	glBegin(GL_QUADS);
	{
		//orange 1.0f, 0.403f, 0.11f
	glColor3f(1.0f, 0.403f, 0.11f);
	glVertex3f(0.5f, 0.8f, 0.0f);
	glVertex3f(0.5f, 1.0f, 0.0f);
	glVertex3f(-0.5f, 1.0f, 0.0f);
	glVertex3f(-0.5f, 0.8f, 0.0f);

	}

	{
//green 0.015, 0.415, 0.219

	glColor3f(0.015, 0.415, 0.219);
	glVertex3f(0.1f, 0.2f, 0.0f);
	glVertex3f(-0.1f, 0.2f, 0.0f);
	glVertex3f(-0.1f, 0.0f, 0.0f);
	glVertex3f(0.1f, 0.0f, 0.0f);

	}
	{
	glColor3f(1.0f, 1.0f, 1.0f); 
	glVertex3f(0.1f, 0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, 0.5f, 0.0f);
	glColor3f(0.015, 0.415, 0.219);
	glVertex3f(-0.1f, 0.2f, 0.0f);
	glColor3f(0.015, 0.415, 0.219);
	glVertex3f(0.1f, 0.2f, 0.0f);

	}

	{
	glColor3f(1.0f, 0.403f, 0.11f);
	glVertex3f(0.1f, 0.8f, 0.0f);
	glColor3f(1.0f, 0.403f, 0.11f);
	glVertex3f(-0.1f, 0.8f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, 0.5, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.1f, 0.5f, 0.0f);

	}
	glEnd();
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


