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

// header file for vmath
#include "vmath.h"
#include "Sphere.h"

using namespace vmath;

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

GLuint shaderProgramObject_pv = 0;
GLuint shaderProgramObject_pf = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCOORDS,
	AMC_ATTRIBUTE_NORMALS// lights
};

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

GLuint modelMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;
GLuint viewMatrixUniform = 0;

GLuint modelMatrixUniform_pf = 0;
GLuint projectionMatrixUniform_pf = 0;
GLuint viewMatrixUniform_pf = 0;

GLuint lightDiffuseUniform[3]; // for diffuse light
GLuint materialDiffuseUniform = 0; // for diffuse material
GLuint lightAmbientUniform[3]; // for ambient light
GLuint materialAmbientUniform = 0; // for ambient material
GLuint lightSpecularUniform[3]; // for specular light
GLuint materialSpecularUniform = 0; // for specular material
GLuint materialShinenessUniform = 0; // for shinneness material
GLuint lightPositionUniform[3];
GLuint keyPressUniform = 0;

GLuint lightDiffuseUniform_pf[3]; // for diffuse light
GLuint materialDiffuseUniform_pf = 0; // for diffuse material
GLuint lightAmbientUniform_pf[3]; // for ambient light
GLuint materialAmbientUniform_pf = 0; // for ambient material
GLuint lightSpecularUniform_pf[3]; // for specular light
GLuint materialSpecularUniform_pf = 0; // for specular material
GLuint materialShinenessUniform_pf = 0; // for shinneness material
GLuint lightPositionUniform_pf[3];

Bool bLightingEnabled = False;
Bool bLightingEnabled_pv = False;
Bool bLightingEnabled_pf = False;

GLfloat materialAmbient[] = { 0.0f , 0.0f , 0.0f , 1.0f };
GLfloat materialDiffuse[] = { 0.5f , 0.2f , 0.7f , 1.0f };
GLfloat materialSpecular[] = { 0.7f , 0.7f , 0.7f , 1.0f };
GLfloat materialShineness =  50.0f ;

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



mat4 perspectiveProjectionMatrix; // mat4 datatype is in vmath.h header file, that means 4X4 matrix

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
    void printGLInfo(void);

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

    // initialize glew
    if(glewInit() != GLEW_OK)
    {
        fprintf(gpFile , "glewinit failed");
        return(-6);
    }

    // printGLInfo

	printGLInfo();

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
			uninitialize();
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
			uninitialize();
		}

		// create shader program

		shaderProgramObject_pv = glCreateProgram();

		glAttachShader(shaderProgramObject_pv, vertexShaderObject_pv);
		glAttachShader(shaderProgramObject_pv, fragmentShaderObject_pv);

		glBindAttribLocation(shaderProgramObject_pv, AMC_ATTRIBUTE_POSITION, "aPosition");
		glBindAttribLocation(shaderProgramObject_pv, AMC_ATTRIBUTE_NORMALS, "aNormal");

		glLinkProgram(shaderProgramObject_pv);

		// for shader compilation

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(shaderProgramObject_pv, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			glGetProgramiv(shaderProgramObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetProgramInfoLog(shaderProgramObject_pv, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialize();
		}

		// get shader uniform location

		modelMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "uModelMatrix");
		viewMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "uViewMatrix");
		projectionMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "uProjectionMatrix");

		lightAmbientUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "uLightAmbient[0]");
		lightDiffuseUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "uLightDiffuse[0]");
		lightSpecularUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular[0]");
		lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "uLightposition[0]");

		lightAmbientUniform[1] = glGetUniformLocation(shaderProgramObject_pv, "uLightAmbient[1]");
		lightDiffuseUniform[1] = glGetUniformLocation(shaderProgramObject_pv, "uLightDiffuse[1]");
		lightSpecularUniform[1] = glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular[1]");
		lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject_pv, "uLightposition[1]");

		lightAmbientUniform[2] = glGetUniformLocation(shaderProgramObject_pv, "uLightAmbient[2]");
		lightDiffuseUniform[2] = glGetUniformLocation(shaderProgramObject_pv, "uLightDiffuse[2]");
		lightSpecularUniform[2] = glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular[2]");
		lightPositionUniform[2] = glGetUniformLocation(shaderProgramObject_pv, "uLightposition[2]");

		materialAmbientUniform = glGetUniformLocation(shaderProgramObject_pv, "uMaterialAmbient");
		materialDiffuseUniform = glGetUniformLocation(shaderProgramObject_pv, "uMaterialDiffuse");
		materialSpecularUniform = glGetUniformLocation(shaderProgramObject_pv, "uMaterialSpecular");
		materialShinenessUniform = glGetUniformLocation(shaderProgramObject_pv, "uMaterialShineness");
		keyPressUniform = glGetUniformLocation(shaderProgramObject_pv, "uKeyPress");

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
			uninitialize();
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
			uninitialize();
		}

		// create shader program

		shaderProgramObject_pf = glCreateProgram();

		glAttachShader(shaderProgramObject_pf, vertexShaderObject_pf);
		glAttachShader(shaderProgramObject_pf, fragmentShaderObject_pf);

		glBindAttribLocation(shaderProgramObject_pf, AMC_ATTRIBUTE_POSITION, "aPosition");
		glBindAttribLocation(shaderProgramObject_pf, AMC_ATTRIBUTE_NORMALS, "aNormal");

		glLinkProgram(shaderProgramObject_pf);

		// for shader compilation

		status = 0;
		infoLogLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(shaderProgramObject_pf, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			glGetProgramiv(shaderProgramObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(infoLogLength);
				if (szInfoLog != NULL)
				{
					glGetProgramInfoLog(shaderProgramObject_pf, infoLogLength, NULL, szInfoLog);
					fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
					free(szInfoLog);
					szInfoLog = NULL;
				}
			}
			uninitialize();
		}

		// get shader uniform location

		modelMatrixUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uModelMatrix");
		viewMatrixUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uViewMatrix");
		projectionMatrixUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uProjectionMatrix");

		lightAmbientUniform_pf[0] = glGetUniformLocation(shaderProgramObject_pf, "uLightAmbient[0]");
		lightDiffuseUniform_pf[0] = glGetUniformLocation(shaderProgramObject_pf, "uLightDiffuse[0]");
		lightSpecularUniform_pf[0] = glGetUniformLocation(shaderProgramObject_pf, "uLightSpecular[0]");
		lightPositionUniform_pf[0] = glGetUniformLocation(shaderProgramObject_pf, "uLightposition[0]");

		lightAmbientUniform_pf[1] = glGetUniformLocation(shaderProgramObject_pf, "uLightAmbient[1]");
		lightDiffuseUniform_pf[1] = glGetUniformLocation(shaderProgramObject_pf, "uLightDiffuse[1]");
		lightSpecularUniform_pf[1] = glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular[1]");
		lightPositionUniform_pf[1] = glGetUniformLocation(shaderProgramObject_pf, "uLightposition[1]");

		lightAmbientUniform_pf[2] = glGetUniformLocation(shaderProgramObject_pf, "uLightAmbient[2]");
		lightDiffuseUniform_pf[2] = glGetUniformLocation(shaderProgramObject_pf, "uLightDiffuse[2]");
		lightSpecularUniform_pf[2] = glGetUniformLocation(shaderProgramObject_pf, "uLightSpecular[2]");
		lightPositionUniform_pf[2] = glGetUniformLocation(shaderProgramObject_pf, "uLightposition[2]");

		materialAmbientUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uMaterialAmbient");
		materialDiffuseUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uMaterialDiffuse");
		materialSpecularUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uMaterialSpecular");
		materialShinenessUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uMaterialShineness");
		keyPressUniform = glGetUniformLocation(shaderProgramObject_pf, "uKeyPress");
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
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORDS);
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

	perspectiveProjectionMatrix = vmath::mat4::identity();



	resize(WINWIDTH, WINHEIGHT);

	return 0;  

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

void resize(int width, int height)
{
	// code
	if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// set perspective projection matrix

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);
	
}


void draw(void)
{
// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (bLightingEnabled_pv == True)
	{
		glUseProgram(shaderProgramObject_pv);
	}
	else if (bLightingEnabled_pf == True)
	{
		glUseProgram(shaderProgramObject_pf);
	}
	else
	{
		glUseProgram(shaderProgramObject_pv);
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



		if (bLightingEnabled == True)
		{
			if (bLightingEnabled_pv == True)
			{
				glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
				glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, ViewMatrix);
				glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

				glUniform1i(keyPressUniform, 1);
				glUniform3fv(lightAmbientUniform[0], 1, light[0].ambient);
				glUniform3fv(lightDiffuseUniform[0], 1, light[0].diffuse);
				glUniform3fv(lightSpecularUniform[0], 1, light[0].specular);

				glUniform3fv(lightAmbientUniform[1], 1, light[1].ambient);
				glUniform3fv(lightDiffuseUniform[1], 1, light[1].diffuse);
				glUniform3fv(lightSpecularUniform[1], 1, light[1].specular);

				glUniform3fv(lightAmbientUniform[2], 1, light[2].ambient);
				glUniform3fv(lightDiffuseUniform[2], 1, light[2].diffuse);
				glUniform3fv(lightSpecularUniform[2], 1, light[2].specular);

				glUniform3fv(materialAmbientUniform, 1, materialAmbient);
				glUniform3fv(materialDiffuseUniform, 1, materialDiffuse);
				glUniform3fv(materialSpecularUniform, 1, materialSpecular);
				glUniform1f(materialShinenessUniform, materialShineness);

				glUniform4fv(lightPositionUniform[0], 1, light[0].position);
				glUniform4fv(lightPositionUniform[1], 1, light[1].position);
				glUniform4fv(lightPositionUniform[2], 1, light[2].position);
			}

			if (bLightingEnabled_pf == True)
			{
				glUniformMatrix4fv(modelMatrixUniform_pf, 1, GL_FALSE, modelMatrix);
				glUniformMatrix4fv(viewMatrixUniform_pf, 1, GL_FALSE, ViewMatrix);
				glUniformMatrix4fv(projectionMatrixUniform_pf, 1, GL_FALSE, perspectiveProjectionMatrix);

				glUniform1i(keyPressUniform, 1);
				glUniform3fv(lightAmbientUniform_pf[0], 1, light[0].ambient);
				glUniform3fv(lightDiffuseUniform_pf[0], 1, light[0].diffuse);
				glUniform3fv(lightSpecularUniform_pf[0], 1, light[0].specular);

				glUniform3fv(lightAmbientUniform_pf[1], 1, light[1].ambient);
				glUniform3fv(lightDiffuseUniform_pf[1], 1, light[1].diffuse);
				glUniform3fv(lightSpecularUniform_pf[1], 1, light[1].specular);

				glUniform3fv(lightAmbientUniform_pf[2], 1, light[2].ambient);
				glUniform3fv(lightDiffuseUniform_pf[2], 1, light[2].diffuse);
				glUniform3fv(lightSpecularUniform_pf[2], 1, light[2].specular);

				glUniform3fv(materialAmbientUniform_pf, 1, materialAmbient);
				glUniform3fv(materialDiffuseUniform_pf, 1, materialDiffuse);
				glUniform3fv(materialSpecularUniform_pf, 1, materialSpecular);
				glUniform1f(materialShinenessUniform_pf, materialShineness);

				glUniform4fv(lightPositionUniform_pf[0], 1, light[0].position);
				glUniform4fv(lightPositionUniform_pf[1], 1, light[1].position);
				glUniform4fv(lightPositionUniform_pf[2], 1, light[2].position);
			}
		}
		else
		{
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, ViewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

			glUniform1i(keyPressUniform, 0);
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


    glXSwapBuffers(display , window);
}

void update(void)
{
    // code
		// code
	if (bLightingEnabled == True )
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

void uninitialize(void)
{
    // code

    	fprintf(gpFile, "****************************************************************************************** \n");
	// code
	if (shaderProgramObject_pf)
	{
		glUseProgram(shaderProgramObject_pf);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_pf, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_pf, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_pf, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_pf);
		shaderProgramObject_pf = 0;
	}

	if (shaderProgramObject_pv)
	{
		glUseProgram(shaderProgramObject_pv);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_pv, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_pv, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_pv, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_pv);
		shaderProgramObject_pv = 0;
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


