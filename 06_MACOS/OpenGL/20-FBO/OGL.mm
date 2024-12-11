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
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_TEXCOORD
    
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
    [window setTitle:@"VB Cocoa Window"];

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
    GLint winWidth;
    GLint winHeight;

    // variables for cube

    GLuint vao_Cube;
    GLuint vbo_position_cube;
    GLuint vbo_Texcoord_cube;
    GLfloat angle_cube;

    GLuint texture_kundali;

    GLuint textureSamplerUniformCube;
    GLuint mvpMatrixUniformCube;


    mat4 perspectiveProjectionMatrixCube; // mat4 datatype is in vmath.h header file, that means 4X4 matrix

    // sphere related global variables

    GLuint shaderProgramObject_pv_Sphere ;
    GLuint shaderProgramObject_pf_Sphere ;

    // variables for sphere
    GLuint gVao_sphere ;
    GLuint gVbo_sphere_position ;
    GLuint gVbo_sphere_normal ; // lights
    GLuint gVbo_sphere_texcoord ; // lights
    GLuint gVbo_sphere_element ;

    struct lights
    {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        vec4 position;
    };

    struct lights light[3];

    GLuint modelMatrixUniform_Sphere ;
    GLuint projectionMatrixUniform_Sphere ;
    GLuint viewMatrixUniform_Sphere ;

    GLuint lightDiffuseUniform_Sphere[3]; // for diffuse light
    GLuint materialDiffuseUniform_Sphere ; // for diffuse material
    GLuint lightAmbientUniform_Sphere[3]; // for ambient light
    GLuint materialAmbientUniform_Sphere ; // for ambient material
    GLuint lightSpecularUniform_Sphere[3]; // for specular light
    GLuint materialSpecularUniform_Sphere ; // for specular material
    GLuint materialShinenessUniform_Sphere ; // for shinneness material
    GLuint lightPositionUniform_Sphere[3];
    GLuint keyPressUniform_Sphere ;

    BOOL bLightingEnabled ;
    BOOL bLightingEnabled_pv ;
    BOOL bLightingEnabled_pf ;

    GLfloat materialAmbient[4] ;
    GLfloat materialDiffuse[4] ;
    GLfloat materialSpecular[4] ;
    GLfloat materialShineness;

    GLfloat lightAngleZero ;
    GLfloat lightAngleOne ;
    GLfloat lightAngleTwo ;

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
    BOOL vFboResult;


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

// load texture function

-(GLuint) loadGLTexture:(const char*)szImageFileName
{
    // code
    
    // step 1
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *imageFileNameWithPath = [NSString stringWithFormat:@"%@/%s" , parentDirPath , szImageFileName];
    
    // step 2
    NSImage *image = [[NSImage alloc]initWithContentsOfFile:imageFileNameWithPath];
    
    // step 3
    CGImageRef cgImage = [image CGImageForProposedRect:nil context:nil hints:nil];
    
    // step 4
    int imageWidth = (int)CGImageGetWidth(cgImage);
    int imageHeight = (int)CGImageGetHeight(cgImage);
    
    // step 5
    CGDataProviderRef imageDataProviderRef = CGImageGetDataProvider(cgImage);
    
    // step 6
    CFDataRef imageDataRef = CGDataProviderCopyData(imageDataProviderRef);
    
    // step 7
    void *imageData = (void *)CFDataGetBytePtr(imageDataRef);
    
    // step 8
    // Create OpenGl texture using above imageWidth height and image data
    
    GLuint texture = 0;
    
    glGenTextures(1,&texture);

    // bind to the generated texture
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // set texture paramerter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    CFRelease(imageDataRef);
    
    return texture;
    
}


-(int)initialize
{
    // code

    [self printGLInfo];

    // shader from here
    // vertex shader

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


    // enabling depth

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // clear color

    // set the clear colour of window to blue

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    perspectiveProjectionMatrix = vmath::mat4::identity();

    if (createFBO(FBO_WIDTH, FBO_HEIGHT) == TRUE)
    {
        vFboResult = initialize_Sphere(FBO_WIDTH, FBO_HEIGHT);
    }

    [self resize:WIN_WIDTH :WIN_HEIGHT];

    return 0;
}

-(void)printGLInfo
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

-(BOOL) initialize_Sphere:(int)texture_width :(int)texture_height
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
        if (status == FALSE)
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
        if (status == FALSE)
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
        if (status == FALSE)
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
        if (status == FALSE)
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


    resize_Sphere(WIN_WIDTH, WIN_HEIGHT);

    return TRUE;
}

// FBO releted code
-(BOOL) createFBO(int)textureWidth :(int)textureHeight
{
    GLint maxRenderBufferSize;
    // check capacity of render buffer

    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);

    if (maxRenderBufferSize < textureWidth || maxRenderBufferSize < textureHeight)
    {
        fprintf(gpFile, "Texture size overflow \n");
        return FALSE;
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
        return FALSE;
    }

    // unbind framebuffer

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return TRUE;
}

-(void)resize_Cube:(int)width :(int)height
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

-(void)resize_Sphere:(int)width :(int)height
{
    // code
    if (height <= 0)
    {
        height = 1;
    }
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrix_Sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

}

-(void)display_Cube
{

    // render FBO scene

    if (vFboResult == TRUE)
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

}

-(void)display_Sphere:(int)textureWidth :(int)textureHeight
{
    // code
    // bind with FBO

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // call resize sphere

    resize_Sphere(textureWidth , textureHeight);

    // set the clear color to black to compensate the change done by display sphere

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (bLightingEnabled_pv == TRUE)
    {
        glUseProgram(shaderProgramObject_pv_Sphere);
    }
    else if (bLightingEnabled_pf == TRUE)
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

        if (bLightingEnabled == TRUE)
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

-(void)update_Cube
{
    // code
    angle_cube -= 1.5f;
    if (angle_cube <= 0.0f)
    {
        angle_cube += 360.0f;
    }
}

-(void)update_Sphere
{
    // code
    if (bLightingEnabled == TRUE)
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


    // delete VBO of position

    if (vbo)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    // delete VAO

    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    
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

        case 'Q':
        case 'q':
            [self release];
            [self uninitialize];
            [NSApp terminate:self];

            break;
            
        case 'F':
        case 'f':
            if (bLightingEnabled_pf == FALSE)
            {
                bLightingEnabled_pf = TRUE;
            }
            else
            {
                bLightingEnabled_pf = FALSE;
            }
            break;

        case 'V':
        case 'v':
            if (bLightingEnabled_pv == FALSE)
            {
                bLightingEnabled_pv = TRUE;
            }
            else
            {
                bLightingEnabled_pv = FALSE;
            }
            break;

        case 'L':
        case 'l':

            if (bLightingEnabled == FALSE)
            {
                bLightingEnabled = TRUE;
            }
            else
            {
                bLightingEnabled = FALSE;
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




