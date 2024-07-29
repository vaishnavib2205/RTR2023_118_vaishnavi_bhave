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

//  header file for soil

#include<SOIL/SOIL.h>

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

GLuint shaderProgramObject = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCOORD
};

// variables for triangle
GLuint vao_Pyramid = 0;
GLuint vbo_position_Pyramid = 0;
GLuint vbo_texcoord = 0;
GLfloat angle_Pyramid = 0.0f;

GLuint texture_stone = 0;

GLuint mvpMatrixUniform = 0;
GLuint textureSamplerUniform = 0;

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
    void printGLInfo(void);
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
		uninitialize();
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
		uninitialize();
	}

	// create shader program

	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

	glLinkProgram(shaderProgramObject);
	
	// for shader compilation

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	// get shader uniform location

	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");

	// declare position and color arrays

	GLfloat pyramidVertices[] =
	{
		// front
		0.0f,  1.0f,  0.0f, // front-top
	   -1.0f, -1.0f,  1.0f, // front-left
		1.0f, -1.0f,  1.0f, // front-right

		// right
		0.0f,  1.0f,  0.0f, // right-top
		1.0f, -1.0f,  1.0f, // right-left
		1.0f, -1.0f, -1.0f, // right-right

		// back
		0.0f,  1.0f,  0.0f, // back-top
		1.0f, -1.0f, -1.0f, // back-left
	   -1.0f, -1.0f, -1.0f, // back-right

	   // left
	   0.0f,  1.0f,  0.0f, // left-top
	  -1.0f, -1.0f, -1.0f, // left-left
	  -1.0f, -1.0f,  1.0f, // left-right
	};

	// color
	GLfloat pyramidColors[] =
	{
		// front
		1.0f, 0.0f, 0.0f, // front-top
		0.0f, 1.0f, 0.0f, // front-left
		0.0f, 0.0f, 1.0f, // front-right

		// right
		1.0f, 0.0f, 0.0f, // right-top
		0.0f, 0.0f, 1.0f, // right-left
		0.0f, 1.0f, 0.0f, // right-right

		// back
		1.0f, 0.0f, 0.0f, // back-top
		0.0f, 1.0f, 0.0f, // back-left
		0.0f, 0.0f, 1.0f, // back-right

		// left
		1.0f, 0.0f, 0.0f, // left-top
		0.0f, 0.0f, 1.0f, // left-left
		0.0f, 1.0f, 0.0f, // left-right
	};

	// texcoords
	GLfloat pyramidTexcoords[] =
	{
		// front
		0.5, 1.0, // front-top
		0.0, 0.0, // front-left
		1.0, 0.0, // front-right

		// right
		0.5, 1.0, // right-top
		1.0, 0.0, // right-left
		0.0, 0.0, // right-right

		// back
		0.5, 1.0, // back-top
		0.0, 0.0, // back-left
		1.0, 0.0, // back-right

		// left
		0.5, 1.0, // left-top
		1.0, 0.0, // left-left
		0.0, 0.0, // left-right
	};



	// square 

	{
		// VAO = vertex array object

		glGenVertexArrays(1, &vao_Pyramid);
		glBindVertexArray(vao_Pyramid);

		// VBO for position

		glGenBuffers(1, &vbo_position_Pyramid);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position_Pyramid);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VBO for texCoord

		glGenBuffers(1, &vbo_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexcoords), pyramidTexcoords, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// unbind with vao_square
		glBindVertexArray(0);

	}

    if(loadGlTexture(&texture_stone , "Stone.bmp") == False)
    {
        printf("failed to load texture \n");
        return -3;
    }

    glEnable(GL_TEXTURE_2D);

	// enabling depth 

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....

	// initialise orthographicProjectionMatrix

	perspectiveProjectionMatrix = vmath::mat4::identity();


	resize(WINWIDTH, WINHEIGHT);

	return 0;  

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

	glUseProgram(shaderProgramObject);


	{
		// transformation

		mat4 modelViewMatrix = mat4::identity();

		mat4 translationMatrix = mat4::identity();
		translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);

		mat4 rotationMatrix = mat4::identity();
		rotationMatrix = vmath::rotate(angle_Pyramid, 0.0f, 1.0f, 0.0f);

		modelViewMatrix = translationMatrix * rotationMatrix;

		mat4 modelViewProjectionMatrix = mat4::identity();
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order of multiplication is very important

		// now push modelViewProjectionMatrix in the "uMVPMatrix" of shader , per frame.

		// push above mvp into vertex shader mvpMatrixUniform

		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_stone);
		glUniform1i(textureSamplerUniform, 0);

		glBindVertexArray(vao_Pyramid);

		glDrawArrays(GL_TRIANGLES, 0, 12);
		glBindVertexArray(0);

	}	




    glXSwapBuffers(display , window);
}

void update(void)
{
    // code
    
    angle_Pyramid += 1.5f;
	if (angle_Pyramid >= 360.0f)
	{
		angle_Pyramid -= 360.0f;
	}

}

void uninitialize(void)
{
    // code

    	fprintf(gpFile, "****************************************************************************************** \n");
	// code

	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}


	// for Pyramid 
	{
		// delete VBO of position

		if (vbo_texcoord)
		{
			glDeleteBuffers(1, &vbo_texcoord);
			vbo_texcoord = 0;
		}

		// delete VBO of position

		if (vbo_position_Pyramid)
		{
			glDeleteBuffers(1, &vbo_position_Pyramid);
			vbo_position_Pyramid = 0;
		}

		// delete VAO

		if (vao_Pyramid)
		{
			glDeleteVertexArrays(1, &vao_Pyramid);
			vao_Pyramid = 0;
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


