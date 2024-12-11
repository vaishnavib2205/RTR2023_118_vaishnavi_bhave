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
    GLuint vao;
    GLuint vbo;
    GLuint vbo_texcoord;
    
    GLuint smiley_Texture;
    
    GLuint mvpMatrixUniform;
    GLuint textureSamplerUniform;
    GLuint keypressUniform;
    
    GLuint key_used;
    
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

    // mat4 used in shader is inbuild in GLSL

    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 aPosition;" \
        "in vec2 aTexCoord;" \
        "out vec2 oTexCoord;" \
        "uniform mat4 uMVPMatrix;" \
        "void main(void)" \
        "{" \
        "gl_Position = uMVPMatrix * aPosition;" \
        "oTexCoord = aTexCoord;" \
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
        "in vec2 oTexCoord;" \
        "uniform sampler2D uTextureSampler;" \
        "uniform int ukey_used;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "if(ukey_used == 0)" \
        "{" \
        "FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
        "}" \
        "else" \
        "{" \
        "FragColor = texture(uTextureSampler, oTexCoord);" \
        "}" \
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
        [self uninitialize];
        [self release];
        [NSApp terminate:self];
    }

    // get shader uniform location

    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject , "uMVPMatrix");


    //get shader uniform locations
    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");

    keypressUniform = glGetUniformLocation(shaderProgramObject, "ukey_used");

    // declare position and color arrays

    const GLfloat square_position[] =
    {
        
        1.0f, 1.0f, 0.0f ,        //glVertex3f(1.0f, 1.0f, 0.0f);
        -1.0f, 1.0f, 0.0f ,        //glVertex3f(-1.0f, 1.0f, 0.0f);
        -1.0f, -1.0f, 0.0f ,    //glVertex3f(-1.0f, -1.0f, 0.0f);
        1.0f, -1.0f, 0.0f        //glVertex3f(1.0f, -1.0f, 0.0f);
    };

    const GLfloat square_texcoords[] =
    {
        1.0f , 1.0f,
        0.0f , 1.0f,
        0.0f , 0.0f,
        1.0f , 0.0f
    };



    // square

    {
        // VAO = vertex array object

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // VBO for position

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // VBO for texCoord

        glGenBuffers(1, &vbo_texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(square_texcoords), square_texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // unbind with vao_square
        glBindVertexArray(0);

    }

    
    smiley_Texture = [self loadGLTexture:"Smiley.bmp"];

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
    // Code
    GLfloat square_texcoord[8];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObject);



    // transformation

    mat4 modelViewMatrix = mat4::identity();

    modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order of multiplication is very important

    // now push modelViewProjectionMatrix in the "uMVPMatrix" of shader , per frame.

    // push above mvp into vertex shader mvpMatrixUniform

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //for texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, smiley_Texture);
    glUniform1i(textureSamplerUniform , 0);

    glBindVertexArray(vao);
    
    //texture coords
    if (key_used == 1)
    {
        square_texcoord[0] = 1.0f;
        square_texcoord[1] = 1.0f;
        square_texcoord[2] = 0.0f;
        square_texcoord[3] = 1.0f;
        square_texcoord[4] = 0.0f;
        square_texcoord[5] = 0.0f;
        square_texcoord[6] = 1.0f;
        square_texcoord[7] = 0.0f;

        glUniform1i(keypressUniform, 1);
    }

    else if (key_used == 2)
    {
        square_texcoord[0] = 0.5f;
        square_texcoord[1] = 0.5f;
        square_texcoord[2] = 0.0f;
        square_texcoord[3] = 0.5f;
        square_texcoord[4] = 0.0f;
        square_texcoord[5] = 0.0f;
        square_texcoord[6] = 0.5f;
        square_texcoord[7] = 0.0f;

        glUniform1i(keypressUniform, 1);
    }
    else if (key_used == 3)
    {
        square_texcoord[0] = 2.0f;
        square_texcoord[1] = 2.0f;
        square_texcoord[2] = 0.0f;
        square_texcoord[3] = 2.0f;
        square_texcoord[4] = 0.0f;
        square_texcoord[5] = 0.0f;
        square_texcoord[6] = 2.0f;
        square_texcoord[7] = 0.0f;

        glUniform1i(keypressUniform, 1);
    }
    else if (key_used == 4)
    {
        square_texcoord[0] = 0.5f;
        square_texcoord[1] = 0.5f;
        square_texcoord[2] = 0.5f;
        square_texcoord[3] = 0.5f;
        square_texcoord[4] = 0.5f;
        square_texcoord[5] = 0.5f;
        square_texcoord[6] = 0.5f;
        square_texcoord[7] = 0.5f;

        glUniform1i(keypressUniform, 1);
    }
    else
    {
        glUniform1i(keypressUniform, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_texcoord), square_texcoord, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
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
            

        case '1':
            key_used = 1;
            glEnable(GL_TEXTURE_2D);
            break;

        case '2':
            key_used = 2;
            glEnable(GL_TEXTURE_2D);
            break;

        case '3':
            key_used = 3;
            glEnable(GL_TEXTURE_2D);
            break;

        case '4':
            key_used = 4;
            glEnable(GL_TEXTURE_2D);
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




