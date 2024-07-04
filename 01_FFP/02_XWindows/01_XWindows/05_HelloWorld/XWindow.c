//standard header file
#include<stdio.h> //for printf
#include<stdlib.h> //for exit() function
#include<memory.h> //memset use karnar aahot tyasathi tikde zeromemory eekde memset

//X11 header files 
#include<X11/Xlib.h> // for all XWindow APIs
#include<X11/Xutil.h> // for xvisualinfo and related APIs
#include<X11/XKBlib.h>

//macros
#define WINWIDTH 800
#define WINHEIGHT 600

//Global Variable Declarations
Display *display = NULL; //display he structure aahe -  xserver and client application madhla communication medium
Colormap colormap;
Window window; //structure la represent karnara
XVisualInfo visualInfo;

Bool bFullscreen = False;

//entry point function
int main(void)
{
//local function declarations

  void toggleFullscreen(void);

  void uninitialize(void);

//local variable declarations
  int defaultScreen;
  int defaultDepth; 
  Status status;
  XSetWindowAttributes windowattributes; //windowattributes analogous to wndclassEx
  int styleMask; // window chi style
  Atom windowManagerDelete;
  XEvent event;
  KeySym keySym;
  int screenWidth, screenHeight;
  char keys[26];
  XColor greenColor;
  XGCValues gcValues; 

  static XFontStruct *pFontStruct = NULL;
  static int winWidth, winHeight;
  static GC gc; //graphic context
  int stringLength;
  int stringWidth;
  int fontHeight;
  char str[] = "Hello World!!!";
  
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
  memset((void*)&visualInfo, 0, sizeof(XVisualInfo));
  status = XMatchVisualInfo(display, defaultScreen, defaultDepth, TrueColor, &visualInfo); //fourth API  //graphic card chya hardware la represent karnara data structure - Xvisualinfo // mla default depth ani default screen shi match honara visualinfo
  
  if(status == 0)
  {
    printf("XMatchVisualInfo Failed./n");
    uninitialize();
    exit(1); //exit 0 and exit 1 he macro portable aahe 
  }
  
  //step 5: Set Window attributes/properties.
  memset((void*)&windowattributes, 0, sizeof(XSetWindowAttributes));
  windowattributes.border_pixel = 0; // 0 kinva null - default value will be given 
  windowattributes.background_pixel = XBlackPixel(display, visualInfo.screen); //hbrbackground analogy **  getstockobject - XBlackPixel
  windowattributes.background_pixmap=0;
  windowattributes.colormap = XCreateColormap(display,
                                              XRootWindow(display, visualInfo.screen),
                                              visualInfo.visual,
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
                        visualInfo.screen),
                        0,
                        0,
                        WINWIDTH,
                        WINHEIGHT,
                        0,
                        visualInfo.depth,
                        InputOutput,
                        visualInfo.visual,
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
  XStoreName(display, window, "Vaishnavi_chi_pahili_XWindow");
  
  // Show/Map the window
  XMapWindow(display, window);
  
  //Center the window
  screenWidth = XWidthOfScreen
                            (XScreenOfDisplay(display, visualInfo.screen)); 
  screenHeight= XHeightOfScreen
                            (XScreenOfDisplay(display, visualInfo.screen)); 
                            //getsystemmetrics in windows
  XMoveWindow(display, 
              window, 
              (screenWidth - WINWIDTH) /2, 
              (screenHeight - WINHEIGHT) /2);

  //memset((void*)&event, sizeof(XNextEvent));
  //Event Loop
  while(1)
  {
    XNextEvent(display, &event);
    
    switch(event.type)
    {

      case MapNotify:
      pFontStruct = XLoadQueryFont(display, "fixed");
      break; // mapnotify -> <- showwindow WM_CREATE

      case FocusIn:
      break;

      case FocusOut:
      break;
      
      case ConfigureNotify:
      winWidth = event.xconfigure.width; //width ghetli
      winHeight = event.xconfigure.height; // height ghetli
      break;

      case Expose:  //wm_paint
      gc = XCreateGC(display, window, 0, &gcValues);
      XSetFont(display, gc, pFontStruct->fid);
      XAllocNamedColor(display, colormap, "green", &greenColor, &greenColor);
      XSetForeground(display, gc, greenColor.pixel);

      stringLength = strlen(str);
      stringWidth = XTextWidth(pFontStruct, str, stringLength);   
      fontHeight = pFontStruct->ascent + pFontStruct -> descent;
      XDrawString(display, window, gc, (winWidth - stringWidth)/2, (winHeight - fontHeight)/2, str, stringLength);   
      break;

      case ButtonPress:
      switch(event.xbutton.button)
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

      case FocusChangeMask:
      printf("FocusChangeMask event is clicked\n");
      break;
    
      case KeyPress:
          keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
          
          switch(keySym)
          {
            case XK_Escape:
            XUnloadFont(display, pFontStruct->fid);
            XFreeGC(display, gc);
              uninitialize();
              exit(0);
            break;
              
            default:
            break;
          }

          XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
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
              
            default:
            break;
          }
            
        break;
        
        case 33:
        XUnloadFont(display, pFontStruct->fid);
        XFreeGC(display, gc);
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
                       visualInfo.screen),
           False,
           SubstructureNotifyMask,
           &event
           );

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


