#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h> // all Programmable content after OpenGL 3.0 version, so gl3
#import <OpenGL/gl3ext.h>

// header file for vmath
#include "vmath.h"
using namespace vmath;

#include "Sphere.h"

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
    AMC_ATTRIBUTE_TEXCOORDS,
    AMC_ATTRIBUTE_NORMALS// lights
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
    GLuint shaderProgramObject_pv;
    GLuint shaderProgramObject_pf;

    // variables for sphere
    GLuint gVao_sphere ;
    GLuint gVbo_sphere_position ;
    GLuint gVbo_sphere_normal ; // lights
    GLuint gVbo_sphere_texcoord ; // lights
    GLuint gVbo_sphere_element ;
    
    GLuint modelMatrixUniform ;
    GLuint projectionMatrixUniform ;
    GLuint viewMatrixUniform ;

    GLuint modelMatrixUniform_pf ;
    GLuint projectionMatrixUniform_pf ;
    GLuint viewMatrixUniform_pf ;

    GLuint lightDiffuseUniform ; // for diffuse light
    GLuint materialDiffuseUniform ; // for diffuse material
    GLuint lightAmbientUniform ; // for ambient light
    GLuint materialAmbientUniform ; // for ambient material
    GLuint lightSpecularUniform ; // for specular light
    GLuint materialSpecularUniform ; // for specular material
    GLuint materialShinenessUniform ; // for shinneness material
    GLuint lightPositionUniform ;

    GLuint lightDiffuseUniform_pf ; // for diffuse light
    GLuint materialDiffuseUniform_pf ; // for diffuse material
    GLuint lightAmbientUniform_pf ; // for ambient light
    GLuint materialAmbientUniform_pf ; // for ambient material
    GLuint lightSpecularUniform_pf ; // for specular light
    GLuint materialSpecularUniform_pf ; // for specular material
    GLuint materialShinenessUniform_pf ; // for shinneness material
    GLuint lightPositionUniform_pf ;

    GLuint keyPressUniform ;


    BOOL bLightingEnabled ;
    BOOL bLightingEnabled_pv ;
    BOOL bLightingEnabled_pf ;


    GLfloat lightAmbient[4] ;
    GLfloat lightDiffuse[4] ; // white diffuse light
    GLfloat lightSpecular[4] ;
    GLfloat lightPosition[4] ;

    GLfloat materialAmbient[4] ;
    GLfloat materialDiffuse[4] ;
    GLfloat materialSpecular[4] ;
    GLfloat materialShineness;

    
    Sphere sphere;
    
    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    unsigned short sphere_elements[2280];

    GLuint gNumVertices;
    GLuint gNumElements;
    
    
    
    
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

    [self printGLInfo];

    // shader from here
    // vertex shader

    // mat4 used in shader is inbuild in GLSL
    
// Per vertex

    const GLchar* vertexShaderSourceCode_pv =
        "#version 410 core" \
        "\n" \
        "in vec4 aPosition;" \
        "in vec3 aNormal;" \
        "uniform mat4 uModelMatrix;" \
        "uniform mat4 uViewMatrix;" \
        "uniform mat4 uProjectionMatrix;" \
        "uniform vec3 uLightAmbient;" \
        "uniform vec3 uLightDiffuse;" \
        "uniform vec3 uLightSpecular;" \
        "uniform vec4 uLightposition;" \
        "uniform vec3 uMaterialAmbient;" \
        "uniform vec3 uMaterialDiffuse;" \
        "uniform vec3 uMaterialSpecular;" \
        "uniform float uMaterialShineness;" \
        "uniform int uKeyPress;" \
        "out vec3 oFong_ADS_Light;" \
        "out vec3 oDiffuseLight;" \
        "in vec4 aColor;" \
        "out vec4 oColor;" \
        "void main(void)" \
        "{" \
        "if(uKeyPress == 1)" \
        "{" \
        "vec4 eyeCoordinates =  uViewMatrix * uModelMatrix * aPosition;" \
        "vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);" \
        "vec3 lightDirection = normalize(vec3(uLightposition - eyeCoordinates));" \
        "vec3 reflectionVector = reflect(-lightDirection , transformedNormals);" \
        "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
        "vec3 ambientLight = uLightAmbient * uMaterialAmbient;" \
        "vec3 diffuseLight = uLightDiffuse * uMaterialDiffuse * max(dot(lightDirection , transformedNormals) , 0.0f); " \
        "vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector , viewerVector) , 0.0f) , uMaterialShineness); " \
        "oFong_ADS_Light = ambientLight + diffuseLight + specularLight; " \
        "}" \
        "else" \
        "{" \
        "oFong_ADS_Light = vec3(0.0f , 0.0f , 0.0f);" \
        "}" \
        "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" \
        "}";


    // uMVPMatrix * aPosition; is a matrix multiplication which we push from host to gpu

    // create vertex shader object

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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
        
    }

    // fragment shader

    const GLchar* fragmentShaderSourceCode_pv =
        "#version 410 core" \
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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    // get shader uniform location

    modelMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "uModelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "uViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "uProjectionMatrix");
    lightAmbientUniform = glGetUniformLocation(shaderProgramObject_pv, "uLightAmbient");
    lightDiffuseUniform = glGetUniformLocation(shaderProgramObject_pv, "uLightDiffuse");
    lightSpecularUniform = glGetUniformLocation(shaderProgramObject_pv, "uLightSpecular");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject_pv, "uLightposition");
    materialAmbientUniform = glGetUniformLocation(shaderProgramObject_pv, "uMaterialAmbient");
    materialDiffuseUniform = glGetUniformLocation(shaderProgramObject_pv, "uMaterialDiffuse");
    materialSpecularUniform = glGetUniformLocation(shaderProgramObject_pv, "uMaterialSpecular");
    materialShinenessUniform = glGetUniformLocation(shaderProgramObject_pv, "uMaterialShineness");
    keyPressUniform = glGetUniformLocation(shaderProgramObject_pv, "uKeyPress");
    
    
    // for per-Fragment

        // vertex shader

    const GLchar* vertexShaderSourceCode_pf =
        "#version 410 core" \
        "\n" \
        "in vec4 aPosition;" \
        "in vec3 aNormal;" \
        "uniform mat4 uModelMatrix;" \
        "uniform mat4 uViewMatrix;" \
        "uniform mat4 uProjectionMatrix;" \
        "uniform vec4 uLightposition;" \
        "uniform int uKeyPress;" \
        "out vec3 otransformedNormals;" \
        "out vec3 olightDirection;" \
        "out vec3 oviewerVector;" \
        "in vec4 aColor;" \
        "out vec4 oColor;" \
        "void main(void)" \
        "{" \
        "if(uKeyPress == 1)" \
        "{" \
        "vec4 eyeCoordinates =  uViewMatrix * uModelMatrix * aPosition;" \
        "otransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;" \
        "olightDirection = vec3(uLightposition - eyeCoordinates);" \
        "oviewerVector = -eyeCoordinates.xyz;" \
        "}" \
        "else" \
        "{" \
        "otransformedNormals = vec3(0.0f , 0.0f , 0.0f);" \
        "olightDirection = vec3(0.0f , 0.0f , 0.0f);" \
        "oviewerVector = vec3(0.0f , 0.0f , 0.0f);" \
        "}" \
        "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;" \
        "}";

    // create vertex shader object

    GLuint vertexShaderObject_pf = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShaderObject_pf, 1, (const GLchar**)&vertexShaderSourceCode_pf, NULL);

    glCompileShader(vertexShaderObject_pf);

    // check for vertex shader compilation error if any

    status = 0;
    infoLogLength = 0;
    szInfoLog = NULL;

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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    // fragment shader

    const GLchar* fragmentShaderSourceCode_pf =
        "#version 410 core" \
        "\n" \
        "in vec4 oColor;" \
        "in vec3 otransformedNormals;" \
        "in vec3 olightDirection;" \
        "in vec3 oviewerVector;" \
        "out vec4 FragColor;" \
        "uniform vec3 uLightAmbient;" \
        "uniform vec3 uLightDiffuse;" \
        "uniform vec3 uLightSpecular;" \
        "uniform vec3 uMaterialAmbient;" \
        "uniform vec3 uMaterialDiffuse;" \
        "uniform vec3 uMaterialSpecular;" \
        "uniform float uMaterialShineness;" \
        "uniform int uKeyPress;" \
        "void main(void)" \
        "{" \
        "vec3 Phong_ADS_Light;" \
        "if(uKeyPress == 1)" \
        "{" \
        "vec3 normalizedTranformedNormals = normalize(otransformedNormals);" \
        "vec3 normalizedLightDirection = normalize(olightDirection);" \
        "vec3 normalizedViewerVector= normalize(oviewerVector);" \
        "vec3 ambientLight = uLightAmbient * uMaterialAmbient;" \
        "vec3 diffuseLight = uLightDiffuse * uMaterialDiffuse * max(dot(normalizedLightDirection , normalizedTranformedNormals) , 0.0f); " \
        "vec3 reflectionVector = reflect(-normalizedLightDirection , normalizedTranformedNormals);" \
        "vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector , normalizedViewerVector) , 0.0f) , uMaterialShineness); " \
        "Phong_ADS_Light = ambientLight + diffuseLight + specularLight;" \
        "}" \
        "else" \
        "{" \
        "Phong_ADS_Light = vec3(1.0f , 1.0f , 1.0f );" \
        "}" \
        "FragColor = vec4(Phong_ADS_Light , 1.0f);" \
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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    // get shader uniform location

    modelMatrixUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uModelMatrix");
    viewMatrixUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uViewMatrix");
    projectionMatrixUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uProjectionMatrix");
    lightAmbientUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uLightAmbient");
    lightDiffuseUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uLightDiffuse");
    lightSpecularUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uLightSpecular");
    lightPositionUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uLightposition");
    materialAmbientUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uMaterialAmbient");
    materialDiffuseUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uMaterialDiffuse");
    materialSpecularUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uMaterialSpecular");
    materialShinenessUniform_pf = glGetUniformLocation(shaderProgramObject_pf, "uMaterialShineness");
    keyPressUniform = glGetUniformLocation(shaderProgramObject_pf, "uKeyPress");






    sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = sphere.getNumberOfSphereVertices();
    gNumElements = sphere.getNumberOfSphereElements();



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

    // element vbo
    glGenBuffers(1, &gVbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // for textures

    glGenBuffers(1, &gVbo_sphere_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_texcoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_textures), sphere_textures, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // unbind vao
    glBindVertexArray(0);


    lightAmbient[0] = 0.1f;
    lightAmbient[1] = 0.1f;
    lightAmbient[2] = 0.1f;
    lightAmbient[3] = 1.0f;

    lightDiffuse[0] = 1.0f;
    lightDiffuse[1] = 1.0f;
    lightDiffuse[2] = 1.0f;
    lightDiffuse[3] = 1.0f;
    
    lightSpecular[0] = 1.0f;
    lightSpecular[1] = 1.0f;
    lightSpecular[2] = 1.0f;
    lightSpecular[3] = 1.0f;
    
    lightPosition[0] = 100.0f;
    lightPosition[1] = 100.0f;
    lightPosition[2] = 100.0f;
    lightPosition[3] = 1.0f;
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;

    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.2f;
    materialDiffuse[2] = 0.7f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.7f;
    materialSpecular[3] = 1.0f;
    
    materialShineness =  128.0f;



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

    if (bLightingEnabled_pv == TRUE)
    {
        glUseProgram(shaderProgramObject_pv);
    }
    else if (bLightingEnabled_pf == TRUE)
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



        if (bLightingEnabled == TRUE)
        {
            if(bLightingEnabled_pv == TRUE)
            {
                glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
                glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, ViewMatrix);
                glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

                glUniform1i(keyPressUniform, 1);
                glUniform3fv(lightAmbientUniform, 1, lightAmbient);
                glUniform3fv(lightDiffuseUniform, 1, lightDiffuse);
                glUniform3fv(lightSpecularUniform, 1, lightSpecular);
                glUniform3fv(materialAmbientUniform, 1, materialAmbient);
                glUniform3fv(materialDiffuseUniform, 1, materialDiffuse);
                glUniform3fv(materialSpecularUniform, 1, materialSpecular);
                glUniform4fv(lightPositionUniform, 1, lightPosition);
                glUniform1f(materialShinenessUniform, materialShineness);
                
            }
            if (bLightingEnabled_pf == TRUE)
            {
                glUniformMatrix4fv(modelMatrixUniform_pf, 1, GL_FALSE, modelMatrix);
                glUniformMatrix4fv(viewMatrixUniform_pf, 1, GL_FALSE, ViewMatrix);
                glUniformMatrix4fv(projectionMatrixUniform_pf, 1, GL_FALSE, perspectiveProjectionMatrix);

                glUniform1i(keyPressUniform, 1);
                glUniform3fv(lightAmbientUniform_pf, 1, lightAmbient);
                glUniform3fv(lightDiffuseUniform_pf, 1, lightDiffuse);
                glUniform3fv(lightSpecularUniform_pf, 1, lightSpecular);
                glUniform3fv(materialAmbientUniform_pf, 1, materialAmbient);
                glUniform3fv(materialDiffuseUniform_pf, 1, materialDiffuse);
                glUniform3fv(materialSpecularUniform_pf, 1, materialSpecular);
                glUniform4fv(lightPositionUniform_pf, 1, lightPosition);
                glUniform1f(materialShinenessUniform_pf, materialShineness);
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
            [[self window]toggleFullScreen:self];
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




