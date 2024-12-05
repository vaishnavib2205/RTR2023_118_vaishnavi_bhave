
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "OGLES.h"
#import "vmath.h"
#include "Sphere.h"
using namespace vmath;

enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_TEXCOORDS,
    AMC_ATTRIBUTE_NORMALS// lights
    
};

@implementation GLESView
{
    EAGLContext * eaglContext;
    
    GLuint customFrameBuffer;
    GLuint colorRenderBuffer;
    GLuint depthRenderBuffer;
    
    id displayLink;
    NSInteger framesPerSecond;
    BOOL isDisplayLink;
    
    GLuint shaderProgramObject;

    // variables for sphere
    GLuint gVao_sphere ;
    GLuint gVbo_sphere_position ;
    GLuint gVbo_sphere_normal ; // lights
    GLuint gVbo_sphere_texcoord ; // lights
    GLuint gVbo_sphere_element ;
    
    GLuint mvpMatrixUniform;
    
    Sphere sphere;
    
    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    unsigned short sphere_elements[2280];

    GLuint gNumVertices;
    GLuint gNumElements;
    
    //stack top index
    int top ;
    int Day ;
    int Year ;
    
    mat4 stack[32];
    
    
    
    
    mat4 perspectiveProjectionMatrix; // mat4 datatype is in vmath.h header file, that means 4X4 matrix
    
    
}

-(void) pushMatrix:(mat4)element
{
    void uninitialized(void);
    if((top + 1) > 32)
    {
        [self uninitialize];
    }
    
    stack[top] = element;
    top++;
    
}

-(mat4) popMatrix
{
    void uninitialized(void);
    if(top < 0)
    {
        // not valid stack
        [self uninitialize];
    }
    
    top--;
    return stack[top];
    
}


-(id)initWithFrame:(CGRect)frame {
    // Code
    self = [super initWithFrame:frame];
    
    if(self)
    {
        // Set the background to black
        [self setBackgroundColor:[UIColor blackColor]];
        
        // ////////////////////////////////////////////////////////////////////////////////////
        // 1. Create EAGL Layer
        CAEAGLLayer * eaglLayer = (CAEAGLLayer *)[super layer];
        // 1.a. Set layer opacity
        [eaglLayer setOpaque:YES];
        // 1.b. Set retained backing property
        // 1.c. Set color format for framebuffer
        // As a dictionary
        NSDictionary * dictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                     [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                     kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                     nil];
        [eaglLayer setDrawableProperties: dictionary];
        // 2. Create EAGLContext
        eaglContext = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES3];
        if (eaglContext == nil) {
            printf("[ERROR] OpenGLES: OpenGLES Context Creation Failed! Exiting...");
            [self uninitialize];
            [self release];
            exit(0);
        }
        // 3. Set this context as current context
        [EAGLContext setCurrentContext: eaglContext];
        // 4. Create custom frammebuffer
        glGenFramebuffers(1, &customFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, customFrameBuffer);
        // 5. Create color render buffer
        // 5.a. Create color render buffers
        glGenRenderbuffers(1, &colorRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
        // 5.b. Give storage to colorRenderBuffer by using the iOS Method (As we're rendering on iOS layer)
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        // 5.c. Give above colorbuffer to customFramebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);
        // 6. Depth Render Buffer
        // 6.a. Find width of the color buffer
        GLint width, height;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
        // 6.b. Create and bind with depth buffer
        glGenRenderbuffers(1, &depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
        // 6.c. Give storage to depth render buffer using usual OpenGLES Function
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        // 6.d. Attach the depth render buffer to customFrameBuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
        // 7. Check status of customFramebuffer
        GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("[ERROR] OpenGLES: Framebuffer Creation Failed! Exiting...");
            [self uninitialize];
            [self release];
            exit(0);
        }
        // 8. Initialize framesPerSeconds
        framesPerSecond = 60; // 60 is recommended since iOS 8.2
        // 9. Initialize displayLink Existence variable
        isDisplayLink = NO;
        // 10. Call our initialize method
        int result = [self initialize];
        if (result != 0) {
            printf("[ERROR] (initialize()): Initialization Failed! Exiting...");
            [self uninitialize];
            [self release];
            exit(0);
        }
        // ////////////////////////////////////////////////////////////////////////////////////
        
        // SingleTap
        // 1. Create object of SingleTapGestureRecgnizer Object
        UITapGestureRecognizer * singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        // 2. Set number of taps
        [singleTapGestureRecognizer setNumberOfTapsRequired: 1];
        // 3. Set number of fingers required
        [singleTapGestureRecognizer setNumberOfTouchesRequired: 1];
        // 4. Set delegate
        [singleTapGestureRecognizer setDelegate:self];
        // 6. Add recognizer in self
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        // Double Tap
        UITapGestureRecognizer * doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired: 2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired: 1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        // Swipe
        UISwipeGestureRecognizer * swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [swipeGestureRecognizer setNumberOfTouchesRequired: 1];
        [swipeGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        // Long Press
        UILongPressGestureRecognizer * longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [longPressGestureRecognizer setNumberOfTapsRequired: 1];
        [longPressGestureRecognizer setNumberOfTouchesRequired: 1];
        [longPressGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:longPressGestureRecognizer];
        
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
    }
    return(self);
}

+(Class)layerClass
{
    // Code
    return([CAEAGLLayer class]);
}

/*
-(void)drawRect:(CGRect)rect
{
    // Code
}
*/

-(void)drawView:(id)displayLink
{
    // Code
    // 1. Set current context again
    [EAGLContext setCurrentContext: eaglContext];
    // 2. Bind with the customFamebuffer again
    glBindFramebuffer(GL_FRAMEBUFFER, customFrameBuffer);
    // 3. Call our display
    [self display];
    // 4. Call our update
    [self myupdate];
    // 5. Bind with the color render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    // 5. Present color render buffer which will internally do double buffering
    [eaglContext presentRenderbuffer: GL_RENDERBUFFER];
}

// Override layoutSubviews method
-(void)layoutSubviews
{
    // Code
    // 1. Bind with the color render buffer again
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    // 2. Create color render buffer storage again as we did override it with depth buffer
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)[self layer]];
    // 3. Depth Render Buffer
    // 3.a. Find width of the color buffer
    GLint width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    // 3.b. Create and bind with depth buffer
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    // 3.c. Give storage to depth render buffer using usual OpenGLES Function
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    // 3.d. Attach the depth render buffer to customFrameBuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    // 4. Check status of customFramebuffer
    GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("[ERROR] OpenGLES: Framebuffer Creation Failed! Exiting...");
        [self uninitialize];
        [self release];
        exit(0);
    }
    // 5. Call our resize here
    [self resize:width :height];
    // 6. [RECOMMENDED] Call drawView here [WarmUp Resize Equivalent]
    [self drawView: displayLink];
}

// Start display link custom method which will be called by AppDelegate
-(void)startDisplayLink
{
    // Code
    if (isDisplayLink == NO) {
        // 1. Create display link
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        // 2. Set FramesPerSecond for this display link
        [displayLink setPreferredFramesPerSecond: framesPerSecond];
        // 3. Add this display link to out runLoop
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        // Set isDisplayLink to YES
        isDisplayLink = YES;
    }
}

// Stop display link custom method which will be called by AppDelegate
-(void)stopDisplayLink
{
    // Code
    if (isDisplayLink == YES) {
        // Remove displayLink from runLoop by invalidating it
        [displayLink invalidate];
        
        // Set isDisplayLink to NO
        isDisplayLink = NO;
    }
}

-(int)initialize
{
    //code
    [self printGLInfo];
    
    // shader from here
    // vertex shader

    // mat4 used in shader is inbuild in GLSL

    const GLchar* vertexShaderSourceCode =
        "#version 300 core" \
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
                printf("Vertex Shader Compilation Error Log : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        [self uninitialize];
        [self release];
        exit(0);
        
    }

    // fragment shader

    const GLchar* fragmentShaderSourceCode =
        "#version 300 core" \
        "\n" \
        "precision highp float;" \
        "precision highp int;" \
        "in vec4 oColor;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(1.0f , 1.0f , 1.0f , 1.0f);" \
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
                printf( "Fragment Shader Compilation Error Log : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        [self uninitialize];
        [self release];
        exit(0);
    }

    // create shader program

    shaderProgramObject = glCreateProgram();

    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");


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
                printf( "Shader Program linking Error Log : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        [self uninitialize];
        [self release];
        exit(0);
        
        
    }

    // get shader uniform location

    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject , "uMVPMatrix");


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

    // enabling depth

    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // clear color

    // set the clear colour of window to blue

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    perspectiveProjectionMatrix = vmath::mat4::identity();


    return(0);
    
}

-(void)printGLInfo
{
    printf("OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
    printf("OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
    printf("OpenGL Version : %s\n", glGetString(GL_VERSION));
    printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

-(void)resize:(int)width :(int)height
{
    // code
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

    // triangle
    {
        // transformation

        mat4 modelMatrix = mat4::identity();

        mat4 ViewMatrix = mat4::identity();
        mat4 modelViewProjectionMatrix = mat4::identity();

        modelMatrix = vmath::translate(0.0f, 0.0f, -8.0f);
        

//        pushMatrix(modelMatrix);
        [self pushMatrix:modelMatrix];
        
        // sun
        modelViewProjectionMatrix = perspectiveProjectionMatrix * ViewMatrix * modelMatrix;
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
        
        glBindVertexArray(gVao_sphere);
        glPolygonOffset(GL_FRONT_AND_BACK, GL_POLYGON_OFFSET_FILL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
        glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
        // *** unbind vao ***
        glBindVertexArray(0);

        modelMatrix = modelMatrix * vmath::rotate((GLfloat)Year, 0.0f, 1.0f, 0.0f);
        modelMatrix = modelMatrix * vmath::translate(2.2f, 0.0f, 0.0f);
        modelMatrix = modelMatrix * vmath::rotate((GLfloat)Day, 0.0f, 1.0f, 0.0f);

        // earth
        modelMatrix = modelMatrix * vmath::rotate(90.0f , 1.0f , 0.0f , 0.0f);
        modelViewProjectionMatrix = perspectiveProjectionMatrix * ViewMatrix * modelMatrix;
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
        
        glBindVertexArray(gVao_sphere);
        glPolygonOffset(GL_FRONT_AND_BACK, GL_LINES);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
        glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        // moon
//        pushMatrix(modelMatrix);
        [self pushMatrix:modelMatrix];

        modelMatrix = modelMatrix * vmath::translate(1.0f, 0.0f, 0.0f);
        modelMatrix = modelMatrix * vmath::rotate((GLfloat)Day, 0.0f, 1.0f, 0.0f);

        modelMatrix = modelMatrix * vmath::rotate(90.0f, 1.0f, 0.0f, 0.0f);
        modelViewProjectionMatrix = perspectiveProjectionMatrix * ViewMatrix * modelMatrix;
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

        glBindVertexArray(gVao_sphere);
        glPolygonOffset(GL_FRONT_AND_BACK, GL_POLYGON_OFFSET_FILL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
        glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        modelMatrix = [self popMatrix];
        
        modelMatrix = [self popMatrix];

//        modelMatrix = popMatrix();
//        modelMatrix = popMatrix();

    }

    glUseProgram(0);



}

-(void)myupdate
{
    // Code
    
}

    
-(void)uninitialize
{
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



    // Delete depth render buffer
    if (depthRenderBuffer) {
        glDeleteRenderbuffers(1, &depthRenderBuffer);
        depthRenderBuffer = 0;
    }
    // Delete color render buffer
    if (colorRenderBuffer) {
        glDeleteRenderbuffers(1, &colorRenderBuffer);
        colorRenderBuffer = 0;
    }
    // Delete customFrameBuffer
    if (customFrameBuffer) {
        glDeleteRenderbuffers(1, &customFrameBuffer);
        customFrameBuffer = 0;
    }
    
    // release eaglContext
    if (eaglContext && [EAGLContext currentContext] == eaglContext) {
        [EAGLContext setCurrentContext: nil];
        [eaglContext release];
        eaglContext = nil;
    }
}


-(BOOL)becomeFirstResponder
{
    return YES;
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
}

-(void)onSingleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // Code

}

-(void)onDoubleTap:(UITapGestureRecognizer *)gestureRecognizer
{


}

-(void)onSwipe:(UISwipeGestureRecognizer *)gestureRecognizer
{
    // Code
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
    // Code
}

-(void)dealloc
{
    // Code
    [self uninitialize];
    
    // Release displayLink
    if (displayLink) {
        [displayLink invalidate];
        [displayLink stop];
        [displayLink release];
        displayLink = nil;
    }
    
    [super dealloc];
}

@end
