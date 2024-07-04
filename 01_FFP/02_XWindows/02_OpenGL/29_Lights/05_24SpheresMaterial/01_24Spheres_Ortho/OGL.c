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

Bool bLight = False;

GLfloat lightAmbient[] = { 0.0f ,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
GLfloat lightPosition[] = { 0.0f , 0.0f , 0.0f , 1.0f };

GLfloat angleForXRotation = 0.0f;
GLfloat angleForYRotation = 0.0f;
GLfloat angleForZRotation = 0.0f;

GLuint keyPressed = 0;
GLfloat cAngle = 0.0f;

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

//for animation of light
GLfloat lightAngleZero = 0.0f;
GLfloat lightAngleOne = 0.0f;
GLfloat lightAngleTwo = 0.0f;

//iinitialisation of quadric
GLUquadric* quadric = NULL;


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

						
					case 'L':
					case 'l':
						if (bLight == False) 
						{
							glEnable(GL_LIGHTING);
							bLight = True;
						}
						else{
							glDisable(GL_LIGHTING);
							bLight = False;
						}
						break;

					case 'X':
                        case 'x':

                            keyPressed = 1;
                            angleForXRotation = 0.0f; // resset the angle here
                            break;

					case 'Y':
					case 'y':

						keyPressed = 2;
						angleForYRotation = 0.0f; // reset the angle for y here
						break;

					case 'Z':
					case 'z':

						keyPressed = 3;
						angleForZRotation = 0.0f; // reset the angle for z here
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

  // initialize quadric

	quadric = gluNewQuadric();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// set the clear colour of window to blue

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // dark greay backGround

	// Here OpenGL starts.....

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glEnable(GL_LIGHT0);

  //warmup resize

  resize(WINWIDTH, WINHEIGHT);

}

void resize(int width, int height)
{
    if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (width < height)
	{
		glOrtho(
			0.0f,
			15.5f,
			0.0f * ((GLfloat)height / (GLfloat)width),
			15.5 * ((GLfloat)height / (GLfloat)width),
			-10.0f,
			10.0f
		);
	}
	else
	{
		glOrtho(
			0.0f * ((GLfloat)width / (GLfloat)height),
			15.5f * ((GLfloat)width / (GLfloat)height),
			0.0f,
			15.5f,
			-10.0f,
			10.0f
		);
	}

}


void draw(void)
{

  // variable declaration

	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat shinniness;

  	
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// animation

	if (keyPressed == 1)
	{
		glRotatef(angleForXRotation, 1.0f, 0.0f, 0.0f);
		lightPosition[2] = angleForXRotation;
	}

	if (keyPressed == 2)
	{
		glRotatef(angleForYRotation, 0.0f, 1.0f, 0.0f);
		lightPosition[0] = angleForYRotation;
	}

	if (keyPressed == 3)
	{
		glRotatef(angleForZRotation, 0.0f, 0.0f, 1.0f);
		lightPosition[1] = angleForZRotation;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// first column of precious stones

	// 1st column, emerald *****

	materialAmbient[0] = 0.0215;
	materialAmbient[1] = 0.1745;
	materialAmbient[2] = 0.0215;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material
	materialDiffuse[0] = 0.07568; // r
	materialDiffuse[1] = 0.61424; // g
	materialDiffuse[2] = 0.07568; // b
	materialDiffuse[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material 
	materialSpecular[0] = 0.633;    // r
	materialSpecular[1] = 0.727811; // g
	materialSpecular[2] = 0.633;    // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(1.5f, 14.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	// ***** 2nd sphere on 1st column, jade *****

	materialAmbient[0] = 0.135;  // r
	materialAmbient[1] = 0.2225; // g
	materialAmbient[2] = 0.1575; // b
	materialAmbient[3] = 1.0f;   // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material
// diffuse material
	materialDiffuse[0] = 0.54; // r
	materialDiffuse[1] = 0.89; // g
	materialDiffuse[2] = 0.63; // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.316228; // r
	materialSpecular[1] = 0.316228; // g
	materialSpecular[2] = 0.316228; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(1.5f, 11.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	// ***** 3rd sphere on 1st column, obsidian *****
// ambient material

	materialAmbient[0] = 0.05375; // r
	materialAmbient[1] = 0.05;    // g
	materialAmbient[2] = 0.06625; // b
	materialAmbient[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material
// diffuse material
	materialDiffuse[0] = 0.18275; // r
	materialDiffuse[1] = 0.17;    // g
	materialDiffuse[2] = 0.22525; // b
	materialDiffuse[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.332741; // r
	materialSpecular[1] = 0.328634; // g
	materialSpecular[2] = 0.346435; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.3 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(1.5f, 9.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 4th sphere on 1st column, pearl *****
// ambient material

	materialAmbient[0] = 0.25;    // r
	materialAmbient[1] = 0.20725; // g
	materialAmbient[2] = 0.20725; // b
	materialAmbient[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material
// diffuse material
	materialDiffuse[0] = 1.0;   // r
	materialDiffuse[1] = 0.829; // g
	materialDiffuse[2] = 0.829; // b
	materialDiffuse[3] = 1.0f;  // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.296648; // r
	materialSpecular[1] = 0.296648; // g
	materialSpecular[2] = 0.296648; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.088 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(1.5f, 6.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 5th sphere on 1st column, ruby *****
	// ambient material
	materialAmbient[0] = 0.1745;  // r
	materialAmbient[1] = 0.01175; // g
	materialAmbient[2] = 0.01175; // b
	materialAmbient[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material
// diffuse material
	materialDiffuse[0] = 0.61424; // r
	materialDiffuse[1] = 0.04136; // g
	materialDiffuse[2] = 0.04136; // b
	materialDiffuse[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.727811; // r
	materialSpecular[1] = 0.626959; // g
	materialSpecular[2] = 0.626959; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(1.5f, 4.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 6th sphere on 1st column, turquoise *****
	// ambient material
	materialAmbient[0] = 0.1;     // r
	materialAmbient[1] = 0.18725; // g
	materialAmbient[2] = 0.1745;  // b
	materialAmbient[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material
// diffuse material
// diffuse material
	materialDiffuse[0] = 0.396;   // r
	materialDiffuse[1] = 0.74151; // g
	materialDiffuse[2] = 0.69102; // b
	materialDiffuse[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.297254; // r
	materialSpecular[1] = 0.30829;  // g
	materialSpecular[2] = 0.306678; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(1.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 1st sphere on 2nd column, brass *****
	// ambient material
	materialAmbient[0] = 0.329412; // r
	materialAmbient[1] = 0.223529; // g
	materialAmbient[2] = 0.027451; // b
	materialAmbient[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.780392; // r
	materialDiffuse[1] = 0.568627; // g
	materialDiffuse[2] = 0.113725; // b
	materialDiffuse[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.992157; // r
	materialSpecular[1] = 0.941176; // g
	materialSpecular[2] = 0.807843; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.21794872 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(7.5f, 14.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 2nd sphere on 2nd column, bronze *****
	// ambient material
	materialAmbient[0] = 0.2125; // r
	materialAmbient[1] = 0.1275; // g
	materialAmbient[2] = 0.054;  // b
	materialAmbient[3] = 1.0f;   // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.714;   // r
	materialDiffuse[1] = 0.4284;  // g
	materialDiffuse[2] = 0.18144; // b
	materialDiffuse[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.393548; // r
	materialSpecular[1] = 0.271906; // g
	materialSpecular[2] = 0.166721; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.2 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(7.5f, 11.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 3rd sphere on 2nd column, chrome *****
	// ambient material
	materialAmbient[0] = 0.25; // r
	materialAmbient[1] = 0.25; // g
	materialAmbient[2] = 0.25; // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.4;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.774597; // r
	materialSpecular[1] = 0.774597; // g
	materialSpecular[2] = 0.774597; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(7.5f, 9.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 4th sphere on 2nd column, copper *****
	// ambient material
	materialAmbient[0] = 0.19125; // r
	materialAmbient[1] = 0.0735;  // g
	materialAmbient[2] = 0.0225;  // b
	materialAmbient[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.7038;  // r
	materialDiffuse[1] = 0.27048; // g
	materialDiffuse[2] = 0.0828;  // b
	materialDiffuse[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.256777; // r
	materialSpecular[1] = 0.137622; // g
	materialSpecular[2] = 0.086014; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(7.5f, 6.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 5th sphere on 2nd column, gold *****
	// ambient material
	materialAmbient[0] = 0.24725; // r
	materialAmbient[1] = 0.1995;  // g
	materialAmbient[2] = 0.0745;  // b
	materialAmbient[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.75164; // r
	materialDiffuse[1] = 0.60648; // g
	materialDiffuse[2] = 0.22648; // b
	materialDiffuse[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.628281; // r
	materialSpecular[1] = 0.555802; // g
	materialSpecular[2] = 0.366065; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.4 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(7.5f, 4.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 6th sphere on 2nd column, silver *****
	// ambient material
	materialAmbient[0] = 0.19225; // r
	materialAmbient[1] = 0.19225; // g
	materialAmbient[2] = 0.19225; // b
	materialAmbient[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.50754; // r
	materialDiffuse[1] = 0.50754; // g
	materialDiffuse[2] = 0.50754; // b
	materialDiffuse[3] = 1.0f;    // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.508273; // r
	materialSpecular[1] = 0.508273; // g
	materialSpecular[2] = 0.508273; // b
	materialSpecular[3] = 1.0f;     // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.4 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(7.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// *******************************************************

// ***** 1st sphere on 3rd column, black *****
// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.01; // r
	materialDiffuse[1] = 0.01; // g
	materialDiffuse[2] = 0.01; // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.50; // r
	materialSpecular[1] = 0.50; // g
	materialSpecular[2] = 0.50; // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(13.5f, 14.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	// ***** 2nd sphere on 3rd column, cyan *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.1;  // g
	materialAmbient[2] = 0.06; // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.0;        // r
	materialDiffuse[1] = 0.50980392; // g
	materialDiffuse[2] = 0.50980392; // b
	materialDiffuse[3] = 1.0f;       // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.50196078; // r
	materialSpecular[1] = 0.50196078; // g
	materialSpecular[2] = 0.50196078; // b
	materialSpecular[3] = 1.0f;       // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(13.5f, 11.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	// ***** 3rd sphere on 2nd column, green *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.1;  // r
	materialDiffuse[1] = 0.35; // g
	materialDiffuse[2] = 0.1;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.45; // r
	materialSpecular[1] = 0.55; // g
	materialSpecular[2] = 0.45; // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(13.5f, 9.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	// ***** 4th sphere on 3rd column, red *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.0;  // g
	materialDiffuse[2] = 0.0;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.6;  // g
	materialSpecular[2] = 0.6;  // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(13.5f, 6.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	// ***** 5th sphere on 3rd column, white *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.55; // r
	materialDiffuse[1] = 0.55; // g
	materialDiffuse[2] = 0.55; // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.70; // r
	materialSpecular[1] = 0.70; // g
	materialSpecular[2] = 0.70; // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(13.5f, 4.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 6th sphere on 3rd column, yellow plastic *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.0;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.60; // r
	materialSpecular[1] = 0.60; // g
	materialSpecular[2] = 0.50; // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(13.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 1st sphere on 4th column, black *****
	// ambient material
	materialAmbient[0] = 0.02; // r
	materialAmbient[1] = 0.02; // g
	materialAmbient[2] = 0.02; // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.01; // r
	materialDiffuse[1] = 0.01; // g
	materialDiffuse[2] = 0.01; // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.4;  // r
	materialSpecular[1] = 0.4;  // g
	materialSpecular[2] = 0.4;  // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(19.5f, 14.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 2nd sphere on 4th column, cyan *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.05; // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.5;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.04; // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.7;  // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(19.5f, 11.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 3rd sphere on 4th column, green *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.04; // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(19.5f, 9.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

	// ***** 4th sphere on 4th column, red *****
// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.4;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.04; // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(19.5f, 6.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 5th sphere on 4th column, white *****
	// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.05; // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.5;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.7;  // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(19.5f, 4.0f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);


	// ***** 6th sphere on 4th column, yellow rubber *****
// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	// diffuse material

	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	shinniness = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

	glLoadIdentity();
	glTranslatef(19.5f, 1.5f, 0.0f);
	gluSphere(quadric, 1.0f, 30, 30);

  glXSwapBuffers(display, window);
}

void update(void)
{
// Code

	if (keyPressed == 1)
	{
		angleForXRotation += 0.09f;
		if (angleForXRotation > 360.0f)
		{
			angleForXRotation -= 360.0f;
		}
	}
	if (keyPressed == 2)
	{
		angleForYRotation += 0.09f;
		if (angleForYRotation > 360.0f)
		{
			angleForYRotation -= 360.0f;
		}
	}
	if (keyPressed == 3)
	{
		angleForZRotation += 0.09f;
		if (angleForZRotation > 360.0f)
		{
			angleForZRotation -= 360.0f;
		}
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


