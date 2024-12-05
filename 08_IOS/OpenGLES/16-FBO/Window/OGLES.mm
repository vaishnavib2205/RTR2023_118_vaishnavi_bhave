

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "OGLES.h"
#import "vmath.h"
using namespace vmath;

enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_TEXCOORD
    
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
    [self update];
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

// load texture function

-(GLuint) loadGLTexture:(NSString *)szImageFileName :(NSString *)extension
{
    // code
    
    // step 1
    NSString *textureFileNameWithPath = [[NSBundle mainBundle]pathForResource:szImageFileName ofType:(extension)];
    
    // step 2
    UIImage *image = [[UIImage alloc]initWithContentsOfFile:textureFileNameWithPath];
    
    // step 3
    CGImageRef cgImage = [image CGImage];
    
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
        "in vec2 aTexCoord;" \
        "out vec2 oTexCoord;" \
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
                printf("Fragment Shader Compilation Error Log : %s \n", szInfoLog);
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
                printf("Shader Program linking Error Log : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        [self uninitialize];
        [self release];
        exit(0);
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
    // Code
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
