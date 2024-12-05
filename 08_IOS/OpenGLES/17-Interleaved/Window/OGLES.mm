

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "OGLES.h"
#import "vmath.h"
using namespace vmath;

enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMALS, // lights
    AMC_ATTRIBUTE_TEXCOORDS
    
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
    
    GLuint vao_Cube;
    GLuint vbo_PCNT;
    GLfloat angle_cube;
    
    GLuint kundali_Texture;
    
    GLuint modelMatrixUniform ;
    GLuint projectionMatrixUniform ;
    GLuint viewMatrixUniform ;

    GLuint lightDiffuseUniform ; // for diffuse light
    GLuint materialDiffuseUniform ; // for diffuse material
    GLuint lightAmbientUniform ; // for ambient light
    GLuint materialAmbientUniform ; // for ambient material
    GLuint lightSpecularUniform ; // for specular light
    GLuint materialSpecularUniform ; // for specular material
    GLuint materialShinenessUniform ; // for shinneness material
    GLuint lightPositionUniform ;
    GLuint keyPressUniform ;

    BOOL bLightingEnabled;


    GLfloat lightAmbient[4] ;
    GLfloat lightDiffuse[4] ; // white diffuse light
    GLfloat lightSpecular[4] ;
    GLfloat lightPosition[4] ;

    GLfloat materialAmbient[4] ;
    GLfloat materialDiffuse[4] ;
    GLfloat materialSpecular[4] ;
    GLfloat materialShineness;

    
    GLuint textureSamplerUniform;
    
    mat4 perspectiveProjectionMatrix; // mat4 datatype is in vmath.h header file, that means 4X4 matrix
    
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
        "in vec3 aNormal;" \
        "in vec2 aTexCoord;" \
        "out vec2 oTexCoord;" \
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
        "oTexCoord = aTexCoord;" \
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
        "in vec2 oTexCoord;" \
        "uniform sampler2D uTextureSampler;" \
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
        "vec3 tex = vec3(texture(uTextureSampler , oTexCoord));" \
        "FragColor = vec4(tex * vec3(oColor) * Phong_ADS_Light , 1.0);" \
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
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORDS, "aTexCoord");

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

    modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
    lightAmbientUniform = glGetUniformLocation(shaderProgramObject, "uLightAmbient");
    lightDiffuseUniform = glGetUniformLocation(shaderProgramObject, "uLightDiffuse");
    lightSpecularUniform = glGetUniformLocation(shaderProgramObject, "uLightSpecular");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "uLightposition");
    materialAmbientUniform = glGetUniformLocation(shaderProgramObject, "uMaterialAmbient");
    materialDiffuseUniform = glGetUniformLocation(shaderProgramObject, "uMaterialDiffuse");
    materialSpecularUniform = glGetUniformLocation(shaderProgramObject, "uMaterialSpecular");
    materialShinenessUniform = glGetUniformLocation(shaderProgramObject, "uMaterialShineness");
    keyPressUniform = glGetUniformLocation(shaderProgramObject, "uKeyPress");
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");


    // declare position and color arrays

    GLfloat cube_PCNT[] =
    {
        // front
        // position                // color             // normals                // texcoords
        1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
        1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
                            
        // right
        // position                // color             // normals                // texcoords
        1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
                            
        // back
        // position                // color             // normals                // texcoords
        1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,     0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,     0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,     0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,     0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
                            
        // left
        // position                // color             // normals                // texcoords
        -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
                            
        // top
        // position                // color             // normals                // texcoords
        1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,     0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,     0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,     0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
        1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,     0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
                            
        // bottom
        // position                // color             // normals                // texcoords
        1.0f, -1.0f,  1.0f,    1.0f, 0.5f, 0.0f,     0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 0.5f, 0.0f,     0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,    1.0f, 0.5f, 0.0f,     0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,    1.0f, 0.5f, 0.0f,     0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
    };


    // cube

    {
        // VAO = vertex array object

        glGenVertexArrays(1, &vao_Cube);

        glBindVertexArray(vao_Cube);

        // VBO for position

        glGenBuffers(1, &vbo_PCNT);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_PCNT);

        glBufferData(GL_ARRAY_BUFFER, 24 * 11 * sizeof(float), cube_PCNT, GL_STATIC_DRAW); // nothing but size of cube_PCNT

        // position
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float) , (void *)(0 * sizeof(float)));
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

        // color
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float) , (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

        // normal
        glVertexAttribPointer(AMC_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float) , (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMALS);

        // texture
        glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float) , (void *)(9 * sizeof(float)));
        glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORDS);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // unbind with vao
        glBindVertexArray(0);

    }


    kundali_Texture = [self loadGLTexture:@"marble" :@"bmp"];

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

    materialDiffuse[0] = 1.0f;
    materialDiffuse[1] = 1.0f;
    materialDiffuse[2] = 1.0f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 1.0f;
    materialSpecular[1] = 1.0f;
    materialSpecular[2] = 1.0f;
    materialSpecular[3] = 1.0f;
    
    materialShineness =  50.0f;

    bLightingEnabled = TRUE;

    
    // set depth
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
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
    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObject);

    // cube
    {

        // transformation

        mat4 modelMatrix = mat4::identity();
        mat4 ViewMatrix = mat4::identity();

        mat4 translationMatrix = mat4::identity();
        translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);

        mat4 scaleMatrix = mat4::identity();
        scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);

        mat4 rotationMatrix1 = mat4::identity();
        rotationMatrix1 = vmath::rotate(angle_cube, 1.0f, 0.0f, 0.0f);

        mat4 rotationMatrix2 = mat4::identity();
        rotationMatrix2 = vmath::rotate(angle_cube, 0.0f, 1.0f, 0.0f);

        mat4 rotationMatrix3 = mat4::identity();
        rotationMatrix3 = vmath::rotate(angle_cube, 0.0f, 0.0f, 1.0f);

        mat4 rotationMatrix = rotationMatrix1 * rotationMatrix2 * rotationMatrix3;


        modelMatrix = translationMatrix * scaleMatrix * rotationMatrix ;

        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, ViewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        if (bLightingEnabled == TRUE)
        {
            glUniform1i(keyPressUniform , 1);
            glUniform3fv(lightAmbientUniform, 1, lightAmbient);
            glUniform3fv(lightDiffuseUniform, 1, lightDiffuse);
            glUniform3fv(lightSpecularUniform, 1, lightSpecular);
            glUniform3fv(materialAmbientUniform, 1, materialAmbient);
            glUniform3fv(materialDiffuseUniform, 1, materialDiffuse);
            glUniform3fv(materialSpecularUniform, 1, materialSpecular);
            glUniform4fv(lightPositionUniform, 1, lightPosition);
            glUniform1f(materialShinenessUniform, materialShineness);

        }
        else
        {
            glUniform1i(keyPressUniform, 0);
        }

        //for texture

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, kundali_Texture);
        glUniform1i(textureSamplerUniform, 0);

        glBindVertexArray(vao_Cube);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

        glBindVertexArray(0);

    }

    glUseProgram(0);


}

-(void)update
{
    // code
    // code
    angle_cube -= 1.5f;
    if (angle_cube <= 0.0f)
    {
        angle_cube += 360.0f;
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
