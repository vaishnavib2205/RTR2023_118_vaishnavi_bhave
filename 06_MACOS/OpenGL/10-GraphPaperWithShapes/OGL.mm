#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h> // all Programmable content after OpenGL 3.0 version, so gl3
#import <OpenGL/gl3ext.h>

// header file for vmath
#include "vmath.h"
using namespace vmath;

// MACROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Function declaration

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef , const CVTimeStamp* , const CVTimeStamp* , CVOptionFlags , CVOptionFlags* , void *);



// Global Variable Declaration

FILE *gpFile = NULL;


enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR

};






@interface AppDelegate:NSObject <NSApplicationDelegate , NSWindowDelegate>
@end

@interface GLView:NSOpenGLView
@end

// main function

int main(int argc , char* argv[])
{
    // code

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; // create autorelease pool for mem management

    NSApp = [NSApplication sharedApplication]; // create global,shared application object NSApp
    
    [NSApp setDelegate : [[AppDelegate alloc]init]];// set its delegate to our own custom AppDelegate

    [NSApp run]; // start NSApps run loop

    [pool release];// let auto release pool release all from our applications

}

// implimentation of our custom appdelegate interface

@implementation AppDelegate
{
    NSWindow *window;
    GLView *glView;
}

-(void)applicationDidFinishLaunching:(NSNotification *)notification
{
    // code
    
    // Log File creation/Opening code
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/Log.txt" , parentDirPath];
    const char *pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    
    gpFile = fopen(pszLogFileNameWithPath , "w");
    if(gpFile == NULL)
    {
        NSLog(@"Log File cannot be created");
        [self release];
        [NSApp terminate:self];
    }

    fprintf(gpFile , "Program Started Successfully \n");
    

    // declare rectangle for frame/border of our window
    NSRect win_rect = NSMakeRect(0.0 , 0.0 , WIN_WIDTH , WIN_HEIGHT);
    // create the window
    window = [[NSWindow alloc]initWithContentRect:win_rect
                                        styleMask:  NSWindowStyleMaskTitled |
                                                    NSWindowStyleMaskClosable |
                                                    NSWindowStyleMaskMiniaturizable |
                                                    NSWindowStyleMaskResizable
                                        backing  :  NSBackingStoreBuffered
                                        defer    :  NO ];

    // give title to the window
    [window setTitle:@"PB Cocoa Window"];

    // center the window
    [window center];
    
    // painting window bg black
    [window setBackgroundColor:[NSColor blackColor]];

    // create the custuom view
    glView = [[GLView alloc] initWithFrame:win_rect];

    // set this newly created custum view as view of our newly created custom view
    [window setContentView:glView];

    // setdelegate for window
    [window setDelegate:self];

    // now actually show the window, give its keyboard focus ,and make it top on z order
    [window makeKeyAndOrderFront:self];

}

-(void) applicationWillTerminate:(NSNotification *)notification
{
    // log file closing code
    if(gpFile)
    {
        fprintf(gpFile , "Program Finished successfully \n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

-(void) windowWillClose:(NSNotification *)notification
{
    // code
    [NSApp terminate:self];

}

-(void)dealloc
{
    [super dealloc];
    [glView release];
    [window release];

}

@end

// implimentation of custom view interface

@implementation GLView
{
    CVDisplayLinkRef displayLink;
    
    // iniline initialization here not allowed
    // if you have to do, do it in constructor
    GLuint shaderProgramObject;
    
    GLuint vao_triangle ;
    GLuint vbo_position_triangle ;

    GLuint vao_square ;
    GLuint vbo_position_square ;

    GLuint vao_circle ;
    GLuint vbo_position_circle ;
    GLuint vbo_color ;

    GLuint vao_blueVerticalLines ;
    GLuint vbo_position_blueVerticalLines ;

    GLuint vao_blueHorizontalLines ;
    GLuint vbo_position_blueHorizontalLine;
    GLuint vbo_blue_color ;

    GLuint vao_green_line ;
    GLuint vbo_position_green_line ;
    GLuint vbo_green_color ;

    GLuint vao_red_line ;
    GLuint vbo_position_red_line ;
    GLuint vbo_red_color ;
    
    GLuint mvpMatrixUniform;
    
    BOOL G_Pressed;
    BOOL T_Pressed;
    BOOL S_Pressed;
    BOOL C_Pressed;

    float x;
    float y;
    float Radian;

    int i;
    GLfloat circle_position[1081];
    
    mat4 perspectiveProjectionMatrix; // mat4 datatype is in vmath.h header file, that means 4X4 matrix
    
}

-(id) initWithFrame:(NSRect)frame  // constructor for our GLView
{
    // code
    self = [super initWithFrame:frame];

    if(self)
    {
        // declare OpenGL attributes
        NSOpenGLPixelFormatAttribute attributes[] =
        {
            NSOpenGLPFAOpenGLProfile , NSOpenGLProfileVersion4_1Core ,
            NSOpenGLPFAScreenMask    , CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay) ,
            NSOpenGLPFAColorSize     , 24 ,
            NSOpenGLPFADepthSize    , 32 ,
            NSOpenGLPFAAlphaSize     , 8  ,
            NSOpenGLPFANoRecovery    ,
            NSOpenGLPFAAccelerated   ,
            NSOpenGLPFADoubleBuffer  ,
            0
        };
        
        // create OpenGL Pixer Format using above attributes

        NSOpenGLPixelFormat *pixelFormat =
        [[[NSOpenGLPixelFormat alloc]initWithAttributes : attributes]autorelease];

        if(pixelFormat == nil)
        {
            fprintf(gpFile , "Creating pixelFormat failed \n");
            [self uninitialize];
            [self release];

        }

        // create OpenGL context using above created pixel format

        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat : pixelFormat shareContext : nil]autorelease];
        if(glContext == nil)
        {
            fprintf(gpFile , "Creating glContext failed \n");
            [self uninitialize];
            [self release];

        }

        // set pixel format
        [self setPixelFormat : pixelFormat ];


        //set OpenGL Context
        [self setOpenGLContext : glContext ];

    }

    return self;
}


-(void)prepareOpenGL
{
    [super prepareOpenGL];
    [[self openGLContext]makeCurrentContext];

    // matching monitor retraceing with the double buffer swapping
    GLint swapInterval = 1;
    [[self openGLContext]setValues : &swapInterval forParameter : NSOpenGLCPSwapInterval];

    int result = [self initialize];

    // create and start display link

    //steps of creating display link in prepareopengl

    // 1 - create display link according to the current active display
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

    // 2 - set the callback method for opengl thread
    
    CVDisplayLinkSetOutputCallback(displayLink , &MyDisplayLinkCallback , self);

    // 3 - convert NSOpenGLPixel format to CGLPixel Format
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];

    // 4 - convert NSOpenGLContext into CGLContext
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];

    // 5 - set above two, for display link
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink , cglContext , cglPixelFormat);

    // 6 - start displaylink , start the OpenGL Thread
    CVDisplayLinkStart(displayLink);

}

-(void)reshape
{
    [super reshape];
    [[self openGLContext]makeCurrentContext];

    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    NSRect rect = [self bounds];
    int width = rect.size.width;
    int height = rect.size.height;

    // call our rezise here
    [self resize : width : height];
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

}

-(void) drawRect:(NSRect)dirtyRect
{
    // code
    // call rendering function here too, to avoid flickering
    [self drawView];
    
}

-(void)drawView
{
    // our rendering function
    
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    [self display];

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);  // swapbuffers in windows
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

}

// function call per frame using auto release pool
-(CVReturn)getFrameForTime : (const CVTimeStamp*)outputTime
{
    // code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    // call rendering function here too
    [self drawView];
    [self myupdate];

    [pool release];

    return kCVReturnSuccess;
}

-(int)initialize
{
    // code

//    [self printGLInfo];

    // shader from here
    // vertex shader

    // mat4 used in shader is inbuild in GLSL

    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 aPosition;" \
        "uniform mat4 uMVPMatrix;" \
        "in vec4 aColor;" \
        "out vec4 oColor;" \
        "void main(void)" \
        "{" \
        "gl_Position = uMVPMatrix * aPosition;" \
        "oColor = aColor;" \
        "}";

    // uMVPMatrix * aPosition; is a matrix multiplication which we push from host to gpu

    // create vertex shader object

    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    glCompileShader(vertexShaderObject);

    // check for vertex shader compilation error if any

    GLint status = 0;
    GLint infoLogLength = 0;
    GLchar* szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
    if (status == FALSE)
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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
        
    }

    // fragment shader

    const GLchar* fragmentShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 oColor;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = oColor;" \
        "}";

    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
    
    glCompileShader(fragmentShaderObject);

    // for fragment shader

    status = 0;
    infoLogLength = 0;
    szInfoLog = NULL;

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
    if (status == FALSE)
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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    // create shader program

    shaderProgramObject = glCreateProgram();

    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor");


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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    // get shader uniform location

    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject , "uMVPMatrix");

    const GLfloat triangle_position[] = {
        0.0f,1.0f,0.0f,
        -1.0f,-1.0f,0.0f,
        
        -1.0f,-1.0f,0.0f,
        1.0f,-1.0f,0.0f,
        
        1.0f,-1.0f,0.0f,
        0.0f,1.0f,0.0f,

    };

    const GLfloat square_position[] = {
        1.0f,1.0f,0.0f,
        -1.0f,1.0f,0.0f,
        
        -1.0f,1.0f,0.0f,
        -1.0f,-1.0f,0.0f,

        -1.0f,-1.0f,0.0f,
        1.0f,-1.0f,0.0f,
        
        1.0f,-1.0f,0.0f,
        1.0f,1.0f,0.0f,

    };
    

    for (float fAngle = 0.0f; fAngle < 361.0f; fAngle++)
    {
        Radian = fAngle * M_PI / 180.0f;

        x = 1.0f * cos(Radian);
        y = 1.0f * sin(Radian);

        if (x != 0.0)
            circle_position[i] = x;

        circle_position[i + 1] = y;
        circle_position[i + 2] = 0.0f;
        fprintf(gpFile,"%f , %f , %f, \n", circle_position[i], circle_position[i + 1], circle_position[i + 2]);
        i += 3;
    }

    const GLfloat blueV_position[] = {
        0.0f,10.0f,0.0f,
        0.0f,-10.0f,0.0f
    };
    const GLfloat blueH_position[] = {
        10.0f,0.0f,0.0f,
        -10.0f,0.0f,0.0f
    };
    const GLfloat blue_color[] = {
        0.0f,0.0f,1.0f,
        0.0f,0.0f,1.0f
    };

    const GLfloat red_position[] = {
        10.0f,0.0f,0.0f,
        -10.0f,0.0f,0.0f
    };

    const GLfloat red_color[] = {
        1.0f,0.0f,0.0f,
        1.0f,0.0f,0.0f
    };

    const GLfloat green_position[] = {
        0.0f,10.0f,0.0f,
        0.0f,-10.0f,0.0f
    };

    const GLfloat green_color[] = {
        0.0f,1.0f,0.0f,
        0.0f,1.0f,0.0f
    };



    // TRIANGLE VAO for position
    glGenVertexArrays(1, &vao_triangle);
    glBindVertexArray(vao_triangle);
    // TRIANGLE VBO for position
    glGenBuffers(1, &vbo_position_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position), triangle_position, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // TRIANGLE VBO for color
    glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);

    // Unbind vertex array
    glBindVertexArray(0);

    // SQUARE VAO for position
    glGenVertexArrays(1, &vao_square);
    glBindVertexArray(vao_square);

    // SQUARE VBO for position
    glGenBuffers(1, &vbo_position_square);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // SQUARE color
    glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);

    // Unbind vertex array
    glBindVertexArray(0);

    // CIRCLE VAO for position
    glGenVertexArrays(1, &vao_circle);
    glBindVertexArray(vao_circle);

    // CIRCLE VBO for position
    glGenBuffers(1, &vbo_position_circle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_circle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_position), circle_position, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // CIRCLE color
    glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);

    // Unbind vertex array
    glBindVertexArray(0);

    //Blue
    // verticle VAO for position
    glGenVertexArrays(1, &vao_blueVerticalLines);
    glBindVertexArray(vao_blueVerticalLines);

    // verticle VBO for position
    glGenBuffers(1, &vbo_position_blueVerticalLines);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_blueVerticalLines);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blueV_position), blueV_position, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // verticle color
    glGenBuffers(1, &vbo_blue_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_blue_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blue_color), blue_color, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind vertex array
    glBindVertexArray(0);


    // horizontal VAO for position
    glGenVertexArrays(1, &vao_blueHorizontalLines);
    glBindVertexArray(vao_blueHorizontalLines);

    // horizontal VBO for position
    glGenBuffers(1, &vbo_position_blueHorizontalLine);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_blueHorizontalLine);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blueH_position), blueH_position, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Horizontal color
    glGenBuffers(1, &vbo_blue_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_blue_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blue_color), blue_color, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind vertex array
    glBindVertexArray(0);

    //Red
    // verticle VAO for position
    glGenVertexArrays(1, &vao_red_line);
    glBindVertexArray(vao_red_line);

    // verticle VBO for position
    glGenBuffers(1, &vbo_position_red_line);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_red_line);
    glBufferData(GL_ARRAY_BUFFER, sizeof(red_position), red_position, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // verticle color
    glGenBuffers(1, &vbo_red_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_red_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(red_color), red_color, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind vertex array
    glBindVertexArray(0);

    // Green
    // verticle VAO for position
    glGenVertexArrays(1, &vao_green_line);
    glBindVertexArray(vao_green_line);

    // verticle VBO for position
    glGenBuffers(1, &vbo_position_green_line);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_green_line);
    glBufferData(GL_ARRAY_BUFFER, sizeof(green_position), green_position, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // verticle color
    glGenBuffers(1, &vbo_green_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_green_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(green_color), green_color, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind vertex array
    glBindVertexArray(0);

    
    // enabling depth

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // clear color

    // set the clear colour of window to blue

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    perspectiveProjectionMatrix = vmath::mat4::identity();


    [self resize:WIN_WIDTH :WIN_HEIGHT];

    return 0;
}

//-(void)printGLInfo
//{
//    // variable declaration
//    GLint numExtentions =0;
//    GLint i;
//
//    // code
//
//    fprintf(gpFile, "OpenGL Vendor : %s \n", glGetString(GL_VENDOR));
//    fprintf(gpFile, "OpenGL Renderer : %s \n", glGetString(GL_RENDERER));
//    fprintf(gpFile, "OpenGL Version : %s \n", glGetString(GL_VERSION));
//    fprintf(gpFile, "OpenGL GLSL Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
//
//    fprintf(gpFile, "****************************************************************************************** \n");
//    // listiong of supported extentions
//
//    fprintf(gpFile, "Number of EXT. : %d \n", numExtentions);
//
//    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtentions);
//
//    for (i = 0; i < numExtentions; i++)
//    {
//        fprintf(gpFile, "%s \n", glGetStringi(GL_EXTENSIONS , i));
//    }
//
//    fprintf(gpFile, "****************************************************************************************** \n");
//
//}

-(void)resize:(int)width :(int)height
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

-(void)display
{
    // code

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObject);

    mat4 modelViewMatrix = mat4::identity();
    modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    // Order of multiplication vimp
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    if(G_Pressed == TRUE)
    {
        for (float i1 = 0.1f; i1 < 4.0f; i1 += 0.1)
        {
            modelViewMatrix = mat4::identity();
            modelViewMatrix = vmath::translate(i1, 0.0f, -6.0f);
            // Order of multiplication vimp
            modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

            glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

            glBindVertexArray(vao_blueVerticalLines);
            glDrawArrays(GL_LINES, 0, 2);
            glBindVertexArray(0);
        }
        for (float i1 = -0.1f; i1 > -4.0f; i1 -= 0.1)
        {
            modelViewMatrix = mat4::identity();
            modelViewMatrix = vmath::translate(i1, 0.0f, -6.0f);
            // Order of multiplication vimp
            modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

            glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

            glBindVertexArray(vao_blueVerticalLines);
            glDrawArrays(GL_LINES, 0, 2);
            glBindVertexArray(0);
        }
        for (float i1 = -0.1f; i1 > -4.0f; i1 -= 0.1)
        {
            modelViewMatrix = mat4::identity();
            modelViewMatrix = vmath::translate(0.0f, i1, -6.0f);
            // Order of multiplication vimp
            modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

            glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

            glBindVertexArray(vao_blueHorizontalLines);
            glDrawArrays(GL_LINES, 0, 2);
            glBindVertexArray(0);
        }
        for (float i1 = 0.1f; i1 < 4.0f; i1 += 0.1)
        {
            modelViewMatrix = mat4::identity();
            modelViewMatrix = vmath::translate(0.0f, i1, -6.0f);
            // Order of multiplication vimp
            modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

            glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

            glBindVertexArray(vao_blueHorizontalLines);
            glDrawArrays(GL_LINES, 0, 2);
            glBindVertexArray(0);
        }

        modelViewMatrix = mat4::identity();
        modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
        // Order of multiplication vimp
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

        glBindVertexArray(vao_red_line);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);


        modelViewMatrix = mat4::identity();
        modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
        // Order of multiplication vimp
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

        glBindVertexArray(vao_green_line);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);

    }
    if(T_Pressed == TRUE)
    {
        glBindVertexArray(vao_triangle);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);
    }
    if(S_Pressed == TRUE)
    {
        glBindVertexArray(vao_square);
        glDrawArrays(GL_LINES, 0, 8);
        glBindVertexArray(0);
    }
    if(C_Pressed == TRUE)
    {
        glBindVertexArray(vao_circle);

        for (int i1 = 0; i1 < 119; i1++) {

            glDrawArrays(GL_LINES, i1, 242);

        }

        glBindVertexArray(0);
    }

    glUseProgram(0);


}

-(void)myupdate
{
    // code

}

-(void)uninitialize
{
    // code
    
    //  function declarations
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

                for (GLint i1 = 0; i1 < numShaders; i1++)
                {
                    glDetachShader(shaderProgramObject, pShaders[i1]);
                    glDeleteShader(pShaders[i1]);
                    pShaders[i1] = 0;
                }
                free(pShaders);
                pShaders = NULL;
            }
        }
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;/var/folders/mq/8n4tvjdx123fd5_ch04p69100000gn/T/TemporaryItems/NSIRD_screencaptureui_clvtUC/Screenshot 2024-09-03 at 9.35.12 PM.png
    }
    
}





-(bool)acceptsFirstResponder
{
    // code
    [[self window]makeFirstResponder:self];

    return YES;
}

-(void)keyDown:(NSEvent *)event
{
    int key = (int)[[event characters]characterAtIndex:0];
    switch(key)
    {
        case 27:
            [self release];
            [self uninitialize];
            [NSApp terminate:self];
            break;

        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
            break;
            
        case 'G':
        case 'g':

            if (G_Pressed == FALSE)
            {
                G_Pressed = TRUE;
            }
            else
            {
                G_Pressed = FALSE;
            }
            
            break;

        case 'T':
        case 't':
            if (T_Pressed == FALSE)
            {
                T_Pressed = TRUE;
            }
            else
            {
                T_Pressed = FALSE;
            }
            break;

        case 'S':
        case 's':
            if (S_Pressed == FALSE)
            {
                S_Pressed = TRUE;
            }
            else
            {
                S_Pressed = FALSE;
            }

            break;

        case 'C':
        case 'c':

            if (C_Pressed == FALSE)
            {
                C_Pressed = TRUE;
            }
            else
            {
                C_Pressed = FALSE;
            }
            break;

        default:
            break;

    }
}

-(void)mouseDown:(NSEvent *)event
{
    
}

-(void)rightMouseDown:(NSEvent *)event
{
    
}

-(void)dealloc
{
    [super dealloc];
    [self uninitialize];
//    if(displayLink)
//    {
//        CVDisplayLinkStop(displayLink);
//        CVDisplayLink(displayLink);
//    }
    
}

@end

// defination of global callback function
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink ,
                               const CVTimeStamp *now ,
                               const CVTimeStamp *outputTime ,
                               CVOptionFlags flagsIn ,
                               CVOptionFlags *flagsOut ,
                               void *displayLinkContext )
{
    // code

    CVReturn result = [(GLView *)displayLinkContext getFrameForTime : outputTime];
    return result;
}




