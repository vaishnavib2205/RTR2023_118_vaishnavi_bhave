//standard header file
#include<stdio.h> //for printf
#include<stdlib.h> //for exit() function
#include<memory.h> //memset use karnar aahot tyasathi tikde zeromemory eekde memset

//X11 header files 
#include<X11/Xlib.h> // for all XWindow APIs
#include<X11/Xutil.h> // for xvisualinfo and related APIs
#include<X11/XKBlib.h>

//macros
#define winWidth 800
#define winHeight 600

//Global Variable Declarations
Display *display = NULL; //display he structure aahe -  xserver and client application madhla communication medium
Colormap colormap;
Window window; //structure la represent karnara

//entry point function
int main(void)
{
//local function declarations

  void uninitialize(void);
 
 //local variable declarations
  int defaultScreen;
  int defaultDepth; 
  XVisualInfo visualinfo;
  Status status;
  XSetWindowAttributes windowattributes; //windowattributes analogous to wndclassEx
  int styleMask; // window chi style
  Atom windowManagerDelete;
  XEvent event;
  KeySym keySym;
  
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
  
  //step 3: Get default depth from above two
  defaultDepth = XDefaultDepth(display, defaultScreen); //third API
  
  //step 4: Get visual info from above three 
  memset((void*)&visualinfo, 0, sizeof(XVisualInfo));
  status = XMatchVisualInfo(display, defaultScreen, defaultDepth, TrueColor, &visualinfo); //fourth API  //graphic card chya hardware la represent karnara data structure - Xvisualinfo // mla default depth ani default screen shi match honara visualinfo
  
  if(status == 0)
  {
     printf("XMatchVisualInfo Failed./n");
     uninitialize();
     exit(1); //exit 0 and exit 1 he macro portable aahe 
  }
  
  //step 5: Set Window attributes/properties.
  memset((void*)&windowattributes, 0, sizeof(XSetWindowAttributes));
  windowattributes.border_pixel = 0; // 0 kinva null - default value will be given 
  windowattributes.background_pixel = XBlackPixel(display, visualinfo.screen); //hbrbackground analogy **  getstockobject - XBlackPixel
  windowattributes.background_pixmap=0;
  windowattributes.colormap = XCreateColormap(display,
                                              XRootWindow(display, visualinfo.screen),
                                              visualinfo.visual,
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
                         visualinfo.screen),
                         0,
                         0,
                         winWidth,
                         winHeight,
                         0,
                         visualinfo.depth,
                         InputOutput,
                         visualinfo.visual,
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
                ExposureMask|VisibilityChangeMask|StructureNotifyMask|KeyPressMask|ButtonPressMask|PointerMotionMask);
                //wmpaint |                      | wmstyle            |wmchar        |lbuttondown etc. | wmmousemove
                
   // Specify Window Manager delete atom
   windowManagerDelete = XInternAtom(display, "WM_DELETE_WINDOW", True); //kayam tayar kar XInternAtom
   
   // Add/Set above atom as protocol for window manager
   XSetWMProtocols(display, window, &windowManagerDelete, 1);
   
   //Give caption to the window
   XStoreName(display, window, "Vaishnavi_chi_pahili_XWindow");
   
   // Show/Map the window
   XMapWindow(display, window);
   
   //Event Loop
   while(1)
   {
     XNextEvent(display, &event);
     
     switch(event.type)
     {
     
       case KeyPress:
          keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
           
           switch(keySym)
           {
             case XK_Escape:
               uninitialize();
               exit(0);
             break;
              
             default:
             break;
           }
             
        break;
         
        case 33:
          uninitialize();
          exit(0);
          
        break;
        
        default:
        break;
           
     }
   
   }
   
   
   uninitialize(); 

   return 0;
}



void uninitialize(void)
{
  //code
  
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


