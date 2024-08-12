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
#include<GL/glu.h>

// header file for vmath
#include "vmath.h"
#include "Sphere.h"

//  header file for soil

#include<SOIL/SOIL.h>

using namespace vmath;

// macros

#define WINWIDTH 800
#define WINHEIGHT 600

#define FBO_WIDTH 512
#define FBO_HEIGHT 512

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

GLuint shaderProgramObjectCube = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCOORD,
	AMC_ATTRIBUTE_NORMALS// lights
};

GLint winWidth = 0;
GLint winHeight = 0;

// variables for cube

GLuint vao_Cube = 0;
GLuint vbo_position_cube = 0;
GLuint vbo_Texcoord_cube = 0;
GLfloat angle_cube = 0.0f;

GLuint texture_kundali = 0;

GLuint textureSamplerUniformCube = 0;
GLuint mvpMatrixUniformCube = 0;


mat4 perspectiveProjectionMatrixCube; // mat4 datatype is in vmath.h header file, that means 4X4 matrix

// sphere related global variables

GLuint shaderProgramObject_pv_Sphere = 0;
GLuint shaderProgramObject_pf_Sphere = 0;

// variables for sphere
GLuint gVao_sphere = 0;
GLuint gVbo_sphere_position = 0;
GLuint gVbo_sphere_normal = 0; // lights
GLuint gVbo_sphere_texcoord = 0; // lights
GLuint gVbo_sphere_element = 0;

struct lights
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec4 position;
};

struct lights light[3];

GLuint modelMatrixUniform_Sphere = 0;
GLuint projectionMatrixUniform_Sphere = 0;
GLuint viewMatrixUniform_Sphere = 0;

GLuint lightDiffuseUniform_Sphere[3]; // for diffuse light
GLuint materialDiffuseUniform_Sphere = 0; // for diffuse material
GLuint lightAmbientUniform_Sphere[3]; // for ambient light
GLuint materialAmbientUniform_Sphere = 0; // for ambient material
GLuint lightSpecularUniform_Sphere[3]; // for specular light
GLuint materialSpecularUniform_Sphere = 0; // for specular material
GLuint materialShinenessUniform_Sphere = 0; // for shinneness material
GLuint lightPositionUniform_Sphere[3];
GLuint keyPressUniform_Sphere = 0;

Bool bLightingEnabled = False;
Bool bLightingEnabled_pv = False;
Bool bLightingEnabled_pf = False;

GLfloat materialAmbient[] = { 0.0f , 0.0f , 0.0f , 1.0f };
GLfloat materialDiffuse[] = { 0.5f , 0.2f , 0.7f , 1.0f };
GLfloat materialSpecular[] = { 0.7f , 0.7f , 0.7f , 1.0f };
GLfloat materialShineness = 50.0f;

GLfloat lightAngleZero = 0.0f;
GLfloat lightAngleOne = 0.0f;
GLfloat lightAngleTwo = 0.0f;

// sphere variables

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLuint gNumVertices;
GLuint gNumElements;

mat4 perspectiveProjectionMatrix_Sphere; // mat4 datatype is in vmath.h header file, that means 4X4 matrix

// FBO related global variables

GLuint fbo;
GLuint rbo;
GLuint texture_FBO;
Bool vFboResult = False;

// main

int main(void)
{
    // local function declaration
    void uninitialized_Cube(void);
    void toggleFullScreen(void);
    int initialize(void);
    void resize_Cube(int , int);
    void draw(void);
    void update_Cube(void);

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
        uninitialized_Cube();
        exit(1);
    }
    // get default screen from above display

    defaultScreen = XDefaultScreen(display);



    glxFBConfigs = glXChooseFBConfig(display , XDefaultScreen(display) , frameBufferAttributes , &numFBConfigs);
    if(glxFBConfigs == NULL)
    {
        fprintf(gpFile , "matching glxFBcongigs files cannot be found \n");
        uninitialized_Cube();
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
        uninitialized_Cube();
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

    XStoreName(display , window , "Vaishnavi bhave");

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
                resize_Cube(event.xconfigure.width , event.xconfigure.height);
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
                XLookupString(&event.xkey , keys , sizeof(keys) , NULL , NULL);
                switch(keys[0])
                {
                    case 'F':
                    case 'f':
                        if (bLightingEnabled_pf == False)
                        {
                            bLightingEnabled_pf = True;
                        }
                        else
                        {
                            bLightingEnabled_pf = False;
                        }
                        break;

                    case 'V':
                    case 'v':
                        if (bLightingEnabled_pv == False)
                        {
                            bLightingEnabled_pv = True;
                        }
                        else
                        {
                            bLightingEnabled_pv = False;
                        }
                        break;

                    case 'L':
                    case 'l':

                        if (bLightingEnabled == False)
                        {
                            bLightingEnabled = True;
                        }
                        else
                        {
                            bLightingEnabled = False;
                        }
                        break;
                    case 'Q':
                    case 'q':
                        bDone = True;
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

            update_Cube();
        }
    }

    uninitialized_Cube();

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

	void resize_Cube(int, int);
	void printGLInfo(void);
	void uninitialized_Cube(void);
	Bool initialize_Sphere(GLuint , GLuint);
	Bool createFBO(GLint , GLint );

    Bool loadGlTexture(GLuint * , const char* );

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
        uninitialized_Cube();
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
            uninitialized_Cube();
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

    // initialize glew
    if(glewInit() != GLEW_OK)
    {
        fprintf(gpFile , "glewinit failed");
        return(-6);
    }

    // printGLInfo

	printGLInfo();

	// vertex shader

	// mat4 used in shader is inbuild in GLSL


	// vertex shader

	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 aPosition;" \
		"uniform mat4 uMVPMatrix;" \
		"in vec2 aTexCoord;" \
		"out vec2 oTexCoord;" \
		"void main(void)" \
		"{" \
		"gl_Position = uMVPMatrix * aPosition;" \
		"oTexCoord = aTexCoord;" \
		"}";

	// create vertex shader object

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	glCompileShader(vertexShaderObject);

	// check for vertex shader compilation error if any

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == False)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Error Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}	
		}
		uninitialized_Cube();
	}

	// fragment shader

	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec2 oTexCoord;" \
		"uniform sampler2D uTextureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(uTextureSampler , oTexCoord);" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	
	glCompileShader(fragmentShaderObject);

	// for fragment shader

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == False)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Error Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialized_Cube();
	}

	// create shader program

	shaderProgramObjectCube = glCreateProgram();

	glAttachShader(shaderProgramObjectCube, vertexShaderObject);
	glAttachShader(shaderProgramObjectCube, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObjectCube, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObjectCube, AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

	glLinkProgram(shaderProgramObjectCube);
	
	// for shader compilation

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObjectCube, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObjectCube, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObjectCube, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialized_Cube();
	}

	// get shader uniform location

	mvpMatrixUniformCube = glGetUniformLocation(shaderProgramObjectCube, "uMVPMatrix");
	textureSamplerUniformCube = glGetUniformLocation(shaderProgramObjectCube, "uTextureSampler");

	// declare position and color arrays

	// cube

	// position
	GLfloat cubeVertices[] =
	{
		// front
		 1.0f,  1.0f,  1.0f, // top-right of front
		-1.0f,  1.0f,  1.0f, // top-left of front
		-1.0f, -1.0f,  1.0f, // bottom-left of front
		 1.0f, -1.0f,  1.0f, // bottom-right of front

		 // right
		  1.0f,  1.0f, -1.0f, // top-right of right
		  1.0f,  1.0f,  1.0f, // top-left of right
		  1.0f, -1.0f,  1.0f, // bottom-left of right
		  1.0f, -1.0f, -1.0f, // bottom-right of right

		  // back
		   1.0f,  1.0f, -1.0f, // top-right of back
		  -1.0f,  1.0f, -1.0f, // top-left of back
		  -1.0f, -1.0f, -1.0f, // bottom-left of back
		   1.0f, -1.0f, -1.0f, // bottom-right of back

		   // left
		   -1.0f,  1.0f,  1.0f, // top-right of left
		   -1.0f,  1.0f, -1.0f, // top-left of left
		   -1.0f, -1.0f, -1.0f, // bottom-left of left
		   -1.0f, -1.0f,  1.0f, // bottom-right of left

		   // top
			1.0f,  1.0f, -1.0f, // top-right of top
		   -1.0f,  1.0f, -1.0f, // top-left of top
		   -1.0f,  1.0f,  1.0f, // bottom-left of top
			1.0f,  1.0f,  1.0f, // bottom-right of top

			// bottom
			 1.0f, -1.0f,  1.0f, // top-right of bottom
			-1.0f, -1.0f,  1.0f, // top-left of bottom
			-1.0f, -1.0f, -1.0f, // bottom-left of bottom
			 1.0f, -1.0f, -1.0f, // bottom-right of bottom
	};

	GLfloat cubeColors[] =
	{
		// front
		1.0f, 0.0f, 0.0f, // top-right of front
		1.0f, 0.0f, 0.0f, // top-left of front
		1.0f, 0.0f, 0.0f, // bottom-left of front
		1.0f, 0.0f, 0.0f, // bottom-right of front

		// right
		0.0f, 0.0f, 1.0f, // top-right of right
		0.0f, 0.0f, 1.0f, // top-left of right
		0.0f, 0.0f, 1.0f, // bottom-left of right
		0.0f, 0.0f, 1.0f, // bottom-right of right

		// back
		1.0f, 1.0f, 0.0f, // top-right of back
		1.0f, 1.0f, 0.0f, // top-left of back
		1.0f, 1.0f, 0.0f, // bottom-left of back
		1.0f, 1.0f, 0.0f, // bottom-right of back

		// left
		1.0f, 0.0f, 1.0f, // top-right of left
		1.0f, 0.0f, 1.0f, // top-left of left
		1.0f, 0.0f, 1.0f, // bottom-left of left
		1.0f, 0.0f, 1.0f, // bottom-right of left

		// top
		0.0f, 1.0f, 0.0f, // top-right of top
		0.0f, 1.0f, 0.0f, // top-left of top
		0.0f, 1.0f, 0.0f, // bottom-left of top
		0.0f, 1.0f, 0.0f, // bottom-right of top

		// bottom
		1.0f, 0.5f, 0.0f, // top-right of bottom
		1.0f, 0.5f, 0.0f, // top-left of bottom
		1.0f, 0.5f, 0.0f, // bottom-left of bottom
		1.0f, 0.5f, 0.0f, // bottom-right of bottom
	};

	GLfloat cubeTexcoords[] =
	{
		// front
		1.0f, 1.0f, // top-right of front
		0.0f, 1.0f, // top-left of front
		0.0f, 0.0f, // bottom-left of front
		1.0f, 0.0f, // bottom-right of front

		// right
		1.0f, 1.0f, // top-right of right
		0.0f, 1.0f, // top-left of right
		0.0f, 0.0f, // bottom-left of right
		1.0f, 0.0f, // bottom-right of right

		// back
		1.0f, 1.0f, // top-right of back
		0.0f, 1.0f, // top-left of back
		0.0f, 0.0f, // bottom-left of back
		1.0f, 0.0f, // bottom-right of back

		// left
		1.0f, 1.0f, // top-right of left
		0.0f, 1.0f, // top-left of left
		0.0f, 0.0f, // bottom-left of left
		1.0f, 0.0f, // bottom-right of left

		// top
		1.0f, 1.0f, // top-right of top
		0.0f, 1.0f, // top-left of top
		0.0f, 0.0f, // bottom-left of top
		1.0f, 0.0f, // bottom-right of top

		// bottom
		1.0f, 1.0f, // top-right of bottom
		0.0f, 1.0f, // top-left of bottom
		0.0f, 0.0f, // bottom-left of bottom
		1.0f, 0.0f, // bottom-right of bottom
	};

	// cube 

	{
		// VAO = vertex array object

		glGenVertexArrays(1, &vao_Cube);

		glBindVertexArray(vao_Cube);

		// VBO for position

		glGenBuffers(1, &vbo_position_cube);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);

		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//// VBO for color
		//glGenBuffers(1, &vbo_color_cube);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo_color_cube);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);
		//glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VBO for texCoord

		glGenBuffers(1, &vbo_Texcoord_cube);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_Texcoord_cube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// unbind with vao
		glBindVertexArray(0);
	}



    if(loadGlTexture(&texture_kundali , "Vijay_Kundali.bmp") == False)
    {
        printf("failed to load texture \n");
        return -3;
    }

    glEnable(GL_TEXTURE_2D);

    if (createFBO(FBO_WIDTH, FBO_HEIGHT) == True)
	{
		vFboResult = initialize_Sphere(FBO_WIDTH, FBO_HEIGHT);
	}

	// enabling depth 

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....

	// initialise orthographicProjectionMatrix

	perspectiveProjectionMatrixCube = vmath::mat4::identity();


	resize_Cube(WINWIDTH, WINHEIGHT);

	return 0;  

}

Bool initialize_Sphere(GLuint texture_width , GLuint texture_height)
{
	// Function Declarations

	void resize_Sphere(int, int);
	void uninitialized_Sphere(void);

	// Code
	
	// for per-vertex
	{
		// vertex shader

		const GLchar* vertexShaderSourceCode_pv =
			"#version 460 core" \
			"\n" \
			"in vec4 aPosition;" \
			"in vec3 aNormal;" \
			"uniform mat4 uModelMatrix;" \
			"uniform mat4 uViewMatrix;" \
			"uniform mat4 uProjectionMatrix;" \
			"uniform vec3 uLightAmbient[3];" \
			"uniform vec3 uLightDiffuse[3];" \
			"uniform vec3 uLightSpecular[3];" \
			"uniform vec4 uLightposition[3];" \
			"uniform vec3 uMaterialAmbient;" \
			"uniform vec3 uMaterialDiffuse;" \
			"uniform vec3 uMaterialSpecular;" \
			"uniform float uMaterialShineness;" \
			"uniform int uKeyPress;" \
			"out vec3 oFong_ADS_Light;" \
			"in vec4 aColor;" \
			"out vec4 oColor;" \
			"void main(void)" \
			"{" \
			"oFong_ADS_Light = vec3(0.0f , 0.0f , 0.0f);" \
			"if(uKeyPress == 1)" \
			"{" \
			"vec4 eyeCoordinates =  uViewMatrix * uModelMatrix * aPosition;" \
			"vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);" \
			"vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
			"vec3 ambientLight[3];" \
			"vec3 diffuseLight[3];" \
			"vec3 specularLight[3]; " \
			"vec3 lightDirection[3];" \
			"vec3 reflectionVector[3];" \
			"for(int i = 0 ; i <3;i++)" \
			"{" \
			"ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;" \
			"lightDirection[i] = normalize(vec3(uLightposition[i] - eyeCoordinates));" \
			"diffuseLight[i] = uLightDiffuse[i] * uMaterialDiffuse * max(dot(lightDirection[i] , transformedNormals) , 0.0f); " \
			"reflectionVector[i] = reflect(-lightDirection[i] , transformedNormals);" \
			"specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i] , viewerVector) , 0.0f) , uMaterialShineness); " \
			"oFong_ADS_Light = oFong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" \
			"}" \
			"}" \
			"else" \
			"{" \
			"oFong_ADS_Light = vec3(0.0f , 0.0f , 0.0f);" \
			"}" \
			"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" \
			"}";

		// create vertex shader object

		GLuint vertexShaderObject_pv = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderObject_pv, 1, (const GLchar**)&vertexShaderSourceCode_pv, NULL);
		glCompileShader(vertexShaderObject_pv);

		// check for vertex shader compilation error if any

		GLint status = 0;
		GLint infoLogLength = 0;
		GLchar* szInfoLog = NULL;

		glGetShaderiv(vertexShaderObject_pv, GL_COMPILE_STATUS, &status);
		if (status == False)
		{
			glGetShaderiv(vertexShaderObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(vertexShaderObject_pv, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Vertex Shader Compilation Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_Sphere();
		}

		// fragment shader

		const GLchar* fragmentShaderSourceCode_pv =
			"#version 460 core" \
			"\n" \
			"in vec4 oColor;" \
			"in vec3 oFong_ADS_Light;" \
			"out vec4 FragColor;" \
			"uniform int uKeyPress;" \
			"void main(void)" \
			"{" \
			"if(uKeyPress == 1)" \
			"{" \
			"FragColor = vec4(oFong_ADS_Light , 1.0f);" \
			"}" \
			"else" \
			"{" \
			"FragColor = vec4(1.0f , 1.0f , 1.0f , 1.0f);" \
			"}" \
			"}";

		GLuint fragmentShaderObject_pv = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShaderObject_pv, 1, (const GLchar**)&fragmentShaderSourceCode_pv, NULL);
		glCompileShader(fragmentShaderObject_pv);

		// for fragment shader

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetShaderiv(fragmentShaderObject_pv, GL_COMPILE_STATUS, &status);
		if (status == False)
		{
			glGetShaderiv(fragmentShaderObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(fragmentShaderObject_pv, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Fragment Shader Compilation Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_Sphere();
		}

		// create shader program

		shaderProgramObject_pv_Sphere = glCreateProgram();

		glAttachShader(shaderProgramObject_pv_Sphere, vertexShaderObject_pv);
		glAttachShader(shaderProgramObject_pv_Sphere, fragmentShaderObject_pv);

		glBindAttribLocation(shaderProgramObject_pv_Sphere, AMC_ATTRIBUTE_POSITION, "aPosition");
		glBindAttribLocation(shaderProgramObject_pv_Sphere, AMC_ATTRIBUTE_NORMALS, "aNormal");

		glLinkProgram(shaderProgramObject_pv_Sphere);

		// for shader compilation

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(shaderProgramObject_pv_Sphere, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			glGetProgramiv(shaderProgramObject_pv_Sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetProgramInfoLog(shaderProgramObject_pv_Sphere, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_Sphere();
		}

		// get shader uniform location

		modelMatrixUniform_Sphere = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uModelMatrix");
		viewMatrixUniform_Sphere = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uViewMatrix");
		projectionMatrixUniform_Sphere = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uProjectionMatrix");

		lightAmbientUniform_Sphere[0] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightAmbient[0]");
		lightDiffuseUniform_Sphere[0] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightDiffuse[0]");
		lightSpecularUniform_Sphere[0] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightSpecular[0]");
		lightPositionUniform_Sphere[0] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightposition[0]");

		lightAmbientUniform_Sphere[1] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightAmbient[1]");
		lightDiffuseUniform_Sphere[1] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightDiffuse[1]");
		lightSpecularUniform_Sphere[1] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightSpecular[1]");
		lightPositionUniform_Sphere[1] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightposition[1]");

		lightAmbientUniform_Sphere[2] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightAmbient[2]");
		lightDiffuseUniform_Sphere[2] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightDiffuse[2]");
		lightSpecularUniform_Sphere[2] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightSpecular[2]");
		lightPositionUniform_Sphere[2] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightposition[2]");

		materialAmbientUniform_Sphere = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uMaterialAmbient");
		materialDiffuseUniform_Sphere = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uMaterialDiffuse");
		materialSpecularUniform_Sphere = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uMaterialSpecular");
		materialShinenessUniform_Sphere = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uMaterialShineness");
		keyPressUniform_Sphere = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uKeyPress");

	}

	// for per-fragment

	{
		// vertex shader

		const GLchar* vertexShaderSourceCode_pf =
			"#version 460 core" \
			"\n" \
			"in vec4 aPosition;" \
			"in vec3 aNormal;" \
			"uniform mat4 uModelMatrix;" \
			"uniform mat4 uViewMatrix;" \
			"uniform mat4 uProjectionMatrix;" \
			"uniform int uKeyPress;" \
			"out vec3 otransformedNormals;" \
			"out vec3 oviewerVector;" \
			"out vec4 oeyeCoordinates;" \
			"in vec4 aColor;" \
			"out vec4 oColor;" \
			"void main(void)" \
			"{" \
			"if(uKeyPress == 1)" \
			"{" \
			"oeyeCoordinates =  uViewMatrix * uModelMatrix * aPosition;" \
			"otransformedNormals = (mat3(uViewMatrix * uModelMatrix) * aNormal);" \
			"oviewerVector = (-oeyeCoordinates.xyz);" \
			"}" \
			"else" \
			"{" \
			"otransformedNormals = vec3(0.0f , 0.0f , 0.0f);" \
			"oviewerVector = vec3(0.0f , 0.0f , 0.0f);" \
			"}" \
			"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" \
			"}";

		// create vertex shader object

		GLuint vertexShaderObject_pf = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderObject_pf, 1, (const GLchar**)&vertexShaderSourceCode_pf, NULL);
		glCompileShader(vertexShaderObject_pf);

		// check for vertex shader compilation error if any

		GLint status = 0;
		GLint infoLogLength = 0;
		GLchar* szInfoLog = NULL;

		glGetShaderiv(vertexShaderObject_pf, GL_COMPILE_STATUS, &status);
		if (status == False)
		{
			glGetShaderiv(vertexShaderObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(vertexShaderObject_pf, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Vertex Shader Compilation Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_Sphere();
		}

		// fragment shader

		const GLchar* fragmentShaderSourceCode_pf =
			"#version 460 core" \
			"\n" \
			"in vec4 oColor;" \
			"in vec3 otransformedNormals;" \
			"in vec3 oviewerVector;" \
			"in vec4 oeyeCoordinates;" \
			"out vec4 FragColor;" \
			"uniform vec3 uLightAmbient[3];" \
			"uniform vec3 uLightDiffuse[3];" \
			"uniform vec3 uLightSpecular[3];" \
			"uniform vec4 uLightposition[3];" \
			"uniform vec3 uMaterialAmbient;" \
			"uniform vec3 uMaterialDiffuse;" \
			"uniform vec3 uMaterialSpecular;" \
			"uniform float uMaterialShineness;" \
			"uniform int uKeyPress;" \
			"void main(void)" \
			"{" \
			"if(uKeyPress == 1)" \
			"{" \
			"vec3 oFong_ADS_Light;" \
			"vec3 ambientLight[3];" \
			"vec3 diffuseLight[3];" \
			"vec3 specularLight[3]; " \
			"vec3 lightDirection[3];" \
			"vec3 reflectionVector[3];" \
			"vec3 normalizedTranformedNormals = normalize(otransformedNormals);" \
			"vec3 normalizedViewerVector= normalize(oviewerVector);" \
			"for(int i = 0 ; i <3;i++)" \
			"{" \
			"ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;" \
			"lightDirection[i] = normalize(vec3(uLightposition[i] - oeyeCoordinates));" \
			"diffuseLight[i] = uLightDiffuse[i] * uMaterialDiffuse * max(dot(lightDirection[i] , normalizedTranformedNormals) , 0.0f); " \
			"reflectionVector[i] = reflect(-lightDirection[i] , normalizedTranformedNormals);" \
			"specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i] , normalizedViewerVector) , 0.0f) , uMaterialShineness); " \
			"oFong_ADS_Light = oFong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" \
			"}" \
			"FragColor = vec4(oFong_ADS_Light , 1.0f);" \
			"}" \
			"else" \
			"{" \
			"FragColor = vec4(1.0f , 1.0f , 1.0f , 1.0f);" \
			"}" \
			"}";

		GLuint fragmentShaderObject_pf = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShaderObject_pf, 1, (const GLchar**)&fragmentShaderSourceCode_pf, NULL);
		glCompileShader(fragmentShaderObject_pf);

		// for fragment shader

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetShaderiv(fragmentShaderObject_pf, GL_COMPILE_STATUS, &status);
		if (status == False)
		{
			glGetShaderiv(fragmentShaderObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetShaderInfoLog(fragmentShaderObject_pf, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Fragment Shader Compilation Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_Sphere();
		}

		// create shader program

		shaderProgramObject_pf_Sphere = glCreateProgram();

		glAttachShader(shaderProgramObject_pf_Sphere, vertexShaderObject_pf);
		glAttachShader(shaderProgramObject_pf_Sphere, fragmentShaderObject_pf);

		glBindAttribLocation(shaderProgramObject_pf_Sphere, AMC_ATTRIBUTE_POSITION, "aPosition");
		glBindAttribLocation(shaderProgramObject_pf_Sphere, AMC_ATTRIBUTE_NORMALS, "aNormal");

		glLinkProgram(shaderProgramObject_pf_Sphere);

		// for shader compilation

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(shaderProgramObject_pf_Sphere, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			glGetProgramiv(shaderProgramObject_pf_Sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetProgramInfoLog(shaderProgramObject_pf_Sphere, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialized_Sphere();
		}

		// get shader uniform location

		modelMatrixUniform_Sphere = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uModelMatrix");
		viewMatrixUniform_Sphere = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uViewMatrix");
		projectionMatrixUniform_Sphere = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uProjectionMatrix");

		lightAmbientUniform_Sphere[0] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightAmbient[0]");
		lightDiffuseUniform_Sphere[0] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightDiffuse[0]");
		lightSpecularUniform_Sphere[0] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightSpecular[0]");
		lightPositionUniform_Sphere[0] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightposition[0]");

		lightAmbientUniform_Sphere[1] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightAmbient[1]");
		lightDiffuseUniform_Sphere[1] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightDiffuse[1]");
		lightSpecularUniform_Sphere[1] = glGetUniformLocation(shaderProgramObject_pv_Sphere, "uLightSpecular[1]");
		lightPositionUniform_Sphere[1] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightposition[1]");

		lightAmbientUniform_Sphere[2] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightAmbient[2]");
		lightDiffuseUniform_Sphere[2] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightDiffuse[2]");
		lightSpecularUniform_Sphere[2] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightSpecular[2]");
		lightPositionUniform_Sphere[2] = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uLightposition[2]");

		materialAmbientUniform_Sphere = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uMaterialAmbient");
		materialDiffuseUniform_Sphere = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uMaterialDiffuse");
		materialSpecularUniform_Sphere = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uMaterialSpecular");
		materialShinenessUniform_Sphere = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uMaterialShineness");
		keyPressUniform_Sphere = glGetUniformLocation(shaderProgramObject_pf_Sphere, "uKeyPress");
	}
	



	// declare position and color arrays

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// vao

	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// position vbo

	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMALS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// for textures

	glGenBuffers(1, &gVbo_sphere_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_textures), sphere_textures, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// unbind vao
	glBindVertexArray(0);



	// enabling depth 

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	light[0].ambient = vec3(0.0f, 0.0f, 0.0f);
	light[1].ambient = vec3(0.0f, 0.0f, 0.0f);
	light[2].ambient = vec3(0.0f, 0.0f, 0.0f);

	light[0].diffuse = vec3(1.0f, 0.0f, 0.0f);
	light[1].diffuse = vec3(0.0f, 1.0f, 0.0f);
	light[2].diffuse = vec3(0.0f, 0.0f, 1.0f);

	light[0].specular = vec3(1.0f, 0.0f, 0.0f);
	light[1].specular = vec3(0.0f, 1.0f, 0.0f);
	light[2].specular = vec3(0.0f, 0.0f, 1.0f);


	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....

	perspectiveProjectionMatrix_Sphere = vmath::mat4::identity();


	resize_Sphere(WINWIDTH, WINHEIGHT);

	return True;
}
// FBO releted code
Bool createFBO(GLint textureWidth, GLint textureHeight)
{
	GLint maxRenderBufferSize;
	// check capacity of render buffer

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);

	if (maxRenderBufferSize < textureWidth || maxRenderBufferSize < textureHeight)
	{
		fprintf(gpFile, "Texture size overflow \n");
		return False;
	}

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// create texture for FBO in which we are going to render the sphere

	glGenTextures(1, &texture_FBO);
	glBindTexture(GL_TEXTURE_2D, texture_FBO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

	// attach above texture to frame buffer at color attachment 0

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_FBO, 0);

	// now create render buffer to hold depth of custom fbo

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	// set storage of render buffer of texture size

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

	// attach above depth related render buffer to FBO at depth attachment

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// check the frame buffer status , wether success or not

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "framebuffer create not complete \n");
		return False;
	}

	// unbind framebuffer

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return True;
}



Bool loadGlTexture(GLuint *texture , const char* path)
{
    unsigned char* imageData = NULL;
    int width, height , numberOfChannels; // numberOfChannels = RGBA sathi


    imageData = SOIL_load_image(path , &width , &height , NULL , SOIL_LOAD_RGB);
    if(imageData == NULL)
    {
        printf("image loading failed");
        return False;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT , 4);
    glGenTextures(1 , texture);

    glBindTexture(GL_TEXTURE_2D , *texture);

    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR_MIPMAP_LINEAR);

    // gluBuild2DMipmaps(GL_TEXTURE_2D , 3 , width , height , GL_RGB , GL_UNSIGNED_BYTE , imageData);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB , GL_UNSIGNED_BYTE, imageData);

	glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(imageData);

    // glBindTexture(GL_TEXTURE_2D , 0);

    imageData = NULL;

    return True;
}

void printGLInfo(void)
{
	// variable declaration
	GLint numExtentions =0;
	GLint i;

	// code

	fprintf(gpFile, "OpenGL Vendor : %s \n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s \n", glGetString(GL_VERSION));
	fprintf(gpFile, "OpenGL GLSL Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	fprintf(gpFile, "****************************************************************************************** \n");
	// listiong of supported extentions

	fprintf(gpFile, "Number of EXT. : %d \n", numExtentions);

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtentions);

	for (i = 0; i < numExtentions; i++)
	{
		fprintf(gpFile, "%s \n", glGetStringi(GL_EXTENSIONS , i));
	}

	fprintf(gpFile, "****************************************************************************************** \n");


}

void resize_Cube(int width, int height)
{
	// code
	if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	winWidth = width;
	winHeight = height;

	perspectiveProjectionMatrixCube = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

}

void resize_Sphere(int width, int height)
{
	// code
	if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix_Sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

}


void draw(void)
{
    void display_Sphere(GLint , GLint);
	void update_Sphere(void);
	void resize_Sphere(int, int);

	// render FBO scene

	if (vFboResult == True)
	{
		display_Sphere(FBO_WIDTH, FBO_HEIGHT);
		update_Sphere();
		
	}

	// call resize cube again to compensate the change done by display sphere

	resize_Cube(winWidth , winHeight);
	
	// reset the color to white to compemsate the change done by display sphere

	glClearColor(1.0f , 1.0f , 1.0f , 1.0f);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObjectCube);

	// cube
	{

		// transformation

		mat4 modelViewMatrix = mat4::identity();

		mat4 translationMatrix = mat4::identity();
		translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

		mat4 rotationMatrix1 = mat4::identity();
		rotationMatrix1 = vmath::rotate(angle_cube, 1.0f, 0.0f, 0.0f);

		mat4 rotationMatrix2 = mat4::identity();
		rotationMatrix2 = vmath::rotate(angle_cube, 0.0f, 1.0f, 0.0f);

		mat4 rotationMatrix3 = mat4::identity();
		rotationMatrix3 = vmath::rotate(angle_cube, 0.0f, 0.0f, 1.0f);

		mat4 rotationMatrix = rotationMatrix1 * rotationMatrix2 * rotationMatrix3;


		modelViewMatrix = translationMatrix * rotationMatrix ;

		mat4 modelViewProjectionMatrix = mat4::identity();
		modelViewProjectionMatrix = perspectiveProjectionMatrixCube * modelViewMatrix; // order of multiplication is very important

		// now push modelViewProjectionMatrix in the "uMVPMatrix" of shader , per frame.

		// push above mvp into vertex shader mvpMatrixUniform

		glUniformMatrix4fv(mvpMatrixUniformCube, 1, GL_FALSE, modelViewProjectionMatrix);

		//for texture

		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture_kundali);
		glBindTexture(GL_TEXTURE_2D, texture_FBO);
		glUniform1i(textureSamplerUniformCube, 0);

		glBindVertexArray(vao_Cube);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	glUseProgram(0);


    glXSwapBuffers(display , window);
}

void display_Sphere(GLint textureWidth , GLint textureHeight)
{
	void resize_Sphere(int, int);
	// bind with FBO

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	// call resize sphere

	resize_Sphere(textureWidth , textureHeight);

	// set the clear color to black to compensate the change done by display sphere

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (bLightingEnabled_pv == True)
	{
		glUseProgram(shaderProgramObject_pv_Sphere);
	}
	else if (bLightingEnabled_pf == True)
	{
		glUseProgram(shaderProgramObject_pf_Sphere);
	}
	else
	{
		glUseProgram(shaderProgramObject_pv_Sphere);
	}

	// sphere
	{

		// transformation

		mat4 modelMatrix = mat4::identity();
		mat4 ViewMatrix = mat4::identity();

		mat4 translationMatrix = mat4::identity();
		translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);

		mat4 scaleMatrix = mat4::identity();
		scaleMatrix = vmath::scale(2.0f, 2.0f, 2.0f);

		modelMatrix = translationMatrix * scaleMatrix;


		// now push modelViewProjectionMatrix in the "uMVPMatrix" of shader , per frame.

		// push above mvp into vertex shader mvpMatrixUniform

		glUniformMatrix4fv(modelMatrixUniform_Sphere, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform_Sphere, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform_Sphere, 1, GL_FALSE, perspectiveProjectionMatrix_Sphere);

		if (bLightingEnabled == True)
		{
			glUniform1i(keyPressUniform_Sphere, 1);
			glUniform3fv(lightAmbientUniform_Sphere[0], 1, light[0].ambient);
			glUniform3fv(lightDiffuseUniform_Sphere[0], 1, light[0].diffuse);
			glUniform3fv(lightSpecularUniform_Sphere[0], 1, light[0].specular);

			glUniform3fv(lightAmbientUniform_Sphere[1], 1, light[1].ambient);
			glUniform3fv(lightDiffuseUniform_Sphere[1], 1, light[1].diffuse);
			glUniform3fv(lightSpecularUniform_Sphere[1], 1, light[1].specular);

			glUniform3fv(lightAmbientUniform_Sphere[2], 1, light[2].ambient);
			glUniform3fv(lightDiffuseUniform_Sphere[2], 1, light[2].diffuse);
			glUniform3fv(lightSpecularUniform_Sphere[2], 1, light[2].specular);

			glUniform3fv(materialAmbientUniform_Sphere, 1, materialAmbient);
			glUniform3fv(materialDiffuseUniform_Sphere, 1, materialDiffuse);
			glUniform3fv(materialSpecularUniform_Sphere, 1, materialSpecular);
			glUniform1f(materialShinenessUniform_Sphere, materialShineness);

			glUniform4fv(lightPositionUniform_Sphere[0], 1, light[0].position);
			glUniform4fv(lightPositionUniform_Sphere[1], 1, light[1].position);
			glUniform4fv(lightPositionUniform_Sphere[2], 1, light[2].position);
		}
		else
		{
			glUniform1i(keyPressUniform_Sphere, 0);
		}





		// *** bind vao ***
		glBindVertexArray(gVao_sphere);

		// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

		// *** unbind vao ***
		glBindVertexArray(0);

	}

	glUseProgram(0);

	// unbind the framebuffer

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}


void update_Cube(void)
{
	// code

	angle_cube -= 1.5f;
	if (angle_cube <= 0.0f)
	{
		angle_cube += 360.0f;
	}

}

void update_Sphere(void)
{
	// code
	if (bLightingEnabled == True)
	{
		light[0].position[0] = 0.0f;
		light[0].position[1] = 5.0f * (GLfloat)sin(lightAngleZero);
		light[0].position[2] = 5.0f * (GLfloat)cos(lightAngleZero);
		light[0].position[3] = 1.0f;


		light[1].position[0] = 5.0f * (GLfloat)cos(lightAngleOne);
		light[1].position[1] = 0.0f;
		light[1].position[2] = 5.0f * (GLfloat)sin(lightAngleOne);
		light[1].position[3] = 1.0f;


		light[2].position[0] = 5.0f * (GLfloat)cos(lightAngleTwo);
		light[2].position[1] = 5.0f * (GLfloat)sin(lightAngleTwo);
		light[2].position[2] = 0.0f;
		light[2].position[3] = 1.0f;

	}

	lightAngleZero = lightAngleZero + 0.05f;
	if (lightAngleZero >= (360.0f))
	{
		lightAngleZero = lightAngleZero - (360.0f);
	}

	lightAngleOne = lightAngleOne + 0.05f;
	if (lightAngleOne >= (360.0f))
	{
		lightAngleOne = lightAngleOne - (360.0f);
	}

	lightAngleTwo = lightAngleTwo + 0.05f;
	if (lightAngleTwo >= (360.0f))
	{
		lightAngleTwo = lightAngleTwo - (360.0f);
	}

}


void uninitialized_Cube(void)
{
    // code

    	fprintf(gpFile, "****************************************************************************************** \n");
	// code

	void uninitialized_Sphere(void);


	//  function declarations
	fprintf(gpFile, "****************************************************************************************** \n");
	// code

	if (shaderProgramObjectCube)
	{
		glUseProgram(shaderProgramObjectCube);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObjectCube, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObjectCube, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObjectCube, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObjectCube);
		shaderProgramObjectCube = 0;
	}

	// for FBO 
	{

		// delete VBO of position

		if (texture_FBO)
		{
			glDeleteTextures(1, &texture_FBO);
			texture_FBO = 0;
		}

		if (rbo)
		{
			glDeleteRenderbuffers(1, &rbo);
			rbo = 0;
		}

		// delete VAO

		if (fbo)
		{
			glDeleteFramebuffers(1, &fbo);
			fbo = 0;
		}
	}

	// for square 
	{

		// delete VBO of position

		if (vbo_Texcoord_cube)
		{
			glDeleteBuffers(1, &vbo_Texcoord_cube);
			vbo_Texcoord_cube = 0;
		}

		if (vbo_position_cube)
		{
			glDeleteBuffers(1, &vbo_position_cube);
			vbo_position_cube = 0;
		}

		// delete VAO

		if (vao_Cube)
		{
			glDeleteVertexArrays(1, &vao_Cube);
			vao_Cube = 0;
		}
	}

    uninitialized_Sphere();

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


void uninitialized_Sphere(void)
{
	//  function declarations
	fprintf(gpFile, "****************************************************************************************** \n");
	// code

	if (shaderProgramObject_pf_Sphere)
	{
		glUseProgram(shaderProgramObject_pf_Sphere);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_pf_Sphere, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_pf_Sphere, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_pf_Sphere, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_pf_Sphere);
		shaderProgramObject_pf_Sphere = 0;
	}

	if (shaderProgramObject_pv_Sphere)
	{
		glUseProgram(shaderProgramObject_pv_Sphere);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_pv_Sphere, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_pv_Sphere, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_pv_Sphere, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_pv_Sphere);
		shaderProgramObject_pv_Sphere = 0;
	}

	// for sphere 
	{

		// delete VBO of position

		if (gVbo_sphere_normal)
		{
			glDeleteBuffers(1, &gVbo_sphere_normal);
			gVbo_sphere_normal = 0;
		}

		if (gVbo_sphere_position)
		{
			glDeleteBuffers(1, &gVbo_sphere_position);
			gVbo_sphere_position = 0;
		}

		// delete VAO

		if (gVao_sphere)
		{
			glDeleteVertexArrays(1, &gVao_sphere);
			gVao_sphere = 0;
		}
	}
}



